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
--  Abstract : The API module C-functions of 8190 AVS Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: avsdecapi.c,v $
--  $Date: 2011/02/04 12:41:37 $
--  $Revision: 1.124 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "avsdecapi.h"
#include "basetype.h"
#include "avs_cfg.h"
#include "avs_container.h"
#include "avs_utils.h"
#include "avs_strm.h"
#include "avsdecapi_internal.h"
#include "dwl.h"
#include "regdrv.h"
#include "avs_headers.h"
#include "deccfg.h"
#include "refbuffer.h"
#include "tiledref.h"

#ifdef AVSDEC_TRACE
#define AVS_API_TRC(str)    AvsDecTrace((str))
#else
#define AVS_API_TRC(str)
#endif

#define AVS_BUFFER_UNDEFINED    16

#define ID8170_DEC_TIMEOUT        0xFFU
#define ID8170_DEC_SYSTEM_ERROR   0xFEU
#define ID8170_DEC_HW_RESERVED    0xFDU

#define AVSDEC_IS_FIELD_OUTPUT \
    !pDecCont->Hdrs.progressiveSequence && !pDecCont->ppConfigQuery.deinterlace

#define AVSDEC_NON_PIPELINE_AND_B_PICTURE \
    ((!pDecCont->ppConfigQuery.pipelineAccepted || !pDecCont->Hdrs.progressiveSequence) \
    && pDecCont->Hdrs.picCodingType == BFRAME)

void AvsRefreshRegs(DecContainer * pDecCont);
void AvsFlushRegs(DecContainer * pDecCont);
static u32 AvsHandleVlcModeError(DecContainer * pDecCont, u32 picNum);
static void AvsHandleFrameEnd(DecContainer * pDecCont);
static u32 RunDecoderAsic(DecContainer * pDecContainer, u32 strmBusAddress);
static void AvsFillPicStruct(AvsDecPicture * pPicture,
                               DecContainer * pDecCont, u32 picIndex);
static u32 AvsSetRegs(DecContainer * pDecContainer, u32 strmBusAddress);
static void AvsDecSetupDeinterlace(DecContainer * pDecCont);
static void AvsDecPrepareFieldProcessing(DecContainer * pDecCont);
static void AvsSetupRefBuffer(DecContainer * pDecCont );
static u32 AvsCheckFormatSupport(void);
static void AvsPpControl(DecContainer * pDecContainer, u32 pipelineOff);
static void AvsPpMultiBufferInit(DecContainer * pDecCont);
static void AvsPpMultiBufferSetup(DecContainer * pDecCont, u32 pipelineOff);
static void AvsDecRunFullmode(DecContainer * pDecCont);

#define DEC_X170_MODE_AVS   11
/*------------------------------------------------------------------------------
       Version Information - DO NOT CHANGE!
------------------------------------------------------------------------------*/

#define AVSDEC_MAJOR_VERSION 1
#define AVSDEC_MINOR_VERSION 1

#define AVSDEC_BUILD_MAJOR 0
#define AVSDEC_BUILD_MINOR 81
#define AVSDEC_SW_BUILD ((AVSDEC_BUILD_MAJOR * 1000) + AVSDEC_BUILD_MINOR)

/*------------------------------------------------------------------------------

    Function: AvsDecGetAPIVersion

        Functional description:
            Return version information of API

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

AvsDecApiVersion AvsDecGetAPIVersion()
{
    AvsDecApiVersion ver;

    ver.major = AVSDEC_MAJOR_VERSION;
    ver.minor = AVSDEC_MINOR_VERSION;

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: AvsDecGetBuild

        Functional description:
            Return build information of SW and HW

        Returns:
            AvsDecBuild

------------------------------------------------------------------------------*/

AvsDecBuild AvsDecGetBuild(void)
{
    AvsDecBuild ver;
    DWLHwConfig_t hwCfg;

    AVS_API_TRC("AvsDecGetBuild#");

    G1DWLmemset(&ver, 0, sizeof(ver));

    ver.swBuild = AVSDEC_SW_BUILD;
    ver.hwBuild = G1DWLReadAsicID();

    DWLReadAsicConfig(&hwCfg);

    SET_DEC_BUILD_SUPPORT(ver.hwConfig, hwCfg);
    
    AVS_API_TRC("AvsDecGetBuild# OK\n");

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: AvsDecInit()

        Functional description:
            Initialize decoder software. Function reserves memory for the
            decoder instance.

        Inputs:
            useVideoFreezeConcealment
                            flag to enable error concealment method where
                            decoding starts at next intra picture after error
                            in bitstream.

		Outputs:
            pDecInst         pointer to initialized instance is stored here

        Returns:
            AVSDEC_OK       successfully initialized the instance
            AVSDEC_MEM_FAIL memory allocation failed

------------------------------------------------------------------------------*/
AvsDecRet AvsDecInit(AvsDecInst * pDecInst,
                     u32 useVideoFreezeConcealment, 
                     u32 numFrameBuffers,
                     DecRefFrmFormat referenceFrameFormat,
                     u32 mmuEnable)
{
    /*@null@ */ DecContainer *pDecCont;
    /*@null@ */ const void *dwl;
    u32 i = 0;

    G1DWLInitParam_t dwlInit;
    DWLHwConfig_t config;

    AVS_API_TRC("AvsDecInit#");
    AVSDEC_DEBUG(("AvsAPI_DecoderInit#"));

    /* check that right shift on negative numbers is performed signed */
    /*lint -save -e* following check causes multiple lint messages */
    if(((-1) >> 1) != (-1))
    {
        AVSDEC_DEBUG(("AVSDecInit# ERROR: Right shift is not signed"));
        return (AVSDEC_INITFAIL);
    }
    /*lint -restore */

    if(pDecInst == NULL)
    {
        AVSDEC_DEBUG(("AVSDecInit# ERROR: pDecInst == NULL"));
        return (AVSDEC_PARAM_ERROR);
    }

    *pDecInst = NULL;

    /* check that AVS decoding supported in HW */
    if(AvsCheckFormatSupport())
    {
        AVSDEC_DEBUG(("AVSDecInit# ERROR: AVS not supported in HW\n"));
        return AVSDEC_FORMAT_NOT_SUPPORTED;
    }

    dwlInit.clientType = DWL_CLIENT_TYPE_AVS_DEC;
    dwlInit.mmuEnable = mmuEnable;

    dwl = G1DWLInit(&dwlInit);

    if(dwl == NULL)
    {
        AVSDEC_DEBUG(("AVSDecInit# ERROR: DWL Init failed"));
        return (AVSDEC_DWL_ERROR);
    }

    AVSDEC_DEBUG(("size of DecContainer %d \n", sizeof(DecContainer)));
    pDecCont = (DecContainer *) G1DWLmalloc(sizeof(DecContainer));

    if(pDecCont == NULL)
    {
        (void) G1DWLRelease(dwl);
        return (AVSDEC_MEMFAIL);
    }

    /* set everything initially zero */
    (void) G1DWLmemset(pDecCont, 0, sizeof(DecContainer));

    pDecCont->dwl = dwl;

    if(numFrameBuffers > 16)   numFrameBuffers = 16;
    pDecCont->StrmStorage.maxNumBuffers = numFrameBuffers;

    AvsAPI_InitDataStructures(pDecCont);

    pDecCont->ApiStorage.DecStat = INITIALIZED;
    pDecCont->ApiStorage.firstField = 1;

    *pDecInst = (DecContainer *) pDecCont;

    for(i = 0; i < DEC_X170_REGISTERS; i++)
        pDecCont->avsRegs[i] = 0;

    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_OUT_ENDIAN,
                   DEC_X170_OUTPUT_PICTURE_ENDIAN);
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_IN_ENDIAN,
                   DEC_X170_INPUT_DATA_ENDIAN);
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_STRENDIAN_E,
                   DEC_X170_INPUT_STREAM_ENDIAN);
    /*
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_OUT_TILED_E,
                   DEC_X170_OUTPUT_FORMAT);
                   */
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_MAX_BURST,
                   DEC_X170_BUS_BURST_LENGTH);
    if ((G1DWLReadAsicID() >> 16) == 0x8170U)
    {
        SetDecRegister(pDecCont->avsRegs, HWIF_PRIORITY_MODE,
                       DEC_X170_ASIC_SERVICE_PRIORITY);
    }
    else
    {
        SetDecRegister(pDecCont->avsRegs, HWIF_DEC_SCMD_DIS,
            DEC_X170_SCMD_DISABLE);
    }
    DEC_SET_APF_THRESHOLD(pDecCont->avsRegs);
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_LATENCY,
                   DEC_X170_LATENCY_COMPENSATION);
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_DATA_DISC_E,
                   DEC_X170_DATA_DISCARD_ENABLE);
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_OUTSWAP32_E,
                   DEC_X170_OUTPUT_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_INSWAP32_E,
                   DEC_X170_INPUT_DATA_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_STRSWAP32_E,
                   DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);

#if( DEC_X170_HW_TIMEOUT_INT_ENA  != 0)
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_TIMEOUT_E, 1);
#else
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_TIMEOUT_E, 0);
#endif

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_CLK_GATE_E, 1);
#else
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_CLK_GATE_E, 0);
#endif

#if( DEC_X170_USING_IRQ  == 0)
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_IRQ_DIS, 1);
#else
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_IRQ_DIS, 0);
#endif
    
    /* set AXI RW IDs */
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_AXI_RD_ID,
        (DEC_X170_AXI_ID_R & 0xFFU));
    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_AXI_WR_ID,
        (DEC_X170_AXI_ID_W & 0xFFU));

    /* Set prediction filter taps */
    SetDecRegister(pDecCont->avsRegs, HWIF_PRED_BC_TAP_0_0,-1);
    SetDecRegister(pDecCont->avsRegs, HWIF_PRED_BC_TAP_0_1, 5);
    SetDecRegister(pDecCont->avsRegs, HWIF_PRED_BC_TAP_0_2, 5);
    SetDecRegister(pDecCont->avsRegs, HWIF_PRED_BC_TAP_0_3,-1);
    SetDecRegister(pDecCont->avsRegs, HWIF_PRED_BC_TAP_1_0, 1);
    SetDecRegister(pDecCont->avsRegs, HWIF_PRED_BC_TAP_1_1, 7);
    SetDecRegister(pDecCont->avsRegs, HWIF_PRED_BC_TAP_1_2, 7);
    SetDecRegister(pDecCont->avsRegs, HWIF_PRED_BC_TAP_1_3, 1);

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
            return AVSDEC_FORMAT_NOT_SUPPORTED;
        }
        pDecCont->tiledModeSupport = config.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;
    pDecCont->StrmStorage.intraFreeze = useVideoFreezeConcealment;
    pDecCont->StrmStorage.pictureBroken = 0;

    AVSDEC_DEBUG(("Container 0x%x\n", (u32) pDecCont));
    AVS_API_TRC("AvsDecInit: OK\n");

    return (AVSDEC_OK);
}

