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
--  Description : Application Programming Interface (API) functions
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1decapi.c,v $
--  $Date: 2011/02/04 12:41:52 $
--  $Revision: 1.272 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1decapi.h"

#include "vc1hwd_container.h"
#include "vc1hwd_decoder.h"
#include "vc1hwd_util.h"
#include "vc1hwd_pp_pipeline.h"
#include "vc1hwd_regdrv.h"
#include "vc1hwd_asic.h"
#include "dwl.h"
#include "deccfg.h"
#include "refbuffer.h"
#include "bqueue.h"
#include "tiledref.h"

/*------------------------------------------------------------------------------
    Version Information
------------------------------------------------------------------------------*/

#define VC1DEC_MAJOR_VERSION 1
#define VC1DEC_MINOR_VERSION 2

#define VC1DEC_BUILD_MAJOR 1
#define VC1DEC_BUILD_MINOR 194
#define VC1DEC_SW_BUILD ((VC1DEC_BUILD_MAJOR * 1000) + VC1DEC_BUILD_MINOR)

/*------------------------------------------------------------------------------
    External compiler flags
--------------------------------------------------------------------------------

_VC1DEC_TRACE         Trace VC1 Decoder API function calls.

--------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#ifdef _VC1DEC_TRACE
#define DEC_API_TRC(str)    VC1DecTrace(str)
#else
#define DEC_API_TRC(str)
#endif

static void WriteBitPlaneCtrl(u32 *bitPlaneCtrl, u8 *mbFlags, u32 numMbs);
static u32 Vc1CheckFormatSupport(void);

/*------------------------------------------------------------------------------

    Function name: VC1DecInit

        Functional description:
            Initializes decoder software. Function reserves memory for the
            instance and calls vc1hwdInit to initialize the instance data.

        Inputs:
            pDecInst    Pointer to decoder instance.
            pMetaData   Pointer to stream metadata container.
            useVideoFreezeConcealment
                        flag to enable error concealment method where
                        decoding starts at next intra picture after error
                        in bitstream.

        Return values:
            VC1DEC_OK           Initialization is successful.
            VC1DEC_PARAM_ERROR  Error with function parameters.
            VC1DEC_MEMFAIL      Memory allocation failed.
            VC1DEC_INITFAIL     Initialization failed.
            VC1DEC_DWL_ERROR    Error initializing the system interface

------------------------------------------------------------------------------*/
VC1DecRet VC1DecInit( VC1DecInst* pDecInst, const VC1DecMetaData* pMetaData,
                      u32 useVideoFreezeConcealment,
                      u32 numFrameBuffers,
                      DecRefFrmFormat referenceFrameFormat, 
                      u32 mmuEnable)
{

    u32 i;
    u32 rv;
    decContainer_t *pDecCont;
    const void *dwl;

    G1DWLInitParam_t dwlInit;
    DWLHwConfig_t config;

    DEC_API_TRC("VC1DecInit#");

    /* check that right shift on negative numbers is performed signed */
    /*lint -save -e* following check causes multiple lint messages */
    if ( ((-1)>>1) != (-1) )
    {
        DEC_API_TRC("VC1DecInit# ERROR: Right shift is not signed");
        return(VC1DEC_INITFAIL);
    }
    /*lint -restore */

    if (pDecInst == NULL || pMetaData == NULL)
    {
        DEC_API_TRC("VC1DecInit# ERROR: NULL argument");
        return(VC1DEC_PARAM_ERROR);
    }

    *pDecInst = NULL; /* return NULL instance for any error */

	/* check that VC-1 decoding supported in HW */
    if(Vc1CheckFormatSupport())
    {
        DEC_API_TRC("VC1DecInit# ERROR: VC-1 not supported in HW\n");
        return VC1DEC_FORMAT_NOT_SUPPORTED;
    }

    dwlInit.clientType = DWL_CLIENT_TYPE_VC1_DEC;
    dwlInit.mmuEnable = mmuEnable;

    dwl = G1DWLInit(&dwlInit);
    if (dwl == NULL)
    {
        DEC_API_TRC("VC1DecInit# ERROR: DWL Init failed");
        return(VC1DEC_DWL_ERROR);
    }

    /* create decoder container */
    pDecCont = (decContainer_t *)G1DWLmalloc(sizeof(decContainer_t));
    if (pDecCont == NULL)
    {
        DEC_API_TRC("VC1DecInit# ERROR: Memory allocation failed");

        (void)G1DWLRelease(dwl);

        return(VC1DEC_MEMFAIL);
    }

    (void) G1DWLmemset(pDecCont, 0, sizeof(decContainer_t));
    pDecCont->dwl = dwl;

    /* Initialize decoder */
    rv = vc1hwdInit(dwl, &pDecCont->storage, pMetaData,
                    numFrameBuffers);
    if ( rv != VC1HWD_OK)
    {
        DEC_API_TRC("VC1DecInit# ERROR: Invalid initialization metadata");
        G1DWLfree(pDecCont);
        (void)G1DWLRelease(dwl);
        return (VC1DEC_PARAM_ERROR);
    }

    pDecCont->decStat  = VC1DEC_INITIALIZED;
    pDecCont->picNumber = 0;
    pDecCont->asicRunning = 0;

    for (i = 0; i < DEC_X170_REGISTERS; i++)
        pDecCont->vc1Regs[i] = 0;

#if( DEC_X170_HW_TIMEOUT_INT_ENA  != 0)
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_TIMEOUT_E, 1);
#else
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_TIMEOUT_E, 0);
#endif

    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_STRSWAP32_E,
        DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_STRENDIAN_E,
        DEC_X170_INPUT_STREAM_ENDIAN);
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_INSWAP32_E,
        DEC_X170_INPUT_DATA_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUTSWAP32_E,
        DEC_X170_OUTPUT_SWAP_32_ENABLE);
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_DATA_DISC_E,
        DEC_X170_DATA_DISCARD_ENABLE);
    /*
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_TILED_E,
        DEC_X170_OUTPUT_FORMAT);
        */
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_LATENCY,
        DEC_X170_LATENCY_COMPENSATION);

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_CLK_GATE_E, 1);
#else
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_CLK_GATE_E, 0);
#endif

    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IN_ENDIAN,
        DEC_X170_INPUT_DATA_ENDIAN);
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_ENDIAN,
        DEC_X170_OUTPUT_PICTURE_ENDIAN);

    if ((G1DWLReadAsicID() >> 16) == 0x8170U)
    {
        SetDecRegister(pDecCont->vc1Regs, HWIF_PRIORITY_MODE,
                       DEC_X170_ASIC_SERVICE_PRIORITY);
    }
    else
    {
        SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_SCMD_DIS,
            DEC_X170_SCMD_DISABLE);
    }
    DEC_SET_APF_THRESHOLD(pDecCont->vc1Regs);
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_MAX_BURST,
        DEC_X170_BUS_BURST_LENGTH);

#if( DEC_X170_USING_IRQ  == 0)
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IRQ_DIS, 1);
#else
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IRQ_DIS, 0);
#endif

    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_MODE, DEC_X170_MODE_VC1);

    /* Set prediction filter taps */
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_0_0, (u32)-4);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_0_1, 53);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_0_2, 18);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_0_3, (u32)-3);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_1_0, (u32)-1);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_1_1,  9);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_1_2,  9);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_1_3, (u32)-1);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_2_0, (u32)-3);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_2_1, 18);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_2_2, 53);
    SetDecRegister(pDecCont->vc1Regs, HWIF_PRED_BC_TAP_2_3, (u32)-4);

    /* set AXI RW IDs */
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_AXI_RD_ID, (DEC_X170_AXI_ID_R & 0xFFU));
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_AXI_WR_ID, (DEC_X170_AXI_ID_W & 0xFFU));

    pDecCont->ppInstance = NULL;
    pDecCont->PPRun = NULL;
    pDecCont->PPEndCallback = NULL;

    ASSERT(pDecCont->dwl != NULL);


    (void)G1DWLmemset(&config, 0, sizeof(DWLHwConfig_t));

    DWLReadAsicConfig(&config);

    pDecCont->refBufSupport = config.refBufSupport;
    if(referenceFrameFormat == DEC_REF_FRM_TILED_DEFAULT)
    {
        /* Assert support in HW before enabling.. */
        if(!config.tiledModeSupport)
        {
            return VC1DEC_FORMAT_NOT_SUPPORTED;
        }
        pDecCont->tiledModeSupport = config.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;
    if( pDecCont->refBufSupport )
    {
        RefbuInit( &pDecCont->refBufferCtrl, DEC_X170_MODE_VC1, 
                   pDecCont->storage.picWidthInMbs,
                   pDecCont->storage.picHeightInMbs,
                   pDecCont->refBufSupport );
    }

    i = G1DWLReadAsicID() >> 16;
    if(i == 0x8170U)
        useVideoFreezeConcealment = 0;
    pDecCont->storage.intraFreeze = useVideoFreezeConcealment;

    DEC_API_TRC("VC1DecInit# OK");

    *pDecInst = (VC1DecInst)pDecCont;

    return(VC1DEC_OK);
}

