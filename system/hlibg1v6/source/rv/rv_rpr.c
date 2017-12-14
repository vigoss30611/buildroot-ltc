/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract :  Header decoding
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rv_rpr.c,v $
--  $Date: 2010/12/14 11:03:58 $
--  $Revision: 1.3 $
-
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "rv_rpr.h"

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/
static void Up2x( u8 * inp, u8 *inpChr, u32 inpW, u32 inpWfrm, u32 inpH, u32 inpHfrm,
           u8 * outp, u8 *outpChr, u32 outpW, u32 outpWfrm, u32 outpH, u32 outpHfrm,
           u32 round );
static void Down2x( u8 * inp, u8 *inpChr, u32 inpW, u32 inpWfrm, u32 inpH, u32 inpHfrm,
             u8 * outp, u8 *outpChr, u32 outpW, u32 outpWfrm, u32 outpH, u32 outpHfrm,
             u32 round );
static void ResampleY( u32 inpW, u32 outpW, u32 outpWfrm, u32 outpH, u8 *pDestRow, 
                  u8 **pSrcRowBuf, u16*srcCol, u8*coeffY, u8*coeffX,
                  u32 leftEdge, u32 useRightEdge, u32 rightEdge, u32 rndVal );
static void ResampleChr( u32 inpW, u32 outpW, u32 outpWfrm, u32 outpH, u8 *pDestRow, 
                  u8 **pSrcRowBuf, u16*srcCol, u8*coeffY, u8*coeffX,
                  u32 leftEdge, u32 useRightEdge, u32 rightEdge, u32 rndVal );
static void CoeffTables( u32 inpW, u32 inpH, u32 outpW, u32 outpH, 
                  u32 lumaCoeffs, u8 *inp, u32 inpWfrm,
                  u16 *pSrcCol, u8 **pSrcRowBuf,
                  u8 *pCoeffX, u8 *pCoeffY,
                  u32 *pLeftEdge, u32 *pUseRightEdge, u32* pRightEdge);
static void RvResample( u8 * inp, u8 *inpChr, u32 inpW, u32 inpWfrm, u32 inpH, 
                    u32 inpHfrm, u8 * outp, u8 *outpChr, u32 outpW, 
                    u32 outpWfrm, u32 outpH, u32 outpHfrm, 
                    u32 round, u8 *workMemory, u32 tiledMode );

/*------------------------------------------------------------------------------

    Function name: ResampleY

        Functional description:
            Resample luma component using bilinear algorithm.

        Inputs:

        Outputs: 
            pDestRow        Resampled luma image

        Returns: 
            HANTRO_OK       Decoding successful
            HANTRO_NOK      Error in decoding
            END_OF_STREAM   End of stream encountered

------------------------------------------------------------------------------*/
void ResampleY( u32 inpW, u32 outpW, u32 outpWfrm, u32 outpH, u8 *pDestRow, 
                  u8 **pSrcRowBuf, u16*srcCol, u8*coeffY, u8*coeffX,
                  u32 leftEdge, u32 useRightEdge, u32 rightEdge, u32 rndVal )
{
    u32 col;
    u8 *pSrcRow0;
    u8 *pSrcRow1;
    u32 midPels;
    u32 rightPels;

    /* Calculate amount of pixels without overfill and with right edge 
     * overfill */
    if(useRightEdge == 0)
    {
        midPels = outpW - leftEdge;
        rightPels = 0;
    }
    else
    {
        midPels = rightEdge - leftEdge;
        rightPels = outpW - rightEdge;
    }

    inpW--; /* We don't need original inpW for anything, but right edge overfill
             * requires inpW-1 */
    while(outpH--)
    {
        u32 coeffY0, coeffY1;
        u8 * pDest = pDestRow;
        u16 * pSrcCol = srcCol;
        u8 * pCoeffXPtr;
        coeffY1 = *coeffY++;
        coeffY0 = 16-coeffY1;
        pSrcRow0 = *pSrcRowBuf++;
        pSrcRow1 = *pSrcRowBuf++;

        /* First pel(s) -- there may be overfill required on left edge, this
         * part of code takes that into account */
        if(leftEdge)
        {
            u32 A,C, pel;
            A = *pSrcRow0;
            C = *pSrcRow1;
            pel = (coeffY0*16*A +
                   coeffY1*16*C +
                   rndVal) / 256;
            col = leftEdge;
            while(col--)
                *pDest++ = pel;
        }

        /* Middle pels on a row. By definition, no overfill is required */
        pCoeffXPtr = coeffX;
        col = midPels;
        while(col--)
        {
            u32 c0;
            u32 A, B, C, D;
            u32 pel;
            u32 top, bot;
            u32 tmp;
            u32 coeffX0, coeffX1;

            coeffX1 = *pCoeffXPtr++; 
            coeffX0 = 16-coeffX1; 
            c0 = *pSrcCol++;

            /* Get input pixels */
            A = pSrcRow0[c0];
            C = pSrcRow1[c0];
            c0++;
            B = pSrcRow0[c0];
            D = pSrcRow1[c0];
            /* Weigh source rows */
            top = coeffX0*A + coeffX1*B;
            bot = coeffX0*C + coeffX1*D;
            /* Combine rows and round */
            tmp = coeffY0*top + coeffY1*bot;
            pel = ( tmp + rndVal) / 256;
            *pDest++ = pel;
        }	/* for col */

        /* If right-edge overfill is required, this part takes care of that */
        if(useRightEdge)
        {
            u32 A, C, pel;
            A = pSrcRow0[inpW];
            C = pSrcRow1[inpW];
            pel = (coeffY0*16*A +
                   coeffY1*16*C +
                   rndVal) / 256;
            col = rightPels;
            while(col--)
                *pDest++ = pel;
        }
        pDestRow += outpWfrm;

    }	/* for row */

}