/*------------------------------------------------------------------------------

    Function: AvsDecGetInfo()

        Functional description:
            This function provides read access to decoder information. This
            function should not be called before AvsDecDecode function has
            indicated that headers are ready.

        Inputs:
            decInst     decoder instance

        Outputs:
            pDecInfo    pointer to info struct where data is written

        Returns:
            AVSDEC_OK            success
            AVSDEC_PARAM_ERROR     invalid parameters

------------------------------------------------------------------------------*/
AvsDecRet AvsDecGetInfo(AvsDecInst decInst, AvsDecInfo * pDecInfo)
{

#define API_STOR ((DecContainer *)decInst)->ApiStorage
#define DEC_STST ((DecContainer *)decInst)->StrmStorage
#define DEC_HDRS ((DecContainer *)decInst)->Hdrs
#define DEC_REGS ((DecContainer *)decInst)->avsRegs

    AVS_API_TRC("AvsDecGetInfo#");

    if(decInst == NULL || pDecInfo == NULL)
    {
        return AVSDEC_PARAM_ERROR;
    }

    pDecInfo->multiBuffPpSize = 2;

    if(API_STOR.DecStat == UNINIT || API_STOR.DecStat == INITIALIZED)
    {
        return AVSDEC_HDRS_NOT_RDY;
    }

    pDecInfo->frameWidth = DEC_STST.frameWidth << 4;
    pDecInfo->frameHeight = DEC_STST.frameHeight << 4;

    pDecInfo->codedWidth = DEC_HDRS.horizontalSize;
    pDecInfo->codedHeight = DEC_HDRS.verticalSize;

    pDecInfo->profileId = DEC_HDRS.profileId;
    pDecInfo->levelId = DEC_HDRS.levelId;
    pDecInfo->videoRange = DEC_HDRS.sampleRange;
    pDecInfo->videoFormat = DEC_HDRS.videoFormat;
    pDecInfo->interlacedSequence = !DEC_HDRS.progressiveSequence;

    AvsDecAspectRatio((DecContainer *) decInst, pDecInfo);

    if(!DEC_HDRS.progressiveSequence)
    {
        pDecInfo->outputFormat = AVSDEC_SEMIPLANAR_YUV420;
    }
    else
    {	
	// +Leo@2011/10/25: here should be tiledModeSupport.
#if 0	
        pDecInfo->outputFormat = 
            ((DecContainer *)decInst)->tiledReferenceEnable ? 
            AVSDEC_TILED_YUV420 : AVSDEC_SEMIPLANAR_YUV420;
#else
        pDecInfo->outputFormat = 
            ((DecContainer *)decInst)->tiledModeSupport ? 
            AVSDEC_TILED_YUV420 : AVSDEC_SEMIPLANAR_YUV420;	
#endif
	// -Leo@2011/10/25
    }

    AVS_API_TRC("AvsDecGetInfo: OK");
    return (AVSDEC_OK);

#undef API_STOR
#undef DEC_STST
#undef DEC_HDRS
#undef DEC_REGS

}

/*------------------------------------------------------------------------------

    Function: AvsDecDecode

        Functional description:
            Decode stream data. Calls StrmDec_Decode to do the actual decoding.

        Input:
            decInst     decoder instance
            pInput      pointer to input struct

        Outputs:
            pOutput     pointer to output struct

        Returns:
            AVSDEC_NOT_INITIALIZED   decoder instance not initialized yet
            AVSDEC_PARAM_ERROR       invalid parameters

            AVSDEC_STRM_PROCESSED    stream buffer decoded
            AVSDEC_HDRS_RDY          headers decoded
            AVSDEC_PIC_DECODED       decoding of a picture finished
            AVSDEC_STRM_ERROR        serious error in decoding, no
                                       valid parameter sets available
                                       to decode picture data

------------------------------------------------------------------------------*/

AvsDecRet AvsDecDecode(AvsDecInst decInst,
                           AvsDecInput * pInput, AvsDecOutput * pOutput)
{
#define API_STOR ((DecContainer *)decInst)->ApiStorage
#define DEC_STRM ((DecContainer *)decInst)->StrmDesc

    DecContainer *pDecCont;
    AvsDecRet internalRet;
    u32 strmDecResult;
    u32 asicStatus;
    i32 ret = 0;
    u32 fieldRdy = 0;
    u32 errorConcealment = 0;

    AVS_API_TRC("\nAvsDecDecode#");

    if(pInput == NULL || pOutput == NULL || decInst == NULL)
    {
        AVS_API_TRC("AvsDecDecode# PARAM_ERROR\n");
        return AVSDEC_PARAM_ERROR;
    }

    pDecCont = ((DecContainer *) decInst);

    /*
     *  Check if decoder is in an incorrect mode
     */
    if(API_STOR.DecStat == UNINIT)
    {
        AVS_API_TRC("AvsDecDecode: NOT_INITIALIZED\n");
        return AVSDEC_NOT_INITIALIZED;
    }

    if(pInput->dataLen == 0 ||
       pInput->dataLen > DEC_X170_MAX_STREAM ||
       pInput->pStream == NULL || pInput->streamBusAddress == 0)
    {
        AVS_API_TRC("AvsDecDecode# PARAM_ERROR\n");
        return AVSDEC_PARAM_ERROR;
    }

    /* If we have set up for delayed resolution change, do it here */
    if(pDecCont->StrmStorage.newHeadersChangeResolution)
    {
        pDecCont->StrmStorage.newHeadersChangeResolution = 0;
        pDecCont->Hdrs.horizontalSize = pDecCont->tmpHdrs.horizontalSize;
        pDecCont->Hdrs.verticalSize = pDecCont->tmpHdrs.verticalSize;
        /* Set rest of parameters just in case */
        pDecCont->Hdrs.aspectRatio = pDecCont->tmpHdrs.aspectRatio;
        pDecCont->Hdrs.frameRateCode = pDecCont->tmpHdrs.frameRateCode;
        pDecCont->Hdrs.bitRateValue = pDecCont->tmpHdrs.bitRateValue;
        pDecCont->StrmStorage.frameWidth =
            (pDecCont->Hdrs.horizontalSize + 15) >> 4;
        if(pDecCont->Hdrs.progressiveSequence)
            pDecCont->StrmStorage.frameHeight =
                (pDecCont->Hdrs.verticalSize + 15) >> 4;
        else
            pDecCont->StrmStorage.frameHeight =
                2 * ((pDecCont->Hdrs.verticalSize + 31) >> 5);
        pDecCont->StrmStorage.totalMbsInFrame =
            (pDecCont->StrmStorage.frameWidth *
             pDecCont->StrmStorage.frameHeight);
    }

    if(API_STOR.DecStat == HEADERSDECODED)
    {
        AvsFreeBuffers(pDecCont);
        /* If buffers not allocated */
        if(!pDecCont->StrmStorage.pPicBuf[0].data.busAddress)
        {
            AVSDEC_DEBUG(("Allocate buffers\n"));
            internalRet = AvsAllocateBuffers(pDecCont);
            if(internalRet != AVSDEC_OK)
            {
                AVSDEC_DEBUG(("ALLOC BUFFER FAIL\n"));
                AVS_API_TRC("AvsDecDecode# MEMFAIL\n");
                return (internalRet);
            }
        }

        /* Headers ready now, mems allocated, decoding can start */
        API_STOR.DecStat = STREAMDECODING;

    }

/*
 *  Update stream structure
 */
    DEC_STRM.pStrmBuffStart = pInput->pStream;
    DEC_STRM.pStrmCurrPos = pInput->pStream;
    DEC_STRM.bitPosInWord = 0;
    DEC_STRM.strmBuffSize = pInput->dataLen;
    DEC_STRM.strmBuffReadBits = 0;

#ifdef _DEC_PP_USAGE
    pDecCont->StrmStorage.latestId = pInput->picId;
#endif
    do
    {
        AVSDEC_DEBUG(("Start Decode\n"));
        /* run SW if HW is not in the middle of processing a picture
         * (indicated as HW_PIC_STARTED decoder status) */
        if(API_STOR.DecStat != HW_PIC_STARTED)
        {
            strmDecResult = AvsStrmDec_Decode(pDecCont);
            /* TODO: voiko olla pariton field, jos voi -> tänne release
             * pp ja hw */
            switch (strmDecResult)
            {
            case DEC_PIC_HDR_RDY:
                /* if type inter predicted and no reference -> error */
                if((pDecCont->Hdrs.picCodingType == PFRAME &&
                    pDecCont->StrmStorage.work0 == INVALID_ANCHOR_PICTURE) ||
                   (pDecCont->Hdrs.picCodingType == BFRAME &&
                    (pDecCont->StrmStorage.work0 == INVALID_ANCHOR_PICTURE ||
                     pDecCont->StrmStorage.skipB ||
                     pInput->skipNonReference)) ||
                   (pDecCont->Hdrs.picCodingType == PFRAME &&
                    pDecCont->StrmStorage.pictureBroken &&
                    pDecCont->StrmStorage.intraFreeze) )
                {
                    if(pDecCont->StrmStorage.skipB ||
                       pInput->skipNonReference)
                    {
                        AVS_API_TRC("AvsDecDecode# AVSDEC_NONREF_PIC_SKIPPED\n");
                    }
                    ret = AvsHandleVlcModeError(pDecCont, pInput->picId);
                    errorConcealment = 1;
                }
                else
                    API_STOR.DecStat = HW_PIC_STARTED;
                break;

            case DEC_PIC_SUPRISE_B:
                /* Handle suprise B */
                pDecCont->Hdrs.lowDelay = 0;

                AvsDecBufferPicture(pDecCont,
                                      pInput->picId, 1, 0,
                                      AVSDEC_PIC_DECODED, 0);

                ret = AvsHandleVlcModeError(pDecCont, pInput->picId);
                errorConcealment = 1;
                break;

            case DEC_PIC_HDR_RDY_ERROR:
                ret = AvsHandleVlcModeError(pDecCont, pInput->picId);
                errorConcealment = 1;
                break;

            case DEC_HDRS_RDY:

                internalRet = AvsDecCheckSupport(pDecCont);
                if(internalRet != AVSDEC_OK)
                {
                    pDecCont->StrmStorage.strmDecReady = FALSE;
                    pDecCont->StrmStorage.validSequence = 0;
                    API_STOR.DecStat = INITIALIZED;
                    return internalRet;
                }

                if(pDecCont->ApiStorage.firstHeaders)
                {
                    pDecCont->ApiStorage.firstHeaders = 0;

                    SetDecRegister(pDecCont->avsRegs, HWIF_PIC_MB_WIDTH,
                                   pDecCont->StrmStorage.frameWidth);

                    SetDecRegister(pDecCont->avsRegs, HWIF_DEC_MODE,
                                   DEC_X170_MODE_AVS);

                    if (pDecCont->refBufSupport)
                    {
                        RefbuInit(&pDecCont->refBufferCtrl,
                                  DEC_X170_MODE_AVS,
                                  pDecCont->StrmStorage.frameWidth,
                                  pDecCont->StrmStorage.frameHeight,
                                  pDecCont->refBufSupport);
                    }
                }

                API_STOR.DecStat = HEADERSDECODED;

                AVSDEC_DEBUG(("HDRS_RDY\n"));
                ret = AVSDEC_HDRS_RDY;
                break;

            default:
                ASSERT(strmDecResult == DEC_END_OF_STREAM);
                if (pDecCont->StrmStorage.newHeadersChangeResolution)
                    ret = AVSDEC_PIC_DECODED;
                else
                    ret = AVSDEC_STRM_PROCESSED;
                break;
            }
        }

        /* picture header properly decoded etc -> start HW */
        if(API_STOR.DecStat == HW_PIC_STARTED)
        {
            if(pDecCont->ApiStorage.firstField &&
               !pDecCont->asicRunning)
            {
                pDecCont->StrmStorage.workOut = BqueueNext( 
                        &pDecCont->StrmStorage.bq,
                        pDecCont->StrmStorage.work0,
                        pDecCont->StrmStorage.work1, 
                        BQUEUE_UNUSED,
                        pDecCont->Hdrs.picCodingType == BFRAME );
            }

            asicStatus = RunDecoderAsic(pDecCont, pInput->streamBusAddress);

            if(asicStatus == ID8170_DEC_TIMEOUT)
            {
                ret = AVSDEC_HW_TIMEOUT;
            }
            else if(asicStatus == ID8170_DEC_SYSTEM_ERROR)
            {
                ret = AVSDEC_SYSTEM_ERROR;
            }
            else if(asicStatus == ID8170_DEC_HW_RESERVED)
            {
                ret = AVSDEC_HW_RESERVED;
            }
            else if(asicStatus & AVS_DEC_X170_IRQ_TIMEOUT)
            {
                AVSDEC_DEBUG(("IRQ TIMEOUT IN HW\n"));
                ret = AvsHandleVlcModeError(pDecCont, pInput->picId);
                errorConcealment = 1;
            }
            else if(asicStatus & AVS_DEC_X170_IRQ_BUS_ERROR)
            {
                ret = AVSDEC_HW_BUS_ERROR;
            }
            else if(asicStatus & AVS_DEC_X170_IRQ_STREAM_ERROR)
            {
                AVSDEC_DEBUG(("STREAM ERROR IN HW\n"));
                ret = AvsHandleVlcModeError(pDecCont, pInput->picId);
                errorConcealment = 1;
                pDecCont->StrmStorage.prevPicCodingType =
                    pDecCont->Hdrs.picCodingType;
            }
            else if(asicStatus & AVS_DEC_X170_IRQ_BUFFER_EMPTY)
            {
                AvsDecPreparePicReturn(pDecCont);
                ret = AVSDEC_STRM_PROCESSED;

            }
            /* HW finished decoding a picture */
            else if(asicStatus & AVS_DEC_X170_IRQ_DEC_RDY)
            {

                if(pDecCont->Hdrs.pictureStructure == FRAMEPICTURE ||
                   !pDecCont->ApiStorage.firstField)
                {
                    fieldRdy = 0;
                    pDecCont->StrmStorage.frameNumber++;

                    AvsHandleFrameEnd(pDecCont);

                    AvsDecBufferPicture(pDecCont,
                                          pInput->picId,
                                          pDecCont->Hdrs.
                                          picCodingType == BFRAME,
                                          pDecCont->Hdrs.
                                          picCodingType == PFRAME,
                                          AVSDEC_PIC_DECODED, 0);

                    ret = AVSDEC_PIC_DECODED;

                    pDecCont->ApiStorage.firstField = 1;
                    if(pDecCont->Hdrs.picCodingType != BFRAME)
                    {
                        pDecCont->StrmStorage.work1 =
                            pDecCont->StrmStorage.work0;
                        pDecCont->StrmStorage.work0 =
                            pDecCont->StrmStorage.workOut;
                        if( pDecCont->StrmStorage.skipB )
                            pDecCont->StrmStorage.skipB--;
                    }
                    pDecCont->StrmStorage.prevPicCodingType =
                        pDecCont->Hdrs.picCodingType;
                    if( pDecCont->Hdrs.picCodingType != BFRAME )
                        pDecCont->StrmStorage.prevPicStructure =
                            pDecCont->Hdrs.pictureStructure;

                    if( pDecCont->Hdrs.picCodingType == IFRAME )
                        pDecCont->StrmStorage.pictureBroken = 0;
                }
                else
                {
                    fieldRdy = 1;
                    AvsHandleFrameEnd(pDecCont);
                    pDecCont->ApiStorage.firstField = 0;
                    if((u32) (pDecCont->StrmDesc.pStrmCurrPos -
                              pDecCont->StrmDesc.pStrmBuffStart) >=
                       pInput->dataLen)
                    {
                        ret = AVSDEC_STRM_PROCESSED;
                    }
                }
                pDecCont->StrmStorage.validPicHeader = HANTRO_FALSE;

                /* handle first field indication */
                if(!pDecCont->Hdrs.progressiveSequence)
                {
                    if(pDecCont->Hdrs.pictureStructure != FRAMEPICTURE)
                        pDecCont->StrmStorage.fieldIndex++;
                    else
                        pDecCont->StrmStorage.fieldIndex = 1;

                }

                AvsDecPreparePicReturn(pDecCont);
            }
            else
            {
                ASSERT(0);
            }
            if(ret != AVSDEC_STRM_PROCESSED && !fieldRdy)
            {
                API_STOR.DecStat = STREAMDECODING;
            }

        }
    }
    while(ret == 0);

    if( errorConcealment && pDecCont->Hdrs.picCodingType != BFRAME )
    {
        pDecCont->StrmStorage.pictureBroken = 1;
    }

    AVS_API_TRC("AvsDecDecode: Exit\n");
    pOutput->pStrmCurrPos = pDecCont->StrmDesc.pStrmCurrPos;
    pOutput->strmCurrBusAddress = pInput->streamBusAddress +
        (pDecCont->StrmDesc.pStrmCurrPos - pDecCont->StrmDesc.pStrmBuffStart);
    pOutput->dataLeft = pDecCont->StrmDesc.strmBuffSize -
        (pOutput->pStrmCurrPos - DEC_STRM.pStrmBuffStart);

    return ((AvsDecRet) ret);

#undef API_STOR
#undef DEC_STRM

}

