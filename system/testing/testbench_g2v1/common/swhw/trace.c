/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Common tracing functionalities for the 8170 system model. */

#include "trace.h"
#include "../../../../system/models/g2hw/asic_trace.h"

extern u32 pic_number;
extern u32 test_data_format; /* 0->trc, 1->hex */

struct TraceMpeg2DecTools trace_mpeg2_dec_tools;
struct TraceH264DecTools trace_h264_dec_tools;
struct TraceJpegDecTools trace_jpeg_dec_tools;
struct TraceMpeg4DecTools trace_mpeg4_dec_tools;
struct TraceVc1DecTools trace_vc1_dec_tools;
struct TraceRvDecTools trace_rv_dec_tools;

extern struct TraceFile trace_files[];

static FILE *trace_decoding_tools_fp = NULL;

/*------------------------------------------------------------------------------
 *   Function : OpenAsicTraceFiles
 * ---------------------------------------------------------------------------*/
u32 OpenAsicTraceFiles(void) {

  char trace_string[80];
  FILE *trace_cfg;
  u32 i;
  u32 top = 0, all = 0;

  /* traces already opened */
  if (trace_files[0].fid) return 1;

  trace_cfg = fopen("trace.cfg", "r");
  if (!trace_cfg) {
    return (0);
  }

  while (fscanf(trace_cfg, "%s\n", trace_string) != EOF) {
    if (!strcmp(trace_string, "toplevel")) top = 1;

    if (!strcmp(trace_string, "all")) all = 1;

#if 0
        if(!strcmp(trace_string, "decoding_tools"))
        {
            /* MPEG2 decoding tools trace */
            memset(&trace_mpeg2_dec_tools, 0,
                   sizeof(struct TraceMpeg2DecTools));
#if 0
            /* by default MPEG-1 is decoded and MPEG-2 is signaled */
            trace_mpeg2_dec_tools.decoding_mode = TRACE_MPEG1;

            /* MPEG-1 is always progressive */
            trace_mpeg2_dec_tools.sequence_type.interlaced = 0;
            trace_mpeg2_dec_tools.sequence_type.progressive = 1;
#endif
            /* H.264 decoding tools trace */
            memset(&trace_h264_dec_tools, 0,
                   sizeof(struct TraceH264DecTools));
#if 0
            trace_h264_dec_tools.decoding_mode = TRACE_H264;
#endif

            /* JPEG decoding tools trace */
            memset(&trace_jpeg_dec_tools, 0,
                   sizeof(struct TraceJpegDecTools));
#if 0
            trace_jpeg_dec_tools.decoding_mode = TRACE_JPEG;
#endif

            /* MPEG4 decoding tools trace */
            memset(&trace_mpeg4_dec_tools, 0,
                   sizeof(struct TraceMpeg4DecTools));
#if 0
            trace_mpeg4_dec_tools.decoding_mode = TRACE_MPEG4;
#endif

            /* VC1 decoding tools trace */
            memset(&trace_vc1_dec_tools, 0,
                   sizeof(struct TraceVc1DecTools));
#if 0
            trace_vc1_dec_tools.decoding_mode = TRACE_VC1;
#endif

            memset(&trace_rv_dec_tools, 0,
                   sizeof(struct TraceRvDecTools));

            trace_decoding_tools_fp = fopen("decoding_tools.trc", "w");

            if(trace_decoding_tools_fp == NULL)
                return (1);
        }
#endif
  }

  for (i = 0; i < NUM_TRACES; i++) {
    if (strlen(trace_files[i].name) == 0) break;
    if (all || (top && trace_files[i].top_level)) {
      trace_files[i].fid =
          fopen(trace_files[i].name, trace_files[i].bin ? "wb" : "w");
      if (trace_files[i].fid == NULL) return 1;
    }
  }

  return (1);
}

/*------------------------------------------------------------------------------
 *   Function : CloseAsicTraceFiles
 * ---------------------------------------------------------------------------*/
void CloseAsicTraceFiles(void) {
  u32 i;

  TraceSimCtrlClose();

  for (i = 0; i < NUM_TRACES; i++) {
    if (strlen(trace_files[i].name) == 0) break;
    if (trace_files[i].fid) {
      fclose(trace_files[i].fid);
      trace_files[i].fid = NULL;
    }
  }
}

/*------------------------------------------------------------------------------
 *   Function : TraceSequenceCtrl
 * ---------------------------------------------------------------------------*/
