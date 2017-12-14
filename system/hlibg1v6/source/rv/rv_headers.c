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
--  $RCSfile: rv_headers.c,v $
--  $Date: 2009/05/15 08:28:37 $
--  $Revision: 1.6 $
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
#include "rv_headers.h"
#include "rv_utils.h"
#include "rv_strm.h"
#include "rv_cfg.h"
#include "rv_debug.h"

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

static const u32 picWidth[8] = {160, 176, 240, 320, 352, 640, 704, 0};
static const u32 picHeight[8] = {120, 132, 144, 240, 288, 480, 0, 0};
static const u32 picHeight2[5] = {180, 360, 576, 0};
static const u32 maxPicSize[] = {48, 99, 396, 1584, 6336, 9216};

#define IS_INTRA_PIC(picType) (picType < RV_P_PIC)

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   5.1  Function name:
                GetPictureSize

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 GetPictureSize(DecContainer * pDecContainer, DecHdrs *pHdrs)
{

    u32 tmp;
    u32 bits;
    u32 w, h;

    tmp = rv_GetBits(pDecContainer, 3);
    if (tmp == END_OF_STREAM)
        return(tmp);
    w = picWidth[tmp];
    if (!w)
    {
        do {
            tmp = rv_GetBits(pDecContainer, 8);
            w += tmp << 2;
        } while (tmp == 255);
    }
    
    bits = pDecContainer->StrmDesc.strmBuffReadBits;

    tmp = rv_GetBits(pDecContainer, 3);
    if (tmp == END_OF_STREAM)
        return(tmp);
    h = picHeight[tmp];
    if (!h)
    {
        tmp = (tmp<<1) | rv_GetBits(pDecContainer, 1);
        tmp &= 0x3;
        h = picHeight2[tmp];
        if (!h)
        {
            do {
                tmp = rv_GetBits(pDecContainer, 8);
                h += tmp << 2;
            } while (tmp == 255);
            if (tmp == END_OF_STREAM)
                return(tmp);
        }
    }

    pHdrs->horizontalSize = w;
    pHdrs->verticalSize = h;
   
    pDecContainer->StrmStorage.frameSizeBits =
        pDecContainer->StrmDesc.strmBuffReadBits - bits;

    return HANTRO_OK;

}

static u32 MbNumLen(u32 numMbs)
{

    u32 tmp;

    if( numMbs <= 48 )          tmp = 6;
    else if( numMbs <= 99   )   tmp = 7;
    else if( numMbs <= 396  )   tmp = 9;
    else if( numMbs <= 1584 )   tmp = 11;
    else if( numMbs <= 6336 )   tmp = 13;
    else                        tmp = 14;
    return tmp;

}