/*------------------------------------------------------------------------------

    Function name: VC1DecDecode

    Functional description:
        Decodes VC-1 stream.

    Inputs:
        decInst Reference to decoder instance.
        pInput  Decoder input structure.

    Output:
        pOutput Decoder output structure.

    Return values:
        VC1DEC_PIC_DECODED        Picture is decoded.
        VC1DEC_STRM_PROCESSED     Stream handled, no picture ready for display.
        VC1DEC_PARAM_ERROR        Error with function parameters.
        VC1DEC_NOT_INITIALIZED    Attempt to decode with uninitalized decoder.
        VC1DEC_MEMFAIL            Memory allocation failed.
        VC1DEC_HW_BUS_ERROR       A bus error
        VC1DEC_HW_TIMEOUT         Timeout occurred while waiting for HW
        VC1DEC_SYSTEM_ERROR       Wait for hardware failed
        VC1DEC_HW_RESERVED        HW reservation attempt failed
        VC1DEC_HDRS_RDY           Stream headeres decoded.
        VC1DEC_STRM_ERROR         Stream decoding failed.
        VC1DEC_RESOLUTION_CHANGED Picture dimensions has been changed.

------------------------------------------------------------------------------*/
VC1DecRet VC1DecDecode( VC1DecInst decInst,
                        const VC1DecInput* pInput,
                        VC1DecOutput* pOutput )
{
    decContainer_t *pDecCont;
    picture_t *pPic;
    strmData_t streamData;
    u32 asicStatus;
    u32 firstFrame;
    VC1DecRet returnValue = VC1DEC_PIC_DECODED;
    u32 decResult = 0;
    u16x tmpRetVal;
    u32 workIndex;
    u32 ffIndex;
    u32 errorConcealment = HANTRO_FALSE;
    u16x isBPic = HANTRO_FALSE;
    u32 isIntra;

    DEC_API_TRC("VC1DecDecode#");

    /* Check that function input parameters are valid */
    if (pInput == NULL || decInst == NULL)
    {
        DEC_API_TRC("VC1DecDecode# ERROR: NULL argument");
        return(VC1DEC_PARAM_ERROR);
    }

    if ((X170_CHECK_VIRTUAL_ADDRESS( pInput->pStream ) ||
        X170_CHECK_BUS_ADDRESS( pInput->streamBusAddress ) ||
        (pInput->streamSize == 0) ||
        pInput->streamSize > DEC_X170_MAX_STREAM))
    {
        DEC_API_TRC("VC1DecDecode# ERROR: Invalid input parameters");
        return(VC1DEC_PARAM_ERROR);
    }

    pDecCont = (decContainer_t *)decInst;

    /* Check if decoder is in an incorrect mode */
    if (pDecCont->decStat == VC1DEC_UNINITIALIZED)
    {
        DEC_API_TRC("VC1DecDecode# ERROR: Decoder not initialized");
        return(VC1DEC_NOT_INITIALIZED);
    }

    /* init strmData_t structure */
    streamData.pStrmBuffStart = (u8*)pInput->pStream;
    streamData.pStrmCurrPos = streamData.pStrmBuffStart;
    streamData.bitPosInWord = 0;
    streamData.strmBuffSize = pInput->streamSize;
    streamData.strmBuffReadBits = 0;
    streamData.strmExhausted = HANTRO_FALSE;

    if (pDecCont->storage.profile == VC1_ADVANCED)
        streamData.removeEmulPrevBytes = 1;
    else
        streamData.removeEmulPrevBytes = 0;

    firstFrame = (pDecCont->storage.firstFrame) ? 1 : 0;

    /* decode SW part (picture layer) */
    if (pDecCont->storage.resolutionChanged == HANTRO_FALSE)
    {
        decResult = vc1hwdDecode(pDecCont, &pDecCont->storage, &streamData);
        if (pDecCont->storage.resolutionChanged)
        {
            /* save stream position and decResult */
            pDecCont->storage.tmpStrmData = streamData;
            pDecCont->storage.prevDecResult = decResult;
        }
        /* if decoder was not released after field decoding finished (PP still running for another frame) and
         * SW either detected missing field or found something else but field header -> release HW and wait PP
         * to finish */
        if (!pDecCont->asicRunning &&
            ( (decResult != VC1HWD_FIELD_HDRS_RDY && 
               decResult != VC1HWD_USER_DATA_RDY) ||
              pDecCont->storage.missingField ) )
        {
            if (pDecCont->storage.keepHwReserved)
            {
                pDecCont->storage.keepHwReserved = 0;
                (void)G1G1DWLReleaseHw(pDecCont->dwl);
            }
            if (pDecCont->ppInstance != NULL &&
                pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED)
            {
                pDecCont->PPEndCallback(pDecCont->ppInstance);
                pDecCont->ppControl.ppStatus = DECPP_PIC_READY;
            }
        }
    }
    else
    {
        if( pDecCont->refBufSupport )
        {
            RefbuInit( &pDecCont->refBufferCtrl, DEC_X170_MODE_VC1, 
                       pDecCont->storage.picWidthInMbs,
                       pDecCont->storage.picHeightInMbs,
                       pDecCont->refBufSupport );
        }

        /* continue and handle errors in pic layer decoding */
        decResult = pDecCont->storage.prevDecResult;
        /* restore stream position */
        streamData = pDecCont->storage.tmpStrmData;
        pDecCont->storage.resolutionChanged = HANTRO_FALSE;
    }

    pOutput->pStreamCurrPos = streamData.pStrmCurrPos;
    pOutput->strmCurrBusAddress = pInput->streamBusAddress +
        (streamData.pStrmCurrPos - streamData.pStrmBuffStart);
    pOutput->dataLeft = streamData.strmBuffSize -
            (streamData.pStrmCurrPos - streamData.pStrmBuffStart);

    /* return if resolution changed so that user can check PP
     * maximum scaling ratio */
    if (pDecCont->storage.resolutionChanged)
    {
        DEC_API_TRC("VC1DecDecode# VC1DEC_RESOLUTION_CHANGED");
        return VC1DEC_RESOLUTION_CHANGED;
    }

    isIntra = HANTRO_FALSE;
    if(pDecCont->storage.picLayer.fcm == FIELD_INTERLACE)
    {
        if( pDecCont->storage.picLayer.fieldPicType == FP_I_I &&
            !pDecCont->storage.picLayer.isFF )
        {
            isIntra = HANTRO_TRUE;
        }
    }
    else if ( pDecCont->storage.picLayer.picType == PTYPE_I ) 
    {
        isIntra = HANTRO_TRUE;
    }

    if( pDecCont->storage.picLayer.picType == PTYPE_B ||
        pDecCont->storage.picLayer.picType == PTYPE_BI )
    {
        isBPic = HANTRO_TRUE;

        /* Skip B frames after corrupted anchor frame */
        if (pDecCont->storage.skipB ||
            pInput->skipNonReference)
        {
            if( pDecCont->storage.intraFreeze &&
                decResult == VC1HWD_PIC_HDRS_RDY &&
                !pDecCont->storage.slice)
            {
                /* skip B frame and seek next frame start */
                (void)vc1hwdSeekFrameStart(&pDecCont->storage, &streamData);
                pOutput->pStreamCurrPos = streamData.pStrmCurrPos;
                pOutput->strmCurrBusAddress = pInput->streamBusAddress +
                    (streamData.pStrmCurrPos - streamData.pStrmBuffStart);
                pOutput->dataLeft = streamData.strmBuffSize -
                    (streamData.pStrmCurrPos - streamData.pStrmBuffStart);

                if(pDecCont->storage.profile != VC1_ADVANCED)
                    pOutput->dataLeft = 0;

                tmpRetVal = vc1hwdBufferPicture( &pDecCont->storage,
                             pDecCont->storage.workOut,
                             1, pInput->picId, 0);

                return VC1DEC_PIC_DECODED;
            }
            else
            {
                /* skip B frame and seek next frame start */
                (void)vc1hwdSeekFrameStart(&pDecCont->storage, &streamData);
                pOutput->pStreamCurrPos = streamData.pStrmCurrPos;
                pOutput->strmCurrBusAddress = pInput->streamBusAddress +
                    (streamData.pStrmCurrPos - streamData.pStrmBuffStart);
                pOutput->dataLeft = streamData.strmBuffSize -
                    (streamData.pStrmCurrPos - streamData.pStrmBuffStart);

                if(pDecCont->storage.profile != VC1_ADVANCED)
                    pOutput->dataLeft = 0;

                DEC_API_TRC("VC1DecDecode# VC1DEC_NONREF_PIC_SKIPPED (Skip B-frame)");
                return VC1DEC_NONREF_PIC_SKIPPED; /*VC1DEC_STRM_PROCESSED;*/
            }
        }
    }

    if( pDecCont->tiledModeSupport)
    {
        pDecCont->tiledReferenceEnable = 
            DecSetupTiledReference( pDecCont->vc1Regs, 
                pDecCont->tiledModeSupport,
                pDecCont->storage.interlace );
    }
    else
    {
        pDecCont->tiledReferenceEnable = 0;
    }

    /* Check decResult */
    if (decResult == VC1HWD_SEQ_HDRS_RDY)
    {
        /* Force raster scan output for interlaced sequences */
        /*
        if (pDecCont->storage.interlace)
            SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_TILED_E, 0);
            */

        DEC_API_TRC("VC1DecDecode# VC1DEC_HDRS_RDY (Sequence layer)");
        return (VC1DEC_HDRS_RDY);
    }
    else if (decResult == VC1HWD_ENTRY_POINT_HDRS_RDY)
    {
        DEC_API_TRC("VC1DecDecode# VC1DEC_HDRS_RDY (Entry-Point layer");
        return (VC1DEC_HDRS_RDY);
    }
    else if (decResult == VC1HWD_USER_DATA_RDY)
    {
        DEC_API_TRC("VC1DecDecode# VC1DEC_STRM_PROCESSED (User data)");
        return (VC1DEC_STRM_PROCESSED);
    }
    else if (decResult == VC1HWD_METADATA_ERROR )
    {
        DEC_API_TRC("VC1DecDecode# VC1DEC_STRM_ERROR");
        return (VC1DEC_STRM_ERROR);
    }
    else if (decResult == VC1HWD_END_OF_SEQ)
    {
        DEC_API_TRC("VC1DecDecode# VC1DEC_END_OF_SEQ");
        return (VC1DEC_END_OF_SEQ);
    }
    else if (decResult == VC1HWD_MEMORY_FAIL)
    {
        DEC_API_TRC("VC1DecDecode# VC1DEC_MEMFAIL");
        return (VC1DEC_MEMFAIL);
    }
    else if (decResult == VC1HWD_HDRS_ERROR)
    {
        /* Handle case where sequence or entry-point headers are not
         * decoded before the first frame (all resources not allocated) */
        DEC_API_TRC("VC1DecDecode# VC1DEC_STRM_ERROR (header error)");
        return (VC1DEC_STRM_ERROR);
    }
    else if (decResult == VC1HWD_ERROR ||
        (firstFrame && decResult == VC1HWD_NOT_CODED_PIC))
    {
        /* Perform error concealment */
        EPRINT(("Picture layer decoding failed!"));
        errorConcealment = HANTRO_TRUE;

        /* force output index to zero if it is first frame
         * and picture is skipped */
        if (firstFrame)
        {
            pDecCont->storage.workOutPrev = 
                pDecCont->storage.workOut = BqueueNext( 
                &pDecCont->storage.bq,
                pDecCont->storage.work0,
                pDecCont->storage.work1,
                BQUEUE_UNUSED,
                0);
        }

        vc1hwdErrorConcealment( firstFrame, &pDecCont->storage );

        /* skip corrupted B frame or orphan field */
        if (firstFrame || (isBPic && !pDecCont->storage.intraFreeze) || pDecCont->storage.missingField)
        {
            (void)vc1hwdSeekFrameStart(&pDecCont->storage, &streamData);
            returnValue = VC1DEC_STRM_PROCESSED;
        }
        else
            returnValue = VC1DEC_PIC_DECODED;

        if(returnValue == VC1DEC_PIC_DECODED)
        {
            /* set non coded picture for stand alone post-processing */
            pPic = (picture_t*)pDecCont->storage.pPicBuf;
            pPic[ pDecCont->storage.workOut ].field[0].decPpStat = STAND_ALONE;
            pPic[ pDecCont->storage.workOut ].field[1].decPpStat = STAND_ALONE;

            tmpRetVal = vc1hwdBufferPicture( &pDecCont->storage,
                         pDecCont->storage.workOut,
                         isBPic, pInput->picId, pDecCont->storage.numOfMbs );
            ASSERT( tmpRetVal == HANTRO_OK );
#ifdef _DEC_PP_USAGE
    PrintDecPpUsage(pDecCont,
                    pDecCont->storage.picLayer.isFF,
                    pDecCont->storage.workOut,
                    HANTRO_TRUE,
                    pInput->picId);
#endif
        }
    }
    else if (decResult == VC1HWD_NOT_CODED_PIC)
    {
        if( pDecCont->storage.work0 == INVALID_ANCHOR_PICTURE )
        {
            pDecCont->storage.workOutPrev = pDecCont->storage.workOut;
            pDecCont->storage.work0 = pDecCont->storage.workOut;
            pDecCont->storage.workOut = BqueueNext( 
                &pDecCont->storage.bq,
                pDecCont->storage.work0,
                pDecCont->storage.work1, 
                BQUEUE_UNUSED, 0 );
        }
        /* Set picture specific information to picture_t struct */
        vc1hwdSetPictureInfo( pDecCont, pInput->picId );

        /* Clear key-frame flag from output picture, skipped pictures
         * are P pictures and therefore not key-frames... */
        pPic = (picture_t*)pDecCont->storage.pPicBuf;
        pPic[ pDecCont->storage.workOut ].keyFrame = HANTRO_FALSE;
        pPic[ pDecCont->storage.workOut ].fcm =
                    pDecCont->storage.picLayer.fcm;

        /* set non coded picture for stand alone post-processing */
        pPic[ pDecCont->storage.workOut ].field[0].decPpStat = STAND_ALONE;
        pPic[ pDecCont->storage.workOut ].field[1].decPpStat = STAND_ALONE;

        returnValue = VC1DEC_PIC_DECODED;
        tmpRetVal = vc1hwdBufferPicture( &pDecCont->storage,
                     pDecCont->storage.workOut,
                     isBPic, pInput->picId, 0);
        ASSERT( tmpRetVal == HANTRO_OK );

        /* update stream position */
        pOutput->pStreamCurrPos = streamData.pStrmCurrPos;
        pOutput->strmCurrBusAddress = pInput->streamBusAddress +
            (streamData.pStrmCurrPos - streamData.pStrmBuffStart);
        pOutput->dataLeft = streamData.strmBuffSize -
            (streamData.pStrmCurrPos - streamData.pStrmBuffStart);

        if(pDecCont->storage.profile != VC1_ADVANCED)
            pOutput->dataLeft = 0;

#ifdef _DEC_PP_USAGE
    PrintDecPpUsage(pDecCont,
                    pDecCont->storage.picLayer.isFF,
                    pDecCont->storage.workOut,
                    HANTRO_TRUE,
                    pInput->picId);
#endif
    }
    /* If we are using intra freeze concealment and picture is broken */
    else if( !isIntra && pDecCont->storage.pictureBroken && 
        pDecCont->storage.intraFreeze &&
        !pDecCont->storage.slice )
    {
        errorConcealment = HANTRO_TRUE;
        if( pDecCont->storage.work0 == INVALID_ANCHOR_PICTURE )
        {
            pDecCont->storage.workOutPrev = pDecCont->storage.workOut;
            pDecCont->storage.work0 = pDecCont->storage.workOut;
            pDecCont->storage.workOut = BqueueNext( 
                &pDecCont->storage.bq,
                pDecCont->storage.work0,
                pDecCont->storage.work1, 
                BQUEUE_UNUSED, 0 );
        }
        /* Set picture specific information to picture_t struct */
        vc1hwdSetPictureInfo( pDecCont, pInput->picId );

        /* Clear key-frame flag from output picture, skipped pictures
         * are P pictures and therefore not key-frames... */
        pPic = (picture_t*)pDecCont->storage.pPicBuf;
        pPic[ pDecCont->storage.workOut ].keyFrame = HANTRO_FALSE;
        pPic[ pDecCont->storage.workOut ].fcm =
                    pDecCont->storage.picLayer.fcm;

        /* set non coded picture for stand alone post-processing */
        pPic[ pDecCont->storage.workOut ].field[0].decPpStat = STAND_ALONE;
        pPic[ pDecCont->storage.workOut ].field[1].decPpStat = STAND_ALONE;

        returnValue = VC1DEC_PIC_DECODED;
        tmpRetVal = vc1hwdBufferPicture( &pDecCont->storage,
                     pDecCont->storage.workOut,
                     isBPic, pInput->picId, 0);
        ASSERT( tmpRetVal == HANTRO_OK );

        /* update stream position */
        (void)vc1hwdSeekFrameStart(&pDecCont->storage, &streamData);
        pOutput->pStreamCurrPos = streamData.pStrmCurrPos;
        pOutput->strmCurrBusAddress = pInput->streamBusAddress +
            (streamData.pStrmCurrPos - streamData.pStrmBuffStart);
        pOutput->dataLeft = streamData.strmBuffSize -
            (streamData.pStrmCurrPos - streamData.pStrmBuffStart);

        if(pDecCont->storage.profile != VC1_ADVANCED)
            pOutput->dataLeft = 0;

    }
    else /* decResult == VC1HWD_OK */
    {
        if(!pDecCont->storage.slice)
        {
            /* update picture buffer work indexes and
             * check anchor availability */
            vc1hwdUpdateWorkBufferIndexes( &pDecCont->storage, isBPic );

#ifdef _DEC_PP_USAGE
            PrintDecPpUsage(pDecCont,
                        pDecCont->storage.picLayer.isFF,
                        pDecCont->storage.workOut,
                        HANTRO_TRUE,
                        pInput->picId);
#endif
            /* Set picture specific information to picture_t struct */
            vc1hwdSetPictureInfo( pDecCont, pInput->picId );

            /* write bit plane control for HW */
            WriteBitPlaneCtrl(pDecCont->bitPlaneCtrl.virtualAddress,
                pDecCont->storage.pMbFlags, pDecCont->storage.numOfMbs);
        }

        /* Start HW */
        asicStatus = VC1RunAsic(pDecCont, &streamData,
            pInput->streamBusAddress);

        if (asicStatus == X170_DEC_TIMEOUT)
        {
            errorConcealment = HANTRO_TRUE;
            vc1hwdErrorConcealment(0, &pDecCont->storage);
            DEC_API_TRC("VC1DecDecode# VC1DEC_HW_TIMEOUT");
            return VC1DEC_HW_TIMEOUT;
        }
        else if (asicStatus == X170_DEC_SYSTEM_ERROR)
        {
            errorConcealment = HANTRO_TRUE;
            vc1hwdErrorConcealment(0, &pDecCont->storage);
            DEC_API_TRC("VC1DecDecode# VC1DEC_SYSTEM_ERROR");
            return VC1DEC_SYSTEM_ERROR;
        }
        else if (asicStatus == X170_DEC_HW_RESERVED)
        {
            errorConcealment = HANTRO_TRUE;
            vc1hwdErrorConcealment(0, &pDecCont->storage);
            DEC_API_TRC("VC1DecDecode# VC1DEC_HW_RESERVED");
            return VC1DEC_HW_RESERVED;
        }

        SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IRQ_STAT, 0);

        if (asicStatus & DEC_X170_IRQ_BUS_ERROR)
        {
            errorConcealment = HANTRO_TRUE;
            vc1hwdErrorConcealment(0, &pDecCont->storage);
            DEC_API_TRC("VC1DecDecode# VC1DEC_HW_BUS_ERROR");
            return VC1DEC_HW_BUS_ERROR;
        }
        else if (asicStatus & DEC_X170_IRQ_BUFFER_EMPTY)
        {
            /* update stream position */
            pOutput->pStreamCurrPos = streamData.pStrmCurrPos;
            pOutput->strmCurrBusAddress = pInput->streamBusAddress +
                (streamData.pStrmCurrPos - streamData.pStrmBuffStart);
            pOutput->dataLeft = streamData.strmBuffSize -
                (streamData.pStrmCurrPos - streamData.pStrmBuffStart);

            if(pDecCont->storage.profile != VC1_ADVANCED)
                pOutput->dataLeft = 0;

            DEC_API_TRC("VC1DecDecode# VC1DEC_STRM_PROCESSED (Buffer empty)");
            return (VC1DEC_STRM_PROCESSED);
        }
        else if (asicStatus &
                (DEC_X170_IRQ_STREAM_ERROR |
                 DEC_X170_IRQ_ASO |
                 DEC_X170_IRQ_TIMEOUT) )
        {
            errorConcealment = HANTRO_TRUE;
            vc1hwdErrorConcealment( firstFrame, &pDecCont->storage );

            if (firstFrame || (isBPic && !pDecCont->storage.intraFreeze) || pDecCont->storage.slice)
            {
                (void)vc1hwdSeekFrameStart(&pDecCont->storage, &streamData);
                returnValue = VC1DEC_STRM_PROCESSED;
            }
            else
                returnValue = VC1DEC_PIC_DECODED;

            if(returnValue == VC1DEC_PIC_DECODED )
            {
                /* buffer concealed frame
                 * (individual fields are not concealed) */
                tmpRetVal = vc1hwdBufferPicture( &pDecCont->storage,
                             pDecCont->storage.workOut,
                             isBPic, pInput->picId, pDecCont->storage.numOfMbs);
                ASSERT( tmpRetVal == HANTRO_OK );
            }
        }
        else /*(DEC_X170_IRQ_RDY)*/
        {
            tmpRetVal = vc1hwdBufferPicture( &pDecCont->storage,
                          pDecCont->storage.workOut,
                          isBPic, pInput->picId, 0 );
            ASSERT( tmpRetVal == HANTRO_OK );
            returnValue = VC1DEC_PIC_DECODED;
            if( isIntra )
            {
                pDecCont->storage.pictureBroken = HANTRO_FALSE;
            }
        }

        if(errorConcealment && pDecCont->storage.picLayer.picType != PTYPE_B )
        {
            pDecCont->storage.pictureBroken = HANTRO_TRUE;
        }
    }

    /* update stream position */
    pOutput->pStreamCurrPos = streamData.pStrmCurrPos;
    pOutput->strmCurrBusAddress = pInput->streamBusAddress +
        (streamData.pStrmCurrPos - streamData.pStrmBuffStart);
    pOutput->dataLeft = streamData.strmBuffSize -
        (streamData.pStrmCurrPos - streamData.pStrmBuffStart);

    if(pDecCont->storage.profile != VC1_ADVANCED)
        pOutput->dataLeft = 0;

    /* Update picture buffer work indexes */
    if ( (!isBPic && (pDecCont->storage.picLayer.fcm != FIELD_INTERLACE)) ||
         (((pDecCont->storage.picLayer.fcm == FIELD_INTERLACE) &&
           (pDecCont->storage.picLayer.isFF == HANTRO_FALSE)) && !isBPic) )
    {
        if( pDecCont->storage.maxBframes > 0 )
        {
            pDecCont->storage.work1 = pDecCont->storage.work0;
        }
        pDecCont->storage.work0 = pDecCont->storage.workOut;
    }
    pDecCont->picNumber++;

    /* force index to 2 for B frames
     * (prevents overwriting return value of successfully decoded pic) */
    workIndex = (isBPic) ? pDecCont->storage.prevBIdx : pDecCont->storage.workOut;
    pPic = (picture_t*)pDecCont->storage.pPicBuf;

    /* anchor picture succesfully decoded */
    if ( !isBPic && !errorConcealment)
    {
        /* reset B picture skip after succesfully decoded anchor picture */
        if(pDecCont->storage.skipB)
            pDecCont->storage.skipB--;
    }

    ffIndex = (pDecCont->storage.picLayer.isFF) ? 0 : 1;

    /* store returnValue to field structure */
    if (returnValue == VC1DEC_PIC_DECODED)
        pPic[ workIndex ].field[ffIndex].returnValue = VC1DEC_PIC_RDY;
    else
        pPic[ workIndex ].field[ffIndex].returnValue = returnValue;

    /* Copy return value etc. to second field structure too... */
    if (pDecCont->storage.picLayer.fcm != FIELD_INTERLACE)
        pPic[ workIndex ].field[1] = pPic[ workIndex ].field[0];

    DEC_API_TRC("VC1DecDecode# OK");

    return(returnValue);

    /*lint --e(550) Symbol 'tmpRetVal' not accessed */
}