void TraceSequenceCtrl(u32 nmb_of_pics, u32 b_frames) {
  FILE *fid = trace_files[SEQUENCE_CTRL].fid;
  if (!fid) return;

  fprintf(fid, "%d\tAmount of pictures in the sequence\n", nmb_of_pics);
  fprintf(fid, "%d\tSequence includes B frames", b_frames);
}

static void trace_decoding_mode(FILE *trc, enum TraceDecodingMode dec_mode) {
  switch (dec_mode) {
    case TRACE_H263:
      fprintf(trc, "# H.263\n");
      break;
    case TRACE_H264:
      fprintf(trc, "# H.264\n");
      break;
    case TRACE_MPEG1:
      fprintf(trc, "# MPEG-1\n");
      break;
    case TRACE_MPEG2:
      fprintf(trc, "# MPEG-2\n");
      break;
    case TRACE_MPEG4:
      fprintf(trc, "# MPEG-4\n");
      break;
    case TRACE_VC1:
      fprintf(trc, "# VC-1\n");
      break;
    case TRACE_JPEG:
      fprintf(trc, "# JPEG\n");
      break;
    default:
      fprintf(trc, "# UNKNOWN\n");
      break;
  }
}

static void trace_pic_coding_type(FILE *trc, char *name,
                                  struct TracePicCodingType *pic_coding_type) {
  fprintf(trc, "%d    I-%s\n", pic_coding_type->i_coded, name);
  fprintf(trc, "%d    P-%s\n", pic_coding_type->p_coded, name);
  fprintf(trc, "%d    B-%s\n", pic_coding_type->b_coded, name);
}

static void trace_sequence_type(FILE *trc,
                                struct TraceSequenceType *sequence_type) {
  fprintf(trc, "%d    Interlaced sequence type\n", sequence_type->interlaced);
  fprintf(trc, "%d    Progressive sequence type\n", sequence_type->progressive);
}

void TraceMPEG2DecodingTools() {
  if (!trace_decoding_tools_fp) return;

  trace_decoding_mode(trace_decoding_tools_fp,
                      trace_mpeg2_dec_tools.decoding_mode);

  trace_pic_coding_type(trace_decoding_tools_fp, "Picture",
                        &trace_mpeg2_dec_tools.pic_coding_type);

  fprintf(trace_decoding_tools_fp, "%d    D-Picture\n",
          trace_mpeg2_dec_tools.d_coded);

  trace_sequence_type(trace_decoding_tools_fp,
                      &trace_mpeg2_dec_tools.sequence_type);
}

