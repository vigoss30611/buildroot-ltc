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
--  $RCSfile: mpeg2hwd_utils.h,v $
--  $Date: 2008/02/19 10:51:10 $
--  $Revision: 1.5 $
--
------------------------------------------------------------------------------*/

#ifndef STRMDEC_UTILS_H_DEFINED
#define STRMDEC_UTILS_H_DEFINED

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mpeg2hwd_container.h"

#ifdef _ASSERT_USED
#include <assert.h>
#endif

#ifdef _UTEST
#include <stdio.h>
#endif

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/* constant definitions */
#ifndef OK
#define OK 0
#endif

#ifndef NOK
#define NOK 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

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

#define HANTRO_OK 0
#define HANTRO_NOK 1

#ifndef NULL
#define NULL 0
#endif

/* picture structure */
#define TOPFIELD     1
#define BOTTOMFIELD  2
#define FRAMEPICTURE 3

/* Error concealment */
#define	FREEZED_PIC_RDY 1

/* start codes */
enum
{
    SC_PICTURE = 0x00,
    SC_SLICE = 0x01,    /* throug AF */
    SC_USER_DATA = 0xB2,
    SC_SEQUENCE = 0xB3,
    SC_SEQ_ERROR = 0xB4,
    SC_EXTENSION = 0xB5,
    SC_SEQ_END = 0xB7,
    SC_GROUP = 0xB8,
    SC_RESYNC = 0x4,
    SC_SV_START = 0x20,
    SC_SV_END = 0x3F,
    SC_NOT_FOUND = 0xFFFE,
    SC_ERROR = 0xFFFF
};

/* start codes */
enum
{
    SC_SEQ_EXT = 0x01,
    SC_SEQ_DISPLAY_EXT = 0x02,
    SC_QMATRIX_EXT = 0x03,
    SC_COPYRIGHT_EXT = 0x04,
    SC_SEQ_SCALABLE_EXT = 0x05,
    SC_PIC_DISPLAY_EXT = 0x07,
    SC_PIC_CODING_EXT = 0x08,
    SC_PIC_SPATIAL_SCALABLE_EXT = 0x09,
    SC_PIC_TEMPORAL_SCALABLE_EXT = 0x0A,
    SC_CAMERA_PARAM_EXT = 0x0B,
    SC_ITU_T_EXT = 0x0C
};

enum
{
    IFRAME = 1,
    PFRAME = 2,
    BFRAME = 3,
    DFRAME = 4
};

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

/* value to be returned by GetBits if stream buffer is empty */
#define END_OF_STREAM 0xFFFFFFFFU

/* macro for debug printing. Note that double parenthesis has to be used, i.e.
 * DEBUG(("Debug printing %d\n",%d)) */
#ifdef _UTEST
#define DEBUG(args) printf args
#else
#define DEBUG(args)
#endif

/* macro for assertion, used only if compiler flag _ASSERT_USED is defined */
#ifdef _ASSERT_USED
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr)
#endif

/* macro to check if stream ends */
#define IS_END_OF_STREAM(pContainer) \
    ( (pContainer)->StrmDesc.strmBuffReadBits == \
      (8*(pContainer)->StrmDesc.strmBuffSize) )

/* macro to saturate value to range [min,max]. Note that for unsigned value
 * both min and max should be positive, otherwise result will be wrong due to
 * arithmetic conversion. If min > max -> value will be equal to min. */
#define SATURATE(min,value,max) \
    if ((value) < (min)) (value) = (min); \
    else if ((value) > (max)) (value) = (max);

#define ABS(val) (((val) < 0) ? -(val) : (val))

#define DEQUANTIZE(val,QP) \
{   /*lint -e514 */ \
    if ((val) < 0) \
    { \
        (val) = (i32)(QP)*(2*(val) - 1) + !((QP)&0x1); \
        if ((val) < -2048) (val) = -2048; \
    } \
    else if ((val) > 0) \
    { \
        (val) = (i32)(QP)*(2*(val) + 1) - !((QP)&0x1); \
        if ((val) > 2047) (val)=2047; \
    }/*lint +e514 */  \
}

/* following four macros are used to handle temporary bit buffer of type
 * tmpBuffer_t. BUFFER_SHOW 'shows' numBits bits in outVal. Bits are right
 * aligned. BUFFER_FLUSH removes numBits from temporal buffer. There is no
 * checking whether buffer contains enough bits -> to be used with caution.
 * BUFFER_GET shows and removes bits. numBits should not be 0 in any of the
 * macros. Note that BUFFER_GET indicates running out of bits kind of late
 * compared to operation of corresponding functions. Additionally, BUFFER_FLUSH
 * does not indicate that situation at all. */
#define BUFFER_INIT(buffer) \
{ \
    (buffer).bits = 32; \
    (buffer).value = Dec_ShowBits32(pDecContainer); \
}

#define BUFFER_SHOW(buffer, outVal, numBits) \
{ \
    if ((buffer).bits < (numBits)) \
    { \
        if(Dec_FlushBits(pDecContainer,32-(buffer).bits) == END_OF_STREAM) \
            return(END_OF_STREAM); \
        (buffer).value = Dec_ShowBits32(pDecContainer); \
        (buffer).bits = 32; \
    } \
    (outVal) = (buffer).value >> (32 - (numBits)); \
}

#define BUFFER_FLUSH(buffer, numBits) \
{ \
    (buffer).value <<= (numBits); \
    (buffer).bits -= (numBits); \
}

#define BUFFER_GET(buffer, outVal, numBits) \
{ \
    if ((buffer).bits < (numBits)) \
    { \
        if(Dec_FlushBits(pDecContainer,32-(buffer).bits) == END_OF_STREAM) \
            return(END_OF_STREAM); \
        (buffer).value = Dec_ShowBits32(pDecContainer); \
        (buffer).bits = 32; \
    } \
    (outVal) = (buffer).value >> (32 - (numBits)); \
    (buffer).value <<= (numBits); \
    (buffer).bits -= (numBits); \
}

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

typedef struct
{
    u32 value;
    u32 bits;
} tmpBuffer_t;

typedef struct
{
    u8 *pStrmBuffStart; /* pointer to start of stream buffer */
    u8 *pStrmCurrPos;   /* current read address in stream buffer */
    u32 bitPosInWord;   /* bit position in stream buffer byte */
    u32 strmBuffSize;   /* size of stream buffer (bytes) */
    u32 strmBuffReadBits;   /* number of bits read from stream buffer */
} strmData_t;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

u32 mpeg2StrmDec_GetBits(DecContainer *, u32 numBits);
u32 mpeg2StrmDec_ShowBits(DecContainer *, u32 numBits);
u32 mpeg2StrmDec_ShowBits32(DecContainer *);
u32 mpeg2StrmDec_ShowBitsAligned(DecContainer *, u32 numBits, u32 numBytes);
u32 mpeg2StrmDec_FlushBits(DecContainer *, u32 numBits);
u32 mpeg2StrmDec_UnFlushBits(DecContainer *, u32 numBits);

u32 mpeg2StrmDec_GetStuffing(DecContainer *);
u32 mpeg2StrmDec_CheckStuffing(DecContainer *);
u32 mpeg2StrmDec_FindSync(DecContainer *);
u32 mpeg2StrmDec_NextStartCode(DecContainer *);

u32 mpeg2StrmDec_NumBits(u32 value);

#endif
