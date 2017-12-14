/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "hevcdecapi.h"
#include "dwl.h"

#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>

#ifdef ASIC_TRACE_SUPPORT
#include "trace.h"
#endif

#include "hevc_container.h"
#include "tb_cfg.h"
#include "tb_tiled.h"
#include "regdrv.h"

#ifdef PP_PIPELINE_ENABLED
#include "ppapi.h"
#include "pptestbench.h"
#endif

#ifdef MD5SUM
#include "tb_md5.h"
#endif

#include "tb_stream_corrupt.h"
#include "tb_sw_performance.h"

/* Debug prints */
#undef DEBUG_PRINT
#define DEBUG_PRINT(argv) printf argv

#define NUM_RETRY 100 /* how many retries after HW timeout */
#define ALIGNMENT_MASK 7
#define MAX_BUFFERS 16
#define BUFFER_ALIGN_MASK 0xF

void WriteOutput(const char *filename, const char *filename_tiled, u8 *data,
                 u32 pic_size, u32 pic_width, u32 pic_height, u32 frame_number,
                 u32 mono_chrome, u32 view, u32 tiled_mode,
                 const struct HevcCropParams *crop, u32 bit_depth_luma, u32 bit_depth_chroma, u32 pic_stride);
u32 NextPacket(u8 **strm);
u32 NextPacketFromFile(u8 **strm);
u32 CropPicture(u8 *out_image, u8 *in_image, u32 frame_width, u32 frame_height,
                const struct HevcCropParams *crop_params, u32 mono_chrome,
                u32 convert2_planar);
static void print_decode_return(i32 retval);

#ifdef PP_PIPELINE_ENABLED
static void HandlePpOutput(u32 pic_display_number, u32 view_id);
#endif

#include "md5.h"
u32 md5 = 0;
struct MD5Context md5_ctx;

u32 fill_buffer(u8 *stream);
/* Global variables for stream handling */
u8 *stream_stop = NULL;
u32 packetize = 0;
u32 nal_unit_stream = 0;
FILE *foutput = NULL, *foutput2 = NULL;
FILE *f_tiled_output = NULL;
FILE *fchroma2 = NULL;

FILE *findex = NULL;

/* stream start address */
u8 *byte_strm_start;
u32 trace_used_stream = 0;

/* SW/SW testing, read stream trace file */
FILE *f_stream_trace = NULL;

/* Pack each pixel in 16 bits based on pixel bit width. */
u32 output16 = 0;
FILE *f_output16 = NULL;

/* output file writing disable */
u32 disable_output_writing = 0;
u32 retry = 0;

u32 clock_gating = DEC_X170_INTERNAL_CLOCK_GATING;
u32 clock_gating_runtime = DEC_X170_INTERNAL_CLOCK_GATING_RUNTIME;
u32 data_discard = DEC_X170_DATA_DISCARD_ENABLE;
u32 latency_comp = DEC_X170_LATENCY_COMPENSATION;
u32 output_picture_endian = DEC_X170_OUTPUT_PICTURE_ENDIAN;
u32 bus_burst_length = DEC_X170_BUS_BURST_LENGTH;
u32 asic_service_priority = DEC_X170_ASIC_SERVICE_PRIORITY;
u32 output_format = DEC_X170_OUTPUT_FORMAT;
u32 service_merge_disable = DEC_X170_SERVICE_MERGE_DISABLE;
u32 bus_width = DEC_X170_BUS_WIDTH;

u32 strm_swap = HANTRODEC_STREAM_SWAP;
u32 pic_swap = HANTRODEC_STREAM_SWAP;
u32 dirmv_swap = HANTRODEC_STREAM_SWAP;
u32 tab0_swap = HANTRODEC_STREAM_SWAP;
u32 tab1_swap = HANTRODEC_STREAM_SWAP;
u32 tab2_swap = HANTRODEC_STREAM_SWAP;
u32 tab3_swap = HANTRODEC_STREAM_SWAP;
u32 rscan_swap = HANTRODEC_STREAM_SWAP;
u32 max_burst = HANTRODEC_MAX_BURST;
u32 double_ref_buffer = HANTRODEC_INTERNAL_DOUBLE_REF_BUFFER;
u32 timeout_cycles = HANTRODEC_TIMEOUT_OVERRIDE;

u32 stream_truncate = 0;
u32 stream_packet_loss = 0;
u32 stream_header_corrupt = 0;
u32 hdrs_rdy = 0;
u32 pic_rdy = 0;
struct TBCfg tb_cfg;

u32 long_stream = 0;
FILE *finput;
u32 planar_output = 0;
/*u32 tiled_output = 0;*/

u32 tiled_output = DEC_REF_FRM_RASTER_SCAN;
u32 dpb_mode = DEC_DPB_DEFAULT;
u32 convert_tiled_output = 0;

u32 use_peek_output = 0;
u32 skip_non_reference = 0;
u32 convert_to_frame_dpb = 0;

u32 write_raster_out = 0;

/* variables for indexing */
u32 save_index = 0;
u32 use_index = 0;
off64_t cur_index = 0;
off64_t next_index = 0;
/* indicate when we save index */
u8 save_flag = 0;

u32 b_frames;

u32 test_case_id = 0;

u8 *grey_chroma = NULL;
size_t grey_chroma_size = 0;

u8 *raster_scan = NULL;
size_t raster_scan_size = 0;

u8 *pic_big_endian = NULL;
size_t pic_big_endian_size = 0;

#ifdef ASIC_TRACE_SUPPORT
extern u32 hw_dec_pic_count;
u32 hevc_support;
#endif

#ifdef HEVC_EVALUATION
u32 hevc_support;
#endif

#ifdef ADS_PERFORMANCE_SIMULATION

volatile u32 tttttt = 0;

void Traceperf() { tttttt++; }

#undef START_SW_PERFORMANCE
#undef END_SW_PERFORMANCE

#define START_SW_PERFORMANCE Traceperf();
#define END_SW_PERFORMANCE Traceperf();

#endif

HevcDecInst dec_inst;

const char *out_file_name = "out.yuv";
const char *out_file_name_tiled = "out_tiled.yuv";
const char *out_file_name_planar = "out_planar.yuv";
const char *out_file_name_16 = "out16.yuv";

u32 crop_display = 0;
u32 pic_size = 0;
u8 *cropped_pic = NULL;
size_t cropped_pic_size = 0;
struct HevcDecInfo dec_info;

pid_t main_pid;
pthread_t output_thread;
int output_thread_run = 0;
u32 num_errors = 0;

#ifdef TESTBENCH_WATCHDOG
static u32 old_pic_display_number = 0;

static void WatchdogCb(int signal_number) {
  if (pic_display_number == old_pic_display_number) {
    fprintf(stderr, "\n\n_testbench TIMEOUT\n");
    kill(main_pid, SIGTERM);
  } else {
    old_pic_display_number = pic_display_number;
  }
}
#endif

static void *HevcOutputThread(void *arg) {
  struct HevcDecPicture dec_picture;
  u32 pic_display_number = 1;

#ifdef TESTBENCH_WATCHDOG
  /* fpga watchdog: 30 sec timer for frozen decoder */
  {
    struct itimerval t = {{30, 0}, {30, 0}};
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = WatchdogCb;
    sa.sa_flags |= SA_RESTART; /* restart of system calls */
    sigaction(SIGALRM, &sa, NULL);

    setitimer(ITIMER_REAL, &t, NULL);
  }
#endif

  while (output_thread_run) {
    enum DecRet ret;

    ret = HevcDecNextPicture(dec_inst, &dec_picture);

    if (ret == DEC_PIC_RDY) {
      DEBUG_PRINT(("PIC %d/%d, type %s", pic_display_number, dec_picture.pic_id,
                   dec_picture.is_idr_picture ? "    IDR" : "NON-IDR"));

      DEBUG_PRINT((", %d x %d, crop: (%d, %d), %d x %d %s\n",
                   dec_picture.pic_width, dec_picture.pic_height,
                   dec_picture.crop_params.crop_left_offset,
                   dec_picture.crop_params.crop_top_offset,
                   dec_picture.crop_params.crop_out_width,
                   dec_picture.crop_params.crop_out_height,
                   dec_picture.pic_corrupt ? "CORRUPT" : ""));

      fflush(stdout);

      /* count erroneous pictures */
      if (dec_picture.pic_corrupt) num_errors++;

      /* Write output picture to file */
      WriteOutput(out_file_name, out_file_name_tiled,
                  (u8 *)dec_picture.output_picture, pic_size,
                  dec_picture.pic_width, dec_picture.pic_height,
                  pic_display_number - 1, dec_info.mono_chrome, 0,
                  dec_picture.output_format, &dec_picture.crop_params,
                  dec_picture.bit_depth_luma, dec_picture.bit_depth_chroma, dec_picture.pic_stride);

      HevcDecPictureConsumed(dec_inst, &dec_picture);

      // usleep(10000); /* 10ms  sleep */

      pic_display_number++;
    } else if (ret == DEC_END_OF_STREAM) {
      DEBUG_PRINT(("END-OF-STREAM received in output thread\n"));
      break;
    }
  }

  return NULL;
}

