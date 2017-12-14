/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/command_line_parser.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

void PrintUsage(char* executable) {
  printf("Usage: %s [options] <file>\n", executable);
  printf("\n\t_decoding options:\n");
  printf(
      "\t-N<n> stop after <n> output pictures. "
      "(--num-pictures <n>)\n");

  printf("\n\t_picture format options:\n");
  printf("\t-C crop the parts not containing visual data in the pictures. ");
  printf("(--crop)\n");
  printf("\t_one of the following:\n");
  printf("\t\t-T 4x4 tiled, uncropped semiplanar YCbCr 4:2:0) (--tiled) ");
  printf("[default]\n");
  printf("\t\t-S use uncropped raster-ordered, semiplanar YCbCr 4:2:0");
  printf(", four_cc 'NV12' (--semiplanar)\n");
  printf("\t\t-P use planar frame format, four_cc 'IYUV'. (--planar)\n");

  printf("\n\t_input options:\n");
  printf("\t(default: auto detect format from file extension");
  printf(", output frame-by-frame)\n");
  printf("\t-i<format> (--input-format), force input file format");
  printf(" interpretation. <format> can be one of the following:\n");
  printf("\t\t bs -- bytestream format\n");
  printf("\t\t ivf -- IVF file format\n");
  printf("\t\t webm -- WebM file format\n");
  printf("\t-p packetize input bitstream. (NOT suppported yet.)");
  printf("(--packet-by-packet)\n");
  printf("\t-F read full-stream into single buffer. Only with bytestream.");
  printf(" (--full-stream)\n");

  printf("\n\t_output options:\n");
  printf("\t-t generate hardware trace files. (--trace-files)\n");
  printf("\t-r trace file format for RTL (extra CU ctrl). (--rtl-trace)\n");
  printf("\t-R Output in decoding order. (--disable-display-order)\n");
  printf("\t_one of the following:\n");
  printf("\t\t-X disable output writing. (--no-write)\n");
  printf(
      "\t\t-O<outfile> write output to <outfile>. "
      "(-\t-output-file <outfile>) (default out.yuv)\n");
  printf("\t\t-Q Output single frames (--single-frames-out)\n");
  printf("\t\t-M write MD5 sum to output instead of yuv. (--md5)\n");
  printf(
      "\t\t-m write MD5 sum for each picture to output instead of yuv. "
      "(--md5-per-pic)\n");
#ifdef SDL_ENABLED
  printf("\t\t-s Use SDL to display the output pictures. Implies also '-P'.");
  printf(" (--sdl)\n");
#endif
  printf("\n\t_threading options\n");
  printf(
      "\t-Z Run output handling in separate thread. "
      "(--separate-output-thread)\n");
  printf("\n\t_enable hardware features (all listed features disabled by ");
  printf("default)\n");
  printf("\t-E<feature> (--enable), enable hw feature where <feature> is ");
  printf("one of the following:\n");
  printf("\t\t rs -- raster scan conversion\n");
  printf("\t\t p010 -- output in P010 format for 10-bit stream\n");
#ifdef DOWN_SCALER
  printf("\t-d<downscale> (--down_scale), enable down scale feature where <downscale> is ");
  printf("one of the following:\n");
  printf("\t\t <ds_ratio> -- down scale to 1/<ds_ratio> in both directions\n");
  printf("\t\t <ds_ratio_x>:<ds_ratio_y> -- down scale to 1/<ds_ratio_x> in horizontal and 1/<ds_ratio_y> in vertical\n");
  printf("\t\t\t <ds_ratio> should be one of following: 2, 4, 8\n");
#endif
  printf("\t-f force output in 8 bits per pixel for HEVC Main 10 profile (--force-8bits)\n");

  printf("\n\tOther features:\n");
  printf("\t-b bypass reference frame compression (--compress-bypass)\n");
  printf("\t-n stream buffer use non-ringbuffer mode, but ringbuffer mode is by default(--non-ringbuffer)\n");
  printf("\t-g disable multi-reference blocks prefetching (--prefetch-onepic)\n");
  printf("\n");
}

void SetupDefaultParams(struct TestParams* params) {
  memset(params, 0, sizeof(struct TestParams));
  params->read_mode = STREAMREADMODE_FRAME;
#ifdef DOWN_SCALER
  params->dscale_cfg.down_scale_x = 1;
  params->dscale_cfg.down_scale_y = 1;
#endif
  params->is_ringbuffer = 1;
  params->fetch_one_pic = 0;
  params->force_output_8_bits = 0;
  params->format = DEC_OUT_FRM_RASTER_SCAN;

}

