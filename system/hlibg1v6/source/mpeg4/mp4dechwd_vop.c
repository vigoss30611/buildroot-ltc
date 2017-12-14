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
--  Abstract : VOP decoding functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_vop.c,v $
--  $Date: 2010/03/31 08:55:00 $
--  $Revision: 1.5 $
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_DecodeVopHeader
        5.2     StrmDec_DecodeVop
        5.3     StrmDec_ReadVopComplexity

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mp4dechwd_vop.h"
#include "mp4dechwd_utils.h"
#include "mp4dechwd_motiontexture.h"
#include "mp4debug.h"
#ifdef ASIC_TRACE_SUPPORT
#include "trace.h"
#endif
/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

u32 StrmDec_ReadVopComplexity(DecContainer * pDecContainer);

/*------------------------------------------------------------------------------

   5.1  Function name: StrmDec_DecodeVopHeader

        Purpose: Decode VideoObjectPlane header

        Input:
            Pointer to DecContainer structure
                -uses and updates VopDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeVopHeader(DecContainer * pDecContainer)
{

    u32 i, tmp;
    i32 itmp;
    u32 moduloTimeBase, vopTimeIncrement;
    u32 status = HANTRO_OK;

    /* check that previous vop was finished, if it wasnt't return start code
     * into the stream and get away */
    if(pDecContainer->StrmStorage.vpMbNumber)
    {
        (void) StrmDec_UnFlushBits(pDecContainer, 32);
        pDecContainer->StrmStorage.pLastSync =
            pDecContainer->StrmDesc.pStrmCurrPos;
        return (HANTRO_NOK);
    }
    if (pDecContainer->rlcMode)
        *pDecContainer->MbSetDesc.pCtrlDataAddr = 0;

    /* vop header is non valid until successively decoded */
    pDecContainer->StrmStorage.validVopHeader = HANTRO_FALSE;

    pDecContainer->StrmStorage.vpMbNumber = 0;
    pDecContainer->StrmStorage.vpNumMbs = 0;

    pDecContainer->StrmStorage.resyncMarkerLength = 0;

    tmp = StrmDec_GetBits(pDecContainer, 2);    /* vop coding type */
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    if((tmp != IVOP) && (tmp != PVOP) && (tmp != BVOP))
    {
        return (HANTRO_NOK);
    }

    pDecContainer->VopDesc.vopCodingType = tmp;

#if 0
#ifdef ASIC_TRACE_SUPPORT
    if (pDecContainer->VopDesc.vopCodingType == IVOP)
        trace_mpeg4DecodingTools.picCodingType.i_coded = 1;
    else if (pDecContainer->VopDesc.vopCodingType == PVOP)
        trace_mpeg4DecodingTools.picCodingType.p_coded = 1;
    else if (pDecContainer->VopDesc.vopCodingType == BVOP)
        trace_mpeg4DecodingTools.picCodingType.b_coded = 1;
#endif
#endif

    /* Time codes updated at the end of function so that they won't be updated
     * twice if header is lost (updated if vop header lost and header extension
     * received in video packet header) */

    /* modulo time base */
    moduloTimeBase = 0;
    while((tmp = StrmDec_GetBits(pDecContainer, 1)) == 1)
        moduloTimeBase++;

    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    /* marker */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
    if(tmp == 0)
    {
        return (HANTRO_NOK);
    }
#endif /* HANTRO_PEDANTIC_MODE */

    /* number of bits needed to represent [0,vop_time_increment_resolution) */
    i = StrmDec_NumBits(pDecContainer->Hdrs.vopTimeIncrementResolution - 1);
    vopTimeIncrement = StrmDec_GetBits(pDecContainer, i);
    if(vopTimeIncrement == END_OF_STREAM)
        return (END_OF_STREAM);

#ifdef HANTRO_PEDANTIC_MODE
    if(vopTimeIncrement >=
       pDecContainer->Hdrs.vopTimeIncrementResolution)
    {
        return (HANTRO_NOK);
    }
#endif /* HANTRO_PEDANTIC_MODE */

    /* marker */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
#ifdef HANTRO_PEDANTIC_MODE
    if(tmp == 0)
    {
        return (HANTRO_NOK);
    }
