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
--  Abstract : The API module C-functions of 8170 MPEG-2 Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2decapi.c,v $
--  $Date: 2011/02/04 12:40:10 $
--  $Revision: 1.296 $

------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "mpeg2hwd_cfg.h"
#include "mpeg2hwd_container.h"
#include "mpeg2hwd_utils.h"
#include "mpeg2hwd_strm.h"
#include "mpeg2decapi.h"
#include "mpeg2decapi_internal.h"
#include "dwl.h"
#include "regdrv.h"
#include "mpeg2hwd_headers.h"
#include "deccfg.h"
#include "refbuffer.h"
#include "workaround.h"
#include "bqueue.h"
#include "tiledref.h"
#include "mpeg2hwd_debug.h"
#ifdef MPEG2_ASIC_TRACE
#include "mpeg2asicdbgtrace.h"
#endif

#ifdef MPEG2DEC_TRACE
#define MPEG2_API_TRC(str)    Mpeg2DecTrace((str))
#else
#define MPEG2_API_TRC(str)
#endif

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

#define MPEG2_BUFFER_UNDEFINED    16

#define ID8170_DEC_TIMEOUT        0xFFU
#define ID8170_DEC_SYSTEM_ERROR   0xFEU
#define ID8170_DEC_HW_RESERVED    0xFDU

#define MPEG2DEC_UPDATE_POUTPUT \
    pDecCont->MbSetDesc.outData.dataLeft = \
    DEC_STRM.pStrmBuffStart - pDecCont->MbSetDesc.outData.pStrmCurrPos; \
    (void) G1DWLmemcpy(pOutput, &pDecCont->MbSetDesc.outData, \
                             sizeof(Mpeg2DecOutput))
#define NON_B_BUT_B_ALLOWED \
   !pDecContainer->Hdrs.lowDelay && pDecContainer->FrameDesc.picCodingType != BFRAME

#define MPEG2DEC_IS_FIELD_OUTPUT \
    pDecCont->Hdrs.interlaced && !pDecCont->ppConfigQuery.deinterlace

#define MPEG2DEC_NON_PIPELINE_AND_B_PICTURE \
    ((!pDecCont->ppConfigQuery.pipelineAccepted || pDecCont->Hdrs.interlaced) \
    && pDecCont->FrameDesc.picCodingType == BFRAME)

void mpeg2RefreshRegs(DecContainer * pDecCont);
void mpeg2FlushRegs(DecContainer * pDecCont);
static u32 mpeg2HandleVlcModeError(DecContainer * pDecCont, u32 picNum);
static void mpeg2HandleFrameEnd(DecContainer * pDecCont);
static u32 RunDecoderAsic(DecContainer * pDecContainer, u32 strmBusAddress);
static void Mpeg2FillPicStruct(Mpeg2DecPicture * pPicture,
                               DecContainer * pDecCont, u32 picIndex);
static u32 Mpeg2SetRegs(DecContainer * pDecContainer, u32 strmBusAddress);
static void Mpeg2DecSetupDeinterlace(DecContainer * pDecCont);
static void Mpeg2DecPrepareFieldProcessing(DecContainer * pDecCont, u32);
static u32 Mp2CheckFormatSupport(void);
static void Mpeg2PpControl(DecContainer * pDecCont, u32 pipelineOff);
static void Mpeg2PpMultiBufferInit(DecContainer * pDecCont);
static void Mpeg2PpMultiBufferSetup(DecContainer * pDecCont, u32 pipelineOff);
static void Mpeg2DecRunFullmode(DecContainer * pDecCont);

static void Mpeg2CheckReleasePpAndHw(DecContainer *pDecCont);
/*------------------------------------------------------------------------------
       Version Information - DO NOT CHANGE!
------------------------------------------------------------------------------*/

#define MPEG2DEC_MAJOR_VERSION 1
#define MPEG2DEC_MINOR_VERSION 2

#define MPEG2DEC_BUILD_MAJOR 1
#define MPEG2DEC_BUILD_MINOR 198
#define MPEG2DEC_SW_BUILD ((MPEG2DEC_BUILD_MAJOR * 1000) + MPEG2DEC_BUILD_MINOR)


/*------------------------------------------------------------------------------

    Function: Mpeg2DecGetAPIVersion

        Functional description:
            Return version information of API

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

Mpeg2DecApiVersion Mpeg2DecGetAPIVersion()
{
    Mpeg2DecApiVersion ver;

    ver.major = MPEG2DEC_MAJOR_VERSION;
    ver.minor = MPEG2DEC_MINOR_VERSION;

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: Mpeg2DecGetBuild

        Functional description:
            Return build information of SW and HW

        Returns:
            Mpeg2DecBuild

------------------------------------------------------------------------------*/

Mpeg2DecBuild Mpeg2DecGetBuild(void)
{
    Mpeg2DecBuild ver;
    DWLHwConfig_t hwCfg;

    MPEG2_API_TRC("Mpeg2DecGetBuild#");

    G1DWLmemset(&ver, 0, sizeof(ver));

    ver.swBuild = MPEG2DEC_SW_BUILD;
    ver.hwBuild = G1DWLReadAsicID();

    DWLReadAsicConfig(&hwCfg);

    SET_DEC_BUILD_SUPPORT(ver.hwConfig, hwCfg);

    MPEG2_API_TRC("Mpeg2DecGetBuild# OK\n");

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: Mpeg2DecInit()

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
            MPEG2DEC_OK       successfully initialized the instance
            MPEG2DEC_MEM_FAIL memory allocation failed

------------------------------------------------------------------------------*/
Mpeg2DecRet Mpeg2DecInit(Mpeg2DecInst * pDecInst, u32 useVideoFreezeConcealment,
                         u32 numFrameBuffers, DecRefFrmFormat referenceFrameFormat, u32 mmuEnable)
{
    /*@null@ */ DecContainer *pDecCont;
    /*@null@ */ const void *dwl;
    u32 i = 0;

    G1DWLInitParam_t dwlInit;
    DWLHwConfig_t config;

    MPEG2_API_TRC("Mpeg2DecInit#");
    MPEG2DEC_API_DEBUG(("Mpeg2API_DecoderInit#"));
    MPEG2FLUSH;

    /* check that right shift on negative numbers is performed signed */
    /*lint -save -e* following check causes multiple lint messages */
    if(((-1) >> 1) != (-1))
    {
        MPEG2DEC_API_DEBUG(("MPEG2DecInit# ERROR: Right shift is not signed"));
        MPEG2FLUSH;
        return (MPEG2DEC_INITFAIL);
    }
    /*lint -restore */

    if(pDecInst == NULL)
    {
        MPEG2DEC_API_DEBUG(("MPEG2DecInit# ERROR: pDecInst == NULL"));
        MPEG2FLUSH;
        return (MPEG2DEC_PARAM_ERROR);
    }

    *pDecInst = NULL;

    /* check that MPEG-2 decoding supported in HW */
    if(Mp2CheckFormatSupport())
    {
        MPEG2DEC_API_DEBUG(("MPEG2DecInit# ERROR: MPEG-2 not supported in HW\n"));
        return MPEG2DEC_FORMAT_NOT_SUPPORTED;
    }

    dwlInit.clientType = DWL_CLIENT_TYPE_MPEG2_DEC;
    dwlInit.mmuEnable = mmuEnable;
    dwl = G1DWLInit(&dwlInit);

    if(dwl == NULL)
    {
        MPEG2DEC_API_DEBUG(("MPEG2DecInit# ERROR: DWL Init failed"));
        MPEG2FLUSH;
        return (MPEG2DEC_DWL_ERROR);
    }

    MPEG2DEC_API_DEBUG(("size of DecContainer %d \n", sizeof(DecContainer)));
    pDecCont = (DecContainer *) G1DWLmalloc(sizeof(DecContainer));
    MPEG2FLUSH;

    if(pDecCont == NULL)
    {
        (void) G1DWLRelease(dwl);
        return (MPEG2DEC_MEMFAIL);
    }

    /* set everything initially zero */
    (void) G1DWLmemset(pDecCont, 0, sizeof(DecContainer));

    pDecCont->dwl = dwl;

    mpeg2API_InitDataStructures(pDecCont);

    pDecCont->ApiStorage.DecStat = INITIALIZED;
    pDecCont->ApiStorage.firstField = 1;

    *pDecInst = (DecContainer *) pDecCont;

    if( numFrameBuffers > 16 )
        numFrameBuffers = 16;
    pDecCont->StrmStorage.maxNumBuffers = numFrameBuffers;

    for(i = 0; i < DEC_X170_REGISTERS; i++)
        pDecCont->mpeg2Regs[i] = 0;

    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_OUT_ENDIAN,
                   DEC_X170_OUTPUT_PICTURE_ENDIAN);
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_IN_ENDIAN,
                   DEC_X170_INPUT_DATA_ENDIAN);
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_STRENDIAN_E,
                   DEC_X170_INPUT_STREAM_ENDIAN);
    /*
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_OUT_TILED_E,
                   DEC_X170_OUTPUT_FORMAT);
                   */
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_MAX_BURST,
                   DEC_X170_BUS_BURST_LENGTH);
    if ((G1DWLReadAsicID() >> 16) == 0x8170U)
    {
        SetDecRegister(pDecCont->mpeg2Regs, HWIF_PRIORITY_MODE,
                       DEC_X170_ASIC_SERVICE_PRIORITY);
    }
    else
    {
        SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_SCMD_DIS,
            DEC_X170_SCMD_DISABLE);
    }
    DEC_SET_APF_THRESHOLD(pDecCont->mpeg2Regs);
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_LATENCY,
                   DEC_X170_LATENCY_COMPENSATION);
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_DATA_DISC_E,
                   DEC_X170_DATA_DISCARD_ENABLE);
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_OUTSWAP32_E,
                   DEC_X170_OUTPUT_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_INSWAP32_E,
                   DEC_X170_INPUT_DATA_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_STRSWAP32_E,
                   DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);

#if( DEC_X170_HW_TIMEOUT_INT_ENA  != 0)
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_TIMEOUT_E, 1);
#else
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_TIMEOUT_E, 0);
#endif

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_CLK_GATE_E, 1);
#else
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_CLK_GATE_E, 0);
#endif

#if( DEC_X170_USING_IRQ  == 0)
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_IRQ_DIS, 1);
#else
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_IRQ_DIS, 0);
#endif

    /* set AXI RW IDs */
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_AXI_RD_ID,
                   (DEC_X170_AXI_ID_R & 0xFFU));
    SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_AXI_WR_ID,
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
            return MPEG2DEC_FORMAT_NOT_SUPPORTED;
        }
        pDecCont->tiledModeSupport = config.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;
    pDecCont->StrmStorage.intraFreeze = useVideoFreezeConcealment;
    pDecCont->StrmStorage.pictureBroken = HANTRO_FALSE;

    InitWorkarounds(MPEG2_DEC_X170_MODE_MPEG2, &pDecCont->workarounds);

    MPEG2DEC_API_DEBUG(("Container 0x%x\n", (u32) pDecCont));
    MPEG2FLUSH;
    MPEG2_API_TRC("Mpeg2DecInit: OK\n");

    return (MPEG2DEC_OK);
}

/*------------------------------------------------------------------------------

    Function: Mpeg2DecGetInfo()

        Functional description:
            This function provides read access to decoder information. This
            function should not be called before Mpeg2DecDecode function has
            indicated that headers are ready.

        Inputs:
            decInst     decoder instance

        Outputs:
            pDecInfo    pointer to info struct where data is written

        Returns:
            MPEG2DEC_OK            success
            MPEG2DEC_PARAM_ERROR     invalid parameters

------------------------------------------------------------------------------*/
Mpeg2DecRet Mpeg2DecGetInfo(Mpeg2DecInst decInst, Mpeg2DecInfo * pDecInfo)
{

#define API_STOR ((DecContainer *)decInst)->ApiStorage
#define DEC_FRAMED ((DecContainer *)decInst)->FrameDesc
#define DEC_STRM ((DecContainer *)decInst)->StrmDesc
#define DEC_STST ((DecContainer *)decInst)->StrmStorage
#define DEC_HDRS ((DecContainer *)decInst)->Hdrs
#define DEC_REGS ((DecContainer *)decInst)->mpeg2Regs

    MPEG2_API_TRC("Mpeg2DecGetInfo#");

    if(decInst == NULL || pDecInfo == NULL)
    {
        return MPEG2DEC_PARAM_ERROR;
    }

    if(API_STOR.DecStat == UNINIT || API_STOR.DecStat == INITIALIZED)
    {
        return MPEG2DEC_HDRS_NOT_RDY;
    }

    pDecInfo->frameWidth = DEC_FRAMED.frameWidth << 4;
    pDecInfo->frameHeight = DEC_FRAMED.frameHeight << 4;

    pDecInfo->codedWidth = DEC_HDRS.horizontalSize;
    pDecInfo->codedHeight = DEC_HDRS.verticalSize;

    pDecInfo->profileAndLevelIndication = DEC_HDRS.profileAndLevelIndication;
    pDecInfo->videoRange = DEC_HDRS.videoRange;
    pDecInfo->videoFormat = DEC_HDRS.videoFormat;
    pDecInfo->streamFormat = DEC_HDRS.mpeg2Stream;
    pDecInfo->interlacedSequence = DEC_HDRS.interlaced;
    /*pDecInfo->multiBuffPpSize = DEC_HDRS.interlaced ? 1 : 2;*/
    pDecInfo->multiBuffPpSize = 2;

    mpeg2DecAspectRatio((DecContainer *) decInst, pDecInfo);

    if(DEC_HDRS.interlaced)
    {
        pDecInfo->outputFormat = MPEG2DEC_SEMIPLANAR_YUV420;
    }
    else
    {
        pDecInfo->outputFormat =
            ((DecContainer *)decInst)->tiledReferenceEnable ? 
            MPEG2DEC_TILED_YUV420 : MPEG2DEC_SEMIPLANAR_YUV420;
    }

    MPEG2_API_TRC("Mpeg2DecGetInfo: OK");
    return (MPEG2DEC_OK);

#undef API_STOR
#undef DEC_STRM
#undef DEC_FRAMED
#undef DEC_STST
#undef DEC_HDRS
}

