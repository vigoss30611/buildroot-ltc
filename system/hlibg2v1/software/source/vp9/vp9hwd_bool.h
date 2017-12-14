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

#ifndef __VP9_BOOL_H__
#define __VP9_BOOL_H__

#include "basetype.h"

#if 0
#include <stdio.h>
#define STREAM_TRACE(x, y) printf("%-30s-%9d\n", x, y);
#define VP9DEC_DEBUG(x) printf x
#else
#define STREAM_TRACE(x, y)
#define VP9DEC_DEBUG(x)
#endif

#define CHECK_END_OF_STREAM(s) \
  if ((s) == END_OF_STREAM) return (s)

struct VpBoolCoder {
  u32 lowvalue;
  u32 range;
  u32 value;
  i32 count;
  u32 read_len;
  const u8 *buffer;
  const u8 *buffer_start;
  u32 BitCounter;
  u32 buffer_len;
  u32 stream_len;
  u32 strm_error;
};

extern void Vp9BoolStart(struct VpBoolCoder *bc, const u8* stream,
                    u32 strm_len, const u8* strm_buf, u32 buf_len);
extern u32 Vp9DecodeBool(struct VpBoolCoder *bc, i32 probability);
extern u32 Vp9DecodeBool128(struct VpBoolCoder *bc);
extern void Vp9BoolStop(struct VpBoolCoder *bc);

u32 Vp9DecodeSubExp(struct VpBoolCoder *bc, u32 k, u32 num_syms);
u32 Vp9ReadBits(struct VpBoolCoder *br, i32 bits);

#endif /* __VP9_BOOL_H__ */
