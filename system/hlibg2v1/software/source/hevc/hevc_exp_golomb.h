/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Exp-Golomb code decoding. */

#ifndef HEVC_EXP_GOLOMB_H_
#define HEVC_EXP_GOLOMB_H_

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "sw_stream.h"

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

u32 HevcDecodeExpGolombUnsigned(struct StrmData *stream, u32 *value);
u32 HevcDecodeExpGolombSigned(struct StrmData *stream, i32 *value);

#endif /* #ifdef HEVC_EXP_GOLOMB_H_ */