/*------------------------------------------------------------------------------

    Function name: VC1DecGetAPIVersion

        Functional description:
            Returns current API version.

        Inputs:
            none

        Return value:
            VC1DecApiVersion  structure containing API version information.

------------------------------------------------------------------------------*/
VC1DecApiVersion VC1DecGetAPIVersion(void)
{
    VC1DecApiVersion ver;

    ver.major = VC1DEC_MAJOR_VERSION;
    ver.minor = VC1DEC_MINOR_VERSION;

    DEC_API_TRC("VC1DecGetAPIVersion# OK");

    return(ver);
}

/*------------------------------------------------------------------------------

    Function name: VC1DecGetBuild

        Functional description:
            Returns current API version.

        Inputs:
            none

        Return value:
            VC1DecApiVersion  structure containing API version information.

------------------------------------------------------------------------------*/
VC1DecBuild VC1DecGetBuild(void)
{
    VC1DecBuild ver;
    DWLHwConfig_t hwCfg;

    G1DWLmemset(&ver, 0, sizeof(ver));

    ver.swBuild = VC1DEC_SW_BUILD;
    ver.hwBuild = G1DWLReadAsicID();

    DWLReadAsicConfig(&hwCfg);

    SET_DEC_BUILD_SUPPORT(ver.hwConfig, hwCfg);
   
    DEC_API_TRC("VC1DecGetBuild# OK");

    return(ver);
}

