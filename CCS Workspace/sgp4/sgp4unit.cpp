/*
 *  MODIFIED James Yoder 12/26/2017
 *
 *  Changed all math to 32-bit floats
 *
 */

/*     ----------------------------------------------------------------
*
*                               sgp4unit.cpp
*
*    this file contains the sgp4 procedures for analytical propagation
*    of a satellite. the code was originally released in the 1980 and 1986
*    spacetrack papers. a detailed discussion of the theory and history
*    may be found in the 2006 aiaa paper by vallado, crawford, hujsak,
*    and kelso.
*
*                            companion code for
*               fundamentals of astrodynamics and applications
*                                    2007
*                              by david vallado
*
*       (w) 719-573-2600, email dvallado@agi.com
*
*    current :
*              30 Aug 10  david vallado
*                           delete unused variables in initl
*                           replace pow inetger 2, 3 with multiplies for speed
*    changes :
*               3 Nov 08  david vallado
*                           put returns in for error codes
*              29 sep 08  david vallado
*                           fix atime for faster operation in dspace
*                           add operationmode for afspc (a) or improved (i)
*                           performance mode
*              16 jun 08  david vallado
*                           update small eccentricity check
*              16 nov 07  david vallado
*                           misc fixes for better compliance
*              20 apr 07  david vallado
*                           misc fixes for constants
*              11 aug 06  david vallado
*                           chg lyddane choice back to strn3, constants, misc doc
*              15 dec 05  david vallado
*                           misc fixes
*              26 jul 05  david vallado
*                           fixes for paper
*                           note that each fix is preceded by a
*                           comment with "sgp4fix" and an explanation of
*                           what was changed
*              10 aug 04  david vallado
*                           2nd printing baseline working
*              14 may 01  david vallado
*                           2nd edition baseline
*                     80  norad
*                           original baseline
*       ----------------------------------------------------------------      */

#include "sgp4unit.h"

const char help = 'n';
FILE *dbgfile;


/* ----------- local functions - only ever used internally by sgp4 ---------- */
static void dpper
     (
       float e3,     float ee2,    float peo,     float pgho,   float pho,
       float pinco,  float plo,    float se2,     float se3,    float sgh2,
       float sgh3,   float sgh4,   float sh2,     float sh3,    float si2,
       float si3,    float sl2,    float sl3,     float sl4,    float t,
       float xgh2,   float xgh3,   float xgh4,    float xh2,    float xh3,
       float xi2,    float xi3,    float xl2,     float xl3,    float xl4,
       float zmol,   float zmos,   float inclo,
       char init,
       float& ep,    float& inclp, float& nodep,  float& argpp, float& mp,
       char opsmode
     );

static void dscom
     (
       float epoch,  float ep,     float argpp,   float tc,     float inclp,
       float nodep,  float np,
       float& snodm, float& cnodm, float& sinim,  float& cosim, float& sinomm,
       float& cosomm,float& day,   float& e3,     float& ee2,   float& em,
       float& emsq,  float& gam,   float& peo,    float& pgho,  float& pho,
       float& pinco, float& plo,   float& rtemsq, float& se2,   float& se3,
       float& sgh2,  float& sgh3,  float& sgh4,   float& sh2,   float& sh3,
       float& si2,   float& si3,   float& sl2,    float& sl3,   float& sl4,
       float& s1,    float& s2,    float& s3,     float& s4,    float& s5,
       float& s6,    float& s7,    float& ss1,    float& ss2,   float& ss3,
       float& ss4,   float& ss5,   float& ss6,    float& ss7,   float& sz1,
       float& sz2,   float& sz3,   float& sz11,   float& sz12,  float& sz13,
       float& sz21,  float& sz22,  float& sz23,   float& sz31,  float& sz32,
       float& sz33,  float& xgh2,  float& xgh3,   float& xgh4,  float& xh2,
       float& xh3,   float& xi2,   float& xi3,    float& xl2,   float& xl3,
       float& xl4,   float& nm,    float& z1,     float& z2,    float& z3,
       float& z11,   float& z12,   float& z13,    float& z21,   float& z22,
       float& z23,   float& z31,   float& z32,    float& z33,   float& zmol,
       float& zmos
     );

static void dsinit
     (
       gravconsttype whichconst,
       float cosim,  float emsq,   float argpo,   float s1,     float s2,
       float s3,     float s4,     float s5,      float sinim,  float ss1,
       float ss2,    float ss3,    float ss4,     float ss5,    float sz1,
       float sz3,    float sz11,   float sz13,    float sz21,   float sz23,
       float sz31,   float sz33,   float t,       float tc,     float gsto,
       float mo,     float mdot,   float no,      float nodeo,  float nodedot,
       float xpidot, float z1,     float z3,      float z11,    float z13,
       float z21,    float z23,    float z31,     float z33,    float ecco,
       float eccsq,  float& em,    float& argpm,  float& inclm, float& mm,
       float& nm,    float& nodem,
       int& irez,
       float& atime, float& d2201, float& d2211,  float& d3210, float& d3222,
       float& d4410, float& d4422, float& d5220,  float& d5232, float& d5421,
       float& d5433, float& dedt,  float& didt,   float& dmdt,  float& dndt,
       float& dnodt, float& domdt, float& del1,   float& del2,  float& del3,
       float& xfact, float& xlamo, float& xli,    float& xni
     );

static void dspace
     (
       int irez,
       float d2201,  float d2211,  float d3210,   float d3222,  float d4410,
       float d4422,  float d5220,  float d5232,   float d5421,  float d5433,
       float dedt,   float del1,   float del2,    float del3,   float didt,
       float dmdt,   float dnodt,  float domdt,   float argpo,  float argpdot,
       float t,      float tc,     float gsto,    float xfact,  float xlamo,
       float no,
       float& atime, float& em,    float& argpm,  float& inclm, float& xli,
       float& mm,    float& xni,   float& nodem,  float& dndt,  float& nm
     );

static void initl
     (
       int satn,      gravconsttype whichconst,
       float ecco,   float epoch,  float inclo,   float& no,
       char& method,
       float& ainv,  float& ao,    float& con41,  float& con42, float& cosio,
       float& cosio2,float& eccsq, float& omeosq, float& posq,
       float& rp,    float& rteosq,float& sinio , float& gsto, char opsmode
     );

/* -----------------------------------------------------------------------------
*
*                           procedure dpper
*
*  this procedure provides deep space long period periodic contributions
*    to the mean elements.  by design, these periodics are zero at epoch.
*    this used to be dscom which included initialization, but it's really a
*    recurring function.
*
*  author        : david vallado                  719-573-2600   28 jun 2005
*
*  inputs        :
*    e3          -
*    ee2         -
*    peo         -
*    pgho        -
*    pho         -
*    pinco       -
*    plo         -
*    se2 , se3 , sgh2, sgh3, sgh4, sh2, sh3, si2, si3, sl2, sl3, sl4 -
*    t           -
*    xh2, xh3, xi2, xi3, xl2, xl3, xl4 -
*    zmol        -
*    zmos        -
*    ep          - eccentricity                           0.0 - 1.0
*    inclo       - inclination - needed for lyddane modification
*    nodep       - right ascension of ascending node
*    argpp       - argument of perigee
*    mp          - mean anomaly
*
*  outputs       :
*    ep          - eccentricity                           0.0 - 1.0
*    inclp       - inclination
*    nodep        - right ascension of ascending node
*    argpp       - argument of perigee
*    mp          - mean anomaly
*
*  locals        :
*    alfdp       -
*    betdp       -
*    cosip  , sinip  , cosop  , sinop  ,
*    dalf        -
*    dbet        -
*    dls         -
*    f2, f3      -
*    pe          -
*    pgh         -
*    ph          -
*    pinc        -
*    pl          -
*    sel   , ses   , sghl  , sghs  , shl   , shs   , sil   , sinzf , sis   ,
*    sll   , sls
*    xls         -
*    xnoh        -
*    zf          -
*    zm          -
*
*  coupling      :
*    none.
*
*  references    :
*    hoots, roehrich, norad spacetrack report #3 1980
*    hoots, norad spacetrack report #6 1986
*    hoots, schumacher and glover 2004
*    vallado, crawford, hujsak, kelso  2006
  ----------------------------------------------------------------------------*/

static void dpper
     (
       float e3,     float ee2,    float peo,     float pgho,   float pho,
       float pinco,  float plo,    float se2,     float se3,    float sgh2,
       float sgh3,   float sgh4,   float sh2,     float sh3,    float si2,
       float si3,    float sl2,    float sl3,     float sl4,    float t,
       float xgh2,   float xgh3,   float xgh4,    float xh2,    float xh3,
       float xi2,    float xi3,    float xl2,     float xl3,    float xl4,
       float zmol,   float zmos,   float inclo,
       char init,
       float& ep,    float& inclp, float& nodep,  float& argpp, float& mp,
       char opsmode
     )
{
     /* --------------------- local variables ------------------------ */
     const float twopi = 2.0f * pi;
     float alfdp, betdp, cosip, cosop, dalf, dbet, dls,
          f2,    f3,    pe,    pgh,   ph,   pinc, pl ,
          sel,   ses,   sghl,  sghs,  shll, shs,  sil,
          sinip, sinop, sinzf, sis,   sll,  sls,  xls,
          xnoh,  zf,    zm,    zel,   zes,  znl,  zns;

     /* ---------------------- constants ----------------------------- */
     zns   = 1.19459e-5;
     zes   = 0.01675;
     znl   = 1.5835218e-4;
     zel   = 0.05490;

     /* --------------- calculate time varying periodics ----------- */
     zm    = zmos + zns * t;
     // be sure that the initial call has time set to zero
     if (init == 'y')
         zm = zmos;
     zf    = zm + 2.0f * zes * sinf(zm);
     sinzf = sinf(zf);
     f2    =  0.5f * sinzf * sinzf - 0.25f;
     f3    = -0.5f * sinzf * cosf(zf);
     ses   = se2* f2 + se3 * f3;
     sis   = si2 * f2 + si3 * f3;
     sls   = sl2 * f2 + sl3 * f3 + sl4 * sinzf;
     sghs  = sgh2 * f2 + sgh3 * f3 + sgh4 * sinzf;
     shs   = sh2 * f2 + sh3 * f3;
     zm    = zmol + znl * t;
     if (init == 'y')
         zm = zmol;
     zf    = zm + 2.0f * zel * sinf(zm);
     sinzf = sinf(zf);
     f2    =  0.5f * sinzf * sinzf - 0.25f;
     f3    = -0.5f * sinzf * cosf(zf);
     sel   = ee2 * f2 + e3 * f3;
     sil   = xi2 * f2 + xi3 * f3;
     sll   = xl2 * f2 + xl3 * f3 + xl4 * sinzf;
     sghl  = xgh2 * f2 + xgh3 * f3 + xgh4 * sinzf;
     shll  = xh2 * f2 + xh3 * f3;
     pe    = ses + sel;
     pinc  = sis + sil;
     pl    = sls + sll;
     pgh   = sghs + sghl;
     ph    = shs + shll;

     if (init == 'n')
       {
       pe    = pe - peo;
       pinc  = pinc - pinco;
       pl    = pl - plo;
       pgh   = pgh - pgho;
       ph    = ph - pho;
       inclp = inclp + pinc;
       ep    = ep + pe;
       sinip = sinf(inclp);
       cosip = cosf(inclp);

       /* ----------------- apply periodics directly ------------ */
       //  sgp4fix for lyddane choice
       //  strn3 used original inclination - this is technically feasible
       //  gsfc used perturbed inclination - also technically feasible
       //  probably best to readjust the 0.2 limit value and limit discontinuity
       //  0.2 rad = 11.45916 deg
       //  use next line for original strn3 approach and original inclination
       //  if (inclo >= 0.2)
       //  use next line for gsfc version and perturbed inclination
       if (inclp >= 0.2f)
         {
           ph     = ph / sinip;
           pgh    = pgh - cosip * ph;
           argpp  = argpp + pgh;
           nodep  = nodep + ph;
           mp     = mp + pl;
         }
         else
         {
           /* ---- apply periodics with lyddane modification ---- */
           sinop  = sinf(nodep);
           cosop  = cosf(nodep);
           alfdp  = sinip * sinop;
           betdp  = sinip * cosop;
           dalf   =  ph * cosop + pinc * cosip * sinop;
           dbet   = -ph * sinop + pinc * cosip * cosop;
           alfdp  = alfdp + dalf;
           betdp  = betdp + dbet;
           nodep  = fmodf(nodep, twopi);
           //  sgp4fix for afspc written intrinsic functions
           // nodep used without a trigonometric function ahead
           if ((nodep < 0.0f) && (opsmode == 'a'))
               nodep = nodep + twopi;
           xls    = mp + argpp + cosip * nodep;
           dls    = pl + pgh - pinc * nodep * sinip;
           xls    = xls + dls;
           xnoh   = nodep;
           nodep  = atan2f(alfdp, betdp);
           //  sgp4fix for afspc written intrinsic functions
           // nodep used without a trigonometric function ahead
           if ((nodep < 0.0f) && (opsmode == 'a'))
               nodep = nodep + twopi;
           if (fabsf(xnoh - nodep) > pi)
             if (nodep < xnoh)
                nodep = nodep + twopi;
               else
                nodep = nodep - twopi;
           mp    = mp + pl;
           argpp = xls - mp - cosip * nodep;
         }
       }   // if init == 'n'

//#include "debug1.cpp"
}  // end dpper

