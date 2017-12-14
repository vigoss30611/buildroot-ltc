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
--  Abstract : Application Programming Interface (API) functions
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264decapi.c,v $
--  $Date: 2011/02/04 12:40:54 $
--  $Revision: 1.310 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "h264hwd_container.h"
#include "h264decapi.h"
#include "h264hwd_decoder.h"
#include "h264hwd_util.h"
#include "h264hwd_exports.h"
#include "h264hwd_dpb.h"
#include "h264hwd_neighbour.h"
#include "h264hwd_asic.h"
#include "h264hwd_regdrv.h"
#include "h264hwd_byte_stream.h"
#include "deccfg.h"
#include "h264_pp_multibuffer.h"
#include "tiledref.h"
#include "dwl.h"
/*------------------------------------------------------------------------------
       Version Information - DO NOT CHANGE!
------------------------------------------------------------------------------*/

#define H264DEC_MAJOR_VERSION 1
#define H264DEC_MINOR_VERSION 1

#define H264DEC_BUILD_MAJOR 2
#define H264DEC_BUILD_MINOR 231
#define H264DEC_SW_BUILD ((H264DEC_BUILD_MAJOR * 1000) + H264DEC_BUILD_MINOR)

/*
 * H264DEC_TRACE         Trace H264 Decoder API function calls. H264DecTrace
 *                       must be implemented externally.
 * H264DEC_EVALUATION    Compile evaluation version, restricts number of frames
 *                       that can be decoded
 */

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

#ifdef H264DEC_TRACE
#define DEC_API_TRC(str)    H264DecTrace(str)
#else
#define DEC_API_TRC(str)    do{}while(0)
#endif

static void h264UpdateAfterPictureDecode(decContainer_t * pDecCont);
static u32 h264SpsSupported(const decContainer_t * pDecCont);
static u32 h264PpsSupported(const decContainer_t * pDecCont);
static u32 h264StreamIsBaseline(const decContainer_t * pDecCont);

static u32 h264AllocateResources(decContainer_t * pDecCont);
static void bsdDecodeReturn(u32 retval);
static void h264InitPicFreezeOutput(decContainer_t * pDecCont, u32 fromOldDpb);
static void h264GetSarInfo(const storage_t * pStorage,
                           u32 * sar_width, u32 * sar_height);
extern void h264PreparePpRun(decContainer_t * pDecCont);

static void h264CheckReleasePpAndHw(decContainer_t *pDecCont);

/*------------------------------------------------------------------------------

    Function: H264DecInit()

        Functional description:
            Initialize decoder software. Function reserves memory for the
            decoder instance and calls h264bsdInit to initialize the
            instance data.

        Inputs:
            noOutputReordering  flag to indicate decoder that it doesn't have
                                to try to provide output pictures in display
                                order, saves memory
            useVideoFreezeConcealment
                                flag to enable error concealment method where
                                decoding starts at next intra picture after 
                                error in bitstream.
            useDisplaySmooothing
                                flag to enable extra buffering in DPB output
                                so that application may read output pictures
                                one by one
        Outputs:
            decInst             pointer to initialized instance is stored here

        Returns:
            H264DEC_OK        successfully initialized the instance
            H264DEC_INITFAIL  initialization failed
            H264DEC_PARAM_ERROR invalid parameters
            H264DEC_MEMFAIL   memory allocation failed
            H264DEC_DWL_ERROR error initializing the system interface
------------------------------------------------------------------------------*/

H264DecRet H264DecInit(H264DecInst * pDecInst, u32 noOutputReordering,
                       u32 useVideoFreezeConcealment,
                       u32 useDisplaySmoothing,
                       DecRefFrmFormat referenceFrameFormat,
                       u32 mmuEnable)
{

    /*@null@ */ decContainer_t *pDecCont;
    /*@null@ */ const void *dwl;

    G1DWLInitParam_t dwlInit;
    DWLHwConfig_t hwCfg;
    u32 asicID;

    DEC_API_TRC("H264DecInit#\n");

    /* check that right shift on negative numbers is performed signed */
    /*lint -save -e* following check causes multiple lint messages */
#if (((-1) >> 1) != (-1))
#error Right bit-shifting (>>) does not preserve the sign
#endif
    /*lint -restore */

    if(pDecInst == NULL)
    {
        DEC_API_TRC("H264DecInit# ERROR: pDecInst == NULL");
        return (H264DEC_PARAM_ERROR);
    }

    *pDecInst = NULL;   /* return NULL instance for any error */

    /* check for proper hardware */
    asicID = G1DWLReadAsicID();

    if((asicID >> 16) < 0x8170U &&
       (asicID >> 16) != 0x6731U )
    {
        DEC_API_TRC("H264DecInit# ERROR: HW not recognized/unsupported!\n");
        return H264DEC_FORMAT_NOT_SUPPORTED;
    }

    /* check that H.264 decoding supported in HW */
    (void) G1DWLmemset(&hwCfg, 0, sizeof(DWLHwConfig_t));
    DWLReadAsicConfig(&hwCfg);
    if(!hwCfg.h264Support)
    {
        DEC_API_TRC("H264DecInit# ERROR: H264 not supported in HW\n");
        return H264DEC_FORMAT_NOT_SUPPORTED;
    }

    dwlInit.clientType = DWL_CLIENT_TYPE_H264_DEC;
    dwlInit.mmuEnable = mmuEnable;

    dwl = G1DWLInit(&dwlInit);

    if(dwl == NULL)
    {
        DEC_API_TRC("H264DecInit# ERROR: DWL Init failed\n");
        return (H264DEC_DWL_ERROR);
    }

    pDecCont = (decContainer_t *) G1DWLmalloc(sizeof(decContainer_t));

    if(pDecCont == NULL)
    {
        (void) G1DWLRelease(dwl);

        DEC_API_TRC("H264DecInit# ERROR: Memory allocation failed\n");
        return (H264DEC_MEMFAIL);
    }

    (void) G1DWLmemset(pDecCont, 0, sizeof(decContainer_t));
    pDecCont->dwl = dwl;

    h264bsdInit(&pDecCont->storage, noOutputReordering,
        useDisplaySmoothing);

    pDecCont->decStat = H264DEC_INITIALIZED;

    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_MODE, DEC_X170_MODE_H264);

    /* these parameters are defined in deccfg.h */

    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_OUT_ENDIAN,
                   DEC_X170_OUTPUT_PICTURE_ENDIAN);
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_IN_ENDIAN,
                   DEC_X170_INPUT_DATA_ENDIAN);
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_STRENDIAN_E,
                   DEC_X170_INPUT_STREAM_ENDIAN);
    /*
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_OUT_TILED_E,
                   DEC_X170_OUTPUT_FORMAT);
                   */
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_MAX_BURST,
                   DEC_X170_BUS_BURST_LENGTH);
    if ((asicID >> 16) == 0x8170U)
    {
        SetDecRegister(pDecCont->h264Regs, HWIF_PRIORITY_MODE,
                       DEC_X170_ASIC_SERVICE_PRIORITY);
    }
    else
    {
        SetDecRegister(pDecCont->h264Regs, HWIF_DEC_SCMD_DIS,
            DEC_X170_SCMD_DISABLE);
    }
    DEC_SET_APF_THRESHOLD(pDecCont->h264Regs);
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_LATENCY,
                   DEC_X170_LATENCY_COMPENSATION);
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_DATA_DISC_E,
                   DEC_X170_DATA_DISCARD_ENABLE);
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_OUTSWAP32_E,
                   DEC_X170_OUTPUT_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_INSWAP32_E,
                   DEC_X170_INPUT_DATA_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_STRSWAP32_E,
                   DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);

#if( DEC_X170_HW_TIMEOUT_INT_ENA  != 0)
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_TIMEOUT_E, 1);
#else
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_TIMEOUT_E, 0);
#endif

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_CLK_GATE_E, 1);
#else
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_CLK_GATE_E, 0);
#endif

#if( DEC_X170_USING_IRQ  == 0)
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_IRQ_DIS, 1);
#else
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_IRQ_DIS, 0);
#endif

    /* set AXI RW IDs */
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_AXI_RD_ID,
        (DEC_X170_AXI_ID_R & 0xFFU));
    SetDecRegister(pDecCont->h264Regs, HWIF_DEC_AXI_WR_ID,
        (DEC_X170_AXI_ID_W & 0xFFU));

    /* Set prediction filter taps */
    SetDecRegister(pDecCont->h264Regs, HWIF_PRED_BC_TAP_0_0, 1);
    SetDecRegister(pDecCont->h264Regs, HWIF_PRED_BC_TAP_0_1, (u32)(-5));
    SetDecRegister(pDecCont->h264Regs, HWIF_PRED_BC_TAP_0_2, 20);

    /* save HW version so we dont need to check it all the time when deciding the control stuff */
    pDecCont->is8190 = (asicID >> 16) != 0x8170U ? 1 : 0;
    pDecCont->h264ProfileSupport = hwCfg.h264Support;
    if((asicID >> 16)  == 0x8170U)
        useVideoFreezeConcealment = 0;

    /* save ref buffer support status */
    pDecCont->refBufSupport = hwCfg.refBufSupport;
    if(referenceFrameFormat == DEC_REF_FRM_TILED_DEFAULT)
    {
        /* Assert support in HW before enabling.. */
        if(!hwCfg.tiledModeSupport)
        {
            return H264DEC_FORMAT_NOT_SUPPORTED;
        }
        pDecCont->tiledModeSupport = hwCfg.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;
#ifndef _DISABLE_PIC_FREEZE
    pDecCont->storage.intraFreeze = useVideoFreezeConcealment;
#endif
    pDecCont->storage.pictureBroken = HANTRO_FALSE;

    pDecCont->maxDecPicWidth = hwCfg.maxDecPicWidth;    /* max decodable picture width */

    pDecCont->checksum = pDecCont;  /* save instance as a checksum */

#ifdef _ENABLE_2ND_CHROMA
    pDecCont->storage.enable2ndChroma = 1;
#endif

    *pDecInst = (H264DecInst) pDecCont;

    DEC_API_TRC("H264DecInit# OK\n");

    return (H264DEC_OK);
}

