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
--  Abstract :  RVLC
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_rvlc.c,v $
--  $Date: 2007/11/26 09:57:03 $
--  $Revision: 1.2 $
--
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

#include "mp4dechwd_rvlc.h"
#include "mp4dechwd_utils.h"
#include "mp4debug.h"

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

static u32 RvlcTableSearch(DecContainer *, u32, u32, u32 *);

enum
{ ERROR = 0x7EEEFFFF, EMPTY = 0x00000000, ESCAPE = 0x0000FFFF };

static const u16 u16_rvlcTable1Intra[234] = {
    1025,
    1537,
    514,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2049,
    2561,
    5,
    6,
    34305,
    34817,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    3073,
    3585,
    1026,
    515,
    7,
    36353,
    36865,
    37377,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4097,
    4609,
    1538,
    2050,
    516,
    517,
    8,
    9,
    32770,
    38913,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    5121,
    2562,
    1027,
    1539,
    518,
    10,
    11,
    33282,
    40449,
    40961,
    41473,
    41985,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    5633,
    6145,
    3074,
    3586,
    4098,
    2051,
    1028,
    519,
    12,
    13,
    14,
    43521,
    44033,
    44545,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    6657,
    4610,
    2563,
    3075,
    3587,
    1540,
    1029,
    1030,
    520,
    521,
    15,
    16,
    17,
    32771,
    33794,
    46081,
    0,
    0,
    0,
    0,
    0,
    0,
    5122,
    2052,
    2564,
    3076,
    1541,
    2053,
    522,
    18,
    19,
    22,
    33283,
    34306,
    34818,
    47617,
    48129,
    48641,
    49153,
    49665,
    0,
    0,
    0,
    0,
    7169,
    7681,
    5634,
    4099,
    4611,
    3588,
    1542,
    1031,
    1032,
    1033,
    523,
    20,
    21,
    23,
    32772,
    35330,
    35842,
    36354,
    36866,
    37378,
    0,
    0,
    8193,
    8705,
    9217,
    4100,
    2565,
    2054,
    2566,
    1543,
    1544,
    1034,
    1035,
    524,
    525,
    24,
    25,
    26,
    32773,
    33284,
    37890,
    38402,
    38914,
    52225,
    27,
    1545,
    3077,
    3589,
    4612,
    6146,
    9729,
    33285,
    33795,
    39426,
    53761,
    54273,
    54785,
    55297
};

static const u16 u16_rvlcTable1Inter[234] = {
    3,
    1537,
    2049,
    2561,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    514,
    3073,
    3585,
    4097,
    34305,
    34817,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    1026,
    4609,
    5121,
    5633,
    36353,
    36865,
    37377,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    5,
    6,
    515,
    1538,
    2050,
    6145,
    6657,
    7169,
    32770,
    38913,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    7,
    516,
    1027,
    2562,
    7681,
    8193,
    8705,
    33282,
    40449,
    40961,
    41473,
    41985,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    8,
    9,
    517,
    1539,
    3074,
    3586,
    4098,
    4610,
    9217,
    9729,
    10241,
    43521,
    44033,
    44545,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    10,
    11,
    518,
    1028,
    2051,
    2563,
    5122,
    10753,
    11265,
    11777,
    12289,
    12801,
    13313,
    32771,
    33794,
    46081,
    0,
    0,
    0,
    0,
    0,
    0,
    12,
    519,
    1029,
    1540,
    3075,
    3587,
    5634,
    13825,
    14337,
    14849,
    33283,
    34306,
    34818,
    47617,
    48129,
    48641,
    49153,
    49665,
    0,
    0,
    0,
    0,
    13,
    14,
    15,
    16,
    520,
    1541,
    2052,
    2564,
    4099,
    6146,
    15361,
    15873,
    16385,
    16897,
    32772,
    35330,
    35842,
    36354,
    36866,
    37378,
    0,
    0,
    17,
    18,
    521,
    522,
    1030,
    1031,
    1542,
    3076,
    4611,
    6658,
    7170,
    7682,
    8194,
    17409,
    17921,
    18433,
    32773,
    33284,
    37890,
    38402,
    38914,
    52225,
    19,
    1543,
    2053,
    3588,
    8706,
    18945,
    19457,
    33285,
    33795,
    39426,
    53761,
    54273,
    54785,
    55297
};

