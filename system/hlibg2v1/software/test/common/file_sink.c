/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/file_sink.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sw_util.h"  /* NEXT_MULTIPLE */

typedef long off_t;

struct FileSink {
  u8* frame_pic;
  FILE* file;
  char filename[FILENAME_MAX];
};

const void* FilesinkOpen(const char* fname) {
  struct FileSink* inst = calloc(1, sizeof(struct FileSink));
  if (inst == NULL) return NULL;
  inst->file = fopen(fname, "wb");
  if (inst->file == NULL) {
    free(inst);
    return NULL;
  }
  strcpy(inst->filename, fname);
  return inst;
}

void FilesinkClose(const void* inst) {
  struct FileSink* output = (struct FileSink*)inst;
  if (output->file != NULL) {
    /* Close the file and if it is empty, remove it. */
    off_t file_size;
    fseeko(output->file, 0, SEEK_END);
    file_size = ftello(output->file);
    fclose(output->file);
    if (file_size == 0) remove(output->filename);
  }
  if (output->frame_pic != NULL) {
    free(output->frame_pic);
  }
  free(output);
}

void FilesinkWritePic(const void* inst, struct DecPicture pic) {
  struct FileSink* output = (struct FileSink*)inst;
#ifdef TB_PP
  u32 w, h, num_of_pixels;
  u8 pixel_bytes = ((pic.sequence_info.bit_depth_luma == 8 && pic.sequence_info.bit_depth_chroma == 8) ||
                     pic.picture_info.pixel_format == DEC_OUT_PIXEL_CUT_8BIT) ? 1 : 2;
  u8* p = (u8*)pic.luma.virtual_address;
  w = pic.sequence_info.pic_width;
  h = pic.sequence_info.pic_height;
  num_of_pixels = w * h;
  fwrite(p, pixel_bytes, num_of_pixels, output->file);
  p += num_of_pixels;
  /* round odd picture dimensions to next multiple of two for chroma */
  if (w & 1) w += 1;
  if (h & 1) h += 1;
  num_of_pixels = w * h;
  fwrite(pic.chroma.virtual_address, pixel_bytes, num_of_pixels / 2, output->file);
#else
  u8 pixel_width = ((pic.sequence_info.bit_depth_luma == 8 && pic.sequence_info.bit_depth_chroma == 8) ||
                     pic.picture_info.pixel_format == DEC_OUT_PIXEL_CUT_8BIT) ? 8 :
                     (pic.picture_info.pixel_format == DEC_OUT_PIXEL_P010) ? 16 : 10;
  u8* p = (u8*)pic.luma.virtual_address;
  u32 s = pic.sequence_info.pic_stride;
  u32 w = pic.sequence_info.pic_width * pixel_width / 8;
  u32 h = pic.sequence_info.pic_height;
  u32 extra_bits = pic.sequence_info.pic_width * pixel_width & 7;
  if (pic.picture_info.pixel_format == DEC_OUT_PIXEL_CUSTOMER1) {
    /* In customized format, fill 0 of padding bytes/bits in last 128-bit burst. */
    u32 padding_bytes, padding_bits, bursts, fill_bits;
    u8 *last_burst;
    bursts = pic.sequence_info.pic_width * pixel_width / 128;
    fill_bits = NEXT_MULTIPLE(pic.sequence_info.pic_width * pixel_width, 128)
                - pic.sequence_info.pic_width * pixel_width;
    padding_bytes = fill_bits / 8;
    padding_bits = fill_bits & 7;
    for (u32 i = 0; i < h; i++) {
      last_burst = p + bursts * 16;
      memset(last_burst, 0, padding_bytes);
      if (padding_bits) {
        u8 b = last_burst[padding_bytes];
        b &= ~((1 << padding_bits) - 1);
        last_burst[padding_bytes] = b;
      }
      p += s;
    }
    p = (u8*)pic.chroma.virtual_address;
    for (u32 i = 0; i < h / 2; i++) {
      last_burst = p + bursts * 16;
      memset(last_burst, 0, padding_bytes);
      if (padding_bits) {
        u8 b = last_burst[padding_bytes];
        b &= ~((1 << padding_bits) - 1);
        last_burst[padding_bytes] = b;
      }
      p += s;
    }

    w = NEXT_MULTIPLE(pic.sequence_info.pic_width * pixel_width, 128) / 8;
    extra_bits = 0;
  }

  p = (u8*)pic.luma.virtual_address;
  for (u32 i = 0; i < h; i++) {
    fwrite(p, 1, w, output->file);
    if (extra_bits) {
      u8 last_byte = p[w];
      last_byte &= (1 << extra_bits) - 1;
      fwrite(&last_byte, 1, 1, output->file);
    }
    p += s;
  }
  /* round odd picture dimensions to next multiple of two for chroma */
  p = (u8*)pic.chroma.virtual_address;
  for (u32 i = 0; i < h / 2; i++) {
    fwrite(p, 1, w, output->file);
    if (extra_bits) {
      u8 last_byte = p[w];
      last_byte &= (1 << extra_bits) - 1;
      fwrite(&last_byte, 1, 1, output->file);
    }
    p += s;
  }
#endif
}

void FilesinkWriteSinglePic(const void* inst, struct DecPicture pic) {
  static int frame_num = 0;
  char name[FILENAME_MAX];
  FILE* fp = NULL;

  memset(name, 0, sizeof(name));
  frame_num++;
  sprintf(name, "out_%03d_%dx%d.yuv", frame_num, pic.sequence_info.pic_width,
          pic.sequence_info.pic_height);

  fp = fopen(name, "wb");
  if (fp) {
    u32 w = pic.sequence_info.pic_width;
    u32 h = pic.sequence_info.pic_height;
    u32 num_of_pixels = w * h;
    fwrite(pic.luma.virtual_address, 1, num_of_pixels, fp);
    /* round odd picture dimensions to next multiple of two for chroma */
    if (w & 1) w += 1;
    if (h & 1) h += 1;
    num_of_pixels = w * h;
    fwrite(pic.chroma.virtual_address, 1, num_of_pixels / 2, fp);
    fclose(fp);
  }
}