/*------------------------------------------------------------------------------

    Function: H264DecGetInfo()

        Functional description:
            This function provides read access to decoder information. This
            function should not be called before H264DecDecode function has
            indicated that headers are ready.

        Inputs:
            decInst     decoder instance

        Outputs:
            pDecInfo    pointer to info struct where data is written

        Returns:
            H264DEC_OK            success
            H264DEC_PARAM_ERROR     invalid parameters
            H264DEC_HDRS_NOT_RDY  information not available yet

------------------------------------------------------------------------------*/
H264DecRet H264DecGetInfo(H264DecInst decInst, H264DecInfo * pDecInfo)
{
    u32 croppingFlag;
    decContainer_t *pDecCont = (decContainer_t *) decInst;
    storage_t *pStorage;

    DEC_API_TRC("H264DecGetInfo#");

    if(decInst == NULL || pDecInfo == NULL)
    {
        DEC_API_TRC("H264DecGetInfo# ERROR: decInst or pDecInfo is NULL\n");
        return (H264DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("H264DecGetInfo# ERROR: Decoder not initialized\n");
        return (H264DEC_NOT_INITIALIZED);
    }

    pStorage = &pDecCont->storage;

    if(pStorage->activeSps == NULL || pStorage->activePps == NULL)
    {
        DEC_API_TRC("H264DecGetInfo# ERROR: Headers not decoded yet\n");
        return (H264DEC_HDRS_NOT_RDY);
    }

    /* h264bsdPicWidth and -Height return dimensions in macroblock units,
     * picWidth and -Height in pixels */
    pDecInfo->picWidth = h264bsdPicWidth(pStorage) << 4;
    pDecInfo->picHeight = h264bsdPicHeight(pStorage) << 4;
    pDecInfo->videoRange = h264bsdVideoRange(pStorage);
    pDecInfo->matrixCoefficients = h264bsdMatrixCoefficients(pStorage);
    pDecInfo->monoChrome = h264bsdIsMonoChrome(pStorage);
    pDecInfo->interlacedSequence = pStorage->activeSps->frameMbsOnlyFlag == 0 ? 1 : 0;
    pDecInfo->picBuffSize = pStorage->dpb->dpbSize + 1;
    pDecInfo->multiBuffPpSize =  pStorage->dpb->noReordering? 2 : pDecInfo->picBuffSize;
    
    if (pStorage->mvc)
        pDecInfo->multiBuffPpSize *= 2;

    h264GetSarInfo(pStorage, &pDecInfo->sarWidth, &pDecInfo->sarHeight);

    h264bsdCroppingParams(pStorage,
                          &croppingFlag,
                          &pDecInfo->cropParams.cropLeftOffset,
                          &pDecInfo->cropParams.cropOutWidth,
                          &pDecInfo->cropParams.cropTopOffset,
                          &pDecInfo->cropParams.cropOutHeight);

    if(croppingFlag == 0)
    {
        pDecInfo->cropParams.cropLeftOffset = 0;
        pDecInfo->cropParams.cropTopOffset = 0;
        pDecInfo->cropParams.cropOutWidth = pDecInfo->picWidth;
        pDecInfo->cropParams.cropOutHeight = pDecInfo->picHeight;
    }

    if(pDecCont->tiledReferenceEnable)
    {
        pDecInfo->outputFormat = H264DEC_TILED_YUV420;
    }
    else if(pDecInfo->monoChrome)
    {
        pDecInfo->outputFormat = H264DEC_YUV400;
    }
    else
    {
        pDecInfo->outputFormat = H264DEC_SEMIPLANAR_YUV420;
    }

    DEC_API_TRC("H264DecGetInfo# OK\n");

    return (H264DEC_OK);
}

/*------------------------------------------------------------------------------

    Function: H264DecRelease()

        Functional description:
            Release the decoder instance. Function calls h264bsdShutDown to
            release instance data and frees the memory allocated for the
            instance.

        Inputs:
            decInst     Decoder instance

        Outputs:
            none

        Returns:
            none

------------------------------------------------------------------------------*/

void H264DecRelease(H264DecInst decInst)
{

    decContainer_t *pDecCont = (decContainer_t *) decInst;
    const void *dwl;

    DEC_API_TRC("H264DecRelease#\n");

    if(pDecCont == NULL)
    {
        DEC_API_TRC("H264DecRelease# ERROR: decInst == NULL\n");
        return;
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("H264DecRelease# ERROR: Decoder not initialized\n");
        return;
    }

    /* PP instance must be already disconnected at this point */
    ASSERT(pDecCont->pp.ppInstance == NULL);

    dwl = pDecCont->dwl;

    if(pDecCont->asicRunning)
    {
        /* stop HW */
        SetDecRegister(pDecCont->h264Regs, HWIF_DEC_IRQ_STAT, 0);
        SetDecRegister(pDecCont->h264Regs, HWIF_DEC_IRQ, 0);
        SetDecRegister(pDecCont->h264Regs, HWIF_DEC_E, 0);
        DWLDisableHW(pDecCont->dwl, 4 * 1, pDecCont->h264Regs[1]);
        G1G1DWLReleaseHw(dwl);  /* release HW lock */
        pDecCont->asicRunning = 0;
    }
    else if (pDecCont->keepHwReserved)
        G1G1DWLReleaseHw(dwl);  /* release HW lock */

    h264bsdShutdown(&pDecCont->storage);

    h264bsdFreeDpb(dwl, pDecCont->storage.dpbs[0]);
    if (pDecCont->storage.dpbs[1]->dpbSize)
        h264bsdFreeDpb(dwl, pDecCont->storage.dpbs[1]);

    ReleaseAsicBuffers(dwl, pDecCont->asicBuff);

    pDecCont->checksum = NULL;
    G1DWLfree(pDecCont);

    (void) G1DWLRelease(dwl);

    DEC_API_TRC("H264DecRelease# OK\n");

    return;
}

/*------------------------------------------------------------------------------

    Function: H264DecDecode

        Functional description:
            Decode stream data. Calls h264bsdDecode to do the actual decoding.

        Input:
            decInst     decoder instance
            pInput      pointer to input struct

        Outputs:
            pOutput     pointer to output struct

        Returns:
            H264DEC_NOT_INITIALIZED   decoder instance not initialized yet
            H264DEC_PARAM_ERROR         invalid parameters

            H264DEC_STRM_PROCESSED    stream buffer decoded
            H264DEC_HDRS_RDY    headers decoded, stream buffer not finished
            H264DEC_PIC_DECODED decoding of a picture finished
            H264DEC_STRM_ERROR  serious error in decoding, no valid parameter
                                sets available to decode picture data
            H264DEC_PENDING_FLUSH   next invocation of H264DecDecode() function
                                    flushed decoded picture buffer, application
                                    needs to read all output pictures (using
                                    H264DecNextPicture function) before calling
                                    H264DecDecode() again. Used only when
                                    useDisplaySmoothing was enabled in init.

            H264DEC_HW_BUS_ERROR    decoder HW detected a bus error
            H264DEC_SYSTEM_ERROR    wait for hardware has failed
            H264DEC_MEMFAIL         decoder failed to allocate memory
            H264DEC_DWL_ERROR   System wrapper failed to initialize
            H264DEC_HW_TIMEOUT  HW timeout
            H264DEC_HW_RESERVED HW could not be reserved
------------------------------------------------------------------------------*/
H264DecRet H264DecDecode(H264DecInst decInst, const H264DecInput * pInput,
                         H264DecOutput * pOutput)
{
    decContainer_t *pDecCont = (decContainer_t *) decInst;
    u32 strmLen;
    const u8 *tmpStream;
    H264DecRet returnValue = H264DEC_STRM_PROCESSED;

    DEC_API_TRC("H264DecDecode#\n");

    /* Check that function input parameters are valid */
    if(pInput == NULL || pOutput == NULL || decInst == NULL)
    {
        DEC_API_TRC("H264DecDecode# ERROR: NULL arg(s)\n");
        return (H264DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("H264DecDecode# ERROR: Decoder not initialized\n");
        return (H264DEC_NOT_INITIALIZED);
    }

    if(pInput->dataLen == 0 ||
       pInput->dataLen > DEC_X170_MAX_STREAM ||
       X170_CHECK_VIRTUAL_ADDRESS(pInput->pStream) ||
       X170_CHECK_BUS_ADDRESS(pInput->streamBusAddress))
    {
        DEC_API_TRC("H264DecDecode# ERROR: Invalid arg value\n");
        return H264DEC_PARAM_ERROR;
    }

#ifdef H264DEC_EVALUATION
    if(pDecCont->picNumber > H264DEC_EVALUATION)
    {
        DEC_API_TRC("H264DecDecode# H264DEC_EVALUATION_LIMIT_EXCEEDED\n");
        return H264DEC_EVALUATION_LIMIT_EXCEEDED;
    }
#endif

    pDecCont->streamPosUpdated = 0;
    pOutput->pStrmCurrPos = NULL;
    pDecCont->hwStreamStartBus = pInput->streamBusAddress;
    pDecCont->pHwStreamStart = pInput->pStream;
    strmLen = pDecCont->hwLength = pInput->dataLen;
    tmpStream = pInput->pStream;

    pDecCont->skipNonReference = pInput->skipNonReference;

    /* Switch to RLC mode, i.e. sw performs entropy decoding */
    if(pDecCont->reallocate)
    {
        DEBUG_PRINT(("H264DecDecode: Reallocate\n"));
        pDecCont->rlcMode = 1;
        pDecCont->reallocate = 0;

        /* Reallocate only once */
        if(pDecCont->asicBuff->mbCtrl.virtualAddress == NULL)
        {
            if(h264AllocateResources(pDecCont) != 0)
            {
                /* signal that decoder failed to init parameter sets */
                pDecCont->storage.activePpsId = MAX_NUM_PIC_PARAM_SETS;
                DEC_API_TRC("H264DecDecode# ERROR: Reallocation failed\n");
                return H264DEC_MEMFAIL;
            }

            h264bsdResetStorage(&pDecCont->storage);
        }

    }

    /* get PP pipeline status at the beginning of each new picture */
    if(pDecCont->pp.ppInstance != NULL &&
       pDecCont->storage.picStarted == HANTRO_FALSE)
    {   
        /* store old multibuffer status to compare with new */
        const u32 oldMulti = pDecCont->pp.ppInfo.multiBuffer; 
        u32 maxId = pDecCont->storage.dpb->noReordering ? 1 :
            pDecCont->storage.dpb->dpbSize;

        ASSERT(pDecCont->pp.PPConfigQuery != NULL);
        pDecCont->pp.ppInfo.tiledMode =
            pDecCont->tiledReferenceEnable;
        pDecCont->pp.PPConfigQuery(pDecCont->pp.ppInstance,
                                   &pDecCont->pp.ppInfo);

        TRACE_PP_CTRL("H264DecDecode: PP pipelineAccepted = %d\n",
            pDecCont->pp.ppInfo.pipelineAccepted);
        TRACE_PP_CTRL("H264DecDecode: PP multiBuffer = %d\n",
            pDecCont->pp.ppInfo.multiBuffer);
        
        /* increase number of buffers if multiview decoding */
        if (pDecCont->storage.numViews)
        {
            maxId += pDecCont->storage.dpb->noReordering ? 1 :
                pDecCont->storage.dpb->dpbSize + 1;
            maxId = MIN(maxId, 16);
        }

        if (oldMulti != pDecCont->pp.ppInfo.multiBuffer)
        {
            h264PpMultiInit(pDecCont, maxId);
        }
        /* just increase amount of buffers */
        else if (maxId > pDecCont->pp.multiMaxId)
            h264PpMultiMvc(pDecCont, maxId);
    }

	if (pDecCont->processed > 0)
	{
		pDecCont->processedinterval++;
	}
    do
    {
        u32 decResult;
        u32 numReadBytes = 0;
        storage_t *pStorage = &pDecCont->storage;

        DEBUG_PRINT(("H264DecDecode: mode is %s\n",
                     pDecCont->rlcMode ? "RLC" : "VLC"));

        if(pDecCont->decStat == H264DEC_NEW_HEADERS)
        {
            decResult = H264BSD_HDRS_RDY;
            pDecCont->decStat = H264DEC_INITIALIZED;
        }
        else if(pDecCont->decStat == H264DEC_BUFFER_EMPTY)
        {
            DEBUG_PRINT(("H264DecDecode: Skip h264bsdDecode\n"));
            DEBUG_PRINT(("H264DecDecode: Jump to H264BSD_PIC_RDY\n"));

            decResult = H264BSD_PIC_RDY;
        }
        else
        {
            decResult = h264bsdDecode(pDecCont, tmpStream, strmLen,
                                      pInput->picId, &numReadBytes);

            ASSERT(numReadBytes <= strmLen);

            bsdDecodeReturn(decResult);
        }

        tmpStream += numReadBytes;
        strmLen -= numReadBytes;

        switch (decResult)
        {
        case H264BSD_HDRS_RDY:
            {
                h264CheckReleasePpAndHw(pDecCont);

                pDecCont->storage.multiBuffPp = 
                    pDecCont->pp.ppInstance != NULL &&
                    pDecCont->pp.ppInfo.multiBuffer;

                if(pStorage->dpb->flushed && pStorage->dpb->numOut)
                {
                    /* output first all DPB stored pictures */
                    pStorage->dpb->flushed = 0;
                    pDecCont->decStat = H264DEC_NEW_HEADERS;
                    /* if display smoothing used -> indicate that all pictures
                     * have to be read out */
                    if ((pStorage->dpb->totBuffers > 
                         pStorage->dpb->dpbSize + 1) &&
                        !pDecCont->storage.multiBuffPp)
                    {
                        returnValue = H264DEC_PENDING_FLUSH;
                    }
                    else
                    {
                        returnValue = H264DEC_PIC_DECODED;
                    }

                    /* base view -> flush another view dpb */
                    if (pDecCont->storage.numViews &&
                        pDecCont->storage.view == 0)
                    {
                        h264bsdFlushDpb(pStorage->dpbs[1]);
                    }

                    DEC_API_TRC
                        ("H264DecDecode# H264DEC_PIC_DECODED (DPB flush caused by new SPS)\n");
                    strmLen = 0;
                    break;
                }

                if(!h264SpsSupported(pDecCont))
                {
                    pStorage->activeSpsId = MAX_NUM_SEQ_PARAM_SETS;
                    pStorage->activePpsId = MAX_NUM_PIC_PARAM_SETS;

                    returnValue = H264DEC_STREAM_NOT_SUPPORTED;
                    DEC_API_TRC
                        ("H264DecDecode# H264DEC_STREAM_NOT_SUPPORTED\n");
                }
                else if((h264bsdAllocateSwResources(pDecCont->dwl,
                                                    pStorage,
                                                    (pDecCont->
                                                    h264ProfileSupport == H264_HIGH_PROFILE) ? 1 :
                                                    0) != 0) ||
                        (h264AllocateResources(pDecCont) != 0))
                {
                    /* signal that decoder failed to init parameter sets */
                    /* TODO: miten viewit */
                    pStorage->activeSpsId = MAX_NUM_SEQ_PARAM_SETS;
                    pStorage->activePpsId = MAX_NUM_PIC_PARAM_SETS;

                    /* reset strmLen to force memfail return also for secondary
                     * view */
                    strmLen = 0;

                    returnValue = H264DEC_MEMFAIL;
                    DEC_API_TRC("H264DecDecode# H264DEC_MEMFAIL\n");
                }
                else
                {
                    if((pStorage->activePps->numSliceGroups != 1) &&
                       (h264StreamIsBaseline(pDecCont) == 0))
                    {
                        pStorage->activeSpsId = MAX_NUM_SEQ_PARAM_SETS;
                        pStorage->activePpsId = MAX_NUM_PIC_PARAM_SETS;

                        returnValue = H264DEC_STREAM_NOT_SUPPORTED;
                        DEC_API_TRC
                            ("H264DecDecode# H264DEC_STREAM_NOT_SUPPORTED, FMO in Main/High Profile\n");
                    }
                    
                    pDecCont->asicBuff->enableDmvAndPoc = 0;
                    pStorage->dpb->interlaced = (pStorage->activeSps->frameMbsOnlyFlag == 0) ? 1 : 0;
                    
                    /* FMO always decoded in rlc mode */
                    if((pStorage->activePps->numSliceGroups != 1) &&
                       (pDecCont->rlcMode == 0))
                    {
                        /* set to uninit state */
                        pStorage->activeSpsId = MAX_NUM_SEQ_PARAM_SETS;
                        pStorage->activeViewSpsId[0] =
                        pStorage->activeViewSpsId[1] =
                            MAX_NUM_SEQ_PARAM_SETS;
                        pStorage->activePpsId = MAX_NUM_PIC_PARAM_SETS;
                        pStorage->picStarted = HANTRO_FALSE;
                        pDecCont->decStat = H264DEC_INITIALIZED;

                        pDecCont->rlcMode = 1;
                        pStorage->prevBufNotFinished = HANTRO_FALSE;
                        DEC_API_TRC
                            ("H264DecDecode# H264DEC_ADVANCED_TOOLS, FMO\n");

                        returnValue = H264DEC_ADVANCED_TOOLS;
                    }
                    else
                    {
                        u32 maxId = pDecCont->storage.dpb->noReordering ? 1 :
                            pDecCont->storage.dpb->dpbSize;

                        /* enable direct MV writing and POC tables for
                         * high/main streams.
                         * enable it also for any "baseline" stream which have
                         * main/high tools enabled */
                        if((pStorage->activeSps->profileIdc > 66 &&
                            pStorage->activeSps->constrained_set0_flag == 0) ||
                           (h264StreamIsBaseline(pDecCont) == 0))
                        {
                            pDecCont->asicBuff->enableDmvAndPoc = 1;
                        }

                        /* increase number of buffers if multiview decoding */
                        if (pDecCont->storage.numViews)
                        {
                            maxId += pDecCont->storage.dpb->noReordering ? 1 :
                                pDecCont->storage.dpb->dpbSize + 1;
                            maxId = MIN(maxId, 16);
                        }

                        /* reset multibuffer status */
                        if (pStorage->view == 0)
                            h264PpMultiInit(pDecCont, maxId);
                        else if (maxId > pDecCont->pp.multiMaxId)
                            h264PpMultiMvc(pDecCont, maxId);

                        DEC_API_TRC("H264DecDecode# H264DEC_HDRS_RDY\n");
                        returnValue = H264DEC_HDRS_RDY;
                    }
                }

                if (!pStorage->view)
                {
                    /* reset strmLen only for base view -> no HDRS_RDY to
                     * application when param sets activated for stereo view */
                    strmLen = 0;
                    pDecCont->storage.secondField = 0;
                }
                break;
            }
        case H264BSD_PIC_RDY:
            {
                u32 asic_status;
                u32 pictureBroken;
                DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

                pictureBroken = (pStorage->pictureBroken && pStorage->intraFreeze &&
                                 !IS_IDR_NAL_UNIT(pStorage->prevNalUnit));

                if( pDecCont->decStat != H264DEC_BUFFER_EMPTY && !pictureBroken)
                {
                    /* setup the reference frame list; just at picture start */
                    dpbStorage_t *dpb = pStorage->dpb;
                    dpbPicture_t *buffer = dpb->buffer;

                    /* list in reorder */
                    u32 i;

                    if(!h264PpsSupported(pDecCont))
                    {
                        pStorage->activeSpsId = MAX_NUM_SEQ_PARAM_SETS;
                        pStorage->activePpsId = MAX_NUM_PIC_PARAM_SETS;

                        returnValue = H264DEC_STREAM_NOT_SUPPORTED;
                        DEC_API_TRC
                            ("H264DecDecode# H264DEC_STREAM_NOT_SUPPORTED, Main/High Profile tools detected\n");
                        goto end;
                    }

                    if((pDecCont->is8190 == 0) && (pDecCont->rlcMode == 0))
                    {
                        for(i = 0; i < dpb->dpbSize; i++)
                        {
                            pAsicBuff->refPicList[i] =
                                buffer[dpb->list[i]].data->busAddress;
                        }
                    }
                    else
                    {
                        for(i = 0; i < dpb->dpbSize; i++)
                        {
                            pAsicBuff->refPicList[i] =
                                buffer[i].data->busAddress;
                        }
                    }

                    pAsicBuff->maxRefFrm = dpb->maxRefFrames;
                    pAsicBuff->outBuffer = pStorage->currImage->data;

                    pAsicBuff->chromaQpIndexOffset =
                        pStorage->activePps->chromaQpIndexOffset;
                    pAsicBuff->chromaQpIndexOffset2 =
                        pStorage->activePps->chromaQpIndexOffset2;
                    pAsicBuff->filterDisable = 0;

                    h264bsdDecodePicOrderCnt(pStorage->poc,
                                             pStorage->activeSps,
                                             pStorage->sliceHeader,
                                             pStorage->prevNalUnit);

                    if(pDecCont->rlcMode)
                    {
                        if(pStorage->numConcealedMbs == pStorage->picSizeInMbs)
                        {
                            pAsicBuff->wholePicConcealed = 1;
                            pAsicBuff->filterDisable = 1;
                            pAsicBuff->chromaQpIndexOffset = 0;
                            pAsicBuff->chromaQpIndexOffset2 = 0;
                        }
                        else
                        {
                            pAsicBuff->wholePicConcealed = 0;
                        }

                        PrepareIntra4x4ModeData(pStorage, pAsicBuff);
                        PrepareMvData(pStorage, pAsicBuff);
                        PrepareRlcCount(pStorage, pAsicBuff);
                    }
                    else
                    {
                        H264SetupVlcRegs(pDecCont);
                    }

                    DEBUG_PRINT(("Save DPB status\n"));
                    /* we trust our memcpy; ignore return value */
                    (void) G1DWLmemcpy(&pStorage->dpb[1], &pStorage->dpb[0],
                                     sizeof(*pStorage->dpb));

                    DEBUG_PRINT(("Save POC status\n"));
                    (void) G1DWLmemcpy(&pStorage->poc[1], &pStorage->poc[0],
                                     sizeof(*pStorage->poc));

                    /* create output picture list */
                    h264UpdateAfterPictureDecode(pDecCont);

                    /* enable output writing by default */
                    pDecCont->asicBuff->disableOutWriting = 0;

                    /* prepare PP if needed */
                    h264PreparePpRun(pDecCont);
                }
                else
                {
                    pDecCont->decStat = H264DEC_INITIALIZED;
                }

                /* run asic and react to the status */
                if( !pictureBroken )
                {
                    asic_status = H264RunAsic(pDecCont, pAsicBuff);
                }
                else
                {
                    if( pDecCont->storage.picStarted )
                    {
                        if( !pStorage->sliceHeader->fieldPicFlag || 
                            !pStorage->secondField )
                        {
                            h264InitPicFreezeOutput(pDecCont, 0);
                            h264UpdateAfterPictureDecode(pDecCont);
                        }
                    }
                    asic_status = DEC_X170_IRQ_STREAM_ERROR;
                }

                if (pStorage->view)
                    pStorage->nonInterViewRef = 0;

                /* Handle system error situations */
                if(asic_status == X170_DEC_TIMEOUT)
                {
                    /* This timeout is DWL(software/os) generated */
                    DEC_API_TRC
                        ("H264DecDecode# H264DEC_HW_TIMEOUT, SW generated\n");
                    return H264DEC_HW_TIMEOUT;
                }
                else if(asic_status == X170_DEC_SYSTEM_ERROR)
                {
                    DEC_API_TRC("H264DecDecode# H264DEC_SYSTEM_ERROR\n");
                    return H264DEC_SYSTEM_ERROR;
                }
                else if(asic_status == X170_DEC_HW_RESERVED)
                {
                    DEC_API_TRC("H264DecDecode# H264DEC_HW_RESERVED\n");
                    return H264DEC_HW_RESERVED;
                }

                /* Handle possible common HW error situations */
                if(asic_status & DEC_X170_IRQ_BUS_ERROR)
                {
                    pOutput->pStrmCurrPos = (u8 *) pInput->pStream;
                    pOutput->strmCurrBusAddress = pInput->streamBusAddress;
                    pOutput->dataLeft = pInput->dataLen;

                    DEC_API_TRC("H264DecDecode# H264DEC_HW_BUS_ERROR\n");
                    return H264DEC_HW_BUS_ERROR;
                }
                else if(asic_status & DEC_X170_IRQ_TIMEOUT)
                {
                    /* This timeout is HW generated */
                    DEBUG_PRINT(("IRQ: HW TIMEOUT\n"));
                    if(pDecCont->mmuEnable)
                    {
                        DWLLibReset(pDecCont->dwl);
                        pOutput->dataLeft = 0;//pInput->dataLen;
                        //only reset mmu 
                        return H264DEC_HW_BUS_ERROR;
                    }

#ifdef H264_TIMEOUT_ASSERT
                    ASSERT(0);
#endif
                    if(pDecCont->packetDecoded != HANTRO_TRUE)
                    {
                        DEBUG_PRINT(("reset picStarted\n"));
                        pDecCont->storage.picStarted = HANTRO_FALSE;
                    }

                    pDecCont->storage.pictureBroken = HANTRO_TRUE;
                    h264InitPicFreezeOutput(pDecCont, 1);

                    /* PP has to run again for the concealed picture */
                    if(pDecCont->pp.ppInstance != NULL)
                    {
                        TRACE_PP_CTRL
                            ("H264DecDecode: Concealed picture, PP should run again\n");
                        pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;

                        if(pDecCont->pp.ppInfo.multiBuffer != 0)
                        {
                            if(pDecCont->pp.decPpIf.usePipeline != 0)
                            {
                                /* remove pipelined pic from PP list */
                                h264PpMultiRemovePic(pDecCont, pStorage->currImage->data);
                            }

                            if(pDecCont->pp.queuedPicToPp[pStorage->view] ==
                               pStorage->currImage->data)
                            {
                                /* current picture cannot be in the queue */
                                pDecCont->pp.queuedPicToPp[pStorage->view] =
                                    NULL;
                            }
                        }
                    }

                    if(!pDecCont->rlcMode)
                    {
                        strmData_t *strm = pDecCont->storage.strm;
                        const u8 *next =
                            h264bsdFindNextStartCode(strm->pStrmBuffStart,
                                                     strm->strmBuffSize);

                        if(next != NULL)
                        {
                            u32 consumed;

                            tmpStream -= numReadBytes;
                            strmLen += numReadBytes;

                            consumed = (u32) (next - tmpStream);
                            tmpStream += consumed;
                            strmLen -= consumed;
                        }
                    }

                    pDecCont->streamPosUpdated = 0;
                    pDecCont->picNumber++;

                    pDecCont->packetDecoded = HANTRO_FALSE;
                    pStorage->skipRedundantSlices = HANTRO_TRUE;

                    /* Remove this NAL unit from stream */
                    strmLen = 0;
                    DEC_API_TRC("H264DecDecode# H264DEC_PIC_DECODED\n");
                    returnValue = H264DEC_PIC_DECODED;
                    break;
                }

                if(pDecCont->rlcMode)
                {
                    if(asic_status & DEC_X170_IRQ_STREAM_ERROR)
                    {
                        DEBUG_PRINT
                            (("H264DecDecode# IRQ_STREAM_ERROR in RLC mode)!\n"));
                    }

                    /* It was rlc mode, but switch back to vlc when allowed */
                    if(pDecCont->tryVlc)
                    {
                        pStorage->prevBufNotFinished = HANTRO_FALSE;
                        DEBUG_PRINT(("H264DecDecode: RLC mode used, but try VLC again\n"));
                        /* start using VLC mode again */
                        pDecCont->rlcMode = 0;
                        pDecCont->tryVlc = 0;
                        pDecCont->modeChange = 0;
                    }

                    pDecCont->picNumber++;

#ifdef FFWD_WORKAROUND
                    pStorage->prevIdrPicReady = 
                        IS_IDR_NAL_UNIT(pStorage->prevNalUnit);
#endif /* FFWD_WORKAROUND */

                    DEC_API_TRC("H264DecDecode# H264DEC_PIC_DECODED\n");
                    returnValue = H264DEC_PIC_DECODED;
                    strmLen = 0;

                    break;
                }

                /* from down here we handle VLC mode */

                /* in High/Main streams if HW model returns ASO interrupt, it
                 * really means that there is a generic stream error. */
                if((asic_status & DEC_X170_IRQ_ASO) &&
                   (pAsicBuff->enableDmvAndPoc != 0 ||
                   (h264StreamIsBaseline(pDecCont) == 0)))
                {
                    DEBUG_PRINT(("ASO received in High/Main stream => STREAM_ERROR\n"));
                    asic_status &= ~DEC_X170_IRQ_ASO;
                    asic_status |= DEC_X170_IRQ_STREAM_ERROR;
                }

                /* Check for CABAC zero words here */
                if( asic_status & DEC_X170_IRQ_BUFFER_EMPTY)
                {
                    if( pDecCont->storage.activePps->entropyCodingModeFlag )
                    {
                        u32 tmp;
                        u32 checkWords = 1;
                        strmData_t strmTmp = *pDecCont->storage.strm;
                        tmp = pDecCont->pHwStreamStart-pInput->pStream;
                        strmTmp.pStrmCurrPos = pDecCont->pHwStreamStart;
                        strmTmp.strmBuffReadBits = 8*tmp;
                        strmTmp.bitPosInWord = 0;
                        strmTmp.strmBuffSize = pInput->dataLen;

                        tmp = GetDecRegister(pDecCont->h264Regs, HWIF_START_CODE_E );
                        /* In NAL unit mode, if NAL unit was of type 
                         * "reserved" or sth other unsupported one, we need
                         * to skip zero word check. */
                        if( tmp == 0 )
                        {
                            tmp = pInput->pStream[0] & 0x1F;
                            if( tmp != NAL_CODED_SLICE &&
                                tmp != NAL_CODED_SLICE_IDR )
                                checkWords = 0;
                        }

                        if(checkWords)
                        {
                            tmp = h264CheckCabacZeroWords( &strmTmp );
                            if( tmp != HANTRO_OK )
                            {
                                DEBUG_PRINT(("CABAC_ZERO_WORD error after packet => STREAM_ERROR\n"));
                            } /* cabacZeroWordError */
                        }
                    }
                }

                /* Handle ASO */
                if(asic_status & DEC_X170_IRQ_ASO)
                {
                    DEBUG_PRINT(("IRQ: ASO dedected\n"));
                    ASSERT(pDecCont->rlcMode == 0);

                    pDecCont->reallocate = 1;
                    pDecCont->tryVlc = 1;
                    pDecCont->modeChange = 1;

                    /* restore DPB status */
                    DEBUG_PRINT(("Restore DPB status\n"));

                    /* we trust our memcpy; ignore return value */
                    (void) G1DWLmemcpy(&pStorage->dpb[0], &pStorage->dpb[1],
                                     sizeof(dpbStorage_t));

                    DEBUG_PRINT(("Restore POC status\n"));
                    (void) G1DWLmemcpy(&pStorage->poc[0], &pStorage->poc[1],
                                     sizeof(*pStorage->poc));

                    pStorage->skipRedundantSlices = HANTRO_FALSE;
                    pStorage->asoDetected = 1;

                    DEC_API_TRC("H264DecDecode# H264DEC_ADVANCED_TOOLS, ASO\n");
                    returnValue = H264DEC_ADVANCED_TOOLS;

                    /* PP has to run again for ASO picture */
                    if(pDecCont->pp.ppInstance != NULL)
                    {
                        TRACE_PP_CTRL
                            ("H264DecDecode: ASO detected, PP should run again\n");
                        pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;
                        
                        if(pDecCont->pp.ppInfo.multiBuffer != 0)
                        {
                            if(pDecCont->pp.decPpIf.usePipeline != 0)
                            {
                                /* remove pipelined pic from PP list */
                                h264PpMultiRemovePic(pDecCont, pStorage->currImage->data);
                            }

                            if(pDecCont->pp.queuedPicToPp[pStorage->view] ==
                               pStorage->currImage->data)
                            {
                                /* current picture cannot be in the queue */
                                pDecCont->pp.queuedPicToPp[pStorage->view] =
                                    NULL;
                            }
                        }
                    }

                    goto end;
                }
                else if(asic_status & DEC_X170_IRQ_BUFFER_EMPTY)
                {
                    DEBUG_PRINT(("IRQ: BUFFER EMPTY\n"));

                    /* a packet successfully decoded, don't reset picStarted flag if
                     * there is a need for rlc mode */
                    pDecCont->decStat = H264DEC_BUFFER_EMPTY;
                    pDecCont->packetDecoded = HANTRO_TRUE;
                    pOutput->dataLeft = 0;

                    DEC_API_TRC
                        ("H264DecDecode# H264DEC_STRM_PROCESSED, give more data\n");
        	        pDecCont->processed++;
		
                    if(pDecCont->processed > 1 && pDecCont->processedinterval < 10)
		            {
			            DWLUpdateClk(pDecCont->dwl);
		            }

			        pDecCont->processedinterval = 0;
                    return H264DEC_STRM_PROCESSED;
                }
                /* Handle stream error dedected in HW */
                else if(asic_status & DEC_X170_IRQ_STREAM_ERROR)
                {
                    DEBUG_PRINT(("IRQ: STREAM ERROR dedected\n"));
                    if(pDecCont->packetDecoded != HANTRO_TRUE)
                    {
                        DEBUG_PRINT(("reset picStarted\n"));
                        pDecCont->storage.picStarted = HANTRO_FALSE;
                    }
                    {
                        //added by ayakashi for mosaic pic
                        u8 interlacedSequence = pStorage->activeSps->frameMbsOnlyFlag == 0 ? 1 : 0;
                        if(interlacedSequence && pStorage->dpb->maxRefFrames > 2)
                            return H264DEC_SYSTEM_ERROR;
                    }

                    {
                        strmData_t *strm = pDecCont->storage.strm;
                        const u8 *next =
                            h264bsdFindNextStartCode(strm->pStrmBuffStart,
                                                     strm->strmBuffSize);

                        if(next != NULL)
                        {
                            u32 consumed;

                            tmpStream -= numReadBytes;
                            strmLen += numReadBytes;

                            consumed = (u32) (next - tmpStream);
                            tmpStream += consumed;
                            strmLen -= consumed;
                        }
                    }

                    /* REMEMBER TO UPDATE(RESET) STREAM POSITIONS */
                    ASSERT(pDecCont->rlcMode == 0);
                    
                    pDecCont->storage.pictureBroken = HANTRO_TRUE;
                    h264InitPicFreezeOutput(pDecCont, 1);

                    /* PP has to run again for the concealed picture */
                    if(pDecCont->pp.ppInstance != NULL)
                    {
                       
                        TRACE_PP_CTRL
                            ("H264DecDecode: Concealed picture, PP should run again\n");
                        pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;

                        if(pDecCont->pp.ppInfo.multiBuffer != 0)
                        {
                            if(pDecCont->pp.decPpIf.usePipeline != 0)
                            {
                                /* remove pipelined pic from PP list */
                                h264PpMultiRemovePic(pDecCont, pStorage->currImage->data);
                            }

                            if(pDecCont->pp.queuedPicToPp[pStorage->view] ==
                               pStorage->currImage->data)
                            {
                                /* current picture cannot be in the queue */
                                pDecCont->pp.queuedPicToPp[pStorage->view] =
                                    NULL;
                            }
                        }
                    }

                    /* HW returned stream position is not valid in this case */
                    pDecCont->streamPosUpdated = 0;
                }
                else /* OK in here */
                {
                    if( IS_IDR_NAL_UNIT(pStorage->prevNalUnit) )
                    {
                        pDecCont->storage.pictureBroken = HANTRO_FALSE;
                    }
                }

                if( pDecCont->storage.activePps->entropyCodingModeFlag &&
                    (asic_status & DEC_X170_IRQ_STREAM_ERROR) == 0)
                {
                    u32 tmp;

                    strmData_t strmTmp = *pDecCont->storage.strm;
                    tmp = pDecCont->pHwStreamStart-pInput->pStream;
                    strmTmp.pStrmCurrPos = pDecCont->pHwStreamStart;
                    strmTmp.strmBuffReadBits = 8*tmp;
                    strmTmp.bitPosInWord = 0;
                    strmTmp.strmBuffSize = pInput->dataLen;
                    tmp = h264CheckCabacZeroWords( &strmTmp );
                    if( tmp != HANTRO_OK )
                    {
                        DEBUG_PRINT(("Error decoding CABAC zero words\n"));
                        {
                            strmData_t *strm = pDecCont->storage.strm;
                            const u8 *next =
                                h264bsdFindNextStartCode(strm->pStrmBuffStart,
                                                         strm->strmBuffSize);

                            if(next != NULL)
                            {
                                u32 consumed;

                                tmpStream -= numReadBytes;
                                strmLen += numReadBytes;

                                consumed = (u32) (next - tmpStream);
                                tmpStream += consumed;
                                strmLen -= consumed;
                            }
                        }

                        ASSERT(pDecCont->rlcMode == 0);
                    }
                }

                /* For the switch between modes */
                /* this is a sign for RLC mode + mb error conceal NOT to reset picStarted flag */
                pDecCont->packetDecoded = HANTRO_FALSE;

                DEBUG_PRINT(("Skip redundant VLC\n"));
                pStorage->skipRedundantSlices = HANTRO_TRUE;
                pDecCont->picNumber++;

#ifdef FFWD_WORKAROUND
                pStorage->prevIdrPicReady = 
                    IS_IDR_NAL_UNIT(pStorage->prevNalUnit);
#endif /* FFWD_WORKAROUND */

                returnValue = H264DEC_PIC_DECODED;
                strmLen = 0;
                break;
            }
        case H264BSD_PARAM_SET_ERROR:
            {
                if(!h264bsdCheckValidParamSets(&pDecCont->storage) &&
                   strmLen == 0)
                {
                    DEC_API_TRC
                        ("H264DecDecode# H264DEC_STRM_ERROR, Invalid parameter set(s)\n");
                    returnValue = H264DEC_STRM_ERROR;
                }

                /* update HW buffers if VLC mode */
                if(!pDecCont->rlcMode)
                {
                    pDecCont->hwLength -= numReadBytes;
                    pDecCont->hwStreamStartBus = pInput->streamBusAddress +
                        (u32) (tmpStream - pInput->pStream);

                    pDecCont->pHwStreamStart = tmpStream;
                }

                break;
            }
        case H264BSD_NEW_ACCESS_UNIT:
            {
                h264CheckReleasePpAndHw(pDecCont);

                pDecCont->streamPosUpdated = 0;

                pDecCont->storage.pictureBroken = HANTRO_TRUE;
                h264InitPicFreezeOutput(pDecCont, 0);

                h264UpdateAfterPictureDecode(pDecCont);

                /* PP will run in H264DecNextPicture() for this concealed picture */

                DEC_API_TRC("H264DecDecode# H264DEC_PIC_DECODED, NEW_ACCESS_UNIT\n");
                returnValue = H264DEC_PIC_DECODED;

                pDecCont->picNumber++;
                strmLen = 0;

                break;
            }
        case H264BSD_FMO:  /* If picture parameter set changed and FMO dedected */
            {
                DEBUG_PRINT(("FMO dedected\n"));

                ASSERT(pDecCont->rlcMode == 0);
                ASSERT(pDecCont->reallocate == 1);

                /* tmpStream = pInput->pStream; */

                DEC_API_TRC("H264DecDecode# H264DEC_ADVANCED_TOOLS, FMO\n");
                returnValue = H264DEC_ADVANCED_TOOLS;

                strmLen = 0;
                break;
            }
        case H264BSD_UNPAIRED_FIELD:
            {
                /* unpaired field detected and PP still running (wait after 
                 * second field decoded) -> wait here */
                h264CheckReleasePpAndHw(pDecCont);

                DEC_API_TRC("H264DecDecode# H264DEC_PIC_DECODED, UNPAIRED_FIELD\n");
                returnValue = H264DEC_PIC_DECODED;

                strmLen = 0;
                break;
            }
        case H264BSD_NONREF_PIC_SKIPPED:
			DWLUpdateClk(pDecCont->dwl);
            returnValue = H264DEC_NONREF_PIC_SKIPPED;
            /* fall through */
        default:   /* case H264BSD_ERROR, H264BSD_RDY */
            {
                pDecCont->hwLength -= numReadBytes;
                pDecCont->hwStreamStartBus = pInput->streamBusAddress +
                    (u32) (tmpStream - pInput->pStream);

                pDecCont->pHwStreamStart = tmpStream;
            }
        }
    }
    while(strmLen);

  end:

    /*  If Hw decodes stream, update stream buffers from "storage" */
    if(pDecCont->streamPosUpdated)
    {
        pOutput->pStrmCurrPos = (u8 *) pDecCont->pHwStreamStart;
        pOutput->strmCurrBusAddress = pDecCont->hwStreamStartBus;
        pOutput->dataLeft = pDecCont->hwLength;
    }
    else
    {
        /* else update based on SW stream decode stream values */
        u32 data_consumed = (u32) (tmpStream - pInput->pStream);

        pOutput->pStrmCurrPos = (u8 *) tmpStream;
        pOutput->strmCurrBusAddress = pInput->streamBusAddress + data_consumed;

        pOutput->dataLeft = pInput->dataLen - data_consumed;
    }

    if(returnValue == H264DEC_PIC_DECODED)
        pDecCont->gapsCheckedForThis = HANTRO_FALSE;
	if(returnValue == H264DEC_STRM_PROCESSED)
	{
		pDecCont->processed++;
		
		if(pDecCont->processed > 1 && pDecCont->processedinterval < 10)
		{
			DWLUpdateClk(pDecCont->dwl);
		}
		pDecCont->processedinterval = 0;
	}
    return (returnValue);
}

/*------------------------------------------------------------------------------
    Function name : H264DecGetAPIVersion
    Description   : Return the API version information

    Return type   : H264DecApiVersion
    Argument      : void
------------------------------------------------------------------------------*/
H264DecApiVersion H264DecGetAPIVersion(void)
{
    H264DecApiVersion ver;

    ver.major = H264DEC_MAJOR_VERSION;
    ver.minor = H264DEC_MINOR_VERSION;

    DEC_API_TRC("H264DecGetAPIVersion# OK\n");

    return ver;
}

/*------------------------------------------------------------------------------
    Function name : H264DecGetBuild
    Description   : Return the SW and HW build information

    Return type   : H264DecBuild
    Argument      : void
------------------------------------------------------------------------------*/
H264DecBuild H264DecGetBuild(void)
{
    H264DecBuild ver;
    DWLHwConfig_t hwCfg;

    (void)G1DWLmemset(&ver, 0, sizeof(ver));

    ver.swBuild = H264DEC_SW_BUILD;
    ver.hwBuild = G1DWLReadAsicID();

    DWLReadAsicConfig(&hwCfg);

    SET_DEC_BUILD_SUPPORT(ver.hwConfig, hwCfg);

    DEC_API_TRC("H264DecGetBuild# OK\n");

    return (ver);
}

/*------------------------------------------------------------------------------

    Function: H264DecNextPicture

        Functional description:
            Get next picture in display order if any available.

        Input:
            decInst     decoder instance.
            endOfStream force output of all buffered pictures

        Output:
            pOutput     pointer to output structure

        Returns:
            H264DEC_OK            no pictures available for display
            H264DEC_PIC_RDY       picture available for display
            H264DEC_PARAM_ERROR     invalid parameters

------------------------------------------------------------------------------*/
H264DecRet H264DecNextPicture(H264DecInst decInst,
                              H264DecPicture * pOutput, u32 endOfStream)
{
    decContainer_t *pDecCont = (decContainer_t *) decInst;
    const dpbOutPicture_t *outPic = NULL;
    dpbStorage_t *outDpb;

    DEC_API_TRC("H264DecNextPicture#\n");

    if(decInst == NULL || pOutput == NULL)
    {
        DEC_API_TRC("H264DecNextPicture# ERROR: decInst or pOutput is NULL\n");
        return (H264DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("H264DecNextPicture# ERROR: Decoder not initialized\n");
        return (H264DEC_NOT_INITIALIZED);
    }

    if(endOfStream)
    {
        if(pDecCont->asicRunning)
        {
            /* stop HW */
            SetDecRegister(pDecCont->h264Regs, HWIF_DEC_IRQ_STAT, 0);
            SetDecRegister(pDecCont->h264Regs, HWIF_DEC_IRQ, 0);
            SetDecRegister(pDecCont->h264Regs, HWIF_DEC_E, 0);
            DWLDisableHW(pDecCont->dwl, 4 * 1, pDecCont->h264Regs[1]);

            /* Wait for PP to end also */
            if(pDecCont->pp.ppInstance != NULL &&
               (pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING ||
                pDecCont->pp.decPpIf.ppStatus == DECPP_PIC_NOT_FINISHED))
            {
                pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;

                TRACE_PP_CTRL("H264RunAsic: PP Wait for end\n");

                pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);

                TRACE_PP_CTRL("H264RunAsic: PP Finished\n");
            }

            G1G1DWLReleaseHw(pDecCont->dwl);    /* release HW lock */

            pDecCont->asicRunning = 0;
            pDecCont->decStat = H264DEC_INITIALIZED;
            h264InitPicFreezeOutput(pDecCont, 1);
        }
        else if (pDecCont->keepHwReserved)
        {
            G1G1DWLReleaseHw(pDecCont->dwl);
            pDecCont->keepHwReserved = 0;
        }
        /* only one field of last frame decoded, PP still running (wait after 
         * second field decoded) -> wait here */
        if (pDecCont->pp.ppInstance != NULL &&
            pDecCont->pp.decPpIf.ppStatus == DECPP_PIC_NOT_FINISHED)
        {
            pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;
            pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);
        }

        h264bsdFlushBuffer(&pDecCont->storage);
    }

    /* pp and decoder running in parallel, decoder finished first field ->
     * decode second field and wait PP after that */
    if (pDecCont->pp.ppInstance != NULL &&
        pDecCont->pp.decPpIf.ppStatus == DECPP_PIC_NOT_FINISHED)
        return (H264DEC_OK);
        
    outDpb = pDecCont->storage.dpbs[pDecCont->storage.outView];

    /* if display order is the same as decoding order and PP is used and
     * cannot be used in pipeline (rotation) -> do not perform PP here but
     * while decoding next picture (parallel processing instead of
     * DEC followed by PP followed by DEC...) */
    if (pDecCont->storage.pendingOutPic)
    {
        outPic = pDecCont->storage.pendingOutPic;
        pDecCont->storage.pendingOutPic = NULL;
    }
    else if(outDpb->noReordering == 0)
    {
        if(!outDpb->delayedOut)
        {
            if (pDecCont->pp.ppInstance && pDecCont->pp.decPpIf.ppStatus ==
                DECPP_PIC_READY)
                outDpb->noOutput = 0;

            pDecCont->storage.dpb =
                pDecCont->storage.dpbs[pDecCont->storage.outView];

            outPic = h264bsdNextOutputPicture(&pDecCont->storage);

            if ( (pDecCont->storage.numViews ||
                  pDecCont->storage.outView) && outPic != NULL)
            {
                pOutput->viewId =
                    pDecCont->storage.viewId[pDecCont->storage.outView];
                pDecCont->storage.outView ^= 0x1;
            }
        }
    }
    else
    {
        if(outDpb->numOut > 1 || endOfStream ||
           pDecCont->storage.prevNalUnit->nalRefIdc == 0 ||
           pDecCont->pp.ppInstance == NULL || pDecCont->pp.decPpIf.usePipeline)
        {
            if((outDpb->numOut == 1 &&
                outDpb->delayedOut) ||
                (pDecCont->storage.sliceHeader->fieldPicFlag &&
                 pDecCont->storage.secondField))
            {
            }
            else
            {
                pDecCont->storage.dpb =
                    pDecCont->storage.dpbs[pDecCont->storage.outView];
                outPic = h264bsdNextOutputPicture(&pDecCont->storage);
                pOutput->viewId =
                    pDecCont->storage.viewId[pDecCont->storage.outView];
                if ( (pDecCont->storage.numViews ||
                      pDecCont->storage.outView) && outPic != NULL)
                    pDecCont->storage.outView ^= 0x1;
            }
        }
    }

    if(outPic != NULL)
    {
        u32 croppingFlag;

        if (!pDecCont->storage.numViews)
            pOutput->viewId = 0;

        pOutput->pOutputPicture = outPic->data->virtualAddress;
        pOutput->outputPictureBusAddress = outPic->data->busAddress;
        pOutput->picId = outPic->picId;
        pOutput->isIdrPicture = outPic->isIdr;
        pOutput->nbrOfErrMBs = outPic->numErrMbs;

        pOutput->interlaced = outPic->interlaced;
        pOutput->fieldPicture = outPic->fieldPicture;
        pOutput->topField = outPic->topField;

        pOutput->picWidth = h264bsdPicWidth(&pDecCont->storage) << 4;
        pOutput->picHeight = h264bsdPicHeight(&pDecCont->storage) << 4;
        pOutput->outputFormat = outPic->tiledMode ?
        DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

        h264bsdCroppingParams(&pDecCont->storage,
                              &croppingFlag,
                              &pOutput->cropParams.cropLeftOffset,
                              &pOutput->cropParams.cropOutWidth,
                              &pOutput->cropParams.cropTopOffset,
                              &pOutput->cropParams.cropOutHeight);

        if(croppingFlag == 0)
        {
            pOutput->cropParams.cropLeftOffset = 0;
            pOutput->cropParams.cropTopOffset = 0;
            pOutput->cropParams.cropOutWidth = pOutput->picWidth;
            pOutput->cropParams.cropOutHeight = pOutput->picHeight;
        }

        if(pDecCont->pp.ppInstance != NULL && (pDecCont->pp.ppInfo.multiBuffer == 0))
        {
            /* single buffer legacy mode */
            DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;

            if(decPpIf->ppStatus == DECPP_PIC_READY)
            {
                pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;
                TRACE_PP_CTRL
                    ("H264DecNextPicture: PP already ran for this picture\n");
            }
            else
            {
                TRACE_PP_CTRL("H264DecNextPicture: PP has to run\n");

                decPpIf->usePipeline = 0; /* we are in standalone mode? */

                decPpIf->inwidth = pOutput->picWidth;
                decPpIf->inheight = pOutput->picHeight;
                decPpIf->croppedW = pOutput->picWidth;
                decPpIf->croppedH = pOutput->picHeight;
                decPpIf->tiledInputMode = 
                    (pOutput->outputFormat == DEC_OUT_FRM_RASTER_SCAN) ? 0 : 1;
                decPpIf->progressiveSequence =
                    pDecCont->storage.activeSps->frameMbsOnlyFlag;

                if(pOutput->interlaced == 0)
                {
                    decPpIf->picStruct = DECPP_PIC_FRAME_OR_TOP_FIELD;
                }
                else
                {
                    if(pOutput->fieldPicture == 0)
                    {
                        decPpIf->picStruct = DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
                    }
                    else
                    {
                        /* TODO: missing field, is this OK? */
                        decPpIf->picStruct = DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
                    }
                }

                decPpIf->inputBusLuma = pOutput->outputPictureBusAddress;
                decPpIf->inputBusChroma = decPpIf->inputBusLuma +
                    pOutput->picWidth * pOutput->picHeight;

                if(decPpIf->picStruct != DECPP_PIC_FRAME_OR_TOP_FIELD)
                {
                    decPpIf->bottomBusLuma = decPpIf->inputBusLuma + decPpIf->inwidth;
                    decPpIf->bottomBusChroma = decPpIf->inputBusChroma + decPpIf->inwidth;
                }
                else
                {
                    decPpIf->bottomBusLuma = (u32)(-1);
                    decPpIf->bottomBusChroma = (u32)(-1);
                }

                decPpIf->littleEndian =
                    GetDecRegister(pDecCont->h264Regs, HWIF_DEC_OUT_ENDIAN);
                decPpIf->wordSwap =
                    GetDecRegister(pDecCont->h264Regs, HWIF_DEC_OUTSWAP32_E);

                pDecCont->pp.PPDecStart(pDecCont->pp.ppInstance, decPpIf);

                TRACE_PP_CTRL("H264DecNextPicture: PP wait to be done\n");

                pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);

                TRACE_PP_CTRL("H264DecNextPicture: PP Finished\n");

            }
        }
   
        if(pDecCont->pp.ppInstance != NULL && (pDecCont->pp.ppInfo.multiBuffer != 0))
        {
            /* multibuffer mode */
            DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;
            u32 id;

            if(decPpIf->ppStatus == DECPP_PIC_READY)
            {
                pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;
                TRACE_PP_CTRL("H264DecNextPicture: PP processed a picture\n");
            }

            id = h264PpMultiFindPic(pDecCont, outPic->data);
            if(id <= pDecCont->pp.multiMaxId)
            {
                TRACE_PP_CTRL("H264DecNextPicture: PPNextDisplayId = %d\n", id);
                TRACE_PP_CTRL("H264DecNextPicture: PP already ran for this picture\n");
                pDecCont->pp.PPNextDisplayId(pDecCont->pp.ppInstance, id);
                h264PpMultiRemovePic(pDecCont, outPic->data);
            }
            else
            {
                if (!endOfStream && outDpb->numFreeBuffers &&
                    pDecCont->pp.queuedPicToPp[pDecCont->storage.view] ==
                    outPic->data &&
                    pDecCont->decStat != H264DEC_NEW_HEADERS &&
                    !pDecCont->pp.ppInfo.pipelineAccepted)
                {
                	//add by franklin for rvds compile
                    pDecCont->storage.pendingOutPic = (dpbOutPicture_t *)outPic;
                    return H264DEC_OK;
                }
                TRACE_PP_CTRL("H264DecNextPicture: PP has to run\n");
                
                id = h264PpMultiAddPic(pDecCont, outPic->data);
                
                ASSERT(id <= pDecCont->pp.multiMaxId);

                TRACE_PP_CTRL("H264RunAsic: PP Multibuffer index = %d\n", id);
                TRACE_PP_CTRL("H264DecNextPicture: PPNextDisplayId = %d\n", id);
                decPpIf->bufferIndex = id;
                decPpIf->displayIndex = id;
                h264PpMultiRemovePic(pDecCont, outPic->data);

                if(pDecCont->pp.queuedPicToPp[pDecCont->storage.view] == outPic->data)
                {
                    pDecCont->pp.queuedPicToPp[pDecCont->storage.view] = NULL; /* remove it from queue */
                }

                decPpIf->usePipeline = 0;   /* we are in standalone mode */

                decPpIf->inwidth = pOutput->picWidth;
                decPpIf->inheight = pOutput->picHeight;
                decPpIf->croppedW = pOutput->picWidth;
                decPpIf->croppedH = pOutput->picHeight;
                decPpIf->tiledInputMode = 
                    (pOutput->outputFormat == DEC_OUT_FRM_RASTER_SCAN) ? 0 : 1;
                decPpIf->progressiveSequence =
                    pDecCont->storage.activeSps->frameMbsOnlyFlag;

                if(pOutput->interlaced == 0)
                {
                    decPpIf->picStruct = DECPP_PIC_FRAME_OR_TOP_FIELD;
                }
                else
                {
                    if(pOutput->fieldPicture == 0)
                    {
                        decPpIf->picStruct = DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
                    }
                    else
                    {
                        /* TODO: missing field, is this OK? */
                        decPpIf->picStruct = DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
                    }
                }

                decPpIf->inputBusLuma = pOutput->outputPictureBusAddress;
                decPpIf->inputBusChroma = decPpIf->inputBusLuma +
                    pOutput->picWidth * pOutput->picHeight;

                if(decPpIf->picStruct != DECPP_PIC_FRAME_OR_TOP_FIELD)
                {
                    decPpIf->bottomBusLuma = decPpIf->inputBusLuma + decPpIf->inwidth;
                    decPpIf->bottomBusChroma = decPpIf->inputBusChroma + decPpIf->inwidth;
                }
                else
                {
                    decPpIf->bottomBusLuma = (u32)(-1);
                    decPpIf->bottomBusChroma = (u32)(-1);
                }

                decPpIf->littleEndian =
                    GetDecRegister(pDecCont->h264Regs, HWIF_DEC_OUT_ENDIAN);
                decPpIf->wordSwap =
                    GetDecRegister(pDecCont->h264Regs, HWIF_DEC_OUTSWAP32_E);

                pDecCont->pp.PPDecStart(pDecCont->pp.ppInstance, decPpIf);

                TRACE_PP_CTRL("H264DecNextPicture: PP wait to be done\n");

                pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);

                TRACE_PP_CTRL("H264DecNextPicture: PP Finished\n");
            }
        }

        DEC_API_TRC("H264DecNextPicture# H264DEC_PIC_RDY\n");
        return (H264DEC_PIC_RDY);
    }
    else
    {
        DEC_API_TRC("H264DecNextPicture# H264DEC_OK\n");
        return (H264DEC_OK);
    }

}

