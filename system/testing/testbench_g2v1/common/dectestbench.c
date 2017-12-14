/* Copyright 2013 Google Inc. All Rights Reserved. */

/* Command line interface for G2 decoder. */

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fr-lib.h>

#include "software/source/common/fifo.h"
#include "software/source/common/regdrv.h"
#include "software/source/inc/basetype.h"
#include "software/source/inc/decapi.h"
#include "software/source/inc/dwl.h"
#include "software/test/common/bytestream_parser.h"
#include "software/test/common/command_line_parser.h"
#include "software/test/common/error_simulator.h"
#include "software/test/common/file_sink.h"
#include "software/test/common/md5_sink.h"
#include "software/test/common/null_sink.h"
#ifdef SDL_ENABLED
#include "software/test/common/sdl_sink.h"
#endif
#include "software/test/common/swhw/tb_cfg.h"
#include "software/test/common/swhw/trace.h"
#include "software/test/common/trace_hooks.h"
#include "software/test/common/vpxfilereader.h"
#include "software/test/common/yuvfilters.h"

#define NUM_OF_STREAM_BUFFERS (MAX_ASIC_CORES + 1)
#define DEFAULT_STREAM_BUFFER_SIZE (1024 * 1024)

#define DEBUG_PRINT(str) printf str

#define NAME_MAX    64
/* Generic yuv writing interface. */
typedef const void* YuvsinkOpenFunc(const char* fname);
typedef void YuvsinkWritePictureFunc(const void* inst, struct DecPicture pic);
typedef void YuvsinkCloseFunc(const void* inst);
typedef struct YuvSink_ {
  const void* inst;
  YuvsinkOpenFunc* open;
  YuvsinkWritePictureFunc* WritePicture;
  YuvsinkCloseFunc* close;
} YuvSink;

struct Client {
  Demuxer demuxer; /* Demuxer instance. */
  DecInst decoder; /* Decoder instance. */
  YuvSink yuvsink; /* Yuvsink instance. */
  const void* dwl; /* OS wrapper layer instance and function pointers. */
  sem_t dec_done;  /* Semaphore which is signalled when decoding is done. */
  struct DecInput buffers[NUM_OF_STREAM_BUFFERS]; /* Stream buffers. */
  u32 num_of_output_pics; /* Counter for number of pictures decoded. */
  FifoInst pic_fifo;      /* Picture FIFO to parallelize output. */
  pthread_t parallel_output_thread; /* Parallel output thread handle. */
  struct TestParams test_params;    /* Parameters for the testing. */
  u8 eos; /* Flag telling whether client has encountered end of stream. */
  u32 cycle_count; /* Sum of average cycles/mb counts */
};

/* Client actions */
static void DispatchBufferForDecoding(struct Client* client,
                                      struct DecInput* buffer);
static void DispatchEndOfStream(struct Client* client);
static void PostProcessPicture(struct Client* client,
                               struct DecPicture* picture);

/* Callbacks from decoder. */
static void InitializedCb(ClientInst inst);
static void HeadersDecodedCb(ClientInst inst,
                             struct DecSequenceInfo sequence_info);
static void BufferDecodedCb(ClientInst inst, struct DecInput* buffer);
static void PictureReadyCb(ClientInst inst, struct DecPicture picture);
static void EndOfStreamCb(ClientInst inst);
static void ReleasedCb(ClientInst inst);
static void NotifyErrorCb(ClientInst inst, u32 pic_id, enum DecRet rv);

/* Output thread functionality. */
static void* ParallelOutput(void* arg); /* Output loop. */

/* Helper functionality. */
static const void* CreateDemuxer(struct Client* client);
static void ReleaseDemuxer(struct Client* client);
static const void* CreateSink(struct Client* client);
static void ReleaseSink(struct Client* client);

static u8 ErrorSimulationNeeded(struct Client* client);
static i32 GetStreamBufferCount(struct Client* client);

