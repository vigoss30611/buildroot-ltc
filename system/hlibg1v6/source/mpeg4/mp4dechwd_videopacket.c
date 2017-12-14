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
--  Abstract : Video packet decoding functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_videopacket.c,v $
--  $Date: 2009/04/02 12:10:57 $
--  $Revision: 1.4 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_DecodeVideoPacketHeader
        5.2     StrmDec_DecodeVideoPacket

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mp4dechwd_videopacket.h"
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

/*------------------------------------------------------------------------------

   5.1  Function name: StrmDec_DecodeVideoPacketHeader

        Purpose: Decode video packet header

        Input:
            Pointer to DecContainer structure
                -uses and updates VopDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeVideoPacketHeader(DecContainer * pDecContainer)
{

    u32 i, tmp;
    i32 itmp;

    pDecContainer->StrmStorage.vpNumMbs = 0;
    MP4DEC_DEBUG(("Decoding video packet header\n"));

    /* length of macro_block_number determined by size of the VOP in mbs */
    tmp = StrmDec_NumBits(pDecContainer->VopDesc.totalMbInVop - 1);
    tmp = StrmDec_GetBits(pDecContainer, tmp);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);

    /* erroneous macro block number */
    if(tmp != pDecContainer->StrmStorage.vpMbNumber)
    {
        return (HANTRO_NOK);
    }

    /* QP */
    tmp = StrmDec_GetBits(pDecContainer, 5);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    if(tmp == 0)
    {
        return (HANTRO_NOK);
    }

    pDecContainer->StrmStorage.qP = tmp;
    pDecContainer->StrmStorage.prevQP = tmp;
    pDecContainer->StrmStorage.vpQP = tmp;

    /* HEC */
    tmp = StrmDec_GetBits(pDecContainer, 1);
    if(tmp == END_OF_STREAM)
        return (END_OF_STREAM);
    
#ifdef ASIC_TRACE_SUPPORT
    if (tmp)
        trace_mpeg4DecodingTools.hdrExtensionCode = 1;