/*-----------------------------------------------------------------------------
*
*                           procedure dscom
*
*  this procedure provides deep space common items used by both the secular
*    and periodics subroutines.  input is provided as shown. this routine
*    used to be called dpper, but the functions inside weren't well organized.
*
*  author        : david vallado                  719-573-2600   28 jun 2005
*
*  inputs        :
*    epoch       -
*    ep          - eccentricity
*    argpp       - argument of perigee
*    tc          -
*    inclp       - inclination
*    nodep       - right ascension of ascending node
*    np          - mean motion
*
*  outputs       :
*    sinim  , cosim  , sinomm , cosomm , snodm  , cnodm
*    day         -
*    e3          -
*    ee2         -
*    em          - eccentricity
*    emsq        - eccentricity squared
*    gam         -
*    peo         -
*    pgho        -
*    pho         -
*    pinco       -
*    plo         -
*    rtemsq      -
*    se2, se3         -
*    sgh2, sgh3, sgh4        -
*    sh2, sh3, si2, si3, sl2, sl3, sl4         -
*    s1, s2, s3, s4, s5, s6, s7          -
*    ss1, ss2, ss3, ss4, ss5, ss6, ss7, sz1, sz2, sz3         -
*    sz11, sz12, sz13, sz21, sz22, sz23, sz31, sz32, sz33        -
*    xgh2, xgh3, xgh4, xh2, xh3, xi2, xi3, xl2, xl3, xl4         -
*    nm          - mean motion
*    z1, z2, z3, z11, z12, z13, z21, z22, z23, z31, z32, z33         -
*    zmol        -
*    zmos        -
*
*  locals        :
*    a1, a2, a3, a4, a5, a6, a7, a8, a9, a10         -
*    betasq      -
*    cc          -
*    ctem, stem        -
*    x1, x2, x3, x4, x5, x6, x7, x8          -
*    xnodce      -
*    xnoi        -
*    zcosg  , zsing  , zcosgl , zsingl , zcosh  , zsinh  , zcoshl , zsinhl ,
*    zcosi  , zsini  , zcosil , zsinil ,
*    zx          -
*    zy          -
*
*  coupling      :
*    none.
*
*  references    :
*    hoots, roehrich, norad spacetrack report #3 1980
*    hoots, norad spacetrack report #6 1986
*    hoots, schumacher and glover 2004
*    vallado, crawford, hujsak, kelso  2006
  ----------------------------------------------------------------------------*/

static void dscom
     (
       float epoch,  float ep,     float argpp,   float tc,     float inclp,
       float nodep,  float np,
       float& snodm, float& cnodm, float& sinim,  float& cosim, float& sinomm,
       float& cosomm,float& day,   float& e3,     float& ee2,   float& em,
       float& emsq,  float& gam,   float& peo,    float& pgho,  float& pho,
       float& pinco, float& plo,   float& rtemsq, float& se2,   float& se3,
       float& sgh2,  float& sgh3,  float& sgh4,   float& sh2,   float& sh3,
       float& si2,   float& si3,   float& sl2,    float& sl3,   float& sl4,
       float& s1,    float& s2,    float& s3,     float& s4,    float& s5,
       float& s6,    float& s7,    float& ss1,    float& ss2,   float& ss3,
       float& ss4,   float& ss5,   float& ss6,    float& ss7,   float& sz1,
       float& sz2,   float& sz3,   float& sz11,   float& sz12,  float& sz13,
       float& sz21,  float& sz22,  float& sz23,   float& sz31,  float& sz32,
       float& sz33,  float& xgh2,  float& xgh3,   float& xgh4,  float& xh2,
       float& xh3,   float& xi2,   float& xi3,    float& xl2,   float& xl3,
       float& xl4,   float& nm,    float& z1,     float& z2,    float& z3,
       float& z11,   float& z12,   float& z13,    float& z21,   float& z22,
       float& z23,   float& z31,   float& z32,    float& z33,   float& zmol,
       float& zmos
     )
{
     /* -------------------------- constants ------------------------- */
     const float zes     =  0.01675;
     const float zel     =  0.05490;
     const float c1ss    =  2.9864797e-6;
     const float c1l     =  4.7968065e-7;
     const float zsinis  =  0.39785416;
     const float zcosis  =  0.91744867;
     const float zcosgs  =  0.1945905;
     const float zsings  = -0.98088458;
     const float twopi   =  2.0f * pi;

     /* --------------------- local variables ------------------------ */
     int lsflg;
     float a1    , a2    , a3    , a4    , a5    , a6    , a7    ,
        a8    , a9    , a10   , betasq, cc    , ctem  , stem  ,
        x1    , x2    , x3    , x4    , x5    , x6    , x7    ,
        x8    , xnodce, xnoi  , zcosg , zcosgl, zcosh , zcoshl,
        zcosi , zcosil, zsing , zsingl, zsinh , zsinhl, zsini ,
        zsinil, zx    , zy;

     nm     = np;
     em     = ep;
     snodm  = sinf(nodep);
     cnodm  = cosf(nodep);
     sinomm = sinf(argpp);
     cosomm = cosf(argpp);
     sinim  = sinf(inclp);
     cosim  = cosf(inclp);
     emsq   = em * em;
     betasq = 1.0f - emsq;
     rtemsq = sqrtf(betasq);

     /* ----------------- initialize lunar solar terms --------------- */
     peo    = 0.0f;
     pinco  = 0.0f;
     plo    = 0.0f;
     pgho   = 0.0f;
     pho    = 0.0f;
     day    = epoch + 18261.5f + tc / 1440.0f;
     xnodce = fmodf(4.5236020f - 9.2422029e-4f * day, twopi);
     stem   = sinf(xnodce);
     ctem   = cosf(xnodce);
     zcosil = 0.91375164f - 0.03568096f * ctem;
     zsinil = sqrtf(1.0f - zcosil * zcosil);
     zsinhl = 0.089683511f * stem / zsinil;
     zcoshl = sqrtf(1.0f - zsinhl * zsinhl);
     gam    = 5.8351514f + 0.0019443680f * day;
     zx     = 0.39785416f * stem / zsinil;
     zy     = zcoshl * ctem + 0.91744867f * zsinhl * stem;
     zx     = atan2f(zx, zy);
     zx     = gam + zx - xnodce;
     zcosgl = cosf(zx);
     zsingl = sinf(zx);

     /* ------------------------- do solar terms --------------------- */
     zcosg = zcosgs;
     zsing = zsings;
     zcosi = zcosis;
     zsini = zsinis;
     zcosh = cnodm;
     zsinh = snodm;
     cc    = c1ss;
     xnoi  = 1.0f / nm;

     for (lsflg = 1; lsflg <= 2; lsflg++)
       {
         a1  =   zcosg * zcosh + zsing * zcosi * zsinh;
         a3  =  -zsing * zcosh + zcosg * zcosi * zsinh;
         a7  =  -zcosg * zsinh + zsing * zcosi * zcosh;
         a8  =   zsing * zsini;
         a9  =   zsing * zsinh + zcosg * zcosi * zcosh;
         a10 =   zcosg * zsini;
         a2  =   cosim * a7 + sinim * a8;
         a4  =   cosim * a9 + sinim * a10;
         a5  =  -sinim * a7 + cosim * a8;
         a6  =  -sinim * a9 + cosim * a10;

         x1  =  a1 * cosomm + a2 * sinomm;
         x2  =  a3 * cosomm + a4 * sinomm;
         x3  = -a1 * sinomm + a2 * cosomm;
         x4  = -a3 * sinomm + a4 * cosomm;
         x5  =  a5 * sinomm;
         x6  =  a6 * sinomm;
         x7  =  a5 * cosomm;
         x8  =  a6 * cosomm;

         z31 = 12.0f * x1 * x1 - 3.0f * x3 * x3;
         z32 = 24.0f * x1 * x2 - 6.0f * x3 * x4;
         z33 = 12.0f * x2 * x2 - 3.0f * x4 * x4;
         z1  =  3.0f *  (a1 * a1 + a2 * a2) + z31 * emsq;
         z2  =  6.0f *  (a1 * a3 + a2 * a4) + z32 * emsq;
         z3  =  3.0f *  (a3 * a3 + a4 * a4) + z33 * emsq;
         z11 = -6.0f * a1 * a5 + emsq *  (-24.0f * x1 * x7-6.0f * x3 * x5);
         z12 = -6.0f *  (a1 * a6 + a3 * a5) + emsq *
                (-24.0f * (x2 * x7 + x1 * x8) - 6.0f * (x3 * x6 + x4 * x5));
         z13 = -6.0f * a3 * a6 + emsq * (-24.0f * x2 * x8 - 6.0f * x4 * x6);
         z21 =  6.0f * a2 * a5 + emsq * (24.0f * x1 * x5 - 6.0f * x3 * x7);
         z22 =  6.0f *  (a4 * a5 + a2 * a6) + emsq *
                (24.0f * (x2 * x5 + x1 * x6) - 6.0f * (x4 * x7 + x3 * x8));
         z23 =  6.0f * a4 * a6 + emsq * (24.0f * x2 * x6 - 6.0f * x4 * x8);
         z1  = z1 + z1 + betasq * z31;
         z2  = z2 + z2 + betasq * z32;
         z3  = z3 + z3 + betasq * z33;
         s3  = cc * xnoi;
         s2  = -0.5f * s3 / rtemsq;
         s4  = s3 * rtemsq;
         s1  = -15.0f * em * s4;
         s5  = x1 * x3 + x2 * x4;
         s6  = x2 * x3 + x1 * x4;
         s7  = x2 * x4 - x1 * x3;

         /* ----------------------- do lunar terms ------------------- */
         if (lsflg == 1)
           {
             ss1   = s1;
             ss2   = s2;
             ss3   = s3;
             ss4   = s4;
             ss5   = s5;
             ss6   = s6;
             ss7   = s7;
             sz1   = z1;
             sz2   = z2;
             sz3   = z3;
             sz11  = z11;
             sz12  = z12;
             sz13  = z13;
             sz21  = z21;
             sz22  = z22;
             sz23  = z23;
             sz31  = z31;
             sz32  = z32;
             sz33  = z33;
             zcosg = zcosgl;
             zsing = zsingl;
             zcosi = zcosil;
             zsini = zsinil;
             zcosh = zcoshl * cnodm + zsinhl * snodm;
             zsinh = snodm * zcoshl - cnodm * zsinhl;
             cc    = c1l;
          }
       }

     zmol = fmodf(4.7199672f + 0.22997150f  * day - gam, twopi);
     zmos = fmodf(6.2565837f + 0.017201977f * day, twopi);

     /* ------------------------ do solar terms ---------------------- */
     se2  =   2.0f * ss1 * ss6;
     se3  =   2.0f * ss1 * ss7;
     si2  =   2.0f * ss2 * sz12;
     si3  =   2.0f * ss2 * (sz13 - sz11);
     sl2  =  -2.0f * ss3 * sz2;
     sl3  =  -2.0f * ss3 * (sz3 - sz1);
     sl4  =  -2.0f * ss3 * (-21.0f - 9.0f * emsq) * zes;
     sgh2 =   2.0f * ss4 * sz32;
     sgh3 =   2.0f * ss4 * (sz33 - sz31);
     sgh4 = -18.0f * ss4 * zes;
     sh2  =  -2.0f * ss2 * sz22;
     sh3  =  -2.0f * ss2 * (sz23 - sz21);

     /* ------------------------ do lunar terms ---------------------- */
     ee2  =   2.0f * s1 * s6;
     e3   =   2.0f * s1 * s7;
     xi2  =   2.0f * s2 * z12;
     xi3  =   2.0f * s2 * (z13 - z11);
     xl2  =  -2.0f * s3 * z2;
     xl3  =  -2.0f * s3 * (z3 - z1);
     xl4  =  -2.0f * s3 * (-21.0f - 9.0f * emsq) * zel;
     xgh2 =   2.0f * s4 * z32;
     xgh3 =   2.0f * s4 * (z33 - z31);
     xgh4 = -18.0f * s4 * zel;
     xh2  =  -2.0f * s2 * z22;
     xh3  =  -2.0f * s2 * (z23 - z21);

//#include "debug2.cpp"
}  // end dscom

