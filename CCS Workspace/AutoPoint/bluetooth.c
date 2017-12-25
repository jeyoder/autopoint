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

    /* reboot device so changes take effect... */
    bluetooth_reset();

    /* wait for it to boot */
    util_delay_us(1 * 1000 * 1000);
    return;
}
