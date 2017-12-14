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
-
-  Description : ...
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp8decapi.c,v $
-  $Revision: 1.109 $
-  $Date: 2011/02/09 14:36:02 $
-
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "decapicommon.h"
#include "vp8decapi.h"

#include "dwl.h"
#include "vp8hwd_container.h"

#include "vp8hwd_debug.h"
#include "vp8hwd_asic.h"
#include "vp8hwd_headers.h"
#include "regdrv.h"
#include "refbuffer.h"
#include "tiledref.h"

#define VP8DEC_MAJOR_VERSION 1
#define VP8DEC_MINOR_VERSION 0

#define VP8DEC_BUILD_MAJOR 0
#define VP8DEC_BUILD_MINOR 95
#define VP8DEC_SW_BUILD ((VP8DEC_BUILD_MAJOR * 1000) + VP8DEC_BUILD_MINOR)

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#include <stdio.h>
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

#ifdef VP8DEC_TRACE
#define DEC_API_TRC(str)    VP8DecTrace(str)
#else
#define DEC_API_TRC(str)    do{}while(0)
#endif

#define MB_MULTIPLE(x)  (((x)+15)&~15)

#define MIN_PIC_WIDTH  48
#define MIN_PIC_HEIGHT 48
#define MAX_PIC_SIZE   1920*1088

extern void vp8hwdPreparePpRun(VP8DecContainer_t *pDecCont);
static u32 vp8hwdCheckSupport( VP8DecContainer_t *pDecCont );
static void vp8hwdFreeze(VP8DecContainer_t *pDecCont);

/*------------------------------------------------------------------------------
    Function name : VP8DecGetAPIVersion
    Description   : Return the API version information

    Return type   : VP8DecApiVersion
    Argument      : void
------------------------------------------------------------------------------*/
VP8DecApiVersion VP8DecGetAPIVersion(void)
{
    VP8DecApiVersion ver;

    ver.major = VP8DEC_MAJOR_VERSION;
    ver.minor = VP8DEC_MINOR_VERSION;

    DEC_API_TRC("VP8DecGetAPIVersion# OK\n");

    return ver;
}

/*------------------------------------------------------------------------------
    Function name : VP8DecGetBuild
    Description   : Return the SW and HW build information

    Return type   : VP8DecBuild
    Argument      : void
------------------------------------------------------------------------------*/
VP8DecBuild VP8DecGetBuild(void)
{
    VP8DecBuild ver;
    DWLHwConfig_t hwCfg;

    (void)G1DWLmemset(&ver, 0, sizeof(ver));

    ver.swBuild = VP8DEC_SW_BUILD;
    ver.hwBuild = G1DWLReadAsicID();

    DWLReadAsicConfig(&hwCfg);

    SET_DEC_BUILD_SUPPORT(ver.hwConfig, hwCfg);

    DEC_API_TRC("VP8DecGetBuild# OK\n");

    return (ver);
}

