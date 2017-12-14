/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2007 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : The API module C-functions of RV Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rvdecapi.c,v $
--  $Date: 2011/02/04 12:42:10 $
--  $Revision: 1.120 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "rv_container.h"
#include "rv_utils.h"
#include "rv_strm.h"
#include "rvdecapi.h"
#include "rvdecapi_internal.h"
#include "dwl.h"
#include "regdrv.h"
#include "rv_headers.h"
#include "deccfg.h"
#include "refbuffer.h"

#include "rv_debug.h"
#include "tiledref.h"

#ifdef RVDEC_TRACE
#define RV_API_TRC(str)    RvDecTrace((str))
#else
#define RV_API_TRC(str)

#endif

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

#define RV_BUFFER_UNDEFINED    16

#define RV_DEC_X170_MODE 8 /* TODO: MIK? ON HUKAN MODE */

#define ID8170_DEC_TIMEOUT        0xFFU
#define ID8170_DEC_SYSTEM_ERROR   0xFEU
#define ID8170_DEC_HW_RESERVED    0xFDU

#define RVDEC_UPDATE_POUTPUT

#define RVDEC_NON_PIPELINE_AND_B_PICTURE \
        ((!pDecCont->ppConfigQuery.pipelineAccepted) \
             && pDecCont->FrameDesc.picCodingType == RV_B_PIC)

static u32 RvCheckFormatSupport(void);
static u32 rvHandleVlcModeError(DecContainer * pDecCont, u32 picNum);
static void rvHandleFrameEnd(DecContainer * pDecCont);
static u32 RunDecoderAsic(DecContainer * pDecContainer, u32 strmBusAddress);
static u32 RvSetRegs(DecContainer * pDecContainer, u32 strmBusAddress);
static void RvPpControl(DecContainer * pDecContainer, u32 pipelineOff);
static void RvFillPicStruct(RvDecPicture * pPicture,
                               DecContainer * pDecCont, u32 picIndex);
static void RvPpMultiBufferInit(DecContainer * pDecCont);
static void RvPpMultiBufferSetup(DecContainer * pDecCont, u32 pipelineOff);
static void RvDecRunFullmode(DecContainer * pDecCont);

/*------------------------------------------------------------------------------
       Version Information - DO NOT CHANGE!
------------------------------------------------------------------------------*/

#define RVDEC_MAJOR_VERSION 0
#define RVDEC_MINOR_VERSION 0

#define RVDEC_BUILD_MAJOR 0
#define RVDEC_BUILD_MINOR 138
#define RVDEC_SW_BUILD ((RVDEC_BUILD_MAJOR * 1000) + RVDEC_BUILD_MINOR)

/*------------------------------------------------------------------------------

    Function: RvDecGetAPIVersion

        Functional description:
            Return version information of API

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

RvDecApiVersion RvDecGetAPIVersion()
{
    RvDecApiVersion ver;

    ver.major = RVDEC_MAJOR_VERSION;
    ver.minor = RVDEC_MINOR_VERSION;

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: RvDecGetBuild

        Functional description:
            Return build information of SW and HW

        Returns:
            RvDecBuild

------------------------------------------------------------------------------*/

RvDecBuild RvDecGetBuild(void)
{
    RvDecBuild ver;
    DWLHwConfig_t hwCfg;

    RV_API_TRC("RvDecGetBuild#");

    G1DWLmemset(&ver, 0, sizeof(ver));

    ver.swBuild = RVDEC_SW_BUILD;
    ver.hwBuild = G1DWLReadAsicID();

    DWLReadAsicConfig(&hwCfg);

    SET_DEC_BUILD_SUPPORT(ver.hwConfig, hwCfg);

    RV_API_TRC("RvDecGetBuild# OK\n");

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: RvDecInit()

        Functional description:
            Initialize decoder software. Function reserves memory for the
            decoder instance.

        Inputs:
            useVideoFreezeConcealment
                            flag to enable error concealment method where
                            decoding starts at next intra picture after error
                            in bitstream.

		Outputs:
            pDecInst        pointer to initialized instance is stored here

        Returns:
            RVDEC_OK       successfully initialized the instance
            RVDEC_MEM_FAIL memory allocation failed

------------------------------------------------------------------------------*/
RvDecRet RvDecInit(RvDecInst * pDecInst, u32 useVideoFreezeConcealment,
    u32 frameCodeLength, u32 *frameSizes, u32 rvVersion,
    u32 maxFrameWidth, u32 maxFrameHeight,
    u32 numFrameBuffers, DecRefFrmFormat referenceFrameFormat,
    u32 mmuEnable)
{
    /*@null@ */ DecContainer *pDecCont;
    /*@null@ */ const void *dwl;
    u32 i = 0;
    u32 ret;

    G1DWLInitParam_t dwlInit;
    DWLHwConfig_t config;

    RV_API_TRC("RvDecInit#");
    RVDEC_API_DEBUG(("RvAPI_DecoderInit#"));

    /* check that right shift on negative numbers is performed signed */
    /*lint -save -e* following check causes multiple lint messages */
    if(((-1) >> 1) != (-1))
    {
        RVDEC_API_DEBUG(("RVDecInit# ERROR: Right shift is not signed"));
        return (RVDEC_INITFAIL);
    }
    /*lint -restore */

    if(pDecInst == NULL)
    {
        RVDEC_API_DEBUG(("RVDecInit# ERROR: pDecInst == NULL"));
        return (RVDEC_PARAM_ERROR);
    }

    *pDecInst = NULL;

    /* check that RV decoding supported in HW */
    if(RvCheckFormatSupport())
    {
        RVDEC_API_DEBUG(("RVDecInit# ERROR: RV not supported in HW\n"));
        return RVDEC_FORMAT_NOT_SUPPORTED;
    }

    dwlInit.clientType = DWL_CLIENT_TYPE_RV_DEC;
    dwlInit.mmuEnable = mmuEnable;

    dwl = G1DWLInit(&dwlInit);

    if(dwl == NULL)
    {
        RVDEC_API_DEBUG(("RVDecInit# ERROR: DWL Init failed"));
        return (RVDEC_DWL_ERROR);
    }

    RVDEC_API_DEBUG(("size of DecContainer %d \n", sizeof(DecContainer)));
    pDecCont = (DecContainer *) G1DWLmalloc(sizeof(DecContainer));

    if(pDecCont == NULL)
    {
        (void) G1DWLRelease(dwl);
        return (RVDEC_MEMFAIL);
    }

    /* set everything initially zero */
    (void) G1DWLmemset(pDecCont, 0, sizeof(DecContainer));

    ret = G1DWLMallocLinear(dwl,
        RV_DEC_X170_MAX_NUM_SLICES*sizeof(u32),
        &pDecCont->StrmStorage.slices);

	G1DWLmemset(pDecCont->StrmStorage.slices.virtualAddress,0,RV_DEC_X170_MAX_NUM_SLICES*sizeof(u32));
    if( ret == HANTRO_NOK )
    {
        G1DWLfree(pDecCont);
        (void) G1DWLRelease(dwl);
        return (RVDEC_MEMFAIL);
    }

    pDecCont->dwl = dwl;

    pDecCont->ApiStorage.DecStat = INITIALIZED;

    *pDecInst = (DecContainer *) pDecCont;

    for(i = 0; i < DEC_X170_REGISTERS; i++)
        pDecCont->rvRegs[i] = 0;

    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_OUT_ENDIAN,
                   DEC_X170_OUTPUT_PICTURE_ENDIAN);
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_IN_ENDIAN,
                   DEC_X170_INPUT_DATA_ENDIAN);
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_STRENDIAN_E,
                   DEC_X170_INPUT_STREAM_ENDIAN);
    /*
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_OUT_TILED_E,
                   DEC_X170_OUTPUT_FORMAT);
                   */
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_MAX_BURST,
                   DEC_X170_BUS_BURST_LENGTH);
    if (( G1DWLReadAsicID() >> 16) == 0x8170U)
    {
        SetDecRegister(pDecCont->rvRegs, HWIF_PRIORITY_MODE,
                       DEC_X170_ASIC_SERVICE_PRIORITY);
    }
    else
    {
        SetDecRegister(pDecCont->rvRegs, HWIF_DEC_SCMD_DIS,
            DEC_X170_SCMD_DISABLE);
    }
    DEC_SET_APF_THRESHOLD(pDecCont->rvRegs);
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_LATENCY,
                   DEC_X170_LATENCY_COMPENSATION);
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_DATA_DISC_E,
                   DEC_X170_DATA_DISCARD_ENABLE);
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_OUTSWAP32_E,
                   DEC_X170_OUTPUT_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_INSWAP32_E,
                   DEC_X170_INPUT_DATA_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_STRSWAP32_E,
                   DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);

#if( DEC_X170_HW_TIMEOUT_INT_ENA  != 0)
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_TIMEOUT_E, 1);
#else
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_TIMEOUT_E, 0);
#endif

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_CLK_GATE_E, 1);
#else
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_CLK_GATE_E, 0);
#endif