/*------------------------------------------------------------------------------

    Function name: VC1DecRelease

        Functional description:
            releases decoder resources.

        Input:
            decInst Reference to decoder instance.

        Return values:
            none

------------------------------------------------------------------------------*/
void VC1DecRelease(VC1DecInst decInst)
{
    decContainer_t *pDecCont;
    const void *dwl;

    DEC_API_TRC("VC1DecRelease#");

    if (decInst == NULL)
    {
        DEC_API_TRC("VC1DecRelease# ERROR: decInst == NULL");
        return;
    }

    pDecCont = (decContainer_t*)decInst;

    /* Check if decoder is in an incorrect mode */
    if (pDecCont->decStat == VC1DEC_UNINITIALIZED)
    {
        DEC_API_TRC("VC1DecRelease# ERROR: Decoder not initialized");
        return;
    }
    dwl = pDecCont->dwl;

     /* PP instance must be already disconnected at this point */
    ASSERT(pDecCont->ppInstance == NULL);

    if(pDecCont->asicRunning)
    {
        G1DWLWriteReg(dwl, 1 * 4, 0); /* stop HW */
        G1G1DWLReleaseHw(dwl);  /* release HW lock */
        pDecCont->asicRunning = 0;
    }

    (void)vc1hwdRelease(dwl, &pDecCont->storage);

    if(pDecCont->bitPlaneCtrl.virtualAddress)
        DWLFreeLinear(dwl, &pDecCont->bitPlaneCtrl);
    if(pDecCont->directMvs.virtualAddress)
        DWLFreeLinear(dwl, &pDecCont->directMvs);

    if (pDecCont->storage.hrdRate)
        G1DWLfree(pDecCont->storage.hrdRate);
    if (pDecCont->storage.hrdBuffer)
        G1DWLfree(pDecCont->storage.hrdBuffer);
    if (pDecCont->storage.hrdFullness)
        G1DWLfree(pDecCont->storage.hrdFullness);

    pDecCont->storage.hrdRate = NULL;
    pDecCont->storage.hrdBuffer = NULL;
    pDecCont->storage.hrdFullness = NULL;

    G1DWLfree(pDecCont);
    (void)G1DWLRelease(dwl);

    DEC_API_TRC("VC1DecRelease# OK");

}

