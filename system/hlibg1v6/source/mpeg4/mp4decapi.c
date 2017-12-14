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
--  Abstract : The API module C-functions of X170 MPEG4 Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4decapi.c,v $
--  $Date: 2011/02/04 12:40:26 $
--  $Revision: 1.392 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "mp4deccfg.h"
#include "mp4dechwd_container.h"
#include "mp4dechwd_strmdec.h"
#include "mp4dechwd_generic.h"
#include "mp4dechwd_utils.h"
#include "mp4decdrv.h"
#include "mp4decapi.h"
#include "mp4decapi_internal.h"
#include "dwl.h"
#include "mp4dechwd_headers.h"
#include "deccfg.h"
#include "decapicommon.h"
#include "refbuffer.h"
#include "workaround.h"
#include "bqueue.h"

#include "tiledref.h"

#include "mp4debug.h"
#ifdef MP4_ASIC_TRACE
#include "mpeg4asicdbgtrace.h"
#endif
#ifdef MP4DEC_TRACE
#define MP4_API_TRC(str)    MP4DecTrace((str))
#else
#define MP4_API_TRC(str)
#endif

#define X170_DEC_TIMEOUT        0xFFU
#define X170_DEC_SYSTEM_ERROR   0xFEU
#define X170_DEC_HW_RESERVED    0xFDU

#define BUFFER_UNDEFINED        16

#define MP4DEC_UPDATE_POUTPUT \
    pDecCont->MbSetDesc.outData.dataLeft = \
    DEC_STRM.pStrmBuffStart - pDecCont->MbSetDesc.outData.pStrmCurrPos; \
    (void) G1DWLmemcpy(pOutput, &pDecCont->MbSetDesc.outData, \
                             sizeof(MP4DecOutput))

#define NON_B_BUT_B_ALLOWED \
   !pDecContainer->Hdrs.lowDelay && pDecContainer->VopDesc.vopCodingType != BVOP
#define MP4_IS_FIELD_OUTPUT \
    pDecCont->Hdrs.interlaced && !pDecCont->ppConfigQuery.deinterlace
#define MP4_NON_PIPELINE_AND_B_PICTURE \
    ((!pDecCont->ppConfigQuery.pipelineAccepted || pDecCont->Hdrs.interlaced) \
    && pDecCont->VopDesc.vopCodingType == BVOP)
#define MP4_SET_BOT_ADDR(idx) pDecCont->ppControl.bottomBusLuma = \
         MP4DecResolveBus(pDecCont, idx) + \
         (pDecCont->VopDesc.vopWidth << 4); \
         pDecCont->ppControl.bottomBusChroma = \
         pDecCont->ppControl.bottomBusLuma + \
         ((pDecCont->VopDesc.vopWidth * \
         pDecCont->VopDesc.vopHeight) << 8)

#define MP4_SET_FIELD_DIMENSIONS \
        pDecCont->ppControl.inwidth = \
            pDecCont->ppControl.croppedW = \
            pDecCont->VopDesc.vopWidth << 4; \
        pDecCont->ppControl.inheight = \
            (((pDecCont->VopDesc.vopHeight + 1) & ~1) / 2) << 4; \
        pDecCont->ppControl.croppedH = \
            (pDecCont->VopDesc.vopHeight << 4) / 2




void MP4RefreshRegs(DecContainer * pDecCont);
void MP4FlushRegs(DecContainer * pDecCont);
static u32 HandleVlcModeError(DecContainer * pDecCont, u32 picNum);
static void HandleVopEnd(DecContainer * pDecCont);
static u32 RunDecoderAsic(DecContainer * pDecContainer, u32 strmBusAddress);
static void MP4FillPicStruct(MP4DecPicture * pPicture,
                             DecContainer * pDecCont, u32 picIndex);

static u32 MP4SetRegs(DecContainer * pDecContainer, u32 strmBusAddress);
static void MP4DecSetupDeinterlace(DecContainer * pDecCont);
static void MP4DecPrepareFieldProcessing(DecContainer * pDecCont, u32);
static void MP4DecParallelPP(DecContainer * pDecContainer, u32);
static void PPControl(DecContainer * pDecCont, u32 pipelineOff);
static void MP4SetIndexes(DecContainer * pDecCont);
static u32 MP4CheckFormatSupport(MP4DecStrmFmt strmFmt);
static u32 MP4DecFilterDisable(DecContainer * pDecCont);
static void MP4DecFieldAndValidBuffer(MP4DecPicture * pPicture,
                             DecContainer * pDecCont, u32* picIndex);
static void PPMultiBufferSetup(DecContainer * pDecCont, u32 pipelineOff);
static void MP4DecRunFullmode(DecContainer * pDecCont);

/*------------------------------------------------------------------------------
       Version Information - DO NOT CHANGE!
------------------------------------------------------------------------------*/

#define MP4DEC_MAJOR_VERSION 1
#define MP4DEC_MINOR_VERSION 2

#define MP4DEC_BUILD_MAJOR 1
#define MP4DEC_BUILD_MINOR 209
#define MP4DEC_SW_BUILD ((MP4DEC_BUILD_MAJOR * 1000) + MP4DEC_BUILD_MINOR)

/*------------------------------------------------------------------------------

    Function: MP4DecGetAPIVersion

        Functional description:
            Return version information of API

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

MP4DecApiVersion MP4DecGetAPIVersion()
{
    MP4DecApiVersion ver;

    ver.major = MP4DEC_MAJOR_VERSION;
    ver.minor = MP4DEC_MINOR_VERSION;

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: MP4DecGetBuild

        Functional description:
            Return build information of SW and HW

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

MP4DecBuild MP4DecGetBuild(void)
{
    MP4DecBuild ver;
    DWLHwConfig_t hwCfg;
    
    G1DWLmemset(&ver, 0, sizeof(ver));
    
    ver.swBuild = MP4DEC_SW_BUILD;
    ver.hwBuild = G1DWLReadAsicID();

    DWLReadAsicConfig(&hwCfg);

    SET_DEC_BUILD_SUPPORT(ver.hwConfig, hwCfg);
        
    MP4_API_TRC("MP4DecGetBuild# OK\n");

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: MP4DecDecode

        Functional description:
            Decode stream data. Calls StrmDec_Decode to do the actual decoding.

        Input:
            decInst     decoder instance
            pInput      pointer to input struct

        Outputs:
            pOutput     pointer to output struct

        Returns:
            MP4DEC_NOT_INITIALIZED      decoder instance not initialized yet
            MP4DEC_PARAM_ERROR          invalid parameters

            MP4DEC_STRM_PROCESSED       stream buffer decoded
            MP4DEC_HDRS_RDY             headers decoded
            MP4DEC_DP_HDRS_RDY          headers decoded, data partitioned stream
            MP4DEC_PIC_DECODED          decoding of a picture finished
            MP4DEC_STRM_ERROR               serious error in decoding, no
                                            valid parameter sets available
                                            to decode picture data
            MP4DEC_VOS_END              video Object Sequence end marker
                                                dedected
            MP4DEC_HW_BUS_ERROR    decoder HW detected a bus error
            MP4DEC_SYSTEM_ERROR    wait for hardware has failed
            MP4DEC_MEMFAIL         decoder failed to allocate memory
            MP4DEC_DWL_ERROR   System wrapper failed to initialize
            MP4DEC_HW_TIMEOUT  HW timeout
            MP4DEC_HW_RESERVED HW could not be reserved

------------------------------------------------------------------------------*/

