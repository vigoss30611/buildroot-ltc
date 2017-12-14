/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/null_sink.h"

#include <stdlib.h>

struct NullSink {
  u32 placeholder;
};

const void* NullsinkOpen(const char* fname) {
  // Just a placeholder.
  return malloc(sizeof(struct NullSink));
}

void NullsinkWrite(const void* inst, struct DecPicture pic) {}

void NullsinkClose(const void* inst) { free((void*)inst); }
