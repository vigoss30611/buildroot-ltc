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
--  $RCSfile: mp4decapi_internal.c,v $
--  $Date: 2010/12/02 10:53:51 $
--  $Revision: 1.18 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of contents

    1.  Include headers
    5.  Fuctions
        5.1 MP4API_InitializeHW()
        5.2 ClearDataStructures()

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1.  Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "mp4dechwd_container.h"
#include "mp4deccfg.h"
#include "mp4decapi.h"
#include "mp4decdrv.h"
#include "mp4debug.h"
#include "mp4dechwd_utils.h"
#include "mp4decapi_internal.h"
#ifdef MP4_ASIC_TRACE
#include "mpeg4asicdbgtrace.h"
#endif

static void MP4LoadQuantMatrix(DecContainer * pDecCont);
static u32 MP4DecCheckProfileSupport(DecContainer * pDecCont);

/*------------------------------------------------------------------------------

        Function name:  ClearDataStructures()

        Purpose:        Initialize Data Structures in DecContainer.

        Input:          DecContainer *pDecCont

        Output:         u32

------------------------------------------------------------------------------*/
void MP4API_InitDataStructures(DecContainer * pDecCont)
{

/*
 *  have to be initialized into 1 to enable
 *  decoding stream without VO-headers
 */
    pDecCont->Hdrs.visualObjectVerid = 1;
    pDecCont->Hdrs.videoFormat = 5;
    pDecCont->Hdrs.colourPrimaries = 1;
    pDecCont->Hdrs.transferCharacteristics = 1;
    pDecCont->Hdrs.matrixCoefficients = 1;
    pDecCont->Hdrs.lowDelay = 0;

    pDecCont->StrmStorage.vpQP = 1;
    pDecCont->ApiStorage.firstHeaders = 1;

    pDecCont->StrmStorage.workOut = 0;
    pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.work1 =
        INVALID_ANCHOR_PICTURE;

}

/*------------------------------------------------------------------------------

        Function name:  MP4DecTimeCode

        Purpose:        Write time data to output

        Input:          DecContainer *pDecCont, MP4DecTime *timeCode

        Output:         void

------------------------------------------------------------------------------*/

void MP4DecTimeCode(DecContainer * pDecCont, MP4DecTime * timeCode)
{

#define DEC_VOPD pDecCont->VopDesc
#define DEC_HDRS pDecCont->Hdrs
#define DEC_STST pDecCont->StrmStorage
#define DEC_SVDS pDecCont->SvDesc

    ASSERT(pDecCont);
    ASSERT(timeCode);

    if(DEC_STST.shortVideo)
    {

        DEC_HDRS.vopTimeIncrementResolution = 30000;

        DEC_VOPD.vopTimeIncrement +=
            (pDecCont->VopDesc.ticsFromPrev * 1001);
        while(DEC_VOPD.vopTimeIncrement >= 30000)
        {
            DEC_VOPD.vopTimeIncrement -= 30000;
            DEC_VOPD.timeCodeSeconds++;
            if(DEC_VOPD.timeCodeSeconds > 59)
            {
                DEC_VOPD.timeCodeMinutes++;
                DEC_VOPD.timeCodeSeconds = 0;
                if(DEC_VOPD.timeCodeMinutes > 59)
                {
                    DEC_VOPD.timeCodeHours++;
                    DEC_VOPD.timeCodeMinutes = 0;
                    if(DEC_VOPD.timeCodeHours > 23)
                    {
                        DEC_VOPD.timeCodeHours = 0;
                    }
                }
            }
        }
    }
    timeCode->hours = DEC_VOPD.timeCodeHours;
    timeCode->minutes = DEC_VOPD.timeCodeMinutes;
    timeCode->seconds = DEC_VOPD.timeCodeSeconds;
    timeCode->timeIncr = DEC_VOPD.vopTimeIncrement;
    timeCode->timeRes = DEC_HDRS.vopTimeIncrementResolution;

#undef DEC_VOPD
#undef DEC_HDRS
#undef DEC_STST
#undef DEC_SVDS

}

 /*------------------------------------------------------------------------------

        Function name:  MP4NotCodedVop

        Purpose:        prepare HW for not coded VOP, rlc mode

        Input:          DecContainer *pDecCont, TimeCode *timeCode

        Output:         void

------------------------------------------------------------------------------*/