MP4DecRet MP4DecDecode(MP4DecInst decInst,
                       const MP4DecInput * pInput, MP4DecOutput * pOutput)
{
#define API_STOR ((DecContainer *)decInst)->ApiStorage
#define DEC_STRM ((DecContainer *)decInst)->StrmDesc
#define DEC_VOPD ((DecContainer *)decInst)->VopDesc

    DecContainer *pDecCont;
    MP4DecRet internalRet;
    u32 i;
    u32 strmDecResult;
    u32 asicStatus;
    i32 ret = MP4DEC_OK;
    u32 errorConcealment = HANTRO_FALSE;

    MP4_API_TRC("MP4DecDecode#\n");

    if(pInput == NULL || pOutput == NULL || decInst == NULL)
    {
        MP4_API_TRC("MP44DecDecode# PARAM_ERROR\n");
        return MP4DEC_PARAM_ERROR;
    }

    pDecCont = ((DecContainer *) decInst);

    if(pDecCont->StrmStorage.unsupportedFeaturesPresent)
    {
        return (MP4DEC_FORMAT_NOT_SUPPORTED);
    }

    /*
     *  Check if decoder is in an incorrect mode
     */
    if(API_STOR.DecStat == UNINIT)
    {

        MP4_API_TRC("MP4DecDecode: NOT_INITIALIZED\n");
        return MP4DEC_NOT_INITIALIZED;
    }

    if(pInput->dataLen == 0 ||
       pInput->dataLen > DEC_X170_MAX_STREAM ||
       pInput->pStream == NULL || pInput->streamBusAddress == 0)
    {
        MP4_API_TRC("MP44DecDecode# PARAM_ERROR\n");
        return MP4DEC_PARAM_ERROR;
    }

    if(pDecCont->StrmStorage.numBuffers == 0 &&
       pDecCont->StrmStorage.customStrmHeaders )
    {
        internalRet = MP4AllocateBuffers( pDecCont );
        if(internalRet != MP4DEC_OK)
        {
            MP4_API_TRC("MP44DecDecode# MEMFAIL\n");
            return (internalRet);
        }
    }
    
    if(API_STOR.DecStat == HEADERSDECODED)
    {
        MP4FreeBuffers(pDecCont);

        internalRet = MP4AllocateBuffers(pDecCont);
        if(internalRet != MP4DEC_OK)
        {
            MP4_API_TRC("MP44DecDecode# MEMFAIL\n");
            return (internalRet);
        }

        /* Headers ready now, mems allocated, decoding can start */
        API_STOR.DecStat = STREAMDECODING;

    }

    if(pInput->enableDeblock)
    {
        API_STOR.disableFilter = 0;
    }
    else
    {
        API_STOR.disableFilter = 1;
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
        MP4DEC_API_DEBUG(("Start Decode\n"));
        /* run SW if HW is not in the middle of processing a picture (in VLC
         * mode, indicated as HW_VOP_STARTED decoder status) */
        if(API_STOR.DecStat != HW_VOP_STARTED)
        {
            strmDecResult = StrmDec_Decode(pDecCont);
            switch (strmDecResult)
            {
            case DEC_ERROR:
                /*
                 *          Nothing finished, decoder not ready, cannot continue decoding
                 *          before some headers received
                 */
                pDecCont->StrmStorage.strmDecReady = HANTRO_FALSE;
                pDecCont->StrmStorage.status = STATE_SYNC_LOST;
                API_STOR.DecStat = INITIALIZED;
                ret = MP4DEC_STRM_ERROR;
                break;

            case DEC_ERROR_BUF_NOT_EMPTY:
                /*
                 *          same as DEC_ERROR but stream buffer not empty
                 */
                MP4DEC_UPDATE_POUTPUT;
                ret = MP4DEC_STRM_ERROR;
                break;

            case DEC_END_OF_STREAM:
                /*
                 *          nothing finished, no data left in stream
                 */
                /* fallthrough */
            case DEC_RDY:
                /*
                 *          everything ok but no VOP finished
                 */
                ret = MP4DEC_STRM_PROCESSED;
                break;

            case DEC_VOP_RDY_BUF_NOT_EMPTY:
            case DEC_VOP_RDY:
                /*
                 * Vop finished and stream buffer could be empty or not. This
                 * is RLC mode processing.
                 */
                if(!DEC_VOPD.vopCoded && !pDecCont->packedMode)
                {
                    /* everything to not coded */
                    MP4NotCodedVop(pDecCont);
                }
#ifdef MP4_ASIC_TRACE
                {
                    u32 halves = 0;
                    u32 numAddr = 0;

                    WriteAsicCtrl(pDecCont);
                    WriteAsicRlc(pDecCont, &halves, &numAddr);
                    writePictureCtrl(pDecCont, &halves, &numAddr);

                }
#endif

                pDecCont->MbSetDesc.oddRlcVp = pDecCont->MbSetDesc.oddRlc = 0;
                pDecCont->StrmStorage.vpMbNumber = 0;
                pDecCont->StrmStorage.vpNumMbs = 0;
                pDecCont->StrmStorage.validVopHeader = HANTRO_FALSE;

                /* reset macro block error status */
                for(i = 0; i < pDecCont->VopDesc.totalMbInVop; i++)
                {
                    pDecCont->MBDesc[i].errorStatus = 0;
                }

                pDecCont->StrmStorage.workOutPrev = 
                    pDecCont->StrmStorage.workOut;
                pDecCont->StrmStorage.workOut = BqueueNext( 
                    &pDecCont->StrmStorage.bq,
                    pDecCont->StrmStorage.work0,
                    pDecCont->StrmStorage.work1,
                    BQUEUE_UNUSED,
                    0 );

                if (DEC_VOPD.vopCoded || !pDecCont->packedMode)
                {
                    asicStatus = RunDecoderAsic(pDecCont, 0);
                    pDecCont->VopDesc.vopNumber++;
                    pDecCont->VopDesc.vopNumberInSeq++;

                    MP4DecBufferPicture(pDecCont, pInput->picId,
                                        pDecCont->VopDesc.vopCodingType,
                                        pDecCont->StrmStorage.numErrMbs);

                    if(pDecCont->VopDesc.vopCodingType != BVOP)
                    {
                        if(pDecCont->Hdrs.lowDelay == 0)
                        {
                            pDecCont->StrmStorage.work1 =
                                pDecCont->StrmStorage.work0;
                        }
                        pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.workOut;
                    }

                    if(asicStatus == X170_DEC_TIMEOUT)
                    {
                        errorConcealment = HANTRO_TRUE;
                        ret = MP4DEC_HW_TIMEOUT;
                    }
                    else if(asicStatus == X170_DEC_SYSTEM_ERROR)
                    {
                        errorConcealment = HANTRO_TRUE;
                        ret = MP4DEC_SYSTEM_ERROR;
                    }
                    else if(asicStatus == X170_DEC_HW_RESERVED)
                    {
                        errorConcealment = HANTRO_TRUE;
                        ret = MP4DEC_HW_RESERVED;
                    }
                    else if(asicStatus & MP4_DEC_X170_IRQ_BUS_ERROR)
                    {
                        errorConcealment = HANTRO_TRUE;
                        ret = MP4DEC_HW_BUS_ERROR;
                    }
                    else if(asicStatus & MP4_DEC_X170_IRQ_TIMEOUT)
                    {
                        errorConcealment = HANTRO_TRUE;
                        ret = MP4DEC_HW_TIMEOUT;
                    }
                    else
                    {
                        ret = MP4DEC_PIC_DECODED;
                        if( pDecCont->VopDesc.vopCodingType == IVOP )
                            pDecCont->StrmStorage.pictureBroken = HANTRO_FALSE;
                    }

                    if(strmDecResult != DEC_OUT_OF_BUFFER &&
                       !pDecCont->Hdrs.dataPartitioned &&
                       pDecCont->StrmStorage.status == STATE_OK)
                    {
                        pDecCont->rlcMode = 0;
                    }

                }
                else /* packed mode N-VOP */
                {
                    ret = IS_END_OF_STREAM(pDecCont) ?
                        MP4DEC_STRM_PROCESSED : 0;
                }

                /* copy output parameters for this PIC (excluding stream pos) */
                pDecCont->MbSetDesc.outData.pStrmCurrPos =
                    pOutput->pStrmCurrPos;
                MP4DEC_UPDATE_POUTPUT;
                break;

            case DEC_VOP_SUPRISE_B:

                if(MP4DecBFrameSupport(pDecCont))
                {
                    internalRet = MP4DecAllocExtraBPic(pDecCont);
                    if(internalRet != MP4DEC_OK)
                    {
                        MP4_API_TRC
                            ("MP44DecDecode# MEMFAIL MP4DecAllocExtraBPic\n");
                        return (internalRet);
                    }
                    HandleVopEnd(pDecCont);

                    pDecCont->StrmStorage.validVopHeader = HANTRO_FALSE;

                    MP4DecBufferPicture(pDecCont, pInput->picId,
                                        pDecCont->VopDesc.vopCodingType, 0);

                    pDecCont->Hdrs.lowDelay = 0;
                    pDecCont->StrmStorage.work1 = pDecCont->StrmStorage.work0;
                    pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.workOut;
                    pDecCont->VopDesc.vopNumber++;
                    pDecCont->VopDesc.vopNumberInSeq++;

                    ret = MP4DEC_PIC_DECODED; 
                    API_STOR.DecStat = INITIALIZED;
                    MP4DEC_UPDATE_POUTPUT;
                }
                else
                {
                    /* B frames *not* supported */
                    errorConcealment = HANTRO_TRUE;
                    ret = HandleVlcModeError(pDecCont, pInput->picId);
                }
                break;

            case DEC_VOP_HDR_RDY:

                /* if type inter predicted and no reference -> error */
                if(( ( pDecCont->VopDesc.vopCodingType == PVOP ||
                       (pDecCont->VopDesc.vopCodingType == IVOP &&
                        !pDecCont->VopDesc.vopCoded) ) &&
                    pDecCont->StrmStorage.work0 == INVALID_ANCHOR_PICTURE) ||
                   (pDecCont->VopDesc.vopCodingType == BVOP &&
                    (pDecCont->StrmStorage.work0 == INVALID_ANCHOR_PICTURE ||
                     pDecCont->StrmStorage.skipB ||
                     pInput->skipNonReference )) ||
                   (pDecCont->VopDesc.vopCodingType == PVOP &&
                    pDecCont->StrmStorage.pictureBroken &&
                    pDecCont->StrmStorage.intraFreeze) )
                {
                    if(pDecCont->StrmStorage.skipB ||
                       pInput->skipNonReference)
                    {
                        MP4_API_TRC("MP4DecDecode# MP4DEC_NONREF_PIC_SKIPPED\n");
                    }
                    errorConcealment = HANTRO_TRUE;
                    ret = HandleVlcModeError(pDecCont, pInput->picId);
                }
                else
                {
                    API_STOR.DecStat = HW_VOP_STARTED;
                }
                break;

            case DEC_VOP_HDR_RDY_ERROR:
                errorConcealment = HANTRO_TRUE;
                ret = HandleVlcModeError(pDecCont, pInput->picId);
                /* copy output parameters for this PIC */
                MP4DEC_UPDATE_POUTPUT;
                break;

            case DEC_HDRS_RDY_BUF_NOT_EMPTY:
            case DEC_HDRS_RDY:

                internalRet = MP4DecCheckSupport(pDecCont);
                if(internalRet != MP4DEC_OK)
                {
                    pDecCont->StrmStorage.strmDecReady = HANTRO_FALSE;
                    pDecCont->StrmStorage.status = STATE_SYNC_LOST;
                    API_STOR.DecStat = INITIALIZED;
                    return internalRet;
                }
                /*
                 *          Either Vol header decoded or short video source format
                 *          determined. Stream buffer could be empty or not
                 */

                if(pDecCont->ApiStorage.firstHeaders)
                {
                    /*pDecCont->ApiStorage.firstHeaders = 0;*/

                    SetDecRegister(pDecCont->mp4Regs, HWIF_PIC_MB_WIDTH,
                                   pDecCont->VopDesc.vopWidth);
                    SetDecRegister(pDecCont->mp4Regs, HWIF_PIC_MB_HEIGHT_P,
                                   pDecCont->VopDesc.vopHeight);
                    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_MODE,
                                   pDecCont->StrmStorage.shortVideo +
                                   MP4_DEC_X170_MODE_MPEG4);

                    SetConformanceFlags( pDecCont );

                    if( pDecCont->StrmStorage.customOverfill)  
                    { 
                        SetDecRegister(pDecCont->mp4Regs, HWIF_MB_WIDTH_OFF,
                                            (pDecCont->StrmStorage.videoObjectLayerWidth & 0xF));
                        SetDecRegister(pDecCont->mp4Regs, HWIF_MB_HEIGHT_OFF,
                                            (pDecCont->StrmStorage.videoObjectLayerHeight & 0xF));
                    }

                    if( pDecCont->refBufSupport )
                    {
                        RefbuInit( &pDecCont->refBufferCtrl, MP4_DEC_X170_MODE_MPEG4, 
                                   pDecCont->VopDesc.vopWidth,
                                   pDecCont->VopDesc.vopHeight,
                                   pDecCont->refBufSupport );
                    }
                }

                API_STOR.DecStat = HEADERSDECODED;                
                pDecCont->VopDesc.vopNumberInSeq = 0;
                pDecCont->StrmStorage.workOutPrev = 
                    pDecCont->StrmStorage.workOut = 0;
                pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.work1 =
                    INVALID_ANCHOR_PICTURE;

                if(pDecCont->StrmStorage.shortVideo)
                    pDecCont->Hdrs.lowDelay = 1;

                if(!pDecCont->Hdrs.dataPartitioned)
                {
                    pDecCont->rlcMode = 0;
                    ret = MP4DEC_HDRS_RDY;
                    MP4DEC_UPDATE_POUTPUT;
                }
                else
                {
                    pDecCont->rlcMode = 1;
                    ret = MP4DEC_DP_HDRS_RDY;
                    MP4DEC_UPDATE_POUTPUT;
                }
                MP4DEC_API_DEBUG(("HDRS_RDY\n"));

                break;
            case DEC_VOS_END:
                /*
                 *          Vos end code encountered, stopping.
                 */
                ret = MP4DEC_VOS_END;
                break;

            default:
                pDecCont->StrmStorage.strmDecReady = HANTRO_FALSE;
                pDecCont->StrmStorage.status = STATE_SYNC_LOST;
                API_STOR.DecStat = INITIALIZED;
                MP4DEC_API_DEBUG(("entry:default\n"));
                ret = MP4DEC_STRM_ERROR;
                break;
            }
        }
        /* VLC mode */
        if(API_STOR.DecStat == HW_VOP_STARTED)
        {
            if(DEC_VOPD.vopCoded)
            {
                if(!pDecCont->asicRunning)
                {
                    MP4SetIndexes(pDecCont);
                    if( pDecCont->workarounds.mpeg.stuffing )
                    {
                        PrepareStuffingWorkaround( (u8*)MP4DecResolveVirtual
                            (pDecCont, pDecCont->StrmStorage.workOut),
                            pDecCont->VopDesc.vopWidth,
                            pDecCont->VopDesc.vopHeight );
                    }
                }

                asicStatus = RunDecoderAsic(pDecCont, pInput->streamBusAddress);

                /* Translate buffer empty IRQ to error in divx3 */
                if((asicStatus & MP4_DEC_X170_IRQ_BUFFER_EMPTY) &&
                    pDecCont->StrmStorage.customStrmHeaders )
                {
                    asicStatus = asicStatus & ~MP4_DEC_X170_IRQ_BUFFER_EMPTY;
                    asicStatus = asicStatus | MP4_DEC_X170_IRQ_STREAM_ERROR;

                    DWLDisableHW(pDecCont->dwl, 0x4, 0);

                    /* End PP co-operation */
                    if(pDecCont->ppControl.ppStatus == DECPP_RUNNING)
                    {
                        if(pDecCont->ppInstance != NULL)
                            pDecCont->PPEndCallback(pDecCont->ppInstance);
                        pDecCont->ppControl.ppStatus = DECPP_PIC_READY;
                    }

                    pDecCont->asicRunning = 0;

                    G1G1DWLReleaseHw(pDecCont->dwl);
                }

                if(asicStatus == X170_DEC_TIMEOUT)
                {
                    ret = MP4DEC_HW_TIMEOUT;
                }
                else if(asicStatus == X170_DEC_SYSTEM_ERROR)
                {
                    ret = MP4DEC_SYSTEM_ERROR;
                }
                else if(asicStatus == X170_DEC_HW_RESERVED)
                {
                    ret = MP4DEC_HW_RESERVED;
                }
                else if(asicStatus & MP4_DEC_X170_IRQ_BUS_ERROR)
                {
                    ret = MP4DEC_HW_BUS_ERROR;
                }
                else if(asicStatus & MP4_DEC_X170_IRQ_STREAM_ERROR ||
                        asicStatus & MP4_DEC_X170_IRQ_TIMEOUT)
                {
                    errorConcealment = HANTRO_TRUE;
                    /* picture freeze concealment disabled -> need to allocate
                     * memory for rlc mode buffers -> return MEMORY_REALLOCATION and
                     * let the application decide. Continue from start of the
                     * current stream buffer with SW */
                    if(asicStatus & MP4_DEC_X170_IRQ_STREAM_ERROR)
                    {
                        MP4DEC_API_DEBUG(("IRQ:STREAM ERROR IN HW\n"));

                        if( pDecCont->workarounds.mpeg.stuffing )
                        {
                            u8 *pRefPic = NULL;

                            if(pDecCont->VopDesc.vopNumberInSeq > 0)
                                pRefPic = (u8*)MP4DecResolveVirtual(pDecCont, pDecCont->StrmStorage.work0);

                            /* We process stuffing workaround. If everything is OK
                             * then mask interrupt as DEC_RDY and not STREAM_ERROR */
                            if(ProcessStuffingWorkaround( (u8*)MP4DecResolveVirtual
                               (pDecCont, pDecCont->StrmStorage.workOut),
                                pRefPic, pDecCont->VopDesc.vopWidth, 
                                pDecCont->VopDesc.vopHeight ) == HANTRO_TRUE)
                            {
                                asicStatus &= ~MP4_DEC_X170_IRQ_STREAM_ERROR;
                                asicStatus |= MP4_DEC_X170_IRQ_DEC_RDY;
                                errorConcealment = HANTRO_FALSE;
                            }
                        }
                    }
                    else
                    {
                        MP4DEC_API_DEBUG(("IRQ: HW TIMEOUT\n"));
                        if(pDecCont->use_mmu)
                            DWLLibReset(pDecCont->dwl);
                        return MP4DEC_HW_BUS_ERROR;
                    }
                    if(errorConcealment)
                    {
                        ret = HandleVlcModeError(pDecCont, pInput->picId);
                    }
                }
                else if(asicStatus & MP4_DEC_X170_IRQ_BUFFER_EMPTY)
                {                    
                    ret = MP4DEC_STRM_PROCESSED;

                }
                else if(asicStatus & MP4_DEC_X170_IRQ_DEC_RDY)
                {
                    /* Nothing here */
                }
                else
                    ASSERT(0);

                /* HW finished decoding a picture */
                if(asicStatus & MP4_DEC_X170_IRQ_DEC_RDY)
                {
                    pDecCont->VopDesc.vopNumber++;
                    pDecCont->VopDesc.vopNumberInSeq++;

                    HandleVopEnd(pDecCont);

                    pDecCont->StrmStorage.validVopHeader = HANTRO_FALSE;
                    if( pDecCont->StrmStorage.skipB )
                        pDecCont->StrmStorage.skipB--;

                    MP4DecBufferPicture(pDecCont, pInput->picId,
                                        pDecCont->VopDesc.vopCodingType, 0);

                    ret = MP4DEC_PIC_DECODED;
                    if( pDecCont->VopDesc.vopCodingType == IVOP )
                        pDecCont->StrmStorage.pictureBroken = HANTRO_FALSE;

                }

                if(ret != MP4DEC_STRM_PROCESSED)
                {
                    API_STOR.DecStat = STREAMDECODING;
                }

                if(ret == MP4DEC_PIC_RDY || ret == MP4DEC_STRM_PROCESSED)
                {
                    /* copy output parameters for this PIC (excluding stream pos) */
                    pDecCont->MbSetDesc.outData.pStrmCurrPos =
                        pOutput->pStrmCurrPos;
                    MP4DEC_UPDATE_POUTPUT;
                }

                if(pDecCont->VopDesc.vopCodingType != BVOP &&
                   ret != MP4DEC_STRM_PROCESSED &&
                   !(pDecCont->StrmStorage.sorensonSpark &&
                     pDecCont->StrmStorage.disposable))
                {
                    if(pDecCont->Hdrs.lowDelay == 0)
                    {
                        pDecCont->StrmStorage.work1 =
                            pDecCont->StrmStorage.work0;
                    }
                    pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.workOut;

                }
            }
            else if (!pDecCont->packedMode)   /* not-coded VOP */
            {
                MP4DEC_API_DEBUG(("\n\nNOT CODED VOP\n"));

                pDecCont->StrmStorage.validVopHeader = HANTRO_FALSE;
                /* stuffing not read for not coded VOPs -> skip */
                pDecCont->StrmDesc.pStrmCurrPos++;

                /* rotate picture indexes for current output */
                MP4SetIndexes(pDecCont);

                /* copy data pointers from prev output as not coded pic
                 * out */
                /*
                MP4DecChangeDataIndex(pDecCont,
                                      pDecCont->StrmStorage.workOut,
                                      pDecCont->StrmStorage.work0);
                                      */

                if(pDecCont->ppInstance != NULL)
                {
                    PPControl(pDecCont, 1);

                    /* End PP co-operation */
                    if(pDecCont->ppControl.ppStatus == DECPP_RUNNING)
                    {
                        MP4DEC_API_DEBUG(("Wait for PP\n"));
                        pDecCont->PPEndCallback(pDecCont->ppInstance);

                        pDecCont->ppControl.ppStatus = DECPP_PIC_READY;
                    }
                }

                HandleVopEnd(pDecCont);

                if( pDecCont->StrmStorage.skipB )
                    pDecCont->StrmStorage.skipB--;

                pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].
                    sendToPp = 1;
                MP4DecBufferPicture(pDecCont, pInput->picId,
                                    pDecCont->VopDesc.vopCodingType, 0);
                pDecCont->VopDesc.vopNumber++;
                pDecCont->VopDesc.vopNumberInSeq++;

                if(pDecCont->VopDesc.vopCodingType != BVOP)
                {
                    /*     MP4DecChangeDataIndex( pDecCont,
                     * pDecCont->StrmStorage.work0,
                     * pDecCont->StrmStorage.work1); */
                    if(pDecCont->Hdrs.lowDelay == 0)
                    {
                        pDecCont->StrmStorage.work1 =
                            pDecCont->StrmStorage.work0;
                    }
                    pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.workOut;

                }

                pDecCont->StrmStorage.previousNotCoded = 1;

                ret = MP4DEC_PIC_DECODED;

                API_STOR.DecStat = STREAMDECODING;

                /* copy output parameters for this PIC */
                MP4DEC_UPDATE_POUTPUT;

            }
            else
            {
                ret = IS_END_OF_STREAM(pDecCont) ? MP4DEC_STRM_PROCESSED : 0;
                API_STOR.DecStat = STREAMDECODING;
                /* TODO: what needs to be done to send previous ref to PP */
            }
        }

    }
    while(ret == 0);

    StrmDec_ProcessPacketFooter( pDecCont );

    if( errorConcealment && pDecCont->VopDesc.vopCodingType != BVOP )
    {
        pDecCont->StrmStorage.pictureBroken = HANTRO_TRUE;
    }

    pOutput->pStrmCurrPos = pDecCont->StrmDesc.pStrmCurrPos;
    pOutput->strmCurrBusAddress = pInput->streamBusAddress +
        (pDecCont->StrmDesc.pStrmCurrPos - pDecCont->StrmDesc.pStrmBuffStart);
    pOutput->dataLeft = pDecCont->StrmDesc.strmBuffSize -
        (pOutput->pStrmCurrPos - DEC_STRM.pStrmBuffStart);

    if(pDecCont->Hdrs.dataPartitioned)
        pDecCont->rlcMode = 1;

    MP4_API_TRC("MP4DecDecode: Exit\n");
    return ((MP4DecRet) ret);

