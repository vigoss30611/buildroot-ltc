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
--  Abstract : Stream decoding top level functions (interface functions)
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_strmdec.c,v $
--  $Date: 2010/05/14 10:45:44 $
--  $Revision: 1.22 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_DecoderInit
        5.2     StrmDec_Decode
        5.3     StrmDec_PrepareConcealment

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mp4dechwd_strmdec.h"
#include "mp4dechwd_utils.h"
#include "mp4dechwd_headers.h"
#include "mp4dechwd_vop.h"
#include "mp4dechwd_videopacket.h"
#include "mp4dechwd_shortvideo.h"
#include "mp4dechwd_generic.h"
#include "mp4dechwd_error_conceal.h"
#include "workaround.h"
#include "mp4debug.h"
/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

enum
{
    CONTINUE
};

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

u32 StrmDec_PrepareConcealment(DecContainer * pDecContainer, u32 startCode);

/*------------------------------------------------------------------------------

   5.1  Function name: StrmDec_DecoderInit

        Purpose: initialize stream decoding related parts of DecContainer

        Input:
            Pointer to DecContainer structure
                -initializes StrmStorage

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

void StrmDec_DecoderInit(DecContainer * pDecContainer)
{
    /* initialize video packet qp to 1 -> reasonable qp value if first vop
     * has to be concealed */
    pDecContainer->StrmStorage.vpQP = 1;
    pDecContainer->packedMode = 0;
}

/*------------------------------------------------------------------------------

   5.2  Function name: StrmDec_Decode

        Purpose: Decode MPEG4 stream. Continues decoding until END_OF_STREAM
        encountered or whole VOP decoded. Returns after decoding of VOL header.
        Also returns after checking source format in case short video header
        stream is decoded.

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmStorage
                -uses and updates StrmDesc
                -uses and updates VopDesc
                -uses Hdrs
                -uses MbSetDesc

        Output:
            DEC_RDY if everything was ok but no VOP finished
            DEC_HDR_RDY if headers decoded
            DEC_HDR_RDY_BUF_NOT_EMPTY if headers decoded but buffer not empty
            DEC_VOP_RDY if whole VOP decoded
            DEC_VOP_RDY_BUF_NOT_EMPTY if whole VOP decoded but buffer not empty
            DEC_END_OF_STREAM if eos encountered while decoding
            DEC_OUT_OF_BUFFER if asic input rlc buffer full
            DEC_ERROR if such an error encountered that recovery needs initial
            headers (in short video case only vop_with_sv_header needed)

------------------------------------------------------------------------------*/

