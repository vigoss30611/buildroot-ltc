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
-  $RCSfile: vp6decodemv.h,v $
-  $Revision: 1.1 $
-  $Date: 2008/04/14 10:13:24 $
-
------------------------------------------------------------------------------*/

#ifndef __VP6DECODEMV_H__
#define __VP6DECODEMV_H__

#include "basetype.h"
#include "vp6dec.h"

extern const u8 VP6HW_DefaultMvShortProbs[2][7];
extern const u8 VP6HW_DefaultMvLongProbs[2][LONG_MV_BITS];
extern const u8 VP6HW_DefaultIsShortProbs[2];
extern const u8 VP6HW_DefaultSignProbs[2];

extern const u8 VP6HWMvUpdateProbs[2][MV_NODES];

void VP6HWConfigureMvEntropyDecoder(PB_INSTANCE * pbi, u8 FrameType);

#endif /* __VP6DECODEMV_H__ */