/*------------------------------------------------------------------------------

    Function: Mpeg2DecDecode

        Functional description:
            Decode stream data. Calls StrmDec_Decode to do the actual decoding.

        Input:
            decInst     decoder instance
            pInput      pointer to input struct

        Outputs:
            pOutput     pointer to output struct

        Returns:
            MPEG2DEC_NOT_INITIALIZED   decoder instance not initialized yet
            MPEG2DEC_PARAM_ERROR       invalid parameters

            MPEG2DEC_STRM_PROCESSED    stream buffer decoded
            MPEG2DEC_HDRS_RDY          headers decoded
            MPEG2DEC_PIC_DECODED       decoding of a picture finished
            MPEG2DEC_STRM_ERROR        serious error in decoding, no
                                       valid parameter sets available
                                       to decode picture data

------------------------------------------------------------------------------*/

Mpeg2DecRet Mpeg2DecDecode(Mpeg2DecInst decInst,
                           Mpeg2DecInput * pInput, Mpeg2DecOutput * pOutput)
{
#define API_STOR ((DecContainer *)decInst)->ApiStorage
#define DEC_STRM ((DecContainer *)decInst)->StrmDesc
#define DEC_FRAMED ((DecContainer *)decInst)->FrameDesc

    DecContainer *pDecCont;
    Mpeg2DecRet internalRet;
    u32 strmDecResult;
    u32 asicStatus;
    i32 ret = 0;
    u32 fieldRdy = 0;
    u32 errorConcealment = 0;

    MPEG2_API_TRC("\nMpeg2DecDecode#");

    if(pInput == NULL || pOutput == NULL || decInst == NULL)
    {
        MPEG2_API_TRC("Mpeg2DecDecode# PARAM_ERROR\n");
        return MPEG2DEC_PARAM_ERROR;
    }

    pDecCont = ((DecContainer *) decInst);

    /*
     *  Check if decoder is in an incorrect mode
     */
    if(API_STOR.DecStat == UNINIT)
    {

        MPEG2_API_TRC("Mpeg2DecDecode: NOT_INITIALIZED\n");
        return MPEG2DEC_NOT_INITIALIZED;
    }

    if(pInput->dataLen == 0 ||
       pInput->dataLen > DEC_X170_MAX_STREAM ||
       pInput->pStream == NULL || pInput->streamBusAddress == 0)
    {
        MPEG2_API_TRC("Mpeg2DecDecode# PARAM_ERROR\n");
        return MPEG2DEC_PARAM_ERROR;
    }

    /* If we have set up for delayed resolution change, do it here */
    if(pDecCont->StrmStorage.newHeadersChangeResolution)
    {
        pDecCont->StrmStorage.newHeadersChangeResolution = 0;
        pDecCont->Hdrs.horizontalSize = pDecCont->tmpHdrs.horizontalSize;
        pDecCont->Hdrs.verticalSize = pDecCont->tmpHdrs.verticalSize;
        /* Set rest of parameters just in case */
        pDecCont->Hdrs.aspectRatioInfo = pDecCont->tmpHdrs.aspectRatioInfo;
        pDecCont->Hdrs.frameRateCode = pDecCont->tmpHdrs.frameRateCode;
        pDecCont->Hdrs.bitRateValue = pDecCont->tmpHdrs.bitRateValue;
        pDecCont->Hdrs.vbvBufferSize = pDecCont->tmpHdrs.vbvBufferSize;
        pDecCont->Hdrs.constrParameters = pDecCont->tmpHdrs.constrParameters;
    }

    if(API_STOR.DecStat == HEADERSDECODED)
    {
        mpeg2FreeBuffers(pDecCont);        
        if(!pDecCont->StrmStorage.pPicBuf[0].data.busAddress)
        {
            MPEG2DEC_DEBUG(("Allocate buffers\n"));
            MPEG2FLUSH;
            internalRet = mpeg2AllocateBuffers(pDecCont);
            /* Reset frame number to ensure PP doesn't run in non-pipeline
             * mode during 1st pic of new headers. */
            pDecCont->FrameDesc.frameNumber = 0;
            if(internalRet != MPEG2DEC_OK)
            {
                MPEG2DEC_DEBUG(("ALLOC BUFFER FAIL\n"));
                MPEG2_API_TRC("Mpeg2DecDecode# MEMFAIL\n");
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
        MPEG2DEC_API_DEBUG(("Start Decode\n"));
        /* run SW if HW is not in the middle of processing a picture
         * (indicated as HW_PIC_STARTED decoder status) */
        if(API_STOR.DecStat != HW_PIC_STARTED)
        {
            strmDecResult = mpeg2StrmDec_Decode(pDecCont);
            if (pDecCont->unpairedField ||
                (strmDecResult != DEC_PIC_HDR_RDY &&
                 strmDecResult != DEC_END_OF_STREAM))
            {
                Mpeg2CheckReleasePpAndHw(pDecCont);
                pDecCont->unpairedField = 0;
            }

            switch (strmDecResult)
            {
            case DEC_PIC_HDR_RDY:
                pDecCont->StrmStorage.lastBSkipped = 0;
                /* if type inter predicted and no reference -> error */
                if((pDecCont->Hdrs.pictureCodingType == PFRAME &&
                    pDecCont->ApiStorage.firstField &&
                    pDecCont->StrmStorage.work0 == INVALID_ANCHOR_PICTURE) ||
                   (pDecCont->Hdrs.pictureCodingType == BFRAME &&
                    (pDecCont->StrmStorage.work0 == INVALID_ANCHOR_PICTURE ||
                     pDecCont->StrmStorage.skipB ||
                     pInput->skipNonReference)) ||
                   (pDecCont->Hdrs.pictureCodingType == PFRAME &&
                    pDecCont->StrmStorage.pictureBroken &&
                    pDecCont->StrmStorage.intraFreeze))
                {
                    if(pDecCont->StrmStorage.skipB ||
                       pInput->skipNonReference)
                    {
                        MPEG2_API_TRC("Mpeg2DecDecode# MPEG2DEC_NONREF_PIC_SKIPPED\n");
                    }
                    ret = mpeg2HandleVlcModeError(pDecCont, pInput->picId);
                    errorConcealment = HANTRO_TRUE;
                    Mpeg2CheckReleasePpAndHw(pDecCont);
                }
                else
                    API_STOR.DecStat = HW_PIC_STARTED;
                break;

            case DEC_PIC_SUPRISE_B:
                /* Handle suprise B */
                internalRet = mpeg2DecAllocExtraBPic(pDecCont);
                if(internalRet != MPEG2DEC_OK)
                {
                    MPEG2_API_TRC
                        ("Mpeg2DecDecode# MEMFAIL Mpeg2DecAllocExtraBPic\n");
                    return (internalRet);
                }

                pDecCont->Hdrs.lowDelay = 0;

                mpeg2DecBufferPicture(pDecCont,
                                      pInput->picId, 1, 0,
                                      MPEG2DEC_PIC_DECODED, 0);

                ret = mpeg2HandleVlcModeError(pDecCont, pInput->picId);
                errorConcealment = HANTRO_TRUE;
                /* copy output parameters for this PIC */
                MPEG2DEC_UPDATE_POUTPUT;
                break;

            case DEC_PIC_HDR_RDY_ERROR:
                ret = mpeg2HandleVlcModeError(pDecCont, pInput->picId);
                errorConcealment = HANTRO_TRUE;
                /* copy output parameters for this PIC */
                MPEG2DEC_UPDATE_POUTPUT;
                break;

            case DEC_HDRS_RDY:
                internalRet = mpeg2DecCheckSupport(pDecCont);
                if(internalRet != MPEG2DEC_OK)
                {
                    pDecCont->StrmStorage.strmDecReady = FALSE;
                    pDecCont->StrmStorage.validSequence = 0;
                    API_STOR.DecStat = INITIALIZED;
                    return internalRet;
                }

                if(pDecCont->ApiStorage.firstHeaders)
                {
                    pDecCont->ApiStorage.firstHeaders = 0;

                    SetDecRegister(pDecCont->mpeg2Regs, HWIF_PIC_MB_WIDTH,
                                   pDecCont->FrameDesc.frameWidth);

                    /* check the decoding mode */
                    if(pDecCont->Hdrs.mpeg2Stream)
                    {
                        SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_MODE,
                                       MPEG2_DEC_X170_MODE_MPEG2);
                    }
                    else
                    {
                        SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_MODE,
                                       MPEG2_DEC_X170_MODE_MPEG1);
                    }

                    if(pDecCont->refBufSupport)
                    {
                        RefbuInit(&pDecCont->refBufferCtrl,
                                  MPEG2_DEC_X170_MODE_MPEG2,
                                  pDecCont->FrameDesc.frameWidth,
                                  pDecCont->FrameDesc.frameHeight,
                                  pDecCont->refBufSupport);
                    }
                }

                /* Handle MPEG-1 parameters */
                if(pDecCont->Hdrs.mpeg2Stream == MPEG1)
                {
                    mpeg2HandleMpeg1Parameters(pDecCont);
                }

                API_STOR.DecStat = HEADERSDECODED;

                MPEG2DEC_API_DEBUG(("HDRS_RDY\n"));
                ret = MPEG2DEC_HDRS_RDY;
                break;

            default:
                ASSERT(strmDecResult == DEC_END_OF_STREAM);
                if(pDecCont->StrmStorage.newHeadersChangeResolution)
                {
                    ret = MPEG2DEC_PIC_DECODED;
                }
                else
                {
                    ret = MPEG2DEC_STRM_PROCESSED;
                }
                break;
            }
        }

        /* picture header properly decoded etc -> start HW */
        if(API_STOR.DecStat == HW_PIC_STARTED)
        {
            if(pDecCont->ApiStorage.firstField &&
               !pDecCont->asicRunning)
            {
                pDecCont->StrmStorage.workOutPrev =
                    pDecCont->StrmStorage.workOut;
                pDecCont->StrmStorage.workOut = BqueueNext( 
                    &pDecCont->StrmStorage.bq,
                    pDecCont->StrmStorage.work0,
                    pDecCont->Hdrs.lowDelay ? BQUEUE_UNUSED : pDecCont->StrmStorage.work1,
                    BQUEUE_UNUSED,
                    pDecCont->FrameDesc.picCodingType == BFRAME );

                if(pDecCont->FrameDesc.picCodingType == BFRAME)
                    pDecCont->StrmStorage.prevBIdx = pDecCont->StrmStorage.workOut;
            }

            if (pDecCont->workarounds.mpeg.startCode)
            {
                PrepareStartCodeWorkaround(
                   (u8*)pDecCont->StrmStorage.pPicBuf[
                        pDecCont->StrmStorage.workOut].data.virtualAddress,
                   pDecCont->FrameDesc.frameWidth,
                   pDecCont->FrameDesc.frameHeight,
                   pDecCont->Hdrs.pictureStructure == TOPFIELD);
            }

            asicStatus = RunDecoderAsic(pDecCont, pInput->streamBusAddress);

            /* check start code workout if applicable: if timeout interrupt
             * from HW, but all macroblocks written to output -> assume
             * picture finished -> change to pic rdy. Stream end address
             * indicated by HW is not properly updated, but is handled in
             * mpeg2HandleFrameEnd() */
            if ( !pDecCont->mmuEnable && (asicStatus & MPEG2_DEC_X170_IRQ_TIMEOUT) &&
                 pDecCont->workarounds.mpeg.startCode )
            {
                if ( ProcessStartCodeWorkaround(
                        (u8*)pDecCont->StrmStorage.pPicBuf[
                             pDecCont->StrmStorage.workOut].data.virtualAddress,
                        pDecCont->FrameDesc.frameWidth,
                        pDecCont->FrameDesc.frameHeight,
                        pDecCont->Hdrs.pictureStructure == TOPFIELD) ==
                     HANTRO_TRUE )
                {
                    asicStatus &= ~MPEG2_DEC_X170_IRQ_TIMEOUT;
                    asicStatus |= MPEG2_DEC_X170_IRQ_DEC_RDY;
                }
            }

            if(asicStatus == ID8170_DEC_TIMEOUT)
            {
				if(pDecCont->mmuEnable)
				{
					DWLLibReset(pDecCont->dwl);
                	ret = MPEG2DEC_HW_BUS_ERROR;
				}
				else
				{
                	ret = MPEG2DEC_HW_TIMEOUT;
				}
            }
            else if(asicStatus == ID8170_DEC_SYSTEM_ERROR)
            {
                ret = MPEG2DEC_SYSTEM_ERROR;
            }
            else if(asicStatus == ID8170_DEC_HW_RESERVED)
            {
                ret = MPEG2DEC_HW_RESERVED;
            }
            else if(asicStatus & MPEG2_DEC_X170_IRQ_TIMEOUT)
            {
                MPEG2DEC_API_DEBUG(("IRQ TIMEOUT IN HW\n"));
				if(pDecCont->mmuEnable)
				{
					DWLLibReset(pDecCont->dwl);
                	ret = MPEG2DEC_HW_BUS_ERROR;
				}
				else
				{
                    MPEG2FLUSH;
                    ret = mpeg2HandleVlcModeError(pDecCont, pInput->picId);
                    errorConcealment = HANTRO_TRUE;
                    MPEG2DEC_UPDATE_POUTPUT;
				}
            }
            else if(asicStatus & MPEG2_DEC_X170_IRQ_BUS_ERROR)
            {
                ret = MPEG2DEC_HW_BUS_ERROR;
            }
            else if(asicStatus & MPEG2_DEC_X170_IRQ_STREAM_ERROR)
            {
                MPEG2DEC_API_DEBUG(("STREAM ERROR IN HW\n"));
                MPEG2FLUSH;
                ret = mpeg2HandleVlcModeError(pDecCont, pInput->picId);
                errorConcealment = HANTRO_TRUE;
                MPEG2DEC_UPDATE_POUTPUT;
            }
            else if(asicStatus & MPEG2_DEC_X170_IRQ_BUFFER_EMPTY)
            {
                mpeg2DecPreparePicReturn(pDecCont);
                ret = MPEG2DEC_STRM_PROCESSED;

            }
            /* HW finished decoding a picture */
            else if(asicStatus & MPEG2_DEC_X170_IRQ_DEC_RDY)
            {
                if(pDecCont->Hdrs.pictureStructure == FRAMEPICTURE ||
                   !pDecCont->ApiStorage.firstField)
                {
                    pDecCont->FrameDesc.frameNumber++;

                    mpeg2HandleFrameEnd(pDecCont);

                    mpeg2DecBufferPicture(pDecCont,
                                          pInput->picId,
                                          pDecCont->Hdrs.
                                          pictureCodingType == BFRAME,
                                          pDecCont->Hdrs.
                                          pictureCodingType == PFRAME,
                                          MPEG2DEC_PIC_DECODED, 0);

                    ret = MPEG2DEC_PIC_DECODED;

                    pDecCont->ApiStorage.firstField = 1;
                    if(pDecCont->Hdrs.pictureCodingType != BFRAME)
                    {
                        /*if(pDecCont->Hdrs.lowDelay == 0)*/
                        {
                            pDecCont->StrmStorage.work1 =
                                pDecCont->StrmStorage.work0;
                        }
                        pDecCont->StrmStorage.work0 =
                            pDecCont->StrmStorage.workOut;
                        if(pDecCont->StrmStorage.skipB)
                            pDecCont->StrmStorage.skipB--;
                    }

                    if(pDecCont->Hdrs.pictureCodingType == IFRAME)
                        pDecCont->StrmStorage.pictureBroken = HANTRO_FALSE;
                }
                else
                {
                    fieldRdy = 1;
                    mpeg2HandleFrameEnd(pDecCont);
                    pDecCont->ApiStorage.firstField = 0;
                    if((u32) (pDecCont->StrmDesc.pStrmCurrPos -
                              pDecCont->StrmDesc.pStrmBuffStart) >=
                       pInput->dataLen)
                    {
                        ret = MPEG2DEC_STRM_PROCESSED;
                    }
                }
                pDecCont->StrmStorage.validPicHeader = FALSE;
                pDecCont->StrmStorage.validPicExtHeader = FALSE;

                /* handle first field indication */
                if(pDecCont->Hdrs.interlaced)
                {
                    if(pDecCont->Hdrs.pictureStructure != FRAMEPICTURE)
                        pDecCont->Hdrs.fieldIndex++;
                    else
                        pDecCont->Hdrs.fieldIndex = 1;

                    pDecCont->Hdrs.firstFieldInFrame++;
                }

                mpeg2DecPreparePicReturn(pDecCont);
            }
            else
            {
                ASSERT(0);
            }
            if(ret != MPEG2DEC_STRM_PROCESSED || fieldRdy)
            {
                API_STOR.DecStat = STREAMDECODING;
            }

            if(ret == MPEG2DEC_PIC_DECODED || ret == MPEG2DEC_STRM_PROCESSED)
            {
                /* copy output parameters for this PIC (excluding stream pos) */
                pDecCont->MbSetDesc.outData.pStrmCurrPos =
                    pOutput->pStrmCurrPos;
                MPEG2DEC_UPDATE_POUTPUT;
            }
        }
    }
    while(ret == 0);

    if(errorConcealment && pDecCont->Hdrs.pictureCodingType != BFRAME)
    {
        pDecCont->StrmStorage.pictureBroken = HANTRO_TRUE;
    }

    MPEG2_API_TRC("Mpeg2DecDecode: Exit\n");
    pOutput->pStrmCurrPos = pDecCont->StrmDesc.pStrmCurrPos;
    pOutput->strmCurrBusAddress = pInput->streamBusAddress +
        (pDecCont->StrmDesc.pStrmCurrPos - pDecCont->StrmDesc.pStrmBuffStart);
    pOutput->dataLeft = pDecCont->StrmDesc.strmBuffSize -
        (pOutput->pStrmCurrPos - DEC_STRM.pStrmBuffStart);

    return ((Mpeg2DecRet) ret);

#undef API_STOR
#undef DEC_STRM
#undef DEC_FRAMED

}