/*-----------------------------------------------------------------------------
*
*                           procedure dsinit
*
*  this procedure provides deep space contributions to mean motion dot due
*    to geopotential resonance with half day and one day orbits.
*
*  author        : david vallado                  719-573-2600   28 jun 2005
*
*  inputs        :
*    cosim, sinim-
*    emsq        - eccentricity squared
*    argpo       - argument of perigee
*    s1, s2, s3, s4, s5      -
*    ss1, ss2, ss3, ss4, ss5 -
*    sz1, sz3, sz11, sz13, sz21, sz23, sz31, sz33 -
*    t           - time
*    tc          -
*    gsto        - greenwich sidereal time                   rad
*    mo          - mean anomaly
*    mdot        - mean anomaly dot (rate)
*    no          - mean motion
*    nodeo       - right ascension of ascending node
*    nodedot     - right ascension of ascending node dot (rate)
*    xpidot      -
*    z1, z3, z11, z13, z21, z23, z31, z33 -
*    eccm        - eccentricity
*    argpm       - argument of perigee
*    inclm       - inclination
*    mm          - mean anomaly
*    xn          - mean motion
*    nodem       - right ascension of ascending node
*
*  outputs       :
*    em          - eccentricity
*    argpm       - argument of perigee
*    inclm       - inclination
*    mm          - mean anomaly
*    nm          - mean motion
*    nodem       - right ascension of ascending node
*    irez        - flag for resonance           0-none, 1-one day, 2-half day
*    atime       -
*    d2201, d2211, d3210, d3222, d4410, d4422, d5220, d5232, d5421, d5433    -
*    dedt        -
*    didt        -
*    dmdt        -
*    dndt        -
*    dnodt       -
*    domdt       -
*    del1, del2, del3        -
*    ses  , sghl , sghs , sgs  , shl  , shs  , sis  , sls
*    theta       -
*    xfact       -
*    xlamo       -
*    xli         -
*    xni
*
*  locals        :
*    ainv2       -
*    aonv        -
*    cosisq      -
*    eoc         -
*    f220, f221, f311, f321, f322, f330, f441, f442, f522, f523, f542, f543  -
*    g200, g201, g211, g300, g310, g322, g410, g422, g520, g521, g532, g533  -
*    sini2       -
*    temp        -
*    temp1       -
*    theta       -
*    xno2        -
*
*  coupling      :
*    getgravconst
*
*  references    :
*    hoots, roehrich, norad spacetrack report #3 1980
*    hoots, norad spacetrack report #6 1986
*    hoots, schumacher and glover 2004
*    vallado, crawford, hujsak, kelso  2006
  ----------------------------------------------------------------------------*/