#if( DEC_X170_USING_IRQ  == 0)
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_IRQ_DIS, 1);
#else
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_IRQ_DIS, 0);
#endif

    /* set AXI RW IDs */
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_AXI_RD_ID,
                   (DEC_X170_AXI_ID_R & 0xFFU));
    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_AXI_WR_ID,
                   (DEC_X170_AXI_ID_W & 0xFFU));

    (void) G1DWLmemset(&config, 0, sizeof(DWLHwConfig_t));

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
            return RVDEC_FORMAT_NOT_SUPPORTED;
        }
        pDecCont->tiledModeSupport = config.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;

    pDecCont->StrmStorage.intraFreeze = useVideoFreezeConcealment;
    pDecCont->StrmStorage.pictureBroken = HANTRO_FALSE;
    if(numFrameBuffers > 16)
        numFrameBuffers = 16;
    pDecCont->StrmStorage.maxNumBuffers = numFrameBuffers;

    pDecCont->StrmStorage.isRv8 = rvVersion == 0;
    if (rvVersion == 0 && frameSizes != NULL)
    {
        pDecCont->StrmStorage.frameCodeLength = frameCodeLength;
        G1DWLmemcpy(pDecCont->StrmStorage.frameSizes, frameSizes,
            18*sizeof(u32));
        SetDecRegister(pDecCont->rvRegs, HWIF_FRAMENUM_LEN,
            frameCodeLength);
    }

    /* prediction filter taps */
    if (rvVersion == 0)
    {
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_0_0, -1);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_0_1, 12);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_0_2,  6);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_1_0,  6);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_1_1,  9);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_1_2,  1);
    }
    else
    {
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_0_0,  1);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_0_1, -5);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_0_2, 20);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_1_0,  1);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_1_1, -5);
        SetDecRegister(pDecCont->rvRegs, HWIF_PRED_BC_TAP_1_2, 52);
    }

    SetDecRegister(pDecCont->rvRegs, HWIF_DEC_MODE, RV_DEC_X170_MODE);
    SetDecRegister(pDecCont->rvRegs, HWIF_RV_PROFILE, rvVersion != 0);

    pDecCont->StrmStorage.maxFrameWidth = maxFrameWidth;
    pDecCont->StrmStorage.maxFrameHeight = maxFrameHeight;
    pDecCont->StrmStorage.maxMbsPerFrame = 
        ((pDecCont->StrmStorage.maxFrameWidth +15)>>4)*
        ((pDecCont->StrmStorage.maxFrameHeight+15)>>4);

    InitWorkarounds(RV_DEC_X170_MODE, &pDecCont->workarounds);

    RVDEC_API_DEBUG(("Container 0x%x\n", (u32) pDecCont));
    RV_API_TRC("RvDecInit: OK\n");

    return (RVDEC_OK);
}

/*------------------------------------------------------------------------------

    Function: RvDecGetInfo()

        Functional description:
            This function provides read access to decoder information. This
            function should not be called before RvDecDecode function has
            indicated that headers are ready.

        Inputs:
            decInst     decoder instance

        Outputs:
            pDecInfo    pointer to info struct where data is written

        Returns:
            RVDEC_OK            success
            RVDEC_PARAM_ERROR     invalid parameters

------------------------------------------------------------------------------*/
RvDecRet RvDecGetInfo(RvDecInst decInst, RvDecInfo * pDecInfo)
{

    DecFrameDesc *pFrameD;
    DecApiStorage *pApiStor;
    DecHdrs *pHdrs;

    RV_API_TRC("RvDecGetInfo#");

    if(decInst == NULL || pDecInfo == NULL)
    {
        return RVDEC_PARAM_ERROR;
    }

    pApiStor = &((DecContainer*)decInst)->ApiStorage;
    pFrameD = &((DecContainer*)decInst)->FrameDesc;
    pHdrs = &((DecContainer*)decInst)->Hdrs;

    pDecInfo->multiBuffPpSize = 2;

    if(pApiStor->DecStat == UNINIT || pApiStor->DecStat == INITIALIZED)
    {
        return RVDEC_HDRS_NOT_RDY;
    }

    pDecInfo->frameWidth = pFrameD->frameWidth <<4;
    pDecInfo->frameHeight = pFrameD->frameHeight <<4;

    pDecInfo->codedWidth = pHdrs->horizontalSize;
    pDecInfo->codedHeight = pHdrs->verticalSize;

    pDecInfo->outputFormat = RVDEC_SEMIPLANAR_YUV420;

    RV_API_TRC("RvDecGetInfo: OK");
    return (RVDEC_OK);

}

/*------------------------------------------------------------------------------

    Function: RvDecDecode

        Functional description:
            Decode stream data. Calls StrmDec_Decode to do the actual decoding.

        Input:
            decInst     decoder instance
            pInput      pointer to input struct

        Outputs:
            pOutput     pointer to output struct

        Returns:
            RVDEC_NOT_INITIALIZED   decoder instance not initialized yet
            RVDEC_PARAM_ERROR       invalid parameters

            RVDEC_STRM_PROCESSED    stream buffer decoded
            RVDEC_HDRS_RDY          headers decoded
            RVDEC_PIC_DECODED       decoding of a picture finished
            RVDEC_STRM_ERROR        serious error in decoding, no
                                       valid parameter sets available
                                       to decode picture data

------------------------------------------------------------------------------*/