/*------------------------------------------------------------------------------
    Function name : h264UpdateAfterPictureDecode
    Description   :

    Return type   : void
    Argument      : decContainer_t * pDecCont
------------------------------------------------------------------------------*/
void h264UpdateAfterPictureDecode(decContainer_t * pDecCont)
{

    u32 tmpRet = HANTRO_OK;
    storage_t *pStorage = &pDecCont->storage;
    sliceHeader_t *sliceHeader = pStorage->sliceHeader;

    h264bsdResetStorage(pStorage);

    ASSERT((pStorage));

    /* determine initial reference picture lists */
    H264InitRefPicList(pDecCont);

    if(pStorage->sliceHeader->fieldPicFlag == 0)
        pStorage->currImage->picStruct = FRAME;
    else
        pStorage->currImage->picStruct = pStorage->sliceHeader->bottomFieldFlag;

    if(pStorage->poc->containsMmco5)
    {
        u32 tmp;

        tmp = MIN(pStorage->poc->picOrderCnt[0], pStorage->poc->picOrderCnt[1]);
        pStorage->poc->picOrderCnt[0] -= tmp;
        pStorage->poc->picOrderCnt[1] -= tmp;
    }

    pStorage->currentMarked = pStorage->validSliceInAccessUnit;

    /* Setup tiled mode for picture before updating DPB */
    if( pDecCont->tiledModeSupport)
    {
        u32 interlaced;
        interlaced = !pDecCont->storage.activeSps->frameMbsOnlyFlag;
        pDecCont->tiledReferenceEnable = 
            DecSetupTiledReference( pDecCont->h264Regs, 
                pDecCont->tiledModeSupport,
                interlaced );
    }
    else
    {
        pDecCont->tiledReferenceEnable = 0;
    }

    if(pStorage->validSliceInAccessUnit)
    {
        if(pStorage->prevNalUnit->nalRefIdc)
        {
            tmpRet = h264bsdMarkDecRefPic(pStorage->dpb,
                                        &sliceHeader->decRefPicMarking,
                                        pStorage->currImage,
                                        sliceHeader->frameNum,
                                        pStorage->poc->picOrderCnt,
                                        IS_IDR_NAL_UNIT(pStorage->prevNalUnit) ?
                                        HANTRO_TRUE : HANTRO_FALSE,
                                        pStorage->currentPicId,
                                        pStorage->numConcealedMbs,
                                        pDecCont->tiledReferenceEnable);
        }
        else
        {
            /* non-reference picture, just store for possible display
             * reordering */
            tmpRet = h264bsdMarkDecRefPic(pStorage->dpb, NULL,
                                        pStorage->currImage,
                                        sliceHeader->frameNum,
                                        pStorage->poc->picOrderCnt,
                                        HANTRO_FALSE,
                                        pStorage->currentPicId,
                                        pStorage->numConcealedMbs,
                                        pDecCont->tiledReferenceEnable);
        }

        if (tmpRet != HANTRO_OK && pStorage->view == 0)
            pStorage->secondField = 0;

        if(pStorage->dpb->delayedOut == 0)
        {
            h264DpbUpdateOutputList(pStorage->dpb, pStorage->currImage);

            if (pStorage->view == 0)
                pStorage->lastBaseNumOut = pStorage->dpb->numOut;
            else if (pStorage->dpb->numOut != pStorage->lastBaseNumOut)
                h264DpbAdjStereoOutput(pStorage->dpb, pStorage->lastBaseNumOut);

            /* check if currentOut already in output buffer and second
             * field to come -> delay output */
            if(pStorage->currImage->picStruct != FRAME &&
               (pStorage->view == 0 ? pStorage->secondField :
                                      !pStorage->baseOppositeFieldPic))
            {
                u32 i, tmp;

                tmp = pStorage->dpb->outIndexR;
                for (i = 0; i < pStorage->dpb->numOut; i++, tmp++)
                {
                    if (tmp == pStorage->dpb->dpbSize + 1)
                        tmp = 0;

                    if(pStorage->dpb->currentOut->data ==
                       pStorage->dpb->outBuf[tmp].data)
                    {
                        pStorage->dpb->delayedId = tmp;
                        DEBUG_PRINT(
                            ("h264UpdateAfterPictureDecode: Current frame in output list; "));
                        pStorage->dpb->delayedOut = 1;
                        break;
                    }
                }
            }
        }
        else
        {
            if (!pStorage->dpb->noReordering)
                h264DpbUpdateOutputList(pStorage->dpb, pStorage->currImage);
            DEBUG_PRINT(
                ("h264UpdateAfterPictureDecode: Output all delayed pictures!\n"));
            pStorage->dpb->delayedOut = 0;
            pStorage->dpb->currentOut->toBeDisplayed = 0;   /* remove it from output list */
        }

    }
    else
    {
        pStorage->dpb->delayedOut = 0;
        pStorage->secondField = 0;
    }

    if ((pStorage->validSliceInAccessUnit && tmpRet == HANTRO_OK) ||
        pStorage->view)
        pStorage->nextView ^= 0x1;
    pStorage->picStarted = HANTRO_FALSE;
    pStorage->validSliceInAccessUnit = HANTRO_FALSE;
    pStorage->asoDetected = 0;
}