static void dsinit
     (
       gravconsttype whichconst,
       float cosim,  float emsq,   float argpo,   float s1,     float s2,
       float s3,     float s4,     float s5,      float sinim,  float ss1,
       float ss2,    float ss3,    float ss4,     float ss5,    float sz1,
       float sz3,    float sz11,   float sz13,    float sz21,   float sz23,
       float sz31,   float sz33,   float t,       float tc,     float gsto,
       float mo,     float mdot,   float no,      float nodeo,  float nodedot,
       float xpidot, float z1,     float z3,      float z11,    float z13,
       float z21,    float z23,    float z31,     float z33,    float ecco,
       float eccsq,  float& em,    float& argpm,  float& inclm, float& mm,
       float& nm,    float& nodem,
       int& irez,
       float& atime, float& d2201, float& d2211,  float& d3210, float& d3222,
       float& d4410, float& d4422, float& d5220,  float& d5232, float& d5421,
       float& d5433, float& dedt,  float& didt,   float& dmdt,  float& dndt,
       float& dnodt, float& domdt, float& del1,   float& del2,  float& del3,
       float& xfact, float& xlamo, float& xli,    float& xni
     )
{
     /* --------------------- local variables ------------------------ */
     const float twopi = 2.0f * pi;

     float ainv2 , aonv=0.0, cosisq, eoc, f220 , f221  , f311  ,
          f321  , f322  , f330  , f441  , f442  , f522  , f523  ,
          f542  , f543  , g200  , g201  , g211  , g300  , g310  ,
          g322  , g410  , g422  , g520  , g521  , g532  , g533  ,
          ses   , sgs   , sghl  , sghs  , shs   , shll  , sis   ,
          sini2 , sls   , temp  , temp1 , theta , xno2  , q22   ,
          q31   , q33   , root22, root44, root54, rptim , root32,
          root52, x2o3  , xke   , znl   , emo   , zns   , emsqo,
          tumin, mu, radiusearthkm, j2, j3, j4, j3oj2;

     q22    = 1.7891679e-6;
     q31    = 2.1460748e-6;
     q33    = 2.2123015e-7;
     root22 = 1.7891679e-6;
     root44 = 7.3636953e-9;
     root54 = 2.1765803e-9;
     rptim  = 4.37526908801129966e-3; // this equates to 7.29211514668855e-5 rad/sec
     root32 = 3.7393792e-7;
     root52 = 1.1428639e-7;
     x2o3   = 2.0f / 3.0f;
     znl    = 1.5835218e-4;
     zns    = 1.19459e-5;

     // sgp4fix identify constants and allow alternate values
     getgravconst( whichconst, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );

     /* -------------------- deep space initialization ------------ */
     irez = 0;
     if ((nm < 0.0052359877f) && (nm > 0.0034906585f))
         irez = 1;
     if ((nm >= 8.26e-3f) && (nm <= 9.24e-3f) && (em >= 0.5f))
         irez = 2;

     /* ------------------------ do solar terms ------------------- */
     ses  =  ss1 * zns * ss5;
     sis  =  ss2 * zns * (sz11 + sz13);
     sls  = -zns * ss3 * (sz1 + sz3 - 14.0f - 6.0f * emsq);
     sghs =  ss4 * zns * (sz31 + sz33 - 6.0f);
     shs  = -zns * ss2 * (sz21 + sz23);
     // sgp4fix for 180 deg incl
     if ((inclm < 5.2359877e-2f) || (inclm > pi - 5.2359877e-2f))
       shs = 0.0f;
     if (sinim != 0.0f)
       shs = shs / sinim;
     sgs  = sghs - cosim * shs;

     /* ------------------------- do lunar terms ------------------ */
     dedt = ses + s1 * znl * s5;
     didt = sis + s2 * znl * (z11 + z13);
     dmdt = sls - znl * s3 * (z1 + z3 - 14.0f - 6.0f * emsq);
     sghl = s4 * znl * (z31 + z33 - 6.0f);
     shll = -znl * s2 * (z21 + z23);
     // sgp4fix for 180 deg incl
     if ((inclm < 5.2359877e-2f) || (inclm > pi - 5.2359877e-2f))
         shll = 0.0f;
     domdt = sgs + sghl;
     dnodt = shs;
     if (sinim != 0.0f)
       {
         domdt = domdt - cosim / sinim * shll;
         dnodt = dnodt + shll / sinim;
       }

     /* ----------- calculate deep space resonance effects -------- */
     dndt   = 0.0f;
     theta  = fmodf(gsto + tc * rptim, twopi);
     em     = em + dedt * t;
     inclm  = inclm + didt * t;
     argpm  = argpm + domdt * t;
     nodem  = nodem + dnodt * t;
     mm     = mm + dmdt * t;
     //   sgp4fix for negative inclinations
     //   the following if statement should be commented out
     //if (inclm < 0.0f)
     //  {
     //    inclm  = -inclm;
     //    argpm  = argpm - pi;
     //    nodem = nodem + pi;
     //  }

     /* -------------- initialize the resonance terms ------------- */
     if (irez != 0)
       {
         aonv = powf(nm / xke, x2o3);

         /* ---------- geopotential resonance for 12 hour orbits ------ */
         if (irez == 2)
           {
             cosisq = cosim * cosim;
             emo    = em;
             em     = ecco;
             emsqo  = emsq;
             emsq   = eccsq;
             eoc    = em * emsq;
             g201   = -0.306f - (em - 0.64f) * 0.440f;

             if (em <= 0.65f)
               {
                 g211 =    3.616f  -  13.2470f * em +  16.2900f * emsq;
                 g310 =  -19.302f  + 117.3900f * em - 228.4190f * emsq +  156.5910f * eoc;
                 g322 =  -18.9068f + 109.7927f * em - 214.6334f * emsq +  146.5816f * eoc;
                 g410 =  -41.122f  + 242.6940f * em - 471.0940f * emsq +  313.9530f * eoc;
                 g422 = -146.407f  + 841.8800f * em - 1629.014f * emsq + 1083.4350f * eoc;
                 g520 = -532.114f  + 3017.977f * em - 5740.032f * emsq + 3708.2760f * eoc;
               }
               else
               {
                 g211 =   -72.099f +   331.819f * em -   508.738f * emsq +   266.724f * eoc;
                 g310 =  -346.844f +  1582.851f * em -  2415.925f * emsq +  1246.113f * eoc;
                 g322 =  -342.585f +  1554.908f * em -  2366.899f * emsq +  1215.972f * eoc;
                 g410 = -1052.797f +  4758.686f * em -  7193.992f * emsq +  3651.957f * eoc;
                 g422 = -3581.690f + 16178.110f * em - 24462.770f * emsq + 12422.520f * eoc;
                 if (em > 0.715f)
                     g520 =-5149.66f + 29936.92f * em - 54087.36f * emsq + 31324.56f * eoc;
                   else
                     g520 = 1464.74f -  4664.75f * em +  3763.64f * emsq;
               }
             if (em < 0.7f)
               {
                 g533 = -919.22770f + 4988.6100f * em - 9064.7700f * emsq + 5542.21f  * eoc;
                 g521 = -822.71072f + 4568.6173f * em - 8491.4146f * emsq + 5337.524f * eoc;
                 g532 = -853.66600f + 4690.2500f * em - 8624.7700f * emsq + 5341.4f  * eoc;
               }
               else
               {
                 g533 =-37995.780f + 161616.52f * em - 229838.20f * emsq + 109377.94f * eoc;
                 g521 =-51752.104f + 218913.95f * em - 309468.16f * emsq + 146349.42f * eoc;
                 g532 =-40023.880f + 170470.89f * em - 242699.48f * emsq + 115605.82f * eoc;
               }

             sini2=  sinim * sinim;
             f220 =  0.75f * (1.0f + 2.0f * cosim+cosisq);
             f221 =  1.5f * sini2;
             f321 =  1.875f * sinim  *  (1.0f - 2.0f * cosim - 3.0f * cosisq);
             f322 = -1.875f * sinim  *  (1.0f + 2.0f * cosim - 3.0f * cosisq);
             f441 = 35.0f * sini2 * f220;
             f442 = 39.3750f * sini2 * sini2;
             f522 =  9.84375f * sinim * (sini2 * (1.0f - 2.0f * cosim- 5.0f * cosisq) +
                     0.33333333f * (-2.0f + 4.0f * cosim + 6.0f * cosisq) );
             f523 = sinim * (4.92187512f * sini2 * (-2.0f - 4.0f * cosim +
                    10.0f * cosisq) + 6.56250012f * (1.0f+2.0f * cosim - 3.0f * cosisq));
             f542 = 29.53125f * sinim * (2.0f - 8.0f * cosim+cosisq *
                    (-12.0f + 8.0f * cosim + 10.0f * cosisq));
             f543 = 29.53125f * sinim * (-2.0f - 8.0f * cosim+cosisq *
                    (12.0f + 8.0f * cosim - 10.0f * cosisq));
             xno2  =  nm * nm;
             ainv2 =  aonv * aonv;
             temp1 =  3.0f * xno2 * ainv2;
             temp  =  temp1 * root22;
             d2201 =  temp * f220 * g201;
             d2211 =  temp * f221 * g211;
             temp1 =  temp1 * aonv;
             temp  =  temp1 * root32;
             d3210 =  temp * f321 * g310;
             d3222 =  temp * f322 * g322;
             temp1 =  temp1 * aonv;
             temp  =  2.0f * temp1 * root44;
             d4410 =  temp * f441 * g410;
             d4422 =  temp * f442 * g422;
             temp1 =  temp1 * aonv;
             temp  =  temp1 * root52;
             d5220 =  temp * f522 * g520;
             d5232 =  temp * f523 * g532;
             temp  =  2.0f * temp1 * root54;
             d5421 =  temp * f542 * g521;
             d5433 =  temp * f543 * g533;
             xlamo =  fmodf(mo + nodeo + nodeo-theta - theta, twopi);
             xfact =  mdot + dmdt + 2.0f * (nodedot + dnodt - rptim) - no;
             em    = emo;
             emsq  = emsqo;
           }

         /* ---------------- synchronous resonance terms -------------- */
         if (irez == 1)
           {
             g200  = 1.0f + emsq * (-2.5f + 0.8125f * emsq);
             g310  = 1.0f + 2.0f * emsq;
             g300  = 1.0f + emsq * (-6.0f + 6.60937f * emsq);
             f220  = 0.75f * (1.0f + cosim) * (1.0f + cosim);
             f311  = 0.9375f * sinim * sinim * (1.0f + 3.0f * cosim) - 0.75f * (1.0f + cosim);
             f330  = 1.0f + cosim;
             f330  = 1.875f * f330 * f330 * f330;
             del1  = 3.0f * nm * nm * aonv * aonv;
             del2  = 2.0f * del1 * f220 * g200 * q22;
             del3  = 3.0f * del1 * f330 * g300 * q33 * aonv;
             del1  = del1 * f311 * g310 * q31 * aonv;
             xlamo = fmodf(mo + nodeo + argpo - theta, twopi);
             xfact = mdot + xpidot - rptim + dmdt + domdt + dnodt - no;
           }

         /* ------------ for sgp4, initialize the integrator ---------- */
         xli   = xlamo;
         xni   = no;
         atime = 0.0f;
         nm    = no + dndt;
       }

//#include "debug3.cpp"
}  // end dsinit

/*-----------------------------------------------------------------------------
*
*                           procedure dspace
*
*  this procedure provides deep space contributions to mean elements for
*    perturbing third body.  these effects have been averaged over one
*    revolution of the sun and moon.  for earth resonance effects, the
*    effects have been averaged over no revolutions of the satellite.
*    (mean motion)
*
*  author        : david vallado                  719-573-2600   28 jun 2005
*
*  inputs        :
*    d2201, d2211, d3210, d3222, d4410, d4422, d5220, d5232, d5421, d5433 -
*    dedt        -
*    del1, del2, del3  -
*    didt        -
*    dmdt        -
*    dnodt       -
*    domdt       -
*    irez        - flag for resonance           0-none, 1-one day, 2-half day
*    argpo       - argument of perigee
*    argpdot     - argument of perigee dot (rate)
*    t           - time
*    tc          -
*    gsto        - gst
*    xfact       -
*    xlamo       -
*    no          - mean motion
*    atime       -
*    em          - eccentricity
*    ft          -
*    argpm       - argument of perigee
*    inclm       - inclination
*    xli         -
*    mm          - mean anomaly
*    xni         - mean motion
*    nodem       - right ascension of ascending node
*
*  outputs       :
*    atime       -
*    em          - eccentricity
*    argpm       - argument of perigee
*    inclm       - inclination
*    xli         -
*    mm          - mean anomaly
*    xni         -
*    nodem       - right ascension of ascending node
*    dndt        -
*    nm          - mean motion
*
*  locals        :
*    delt        -
*    ft          -
*    theta       -
*    x2li        -
*    x2omi       -
*    xl          -
*    xldot       -
*    xnddt       -
*    xndt        -
*    xomi        -
*
*  coupling      :
*    none        -
*
*  references    :
*    hoots, roehrich, norad spacetrack report #3 1980
*    hoots, norad spacetrack report #6 1986
*    hoots, schumacher and glover 2004
*    vallado, crawford, hujsak, kelso  2006
  ----------------------------------------------------------------------------*/

static void dspace
     (
       int irez,
       float d2201,  float d2211,  float d3210,   float d3222,  float d4410,
       float d4422,  float d5220,  float d5232,   float d5421,  float d5433,
       float dedt,   float del1,   float del2,    float del3,   float didt,
       float dmdt,   float dnodt,  float domdt,   float argpo,  float argpdot,
       float t,      float tc,     float gsto,    float xfact,  float xlamo,
       float no,
       float& atime, float& em,    float& argpm,  float& inclm, float& xli,
       float& mm,    float& xni,   float& nodem,  float& dndt,  float& nm
     )
{
     const float twopi = 2.0f * pi;
     int iretn , iret;
     float delt, ft, theta, x2li, x2omi, xl, xldot , xnddt, xndt, xomi, g22, g32,
          g44, g52, g54, fasx2, fasx4, fasx6, rptim , step2, stepn , stepp;

     fasx2 = 0.13130908;
     fasx4 = 2.8843198;
     fasx6 = 0.37448087;
     g22   = 5.7686396;
     g32   = 0.95240898;
     g44   = 1.8014998;
     g52   = 1.0508330;
     g54   = 4.4108898;
     rptim = 4.37526908801129966e-3; // this equates to 7.29211514668855e-5 rad/sec
     stepp =    720.0f;
     stepn =   -720.0f;
     step2 = 259200.0f;

     /* ----------- calculate deep space resonance effects ----------- */
     dndt   = 0.0f;
     theta  = fmodf(gsto + tc * rptim, twopi);
     em     = em + dedt * t;

     inclm  = inclm + didt * t;
     argpm  = argpm + domdt * t;
     nodem  = nodem + dnodt * t;
     mm     = mm + dmdt * t;

     //   sgp4fix for negative inclinations
     //   the following if statement should be commented out
     //  if (inclm < 0.0f)
     // {
     //    inclm = -inclm;
     //    argpm = argpm - pi;
     //    nodem = nodem + pi;
     //  }

     /* - update resonances : numerical (euler-maclaurin) integration - */
     /* ------------------------- epoch restart ----------------------  */
     //   sgp4fix for propagator problems
     //   the following integration works for negative time steps and periods
     //   the specific changes are unknown because the original code was so convoluted

     // sgp4fix take out atime = 0.0 and fix for faster operation
     ft    = 0.0f;
     if (irez != 0)
       {
         // sgp4fix streamline check
         if ((atime == 0.0f) || (t * atime <= 0.0f) || (fabsf(t) < fabsf(atime)) )
           {
             atime  = 0.0f;
             xni    = no;
             xli    = xlamo;
           }
           // sgp4fix move check outside loop
           if (t > 0.0f)
               delt = stepp;
             else
               delt = stepn;

         iretn = 381; // added for do loop
         iret  =   0; // added for loop
         while (iretn == 381)
           {
             /* ------------------- dot terms calculated ------------- */
             /* ----------- near - synchronous resonance terms ------- */
             if (irez != 2)
               {
                 xndt  = del1 * sinf(xli - fasx2) + del2 * sinf(2.0f * (xli - fasx4)) +
                         del3 * sinf(3.0f * (xli - fasx6));
                 xldot = xni + xfact;
                 xnddt = del1 * cosf(xli - fasx2) +
                         2.0f * del2 * cosf(2.0f * (xli - fasx4)) +
                         3.0f * del3 * cosf(3.0f * (xli - fasx6));
                 xnddt = xnddt * xldot;
               }
               else
               {
                 /* --------- near - half-day resonance terms -------- */
                 xomi  = argpo + argpdot * atime;
                 x2omi = xomi + xomi;
                 x2li  = xli + xli;
                 xndt  = d2201 * sinf(x2omi + xli - g22) + d2211 * sinf(xli - g22) +
                       d3210 * sinf(xomi + xli - g32)  + d3222 * sinf(-xomi + xli - g32)+
                       d4410 * sinf(x2omi + x2li - g44)+ d4422 * sinf(x2li - g44) +
                       d5220 * sinf(xomi + xli - g52)  + d5232 * sinf(-xomi + xli - g52)+
                       d5421 * sinf(xomi + x2li - g54) + d5433 * sinf(-xomi + x2li - g54);
                 xldot = xni + xfact;
                 xnddt = d2201 * cosf(x2omi + xli - g22) + d2211 * cosf(xli - g22) +
                       d3210 * cosf(xomi + xli - g32) + d3222 * cosf(-xomi + xli - g32) +
                       d5220 * cosf(xomi + xli - g52) + d5232 * cosf(-xomi + xli - g52) +
                       2.0f * (d4410 * cosf(x2omi + x2li - g44) +
                       d4422 * cosf(x2li - g44) + d5421 * cosf(xomi + x2li - g54) +
                       d5433 * cosf(-xomi + x2li - g54));
                 xnddt = xnddt * xldot;
               }

             /* ----------------------- integrator ------------------- */
             // sgp4fix move end checks to end of routine
             if (fabsf(t - atime) >= stepp)
               {
                 iret  = 0;
                 iretn = 381;
               }
               else // exit here
               {
                 ft    = t - atime;
                 iretn = 0;
               }

             if (iretn == 381)
               {
                 xli   = xli + xldot * delt + xndt * step2;
                 xni   = xni + xndt * delt + xnddt * step2;
                 atime = atime + delt;
               }
           }  // while iretn = 381

         nm = xni + xndt * ft + xnddt * ft * ft * 0.5f;
         xl = xli + xldot * ft + xndt * ft * ft * 0.5f;
         if (irez != 1)
           {
             mm   = xl - 2.0f * nodem + 2.0f * theta;
             dndt = nm - no;
           }
           else
           {
             mm   = xl - nodem - argpm + theta;
             dndt = nm - no;
           }
         nm = no + dndt;
       }

//#include "debug4.cpp"
}  // end dsspace

