/*  Copyright 2013 Google Inc. All Rights Reserved. */
#include "basetype.h"
#include "dwl_activity_trace.h"
#include "dwl.h"

u32 ActivityTraceInit(struct ActivityTrace* inst) {
#ifdef _DWL_ENABLE_ACTIVITY_TRACE
  if (inst == NULL) return 1;

  DWLmemset(inst, 0, sizeof(struct ActivityTrace));
#endif
  return 0;
}

u32 ActivityTraceStartDec(struct ActivityTrace* inst) {
#ifdef _DWL_ENABLE_ACTIVITY_TRACE
  if (inst == NULL) return 1;
  gettimeofday(&inst->start, NULL);

  if (inst->stop.tv_usec + inst->stop.tv_sec) {
    unsigned long idle = 1000000 * inst->start.tv_sec + inst->start.tv_usec -
                         1000000 * inst->stop.tv_sec - inst->stop.tv_usec;
    inst->idle_time += idle / 10;
  }
  inst->start_count++;
#endif
  return 0;
}

u32 ActivityTraceStopDec(struct ActivityTrace* inst) {

#ifdef _DWL_ENABLE_ACTIVITY_TRACE
  unsigned long active;
  if (inst == NULL) return 1;

  gettimeofday(&inst->stop, NULL);

  active = 1000000 * inst->stop.tv_sec + inst->stop.tv_usec -
           1000000 * inst->start.tv_sec - inst->start.tv_usec;
  inst->active_time += active / 10;
#endif
  return 0;
}

u32 ActivityTraceRelease(struct ActivityTrace* inst) {
#ifdef _DWL_ENABLE_ACTIVITY_TRACE
  if (inst == NULL) return 1;

  if (inst->active_time || inst->idle_time) {
    printf("\nHardware active/idle statistics:\n");
    printf("Active: %9lu msec\n", inst->active_time / 100);
    printf("Idle: %11lu msec\n", inst->idle_time / 100);
    if (inst->active_time + inst->idle_time)
      printf("Hardware utilization: %lu%\n",
             inst->active_time / ((inst->active_time + inst->idle_time) / 100));
    printf("Core was enabled %lu times.\n", inst->start_count);
  }
#endif
  return 0;
}
