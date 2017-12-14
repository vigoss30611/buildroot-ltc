#ifndef _JPEG_ENC_API_H_
#define _JPEG_ENC_API_H_

#include "basetype.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef const void *JpegEncInst;

#ifdef __GNUC__
#define EXPORT __attribute__((visibility("default")))   //just for gcc4+
#else
#define EXPORT  __declspec(dllexport)
#endif

typedef enum
{
    JPEGENC_FRAME_READY = 1,
    JPEGENC_OK = 0,
    JPEGENC_ERROR = -1,
    JPEGENC_NULL_ARGUMENT = -2,
    JPEGENC_INVALID_ARGUMENT = -3,
    JPEGENC_MEMORY_ERROR = -4,
    JPEGENC_INVALID_STATUS = -5,
    JPEGENC_OUTPUT_BUFFER_OVERFLOW = -6,
    JPEGENC_HW_TIMEOUT = -7,
    JPEGENC_SYSTEM_ERROR = -8,
    JPEGNEC_INSTANCE_ERROR = -9,
    JPEGENC_HW_RESET = -10,
    JPEGENC_EWL_ERROR = -11
}JpegEncRet;

typedef enum
{
    JPEGENC_NV21 = 0,
    JPEGENC_NV12 = 1

}JpegEncFrameType;

typedef enum
{
    JPEGENC_NO_UNITS = 0,
    JPEGENC_DOTS_PER_INCH = 1,
    JPEGENC_DOTS_PER_CM = 2
}JpegEncAppUnitsType;


typedef enum
{
    JPEGENC_SINGLE_MARKER = 0,
    JPEGENC_MUTI_MARKER
}JpegEncTableMarkerType;

typedef struct
{
    u32 inputWidth;
    u32 inputHeight;
    u32 stride;
    u32 qLevel;

    JpegEncFrameType frameType;
    JpegEncAppUnitsType unitsType;
    u32 xDensity;
    u32 yDensity;
    JpegEncTableMarkerType markerType;
    u32 comLength;
    const u8 *pCom;

}JpegEncCfg;

typedef enum
{
    JPEGENC_THUMB_JPEG = 0x10,  /* Thumbnail coded using JPEG  */
    JPEGENC_THUMB_PALETTE_RGB8 = 0x11,  /* Thumbnail stored using 1 byte/pixel */
    JPEGENC_THUMB_RGB24 = 0x13  /* Thumbnail stored using 3 bytes/pixel */
} JpegEncThumbFormat;

/* thumbnail info */
typedef struct
{
    JpegEncThumbFormat format;  /* Format of the thumbnail */
    u8 width;            /* Width in pixels of thumbnail */
    u8 height;           /* Height in pixels of thumbnail */
    const void *data;    /* Thumbnail data */
    u16 dataLength;      /* Data amount in bytes */
} JpegEncThumb;

typedef struct
{
    u32 frameHeader;
    u32 busLum;
    u32 busCb;
    u32 busCr;
    const u8 *pLum;
    const u8 *pCb;
    const u8 *pCr;
    u8 *pOutBuf;
    u32 busOutBuf;
    u32 outBufSize;
    u32 app1;            /* Enable/disable creation of app1 xmp          */
    u8 *xmpId;           /* Pointer to app1 xmp identify                 */
    u8 *xmpData;         /* Pointer to app1 xmp content                  */
}JpegEncIn;

typedef struct
{
    u32 jfifSize;
}JpegEncOut;


EXPORT JpegEncRet JpegEncInit(const JpegEncCfg *pEncCfg, JpegEncInst *instAddr);
EXPORT JpegEncRet JpegEncEncodeSyn(JpegEncInst inst, const JpegEncIn * pEncIn, JpegEncOut * pEncOut);
EXPORT JpegEncRet JpegEncRelease(JpegEncInst inst);
EXPORT JpegEncRet JpegEncSetPictureSize(JpegEncInst inst, const JpegEncCfg *pEncCfg);
EXPORT JpegEncRet JpegEncOff(JpegEncInst inst);
EXPORT JpegEncRet JpegEncOn(JpegEncInst inst);
EXPORT JpegEncRet JpegEncEncode(JpegEncInst inst, const JpegEncIn * pEncIn,JpegEncOut * pEncOut);
EXPORT JpegEncRet JpegEncInitSyn(const JpegEncCfg * pEncCfg, JpegEncInst * instAddr);
EXPORT JpegEncRet JpegEncSetQuality(JpegEncInst instAddr, const JpegEncCfg * pEncCfg);
EXPORT JpegEncRet JpegEncReleaseSyn(JpegEncInst inst);
EXPORT JpegEncRet JpegEncSetThumbnail(JpegEncInst inst, const JpegEncThumb *pJpegThumb);




//void JpegEnc_Trace(const char *msg);


#ifdef __cplusplus
}
#endif
#endif