/*-----------------------------------------------------------------------------
*
*                           procedure initl
*
*  this procedure initializes the spg4 propagator. all the initialization is
*    consolidated here instead of having multiple loops inside other routines.
*
*  author        : david vallado                  719-573-2600   28 jun 2005
*
*  inputs        :
*    ecco        - eccentricity                           0.0 - 1.0
*    epoch       - epoch time in days from jan 0, 1950. 0 hr
*    inclo       - inclination of satellite
*    no          - mean motion of satellite
*    satn        - satellite number
*
*  outputs       :
*    ainv        - 1.0 / a
*    ao          - semi major axis
*    con41       -
*    con42       - 1.0 - 5.0 cosf(i)
*    cosio       - cosine of inclination
*    cosio2      - cosio squared
*    eccsq       - eccentricity squared
*    method      - flag for deep space                    'd', 'n'
*    omeosq      - 1.0 - ecco * ecco
*    posq        - semi-parameter squared
*    rp          - radius of perigee
*    rteosq      - square root of (1.0 - ecco*ecco)
*    sinio       - sine of inclination
*    gsto        - gst at time of observation               rad
*    no          - mean motion of satellite
*
*  locals        :
*    ak          -
*    d1          -
*    del         -
*    adel        -
*    po          -
*
*  coupling      :
*    getgravconst
*    gstime      - find greenwich sidereal time from the julian date
*
*  references    :
*    hoots, roehrich, norad spacetrack report #3 1980
*    hoots, norad spacetrack report #6 1986
*    hoots, schumacher and glover 2004
*    vallado, crawford, hujsak, kelso  2006
  ----------------------------------------------------------------------------*/

static void initl
     (
       int satn,      gravconsttype whichconst,
       float ecco,   float epoch,  float inclo,   float& no,
       char& method,
       float& ainv,  float& ao,    float& con41,  float& con42, float& cosio,
       float& cosio2,float& eccsq, float& omeosq, float& posq,
       float& rp,    float& rteosq,float& sinio , float& gsto,
       char opsmode
     )
{
     /* --------------------- local variables ------------------------ */
     float ak, d1, del, adel, po, x2o3, j2, xke,
            tumin, mu, radiusearthkm, j3, j4, j3oj2;

     // sgp4fix use old way of finding gst
     float ds70;
     float ts70, tfrac, c1, thgr70, fk5r, c1p2p;
     const float twopi = 2.0f * pi;

     /* ----------------------- earth constants ---------------------- */
     // sgp4fix identify constants and allow alternate values
     getgravconst( whichconst, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );
     x2o3   = 2.0f / 3.0f;

     /* ------------- calculate auxillary epoch quantities ---------- */
     eccsq  = ecco * ecco;
     omeosq = 1.0f - eccsq;
     rteosq = sqrtf(omeosq);
     cosio  = cosf(inclo);
     cosio2 = cosio * cosio;

     /* ------------------ un-kozai the mean motion ----------------- */
     ak    = powf(xke / no, x2o3);
     d1    = 0.75f * j2 * (3.0f * cosio2 - 1.0f) / (rteosq * omeosq);
     del   = d1 / (ak * ak);
     adel  = ak * (1.0f - del * del - del *
             (1.0f / 3.0f + 134.0f * del * del / 81.0f));
     del   = d1/(adel * adel);
     no    = no / (1.0f + del);

     ao    = powf(xke / no, x2o3);
     sinio = sinf(inclo);
     po    = ao * omeosq;
     con42 = 1.0f - 5.0f * cosio2;
     con41 = -con42-cosio2-cosio2;
     ainv  = 1.0f / ao;
     posq  = po * po;
     rp    = ao * (1.0f - ecco);
     method = 'n';

     // sgp4fix modern approach to finding sidereal time
     if (opsmode == 'a')
        {
         // sgp4fix use old way of finding gst
         // count integer number of days from 0 jan 1970
         ts70  = epoch - 7305.0f;
         ds70 = floorf(ts70 + 1.0e-8f);
         tfrac = ts70 - ds70;
         // find greenwich location at epoch
         c1    = 1.72027916940703639e-2;
         thgr70= 1.7321343856509374;
         fk5r  = 5.07551419432269442e-15;
         c1p2p = c1 + twopi;
         gsto  = fmodf( thgr70 + c1*ds70 + c1p2p*tfrac + ts70*ts70*fk5r, twopi);
         if ( gsto < 0.0f )
             gsto = gsto + twopi;
       }
       else
        gsto = gstime(epoch + 2433281.5);


//#include "debug5.cpp"
}  // end initl

/*-----------------------------------------------------------------------------
*
*                             procedure sgp4init
*
*  this procedure initializes variables for sgp4.
*
*  author        : david vallado                  719-573-2600   28 jun 2005
*
*  inputs        :
*    opsmode     - mode of operation afspc or improved 'a', 'i'
*    whichconst  - which set of constants to use  72, 84
*    satn        - satellite number
*    bstar       - sgp4 type drag coefficient              kg/m2er
*    ecco        - eccentricity
*    epoch       - epoch time in days from jan 0, 1950. 0 hr
*    argpo       - argument of perigee (output if ds)
*    inclo       - inclination
*    mo          - mean anomaly (output if ds)
*    no          - mean motion
*    nodeo       - right ascension of ascending node
*
*  outputs       :
*    satrec      - common values for subsequent calls
*    return code - non-zero on error.
*                   1 - mean elements, ecc >= 1.0 or ecc < -0.001 or a < 0.95 er
*                   2 - mean motion less than 0.0
*                   3 - pert elements, ecc < 0.0  or  ecc > 1.0
*                   4 - semi-latus rectum < 0.0
*                   5 - epoch elements are sub-orbital
*                   6 - satellite has decayed
*
*  locals        :
*    cnodm  , snodm  , cosim  , sinim  , cosomm , sinomm
*    cc1sq  , cc2    , cc3
*    coef   , coef1
*    cosio4      -
*    day         -
*    dndt        -
*    em          - eccentricity
*    emsq        - eccentricity squared
*    eeta        -
*    etasq       -
*    gam         -
*    argpm       - argument of perigee
*    nodem       -
*    inclm       - inclination
*    mm          - mean anomaly
*    nm          - mean motion
*    perige      - perigee
*    pinvsq      -
*    psisq       -
*    qzms24      -
*    rtemsq      -
*    s1, s2, s3, s4, s5, s6, s7          -
*    sfour       -
*    ss1, ss2, ss3, ss4, ss5, ss6, ss7         -
*    sz1, sz2, sz3
*    sz11, sz12, sz13, sz21, sz22, sz23, sz31, sz32, sz33        -
*    tc          -
*    temp        -
*    temp1, temp2, temp3       -
*    tsi         -
*    xpidot      -
*    xhdot1      -
*    z1, z2, z3          -
*    z11, z12, z13, z21, z22, z23, z31, z32, z33         -
*
*  coupling      :
*    getgravconst-
*    initl       -
*    dscom       -
*    dpper       -
*    dsinit      -
*    sgp4        -
*
*  references    :
*    hoots, roehrich, norad spacetrack report #3 1980
*    hoots, norad spacetrack report #6 1986
*    hoots, schumacher and glover 2004
*    vallado, crawford, hujsak, kelso  2006
  ----------------------------------------------------------------------------*/

