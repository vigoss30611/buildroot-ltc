/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/md5_sink.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "software/test/common/swhw/md5.h"
#include "sw_util.h"  /* NEXT_MULTIPLE */

struct MD5Sink {
  FILE* file;
  char filename[FILENAME_MAX];
  struct MD5Context ctx;
};

const void* Md5sinkOpen(const char* fname) {
  struct MD5Sink* inst = calloc(1, sizeof(struct MD5Sink));
  if (inst == NULL) return NULL;
  inst->file = fopen(fname, "wb");
  if (inst->file == NULL) {
    free(inst);
    return NULL;
  }
  strcpy(inst->filename, fname);
  MD5Init(&inst->ctx);
  return inst;
}

void Md5sinkClose(const void* inst) {
  struct MD5Sink* md5sink = (struct MD5Sink*)inst;
  if (md5sink->file != NULL) {
    u32 file_size;
    unsigned char digest[16];
    MD5Final(digest, &md5sink->ctx);
    for (int i = 0; i < sizeof digest; i++) {
      fprintf(md5sink->file, "%02x", digest[i]);
    }
    fprintf(md5sink->file, "  %s\n", md5sink->filename);
    fflush(md5sink->file);
    /* Close the file and if it is empty, remove it. */
    fseek(md5sink->file, 0, SEEK_END);
    file_size = ftell(md5sink->file);
    fclose(md5sink->file);
    if (file_size == 0) remove(md5sink->filename);
  }
  free(md5sink);
}

void Md5sinkWritePic(const void* inst, struct DecPicture pic) {
  struct MD5Sink* md5sink = (struct MD5Sink*)inst;
#ifdef TB_PP
  u32 w = pic.sequence_info.pic_width;
  u32 h = pic.sequence_info.pic_height;
  u8 pixel_bytes = ((pic.sequence_info.bit_depth_luma == 8 && pic.sequence_info.bit_depth_chroma == 8) ||
                     pic.picture_info.pixel_format == DEC_OUT_PIXEL_CUT_8BIT) ? 1 : 2;
  u32 num_of_pixels = w * h;
  MD5Update(&md5sink->ctx, pic.luma.virtual_address, num_of_pixels * pixel_bytes);
  /* round odd picture dimensions to next multiple of two for chroma */
  if (w & 1) w += 1;
  if (h & 1) h += 1;
  num_of_pixels = w * h;
  MD5Update(&md5sink->ctx, pic.chroma.virtual_address, num_of_pixels / 2 * pixel_bytes);
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
    MD5Update(&md5sink->ctx, p, w);
    if (extra_bits) {
      u8 last_byte = p[w];
      last_byte &= (1 << extra_bits) - 1;
      MD5Update(&md5sink->ctx, &last_byte, 1);
    }
    p += s;
  }
  /* round odd picture dimensions to next multiple of two for chroma */
  p = (u8*)pic.chroma.virtual_address;
  for (u32 i = 0; i < h / 2; i++) {
    MD5Update(&md5sink->ctx, p, w);
    if (extra_bits) {
      u8 last_byte = p[w];
      last_byte &= (1 << extra_bits) - 1;
      MD5Update(&md5sink->ctx, &last_byte, 1);
    }
    p += s;
  }
#endif
}

const void* md5perpicsink_open(const char* fname) {
  struct MD5Sink* inst = calloc(1, sizeof(struct MD5Sink));
  if (inst == NULL) return NULL;
  inst->file = fopen(fname, "wb");
  if (inst->file == NULL) {
    free(inst);
    return NULL;
  }
  strcpy(inst->filename, fname);
  return inst;
}

void md5perpicsink_close(const void* inst) {
  struct MD5Sink* md5sink = (struct MD5Sink*)inst;
  if (md5sink->file != NULL) {
    /* Close the file and if it is empty, remove it. */
    u32 file_size;
    fseek(md5sink->file, 0, SEEK_END);
    file_size = ftell(md5sink->file);
    fclose(md5sink->file);
    if (file_size == 0) remove(md5sink->filename);
  }
  free(md5sink);
}

void md5perpicsink_write_pic(const void* inst, struct DecPicture pic) {
  struct MD5Sink* md5sink = (struct MD5Sink*)inst;
  struct MD5Context ctx;
#ifdef TB_PP
  u32 w = pic.sequence_info.pic_width;
  u32 h = pic.sequence_info.pic_height;
  u32 num_of_pixels = w * h;
  unsigned char digest[16];
  u8 pixel_bytes = ((pic.sequence_info.bit_depth_luma == 8 && pic.sequence_info.bit_depth_chroma == 8) ||
                     pic.picture_info.pixel_format == DEC_OUT_PIXEL_CUT_8BIT) ? 1 : 2;
  MD5Init(&ctx);
  MD5Update(&ctx, pic.luma.virtual_address, num_of_pixels * pixel_bytes);
  /* round odd picture dimensions to next multiple of two for chroma */
  if (w & 1) w += 1;
  if (h & 1) h += 1;
  num_of_pixels = w * h;
  MD5Update(&ctx, pic.chroma.virtual_address, num_of_pixels / 2 * pixel_bytes);
  MD5Final(digest, &ctx);
#else
  u8 pixel_width = ((pic.sequence_info.bit_depth_luma == 8 && pic.sequence_info.bit_depth_chroma == 8) ||
                     pic.picture_info.pixel_format == DEC_OUT_PIXEL_CUT_8BIT) ? 8 :
                     (pic.picture_info.pixel_format == DEC_OUT_PIXEL_P010) ? 16 : 10;
  u8* p = (u8*)pic.luma.virtual_address;
  u32 s = pic.sequence_info.pic_stride;
  u32 w = pic.sequence_info.pic_width * pixel_width / 8;
  u32 h = pic.sequence_info.pic_height;
  u32 extra_bits = pic.sequence_info.pic_width * pixel_width & 7;
  unsigned char digest[16];

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

  MD5Init(&ctx);

  p = (u8*)pic.luma.virtual_address;
  for (u32 i = 0; i < h; i++) {
    MD5Update(&ctx, p, w);
    if (extra_bits) {
      u8 last_byte = p[w];
      last_byte &= (1 << extra_bits) - 1;
      MD5Update(&ctx, &last_byte, 1);
    }
    p += s;
  }
  /* round odd picture dimensions to next multiple of two for chroma */
  p = (u8*)pic.chroma.virtual_address;
  for (u32 i = 0; i < h / 2; i++) {
    MD5Update(&ctx, p, w);
    if (extra_bits) {
      u8 last_byte = p[w];
      last_byte &= (1 << extra_bits) - 1;
      MD5Update(&ctx, &last_byte, 1);
    }
    p += s;
  }
  MD5Final(digest, &ctx);
#endif
  for (int i = 0; i < sizeof(digest); i++) {
    fprintf(md5sink->file, "%02X", digest[i]);
  }
  fprintf(md5sink->file, "\n");
  fflush(md5sink->file);
}
