/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "hevc_util.h"

static u32 MoreRbspTrailingData(struct StrmData *stream);

u32 HevcRbspTrailingBits(struct StrmData *stream) {

  u32 stuffing;
  u32 stuffing_length;

  ASSERT(stream);
  ASSERT(stream->bit_pos_in_word < 8);

  stuffing_length = 8 - stream->bit_pos_in_word;

  stuffing = SwGetBits(stream, stuffing_length);
  if (stuffing == END_OF_STREAM) return (HANTRO_NOK);

  return (HANTRO_OK);
}

u32 MoreRbspTrailingData(struct StrmData *stream) {

  i32 bits;

  ASSERT(stream);
  ASSERT(stream->strm_buff_read_bits <= 8 * stream->strm_data_size);

  bits = (i32)stream->strm_data_size * 8 - (i32)stream->strm_buff_read_bits;
  if (bits >= 8)
    return (HANTRO_TRUE);
  else
    return (HANTRO_FALSE);
}

u32 HevcMoreRbspData(struct StrmData *stream) {

  u32 bits;

  ASSERT(stream);
  ASSERT(stream->strm_buff_read_bits <= 8 * stream->strm_data_size);

  bits = stream->strm_data_size * 8 - stream->strm_buff_read_bits;

  if (bits == 0) return (HANTRO_FALSE);

  if (bits > 8) {
    if (stream->remove_emul3_byte) return (HANTRO_TRUE);

    bits &= 0x7;
    if (!bits) bits = 8;
    if (SwShowBits(stream, bits) != (1U << (bits - 1)) ||
        (SwShowBits(stream, 23 + bits) << 9))
      return (HANTRO_TRUE);
    else
      return (HANTRO_FALSE);
  } else if (SwShowBits(stream, bits) != (1U << (bits - 1)))
    return (HANTRO_TRUE);
  else
    return (HANTRO_FALSE);
}

u32 HevcCheckCabacZeroWords(struct StrmData *strm_data) {

  u32 tmp;

  ASSERT(strm_data);

  while (MoreRbspTrailingData(strm_data)) {
    tmp = SwGetBits(strm_data, 16);
    if (tmp == END_OF_STREAM)
      return HANTRO_OK;
    else if (tmp)
      return HANTRO_NOK;
  }

  return HANTRO_OK;
}
