/* Copyright 2013 Google Inc. All Rights Reserved. */

#ifndef YUVFILTERS_H

#include "software/source/inc/dectypes.h"

void YuvfilterTiled2Semiplanar(struct DecPicture* pic);
void YuvfilterTiled2Planar(struct DecPicture* pic);
void YuvfilterTiledcrop(struct DecPicture* pic);
void YuvfilterSemiplanarcrop(struct DecPicture* pic);
void YuvfilterSemiplanar2Planar(struct DecPicture* pic);
void YuvfilterConvertPixel16Bits(struct DecPicture *pic, struct DecPicture *dst);
void YuvfilterPrepareOutput(struct DecPicture *pic);

#endif /* YUVFILTERS_H */