int main(int argc, char **argv) {

  u32 i, tmp;
  u32 max_num_pics = 0;
  long int strm_len;
  enum DecRet ret;
  struct HevcDecInput dec_input;
  struct HevcDecOutput dec_output;
  struct DWLLinearMem stream_mem;
#ifdef USE_EXTERNAL_BUFFER
  struct DWLLinearMem ext_buffers[MAX_BUFFERS];
  memset(ext_buffers, 0, sizeof(ext_buffers));
#endif

  u32 pic_decode_number = 1;
  u32 disable_output_reordering = 0;
  u32 use_extra_dpb_frms = 0;
  u32 rlc_mode = 0;
  u32 trace_target = 0;
  u32 mb_error_concealment = 0;
  u32 force_whole_stream = 0;
  const u8 *ptmpstream = NULL;
  u32 stream_will_end = 0;

#ifdef PP_PIPELINE_ENABLED
  PPApiVersion pp_ver;
  PPBuild pp_build;
#endif

  FILE *f_tbcfg;
  u32 seed_rnd = 0;
  u32 stream_bit_swap = 0;
  i32 corrupted_bytes = 0; /*  */
  u32 bus_width_in_bytes;
  u32 raster_stride = 0;

  const void *dwl_inst = NULL;
  struct DWLInitParam dwl_params = {DWL_CLIENT_TYPE_HEVC_DEC};

#ifdef ASIC_TRACE_SUPPORT
  hevc_support = 1;
#endif

#ifndef EXPIRY_DATE
#define EXPIRY_DATE (u32)0xFFFFFFFF
#endif /* EXPIRY_DATE */

  main_pid = getpid();

  /* expiry stuff */
  {
    char tm_buf[7];
    time_t sys_time;
    struct tm *tm;
    u32 tmp1;

    /* Check expiry date */
    time(&sys_time);
    tm = localtime(&sys_time);
    strftime(tm_buf, sizeof(tm_buf), "%y%m%d", tm);
    tmp1 = 1000000 + atoi(tm_buf);
    if (tmp1 > (EXPIRY_DATE) && (EXPIRY_DATE) > 1) {
      fprintf(stderr,
              "EVALUATION PERIOD EXPIRED.\n"
              "Please contact On2 Sales.\n");
      return -1;
    }
  }

  stream_mem.virtual_address = NULL;
  stream_mem.bus_address = 0;

  INIT_SW_PERFORMANCE;

  {
    HevcDecBuild dec_build;

    dec_build = HevcDecGetBuild();
    DEBUG_PRINT(("\n_hevc SW build: %d - HW build: %x\n\n", dec_build.sw_build,
                 dec_build.hw_build));
  }

#ifndef PP_PIPELINE_ENABLED
  /* Check that enough command line arguments given, if not -> print usage
   * information out */
  if (argc < 2) {
    DEBUG_PRINT(("Usage: %s [options] file.hevc\n", argv[0]));
    DEBUG_PRINT(("\t-Nn forces decoding to stop after n pictures\n"));
    DEBUG_PRINT(
        ("\t-Ooutfile write output to \"outfile\" (default "
         "out_wxxxhyyy.yuv)\n"));
    DEBUG_PRINT(("\t-X Disable output file writing\n"));
    DEBUG_PRINT(("\t-C display cropped image (default decoded image)\n"));
    DEBUG_PRINT(("\t-R disable DPB output reordering\n"));
    DEBUG_PRINT(("\t-J<n> use 'n' extra frames in DPB\n"));
    DEBUG_PRINT(("\t-Sfile.hex stream control trace file\n"));
    DEBUG_PRINT(
        ("\t-W disable packetizing even if stream does not fit to buffer.\n"
         "\t   Only the first 0x1FFFFF bytes of the stream are decoded.\n"));
    DEBUG_PRINT(("\t-E use tiled reference frame format.\n"));
    DEBUG_PRINT(("\t-G convert tiled output pictures to raster scan\n"));
    DEBUG_PRINT(("\t-L enable support for long streams\n"));
    DEBUG_PRINT(("\t-P write planar output\n"));
    DEBUG_PRINT(("\t-I save index file\n"));
    /*        DEBUG_PRINT(("\t-T write tiled output (out_tiled.yuv) by
     * converting raster scan output\n"));*/
    DEBUG_PRINT(("\t-Z output pictures using HevcDecPeek() function\n"));
    DEBUG_PRINT(("\t-r trace file format for RTL  (extra CU ctrl)\n"));
    DEBUG_PRINT(("\t--raster-out write additional raster scan output pic\n"));
    DEBUG_PRINT(
        ("\t--packet-by-packet parse stream and decode packet-by-packet\n"));
    DEBUG_PRINT(
        ("\t--md5 write md5 checksum for whole output (not frame-by-frame)\n"));
#ifdef ASIC_TRACE_SUPPORT
    DEBUG_PRINT(("\t-Q Skip decoding non-reference pictures.\n"));
    DEBUG_PRINT(("\t--output16 Output each pixel in 16 bits based on pixel bit width.\n"));
#endif
    return 0;
  }

  /* read command line arguments */
  for (i = 1; i < (u32)(argc - 1); i++) {
    if (strncmp(argv[i], "-N", 2) == 0) {
      max_num_pics = (u32)atoi(argv[i] + 2);
    } else if (strncmp(argv[i], "-O", 2) == 0) {
      out_file_name = argv[i] + 2;
    } else if (strcmp(argv[i], "-X") == 0) {
      disable_output_writing = 1;
    } else if (strcmp(argv[i], "-C") == 0) {
      crop_display = 1;
    } else if (strcmp(argv[i], "-E") == 0) {
      tiled_output = DEC_REF_FRM_TILED_DEFAULT;
      convert_tiled_output = 0;
    } else if (strncmp(argv[i], "-G", 2) == 0)
      convert_tiled_output = 1;
      else if (strcmp(argv[i], "-R") == 0) {
      disable_output_reordering = 1;
    } else if (strncmp(argv[i], "-J", 2) == 0) {
      use_extra_dpb_frms = (u32)atoi(argv[i] + 2);
    } else if (strncmp(argv[i], "-S", 2) == 0) {
      f_stream_trace = fopen((argv[i] + 2), "r");
    } else if (strcmp(argv[i], "-W") == 0) {
      force_whole_stream = 1;
    } else if (strcmp(argv[i], "-L") == 0) {
      long_stream = 1;
    } else if (strcmp(argv[i], "-P") == 0) {
      planar_output = 1;
    } else if (strcmp(argv[i], "-I") == 0) {
      save_index = 1;
    } else if (strcmp(argv[i], "-Q") == 0) {
      skip_non_reference = 1;
    } else if (strcmp(argv[i], "--separate-fields-in-dpb") == 0) {
      dpb_mode = DEC_DPB_INTERLACED_FIELD;
    } else if (strcmp(argv[i], "--output-frame-dpb") == 0) {
      convert_to_frame_dpb = 1;
    } else if (strcmp(argv[i], "-Z") == 0) {
      use_peek_output = 1;
    } else if (strcmp(argv[i], "-r") == 0) {
      trace_target = 1;
    } else if (strcmp(argv[i], "--raster-out") == 0 ||
               strcmp(argv[i], "-Ers") == 0) {
      write_raster_out = 1;
    } else if (strcmp(argv[i], "--packet-by-packet") == 0) {
      packetize = 1;
    } else if (strcmp(argv[i], "--md5") == 0) {
      md5 = 1;
    } else if (strcmp(argv[i], "--output16") == 0) {
      output16 = 1;
    } else {
      DEBUG_PRINT(("UNKNOWN PARAMETER: %s\n", argv[i]));
      return 1;
    }
  }

  if (md5) {
    MD5Init(&md5_ctx);
  }

  if (planar_output) {
    if (!write_raster_out) {
      DEBUG_PRINT(("-P can only be used when \"-Ers\" is enabled.\n"));
      return -1;
    }
  }

  /* open input file for reading, file name given by user. If file open
   * fails -> exit */
  finput = fopen(argv[argc - 1], "rb");
  if (finput == NULL) {
    DEBUG_PRINT(("UNABLE TO OPEN INPUT FILE\n"));
    return -1;
  }

#else
  /* Print API and build version numbers */
  pp_ver = PPGetAPIVersion();
  pp_build = PPGetBuild();

  /* Version */
  fprintf(stdout, "\n_x170 PP API v%d.%d - SW build: %d - HW build: %x\n\n",
          pp_ver.major, pp_ver.minor, pp_build.sw_build, pp_build.hw_build);

  /* Check that enough command line arguments given, if not -> print usage
   * information out */
  if (argc < 3) {
    DEBUG_PRINT(("Usage: %s [options] file.hevc  pp.cfg\n", argv[0]));
    DEBUG_PRINT(("\t-Nn forces decoding to stop after n pictures\n"));
    DEBUG_PRINT(("\t-X Disable output file writing\n"));
    DEBUG_PRINT(("\t-R disable DPB output reordering\n"));
    DEBUG_PRINT(("\t-J enable double DPB for smooth display\n"));
    DEBUG_PRINT(
        ("\t-W disable packetizing even if stream does not fit to buffer.\n"
         "\t   NOTE: Useful only for debug purposes.\n"));
    DEBUG_PRINT(("\t-L enable support for long streams.\n"));
    DEBUG_PRINT(("\t-I save index file\n"));
    DEBUG_PRINT(("\t-E use tiled reference frame format.\n"));
#ifdef ASIC_TRACE_SUPPORT
    DEBUG_PRINT(
        ("\t-F force 8170 mode in 8190 HW model (baseline configuration "
         "forced)\n"));
    DEBUG_PRINT(("\t-B force Baseline configuration to 8190 HW model\n"));
    DEBUG_PRINT(("\t-Q Skip decoding non-reference pictures.\n"));
#endif
    return 0;
  }

  /* read command line arguments */
  for (i = 1; i < (u32)(argc - 2); i++) {
    if (strncmp(argv[i], "-N", 2) == 0) {
      max_num_pics = (u32)atoi(argv[i] + 2);
    } else if (strcmp(argv[i], "-X") == 0) {
      disable_output_writing = 1;
    } else if (strcmp(argv[i], "-R") == 0) {
      disable_output_reordering = 1;
    } else if (strcmp(argv[i], "-J") == 0) {
      use_display_smoothing = 1;
    } else if (strncmp(argv[i], "-E", 2) == 0)
      tiled_output = DEC_REF_FRM_TILED_DEFAULT;
    else if (strcmp(argv[i], "-W") == 0) {
      force_whole_stream = 1;
    } else if (strcmp(argv[i], "-L") == 0) {
      long_stream = 1;
    } else if (strcmp(argv[i], "-I") == 0) {
      save_index = 1;
    } else if (strcmp(argv[i], "-Q") == 0) {
      skip_non_reference = 1;
    } else if (strcmp(argv[i], "--separate-fields-in-dpb") == 0) {
      dpb_mode = DEC_DPB_INTERLACED_FIELD;
    }
#ifdef ASIC_TRACE_SUPPORT
    else if (strcmp(argv[i], "-F") == 0) {
      hevc_support = 0;
      printf("\n\n_force 8170 mode to HW model!!!\n\n");
    } else if (strcmp(argv[i], "-B") == 0) {
      hevc_support = 0;
      printf("\n\n_force Baseline configuration to 8190 HW model!!!\n\n");
    }
#endif
    else {
      DEBUG_PRINT(("UNKNOWN PARAMETER: %s\n", argv[i]));
      return 1;
    }
  }

  /* open data file */
  finput = fopen(argv[argc - 2], "rb");
  if (finput == NULL) {
    DEBUG_PRINT(("Unable to open input file %s\n", argv[argc - 2]));
    exit(100);
  }
#endif

  /* determine test case id from input file name (if contains "case_") */
  {
    char *pc, *pe;
    char in[256];
    strncpy(in, argv[argc - 1], sizeof(in));
    pc = strstr(in, "case_");
    if (pc != NULL) {
      pc += 5;
      pe = strstr(pc, "/");
      if (pe == NULL) pe = strstr(pc, ".");
      if (pe != NULL) {
        *pe = '\0';
        test_case_id = atoi(pc);
      }
    }
  }

  /* open index file for saving */
  if (save_index) {
    findex = fopen("stream.cfg", "w");
    if (findex == NULL) {
      DEBUG_PRINT(("UNABLE TO OPEN INDEX FILE\n"));
      return -1;
    }
  }
  /* try open index file */
  else {
    findex = fopen("stream.cfg", "r");
    if (findex != NULL) {
      use_index = 1;
    }
  }

  /* set test bench configuration */
  TBSetDefaultCfg(&tb_cfg);
  f_tbcfg = fopen("tb.cfg", "r");
  if (f_tbcfg == NULL) {
    DEBUG_PRINT(("UNABLE TO OPEN INPUT FILE: \"tb.cfg\"\n"));
    DEBUG_PRINT(("USING DEFAULT CONFIGURATION\n"));
  } else {
    fclose(f_tbcfg);
    if (TBParseConfig("tb.cfg", TBReadParam, &tb_cfg) == TB_FALSE) return -1;
    if (TBCheckCfg(&tb_cfg) != 0) return -1;
  }
  /*TBPrintCfg(&tb_cfg); */
  mb_error_concealment = 0; /* TBGetDecErrorConcealment(&tb_cfg); */
  rlc_mode = TBGetDecRlcModeForced(&tb_cfg);
  clock_gating = TBGetDecClockGating(&tb_cfg);
  clock_gating_runtime = TBGetDecClockGatingRuntime(&tb_cfg);
  data_discard = TBGetDecDataDiscard(&tb_cfg);
  latency_comp = tb_cfg.dec_params.latency_compensation;
  output_picture_endian = TBGetDecOutputPictureEndian(&tb_cfg);
  bus_burst_length = tb_cfg.dec_params.bus_burst_length;
  asic_service_priority = tb_cfg.dec_params.asic_service_priority;
  output_format = TBGetDecOutputFormat(&tb_cfg);
  service_merge_disable = TBGetDecServiceMergeDisable(&tb_cfg);
  bus_width = TBGetDecBusWidth(&tb_cfg);
  /* bus_width 0 -> 32b, 1 -> 64b etc */
  bus_width_in_bytes = 4 << bus_width;
  /*#if MD5SUM
      output_picture_endian = DEC_X170_LITTLE_ENDIAN;
      printf("Decoder Output Picture Endian forced to %d\n",
             output_picture_endian);
  #endif*/

  strm_swap = tb_cfg.dec_params.strm_swap;
  pic_swap = tb_cfg.dec_params.pic_swap;
  dirmv_swap = tb_cfg.dec_params.dirmv_swap;
  tab0_swap = tb_cfg.dec_params.tab0_swap;
  tab1_swap = tb_cfg.dec_params.tab1_swap;
  tab2_swap = tb_cfg.dec_params.tab2_swap;
  tab3_swap = tb_cfg.dec_params.tab3_swap;
  rscan_swap = tb_cfg.dec_params.rscan_swap;
  max_burst = tb_cfg.dec_params.max_burst;

  double_ref_buffer = tb_cfg.dec_params.ref_double_buffer_enable;
  timeout_cycles = tb_cfg.dec_params.timeout_cycles;

  DEBUG_PRINT(
      ("Decoder Macro Block Error Concealment %d\n", mb_error_concealment));
  DEBUG_PRINT(("Decoder RLC %d\n", rlc_mode));
  DEBUG_PRINT(("Decoder Clock Gating %d\n", clock_gating));
  DEBUG_PRINT(("Decoder Clock Gating Runtime%d\n", clock_gating_runtime));
  DEBUG_PRINT(("Decoder Data Discard %d\n", data_discard));
  DEBUG_PRINT(("Decoder Latency Compensation %d\n", latency_comp));
  DEBUG_PRINT(("Decoder Output Picture Endian %d\n", output_picture_endian));
  DEBUG_PRINT(("Decoder Bus Burst Length %d\n", bus_burst_length));
  DEBUG_PRINT(("Decoder Asic Service Priority %d\n", asic_service_priority));
  DEBUG_PRINT(("Decoder Output Format %d\n", output_format));

  seed_rnd = tb_cfg.tb_params.seed_rnd;
  stream_header_corrupt = TBGetTBStreamHeaderCorrupt(&tb_cfg);
  /* if headers are to be corrupted
   * -> do not wait the picture to finalize before starting stream corruption */
  if (stream_header_corrupt) pic_rdy = 1;
  stream_truncate = TBGetTBStreamTruncate(&tb_cfg);
  if (strcmp(tb_cfg.tb_params.stream_bit_swap, "0") != 0) {
    stream_bit_swap = 1;
  } else {
    stream_bit_swap = 0;
  }
  if (strcmp(tb_cfg.tb_params.stream_packet_loss, "0") != 0) {
    stream_packet_loss = 1;
  } else {
    stream_packet_loss = 0;
  }

  packetize = packetize ? 1 : TBGetTBPacketByPacket(&tb_cfg);
  nal_unit_stream = TBGetTBNalUnitStream(&tb_cfg);
  DEBUG_PRINT(("TB Packet by Packet  %d\n", packetize));
  DEBUG_PRINT(("TB Nal Unit Stream %d\n", nal_unit_stream));
  DEBUG_PRINT(("TB Seed Rnd %d\n", seed_rnd));
  DEBUG_PRINT(("TB Stream Truncate %d\n", stream_truncate));
  DEBUG_PRINT(("TB Stream Header Corrupt %d\n", stream_header_corrupt));
  DEBUG_PRINT(("TB Stream Bit Swap %d; odds %s\n", stream_bit_swap,
               tb_cfg.tb_params.stream_bit_swap));
  DEBUG_PRINT(("TB Stream Packet Loss %d; odds %s\n", stream_packet_loss,
               tb_cfg.tb_params.stream_packet_loss));

  {
    remove("regdump.txt");
    remove("mbcontrol.hex");
    remove("intra4x4_modes.hex");
    remove("motion_vectors.hex");
    remove("rlc.hex");
    remove("picture_ctrl_dec.trc");
  }

  if (trace_target) tb_cfg.tb_params.extra_cu_ctrl_eof = 1;

#ifdef ASIC_TRACE_SUPPORT
  /* open tracefiles */
  tmp = OpenAsicTraceFiles();
  if (!tmp) {
    DEBUG_PRINT(("UNABLE TO OPEN TRACE FILE(S)\n"));
  }
#if 0
    if(nal_unit_stream)
        trace_hevc_decoding_tools.stream_mode.nal_unit_strm = 1;
    else
        trace_hevc_decoding_tools.stream_mode.byte_strm = 1;
#endif
#endif

  dwl_inst = DWLInit(&dwl_params);

  /* initialize decoder. If unsuccessful -> exit */
  START_SW_PERFORMANCE;
  {
    struct HevcDecConfig dec_cfg;
    enum DecDpbFlags flags = 0;
    u32 use_8bits_output = 0;
    u32 use_video_compressor = 0;
    u32 use_ringbuffer = 0;
    if (tiled_output) flags |= DEC_REF_FRM_TILED_DEFAULT;
    if (dpb_mode == DEC_DPB_INTERLACED_FIELD)
      flags |= DEC_DPB_ALLOW_FIELD_ORDERING;
    dec_cfg.no_output_reordering = 0;
    dec_cfg.use_video_freeze_concealment = 0;
    dec_cfg.use_video_compressor = 0;
    dec_cfg.use_fetch_one_pic = 1;
    dec_cfg.use_ringbuffer = 0;
    dec_cfg.output_format = DEC_OUT_FRM_RASTER_SCAN;
    dec_cfg.pixel_format = DEC_OUT_PIXEL_DEFAULT;
#ifdef DOWN_SCALER
    dec_cfg.dscale_cfg.down_scale_x = 1;
    dec_cfg.dscale_cfg.down_scale_y = 1;
#endif
    ret = HevcDecInit(&dec_inst, dwl_inst, &dec_cfg);
  }
  END_SW_PERFORMANCE;
  if (ret != DEC_OK) {
    DEBUG_PRINT(("DECODER INITIALIZATION FAILED\n"));
    goto end;
  }

  if (use_extra_dpb_frms)
    HevcDecUseExtraFrmBuffers(dec_inst, use_extra_dpb_frms);

#ifdef PP_PIPELINE_ENABLED
  /* Initialize the post processer. If unsuccessful -> exit */
  if (pp_startup(argv[argc - 1], dec_inst, PP_PIPELINED_DEC_TYPE_HEVC,
                 &tb_cfg) != 0) {
    DEBUG_PRINT(("PP INITIALIZATION FAILED\n"));
    goto end;
  }

  if (pp_update_config(dec_inst, PP_PIPELINED_DEC_TYPE_HEVC, &tb_cfg) ==
      CFG_UPDATE_FAIL) {
    fprintf(stdout, "PP CONFIG LOAD FAILED\n");
    goto end;
  }
#endif

  /*
  SetDecRegister(((struct DecContainer *) dec_inst)->hevc_regs,
  HWIF_DEC_LATENCY,
                 latency_comp);
  SetDecRegister(((struct DecContainer *) dec_inst)->hevc_regs,
  HWIF_DEC_CLK_GATE_E,
                 clock_gating);
  SetDecRegister(((struct DecContainer *) dec_inst)->hevc_regs,
  HWIF_DEC_OUT_BYTESWAP,
                 output_picture_endian);
  SetDecRegister(((struct DecContainer *) dec_inst)->hevc_regs,
  HWIF_DEC_MAX_BURST,
                 bus_burst_length);
  */

  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_BUSWIDTH, bus_width);

  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_STRM_SWAP, strm_swap);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_PIC_SWAP, pic_swap);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_DIRMV_SWAP, dirmv_swap);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_TAB0_SWAP, tab0_swap);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_TAB1_SWAP, tab1_swap);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_TAB2_SWAP, tab2_swap);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_TAB3_SWAP, tab3_swap);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_RSCAN_SWAP, rscan_swap);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_MAX_BURST, max_burst);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_REFER_DOUBLEBUFFER_E, double_ref_buffer);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_DEC_BUSWIDTH, bus_width);

  if (clock_gating) {
    SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                   HWIF_DEC_CLK_GATE_E, clock_gating);
    SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                   HWIF_DEC_CLK_GATE_IDLE_E, clock_gating_runtime);
  }
  /*
  SetDecRegister(((struct HevcDecContainer *) dec_inst)->hevc_regs,
  HWIF_SERV_MERGE_DIS,
                 service_merge_disable);
  */

  /* APF disabled? */
  if (tb_cfg.dec_params.apf_disable != 0)
    SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                   HWIF_APF_DISABLE, tb_cfg.dec_params.apf_disable);

  /* APF threshold disabled? */
  if (!TBGetDecApfThresholdEnabled(&tb_cfg))
    SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                   HWIF_APF_THRESHOLD, 0);
  else if (tb_cfg.dec_params.apf_threshold_value != -1)
    SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                   HWIF_APF_THRESHOLD, tb_cfg.dec_params.apf_threshold_value);

  /* Set timeouts. Value of 0 implies use of hardware default. */
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_TIMEOUT_OVERRIDE_E, timeout_cycles ? 1 : 0);
  SetDecRegister(((struct HevcDecContainer *)dec_inst)->hevc_regs,
                 HWIF_TIMEOUT_CYCLES, timeout_cycles);

  TBInitializeRandom(seed_rnd);

  /* check size of the input file -> length of the stream in bytes */
  fseek(finput, 0L, SEEK_END);
  strm_len = ftell(finput);
  rewind(finput);

  /* REMOVED: dec_input.skip_non_reference = skip_non_reference; */

  if (!long_stream) {
    /* If the decoder can not handle the whole stream at once, force
     * packet-by-packet mode */
    if (!force_whole_stream) {
      if (strm_len > DEC_X170_MAX_STREAM) {
        packetize = 1;
      }
    } else {
      if (strm_len > DEC_X170_MAX_STREAM) {
        packetize = 0;
        strm_len = DEC_X170_MAX_STREAM;
      }
    }

    /* sets the stream length to random value */
    if (stream_truncate && !packetize && !nal_unit_stream) {
      DEBUG_PRINT(("strm_len %d\n", strm_len));
      ret = TBRandomizeU32(&strm_len);
      if (ret != 0) {
        DEBUG_PRINT(("RANDOM STREAM ERROR FAILED\n"));
        goto end;
      }
      DEBUG_PRINT(("Randomized strm_len %d\n", strm_len));
    }

/* NOTE: The struct DWL should not be used outside decoder SW.
 * here we call it because it is the easiest way to get
 * dynamically allocated linear memory
 * */

/* allocate memory for stream buffer. if unsuccessful -> exit */
#ifndef ADS_PERFORMANCE_SIMULATION
    if (DWLMallocLinear(dwl_inst, strm_len,
                        &stream_mem) != DWL_OK) {
      DEBUG_PRINT(("UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n"));
      goto end;
    }
#else
    stream_mem.virtual_address = memalign(16, strm_len);
    stream_mem.bus_address = (size_t)stream_mem.virtual_address;
#endif

    byte_strm_start = (u8 *)stream_mem.virtual_address;

    if (byte_strm_start == NULL) {
      DEBUG_PRINT(("UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n"));
      goto end;
    }

    /* read input stream from file to buffer and close input file */
    fread(byte_strm_start, 1, strm_len, finput);

    /* initialize HevcDecDecode() input structure */
    stream_stop = byte_strm_start + strm_len;
    dec_input.stream = byte_strm_start;
    dec_input.stream_bus_address = (u32)stream_mem.bus_address;

    dec_input.data_len = strm_len;
  } else {
#ifndef ADS_PERFORMANCE_SIMULATION
    if (DWLMallocLinear(((struct HevcDecContainer *)dec_inst)->dwl,
                        DEC_X170_MAX_STREAM, &stream_mem) != DWL_OK) {
      DEBUG_PRINT(("UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n"));
      goto end;
    }
#else
    stream_mem.virtual_address = malloc(DEC_X170_MAX_STREAM);
    stream_mem.bus_address = (size_t)stream_mem.virtual_address;

    if (stream_mem.virtual_address == NULL) {
      DEBUG_PRINT(("UNABLE TO ALLOCATE STREAM BUFFER MEMORY\n"));
      goto end;
    }
#endif

    /* initialize HevcDecDecode() input structure */
    dec_input.stream = (u8 *)stream_mem.virtual_address;
    dec_input.stream_bus_address = (u32)stream_mem.bus_address;
  }

  if (long_stream && !packetize && !nal_unit_stream) {
    if (use_index == 1) {
      u32 amount = 0;
      cur_index = 0;

      /* read index */
      fscanf(findex, "%llu", &next_index);

      {
        /* check if last index -> stream end */
        u32 byte = 0;
        fread(&byte, 2, 1, findex);
        if (feof(findex)) {
          DEBUG_PRINT(("STREAM WILL END\n"));
          stream_will_end = 1;
        } else {
          fseek(findex, -2, SEEK_CUR);
        }
      }

      amount = next_index - cur_index;
      cur_index = next_index;

      /* read first block */
      dec_input.data_len = fread(dec_input.stream, 1, amount, finput);
    } else {
      dec_input.data_len =
          fread(dec_input.stream, 1, DEC_X170_MAX_STREAM, finput);
    }
    /*DEBUG_PRINT(("BUFFER READ\n")); */
    if (feof(finput)) {
      DEBUG_PRINT(("STREAM WILL END\n"));
      stream_will_end = 1;
    }
  } else {
    if (use_index) {
      if (!nal_unit_stream) fscanf(findex, "%llu", &cur_index);
    }

    /* get pointer to next packet and the size of packet
     * (for packetize or nal_unit_stream modes) */
    ptmpstream = dec_input.stream;
    if ((tmp = NextPacket((u8 **)(&dec_input.stream))) != 0) {
      dec_input.data_len = tmp;
      dec_input.stream_bus_address += (u32)(dec_input.stream - ptmpstream);
    }
  }

  /* main decoding loop */
  do {
    save_flag = 1;
    /*DEBUG_PRINT(("dec_input.data_len %d\n", dec_input.data_len));
     * DEBUG_PRINT(("dec_input.stream %d\n", dec_input.stream)); */

    if (stream_truncate && pic_rdy && (hdrs_rdy || stream_header_corrupt) &&
        (long_stream || (!long_stream && (packetize || nal_unit_stream)))) {
      i32 ret;

      ret = TBRandomizeU32(&dec_input.data_len);
      if (ret != 0) {
        DEBUG_PRINT(("RANDOM STREAM ERROR FAILED\n"));
        return 0;
      }
      DEBUG_PRINT(("Randomized stream size %d\n", dec_input.data_len));
    }

    /* If enabled, break the stream */
    if (stream_bit_swap) {
      if ((hdrs_rdy && !stream_header_corrupt) || stream_header_corrupt) {
        /* Picture needs to be ready before corrupting next picture */
        if (pic_rdy && corrupted_bytes <= 0) {
          ret = TBRandomizeBitSwapInStream(dec_input.stream, dec_input.data_len,
                                           tb_cfg.tb_params.stream_bit_swap);
          if (ret != 0) {
            DEBUG_PRINT(("RANDOM STREAM ERROR FAILED\n"));
            goto end;
          }

          corrupted_bytes = dec_input.data_len;
          DEBUG_PRINT(("corrupted_bytes %d\n", corrupted_bytes));
        }
      }
    }

    /* Picture ID is the picture number in decoding order */
    dec_input.pic_id = pic_decode_number;

    /* call API function to perform decoding */
    START_SW_PERFORMANCE;
#if 0
    dec_input.buffer = dec_input.stream;
    dec_input.buffer_bus_address = dec_input.stream_bus_address;
    dec_input.buff_len = dec_input.data_len;
#else
    /* buffer related info should be aligned to 16 */
    dec_input.buffer = (u8 *)((u32)dec_input.stream & (~BUFFER_ALIGN_MASK));
    dec_input.buffer_bus_address = dec_input.stream_bus_address & (~BUFFER_ALIGN_MASK);
    dec_input.buff_len = dec_input.data_len + (dec_input.stream_bus_address & BUFFER_ALIGN_MASK);
#endif
    ret = HevcDecDecode(dec_inst, &dec_input, &dec_output);
    END_SW_PERFORMANCE;
    print_decode_return(ret);
    switch (ret) {
      case DEC_STREAM_NOT_SUPPORTED: {
        DEBUG_PRINT(("ERROR: UNSUPPORTED STREAM!\n"));
        goto end;
      }
      case DEC_HDRS_RDY: {
        save_flag = 0;
        /* Set a flag to indicate that headers are ready */
        hdrs_rdy = 1;

        /* Stream headers were successfully decoded
         * -> stream information is available for query now */

        START_SW_PERFORMANCE;
        tmp = HevcDecGetInfo(dec_inst, &dec_info);
        END_SW_PERFORMANCE;
        if (tmp != DEC_OK) {
          DEBUG_PRINT(("ERROR in getting stream info!\n"));
          goto end;
        }

        DEBUG_PRINT(
            ("Width %d Height %d\n", dec_info.pic_width, dec_info.pic_height));

        DEBUG_PRINT(("Cropping params: (%d, %d) %dx%d\n",
                     dec_info.crop_params.crop_left_offset,
                     dec_info.crop_params.crop_top_offset,
                     dec_info.crop_params.crop_out_width,
                     dec_info.crop_params.crop_out_height));

        DEBUG_PRINT(("MonoChrome = %d\n", dec_info.mono_chrome));
        DEBUG_PRINT(("DPB mode   = %d\n", dec_info.dpb_mode));
        DEBUG_PRINT(("Pictures in DPB = %d\n", dec_info.pic_buff_size));
        DEBUG_PRINT(
            ("Pictures in Multibuffer PP = %d\n", dec_info.multi_buff_pp_size));
        DEBUG_PRINT(("video_range %d, matrix_coefficients %d\n",
                     dec_info.video_range, dec_info.matrix_coefficients));

        if (dec_info.pic_buff_size < 10)
          HevcDecUseExtraFrmBuffers(dec_inst, 10 - dec_info.pic_buff_size);

        dpb_mode = dec_info.dpb_mode;
#ifdef PP_PIPELINE_ENABLED
        PpNumberOfBuffers(dec_info.multi_buff_pp_size);
#endif
        /* Decoder output frame size in planar YUV 4:2:0 */
        pic_size = dec_info.pic_width * dec_info.pic_height;
        if (!dec_info.mono_chrome) pic_size = (3 * pic_size) / 2;

        /* raster picture stride is a bus width multiple */
        raster_stride = (dec_info.pic_width + bus_width_in_bytes - 1) &
                        (~(bus_width_in_bytes - 1));

        break;
      }
      case DEC_ADVANCED_TOOLS: {
        /* ASO/STREAM ERROR was noticed in the stream. The decoder has to
         * reallocate resources */
        assert(dec_output.data_left);
        /* we should have some data left */ /* Used to indicate that picture
                                               decoding needs to finalized
                                               prior to corrupting next
                                               picture */

        /* Used to indicate that picture decoding needs to finalized prior to
         * corrupting next picture
         * pic_rdy = 0; */
        break;
      }
      case DEC_PIC_DECODED:
        /* If enough pictures decoded -> force decoding to end
         * by setting that no more stream is available */
        if (pic_decode_number == max_num_pics) dec_input.data_len = 0;

        printf("DECODED PICTURE %d\n", pic_decode_number);
        /* Increment decoding number for every decoded picture */
        pic_decode_number++;

        if (!output_thread_run) {
          output_thread_run = 1;
          pthread_create(&output_thread, NULL, HevcOutputThread, NULL);
        }

      case DEC_PENDING_FLUSH:
        /* case DEC_FREEZED_PIC_RDY: */
        /* Picture is now ready */
        pic_rdy = 1;
        /* use function HevcDecNextPicture() to obtain next picture
         * in display order. Function is called until no more images
         * are ready for display */

        retry = 0;
        break;

      case DEC_STRM_PROCESSED:
      case DEC_NONREF_PIC_SKIPPED:
      case DEC_STRM_ERROR: {
        /* Used to indicate that picture decoding needs to finalized prior to
         * corrupting next picture
         * pic_rdy = 0; */

        break;
      }
      case DEC_WAITING_FOR_BUFFER:
      {
#ifdef USE_EXTERNAL_BUFFER
        DEBUG_PRINT(("Waiting for frame buffers\n"));
        struct HevcDecBufferInfo hbuf;
        struct DWLLinearMem mem;
        enum DecRet rv;

        rv = HevcDecGetBufferInfo(dec_inst, &hbuf);
        DEBUG_PRINT(("HevcDecGetBufferInfo ret %d\n", rv));
        DEBUG_PRINT(("buf_to_free %p, next_buf_size %d, buf_num %d\n",
            (void *)hbuf.buf_to_free.virtual_address, hbuf.next_buf_size, hbuf.buf_num));

        if (rv == DEC_WAITING_FOR_BUFFER && hbuf.buf_to_free.virtual_address)
          DWLFreeLinear(dwl_inst, &hbuf.buf_to_free);

        if(hbuf.next_buf_size) {
            for (int i=0; i<hbuf.buf_num; i++) {
                DWLMallocLinear(dwl_inst, hbuf.next_buf_size, &mem);
                rv = HevcDecAddBuffer(dec_inst, &mem);
                DEBUG_PRINT(("HevcDecAddBuffer ret %d\n", rv));
                ext_buffers[i] = mem;
            }
        }
#endif
      }
        break;
      case DEC_OK:
        /* nothing to do, just call again */
        break;
      case DEC_HW_TIMEOUT:
        DEBUG_PRINT(("Timeout\n"));
        goto end;
      default:
        DEBUG_PRINT(("FATAL ERROR: %d\n", ret));
        goto end;
    }

    /* break out of do-while if max_num_pics reached (data_len set to 0) */
    if (dec_input.data_len == 0) break;

    if (long_stream && !packetize && !nal_unit_stream) {
      if (stream_will_end) {
        corrupted_bytes -= (dec_input.data_len - dec_output.data_left);
        dec_input.data_len = dec_output.data_left;
        dec_input.stream = dec_output.strm_curr_pos;
        dec_input.stream_bus_address = dec_output.strm_curr_bus_address;
      } else {
        if (use_index == 1) {
          if (dec_output.data_left) {
            dec_input.stream_bus_address +=
                (dec_output.strm_curr_pos - dec_input.stream);
            corrupted_bytes -= (dec_input.data_len - dec_output.data_left);
            dec_input.data_len = dec_output.data_left;
            dec_input.stream = dec_output.strm_curr_pos;
          } else {
            dec_input.stream_bus_address = (u32)stream_mem.bus_address;
            dec_input.stream = (u8 *)stream_mem.virtual_address;
            dec_input.data_len = fill_buffer(dec_input.stream);
          }
        } else {
          if (fseek(finput, -dec_output.data_left, SEEK_CUR) == -1) {
            DEBUG_PRINT(("SEEK FAILED\n"));
            dec_input.data_len = 0;
          } else {
            /* store file index */
            if (save_index && save_flag) {
              fprintf(findex, "%llu\n", ftello64(finput));
            }

            dec_input.data_len =
                fread(dec_input.stream, 1, DEC_X170_MAX_STREAM, finput);
          }
        }

        if (feof(finput)) {
          DEBUG_PRINT(("STREAM WILL END\n"));
          stream_will_end = 1;
        }

        corrupted_bytes = 0;
      }
    } else {
      if (dec_output.data_left) {
        dec_input.stream_bus_address +=
            (dec_output.strm_curr_pos - dec_input.stream);
        corrupted_bytes -= (dec_input.data_len - dec_output.data_left);
        dec_input.data_len = dec_output.data_left;
        dec_input.stream = dec_output.strm_curr_pos;
      } else {
        dec_input.stream_bus_address = (u32)stream_mem.bus_address;
        dec_input.stream = (u8 *)stream_mem.virtual_address;
        /*u32 stream_packet_loss_tmp = stream_packet_loss;
         *
         * if(!pic_rdy)
         * {
         * stream_packet_loss = 0;
         * } */
        ptmpstream = dec_input.stream;

        tmp = ftell(finput);
        dec_input.data_len = NextPacket((u8 **)(&dec_input.stream));
        printf("NextPacket = %d at %d\n", dec_input.data_len, tmp);

        dec_input.stream_bus_address += (u32)(dec_input.stream - ptmpstream);

        /*stream_packet_loss = stream_packet_loss_tmp; */

        corrupted_bytes = 0;
      }
    }

    /* keep decoding until all data from input stream buffer consumed
     * and all the decoded/queued frames are ready */
  } while (dec_input.data_len > 0);