u32 StrmDec_Decode(DecContainer * pDecContainer)
{

    u32 tmp;
    u32 status;
    u32 startCode = 0;

    /* flag to prevent concealment after failure in decoding of repeated
     * headers (VOS,VISO,VO,VOL) */
    u32 headerFailure;

    MP4DEC_DEBUG(("entry StrmDec_Decode\n"));

    status = HANTRO_OK;
    headerFailure = HANTRO_FALSE;

    /* make sure that rlc buffer is large enough for decoding */
    ASSERT((pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE) ||
           !pDecContainer->Hdrs.dataPartitioned ||
           (pDecContainer->MbSetDesc.rlcDataBufferSize >= 1750));
    ASSERT((pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE) ||
           (pDecContainer->rlcMode == 0) ||
           (pDecContainer->MbSetDesc.rlcDataBufferSize >= 64));
    ASSERT(!pDecContainer->Hdrs.dataPartitioned ||
            pDecContainer->rlcMode);

    if( pDecContainer->StrmStorage.customStrmHeaders )
    {
        return StrmDec_DecodeCustomHeaders( pDecContainer );
    }

    /* reset last sync pointer if in the beginning of stream */
    if(pDecContainer->StrmDesc.pStrmCurrPos ==
       pDecContainer->StrmDesc.pStrmBuffStart)
    {
        pDecContainer->StrmStorage.pLastSync =
            pDecContainer->StrmDesc.pStrmBuffStart;
    }
    /* initialize pRlcDataVpAddr pointer (pointer to beginning of rlc data of
     * current video packet) */
    if (pDecContainer->StrmStorage.status == STATE_OK)
    {
        pDecContainer->MbSetDesc.pRlcDataVpAddr =
            pDecContainer->MbSetDesc.pRlcDataCurrAddr;
        pDecContainer->MbSetDesc.oddRlcVp =
            pDecContainer->MbSetDesc.oddRlc;
    }

    /* reset error counter in the beginning of VOP */
    if(pDecContainer->StrmStorage.vpMbNumber == 0)
    {
        pDecContainer->StrmStorage.numErrMbs = 0;
    }

    /* keep decoding till something ready or something wrong */
    do
    {
        if(pDecContainer->StrmStorage.status == STATE_OK)
        {
            startCode = StrmDec_GetStartCode(pDecContainer);
            if(startCode == END_OF_STREAM)
            {
                if(pDecContainer->StrmStorage.strmDecReady == HANTRO_TRUE)
                {
                    return (DEC_END_OF_STREAM);
                }
                else
                {
                    return (DEC_ERROR);
                }
            }
        }
        /* sync lost -> find sync and conceal */
        else
        {
            startCode = StrmDec_FindSync(pDecContainer);
            /* return DEC_END_OF_STREAM only if resync markers are enabled
             * or vop has not been started yet (header not
             * decoded successfully), return DEC_ERROR if decoder not ready,
             * otherwise VOP is concealed */
            if(startCode == END_OF_STREAM)
            {
                if(pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE)
                {
                    return (DEC_ERROR);
                }
                else if(!pDecContainer->Hdrs.resyncMarkerDisable ||
                        (pDecContainer->StrmStorage.validVopHeader ==
                         HANTRO_FALSE))
                {
                    return (DEC_END_OF_STREAM);
                }
            }

            if(pDecContainer->StrmStorage.strmDecReady == HANTRO_TRUE)
            {
                /* concealment if sync loss was not due to errors in decoding
                 * of initial headers (repeated headers as strmDecReady is
                 * true here) */
                if(headerFailure == HANTRO_FALSE && pDecContainer->rlcMode &&
                   pDecContainer->VopDesc.vopCodingType != BVOP)
                {
                    if(pDecContainer->MbSetDesc.ctrlDataMem.virtualAddress ==
                        NULL)
                    {
                        return DEC_VOP_HDR_RDY_ERROR;
                    }
                    status = StrmDec_PrepareConcealment(pDecContainer,
                                                        startCode);
                }
                else
                {
                    pDecContainer->StrmStorage.status = STATE_OK;
                    status = CONTINUE;
                }

                pDecContainer->StrmStorage.startCodeLoss = HANTRO_FALSE;

                /* decoding of VOP finished -> return */
                if(status == DEC_VOP_RDY)
                {
                    if(startCode == END_OF_STREAM)
                    {
                        return (DEC_VOP_RDY);
                    }
                    else
                    {
                        return (DEC_VOP_RDY_BUF_NOT_EMPTY);
                    }
                }
            }
            else
            {
                pDecContainer->StrmStorage.status = STATE_OK;
                pDecContainer->StrmStorage.startCodeLoss = HANTRO_FALSE;
            }
        }

        headerFailure = HANTRO_FALSE;

        switch (startCode)
        {
        case SC_VO_START:
            break;
        case SC_VOS_START:
        case SC_VISO_START:
        case SC_VOL_START:

            pDecContainer->StrmStorage.mpeg4Video = HANTRO_TRUE;
            if((pDecContainer->Hdrs.lock == HANTRO_FALSE) &&
               (pDecContainer->StrmStorage.strmDecReady == HANTRO_TRUE))
            {
                StrmDec_DecoderInit(pDecContainer);
                pDecContainer->StrmStorage.pLastSync =
                    pDecContainer->StrmDesc.pStrmCurrPos;
                if (startCode == SC_VOL_START)
                {
                    pDecContainer->tmpHdrs = pDecContainer->Hdrs;
                    StrmDec_ClearHeaders(&(pDecContainer->Hdrs));
                }
            }
            status = StrmDec_DecodeHdrs(pDecContainer, startCode);
            if (status != HANTRO_OK &&
                startCode == SC_VOL_START)
            {
                pDecContainer->Hdrs = pDecContainer->tmpHdrs;
            }

            if(pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE ||
               pDecContainer->StrmStorage.lowDelay !=
                   pDecContainer->Hdrs.lowDelay ||
               pDecContainer->StrmStorage.videoObjectLayerWidth !=
                   pDecContainer->Hdrs.videoObjectLayerWidth ||
               pDecContainer->StrmStorage.videoObjectLayerHeight !=
                   pDecContainer->Hdrs.videoObjectLayerHeight )
            {
                /* VOL decoded -> set vop size parameters and set
                 * strmDecReady. Return control to caller with
                 * indication whether buffer is empty or not */
                if(pDecContainer->Hdrs.lastHeaderType == SC_VOL_START)
                {
                    /* error in header decoding and decoder not ready (i.e. 
                     * this is not repetition of header info) -> return
                     * control to caller */
                    if(status != HANTRO_OK)
                    {
                        pDecContainer->StrmStorage.status = STATE_SYNC_LOST;
                        if(status != END_OF_STREAM)
                            return (DEC_ERROR_BUF_NOT_EMPTY);
                        else
                            return (DEC_ERROR);
                    }

                    pDecContainer->VopDesc.vopWidth =
                        (pDecContainer->Hdrs.videoObjectLayerWidth + 15) >> 4;
                    pDecContainer->VopDesc.vopHeight =
                        (pDecContainer->Hdrs.videoObjectLayerHeight + 15) >> 4;
                    pDecContainer->VopDesc.totalMbInVop =
                        pDecContainer->VopDesc.vopWidth *
                        pDecContainer->VopDesc.vopHeight;

                    pDecContainer->StrmStorage.strmDecReady = HANTRO_TRUE;

                    /* store low_delay flag and image dimensions to see if they
                     * change */
                    pDecContainer->StrmStorage.lowDelay =
                        pDecContainer->Hdrs.lowDelay;
                    pDecContainer->StrmStorage.videoObjectLayerWidth =
                        pDecContainer->Hdrs.videoObjectLayerWidth;
                    pDecContainer->StrmStorage.videoObjectLayerHeight =
                        pDecContainer->Hdrs.videoObjectLayerHeight;

                    if(IS_END_OF_STREAM(pDecContainer))
                        return (DEC_HDRS_RDY);
                    else
                        return (DEC_HDRS_RDY_BUF_NOT_EMPTY);
                }
                /* check if short video header -> continue decoding.
                 * Otherwise return ontrol */
                /*
                else if(StrmDec_ShowBits(pDecContainer, 22) != SC_SV_START)
                {
                    return (DEC_RDY);
                }
                */
            }
            /* set flag to prevent concealment of vop (lost just repeated
             * header information and should be able to decode following
             * vops with no problems) */
            else if(status != HANTRO_OK)
            {
                headerFailure = HANTRO_TRUE;
            }
            break;

        case SC_GVOP_START:
            status = StrmDec_DecodeGovHeader(pDecContainer);
            break;

        case SC_VOP_START:
            /*MP4DEC_DEBUG(("before vop decode pDecCont->MbSetDesc.pCtrl %x %x\n",
             * (u32)pDecContainer->MbSetDesc.pCtrlDataAddr,
             * *pDecContainer->MbSetDesc.pCtrlDataAddr)); */
            if (pDecContainer->rlcMode)
            {
                status = StrmDec_DecodeVop(pDecContainer);
                if(pDecContainer->VopDesc.vopCodingType == BVOP &&
                   status == HANTRO_OK)
                {
                    if(pDecContainer->Hdrs.lowDelay)
                        return(DEC_VOP_SUPRISE_B);
                    else
                        return(DEC_VOP_HDR_RDY);
                }
            }
            else
            {
                status = StrmDec_DecodeVopHeader(pDecContainer);
                if(status == HANTRO_OK)
                {
                    if(pDecContainer->Hdrs.lowDelay &&
                       pDecContainer->VopDesc.vopCodingType == BVOP)
                    {
                        return(DEC_VOP_SUPRISE_B);
                    }
                    else
                        return(DEC_VOP_HDR_RDY);
                }
            }
            /*MP4DEC_DEBUG(("after vop decode pDecCont->MbSetDesc.pCtrl %x %x\n",
             * (u32)pDecContainer->MbSetDesc.pCtrlDataAddr,
             * *pDecContainer->MbSetDesc.pCtrlDataAddr)); */
            break;

        case SC_RESYNC:
            /* for REVIEW */
            /* TODO Decode only if rlcMode */
            if(pDecContainer->StrmStorage.shortVideo == HANTRO_TRUE)
            {
                pDecContainer->StrmStorage.gobResyncFlag = HANTRO_TRUE;

                /* TODO change, */
                /* error situation */
                status = StrmDec_DecodeGobLayer(pDecContainer); /* REMOVE */
            }
            else
            {
                status = StrmDec_DecodeVideoPacket(pDecContainer);
            }
            break;

        case SC_SV_START:
            /* Re-init workarounds for H263/SVH data */
            InitWorkarounds(2, &pDecContainer->workarounds);
            /* TODO remove */
            if (pDecContainer->rlcMode)
            {
                status = StrmDec_DecodeShortVideo(pDecContainer);
            }
            else
            {
                status = StrmDec_DecodeShortVideoHeader(pDecContainer);
                if (status == HANTRO_OK &&
                    pDecContainer->StrmStorage.strmDecReady == HANTRO_TRUE)
                {
                    return(DEC_VOP_HDR_RDY);
                }
            }
            if(pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE)
            {
                MP4DEC_DEBUG(("pDecContainer->SvDesc.sourceFormat %d \n",
                       pDecContainer->SvDesc.sourceFormat));
                /* first visit and source format set -> return */
                if((status == HANTRO_OK) && pDecContainer->SvDesc.sourceFormat)
                {
                    pDecContainer->StrmStorage.strmDecReady = HANTRO_TRUE;
                    return (DEC_HDRS_RDY_BUF_NOT_EMPTY);
                }
                else
                {
                    pDecContainer->StrmStorage.status = STATE_SYNC_LOST;
                    if(status == END_OF_STREAM)
                    {
                        return (DEC_ERROR);
                    }
                    else
                        return (DEC_ERROR_BUF_NOT_EMPTY);
                }
            }
            break;

        case SC_SORENSON_START:
            status = StrmDec_DecodeSorensonSparkHeader(pDecContainer);
            if (status == HANTRO_OK &&
                pDecContainer->StrmStorage.strmDecReady == HANTRO_TRUE)
                return(DEC_VOP_HDR_RDY);
            if(pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE)
            {
                MP4DEC_DEBUG(("pDecContainer->SvDesc.sourceFormat %d \n",
                       pDecContainer->SvDesc.sourceFormat));
                /* first visit and source format set -> return */
                if(status == HANTRO_OK)
                {
                    pDecContainer->StrmStorage.strmDecReady = HANTRO_TRUE;
                    return (DEC_HDRS_RDY_BUF_NOT_EMPTY);
                }
                else
                {
                    pDecContainer->StrmStorage.status = STATE_SYNC_LOST;
                    if(status == END_OF_STREAM)
                        return (DEC_ERROR);
                    else
                        return (DEC_ERROR_BUF_NOT_EMPTY);
                }
            }
            break;

        case SC_SV_END:
            /* remove stuffing */
            status = HANTRO_OK;
            if(pDecContainer->StrmDesc.bitPosInWord)
            {
                tmp = StrmDec_GetBits(pDecContainer,
                                      8 -
                                      pDecContainer->StrmDesc.bitPosInWord);
#ifdef HANTRO_PEDANTIC_MODE
                if(tmp)
                {
                    status = HANTRO_NOK;
                }
#endif
            }
            break;

        case SC_VOS_END:
            return (DEC_VOS_END);

        case SC_NOT_FOUND:
            if(pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE)
            {
                pDecContainer->StrmStorage.status = STATE_SYNC_LOST;
                if(IS_END_OF_STREAM(pDecContainer))
                    return (DEC_ERROR);
                else
                    return (DEC_ERROR_BUF_NOT_EMPTY);
            }
            /* start code not found and decoding short video stream ->
             * try to decode gob layer */ /* TODO REMOVE, RESYNC TO SV_HEADER*/
            else if(pDecContainer->rlcMode &&
                    pDecContainer->StrmStorage.shortVideo == HANTRO_TRUE)
            {
                pDecContainer->StrmStorage.gobResyncFlag = HANTRO_FALSE;
                status = StrmDec_DecodeGobLayer(pDecContainer);
            }
            else
            {
                status = HANTRO_NOK;
            }
            break;

        default:
            if(pDecContainer->StrmStorage.strmDecReady == HANTRO_FALSE)
            {
                pDecContainer->StrmStorage.status = STATE_SYNC_LOST;
                if(IS_END_OF_STREAM(pDecContainer))
                    return (DEC_ERROR);
                else
                    return (DEC_ERROR_BUF_NOT_EMPTY);
            }
            status = HANTRO_NOK;
        }

        if(status != HANTRO_OK)
        {
            if (!pDecContainer->rlcMode &&
                (startCode == SC_VOP_START || startCode == SC_SV_START))
            {
                return(DEC_VOP_HDR_RDY_ERROR);
            }
            pDecContainer->StrmStorage.status = STATE_SYNC_LOST;
        }
        /* status HANTRO_OK in all cases below */
        /* decoding ok, whole VOP decoded -> return DEC_VOP_RDY */
        else if((pDecContainer->StrmStorage.strmDecReady == HANTRO_TRUE) &&
                (pDecContainer->StrmStorage.vpMbNumber ==
                 pDecContainer->VopDesc.totalMbInVop))
        {
            if(IS_END_OF_STREAM(pDecContainer))
            {
                return (DEC_VOP_RDY);
            }
            else
            {
                return (DEC_VOP_RDY_BUF_NOT_EMPTY);
            }
        }
        /* decoding ok but stream ends -> get away */
        else if(IS_END_OF_STREAM(pDecContainer))
        {
            return (DEC_RDY);
        }
    }
    /*lint -e(506) */ while(1);

    /* execution never reaches this point (hope so) */
    /*lint -e(527) */ return (DEC_END_OF_STREAM);
    /*lint -restore */
}