#endif /* HANTRO_PEDANTIC_MODE */

    /* vop coded */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    pDecContainer->VopDesc.vopCoded = tmp;

    if(pDecContainer->VopDesc.vopCoded)
    {
        /* vop rounding type */
        if(pDecContainer->VopDesc.vopCodingType == PVOP)
        {
            tmp = StrmDec_GetBits(pDecContainer, 1);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
        }
        else
        {
            tmp = 0;
        }
        pDecContainer->VopDesc.vopRoundingType = tmp;

        if(pDecContainer->Hdrs.complexityEstimationDisable == HANTRO_FALSE)
        {
            status = StrmDec_ReadVopComplexity(pDecContainer);
            if(status != HANTRO_OK)
                return (status);
        }

        tmp = StrmDec_GetBits(pDecContainer, 3);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        pDecContainer->VopDesc.intraDcVlcThr = tmp;

        if(pDecContainer->Hdrs.interlaced)
        {
            tmp = StrmDec_GetBits(pDecContainer, 1);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
            pDecContainer->VopDesc.topFieldFirst = tmp;

            tmp = StrmDec_GetBits(pDecContainer, 1);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
            pDecContainer->VopDesc.altVerticalScanFlag = tmp;

        }


        pDecContainer->VopDesc.qP = StrmDec_GetBits(pDecContainer, 5);
        if(pDecContainer->VopDesc.qP == END_OF_STREAM)
            return (END_OF_STREAM);
        if(pDecContainer->VopDesc.qP == 0)
        {
            return (HANTRO_NOK);
        }
        pDecContainer->StrmStorage.qP = pDecContainer->VopDesc.qP;
        pDecContainer->StrmStorage.prevQP = pDecContainer->VopDesc.qP;
        pDecContainer->StrmStorage.vpQP = pDecContainer->VopDesc.qP;

        if(pDecContainer->VopDesc.vopCodingType != IVOP)
        {
            tmp = StrmDec_GetBits(pDecContainer, 3);
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
            if(tmp == 0)
            {
                return (HANTRO_NOK);
            }
            pDecContainer->VopDesc.fcodeFwd = tmp;

            if(pDecContainer->VopDesc.vopCodingType == BVOP)
            {
                tmp = StrmDec_GetBits(pDecContainer, 3);
                if(tmp == END_OF_STREAM)
                    return (END_OF_STREAM);
                if(tmp == 0)
                {
                    return (HANTRO_NOK);
                }
                pDecContainer->VopDesc.fcodeBwd = tmp;
            }

        }
        else
        {
            /* set vop_fcode_fwd of intra VOP to 1 for resync marker length
             * computation */
            pDecContainer->VopDesc.fcodeFwd = 1;
        }
    }

    pDecContainer->StrmStorage.resyncMarkerLength =
        pDecContainer->VopDesc.fcodeFwd + 16;

    if (pDecContainer->VopDesc.vopCodingType != BVOP)
    {
        /* update time codes */
        pDecContainer->VopDesc.timeCodeSeconds += moduloTimeBase;
        /* to support modulo_time_base values higher than 60 -> while */
        while(pDecContainer->VopDesc.timeCodeSeconds >= 60)
        {
            pDecContainer->VopDesc.timeCodeSeconds -= 60;
            pDecContainer->VopDesc.timeCodeMinutes++;
            if(pDecContainer->VopDesc.timeCodeMinutes >= 60)
            {
                pDecContainer->VopDesc.timeCodeMinutes -= 60;
                pDecContainer->VopDesc.timeCodeHours++;
            }
        }
        /* compute tics since previous picture */
        itmp = (i32) vopTimeIncrement -
            (i32) pDecContainer->VopDesc.vopTimeIncrement +
            (i32) moduloTimeBase *
            (i32) pDecContainer->Hdrs.vopTimeIncrementResolution;
        pDecContainer->VopDesc.ticsFromPrev = (itmp >= 0) ? itmp :
            (itmp + pDecContainer->Hdrs.vopTimeIncrementResolution);
        if(pDecContainer->StrmStorage.govTimeIncrement)
        {
            pDecContainer->VopDesc.ticsFromPrev +=
                pDecContainer->StrmStorage.govTimeIncrement;
            pDecContainer->StrmStorage.govTimeIncrement = 0;
        }
        pDecContainer->VopDesc.prevVopTimeIncrement =
            pDecContainer->VopDesc.vopTimeIncrement;
        pDecContainer->VopDesc.prevModuloTimeBase =
            pDecContainer->VopDesc.moduloTimeBase;

        pDecContainer->VopDesc.vopTimeIncrement = vopTimeIncrement;
        pDecContainer->VopDesc.moduloTimeBase = moduloTimeBase;
    }
    else
    {
        itmp = (i32) vopTimeIncrement -
            (i32) pDecContainer->VopDesc.prevVopTimeIncrement +
            (i32) moduloTimeBase *
            (i32) pDecContainer->Hdrs.vopTimeIncrementResolution;
        pDecContainer->VopDesc.trb = (itmp >= 0) ? itmp :
            (itmp + pDecContainer->Hdrs.vopTimeIncrementResolution);

        pDecContainer->VopDesc.trd = pDecContainer->VopDesc.ticsFromPrev;
    }

    /* everything ok this far -> set vop header valid. If vop was not coded
     * don't set the flag -> if there were errors in vop header decoding they
     * don't limit decoding of header extension in video packets. */
    if(pDecContainer->VopDesc.vopCoded)
    {
        pDecContainer->StrmStorage.validVopHeader = HANTRO_TRUE;
    }

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name: StrmDec_DecodeVop

        Purpose: Decode VideoObjectPlane

        Input:
            Pointer to DecContainer structure
                -uses and updates VopDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeVop(DecContainer * pDecContainer)
{

    u32 tmp;
    u32 status = HANTRO_OK;

    /*ASSERT(pDecContainer->Hdrs.dataPartitioned);*/

    status = StrmDec_DecodeVopHeader(pDecContainer);
    if(status != HANTRO_OK)
        return (status);

    if(pDecContainer->VopDesc.vopCoded)
    {
        if(pDecContainer->VopDesc.vopCodingType == BVOP)
        {
            pDecContainer->rlcMode = 0;
            return HANTRO_OK;
        }
        else
            pDecContainer->rlcMode = 1;


        status = StrmDec_DecodeMotionTexture(pDecContainer);

        if(status != HANTRO_OK)
            return (status);
    }
    else    /* not coded vop */
    {
        pDecContainer->StrmStorage.vpNumMbs =
            pDecContainer->VopDesc.totalMbInVop;
    }

    status = StrmDec_GetStuffing(pDecContainer);
    if(status != HANTRO_OK)
        return (status);

    /* there might be extra stuffing byte if next start code is video
     * object sequence start or end code */
    tmp = StrmDec_ShowBitsAligned(pDecContainer, 32, 1);
    if((tmp == SC_VOS_START) || (tmp == SC_VOS_END) ||
       (tmp == 0 && StrmDec_ShowBits(pDecContainer, 8) == 0x7F) )
    {
        tmp = StrmDec_GetStuffing(pDecContainer);
        if(tmp != HANTRO_OK)
            return (tmp);
    }

    /* stuffing ok -> check that there is at least 23 zeros after last
     * macroblock and either resync marker or 32 zeros (END_OF_STREAM) after
     * any other macroblock */
    tmp = StrmDec_ShowBits(pDecContainer, 32);

    /* JanSa: modified to handle extra zeros after VOP */
    if ( !(tmp>>8) &&
          ((pDecContainer->StrmStorage.vpMbNumber +
            pDecContainer->StrmStorage.vpNumMbs) ==
           pDecContainer->VopDesc.totalMbInVop) )
    {
        do
        {
            if (StrmDec_FlushBits(pDecContainer, 8) == END_OF_STREAM)
                break;
            tmp = StrmDec_ShowBits(pDecContainer,32);
        } while (!(tmp>>8));
    }

    if(tmp)
    {
        if((tmp >> 9) &&
            /* ignore any stuff that follows VOP end */
           (((pDecContainer->StrmStorage.vpMbNumber +
              pDecContainer->StrmStorage.vpNumMbs) !=
             pDecContainer->VopDesc.totalMbInVop) &&
            ((tmp >> (32 - pDecContainer->StrmStorage.resyncMarkerLength))
             != 0x01)))
        {
            return (HANTRO_NOK);
        }
    }
    /* just zeros in the stream and not end of stream -> error */
    else if(!IS_END_OF_STREAM(pDecContainer))
    {
        return (HANTRO_NOK);
    }
    /* end of stream -> check that whole vop was decoded in case resync
     * markers are disabled */
    else if(pDecContainer->Hdrs.resyncMarkerDisable &&
            ((pDecContainer->StrmStorage.vpMbNumber +
              pDecContainer->StrmStorage.vpNumMbs) !=
             pDecContainer->VopDesc.totalMbInVop))
    {
        return (HANTRO_NOK);
    }

    /* whole video packet decoded and stuffing ok -> set vpMbNumber in
     * StrmStorage so that this video packet won't be touched/concealed
     * anymore. Also set VpQP to QP so that concealment will use qp of last
     * decoded macro block */
    pDecContainer->StrmStorage.vpMbNumber +=
        pDecContainer->StrmStorage.vpNumMbs;
    pDecContainer->StrmStorage.vpQP = pDecContainer->StrmStorage.qP;
    pDecContainer->MbSetDesc.pRlcDataVpAddr =
        pDecContainer->MbSetDesc.pRlcDataCurrAddr;
    pDecContainer->MbSetDesc.oddRlcVp =
        pDecContainer->MbSetDesc.oddRlc;

    pDecContainer->StrmStorage.vpNumMbs = 0;

    return (status);

}

