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
--  Description : API internal functions
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2decapi_internal.c,v $
--  $Date: 2010/12/02 10:53:51 $
--  $Revision: 1.24 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of contents

    1.  Include headers
    5.  Fuctions
        5.1 ClearDataStructures()

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1.  Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "mpeg2hwd_container.h"
#include "mpeg2hwd_cfg.h"
#include "mpeg2decapi.h"
#include "mpeg2hwd_debug.h"
#include "mpeg2hwd_utils.h"
#include "mpeg2decapi_internal.h"
#include "regdrv.h"
#ifdef MPEG2_ASIC_TRACE
#include "mpeg2asicdbgtrace.h"
#endif

#include "bqueue.h"

/*------------------------------------------------------------------------------

    5.1 Function name:  mpeg2API_InitDataStructures()

        Purpose:        Initialize Data Structures in DecContainer.

        Input:          DecContainer *pDecCont

        Output:         u32

------------------------------------------------------------------------------*/
void mpeg2API_InitDataStructures(DecContainer * pDecCont)
{
/*
 *  have to be initialized into 1 to enable
 *  decoding stream without VO-headers
 */
    pDecCont->Hdrs.videoFormat = 5;
    pDecCont->Hdrs.transferCharacteristics = 1;
    pDecCont->Hdrs.matrixCoefficients = 1;
    pDecCont->Hdrs.progressiveSequence = 1;
    pDecCont->Hdrs.pictureStructure = 3;
    pDecCont->Hdrs.framePredFrameDct = 1;
    pDecCont->Hdrs.firstFieldInFrame = 0;
    pDecCont->Hdrs.fieldIndex = -1;
    pDecCont->Hdrs.fieldOutIndex = 1;
    pDecCont->Hdrs.videoRange = 0;
    pDecCont->StrmStorage.vpQP = 1;
    pDecCont->ApiStorage.firstHeaders = 1;
    pDecCont->StrmStorage.workOut = 0;
    pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.work1 =
        INVALID_ANCHOR_PICTURE;
}

/*------------------------------------------------------------------------------

    5.5 Function name:  mpeg2DecTimeCode();

        Purpose:        Write time data to output

        Input:          DecContainer *pDecCont, Mpeg2DecTime *timeCode

        Output:         void

------------------------------------------------------------------------------*/

void mpeg2DecTimeCode(DecContainer * pDecCont, Mpeg2DecTime * timeCode)
{

#define DEC_FRAMED pDecCont->FrameDesc
#define DEC_HDRS pDecCont->Hdrs
#define DEC_STST pDecCont->StrmStorage
#define DEC_SVDS pDecCont->SvDesc

    ASSERT(pDecCont);
    ASSERT(timeCode);

    timeCode->hours = DEC_FRAMED.timeCodeHours;
    timeCode->minutes = DEC_FRAMED.timeCodeMinutes;
    timeCode->seconds = DEC_FRAMED.timeCodeSeconds;
    timeCode->pictures = DEC_FRAMED.frameTimePictures;

#undef DEC_FRAMED
#undef DEC_HDRS
#undef DEC_STST
#undef DEC_SVDS

}

