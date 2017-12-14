/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2007 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : Utility macros for debugging and tracing
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2hwd_debug.h,v $
--  $Date: 2007/12/14 10:59:19 $
--  $Revision: 1.3 $
--
------------------------------------------------------------------------------*/

#ifndef __MPEG2DEBUG_H__
#define __MPEG2DEBUG_H__

#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

#ifdef _MPEG2APITRACE
#include <stdio.h>
#endif

#ifdef _MPEG2_DEBUG_TRACE
#include <stdio.h>
#endif

#ifdef  _DEBUG_PRINT
#include <stdio.h>
#endif

#ifdef _MPEG2APITRACE
#define MPEG2FLUSH fflush(stdout)
#endif
#ifdef _DEBUG_PRINT
#define MPEG2FLUSH fflush(stdout)
#endif
#ifndef MPEG2FLUSH
#define MPEG2FLUSH
#endif
/* macro for debug printing. Note that double parenthesis has to be used, i.e.
 * DEBUG(("Debug printing %d\n",%d)) */
#ifdef _MPEG2APITRACE
#define MPEG2DEC_API_DEBUG(args) printf args
#else
#define MPEG2DEC_API_DEBUG(args)
#endif

#ifdef _DEBUG_PRINT
#define MPEG2DEC_DEBUG(args) printf args
#else
#define MPEG2DEC_DEBUG(args)
#endif

#ifdef _DEC_PP_USAGE
#define DECPP_STAND_ALONE 0
#define DECPP_PARALLEL 1
#define DECPP_PIPELINED 2
#define DECPP_UNSPECIFIED 3

void Mpeg2DecPpUsagePrint(DecContainer * pDecCont,
                          u32 ppmode, u32 picIndex, u32 decStatus, u32 picId);
#endif

#endif /* #ifndef __MPEG2DEBUG_H__ */