bool sgp4init
     (
       gravconsttype whichconst, char opsmode,   const int satn,     const float epoch,
       const float xbstar,  const float xecco, const float xargpo,
       const float xinclo,  const float xmo,   const float xno,
       const float xnodeo,  elsetrec& satrec
     )
{
     /* --------------------- local variables ------------------------ */
     float ao, ainv,   con42, cosio, sinio, cosio2, eccsq,
          omeosq, posq,   rp,     rteosq,
          cnodm , snodm , cosim , sinim , cosomm, sinomm, cc1sq ,
          cc2   , cc3   , coef  , coef1 , cosio4, day   , dndt  ,
          em    , emsq  , eeta  , etasq , gam   , argpm , nodem ,
          inclm , mm    , nm    , perige, pinvsq, psisq , qzms24,
          rtemsq, s1    , s2    , s3    , s4    , s5    , s6    ,
          s7    , sfour , ss1   , ss2   , ss3   , ss4   , ss5   ,
          ss6   , ss7   , sz1   , sz2   , sz3   , sz11  , sz12  ,
          sz13  , sz21  , sz22  , sz23  , sz31  , sz32  , sz33  ,
          tc    , temp  , temp1 , temp2 , temp3 , tsi   , xpidot,
          xhdot1, z1    , z2    , z3    , z11   , z12   , z13   ,
          z21   , z22   , z23   , z31   , z32   , z33,
          qzms2t, ss, j2, j3oj2, j4, x2o3, r[3], v[3],
          tumin, mu, radiusearthkm, xke, j3, delmotemp, qzms2ttemp, qzms24temp;

     /* ------------------------ initialization --------------------- */
     // sgp4fix divisor for divide by zero check on inclination
     // the old check used 1.0 + cosf(pi-1.0e-9), but then compared it to
     // 1.5 e-12, so the threshold was changed to 1.5e-12 for consistency
     const float temp4    =   1.5e-12;

     /* ----------- set all near earth variables to zero ------------ */
     satrec.isimp   = 0;   satrec.method = 'n'; satrec.aycof    = 0.0;
     satrec.con41   = 0.0; satrec.cc1    = 0.0; satrec.cc4      = 0.0;
     satrec.cc5     = 0.0; satrec.d2     = 0.0; satrec.d3       = 0.0;
     satrec.d4      = 0.0; satrec.delmo  = 0.0; satrec.eta      = 0.0;
     satrec.argpdot = 0.0; satrec.omgcof = 0.0; satrec.sinmao   = 0.0;
     satrec.t       = 0.0; satrec.t2cof  = 0.0; satrec.t3cof    = 0.0;
     satrec.t4cof   = 0.0; satrec.t5cof  = 0.0; satrec.x1mth2   = 0.0;
     satrec.x7thm1  = 0.0; satrec.mdot   = 0.0; satrec.nodedot  = 0.0;
     satrec.xlcof   = 0.0; satrec.xmcof  = 0.0; satrec.nodecf   = 0.0;

     /* ----------- set all deep space variables to zero ------------ */
     satrec.irez  = 0;   satrec.d2201 = 0.0; satrec.d2211 = 0.0;
     satrec.d3210 = 0.0; satrec.d3222 = 0.0; satrec.d4410 = 0.0;
     satrec.d4422 = 0.0; satrec.d5220 = 0.0; satrec.d5232 = 0.0;
     satrec.d5421 = 0.0; satrec.d5433 = 0.0; satrec.dedt  = 0.0;
     satrec.del1  = 0.0; satrec.del2  = 0.0; satrec.del3  = 0.0;
     satrec.didt  = 0.0; satrec.dmdt  = 0.0; satrec.dnodt = 0.0;
     satrec.domdt = 0.0; satrec.e3    = 0.0; satrec.ee2   = 0.0;
     satrec.peo   = 0.0; satrec.pgho  = 0.0; satrec.pho   = 0.0;
     satrec.pinco = 0.0; satrec.plo   = 0.0; satrec.se2   = 0.0;
     satrec.se3   = 0.0; satrec.sgh2  = 0.0; satrec.sgh3  = 0.0;
     satrec.sgh4  = 0.0; satrec.sh2   = 0.0; satrec.sh3   = 0.0;
     satrec.si2   = 0.0; satrec.si3   = 0.0; satrec.sl2   = 0.0;
     satrec.sl3   = 0.0; satrec.sl4   = 0.0; satrec.gsto  = 0.0;
     satrec.xfact = 0.0; satrec.xgh2  = 0.0; satrec.xgh3  = 0.0;
     satrec.xgh4  = 0.0; satrec.xh2   = 0.0; satrec.xh3   = 0.0;
     satrec.xi2   = 0.0; satrec.xi3   = 0.0; satrec.xl2   = 0.0;
     satrec.xl3   = 0.0; satrec.xl4   = 0.0; satrec.xlamo = 0.0;
     satrec.zmol  = 0.0; satrec.zmos  = 0.0; satrec.atime = 0.0;
     satrec.xli   = 0.0; satrec.xni   = 0.0;

     // sgp4fix - note the following variables are also passed directly via satrec.
     // it is possible to streamline the sgp4init call by deleting the "x"
     // variables, but the user would need to set the satrec.* values first. we
     // include the additional assignments in case twoline2rv is not used.
     satrec.bstar   = xbstar;
     satrec.ecco    = xecco;
     satrec.argpo   = xargpo;
     satrec.inclo   = xinclo;
     satrec.mo	    = xmo;
     satrec.no	    = xno;
     satrec.nodeo   = xnodeo;

     // sgp4fix add opsmode
     satrec.operationmode = opsmode;

     /* ------------------------ earth constants ----------------------- */
     // sgp4fix identify constants and allow alternate values
     getgravconst( whichconst, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );
     ss     = 78.0f / radiusearthkm + 1.0;
     // sgp4fix use multiply for speed instead of pow
     qzms2ttemp = (120.0f - 78.0f) / radiusearthkm;
     qzms2t = qzms2ttemp * qzms2ttemp * qzms2ttemp * qzms2ttemp;
     x2o3   =  2.0f / 3.0f;

     satrec.init = 'y';
     satrec.t	 = 0.0;

     initl
         (
           satn, whichconst, satrec.ecco, epoch, satrec.inclo, satrec.no, satrec.method,
           ainv, ao, satrec.con41, con42, cosio, cosio2, eccsq, omeosq,
           posq, rp, rteosq, sinio, satrec.gsto, satrec.operationmode
         );
     satrec.error = 0;

     // sgp4fix remove this check as it is unnecessary
     // the mrt check in sgp4 handles decaying satellite cases even if the starting
     // condition is below the surface of te earth
//     if (rp < 1.0f)
//       {
//         printf("# *** satn%d epoch elts sub-orbital ***\n", satn);
//         satrec.error = 5;
//       }

     if ((omeosq >= 0.0f ) || ( satrec.no >= 0.0f))
       {
         satrec.isimp = 0;
         if (rp < (220.0f / radiusearthkm + 1.0f))
             satrec.isimp = 1;
         sfour  = ss;
         qzms24 = qzms2t;
         perige = (rp - 1.0f) * radiusearthkm;

         /* - for perigees below 156 km, s and qoms2t are altered - */
         if (perige < 156.0f)
           {
             sfour = perige - 78.0;
             if (perige < 98.0f)
                 sfour = 20.0;
             // sgp4fix use multiply for speed instead of pow
             qzms24temp =  (120.0f - sfour) / radiusearthkm;
             qzms24 = qzms24temp * qzms24temp * qzms24temp * qzms24temp;
             sfour  = sfour / radiusearthkm + 1.0;
           }
         pinvsq = 1.0f / posq;

         tsi  = 1.0f / (ao - sfour);
         satrec.eta  = ao * satrec.ecco * tsi;
         etasq = satrec.eta * satrec.eta;
         eeta  = satrec.ecco * satrec.eta;
         psisq = fabsf(1.0f - etasq);
         coef  = qzms24 * powf(tsi, 4.0f);
         coef1 = coef / powf(psisq, 3.5);
         cc2   = coef1 * satrec.no * (ao * (1.0f + 1.5f * etasq + eeta *
                        (4.0f + etasq)) + 0.375f * j2 * tsi / psisq * satrec.con41 *
                        (8.0f + 3.0f * etasq * (8.0f + etasq)));
         satrec.cc1   = satrec.bstar * cc2;
         cc3   = 0.0;
         if (satrec.ecco > 1.0e-4f)
             cc3 = -2.0f * coef * tsi * j3oj2 * satrec.no * sinio / satrec.ecco;
         satrec.x1mth2 = 1.0f - cosio2;
         satrec.cc4    = 2.0f* satrec.no * coef1 * ao * omeosq *
                           (satrec.eta * (2.0f + 0.5f * etasq) + satrec.ecco *
                           (0.5f + 2.0f * etasq) - j2 * tsi / (ao * psisq) *
                           (-3.0f * satrec.con41 * (1.0f - 2.0f * eeta + etasq *
                           (1.5f - 0.5f * eeta)) + 0.75f * satrec.x1mth2 *
                           (2.0f * etasq - eeta * (1.0f + etasq)) * cosf(2.0f * satrec.argpo)));
         satrec.cc5 = 2.0f * coef1 * ao * omeosq * (1.0f + 2.75f *
                        (etasq + eeta) + eeta * etasq);
         cosio4 = cosio2 * cosio2;
         temp1  = 1.5f * j2 * pinvsq * satrec.no;
         temp2  = 0.5f * temp1 * j2 * pinvsq;
         temp3  = -0.46875f * j4 * pinvsq * pinvsq * satrec.no;
         satrec.mdot     = satrec.no + 0.5f * temp1 * rteosq * satrec.con41 + 0.0625f *
                            temp2 * rteosq * (13.0f - 78.0f * cosio2 + 137.0f * cosio4);
         satrec.argpdot  = -0.5f * temp1 * con42 + 0.0625f * temp2 *
                             (7.0f - 114.0f * cosio2 + 395.0f * cosio4) +
                             temp3 * (3.0f - 36.0f * cosio2 + 49.0f * cosio4);
         xhdot1            = -temp1 * cosio;
         satrec.nodedot = xhdot1 + (0.5f * temp2 * (4.0f - 19.0f * cosio2) +
                              2.0f * temp3 * (3.0f - 7.0f * cosio2)) * cosio;
         xpidot            =  satrec.argpdot+ satrec.nodedot;
         satrec.omgcof   = satrec.bstar * cc3 * cosf(satrec.argpo);
         satrec.xmcof    = 0.0;
         if (satrec.ecco > 1.0e-4f)
             satrec.xmcof = -x2o3 * coef * satrec.bstar / eeta;
         satrec.nodecf = 3.5f * omeosq * xhdot1 * satrec.cc1;
         satrec.t2cof   = 1.5f * satrec.cc1;
         // sgp4fix for divide by zero with xinco = 180 deg
         if (fabsf(cosio+1.0f) > 1.5e-12f)
             satrec.xlcof = -0.25f * j3oj2 * sinio * (3.0f + 5.0f * cosio) / (1.0f + cosio);
           else
             satrec.xlcof = -0.25f * j3oj2 * sinio * (3.0f + 5.0f * cosio) / temp4;
         satrec.aycof   = -0.5f * j3oj2 * sinio;
         // sgp4fix use multiply for speed instead of pow
         delmotemp = 1.0f + satrec.eta * cosf(satrec.mo);
         satrec.delmo   = delmotemp * delmotemp * delmotemp;
         satrec.sinmao  = sinf(satrec.mo);
         satrec.x7thm1  = 7.0f * cosio2 - 1.0f;

         /* --------------- deep space initialization ------------- */
         if ((2*pi / satrec.no) >= 225.0f)
           {
             satrec.method = 'd';
             satrec.isimp  = 1;
             tc    =  0.0f;
             inclm = satrec.inclo;

             dscom
                 (
                   epoch, satrec.ecco, satrec.argpo, tc, satrec.inclo, satrec.nodeo,
                   satrec.no, snodm, cnodm,  sinim, cosim,sinomm,     cosomm,
                   day, satrec.e3, satrec.ee2, em,         emsq, gam,
                   satrec.peo,  satrec.pgho,   satrec.pho, satrec.pinco,
                   satrec.plo,  rtemsq,        satrec.se2, satrec.se3,
                   satrec.sgh2, satrec.sgh3,   satrec.sgh4,
                   satrec.sh2,  satrec.sh3,    satrec.si2, satrec.si3,
                   satrec.sl2,  satrec.sl3,    satrec.sl4, s1, s2, s3, s4, s5,
                   s6,   s7,   ss1,  ss2,  ss3,  ss4,  ss5,  ss6,  ss7, sz1, sz2, sz3,
                   sz11, sz12, sz13, sz21, sz22, sz23, sz31, sz32, sz33,
                   satrec.xgh2, satrec.xgh3,   satrec.xgh4, satrec.xh2,
                   satrec.xh3,  satrec.xi2,    satrec.xi3,  satrec.xl2,
                   satrec.xl3,  satrec.xl4,    nm, z1, z2, z3, z11,
                   z12, z13, z21, z22, z23, z31, z32, z33,
                   satrec.zmol, satrec.zmos
                 );
             dpper
                 (
                   satrec.e3, satrec.ee2, satrec.peo, satrec.pgho,
                   satrec.pho, satrec.pinco, satrec.plo, satrec.se2,
                   satrec.se3, satrec.sgh2, satrec.sgh3, satrec.sgh4,
                   satrec.sh2, satrec.sh3, satrec.si2,  satrec.si3,
                   satrec.sl2, satrec.sl3, satrec.sl4,  satrec.t,
                   satrec.xgh2,satrec.xgh3,satrec.xgh4, satrec.xh2,
                   satrec.xh3, satrec.xi2, satrec.xi3,  satrec.xl2,
                   satrec.xl3, satrec.xl4, satrec.zmol, satrec.zmos, inclm, satrec.init,
                   satrec.ecco, satrec.inclo, satrec.nodeo, satrec.argpo, satrec.mo,
                   satrec.operationmode
                 );

             argpm  = 0.0f;
             nodem  = 0.0f;
             mm     = 0.0f;

             dsinit
                 (
                   whichconst,
                   cosim, emsq, satrec.argpo, s1, s2, s3, s4, s5, sinim, ss1, ss2, ss3, ss4,
                   ss5, sz1, sz3, sz11, sz13, sz21, sz23, sz31, sz33, satrec.t, tc,
                   satrec.gsto, satrec.mo, satrec.mdot, satrec.no, satrec.nodeo,
                   satrec.nodedot, xpidot, z1, z3, z11, z13, z21, z23, z31, z33,
                   satrec.ecco, eccsq, em, argpm, inclm, mm, nm, nodem,
                   satrec.irez,  satrec.atime,
                   satrec.d2201, satrec.d2211, satrec.d3210, satrec.d3222 ,
                   satrec.d4410, satrec.d4422, satrec.d5220, satrec.d5232,
                   satrec.d5421, satrec.d5433, satrec.dedt,  satrec.didt,
                   satrec.dmdt,  dndt,         satrec.dnodt, satrec.domdt ,
                   satrec.del1,  satrec.del2,  satrec.del3,  satrec.xfact,
                   satrec.xlamo, satrec.xli,   satrec.xni
                 );
           }

       /* ----------- set variables if not deep space ----------- */
       if (satrec.isimp != 1)
         {
           cc1sq          = satrec.cc1 * satrec.cc1;
           satrec.d2    = 4.0f * ao * tsi * cc1sq;
           temp           = satrec.d2 * tsi * satrec.cc1 / 3.0f;
           satrec.d3    = (17.0f * ao + sfour) * temp;
           satrec.d4    = 0.5f * temp * ao * tsi * (221.0f * ao + 31.0f * sfour) *
                            satrec.cc1;
           satrec.t3cof = satrec.d2 + 2.0f * cc1sq;
           satrec.t4cof = 0.25f * (3.0f * satrec.d3 + satrec.cc1 *
                            (12.0f * satrec.d2 + 10.0f * cc1sq));
           satrec.t5cof = 0.2f * (3.0f * satrec.d4 +
                            12.0f * satrec.cc1 * satrec.d3 +
                            6.0f * satrec.d2 * satrec.d2 +
                            15.0f * cc1sq * (2.0f * satrec.d2 + cc1sq));
         }
       } // if omeosq = 0 ...

       /* finally propogate to zero epoch to initialize all others. */
       // sgp4fix take out check to let satellites process until they are actually below earth surface
//       if(satrec.error == 0)
       sgp4(whichconst, satrec, 0.0, r, v);

       satrec.init = 'n';

//#include "debug6.cpp"
       //sgp4fix return boolean. satrec.error contains any error codes
       return true;
}  // end sgp4init