end:

  DEBUG_PRINT(("Decoding ended, flush the DPB\n"));

  HevcDecEndOfStream(dec_inst);

  if (output_thread) pthread_join(output_thread, NULL);

  byte_strm_start = NULL;
  fflush(stdout);

  if (stream_mem.virtual_address != NULL) {
#ifndef ADS_PERFORMANCE_SIMULATION
    if (dec_inst)
      DWLFreeLinear(dwl_inst, &stream_mem);
#else
    free(stream_mem.virtual_address);
#endif
  }

#ifdef USE_EXTERNAL_BUFFER
        DEBUG_PRINT(("Releasing external frame buffers\n"));
        enum DecRet rv;
        for (int i=0; i<MAX_BUFFERS; i++) {
            if (ext_buffers[i].virtual_address != NULL) {
              DEBUG_PRINT(("Freeing buffer %p\n", ext_buffers[i].virtual_address));
              DWLFreeLinear(dwl_inst, &ext_buffers[i]);
              DWLmemset(&ext_buffers[i], 0, sizeof(ext_buffers[i]));
              DEBUG_PRINT(("DWLFreeLinear ret %d\n", rv));
            }
        }
#endif

#ifdef PP_PIPELINE_ENABLED
  PpClose();
#endif

  /* release decoder instance */
  START_SW_PERFORMANCE;
  HevcDecRelease(dec_inst);
  END_SW_PERFORMANCE;
  DWLRelease(dwl_inst);

  if (md5) {
    unsigned char digest[16];

    MD5Final(digest, &md5_ctx);

    for (i = 0; i < sizeof digest; i++) {
      fprintf(foutput, "%02x", digest[i]);
    }
    fprintf(foutput, "  %s\n", out_file_name);
  }

  strm_len = 0;
  if (foutput) {
    fseek(foutput, 0L, SEEK_END);
    strm_len = (u32)ftell(foutput);
    fclose(foutput);
  }
  if (foutput2) fclose(foutput2);
  if (fchroma2) fclose(fchroma2);
  if (f_tiled_output) fclose(f_tiled_output);
  if (f_stream_trace) fclose(f_stream_trace);
  if (finput) fclose(finput);
  if (f_output16) fclose(f_output16);

  /* free allocated buffers */
  if (cropped_pic != NULL) free(cropped_pic);
  if (grey_chroma != NULL) free(grey_chroma);
  if (pic_big_endian) free(pic_big_endian);
  if (raster_scan != NULL) free(raster_scan);

  DEBUG_PRINT(("Output file: %s\n", out_file_name));

  DEBUG_PRINT(("OUTPUT_SIZE %d\n", strm_len));

  FINALIZE_SW_PERFORMANCE;

  DEBUG_PRINT(("DECODING DONE\n"));