RvDecRet RvDecDecode(RvDecInst decInst,
                           RvDecInput * pInput, RvDecOutput * pOutput)
{

    DecContainer *pDecCont;
    RvDecRet internalRet;
    DecApiStorage *pApiStor;
    DecStrmDesc *pStrmDesc;
    u32 strmDecResult;
    u32 asicStatus;
    i32 ret = 0;
    u32 errorConcealment = 0;
    u32 i;
    u32 *pSliceInfo;
    u32 containsInvalidSlice = HANTRO_FALSE;

    RV_API_TRC("\nRvDecDecode#");

    if(pInput == NULL || pOutput == NULL || decInst == NULL)
    {
        RV_API_TRC("RvDecDecode# PARAM_ERROR\n");
        return RVDEC_PARAM_ERROR;
    }

    pDecCont = ((DecContainer *) decInst);
    pApiStor = &pDecCont->ApiStorage;
    pStrmDesc = &pDecCont->StrmDesc;

    /*
     *  Check if decoder is in an incorrect mode
     */
    if(pApiStor->DecStat == UNINIT)
    {

        RV_API_TRC("RvDecDecode: NOT_INITIALIZED\n");
        return RVDEC_NOT_INITIALIZED;
    }

    if(pInput->dataLen == 0 ||
       pInput->dataLen > DEC_X170_MAX_STREAM ||
       pInput->pStream == NULL || pInput->streamBusAddress == 0)
    {
        RV_API_TRC("RvDecDecode# PARAM_ERROR\n");
        return RVDEC_PARAM_ERROR;
    }

    /* If we have set up for delayed resolution change, do it here */
    if(pDecCont->StrmStorage.rprDetected)
    {
        u32 sizeTmp;
        u32 newWidth, newHeight;
        u32 numPicsResampled;
        u32 resamplePics[2];            
        DWLLinearMem_t tmpData;

        pDecCont->StrmStorage.rprDetected = 0;

        numPicsResampled = 0;
        if( pDecCont->StrmStorage.rprNextPicType == RV_P_PIC)
        {
            resamplePics[0] = pDecCont->StrmStorage.work0;
            numPicsResampled = 1;
        }
        else if ( pDecCont->StrmStorage.rprNextPicType == RV_B_PIC )
        {
            /* B picture resampling not supported (would affect picture output
             * order and co-located MB data). So let's skip B frames until 
             * next reference picture. */
            pDecCont->StrmStorage.skipB = 1;
        }

        /* Allocate extra picture buffer for resampling */
        if( numPicsResampled )
        {
            internalRet = rvAllocateRprBuffer( pDecCont );
            if( internalRet != RVDEC_OK )
            {
                RVDEC_DEBUG(("ALLOC RPR BUFFER FAIL\n"));
                RV_API_TRC("RvDecDecode# MEMFAIL\n");
                return (internalRet);
            }
        }
            
        newWidth = pDecCont->tmpHdrs.horizontalSize;
        newHeight = pDecCont->tmpHdrs.verticalSize;

        /* Resample ref picture(s). Should be safe to do at this point; all 
         * original size pictures are output before this point. */
        for( i = 0 ; i < numPicsResampled ; ++i )
        {
            u32 j = resamplePics[i];
            picture_t * pRefPic;

            pRefPic = &pDecCont->StrmStorage.pPicBuf[j];

            if( pRefPic->codedWidth == newWidth &&
                pRefPic->codedHeight == newHeight )
                continue;

            rvRpr( pRefPic,
                      &pDecCont->StrmStorage.pRprBuf,
                      &pDecCont->StrmStorage.rprWorkBuffer,
                      0 /*round*/,
                      newWidth,
                      newHeight,
                      pDecCont->tiledReferenceEnable);

            pRefPic->codedWidth = newWidth;
            pRefPic->frameWidth = ( 15 + newWidth ) & ~15;
            pRefPic->codedHeight = newHeight;
            pRefPic->frameHeight = ( 15 + newHeight ) & ~15;

            tmpData = pDecCont->StrmStorage.pRprBuf.data;
            pDecCont->StrmStorage.pRprBuf.data = pRefPic->data;
            pRefPic->data = tmpData;
        }

        pDecCont->Hdrs.horizontalSize = newWidth;
        pDecCont->Hdrs.verticalSize = newHeight;

        SetDecRegister(pDecCont->rvRegs, HWIF_PIC_MB_WIDTH,
                       pDecCont->FrameDesc.frameWidth);
        SetDecRegister(pDecCont->rvRegs, HWIF_PIC_MB_HEIGHT_P,
                       pDecCont->FrameDesc.frameHeight);

        if(pDecCont->refBufSupport)
        {
            RefbuInit(&pDecCont->refBufferCtrl,
                      RV_DEC_X170_MODE,
                      pDecCont->FrameDesc.frameWidth,
                      pDecCont->FrameDesc.frameHeight,
                      pDecCont->refBufSupport);
        }

        pDecCont->StrmStorage.strmDecReady = HANTRO_TRUE;
    }

    if(pApiStor->DecStat == HEADERSDECODED)
    {
        rvFreeBuffers(pDecCont);

        if( pDecCont->StrmStorage.maxFrameWidth == 0 )
        {
            pDecCont->StrmStorage.maxFrameWidth =
                pDecCont->Hdrs.horizontalSize;
            pDecCont->StrmStorage.maxFrameHeight =
                pDecCont->Hdrs.verticalSize;
            pDecCont->StrmStorage.maxMbsPerFrame = 
                ((pDecCont->StrmStorage.maxFrameWidth +15)>>4)*
                ((pDecCont->StrmStorage.maxFrameHeight+15)>>4);
        }

        if(!pDecCont->StrmStorage.pPicBuf[0].data.busAddress)
        {
            RVDEC_DEBUG(("Allocate buffers\n"));
            internalRet = rvAllocateBuffers(pDecCont);
            if(internalRet != RVDEC_OK)
            {
                RVDEC_DEBUG(("ALLOC BUFFER FAIL\n"));
                RV_API_TRC("RvDecDecode# MEMFAIL\n");
                return (internalRet);
            }
        }

        /* Headers ready now, mems allocated, decoding can start */
        pApiStor->DecStat = STREAMDECODING;

    }

/*
 *  Update stream structure
 */
    pStrmDesc->pStrmBuffStart = pInput->pStream;
    pStrmDesc->pStrmCurrPos = pInput->pStream;
    pStrmDesc->bitPosInWord = 0;
    pStrmDesc->strmBuffSize = pInput->dataLen;
    pStrmDesc->strmBuffReadBits = 0;

    pDecCont->StrmStorage.numSlices = pInput->sliceInfoNum+1;
    /* Limit maximum n:o of slices 
     * (TODO, should we report an error?) */
    if(pDecCont->StrmStorage.numSlices > RV_DEC_X170_MAX_NUM_SLICES)
        pDecCont->StrmStorage.numSlices = RV_DEC_X170_MAX_NUM_SLICES;
    pSliceInfo = pDecCont->StrmStorage.slices.virtualAddress;

#ifdef RV_RAW_STREAM_SUPPORT
    pDecCont->StrmStorage.rawMode = pInput->sliceInfoNum == 0;
#endif

    /* convert slice offsets into slice sizes, TODO: check if memory given by application is writable */
    if (pApiStor->DecStat == STREAMDECODING
#ifdef RV_RAW_STREAM_SUPPORT
        && !pDecCont->StrmStorage.rawMode
#endif
        )
    /* Copy offsets to HW external memory */
    for( i = 0 ; i < pInput->sliceInfoNum ; ++i )
    {
        i32 tmp;
        if( i == pInput->sliceInfoNum-1 )
            tmp = pInput->dataLen;
        else
            tmp = pInput->pSliceInfo[i+1].offset;
        pSliceInfo[i] = tmp - pInput->pSliceInfo[i].offset;
        if(!pInput->pSliceInfo[i].isValid)
        {
            containsInvalidSlice = HANTRO_TRUE;
        }
    }

#ifdef _DEC_PP_USAGE
    pDecCont->StrmStorage.latestId = pInput->picId;
#endif

    if(containsInvalidSlice)
    {
        /* If stream contains even one invalid slice, we freeze the whole 
         * picture. At this point we could try to do some advanced
         * error concealment stuff */
        RVDEC_API_DEBUG(("STREAM ERROR; LEAST ONE SLICE BROKEN\n"));
        RVFLUSH;
        pDecCont->FrameDesc.picCodingType = RV_P_PIC;
        ret = rvHandleVlcModeError(pDecCont, pInput->picId);
        errorConcealment = HANTRO_TRUE;
        RVDEC_UPDATE_POUTPUT;
        ret = RVDEC_PIC_DECODED;
    }
    else /* All slices OK */
    {
        /* TODO: tarviiko luuppia (ehk? jos useampia slicej??) */
        do
        {
            RVDEC_API_DEBUG(("Start Decode\n"));
            /* run SW if HW is not in the middle of processing a picture
             * (indicated as HW_PIC_STARTED decoder status) */
            if (pApiStor->DecStat != HW_PIC_STARTED)
            {
                strmDecResult = rv_StrmDecode(pDecCont);
                switch (strmDecResult)
                {
                case DEC_PIC_HDR_RDY:
                    /* if type inter predicted and no reference -> error */
                    if((pDecCont->FrameDesc.picCodingType == RV_P_PIC &&
                        pDecCont->StrmStorage.work0 == INVALID_ANCHOR_PICTURE) ||
                       (pDecCont->FrameDesc.picCodingType == RV_B_PIC &&
                        (pDecCont->StrmStorage.work1 == INVALID_ANCHOR_PICTURE ||
                         pDecCont->StrmStorage.skipB ||
                         pInput->skipNonReference)) ||
                       (pDecCont->FrameDesc.picCodingType == RV_P_PIC &&
                        pDecCont->StrmStorage.pictureBroken &&
                        pDecCont->StrmStorage.intraFreeze))
                    {
                        if(pDecCont->StrmStorage.skipB ||
                           pInput->skipNonReference )
                        {
                            RV_API_TRC("RvDecDecode# RVDEC_NONREF_PIC_SKIPPED\n");
                        }
                        ret = rvHandleVlcModeError(pDecCont, pInput->picId);
                        errorConcealment = HANTRO_TRUE;
                    }
                    else
                    {
                        pApiStor->DecStat = HW_PIC_STARTED;
                    }
                    break;

                case DEC_PIC_HDR_RDY_ERROR:
                    ret = rvHandleVlcModeError(pDecCont, pInput->picId);
                    errorConcealment = HANTRO_TRUE;
                    /* copy output parameters for this PIC */
                    RVDEC_UPDATE_POUTPUT;
                    break;

                case DEC_PIC_HDR_RDY_RPR:
                    pDecCont->StrmStorage.strmDecReady = FALSE;
                    pApiStor->DecStat = STREAMDECODING;

                    if(pDecCont->refBufSupport)
                    {
                        RefbuInit(&pDecCont->refBufferCtrl,
                                  RV_DEC_X170_MODE,
                                  pDecCont->FrameDesc.frameWidth,
                                  pDecCont->FrameDesc.frameHeight,
                                  pDecCont->refBufSupport);
                    }
                    ret = RVDEC_STRM_PROCESSED;
                    break;

                case DEC_HDRS_RDY:
                    internalRet = rvDecCheckSupport(pDecCont);
                    if(internalRet != RVDEC_OK)
                    {
                        pDecCont->StrmStorage.strmDecReady = FALSE;
                        pApiStor->DecStat = INITIALIZED;
                        return internalRet;
                    }

                    pDecCont->ApiStorage.firstHeaders = 0;

                    SetDecRegister(pDecCont->rvRegs, HWIF_PIC_MB_WIDTH,
                                   pDecCont->FrameDesc.frameWidth);
                    SetDecRegister(pDecCont->rvRegs, HWIF_PIC_MB_HEIGHT_P,
                                   pDecCont->FrameDesc.frameHeight);

                    if(pDecCont->refBufSupport)
                    {
                        RefbuInit(&pDecCont->refBufferCtrl,
                                  RV_DEC_X170_MODE,
                                  pDecCont->FrameDesc.frameWidth,
                                  pDecCont->FrameDesc.frameHeight,
                                  pDecCont->refBufSupport);
                    }

                    pApiStor->DecStat = HEADERSDECODED;

                    RVDEC_API_DEBUG(("HDRS_RDY\n"));
                    ret = RVDEC_HDRS_RDY;
                    break;

                default:
                    ASSERT(strmDecResult == DEC_END_OF_STREAM);
                    if(pDecCont->StrmStorage.rprDetected)
                    {
                        ret = RVDEC_PIC_DECODED;
                    }
                    else
                    {
                        ret = RVDEC_STRM_PROCESSED;
                    }
                    break;
                }
            }

            /* picture header properly decoded etc -> start HW */
            if(pApiStor->DecStat == HW_PIC_STARTED)
            {
                if (!pDecCont->asicRunning)
                {
                    pDecCont->StrmStorage.workOut =
                        BqueueNext( &pDecCont->StrmStorage.bq,
                                     pDecCont->StrmStorage.work0,
                                     pDecCont->StrmStorage.work1,
                                     BQUEUE_UNUSED,
                                     pDecCont->FrameDesc.picCodingType == RV_B_PIC );
                    if(pDecCont->FrameDesc.picCodingType == RV_B_PIC)
                    {
                        pDecCont->StrmStorage.prevBIdx = pDecCont->StrmStorage.workOut;
                    }
                }
                asicStatus = RunDecoderAsic(pDecCont, pInput->streamBusAddress);

                if(asicStatus == ID8170_DEC_TIMEOUT)
                {
                    ret = RVDEC_HW_TIMEOUT;
                }
                else if(asicStatus == ID8170_DEC_SYSTEM_ERROR)
                {
                    ret = RVDEC_SYSTEM_ERROR;
                }
                else if(asicStatus == ID8170_DEC_HW_RESERVED)
                {
                    ret = RVDEC_HW_RESERVED;
                }
                else if(asicStatus & RV_DEC_X170_IRQ_TIMEOUT)
                {
                    RVDEC_API_DEBUG(("IRQ TIMEOUT IN HW\n"));
                    ret = rvHandleVlcModeError(pDecCont, pInput->picId);
                    errorConcealment = HANTRO_TRUE;
                    RVDEC_UPDATE_POUTPUT;
                }
                else if(asicStatus & RV_DEC_X170_IRQ_BUS_ERROR)
                {
                    ret = RVDEC_HW_BUS_ERROR;
                }
                else if(asicStatus & RV_DEC_X170_IRQ_STREAM_ERROR)
                {
                    RVDEC_API_DEBUG(("STREAM ERROR IN HW\n"));
                    RVFLUSH;
                    ret = rvHandleVlcModeError(pDecCont, pInput->picId);
                    errorConcealment = HANTRO_TRUE;
                    RVDEC_UPDATE_POUTPUT;
                }
                else if(asicStatus & RV_DEC_X170_IRQ_BUFFER_EMPTY)
                {
                    rvDecPreparePicReturn(pDecCont);
                    ret = RVDEC_STRM_PROCESSED;

                }
                /* HW finished decoding a picture */
                else if(asicStatus & RV_DEC_X170_IRQ_DEC_RDY)
                {
                    pDecCont->FrameDesc.frameNumber++;

                    rvHandleFrameEnd(pDecCont);

                    rvDecBufferPicture(pDecCont,
                                          pInput->picId,
                                          pDecCont->FrameDesc.
                                          picCodingType == RV_B_PIC,
                                          pDecCont->FrameDesc.
                                          picCodingType == RV_P_PIC,
                                          RVDEC_PIC_DECODED, 0);

                    ret = RVDEC_PIC_DECODED;

                    if(pDecCont->FrameDesc.picCodingType != RV_B_PIC)
                    {
                        pDecCont->StrmStorage.work1 =
                            pDecCont->StrmStorage.work0;
                        pDecCont->StrmStorage.work0 =
                            pDecCont->StrmStorage.workOut;
                        if(pDecCont->StrmStorage.skipB)
                            pDecCont->StrmStorage.skipB--;
                    }

                    if(pDecCont->FrameDesc.picCodingType == RV_I_PIC)
                        pDecCont->StrmStorage.pictureBroken = HANTRO_FALSE;

                    rvDecPreparePicReturn(pDecCont);
                }
                else
                {
                    ASSERT(0);
                }
                if(ret != RVDEC_STRM_PROCESSED)
                {
                    pApiStor->DecStat = STREAMDECODING;
                }

                if(ret == RVDEC_PIC_DECODED || ret == RVDEC_STRM_PROCESSED)
                {
                    /* copy output parameters for this PIC (excluding stream pos) */
                    pDecCont->MbSetDesc.outData.pStrmCurrPos =
                        pOutput->pStrmCurrPos;
                    RVDEC_UPDATE_POUTPUT;
                }
            }
        }
        while(ret == 0);
    }

    if(errorConcealment && pDecCont->FrameDesc.picCodingType != RV_B_PIC)
    {
        pDecCont->StrmStorage.pictureBroken = HANTRO_TRUE;
    }

    RV_API_TRC("RvDecDecode: Exit\n");
    if(!pDecCont->StrmStorage.rprDetected)
    {
        pOutput->pStrmCurrPos = pDecCont->StrmDesc.pStrmCurrPos;
        pOutput->strmCurrBusAddress = pInput->streamBusAddress +
            (pDecCont->StrmDesc.pStrmCurrPos - pDecCont->StrmDesc.pStrmBuffStart);
        pOutput->dataLeft = pDecCont->StrmDesc.strmBuffSize -
            (pOutput->pStrmCurrPos - pStrmDesc->pStrmBuffStart);
    }
    else
    {
        ret = RVDEC_STRM_PROCESSED;
    }

    return ((RvDecRet) ret);


}

