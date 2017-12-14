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
--  $RCSfile: avs_headers.c,v $
--  $Date: 2010/03/09 05:54:00 $
--  $Revision: 1.8 $
-
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "avs_headers.h"
#include "avs_utils.h"
#include "avs_strm.h"
#include "avs_cfg.h"
#include "avs_vlc.h"
#ifdef ASIC_TRACE_SUPPORT
#include "trace.h"
#endif
/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   5.1  Function name:
                AvsStrmDec_DecodeSequenceHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 AvsStrmDec_DecodeSequenceHeader(DecContainer * pDecContainer)
{
    u32 i;
    u32 tmp;
    DecHdrs *pHdr;

    ASSERT(pDecContainer);

    AVSDEC_DEBUG(("Decode Sequence Header Start\n"));

    pHdr = pDecContainer->StrmStorage.strmDecReady == FALSE ?
        &pDecContainer->Hdrs : &pDecContainer->tmpHdrs;

    /* profile_id (needs to be checked, affects the fields that need to be
     * read from the stream) */
    tmp = AvsStrmDec_GetBits(pDecContainer, 8);
    if (tmp != 0x20)
    {
        AVSDEC_DEBUG(("UNSUPPORTED PROFILE\n"));
        return HANTRO_NOK;
    }
    pHdr->profileId = tmp;

    /* level_id */
    tmp = pHdr->levelId = AvsStrmDec_GetBits(pDecContainer, 8);

    tmp = pHdr->progressiveSequence = AvsStrmDec_GetBits(pDecContainer, 1);

    tmp = pHdr->horizontalSize = AvsStrmDec_GetBits(pDecContainer, 14);
    if(!tmp)
        return (HANTRO_NOK);

    tmp = pHdr->verticalSize = AvsStrmDec_GetBits(pDecContainer, 14);
    if(!tmp)
        return (HANTRO_NOK);

    tmp = pHdr->chromaFormat = AvsStrmDec_GetBits(pDecContainer, 2);
    if (pHdr->chromaFormat != 1) /* only 4:2:0 supported */
    {
        AVSDEC_DEBUG(("UNSUPPORTED CHROMA_FORMAT\n"));
        return (HANTRO_NOK);
    }

    /* sample_precision, shall be 8-bit */
    tmp = AvsStrmDec_GetBits(pDecContainer, 3);
    if (tmp != 1)
        return (HANTRO_NOK);

    tmp = pHdr->aspectRatio = AvsStrmDec_GetBits(pDecContainer, 4);
    tmp = pHdr->frameRateCode = AvsStrmDec_GetBits(pDecContainer, 4);

    /* bit_rate_lower */
    tmp = pHdr->bitRateValue = AvsStrmDec_GetBits(pDecContainer, 18);

    /* marker */
    tmp = AvsStrmDec_GetBits(pDecContainer, 1);

    /* bit_rate_upper */
    tmp = AvsStrmDec_GetBits(pDecContainer, 12);
    pHdr->bitRateValue |= tmp << 18;

    tmp = pDecContainer->Hdrs.lowDelay = AvsStrmDec_GetBits(pDecContainer, 1);

    /* marker */
    tmp = AvsStrmDec_GetBits(pDecContainer, 1);

    tmp = pHdr->bbvBufferSize = AvsStrmDec_GetBits(pDecContainer, 18);

    /* reserved_bits, shall be '000', not checked */
    tmp = AvsStrmDec_GetBits(pDecContainer, 3);

    if(pDecContainer->StrmStorage.strmDecReady)
    {
        if (pHdr->horizontalSize != pDecContainer->Hdrs.horizontalSize ||
            pHdr->verticalSize != pDecContainer->Hdrs.verticalSize)
        {
            pDecContainer->ApiStorage.firstHeaders = 1;
            pDecContainer->StrmStorage.strmDecReady = HANTRO_FALSE;
            /* delayed resolution change */
            if (!pDecContainer->StrmStorage.sequenceLowDelay)
            {
                pDecContainer->StrmStorage.newHeadersChangeResolution = 1;
            }
            else
            {
                pDecContainer->Hdrs.horizontalSize = pHdr->horizontalSize;
                pDecContainer->Hdrs.verticalSize = pHdr->verticalSize;
                pDecContainer->Hdrs.aspectRatio = pHdr->aspectRatio;
                pDecContainer->Hdrs.frameRateCode =
                    pHdr->frameRateCode;
                pDecContainer->Hdrs.bitRateValue =
                    pHdr->bitRateValue;
            }
        }

        if (pDecContainer->StrmStorage.sequenceLowDelay &&
            !pDecContainer->Hdrs.lowDelay)
            pDecContainer->StrmStorage.sequenceLowDelay = 0;

    }
    else
        pDecContainer->StrmStorage.sequenceLowDelay =
            pDecContainer->Hdrs.lowDelay;

    pDecContainer->StrmStorage.frameWidth =
        (pDecContainer->Hdrs.horizontalSize + 15) >> 4;
    if(pDecContainer->Hdrs.progressiveSequence)
        pDecContainer->StrmStorage.frameHeight =
            (pDecContainer->Hdrs.verticalSize + 15) >> 4;
    else
        pDecContainer->StrmStorage.frameHeight =
            2 * ((pDecContainer->Hdrs.verticalSize + 31) >> 5);
    pDecContainer->StrmStorage.totalMbsInFrame =
        (pDecContainer->StrmStorage.frameWidth *
         pDecContainer->StrmStorage.frameHeight);

    AVSDEC_DEBUG(("Decode Sequence Header Done\n"));

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.2  Function name:
                AvsStrmDec_DecodeIPictureHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 AvsStrmDec_DecodeIPictureHeader(DecContainer * pDecContainer)
{
    u32 tmp, val;
    DecHdrs *pHdr;

    ASSERT(pDecContainer);

    AVSDEC_DEBUG(("Decode Picture Header Start\n"));

    pHdr = &pDecContainer->Hdrs;

    pHdr->picCodingType = IFRAME;

    /* bbv_delay */
    tmp = AvsStrmDec_GetBits(pDecContainer, 16);

    /* time_code_flag */
    tmp = AvsStrmDec_GetBits(pDecContainer, 1);
    if (tmp)
    {
        /* time_code */
        tmp = AvsStrmDec_GetBits(pDecContainer, 1); /* DropFrameFlag */
        tmp = AvsStrmDec_GetBits(pDecContainer, 5); /* TimeCodeHours */
        pHdr->timeCode.hours = tmp;
        tmp = AvsStrmDec_GetBits(pDecContainer, 6); /* TimeCodeMinutes */
        pHdr->timeCode.minutes = tmp;
        tmp = AvsStrmDec_GetBits(pDecContainer, 6); /* TimeCodeSeconds */
        pHdr->timeCode.seconds = tmp;
        tmp = AvsStrmDec_GetBits(pDecContainer, 6); /* TimeCodePictures */
        pHdr->timeCode.picture = tmp;
    }

    tmp = AvsStrmDec_GetBits(pDecContainer, 1);

    tmp = pHdr->pictureDistance = AvsStrmDec_GetBits(pDecContainer, 8);

    if (pHdr->lowDelay)
        /* bbv_check_times */
        tmp = AvsDecodeExpGolombUnsigned(pDecContainer, &val);

    tmp = pHdr->progressiveFrame = AvsStrmDec_GetBits(pDecContainer, 1);

    if (!tmp)
    {
        tmp = pHdr->pictureStructure = AvsStrmDec_GetBits(pDecContainer, 1);
    }
    else pHdr->pictureStructure = FRAMEPICTURE;

    tmp = pHdr->topFieldFirst = AvsStrmDec_GetBits(pDecContainer, 1);
    tmp = pHdr->repeatFirstField = AvsStrmDec_GetBits(pDecContainer, 1);
    tmp = pHdr->fixedPictureQp = AvsStrmDec_GetBits(pDecContainer, 1);
    tmp = pHdr->pictureQp = AvsStrmDec_GetBits(pDecContainer, 6);

    if (pHdr->progressiveFrame == 0 && pHdr->pictureStructure == 0)
        tmp = pHdr->skipModeFlag = AvsStrmDec_GetBits(pDecContainer, 1);

    /* reserved_bits, shall be '0000', not checked */
    tmp = AvsStrmDec_GetBits(pDecContainer, 4);

    tmp = pHdr->loopFilterDisable = AvsStrmDec_GetBits(pDecContainer, 1);
    if (!tmp)
    {
        tmp = AvsStrmDec_GetBits(pDecContainer, 1);
        if (tmp)
        {
            tmp = AvsDecodeExpGolombSigned(pDecContainer, (i32*)&val);
            pHdr->alphaOffset = (i32)val;
            if (pHdr->alphaOffset < -8 || pHdr->alphaOffset > 8)
                return (HANTRO_NOK);
            tmp = AvsDecodeExpGolombSigned(pDecContainer, (i32*)&val);
            pHdr->betaOffset = (i32)val;
            if (pHdr->betaOffset < -8 || pHdr->betaOffset > 8)
                return (HANTRO_NOK);
        }
    }

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name:
                AvsStrmDec_DecodePBPictureHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 AvsStrmDec_DecodePBPictureHeader(DecContainer * pDecContainer)
{
    u32 tmp, val;
    DecHdrs *pHdr;

    ASSERT(pDecContainer);

    AVSDEC_DEBUG(("Decode Picture Header Start\n"));

    pHdr = &pDecContainer->Hdrs;

    /* bbv_delay */
    tmp = AvsStrmDec_GetBits(pDecContainer, 16);

    tmp = pHdr->picCodingType = AvsStrmDec_GetBits(pDecContainer, 2)+1;
    if (tmp != PFRAME && tmp != BFRAME)
        return (HANTRO_NOK);

    tmp = pHdr->pictureDistance = AvsStrmDec_GetBits(pDecContainer, 8);

    if (pHdr->lowDelay)
        /* bbv_check_times */
        tmp = AvsDecodeExpGolombUnsigned(pDecContainer, &val);

    tmp = pHdr->progressiveFrame = AvsStrmDec_GetBits(pDecContainer, 1);

    if (!tmp)
    {
        tmp = pHdr->pictureStructure = AvsStrmDec_GetBits(pDecContainer, 1);
        if (tmp == 0)
            tmp = pHdr->advancedPredModeDisable =
                AvsStrmDec_GetBits(pDecContainer, 1);
    }
    else pHdr->pictureStructure = FRAMEPICTURE;

    tmp = pHdr->topFieldFirst = AvsStrmDec_GetBits(pDecContainer, 1);
    tmp = pHdr->repeatFirstField = AvsStrmDec_GetBits(pDecContainer, 1);
    tmp = pHdr->fixedPictureQp = AvsStrmDec_GetBits(pDecContainer, 1);
    tmp = pHdr->pictureQp = AvsStrmDec_GetBits(pDecContainer, 6);

    if (!(pHdr->picCodingType == BFRAME && pHdr->pictureStructure == 1))
    {
        tmp = pHdr->pictureReferenceFlag = AvsStrmDec_GetBits(pDecContainer, 1);
    }

    /* reserved_bits, shall be '0000', not checked */
    tmp = AvsStrmDec_GetBits(pDecContainer, 4);

    tmp = pHdr->skipModeFlag = AvsStrmDec_GetBits(pDecContainer, 1);

    tmp = pHdr->loopFilterDisable = AvsStrmDec_GetBits(pDecContainer, 1);
    if (!tmp)
    {
        tmp = AvsStrmDec_GetBits(pDecContainer, 1);
        if (tmp)
        {
            tmp = AvsDecodeExpGolombSigned(pDecContainer, (i32*)&val);
            pHdr->alphaOffset = (i32)val;
            if (pHdr->alphaOffset < -8 || pHdr->alphaOffset > 8)
                return (HANTRO_NOK);
            tmp = AvsDecodeExpGolombSigned(pDecContainer, (i32*)&val);
            pHdr->betaOffset = (i32)val;
            if (pHdr->betaOffset < -8 || pHdr->betaOffset > 8)
                return (HANTRO_NOK);
        }
    }

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.1  Function name:
                AvsStrmDec_DecodeExtensionHeader

        Purpose:
                Decodes AVS extension headers

        Input:
                pointer to DecContainer

        Output:
                status (OK/NOK/END_OF_STREAM/... enum in .h file!)

------------------------------------------------------------------------------*/
u32 AvsStrmDec_DecodeExtensionHeader(DecContainer * pDecContainer)
{
    u32 extensionStartCode;
    u32 status = HANTRO_OK;

    /* get extension header ID */
    extensionStartCode = AvsStrmDec_GetBits(pDecContainer, 4);

    switch (extensionStartCode)
    {
        case SC_SEQ_DISPLAY_EXT:
            /* sequence display extension header */
            status = AvsStrmDec_DecodeSeqDisplayExtHeader(pDecContainer);
            break;

        default:
            break;
    }

    return (status);

}

/*------------------------------------------------------------------------------

   5.2  Function name:
                AvsStrmDec_DecodeSeqDisplayExtHeader

        Purpose:

        Input:

        Output:

------------------------------------------------------------------------------*/
u32 AvsStrmDec_DecodeSeqDisplayExtHeader(DecContainer * pDecContainer)
{
    u32 tmp;

    ASSERT(pDecContainer);

    AVSDEC_DEBUG(("Decode Sequence Display Extension Header Start\n"));

    tmp = pDecContainer->Hdrs.videoFormat =
        AvsStrmDec_GetBits(pDecContainer, 3);
    tmp = pDecContainer->Hdrs.sampleRange =
        AvsStrmDec_GetBits(pDecContainer, 1);
    tmp = pDecContainer->Hdrs.colorDescription =
        AvsStrmDec_GetBits(pDecContainer, 1);

    if(pDecContainer->Hdrs.colorDescription)
    {
        tmp = pDecContainer->Hdrs.colorPrimaries =
            AvsStrmDec_GetBits(pDecContainer, 8);
        tmp = pDecContainer->Hdrs.transferCharacteristics =
            AvsStrmDec_GetBits(pDecContainer, 8);
        tmp = pDecContainer->Hdrs.matrixCoefficients =
            AvsStrmDec_GetBits(pDecContainer, 8);
    }

    tmp = pDecContainer->Hdrs.displayHorizontalSize =
        AvsStrmDec_GetBits(pDecContainer, 14);

    /* marker bit ==> flush */
    tmp = AvsStrmDec_FlushBits(pDecContainer, 1);

    tmp = pDecContainer->Hdrs.displayVerticalSize =
        AvsStrmDec_GetBits(pDecContainer, 14);

    /* reserved_bits */
    tmp = AvsStrmDec_GetBits(pDecContainer, 2);

    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    AVSDEC_DEBUG(("Decode Sequence Display Extension Header Done\n"));

    return (HANTRO_OK);
}
