/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Definition of container structure representing a decoder instance. */

#ifndef HEVC_CONTAINER_H_
#define HEVC_CONTAINER_H_

#include "basetype.h"
#include "hevc_storage.h"
#include "hevc_util.h"
#include "hevc_fb_mngr.h"
#include "deccfg.h"
#include "decppif.h"
#include "dwl.h"

#ifdef USE_EXTERNAL_BUFFER
#include "raster_buffer_mgr.h"
#endif

#ifdef USE_EXTERNAL_BUFFER
enum BufferType {
  REFERENCE_BUFFER = 0, /* reference + compression table + DMV*/
  RASTERSCAN_OUT_BUFFER,
#ifdef DOWN_SCALER
  DOWNSCALE_OUT_BUFFER,
#endif
  TILE_EDGE_BUFFER,  /* filter mem + bsd control mem */
  SEGMENT_MAP_BUFFER, /* segment map */
  MISC_LINEAR_BUFFER /* tile info + prob table + entropy context counter */
};

#define IS_EXTERNAL_BUFFER(config, type) (((config) >> (type)) & 1)
#endif

/* asic interface */
struct HevcDecAsic {
  struct DWLLinearMem *out_buffer;
#ifndef USE_EXTERNAL_BUFFER
  struct DWLLinearMem scaling_lists;
  struct DWLLinearMem tile_info;
  struct DWLLinearMem filter_mem;
  struct DWLLinearMem sao_mem;
  struct DWLLinearMem bsd_control_mem;
#else
  struct DWLLinearMem misc_linear;
  struct DWLLinearMem tile_edge;
  u32 scaling_lists_offset;
  u32 tile_info_offset;
  u32 filter_mem_offset;
  u32 sao_mem_offset;
  u32 bsd_control_mem_offset;
#endif
  i32 chroma_qp_index_offset;
  i32 chroma_qp_index_offset2;
  u32 disable_out_writing;
  u32 asic_id;
};

struct HevcDecContainer {
  const void *checksum;

  enum {
    HEVCDEC_UNINITIALIZED,
    HEVCDEC_INITIALIZED,
    HEVCDEC_BUFFER_EMPTY,
#ifdef USE_EXTERNAL_BUFFER
    HEVCDEC_WAITING_FOR_BUFFER,
#endif
    HEVCDEC_NEW_HEADERS,
    HEVCDEC_EOS
  } dec_state;

  i32 core_id;
  u32 max_dec_pic_width;
  u32 max_dec_pic_height;
  u32 dpb_mode;
  u32 output_format;
  u32 asic_running;
  u32 start_code_detected;

  u32 pic_number;
  const u8 *hw_stream_start;
  const u8 *hw_buffer;
  addr_t hw_stream_start_bus;
  addr_t hw_buffer_start_bus;
  u32 hw_buffer_length;
  u32 hw_bit_pos;
  u32 hw_length;
  u32 stream_pos_updated;
  u32 packet_decoded;

  u32 down_scale_enabled;
#ifdef DOWN_SCALER
  u32 down_scale_x;
  u32 down_scale_y;
#endif

  u32 hevc_main10_support;
  u32 use_8bits_output;
  u32 use_ringbuffer;
  u32 use_fetch_one_pic;
  u32 use_video_compressor;
  u32 use_p010_output;
  enum DecPicturePixelFormat pixel_format;

  const void *dwl; /* DWL instance */
  struct FrameBufferList fb_list;

  struct Storage storage;
  struct HevcDecAsic asic_buff[1];
  u32 hevc_regs[DEC_X170_REGISTERS];

#ifdef USE_EXTERNAL_BUFFER
  u32 ext_buffer_config;  /* Bit map config for external buffers. */

  u32 next_buf_size;  /* size of the requested external buffer */
  u32 buf_num;        /* number of buffers (with size of next_buf_size) requested to be allocated externally */
  struct DWLLinearMem _buf_to_free; /* For internal temp use, holding the info of linear mem to be released. */
  struct DWLLinearMem *buf_to_free;
  enum BufferType buf_type;
  u32 buffer_index;

  u32 resource_ready;
  struct DWLLinearMem tiled_buffers[MAX_FRAME_BUFFER_NUMBER];
  struct RasterBufferParams params;
  u32 rbm_release;    // rbm release not done, used to free old raster buffer before reallocating.
#ifdef ASIC_TRACE_SUPPORT
  u32 is_frame_buffer;  /* Whether it's a frame buffer (reference/raster scan/down scale, which will be allocated by DWLMallocRefFrm */
#endif
#endif

  u32 legacy_regs;  /* Legacy registers. */
  u32 u32Flush;  /* InfoTM patch */
};

#endif /* #ifdef HEVC_CONTAINER_H_ */
