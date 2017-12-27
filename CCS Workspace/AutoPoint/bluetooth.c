/*
 * bluetooth.c
 *
 *  Created on: Dec 20, 2017
 *      Author: james
 */

#include <stdbool.h>
#include <stdint.h>

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_uart.h"
#include "driverlib/pin_map.h"

#include "util.h"
#include "bluetooth.h"

/* BT module pin assignments
 *
 * UART RX: PB0 / U1RX
 * UART TX: PB1 / U1TX
 * UART RTS: PF0 / U1RTS (may not work)
 * UART CTS: PF1 / U1CTS (may not work)
 *
 * ~RESET:          PE0
 * 9.6K_BAUD:       PE3
 * CONNECTED:       PE4
 */

#define TX_BUFF_SIZE 1024
#define RX_BUFF_SIZE 1024

/* TX buffer, insert chars at head, retrieve at tail. */
char tx_buff[TX_BUFF_SIZE];
uint32_t tx_head = 0;
uint32_t tx_tail = 0;

/* RX buffer, insert chars at head, retrieve at tail. */
char rx_buff[RX_BUFF_SIZE];
uint32_t rx_head = 0;
uint32_t rx_tail = 0;

/* Packets guaranteed(tm) to be 255b or smaller */
char packet_buff[256];
uint32_t packet_ptr = 0;

/* Try to pop a char from the RX buffer.
 * Returns true if successful, false if empty
 */
