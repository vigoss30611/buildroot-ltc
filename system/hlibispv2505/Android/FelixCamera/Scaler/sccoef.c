/**************************************************************************
 Name         : ScaleCoef.c
 Title        : ScaleCoef
 Author       : Paul Buxton
 Created      : 28-9-99

 Copyright    : 1999 by Imagination Technologies Limited. All rights reserved.
              : No part of this software, either material or conceptual
              : may be copied or distributed, transmitted, transcribed,
              : stored in a retrieval system or translated into any
              : human or computer language in any form by any means,
              : electronic, mechanical, manual or other-wise, or
              : disclosed to third parties without the express written
              : permission of VideoLogic Limited, Unit 8, HomePark
              : Industrial Estate, King's Langley, Hertfordshire,
              : WD4 8LZ, U.K.

 Description  : Scale Factor Calculation and Coeficcient Generation.

 Platform     : Independent
 Version      : $Revision: 1.2 $
 $Log: ScaleCoef.c

 **************************************************************************/


/* This file abstracts the maths required to generate the coefficients
for scaling


The formula basically creates a discret weighted Sinc function.

table[t][i] = A - (1-A)*Cos(PI*Scale*(t+i/I))/(T/2))
x = PI*Scale*(T/2 -t -i/I + 0.000001)
sincfunc = sin(x)/x
table[t][i] *= sincfunc

where:

table[t][i] is the coeff for current tap and interpolation point. t is the current tap.
i is the current interpolation point.
I is the total interpolation points per tap. T is the total number of taps in the filter
Scale is the scaling factor clamped to [0.0, 1.0] A is a mystical number

Simulations show that the following setting are best:

A = 0.54
I = 8
T = 8 for horizontal scale, T = 4 for vertical scale

Note that the above function has reflective symmetry.


*/



typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;

#define TESTRIG
#define DEBUG
#include "sccoef.h"
#include <stdio.h>
#include <math.h>

__inline float func(float fi,float ft, float fI,float fT,float fScale);
#define Index(X,Y) ((X)+((Y)*T))


/* Due to the nature of the function we will only ever want to calculate the first half of the taps +the middl one( is this really
a tap. as the seconda half are dervied from the firs half as the function is symetrical */
void CalcCoefs(DWORD dwSrc,DWORD dwDest,BYTE Table[MAXTAP][MAXINTPT],DWORD I, DWORD T, DWORD Tdiv)
{
    float fScale;
    DWORD i,t;
    float flTable[MAXTAP][MAXINTPT]={0.0};
    long nTotal;
    float ftotal;
    long val;
    float fI, fT;
    FILE *fp;
    int mT,mI; // mirrored/middle Values for I and T
    int startoffset=0;
    fI = (float)I;
    fT = (float)T;
    fScale=((float)dwDest/(float)dwSrc);
    if (fScale>1.0f)
    {
        fScale=1.0f;
    }
    for(i=0;i<I;i++)
    {
        for(t=0;t<T;t++)
        {
            flTable[t][i]=0.0;
        }
    }

    for(i=0;i<I;i++)
    {
        for(t=0;t<T/Tdiv;t++)
        {
            flTable[t+(T-T/Tdiv)/2][i]=func((float)i,(float)t,fI,fT/Tdiv,fScale);
        }
    }

    if(T>2)
    {
        for(t=0;t<((T/2)+(T%2));t++)
        {
            for(i=0;i<I;i++)
            {
                // copy the table around the centrepoint
                mT=((T-1)-t)+(I-i)/I;
                mI=(I-i)%I;
                if((mI<I) && (mT<T) && ((t<((T/2)+(T%2)-1)) || ((I-i)>((T%2)*(I/2)))))
                {
                    flTable[mT][mI]=flTable[t][i];
                }
            }
        }

        // the middle value
        mT = T/2;
        if ((T%2) != 0) {
            mI = I/2;
        } else {
            mI = 0;
        }
        flTable[mT][mI]=func((float)mI,(float)mT,fI,fT,fScale);
    }

    // normalize this interpolation point, and convert to 2.6 format trucating the result
    for(i=0;i<I;i++)
    {
        nTotal=0;
        for(ftotal=0,t=0;t<T;t++)
        {
            ftotal+=flTable[t][i];
        }
        for(t=0;t<T;t++)
        {
            val=(long)((flTable[t][i]*64.0f)/ftotal);
            Table[t][i]=(BYTE)val;
            nTotal+=val;
        }
        if((i<=(I/2)) || (T<=2)) // normalize any floating point errors
        {
            nTotal-=64;
            if((i==(I/2)) && (T > 2))
            {
                nTotal/=2;
            }
            Table[0][i]=(BYTE)(Table[0][i]-(BYTE)nTotal); // subtract the error from the I Point in the first tap ( this will not get mirrored, as it would go off the end).

        }
    }

    // copy the normalised table around the centrepoint
    if(T>2)
    {
        for(t=0;t<((T/2)+(T%2));t++)
        {
            for(i=0;i<I;i++)
            {
                mT=((T-1)-t)+(I-i)/I;
                mI=(I-i)%I;
                if((mI<I) && (mT<T) && ((t<((T/2)+(T%2)-1)) || ((I-i)>((T%2)*(I/2)))))
                {
                    Table[mT][mI]=Table[t][i];
                }
            }
        }
    }

    for(t=0;t<T;t++)
    {
        for(i=0;i<I;i++)
        {
//            printf("i=%d, t=%d, coeff=%d\n", (int)i, (int)t, Table[t][i]);
        }
    }

}