/*------------------------------------------------------------------------------

    Function name: ResampleChr

        Functional description:
            Resample semi-planar chroma components using bilinear algorithm.

        Inputs:

        Outputs: 
            pDestRow        Resampled chroma image

        Returns: 
            HANTRO_OK       Decoding successful
            HANTRO_NOK      Error in decoding
            END_OF_STREAM   End of stream encountered

------------------------------------------------------------------------------*/
void ResampleChr( u32 inpW, u32 outpW, u32 outpWfrm, u32 outpH, u8 *pDestRow, 
                  u8 **pSrcRowBuf, u16*srcCol, u8*coeffY, u8*coeffX,
                  u32 leftEdge, u32 useRightEdge, u32 rightEdge, u32 rndVal )
{
    u32 col;
    u8 *pSrcRow0;
    u8 *pSrcRow1;
    u32 midPels;
    u32 rightPels;

    /* Calculate amount of pixels without overfill and with right edge 
     * overfill */
    if(useRightEdge == 0)
    {
        midPels = outpW - leftEdge;
        rightPels = 0;
    }
    else
    {
        midPels = rightEdge - leftEdge;
        rightPels = outpW - rightEdge;
    }

    inpW -= 2; /* We don't need original inpW for anything, but right edge 
                * overfill requires inpW-1 */
    while(outpH--)
    {
        u32 coeffY0, coeffY1;
        u8 * pDest = pDestRow;
        u16 * pSrcCol = srcCol;
        u8 * pCoeffXPtr;
        coeffY1 = *coeffY++;
        coeffY0 = 16-coeffY1;
        pSrcRow0 = (u8*)*pSrcRowBuf++;
        pSrcRow1 = (u8*)*pSrcRowBuf++;

        /* First pel(s) -- there may be overfill required on left edge, this
         * part of code takes that into account */
        if(leftEdge)
        {
            u32 A, B, C, D, pelCb, pelCr;
            A = pSrcRow0[0];    /* Cb */
            B = pSrcRow0[1];    /* Cr */
            C = pSrcRow1[0];    /* Cb */
            D = pSrcRow1[1];    /* Cr */
            pelCb = (coeffY0*(16*A) +
                     coeffY1*(16*C) +
                     rndVal) / 256;
            pelCr = (coeffY0*(16*B) +
                     coeffY1*(16*D) +
                     rndVal) / 256;
            col = leftEdge;
            while(col--)
            {
                *pDest++ = pelCb;
                *pDest++ = pelCr;
            }
        }

        /* Middle pels on a row. By definition, no overfill is required */
        pCoeffXPtr = coeffX;
        col = midPels;
        while(col--)
        {
            u32 c0;
            u32 A, B, C, D, E, F;
            u32 pelCb, pelCr;
            u32 top, bot;
            u32 tmp;
            u32 coeffX0, coeffX1;

            coeffX1 = *pCoeffXPtr++; 
            coeffX0 = 16-coeffX1; 
            c0 = *pSrcCol++;

            /* Get Cb input pixels */
            A = pSrcRow0[c0];
            C = pSrcRow1[c0];
            c0 += 2;
            B = pSrcRow0[c0];
            D = pSrcRow1[c0];
            /* Weigh source rows */
            top = coeffX0*A + coeffX1*B;
            bot = coeffX0*C + coeffX1*D;
            /* Combine rows and round */
            tmp = coeffY0*top + coeffY1*bot;
            pelCb = ( tmp + rndVal) / 256;

            /* Get Cr input pixels */
            c0--;
            E = pSrcRow0[c0];
            F = pSrcRow1[c0];
            c0 += 2;
            A = pSrcRow0[c0];
            B = pSrcRow1[c0];
            /* Weigh source rows */
            top = coeffX0*E + coeffX1*A;
            bot = coeffX0*F + coeffX1*B;
            /* Combine rows and round */
            tmp = coeffY0*top + coeffY1*bot;
            pelCr = ( tmp + rndVal) / 256;

            *pDest++ = pelCb;
            *pDest++ = pelCr;
        }	/* for col */

        /* If right-edge overfill is required, this part takes care of that */
        if(rightPels)
        {
            u32 A, B, C, D, pelCb, pelCr;
            A = pSrcRow0[inpW];
            B = pSrcRow0[inpW+1];
            C = pSrcRow1[inpW];
            D = pSrcRow1[inpW+1];
            pelCb = (coeffY0*(16*A) +
                     coeffY1*(16*C) +
                     rndVal) / 256;
            pelCr = (coeffY0*(16*B) +
                     coeffY1*(16*D) +
                     rndVal) / 256;
            col = rightPels;
            while(col--)
            {
                *pDest++ = pelCb;
                *pDest++ = pelCr;
            }
        }
        pDestRow += outpWfrm;

    }	/* for row */

}

