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
--  $RCSfile: vp6_pp_pipeline.h,v $
--  $Date: 2008/05/15 18:56:40 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef VP6_PP_PIPELINE_H
#define VP6_PP_PIPELINE_H

#include "decppif.h"

i32 vp6RegisterPP(const void *decInst, const void *ppInst,
                   void (*PPDecStart) (const void *, const DecPpInterface *),
                   void (*PPDecWaitEnd) (const void *),
                   void (*PPConfigQuery) (const void *, DecPpQuery *));

i32 vp6UnregisterPP(const void *decInst, const void *ppInst);

#endif /* #ifdef VP6_PP_PIPELINE_H */
