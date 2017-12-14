#ifndef _JPEGENCCODEFRAME_H_
#define JPEGENCCODEFRAME_H_

#include "basetype.h"
#include "JpegEncInstance.h"

#define OFFSET_JECTRL0      0//0x00000000
#define OFFSET_JESTAT0      1//0x00000004
#define OFFSET_JESRCA       2//0x00000008
#define OFFSET_JESRCUVA     3//0x0000000C
#define OFFSET_JESRCS       4//0x00000010
#define OFFSET_JESZ         5//0x00000014
#define OFFSET_JEOA         6//0x00000018
#define OFFSET_JEOEA        7//0x0000001C
#define OFFSET_JEQCIDX      8//0x00000020
#define OFFSET_JEQCV        9//0x00000024
#define OFFSET_JEOBEA       10//0x00000028
#define OFFSET_JEOCBA       11//0x0000002C
#define OFFSET_JEAXI        12//0x00000030

typedef enum
{
    JPEGENCODE_OK = 0,
    JPEGENCODE_TIMEOUT = 1,
    JPEGENCODE_DATA_ERROR = 2,
    JPEGENCODE_HW_ERROR = 3,
    JPEGENCODE_SYSTEM_ERROR = 4,
    JPEGENCODE_HW_RESET = 5
} jpegEncodeFrame_e;

jpegEncodeFrame_e EncJpegCodeFrame(jpegInstance_s * inst);
#endif

