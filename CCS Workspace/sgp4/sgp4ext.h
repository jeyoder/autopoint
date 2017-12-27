#ifndef _sgp4ext_
#define _sgp4ext_

/*
 *  MODIFIED James Yoder 12/26/2017
 *
 *  Changed all math to 32-bit floats
 *
 */

/*     ----------------------------------------------------------------
*
*                                 sgp4ext.h
*
*    this file contains extra routines needed for the main test program for sgp4.
*    these routines are derived from the astro libraries.
*
*                            companion code for
*               fundamentals of astrodynamics and applications
*                                    2007
*                              by david vallado
*
*       (w) 719-573-2600, email dvallado@agi.com
*
*    current :
*              20 apr 07  david vallado
*                           misc documentation updates
*    changes :
*              14 aug 06  david vallado
*                           original baseline
*       ----------------------------------------------------------------      */

#include <string.h>
#include <math.h>

#include "sgp4unit.h"


// ------------------------- function declarations -------------------------

float  sgn
        (
          float x
        );

float  mag
        (
          float x[3]
        );

void    cross
        (
          float vec1[3], float vec2[3], float outvec[3]
        );

float  dot
        (
          float x[3], float y[3]
        );

float  angle
        (
          float vec1[3],
          float vec2[3]
        );

void    newtonnu
        (
          float ecc, float nu,
          float& e0, float& m
        );

float  asinh
        (
          float xval
        );

void    rv2coe
        (
          float r[3], float v[3], float mu,
          float& p, float& a, float& ecc, float& incl, float& omega, float& argp,
          float& nu, float& m, float& arglat, float& truelon, float& lonper
        );

void    jday
        (
          int year, int mon, int day, int hr, int minute, float sec,
          float& jd
        );

void    days2mdhms
        (
          int year, float days,
          int& mon, int& day, int& hr, int& minute, float& sec
        );

void    invjday
        (
          float jd,
          int& year, int& mon, int& day,
          int& hr, int& minute, float& sec
        );

#endif

