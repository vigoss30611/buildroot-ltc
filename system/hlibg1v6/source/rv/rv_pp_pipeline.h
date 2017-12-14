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
--  Abstract : RV decoder and PP pipeline support
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rv_pp_pipeline.h,v $
--  $Date: 2009/03/11 14:00:12 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef RV_PP_PIPELINE_H
#define RV_PP_PIPELINE_H

#include "decppif.h"

i32 rvRegisterPP(const void *decInst, const void *ppInst,
                    void (*PPDecSetup) (const void *, const DecPpInterface *),
                    void (*PPDecPipelineEndCallback) (const void *),
                    void (*PPConfigQuery) (const void *, DecPpQuery *),
		    void (*PPDisplayIndex)(const void *, u32),
		    void (*PPBufferData) (const void *, u32, u32, u32));

i32 rvUnregisterPP(const void *decInst, const void *ppInst);

#endif /* #ifdef RV_PP_PIPELINE_H */
