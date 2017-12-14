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
--  Abstract : Header file for short video (h263) decoding functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_shortvideo.h,v $
--  $Date: 2007/11/26 08:35:32 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/
#ifndef STRMDEC_SHORTVIDEO_H_DEFINED
#define STRMDEC_SHORTVIDEO_H_DEFINED

#include "mp4dechwd_container.h"

u32 StrmDec_DecodeShortVideoHeader(DecContainer * pDecContainer);
u32 StrmDec_DecodeSorensonHeader(DecContainer * pDecContainer);
u32 StrmDec_DecodeGobLayer(DecContainer *);
u32 StrmDec_DecodeShortVideo(DecContainer *);
u32 StrmDec_CheckNextGobNumber(DecContainer *);
u32 StrmDec_DecodeSorensonSparkHeader(DecContainer * pDecContainer);

#endif
