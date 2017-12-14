/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/yuvfilters.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "software/test/common/swhw/tb_tiled.h"

static u32 TileOffset(u32 x, u32 y, u32 w, u32 h, u32 stride) {
  return y * stride * h + x * w * h;
}

static u32 RasterOffset(u32 x, u32 y, u32 w, u32 h, u32 stride) {
  return y * stride * h + x * w;
}

static void raster_to_tile4x4(u16* tile, u16* raster, u32 stride) {
  tile[0] = raster[0];
  tile[1] = raster[1];
  tile[2] = raster[2];
  tile[3] = raster[3];
  tile[4] = raster[stride];
  tile[5] = raster[stride + 1];
  tile[6] = raster[stride + 2];
  tile[7] = raster[stride + 3];
  tile[8] = raster[2 * stride];
  tile[9] = raster[2 * stride + 1];
  tile[10] = raster[2 * stride + 2];
  tile[11] = raster[2 * stride + 3];
  tile[12] = raster[3 * stride];
  tile[13] = raster[3 * stride + 1];
  tile[14] = raster[3 * stride + 2];
  tile[15] = raster[3 * stride + 3];
}

static void Tile4x4ToRaster(u16* raster, u16* tile, u32 stride) {
  raster[0] = tile[0];
  raster[1] = tile[1];
  raster[2] = tile[2];
  raster[3] = tile[3];
  raster[stride] = tile[4];
  raster[stride + 1] = tile[5];
  raster[stride + 2] = tile[6];
  raster[stride + 3] = tile[7];
  raster[2 * stride] = tile[8];
  raster[2 * stride + 1] = tile[9];
  raster[2 * stride + 2] = tile[10];
  raster[2 * stride + 3] = tile[11];
  raster[3 * stride] = tile[12];
  raster[3 * stride + 1] = tile[13];
  raster[3 * stride + 2] = tile[14];
  raster[3 * stride + 3] = tile[15];
}

static void Tiled4x4picToRaster(u16* dst, u16* src, u32 w, u32 h) {
  const u32 tile_w = 4;
  const u32 tile_h = 4;
  const u32 raster_stride = w;
  for (int y = 0; y < h / tile_h; y++) {
    for (int x = 0; x < w / tile_w; x++) {
      Tile4x4ToRaster(dst + RasterOffset(x, y, tile_w, tile_h, raster_stride),
                      src + TileOffset(x, y, tile_w, tile_h, raster_stride),
                      raster_stride);
    }
  }
}

/* Generic tile to raster function to convert tiles of any size. */
#if 0
static u32 TileToRaster(u8* tile_begin, u8* raster_begin, u32 raster_stride,
                          u8 src_sample_offset, u8 tile_width, u8 tile_height)
{
    int row = 0, col = 0;
    for (row = 0; row < tile_height; row++)
    {
        for (col = 0; col < tile_width; col++)
        {
           raster_begin[row * raster_stride + col] =
                tile_begin[(row * tile_width + col) * src_sample_offset];
        }
    }
    return row * col;
}
#endif

void YuvfilterTiled2Semiplanar(struct DecPicture* pic) {
  u32 num_of_pixels =
      pic->sequence_info.pic_width * pic->sequence_info.pic_height;
  u16* luma_src = (u16*)pic->g2_luma.virtual_address;
  u16* luma_dst = malloc(num_of_pixels * sizeof(u16));
  u16* cr_src = (u16*)pic->g2_chroma.virtual_address;
  u16* cr_dst = malloc(num_of_pixels / 2 * sizeof(u16));
  /* Converts 4x4 tiled luma into semiplanar format. */
  Tiled4x4picToRaster(luma_dst, luma_src, pic->sequence_info.pic_width,
                      pic->sequence_info.pic_height);
  /* Convert 4x4 tiled chroma into semiplanar format. */
  Tiled4x4picToRaster(cr_dst, cr_src, pic->sequence_info.pic_width,
                      pic->sequence_info.pic_height / 2);
  memcpy(luma_src, luma_dst, num_of_pixels * sizeof(u16));
  memcpy(cr_src, cr_dst, num_of_pixels / 2 * sizeof(u16));
  pic->picture_info.format = G2DEC_OUT_FRM_RASTER_SCAN;
  free(luma_dst);
  free(cr_dst);
}

