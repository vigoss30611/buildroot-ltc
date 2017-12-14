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
--  Description : Utility macros and functions
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_util.h,v $
--  $Revision: 1.2 $
--  $Date: 2007/11/19 13:00:43 $
--
------------------------------------------------------------------------------*/

#ifndef VC1HWD_UTIL_H
#define VC1HWD_UTIL_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"

#ifdef _ASSERT_USED
#include <assert.h>
#endif

#if defined(_DEBUG_PRINT) || defined(_ERROR_PRINT)
#include <stdio.h>
#endif

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define HANTRO_OK       0
#define HANTRO_NOK      1

#define HANTRO_TRUE     1
#define HANTRO_FALSE    0

#ifndef NULL
#define NULL 0
#endif

/* value to be returned by GetBits if stream buffer is empty */
#define END_OF_STREAM 0xFFFFFFFFU

/* macro for assertion, used only if compiler flag _ASSERT_USED is defined */
#ifdef _ASSERT_USED
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr)
#endif

/* macro for debug printing, used only if compiler flag _DEBUG_PRINT is
 * defined */
#ifdef _DEBUG_PRINT
#define DPRINT(args) printf args ; fflush(stdout)
#else
#define DPRINT(args)
#endif

/* macro for error printing, used only if compiler flag _ERROR_PRINT is
 * defined */
#ifdef _ERROR_PRINT
#define EPRINT(msg) fprintf(stderr,"ERROR: %s\n",msg)
#else
#define EPRINT(msg)
#endif

/* macro to get smaller of two values */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* macro to get greater of two values */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* macro to get absolute value */
#define ABS(a) (((a) < 0) ? -(a) : (a))

/* macro to clip a value z, so that x <= z =< y */
#define CLIP3(x,y,z) (((z) < (x)) ? (x) : (((z) > (y)) ? (y) : (z)))

/* macro to clip a value z, so that 0 <= z =< 255 */
#define CLIP1(z) (((z) < 0) ? 0 : (((z) > 255) ? 255 : (z)))

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

#endif /* #ifndef VC1HWD_UTIL_H */