/*------------------------------------------------------------------------------

    Function: VC1DecGetInfo()

        Functional description:
            This function provides read access to decoder information.

        Inputs:
            decInst     decoder instance

        Outputs:
            pDecInfo    pointer to info struct where data is written

        Returns:
            VC1DEC_OK             success
            VC1DEC_PARAM_ERROR    invalid parameters

------------------------------------------------------------------------------*/
VC1DecRet VC1DecGetInfo(VC1DecInst decInst, VC1DecInfo * pDecInfo)
{
    decContainer_t *pDecCont;

    DEC_API_TRC("VC1DecGetInfo#");

    if(decInst == NULL || pDecInfo == NULL)
    {
        DEC_API_TRC("VC1DecGetInfo# ERROR: decInst or pDecInfo is NULL");
        return (VC1DEC_PARAM_ERROR);
    }

    pDecCont = (decContainer_t*)decInst;

    // ++Leo@2011/10/21: this should use 
#if 0
    if(pDecCont->tiledReferenceEnable)
#else
    if(pDecCont->tiledModeSupport)
#endif
    // --Leo@2011/10/21
    {
        pDecInfo->outputFormat = VC1DEC_TILED_YUV420;
    }
    else
    {
        pDecInfo->outputFormat = VC1DEC_SEMIPLANAR_YUV420;
    }

    pDecInfo->maxCodedWidth  = pDecCont->storage.maxCodedWidth;
    pDecInfo->maxCodedHeight = pDecCont->storage.maxCodedHeight;
    pDecInfo->codedWidth     = pDecCont->storage.curCodedWidth;
    pDecInfo->codedHeight    = pDecCont->storage.curCodedHeight;
    pDecInfo->parWidth       = pDecCont->storage.aspectHorizSize;
    pDecInfo->parHeight      = pDecCont->storage.aspectVertSize;
    pDecInfo->frameRateNumerator    = pDecCont->storage.frameRateNr;
    pDecInfo->frameRateDenominator  = pDecCont->storage.frameRateDr;
    pDecInfo->interlacedSequence = pDecCont->storage.interlace;
    pDecInfo->multiBuffPpSize = 2; /*pDecCont->storage.interlace ? 1 : 2;*/

    DEC_API_TRC("VC1DecGetInfo# OK");

    return (VC1DEC_OK);

}