/* Pointers to the DWL functionality. */
struct DWL dwl = {G2DWLReserveHw,          /* ReserveHw */
                  G2DWLReserveHwPipe,      /* ReserveHwPipe */
                  G2DWLReleaseHw,          /* ReleaseHw */
                  G2DWLMallocLinear,       /* MallocLinear */
                  G2DWLFreeLinear,         /* FreeLinear */
                  G2DWLWriteReg,           /* WriteReg */
                  G2DWLReadReg,            /* ReadReg */
                  G2DWLEnableHw,           /* EnableHw */
                  G2DWLDisableHw,          /* DisableHw */
                  G2DWLWaitHwReady,        /* WaitHwReady */
                  G2DWLSetIRQCallback,     /* SetIRQCallback */
                  malloc,                /* malloc */
                  free,                  /* free */
                  calloc,                /* calloc */
                  memcpy,                /* memcpy */
                  memset,                /* memset */
                  pthread_create,        /* pthread_create */
                  pthread_exit,          /* pthread_exit */
                  pthread_join,          /* pthread_join */
                  pthread_mutex_init,    /* pthread_mutex_init */
                  pthread_mutex_destroy, /* pthread_mutex_destroy */
                  pthread_mutex_lock,    /* pthread_mutex_lock */
                  pthread_mutex_unlock,  /* pthread_mutex_unlock */
                  pthread_cond_init,     /* pthread_cond_init */
                  pthread_cond_destroy,  /* pthread_cond_destroy */
                  pthread_cond_wait,     /* pthread_cond_wait */
                  pthread_cond_signal,   /* pthread_cond_signal */
                  printf,                /* printf */
};

static char out_file_name_buffer[NAME_MAX];

/* (Unfortunate) Hack globals to carry around data for model. */
struct TBCfg tb_cfg;
u32 b_frames = 0;
u32 test_case_id = 0;
static void OpenTestHooks(struct Client* client);
static void CloseTestHooks(struct Client* client);
static int HwconfigOverride(DecInst dec_inst, struct TBCfg* tbcfg);



int main(int argc, char* argv[]) {
	fr_init();
  struct Client client;
  memset(&client, 0, sizeof(struct Client));
  struct DecClientHandle client_if = {
      &client,        InitializedCb, HeadersDecodedCb, BufferDecodedCb,
      PictureReadyCb, EndOfStreamCb, ReleasedCb,       NotifyErrorCb, };

  printf("\n_hantro G2 decoder command line interface, SetupDefaultParams\n\n");
  printf("\n****** STEP 0: SetupDefaultParams\n");
  SetupDefaultParams(&client.test_params);
#if 1
  if (argc < 2) {
    PrintUsage(argv[0]);
    return 0;
  }
#endif
  if (ParseParams(argc, argv, &client.test_params)) {
    printf("Failed to parse params.\n\n");
    PrintUsage(argv[0]);
    return 1;
  }

  printf("\n****** STEP 1: OpenTestHooks\n");
  OpenTestHooks(&client);

  struct DecSwHwBuild build = DecGetBuild();
  printf("Hardware build: 0x%x, Software build: 0x%x\n", build.hw_build, build.sw_build);
  printf("VP9 %s\n", build.hw_config.vp9_support ? "enabled" : "disabled");
  printf("HEVC %s\n", build.hw_config.hevc_support ? "enabled" : "disabled");
  printf("PP %s\n", build.hw_config.pp_support ? "enabled" : "disabled");
  if (!build.hw_config.max_dec_pic_width ||
      !build.hw_config.max_dec_pic_height) {
    printf("Maximum supported picture width, height unspecified");
    printf(" (hw config reports %d, %d)\n\n", build.hw_config.max_dec_pic_width,
           build.hw_config.max_dec_pic_height);
  } else
    printf("Maximum supported picture width %d, height %d\n\n",
           build.hw_config.max_dec_pic_width,
           build.hw_config.max_dec_pic_height);

  printf("\n****** STEP 2: CreateDemuxer\n");
  client.demuxer.inst = CreateDemuxer(&client);
  if (client.demuxer.inst == NULL) {
    printf("Failed to open demuxer (file missing or of wrong type?)\n");
    return -1;
  }

  /* Create struct DWL. */
  printf("\n****** STEP 3: G2DWLInit\n");
  struct G2DWLInitParam dwl_params = {DWL_CLIENT_TYPE_HEVC_DEC};
  client.dwl = G2DWLInit(&dwl_params);

  if (client.test_params.extra_output_thread){
    printf("extra_output_thread\n");
    /* Create the fifo to enable parallel output processing */
    FifoInit(2, &client.pic_fifo);
    }

  sem_init(&client.dec_done, 0, 0);

  enum DecCodec codec;
  switch (client.demuxer.GetVideoFormat(client.demuxer.inst)) {
    case BITSTREAM_HEVC:
      codec = DEC_HEVC;
      break;
    case BITSTREAM_VP9:
      codec = DEC_VP9;
      if (client.test_params.read_mode == STREAMREADMODE_FULLSTREAM) {
        fprintf(stderr, "Full-stream (-F) is not supported in VP9.\n");
        return -1;
      }
      if (client.test_params.disable_display_order) {
        fprintf(stderr, "Disable display reorder (-R) is not supported in VP9.\n");
        return -1;
      }
      break;
    default:
      return -1;
  }

  struct DecConfig config;
  config.disable_picture_reordering = client.test_params.disable_display_order;
  config.concealment_mode = client.test_params.concealment_mode;
  switch (client.test_params.hw_format) {
    case DEC_OUT_FRM_TILED_4X4:
      config.output_format = DEC_OUT_FRM_TILED_4X4;
      break;
    case G2DEC_OUT_FRM_RASTER_SCAN: /* fallthrough */
    case DEC_OUT_FRM_PLANAR_420:
      if (!build.hw_config.pp_support) {
        fprintf(stderr, "Cannot do raster output; No PP support.\n");
        return -1;
      }
      config.output_format = G2DEC_OUT_FRM_RASTER_SCAN;
      break;
    default:
      return -1;
  }
  printf("\n****** STEP 4 Configuring hardware to output: %s\n",
         config.output_format == G2DEC_OUT_FRM_RASTER_SCAN
             ? "Semiplanar YCbCr 4:2:0 (four_cc 'NV12')"
             : "4x4 tiled YCbCr 4:2:0");
  config.dwl = dwl;
  config.dwl_inst = client.dwl;
  config.max_num_pics_to_decode = client.test_params.num_of_decoded_pics;
  config.force_output_8_bits = client.test_params.force_output_8_bits;
#ifdef DOWN_SCALER
  config.dscale_cfg = client.test_params.dscale_cfg;
#endif
  /* Initialize the decoder. */
  printf("\n****** STEP 5 DecInit\n");
  if (DecInit(codec, &client.decoder, config, client_if) != DEC_OK) {
    return -1;
  }

  /* The rest is driven by the callbacks and this thread just has to Wait
   * until decoder has finished it's job. */
  sem_wait(&client.dec_done);

  ReleaseDemuxer(&client);
  if (client.pic_fifo != NULL) FifoRelease(client.pic_fifo);
  if (client.dwl != NULL) G2DWLRelease(client.dwl);

  CloseTestHooks(&client);

  return 0;
}

