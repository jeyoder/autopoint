/*
 * propagator.c
 *
 *  Created on: Dec 26, 2017
 *      Author: james
 */

#include <stdbool.h>
#include <stddef.h>

#include "util.h"
#include "clock.h"
#include "sgp4_wrapper.h"

#include "propagator.h"

char TLE_LINE1[130] = "1 25544U 98067A   17360.63489756  .00001290  00000-0  26644-4 0  9993";
char TLE_LINE2[130] = "2 25544  51.6415 158.9361 0002587 294.1321 164.6117 15.54215205 91681";

/* Run the sgp4 lib in "catalog" mode, where it takes normal TLEs */
const char typerun = 'c';

/* Doesn't matter, since we're not in manual input mode */
const char typeinput = 'm';

/* 'a' for AFSPC, 'i' for "improved" */
const char opsmode = 'i';

/* WGS72 or WGS84 mode. WGS84, since we're using this stuff with GPS data. */
const gravconsttype whichconst = wgs84;

elsetrec current_sat;

void propagator_init() {

    uint32_t start = util_clock_us();

    float a, b, c;
    twoline2rv_wrapper(TLE_LINE1, TLE_LINE2, typerun, typeinput, opsmode, whichconst, &a, &b, &c, &current_sat);

    uint32_t init_end = util_clock_us();

    float r[3];
    float v[3];
    sgp4_wrapper(whichconst, &current_sat, 1.0, r, v);

    uint32_t prop_end = util_clock_us();

    uint32_t parse_time = init_end - start;
    uint32_t prop_time = prop_end - init_end;

    return;
}

void propagator_propagate(elsetrec *satrec) {

    float jd_epoch = satrec->jdsatepoch;

    float jd_test = clock_set_utc(0,0,0,0,0,0,0);

    /* sgp4_wrapper takes time, in minutes, from satellite TLE epoch (stored in satrec as a jd float)*/
    float r[3];
    float v[3];
    float tsince = 0;
    sgp4_wrapper(whichconst, satrec, tsince, r, v);
}

void propagator_test() {
    propagator_propagate(&current_sat);
}
