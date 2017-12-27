/*
 * main.c
 */

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

#include "laser_control.h"
#include "bluetooth.h"

int main(void) {
    /* set system clock to 80MHz with 16MHz external crystal */
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    /* make sure it actually happened */
    uint32_t clock_freq = SysCtlClockGet();
    if (clock_freq != 80000000) {
        while(1);
    }

    util_init();
    laser_init();
    bluetooth_init();
    propagator_init();

    propagator_test();

    /* main loop */
    while(1) {

    }
}