void MP4NotCodedVop(DecContainer * pDecContainer)
{

    extern const u8 asicPosNoRlc[6];
    u32 asicTmp = 0;
    u32 i = 0;

    asicTmp |= (1U << ASICPOS_VPBI);
    asicTmp |= (1U << ASICPOS_MBTYPE);
    asicTmp |= (1U << ASICPOS_MBNOTCODED);

    asicTmp |= (pDecContainer->StrmStorage.qP << ASICPOS_QP);
    for(i = 0; i < 6; i++)
    {
        asicTmp |= (1 << asicPosNoRlc[i]);
    }

    *pDecContainer->MbSetDesc.pCtrlDataAddr = asicTmp;

    /* only first has VP boundary */

    asicTmp &= ~(1U << ASICPOS_VPBI);

    for(i = 1; i < pDecContainer->VopDesc.totalMbInVop; i++)
    {
        *(pDecContainer->MbSetDesc.pCtrlDataAddr + i) = asicTmp;
        pDecContainer->MbSetDesc.pMvDataAddr[i*NBR_MV_WORDS_MB] = 0;
    }
    pDecContainer->MbSetDesc.pMvDataAddr[0*NBR_MV_WORDS_MB] = 0;

}

 /*------------------------------------------------------------------------------

        Function name:  MP4AllocateBuffers

        Purpose:        Allocate memory

        Input:          DecContainer *pDecCont

        Output:         MP4DEC_MEMFAIL/MP4DEC_OK

------------------------------------------------------------------------------*/

MP4DecRet MP4AllocateBuffers(DecContainer * pDecCont)
{
#define DEC_VOPD pDecCont->VopDesc

    u32 i;
    i32 ret = 0;
    u32 sizeTmp = 0;
    u32 buffers = 0;
    u32 *p;
    u32 tmp;
    const u8 defaultIntraMat[64] = {
         8, 17, 18, 19, 21, 23, 25, 27, 17, 18, 19, 21, 23, 25, 27, 28,
        20, 21, 22, 23, 24, 26, 28, 30, 21, 22, 23, 24, 26, 28, 30, 32,
        22, 23, 24, 26, 28, 30, 32, 35, 23, 24, 26, 28, 30, 32, 35, 38,
        25, 26, 28, 30, 32, 35, 38, 41, 27, 28, 30, 32, 35, 38, 41, 45 };

    const u8 defaultNonIntraMat[64] = {
        16, 17, 18, 19, 20, 21, 22, 23, 17, 18, 19, 20, 21, 22, 23, 24,
        18, 19, 20, 21, 22, 23, 24, 25, 19, 20, 21, 22, 23, 24, 26, 27,
        20, 21, 22, 23, 25, 26, 27, 28, 21, 22, 23, 24, 26, 27, 28, 30,
        22, 23, 24, 26, 27, 28, 30, 31, 23, 24, 25, 27, 28, 30, 31, 33 };

    /* Allocate mb control buffer */

    ASSERT(DEC_VOPD.totalMbInVop != 0);

    if (pDecCont->rlcMode)
    {
        if (MP4AllocateRlcBuffers(pDecCont) != MP4DEC_OK)
            return (MP4DEC_MEMFAIL);
    }

    sizeTmp = (MP4API_DEC_FRAME_BUFF_SIZE * DEC_VOPD.totalMbInVop * 4);

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
        buffers =  2; /*pDecCont->Hdrs.interlaced ? 1 : 2;*/
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
        return MP4DEC_MEMFAIL;

    ret = BqueueInit(&pDecCont->StrmStorage.bqPp, 
                     pDecCont->StrmStorage.numPpBuffers );
    if( ret != HANTRO_OK )
        return MP4DEC_MEMFAIL;


    for (i = 0; i < pDecCont->StrmStorage.numBuffers ; i++)
    {
        ret |= G1DWLMallocRefFrm(pDecCont->dwl, sizeTmp,
            &pDecCont->StrmStorage.data[i]);
        pDecCont->StrmStorage.pPicBuf[i].dataIndex = i;
        if(pDecCont->StrmStorage.data[i].busAddress == 0)
        {
            return (MP4DEC_MEMFAIL);
        }
    }

    ret = G1DWLMallocLinear(pDecCont->dwl,
        ((DEC_VOPD.totalMbInVop+1)&~0x1)*4*sizeof(u32),
        &pDecCont->StrmStorage.directMvs);
    ret |= G1DWLMallocLinear(pDecCont->dwl, 2*64,
        &pDecCont->StrmStorage.quantMatLinear);
    if(ret)
    {
        return (MP4DEC_MEMFAIL);
    }

    /* initialize quantization tables */
    p = (u32 *)pDecCont->StrmStorage.quantMatLinear.virtualAddress;
    for (i = 0; i < 16; i++)
    {
        tmp = (defaultIntraMat[4*i+0]<<24) |
              (defaultIntraMat[4*i+1]<<16) |
              (defaultIntraMat[4*i+2]<<8) |
              (defaultIntraMat[4*i+3]<<0);
        p[i] = tmp;
        tmp = (defaultNonIntraMat[4*i+0]<<24) |
              (defaultNonIntraMat[4*i+1]<<16) |
              (defaultNonIntraMat[4*i+2]<<8) |
              (defaultNonIntraMat[4*i+3]<<0);
        p[i+16] = tmp;
    }

    if(pDecCont->Hdrs.quantType)
        MP4LoadQuantMatrix(pDecCont);
/*
 *  pDecCont->MbSetDesc
 */

    pDecCont->MbSetDesc.oddRlc = 0;

    /* initialize first picture buffer grey, may be used as reference
     * in certain error cases */
    (void) G1DWLmemset(pDecCont->StrmStorage.data[0].virtualAddress,
                     128, 384 * DEC_VOPD.totalMbInVop);

    return MP4DEC_OK;

#undef DEC_VOPD
}