#undef API_STOR
#undef DEC_STRM
#undef DEC_VOPD

}

/*------------------------------------------------------------------------------

    Function: MP4DecSetInfo()

        Functional description:
            Provide external information to decoder. Used for some custom
            streams which do not contain all necessary information in the
            elementary bitstream.

        Inputs:
            pDecInst        pointer to initialized instance 
            width           frame width in pixels
            height          frame height in pixels

        Outputs:

        Returns:
            MP4DEC_OK
                successfully updated info

------------------------------------------------------------------------------*/
MP4DecRet MP4DecSetInfo(MP4DecInst * pDecInst, const u32 width, 
                        const u32 height )
{

    DecContainer *pDecCont;

    MP4_API_TRC("MP4DecSetInfo#\n");

    if(pDecInst == NULL)
    {
        return MP4DEC_PARAM_ERROR;
    }

    pDecCont = ((DecContainer *) pDecInst);
    SetCustomInfo( pDecCont, width, height );

    MP4_API_TRC("MP4DecSetInfo: OK\n");

    return MP4DEC_OK;
}

/*------------------------------------------------------------------------------

    Function: MP4DecInit()

        Functional description:
            Initialize decoder software. Function reserves memory for the
            decoder instance.

        Inputs:
            strmFmt         specifies input stream format
                                (MPEG4, Sorenson Spark)
            useVideoFreezeConcealment
                            flag to enable error concealment method where
                            decoding starts at next intra picture after error
                            in bitstream.

        Outputs:
            pDecInst         pointer to initialized instance is stored here

        Returns:
            MP4DEC_OK
                successfully initialized the instance
            MP4DEC_PARAM_ERROR
                invalid parameters
            MP4DEC_MEMFAIL
                memory allocation failed
            MP4DEC_UNSUPPORTED_FORMAT
                hw doesn't support the initialized format
            MP4DEC_DWL_ERROR
                error initializing the system interface

------------------------------------------------------------------------------*/
MP4DecRet MP4DecInit(MP4DecInst * pDecInst, MP4DecStrmFmt strmFmt,
                     u32 useVideoFreezeConcealment,
                     u32 numFrameBuffers,
                     DecRefFrmFormat referenceFrameFormat, 
                     u32 mmuEnable)
{
    /*@null@ */ DecContainer *pDecCont;
    /*@null@ */ const void *dwl;
    u32 i;

    G1DWLInitParam_t dwlInit;
    DWLHwConfig_t config;

    MP4_API_TRC("MP4DecInit#\n");

    /* check that right shift on negative numbers is performed signed */
    /*lint -save -e* following check causes multiple lint messages */
#if (((-1) >> 1) != (-1))
#error Right bit-shifting (>>) does not preserve the sign
#endif
    /*lint -restore */

    if(pDecInst == NULL)
    {
        MP4_API_TRC("MPEG4DecInit# ERROR: pDecInst == NULL\n");
        return (MP4DEC_PARAM_ERROR);
    }

    *pDecInst = NULL; /* return NULL for any error */    
    
    if(MP4CheckFormatSupport(strmFmt))
    {
        MP4_API_TRC("MPEG4DecInit# ERROR: Format not supported!\n");
        return (MP4DEC_FORMAT_NOT_SUPPORTED);
    }

    dwlInit.clientType = DWL_CLIENT_TYPE_MPEG4_DEC;
    dwlInit.mmuEnable = mmuEnable;

    dwl = G1DWLInit(&dwlInit);

    if(dwl == NULL)
    {
        MP4_API_TRC("MPEG4DecInit# ERROR: DWL Init failed\n");
        return (MP4DEC_DWL_ERROR);
    }

    pDecCont = (DecContainer *) G1DWLmalloc(sizeof(DecContainer));

    if(pDecCont == NULL)
    {
        (void) G1DWLRelease(dwl);
         MP4_API_TRC("MPEG4DecInit# ERROR: Memory allocation failed\n");
        return (MP4DEC_MEMFAIL);
    }

    /* set everything initially zero */
    (void) G1DWLmemset(pDecCont, 0, sizeof(DecContainer));

    pDecCont->dwl = dwl;

    MP4API_InitDataStructures(pDecCont);

    pDecCont->ApiStorage.DecStat = INITIALIZED;

    pDecCont->StrmStorage.unsupportedFeaturesPresent = 0; /* will be se 
                                                           * later on */
    SetStrmFmt( pDecCont, strmFmt );

    pDecCont->StrmStorage.lastPacketByte = 0xFF;
    if( numFrameBuffers > 16 )
        numFrameBuffers = 16;
    pDecCont->StrmStorage.maxNumBuffers = numFrameBuffers;

    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_OUT_ENDIAN,
                   DEC_X170_OUTPUT_PICTURE_ENDIAN);
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_IN_ENDIAN,
                   DEC_X170_INPUT_DATA_ENDIAN);
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_STRENDIAN_E,
                   DEC_X170_INPUT_STREAM_ENDIAN);
    /*
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_OUT_TILED_E,
                   DEC_X170_OUTPUT_FORMAT);
                   */
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_MAX_BURST,
                   DEC_X170_BUS_BURST_LENGTH);
    if ((G1DWLReadAsicID() >> 16) == 0x8170U)
    {
        SetDecRegister(pDecCont->mp4Regs, HWIF_PRIORITY_MODE,
                       DEC_X170_ASIC_SERVICE_PRIORITY);
    }
    else
    {
        SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_SCMD_DIS,
            DEC_X170_SCMD_DISABLE);
    }
    DEC_SET_APF_THRESHOLD(pDecCont->mp4Regs);
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_LATENCY,
                   DEC_X170_LATENCY_COMPENSATION);
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_DATA_DISC_E,
                   DEC_X170_DATA_DISCARD_ENABLE);
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_OUTSWAP32_E,
                   DEC_X170_OUTPUT_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_INSWAP32_E,
                   DEC_X170_INPUT_DATA_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_STRSWAP32_E,
                   DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);

#if( DEC_X170_HW_TIMEOUT_INT_ENA != 0)
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_TIMEOUT_E, 1);
#else
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_TIMEOUT_E, 0);
#endif

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_CLK_GATE_E, 1);
#else
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_CLK_GATE_E, 0);
#endif

#if( DEC_X170_USING_IRQ  == 0)
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_IRQ_DIS, 1);
#else
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_IRQ_DIS, 0);
#endif

    /* Set prediction filter taps */
    SetDecRegister(pDecCont->mp4Regs, HWIF_PRED_BC_TAP_0_0, -1);
    SetDecRegister(pDecCont->mp4Regs, HWIF_PRED_BC_TAP_0_1,  3);
    SetDecRegister(pDecCont->mp4Regs, HWIF_PRED_BC_TAP_0_2, -6);
    SetDecRegister(pDecCont->mp4Regs, HWIF_PRED_BC_TAP_0_3, 20);

    /* set AXI RW IDs */
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_AXI_RD_ID, (DEC_X170_AXI_ID_R & 0xFFU));
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_AXI_WR_ID, (DEC_X170_AXI_ID_W & 0xFFU));

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
            return MP4DEC_FORMAT_NOT_SUPPORTED;
        }
        pDecCont->tiledModeSupport = config.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;
    pDecCont->StrmStorage.intraFreeze = useVideoFreezeConcealment;
    pDecCont->StrmStorage.pictureBroken = HANTRO_FALSE;

    InitWorkarounds(MP4_DEC_X170_MODE_MPEG4, &pDecCont->workarounds);

    /* return the newly created instance */
    *pDecInst = (DecContainer *) pDecCont;
        
    MP4_API_TRC("MP4DecInit: OK\n");

    return (MP4DEC_OK);
}

/*------------------------------------------------------------------------------

    Function: MP4DecGetInfo()

        Functional description:
            This function provides read access to decoder information. This
            function should not be called before MP4DecDecode function has
            indicated that headers are ready.

        Inputs:
            decInst     decoder instance

        Outputs:
            pDecInfo    pointer to info struct where data is written

        Returns:
            MP4DEC_OK            success
            MP4DEC_PARAM_ERROR   invalid parameters
            MP4DEC_HDRS_NOT_RDY  information not available yet

------------------------------------------------------------------------------*/
MP4DecRet MP4DecGetInfo(MP4DecInst decInst, MP4DecInfo * pDecInfo)
{

#define API_STOR ((DecContainer *)decInst)->ApiStorage
#define DEC_VOPD ((DecContainer *)decInst)->VopDesc
#define DEC_STRM ((DecContainer *)decInst)->StrmDesc
#define DEC_STST ((DecContainer *)decInst)->StrmStorage
#define DEC_HDRS ((DecContainer *)decInst)->Hdrs
#define DEC_REGS ((DecContainer *)decInst)->mp4Regs

    MP4_API_TRC("MP4DecGetInfo#\n");

    if(decInst == NULL || pDecInfo == NULL)
    {
        return MP4DEC_PARAM_ERROR;
    }

    pDecInfo->multiBuffPpSize = 2;

    if(API_STOR.DecStat == UNINIT || API_STOR.DecStat == INITIALIZED)
    {
        return MP4DEC_HDRS_NOT_RDY;
    }

    pDecInfo->frameWidth = DEC_VOPD.vopWidth << 4;
    pDecInfo->frameHeight = DEC_VOPD.vopHeight << 4;
    if(DEC_STST.shortVideo)
        pDecInfo->streamFormat = DEC_STST.mpeg4Video ? 1 : 2;
    else
        pDecInfo->streamFormat = 0;

    pDecInfo->profileAndLevelIndication = DEC_HDRS.profileAndLevelIndication;
    pDecInfo->videoRange = DEC_HDRS.videoRange;
    pDecInfo->videoFormat = DEC_HDRS.videoFormat;

    if(DEC_STST.shortVideo && !DEC_STST.sorensonSpark)
    {
        pDecInfo->codedWidth = pDecInfo->frameWidth;
        pDecInfo->codedHeight = pDecInfo->frameHeight;
    }
    else
    {
        pDecInfo->codedWidth = DEC_HDRS.videoObjectLayerWidth;
        pDecInfo->codedHeight = DEC_HDRS.videoObjectLayerHeight;
    }

    /* length of user data fields */
    pDecInfo->userDataVOSLen = DEC_STRM.userDataVOSLen;
    pDecInfo->userDataVISOLen = DEC_STRM.userDataVOLen;
    pDecInfo->userDataVOLLen = DEC_STRM.userDataVOLLen;
    pDecInfo->userDataGOVLen = DEC_STRM.userDataGOVLen;

    MP4DecPixelAspectRatio((DecContainer *) decInst, pDecInfo);

    pDecInfo->interlacedSequence = DEC_HDRS.interlaced;

    pDecInfo->multiBuffPpSize = 2; /*DEC_HDRS.interlaced ? 1 : 2;*/

    if(DEC_HDRS.interlaced)
    {
        pDecInfo->outputFormat = MP4DEC_SEMIPLANAR_YUV420;

    }
    else
    {

        pDecInfo->outputFormat =
            ((DecContainer *)decInst)->tiledReferenceEnable ? 
            MP4DEC_TILED_YUV420 : MP4DEC_SEMIPLANAR_YUV420;

    }

    MP4_API_TRC("MP4DecGetInfo: OK\n");
    return (MP4DEC_OK);

#undef API_STOR
#undef DEC_STRM
#undef DEC_VOPD
#undef DEC_STST
#undef DEC_HDRS
#undef DEC_REGS
}

/*------------------------------------------------------------------------------

    Function: MP4DecRelease()

        Functional description:
            Release the decoder instance.

        Inputs:
            decInst     Decoder instance

        Outputs:
            none

        Returns:
            none

------------------------------------------------------------------------------*/

void MP4DecRelease(MP4DecInst decInst)
{
#define API_STOR ((DecContainer *)decInst)->ApiStorage
    DecContainer *pDecCont = NULL;
    const void *dwl;
    u32 i;

    MP4DEC_DEBUG(("1\n"));
    MP4_API_TRC("MP4DecRelease#\n");
    if(decInst == NULL)
    {
        MP4_API_TRC("MP4DecRelease# ERROR: decInst == NULL\n");
        return;
    }

    pDecCont = ((DecContainer *) decInst);
    dwl = pDecCont->dwl;

    if (pDecCont->asicRunning)
        G1G1DWLReleaseHw(pDecCont->dwl);

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
        DWLFreeLinear(pDecCont->dwl, &pDecCont->StrmStorage.directMvs);

    if(pDecCont->StrmStorage.quantMatLinear.virtualAddress != NULL)
        DWLFreeLinear(pDecCont->dwl, &pDecCont->StrmStorage.quantMatLinear);
    for(i = 0; i < pDecCont->StrmStorage.numBuffers ; i++)
        if(pDecCont->StrmStorage.data[i].virtualAddress != NULL)
            DWLFreeRefFrm(pDecCont->dwl, &pDecCont->StrmStorage.data[i]);

    G1DWLfree(pDecCont);

    (void) G1DWLRelease(dwl);

    MP4_API_TRC("MP4DecRelease: OK\n");
#undef API_STOR
}

/*------------------------------------------------------------------------------

    Function: mpeg4RegisterPP()

        Functional description:
            Register the pp for mpeg-4 pipeline

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

i32 mpeg4RegisterPP(const void *decInst, const void *ppInst,
                    void (*PPRun) (const void *, DecPpInterface *),
                    void (*PPEndCallback) (const void *),
                    void (*PPConfigQuery) (const void *, DecPpQuery *),
                    void (*PPDisplayIndex)(const void *, u32),
		    void (*PPBufferData) (const void *, u32, u32, u32))
{
    DecContainer *pDecCont;

    pDecCont = (DecContainer *) decInst;

    if(decInst == NULL || pDecCont->ppInstance != NULL ||
       ppInst == NULL || PPRun == NULL || PPEndCallback == NULL ||
       PPConfigQuery == NULL || PPDisplayIndex == NULL ||
       PPBufferData == NULL)
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

    Function: mpeg4RegisterPP()

        Functional description:
            Unregister the pp from mpeg-4 pipeline

        Inputs:
            decInst     Decoder instance
            const void  *ppInst - post-processor instance

        Outputs:
            none

        Returns:
            i32 - return 0 for success or a negative error code

------------------------------------------------------------------------------*/

i32 mpeg4UnregisterPP(const void *decInst, const void *ppInst)
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

        Function name: MP4DecGetUserData()

        Purpose:    This function is used to get user data information.

        Input:      MP4DecInst        pDecInst   (decoder instance)
                    MP4DecUserConf     .userDataConfig (config structure ptr)

        Output:     MP4DEC_PARAM_ERROR  error in parameters
                    MP4DEC_OK           success