void YuvfilterTiled2Planar(struct DecPicture* pic) {
  u16* luma_src = (u16*)pic->g2_luma.virtual_address;
  u32 num_pixels = pic->sequence_info.pic_width * pic->sequence_info.pic_height;
  u16* cbcr_src = (u16*)pic->g2_chroma.virtual_address;
  u16* luma_dst = malloc(num_pixels * sizeof(u16));
  u16* cbcr_dst = malloc(num_pixels / 2 * sizeof(u16));
  /* luma */
  Tiled4x4picToRaster(luma_dst, luma_src, pic->sequence_info.pic_width,
                      pic->sequence_info.pic_height);
  memcpy(luma_src, luma_dst, num_pixels * sizeof(u16));
  /* Two-step conversion for chroma blue and red, subsampled by half both
   * horizontally and vertically. First from tiled to semiplanar and then each
   * plane (cr blue and red) are split from semiplanar to own planes. */
  Tiled4x4picToRaster(cbcr_dst, cbcr_src, pic->sequence_info.pic_width,
                      pic->sequence_info.pic_height / 2);
  for (int i = 0;
       i < pic->sequence_info.pic_width * pic->sequence_info.pic_height / 2;
       i++) {
    if (i % 2 == 0)
      cbcr_src[i / 2] = cbcr_dst[i];
    else
      cbcr_src[num_pixels / 4 + i / 2] = cbcr_dst[i];
  }
  free(luma_dst);
  free(cbcr_dst);
}

void CropPlane(u16* dst, u16* src, u32 w, u32 h, u32 crop_top, u32 crop_left,
               u32 crop_w, u32 crop_h, u32 s) {
  u32 i, j, k;
  src += crop_top * w + crop_left;
  for (i = crop_h; i; i--) {
    for (j = crop_w; j; j -= s) {
      for (k = s; k; k--) *dst++ = *src++;
    }
    src += w - crop_w;
  }
}