/*------------------------------------------------------------------------------

    x.x Function name:  mpeg2AllocateBuffers

        Purpose:        Allocate memory

        Input:          DecContainer *pDecCont

        Output:         MPEG2DEC_MEMFAIL/MPEG2DEC_OK

------------------------------------------------------------------------------*/
Mpeg2DecRet mpeg2AllocateBuffers(DecContainer * pDecCont)
{
#define DEC_FRAMED pDecCont->FrameDesc

    u32 i;
    i32 ret = 0;
    u32 sizeTmp = 0;
    u32 buffers = 0;

    ASSERT(DEC_FRAMED.totalMbInFrame != 0);

    /* allocate Q-table buffer */
    ret |= G1DWLMallocLinear(pDecCont->dwl, MPEG2DEC_TABLE_SIZE,
                           &pDecCont->ApiStorage.pQTableBase);
    if(ret)
        return (MPEG2DEC_MEMFAIL);

    MPEG2DEC_DEBUG(("Q-table: %x, %x\n",
                    (u32) pDecCont->ApiStorage.pQTableBase.virtualAddress,
                    pDecCont->ApiStorage.pQTableBase.busAddress));

    /* Reference images */
    if(!pDecCont->ApiStorage.externalBuffers)
    {
        sizeTmp =
            (MPEG2API_DEC_FRAME_BUFF_SIZE * DEC_FRAMED.totalMbInFrame * 4);

        /* Calculate minimum amount of buffers */
        buffers = pDecCont->Hdrs.lowDelay ? 2 : 3; 
        pDecCont->StrmStorage.parallelMode2 = 0;
        pDecCont->ppControl.prevAnchorDisplayIndex = BQUEUE_UNUSED;
        if( pDecCont->ppInstance ) /* Combined mode used */
        {
            /* If low-delay not set, then we might need extra buffers in
             * case we're using multibuffer mode. */
            if(!pDecCont->Hdrs.lowDelay)
            {
                pDecCont->ppConfigQuery.tiledMode =
                    pDecCont->tiledReferenceEnable;
                pDecCont->PPConfigQuery(pDecCont->ppInstance,
                                             &pDecCont->ppConfigQuery);
                if(pDecCont->ppConfigQuery.multiBuffer)
                    buffers = 4; 
            }

            pDecCont->StrmStorage.numPpBuffers = pDecCont->StrmStorage.maxNumBuffers;
            pDecCont->StrmStorage.numBuffers = buffers; /* Use bare minimum in decoder */
            /*buffers =  pDecCont->Hdrs.interlaced ? 1 : 2;*/
            buffers = 2;
            if( pDecCont->StrmStorage.numPpBuffers < buffers )
                pDecCont->StrmStorage.numPpBuffers = buffers;
        }
        else /* Dec only or separate PP */
        {
            pDecCont->StrmStorage.numBuffers = pDecCont->StrmStorage.maxNumBuffers;
            pDecCont->StrmStorage.numPpBuffers = 0;
            if( pDecCont->StrmStorage.numBuffers < buffers )
                pDecCont->StrmStorage.numBuffers = buffers;
        }

        ret = BqueueInit(&pDecCont->StrmStorage.bq, 
                         pDecCont->StrmStorage.numBuffers );
        if( ret != HANTRO_OK )
            return MPEG2DEC_MEMFAIL;

        ret = BqueueInit(&pDecCont->StrmStorage.bqPp, 
                         pDecCont->StrmStorage.numPpBuffers );
        if( ret != HANTRO_OK )
            return MPEG2DEC_MEMFAIL;

        for(i = 0; i < pDecCont->StrmStorage.numBuffers ; i++)
        {
            ret |= G1DWLMallocRefFrm(pDecCont->dwl, sizeTmp,
                                   &pDecCont->StrmStorage.pPicBuf[i].data);

            MPEG2DEC_DEBUG(("PicBuffer[%d]: %x, %x\n",
                            i,
                            (u32) pDecCont->StrmStorage.pPicBuf[i].data.
                            virtualAddress,
                            pDecCont->StrmStorage.pPicBuf[i].data.busAddress));

            if(pDecCont->StrmStorage.pPicBuf[i].data.busAddress == 0)
            {
                Mpeg2DecRelease(pDecCont);
                return (MPEG2DEC_MEMFAIL);
            }
        }
        /* initialize first picture buffer (workOut is 1 for the first picture)
         * grey, may be used as reference in certain error cases */
        (void) G1DWLmemset(pDecCont->StrmStorage.pPicBuf[1].data.virtualAddress,
                         128, 384 * pDecCont->FrameDesc.totalMbInFrame);
    }

    if(ret)
    {
        return (MPEG2DEC_MEMFAIL);
    }

/*
 *  pDecCont->MbSetDesc
 */

    return MPEG2DEC_OK;

#undef DEC_FRAMED
}

