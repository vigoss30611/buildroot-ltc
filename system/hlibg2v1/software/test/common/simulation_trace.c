/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/source/inc/basetype.h"
#include "software/test/common/swhw/trace.h"

u32 OpenTraceFiles(void) { return OpenAsicTraceFiles(); }

void CloseTraceFiles(void) { CloseAsicTraceFiles(); }
