/*
 * laser_control.c
 *
 *  Created on: Dec 20, 2017
 *      Author: james
 */

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

#include "laser_control.h"

void laser_init() {
    /* laser switch, default to off */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 1<<5);
    GPIOPinWrite(GPIO_PORTB_BASE, 1<<5, 0<<5);

    /* laser enable, default to off */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 1<<2);
    GPIOPinWrite(GPIO_PORTA_BASE, 1<<2, 0<<2);
}