void DispatchBufferForDecoding(struct Client* client,
                                      struct DecInput* buffer) {
  enum DecRet rv;
  i32 size = buffer->buffer.size;
  memset(buffer->buffer.virtual_address, 0, size);
  i32 len = client->demuxer.ReadPacket(
      client->demuxer.inst, (u8*)buffer->buffer.virtual_address, &size);
  if (len <= 0) {/* EOS or error. */
    /* If reading was due to insufficient buffer size, try to realloc with
       sufficient size. */
    if (size > buffer->buffer.size) {
      i32 i;
      DEBUG_PRINT(("Trying to reallocate buffer to fit next buffer.\n"));
      for (i = 0; i < GetStreamBufferCount(client); i++) {
        if (client->buffers[i].buffer.virtual_address ==
            buffer->buffer.virtual_address) {
          G2DWLFreeLinear(client->dwl, &client->buffers[i].buffer);
          if (G2DWLMallocLinear(client->dwl, size, &client->buffers[i].buffer)) {
            fprintf(stderr, "No memory available for the stream buffer\n");
            DispatchEndOfStream(client);
            return;
          }
          DispatchBufferForDecoding(client, &client->buffers[i]);
          return;
        }
      }
    } else {
      DispatchEndOfStream(client);
      return;
    }
  }
  buffer->data_len = len;
  if (client->eos) return; /* Don't dispatch new buffers if EOS already done. */

  /* Decode the contents of the input stream buffer. */
  
  if(0)printf("DispatchBufferForDecoding -> DecDecode. read size:%d, data:%.8x %.8x \n", 
    len, *(buffer->buffer.virtual_address), *(buffer->buffer.virtual_address+1));
  switch (rv = DecDecode(client->decoder, buffer)) {
    case DEC_OK:
      /* Everything is good, keep on going. */
      break;
    default:
      fprintf(stderr, "UNKNOWN ERROR! %d\n", rv);
      DispatchEndOfStream(client);
      break;
  }
}

static void DispatchEndOfStream(struct Client* client) {
  if (!client->eos) {
    client->eos = 1;
    DecEndOfStream(client->decoder);
  }
}

static void InitializedCb(ClientInst inst) {
  struct Client* client = (struct Client*)inst;
  /* Internal testing feature: Override HW configuration parameters */
  HwconfigOverride(client->decoder, &tb_cfg);

  /* Start the output handling thread, if needed. */
  if (client->test_params.extra_output_thread)
    pthread_create(&client->parallel_output_thread, NULL, ParallelOutput,
                   client);
  for (int i = 0; i < GetStreamBufferCount(client); i++) {
    if (G2DWLMallocLinear(client->dwl, DEFAULT_STREAM_BUFFER_SIZE,
                        &client->buffers[i].buffer)) {
      fprintf(stderr, "No memory available for the stream buffer\n");
      DecRelease(client->decoder);
      return;
    }
    /* Dispatch the first buffers for decoding. When decoder finished
     * decoding each buffer it will be refilled within the callback. */
    printf("\tCBBBBB: InitializedCb[%d] -> DispatchBufferForDecoding\n", i);
    DispatchBufferForDecoding(client, &client->buffers[i]);
  }
}