float Sinc(float x)
{
    if (x == 0) {
        return 1.0f;
    } else {
        return sin(x)/x;
    }
}


// polynomial approximation of the zeroth order modified Bessel function
// from the Numerical Recipes in C p. 237

float bessi0(float x)
{
    float ax,ans;
    float y;

    ax=fabs(x);
    if (ax < 3.75)
    {
        y=x/3.75;
        y*=y;
        ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
            +y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
    }
    else
    {
        y=3.75/ax;
        ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
            +y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
            +y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
            +y*0.392377e-2))))))));
    }
    return ans;
}


/* Izero() computes the 0th order modified bessel function of the first kind.
 *    (Needed to compute Kaiser window). */
float Izero(x)
float x;
{
   float sum, u, halfx, temp;
   float n;

   sum = u = n = 1.0;
   halfx = (float)x/2.09f;
   do {
      temp = halfx/(float)n;
      n += 1;
      temp *= temp;
      u *= temp;
      sum += u;
      } while (u >= IzeroEPSILON*sum);
   return(sum);
}


// the Mitchell filter function
float MitchellSample(float x) {
  // Take the absolute value of x
  if (x < 0) x = 0 - x;
  // Now compute the value
  if (x < 1)
    return (7./6 * x*x*x - 2. * x*x + 8./9);
  else if (x < 2)
    return (-7./18 * x*x*x + 2. * x*x - 10./3 * x + 16./9);
  else
    return 0;
}


__inline float func(float fi,float ft, float fI,float fT,float fScale)
{
    float sincfunc,x;
    float tempval;
    float gaussop;
    float gaussval;
    float PI;
    float A;
    float IBeta;

    PI=3.1415926535897f;
    A=0.54f;

#if OLD_COEFFS
    /**************************************/
    /* old Hamming windowed-sinc function */
    /**************************************/

    // Hamming window
    tempval=A - (1-A)*cos(2*PI*fScale*(ft + fi/fI)/fT);

    // Sinc function
    if ((fT/2 - ft - fi/fI) == 0) {
        sincfunc=1;
    } else {
        x=PI*fScale*(fT/2 - (ft + fi/fI));
        sincfunc=sin(x)/x;
    }

    return sincfunc*tempval;
#else
    /*************************************/
    /* new Kaiser windowed-sinc function */
    /*************************************/

    /* Kaiser window */
    x=((ft*fI + fi)-(fT*fI/2))/(fT*fI/2);

#if 1
    IBeta = 1.0f/(bessi0(Beta));
    tempval = bessi0(Beta*sqrt(1.0f-x*x)) * IBeta;
#else
    IBeta = 1.0f/(Izero(Beta));
    tempval = Izero(Beta*sqrt(1.0f-x*x)) * IBeta;
#endif

    // Sinc function
//    x=0.9f*fScale*PI*(fT/2 - (ft + fi/fI));
    x=Lanczos*fScale*PI*(fT/2 - (ft + fi/fI));
    sincfunc=Sinc(x);
/*    if ((fT/2 - ft - fi/fI) == 0) {
        sincfunc = 1.0f;
    } else {
        sincfunc=sin(x)/x;
    }
*/
    // Gaussian window, G(x)=exp((-2*x^2)/(r^2)), where r = radius
    // To remove negative lobes from sinc function which cause ripple in the output
    if ((fT/2 - ft - fi/fI) == 0) {
        gaussval = 1.0f;
    } else {
        gaussop = (-2*x*x);
        gaussval = exp(gaussop);
    }

//    printf("i=%d, t=%d, sincfunc=%f, tempval=%f\n", (int)fi, (int)ft, sincfunc*tempval, tempval);

//    return MitchellSample(x);
    return sincfunc*tempval;
//    return sincfunc*tempval*gaussval;
//    return tempval*gaussval;
#endif

}
