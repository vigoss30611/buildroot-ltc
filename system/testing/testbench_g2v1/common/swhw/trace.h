/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Common tracing functionalities for the 8170 system model. */

#ifndef TRACE_H
#define TRACE_H

#include <stdio.h>
#include <string.h>
#include "basetype.h"

/* common tools */
enum TraceDecodingMode {
  TRACE_H263 = 0,
  TRACE_H264 = 1,
  TRACE_MPEG1 = 2,
  TRACE_MPEG2 = 3,
  TRACE_MPEG4 = 4,
  TRACE_VC1 = 5,
  TRACE_JPEG = 6,
  TRACE_RV = 8
};

struct TracePicCodingType {
  u8 i_coded; /* intra coded */
  u8 p_coded; /* predictive coded */
  u8 b_coded; /* bidirectionally predictive coded */
};

struct TraceSequenceType {
  u8 interlaced;
  u8 progressive;
};

/* mpeg-1 & mpeg-2 tools */
struct TraceMpeg2DecTools {
  enum TraceDecodingMode decoding_mode; /* MPEG-1 or MPEG-2 */
  struct TracePicCodingType pic_coding_type;
  u8 d_coded; /* MPEG-1 specific */
  struct TraceSequenceType sequence_type;
};

/* h.264 tools */
struct TraceH264StreamMode {
  u8 nal_unit_strm;
  u8 byte_strm;
};

struct TraceH264EntropyCoding {
  u8 cavlc;
  u8 cabac;
};

struct TraceH264ProfileType {
  u8 baseline;
  u8 main;
  u8 high;
};

struct TraceH264ScalingMatrixPresentType {
  u8 pic;
  u8 seq;
};

struct TraceH264WeightedPredictionType {
  u8 explicit;
  u8 explicit_b;
  u8 implicit;
};

struct TraceH264InterlaceType {
  u8 pic_aff;
  u8 mb_aff;
};

struct TraceH264SequenceType {
  u8 ilaced;
  u8 prog;
};

struct TraceH264DecTools {
  enum TraceDecodingMode decoding_mode;
  struct TracePicCodingType pic_coding_type;
  struct TraceH264StreamMode stream_mode;
  struct TraceH264EntropyCoding entropy_coding;
  struct TraceH264ProfileType profile_type;
  struct TraceH264ScalingMatrixPresentType scaling_matrix_present_type;
  struct TraceH264WeightedPredictionType weighted_prediction_type;
  struct TraceH264InterlaceType interlace_type;
  struct TraceH264SequenceType seq_type;
  u8 slice_groups; /* more thant 1 slice groups */
  u8 arbitrary_slice_order;
  u8 redundant_slices;
  u8 weighted_prediction;
  u8 image_cropping;
  u8 monochrome;
  u8 scaling_list_present;
  u8 transform8x8;
  u8 loop_filter;
  u8 loop_filter_dis;
  u8 error;
  u8 intra_prediction8x8;
  u8 ipcm;
  u8 direct_mode;
  u8 multiple_slices_per_picture;
};

/* jpeg tools */
struct TraceJpegDecTools {
  enum TraceDecodingMode decoding_mode;
  u8 sampling_4_2_0;
  u8 sampling_4_2_2;
  u8 sampling_4_0_0;
  u8 sampling_4_4_0;
  u8 sampling_4_1_1;
  u8 sampling_4_4_4;
  u8 thumbnail;
  u8 progressive;
};

/* mpeg-4 & h.263 tools */
struct TraceMpeg4DecTools {
  enum TraceDecodingMode decoding_mode;
  struct TracePicCodingType pic_coding_type;
  struct TraceSequenceType sequence_type;
  u32 four_mv;
  u32 ac_pred;
  u32 data_partition;
  u32 resync_marker;
  u32 reversible_vlc;
  u32 hdr_extension_code;
  u32 q_method1;
  u32 q_method2;
  u32 half_pel;
  u32 quarter_pel;
  u32 short_video;
};

/* vc-1 tools */

#if 0
/* storage for temp trace values */
typedef struct
{
    u32 quantizer;
} struct TraceStorage;
#endif

struct TraceVc1DecTools {
  enum TraceDecodingMode decoding_mode;
  struct TracePicCodingType pic_coding_type;
  struct TraceSequenceType sequence_type;
  u32 vs_transform;
  u32 overlap_transform;
  u32 four_mv;
  u32 qpel_luma;
  u32 qpel_chroma;
  u32 range_reduction;
  u32 intensity_compensation;
  u32 multi_resolution;
  u32 adaptive_mblock_quant;
  u32 loop_filter;
  u32 range_mapping;
  u32 extended_mv;
#if 0
    struct TraceStorage storage;
#endif
};

struct TraceRvDecTools {
  enum TraceDecodingMode decoding_mode;
  struct TracePicCodingType pic_coding_type;
};

/* global variables for tool tracing */
extern struct TraceMpeg2DecTools trace_mpeg2_dec_tools;
extern struct TraceH264DecTools trace_h264_dec_tools;
extern struct TraceJpegDecTools trace_jpeg_dec_tools;
extern struct TraceMpeg4DecTools trace_mpeg4_dec_tools;
extern struct TraceVc1DecTools trace_vc1_dec_tools;
extern struct TraceRvDecTools trace_rv_dec_tools;

/* tracing functions */
u32 OpenAsicTraceFiles(void);
void CloseAsicTraceFiles(void);
void TraceSequenceCtrl(u32 nmb_of_pics, u32 b_frames);
void TraceMPEG2DecodingTools();
void TraceH264DecodingTools();
void TraceJpegDecodingTools();
void TraceMPEG4DecodingTools();
void TraceVC1DecodingTools();
void TraceRvDecodingTools();

#endif /* TRACE_H */
