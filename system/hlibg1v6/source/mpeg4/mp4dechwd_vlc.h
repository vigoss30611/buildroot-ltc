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
--  $RCSfile: mp4dechwd_vlc.h,v $
--  $Date: 2007/11/26 10:06:47 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. xxx...


------------------------------------------------------------------------------*/

#ifndef STRMDEC_VLC_H
#define STRMDEC_VLC_H

#include "basetype.h"
#include "mp4dechwd_container.h"

/* constant definitions */

/* function prototypes */

u32 StrmDec_DecodeMcbpc(DecContainer *, u32, u32, u32 *);
u32 StrmDec_DecodeCbpy(DecContainer *, u32, u32, u32 *);
u32 mp4dechwd_vlcDec(DecContainer * pDecContainer, u32 mbNum, u32 blockNum);
u32 StrmDec_DecodeDcCoeff(DecContainer * pDecContainer, u32 mbNum, u32 blockNum,
              i32 * dcCoeff);
u32 StrmDec_DecodeMv(DecContainer * pDecContainer, u32 mbNum);
u32 StrmDec_DecodeVlcBlock(DecContainer * pDecContainer, u32 mbNum,
    u32 blockNum);


#endif
