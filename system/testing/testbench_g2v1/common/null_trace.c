/* Copyright 2013 Google Inc. All Rights Reserved. */

#include <stdio.h>

#include "software/source/inc/basetype.h"

u32 OpenTraceFiles(void) {
  printf(
      "Cannot init trace generation, only available with system model "
      "simulation\n");
  return 0;
}

void CloseTraceFiles(void) { return; }
