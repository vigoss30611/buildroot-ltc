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
--  $RCSfile: avs_utils.h,v $
--  $Date: 2008/07/04 10:39:03 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef STRMDEC_UTILS_H_DEFINED
#define STRMDEC_UTILS_H_DEFINED

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "avs_container.h"

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
#define FIELDPICTURE 0
#define FRAMEPICTURE 1

/* Error concealment */
#define	FREEZED_PIC_RDY 1

/* start codes */
enum
{
    SC_SLICE = 0x00,    /* throug AF */
    SC_SEQUENCE = 0xB0,
    SC_SEQ_END = 0xB1,
    SC_USER_DATA = 0xB2,
    SC_I_PICTURE = 0xB3,
    SC_EXTENSION = 0xB5,
    SC_PB_PICTURE = 0xB6,
    SC_NOT_FOUND = 0xFFFE,
    SC_ERROR = 0xFFFF
};

/* start codes */
enum
{
    SC_SEQ_DISPLAY_EXT = 0x02
};

enum
{
    IFRAME = 1,
    PFRAME = 2,
    BFRAME = 3
};

enum
{
    OUT_OF_BUFFER = 0xFF
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

#ifdef _DEBUG_PRINT
#define AVSDEC_DEBUG(args) printf args
#else
#define AVSDEC_DEBUG(args)
#endif

/* macro to check if stream ends */
#define IS_END_OF_STREAM(pContainer) \
    ( (pContainer)->StrmDesc.strmBuffReadBits == \
      (8*(pContainer)->StrmDesc.strmBuffSize) )

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

u32 AvsStrmDec_GetBits(DecContainer *, u32 numBits);
u32 AvsStrmDec_ShowBits(DecContainer *, u32 numBits);
u32 AvsStrmDec_ShowBits32(DecContainer *);
u32 AvsStrmDec_ShowBitsAligned(DecContainer *, u32 numBits, u32 numBytes);
u32 AvsStrmDec_FlushBits(DecContainer *, u32 numBits);
u32 AvsStrmDec_UnFlushBits(DecContainer *, u32 numBits);

u32 AvsStrmDec_NextStartCode(DecContainer *);

u32 AvsStrmDec_NumBits(u32 value);
u32 AvsStrmDec_CountLeadingZeros(u32 value, u32 len);

#endif
