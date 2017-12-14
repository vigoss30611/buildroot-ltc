/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef SW_DEBUG_H_
#define SW_DEBUG_H_

/* macro for assertion, used only when _ASSERT_USED is defined */
#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

/* macros for range checking used only when _RANGE_CHECK is defined */
#ifdef _RANGE_CHECK

#include <stdio.h>

/* macro for range checking an single value */
#define RANGE_CHECK(value, min_bound, max_bound)                   \
  {                                                                \
    if ((value) < (min_bound) || (value) > (max_bound))            \
      fprintf(stderr, "Warning: Value exceeds given limit(s)!\n"); \
  }

/* macro for range checking an array of values */
#define RANGE_CHECK_ARRAY(array, min_bound, max_bound, length)               \
  {                                                                          \
    i32 i;                                                                   \
    for (i = 0; i < (length); i++)                                           \
      if ((array)[i] < (min_bound) || (array)[i] > (max_bound))              \
        fprintf(stderr, "Warning: Value [%d] exceeds given limit(s)!\n", i); \
  }

#else /* _RANGE_CHECK */

#define RANGE_CHECK_ARRAY(array, min_bound, max_bound, length)
#define RANGE_CHECK(value, min_bound, max_bound)

#endif /* _RANGE_CHECK */

/* macro for debug printing, used only when _DEBUG_PRINT is defined */
#ifdef _SW_DEBUG_PRINT
#include <stdio.h>
#define DEBUG_PRINT(args) printf args
#else
#define DEBUG_PRINT(args)
#endif

/* macro for error printing, used only when _ERROR_PRINT is defined */
#ifdef _ERROR_PRINT
#include <stdio.h>
#define ERROR_PRINT(msg) fprintf(stderr, "ERROR: %s\n", msg)
#else
#define ERROR_PRINT(msg)
#endif

#endif /* SW_DEBUG_H_ */
