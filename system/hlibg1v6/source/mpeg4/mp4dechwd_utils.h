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
--  Abstract : Header file for stream decoding utilities
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_utils.h,v $
--  $Date: 2008/01/14 14:36:24 $
--  $Revision: 1.3 $
--
------------------------------------------------------------------------------*/

#ifndef STRMDEC_UTILS_H_DEFINED
#define STRMDEC_UTILS_H_DEFINED

#include "mp4dechwd_container.h"

/* constant definitions */
#ifndef HANTRO_OK
#define HANTRO_OK 0
#endif

#ifndef HANTRO_NOK
#define HANTRO_NOK 1
#endif

#ifndef HANTRO_FALSE
#define HANTRO_FALSE 0
#endif

#ifndef HANTRO_TRUE
#define HANTRO_TRUE 1
#endif

#define AMBIGUOUS 2

#ifndef NULL
#define NULL 0
#endif

/* decoder states */
enum
{
    STATE_OK,
    STATE_NOT_READY,
    STATE_SYNC_LOST
};

/* start codes */
enum
{
    SC_VO_START = 0x00000100,
    SC_VOL_START = 0x00000120,
    SC_VOS_START = 0x000001B0,
    SC_VOS_END = 0x000001B1,
    SC_UD_START = 0x000001B2,
    SC_GVOP_START = 0x000001B3,
    SC_VISO_START = 0x000001B5,
    SC_VOP_START = 0x000001B6,
    SC_RESYNC = 0x1,
    SC_SV_START = 0x20,
    SC_SORENSON_START = 0x10,
    SC_SV_END = 0x3F,
    SC_NOT_FOUND = 0xFFFE,
    SC_ERROR = 0xFFFF
};

enum
{
    MB_INTER = 0,
    MB_INTERQ = 1,
    MB_INTER4V = 2,
    MB_INTRA = 3,
    MB_INTRAQ = 4,
    MB_STUFFING = 5
};

enum
{
    IVOP = 0,
    PVOP = 1,
    BVOP = 2,
    NOT_SET
};

/*enum {
    DEC = 0,
    HEX = 1,
    BIN = 2
};*/

/* value to be returned by StrmDec_GetBits if stream buffer is empty */
#define END_OF_STREAM 0xFFFFFFFFU

/* How many control words(32bit) does one mb take */

#define NBR_OF_WORDS_MB 1

/* how many motion vector data words for mb */

#define NBR_MV_WORDS_MB 4

/* how many DC VLC data words for mb */

#define NBR_DC_WORDS_MB 2

enum
{
    OUT_OF_BUFFER = 0xFF
};

/* Bit positions in Asic CtrlBits memory ( specified in top level spec ) */

enum
{
    ASICPOS_MBTYPE = 31,
    ASICPOS_4MV = 30,
    ASICPOS_USEINTRADCVLC = 30,
    ASICPOS_VPBI = 29,
    ASICPOS_ACPREDFLAG = 28,
    ASICPOS_QP = 16,
    ASICPOS_CONCEAL = 15,
    ASICPOS_MBNOTCODED = 14
};

/* Boundary strenght positions in ctrlBits  */

enum
{
    BS_VER_00 = 13,
    BS_HOR_00 = 10,
    BS_VER_02 = 7,
    BS_HOR_01 = 4,
    BS_VER_04 = 1,
    BS_HOR_02_MSB = 0,
    BS_HOR_02_LSB = 30,
    BS_VER_06 = 27,
    BS_HOR_03 = 24,
    BS_VER_08 = 21,
    BS_HOR_08 = 18,
    BS_VER_10 = 15,
    BS_HOR_09 = 12,
    BS_VER_12 = 9,
    BS_HOR_10 = 6,
    BS_VER_14 = 3,
    BS_HOR_11 = 0
};

enum
{

    ASIC_ZERO_MOTION_VECTORS_LUM = 0,
    ASIC_ZERO_MOTION_VECTORS_CHR = 0
};

enum
{
    MB_NOT_CODED = 0x80
};

#define MB_IS_INTRA(mbNumber) \
    ( (pDecContainer->MBDesc[mbNumber].typeOfMB == MB_INTRA) || \
      (pDecContainer->MBDesc[mbNumber].typeOfMB == MB_INTRAQ) )

