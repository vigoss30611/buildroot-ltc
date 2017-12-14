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
-  $RCSfile: vp6hwd_api.c,v $
-  $Revision: 1.212 $
-  $Date: 2011/02/04 12:41:08 $
-
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "decapicommon.h"
#include "vp6decapi.h"

#include "dwl.h"
#include "vp6hwd_container.h"
#include "vp6hwd_debug.h"
#include "vp6hwd_asic.h"
#include "refbuffer.h"
#include "bqueue.h"
#include "tiledref.h"

#define VP6DEC_MAJOR_VERSION 1
#define VP6DEC_MINOR_VERSION 0

#define VP6DEC_BUILD_MAJOR 0
#define VP6DEC_BUILD_MINOR 189
#define VP6DEC_SW_BUILD ((VP6DEC_BUILD_MAJOR * 1000) + VP6DEC_BUILD_MINOR)

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#include <stdio.h>
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

#ifdef VP6DEC_TRACE
#define DEC_API_TRC(str)    VP6DecTrace(str)
#else
#define DEC_API_TRC(str)    do{}while(0)
#endif

extern void vp6PreparePpRun(VP6DecContainer_t *pDecCont);

/*------------------------------------------------------------------------------
    Function name : VP6DecGetAPIVersion
    Description   : Return the API version information

    Return type   : VP6DecApiVersion
    Argument      : void
------------------------------------------------------------------------------*/
VP6DecApiVersion VP6DecGetAPIVersion(void)
{
    VP6DecApiVersion ver;

    ver.major = VP6DEC_MAJOR_VERSION;
    ver.minor = VP6DEC_MINOR_VERSION;

    DEC_API_TRC("VP6DecGetAPIVersion# OK\n");

    return ver;
}

/*------------------------------------------------------------------------------
    Function name : VP6DecGetBuild
    Description   : Return the SW and HW build information

    Return type   : VP6DecBuild
    Argument      : void
------------------------------------------------------------------------------*/
VP6DecBuild VP6DecGetBuild(void)
{
    VP6DecBuild ver;
    DWLHwConfig_t hwCfg;

    (void)G1DWLmemset(&ver, 0, sizeof(ver));

    ver.swBuild = VP6DEC_SW_BUILD;
    ver.hwBuild = G1DWLReadAsicID();

    DWLReadAsicConfig(&hwCfg);

    SET_DEC_BUILD_SUPPORT(ver.hwConfig, hwCfg);

    DEC_API_TRC("VP6DecGetBuild# OK\n");

    return (ver);
}