void TraceH264DecodingTools() {
  if (!trace_decoding_tools_fp) return;

  trace_decoding_mode(trace_decoding_tools_fp,
                      trace_h264_dec_tools.decoding_mode);

  trace_pic_coding_type(trace_decoding_tools_fp, "Slice",
                        &trace_h264_dec_tools.pic_coding_type);

  fprintf(trace_decoding_tools_fp, "%d    Multiple slices per picture\n",
          trace_h264_dec_tools.multiple_slices_per_picture);

  fprintf(trace_decoding_tools_fp, "%d    Baseline profile\n",
          trace_h264_dec_tools.profile_type.baseline);

  fprintf(trace_decoding_tools_fp, "%d    Main profile\n",
          trace_h264_dec_tools.profile_type.main);

  fprintf(trace_decoding_tools_fp, "%d    High profile\n",
          trace_h264_dec_tools.profile_type.high);

  fprintf(trace_decoding_tools_fp, "%d    I-PCM MB type\n",
          trace_h264_dec_tools.ipcm);

  fprintf(trace_decoding_tools_fp, "%d    Direct mode MB type\n",
          trace_h264_dec_tools.direct_mode);

  fprintf(trace_decoding_tools_fp, "%d    Monochrome\n",
          trace_h264_dec_tools.monochrome);

  fprintf(trace_decoding_tools_fp, "%d    8x8 transform\n",
          trace_h264_dec_tools.transform8x8);

  fprintf(trace_decoding_tools_fp, "%d    8x8 intra prediction\n",
          trace_h264_dec_tools.intra_prediction8x8);

  fprintf(trace_decoding_tools_fp, "%d    Scaling list present\n",
          trace_h264_dec_tools.scaling_list_present);

  fprintf(trace_decoding_tools_fp, "%d    Scaling list present (SPS)\n",
          trace_h264_dec_tools.scaling_matrix_present_type.seq);

  fprintf(trace_decoding_tools_fp, "%d    Scaling list present (PPS)\n",
          trace_h264_dec_tools.scaling_matrix_present_type.pic);

  fprintf(trace_decoding_tools_fp,
          "%d    Weighted prediction, explicit, P slice\n",
          trace_h264_dec_tools.weighted_prediction_type.explicit);

  fprintf(trace_decoding_tools_fp,
          "%d    Weighted prediction, explicit, B slice\n",
          trace_h264_dec_tools.weighted_prediction_type.explicit_b);

  fprintf(trace_decoding_tools_fp, "%d    Weighted prediction, implicit\n",
          trace_h264_dec_tools.weighted_prediction_type.implicit);

  fprintf(trace_decoding_tools_fp, "%d    In-loop filter control present\n",
          trace_h264_dec_tools.loop_filter);

  fprintf(trace_decoding_tools_fp,
          "%d    Disable Loop filter in slice header\n",
          trace_h264_dec_tools.loop_filter_dis);

  fprintf(trace_decoding_tools_fp, "%d    CAVLC entropy coding\n",
          trace_h264_dec_tools.entropy_coding.cavlc);

  fprintf(trace_decoding_tools_fp, "%d    CABAC entropy coding\n",
          trace_h264_dec_tools.entropy_coding.cabac);

  if (trace_h264_dec_tools.seq_type.ilaced == 0)
    trace_h264_dec_tools.seq_type.prog = 1;
  fprintf(trace_decoding_tools_fp, "%d    Progressive sequence type\n",
          trace_h264_dec_tools.seq_type.prog);

  fprintf(trace_decoding_tools_fp, "%d    Interlace sequence type\n",
          trace_h264_dec_tools.seq_type.ilaced);

  fprintf(trace_decoding_tools_fp, "%d    PicAff\n",
          trace_h264_dec_tools.interlace_type.pic_aff);

  fprintf(trace_decoding_tools_fp, "%d    MbAff\n",
          trace_h264_dec_tools.interlace_type.mb_aff);

  fprintf(trace_decoding_tools_fp, "%d    NAL unit stream\n",
          trace_h264_dec_tools.stream_mode.nal_unit_strm);

  fprintf(trace_decoding_tools_fp, "%d    Byte stream\n",
          trace_h264_dec_tools.stream_mode.byte_strm);

  fprintf(trace_decoding_tools_fp, "%d    More than 1 slice groups (FMO)\n",
          trace_h264_dec_tools.slice_groups);

  fprintf(trace_decoding_tools_fp, "%d    Arbitrary slice order (ASO)\n",
          trace_h264_dec_tools.arbitrary_slice_order);

  fprintf(trace_decoding_tools_fp, "%d    Redundant slices\n",
          trace_h264_dec_tools.redundant_slices);

  fprintf(trace_decoding_tools_fp, "%d    Image cropping\n",
          trace_h264_dec_tools.image_cropping);

  fprintf(trace_decoding_tools_fp, "%d    Error stream\n",
          trace_h264_dec_tools.error);
}

void TraceJpegDecodingTools() {
  if (!trace_decoding_tools_fp) return;

  trace_decoding_mode(trace_decoding_tools_fp,
                      trace_jpeg_dec_tools.decoding_mode);

  fprintf(trace_decoding_tools_fp, "%d     YCbCr 4:2:0 sampling\n",
          trace_jpeg_dec_tools.sampling_4_2_0);

  fprintf(trace_decoding_tools_fp, "%d     YCbCr 4:2:2 sampling\n",
          trace_jpeg_dec_tools.sampling_4_2_2);

  fprintf(trace_decoding_tools_fp, "%d     YCbCr 4:0:0 sampling\n",
          trace_jpeg_dec_tools.sampling_4_0_0);

  fprintf(trace_decoding_tools_fp, "%d     YCbCr 4:4:0 sampling\n",
          trace_jpeg_dec_tools.sampling_4_4_0);

  fprintf(trace_decoding_tools_fp, "%d     YCbCr 4:1:1 sampling\n",
          trace_jpeg_dec_tools.sampling_4_1_1);

  fprintf(trace_decoding_tools_fp, "%d     YCbCr 4:4:4 sampling\n",
          trace_jpeg_dec_tools.sampling_4_4_4);

  fprintf(trace_decoding_tools_fp, "%d     JPEG compressed thumbnail\n",
          trace_jpeg_dec_tools.thumbnail);

  fprintf(trace_decoding_tools_fp, "%d     Progressive decoding\n",
          trace_jpeg_dec_tools.progressive);
}