static bool rx_pop(char* data) {
    if(rx_head == rx_tail) return false;
    *data = rx_buff[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFF_SIZE;
    return true;
}

/* Called periodically from the main loop. Pops any packets from the rx buffer and
 * passes them on to the packet handler.
 */

/* TODO: Fix  this to separate \n from out of data (could be more packet coming) */
void bluetooth_handle_packets() {

    packet_ptr = 0;

    while(true) {
        char data;
        bool ok = rx_pop(&data);
        if (!ok) break;
        packet_buff[packet_ptr++] = data;
        if (data == '\n') break;
        if (packet_ptr > 255) break;
    }

    if (packet_ptr > 0) {
        handle_new_packet(packet_buff, packet_ptr);
    }
}

static void bluetooth_uart_isr(void) {
    uint32_t cause = UARTIntStatus(UART1_BASE, true);

    if (cause & UART_INT_TX) {
        /* TX complete interrupt. Send more data. */
        UARTIntClear(UART1_BASE, UART_INT_TX);

        /* fill with 16 chars or until FIFO full, whichever comes first */
        for (int i = 0; i < 16; i++) {

            if (tx_tail == tx_head) {
                /* no more data to send */
                break;
            }

            char data = tx_buff[tx_tail];
            bool ok = UARTCharPutNonBlocking(UART1_BASE, data);

            if (ok) {
                tx_tail = (tx_tail + 1) % TX_BUFF_SIZE;
            } else {
                break;
            }
        }
    }

    if (cause & (UART_INT_RX | UART_INT_RT)) {
        /* RX interrupt, we got data. */
        UARTIntClear(UART1_BASE, UART_INT_RX | UART_INT_RT);

        while(true) {
            /* Pull data out of the hardware FIFO until there isn't any more */
            int32_t data = UARTCharGetNonBlocking(UART1_BASE);

            if (data == -1) {
                /* FIFO is empty, stop. */
                break;
            } else {
                rx_buff[rx_head] = data;
                rx_head = (rx_head + 1) & RX_BUFF_SIZE;
            }
        }
    }
}

/* Send a LOW pulse to the RN42 ~RESET pin */
static void bluetooth_reset() {
    GPIOPinWrite(GPIO_PORTE_BASE, (1<<0), 0);
    util_delay_us(250 * 1000);
    GPIOPinWrite(GPIO_PORTE_BASE, (1<<0), (1<<0));
}

static void bluetooth_force_baud(bool force) {
    if(force) {
        GPIOPinWrite(GPIO_PORTE_BASE, (1<<3), (1<<3));
    } else {
        GPIOPinWrite(GPIO_PORTE_BASE, (1<<3), 0);
    }
}

static void bluetooth_send_sync(const char* data) {
    const char* ptr = data;

    while (*ptr != 0) {
        UARTCharPut(UART1_BASE, *ptr);
        ptr++;
    }
}

static void bluetooth_uart_clear() {
    while(UARTCharGetNonBlocking(UART1_BASE) != -1);
}

static uint32_t bluetooth_read_line(char* buf, uint32_t size) {

    /* wait for something to happen... */

    uint32_t i = 0;

    while(i < size) {
       int c = UARTCharGet(UART1_BASE);
       if (c == '\r' || c == '\n') {
           buf[i] = 0;
           bluetooth_uart_clear();
           return i;
       } else {
           buf[i] = c;
           i++;
       }
    }
}

static bool bluetooth_get_ack() {
    char buf[8];
    uint32_t len = bluetooth_read_line(buf, 8);
    if (len != 3) goto fail;
    if (buf[0] != 'A') goto fail;
    if (buf[1] != 'O') goto fail;
    if (buf[2] != 'K') goto fail;
    return true;

    fail:
    while(true);
}

void bluetooth_init(void) {

    /* initialize UART comm pins */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));

    /* Configure GPIO pins */
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, (1<<0) | (1<<3));
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, (1<<4));

    /* Configure UART pins: drive settings */
    GPIOPinTypeUART(GPIO_PORTB_BASE, (1<<0) | (1<<1));
    GPIOPinTypeUART(GPIO_PORTF_BASE, (1<<0) | (1<<1));

    /* Configure UART pins: set pinmux */
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinConfigure(GPIO_PF0_U1RTS);
    GPIOPinConfigure(GPIO_PF1_U1CTS);

    /* Configure comm UART:
     * UART 1
     * 80 MHz system clock
     * 9600 baud
     * 8, N, 1 */
    UARTConfigSetExpClk(UART1_BASE, 80000000, 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE);
    UARTEnable(UART1_BASE);

    /* Reset the RN42 bluetooth module, and force a known baud rate. */
    bluetooth_force_baud(true);
    bluetooth_reset();

    util_delay_us(1 * 1000 * 1000);

    /* Send configuration strings to the RN42. Note settings starting with "S" */
    char buf[100];

    bluetooth_uart_clear();
    bluetooth_send_sync("$$$");
    bluetooth_read_line(buf, 100);
    util_delay_us(100 * 1000);

    /* set security PIN to 69420 */
    bluetooth_uart_clear();
    bluetooth_send_sync("SP,69420\r\n");
    bluetooth_get_ack();

    /* full power */
    util_delay_us(100 * 1000);
    bluetooth_uart_clear();
    bluetooth_send_sync("SY,0010\r\n");
    bluetooth_get_ack();

    /* serial port profile */
    util_delay_us(100 * 1000);
    bluetooth_uart_clear();
    bluetooth_send_sync("S~,0\r\n");
    bluetooth_get_ack();

    /* set name */
    util_delay_us(100 * 1000);
    bluetooth_uart_clear();
    bluetooth_send_sync("SN,laser\r\n");
    bluetooth_get_ack();

    /* set UUID for android shit */
    util_delay_us(100 * 1000);
    bluetooth_uart_clear();
    bluetooth_send_sync("SE,0000110100001000800000805F9B34FB\r\n");
    bluetooth_get_ack();

    /* reboot device so changes take effect... */
    bluetooth_reset();

    /* Enable interrupts: RX fifo, RX timeout, TX complete */
    UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX1_8, UART_FIFO_RX6_8);
    UARTTxIntModeSet(UART1_BASE, UART_TXINT_MODE_EOT);
    UARTIntRegister(UART1_BASE, bluetooth_uart_isr);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX);
    return;
}