/*------------------------------------------------------------------------------

    Function: Mpeg2DecRelease()

        Functional description:
            Release the decoder instance.

        Inputs:
            decInst     Decoder instance

        Outputs:
            none

        Returns:
            none

------------------------------------------------------------------------------*/

void Mpeg2DecRelease(Mpeg2DecInst decInst)
{
#define API_STOR ((DecContainer *)decInst)->ApiStorage
    DecContainer *pDecCont = NULL;
    const void *dwl;

    MPEG2DEC_DEBUG(("1\n"));
    MPEG2_API_TRC("Mpeg2DecRelease#");
    if(decInst == NULL)
    {
        MPEG2_API_TRC("Mpeg2DecRelease# ERROR: decInst == NULL");
        return;
    }

    pDecCont = ((DecContainer *) decInst);
    dwl = pDecCont->dwl;

    if (pDecCont->asicRunning)
        (void) G1G1DWLReleaseHw(pDecCont->dwl);

    mpeg2FreeBuffers(pDecCont);

    G1DWLfree(pDecCont);

    (void) G1DWLRelease(dwl);

    MPEG2_API_TRC("Mpeg2DecRelease: OK");
#undef API_STOR
}

/*------------------------------------------------------------------------------

    Function: mpeg2RegisterPP()

        Functional description:
            Register the pp for mpeg-2 pipeline

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

i32 mpeg2RegisterPP(const void *decInst, const void *ppInst,
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

    Function: mpeg2UnregisterPP()

        Functional description:
            Unregister the pp from mpeg-2 pipeline

        Inputs:
            decInst     Decoder instance
            const void  *ppInst - post-processor instance

        Outputs:
            none

        Returns:
            i32 - return 0 for success or a negative error code

------------------------------------------------------------------------------*/

i32 mpeg2UnregisterPP(const void *decInst, const void *ppInst)
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
    Function name   : mpeg2RefreshRegs
    Description     :
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void mpeg2RefreshRegs(DecContainer * pDecCont)
{
    i32 i;
    u32 *ppRegs = pDecCont->mpeg2Regs;

    for(i = 0; i < DEC_X170_REGISTERS; i++)
    {
        ppRegs[i] = G1DWLReadReg(pDecCont->dwl, 4 * i);
    }
}