/*------------------------------------------------------------------------------

    Function: VC1DecUnpackMetadata

        Functional description:
            Unpacks metadata elements for sequence header C from buffer,
            when metadata is packed according to SMPTE VC-1 Standard Annex J.

        Inputs:
            pBuffer     Pointer to buffer containing packed metadata. Buffer
                        must contain at least 4 bytes.
            bufferSize  Buffer size in bytes.
            pMetaData   Pointer to stream metadata container.

        Return values:
            VC1DEC_OK             Metadata successfully unpacked.
            VC1DEC_PARAM_ERROR    Error with function parameters.
            VC1DEC_METADATA_FAIL  Meta data is in wrong format or indicates
                                  unsupported tools

------------------------------------------------------------------------------*/
VC1DecRet VC1DecUnpackMetaData(const u8* pBuffer, u32 bufferSize,
    VC1DecMetaData* pMetaData )
{
    DEC_API_TRC("VC1DecUnpackMetadata#");

    if (bufferSize < 4)
    {
        DEC_API_TRC("VC1DecUnpackMetadata# ERROR: bufferSize < 4");
        return(VC1DEC_PARAM_ERROR);
    }
    if (pBuffer == NULL)
    {
        DEC_API_TRC("VC1DecUnpackMetadata# ERROR: pBuffer == NULL");
        return(VC1DEC_PARAM_ERROR);
    }
    if (pMetaData == NULL)
    {
        DEC_API_TRC("VC1DecUnpackMetadata# ERROR: pMetaData == NULL");
        return(VC1DEC_PARAM_ERROR);
    }

    if ( vc1hwdUnpackMetaData( pBuffer, pMetaData ) != HANTRO_OK )
    {
        DEC_API_TRC("VC1DecUnpackMetadata# ERROR: Metadata failure");
        return(VC1DEC_METADATA_FAIL);
    }

    DEC_API_TRC("VC1DecUnpackMetadata# OK");

    return(VC1DEC_OK);
}

/*------------------------------------------------------------------------------
    Function name   : vc1RegisterPP
    Description     :
    Return type     : i32
    Argument        : const void * decInst
    Argument        : const void  *ppInst
    Argument        : (*PPRun)(const void *)
    Argument        : void (*PPEndCallback)(const void *)
------------------------------------------------------------------------------*/