/*------------------------------------------------------------------------------

    Function name: CoeffTables

        Functional description:
            Create coefficient and row look-up tables for bilinear algotihm.
            Also evaluates amounts of overfill required per row.

        Inputs:

        Outputs: 
            pSrcCol         Array translating output columns to source columns.
            pSrcRowBuf      Array translating output rows to source rows.
            pCoeffX         X coefficients per output column.
            pCoeffY         Y coefficients per output column.

        Returns: 
            HANTRO_OK       Decoding successful
            HANTRO_NOK      Error in decoding
            END_OF_STREAM   End of stream encountered

------------------------------------------------------------------------------*/
void CoeffTables( u32 inpW, u32 inpH, u32 outpW, u32 outpH, 
                  u32 lumaCoeffs, u8 *inp, u32 inpWfrm,
                  u16 *pSrcCol, u8 **pSrcRowBuf,
                  u8 *pCoeffX, u8 *pCoeffY,
                  u32 *pLeftEdge, u32 *pUseRightEdge, u32* pRightEdge)
{

/* Variables */

    u32 lastCol, lastRow;
    u32 m,n;
    u32 tmp;
    u32 D;
    const u32 P = 16;
    u32 Hprime, Vprime;
	i32 uxl, uxr;	/* row start/end offsets -- see notes */
	i32 uy0V, uy00, uylB;
	i32 uylNum;			/* Uyl numerator */
	i32 uylInc;			/* Uyl numerator increment each row */
    i32 axInitial;
    i32 axIncrement;
    i32 ayInitial;
    i32 ayIncrement;
    i32 ax, ay;
    u32 i, j = 0;
    u32 leftEdge = 0, useRightEdge = 0, rightEdge = outpW;

/* Code */

	/* initialize vars */
	lastCol = (inpW>>(1-lumaCoeffs)) - 1;
	lastRow = (inpH>>(1-lumaCoeffs)) - 1;
	m = 0;
	tmp = inpW;
	while (tmp > 0)
	{
		m++;
		tmp >>= 1;
	}
	/* check for case when inpW is power of two */
	if (inpW == (u32)(1<<(m-1))) m--;
	Hprime = 1<<m;
	D = (64*Hprime)/P;

	n = 0;
	tmp = inpH;
	while (tmp > 0)
	{
		n++;
		tmp >>= 1;
	}
	/* check for case when inpH is power of two */
	if (inpH == (u32)(1<<(n-1))) n--;
	Vprime = 1<<n;

	/* uxl and uxr are independent of row, so compute once only */
	uxl = 0;
	uxr = (outpW - Hprime)*uxl + ((((inpW - outpW)<<1))<<(4+m));		/* numerator part */
	/* complete uxr init by dividing by H with rounding to nearest integer, */
	/* half-integers away from 0 */
	if (uxr >= 0)
		uxr = (uxr + (outpW>>1))/outpW;
	else
    {
		uxr = (uxr - (i32)(outpW>>1))/(i32)outpW;
    }

	/* initial x displacement and the x increment are independent of row */
	/* so compute once only */
	axInitial = (uxl<<(m+lumaCoeffs)) + (uxr - uxl) + (D>>1);
	axIncrement = (Hprime<<6) + ((uxr - uxl)<<1);

	/* most contributions to calculation of uyl do not depend upon row, */
	/* compute once only */
	uy00 = 0;
	uy0V = ((inpH - outpH)<<1)<<4;
	uylB = outpH*uy00 + ((uy0V - uy00)<<n); /* numerator */
	/* complete uylB by dividing by V with rounding to nearest integer, */
	/* half-integers away from 0 */
	if (uylB >= 0)
		uylB = (uylB + (outpH>>1))/outpH;
	else
		uylB = (uylB - (i32)(outpH>>1))/(i32)outpH;
	uylInc = (uylB - uy00)<<1;
	uylNum = ((Vprime<<lumaCoeffs) - 1)*uy00 + uylB;

    ayInitial = D/2;
    ayIncrement = D*P;
    ay = ayInitial;
    ax = axInitial;
    if(!lumaCoeffs)
    {
        outpW >>= 1;
        outpH >>= 1;
    }
    for( i = 0 ; i < outpW ; ++i )
    {
        i32 x0, x1;
        x0 = ax >> (m+6);
        x1 = x0 + 1;
        if( x0 < 0 )
        {
            x0 = 0;
            leftEdge = i+1;
        }
        else if ( x0 > lastCol )    x0 = lastCol;
        if( x1 < 0 )                x1 = 0;
        else if ( x1 > lastCol )
        {
            x1 = lastCol;
            useRightEdge = 1;
            if(i < rightEdge)
                rightEdge = i;
        }
        if( x0 != x1 )
        {
            pSrcCol[j] = lumaCoeffs ? x0 : 2*x0;
            pCoeffX[j] = (ax >> (m+2)) & 0xf;
            j++;
        }
        ax += axIncrement;
    }
    
    for( i = 0 ; i < outpH ; ++i )
    {
        i32 add;
        i32 ayRow;
        i32 y0, y1;
		/* ay var is constant for all columns */

        add = (uylNum >> (n + lumaCoeffs)) 
            << (m + lumaCoeffs);
        ayRow = ay + add;

		y0 = ( ayRow ) >> (m+6);
        y1 = y0 + 1;
        if( y0 < 0 )                y0 = 0;
        else if ( y0 > lastRow )    y0 = lastRow;
        if( y1 < 0 )                y1 = 0;
        else if ( y1 > lastRow )    y1 = lastRow;
        *pSrcRowBuf++ = inp + y0*inpWfrm;
        *pSrcRowBuf++ = inp + y1*inpWfrm;
		pCoeffY[i] = ((ayRow) >> (m+2)) & 0xf;

        ay += ayIncrement;
		uylNum += uylInc;
    }

    *pLeftEdge = leftEdge;
    *pUseRightEdge = useRightEdge;
    *pRightEdge = rightEdge;

}

