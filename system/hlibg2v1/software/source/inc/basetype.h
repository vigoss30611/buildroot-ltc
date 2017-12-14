/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef BASETYPE_H
#define BASETYPE_H

#if defined(__linux__) || defined(WIN32)
#include <stddef.h>
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;
typedef signed int i32;
typedef long long i64;
typedef unsigned long long u64;
typedef unsigned long addr_t;

/* SW decoder 16 bits types */
#if defined(VC1SWDEC_16BIT) || defined(MP4ENC_ARM11)
typedef unsigned short u16x;
typedef signed short i16x;
#else
typedef unsigned int u16x;
typedef signed int i16x;
#endif

#endif /* BASETYPE_H */