/*------------------------------------------------------------------------------

    Function: RvDecRelease()

        Functional description:
            Release the decoder instance.

        Inputs:
            decInst     Decoder instance

        Outputs:
            none

        Returns:
            none

------------------------------------------------------------------------------*/

void RvDecRelease(RvDecInst decInst)
{
    DecContainer *pDecCont = NULL;
    const void *dwl;

    RVDEC_DEBUG(("1\n"));
    RV_API_TRC("RvDecRelease#");
    if(decInst == NULL)
    {
        RV_API_TRC("RvDecRelease# ERROR: decInst == NULL");
        return;
    }

    pDecCont = ((DecContainer *) decInst);
    dwl = pDecCont->dwl;

    if (pDecCont->asicRunning)
        (void) G1G1DWLReleaseHw(pDecCont->dwl);

    rvFreeBuffers(pDecCont);

    if (pDecCont->StrmStorage.slices.virtualAddress != NULL)
        DWLFreeLinear(pDecCont->dwl, &pDecCont->StrmStorage.slices);

    G1DWLfree(pDecCont);

    (void) G1DWLRelease(dwl);

    RV_API_TRC("RvDecRelease: OK");
}

/*------------------------------------------------------------------------------

    Function: rvRegisterPP()

        Functional description:
            Register the pp for RV pipeline

        Inputs:
            decInst     Decoder instance
            const void  *ppInst - post-processor instance
            (*PPRun)(const void *) - decoder calls this to start PP
            void (*PPEndCallback)(const void *) - decoder calls this
                        to notify PP that a picture was done.

        Outputs:
            none

        Returns:
            i32 - return 0 for success or a negative error code

------------------------------------------------------------------------------*/

i32 rvRegisterPP(const void *decInst, const void *ppInst,
                    void (*PPRun) (const void *, DecPpInterface *),
                    void (*PPEndCallback) (const void *),
                    void (*PPConfigQuery) (const void *, DecPpQuery *),
                    void (*PPDisplayIndex) (const void *, u32),
                    void (*PPBufferData) (const void *, u32, u32, u32))
{
    DecContainer *pDecCont;

    pDecCont = (DecContainer *) decInst;

    if(decInst == NULL || pDecCont->ppInstance != NULL ||
       ppInst == NULL || PPRun == NULL || PPEndCallback == NULL ||
       PPConfigQuery == NULL || PPDisplayIndex == NULL || PPBufferData == NULL)
        return -1;

    if(pDecCont->asicRunning)
        return -2;

    pDecCont->ppInstance = ppInst;
    pDecCont->PPEndCallback = PPEndCallback;
    pDecCont->PPRun = PPRun;
    pDecCont->PPConfigQuery = PPConfigQuery;
    pDecCont->PPDisplayIndex = PPDisplayIndex;
    pDecCont->PPBufferData = PPBufferData;

    return 0;
}

/*------------------------------------------------------------------------------

    Function: rvUnregisterPP()

        Functional description:
            Unregister the pp from RV pipeline

        Inputs:
            decInst     Decoder instance
            const void  *ppInst - post-processor instance

        Outputs:
            none

        Returns:
            i32 - return 0 for success or a negative error code

------------------------------------------------------------------------------*/

i32 rvUnregisterPP(const void *decInst, const void *ppInst)
{
    DecContainer *pDecCont;

    pDecCont = (DecContainer *) decInst;

    if(decInst == NULL || ppInst != pDecCont->ppInstance)
        return -1;

    if(pDecCont->asicRunning)
        return -2;

    pDecCont->ppInstance = NULL;
    pDecCont->PPEndCallback = NULL;
    pDecCont->PPRun = NULL;
    pDecCont->PPConfigQuery = NULL;
    pDecCont->PPDisplayIndex = NULL;
    pDecCont->PPBufferData = NULL;

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : rvRefreshRegs
    Description     :
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void rvRefreshRegs(DecContainer * pDecCont)
{
    i32 i;
    u32 *ppRegs = pDecCont->rvRegs;

    for(i = 0; i < DEC_X170_REGISTERS; i++)
    {
        ppRegs[i] = G1DWLReadReg(pDecCont->dwl, 4 * i);
    }
}

/*------------------------------------------------------------------------------
    Function name   : rvFlushRegs
    Description     :
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void rvFlushRegs(DecContainer * pDecCont)
{
    i32 i;
    u32 *ppRegs = pDecCont->rvRegs;

    for(i = 2; i < DEC_X170_REGISTERS; i++)
    {
        G1DWLWriteReg(pDecCont->dwl, 4 * i, ppRegs[i]);
        ppRegs[i] = 0;
    }
}

/*------------------------------------------------------------------------------
    Function name   : rvHandleVlcModeError
    Description     :
    Return type     : u32
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
u32 rvHandleVlcModeError(DecContainer * pDecCont, u32 picNum)
{
    u32 ret = RVDEC_STRM_PROCESSED;

    ASSERT(pDecCont->StrmStorage.strmDecReady);

    /*
    tmp = rvStrmDec_NextStartCode(pDecCont);
    if(tmp != END_OF_STREAM)
    {
        pDecCont->StrmDesc.pStrmCurrPos -= 4;
        pDecCont->StrmDesc.strmBuffReadBits -= 32;
    }
    */

    /* error in first picture -> set reference to grey */
    if(!pDecCont->FrameDesc.frameNumber)
    {
        (void) G1DWLmemset(pDecCont->StrmStorage.
                         pPicBuf[pDecCont->StrmStorage.workOut].data.
                         virtualAddress, 128,
                         384 * pDecCont->FrameDesc.totalMbInFrame);

        rvDecPreparePicReturn(pDecCont);

        /* no pictures finished -> return STRM_PROCESSED */
        ret = RVDEC_STRM_PROCESSED;
        pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.workOut;
        pDecCont->StrmStorage.skipB = 2;
    }
    else
    {
        if(pDecCont->FrameDesc.picCodingType != RV_B_PIC)
        {
            pDecCont->FrameDesc.frameNumber++;

            /* reset sendToPp to prevent post-processing partly decoded
             * pictures */
            if(pDecCont->StrmStorage.workOut != pDecCont->StrmStorage.work0)
                pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].
                    sendToPp = 0;

            BqueueDiscard(&pDecCont->StrmStorage.bq,
                pDecCont->StrmStorage.workOut );
            pDecCont->StrmStorage.workOut = pDecCont->StrmStorage.work0;

            rvDecBufferPicture(pDecCont,
                                  picNum,
                                  pDecCont->FrameDesc.picCodingType == RV_B_PIC,
                                  1, (RvDecRet) FREEZED_PIC_RDY,
                                  pDecCont->FrameDesc.totalMbInFrame);

            ret = RVDEC_PIC_DECODED;

            pDecCont->StrmStorage.work1 = pDecCont->StrmStorage.work0;
            pDecCont->StrmStorage.skipB = 2;
        }
        else
        {
            if(pDecCont->StrmStorage.intraFreeze)
            {
                pDecCont->FrameDesc.frameNumber++;
                rvDecBufferPicture(pDecCont,
                                      picNum,
                                      pDecCont->FrameDesc.picCodingType ==
                                      RV_B_PIC, 1, (RvDecRet) FREEZED_PIC_RDY,
                                      pDecCont->FrameDesc.totalMbInFrame);

                ret = RVDEC_PIC_DECODED;

            }
            else
            {
                ret = RVDEC_NONREF_PIC_SKIPPED; 
            }

            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.prevBIdx].sendToPp = 0;
        }
    }

    pDecCont->ApiStorage.DecStat = STREAMDECODING;

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : rvHandleFrameEnd
    Description     :
    Return type     : u32
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void rvHandleFrameEnd(DecContainer * pDecCont)
{

    pDecCont->StrmDesc.strmBuffReadBits =
        8 * (pDecCont->StrmDesc.pStrmCurrPos -
             pDecCont->StrmDesc.pStrmBuffStart);
    pDecCont->StrmDesc.bitPosInWord = 0;

}

