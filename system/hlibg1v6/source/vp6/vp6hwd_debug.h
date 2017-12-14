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
-
-  Description : ...
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp6hwd_debug.h,v $
-  $Revision: 1.1 $
-  $Date: 2008/04/14 10:13:25 $
-
------------------------------------------------------------------------------*/

#ifndef __VP6HWD_DEBUG_H__
#define __VP6HWD_DEBUG_H__

/* macro for assertion, used only when _ASSERT_USED is defined */
#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

/* macro for debug printing, used only when _DEBUG_PRINT is defined */
#ifdef _DEBUG_PRINT
#include <stdio.h>
#define DEBUG_PRINT(args) printf args
#else
#define DEBUG_PRINT(args)
#endif

/* macro for error printing, used only when _ERROR_PRINT is defined */
#ifdef _ERROR_PRINT
#include <stdio.h>
#define ERROR_PRINT(msg) fprintf(stderr,"ERROR: %s\n",msg)
#else
#define ERROR_PRINT(msg)
#endif

#endif /* __VP6HWD_DEBUG_H__ */
