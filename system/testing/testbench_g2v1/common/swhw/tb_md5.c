/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "tb_md5.h"
#include "md5.h"

/* Calculates MD5 check sum for the provided frame and writes the result to
 * provided file. */
u32 TBWriteFrameMD5Sum(FILE* f_out, u8* yuv, u32 yuv_size, u32 frame_number) {
  unsigned char digest[16];
  struct MD5Context ctx;
  int i = 0;
  MD5Init(&ctx);
  MD5Update(&ctx, yuv, yuv_size);
  MD5Final(digest, &ctx);
  /*    fprintf(f_out, "FRAME %d: ", frame_number);*/
  for (i = 0; i < sizeof digest; i++) {
    fprintf(f_out, "%02X", digest[i]);
  }
  fprintf(f_out, "\n");
  fflush(f_out);
  return 0;
}