/*------------------------------------------------------------------------------

        Function name:  MP4FreeBuffers

        Purpose:        Free memory

        Input:          DecContainer *pDecCont

        Output:         

------------------------------------------------------------------------------*/

void MP4FreeBuffers(DecContainer * pDecCont)
{

    u32 i;

    /* Allocate mb control buffer */
    BqueueRelease(&pDecCont->StrmStorage.bq);
    BqueueRelease(&pDecCont->StrmStorage.bqPp);

    if(pDecCont->MbSetDesc.ctrlDataMem.virtualAddress != NULL)
    {
        DWLFreeLinear(pDecCont->dwl, &pDecCont->MbSetDesc.ctrlDataMem);
        pDecCont->MbSetDesc.ctrlDataMem.virtualAddress = NULL;
    }
    if(pDecCont->MbSetDesc.mvDataMem.virtualAddress != NULL)
    {
        DWLFreeLinear(pDecCont->dwl, &pDecCont->MbSetDesc.mvDataMem);
        pDecCont->MbSetDesc.mvDataMem.virtualAddress = NULL;
    }
    if(pDecCont->MbSetDesc.rlcDataMem.virtualAddress != NULL)
    {
        DWLFreeLinear(pDecCont->dwl, &pDecCont->MbSetDesc.rlcDataMem);
        pDecCont->MbSetDesc.rlcDataMem.virtualAddress = NULL;
    }
    if(pDecCont->MbSetDesc.DcCoeffMem.virtualAddress != NULL)
    {
        DWLFreeLinear(pDecCont->dwl, &pDecCont->MbSetDesc.DcCoeffMem);
        pDecCont->MbSetDesc.DcCoeffMem.virtualAddress = NULL;
    }
    if(pDecCont->StrmStorage.directMvs.virtualAddress != NULL)
    {
        DWLFreeLinear(pDecCont->dwl, &pDecCont->StrmStorage.directMvs);
        pDecCont->StrmStorage.directMvs.virtualAddress = NULL;
    }

    if(pDecCont->StrmStorage.quantMatLinear.virtualAddress != NULL)
    {
        DWLFreeLinear(pDecCont->dwl, &pDecCont->StrmStorage.quantMatLinear);
        pDecCont->StrmStorage.quantMatLinear.virtualAddress = NULL;
    }

    for(i = 0; i < pDecCont->StrmStorage.numBuffers ; i++)
    {
        if(pDecCont->StrmStorage.data[i].virtualAddress != NULL)
        {
            DWLFreeRefFrm(pDecCont->dwl, &pDecCont->StrmStorage.data[i]);
            pDecCont->StrmStorage.data[i].virtualAddress = NULL;
        }
    }

}