#ifdef ASIC_TRACE_SUPPORT
  TraceSequenceCtrl(hw_dec_pic_count, 0);
  /*TraceHevcDecodingTools();*/
  /* close trace files */
  CloseAsicTraceFiles();
#endif

  if (retry > NUM_RETRY) {
    return -1;
  }

  if (num_errors) {
    DEBUG_PRINT(("ERRORS FOUND %d %d\n", num_errors, pic_decode_number));
    /*return 1;*/
    return 0;
  }

  return 0;
}

void CropSemiplanarPicture(u8 * data, u8 *crop_data, const struct HevcCropParams *crop,
                 u32 pic_width, u32 pic_height,
                 u32 bit_depth_luma, u32 bit_depth_chroma, u32 pic_stride)
{
    u16 *pixel_line;
    int i, j, k;
    u32 crop_size = crop->crop_out_width * crop->crop_out_height;

    if (bit_depth_luma == 8 && bit_depth_chroma == 8)
    {
        /* semi planar -> cropped semi planar */
        /* luma */
        u32 j = crop->crop_top_offset * pic_stride + crop->crop_left_offset;
        for (i = 0; i < crop->crop_out_height; i++)
        {
            memcpy(crop_data + i * crop->crop_out_width, data + j, crop->crop_out_width);
            j += pic_stride;
        }
        /* chroma */
        j = pic_height * pic_stride + crop->crop_top_offset * pic_stride / 2 + crop->crop_left_offset;
        for (i = 0; i < crop->crop_out_height / 2; i++)
        {
            memcpy(crop_data + crop_size + i * crop->crop_out_width, data + j, crop->crop_out_width);
            j += pic_stride;
        }
    }
    else
    {
        /* One pixel in 10 bits. */
        u8 *line = data + crop->crop_top_offset * pic_stride;
        pixel_line = (u16 *)malloc(pic_width * 2);
        if (!pixel_line)
        {
            DEBUG_PRINT(("UNABLE TO ALLOCATE TEMP BUFFERS\n"));
            //return -1;
        }


        /* semi planar -> cropped semi planar */
        for (i = 0; i < crop->crop_out_height; i++)
        {
            for (j = k = 0; j < pic_width; j += 4)
            {
                /* unpack  4 pixel of 10 bits to 16 bits */
                pixel_line[j] = ((u16)line[k] | (((u16)line[k+1] & 0x3) << 8)); // << (16 - bit_depth_luma);
                pixel_line[j + 1] = (((u16)line[k + 1] >> 2) | (((u16)line[k + 2] & 0x0f) << 6)); // << (16 - bit_depth_luma);
                pixel_line[j + 2] = (((u16)line[k + 2] >> 4) | (((u16)line[k + 3] & 0x3f) << 4)); // << (16 - bit_depth_luma);
                pixel_line[j + 3] = (((u16)line[k + 3] >> 6) | (((u16)line[k + 4] & 0xff) << 2)); // << (16 - bit_depth_luma);
                k += 5;
            }
            memcpy((u16 *)crop_data + i * crop->crop_out_width, pixel_line + crop->crop_left_offset, crop->crop_out_width * sizeof(u16));
            line += pic_stride;
        }

        /* chroma */
        line = data + pic_height * pic_stride + crop->crop_top_offset * pic_stride / 2;
        for (i = 0; i < crop->crop_out_height / 2; i++)
        {
            for (j = k = 0; j < pic_width; j += 4)
            {
                pixel_line[j] = ((u16)line[k] | (((u16)line[k+1] & 0x3) << 8)); // << (16 - bit_depth_chroma);
                pixel_line[j + 1] = (((u16)line[k + 1] >> 2) | (((u16)line[k + 2] & 0x0f) << 6)); // << (16 - bit_depth_chroma);
                pixel_line[j + 2] = (((u16)line[k + 2] >> 4) | (((u16)line[k + 3] & 0x3f) << 4)); // << (16 - bit_depth_chroma);
                pixel_line[j + 3] = (((u16)line[k + 3] >> 6) | (((u16)line[k + 4] & 0xff) << 2)); // << (16 - bit_depth_chroma);
                k += 5;
            }
              memcpy((u16 *)crop_data + crop_size + i * crop->crop_out_width, pixel_line + crop->crop_left_offset, crop->crop_out_width * sizeof(u16));
              line += pic_stride;
        }
        free(pixel_line);
    }
}