/*------------------------------------------------------------------------------

    Function name: Up2x

        Functional description:
            Perform fast 2x downsampling on both X and Y axis

        Inputs:

        Outputs: 

        Returns: 
            HANTRO_OK       Decoding successful
            HANTRO_NOK      Error in decoding
            END_OF_STREAM   End of stream encountered

------------------------------------------------------------------------------*/
void Up2x( u8 * inp, u8 *inpChr, u32 inpW, u32 inpWfrm, u32 inpH, u32 inpHfrm,
           u8 * outp, u8 *outpChr, u32 outpW, u32 outpWfrm, u32 outpH, u32 outpHfrm,
           u32 round )
{

/* Variables */

    u32 midCols;
    u32 midRows;
    u32 col, row;
    u8 * pSrc0, * pDest0;
    u8 * pSrc1, * pDest1;
    u32 inpSkip, outpSkip;
    u32 A, B;
    u32 round8;

/* Code */

    midCols = inpW-1;
    midRows = inpH-1;
    round8 = 7+round;
    round++;

    inpSkip = inpWfrm - inpW;
    outpSkip = 2*outpWfrm - outpW;

    /* Luma */

    /* Process first row */
    pSrc0 = inp;
    pDest0 = outp;
    A = *pSrc0++;
    *pDest0++ = A;
    col = midCols;
    while( col-- )
    {
        u32 pel0, pel1;
        B = *pSrc0++;
        pel0 = ( A*3 + B + round ) >> 2;
        pel1 = ( B*3 + A + round ) >> 2;
        *pDest0++ = pel0;
        *pDest0++ = pel1;
        A = B;
    }
    *pDest0++ = A;

    /* Process middle rows */
    pSrc0 = inp;
    pSrc1 = pSrc0 + inpWfrm;
    pDest0 = outp + outpWfrm;
    pDest1 = pDest0 + outpWfrm;
    row = midRows;
    while( row-- )
    {
        u32 C, D;
        u32 pel0, pel1;
        u32 tmp0;
        u32 tmp1;
        u32 tmp2;
        u32 tmp3;
        A = *pSrc0++;
        B = *pSrc0++;
        C = *pSrc1++;
        D = *pSrc1++;
        /* First pels */
        pel0 = ( A*3 + C + round ) >> 2;
        pel1 = ( C*3 + A + round ) >> 2;
        *pDest0++ = pel0;
        *pDest1++ = pel1;
        col = midCols >> 1;

        while( col-- )
        {
            /* 1st part */
            tmp0 = A*3 + B;
            tmp1 = B*3 + A;
            tmp2 = C*3 + D;
            tmp3 = D*3 + C;
            pel0 = ( 3*tmp0 + tmp2 + round8 ) >> 4;
            pel1 = ( 3*tmp1 + tmp3 + round8 ) >> 4;
            *pDest0++ = pel0;
            *pDest0++ = pel1;
            pel0 = ( tmp0 + 3*tmp2 + round8 ) >> 4;
            pel1 = ( tmp1 + 3*tmp3 + round8 ) >> 4;
            *pDest1++ = pel0;
            *pDest1++ = pel1;
            A = *pSrc0++;
            C = *pSrc1++;

            /* 2nd part (same as 1st but pixel positions reversed) */
            tmp1 = A*3 + B;
            tmp0 = B*3 + A;
            tmp3 = C*3 + D;
            tmp2 = D*3 + C;
            pel0 = ( 3*tmp0 + tmp2 + round8 ) >> 4;
            pel1 = ( 3*tmp1 + tmp3 + round8 ) >> 4;
            *pDest0++ = pel0;
            *pDest0++ = pel1;
            pel0 = ( tmp0 + 3*tmp2 + round8 ) >> 4;
            pel1 = ( tmp1 + 3*tmp3 + round8 ) >> 4;
            *pDest1++ = pel0;
            *pDest1++ = pel1;
            B = *pSrc0++;
            D = *pSrc1++;
        }
        /* extra 1st part, required because midCols is always odd */
        tmp0 = A*3 + B;
        tmp1 = B*3 + A;
        tmp2 = C*3 + D;
        tmp3 = D*3 + C;
        pel0 = ( 3*tmp0 + tmp2 + round8 ) >> 4;
        pel1 = ( 3*tmp1 + tmp3 + round8 ) >> 4;
        *pDest0++ = pel0;
        *pDest0++ = pel1;
        pel0 = ( tmp0 + 3*tmp2 + round8 ) >> 4;
        pel1 = ( tmp1 + 3*tmp3 + round8 ) >> 4;
        *pDest1++ = pel0;
        *pDest1++ = pel1;

        pel0 = ( B*3 + D + round ) >> 2;
        pel1 = ( D*3 + B + round ) >> 2;
        *pDest0++ = pel0;
        *pDest1++ = pel1;

        pSrc0 += inpSkip;
        pSrc1 += inpSkip;
        pDest0 += outpSkip;
        pDest1 += outpSkip;
    }

    /* Last row */
    A = *pSrc0++;
    *pDest0++ = A;
    col = midCols;
    while( col-- )
    {
        u32 pel0, pel1;
        B = *pSrc0++;
        pel0 = ( A*3 + B + round ) >> 2;
        pel1 = ( B*3 + A + round ) >> 2;
        *pDest0++ = pel0;
        *pDest0++ = pel1;
        A = B;
    }
    *pDest0++ = A;

    /* Chroma */
    inpW >>= 1;
    inpH >>= 1;
    midCols = inpW-1;
    midRows = inpH-1;
    inpSkip -= 2;

    /* Process first row */
    pSrc0 = inpChr;
    pDest0 = outpChr;
    A = *pSrc0++;
    B = *pSrc0++;
    *pDest0++ = A;
    *pDest0++ = B;
    col = midCols;
    while( col-- )
    {
        u32 C, D;
        u32 pel0, pel1;
        u32 pel2, pel3;
        C = *pSrc0++;
        D = *pSrc0++;
        pel0 = ( A*3 + C + round ) >> 2;
        pel1 = ( C*3 + A + round ) >> 2;
        pel2 = ( B*3 + D + round ) >> 2;
        pel3 = ( D*3 + B + round ) >> 2;
        *pDest0++ = pel0;
        *pDest0++ = pel2;
        *pDest0++ = pel1;
        *pDest0++ = pel3;
        A = C;
        B = D;
    }
    *pDest0++ = A;
    *pDest0++ = B;

    /* Process middle rows */
    pSrc0 = inpChr;
    pSrc1 = pSrc0 + inpWfrm;
    pDest0 = outpChr + outpWfrm;
    pDest1 = pDest0 + outpWfrm;
    row = midRows;
    while( row-- )
    {
        u32 C, D, E, F, G, H;
        u32 tmp0;
        u32 tmp1;
        u32 tmp2;
        u32 tmp3;
        A = *pSrc0++;
        E = *pSrc0++;
        C = *pSrc1++;
        G = *pSrc1++;
        B = *pSrc0++;
        F = *pSrc0++;
        D = *pSrc1++;
        H = *pSrc1++;
        /* First pels */
        tmp0 = ( A*3 + C + round ) >> 2;
        tmp1 = ( C*3 + A + round ) >> 2;
        tmp2 = ( E*3 + G + round ) >> 2;
        tmp3 = ( G*3 + E + round ) >> 2;
        *pDest0++ = tmp0;
        *pDest0++ = tmp2;
        *pDest1++ = tmp1;
        *pDest1++ = tmp3;
        col = midCols;
        while( col-- )
        {
            u32 pel0, pel1;
            /* Cb */
            tmp0 = A*3 + B;
            tmp1 = B*3 + A;
            tmp2 = C*3 + D;
            tmp3 = D*3 + C;
            pel0 = ( 3*tmp0 + tmp2 + round8 ) >> 4;
            pel1 = ( 3*tmp1 + tmp3 + round8 ) >> 4;
            pDest0[0] = pel0;
            pDest0[2] = pel1;
            pel0 = ( tmp0 + 2*tmp2 + tmp2 + round8 ) >> 4;
            pel1 = ( tmp1 + 2*tmp3 + tmp3 + round8 ) >> 4;
            pDest1[0] = pel0;
            pDest1[2] = pel1;

            /* Cr */
            tmp0 = E*3 + F;
            tmp1 = F*3 + E;
            tmp2 = G*3 + H;
            tmp3 = H*3 + G;
            pel0 = ( 3*tmp0 + tmp2 + round8 ) >> 4;
            pel1 = ( 3*tmp1 + tmp3 + round8 ) >> 4;
            pDest0[1] = pel0;
            pDest0[3] = pel1;
            pel0 = ( tmp0 + 3*tmp2 + round8 ) >> 4;
            pel1 = ( tmp1 + 3*tmp3 + round8 ) >> 4;
            pDest1[1] = pel0;
            pDest1[3] = pel1;

            pDest0 += 4;
            pDest1 += 4;

            A = B;
            C = D;
            E = F;
            G = H;
            B = *pSrc0++;
            F = *pSrc0++;
            D = *pSrc1++;
            H = *pSrc1++;
        }

        tmp0 = ( A*3 + C + round ) >> 2;
        tmp1 = ( C*3 + A + round ) >> 2;
        tmp2 = ( E*3 + G + round ) >> 2;
        tmp3 = ( G*3 + E + round ) >> 2;
        *pDest0++ = tmp0;
        *pDest0++ = tmp2;
        *pDest1++ = tmp1;
        *pDest1++ = tmp3;

        pSrc0 += inpSkip;
        pSrc1 += inpSkip;
        pDest0 += outpSkip;
        pDest1 += outpSkip;
    }

    /* Last row */
    A = *pSrc0++;
    B = *pSrc0++;
    *pDest0++ = A;
    *pDest0++ = B;
    col = midCols;
    while( col-- )
    {
        u32 C, D;
        u32 pel0, pel1;
        u32 pel2, pel3;
        C = *pSrc0++;
        D = *pSrc0++;
        pel0 = ( A*3 + C + round ) >> 2;
        pel1 = ( C*3 + A + round ) >> 2;
        pel2 = ( B*3 + D + round ) >> 2;
        pel3 = ( D*3 + B + round ) >> 2;
        *pDest0++ = pel0;
        *pDest0++ = pel2;
        *pDest0++ = pel1;
        *pDest0++ = pel3;
        A = C;
        B = D;
    }
    *pDest0++ = A;
    *pDest0++ = B;

    /* TODO padding? */

}