/*------------------------------------------------------------------------------
    Function name   : mpeg2FlushRegs
    Description     :
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void mpeg2FlushRegs(DecContainer * pDecCont)
{
    i32 i;
    u32 *ppRegs = pDecCont->mpeg2Regs;

    for(i = 2; i < DEC_X170_REGISTERS; i++)
    {
        G1DWLWriteReg(pDecCont->dwl, 4 * i, ppRegs[i]);
        ppRegs[i] = 0;
    }
}

/*------------------------------------------------------------------------------
    Function name   : mpeg2HandleVlcModeError
    Description     :
    Return type     : u32
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
u32 mpeg2HandleVlcModeError(DecContainer * pDecCont, u32 picNum)
{
    u32 ret = 0, tmp;

    ASSERT(pDecCont->StrmStorage.strmDecReady);

    tmp = mpeg2StrmDec_NextStartCode(pDecCont);
    if(tmp != END_OF_STREAM)
    {
        pDecCont->StrmDesc.pStrmCurrPos -= 4;
        pDecCont->StrmDesc.strmBuffReadBits -= 32;
    }

    /* error in first picture -> set reference to grey */
    if(!pDecCont->FrameDesc.frameNumber)
    {
        (void) G1DWLmemset(pDecCont->StrmStorage.
                         pPicBuf[pDecCont->StrmStorage.workOut].data.
                         virtualAddress, 128,
                         384 * pDecCont->FrameDesc.totalMbInFrame);

        mpeg2DecPreparePicReturn(pDecCont);

        /* no pictures finished -> return STRM_PROCESSED */
        if(tmp == END_OF_STREAM)
        {
            ret = MPEG2DEC_STRM_PROCESSED;
        }
        else
            ret = 0;
        pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.workOut;
        pDecCont->StrmStorage.skipB = 2;
    }
    else
    {
        if(pDecCont->Hdrs.pictureCodingType != BFRAME)
        {
            pDecCont->FrameDesc.frameNumber++;

            /* reset sendToPp to prevent post-processing partly decoded
             * pictures */
            if(pDecCont->StrmStorage.workOut != pDecCont->StrmStorage.work0)
                pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].
                    sendToPp = 0;

            pDecCont->StrmStorage.workOutPrev = pDecCont->StrmStorage.workOut;
            pDecCont->StrmStorage.workOut = pDecCont->StrmStorage.work0;

            mpeg2DecBufferPicture(pDecCont,
                                  picNum,
                                  pDecCont->Hdrs.pictureCodingType == BFRAME,
                                  1, (Mpeg2DecRet) FREEZED_PIC_RDY,
                                  pDecCont->FrameDesc.totalMbInFrame);

            ret = MPEG2DEC_PIC_DECODED;

            /*if(pDecCont->Hdrs.lowDelay == 0)*/
            {
                pDecCont->StrmStorage.work1 = pDecCont->StrmStorage.work0;
            }
            pDecCont->StrmStorage.skipB = 2;
        }
        else
        {
            if(pDecCont->StrmStorage.intraFreeze)
            {
                pDecCont->FrameDesc.frameNumber++;
                mpeg2DecBufferPicture(pDecCont,
                                      picNum,
                                      pDecCont->Hdrs.pictureCodingType ==
                                      BFRAME, 1, (Mpeg2DecRet) FREEZED_PIC_RDY,
                                      pDecCont->FrameDesc.totalMbInFrame);

                ret = MPEG2DEC_PIC_DECODED;

            }
            else
            {
                ret = MPEG2DEC_NONREF_PIC_SKIPPED; 
                pDecCont->StrmStorage.lastBSkipped = 1;
            }
            pDecCont->StrmStorage.workOutPrev = pDecCont->StrmStorage.work0;
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.prevBIdx].sendToPp = 0;
        }
    }

    /* picture freezed due to error in first field -> skip/ignore 2. field */
    if(pDecCont->Hdrs.pictureStructure != FRAMEPICTURE &&
       pDecCont->Hdrs.pictureCodingType != BFRAME &&
       pDecCont->ApiStorage.firstField)
    {
        pDecCont->ApiStorage.ignoreField = 1;
        pDecCont->ApiStorage.firstField = 0;
    }
    else
        pDecCont->ApiStorage.firstField = 1;

    pDecCont->ApiStorage.DecStat = STREAMDECODING;
    pDecCont->StrmStorage.validPicHeader = FALSE;
    pDecCont->StrmStorage.validPicExtHeader = FALSE;
    pDecCont->Hdrs.pictureStructure = FRAMEPICTURE;

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : mpeg2HandleFrameEnd
    Description     :
    Return type     : u32
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void mpeg2HandleFrameEnd(DecContainer * pDecCont)
{

    u32 tmp;

    pDecCont->StrmDesc.strmBuffReadBits =
        8 * (pDecCont->StrmDesc.pStrmCurrPos -
             pDecCont->StrmDesc.pStrmBuffStart);
    pDecCont->StrmDesc.bitPosInWord = 0;

    do
    {
        tmp = mpeg2StrmDec_ShowBits(pDecCont, 32);
        if((tmp >> 8) == 0x1)
            break;
    }
    while(mpeg2StrmDec_FlushBits(pDecCont, 8) == HANTRO_OK);

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

    MPEG2FLUSH;

    ASSERT(pDecContainer->StrmStorage.
           pPicBuf[pDecContainer->StrmStorage.workOut].data.busAddress != 0);
    ASSERT(strmBusAddress != 0);

    /* q-tables to buffer */
    mpeg2HandleQTables(pDecContainer);

    /* set pp luma bus */
    pDecContainer->ppControl.inputBusLuma = 0;

    if(!pDecContainer->asicRunning)
    {
        tmp = Mpeg2SetRegs(pDecContainer, strmBusAddress);
        if(tmp == HANTRO_NOK)
            return 0;

        if (!pDecContainer->keepHwReserved)
            (void) G1DWLReserveHw(pDecContainer->dwl);

        /* Start PP */
        if(pDecContainer->ppInstance != NULL)
        {
            Mpeg2PpControl(pDecContainer, 0);
        }
        else
        {
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_OUT_DIS, 0);
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FILTERING_DIS, 1);
        }

        pDecContainer->asicRunning = 1;

        G1DWLWriteReg(pDecContainer->dwl, 0x4, 0);

        mpeg2FlushRegs(pDecContainer);

        /* Enable HW */
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_E, 1);
        DWLEnableHW(pDecContainer->dwl, 4 * 1, pDecContainer->mpeg2Regs[1]);
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

        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_RLC_VLC_BASE, tmp & ~0x7);
        /* amount of stream (as seen by the HW), obtained as amount of stream
         * given by the application subtracted by number of bytes decoded by
         * SW (if strmBusAddress is not 64-bit aligned -> adds number of bytes
         * from previous 64-bit aligned boundary) */
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_STREAM_LEN,
                       pDecContainer->StrmDesc.strmBuffSize -
                       ((tmp & ~0x7) - strmBusAddress));
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_STRM_START_BIT,
                       pDecContainer->StrmDesc.bitPosInWord + 8 * (tmp & 0x7));

        /* This depends on actual register allocation */
        G1DWLWriteReg(pDecContainer->dwl, 4 * 5, pDecContainer->mpeg2Regs[5]);
        G1DWLWriteReg(pDecContainer->dwl, 4 * 6, pDecContainer->mpeg2Regs[6]);
        G1DWLWriteReg(pDecContainer->dwl, 4 * 12, pDecContainer->mpeg2Regs[12]);

        G1DWLWriteReg(pDecContainer->dwl, 4 * 1, pDecContainer->mpeg2Regs[1]);
    }

    /* Wait for HW ready */
    MPEG2DEC_API_DEBUG(("Wait for Decoder\n"));
    ret = G1DWLWaitHwReady(pDecContainer->dwl, (u32) DEC_X170_TIMEOUT_LENGTH);

    mpeg2RefreshRegs(pDecContainer);

    if(ret == DWL_HW_WAIT_OK)
    {
        asicStatus =
            GetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_IRQ_STAT);
    }
    else if(ret == DWL_HW_WAIT_TIMEOUT)
    {
        asicStatus = ID8170_DEC_TIMEOUT;
    }
    else
    {
        asicStatus = ID8170_DEC_SYSTEM_ERROR;
    }

    if(!(asicStatus & MPEG2_DEC_X170_IRQ_BUFFER_EMPTY) ||
       (asicStatus & MPEG2_DEC_X170_IRQ_STREAM_ERROR) ||
       (asicStatus & MPEG2_DEC_X170_IRQ_BUS_ERROR) ||
       (asicStatus == ID8170_DEC_TIMEOUT) ||
       (asicStatus == ID8170_DEC_SYSTEM_ERROR))
    {
        /* reset HW */
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_IRQ_STAT, 0);
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_IRQ, 0);

        DWLDisableHW(pDecContainer->dwl, 4 * 1, pDecContainer->mpeg2Regs[1]);

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
                       (asicStatus & MPEG2_DEC_X170_IRQ_DEC_RDY))
                    {
                        pDecContainer->ppControl.ppStatus = DECPP_PIC_NOT_FINISHED;
                        pDecContainer->keepHwReserved = 1;
                    }
                    else
                    {
                        pDecContainer->ppControl.ppStatus = DECPP_PIC_READY;
                        TRACE_PP_CTRL("RunDecoderAsic: PP Wait for end\n");
                        pDecContainer->PPEndCallback(pDecContainer->ppInstance);
                        TRACE_PP_CTRL("RunDecoderAsic: PP Finished\n");
                    }
                }
            }
            else
            {
                /* End PP co-operation */
                if(pDecContainer->ppControl.ppStatus == DECPP_RUNNING)
                {
                    MPEG2DEC_API_DEBUG(("RunDecoderAsic: PP Wait for end\n"));
                    if(pDecContainer->ppInstance != NULL)
                        pDecContainer->PPEndCallback(pDecContainer->ppInstance);
                    MPEG2DEC_API_DEBUG(("RunDecoderAsic: PP Finished\n"));

                    if((asicStatus & MPEG2_DEC_X170_IRQ_STREAM_ERROR) &&
                       pDecContainer->ppControl.usePipeline)
                    {
                        pDecContainer->ppControl.ppStatus = DECPP_IDLE;
                    }
                    else
                    {
                        pDecContainer->ppControl.ppStatus = DECPP_PIC_READY;
                    }
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
        (MPEG2_DEC_X170_IRQ_BUFFER_EMPTY | MPEG2_DEC_X170_IRQ_DEC_RDY |
         MPEG2_DEC_X170_IRQ_TIMEOUT)))
    {
        tmp = GetDecRegister(pDecContainer->mpeg2Regs, HWIF_RLC_VLC_BASE);
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
        /* if timeout interrupt and no bytes consumed by HW -> advance one
         * byte to prevent processing current slice again (may only happen
         * in slice-by-slice mode) */
        if ( (asicStatus & MPEG2_DEC_X170_IRQ_TIMEOUT) &&
             tmp == strmBusAddress &&
             pDecContainer->StrmDesc.pStrmCurrPos <
                (pDecContainer->StrmDesc.pStrmBuffStart +
                 pDecContainer->StrmDesc.strmBuffSize) )
            pDecContainer->StrmDesc.pStrmCurrPos++;
    }

    if( pDecContainer->Hdrs.pictureCodingType != BFRAME &&
        pDecContainer->refBufSupport &&
        (asicStatus & MPEG2_DEC_X170_IRQ_DEC_RDY) &&
        pDecContainer->asicRunning == 0)
    {
        RefbuMvStatistics( &pDecContainer->refBufferCtrl, 
                            pDecContainer->mpeg2Regs,
                            NULL,
                            HANTRO_FALSE,
                            pDecContainer->Hdrs.pictureCodingType == IFRAME );
    }

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_IRQ_STAT, 0);

    return asicStatus;

}