void Semiplanar2Planar(const u16 *src, u16 *dst, int w, int h)
{
  int i,j,l;
  u16 *dst_u;
  u16 *dst_v;

  memcpy(dst, src, w * h * 2);

  dst += w * h;
  src += w * h;

  dst_u = dst;
  dst_v = dst + (w * h) / 4;

  for (i = 0; i < h / 2; i++) {
    for (j = 0; j < w / 2; j += 8) {
      for (l = 0; l < 8; l++) {
        dst_u[i * (w / 2) + j + l] = src[i * w + j * 2 + 2 * l];
        dst_v[i * (w / 2) + j + l] = src[i * w + j * 2 + 2 * l + 1];
      }
    }
  }
}

void Semiplanar2Planar8(const u8 *src, u8 *dst, int w, int h)
{
  int i,j,l;
  u8 *dst_u;
  u8 *dst_v;

  memcpy(dst, src, w * h);

  dst += w * h;
  src += w * h;

  dst_u = dst;
  dst_v = dst + (w * h) / 4;

  for (i = 0; i < h / 2; i++) {
    for (j = 0; j < w / 2; j += 8) {
      for (l = 0; l < 8; l++) {
        dst_u[i * (w / 2) + j + l] = src[i * w + j * 2 + 2 * l];
        dst_v[i * (w / 2) + j + l] = src[i * w + j * 2 + 2 * l + 1];
      }
    }
  }
}