------------------------------------------------------------------------------*/

MP4DecRet MP4DecGetUserData(MP4DecInst decInst,
                            const MP4DecInput * pInput,
                            MP4DecUserConf * pUserDataConfig)
{
#define API_STOR ((DecContainer *)decInst)->ApiStorage
#define DEC_STRM ((DecContainer *)decInst)->StrmDesc
#define DEC_HDRS ((DecContainer *)decInst)->Hdrs
#define DEC_VOPD ((DecContainer *)decInst)->VopDesc

    DecContainer *pDecCont;
    u32 mode = 0;

    MP4_API_TRC("MP4DecGetUserData#\n");
    if((decInst == NULL) || (pUserDataConfig == NULL) || (pInput == NULL))
    {
        MP4DEC_API_DEBUG(("MP4DecGetUserData# ERROR: input pointer is NULL\n"));
        return (MP4DEC_PARAM_ERROR);
    }
    pDecCont = (DecContainer *) decInst;

    if((pInput->pStream == NULL) || (pInput->dataLen == 0))
    {
        MP4DEC_API_DEBUG(("MP4DecGetUserData# ERROR: stream pointer is NULL\n"));
        return (MP4DEC_PARAM_ERROR);
    }

    /* Assign pointers into structures */

    DEC_STRM.pStrmBuffStart = pInput->pStream;
    DEC_STRM.pStrmCurrPos = pInput->pStream;
    DEC_STRM.bitPosInWord = 0;
    DEC_STRM.strmBuffSize = pInput->dataLen;
    DEC_STRM.strmBuffReadBits = 0;

    switch (pUserDataConfig->userDataType)
    {
    case MP4DEC_USER_DATA_VOS:
        mode = SC_VOS_START;
        if(pUserDataConfig->pUserDataVOS)
        {
            DEC_STRM.pUserDataVOS = pUserDataConfig->pUserDataVOS;
        }
        else
        {
            MP4DEC_API_DEBUG(("MP4DecGetUserData# ERR:pUserDataVOS = NULL"));
            return (MP4DEC_PARAM_ERROR);
        }
        DEC_STRM.userDataVOSMaxLen = pUserDataConfig->userDataVOSMaxLen;
        break;
    case MP4DEC_USER_DATA_VISO:
        mode = SC_VISO_START;
        if(pUserDataConfig->pUserDataVISO)
        {
            DEC_STRM.pUserDataVO = pUserDataConfig->pUserDataVISO;
        }
        else
        {
            MP4DEC_API_DEBUG(("MP4DecGetUserData# ERR:pUserDataVISO = NULL"));
            return (MP4DEC_PARAM_ERROR);
        }
        DEC_STRM.userDataVOMaxLen = pUserDataConfig->userDataVISOMaxLen;
        break;
    case MP4DEC_USER_DATA_VOL:
        mode = SC_VOL_START;
        if(pUserDataConfig->pUserDataVOL)
        {
            DEC_STRM.pUserDataVOL = pUserDataConfig->pUserDataVOL;
        }
        else
        {
            MP4DEC_API_DEBUG(("MP4DecGetUserData# ERR:pUserDataVOL = NULL"));
            return (MP4DEC_PARAM_ERROR);
        }
        DEC_STRM.userDataVOLMaxLen = pUserDataConfig->userDataVOLMaxLen;
        break;
    case MP4DEC_USER_DATA_GOV:
        mode = SC_GVOP_START;
        if(pUserDataConfig->pUserDataGOV)
        {
            DEC_STRM.pUserDataGOV = pUserDataConfig->pUserDataGOV;
        }
        else
        {
            MP4DEC_API_DEBUG(("MP4DecGetUserData# ERR:pUserDataGOV = NULL"));
            return (MP4DEC_PARAM_ERROR);
        }
        DEC_STRM.userDataGOVMaxLen = pUserDataConfig->userDataGOVMaxLen;
        break;
    default:
        MP4DEC_API_DEBUG(("MP4DecGetUserData# ERR:incorrect user data type"));
        return (MP4DEC_PARAM_ERROR);
    }

    /* search VOS, VISO, VOL or GOV start code */
    while(!IS_END_OF_STREAM(pDecCont))
    {
        if(StrmDec_ShowBits(pDecCont, 32) == mode)
        {
            MP4DEC_DEBUG(("SEARCH MODE START CODE\n"));
            break;
        }
        (void) StrmDec_FlushBits(pDecCont, 8);
    }

    /* search user data start code */
    while(!IS_END_OF_STREAM(pDecCont))
    {
        if(StrmDec_ShowBits(pDecCont, 32) == SC_UD_START)
        {
            MP4DEC_DEBUG(("FOUND THE START CODE\n"));
            break;
        }
        (void) StrmDec_FlushBits(pDecCont, 8);
    }

    /* read and save user data */
    if(StrmDec_SaveUserData(pDecCont, mode) == HANTRO_NOK)
    {
        MP4DEC_API_DEBUG(("MP4DecGetUserData# ERR: reading user data failed"));
        return (MP4DEC_PARAM_ERROR);
    }

    /* restore StrmDesc structure */

    /* set zeros after reading user data field */
    switch (pUserDataConfig->userDataType)
    {
    case MP4DEC_USER_DATA_VOS:
        DEC_STRM.userDataVOSLen = 0;
        DEC_STRM.pUserDataVOS = 0;
        DEC_STRM.userDataVOSMaxLen = 0;
        break;
    case MP4DEC_USER_DATA_VISO:
        DEC_STRM.userDataVOLen = 0;
        DEC_STRM.pUserDataVO = 0;
        DEC_STRM.userDataVOMaxLen = 0;
        break;
    case MP4DEC_USER_DATA_VOL:
        DEC_STRM.userDataVOLLen = 0;
        DEC_STRM.pUserDataVOL = 0;
        DEC_STRM.userDataVOLMaxLen = 0;
        break;
    case MP4DEC_USER_DATA_GOV:
        DEC_STRM.userDataGOVLen = 0;
        DEC_STRM.pUserDataGOV = 0;
        DEC_STRM.userDataGOVMaxLen = 0;
        break;
    default:
        break;
    }
    MP4_API_TRC("MP4DecGetUserData# OK\n");
    return (MP4DEC_OK);

#undef API_STOR
#undef DEC_STRM
#undef DEC_HDRS
#undef DEC_VOPD

}

/*------------------------------------------------------------------------------
    Function name   : MP4RefreshRegs
    Description     : update shadow registers from real register
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void MP4RefreshRegs(DecContainer * pDecCont)
{
    i32 i;
    u32 *ppRegs = pDecCont->mp4Regs;

    for(i = 0; i < DEC_X170_REGISTERS; i++)
    {
        ppRegs[i] = G1DWLReadReg(pDecCont->dwl, 4 * i);
    }
}

/*------------------------------------------------------------------------------
    Function name   : MP4FlushRegs
    Description     : Flush shadow register to real register
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void MP4FlushRegs(DecContainer * pDecCont)
{
    i32 i;
    u32 *ppRegs = pDecCont->mp4Regs;

    for(i = 2; i < DEC_X170_REGISTERS; i++)
    {
        G1DWLWriteReg(pDecCont->dwl, 4 * i, ppRegs[i]);
        ppRegs[i] = 0;
    }
}

/*------------------------------------------------------------------------------
    Function name   : HandleVlcModeError
    Description     : error handling for VLC mode
    Return type     : u32
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
u32 HandleVlcModeError(DecContainer * pDecCont, u32 picNum)
{

    u32 ret = MP4DEC_STRM_PROCESSED, tmp;

    tmp = StrmDec_FindSync(pDecCont);
    /* error in first picture -> set reference to grey */
    if(!pDecCont->VopDesc.vopNumberInSeq)
    {
        (void)
            G1DWLmemset(MP4DecResolveVirtual
                      (pDecCont, pDecCont->StrmStorage.workOut), 128,
                      384 * pDecCont->VopDesc.totalMbInVop);

        if(pDecCont->ppInstance != NULL)
            pDecCont->StrmStorage.pPicBuf
                [pDecCont->StrmStorage.workOut].sendToPp = 0;

        pDecCont->StrmStorage.work0 = pDecCont->StrmStorage.workOut;
        pDecCont->StrmStorage.skipB = 2;
        /* no pictures finished -> return STRM_PROCESSED */
        if(tmp == END_OF_STREAM)
        {
            ret = MP4DEC_STRM_PROCESSED;
        }
        else
        {
            if(pDecCont->StrmDesc.strmBuffReadBits > 39)
            {
                pDecCont->StrmDesc.strmBuffReadBits -= 32;
                /* drop 3 lsbs (i.e. make read bits next lowest multiple of byte) */
                pDecCont->StrmDesc.strmBuffReadBits &= 0xFFFFFFF8;
                pDecCont->StrmDesc.bitPosInWord = 0;
                pDecCont->StrmDesc.pStrmCurrPos -= 4;
            }
            else
            {
                pDecCont->StrmDesc.strmBuffReadBits = 0;
                pDecCont->StrmDesc.bitPosInWord = 0;
                pDecCont->StrmDesc.pStrmCurrPos =
                    pDecCont->StrmDesc.pStrmBuffStart;
            }
            pDecCont->StrmStorage.status = STATE_OK;

            ret = MP4DEC_OK;
        }

    }
    else
    {
        if(pDecCont->VopDesc.vopCodingType != BVOP)
        {
            ret = MP4DEC_PIC_DECODED;

            if(pDecCont->ppInstance != NULL)
                pDecCont->StrmStorage.pPicBuf
                    [pDecCont->StrmStorage.workOut].sendToPp = 0;

            BqueueDiscard( &pDecCont->StrmStorage.bq, pDecCont->StrmStorage.workOut );
            pDecCont->StrmStorage.workOutPrev = pDecCont->StrmStorage.workOut;
            pDecCont->StrmStorage.workOut = pDecCont->StrmStorage.work0;

			
            /* start PP! */
            if (pDecCont->ppInstance != NULL &&
                !pDecCont->StrmStorage.parallelMode2 &&
                /* parallel mode and PP finished -> do not run again */
                (pDecCont->ppControl.usePipeline || 
                 pDecCont->ppControl.ppStatus != DECPP_PIC_READY))
            {
                PPControl(pDecCont, 1);

                /* End PP co-operation */
                if(pDecCont->ppControl.ppStatus == DECPP_RUNNING)
                {
                    MP4DEC_API_DEBUG(("Wait for PP\n"));
                    pDecCont->PPEndCallback(pDecCont->ppInstance);

                    pDecCont->ppControl.ppStatus = DECPP_PIC_READY;
                }
            }

            if(pDecCont->ppInstance != NULL &&
                !pDecCont->StrmStorage.parallelMode2)
                pDecCont->StrmStorage.pPicBuf
                    [pDecCont->StrmStorage.workOut].sendToPp = 1;

            pDecCont->StrmStorage.skipB = 2;
            MP4DecBufferPicture(pDecCont, picNum,
                                pDecCont->VopDesc.vopCodingType,
                                pDecCont->VopDesc.totalMbInVop);

            if( pDecCont->StrmStorage.work1 != INVALID_ANCHOR_PICTURE ) 
                pDecCont->StrmStorage.work1 = pDecCont->StrmStorage.work0;
        }
        else
        {
            if(pDecCont->StrmStorage.intraFreeze)
            {
                MP4DecBufferPicture(pDecCont, picNum,
                                    pDecCont->VopDesc.vopCodingType,
                                    pDecCont->VopDesc.totalMbInVop);

                ret = MP4DEC_PIC_DECODED;
            }
            else
            {
                ret = MP4DEC_NONREF_PIC_SKIPPED; 
            }

            /* prevBIdx is not valid if this is B skipped due to previous
             * errors */
            if (!pDecCont->StrmStorage.skipB)
                pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.prevBIdx].
                    sendToPp = 0;
        }

    }

    if(pDecCont->VopDesc.vopCodingType != BVOP)
    {
        pDecCont->VopDesc.vopNumber++;
        pDecCont->VopDesc.vopNumberInSeq++;
    }
    pDecCont->ApiStorage.DecStat = STREAMDECODING;
    pDecCont->StrmStorage.validVopHeader = HANTRO_FALSE;

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : HandleVopEnd
    Description     : VOP end special cases
    Return type     : void
    Argument        : DecContainer *pDecCont
------------------------------------------------------------------------------*/
void HandleVopEnd(DecContainer * pDecCont)
{

    u32 tmp;

    ProcessHwOutput( pDecCont );

    pDecCont->StrmDesc.strmBuffReadBits =
        8 * (pDecCont->StrmDesc.pStrmCurrPos -
             pDecCont->StrmDesc.pStrmBuffStart);
    pDecCont->StrmDesc.bitPosInWord = 0;

    /* If last MBs of BVOP were skipped using colocated MB status,
     * eat away possible resync markers. */
    if(pDecCont->VopDesc.vopCodingType == BVOP)
    {
        StrmDec_ProcessBvopExtraResync( pDecCont );
    }

    /* there might be extra stuffing byte if next start code is
     * video object sequence start or end code. If this is the
     * case the stuffing is normal mpeg4 stuffing. */
    tmp = StrmDec_ShowBitsAligned(pDecCont, 32, 1);
    if(((tmp == SC_VOS_START) || (tmp == SC_VOS_END) ||
        ((pDecCont->StrmDesc.pStrmCurrPos -
          pDecCont->StrmDesc.pStrmBuffStart) ==
         (pDecCont->StrmDesc.strmBuffSize - 1))) &&
       (pDecCont->StrmDesc.pStrmCurrPos[0] == 0x7F))
    {
        (void) StrmDec_FlushBits(pDecCont, 8);
    }

    /* handle extra zeros after VOP */
    if(!pDecCont->StrmStorage.shortVideo)
    {
        tmp = StrmDec_ShowBits(pDecCont, 32);
        if(!(tmp >> 8))
        {
            do
            {
                if(StrmDec_FlushBits(pDecCont, 8) == END_OF_STREAM)
                    break;
                tmp = StrmDec_ShowBits(pDecCont, 32);
            }
            while(!(tmp >> 8));
        }
    }
    else
    {
        tmp = StrmDec_ShowBits(pDecCont, 24);
        if(!tmp)
        {
            do
            {
                if(StrmDec_FlushBits(pDecCont, 8) == END_OF_STREAM)
                    break;
                tmp = StrmDec_ShowBits(pDecCont, 24);
            }
            while(!tmp);
        }
    }
}