/*------------------------------------------------------------------------------

    Function: AvsDecRelease()

        Functional description:
            Release the decoder instance.

        Inputs:
            decInst     Decoder instance

        Outputs:
            none

        Returns:
            none

------------------------------------------------------------------------------*/

void AvsDecRelease(AvsDecInst decInst)
{
    DecContainer *pDecCont = NULL;
    const void *dwl;
    u32 i;

    AVSDEC_DEBUG(("1\n"));
    AVS_API_TRC("AvsDecRelease#");
    if(decInst == NULL)
    {
        AVS_API_TRC("AvsDecRelease# ERROR: decInst == NULL");
        return;
    }

    pDecCont = ((DecContainer *) decInst);
    dwl = pDecCont->dwl;

    if(pDecCont->asicRunning)
        (void) G1G1DWLReleaseHw(pDecCont->dwl);

    AvsFreeBuffers(pDecCont);

    G1DWLfree(pDecCont);

    (void) G1DWLRelease(dwl);

    AVS_API_TRC("AvsDecRelease: OK");
}

/*------------------------------------------------------------------------------

    Function: avsRegisterPP()

        Functional description:
            Register the pp for avs pipeline

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

i32 avsRegisterPP(const void *decInst, const void *ppInst,
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

    Function: avsUnregisterPP()

        Functional description:
            Unregister the pp from avs pipeline

        Inputs:
            decInst     Decoder instance
            const void  *ppInst - post-processor instance

        Outputs:
            none

        Returns:
            i32 - return 0 for success or a negative error code

------------------------------------------------------------------------------*/

i32 avsUnregisterPP(const void *decInst, const void *ppInst)
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
    Function name   : AvsRefreshRegs
    Description     :
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void AvsRefreshRegs(DecContainer * pDecCont)
{
    i32 i;
    u32 *ppRegs = pDecCont->avsRegs;

    for(i = 0; i < DEC_X170_REGISTERS; i++)
    {
        ppRegs[i] = G1DWLReadReg(pDecCont->dwl, 4 * i);
    }
}

