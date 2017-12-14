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
--  Abstract : VP6 decoder and PP pipeline support
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vp8hwd_pp_pipeline.h,v $
--  $Date: 2009/12/17 12:29:24 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

#ifndef VP8_PP_PIPELINE_H
#define VP8_PP_PIPELINE_H

#include "decppif.h"

i32 vp8RegisterPP(const void *decInst, const void *ppInst,
                   void (*PPDecStart) (const void *, const DecPpInterface *),
                   void (*PPDecWaitEnd) (const void *),
                   void (*PPConfigQuery) (const void *, DecPpQuery *));

i32 vp8UnregisterPP(const void *decInst, const void *ppInst);

#endif /* #ifdef VP8_PP_PIPELINE_H */
