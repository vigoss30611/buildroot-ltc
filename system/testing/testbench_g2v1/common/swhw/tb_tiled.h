/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Converts data to tiled format. */

#include <stdio.h>
#include "basetype.h"

extern void TbWriteTiledOutput(FILE *file, u8 *data, u32 mb_width,
                               u32 mb_height, u32 pic_num, u32 md5_sum_output,
                               u32 input_format);

extern void TbChangeEndianess(u8 *data, u32 data_size);

void TBTiledToRaster(u32 tile_mode, u32 dpb_mode, u8 *in, u8 *out,
                     u32 pic_width, u32 pic_height);
void TBFieldDpbToFrameDpb(u32 dpb_mode, u8 *in, u8 *out, u32 pic_width,
                          u32 pic_height);
