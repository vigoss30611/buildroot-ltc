/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : API of VC-1 Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1decapi.h,v $
--  $Date: 2010/12/13 13:04:01 $
--  $Revision: 1.13 $
--
------------------------------------------------------------------------------*/

#ifndef VC1DECAPI_H
#define VC1DECAPI_H

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
    typedef enum VC1DecRet
    {
        VC1DEC_OK = 0,                /* Operation successful.*/
        VC1DEC_PIC_RDY = 1,           /* Picture decoded.*/
        VC1DEC_STRM_PROCESSED = 2,    /* Stream handled, no picture finished.*/
        VC1DEC_HDRS_RDY = 3,          /* Stream headers decoded */
        VC1DEC_END_OF_SEQ = 4,        /* End of video sequence */
        VC1DEC_PIC_DECODED = 5,       /* Picture decoded */
        VC1DEC_RESOLUTION_CHANGED = 6,/* Resolution changed in 
                                         multiresolution video */
        VC1DEC_NONREF_PIC_SKIPPED = 7,/* Skipped non-reference picture */

        VC1DEC_PARAM_ERROR = -1,      /* Function called with invalid
                                         parameters. */
        VC1DEC_NOT_INITIALIZED = -3,  /* Attempt to decode with
                                         uninitialized Decoder.*/
        VC1DEC_MEMFAIL = -4,          /* Memory failure. */
        VC1DEC_INITFAIL = -5,         /* Decoder initialization failure.
                                         Right shift is not signed. */
        VC1DEC_METADATA_FAIL = -6,    /* Supplied metadata is in wrong
                                         format */
        VC1DEC_STRM_ERROR = -7,       /* Stream error */
        VC1DEC_HW_RESERVED = -254,
        VC1DEC_HW_TIMEOUT = -255,
        VC1DEC_HW_BUS_ERROR = -256,
        VC1DEC_SYSTEM_ERROR = -257,
        VC1DEC_DWL_ERROR = -258,

		VC1DEC_FORMAT_NOT_SUPPORTED = -1000
    } VC1DecRet;

    /*
    * Container for the metadata of the stream.
    * Contains a wide range of information about the stream, e.g. what kind of
    * tools are needed to decode the stream.
    */
    typedef struct
    {
        u32     maxCodedWidth;  /**< Specifies the maximum coded width in
                                  *  pixels of picture within the sequence.
                                  *  Valid range [2,8192] (even values).*/
        u32     maxCodedHeight; /**< Specifies the maximum coded height in
                                  *  pixels of picture within the sequence.
                                  *  Valid range [2,8192] (even values).*/
        u32     vsTransform;    /**< Indicates whether variable sized transform
                                  *  is enabled for the sequence. Valid range [0,1].*/
        u32     overlap;        /**< Indicates whether overlap smoothing is
                                  *  enabled for the sequence. Valid range [0,1].*/
        u32     syncMarker;     /**< Indicates whether there are syncronization markers in
                                  *  the stream. Valid range [0,1].*/
        u32     quantizer;      /**< Indicates quantizer type used for the
                                  *  sequence. Valid range [0,3].*/
        u32     frameInterp;     /**< Indicates whether the INTERPFRM flag (which
                                  *  provides information to display process)
                                  *  exists in the picture headers. Valid range [0,1].*/
        u32     maxBframes;     /**< Specifies the maximum amount of consecutive
                                  *  B-frames within the sequence. Valid range [0,7].*/
        u32     fastUvMc;       /**< Indicates whether the rounding of color
                                  *  difference motion vectors is enabled. Valid range [0,1].*/
        u32     extendedMv;     /**< Indicates whether extended motion
                                  *  vectors are enabled for the sequence. Valid range [0,1].*/
        u32     multiRes;       /**< Indicates whether frames may be coded
                                  *  at smaller resolutions than
                                  *  the specified frame resolution. Valid range [0,1].*/
        u32     rangeRed;       /**< Indicates whether range reduction is used
                                  *  in the sequence. Valid range [0,1].*/
        u32     dquant;         /**< Indicates whether the quantization step
                                  *  may vary within a frame. Valid range [0,2].*/
        u32     loopFilter;     /**< Indicates whether loop filtering is
                                  *  enabled for the sequence. Valid range [0,1].*/
        u32     profile;        /**< Specifies profile of the input video bitstream. */
    }VC1DecMetaData;

    /*
    *   Decoder instance is used for identifying subsequent calls to api
    *   functions.
    */
    typedef const void *VC1DecInst;

    /*
    *  Decoder input structure.
    *  This is a container to pass data to the Decoder.
    */
    typedef struct
    {

        const u8* pStream;     /* Pointer to the video stream. Decoder does
                                  not change the contents of stream buffer.*/
        u32 streamBusAddress;  /* DMA bus address of the input stream */
        u32 streamSize;        /* Number of bytes in the stream buffer.*/
        u32 picId;             /**< User-defined identifier to bind into
                                 *  decoded picture. */
        u32 skipNonReference; /* Flag to enable decoder skip non-reference 
                               * frames to reduce processor load */
    } VC1DecInput;

    typedef struct
    {
        u32 dataLeft;
        u8 *pStreamCurrPos;
        u32 strmCurrBusAddress;
    } VC1DecOutput;

    /*
    *   Decoder output structure.
    *   This is a container for Decoder output data like decoded picture and its
    *   dimensions.
    */
    typedef struct
    {
        u32 frameWidth;
        u32 frameHeight;
        u32 codedWidth;
        u32 codedHeight;
        const u8 *pOutputPicture;
        u32 outputPictureBusAddress;
        u32 keyPicture;
        u32 picId;
        u32 rangeRedFrm;

        u32 rangeMapYFlag;
        u32 rangeMapY;
        u32 rangeMapUvFlag;
        u32 rangeMapUv;

        u32 interlaced;
        u32 fieldPicture;
        u32 topField;

        u32 firstField;
        u32 repeatFirstField;
        u32 repeatFrameCount;

        u32 numberOfErrMBs;
        u32 anchorPicture;
        DecOutFrmFormat outputFormat;
    } VC1DecPicture;

    /* Version information. */
    typedef struct
    {
        u32 major;    /* Decoder API major version number. */
        u32 minor;    /* Decoder API minor version number. */
    } VC1DecApiVersion; 

    typedef struct
    {
        u32 swBuild;            /* Software build ID */
        u32 hwBuild;            /* Hardware build ID */
        DecHwConfig hwConfig;   /* hardware supported configuration */
    } VC1DecBuild;

    /* decoder  output picture format */
    typedef enum
    {
        VC1DEC_SEMIPLANAR_YUV420 = 0x020001,
        VC1DEC_TILED_YUV420 = 0x020002
    } VC1DecOutFormat;

    typedef struct
    {
        VC1DecOutFormat outputFormat; /* format of the output picture */
        u32 maxCodedWidth;
        u32 maxCodedHeight;
        u32 codedWidth;
        u32 codedHeight;
        u32 parWidth;
        u32 parHeight;
        u32 frameRateNumerator;
        u32 frameRateDenominator;
        u32 interlacedSequence;
    	u32 multiBuffPpSize;
    } VC1DecInfo;

