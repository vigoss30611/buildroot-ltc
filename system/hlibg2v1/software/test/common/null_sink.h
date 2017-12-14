/* Copyright 2013 Google Inc. All Rights Reserved. */

#ifndef __NULL_SINK_H__
#define __NULL_SINK_H__

#include "software/source/inc/dectypes.h"

/* NULL yuv sink for g2 decoder testbench. */
const void* NullsinkOpen(const char* fname);
void NullsinkWrite(const void* inst, struct DecPicture pic);
void NullsinkClose(const void* inst);

#endif /* __NULL_SINK_H__ */