/*------------------------------------------------------------------------------

         Function name: RunDecoderAsic
         Purpose:       Set Asic run lenght and run Asic
         Input:         DecContainer *pDecCont
         Output:        u32 asic status

------------------------------------------------------------------------------*/
u32 RunDecoderAsic(DecContainer * pDecContainer, u32 strmBusAddress)
{

    i32 ret;
    u32 tmp = 0;
    u32 asicStatus = 0;

    ASSERT(MP4DecResolveVirtual(pDecContainer,
                                pDecContainer->StrmStorage.workOut) != 0);
    ASSERT(pDecContainer->rlcMode || strmBusAddress != 0);
    pDecContainer->ppControl.inputBusLuma = 0;
    if(!pDecContainer->asicRunning)
    {

        tmp = MP4SetRegs(pDecContainer, strmBusAddress);
        if(tmp == HANTRO_NOK)
            return 0;

        (void) G1DWLReserveHw(pDecContainer->dwl);

        /* Start PP */
        if(pDecContainer->ppInstance != NULL)
        {
            PPControl(pDecContainer, 0);
        }
        else
        {
            SetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_OUT_DIS, 0);
            SetDecRegister(pDecContainer->mp4Regs, HWIF_FILTERING_DIS, 1);
        }

#ifdef MP4_ASIC_TRACE
        writePictureCtrlHex(pDecContainer, pDecContainer->rlcMode);
#endif

        pDecContainer->asicRunning = 1;

        G1DWLWriteReg(pDecContainer->dwl, 0x4, 0);

        MP4FlushRegs(pDecContainer);

        /* Enable HW */
        DWLEnableHW(pDecContainer->dwl, 4 * 1, 1);
    }
    else    /* in the middle of VOP, continue decoding */
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

        SetDecRegister(pDecContainer->mp4Regs, HWIF_RLC_VLC_BASE, tmp & ~0x7);
        /* amount of stream (as seen by the HW), obtained as amount of stream
         * given by the application subtracted by number of bytes decoded by
         * SW (if strmBusAddress is not 64-bit aligned -> adds number of bytes
         * from previous 64-bit aligned boundary) */
        SetDecRegister(pDecContainer->mp4Regs, HWIF_STREAM_LEN,
                       pDecContainer->StrmDesc.strmBuffSize -
                       ((tmp & ~0x7) - strmBusAddress));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_STRM_START_BIT,
                       pDecContainer->StrmDesc.bitPosInWord + 8 * (tmp & 0x7));

        G1DWLWriteReg(pDecContainer->dwl, 4 * 5, pDecContainer->mp4Regs[5]);
        G1DWLWriteReg(pDecContainer->dwl, 4 * 6, pDecContainer->mp4Regs[6]);
        G1DWLWriteReg(pDecContainer->dwl, 4 * 12, pDecContainer->mp4Regs[12]);

        DWLEnableHW(pDecContainer->dwl, 4 * 1, pDecContainer->mp4Regs[1]);
    }

    MP4DEC_API_DEBUG(("Wait for Decoder\n"));
    ret = G1DWLWaitHwReady(pDecContainer->dwl, (u32) DEC_X170_TIMEOUT_LENGTH);

    MP4RefreshRegs(pDecContainer);

    if(ret == DWL_HW_WAIT_OK)
    {
        asicStatus = GetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_IRQ_STAT);
    }
    else
    {
        /* reset HW */
        SetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_IRQ_STAT, 0);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_IRQ, 0);

        DWLDisableHW(pDecContainer->dwl, 4 * 1, 0);

        /* Wait for PP to end also */
        if(pDecContainer->ppInstance != NULL &&
           pDecContainer->ppControl.ppStatus == DECPP_RUNNING)
        {
            pDecContainer->ppControl.ppStatus = DECPP_PIC_READY;

            MP4DEC_API_DEBUG(("RunDecoderAsic: PP Wait for end\n"));

            pDecContainer->PPEndCallback(pDecContainer->ppInstance);

            MP4DEC_API_DEBUG(("RunDecoderAsic: PP Finished\n"));
        }

        pDecContainer->asicRunning = 0;

        G1G1DWLReleaseHw(pDecContainer->dwl);

        return (ret == DWL_HW_WAIT_ERROR) ?
            X170_DEC_SYSTEM_ERROR : X170_DEC_TIMEOUT;
    }

    if(!(asicStatus & MP4_DEC_X170_IRQ_BUFFER_EMPTY))
    {
        DWLDisableHW(pDecContainer->dwl, 0x4, 0);


        /* End PP co-operation */
        if(pDecContainer->ppControl.ppStatus == DECPP_RUNNING)
        {
            MP4DEC_API_DEBUG(("RunDecoderAsic: PP Wait for end\n"));
            if(pDecContainer->ppInstance != NULL)
                pDecContainer->PPEndCallback(pDecContainer->ppInstance);
            MP4DEC_API_DEBUG(("RunDecoderAsic: PP Finished\n"));

            pDecContainer->ppControl.ppStatus = DECPP_PIC_READY;
        }

        pDecContainer->asicRunning = 0;

        G1G1DWLReleaseHw(pDecContainer->dwl);
    }


    /* if in VLC mode and HW interrupt indicated either BUFFER_EMPTY or
     * DEC_RDY -> read stream end pointer and update StrmDesc structure */
    if((!pDecContainer->rlcMode ||
         pDecContainer->VopDesc.vopCodingType == BVOP) &&
        (asicStatus & (MP4_DEC_X170_IRQ_BUFFER_EMPTY|MP4_DEC_X170_IRQ_DEC_RDY)))
    {
        tmp = GetDecRegister(pDecContainer->mp4Regs, HWIF_RLC_VLC_BASE);
        /* update buffer size only for reasonable size of used data */
        if((tmp - strmBusAddress) <= pDecContainer->StrmDesc.strmBuffSize)
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

    SetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_IRQ_STAT, 0);

    if(pDecContainer->rlcMode)
    {
        /* Reset pointers of the SW/HW-shared buffers */
        pDecContainer->MbSetDesc.pRlcDataCurrAddr =
            pDecContainer->MbSetDesc.pRlcDataAddr;
        pDecContainer->MbSetDesc.pRlcDataVpAddr =
            pDecContainer->MbSetDesc.pRlcDataAddr;
        pDecContainer->MbSetDesc.oddRlcVp = pDecContainer->MbSetDesc.oddRlc;
    }

    if( pDecContainer->VopDesc.vopCodingType != BVOP &&
        pDecContainer->refBufSupport &&
        (asicStatus & MP4_DEC_X170_IRQ_DEC_RDY) &&
        pDecContainer->asicRunning == 0)
    {
        RefbuMvStatistics( &pDecContainer->refBufferCtrl, 
                            pDecContainer->mp4Regs,
                            pDecContainer->StrmStorage.directMvs.virtualAddress,
                            !pDecContainer->Hdrs.lowDelay,
                            pDecContainer->VopDesc.vopCodingType == IVOP );
    }

    return asicStatus;

}