static const u16 u16_rvlcTable2Intra[20] = {
    33281,
    33793,
    35329,
    35841,
    37889,
    38401,
    39425,
    39937,
    42497,
    43009,
    45057,
    45569,
    46593,
    47105,
    50177,
    50689,
    51201,
    51713,
    52737,
    53249
};

static const u16 u16_rvlcTable2Inter[20] = {
    33281,
    33793,
    35329,
    35841,
    37889,
    38401,
    39425,
    39937,
    42497,
    43009,
    45057,
    45569,
    46593,
    47105,
    50177,
    50689,
    51201,
    51713,
    52737,
    53249
};

static const u32 ShortIntra[32] = {
    ERROR, ESCAPE, 0x201, 0x3FF, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, 0x3, 0x1FD, 0x8001, 0x81FF,
    0x1, 0x1, 0x1FF, 0x1FF, 0x2, 0x2, 0x1FE, 0x1FE
};
static const u32 ShortInter[32] = {
    ERROR, ESCAPE, 0x2, 0x1FE, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, 0x401, 0x5FF, 0x8001, 0x81FF,
    0x1, 0x1, 0x1FF, 0x1FF, 0x201, 0x201, 0x3FF, 0x3FF
};

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/* ------------- RVLC Table search ------------------------------*/

u32 RvlcTableSearch(DecContainer * pDecContainer, u32 input, u32 mbNumber,
                    u32 * plen)
{
    u32 SecondZero;
    u32 i;
    u32 rlc;
    u32 index;
    i32 level;
    u32 length;
    u32 tmp = 0;
    u32 lastBit = 0;
    u32 sign = 0;

    SecondZero = 999;
    length = 0;

    if(input < 0x80000000)
    {
        /* scan next (2nd) zero */
        for(i = 0; i < 11; i++)
        {
            if(!(input & (0x40000000 >> i)))
            {
                SecondZero = i;
                break;
            }
        }
        if(SecondZero == 999)
        {
            return (ERROR);
        }
        /* scan next (3rd) zero, calculate rvlc length (without sign),
         * save the last bit of rvlc code and the sign bit */
        for(i = (SecondZero + 1); i < 13; i++)
        {
            if(!(input & (0x40000000 >> i)))
            {
                length = i + 4;
                lastBit = input & (0x20000000 >> i) ? 1 : 0;
                sign = input & (0x10000000 >> i);
                break;
            }
        }
        if(!length)
        {
            return (ERROR);
        }

        /* calculate index to table */

        index = ((length - 6) * 22) + (SecondZero * 2) + lastBit;
        if(index >= 234)
        {
            return (ERROR);
        }

        if(MB_IS_INTRA(mbNumber))
        {
            rlc = (u32) u16_rvlcTable1Intra[index];
        }
        else
        {
            rlc = (u32) u16_rvlcTable1Inter[index];
        }
        if(rlc == EMPTY)
        {
            return (ERROR);
        }
        *plen = length;
        if(tmp == END_OF_STREAM)
        {
            return (tmp);
        }
        if(!sign)
        {
            return (rlc);
        }
        else
        {
            level = rlc & 0x1FF;
            level = -level;
            rlc = rlc & 0xFE00;
            rlc = rlc | (level & 0x1FF);
            return (rlc);
        }
    }
    else
    {
        /* 1st bit is 1 (msb) */
        /* scan next high bit (starting from bit number 3) */
        for(i = 2; i < 12; i++)
        {
            if(input & (0x40000000 >> i))
            {
                lastBit = input & (0x20000000 >> i) ? 1 : 0;
                sign = input & (0x10000000 >> i);
                length = i + 4;
                break;
            }
        }
        if(!length)
        {
            return (ERROR);
        }
        index = (length - 6) * 2 + lastBit;

        if(MB_IS_INTRA(mbNumber))
        {
            rlc = u16_rvlcTable2Intra[index];
        }
        else
        {
            rlc = u16_rvlcTable2Inter[index];
        }

        *plen = length;
        if(tmp == END_OF_STREAM)
        {
            return (tmp);
        }
        if(!sign)
        {
            return (rlc);
        }
        else
        {
            level = rlc & 0x1FF;
            level = -level;
            rlc = rlc & 0xFE00;
            rlc = rlc | (level & 0x1FF);
            return (rlc);
        }
    }
}