void YuvfilterTiledcrop(struct DecPicture* pic) {
  struct DecCropParams crop = pic->sequence_info.crop_params;
  const u32 num_pixels =
      pic->sequence_info.pic_width * pic->sequence_info.pic_height;
  const u32 tile_w = 4;
  const u32 tile_h = 4;
  u32 raster_stride = pic->sequence_info.pic_width;
  /* Three step conversion:
   * 1. tile-to-semiplanar conversion. */
  u16* luma_src = (u16*)pic->g2_luma.virtual_address;
  u16* luma_tmp = malloc(num_pixels * sizeof(u16));
  u16* cbcr_src;
  u16* cbcr_tmp = 0;
  for (int y = 0; y < pic->sequence_info.pic_height / tile_h; y++) {
    for (int x = 0; x < pic->sequence_info.pic_width / tile_w; x++) {
      Tile4x4ToRaster(
          luma_tmp + RasterOffset(x, y, tile_w, tile_h, raster_stride),
          luma_src + TileOffset(x, y, tile_w, tile_h, raster_stride),
          raster_stride);
    }
  }
  if (!pic->sequence_info.is_mono_chrome) {
    cbcr_src = (u16*)pic->g2_chroma.virtual_address;
    cbcr_tmp = malloc(num_pixels / 2 * sizeof(u16));
    for (int y = 0; y < pic->sequence_info.pic_height / (tile_h * 2); y++) {
      for (int x = 0; x < pic->sequence_info.pic_width / tile_w; x++) {
        Tile4x4ToRaster(
            cbcr_tmp + RasterOffset(x, y, tile_w, tile_h, raster_stride),
            cbcr_src + TileOffset(x, y, tile_w, tile_h, raster_stride),
            raster_stride);
      }
    }
  }

  /* must be multiple of 8 to get full 4x4 chroma tiles */
  assert((crop.crop_out_width & 7) == 0);
  assert((crop.crop_out_height & 7) == 0);

  /* 2. semiplanar crop. */
  CropPlane(luma_tmp, luma_tmp, pic->sequence_info.pic_width,
            pic->sequence_info.pic_height, crop.crop_top_offset,
            crop.crop_left_offset, crop.crop_out_width, crop.crop_out_height,
            1);
  if (!pic->sequence_info.is_mono_chrome) {
    CropPlane(cbcr_tmp, cbcr_tmp, pic->sequence_info.pic_width,
              pic->sequence_info.pic_height / 2, crop.crop_top_offset / 2,
              crop.crop_left_offset, crop.crop_out_width,
              crop.crop_out_height / 2, 2);
  }
  /* 3. semiplanar-to-tile conversion. */
  u16* luma_dst = (u16*)pic->g2_luma.virtual_address;
  raster_stride = crop.crop_out_width;
  for (int y = 0; y < crop.crop_out_height / tile_h; y++) {
    for (int x = 0; x < crop.crop_out_width / tile_w; x++) {
      raster_to_tile4x4(
          luma_dst + TileOffset(x, y, tile_w, tile_h, raster_stride),
          luma_tmp + RasterOffset(x, y, tile_w, tile_h, raster_stride),
          raster_stride);
    }
  }
  if (!pic->sequence_info.is_mono_chrome) {
    u16* cbcr_dst = (u16*)pic->g2_chroma.virtual_address;
    for (int y = 0; y < crop.crop_out_height / (tile_h * 2); y++) {
      for (int x = 0; x < crop.crop_out_width / tile_w; x++) {
        raster_to_tile4x4(
            cbcr_dst + TileOffset(x, y, tile_w, tile_h, raster_stride),
            cbcr_tmp + RasterOffset(x, y, tile_w, tile_h, raster_stride),
            raster_stride);
      }
    }
  }
  pic->sequence_info.pic_width = crop.crop_out_width;
  pic->sequence_info.pic_height = crop.crop_out_height;
  free(luma_tmp);
  if (!pic->sequence_info.is_mono_chrome) free(cbcr_tmp);
}

void YuvfilterSemiplanarcrop(struct DecPicture* pic) {
  u16* in = (u16*)pic->g2_luma.virtual_address;
  struct DecCropParams crop = pic->sequence_info.crop_params;

  CropPlane(in, in, pic->sequence_info.pic_width, pic->sequence_info.pic_height,
            crop.crop_top_offset, crop.crop_left_offset, crop.crop_out_width,
            crop.crop_out_height, 1);

  u16* in_cr = (u16*)pic->g2_chroma.virtual_address;

  if (!pic->sequence_info.is_mono_chrome) {
    u32 w, h;
    /* round odd picture dimensions to next multiple of two for chroma */
    w = (crop.crop_out_width & 1) ? crop.crop_out_width + 1
                                  : crop.crop_out_width;
    h = (crop.crop_out_height & 1) ? (crop.crop_out_height + 1) / 2
                                   : crop.crop_out_height / 2;
    CropPlane(in_cr, in_cr, pic->sequence_info.pic_width,
              pic->sequence_info.pic_height / 2, crop.crop_top_offset / 2,
              crop.crop_left_offset, w, h, 2);
  }
  pic->sequence_info.pic_width = crop.crop_out_width;
  pic->sequence_info.pic_height = crop.crop_out_height;
}

void YuvfilterSemiplanar2Planar(struct DecPicture* pic) {
  u16* cbcr_src = (u16*)pic->g2_chroma.virtual_address;
  u32 w = pic->sequence_info.pic_width;
  u32 h = pic->sequence_info.pic_height;
  /* round odd picture dimensions to next multiple of two for chroma */
  if (w & 1) w += 1;
  if (h & 1) h += 1;
  u32 num_pixels = w * h;
  u16* cbcr_tmp = malloc(num_pixels / 2 * sizeof(u16));

  /* nothing to do for luma */

  /* chroma */
  for (int i = 0; i < num_pixels / 2; i++) {
    if (i % 2 == 0)
      cbcr_tmp[i / 2] = cbcr_src[i];
    else
      cbcr_tmp[num_pixels / 4 + i / 2] = cbcr_src[i];
  }
  memcpy(cbcr_src, cbcr_tmp, num_pixels / 2 * sizeof(u16));
  free(cbcr_tmp);
}