/*------------------------------------------------------------------------------

    Function name: MP4DecNextPicture

    Functional description:
        Retrieve next decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct
        endOfStream Indicates whether end of stream has been reached

    Output:
        pPicture Decoder output picture.

    Return values:
        MP4DEC_OK                   No picture available.
        MP4DEC_PIC_RDY              Picture ready.
        MP4DEC_NOT_INITIALIZED      Decoder instance not initialized yet
------------------------------------------------------------------------------*/
MP4DecRet MP4DecNextPicture(MP4DecInst decInst, MP4DecPicture * pPicture,
                            u32 endOfStream)
{

/* Variables */

    MP4DecRet returnValue = MP4DEC_PIC_RDY;
    DecContainer *pDecCont;
    picture_t *pPic;
    u32 picIndex = BUFFER_UNDEFINED;
    u32 minCount;
    u32 luma = 0;
    u32 chroma = 0;
    u32 parallelMode2Flag = 0;

/* Code */

    MP4_API_TRC("MP4DecNextPicture#\n");

    /* Check that function input parameters are valid */
    if(pPicture == NULL)
    {
        MP4_API_TRC("MP4DecNextPicture# ERROR: pPicture is NULL\n");
        return (MP4DEC_PARAM_ERROR);
    }

    pDecCont = (DecContainer *) decInst;

    /* Check if decoder is in an incorrect mode */
    if(decInst == NULL || pDecCont->ApiStorage.DecStat == UNINIT)
    {
        MP4_API_TRC("MP4DecNextPicture# ERROR: Decoder not initialized\n");
        return (MP4DEC_NOT_INITIALIZED);
    }
    if(pDecCont->ApiStorage.DecStat == HEADERSDECODED)
        endOfStream = 1;
    minCount = 0;
    if(pDecCont->Hdrs.lowDelay == 0 && !endOfStream)
        minCount = 1;

    if(pDecCont->StrmStorage.parallelMode2 && !endOfStream)
        minCount = 2;

    if(pDecCont->ppInstance &&
     pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED)
    {
        pDecCont->ppConfigQuery.tiledMode =
            pDecCont->tiledReferenceEnable;
        pDecCont->PPConfigQuery(pDecCont->ppInstance,
                                 &pDecCont->ppConfigQuery);
				 
       if(pDecCont->ppConfigQuery.pipelineAccepted &&
    	   pDecCont->Hdrs.lowDelay && !MP4_IS_FIELD_OUTPUT &&
           !pDecCont->ppControl.usePipeline &&
           !pDecCont->StrmStorage.previousNotCoded)
       {
    	   pDecCont->StrmStorage.outCount = 1;
       }
    }

    /* Nothing to send out */
    if(pDecCont->StrmStorage.outCount <= minCount)
    {
        (void) G1DWLmemset(pPicture, 0, sizeof(MP4DecPicture));
        pPicture->pOutputPicture = NULL;
        returnValue = MP4DEC_OK;
    }
    else
    {
        picIndex = pDecCont->StrmStorage.outIndex;
        picIndex = pDecCont->StrmStorage.outBuf[picIndex];

        MP4FillPicStruct(pPicture, pDecCont, picIndex);

        /* Fill field related */
        if(MP4_IS_FIELD_OUTPUT)
        {
            pPicture->fieldPicture = 1;

            if(!pDecCont->ApiStorage.outputOtherField)
            {
                pPicture->topField = pDecCont->VopDesc.topFieldFirst ? 1 : 0;
                pDecCont->ApiStorage.outputOtherField = 1;
            }
            else
            {
                pPicture->topField = pDecCont->VopDesc.topFieldFirst ? 0 : 1;
                pDecCont->ApiStorage.outputOtherField = 0;
                pDecCont->StrmStorage.outCount--;
                pDecCont->StrmStorage.outIndex++;
                pDecCont->StrmStorage.outIndex &= 15;
            }
        }
        else
        {
            /* progressive or deinterlaced frame output */
            pPicture->topField = 0;
            pPicture->fieldPicture = 0;
            pDecCont->StrmStorage.outCount--;
            pDecCont->StrmStorage.outIndex++;
            pDecCont->StrmStorage.outIndex &= 15;
        }
    }

    if(pDecCont->ppInstance &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE) && 
       endOfStream && (returnValue == MP4DEC_PIC_RDY))
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
        pDecCont->PPDisplayIndex(pDecCont->ppInstance, pDecCont->ppControl.bufferIndex);
    }
    /* pp display process is separate of decoding process  */
    if(pDecCont->ppInstance && 
       (pDecCont->ppControl.multiBufStat != MULTIBUFFER_FULLMODE) &&
       (!pDecCont->StrmStorage.parallelMode2 || returnValue == MP4DEC_PIC_RDY))
    {
        /* In parallel mode 2, this is the last frame to display */
        if(pDecCont->StrmStorage.pm2AllProcessedFlag &&
            !(MP4_IS_FIELD_OUTPUT && 
                (pDecCont->ApiStorage.bufferForPp != NO_BUFFER)))
        {
            picIndex = pDecCont->StrmStorage.work0;

            pDecCont->ppControl.displayIndex = 
                pDecCont->ppControl.bufferIndex = 
                pDecCont->StrmStorage.pm2lockBuf;
            pDecCont->PPDisplayIndex(pDecCont->ppInstance,
                                     pDecCont->ppControl.displayIndex);
            if(MP4_IS_FIELD_OUTPUT)
            {
                pPicture->interlaced = 1;
                pPicture->fieldPicture = 1;
                pPicture->topField =
                    pDecCont->VopDesc.topFieldFirst ? 1 : 0;

                MP4DecPrepareFieldProcessing(pDecCont, picIndex);
            }

            MP4FillPicStruct(pPicture, pDecCont, picIndex);
            returnValue = MP4DEC_PIC_RDY;
            pDecCont->ppControl.ppStatus = DECPP_IDLE;
        }
        else if(pDecCont->ppControl.ppStatus == DECPP_PIC_READY &&
                (!(pDecCont->StrmStorage.parallelMode2 && endOfStream)))
        {
            if(MP4_IS_FIELD_OUTPUT)
            {
                pPicture->interlaced = 1;
                pPicture->fieldPicture = 1;
                pPicture->topField = pDecCont->VopDesc.topFieldFirst ? 1 : 0;
            }
            MP4FillPicStruct(pPicture, pDecCont, picIndex);
            returnValue = MP4DEC_PIC_RDY;
            pDecCont->ppControl.ppStatus = DECPP_IDLE;
        }
        else
        {
            pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;
            returnValue = MP4DEC_OK;

           if(((pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED &&
              !pDecCont->Hdrs.lowDelay) ) ||
              ((pDecCont->ppControl.multiBufStat == MULTIBUFFER_SEMIMODE) &&
                MP4_IS_FIELD_OUTPUT) ||
               (pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED &&
                MP4_IS_FIELD_OUTPUT ))
            {
                picIndex = BUFFER_UNDEFINED;
            }

            if(endOfStream)
            {
                picIndex = 0;
                while((picIndex < pDecCont->StrmStorage.numBuffers ) && !pPic[picIndex].sendToPp)
                    picIndex++;

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

                        luma = MP4DecResolveBus(pDecCont, pDecCont->StrmStorage.workOut);
                        chroma = luma + ((pDecCont->VopDesc.vopWidth *
                                 pDecCont->VopDesc.vopHeight) << 8);

                        pDecCont->PPBufferData(pDecCont->ppInstance, 
                            pDecCont->ppControl.bufferIndex, luma, chroma);
                    }

                    /* If previous picture was B, we send that to PP now */
                    if(pDecCont->StrmStorage.previousB)
                    {
                        picIndex = pDecCont->StrmStorage.prevBIdx;
                        pPic[picIndex].sendToPp = 1;
                        pDecCont->StrmStorage.previousB = 0;
                        parallelMode2Flag = 1;
                        pDecCont->ppControl.displayIndex = 
                            pDecCont->ppControl.bufferIndex;
                    }
                    /* ...otherwise we send the previous anchor. */
                    else
                    {
                        picIndex = pDecCont->StrmStorage.work0;
                        pPic[picIndex].sendToPp = 1;
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
                    if(MP4_IS_FIELD_OUTPUT)
                        MP4DecPrepareFieldProcessing(pDecCont, picIndex);
                }
                else if(MP4_IS_FIELD_OUTPUT)
                {
                    if(MP4_IS_FIELD_OUTPUT &&
                        (pDecCont->ApiStorage.bufferForPp != NO_BUFFER))
                    {
                        MP4DecFieldAndValidBuffer(pPicture,
                             pDecCont, &picIndex);

                        /* Restore correct buffer index */
                        if(pDecCont->StrmStorage.parallelMode2 &&
                           pDecCont->ppControl.bufferIndex != pDecCont->ppControl.displayIndex)
                        {
                            picture_t *pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;

                            pDecCont->ppControl.bufferIndex = 
                                pDecCont->ppControl.displayIndex;
                            picIndex = pDecCont->StrmStorage.work1;

                            pPicture->interlaced = 1;
                            pPicture->fieldPicture = 1;
                            pDecCont->ppControl.picStruct =
                                pDecCont->VopDesc.
                                topFieldFirst ? DECPP_PIC_BOT_FIELD_FRAME :
                                DECPP_PIC_TOP_FIELD_FRAME;
                            pPicture->topField = pDecCont->VopDesc.topFieldFirst ? 0 : 1;

                            pDecCont->ApiStorage.bufferForPp = NO_BUFFER;
                            pPic[picIndex].sendToPp = 1;
                        }

                    }
                    else
                    {
                        if(picIndex < pDecCont->StrmStorage.numBuffers)
                        {
                            pPicture->interlaced = 1;
                            pPicture->fieldPicture = 1;
                            /* if field output, other field must be processed also */
                            if(!pDecCont->StrmStorage.parallelMode2)
                            {
                                if(picIndex >= 0 &&
                                   picIndex < pDecCont->StrmStorage.numBuffers )
                                    pDecCont->ApiStorage.bufferForPp = picIndex+1;
                                else
                                    ASSERT(0);
                            }

                            MP4DEC_API_DEBUG(("first field of last frame, send %d still to pp", pDecCont->ApiStorage.bufferForPp));
                            /* set field processing */
                            pDecCont->ppControl.picStruct =
                                pDecCont->VopDesc.topFieldFirst ?
                                DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;
                            /* first field */

                            if(pDecCont->ppControl.picStruct ==
                               DECPP_PIC_BOT_FIELD_FRAME)
                            {
                                pDecCont->ppControl.bottomBusLuma =
                                    MP4DecResolveBus(pDecCont, picIndex) +
                                    (pDecCont->VopDesc.vopWidth << 4);

                                pDecCont->ppControl.bottomBusChroma =
                                    pDecCont->ppControl.bottomBusLuma +
                                    ((pDecCont->VopDesc.vopWidth *
                                      pDecCont->VopDesc.vopHeight) << 8);
                            }
                        }
                    }
                }
                else if(pDecCont->ppConfigQuery.deinterlace)
                {
                    MP4DecSetupDeinterlace(pDecCont);
                }
            }
            else if(MP4_NON_PIPELINE_AND_B_PICTURE &&
                    !pDecCont->StrmStorage.parallelMode2)
            {
                picIndex = pDecCont->StrmStorage.prevBIdx;   /* send index 2 (B Picture output) to PP) */
                pDecCont->VopDesc.vopCodingType = IVOP;

                /* Field decoding for first field of a B picture */
                if(MP4_IS_FIELD_OUTPUT)
                {
                    if(!pDecCont->StrmStorage.parallelMode2)
                    {
                        if(picIndex >= 0 &&
                           picIndex < pDecCont->StrmStorage.numBuffers )
                            pDecCont->ApiStorage.bufferForPp = picIndex+1;
                        else
                            ASSERT(0);
                    }
                    pPicture->interlaced = 1;
                    pPicture->fieldPicture = 1;
                    MP4DEC_API_DEBUG((" Processing first field in NextPicture %d\n", picIndex));
                    pPicture->topField =
                        pDecCont->VopDesc.topFieldFirst ? 1 : 0;
                    pDecCont->ppControl.picStruct =
                        pDecCont->VopDesc.topFieldFirst ?
                        DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;

                    /* first field */
                    if(pDecCont->ppControl.picStruct ==
                       DECPP_PIC_BOT_FIELD_FRAME)
                    {
                        MP4_SET_BOT_ADDR(picIndex);
                    }
                }
            }
            else if(MP4_IS_FIELD_OUTPUT &&
                    (pDecCont->ApiStorage.bufferForPp != NO_BUFFER))
            {
                MP4DecFieldAndValidBuffer(pPicture, pDecCont, &picIndex);

                /* Restore correct buffer index */
                if(pDecCont->StrmStorage.parallelMode2 &&
                   pDecCont->ppControl.bufferIndex != pDecCont->ppControl.displayIndex)
                {
                    picture_t *pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;

                    pDecCont->ppControl.bufferIndex = 
                        pDecCont->ppControl.displayIndex;
                    picIndex = pDecCont->StrmStorage.work1;

                    pPicture->interlaced = 1;
                    pPicture->fieldPicture = 1;
                    pDecCont->ppControl.picStruct =
                        pDecCont->VopDesc.
                        topFieldFirst ? DECPP_PIC_BOT_FIELD_FRAME :
                        DECPP_PIC_TOP_FIELD_FRAME;
                    pPicture->topField = pDecCont->VopDesc.topFieldFirst ? 0 : 1;

                    pDecCont->ApiStorage.bufferForPp = NO_BUFFER;
                    pPic[picIndex].sendToPp = 1;
                }
            }

            if(picIndex != BUFFER_UNDEFINED)
            {
                if(pPic[picIndex].sendToPp)
                {
                    MP4DEC_API_DEBUG(("NextPicture: send to pp %d\n",
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

                        MP4_SET_BOT_ADDR(picIndex);

                        MP4_SET_FIELD_DIMENSIONS;
                    }
                    else
                    {
                        pDecCont->ppControl.inputBusLuma =
                            MP4DecResolveBus(pDecCont, picIndex);
                        pDecCont->ppControl.inputBusChroma =
                            pDecCont->ppControl.inputBusLuma +
                            ((pDecCont->VopDesc.vopWidth *
                              pDecCont->VopDesc.vopHeight) << 8);
                        if(pDecCont->ppControl.picStruct ==
                           DECPP_PIC_TOP_FIELD_FRAME)
                        {
                            pDecCont->ppControl.bottomBusLuma = 0;
                            pDecCont->ppControl.bottomBusChroma = 0;

                            MP4_SET_FIELD_DIMENSIONS;
                        }
                        else
                        {
                            pDecCont->ppControl.inwidth =
                                pDecCont->ppControl.croppedW =
                                pDecCont->VopDesc.vopWidth << 4;
                            pDecCont->ppControl.inheight =
                                pDecCont->ppControl.croppedH =
                                pDecCont->VopDesc.vopHeight << 4;
                            if(pDecCont->ppConfigQuery.deinterlace)
                            {
                                MP4DecSetupDeinterlace(pDecCont);
                            }

                        }
                    }

                    pDecCont->ppControl.usePipeline = 0;
                    {
                        u32 tmp =
                            GetDecRegister(pDecCont->mp4Regs,
                                           HWIF_DEC_OUT_ENDIAN);
                        pDecCont->ppControl.littleEndian =
                            (tmp == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pDecCont->ppControl.wordSwap =
                        GetDecRegister(pDecCont->mp4Regs,
                                       HWIF_DEC_OUTSWAP32_E) ? 1 : 0;

                    /* Run pp */
                    SetDecRegister(pDecCont->mp4Regs, HWIF_FILTERING_DIS, 1);
                    pDecCont->PPRun(pDecCont->ppInstance, &pDecCont->ppControl);
                    pDecCont->ppControl.ppStatus = DECPP_RUNNING;
                    ASSERT(pDecCont->StrmStorage.pPicBuf[picIndex].sendToPp ==
                           1);
                    pDecCont->StrmStorage.pPicBuf[picIndex].sendToPp = 0;
                    /* Wait for result */
                    pDecCont->PPEndCallback(pDecCont->ppInstance);

                    MP4FillPicStruct(pPicture, pDecCont, picIndex);
                    returnValue = MP4DEC_PIC_RDY;
                    if(!parallelMode2Flag)
                        pDecCont->ppControl.ppStatus = DECPP_IDLE;
                    pDecCont->ppControl.picStruct =
                        DECPP_PIC_FRAME_OR_TOP_FIELD;
                }
            }
        }
    }

    if(returnValue == MP4DEC_PIC_RDY)
        MP4_API_TRC("MP4DecNextPicture# MP4DEC_PIC_RDY\n");
    else 
        MP4_API_TRC("MP4DecNextPicture# MP4DEC_OK\n");

    return returnValue;

}

/*------------------------------------------------------------------------------

    Function name: MP4FillPicStruct

    Functional description:
        Fill data to output pic description

    Input:
        pDecCont    Decoder container
        pPicture    Pointer to return value struct

    Return values:
        void

------------------------------------------------------------------------------*/
static void MP4FillPicStruct(MP4DecPicture * pPicture,
                             DecContainer * pDecCont, u32 picIndex)
{
    picture_t *pPic;

    pPicture->frameWidth = pDecCont->VopDesc.vopWidth << 4;
    pPicture->frameHeight = pDecCont->VopDesc.vopHeight << 4;
    pPicture->interlaced = pDecCont->Hdrs.interlaced;

    if(pDecCont->StrmStorage.shortVideo && !pDecCont->StrmStorage.sorensonSpark)
    {
        pPicture->codedWidth = pPicture->frameWidth;
        pPicture->codedHeight = pPicture->frameHeight;
    }
    else
    {
        pPicture->codedWidth = pDecCont->Hdrs.videoObjectLayerWidth;
        pPicture->codedHeight = pDecCont->Hdrs.videoObjectLayerHeight;
    }

    pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;
    pPicture->pOutputPicture = (u8 *) MP4DecResolveVirtual(pDecCont, picIndex);

    pPicture->outputPictureBusAddress = MP4DecResolveBus(pDecCont, picIndex);
    pPicture->keyPicture = pPic[picIndex].picType == IVOP;
    pPicture->picId = pPic[picIndex].picId;
    pPicture->nbrOfErrMBs = pPic[picIndex].nbrErrMbs;
    pPicture->outputFormat = pPic[picIndex].tiledMode ?
        DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

    (void) G1DWLmemcpy(&pPicture->timeCode,
                     &pPic[picIndex].timeCode, sizeof(MP4DecTime));

}

/*------------------------------------------------------------------------------

    Function name: MP4SetRegs

    Functional description:
        Set registers

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static u32 MP4SetRegs(DecContainer * pDecContainer, u32 strmBusAddress)
{

    u32 tmp = 0;
    i32 itmp;

#ifdef _DEC_PP_USAGE
    MP4DecPpUsagePrint(pDecContainer, DECPP_UNSPECIFIED,
                       pDecContainer->StrmStorage.workOut, 1,
                       pDecContainer->StrmStorage.latestId);
#endif

/*
    if(pDecContainer->Hdrs.interlaced)
        SetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_OUT_TILED_E, 0);
        */

    SetDecRegister(pDecContainer->mp4Regs, HWIF_STARTMB_X, 0);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_STARTMB_Y, 0);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_BLACKWHITE_E, 0);

    SetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_OUT_BASE,
                   MP4DecResolveBus(pDecContainer,
                                    pDecContainer->StrmStorage.workOut));

    SetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_OUT_DIS, 0);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_FILTERING_DIS, 1);

    if(pDecContainer->VopDesc.vopCodingType == BVOP)
    {
        MP4DEC_API_DEBUG(("decoding a B picture\n"));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_REFER0_BASE,
                       MP4DecResolveBus(pDecContainer,
                                        pDecContainer->StrmStorage.work1));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_REFER1_BASE,
                       MP4DecResolveBus(pDecContainer,
                                        pDecContainer->StrmStorage.work1));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_REFER2_BASE,
                       MP4DecResolveBus(pDecContainer,
                                        pDecContainer->StrmStorage.work0));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_REFER3_BASE,
                       MP4DecResolveBus(pDecContainer,
                                        pDecContainer->StrmStorage.work0));
    }
    else
    {
        MP4DEC_API_DEBUG(("decoding anchor picture\n"));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_REFER0_BASE,
                       MP4DecResolveBus(pDecContainer,
                                        pDecContainer->StrmStorage.work0));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_REFER1_BASE,
                       MP4DecResolveBus(pDecContainer,
                                        pDecContainer->StrmStorage.work0));

    }

    SetDecRegister(pDecContainer->mp4Regs, HWIF_FCODE_FWD_HOR,
                   pDecContainer->VopDesc.fcodeFwd);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_FCODE_FWD_VER,
                   pDecContainer->VopDesc.fcodeFwd);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_MPEG4_VC1_RC,
                   pDecContainer->VopDesc.vopRoundingType);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_INTRADC_VLC_THR,
                   pDecContainer->VopDesc.intraDcVlcThr);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_INIT_QP,
                   pDecContainer->VopDesc.qP);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_SYNC_MARKER_E, 1);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_PIC_INTER_E,
                   pDecContainer->VopDesc.vopCodingType != IVOP ? 1 : 0);

    if(pDecContainer->rlcMode && pDecContainer->VopDesc.vopCodingType != BVOP)
    {
        MP4DEC_API_DEBUG(("RLC mode\n"));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_RLC_MODE_E, 1);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_RLC_VLC_BASE,
                       pDecContainer->MbSetDesc.rlcDataMem.busAddress);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_MB_CTRL_BASE,
                       pDecContainer->MbSetDesc.ctrlDataMem.busAddress);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_MPEG4_DC_BASE,
                       pDecContainer->MbSetDesc.DcCoeffMem.busAddress);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_DIFF_MV_BASE,
                       pDecContainer->MbSetDesc.mvDataMem.busAddress);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_STREAM_LEN, 0);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_STRM_START_BIT, 0);
    }
    else
    {
        SetDecRegister(pDecContainer->mp4Regs, HWIF_RLC_MODE_E, 0);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_VOP_TIME_INCR,
                       pDecContainer->Hdrs.vopTimeIncrementResolution);

        /* tmp is strmBusAddress + number of bytes decoded by SW */
        tmp = pDecContainer->StrmDesc.pStrmCurrPos -
            pDecContainer->StrmDesc.pStrmBuffStart;
        tmp = strmBusAddress + tmp;

        /* bus address must not be zero */
        if(!(tmp & ~0x7))
        {
            return HANTRO_NOK;
        }

        /* pointer to start of the stream, mask to get the pointer to
         * previous 64-bit aligned position */
        SetDecRegister(pDecContainer->mp4Regs, HWIF_RLC_VLC_BASE, tmp & ~0x7);

        /* amount of stream (as seen by the HW), obtained as amount of
         * stream given by the application subtracted by number of bytes
         * decoded by SW (if strmBusAddress is not 64-bit aligned -> adds
         * number of bytes from previous 64-bit aligned boundary) */
        SetDecRegister(pDecContainer->mp4Regs, HWIF_STREAM_LEN,
                       pDecContainer->StrmDesc.strmBuffSize -
                       ((tmp & ~0x7) - strmBusAddress));
        SetDecRegister(pDecContainer->mp4Regs, HWIF_STRM_START_BIT,
                       pDecContainer->StrmDesc.bitPosInWord + 8 * (tmp & 0x7));

    }

    /* MPEG-4 ASP */
    SetDecRegister(pDecContainer->mp4Regs, HWIF_FCODE_BWD_HOR,
                   pDecContainer->VopDesc.fcodeBwd);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_FCODE_BWD_VER,
                   pDecContainer->VopDesc.fcodeBwd);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_PIC_INTERLACE_E,
                   pDecContainer->Hdrs.interlaced);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_PIC_B_E,
                   pDecContainer->VopDesc.vopCodingType == BVOP ? 1 : 0);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_WRITE_MVS_E,
                   (pDecContainer->VopDesc.vopCodingType == PVOP ? 1 : 0) &&
                   !pDecContainer->Hdrs.lowDelay);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_DIR_MV_BASE,
                   pDecContainer->StrmStorage.directMvs.busAddress);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_PREV_ANC_TYPE,
                   pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                      StrmStorage.work0].
                   picType == PVOP);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_TYPE1_QUANT_E,
                   pDecContainer->Hdrs.quantType == 1);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_QTABLE_BASE,
                   pDecContainer->StrmStorage.quantMatLinear.busAddress);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_MV_ACCURACY_FWD,
                   pDecContainer->Hdrs.quarterpel ? 1 : 0);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_ALT_SCAN_FLAG_E,
                   pDecContainer->VopDesc.altVerticalScanFlag);
    SetDecRegister(pDecContainer->mp4Regs, HWIF_TOPFIELDFIRST_E,
                   pDecContainer->VopDesc.topFieldFirst);
    if(pDecContainer->VopDesc.vopCodingType == BVOP)
    {
        /*  use 32 bit variables? */
        if(pDecContainer->VopDesc.trd == 0)
            itmp = 0;
        else
            itmp = (((long long int) pDecContainer->VopDesc.trb << 27) +
                    pDecContainer->VopDesc.trd - 1) /
                pDecContainer->VopDesc.trd;

        SetDecRegister(pDecContainer->mp4Regs, HWIF_TRB_PER_TRD_D0, itmp);

        /* plus 1 */
        itmp = (((long long int) (2 * pDecContainer->VopDesc.trb + 1) << 27) +
                2 * pDecContainer->VopDesc.trd) /
            (2 * pDecContainer->VopDesc.trd + 1);

        SetDecRegister(pDecContainer->mp4Regs, HWIF_TRB_PER_TRD_D1, itmp);

        /* minus 1 */
        if(pDecContainer->VopDesc.trd == 0)
            itmp = 0;
        else
            itmp =
                (((long long int) (2 * pDecContainer->VopDesc.trb - 1) << 27) +
                 2 * pDecContainer->VopDesc.trd -
                 2) / (2 * pDecContainer->VopDesc.trd - 1);

        SetDecRegister(pDecContainer->mp4Regs, HWIF_TRB_PER_TRD_DM1, itmp);

    }

    if(pDecContainer->StrmStorage.sorensonSpark)
        SetDecRegister(pDecContainer->mp4Regs, HWIF_SORENSON_E,
                       pDecContainer->StrmStorage.sorensonVer);

    SetConformanceRegs( pDecContainer );

    /* Setup reference picture buffer */
    if( pDecContainer->refBufSupport )
    {
        RefbuSetup( &pDecContainer->refBufferCtrl, pDecContainer->mp4Regs, 
                    REFBU_FRAME, 
                    pDecContainer->VopDesc.vopCodingType == IVOP,
                    pDecContainer->VopDesc.vopCodingType == BVOP, 
                    0, 2, 0);
    }

    if( pDecContainer->tiledModeSupport)
    {
        pDecContainer->tiledReferenceEnable = 
            DecSetupTiledReference( pDecContainer->mp4Regs, 
                pDecContainer->tiledModeSupport,
                pDecContainer->Hdrs.interlaced );
    }
    else
    {
        pDecContainer->tiledReferenceEnable = 0;
    }

    return HANTRO_OK;
}