/* Write picture pointed by data to file. Size of the picture in pixels is
 * indicated by pic_size. */
void WriteOutput(const char *filename, const char *filename_tiled, u8 *data,
                 u32 pic_size, u32 pic_width, u32 pic_height, u32 frame_number,
                 u32 mono_chrome, u32 view, u32 tiled_mode,
                 const struct HevcCropParams *crop, u32 bit_depth_luma,
                 u32 bit_depth_chroma, u32 pic_stride) {

  u32 i, tmp;
  FILE **fout;
  u32 pixel_width = 8;
  u32 bit_depth = 8;

  if (disable_output_writing != 0) {
    return;
  }

  fout = &foutput;
  /* foutput is global file pointer */
  if (*fout == NULL) {
    /* open output file for writing, can be disabled with define.
     * If file open fails -> exit */
    if (strcmp(filename, "none") != 0) {
      *fout = fopen(filename, "wb");
      if (*fout == NULL) {
        DEBUG_PRINT(("UNABLE TO OPEN OUTPUT FILE\n"));
        return;
      }
    }
  }

  if (bit_depth_luma != 8 || bit_depth_chroma != 8) {
      pixel_width = 10;
      bit_depth = 10;
  }

#if 0
    if(f_tiled_output == NULL && tiled_output)
    {
        /* open output file for writing, can be disabled with define.
         * If file open fails -> exit */
        if(strcmp(filename_tiled, "none") != 0)
        {
            f_tiled_output = fopen(filename_tiled, "wb");
            if(f_tiled_output == NULL)
            {
                DEBUG_PRINT(("UNABLE TO OPEN TILED OUTPUT FILE\n"));
                return;
            }
        }
    }
#endif

  /* Convert back to raster scan format if decoder outputs
   * tiled format */
  if (tiled_mode == DEC_OUT_FRM_TILED_4X4 && convert_tiled_output) {
    u32 eff_height = (pic_height + 15) & (~15);

    if (raster_scan_size != (pic_width * eff_height * 3 / 2 * pixel_width / 8)) {
      raster_scan_size = (pic_width * eff_height * 3 / 2 * pixel_width / 8);
      if (raster_scan != NULL) free(raster_scan);

      raster_scan = (u8 *)malloc(raster_scan_size);
      if (!raster_scan) {
        fprintf(stderr,
                "error allocating memory for tiled"
                "-->raster conversion!\n");
        return;
      }
    }

    TBTiledToRaster(1, convert_to_frame_dpb ? dpb_mode : DEC_DPB_FRAME, data,
                    raster_scan, pic_width, eff_height);
    if (!mono_chrome)
      TBTiledToRaster(1, convert_to_frame_dpb ? dpb_mode : DEC_DPB_FRAME,
                      data + pic_width * eff_height,
                      raster_scan + pic_width * eff_height, pic_width,
                      eff_height / 2);
    data = raster_scan;
  } else {
    /* raster scan output always 16px multiple */
    pic_size = pic_stride * pic_height * (mono_chrome ? 2 : 3) / 2 * pixel_width / 8;
  }

#if 0
  if (crop_display || planar_output) {
    /* Cropped frame size in planar YUV 4:2:0 */
    size_t new_size;
    if (crop_display)
      new_size = crop->crop_out_width * crop->crop_out_height;
    else
      new_size = pic_width * pic_height;

    if (!mono_chrome) new_size = (3 * new_size) / 2;

    if (new_size > cropped_pic_size) {
      if (cropped_pic != NULL) free(cropped_pic);
      cropped_pic_size = new_size;
      cropped_pic = malloc(cropped_pic_size);
      if (cropped_pic == NULL) {
        DEBUG_PRINT(("ERROR in allocating cropped image!\n"));
        exit(1);
      }
    }
        if (crop_display)
        {
            if(CropPicture(cropped_pic, data,
                           pic_width, pic_height,
                           crop, monoChrome, planarOutput))
            {
                DEBUG_PRINT(("ERROR in cropping!\n"));
                exit(100);
            }
        }
        else
            Semiplanar2Planar(data,
                              cropped_pic,
                              pic_width, pic_height);

        data = cropped_pic;
        pic_width = crop->crop_out_width;
        pic_height= crop->crop_out_height;
        picSize = cropped_pic_size;
    }
#endif


  if (mono_chrome) {
    // allocate "neutral" chroma buffer
    if (grey_chroma_size != (pic_size / 2)) {
      if (grey_chroma != NULL) free(grey_chroma);

      grey_chroma = (u8 *)malloc(pic_size / 2);
      if (grey_chroma == NULL) {
        DEBUG_PRINT(("UNABLE TO ALLOCATE GREYSCALE CHROMA BUFFER\n"));
        return;
      }
      grey_chroma_size = pic_size / 2;
      memset(grey_chroma, 128, grey_chroma_size);
    }
  }

  if (*fout == NULL || data == NULL) {
    return;
  }

#ifndef ASIC_TRACE_SUPPORT
  if (output_picture_endian == DEC_X170_BIG_ENDIAN) {
    if (pic_big_endian_size != pic_size) {
      if (pic_big_endian != NULL) free(pic_big_endian);

      pic_big_endian = (u8 *)malloc(pic_size);
      if (pic_big_endian == NULL) {
        DEBUG_PRINT(("MALLOC FAILED @ %s %d", __FILE__, __LINE__));
        return;
      }

      pic_big_endian_size = pic_size;
    }

    memcpy(pic_big_endian, data, pic_size);
    TbChangeEndianess(pic_big_endian, pic_size);
    data = pic_big_endian;
  }
#endif

#ifdef MD5SUM
  TBWriteFrameMD5Sum(*fout, data, pic_size, frame_number);
#else
  if (md5) {
    MD5Update(&md5_ctx, data, pic_size);
    if (mono_chrome) {
      MD5Update(&md5_ctx, grey_chroma, grey_chroma_size);
    }
  } else {
    fwrite(data, 1, pic_size, *fout);
    if (mono_chrome) {
      fwrite(grey_chroma, 1, grey_chroma_size, *fout);
    }
  }
#endif

  if (planar_output)
  {
      /* semi planar -> planar */
      u16 *planar_data;
      u8 *crop_data;
      u32 crop_size = crop->crop_out_width * crop->crop_out_height;
      if (f_output16 == NULL)
      {
          f_output16 = fopen(out_file_name_planar, "wb");
          if(f_output16 == NULL)
          {
              DEBUG_PRINT(("UNABLE TO OPEN OUTPUT FILE\n"));
              return;
          }
      }

      crop_data = (u8 *)malloc(crop_size * 3);
      planar_data = (u8 *)malloc(crop_size * 3);
      if (!planar_data || !crop_data)
      {
          DEBUG_PRINT(("UNABLE TO ALLOCATE TEMP BUFFERS\n"));
          //return -1;
      }

      CropSemiplanarPicture(data, crop_data, crop, pic_width, pic_height, bit_depth_luma, bit_depth_chroma, pic_stride);

      if (bit_depth_luma == 8 && bit_depth_chroma == 8)
      {
          Semiplanar2Planar8(crop_data, (u8 *)planar_data, crop->crop_out_width, crop->crop_out_height);

          fwrite(planar_data, 1, crop_size * 3 / 2, f_output16);
      }
      else
      {
          Semiplanar2Planar((u16 *)crop_data, planar_data, crop->crop_out_width, crop->crop_out_height);

          fwrite(planar_data, 2, crop_size * 3 / 2, f_output16);
      }

      free(crop_data);
      free(planar_data);
  } else if (output16) {
    u16 buf[4];
    int j = 0;
    int samples;
    if (f_output16 == NULL)
    {
        f_output16 = fopen(out_file_name_16, "wb");
        if(f_output16 == NULL)
        {
            DEBUG_PRINT(("UNABLE TO OPEN OUTPUT FILE\n"));
            return;
        }
    }
    if (bit_depth_luma == 8 && bit_depth_chroma == 8) {
      /* One pixel in 8 bits. */
      samples = pic_size;
      for (int i = 0; i < samples / 4; i++) {
        buf[0] = data[j];
        buf[1] = data[j + 1];
        buf[2] = data[j + 2];
        buf[3] = data[j + 3];
        fwrite(buf, sizeof(u16), 4, f_output16);
        j += 4;
      }
    } else {
      /* One pixel in 10 bits. */
      samples = pic_size * 8 / 10;
      for (int i = 0; i < samples / 4; i++) {
        buf[0] = (u16)data[j] | (((u16)data[j+1] & 0x3) << 8);
        buf[1] = ((u16)data[j + 1] >> 2) | (((u16)data[j + 2] & 0xf) << 6);
        buf[2] = ((u16)data[j + 2] >> 4) | (((u16)data[j + 3] & 0x3f) << 4);
        buf[3] = ((u16)data[j + 3] >> 6) | (((u16)data[j + 4] & 0xff) << 2);
        fwrite(buf, sizeof(u16), 4, f_output16);
        j += 5;
      }
    }
  }
}