/*------------------------------------------------------------------------------

   5.1  Function name:
                rv_DecodeSliceHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/

#ifdef RV_RAW_STREAM_SUPPORT

#define PACK_LENGTH_AND_INFO(length, info) (((length) << 4) | (info))

static const u8 rvVlcTable[256] =
{
    PACK_LENGTH_AND_INFO(8,  0),   /* 00000000 */
    PACK_LENGTH_AND_INFO(8,  1),   /* 00000001 */
    PACK_LENGTH_AND_INFO(7,  0),   /* 00000010 */
    PACK_LENGTH_AND_INFO(7,  0),   /* 00000011 */
    PACK_LENGTH_AND_INFO(8,  2),   /* 00000100 */
    PACK_LENGTH_AND_INFO(8,  3),   /* 00000101 */
    PACK_LENGTH_AND_INFO(7,  1),   /* 00000110 */
    PACK_LENGTH_AND_INFO(7,  1),   /* 00000111 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001000 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001001 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001010 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001011 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001100 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001101 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001110 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001111 */
    PACK_LENGTH_AND_INFO(8,  4),   /* 00010000 */
    PACK_LENGTH_AND_INFO(8,  5),   /* 00010001 */
    PACK_LENGTH_AND_INFO(7,  2),   /* 00010010 */
    PACK_LENGTH_AND_INFO(7,  2),   /* 00010011 */
    PACK_LENGTH_AND_INFO(8,  6),   /* 00010100 */
    PACK_LENGTH_AND_INFO(8,  7),   /* 00010101 */
    PACK_LENGTH_AND_INFO(7,  3),   /* 00010110 */
    PACK_LENGTH_AND_INFO(7,  3),   /* 00010111 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011000 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011001 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011010 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011011 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011100 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011101 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011110 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111111 */
    PACK_LENGTH_AND_INFO(8,  8),   /* 01000000 */
    PACK_LENGTH_AND_INFO(8,  9),   /* 01000001 */
    PACK_LENGTH_AND_INFO(7,  4),   /* 01000010 */
    PACK_LENGTH_AND_INFO(7,  4),   /* 01000011 */
    PACK_LENGTH_AND_INFO(8, 10),   /* 01000100 */
    PACK_LENGTH_AND_INFO(8, 11),   /* 01000101 */
    PACK_LENGTH_AND_INFO(7,  5),   /* 01000110 */
    PACK_LENGTH_AND_INFO(7,  5),   /* 01000111 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001000 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001001 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001010 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001011 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001100 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001101 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001110 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001111 */
    PACK_LENGTH_AND_INFO(8, 12),   /* 01010000 */
    PACK_LENGTH_AND_INFO(8, 13),   /* 01010001 */
    PACK_LENGTH_AND_INFO(7,  6),   /* 01010010 */
    PACK_LENGTH_AND_INFO(7,  6),   /* 01010011 */
    PACK_LENGTH_AND_INFO(8, 14),   /* 01010100 */
    PACK_LENGTH_AND_INFO(8, 15),   /* 01010101 */
    PACK_LENGTH_AND_INFO(7,  7),   /* 01010110 */
    PACK_LENGTH_AND_INFO(7,  7),   /* 01010111 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011000 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011001 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011010 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011011 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011100 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011101 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011110 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111110 */
    PACK_LENGTH_AND_INFO(1,  0)    /* 11111111 */
};

static u32 RvDecodeCode( DecContainer *pD, u32 *code )
{

/* Variables */

    u32 bits, len;
    u32 info;
    u32 symbol = 0;
    u32 chunkLen;

/* Code */

    len = 0;

    do
    {
        bits = rv_ShowBits(pD, 8);
        info = rvVlcTable[bits];
        chunkLen = info>>4;
        symbol = (symbol << (chunkLen>>1)) | (info & 0xF);
        len += chunkLen;

        if (rv_FlushBits(pD, chunkLen) == END_OF_STREAM)
            return END_OF_STREAM;
    } while (!(len&0x1));

    *code = symbol;

    return len;
}
#endif

