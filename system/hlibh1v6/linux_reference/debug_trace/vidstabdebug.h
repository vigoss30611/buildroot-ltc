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
--  Abstract : Video stabilization. Utility macros for debugging and tracing 
--
------------------------------------------------------------------------------*/
#ifndef __VIDSTABDEBUG_H__
#define __VIDSTABDEBUG_H__

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

#endif /* __VIDSTABDEBUG_H__ */
