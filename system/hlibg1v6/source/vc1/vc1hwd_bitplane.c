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
--  Description : Bitplane decoding functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_bitplane.c,v $
--  $Revision: 1.7 $
--  $Date: 2007/12/13 13:27:44 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/
#include "vc1hwd_bitplane.h"
#include "vc1hwd_stream.h"
#include "vc1hwd_util.h"

/*------------------------------------------------------------------------------
    External compiler flags
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define INVALID_TILE_VLC    ((u16x)(-1))

/* 3x2 and 2x3 tiles - VLC length 8 bits -> t[n]
                       VLC length 13 bits -> use 63 - t[n] */
static const u16x codeTile_8[15] = {
    3, 5, 6, 9, 10, 12, 17, 18, 20, 24, 33, 34, 36, 40, 48
};

/* 3x2 and 2x3 tiles - VLC length 9 bits */
static const u16x codeTile_9[6] = {
    62, 61, 59, 55, 47, 31
};

/* 3x2 and 2x3 tiles - VLC length 10 bits */
static const u16x codeTile_10[26] = {
    35, INVALID_TILE_VLC, 37, 38, 7, INVALID_TILE_VLC, 41, 42, 11, 44, 13, 14,
    INVALID_TILE_VLC, INVALID_TILE_VLC, 49, 50, 19, 52, 21, 22,
    INVALID_TILE_VLC, 56, 25, 26, INVALID_TILE_VLC, 28
};

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

static u16x DecodeTile( u32 bits, u32 *bitsUsed );
static void DecodeNormal2( strmData_t * const strmData, i16x count,
                           u8 *pData, const u16x bit );
static u16x DecodeNormal6( strmData_t * strmData, const u16x colMb,
                           const u16x rowMb, u8 *pData,
                           const u16x bit, u16x invert );
static void DecodeDifferential2( strmData_t * const strmData, i16x count,
                                 u8 *pData, const u16x bit,
                                 const u16x colMb );
static u16x DecodeDifferential6( strmData_t * strmData, const u16x colMb,
                                 const u16x rowMb, u8 *pData,
                                 const u16x bit, u16x invert );

/*------------------------------------------------------------------------------
    Functions
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------

   Function: DecodeTile

        Functional description:
            Decode 2x3 or 3x2 tile code word.

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:
            u16x            Tile code word [0, 63]

------------------------------------------------------------------------------*/
u16x DecodeTile( u32 bits, u32 *bitsUsed )
{

/* Variables */

    /*u16x bits;*/
    u16x code;
    u16x len;
    u16x tmp;

/* Code */

    /* Max VLC codeword size 13 bits */
    /*bits = vc1hwdShowBits( strmData, 13 );*/
    bits = (bits << *bitsUsed);

    if( bits >= (4096U<<19) )
    {
        len = 1;
        code = 0;
    }
    else {
        tmp = bits >> (8+19);
        if( tmp == 2 ) {
            /* 10-bit code */
            tmp = ( ( bits >> (3+19) ) & 31 ) - 3;
            len = 10;
            if( tmp <= 25 )
                code = codeTile_10[tmp];
            else
                code = INVALID_TILE_VLC;
        } else if ( tmp == 3 ) {
            tmp = (bits >> 19) & 255;
            if( tmp >= 128 ) {
                len = 6;
                code = 63;
            }
            else if( tmp >= 16 ) {
                /* 9-bit code */
                len = 9;
                tmp = ( tmp >> 4 ) - 2;
                if( tmp <= 5 )
                    code = codeTile_9[tmp];
                else
                    code = INVALID_TILE_VLC; /* Invalid VLC encountered */
            }
            else {
                /* 13-bit code */
                len = 13;
                if( tmp == 15 )
                    code = INVALID_TILE_VLC; /* Invalid VLC encountered */
                else
                    code = 63 - codeTile_8[ tmp ];
            }
        } else if( bits >= (512U<<19) ) {
            /* 4-bit code */
            tmp = bits >> (9+19);
            len = 4;
            code = 1 << ( tmp - 2 );
        } else {
            /* 8-bit code */
            len = 8;
            tmp = bits >> (5+19);
            if( tmp == 15 )
                code = INVALID_TILE_VLC; /* Invalid VLC encountered */
            else
                code = codeTile_8[ tmp ];
        }
    }

    /* Flush data */
    /*(void)vc1hwdFlushBits(strmData, len);*/
    *bitsUsed += len;

    return code;
}