/* Get the pointer to start of next packet in input stream. Uses global
 * variables 'packetize' and 'nal_unit_stream' to determine the decoder input
 * stream mode and 'stream_stop' to determine the end of stream. There are three
 * possible stream modes:
 *
 * default - the whole stream at once
 * packetize - a single NAL-unit with start code prefix
 * nal_unit_stream - a single NAL-unit without start code prefix
 *
 * strm stores pointer to the start of previous decoder input and is replaced
 * with pointer to the start of the next decoder input.
 *
 * Returns the packet size in bytes. */
u32 NextPacket(u8 **strm) {
  u32 index;
  u32 max_index;
  u32 zero_count;
  u8 byte;
  static u32 prev_index = 0;
  static u8 *stream = NULL;
  u8 next_packet = 0;

  /* Next packet is read from file is long stream support is enabled */
  if (long_stream) {
    return NextPacketFromFile(strm);
  }

  /* For default stream mode all the stream is in first packet */
  if (!packetize && !nal_unit_stream) return 0;

  /* If enabled, loose the packets (skip this packet first though) */
  if (stream_packet_loss) {
    u32 ret = 0;

    ret = TBRandomizePacketLoss(tb_cfg.tb_params.stream_packet_loss,
                                &next_packet);
    if (ret != 0) {
      DEBUG_PRINT(("RANDOM STREAM ERROR FAILED\n"));
      return 0;
    }
  }

  index = 0;

  if (stream == NULL)
    stream = *strm;
  else
    stream += prev_index;

  max_index = (u32)(stream_stop - stream);

  if (stream > stream_stop) return (0);

  if (max_index == 0) return (0);

  /* leading zeros of first NAL unit */
  do {
    byte = stream[index++];
  } while (byte != 1 && index < max_index);

  /* invalid start code prefix */
  if (index == max_index || index < 3) {
    DEBUG_PRINT(("INVALID BYTE STREAM\n"));
    return 0;
  }

  /* nal_unit_stream is without start code prefix */
  if (nal_unit_stream) {
    stream += index;
    max_index -= index;
    index = 0;
  }

  zero_count = 0;

  /* Search stream for next start code prefix */
  /*lint -e(716) while(1) used consciously */
  while (1) {
    byte = stream[index++];
    if (!byte) zero_count++;

    if ((byte == 0x01) && (zero_count >= 2)) {
      /* Start code prefix has two zeros
       * Third zero is assumed to be leading zero of next packet
       * Fourth and more zeros are assumed to be trailing zeros of this
       * packet */
      if (zero_count > 3) {
        index -= 4;
        zero_count -= 3;
      } else {
        index -= zero_count + 1;
        zero_count = 0;
      }
      break;
    } else if (byte)
      zero_count = 0;

    if (index == max_index) {
      break;
    }
  }

  /* Store pointer to the beginning of the packet */
  *strm = stream;
  prev_index = index;

  /* If we skip this packet */
  if (pic_rdy && next_packet &&
      ((hdrs_rdy && !stream_header_corrupt) || stream_header_corrupt)) {
    /* Get the next packet */
    DEBUG_PRINT(("Packet Loss\n"));
    return NextPacket(strm);
  } else {
    /* nal_unit_stream is without trailing zeros */
    if (nal_unit_stream) index -= zero_count;
    /*DEBUG_PRINT(("No Packet Loss\n")); */
    /*if (pic_rdy && stream_truncate && ((hdrs_rdy && !stream_header_corrupt) ||
     * stream_header_corrupt))
     * {
     * i32 ret;
     * DEBUG_PRINT(("Original packet size %d\n", index));
     * ret = TBRandomizeU32(&index);
     * if(ret != 0)
     * {
     * DEBUG_PRINT(("RANDOM STREAM ERROR FAILED\n"));
     * return 0;
     * }
     * DEBUG_PRINT(("Randomized packet size %d\n", index));
     * } */
    return (index);
  }
}

/* Get the pointer to start of next NAL-unit in input stream. Uses global
 * variables 'finput' to read the stream and 'packetize' to determine the
 * decoder input stream mode.
 *
 * nal_unit_stream a single NAL-unit without start code prefix. If not enabled,
 *a
 * single NAL-unit with start code prefix
 *
 * strm stores pointer to the start of previous decoder input and is replaced
 * with pointer to the start of the next decoder input.
 *
 * Returns the packet size in bytes. */