#define MB_IS_INTER(mbNumber) \
    (pDecContainer->MBDesc[mbNumber].typeOfMB <= MB_INTER4V)

#define MB_IS_INTER4V(mbNumber) \
    (pDecContainer->MBDesc[mbNumber].typeOfMB == MB_INTER4V)

#define MB_IS_STUFFING(mbNumber) \
    (pDecContainer->MBDesc[mbNumber].typeOfMB == MB_STUFFING)

#define MB_HAS_DQUANT(mbNumber) \
    ( (pDecContainer->MBDesc[mbNumber].typeOfMB == MB_INTERQ) || \
      (pDecContainer->MBDesc[mbNumber].typeOfMB == MB_INTRAQ) )

/* macro to check if stream ends */
#define IS_END_OF_STREAM(pContainer) \
    ( (pContainer)->StrmDesc.strmBuffReadBits == \
      (8*(pContainer)->StrmDesc.strmBuffSize) )

#define SATURATE(min,value,max) \
    if ((value) < (min)) (value) = (min); \
    else if ((value) > (max)) (value) = (max);

#define SHOWBITS32(tmp) \
{ \
    i32 bits, shift; \
    const u8 *pstrm = pDecContainer->StrmDesc.pStrmCurrPos; \
    bits = (i32)pDecContainer->StrmDesc.strmBuffSize*8 - \
               (i32)pDecContainer->StrmDesc.strmBuffReadBits; \
    if (bits >= 32) \
    { \
        tmp = ((u32)pstrm[0] << 24) | ((u32)pstrm[1] << 16) |  \
                  ((u32)pstrm[2] <<  8) | ((u32)pstrm[3]); \
        if (pDecContainer->StrmDesc.bitPosInWord) \
        { \
            tmp <<= pDecContainer->StrmDesc.bitPosInWord; \
            tmp |= \
                (u32)pstrm[4]>>(8-pDecContainer->StrmDesc.bitPosInWord); \
        } \
    } \
    else if (bits) \
    { \
        shift = 24 + pDecContainer->StrmDesc.bitPosInWord; \
        tmp = (u32)(*pstrm++) << shift; \
        bits -= 8 - pDecContainer->StrmDesc.bitPosInWord; \
        while (bits > 0) \
        { \
            shift -= 8; \
            tmp |= (u32)(*pstrm++) << shift; \
            bits -= 8; \
        } \
    } \
    else \
        tmp = 0; \
}
#define FLUSHBITS(bits) \
{ \
    u32 FBtmp;\
    if ( (pDecContainer->StrmDesc.strmBuffReadBits + bits) > \
         (8*pDecContainer->StrmDesc.strmBuffSize) ) \
    { \
    pDecContainer->StrmDesc.strmBuffReadBits = \
            8 * pDecContainer->StrmDesc.strmBuffSize; \
        pDecContainer->StrmDesc.bitPosInWord = 0; \
        pDecContainer->StrmDesc.pStrmCurrPos = \
            pDecContainer->StrmDesc.pStrmBuffStart + \
            pDecContainer->StrmDesc.strmBuffSize;\
        return(END_OF_STREAM);\
    }\
    else\
    {\
        pDecContainer->StrmDesc.strmBuffReadBits += bits;\
        FBtmp = pDecContainer->StrmDesc.bitPosInWord + bits;\
        pDecContainer->StrmDesc.pStrmCurrPos += FBtmp >> 3;\
        pDecContainer->StrmDesc.bitPosInWord = FBtmp & 0x7;\
    }\
}

/* function prototypes */

u32 StrmDec_GetBits(DecContainer *, u32 numBits);
u32 StrmDec_ShowBits(DecContainer *, u32 numBits);
u32 StrmDec_ShowBitsAligned(DecContainer *, u32 numBits, u32 numBytes);
u32 StrmDec_FlushBits(DecContainer *, u32 numBits);
u32 StrmDec_UnFlushBits(DecContainer *, u32 numBits);
void StrmDec_ProcessPacketFooter( DecContainer * );

u32 StrmDec_GetStuffing(DecContainer *);
u32 StrmDec_CheckStuffing(DecContainer *);

u32 StrmDec_FindSync(DecContainer *);
u32 StrmDec_GetStartCode(DecContainer *);
u32 StrmDec_ProcessBvopExtraResync(DecContainer *);

u32 StrmDec_NumBits(u32 value);

#endif