/*------------------------------------------------------------------------------
    Function name : h264SpsSupported
    Description   :

    Return type   : u32
    Argument      : const decContainer_t * pDecCont
------------------------------------------------------------------------------*/
u32 h264SpsSupported(const decContainer_t * pDecCont)
{
    const seqParamSet_t *sps = pDecCont->storage.activeSps;

    /* check picture size */
    if(sps->picWidthInMbs * 16 > pDecCont->maxDecPicWidth ||
       sps->picWidthInMbs < 3 || sps->picHeightInMbs < 3 ||
       (sps->picWidthInMbs * sps->picHeightInMbs) > ((1920 >> 4) * (1088 >> 4)))
    {
        DEBUG_PRINT(("Picture size not supported!\n"));
        return 0;
    }

    if(pDecCont->h264ProfileSupport == H264_BASELINE_PROFILE)
    {
        if(sps->frameMbsOnlyFlag != 1)
        {
            DEBUG_PRINT(("INTERLACED!!! Not supported in baseline decoder\n"));
            return 0;
        }
        if(sps->chromaFormatIdc != 1)
        {
            DEBUG_PRINT(("CHROMA!!! Only 4:2:0 supported in baseline decoder\n"));
            return 0;
        }
        if(sps->scalingMatrixPresentFlag != 0)
        {
            DEBUG_PRINT(("SCALING Matrix!!! Not supported in baseline decoder\n"));
            return 0;
        }
    }

    return 1;
}