/*------------------------------------------------------------------------------

    Function name: Down2x

        Functional description:
            Perform fast 2x downsampling on both X and Y axis

        Inputs:

        Outputs: 

        Returns: 
            HANTRO_OK       Decoding successful
            HANTRO_NOK      Error in decoding
            END_OF_STREAM   End of stream encountered

------------------------------------------------------------------------------*/
void Down2x( u8 * inp, u8 *inpChr, u32 inpW, u32 inpWfrm, u32 inpH, u32 inpHfrm,
             u8 * outp, u8 *outpChr, u32 outpW, u32 outpWfrm, u32 outpH, u32 outpHfrm,
             u32 round )
{

/* Variables */

    u32 row, col;
    u8 *pSrcRow0, *pSrcRow1;
    u8 *pDest;
    u32 inpSkip, outpSkip;

/* Code */

    pSrcRow0 = inp;
    pSrcRow1 = inp + inpWfrm;
    pDest = outp;

    inpSkip = 2*inpWfrm - inpW;
    outpSkip = outpWfrm - outpW;

    /* Luma */

    row = inpH >> 1;
    round++;
    while(row--)
    {
        col = inpW >> 2;
        while(col--)
        {
            u32 pel0, pel1;
            u32 A, B, C, D, E, F, G, H;
            /* Do two output pixels at a time */
            A = *pSrcRow0++;
            B = *pSrcRow0++;
            C = *pSrcRow0++;
            D = *pSrcRow0++;
            E = *pSrcRow1++;
            F = *pSrcRow1++;
            G = *pSrcRow1++;
            H = *pSrcRow1++;
            pel0 = (A + B + E + F + round) >> 2;
            pel1 = (C + D + G + H + round) >> 2;
            *pDest++ = pel0;
            *pDest++ = pel1;
        }

        pSrcRow0 += inpSkip;
        pSrcRow1 += inpSkip;
        pDest += outpSkip;
    }   

    pSrcRow0 = inpChr;
    pSrcRow1 = inpChr + inpWfrm;
    pDest = outpChr;

    /* Chroma */

    row = inpH >> 2;
    while(row--)
    {
        col = inpW >> 2;
        while(col--)
        {
            u32 pelCb, pelCr;
            u32 A, B, C, D;
            u32 E, F, G, H;
            A = *pSrcRow0++;
            B = *pSrcRow0++;
            C = *pSrcRow0++;
            D = *pSrcRow0++;
            E = *pSrcRow1++;
            F = *pSrcRow1++;
            G = *pSrcRow1++;
            H = *pSrcRow1++;
            pelCb = (A + C + E + G + round) >> 2;
            pelCr = (B + D + F + H + round) >> 2;
            *pDest++ = pelCb;
            *pDest++ = pelCr;
        }

        pSrcRow0 += inpSkip;
        pSrcRow1 += inpSkip;
        pDest += outpSkip;
    } 
    
    /* TODO padding? */
}


