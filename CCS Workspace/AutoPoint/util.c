/*
 * util.c
 *
 *  Created on: May 1, 2017
 *      Author: james
 */

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"

#include "util.h"

/* Initialize the util module.
 *
 * Sets up TIMER0 to count system clock cycles (ideally 80MHz)
 * in full width (32-bit) mode.
 */
void util_init(void) {

    /* Initialize the cycle (TIMER0) counter, straight off the system clock */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC_UP);
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerEnable(TIMER0_BASE, TIMER_A);
}

uint32_t debug_clock_cycles(void){
    return TimerValueGet(TIMER0_BASE, TIMER_A);
}

uint32_t util_clock_us(void) {
    return (TimerValueGet(TIMER0_BASE, TIMER_A) / 80); //divide 80 MHz timer by 80 to get 1 MHz
}

void util_delay_us(uint32_t delay) {
    uint32_t start = util_clock_us();

    while( util_clock_us() - start < delay);
}