/*------------------------------------------------------------------------------

    Function name: Mpeg2DecNextPicture

    Functional description:
        Retrieve next decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct
        endOfStream Indicates whether end of stream has been reached

    Output:
        pPicture Decoder output picture.

    Return values:
        MPEG2DEC_OK         No picture available.
        MPEG2DEC_PIC_RDY    Picture ready.

------------------------------------------------------------------------------*/
Mpeg2DecRet Mpeg2DecNextPicture(Mpeg2DecInst decInst,
                                Mpeg2DecPicture * pPicture, u32 endOfStream)
{
    /* Variables */
    Mpeg2DecRet returnValue = MPEG2DEC_PIC_RDY;
    DecContainer *pDecCont;
    picture_t *pPic;
    u32 picIndex = MPEG2_BUFFER_UNDEFINED;
    u32 minCount;
    u32 tmp = 0;
    u32 luma = 0;
    u32 chroma = 0;
    u32 parallelMode2Flag = 0; /* */

    /* Code */
    MPEG2_API_TRC("\nMpeg2DecNextPicture#");

    /* Check that function input parameters are valid */
    if(pPicture == NULL)
    {
        MPEG2_API_TRC("Mpeg2DecNextPicture# ERROR: pPicture is NULL");
        return (MPEG2DEC_PARAM_ERROR);
    }

    pDecCont = (DecContainer *) decInst;

    /* Check if decoder is in an incorrect mode */
    if(decInst == NULL || pDecCont->ApiStorage.DecStat == UNINIT)
    {
        MPEG2_API_TRC("Mpeg2DecNextPicture# ERROR: Decoder not initialized");
        return (MPEG2DEC_NOT_INITIALIZED);
    }

    minCount = 0;
    if(pDecCont->Hdrs.lowDelay == 0 && !endOfStream &&
       !pDecCont->StrmStorage.newHeadersChangeResolution)
        minCount = 1;

    if(pDecCont->StrmStorage.parallelMode2 && !endOfStream &&
        !pDecCont->StrmStorage.newHeadersChangeResolution)
        minCount = 2;

    /* this is to prevent post-processing of non-finished pictures in the
     * end of the stream */
    if(endOfStream && pDecCont->FrameDesc.picCodingType == BFRAME)
    {
        pDecCont->FrameDesc.picCodingType = PFRAME;
        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.prevBIdx].sendToPp = 0;
    }

    /* Nothing to send out */
    if(pDecCont->StrmStorage.outCount <= minCount)
    {
        (void) G1DWLmemset(pPicture, 0, sizeof(Mpeg2DecPicture));
        pPicture->pOutputPicture = NULL;
        pPicture->interlaced = pDecCont->Hdrs.interlaced;

	//here is a bug because on2 do not consider that we will apply their decode lib in multimedia framework
	//In Directshow or opencore,when seek,we should flush all picture out using NextPicture(instance,pic,1),
	//but the stream is not end yet,so we should
	//not make the pDecCont->StrmStorage.pm2AllProcessedFlag flag always set on,but to reset it to zero when we use 
	//parallelmode2 to do decode @by franklin @infotmic 2010/10/22
	if(pDecCont->StrmStorage.pm2AllProcessedFlag && pDecCont->StrmStorage.parallelMode2){
		pDecCont->StrmStorage.pm2AllProcessedFlag =0;
	} 

        /* print nothing to send out */
        returnValue = MPEG2DEC_OK;
    }
    else
    {
        picIndex = pDecCont->StrmStorage.outIndex;
        picIndex = pDecCont->StrmStorage.outBuf[picIndex];

        Mpeg2FillPicStruct(pPicture, pDecCont, picIndex);

        /* field output */
        if(MPEG2DEC_IS_FIELD_OUTPUT)
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
            pPicture->interlaced = pDecCont->Hdrs.interlaced;
            pPicture->topField = 0;
            pPicture->fieldPicture = 0;
            pDecCont->StrmStorage.outCount--;
            pDecCont->StrmStorage.outIndex++;
            pDecCont->StrmStorage.outIndex &= 15;
        }
    }

    if(pDecCont->ppInstance &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE) &&
       endOfStream && (returnValue == MPEG2DEC_PIC_RDY))
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
    if(pDecCont->ppInstance && returnValue == MPEG2DEC_PIC_RDY &&
       (pDecCont->ppControl.multiBufStat != MULTIBUFFER_FULLMODE))
    {
        /* pp and decoder running in parallel, decoder finished first field ->
         * decode second field and wait PP after that */
        if(pDecCont->ppInstance != NULL &&
           pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED)
        {
            return (MPEG2DEC_OK);
        }

        /* In parallel mode 2, this is the last frame to display */
        if(pDecCont->StrmStorage.pm2AllProcessedFlag &&
            !(MPEG2DEC_IS_FIELD_OUTPUT && 
                (pDecCont->ApiStorage.bufferForPp != NO_BUFFER)))
        {
            picIndex = pDecCont->StrmStorage.work0;
            pDecCont->ppControl.displayIndex = 
                pDecCont->ppControl.bufferIndex = 
                pDecCont->StrmStorage.pm2lockBuf;
            pDecCont->PPDisplayIndex(pDecCont->ppInstance,
                                     pDecCont->ppControl.displayIndex);
            if(MPEG2DEC_IS_FIELD_OUTPUT)
            {
                pPicture->interlaced = 1;
                pPicture->fieldPicture = 1;
                pPicture->topField =
                    pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 1 : 0;

                Mpeg2DecPrepareFieldProcessing(pDecCont, picIndex);
            }

            Mpeg2FillPicStruct(pPicture, pDecCont, picIndex);
            returnValue = MPEG2DEC_PIC_RDY;
            pDecCont->ppControl.ppStatus = DECPP_IDLE;
        }
        else if(pDecCont->ppControl.ppStatus == DECPP_PIC_READY &&
                (!(pDecCont->StrmStorage.parallelMode2 && endOfStream)))
        {
            if(MPEG2DEC_IS_FIELD_OUTPUT)
            {
                pPicture->interlaced = 1;
                pPicture->fieldPicture = 1;
                pPicture->topField =
                    pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 1 : 0;
            }
            Mpeg2FillPicStruct(pPicture, pDecCont, picIndex);
            returnValue = MPEG2DEC_PIC_RDY;
            pDecCont->ppControl.ppStatus = DECPP_IDLE;
        }
        else
        {
            pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;
            returnValue = MPEG2DEC_OK;

            if((pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED) ||
               ((pDecCont->ppControl.multiBufStat == MULTIBUFFER_SEMIMODE) &&
                MPEG2DEC_IS_FIELD_OUTPUT))
            {
                picIndex = MPEG2_BUFFER_UNDEFINED;
            }

            if(MPEG2DEC_NON_PIPELINE_AND_B_PICTURE &&
                !pDecCont->StrmStorage.parallelMode2)
            {
                /* send index 2 (B Picture output) to PP) */
                picIndex = pDecCont->StrmStorage.prevBIdx;
                pDecCont->FrameDesc.picCodingType = IFRAME;

                /* Set here field decoding for first field of a B picture */
                if(MPEG2DEC_IS_FIELD_OUTPUT)
                {
                    if( picIndex >= 0 &&
                        picIndex < pDecCont->StrmStorage.numBuffers )
                    {
                        if(!pDecCont->StrmStorage.parallelMode2)
                            pDecCont->ApiStorage.bufferForPp = 1+picIndex;
                    }
                    else
                    {
                        ASSERT(0);
                    }
                    pPicture->interlaced = 1;
                    pPicture->fieldPicture = 1;
                    MPEG2DEC_API_DEBUG(("Processing first field in NextPicture %d\n", picIndex));
                    pPicture->topField =
                        pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 1 : 0;
                    pDecCont->ppControl.picStruct =
                        pDecCont->StrmStorage.pPicBuf[picIndex].
                        tf ? DECPP_PIC_TOP_FIELD_FRAME :
                        DECPP_PIC_BOT_FIELD_FRAME;

                    /* first field */
                    if(pDecCont->ppControl.picStruct ==
                       DECPP_PIC_BOT_FIELD_FRAME)
                    {
                        pDecCont->ppControl.bottomBusLuma =
                            pDecCont->StrmStorage.pPicBuf[picIndex].data.
                            busAddress + (pDecCont->ppControl.inwidth << 4);

                        pDecCont->ppControl.bottomBusChroma =
                            pDecCont->StrmStorage.pPicBuf[picIndex].
                            data.busAddress + ((pDecCont->FrameDesc.frameWidth *
                                                pDecCont->FrameDesc.
                                                frameHeight) << 8);

                    }
                }
            }
            else if(MPEG2DEC_IS_FIELD_OUTPUT &&
                    (pDecCont->ApiStorage.bufferForPp != NO_BUFFER))
            {
                pPicture->interlaced = 1;
                pPicture->fieldPicture = 1;
                picIndex = pDecCont->ApiStorage.bufferForPp - 1;

                pDecCont->ppControl.picStruct =
                    pDecCont->StrmStorage.pPicBuf[picIndex].
                    tf ? DECPP_PIC_BOT_FIELD_FRAME : DECPP_PIC_TOP_FIELD_FRAME;

                /* Restore correct buffer index */
                if(pDecCont->StrmStorage.parallelMode2 &&
                   pDecCont->ppControl.bufferIndex != pDecCont->ppControl.displayIndex)
                {
                    pDecCont->ppControl.bufferIndex = 
                        pDecCont->ppControl.displayIndex;
                    picIndex = pDecCont->StrmStorage.work1;
                }

                if(!pDecCont->Hdrs.topFieldFirst)
                {
                    pDecCont->ppControl.inputBusLuma =
                        pDecCont->StrmStorage.pPicBuf[picIndex].data.busAddress;

                    pDecCont->ppControl.inputBusChroma =
                        pDecCont->ppControl.inputBusLuma +
                        ((pDecCont->FrameDesc.frameWidth *
                          pDecCont->FrameDesc.frameHeight) << 8);
                }
                else
                {

                    pDecCont->ppControl.bottomBusLuma =
                        pDecCont->StrmStorage.pPicBuf[picIndex].data.busAddress +
                        (pDecCont->FrameDesc.frameWidth << 4);

                    pDecCont->ppControl.bottomBusChroma =
                        pDecCont->ppControl.bottomBusLuma +
                        ((pDecCont->FrameDesc.frameWidth *
                          pDecCont->FrameDesc.frameHeight) << 8);
                }

                pPicture->topField =
                    pDecCont->StrmStorage.pPicBuf[picIndex].tf ? 0 : 1;                
                pDecCont->ApiStorage.bufferForPp = NO_BUFFER;
                pPic[picIndex].sendToPp = 1;
                MPEG2DEC_API_DEBUG(("Processing second field in NextPicture index %d\n", picIndex));
            }
            else if(endOfStream)
            {

                /* In parallel mode 2, at end of stream there are 2 PP output 
                 * pictures left to show. If previous decoded picture was B
                 * picture, we need to run that through PP now */
                if( pDecCont->StrmStorage.parallelMode2 &&
                    !pDecCont->StrmStorage.pm2AllProcessedFlag)
                {
                    /* Set flag to signal PP has run through every frame
                     * it has to. Also set previous PP picture as "locked",
                     * so we now to show it last. */
                    pDecCont->StrmStorage.pm2AllProcessedFlag = 1;
                    pDecCont->ppControl.bufferIndex = 
                        BqueueNext( &pDecCont->StrmStorage.bqPp,
                            pDecCont->StrmStorage.pm2lockBuf,
                            BQUEUE_UNUSED,
                            BQUEUE_UNUSED,
                            0 );

                    /* Connect PP output buffer to decoder output buffer */
                    {
                        u32 workBuffer;
                        u32 luma = 0;
                        u32 chroma = 0;

                        workBuffer = pDecCont->StrmStorage.workOutPrev;

                        luma = pDecCont->StrmStorage.
                            pPicBuf[workBuffer].data.busAddress;
                        chroma = luma + ((pDecCont->FrameDesc.frameWidth *
                                 pDecCont->FrameDesc.frameHeight) << 8);

                        pDecCont->PPBufferData(pDecCont->ppInstance, 
                            pDecCont->ppControl.bufferIndex, luma, chroma);
                    }

                    /* If previous picture was B, we send that to PP now */
                    if(pDecCont->StrmStorage.previousB)
                    {
                        parallelMode2Flag = 1;
                        pDecCont->StrmStorage.previousB = 0;
                        if (!pDecCont->StrmStorage.lastBSkipped)
                        {
                            picIndex = pDecCont->StrmStorage.prevBIdx;
                            pPic[picIndex].sendToPp = 2;
                            pDecCont->ppControl.displayIndex = 
                                pDecCont->ppControl.bufferIndex;
                        }
                        /* last picture was B and was never finished -> all
                         * pictures already processed */
                        else
                        {
                            pDecCont->PPDisplayIndex(pDecCont->ppInstance,
                                                     pDecCont->ppControl.displayIndex);
                            returnValue = MPEG2DEC_PIC_RDY;
                        }
                    }
                    /* ...otherwise we send the previous anchor. */
                    else
                    {
                        picIndex = pDecCont->StrmStorage.work0;
                        pPic[picIndex].sendToPp = 2;
                        parallelMode2Flag = 1;
                        pDecCont->ppControl.displayIndex = 
                            pDecCont->StrmStorage.pm2lockBuf;
                        pDecCont->StrmStorage.pm2lockBuf =
                            pDecCont->ppControl.bufferIndex;
                        /* Fix for case if stream has only one frame; then 
                         * we should have a proper buffer index instead of 
                         * undefined. */
                        if(pDecCont->ppControl.displayIndex == BQUEUE_UNUSED)
                            pDecCont->ppControl.displayIndex = 
                                pDecCont->ppControl.bufferIndex;
                        pDecCont->PPDisplayIndex(pDecCont->ppInstance,
                                                 pDecCont->ppControl.displayIndex);
                    }
                    if(MPEG2DEC_IS_FIELD_OUTPUT)
                        Mpeg2DecPrepareFieldProcessing(pDecCont, picIndex);
                }
                else if(pDecCont->Hdrs.lowDelay == 0)
                {
                    picIndex = pDecCont->StrmStorage.work0;
                    pPic[picIndex].sendToPp = 2;
                }
                else
                    return MPEG2DEC_OK;
                /*
                 * picIndex = 0;
                 * while((picIndex < 3) && !pPic[picIndex].sendToPp)
                 * picIndex++;
                 * if (picIndex == 3)
                 * return MPEG2DEC_OK;
                 */

                if(MPEG2DEC_IS_FIELD_OUTPUT)
                {
                    /* if field output, other field must be processed also */
                    if( picIndex >= 0 &&
                        picIndex < pDecCont->StrmStorage.numBuffers )
                    {
                        if(!pDecCont->StrmStorage.parallelMode2)
                            pDecCont->ApiStorage.bufferForPp = 1+picIndex;
                    }
                    else
                    {
                        ASSERT(0);
                    }

                    /* set field processing */
                    pDecCont->ppControl.picStruct =
                        pDecCont->StrmStorage.pPicBuf[picIndex].tf ?
                        DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;

                    /* first field */
                    if(pDecCont->ppControl.picStruct ==
                       DECPP_PIC_BOT_FIELD_FRAME)
                    {
                        pDecCont->ppControl.bottomBusLuma =
                            pDecCont->StrmStorage.pPicBuf[picIndex].data.
                            busAddress + (pDecCont->ppControl.inwidth << 4);

                        pDecCont->ppControl.bottomBusChroma =
                            pDecCont->StrmStorage.pPicBuf[picIndex].
                            data.busAddress + ((pDecCont->FrameDesc.frameWidth *
                                                pDecCont->FrameDesc.
                                                frameHeight) << 8);
                    }
                }
                else if(pDecCont->ppConfigQuery.deinterlace)
                {
                    Mpeg2DecSetupDeinterlace(pDecCont);
                }
            }
            /* handle I/P pictures where RunDecoderAsic was not invoced (error
             * in picture headers etc) */
            else if(!pDecCont->ppConfigQuery.pipelineAccepted ||
                    pDecCont->Hdrs.interlaced)
            {
                if(pDecCont->StrmStorage.outCount &&
                   pDecCont->StrmStorage.outBuf[0] ==
                   pDecCont->StrmStorage.outBuf[1])
                {
                    picIndex = pDecCont->StrmStorage.outBuf[0];
                    tmp = 1;

                    if(MPEG2DEC_IS_FIELD_OUTPUT)
                        Mpeg2DecPrepareFieldProcessing(pDecCont, BQUEUE_UNUSED);
                }
            }

            if(picIndex != MPEG2_BUFFER_UNDEFINED)
            {
                if(pPic[picIndex].sendToPp && pPic[picIndex].sendToPp < 4)
                {
                    MPEG2DEC_API_DEBUG(("NextPicture: send to pp %d\n",
                                        picIndex));

                    /* forward tiled mode */
                    pDecCont->ppControl.tiledInputMode = 
                        pPic[picIndex].tiledMode;
                    pDecCont->ppControl.progressiveSequence =
                        !pDecCont->Hdrs.interlaced;

                    /* Set up pp */
                    if(pDecCont->ppControl.picStruct ==
                       DECPP_PIC_BOT_FIELD_FRAME)
                    {
                        pDecCont->ppControl.inputBusLuma = 0;
                        pDecCont->ppControl.inputBusChroma = 0;

                        pDecCont->ppControl.bottomBusLuma =
                            pDecCont->StrmStorage.pPicBuf[picIndex].data.
                            busAddress + (pDecCont->FrameDesc.frameWidth << 4);

                        pDecCont->ppControl.bottomBusChroma =
                            pDecCont->ppControl.bottomBusLuma +
                            ((pDecCont->FrameDesc.frameWidth *
                              pDecCont->FrameDesc.frameHeight) << 8);

                        pDecCont->ppControl.inwidth =
                            pDecCont->ppControl.croppedW =
                            pDecCont->FrameDesc.frameWidth << 4;
                        pDecCont->ppControl.inheight =
                            (((pDecCont->FrameDesc.frameHeight +
                               1) & ~1) / 2) << 4;
                        pDecCont->ppControl.croppedH =
                            (pDecCont->FrameDesc.frameHeight << 4) / 2;
                    }
                    else
                    {
                        pDecCont->ppControl.inputBusLuma =
                            pDecCont->StrmStorage.pPicBuf[picIndex].data.
                            busAddress;
                        pDecCont->ppControl.inputBusChroma =
                            pDecCont->ppControl.inputBusLuma +
                            ((pDecCont->FrameDesc.frameWidth *
                              pDecCont->FrameDesc.frameHeight) << 8);
                        if(pDecCont->ppControl.picStruct ==
                           DECPP_PIC_TOP_FIELD_FRAME)
                        {
                            pDecCont->ppControl.bottomBusLuma = 0;
                            pDecCont->ppControl.bottomBusChroma = 0;
                            pDecCont->ppControl.inwidth =
                                pDecCont->ppControl.croppedW =
                                pDecCont->FrameDesc.frameWidth << 4;
                            pDecCont->ppControl.inheight =
                                (((pDecCont->FrameDesc.frameHeight +
                                   1) & ~1) / 2) << 4;
                            pDecCont->ppControl.croppedH =
                                (pDecCont->FrameDesc.frameHeight << 4) / 2;
                        }
                        else
                        {
                            pDecCont->ppControl.inwidth =
                                pDecCont->ppControl.croppedW =
                                pDecCont->FrameDesc.frameWidth << 4;
                            pDecCont->ppControl.inheight =
                                pDecCont->ppControl.croppedH =
                                pDecCont->FrameDesc.frameHeight << 4;
                            if(pDecCont->ppConfigQuery.deinterlace)
                            {
                                Mpeg2DecSetupDeinterlace(pDecCont);
                            }

                        }
                    }

                    pDecCont->ppControl.usePipeline = 0;
                    {
                        u32 value = GetDecRegister(pDecCont->mpeg2Regs,
                                                   HWIF_DEC_OUT_ENDIAN);

                        pDecCont->ppControl.littleEndian =
                            (value == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pDecCont->ppControl.wordSwap =
                        GetDecRegister(pDecCont->mpeg2Regs,
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

                    Mpeg2FillPicStruct(pPicture, pDecCont, picIndex);
                    returnValue = MPEG2DEC_PIC_RDY;
                    if(!parallelMode2Flag)
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

    Function name: Mpeg2FillPicStruct

    Functional description:
        Fill data to output pic description

    Input:
        pDecCont    Decoder container
        pPicture    Pointer to return value struct

    Return values:
        void

------------------------------------------------------------------------------*/
static void Mpeg2FillPicStruct(Mpeg2DecPicture * pPicture,
                               DecContainer * pDecCont, u32 picIndex)
{
    picture_t *pPic;

    pPicture->frameWidth = pDecCont->FrameDesc.frameWidth << 4;
    pPicture->frameHeight = pDecCont->FrameDesc.frameHeight << 4;
    pPicture->codedWidth = pDecCont->Hdrs.horizontalSize;
    pPicture->codedHeight = pDecCont->Hdrs.verticalSize;

    pPicture->interlaced = pDecCont->Hdrs.interlaced;

    pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;
    pPicture->pOutputPicture = (u8 *) pPic[picIndex].data.virtualAddress;
    pPicture->outputPictureBusAddress = pPic[picIndex].data.busAddress;
    pPicture->keyPicture = pPic[picIndex].picType;
    pPicture->picId = pPic[picIndex].picId;

    /* handle first field indication */
    if(pDecCont->Hdrs.interlaced)
    {
        if(pDecCont->Hdrs.fieldOutIndex)
            pDecCont->Hdrs.fieldOutIndex = 0;
        else
            pDecCont->Hdrs.fieldOutIndex = 1;
    }

    pPicture->firstField = pPic[picIndex].ff[pDecCont->Hdrs.fieldOutIndex];
    pPicture->repeatFirstField = pPic[picIndex].rff;
    pPicture->repeatFrameCount = pPic[picIndex].rfc;
    pPicture->numberOfErrMBs = pPic[picIndex].nbrErrMbs;
    pPicture->outputFormat = pPic[picIndex].tiledMode ?
        DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

    (void) G1DWLmemcpy(&pPicture->timeCode,
                     &pPic[picIndex].timeCode, sizeof(Mpeg2DecTime));
}

/*------------------------------------------------------------------------------

    Function name: Mpeg2SetRegs

    Functional description:
        Set registers

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static u32 Mpeg2SetRegs(DecContainer * pDecContainer, u32 strmBusAddress)
{
    u32 tmp = 0;
    u32 tmpFwd, tmpCurr;

#ifdef _DEC_PP_USAGE
    Mpeg2DecPpUsagePrint(pDecContainer, DECPP_UNSPECIFIED,
                         pDecContainer->StrmStorage.workOut, 1,
                         pDecContainer->StrmStorage.latestId);
#endif

    /*
    if(pDecContainer->Hdrs.interlaced)
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_OUT_TILED_E, 0);
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

    MPEG2DEC_API_DEBUG(("Decoding to index %d \n",
                        pDecContainer->StrmStorage.workOut));

    /* swReg3 */
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_PIC_INTERLACE_E,
                   pDecContainer->Hdrs.interlaced);

    if(pDecContainer->Hdrs.pictureStructure == FRAMEPICTURE)
    {
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_PIC_FIELDMODE_E, 0);
    }
    else
    {
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_PIC_FIELDMODE_E, 1);
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_PIC_TOPFIELD_E,
                       pDecContainer->Hdrs.pictureStructure == 1);
    }
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_PIC_MB_HEIGHT_P,
                   pDecContainer->FrameDesc.frameHeight);

    if(pDecContainer->Hdrs.pictureCodingType == BFRAME || pDecContainer->Hdrs.pictureCodingType == DFRAME)  /* ? */
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_PIC_B_E, 1);
    else
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_PIC_B_E, 0);

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_PIC_INTER_E,
                   pDecContainer->Hdrs.pictureCodingType == PFRAME ||
                   pDecContainer->Hdrs.pictureCodingType == BFRAME ? 1 : 0);

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_TOPFIELDFIRST_E, pDecContainer->Hdrs.topFieldFirst);  /* ? */

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FWD_INTERLACE_E, 0);  /* ??? */
    
    /* Never write out mvs, as SW doesn't allocate any memory for them */
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_WRITE_MVS_E, 0 );

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_ALT_SCAN_E,
                   pDecContainer->Hdrs.alternateScan);

    /* swReg5 */

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
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_RLC_VLC_BASE, tmp & ~0x7);

    /* amount of stream (as seen by the HW), obtained as amount of
     * stream given by the application subtracted by number of bytes
     * decoded by SW (if strmBusAddress is not 64-bit aligned -> adds
     * number of bytes from previous 64-bit aligned boundary) */
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_STREAM_LEN,
                   pDecContainer->StrmDesc.strmBuffSize -
                   ((tmp & ~0x7) - strmBusAddress));

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_STRM_START_BIT,
                   pDecContainer->StrmDesc.bitPosInWord + 8 * (tmp & 0x7));

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_CH_QP_OFFSET, 0); /* ? */

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_QSCALE_TYPE,
                   pDecContainer->Hdrs.quantType);

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_CON_MV_E, pDecContainer->Hdrs.concealmentMotionVectors);  /* ? */

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_INTRA_DC_PREC,
                   pDecContainer->Hdrs.intraDcPrecision);

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_INTRA_VLC_TAB,
                   pDecContainer->Hdrs.intraVlcFormat);

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FRAME_PRED_DCT,
                   pDecContainer->Hdrs.framePredFrameDct);

    /* swReg6 */

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_INIT_QP, 1);

    /* swReg12 */
    /*SetDecRegister(pDecContainer->mpeg2Regs, HWIF_RLC_VLC_BASE, 0);     */

    /* swReg13 */
    if(pDecContainer->Hdrs.pictureStructure == TOPFIELD ||
       pDecContainer->Hdrs.pictureStructure == FRAMEPICTURE)
    {
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_OUT_BASE,
                       pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                          StrmStorage.workOut].
                       data.busAddress);
    }
    else
    {
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_OUT_BASE,
                       (pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                           StrmStorage.workOut].
                        data.busAddress +
                        ((pDecContainer->FrameDesc.frameWidth << 4))));
    }

    if(pDecContainer->Hdrs.pictureStructure == FRAMEPICTURE)
    {
        if(pDecContainer->Hdrs.pictureCodingType == BFRAME) /* ? */
        {
            /* past anchor set to future anchor if past is invalid (second
             * picture in sequence is B) */
            tmpFwd =
                pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE ?
                pDecContainer->StrmStorage.work1 :
                pDecContainer->StrmStorage.work0;

            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER0_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER1_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER2_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER3_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);
        }
        else
        {
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER0_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER1_BASE,
                           pDecContainer->StrmStorage.
                           pPicBuf[pDecContainer->StrmStorage.work0].data.
                           busAddress);
        }
    }
    else
    {
        if(pDecContainer->Hdrs.pictureCodingType == BFRAME)
            /* past anchor set to future anchor if past is invalid */
            tmpFwd =
                pDecContainer->StrmStorage.work1 != INVALID_ANCHOR_PICTURE ?
                pDecContainer->StrmStorage.work1 :
                pDecContainer->StrmStorage.work0;
        else
            tmpFwd = pDecContainer->StrmStorage.work0;
        tmpCurr = pDecContainer->StrmStorage.workOut;
        /* past anchor not available -> use current (this results in using the
         * same top or bottom field as reference and output picture base, 
         * output is probably corrupted) */
        if(tmpFwd == INVALID_ANCHOR_PICTURE)
            tmpFwd = tmpCurr;

        if(pDecContainer->ApiStorage.firstField ||
           pDecContainer->Hdrs.pictureCodingType == BFRAME)
            /* 
             * if ((pDecContainer->Hdrs.pictureStructure == 1 &&
             * pDecContainer->Hdrs.topFieldFirst) ||
             * (pDecContainer->Hdrs.pictureStructure == 2 &&
             * !pDecContainer->Hdrs.topFieldFirst))
             */
        {
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER0_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER1_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
        }
        else if(pDecContainer->Hdrs.pictureStructure == 1)
        {
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER0_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER1_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpCurr].data.
                           busAddress);
        }
        else if(pDecContainer->Hdrs.pictureStructure == 2)
        {
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER0_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpCurr].data.
                           busAddress);
            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER1_BASE,
                           pDecContainer->StrmStorage.pPicBuf[tmpFwd].data.
                           busAddress);
        }
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER2_BASE,
                       pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                          StrmStorage.
                                                          work0].data.
                       busAddress);
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_REFER3_BASE,
                       pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                          StrmStorage.
                                                          work0].data.
                       busAddress);
    }

    /* swReg18 */
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_ALT_SCAN_FLAG_E,
                   pDecContainer->Hdrs.alternateScan);

    /* ? */
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FCODE_FWD_HOR,
                   pDecContainer->Hdrs.fCodeFwdHor);
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FCODE_FWD_VER,
                   pDecContainer->Hdrs.fCodeFwdVer);
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FCODE_BWD_HOR,
                   pDecContainer->Hdrs.fCodeBwdHor);
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FCODE_BWD_VER,
                   pDecContainer->Hdrs.fCodeBwdVer);

    if(!pDecContainer->Hdrs.mpeg2Stream && pDecContainer->Hdrs.fCode[0][0])
    {
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_MV_ACCURACY_FWD, 0);
    }
    else
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_MV_ACCURACY_FWD, 1);

    if(!pDecContainer->Hdrs.mpeg2Stream && pDecContainer->Hdrs.fCode[1][0])
    {
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_MV_ACCURACY_BWD, 0);
    }
    else
        SetDecRegister(pDecContainer->mpeg2Regs, HWIF_MV_ACCURACY_BWD, 1);

    /* swReg40 */
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_QTABLE_BASE,
                   pDecContainer->ApiStorage.pQTableBase.busAddress);

    /* swReg48 */
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_STARTMB_X, 0);
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_STARTMB_Y, 0);

    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_OUT_DIS, 0);
    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FILTERING_DIS, 1);

    /* Setup reference picture buffer */
    if(pDecContainer->refBufSupport)
    {
        RefbuSetup(&pDecContainer->refBufferCtrl, pDecContainer->mpeg2Regs,
                   (pDecContainer->Hdrs.pictureStructure == BOTTOMFIELD ||
                    pDecContainer->Hdrs.pictureStructure == TOPFIELD) ?
                   REFBU_FIELD : REFBU_FRAME,
                   pDecContainer->Hdrs.pictureCodingType == IFRAME,
                   pDecContainer->Hdrs.pictureCodingType == BFRAME, 0, 2,
                   0 );
    }

    if( pDecContainer->tiledModeSupport)
    {
        pDecContainer->tiledReferenceEnable = 
            DecSetupTiledReference( pDecContainer->mpeg2Regs, 
                pDecContainer->tiledModeSupport,
                !pDecContainer->Hdrs.progressiveSequence ||
                !pDecContainer->Hdrs.framePredFrameDct );
    }
    else
    {
        pDecContainer->tiledReferenceEnable = 0;
    }

    return HANTRO_OK;
}