/*------------------------------------------------------------------------------

    Function name: RvRasterToTiled8x4

        Functional description:
            Convert 8x4 tiled output to raster scan output.

        Inputs:

        Outputs: 

        Returns: 

------------------------------------------------------------------------------*/
void RvTiledToRaster8x4( u32 *pIn, u32 *pOut, u32 picWidth, u32 picHeight )
{
    u32 i, j;
    u32 tilesW;
    u32 tilesH;
    u32 skip;
    u32 *pOut0 = pOut;
    u32 *pOut1 = pOut +   (picWidth/4);
    u32 *pOut2 = pOut + 2*(picWidth/4);
    u32 *pOut3 = pOut + 3*(picWidth/4);
   
    tilesW = picWidth/8;
    tilesH = picHeight/4;
    skip = picWidth-picWidth/4;

    for( i = 0 ; i < tilesH ; ++i)
    {
        for( j = 0 ; j < tilesW ; ++j )
        {
            *pOut0++ = *pIn++;
            *pOut0++ = *pIn++;
            *pOut1++ = *pIn++;
            *pOut1++ = *pIn++;
            *pOut2++ = *pIn++;
            *pOut2++ = *pIn++;
            *pOut3++ = *pIn++;
            *pOut3++ = *pIn++;
        }
        pOut0 += skip;
        pOut1 += skip;
        pOut2 += skip;
        pOut3 += skip;
    }
}


