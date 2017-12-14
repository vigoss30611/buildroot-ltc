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
-  $RCSfile: vp6booldec.h,v $
-  $Revision: 1.2 $
-  $Date: 2009/09/03 08:24:48 $
-
------------------------------------------------------------------------------*/

#ifndef __VP6BOOLDEC_H__
#define __VP6BOOLDEC_H__

#include "basetype.h"

typedef struct
{
    u32 lowvalue;
    u32 range;
    u32 value;
    i32 count;
    u32 pos;
    u8 *buffer;
    u32 BitCounter;
    u32 streamEndPos;
    u32 strmError;
} BOOL_CODER;

extern void VP6HWStartDecode(BOOL_CODER * bc, u8 *buffer, u32 len);
extern u32 VP6HWDecodeBool(BOOL_CODER * bc, i32 probability);
extern u32 VP6HWDecodeBool128(BOOL_CODER * bc);
extern void VP6HWStopDecode(BOOL_CODER * bc);

u32 VP6HWbitread ( BOOL_CODER *br, i32 bits );

#endif /* __VP6BOOLDEC_H__ */