void TraceMPEG4DecodingTools() {
  if (!trace_decoding_tools_fp) return;

  trace_decoding_mode(trace_decoding_tools_fp,
                      trace_mpeg4_dec_tools.decoding_mode);

  trace_pic_coding_type(trace_decoding_tools_fp, "VOP",
                        &trace_mpeg4_dec_tools.pic_coding_type);

  trace_sequence_type(trace_decoding_tools_fp,
                      &trace_mpeg4_dec_tools.sequence_type);

  fprintf(trace_decoding_tools_fp, "%d    4-MV\n",
          trace_mpeg4_dec_tools.four_mv);

  fprintf(trace_decoding_tools_fp, "%d    AC/DC prediction\n",
          trace_mpeg4_dec_tools.ac_pred);

  fprintf(trace_decoding_tools_fp, "%d    Data partition\n",
          trace_mpeg4_dec_tools.data_partition);

  fprintf(trace_decoding_tools_fp,
          "%d    Slice resynchronization / Video packets\n",
          trace_mpeg4_dec_tools.resync_marker);

  fprintf(trace_decoding_tools_fp, "%d    Reversible VLC\n",
          trace_mpeg4_dec_tools.reversible_vlc);

  fprintf(trace_decoding_tools_fp, "%d    Header extension code\n",
          trace_mpeg4_dec_tools.hdr_extension_code);

  fprintf(trace_decoding_tools_fp, "%d    Quantisation Method 1\n",
          trace_mpeg4_dec_tools.q_method1);

  fprintf(trace_decoding_tools_fp, "%d    Quantisation Method 2\n",
          trace_mpeg4_dec_tools.q_method2);

  fprintf(trace_decoding_tools_fp, "%d    Half-pel motion compensation\n",
          trace_mpeg4_dec_tools.half_pel);

  fprintf(trace_decoding_tools_fp, "%d    Quarter-pel motion compensation\n",
          trace_mpeg4_dec_tools.quarter_pel);

  fprintf(trace_decoding_tools_fp, "%d    Short video header\n",
          trace_mpeg4_dec_tools.short_video);
}

void TraceVC1DecodingTools() {
  if (!trace_decoding_tools_fp) return;

  trace_decoding_mode(trace_decoding_tools_fp,
                      trace_vc1_dec_tools.decoding_mode);

  trace_pic_coding_type(trace_decoding_tools_fp, "frames",
                        &trace_vc1_dec_tools.pic_coding_type);

  trace_sequence_type(trace_decoding_tools_fp,
                      &trace_vc1_dec_tools.sequence_type);

  fprintf(trace_decoding_tools_fp, "%d    Variable-sized transform\n",
          trace_vc1_dec_tools.vs_transform);

  fprintf(trace_decoding_tools_fp, "%d    Overlapped transform\n",
          trace_vc1_dec_tools.overlap_transform);

  fprintf(trace_decoding_tools_fp, "%d    4 motion vectors per macroblock\n",
          trace_vc1_dec_tools.four_mv);

  fprintf(trace_decoding_tools_fp,
          "%d    Quarter-pixel motion compensation Y\n",
          trace_vc1_dec_tools.qpel_luma);

  fprintf(trace_decoding_tools_fp,
          "%d    Quarter-pixel motion compensation C\n",
          trace_vc1_dec_tools.qpel_chroma);

  fprintf(trace_decoding_tools_fp, "%d    Range reduction\n",
          trace_vc1_dec_tools.range_reduction);

  fprintf(trace_decoding_tools_fp, "%d    Intensity compensation\n",
          trace_vc1_dec_tools.intensity_compensation);

  fprintf(trace_decoding_tools_fp, "%d    Multi-resolution\n",
          trace_vc1_dec_tools.multi_resolution);

  fprintf(trace_decoding_tools_fp, "%d    Adaptive macroblock quantization\n",
          trace_vc1_dec_tools.adaptive_mblock_quant);

  fprintf(trace_decoding_tools_fp, "%d    In-loop deblock filtering\n",
          trace_vc1_dec_tools.loop_filter);

  fprintf(trace_decoding_tools_fp, "%d    Range mapping\n",
          trace_vc1_dec_tools.range_mapping);

  fprintf(trace_decoding_tools_fp, "%d    Extended motion vectors\n",
          trace_vc1_dec_tools.extended_mv);
}

void TraceRvDecodingTools() {
  if (!trace_decoding_tools_fp) return;

  trace_decoding_mode(trace_decoding_tools_fp,
                      trace_rv_dec_tools.decoding_mode);

  trace_pic_coding_type(trace_decoding_tools_fp, "frames",
                        &trace_rv_dec_tools.pic_coding_type);
}