/*------------------------------------------------------------------------------
    Function name : h264PpsSupported
    Description   :

    Return type   : u32
    Argument      : const decContainer_t * pDecCont
------------------------------------------------------------------------------*/
u32 h264PpsSupported(const decContainer_t * pDecCont)
{
    const picParamSet_t *pps = pDecCont->storage.activePps;

    if(pDecCont->h264ProfileSupport == H264_BASELINE_PROFILE)
    {
        if(pps->entropyCodingModeFlag != 0)
        {
            DEBUG_PRINT(("CABAC!!! Not supported in baseline decoder\n"));
            return 0;
        }
        if(pps->weightedPredFlag != 0 || pps->weightedBiPredIdc != 0)
        {
            DEBUG_PRINT(("WEIGHTED Pred!!! Not supported in baseline decoder\n"));
            return 0;
        }
        if(pps->transform8x8Flag != 0)
        {
            DEBUG_PRINT(("TRANSFORM 8x8!!! Not supported in baseline decoder\n"));
            return 0;
        }
        if(pps->scalingMatrixPresentFlag != 0)
        {
            DEBUG_PRINT(("SCALING Matrix!!! Not supported in baseline decoder\n"));
            return 0;
        }
    }
    return 1;
}

/*------------------------------------------------------------------------------
    Function name   : h264StreamIsBaseline
    Description     : 
    Return type     : u32 
    Argument        : const decContainer_t * pDecCont
------------------------------------------------------------------------------*/
u32 h264StreamIsBaseline(const decContainer_t * pDecCont)
{
    const picParamSet_t *pps = pDecCont->storage.activePps;
    const seqParamSet_t *sps = pDecCont->storage.activeSps;

    if(sps->frameMbsOnlyFlag != 1)
    {
        return 0;
    }
    if(sps->chromaFormatIdc != 1)
    {
        return 0;
    }
    if(sps->scalingMatrixPresentFlag != 0)
    {
        return 0;
    }
    if(pps->entropyCodingModeFlag != 0)
    {
        return 0;
    }
    if(pps->weightedPredFlag != 0 || pps->weightedBiPredIdc != 0)
    {
        return 0;
    }
    if(pps->transform8x8Flag != 0)
    {
        return 0;
    }
    if(pps->scalingMatrixPresentFlag != 0)
    {
        return 0;
    }
    return 1;
}

