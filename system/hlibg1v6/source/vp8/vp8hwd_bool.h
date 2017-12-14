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
-  $RCSfile: vp8hwd_bool.h,v $
-  $Revision: 1.1 $
-  $Date: 2009/11/17 13:35:15 $
-
------------------------------------------------------------------------------*/

#ifndef __VP8_BOOL_H__
#define __VP8_BOOL_H__

#include "basetype.h"

#define END_OF_STREAM   (0xFFFFFFFF)
#define CHECK_END_OF_STREAM(s) if((s)==END_OF_STREAM) return (s)

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
} vpBoolCoder_t;

extern void vp8hwdBoolStart(vpBoolCoder_t * bc, const u8 *buffer, u32 len);
extern u32 vp8hwdDecodeBool(vpBoolCoder_t * bc, i32 probability);
extern u32 vp8hwdDecodeBool128(vpBoolCoder_t * bc);
extern void vp8hwdBoolStop(vpBoolCoder_t * bc);

u32 vp8hwdReadBits ( vpBoolCoder_t *br, i32 bits );

#endif /* __VP8_BOOL_H__ */
