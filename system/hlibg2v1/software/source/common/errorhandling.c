/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "errorhandling.h"
#include "dwl.h"
#include "deccfg.h"

#define MAGIC_WORD_LENGTH (8)

static const u8 magic_word[MAGIC_WORD_LENGTH] = "Rosebud\0";

#define NUM_OFFSETS 6

static const u32 row_offsets[] = {1, 2, 4, 8, 12, 16};

static u32 GetMbOffset(u32 mb_num, u32 vop_width, u32 vop_height);

u32 GetMbOffset(u32 mb_num, u32 vop_width, u32 vop_height) {
  u32 mb_row, mb_col;
  u32 offset;

  mb_row = mb_num / vop_width;
  mb_col = mb_num % vop_width;
  offset = mb_row * 16 * 16 * vop_width + mb_col * 16;

  return offset;
}

/* Copy num_rows bottom mb rows from ref_pic to dec_out. */
void CopyRows(u32 num_rows, u8 *dec_out, u8 *ref_pic, u32 vop_width,
              u32 vop_height) {

  u32 pix_width;
  u32 offset;
  u32 luma_size;
  u8 *src;
  u8 *dst;

  pix_width = 16 * vop_width;

  offset = (vop_height - num_rows) * 16 * pix_width;
  luma_size = 256 * vop_width * vop_height;

  dst = dec_out;
  src = ref_pic;

  dst += offset;
  src += offset;

  if (ref_pic)
    DWLPrivateAreaMemcpy(dst, src, num_rows * 16 * pix_width);
  else
    DWLPrivateAreaMemset(dst, 0, num_rows * 16 * pix_width);

  /* Chroma data */
  offset = (vop_height - num_rows) * 8 * pix_width;

  dst = dec_out;
  src = ref_pic;

  dst += luma_size;
  src += luma_size;
  dst += offset;
  src += offset;

  if (ref_pic)
    DWLPrivateAreaMemcpy(dst, src, num_rows * 8 * pix_width);
  else
    DWLPrivateAreaMemset(dst, 128, num_rows * 8 * pix_width);
}

void PreparePartialFreeze(u8 *dec_out, u32 vop_width, u32 vop_height) {

  u32 i, j;
  u8 *base;

  for (i = 0; i < NUM_OFFSETS && row_offsets[i] < vop_height / 4 &&
                  row_offsets[i] <= DEC_X170_MAX_EC_COPY_ROWS;
       i++) {
    base = dec_out + GetMbOffset(vop_width * (vop_height - row_offsets[i]),
                                 vop_width, vop_height);

    for (j = 0; j < MAGIC_WORD_LENGTH; ++j) base[j] = magic_word[j];
  }
}

u32 ProcessPartialFreeze(u8 *dec_out, u8 *ref_pic, u32 vop_width,
                         u32 vop_height, u32 copy) {

  u32 i, j;
  u8 *base;
  u32 num_mbs;
  u32 match = HANTRO_TRUE;

  num_mbs = vop_width * vop_height;

  for (i = 0; i < NUM_OFFSETS && row_offsets[i] < vop_height / 4 &&
                  row_offsets[i] <= DEC_X170_MAX_EC_COPY_ROWS;
       i++) {
    base = dec_out + GetMbOffset(vop_width * (vop_height - row_offsets[i]),
                                 vop_width, vop_height);

    for (j = 0; j < MAGIC_WORD_LENGTH && match; ++j)
      if (base[j] != magic_word[j]) match = HANTRO_FALSE;

    if (!match) {
      if (copy)
        CopyRows(row_offsets[i], dec_out, ref_pic, vop_width, vop_height);
      return HANTRO_TRUE;
    }
  }

  return HANTRO_FALSE;
}