/*-----------------------------------------------------------------------------
*
*                             procedure sgp4
*
*  this procedure is the sgp4 prediction model from space command. this is an
*    updated and combined version of sgp4 and sdp4, which were originally
*    published separately in spacetrack report #3. this version follows the
*    methodology from the aiaa paper (2006) describing the history and
*    development of the code.
*
*  author        : david vallado                  719-573-2600   28 jun 2005
*
*  inputs        :
*    satrec	 - initialised structure from sgp4init() call.
*    tsince	 - time eince epoch (minutes)
*
*  outputs       :
*    r           - position vector                     km
*    v           - velocity                            km/sec
*  return code - non-zero on error.
*                   1 - mean elements, ecc >= 1.0 or ecc < -0.001 or a < 0.95 er
*                   2 - mean motion less than 0.0
*                   3 - pert elements, ecc < 0.0  or  ecc > 1.0
*                   4 - semi-latus rectum < 0.0
*                   5 - epoch elements are sub-orbital
*                   6 - satellite has decayed
*
*  locals        :
*    am          -
*    axnl, aynl        -
*    betal       -
*    cosim   , sinim   , cosomm  , sinomm  , cnod    , snod    , cos2u   ,
*    sin2u   , coseo1  , sineo1  , cosi    , sini    , cosip   , sinip   ,
*    cosisq  , cossu   , sinsu   , cosu    , sinu
*    delm        -
*    delomg      -
*    dndt        -
*    eccm        -
*    emsq        -
*    ecose       -
*    el2         -
*    eo1         -
*    eccp        -
*    esine       -
*    argpm       -
*    argpp       -
*    omgadf      -
*    pl          -
*    r           -
*    rtemsq      -
*    rdotl       -
*    rl          -
*    rvdot       -
*    rvdotl      -
*    su          -
*    t2  , t3   , t4    , tc
*    tem5, temp , temp1 , temp2  , tempa  , tempe  , templ
*    u   , ux   , uy    , uz     , vx     , vy     , vz
*    inclm       - inclination
*    mm          - mean anomaly
*    nm          - mean motion
*    nodem       - right asc of ascending node
*    xinc        -
*    xincp       -
*    xl          -
*    xlm         -
*    mp          -
*    xmdf        -
*    xmx         -
*    xmy         -
*    nodedf      -
*    xnode       -
*    nodep       -
*    np          -
*
*  coupling      :
*    getgravconst-
*    dpper
*    dpspace
*
*  references    :
*    hoots, roehrich, norad spacetrack report #3 1980
*    hoots, norad spacetrack report #6 1986
*    hoots, schumacher and glover 2004
*    vallado, crawford, hujsak, kelso  2006
  ----------------------------------------------------------------------------*/

