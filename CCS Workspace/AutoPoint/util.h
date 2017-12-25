/*
 * util.h
 *
 *  Created on: May 1, 2017
 *      Author: james
 */

#ifndef COMMONE_UTIL_H_
#define COMMONE_UTIL_H_

#include <stdint.h>

void util_init(void);
uint32_t util_clock_us(void);
void util_delay_us(uint32_t delay);



#endif /* COMMON_UTIL_H_ */