/*------------------------------------------------------------------------------

        Function name:  MP4AllocateRlcBuffers

        Purpose:        Allocate memory for rlc mode

        Input:          DecContainer *pDecCont

        Output:         MP4DEC_MEMFAIL/MP4DEC_OK

------------------------------------------------------------------------------*/

MP4DecRet MP4AllocateRlcBuffers(DecContainer * pDecCont)
{
#define DEC_VOPD pDecCont->VopDesc

    i32 ret = 0;
    u32 sizeRlc = 0, sizeMv = 0, sizeControl = 0, sizeDc = 0;

    ASSERT(DEC_VOPD.totalMbInVop != 0);

    if (pDecCont->rlcMode)
    {
        sizeControl = NBR_OF_WORDS_MB * DEC_VOPD.totalMbInVop * 4;

        ret |= G1DWLMallocLinear(pDecCont->dwl, sizeControl,
                               &pDecCont->MbSetDesc.ctrlDataMem);

        pDecCont->MbSetDesc.pCtrlDataAddr =
            pDecCont->MbSetDesc.ctrlDataMem.virtualAddress;

        /* Allocate motion vector data buffer */
        sizeMv = NBR_MV_WORDS_MB * DEC_VOPD.totalMbInVop * 4;
        ret |= G1DWLMallocLinear(pDecCont->dwl, sizeMv,
                               &pDecCont->MbSetDesc.mvDataMem);
        pDecCont->MbSetDesc.pMvDataAddr =
            pDecCont->MbSetDesc.mvDataMem.virtualAddress;

        /* RLC data buffer */

        sizeRlc = (_MP4_RLC_BUFFER_SIZE * DEC_VOPD.totalMbInVop * 4);

        ret |= G1DWLMallocLinear(pDecCont->dwl, sizeRlc,
                              &pDecCont->MbSetDesc.rlcDataMem);
        pDecCont->MbSetDesc.rlcDataBufferSize = sizeRlc;
        pDecCont->MbSetDesc.pRlcDataAddr =
            pDecCont->MbSetDesc.rlcDataMem.virtualAddress;
        pDecCont->MbSetDesc.pRlcDataCurrAddr = pDecCont->MbSetDesc.pRlcDataAddr;
        pDecCont->MbSetDesc.pRlcDataVpAddr = pDecCont->MbSetDesc.pRlcDataAddr;

        /* Separate DC component data buffer */

        sizeDc = (NBR_DC_WORDS_MB * DEC_VOPD.totalMbInVop * 4);

        ret |= G1DWLMallocLinear(pDecCont->dwl, sizeDc,
                               &pDecCont->MbSetDesc.DcCoeffMem);
        pDecCont->MbSetDesc.pDcCoeffDataAddr =
            pDecCont->MbSetDesc.DcCoeffMem.virtualAddress;

        if(ret)
            return (MP4DEC_MEMFAIL);
    }

    /* reset memories */

    (void)G1DWLmemset(pDecCont->MbSetDesc.ctrlDataMem.virtualAddress,
        0x0, sizeControl);
    (void)G1DWLmemset(pDecCont->MbSetDesc.mvDataMem.virtualAddress,
        0x0, sizeMv);
    (void)G1DWLmemset(pDecCont->MbSetDesc.rlcDataMem.virtualAddress,
        0x0, sizeRlc);
    (void)G1DWLmemset(pDecCont->MbSetDesc.DcCoeffMem.virtualAddress,
        0x0, sizeDc);

    return MP4DEC_OK;

#undef DEC_VOPD
}

