/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "hevc_byte_stream.h"
#include "hevc_util.h"

#define BYTE_STREAM_ERROR 0xFFFFFFFF

/*------------------------------------------------------------------------------

    Extracts one NAL unit from the byte stream buffer.

    Stream buffer is assumed to contain either exactly one NAL unit
    and nothing else, or one or more NAL units embedded in byte
    stream format described in the Annex B of the standard. Function
    detects which one is used based on the first bytes in the buffer.

------------------------------------------------------------------------------*/
u32 HevcExtractNalUnit(const u8 *byte_stream, u32 strm_len,
                            const u8 *strm_buf, u32 buf_len,
                            struct StrmData *stream, u32 *read_bytes,
                            u32 *start_code_detected) {

  /* Variables */

  /* Code */

  ASSERT(byte_stream);
  ASSERT(strm_len);
  ASSERT(strm_len < BYTE_STREAM_ERROR);
  ASSERT(stream);

  /* from strm to buf end */
  stream->strm_buff_start = strm_buf;
  stream->strm_curr_pos = byte_stream;
  stream->bit_pos_in_word = 0;
  stream->strm_buff_read_bits = 0;
  stream->strm_buff_size = buf_len;
  stream->strm_data_size = strm_len;

  stream->remove_emul3_byte = 1;

  /* byte stream format if starts with 0x000001 or 0x000000. Force using
   * byte stream format if start codes found earlier. */
  if (*start_code_detected || SwShowBits(stream, 3) <= 0x01) {
    *start_code_detected = 1;
    DEBUG_PRINT(("BYTE STREAM detected\n"));

    /* search for NAL unit start point, i.e. point after first start code
     * prefix in the stream */
    while (SwShowBits(stream, 24) != 0x01) {
      if (SwFlushBits(stream, 8) == END_OF_STREAM) {
        *read_bytes = strm_len;
        stream->remove_emul3_byte = 0;
        return HANTRO_NOK;
      }
    }
    (void)SwFlushBits(stream, 24);
  }

  /* return number of bytes "consumed" */
  stream->remove_emul3_byte = 0;
  *read_bytes = stream->strm_buff_read_bits / 8;
  return (HANTRO_OK);
}

/* Searches next start code in the stream buffer. */
u32 HevcNextStartCode(struct StrmData *stream) {

  u32 tmp;

  if (stream->bit_pos_in_word) SwGetBits(stream, 8 - stream->bit_pos_in_word);

  stream->remove_emul3_byte = 1;

  while (1) {
    tmp = SwShowBits(stream, 32);
    if (tmp <= 0x01 || (tmp >> 8) == 0x01) {
      stream->remove_emul3_byte = 0;
      return HANTRO_OK;
    }

    if (SwFlushBits(stream, 8) == END_OF_STREAM) {
      stream->remove_emul3_byte = 0;
      return END_OF_STREAM;
    }
  }

  return HANTRO_OK;
}
