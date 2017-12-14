/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Byte stream handling functions. */

#ifndef HEVC_BYTE_STREAM_H_
#define HEVC_BYTE_STREAM_H_

#include "basetype.h"
#include "sw_stream.h"

u32 HevcExtractNalUnit(const u8 *byte_stream, u32 strm_len,
                            const u8 *strm_buf, u32 buf_len,
                            struct StrmData *stream, u32 *read_bytes,
                            u32 *start_code_detected);

u32 HevcNextStartCode(struct StrmData *stream);

#endif /* #ifdef HEVC_BYTE_STREAM_H_ */