/*------------------------------------------------------------------------------

         Function name: RunDecoderAsic

         Purpose:       Set Asic run lenght and run Asic

         Input:         DecContainer *pDecCont

         Output:        void

------------------------------------------------------------------------------*/
u32 RunDecoderAsic(DecContainer * pDecContainer, u32 strmBusAddress)
{
    i32 ret;
    u32 tmp = 0;
    u32 asicStatus = 0;

    ASSERT(pDecContainer->StrmStorage.
           pPicBuf[pDecContainer->StrmStorage.workOut].data.busAddress != 0);
    ASSERT(strmBusAddress != 0);

    /* set pp luma bus */
    pDecContainer->ppControl.inputBusLuma = 0;

    if(!pDecContainer->asicRunning)
    {
        tmp = RvSetRegs(pDecContainer, strmBusAddress);
        if(tmp == HANTRO_NOK)
            return 0;

        (void) G1DWLReserveHw(pDecContainer->dwl);

        /* Start PP */
        if(pDecContainer->ppInstance != NULL)
        {
            RvPpControl(pDecContainer, 0);
        }
        else
        {
            SetDecRegister(pDecContainer->rvRegs, HWIF_DEC_OUT_DIS, 0);
            SetDecRegister(pDecContainer->rvRegs, HWIF_FILTERING_DIS, 1);
        }

        pDecContainer->asicRunning = 1;

        G1DWLWriteReg(pDecContainer->dwl, 0x4, 0);

        rvFlushRegs(pDecContainer);

        /* Enable HW */
        SetDecRegister(pDecContainer->rvRegs, HWIF_DEC_E, 1);
        DWLEnableHW(pDecContainer->dwl, 4 * 1, pDecContainer->rvRegs[1]);
    }
    else    /* in the middle of decoding, continue decoding */
    {
        /* tmp is strmBusAddress + number of bytes decoded by SW */
        /* TODO: alotetaanko aina bufferin alusta? */
        tmp = pDecContainer->StrmDesc.pStrmCurrPos -
            pDecContainer->StrmDesc.pStrmBuffStart;
        tmp = strmBusAddress + tmp;

        /* pointer to start of the stream, mask to get the pointer to
         * previous 64-bit aligned position */
        if(!(tmp & ~0x7))
        {
            return 0;
        }

        SetDecRegister(pDecContainer->rvRegs, HWIF_RLC_VLC_BASE, tmp & ~0x7);
        /* amount of stream (as seen by the HW), obtained as amount of stream
         * given by the application subtracted by number of bytes decoded by
         * SW (if strmBusAddress is not 64-bit aligned -> adds number of bytes
         * from previous 64-bit aligned boundary) */
        SetDecRegister(pDecContainer->rvRegs, HWIF_STREAM_LEN,
                       pDecContainer->StrmDesc.strmBuffSize -
                       ((tmp & ~0x7) - strmBusAddress));
        SetDecRegister(pDecContainer->rvRegs, HWIF_STRM_START_BIT,
                       pDecContainer->StrmDesc.bitPosInWord + 8 * (tmp & 0x7));

        /* This depends on actual register allocation */
        G1DWLWriteReg(pDecContainer->dwl, 4 * 5, pDecContainer->rvRegs[5]);
        G1DWLWriteReg(pDecContainer->dwl, 4 * 6, pDecContainer->rvRegs[6]);
        G1DWLWriteReg(pDecContainer->dwl, 4 * 12, pDecContainer->rvRegs[12]);

        G1DWLWriteReg(pDecContainer->dwl, 4 * 1, pDecContainer->rvRegs[1]);
    }

    /* Wait for HW ready */
    RVDEC_API_DEBUG(("Wait for Decoder\n"));
    ret = G1DWLWaitHwReady(pDecContainer->dwl, (u32) DEC_X170_TIMEOUT_LENGTH);

    rvRefreshRegs(pDecContainer);

    if(ret == DWL_HW_WAIT_OK)
    {
        asicStatus =
            GetDecRegister(pDecContainer->rvRegs, HWIF_DEC_IRQ_STAT);
    }
    else if(ret == DWL_HW_WAIT_TIMEOUT)
    {
        asicStatus = ID8170_DEC_TIMEOUT;
    }
    else
    {
        asicStatus = ID8170_DEC_SYSTEM_ERROR;
    }

    if(!(asicStatus & RV_DEC_X170_IRQ_BUFFER_EMPTY) ||
       (asicStatus & RV_DEC_X170_IRQ_STREAM_ERROR) ||
       (asicStatus & RV_DEC_X170_IRQ_BUS_ERROR) ||
       (asicStatus == ID8170_DEC_TIMEOUT) ||
       (asicStatus == ID8170_DEC_SYSTEM_ERROR))
    {
        /* reset HW */
        SetDecRegister(pDecContainer->rvRegs, HWIF_DEC_IRQ_STAT, 0);
        SetDecRegister(pDecContainer->rvRegs, HWIF_DEC_IRQ, 0);

        DWLDisableHW(pDecContainer->dwl, 4 * 1, pDecContainer->rvRegs[1]);

        /* End PP co-operation */
        if(pDecContainer->ppControl.ppStatus == DECPP_RUNNING)
        {
            RVDEC_API_DEBUG(("RunDecoderAsic: PP Wait for end\n"));
            if(pDecContainer->ppInstance != NULL)
                pDecContainer->PPEndCallback(pDecContainer->ppInstance);
            RVDEC_API_DEBUG(("RunDecoderAsic: PP Finished\n"));

            if((asicStatus & RV_DEC_X170_IRQ_STREAM_ERROR) &&
               pDecContainer->ppControl.usePipeline)
                pDecContainer->ppControl.ppStatus = DECPP_IDLE;
            else
                pDecContainer->ppControl.ppStatus = DECPP_PIC_READY;
        }

        pDecContainer->asicRunning = 0;
        (void) G1G1DWLReleaseHw(pDecContainer->dwl);
    }

    /* if HW interrupt indicated either BUFFER_EMPTY or
     * DEC_RDY -> read stream end pointer and update StrmDesc structure */
    if((asicStatus &
        (RV_DEC_X170_IRQ_BUFFER_EMPTY | RV_DEC_X170_IRQ_DEC_RDY)))
    {
        tmp = GetDecRegister(pDecContainer->rvRegs, HWIF_RLC_VLC_BASE);
        if((tmp - strmBusAddress) <= DEC_X170_MAX_STREAM)
        {
            pDecContainer->StrmDesc.pStrmCurrPos =
                pDecContainer->StrmDesc.pStrmBuffStart + (tmp - strmBusAddress);
        }
        else
        {
            pDecContainer->StrmDesc.pStrmCurrPos =
                pDecContainer->StrmDesc.pStrmBuffStart +
                pDecContainer->StrmDesc.strmBuffSize;
        }
    }

    if( pDecContainer->FrameDesc.picCodingType != RV_B_PIC &&
        pDecContainer->refBufSupport &&
        (asicStatus & RV_DEC_X170_IRQ_DEC_RDY) &&
        pDecContainer->asicRunning == 0)
    {
        RefbuMvStatistics( &pDecContainer->refBufferCtrl, 
                            pDecContainer->rvRegs,
                            pDecContainer->StrmStorage.directMvs.virtualAddress,
                            pDecContainer->FrameDesc.picCodingType == RV_P_PIC,
                            pDecContainer->FrameDesc.picCodingType == RV_I_PIC );
    }

    SetDecRegister(pDecContainer->rvRegs, HWIF_DEC_IRQ_STAT, 0);

    return asicStatus;

}