static void HeadersDecodedCb(ClientInst inst, struct DecSequenceInfo info) {
  struct Client* client = (struct Client*)inst;
  DEBUG_PRINT(
      ("Headers: Width %d Height %d\n", info.pic_width, info.pic_height));
  DEBUG_PRINT(
      ("Headers: Cropping params: (%d, %d) %dx%d\n",
       info.crop_params.crop_left_offset, info.crop_params.crop_top_offset,
       info.crop_params.crop_out_width, info.crop_params.crop_out_height));
  DEBUG_PRINT(("Headers: MonoChrome = %d\n", info.is_mono_chrome));
  DEBUG_PRINT(("Headers: Pictures in DPB = %d\n", info.num_of_ref_frames));
  DEBUG_PRINT(("Headers: video_range %d\n", info.video_range));
  DEBUG_PRINT(("Headers: matrix_coefficients %d\n", info.matrix_coefficients));

  if (client->yuvsink.inst == NULL) {
    if (client->test_params.out_file_name == NULL) {
      u32 w = client->test_params.display_cropped
                  ? info.crop_params.crop_out_width
                  : info.pic_width;
      u32 h = client->test_params.display_cropped
                  ? info.crop_params.crop_out_height
                  : info.pic_height;
      char* fourcc[] = {"tiled4x4", "nv12", "iyuv"};
      client->test_params.out_file_name = out_file_name_buffer;
#ifndef DOWN_SCALER
      sprintf(client->test_params.out_file_name, "out_w%dh%d_%s.yuv", w, h,
              fourcc[client->test_params.format]);
#else
      if (client->test_params.dscale_cfg.down_scale_x == 1
        && client->test_params.dscale_cfg.down_scale_y == 1)
        sprintf(client->test_params.out_file_name, "out_w%dh%d_%s.yuv", w, h,
              fourcc[client->test_params.format]);
      else {
        u32 dscale_shift[9] = {0, 0, 1, 0, 2, 0, 0, 0, 3};
        u32 dscale_x_shift = dscale_shift[client->test_params.dscale_cfg.down_scale_x];
        u32 dscale_y_shift = dscale_shift[client->test_params.dscale_cfg.down_scale_y];
        w = ((info.crop_params.crop_out_width / 2) >> dscale_x_shift) << 1;
        h = ((info.crop_params.crop_out_height / 2) >> dscale_y_shift) << 1;
        sprintf(client->test_params.out_file_name, "out_w%dh%d_%s.yuv", w, h,
              fourcc[client->test_params.format]);
      }
#endif
    }
    if ((client->yuvsink.inst = CreateSink(client)) == NULL) {
      fprintf(stderr, "Failed to create YUV sink\n");
      DispatchEndOfStream(client);
    }
  }

  /* TODO(vmr): free possibly existing buffers and allocate new buffers. */
  printf("\n\tCBBBBB: HeadersDecodedCb ->DecSetPictureBuffers\n");
  DecSetPictureBuffers(client->decoder, NULL, 0);
}

static void BufferDecodedCb(ClientInst inst, struct DecInput* buffer) {
  struct Client* client = (struct Client*)inst;
  if (!client->eos) {
    printf("\n\tCBBBBB: BufferDecodedCb -> DispatchBufferForDecoding\n");
    DispatchBufferForDecoding(client, buffer);
  }
}

static void PictureReadyCb(ClientInst inst, struct DecPicture picture) {
  
  static char* pic_types[] = {"        IDR", "Non-IDR (P)", "Non-IDR (B)"};
  struct Client* client = (struct Client*)inst;
  client->num_of_output_pics++;
  DEBUG_PRINT(("PIC %2d/%2d, type %s,", client->num_of_output_pics,
               picture.picture_info.pic_id,
               pic_types[picture.picture_info.pic_coding_type]));
  if (picture.picture_info.cycles_per_mb) {
    client->cycle_count += picture.picture_info.cycles_per_mb;
    DEBUG_PRINT((" %4d cycles / mb,", picture.picture_info.cycles_per_mb));
  }
  DEBUG_PRINT((" %d x %d, Crop: (%d, %d), %d x %d %s\n",
               picture.sequence_info.pic_width,
               picture.sequence_info.pic_height,
               picture.sequence_info.crop_params.crop_left_offset,
               picture.sequence_info.crop_params.crop_top_offset,
               picture.sequence_info.crop_params.crop_out_width,
               picture.sequence_info.crop_params.crop_out_height,
               picture.picture_info.is_corrupted ? "CORRUPT" : ""));
  if (client->test_params.extra_output_thread) {
    struct DecPicture* copy = malloc(sizeof(struct DecPicture));
    *copy = picture;
    printf("\tPictureReadyCb->FifoPush\n");
    FifoPush(client->pic_fifo, copy, FIFO_EXCEPTION_DISABLE);
  } else {
    printf("\n\tCBBBBB: PictureReadyCb->PostProcessPicture\n");
    PostProcessPicture(client, &picture);
  }
}