/*------------------------------------------------------------------------------
    Function name   : vp6decinit
    Description     : 
    Return type     : VP6DecRet 
    Argument        : VP6DecInst * pDecInst
                      u32 useVideoFreezeConcealment
------------------------------------------------------------------------------*/
VP6DecRet VP6DecInit(VP6DecInst * pDecInst, 
                     u32 useVideoFreezeConcealment,
                     u32 numFrameBuffers,
                     DecRefFrmFormat referenceFrameFormat, 
                     u32 mmuEnable)
{
    VP6DecRet ret;
    VP6DecContainer_t *pDecCont;
    const void *dwl;
    u32 i;

    G1DWLInitParam_t dwlInit;
    DWLHwConfig_t config;

    DEC_API_TRC("VP6DecInit#\n");

    /* check that right shift on negative numbers is performed signed */
    /*lint -save -e* following check causes multiple lint messages */
#if (((-1) >> 1) != (-1))
#error Right bit-shifting (>>) does not preserve the sign
#endif
    /*lint -restore */

    if(pDecInst == NULL)
    {
        DEC_API_TRC("VP6DecInit# ERROR: pDecInst == NULL");
        return (VP6DEC_PARAM_ERROR);
    }

    *pDecInst = NULL;   /* return NULL instance for any error */

    /* check that VP6 decoding supported in HW */
    {

        DWLHwConfig_t hwCfg;

        DWLReadAsicConfig(&hwCfg);
        if(!hwCfg.vp6Support)
        {
            DEC_API_TRC("VP6DecInit# ERROR: VP6 not supported in HW\n");
            return VP6DEC_FORMAT_NOT_SUPPORTED;
        }
    }

    /* init DWL for the specified client */
    dwlInit.clientType = DWL_CLIENT_TYPE_VP6_DEC;
    dwlInit.mmuEnable = mmuEnable;

    dwl = G1DWLInit(&dwlInit);

    if(dwl == NULL)
    {
        DEC_API_TRC("VP6DecInit# ERROR: DWL Init failed\n");
        return (VP6DEC_DWL_ERROR);
    }

    /* allocate instance */
    pDecCont = (VP6DecContainer_t *) G1DWLmalloc(sizeof(VP6DecContainer_t));

    if(pDecCont == NULL)
    {
        DEC_API_TRC("VP6DecInit# ERROR: Memory allocation failed\n");
        ret = VP6DEC_MEMFAIL;
        goto err;
    }

    (void) G1DWLmemset(pDecCont, 0, sizeof(VP6DecContainer_t));
    pDecCont->dwl = dwl;

    /* initial setup of instance */

    pDecCont->decStat = VP6DEC_INITIALIZED;
    pDecCont->checksum = pDecCont;  /* save instance as a checksum */
    if(numFrameBuffers > 16)    numFrameBuffers = 16;
    if(numFrameBuffers < 3)     numFrameBuffers = 3;
    pDecCont->numBuffers = numFrameBuffers;

    VP6HwdAsicInit(pDecCont);   /* Init ASIC */

    if(VP6HwdAsicAllocateMem(pDecCont) != 0)
    {
        DEC_API_TRC("VP6DecInit# ERROR: ASIC Memory allocation failed\n");
        ret = VP6DEC_MEMFAIL;
        goto err;
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
            return VP6DEC_FORMAT_NOT_SUPPORTED;
        }
        pDecCont->tiledModeSupport = config.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;

    pDecCont->intraFreeze = useVideoFreezeConcealment;
    pDecCont->pictureBroken = 0;

    pDecCont->decStat = VP6DEC_INITIALIZED;

    /* return new instance to application */
    *pDecInst = (VP6DecInst) pDecCont;

    DEC_API_TRC("VP6DecInit# OK\n");
    return (VP6DEC_OK);

  err:
    if(pDecCont != NULL)
        G1DWLfree(pDecCont);

    if(dwl != NULL)
    {
        i32 dwlret = G1DWLRelease(dwl);

        ASSERT(dwlret == DWL_OK);
        (void) dwlret;
    }
    *pDecInst = NULL;
    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : VP6DecRelease
    Description     : 
    Return type     : void 
    Argument        : VP6DecInst decInst
------------------------------------------------------------------------------*/
void VP6DecRelease(VP6DecInst decInst)
{

    VP6DecContainer_t *pDecCont = (VP6DecContainer_t *) decInst;
    const void *dwl;

    DEC_API_TRC("VP6DecRelease#\n");

    if(pDecCont == NULL)
    {
        DEC_API_TRC("VP6DecRelease# ERROR: decInst == NULL\n");
        return;
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP6DecRelease# ERROR: Decoder not initialized\n");
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

    VP6HwdAsicReleaseMem(pDecCont);
    VP6HwdAsicReleasePictures(pDecCont);
    VP6HWDeleteHuffman(&pDecCont->pb);

    pDecCont->checksum = NULL;
    G1DWLfree(pDecCont);

    {
        i32 dwlret = G1DWLRelease(dwl);

        ASSERT(dwlret == DWL_OK);
        (void) dwlret;
    }

    DEC_API_TRC("VP6DecRelease# OK\n");

    return;
}

/*------------------------------------------------------------------------------
    Function name   : VP6DecGetInfo
    Description     : 
    Return type     : VP6DecRet 
    Argument        : VP6DecInst decInst
    Argument        : VP6DecInfo * pDecInfo
------------------------------------------------------------------------------*/
VP6DecRet VP6DecGetInfo(VP6DecInst decInst, VP6DecInfo * pDecInfo)
{
    const VP6DecContainer_t *pDecCont = (VP6DecContainer_t *) decInst;

    DEC_API_TRC("VP6DecGetInfo#");

    if(decInst == NULL || pDecInfo == NULL)
    {
        DEC_API_TRC("VP6DecGetInfo# ERROR: decInst or pDecInfo is NULL\n");
        return VP6DEC_PARAM_ERROR;
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP6DecGetInfo# ERROR: Decoder not initialized\n");
        return VP6DEC_NOT_INITIALIZED;
    }

    pDecInfo->vp6Version = pDecCont->pb.Vp3VersionNo;
    pDecInfo->vp6Profile = pDecCont->pb.VpProfile;

	// +Leo@2011/10/25: here should be tiledModeSupport.
#if 0	
    if(pDecCont->tiledReferenceEnable)
#else
	if(pDecCont->tiledModeSupport)
#endif
	// -Leo@2011/10/25
    {
        pDecInfo->outputFormat = VP6DEC_TILED_YUV420;
    }
    else
    {
        pDecInfo->outputFormat = VP6DEC_SEMIPLANAR_YUV420;
    }

    /* Fragments have 8 pixels */
    pDecInfo->frameWidth = pDecCont->pb.HFragments * 8;
    pDecInfo->frameHeight = pDecCont->pb.VFragments * 8;
    pDecInfo->scaledWidth = pDecCont->pb.OutputWidth * 8;
    pDecInfo->scaledHeight = pDecCont->pb.OutputHeight * 8;

    pDecInfo->scalingMode = pDecCont->pb.ScalingMode;

    return VP6DEC_OK;
}

/*------------------------------------------------------------------------------
    Function name   : VP6DecDecode
    Description     : 
    Return type     : VP6DecRet 
    Argument        : VP6DecInst decInst
    Argument        : const VP6DecInput * pInput
    Argument        : VP6DecFrame * pOutput
------------------------------------------------------------------------------*/
VP6DecRet VP6DecDecode(VP6DecInst decInst,
                       const VP6DecInput * pInput, VP6DecOutput * pOutput)
{
    VP6DecContainer_t *pDecCont = (VP6DecContainer_t *) decInst;
    DecAsicBuffers_t *pAsicBuff;
    i32 ret;
    u32 asic_status;
    u32 errorConcealment = 0;

    DEC_API_TRC("VP6DecDecode#\n");

    /* Check that function input parameters are valid */
    if(pInput == NULL || pOutput == NULL || decInst == NULL)
    {
        DEC_API_TRC("VP6DecDecode# ERROR: NULL arg(s)\n");
        return (VP6DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP6DecDecode# ERROR: Decoder not initialized\n");
        return (VP6DEC_NOT_INITIALIZED);
    }

    if(pInput->dataLen == 0 ||
       pInput->dataLen > DEC_X170_MAX_STREAM ||
       X170_CHECK_VIRTUAL_ADDRESS(pInput->pStream) ||
       X170_CHECK_BUS_ADDRESS(pInput->streamBusAddress))
    {
        DEC_API_TRC("VP6DecDecode# ERROR: Invalid arg value\n");
        return VP6DEC_PARAM_ERROR;
    }

#ifdef VP6DEC_EVALUATION
    if(pDecCont->picNumber > VP6DEC_EVALUATION)
    {
        DEC_API_TRC("VP6DecDecode# VP6DEC_EVALUATION_LIMIT_EXCEEDED\n");
        return VP6DEC_EVALUATION_LIMIT_EXCEEDED;
    }
#endif

    /* aliases */
    pAsicBuff = pDecCont->asicBuff;

    if(pDecCont->decStat == VP6DEC_NEW_HEADERS)
    {
        /* we stopped the decoding after noticing new picture size */
        /* continue from where we left */
        pDecCont->decStat = VP6DEC_INITIALIZED;
        goto continue_pic_decode;
    }

    Vp6StrmInit(&pDecCont->pb.strm, pInput->pStream, pInput->dataLen);

    /* update strm base addresses to ASIC */
    pAsicBuff->partition1Base = pInput->streamBusAddress;
    pAsicBuff->partition2Base = pInput->streamBusAddress;

    /* decode frame headers */
    ret = VP6HWLoadFrameHeader(&pDecCont->pb);

    if(ret)
    {
        DEC_API_TRC("VP6DecDecode# ERROR: Frame header decoding failed\n");
        return VP6DEC_STRM_ERROR;
    }

    if (pDecCont->pb.br.strmError)
    {
        if (!pDecCont->picNumber)
            return VP6DEC_STRM_ERROR;
        else
            goto freeze;
    }
   
    /* check for picture size change */
    if((pDecCont->width != (pDecCont->pb.HFragments * 8)) ||
       (pDecCont->height != (pDecCont->pb.VFragments * 8)))
    {
        /* reallocate picture buffers */
        pAsicBuff->width = pDecCont->pb.HFragments * 8;
        pAsicBuff->height = pDecCont->pb.VFragments * 8;

        VP6HwdAsicReleasePictures(pDecCont);
        if(VP6HwdAsicAllocatePictures(pDecCont) != 0)
        {
            DEC_API_TRC
                ("VP6DecDecode# ERROR: Picture memory allocation failed\n");
            return VP6DEC_MEMFAIL;
        }

        pDecCont->width = pDecCont->pb.HFragments * 8;
        pDecCont->height = pDecCont->pb.VFragments * 8;

        pDecCont->decStat = VP6DEC_NEW_HEADERS;

        if( pDecCont->refBufSupport )
        {
            RefbuInit( &pDecCont->refBufferCtrl, 7 /* dec_mode_vp6 */, 
                       pDecCont->pb.HFragments / 2,
                       pDecCont->pb.VFragments / 2,
                       pDecCont->refBufSupport);
        }

        DEC_API_TRC("VP6DecDecode# VP6DEC_HDRS_RDY\n");

        return VP6DEC_HDRS_RDY;
    }

  continue_pic_decode:

    /* If output picture is broken and we are not decoding a base frame, 
     * don't even start HW, just output same picture again. */
    if( pDecCont->pb.FrameType != BASE_FRAME &&
        pDecCont->pictureBroken &&
        pDecCont->intraFreeze) 
    {

freeze:

        /* Skip */
        pDecCont->picNumber++;
        pDecCont->refToOut = 1;
        pDecCont->outCount++;

        DEC_API_TRC("VP6DecDecode# VP6DEC_PIC_DECODED\n");

        if(pDecCont->pp.ppInstance != NULL)
        {
            vp6PreparePpRun(pDecCont);
            pDecCont->pp.decPpIf.usePipeline = 0;
        }

        return VP6DEC_PIC_DECODED;
    }
    else
    {
        pDecCont->refToOut = 0;
    }

    /* decode probability updates */
    ret = VP6HWDecodeProbUpdates(&pDecCont->pb);
    if(ret)
    {
        DEC_API_TRC
            ("VP6DecDecode# ERROR: Priobability updates decoding failed\n");
        return VP6DEC_STRM_ERROR;
    }
    if (pDecCont->pb.br.strmError)
    {
        if (!pDecCont->picNumber)
            return VP6DEC_STRM_ERROR;
        else
            goto freeze;
    }

    /* prepare asic */
    VP6HwdAsicProbUpdate(pDecCont);

    VP6HwdAsicInitPicture(pDecCont);

    VP6HwdAsicStrmPosUpdate(pDecCont);

    /* PP setup stuff */
    vp6PreparePpRun(pDecCont);

    /* run the hardware */
    asic_status = VP6HwdAsicRun(pDecCont);

    /* Handle system error situations */
    if(asic_status == VP6HWDEC_SYSTEM_TIMEOUT)
    {
        /* This timeout is DWL(software/os) generated */
        DEC_API_TRC("VP6DecDecode# VP6DEC_HW_TIMEOUT, SW generated\n");
        return VP6DEC_HW_TIMEOUT;
    }
    else if(asic_status == VP6HWDEC_SYSTEM_ERROR)
    {
        DEC_API_TRC("VP6DecDecode# VP6HWDEC_SYSTEM_ERROR\n");
        return VP6DEC_SYSTEM_ERROR;
    }
    else if(asic_status == VP6HWDEC_HW_RESERVED)
    {
        DEC_API_TRC("VP6DecDecode# VP6HWDEC_HW_RESERVED\n");
        return VP6DEC_HW_RESERVED;
    }

    /* Handle possible common HW error situations */
    if(asic_status & DEC_8190_IRQ_BUS)
    {
        DEC_API_TRC("VP6DecDecode# VP6DEC_HW_BUS_ERROR\n");
        return VP6DEC_HW_BUS_ERROR;
    }

    /* for all the rest we will output a picture (concealed or not) */
    if((asic_status & DEC_8190_IRQ_TIMEOUT) ||
       (asic_status & DEC_8190_IRQ_ERROR))
    {
        /* This timeout is HW generated */
        if(asic_status & DEC_8190_IRQ_TIMEOUT)
        {
#ifdef VP6HWTIMEOUT_ASSERT
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
            TRACE_PP_CTRL
                ("VP6DecDecode: Concealed picture, PP should run again\n");
            pDecCont->pp.decPpIf.ppStatus = DECPP_RUNNING;
        }
        pDecCont->outCount++;

        errorConcealment = 1;
    }
    else if(asic_status & DEC_8190_IRQ_RDY)
    {
        DEBUG_PRINT(("IRQ: PICTURE RDY\n"));

        if(pDecCont->pb.FrameType == BASE_FRAME)
        {
            pAsicBuff->refBuffer = pAsicBuff->outBuffer;
            pAsicBuff->goldenBuffer = pAsicBuff->outBuffer;

            pAsicBuff->refBufferI   = pAsicBuff->outBufferI;
            pAsicBuff->goldenBufferI = pAsicBuff->outBufferI;

            pDecCont->pictureBroken = 0;

        }
        else if(pDecCont->pb.RefreshGoldenFrame)
        {
            pAsicBuff->refBuffer = pAsicBuff->outBuffer;
            pAsicBuff->refBufferI   = pAsicBuff->outBufferI;
            pAsicBuff->goldenBuffer = pAsicBuff->outBuffer;
            pAsicBuff->goldenBufferI = pAsicBuff->outBufferI;
        }
        else
        {
            pAsicBuff->refBuffer = pAsicBuff->outBuffer;
            pAsicBuff->refBufferI   = pAsicBuff->outBufferI;
        }
        pDecCont->outCount++;
    }
    else
    {
        ASSERT(0);
    }

    /* find first free buffer and use it as next output */
    {
        i32 i;

        pAsicBuff->prevOutBuffer = pAsicBuff->outBuffer;
        pAsicBuff->outBuffer = NULL;

        pAsicBuff->outBufferI = BqueueNext( &pDecCont->bq, 
                                 pAsicBuff->refBufferI,
                                 pAsicBuff->goldenBufferI,
                                 BQUEUE_UNUSED, 0);
        pAsicBuff->outBuffer = &pAsicBuff->pictures[pAsicBuff->outBufferI];
        ASSERT(pAsicBuff->outBuffer != NULL);
    }

    if( errorConcealment )
    {
        pDecCont->refToOut = 1;
        pDecCont->pictureBroken = 1;
        BqueueDiscard(&pDecCont->bq, pAsicBuff->outBufferI);
        if (!pDecCont->picNumber)
        {
            (void) G1DWLmemset( pAsicBuff->refBuffer->virtualAddress, 128,
                pAsicBuff->width * pAsicBuff->height * 3 / 2);
        }
    }

    pDecCont->picNumber++;


    DEC_API_TRC("VP6DecDecode# VP6DEC_PIC_DECODED\n");
    return VP6DEC_PIC_DECODED;
}

/*------------------------------------------------------------------------------
    Function name   : VP6DecNextPicture
    Description     : 
    Return type     : VP6DecRet 
    Argument        : VP6DecInst decInst
------------------------------------------------------------------------------*/
VP6DecRet VP6DecNextPicture(VP6DecInst decInst,
                            VP6DecPicture * pOutput, u32 endOfStream)
{
    VP6DecContainer_t *pDecCont = (VP6DecContainer_t *) decInst;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;
    u32 picForOutput = 0;

    DEC_API_TRC("VP6DecNextPicture#\n");

    if(decInst == NULL || pOutput == NULL)
    {
        DEC_API_TRC("VP6DecNextPicture# ERROR: decInst or pOutput is NULL\n");
        return (VP6DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP6DecNextPicture# ERROR: Decoder not initialized\n");
        return (VP6DEC_NOT_INITIALIZED);
    }

    if (!pDecCont->pp.ppInstance)
        picForOutput = pDecCont->outCount != 0;
    else if (pDecCont->pp.decPpIf.ppStatus != DECPP_IDLE)
        picForOutput = 1;
    else if (endOfStream && pDecCont->outCount)
    {
        picForOutput = 1;
        pDecCont->pp.decPpIf.ppStatus = DECPP_RUNNING;
    }
        
    /* MIT? JOS intraFreeze ja not pipelined pp? */
    if (pDecCont->pp.ppInstance != NULL &&
        pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING)
    {
        DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;
        TRACE_PP_CTRL("VP6DecNextPicture: PP has to run\n");

        decPpIf->usePipeline = 0;

        decPpIf->inwidth = pDecCont->width;
        decPpIf->inheight = pDecCont->height;
        decPpIf->croppedW = pDecCont->width;
        decPpIf->croppedH = pDecCont->height;
        decPpIf->tiledInputMode = pDecCont->tiledReferenceEnable;
        decPpIf->progressiveSequence = 1;

        decPpIf->picStruct = DECPP_PIC_FRAME_OR_TOP_FIELD;
        decPpIf->inputBusLuma = pAsicBuff->refBuffer->busAddress;
        pOutput->outputFrameBusAddress = pAsicBuff->refBuffer->busAddress;
        decPpIf->inputBusChroma = decPpIf->inputBusLuma +
            decPpIf->inwidth * decPpIf->inheight;

        decPpIf->littleEndian =
            GetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUT_ENDIAN);
        decPpIf->wordSwap =
            GetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUTSWAP32_E);

        pDecCont->pp.PPDecStart(pDecCont->pp.ppInstance, decPpIf);

        TRACE_PP_CTRL("VP6DecNextPicture: PP wait to be done\n");

        pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);
        pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;

        TRACE_PP_CTRL("VP6DecNextPicture: PP Finished\n");
    }

    if (picForOutput)
    {
        const DWLLinearMem_t *outPic = NULL;

        /* HANSKAA output, outPic jos !PP tai pipeline ja !refToOut, muutoin
         * refPic */

        if ( (pDecCont->pp.ppInstance == NULL ||
              pDecCont->pp.decPpIf.usePipeline) && !pDecCont->refToOut)
            outPic = pAsicBuff->prevOutBuffer;
        else
            outPic = pAsicBuff->refBuffer;

        pDecCont->outCount--;

        pOutput->pOutputFrame = outPic->virtualAddress;
        pOutput->outputFrameBusAddress = outPic->busAddress;
        pOutput->picId = 0;
        pOutput->isIntraFrame = 0;
        pOutput->isGoldenFrame = 0;
        pOutput->nbrOfErrMBs = 0;
        pOutput->outputFormat = pDecCont->tiledReferenceEnable ?
            DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

        pOutput->frameWidth = pDecCont->width;
        pOutput->frameHeight = pDecCont->height;

        DEC_API_TRC("VP6DecNextPicture# VP6DEC_PIC_RDY\n");

        pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;

        return (VP6DEC_PIC_RDY);
    }

    DEC_API_TRC("VP6DecNextPicture# VP6DEC_OK\n");
    return (VP6DEC_OK);

}

/*------------------------------------------------------------------------------
    Function name   : VP6DecPeek
    Description     : 
    Return type     : VP6DecRet 
    Argument        : VP6DecInst decInst
------------------------------------------------------------------------------*/
VP6DecRet VP6DecPeek(VP6DecInst decInst, VP6DecPicture * pOutput)
{
    VP6DecContainer_t *pDecCont = (VP6DecContainer_t *) decInst;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;
    const DWLLinearMem_t *outPic = NULL;

    DEC_API_TRC("VP6DecPeek#\n");

    if(decInst == NULL || pOutput == NULL)
    {
        DEC_API_TRC("VP6DecPeek# ERROR: decInst or pOutput is NULL\n");
        return (VP6DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("VP6DecPeek# ERROR: Decoder not initialized\n");
        return (VP6DEC_NOT_INITIALIZED);
    }

    if (pDecCont->outCount == 0)
    {
        (void)G1DWLmemset(pOutput, 0, sizeof(VP6DecPicture));
        return VP6DEC_OK;
    }

    outPic = pAsicBuff->prevOutBuffer;

    pOutput->pOutputFrame = outPic->virtualAddress;
    pOutput->outputFrameBusAddress = outPic->busAddress;
    pOutput->picId = 0;
    pOutput->isIntraFrame = 0;
    pOutput->isGoldenFrame = 0;
    pOutput->nbrOfErrMBs = 0;

    pOutput->frameWidth = pDecCont->width;
    pOutput->frameHeight = pDecCont->height;

    DEC_API_TRC("VP6DecPeek# VP6DEC_PIC_RDY\n");

    return (VP6DEC_PIC_RDY);

}

/*------------------------------------------------------------------------------

    Function name: VP6DecSetLatency

    Functional description:
        set current video latencyMS to vdec driver for vpu dvfs

    Input:
        decInst     Reference to decoder instance.
        latencyMS   


    Return values:
        VP6DEC_OK;

------------------------------------------------------------------------------*/
VP6DecRet VP6DecSetLatency(VP6DecInst decInst, int latencyMS)
{
    VP6DecContainer_t *pDecCont = (VP6DecContainer_t *) decInst;

    DWLSetLatency(pDecCont->dwl, latencyMS);

    return VP6DEC_OK;
}
