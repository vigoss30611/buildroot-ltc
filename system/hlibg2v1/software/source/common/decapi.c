/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/source/inc/decapi.h"

/*#include "software/source/common/decapi_trace.h" */
#include "software/source/common/fifo.h"
#include "software/source/common/sw_util.h"
#include "software/source/common/version.h"
#include "software/source/inc/dwl.h"
#ifdef ENABLE_HEVC_SUPPORT
#include "software/source/inc/hevcdecapi.h"
#endif /* ENABLE_HEVC_SUPPORT */
#ifdef ENABLE_VP9_SUPPORT
#include "software/source/inc/vp9decapi.h"
#endif /* ENABLE_VP9_SUPPORT */

#define MAX_FIFO_CAPACITY (3)

#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

enum DecodingState {
  DECODER_WAITING_HEADERS,
  DECODER_WAITING_RESOURCES,
  DECODER_DECODING,
  DECODER_SHUTTING_DOWN
};

struct DecOutput {
  u8* strm_curr_pos;
  addr_t strm_curr_bus_address;
  u32 data_left;
  u8* strm_buff;
  addr_t strm_buff_bus_address;
  u32 buff_size;
};

struct Command {
  enum {
    COMMAND_INIT,
    COMMAND_DECODE,
    COMMAND_SETBUFFERS,
    COMMAND_END_OF_STREAM,
    COMMAND_RELEASE
  } id;
  union {
    struct DecConfig config;
    struct DecInput input;
  } params;
};

struct DecoderWrapper {
  void* inst;
  enum DecRet (*init)(const void** inst, struct DecConfig config,
                      const void *dwl);
  enum DecRet (*GetInfo)(void* inst, struct DecSequenceInfo* info);
  enum DecRet (*Decode)(void* inst, struct DWLLinearMem input, struct DecOutput* output,
                      u8* stream, u32 strm_len, struct DWL dwl, u32 pic_id);
  enum DecRet (*NextPicture)(void* inst, struct DecPicture* pic,
                             struct DWL dwl);
  enum DecRet (*PictureConsumed)(void* inst, struct DecPicture pic,
                                 struct DWL dwl);
  enum DecRet (*EndOfStream)(void* inst);
  void (*Release)(void* inst);
#ifdef USE_EXTERNAL_BUFFER
  enum DecRet (*GetBufferInfo)(void *inst, struct DecBufferInfo *buf_info);
  enum DecRet (*AddBuffer)(void *inst, struct DWLLinearMem *buf);
#endif
  enum DecRet (*UseExtraFrmBuffers)(const void *inst, u32 n);
};

typedef struct DecoderInstance_ {
  struct DecoderWrapper dec;
  enum DecodingState state;
  FifoInst input_queue;
  pthread_t decode_thread;
  pthread_t output_thread;
  pthread_mutex_t cs_mutex;
  pthread_mutex_t eos_mutex;
  pthread_mutex_t resource_mutex;
  pthread_cond_t eos_cond;
  pthread_cond_t resource_cond;
  u8 eos_ready;
  u8 resources_acquired;
  struct Command* current_command;
  struct DecOutput buffer_status;
  struct DecClientHandle client;
  struct DecSequenceInfo sequence_info;
  struct DWL dwl;
  const void* dwl_inst;
  u8 pending_eos; /* TODO(vmr): slightly ugly, figure out better way. */
  u32 max_num_of_decoded_pics;
  u32 num_of_decoded_pics;
  struct DecInput prev_input;
  /* TODO(mheikkinen) this is a temporary handler for stream decoded callback
   * until HEVC gets delayed sync implementation */
  void (*stream_decoded)(void* inst);
} DecoderInstance;

/* Decode loop and handlers for different states. */
static void* DecodeLoop(void* arg);
static void Initialize(DecoderInstance* inst);
static void WaitForResources(DecoderInstance* inst);
static void Decode(DecoderInstance* inst);
static void EndOfStream(DecoderInstance* inst);
static void Release(DecoderInstance* inst);

/* Output loop. */
static void* OutputLoop(void* arg);

/* Local helpers to manage and protect the state of the decoder. */
static enum DecodingState GetState(DecoderInstance* inst);
static void SetState(DecoderInstance* inst, enum DecodingState state);

/* Hevc codec wrappers. */
#ifdef ENABLE_HEVC_SUPPORT
static enum DecRet HevcInit(const void** inst, struct DecConfig config,
                            const void *dwl);
static enum DecRet HevcGetInfo(void* inst, struct DecSequenceInfo* info);
static enum DecRet HevcDecode(void* inst, struct DWLLinearMem input, struct DecOutput* output,
                                    u8* stream, u32 strm_len, struct DWL dwl, u32 pic_id);
static enum DecRet HevcNextPicture(void* inst, struct DecPicture* pic,
                                   struct DWL dwl);
static enum DecRet HevcPictureConsumed(void* inst, struct DecPicture pic,
                                       struct DWL dwl);
static enum DecRet HevcEndOfStream(void* inst);
#ifdef USE_EXTERNAL_BUFFER
static enum DecRet HevcGetBufferInfo(void *inst, struct DecBufferInfo *buf_info);
static enum DecRet HevcAddBuffer(void *inst, struct DWLLinearMem *buf);
#endif


static void HevcRelease(void* inst);
static void HevcStreamDecoded(void* inst);
#endif /* ENABLE_HEVC_SUPPORT */

#ifdef ENABLE_VP9_SUPPORT
/* VP9 codec wrappers. */
static enum DecRet Vp9Init(const void** inst, struct DecConfig config,
                           const void *dwl);
static enum DecRet Vp9GetInfo(void* inst, struct DecSequenceInfo* info);
static enum DecRet Vp9Decode(void* inst, struct DWLLinearMem input, struct DecOutput* output,
                                  u8* stream, u32 strm_len, struct DWL dwl, u32 pic_id);
static enum DecRet Vp9NextPicture(void* inst, struct DecPicture* pic,
                                  struct DWL dwl);
static enum DecRet Vp9PictureConsumed(void* inst, struct DecPicture pic,
                                      struct DWL dwl);