static void EndOfStreamCb(ClientInst inst) {
  struct Client* client = (struct Client*)inst;
  client->eos = 1;

  if (client->test_params.extra_output_thread) {
    /* We're done, wait for the output to Finish it's job. */
    FifoPush(client->pic_fifo, NULL, FIFO_EXCEPTION_DISABLE);
    pthread_join(client->parallel_output_thread, NULL);
  }
  DecRelease(client->decoder);
}

static void ReleasedCb(ClientInst inst) {
  struct Client* client = (struct Client*)inst;
  for (int i = 0; i < NUM_OF_STREAM_BUFFERS; i++) {
    if (client->buffers[i].buffer.virtual_address) {
      G2DWLFreeLinear(client->dwl, &client->buffers[i].buffer);
    }
  }
  if(client->cycle_count && client->num_of_output_pics)
    DEBUG_PRINT(("\nAverage cycles/MB: %4d", client->cycle_count/client->num_of_output_pics));
  ReleaseSink(client);
  sem_post(&client->dec_done);
}

static void NotifyErrorCb(ClientInst inst, u32 pic_id, enum DecRet rv) {
  struct Client* client = (struct Client*)inst;
  fprintf(stderr, "Decoding error on pic_id %d: %d\n", pic_id, rv);
  /* There's serious decoding error, so we'll consider it as end of stream to
     get the pending pictures out of the decoder. */
  DispatchEndOfStream(client);
}

static void PostProcessPicture(struct Client* client,
                               struct DecPicture* picture) {
  struct DecPicture copy;
  struct DecPicture in = *picture;
  u32 pic_size;

  if (client->test_params.sink_type == SINK_NULL) goto PIC_CONSUMED;

#ifdef DOWN_SCALER
  if (client->test_params.dscale_cfg.down_scale_x != 1 &&
      client->test_params.dscale_cfg.down_scale_y != 1) {
    in.sequence_info.pic_width = in.dscale_width;
    in.sequence_info.pic_height = in.dscale_height;
    in.sequence_info.pic_stride = in.dscale_stride;

    in.g2_luma = picture->dscale_luma;
    in.g2_chroma = picture->dscale_chroma;
  }
#endif
#ifndef NO_TB_PP
  copy = in;
  pic_size = in.sequence_info.pic_width *
          in.sequence_info.pic_height * 2;  // one pixel in 16 bits

  copy.g2_luma.virtual_address = malloc(pic_size);
  copy.g2_chroma.virtual_address = malloc(pic_size / 2);

  YuvfilterConvertPixel16Bits(&in, &copy);

#if 0
  if (client->test_params.display_cropped ||
      client->test_params.format != DEC_OUT_FRM_TILED_4X4) {
    u32 pic_size =
        picture->sequence_info.pic_width * picture->sequence_info.pic_height;
    copy.luma.virtual_address = malloc(pic_size);
    memcpy(copy.luma.virtual_address, picture->luma.virtual_address,
           pic_size);
    copy.chroma.virtual_address = malloc(pic_size / 2);
    memcpy(copy.chroma.virtual_address, picture->chroma.virtual_address,
           pic_size / 2);
  }
#endif

  if (in.picture_info.format == DEC_OUT_FRM_TILED_4X4) {
    if (client->test_params.display_cropped &&
        (client->test_params.format == DEC_OUT_FRM_TILED_4X4)) {
      YuvfilterTiledcrop(&copy);
    } else if (client->test_params.format == G2DEC_OUT_FRM_RASTER_SCAN) {
      YuvfilterTiled2Semiplanar(&copy);
      if (client->test_params.display_cropped) YuvfilterSemiplanarcrop(&copy);
    } else if (client->test_params.format == DEC_OUT_FRM_PLANAR_420) {
      if (client->test_params.display_cropped) {
        YuvfilterTiled2Semiplanar(&copy);
        YuvfilterSemiplanarcrop(&copy);
        YuvfilterSemiplanar2Planar(&copy);
      } else {
        YuvfilterTiled2Planar(&copy);
      }
    }
  } else if (in.picture_info.format ==
             G2DEC_OUT_FRM_RASTER_SCAN) {
    if (client->test_params.display_cropped) YuvfilterSemiplanarcrop(&copy);
    if (client->test_params.format == DEC_OUT_FRM_PLANAR_420)
      YuvfilterSemiplanar2Planar(&copy);
  }

  YuvfilterPrepareOutput(&copy);
  client->yuvsink.WritePicture(client->yuvsink.inst, copy);

  free(copy.g2_luma.virtual_address);
  free(copy.g2_chroma.virtual_address);
#else
  client->yuvsink.WritePicture(client->yuvsink.inst, in);
#endif

PIC_CONSUMED:
  DecPictureConsumed(client->decoder, *picture);
}

