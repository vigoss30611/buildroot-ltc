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
-  $RCSfile: vp6hwd_asic.h,v $
-  $Revision: 1.2 $
-  $Date: 2008/04/16 14:13:28 $
-
------------------------------------------------------------------------------*/

#ifndef __VP6HWD_ASIC_H__
#define __VP6HWD_ASIC_H__

#include "basetype.h"
#include "vp6hwd_container.h"
#include "regdrv.h"

#define DEC_8190_ALIGN_MASK         0x07U
#define DEC_8190_MODE_VP6           0x07U

#define VP6HWDEC_HW_RESERVED         0x0100
#define VP6HWDEC_SYSTEM_ERROR        0x0200
#define VP6HWDEC_SYSTEM_TIMEOUT      0x0300

void VP6HwdAsicInit(VP6DecContainer_t * pDecCont);
i32 VP6HwdAsicAllocateMem(VP6DecContainer_t * pDecCont);
void VP6HwdAsicReleaseMem(VP6DecContainer_t * pDecCont);
i32 VP6HwdAsicAllocatePictures(VP6DecContainer_t * pDecCont);
void VP6HwdAsicReleasePictures(VP6DecContainer_t * pDecCont);

void VP6HwdAsicInitPicture(VP6DecContainer_t * pDecCont);
void VP6HwdAsicStrmPosUpdate(VP6DecContainer_t * pDecCont);
u32 VP6HwdAsicRun(VP6DecContainer_t * pDecCont);

void VP6HwdAsicProbUpdate(VP6DecContainer_t * pDecCont);

#endif /* __VP6HWD_ASIC_H__ */