static enum DecRet Vp9EndOfStream(void* inst);
#ifdef USE_EXTERNAL_BUFFER
static enum DecRet Vp9GetBufferInfo(void *inst, struct DecBufferInfo *buf_info);
static enum DecRet Vp9AddBuffer(void *inst, struct DWLLinearMem *buf);
#endif
static void Vp9Release(void* inst);
static void Vp9StreamDecoded(void* inst);
#endif /* ENABLE_VP9_SUPPORT */

struct DecSwHwBuild DecGetBuild(void) {
  struct DecSwHwBuild build;

  (void)DWLmemset(&build, 0, sizeof(build));

  build.sw_build = HANTRO_DEC_SW_BUILD;
  build.hw_build = DWLReadAsicID();

  DWLReadAsicConfig(&build.hw_config);

  return build;
}

enum DecRet DecInit(enum DecCodec codec, DecInst* decoder,
                    struct DecConfig config, struct DecClientHandle client) {
  if (decoder == NULL || client.Initialized == NULL ||
      client.HeadersDecoded == NULL || client.BufferDecoded == NULL ||
      client.PictureReady == NULL || client.EndOfStream == NULL ||
      client.Released == NULL || client.NotifyError == NULL) {
    return DEC_PARAM_ERROR;
  }

  DecoderInstance* inst = config.dwl.calloc(1, sizeof(DecoderInstance));
  if (inst == NULL) return DEC_MEMFAIL;
  inst->dwl = config.dwl;
  inst->dwl_inst = config.dwl_inst;
  if (FifoInit(MAX_FIFO_CAPACITY, &inst->input_queue) != FIFO_OK) {
    inst->dwl.free(inst);
    return DEC_MEMFAIL;
  }
  inst->client = client;
  inst->dwl.pthread_mutex_init(&inst->cs_mutex, NULL);
  inst->dwl.pthread_mutex_init(&inst->resource_mutex, NULL);
  inst->dwl.pthread_cond_init(&inst->resource_cond, NULL);
  inst->dwl.pthread_mutex_init(&inst->eos_mutex, NULL);
  inst->dwl.pthread_cond_init(&inst->eos_cond, NULL);
  inst->eos_ready = 0;
  inst->resources_acquired = 0;
  inst->dwl.pthread_create(&inst->decode_thread, NULL, DecodeLoop, inst);
  pthread_detach(inst->decode_thread);
  switch (codec) {
#ifdef ENABLE_HEVC_SUPPORT
    case DEC_HEVC:
      inst->dec.init = HevcInit;
      inst->dec.GetInfo = HevcGetInfo;
      inst->dec.Decode = HevcDecode;
      inst->dec.NextPicture = HevcNextPicture;
      inst->dec.PictureConsumed = HevcPictureConsumed;
      inst->dec.EndOfStream = HevcEndOfStream;
      inst->dec.Release = HevcRelease;
      inst->dec.UseExtraFrmBuffers = HevcDecUseExtraFrmBuffers;
#ifdef USE_EXTERNAL_BUFFER
      inst->dec.GetBufferInfo = HevcGetBufferInfo;
      inst->dec.AddBuffer = HevcAddBuffer;
#endif
      inst->stream_decoded = HevcStreamDecoded;
      break;
#endif
#ifdef ENABLE_VP9_SUPPORT
    case DEC_VP9:
      inst->dec.init = Vp9Init;
      inst->dec.GetInfo = Vp9GetInfo;
      inst->dec.Decode = Vp9Decode;
      inst->dec.NextPicture = Vp9NextPicture;
      inst->dec.PictureConsumed = Vp9PictureConsumed;
      inst->dec.EndOfStream = Vp9EndOfStream;
      inst->dec.Release = Vp9Release;
      inst->dec.UseExtraFrmBuffers = Vp9DecUseExtraFrmBuffers;
      inst->stream_decoded = Vp9StreamDecoded;
#ifdef USE_EXTERNAL_BUFFER
      inst->dec.GetBufferInfo = Vp9GetBufferInfo;
      inst->dec.AddBuffer = Vp9AddBuffer;
#endif
      break;
#endif
    default:
      return DEC_FORMAT_NOT_SUPPORTED;
  }
  SetState(inst, DECODER_WAITING_HEADERS);
  *decoder = inst;
  struct Command* command = inst->dwl.calloc(1, sizeof(struct Command));
  command->id = COMMAND_INIT;
  command->params.config = config;
  FifoPush(inst->input_queue, command, FIFO_EXCEPTION_DISABLE);
  return DEC_OK;
}

enum DecRet DecDecode(DecInst dec_inst, struct DecInput* input) {
  DecoderInstance* inst = (DecoderInstance*)dec_inst;
  if (dec_inst == NULL || input == NULL || input->data_len == 0 ||
      input->buffer.virtual_address == NULL || input->buffer.bus_address == 0) {
    return DEC_PARAM_ERROR;
  }

  switch (GetState(inst)) {
    case DECODER_WAITING_HEADERS:
    case DECODER_WAITING_RESOURCES:
    case DECODER_DECODING:
    case DECODER_SHUTTING_DOWN: {
      struct Command* command = inst->dwl.calloc(1, sizeof(struct Command));
      command->id = COMMAND_DECODE;
      inst->dwl.memcpy(&command->params.input, input, sizeof(struct DecInput));
      FifoPush(inst->input_queue, command, FIFO_EXCEPTION_DISABLE);
      return DEC_OK;
    }
    default:
      return DEC_NOT_INITIALIZED;
  }
}

#ifdef USE_EXTERNAL_BUFFER
enum DecRet DecGetPictureBuffersInfo(DecInst dec_inst, struct DecBufferInfo *info) {
  DecoderInstance* inst = (DecoderInstance*)dec_inst;

  if (dec_inst == NULL || info == NULL) {
    return DEC_PARAM_ERROR;
  }

