/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2007 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : API for the 8170 MPEG-2 Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2decapi.h,v $
--  $Date: 2010/12/13 13:04:01 $
--  $Revision: 1.21 $
--
------------------------------------------------------------------------------*/

#ifndef __MPEG2DECAPI_H__
#define __MPEG2DECAPI_H__

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
    typedef enum
    {
        MPEG2DEC_OK = 0,
        MPEG2DEC_STRM_PROCESSED = 1,
        MPEG2DEC_PIC_RDY = 2,
        MPEG2DEC_HDRS_RDY = 3,
        MPEG2DEC_HDRS_NOT_RDY = 4,
        MPEG2DEC_PIC_DECODED = 5,
        MPEG2DEC_NONREF_PIC_SKIPPED = 6,/* Skipped non-reference picture */


        MPEG2DEC_PARAM_ERROR = -1,
        MPEG2DEC_STRM_ERROR = -2,
        MPEG2DEC_NOT_INITIALIZED = -3,
        MPEG2DEC_MEMFAIL = -4,
        MPEG2DEC_INITFAIL = -5,
        MPEG2DEC_STREAM_NOT_SUPPORTED = -8,

        MPEG2DEC_HW_RESERVED = -254,
        MPEG2DEC_HW_TIMEOUT = -255,
        MPEG2DEC_HW_BUS_ERROR = -256,
        MPEG2DEC_SYSTEM_ERROR = -257,
        MPEG2DEC_DWL_ERROR = -258,
        MPEG2DEC_FORMAT_NOT_SUPPORTED = -1000
    } Mpeg2DecRet;

    /* decoder output picture format */
    typedef enum
    {
        MPEG2DEC_SEMIPLANAR_YUV420 = 0x020001,
        MPEG2DEC_TILED_YUV420 = 0x020002
    } Mpeg2DecOutFormat;

    /* DAR (Display aspect ratio) */
    typedef enum
    {
        MPEG2DEC_1_1 = 0x01,
        MPEG2DEC_4_3 = 0x02,
        MPEG2DEC_16_9 = 0x03,
        MPEG2DEC_2_21_1 = 0x04
    } Mpeg2DecDARFormat;

    typedef struct
    {
        u32 *pVirtualAddress;
        u32 busAddress;
    } Mpeg2DecLinearMem;

    /* Decoder instance */
    typedef void *Mpeg2DecInst;

    /* Input structure */
    typedef struct
    {
        u8 *pStream;         /* Pointer to stream to be decoded              */
        u32 streamBusAddress;   /* DMA bus address of the input stream */
        u32 dataLen;         /* Number of bytes to be decoded                */
        u32 picId;
        u32 skipNonReference; /* Flag to enable decoder skip non-reference 
                               * frames to reduce processor load */
    } Mpeg2DecInput;

    /* Time code */
    typedef struct
    {
        u32 hours;
        u32 minutes;
        u32 seconds;
        u32 pictures;
    } Mpeg2DecTime;

    typedef struct
    {
        u8 *pStrmCurrPos;
        u32 strmCurrBusAddress; /* DMA bus address location where the decoding
                                 * ended */
        u32 dataLeft;
    } Mpeg2DecOutput;

    /* stream info filled by Mpeg2DecGetInfo */
    typedef struct
    {
        u32 frameWidth;
        u32 frameHeight;
        u32 codedWidth;
        u32 codedHeight;
        u32 profileAndLevelIndication;
        u32 displayAspectRatio;
        u32 streamFormat;
        u32 videoFormat;
        u32 videoRange;      /* ??? only [0-255] */
        u32 interlacedSequence;
        u32 multiBuffPpSize;
        Mpeg2DecOutFormat outputFormat;
    } Mpeg2DecInfo;

    typedef struct
    {
        u8 *pOutputPicture;
        u32 outputPictureBusAddress;
        u32 frameWidth;
        u32 frameHeight;
        u32 codedWidth;
        u32 codedHeight;
        u32 keyPicture;
        u32 picId;
        u32 interlaced;
        u32 fieldPicture;
        u32 topField;
        u32 firstField;
        u32 repeatFirstField;
        u32 repeatFrameCount;
        u32 numberOfErrMBs;
        DecOutFrmFormat outputFormat;
        Mpeg2DecTime timeCode;
    } Mpeg2DecPicture;

    /* Version information */
    typedef struct
    {
        u32 major;           /* API major version */
        u32 minor;           /* API minor version */

    } Mpeg2DecApiVersion;

    typedef struct
    {
        u32 swBuild;         /* Software build ID */
        u32 hwBuild;         /* Hardware build ID */
        DecHwConfig hwConfig;   /* hardware supported configuration */
    } Mpeg2DecBuild;

/*------------------------------------------------------------------------------
    Prototypes of Decoder API functions
------------------------------------------------------------------------------*/

    Mpeg2DecApiVersion Mpeg2DecGetAPIVersion(void);

    Mpeg2DecBuild Mpeg2DecGetBuild(void);

    Mpeg2DecRet Mpeg2DecInit(Mpeg2DecInst * pDecInst,
                             u32 useVideoFreezeConcealment,
                             u32 numFrameBuffers,
                             DecRefFrmFormat referenceFrameFormat,
                             u32 mmuEnable);

    Mpeg2DecRet Mpeg2DecDecode(Mpeg2DecInst decInst,
                               Mpeg2DecInput * pInput,
                               Mpeg2DecOutput * pOutput);

    Mpeg2DecRet Mpeg2DecGetInfo(Mpeg2DecInst decInst, Mpeg2DecInfo * pDecInfo);

    Mpeg2DecRet Mpeg2DecNextPicture(Mpeg2DecInst decInst,
                                    Mpeg2DecPicture * pPicture,
                                    u32 endOfStream);

    void Mpeg2DecRelease(Mpeg2DecInst decInst);

    Mpeg2DecRet Mpeg2DecPeek(Mpeg2DecInst decInst, Mpeg2DecPicture * pPicture);

    Mpeg2DecRet Mpeg2DecSetLatency(Mpeg2DecInst decInst, int lantencyMS);
/*------------------------------------------------------------------------------
    Prototype of the API trace funtion. Traces all API entries and returns.
    This must be implemented by the application using the decoder API!
    Argument:
        string - trace message, a null terminated string
------------------------------------------------------------------------------*/
    void Mpeg2DecTrace(const char *string);

#ifdef __cplusplus
}
#endif

#endif                       /* __MPEG2DECAPI_H__ */
