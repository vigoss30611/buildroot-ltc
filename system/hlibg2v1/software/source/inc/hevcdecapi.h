/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef HEVCDECAPI_H
#define HEVCDECAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "basetype.h"
#include "decapicommon.h"
#include "dectypes.h"
#ifdef __GNUC__
#define EXPORT __attribute__((visibility("default")))   //just for gcc4+
#else
#define EXPORT  __declspec(dllexport)
#endif

/*------------------------------------------------------------------------------
    API type definitions
------------------------------------------------------------------------------*/

/* Decoder instance */
typedef const void *HevcDecInst;

/* Input structure */
struct HevcDecInput {
  u8 *stream;             /* Pointer to the input */
  addr_t stream_bus_address; /* DMA bus address of the input stream */
  u32 data_len;           /* Number of bytes to be decoded         */
  u8 *buffer;
  addr_t buffer_bus_address;
  u32 buff_len;           /* Stream buffer byte length         */
  u32 pic_id;             /* Identifier for the picture to be decoded */
  u32 *raster_out_y;
  addr_t raster_out_bus_address_y; /* DMA bus address of the input stream */
  u32 *raster_out_c;
  addr_t raster_out_bus_address_c;
};

/* Output structure */
struct HevcDecOutput {
  u8 *strm_curr_pos; /* Pointer to stream position where decoding ended */
  u32 strm_curr_bus_address; /* DMA bus address location where the decoding
                                ended */
  u32 data_left; /* how many bytes left undecoded */
};

/* cropping info */
struct HevcCropParams {
  u32 crop_left_offset;
  u32 crop_out_width;
  u32 crop_top_offset;
  u32 crop_out_height;
};

/* stream info filled by HevcDecGetInfo */
struct HevcDecInfo {
  u32 pic_width;   /* decoded picture width in pixels */
  u32 pic_height;  /* decoded picture height in pixels */
  u32 video_range; /* samples' video range */
  u32 matrix_coefficients;
  struct HevcCropParams crop_params;   /* display cropping information */
  enum DecPictureFormat output_format; /* format of the output picture */
  enum DecPicturePixelFormat pixel_format; /* format of the pixels in output picture */
  u32 sar_width;                       /* sample aspect ratio */
  u32 sar_height;                      /* sample aspect ratio */
  u32 mono_chrome;                     /* is sequence monochrome */
  u32 interlaced_sequence;             /* is sequence interlaced */
  u32 dpb_mode;      /* DPB mode; frame, or field interlaced */
  u32 pic_buff_size; /* number of picture buffers allocated&used by decoder */
  u32 multi_buff_pp_size; /* number of picture buffers needed in
                             decoder+postprocessor multibuffer mode */
  u32 bit_depth;     /* bit depth per pixel stored in memory */
  u32 pic_stride;         /* Byte width of the pixel as stored in memory */

};

/* Output structure for HevcDecNextPicture */
struct HevcDecPicture {
  u32 pic_width;  /* pixels width of the picture as stored in memory */
  u32 pic_height; /* pixel height of the picture as stored in memory */
  struct HevcCropParams crop_params; /* cropping parameters */
  const u32 *output_picture;         /* Pointer to the picture */
  addr_t output_picture_bus_address;    /* DMA bus address of the output picture
                               buffer */
  u32 pic_id;         /* Identifier of the picture to be displayed */
  u32 is_idr_picture; /* Indicates if picture is an IDR picture */
  u32 pic_corrupt;    /* Indicates that picture is corrupted */
  enum DecPictureFormat output_format;
  enum DecPicturePixelFormat pixel_format;
  u32 bit_depth_luma;
  u32 bit_depth_chroma;
  u32 pic_stride;       /* Byte width of the picture as stored in memory */
#ifdef DOWN_SCALER
  u32 dscale_width;   /* valid pixels width of down scaled buffer in memory */
  u32 dscale_height;  /* valid pixels height of down scaled buffer in memory */
  u32 dscale_stride;  /* total bytes count of a pixel line in down scaled buffer in memory */
  const u32 *output_downscale_picture;         /* Pointer to the picture */
  addr_t output_downscale_picture_bus_address;    /* DMA bus address of the output picture
                               buffer */
  const u32 *output_downscale_picture_chroma;         /* Pointer to the picture */
  addr_t output_downscale_picture_chroma_bus_address;    /* DMA bus address of the output picture
                               buffer */
#endif
  const u32 *output_rfc_luma_base;   /* Pointer to the rfc table */
  addr_t output_rfc_luma_bus_address;
  const u32 *output_rfc_chroma_base; /* Pointer to the rfc chroma table */
  addr_t output_rfc_chroma_bus_address; /* Bus address of the chrominance table */
  u32 cycles_per_mb;   /* Avarage cycle count per macroblock */
  struct HevcDecInfo dec_info; /* Stream info by HevcDecGetInfo */
};

struct HevcDecConfig {
  u32 no_output_reordering;
  u32 use_video_freeze_concealment;
#ifdef DOWN_SCALER
  struct DecDownscaleCfg dscale_cfg;
#endif
  u32 use_video_compressor;
  u32 use_ringbuffer;
  u32 use_fetch_one_pic;
  enum DecPictureFormat output_format;
  enum DecPicturePixelFormat pixel_format;
};

typedef struct DecSwHwBuild HevcDecBuild;

#ifdef USE_EXTERNAL_BUFFER
struct HevcDecBufferInfo {
  u32 next_buf_size;
  u32 buf_num;
  struct DWLLinearMem buf_to_free;
#ifdef ASIC_TRACE_SUPPORT
  u32 is_frame_buffer;
#endif
};
#endif
/*------------------------------------------------------------------------------
    Prototypes of Decoder API functions
------------------------------------------------------------------------------*/

HevcDecBuild HevcDecGetBuild(void);

EXPORT enum DecRet HevcDecInit(HevcDecInst *dec_inst, const void *dwl, struct HevcDecConfig *dec_cfg);

enum DecRet HevcDecUseExtraFrmBuffers(HevcDecInst dec_inst, u32 n);

EXPORT void HevcDecRelease(HevcDecInst dec_inst);

EXPORT enum DecRet HevcDecDecode(HevcDecInst dec_inst,
                          const struct HevcDecInput *input,
                          struct HevcDecOutput *output);

EXPORT enum DecRet HevcDecNextPicture(HevcDecInst dec_inst,
                               struct HevcDecPicture *picture);

EXPORT enum DecRet HevcDecPictureConsumed(HevcDecInst dec_inst,
                                   const struct HevcDecPicture *picture);

EXPORT enum DecRet HevcDecEndOfStream(HevcDecInst dec_inst);

EXPORT enum DecRet HevcDecGetInfo(HevcDecInst dec_inst, struct HevcDecInfo *dec_info);

enum DecRet HevcDecPeek(HevcDecInst dec_inst, struct HevcDecPicture *output);

#ifdef USE_EXTERNAL_BUFFER
enum DecRet HevcDecAddBuffer(HevcDecInst dec_inst, struct DWLLinearMem *info);

enum DecRet HevcDecGetBufferInfo(HevcDecInst dec_inst, struct HevcDecBufferInfo *mem_info);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HEVCDECAPI_H */