u32 NextPacketFromFile(u8 **strm) {

  u32 index = 0;
  u32 zero_count;
  u8 byte;
  u8 next_packet = 0;
  i32 ret = 0;
  u8 first_read = 1;
  fpos_t strm_pos;
  static u8 *stream = NULL;

  /* store the buffer start address for later use */
  if (stream == NULL)
    stream = *strm;
  else
    *strm = stream;

  /* If enabled, loose the packets (skip this packet first though) */
  if (stream_packet_loss) {
    ret = TBRandomizePacketLoss(tb_cfg.tb_params.stream_packet_loss,
                                &next_packet);
    if (ret != 0) {
      DEBUG_PRINT(("RANDOM STREAM ERROR FAILED\n"));
      return 0;
    }
  }

  if (fgetpos(finput, &strm_pos)) {
    DEBUG_PRINT(("FILE POSITION GET ERROR\n"));
    return 0;
  }

  if (use_index == 0) {
    /* test end of stream */
    ret = fread(&byte, 1, 1, finput);
    if (ferror(finput)) {
      DEBUG_PRINT(("STREAM READ ERROR\n"));
      return 0;
    }
    if (feof(finput)) {
      DEBUG_PRINT(("END OF STREAM\n"));
      return 0;
    }

    /* leading zeros of first NAL unit */
    do {
      index++;
      /* the byte is already read to test the end of stream */
      if (!first_read) {
        ret = fread(&byte, 1, 1, finput);
        if (ferror(finput)) {
          DEBUG_PRINT(("STREAM READ ERROR\n"));
          return 0;
        }
      } else {
        first_read = 0;
      }
    } while (byte != 1 && !feof(finput));

    /* invalid start code prefix */
    if (feof(finput) || index < 3) {
      DEBUG_PRINT(("INVALID BYTE STREAM\n"));
      return 0;
    }

    /* nal_unit_stream is without start code prefix */
    if (nal_unit_stream) {
      if (fgetpos(finput, &strm_pos)) {
        DEBUG_PRINT(("FILE POSITION GET ERROR\n"));
        return 0;
      }
      index = 0;
    }

    zero_count = 0;

    /* Search stream for next start code prefix */
    /*lint -e(716) while(1) used consciously */
    while (1) {
      /*byte = stream[index++]; */
      index++;
      ret = fread(&byte, 1, 1, finput);
      if (ferror(finput)) {
        DEBUG_PRINT(("FILE ERROR\n"));
        return 0;
      }
      if (!byte) zero_count++;

      if ((byte == 0x01) && (zero_count >= 2)) {
        /* Start code prefix has two zeros
         * Third zero is assumed to be leading zero of next packet
         * Fourth and more zeros are assumed to be trailing zeros of this
         * packet */
        if (zero_count > 3) {
          index -= 4;
          zero_count -= 3;
        } else {
          index -= zero_count + 1;
          zero_count = 0;
        }
        break;
      } else if (byte)
        zero_count = 0;

      if (feof(finput)) {
        --index;
        break;
      }
    }

    /* Store pointer to the beginning of the packet */
    if (fsetpos(finput, &strm_pos)) {
      DEBUG_PRINT(("FILE POSITION SET ERROR\n"));
      return 0;
    }

    if (save_index) {
      fprintf(findex, "%llu\n", strm_pos);
      if (nal_unit_stream) {
        /* store amount */
        fprintf(findex, "%llu\n", index);
      }
    }

    /* Read the rewind stream */
    fread(*strm, 1, index, finput);
    if (feof(finput)) {
      DEBUG_PRINT(("TRYING TO READ STREAM BEYOND STREAM END\n"));
      return 0;
    }
    if (ferror(finput)) {
      DEBUG_PRINT(("FILE ERROR\n"));
      return 0;
    }
  } else {
    u32 amount = 0;
    u32 f_pos = 0;

    if (nal_unit_stream) fscanf(findex, "%llu", &cur_index);

    /* check position */
    f_pos = ftell(finput);
    if (f_pos != cur_index) {
      fseeko64(finput, cur_index - f_pos, SEEK_CUR);
    }

    if (nal_unit_stream) {
      fscanf(findex, "%llu", &amount);
    } else {
      fscanf(findex, "%llu", &next_index);
      amount = next_index - cur_index;
      cur_index = next_index;
    }

    fread(*strm, 1, amount, finput);
    index = amount;
  }
  /* If we skip this packet */
  if (pic_rdy && next_packet &&
      ((hdrs_rdy && !stream_header_corrupt) || stream_header_corrupt)) {
    /* Get the next packet */
    DEBUG_PRINT(("Packet Loss\n"));
    return NextPacket(strm);
  } else {
    /*DEBUG_PRINT(("No Packet Loss\n")); */
    /*if (pic_rdy && stream_truncate && ((hdrs_rdy && !stream_header_corrupt) ||
     * stream_header_corrupt))
     * {
     * i32 ret;
     * DEBUG_PRINT(("Original packet size %d\n", index));
     * ret = TBRandomizeU32(&index);
     * if(ret != 0)
     * {
     * DEBUG_PRINT(("RANDOM STREAM ERROR FAILED\n"));
     * return 0;
     * }
     * DEBUG_PRINT(("Randomized packet size %d\n", index));
     * } */
    return (index);
  }
}

/* Perform cropping for picture. Input picture in_image with dimensions
 * frame_width x frame_height is cropped with crop_params and the resulting
 * picture is stored in out_image. */
u32 CropPicture(u8 *out_image, u8 *in_image, u32 frame_width, u32 frame_height,
                const struct HevcCropParams *crop_params, u32 mono_chrome,
                u32 convert2_planar) {

  u32 i, j;
  u32 out_width, out_height;
  u8 *out, *in, *out_u, *out_v;

  if (out_image == NULL || in_image == NULL || crop_params == NULL ||
      !frame_width || !frame_height) {
    return (1);
  }

  if (((crop_params->crop_left_offset + crop_params->crop_out_width) >
       frame_width) ||
      ((crop_params->crop_top_offset + crop_params->crop_out_height) >
       frame_height)) {
    return (1);
  }

  out_width = crop_params->crop_out_width;
  out_height = crop_params->crop_out_height;

  /* Calculate starting pointer for luma */
  in = in_image + crop_params->crop_top_offset * frame_width +
       crop_params->crop_left_offset;
  out = out_image;
  out_u = out + out_height * out_width;
  out_v = out_u + (out_height * out_width) / 4;

  /* Copy luma pixel values */
  for (i = out_height; i; i--) {
    for (j = out_width; j; j--) {
      *out++ = *in++;
    }
    in += frame_width - out_width;
  }

  if (mono_chrome) return 0;

  out_height >>= 1;

  /* Calculate starting pointer for chroma */
  in = in_image + frame_width * frame_height +
       (crop_params->crop_top_offset * frame_width / 2 +
        crop_params->crop_left_offset);

  /* Copy chroma pixel values */
  for (i = out_height; i; i--) {
    for (j = out_width; j; j -= 2) {
      if (convert2_planar) {
        *out_u++ = *in++;
        *out_v++ = *in++;
      } else {
        *out++ = *in++;
        *out++ = *in++;
      }
    }
    in += (frame_width - out_width);
  }

  return (0);
}

/* Example implementation of HevcDecTrace function. Prototype of this function
 * is given in HevcDecApi.h. This implementation appends trace messages to file
 * named 'dec_api.trc'. */
void HevcDecTrace(const char *string) {
  FILE *fp;

#if 0
    fp = fopen("dec_api.trc", "at");
#else
  fp = stderr;
#endif

  if (!fp) return;

  fprintf(fp, "%s", string);

  if (fp != stderr) fclose(fp);
}

static void print_decode_return(i32 retval) {

  DEBUG_PRINT(("TB: HevcDecDecode returned: "));
  switch (retval) {

    case DEC_OK:
      DEBUG_PRINT(("DEC_OK\n"));
      break;
    case DEC_NONREF_PIC_SKIPPED:
      DEBUG_PRINT(("DEC_NONREF_PIC_SKIPPED\n"));
      break;
    case DEC_STRM_PROCESSED:
      DEBUG_PRINT(("DEC_STRM_PROCESSED\n"));
      break;
    case DEC_PIC_RDY:
      DEBUG_PRINT(("DEC_PIC_RDY\n"));
      break;
    case DEC_PIC_DECODED:
      DEBUG_PRINT(("DEC_PIC_DECODED\n"));
      break;
    case DEC_ADVANCED_TOOLS:
      DEBUG_PRINT(("DEC_ADVANCED_TOOLS\n"));
      break;
    case DEC_HDRS_RDY:
      DEBUG_PRINT(("DEC_HDRS_RDY\n"));
      break;
    case DEC_STREAM_NOT_SUPPORTED:
      DEBUG_PRINT(("DEC_STREAM_NOT_SUPPORTED\n"));
      break;
    case DEC_DWL_ERROR:
      DEBUG_PRINT(("DEC_DWL_ERROR\n"));
      break;
    case DEC_HW_TIMEOUT:
      DEBUG_PRINT(("DEC_HW_TIMEOUT\n"));
      break;
    default:
      DEBUG_PRINT(("Other %d\n", retval));
      break;
  }
}

u32 fill_buffer(u8 *stream) {
  u32 amount = 0;
  u32 data_len = 0;

  if (cur_index != ftell(finput)) {
    fseeko64(finput, cur_index, SEEK_SET);
  }

  /* read next index */
  fscanf(findex, "%llu", &next_index);
  amount = next_index - cur_index;
  cur_index = next_index;

  /* read data */
  data_len = fread(stream, 1, amount, finput);

  return data_len;
}

#ifdef PP_PIPELINE_ENABLED
void HandlePpOutput(u32 pic_display_number, u32 view_id) {

  static char catcmd0[100];
  static char catcmd1[100];
  static char rmcmd[100];
  const char *fname;

  PpWriteOutput(pic_display_number - 1, 0, 0);
}
#endif