/*------------------------------------------------------------------------------
    Function name : h264AllocateResources
    Description   :

    Return type   : u32
    Argument      : decContainer_t * pDecCont
------------------------------------------------------------------------------*/
u32 h264AllocateResources(decContainer_t * pDecCont)
{
    u32 ret, mbs_in_pic;
    DecAsicBuffers_t *asic = pDecCont->asicBuff;
    storage_t *pStorage = &pDecCont->storage;

    const seqParamSet_t *sps = pStorage->activeSps;

    SetDecRegister(pDecCont->h264Regs, HWIF_PIC_MB_WIDTH, sps->picWidthInMbs);
    SetDecRegister(pDecCont->h264Regs, HWIF_PIC_MB_HEIGHT_P,
                   sps->picHeightInMbs);

    ReleaseAsicBuffers(pDecCont->dwl, asic);

    ret = AllocateAsicBuffers(pDecCont, asic, pStorage->picSizeInMbs);
    if(ret == 0)
    {
        SetDecRegister(pDecCont->h264Regs, HWIF_INTRA_4X4_BASE,
                       asic->intraPred.busAddress);
        SetDecRegister(pDecCont->h264Regs, HWIF_DIFF_MV_BASE,
                       asic->mv.busAddress);
        if(pDecCont->h264ProfileSupport != H264_BASELINE_PROFILE)
        {
            SetDecRegister(pDecCont->h264Regs, HWIF_QTABLE_BASE,
                           asic->cabacInit.busAddress);
        }

        if(pDecCont->rlcMode)
        {
            /* release any previously allocated stuff */
            FREE(pStorage->mb);
            FREE(pStorage->sliceGroupMap);

            mbs_in_pic = sps->picWidthInMbs * sps->picHeightInMbs;

            DEBUG_PRINT(("ALLOCATE pStorage->mb            - %8d bytes\n",
                         mbs_in_pic * sizeof(mbStorage_t)));
            pStorage->mb = G1DWLcalloc(mbs_in_pic, sizeof(mbStorage_t));

            DEBUG_PRINT(("ALLOCATE pStorage->sliceGroupMap - %8d bytes\n",
                         mbs_in_pic * sizeof(u32)));

            ALLOCATE(pStorage->sliceGroupMap, mbs_in_pic, u32);

            if(pStorage->mb == NULL || pStorage->sliceGroupMap == NULL)
            {
                ret = MEMORY_ALLOCATION_ERROR;
            }
            else
            {
                h264bsdInitMbNeighbours(pStorage->mb, sps->picWidthInMbs,
                                        pStorage->picSizeInMbs);
            }
        }
        else
        {
            pStorage->mb = NULL;
            pStorage->sliceGroupMap = NULL;
        }
    }

    return ret;
}

