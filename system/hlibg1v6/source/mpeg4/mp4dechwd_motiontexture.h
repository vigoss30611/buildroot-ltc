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
--  Abstract : Header file for motion-texture decoding functionality
--
------------------------------------------------------------------------------*/
#ifndef STRMDEC_MOTIONTEXTURE_H_DEFINED
#define STRMDEC_MOTIONTEXTURE_H_DEFINED

#include "mp4dechwd_container.h"

u32 StrmDec_DecodeMotionTexture(DecContainer * pDecContainer);
u32 StrmDec_DecodeMb(DecContainer * pDecContainer, u32 mbNumber);

#endif
