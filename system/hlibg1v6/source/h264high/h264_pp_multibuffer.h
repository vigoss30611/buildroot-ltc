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
-  Description : Dec/PP multibuiffer handling
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: h264_pp_multibuffer.h,v $
-  $Revision: 1.2 $
-  $Date: 2010/07/23 09:47:30 $
-
------------------------------------------------------------------------------*/

#ifndef H264_PP_MULTIBUFFER_H
#define H264_PP_MULTIBUFFER_H

#include "basetype.h"
#include "dwl.h"
#include "h264hwd_container.h"

void h264PpMultiInit(decContainer_t * pDecCont, u32 maxBuff);
u32 h264PpMultiAddPic(decContainer_t * pDecCont, const DWLLinearMem_t * data);
u32 h264PpMultiRemovePic(decContainer_t * pDecCont,
                         const DWLLinearMem_t * data);
u32 h264PpMultiFindPic(decContainer_t * pDecCont, const DWLLinearMem_t * data);

void h264PpMultiMvc(decContainer_t * pDecCont, u32 maxBuffId);

#endif /* H264_PP_MULTIBUFFER_H */
