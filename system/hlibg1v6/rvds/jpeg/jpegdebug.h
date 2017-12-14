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
--  $RCSfile: jpegdebug.h,v $
--  $Date: 2007/03/30 05:45:21 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef __JPEGDEBUG_H__
#define __JPEGDEBUG_H__

#ifdef _ASSERT_USED
    #ifndef ASSERT
    #include <assert.h>
    #define ASSERT(expr) assert(expr)
    #endif
#else
    #define ASSERT(expr)
#endif

#ifdef _JPEGDECAPITRACE
    #include <stdio.h>
#elif _JPEGDEC_TRACE
    #include <stdio.h>
#elif _JPEGDEC_API_TRC
    #include <stdio.h>
#elif _JPEGDEC_PP_TRACE
    #include <stdio.h>
#elif _DEBUG
    #include <stdio.h>
#elif _DEBUG
    #include <stdio.h>
#endif

/* macro for debug printing. Note that double parenthesis has to be used, i.e.
 * DEBUG(("Debug printing %d\n",%d)) */
#ifdef _JPEGAPITRACE
#define JPEG_API_TRC(args) printf args
#else
#define JPEG_API_TRC(args)
#endif

#ifdef _DEBUG
#define DEBUG(args) printf args
#else
#define DEBUG(args)
#endif


#endif
