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
-  $RCSfile: vp6strmbuffer.c,v $
-  $Revision: 1.1 $
-  $Date: 2008/04/14 10:13:25 $
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp6strmbuffer.h"

/*------------------------------------------------------------------------------
    Function name   : Vp6StrmInit
    Description     : 
    Return type     : u32 
    Argument        : Vp6StrmBuffer * sb
    Argument        : const u8 * data
    Argument        : u32 amount
------------------------------------------------------------------------------*/
u32 Vp6StrmInit(Vp6StrmBuffer * sb, const u8 * data, u32 amount)
{
    sb->buffer = data;
    sb->pos = 4;
    sb->bitsInBuffer = 32;
    sb->val = (u32) data[0] << 24 | (u32) data[1] << 16 | (u32) data[2] << 8
        | (u32) data[3];
    sb->amountLeft = amount - 4;
    sb->bitsConsumed = 0;
    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : Vp6StrmGetBits
    Description     : 
    Return type     : u32 
    Argument        : Vp6StrmBuffer * sb
    Argument        : u32 bits
------------------------------------------------------------------------------*/
u32 Vp6StrmGetBits(Vp6StrmBuffer * sb, u32 bits)
{
    u32 retVal;
    const u8 *strm = &sb->buffer[sb->pos];

    sb->bitsConsumed += bits;

    if(bits < sb->bitsInBuffer)
    {
        retVal = sb->val >> (32 - bits);
    }
    else
    {
        retVal = sb->val >> (32 - bits);
        bits -= sb->bitsInBuffer;
        sb->val =
            ((u32) strm[0]) << 24 | ((u32) strm[1]) << 16 | ((u32) strm[2]) << 8
            | ((u32) strm[3]);
        sb->pos += 4;
        sb->bitsInBuffer = 32;
        if(bits)
        {
            u32 tmp = sb->val >> (32 - bits);

            retVal <<= bits;
            retVal |= tmp;
        }
    }
    sb->val <<= bits;
    sb->bitsInBuffer -= bits;

    return retVal;
}
