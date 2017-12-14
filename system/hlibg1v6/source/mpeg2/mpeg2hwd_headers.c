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
--  Abstract :  Header decoding
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2hwd_headers.c,v $
--  $Date: 2009/04/27 08:56:17 $
--  $Revision: 1.28 $
-
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_DecodeHdrs

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "mpeg2hwd_headers.h"
#include "mpeg2hwd_utils.h"
#include "mpeg2hwd_strm.h"
#include "mpeg2hwd_debug.h"
#include "mpeg2hwd_cfg.h"
#ifdef ASIC_TRACE_SUPPORT
#include "trace.h"
#endif
/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/
#define ZIGZAG		0
#define ALTERNATE	1

/* zigzag/alternate */
static const u8 scanOrder[2][64] = {
    {   /* zig-zag */
     0, 1, 8, 16, 9, 2, 3, 10,
     17, 24, 32, 25, 18, 11, 4, 5,
     12, 19, 26, 33, 40, 48, 41, 34,
     27, 20, 13, 6, 7, 14, 21, 28,
     35, 42, 49, 56, 57, 50, 43, 36,
     29, 22, 15, 23, 30, 37, 44, 51,
     58, 59, 52, 45, 38, 31, 39, 46,
     53, 60, 61, 54, 47, 55, 62, 63},

    {   /* Alternate */
     0, 8, 16, 24, 1, 9, 2, 10,
     17, 25, 32, 40, 48, 56, 57, 49,
     41, 33, 26, 18, 3, 11, 4, 12,
     19, 27, 34, 42, 50, 58, 35, 43,
     51, 59, 20, 28, 5, 13, 6, 14,
     21, 29, 36, 44, 52, 60, 37, 45,
     53, 61, 22, 30, 7, 15, 23, 31,
     38, 46, 54, 62, 39, 47, 55, 63}
};

enum
{
    VIDEO_ID = 1
};
enum
{
    SIMPLE_OBJECT_TYPE = 1
};

/* aspect ratio info */
enum
{
    EXTENDED_PAR = 15
};
enum
{
    RECTANGULAR = 0
};

enum
{
    VERSION1 = 0,
    VERSION2 = 1
};

/* Default intra matrix */
static const u8 intraDefaultQMatrix[64] = {
    8, 16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83
};