/*------------------------------------------------------------------------------

   5.3  Function name: StrmDec_PrepareConcealment

        Purpose: Set data needed by error concealment and call concealment

        Input:
            Pointer to DecContainer structure
                -uses and updates StrmStorage
                -uses and updates StrmDesc
                -uses VopDesc
                -updates MBDesc
                -uses MbSetDesc
            startCode

        Output:
            CONTINUE to indicate that decoding should be continued
            DEC_VOP_RDY to indicate that decoding of VOP was finished

------------------------------------------------------------------------------*/

u32 StrmDec_PrepareConcealment(DecContainer * pDecContainer, u32 startCode)
{

    u32 i;
    u32 tmp;
    u32 first, last;

    /* first mb to be concealed */
    first = pDecContainer->StrmStorage.vpMbNumber;

    if(startCode == SC_RESYNC)
    {
        /* determine macro block number of next video packet */
        if(pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE)
        {
            last = StrmDec_CheckNextVpMbNumber(pDecContainer);
        }
        else
        {
            last = StrmDec_CheckNextGobNumber(pDecContainer) *
                pDecContainer->VopDesc.vopWidth;
        }

        /* last equal to zero -> incorrect number found from stream, cannot
         * do anything */
        if(!last)
        {
            return (CONTINUE);
        }

        /* mb number less than previous by macro_blocks_in_vop/8 or start code
         * loss detected (expection: if first is zero -> previous vop was
         * successfully decoded and we may have lost e.g. GVOP start code and
         * header) -> consider VOP start code and header lost, conceal rest of
         * the VOP. Beginning of next VOP will be concealed on next invocation
         * -> status left as STATE_SYNC_LOST */
        if(((last < first) &&
            ((first - last) >
             (pDecContainer->VopDesc.totalMbInVop >> 3))) ||
           (first &&
            (pDecContainer->StrmStorage.startCodeLoss == HANTRO_TRUE)))
        {
            /* conceal rest of the vop */
            last = pDecContainer->VopDesc.totalMbInVop;
            /* return resync marker into stream */
            (void)StrmDec_UnFlushBits(pDecContainer,
                                      pDecContainer->StrmStorage.
                                      resyncMarkerLength);
            /* set last sync to beginning of marker -> will be found next
             * time (starting search before last sync point prohibited) */
            pDecContainer->StrmStorage.pLastSync =
                pDecContainer->StrmDesc.pStrmCurrPos;

        }
        /* cannot conceal -> continue and conceal at next sync point */
        else if(last < first)
        {
            return (CONTINUE);
        }
        /* macro block number in the stream matches expected -> continue
         * decoding without concealment */
        else if(last == first)
        {
            pDecContainer->StrmStorage.status = HANTRO_OK;
            pDecContainer->MbSetDesc.pRlcDataCurrAddr =
                pDecContainer->MbSetDesc.pRlcDataVpAddr;
            pDecContainer->MbSetDesc.oddRlc =
                pDecContainer->MbSetDesc.oddRlcVp;
            return (CONTINUE);
        }

        /* data partitioned and first partition ok -> texture lost
         * (vpNumMbs set by PartitionedMotionTexture decoding
         * function right after detection of motion/dc marker) */
        if(pDecContainer->Hdrs.dataPartitioned &&
           ((pDecContainer->StrmStorage.vpMbNumber +
             pDecContainer->StrmStorage.vpNumMbs) == last))
            tmp = 0x1;
        /* otherwise everything lost */
        else
            tmp = 0x3;

        /* set error status for all macro blocks to be concealed */
        for(i = first; i < last; i++)
            pDecContainer->MBDesc[i].errorStatus = tmp;

        /*last = pDecContainer->VopDesc.totalMbInVop;*/
        (void)StrmDec_ErrorConcealment(pDecContainer, first, last - 1);

        /* set rlc buffer pointer to beginning of concealed area */
        if(pDecContainer->MbSetDesc.pRlcDataCurrAddr !=
           pDecContainer->MbSetDesc.pRlcDataAddr)
        {
            pDecContainer->MbSetDesc.pRlcDataCurrAddr =
                pDecContainer->MbSetDesc.pRlcDataVpAddr;
            pDecContainer->MbSetDesc.oddRlc =
                pDecContainer->MbSetDesc.oddRlcVp;
            /* half of the word written with corrupted data -> reset */
            if (pDecContainer->MbSetDesc.oddRlc){
                *pDecContainer->MbSetDesc.pRlcDataCurrAddr &= 0xFFFF0000;
            }
        }

        if(last == pDecContainer->VopDesc.totalMbInVop)
        {
            return (DEC_VOP_RDY);
        }
        else
        {
            pDecContainer->StrmStorage.vpMbNumber = last;
            pDecContainer->StrmStorage.vpNumMbs = 0;
            pDecContainer->StrmStorage.status = STATE_OK;
            return (CONTINUE);
        }
    }
    /* start code (or END_OF_STREAM) */
    else
    {
        /* conceal rest of the vop and return DEC_VOP_RDY */
        last = pDecContainer->VopDesc.totalMbInVop;

        pDecContainer->VopDesc.vopCoded = HANTRO_TRUE;

        /* return start code into stream */
        if((startCode == SC_SV_START) || (startCode == SC_SV_END))
        {
            (void)StrmDec_UnFlushBits(pDecContainer, 22);
        }
        /* start code value END_OF_STREAM if nothing was found (resync markers
         * disabled -> conceal) */
        else if(startCode != END_OF_STREAM)
        {
            (void)StrmDec_UnFlushBits(pDecContainer, 32);
        }

        /* data partitioned and first partition ok -> texture lost
         * (vpNumMbs set by PartitionedMotionTexture decoding
         * function right after detection of motion/dc marker) */
        if(pDecContainer->Hdrs.dataPartitioned &&
           ((pDecContainer->StrmStorage.vpMbNumber +
             pDecContainer->StrmStorage.vpNumMbs) == last))
            tmp = 0x1;
        /* otherwise everything lost */
        else
            tmp = 0x3;

        /* set error status for all macro blocks to be concealed */
        for(i = first; i < last; i++)
        {
            pDecContainer->MBDesc[i].errorStatus = tmp;
        }

        (void)StrmDec_ErrorConcealment(pDecContainer, first, last - 1);

        /* set rlc buffer pointer to beginning of concealed area */
        if(pDecContainer->MbSetDesc.pRlcDataCurrAddr !=
           pDecContainer->MbSetDesc.pRlcDataAddr)
        {
            pDecContainer->MbSetDesc.pRlcDataCurrAddr =
                pDecContainer->MbSetDesc.pRlcDataVpAddr;
            pDecContainer->MbSetDesc.oddRlc =
                pDecContainer->MbSetDesc.oddRlcVp;
        }

        pDecContainer->StrmStorage.status = STATE_OK;

        return (DEC_VOP_RDY);
    }
}
