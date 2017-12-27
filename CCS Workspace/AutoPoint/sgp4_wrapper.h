/*
 * sgp4unit_c.h
 *
 *  Created on: Dec 26, 2017
 *      Author: james
 */

/* C-compatible wrapper (but written in C++) for C++-only SGP4 functions. */

#ifndef SGP4_WRAPPER_H_
#define SGP4_WRAPPER_H_

/* Don't include the struct defs if they've already been included by C++ code */
#ifndef __cplusplus
// -------------------------- structure declarations ----------------------------
typedef enum
{
  wgs72old,
  wgs72,
  wgs84
} gravconsttype;

typedef struct elsetrec
{
  long int  satnum;
  int       epochyr, epochtynumrev;
  int       error;
  char      operationmode;
  char      init, method;

  /* Near Earth */
  int    isimp;
  float aycof  , con41  , cc1    , cc4      , cc5    , d2      , d3   , d4    ,
         delmo  , eta    , argpdot, omgcof   , sinmao , t       , t2cof, t3cof ,
         t4cof  , t5cof  , x1mth2 , x7thm1   , mdot   , nodedot, xlcof , xmcof ,
         nodecf;

  /* Deep Space */
  int    irez;
  float d2201  , d2211  , d3210  , d3222    , d4410  , d4422   , d5220 , d5232 ,
         d5421  , d5433  , dedt   , del1     , del2   , del3    , didt  , dmdt  ,
         dnodt  , domdt  , e3     , ee2      , peo    , pgho    , pho   , pinco ,
         plo    , se2    , se3    , sgh2     , sgh3   , sgh4    , sh2   , sh3   ,
         si2    , si3    , sl2    , sl3      , sl4    , gsto    , xfact , xgh2  ,
         xgh3   , xgh4   , xh2    , xh3      , xi2    , xi3     , xl2   , xl3   ,
         xl4    , xlamo  , zmol   , zmos     , atime  , xli     , xni;

  float a      , altp   , alta   , epochdays, jdsatepoch       , nddot , ndot  ,
         bstar  , rcse   , inclo  , nodeo    , ecco             , argpo , mo    ,
         no;
} elsetrec;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------- function declarations ----------------------------
bool sgp4init_wrapper
     (
       gravconsttype whichconst,  char opsmode,  const int satn,     const float epoch,
       const float xbstar,  const float xecco, const float xargpo,
       const float xinclo,  const float xmo,   const float xno,
       const float xnodeo,  elsetrec* satrec);

bool sgp4_wrapper
     (
       gravconsttype whichconst, elsetrec* satrec,  float tsince,
       float r[3],  float v[3]);

float  gstime_wrapper
        (
          float jdut1);

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
      float* j3oj2);

void twoline2rv_wrapper
     (
      char      longstr1[130], char longstr2[130],
      char      typerun,  char typeinput, char opsmode,
      gravconsttype       whichconst,
      float* startmfe, float* stopmfe, float* deltamin,
      elsetrec* satrec
     );

#ifdef __cplusplus
}
#endif

#endif /* SGP4_WRAPPER_H_ */
