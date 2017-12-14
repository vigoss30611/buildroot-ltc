/*  Copyright 2013 Google Inc. All Rights Reserved. */
#ifndef __DWL_ACTIVITY_TRACE_H__
#define __DWL_ACTIVITY_TRACE_H__

#include <stdio.h>
#include <sys/time.h>
#include "basetype.h"

struct ActivityTrace {
  struct timeval start;
  struct timeval stop;
  unsigned long active_time;
  unsigned long idle_time;
  unsigned long start_count;
};

u32 ActivityTraceInit(struct ActivityTrace* inst);
u32 ActivityTraceRelease(struct ActivityTrace* inst);
u32 ActivityTraceStartDec(struct ActivityTrace* inst);
u32 ActivityTraceStopDec(struct ActivityTrace* inst);

#endif /* __DWL_ACTIVITY_TRACE_H__ */