u32 rv_DecodeSliceHeader(DecContainer * pDecContainer)
{
    u32 i, tmp;
    DecHdrs *pHdrs;

    ASSERT(pDecContainer);

    RVDEC_DEBUG(("Decode Slice Header Start\n"));

    pHdrs = pDecContainer->StrmStorage.strmDecReady == FALSE ?
        &pDecContainer->Hdrs : &pDecContainer->tmpHdrs;

#ifdef RV_RAW_STREAM_SUPPORT
    if (pDecContainer->StrmStorage.rawMode)
    {
        if (pDecContainer->StrmStorage.isRv8)
        {
            if (rv_ShowBits(pDecContainer, 24) != 0x000001)
                return HANTRO_NOK;
            rv_FlushBits(pDecContainer, 24);
        }
        else
        {
            if (rv_ShowBits(pDecContainer, 32) != 0x55555555)
                return HANTRO_NOK;
            rv_FlushBits(pDecContainer, 32);
        }

        tmp = RvDecodeCode(pDecContainer, &i);
        if (tmp < 31 || (i & 1))
            return HANTRO_NOK;

        if (!((i>>1)&1))
        {
            pHdrs->horizontalSize = 176;
            pHdrs->verticalSize = 144;
        }
        else
        {
            pHdrs->horizontalSize = 0;
            pHdrs->verticalSize = 0;
        }
        pDecContainer->FrameDesc.qp = (i >> 2) & 0x1F;
        pHdrs->temporalReference = (i >> 7) & 0xFF;

        /* picture type */
        tmp = RvDecodeCode(pDecContainer, &i);
        if (tmp == 1)
            pDecContainer->FrameDesc.picCodingType = RV_P_PIC;
        else if (tmp == 3 && i == 1)
            pDecContainer->FrameDesc.picCodingType = RV_I_PIC;
        else if (tmp == 5 && i == 0)
            pDecContainer->FrameDesc.picCodingType = RV_B_PIC;
        else
            return HANTRO_NOK;

        if (pHdrs->horizontalSize == 0)
        {
            /* pixel_aspect_ratio */
            rv_FlushBits(pDecContainer, 4);
            tmp = rv_GetBits(pDecContainer, 9);
            pHdrs->horizontalSize = (tmp + 1)  * 4;
            if (!rv_GetBits(pDecContainer, 1))
                return HANTRO_NOK;
            tmp = rv_GetBits(pDecContainer, 9);
            pHdrs->verticalSize = tmp * 4;
        }

        if (!pDecContainer->StrmStorage.isRv8)
            pDecContainer->FrameDesc.vlcSet = rv_GetBits(pDecContainer, 2);
        else
            pDecContainer->FrameDesc.vlcSet = 0;

        pDecContainer->FrameDesc.frameWidth =
            (pHdrs->horizontalSize + 15) >> 4;
        pDecContainer->FrameDesc.frameHeight =
            (pHdrs->verticalSize + 15) >> 4;
        pDecContainer->FrameDesc.totalMbInFrame =
            (pDecContainer->FrameDesc.frameWidth *
             pDecContainer->FrameDesc.frameHeight);

    }
    else
#endif
    if (pDecContainer->StrmStorage.isRv8)
    {
        /* bitstream version */
        tmp = rv_GetBits(pDecContainer, 3);

        tmp = pDecContainer->FrameDesc.picCodingType = 
            rv_GetBits(pDecContainer, 2);
        if (pDecContainer->FrameDesc.picCodingType == RV_FI_PIC)
            pDecContainer->FrameDesc.picCodingType = RV_I_PIC;

        /* ecc */
        tmp = rv_GetBits(pDecContainer, 1);
        
        /* qp */
        tmp = pDecContainer->FrameDesc.qp =
            rv_GetBits(pDecContainer, 5);

        /* deblocking filter flag */
        tmp = rv_GetBits(pDecContainer, 1);

        tmp = pHdrs->temporalReference = rv_GetBits(pDecContainer, 13);

        /* frame size code */
        tmp = rv_GetBits(pDecContainer,
            pDecContainer->StrmStorage.frameCodeLength);

        pHdrs->horizontalSize = pDecContainer->StrmStorage.frameSizes[2*tmp];
        pHdrs->verticalSize = pDecContainer->StrmStorage.frameSizes[2*tmp+1];

        pDecContainer->FrameDesc.frameWidth =
            (pHdrs->horizontalSize + 15) >> 4;
        pDecContainer->FrameDesc.frameHeight =
            (pHdrs->verticalSize + 15) >> 4;
        pDecContainer->FrameDesc.totalMbInFrame =
            (pDecContainer->FrameDesc.frameWidth *
             pDecContainer->FrameDesc.frameHeight);

        /* first mb in slice */
        tmp = rv_GetBits(pDecContainer,
            MbNumLen(pDecContainer->FrameDesc.totalMbInFrame));
        if (tmp == END_OF_STREAM)
            return tmp;

        /* rounding type */
        tmp = rv_GetBits(pDecContainer, 1);

    }
    else
    {

        /* TODO: shall be 0, check? */
        tmp = rv_GetBits(pDecContainer, 1);

        tmp = pDecContainer->FrameDesc.picCodingType = 
            rv_GetBits(pDecContainer, 2);
        if (pDecContainer->FrameDesc.picCodingType == RV_FI_PIC)
            pDecContainer->FrameDesc.picCodingType = RV_I_PIC;

        /* qp */
        tmp = pDecContainer->FrameDesc.qp =
            rv_GetBits(pDecContainer, 5);
            
        /* TODO: shall be 0, check? */
        tmp = rv_GetBits(pDecContainer, 2);

        tmp = pDecContainer->FrameDesc.vlcSet = 
            rv_GetBits(pDecContainer, 2);

        /* deblocking filter flag */
        tmp = rv_GetBits(pDecContainer, 1);

        tmp = pHdrs->temporalReference = rv_GetBits(pDecContainer, 13);

        /* picture size */
        if (IS_INTRA_PIC(pDecContainer->FrameDesc.picCodingType) ||
            rv_GetBits(pDecContainer, 1) == 0)
        {
            GetPictureSize(pDecContainer, pHdrs);
        }
        
        pDecContainer->FrameDesc.frameWidth =
            (pHdrs->horizontalSize + 15) >> 4;
        pDecContainer->FrameDesc.frameHeight =
            (pHdrs->verticalSize + 15) >> 4;
        pDecContainer->FrameDesc.totalMbInFrame =
            (pDecContainer->FrameDesc.frameWidth *
             pDecContainer->FrameDesc.frameHeight);

        /* first mb in slice */
        tmp = rv_GetBits(pDecContainer,
            MbNumLen(pDecContainer->FrameDesc.totalMbInFrame));
        if (tmp == END_OF_STREAM)
            return tmp;

        /* TODO: check that this slice starts from mb 0 */
    }

    /* check that picture size does not exceed maximum */
    if ( !pDecContainer->StrmStorage.rawMode &&
         (pHdrs->horizontalSize > pDecContainer->StrmStorage.maxFrameWidth ||
          pHdrs->verticalSize   > pDecContainer->StrmStorage.maxFrameHeight) )
        return HANTRO_NOK;

    if(pDecContainer->StrmStorage.strmDecReady)
    {
        if( pHdrs->horizontalSize != pDecContainer->Hdrs.horizontalSize ||
            pHdrs->verticalSize != pDecContainer->Hdrs.verticalSize )
        {
            /*pDecContainer->ApiStorage.firstHeaders = 1;*/
            /*pDecContainer->StrmStorage.strmDecReady = HANTRO_FALSE;*/

            /* Resolution change delayed */
            pDecContainer->StrmStorage.rprDetected = 1;
            /* Get picture type from here to perform resampling */
            pDecContainer->StrmStorage.rprNextPicType = 
                pDecContainer->FrameDesc.picCodingType;
        }
        if (pDecContainer->FrameDesc.picCodingType != RV_B_PIC)
        {
            pDecContainer->StrmStorage.prevTr = pDecContainer->StrmStorage.tr;
            pDecContainer->StrmStorage.tr = pHdrs->temporalReference;
        }
        else
        {
            i32 trb, trd;

            trb = pHdrs->temporalReference -
                pDecContainer->StrmStorage.prevTr;
            trd = pDecContainer->StrmStorage.tr -
                pDecContainer->StrmStorage.prevTr;

            if (trb < 0) trb += (1<<13);
            if (trd < 0) trd += (1<<13);
            
            pDecContainer->StrmStorage.trb = trb;

            /* current time stamp not between the fwd and bwd references */
            if (trb > trd)
                trb = trd/2;

            if (trd)
            {
                pDecContainer->StrmStorage.bwdScale = (trb << 14) / trd;
                pDecContainer->StrmStorage.fwdScale = ((trd-trb) << 14) / trd;
            }
            else
            {
                pDecContainer->StrmStorage.fwdScale = 0;
                pDecContainer->StrmStorage.bwdScale = 0;
            }
        }
    }

    RVDEC_DEBUG(("Decode Slice Header Done\n"));

    return (HANTRO_OK);
}