i32 vc1RegisterPP(const void *decInst, const void *ppInst,
                   void (*PPRun)(const void *, DecPpInterface *),
                   void (*PPEndCallback) (const void *),
                   void  (*PPConfigQuery)(const void *, DecPpQuery *),
                    void (*PPDisplayIndex)(const void *, u32),
		    void (*PPBufferData) (const void *, u32, u32, u32))
{
    decContainer_t *pDecCont;

    pDecCont = (decContainer_t *) decInst;

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
    
    pDecCont->ppControl.bufferIndex = 0;
    pDecCont->ppControl.multiBufStat = MULTIBUFFER_UNINIT;

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : vc1UnregisterPP
    Description     :
    Return type     : i32
    Argument        : const void * decInst
    Argument        : const void  *ppInst
------------------------------------------------------------------------------*/
i32 vc1UnregisterPP(const void *decInst, const void *ppInst)
{
    decContainer_t *pDecCont;

    pDecCont = (decContainer_t *) decInst;

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

    Function name: WriteBitPlaneCtrl

        Functional description:
            Write bit plane control information to SW-HW shared memory.

        Inputs:
            mbFlags     pointer to data decoded from bitplane
            numMbs      number of macroblocks in picture

        Outputs:
            bitPlaneCtrl    pointer to SW-HW shared memory where bit plane
                            control data shall be written

        Return values:

------------------------------------------------------------------------------*/
void WriteBitPlaneCtrl(u32 *bitPlaneCtrl, u8 *mbFlags, u32 numMbs)
{

    u32 i, j;
    u32 tmp;
    u8 tmp1;
    u32 *pTmp = bitPlaneCtrl;

    for (i = (numMbs+9)/10; i--;)
    {
        tmp = 0;
        for (j = 0; j < 10; j++)
        {
            tmp1 = *mbFlags++;
            tmp = (tmp<<3) | (tmp1 & 0x7);
        }
        tmp <<= 2;
        *pTmp++ = tmp;
    }

}

/*------------------------------------------------------------------------------

    Function name: VC1DecNextPicture

    Functional description:
        Retrieve next decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct
        endOfStream Indicates whether end of stream has been reached

    Output:
        pPicture Decoder output picture.

    Return values:
        VC1DEC_OK               No picture available.
        VC1DEC_PIC_RDY          Picture ready.
        VC1DEC_PARAM_ERROR      Error with function parameters.
        VC1DEC_NOT_INITIALIZED  Attempt to call function with uninitalized
                                decoder.

------------------------------------------------------------------------------*/
VC1DecRet VC1DecNextPicture( VC1DecInst     decInst,
                             VC1DecPicture  *pPicture,
                             u32            endOfStream)
{

/* Variables */

    VC1DecRet returnValue = VC1DEC_PIC_RDY;
    decContainer_t *pDecCont;
    picture_t *pPic;
    u32 picIndex;
    u32 fieldToReturn = 0;
    u32 tmp;
    u32 errMbs, picId;
    u32 luma = 0;
    u32 chroma = 0;
    u32 forcePpStandalone = 0;
    u32 parallelMode2Flag = 0;

/* Code */

    DEC_API_TRC("VC1DecNextPicture#");

    /* Check that function input parameters are valid */
    if (pPicture == NULL)
    {
        DEC_API_TRC("VC1DecNextPicture# ERROR: pPicture is NULL");
        return(VC1DEC_PARAM_ERROR);
    }

    pDecCont = (decContainer_t *)decInst;

    /* Check if decoder is in an incorrect mode */
    if (decInst == NULL || pDecCont->decStat == VC1DEC_UNINITIALIZED)
    {
        DEC_API_TRC("VC1SwDecNextPicture# ERROR: Decoder not initialized");
        return(VC1DEC_NOT_INITIALIZED);
    }

    /* Check that asic is ready */
    if(pDecCont->asicRunning)
    {
        G1DWLWriteReg(pDecCont->dwl, 1 * 4, 0);   /* stop HW */

        /* Wait for PP to end also */
        if(pDecCont->ppInstance != NULL &&
           pDecCont->ppControl.ppStatus == DECPP_RUNNING)
        {
            pDecCont->ppControl.ppStatus = DECPP_PIC_READY;

            pDecCont->PPEndCallback(pDecCont->ppInstance);
        }

        G1G1DWLReleaseHw(pDecCont->dwl);    /* release HW lock */

        pDecCont->asicRunning = 0;
    }
    /* check deinterlacing status */
    if ( pDecCont->ppInstance != NULL )
    {
        pDecCont->ppConfigQuery.tiledMode =
            pDecCont->tiledReferenceEnable;
        pDecCont->PPConfigQuery(pDecCont->ppInstance,
                                &pDecCont->ppConfigQuery);
    }
    else
    {
        pDecCont->ppConfigQuery.deinterlace = 0;
    }    
    
    if(pDecCont->ppInstance &&
     (pDecCont->ppControl.multiBufStat == MULTIBUFFER_DISABLED) &&
     (pDecCont->ppConfigQuery.ppConfigChanged == 1) &&
     (pDecCont->storage.maxBframes == 0))
    {
       pDecCont->storage.minCount = 0;
       forcePpStandalone = 1;
    }

    if(pDecCont->ppInstance &&
       (pDecCont->ppControl.multiBufStat == MULTIBUFFER_FULLMODE) &&
       endOfStream && (returnValue == VC1DEC_PIC_RDY))
    {
        pDecCont->ppControl.multiBufStat = MULTIBUFFER_UNINIT;
        if( pDecCont->storage.previousB)
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

    /* Query next picture */
    if( vc1hwdNextPicture( &pDecCont->storage, &picIndex, &fieldToReturn,
                endOfStream, pDecCont->ppConfigQuery.deinterlace,
                &picId, &errMbs ) == HANTRO_NOK )
    {
        pPicture->pOutputPicture = NULL;
        pPicture->outputPictureBusAddress = 0;
        pPicture->codedWidth = 0;
        pPicture->codedHeight = 0;
        pPicture->frameWidth = 0;
        pPicture->frameHeight = 0;
        pPicture->keyPicture = 0;
        pPicture->rangeRedFrm = 0;
        pPicture->rangeMapYFlag = 0;
        pPicture->rangeMapY = 0;
        pPicture->rangeMapUvFlag = 0;
        pPicture->rangeMapUv = 0;
        pPicture->picId = 0;
        pPicture->interlaced = 0;
        pPicture->fieldPicture = 0;
        pPicture->topField = 0;
        pPicture->firstField = 0;
        pPicture->anchorPicture = 0;
        pPicture->repeatFirstField = 0;
        pPicture->repeatFrameCount = 0;
        pPicture->numberOfErrMBs = 0;
        pPicture->outputFormat = DEC_OUT_FRM_RASTER_SCAN;
        returnValue = VC1DEC_OK;
    }
    else
    {
        /* pp and decoder running in parallel, decoder finished first field ->
        * decode second field and wait PP after that */
        if(pDecCont->ppInstance != NULL &&
            pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED && 
            !endOfStream)
            return (VC1DEC_OK);
        
        pPic = (picture_t*)pDecCont->storage.pPicBuf;
        pPicture->pOutputPicture    = (u8*)pPic[picIndex].data.virtualAddress;
        pPicture->outputPictureBusAddress = pPic[picIndex].data.busAddress;
        pPicture->codedWidth        = pPic[picIndex].codedWidth;
        pPicture->codedHeight       = pPic[picIndex].codedHeight;
        pPicture->frameWidth        = ( pPicture->codedWidth + 15 ) & ~15;
        pPicture->frameHeight       = ( pPicture->codedHeight + 15 ) & ~15;
        pPicture->keyPicture        = pPic[picIndex].keyFrame;
        pPicture->rangeRedFrm       = pPic[picIndex].rangeRedFrm;
        pPicture->rangeMapYFlag     = pPic[picIndex].rangeMapYFlag;
        pPicture->rangeMapY         = pPic[picIndex].rangeMapY;
        pPicture->rangeMapUvFlag    = pPic[picIndex].rangeMapUvFlag;
        pPicture->rangeMapUv        = pPic[picIndex].rangeMapUv;
        pPicture->outputFormat      = pPic[picIndex].tiledMode ?
            DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;

        pPicture->interlaced        =
                                (pPic[picIndex].fcm == PROGRESSIVE) ? 0 : 1;
        pPicture->fieldPicture      =
                                (pDecCont->storage.interlace) ? 1 : 0;
        pPicture->topField          = ((1-fieldToReturn) ==
                                pPic[picIndex].isTopFieldFirst) ? 1 : 0;
        pPicture->firstField        = (fieldToReturn == 0) ? 1 : 0;
        pPicture->anchorPicture     =
                ((pPic[picIndex].field[fieldToReturn].type == PTYPE_I) ||
                 (pPic[picIndex].field[fieldToReturn].type == PTYPE_P)) ? 1 : 0;
        pPicture->repeatFirstField  = pPic[picIndex].rff;
        pPicture->repeatFrameCount  = pPic[picIndex].rptfrm;
        pPicture->picId             = picId;
        pPicture->numberOfErrMBs    = errMbs;
        returnValue                 =
                pPic[picIndex].field[fieldToReturn].returnValue;

        if ( pDecCont->ppInstance != NULL )
        {
    
            if(forcePpStandalone && pPic[picIndex].field[0].decPpStat == NONE)
            {
                pPic[picIndex].field[0].decPpStat = STAND_ALONE;
                pPic[picIndex].field[1].decPpStat = STAND_ALONE;
            }
	    
            /* post-process non-post-processed pictures
             * in case of end of stream */
            if ( endOfStream &&
                 (pPic[picIndex].field[fieldToReturn].decPpStat == NONE) )
            {
                pPic[picIndex].field[fieldToReturn].decPpStat = STAND_ALONE;
            }

            if( endOfStream &&
                pDecCont->storage.parallelMode2 )
            {
                if( !pDecCont->storage.pm2AllProcessedFlag )
                {

                    /* Set flag to signal PP has run through every frame
                     * it has to. Also set previous PP picture as "locked",
                     * so we now to show it last. */
                    pDecCont->storage.pm2AllProcessedFlag = 1;
                    pDecCont->ppControl.bufferIndex = 
                        BqueueNext( &pDecCont->storage.bqPp,
                            pDecCont->storage.pm2lockBuf,
                            BQUEUE_UNUSED,
                            BQUEUE_UNUSED,
                            0 );

                    /* Connect PP output buffer to decoder output buffer */
                    {
                        u32 workBuffer;
                        u32 luma = 0;
                        u32 chroma = 0;

                        workBuffer = pDecCont->storage.workOutPrev;

                        luma = pDecCont->storage.pPicBuf[workBuffer].
                                                data.busAddress;
                        chroma = luma + ((pDecCont->storage.picWidthInMbs * 16) *
                                        (pDecCont->storage.picHeightInMbs * 16));

                        pDecCont->PPBufferData(pDecCont->ppInstance, 
                            pDecCont->ppControl.bufferIndex, luma, chroma);
                    }

                    /* If previous picture was B, we send that to PP now */
                    if(pDecCont->storage.previousB)
                    {
                        /*pPic[picIndex].sendToPp = 1;*/
                        pDecCont->storage.previousB = 0;
                        /*parallelMode2Flag = 1;*/
                        pDecCont->ppControl.displayIndex = 
                            pDecCont->ppControl.bufferIndex;
                    }
                    /* ...otherwise we send the previous anchor. */
                    else
                    {
                        /*pPic[picIndex].sendToPp = 1;*/
                        parallelMode2Flag = 1;
                        pDecCont->ppControl.displayIndex = 
                            pDecCont->storage.pm2lockBuf;
                        pDecCont->storage.pm2lockBuf =
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
                }
                else
                {
                    pDecCont->ppControl.displayIndex = 
                        pDecCont->ppControl.bufferIndex = 
                        pDecCont->storage.pm2lockBuf;
                    pDecCont->PPDisplayIndex(pDecCont->ppInstance,
                                             pDecCont->ppControl.displayIndex);

                }
            }

            if (pDecCont->storage.interlace)
            {
                /* if deinterlace is ON, read also second field
                 * from NextPicture */
                if ( pDecCont->ppConfigQuery.deinterlace)
                {
                    pDecCont->ppControl.picStruct =
                        DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;

                    /* if deinterlace is ON -> output is frame */
                    pPicture->fieldPicture = 0;
                }
                else /* separate fields */
                {
                    pDecCont->ppControl.picStruct = (pPicture->topField) ?
                        DECPP_PIC_TOP_FIELD_FRAME : DECPP_PIC_BOT_FIELD_FRAME;
                }
            }
            else /* progressive frame */
            {
                pDecCont->ppControl.picStruct = DECPP_PIC_FRAME_OR_TOP_FIELD;
            }

            if (pPic[picIndex].field[fieldToReturn].decPpStat == STAND_ALONE)
            {
                if (pDecCont->ppConfigQuery.multiBuffer)
                {
                    pDecCont->ppControl.multiBufStat = MULTIBUFFER_SEMIMODE;
                    pDecCont->storage.previousModeFull = 0;

                    if(pDecCont->storage.parallelMode2)
                    {
                        Vc1DecPpSetPpOutpStandalone( &pDecCont->storage.decPp, 
                            &pDecCont->ppControl );
                        /*
                        pDecCont->ppControl.bufferIndex =
                            pPic[picIndex].field[fieldToReturn].ppBufferIndex;
                            */
                    }
                    
                    if(pPic[picIndex].field[fieldToReturn].type == PTYPE_Skip)
                    {
                        pDecCont->storage.minCount = 0;
                    }		
                }
                
                if((pDecCont->storage.picLayer.picType == PTYPE_B) ||
                   (pDecCont->storage.picLayer.picType == PTYPE_BI))
                {
                    pDecCont->storage.previousB = 1;
                }
                else
                {
                    pDecCont->storage.previousB = 0;
                }
            
                tmp = pPicture->frameWidth*pPicture->frameHeight;
                /* setup PP control parameters */
                pDecCont->ppControl.inputBusLuma =
                                pPicture->outputPictureBusAddress;
                pDecCont->ppControl.inputBusChroma =
                                pPicture->outputPictureBusAddress + tmp;
                pDecCont->ppControl.bottomBusLuma =
                                pPicture->outputPictureBusAddress +
                                (pDecCont->storage.picWidthInMbs * 16);
                pDecCont->ppControl.bottomBusChroma =
                                pDecCont->ppControl.bottomBusLuma + tmp;
                pDecCont->ppControl.usePipeline = HANTRO_FALSE;
                pDecCont->ppControl.littleEndian =
                    GetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_ENDIAN);
                pDecCont->ppControl.wordSwap =
                    GetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUTSWAP32_E);
                pDecCont->ppControl.inwidth = pPicture->frameWidth;
                pDecCont->ppControl.inheight = pPicture->frameHeight;
                pDecCont->ppControl.tiledInputMode = pPic[picIndex].tiledMode;
                pDecCont->ppControl.progressiveSequence = !pDecCont->storage.interlace;
                pDecCont->ppControl.croppedW =
                        ((pPic[picIndex].codedWidth+7) & ~7);
                pDecCont->ppControl.croppedH =
                        ((pPic[picIndex].codedHeight+7) & ~7);

                if ( (pDecCont->ppControl.picStruct ==
                                            DECPP_PIC_TOP_FIELD_FRAME) ||
                     (pDecCont->ppControl.picStruct ==
                                            DECPP_PIC_BOT_FIELD_FRAME) )
                {
                    /* input height is multiple of 16 */
                    pDecCont->ppControl.inheight >>= 1;
                    pDecCont->ppControl.inheight =
                        ((pDecCont->ppControl.inheight + 15) & ~15);
                    /* cropped height is multiple of 8 */
                    pDecCont->ppControl.croppedH >>= 1;
                    pDecCont->ppControl.croppedH =
                        ((pDecCont->ppControl.croppedH + 7) & ~7);
                }

                pDecCont->ppControl.rangeRed = pPicture->rangeRedFrm;
                pDecCont->ppControl.rangeMapYEnable = pPicture->rangeMapYFlag;
                pDecCont->ppControl.rangeMapYCoeff = pPicture->rangeMapY;
                pDecCont->ppControl.rangeMapCEnable = pPicture->rangeMapUvFlag;
                pDecCont->ppControl.rangeMapCCoeff = pPicture->rangeMapUv;

                ASSERT(pDecCont->ppControl.ppStatus != DECPP_RUNNING);

                /* start PP */
                pDecCont->PPRun( pDecCont->ppInstance,
                                 &pDecCont->ppControl );
                pDecCont->ppControl.ppStatus = DECPP_RUNNING;

#ifdef _DEC_PP_USAGE
    PrintDecPpUsage(pDecCont,
                    pPicture->firstField,
                    picIndex,
                    HANTRO_FALSE,
                    pPic[picIndex].field[fieldToReturn].picId);
#endif
            }
        }
        if ( (pDecCont->ppInstance != NULL) &&
             (pDecCont->ppControl.ppStatus == DECPP_RUNNING) )
        {
            pDecCont->PPEndCallback(pDecCont->ppInstance);
            pDecCont->ppControl.ppStatus = DECPP_PIC_READY;
        }
    }

    return returnValue;
}

/*------------------------------------------------------------------------------

    Function name: Vc1CheckFormatSupport

    Functional description:
        Check if VC-1 supported

    Input:
        container

    Return values:
        return zero for OK

------------------------------------------------------------------------------*/
u32 Vc1CheckFormatSupport(void)
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

    return (hwConfig.vc1Support == VC1_NOT_SUPPORTED);
}

/*------------------------------------------------------------------------------

    Function name: VC1DecPeek

    Functional description:
        Retrieve last decoded picture

    Input:
        decInst     Reference to decoder instance.
        pPicture    Pointer to return value struct

    Output:
        pPicture Decoder output picture.

    Return values:
        VC1DEC_OK               No picture available.
        VC1DEC_PIC_RDY          Picture ready.
        VC1DEC_PARAM_ERROR      Error with function parameters.
        VC1DEC_NOT_INITIALIZED  Attempt to call function with uninitalized
                                decoder.

------------------------------------------------------------------------------*/
VC1DecRet VC1DecPeek( VC1DecInst     decInst,
                      VC1DecPicture  *pPicture )
{

/* Variables */

    decContainer_t *pDecCont;
    picture_t *pPic;

/* Code */

    DEC_API_TRC("VC1DecPeek#");

    /* Check that function input parameters are valid */
    if (pPicture == NULL)
    {
        DEC_API_TRC("VC1DecPeek# ERROR: pPicture is NULL");
        return(VC1DEC_PARAM_ERROR);
    }

    pDecCont = (decContainer_t *)decInst;

    /* Check if decoder is in an incorrect mode */
    if (decInst == NULL || pDecCont->decStat == VC1DEC_UNINITIALIZED)
    {
        DEC_API_TRC("VC1SwDecNextPicture# ERROR: Decoder not initialized");
        return(VC1DEC_NOT_INITIALIZED);
    }

    /* Query next picture */
    if( pDecCont->storage.outpCount == 0 ||
        ( pDecCont->storage.picLayer.fcm == FIELD_INTERLACE &&
          pDecCont->storage.picLayer.isFF ) )
    {
        G1DWLmemset(pPicture, 0, sizeof(VC1DecPicture));
        return VC1DEC_OK;
    }

    pPic = (picture_t*)pDecCont->storage.pPicBuf +
        pDecCont->storage.workOut;
    pPicture->pOutputPicture    = (u8*)pPic->data.virtualAddress;
    pPicture->outputPictureBusAddress = pPic->data.busAddress;
    pPicture->codedWidth        = pPic->codedWidth;
    pPicture->codedHeight       = pPic->codedHeight;
    pPicture->frameWidth        = ( pPicture->codedWidth + 15 ) & ~15;
    pPicture->frameHeight       = ( pPicture->codedHeight + 15 ) & ~15;
    pPicture->keyPicture        = pPic->keyFrame;
    pPicture->rangeRedFrm       = pPic->rangeRedFrm;
    pPicture->rangeMapYFlag     = pPic->rangeMapYFlag;
    pPicture->rangeMapY         = pPic->rangeMapY;
    pPicture->rangeMapUvFlag    = pPic->rangeMapUvFlag;
    pPicture->rangeMapUv        = pPic->rangeMapUv;

    pPicture->interlaced        = (pPic->fcm == PROGRESSIVE) ? 0 : 1;
    pPicture->fieldPicture      = 0;
    pPicture->topField          = 0;
    pPicture->firstField        = 0;
    pPicture->anchorPicture     =
            ((pPic->field[0].type == PTYPE_I) ||
             (pPic->field[0].type == PTYPE_P)) ? 1 : 0;
    pPicture->repeatFirstField  = pPic->rff;
    pPicture->repeatFrameCount  = pPic->rptfrm;
    pPicture->picId             =
        pDecCont->storage.outPicId[0][pDecCont->storage.prevOutpIdx];
    pPicture->numberOfErrMBs    =
        pDecCont->storage.outErrMbs[pDecCont->storage.prevOutpIdx];

    return VC1DEC_PIC_RDY;

}

/*------------------------------------------------------------------------------

    Function name: VC1DecSetLatency

    Functional description:
        set current video latencyMS to vdec driver for vpu dvfs

    Input:
        decInst     Reference to decoder instance.
        latencyMS   


    Return values:
        VC1DEC_OK;

------------------------------------------------------------------------------*/
VC1DecRet VC1DecSetLatency(VC1DecInst decInst, int latencyMS)
{
    decContainer_t *pDecCont = (decContainer_t *) decInst;

    DWLSetLatency(pDecCont->dwl, latencyMS);

    return VC1DEC_OK;
}
