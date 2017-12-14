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
--  Abstract : MPEG2 decoder and PP pipeline support
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2hwd_pp_pipeline.h,v $
--  $Date: 2008/11/04 13:21:29 $
--  $Revision: 1.4 $
--
------------------------------------------------------------------------------*/

#ifndef MPEG2_PP_PIPELINE_H
#define MPEG2_PP_PIPELINE_H

#include "decppif.h"

i32 mpeg2RegisterPP(const void *decInst, const void *ppInst,
                    void (*PPDecSetup) (const void *, const DecPpInterface *),
                    void (*PPDecPipelineEndCallback) (const void *),
                    void (*PPConfigQuery) (const void *, DecPpQuery *),
		    void (*PPDisplayIndex)(const void *, u32),
		    void (*PPBufferData) (const void *, u32, u32, u32));

i32 mpeg2UnregisterPP(const void *decInst, const void *ppInst);

#endif /* #ifdef MPEG2_PP_PIPELINE_H */
