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
-  $RCSfile: vp6strmbuffer.h,v $
-  $Revision: 1.1 $
-  $Date: 2008/04/14 10:13:25 $
-
------------------------------------------------------------------------------*/

#ifndef __VP6STRMBUFFER_H__
#define __VP6STRMBUFFER_H__

#include "basetype.h"
    typedef struct Vp6StrmBuffer_
{
    const u8 *buffer;
    u32 pos;
    u32 amountLeft;
    u32 bitsInBuffer;
    u32 val;
    u32 bitsConsumed;
} Vp6StrmBuffer;

u32 Vp6StrmInit(Vp6StrmBuffer * sb, const u8 * data, u32 amount);
u32 Vp6StrmGetBits(Vp6StrmBuffer * sb, u32 bits);

#endif /* __VP6STRMBUFFER_H__ */
