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
-  $RCSfile: vp6scanorder.h,v $
-  $Revision: 1.1 $
-  $Date: 2008/04/14 10:13:25 $
-
------------------------------------------------------------------------------*/

#ifndef __VP6SCANORDER_H__
#define __VP6SCANORDER_H__

#include "basetype.h"
#include "vp6dec.h"

extern const u8 VP6HW_ScanBandUpdateProbs[BLOCK_SIZE];
extern const u8 VP6HW_DefaultScanBands[BLOCK_SIZE];
extern const i8 VP6HWCoeffToBand[65];
extern const i8 VP6HWCoeffToHuffBand[65];
extern const u8 VP6HWtransIndexC[64];

void VP6HWBuildScanOrder(PB_INSTANCE * pbi, u8 * ScanBands);

#endif /* __VP6SCANORDER_H__ */