/*------------------------------------------------------------------------------

    Function name: DecodeNormal2

        Functional description:
            Decode Normal-2 mode bitplane.

        Inputs:
            strmData        Pointer to stream data descriptor.
            count           Amount of bits to read. If < 0 then INVERT bit is
                            set and amount of bits is -count.
            bit             Bit value.

        Outputs:

------------------------------------------------------------------------------*/
void DecodeNormal2( strmData_t * const strmData, i16x count, u8 *pData,
                    const u16x bit )
{

/* Variables */

    u32 tmp;
    u32 bitsUsed = 0;
    u32 bits = 0;

/* Code */

    ASSERT( strmData );
    ASSERT( pData );
    ASSERT( count != 0 );
    ASSERT( bit != 0 );

    if ( count < 0 ) /* INVERT = 1 */
    {
        count = -count;
        /* Decode odd symbol */
        if( count & 1 ) {
            (*pData++) |= ( vc1hwdGetBits( strmData, 1 ) == 0 ) ? bit : 0;
            count--;
        }

        /*lint -save -e702 */
        count >>= 1; /* What follows are pairs */
        /*lint -restore */

        /* Decode subsequent symbol pairs */
        bits = vc1hwdShowBits( strmData, 32 );
        while( count-- )
        {
            tmp = ( bits << bitsUsed ); /* (u16x)vc1hwdShowBits(strmData, 3);*/

            if( tmp < (4U<<29) )
            {
                /* Codeword 0, symbols 1 and 1 */
                bitsUsed++;
                *pData++ |= (u8)bit;
                *pData++ |= (u8)bit;
            } else {
                if( tmp < (6U<<29) ) {
                    /* Codeword 100 or 101 */
                    tmp = (~tmp) >> 29;
                    pData[tmp & 1] |= (u8)bit;
                    bitsUsed ++;
                }
                bitsUsed += 2;
                pData += 2;
            } 
            if( bitsUsed > 29 )
            {
                (void)vc1hwdFlushBits( strmData, bitsUsed );
                bits = vc1hwdShowBits( strmData, 32 );
                bitsUsed = 0;
            }
        }
        (void)vc1hwdFlushBits( strmData, bitsUsed );
    }
    else /* INVERT = 0 */
    {
        /* Decode odd symbol */
        if( count & 1 ) {
            *pData++ |= ( vc1hwdGetBits( strmData, 1 ) == 1 ) ? bit : 0;
            count--;
        }

        /*lint -save -e702 */
        count >>= 1; /* What follows are pairs */
        /*lint -restore */

        /* Decode subsequent symbol pairs */
        bits = vc1hwdShowBits( strmData, 32 );
        while( count-- )
        {
            tmp = ( bits << bitsUsed ); /* (u16x)vc1hwdShowBits(strmData, 3);*/
            if( tmp < (4U<<29) )
            {
                /* Codeword 0, symbols 0 and 0 */
                bitsUsed++;
                pData += 2;
            } else {
                if( tmp >= (6U<<29) ) {
                    /* Codeword 11, symbols 1 and 1 */
                    bitsUsed += 2;
                    *pData++ |= (u8)bit;
                    *pData++ |= (u8)bit;
                } else {
                    /* Codeword 100 or 101 */
                    tmp >>= 29;
                    bitsUsed += 3;
                    pData[tmp & 1] |= (u8)bit;
                    pData += 2;
                }
            } 
            if( bitsUsed > 29 )
            {
                (void)vc1hwdFlushBits( strmData, bitsUsed );
                bitsUsed = 0;
                bits = vc1hwdShowBits( strmData, 32 );
            }
        }
        (void)vc1hwdFlushBits( strmData, bitsUsed );
    }
}


/*------------------------------------------------------------------------------

    Function name: DecodeDifferential2

        Functional description:
            Decode Differential-2 mode bitplane.

        Inputs:
            strmData        Pointer to stream data descriptor.
            count           Amount of bits to read. If < 0 then INVERT bit is
                            set and amount of bits is -count.
            bit             Bit value.
            colMb           Number of macroblock columns.

        Outputs:

------------------------------------------------------------------------------*/
void DecodeDifferential2( strmData_t * const strmData, i16x count,
                          u8 *pData, const u16x bit,
                          const u16x colMb )
{

/* Variables */

    u16x tmp, k, i, j;
    u16x a; /* predictor 'A' in standard */
    u8 *pTmp;
    u32 bitsUsed = 0;
    u32 bits = 0;

/* Code */

    ASSERT( strmData );
    ASSERT( pData );
    ASSERT( count != 0 );
    ASSERT( bit != 0 );

    if( count < 0 ) {
        a = bit;
        count = -count;
    }
    else {
        a = 0 ;
    }
    pTmp = pData;
    k = (u16x)count;

    /* Decode odd symbol */
    if( count & 1 ) {
        *pTmp++ |= ( vc1hwdGetBits( strmData, 1 ) == 1 ) ? bit : 0;
        k--;
    }

    k >>= 1; /* What follows are pairs */

    /* Decode subsequent symbol pairs */
    bits = vc1hwdShowBits( strmData, 32 );
    while( k-- )
    {
        tmp = (bits << bitsUsed ); /* (u16x) vc1hwdShowBits( strmData, 3 );*/
        if( tmp < (4U<<29) )
        {
            /* Codeword 0, symbols 0 and 0 */
            bitsUsed++;
            pTmp += 2;
        } else {
            if( tmp >= (6U<<29) ) {
                /* Codeword 11, symbols 1 and 1 */
                bitsUsed += 2;
                *pTmp++ |= (u8)bit;
                *pTmp++ |= (u8)bit;
            } else {
                /* Codeword 100 or 101 */
                tmp >>= 29;
                pTmp[tmp & 1] |= (u8)bit;
                bitsUsed += 3;
                pTmp += 2;
            }
        } 
        if( bitsUsed > 29 )
        {
            (void)vc1hwdFlushBits( strmData, bitsUsed );
            bits = vc1hwdShowBits( strmData, 32 );
            bitsUsed = 0;
        }
    }
    (void)vc1hwdFlushBits( strmData, bitsUsed );

    /* Perform differential operation */
    pTmp = pData;
    k = (u16x)count-1;
    i = 1;
    j = 0;
    *pTmp ^= (u8)a;
    while( k-- ) {
        if( i == colMb ) {
            i = 0;
            j++;
        }
        if( i == 0 )
            tmp = *(pTmp-colMb+1) & bit;
        else if ( j > 0 && ( ( *(pTmp-colMb+1) ^ *pTmp) & bit ) )
            tmp = a;
        else
            tmp = *pTmp & bit;
        pTmp++;
        *pTmp ^= (u8)tmp;
        i++;
    }
}


