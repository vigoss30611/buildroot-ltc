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
--  $RCSfile: avsdecapi_internal.c,v $
--  $Date: 2010/12/01 12:30:58 $
--  $Revision: 1.11 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1.  Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "avs_container.h"
#include "avs_cfg.h"
#include "avsdecapi.h"
#include "avs_utils.h"
#include "avsdecapi_internal.h"
#include "regdrv.h"

/*------------------------------------------------------------------------------

    5.1 Function name:  AvsAPI_InitDataStructures()

        Purpose:        Initialize Data Structures in DecContainer.

        Input:          DecContainer *pDecCont

        Output:         u32

------------------------------------------------------------------------------*/
void AvsAPI_InitDataStructures(DecContainer * pDecCont)
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
    pDecCont->StrmStorage.fieldOutIndex = 1;
    pDecCont->Hdrs.sampleRange = 0;
    pDecCont->ApiStorage.firstHeaders = 1;
    pDecCont->StrmStorage.workOut = BqueueNext( 
            &pDecCont->StrmStorage.bq,
            BQUEUE_UNUSED, BQUEUE_UNUSED, 
            BQUEUE_UNUSED, 0 );
    pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.work1 =
        INVALID_ANCHOR_PICTURE;
}

/*------------------------------------------------------------------------------

    5.5 Function name:  AvsDecTimeCode();

        Purpose:        Write time data to output

        Input:          DecContainer *pDecCont, AvsDecTime *timeCode

        Output:         void

------------------------------------------------------------------------------*/

void AvsDecTimeCode(DecContainer * pDecCont, AvsDecTime * timeCode)
{


    ASSERT(pDecCont);
    ASSERT(timeCode);

    timeCode->hours = pDecCont->Hdrs.timeCode.hours;
    timeCode->minutes = pDecCont->Hdrs.timeCode.minutes;
    timeCode->seconds = pDecCont->Hdrs.timeCode.seconds;
    timeCode->pictures = pDecCont->Hdrs.timeCode.picture;

}

/*------------------------------------------------------------------------------

    x.x Function name:  AvsAllocateBuffers

        Purpose:        Allocate memory

        Input:          DecContainer *pDecCont

        Output:         AVSDEC_MEMFAIL/AVSDEC_OK

------------------------------------------------------------------------------*/
AvsDecRet AvsAllocateBuffers(DecContainer * pDecCont)
{

    u32 i;
    i32 ret = 0;
    u32 sizeTmp = 0;
    u32 buffers = 0;

    ASSERT(pDecCont->StrmStorage.totalMbsInFrame != 0);

    /* Reference images */
    if(!pDecCont->ApiStorage.externalBuffers)
    {
        sizeTmp =
            (AVSAPI_DEC_FRAME_BUFF_SIZE * pDecCont->StrmStorage.totalMbsInFrame * 4);

        /* Calculate minimum amount of buffers */
        buffers = 3; 

        if( pDecCont->StrmStorage.numBuffers < buffers )
        {
            pDecCont->StrmStorage.numBuffers = buffers;
        }

        if( pDecCont->ppInstance ) /* Combined mode used */
        {
            pDecCont->StrmStorage.numPpBuffers = pDecCont->StrmStorage.maxNumBuffers;
            pDecCont->StrmStorage.numBuffers = buffers; /* Use bare minimum in decoder */
            buffers =  pDecCont->Hdrs.progressiveSequence ? 2 : 1;
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
            return AVSDEC_MEMFAIL;

        ret = BqueueInit(&pDecCont->StrmStorage.bqPp, 
                         pDecCont->StrmStorage.numPpBuffers );
        if( ret != HANTRO_OK )
            return AVSDEC_MEMFAIL;

        for(i = 0; i < pDecCont->StrmStorage.numBuffers; i++)
        {
            ret |= G1DWLMallocRefFrm(pDecCont->dwl, sizeTmp,
                                   &pDecCont->StrmStorage.pPicBuf[i].data);

            AVSDEC_DEBUG(("PicBuffer[%d]: %x, %x\n",
                            i,
                            (u32) pDecCont->StrmStorage.pPicBuf[i].data.
                            virtualAddress,
                            pDecCont->StrmStorage.pPicBuf[i].data.busAddress));

            if(pDecCont->StrmStorage.pPicBuf[i].data.busAddress == 0)
            {
                AvsDecRelease(pDecCont);
                return (AVSDEC_MEMFAIL);
            }
        }
        /* initialize first picture buffer (workOut is 1 for the first picture)
         * grey, may be used as reference in certain error cases */
        (void) G1DWLmemset(pDecCont->StrmStorage.pPicBuf[1].data.virtualAddress,
                         128, 384 * pDecCont->StrmStorage.totalMbsInFrame);
    }

    ret |= G1DWLMallocLinear(pDecCont->dwl,
        ((pDecCont->StrmStorage.totalMbsInFrame+3)&~0x3) * 4 * sizeof(u32),
        &pDecCont->StrmStorage.directMvs);

    if(ret)
    {
        return (AVSDEC_MEMFAIL);
    }

    return AVSDEC_OK;

}

 /*------------------------------------------------------------------------------

    x.x Function name:  AvsDecCheckSupport

        Purpose:        Check picture sizes etc

        Input:          DecContainer *pDecCont

        Output:         AVSDEC_STRMERROR/AVSDEC_OK

------------------------------------------------------------------------------*/

AvsDecRet AvsDecCheckSupport(DecContainer * pDecCont)
{

    DWLHwConfig_t hwConfig;

    DWLReadAsicConfig(&hwConfig);

    if((pDecCont->StrmStorage.frameHeight > (hwConfig.maxDecPicWidth >> 4)) ||
       (pDecCont->StrmStorage.frameHeight < (AVS_MIN_HEIGHT >> 4)))
    {

        AVSDEC_DEBUG(("AvsDecCheckSupport# Height not supported %d \n",
                        pDecCont->StrmStorage.frameHeight));
        return AVSDEC_STREAM_NOT_SUPPORTED;
    }

    if((pDecCont->StrmStorage.frameWidth > (hwConfig.maxDecPicWidth >> 4)) ||
       (pDecCont->StrmStorage.frameWidth < (AVS_MIN_WIDTH >> 4)))
    {

        AVSDEC_DEBUG(("AvsDecCheckSupport# Width not supported %d \n",
                        pDecCont->StrmStorage.frameWidth));
        return AVSDEC_STREAM_NOT_SUPPORTED;
    }

    if(pDecCont->StrmStorage.totalMbsInFrame > AVSAPI_DEC_MBS)
    {
        AVSDEC_DEBUG(("Maximum number of macroblocks exceeded %d \n",
                        pDecCont->StrmStorage.totalMbsInFrame));
        return AVSDEC_STREAM_NOT_SUPPORTED;
    }

    return AVSDEC_OK;

}

/*------------------------------------------------------------------------------

    x.x Function name:  AvsDecPreparePicReturn

        Purpose:        Prepare return values for PIC returns
                        For use after HW start

        Input:          DecContainer *pDecCont
                        AvsDecOutput *outData    currently used out

        Output:         void

------------------------------------------------------------------------------*/

void AvsDecPreparePicReturn(DecContainer * pDecCont)
{

    AvsDecOutput *pOut;

    ASSERT(pDecCont != NULL);

    pOut = &pDecCont->outData;

    if(!pDecCont->Hdrs.progressiveSequence)
    {
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].ff[0] = 1;
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].ff[1] = 0;
    }
    else
    {
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].ff[0] = 0;
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].ff[1] = 0;
    }

    return;

}

 /*------------------------------------------------------------------------------

    x.x Function name:  AvsDecAspectRatio

        Purpose:        Set aspect ratio values for GetInfo

        Input:          DecContainer *pDecCont
                        AvsDecInfo * pDecInfo    pointer to DecInfo

        Output:         void

------------------------------------------------------------------------------*/

