/* Copyright 2013 Google Inc. All Rights Reserved. */

#ifndef __SDL_SINK_H__
#define __SDL_SINK_H__

#include "software/source/inc/dectypes.h"

/* SDL yuv sink for g2 decoder testbench. */
const void* SdlSinkOpen(const char* fname);
void SdlSinkWrite(const void* inst, struct DecPicture pic);
void SdlSinkClose(const void* inst);

#endif /* __SDL_SINK_H__ */