/*------------------------------------------------------------------------------

    Function name: RvDecNextPicture

    Functional description:
        Retrieve next decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct
        endOfStream Indicates whether end of stream has been reached

    Output:
        pPicture Decoder output picture.

    Return values:
        RVDEC_OK         No picture available.
        RVDEC_PIC_RDY    Picture ready.

------------------------------------------------------------------------------*/
RvDecRet RvDecNextPicture(RvDecInst decInst,
                                RvDecPicture * pPicture, u32 endOfStream)
{
    /* Variables */
    RvDecRet returnValue = RVDEC_PIC_RDY;
    DecContainer *pDecCont;
    picture_t *pPic;
    u32 picIndex = RV_BUFFER_UNDEFINED;
    u32 minCount;
    u32 tmp = 0;
    u32 luma = 0;
    u32 chroma = 0;
    static u32 picCount = 0;

    /* Code */
    RV_API_TRC("\nRvDecNextPicture#");

    /* Check that function input parameters are valid */
    if(pPicture == NULL)
    {
        RV_API_TRC("RvDecNextPicture# ERROR: pPicture is NULL");
        return (RVDEC_PARAM_ERROR);
    }

    pDecCont = (DecContainer *) decInst;

    /* Check if decoder is in an incorrect mode */
    if(decInst == NULL || pDecCont->ApiStorage.DecStat == UNINIT)
    {
        RV_API_TRC("RvDecNextPicture# ERROR: Decoder not initialized");
        return (RVDEC_NOT_INITIALIZED);
    }

    minCount = 0;
    if(!endOfStream && !pDecCont->StrmStorage.rprDetected)
        minCount = 1;

    /* this is to prevent post-processing of non-finished pictures in the
     * end of the stream */
    if(endOfStream && pDecCont->FrameDesc.picCodingType == RV_B_PIC)
    {
        pDecCont->FrameDesc.picCodingType = RV_P_PIC;
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.prevBIdx].sendToPp = 0;
    }

    /* Nothing to send out */
    if(pDecCont->StrmStorage.outCount <= minCount)
    {
        (void) G1DWLmemset(pPicture, 0, sizeof(RvDecPicture));
        pPicture->pOutputPicture = NULL;
        returnValue = RVDEC_OK;
    }
    else
    {
        picIndex = pDecCont->StrmStorage.outIndex;
        picIndex = pDecCont->StrmStorage.outBuf[picIndex];

        RvFillPicStruct(pPicture, pDecCont, picIndex);

        picCount++;

        pDecCont->StrmStorage.outCount--;
        pDecCont->StrmStorage.outIndex++;
        pDecCont->StrmStorage.outIndex &= 0xF;
    }

    if(pDecCont->ppInstance &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE) &&
       endOfStream && (returnValue == RVDEC_PIC_RDY))
    {
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_UNINIT;
        if( pDecCont->StrmStorage.previousB)
        {
            pDecCont->ppControl.displayIndex = 
                pDecCont->ppControl.prevAnchorDisplayIndex;
            pDecCont->ppControl.bufferIndex =
                pDecCont->ppControl.displayIndex;
        }
        else
        {
            pDecCont->ppControl.displayIndex = pDecCont->ppControl.bufferIndex;
        }
        pDecCont->PPDisplayIndex(pDecCont->ppInstance,
                                 pDecCont->ppControl.bufferIndex);
    }

    /* pp display process is separate of decoding process */
    if(pDecCont->ppInstance && returnValue == RVDEC_PIC_RDY &&
       (pDecCont->ppControl.multiBufStat != MULTIBUFFER_FULLMODE))
    {
        /* pp and decoder running in parallel, decoder finished first field ->
         * decode second field and wait PP after that */
        if(pDecCont->ppInstance != NULL &&
           pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED)
        {
            return (RVDEC_OK);
        }

        if(pDecCont->ppControl.ppStatus == DECPP_PIC_READY)
        {
            RvFillPicStruct(pPicture, pDecCont, picIndex);
            returnValue = RVDEC_PIC_RDY;
            pDecCont->ppControl.ppStatus = DECPP_IDLE;
        }
        else
        {
            pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;
            returnValue = RVDEC_OK;

            if(RVDEC_NON_PIPELINE_AND_B_PICTURE)
            {
                picIndex = pDecCont->StrmStorage.prevBIdx;   /* send index 2 (B Picture output) to PP) */
                pDecCont->FrameDesc.picCodingType = RV_I_PIC;

            }
            else if(endOfStream)
            {
                picIndex = pDecCont->StrmStorage.work0;
                pPic[picIndex].sendToPp = 2;
                /*
                 * picIndex = 0;
                 * while((picIndex < 3) && !pPic[picIndex].sendToPp)
                 * picIndex++;
                 * if (picIndex == 3)
                 * return RVDEC_OK;
                 */

            }
            /* handle I/P pictures where RunDecoderAsic was not invoced (error
             * in picture headers etc) */
            else if(!pDecCont->ppConfigQuery.pipelineAccepted)
            {
                if(pDecCont->StrmStorage.outCount &&
                   pDecCont->StrmStorage.outBuf[0] ==
                   pDecCont->StrmStorage.outBuf[1])
                {
                    picIndex = pDecCont->StrmStorage.outBuf[0];
                    tmp = 1;
                }
            }

            if(picIndex != RV_BUFFER_UNDEFINED)
            {
                if(pPic[picIndex].sendToPp && pPic[picIndex].sendToPp < 3)
                {
                    RVDEC_API_DEBUG(("NextPicture: send to pp %d\n",
                                        picIndex));
                    pDecCont->ppControl.tiledInputMode =
                        pDecCont->tiledReferenceEnable;
                    pDecCont->ppControl.progressiveSequence = 1;
                    pDecCont->ppControl.inputBusLuma =
                        pDecCont->StrmStorage.pPicBuf[picIndex].data.
                        busAddress;
                    pDecCont->ppControl.inputBusChroma =
                        pDecCont->ppControl.inputBusLuma +
                        ((pDecCont->FrameDesc.frameWidth *
                          pDecCont->FrameDesc.frameHeight) << 8);
                    pDecCont->ppControl.inwidth =
                        pDecCont->ppControl.croppedW =
                        pDecCont->FrameDesc.frameWidth << 4;
                    pDecCont->ppControl.inheight =
                        pDecCont->ppControl.croppedH =
                        pDecCont->FrameDesc.frameHeight << 4;

                    pDecCont->ppControl.usePipeline = 0;
                    {
                        u32 value = GetDecRegister(pDecCont->rvRegs,
                                                   HWIF_DEC_OUT_ENDIAN);

                        pDecCont->ppControl.littleEndian =
                            (value == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pDecCont->ppControl.wordSwap =
                        GetDecRegister(pDecCont->rvRegs,
                                       HWIF_DEC_OUTSWAP32_E) ? 1 : 0;

                    /* Run pp */
                    pDecCont->PPRun(pDecCont->ppInstance, &pDecCont->ppControl);
                    pDecCont->ppControl.ppStatus = DECPP_RUNNING;

                    if(pDecCont->ppControl.picStruct ==
                       DECPP_PIC_FRAME_OR_TOP_FIELD ||
                       pDecCont->ppControl.picStruct ==
                       DECPP_PIC_TOP_AND_BOT_FIELD_FRAME)
                    {
                        /* tmp set if freezed pic and has will be used as
                         * output another time */
                        if(!tmp)
                            pDecCont->StrmStorage.pPicBuf[picIndex].sendToPp =
                                0;
                    }
                    else
                    {
                        pDecCont->StrmStorage.pPicBuf[picIndex].sendToPp--;
                    }

                    /* Wait for result */
                    pDecCont->PPEndCallback(pDecCont->ppInstance);

                    RvFillPicStruct(pPicture, pDecCont, picIndex);
                    returnValue = RVDEC_PIC_RDY;
                    pDecCont->ppControl.ppStatus = DECPP_IDLE;
                    pDecCont->ppControl.picStruct =
                        DECPP_PIC_FRAME_OR_TOP_FIELD;
                }
            }
        }
    }

    return returnValue;
}

/*----------------------=-------------------------------------------------------

    Function name: RvFillPicStruct

    Functional description:
        Fill data to output pic description

    Input:
        pDecCont    Decoder container
        pPicture    Pointer to return value struct

    Return values:
        void

------------------------------------------------------------------------------*/
void RvFillPicStruct(RvDecPicture * pPicture,
                        DecContainer * pDecCont, u32 picIndex)
{
    picture_t *pPic;

    pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;

    pPicture->frameWidth = pPic[picIndex].frameWidth;
    pPicture->frameHeight = pPic[picIndex].frameHeight;
    pPicture->codedWidth = pPic[picIndex].codedWidth;
    pPicture->codedHeight = pPic[picIndex].codedHeight;

    pPicture->pOutputPicture = (u8 *) pPic[picIndex].data.virtualAddress;
    pPicture->outputPictureBusAddress = pPic[picIndex].data.busAddress;
    pPicture->keyPicture = pPic[picIndex].picType;
    pPicture->picId = pPic[picIndex].picId;
    pPicture->outputFormat = pPic[picIndex].tiledMode ?
        DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

    pPicture->numberOfErrMBs = pPic[picIndex].nbrErrMbs;

}

/*------------------------------------------------------------------------------

    Function name: RvSetRegs

    Functional description:
        Set registers

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
u32 RvSetRegs(DecContainer * pDecContainer, u32 strmBusAddress)
{
    u32 tmp = 0;
    u32 tmpFwd;
    const u32 intraQpIndex[] =
        {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,3,3,3,3,3,4,4,4,4,4,0};
    const u32 interQpIndex[] =
        {0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6};

#ifdef _DEC_PP_USAGE
    RvDecPpUsagePrint(pDecContainer, DECPP_UNSPECIFIED,
                         pDecContainer->StrmStorage.workOut, 1,
                         pDecContainer->StrmStorage.latestId);
#endif

    pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.workOut].
        sendToPp = 1;


    RVDEC_API_DEBUG(("Decoding to index %d \n",
                        pDecContainer->StrmStorage.workOut));

    /* swReg3 */
    SetDecRegister(pDecContainer->rvRegs, HWIF_PIC_INTERLACE_E, 0);
    SetDecRegister(pDecContainer->rvRegs, HWIF_PIC_FIELDMODE_E, 0);

    /*
    SetDecRegister(pDecContainer->rvRegs, HWIF_PIC_MB_HEIGHT_P,
                   pDecContainer->FrameDesc.frameHeight);
    */

    if(pDecContainer->FrameDesc.picCodingType == RV_B_PIC)
        SetDecRegister(pDecContainer->rvRegs, HWIF_PIC_B_E, 1);
    else
        SetDecRegister(pDecContainer->rvRegs, HWIF_PIC_B_E, 0);

    SetDecRegister(pDecContainer->rvRegs, HWIF_PIC_INTER_E,
                   pDecContainer->FrameDesc.picCodingType == RV_P_PIC ||
                   pDecContainer->FrameDesc.picCodingType == RV_B_PIC ? 1 : 0);

    SetDecRegister(pDecContainer->rvRegs, HWIF_WRITE_MVS_E,
                   pDecContainer->FrameDesc.picCodingType == RV_P_PIC);


    SetDecRegister(pDecContainer->rvRegs, HWIF_INIT_QP,
        pDecContainer->FrameDesc.qp);

    SetDecRegister(pDecContainer->rvRegs, HWIF_RV_FWD_SCALE,
        pDecContainer->StrmStorage.fwdScale);
    SetDecRegister(pDecContainer->rvRegs, HWIF_RV_BWD_SCALE,
        pDecContainer->StrmStorage.bwdScale);

    /* swReg5 */

    /* tmp is strmBusAddress + number of bytes decoded by SW */
#ifdef RV_RAW_STREAM_SUPPORT
    if (pDecContainer->StrmStorage.rawMode)
        tmp = pDecContainer->StrmDesc.pStrmCurrPos -
            pDecContainer->StrmDesc.pStrmBuffStart;
    else
#endif
        tmp = 0;

    tmp = strmBusAddress + tmp;

    /* bus address must not be zero */
    if(!(tmp & ~0x7))
    {
        return 0;
    }

    /* pointer to start of the stream, mask to get the pointer to
     * previous 64-bit aligned position */
    SetDecRegister(pDecContainer->rvRegs, HWIF_RLC_VLC_BASE, tmp & ~0x7);

    /* amount of stream (as seen by the HW), obtained as amount of
     * stream given by the application subtracted by number of bytes
     * decoded by SW (if strmBusAddress is not 64-bit aligned -> adds
     * number of bytes from previous 64-bit aligned boundary) */
    SetDecRegister(pDecContainer->rvRegs, HWIF_STREAM_LEN,
                   pDecContainer->StrmDesc.strmBuffSize -
                   ((tmp & ~0x7) - strmBusAddress));


#ifdef RV_RAW_STREAM_SUPPORT
    if (pDecContainer->StrmStorage.rawMode)
        SetDecRegister(pDecContainer->rvRegs, HWIF_STRM_START_BIT,
                       pDecContainer->StrmDesc.bitPosInWord + 8 * (tmp & 0x7));
    else
#endif
        SetDecRegister(pDecContainer->rvRegs, HWIF_STRM_START_BIT, 0);

    /* swReg13 */
    SetDecRegister(pDecContainer->rvRegs, HWIF_DEC_OUT_BASE,
       pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.workOut].
                   data.busAddress);

    if(pDecContainer->FrameDesc.picCodingType == RV_B_PIC) /* ? */
    {
        /* past anchor set to future anchor if past is invalid (second
         * picture in sequence is B) */
        tmpFwd =
            pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE ?
            pDecContainer->StrmStorage.work1 :
            pDecContainer->StrmStorage.work0;

        SetDecRegister(pDecContainer->rvRegs, HWIF_REFER0_BASE,
                       pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                       busAddress);
        SetDecRegister(pDecContainer->rvRegs, HWIF_REFER1_BASE,
                       pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                       busAddress);
        SetDecRegister(pDecContainer->rvRegs, HWIF_REFER2_BASE,
                       pDecContainer->StrmStorage.
                       pPicBuf[pDecContainer->StrmStorage.work0].data.
                       busAddress);
        SetDecRegister(pDecContainer->rvRegs, HWIF_REFER3_BASE,
                       pDecContainer->StrmStorage.
                       pPicBuf[pDecContainer->StrmStorage.work0].data.
                       busAddress);
    }
    else
    {
        SetDecRegister(pDecContainer->rvRegs, HWIF_REFER0_BASE,
                       pDecContainer->StrmStorage.
                       pPicBuf[pDecContainer->StrmStorage.work0].data.
                       busAddress);
        SetDecRegister(pDecContainer->rvRegs, HWIF_REFER1_BASE,
                       pDecContainer->StrmStorage.
                       pPicBuf[pDecContainer->StrmStorage.work0].data.
                       busAddress);
    }

    SetDecRegister(pDecContainer->rvRegs, HWIF_STARTMB_X, 0);
    SetDecRegister(pDecContainer->rvRegs, HWIF_STARTMB_Y, 0);

    SetDecRegister(pDecContainer->rvRegs, HWIF_DEC_OUT_DIS, 0);
    SetDecRegister(pDecContainer->rvRegs, HWIF_FILTERING_DIS, 1);

    SetDecRegister(pDecContainer->rvRegs, HWIF_DIR_MV_BASE,
                   pDecContainer->StrmStorage.directMvs.busAddress);
    SetDecRegister(pDecContainer->rvRegs, HWIF_PREV_ANC_TYPE,
                   pDecContainer->StrmStorage.pPicBuf[
                         pDecContainer->StrmStorage.work0].isInter);