/*------------------------------------------------------------------------------
    Function name   : AvsFlushRegs
    Description     :
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void AvsFlushRegs(DecContainer * pDecCont)
{
    i32 i;
    u32 *ppRegs = pDecCont->avsRegs;

    for(i = 2; i < DEC_X170_REGISTERS; i++)
    {
        G1DWLWriteReg(pDecCont->dwl, 4 * i, ppRegs[i]);
        ppRegs[i] = 0;
    }
}

/*------------------------------------------------------------------------------
    Function name   : AvsHandleVlcModeError
    Description     :
    Return type     : u32
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
u32 AvsHandleVlcModeError(DecContainer * pDecCont, u32 picNum)
{
    u32 ret = 0, tmp;

    ASSERT(pDecCont->StrmStorage.strmDecReady);

    tmp = AvsStrmDec_NextStartCode(pDecCont);
    if(tmp != END_OF_STREAM)
    {
        pDecCont->StrmDesc.pStrmCurrPos -= 4;
        pDecCont->StrmDesc.strmBuffReadBits -= 32;
    }

    /* error in first picture -> set reference to grey */
    if(!pDecCont->StrmStorage.frameNumber)
    {
        (void) G1DWLmemset(pDecCont->StrmStorage.
                         pPicBuf[pDecCont->StrmStorage.workOut].data.
                         virtualAddress, 128,
                         384 * pDecCont->StrmStorage.totalMbsInFrame);

        AvsDecPreparePicReturn(pDecCont);

        /* no pictures finished -> return STRM_PROCESSED */
        if(tmp == END_OF_STREAM)
            ret = AVSDEC_STRM_PROCESSED;
        else
            ret = 0;
        pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.workOut;
        pDecCont->StrmStorage.skipB = 2;
    }
    else
    {
        if (pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE)
            pDecCont->ppControl.multiBufStat = MULTIBUFFER_SEMIMODE;

        if(pDecCont->Hdrs.picCodingType != BFRAME)
        {
            pDecCont->StrmStorage.frameNumber++;

            /* reset sendToPp to prevent post-processing partly decoded
             * pictures */
            if(pDecCont->StrmStorage.workOut != pDecCont->StrmStorage.work0)
                pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].
                    sendToPp = 0;
        
            BqueueDiscard( &pDecCont->StrmStorage.bq, 
                        pDecCont->StrmStorage.workOut );
            pDecCont->StrmStorage.workOut = pDecCont->StrmStorage.work0;

            AvsDecBufferPicture(pDecCont,
                                  picNum,
                                  pDecCont->Hdrs.picCodingType == BFRAME,
                                  1, (AvsDecRet) FREEZED_PIC_RDY,
                                  pDecCont->StrmStorage.totalMbsInFrame);

            ret = AVSDEC_PIC_DECODED;

            pDecCont->StrmStorage.work1 = pDecCont->StrmStorage.work0;
            pDecCont->StrmStorage.skipB = 2;
        }
        else
        {
            if(pDecCont->StrmStorage.intraFreeze)
            {
                pDecCont->StrmStorage.frameNumber++;

                AvsDecBufferPicture(pDecCont,
                                      picNum,
                                      pDecCont->Hdrs.picCodingType == BFRAME,
                                      1, (AvsDecRet) FREEZED_PIC_RDY,
                                      pDecCont->StrmStorage.totalMbsInFrame);

                ret = AVSDEC_PIC_DECODED;

            }

            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].
                sendToPp = 0;
        }
    }

    pDecCont->ApiStorage.firstField = 1;

    pDecCont->ApiStorage.DecStat = STREAMDECODING;
    pDecCont->StrmStorage.validPicHeader = HANTRO_FALSE;
    pDecCont->Hdrs.pictureStructure = FRAMEPICTURE;

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : AvsHandleFrameEnd
    Description     :
    Return type     : u32
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void AvsHandleFrameEnd(DecContainer * pDecCont)
{

    u32 tmp;

    pDecCont->StrmDesc.strmBuffReadBits =
        8 * (pDecCont->StrmDesc.pStrmCurrPos -
             pDecCont->StrmDesc.pStrmBuffStart);
    pDecCont->StrmDesc.bitPosInWord = 0;

    do
    {
        tmp = AvsStrmDec_ShowBits(pDecCont, 32);
        if((tmp >> 8) == 0x1)
            break;
    }
    while(AvsStrmDec_FlushBits(pDecCont, 8) == HANTRO_OK);

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
        tmp = AvsSetRegs(pDecContainer, strmBusAddress);
        if(tmp == HANTRO_NOK)
            return 0;

        if (!pDecContainer->keepHwReserved)
            (void) G1DWLReserveHw(pDecContainer->dwl);

        SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_OUT_DIS, 0);

        /* Start PP */
        if(pDecContainer->ppInstance != NULL)
        {
            AvsPpControl(pDecContainer, 0);
        }

        pDecContainer->asicRunning = 1;

        G1DWLWriteReg(pDecContainer->dwl, 0x4, 0);

        AvsFlushRegs(pDecContainer);

        /* Enable HW */
        SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_E, 1);
        DWLEnableHW(pDecContainer->dwl, 4 * 1, pDecContainer->avsRegs[1]);
    }
    else    /* in the middle of decoding, continue decoding */
    {
        /* tmp is strmBusAddress + number of bytes decoded by SW */
        tmp = pDecContainer->StrmDesc.pStrmCurrPos -
            pDecContainer->StrmDesc.pStrmBuffStart;
        tmp = strmBusAddress + tmp;

        /* pointer to start of the stream, mask to get the pointer to
         * previous 64-bit aligned position */
        if(!(tmp & ~0x7))
        {
            return 0;
        }

        SetDecRegister(pDecContainer->avsRegs, HWIF_RLC_VLC_BASE, tmp & ~0x7);
        /* amount of stream (as seen by the HW), obtained as amount of stream
         * given by the application subtracted by number of bytes decoded by
         * SW (if strmBusAddress is not 64-bit aligned -> adds number of bytes
         * from previous 64-bit aligned boundary) */
        SetDecRegister(pDecContainer->avsRegs, HWIF_STREAM_LEN,
                       pDecContainer->StrmDesc.strmBuffSize -
                       ((tmp & ~0x7) - strmBusAddress));
        SetDecRegister(pDecContainer->avsRegs, HWIF_STRM_START_BIT,
                       pDecContainer->StrmDesc.bitPosInWord + 8 * (tmp & 0x7));

        /* This depends on actual register allocation */
        G1DWLWriteReg(pDecContainer->dwl, 4 * 5, pDecContainer->avsRegs[5]);
        G1DWLWriteReg(pDecContainer->dwl, 4 * 6, pDecContainer->avsRegs[6]);
        G1DWLWriteReg(pDecContainer->dwl, 4 * 12, pDecContainer->avsRegs[12]);

        G1DWLWriteReg(pDecContainer->dwl, 4 * 1, pDecContainer->avsRegs[1]);
    }

    /* Wait for HW ready */
    ret = G1DWLWaitHwReady(pDecContainer->dwl, (u32) DEC_X170_TIMEOUT_LENGTH);

    AvsRefreshRegs(pDecContainer);

    if(ret == DWL_HW_WAIT_OK)
    {
        asicStatus =
            GetDecRegister(pDecContainer->avsRegs, HWIF_DEC_IRQ_STAT);
    }
    else if(ret == DWL_HW_WAIT_TIMEOUT)
    {
        asicStatus = ID8170_DEC_TIMEOUT;
    }
    else
    {
        asicStatus = ID8170_DEC_SYSTEM_ERROR;
    }

    if(!(asicStatus & AVS_DEC_X170_IRQ_BUFFER_EMPTY) ||
       (asicStatus & AVS_DEC_X170_IRQ_STREAM_ERROR) ||
       (asicStatus & AVS_DEC_X170_IRQ_BUS_ERROR) ||
       (asicStatus == ID8170_DEC_TIMEOUT) ||
       (asicStatus == ID8170_DEC_SYSTEM_ERROR))
    {
        /* reset HW */
        SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_IRQ_STAT, 0);
        SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_IRQ, 0);

        DWLDisableHW(pDecContainer->dwl, 4 * 1, pDecContainer->avsRegs[1]);

        pDecContainer->keepHwReserved = 0;

        /* End PP co-operation */
        if(pDecContainer->ppInstance != NULL)
        {
            if(pDecContainer->Hdrs.pictureStructure != FRAMEPICTURE)
            {
                if(pDecContainer->ppControl.ppStatus == DECPP_RUNNING ||
                   pDecContainer->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED)
                {
                    if(pDecContainer->ApiStorage.firstField &&
                       (asicStatus & AVS_DEC_X170_IRQ_DEC_RDY))
                    {
                        pDecContainer->ppControl.ppStatus =
                            DECPP_PIC_NOT_FINISHED;
                        pDecContainer->keepHwReserved = 1;
                    }
                    else
                    {
                        pDecContainer->ppControl.ppStatus = DECPP_PIC_READY;
                        pDecContainer->PPEndCallback(pDecContainer->ppInstance);
                    }
                }
            }
            else
            {
                /* End PP co-operation */
                if(pDecContainer->ppControl.ppStatus == DECPP_RUNNING)
                {
                    pDecContainer->PPEndCallback(pDecContainer->ppInstance);

                    if((asicStatus & AVS_DEC_X170_IRQ_STREAM_ERROR) &&
                       pDecContainer->ppControl.usePipeline)
                        pDecContainer->ppControl.ppStatus = DECPP_IDLE;
                    else
                        pDecContainer->ppControl.ppStatus = DECPP_PIC_READY;
                }
            }
        }

        pDecContainer->asicRunning = 0;
        if (!pDecContainer->keepHwReserved)
            (void) G1G1DWLReleaseHw(pDecContainer->dwl);
    }

    /* if HW interrupt indicated either BUFFER_EMPTY or
     * DEC_RDY -> read stream end pointer and update StrmDesc structure */
    if((asicStatus &
        (AVS_DEC_X170_IRQ_BUFFER_EMPTY | AVS_DEC_X170_IRQ_DEC_RDY)))
    {
        tmp = GetDecRegister(pDecContainer->avsRegs, HWIF_RLC_VLC_BASE);
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

    if( pDecContainer->Hdrs.picCodingType != BFRAME &&
        pDecContainer->refBufSupport &&
        (asicStatus & AVS_DEC_X170_IRQ_DEC_RDY) &&
        pDecContainer->asicRunning == 0)
    {
        RefbuMvStatistics( &pDecContainer->refBufferCtrl, 
                            pDecContainer->avsRegs,
                            NULL,
                            HANTRO_FALSE,
                            pDecContainer->Hdrs.picCodingType == IFRAME );
    }

    SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_IRQ_STAT, 0);

    return asicStatus;

}

