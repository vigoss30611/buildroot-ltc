/* Copyright 2013 Google Inc. All Rights Reserved. */

#ifndef DECTYPES_H
#define DECTYPES_H

#include "software/source/inc/basetype.h"
#include "software/source/inc/dwl.h"

enum DecCodec {
  DEC_VP9,
  DEC_HEVC
};

/*!\enum enum enum DecRet
 * Return values for API
 */
enum DecRet {
  /** Success */
  DEC_OK = 0, /**<\hideinitializer */
  /** Stream processed */
  DEC_STRM_PROCESSED = 1, /**<\hideinitializer */
  /** Picture available for output */
  DEC_PIC_RDY = 2, /**<\hideinitializer */
  /** Picture decoded */
  DEC_PIC_DECODED = 3, /**<\hideinitializer */
  /** New stream headers decoded */
  DEC_HDRS_RDY = 4, /**<\hideinitializer */
  /** Advanced coding tools detected in stream */
  DEC_ADVANCED_TOOLS = 5, /**<\hideinitializer */
  /** Output pictures must be retrieved before continuing decode */
  DEC_PENDING_FLUSH = 6, /**<\hideinitializer */
  /** Skipped decoding non-reference picture */
  DEC_NONREF_PIC_SKIPPED = 7, /**<\hideinitializer */
  /** End-of-stream state set in the decoder */
  DEC_END_OF_STREAM = 8,         /**<\hideinitializer */
  /** Waiting for external buffers allocated. */
  DEC_WAITING_FOR_BUFFER = 9,    /**<\hideinitializer */
  DEC_PARAM_ERROR = -1,          /**<\hideinitializer */
  DEC_STRM_ERROR = -2,           /**<\hideinitializer */
  DEC_NOT_INITIALIZED = -3,      /**<\hideinitializer */
  DEC_MEMFAIL = -4,              /**<\hideinitializer */
  DEC_INITFAIL = -5,             /**<\hideinitializer */
  DEC_HDRS_NOT_RDY = -6,         /**<\hideinitializer */
  DEC_STREAM_NOT_SUPPORTED = -8, /**<\hideinitializer */
  DEC_EXT_BUFFER_REJECTED = -9,    /**<\hideinitializer */
  /** External buffer rejected. (Too much than requested) */
  DEC_HW_RESERVED = -254,        /**<\hideinitializer */
  DEC_HW_TIMEOUT = -255,         /**<\hideinitializer */
  DEC_HW_BUS_ERROR = -256,       /**<\hideinitializer */
  DEC_SYSTEM_ERROR = -257,       /**<\hideinitializer */
  DEC_DWL_ERROR = -258,          /**<\hideinitializer */
  DEC_FORMAT_NOT_SUPPORTED =
      -1000 /**<\hideinitializer */
            /* TODO(vmr): Prune what is not needed from these. */
};

/* cropping info */
struct DecCropParams {
  u32 crop_left_offset;
  u32 crop_out_width;
  u32 crop_top_offset;
  u32 crop_out_height;
};

/* Input structure */
struct DecInput {
  struct DWLLinearMem buffer; /**< Pointer to the input buffer. */
  u8* stream[2];              /** < stream[0]: stream start before inputting. */
                              /** < stream[1]: stream end after inputting. */
  u32 data_len;               /**< Number of bytes to be decoded. */
};

/** Picture coding type */
enum DecPicCodingType {
  DEC_PIC_TYPE_I = 0,
  DEC_PIC_TYPE_P = 1,
  DEC_PIC_TYPE_B = 2
};

/** Error concealment mode */
enum DecErrorConcealment {
  DEC_PICTURE_FREEZE = 0,
  DEC_INTRA_FREEZE = 1
};

/** Decoder initialization params */
struct DecConfig {
  u32 disable_picture_reordering;
  enum DecPictureFormat output_format; /**< Format of the output picture */
  struct DWL dwl; /**< Pointers to the struct struct struct DWL functions. */
  const void* dwl_inst;       /**< struct struct struct DWL instance. */
  u32 max_num_pics_to_decode; /**< Limits the decoding to N pictures. 0 for
                                   unlimited. */
  enum DecErrorConcealment concealment_mode;
#ifdef DOWN_SCALER
  struct DecDownscaleCfg dscale_cfg;
#endif
  u32 use_video_compressor;
  u32 use_ringbuffer;
  u32 use_fetch_one_pic;
  u32 use_8bits_output;
  u32 use_p010_output;
  u32 use_bige_output;  /* Output pixel format. */
};

/** Sample range of the YCbCr samples in the decoded picture. */
enum DecVideoRange {
  DEC_VIDEO_RANGE_NORMAL = 0x0, /**< Sample range [16, 235] */
  DEC_VIDEO_RANGE_FULL = 0x1    /**< Sample range [0, 255] */
};

/* Video sequence information. */
struct DecSequenceInfo {
  u32 pic_width;                    /**< decoded picture width in pixels */
  u32 pic_height;                   /**< decoded picture height in pixels */
  u32 sar_width;                    /**< sample aspect ratio */
  u32 sar_height;                   /**< sample aspect ratio */
  struct DecCropParams crop_params; /**< Cropping parameters for the picture */
  enum DecVideoRange video_range;   /**< YUV sample video range */
  u32 matrix_coefficients; /**< matrix coefficients RGB->YUV conversion */
  u32 is_mono_chrome;      /**< is sequence monochrome */
  u32 is_interlaced;       /**< is sequence interlaced */
  u32 num_of_ref_frames;   /**< Maximum number of reference frames */
  u32 bit_depth_luma;    /* Bit depth of stored picture */
  u32 bit_depth_chroma;
  u32 pic_stride;       /* Byte width of the picture as stored in memory */
};

/* Picture specific information. */
struct DecPictureInfo {
  enum DecPicCodingType pic_coding_type; /**< Picture coding type */
  u32 is_corrupted;             /**< Tells whether picture is corrupted */
  enum DecPictureFormat format; /**< Color format of the picture */
  enum DecPicturePixelFormat pixel_format; /**< Pixel format of the picture */
  u32 cycles_per_mb;            /**< Avarage decoding time in cycles per mb */
  u32 pic_id;                   /**< Identifier for the picture to be decoded */
};

/* Structure to carry information about decoded pictures. */
struct DecPicture {
  struct DecSequenceInfo sequence_info; /**< Sequence coding parameters used */
  struct DWLLinearMem luma;             /**< Buffer properties */
  struct DWLLinearMem chroma;           /**< Buffer properties */
  struct DecPictureInfo picture_info;   /**< Picture specific parameters */
#ifdef DOWN_SCALER
  struct DWLLinearMem dscale_luma;      /**< Buffer properties */
  struct DWLLinearMem dscale_chroma;    /**< Buffer properties */
  u32 dscale_width;
  u32 dscale_height;
  u32 dscale_stride;
#endif
};

#ifdef USE_EXTERNAL_BUFFER
struct DecBufferInfo {
  u32 next_buf_size;
  u32 buf_num;
  struct DWLLinearMem buf_to_free;
#ifdef ASIC_TRACE_SUPPORT
  u32 is_frame_buffer;
#endif
};
#endif
#endif /* DECTYPES_H */