/* Unpack pixel with @pixel_width to u16. */
/* Return n pixels in u16. */
static u32 UnpackPixels(u16 *out, u8 *in, u32 n, u32 pixel_width){

  if (pixel_width == 8) {
    for (u32 i = 0; i < n; i ++) {
      out[i] = in[i];
    }
  } else {
    /* 10 bits per pixel. */
    for (u32 i = 0; i < (n & ~0x3); i += 4)
    {
      out[0] = (u16) in[0]       | (u16)(in[1] & 0x3)  << 8;
      out[1] = (u16)(in[1] >> 2) | (u16)(in[2] & 0xf)  << 6;
      out[2] = (u16)(in[2] >> 4) | (u16)(in[3] & 0x3f) << 4;
      out[3] = (u16)(in[3] >> 6) | (u16) in[4]         << 2;

      in += 5;
      out += 4;
    }
    switch (n & 3) {
      break;
    case 3:
      out[2] = (u16)(in[2] >> 4) | (u16)(in[3] & 0x3f) << 4;
    case 2:
      out[1] = (u16)(in[1] >> 2) | (u16)(in[2] & 0xf)  << 6;
    case 1:
      out[0] = (u16) in[0]       | (u16)(in[1] & 0x3)  << 8;
    case 0:
    default:
      break;
    }
  }

  return (n);
}

void YuvfilterConvertPixel16Bits(struct DecPicture *pic, struct DecPicture *dst) {
  u32 j;
  u8 *in;
  u16 *out;
  u32 pixel_width =(pic->sequence_info.bit_depth_luma == 8 &&
                    pic->sequence_info.bit_depth_chroma == 8) ? 8 : 10;

  // 10 bits -> 16 bits
  // luma
  in = (u8 *)pic->g2_luma.virtual_address;
  out = (u16 *)dst->g2_luma.virtual_address;
  for (j = 0; j < pic->sequence_info.pic_height; j++) {
    out += UnpackPixels(out, in, pic->sequence_info.pic_width, pixel_width);
    in += pic->sequence_info.pic_stride;
  }
  // chroma
  in = (u8 *)pic->g2_chroma.virtual_address;
  out = (u16 *)dst->g2_chroma.virtual_address;
  for (j = 0; j < pic->sequence_info.pic_height/2; j++) {
    out += UnpackPixels(out, in, pic->sequence_info.pic_width, pixel_width);
    in += pic->sequence_info.pic_stride;
  }
}

// Convert 16 bits pixel to 8 bits pixel for file sink when the bit depth is 8 bits.
void YuvfilterPrepareOutput(struct DecPicture *pic) {
  u32 i, j;
  u16 *in;
  u8 *out;

  if (pic->sequence_info.bit_depth_luma == 8 &&
      pic->sequence_info.bit_depth_chroma == 8) {
      printf("YuvfilterPrepareOutput Convert 16 bits pixel to 8 bits\n");
    // luma
    in = (u16 *)pic->g2_luma.virtual_address;
    out = (u8 *)pic->g2_luma.virtual_address;
    for (j = 0; j < pic->sequence_info.pic_height; j++) {
      for (i = 0; i < pic->sequence_info.pic_width; i++)
        out[i] = in[i];
      in += pic->sequence_info.pic_width;
      out += pic->sequence_info.pic_width;
    }
    // chroma
    in = (u16 *)pic->g2_chroma.virtual_address;
    out = (u8 *)pic->g2_chroma.virtual_address;
    for (j = 0; j < pic->sequence_info.pic_height/2; j++) {
      for (i = 0; i < pic->sequence_info.pic_width; i++)
        out[i] = in[i];
      in += pic->sequence_info.pic_width;
      out += pic->sequence_info.pic_width;
    }
  }
}

