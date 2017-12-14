#ifndef __basetype_h__
#define __basetype_h__

#include <stdint.h>

typedef unsigned char           uint8;
typedef char		        sint8;
typedef unsigned char           uchar;
typedef short			sint16;
typedef unsigned short          uint16;
typedef int			sint32;
typedef unsigned int            uint32;
typedef unsigned long long      uint64;
typedef long long		sint64;

#ifndef WIN32

typedef unsigned char BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

typedef unsigned char UCHAR;
typedef uint16_t USHORT;
typedef uint32_t UINT;
typedef unsigned long ULONG;

#define BOOL	int
#define TRUE	1
#define FALSE	0

#endif

#endif