/*------------------------------------------------------------------------------
    Prototypes of Decoder API functions
------------------------------------------------------------------------------*/

    VC1DecApiVersion VC1DecGetAPIVersion(void);

    VC1DecBuild VC1DecGetBuild(void);

    VC1DecRet VC1DecInit( VC1DecInst* pDecInst, const VC1DecMetaData* pMetaData,
                          u32 useVideoFreezeConcealment,
                          u32 numFrameBuffers,
                          DecRefFrmFormat referenceFrameFormat,
                          u32 mmuEnable);

    VC1DecRet VC1DecDecode( VC1DecInst decInst,
                            const VC1DecInput* pInput,
                            VC1DecOutput* pOutput);

    void VC1DecRelease(VC1DecInst decInst);

    VC1DecRet VC1DecGetInfo(VC1DecInst decInst, VC1DecInfo * pDecInfo);

    VC1DecRet VC1DecUnpackMetaData( const u8 *pBuffer, u32 bufferSize,
                                    VC1DecMetaData *pMetaData );

    VC1DecRet VC1DecNextPicture(VC1DecInst  decInst,
                            VC1DecPicture *pPicture,
                            u32 endOfStream);

    VC1DecRet VC1DecPeek(VC1DecInst  decInst, VC1DecPicture *pPicture);

    VC1DecRet VC1DecSetLatency(VC1DecInst decInst, int lantencyMS);

/*------------------------------------------------------------------------------
    Prototype of the API trace funtion. Traces all API entries and returns.
    This must be implemented by the application using the decoder API!
    Argument:
        string - trace message, a null terminated string
------------------------------------------------------------------------------*/
    void VC1DecTrace(const char *string);

#ifdef __cplusplus
}
#endif

#endif /* VC1DECAPI_H */