/*------------------------------------------------------------------------------
    Function name   : vp8decinit
    Description     : 
    Return type     : VP8DecRet 
    Argument        : VP8DecInst * pDecInst
                      u32 useVideoFreezeConcealment
------------------------------------------------------------------------------*/
VP8DecRet VP8DecInit(VP8DecInst * pDecInst, VP8DecFormat decFormat, 
                     u32 useVideoFreezeConcealment,
                     u32 numFrameBuffers,
                     DecRefFrmFormat referenceFrameFormat, 
                     u32 mmuEnable )
{
    VP8DecRet ret;
    VP8DecContainer_t *pDecCont;
    const void *dwl;
    u32 i;

    G1DWLInitParam_t dwlInit;
    DWLHwConfig_t config;

    DEC_API_TRC("VP8DecInit#\n");

    /* check that right shift on negative numbers is performed signed */
    /*lint -save -e* following check causes multiple lint messages */
#if (((-1) >> 1) != (-1))
#error Right bit-shifting (>>) does not preserve the sign
#endif
    /*lint -restore */

    if(pDecInst == NULL)
    {
        DEC_API_TRC("VP8DecInit# ERROR: pDecInst == NULL");
        return (VP8DEC_PARAM_ERROR);
    }

    *pDecInst = NULL;   /* return NULL instance for any error */

    /* check that decoding supported in HW */
    {

        DWLHwConfig_t hwCfg;

        DWLReadAsicConfig(&hwCfg);
        if(decFormat == VP8DEC_VP7 && !hwCfg.vp7Support)
        {
            DEC_API_TRC("VP8DecInit# ERROR: VP7 not supported in HW\n");
            return VP8DEC_FORMAT_NOT_SUPPORTED;
        }

        if((decFormat == VP8DEC_VP8 || decFormat == VP8DEC_WEBP) &&
            !hwCfg.vp8Support)
        {
            DEC_API_TRC("VP8DecInit# ERROR: VP8 not supported in HW\n");
            return VP8DEC_FORMAT_NOT_SUPPORTED;
        }

    }

    /* init DWL for the specified client */
    dwlInit.clientType = DWL_CLIENT_TYPE_VP8_DEC;
    dwlInit.mmuEnable = mmuEnable;

    dwl = G1DWLInit(&dwlInit);

    if(dwl == NULL)
    {
        DEC_API_TRC("VP8DecInit# ERROR: DWL Init failed\n");
        return (VP8DEC_DWL_ERROR);
    }

    /* allocate instance */
    pDecCont = (VP8DecContainer_t *) G1DWLmalloc(sizeof(VP8DecContainer_t));

    if(pDecCont == NULL)
    {
        DEC_API_TRC("VP8DecInit# ERROR: Memory allocation failed\n");

        (void)G1DWLRelease(dwl);
        return VP8DEC_MEMFAIL;
    }

    (void) G1DWLmemset(pDecCont, 0, sizeof(VP8DecContainer_t));
    pDecCont->dwl = dwl;

    /* initial setup of instance */

    pDecCont->decStat = VP8DEC_INITIALIZED;
    pDecCont->checksum = pDecCont;  /* save instance as a checksum */    

    if( numFrameBuffers > 16)
        numFrameBuffers = 16;
    switch(decFormat)
    {
        case VP8DEC_VP7:
            pDecCont->decMode = pDecCont->decoder.decMode = VP8HWD_VP7;
            if(numFrameBuffers < 3)
                numFrameBuffers = 3;
            break;
        case VP8DEC_VP8:
            pDecCont->decMode = pDecCont->decoder.decMode = VP8HWD_VP8;
            if(numFrameBuffers < 4)
                numFrameBuffers = 4;
            break;
        case VP8DEC_WEBP:
            pDecCont->decMode = pDecCont->decoder.decMode = VP8HWD_VP8;
            pDecCont->intraOnly = HANTRO_TRUE;
            numFrameBuffers = 1;
            break;
    }
    pDecCont->numBuffers = numFrameBuffers;
    VP8HwdAsicInit(pDecCont);   /* Init ASIC */

    if(VP8HwdAsicAllocateMem(pDecCont) != 0)
    {
        DEC_API_TRC("VP8DecInit# ERROR: ASIC Memory allocation failed\n");
        G1DWLfree(pDecCont);
        (void)G1DWLRelease(dwl);
        return VP8DEC_MEMFAIL;
    }

    (void)G1DWLmemset(&config, 0, sizeof(DWLHwConfig_t));

    DWLReadAsicConfig(&config);

    i = G1DWLReadAsicID() >> 16;
    if(i == 0x8170U)
        useVideoFreezeConcealment = 0;
    pDecCont->refBufSupport = config.refBufSupport;
    if(referenceFrameFormat == DEC_REF_FRM_TILED_DEFAULT)
    {
        /* Assert support in HW before enabling.. */
        if(!config.tiledModeSupport)
        {
            DEC_API_TRC("VP8DecInit# ERROR: Tiled reference picture format not supported in HW\n");
            return VP8DEC_FORMAT_NOT_SUPPORTED;
        }
        pDecCont->tiledModeSupport = config.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;
    pDecCont->intraFreeze = useVideoFreezeConcealment;
    pDecCont->pictureBroken = 0;
    pDecCont->decoder.refbuPredHits = 0;

    /* return new instance to application */
    *pDecInst = (VP8DecInst) pDecCont;

    DEC_API_TRC("VP8DecInit# OK\n");
    return (VP8DEC_OK);

}

/*------------------------------------------------------------------------------
    Function name   : VP8DecRelease
    Description     : 
    Return type     : void 
    Argument        : VP8DecInst decInst
------------------------------------------------------------------------------*/
void VP8DecRelease(VP8DecInst decInst)
{

    VP8DecContainer_t *pDecCont = (VP8DecContainer_t *) decInst;
    const void *dwl;

    DEC_API_TRC("VP8DecRelease#\n");

    if(pDecCont == NULL)
    {
        DEC_API_TRC("VP8DecRelease# ERROR: decInst == NULL\n");
        return;
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP8DecRelease# ERROR: Decoder not initialized\n");
        return;
    }

    /* PP instance must be already disconnected at this point */
    ASSERT(pDecCont->pp.ppInstance == NULL);

    dwl = pDecCont->dwl;

    if(pDecCont->asicRunning)
    {
        DWLDisableHW(dwl, 1 * 4, 0);    /* stop HW */
        G1G1DWLReleaseHw(dwl);  /* release HW lock */
        pDecCont->asicRunning = 0;
    }

    VP8HwdAsicReleaseMem(pDecCont);
    VP8HwdAsicReleasePictures(pDecCont);

    pDecCont->checksum = NULL;
    G1DWLfree(pDecCont);

    {
        i32 dwlret = G1DWLRelease(dwl);

        ASSERT(dwlret == DWL_OK);
        (void) dwlret;
    }

    DEC_API_TRC("VP8DecRelease# OK\n");

    return;
}

/*------------------------------------------------------------------------------
    Function name   : VP8DecGetInfo
    Description     : 
    Return type     : VP8DecRet 
    Argument        : VP8DecInst decInst
    Argument        : VP8DecInfo * pDecInfo
------------------------------------------------------------------------------*/
VP8DecRet VP8DecGetInfo(VP8DecInst decInst, VP8DecInfo * pDecInfo)
{
    const VP8DecContainer_t *pDecCont = (VP8DecContainer_t *) decInst;

    DEC_API_TRC("VP8DecGetInfo#");

    if(decInst == NULL || pDecInfo == NULL)
    {
        DEC_API_TRC("VP8DecGetInfo# ERROR: decInst or pDecInfo is NULL\n");
        return VP8DEC_PARAM_ERROR;
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP8DecGetInfo# ERROR: Decoder not initialized\n");
        return VP8DEC_NOT_INITIALIZED;
    }

    if (pDecCont->decStat == VP8DEC_INITIALIZED)
    {
        return VP8DEC_HDRS_NOT_RDY;
    }

    pDecInfo->vpVersion = pDecCont->decoder.vpVersion;
    pDecInfo->vpProfile = pDecCont->decoder.vpProfile;

	// +Leo@2011/10/25: here should be tiledModeSupport.
#if 0	
    if(pDecCont->tiledReferenceEnable)
#else
	if(pDecCont->tiledModeSupport)
#endif
	// -Leo@2011/10/25
    {
        pDecInfo->outputFormat = VP8DEC_TILED_YUV420;
    }
    else
    {
        pDecInfo->outputFormat = VP8DEC_SEMIPLANAR_YUV420;
    }

    /* Fragments have 8 pixels */
    pDecInfo->codedWidth = pDecCont->decoder.width;
    pDecInfo->codedHeight = pDecCont->decoder.height;
    pDecInfo->frameWidth = (pDecCont->decoder.width + 15) & ~15;
    pDecInfo->frameHeight = (pDecCont->decoder.height + 15) & ~15;
    pDecInfo->scaledWidth = pDecCont->decoder.scaledWidth;
    pDecInfo->scaledHeight = pDecCont->decoder.scaledHeight;

    return VP8DEC_OK;
}

/*------------------------------------------------------------------------------
    Function name   : VP8DecDecode
    Description     : 
    Return type     : VP8DecRet 
    Argument        : VP8DecInst decInst
    Argument        : const VP8DecInput * pInput
    Argument        : VP8DecFrame * pOutput
------------------------------------------------------------------------------*/
VP8DecRet VP8DecDecode(VP8DecInst decInst,
                       const VP8DecInput * pInput, VP8DecOutput * pOutput)
{
    VP8DecContainer_t *pDecCont = (VP8DecContainer_t *) decInst;
    DecAsicBuffers_t *pAsicBuff;
    i32 ret;
    u32 asic_status;
    u32 errorConcealment = 0;

    DEC_API_TRC("VP8DecDecode#\n");

    /* Check that function input parameters are valid */
    if(pInput == NULL || pOutput == NULL || decInst == NULL)
    {
        DEC_API_TRC("VP8DecDecode# ERROR: NULL arg(s)\n");
        return (VP8DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP8DecDecode# ERROR: Decoder not initialized\n");
        return (VP8DEC_NOT_INITIALIZED);
    }

    if(pInput->dataLen == 0 ||
       pInput->dataLen > DEC_X170_MAX_STREAM ||
       X170_CHECK_VIRTUAL_ADDRESS(pInput->pStream) ||
       X170_CHECK_BUS_ADDRESS(pInput->streamBusAddress))
    {
        DEC_API_TRC("VP8DecDecode# ERROR: Invalid arg value\n");
        return VP8DEC_PARAM_ERROR;
    }

    if ((pInput->pPicBufferY != NULL && pInput->picBufferBusAddressY == 0) ||
        (pInput->pPicBufferY == NULL && pInput->picBufferBusAddressY != 0) ||
        (pInput->pPicBufferC != NULL && pInput->picBufferBusAddressC == 0) ||
        (pInput->pPicBufferC == NULL && pInput->picBufferBusAddressC != 0) ||
        (pInput->pPicBufferY == NULL && pInput->pPicBufferC != 0) ||
        (pInput->pPicBufferY != NULL && pInput->pPicBufferC == 0))
    {
        DEC_API_TRC("VP8DecDecode# ERROR: Invalid arg value\n");
        return VP8DEC_PARAM_ERROR;
    }

#ifdef VP8DEC_EVALUATION
    if(pDecCont->picNumber > VP8DEC_EVALUATION)
    {
        DEC_API_TRC("VP8DecDecode# VP8DEC_EVALUATION_LIMIT_EXCEEDED\n");
        return VP8DEC_EVALUATION_LIMIT_EXCEEDED;
    }
#endif

    /* aliases */
    pAsicBuff = pDecCont->asicBuff;

    /* application indicates that slice mode decoding should be used ->
     * disabled unless WebP and PP not used */
    if (pDecCont->decStat != VP8DEC_MIDDLE_OF_PIC &&
        pDecCont->intraOnly && pInput->sliceHeight &&
        pDecCont->pp.ppInstance == NULL)
    {
        pDecCont->sliceHeight = pInput->sliceHeight;
    }

    if (pDecCont->intraOnly && pInput->pPicBufferY)
    {
        pDecCont->userMem = 1;
        pDecCont->asicBuff->userMem.pPicBufferY = pInput->pPicBufferY;
        pDecCont->asicBuff->userMem.picBufferBusAddrY =
            pInput->picBufferBusAddressY;
        pDecCont->asicBuff->userMem.pPicBufferC = pInput->pPicBufferC;
        pDecCont->asicBuff->userMem.picBufferBusAddrC =
            pInput->picBufferBusAddressC;
    }

    if (pDecCont->decStat == VP8DEC_NEW_HEADERS)
    {
        pDecCont->decStat = VP8DEC_DECODING;

        if(VP8HwdAsicAllocatePictures(pDecCont) != 0)
        {
            DEC_API_TRC
                ("VP8DecDecode# ERROR: Picture memory allocation failed\n");
            return VP8DEC_MEMFAIL;
        }
    }
    else if (pDecCont->decStat != VP8DEC_MIDDLE_OF_PIC)
    {
        /* decode frame tag */
        vp8hwdDecodeFrameTag( pInput->pStream, &pDecCont->decoder );

        /* When on key-frame, reset probabilities and such */
        if( pDecCont->decoder.keyFrame )
        {
            vp8hwdResetDecoder( &pDecCont->decoder, pAsicBuff );
        }
        /* intra only and non key-frame */
        else if (pDecCont->intraOnly)
        {
            return VP8DEC_STRM_ERROR;
        }

        if (pDecCont->decoder.keyFrame || pDecCont->decoder.vpVersion > 0)
        {
            pAsicBuff->dcPred[0] = pAsicBuff->dcPred[1] =
            pAsicBuff->dcMatch[0] = pAsicBuff->dcMatch[1] = 0;
        }

        /* Decode frame header (now starts bool coder as well) */
        ret = vp8hwdDecodeFrameHeader( 
            pInput->pStream + pDecCont->decoder.frameTagSize,
            pInput->dataLen - pDecCont->decoder.frameTagSize,
            &pDecCont->bc, &pDecCont->decoder );
        if( ret != HANTRO_OK )
        {
            DEC_API_TRC("VP8DecDecode# ERROR: Frame header decoding failed\n");
            if (!pDecCont->picNumber || pDecCont->decStat != VP8DEC_DECODING)
                return VP8DEC_STRM_ERROR;
            else
            {
                vp8hwdFreeze(pDecCont);
                DEC_API_TRC("VP8DecDecode# VP8DEC_PIC_DECODED\n");
                return VP8DEC_PIC_DECODED;
            }
        }

        ret = vp8hwdSetPartitionOffsets(pInput->pStream, pInput->dataLen,
            &pDecCont->decoder);
        if (ret != HANTRO_OK)
        {
            if (!pDecCont->picNumber || pDecCont->decStat != VP8DEC_DECODING)
                return VP8DEC_STRM_ERROR;
            else
            {
                vp8hwdFreeze(pDecCont);
                DEC_API_TRC("VP8DecDecode# VP8DEC_PIC_DECODED\n");
                return VP8DEC_PIC_DECODED;
            }
        }

        /* check for picture size change */
        if((pDecCont->width != (pDecCont->decoder.width)) ||
           (pDecCont->height != (pDecCont->decoder.height)))
        {

            /* reallocate picture buffers */
            pAsicBuff->width = ( pDecCont->decoder.width + 15 ) & ~15;
            pAsicBuff->height = ( pDecCont->decoder.height + 15 ) & ~15;

            VP8HwdAsicReleasePictures(pDecCont);

            if (vp8hwdCheckSupport(pDecCont) != HANTRO_OK)
            {
                pDecCont->decStat = VP8DEC_INITIALIZED;
                return VP8DEC_STREAM_NOT_SUPPORTED;
            }

            pDecCont->width = pDecCont->decoder.width;
            pDecCont->height = pDecCont->decoder.height;

            pDecCont->decStat = VP8DEC_NEW_HEADERS;

            if( pDecCont->refBufSupport && !pDecCont->intraOnly )
            {
                RefbuInit( &pDecCont->refBufferCtrl, 10, 
                           MB_MULTIPLE(pDecCont->decoder.width)>>4,
                           MB_MULTIPLE(pDecCont->decoder.height)>>4,
                           pDecCont->refBufSupport);
            }

            DEC_API_TRC("VP8DecDecode# VP8DEC_HDRS_RDY\n");

            return VP8DEC_HDRS_RDY;
        }

        /* If we are here and dimensions are still 0, it means that we have 
         * yet to decode a valid keyframe, in which case we must give up. */
        if( pDecCont->width == 0 || pDecCont->height == 0 )
        {
            return VP8DEC_STRM_PROCESSED;
        }

        /* If output picture is broken and we are not decoding a base frame, 
         * don't even start HW, just output same picture again. */
        if( !pDecCont->decoder.keyFrame &&
            pDecCont->pictureBroken &&
            pDecCont->intraFreeze )
        {
            vp8hwdFreeze(pDecCont);
            DEC_API_TRC("VP8DecDecode# VP8DEC_PIC_DECODED\n");
            return VP8DEC_PIC_DECODED;
        }

    }

    if (pDecCont->decStat != VP8DEC_MIDDLE_OF_PIC)
    {
        pDecCont->refToOut = 0;

        /* prepare asic */
        VP8HwdAsicProbUpdate(pDecCont);

        VP8HwdAsicInitPicture(pDecCont);

        VP8HwdAsicStrmPosUpdate(pDecCont, pInput->streamBusAddress);

        /* PP setup stuff */
        vp8hwdPreparePpRun(pDecCont);
    }
    else
        VP8HwdAsicContPicture(pDecCont);

    /* run the hardware */
    asic_status = VP8HwdAsicRun(pDecCont);

    /* Rollback entropy probabilities if refresh is not set */
    if(pDecCont->decoder.refreshEntropyProbs == HANTRO_FALSE)
    {
        G1DWLmemcpy( &pDecCont->decoder.entropy, &pDecCont->decoder.entropyLast, 
                   sizeof(vp8EntropyProbs_t));
        G1DWLmemcpy( pDecCont->decoder.vp7ScanOrder, pDecCont->decoder.vp7PrevScanOrder, 
                   sizeof(pDecCont->decoder.vp7ScanOrder));
    }

    /* Handle system error situations */
    if(asic_status == VP8HWDEC_SYSTEM_TIMEOUT)
    {
        /* This timeout is DWL(software/os) generated */
        DEC_API_TRC("VP8DecDecode# VP8DEC_HW_TIMEOUT, SW generated\n");
        return VP8DEC_HW_TIMEOUT;
    }
    else if(asic_status == VP8HWDEC_SYSTEM_ERROR)
    {
        DEC_API_TRC("VP8DecDecode# VP8HWDEC_SYSTEM_ERROR\n");
        return VP8DEC_SYSTEM_ERROR;
    }
    else if(asic_status == VP8HWDEC_HW_RESERVED)
    {
        DEC_API_TRC("VP8DecDecode# VP8HWDEC_HW_RESERVED\n");
        return VP8DEC_HW_RESERVED;
    }

    /* Handle possible common HW error situations */
    if(asic_status & DEC_8190_IRQ_BUS)
    {
        DEC_API_TRC("VP8DecDecode# VP8DEC_HW_BUS_ERROR\n");
        return VP8DEC_HW_BUS_ERROR;
    }

    /* for all the rest we will output a picture (concealed or not) */
    if((asic_status & DEC_8190_IRQ_TIMEOUT) ||
       (asic_status & DEC_8190_IRQ_ERROR))
    {
        /* This timeout is HW generated */
        if(asic_status & DEC_8190_IRQ_TIMEOUT)
        {
#ifdef VP8HWTIMEOUT_ASSERT
            ASSERT(0);
#endif
            DEBUG_PRINT(("IRQ: HW TIMEOUT\n"));
        }
        else
        {
            DEBUG_PRINT(("IRQ: STREAM ERROR\n"));
        }

        /* PP has to run again for the concealed picture */
        if(pDecCont->pp.ppInstance != NULL && pDecCont->pp.decPpIf.usePipeline)
        {
            /* concealed current, i.e. last  ref to PP */
            TRACE_PP_CTRL
                ("VP8DecDecode: Concealed picture, PP should run again\n");
            pDecCont->pp.decPpIf.ppStatus = DECPP_RUNNING;
        }

        errorConcealment = 1;
    }
    else if(asic_status & DEC_8190_IRQ_RDY)
    {
        DEBUG_PRINT(("IRQ: PICTURE RDY\n"));

        if (pDecCont->decoder.keyFrame)
            pDecCont->pictureBroken = 0;

        if (pDecCont->sliceHeight)
        {
            pDecCont->outputRows = pAsicBuff->height -  pDecCont->totDecodedRows*16;
            if (pDecCont->totDecodedRows)
                pDecCont->outputRows += 8;
            return VP8DEC_PIC_DECODED;
        }

    }
    else if (asic_status & DEC_8190_IRQ_SLICE)
    {
        pDecCont->decStat = VP8DEC_MIDDLE_OF_PIC;

        pDecCont->outputRows = pDecCont->sliceHeight * 16;
        if (!pDecCont->totDecodedRows)
            pDecCont->outputRows -= 8;

        pDecCont->totDecodedRows += pDecCont->sliceHeight;

        return VP8DEC_SLICE_RDY;
    }
    else
    {
        ASSERT(0);
    }

    VP8HwdUpdateRefs(pDecCont, errorConcealment);

    /* find first free buffer and use it as next output */
    if (!errorConcealment || pDecCont->intraOnly)
    {
        i32 i;

        pAsicBuff->prevOutBuffer = pAsicBuff->outBuffer;
        pAsicBuff->outBuffer = NULL;

        pAsicBuff->outBufferI = BqueueNext( &pDecCont->bq, 
                                 pAsicBuff->refBufferI,
                                 pAsicBuff->goldenBufferI,
                                 pAsicBuff->alternateBufferI, 0);
        pAsicBuff->outBuffer = &pAsicBuff->pictures[pAsicBuff->outBufferI];
        ASSERT(pAsicBuff->outBuffer != NULL);
    }
    else
    {
        pDecCont->refToOut = 1;
        pDecCont->pictureBroken = 1;
        BqueueDiscard( &pDecCont->bq,
                       pAsicBuff->outBufferI );
        if (!pDecCont->picNumber)
        {
            (void) G1DWLmemset( pAsicBuff->refBuffer->virtualAddress, 128,
                pAsicBuff->width * pAsicBuff->height * 3 / 2);
        }
    }

    pDecCont->picNumber++;
    if (pDecCont->decoder.showFrame)
        pDecCont->outCount++;

    DEC_API_TRC("VP8DecDecode# VP8DEC_PIC_DECODED\n");
    return VP8DEC_PIC_DECODED;
}

/*------------------------------------------------------------------------------
    Function name   : VP8DecNextPicture
    Description     : 
    Return type     : VP8DecRet 
    Argument        : VP8DecInst decInst
------------------------------------------------------------------------------*/
VP8DecRet VP8DecNextPicture(VP8DecInst decInst,
                            VP8DecPicture * pOutput, u32 endOfStream)
{
    VP8DecContainer_t *pDecCont = (VP8DecContainer_t *) decInst;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;
    u32 picForOutput = 0;

    DEC_API_TRC("VP8DecNextPicture#\n");

    if(decInst == NULL || pOutput == NULL)
    {
        DEC_API_TRC("VP8DecNextPicture# ERROR: decInst or pOutput is NULL\n");
        return (VP8DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP8DecNextPicture# ERROR: Decoder not initialized\n");
        return (VP8DEC_NOT_INITIALIZED);
    }

    if (!pDecCont->outCount && !pDecCont->outputRows)
        return VP8DEC_OK;

    /* slice for output */
    if (pDecCont->outputRows)
    {
        pOutput->numSliceRows = pDecCont->outputRows;

        if (pDecCont->userMem)
        {
            pOutput->pOutputFrame = pAsicBuff->userMem.pPicBufferY;
            pOutput->pOutputFrameC = pAsicBuff->userMem.pPicBufferC;
            pOutput->outputFrameBusAddress = 
                pAsicBuff->userMem.picBufferBusAddrY;
            pOutput->outputFrameBusAddressC =
                pAsicBuff->userMem.picBufferBusAddrC;
        }
        else
        {
            u32 offset = 16 * (pDecCont->sliceHeight + 1) * pAsicBuff->width;

            pOutput->pOutputFrame = pAsicBuff->pictures[0].virtualAddress;
            pOutput->outputFrameBusAddress =
                (u32 )(pAsicBuff->pictures[0].virtualAddress);

		//add by franklin ,for rvds compile
		//pOutput->pOutputFrameC = (u32 )pOutput->pOutputFrame + offset;
            pOutput->pOutputFrameC = (const u32 *)((u32 *)pOutput->pOutputFrame + offset);
            pOutput->outputFrameBusAddressC =
                pOutput->outputFrameBusAddress + offset;
        }

        pOutput->picId = 0;
        pOutput->isIntraFrame = pDecCont->decoder.keyFrame;
        pOutput->isGoldenFrame = 0;
        pOutput->nbrOfErrMBs = 0;

        pOutput->frameWidth = (pDecCont->width + 15) & ~15;
        pOutput->frameHeight = (pDecCont->height + 15) & ~15;
        pOutput->codedWidth = pDecCont->width;
        pOutput->codedHeight = pDecCont->height;
        pOutput->outputFormat = pDecCont->tiledReferenceEnable ?
            DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

        pDecCont->outputRows = 0;

        return (VP8DEC_PIC_RDY);
    }

    pOutput->numSliceRows = 0;

    if (!pDecCont->pp.ppInstance || pDecCont->pp.decPpIf.ppStatus != DECPP_IDLE)
        picForOutput = 1;
    else if (!pDecCont->decoder.refreshLast || endOfStream)
    {
        picForOutput = 1;
        pDecCont->pp.decPpIf.ppStatus = DECPP_RUNNING;
    }
    else if (pDecCont->pp.ppInstance && pDecCont->pendingPicToPp)
    {
        picForOutput = 1;
        pDecCont->pp.decPpIf.ppStatus = DECPP_RUNNING;
    }
        
    /* MIT? JOS intraFreeze ja not pipelined pp? */
    if (pDecCont->pp.ppInstance != NULL &&
        pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING)
    {
        DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;
        TRACE_PP_CTRL("VP8DecNextPicture: PP has to run\n");

        decPpIf->usePipeline = 0;

        decPpIf->inwidth = (pDecCont->width+15) & ~15;
        decPpIf->inheight = (pDecCont->height+15) & ~15;
        decPpIf->croppedW = pDecCont->width;
        decPpIf->croppedH = pDecCont->height;

        /* forward tiled mode */
        decPpIf->tiledInputMode = pDecCont->tiledReferenceEnable;
        decPpIf->progressiveSequence = 1;

        decPpIf->picStruct = DECPP_PIC_FRAME_OR_TOP_FIELD;
        if (pDecCont->userMem)
        {
            decPpIf->inputBusLuma =
                   pAsicBuff->userMem.picBufferBusAddrY;
            decPpIf->inputBusChroma =
                   pAsicBuff->userMem.picBufferBusAddrC;
        }
        else
        {
            if (pDecCont->decoder.refreshLast || pDecCont->refToOut)
                decPpIf->inputBusLuma = pAsicBuff->refBuffer->busAddress;
            else
                decPpIf->inputBusLuma = pAsicBuff->prevOutBuffer->busAddress;

            decPpIf->inputBusChroma = decPpIf->inputBusLuma +
                decPpIf->inwidth * decPpIf->inheight;
        }

        pOutput->outputFrameBusAddress = decPpIf->inputBusLuma;

        decPpIf->littleEndian =
            GetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_ENDIAN);
        decPpIf->wordSwap =
            GetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUTSWAP32_E);

        pDecCont->pp.PPDecStart(pDecCont->pp.ppInstance, decPpIf);

        TRACE_PP_CTRL("VP8DecNextPicture: PP wait to be done\n");

        pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);
        pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;

        TRACE_PP_CTRL("VP8DecNextPicture: PP Finished\n");
    }

    if (picForOutput)
    {
        const DWLLinearMem_t *outPic = NULL;

        /* no pp -> current output (ref if concealed)
         * pipeline -> the same
         * pp stand-alone (non-reference frame) -> current output
         * pp stand-alone  */
        if ( (pDecCont->pp.ppInstance == NULL ||
              pDecCont->pp.decPpIf.usePipeline ||
              (pDecCont->outCount == 1 &&
               !pDecCont->decoder.refreshLast) ) && !pDecCont->refToOut)
            outPic = pAsicBuff->prevOutBuffer;
        else
            outPic = pAsicBuff->refBuffer;

        pDecCont->outCount--;

        if (pDecCont->userMem)
        {
            pOutput->pOutputFrame = pAsicBuff->userMem.pPicBufferY;
            pOutput->outputFrameBusAddress =
                pAsicBuff->userMem.picBufferBusAddrY;
            pOutput->pOutputFrameC = pAsicBuff->userMem.pPicBufferC;
            pOutput->outputFrameBusAddressC =
                pAsicBuff->userMem.picBufferBusAddrC;
        }
        else
        {
            pOutput->pOutputFrame = outPic->virtualAddress;
            pOutput->outputFrameBusAddress = outPic->busAddress;
            pOutput->pOutputFrameC = pOutput->pOutputFrame +
                pAsicBuff->width * pAsicBuff->height / 4;
            pOutput->outputFrameBusAddressC = pOutput->outputFrameBusAddress +
                pAsicBuff->width * pAsicBuff->height;
        }
        pOutput->picId = 0;
        pOutput->isIntraFrame = pDecCont->decoder.keyFrame;
        pOutput->isGoldenFrame = 0;
        pOutput->nbrOfErrMBs = 0;

        pOutput->frameWidth = (pDecCont->width + 15) & ~15;
        pOutput->frameHeight = (pDecCont->height + 15) & ~15;
        pOutput->codedWidth = pDecCont->width;
        pOutput->codedHeight = pDecCont->height;
        pOutput->outputFormat = pDecCont->tiledReferenceEnable ?
            DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

        DEC_API_TRC("VP8DecNextPicture# VP8DEC_PIC_RDY\n");

        pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;

        return (VP8DEC_PIC_RDY);
    }

    DEC_API_TRC("VP8DecNextPicture# VP8DEC_OK\n");
    return (VP8DEC_OK);

}