/*------------------------------------------------------------------------------

    Function name: AvsDecNextPicture

    Functional description:
        Retrieve next decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct
        endOfStream Indicates whether end of stream has been reached

    Output:
        pPicture Decoder output picture.

    Return values:
        AVSDEC_OK         No picture available.
        AVSDEC_PIC_RDY    Picture ready.

------------------------------------------------------------------------------*/
AvsDecRet AvsDecNextPicture(AvsDecInst decInst,
                                AvsDecPicture * pPicture, u32 endOfStream)
{
    /* Variables */
    AvsDecRet returnValue = AVSDEC_PIC_RDY;
    DecContainer *pDecCont;
    picture_t *pPic;
    u32 picIndex = AVS_BUFFER_UNDEFINED;
    u32 minCount;
    u32 tmp = 0;
    u32 luma = 0, chroma = 0;

    /* Code */
    AVS_API_TRC("\nAvsDecNextPicture#");

    /* Check that function input parameters are valid */
    if(pPicture == NULL)
    {
        AVS_API_TRC("AvsDecNextPicture# ERROR: pPicture is NULL");
        return (AVSDEC_PARAM_ERROR);
    }

    pDecCont = (DecContainer *) decInst;

    /* Check if decoder is in an incorrect mode */
    if(decInst == NULL || pDecCont->ApiStorage.DecStat == UNINIT)
    {
        AVS_API_TRC("AvsDecNextPicture# ERROR: Decoder not initialized");
        return (AVSDEC_NOT_INITIALIZED);
    }

    minCount = 0;
    if(pDecCont->StrmStorage.sequenceLowDelay == 0 && !endOfStream &&
        !pDecCont->StrmStorage.newHeadersChangeResolution)
        minCount = 1;

    /* this is to prevent post-processing of non-finished pictures in the
     * end of the stream */
    if(endOfStream && pDecCont->Hdrs.picCodingType == BFRAME)
    {
        pDecCont->Hdrs.picCodingType = PFRAME;
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].sendToPp =
            0;
    }

    /* Nothing to send out */
    if(pDecCont->StrmStorage.outCount <= minCount)
    {
        (void) G1DWLmemset(pPicture, 0, sizeof(AvsDecPicture));
        pPicture->pOutputPicture = NULL;
        pPicture->interlaced = !pDecCont->Hdrs.progressiveSequence;
        returnValue = AVSDEC_OK;
    }
    else
    {
        picIndex = pDecCont->StrmStorage.outIndex;
        picIndex = pDecCont->StrmStorage.outBuf[picIndex];

        AvsFillPicStruct(pPicture, pDecCont, picIndex);

        /* field output */
        if(AVSDEC_IS_FIELD_OUTPUT)
        {
            pPicture->interlaced = 1;
            pPicture->fieldPicture = 1;

            if(!pDecCont->ApiStorage.outputOtherField)
            {
                pPicture->topField =
                    pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 1 : 0;
                pDecCont->ApiStorage.outputOtherField = 1;
            }
            else
            {
                pPicture->topField =
                    pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 0 : 1;
                pDecCont->ApiStorage.outputOtherField = 0;
                pDecCont->StrmStorage.outCount--;
                pDecCont->StrmStorage.outIndex++;
                pDecCont->StrmStorage.outIndex &= 15;
            }
        }
        else
        {
            /* progressive or deinterlaced frame output */
            pPicture->interlaced = !pDecCont->Hdrs.progressiveSequence;
            pPicture->topField = 0;
            pPicture->fieldPicture = 0;
            pDecCont->StrmStorage.outCount--;
            pDecCont->StrmStorage.outIndex++;
            pDecCont->StrmStorage.outIndex &= 15;
        }
    }

    if(pDecCont->ppInstance &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE) &&
       endOfStream && (returnValue == AVSDEC_PIC_RDY))
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
    if(pDecCont->ppInstance &&
       (pDecCont->ppControl.multiBufStat != MULTIBUFFER_FULLMODE))
    {
        /* pp and decoder running in parallel, decoder finished first field ->
         * decode second field and wait PP after that */
        if(pDecCont->ppInstance != NULL &&
           pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED)
        {
            return (AVSDEC_OK);
        }

        if(pDecCont->ppControl.ppStatus == DECPP_PIC_READY)
        {
            picIndex = pDecCont->ApiStorage.ppPicIndex;
            if(AVSDEC_IS_FIELD_OUTPUT)
            {
                if (pDecCont->ApiStorage.bufferForPp != NO_BUFFER)
                    picIndex = pDecCont->ApiStorage.bufferForPp - 1;
                pPicture->interlaced = 1;
                pPicture->fieldPicture = 1;
                pPicture->topField =
                    pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 1 : 0;
            }
            AvsFillPicStruct(pPicture, pDecCont, picIndex);
            returnValue = AVSDEC_PIC_RDY;
            pDecCont->ppControl.ppStatus = DECPP_IDLE;
        }
        else
        {
            pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;
            returnValue = AVSDEC_OK;
            picIndex = AVS_BUFFER_UNDEFINED;

            if(AVSDEC_NON_PIPELINE_AND_B_PICTURE)
            {
                /* send current B Picture output to PP */
                picIndex = pDecCont->StrmStorage.workOut;
                pDecCont->Hdrs.picCodingType = IFRAME;

                /* Set here field decoding for first field of a B picture */
                if(AVSDEC_IS_FIELD_OUTPUT)
                {
                    pDecCont->ApiStorage.bufferForPp = picIndex+1;
                    pPicture->interlaced = 1;
                    pPicture->fieldPicture = 1;
                    pPicture->topField =
                        pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 1 : 0;
                    pDecCont->ppControl.picStruct =
                        pDecCont->StrmStorage.pPicBuf[picIndex].tf ?
                        DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;
                }
            }
            else if(AVSDEC_IS_FIELD_OUTPUT &&
                    (pDecCont->ApiStorage.bufferForPp != NO_BUFFER))
            {
                pPicture->interlaced = 1;
                pPicture->fieldPicture = 1;
                picIndex = (pDecCont->ApiStorage.bufferForPp-1);
                pDecCont->ppControl.picStruct =
                    pDecCont->StrmStorage.pPicBuf[picIndex].tf ?
                    DECPP_PIC_BOT_FIELD_FRAME : DECPP_PIC_TOP_FIELD_FRAME;
                pPicture->topField =
                    pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 0 : 1;
                pDecCont->ApiStorage.bufferForPp = NO_BUFFER;
                pPic[picIndex].sendToPp = 1;
            }
            else if(endOfStream)
            {
                 picIndex = 0;
                 while((picIndex < pDecCont->StrmStorage.numBuffers) && !pPic[picIndex].sendToPp)
                     picIndex++;

                 if (picIndex == pDecCont->StrmStorage.numBuffers)
                     return AVSDEC_OK;
                 
                if(AVSDEC_IS_FIELD_OUTPUT)
                {
                    /* if field output, other field must be processed also */
                    pDecCont->ApiStorage.bufferForPp = picIndex+1;

                    /* set field processing */
                    pDecCont->ppControl.picStruct =
                        pDecCont->StrmStorage.pPicBuf[picIndex].tf ?
                        DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;

                    pPicture->topField =
                        pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 1 : 0;
                    pPicture->fieldPicture = 1;
                }
                else if(pDecCont->ppConfigQuery.deinterlace)
                {
                    AvsDecSetupDeinterlace(pDecCont);
                }
            }

            if(picIndex != AVS_BUFFER_UNDEFINED)
            {
                if(pPic[picIndex].sendToPp && pPic[picIndex].sendToPp < 3)
                {
                    /* forward tiled mode */
                    pDecCont->ppControl.tiledInputMode = 
                        pPic[picIndex].tiledMode;
                    pDecCont->ppControl.progressiveSequence =
                        pDecCont->Hdrs.progressiveSequence;

                    /* Set up pp */
                    if(pDecCont->ppControl.picStruct ==
                       DECPP_PIC_BOT_FIELD_FRAME)
                    {
                        pDecCont->ppControl.inputBusLuma = 0;
                        pDecCont->ppControl.inputBusChroma = 0;

                        pDecCont->ppControl.bottomBusLuma =
                            pDecCont->StrmStorage.pPicBuf[picIndex].data.
                            busAddress + (pDecCont->StrmStorage.frameWidth << 4);

                        pDecCont->ppControl.bottomBusChroma =
                            pDecCont->ppControl.bottomBusLuma +
                            ((pDecCont->StrmStorage.frameWidth *
                              pDecCont->StrmStorage.frameHeight) << 8);

                        pDecCont->ppControl.inwidth =
                            pDecCont->ppControl.croppedW =
                            pDecCont->StrmStorage.frameWidth << 4;
                        pDecCont->ppControl.inheight =
                            (((pDecCont->StrmStorage.frameHeight +
                               1) & ~1) / 2) << 4;
                        pDecCont->ppControl.croppedH =
                            (pDecCont->StrmStorage.frameHeight << 4) / 2;
                    }
                    else
                    {
                        pDecCont->ppControl.inputBusLuma =
                            pDecCont->StrmStorage.pPicBuf[picIndex].data.
                            busAddress;
                        pDecCont->ppControl.inputBusChroma =
                            pDecCont->ppControl.inputBusLuma +
                            ((pDecCont->StrmStorage.frameWidth *
                              pDecCont->StrmStorage.frameHeight) << 8);
                        if(pDecCont->ppControl.picStruct ==
                           DECPP_PIC_TOP_FIELD_FRAME)
                        {
                            pDecCont->ppControl.bottomBusLuma = 0;
                            pDecCont->ppControl.bottomBusChroma = 0;
                            pDecCont->ppControl.inwidth =
                                pDecCont->ppControl.croppedW =
                                pDecCont->StrmStorage.frameWidth << 4;
                            pDecCont->ppControl.inheight =
                                (((pDecCont->StrmStorage.frameHeight +
                                   1) & ~1) / 2) << 4;
                            pDecCont->ppControl.croppedH =
                                (pDecCont->StrmStorage.frameHeight << 4) / 2;
                        }
                        else
                        {
                            pDecCont->ppControl.inwidth =
                                pDecCont->ppControl.croppedW =
                                pDecCont->StrmStorage.frameWidth << 4;
                            pDecCont->ppControl.inheight =
                                pDecCont->ppControl.croppedH =
                                pDecCont->StrmStorage.frameHeight << 4;
                            if(pDecCont->ppConfigQuery.deinterlace)
                            {
                                AvsDecSetupDeinterlace(pDecCont);
                            }

                        }
                    }

                    pDecCont->ppControl.usePipeline = 0;
                    {
                        u32 value = GetDecRegister(pDecCont->avsRegs,
                                                   HWIF_DEC_OUT_ENDIAN);

                        pDecCont->ppControl.littleEndian =
                            (value == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pDecCont->ppControl.wordSwap =
                        GetDecRegister(pDecCont->avsRegs,
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

                    AvsFillPicStruct(pPicture, pDecCont, picIndex);
                    returnValue = AVSDEC_PIC_RDY;
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

    Function name: AvsFillPicStruct

    Functional description:
        Fill data to output pic description

    Input:
        pDecCont    Decoder container
        pPicture    Pointer to return value struct

    Return values:
        void

------------------------------------------------------------------------------*/
static void AvsFillPicStruct(AvsDecPicture * pPicture,
                               DecContainer * pDecCont, u32 picIndex)
{
    picture_t *pPic;

    pPicture->frameWidth = pDecCont->StrmStorage.frameWidth << 4;
    pPicture->frameHeight = pDecCont->StrmStorage.frameHeight << 4;
    pPicture->codedWidth = pDecCont->Hdrs.horizontalSize;
    pPicture->codedHeight = pDecCont->Hdrs.verticalSize;
    pPicture->interlaced = !pDecCont->Hdrs.progressiveSequence;

    pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;
    pPicture->pOutputPicture = (u8 *) pPic[picIndex].data.virtualAddress;
    pPicture->outputPictureBusAddress = pPic[picIndex].data.busAddress;
    pPicture->keyPicture = pPic[picIndex].picType;
    pPicture->picId = pPic[picIndex].picId;

    /* handle first field indication */
    if(!pDecCont->Hdrs.progressiveSequence)
    {
        if(pDecCont->StrmStorage.fieldOutIndex)
            pDecCont->StrmStorage.fieldOutIndex = 0;
        else
            pDecCont->StrmStorage.fieldOutIndex = 1;
    }

    pPicture->firstField = pPic[picIndex].ff[pDecCont->StrmStorage.fieldOutIndex];
    pPicture->repeatFirstField = pPic[picIndex].rff;
    pPicture->repeatFrameCount = pPic[picIndex].rfc;
    pPicture->numberOfErrMBs = pPic[picIndex].nbrErrMbs;
    pPicture->outputFormat = pPic[picIndex].tiledMode ?
        DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

    (void) G1DWLmemcpy(&pPicture->timeCode,
                     &pPic[picIndex].timeCode, sizeof(AvsDecTime));
}

/*------------------------------------------------------------------------------

    Function name: AvsSetRegs

    Functional description:
        Set registers

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static u32 AvsSetRegs(DecContainer * pDecContainer, u32 strmBusAddress)
{
    u32 tmp = 0;
    u32 tmpFwd, tmpCurr;

#ifdef _DEC_PP_USAGE
    AvsDecPpUsagePrint(pDecContainer, DECPP_UNSPECIFIED,
                         pDecContainer->StrmStorage.workOut, 1,
                         pDecContainer->StrmStorage.latestId);
#endif

    /*
    if(!pDecContainer->Hdrs.progressiveSequence)
        SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_OUT_TILED_E, 0);
        */

    if(pDecContainer->Hdrs.pictureStructure == FRAMEPICTURE ||
       pDecContainer->ApiStorage.firstField)
    {
        pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.workOut].
            sendToPp = 1;
    }
    else
    {
        pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.workOut].
            sendToPp = 2;
    }

    AVSDEC_DEBUG(("Decoding to index %d \n",
                        pDecContainer->StrmStorage.workOut));

    if(pDecContainer->Hdrs.pictureStructure == FRAMEPICTURE)
    {
        SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_INTERLACE_E, 0);
        SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_FIELDMODE_E, 0);
        SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_TOPFIELD_E, 0);
    }
    else
    {
        SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_INTERLACE_E, 1);
        SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_FIELDMODE_E, 1);
        SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_TOPFIELD_E,
            pDecContainer->ApiStorage.firstField);
    }

    SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_MB_HEIGHT_P,
                   pDecContainer->StrmStorage.frameHeight);

    if(pDecContainer->Hdrs.picCodingType == BFRAME)
        SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_B_E, 1);
    else
        SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_B_E, 0);

    SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_INTER_E,
                   pDecContainer->Hdrs.picCodingType != IFRAME);

    /* tmp is strmBusAddress + number of bytes decoded by SW */
    tmp = pDecContainer->StrmDesc.pStrmCurrPos -
        pDecContainer->StrmDesc.pStrmBuffStart;
    tmp = strmBusAddress + tmp;

    /* bus address must not be zero */
    if(!(tmp & ~0x7))
    {
        return 0;
    }

    /* pointer to start of the stream, mask to get the pointer to
     * previous 64-bit aligned position */
    SetDecRegister(pDecContainer->avsRegs, HWIF_RLC_VLC_BASE, tmp & ~0x7);

    /* amount of stream (as seen by the HW), obtained as amount of
     * stream given by the application subtracted by number of bytes
     * decoded by SW (if strmBusAddress is not 64-bit aligned -> adds
     * number of bytes from previous 64-bit aligned boundary) */
    SetDecRegister(pDecContainer->avsRegs, HWIF_STREAM_LEN,
                   pDecContainer->StrmDesc.strmBuffSize -
                   ((tmp & ~0x7) - strmBusAddress));

    SetDecRegister(pDecContainer->avsRegs, HWIF_STRM_START_BIT,
                   pDecContainer->StrmDesc.bitPosInWord + 8 * (tmp & 0x7));

    SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_FIXED_QUANT,
        pDecContainer->Hdrs.fixedPictureQp);
    SetDecRegister(pDecContainer->avsRegs, HWIF_INIT_QP, 
        pDecContainer->Hdrs.pictureQp);

    if (pDecContainer->Hdrs.pictureStructure == FRAMEPICTURE ||
        pDecContainer->ApiStorage.firstField)
    {
        SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_OUT_BASE,
                       pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                          StrmStorage.workOut].
                       data.busAddress);
    }
    else
    {
        SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_OUT_BASE,
                       (pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                           StrmStorage.workOut].
                        data.busAddress +
                        ((pDecContainer->StrmStorage.frameWidth << 4))));
    }

    if(pDecContainer->Hdrs.pictureStructure == FRAMEPICTURE)
    {
        if(pDecContainer->Hdrs.picCodingType == BFRAME)
        {
            /* past anchor set to future anchor if past is invalid (second
             * picture in sequence is B) */
            tmpFwd =
                pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE ?
                pDecContainer->StrmStorage.work1 :
                pDecContainer->StrmStorage.work0;

            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER0_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER1_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER2_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER3_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);

            /* block distances */

            /* current to future anchor */
            tmp = (2*pDecContainer->StrmStorage.
                pPicBuf[pDecContainer->StrmStorage.work0].pictureDistance -
                2*pDecContainer->Hdrs.pictureDistance + 512) & 0x1FF;
            /* prevent division by zero */
            if (!tmp) tmp = 2;
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_2, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_3, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_2,
                512/tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_3,
                512/tmp);

            /* current to past anchor */
            if (pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE)
            {
                tmp = (2*pDecContainer->Hdrs.pictureDistance -
                    2*pDecContainer->StrmStorage.
                    pPicBuf[pDecContainer->StrmStorage.work1].pictureDistance -
                    512) & 0x1FF;
                if (!tmp) tmp = 2;
            }
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_0, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_1, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_0,
                512/tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_1,
                512/tmp);
            
            /* future anchor to past anchor */
            if (pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE)
            {
                tmp = (2*pDecContainer->StrmStorage.
                    pPicBuf[pDecContainer->StrmStorage.work0].pictureDistance -
                       2*pDecContainer->StrmStorage.
                    pPicBuf[pDecContainer->StrmStorage.work1].pictureDistance -
                      + 512) & 0x1FF;
                if (!tmp) tmp = 2;
            }
            tmp = 16384/tmp;
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_0, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_1, tmp);

            /* future anchor to previous past anchor */
            tmp = (2*pDecContainer->StrmStorage.
                pPicBuf[pDecContainer->StrmStorage.work0].pictureDistance -
                   2*pDecContainer->StrmStorage.
                pPicBuf[pDecContainer->StrmStorage.workOut].pictureDistance -
                  + 512) & 0x1FF;
            if (!tmp) tmp = 2;
            tmp = 16384/tmp;
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_2, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_3, tmp);
        }
        else
        {
            tmpFwd =
                pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE ?
                pDecContainer->StrmStorage.work1 :
                pDecContainer->StrmStorage.work0;

            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER0_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER1_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER2_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[tmpFwd].data.busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER3_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[tmpFwd].data.busAddress);

            /* current to past anchor */
            tmp = (2*pDecContainer->Hdrs.pictureDistance -
                2*pDecContainer->StrmStorage.
                pPicBuf[pDecContainer->StrmStorage.work0].pictureDistance -
                512) & 0x1FF;
            if (!tmp) tmp = 2;
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_0, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_1, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_0,
                512/tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_1,
                512/tmp);
            /* current to previous past anchor */
            if (pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE)
            {
                tmp = (2*pDecContainer->Hdrs.pictureDistance -
                    2*pDecContainer->StrmStorage.
                    pPicBuf[pDecContainer->StrmStorage.work1].pictureDistance -
                    512) & 0x1FF;
                if (!tmp) tmp = 2;
            }
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_2, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_3, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_2,
                512/tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_3,
                512/tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_0, 0);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_1, 0);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_2, 0);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_3, 0);
        }
    }
    else /* field interlaced */
    {
        if(pDecContainer->Hdrs.picCodingType == BFRAME)
        {
            /* past anchor set to future anchor if past is invalid (second
             * picture in sequence is B) */
            tmpFwd =
                pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE ?
                pDecContainer->StrmStorage.work1 :
                pDecContainer->StrmStorage.work0;

            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER0_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER1_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER2_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REFER3_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);

            /* block distances */
            tmp = (2*pDecContainer->StrmStorage.
                pPicBuf[pDecContainer->StrmStorage.work0].pictureDistance -
                2*pDecContainer->Hdrs.pictureDistance + 512) & 0x1FF;
            /* prevent division by zero */
            if (!tmp) tmp = 2;
            if (pDecContainer->ApiStorage.firstField)
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_2,
                    tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_3,
                    tmp+1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_2,
                    512/tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_3,
                    512/(tmp+1));
            }
            else
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_2,
                    tmp-1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_3,
                    tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_2,
                    512/(tmp-1));
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_3,
                    512/tmp);
            }

            if (pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE)
            {
                tmp = (2*pDecContainer->Hdrs.pictureDistance -
                    2*pDecContainer->StrmStorage.
                    pPicBuf[pDecContainer->StrmStorage.work1].pictureDistance -
                    512) & 0x1FF;
                if (!tmp) tmp = 2;
            }
            if (pDecContainer->ApiStorage.firstField)
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_0,
                    tmp-1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_1,
                    tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_0,
                    512/(tmp-1));
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_1,
                    512/tmp);
            }
            else
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_0,
                    tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_1,
                    tmp+1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_0,
                    512/tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_1,
                    512/(tmp+1));
            }
            
            if (pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE)
            {
                tmp = (2*pDecContainer->StrmStorage.
                    pPicBuf[pDecContainer->StrmStorage.work0].pictureDistance -
                       2*pDecContainer->StrmStorage.
                    pPicBuf[pDecContainer->StrmStorage.work1].pictureDistance -
                      + 512) & 0x1FF;
                if (!tmp) tmp = 2;
            }
            if(pDecContainer->ApiStorage.firstField)
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_0,
                    16384/(tmp-1));
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_1,
                    16384/tmp);
            }
            else
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_0,
                    16384/1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_1,
                    16384/tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_2,
                    16384/(tmp+1));
            }
            tmp = (2*pDecContainer->StrmStorage.
                pPicBuf[pDecContainer->StrmStorage.work0].pictureDistance -
                   2*pDecContainer->StrmStorage.
                pPicBuf[pDecContainer->StrmStorage.workOut].pictureDistance -
                  + 512) & 0x1FF;
            if (!tmp) tmp = 2;
            if(pDecContainer->ApiStorage.firstField)
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_2,
                    16384/(tmp-1));
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_3,
                    16384/tmp);
            }
            else
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_3,
                    16384/tmp);
        }
        else
        {
            tmpFwd =
                pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE ?
                pDecContainer->StrmStorage.work1 :
                pDecContainer->StrmStorage.work0;

            tmpCurr = pDecContainer->StrmStorage.workOut;
            /* past anchor not available -> use current (this results in using
             * the same top or bottom field as reference and output picture
             * base, utput is probably corrupted) */
            if(tmpFwd == INVALID_ANCHOR_PICTURE)
                tmpFwd = tmpCurr;

            if(pDecContainer->ApiStorage.firstField)
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REFER0_BASE,
                               pDecContainer->StrmStorage.
                               pPicBuf[pDecContainer->StrmStorage.work0].data.
                               busAddress);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REFER1_BASE,
                               pDecContainer->StrmStorage.
                               pPicBuf[pDecContainer->StrmStorage.work0].data.
                               busAddress);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REFER2_BASE,
                               pDecContainer->StrmStorage.
                               pPicBuf[tmpFwd].data.busAddress);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REFER3_BASE,
                               pDecContainer->StrmStorage.
                               pPicBuf[tmpFwd].data.busAddress);

            }
            else
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REFER0_BASE,
                               pDecContainer->StrmStorage.pPicBuf[tmpCurr].data.
                               busAddress);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REFER1_BASE,
                               pDecContainer->StrmStorage.
                               pPicBuf[pDecContainer->StrmStorage.work0].data.
                               busAddress);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REFER2_BASE,
                               pDecContainer->StrmStorage.
                               pPicBuf[pDecContainer->StrmStorage.work0].data.
                               busAddress);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REFER3_BASE,
                               pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                               busAddress);
            }

            tmp = (2*pDecContainer->Hdrs.pictureDistance -
                2*pDecContainer->StrmStorage.
                pPicBuf[pDecContainer->StrmStorage.work0].pictureDistance -
                512) & 0x1FF;
            if (!tmp) tmp = 2;

            if(!pDecContainer->ApiStorage.firstField)
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_0, 1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_2,
                    tmp+1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_0,
                    512/1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_2,
                    512/(tmp+1));
            }
            else
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_0,
                    tmp-1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_0,
                    512/(tmp-1));
            }
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_1, tmp);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_1,
                512/tmp);

            if (pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE)
            {
                tmp = (2*pDecContainer->Hdrs.pictureDistance -
                    2*pDecContainer->StrmStorage.
                    pPicBuf[pDecContainer->StrmStorage.work1].pictureDistance -
                    512) & 0x1FF;
                if (!tmp) tmp = 2;
            }

            if(pDecContainer->ApiStorage.firstField)
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_2,
                    tmp-1);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_3,
                    tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_2,
                    512/(tmp-1));
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_3,
                    512/tmp);
            }
            else
            {
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_DIST_CUR_3,
                    tmp);
                SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_CUR_3,
                    512/tmp);
            }

            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_0, 0);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_1, 0);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_2, 0);
            SetDecRegister(pDecContainer->avsRegs, HWIF_REF_INVD_COL_3, 0);
        }

    }

    SetDecRegister(pDecContainer->avsRegs, HWIF_STARTMB_X, 0);
    SetDecRegister(pDecContainer->avsRegs, HWIF_STARTMB_Y, 0);

    SetDecRegister(pDecContainer->avsRegs, HWIF_FILTERING_DIS, 
        pDecContainer->Hdrs.loopFilterDisable);
    SetDecRegister(pDecContainer->avsRegs, HWIF_ALPHA_OFFSET, 
        pDecContainer->Hdrs.alphaOffset);
    SetDecRegister(pDecContainer->avsRegs, HWIF_BETA_OFFSET, 
        pDecContainer->Hdrs.betaOffset);
    SetDecRegister(pDecContainer->avsRegs, HWIF_SKIP_MODE, 
        pDecContainer->Hdrs.skipModeFlag);

    SetDecRegister(pDecContainer->avsRegs, HWIF_PIC_REFER_FLAG,
        pDecContainer->Hdrs.pictureReferenceFlag);

    if (pDecContainer->Hdrs.picCodingType == PFRAME ||
        (pDecContainer->Hdrs.picCodingType == IFRAME &&
         !pDecContainer->ApiStorage.firstField))
    {
        SetDecRegister(pDecContainer->avsRegs, HWIF_WRITE_MVS_E, 1);
    }
    else
        SetDecRegister(pDecContainer->avsRegs, HWIF_WRITE_MVS_E, 0);

    if (pDecContainer->ApiStorage.firstField || 
        ( pDecContainer->Hdrs.picCodingType == BFRAME &&
          pDecContainer->StrmStorage.prevPicStructure ))
        SetDecRegister(pDecContainer->avsRegs, HWIF_DIR_MV_BASE,
            pDecContainer->StrmStorage.directMvs.busAddress);
    else
        SetDecRegister(pDecContainer->avsRegs, HWIF_DIR_MV_BASE,
            pDecContainer->StrmStorage.directMvs.busAddress+
            ( ( ( pDecContainer->StrmStorage.frameWidth *
                  pDecContainer->StrmStorage.frameHeight/2 + 1) & ~0x1) *
              4 * sizeof(u32)));

    SetDecRegister(pDecContainer->avsRegs, HWIF_PREV_ANC_TYPE,
        !pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.work0].
        picType ||
        (!pDecContainer->ApiStorage.firstField &&
          pDecContainer->StrmStorage.prevPicStructure == 0));

    /* b-picture needs to know if future reference is field or frame coded */
    SetDecRegister(pDecContainer->avsRegs, HWIF_REFER2_FIELD_E,
        pDecContainer->StrmStorage.prevPicStructure == 0);
    SetDecRegister(pDecContainer->avsRegs, HWIF_REFER3_FIELD_E,
        pDecContainer->StrmStorage.prevPicStructure == 0);

    /* Setup reference picture buffer */
    if( pDecContainer->refBufSupport )
        RefbuSetup(&pDecContainer->refBufferCtrl, pDecContainer->avsRegs,
                   pDecContainer->Hdrs.pictureStructure == FIELDPICTURE ?
                        REFBU_FIELD : REFBU_FRAME,
                   pDecContainer->Hdrs.picCodingType == IFRAME,
                   pDecContainer->Hdrs.picCodingType == BFRAME, 0, 2,
                   0 );

    if( pDecContainer->tiledModeSupport)
    {
        pDecContainer->tiledReferenceEnable = 
            DecSetupTiledReference( pDecContainer->avsRegs, 
                pDecContainer->tiledModeSupport,
                !(pDecContainer->Hdrs.progressiveSequence) );
    }
    else
    {
        pDecContainer->tiledReferenceEnable = 0;
    }

    return HANTRO_OK;
}

