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
--  $RCSfile: rv_utils.h,v $
--  $Date: 2009/03/11 14:00:12 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef RV_UTILS_H
#define RV_UTILS_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "rv_container.h"

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

/* Error concealment */
#define	FREEZED_PIC_RDY 1

enum
{
    RV_I_PIC = 0,
    RV_FI_PIC = 1,
    RV_P_PIC = 2,
    RV_B_PIC = 3
};

enum
{
    RV_SLICE
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

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

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

u32 rv_GetBits(DecContainer *, u32 numBits);
u32 rv_ShowBits(DecContainer *, u32 numBits);
u32 rv_ShowBits32(DecContainer *);
u32 rv_FlushBits(DecContainer *, u32 numBits);

u32 rv_CheckStuffing(DecContainer *);

u32 rv_NumBits(u32 value);

#endif /* RV_UTILS_H */