/*------------------------------------------------------------------------------

    Function name: Mpeg2DecSetupDeinterlace

    Functional description:
        Setup PP interface for deinterlacing

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void Mpeg2DecSetupDeinterlace(DecContainer * pDecCont)
{
    pDecCont->ppControl.picStruct = DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
    pDecCont->ppControl.bottomBusLuma = pDecCont->ppControl.inputBusLuma +
        (pDecCont->FrameDesc.frameWidth << 4);
    pDecCont->ppControl.bottomBusChroma = pDecCont->ppControl.inputBusChroma +
        (pDecCont->FrameDesc.frameWidth << 4);
}

/*------------------------------------------------------------------------------

    Function name: Mpeg2DecPrepareFieldProcessing

    Functional description:
        Setup PP interface for deinterlacing

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void Mpeg2DecPrepareFieldProcessing(DecContainer * pDecCont,
    u32 picIndexOverride)
{

    u32 picIndex;

    pDecCont->ppControl.picStruct =
        pDecCont->Hdrs.topFieldFirst ?
        DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;

    if( picIndexOverride != BQUEUE_UNUSED )
    {
        picIndex = picIndexOverride;
    }
    else if( pDecCont->StrmStorage.parallelMode2 )
    {
        picIndex = pDecCont->StrmStorage.workOutPrev;
    }
    else if( pDecCont->StrmStorage.work0 >= 0 &&
        pDecCont->StrmStorage.work0 < pDecCont->StrmStorage.numBuffers )
    {
        picIndex = pDecCont->StrmStorage.work0;
    }
    else
    {
        ASSERT(0);
    }

    /* forward tiled mode */
    pDecCont->ppControl.tiledInputMode = pDecCont->tiledReferenceEnable;
    pDecCont->ppControl.progressiveSequence =
        !pDecCont->Hdrs.interlaced;
    pDecCont->ApiStorage.bufferForPp = 1+picIndex;

    if(pDecCont->Hdrs.topFieldFirst)
    {
        pDecCont->ppControl.inputBusLuma =
            pDecCont->StrmStorage.pPicBuf[picIndex].data.busAddress;
        pDecCont->ppControl.inputBusChroma =
            pDecCont->ppControl.inputBusLuma +
            ((pDecCont->FrameDesc.frameWidth *
              pDecCont->FrameDesc.frameHeight) << 8);
    }
    else
    {
        pDecCont->ppControl.bottomBusLuma =
            pDecCont->StrmStorage.pPicBuf[picIndex].data.busAddress +
            (pDecCont->FrameDesc.frameWidth << 4);

        pDecCont->ppControl.bottomBusChroma =
            pDecCont->ppControl.bottomBusLuma +
            ((pDecCont->FrameDesc.frameWidth *
              pDecCont->FrameDesc.frameHeight) << 8);
    }

    pDecCont->ppControl.inwidth =
        pDecCont->ppControl.croppedW = pDecCont->FrameDesc.frameWidth << 4;
    pDecCont->ppControl.inheight =
        (((pDecCont->FrameDesc.frameHeight + 1) & ~1) / 2) << 4;
    pDecCont->ppControl.croppedH = (pDecCont->FrameDesc.frameHeight << 4) / 2;

    MPEG2DEC_API_DEBUG(("FIELD: send %s\n",
                        pDecCont->ppControl.picStruct ==
                        DECPP_PIC_TOP_FIELD_FRAME ? "top" : "bottom"));
}

