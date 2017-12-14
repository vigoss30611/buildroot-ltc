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
-  $RCSfile: vp8hwd_asic.h,v $
-  $Revision: 1.4 $
-  $Date: 2010/10/20 07:29:05 $
-
------------------------------------------------------------------------------*/

#ifndef __VP8HWD_ASIC_H__
#define __VP8HWD_ASIC_H__

#include "basetype.h"
#include "vp8hwd_container.h"
#include "regdrv.h"

#define DEC_8190_ALIGN_MASK         0x07U

#define VP8HWDEC_HW_RESERVED         0x0100
#define VP8HWDEC_SYSTEM_ERROR        0x0200
#define VP8HWDEC_SYSTEM_TIMEOUT      0x0300

void VP8HwdAsicInit(VP8DecContainer_t * pDecCont);
i32 VP8HwdAsicAllocateMem(VP8DecContainer_t * pDecCont);
void VP8HwdAsicReleaseMem(VP8DecContainer_t * pDecCont);
i32 VP8HwdAsicAllocatePictures(VP8DecContainer_t * pDecCont);
void VP8HwdAsicReleasePictures(VP8DecContainer_t * pDecCont);

void VP8HwdAsicInitPicture(VP8DecContainer_t * pDecCont);
void VP8HwdAsicStrmPosUpdate(VP8DecContainer_t * pDecCont, u32 busAddress);
u32 VP8HwdAsicRun(VP8DecContainer_t * pDecCont);

void VP8HwdAsicProbUpdate(VP8DecContainer_t * pDecCont);

void VP8HwdUpdateRefs(VP8DecContainer_t * pDecCont, u32 corrupted);

void VP8HwdAsicContPicture(VP8DecContainer_t * pDecCont);

#endif /* __VP8HWD_ASIC_H__ */