/*------------------------------------------------------------------------------
    Function name : h264InitPicFreezeOutput
    Description   :

    Return type   : u32
    Argument      : decContainer_t * pDecCont
------------------------------------------------------------------------------*/
void h264InitPicFreezeOutput(decContainer_t * pDecCont, u32 fromOldDpb)
{

    u32 index = 0;
    const u8 *refData;
    storage_t *storage = &pDecCont->storage;

#ifdef _DISABLE_PIC_FREEZE
    return;
#endif

    /* for concealment after a HW error report we use the saved reference list */
    dpbStorage_t *dpb = &storage->dpb[fromOldDpb];

    do
    {
        refData = h264bsdGetRefPicDataVlcMode(dpb, dpb->list[index], 0);
        index++;
    }
    while(index < 16 && refData == NULL);

    ASSERT(dpb->currentOut->data != NULL);

    /* "freeze" whole picture if not field pic or if opposite field of the
     * field pic does not exist in the buffer */
    if(pDecCont->storage.sliceHeader->fieldPicFlag == 0 ||
       storage->dpb[0].currentOut->status[
        !pDecCont->storage.sliceHeader->bottomFieldFlag] == EMPTY)
    {
        /* reset DMV storage for erroneous pictures */
        if(pDecCont->asicBuff->enableDmvAndPoc != 0)
        {
            const u32 dvm_mem_size = storage->picSizeInMbs * 64;
            void * dvm_base = (u8*)storage->currImage->data->virtualAddress + 
                                pDecCont->storage.dpb->dirMvOffset;

            (void) G1DWLmemset(dvm_base, 0, dvm_mem_size);    
        }
        
        if(refData == NULL)
        {
            DEBUG_PRINT(("h264InitPicFreezeOutput: pic freeze memset\n"));
            (void) G1DWLmemset(storage->currImage->data->
                             virtualAddress, 128,
                             pDecCont->storage.activeSps->monoChrome ?
                             256 * storage->picSizeInMbs :
                             384 * storage->picSizeInMbs);
            if (storage->enable2ndChroma &&
                !storage->activeSps->monoChrome)
                (void) G1DWLmemset((u8*)storage->currImage->data->virtualAddress +
                                 dpb->ch2Offset, 128,
                                 128 * storage->picSizeInMbs);
        }
        else
        {
            DEBUG_PRINT(("h264InitPicFreezeOutput: pic freeze memcopy\n"));
            (void) G1DWLmemcpy(storage->currImage->data->virtualAddress,
                             refData,
                             pDecCont->storage.activeSps->monoChrome ?
                             256 * storage->picSizeInMbs :
                             384 * storage->picSizeInMbs);
            if (storage->enable2ndChroma &&
                !storage->activeSps->monoChrome)
                (void) G1DWLmemcpy((u8*)storage->currImage->data->virtualAddress +
                                 dpb->ch2Offset, refData + dpb->ch2Offset,
                                 128 * storage->picSizeInMbs);
        }
    }
    else
    {
        u32 i;
        u32 fieldOffset = storage->activeSps->picWidthInMbs * 16;
        u8 *lumBase = (u8*)storage->currImage->data->virtualAddress;
        u8 *chBase = (u8*)storage->currImage->data->virtualAddress + storage->picSizeInMbs * 256;
        u8 *ch2Base = (u8*)storage->currImage->data->virtualAddress + dpb->ch2Offset;
        const u8 *refChData = refData + storage->picSizeInMbs * 256;
        const u8 *refCh2Data = refData + dpb->ch2Offset;

        if(pDecCont->storage.sliceHeader->bottomFieldFlag != 0)
        {
            lumBase += fieldOffset;
            chBase += fieldOffset;
            ch2Base += fieldOffset;
        }

        if(refData == NULL)
        {
            DEBUG_PRINT(("h264InitPicFreezeOutput: pic freeze memset, one field\n"));
            
            for(i = 0; i < (storage->activeSps->picHeightInMbs*8); i++)
            {
                (void) G1DWLmemset(lumBase, 128, fieldOffset);
                if((pDecCont->storage.activeSps->monoChrome == 0) && (i & 0x1U))
                {
                    (void) G1DWLmemset(chBase, 128, fieldOffset);
                    chBase += 2*fieldOffset;
                    if (storage->enable2ndChroma)
                    {
                        (void) G1DWLmemset(ch2Base, 128, fieldOffset);
                        ch2Base += 2*fieldOffset;
                    }
                }
                lumBase += 2*fieldOffset;
            }
        }
        else
        {
            if(pDecCont->storage.sliceHeader->bottomFieldFlag != 0)
            {
                refData += fieldOffset;
                refChData += fieldOffset;
                refCh2Data += fieldOffset;
            }

            DEBUG_PRINT(("h264InitPicFreezeOutput: pic freeze memcopy, one field\n"));
            for(i = 0; i < (storage->activeSps->picHeightInMbs*8); i++)
            {
                (void) G1DWLmemcpy(lumBase, refData, fieldOffset);
                if((pDecCont->storage.activeSps->monoChrome == 0) && (i & 0x1U))
                {
                    (void) G1DWLmemcpy(chBase, refChData, fieldOffset);
                    chBase += 2*fieldOffset;
                    refChData += 2*fieldOffset;
                    if (storage->enable2ndChroma)
                    {
                        (void) G1DWLmemcpy(ch2Base, refCh2Data, fieldOffset);
                        ch2Base += 2*fieldOffset;
                        refCh2Data += 2*fieldOffset;
                    }
                }
                lumBase += 2*fieldOffset;
                refData += 2*fieldOffset;
            }
        }
    }

    dpb = &storage->dpb[0]; /* update results for current output */

    {
        i32 i = dpb->numOut;
        u32 tmp = dpb->outIndexR;

        while((i--) > 0)
        {
            if (tmp == dpb->dpbSize + 1)
                tmp = 0;

            if(dpb->outBuf[tmp].data == storage->currImage->data)
            {
                dpb->outBuf[tmp].numErrMbs = storage->picSizeInMbs;
                break;
            }
            tmp++;
        }

        i = dpb->dpbSize + 1;

        while((i--) > 0)
        {
            if(dpb->buffer[i].data == storage->currImage->data)
            {
                dpb->buffer[i].numErrMbs = storage->picSizeInMbs;
                ASSERT(&dpb->buffer[i] == dpb->currentOut);
                break;
            }
        }
    }

    pDecCont->storage.numConcealedMbs = storage->picSizeInMbs;

}

