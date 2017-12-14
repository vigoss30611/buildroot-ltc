/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/md5_sink.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "software/test/common/swhw/md5.h"

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
  u32 w = pic.sequence_info.pic_width;
  u32 h = pic.sequence_info.pic_height;
  u32 num_of_pixels = w * h;
  MD5Update(&md5sink->ctx, pic.g2_luma.virtual_address, num_of_pixels);
  /* round odd picture dimensions to next multiple of two for chroma */
  if (w & 1) w += 1;
  if (h & 1) h += 1;
  num_of_pixels = w * h;
  MD5Update(&md5sink->ctx, pic.g2_chroma.virtual_address, num_of_pixels / 2);
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
  u32 w = pic.sequence_info.pic_width;
  u32 h = pic.sequence_info.pic_height;
  u32 num_of_pixels = w * h;
  unsigned char digest[16];
  struct MD5Context ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, pic.g2_luma.virtual_address, num_of_pixels);
  /* round odd picture dimensions to next multiple of two for chroma */
  if (w & 1) w += 1;
  if (h & 1) h += 1;
  num_of_pixels = w * h;
  MD5Update(&ctx, pic.g2_chroma.virtual_address, num_of_pixels / 2);
  MD5Final(digest, &ctx);
  for (int i = 0; i < sizeof(digest); i++) {
    fprintf(md5sink->file, "%02X", digest[i]);
  }
  fprintf(md5sink->file, "\n");
  fflush(md5sink->file);
}