  return (inst->dec.GetBufferInfo(inst->dec.inst, info));
}
#endif
enum DecRet DecSetPictureBuffers(DecInst dec_inst,
                                 const struct DWLLinearMem* buffers,
                                 u32 num_of_buffers) {
  enum DecRet ret = DEC_OK;
  /* TODO(vmr): Enable checks once we have implementation in place. */
  /* if (dec_inst == NULL || buffers == NULL || num_of_buffers == 0)
  {
      return DEC_PARAM_ERROR;
  } */
#ifdef USE_EXTERNAL_BUFFER
  if (dec_inst == NULL || buffers == NULL || num_of_buffers == 0)
  {
      return DEC_PARAM_ERROR;
  }
#endif
  DecoderInstance* inst = (DecoderInstance*)dec_inst;
  /* TODO(vmr): Check the buffers and set them, if they're good. */
  inst->dwl.pthread_mutex_lock(&inst->resource_mutex);
#ifdef USE_EXTERNAL_BUFFER
  for (int i = 0; i < num_of_buffers; i++) {
    ret = inst->dec.AddBuffer(inst->dec.inst, (struct DWLLinearMem *)&buffers[i]);

    /* TODO(min): Check return code ... */
  }
#endif
  inst->resources_acquired = 1;
  SetState(inst, DECODER_DECODING);
  inst->dwl.pthread_cond_signal(&inst->resource_cond);
  inst->dwl.pthread_mutex_unlock(&inst->resource_mutex);
  return ret; //DEC_OK;
}

enum DecRet DecUseExtraFrmBuffers(DecInst dec_inst, u32 n) {
  enum DecRet ret = DEC_OK;

  if (dec_inst == NULL)
    return DEC_PARAM_ERROR;

  DecoderInstance* inst = (DecoderInstance*)dec_inst;

  inst->dec.UseExtraFrmBuffers(inst->dec.inst, n);
  return ret;
}

enum DecRet DecPictureConsumed(DecInst dec_inst, struct DecPicture picture) {
  if (dec_inst == NULL) {
    return DEC_PARAM_ERROR;
  }
  DecoderInstance* inst = (DecoderInstance*)dec_inst;
  switch (GetState(inst)) {
    case DECODER_WAITING_HEADERS:
    case DECODER_WAITING_RESOURCES:
    case DECODER_DECODING:
    case DECODER_SHUTTING_DOWN:
      inst->dec.PictureConsumed(inst->dec.inst, picture, inst->dwl);
      return DEC_OK;
    default:
      return DEC_NOT_INITIALIZED;
  }
  return DEC_OK;
}

enum DecRet DecEndOfStream(DecInst dec_inst) {
  if (dec_inst == NULL) {
    return DEC_PARAM_ERROR;
  }
  DecoderInstance* inst = (DecoderInstance*)dec_inst;
  struct Command* command;
  switch (GetState(inst)) {
    case DECODER_WAITING_HEADERS:
    case DECODER_WAITING_RESOURCES:
    case DECODER_DECODING:
    case DECODER_SHUTTING_DOWN:
      command = inst->dwl.calloc(1, sizeof(struct Command));
      inst->dwl.memset(command, 0, sizeof(struct Command));
      command->id = COMMAND_END_OF_STREAM;
      FifoPush(inst->input_queue, command, FIFO_EXCEPTION_DISABLE);
      return DEC_OK;
    default:
      return DEC_INITFAIL;
  }
}

void DecRelease(DecInst dec_inst) {
  DecoderInstance* inst = (DecoderInstance*)dec_inst;
  /* If we are already shutting down, no need to do it more than once. */
  if (GetState(inst) == DECODER_SHUTTING_DOWN) return;
  /* Abort the current command (it may be long-lasting task). */
  SetState(inst, DECODER_SHUTTING_DOWN);
  struct Command* command = inst->dwl.calloc(1, sizeof(struct Command));
  inst->dwl.memset(command, 0, sizeof(struct Command));
  command->id = COMMAND_RELEASE;
  FifoPush(inst->input_queue, command, FIFO_EXCEPTION_DISABLE);
}

static void NextCommand(DecoderInstance* inst) {
  /* Read next command from stream, if we've finished the previous. */
  if (inst->current_command == NULL) {
    FifoPop(inst->input_queue, (void**)&inst->current_command,
            FIFO_EXCEPTION_DISABLE);
    if (inst->current_command != NULL &&
        inst->current_command->id == COMMAND_DECODE) {
      inst->buffer_status.strm_curr_pos =
          (u8*)inst->current_command->params.input.stream[0];
      inst->buffer_status.strm_curr_bus_address =
          inst->current_command->params.input.buffer.bus_address;
      inst->buffer_status.data_left =
          inst->current_command->params.input.data_len;
      inst->buffer_status.strm_buff =
          (u8*)inst->current_command->params.input.buffer.virtual_address;
      inst->buffer_status.strm_buff_bus_address =
          inst->current_command->params.input.buffer.bus_address;
      inst->buffer_status.buff_size =
          inst->current_command->params.input.buffer.size;
    }
  }
}

static void CommandCompleted(DecoderInstance* inst) {
  if (inst->current_command->id == COMMAND_DECODE) {
    inst->stream_decoded(inst);
  }
  inst->dwl.free(inst->current_command);
  inst->current_command = NULL;
}

static void* DecodeLoop(void* arg) {
  DecoderInstance* inst = (DecoderInstance*)arg;
  while (1) {
    NextCommand(inst);
    if (inst->current_command == NULL) /* NULL command means to quit. */
      return NULL;
    switch (GetState(inst)) {
      case DECODER_WAITING_HEADERS:
      case DECODER_DECODING:
        if (inst->current_command->id == COMMAND_INIT) {
          Initialize(inst);
        } else if (inst->current_command->id == COMMAND_DECODE) {
          Decode(inst);
        } else if (inst->current_command->id == COMMAND_END_OF_STREAM) {
          EndOfStream(inst);
        } else if (inst->current_command->id == COMMAND_RELEASE) {
          Release(inst);
          return NULL;
        }
        break;
      case DECODER_SHUTTING_DOWN:
        if (inst->current_command->id == COMMAND_RELEASE) {
          Release(inst);
          return NULL;
        } else {
          CommandCompleted(inst);
        }
        break;
      case DECODER_WAITING_RESOURCES:
        WaitForResources(inst);
        break;
      default:
        break;
    }
  }
  return NULL;
}

static void Initialize(DecoderInstance* inst) {
  enum DecRet rv =
      inst->dec.init((const void**)&inst->dec.inst,
                     inst->current_command->params.config, inst->dwl_inst);
  if (rv == DEC_OK)
    inst->client.Initialized(inst->client.client);
  else
    inst->client.NotifyError(inst->client.client, 0, rv);

  inst->max_num_of_decoded_pics =
      inst->current_command->params.config.max_num_pics_to_decode;
  inst->dwl.pthread_create(&inst->output_thread, NULL, OutputLoop, inst);
  CommandCompleted(inst);
}

