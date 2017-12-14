#ifndef _COMMON_H_
#define _COMMON_H_

#include <assert.h>

#define COMMENT(x)
#define ASSERT(expr) assert(expr)


typedef enum
{
    ENCHW_NOK = -1,
    ENCHW_OK = 0
} bool_e;

typedef enum
{
    ENCHW_NO = 0,
    ENCHW_YES = 1
} true_e;

typedef struct
{
    u8 *stream;
    u32 size;
    u32 byteCnt;
    u32 overflow;
    u32 byteBuffer;
}stream_s;

#endif
