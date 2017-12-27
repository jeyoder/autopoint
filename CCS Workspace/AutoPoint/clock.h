/*
 * clock.h
 *
 *  Created on: Dec 26, 2017
 *      Author: james
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

float clock_now_jday(void);

float clock_set_utc(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t min, uint32_t sec, uint32_t msec);

#endif /* CLOCK_H_ */