#ifdef RV_RAW_STREAM_SUPPORT
    if (pDecContainer->StrmStorage.rawMode)
        SetDecRegister(pDecContainer->rvRegs, HWIF_PIC_SLICE_AM, 0);
    else
#endif
    SetDecRegister(pDecContainer->rvRegs, HWIF_PIC_SLICE_AM,
        pDecContainer->StrmStorage.numSlices-1);
    SetDecRegister(pDecContainer->rvRegs, HWIF_QTABLE_BASE,
        pDecContainer->StrmStorage.slices.busAddress);

    if (!pDecContainer->StrmStorage.isRv8)
        SetDecRegister(pDecContainer->rvRegs, HWIF_FRAMENUM_LEN,
            pDecContainer->StrmStorage.frameSizeBits);

    /* Setup reference picture buffer */
    if(pDecContainer->refBufSupport)
    {
        RefbuSetup(&pDecContainer->refBufferCtrl, pDecContainer->rvRegs,
                   REFBU_FRAME,
                   pDecContainer->FrameDesc.picCodingType == RV_I_PIC,
                   pDecContainer->FrameDesc.picCodingType == RV_B_PIC, 0, 2,
                   0 );
    }

    if( pDecContainer->tiledModeSupport)
    {
        pDecContainer->tiledReferenceEnable = 
            DecSetupTiledReference( pDecContainer->rvRegs, 
                pDecContainer->tiledModeSupport,
                0 /* interlaced content not present */ );
    }
    else
    {
        pDecContainer->tiledReferenceEnable = 0;
    }


    if (pDecContainer->StrmStorage.rawMode)
    {
        SetDecRegister(pDecContainer->rvRegs, HWIF_RV_OSV_QUANT,
            pDecContainer->FrameDesc.vlcSet );
    }
    return HANTRO_OK;
}

/*------------------------------------------------------------------------------

    Function name: RvCheckFormatSupport

    Functional description:
        Check if RV supported

    Input:
        container

    Return values:
        return zero for OK

------------------------------------------------------------------------------*/
u32 RvCheckFormatSupport(void)
{
    u32 id = 0;
    u32 product = 0;
    DWLHwConfig_t hwConfig;

    id = G1DWLReadAsicID();

    product = id >> 16;

    if(product < 0x8170 &&
       product != 0x6731 )
        return ~0;

    DWLReadAsicConfig(&hwConfig);

    return (hwConfig.rvSupport == RV_NOT_SUPPORTED);
}

/*------------------------------------------------------------------------------

    Function name: RvPpControl

    Functional description:
        set up and start pp

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/

void RvPpControl(DecContainer * pDecContainer, u32 pipelineOff)
{
    u32 indexForPp = RV_BUFFER_UNDEFINED;
    u32 nextBufferIndex;

    DecPpInterface *pc = &pDecContainer->ppControl;

    /* PP not connected or still running (not waited when first field of frame
     * finished */
    if(pc->multiBufStat == MULTIBUFFER_DISABLED &&
       pc->ppStatus == DECPP_PIC_NOT_FINISHED)
        return;

    pDecContainer->ppConfigQuery.tiledMode =
        pDecContainer->tiledReferenceEnable;
    pDecContainer->PPConfigQuery(pDecContainer->ppInstance,
                                 &pDecContainer->ppConfigQuery);

    RvPpMultiBufferSetup(pDecContainer, (pipelineOff ||
                                            !pDecContainer->ppConfigQuery.
                                            pipelineAccepted));

    pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.workOut].
        sendToPp = 1;

    /* Select new PP buffer index to use. If multibuffer is disabled, use
     * previous buffer, otherwise select new buffer from queue. */
    if(pc->multiBufStat != MULTIBUFFER_DISABLED)
    {
        nextBufferIndex = BqueueNext( &pDecContainer->StrmStorage.bqPp,
            BQUEUE_UNUSED,
            BQUEUE_UNUSED,
            BQUEUE_UNUSED,
            pDecContainer->FrameDesc.picCodingType == RV_B_PIC);
        pc->bufferIndex = nextBufferIndex;
    }
    else
    {
        nextBufferIndex = pc->bufferIndex;
    }

    if(/*pHdrs->lowDelay ||*/
       pDecContainer->FrameDesc.picCodingType == RV_B_PIC)
    {
        pc->displayIndex = pc->bufferIndex;
    }
    else
    {
        pc->displayIndex = pc->prevAnchorDisplayIndex;
    }

    /* Connect PP output buffer to decoder output buffer */
    {
        u32 luma = 0;
        u32 chroma = 0;

        luma = pDecContainer->StrmStorage.
            pPicBuf[pDecContainer->StrmStorage.workOut].data.busAddress;
        chroma = luma + ((pDecContainer->FrameDesc.frameWidth *
                 pDecContainer->FrameDesc.frameHeight) << 8);

        pDecContainer->PPBufferData(pDecContainer->ppInstance, 
            pc->bufferIndex, luma, chroma);
    }

    if(pc->multiBufStat == MULTIBUFFER_FULLMODE)
    {
        RVDEC_API_DEBUG(("Full pipeline# \n"));
        pc->usePipeline = pDecContainer->ppConfigQuery.pipelineAccepted;
        RvDecRunFullmode(pDecContainer);
        pDecContainer->StrmStorage.previousModeFull = 1;
    }
    else if(pDecContainer->StrmStorage.previousModeFull == 1)
    {
        if(pDecContainer->FrameDesc.picCodingType == RV_B_PIC)
        {
            pDecContainer->StrmStorage.previousB = 1;
        }
        else
        {
            pDecContainer->StrmStorage.previousB = 0;
        }

        if(pDecContainer->FrameDesc.picCodingType == RV_B_PIC)
        {
            RVDEC_API_DEBUG(("PIPELINE OFF, DON*T SEND B TO PP\n"));
            indexForPp = RV_BUFFER_UNDEFINED;
            pc->inputBusLuma = 0;
        }
        pc->ppStatus = DECPP_IDLE;

        pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.work0].sendToPp = 1;

        pDecContainer->StrmStorage.previousModeFull = 0;
    }
    else
    {
        pc->bufferIndex = pc->displayIndex;

        if(pDecContainer->FrameDesc.picCodingType == RV_B_PIC)
        {
            pDecContainer->StrmStorage.previousB = 1;
        }
        else
        {
            pDecContainer->StrmStorage.previousB = 0;
        }

        if((pDecContainer->FrameDesc.picCodingType != RV_B_PIC) ||
            pipelineOff)
        {
            pc->usePipeline = 0;
        }
        else
        {
            RVDEC_API_DEBUG(("RUN PP  # \n"));
            RVFLUSH;
            pc->usePipeline = pDecContainer->ppConfigQuery.pipelineAccepted;
        }

        if(!pc->usePipeline)
        {
            /* pipeline not accepted, don't run for first picture */
            if(pDecContainer->FrameDesc.frameNumber &&
               (pDecContainer->ApiStorage.bufferForPp == NO_BUFFER))
            {
                pc->tiledInputMode = pDecContainer->tiledReferenceEnable;
                pc->progressiveSequence = 1;

                /*if:
                 * B pictures allowed and non B picture OR
                 * B pictures not allowed */
                if(pDecContainer->FrameDesc.picCodingType != RV_B_PIC)
                {
#ifdef _DEC_PP_USAGE
                    RvDecPpUsagePrint(pDecContainer, DECPP_PARALLEL,
                                         pDecContainer->StrmStorage.work0,
                                         0,
                                         pDecContainer->StrmStorage.
                                         pPicBuf[pDecContainer->StrmStorage.
                                                 work0].picId);
#endif
                    pc->inputBusLuma =
                        pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                           StrmStorage.work0].
                        data.busAddress;

                    pc->inputBusChroma =
                        pc->inputBusLuma +
                        ((pDecContainer->FrameDesc.frameWidth *
                          pDecContainer->FrameDesc.frameHeight) << 8);

                    pc->inwidth = pc->croppedW =
                        pDecContainer->FrameDesc.frameWidth << 4;
                    pc->inheight = pc->croppedH =
                        pDecContainer->FrameDesc.frameHeight << 4;
                    {
                        u32 value = GetDecRegister(pDecContainer->rvRegs,
                                                   HWIF_DEC_OUT_ENDIAN);

                        pc->littleEndian =
                            (value == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pc->wordSwap =
                        GetDecRegister(pDecContainer->rvRegs,
                                       HWIF_DEC_OUTSWAP32_E) ? 1 : 0;

                    RVDEC_API_DEBUG(("sending NON B to pp\n"));
                    indexForPp = pDecContainer->StrmStorage.work0;

                    pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp = 2;
                }
                else
                {
                    RVDEC_API_DEBUG(("PIPELINE OFF, DON*T SEND B TO PP\n"));
                    indexForPp = pDecContainer->StrmStorage.workOut;
                    indexForPp = RV_BUFFER_UNDEFINED;
                    pc->inputBusLuma = 0;
                }
            }
            else
            {
                pc->inputBusLuma = 0;
            }
        }
        else    /* pipeline */
        {
#ifdef _DEC_PP_USAGE
            RvDecPpUsagePrint(pDecContainer, DECPP_PIPELINED,
                                 pDecContainer->StrmStorage.workOut, 0,
                                 pDecContainer->StrmStorage.
                                 pPicBuf[pDecContainer->StrmStorage.
                                         workOut].picId);
#endif
            pc->inputBusLuma = pc->inputBusChroma = 0;
            indexForPp = pDecContainer->StrmStorage.workOut;
            pc->tiledInputMode = pDecContainer->tiledReferenceEnable;
            pc->progressiveSequence = 1;

            pc->inwidth = pc->croppedW =
                pDecContainer->FrameDesc.frameWidth << 4;
            pc->inheight = pc->croppedH =
                pDecContainer->FrameDesc.frameHeight << 4;
        }

        /* start PP */
        if(((pc->inputBusLuma && !pc->usePipeline) ||
            (!pc->inputBusLuma && pc->usePipeline))
           && pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp)
        {
            RVDEC_API_DEBUG(("pprun, pipeline %s\n",
                                pc->usePipeline ? "on" : "off"));

            /* filter needs pipeline to work! */
            RVDEC_API_DEBUG(("Filter %s# \n",
                                pDecContainer->ApiStorage.
                                disableFilter ? "off" : "on"));

            /* always disabled in MPEG-2 */
            pDecContainer->ApiStorage.disableFilter = 1;

            SetDecRegister(pDecContainer->rvRegs, HWIF_FILTERING_DIS,
                           pDecContainer->ApiStorage.disableFilter);

            if(pc->usePipeline) /*CHECK !! */
            {
                if(pDecContainer->FrameDesc.picCodingType == RV_B_PIC)
                {

                    SetDecRegister(pDecContainer->rvRegs, HWIF_DEC_OUT_DIS,
                                   1);

                    /*frame or top or bottom */
                    ASSERT((pc->picStruct == DECPP_PIC_FRAME_OR_TOP_FIELD) ||
                           (pc->picStruct == DECPP_PIC_BOT_FIELD));
                }
            }

            /*ASSERT(indexForPp != pDecContainer->StrmStorage.workOut); */
            ASSERT(indexForPp != RV_BUFFER_UNDEFINED);

            RVDEC_API_DEBUG(("send %d %d %d %d, indexForPp %d\n",
                                pDecContainer->StrmStorage.pPicBuf[0].sendToPp,
                                pDecContainer->StrmStorage.pPicBuf[1].sendToPp,
                                pDecContainer->StrmStorage.pPicBuf[2].sendToPp,
                                pDecContainer->StrmStorage.pPicBuf[3].sendToPp,
                                indexForPp));

            pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp = 0;

            pDecContainer->PPRun(pDecContainer->ppInstance, pc);

            pc->ppStatus = DECPP_RUNNING;
        }
        pDecContainer->StrmStorage.previousModeFull = 0;
    }

    if( pDecContainer->FrameDesc.picCodingType != RV_B_PIC )
    {
        pc->prevAnchorDisplayIndex = nextBufferIndex;
    }

}