/*------------------------------------------------------------------------------

        Function name:  MP4DecAllocExtraBPic

        Purpose:        allocate b picture after normal allocation

        Input:          DecContainer *pDecCont

        Output:         MP4DEC_STRMERROR/MP4DEC_OK

------------------------------------------------------------------------------*/

MP4DecRet MP4DecAllocExtraBPic(DecContainer * pDecCont)
{
    i32 ret = 0;
    u32 sizeTmp = 0;
    u32 extraBuffer = 0;

    /* If we already have enough buffers, do nothing. */
    if( pDecCont->StrmStorage.numBuffers >= 3)
        return MP4DEC_OK;

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

    sizeTmp = (MP4API_DEC_FRAME_BUFF_SIZE * pDecCont->VopDesc.totalMbInVop * 4);

    BqueueRelease(&pDecCont->StrmStorage.bq);
    ret = BqueueInit(&pDecCont->StrmStorage.bq, 
                        pDecCont->StrmStorage.numBuffers );
    if(ret != HANTRO_OK)
        return (MP4DEC_MEMFAIL);

    ret = G1DWLMallocRefFrm(pDecCont->dwl, sizeTmp,
           &pDecCont->StrmStorage.data[2]);
    pDecCont->StrmStorage.pPicBuf[2].dataIndex = 2;
    if(pDecCont->StrmStorage.data[2].busAddress == 0 || ret)
    {
        return (MP4DEC_MEMFAIL);
    }

    /* Allocate "extra" extra B buffer */
    if(extraBuffer)
    {
        ret = G1DWLMallocRefFrm(pDecCont->dwl, sizeTmp,
                              &pDecCont->StrmStorage.data[3]);
         if(pDecCont->StrmStorage.data[3].busAddress == 0 || ret)
        {
            return (MP4DEC_MEMFAIL);
        }
    }
    return (MP4DEC_OK);
}

/*------------------------------------------------------------------------------

        Function name:  MP4DecCheckSupport

        Purpose:        Check picture sizes etc

        Input:          DecContainer *pDecCont

        Output:         MP4DEC_STRM_NOT_SUPPORTED/MP4DEC_OK

------------------------------------------------------------------------------*/

MP4DecRet MP4DecCheckSupport(DecContainer * pDecCont)
{
#define DEC_VOPD pDecCont->VopDesc
    DWLHwConfig_t hwConfig;

    DWLReadAsicConfig(&hwConfig);

    if((pDecCont->VopDesc.vopHeight > (hwConfig.maxDecPicWidth >> 4)) ||
       (pDecCont->VopDesc.vopHeight < (MP4_MIN_HEIGHT >> 4)))
    {

        MP4DEC_DEBUG(("MP4DecCheckSupport# Height not supported %d \n",
                      DEC_VOPD.vopHeight));
        return MP4DEC_STRM_NOT_SUPPORTED;
    }

    if((DEC_VOPD.vopWidth > (hwConfig.maxDecPicWidth >> 4)) ||
       (DEC_VOPD.vopWidth < (MP4_MIN_WIDTH >> 4)))
    {

        MP4DEC_DEBUG(("MP4DecCheckSupport# Width not supported %d \n",
                       DEC_VOPD.vopWidth));
        return MP4DEC_STRM_NOT_SUPPORTED;
    }

    /* Check height of interlaced pic */

    if(pDecCont->VopDesc.vopHeight < (MP4_MIN_HEIGHT >> 3)
       && pDecCont->Hdrs.interlaced )
    {
        MP4DEC_DEBUG(("Interlaced height not supported\n"));
        return MP4DEC_STRM_NOT_SUPPORTED;
    }

    if(DEC_VOPD.totalMbInVop > MP4API_DEC_MBS)
    {
        MP4DEC_DEBUG(("Maximum number of macroblocks exceeded %d \n",
                     DEC_VOPD.totalMbInVop));
        return MP4DEC_STRM_NOT_SUPPORTED;
    }

    if(MP4DecCheckProfileSupport(pDecCont))
    {
        MP4DEC_DEBUG(("Profile not supported\n"));
        return MP4DEC_STRM_NOT_SUPPORTED;
    }
    return MP4DEC_OK;

#undef DEC_VOPD
}


 /*------------------------------------------------------------------------------

    x.x Function name:  MP4DecPixelAspectRatio

        Purpose:        Set pixel aspext ratio values for GetInfo


        Input:          DecContainer *pDecCont
                        MP4DecInfo * pDecInfo    pointer to DecInfo

        Output:         void

------------------------------------------------------------------------------*/
void MP4DecPixelAspectRatio(DecContainer * pDecCont, MP4DecInfo * pDecInfo){


    MP4DEC_DEBUG(("PAR %d\n", pDecCont->Hdrs.aspectRatioInfo));


    switch(pDecCont->Hdrs.aspectRatioInfo)
    {

        case 0x2: /* 0010 12:11 */
            pDecInfo->parWidth = 12;
            pDecInfo->parHeight = 11;
            break;

        case 0x3: /* 0011 10:11 */
            pDecInfo->parWidth = 10;
            pDecInfo->parHeight = 11;
            break;

        case 0x4: /* 0100 16:11 */
            pDecInfo->parWidth = 16;
            pDecInfo->parHeight = 11;
            break;

        case 0x5: /* 0101 40:11 */
            pDecInfo->parWidth = 40;
            pDecInfo->parHeight = 33;
            break;

        case 0xF: /* 1111 Extended PAR */
            pDecInfo->parWidth = pDecCont->Hdrs.parWidth;
            pDecInfo->parHeight = pDecCont->Hdrs.parHeight;
            break;

        default: /* Square */
            pDecInfo->parWidth = pDecInfo->parHeight = 1;
            break;
    }
    return;
}