static void WaitForResources(DecoderInstance* inst) {
  inst->dwl.pthread_mutex_lock(&inst->resource_mutex);
  while (!inst->resources_acquired)
    inst->dwl.pthread_cond_wait(&inst->resource_cond, &inst->resource_mutex);
  SetState(inst, DECODER_DECODING);
  inst->dwl.pthread_mutex_unlock(&inst->resource_mutex);
}

static void Decode(DecoderInstance* inst) {
  enum DecRet rv = DEC_OK;
  do {
    /* Skip decoding if we've decoded as many pics requested by the user. */
    if (inst->max_num_of_decoded_pics > 0 &&
        inst->num_of_decoded_pics >= inst->max_num_of_decoded_pics) {
      EndOfStream(inst);
      break;
    }

    struct DWLLinearMem buffer;
    u8* stream; u32 strm_len;
    buffer.virtual_address = (u32*)inst->buffer_status.strm_buff;
    buffer.bus_address = inst->buffer_status.strm_buff_bus_address;
    buffer.size = inst->buffer_status.buff_size;
    stream = inst->buffer_status.strm_curr_pos;
    strm_len = inst->buffer_status.data_left;
    rv = inst->dec.Decode(inst->dec.inst, buffer, &inst->buffer_status, stream,
                          strm_len, inst->dwl, inst->num_of_decoded_pics + 1);
    if (GetState(inst) == DECODER_SHUTTING_DOWN) {
      break;
    }
    if (rv == DEC_HDRS_RDY) {
      inst->dec.GetInfo(inst->dec.inst, &inst->sequence_info);
#ifndef USE_EXTERNAL_BUFFER
      SetState(inst, DECODER_WAITING_RESOURCES);
#endif
      inst->client.HeadersDecoded(inst->client.client, inst->sequence_info);
      if (inst->buffer_status.data_left == 0) break;
    }
#ifdef USE_EXTERNAL_BUFFER
    else if (rv == DEC_WAITING_FOR_BUFFER) { /* Allocate buffers externally. */
      inst->dec.GetInfo(inst->dec.inst, &inst->sequence_info);
      SetState(inst, DECODER_WAITING_RESOURCES);
      inst->client.ExtBufferReq(inst->client.client);
      //if (inst->buffer_status.data_left == 0) break;
    }
#endif
      else if (rv < 0) { /* Error */
      inst->client.NotifyError(inst->client.client, 0, rv);
      break; /* Give up on the input buffer. */
    }
    if (rv == DEC_PIC_DECODED) inst->num_of_decoded_pics++;
  } while (inst->buffer_status.data_left > 0 &&
           GetState(inst) != DECODER_SHUTTING_DOWN);
  CommandCompleted(inst);
}

static void EndOfStream(DecoderInstance* inst) {
  inst->dec.EndOfStream(inst->dec.inst);

  inst->dwl.pthread_mutex_lock(&inst->eos_mutex);
  while (!inst->eos_ready)
    inst->dwl.pthread_cond_wait(&inst->eos_cond, &inst->eos_mutex);
  inst->dwl.pthread_mutex_unlock(&inst->eos_mutex);
  inst->pending_eos = 1;
  if (inst->current_command->id == COMMAND_END_OF_STREAM)
    CommandCompleted(inst);
  inst->client.EndOfStream(inst->client.client);
}

static void Release(DecoderInstance* inst) {
  if (!inst->pending_eos) {
    inst->dec.EndOfStream(inst->dec.inst); /* In case user didn't. */
  }
  /* Release the resource condition, if decoder is waiting for resources. */
  inst->dwl.pthread_mutex_lock(&inst->resource_mutex);
  inst->dwl.pthread_cond_signal(&inst->resource_cond);
  inst->dwl.pthread_mutex_unlock(&inst->resource_mutex);
  inst->dwl.pthread_join(inst->output_thread, NULL);
  inst->dwl.pthread_cond_destroy(&inst->resource_cond);
  inst->dwl.pthread_mutex_destroy(&inst->resource_mutex);
  inst->dwl.pthread_mutex_destroy(&inst->cs_mutex);
  inst->dec.Release(inst->dec.inst);
  FifoRelease(inst->input_queue);
  struct DecClientHandle client = inst->client;
  CommandCompleted(inst);
  inst->dwl.free(inst);
  client.Released(client.client);
}

static void* OutputLoop(void* arg) {
  DecoderInstance* inst = (DecoderInstance*)arg;
  struct DecPicture pic;
  enum DecRet rv;
  inst->dwl.memset(&pic, 0, sizeof(pic));
  while (1) {
    switch (GetState(inst)) {
      case DECODER_WAITING_RESOURCES:
        inst->dwl.pthread_mutex_lock(&inst->resource_mutex);
        while (!inst->resources_acquired &&
               GetState(inst) != DECODER_SHUTTING_DOWN)
          inst->dwl.pthread_cond_wait(&inst->resource_cond,
                                      &inst->resource_mutex);
        inst->dwl.pthread_mutex_unlock(&inst->resource_mutex);
        break;
      case DECODER_WAITING_HEADERS:
      case DECODER_DECODING:
        while ((rv = inst->dec.NextPicture(inst->dec.inst, &pic, inst->dwl)) ==
               DEC_PIC_RDY) {
          inst->client.PictureReady(inst->client.client, pic);
        }
        if (rv == DEC_END_OF_STREAM) {
          inst->dwl.pthread_mutex_lock(&inst->eos_mutex);
          inst->eos_ready = 1;
          inst->pending_eos = 0;
          inst->dwl.pthread_cond_signal(&inst->eos_cond);
          inst->dwl.pthread_mutex_unlock(&inst->eos_mutex);

          inst->dwl.pthread_exit(0);
          return NULL;
        }
        break;
      case DECODER_SHUTTING_DOWN:
        return NULL;
      default:
        break;
    }
  }
  return NULL;
}

static enum DecodingState GetState(DecoderInstance* inst) {
  inst->dwl.pthread_mutex_lock(&inst->cs_mutex);
  enum DecodingState state = inst->state;
  inst->dwl.pthread_mutex_unlock(&inst->cs_mutex);
  return state;
}