/*------------------------------------------------------------------------------

    Function name: DecodeNormal6

        Functional description:
            Decode Normal-6 mode bitplane.

        Inputs:
            strmData        Pointer to stream data descriptor.

        Outputs:

------------------------------------------------------------------------------*/
u16x DecodeNormal6( strmData_t * const strmData, const u16x colMb,
                    const u16x rowMb, u8 *pData, const u16x bit,
                    u16x invert )
{

/* Variables */

    u16x tmp, k, i;
    u16x tileW, tileH; /* width in tiles */
    u16x rowskip, colskip;
    u16x rv = HANTRO_OK;
    u8 * pTmp;
    u32 bits;
    u32 bitsUsed;

/* Code */

    ASSERT( strmData );
    ASSERT( pData );

    bits = vc1hwdShowBits( strmData, 32 );
    bitsUsed = 0;

    if(!invert)
    {
        tmp = colMb % 3;
        if( ( rowMb % 3 ) == 0 && tmp > 0 ) { /* 2x3 tiles */
            tileW = colMb >> 1;
            tileH = rowMb / 3;
            i = 0;
            colskip = colMb & 1;
            rowskip = 0;
            pTmp = pData + colskip;
            bits = vc1hwdShowBits( strmData, 32 );
            bitsUsed = 0;
            while( tileH ) {
                /* Decode next tile codeword */
                k = DecodeTile( bits, &bitsUsed );
                if( bitsUsed > 19 )
                {
                    (void)vc1hwdFlushBits( strmData, bitsUsed );
                    bits = vc1hwdShowBits( strmData, 32 );
                    bitsUsed = 0;
                }
                if( k == INVALID_TILE_VLC ) {
                    /* Error handling */
                    EPRINT(("DecodeNormal6: Invalid VLC code word encountered.\n"));
                    rv = HANTRO_NOK;
                    k = 0;
                }
                /* Process 2x3 tile */
                pTmp[0] |= (u8)k & 1 ? bit : 0;
                pTmp[colMb] |= (u8)k & 4 ? bit : 0;
                pTmp[2*colMb] |= (u8)k & 16 ? bit : 0;
                pTmp++;
                pTmp[0] |= (u8)k & 2 ? bit : 0;
                pTmp[colMb] |= (u8)k & 8 ? bit : 0;
                pTmp[2*colMb] |= (u8)k & 32 ? bit : 0;
                pTmp++;
                /* Skip to next tile row */
                if( ++i == tileW ) {
                    pTmp += 2*colMb + colskip;
                    i = 0;
                    tileH--;
                }
            }
        }
        else { /* 3x2 tiles */
            tileW = colMb / 3;
            tileH = rowMb >> 1;
            i = 0;
            colskip = tmp;
            rowskip = rowMb & 1;
            pTmp = pData + colskip + rowskip*colMb;
            while( tileH ) {
                /* Decode next tile codeword */
                k = DecodeTile( bits, &bitsUsed );
                if( bitsUsed > 19 )
                {
                    (void)vc1hwdFlushBits( strmData, bitsUsed );
                    bits = vc1hwdShowBits( strmData, 32 );
                    bitsUsed = 0;
                }
                if( k == INVALID_TILE_VLC ) {
                    /* Error handling */
                    EPRINT(("DecodeNormal6: Invalid VLC code word encountered.\n"));
                    rv = HANTRO_NOK;
                    k = 0;
                }
                /* Process 3x2 tile */
                pTmp[0] |= (u8)k & 1 ? bit : 0;
                pTmp[colMb] |= (u8)k & 8 ? bit : 0;
                pTmp++;
                pTmp[0] |= (u8)k & 2 ? bit : 0;
                pTmp[colMb] |= (u8)k & 16 ? bit : 0;
                pTmp++;
                pTmp[0] |= (u8)k & 4 ? bit : 0;
                pTmp[colMb] |= (u8)k & 32 ? bit : 0;
                pTmp++;
                /* Skip to next tile row */
                if( ++i == tileW ) {
                    pTmp += colMb + colskip;
                    i = 0;
                    tileH--;
                }
            }
        }

        (void)vc1hwdFlushBits( strmData, bitsUsed );

        /* Process column-skip tiles */
        if( colskip > 0 ) {
            DPRINT(("Col-skip %d\n", colskip));
            for( i = 0 ; i < colskip ; ++i ) {
                if( vc1hwdGetBits( strmData, 1 ) == 0 )
                {
                    /* Process inverted columns */
                    if(invert) {
                        k = rowMb;
                        pTmp = pData + i;
                        while( k-- ) {
                            *pTmp |= (u8)bit;
                            pTmp += colMb;
                        }
                    }
                    continue;
                }
                pTmp = pData + i;
                k = rowMb >> 2;
                while ( k-- ) {
                    tmp = vc1hwdGetBits( strmData, 4 );
                    *pTmp |= (u8)tmp & 8 ? bit : 0;
                    pTmp += colMb;
                    *pTmp |= (u8)tmp & 4 ? bit : 0;
                    pTmp += colMb;
                    *pTmp |= (u8)tmp & 2 ? bit : 0;
                    pTmp += colMb;
                    *pTmp |= (u8)tmp & 1 ? bit : 0;
                    pTmp += colMb;
                }
                k = rowMb & 3;
                while ( k-- ) {
                    tmp = vc1hwdGetBits( strmData, 1 ) ? bit : 0;
                    *pTmp |= (u8)tmp;
                    pTmp += colMb;
                }
            }
        }

        /* Process row-skip tiles */
        if( rowskip > 0 ) {
            DPRINT(("Row-skip %d\n", rowskip));
            ASSERT( rowskip == 1 );
            if( vc1hwdGetBits( strmData, 1 ) == 1 )
            {
                pTmp = pData + colskip;
                colskip = colMb - colskip;
                k = colskip >> 2;
                while ( k-- ) {
                    tmp = vc1hwdGetBits( strmData, 4 );
                    *pTmp |= (u8)tmp & 8 ? bit : 0;
                    pTmp++;
                    *pTmp |= (u8)tmp & 4 ? bit : 0;
                    pTmp++;
                    *pTmp |= (u8)tmp & 2 ? bit : 0;
                    pTmp++;
                    *pTmp |= (u8)tmp & 1 ? bit : 0;
                    pTmp++;
                }
                k = colskip & 3;
                while( k-- ) {
                    tmp = vc1hwdGetBits( strmData, 1 ) ? bit : 0;
                    *pTmp |= (u8)tmp;
                    pTmp++;
                }
            }
            else
            {
                /* Process inverted row */
                if( invert )
                {
                    k = colMb - colskip;
                    pTmp = pData + colskip;
                    while(k--)
                    {
                        *pTmp |= (u8)bit;
                        pTmp++;
                    }
                }
            }
        }
    }
    else
    {
        tmp = colMb % 3;
        if( ( rowMb % 3 ) == 0 && tmp > 0 ) { /* 2x3 tiles */
            tileW = colMb >> 1;
            tileH = rowMb / 3;
            i = 0;
            colskip = colMb & 1;
            rowskip = 0;
            pTmp = pData + colskip;
            bits = vc1hwdShowBits( strmData, 32 );
            bitsUsed = 0;
            while( tileH ) {
                /* Decode next tile codeword */
                k = DecodeTile( bits, &bitsUsed );
                if( bitsUsed > 19 )
                {
                    (void)vc1hwdFlushBits( strmData, bitsUsed );
                    bits = vc1hwdShowBits( strmData, 32 );
                    bitsUsed = 0;
                }
                if( k == INVALID_TILE_VLC ) {
                    /* Error handling */
                    EPRINT(("DecodeNormal6: Invalid VLC code word encountered.\n"));
                    rv = HANTRO_NOK;
                    k = 0;
                }
                /* Process 2x3 tile */
                pTmp[0] |= (u8)k & 1 ? 0 : bit;
                pTmp[colMb] |= (u8)k & 4 ? 0 : bit;
                pTmp[2*colMb] |= (u8)k & 16 ? 0 : bit;
                pTmp++;
                pTmp[0] |= (u8)k & 2 ? 0 : bit;
                pTmp[colMb] |= (u8)k & 8 ? 0 : bit;
                pTmp[2*colMb] |= (u8)k & 32 ? 0 : bit;
                pTmp++;
                /* Skip to next tile row */
                if( ++i == tileW ) {
                    pTmp += 2*colMb + colskip;
                    i = 0;
                    tileH--;
                }
            }
        }
        else { /* 3x2 tiles */
            tileW = colMb / 3;
            tileH = rowMb >> 1;
            i = 0;
            colskip = tmp;
            rowskip = rowMb & 1;
            pTmp = pData + colskip + rowskip*colMb;
            while( tileH ) {
                /* Decode next tile codeword */
                k = DecodeTile( bits, &bitsUsed );
                if( bitsUsed > 19 )
                {
                    (void)vc1hwdFlushBits( strmData, bitsUsed );
                    bits = vc1hwdShowBits( strmData, 32 );
                    bitsUsed = 0;
                }
                if( k == INVALID_TILE_VLC ) {
                    /* Error handling */
                    EPRINT(("DecodeNormal6: Invalid VLC code word encountered.\n"));
                    rv = HANTRO_NOK;
                    k = 0;
                }
                /* Process 3x2 tile */
                pTmp[0] |= (u8)k & 1 ? 0 : bit;
                pTmp[colMb] |= (u8)k & 8 ? 0 : bit;
                pTmp++;
                pTmp[0] |= (u8)k & 2 ? 0 : bit;
                pTmp[colMb] |= (u8)k & 16 ? 0 : bit;
                pTmp++;
                pTmp[0] |= (u8)k & 4 ? 0 : bit;
                pTmp[colMb] |= (u8)k & 32 ? 0 : bit;
                pTmp++;
                /* Skip to next tile row */
                if( ++i == tileW ) {
                    pTmp += colMb + colskip;
                    i = 0;
                    tileH--;
                }
            }
        }

        (void)vc1hwdFlushBits( strmData, bitsUsed );

        /* Process column-skip tiles */
        if( colskip > 0 ) {
            DPRINT(("Col-skip %d\n", colskip));
            for( i = 0 ; i < colskip ; ++i ) {
                if( vc1hwdGetBits( strmData, 1 ) == 0 )
                {
                    /* Process inverted columns */
                    if(invert) {
                        k = rowMb;
                        pTmp = pData + i;
                        while( k-- ) {
                            *pTmp |= (u8)bit;
                            pTmp += colMb;
                        }
                    }
                    continue;
                }
                pTmp = pData + i;
                k = rowMb >> 2;
                while ( k-- ) {
                    tmp = vc1hwdGetBits( strmData, 4 );
                    *pTmp |= (u8)tmp & 8 ? 0 : bit;
                    pTmp += colMb;
                    *pTmp |= (u8)tmp & 4 ? 0 : bit;
                    pTmp += colMb;
                    *pTmp |= (u8)tmp & 2 ? 0 : bit;
                    pTmp += colMb;
                    *pTmp |= (u8)tmp & 1 ? 0 : bit;
                    pTmp += colMb;
                }
                k = rowMb & 3;
                while ( k-- ) {
                    tmp = vc1hwdGetBits( strmData, 1 ) ? 0 : bit;
                    *pTmp |= (u8)tmp;
                    pTmp += colMb;
                }
            }
        }

        /* Process row-skip tiles */
        if( rowskip > 0 ) {
            DPRINT(("Row-skip %d\n", rowskip));
            ASSERT( rowskip == 1 );
            if( vc1hwdGetBits( strmData, 1 ) == 1 )
            {
                pTmp = pData + colskip;
                colskip = colMb - colskip;
                k = colskip >> 2;
                while ( k-- ) {
                    tmp = vc1hwdGetBits( strmData, 4 );
                    *pTmp |= (u8)tmp & 8 ? 0 : bit;
                    pTmp++;
                    *pTmp |= (u8)tmp & 4 ? 0 : bit;
                    pTmp++;
                    *pTmp |= (u8)tmp & 2 ? 0 : bit;
                    pTmp++;
                    *pTmp |= (u8)tmp & 1 ? 0 : bit;
                    pTmp++;
                }
                k = colskip & 3;
                while( k-- ) {
                    tmp = vc1hwdGetBits( strmData, 1 ) ? 0 : bit;
                    *pTmp |= (u8)tmp;
                    pTmp++;
                }
            }
            else
            {
                /* Process inverted row */
                if( invert )
                {
                    k = colMb - colskip;
                    pTmp = pData + colskip;
                    while(k--)
                    {
                        *pTmp |= (u8)bit;
                        pTmp++;
                    }
                }
            }
        }
    }

    return rv;
}


