/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------*/

#ifndef BASE_TYPE_H
#define BASE_TYPE_H

#include <stdint.h>
#include <stdio.h>
#ifndef NDEBUG
#include <assert.h>
#endif

typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t   i16;
typedef uint16_t  u16;
typedef int32_t   i32;
typedef uint32_t  u32;
typedef int64_t   i64;
typedef uint64_t  u64;

#define INLINE inline

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else /*  */
#define NULL    ((void *)0)
#endif /*  */
#endif

typedef       short               Short;
typedef       int                 Int;
typedef       unsigned int        UInt;


#ifndef __cplusplus
/*enum {
  false = 0,
  true  = 1
};
typedef _Bool           bool;*/
typedef enum
{
  false = 0,
  true  = 1
} bool;

enum
{
  OK  = 0,
  NOK = -1
};
#endif

/* ASSERT */
#ifndef NDEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#endif