static void SetState(DecoderInstance* inst, enum DecodingState state) {
  const char* states[] = {
      "DECODER_WAITING_HEADERS", "DECODER_WAITING_RESOURCES",
      "DECODER_DECODING",        "DECODER_SHUTTING_DOWN"};
  inst->dwl.pthread_mutex_lock(&inst->cs_mutex);
  inst->dwl.printf("Decoder state change: %s => %s\n", states[inst->state],
                   states[state]);
  inst->state = state;
  inst->dwl.pthread_mutex_unlock(&inst->cs_mutex);
}

#ifdef ENABLE_HEVC_SUPPORT
static enum DecRet HevcInit(const void** inst, struct DecConfig config,
                            const void *dwl) {
  struct HevcDecConfig dec_cfg;
  dec_cfg.no_output_reordering = config.disable_picture_reordering;
  dec_cfg.use_video_freeze_concealment = config.concealment_mode;
  dec_cfg.use_video_compressor = config.use_video_compressor;
  dec_cfg.use_fetch_one_pic = config.use_fetch_one_pic;
  dec_cfg.use_ringbuffer = config.use_ringbuffer;
  dec_cfg.output_format = config.output_format;
  if (config.use_8bits_output)
    dec_cfg.pixel_format = DEC_OUT_PIXEL_CUT_8BIT;
  else if (config.use_p010_output)
    dec_cfg.pixel_format = DEC_OUT_PIXEL_P010;
  else if (config.use_bige_output)
    dec_cfg.pixel_format = DEC_OUT_PIXEL_CUSTOMER1;
  else
    dec_cfg.pixel_format = DEC_OUT_PIXEL_DEFAULT;
#ifdef DOWN_SCALER
  dec_cfg.dscale_cfg = config.dscale_cfg;
#endif
  return HevcDecInit(inst, dwl, &dec_cfg);
}

static enum DecRet HevcGetInfo(void* inst, struct DecSequenceInfo* info) {
  struct HevcDecInfo hevc_info;
  enum DecRet rv = HevcDecGetInfo(inst, &hevc_info);
  info->pic_width = hevc_info.pic_width;
  info->pic_height = hevc_info.pic_height;
  info->sar_width = hevc_info.sar_width;
  info->sar_height = hevc_info.sar_height;
  info->crop_params.crop_left_offset = hevc_info.crop_params.crop_left_offset;
  info->crop_params.crop_out_width = hevc_info.crop_params.crop_out_width;
  info->crop_params.crop_top_offset = hevc_info.crop_params.crop_top_offset;
  info->crop_params.crop_out_height = hevc_info.crop_params.crop_out_height;
  info->video_range = hevc_info.video_range;
  info->matrix_coefficients = hevc_info.matrix_coefficients;
  info->is_mono_chrome = hevc_info.mono_chrome;
  info->is_interlaced = hevc_info.interlaced_sequence;
  info->num_of_ref_frames = hevc_info.pic_buff_size;
  return rv;
}

static enum DecRet HevcDecode(void* inst, struct DWLLinearMem input, struct DecOutput* output,
                                    u8* stream, u32 strm_len, struct DWL dwl, u32 pic_id) {
  enum DecRet rv;
  struct HevcDecInput hevc_input;
  struct HevcDecOutput hevc_output;
  dwl.memset(&hevc_input, 0, sizeof(hevc_input));
  dwl.memset(&hevc_output, 0, sizeof(hevc_output));
  hevc_input.stream = (u8*)stream;
  hevc_input.stream_bus_address = input.bus_address + ((addr_t)stream - (addr_t)input.virtual_address);
  hevc_input.data_len = strm_len;
  hevc_input.buffer = (u8 *)input.virtual_address;
  hevc_input.buffer_bus_address = input.bus_address;
  hevc_input.buff_len = input.size;
  hevc_input.pic_id = pic_id;
  /* TODO(vmr): hevc must not acquire the resources automatically after
   *            successful header decoding. */
  rv = HevcDecDecode(inst, &hevc_input, &hevc_output);
  output->strm_curr_pos = hevc_output.strm_curr_pos;
  output->strm_curr_bus_address = hevc_output.strm_curr_bus_address;
  output->data_left = hevc_output.data_left;
  return rv;
}

static enum DecRet HevcNextPicture(void* inst, struct DecPicture* pic,
                                   struct DWL dwl) {
  enum DecRet rv;
  struct HevcDecPicture hpic;
  rv = HevcDecNextPicture(inst, &hpic);
  dwl.memset(pic, 0, sizeof(struct DecPicture));
  pic->luma.virtual_address = (u32*)hpic.output_picture;
  pic->luma.bus_address = hpic.output_picture_bus_address;
#if 0
  if (hpic.output_format == DEC_OUT_FRM_RASTER_SCAN)
    hpic.pic_width = NEXT_MULTIPLE(hpic.pic_width, 16);
#endif
  pic->luma.size = hpic.pic_stride * hpic.pic_height;
  /* TODO temporal solution to set chroma base here */
  pic->chroma.virtual_address =
    pic->luma.virtual_address + (pic->luma.size);
  pic->chroma.bus_address = pic->luma.bus_address + pic->luma.size;
  pic->chroma.size = pic->luma.size / 2;
  /* TODO(vmr): find out for real also if it is B frame */
  pic->picture_info.pic_coding_type =
    hpic.is_idr_picture ? DEC_PIC_TYPE_I : DEC_PIC_TYPE_P;
  pic->picture_info.format = hpic.output_format;
  pic->picture_info.pixel_format = hpic.pixel_format;
  pic->picture_info.pic_id = hpic.pic_id;
  pic->picture_info.cycles_per_mb = hpic.cycles_per_mb;
  pic->sequence_info.pic_width = hpic.pic_width;
  pic->sequence_info.pic_height = hpic.pic_height;
  pic->sequence_info.crop_params.crop_left_offset =
    hpic.crop_params.crop_left_offset;
  pic->sequence_info.crop_params.crop_out_width =
    hpic.crop_params.crop_out_width;
  pic->sequence_info.crop_params.crop_top_offset =
    hpic.crop_params.crop_top_offset;
  pic->sequence_info.crop_params.crop_out_height =
    hpic.crop_params.crop_out_height;
  pic->sequence_info.sar_width = hpic.dec_info.sar_width;
  pic->sequence_info.sar_height = hpic.dec_info.sar_height;
  pic->sequence_info.video_range = hpic.dec_info.video_range;
  pic->sequence_info.matrix_coefficients =
    hpic.dec_info.matrix_coefficients;
  pic->sequence_info.is_mono_chrome =
    hpic.dec_info.mono_chrome;
  pic->sequence_info.is_interlaced = hpic.dec_info.interlaced_sequence;
  pic->sequence_info.num_of_ref_frames =
    hpic.dec_info.pic_buff_size;
  pic->sequence_info.bit_depth_luma = hpic.bit_depth_luma;
  pic->sequence_info.bit_depth_chroma = hpic.bit_depth_chroma;
  pic->sequence_info.pic_stride = hpic.pic_stride;
#ifdef DOWN_SCALER
  pic->dscale_luma.virtual_address = (u32*)hpic.output_downscale_picture;
  pic->dscale_luma.bus_address = hpic.output_downscale_picture_bus_address;
  pic->dscale_luma.size = hpic.dscale_stride * hpic.dscale_height;
  pic->dscale_chroma.virtual_address = (u32*)hpic.output_downscale_picture_chroma;
  pic->dscale_chroma.bus_address = hpic.output_downscale_picture_chroma_bus_address;
  pic->dscale_chroma.size = (hpic.dscale_stride * hpic.dscale_height) / 2;
  pic->dscale_width = hpic.dscale_width;
  pic->dscale_height = hpic.dscale_height;
  pic->dscale_stride = hpic.dscale_stride;
#endif

  return rv;
}

