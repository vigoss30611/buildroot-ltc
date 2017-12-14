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
--  Abstract : Short video (h263) decoding functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_shortvideo.c,v $
--  $Date: 2010/05/06 06:47:37 $
--  $Revision: 1.7 $
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_DecodeGobLayer
        5.2     StrmDec_DecodeShortVideo
        5.3     StrmDec_DecodeGobHeader

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mp4dechwd_shortvideo.h"
#include "mp4dechwd_utils.h"
#include "mp4dechwd_motiontexture.h"
#include "mp4debug.h"
#define INFOTM_SUPPORT_H263_TIME_EXTENSION_CODING
/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

u32 StrmDec_DecodeGobHeader(DecContainer * pDecContainer);
static u32 StrmDec_DecodeSVHPlusHeader(DecContainer * pDecContainer);

/*------------------------------------------------------------------------------

   5.1  Function name: StrmDec_DecodeGobLayer

        Purpose: decode Group Of Blocks (GOB) layer

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc
                -uses and updates SvDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeGobLayer(DecContainer * pDecContainer)
{
    u32 tmp;
    u32 mbNumber, mbCounter;
    u32 numMbs;
    u32 status = HANTRO_OK;

    /* gob header */
    if(pDecContainer->StrmStorage.gobResyncFlag == HANTRO_TRUE)
    {
        status = StrmDec_DecodeGobHeader(pDecContainer);
        if(status != HANTRO_OK)
            return (status);
        pDecContainer->StrmStorage.vpFirstCodedMb =
            pDecContainer->StrmStorage.vpMbNumber;
    }

    if(pDecContainer->SvDesc.numMbsInGob)
    {
        numMbs = pDecContainer->SvDesc.numMbsInGob;
    }
    else
    {
        numMbs = pDecContainer->VopDesc.vopWidth;
    }

    mbNumber = pDecContainer->StrmStorage.vpMbNumber;

    mbCounter = 0;
    while(1)
    {
        status = StrmDec_DecodeMb(pDecContainer, mbNumber);
        if(status != HANTRO_OK)
            return (status);
        if(!MB_IS_STUFFING(mbNumber))
        {
            mbNumber++;
            mbCounter++;
            if(mbCounter == numMbs)
            {
                tmp = 9 + (u32)(pDecContainer->VopDesc.vopCodingType == PVOP);
                while(StrmDec_ShowBits(pDecContainer, tmp) == 0x1)
                    (void) StrmDec_FlushBits(pDecContainer, tmp);
                break;
            }
        }
    }

    /* stuffing */
    if(pDecContainer->StrmDesc.bitPosInWord)
    {
        /* there is stuffing if next byte aligned bits are resync marker or
         * stream ends at next byte aligned position */
        if((StrmDec_ShowBitsAligned(pDecContainer, 17, 1) == SC_RESYNC) ||
           (((pDecContainer->StrmDesc.strmBuffReadBits >> 3) + 1) ==
            pDecContainer->StrmDesc.strmBuffSize))
        {
            tmp = StrmDec_GetBits(pDecContainer,
                                  8 - pDecContainer->StrmDesc.bitPosInWord);

            if(tmp != 0)
            {
                return (HANTRO_NOK);
            }
        }
    }

    /* last gob of vop -> check if short video end code. Read if yes and
     * check that there is stuffing and either end of stream or short video
     * start */
    if(mbNumber == pDecContainer->VopDesc.totalMbInVop)
    {
        tmp = StrmDec_ShowBits(pDecContainer, 22);
        if(tmp == SC_SV_END)
        {
            (void) StrmDec_FlushBits(pDecContainer, 22);
        }
        /* stuffing */
        if(pDecContainer->StrmDesc.bitPosInWord)
        {
            tmp = StrmDec_GetBits(pDecContainer,
                                  8 - pDecContainer->StrmDesc.bitPosInWord);
            if(tmp != 0)
            {
                return (HANTRO_NOK);
            }
        }

        /* there might be extra stuffing byte if next start code is video
         * object sequence start or end code. If this is the case the
         * stuffing is normal mpeg4 stuffing. */
        tmp = StrmDec_ShowBitsAligned(pDecContainer, 32, 1);
        if((tmp == SC_VOS_START) || (tmp == SC_VOS_END) ||
           (tmp == 0 && StrmDec_ShowBits(pDecContainer, 8) == 0x7F) )
        {
            tmp = StrmDec_GetStuffing(pDecContainer);
            if(tmp != HANTRO_OK)
                return (tmp);
        }

        /* JanSa: modified to handle extra zeros after VOP */
        tmp = StrmDec_ShowBits(pDecContainer,24);
        if ( !tmp )
        {
            do
            {
                if (StrmDec_FlushBits(pDecContainer, 8) == END_OF_STREAM)
                    break;
                tmp = StrmDec_ShowBits(pDecContainer,24);
            } while (!(tmp));
        }

        /* check that there is either end of stream or short video start or
         * at least 23 zeros in the stream */
        tmp = StrmDec_ShowBits(pDecContainer, 23);
        if(!IS_END_OF_STREAM(pDecContainer) && ((tmp >> 6) != SC_RESYNC) && tmp)
        {
            return (HANTRO_NOK);
        }
    }

    /* whole video packet decoded and stuffing ok (if stuffed) -> set
     * vpMbNumber in StrmStorage so that this gob layer won't
     * be touched/concealed anymore. Also set VpQP to QP so that concealment
     * will use qp of last decoded macro block. */
    pDecContainer->StrmStorage.vpMbNumber = mbNumber;
    pDecContainer->StrmStorage.vpQP = pDecContainer->StrmStorage.qP;

    pDecContainer->StrmStorage.vpNumMbs = 0;

    /* store pointer to rlc data buffer for concealment purposes */
    pDecContainer->MbSetDesc.pRlcDataVpAddr =
        pDecContainer->MbSetDesc.pRlcDataCurrAddr;

    return (status);

}

