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
--  $RCSfile: rv_debug.h,v $
--  $Date: 2009/03/11 14:01:07 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef RV_DEBUG_H
#define RV_DEBUG_H

#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

#ifdef _RVAPITRACE
#include <stdio.h>
#endif

#ifdef _RV_DEBUG_TRACE
#include <stdio.h>
#endif

#ifdef  _DEBUG_PRINT
#include <stdio.h>
#endif

#ifdef _RVAPITRACE
#define RVFLUSH fflush(stdout)
#endif
#ifdef _DEBUG_PRINT
#define RVFLUSH fflush(stdout)
#endif
#ifndef RVFLUSH
#define RVFLUSH
#endif
/* macro for debug printing. Note that double parenthesis has to be used, i.e.
 * DEBUG(("Debug printing %d\n",%d)) */
#ifdef _RVAPITRACE
#define RVDEC_API_DEBUG(args) printf args
#else
#define RVDEC_API_DEBUG(args)
#endif

#ifdef _DEBUG_PRINT
#define RVDEC_DEBUG(args) printf args
#else
#define RVDEC_DEBUG(args)
#endif

#ifdef _DEC_PP_USAGE
#define DECPP_STAND_ALONE 0
#define DECPP_PARALLEL 1
#define DECPP_PIPELINED 2
#define DECPP_UNSPECIFIED 3

void RvDecPpUsagePrint(DecContainer * pDecCont,
                          u32 ppmode, u32 picIndex, u32 decStatus, u32 picId);
#endif

#endif /* #ifndef RV_DEBUG_H */