/*------------------------------------------------------------------------------

    Function name: AvsDecSetupDeinterlace

    Functional description:
        Setup PP interface for deinterlacing

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void AvsDecSetupDeinterlace(DecContainer * pDecCont)
{
    pDecCont->ppControl.picStruct = DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
    pDecCont->ppControl.bottomBusLuma = pDecCont->ppControl.inputBusLuma +
        (pDecCont->StrmStorage.frameWidth << 4);
    pDecCont->ppControl.bottomBusChroma = pDecCont->ppControl.inputBusChroma +
        (pDecCont->StrmStorage.frameWidth << 4);
}

/*------------------------------------------------------------------------------

    Function name: AvsDecPrepareFieldProcessing

    Functional description:
        Setup PP interface for deinterlacing

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void AvsDecPrepareFieldProcessing(DecContainer * pDecCont)
{
    pDecCont->ppControl.picStruct =
        pDecCont->Hdrs.topFieldFirst ?
        DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;
    pDecCont->ApiStorage.bufferForPp = pDecCont->StrmStorage.work0 + 1; 

    /* forward tiled mode */
    pDecCont->ppControl.tiledInputMode = pDecCont->tiledReferenceEnable;
    pDecCont->ppControl.progressiveSequence =
        pDecCont->Hdrs.progressiveSequence;

    if(pDecCont->Hdrs.topFieldFirst)
    {
        pDecCont->ppControl.inputBusLuma =
            pDecCont->StrmStorage.pPicBuf[pDecCont->
                                          StrmStorage.work0].data.busAddress;

        pDecCont->ppControl.inputBusChroma =
            pDecCont->ppControl.inputBusLuma +
            ((pDecCont->StrmStorage.frameWidth *
              pDecCont->StrmStorage.frameHeight) << 8);
    }
    else
    {

        pDecCont->ppControl.bottomBusLuma =
            pDecCont->StrmStorage.pPicBuf[pDecCont->
                                          StrmStorage.work0].data.busAddress +
            (pDecCont->StrmStorage.frameWidth << 4);

        pDecCont->ppControl.bottomBusChroma =
            pDecCont->ppControl.bottomBusLuma +
            ((pDecCont->StrmStorage.frameWidth *
              pDecCont->StrmStorage.frameHeight) << 8);
    }

    pDecCont->ppControl.inwidth =
        pDecCont->ppControl.croppedW = pDecCont->StrmStorage.frameWidth << 4;
    pDecCont->ppControl.inheight =
        (((pDecCont->StrmStorage.frameHeight + 1) & ~1) / 2) << 4;
    pDecCont->ppControl.croppedH = (pDecCont->StrmStorage.frameHeight << 4) / 2;

    AVSDEC_DEBUG(("FIELD: send %s\n",
                        pDecCont->ppControl.picStruct ==
                        DECPP_PIC_TOP_FIELD_FRAME ? "top" : "bottom"));
}