/*------------------------------------------------------------------------------

    Function name: Mp2CheckFormatSupport

    Functional description:
        Check if mpeg2 supported

    Input:
        container

    Return values:
        return zero for OK

------------------------------------------------------------------------------*/
u32 Mp2CheckFormatSupport(void)
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

    return (hwConfig.mpeg2Support == MPEG2_NOT_SUPPORTED);
}

/*------------------------------------------------------------------------------

    Function name: Mpeg2PpControl

    Functional description:
        set up and start pp

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/

static void Mpeg2PpControl(DecContainer * pDecContainer, u32 pipelineOff)
{
    u32 indexForPp = MPEG2_BUFFER_UNDEFINED;
    u32 nextBufferIndex;
    u32 prevBufferIndex;
    u32 previousB;

    DecPpInterface *pc = &pDecContainer->ppControl;
    DecHdrs *pHdrs = &pDecContainer->Hdrs;

    /* PP not connected or still running (not waited when first field of frame
     * finished */
    if (pc->ppStatus == DECPP_PIC_NOT_FINISHED)
    {
        return;
    }

    pDecContainer->ppConfigQuery.tiledMode =
        pDecContainer->tiledReferenceEnable;
    pDecContainer->PPConfigQuery(pDecContainer->ppInstance,
                                 &pDecContainer->ppConfigQuery);

    /* If we have once enabled parallel mode 2, keep it on */
    if(pDecContainer->StrmStorage.parallelMode2)
        pipelineOff = 1;
                                            
    Mpeg2PpMultiBufferSetup(pDecContainer, (pipelineOff ||
                                            !pDecContainer->ppConfigQuery.
                                            pipelineAccepted ));
    /* Check whether to enable parallel mode 2 */
    if( (!pDecContainer->ppConfigQuery.pipelineAccepted ||
         pDecContainer->Hdrs.interlaced) &&
        pc->multiBufStat != MULTIBUFFER_DISABLED &&
        !pDecContainer->Hdrs.lowDelay &&
        !pDecContainer->StrmStorage.parallelMode2 )
    {
        pDecContainer->StrmStorage.parallelMode2 = 1;
        pDecContainer->StrmStorage.pm2AllProcessedFlag = 0;
        pDecContainer->StrmStorage.pm2lockBuf    = pc->prevAnchorDisplayIndex;
        pDecContainer->StrmStorage.pm2StartPoint = 
            pDecContainer->FrameDesc.frameNumber;
    }
    else
    {
        /*pDecContainer->StrmStorage.parallelMode2 = 0;*/
    }

    pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.workOut].
        sendToPp = 1;

    /* Select new PP buffer index to use. If multibuffer is disabled, use
     * previous buffer, otherwise select new buffer from queue. */
    prevBufferIndex = pc->bufferIndex;
    if(pc->multiBufStat != MULTIBUFFER_DISABLED)
    {
        u32 buf0 = BQUEUE_UNUSED;
        /* In parallel mode 2, we must refrain from reusing future
         * anchor frame output buffer until it has been put out. */
        if(pDecContainer->StrmStorage.parallelMode2)
            buf0 = pDecContainer->StrmStorage.pm2lockBuf;
        nextBufferIndex = BqueueNext( &pDecContainer->StrmStorage.bqPp,
            buf0,
            BQUEUE_UNUSED,
            BQUEUE_UNUSED,
            pDecContainer->FrameDesc.picCodingType == BFRAME );
        pc->bufferIndex = nextBufferIndex;
    }
    else
    {
        nextBufferIndex = pc->bufferIndex = 0;
    }

    if(pDecContainer->StrmStorage.parallelMode2)
    {
        if(pDecContainer->StrmStorage.previousB)
        {
            pc->displayIndex = pc->bufferIndex;
        }
        else
        {
            pc->displayIndex = pDecContainer->StrmStorage.pm2lockBuf;
            /* Fix for case if stream has only one frame; then for NextPicture() 
             * we should have a proper buffer index instead of undefined. */
            if(pc->displayIndex == BQUEUE_UNUSED)
                pc->displayIndex = pc->bufferIndex;
        }
    }
    else if(pHdrs->lowDelay ||
       pDecContainer->FrameDesc.picCodingType == BFRAME)
    {
        pc->displayIndex = pc->bufferIndex;
    }
    else
    {
        pc->displayIndex = pc->prevAnchorDisplayIndex;
    }

    /* Connect PP output buffer to decoder output buffer */
    {
        u32 workBuffer;
        u32 luma = 0;
        u32 chroma = 0;

        if(pDecContainer->StrmStorage.parallelMode2)
            workBuffer = pDecContainer->StrmStorage.workOutPrev;
        else
            workBuffer = pDecContainer->StrmStorage.workOut;

        luma = pDecContainer->StrmStorage.
            pPicBuf[workBuffer].data.busAddress;
        chroma = luma + ((pDecContainer->FrameDesc.frameWidth *
                 pDecContainer->FrameDesc.frameHeight) << 8);

        pDecContainer->PPBufferData(pDecContainer->ppInstance, 
            pc->bufferIndex, luma, chroma);
    }

    if(pc->multiBufStat == MULTIBUFFER_FULLMODE)
    {
        MPEG2DEC_API_DEBUG(("Full pipeline# \n"));
        pc->usePipeline = pDecContainer->ppConfigQuery.pipelineAccepted;
        Mpeg2DecRunFullmode(pDecContainer);
        pDecContainer->StrmStorage.previousModeFull = 1;
    }
    else if(pDecContainer->StrmStorage.previousModeFull == 1)
    {
        if(pDecContainer->FrameDesc.picCodingType == BFRAME)
        {
            pDecContainer->StrmStorage.previousB = 1;
        }
        else
        {
            pDecContainer->StrmStorage.previousB = 0;
        }

        if(pDecContainer->FrameDesc.picCodingType == BFRAME)
        {
            MPEG2DEC_API_DEBUG(("PIPELINE OFF, DON*T SEND B TO PP\n"));
            indexForPp = MPEG2_BUFFER_UNDEFINED;
            pc->inputBusLuma = 0;
        }
        pc->ppStatus = DECPP_IDLE;

        pDecContainer->StrmStorage.pPicBuf[pDecContainer->StrmStorage.work0].sendToPp = 1;
        pDecContainer->StrmStorage.previousModeFull = 0;
    }
    else
    {
        if(!pDecContainer->StrmStorage.parallelMode2)
        {
            pc->bufferIndex = pc->displayIndex;
        }
        else
        {
            previousB = pDecContainer->StrmStorage.previousB;
        }
        if(pDecContainer->FrameDesc.picCodingType == BFRAME)
        {
            pDecContainer->StrmStorage.previousB = 1;
        }
        else
        {
            pDecContainer->StrmStorage.previousB = 0;
        }

        if((!pHdrs->lowDelay &&
            (pDecContainer->FrameDesc.picCodingType != BFRAME)) ||
           pHdrs->interlaced || pipelineOff)
        {
            pc->usePipeline = 0;
        }
        else
        {
            MPEG2DEC_API_DEBUG(("RUN PP  # \n"));
            MPEG2FLUSH;
            pc->usePipeline = pDecContainer->ppConfigQuery.pipelineAccepted;
        }

        if(!pc->usePipeline)
        {

            /* pipeline not accepted, don't run for first picture */
            if(pDecContainer->FrameDesc.frameNumber &&
               (pDecContainer->ApiStorage.bufferForPp == NO_BUFFER ||
                pDecContainer->StrmStorage.parallelMode2) &&
               (!pHdrs->interlaced ||
                pDecContainer->ApiStorage.firstField ||
                !pDecContainer->ppConfigQuery.deinterlace))
            {
                /* In parallel mode 2 we always run previous decoder output
                 * picture through PP */
                if( pDecContainer->StrmStorage.parallelMode2)
                {

                    /* forward tiled mode */
                    pDecContainer->ppControl.tiledInputMode = 
                        pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                           StrmStorage.workOutPrev].
                        tiledMode;
                    pDecContainer->ppControl.progressiveSequence =
                        !pDecContainer->Hdrs.interlaced;

                    pc->inputBusLuma =
                        pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                           StrmStorage.workOutPrev].
                        data.busAddress;

                    /* If we got an anchor frame, lock the PP output buffer */
                    if(!previousB)
                    {
                        pDecContainer->StrmStorage.pm2lockBuf = pc->bufferIndex;
                    }

                    pc->inputBusChroma =
                        pc->inputBusLuma +
                        ((pDecContainer->FrameDesc.frameWidth *
                          pDecContainer->FrameDesc.frameHeight) << 8);

                    pc->inwidth = pc->croppedW =
                        pDecContainer->FrameDesc.frameWidth << 4;
                    pc->inheight = pc->croppedH =
                        pDecContainer->FrameDesc.frameHeight << 4;
                    {
                        u32 value = GetDecRegister(pDecContainer->mpeg2Regs,
                                                   HWIF_DEC_OUT_ENDIAN);

                        pc->littleEndian =
                            (value == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pc->wordSwap =
                        GetDecRegister(pDecContainer->mpeg2Regs,
                                       HWIF_DEC_OUTSWAP32_E) ? 1 : 0;

                    MPEG2DEC_API_DEBUG(("sending NON B to pp\n"));
                    indexForPp = pDecContainer->StrmStorage.workOutPrev;

                    if(pDecContainer->ppConfigQuery.deinterlace)
                    {
                        Mpeg2DecSetupDeinterlace(pDecContainer);
                    }
                    /* if field output is used, send only a field to PP */
                    else if(pHdrs->interlaced)
                    {
                        Mpeg2DecPrepareFieldProcessing(pDecContainer, 
                            BQUEUE_UNUSED);
                    }
                    pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp = 2;
                }

                /*if:
                 * B pictures allowed and non B picture OR
                 * B pictures not allowed */
                else if((!pHdrs->lowDelay &&
                         pDecContainer->FrameDesc.picCodingType != BFRAME) ||
                         pHdrs->lowDelay)
                {
#ifdef _DEC_PP_USAGE
                    Mpeg2DecPpUsagePrint(pDecContainer, DECPP_PARALLEL,
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
                        u32 value = GetDecRegister(pDecContainer->mpeg2Regs,
                                                   HWIF_DEC_OUT_ENDIAN);

                        pc->littleEndian =
                            (value == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pc->wordSwap =
                        GetDecRegister(pDecContainer->mpeg2Regs,
                                       HWIF_DEC_OUTSWAP32_E) ? 1 : 0;

                    /* forward tiled mode */
                    pDecContainer->ppControl.tiledInputMode = 
                        pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                           StrmStorage.work0].
                        tiledMode;
                    pDecContainer->ppControl.progressiveSequence =
                        !pDecContainer->Hdrs.interlaced;


                    MPEG2DEC_API_DEBUG(("sending NON B to pp\n"));
                    indexForPp = pDecContainer->StrmStorage.work0;

                    if(pDecContainer->ppConfigQuery.deinterlace)
                    {
                        Mpeg2DecSetupDeinterlace(pDecContainer);
                    }
                    /* if field output is used, send only a field to PP */
                    else if(pHdrs->interlaced)
                    {
                        Mpeg2DecPrepareFieldProcessing(pDecContainer, 
                            BQUEUE_UNUSED);
                    }
                    pDecContainer->StrmStorage.pPicBuf[indexForPp].sendToPp = 2;
                }
                else
                {
                    MPEG2DEC_API_DEBUG(("PIPELINE OFF, DON*T SEND B TO PP\n"));
                    indexForPp = pDecContainer->StrmStorage.workOut;
                    indexForPp = MPEG2_BUFFER_UNDEFINED;
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
            Mpeg2DecPpUsagePrint(pDecContainer, DECPP_PIPELINED,
                                 pDecContainer->StrmStorage.workOut, 0,
                                 pDecContainer->StrmStorage.
                                 pPicBuf[pDecContainer->StrmStorage.
                                         workOut].picId);
#endif
            pc->inputBusLuma = pc->inputBusChroma = 0;
            indexForPp = pDecContainer->StrmStorage.workOut;

            /* forward tiled mode */
            pc->tiledInputMode = pDecContainer->tiledReferenceEnable;
            pc->progressiveSequence =
                !pDecContainer->Hdrs.interlaced;

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
            MPEG2DEC_API_DEBUG(("pprun, pipeline %s\n",
                                pc->usePipeline ? "on" : "off"));

            /* filter needs pipeline to work! */
            MPEG2DEC_API_DEBUG(("Filter %s# \n",
                                pDecContainer->ApiStorage.
                                disableFilter ? "off" : "on"));

            /* always disabled in MPEG-2 */
            pDecContainer->ApiStorage.disableFilter = 1;

            SetDecRegister(pDecContainer->mpeg2Regs, HWIF_FILTERING_DIS,
                           pDecContainer->ApiStorage.disableFilter);

            if(pc->usePipeline) /*CHECK !! */
            {
                if(pDecContainer->FrameDesc.picCodingType == BFRAME)
                {

                    SetDecRegister(pDecContainer->mpeg2Regs, HWIF_DEC_OUT_DIS,
                                   1);

                    /*frame or top or bottom */
                    ASSERT((pc->picStruct == DECPP_PIC_FRAME_OR_TOP_FIELD) ||
                           (pc->picStruct == DECPP_PIC_BOT_FIELD));
                }
            }

            /*ASSERT(indexForPp != pDecContainer->StrmStorage.workOut); */
            ASSERT(indexForPp != MPEG2_BUFFER_UNDEFINED);

            MPEG2DEC_API_DEBUG(("send %d %d %d %d, indexForPp %d\n",
                                pDecContainer->StrmStorage.pPicBuf[0].sendToPp,
                                pDecContainer->StrmStorage.pPicBuf[1].sendToPp,
                                pDecContainer->StrmStorage.pPicBuf[2].sendToPp,
                                pDecContainer->StrmStorage.pPicBuf[3].sendToPp,
                                indexForPp));

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

    if( pDecContainer->FrameDesc.picCodingType != BFRAME )
    {
        pc->prevAnchorDisplayIndex = nextBufferIndex;
    }

    if( pc->inputBusLuma == 0 &&
        pDecContainer->StrmStorage.parallelMode2 )
    {
        BqueueDiscard( &pDecContainer->StrmStorage.bqPp,
                       pc->bufferIndex );
    }

    /* Clear 2nd field indicator from structure if in parallel mode 2 and
     * outputting separate fields and output buffer not yet filled. This 
     * prevents NextPicture from going bonkers if stream ends before 
     * frame 2. */

    if(pDecContainer->FrameDesc.frameNumber -
            pDecContainer->StrmStorage.pm2StartPoint < 2 &&
       pDecContainer->StrmStorage.parallelMode2 &&
       pDecContainer->Hdrs.interlaced && 
            !pDecContainer->ppConfigQuery.deinterlace )
    {
        pDecContainer->ApiStorage.bufferForPp = NO_BUFFER;
    }

}

/*------------------------------------------------------------------------------

    Function name: Mpeg2PpMultiBufferInit

    Functional description:
        Modify state of pp output buffering.

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void Mpeg2PpMultiBufferInit(DecContainer * pDecCont)
{
    DecPpQuery *pq = &pDecCont->ppConfigQuery;
    DecPpInterface *pc = &pDecCont->ppControl;

    if(pq->multiBuffer)
    {
        if(!pq->pipelineAccepted || pDecCont->Hdrs.interlaced)
        {
            MPEG2DEC_API_DEBUG(("MULTIBUFFER_SEMIMODE\n"));
            pc->multiBufStat = MULTIBUFFER_SEMIMODE;
        }
        else
        {
            MPEG2DEC_API_DEBUG(("MULTIBUFFER_FULLMODE\n"));
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

    Function name: Mpeg2PpMultiBufferSetup

    Functional description:
        Modify state of pp output buffering.

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/
static void Mpeg2PpMultiBufferSetup(DecContainer * pDecCont, u32 pipelineOff)
{

    if(pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED)
    {
        MPEG2DEC_API_DEBUG(("MULTIBUFFER_DISABLED\n"));
        return;
    }

    if(pipelineOff && pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE)
    {
        MPEG2DEC_API_DEBUG(("MULTIBUFFER_SEMIMODE\n"));
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_SEMIMODE;
    }

    if(!pipelineOff &&
       !pDecCont->Hdrs.interlaced &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_SEMIMODE))
    {
        MPEG2DEC_API_DEBUG(("MULTIBUFFER_FULLMODE\n"));
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_FULLMODE;
    }

    if(pDecCont->ppControl.multiBufStat == MULTIBUFFER_UNINIT)
    {
        Mpeg2PpMultiBufferInit(pDecCont);
    }

}

/*------------------------------------------------------------------------------

    Function name: Mpeg2DecRunFullmode

    Functional description:

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void Mpeg2DecRunFullmode(DecContainer * pDecCont)
{
    u32 indexForPp = MPEG2_BUFFER_UNDEFINED;
    DecPpInterface *pc = &pDecCont->ppControl;
    DecHdrs *pHdrs = &pDecCont->Hdrs;

#ifdef _DEC_PP_USAGE
    Mpeg2DecPpUsagePrint(pDecCont, DECPP_PIPELINED,
                         pDecCont->StrmStorage.workOut, 0,
                         pDecCont->StrmStorage.pPicBuf[pDecCont->
                                                       StrmStorage.
                                                       workOut].picId);
#endif

    if(!pDecCont->StrmStorage.previousModeFull &&
       pDecCont->FrameDesc.frameNumber)
    {
        if(pDecCont->FrameDesc.picCodingType == BFRAME)
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work0].sendToPp = 0;
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work1].sendToPp = 0;
        }
        else
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work0].sendToPp = 0;
        }
    }

    /* forward tiled mode */
    pc->tiledInputMode = pDecCont->tiledReferenceEnable;
    pc->progressiveSequence =
        !pDecCont->Hdrs.interlaced;

    if(pDecCont->FrameDesc.picCodingType == BFRAME)
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

    pc->inwidth = pc->croppedW = pDecCont->FrameDesc.frameWidth << 4;
    pc->inheight = pc->croppedH = pDecCont->FrameDesc.frameHeight << 4;

    {
        if(pDecCont->FrameDesc.picCodingType == BFRAME)
            SetDecRegister(pDecCont->mpeg2Regs, HWIF_DEC_OUT_DIS, 1);

        /* always disabled in MPEG-2 */
        pDecCont->ApiStorage.disableFilter = 1;

        SetDecRegister(pDecCont->mpeg2Regs, HWIF_FILTERING_DIS,
                       pDecCont->ApiStorage.disableFilter);
    }

    ASSERT(indexForPp != MPEG2_BUFFER_UNDEFINED);

    MPEG2DEC_API_DEBUG(("send %d %d %d %d, indexForPp %d\n",
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

    Function name: Mpeg2CheckReleasePpAndHw

    Functional description:
        Function handles situations when PP was left running and decoder
        reserved after decoding of first field of a frame was finished but
        error occurred and second field of the picture in question is lost
        or corrupted

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
void Mpeg2CheckReleasePpAndHw(DecContainer *pDecCont)
{

    if(pDecCont->ppInstance != NULL &&
       pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED)
    {
        pDecCont->ppControl.ppStatus = DECPP_PIC_READY;
        pDecCont->PPEndCallback(pDecCont->ppInstance);
    }

    if (pDecCont->keepHwReserved)
    {
        (void) G1G1DWLReleaseHw(pDecCont->dwl);
        pDecCont->keepHwReserved = 0;
    }
}

/*------------------------------------------------------------------------------

    Function name: Mpeg2DecPeek

    Functional description:
        Retrieve last decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct

    Output:
        pPicture Decoder output picture.

    Return values:
        MPEG2DEC_OK         No picture available.
        MPEG2DEC_PIC_RDY    Picture ready.

------------------------------------------------------------------------------*/
Mpeg2DecRet Mpeg2DecPeek(Mpeg2DecInst decInst, Mpeg2DecPicture * pPicture)
{
    /* Variables */
    Mpeg2DecRet returnValue = MPEG2DEC_PIC_RDY;
    DecContainer *pDecCont;
    picture_t *pPic;
    u32 picIndex = MPEG2_BUFFER_UNDEFINED;
    u32 tmp = 0;
    u32 luma = 0;
    u32 chroma = 0;

    /* Code */
    MPEG2_API_TRC("\nMpeg2DecPeek#");

    /* Check that function input parameters are valid */
    if(pPicture == NULL)
    {
        MPEG2_API_TRC("Mpeg2DecPeek# ERROR: pPicture is NULL");
        return (MPEG2DEC_PARAM_ERROR);
    }

    pDecCont = (DecContainer *) decInst;

    /* Check if decoder is in an incorrect mode */
    if(decInst == NULL || pDecCont->ApiStorage.DecStat == UNINIT)
    {
        MPEG2_API_TRC("Mpeg2DecPeek# ERROR: Decoder not initialized");
        return (MPEG2DEC_NOT_INITIALIZED);
    }

    /* no output pictures available */
    if(!pDecCont->StrmStorage.outCount ||
        pDecCont->StrmStorage.newHeadersChangeResolution)
    {
        (void) G1DWLmemset(pPicture, 0, sizeof(Mpeg2DecPicture));
        pPicture->pOutputPicture = NULL;
        pPicture->interlaced = pDecCont->Hdrs.interlaced;
        /* print nothing to send out */
        returnValue = MPEG2DEC_OK;
    }
    else
    {
        /* output current (last decoded) picture */
        picIndex = pDecCont->StrmStorage.workOut;

        pPicture->frameWidth = pDecCont->FrameDesc.frameWidth << 4;
        pPicture->frameHeight = pDecCont->FrameDesc.frameHeight << 4;
        pPicture->codedWidth = pDecCont->Hdrs.horizontalSize;
        pPicture->codedHeight = pDecCont->Hdrs.verticalSize;

        pPicture->interlaced = pDecCont->Hdrs.interlaced;

        pPic = pDecCont->StrmStorage.pPicBuf + picIndex;
        pPicture->pOutputPicture = (u8 *) pPic->data.virtualAddress;
        pPicture->outputPictureBusAddress = pPic->data.busAddress;
        pPicture->keyPicture = pPic->picType;
        pPicture->picId = pPic->picId;

        pPicture->repeatFirstField = pPic->rff;
        pPicture->repeatFrameCount = pPic->rfc;
        pPicture->numberOfErrMBs = pPic->nbrErrMbs;

        (void) G1DWLmemcpy(&pPicture->timeCode,
                         &pPic->timeCode, sizeof(Mpeg2DecTime));

        /* frame output */
        pPicture->fieldPicture = 0;
        pPicture->topField = 0;
        pPicture->firstField = 0;
        pPicture->outputFormat = pPic->tiledMode ?
            DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;
    }

    return returnValue;
}

/*------------------------------------------------------------------------------

    Function name: Mpeg2DecSetLatency

    Functional description:
        set current video latencyMS to vdec driver for vpu dvfs

    Input:
        decInst     Reference to decoder instance.
        latencyMS   


    Return values:
        MPEG2DEC_OK;

------------------------------------------------------------------------------*/
Mpeg2DecRet Mpeg2DecSetLatency(Mpeg2DecInst decInst, int latencyMS)
{
    DecContainer *pDecCont = (DecContainer *) decInst;

    DWLSetLatency(pDecCont->dwl, latencyMS);

    return MPEG2DEC_OK;
}
