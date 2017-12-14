/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Common definitions and utility functions. */

#ifndef SW_UTIL_H_
#define SW_UTIL_H_

#include "basetype.h"
#include "dwl.h"

#define HANTRO_OK 0
#define HANTRO_NOK 1

#define HANTRO_FALSE (0U)
#define HANTRO_TRUE (1U)

#define MEMORY_ALLOCATION_ERROR 0xFFFF
#define PARAM_SET_ERROR 0xFFF0

/* value to be returned by GetBits if stream buffer is empty */
#define END_OF_STREAM 0xFFFFFFFFU

/* macro to get smaller of two values */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* macro to get greater of two values */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* macro to get absolute value */
#define ABS(a) (((a) < 0) ? -(a) : (a))

/* macro to clip a value z, so that x <= z =< y */
#define CLIP3(x, y, z) (((z) < (x)) ? (x) : (((z) > (y)) ? (y) : (z)))

/* macro to clip a value z, so that 0 <= z =< 255 */
#define CLIP1(z) (((z) < 0) ? 0 : (((z) > 255) ? 255 : (z)))

/* macro to allocate memory */
#define ALLOCATE(ptr, count, type) \
  { ptr = DWLmalloc((count) * sizeof(type)); }

/* macro to free allocated memory */
#define FREE(ptr)      \
  {                    \
    if (ptr != NULL) { \
      DWLfree(ptr);    \
      ptr = NULL;      \
    }                  \
  }

/* round to next multiple of n */
#define NEXT_MULTIPLE(value, n) (((value) + (n) - 1) & ~((n) - 1))

u32 SwCountLeadingZeros(u32 value, u32 length);
u32 SwNumBits(u32 value);
u8* SwTurnAround(const u8 * strm, const u8* buf, u8* tmp_buf, u32 buf_size, u32 num_bits);

#endif /* #ifdef SW_UTIL_H_ */