/*------------------------------------------------------------------------------

    Function name: AvsCheckFormatSupport

    Functional description:
        Check if avs supported

    Input:
        container

    Return values:
        return zero for OK

------------------------------------------------------------------------------*/
u32 AvsCheckFormatSupport(void)
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

    return (hwConfig.avsSupport == AVS_NOT_SUPPORTED);
}

/*------------------------------------------------------------------------------

    Function name: AvsPpControl

    Functional description:
        set up and start pp

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/

void AvsPpControl(DecContainer * pDecContainer, u32 pipelineOff)
{
    u32 indexForPp = AVS_BUFFER_UNDEFINED;

    DecPpInterface *pc = &pDecContainer->ppControl;
    DecHdrs *pHdrs = &pDecContainer->Hdrs;
    u32 nextBufferIndex;

    /* PP not connected or still running (not waited when first field of frame
     * finished */
    if (pc->ppStatus == DECPP_PIC_NOT_FINISHED)
        return;

    pDecContainer->ppConfigQuery.tiledMode =
        pDecContainer->tiledReferenceEnable;
    pDecContainer->PPConfigQuery(pDecContainer->ppInstance,
                                 &pDecContainer->ppConfigQuery);

    AvsPpMultiBufferSetup(pDecContainer, (pipelineOff ||
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
            pDecContainer->Hdrs.picCodingType == BFRAME);
        pc->bufferIndex = nextBufferIndex;
    }
    else
    {
        nextBufferIndex = pc->bufferIndex;
    }

    if(pHdrs->lowDelay ||
       pDecContainer->Hdrs.picCodingType == BFRAME)
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
        chroma = luma + ((pDecContainer->StrmStorage.frameWidth *
                 pDecContainer->StrmStorage.frameHeight) << 8);

        pDecContainer->PPBufferData(pDecContainer->ppInstance, 
            pc->bufferIndex, luma, chroma);
    }

    if(pc->multiBufStat == MULTIBUFFER_FULLMODE)
    {
        pc->usePipeline = pDecContainer->ppConfigQuery.pipelineAccepted;
        AvsDecRunFullmode(pDecContainer);
        pDecContainer->StrmStorage.previousModeFull = 1;
    }
    else if(pDecContainer->StrmStorage.previousModeFull == 1)
    {
        if(pDecContainer->Hdrs.picCodingType == BFRAME)
        {
            pDecContainer->StrmStorage.previousB = 1;
        }
        else
        {
            pDecContainer->StrmStorage.previousB = 0;
        }

        if(pDecContainer->Hdrs.picCodingType == BFRAME)
        {
            indexForPp = AVS_BUFFER_UNDEFINED;
            pc->inputBusLuma = 0;
        }
        pc->ppStatus = DECPP_IDLE;

        pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.work0].
            sendToPp = 1;

        pDecContainer->StrmStorage.previousModeFull = 0;
    }
    else
    {
        pc->bufferIndex = pc->displayIndex;

        if(pDecContainer->Hdrs.picCodingType == BFRAME)
        {
            pDecContainer->StrmStorage.previousB = 1;
        }
        else
        {
            pDecContainer->StrmStorage.previousB = 0;
        }

        if((!pDecContainer->StrmStorage.sequenceLowDelay &&
            (pDecContainer->Hdrs.picCodingType != BFRAME)) ||
           !pHdrs->progressiveSequence || pipelineOff)
        {
            pc->usePipeline = 0;
        }
        else
        {
            pc->usePipeline = pDecContainer->ppConfigQuery.pipelineAccepted;
        }

        if(!pc->usePipeline)
        {
            /* pipeline not accepted, don't run for first picture */
            if(pDecContainer->StrmStorage.frameNumber &&
               (pDecContainer->ApiStorage.bufferForPp == NO_BUFFER) &&
               (pHdrs->progressiveSequence ||
                pDecContainer->ApiStorage.firstField ||
                !pDecContainer->ppConfigQuery.deinterlace))
            {
                /*if:
                 * B pictures allowed and non B picture OR
                 * B pictures not allowed */
                if(pDecContainer->StrmStorage.sequenceLowDelay ||
                   pDecContainer->Hdrs.picCodingType != BFRAME)
                {

                    /* forward tiled mode */
                    pDecContainer->ppControl.tiledInputMode = 
                        pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                           StrmStorage.work0].
                        tiledMode;
                    pDecContainer->ppControl.progressiveSequence =
                        pDecContainer->Hdrs.progressiveSequence;

                    pc->inputBusLuma =
                        pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                           StrmStorage.work0].
                        data.busAddress;

                    pc->inputBusChroma =
                        pc->inputBusLuma +
                        ((pDecContainer->StrmStorage.frameWidth *
                          pDecContainer->StrmStorage.frameHeight) << 8);

                    pc->inwidth = pc->croppedW =
                        pDecContainer->StrmStorage.frameWidth << 4;
                    pc->inheight = pc->croppedH =
                        pDecContainer->StrmStorage.frameHeight << 4;
                    {
                        u32 value = GetDecRegister(pDecContainer->avsRegs,
                                                   HWIF_DEC_OUT_ENDIAN);

                        pc->littleEndian =
                            (value == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pc->wordSwap =
                        GetDecRegister(pDecContainer->avsRegs,
                                       HWIF_DEC_OUTSWAP32_E) ? 1 : 0;

                    indexForPp = pDecContainer->StrmStorage.work0;

                    if(pDecContainer->ppConfigQuery.deinterlace)
                    {
                        AvsDecSetupDeinterlace(pDecContainer);
                    }
                    /* if field output is used, send only a field to PP */
                    else if(!pHdrs->progressiveSequence)
                    {
                        AvsDecPrepareFieldProcessing(pDecContainer);
                    }
                    pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp = 2;

                    pDecContainer->ApiStorage.ppPicIndex = indexForPp;
                }
                else
                {
                    indexForPp = pDecContainer->StrmStorage.workOut;
                    indexForPp = AVS_BUFFER_UNDEFINED;
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
            pc->inputBusLuma = pc->inputBusChroma = 0;
            indexForPp = pDecContainer->StrmStorage.workOut;

            /* forward tiled mode */
            pc->tiledInputMode = pDecContainer->tiledReferenceEnable;
            pc->progressiveSequence =
                        pDecContainer->Hdrs.progressiveSequence;

            pc->inwidth = pc->croppedW =
                pDecContainer->StrmStorage.frameWidth << 4;
            pc->inheight = pc->croppedH =
                pDecContainer->StrmStorage.frameHeight << 4;
        }

        /* start PP */
        if(((pc->inputBusLuma && !pc->usePipeline) ||
            (!pc->inputBusLuma && pc->usePipeline))
           && pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp)
        {

            if(pc->usePipeline) /*CHECK !! */
            {
                if(pDecContainer->Hdrs.picCodingType == BFRAME)
                    SetDecRegister(pDecContainer->avsRegs, HWIF_DEC_OUT_DIS, 1);
            }

            ASSERT(indexForPp != AVS_BUFFER_UNDEFINED);

            if(pc->picStruct == DECPP_PIC_FRAME_OR_TOP_FIELD ||
               pc->picStruct == DECPP_PIC_TOP_AND_BOT_FIELD_FRAME)

            {
                pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp = 0;
            }
            else
            {
                pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp--;
            }

            pDecContainer->PPRun(pDecContainer->ppInstance, pc);

            pc->ppStatus = DECPP_RUNNING;
        }
        pDecContainer->StrmStorage.previousModeFull = 0;
    }

    if( pDecContainer->Hdrs.picCodingType != BFRAME )
    {
        pc->prevAnchorDisplayIndex = nextBufferIndex;
    }
}

/*------------------------------------------------------------------------------

    Function name: AvsPpMultiBufferSetup

    Functional description:
        Modify state of pp output buffering.

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/
void AvsPpMultiBufferSetup(DecContainer * pDecCont, u32 pipelineOff)
{

    if(pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED)
    {
        return;
    }

    if(pipelineOff && pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE)
    {
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_SEMIMODE;
    }

    if(!pipelineOff &&
       pDecCont->Hdrs.progressiveSequence &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_SEMIMODE))
    {
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_FULLMODE;
    }

    if(pDecCont->ppControl.multiBufStat == MULTIBUFFER_UNINIT)
        AvsPpMultiBufferInit(pDecCont);

}

/*------------------------------------------------------------------------------

    Function name: AvsPpMultiBufferInit

    Functional description:
        Modify state of pp output buffering.

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
void AvsPpMultiBufferInit(DecContainer * pDecCont)
{
    DecPpQuery *pq = &pDecCont->ppConfigQuery;
    DecPpInterface *pc = &pDecCont->ppControl;

    if(pq->multiBuffer)
    {
        if(!pq->pipelineAccepted || !pDecCont->Hdrs.progressiveSequence)
        {

            pc->multiBufStat = MULTIBUFFER_SEMIMODE;
        }
        else
        {
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

    Function name: AvsDecRunFullmode

    Functional description:

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
void AvsDecRunFullmode(DecContainer * pDecCont)
{
    u32 indexForPp = AVS_BUFFER_UNDEFINED;
    DecPpInterface *pc = &pDecCont->ppControl;
    DecHdrs *pHdrs = &pDecCont->Hdrs;

#ifdef _DEC_PP_USAGE
    AvsDecPpUsagePrint(pDecCont, DECPP_PIPELINED,
                         pDecCont->StrmStorage.workOut, 0,
                         pDecCont->StrmStorage.pPicBuf[pDecCont->
                                                       StrmStorage.
                                                       workOut].picId);
#endif

    if(!pDecCont->StrmStorage.previousModeFull &&
       pDecCont->StrmStorage.frameNumber)
    {
        if(pDecCont->Hdrs.picCodingType == BFRAME)
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work0].
                sendToPp = 0;
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work1].
                sendToPp = 0;
        }
        else
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work0].
                sendToPp = 0;
        }
    }

    if(pDecCont->Hdrs.picCodingType == BFRAME)
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
        ((pDecCont->StrmStorage.frameWidth *
          pDecCont->StrmStorage.frameHeight) << 8);

    pc->inwidth = pc->croppedW = pDecCont->StrmStorage.frameWidth << 4;
    pc->inheight = pc->croppedH = pDecCont->StrmStorage.frameHeight << 4;
    /* forward tiled mode */
    pc->tiledInputMode = pDecCont->tiledReferenceEnable;
    pc->progressiveSequence = pDecCont->Hdrs.progressiveSequence;

    {
        if(pDecCont->Hdrs.picCodingType == BFRAME)
            SetDecRegister(pDecCont->avsRegs, HWIF_DEC_OUT_DIS, 1);
    }

    ASSERT(indexForPp != AVS_BUFFER_UNDEFINED);

    ASSERT(pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp == 1);
    pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp = 0;

    pDecCont->PPRun(pDecCont->ppInstance, pc);

    pc->ppStatus = DECPP_RUNNING;
}

