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
--  Abstract : 
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2hwd_hdrs.h,v $
--  $Date: 2008/01/07 12:09:49 $
--  $Revision: 1.7 $
--
------------------------------------------------------------------------------*/

#ifndef MPEG2DECHDRS_H
#define MPEG2DECHDRS_H

#include "basetype.h"

typedef struct DecTimeCode_t
{
    u32 dropFlag;
    u32 hours;
    u32 minutes;
    u32 seconds;
    u32 picture;
} DecTimeCode;

typedef struct
{
    /* sequence header */
    u32 horizontalSize;
    u32 verticalSize;
    u32 aspectRatioInfo;
    u32 parWidth;
    u32 parHeight;
    u32 frameRateCode;
    u32 bitRateValue;
    u32 vbvBufferSize;
    u32 constrParameters;
    u32 loadIntraMatrix;
    u32 loadNonIntraMatrix;
    u8 qTableIntra[64];
    u8 qTableNonIntra[64];
    /* sequence extension header */
    u32 profileAndLevelIndication;
    u32 progressiveSequence;
    u32 chromaFormat;
    u32 horSizeExtension;
    u32 verSizeExtension;
    u32 bitRateExtension;
    u32 vbvBufferSizeExtension;
    u32 lowDelay;
    u32 frameRateExtensionN;
    u32 frameRateExtensionD;
    /* sequence display extension header */
    u32 videoFormat;
    u32 colorDescription;
    u32 colorPrimaries;
    u32 transferCharacteristics;
    u32 matrixCoefficients;
    u32 displayHorizontalSize;
    u32 displayVerticalSize;
    /* GOP (Group of Pictures) header */
    DecTimeCode time;
    u32 closedGop;
    u32 brokenLink;
    /* picture header */
    u32 temporalReference;
    u32 pictureCodingType;
    u32 vbvDelay;
    u32 extraInfoByteCount;
    /* picture coding extension header */
    u32 fCode[2][2];
    u32 intraDcPrecision;
    u32 pictureStructure;
    u32 topFieldFirst;
    u32 framePredFrameDct;
    u32 concealmentMotionVectors;
    u32 quantType;
    u32 intraVlcFormat;
    u32 alternateScan;
    u32 repeatFirstField;
    u32 chroma420Type;
    u32 progressiveFrame;
    u32 compositeDisplayFlag;
    u32 vAxis;
    u32 fieldSequence;
    u32 subCarrier;
    u32 burstAmplitude;
    u32 subCarrierPhase;
    /* picture display extension header */
    u32 frameCentreHorOffset[3];
    u32 frameCentreVerOffset[3];

    /* extra */
    u32 mpeg2Stream;
    u32 frameRate;
    u32 videoRange;
    u32 interlaced;
    u32 repeatFrameCount;
    i32 firstFieldInFrame;
    i32 fieldIndex;
    i32 fieldOutIndex;

    /* for motion vectors */
    u32 fCodeFwdHor;
    u32 fCodeFwdVer;
    u32 fCodeBwdHor;
    u32 fCodeBwdVer;

} DecHdrs;

#endif /* #ifndef MPEG2DECHDRS_H */
