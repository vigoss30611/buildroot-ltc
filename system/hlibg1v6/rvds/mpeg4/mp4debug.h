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
--  Abstract : Utility macros for debugging and tracing
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4debug.h,v $
--  $Date: 2007/09/17 13:12:36 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

#ifndef __MP4DEBUG_H__
#define __MP4DEBUG_H__

#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

#ifdef _MPEG4APITRACE
#include <stdio.h>
#endif

#ifdef _MPEG4_DEBUG_TRACE
#include <stdio.h>
#endif

#ifdef  _DEBUG_PRINT
#include <stdio.h>
#endif

#ifdef _MPEG4APITRACE
#define MP4FLUSH fflush(stdout)
#endif
#ifdef _DEBUG_PRINT
#define MP4FLUSH fflush(stdout)
#endif
#ifndef MP4FLUSH
#define MP4FLUSH
#endif
/* macro for debug printing. Note that double parenthesis has to be used, i.e.
 * DEBUG(("Debug printing %d\n",%d)) */
#ifdef _MPEG4APITRACE
#define MP4DEC_API_DEBUG(args) printf args
#else
#define MP4DEC_API_DEBUG(args)
#endif

#ifdef _DEBUG_PRINT
#define MP4DEC_DEBUG(args) printf args
#else
#define MP4DEC_DEBUG(args)
#endif

#ifdef _DEC_PP_USAGE
#define DECPP_STAND_ALONE 0
#define DECPP_PARALLEL 1
#define DECPP_PIPELINED 2
#define DECPP_UNSPECIFIED 3

void MP4DecPpUsagePrint(DecContainer *pDecCont,
                      u32 ppmode,
                      u32 picIndex,
                      u32 decStatus,
                      u32 picId);
#endif

#endif
