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
--  Abstract : Error concealment
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_conceal.h,v $
--  $Date: 2008/03/13 12:48:06 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/
#ifndef __H264HWD_CONCEAL_H__
#define __H264HWD_CONCEAL_H__

#include "basetype.h"
#include "h264hwd_slice_header.h"
#include "h264hwd_storage.h"
#include "h264hwd_asic.h"


void h264bsdConceal(storage_t *pStorage, DecAsicBuffers_t * pAsicBuff,
                   u32 sliceType);

#endif /* __H264HWD_CONCEAL_H__ */
