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
--  Description : API for the 8190 AVS Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: avsdecapi.h,v $
--  $Date: 2010/12/13 13:04:00 $
--  $Revision: 1.12 $
--
------------------------------------------------------------------------------*/

#ifndef __AVSDECAPI_H__
#define __AVSDECAPI_H__

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
        AVSDEC_OK = 0,
        AVSDEC_STRM_PROCESSED = 1,
        AVSDEC_PIC_RDY = 2,
        AVSDEC_HDRS_RDY = 3,
        AVSDEC_HDRS_NOT_RDY = 4,
        AVSDEC_PIC_DECODED = 5,
        AVSDEC_NONREF_PIC_SKIPPED = 6,/* Skipped non-reference picture */

        AVSDEC_PARAM_ERROR = -1,
        AVSDEC_STRM_ERROR = -2,
        AVSDEC_NOT_INITIALIZED = -3,
        AVSDEC_MEMFAIL = -4,
        AVSDEC_INITFAIL = -5,
        AVSDEC_STREAM_NOT_SUPPORTED = -8,

        AVSDEC_HW_RESERVED = -254,
        AVSDEC_HW_TIMEOUT = -255,
        AVSDEC_HW_BUS_ERROR = -256,
        AVSDEC_SYSTEM_ERROR = -257,
        AVSDEC_DWL_ERROR = -258,
        AVSDEC_FORMAT_NOT_SUPPORTED = -1000
    } AvsDecRet;

    /* decoder output picture format */
    typedef enum
    {
        AVSDEC_SEMIPLANAR_YUV420 = 0x020001,
        AVSDEC_TILED_YUV420 = 0x020002
    } AvsDecOutFormat;

    /* DAR (Display aspect ratio) */
    typedef enum
    {
        AVSDEC_1_1 = 0x01,
        AVSDEC_4_3 = 0x02,
        AVSDEC_16_9 = 0x03,
        AVSDEC_2_21_1 = 0x04
    } AvsDecDARFormat;

    /* SAR (Sample aspect ratio) */
    /* TODO! */

    typedef struct
    {
        u32 *pVirtualAddress;
        u32 busAddress;
    } AvsDecLinearMem;

    /* Decoder instance */
    typedef void *AvsDecInst;

    /* Input structure */
    typedef struct
    {
        u8 *pStream;         /* Pointer to stream to be decoded              */
        u32 streamBusAddress;   /* DMA bus address of the input stream */
        u32 dataLen;         /* Number of bytes to be decoded                */
        u32 picId;
        u32 skipNonReference; /* Flag to enable decoder skip non-reference 
                               * frames to reduce processor load */
    } AvsDecInput;

    /* Time code */
    typedef struct
    {
        u32 hours;
        u32 minutes;
        u32 seconds;
        u32 pictures;
    } AvsDecTime;

    typedef struct
    {
        u8 *pStrmCurrPos;
        u32 strmCurrBusAddress; /* DMA bus address location where the decoding
                                 * ended */
        u32 dataLeft;
    } AvsDecOutput;

    /* stream info filled by AvsDecGetInfo */
    typedef struct
    {
        u32 frameWidth;
        u32 frameHeight;
        u32 codedWidth;
        u32 codedHeight;
        u32 profileId;
        u32 levelId;
        u32 displayAspectRatio;
        u32 videoFormat;
        u32 videoRange;
        u32 interlacedSequence;
        u32 multiBuffPpSize;
        AvsDecOutFormat outputFormat;
    } AvsDecInfo;

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
        AvsDecTime timeCode;
    } AvsDecPicture;

    /* Version information */
    typedef struct
    {
        u32 major;           /* API major version */
        u32 minor;           /* API minor version */

    } AvsDecApiVersion;

    typedef struct
    {
        u32 swBuild;         /* Software build ID */
        u32 hwBuild;         /* Hardware build ID */
        DecHwConfig hwConfig;   /* hardware supported configuration */
    } AvsDecBuild;

/*------------------------------------------------------------------------------
    Prototypes of Decoder API functions
------------------------------------------------------------------------------*/

    AvsDecApiVersion AvsDecGetAPIVersion(void);

    AvsDecBuild AvsDecGetBuild(void);

    AvsDecRet AvsDecInit(AvsDecInst * pDecInst,
                         u32 useVideoFreezeConcealment,
                         u32 numFrameBuffers,
                         DecRefFrmFormat referenceFrameFormat,
                         u32 mmuEnable);

    AvsDecRet AvsDecDecode(AvsDecInst decInst,
                               AvsDecInput * pInput,
                               AvsDecOutput * pOutput);

    AvsDecRet AvsDecGetInfo(AvsDecInst decInst, AvsDecInfo * pDecInfo);

    AvsDecRet AvsDecNextPicture(AvsDecInst decInst,
                                    AvsDecPicture * pPicture,
                                    u32 endOfStream);

    void AvsDecRelease(AvsDecInst decInst);

    AvsDecRet AvsDecPeek(AvsDecInst decInst, AvsDecPicture * pPicture);

    AvsDecRet AvsDecSetLatency(AvsDecInst decInst, int lantencyMS);
/*------------------------------------------------------------------------------
    Prototype of the API trace funtion. Traces all API entries and returns.
    This must be implemented by the application using the decoder API!
    Argument:
        string - trace message, a null terminated string
------------------------------------------------------------------------------*/
    void AvsDecTrace(const char *string);

#ifdef __cplusplus
}
#endif

#endif                       /* __AVSDECAPI_H__ */