/*------------------------------------------------------------------------------

    x.x Function name:  mpeg2DecAllocExtraBPic

        Purpose:        Allocate memory

        Input:          DecContainer *pDecCont

        Output:         MPEG2DEC_MEMFAIL/MPEG2DEC_OK

------------------------------------------------------------------------------*/
Mpeg2DecRet mpeg2DecAllocExtraBPic(DecContainer * pDecCont)
{
    i32 ret = 0;
    u32 sizeTmp = 0;
    u32 extraBuffer = 0;

    /* If we already have enough buffers, do nothing. */
    if( pDecCont->StrmStorage.numBuffers >= 3)
        return MPEG2DEC_OK;

    pDecCont->StrmStorage.numBuffers = 3;
    /* We need one more buffer if using PP multibuffer */
    if(pDecCont->ppInstance != NULL)
    {
        pDecCont->ppConfigQuery.tiledMode =
            pDecCont->tiledReferenceEnable;
        pDecCont->PPConfigQuery(pDecCont->ppInstance,
                                     &pDecCont->ppConfigQuery);
        if(pDecCont->ppConfigQuery.multiBuffer)
        {
            pDecCont->StrmStorage.numBuffers = 4;
            extraBuffer = 1;
        }
    }   

    sizeTmp =
        (MPEG2API_DEC_FRAME_BUFF_SIZE * pDecCont->FrameDesc.totalMbInFrame * 4);

    BqueueRelease(&pDecCont->StrmStorage.bq);
    ret = BqueueInit(&pDecCont->StrmStorage.bq, 
                        pDecCont->StrmStorage.numBuffers );
    if(ret != HANTRO_OK)
        return (MPEG2DEC_MEMFAIL);

    ret = G1DWLMallocRefFrm(pDecCont->dwl, sizeTmp,
                          &pDecCont->StrmStorage.pPicBuf[2].data);
    if(pDecCont->StrmStorage.pPicBuf[2].data.busAddress == 0 || ret)
    {
        return (MPEG2DEC_MEMFAIL);
    }

    /* Allocate "extra" extra B buffer */
    if(extraBuffer)
    {
        ret = G1DWLMallocRefFrm(pDecCont->dwl, sizeTmp,
                              &pDecCont->StrmStorage.pPicBuf[3].data);
        if(pDecCont->StrmStorage.pPicBuf[3].data.busAddress == 0 || ret)
        {
            return (MPEG2DEC_MEMFAIL);
        }
    }

    return (MPEG2DEC_OK);
}

/*------------------------------------------------------------------------------

    x.x Function name:  mpeg2HandleQTables

        Purpose:        Copy Q-tables to HW buffer

        Input:          DecContainer *pDecCont

        Output:         

------------------------------------------------------------------------------*/
void mpeg2HandleQTables(DecContainer * pDecCont)
{
    u32 i = 0;
    u32 shifter = 32;
    u32 tableWord = 0;
    u32 *pTableBase = NULL;

    ASSERT(pDecCont->ApiStorage.pQTableBase.virtualAddress);
    ASSERT(pDecCont->ApiStorage.pQTableBase.busAddress);

    pTableBase = pDecCont->ApiStorage.pQTableBase.virtualAddress;

    /* QP tables for intra */
    for(i = 0; i < 64; i++)
    {
        shifter -= 8;
        if(shifter == 24)
            tableWord = (pDecCont->Hdrs.qTableIntra[i] << shifter);
        else
            tableWord |= (pDecCont->Hdrs.qTableIntra[i] << shifter);

        if(shifter == 0)
        {
            *(pTableBase) = tableWord;
            pTableBase++;
            shifter = 32;
        }
    }

    /* QP tables for non-intra */
    for(i = 0; i < 64; i++)
    {
        shifter -= 8;
        if(shifter == 24)
            tableWord = (pDecCont->Hdrs.qTableNonIntra[i] << shifter);
        else
            tableWord |= (pDecCont->Hdrs.qTableNonIntra[i] << shifter);

        if(shifter == 0)
        {
            *(pTableBase) = tableWord;
            pTableBase++;
            shifter = 32;
        }
    }
}

/*------------------------------------------------------------------------------

    x.x Function name:  mpeg2HandleMpeg1Parameters

        Purpose:        Set MPEG-1 parameters

        Input:          DecContainer *pDecCont

        Output:         

------------------------------------------------------------------------------*/

void mpeg2HandleMpeg1Parameters(DecContainer * pDecCont)
{
    ASSERT(!pDecCont->Hdrs.mpeg2Stream);

    pDecCont->Hdrs.progressiveSequence = 1;
    pDecCont->Hdrs.chromaFormat = 1;
    pDecCont->Hdrs.frameRateExtensionN = pDecCont->Hdrs.frameRateExtensionD = 0;
    pDecCont->Hdrs.intraDcPrecision = 0;
    pDecCont->Hdrs.pictureStructure = 3;
    pDecCont->Hdrs.framePredFrameDct = 1;
    pDecCont->Hdrs.concealmentMotionVectors = 0;
    pDecCont->Hdrs.intraVlcFormat = 0;
    pDecCont->Hdrs.alternateScan = 0;
    pDecCont->Hdrs.repeatFirstField = 0;
    pDecCont->Hdrs.chroma420Type = 1;
    pDecCont->Hdrs.progressiveFrame = 1;

}

 /*------------------------------------------------------------------------------

    x.x Function name:  mpeg2DecCheckSupport

        Purpose:        Check picture sizes etc

        Input:          DecContainer *pDecCont

        Output:         MPEG2DEC_STRMERROR/MPEG2DEC_OK

------------------------------------------------------------------------------*/