/*------------------------------------------------------------------------------

    Function name: AvsDecPeek

    Functional description:
        Retrieve last decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct

    Output:
        pPicture Decoder output picture.

    Return values:
        AVSDEC_OK         No picture available.
        AVSDEC_PIC_RDY    Picture ready.

------------------------------------------------------------------------------*/
AvsDecRet AvsDecPeek(AvsDecInst decInst, AvsDecPicture * pPicture)
{
    /* Variables */

    DecContainer *pDecCont;
    u32 picIndex;
    picture_t *pPic;

    /* Code */

    AVS_API_TRC("\nAvsDecPeek#");

    /* Check that function input parameters are valid */
    if(pPicture == NULL)
    {
        AVS_API_TRC("AvsDecPeek# ERROR: pPicture is NULL");
        return (AVSDEC_PARAM_ERROR);
    }

    pDecCont = (DecContainer *) decInst;

    /* Check if decoder is in an incorrect mode */
    if(decInst == NULL || pDecCont->ApiStorage.DecStat == UNINIT)
    {
        AVS_API_TRC("AvsDecPeek# ERROR: Decoder not initialized");
        return (AVSDEC_NOT_INITIALIZED);
    }

    if(!pDecCont->StrmStorage.outCount ||
        pDecCont->StrmStorage.newHeadersChangeResolution)
    {
        (void) G1DWLmemset(pPicture, 0, sizeof(AvsDecPicture));
        return AVSDEC_OK;
    }

    picIndex = pDecCont->StrmStorage.workOut;

    pPicture->frameWidth = pDecCont->StrmStorage.frameWidth << 4;
    pPicture->frameHeight = pDecCont->StrmStorage.frameHeight << 4;
    pPicture->codedWidth = pDecCont->Hdrs.horizontalSize;
    pPicture->codedHeight = pDecCont->Hdrs.verticalSize;
    pPicture->interlaced = !pDecCont->Hdrs.progressiveSequence;

    pPic = pDecCont->StrmStorage.pPicBuf + picIndex;
    pPicture->pOutputPicture = (u8 *) pPic->data.virtualAddress;
    pPicture->outputPictureBusAddress = pPic->data.busAddress;
    pPicture->keyPicture = pPic->picType;
    pPicture->picId = pPic->picId;

    pPicture->repeatFirstField = pPic->rff;
    pPicture->repeatFrameCount = pPic->rfc;
    pPicture->numberOfErrMBs = pPic->nbrErrMbs;
    pPicture->outputFormat = pPic->tiledMode ?
        DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

    (void) G1DWLmemcpy(&pPicture->timeCode,
                     &pPic->timeCode, sizeof(AvsDecTime));

    /* frame output */
    pPicture->fieldPicture = 0;
    pPicture->topField = 0;
    pPicture->firstField = 0;

    return AVSDEC_PIC_RDY;
}

/*------------------------------------------------------------------------------

    Function name: AvsDecSetLatency

    Functional description:
        set current video latencyMS to vdec driver for vpu dvfs

    Input:
        decInst     Reference to decoder instance.
        latencyMS   


    Return values:
        AVSDEC_OK;

------------------------------------------------------------------------------*/
AvsDecRet AvsDecSetLatency(AvsDecInst decInst, int latencyMS)
{
    DecContainer *pDecCont = (DecContainer *) decInst;

    DWLSetLatency(pDecCont->dwl, latencyMS);

    return AVSDEC_OK;
}
