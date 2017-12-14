/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef ERRORHANDLING_H_DEFINED
#define ERRORHANDLING_H_DEFINED

#include "basetype.h"

#ifndef HANTRO_TRUE
#define HANTRO_TRUE (1)
#endif /* HANTRO_TRUE */

#ifndef HANTRO_FALSE
#define HANTRO_FALSE (0)
#endif /* HANTRO_FALSE*/

void PreparePartialFreeze(u8 *dec_out, u32 vop_width, u32 vop_height);
u32 ProcessPartialFreeze(u8 *dec_out, u8 *ref_pic, u32 vop_width,
                         u32 vop_height, u32 copy);

#endif /* ERRORHANDLING_H_DEFINED */