Mpeg2DecRet mpeg2DecCheckSupport(DecContainer * pDecCont)
{
#define DEC_FRAMED pDecCont->FrameDesc
    DWLHwConfig_t hwConfig;

    DWLReadAsicConfig(&hwConfig);

    if((DEC_FRAMED.frameHeight > (hwConfig.maxDecPicWidth >> 4)) ||
       (DEC_FRAMED.frameHeight < (MPEG2_MIN_HEIGHT >> 4)))
    {

        MPEG2DEC_DEBUG(("Mpeg2DecCheckSupport# Height not supported %d \n",
                        DEC_FRAMED.frameHeight));
        return MPEG2DEC_STREAM_NOT_SUPPORTED;
    }

    if((DEC_FRAMED.frameWidth > (hwConfig.maxDecPicWidth >> 4)) ||
       (DEC_FRAMED.frameWidth < (MPEG2_MIN_WIDTH >> 4)))
    {

        MPEG2DEC_DEBUG(("Mpeg2DecCheckSupport# Width not supported %d \n",
                        DEC_FRAMED.frameWidth));
        return MPEG2DEC_STREAM_NOT_SUPPORTED;
    }

    if(DEC_FRAMED.totalMbInFrame > MPEG2API_DEC_MBS)
    {
        MPEG2DEC_DEBUG(("Maximum number of macroblocks exceeded %d \n",
                        DEC_FRAMED.totalMbInFrame));
        return MPEG2DEC_STREAM_NOT_SUPPORTED;
    }

    return MPEG2DEC_OK;

#undef DEC_FRAMED
}

/*------------------------------------------------------------------------------

    x.x Function name:  mpeg2DecPreparePicReturn

        Purpose:        Prepare return values for PIC returns
                        For use after HW start

        Input:          DecContainer *pDecCont
                        Mpeg2DecOutput *outData    currently used out

        Output:         void

------------------------------------------------------------------------------*/

void mpeg2DecPreparePicReturn(DecContainer * pDecCont)
{

#define DEC_FRAMED pDecCont->FrameDesc
#define DEC_MBSD pDecCont->MbSetDesc
#define DEC_STRM pDecCont->StrmDesc
#define DEC_HDRS pDecCont->Hdrs
#define DEC_SVDS pDecCont->SvDesc
#define API_STOR pDecCont->ApiStorage

    Mpeg2DecOutput *pOut;

    ASSERT(pDecCont != NULL);

    pOut = &pDecCont->MbSetDesc.outData;

    if(pDecCont->Hdrs.interlaced)
    {
        if(pDecCont->Hdrs.pictureStructure != FRAMEPICTURE)
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].
                ff[pDecCont->Hdrs.fieldIndex] =
                pDecCont->Hdrs.firstFieldInFrame;
        }
        else
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].ff[0] =
                1;
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].ff[1] =
                0;
        }
    }
    else
    {
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].ff[0] = 0;
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].ff[1] = 0;
    }

    /* update index */
    if(pDecCont->Hdrs.fieldIndex == 1)
        pDecCont->Hdrs.fieldIndex = -1;

    if(pDecCont->Hdrs.firstFieldInFrame == 1)
        pDecCont->Hdrs.firstFieldInFrame = -1;

    return;

#undef DEC_FRAMED
#undef DEC_MBSD
#undef DEC_STRM
#undef DEC_HDRS
#undef DEC_SVDS
#undef API_STOR

}

 /*------------------------------------------------------------------------------

    x.x Function name:  mpeg2DecAspectRatio

        Purpose:        Set aspect ratio values for GetInfo

        Input:          DecContainer *pDecCont
                        Mpeg2DecInfo * pDecInfo    pointer to DecInfo

        Output:         void

------------------------------------------------------------------------------*/

void mpeg2DecAspectRatio(DecContainer * pDecCont, Mpeg2DecInfo * pDecInfo)
{

    MPEG2DEC_DEBUG(("SAR %d\n", pDecCont->Hdrs.aspectRatioInfo));

    /* If forbidden or reserved */
    if(pDecCont->Hdrs.aspectRatioInfo == 0 ||
       pDecCont->Hdrs.aspectRatioInfo > 4)
    {
        pDecInfo->displayAspectRatio = 0;
        return;
    }

    switch (pDecCont->Hdrs.aspectRatioInfo)
    {
    case 0x2:  /* 0010 4:3 */
        pDecInfo->displayAspectRatio = MPEG2DEC_4_3;
        break;

    case 0x3:  /* 0011 16:9 */
        pDecInfo->displayAspectRatio = MPEG2DEC_16_9;
        break;

    case 0x4:  /* 0100 2.21:1 */
        pDecInfo->displayAspectRatio = MPEG2DEC_2_21_1;
        break;

    default:   /* Square 0001 1/1 */
        pDecInfo->displayAspectRatio = MPEG2DEC_1_1;
        break;
    }

    /* TODO!  "DAR" */
    MPEG2DEC_DEBUG(("DAR %d\n", pDecCont->Hdrs.aspectRatioInfo));

    return;
}