u32 vp8hwdCheckSupport( VP8DecContainer_t *pDecCont )
{

    DWLHwConfig_t hwConfig;

    DWLReadAsicConfig(&hwConfig);

    if ( (pDecCont->asicBuff->width > hwConfig.maxDecPicWidth) ||
         (pDecCont->asicBuff->width < MIN_PIC_WIDTH) ||
         (pDecCont->asicBuff->height < MIN_PIC_HEIGHT) ||
         (pDecCont->asicBuff->width*pDecCont->asicBuff->height > MAX_PIC_SIZE) )
    {
        /* check if webp support */
        if (pDecCont->intraOnly && hwConfig.webpSupport &&
            pDecCont->asicBuff->width <= MAX_SNAPSHOT_WIDTH*16 &&
            pDecCont->asicBuff->height <= MAX_SNAPSHOT_HEIGHT*16)
        {
            return HANTRO_OK;
        }
        else 
            return HANTRO_NOK;
    }

    return HANTRO_OK;
}

/*------------------------------------------------------------------------------
    Function name   : VP8DecPeek
    Description     : 
    Return type     : VP8DecRet 
    Argument        : VP8DecInst decInst
------------------------------------------------------------------------------*/
VP8DecRet VP8DecPeek(VP8DecInst decInst, VP8DecPicture * pOutput)
{
    VP8DecContainer_t *pDecCont = (VP8DecContainer_t *) decInst;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;
    const DWLLinearMem_t *outPic = NULL;

    DEC_API_TRC("VP8DecPeek#\n");

    if(decInst == NULL || pOutput == NULL)
    {
        DEC_API_TRC("VP8DecPeek# ERROR: decInst or pOutput is NULL\n");
        return (VP8DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP8DecPeek# ERROR: Decoder not initialized\n");
        return (VP8DEC_NOT_INITIALIZED);
    }

    if (!pDecCont->outCount)
    {
        G1DWLmemset(pOutput, 0, sizeof(VP8DecPicture));
        return VP8DEC_OK;
    }

    outPic = pAsicBuff->prevOutBuffer;

    pOutput->pOutputFrame = outPic->virtualAddress;
    pOutput->outputFrameBusAddress = outPic->busAddress;
    pOutput->picId = 0;
    pOutput->isIntraFrame = pDecCont->decoder.keyFrame;
    pOutput->isGoldenFrame = 0;
    pOutput->nbrOfErrMBs = 0;

    pOutput->frameWidth = (pDecCont->width + 15) & ~15;
    pOutput->frameHeight = (pDecCont->height + 15) & ~15;
    pOutput->codedWidth = pDecCont->width;
    pOutput->codedHeight = pDecCont->height;

    return (VP8DEC_PIC_RDY);


}

void vp8hwdFreeze(VP8DecContainer_t *pDecCont)
{

    /* Skip */
    pDecCont->picNumber++;
    pDecCont->refToOut = 1;
    if (pDecCont->decoder.showFrame) 
        pDecCont->outCount++;

    if(pDecCont->pp.ppInstance != NULL)
    {
        /* last ref to PP */
        pDecCont->pp.decPpIf.usePipeline = 0;
        pDecCont->pp.decPpIf.ppStatus = DECPP_RUNNING;
    }

}

/*------------------------------------------------------------------------------

    Function name: VP8DecSetLatency

    Functional description:
        set current video latencyMS to vdec driver for vpu dvfs

    Input:
        decInst     Reference to decoder instance.
        latencyMS   


    Return values:
        VP8EC_OK;

------------------------------------------------------------------------------*/
VP8DecRet VP8DecSetLatency(VP8DecInst decInst, int latencyMS)
{
    VP8DecContainer_t *pDecCont = (VP8DecContainer_t *) decInst;

    DWLSetLatency(pDecCont->dwl, latencyMS);

    return VP8DEC_OK;
}