int ParseParams(int argc, char* argv[], struct TestParams* params) {
  i32 c;
  i32 option_index = 0;
  u8 flag_S, flag_T, flag_P, flag_s;
  u8 flag_d = 0;
  u8 flag_b = 0;
  flag_S = flag_T = flag_P = flag_s = 0;
  static struct option long_options[] = {
      {"crop", no_argument, 0, 'C'},
      {"no-write", no_argument, 0, 'X'},
      {"semiplanar", no_argument, 0, 'S'},
      {"planar", no_argument, 0, 'P'},
      {"md5", no_argument, 0, 'M'},
      {"md5-per-pic", no_argument, 0, 'm'},
      {"num-pictures", required_argument, 0, 'N'},
      {"output-file", required_argument, 0, 'O'},
      {"trace-files", no_argument, 0, 't'},
      {"rtl-trace", no_argument, 0, 'r'},
      {"separate-output-thread", no_argument, 0, 'Z'},
      {"single-frames-out", no_argument, 0, 'Q'},
      {"full-stream", no_argument, 0, 'F'},
      {"packet-by-packet", no_argument, 0, 'p'},
      {"enable", required_argument, 0, 'E'},
      {"tiled", no_argument, 0, 'T'},
      {"disable-display-order", no_argument, 0, 'R'},
      {"sdl", no_argument, 0, 's'},
      {"input-format", required_argument, 0, 'i'},
#ifdef DOWN_SCALER
      {"down-scale", required_argument, 0, 'd'},
#endif
      {"force-8bits", no_argument, 0, 'f'},
      {"compress-bypass", no_argument, 0, 'b'},
      {"non-ringbuffer", no_argument, 0, 'n'},
      {"prefetch-onepic", no_argument, 0, 'g'},
      {0, 0, 0, 0}};

  /* read command line arguments */
  while ((c = getopt_long(argc, argv,
#ifdef DOWN_SCALER
                          "CE:Fi:MmN:O:PpSTtrXZQRsd:fbng",
#else
                          "CE:Fi:MmN:O:PpSTtrXZQRs:fbng",
#endif
                          long_options,
                          &option_index)) != -1) {
    switch (c) {
      case 'C':
        params->display_cropped = 1;
        break;
      case 'X':
        params->sink_type = SINK_NULL;
        break;
      case 'S':
        if (flag_T || flag_P) {
          fprintf(stderr,
                  "ERROR: options -T -P and -S are mutually "
                  "exclusive!\n");
          return 1;
        }
        if (flag_s) {
          fprintf(stderr, "ERROR: SDL sink supports only -P!\n");
          return 1;
        }
        flag_S = 1;
        params->format = DEC_OUT_FRM_RASTER_SCAN;
        break;
      case 't':
        params->hw_traces = 1;
        /* TODO(vmr): Check SW support for traces. */
        break;
      case 'r':
        params->trace_target = 1;
        break;
      case 'M':
        params->sink_type = SINK_MD5_SEQUENCE;
        break;
      case 'm':
        params->sink_type = SINK_MD5_PICTURE;
        break;
      case 'N':
        params->num_of_decoded_pics = atoi(optarg);
        break;
      case 'O':
        params->out_file_name = optarg;
        break;
      case 'p':
        params->read_mode = STREAMREADMODE_PACKETIZE;
        fprintf(stderr,
                "ERROR: option -p is NOT supported yet!\n");
        return 1;
        break;
      case 'P':
#ifdef TB_PP
        if (flag_T || flag_S) {
          fprintf(stderr,
                  "ERROR: options -T -P and -S are mutually "
                  "exclusive!\n");
          return 1;
        }
        flag_P = 1;
        params->format = DEC_OUT_FRM_PLANAR_420;
#else
        fprintf(stderr,
                  "ERROR: -P is not supported when TB_PP is NOT defined.\n");
        return 1;
#endif
        break;
      case 'Z':
        params->extra_output_thread = 1;
        break;
      case 'Q':
        params->sink_type = SINK_FILE_PICTURE;
        break;
      case 'R':
        params->disable_display_order = 1;
        break;
      case 'F':
        params->read_mode = STREAMREADMODE_FULLSTREAM;
        break;
      case 'E':
        if (strcmp(optarg, "rs") == 0)
          params->hw_format = DEC_OUT_FRM_RASTER_SCAN;
        if (strcmp(optarg, "p010") == 0)
          params->p010_output = 1;
        if (strcmp(optarg, "pbe") == 0)
          params->bigendian_output = 1;
        break;
      case 'T':
        if (flag_P || flag_S) {
          fprintf(stderr,
                  "ERROR: options -T -P and -S are mutually "
                  "exclusive!\n");
          return 1;
        }
        if (flag_s) {
          fprintf(stderr, "ERROR: SDL sink supports only -P!\n");
          return 1;
        }
        flag_T = 1;
        params->format = DEC_OUT_FRM_TILED_4X4;
        break;
#ifdef SDL_ENABLED
      case 's':
        if (flag_T || flag_S) {
          fprintf(stderr, "ERROR: SDL sink supports only -P!\n");
          return 1;
        }
        params->sink_type = SINK_SDL;
        params->format = DEC_OUT_FRM_PLANAR_420;
        flag_s = 1;
        break;
#endif /* SDL_ENABLED */
      case 'i':
        if (strcmp(optarg, "bs") == 0) {
          params->file_format = FILEFORMAT_BYTESTREAM;
        } else if (strcmp(optarg, "ivf") == 0) {
          params->file_format = FILEFORMAT_IVF;
        } else if (strcmp(optarg, "webm") == 0) {
          params->file_format = FILEFORMAT_WEBM;
        } else {
          fprintf(stderr, "Unsupported file format\n");
          return 1;
        }
        break;
      case ':':
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        return 1;
#ifdef DOWN_SCALER
      case 'd':
        if (strlen(optarg) == 1 && (optarg[0] == '2' || optarg[0] == '4' || optarg[0] == '8'))
          params->dscale_cfg.down_scale_x = params->dscale_cfg.down_scale_y = optarg[0] - '0';
        else if (strlen(optarg) == 3 &&
                (optarg[0] == '2' || optarg[0] == '4' || optarg[0] == '8') &&
                (optarg[2] == '2' || optarg[2] == '4' || optarg[2] == '8') &&
                 optarg[1] == ':') {
          params->dscale_cfg.down_scale_x = optarg[0] - '0';
          params->dscale_cfg.down_scale_y = optarg[2] - '0';
        } else {
          fprintf(stderr, "ERROR: Enable down scaler parameter by using: -ds[248][:[248]]\n");
          return 1;
        }
        fprintf(stderr, "Down scaler enabled: 1/%d, 1/%d\n",
          params->dscale_cfg.down_scale_x, params->dscale_cfg.down_scale_y);
        flag_d = 1;
        break;
#endif
      case 'b':
        flag_b = 1;
        params->compress_bypass = 1;
        break;
      case 'n':
        params->is_ringbuffer = 0;
        break;
      case 'g':
        params->fetch_one_pic = 1;
        break;
#ifndef SDL_ENABLED
      case 's':
#endif /* SDL_ENABLED */
      case 'f':
        params->force_output_8_bits = 1;
        break;
      case '?':
        if (isprint(optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        break;
    }
  }
  if (optind >= argc) {
    fprintf(stderr, "Invalid or no input file specified\n");
    return 1;
  }
#ifdef DOWN_SCALER
  if (flag_d && params->display_cropped) {
    fprintf(stderr,
                  "ERROR: options -C and -d are mutually "
                  "exclusive!\n");
    return 1;
  }
#endif

  if ((params->p010_output && (params->force_output_8_bits || params->bigendian_output)) ||
      (params->force_output_8_bits && params->bigendian_output)) {
    fprintf(stderr,
                  "ERROR: options -f or -Epbe and -Ep010 are mutually "
                  "exclusive!\n");
    return 1;
  }

  params->in_file_name = argv[optind];
  if (ResolveOverlap(params)) return 1;

  if (params->p010_output && (params->hw_format != DEC_OUT_FRM_RASTER_SCAN && !flag_d)) {
    fprintf(stderr,
          "-Ep010 option is ignored when neither raster scan output (-Ers) nor down scale output (-d) is enabled.\n\n");
    params->p010_output = 0;
  }
  return 0;
}

int ResolveOverlap(struct TestParams* params) {
  if (params->format == DEC_OUT_FRM_TILED_4X4 &&
      params->hw_format == DEC_OUT_FRM_RASTER_SCAN) {
    fprintf(stderr,
            "Overriding hw_format to tiled 4x4 as the requested "
            "output is tiled 4x4. (i.e. '-Ers' or '-Ep010' option ignored)\n");
    params->hw_format = DEC_OUT_FRM_TILED_4X4;
  }

  if (params->format != DEC_OUT_FRM_TILED_4X4 &&
      params->hw_format == DEC_OUT_FRM_TILED_4X4 &&
      !params->compress_bypass) {
    fprintf(stderr,
            "Disable conversion from compressed tiled 4x4 to Semi_Planar/Planar "
            "when hw_format is tiled 4x4 and compression is enabled.\n\n");
    params->format = DEC_OUT_FRM_TILED_4X4;
  }

  return 0;
}