static void* ParallelOutput(void* arg) {
  struct Client* client = arg;
  struct DecPicture* pic = NULL;
  do {
    FifoPop(client->pic_fifo, (void**)&pic, FIFO_EXCEPTION_DISABLE);
    if (pic == NULL) {
      printf(("END-OF-STREAM received in ParallelOutput thread\n"));
      return NULL;
    }
    PostProcessPicture(client, pic);
  } while (1);
  return NULL;
}

static const void* CreateDemuxer(struct Client* client) {
  Demuxer demuxer;
  enum FileFormat ff = client->test_params.file_format;
  if (ff == FILEFORMAT_AUTO_DETECT) {
    if (strstr(client->test_params.in_file_name, ".ivf") ||
        strstr(client->test_params.in_file_name, ".vp9"))
      ff = FILEFORMAT_IVF;
    else if (strstr(client->test_params.in_file_name, ".webm"))
      ff = FILEFORMAT_WEBM;
    else if (strstr(client->test_params.in_file_name, ".hevc"))
      ff = FILEFORMAT_BYTESTREAM;
  }
  if (ff == FILEFORMAT_IVF || ff == FILEFORMAT_WEBM) {
    demuxer.open = VpxRdrOpen;
    demuxer.GetVideoFormat = VpxRdrIdentifyFormat;
    demuxer.ReadPacket = VpxRdrReadFrame;
    demuxer.close = VpxRdrClose;
  } else if (ff == FILEFORMAT_BYTESTREAM) {
    demuxer.open = ByteStreamParserOpen;
    demuxer.GetVideoFormat = ByteStreamParserIdentifyFormat;
    demuxer.ReadPacket = ByteStreamParserReadFrame;
    demuxer.close = ByteStreamParserClose;
  } else {
    /* TODO(vmr): In addition to file suffix, consider also looking into
     *            shebang of the files. */
    return NULL;
  }

  demuxer.inst = demuxer.open(client->test_params.in_file_name,
                              client->test_params.read_mode);
  /* If needed, instantiate error simulator to process data from demuxer. */
  if (ErrorSimulationNeeded(client))
    demuxer.inst =
        ErrorSimulatorInject(&demuxer, client->test_params.error_sim);
  client->demuxer = demuxer;
  return demuxer.inst;
}

static void ReleaseDemuxer(struct Client* client) {
  client->demuxer.close(client->demuxer.inst);
}

static const void* CreateSink(struct Client* client) {
  YuvSink yuvsink;
  switch (client->test_params.sink_type) {
    case SINK_FILE_SEQUENCE:
      yuvsink.open = FilesinkOpen;
      yuvsink.WritePicture = FilesinkWritePic;
      yuvsink.close = FilesinkClose;
      break;
    case SINK_FILE_PICTURE:
      yuvsink.open = NullsinkOpen;
      yuvsink.WritePicture = FilesinkWriteSinglePic;
      yuvsink.close = NullsinkClose;
      break;
    case SINK_MD5_SEQUENCE:
      yuvsink.open = Md5sinkOpen;
      yuvsink.WritePicture = Md5sinkWritePic;
      yuvsink.close = Md5sinkClose;
      break;
    case SINK_MD5_PICTURE:
      yuvsink.open = md5perpicsink_open;
      yuvsink.WritePicture = md5perpicsink_write_pic;
      yuvsink.close = md5perpicsink_close;
      break;
#ifdef SDL_ENABLED
    case SINK_SDL:
      yuvsink.open = SdlSinkOpen;
      yuvsink.WritePicture = SdlSinkWrite;
      yuvsink.close = SdlSinkClose;
      break;
#endif
    case SINK_NULL:
      yuvsink.open = NullsinkOpen;
      yuvsink.WritePicture = NullsinkWrite;
      yuvsink.close = NullsinkClose;
      break;
    default:
      assert(0);
  }
  yuvsink.inst = yuvsink.open(client->test_params.out_file_name);
  client->yuvsink = yuvsink;
  return client->yuvsink.inst;
}

