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
--  $RCSfile: avs_hdrs.h,v $
--  $Date: 2010/03/09 05:54:00 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

#ifndef AVSDECHDRS_H
#define AVSDECHDRS_H

#include "basetype.h"

typedef struct
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
    u32 profileId;
    u32 levelId;
    u32 progressiveSequence;
    u32 horizontalSize;
    u32 verticalSize;
    u32 chromaFormat;
    u32 aspectRatio;
    u32 frameRateCode;
    u32 bitRateValue;
    u32 lowDelay;
    u32 bbvBufferSize;

    /* sequence display extension header */
    u32 videoFormat;
    u32 sampleRange;
    u32 colorDescription;
    u32 colorPrimaries;
    u32 transferCharacteristics;
    u32 matrixCoefficients;
    u32 displayHorizontalSize;
    u32 displayVerticalSize;

    /* picture header */
    u32 picCodingType;
    u32 bbvDelay;
    DecTimeCode timeCode;
    u32 pictureDistance;
    u32 progressiveFrame;
    u32 pictureStructure;
    u32 advancedPredModeDisable;
    u32 topFieldFirst;
    u32 repeatFirstField;
    u32 fixedPictureQp;
    u32 pictureQp;
    u32 pictureReferenceFlag;
    u32 skipModeFlag;
    u32 loopFilterDisable;
    i32 alphaOffset;
    i32 betaOffset;

} DecHdrs;

#endif /* #ifndef AVSDECHDRS_H */