/*------------------------------------------------------------------------------

   5.2  Function name: StrmDec_DecodeShortVideo

        Purpose: decode VOP with short video header

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeShortVideo(DecContainer * pDecContainer)
{
    u32 status = HANTRO_OK;

    status = StrmDec_DecodeShortVideoHeader(pDecContainer);
    /* return if status not ok or stream decoder is not ready (first time here
     * and function just read the source format and left the stream as it
     * was) */
    if((status != HANTRO_OK) ||
       (pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE))
    {
        return (status);
    }

    status = StrmDec_DecodeGobLayer(pDecContainer);

    return (status);
}

/*------------------------------------------------------------------------------

   5.3  Function name: StrmDec_DecodeShortVideoHeader

        Purpose: decode short video header

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc
                -uses and updates SvDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeShortVideoHeader(DecContainer * pDecContainer)
{
    u32 tmp, status = 0;

    if(pDecContainer->StrmStorage.vpMbNumber)
    {
        (void) StrmDec_UnFlushBits(pDecContainer, 22);
        pDecContainer->StrmStorage.pLastSync =
            pDecContainer->StrmDesc.pStrmCurrPos;
        return (HANTRO_NOK);
    }
    pDecContainer->StrmStorage.validVopHeader = HANTRO_FALSE;

    pDecContainer->Hdrs.lowDelay = HANTRO_TRUE;
    /* initialize short video header stuff if first VOP */
    if(pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE)
    {
        pDecContainer->StrmStorage.resyncMarkerLength = 17;
        pDecContainer->StrmStorage.shortVideo = HANTRO_TRUE;

        pDecContainer->VopDesc.vopRoundingType = 0;
        pDecContainer->VopDesc.fcodeFwd = 1;
        pDecContainer->VopDesc.intraDcVlcThr = 0;
        pDecContainer->VopDesc.vopCoded = 1;

        pDecContainer->SvDesc.gobFrameId = 0;
        pDecContainer->SvDesc.temporalReference = 0;
        pDecContainer->SvDesc.tics = 0;

        pDecContainer->Hdrs.vopTimeIncrementResolution = 30000;
        pDecContainer->Hdrs.dataPartitioned = HANTRO_FALSE;
        pDecContainer->Hdrs.resyncMarkerDisable = HANTRO_FALSE;

        pDecContainer->Hdrs.colourPrimaries = 1;
        pDecContainer->Hdrs.transferCharacteristics = 1;
        pDecContainer->Hdrs.matrixCoefficients = 6;
    }

    if(!pDecContainer->SvDesc.sourceFormat)
    {
        /* vop size not known yet -> read and return.
         * source format is bits 14-16 from current position */
        tmp = StrmDec_ShowBits(pDecContainer, 16) & 0x7;

        switch (tmp)
        {
        case 1:    /* sub-QCIF */
            pDecContainer->VopDesc.vopWidth = 8;
            pDecContainer->VopDesc.vopHeight = 6;
            pDecContainer->VopDesc.totalMbInVop = 48;
            break;

        case 2:    /* QCIF */
            pDecContainer->VopDesc.vopWidth = 11;
            pDecContainer->VopDesc.vopHeight = 9;
            pDecContainer->VopDesc.totalMbInVop = 99;
            break;

        case 3:    /* CIF */
            pDecContainer->VopDesc.vopWidth = 22;
            pDecContainer->VopDesc.vopHeight = 18;
            pDecContainer->VopDesc.totalMbInVop = 396;
            break;

        case 4:    /* 4CIF */
            pDecContainer->VopDesc.vopWidth = 44;
            pDecContainer->VopDesc.vopHeight = 36;
            pDecContainer->VopDesc.totalMbInVop = 1584;
            break;

        case 7:    /* H.263 */

            /* go to start of plus header */
            status = StrmDec_GetBits(pDecContainer, 16);
            if(status == END_OF_STREAM)
                return (HANTRO_NOK);
            status = StrmDec_DecodeSVHPlusHeader(pDecContainer);
            if (status != HANTRO_OK || !pDecContainer->VopDesc.totalMbInVop)
            {
                pDecContainer->VopDesc.vopWidth = 0;
                pDecContainer->VopDesc.vopHeight = 0;
                pDecContainer->VopDesc.totalMbInVop = 0;
                pDecContainer->SvDesc.sourceFormat = 0;
                return(status);
            }
            (void) StrmDec_UnFlushBits(pDecContainer, 100);

            break;

        default:
            pDecContainer->SvDesc.sourceFormat = 0;
            return (HANTRO_NOK);
        }
        pDecContainer->SvDesc.sourceFormat = tmp;
        /* return start marker into stream */
        (void) StrmDec_UnFlushBits(pDecContainer, 22);
        pDecContainer->Hdrs.lastHeaderType = SC_SV_START;
        return (HANTRO_OK);
    }

    /* temporal reference. Note that arithmetics are performed only with
     * eight LSBs */
    tmp = StrmDec_GetBits(pDecContainer, 8);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    pDecContainer->SvDesc.temporalReference = tmp;

    pDecContainer->VopDesc.ticsFromPrev = (tmp + 256 -
                                               (pDecContainer->SvDesc.
                                                tics & 0xFF)) & 0xFF;
    pDecContainer->SvDesc.tics += pDecContainer->VopDesc.ticsFromPrev;

    tmp = StrmDec_GetBits(pDecContainer, 1);    /* marker */
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
    if(tmp == 0)
        return (HANTRO_NOK);