void AvsDecAspectRatio(DecContainer * pDecCont, AvsDecInfo * pDecInfo)
{

    AVSDEC_DEBUG(("SAR %d\n", pDecCont->Hdrs.aspectRatio));

    /* If forbidden or reserved */
    if(pDecCont->Hdrs.aspectRatio == 0 || pDecCont->Hdrs.aspectRatio > 4)
    {
        pDecInfo->displayAspectRatio = 0;
        return;
    }

    switch (pDecCont->Hdrs.aspectRatio)
    {
        case 0x2:  /* 0010 4:3 */
            pDecInfo->displayAspectRatio = AVSDEC_4_3;
            break;

        case 0x3:  /* 0011 16:9 */
            pDecInfo->displayAspectRatio = AVSDEC_16_9;
            break;

        case 0x4:  /* 0100 2.21:1 */
            pDecInfo->displayAspectRatio = AVSDEC_2_21_1;
            break;

        default:   /* Square 0001 1/1 */
            pDecInfo->displayAspectRatio = AVSDEC_1_1;
            break;
    }

    /* TODO!  "DAR" */

}

/*------------------------------------------------------------------------------

    x.x Function name:  AvsDecBufferPicture

        Purpose:        Handles picture buffering

        Input:          
                        

        Output:         

------------------------------------------------------------------------------*/
void AvsDecBufferPicture(DecContainer * pDecCont, u32 picId, u32 bufferB,
                           u32 isInter, AvsDecRet returnValue, u32 nbrErrMbs)
{
    i32 i, j;

    ASSERT(pDecCont);
    ASSERT(pDecCont->StrmStorage.outCount <= 16);

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
        if(j >= 16)
            j -= 16;
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
    if (!bufferB)
        pDecCont->StrmStorage.pPicBuf[j].pictureDistance =
            pDecCont->Hdrs.pictureDistance;

    if(pDecCont->Hdrs.pictureStructure != FRAMEPICTURE)
        pDecCont->StrmStorage.pPicBuf[j].nbrErrMbs = nbrErrMbs / 2;
    else
        pDecCont->StrmStorage.pPicBuf[j].nbrErrMbs = nbrErrMbs;

    if(pDecCont->ppInstance != NULL && returnValue == FREEZED_PIC_RDY)
        pDecCont->StrmStorage.pPicBuf[j].sendToPp = 2;

    AvsDecTimeCode(pDecCont, &pDecCont->StrmStorage.pPicBuf[j].timeCode);

    pDecCont->StrmStorage.outCount++;

}

/*------------------------------------------------------------------------------

    x.x Function name:  AvsFreeBuffers

        Purpose:        

        Input:          DecContainer *pDecCont

        Output:         

------------------------------------------------------------------------------*/
void AvsFreeBuffers(DecContainer * pDecCont)
{

    u32 i;

    BqueueRelease( &pDecCont->StrmStorage.bq );

    /* Reference images */
    for(i = 0; i < 16; i++)
    {
        if (pDecCont->StrmStorage.pPicBuf[i].data.virtualAddress != NULL)
        {
            DWLFreeRefFrm(pDecCont->dwl, &pDecCont->StrmStorage.pPicBuf[i].data);
            pDecCont->StrmStorage.pPicBuf[i].data.virtualAddress = NULL;
            pDecCont->StrmStorage.pPicBuf[i].data.busAddress = 0;
        }
    }

    if (pDecCont->StrmStorage.directMvs.virtualAddress != NULL)
        DWLFreeLinear(pDecCont->dwl, &pDecCont->StrmStorage.directMvs);

}
