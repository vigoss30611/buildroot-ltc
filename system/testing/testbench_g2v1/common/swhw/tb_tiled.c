/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Converts data to tiled format. */

#include <stdlib.h>
#include <string.h>

#include "tb_tiled.h"
#include "tb_md5.h"

void TbWriteTiledOutput(FILE *file, u8 *data, u32 mb_width, u32 mb_height,
                        u32 pic_num, u32 md5_sum_output, u32 input_format) {

  /* Variables */

  u8 *tiled_yuv = NULL;
  u8 *planar_data = NULL;
  u8 *out_pixel = NULL;
  u8 *read = NULL;
  u32 line_start_offset = 0;
  u32 row = 0, col = 0;
  u32 i, line;
  u32 width = mb_width << 4;
  u32 height = mb_height << 4;
  u32 cb_offset = 0, cr_offset = 0;
  u32 luma_pels, ch_pels, mb_lum_width, mb_lum_height, mb_ch_width,
      mb_ch_height;

  u8 *frame = NULL;

  tiled_yuv = (u8 *)malloc((width * height * 3) / 2);

  if (file == NULL) {
    return;
  }

  if (tiled_yuv == NULL) {
    return;
  }

  if (md5_sum_output) {
    frame = tiled_yuv;
  }

  out_pixel = tiled_yuv;

  /* convert semi-planar to planar */
  if (input_format) {
    u8 *planar_data_base;
    u32 tmp = width * height;

    planar_data = (u8 *)malloc((width * height * 3) / 2);
    planar_data_base = planar_data;

    if (planar_data == NULL) {
      free(tiled_yuv);
      return;
    }

    memcpy(planar_data, data, tmp);
    planar_data += tmp;

    for (i = 0; i < tmp / 4; i++) {
      memcpy(planar_data, data + tmp + i * 2, 1);
      ++planar_data;
    }

    for (i = 0; i < tmp / 4; i++) {
      memcpy(planar_data, data + tmp + 1 + i * 2, 1);
      ++planar_data;
    }

    planar_data = planar_data_base;
    data = planar_data;
  }

  /* Code */

  luma_pels = 256;
  ch_pels = 64;
  mb_lum_width = 16;
  mb_lum_height = 16;
  mb_ch_width = 8;
  mb_ch_height = 8;
  /* Luma */
  /* loop through MB rows */
  for (row = 0; row < mb_height; row++) {
    /* loop through mbs in a MB row */
    for (col = 0; col < mb_width; col++) {
      /* MB start location */
      read = data + (row * mb_width * luma_pels) + (col * mb_lum_width);

      /* loop through all the lines of the MB */
      for (line = 0; line < mb_lum_height; line++) {
        /* offset for each line inside the
         * MB from the start of the MB */
        line_start_offset = (mb_lum_width * mb_width * line);

        /* Write pels in a line to the tiled temp picture */
        for (i = 0; i < mb_lum_width; i++) {
          /*mboffs + line inside MB  + this pixel*/
          *(out_pixel++) = *(read + line_start_offset + i);
        }
      }
    }
  }

  /* Chroma */
  /* Cb start offset */
  cb_offset = width * height;
  cr_offset = cb_offset + (cb_offset >> 2);

  /* loop through MB rows */
  for (row = 0; row < mb_height; row++) {
    /* loop through mbs in a MB row */
    for (col = 0; col < mb_width; col++) {
      /* MB start location */
      read = data + (row * mb_width * ch_pels) + (col * mb_ch_width);

      /* loop through all the lines of the MB */
      for (line = 0; line < mb_ch_height; line++) {
        /* offset for each line inside the
         * MB from the start of the MB */
        line_start_offset = (mb_ch_width * mb_width * line);

        /* Write pels in a line to the tiled temp picture */
        for (i = 0; i < mb_ch_width; i++) {
          /* Write Cb */
          /*mboffs + line inside MB  + this pixel*/
          *(out_pixel++) = *(read + cb_offset + line_start_offset + i);
          /* Write Cr */
          *(out_pixel++) = *(read + cr_offset + line_start_offset + i);
        }
      }
    }
  }

  read = tiled_yuv;

  if (md5_sum_output) {
    TBWriteFrameMD5Sum(file, frame, (width * height * 3) / 2, pic_num);
  } else {
    /* Write temp frame out */
    fwrite(read, sizeof(u8), (width * height * 3) / 2, file);
  }

  free(tiled_yuv);
  if (planar_data != NULL) {
    free(planar_data);
  }
}