/*------------------------------------------------------------------------------

        Function name:  MP4DecBufferPicture

        Purpose:        Rotate buffers and store information about picture


        Input:          DecContainer *pDecCont
                        picId, vop type, return value and time information

        Output:         HANTRO_OK / HANTRO_NOK

------------------------------------------------------------------------------*/
void MP4DecBufferPicture(DecContainer *pDecCont, u32 picId, u32 vopType,
                        u32 nbrErrMbs)
{

    i32 i, j;

    ASSERT(pDecCont);
    ASSERT(pDecCont->StrmStorage.outCount <= 
        pDecCont->StrmStorage.numBuffers - 1);

    if( vopType != BVOP ) /* Buffer I or P picture */
    {
        i = pDecCont->StrmStorage.outIndex + pDecCont->StrmStorage.outCount;
        if( i >= 16 )
        {
            i -= 16;
        }
    }
    else /* Buffer B picture */
    {
        j = pDecCont->StrmStorage.outIndex + pDecCont->StrmStorage.outCount;
        i = j - 1;
        if( j >= 16 ) j -= 16;
        if( i >= 16 ) i -= 16;
        if( i < 0 ) i += 16;
        pDecCont->StrmStorage.outBuf[j] = pDecCont->StrmStorage.outBuf[i];
    }
    j = pDecCont->StrmStorage.workOut;

    pDecCont->StrmStorage.outBuf[i] = j;
    pDecCont->StrmStorage.pPicBuf[j].picId = picId;
    pDecCont->StrmStorage.pPicBuf[j].isInter = vopType;
    pDecCont->StrmStorage.pPicBuf[j].picType = vopType;
    pDecCont->StrmStorage.pPicBuf[j].nbrErrMbs = nbrErrMbs;
    pDecCont->StrmStorage.pPicBuf[j].tiledMode =
        pDecCont->tiledReferenceEnable;

    MP4DecTimeCode(pDecCont, &pDecCont->StrmStorage.pPicBuf[j].timeCode);

    pDecCont->StrmStorage.outCount++;

}
 /*------------------------------------------------------------------------------

        Function name:  MP4LoadQuantMatrix

        Purpose:        Set hw to use stream define matrises



        Input:          DecContainer *pDecCont

        Output:         void

------------------------------------------------------------------------------*/
static void MP4LoadQuantMatrix(DecContainer * pDecCont)
{

    u32 i, tmp;
    u8 *p;
    u32 *pLin;

    p = (u8 *)pDecCont->StrmStorage.quantMat;
    pLin = (u32 *)pDecCont->StrmStorage.quantMatLinear.virtualAddress;

    if(p[0])
    {
        for (i = 0; i < 16; i++)
        {
            tmp = (p[4*i+0]<<24) | (p[4*i+1]<<16) |
                  (p[4*i+2]<<8)  | (p[4*i+3]<<0);
            pLin[i] = tmp;
        }
    }

    if(p[64])
    {
        for (i = 16; i < 32; i++)
        {
            tmp = (p[4*i+0]<<24) | (p[4*i+1]<<16) |
                  (p[4*i+2]<<8)  | (p[4*i+3]<<0);
            pLin[i] = tmp;
        }
    }

}
 /*------------------------------------------------------------------------------

        Function name:  MP4DecCheckProfileSupport

        Purpose:        Check support for ASP tools

        Input:          DecContainer *pDecCont

        Output:         void

------------------------------------------------------------------------------*/
static u32 MP4DecCheckProfileSupport(DecContainer * pDecCont)
{
    u32 ret = 0;
    DWLHwConfig_t hwConfig;

    DWLReadAsicConfig(&hwConfig);

    if(hwConfig.mpeg4Support == MPEG4_SIMPLE_PROFILE &&
       !pDecCont->StrmStorage.sorensonSpark)
    {

        if(pDecCont->Hdrs.quantType)
            ret++;

        if(!pDecCont->Hdrs.lowDelay)
            ret++;

        if(pDecCont->Hdrs.interlaced)
            ret++;

        if(pDecCont->Hdrs.quarterpel)
            ret++;
    }

    return ret;
}


 /*------------------------------------------------------------------------------

        Function name:  MP4DecBFrameSupport

        Purpose:        Check support for B frames

        Input:          DecContainer *pDecCont

        Output:         void

------------------------------------------------------------------------------*/
u32 MP4DecBFrameSupport(DecContainer * pDecCont)
{
    u32 ret = HANTRO_TRUE;
    DWLHwConfig_t hwConfig;

    DWLReadAsicConfig(&hwConfig);

    if(hwConfig.mpeg4Support == MPEG4_SIMPLE_PROFILE )
        ret = HANTRO_FALSE;

    return ret;
}
/*------------------------------------------------------------------------------

        Function name:  MP4DecResolveVirtual

        Purpose:        Get virtual address for this picture

        Input:          DecContainer *pDecCont

        Output:         void

------------------------------------------------------------------------------*/
u32 * MP4DecResolveVirtual(DecContainer * pDecCont, u32 index )
{
    if( (i32)index < 0 )
        return NULL;
    if(index > 15)
        return NULL;
    if(pDecCont->StrmStorage.pPicBuf[index].dataIndex > 15)
        return NULL;
    return pDecCont->StrmStorage.data[pDecCont->StrmStorage.
           pPicBuf[index].dataIndex].virtualAddress;
}

/*------------------------------------------------------------------------------

        Function name:  MP4DecResolveBus

        Purpose:        Get bus address for this picture

        Input:          DecContainer *pDecCont

        Output:         void

------------------------------------------------------------------------------*/
u32 MP4DecResolveBus(DecContainer * pDecCont, u32 index )
{
    if( (i32)index < 0 )
        return 0;
    if(index > 15)
        return 0;
    if(pDecCont->StrmStorage.pPicBuf[index].dataIndex > 15)
        return 0;
    return pDecCont->StrmStorage.data[pDecCont->StrmStorage.
           pPicBuf[index].dataIndex].busAddress;
}

/*------------------------------------------------------------------------------

        Function name:  MP4DecChangeDataIndex

        Purpose:        Move picture storage to point to a different physical
                        picture

        Input:          DecContainer *pDecCont

        Output:         void

------------------------------------------------------------------------------*/
void MP4DecChangeDataIndex( DecContainer * pDecCont, u32 to, u32 from)
{

    pDecCont->StrmStorage.pPicBuf[to].dataIndex =
    pDecCont->StrmStorage.pPicBuf[from].dataIndex;
}