#endif /* HANTRO_PEDANTIC_MODE */
    tmp = StrmDec_GetBits(pDecContainer, 1);    /* zero */
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
    if(tmp)
        return (HANTRO_NOK);
#endif /* HANTRO_PEDANTIC_MODE */
    pDecContainer->SvDesc.splitScreenIndicator =
        StrmDec_GetBits(pDecContainer, 1);
    if(pDecContainer->SvDesc.splitScreenIndicator == END_OF_STREAM)
        return (END_OF_STREAM);
    pDecContainer->SvDesc.documentCameraIndicator =
        StrmDec_GetBits(pDecContainer, 1);
    if(pDecContainer->SvDesc.documentCameraIndicator == END_OF_STREAM)
        return (END_OF_STREAM);
    pDecContainer->SvDesc.fullPictureFreezeRelease =
        StrmDec_GetBits(pDecContainer, 1);
    if(pDecContainer->SvDesc.fullPictureFreezeRelease == END_OF_STREAM)
        return (END_OF_STREAM);

    tmp = StrmDec_GetBits(pDecContainer, 3);    /* source_format */
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    if(tmp != pDecContainer->SvDesc.sourceFormat)
    {
        return (HANTRO_NOK);
    }

    /* If source format is 7, plus header will be decoded which is h.263
     * baseline compatible. Otherwise stream is MPEG-4 shortvideo and it
     * will be decoded without plus header. */
    if(pDecContainer->SvDesc.sourceFormat != 7)
    {
        pDecContainer->VopDesc.vopCodingType =
            StrmDec_GetBits(pDecContainer, 1);
        if(pDecContainer->VopDesc.vopCodingType == END_OF_STREAM)
            return (END_OF_STREAM);
        tmp = StrmDec_GetBits(pDecContainer, 4);    /* 4 zero */
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
        if(tmp)
#else
        if(tmp && tmp != 8) /* Allow unrestricted mvs although 100% compatibility/
                             * conformity isn't necessarily achieved.. */
#endif /* HANTRO_PEDANTIC_MODE */
            return (HANTRO_NOK);

        pDecContainer->VopDesc.qP = StrmDec_GetBits(pDecContainer, 5);
        if(pDecContainer->VopDesc.qP == END_OF_STREAM)
            return (END_OF_STREAM);
        if(pDecContainer->VopDesc.qP == 0)
            return (HANTRO_NOK);

        pDecContainer->StrmStorage.qP = pDecContainer->VopDesc.qP;

        tmp = StrmDec_GetBits(pDecContainer, 1);    /* zero */
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

    }
    else
    {
        tmp = StrmDec_DecodeSVHPlusHeader(pDecContainer);
        if(tmp != HANTRO_OK)
            return (tmp);
        pDecContainer->StrmStorage.qP = pDecContainer->VopDesc.qP;
    }

    do
    {
        tmp = StrmDec_GetBits(pDecContainer, 1);    /* pei */
        if(tmp == 1)
        {
            (void) StrmDec_FlushBits(pDecContainer, 8); /* psupp */
        }
    }
    while(tmp == 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    /* no resync marker in the beginning of first gob */
    pDecContainer->StrmStorage.gobResyncFlag = HANTRO_FALSE;

    pDecContainer->StrmStorage.vpMbNumber = 0;
    pDecContainer->StrmStorage.vpFirstCodedMb = 0;

    /* successful decoding -> set valid vop header */
    pDecContainer->StrmStorage.validVopHeader = HANTRO_TRUE;


    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.4  Function name: StrmDec_CheckNextGobNumber

        Purpose:

        Input:
            Pointer to DecContainer structure
                -uses StrmDesc
                -uses SvDesc

        Output:
            GOB number
            0 if non-valid gob number found in stream

------------------------------------------------------------------------------*/

u32 StrmDec_CheckNextGobNumber(DecContainer * pDecContainer)
{
    u32 tmp;

    tmp = StrmDec_ShowBits(pDecContainer, 5);
    if(tmp >= pDecContainer->VopDesc.vopHeight)
    {
        tmp = 0;
    }
    return (tmp);

}

/*------------------------------------------------------------------------------

   5.3  Function name: StrmDec_DecodeGobHeader

        Purpose: decode Group Of Blocks (GOB) header

        Input:
            Pointer to DecContainer structure

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeGobHeader(DecContainer * pDecContainer)
{
    u32 tmp, tmp2;

    tmp = StrmDec_GetBits(pDecContainer, 5);    /* gob_number */
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    /* gob_number should be between (0,vopHeight) */
    if(!tmp || (tmp >= pDecContainer->VopDesc.vopHeight))
        return (HANTRO_NOK);

    /* store old gob frame id */
    tmp = pDecContainer->SvDesc.gobFrameId;

    tmp2 = StrmDec_GetBits(pDecContainer, 2);   /* gob frame id */
    if(tmp2 == END_OF_STREAM)
        return (END_OF_STREAM);

    pDecContainer->SvDesc.gobFrameId = tmp2;

    /* sv vop header lost -> check if gob_frame_id indicates that
     * old source_format etc. are invalid and get away if this is the
     * case */
    if((pDecContainer->StrmStorage.validVopHeader == HANTRO_FALSE) &&
       (pDecContainer->SvDesc.gobFrameId != tmp))
    {
        return (HANTRO_NOK);
    }

    /* QP */
    tmp = StrmDec_GetBits(pDecContainer, 5);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    if(tmp == 0)
        return (HANTRO_NOK);

    pDecContainer->StrmStorage.qP = tmp;
    pDecContainer->StrmStorage.prevQP = tmp;
    pDecContainer->StrmStorage.vpQP = pDecContainer->StrmStorage.qP;

    pDecContainer->StrmStorage.validVopHeader = HANTRO_TRUE;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.3  Function name: StrmDec_DecodeSVHPlusHeader

        Purpose: decode short video plus header

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc
                -uses and updates SvDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

static u32 StrmDec_DecodeSVHPlusHeader(DecContainer * pDecContainer)
{

    u32 tmp;
    u32 ufep = 0;
    u32 customWidth = 0, customHeight = 0;
#ifdef INFOTM_SUPPORT_H263_TIME_EXTENSION_CODING
	i32 custom_pcf =0;
#endif

    MP4DEC_DEBUG(("SVHPLUSHEADERDECODE\n"));

    ufep = StrmDec_GetBits(pDecContainer, 3);   /* update full extended ptype */

    if(END_OF_STREAM == ufep)
        return (END_OF_STREAM);
    /* if ufep == 1, opptype will be decoded */
    if(ufep == 1)
    {
        /* source format */
        tmp = StrmDec_GetBits(pDecContainer, 3);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp == 0 || tmp == 7)
            return (HANTRO_NOK);

        pDecContainer->SvDesc.sourceFormat = tmp;

        switch (pDecContainer->SvDesc.sourceFormat)
        {

        case 1:    /* sub-QCIF */

            pDecContainer->VopDesc.vopWidth = 8;
            pDecContainer->VopDesc.vopHeight = 6;
            pDecContainer->VopDesc.totalMbInVop = 48;
            break;

        case 2:    /* QCIF */
            pDecContainer->VopDesc.vopWidth = 11;
            pDecContainer->VopDesc.vopHeight = 9;
            pDecContainer->VopDesc.totalMbInVop = 99;
            break;

        case 3:    /* CIF */
            pDecContainer->VopDesc.vopWidth = 22;
            pDecContainer->VopDesc.vopHeight = 18;
            pDecContainer->VopDesc.totalMbInVop = 396;
            break;

        case 4:    /* 4CIF */
            pDecContainer->VopDesc.vopWidth = 44;
            pDecContainer->VopDesc.vopHeight = 36;
            pDecContainer->VopDesc.totalMbInVop = 1584;
            break;

        case 6:
            break;
        default:
            /* MPEG4 shortvideo type source format is still 7 */
            pDecContainer->SvDesc.sourceFormat = 7;
            return (HANTRO_NOK);
        }
        /* MPEG4 shortvideo type source format is still 7 */
        pDecContainer->SvDesc.sourceFormat = 7;

        /* custom picture clock freq. disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
#ifndef INFOTM_SUPPORT_H263_TIME_EXTENSION_CODING
        if(tmp)
            return (HANTRO_NOK);
#else
		custom_pcf = tmp;
#endif

        /* unrestricted motion vector mode disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Syntax based arithmetic coding disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Advanced prediction mode disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Advanced intra coding mode disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Deblocking filter mode disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Slice structured mode disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Reference picture selection disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Independent segment decoding mode disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Alternative inter vlc disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* Modified quantization mode disab/enab */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp)
            return (HANTRO_NOK);

        /* marker */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
        if(!tmp)
            return (HANTRO_NOK);
#endif /* HANTRO_PEDANTIC_MODE */

        /*reserved (zero) bits */
        tmp = StrmDec_GetBits(pDecContainer, 3);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
        if(tmp)
            return (HANTRO_NOK);
#endif /* HANTRO_PEDANTIC_MODE */
    }
    else if(ufep > 1)
    {
        return (HANTRO_NOK);
    }
    /* mpptype is decoded anyway regardless of value of ufep */
    /* picture coding type */
    tmp = StrmDec_GetBits(pDecContainer, 3);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    if(tmp > 1 || (!tmp && !ufep))
        return (HANTRO_NOK);

    pDecContainer->VopDesc.vopCodingType = tmp;

    /* Reference picture resampling disab/enab */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    if(tmp)
        return (HANTRO_NOK);

    /* Reduced-resolution update mode disab/enab */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    if(tmp)
        return (HANTRO_NOK);

    /* Rounding type */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    pDecContainer->VopDesc.vopRoundingType = tmp;

    /* reserved (zero) bits */
    tmp = StrmDec_GetBits(pDecContainer, 2);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
    if(tmp)
        return (HANTRO_NOK);
