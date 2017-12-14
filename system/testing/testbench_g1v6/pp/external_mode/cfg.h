/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract :
------------------------------------------------------------------------------*/
#ifndef __CFG_H_
#define __CFG_H_

#include "defs.h"

/*------------------------------------------------------------------------------
    Parse mode constants
------------------------------------------------------------------------------*/
typedef enum {
    CFG_OK,
    CFG_ERROR           = 500,
    CFG_INVALID_BLOCK   = 501,
    CFG_INVALID_PARAM   = 502,
    CFG_INVALID_VALUE   = 503,
    CFG_INVALID_CODE    = 504,
    CFG_DUPLICATE_BLOCK = 505
} CfgCallbackResult;

/*------------------------------------------------------------------------------
    Parse mode constants
------------------------------------------------------------------------------*/
typedef enum {
    CFG_CALLBACK_BLK_START  = 300,
    CFG_CALLBACK_VALUE      = 301,
} CfgCallbackParam;

/*------------------------------------------------------------------------------
    Interface to callback function
------------------------------------------------------------------------------*/
typedef CfgCallbackResult (*CfgCallback)(char*, char*, char*, CfgCallbackParam,
    void** );

u32 ParseConfig( char * filename, CfgCallback callback, void **cbParam );
CfgCallbackResult ReadParam( char * block, char * key, char * value,
    CfgCallbackParam state, void **cbParam );

#endif /* __CFG_H_ */
