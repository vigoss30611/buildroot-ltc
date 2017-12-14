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

#ifndef __VP9_HEADERS_H__
#define __VP9_HEADERS_H__

#include "basetype.h"

#include "vp9hwd_container.h"
#include "vp9hwd_decoder.h"
#include "vp9hwd_bool.h"

u32 Vp9DecodeFrameTag(const u8 *strm, u32 data_len,
                            const u8 *buf, u32 buf_len,
                           struct Vp9DecContainer *dec_cont);
u32 Vp9SetPartitionOffsets(const u8 *stream, u32 len, struct Vp9Decoder *dec);
u32 Vp9DecodeFrameHeader(const u8 *strm, u32 strm_len, struct VpBoolCoder *bc,
                                const u8 *buffer, u32 buf_len, struct Vp9Decoder *dec);

#endif