/*------------------------------------------------------------------------------

    x.x Function name:  mpeg2DecBufferPicture

        Purpose:        Handles picture buffering

        Input:          
                        

        Output:         

------------------------------------------------------------------------------*/
void mpeg2DecBufferPicture(DecContainer * pDecCont, u32 picId, u32 bufferB,
                           u32 isInter, Mpeg2DecRet returnValue, u32 nbrErrMbs)
{
    i32 i, j;

    ASSERT(pDecCont);
    ASSERT(pDecCont->StrmStorage.outCount <= 
        pDecCont->StrmStorage.numBuffers - 1);

    if(bufferB == 0)    /* Buffer I or P picture */
    {
        i = pDecCont->StrmStorage.outIndex + pDecCont->StrmStorage.outCount;
        if(i >= 16)
            i -= 16;
    }
    else    /* Buffer B picture */
    {
        j = pDecCont->StrmStorage.outIndex + pDecCont->StrmStorage.outCount;
        i = j - 1;
        if(j >= 16) j -= 16;
        if(i >= 16) i -= 16; /* Added check due to max 2 pic latency */
        if(i < 0)
            i += 16;
        pDecCont->StrmStorage.outBuf[j] = pDecCont->StrmStorage.outBuf[i];
    }
    j = pDecCont->StrmStorage.workOut;

    pDecCont->StrmStorage.outBuf[i] = j;

    pDecCont->StrmStorage.pPicBuf[j].picId = picId;
    pDecCont->StrmStorage.pPicBuf[j].retVal = returnValue;
    pDecCont->StrmStorage.pPicBuf[j].isInter = isInter;
    pDecCont->StrmStorage.pPicBuf[j].picType = !isInter && !bufferB;
    pDecCont->StrmStorage.pPicBuf[j].tiledMode = pDecCont->tiledReferenceEnable;

    pDecCont->StrmStorage.pPicBuf[j].tf = pDecCont->Hdrs.topFieldFirst;
    pDecCont->StrmStorage.pPicBuf[j].rff = pDecCont->Hdrs.repeatFirstField;
    pDecCont->StrmStorage.pPicBuf[j].rfc = pDecCont->Hdrs.repeatFrameCount;

    if(pDecCont->Hdrs.pictureStructure != FRAMEPICTURE)
        pDecCont->StrmStorage.pPicBuf[j].nbrErrMbs = nbrErrMbs / 2;
    else
        pDecCont->StrmStorage.pPicBuf[j].nbrErrMbs = nbrErrMbs;

    if(pDecCont->ppInstance != NULL && returnValue == FREEZED_PIC_RDY)
        pDecCont->StrmStorage.pPicBuf[j].sendToPp = 2;

    mpeg2DecTimeCode(pDecCont, &pDecCont->StrmStorage.pPicBuf[j].timeCode);

    pDecCont->StrmStorage.outCount++;

}

/*------------------------------------------------------------------------------

    x.x Function name:  mpeg2FreeBuffers

        Purpose:        Allocate memory

        Input:          DecContainer *pDecCont

        Output:         MPEG2DEC_MEMFAIL/MPEG2DEC_OK

------------------------------------------------------------------------------*/
void mpeg2FreeBuffers(DecContainer * pDecCont)
{
    u32 i;

#define DEC_FRAMED pDecCont->FrameDesc

    BqueueRelease(&pDecCont->StrmStorage.bq);
    BqueueRelease(&pDecCont->StrmStorage.bqPp);

    if(pDecCont->ApiStorage.pQTableBase.virtualAddress != NULL)
    {
        DWLFreeLinear(pDecCont->dwl, &pDecCont->ApiStorage.pQTableBase);
        pDecCont->ApiStorage.pQTableBase.virtualAddress = NULL;
    }

    for(i = 0; i < pDecCont->StrmStorage.numBuffers ; i++)
    {
        if(pDecCont->StrmStorage.pPicBuf[i].data.virtualAddress != NULL)
        {
            DWLFreeRefFrm(pDecCont->dwl,
                          &pDecCont->StrmStorage.pPicBuf[i].data);
            pDecCont->StrmStorage.pPicBuf[i].data.virtualAddress = NULL;
            pDecCont->StrmStorage.pPicBuf[i].data.busAddress = 0;
        }
    }

#undef DEC_FRAMED
}

