/* Copyright 2013 Google Inc. All Rights Reserved. */

#include <stdio.h>
#include "software/test/common/file_sink.h"

#include <stdlib.h>
#include <string.h>

struct FileSink {
  u8* frame_pic;
  FILE* file;
  char filename[FILENAME_MAX];
};
typedef long off_t ;
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
#ifndef NO_TB_PP
  u32 w, h, num_of_pixels;
  u8 pixel_bytes = (pic.sequence_info.bit_depth_luma == 8 && pic.sequence_info.bit_depth_chroma == 8) ? 1 : 2;
  u8* p = (u8*)pic.g2_luma.virtual_address;
  w = pic.sequence_info.pic_width;
  h = pic.sequence_info.pic_height;
  num_of_pixels = w * h;
#if 0
  static int debug_write_file = 0;
  debug_write_file++;
  FILE *fp=0 ;
  if(1)
  {
        char fname[100];
        sprintf(fname, "/tmp/nfs/11%d.yuv", debug_write_file);
        fp = fopen(fname, "wb");
        if(fp != 0)fwrite(p, pixel_bytes, num_of_pixels, fp);
        printf("FilesinkWritePic write y %d, file:%s, y_address:%x, y_phyAddr:%x\n", 
            num_of_pixels, fname, p, pic.g2_luma.bus_address);
  }
  else   printf("FilesinkWritePic no write y %d\n", num_of_pixels);  
#endif 
  fwrite(p, pixel_bytes, num_of_pixels, output->file);
  
  p += num_of_pixels;
  /* round odd picture dimensions to next multiple of two for chroma */
  if (w & 1) w += 1;
  if (h & 1) h += 1;
  num_of_pixels = w * h;
  fwrite(pic.g2_chroma.virtual_address, pixel_bytes, num_of_pixels / 2, output->file);
#if 0
  if(fp !=0 )
  {
    fwrite(pic.g2_chroma.virtual_address, pixel_bytes, num_of_pixels / 2, fp);  
    printf("FilesinkWritePic write uv sz%d, uv_address:%x, uv_phyAddr:%x\n", 
        num_of_pixels/2, pic.g2_chroma.virtual_address, pic.g2_chroma.bus_address);
    fclose(fp);
    fp = 0;
  }
#endif 
#else
  u8 pixel_width = (pic.sequence_info.bit_depth_luma == 8 && pic.sequence_info.bit_depth_chroma == 8) ? 8 : 10;
  u8* p = (u8*)pic.g2_luma.virtual_address;
  u32 s = pic.sequence_info.pic_stride;
  u32 w = (pic.sequence_info.pic_width * pixel_width + 7) / 8;
  u32 h = pic.sequence_info.pic_height;
  for (u32 i = 0; i < h; i++) {
    fwrite(p, 1, w, output->file);
    p += s;
  }
  /* round odd picture dimensions to next multiple of two for chroma */
  p = (u8*)pic.g2_chroma.virtual_address;
  for (u32 i = 0; i < h / 2; i++) {
    fwrite(p, 1, w, output->file);
    
    printf("FilesinkWritePic write %d\n", w);
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
    fwrite(pic.g2_luma.virtual_address, 1, num_of_pixels, fp);
    /* round odd picture dimensions to next multiple of two for chroma */
    if (w & 1) w += 1;
    if (h & 1) h += 1;
    num_of_pixels = w * h;
    fwrite(pic.g2_chroma.virtual_address, 1, num_of_pixels / 2, fp);
    printf("FilesinkWriteSinglePic write %d\n", num_of_pixels / 2);
    fclose(fp);
  }
}