void TbChangeEndianess(u8 *data, u32 data_size) {
  u32 x = 0;
  for (x = 0; x < data_size; x += 4) {
    u32 tmp = 0;
    u32 *word = (u32 *)(data + x);

    tmp |= (*word & 0xFF) << 24;
    tmp |= (*word & 0xFF00) << 8;
    tmp |= (*word & 0xFF0000) >> 8;
    tmp |= (*word & 0xFF000000) >> 24;
    *word = tmp;
  }
}

/*------------------------------------------------------------------------------

    Function name: TBTiledToRaster

        Functional description:
            test bench common function to convert tiled mode ref picture to
            raster scan picture

        Inputs:

        Outputs:

        Returns:

------------------------------------------------------------------------------*/
void TBTiledToRaster(u32 tile_mode, u32 dpb_mode, u8 *in, u8 *out,
                     u32 pic_width, u32 pic_height) {
  u32 i, j;
  u32 k, l;
  u32 s, t;

  const u32 tile_width = 4, tile_height = 4;

  if (tile_mode == 1) {
    if (dpb_mode == 0) {
      for (i = 0; i < pic_height; i += tile_height) {
        t = 0;
        s = 0;
        for (j = 0; j < pic_width; j += tile_width) {
          /* copy this tile */
          for (k = 0; k < tile_height; ++k) {
            for (l = 0; l < tile_width; ++l) {
              out[k * pic_width + l + s] = in[t++];
            }
          }

          /* move to next horizontal tile */
          s += tile_width;
        }
        out += pic_width * tile_height;
        in += pic_width * tile_height;
      }
    } else if (dpb_mode == 1) {
      u8 *out_bot = out + pic_width;
      u8 *in_bot = in + (pic_width * pic_height / 2);
      /* 1st field */
      for (i = 0; i < pic_height / 2; i += tile_height) {
        t = 0;
        s = 0;
        for (j = 0; j < pic_width; j += tile_width) {
          /* copy this tile */
          for (k = 0; k < tile_height; ++k) {
            for (l = 0; l < tile_width; ++l) {
              out[k * 2 * pic_width + l + s] = in[t++];
            }
          }

          /* move to next horizontal tile */
          s += tile_width;
        }
        out += pic_width * tile_height * 2;
        in += pic_width * tile_height;
      }
      /* 2nd field */
      out = out_bot;
      in = in_bot;
      for (i = 0; i < pic_height / 2; i += tile_height) {
        t = 0;
        s = 0;
        for (j = 0; j < pic_width; j += tile_width) {
          /* copy this tile */
          for (k = 0; k < tile_height; ++k) {
            for (l = 0; l < tile_width; ++l) {
              out[k * 2 * pic_width + l + s] = in[t++];
            }
          }

          /* move to next horizontal tile */
          s += tile_width;
        }
        out += pic_width * tile_height * 2;
        in += pic_width * tile_height;
      }
    }
  }
}

/*------------------------------------------------------------------------------

    Function name: TBFieldDpbToFrameDpb

        Functional description:
            test bench common function to convert tiled mode ref picture to
            raster scan picture

        Inputs:

        Outputs:

        Returns:

------------------------------------------------------------------------------*/
void TBFieldDpbToFrameDpb(u32 dpb_mode, u8 *in, u8 *out, u32 pic_width,
                          u32 pic_height) {
  u32 i, j;
  u32 k, l;
  u32 s, t;

  if (dpb_mode == 1) {
    u8 *out_bot = out + pic_width;
    u8 *in_bot = in + (pic_width * pic_height / 2);
    /* 1st field */
    for (i = 0; i < pic_height / 2; i += 1) {
      t = 0;
      s = 0;
      for (j = 0; j < pic_width; j += 1) {
        /* copy this tile */
        for (k = 0; k < 1; ++k) {
          for (l = 0; l < 1; ++l) {
            out[k * 2 * pic_width + l + s] = in[t++];
          }
        }

        /* move to next horizontal tile */
        s += 1;
      }
      out += pic_width * 1 * 2;
      in += pic_width * 1;
    }
    /* 2nd field */
    out = out_bot;
    in = in_bot;
    for (i = 0; i < pic_height / 2; i += 1) {
      t = 0;
      s = 0;
      for (j = 0; j < pic_width; j += 1) {
        /* copy this tile */
        for (k = 0; k < 1; ++k) {
          for (l = 0; l < 1; ++l) {
            out[k * 2 * pic_width + l + s] = in[t++];
          }
        }

        /* move to next horizontal tile */
        s += 1;
      }
      out += pic_width * 1 * 2;
      in += pic_width * 1;
    }
  }
}