/*------------------------------------------------------------------------------

    Function name: RvPpMultiBufferInit

    Functional description:
        Modify state of pp output buffering.

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
void RvPpMultiBufferInit(DecContainer * pDecCont)
{
    DecPpQuery *pq = &pDecCont->ppConfigQuery;
    DecPpInterface *pc = &pDecCont->ppControl;

    if(pq->multiBuffer && 
       !pDecCont->workarounds.rv.multibuffer )
    {
        if(!pq->pipelineAccepted)
        {
            RVDEC_API_DEBUG(("MULTIBUFFER_SEMIMODE\n"));
            pc->multiBufStat = MULTIBUFFER_SEMIMODE;
        }
        else
        {
            RVDEC_API_DEBUG(("MULTIBUFFER_FULLMODE\n"));
            pc->multiBufStat = MULTIBUFFER_FULLMODE;
        }

        pc->bufferIndex = 1;
    }
    else
    {
        pc->multiBufStat = MULTIBUFFER_DISABLED;
    }

}

/*------------------------------------------------------------------------------

    Function name: RvPpMultiBufferSetup

    Functional description:
        Modify state of pp output buffering.

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/
void RvPpMultiBufferSetup(DecContainer * pDecCont, u32 pipelineOff)
{

    if(pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED)
    {
        RVDEC_API_DEBUG(("MULTIBUFFER_DISABLED\n"));
        return;
    }

    if(pipelineOff && pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE)
    {
        RVDEC_API_DEBUG(("MULTIBUFFER_SEMIMODE\n"));
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_SEMIMODE;
    }

    if(!pipelineOff &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_SEMIMODE))
    {
        RVDEC_API_DEBUG(("MULTIBUFFER_FULLMODE\n"));
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_FULLMODE;
    }

    if(pDecCont->ppControl.multiBufStat == MULTIBUFFER_UNINIT)
        RvPpMultiBufferInit(pDecCont);

}

/*------------------------------------------------------------------------------

    Function name: RvDecRunFullmode

    Functional description:

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
void RvDecRunFullmode(DecContainer * pDecCont)
{
    u32 indexForPp = RV_BUFFER_UNDEFINED;
    DecPpInterface *pc = &pDecCont->ppControl;

#ifdef _DEC_PP_USAGE
    RvDecPpUsagePrint(pDecCont, DECPP_PIPELINED,
                         pDecCont->StrmStorage.workOut, 0,
                         pDecCont->StrmStorage.pPicBuf[pDecCont->
                                                       StrmStorage.
                                                       workOut].picId);
#endif

    if(!pDecCont->StrmStorage.previousModeFull &&
       pDecCont->FrameDesc.frameNumber)
    {
        if(pDecCont->FrameDesc.picCodingType == RV_B_PIC)
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->
                                          StrmStorage.work0].sendToPp = 0;
            pDecCont->StrmStorage.pPicBuf[pDecCont->
                                          StrmStorage.work1].sendToPp = 0;
        }
        else
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->
                                          StrmStorage.work0].sendToPp = 0;
        }
    }

    if(pDecCont->FrameDesc.picCodingType == RV_B_PIC)
    {
        pDecCont->StrmStorage.previousB = 1;
    }
    else
    {
        pDecCont->StrmStorage.previousB = 0;
    }

    indexForPp = pDecCont->StrmStorage.workOut;
    pc->inputBusLuma = pDecCont->StrmStorage.pPicBuf[pDecCont->
                                                     StrmStorage.workOut].
        data.busAddress;

    pc->inputBusChroma = pc->inputBusLuma +
        ((pDecCont->FrameDesc.frameWidth *
          pDecCont->FrameDesc.frameHeight) << 8);

    pc->tiledInputMode = pDecCont->tiledReferenceEnable;
    pc->progressiveSequence = 1;
    pc->inwidth = pc->croppedW = pDecCont->FrameDesc.frameWidth << 4;
    pc->inheight = pc->croppedH = pDecCont->FrameDesc.frameHeight << 4;

    {
        if(pDecCont->FrameDesc.picCodingType == RV_B_PIC)
            SetDecRegister(pDecCont->rvRegs, HWIF_DEC_OUT_DIS, 1);

        /* always disabled in MPEG-2 */
        pDecCont->ApiStorage.disableFilter = 1;

        SetDecRegister(pDecCont->rvRegs, HWIF_FILTERING_DIS,
                       pDecCont->ApiStorage.disableFilter);
    }

    ASSERT(indexForPp != RV_BUFFER_UNDEFINED);

    RVDEC_API_DEBUG(("send %d %d %d %d, indexForPp %d\n",
                        pDecCont->StrmStorage.pPicBuf[0].sendToPp,
                        pDecCont->StrmStorage.pPicBuf[1].sendToPp,
                        pDecCont->StrmStorage.pPicBuf[2].sendToPp, 
                        pDecCont->StrmStorage.pPicBuf[3].sendToPp, 
                        indexForPp));

    ASSERT(pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp == 1);
    pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp = 0;

    pDecCont->PPRun(pDecCont->ppInstance, pc);

    pc->ppStatus = DECPP_RUNNING;
}

/*------------------------------------------------------------------------------

    Function name: RvDecPeek

    Functional description:
        Retrieve last decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct

    Output:
        pPicture Decoder output picture.

    Return values:
        RVDEC_OK         No picture available.
        RVDEC_PIC_RDY    Picture ready.

------------------------------------------------------------------------------*/
RvDecRet RvDecPeek(RvDecInst decInst, RvDecPicture * pPicture)
{
    /* Variables */
    RvDecRet returnValue = RVDEC_PIC_RDY;
    DecContainer *pDecCont;
    picture_t *pPic;
    u32 picIndex = RV_BUFFER_UNDEFINED;

    /* Code */
    RV_API_TRC("\nRvDecPeek#");

    /* Check that function input parameters are valid */
    if(pPicture == NULL)
    {
        RV_API_TRC("RvDecPeek# ERROR: pPicture is NULL");
        return (RVDEC_PARAM_ERROR);
    }

    pDecCont = (DecContainer *) decInst;

    /* Check if decoder is in an incorrect mode */
    if(decInst == NULL || pDecCont->ApiStorage.DecStat == UNINIT)
    {
        RV_API_TRC("RvDecPeek# ERROR: Decoder not initialized");
        return (RVDEC_NOT_INITIALIZED);
    }

    /* Nothing to send out */
    if(!pDecCont->StrmStorage.outCount)
    {
        (void) G1DWLmemset(pPicture, 0, sizeof(RvDecPicture));
        returnValue = RVDEC_OK;
    }
    else
    {
        picIndex = pDecCont->StrmStorage.workOut;

        RvFillPicStruct(pPicture, pDecCont, picIndex);

    }

    return returnValue;
}

/*------------------------------------------------------------------------------

    Function name: RvDecSetLatency

    Functional description:
        set current video latencyMS to vdec driver for vpu dvfs

    Input:
        decInst     Reference to decoder instance.
        latencyMS   


    Return values:
        AVSDEC_OK;

------------------------------------------------------------------------------*/
RvDecRet RvDecSetLatency(RvDecInst decInst, int latencyMS)
{
    DecContainer *pDecCont = (DecContainer *) decInst;

    DWLSetLatency(pDecCont->dwl, latencyMS);

    return RVDEC_OK;
}
