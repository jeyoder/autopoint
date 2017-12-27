/*
 *  MODIFIED James Yoder 12/26/2017
 *
 *  Changed all math to 32-bit floats
 *
 */


/*     ----------------------------------------------------------------
*
*                               sgp4ext.cpp
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
*               7 may 08  david vallado
*                           fix sgn
*    changes :
*               2 apr 07  david vallado
*                           fix jday floor and str lengths
*                           updates for constants
*              14 aug 06  david vallado
*                           original baseline
*       ----------------------------------------------------------------      */

#include "sgp4ext.h"


float  sgn
        (
          float x
        )
   {
     if (x < 0.0)
       {
          return -1.0f;
       }
       else
       {
          return 1.0f;
       }

   }  // end sgn

/* -----------------------------------------------------------------------------
*
*                           function mag
*
*  this procedure finds the magnitude of a vector.  the tolerance is set to
*    0.000001, thus the 1.0e-12 for the squared test of underflows.
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    vec         - vector
*
*  outputs       :
*    vec         - answer stored in fourth component
*
*  locals        :
*    none.
*
*  coupling      :
*    none.
* --------------------------------------------------------------------------- */

float  mag
        (
          float x[3]
        )
   {
     return sqrtf(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
   }  // end mag

/* -----------------------------------------------------------------------------
*
*                           procedure cross
*
*  this procedure crosses two vectors.
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    vec1        - vector number 1
*    vec2        - vector number 2
*
*  outputs       :
*    outvec      - vector result of a x b
*
*  locals        :
*    none.
*
*  coupling      :
*    mag           magnitude of a vector
 ---------------------------------------------------------------------------- */

void    cross
        (
          float vec1[3], float vec2[3], float outvec[3]
        )
   {
     outvec[0]= vec1[1]*vec2[2] - vec1[2]*vec2[1];
     outvec[1]= vec1[2]*vec2[0] - vec1[0]*vec2[2];
     outvec[2]= vec1[0]*vec2[1] - vec1[1]*vec2[0];
   }  // end cross


/* -----------------------------------------------------------------------------
*
*                           function dot
*
*  this function finds the dot product of two vectors.
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    vec1        - vector number 1
*    vec2        - vector number 2
*
*  outputs       :
*    dot         - result
*
*  locals        :
*    none.
*
*  coupling      :
*    none.
*
* --------------------------------------------------------------------------- */

float  dot
        (
          float x[3], float y[3]
        )
   {
     return (x[0]*y[0] + x[1]*y[1] + x[2]*y[2]);
   }  // end dot

/* -----------------------------------------------------------------------------
*
*                           procedure angle
*
*  this procedure calculates the angle between two vectors.  the output is
*    set to 999999.1 to indicate an undefined value.  be sure to check for
*    this at the output phase.
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    vec1        - vector number 1
*    vec2        - vector number 2
*
*  outputs       :
*    theta       - angle between the two vectors  -pi to pi
*
*  locals        :
*    temp        - temporary real variable
*
*  coupling      :
*    dot           dot product of two vectors
* --------------------------------------------------------------------------- */

float  angle
        (
          float vec1[3],
          float vec2[3]
        )
   {
     float small, undefined, magv1, magv2, temp;
     small     = 0.00000001f;
     undefined = 999999.1f;

     magv1 = mag(vec1);
     magv2 = mag(vec2);

     if (magv1*magv2 > small*small)
       {
         temp= dot(vec1,vec2) / (magv1*magv2);
         if (fabsf( temp ) > 1.0f)
             temp= sgn(temp) * 1.0f;
         return acosf( temp );
       }
       else
         return undefined;
   }  // end angle


/* -----------------------------------------------------------------------------
*
*                           function asinh
*
*  this function evaluates the inverse hyperbolic sine function.
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    xval        - angle value                                  any real
*
*  outputs       :
*    arcsinh     - result                                       any real
*
*  locals        :
*    none.
*
*  coupling      :
*    none.
*
* --------------------------------------------------------------------------- */

float  asinh2
        (
          float xval
        )
   {
     return logf( xval + sqrtf( xval*xval + 1.0f ) );
   }  // end asinh


/* -----------------------------------------------------------------------------
*
*                           function newtonnu
*
*  this function solves keplers equation when the true anomaly is known.
*    the mean and eccentric, parabolic, or hyperbolic anomaly is also found.
*    the parabolic limit at 168ø is arbitrary. the hyperbolic anomaly is also
*    limited. the hyperbolic sine is used because it's not float valued.
*
*  author        : david vallado                  719-573-2600   27 may 2002
*
*  revisions
*    vallado     - fix small                                     24 sep 2002
*
*  inputs          description                    range / units
*    ecc         - eccentricity                   0.0  to
*    nu          - true anomaly                   -2pi to 2pi rad
*
*  outputs       :
*    e0          - eccentric anomaly              0.0  to 2pi rad       153.02 ø
*    m           - mean anomaly                   0.0  to 2pi rad       151.7425 ø
*
*  locals        :
*    e1          - eccentric anomaly, next value  rad
*    sine        - sine of e
*    cose        - cosine of e
*    ktr         - index
*
*  coupling      :
*    asinh       - arc hyperbolic sine
*
*  references    :
*    vallado       2007, 85, alg 5
* --------------------------------------------------------------------------- */

void newtonnu
     (
       float ecc, float nu,
       float& e0, float& m
     )
     {
       float small, sine, cose;

     // ---------------------  implementation   ---------------------
     e0= 999999.9f;
     m = 999999.9f;
     small = 0.00000001f;

     // --------------------------- circular ------------------------
     if ( fabsf( ecc ) < small  )
       {
         m = nu;
         e0= nu;
       }
       else
         // ---------------------- elliptical -----------------------
         if ( ecc < 1.0f-small  )
           {
             sine= ( sqrtf( 1.0f -ecc*ecc ) * sinf(nu) ) / ( 1.0f +ecc*cosf(nu) );
             cose= ( ecc + cosf(nu) ) / ( 1.0f  + ecc*cosf(nu) );
             e0  = atan2f( sine,cose );
             m   = e0 - ecc*sinf(e0);
           }
           else
             // -------------------- hyperbolic  --------------------
             if ( ecc > 1.0f + small  )
               {
                 if ((ecc > 1.0f ) && (fabsf(nu)+0.00001f < pi-acosf(1.0f /ecc)))
                   {
                     sine= ( sqrtf( ecc*ecc-1.0f  ) * sinf(nu) ) / ( 1.0f  + ecc*cosf(nu) );
                     e0  = asinh2( sine );
                     m   = ecc*sinhf(e0) - e0;
                   }
                }
               else
                 // ----------------- parabolic ---------------------
                 if ( fabsf(nu) < 168.0f*pi/180.0f  )
                   {
                     e0= tanf( nu*0.5f  );
                     m = e0 + (e0*e0*e0)/3.0f;
                   }

     if ( ecc < 1.0f  )
       {
         m = fmodf( m,2.0f *pi );
         if ( m < 0.0  )
             m = m + 2.0f *pi;
         e0 = fmodf( e0,2.0f *pi );
       }
   }  // end newtonnu


/* -----------------------------------------------------------------------------
*
*                           function rv2coe
*
*  this function finds the classical orbital elements given the geocentric
*    equatorial position and velocity vectors.
*
*  author        : david vallado                  719-573-2600   21 jun 2002
*
*  revisions
*    vallado     - fix special cases                              5 sep 2002
*    vallado     - delete extra check in inclination code        16 oct 2002
*    vallado     - add constant file use                         29 jun 2003
*    vallado     - add mu                                         2 apr 2007
*
*  inputs          description                    range / units
*    r           - ijk position vector            km
*    v           - ijk velocity vector            km / s
*    mu          - gravitational parameter        km3 / s2
*
*  outputs       :
*    p           - semilatus rectum               km
*    a           - semimajor axis                 km
*    ecc         - eccentricity
*    incl        - inclination                    0.0  to pi rad
*    omega       - longitude of ascending node    0.0  to 2pi rad
*    argp        - argument of perigee            0.0  to 2pi rad
*    nu          - true anomaly                   0.0  to 2pi rad
*    m           - mean anomaly                   0.0  to 2pi rad
*    arglat      - argument of latitude      (ci) 0.0  to 2pi rad
*    truelon     - true longitude            (ce) 0.0  to 2pi rad
*    lonper      - longitude of periapsis    (ee) 0.0  to 2pi rad
*
*  locals        :
*    hbar        - angular momentum h vector      km2 / s
*    ebar        - eccentricity     e vector
*    nbar        - line of nodes    n vector
*    c1          - v**2 - u/r
*    rdotv       - r dot v
*    hk          - hk unit vector
*    sme         - specfic mechanical energy      km2 / s2
*    i           - index
*    e           - eccentric, parabolic,
*                  hyperbolic anomaly             rad
*    temp        - temporary variable
*    typeorbit   - type of orbit                  ee, ei, ce, ci
*
*  coupling      :
*    mag         - magnitude of a vector
*    cross       - cross product of two vectors
*    angle       - find the angle between two vectors
*    newtonnu    - find the mean anomaly
*
*  references    :
*    vallado       2007, 126, alg 9, ex 2-5
* --------------------------------------------------------------------------- */

void rv2coe
     (
       float r[3], float v[3], float mu,
       float& p, float& a, float& ecc, float& incl, float& omega, float& argp,
       float& nu, float& m, float& arglat, float& truelon, float& lonper
     )
     {
       float undefined, small, hbar[3], nbar[3], magr, magv, magn, ebar[3], sme,
              rdotv, infinite, temp, c1, hk, twopi, magh, halfpi, e;

       int i;
       char typeorbit[3];

     twopi  = 2.0 * pi;
     halfpi = 0.5 * pi;
     small  = 0.00000001;
     undefined = 999999.1;
     infinite  = 999999.9;

     // -------------------------  implementation   -----------------
     magr = mag( r );
     magv = mag( v );

     // ------------------  find h n and e vectors   ----------------
     cross( r,v, hbar );
     magh = mag( hbar );
     if ( magh > small )
       {
         nbar[0]= -hbar[1];
         nbar[1]=  hbar[0];
         nbar[2]=   0.0;
         magn = mag( nbar );
         c1 = magv*magv - mu /magr;
         rdotv = dot( r,v );
         for (i= 0; i <= 2; i++)
             ebar[i]= (c1*r[i] - rdotv*v[i])/mu;
         ecc = mag( ebar );

         // ------------  find a e and semi-latus rectum   ----------
         sme= ( magv*magv*0.5f  ) - ( mu /magr );
         if ( fabsf( sme ) > small )
             a= -mu  / (2.0f *sme);
           else
             a= infinite;
         p = magh*magh/mu;

         // -----------------  find inclination   -------------------
         hk= hbar[2]/magh;
         incl= acosf( hk );

         // --------  determine type of orbit for later use  --------
         // ------ elliptical, parabolic, hyperbolic inclined -------
         strcpy(typeorbit,"ei");
         if ( ecc < small )
           {
             // ----------------  circular equatorial ---------------
             if  ((incl<small) | (fabsf(incl-pi)<small))
                 strcpy(typeorbit,"ce");
               else
                 // --------------  circular inclined ---------------
                 strcpy(typeorbit,"ci");
           }
           else
           {
             // - elliptical, parabolic, hyperbolic equatorial --
             if  ((incl<small) | (fabsf(incl-pi)<small))
                 strcpy(typeorbit,"ee");
           }

         // ----------  find longitude of ascending node ------------
         if ( magn > small )
           {
             temp= nbar[0] / magn;
             if ( fabsf(temp) > 1.0f  )
                 temp= sgn(temp);
             omega= acosf( temp );
             if ( nbar[1] < 0.0  )
                 omega= twopi - omega;
           }
           else
             omega= undefined;

         // ---------------- find argument of perigee ---------------
         if ( strcmp(typeorbit,"ei") == 0 )
           {
             argp = angle( nbar,ebar);
             if ( ebar[2] < 0.0  )
                 argp= twopi - argp;
           }
           else
             argp= undefined;

         // ------------  find true anomaly at epoch    -------------
         if ( typeorbit[0] == 'e' )
           {
             nu =  angle( ebar,r);
             if ( rdotv < 0.0  )
                 nu= twopi - nu;
           }
           else
             nu= undefined;

         // ----  find argument of latitude - circular inclined -----
         if ( strcmp(typeorbit,"ci") == 0 )
           {
             arglat = angle( nbar,r );
             if ( r[2] < 0.0  )
                 arglat= twopi - arglat;
             m = arglat;
           }
           else
             arglat= undefined;

         // -- find longitude of perigee - elliptical equatorial ----
         if  (( ecc>small ) && (strcmp(typeorbit,"ee") == 0))
           {
             temp= ebar[0]/ecc;
             if ( fabsf(temp) > 1.0f  )
                 temp= sgn(temp);
             lonper= acosf( temp );
             if ( ebar[1] < 0.0  )
                 lonper= twopi - lonper;
             if ( incl > halfpi )
                 lonper= twopi - lonper;
           }
           else
             lonper= undefined;

         // -------- find true longitude - circular equatorial ------
         if  (( magr>small ) && ( strcmp(typeorbit,"ce") == 0 ))
           {
             temp= r[0]/magr;
             if ( fabsf(temp) > 1.0f  )
                 temp= sgn(temp);
             truelon= acosf( temp );
             if ( r[1] < 0.0  )
                 truelon= twopi - truelon;
             if ( incl > halfpi )
                 truelon= twopi - truelon;
             m = truelon;
           }
           else
             truelon= undefined;

         // ------------ find mean anomaly for all orbits -----------
         if ( typeorbit[0] == 'e' )
             newtonnu(ecc,nu,  e, m);
     }
      else
     {
        p    = undefined;
        a    = undefined;
        ecc  = undefined;
        incl = undefined;
        omega= undefined;
        argp = undefined;
        nu   = undefined;
        m    = undefined;
        arglat = undefined;
        truelon= undefined;
        lonper = undefined;
     }
   }  // end rv2coe

/* -----------------------------------------------------------------------------
*
*                           procedure jday
*
*  this procedure finds the julian date given the year, month, day, and time.
*    the julian date is defined by each elapsed day since noon, jan 1, 4713 bc.
*
*  algorithm     : calculate the answer in one step for efficiency
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    year        - year                           1900 .. 2100
*    mon         - month                          1 .. 12
*    day         - day                            1 .. 28,29,30,31
*    hr          - universal time hour            0 .. 23
*    min         - universal time min             0 .. 59
*    sec         - universal time sec             0.0 .. 59.999
*
*  outputs       :
*    jd          - julian date                    days from 4713 bc
*
*  locals        :
*    none.
*
*  coupling      :
*    none.
*
*  references    :
*    vallado       2007, 189, alg 14, ex 3-14
*
* --------------------------------------------------------------------------- */

void    jday
        (
          int year, int mon, int day, int hr, int minute, float sec,
          float& jd
        )
   {
     jd = 367.0f * year -
          floorf((7 * (year + floorf((mon + 9) / 12.0f))) * 0.25f) +
          floorf( 275 * mon / 9.0f ) +
          day + 1721013.5f +
          ((sec / 60.0f + minute) / 60.0f + hr) / 24.0f;  // ut in days
          // - 0.5*sgn(100.0*year + mon - 190002.5) + 0.5;
   }  // end jday


/* -----------------------------------------------------------------------------
*
*                           procedure days2mdhms
*
*  this procedure converts the day of the year, days, to the equivalent month
*    day, hour, minute and second.
*
*  algorithm     : set up array for the number of days per month
*                  find leap year - use 1900 because 2000 is a leap year
*                  loop through a temp value while the value is < the days
*                  perform int conversions to the correct day and month
*                  convert remainder into h m s using type conversions
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    year        - year                           1900 .. 2100
*    days        - julian day of the year         0.0  .. 366.0
*
*  outputs       :
*    mon         - month                          1 .. 12
*    day         - day                            1 .. 28,29,30,31
*    hr          - hour                           0 .. 23
*    min         - minute                         0 .. 59
*    sec         - second                         0.0 .. 59.999
*
*  locals        :
*    dayofyr     - day of year
*    temp        - temporary extended values
*    inttemp     - temporary int value
*    i           - index
*    lmonth[12]  - int array containing the number of days per month
*
*  coupling      :
*    none.
* --------------------------------------------------------------------------- */

void    days2mdhms
        (
          int year, float days,
          int& mon, int& day, int& hr, int& minute, float& sec
        )
   {
     int i, inttemp, dayofyr;
     float    temp;
     int lmonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

     dayofyr = (int)floorf(days);
     /* ----------------- find month and day of month ---------------- */
     if ( (year % 4) == 0 )
       lmonth[1] = 29;

     i = 1;
     inttemp = 0;
     while ((dayofyr > inttemp + lmonth[i-1]) && (i < 12))
     {
       inttemp = inttemp + lmonth[i-1];
       i++;
     }
     mon = i;
     day = dayofyr - inttemp;

     /* ----------------- find hours minutes and seconds ------------- */
     temp = (days - dayofyr) * 24.0;
     hr   = (int)floorf(temp);
     temp = (temp - hr) * 60.0;
     minute  = (int)floorf(temp);
     sec  = (temp - minute) * 60.0;
   }  // end days2mdhms

/* -----------------------------------------------------------------------------
*
*                           procedure invjday
*
*  this procedure finds the year, month, day, hour, minute and second
*  given the julian date. tu can be ut1, tdt, tdb, etc.
*
*  algorithm     : set up starting values
*                  find leap year - use 1900 because 2000 is a leap year
*                  find the elapsed days through the year in a loop
*                  call routine to find each individual value
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    jd          - julian date                    days from 4713 bc
*
*  outputs       :
*    year        - year                           1900 .. 2100
*    mon         - month                          1 .. 12
*    day         - day                            1 .. 28,29,30,31
*    hr          - hour                           0 .. 23
*    min         - minute                         0 .. 59
*    sec         - second                         0.0 .. 59.999
*
*  locals        :
*    days        - day of year plus fractional
*                  portion of a day               days
*    tu          - julian centuries from 0 h
*                  jan 0, 1900
*    temp        - temporary float values
*    leapyrs     - number of leap years from 1900
*
*  coupling      :
*    days2mdhms  - finds month, day, hour, minute and second given days and year
*
*  references    :
*    vallado       2007, 208, alg 22, ex 3-13
* --------------------------------------------------------------------------- */

void    invjday
        (
          float jd,
          int& year, int& mon, int& day,
          int& hr, int& minute, float& sec
        )
   {
     int leapyrs;
     float    days, tu, temp;

     /* --------------- find year and days of the year --------------- */
     temp    = jd - 2415019.5;
     tu      = temp / 365.25;
     year    = 1900 + (int)floorf(tu);
     leapyrs = (int)floorf((year - 1901) * 0.25f);

     // optional nudge by 8.64x10-7 sec to get even outputs
     days    = temp - ((year - 1900) * 365.0f + leapyrs) + 0.00000000001f;

     /* ------------ check for case of beginning of a year ----------- */
     if (days < 1.0f)
       {
         year    = year - 1;
         leapyrs = (int)floorf((year - 1901) * 0.25f);
         days    = temp - ((year - 1900) * 365.0f + leapyrs);
       }

     /* ----------------- find remaing data  ------------------------- */
     days2mdhms(year, days, mon, day, hr, minute, sec);
     sec = sec - 0.00000086400f;
   }  // end invjday





