/*
 * clock.c
 *
 *  Created on: Dec 26, 2017
 *      Author: james
 */

#include <math.h>

#include "clock.h"

/* Use a 64-bit wide timer to use as our "time since epoch" clock */
void clock_init() {

}

/* Returns the julian day value for a given time IN UTC:
 * year: like "2017"
 * month: range 1-12
 * day: 0-31ish or w/e
 */
static float jday(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t min, uint32_t sec, uint32_t msec) {
    return (
            367.0f * year
            - floorf((7.0f * (year + floorf((month + 9.0f) / 12.0f))) * 0.25f)
            + floorf( 275 * month / 9.0f )
            + day
            + 1721013.5f
            + ((sec / 60.0f + min) / 60.0f + hour) / 24.0f
            + msec * 1.15471e-8f
            );
}

float clock_now_jday(void);

float clock_set_utc(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t min, uint32_t sec, uint32_t msec) {
    return jday(2000, 1, 1, 18, 0, 0, 0);
}