/*------------------------------------------------------------------------------

    Function name: MP4DecSetupDeinterlace

    Functional description:
        Setup PP interface for deinterlacing

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void MP4DecSetupDeinterlace(DecContainer * pDecCont)
{
    pDecCont->ppControl.picStruct = DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
    pDecCont->ppControl.bottomBusLuma = pDecCont->ppControl.inputBusLuma +
        (pDecCont->VopDesc.vopWidth << 4);
    pDecCont->ppControl.bottomBusChroma = pDecCont->ppControl.inputBusChroma +
        (pDecCont->VopDesc.vopWidth << 4);
}

/*------------------------------------------------------------------------------

    Function name: MP4DecPrepareFieldProcessing

    Functional description:
        Setup PP interface for deinterlacing

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void MP4DecPrepareFieldProcessing(DecContainer * pDecCont, u32 picIndexOverride)
{
    u32 picIndex;
    pDecCont->ppControl.picStruct =
        pDecCont->VopDesc.topFieldFirst ?
        DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;

    if(picIndexOverride != BQUEUE_UNUSED)
        picIndex = picIndexOverride;
    else if( pDecCont->StrmStorage.parallelMode2)
        picIndex = pDecCont->StrmStorage.workOutPrev;
    else 
        picIndex = pDecCont->StrmStorage.work0;

    if(pDecCont->StrmStorage.work0 >= 0 &&
       pDecCont->StrmStorage.work0 < pDecCont->StrmStorage.numBuffers )
        pDecCont->ApiStorage.bufferForPp = picIndex+1;
    else
        ASSERT(0);
  
    if(pDecCont->VopDesc.topFieldFirst)
    {
        pDecCont->ppControl.inputBusLuma =
            MP4DecResolveBus(pDecCont, picIndex);

        pDecCont->ppControl.inputBusChroma =
            pDecCont->ppControl.inputBusLuma +
            ((pDecCont->VopDesc.vopWidth * pDecCont->VopDesc.vopHeight) << 8);
    }
    else
    {

        MP4_SET_BOT_ADDR(picIndex);

    }

    MP4_SET_FIELD_DIMENSIONS;

    MP4DEC_API_DEBUG(("FIELD: send %s\n",
                      pDecCont->ppControl.picStruct ==
                      DECPP_PIC_TOP_FIELD_FRAME ? "top" : "bottom"));
}

/*------------------------------------------------------------------------------

    Function name: MP4DecParallelPP

    Functional description:
        Setup PP for parallel use

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void MP4DecParallelPP(DecContainer * pDecContainer, u32 indexForPp)
{

#ifdef _DEC_PP_USAGE
    MP4DecPpUsagePrint(pDecContainer, DECPP_PARALLEL,
                       pDecContainer->StrmStorage.work0, 0,
                       pDecContainer->StrmStorage.pPicBuf[pDecContainer->
                                                          StrmStorage.work0].
                       picId);
#endif

    pDecContainer->ppControl.usePipeline = 0;

    pDecContainer->ppControl.inputBusLuma =
        MP4DecResolveBus(pDecContainer, indexForPp);

    pDecContainer->ppControl.inputBusChroma =
        pDecContainer->ppControl.inputBusLuma +
        ((pDecContainer->VopDesc.vopWidth *
          pDecContainer->VopDesc.vopHeight) << 8);

    /* forward tiled mode */
    pDecContainer->ppControl.tiledInputMode = 
        pDecContainer->StrmStorage.pPicBuf[indexForPp].tiledMode;
    pDecContainer->ppControl.progressiveSequence =
        !pDecContainer->Hdrs.interlaced;

    pDecContainer->ppControl.inwidth =
        pDecContainer->ppControl.croppedW =
        pDecContainer->VopDesc.vopWidth << 4;
    pDecContainer->ppControl.inheight =
        pDecContainer->ppControl.croppedH =
        pDecContainer->VopDesc.vopHeight << 4;

    {
        u32 tmp = GetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_OUT_ENDIAN);

        pDecContainer->ppControl.littleEndian =
            (tmp == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
    }
    pDecContainer->ppControl.wordSwap =
        GetDecRegister(pDecContainer->mp4Regs, HWIF_DEC_OUTSWAP32_E) ? 1 : 0;

    if(pDecContainer->ppConfigQuery.deinterlace)
    {
        MP4DecSetupDeinterlace(pDecContainer);
    }
    /* if field output is used, send only a field to PP */
    else if(pDecContainer->Hdrs.interlaced)
    {
        MP4DecPrepareFieldProcessing(pDecContainer, BQUEUE_UNUSED);
    }

}

/*------------------------------------------------------------------------------

    Function name: PPControl

    Functional description:
        set up and start pp

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/

static void PPControl(DecContainer * pDecCont, u32 pipelineOff)
{
    u32 indexForPp = BUFFER_UNDEFINED;
    u32 nextBufferIndex;
    u32 prevBufferIndex;
    u32 previousB;

    DecPpInterface * pc = &pDecCont->ppControl;
    DecHdrs        * pHdrs = &pDecCont->Hdrs;

    pDecCont->ppConfigQuery.tiledMode =
        pDecCont->tiledReferenceEnable;
    pDecCont->PPConfigQuery(pDecCont->ppInstance,
                            &pDecCont->ppConfigQuery);

    /* If we have once enabled parallel mode 2, keep it on */
    if(pDecCont->StrmStorage.parallelMode2)
        pipelineOff = 1;

    PPMultiBufferSetup(pDecCont, (pipelineOff ||
    	!pDecCont->ppConfigQuery.pipelineAccepted));

    /* Check whether to enable parallel mode 2 */
    if( (!pDecCont->ppConfigQuery.pipelineAccepted ||
         pDecCont->Hdrs.interlaced) &&
        pc->multiBufStat != MULTIBUFFER_DISABLED &&
        !pDecCont->Hdrs.lowDelay &&
        !pDecCont->StrmStorage.parallelMode2 )
    {
        pDecCont->StrmStorage.parallelMode2 = 1;
        pDecCont->StrmStorage.pm2AllProcessedFlag = 0;
        pDecCont->StrmStorage.pm2lockBuf    = pc->prevAnchorDisplayIndex;
        pDecCont->StrmStorage.pm2StartPoint = 
            pDecCont->VopDesc.vopNumber;
    }

    pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].
        sendToPp = 1;

    /* Select new PP buffer index to use. If multibuffer is disabled, use
     * previous buffer, otherwise select new buffer from queue. */
    prevBufferIndex = pc->bufferIndex;
    if(pc->multiBufStat != MULTIBUFFER_DISABLED)
    {
        u32 buf0 = BQUEUE_UNUSED;
        /* In parallel mode 2, we must refrain from reusing future
         * anchor frame output buffer until it has been put out. */
        if(pDecCont->StrmStorage.parallelMode2)
            buf0 = pDecCont->StrmStorage.pm2lockBuf;
        nextBufferIndex = BqueueNext( &pDecCont->StrmStorage.bqPp,
            buf0,
            BQUEUE_UNUSED,
            BQUEUE_UNUSED,
            pDecCont->VopDesc.vopCodingType == BVOP );
        pc->bufferIndex = nextBufferIndex;
    }
    else
    {
        nextBufferIndex = pc->bufferIndex = 0;
    }

    if(pDecCont->StrmStorage.parallelMode2)
    {
        if(pDecCont->StrmStorage.previousB)
        {
            pc->displayIndex = pc->bufferIndex;
        }
        else
        {
            pc->displayIndex = pDecCont->StrmStorage.pm2lockBuf;
            /* Fix for case if stream has only one frame; then for NextPicture() 
             * we should have a proper buffer index instead of undefined. */
            if(pc->displayIndex == BQUEUE_UNUSED)
                pc->displayIndex = pc->bufferIndex;
        }
    }
    else if(pHdrs->lowDelay ||
       pDecCont->VopDesc.vopCodingType == BVOP)
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
        u32 workBuffer;

        if(pDecCont->StrmStorage.parallelMode2)
            workBuffer = pDecCont->StrmStorage.workOutPrev;
        else
            workBuffer = pDecCont->StrmStorage.workOut;

        luma = MP4DecResolveBus(pDecCont, workBuffer);
        chroma = luma + ((pDecCont->VopDesc.vopWidth *
                 pDecCont->VopDesc.vopHeight) << 8);

        pDecCont->PPBufferData(pDecCont->ppInstance, 
            pc->bufferIndex, luma, chroma);
    }

    if(pc->multiBufStat == MULTIBUFFER_FULLMODE)
    {
        MP4DEC_API_DEBUG(("Full pipeline# \n"));
        pc->usePipeline =
                pDecCont->ppConfigQuery.pipelineAccepted;
        MP4DecRunFullmode(pDecCont);
	    pDecCont->StrmStorage.previousModeFull = 1;
    }
    else if(pDecCont->StrmStorage.previousModeFull == 1)
    {
        if(pDecCont->VopDesc.vopCodingType == BVOP)
    	{
        	pDecCont->StrmStorage.previousB = 1;
    	}
    	else
    	{
        	pDecCont->StrmStorage.previousB = 0;
    	}
	
    	if(pDecCont->VopDesc.vopCodingType == BVOP)
        {
            MP4DEC_API_DEBUG(("PIPELINE OFF, DON*T SEND B TO PP\n"));
            indexForPp = BUFFER_UNDEFINED;
            pc->inputBusLuma = 0;
        }
    	pc->ppStatus = DECPP_IDLE;
							      
    	pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work0].sendToPp = 1;
	    
    	pDecCont->StrmStorage.previousModeFull = 0;
    }
    else
    {
        if(!pDecCont->StrmStorage.parallelMode2)
        {
            pc->bufferIndex = pc->displayIndex;
        }
        else
        {
            previousB = pDecCont->StrmStorage.previousB;
        }
        if(pDecCont->VopDesc.vopCodingType == BVOP)
        {
                pDecCont->StrmStorage.previousB = 1;
        }
        else
        {
                pDecCont->StrmStorage.previousB = 0;
        }

        if((!pHdrs->lowDelay && (pDecCont->VopDesc.vopCodingType != BVOP)) ||
            pHdrs->interlaced ||
            (!pDecCont->VopDesc.vopCoded && !pDecCont->rlcMode) ||
            pipelineOff)
        {
            pc->usePipeline = 0;
        }
        else
        {
            MP4DEC_API_DEBUG(("RUN PP  # \n"));
            pc->usePipeline =
                pDecCont->ppConfigQuery.pipelineAccepted;
        }

        if(!pc->usePipeline)
        {
            /* pipeline not accepted, don't run for first picture */
            if(pDecCont->VopDesc.vopNumberInSeq)
            {

                /* In parallel mode 2 we always run previous decoder output
                 * picture through PP */
                if( pDecCont->StrmStorage.parallelMode2)
                {
                    pc->inputBusLuma =
                        MP4DecResolveBus(pDecCont, pDecCont->StrmStorage.workOutPrev);

                    /* If we got an anchor frame, lock the PP output buffer */
                    if(!previousB)
                    {
                        pDecCont->StrmStorage.pm2lockBuf = pc->bufferIndex;
                    }

                    /* forward tiled mode */
                    pDecCont->ppControl.tiledInputMode = 
                        pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOutPrev].
                        tiledMode;
                    pDecCont->ppControl.progressiveSequence =
                        !pDecCont->Hdrs.interlaced;

                    pc->inputBusChroma =
                        pc->inputBusLuma +
                        ((pDecCont->VopDesc.vopWidth *
                          pDecCont->VopDesc.vopHeight) << 8);

                    pc->inwidth = pc->croppedW =
                        pDecCont->VopDesc.vopWidth << 4;
                    pc->inheight = pc->croppedH =
                        pDecCont->VopDesc.vopHeight << 4;
                    {
                        u32 value = GetDecRegister(pDecCont->mp4Regs,
                                                   HWIF_DEC_OUT_ENDIAN);

                        pc->littleEndian =
                            (value == DEC_X170_LITTLE_ENDIAN) ? 1 : 0;
                    }
                    pc->wordSwap =
                        GetDecRegister(pDecCont->mp4Regs,
                                       HWIF_DEC_OUTSWAP32_E) ? 1 : 0;

                    MP4DEC_API_DEBUG(("sending NON B to pp\n"));
                    indexForPp = pDecCont->StrmStorage.workOutPrev;

                    if(pDecCont->ppConfigQuery.deinterlace)
                    {
                        MP4DecSetupDeinterlace(pDecCont);
                    }
                    /* if field output is used, send only a field to PP */
                    else if(pHdrs->interlaced)
                    {
                        MP4DecPrepareFieldProcessing(pDecCont, 
                            BQUEUE_UNUSED);
                    }
                    pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp = 1;
                }

                /*if:
                 * B pictures allowed and non B picture OR
                 * B pictures not allowed. must be coded*/
                else if(((!pHdrs->lowDelay &&
                     pDecCont->VopDesc.vopCodingType != BVOP) ||
                    pHdrs->lowDelay) &&
                   pDecCont->VopDesc.vopCoded)
                {
                    MP4DEC_API_DEBUG(("sending NON B to pp\n"));
                    indexForPp = pDecCont->StrmStorage.work0;
                    /* set up parallel pp run */
                    MP4DecParallelPP(pDecCont, indexForPp);
                }
                else
                {
                    if(pDecCont->VopDesc.vopCodingType == BVOP)
                    {
                        MP4DEC_API_DEBUG(("PIPELINE OFF, DON*T SEND B TO PP\n"));
                        indexForPp = BUFFER_UNDEFINED;
                        pc->inputBusLuma = 0;
                    }
                    else
                    {
                        MP4DEC_API_DEBUG(("sending NON B to pp\n"));
                        indexForPp = pDecCont->StrmStorage.workOut;
                        /* set up parallel pp run */
                        MP4DecParallelPP(pDecCont, indexForPp);
                    }
                }
            }
            else
            {
                pc->inputBusLuma = 0;
            }
        }
        else
        {
#ifdef _DEC_PP_USAGE
            MP4DecPpUsagePrint(pDecCont, DECPP_PIPELINED,
                               pDecCont->StrmStorage.workOut, 0,
                               pDecCont->StrmStorage.pPicBuf[pDecCont->
                                                                  StrmStorage.
                                                                  workOut].picId);
#endif

            pc->inputBusLuma = pc->inputBusChroma = 0;
            indexForPp = pDecCont->StrmStorage.workOut;
            pc->inwidth = pc->croppedW =
                pDecCont->VopDesc.vopWidth << 4;
            pc->inheight = pc->croppedH =
                pDecCont->VopDesc.vopHeight << 4;
            pc->tiledInputMode = pDecCont->tiledReferenceEnable;
            pc->progressiveSequence =
                !pDecCont->Hdrs.interlaced;
        }

        /* start PP */
        if(((pc->inputBusLuma && !pc->usePipeline) ||
           (!pc->inputBusLuma && pc->usePipeline))
           && pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp)
        {
            {
                u32 fdis = 1;

                if(pc->usePipeline)
                {
                    if(pDecCont->VopDesc.vopCodingType == BVOP)
                        SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_OUT_DIS, 1);

                    fdis = pDecCont->ApiStorage.disableFilter;
                }
                SetDecRegister(pDecCont->mp4Regs, HWIF_FILTERING_DIS, fdis);

            }

            ASSERT(indexForPp != BUFFER_UNDEFINED);
            MP4DEC_API_DEBUG(("sent to pp %d\n", indexForPp));

            MP4DEC_API_DEBUG(("send %d %d %d %d, indexForPp %d\n",
                              pDecCont->StrmStorage.pPicBuf[0].sendToPp,
                              pDecCont->StrmStorage.pPicBuf[1].sendToPp,
                              pDecCont->StrmStorage.pPicBuf[2].sendToPp,
                              pDecCont->StrmStorage.pPicBuf[3].sendToPp,
                              indexForPp));

            ASSERT(pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp == 1);
            pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp = 0;
            pDecCont->PPRun(pDecCont->ppInstance,
                                 pc);

            pc->ppStatus = DECPP_RUNNING;
        }
    	pDecCont->StrmStorage.previousModeFull = 0;
    }

    if( pDecCont->VopDesc.vopCodingType != BVOP )
    {
        pc->prevAnchorDisplayIndex = nextBufferIndex;
    }

    if( pc->inputBusLuma == 0 &&
        pDecCont->StrmStorage.parallelMode2 )
    {
        BqueueDiscard( &pDecCont->StrmStorage.bqPp,
                       pc->bufferIndex );
    }

    /* Clear 2nd field indicator from structure if in parallel mode 2 and
     * outputting separate fields and output buffer not yet filled. This 
     * prevents NextPicture from going bonkers if stream ends before 
     * frame 2. */

    if(pDecCont->VopDesc.vopNumber -
            pDecCont->StrmStorage.pm2StartPoint < 2 &&
       pDecCont->StrmStorage.parallelMode2 &&
       pDecCont->Hdrs.interlaced && 
            !pDecCont->ppConfigQuery.deinterlace )
    {
        pDecCont->ApiStorage.bufferForPp = NO_BUFFER;
    }

}

