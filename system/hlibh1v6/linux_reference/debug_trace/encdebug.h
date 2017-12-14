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
------------------------------------------------------------------------------*/
#ifndef __ENCDEBUG_H__
#define __ENCDEBUG_H__

/* macro for assertion, used only when _ASSERT_USED is defined */
#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr)                assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

/* macro for debug printing, used only when _DEBUG_PRINT is defined */
#ifdef _DEBUG_PRINT
#include <stdio.h>
#define DEBUG_PRINT(args)           printf args
#else
#define DEBUG_PRINT(args)
#endif

#ifdef TRACE_STREAM
#include "enctracestream.h"
#define COMMENT(x)                  EncComment(x)
#define COMMENTMBTYPE(x,y)          EncCommentMbType(x, y)
#define TRACE_BIT_STREAM(v,n)       EncTraceStream(v, n)
#else
#define COMMENT(x)
#define COMMENTMBTYPE(x,y)
#define TRACE_BIT_STREAM(v,n)
#endif

#endif /* __ENCDEBUG_H__ */
