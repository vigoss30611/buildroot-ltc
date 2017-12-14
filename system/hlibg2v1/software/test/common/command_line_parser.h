/* Copyright 2013 Google Inc. All Rights Reserved. */

/* Command line parser module. */

#ifndef __COMMAND_LINE_PARSER_H__
#define __COMMAND_LINE_PARSER_H__

#include "software/source/inc/basetype.h"
#include "software/source/inc/decapicommon.h"
#include "software/test/common/error_simulator.h"
#include "software/source/inc/dectypes.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
  VP8DEC_DECODER_ALLOC = 0,
  VP8DEC_EXTERNAL_ALLOC = 1,
  VP8DEC_EXTERNAL_ALLOC_ALT = 2
};

enum FileFormat {
  FILEFORMAT_AUTO_DETECT = 0,
  FILEFORMAT_BYTESTREAM,
  FILEFORMAT_IVF,
  FILEFORMAT_WEBM
};

enum StreamReadMode {
  STREAMREADMODE_FRAME = 0,
  STREAMREADMODE_NALUNIT = 1,
  STREAMREADMODE_FULLSTREAM = 2,
  STREAMREADMODE_PACKETIZE = 3
};

enum SinkType {
  SINK_FILE_SEQUENCE = 0,
  SINK_FILE_PICTURE,
  SINK_MD5_SEQUENCE,
  SINK_MD5_PICTURE,
  SINK_SDL,
  SINK_NULL
};

struct TestParams {
  char* in_file_name;
  char* out_file_name;
  u32 num_of_decoded_pics;
  enum DecPictureFormat format;
  enum DecPictureFormat hw_format;
  enum SinkType sink_type;
  u8 display_cropped;
  u8 hw_traces;
  u8 trace_target;
  u8 extra_output_thread;
  u8 disable_display_order;
  enum FileFormat file_format;
  enum StreamReadMode read_mode;
  struct ErrorSimulationParams error_sim;
  enum DecErrorConcealment concealment_mode;
#ifdef DOWN_SCALER
  struct DecDownscaleCfg dscale_cfg;
#endif
  u8 compress_bypass;   /* compressor bypass flag */
  u8 is_ringbuffer;     /* ringbuffer mode by default */
  u8 fetch_one_pic;     /* prefetch one pic_id together */
  u32 force_output_8_bits;  /* Output 8 bits per pixel. */
  u32 p010_output;          /* Output in MS P010 format. */
  u32 bigendian_output;            /* Output big endian format. */
};

void PrintUsage(char* executable);
void SetupDefaultParams(struct TestParams* params);
int ParseParams(int argc, char* argv[], struct TestParams* params);
int ResolveOverlap(struct TestParams* params);

#ifdef __cplusplus
}
#endif

#endif /* __COMMAND_LINE_PARSER_H__ */