#endif /* HANTRO_PEDANTIC_MODE */

    /* marker */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
    if(!tmp)
        return (HANTRO_NOK);
#endif /* HANTRO_PEDANTIC_MODE */

    /* Plus header decoding continues after plustype */
    /* Picture header location of CPM */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
    if(tmp)
        return (HANTRO_NOK);
#endif /* HANTRO_PEDANTIC_MODE */

    if(ufep == 1)
    {
        /* Custom Picture Format */
        /* Pixel aspect ratio */
        tmp = StrmDec_GetBits(pDecContainer, 4);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp > 2)
            return (HANTRO_NOK);

        /* Picture width indication */
        tmp = StrmDec_GetBits(pDecContainer, 9);
        customWidth = (tmp + 1) * 4;

        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        pDecContainer->VopDesc.vopWidth = ((tmp + 1) * 4) / 16;

        /* marker */
        tmp = StrmDec_GetBits(pDecContainer, 1);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
        if(!tmp)
            return (HANTRO_NOK);
#endif /* HANTRO_PEDANTIC_MODE */

        /* Picture height indication */
        tmp = StrmDec_GetBits(pDecContainer, 9);
        customHeight = tmp * 4;

        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(!tmp)
            return (HANTRO_NOK);
        pDecContainer->VopDesc.vopHeight = (tmp * 4) / 16;
        pDecContainer->VopDesc.totalMbInVop =
            pDecContainer->VopDesc.vopWidth *
            pDecContainer->VopDesc.vopHeight;
    }
