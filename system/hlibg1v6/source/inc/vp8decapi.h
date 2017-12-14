/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2008 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
-
-  Description : VP7/8 Decoder API
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp8decapi.h,v $
-  $Revision: 1.8 $
-  $Date: 2010/12/13 13:04:01 $
-
------------------------------------------------------------------------------*/

#ifndef __VP8DECAPI_H__
#define __VP8DECAPI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "basetype.h"
#include "decapicommon.h"

/*------------------------------------------------------------------------------
    API type definitions
------------------------------------------------------------------------------*/
    /* Return values */
    typedef enum VP8DecRet_
    {
        VP8DEC_OK = 0,
        VP8DEC_STRM_PROCESSED = 1,
        VP8DEC_PIC_RDY = 2,
        VP8DEC_PIC_DECODED = 3,
        VP8DEC_HDRS_RDY = 4,
        VP8DEC_ADVANCED_TOOLS = 5,
        VP8DEC_SLICE_RDY = 6,

        VP8DEC_PARAM_ERROR = -1,
        VP8DEC_STRM_ERROR = -2,
        VP8DEC_NOT_INITIALIZED = -3,
        VP8DEC_MEMFAIL = -4,
        VP8DEC_INITFAIL = -5,
        VP8DEC_HDRS_NOT_RDY = -6,
        VP8DEC_STREAM_NOT_SUPPORTED = -8,

        VP8DEC_HW_RESERVED = -254,
        VP8DEC_HW_TIMEOUT = -255,
        VP8DEC_HW_BUS_ERROR = -256,
        VP8DEC_SYSTEM_ERROR = -257,
        VP8DEC_DWL_ERROR = -258,

        VP8DEC_EVALUATION_LIMIT_EXCEEDED = -999,
        VP8DEC_FORMAT_NOT_SUPPORTED = -1000
    } VP8DecRet;

    /* decoder  output Frame format */
    typedef enum VP8DecOutFormat_
    {
        VP8DEC_SEMIPLANAR_YUV420 = 0x020001,
        VP8DEC_TILED_YUV420 = 0x020002
    } VP8DecOutFormat;

    /* decoder input stream format */
    typedef enum VP8DecFormat_
    {
        VP8DEC_VP7 = 0x01,
        VP8DEC_VP8 = 0x02,
        VP8DEC_WEBP = 0x03
    } VP8DecFormat;

    /* Input structure */
    typedef struct VP8DecInput_
    {
        const u8 *pStream;   /* Pointer to the input data */
        u32 streamBusAddress;   /* DMA bus address of the input stream */
        u32 dataLen;         /* Number of bytes to be decoded         */
        /* used only for WebP */
        u32 sliceHeight;
        u32 *pPicBufferY;    /* luma output address ==> if user allocated */
        u32 picBufferBusAddressY;
        u32 *pPicBufferC;    /* chroma output address ==> if user allocated */
        u32 picBufferBusAddressC;
    } VP8DecInput;

    /* Output structure */
    typedef struct VP8DecOutput_
    {
        u32 unused;
    } VP8DecOutput;

#define VP8_SCALE_MAINTAIN_ASPECT_RATIO     0
#define VP8_SCALE_TO_FIT                    1
#define VP8_SCALE_CENTER                    2
#define VP8_SCALE_OTHER                     3

    /* stream info filled by VP8DecGetInfo */
    typedef struct VP8DecInfo_
    {
        u32 vpVersion;
        u32 vpProfile;
        u32 codedWidth;      /* coded width */
        u32 codedHeight;     /* coded height */
        u32 frameWidth;      /* pixels width of the frame as stored in memory */
        u32 frameHeight;     /* pixel height of the frame as stored in memory */
        u32 scaledWidth;     /* scaled width of the displayed video */
        u32 scaledHeight;    /* scaled height of the displayed video */
        VP8DecOutFormat outputFormat;   /* format of the output frame */
    } VP8DecInfo;

    /* Version information */
    typedef struct VP8DecApiVersion_
    {
        u32 major;           /* API major version */
        u32 minor;           /* API minor version */
    } VP8DecApiVersion;

    typedef struct VP8DecBuild_
    {
        u32 swBuild;         /* Software build ID */
        u32 hwBuild;         /* Hardware build ID */
        DecHwConfig hwConfig;   /* hardware supported configuration */
    } VP8DecBuild;

    /* Output structure for VP8DecNextPicture */
    typedef struct VP8DecPicture_
    {
        u32 codedWidth;      /* coded width of the picture */
        u32 codedHeight;     /* coded height of the picture */
        u32 frameWidth;      /* pixels width of the frame as stored in memory */
        u32 frameHeight;     /* pixel height of the frame as stored in memory */
        const u32 *pOutputFrame;    /* Pointer to the frame */
        u32 outputFrameBusAddress;  /* DMA bus address of the output frame buffer */
        const u32 *pOutputFrameC;   /* Pointer to chroma output */
        u32 outputFrameBusAddressC;
        u32 picId;           /* Identifier of the Frame to be displayed */
        u32 isIntraFrame;    /* Indicates if Frame is an Intra Frame */
        u32 isGoldenFrame;   /* Indicates if Frame is a Golden reference Frame */
        u32 nbrOfErrMBs;     /* Number of concealed MB's in the frame  */
        u32 numSliceRows;
        DecOutFrmFormat outputFormat;
    } VP8DecPicture;

    /* Decoder instance */
    typedef const void *VP8DecInst;

/*------------------------------------------------------------------------------
    Prototypes of Decoder API functions
------------------------------------------------------------------------------*/
    VP8DecApiVersion VP8DecGetAPIVersion(void);

    VP8DecBuild VP8DecGetBuild(void);

    VP8DecRet VP8DecInit(VP8DecInst * pDecInst, VP8DecFormat decFormat,
                         u32 useVideoFreezeConcealment,
                         u32 numFrameBuffers,
                         DecRefFrmFormat referenceFrameFormat, 
                         u32 mmuEnable);

    void VP8DecRelease(VP8DecInst decInst);

    VP8DecRet VP8DecDecode(VP8DecInst decInst,
                           const VP8DecInput * pInput, VP8DecOutput * pOutput);

    VP8DecRet VP8DecNextPicture(VP8DecInst decInst,
                                VP8DecPicture * pOutput, u32 endOfStream);

    VP8DecRet VP8DecGetInfo(VP8DecInst decInst, VP8DecInfo * pDecInfo);

    VP8DecRet VP8DecPeek(VP8DecInst decInst, VP8DecPicture * pOutput);

    VP8DecRet VP8DecSetLatency(VP8DecInst decInst, int latencyMS);
/*------------------------------------------------------------------------------
    Prototype of the API trace funtion. Traces all API entries and returns.
    This must be implemented by the application using the decoder API!
    Argument:
        string - trace message, a null terminated string
------------------------------------------------------------------------------*/
    void VP8DecTrace(const char *string);

#ifdef __cplusplus
}
#endif

#endif                       /* __VP8DECAPI_H__ */