static void ReleaseSink(struct Client* client) {
  if (client->yuvsink.inst) {
    client->yuvsink.close(client->yuvsink.inst);
  }
}

static u8 ErrorSimulationNeeded(struct Client* client) {
  return client->test_params.error_sim.corrupt_headers ||
         client->test_params.error_sim.truncate_stream ||
         client->test_params.error_sim.swap_bits_in_stream ||
         client->test_params.error_sim.lose_packets;
}

static i32 GetStreamBufferCount(struct Client* client) {
  return client->test_params.read_mode == STREAMREADMODE_FULLSTREAM
             ? 1
             : NUM_OF_STREAM_BUFFERS;
}

/* These global values are found from commonconfig.c */
extern u32 dec_stream_swap;
extern u32 dec_pic_swap;
extern u32 dec_dirmv_swap;
extern u32 dec_tab0_swap;
extern u32 dec_tab1_swap;
extern u32 dec_tab2_swap;
extern u32 dec_tab3_swap;
extern u32 dec_rscan_swap;
extern u32 dec_burst_length;
extern u32 dec_bus_width;
extern u32 dec_apf_treshold;
extern u32 dec_apf_disable;
extern u32 dec_clock_gating;
extern u32 dec_clock_gating_runtime;
extern u32 dec_ref_double_buffer;
extern u32 dec_timeout_cycles;
extern u32 dec_axi_id_rd;
extern u32 dec_axi_id_rd_unique_enable;
extern u32 dec_axi_id_wr;
extern u32 dec_axi_id_wr_unique_enable;

