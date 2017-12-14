/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef TB_DEFS_H
#define TB_DEFS_H

#ifdef _ASSERT_USED
#include <assert.h>
#endif

#include <stdio.h>

#include "basetype.h"

/*------------------------------------------------------------------------------
    Generic data type stuff
------------------------------------------------------------------------------*/

typedef enum {
  TB_FALSE = 0,
  TB_TRUE = 1
} TBBool;

/*------------------------------------------------------------------------------
    Test bench configuration    u32 stride_enable;

------------------------------------------------------------------------------*/
struct TBParams {
  char packet_by_packet[9];
  char nal_unit_stream[9];
  u32 seed_rnd;
  char stream_bit_swap[24];
  char stream_bit_loss[24];
  char stream_packet_loss[24];
  char stream_header_corrupt[9];
  char stream_truncate[9];
  char slice_ud_in_packet[9];
  u32 first_trace_frame;

  u32 extra_cu_ctrl_eof;
  u32 memory_page_size;
  i32 ref_frm_buffer_size;
};

struct TBDecParams {
  char output_picture_endian[14];
  u32 bus_burst_length;
  u32 asic_service_priority;
  char output_format[12];
  u32 latency_compensation;
  u32 clk_gate_decoder;
  u32 clk_gate_decoder_idle;
  char data_discard[9];

  char memory_allocation[9];
  char rlc_mode_forced[9];
  char error_concealment[15];

  u32 jpeg_mcus_slice;
  u32 jpeg_input_buffer_size;

  u32 refbu_enable;
  u32 refbu_disable_interlaced;
  u32 refbu_disable_double;
  u32 refbu_disable_eval_mode;
  u32 refbu_disable_checkpoint;
  u32 refbu_disable_offset;
  u32 refbu_data_excess_max_pct;
  u32 refbu_disable_top_bot_sum;

  u32 mpeg2_support;
  u32 vc1_support;
  u32 jpeg_support;
  u32 mpeg4_support;
  u32 custom_mpeg4_support;
  u32 h264_support;
  u32 vp6_support;
  u32 vp7_support;
  u32 vp8_support;
  u32 prog_jpeg_support;
  u32 sorenson_support;
  u32 avs_support;
  u32 rv_support;
  u32 mvc_support;
  u32 webp_support;
  u32 ec_support;
  u32 max_dec_pic_width;
  u32 max_dec_pic_height;
  u32 hw_version;
  u32 hw_build;
  u32 bus_width;
  u32 latency;
  u32 non_seq_clk;
  u32 seq_clk;
  u32 support_non_compliant;
  u32 jpeg_esupport;
  u32 hevc_main10_support;
  u32 vp9_profile2_support;
  u32 rfc_support;
  u32 ds_support;
  u32 ring_buffer_support;
  u32 mrb_prefetch;
  u32 addr_64bit_support;
  u32 format_p010_support;
  u32 format_customer1_support;

  u32 force_mpeg4_idct;
  u32 ch8_pix_ileav_output;

  u32 ref_buffer_test_mode_offset_enable;
  i32 ref_buffer_test_mode_offset_min;
  i32 ref_buffer_test_mode_offset_max;
  i32 ref_buffer_test_mode_offset_start;
  i32 ref_buffer_test_mode_offset_incr;

  u32 apf_disable;
  u32 apf_threshold_disable;
  i32 apf_threshold_value;

  u32 tiled_ref_support;
  u32 stride_support;
  i32 field_dpb_support;

  u32 service_merge_disable;

  u32 strm_swap;
  u32 pic_swap;
  u32 dirmv_swap;
  u32 tab0_swap;
  u32 tab1_swap;
  u32 tab2_swap;
  u32 tab3_swap;
  u32 rscan_swap;
  u32 comp_tab_swap;
  u32 max_burst;
  u32 ref_double_buffer_enable;

  u32 timeout_cycles;

  u32 axi_id_rd;
  u32 axi_id_rd_unique_enable;
  u32 axi_id_wr;
  u32 axi_id_wr_unique_enable;
};

struct TBPpParams {
  char output_picture_endian[14];
  char input_picture_endian[14];
  char word_swap[9];
  char word_swap16[9];
  u32 bus_burst_length;
  char clock_gating[9];
  char data_discard[9];
  char multi_buffer[9];

  u32 max_pp_out_pic_width;
  u32 ppd_exists;
  u32 dithering_support;
  u32 scaling_support;
  u32 deinterlacing_support;
  u32 alpha_blending_support;
  u32 ablend_crop_support;
  u32 pp_out_endian_support;
  u32 tiled_support;
  u32 tiled_ref_support;

  i32 fast_hor_down_scale_disable;
  i32 fast_ver_down_scale_disable;
  i32 vert_down_scale_stripe_disable_support;
  u32 pix_acc_out_support;
};

struct TBCfg {
  struct TBParams tb_params;
  struct TBDecParams dec_params;
  struct TBPpParams pp_params;
};

#endif /* TB_DEFS_H */