/*------------------------------------------------------------------------------

   5.1  Function name:  StrmDec_DecodeRvlc

        Purpose:        To decode forward direction

        Input:          MPEG-4 stream

        Output:         Transform coefficients (Last, run, level)

------------------------------------------------------------------------------*/
u32 StrmDec_DecodeRvlc(DecContainer * pDecContainer, u32 mbNumber,
    u32 mbNumbs)
{

    u32 tmpBuf;
    u32 shiftt;
    u32 usedBits;
    u32 length;
    u32 last;
    u32 MbNo;
    u32 rlc;
    u32 blockNo;
    u32 codedBlocks;
    u32 decodeBlock;
    u32 CodeCount;
    u32 escapeTmp;
    u32 run, sign;
    i32 level;
    u32 rlcAddrCount;
    u32 tmp = 0;

    MP4DEC_API_DEBUG((" Rvlc_Decode # \n"));
    /* read in 32 bits */
    shiftt = usedBits = 0;
    SHOWBITS32(tmpBuf);

    for(MbNo = 0; MbNo < mbNumbs; MbNo++)
    {

        if(pDecContainer->StrmStorage.codedBits[mbNumber + MbNo] & 0x3F)
        {
            codedBlocks =
                pDecContainer->StrmStorage.codedBits[mbNumber + MbNo];

            /* Check that there is enough 'space' in rlc data buffer (max
             * locations needed by block is 64) */
            if((i32)
               ((pDecContainer->MbSetDesc.pRlcDataCurrAddr + 64 -
                 pDecContainer->MbSetDesc.pRlcDataAddr)) <=
               (i32) (pDecContainer->MbSetDesc.rlcDataBufferSize))
            {

                for(blockNo = 0; blockNo < 6; blockNo++)
                {
                    decodeBlock = codedBlocks & ((0x20) >> blockNo);
                    CodeCount = 0;
                    if(decodeBlock)
                    {
                        u32 oddRlcTmp = 0;

                        rlcAddrCount = 0;
                        do
                        {
                            length = 0;
                            /* short word search */
                            if(MB_IS_INTRA(mbNumber + MbNo))
                            {
                                rlc = ShortIntra[tmpBuf >> 27];
                            }
                            else
                            {
                                rlc = ShortInter[tmpBuf >> 27];
                            }

                            if(rlc == EMPTY)
                            {
                                /* no escape or indexes between 0-4 or
                                 * ERROR-sig, normal table search */
                                rlc = RvlcTableSearch(pDecContainer, tmpBuf,
                                                      (mbNumber + MbNo),
                                                      &length);
                                usedBits += length;
                                shiftt = length;
                            }
                            else if(rlc == ESCAPE)
                            {
                                /* escape code (25 + 5 bits long) */
                                FLUSHBITS((usedBits + 5));
                                SHOWBITS32(escapeTmp);
                                usedBits = shiftt = 25;
                                last = escapeTmp >> 31;
                                run = (escapeTmp & 0x7FFFFFFF) >> 25;
                                level = (escapeTmp & 0x00FFFFFF) >> 13;
                                sign = (escapeTmp & 0x000000FF) >> 7;
                                /* check marker bits & escape from the end */
                                if(!(escapeTmp & 0x01000000) ||
                                   !(escapeTmp & 0x00001000) ||
                                   (escapeTmp & 0x00000F00))
                                {
                                    return (HANTRO_NOK);
                                }
                                if(sign)
                                    level = -level;
                                if((level > 255) || (level < -256))
                                {
                                    rlc = ((level << 16) | (last << 15) |
                                           (run << 9)) & 0xFFFFFE00;
                                }
                                else
                                {
                                    rlc = (last << 15) | (run << 9) |
                                        (level & 0x1ff);
                                }
                            }
                            else
                            {
                                /* Short word -> update used bits */
                                {
                                    u32 cmpTmp = 0;

                                    cmpTmp = (tmpBuf < 0xC0000000);
                                    length = 4 + cmpTmp;
                                    usedBits += length;
                                    shiftt = length;
                                }
                            }

                            if(rlc == ERROR)
                                return (HANTRO_NOK);

                            if(usedBits > 16)
                            {
                                /* Not enough bits in input buffer */
                                FLUSHBITS(usedBits);
                                usedBits = shiftt = 0;
                                SHOWBITS32(tmpBuf);
                            }
                            else
                            {
                                tmpBuf <<= shiftt;
                                shiftt = 0;
                            }

                            /* SAVING RLC WORD'S */

                            /* if half of a word was left empty last time, fill
                             * it first */
                            if(pDecContainer->MbSetDesc.oddRlc)
                            {
                                oddRlcTmp =
                                    pDecContainer->MbSetDesc.oddRlc;
                                tmp =
                                    *pDecContainer->MbSetDesc.pRlcDataCurrAddr;
                            }
                            /* odd address count -> start saving in 15:0 */
                            /* oddRlcTmp means that a word was left halp empty
                             * in the last block */

                            if((rlcAddrCount + oddRlcTmp) & 0x01)
                            {

                                pDecContainer->MbSetDesc.oddRlc = 0;
                                rlcAddrCount++;
                                if((rlc & 0x1FF) == 0)
                                {
                                    rlcAddrCount++;
                                    tmp |= (0xFFFF & rlc);
                                    *pDecContainer->MbSetDesc.
                                        pRlcDataCurrAddr++ = tmp;

                                    tmp = (rlc & 0xFFFF0000);

                                }
                                else
                                {
                                    tmp = tmp | (rlc & 0xFFFF);
                                }

                            }

                            /* even address count -> start saving in 31:16 */
                            else
                            {
                                rlcAddrCount++;
                                if((rlc & 0x1FF) == 0)  /* BIG level  */
                                {
                                    rlcAddrCount++;

                                    tmp = ((rlc & 0xFFFF) << 16);
                                    tmp = tmp | (rlc >> 16 /*& 0xFFFF */ );

                                }
                                else
                                {
                                    tmp = (rlc << 16);
                                }
                            }

                            if(((rlcAddrCount + oddRlcTmp) & 0x01) == 0)
                            {
                                *pDecContainer->MbSetDesc.
                                    pRlcDataCurrAddr++ = tmp;
                            }

                            last = rlc & 0x8000;
                            CodeCount += (((rlc & 0x7E00) >> 9) + 1);

                            if(CodeCount > 64)
                            {
                                return (HANTRO_NOK);
                            }

                        }
                        while(!last);

                        /* lonely 16 bits stored in tmp -> write to asic input
                         * buffer */
                        if(((rlcAddrCount + oddRlcTmp) & 0x01) == 1)
                        {
                            *pDecContainer->MbSetDesc.
                                pRlcDataCurrAddr = tmp;
                            pDecContainer->MbSetDesc.oddRlc = 1;
                        }
                    }   /* end of decodeBlock */
                }   /* end of block-loop */

            }
            else
            {
                return (HANTRO_NOK);
            }

        }   /* end coded loop */
    }   /* end of mb-loop */
    /* flush used bits */
    if(usedBits)
        FLUSHBITS(usedBits);
    return (HANTRO_OK);
}   /* end of function */