/*------------------------------------------------------------------------------

    Function name: DecodeDifferential6

        Functional description:
            Decode Differential-6 mode bitplane.

        Inputs:
            strmData        Pointer to stream data descriptor.
            colMb           Number of macroblock columns.
            rowMb           Number of macroblock rows.
            mbs             Macroblock array.
            bit             Bit to set.
            invert          INVERT bit

        Outputs:

------------------------------------------------------------------------------*/
u16x DecodeDifferential6( strmData_t * const strmData, const u16x colMb,
                          const u16x rowMb, u8 *pData,
                          const u16x bit, const u16x invert )
{

/* Variables */

    u16x tmp, tmp2, k, i;
    u16x tileW, tileH; /* width in tiles */
    u16x rowskip, colskip;
    u16x rv = HANTRO_OK;
    u16x a; /* predictor 'A' in standard */
    u8 * pTmp;
    u32 bits;
    u32 bitsUsed;

/* Code */

    ASSERT( strmData );
    ASSERT( pData );

    if(invert) a = bit;
    else a = 0;

    bits = vc1hwdShowBits( strmData, 32 );
    bitsUsed = 0;

    tmp = colMb % 3;
    if( ( rowMb % 3 ) == 0 && tmp > 0 ) { /* 2x3 tiles */
        tileW = colMb >> 1;
        tileH = rowMb / 3;
        i = 0;
        colskip = colMb & 1;
        rowskip = 0;
        pTmp = pData + colskip;
        while( tileH ) {
            /* Decode next tile codeword */
            k = DecodeTile( bits, &bitsUsed );
            if( bitsUsed > (32-13) )
            {
                (void)vc1hwdFlushBits( strmData, bitsUsed );
                bits = vc1hwdShowBits( strmData, 32 );
                bitsUsed = 0;
            }
            if( k == INVALID_TILE_VLC ) {
                /* Error handling */
                EPRINT(("DecodeDifferential6: Invalid VLC code "
                        "word encountered.\n"));
                rv = HANTRO_NOK;
                k = 0;
            }
            /* Process 2x3 tile */
            tmp = k & 1 ? bit : 0;
            pTmp[0] |= (u8)tmp;
            tmp = k & 4 ? bit : 0;
            pTmp[colMb] |= (u8)tmp;
            tmp = k & 16 ? bit : 0;
            pTmp[2*colMb] |= (u8)tmp;
            pTmp++;
            tmp = k & 2 ? bit : 0;
            pTmp[0] |= (u8)tmp;
            tmp = k & 8 ? bit : 0;
            pTmp[colMb] |= (u8)tmp;
            tmp = k & 32 ? bit : 0;
            pTmp[2*colMb] |= (u8)tmp;
            pTmp++;
            /* Skip to next tile row */
            if( ++i == tileW ) {
                pTmp += 2*colMb + colskip;
                i = 0;
                tileH--;
            }
        }
    }
    else { /* 3x2 tiles */
        tileW = colMb / 3;
        tileH = rowMb >> 1;
        i = 0;
        colskip = tmp;
        rowskip = rowMb & 1;
        pTmp = pData + colskip + rowskip*colMb;
        while( tileH ) {
            /* Decode next tile codeword */
            k = DecodeTile( bits, &bitsUsed );
            if( bitsUsed > (32-13) )
            {
                (void)vc1hwdFlushBits( strmData, bitsUsed );
                bits = vc1hwdShowBits( strmData, 32 );
                bitsUsed = 0;
            }
            if( k == INVALID_TILE_VLC ) {
                /* Error handling */
                EPRINT(("DecodeDifferential6: Invalid VLC code "
                        "word encountered.\n"));
                rv = HANTRO_NOK;
                k = 0;
            }
            /* Process 3x2 tile */
            tmp = k & 1 ? bit : 0;
            pTmp[0] |= (u8)tmp;
            tmp = k & 8 ? bit : 0;
            pTmp[colMb] |= (u8)tmp;
            pTmp++;
            tmp = k & 2 ? bit : 0;
            pTmp[0] |= (u8)tmp;
            tmp = k & 16 ? bit : 0;
            pTmp[colMb] |= (u8)tmp;
            pTmp++;
            tmp = k & 4 ? bit : 0;
            pTmp[0] |= (u8)tmp;
            tmp = k & 32 ? bit : 0;
            pTmp[colMb] |= (u8)tmp;
            pTmp++;
            /* Skip to next tile row */
            if( ++i == tileW ) {
                pTmp += colMb + colskip;
                i = 0;
                tileH--;
            }
        }
    }

    (void)vc1hwdFlushBits( strmData, bitsUsed );

    /* Process column-skip tiles */
    if( colskip > 0 ) {
        DPRINT(("Col-skip %d\n", colskip));
        for( i = 0 ; i < colskip ; ++i ) {
            if( vc1hwdGetBits( strmData, 1 ) == 0 )
            {
                continue;
            }
            pTmp = pData + i;
            k = rowMb >> 2;
            while ( k-- ) {
                tmp = vc1hwdGetBits( strmData, 4 );
                tmp2 = tmp & 8 ? bit : 0;
                *pTmp |= (u8)tmp2;
                pTmp += colMb;
                tmp2 = tmp & 4 ? bit : 0;
                *pTmp |= (u8)tmp2;
                pTmp += colMb;
                tmp2 = tmp & 2 ? bit : 0;
                *pTmp |= (u8)tmp2;
                pTmp += colMb;
                tmp2 = tmp & 1 ? bit : 0;
                *pTmp |= (u8)tmp2;
                pTmp += colMb;
            }
            k = rowMb & 3;
            while ( k-- ) {
                tmp = vc1hwdGetBits( strmData, 1 ) ? bit : 0;
                *pTmp |= (u8)tmp;
                pTmp += colMb;
            }
        }
    }

    /* Process row-skip tiles */
    if( rowskip > 0 ) {
        DPRINT(("Row-skip %d\n", rowskip));
        ASSERT( rowskip == 1 );
        if( vc1hwdGetBits( strmData, 1 ) == 1 )
        {
            pTmp = pData + colskip;
            colskip = colMb - colskip;
            k = colskip >> 2;
            while ( k-- ) {
                tmp = vc1hwdGetBits( strmData, 4 );
                tmp2 = tmp & 8 ? bit : 0;
                *pTmp |= (u8)tmp2;
                pTmp++;
                tmp2 = tmp & 4 ? bit : 0;
                *pTmp |= (u8)tmp2;
                pTmp++;
                tmp2 = tmp & 2 ? bit : 0;
                *pTmp |= (u8)tmp2;
                pTmp++;
                tmp2 = tmp & 1 ? bit : 0;
                *pTmp |= (u8)tmp2;
                pTmp++;
            }
            k = colskip & 3;
            while( k-- ) {
                tmp = vc1hwdGetBits( strmData, 1 ) ? bit : 0;
                *pTmp |= (u8)tmp;
                pTmp++;
            }
        }
    }

    /* Perform differential operation */
    pTmp = pData;
    k = (u16x)(rowMb*colMb)-1;
    i = 1;
    tmp2 = 0;
    *pTmp ^= (u8)a;
    while( k-- ) {
        if( i == colMb ) {
            i = 0;
            tmp2++;
        }
        if( i == 0 )
            tmp = *(pTmp-colMb+1) & bit;
        else if ( tmp2 > 0 &&
            ( ( *(pTmp-colMb+1) ^ *pTmp) & bit ) )
            tmp = a;
        else
            tmp = *pTmp & bit;
        pTmp++;
        *pTmp ^= tmp;
        i++;
    }

    return rv;
}