/*------------------------------------------------------------------------------
    Function name : bsdDecodeReturn
    Description   :

    Return type   : void
    Argument      : bsd decoder return value
------------------------------------------------------------------------------*/
static void bsdDecodeReturn(u32 retval)
{

    DEBUG_PRINT(("H264bsdDecode returned: "));
    switch (retval)
    {
    case H264BSD_PIC_RDY:
        DEBUG_PRINT(("H264BSD_PIC_RDY\n"));
        break;
    case H264BSD_RDY:
        DEBUG_PRINT(("H264BSD_RDY\n"));
        break;
    case H264BSD_HDRS_RDY:
        DEBUG_PRINT(("H264BSD_HDRS_RDY\n"));
        break;
    case H264BSD_ERROR:
        DEBUG_PRINT(("H264BSD_ERROR\n"));
        break;
    case H264BSD_PARAM_SET_ERROR:
        DEBUG_PRINT(("H264BSD_PARAM_SET_ERROR\n"));
        break;
    case H264BSD_NEW_ACCESS_UNIT:
        DEBUG_PRINT(("H264BSD_NEW_ACCESS_UNIT\n"));
        break;
    case H264BSD_FMO:
        DEBUG_PRINT(("H264BSD_FMO\n"));
        break;
    default:
        DEBUG_PRINT(("UNKNOWN\n"));
        break;
    }
}

/*------------------------------------------------------------------------------
    Function name   : h264GetSarInfo
    Description     : Returns the sample aspect ratio size info
    Return type     : void
    Argument        : storage_t *pStorage - decoder storage
    Argument        : u32 * sar_width - SAR width returned here
    Argument        : u32 *sar_height - SAR height returned here
------------------------------------------------------------------------------*/
void h264GetSarInfo(const storage_t * pStorage, u32 * sar_width,
                    u32 * sar_height)
{
    switch (h264bsdAspectRatioIdc(pStorage))
    {
    case 0:
        *sar_width = 0;
        *sar_height = 0;
        break;
    case 1:
        *sar_width = 1;
        *sar_height = 1;
        break;
    case 2:
        *sar_width = 12;
        *sar_height = 11;
        break;
    case 3:
        *sar_width = 10;
        *sar_height = 11;
        break;
    case 4:
        *sar_width = 16;
        *sar_height = 11;
        break;
    case 5:
        *sar_width = 40;
        *sar_height = 33;
        break;
    case 6:
        *sar_width = 24;
        *sar_height = 1;
        break;
    case 7:
        *sar_width = 20;
        *sar_height = 11;
        break;
    case 8:
        *sar_width = 32;
        *sar_height = 11;
        break;
    case 9:
        *sar_width = 80;
        *sar_height = 33;
        break;
    case 10:
        *sar_width = 18;
        *sar_height = 11;
        break;
    case 11:
        *sar_width = 15;
        *sar_height = 11;
        break;
    case 12:
        *sar_width = 64;
        *sar_height = 33;
        break;
    case 13:
        *sar_width = 160;
        *sar_height = 99;
        break;
    case 255:
        h264bsdSarSize(pStorage, sar_width, sar_height);
        break;
    default:
        *sar_width = 0;
        *sar_height = 0;
    }
}

/*------------------------------------------------------------------------------
    Function name   : h264CheckReleasePpAndHw
    Description     : Release HW lock and wait for PP to finish, need to be
                      called if errors/problems after first field of a picture
                      finished and PP left running
    Return type     : void
    Argument        : 
    Argument        : 
    Argument        :
------------------------------------------------------------------------------*/
void h264CheckReleasePpAndHw(decContainer_t *pDecCont)
{

    if(pDecCont->pp.ppInstance != NULL &&
       (pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING ||
        pDecCont->pp.decPpIf.ppStatus == DECPP_PIC_NOT_FINISHED))
    {
        pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;
        pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);
    }

    if (pDecCont->keepHwReserved)
    {
        pDecCont->keepHwReserved = 0;
        G1G1DWLReleaseHw(pDecCont->dwl);
    }

}

/*------------------------------------------------------------------------------

    Function: H264DecPeek

        Functional description:
            Get last decoded picture if any available. No pictures are removed
            from output nor DPB buffers.

        Input:
            decInst     decoder instance.

        Output:
            pOutput     pointer to output structure

        Returns:
            H264DEC_OK            no pictures available for display
            H264DEC_PIC_RDY       picture available for display
            H264DEC_PARAM_ERROR   invalid parameters

------------------------------------------------------------------------------*/
H264DecRet H264DecPeek(H264DecInst decInst, H264DecPicture * pOutput)
{
    decContainer_t *pDecCont = (decContainer_t *) decInst;
    dpbPicture_t *currentOut = pDecCont->storage.dpb->currentOut;

    DEC_API_TRC("H264DecPeek#\n");

    if(decInst == NULL || pOutput == NULL)
    {
        DEC_API_TRC("H264DecPeek# ERROR: decInst or pOutput is NULL\n");
        return (H264DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("H264DecPeek# ERROR: Decoder not initialized\n");
        return (H264DEC_NOT_INITIALIZED);
    }

    if (pDecCont->decStat != H264DEC_NEW_HEADERS &&
        pDecCont->storage.dpb->fullness && currentOut != NULL &&
        (currentOut->status[0] != EMPTY || currentOut->status[1] != EMPTY))
    {

        u32 croppingFlag;

        pOutput->pOutputPicture = currentOut->data->virtualAddress;
        pOutput->outputPictureBusAddress = currentOut->data->busAddress;
        pOutput->picId = currentOut->picId;
        pOutput->isIdrPicture = currentOut->isIdr;
        pOutput->nbrOfErrMBs = currentOut->numErrMbs;

        pOutput->interlaced = pDecCont->storage.dpb->interlaced;
        pOutput->fieldPicture = currentOut->isFieldPic;
        pOutput->outputFormat = currentOut->tiledMode ?
        DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

        if (pOutput->fieldPicture)
        {
            /* just one field in buffer -> that is the output */
            if(currentOut->status[0] == EMPTY || currentOut->status[1] == EMPTY)
            {
                pOutput->topField = (currentOut->status[0] == EMPTY) ? 0 : 1;
            }
            /* both fields decoded -> check field parity from slice header */
            else
                pOutput->topField =
                    pDecCont->storage.sliceHeader->bottomFieldFlag == 0;
        }
        else
            pOutput->topField = 1;

        pOutput->picWidth = h264bsdPicWidth(&pDecCont->storage) << 4;
        pOutput->picHeight = h264bsdPicHeight(&pDecCont->storage) << 4;

        h264bsdCroppingParams(&pDecCont->storage,
                              &croppingFlag,
                              &pOutput->cropParams.cropLeftOffset,
                              &pOutput->cropParams.cropOutWidth,
                              &pOutput->cropParams.cropTopOffset,
                              &pOutput->cropParams.cropOutHeight);

        if(croppingFlag == 0)
        {
            pOutput->cropParams.cropLeftOffset = 0;
            pOutput->cropParams.cropTopOffset = 0;
            pOutput->cropParams.cropOutWidth = pOutput->picWidth;
            pOutput->cropParams.cropOutHeight = pOutput->picHeight;
        }


        DEC_API_TRC("H264DecPeek# H264DEC_PIC_RDY\n");
        return (H264DEC_PIC_RDY);
    }
    else
    {
        DEC_API_TRC("H264DecPeek# H264DEC_OK\n");
        return (H264DEC_OK);
    }

}

/*------------------------------------------------------------------------------

    Function: H264DecSetMvc()

        Functional description:
            This function configures decoder to decode both views of MVC
            stereo high profile compliant streams.

        Inputs:
            decInst     decoder instance

        Outputs:

        Returns:
            H264DEC_OK            success
            H264DEC_PARAM_ERROR   invalid parameters
            H264DEC_NOT_INITIALIZED   decoder instance not initialized yet

------------------------------------------------------------------------------*/
H264DecRet H264DecSetMvc(H264DecInst decInst)
{
    decContainer_t *pDecCont = (decContainer_t *) decInst;
    DWLHwConfig_t hwCfg;

    DEC_API_TRC("H264DecSetMvc#");

    if(decInst == NULL)
    {
        DEC_API_TRC("H264DecSetMvc# ERROR: decInst is NULL\n");
        return (H264DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        DEC_API_TRC("H264DecSetMvc# ERROR: Decoder not initialized\n");
        return (H264DEC_NOT_INITIALIZED);
    }

    (void) G1DWLmemset(&hwCfg, 0, sizeof(DWLHwConfig_t));
    DWLReadAsicConfig(&hwCfg);
    if(!hwCfg.mvcSupport)
    {
        DEC_API_TRC("H264DecSetMvc# ERROR: H264 MVC not supported in HW\n");
        return H264DEC_FORMAT_NOT_SUPPORTED;
    }

    pDecCont->storage.mvc = HANTRO_TRUE;

    DEC_API_TRC("H264DecSetMvc# OK\n");

    return (H264DEC_OK);
}

/*------------------------------------------------------------------------------

    Function name: H264DecSetLatency

    Functional description:
        set current video latencyMS to vdec driver for vpu dvfs

    Input:
        decInst     Reference to decoder instance.
        latencyMS   


    Return values:
        H264DEC_OK;

------------------------------------------------------------------------------*/
H264DecRet H264DecSetLatency(H264DecInst decInst, int latencyMS)
{
    decContainer_t *pDecCont = (decContainer_t *) decInst;

    DWLSetLatency(pDecCont->dwl, latencyMS);

    return H264DEC_OK;
}
