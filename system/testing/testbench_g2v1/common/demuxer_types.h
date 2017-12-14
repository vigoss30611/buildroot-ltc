/* Copyright 2013 Google Inc. All Rights Reserved. */

/* Common types for G2 decoder demuxers. */

#ifndef __DEMUXER_TYPES_H__
#define __DEMUXER_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "software/source/inc/basetype.h"

/* Enumeration for the bitstream format outputted from the reader */
#define BITSTREAM_VP7 0x01
#define BITSTREAM_VP8 0x02
#define BITSTREAM_VP9 0x03
#define BITSTREAM_WEBP 0x04
#define BITSTREAM_H264 0x05
#define BITSTREAM_HEVC 0x06

/* Generic demuxer interface. */
typedef const void* DemuxerOpenFunc(const char* fname, u32 mode);
typedef int DemuxerIdentifyFormatFunc(const void* inst);
typedef int DemuxerReadPacketFunc(const void* inst, u8* buffer, i32* size);
typedef void DemuxerCloseFunc(const void* inst);
typedef struct Demuxer_ {
  const void* inst;
  DemuxerOpenFunc* open;
  DemuxerIdentifyFormatFunc* GetVideoFormat;
  DemuxerReadPacketFunc* ReadPacket;
  DemuxerCloseFunc* close;
} Demuxer;

#ifdef __cplusplus
}
#endif

#endif /* __DEMUXER_TYPES_H__ */