/*------------------------------------------------------------------------------

    Function name: vc1hwdDecodeBitPlane

        Functional description:
            Decode bitplane.

        Inputs:
            strmData        Pointer to stream data descriptor.
            colMb           Number of macroblock columns.
            rowMb           Number of macroblock rows.
            pData
            bit             Bit value to set.
            pRawMask        Mask to indicate if bits are raw-coded.
            maskbit         Bit value to set in the rawmask.

        Outputs
            mbs             Bitplane, if coding mode other than raw is used.
            pRawMask        Respective invert and raw bits will be set if
                            bitplane uses raw coding mode.

        Returns:
            HANTRO_OK       Bitplane decoded successfully.
            HANTRO_NOK      There was an error in decoding.

------------------------------------------------------------------------------*/
u16x vc1hwdDecodeBitPlane( strmData_t * const strmData, const u16x colMb,
                           const u16x rowMb, u8 *pData, const u16x bit,
                           u16x * const pRawMask,  const u16x maskbit,
                           const u16x syncMarker )
{

/* Variables */

    u16x invert = 0;
    u16x count;
    u16x tmp, tmp2, tmp3, tmp4;
    u16x len;
    u16x rv = HANTRO_OK;
    BitPlaneCodingMode_e imode;
    static const BitPlaneCodingMode_e bpcmTable[3] = {
        BPCM_DIFFERENTIAL_2, BPCM_ROW_SKIP, BPCM_COLUMN_SKIP };
    i16x tmp5;
#if defined(_DEBUG_PRINT)
    u8 *pDataTmp = pData;
#endif

/* Code */

    ASSERT( strmData );
    ASSERT( pData );
    ASSERT( pRawMask );

    /* Decode INVERT and IMODE */
    tmp = vc1hwdShowBits( strmData, 5 );
    if( tmp >= 16 ) {
        tmp -= 16;
        invert = 1;
        DPRINT(("INVERT=1\n"));
    }
    if( tmp >= 8 ) {
        len = 3;
        if( tmp >= 12 )
            imode = BPCM_NORMAL_6;
        else
            imode = BPCM_NORMAL_2;
    } else if ( tmp >= 2 ) {
        tmp = (tmp >> 1) - 1;
        len = 4;
        imode = bpcmTable[tmp];
    } else if ( tmp ) {
        len = 5;
        imode = BPCM_DIFFERENTIAL_6;
    } else {
        len = 5;
        imode = BPCM_RAW;
    }
    (void)vc1hwdFlushBits( strmData, len );

    /* Decode DATABITS */
    if( imode == BPCM_NORMAL_2 ) {
        if(invert) tmp5 = - (i16x)(rowMb*colMb);
        else tmp5 = (i16x)(rowMb*colMb);
        DecodeNormal2( strmData, tmp5, pData, bit );
    }
    else if ( imode == BPCM_DIFFERENTIAL_2 ) {
        if(invert) tmp5 = - (i16x)(rowMb*colMb);
        else tmp5 = (i16x)(rowMb*colMb);
        DecodeDifferential2( strmData, tmp5, pData, bit, colMb);
    }
    else if ( imode == BPCM_NORMAL_6 ) {
        rv = DecodeNormal6( strmData, colMb, rowMb, pData, bit, invert );
    }
    else if ( imode == BPCM_DIFFERENTIAL_6 ) {
        rv = DecodeDifferential6( strmData, colMb, rowMb, pData, bit, invert );
    }
    else { /* use in-line decoding */
        invert *= bit;
        if ( imode == BPCM_ROW_SKIP ) { /* Row-skip mode */
            tmp = rowMb;
            while(tmp--) {
                /* Read ROWSKIP element */
                if(vc1hwdGetBits(strmData, 1)) {
                    /* read ROWBITS */
                    count = colMb;
                    tmp2 = count >> 2;
                    /* Read 4 bit chunks */
                    while( tmp2-- ) {
                        tmp3 = vc1hwdGetBits( strmData, 4 );
                        tmp4 = tmp3 & 8 ? bit : 0;
                        *pData++ |= (u8)(tmp4 ^ invert);
                        tmp4 = tmp3 & 4 ? bit : 0;
                        *pData++ |= (u8)(tmp4 ^ invert);
                        tmp4 = tmp3 & 2 ? bit : 0;
                        *pData++ |= (u8)(tmp4 ^ invert);
                        tmp4 = tmp3 & 1 ? bit : 0;
                        *pData++ |= (u8)(tmp4 ^ invert);
                    }
                    /* Read remainder */
                    for( tmp2 = count & 3 ; tmp2 ; tmp2-- ) {
                        *pData++ |=
                            ((vc1hwdGetBits( strmData, 1)) ? bit : 0 ) ^ invert;
                    }
                } else {
                    /* skip row */
                    if(invert)
                        for( tmp3 = colMb ; tmp3-- ; )
                            *pData++ |= (u8)invert;
                    else
                        pData += colMb;
                }
            }
        }
        else if ( imode == BPCM_COLUMN_SKIP ) { /* Column-skip mode */
            tmp = colMb;
            while( tmp-- ) {
                /* Read COLUMNSKIP element */
                if(vc1hwdGetBits(strmData, 1)) {
                    /* read COLUMNBITS */
                    count = rowMb;
                    tmp2 = count >> 2;
                    /* Read 4 bit chunks */
                    while( tmp2-- ) {
                        tmp3 = vc1hwdGetBits( strmData, 4 );
                        tmp4 = tmp3 & 8 ? bit : 0;
                        *pData |= (u8)(tmp4 ^ invert);
                        pData += colMb;
                        tmp4 = tmp3 & 4 ? bit : 0;
                        *pData |= (u8)(tmp4 ^ invert);
                        pData += colMb;
                        tmp4 = tmp3 & 2 ? bit : 0;
                        *pData |= (u8)(tmp4 ^ invert);
                        pData += colMb;
                        tmp4 = tmp3 & 1 ? bit : 0;
                        *pData |= (u8)(tmp4 ^ invert);
                        pData += colMb;
                    }
                    /* Read remainder */
                    for( tmp2 = count & 3 ; tmp2 ; tmp2-- ) {
                        *pData |=
                            ((vc1hwdGetBits( strmData, 1)) ? bit : 0 ) ^ invert;
                        pData += colMb;
                    }
                    pData -= colMb * rowMb - 1;
                } else {
                    /* skip column */
                    if(invert)
                        for( tmp3 = rowMb, tmp2 = 0 ; tmp3-- ; tmp2 += colMb )
                            pData[tmp2] |= (u8)invert;
                    pData ++;
                }
            }
        }
        else { /* Raw Mode */
            *pRawMask |= maskbit;
        }
    } /* Decode DATABITS */

    /* when syncmarkers enabled -> should be raw coded */ 
    if (syncMarker && ((*pRawMask & maskbit) == 0))
        rv = HANTRO_NOK;

#if defined(_DEBUG_PRINT)
    DPRINT(("vc1hwdDecodeBitPlane\n"));
    for( tmp = 0 ; tmp < rowMb ; ++tmp ) {
        for( tmp2 = 0 ; tmp2 < colMb ; ++tmp2 ) {
            DPRINT(("%d", (*pDataTmp) & bit ? 1 : 0 ));
            pDataTmp++;
        }
        DPRINT(("\n"));
    }
#endif /* defined(_DEBUG_PRINT) */
    return rv;
}