/*------------------------------------------------------------------------------

    Function name: RvRasterToTiled8x4

        Functional description:
            Convert raster scan output to 8x4 tiled output.

        Inputs:

        Outputs: 

        Returns: 

------------------------------------------------------------------------------*/
void RvRasterToTiled8x4( u32 *inp, u32 *outp, u32 width, u32 height)
{
    u32 i, j;
    u32 tilesW;
    u32 tilesH;
    u32 skip;
    u32 *pIn0 = inp,
        *pIn1 = inp +   (width/4),
        *pIn2 = inp + 2*(width/4),
        *pIn3 = inp + 3*(width/4);
    
    tilesW = width/8;
    tilesH = height/4;
    skip = width-width/4;

    for( i = 0 ; i < tilesH ; ++i)
    {
        for( j = 0 ; j < tilesW ; ++j )
        {
            *outp++ = *pIn0++;
            *outp++ = *pIn0++;
            *outp++ = *pIn1++;
            *outp++ = *pIn1++;
            *outp++ = *pIn2++;
            *outp++ = *pIn2++;
            *outp++ = *pIn3++;
            *outp++ = *pIn3++;
        }

        pIn0 += skip;
        pIn1 += skip;
        pIn2 += skip;
        pIn3 += skip;
    }
}

/*------------------------------------------------------------------------------

    Function name: RvResample

        Functional description:
            Create coefficient and row look-up tables for bilinear algotihm.
            Also evaluates amounts of overfill required per row.

        Inputs:

        Outputs: 
            pSrcCol         Array translating output columns to source columns.
            pSrcRowBuf      Array translating output rows to source rows.
            pCoeffX         X coefficients per output column.
            pCoeffY         Y coefficients per output column.

        Returns: 
            HANTRO_OK       Decoding successful
            HANTRO_NOK      Error in decoding
            END_OF_STREAM   End of stream encountered

------------------------------------------------------------------------------*/
void RvResample( u8 * inp, u8 *inpChr, u32 inpW, u32 inpWfrm, u32 inpH, 
                    u32 inpHfrm, u8 * outp, u8 *outpChr, u32 outpW, 
                    u32 outpWfrm, u32 outpH, u32 outpHfrm, 
                    u32 round, u8 *workMemory, u32 tiledMode )
{

/* Variables */

    u8  **pSrcRowBuf;
    u16 *srcCol;
    u8  *coeffX;
    u8  *coeffY;
    u32 rndVal;
    u32 leftEdge = 0;
    u32 rightEdge = outpW;
    u32 useRightEdge = 0;

/* Code */

    /* If we are using tiled mode do:
     * 1) tiled-to-raster conversion from inp-buffer to outp-buffer
     * 2) resampling from outp-buffer to inp-buffer
     * 3) raster-to-tiled conversion from inp-buffer to outp-buffer
     * Buffer sizes are always ok because they are allocated 
     * according to maximum frame dimensions for the stream.
     */
    if(tiledMode)
    {
        u8 *pTmp;
        RvTiledToRaster8x4( (u32*)inp, (u32*)outp, inpWfrm, inpHfrm );
        RvTiledToRaster8x4( (u32*)inpChr, (u32*)(outp+inpWfrm*inpHfrm), 
            inpW, inpH/2 );
        /* Swap ptrs */
        pTmp = inp;
        inp = outp;
        outp = pTmp;
        inpChr = inp+inpWfrm*inpHfrm;
        outpChr = outp+outpWfrm*outpHfrm;
    }

    if ((inpW<<1) == outpW && (inpH<<1) == outpH )
    {
        Up2x( inp, inpChr, inpW, inpWfrm, inpH, inpHfrm,
              outp, outpChr, outpW, outpWfrm, outpH, outpHfrm,
              round );
    }
    else if ((inpW>>1) == outpW && (inpH>>1) == outpH )
    {
        Down2x( inp, inpChr, inpW, inpWfrm, inpH, inpHfrm,
                outp, outpChr, outpW, outpWfrm, outpH, outpHfrm,
                round );
    }
    else /* Arbitrary scale ratios */
    {
        /* Assign work memory */
        pSrcRowBuf = (u8**)workMemory;
        srcCol = (u16*)(pSrcRowBuf + outpH*2);
        coeffX = ((u8*)srcCol) + 2*outpW;
        coeffY = coeffX + outpW;

        rndVal = 127 + round;

        /* Process luma */
        CoeffTables( inpW, inpH, outpW, outpH, 
                     1/*lumaCoeffs*/, inp, inpWfrm,
                     srcCol, pSrcRowBuf,
                     coeffX, coeffY,
                     &leftEdge, &useRightEdge, &rightEdge );

        ResampleY( inpW, outpW, outpWfrm, outpH, outp, pSrcRowBuf, srcCol, coeffY, coeffX,
                        leftEdge, useRightEdge, rightEdge, rndVal );
        /* And then process chroma */
        CoeffTables( inpW, inpH, outpW, outpH, 
                     0/*lumaCoeffs*/, inpChr, 
                     inpWfrm,
                     srcCol, pSrcRowBuf,
                     coeffX, coeffY,
                     &leftEdge, &useRightEdge, &rightEdge );
        ResampleChr( inpW, outpW>>1, outpWfrm, outpH>>1, 
                     outpChr, pSrcRowBuf, 
                     srcCol, coeffY, coeffX,
                     leftEdge, useRightEdge, rightEdge, rndVal );
    }

    if(tiledMode)
    {
        RvRasterToTiled8x4( (u32*)outp, (u32*)inp, outpWfrm, outpHfrm );
        RvRasterToTiled8x4( (u32*)outpChr, (u32*)(inp+outpWfrm*outpHfrm), 
            outpW, outpH/2 );
    }
}