#define DIMENSION_MASK 0x0FFF

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   5.1  Function name:
                mpeg2StrmDec_DecodeSequenceHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodeSequenceHeader(DecContainer * pDecContainer)
{
    u32 i;
    u32 tmp;
    DecHdrs *pHdr;

    ASSERT(pDecContainer);

    MPEG2DEC_DEBUG(("Decode Sequence Header Start\n"));

    pHdr = pDecContainer->StrmStorage.strmDecReady == FALSE ?
        &pDecContainer->Hdrs : &pDecContainer->tmpHdrs;

    /* read parameters from stream */
    tmp = pHdr->horizontalSize = mpeg2StrmDec_GetBits(pDecContainer, 12);
    if(!tmp)
        return (HANTRO_NOK);
    tmp = pHdr->verticalSize = mpeg2StrmDec_GetBits(pDecContainer, 12);
    if(!tmp)
        return (HANTRO_NOK);
    tmp = pHdr->aspectRatioInfo = mpeg2StrmDec_GetBits(pDecContainer, 4);
    tmp = pHdr->frameRateCode = mpeg2StrmDec_GetBits(pDecContainer, 4);
    tmp = pHdr->bitRateValue = mpeg2StrmDec_GetBits(pDecContainer, 18);

    /* marker bit ==> flush */
    tmp = mpeg2StrmDec_FlushBits(pDecContainer, 1);

    tmp = pHdr->vbvBufferSize = mpeg2StrmDec_GetBits(pDecContainer, 10);

    tmp = pHdr->constrParameters = mpeg2StrmDec_GetBits(pDecContainer, 1);

    /* check if need to load an q-matrix */
    tmp = pHdr->loadIntraMatrix = mpeg2StrmDec_GetBits(pDecContainer, 1);

    if(pHdr->loadIntraMatrix == 1)
    {
        /* load intra matrix */
        for(i = 0; i < 64; i++)
        {
            tmp = pHdr->qTableIntra[scanOrder[ZIGZAG][i]] =
                mpeg2StrmDec_GetBits(pDecContainer, 8);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
        }
    }
    else
    {
        /* use default intra matrix */
        for(i = 0; i < 64; i++)
        {
            pHdr->qTableIntra[i] = intraDefaultQMatrix[i];
        }
    }

    tmp = pHdr->loadNonIntraMatrix = mpeg2StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    if(pHdr->loadNonIntraMatrix)
    {
        /* load non-intra matrix */
        for(i = 0; i < 64; i++)
        {
            tmp = pHdr->qTableNonIntra[scanOrder[ZIGZAG][i]] =
                mpeg2StrmDec_GetBits(pDecContainer, 8);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
        }

    }
    else
    {
        /* use default non-intra matrix */
        for(i = 0; i < 64; i++)
        {
            pHdr->qTableNonIntra[i] = 16;
        }
    }

    /* headers already successfully decoded -> only use new quantization
     * tables */
    if(pDecContainer->StrmStorage.strmDecReady)
    {
        for(i = 0; i < 64; i++)
        {
            pDecContainer->Hdrs.qTableIntra[i] =
                pDecContainer->tmpHdrs.qTableIntra[i];
            pDecContainer->Hdrs.qTableNonIntra[i] =
                pDecContainer->tmpHdrs.qTableNonIntra[i];
        }
        if( pHdr->horizontalSize != pDecContainer->Hdrs.horizontalSize ||
            pHdr->verticalSize != pDecContainer->Hdrs.verticalSize )
        {
            pDecContainer->ApiStorage.firstHeaders = 1;
            pDecContainer->StrmStorage.strmDecReady = HANTRO_FALSE;
            /* If there have been B frames yet, we need to do extra round 
             * in the API to get the last frame out */
            if( !pDecContainer->Hdrs.lowDelay )
            {
                pDecContainer->StrmStorage.newHeadersChangeResolution = 1;
                /* Resolution change delayed */
            }
            else
            {
                pDecContainer->Hdrs.horizontalSize = pHdr->horizontalSize;
                pDecContainer->Hdrs.verticalSize = pHdr->verticalSize;
                /* Set rest of parameters just in case */
                pDecContainer->Hdrs.aspectRatioInfo = pHdr->aspectRatioInfo;
                pDecContainer->Hdrs.frameRateCode = pHdr->frameRateCode;
                pDecContainer->Hdrs.bitRateValue = pHdr->bitRateValue;
                pDecContainer->Hdrs.vbvBufferSize = pHdr->vbvBufferSize;
                pDecContainer->Hdrs.constrParameters = pHdr->constrParameters;
                pDecContainer->Hdrs.lowDelay = pHdr->lowDelay;
            }
        }
    }

    pDecContainer->FrameDesc.frameWidth =
        (pDecContainer->Hdrs.horizontalSize + 15) >> 4;
    pDecContainer->FrameDesc.frameHeight =
        (pDecContainer->Hdrs.verticalSize + 15) >> 4;
    pDecContainer->FrameDesc.totalMbInFrame =
        (pDecContainer->FrameDesc.frameWidth *
         pDecContainer->FrameDesc.frameHeight);

    MPEG2DEC_DEBUG(("Decode Sequence Header Done\n"));

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.2  Function name:
                mpeg2StrmDec_DecodeGOPHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodeGOPHeader(DecContainer * pDecContainer)
{
    u32 tmp;

    ASSERT(pDecContainer);

    MPEG2DEC_DEBUG(("Decode GOP (Group of Pictures) Header Start\n"));

    /* read parameters from stream */
    tmp = pDecContainer->Hdrs.time.dropFlag =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->FrameDesc.timeCodeHours =
        mpeg2StrmDec_GetBits(pDecContainer, 5);
    tmp = pDecContainer->FrameDesc.timeCodeMinutes =
        mpeg2StrmDec_GetBits(pDecContainer, 6);

    /* marker bit ==> flush */
    tmp = mpeg2StrmDec_FlushBits(pDecContainer, 1);

    tmp = pDecContainer->FrameDesc.timeCodeSeconds =
        mpeg2StrmDec_GetBits(pDecContainer, 6);
    tmp = pDecContainer->FrameDesc.frameTimePictures =
        mpeg2StrmDec_GetBits(pDecContainer, 6);

    tmp = pDecContainer->Hdrs.closedGop =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.brokenLink =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    MPEG2DEC_DEBUG(("Decode GOP (Group of Pictures) Header Done\n"));

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name:
                mpeg2StrmDec_DecodePictureHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodePictureHeader(DecContainer * pDecContainer)
{
    u32 tmp;
    u32 extraInfoByteCount = 0;

    ASSERT(pDecContainer);

    MPEG2DEC_DEBUG(("Decode Picture Header Start\n"));

    /* read parameters from stream */
    tmp = pDecContainer->Hdrs.temporalReference =
        mpeg2StrmDec_GetBits(pDecContainer, 10);
    tmp = pDecContainer->Hdrs.pictureCodingType =
        pDecContainer->FrameDesc.picCodingType =
        mpeg2StrmDec_GetBits(pDecContainer, 3);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    if(tmp < IFRAME || tmp > DFRAME)
    {
        pDecContainer->FrameDesc.picCodingType = PFRAME;
        return (HANTRO_NOK);
    }

#ifdef _MPEG2_8170_UNIT_TEST
    if(pDecContainer->Hdrs.pictureCodingType == IFRAME)
        printf("PICTURE CODING TYPE: IFRAME\n");
    else if(pDecContainer->Hdrs.pictureCodingType == PFRAME)
        printf("PICTURE CODING TYPE: PFRAME\n");
    else if(pDecContainer->Hdrs.pictureCodingType == BFRAME)
        printf("PICTURE CODING TYPE: BFRAME\n");
    else
        printf("PICTURE CODING TYPE: DFRAME\n");
#endif

#if 0
#ifdef ASIC_TRACE_SUPPORT
    if(pDecContainer->Hdrs.pictureCodingType == IFRAME)
        trace_mpeg2DecodingTools.picCodingType.i_coded = 1;

    else if(pDecContainer->Hdrs.pictureCodingType == PFRAME)
        trace_mpeg2DecodingTools.picCodingType.p_coded = 1;

    else if(pDecContainer->Hdrs.pictureCodingType == BFRAME)
        trace_mpeg2DecodingTools.picCodingType.b_coded = 1;

    else if(pDecContainer->Hdrs.pictureCodingType == DFRAME)
        trace_mpeg2DecodingTools.d_coded = 1;
#endif
#endif

    tmp = pDecContainer->Hdrs.vbvDelay =
        mpeg2StrmDec_GetBits(pDecContainer, 16);

    if(pDecContainer->Hdrs.pictureCodingType == PFRAME ||
       pDecContainer->Hdrs.pictureCodingType == BFRAME)
    {
        tmp = pDecContainer->Hdrs.fCode[0][0] =
            mpeg2StrmDec_GetBits(pDecContainer, 1);
        tmp = pDecContainer->Hdrs.fCode[0][1] =
            mpeg2StrmDec_GetBits(pDecContainer, 3);
        if(pDecContainer->Hdrs.mpeg2Stream == MPEG1 &&
           pDecContainer->Hdrs.fCode[0][1] == 0)
        {
            return (HANTRO_NOK);
        }
    }

    if(pDecContainer->Hdrs.pictureCodingType == BFRAME)
    {
        tmp = pDecContainer->Hdrs.fCode[1][0] =
            mpeg2StrmDec_GetBits(pDecContainer, 1);
        tmp = pDecContainer->Hdrs.fCode[1][1] =
            mpeg2StrmDec_GetBits(pDecContainer, 3);
        if(pDecContainer->Hdrs.mpeg2Stream == MPEG1 &&
           pDecContainer->Hdrs.fCode[1][1] == 0)
            return (HANTRO_NOK);
    }

    if(pDecContainer->Hdrs.mpeg2Stream == MPEG1)
    {
        /* forward */
        pDecContainer->Hdrs.fCodeFwdHor = pDecContainer->Hdrs.fCode[0][1];
        pDecContainer->Hdrs.fCodeFwdVer = pDecContainer->Hdrs.fCode[0][1];

        /* backward */
        pDecContainer->Hdrs.fCodeBwdHor = pDecContainer->Hdrs.fCode[1][1];
        pDecContainer->Hdrs.fCodeBwdVer = pDecContainer->Hdrs.fCode[1][1];
    }

    /* handle extra bit picture */
    while(mpeg2StrmDec_GetBits(pDecContainer, 1))
    {
        /* flush */
        tmp = mpeg2StrmDec_FlushBits(pDecContainer, 8);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        extraInfoByteCount++;
    }

    /* update extra info byte count */
    pDecContainer->Hdrs.extraInfoByteCount = extraInfoByteCount;

    MPEG2DEC_DEBUG(("Decode Picture Header Done\n"));

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.1  Function name:
                mpeg2StrmDec_DecodeExtensionHeader

        Purpose:
                Decodes MPEG-2 extension headers

        Input:
                pointer to DecContainer

        Output:
                status (OK/NOK/END_OF_STREAM/... enum in .h file!)

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodeExtensionHeader(DecContainer * pDecContainer)
{
    u32 extensionStartCode;
    u32 status = HANTRO_OK;

    /* get extension header ID */
    extensionStartCode = mpeg2StrmDec_GetBits(pDecContainer, 4);

    switch (extensionStartCode)
    {
    case SC_SEQ_EXT:
        /* sequence extension header, decoded only if
         * 1) decoder still in "initialization" phase
         * 2) sequence header was properly decoded */
        if(pDecContainer->StrmStorage.strmDecReady == FALSE &&
           pDecContainer->StrmStorage.validSequence)
        {
            status = mpeg2StrmDec_DecodeSeqExtHeader(pDecContainer);
            if(status != HANTRO_OK)
                pDecContainer->StrmStorage.validSequence = 0;
        }
        break;

    case SC_SEQ_DISPLAY_EXT:
        /* sequence display extension header */
        status = mpeg2StrmDec_DecodeSeqDisplayExtHeader(pDecContainer);
        break;

    case SC_QMATRIX_EXT:
        /* Q matrix extension header */
        status = mpeg2StrmDec_DecodeQMatrixExtHeader(pDecContainer);
        break;

    case SC_PIC_DISPLAY_EXT:
        /* picture display extension header */
        status = mpeg2StrmDec_DecodePicDisplayExtHeader(pDecContainer);
        break;

    case SC_PIC_CODING_EXT:
        /* picture coding extension header, decoded only if decoder is locked
         * to MPEG-2 mode */
        if(pDecContainer->StrmStorage.strmDecReady &&
           pDecContainer->Hdrs.mpeg2Stream)
        {
            status = mpeg2StrmDec_DecodePicCodingExtHeader(pDecContainer);
            if(status != HANTRO_OK)
                status = DEC_PIC_HDR_RDY_ERROR;
            /* wrong parity field, or wrong coding type for second field */
            else if(!pDecContainer->ApiStorage.ignoreField &&
                    !pDecContainer->ApiStorage.firstField &&
                    (((pDecContainer->ApiStorage.parity == 0 &&
                       pDecContainer->Hdrs.pictureStructure != 2) ||
                      (pDecContainer->ApiStorage.parity == 1 &&
                       pDecContainer->Hdrs.pictureStructure != 1)) ||
                     ((pDecContainer->Hdrs.pictureCodingType !=
                       pDecContainer->StrmStorage.prevPicCodingType) &&
                      (pDecContainer->Hdrs.pictureCodingType == BFRAME ||
                       pDecContainer->StrmStorage.prevPicCodingType ==
                       BFRAME))))
            {
                /* prev pic I/P and new B  ->.field) -> freeze previous and
                 * ignore curent field/frame */
                if(pDecContainer->Hdrs.pictureCodingType == BFRAME &&
                   pDecContainer->StrmStorage.prevPicCodingType != BFRAME)
                {
                    status = DEC_PIC_HDR_RDY_ERROR;
                    pDecContainer->Hdrs.pictureCodingType =
                        pDecContainer->StrmStorage.prevPicCodingType;
                    pDecContainer->Hdrs.pictureStructure =
                        pDecContainer->StrmStorage.prevPicStructure;
                }
                /* continue decoding, assume first field */
                else
                {
                    pDecContainer->StrmStorage.validPicExtHeader = 1;
                    pDecContainer->ApiStorage.parity =
                        pDecContainer->Hdrs.pictureStructure == 2;
                    pDecContainer->ApiStorage.firstField = 1;
                }
                /* set flag to release HW and wait PP if left running */
                pDecContainer->unpairedField = 1;
            }
            /* first field resulted in picture freeze -> ignore 2. field */
            else if(pDecContainer->ApiStorage.ignoreField &&
                    ((pDecContainer->ApiStorage.parity == 0 &&
                      pDecContainer->Hdrs.pictureStructure == 2) ||
                     (pDecContainer->ApiStorage.parity == 1 &&
                      pDecContainer->Hdrs.pictureStructure == 1)))
            {
                pDecContainer->ApiStorage.firstField = 1;
                pDecContainer->ApiStorage.parity = 0;
            }
            else
            {
                pDecContainer->StrmStorage.validPicExtHeader = 1;
                pDecContainer->ApiStorage.parity =
                    pDecContainer->Hdrs.pictureStructure == 2;
                /* non-matching field after error -> assume start of
                 * new picture */
                if(pDecContainer->ApiStorage.ignoreField)
                    pDecContainer->ApiStorage.firstField = 1;
            }
            pDecContainer->ApiStorage.ignoreField = 0;
            pDecContainer->StrmStorage.prevPicCodingType =
                pDecContainer->Hdrs.pictureCodingType;
            pDecContainer->StrmStorage.prevPicStructure =
                pDecContainer->Hdrs.pictureStructure;
        }
        break;

    default:
        break;
    }

    return (status);

}

/*------------------------------------------------------------------------------

   5.2  Function name:
                mpeg2StrmDec_DecodeSeqExtHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodeSeqExtHeader(DecContainer * pDecContainer)
{
    u32 tmp;

    ASSERT(pDecContainer);

    MPEG2DEC_DEBUG(("Decode Sequence Extension Header Start\n"));

#if 0
#ifdef ASIC_TRACE_SUPPORT
    trace_mpeg2DecodingTools.decodingMode = TRACE_MPEG2;
    /* the seuqnce type will be decided later and no MPEG-1 default is used */
    trace_mpeg2DecodingTools.sequenceType.interlaced = 0;
    trace_mpeg2DecodingTools.sequenceType.progressive = 0;
#endif
#endif

    /* read parameters from stream */
    tmp = pDecContainer->Hdrs.profileAndLevelIndication =
        mpeg2StrmDec_GetBits(pDecContainer, 8);
    tmp = pDecContainer->Hdrs.progressiveSequence =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.chromaFormat =
        mpeg2StrmDec_GetBits(pDecContainer, 2);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    if(tmp != 1)    /* 4:2:0 */
        return (HANTRO_NOK);
    tmp = pDecContainer->Hdrs.horSizeExtension =
        mpeg2StrmDec_GetBits(pDecContainer, 2);
    tmp = pDecContainer->Hdrs.verSizeExtension =
        mpeg2StrmDec_GetBits(pDecContainer, 2);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    /* update parameters */
    pDecContainer->Hdrs.horizontalSize =
        ((pDecContainer->Hdrs.horSizeExtension << 12) | (pDecContainer->Hdrs.
                                                         horizontalSize &
                                                         DIMENSION_MASK));

    pDecContainer->Hdrs.verticalSize =
        ((pDecContainer->Hdrs.verSizeExtension << 12) | (pDecContainer->Hdrs.
                                                         verticalSize &
                                                         DIMENSION_MASK));

    tmp = pDecContainer->Hdrs.bitRateExtension =
        mpeg2StrmDec_GetBits(pDecContainer, 12);

    /* marker bit ==> flush */
    tmp = mpeg2StrmDec_FlushBits(pDecContainer, 1);

    tmp = pDecContainer->Hdrs.vbvBufferSizeExtension =
        mpeg2StrmDec_GetBits(pDecContainer, 8);

    tmp = pDecContainer->Hdrs.lowDelay = mpeg2StrmDec_GetBits(pDecContainer, 1);

    tmp = pDecContainer->Hdrs.frameRateExtensionN =
        mpeg2StrmDec_GetBits(pDecContainer, 2);
    tmp = pDecContainer->Hdrs.frameRateExtensionD =
        mpeg2StrmDec_GetBits(pDecContainer, 5);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    /* update image dimensions */
    pDecContainer->FrameDesc.frameWidth =
        (pDecContainer->Hdrs.horizontalSize + 15) >> 4;
    if(pDecContainer->Hdrs.progressiveSequence)
        pDecContainer->FrameDesc.frameHeight =
            (pDecContainer->Hdrs.verticalSize + 15) >> 4;
    else
        pDecContainer->FrameDesc.frameHeight =
            2 * ((pDecContainer->Hdrs.verticalSize + 31) >> 5);
    pDecContainer->FrameDesc.totalMbInFrame =
        (pDecContainer->FrameDesc.frameWidth *
         pDecContainer->FrameDesc.frameHeight);

    MPEG2DEC_DEBUG(("Decode Sequence Extension Header Done\n"));

    /* Mark as a MPEG-2 stream */
    pDecContainer->Hdrs.mpeg2Stream = MPEG2;

    /* check if interlaced content */
    if(pDecContainer->Hdrs.progressiveSequence == 0)
        pDecContainer->Hdrs.interlaced = 1;
    else
        pDecContainer->Hdrs.interlaced = 0;

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name:
                mpeg2StrmDec_DecodeSeqDisplayExtHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodeSeqDisplayExtHeader(DecContainer * pDecContainer)
{
    u32 tmp;

    ASSERT(pDecContainer);

    MPEG2DEC_DEBUG(("Decode Sequence Display Extension Header Start\n"));

    tmp = pDecContainer->Hdrs.videoFormat =
        mpeg2StrmDec_GetBits(pDecContainer, 3);
    tmp = pDecContainer->Hdrs.colorDescription =
        mpeg2StrmDec_GetBits(pDecContainer, 1);

    if(pDecContainer->Hdrs.colorDescription)
    {
        tmp = pDecContainer->Hdrs.colorPrimaries =
            mpeg2StrmDec_GetBits(pDecContainer, 8);
        tmp = pDecContainer->Hdrs.transferCharacteristics =
            mpeg2StrmDec_GetBits(pDecContainer, 8);
        tmp = pDecContainer->Hdrs.matrixCoefficients =
            mpeg2StrmDec_GetBits(pDecContainer, 8);
    }

    tmp = pDecContainer->Hdrs.displayHorizontalSize =
        mpeg2StrmDec_GetBits(pDecContainer, 14);

    /* marker bit ==> flush */
    tmp = mpeg2StrmDec_FlushBits(pDecContainer, 1);

    tmp = pDecContainer->Hdrs.displayVerticalSize =
        mpeg2StrmDec_GetBits(pDecContainer, 14);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    MPEG2DEC_DEBUG(("Decode Sequence Display Extension Header Done\n"));

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name:
                mpeg2StrmDec_DecodeSeqQMatrixExtHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodeQMatrixExtHeader(DecContainer * pDecContainer)
{
    u32 i;
    u32 tmp;
    DecHdrs *pHdr;

    ASSERT(pDecContainer);

    MPEG2DEC_DEBUG(("Decode Sequence Quant Matrix Extension Header Start\n"));

    pHdr = &pDecContainer->tmpHdrs;

    /* check if need to load an q-matrix */
    tmp = pHdr->loadIntraMatrix = mpeg2StrmDec_GetBits(pDecContainer, 1);

    if(pHdr->loadIntraMatrix == 1)
    {
        /* load intra matrix */
        for(i = 0; i < 64; i++)
        {
            tmp = pHdr->qTableIntra[scanOrder[ZIGZAG][i]] =
                mpeg2StrmDec_GetBits(pDecContainer, 8);
        }
    }

    tmp = pHdr->loadNonIntraMatrix = mpeg2StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    if(pHdr->loadNonIntraMatrix)
    {
        /* load non-intra matrix */
        for(i = 0; i < 64; i++)
        {
            tmp = pHdr->qTableNonIntra[scanOrder[ZIGZAG][i]] =
                mpeg2StrmDec_GetBits(pDecContainer, 8);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
        }
    }

    /* successfully decoded -> overwrite previous tables */
    if(pHdr->loadIntraMatrix)
    {
        for(i = 0; i < 64; i++)
            pDecContainer->Hdrs.qTableIntra[i] = pHdr->qTableIntra[i];
    }
    if(pHdr->loadNonIntraMatrix)
    {
        for(i = 0; i < 64; i++)
            pDecContainer->Hdrs.qTableNonIntra[i] = pHdr->qTableNonIntra[i];
    }

    MPEG2DEC_DEBUG(("Decode Sequence Quant Matrix Extension Header Done\n"));

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name:
                mpeg2StrmDec_DecodePicCodingExtHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodePicCodingExtHeader(DecContainer * pDecContainer)
{
    u32 i, j;
    u32 tmp;

    ASSERT(pDecContainer);

    MPEG2DEC_DEBUG(("Decode Picture Coding Extension Header Start\n"));

    /* read parameters from stream */
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 2; j++)
        {
            tmp = pDecContainer->Hdrs.fCode[i][j] =
                mpeg2StrmDec_GetBits(pDecContainer, 4);
            if(tmp == 0 && pDecContainer->Hdrs.pictureCodingType != IFRAME)
                return (HANTRO_NOK);
        }
    }

    /* forward */
    pDecContainer->Hdrs.fCodeFwdHor = (pDecContainer->Hdrs.fCode[0][0]);
    pDecContainer->Hdrs.fCodeFwdVer = (pDecContainer->Hdrs.fCode[0][1]);

    /* backward */
    pDecContainer->Hdrs.fCodeBwdHor = (pDecContainer->Hdrs.fCode[1][0]);
    pDecContainer->Hdrs.fCodeBwdVer = (pDecContainer->Hdrs.fCode[1][1]);

    tmp = pDecContainer->Hdrs.intraDcPrecision =
        mpeg2StrmDec_GetBits(pDecContainer, 2);
    tmp = pDecContainer->Hdrs.pictureStructure =
        mpeg2StrmDec_GetBits(pDecContainer, 2);
    if (tmp == 0) /* reserved value -> assume equal to prev */
        pDecContainer->Hdrs.pictureStructure =
            pDecContainer->StrmStorage.prevPicStructure ?
            pDecContainer->StrmStorage.prevPicStructure : FRAMEPICTURE;
    tmp = pDecContainer->Hdrs.topFieldFirst =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.framePredFrameDct =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    if (pDecContainer->Hdrs.pictureStructure < FRAMEPICTURE)
        pDecContainer->Hdrs.framePredFrameDct = 0;
    tmp = pDecContainer->Hdrs.concealmentMotionVectors =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.quantType =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.intraVlcFormat =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.alternateScan =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.repeatFirstField =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.chroma420Type =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.progressiveFrame =
        mpeg2StrmDec_GetBits(pDecContainer, 1);

#ifdef _MPEG2_8170_UNIT_TEST
    if(pDecContainer->Hdrs.pictureStructure == TOPFIELD)
        printf("PICTURE STURCTURE: TOPFIELD\n");
    else if(pDecContainer->Hdrs.pictureStructure == BOTTOMFIELD)
        printf("PICTURE STURCTURE: BOTTOMFIELD\n");
    else
        printf("PICTURE STURCTURE: FRAMEPICTURE\n");
#endif

    /* check if interlaced content */
    if(pDecContainer->Hdrs.progressiveSequence == 0 /*&&
                                                     * pDecContainer->Hdrs.progressiveFrame == 0 */ )
        pDecContainer->Hdrs.interlaced = 1;
    else
        pDecContainer->Hdrs.interlaced = 0;

#if 0
#ifdef ASIC_TRACE_SUPPORT
    if(pDecContainer->Hdrs.progressiveSequence == 0 &&
       pDecContainer->Hdrs.progressiveFrame == 0)
    {
        trace_mpeg2DecodingTools.sequenceType.interlaced = 1;
    }

    else
    {
        trace_mpeg2DecodingTools.sequenceType.progressive = 1;
    }
#endif
#endif

    tmp = pDecContainer->Hdrs.compositeDisplayFlag =
        mpeg2StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    if(pDecContainer->Hdrs.compositeDisplayFlag)
    {
        tmp = pDecContainer->Hdrs.vAxis =
            mpeg2StrmDec_GetBits(pDecContainer, 1);
        tmp = pDecContainer->Hdrs.fieldSequence =
            mpeg2StrmDec_GetBits(pDecContainer, 3);
        tmp = pDecContainer->Hdrs.subCarrier =
            mpeg2StrmDec_GetBits(pDecContainer, 1);
        tmp = pDecContainer->Hdrs.burstAmplitude =
            mpeg2StrmDec_GetBits(pDecContainer, 7);
        tmp = pDecContainer->Hdrs.subCarrierPhase =
            mpeg2StrmDec_GetBits(pDecContainer, 8);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
    }

    MPEG2DEC_DEBUG(("Decode Picture Coding Extension Header Done\n"));

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name:
                mpeg2StrmDec_DecodePicDisplayExtHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 mpeg2StrmDec_DecodePicDisplayExtHeader(DecContainer * pDecContainer)
{
    u32 i;
    u32 tmp;
    u32 frameCenterOffsets = 0;

    ASSERT(pDecContainer);

    MPEG2DEC_DEBUG(("Decode Picture Display Extension Header Start\n"));

    /* calculate number of frameCenterOffset's */
    if(pDecContainer->Hdrs.progressiveSequence)
    {
        if(!pDecContainer->Hdrs.repeatFirstField)
        {
            frameCenterOffsets = 1;
        }
        else
        {
            if(pDecContainer->Hdrs.topFieldFirst)
            {
                frameCenterOffsets = 3;
            }
            else
            {
                frameCenterOffsets = 2;
            }
        }
    }
    else
    {
        if(pDecContainer->Hdrs.pictureStructure == FRAMEPICTURE)
        {
            if(pDecContainer->Hdrs.repeatFirstField)
            {
                frameCenterOffsets = 3;
            }
            else
            {
                frameCenterOffsets = 2;
            }
        }
        else
        {
            frameCenterOffsets = 1;
        }
    }

    /* todo */
    pDecContainer->Hdrs.repeatFrameCount = frameCenterOffsets;

    /* Read parameters from stream */
    for(i = 0; i < frameCenterOffsets; i++)
    {
        tmp = pDecContainer->Hdrs.frameCentreHorOffset[i] =
            mpeg2StrmDec_GetBits(pDecContainer, 16);
        /* marker bit ==> flush */
        tmp = mpeg2StrmDec_FlushBits(pDecContainer, 1);
        tmp = pDecContainer->Hdrs.frameCentreVerOffset[i] =
            mpeg2StrmDec_GetBits(pDecContainer, 16);
        /* marker bit ==> flush */
        tmp = mpeg2StrmDec_FlushBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
    }

    MPEG2DEC_DEBUG(("Decode Picture Display Extension Header DONE\n"));

    return (HANTRO_OK);
}
