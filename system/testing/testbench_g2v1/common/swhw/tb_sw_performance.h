/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Interface to measuring SW performance. */

#ifndef TB_SW_PERFORMANCE_H
#define TB_SW_PERFORMANCE_H

#include "basetype.h"
#include "time.h"

#ifdef SW_PERFORMANCE
#define INIT_SW_PERFORMANCE   \
  double dec_cpu_time = 0;    \
  clock_t dec_start_time = 0; \
  clock_t dec_end_time = 0;
#else
#define INIT_SW_PERFORMANCE
#endif

#ifdef SW_PERFORMANCE
#define START_SW_PERFORMANCE dec_start_time = clock();
#else
#define START_SW_PERFORMANCE
#endif

#ifdef SW_PERFORMANCE
#define END_SW_PERFORMANCE \
  dec_end_time = clock();  \
  dec_cpu_time += ((double)(dec_end_time - dec_start_time)) / CLOCKS_PER_SEC;
#else
#define END_SW_PERFORMANCE
#endif

#ifdef SW_PERFORMANCE
#define FINALIZE_SW_PERFORMANCE printf("SW_PERFORMANCE %0.5f\n", dec_cpu_time);
#else
#define FINALIZE_SW_PERFORMANCE
#endif

#ifdef SW_PERFORMANCE
#define FINALIZE_SW_PERFORMANCE_PP \
  printf("SW_PERFORMANCE_PP %0.5f\n", dec_cpu_time);
#else
#define FINALIZE_SW_PERFORMANCE_PP
#endif

#endif /* TB_SW_PERFORMANCE_H */