void rvRpr( picture_t *pSrc,
               picture_t *pDst,
               DWLLinearMem_t *rprWorkBuffer,
               u32 round,
               u32 newCodedWidth, u32 newCodedHeight,
               u32 tiledMode )
{

/* Variables */

    u8 * pInpY, * pInpChr;
    u8 * pOutpY, * pOutpChr;
    u32 inpW, inpWfrm;
    u32 inpH, inpHfrm;
    u32 outpW, outpWfrm;
    u32 outpH, outpHfrm;
    u8 *memory;

    /* Get pointers etc */
    inpW = pSrc->codedWidth;
    inpWfrm = pSrc->frameWidth;
    inpH = pSrc->codedHeight;
    inpHfrm = pSrc->codedHeight;
    outpW = newCodedWidth;
    outpWfrm = ( 15 + outpW ) & ~15;
    outpH = newCodedHeight;
    outpHfrm = ( 15 + outpH ) & ~15;
//add by franklin for rvds
    pInpY = (u8 *)(pSrc->data.virtualAddress);
    pInpChr = pInpY + inpWfrm*inpHfrm;
    pOutpY = (u8 *)(pDst->data.virtualAddress);
    pOutpChr = pOutpY + outpWfrm*outpHfrm;
    memory = (u8 *)(rprWorkBuffer->virtualAddress);

    /* Resample */
    RvResample( pInpY, pInpChr, inpW, inpWfrm, inpH, inpHfrm,
                   pOutpY, pOutpChr, outpW, outpWfrm, outpH, outpHfrm,
                   round, memory, tiledMode);
}