#ifdef INFOTM_SUPPORT_H263_TIME_EXTENSION_CODING
   if(custom_pcf){
   		//caculate time base / inc
		int timeden = 0;
		int timenum = 0;
		timeden = 1800000;
		timenum = 1000 + StrmDec_GetBits(pDecContainer, 1);
		timenum *= StrmDec_GetBits(pDecContainer, 7);
		if(timenum == 0)
			return HANTRO_NOK;

		//extended Temporal reference
		tmp = StrmDec_FlushBits(pDecContainer, 2);
   }
#endif

    /* QP */
    pDecContainer->VopDesc.qP = StrmDec_GetBits(pDecContainer, 5);
    MP4DEC_DEBUG(("pDecContainer->VopDesc.qP %d\n",
           pDecContainer->VopDesc.qP));
    if(pDecContainer->VopDesc.qP == END_OF_STREAM)
        return (END_OF_STREAM);
    if(pDecContainer->VopDesc.qP == 0)
        return (HANTRO_NOK);

    /* Number of GOBs in VOP */
    if(customHeight <= 400)
    {
        pDecContainer->SvDesc.numMbsInGob = customWidth / 16;
        if(customWidth & (15))
        {
            pDecContainer->SvDesc.numMbsInGob++;
        }

        pDecContainer->SvDesc.numGobsInVop = customHeight / 16;
        if(customHeight & (15))
        {
            pDecContainer->SvDesc.numGobsInVop++;
        }
    }
    else
    {
        pDecContainer->SvDesc.numMbsInGob = customWidth / 16;
        if(customWidth & (15))
        {
            pDecContainer->SvDesc.numMbsInGob++;
        }
        pDecContainer->SvDesc.numMbsInGob *= 2;

        pDecContainer->SvDesc.numGobsInVop = customHeight / 32;
        if(customHeight & (31))
        {
            pDecContainer->SvDesc.numGobsInVop++;
        }
    }
    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   Function name: StrmDec_DecodeSorensonSparkHeader

        Purpose:

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmDesc
                -uses and updates SvDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeSorensonSparkHeader(DecContainer * pDecContainer)
{
    u32 tmp = 0;
    u32 sourceFormat;

    pDecContainer->StrmStorage.validVopHeader = HANTRO_FALSE;

    /* initialize short video header stuff if first VOP */
    if(pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE)
    {
        pDecContainer->StrmStorage.resyncMarkerLength = 17;
        pDecContainer->StrmStorage.shortVideo = HANTRO_TRUE;

        pDecContainer->VopDesc.vopRoundingType = 0;
        pDecContainer->VopDesc.fcodeFwd = 1;
        pDecContainer->VopDesc.intraDcVlcThr = 0;
        pDecContainer->VopDesc.vopCoded = 1;

        pDecContainer->SvDesc.gobFrameId = 0;
        pDecContainer->SvDesc.temporalReference = 0;
        pDecContainer->SvDesc.tics = 0;

        pDecContainer->Hdrs.vopTimeIncrementResolution = 30000;
        pDecContainer->Hdrs.dataPartitioned = HANTRO_FALSE;
        pDecContainer->Hdrs.resyncMarkerDisable = HANTRO_FALSE;

        pDecContainer->Hdrs.colourPrimaries = 1;
        pDecContainer->Hdrs.transferCharacteristics = 1;
        pDecContainer->Hdrs.matrixCoefficients = 6;
    }

    if(!pDecContainer->StrmStorage.strmDecReady)
    {
        /* vop size not known yet -> read and return.
         * source format is bits 14-16 from current position */
        sourceFormat = StrmDec_ShowBits(pDecContainer, 12) & 0x7;

        switch (sourceFormat)
        {
            /* TODO: todellisen kuvan koko talteen!! */
            case 0:
                tmp = StrmDec_ShowBitsAligned(pDecContainer, 17, 2) & 0xFFFF;
                pDecContainer->VopDesc.vopWidth = ((tmp>>8)+15)/16;
                pDecContainer->Hdrs.videoObjectLayerWidth = (tmp>>8);
                pDecContainer->VopDesc.vopHeight = ((tmp&0xFF)+15)/16;
                pDecContainer->Hdrs.videoObjectLayerHeight = (tmp&0xFF);
                pDecContainer->VopDesc.totalMbInVop =
                    pDecContainer->VopDesc.vopWidth *
                    pDecContainer->VopDesc.vopHeight;
                break;

            case 1:
                tmp = StrmDec_ShowBitsAligned(pDecContainer, 17, 2) & 0xFFFF;
                pDecContainer->VopDesc.vopWidth = (tmp+15)/16;
                pDecContainer->Hdrs.videoObjectLayerWidth = tmp;
                tmp = StrmDec_ShowBitsAligned(pDecContainer, 17, 4) & 0xFFFF;
                pDecContainer->VopDesc.vopHeight = (tmp+15)/16;
                pDecContainer->Hdrs.videoObjectLayerHeight = tmp;
                pDecContainer->VopDesc.totalMbInVop =
                    pDecContainer->VopDesc.vopWidth *
                    pDecContainer->VopDesc.vopHeight;
                break;

            case 2:    /* CIF */
                pDecContainer->VopDesc.vopWidth = 22;
                pDecContainer->VopDesc.vopHeight = 18;
                pDecContainer->VopDesc.totalMbInVop = 396;
                break;

            case 3:    /* QCIF */
                pDecContainer->VopDesc.vopWidth = 11;
                pDecContainer->VopDesc.vopHeight = 9;
                pDecContainer->VopDesc.totalMbInVop = 99;
                break;

            case 4:    /* sub-QCIF */
                pDecContainer->VopDesc.vopWidth = 8;
                pDecContainer->VopDesc.vopHeight = 6;
                pDecContainer->VopDesc.totalMbInVop = 48;
                break;

            case 5:    /* QVGA */
                pDecContainer->VopDesc.vopWidth = 20;
                pDecContainer->VopDesc.vopHeight = 15;
                pDecContainer->VopDesc.totalMbInVop = 300;
                break;

            case 6:    /* QQVGA */
                pDecContainer->VopDesc.vopWidth = 10;
                pDecContainer->VopDesc.vopHeight = 8;
                pDecContainer->VopDesc.totalMbInVop = 80;
                break;

            default:    /* reserved */
                pDecContainer->SvDesc.sourceFormat = 0;
                return (HANTRO_NOK);
        }
        if (sourceFormat > 1)
        {
            pDecContainer->Hdrs.videoObjectLayerWidth = 
                16*pDecContainer->VopDesc.vopWidth;
            pDecContainer->Hdrs.videoObjectLayerHeight =
                16*pDecContainer->VopDesc.vopHeight;
        }
        pDecContainer->SvDesc.sourceFormat = sourceFormat;
        /* return start marker into stream */
        (void) StrmDec_UnFlushBits(pDecContainer, 22);
        pDecContainer->Hdrs.lastHeaderType = SC_SV_START;
        return (HANTRO_OK);
    }

    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    pDecContainer->StrmStorage.sorensonVer = tmp;

    /* temporal reference. Note that arithmetics are performed only with
     * eight LSBs */
    tmp = StrmDec_GetBits(pDecContainer, 8);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    pDecContainer->SvDesc.temporalReference = tmp;

    pDecContainer->VopDesc.ticsFromPrev = (tmp + 256 -
                                               (pDecContainer->SvDesc.
                                                tics & 0xFF)) & 0xFF;
    pDecContainer->SvDesc.tics += pDecContainer->VopDesc.ticsFromPrev;

    tmp = StrmDec_GetBits(pDecContainer, 3);    /* source_format */
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    if(tmp != pDecContainer->SvDesc.sourceFormat)
        return (HANTRO_NOK);

    if (pDecContainer->SvDesc.sourceFormat == 0 ||
        pDecContainer->SvDesc.sourceFormat == 1)
    {
        tmp = StrmDec_GetBits(pDecContainer,
            8*(pDecContainer->SvDesc.sourceFormat+1)); /* width */
        tmp = StrmDec_GetBits(pDecContainer,
            8*(pDecContainer->SvDesc.sourceFormat+1)); /* height */
    }

    tmp = StrmDec_GetBits(pDecContainer, 2);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    if(tmp > 2)
        return (HANTRO_NOK);
    if (tmp == 2) /* disposable */
    {
        pDecContainer->StrmStorage.disposable = 1;
        tmp = 1;
    }
    else
        pDecContainer->StrmStorage.disposable = 0;
    pDecContainer->VopDesc.vopCodingType = tmp;

    tmp = StrmDec_GetBits(pDecContainer, 1); /* deblocking filter */

    pDecContainer->VopDesc.qP = StrmDec_GetBits(pDecContainer, 5);
    if(pDecContainer->VopDesc.qP == END_OF_STREAM)
        return (END_OF_STREAM);
    if(pDecContainer->VopDesc.qP == 0)
        return (HANTRO_NOK);

    pDecContainer->StrmStorage.qP = pDecContainer->VopDesc.qP;

    do
    {
        tmp = StrmDec_GetBits(pDecContainer, 1);    /* pei */
        if(tmp == 1)
        {
            (void) StrmDec_FlushBits(pDecContainer, 8); /* psupp */
        }
    }
    while(tmp == 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    /* no resync marker in the beginning of first gob */
    pDecContainer->StrmStorage.gobResyncFlag = HANTRO_FALSE;

    pDecContainer->StrmStorage.vpMbNumber = 0;
    pDecContainer->StrmStorage.vpFirstCodedMb = 0;

    /* successful decoding -> set valid vop header */
    pDecContainer->StrmStorage.validVopHeader = HANTRO_TRUE;

    return (HANTRO_OK);

}