#endif

    /* decode header extension. Values are used only if vop header was
     * lost for any reason. Otherwise values are just compared to ones
     * received in vop header and errors are reported if they do not match */
    if(tmp)
    {
        /* modulo time base */
        i = 0;
        while((tmp = StrmDec_GetBits(pDecContainer, 1)) == 1)
            i++;

        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);

        /* update time codes if vop header not valid */
        if(pDecContainer->StrmStorage.validVopHeader == HANTRO_FALSE)
        {
            pDecContainer->VopDesc.timeCodeSeconds += i;
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
            pDecContainer->VopDesc.moduloTimeBase = i;
        }
        else if(i != pDecContainer->VopDesc.moduloTimeBase)
        {
            return (HANTRO_NOK);
        }

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

        /* number of bits needed to represent
         * [0,vop_time_increment_resolution) */
        i = StrmDec_NumBits(pDecContainer->Hdrs.vopTimeIncrementResolution
                            - 1);
        tmp = StrmDec_GetBits(pDecContainer, i);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(tmp >= pDecContainer->Hdrs.vopTimeIncrementResolution)
        {
            return (HANTRO_NOK);
        }

        if(pDecContainer->StrmStorage.validVopHeader == HANTRO_FALSE)
        {
            /* compute tics since previous picture */
            itmp =
                (i32) tmp - (i32) pDecContainer->VopDesc.vopTimeIncrement +
                (i32) pDecContainer->VopDesc.moduloTimeBase *
                pDecContainer->Hdrs.vopTimeIncrementResolution;

            pDecContainer->VopDesc.ticsFromPrev = (itmp >= 0) ? itmp :
                (itmp + pDecContainer->Hdrs.vopTimeIncrementResolution);
            if(pDecContainer->StrmStorage.govTimeIncrement)
            {
                pDecContainer->VopDesc.ticsFromPrev +=
                    pDecContainer->StrmStorage.govTimeIncrement;
                pDecContainer->StrmStorage.govTimeIncrement = 0;
            }
            pDecContainer->VopDesc.vopTimeIncrement = tmp;
        }
        else if(tmp != pDecContainer->VopDesc.vopTimeIncrement)
        {
            return (HANTRO_NOK);
        }

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

        tmp = StrmDec_GetBits(pDecContainer, 2);    /* vop_coding_type */
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        if(pDecContainer->StrmStorage.validVopHeader == HANTRO_TRUE)
        {
            if(tmp != pDecContainer->VopDesc.vopCodingType)
            {
                return (HANTRO_NOK);
            }
        }
        else
        {
            if((tmp != IVOP) && (tmp != PVOP))
            {
                return (HANTRO_NOK);
            }
            else
            {
                pDecContainer->VopDesc.vopCodingType = tmp;
            }
        }

        tmp = StrmDec_GetBits(pDecContainer, 3);    /* intra_dc_vlc_thr */
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);

        if(pDecContainer->StrmStorage.validVopHeader == HANTRO_TRUE)
        {
            if(tmp != pDecContainer->VopDesc.intraDcVlcThr)
            {
                return (HANTRO_NOK);
            }
        }
        else
        {
            pDecContainer->VopDesc.intraDcVlcThr = tmp;
        }

        if(pDecContainer->VopDesc.vopCodingType != IVOP)
        {
            tmp = StrmDec_GetBits(pDecContainer, 3);    /* vop_fcode_fwd */
            if(tmp == END_OF_STREAM)
                return (END_OF_STREAM);
            if(pDecContainer->StrmStorage.validVopHeader == HANTRO_TRUE)
            {
                if(tmp != pDecContainer->VopDesc.fcodeFwd)
                {
                    return (HANTRO_NOK);
                }
            }
            else
            {
                if(tmp == 0)
                {
                    return (HANTRO_NOK);
                }
                else
                {
                    pDecContainer->VopDesc.fcodeFwd = tmp;
                }
            }

            if(pDecContainer->VopDesc.vopCodingType == BVOP)
            {
                tmp = StrmDec_GetBits(pDecContainer, 3);    /* vop_fcode_backward */
                if(tmp == END_OF_STREAM)
                    return (END_OF_STREAM);
                if(pDecContainer->StrmStorage.validVopHeader == HANTRO_TRUE)
                {
                    if(tmp != pDecContainer->VopDesc.fcodeBwd)
                    {
                        return (HANTRO_NOK);
                    }
                }
                else
                {
                    if(tmp == 0)
                    {
                        return (HANTRO_NOK);
                    }
                    else
                    {
                        pDecContainer->VopDesc.fcodeBwd = tmp;
                    }
                }



            }


        }
        else
        {
            /* set vop_fcode_fwd of intra VOP to 1 for resync marker length
             * computation */
            pDecContainer->VopDesc.fcodeFwd = 1;
        }

        pDecContainer->StrmStorage.resyncMarkerLength =
            pDecContainer->VopDesc.fcodeFwd + 16;

        if(pDecContainer->StrmStorage.validVopHeader == HANTRO_FALSE)
        {
            pDecContainer->StrmStorage.validVopHeader = HANTRO_TRUE;
        }
    }

    if(pDecContainer->StrmStorage.validVopHeader == HANTRO_TRUE)
    {
        return (HANTRO_OK);
    }
    else
    {
        return (HANTRO_NOK);
    }
}

/*------------------------------------------------------------------------------

   5.2  Function name: StrmDec_DecodeVideoPacket

        Purpose: Decode video packet

        Input:
            Pointer to DecContainer structure
                -uses and updates VopDesc

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeVideoPacket(DecContainer * pDecContainer)
{

    u32 tmp;
    u32 status = HANTRO_OK;
    status = StrmDec_DecodeVideoPacketHeader(pDecContainer);
    if(status != HANTRO_OK)
        return (status);

    status = StrmDec_DecodeMotionTexture(pDecContainer);
    if(status != HANTRO_OK)
        return (status);

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
            /* ignore anything that follows after VOP end */
           (((pDecContainer->StrmStorage.vpMbNumber +
              pDecContainer->StrmStorage.vpNumMbs) !=
             pDecContainer->VopDesc.totalMbInVop) &&
            ((tmp >> (32 - pDecContainer->StrmStorage.resyncMarkerLength))
             != 0x01)))
        {
            return (HANTRO_NOK);
        }
    }
    else if(!IS_END_OF_STREAM(pDecContainer))
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

   5.3  Function name: StrmDec_CheckNextVpMbNumber

        Purpose:

        Input:
            Pointer to DecContainer structure
                -uses VopDesc

        Output:
            macro block number
            0 if non-valid number found in stream

------------------------------------------------------------------------------*/

u32 StrmDec_CheckNextVpMbNumber(DecContainer * pDecContainer)
{

    u32 tmp;
    u32 mbNumber;

    tmp = StrmDec_NumBits(pDecContainer->VopDesc.totalMbInVop - 1);

    mbNumber = StrmDec_ShowBits(pDecContainer, tmp);

    /* use zero value to indicate non valid macro block number */
    if(mbNumber >= pDecContainer->VopDesc.totalMbInVop)
    {
        mbNumber = 0;
    }

    return (mbNumber);

}