/*------------------------------------------------------------------------------

    Function name: MP4Setindexes

    Functional description:
        set up index bank for this picture

    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void MP4SetIndexes(DecContainer * pDecCont)
{

    pDecCont->StrmStorage.workOutPrev =
        pDecCont->StrmStorage.workOut;

    if(pDecCont->VopDesc.vopCoded)
    {
        pDecCont->StrmStorage.workOut = BqueueNext( 
            &pDecCont->StrmStorage.bq,
            pDecCont->StrmStorage.work0,
            pDecCont->Hdrs.lowDelay ? BQUEUE_UNUSED : pDecCont->StrmStorage.work1, 
            BQUEUE_UNUSED,
            pDecCont->VopDesc.vopCodingType == BVOP );
    }
    else
    {
        pDecCont->StrmStorage.workOut =
            pDecCont->StrmStorage.work0;
    }

    if(pDecCont->VopDesc.vopCodingType == BVOP)
    {
        pDecCont->StrmStorage.prevBIdx = pDecCont->StrmStorage.workOut;
    }

    if(pDecCont->StrmStorage.previousNotCoded)
    {
        if(pDecCont->VopDesc.vopCodingType != BVOP)
        {
            /*
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.workOut].
                dataIndex = 
              pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work0].
                dataIndex;*/
            pDecCont->StrmStorage.previousNotCoded = 0;
        }
    }

}

/*------------------------------------------------------------------------------

    Function name: MP4CheckFormatSupport

    Functional description:
        Check if mpeg4 or sorenson are supported

    Input:
        container

    Return values:
        return zero for OK

------------------------------------------------------------------------------*/
static u32 MP4CheckFormatSupport(MP4DecStrmFmt strmFmt)
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

    if(strmFmt == MP4DEC_SORENSON)
        return (hwConfig.sorensonSparkSupport == SORENSON_SPARK_NOT_SUPPORTED);
    else
        return (hwConfig.mpeg4Support == MPEG4_NOT_SUPPORTED);
}

/*------------------------------------------------------------------------------

    Function name: MP4DecFilterDisable

    Functional description:
        check possibility to use filter

    Input:
        container

    Return values:
        returns nonzero for disable

------------------------------------------------------------------------------*/
static u32 MP4DecFilterDisable(DecContainer * pDecCont)
{
    u32 ret = 0;

    /* combined mode must be enabled */
    if(pDecCont->ppInstance == NULL)
        ret++;

    if(!pDecCont->Hdrs.lowDelay)
        ret++;

    if(pDecCont->Hdrs.interlaced)
        ret++;

    return ret;
}

/*------------------------------------------------------------------------------

    Function name: MP4DecFieldAndValidBuffer

    Functional description:
        set picture info for field data for second field

    Input:
        container, picture desc, id

    Return values:
        void

------------------------------------------------------------------------------*/
static void MP4DecFieldAndValidBuffer(MP4DecPicture * pPicture,
                             DecContainer * pDecCont, u32* picIndex )
{

    picture_t *pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf;
    pPicture->interlaced = 1;
    pPicture->fieldPicture = 1;
    pDecCont->ppControl.picStruct =
        pDecCont->VopDesc.
        topFieldFirst ? DECPP_PIC_BOT_FIELD_FRAME :
        DECPP_PIC_TOP_FIELD_FRAME;
    pPicture->topField = pDecCont->VopDesc.topFieldFirst ? 0 : 1;

    if(pDecCont->ApiStorage.bufferForPp >= 1)
        *picIndex = pDecCont->ApiStorage.bufferForPp - 1;
    else
        *picIndex = 0;

    pDecCont->ApiStorage.bufferForPp = NO_BUFFER;
    pPic[*picIndex].sendToPp = 1;
    MP4DEC_API_DEBUG(("Processing second field in NextPicture index %d\n", *picIndex));

}

/*------------------------------------------------------------------------------

    Function name: PPMultiBufferInit

    Functional description:
        Modify state of pp output buffering.

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/
static void PPMultiBufferInit(DecContainer * pDecCont)
{
    DecPpQuery * pq = &pDecCont->ppConfigQuery;
    DecPpInterface * pc = &pDecCont->ppControl;

    if(pq->multiBuffer)
    {
        if(!pq->pipelineAccepted || pDecCont->Hdrs.interlaced)
        {

            MP4DEC_API_DEBUG(("MULTIBUFFER_SEMIMODE\n"));
            pc->multiBufStat = MULTIBUFFER_SEMIMODE;
        }
        else
        {
            MP4DEC_API_DEBUG(("MULTIBUFFER_FULLMODE\n"));
            pc->multiBufStat = MULTIBUFFER_FULLMODE;
        }
    }
    else
    {
        pc->multiBufStat = MULTIBUFFER_DISABLED;
    }

}

/*------------------------------------------------------------------------------

    Function name: PPMultiBufferSetup

    Functional description:
        Modify state of pp output buffering.

    Input:
        container
        pipelineOff     override pipeline setting

    Return values:
        void

------------------------------------------------------------------------------*/
static void PPMultiBufferSetup(DecContainer * pDecCont, u32 pipelineOff)
{

    if( pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED)
    {
        MP4DEC_API_DEBUG(("MULTIBUFFER_DISABLED\n"));
        return;
    }

    if(pipelineOff &&
    	 (pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE))
    {
        MP4DEC_API_DEBUG(("MULTIBUFFER_SEMIMODE\n"));
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_SEMIMODE;
    }
    
    if(!pipelineOff && !pDecCont->Hdrs.interlaced &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_SEMIMODE))
    {
    	MP4DEC_API_DEBUG(("MULTIBUFFER_FULLMODE\n"));
    	pDecCont->ppControl.multiBufStat = MULTIBUFFER_FULLMODE;
    }

    if(pDecCont->ppControl.multiBufStat == MULTIBUFFER_UNINIT)
        PPMultiBufferInit(pDecCont);

}

/*------------------------------------------------------------------------------

    Function name: MP4DecRunFullmode

    Functional description:


    Input:
        container

    Return values:
        void

------------------------------------------------------------------------------*/
static void MP4DecRunFullmode(DecContainer * pDecCont)
{
    u32 indexForPp = BUFFER_UNDEFINED;
    DecPpInterface * pc = &pDecCont->ppControl;
    DecHdrs        * pHdrs = &pDecCont->Hdrs;

#ifdef _DEC_PP_USAGE
        MP4DecPpUsagePrint(pDecCont, DECPP_PIPELINED,
                           pDecCont->StrmStorage.workOut, 0,
                           pDecCont->StrmStorage.pPicBuf[pDecCont->
                                                              StrmStorage.
                                                              workOut].picId);
#endif

    if(!pDecCont->StrmStorage.previousModeFull && pDecCont->VopDesc.vopNumber)
    {
        if(pDecCont->VopDesc.vopCodingType == BVOP)
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work0].sendToPp = 0;
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work1].sendToPp = 0;	   
        }
        else
        {
            pDecCont->StrmStorage.pPicBuf[pDecCont->StrmStorage.work0].sendToPp = 0;
        }	
    }
    
    if(pDecCont->VopDesc.vopCodingType == BVOP)
    {
        pDecCont->StrmStorage.previousB = 1;
    }
    else
    {
        pDecCont->StrmStorage.previousB = 0;
    }
    
    indexForPp = pDecCont->StrmStorage.workOut;
    pc->tiledInputMode = pDecCont->tiledReferenceEnable;
    pc->progressiveSequence =
        !pDecCont->Hdrs.interlaced;

    pc->inputBusLuma = MP4DecResolveBus(pDecCont, pDecCont->StrmStorage.workOut);
    pc->inputBusChroma =  pc->inputBusLuma +
        ((pDecCont->VopDesc.vopWidth *
          pDecCont->VopDesc.vopHeight) << 8);

    pc->inwidth = pc->croppedW =
        pDecCont->VopDesc.vopWidth << 4;
    pc->inheight = pc->croppedH =
        pDecCont->VopDesc.vopHeight << 4;
    
    {
        u32 fdis = 1;

        if(pDecCont->VopDesc.vopCodingType == BVOP)
            SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_OUT_DIS, 1);

        fdis = pDecCont->ApiStorage.disableFilter;
        SetDecRegister(pDecCont->mp4Regs, HWIF_FILTERING_DIS, fdis);
    }

    ASSERT(indexForPp != BUFFER_UNDEFINED);
    MP4DEC_API_DEBUG(("sent to pp %d\n", indexForPp));

    MP4DEC_API_DEBUG(("send %d %d %d %d, indexForPp %d\n",
                      pDecCont->StrmStorage.pPicBuf[0].sendToPp,
                      pDecCont->StrmStorage.pPicBuf[1].sendToPp,
                      pDecCont->StrmStorage.pPicBuf[2].sendToPp,
                      pDecCont->StrmStorage.pPicBuf[3].sendToPp,
                      indexForPp));

    ASSERT(pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp == 1);
    pDecCont->StrmStorage.pPicBuf[indexForPp].sendToPp = 0;

    pDecCont->PPRun(pDecCont->ppInstance,
                         pc);

    pc->ppStatus = DECPP_RUNNING;

}

/*------------------------------------------------------------------------------

    Function name: MP4DecPeek

    Functional description:
        Retrieve last decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct

    Output:
        pPicture Decoder output picture.

    Return values:
        MP4DEC_OK         No picture available.
        MP4DEC_PIC_RDY    Picture ready.

------------------------------------------------------------------------------*/
MP4DecRet MP4DecPeek(MP4DecInst decInst, MP4DecPicture * pPicture)
{
    /* Variables */
    MP4DecRet returnValue = MP4DEC_PIC_RDY;
    DecContainer *pDecCont;
    picture_t *pPic;
    u32 picIndex = BUFFER_UNDEFINED;
    u32 tmp = 0;
    u32 luma = 0;
    u32 chroma = 0;

    /* Code */
    MP4_API_TRC("\nMP4DecPeek#");

    /* Check that function input parameters are valid */
    if(pPicture == NULL)
    {
        MP4_API_TRC("MP4DecPeek# ERROR: pPicture is NULL");
        return (MP4DEC_PARAM_ERROR);
    }

    pDecCont = (DecContainer *) decInst;

    /* Check if decoder is in an incorrect mode */
    if(decInst == NULL || pDecCont->ApiStorage.DecStat == UNINIT)
    {
        MP4_API_TRC("MP4DecPeek# ERROR: Decoder not initialized");
        return (MP4DEC_NOT_INITIALIZED);
    }

    /* no output pictures available */
    if(!pDecCont->StrmStorage.outCount)
    {
        (void) G1DWLmemset(pPicture, 0, sizeof(MP4DecPicture));
        pPicture->pOutputPicture = NULL;
        pPicture->interlaced = pDecCont->Hdrs.interlaced;
        /* print nothing to send out */
        returnValue = MP4DEC_OK;
    }
    else
    {
        /* output current (last decoded) picture */
        picIndex = pDecCont->StrmStorage.workOut;

        MP4FillPicStruct(pPicture, pDecCont, picIndex);

        /* frame output */
        pPicture->fieldPicture = 0;
        pPicture->topField = 0;
    }

    return returnValue;
}

/*------------------------------------------------------------------------------

    Function name: MP4DecSetLatency

    Functional description:
        set current video latencyMS to vdec driver for vpu dvfs

    Input:
        decInst     Reference to decoder instance.
        latencyMS   


    Return values:
        MP4DEC_OK;

------------------------------------------------------------------------------*/
MP4DecRet MP4DecSetLatency(MP4DecInst decInst, int latencyMS)
{
    DecContainer *pDecCont = (DecContainer *) decInst;

    DWLSetLatency(pDecCont->dwl, latencyMS);

    return MP4DEC_OK;
}
