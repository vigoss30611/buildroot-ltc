/* Copyright 2012 Google Inc. All Rights Reserved. */

#include <string.h>
#include <stdlib.h>
#include "tb_cfg.h"
#include "deccfg.h"
#include "ppcfg.h"
#include "dwl.h"

#include "regdrv.h"

#ifdef _ASSERT_USED
#include <assert.h>
#endif
#ifdef _ASSERT_USED
#define ASSERT(expr) if (!(expr)) printf("+++ G2 ASSERT(%s:%d) 555:"#expr"...\n", __FILE__, __LINE__)//assert(expr)
#else
#define ASSERT(expr)                                                        \
  if (!(expr)) {                                                            \
    printf("G2 ASSERT, file: %s line: %d :: %s.\n", __FILE__, __LINE__, \
           #expr);                                                          \
    abort();                                                                \
  }
#endif

/*------------------------------------------------------------------------------
    Blocks
------------------------------------------------------------------------------*/
#define BLOCK_TB_PARAMS ("TbParams")
#define BLOCK_DEC_PARAMS ("DecParams")
#define BLOCK_PP_PARAMS ("PpParams")


/*------------------------------------------------------------------------------
    Keys
------------------------------------------------------------------------------*/
#define KEY_PACKET_BY_PACKET      ("PacketByPacket")
#define KEY_NAL_UNIT_STREAM       ("NalUnitStream")
#define KEY_SEED_RND              ("SeedRnd")
#define KEY_STREAM_BIT_SWAP       ("StreamBitSwap")
#define KEY_STREAM_BIT_LOSS       ("StreamBitLoss")
#define KEY_STREAM_PACKET_LOSS    ("StreamPacketLoss")
#define KEY_STREAM_HEADER_CORRUPT ("StreamHeaderCorrupt")
#define KEY_STREAM_TRUNCATE       ("StreamTruncate")
#define KEY_SLICE_UD_IN_PACKET    ("SliceUdInPacket")
#define KEY_FIRST_TRACE_FRAME     ("FirstTraceFrame")
#define KEY_EXTRA_CU_CTRL_EOF     ("ExtraCuCtrlEof")

#define KEY_REFBU_ENABLED         ("refbuEnable")
#define KEY_REFBU_DIS_INTERLACED  ("refbuDisableInterlaced")
#define KEY_REFBU_DIS_DOUBLE      ("refbuDisableDouble")
#define KEY_REFBU_DIS_EVAL_MODE   ("refbuDisableEvalMode")
#define KEY_REFBU_DIS_CHECKPOINT  ("refbuDisableCheckpoint")
#define KEY_REFBU_DIS_OFFSET      ("refbuDisableOffset")
#define KEY_REFBU_DIS_TOPBOT      ("refbuDisableTopBotSum")
#define KEY_REFBU_DATA_EXCESS     ("refbuAdjustValue")

#define KEY_REFBU_TEST_OFFS       ("refbuTestOffsetEnable")
#define KEY_REFBU_TEST_OFFS_MIN   ("refbuTestOffsetMin")
#define KEY_REFBU_TEST_OFFS_MAX   ("refbuTestOffsetMax")
#define KEY_REFBU_TEST_OFFS_START ("refbuTestOffsetStart")
#define KEY_REFBU_TEST_OFFS_INCR  ("refbuTestOffsetIncr")

#define KEY_APF_THRESHOLD_DIS     ("apfDisableThreshold")
#define KEY_APF_THRESHOLD_VAL     ("apfThresholdValue")
#define KEY_APF_DISABLE           ("apfDisable")

#define KEY_BUS_WIDTH             ("BusWidth")
#define KEY_MEM_LATENCY           ("MemLatencyClks")
#define KEY_MEM_NONSEQ            ("MemNonSeqClks")
#define KEY_MEM_SEQ               ("MemSeqClks")
#define KEY_STRM_SWAP             ("strmSwap")
#define KEY_PIC_SWAP              ("picSwap")
#define KEY_DIRMV_SWAP            ("dirmvSwap")
#define KEY_TAB0_SWAP             ("tab0Swap")
#define KEY_TAB1_SWAP             ("tab1Swap")
#define KEY_TAB2_SWAP             ("tab2Swap")
#define KEY_TAB3_SWAP             ("tab3Swap")
#define KEY_RSCAN_SWAP            ("rscanSwap")
#define KEY_MAX_BURST             ("maxBurst")

#define KEY_SUPPORT_MPEG2         ("SupportMpeg2")
#define KEY_SUPPORT_VC1           ("SupportVc1")
#define KEY_SUPPORT_JPEG          ("SupportJpeg")
#define KEY_SUPPORT_MPEG4         ("SupportMpeg4")
#define KEY_SUPPORT_H264          ("SupportH264")
#define KEY_SUPPORT_VP6           ("SupportVp6")
#define KEY_SUPPORT_VP7           ("SupportVp7")
#define KEY_SUPPORT_VP8           ("SupportVp8")
#define KEY_SUPPORT_PJPEG         ("SupportPjpeg")
#define KEY_SUPPORT_SORENSON      ("SupportSorenson")
#define KEY_SUPPORT_AVS           ("SupportAvs")
#define KEY_SUPPORT_RV            ("SupportRv")
#define KEY_SUPPORT_MVC           ("SupportMvc")
#define KEY_SUPPORT_WEBP          ("SupportWebP")
#define KEY_SUPPORT_EC            ("SupportEc")
#define KEY_SUPPORT_STRIDE        ("SupportStride")
#define KEY_SUPPORT_CUSTOM_MPEG4  ("SupportCustomMpeg4")
#define KEY_SUPPORT_JPEGE         ("SupportJpegE")
#define KEY_SUPPORT_NON_COMPLIANT ("SupportNonCompliant")
#define KEY_SUPPORT_PP_OUT_ENDIAN ("SupportPpOutEndianess")
#define KEY_SUPPORT_STRIPE_DIS    ("SupportStripeRemoval")
#define KEY_MAX_DEC_PIC_WIDTH     ("MaxDecPicWidth")
#define KEY_MAX_DEC_PIC_HEIGHT    ("MaxDecPicHeight")

#define KEY_SUPPORT_PPD           ("SupportPpd")
#define KEY_SUPPORT_DITHER        ("SupportDithering")
#define KEY_SUPPORT_TILED         ("SupportTiled")
#define KEY_SUPPORT_TILED_REF     ("SupportTiledReference")
#define KEY_SUPPORT_FIELD_DPB     ("SupportFieldDPB")
#define KEY_SUPPORT_PIX_ACC_OUT   ("SupportPixelAccurOut")
#define KEY_SUPPORT_SCALING       ("SupportScaling")
#define KEY_SUPPORT_DEINT         ("SupportDeinterlacing")
#define KEY_SUPPORT_ABLEND        ("SupportAlphaBlending")
#define KEY_SUPPORT_ABLEND_CROP   ("SupportAblendCrop")
#define KEY_FAST_HOR_D_SCALE_DIS  ("FastHorizontalDownscaleDisable")
#define KEY_FAST_VER_D_SCALE_DIS  ("FastVerticalDownscaleDisable")
#define KEY_D_SCALE_STRIPES_DIS   ("VerticalDownscaleStripesDisable")
#define KEY_MAX_PP_OUT_PIC_WIDTH  ("MaxPpOutPicWidth")
#define KEY_HW_VERSION            ("HwVersion")
#define KEY_HW_BUILD              ("HwBuild")

#define KEY_DWL_PAGE_SIZE         ("DwlMemPageSize")
#define KEY_DWL_REF_FRM_BUFFER    ("DwlRefFrmBufferSize")

#define KEY_OUTPUT_PICTURE_ENDIAN ("OutputPictureEndian")
#define KEY_BUS_BURST_LENGTH      ("BusBurstLength")
#define KEY_ASIC_SERVICE_PRIORITY ("AsicServicePriority")
#define KEY_OUTPUT_FORMAT         ("OutputFormat")
#define KEY_LATENCY_COMPENSATION  ("LatencyCompensation")
#define KEY_CLOCK_GATING          ("clkGateDecoder")
#define KEY_CLOCK_GATING_RUNTIME  ("clkGateDecoderIdle")
#define KEY_DATA_DISCARD          ("DataDiscard")
#define KEY_MEMORY_ALLOCATION     ("MemoryAllocation")
#define KEY_RLC_MODE_FORCED       ("RlcModeForced")
#define KEY_ERROR_CONCEALMENT     ("ErrorConcealment")
#define KEY_JPEG_MCUS_SLICE            ("JpegMcusSlice")
#define KEY_JPEG_INPUT_BUFFER_SIZE     ("JpegInputBufferSize")

#define KEY_INPUT_PICTURE_ENDIAN  ("InputPictureEndian")
#define KEY_WORD_SWAP             ("WordSwap")
#define KEY_WORD_SWAP_16          ("WordSwap16")
#define KEY_FORCE_MPEG4_IDCT      ("ForceMpeg4Idct")

#define KEY_MULTI_BUFFER          ("MultiBuffer")

#define KEY_CH_8PIX_ILEAV         ("Ch8PixIleavOutput")

#define KEY_SERV_MERGE_DISABLE    ("ServiceMergeDisable")

#define KEY_DOUBLE_REF_BUFFER     ("refDoubleBufferEnable")

#define KEY_TIMEOUT_CYCLES        ("timeoutOverrideLimit")

#define KEY_AXI_ID_R              ("axiIdRd")
#define KEY_AXI_ID_RE             ("axiIdRdUniqueE")
#define KEY_AXI_ID_W              ("axiIdWr")
#define KEY_AXI_ID_WE             ("axiIdWrUniqueE")

/*------------------------------------------------------------------------------
    Implement reading interer parameter
------------------------------------------------------------------------------*/
#define IMPLEMENT_PARAM_INTEGER(b, k, tgt)    \
  if (!strcmp(block, b) && !strcmp(key, k)) { \
    char* endptr;                             \
    tgt = strtol(value, &endptr, 10);         \
    if (*endptr) return TB_CFG_INVALID_VALUE; \
  }
/*------------------------------------------------------------------------------
    Implement reading string parameter
------------------------------------------------------------------------------*/
#define IMPLEMENT_PARAM_STRING(b, k, tgt)     \
  if (!strcmp(block, b) && !strcmp(key, k)) { \
    strncpy(tgt, value, sizeof(tgt));         \
  }

/*------------------------------------------------------------------------------
    Implement reading code parameter; Code parsing is handled by supplied
    function fn.
------------------------------------------------------------------------------*/
#define INVALID_CODE (0xFFFFFFFF)
#define IMPLEMENT_PARAM_CODE(b, k, tgt, fn)                            \
  if (block && key && !strcmp(block, b) && !strcmp(key, k)) {          \
    if ((tgt = fn(value)) == INVALID_CODE) return TB_CFG_INVALID_CODE; \
  }
/*------------------------------------------------------------------------------
    Implement structure allocation upon parsing a specific block.
------------------------------------------------------------------------------*/
#define IMPLEMENT_ALLOC_BLOCK(b, tgt, type) \
  if (key && !strcmp(key, b)) {             \
    register type** t = (type**)&tgt;       \
    if (!*t) {                              \
      *t = (type*)malloc(sizeof(type));     \
      ASSERT(*t);                           \
      memset(*t, 0, sizeof(type));          \
    } else                                  \
      return CFG_DUPLICATE_BLOCK;           \
  }

u32 ParseRefbuTestMode(char* value) {
  if (!strcmp(value, "NONE")) return 0;
  if (!strcmp(value, "OFFSET")) return 1;

  return INVALID_CODE;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: ReadParam

        Functional description:
          Read parameter callback function.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
enum TBCfgCallbackResult TBReadParam(char* block, char* key, char* value,
                                     enum TBCfgCallbackParam state,
                                     void* cb_param) {
  struct TBCfg* tb_cfg = (struct TBCfg*)cb_param;

  ASSERT(key);
  ASSERT(tb_cfg);

  switch (state) {
    case TB_CFG_CALLBACK_BLK_START:
      /*printf("CFG_CALLBACK_BLK_START\n");
        printf("block == %s\n", block);
        printf("key == %s\n", key);
        printf("value == %s\n\n", value);*/
      /*IMPLEMENT_ALLOC_BLOCK( BLOCK_TD_PARAMS, tb_cfg->tb_params, TbParams );
        IMPLEMENT_ALLOC_BLOCK( BLOCK_DEC_PARAMS, tb_cfg->dec_params, DecParams
        );
        IMPLEMENT_ALLOC_BLOCK( BLOCK_PP_PARAMS, tb_cfg->pp_params, PpParams );*/
      break;
    case TB_CFG_CALLBACK_VALUE:
      /*printf("CFG_CALLBACK_VALUE\n");
        printf("block == %s\n", block);
        printf("key == %s\n", key);
        printf("value == %s\n\n", value);*/
      /* TbParams */
      IMPLEMENT_PARAM_STRING(BLOCK_TB_PARAMS, KEY_PACKET_BY_PACKET,
                             tb_cfg->tb_params.packet_by_packet);
      IMPLEMENT_PARAM_STRING(BLOCK_TB_PARAMS, KEY_NAL_UNIT_STREAM,
                             tb_cfg->tb_params.nal_unit_stream);
      IMPLEMENT_PARAM_INTEGER(BLOCK_TB_PARAMS, KEY_SEED_RND,
                              tb_cfg->tb_params.seed_rnd);
      IMPLEMENT_PARAM_STRING(BLOCK_TB_PARAMS, KEY_STREAM_BIT_SWAP,
                             tb_cfg->tb_params.stream_bit_swap);
      IMPLEMENT_PARAM_STRING(BLOCK_TB_PARAMS, KEY_STREAM_BIT_LOSS,
                             tb_cfg->tb_params.stream_bit_loss);
      IMPLEMENT_PARAM_STRING(BLOCK_TB_PARAMS, KEY_STREAM_PACKET_LOSS,
                             tb_cfg->tb_params.stream_packet_loss);
      IMPLEMENT_PARAM_STRING(BLOCK_TB_PARAMS, KEY_STREAM_HEADER_CORRUPT,
                             tb_cfg->tb_params.stream_header_corrupt);
      IMPLEMENT_PARAM_STRING(BLOCK_TB_PARAMS, KEY_STREAM_TRUNCATE,
                             tb_cfg->tb_params.stream_truncate);
      IMPLEMENT_PARAM_STRING(BLOCK_TB_PARAMS, KEY_SLICE_UD_IN_PACKET,
                             tb_cfg->tb_params.slice_ud_in_packet);
      IMPLEMENT_PARAM_INTEGER(BLOCK_TB_PARAMS, KEY_FIRST_TRACE_FRAME,
                              tb_cfg->tb_params.first_trace_frame);
      IMPLEMENT_PARAM_INTEGER(BLOCK_TB_PARAMS, KEY_EXTRA_CU_CTRL_EOF,
                              tb_cfg->tb_params.extra_cu_ctrl_eof);
      IMPLEMENT_PARAM_INTEGER(BLOCK_TB_PARAMS, KEY_DWL_PAGE_SIZE,
                              tb_cfg->tb_params.memory_page_size);
      IMPLEMENT_PARAM_INTEGER(BLOCK_TB_PARAMS, KEY_DWL_REF_FRM_BUFFER,
                              tb_cfg->tb_params.ref_frm_buffer_size);

      /* DecParams */
      IMPLEMENT_PARAM_STRING(BLOCK_DEC_PARAMS, KEY_OUTPUT_PICTURE_ENDIAN,
                             tb_cfg->dec_params.output_picture_endian);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_BUS_BURST_LENGTH,
                              tb_cfg->dec_params.bus_burst_length);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_ASIC_SERVICE_PRIORITY,
                              tb_cfg->dec_params.asic_service_priority);
      IMPLEMENT_PARAM_STRING(BLOCK_DEC_PARAMS, KEY_OUTPUT_FORMAT,
                             tb_cfg->dec_params.output_format);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_LATENCY_COMPENSATION,
                              tb_cfg->dec_params.latency_compensation);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_CLOCK_GATING,
                              tb_cfg->dec_params.clk_gate_decoder);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_CLOCK_GATING_RUNTIME,
                              tb_cfg->dec_params.clk_gate_decoder_idle);
      IMPLEMENT_PARAM_STRING(BLOCK_DEC_PARAMS, KEY_DATA_DISCARD,
                             tb_cfg->dec_params.data_discard);
      IMPLEMENT_PARAM_STRING(BLOCK_DEC_PARAMS, KEY_MEMORY_ALLOCATION,
                             tb_cfg->dec_params.memory_allocation);
      IMPLEMENT_PARAM_STRING(BLOCK_DEC_PARAMS, KEY_RLC_MODE_FORCED,
                             tb_cfg->dec_params.rlc_mode_forced);
      IMPLEMENT_PARAM_STRING(BLOCK_DEC_PARAMS, KEY_ERROR_CONCEALMENT,
                             tb_cfg->dec_params.error_concealment);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_JPEG_MCUS_SLICE,
                              tb_cfg->dec_params.jpeg_mcus_slice);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_JPEG_INPUT_BUFFER_SIZE,
                              tb_cfg->dec_params.jpeg_input_buffer_size);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_REFBU_DIS_INTERLACED,
                              tb_cfg->dec_params.refbu_disable_interlaced);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_REFBU_DIS_DOUBLE,
                              tb_cfg->dec_params.refbu_disable_double);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_REFBU_DIS_EVAL_MODE,
                              tb_cfg->dec_params.refbu_disable_eval_mode);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_REFBU_DIS_CHECKPOINT,
                              tb_cfg->dec_params.refbu_disable_checkpoint);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_REFBU_DIS_OFFSET,
                              tb_cfg->dec_params.refbu_disable_offset);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_REFBU_DIS_TOPBOT,
                              tb_cfg->dec_params.refbu_disable_top_bot_sum);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_REFBU_DATA_EXCESS,
                              tb_cfg->dec_params.refbu_data_excess_max_pct);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_REFBU_ENABLED,
                              tb_cfg->dec_params.refbu_enable);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_TILED_REF,
                              tb_cfg->dec_params.tiled_ref_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_FIELD_DPB,
                              tb_cfg->dec_params.field_dpb_support);

      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_APF_THRESHOLD_DIS,
                              tb_cfg->dec_params.apf_threshold_disable);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_APF_THRESHOLD_VAL,
                              tb_cfg->dec_params.apf_threshold_value);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_APF_DISABLE,
                              tb_cfg->dec_params.apf_disable);

      IMPLEMENT_PARAM_INTEGER(
          BLOCK_DEC_PARAMS, KEY_REFBU_TEST_OFFS,
          tb_cfg->dec_params.ref_buffer_test_mode_offset_enable);
      IMPLEMENT_PARAM_INTEGER(
          BLOCK_DEC_PARAMS, KEY_REFBU_TEST_OFFS_MIN,
          tb_cfg->dec_params.ref_buffer_test_mode_offset_min);
      IMPLEMENT_PARAM_INTEGER(
          BLOCK_DEC_PARAMS, KEY_REFBU_TEST_OFFS_MAX,
          tb_cfg->dec_params.ref_buffer_test_mode_offset_max);
      IMPLEMENT_PARAM_INTEGER(
          BLOCK_DEC_PARAMS, KEY_REFBU_TEST_OFFS_START,
          tb_cfg->dec_params.ref_buffer_test_mode_offset_start);
      IMPLEMENT_PARAM_INTEGER(
          BLOCK_DEC_PARAMS, KEY_REFBU_TEST_OFFS_INCR,
          tb_cfg->dec_params.ref_buffer_test_mode_offset_incr);

      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_MPEG2,
                              tb_cfg->dec_params.mpeg2_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_VC1,
                              tb_cfg->dec_params.vc1_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_JPEG,
                              tb_cfg->dec_params.jpeg_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_MPEG4,
                              tb_cfg->dec_params.mpeg4_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_CUSTOM_MPEG4,
                              tb_cfg->dec_params.custom_mpeg4_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_H264,
                              tb_cfg->dec_params.h264_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_VP6,
                              tb_cfg->dec_params.vp6_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_VP7,
                              tb_cfg->dec_params.vp7_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_VP8,
                              tb_cfg->dec_params.vp8_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_PJPEG,
                              tb_cfg->dec_params.prog_jpeg_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_SORENSON,
                              tb_cfg->dec_params.sorenson_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_AVS,
                              tb_cfg->dec_params.avs_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_RV,
                              tb_cfg->dec_params.rv_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_MVC,
                              tb_cfg->dec_params.mvc_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_WEBP,
                              tb_cfg->dec_params.webp_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_EC,
                              tb_cfg->dec_params.ec_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_STRIDE,
                              tb_cfg->dec_params.stride_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_JPEGE,
                              tb_cfg->dec_params.jpeg_esupport);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_MAX_DEC_PIC_WIDTH,
                              tb_cfg->dec_params.max_dec_pic_width);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_MAX_DEC_PIC_HEIGHT,
                              tb_cfg->dec_params.max_dec_pic_height);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SUPPORT_NON_COMPLIANT,
                              tb_cfg->dec_params.support_non_compliant);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_HW_VERSION,
                              tb_cfg->dec_params.hw_version);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_HW_BUILD,
                              tb_cfg->dec_params.hw_build);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_BUS_WIDTH,
                              tb_cfg->dec_params.bus_width);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_MEM_LATENCY,
                              tb_cfg->dec_params.latency);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_MEM_NONSEQ,
                              tb_cfg->dec_params.non_seq_clk);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_MEM_SEQ,
                              tb_cfg->dec_params.seq_clk);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_FORCE_MPEG4_IDCT,
                              tb_cfg->dec_params.force_mpeg4_idct);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_CH_8PIX_ILEAV,
                              tb_cfg->dec_params.ch8_pix_ileav_output);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_SERV_MERGE_DISABLE,
                              tb_cfg->dec_params.service_merge_disable);

      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_STRM_SWAP,
                              tb_cfg->dec_params.strm_swap);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_PIC_SWAP,
                              tb_cfg->dec_params.pic_swap);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_DIRMV_SWAP,
                              tb_cfg->dec_params.dirmv_swap);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_TAB0_SWAP,
                              tb_cfg->dec_params.tab0_swap);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_TAB1_SWAP,
                              tb_cfg->dec_params.tab1_swap);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_TAB2_SWAP,
                              tb_cfg->dec_params.tab2_swap);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_TAB3_SWAP,
                              tb_cfg->dec_params.tab3_swap);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_RSCAN_SWAP,
                              tb_cfg->dec_params.rscan_swap);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_MAX_BURST,
                              tb_cfg->dec_params.max_burst);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_DOUBLE_REF_BUFFER,
                              tb_cfg->dec_params.ref_double_buffer_enable);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_TIMEOUT_CYCLES,
                              tb_cfg->dec_params.timeout_cycles);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_AXI_ID_R,
                              tb_cfg->dec_params.axi_id_rd);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_AXI_ID_RE,
                              tb_cfg->dec_params.axi_id_rd_unique_enable);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_AXI_ID_W,
                              tb_cfg->dec_params.axi_id_wr);
      IMPLEMENT_PARAM_INTEGER(BLOCK_DEC_PARAMS, KEY_AXI_ID_WE,
                              tb_cfg->dec_params.axi_id_wr_unique_enable);
      /* PpParams */
      IMPLEMENT_PARAM_STRING(BLOCK_PP_PARAMS, KEY_OUTPUT_PICTURE_ENDIAN,
                             tb_cfg->pp_params.output_picture_endian);
      IMPLEMENT_PARAM_STRING(BLOCK_PP_PARAMS, KEY_INPUT_PICTURE_ENDIAN,
                             tb_cfg->pp_params.input_picture_endian);
      IMPLEMENT_PARAM_STRING(BLOCK_PP_PARAMS, KEY_WORD_SWAP,
                             tb_cfg->pp_params.word_swap);
      IMPLEMENT_PARAM_STRING(BLOCK_PP_PARAMS, KEY_WORD_SWAP_16,
                             tb_cfg->pp_params.word_swap16);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_BUS_BURST_LENGTH,
                              tb_cfg->pp_params.bus_burst_length);
      IMPLEMENT_PARAM_STRING(BLOCK_PP_PARAMS, KEY_CLOCK_GATING,
                             tb_cfg->pp_params.clock_gating);
      IMPLEMENT_PARAM_STRING(BLOCK_PP_PARAMS, KEY_DATA_DISCARD,
                             tb_cfg->pp_params.data_discard);
      IMPLEMENT_PARAM_STRING(BLOCK_PP_PARAMS, KEY_MULTI_BUFFER,
                             tb_cfg->pp_params.multi_buffer);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_PPD,
                              tb_cfg->pp_params.ppd_exists);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_DITHER,
                              tb_cfg->pp_params.dithering_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_SCALING,
                              tb_cfg->pp_params.scaling_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_DEINT,
                              tb_cfg->pp_params.deinterlacing_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_ABLEND,
                              tb_cfg->pp_params.alpha_blending_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_ABLEND_CROP,
                              tb_cfg->pp_params.ablend_crop_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_PP_OUT_ENDIAN,
                              tb_cfg->pp_params.pp_out_endian_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_TILED,
                              tb_cfg->pp_params.tiled_support);
      IMPLEMENT_PARAM_INTEGER(
          BLOCK_PP_PARAMS, KEY_SUPPORT_STRIPE_DIS,
          tb_cfg->pp_params.vert_down_scale_stripe_disable_support);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_MAX_PP_OUT_PIC_WIDTH,
                              tb_cfg->pp_params.max_pp_out_pic_width);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_TILED_REF,
                              tb_cfg->pp_params.tiled_ref_support);

      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_FAST_HOR_D_SCALE_DIS,
                              tb_cfg->pp_params.fast_hor_down_scale_disable);
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_FAST_VER_D_SCALE_DIS,
                              tb_cfg->pp_params.fast_ver_down_scale_disable);
      /*        IMPLEMENT_PARAM_INTEGER( BLOCK_PP_PARAMS,
       * KEY_D_SCALE_STRIPES_DIS,
       * tb_cfg->pp_params.ver_downscale_stripes_disable );*/
      IMPLEMENT_PARAM_INTEGER(BLOCK_PP_PARAMS, KEY_SUPPORT_PIX_ACC_OUT,
                              tb_cfg->pp_params.pix_acc_out_support);
      break;
  }
  return TB_CFG_OK;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBPrintCfg

        Functional description:
          Prints the cofiguration to stdout.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void TBPrintCfg(const struct TBCfg* tb_cfg) {
  /* TbParams */
  printf("tb_cfg->tb_params.packet_by_packet: %s\n",
         tb_cfg->tb_params.packet_by_packet);
  printf("tb_cfg->tb_params.nal_unit_stream: %s\n",
         tb_cfg->tb_params.nal_unit_stream);
  printf("tb_cfg->tb_params.seed_rnd: %d\n", tb_cfg->tb_params.seed_rnd);
  printf("tb_cfg->tb_params.stream_bit_swap: %s\n",
         tb_cfg->tb_params.stream_bit_swap);
  printf("tb_cfg->tb_params.stream_bit_loss: %s\n",
         tb_cfg->tb_params.stream_bit_loss);
  printf("tb_cfg->tb_params.stream_packet_loss: %s\n",
         tb_cfg->tb_params.stream_packet_loss);
  printf("tb_cfg->tb_params.stream_header_corrupt: %s\n",
         tb_cfg->tb_params.stream_header_corrupt);
  printf("tb_cfg->tb_params.stream_truncate: %s\n",
         tb_cfg->tb_params.stream_truncate);
  printf("tb_cfg->tb_params.slice_ud_in_packet: %s\n",
         tb_cfg->tb_params.slice_ud_in_packet);
  printf("tb_cfg->tb_params.first_trace_frame: %d\n",
         tb_cfg->tb_params.first_trace_frame);
  printf("tb_cfg->tb_params.extra_cu_ctrl_eof: %d\n",
         tb_cfg->tb_params.extra_cu_ctrl_eof);

  /* DecParams */
  printf("tb_cfg->dec_params.output_picture_endian: %s\n",
         tb_cfg->dec_params.output_picture_endian);
  printf("tb_cfg->dec_params.bus_burst_length: %d\n",
         tb_cfg->dec_params.bus_burst_length);
  printf("tb_cfg->dec_params.asic_service_priority: %d\n",
         tb_cfg->dec_params.asic_service_priority);
  printf("tb_cfg->dec_params.output_format: %s\n",
         tb_cfg->dec_params.output_format);
  printf("tb_cfg->dec_params.latency_compensation: %d\n",
         tb_cfg->dec_params.latency_compensation);
  printf("tb_cfg->dec_params.clk_gate_decoder: %d\n",
         tb_cfg->dec_params.clk_gate_decoder);
  printf("tb_cfg->dec_params.clk_gate_decoder_idle: %d\n",
         tb_cfg->dec_params.clk_gate_decoder_idle);
  printf("tb_cfg->dec_params.data_discard: %s\n",
         tb_cfg->dec_params.data_discard);
  printf("tb_cfg->dec_params.memory_allocation: %s\n",
         tb_cfg->dec_params.memory_allocation);
  printf("tb_cfg->dec_params.rlc_mode_forced: %s\n",
         tb_cfg->dec_params.rlc_mode_forced);
  printf("tb_cfg->dec_params.error_concealment: %s\n",
         tb_cfg->dec_params.error_concealment);
  printf("tb_cfg->dec_params.jpeg_mcus_slice: %d\n",
         tb_cfg->dec_params.jpeg_mcus_slice);
  printf("tb_cfg->dec_params.jpeg_input_buffer_size: %d\n",
         tb_cfg->dec_params.jpeg_input_buffer_size);

  /* PpParams */
  printf("tb_cfg->pp_params.output_picture_endian: %s\n",
         tb_cfg->pp_params.output_picture_endian);
  printf("tb_cfg->pp_params.input_picture_endian: %s\n",
         tb_cfg->pp_params.input_picture_endian);
  printf("tb_cfg->pp_params.word_swap: %s\n", tb_cfg->pp_params.word_swap);
  printf("tb_cfg->pp_params.word_swap16: %s\n", tb_cfg->pp_params.word_swap16);
  printf("tb_cfg->pp_params.multi_buffer: %s\n",
         tb_cfg->pp_params.multi_buffer);
  printf("tb_cfg->pp_params.bus_burst_length: %d\n",
         tb_cfg->pp_params.bus_burst_length);
  printf("tb_cfg->pp_params.clock_gating: %s\n",
         tb_cfg->pp_params.clock_gating);
  printf("tb_cfg->pp_params.data_discard: %s\n",
         tb_cfg->pp_params.data_discard);
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBSetDefaultCfg

        Functional description:
          Sets the default configuration.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void TBSetDefaultCfg(struct TBCfg* tb_cfg) {
  /* TbParams */
  strcpy(tb_cfg->tb_params.packet_by_packet, "DISABLED");
  strcpy(tb_cfg->tb_params.nal_unit_stream, "DISABLED");
  tb_cfg->tb_params.seed_rnd = 1;
  strcpy(tb_cfg->tb_params.stream_bit_swap, "0");
  strcpy(tb_cfg->tb_params.stream_bit_loss, "0");
  strcpy(tb_cfg->tb_params.stream_packet_loss, "0");
  strcpy(tb_cfg->tb_params.stream_header_corrupt, "DISABLED");
  strcpy(tb_cfg->tb_params.stream_truncate, "DISABLED");
  strcpy(tb_cfg->tb_params.slice_ud_in_packet, "DISABLED");
  tb_cfg->tb_params.memory_page_size = 1;
  tb_cfg->tb_params.ref_frm_buffer_size = -1;
  tb_cfg->tb_params.first_trace_frame = 0;
  tb_cfg->tb_params.extra_cu_ctrl_eof = 0;
  tb_cfg->dec_params.force_mpeg4_idct = 0;
  tb_cfg->dec_params.ref_buffer_test_mode_offset_enable = 0;
  tb_cfg->dec_params.ref_buffer_test_mode_offset_min = -256;
  tb_cfg->dec_params.ref_buffer_test_mode_offset_start = -256;
  tb_cfg->dec_params.ref_buffer_test_mode_offset_max = 255;
  tb_cfg->dec_params.ref_buffer_test_mode_offset_incr = 16;
  tb_cfg->dec_params.apf_threshold_disable = 1;
  tb_cfg->dec_params.apf_threshold_value = -1;
  tb_cfg->dec_params.apf_disable = 0;
  tb_cfg->dec_params.field_dpb_support = 0;
  tb_cfg->dec_params.service_merge_disable = 0;

/* DecParams */
#if (DEC_X170_OUTPUT_PICTURE_ENDIAN == DEC_X170_BIG_ENDIAN)
  strcpy(tb_cfg->dec_params.output_picture_endian, "BIG_ENDIAN");
#else
  strcpy(tb_cfg->dec_params.output_picture_endian, "LITTLE_ENDIAN");
#endif

  tb_cfg->dec_params.bus_burst_length = DEC_X170_BUS_BURST_LENGTH;
  tb_cfg->dec_params.asic_service_priority = DEC_X170_ASIC_SERVICE_PRIORITY;

#if (DEC_X170_OUTPUT_FORMAT == DEC_X170_OUTPUT_FORMAT_RASTER_SCAN)
  strcpy(tb_cfg->dec_params.output_format, "RASTER_SCAN");
#else
  strcpy(tb_cfg->dec_params.output_format, "TILED");
#endif

  tb_cfg->dec_params.latency_compensation = DEC_X170_LATENCY_COMPENSATION;

  tb_cfg->dec_params.clk_gate_decoder = DEC_X170_INTERNAL_CLOCK_GATING;
  tb_cfg->dec_params.clk_gate_decoder_idle =
      DEC_X170_INTERNAL_CLOCK_GATING_RUNTIME;

#if (DEC_X170_DATA_DISCARD_ENABLE == 0)
  strcpy(tb_cfg->dec_params.data_discard, "DISABLED");
#else
  strcpy(tb_cfg->dec_params.data_discard, "ENABLED");
#endif

  strcpy(tb_cfg->dec_params.memory_allocation, "INTERNAL");
  strcpy(tb_cfg->dec_params.rlc_mode_forced, "DISABLED");
  strcpy(tb_cfg->dec_params.error_concealment, "PICTURE_FREEZE");
  tb_cfg->dec_params.stride_support = 0;
  tb_cfg->dec_params.jpeg_mcus_slice = 0;
  tb_cfg->dec_params.jpeg_input_buffer_size = 0;
  tb_cfg->dec_params.ch8_pix_ileav_output = 0;

  tb_cfg->dec_params.tiled_ref_support = tb_cfg->pp_params.tiled_ref_support =
      0;

  tb_cfg->dec_params.refbu_enable = 0;
  tb_cfg->dec_params.refbu_disable_interlaced = 1;
  tb_cfg->dec_params.refbu_disable_double = 1;
  tb_cfg->dec_params.refbu_disable_eval_mode = 1;
  tb_cfg->dec_params.refbu_disable_checkpoint = 1;
  tb_cfg->dec_params.refbu_disable_offset = 1;
  tb_cfg->dec_params.refbu_disable_top_bot_sum = 1;
#ifdef DEC_X170_REFBU_ADJUST_VALUE
  tb_cfg->dec_params.refbu_data_excess_max_pct = DEC_X170_REFBU_ADJUST_VALUE;
#else
  tb_cfg->dec_params.refbu_data_excess_max_pct = 130;
#endif

  tb_cfg->dec_params.mpeg2_support = 1;
  tb_cfg->dec_params.vc1_support = 3; /* Adv profile */
  tb_cfg->dec_params.jpeg_support = 1;
  tb_cfg->dec_params.mpeg4_support = 2; /* ASP */
  tb_cfg->dec_params.h264_support = 3;  /* High */
  tb_cfg->dec_params.vp6_support = 1;
  tb_cfg->dec_params.vp7_support = 1;
  tb_cfg->dec_params.vp8_support = 1;
  tb_cfg->dec_params.prog_jpeg_support = 1;
  tb_cfg->dec_params.sorenson_support = 1;
  tb_cfg->dec_params.custom_mpeg4_support = 1; /* custom feature 1 */
  tb_cfg->dec_params.avs_support = 1;
  tb_cfg->dec_params.rv_support = 1;
  tb_cfg->dec_params.mvc_support = 1;
  tb_cfg->dec_params.webp_support = 1;
  tb_cfg->dec_params.ec_support = 0;
  tb_cfg->dec_params.jpeg_esupport = 1;
  tb_cfg->dec_params.support_non_compliant = 1;
  tb_cfg->dec_params.max_dec_pic_width = 4096;
  tb_cfg->dec_params.max_dec_pic_height = 2304;
  tb_cfg->dec_params.hw_version = 10001;
  tb_cfg->dec_params.hw_build = 1000;

  tb_cfg->dec_params.bus_width = DEC_X170_BUS_WIDTH;
  tb_cfg->dec_params.latency = DEC_X170_REFBU_LATENCY;
  tb_cfg->dec_params.non_seq_clk = DEC_X170_REFBU_NONSEQ;
  tb_cfg->dec_params.seq_clk = DEC_X170_REFBU_SEQ;

  tb_cfg->dec_params.strm_swap = HANTRODEC_STREAM_SWAP;
  tb_cfg->dec_params.pic_swap = HANTRODEC_STREAM_SWAP;
  tb_cfg->dec_params.dirmv_swap = HANTRODEC_STREAM_SWAP;
  tb_cfg->dec_params.tab0_swap = HANTRODEC_STREAM_SWAP;
  tb_cfg->dec_params.tab1_swap = HANTRODEC_STREAM_SWAP;
  tb_cfg->dec_params.tab2_swap = HANTRODEC_STREAM_SWAP;
  tb_cfg->dec_params.tab3_swap = HANTRODEC_STREAM_SWAP;
  tb_cfg->dec_params.rscan_swap = HANTRODEC_STREAM_SWAP;
  tb_cfg->dec_params.max_burst = HANTRODEC_MAX_BURST;
  tb_cfg->dec_params.ref_double_buffer_enable =
      HANTRODEC_INTERNAL_DOUBLE_REF_BUFFER;
  /* PpParams */
  strcpy(tb_cfg->pp_params.output_picture_endian, "PP_CFG");
  strcpy(tb_cfg->pp_params.input_picture_endian, "PP_CFG");
  strcpy(tb_cfg->pp_params.word_swap, "PP_CFG");
  strcpy(tb_cfg->pp_params.word_swap16, "PP_CFG");
  tb_cfg->pp_params.bus_burst_length = PP_X170_BUS_BURST_LENGTH;

  strcpy(tb_cfg->pp_params.multi_buffer, "DISABLED");

#if (PP_X170_INTERNAL_CLOCK_GATING == 0)
  strcpy(tb_cfg->pp_params.clock_gating, "DISABLED");
#else
  strcpy(tb_cfg->pp_params.clock_gating, "ENABLED");
#endif

#if (DEC_X170_DATA_DISCARD_ENABLE == 0)
  strcpy(tb_cfg->pp_params.data_discard, "DISABLED");
#else
  strcpy(tb_cfg->params.data_discard, "ENABLED");
#endif

  tb_cfg->pp_params.ppd_exists = 1;
  tb_cfg->pp_params.dithering_support = 1;
  tb_cfg->pp_params.scaling_support = 1; /* Lo/Hi performance? */
  tb_cfg->pp_params.deinterlacing_support = 1;
  tb_cfg->pp_params.alpha_blending_support = 1;
  tb_cfg->pp_params.ablend_crop_support = 0;
  tb_cfg->pp_params.pp_out_endian_support = 1;
  tb_cfg->pp_params.tiled_support = 1;
  tb_cfg->pp_params.max_pp_out_pic_width = 4096;

  tb_cfg->pp_params.fast_hor_down_scale_disable = 0;
  tb_cfg->pp_params.fast_ver_down_scale_disable = 0;
  /*    tb_cfg->pp_params.ver_downscale_stripes_disable = 0;*/

  tb_cfg->pp_params.pix_acc_out_support = 1;
  tb_cfg->pp_params.vert_down_scale_stripe_disable_support = 0;
}

u32 TBCheckCfg(const struct TBCfg* tb_cfg) {
  /* TbParams */
  /*if (tb_cfg->tb_params.max_pics)
  {
  }*/

  if (strcmp(tb_cfg->tb_params.packet_by_packet, "ENABLED") &&
      strcmp(tb_cfg->tb_params.packet_by_packet, "DISABLED")) {
    printf("Error in TbParams.PacketByPacket: %s\n",
           tb_cfg->tb_params.packet_by_packet);
    return 1;
  }

  if (strcmp(tb_cfg->tb_params.nal_unit_stream, "ENABLED") &&
      strcmp(tb_cfg->tb_params.nal_unit_stream, "DISABLED")) {
    printf("Error in TbParams.NalUnitStream: %s\n",
           tb_cfg->tb_params.nal_unit_stream);
    return 1;
  }

  /*if (strcmp(tb_cfg->tb_params.stream_bit_swap, "0") == 0 &&
          strcmp(tb_cfg->tb_params.stream_header_corrupt, "ENABLED") == 0)
  {
      printf("Stream header corrupt requires enabled stream bit swap (see test
  bench configuration)\n");
      return 1;
  }*/

  /*if (strcmp(tb_cfg->tb_params.stream_packet_loss, "0") &&
          strcmp(tb_cfg->tb_params.packet_by_packet, "DISABLED") == 0)
  {
      printf("Stream packet loss requires enabled packet by packet mode (see
  test bench configuration)\n");
      return 1;
  }*/

  if (strcmp(tb_cfg->tb_params.stream_header_corrupt, "ENABLED") &&
      strcmp(tb_cfg->tb_params.stream_header_corrupt, "DISABLED")) {
    printf("Error in TbParams.StreamHeaderCorrupt: %s\n",
           tb_cfg->tb_params.stream_header_corrupt);
    return 1;
  }

  if (strcmp(tb_cfg->tb_params.stream_truncate, "ENABLED") &&
      strcmp(tb_cfg->tb_params.stream_truncate, "DISABLED")) {
    printf("Error in TbParams.StreamTruncate: %s\n",
           tb_cfg->tb_params.stream_truncate);
    return 1;
  }

  if (strcmp(tb_cfg->tb_params.slice_ud_in_packet, "ENABLED") &&
      strcmp(tb_cfg->tb_params.slice_ud_in_packet, "DISABLED")) {
    printf("Error in TbParams.stream_truncate: %s\n",
           tb_cfg->tb_params.slice_ud_in_packet);
    return 1;
  }

  /* DecParams */
  if (strcmp(tb_cfg->dec_params.output_picture_endian, "LITTLE_ENDIAN") &&
      strcmp(tb_cfg->dec_params.output_picture_endian, "BIG_ENDIAN")) {
    printf("Error in DecParams.OutputPictureEndian: %s\n",
           tb_cfg->dec_params.output_picture_endian);
    return 1;
  }

  if (tb_cfg->dec_params.bus_burst_length > 31) {
    printf("Error in DecParams.BusBurstLength: %d\n",
           tb_cfg->dec_params.bus_burst_length);
    return 1;
  }

  if (tb_cfg->dec_params.asic_service_priority > 4) {
    printf("Error in DecParams.AsicServicePriority: %d\n",
           tb_cfg->dec_params.asic_service_priority);
    return 1;
  }

  if (strcmp(tb_cfg->dec_params.output_format, "RASTER_SCAN") &&
      strcmp(tb_cfg->dec_params.output_format, "TILED")) {
    printf("Error in DecParams.OutputFormat: %s\n",
           tb_cfg->dec_params.output_format);
    return 1;
  }

  if (tb_cfg->dec_params.latency_compensation > 63 ||
      tb_cfg->dec_params.latency_compensation < 0) {
    printf("Error in DecParams.LatencyCompensation: %d\n",
           tb_cfg->dec_params.latency_compensation);
    return 1;
  }

  if (tb_cfg->dec_params.clk_gate_decoder > 1) {
    printf("Error in DecParams.clk_gate_decoder: %d\n",
           tb_cfg->dec_params.clk_gate_decoder);
    return 1;
  }
  if (tb_cfg->dec_params.clk_gate_decoder_idle > 1) {
    printf("Error in DecParams.clk_gate_decoder_idle: %d\n",
           tb_cfg->dec_params.clk_gate_decoder_idle);
    return 1;
  }
  if (tb_cfg->dec_params.clk_gate_decoder_idle &&
      !tb_cfg->dec_params.clk_gate_decoder) {
    printf("Error in DecParams.clk_gate_decoder_idle: %d\n",
           tb_cfg->dec_params.clk_gate_decoder_idle);
    return 1;
  }

  if (strcmp(tb_cfg->dec_params.data_discard, "ENABLED") &&
      strcmp(tb_cfg->dec_params.data_discard, "DISABLED")) {
    printf("Error in DecParams.DataDiscard: %s\n",
           tb_cfg->dec_params.data_discard);
    return 1;
  }

  if (strcmp(tb_cfg->dec_params.memory_allocation, "INTERNAL") &&
      strcmp(tb_cfg->dec_params.memory_allocation, "EXTERNAL")) {
    printf("Error in DecParams.MemoryAllocation: %s\n",
           tb_cfg->dec_params.memory_allocation);
    return 1;
  }

  if (strcmp(tb_cfg->dec_params.rlc_mode_forced, "DISABLED") &&
      strcmp(tb_cfg->dec_params.rlc_mode_forced, "ENABLED")) {
    printf("Error in DecParams.RlcModeForced: %s\n",
           tb_cfg->dec_params.rlc_mode_forced);
    return 1;
  }

  /*if (strcmp(tb_cfg->dec_params.rlc_mode_forced, "ENABLED") == 0 &&
          strcmp(tb_cfg->dec_params.error_concealment, "PICTURE_FREEZE") == 0)
  {
      printf("MACRO_BLOCK DecParams.ErrorConcealment must be enabled if RLC
  coding\n");
          return 1;
  }*/

  /*
    if (strcmp(tb_cfg->dec_params.rlc_mode_forced, "ENABLED") == 0 &&
            (strcmp(tb_cfg->tb_params.packet_by_packet, "ENABLED") == 0 ||
	        strcmp(tb_cfg->tb_params.nal_unit_stream, "ENABLED") == 0))
    {
        printf("TbParams.PacketByPacket and TbParams.NalUnitStream must not be enabled if RLC coding\n");
	    return 1;
    }
    */ /* why is that above? */

  if (strcmp(tb_cfg->tb_params.nal_unit_stream, "ENABLED") == 0 &&
      strcmp(tb_cfg->tb_params.packet_by_packet, "DISABLED") == 0) {
    printf(
        "TbParams.PacketByPacket must be enabled if NAL unit stream is used\n");
    return 1;
  }

  if (strcmp(tb_cfg->tb_params.slice_ud_in_packet, "ENABLED") == 0 &&
      strcmp(tb_cfg->tb_params.packet_by_packet, "DISABLED") == 0) {
    printf(
        "TbParams.PacketByPacket must be enabled if slice user data is "
        "included in packet\n");
    return 1;
  }

  /*if (strcmp(tb_cfg->dec_params.error_concealment, "MACRO_BLOCK") &&
      strcmp(tb_cfg->dec_params.error_concealment, "PICTURE_FREEZE"))
  {
      printf("Error in DecParams.ErrorConcealment: %s\n",
  tb_cfg->dec_params.error_concealment);
          return 1;
  }*/

  /*if (tb_cfg->dec_params.mcus_slice)
  {
  }*/

  if (tb_cfg->dec_params.jpeg_input_buffer_size != 0 &&
      ((tb_cfg->dec_params.jpeg_input_buffer_size > 0 &&
        tb_cfg->dec_params.jpeg_input_buffer_size < 5120) ||
       tb_cfg->dec_params.jpeg_input_buffer_size > 16776960 ||
       tb_cfg->dec_params.jpeg_input_buffer_size % 256 != 0)) {
    printf("Error in DecParams.input_buffer_size: %d\n",
           tb_cfg->dec_params.jpeg_input_buffer_size);
    return 1;
  }

  /* PpParams */
  if (strcmp(tb_cfg->pp_params.output_picture_endian, "LITTLE_ENDIAN") &&
      strcmp(tb_cfg->pp_params.output_picture_endian, "BIG_ENDIAN") &&
      strcmp(tb_cfg->pp_params.output_picture_endian, "PP_CFG")) {
    printf("Error in PpParams.OutputPictureEndian: %s\n",
           tb_cfg->pp_params.output_picture_endian);
    return 1;
  }

  if (strcmp(tb_cfg->pp_params.input_picture_endian, "LITTLE_ENDIAN") &&
      strcmp(tb_cfg->pp_params.input_picture_endian, "BIG_ENDIAN") &&
      strcmp(tb_cfg->pp_params.input_picture_endian, "PP_CFG")) {
    printf("Error in PpParams.InputPictureEndian: %s\n",
           tb_cfg->pp_params.input_picture_endian);
    return 1;
  }

  if (strcmp(tb_cfg->pp_params.word_swap, "ENABLED") &&
      strcmp(tb_cfg->pp_params.word_swap, "DISABLED") &&
      strcmp(tb_cfg->pp_params.word_swap, "PP_CFG")) {
    printf("Error in PpParams.WordSwap: %s\n", tb_cfg->pp_params.word_swap);
    return 1;
  }

  if (strcmp(tb_cfg->pp_params.word_swap16, "ENABLED") &&
      strcmp(tb_cfg->pp_params.word_swap16, "DISABLED") &&
      strcmp(tb_cfg->pp_params.word_swap16, "PP_CFG")) {
    printf("Error in PpParams.WordSwap16: %s\n", tb_cfg->pp_params.word_swap16);
    return 1;
  }

  if (tb_cfg->pp_params.bus_burst_length > 31) {
    printf("Error in PpParams.BusBurstLength: %d\n",
           tb_cfg->pp_params.bus_burst_length);
    return 1;
  }

  if (strcmp(tb_cfg->pp_params.clock_gating, "ENABLED") &&
      strcmp(tb_cfg->pp_params.clock_gating, "DISABLED")) {
    printf("Error in PpParams.ClockGating: %s\n",
           tb_cfg->pp_params.clock_gating);
    return 1;
  }

  if (strcmp(tb_cfg->pp_params.data_discard, "ENABLED") &&
      strcmp(tb_cfg->pp_params.data_discard, "DISABLED")) {
    printf("Error in PpParams.DataDiscard: %s\n",
           tb_cfg->pp_params.data_discard);
    return 1;
  }

  return 0;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetPPDataDiscard

        Functional description:
          Gets the integer values of PP data disgard.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetPPDataDiscard(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->pp_params.data_discard, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->pp_params.data_discard, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetPPClockGating

        Functional description:
          Gets the integer values of PP clock gating.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetPPClockGating(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->pp_params.clock_gating, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->pp_params.clock_gating, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetPPWordSwap

        Functional description:
          Gets the integer values of PP word swap.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetPPWordSwap(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->pp_params.word_swap, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->pp_params.word_swap, "DISABLED") == 0) {
    return 0;
  } else if (strcmp(tb_cfg->pp_params.word_swap, "PP_CFG") == 0) {
    return 2;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetPPWordSwap

        Functional description:
          Gets the integer values of PP word swap.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetPPWordSwap16(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->pp_params.word_swap16, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->pp_params.word_swap16, "DISABLED") == 0) {
    return 0;
  } else if (strcmp(tb_cfg->pp_params.word_swap16, "PP_CFG") == 0) {
    return 2;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetPPInputPictureEndian

        Functional description:
          Gets the integer values of PP input picture endian.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetPPInputPictureEndian(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->pp_params.input_picture_endian, "BIG_ENDIAN") == 0) {
    return PP_X170_PICTURE_BIG_ENDIAN;
  } else if (strcmp(tb_cfg->pp_params.input_picture_endian, "LITTLE_ENDIAN") ==
             0) {
    return PP_X170_PICTURE_LITTLE_ENDIAN;
  } else if (strcmp(tb_cfg->pp_params.input_picture_endian, "PP_CFG") == 0) {
    return 2;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetPPOutputPictureEndian

        Functional description:
          Gets the integer values of PP out picture endian.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetPPOutputPictureEndian(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->pp_params.output_picture_endian, "BIG_ENDIAN") == 0) {
    return PP_X170_PICTURE_BIG_ENDIAN;
  } else if (strcmp(tb_cfg->pp_params.output_picture_endian, "LITTLE_ENDIAN") ==
             0) {
    return PP_X170_PICTURE_LITTLE_ENDIAN;
  } else if (strcmp(tb_cfg->pp_params.output_picture_endian, "PP_CFG") == 0) {
    return 2;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecErrorConcealment

        Functional description:
          Gets the integer values of decoder error concealment.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecErrorConcealment(const struct TBCfg* tb_cfg) {
  /*
  if (strcmp(tb_cfg->dec_params.error_concealment, "MACRO_BLOCK") == 0)
  {
      return 1;
  }
  else*/
  if (strcmp(tb_cfg->dec_params.error_concealment, "PICTURE_FREEZE") == 0)
    return 0;
  else if (strcmp(tb_cfg->dec_params.error_concealment, "INTRA_FREEZE") == 0)
    return 1;
  else if (strcmp(tb_cfg->dec_params.error_concealment, "PARTIAL_FREEZE") == 0)
    return 2;
  else if (strcmp(tb_cfg->dec_params.error_concealment, "PARTIAL_IGNORE") == 0)
    return 3;
  else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecRlcModeForced

        Functional description:
          Gets the integer values of decoder rlc mode forced.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecRlcModeForced(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->dec_params.rlc_mode_forced, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->dec_params.rlc_mode_forced, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecMemoryAllocation

        Functional description:
          Gets the integer values of decoder memory allocation.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecMemoryAllocation(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->dec_params.memory_allocation, "INTERNAL") == 0) {
    return 0;
  } else if (strcmp(tb_cfg->dec_params.memory_allocation, "EXTERNAL") == 0) {
    return 1;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecDataDiscard

        Functional description:
          Gets the integer values of decoder data disgard.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecDataDiscard(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->dec_params.data_discard, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->dec_params.data_discard, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecClockGating

        Functional description:
          Gets the integer values of decoder clock gating.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecClockGating(const struct TBCfg* tb_cfg) {
  return tb_cfg->dec_params.clk_gate_decoder ? 1 : 0;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecClockGatingRuntime

        Functional description:
          Gets the integer values of decoder runtime clock gating.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecClockGatingRuntime(const struct TBCfg* tb_cfg) {
  return tb_cfg->dec_params.clk_gate_decoder_idle ? 1 : 0;
}
/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecOutputFormat

        Functional description:
          Gets the integer values of decoder output format.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecOutputFormat(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->dec_params.output_format, "RASTER_SCAN") == 0) {
    return DEC_X170_OUTPUT_FORMAT_RASTER_SCAN;
  } else if (strcmp(tb_cfg->dec_params.output_format, "TILED") == 0) {
    return DEC_X170_OUTPUT_FORMAT_TILED;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecOutputPictureEndian

        Functional description:
          Gets the integer values of decoder output format.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecOutputPictureEndian(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->dec_params.output_picture_endian, "BIG_ENDIAN") == 0) {
    return DEC_X170_BIG_ENDIAN;
  } else if (strcmp(tb_cfg->dec_params.output_picture_endian,
                    "LITTLE_ENDIAN") == 0) {
    return DEC_X170_LITTLE_ENDIAN;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetTBPacketByPacket

        Functional description:
          Gets the integer values of TB packet by packet.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetTBPacketByPacket(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->tb_params.packet_by_packet, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->tb_params.packet_by_packet, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetTBNalUnitStream

        Functional description:
          Gets the integer values of TB NALU unit stream.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetTBNalUnitStream(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->tb_params.nal_unit_stream, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->tb_params.nal_unit_stream, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetTBStreamHeaderCorrupt

        Functional description:
          Gets the integer values of TB header corrupt.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetTBStreamHeaderCorrupt(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->tb_params.stream_header_corrupt, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->tb_params.stream_header_corrupt, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetTBStreamTruncate

        Functional description:
          Gets the integer values of TB stream truncate.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetTBStreamTruncate(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->tb_params.stream_truncate, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->tb_params.stream_truncate, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetTBStreamTruncate

        Functional description:
          Gets the integer values of TB stream truncate.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetTBSliceUdInPacket(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->tb_params.slice_ud_in_packet, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->tb_params.slice_ud_in_packet, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetTBmultiBuffer

        Functional description:


        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetTBMultiBuffer(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->pp_params.multi_buffer, "ENABLED") == 0) {
    return 1;
  } else if (strcmp(tb_cfg->pp_params.multi_buffer, "DISABLED") == 0) {
    return 0;
  } else {
    ASSERT(0);
    return -1;
  }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecRefbuEvalMode

        Functional description:
          Gets the integer values of TB disable reference buffer eval mode

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecRefbuEvalMode(const struct TBCfg* tb_cfg) {
  return !tb_cfg->dec_params.refbu_disable_eval_mode;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecRefbuCheckpoint

        Functional description:
          Gets the integer values of TB disable reference buffer eval mode

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecRefbuCheckpoint(const struct TBCfg* tb_cfg) {
  return !tb_cfg->dec_params.refbu_disable_checkpoint;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecRefbuOffset

        Functional description:
          Gets the integer values of TB disable reference buffer eval mode

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecRefbuOffset(const struct TBCfg* tb_cfg) {
  return !tb_cfg->dec_params.refbu_disable_offset;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecRefbuEnabled

        Functional description:
          Gets the integer values of TB disable reference buffer eval mode

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecRefbuEnabled(const struct TBCfg* tb_cfg) {
  return tb_cfg->dec_params.refbu_enable;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecBusWidth

        Functional description:
          Gets the integer values of TB disable reference buffer eval mode

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecBusWidth(const struct TBCfg* tb_cfg) {
  return tb_cfg->dec_params.bus_width;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecSupportNonCompliant

        Functional description:

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecSupportNonCompliant(const struct TBCfg* tb_cfg) {
  return tb_cfg->dec_params.support_non_compliant;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetDecRefbuEnabled

        Functional description:
          Gets the integer values of TB disable reference buffer eval mode

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void TBGetHwConfig(const struct TBCfg* tb_cfg, DWLHwConfig* hw_cfg) {

  u32 tmp;

  hw_cfg->max_dec_pic_width = tb_cfg->dec_params.max_dec_pic_width;
  hw_cfg->max_dec_pic_height = tb_cfg->dec_params.max_dec_pic_height;
  hw_cfg->max_pp_out_pic_width = tb_cfg->pp_params.max_pp_out_pic_width;

  hw_cfg->h264_support = tb_cfg->dec_params.h264_support;
  if (tb_cfg->dec_params.hw_version < 8190)
    hw_cfg->h264_support = hw_cfg->h264_support ? 1 : 0;
  hw_cfg->jpeg_support = tb_cfg->dec_params.jpeg_support;
  hw_cfg->mpeg4_support = tb_cfg->dec_params.mpeg4_support;
  hw_cfg->mpeg2_support = tb_cfg->dec_params.mpeg2_support;
  hw_cfg->vc1_support = tb_cfg->dec_params.vc1_support;
  hw_cfg->vp6_support = tb_cfg->dec_params.vp6_support;
  hw_cfg->vp7_support = tb_cfg->dec_params.vp7_support;
  hw_cfg->vp8_support = tb_cfg->dec_params.vp8_support;

  hw_cfg->custom_mpeg4_support = tb_cfg->dec_params.custom_mpeg4_support;
  hw_cfg->pp_support = tb_cfg->pp_params.ppd_exists;
  tmp = 0;
  if (tb_cfg->pp_params.dithering_support) tmp |= PP_DITHERING;
  if (tb_cfg->pp_params.tiled_support) tmp |= PP_TILED_4X4;
  if (tb_cfg->pp_params.scaling_support) {
    u32 scaling_bits;
    scaling_bits = tb_cfg->pp_params.scaling_support & 0x3;
    scaling_bits <<= 26;
    tmp |= scaling_bits; /* PP_SCALING */
  }
  if (tb_cfg->pp_params.deinterlacing_support) tmp |= PP_DEINTERLACING;
  if (tb_cfg->pp_params.alpha_blending_support) tmp |= PP_ALPHA_BLENDING;
  if (tb_cfg->pp_params.pp_out_endian_support) tmp |= PP_OUTP_ENDIAN;
  if (tb_cfg->pp_params.pix_acc_out_support) tmp |= PP_PIX_ACC_OUTPUT;
  if (tb_cfg->pp_params.ablend_crop_support) tmp |= PP_ABLEND_CROP;
  if (tb_cfg->pp_params.tiled_ref_support) {
    u32 tiled_bits;
    tiled_bits = tb_cfg->pp_params.tiled_ref_support & 0x3;
    tiled_bits <<= 14;
    tmp |= tiled_bits; /* PP_TILED_INPUT */
  }

  hw_cfg->pp_config = tmp;
  hw_cfg->sorenson_spark_support = tb_cfg->dec_params.sorenson_support;
  hw_cfg->ref_buf_support =
      (tb_cfg->dec_params.refbu_enable ? REF_BUF_SUPPORTED : 0) |
      ((!tb_cfg->dec_params.refbu_disable_interlaced) ? REF_BUF_INTERLACED
                                                      : 0) |
      ((!tb_cfg->dec_params.refbu_disable_double) ? REF_BUF_DOUBLE : 0);
  hw_cfg->tiled_mode_support = tb_cfg->dec_params.tiled_ref_support;
  hw_cfg->field_dpb_support = tb_cfg->dec_params.field_dpb_support;
  hw_cfg->stride_support = tb_cfg->dec_params.stride_support;

#ifdef DEC_X170_APF_DISABLE
  if (DEC_X170_APF_DISABLE) {
    hw_cfg->tiled_mode_support = 0;
  }
#endif /* DEC_X170_APF_DISABLE */

  if (!tb_cfg->dec_params.refbu_disable_offset) /* enable support for G1 */
  {
    hw_cfg->ref_buf_support |= 8 /* offset */;
  }
  hw_cfg->vp6_support = tb_cfg->dec_params.vp6_support;
  hw_cfg->avs_support = tb_cfg->dec_params.avs_support;
  if (tb_cfg->dec_params.hw_version < 9170)
    hw_cfg->rv_support = 0;
  else
    hw_cfg->rv_support = tb_cfg->dec_params.rv_support;
  hw_cfg->jpeg_esupport = tb_cfg->dec_params.jpeg_esupport;
  if (tb_cfg->dec_params.hw_version < 10000)
    hw_cfg->mvc_support = 0;
  else
    hw_cfg->mvc_support = tb_cfg->dec_params.mvc_support;

  if (tb_cfg->dec_params.hw_version < 10000)
    hw_cfg->webp_support = 0;
  else
    hw_cfg->webp_support = tb_cfg->dec_params.webp_support;

  if (tb_cfg->dec_params.hw_version < 10000)
    hw_cfg->ec_support = 0;
  else
    hw_cfg->ec_support = tb_cfg->dec_params.ec_support;

  hw_cfg->double_buffer_support = tb_cfg->dec_params.ref_double_buffer_enable;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetIntraFreezeEnable

        Functional description:
          Override reference buffer memory model parameters

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecIntraFreezeEnable(const struct TBCfg* tb_cfg) {
  if (strcmp(tb_cfg->dec_params.error_concealment, "INTRA_FREEZE") == 0) {
    return 1;
  }
  return 0;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetIntraFreezeEnable

        Functional description:
          Override reference buffer memory model parameters

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecDoubleBufferSupported(const struct TBCfg* tb_cfg) {
  return !tb_cfg->dec_params.refbu_disable_double;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetIntraFreezeEnable

        Functional description:
          Override reference buffer memory model parameters

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecTopBotSumSupported(const struct TBCfg* tb_cfg) {
  return !tb_cfg->dec_params.refbu_disable_top_bot_sum;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetTBFirstTraceFrame

        Functional description:

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetTBFirstTraceFrame(const struct TBCfg* tb_cfg) {
  return tb_cfg->tb_params.first_trace_frame;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBGetTBFirstTraceFrame

        Functional description:

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 TBGetDecForceMpeg4Idct(const struct TBCfg* tb_cfg) {
  return tb_cfg->dec_params.force_mpeg4_idct;
}

u32 TBGetDecCh8PixIleavOutput(const struct TBCfg* tb_cfg) {
  return tb_cfg->dec_params.ch8_pix_ileav_output;
}

u32 TBGetDecApfThresholdEnabled(const struct TBCfg* tb_cfg) {
  return !tb_cfg->dec_params.apf_threshold_disable;
}

u32 TBGetDecServiceMergeDisable(const struct TBCfg* tb_cfg) {
  return tb_cfg->dec_params.service_merge_disable;
}