/*------------------------------------------------------------------------------

   5.3  Function name: StrmDec_ReadVopComplexity

        Purpose: read vop complexity estimation header

        Input:
            Pointer to DecContainer structure
                -uses and updates VopDesc
                -uses Hdrs

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_ReadVopComplexity(DecContainer * pDecContainer)
{

    u32 tmp;

    tmp = 0;
    if(pDecContainer->Hdrs.estimationMethod == 0)
    {
        /* common stuff for I- and P-VOPs */
        if(pDecContainer->Hdrs.opaque)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.transparent)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.intraCae)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.interCae)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.noUpdate)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.upsampling)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.intraBlocks)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.notCodedBlocks)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.dctCoefs)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.dctLines)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        if(pDecContainer->Hdrs.vlcSymbols)
            tmp = StrmDec_GetBits(pDecContainer, 8);
        /* NOTE that this is just 4 bits long */
        if(pDecContainer->Hdrs.vlcBits)
            tmp = StrmDec_GetBits(pDecContainer, 4);

        if(pDecContainer->VopDesc.vopCodingType == IVOP)
        {
            if(pDecContainer->Hdrs.sadct)
                tmp = StrmDec_GetBits(pDecContainer, 8);
        }
        else    /* PVOP */
        {
            if(pDecContainer->Hdrs.interBlocks)
                tmp = StrmDec_GetBits(pDecContainer, 8);
            if(pDecContainer->Hdrs.inter4vBlocks)
                tmp = StrmDec_GetBits(pDecContainer, 8);
            if(pDecContainer->Hdrs.apm)
                tmp = StrmDec_GetBits(pDecContainer, 8);
            if(pDecContainer->Hdrs.npm)
                tmp = StrmDec_GetBits(pDecContainer, 8);
            if(pDecContainer->Hdrs.forwBackMcQ)
                tmp = StrmDec_GetBits(pDecContainer, 8);
            if(pDecContainer->Hdrs.halfpel2)
                tmp = StrmDec_GetBits(pDecContainer, 8);
            if(pDecContainer->Hdrs.halfpel4)
                tmp = StrmDec_GetBits(pDecContainer, 8);
            if(pDecContainer->Hdrs.sadct)
                tmp = StrmDec_GetBits(pDecContainer, 8);
            if(pDecContainer->Hdrs.quarterpel)
                tmp = StrmDec_GetBits(pDecContainer, 8);
        }
    }
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    return (HANTRO_OK);

}