static void OpenTestHooks(struct Client* client) {
  /* set test bench configuration */
  TBSetDefaultCfg(&tb_cfg);
  FILE* f_tbcfg = fopen("tb.cfg", "r");
  if (f_tbcfg == NULL) {
    DEBUG_PRINT(("UNABLE TO OPEN INPUT FILE: \"tb.cfg\", use Default\n"));
  } else {
    fclose(f_tbcfg);
    if (TBParseConfig("tb.cfg", TBReadParam, &tb_cfg) == TB_FALSE) return;
    if (TBCheckCfg(&tb_cfg) != 0) return;
  }

  /* Read the error simulation parameters. */
  struct ErrorSimulationParams* error_params = &client->test_params.error_sim;
  error_params->seed = tb_cfg.tb_params.seed_rnd;
  error_params->truncate_stream = TBGetTBStreamTruncate(&tb_cfg);
  error_params->corrupt_headers = TBGetTBStreamHeaderCorrupt(&tb_cfg);
  if (strcmp(tb_cfg.tb_params.stream_bit_swap, "0") != 0) {
    error_params->swap_bits_in_stream = 1;
    memcpy(error_params->swap_bit_odds, tb_cfg.tb_params.stream_bit_swap,
           sizeof(tb_cfg.tb_params.stream_bit_swap));
  }
  if (strcmp(tb_cfg.tb_params.stream_packet_loss, "0") != 0) {
    error_params->lose_packets = 1;
    memcpy(error_params->packet_loss_odds, tb_cfg.tb_params.stream_packet_loss,
           sizeof(tb_cfg.tb_params.stream_packet_loss));
  }
  DEBUG_PRINT(("TB Seed Rnd %d\n", error_params->seed));
  DEBUG_PRINT(("TB Stream Truncate %d\n", error_params->truncate_stream));
  DEBUG_PRINT(("TB Stream Header Corrupt %d\n", error_params->corrupt_headers));
  DEBUG_PRINT(("TB Stream Bit Swap %d; odds %s\n",
               error_params->swap_bits_in_stream, error_params->swap_bit_odds));
  DEBUG_PRINT(("TB Stream Packet Loss %d; odds %s\n",
               error_params->lose_packets, error_params->packet_loss_odds));

  if (client->test_params.trace_target) tb_cfg.tb_params.extra_cu_ctrl_eof = 1;

  if (client->test_params.hw_traces) {
    if (!OpenTraceFiles())
      DEBUG_PRINT(
          ("UNABLE TO OPEN TRACE FILE(S) Do you have a trace.cfg "
           "file?\n"));
  }

  if (f_tbcfg != NULL) {
    dec_stream_swap = tb_cfg.dec_params.strm_swap;
    dec_pic_swap = tb_cfg.dec_params.pic_swap;
    dec_dirmv_swap = tb_cfg.dec_params.dirmv_swap;
    dec_tab0_swap = tb_cfg.dec_params.tab0_swap;
    dec_tab1_swap = tb_cfg.dec_params.tab1_swap;
    dec_tab2_swap = tb_cfg.dec_params.tab2_swap;
    dec_tab3_swap = tb_cfg.dec_params.tab3_swap;
    dec_rscan_swap = tb_cfg.dec_params.rscan_swap;
    dec_burst_length = tb_cfg.dec_params.max_burst;
    dec_bus_width = TBGetDecBusWidth(&tb_cfg);
    dec_apf_treshold = tb_cfg.dec_params.apf_threshold_value;
    dec_apf_disable = tb_cfg.dec_params.apf_disable;
    dec_clock_gating = TBGetDecClockGating(&tb_cfg);
    dec_clock_gating_runtime = TBGetDecClockGatingRuntime(&tb_cfg);
    dec_ref_double_buffer = tb_cfg.dec_params.ref_double_buffer_enable;
    dec_timeout_cycles = tb_cfg.dec_params.timeout_cycles;
    dec_axi_id_rd = tb_cfg.dec_params.axi_id_rd;
    dec_axi_id_rd_unique_enable = tb_cfg.dec_params.axi_id_rd_unique_enable;
    dec_axi_id_wr = tb_cfg.dec_params.axi_id_wr;
    dec_axi_id_wr_unique_enable = tb_cfg.dec_params.axi_id_wr_unique_enable;
    client->test_params.concealment_mode = TBGetDecErrorConcealment(&tb_cfg);
  }
  /* determine test case id from input file name (if contains "case_") */
  {
    char* pc, *pe;
    char in[256] = {0};
    strncpy(in, client->test_params.in_file_name,
            strnlen(client->test_params.in_file_name, 256));
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
}

static void CloseTestHooks(struct Client* client) {
  if (client->test_params.hw_traces) {
#ifdef ASIC_TRACE_SUPPORT
    TraceSequenceCtrl(client->num_of_output_pics, 0);
#endif
    CloseTraceFiles();
  }
}

static int HwconfigOverride(DecInst dec_inst, struct TBCfg* tbcfg) {
  u32 data_discard = TBGetDecDataDiscard(&tb_cfg);
  u32 latency_comp = tb_cfg.dec_params.latency_compensation;
  u32 output_picture_endian = TBGetDecOutputPictureEndian(&tb_cfg);
  u32 bus_burst_length = tb_cfg.dec_params.bus_burst_length;
  u32 asic_service_priority = tb_cfg.dec_params.asic_service_priority;
  u32 output_format = TBGetDecOutputFormat(&tb_cfg);
  u32 service_merge_disable = TBGetDecServiceMergeDisable(&tb_cfg);

  DEBUG_PRINT(("TBCfg: Decoder Data Discard %d\n", data_discard));
  DEBUG_PRINT(("TBCfg: Decoder Latency Compensation %d\n", latency_comp));
  DEBUG_PRINT(
      ("TBCfg: Decoder Output Picture Endian %d\n", output_picture_endian));
  DEBUG_PRINT(("TBCfg: Decoder Bus Burst Length %d\n", bus_burst_length));
  DEBUG_PRINT(
      ("TBCfg: Decoder Asic Service Priority %d\n", asic_service_priority));
  DEBUG_PRINT(("TBCfg: Decoder Output Format %d\n", output_format));
  DEBUG_PRINT(
      ("TBCfg: Decoder Service Merge Disable %d\n", service_merge_disable));

  /* TODO(vmr): Enable these, remove what's not needed, add what's needed.
  G2SetDecRegister(((struct HevcDecContainer *) dec_inst)->hevc_regs,
                 HWIF_DEC_LATENCY,
                 latency_comp);
  G2SetDecRegister(((struct HevcDecContainer *) dec_inst)->hevc_regs,
                 HWIF_DEC_CLK_GATE_E,
                 clock_gating);
  G2SetDecRegister(((struct HevcDecContainer *) dec_inst)->hevc_regs,
                 HWIF_DEC_OUT_TILED_E,
                 output_format);
  G2SetDecRegister(((struct HevcDecContainer *) dec_inst)->hevc_regs,
                 HWIF_DEC_OUT_ENDIAN,
                 output_picture_endian);
  G2SetDecRegister(((struct HevcDecContainer *) dec_inst)->hevc_regs,
                 HWIF_DEC_MAX_BURST,
                 bus_burst_length);
  G2SetDecRegister(((struct HevcDecContainer *) dec_inst)->hevc_regs,
                 HWIF_DEC_DATA_DISC_E,
                 data_discard);
  G2SetDecRegister(((struct HevcDecContainer *) dec_inst)->hevc_regs,
                 HWIF_SERV_MERGE_DIS,
                 service_merge_disable);
  */

  return 0;
}
