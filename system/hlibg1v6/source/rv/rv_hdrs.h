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
--  Abstract : 
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rv_hdrs.h,v $
--  $Date: 2009/03/11 14:00:12 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef RV_HDRS_H
#define RV_HDRS_H

#include "basetype.h"

typedef struct DecTimeCode_t
{
    u32 dropFlag;
    u32 hours;
    u32 minutes;
    u32 seconds;
    u32 picture;
} DecTimeCode;

typedef struct
{
    u32 horizontalSize;
    u32 verticalSize;

    u32 temporalReference;
    u32 pictureCodingType;

} DecHdrs;

#endif /* #ifndef RV_HDRS_H */
