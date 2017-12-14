/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Definition of image representation. */

#ifndef HEVC_IMAGE_H_
#define HEVC_IMAGE_H_

#include "basetype.h"
#include "dwl.h"

struct Image {
  struct DWLLinearMem *data;
  u32 width;
  u32 height;
  u32 pic_struct;
};

#endif /* #ifdef HEVC_IMAGE_H_ */