static enum DecRet HevcPictureConsumed(void* inst, struct DecPicture pic,
                                       struct DWL dwl) {
  struct HevcDecPicture hpic;
  dwl.memset(&hpic, 0, sizeof(struct HevcDecPicture));
  /* TODO update chroma luma/chroma base */
  hpic.output_picture = pic.luma.virtual_address;
  hpic.output_picture_bus_address = pic.luma.bus_address;
  hpic.is_idr_picture = pic.picture_info.pic_coding_type == DEC_PIC_TYPE_I;
#ifdef DOWN_SCALER
  hpic.output_downscale_picture = pic.dscale_luma.virtual_address;
  hpic.output_downscale_picture_bus_address = pic.dscale_luma.bus_address;
#endif
  return HevcDecPictureConsumed(inst, &hpic);
}

static enum DecRet HevcEndOfStream(void* inst) {
  return HevcDecEndOfStream(inst);
}

#ifdef USE_EXTERNAL_BUFFER
static enum DecRet HevcGetBufferInfo(void *inst, struct DecBufferInfo *buf_info) {
  struct HevcDecBufferInfo hbuf;
  enum DecRet rv;

  rv = HevcDecGetBufferInfo(inst, &hbuf);
  buf_info->buf_to_free = hbuf.buf_to_free;
  buf_info->next_buf_size = hbuf.next_buf_size;
  buf_info->buf_num = hbuf.buf_num;
#ifdef ASIC_TRACE_SUPPORT
  buf_info->is_frame_buffer = hbuf.is_frame_buffer;
#endif
  return rv;
}

static enum DecRet HevcAddBuffer(void *inst, struct DWLLinearMem *buf) {
  return HevcDecAddBuffer(inst, buf);
}
#endif

static void HevcRelease(void* inst) { HevcDecRelease(inst); }

static void HevcStreamDecoded(void* dec_inst) {
  DecoderInstance* inst = (DecoderInstance*)dec_inst;
  inst->client.BufferDecoded(inst->client.client,
                             &inst->current_command->params.input);
}
#endif /* ENABLE_HEVC_SUPPORT */

#ifdef ENABLE_VP9_SUPPORT
static enum DecRet Vp9Init(const void** inst, struct DecConfig config,
                           const void *dwl) {
  enum DecPictureFormat format = config.output_format;

  struct Vp9DecConfig dec_cfg;
  dec_cfg.use_video_freeze_concealment = config.concealment_mode;
  dec_cfg.num_frame_buffers = 4;
  dec_cfg.dpb_flags = 4;
  dec_cfg.use_video_compressor = config.use_video_compressor;
  dec_cfg.use_fetch_one_pic = config.use_fetch_one_pic;
  dec_cfg.use_ringbuffer = config.use_ringbuffer;
  dec_cfg.output_format = format;
  if (config.use_8bits_output)
    dec_cfg.pixel_format = DEC_OUT_PIXEL_CUT_8BIT;
  else if (config.use_p010_output)
    dec_cfg.pixel_format = DEC_OUT_PIXEL_P010;
  else if (config.use_bige_output)
    dec_cfg.pixel_format = DEC_OUT_PIXEL_CUSTOMER1;
  else
    dec_cfg.pixel_format = DEC_OUT_PIXEL_DEFAULT;

#ifdef DOWN_SCALER
  dec_cfg.dscale_cfg = config.dscale_cfg;
#endif
  return Vp9DecInit(inst, dwl, &dec_cfg);
}

static enum DecRet Vp9GetInfo(void* inst, struct DecSequenceInfo* info) {
  struct Vp9DecInfo vp9_info = {0};
  enum DecRet rv = Vp9DecGetInfo(inst, &vp9_info);
  info->pic_width = vp9_info.frame_width;
  info->pic_height = vp9_info.frame_height;
  info->sar_width = 1;
  info->sar_height = 1;
  info->crop_params.crop_left_offset = 0;
  info->crop_params.crop_out_width = vp9_info.coded_width;
  info->crop_params.crop_top_offset = 0;
  info->crop_params.crop_out_height = vp9_info.coded_height;
  /* TODO(vmr): Consider adding scaled_width & scaled_height. */
  /* TODO(vmr): output_format? */
  info->num_of_ref_frames = vp9_info.pic_buff_size;
  info->video_range = DEC_VIDEO_RANGE_NORMAL;
  info->matrix_coefficients = 0;
  info->is_mono_chrome = 0;
  info->is_interlaced = 0;
  info->bit_depth_luma = info->bit_depth_chroma = vp9_info.bit_depth;
  return rv;
}