bool sgp4
     (
       gravconsttype whichconst, elsetrec& satrec,  float tsince,
       float r[3],  float v[3]
     )
{
     float am   , axnl  , aynl , betal ,  cosim , cnod  ,
         cos2u, coseo1, cosi , cosip ,  cosisq, cossu , cosu,
         delm , delomg, em   , emsq  ,  ecose , el2   , eo1 ,
         ep   , esine , argpm, argpp ,  argpdf, pl,     mrt = 0.0,
         mvt  , rdotl , rl   , rvdot ,  rvdotl, sinim ,
         sin2u, sineo1, sini , sinip ,  sinsu , sinu  ,
         snod , su    , t2   , t3    ,  t4    , tem5  , temp,
         temp1, temp2 , tempa, tempe ,  templ , u     , ux  ,
         uy   , uz    , vx   , vy    ,  vz    , inclm , mm  ,
         nm   , nodem, xinc , xincp ,  xl    , xlm   , mp  ,
         xmdf , xmx   , xmy  , nodedf, xnode , nodep, tc  , dndt,
         twopi, x2o3  , j2   , j3    , tumin, j4 , xke   , j3oj2, radiusearthkm,
         mu, vkmpersec, delmtemp;
     int ktr;

     /* ------------------ set mathematical constants --------------- */
     // sgp4fix divisor for divide by zero check on inclination
     // the old check used 1.0 + cosf(pi-1.0e-9), but then compared it to
     // 1.5 e-12, so the threshold was changed to 1.5e-12 for consistency
     const float temp4 =   1.5e-12;
     twopi = 2.0f * pi;
     x2o3  = 2.0f / 3.0f;
     // sgp4fix identify constants and allow alternate values
     getgravconst( whichconst, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );
     vkmpersec     = radiusearthkm * xke/60.0f;

     /* --------------------- clear sgp4 error flag ----------------- */
     satrec.t     = tsince;
     satrec.error = 0;

     /* ------- update for secular gravity and atmospheric drag ----- */
     xmdf    = satrec.mo + satrec.mdot * satrec.t;
     argpdf  = satrec.argpo + satrec.argpdot * satrec.t;
     nodedf  = satrec.nodeo + satrec.nodedot * satrec.t;
     argpm   = argpdf;
     mm      = xmdf;
     t2      = satrec.t * satrec.t;
     nodem   = nodedf + satrec.nodecf * t2;
     tempa   = 1.0f - satrec.cc1 * satrec.t;
     tempe   = satrec.bstar * satrec.cc4 * satrec.t;
     templ   = satrec.t2cof * t2;

     if (satrec.isimp != 1)
       {
         delomg = satrec.omgcof * satrec.t;
         // sgp4fix use mutliply for speed instead of pow
         delmtemp =  1.0f + satrec.eta * cosf(xmdf);
         delm   = satrec.xmcof *
                  (delmtemp * delmtemp * delmtemp -
                  satrec.delmo);
         temp   = delomg + delm;
         mm     = xmdf + temp;
         argpm  = argpdf - temp;
         t3     = t2 * satrec.t;
         t4     = t3 * satrec.t;
         tempa  = tempa - satrec.d2 * t2 - satrec.d3 * t3 -
                          satrec.d4 * t4;
         tempe  = tempe + satrec.bstar * satrec.cc5 * (sinf(mm) -
                          satrec.sinmao);
         templ  = templ + satrec.t3cof * t3 + t4 * (satrec.t4cof +
                          satrec.t * satrec.t5cof);
       }

     nm    = satrec.no;
     em    = satrec.ecco;
     inclm = satrec.inclo;
     if (satrec.method == 'd')
       {
         tc = satrec.t;
         dspace
             (
               satrec.irez,
               satrec.d2201, satrec.d2211, satrec.d3210,
               satrec.d3222, satrec.d4410, satrec.d4422,
               satrec.d5220, satrec.d5232, satrec.d5421,
               satrec.d5433, satrec.dedt,  satrec.del1,
               satrec.del2,  satrec.del3,  satrec.didt,
               satrec.dmdt,  satrec.dnodt, satrec.domdt,
               satrec.argpo, satrec.argpdot, satrec.t, tc,
               satrec.gsto, satrec.xfact, satrec.xlamo,
               satrec.no, satrec.atime,
               em, argpm, inclm, satrec.xli, mm, satrec.xni,
               nodem, dndt, nm
             );
       } // if method = d

     if (nm <= 0.0f)
       {
//         printf("# error nm %f\n", nm);
         satrec.error = 2;
         // sgp4fix add return
         return false;
       }
     am = powf((xke / nm),x2o3) * tempa * tempa;
     nm = xke / powf(am, 1.5f);
     em = em - tempe;

     // fix tolerance for error recognition
     // sgp4fix am is fixed from the previous nm check
     if ((em >= 1.0f) || (em < -0.001f)/* || (am < 0.95)*/ )
       {
//         printf("# error em %f\n", em);
         satrec.error = 1;
         // sgp4fix to return if there is an error in eccentricity
         return false;
       }
     // sgp4fix fix tolerance to avoid a divide by zero
     if (em < 1.0e-6f)
         em  = 1.0e-6;
     mm     = mm + satrec.no * templ;
     xlm    = mm + argpm + nodem;
     emsq   = em * em;
     temp   = 1.0f - emsq;

     nodem  = fmodf(nodem, twopi);
     argpm  = fmodf(argpm, twopi);
     xlm    = fmodf(xlm, twopi);
     mm     = fmodf(xlm - argpm - nodem, twopi);

     /* ----------------- compute extra mean quantities ------------- */
     sinim = sinf(inclm);
     cosim = cosf(inclm);

     /* -------------------- add lunar-solar periodics -------------- */
     ep     = em;
     xincp  = inclm;
     argpp  = argpm;
     nodep  = nodem;
     mp     = mm;
     sinip  = sinim;
     cosip  = cosim;
     if (satrec.method == 'd')
       {
         dpper
             (
               satrec.e3,   satrec.ee2,  satrec.peo,
               satrec.pgho, satrec.pho,  satrec.pinco,
               satrec.plo,  satrec.se2,  satrec.se3,
               satrec.sgh2, satrec.sgh3, satrec.sgh4,
               satrec.sh2,  satrec.sh3,  satrec.si2,
               satrec.si3,  satrec.sl2,  satrec.sl3,
               satrec.sl4,  satrec.t,    satrec.xgh2,
               satrec.xgh3, satrec.xgh4, satrec.xh2,
               satrec.xh3,  satrec.xi2,  satrec.xi3,
               satrec.xl2,  satrec.xl3,  satrec.xl4,
               satrec.zmol, satrec.zmos, satrec.inclo,
               'n', ep, xincp, nodep, argpp, mp, satrec.operationmode
             );
         if (xincp < 0.0f)
           {
             xincp  = -xincp;
             nodep = nodep + pi;
             argpp  = argpp - pi;
           }
         if ((ep < 0.0f ) || ( ep > 1.0f))
           {
//            printf("# error ep %f\n", ep);
             satrec.error = 3;
             // sgp4fix add return
             return false;
           }
       } // if method = d

     /* -------------------- long period periodics ------------------ */
     if (satrec.method == 'd')
       {
         sinip =  sinf(xincp);
         cosip =  cosf(xincp);
         satrec.aycof = -0.5f*j3oj2*sinip;
         // sgp4fix for divide by zero for xincp = 180 deg
         if (fabsf(cosip+1.0f) > 1.5e-12f)
             satrec.xlcof = -0.25f * j3oj2 * sinip * (3.0f + 5.0f * cosip) / (1.0f + cosip);
           else
             satrec.xlcof = -0.25f * j3oj2 * sinip * (3.0f + 5.0f * cosip) / temp4;
       }
     axnl = ep * cosf(argpp);
     temp = 1.0f / (am * (1.0f - ep * ep));
     aynl = ep* sinf(argpp) + temp * satrec.aycof;
     xl   = mp + argpp + nodep + temp * satrec.xlcof * axnl;

     /* --------------------- solve kepler's equation --------------- */
     u    = fmodf(xl - nodep, twopi);
     eo1  = u;
     tem5 = 9999.9;
     ktr = 1;
     //   sgp4fix for kepler iteration
     //   the following iteration needs better limits on corrections
     while (( fabsf(tem5) >= 1.0e-12f) && (ktr <= 10) )
       {
         sineo1 = sinf(eo1);
         coseo1 = cosf(eo1);
         tem5   = 1.0f - coseo1 * axnl - sineo1 * aynl;
         tem5   = (u - aynl * coseo1 + axnl * sineo1 - eo1) / tem5;
         if(fabsf(tem5) >= 0.95f)
             tem5 = tem5 > 0.0f ? 0.95f : -0.95f;
         eo1    = eo1 + tem5;
         ktr = ktr + 1;
       }

     /* ------------- short period preliminary quantities ----------- */
     ecose = axnl*coseo1 + aynl*sineo1;
     esine = axnl*sineo1 - aynl*coseo1;
     el2   = axnl*axnl + aynl*aynl;
     pl    = am*(1.0f-el2);
     if (pl < 0.0f)
       {
//         printf("# error pl %f\n", pl);
         satrec.error = 4;
         // sgp4fix add return
         return false;
       }
       else
       {
         rl     = am * (1.0f - ecose);
         rdotl  = sqrtf(am) * esine/rl;
         rvdotl = sqrtf(pl) / rl;
         betal  = sqrtf(1.0f - el2);
         temp   = esine / (1.0f + betal);
         sinu   = am / rl * (sineo1 - aynl - axnl * temp);
         cosu   = am / rl * (coseo1 - axnl + aynl * temp);
         su     = atan2f(sinu, cosu);
         sin2u  = (cosu + cosu) * sinu;
         cos2u  = 1.0f - 2.0f * sinu * sinu;
         temp   = 1.0f / pl;
         temp1  = 0.5f * j2 * temp;
         temp2  = temp1 * temp;

         /* -------------- update for short period periodics ------------ */
         if (satrec.method == 'd')
           {
             cosisq                 = cosip * cosip;
             satrec.con41  = 3.0f*cosisq - 1.0f;
             satrec.x1mth2 = 1.0f - cosisq;
             satrec.x7thm1 = 7.0f*cosisq - 1.0f;
           }
         mrt   = rl * (1.0f - 1.5f * temp2 * betal * satrec.con41) +
                 0.5f * temp1 * satrec.x1mth2 * cos2u;
         su    = su - 0.25f * temp2 * satrec.x7thm1 * sin2u;
         xnode = nodep + 1.5f * temp2 * cosip * sin2u;
         xinc  = xincp + 1.5f * temp2 * cosip * sinip * cos2u;
         mvt   = rdotl - nm * temp1 * satrec.x1mth2 * sin2u / xke;
         rvdot = rvdotl + nm * temp1 * (satrec.x1mth2 * cos2u +
                 1.5f * satrec.con41) / xke;

         /* --------------------- orientation vectors ------------------- */
         sinsu =  sinf(su);
         cossu =  cosf(su);
         snod  =  sinf(xnode);
         cnod  =  cosf(xnode);
         sini  =  sinf(xinc);
         cosi  =  cosf(xinc);
         xmx   = -snod * cosi;
         xmy   =  cnod * cosi;
         ux    =  xmx * sinsu + cnod * cossu;
         uy    =  xmy * sinsu + snod * cossu;
         uz    =  sini * sinsu;
         vx    =  xmx * cossu - cnod * sinsu;
         vy    =  xmy * cossu - snod * sinsu;
         vz    =  sini * cossu;

         /* --------- position and velocity (in km and km/sec) ---------- */
         r[0] = (mrt * ux)* radiusearthkm;
         r[1] = (mrt * uy)* radiusearthkm;
         r[2] = (mrt * uz)* radiusearthkm;
         v[0] = (mvt * ux + rvdot * vx) * vkmpersec;
         v[1] = (mvt * uy + rvdot * vy) * vkmpersec;
         v[2] = (mvt * uz + rvdot * vz) * vkmpersec;
       }  // if pl > 0

     // sgp4fix for decaying satellites
     if (mrt < 1.0f)
       {
//         printf("# decay condition %11.6f \n",mrt);
         satrec.error = 6;
         return false;
       }

//#include "debug7.cpp"
     return true;
}  // end sgp4


/* -----------------------------------------------------------------------------
*
*                           function gstime
*
*  this function finds the greenwich sidereal time.
*
*  author        : david vallado                  719-573-2600    1 mar 2001
*
*  inputs          description                    range / units
*    jdut1       - julian date in ut1             days from 4713 bc
*
*  outputs       :
*    gstime      - greenwich sidereal time        0 to 2pi rad
*
*  locals        :
*    temp        - temporary variable for floats   rad
*    tut1        - julian centuries from the
*                  jan 1, 2000 12 h epoch (ut1)
*
*  coupling      :
*    none
*
*  references    :
*    vallado       2004, 191, eq 3-45
* --------------------------------------------------------------------------- */

float  gstime
        (
          float jdut1
        )
   {
     const float twopi = 2.0f * pi;
     const float deg2rad = pi / 180.0f;
     float       temp, tut1;

     tut1 = (jdut1 - 2451545.0f) / 36525.0f;
     temp = -6.2e-6f* tut1 * tut1 * tut1 + 0.093104f * tut1 * tut1 +
             (876600.0f*3600 + 8640184.812866f) * tut1 + 67310.54841f;  // sec
     temp = fmodf(temp * deg2rad / 240.0, twopi); //360/86400 = 1/240, to deg, to rad

     // ------------------------ check quadrants ---------------------
     if (temp < 0.0f)
         temp += twopi;

     return temp;
   }  // end gstime


/* -----------------------------------------------------------------------------
*
*                           function getgravconst
*
*  this function gets constants for the propagator. note that mu is identified to
*    facilitiate comparisons with newer models. the common useage is wgs72.
*
*  author        : david vallado                  719-573-2600   21 jul 2006
*
*  inputs        :
*    whichconst  - which set of constants to use  wgs72old, wgs72, wgs84
*
*  outputs       :
*    tumin       - minutes in one time unit
*    mu          - earth gravitational parameter
*    radiusearthkm - radius of the earth in km
*    xke         - reciprocal of tumin
*    j2, j3, j4  - un-normalized zonal harmonic values
*    j3oj2       - j3 divided by j2
*
*  locals        :
*
*  coupling      :
*    none
*
*  references    :
*    norad spacetrack report #3
*    vallado, crawford, hujsak, kelso  2006
  --------------------------------------------------------------------------- */

void getgravconst
     (
      gravconsttype whichconst,
      float& tumin,
      float& mu,
      float& radiusearthkm,
      float& xke,
      float& j2,
      float& j3,
      float& j4,
      float& j3oj2
     )
     {

       switch (whichconst)
         {
           // -- wgs-72 low precision str#3 constants --
           case wgs72old:
           mu     = 398600.79964f;        // in km3 / s2
           radiusearthkm = 6378.135f;     // km
           xke    = 0.0743669161f;
           tumin  = 1.0f / xke;
           j2     =   0.001082616f;
           j3     =  -0.00000253881f;
           j4     =  -0.00000165597f;
           j3oj2  =  j3 / j2;
         break;
           // ------------ wgs-72 constants ------------
           case wgs72:
           mu     = 398600.8f;            // in km3 / s2
           radiusearthkm = 6378.135f;     // km
           xke    = 60.0f / sqrtf(radiusearthkm*radiusearthkm*radiusearthkm/mu);
           tumin  = 1.0f / xke;
           j2     =   0.001082616f;
           j3     =  -0.00000253881f;
           j4     =  -0.00000165597f;
           j3oj2  =  j3 / j2;
         break;
           case wgs84:
           // ------------ wgs-84 constants ------------
           mu     = 398600.5;            // in km3 / s2
           radiusearthkm = 6378.137;     // km
           xke    = 60.0f / sqrtf(radiusearthkm*radiusearthkm*radiusearthkm/mu);
           tumin  = 1.0f / xke;
           j2     =   0.00108262998905f;
           j3     =  -0.00000253215306f;
           j4     =  -0.00000161098761f;
           j3oj2  =  j3 / j2;
         break;
         default:
           fprintf(stderr,"unknown gravity option (%d)\n",whichconst);
         break;
         }

     }   // end getgravconst





