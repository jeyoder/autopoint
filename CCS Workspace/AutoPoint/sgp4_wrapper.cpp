/*
 * sgp4_wrapper.cpp
 *
 *  Created on: Dec 26, 2017
 *      Author: james
 */
#ifndef __cplusplus
#error "WAT"
#endif

#include "sgp4/sgp4unit.h"
#include "sgp4/sgp4io.h"
#include "sgp4_wrapper.h"

/* SGP4 C WRAPPER
 *
 * James Yoder
 * 12/26/2017
 *
 * These functions should compile as C++  with C linkages so the sgp4 C++ functions can be called by C code.
 *
 * NOTE 1:
 *      The sgp4 C++ code uses a buttload of stack space, and actually overflowed the stack by default on my TM4C123 (looks like 512 bytes default stack space)
 *
 *      20171226 8:12PM: bumping up stack space to 1024b
 *      20171226 8:14PM: not enough, bumped up to 2048b
 *      20171226 8:17PM: ..., bumped up to 4096b, seems to work
 *
 * NOTE 2:
 *      The TM4C doesn't have hardware support for 64-bit floating point, meaning all of sgp4unit's
 *      operations on floats fall back to software...
 */

/* Benchmarking Results
 *
 * CPU: TM4C123GH6PMI
 *
 * Compiler settings: -O0 -g
 * Floating point mode: double
 * Parse TLE:       6.161 ms
 * Propagate TLE:   2.317 ms
 *
 * Compiler settings: -O2 -g
 * Floating point mode: double
 * Parse TLE:       6.079 ms
 * Propagate TLE:   2.277 ms
 *
 * Compiler settings: -O0 -g  [use me]
 * Floating point mode: float
 * Parse TLE:       0.952 ms
 * Propagate TLE:   0.035 ms
 *
 * Compiler settings: -O2 -g
 * Floating point mode: float
 * Parse TLE:       0.992 ms
 * Propagate TLE:   0.052 ms  (lol what)
 *
 * Compiler settings: -O2 -mf=3 --symdebug:none
 * (even worse, lol)
 */

bool sgp4init_wrapper
     (
       gravconsttype whichconst,  char opsmode,  const int satn,     const float epoch,
       const float xbstar,  const float xecco, const float xargpo,
       const float xinclo,  const float xmo,   const float xno,
       const float xnodeo,  elsetrec* satrec
     ) {

    return sgp4init(whichconst,opsmode,satn,epoch,xbstar,xecco,xargpo,xinclo,xmo,xno,xnodeo,*satrec);

}

bool sgp4_wrapper
     (
       gravconsttype whichconst, elsetrec* satrec,  float tsince,
       float r[3],  float v[3]
     ) {

    return sgp4(whichconst,*satrec,tsince,r,v);

}

float gstime_wrapper(float jdut1)
{
    return gstime(jdut1);
}

void getgravconst_wrapper
     (
      gravconsttype whichconst,
      float* tumin,
      float* mu,
      float* radiusearthkm,
      float* xke,
      float* j2,
      float* j3,
      float* j4,
      float* j3oj2
     ) {

   getgravconst(whichconst, *tumin, *mu, *radiusearthkm, *xke, *j2, *j3, *j4, *j3oj2);
}

void twoline2rv_wrapper
     (
      char      longstr1[130], char longstr2[130],
      char      typerun,  char typeinput, char opsmode,
      gravconsttype       whichconst,
      float* startmfe, float* stopmfe, float* deltamin,
      elsetrec* satrec
     ) {

    twoline2rv(longstr1, longstr2, typerun, typeinput, opsmode, whichconst, *startmfe, *stopmfe, *deltamin, *satrec);
}