/* function copied from the libvpx */
static void ParseSuperframeIndex(const u8* data, size_t data_sz,
                                        const u8* buf, size_t buf_sz,
                                        u32 sizes[8], i32* count) {
  u8 marker;
  u8* buf_end = (u8*)buf + buf_sz;
  if ((data + data_sz - 1) < buf_end)
    marker = DWLPrivateAreaReadByte(data + data_sz - 1);
  else
    marker = DWLPrivateAreaReadByte(data + (i32)data_sz - 1 - (i32)buf_sz);
  //marker = data[data_sz - 1];
  *count = 0;

  if ((marker & 0xe0) == 0xc0) {
    const u32 frames = (marker & 0x7) + 1;
    const u32 mag = ((marker >> 3) & 0x3) + 1;
    const u32 index_sz = 2 + mag * frames;
    u8 index_value;
    u32 turn_around = 0;
    if((data + data_sz - index_sz) < buf_end)
      index_value = DWLPrivateAreaReadByte(data + data_sz - index_sz);
    else {
      index_value = DWLPrivateAreaReadByte(data + data_sz - index_sz - buf_sz);
      turn_around = 1;
    }

    if (data_sz >= index_sz && index_value == marker) {
      /* found a valid superframe index */
      u32 i, j;
      const u8* x = data + data_sz - index_sz + 1;
      if(turn_around)
        x = data + data_sz - index_sz + 1 - buf_sz;

      for (i = 0; i < frames; i++) {
        u32 this_sz = 0;

        for (j = 0; j < mag; j++) {
          if (x == buf + buf_sz)
            x = buf;
          this_sz |= DWLPrivateAreaReadByte(x) << (j * 8);
          x++;
        }
        sizes[i] = this_sz;
      }

      *count = frames;
    }
  }
}

static enum DecRet Vp9Decode(void* inst, struct DWLLinearMem input, struct DecOutput* output,
                                    u8* stream, u32 strm_len, struct DWL dwl, u32 pic_id) {
  enum DecRet rv;
  struct Vp9DecInput vp9_input;
  struct Vp9DecOutput vp9_output;
  dwl.memset(&vp9_input, 0, sizeof(vp9_input));
  dwl.memset(&vp9_output, 0, sizeof(vp9_output));
  vp9_input.stream = (u8*)stream;
  vp9_input.stream_bus_address = input.bus_address + ((addr_t)stream - (addr_t)input.virtual_address);
  vp9_input.data_len = strm_len;
  vp9_input.buffer = (u8*)input.virtual_address;
  vp9_input.buffer_bus_address = input.bus_address;
  vp9_input.buff_len = input.size;
  vp9_input.pic_id = pic_id;
  static u32 sizes[8];
  static i32 frames_this_pts, frame_count = 0;
  u32 data_sz = vp9_input.data_len;
  u32 data_len = vp9_input.data_len;
  u32 consumed_sz = 0;
  const u8* data_start = vp9_input.stream;
  const u8* buf_end = vp9_input.buffer + vp9_input.buff_len;
  //const u8* data_end = data_start + data_sz;

  /* TODO(vmr): vp9 must not acquire the resources automatically after
   *            successful header decoding. */

  /* TODO: Is this correct place to handle superframe indexes? */
  ParseSuperframeIndex(vp9_input.stream, data_sz,
                       vp9_input.buffer, input.size,
                        sizes, &frames_this_pts);

  do {
    /* Skip over the superframe index, if present */
    if (data_sz && (DWLPrivateAreaReadByte(data_start) & 0xe0) == 0xc0) {
      const u8 marker = DWLPrivateAreaReadByte(data_start);
      const u32 frames = (marker & 0x7) + 1;
      const u32 mag = ((marker >> 3) & 0x3) + 1;
      const u32 index_sz = 2 + mag * frames;
      u8 index_value;
      if(data_start + index_sz - 1 < buf_end)
        index_value = DWLPrivateAreaReadByte(data_start + index_sz - 1);
      else
        index_value = DWLPrivateAreaReadByte(data_start + (i32)index_sz - 1 - (i32)vp9_input.buff_len);

      if (data_sz >= index_sz && index_value == marker) {
        data_start += index_sz;
        if (data_start >= buf_end)
          data_start -= vp9_input.buff_len;
        consumed_sz += index_sz;
        data_sz -= index_sz;
        if (consumed_sz < data_len)
          continue;
        else{
          frames_this_pts = 0;
          frame_count = 0;
          break;
        }
      }
    }

    /* Use the correct size for this frame, if an index is present. */
    if (frames_this_pts) {
      u32 this_sz = sizes[frame_count];

      if (data_sz < this_sz) {
        /* printf("Invalid frame size in index\n"); */
        return DEC_STRM_ERROR;
      }

      data_sz = this_sz;
     // frame_count++;
    }

    vp9_input.stream_bus_address =
        input.bus_address + (addr_t)data_start - (addr_t)input.virtual_address;
    vp9_input.stream = (u8*)data_start;
    vp9_input.data_len = data_sz;

    rv = Vp9DecDecode(inst, &vp9_input, &vp9_output);
    /* Headers decoded or error occurred */
    if (rv == DEC_HDRS_RDY || rv != DEC_PIC_DECODED) break;
    else if (frames_this_pts) frame_count++;

    data_start += data_sz;
    consumed_sz += data_sz;
    if (data_start >= buf_end)
      data_start -= vp9_input.buff_len;

    /* Account for suboptimal termination by the encoder. */
    while (consumed_sz < data_len && *data_start == 0) {
      data_start++;
      consumed_sz++;
      if (data_start >= buf_end)
        data_start -= vp9_input.buff_len;
    }

    data_sz = data_len - consumed_sz;

  } while (consumed_sz < data_len);

  /* TODO(vmr): output is complete garbage on on VP9. Fix in the code decoding
   *            the frame headers. */
  /*output->strm_curr_pos = vp9_output.strm_curr_pos;
    output->strm_curr_bus_address = vp9_output.strm_curr_bus_address;
    output->data_left = vp9_output.data_left;*/
  switch (rv) {/* Workaround */
    case DEC_HDRS_RDY:
      output->strm_curr_pos = (u8*)vp9_input.stream;
      output->strm_curr_bus_address = vp9_input.stream_bus_address;
      output->data_left = vp9_input.data_len;
      break;
#ifdef USE_EXTERNAL_BUFFER
    case DEC_WAITING_FOR_BUFFER:
      output->strm_curr_pos = (u8*)vp9_input.stream;
      output->strm_curr_bus_address = vp9_input.stream_bus_address;
      output->data_left = data_len - consumed_sz;
      break;
#endif
    default:
      if ((vp9_input.stream + vp9_input.data_len) >= buf_end) {
        output->strm_curr_pos = (u8*)(vp9_input.stream + vp9_input.data_len
                                      - vp9_input.buff_len);
        output->strm_curr_bus_address = vp9_input.stream_bus_address + vp9_input.data_len
                                          - vp9_input.buff_len;
      } else {
        output->strm_curr_pos = (u8*)(vp9_input.stream + vp9_input.data_len);
        output->strm_curr_bus_address = vp9_input.stream_bus_address + vp9_input.data_len;
      }
      output->data_left = 0;
      break;
  }
  return rv;
}

