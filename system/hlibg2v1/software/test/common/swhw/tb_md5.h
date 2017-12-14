/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef TB_MD5_H
#define TB_MD5_H

#include "basetype.h"
#include <stdio.h>

extern u32 TBWriteFrameMD5Sum(FILE* f_out, u8* yuv, u32 yuv_size,
                              u32 frame_number);

#endif
