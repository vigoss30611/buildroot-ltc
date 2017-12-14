#ifndef _JPEGENC_INSTANCE_H_
#define _JPEGENC_INSTANCE_H_
#include "JpegEnc.h"

typedef struct
{
    u32 codingMode;
    u32 picWidth;
    u32 picHeight;
    u32 stride;
    u32 qtLevel;

    u32 inputLumBase;
    u32 inputCbBase;
    u32 inputCrBase;

    u32 outputStrmBase;
    u32 outputStrmSize;
}regValues_s;

typedef struct
{
    const void *ewl;
    regValues_s regs;

}asicData_s;

typedef struct
{
 u32 encStatus;
 stream_s stream;
 jpegData_s jpeg;
 asicData_s asic;
 const void *inst;
}jpegInstance_s;


#endif
