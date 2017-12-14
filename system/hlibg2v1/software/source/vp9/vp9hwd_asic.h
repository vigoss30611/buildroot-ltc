/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
-
-  Description : ...
-
------------------------------------------------------------------------------*/

#ifndef __VP9_ASIC_H__
#define __VP9_ASIC_H__

#include "basetype.h"
#include "vp9hwd_container.h"
#include "regdrv.h"

#define DEC_8190_ALIGN_MASK 0x07U

#define VP9HWDEC_HW_RESERVED 0x0100
#define VP9HWDEC_SYSTEM_ERROR 0x0200
#define VP9HWDEC_SYSTEM_TIMEOUT 0x0300

void Vp9AsicInit(struct Vp9DecContainer *dec_cont);
i32 Vp9AsicAllocateMem(struct Vp9DecContainer *dec_cont);
#ifdef USE_EXTERNAL_BUFFER
i32 Vp9AsicReleaseMem(struct Vp9DecContainer *dec_cont);
#else
void Vp9AsicReleaseMem(struct Vp9DecContainer *dec_cont);
#endif
i32 Vp9AsicAllocateFilterBlockMem(struct Vp9DecContainer *dec_cont);
#ifdef USE_EXTERNAL_BUFFER
i32 Vp9AsicReleaseFilterBlockMem(struct Vp9DecContainer *dec_cont);
#else
void Vp9AsicReleaseFilterBlockMem(struct Vp9DecContainer *dec_cont);
#endif
i32 Vp9AsicAllocatePictures(struct Vp9DecContainer *dec_cont);
void Vp9AsicReleasePictures(struct Vp9DecContainer *dec_cont);

i32 Vp9AllocateFrame(struct Vp9DecContainer *dec_cont, u32 index);
i32 Vp9ReallocateFrame(struct Vp9DecContainer *dec_cont, u32 index);
void Vp9AsicInitPicture(struct Vp9DecContainer *dec_cont);
void Vp9AsicStrmPosUpdate(struct Vp9DecContainer *dec_cont, addr_t bus_address,
                          u32 data_len, addr_t buf_address, u32 buf_len);
u32 Vp9AsicRun(struct Vp9DecContainer *dec_cont);
u32 Vp9AsicSync(struct Vp9DecContainer *dec_cont);

void Vp9AsicProbUpdate(struct Vp9DecContainer *dec_cont);

void Vp9UpdateRefs(struct Vp9DecContainer *dec_cont, u32 corrupted);

i32 Vp9GetRefFrm(struct Vp9DecContainer *dec_cont);
void Vp9UpdateProbabilities(struct Vp9DecContainer *dec_cont);

#endif /* __VP9_ASIC_H__ */