static enum DecRet Vp9NextPicture(void* inst, struct DecPicture* pic,
                                  struct DWL dwl) {
  enum DecRet rv;
  struct Vp9DecPicture vpic = {0};
  rv = Vp9DecNextPicture(inst, &vpic);
  dwl.memset(pic, 0, sizeof(struct DecPicture));
  pic->luma.virtual_address = (u32*)vpic.output_luma_base;
  pic->luma.bus_address = vpic.output_luma_bus_address;
  pic->luma.size = vpic.frame_width * vpic.frame_height;
  pic->chroma.virtual_address = (u32*)vpic.output_chroma_base;
  pic->chroma.bus_address = vpic.output_chroma_bus_address;
  pic->chroma.size = (vpic.frame_width * vpic.frame_height) / 2;
#ifdef DOWN_SCALER
  pic->dscale_luma.virtual_address = (u32*)vpic.output_dscale_luma_base;
  pic->dscale_luma.bus_address = vpic.output_dscale_luma_bus_address;
  pic->dscale_luma.size = vpic.dscale_stride * vpic.dscale_height;
  pic->dscale_chroma.virtual_address = (u32*)vpic.output_dscale_chroma_base;
  pic->dscale_chroma.bus_address = vpic.output_dscale_chroma_bus_address;
  pic->dscale_chroma.size = (vpic.dscale_stride * vpic.dscale_height) / 2;
  pic->dscale_width = vpic.dscale_width;
  pic->dscale_height = vpic.dscale_height;
  pic->dscale_stride = vpic.dscale_stride;
#endif

  /* TODO(vmr): find out for real also if it is B frame */
  pic->picture_info.pic_coding_type =
      vpic.is_intra_frame ? DEC_PIC_TYPE_I : DEC_PIC_TYPE_P;
  pic->sequence_info.pic_width = vpic.frame_width;
  pic->sequence_info.pic_height = vpic.frame_height;
  pic->sequence_info.sar_width = 1;
  pic->sequence_info.sar_height = 1;
  pic->sequence_info.crop_params.crop_left_offset = 0;
  pic->sequence_info.crop_params.crop_out_width = vpic.coded_width;
  pic->sequence_info.crop_params.crop_top_offset = 0;
  pic->sequence_info.crop_params.crop_out_height = vpic.coded_height;
  pic->sequence_info.video_range = DEC_VIDEO_RANGE_NORMAL;
  pic->sequence_info.matrix_coefficients = 0;
  pic->sequence_info.is_mono_chrome = 0;
  pic->sequence_info.is_interlaced = 0;
  pic->sequence_info.num_of_ref_frames = pic->sequence_info.num_of_ref_frames;
  pic->picture_info.format = vpic.output_format;
  pic->picture_info.pixel_format = vpic.pixel_format;
  pic->picture_info.pic_id = vpic.pic_id;
  pic->picture_info.cycles_per_mb = vpic.cycles_per_mb;
  pic->sequence_info.bit_depth_luma = vpic.bit_depth_luma;
  pic->sequence_info.bit_depth_chroma = vpic.bit_depth_chroma;
  pic->sequence_info.pic_stride = vpic.pic_stride;
  return rv;
}

static enum DecRet Vp9PictureConsumed(void* inst, struct DecPicture pic,
                                      struct DWL dwl) {
  struct Vp9DecPicture vpic;
  dwl.memset(&vpic, 0, sizeof(struct Vp9DecPicture));
  /* TODO chroma base needed? */
  vpic.output_luma_base = pic.luma.virtual_address;
  vpic.output_luma_bus_address = pic.luma.bus_address;
  vpic.is_intra_frame = pic.picture_info.pic_coding_type == DEC_PIC_TYPE_I;
  return Vp9DecPictureConsumed(inst, &vpic);
}

static enum DecRet Vp9EndOfStream(void* inst) {
  return Vp9DecEndOfStream(inst);
}

#ifdef USE_EXTERNAL_BUFFER
static enum DecRet Vp9GetBufferInfo(void *inst, struct DecBufferInfo *buf_info) {
  struct Vp9DecBufferInfo vbuf;
  enum DecRet rv;

  rv = Vp9DecGetBufferInfo(inst, &vbuf);
  buf_info->buf_to_free = vbuf.buf_to_free;
  buf_info->next_buf_size = vbuf.next_buf_size;
  buf_info->buf_num = vbuf.buf_num;
#ifdef ASIC_TRACE_SUPPORT
  buf_info->is_frame_buffer = vbuf.is_frame_buffer;
#endif
  return rv;
}

static enum DecRet Vp9AddBuffer(void *inst, struct DWLLinearMem *buf) {
  return Vp9DecAddBuffer(inst, buf);
}
#endif
static void Vp9Release(void* inst) { Vp9DecRelease(inst); }

static void Vp9StreamDecoded(void* dec_inst) {
  DecoderInstance* inst = (DecoderInstance*)dec_inst;
  if (inst->prev_input.data_len) {
    inst->client.BufferDecoded(inst->client.client, &inst->prev_input);
  }
  inst->prev_input.data_len = inst->current_command->params.input.data_len;
  inst->prev_input.buffer.virtual_address =
      inst->current_command->params.input.buffer.virtual_address;
  inst->prev_input.buffer.bus_address =
      inst->current_command->params.input.buffer.bus_address;
  inst->prev_input.buffer.size =
      inst->current_command->params.input.buffer.size;
  inst->prev_input.buffer.logical_size =
      inst->current_command->params.input.buffer.logical_size;
  inst->prev_input.stream[0] = (u8*)inst->current_command->params.input.stream[0];
  inst->prev_input.stream[1] = (u8*)inst->current_command->params.input.stream[1];

}
#endif /* ENABLE_VP9_SUPPORT */
