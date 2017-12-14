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
--  Abstract : Motion-texture decoding functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_motiontexture.c,v $
--  $Date: 2008/01/17 14:13:51 $
--  $Revision: 1.5 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_DecodeCombinedMT
        5.2     StrmDec_DecodePartitionedIVop
        5.3     StrmDec_DecodePartitionedPVop
        5.4     StrmDec_DecodeMotionTexture
        5.5     StrmDec_UseIntraDcVlc
        5.6     StrmDec_DecodeMb

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mp4dechwd_motiontexture.h"
#include "mp4dechwd_vlc.h"
#include "mp4dechwd_utils.h"
#include "mp4dechwd_rvlc.h"

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

enum
{
    MOTION_MARKER = 0x1F001,/* 17 bits */
    DC_MARKER     = 0x6B001 /* 19 bits */
};

/* masks defining bits not to be changed when writing to ctrl register. Each
 * '1' in the mask means that bit value will not be changed. See AMBAIFD spec
 * for meaning of the bits (table 2-12) */
#define MASK1   0x000001FF  /* don't change bits [8,0] */

/* Mask 2: don't change VPBI, INTRA/INTER,DCincluded, QP, all BS bits */
#define MASK2   (((u32)1<<ASICPOS_MBTYPE)|((u32)1<<ASICPOS_VPBI)| \
                ((u32)1<<ASICPOS_USEINTRADCVLC)|((u32)0x1F<<ASICPOS_QP)| \
                0xFFFF)

#define MASK3   0x000001FF  /* don't change bits [8,0] */

#define ASIC_VPBI ((u32)1<<ASICPOS_VPBI)
#define ASIC_INTER ((u32)1<<ASICPOS_MBTYPE)
#define ASIC_INTER_VPBI1 (((u32)1<<ASICPOS_VPBI)|((u32)1<<ASICPOS_MBTYPE))

#define ASIC_SEPAR_DC ((u32)1<<ASICPOS_USEINTRADCVLC)

/* macro to write value into asic control bits register. 2 addresses per block
 * first for motion vectors. '1's in mask determine bits not to be changed. */
#define WRITE_ASIC_CTRL_BITS(mb,offset,value,mask) \
    pDecContainer->MbSetDesc.pCtrlDataAddr[NBR_OF_WORDS_MB*(mb) + offset] = \
    (pDecContainer->MbSetDesc.pCtrlDataAddr[NBR_OF_WORDS_MB*(mb) + offset] \
     & mask) | (value);

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/* value dquant[i] added to QP when value of dquant is i */
static const i32 dquant[4] = { -1, -2, 1, 2 };

/* intra DC vlc used at threshold i if QP less than value intraDcQp[i] */
static const u32 intraDcQp[8] = { 32, 13, 15, 17, 19, 21, 23, 0 };
const u8 asicPosNoRlc[6] = { 27, 26, 25, 24, 23, 22 };

u32 StrmDec_DecodeCombinedMT(DecContainer * pDecContainer);
u32 StrmDec_DecodePartitionedIVop(DecContainer * pDecContainer);
u32 StrmDec_DecodePartitionedPVop(DecContainer * pDecContainer);
u32 StrmDec_UseIntraDcVlc(DecContainer * pDecContainer, u32 mbNumber);

/*------------------------------------------------------------------------------

   5.1  Function name: StrmDec_DecodeCombinedMT

        Purpose: Decode combined motion texture data

        Input:
            Pointer to DecContainer structure

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeCombinedMT(DecContainer * pDecContainer)
{

    u32 tmp = 0;
    u32 mbNumber = 0;
    u32 mbCounter = 0;

    MP4DEC_API_DEBUG(("entry: StrmDec_DecodeCombinedMT \n"));

    mbNumber = pDecContainer->StrmStorage.vpMbNumber;
    pDecContainer->StrmStorage.vpFirstCodedMb = mbNumber;

    do
    {
        tmp = StrmDec_DecodeMb(pDecContainer, mbNumber);
        if(tmp != HANTRO_OK)
            return (tmp);

        if(!MB_IS_STUFFING(mbNumber))
        {
            mbNumber++;
            mbCounter++;
            /* read remaining stuffing macro block if end of VOP */
            if(mbNumber == pDecContainer->VopDesc.totalMbInVop)
            {
                tmp = 9 + (u32)(pDecContainer->VopDesc.vopCodingType == PVOP);
                while(StrmDec_ShowBits(pDecContainer, tmp) == 0x1)
                    (void) StrmDec_FlushBits(pDecContainer, tmp);
            }
        }
    }
    while( (mbNumber < pDecContainer->VopDesc.totalMbInVop) &&
           ( (StrmDec_CheckStuffing(pDecContainer) != HANTRO_OK) ||
             StrmDec_ShowBitsAligned(pDecContainer, 16, 1) ) );

    pDecContainer->StrmStorage.vpNumMbs = mbCounter;

    MP4DEC_API_DEBUG(("StrmDec_DecodeCombinedMT \n"));
    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name: StrmDec_DecodePartitionedIVop

        Purpose: Decode data partitioned motion texture data of I-VOP

        Input:
            Pointer to DecContainer structure

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodePartitionedIVop(DecContainer * pDecContainer)
{

    u32 inputBuffer;
    u32 usedBits;
    u32 i = 0, j, tmp, tmpMask;
    u32 mbNumber, mbCounter;
    i32 qP;
    i32 dcCoeff;
    u32 coded;
    u32 controlBits;
    u32 *pu32;
    u32 status = HANTRO_OK;
    u32 dcTmp0 = 0, dcTmp1 = 0;

    MP4DEC_API_DEBUG(("entry: StrmDec_DecodePartitionedIVop # \n"));
    mbCounter = 0;
    mbNumber = pDecContainer->StrmStorage.vpMbNumber;
    pDecContainer->StrmStorage.vpFirstCodedMb = mbNumber;
    pDecContainer->StrmStorage.vpNumMbs = 0;

    do
    {
        SHOWBITS32(inputBuffer);
        usedBits = 0;
        controlBits = 0;
        /* set video packet boundary flag if first mb of video packet */
        if(mbCounter == 0)
        {
            controlBits |= ASIC_VPBI;
        }

        status = StrmDec_DecodeMcbpc(pDecContainer, mbNumber,
                                        (inputBuffer << usedBits) >> 23,
                                        &usedBits);
        if(status != HANTRO_OK)
            return (status);

        if(MB_IS_STUFFING(mbNumber))
        {
            FLUSHBITS(usedBits);
            usedBits = 0;
            continue;
        }

        pDecContainer->StrmStorage.prevQP =
            pDecContainer->StrmStorage.qP;
        if(pDecContainer->MBDesc[mbNumber].typeOfMB == MB_INTRAQ)
        {
            tmp = (inputBuffer << usedBits) >> 30;  /* dquant */
            usedBits += 2;

            qP =
                (i32) (pDecContainer->StrmStorage.qP) + dquant[tmp];
            SATURATE(1, qP, 31);
            pDecContainer->StrmStorage.qP = (u32) qP;
        }

        FLUSHBITS(usedBits);
        usedBits = 0;

        controlBits |= pDecContainer->StrmStorage.qP << ASICPOS_QP;

        pu32 = pDecContainer->MbSetDesc.pCtrlDataAddr +
            NBR_OF_WORDS_MB * mbNumber;
        if(StrmDec_UseIntraDcVlc(pDecContainer, mbNumber))
        {
            controlBits |= ASIC_SEPAR_DC;   /* if true == coded separately */
            dcTmp0 = dcTmp1 = 0;
            for(j = 0; j < 6; j++)
            {
                status =
                    StrmDec_DecodeDcCoeff(pDecContainer, mbNumber, j, &dcCoeff);
                if(status != HANTRO_OK)
                {
                    return (status);
                }

                if (j < 3)
                    dcTmp0 = (dcTmp0 << 10) | (dcCoeff & 0x3ff);
                else
                    dcTmp1 = (dcTmp1 << 10) | (dcCoeff & 0x3ff);
            }

            /*MP4DEC_API_DEBUG(("datap ivop DC %d %d  #\n", dcTmp0, dcTmp1)); */
            pDecContainer->MbSetDesc.
                pDcCoeffDataAddr[mbNumber * NBR_DC_WORDS_MB] = dcTmp0;
            pDecContainer->MbSetDesc.
                pDcCoeffDataAddr[mbNumber * NBR_DC_WORDS_MB + 1] = dcTmp1;

        }

        *pu32 = controlBits;
        mbCounter++;
        mbNumber++;

        /* last macro block of vop -> check stuffing macro blocks */
        if(mbNumber == pDecContainer->VopDesc.totalMbInVop)
        {
            while(StrmDec_ShowBits(pDecContainer, 9) == 0x1)
            {
                tmp = StrmDec_FlushBits(pDecContainer, 9);
                if(tmp == END_OF_STREAM)
                    return (END_OF_STREAM);
            }
            if(StrmDec_ShowBits(pDecContainer, 19) == DC_MARKER)
            {
                break;
            }
            else
            {
                return (HANTRO_NOK);
            }
        }

    }
    while(StrmDec_ShowBits(pDecContainer, 19) != DC_MARKER);

    status = StrmDec_FlushBits(pDecContainer, 19);
    if(status != HANTRO_OK)
        return (status);

    pDecContainer->StrmStorage.vpNumMbs = mbCounter;

    mbNumber = pDecContainer->StrmStorage.vpMbNumber;

    SHOWBITS32(inputBuffer);
    usedBits = 0;

    for(i = mbNumber; i < (mbNumber + mbCounter); i++)
    {
        if(usedBits > 25)
        {
            FLUSHBITS(usedBits);
            usedBits = 0;
            SHOWBITS32(inputBuffer);
        }
        controlBits = 0;
        /* ac prediction flag */
        tmp = (inputBuffer << usedBits) >> 31;
        usedBits++;
#ifdef ASIC_TRACE_SUPPORT
        if (tmp)
            trace_mpeg4DecodingTools.acPred = 1;
#endif
        controlBits |= tmp << ASICPOS_ACPREDFLAG;

        status = StrmDec_DecodeCbpy(pDecContainer, i,
                                       (inputBuffer << usedBits) >> 26,
                                       &usedBits);
        if(status != HANTRO_OK)
            return (status);

        /* write ASIC control bits */
        for(j = 0; j < 6; j++)
        {
            coded =
                !((pDecContainer->StrmStorage.
                   codedBits[i] >> (5 - j)) & 0x1);
            controlBits |= (coded << asicPosNoRlc[j]);
            /*WRITE_ASIC_CTRL_BITS(i,2*j+1,tmp,MASK2) moved couple lines ; */
        }
        WRITE_ASIC_CTRL_BITS(i, 0 /*2*j */ , controlBits, MASK2);

    }
    FLUSHBITS(usedBits);

    if(pDecContainer->Hdrs.reversibleVlc)
    {
        status = StrmDec_DecodeRvlc(pDecContainer, mbNumber, mbCounter);
        if(status != HANTRO_OK)
            return (status);
    }
    else
    {
        for(i = mbNumber; i < (mbNumber + mbCounter); i++)
        {
            /* initialize mask for block number 0 */
            tmpMask = 0x20;
            for(j = 0; j < 6; j++)
            {
                if(pDecContainer->StrmStorage.codedBits[i] & tmpMask)
                {
                    status = StrmDec_DecodeVlcBlock(pDecContainer, i, j);
                    if(status != HANTRO_OK)
                        return (status);
                }
                tmpMask >>= 1;
            }
        }
    }
    return (status);

}

/*------------------------------------------------------------------------------

   5.3  Function name: StrmDec_DecodePartitionedPVop

        Purpose: Decode data partitioned motion texture data of P-VOP

        Input:
            Pointer to DecContainer structure

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodePartitionedPVop(DecContainer * pDecContainer)
{

    u32 inputBuffer;
    u32 usedBits;
    u32 h, i, j, tmp, tmpMask;
    u32 mbNumber, mbCounter;
    i32 qP;
    i32 dcCoeff;
    u32 controlBits;
    u32 coded;
    u32 intraDcVlc;
    u32 *pu32;
    u32 status = HANTRO_OK;
    u32 dcTmp0 = 0, dcTmp1 = 0;

    MP4DEC_API_DEBUG(("Decode PartitionedP #\n"));

    mbCounter = 0;
    mbNumber = pDecContainer->StrmStorage.vpMbNumber;
    pDecContainer->StrmStorage.vpFirstCodedMb = mbNumber;
    pDecContainer->StrmStorage.vpNumMbs = 0;

    do
    {

        SHOWBITS32(inputBuffer);
        usedBits = 0;
        tmp = inputBuffer >> 31;    /* not_coded */
        usedBits++;

        if(!tmp)
        {
            status = StrmDec_DecodeMcbpc(pDecContainer, mbNumber,
                                            (inputBuffer << usedBits) >>
                                            23, &usedBits);
            if(status != HANTRO_OK)
                return (status);
            FLUSHBITS(usedBits);
            usedBits = 0;

            if(pDecContainer->MBDesc[mbNumber].typeOfMB == MB_STUFFING)
            {
                continue;
            }

            if(MB_IS_INTER(mbNumber))
            {
                status = StrmDec_DecodeMv(pDecContainer, mbNumber);
                if(status != HANTRO_OK)
                    return (status);
            }
            else
            {
                /* write 0 motion vectors to asic control bits */
                pDecContainer->MbSetDesc.
                    pMvDataAddr[mbNumber * NBR_MV_WORDS_MB]
                    = ASIC_ZERO_MOTION_VECTORS_LUM;
            }
        }
        /* not coded -> increment first coded if necessary */
        else
        {
            FLUSHBITS(usedBits);
            usedBits = 0;
            if(mbNumber == pDecContainer->StrmStorage.vpFirstCodedMb)
            {
                pDecContainer->StrmStorage.vpFirstCodedMb++;
            }
            pDecContainer->StrmStorage.codedBits[mbNumber] =
                MB_NOT_CODED;
            pDecContainer->MBDesc[mbNumber].typeOfMB = MB_INTER;

            pDecContainer->MbSetDesc.pMvDataAddr[mbNumber * NBR_MV_WORDS_MB] =
                ASIC_ZERO_MOTION_VECTORS_LUM;

        }

        mbCounter++;
        mbNumber++;

        /* last macro block of vop -> check stuffing macro blocks */
        if(mbNumber == pDecContainer->VopDesc.totalMbInVop)
        {
            while(StrmDec_ShowBits(pDecContainer, 10) == 0x1)
            {
                tmp = StrmDec_FlushBits(pDecContainer, 10);
                if(tmp == END_OF_STREAM)
                {
                    return (END_OF_STREAM);
                }
            }
            if(StrmDec_ShowBits(pDecContainer, 17) == MOTION_MARKER)
            {
                break;
            }
            else
            {
                return (HANTRO_NOK);
            }
        }

    }
    while(StrmDec_ShowBits(pDecContainer, 17) != MOTION_MARKER);

    status = StrmDec_FlushBits(pDecContainer, 17);
    if(status != HANTRO_OK)
        return (status);

    pDecContainer->StrmStorage.vpNumMbs = mbCounter;

    mbNumber = pDecContainer->StrmStorage.vpMbNumber;

    SHOWBITS32(inputBuffer);
    usedBits = 0;
    for(i = mbNumber; i < (mbNumber + mbCounter); i++)
    {
        if(usedBits > 23)
        {
            FLUSHBITS(usedBits);
            usedBits = 0;
            SHOWBITS32(inputBuffer);
        }
        controlBits = 0;
        /* set video packet boundary flag if first mb of video packet */
        if(i == mbNumber)
        {

            controlBits |= (ASIC_VPBI);
            /*SET_CONCEALMENT_POINTERS(); */
        }
        if(MB_IS_INTER(i))
        {
            controlBits |= ((u32) 0x1 << (ASICPOS_MBTYPE));
        }

        pu32 = pDecContainer->MbSetDesc.pCtrlDataAddr
            + NBR_OF_WORDS_MB * i;
        if(pDecContainer->StrmStorage.codedBits[i] != MB_NOT_CODED)
        {
            /* ac prediction flag */
            if(MB_IS_INTRA(i))
            {
                tmp = (inputBuffer << usedBits) >> 31;
                usedBits++;
#ifdef ASIC_TRACE_SUPPORT
                if (tmp)
                    trace_mpeg4DecodingTools.acPred = 1;
#endif
                controlBits |= (tmp << ASICPOS_ACPREDFLAG);
            }

            status = StrmDec_DecodeCbpy(pDecContainer, i,
                                           (inputBuffer << usedBits) >> 26,
                                           &usedBits);
            if(status != HANTRO_OK)
                return (status);

            pDecContainer->StrmStorage.prevQP =
                pDecContainer->StrmStorage.qP;
            if(MB_HAS_DQUANT(i))
            {
                tmp = (inputBuffer << usedBits) >> 30;  /* dquant */
                usedBits += 2;

                qP = (i32) (pDecContainer->StrmStorage.qP) +
                    dquant[tmp];
                SATURATE(1, qP, 31);
                pDecContainer->StrmStorage.qP = (u32) qP;
            }

            controlBits |= pDecContainer->StrmStorage.qP << ASICPOS_QP;

            intraDcVlc = MB_IS_INTRA(i) &&
                StrmDec_UseIntraDcVlc(pDecContainer, i);
            if(intraDcVlc)
            {
                FLUSHBITS(usedBits);
                usedBits = 0;
                controlBits |= ASIC_SEPAR_DC;
            }
            else
            {
                controlBits &= ~(ASIC_SEPAR_DC);
            }
            if(MB_IS_INTER4V(i))
            {
                controlBits |= (0x1 << (ASICPOS_4MV));
            }

            dcTmp0 = dcTmp1 = 0;
            for(j = 0; j < 6; j++)
            {

                if(intraDcVlc)
                {
                    status = StrmDec_DecodeDcCoeff(pDecContainer, i, j,
                        &dcCoeff);
                    if(status != HANTRO_OK)
                        return (status);

                    if (j < 3)
                        dcTmp0 = (dcTmp0 << 10) | (dcCoeff & 0x3ff);
                    else
                        dcTmp1 = (dcTmp1 << 10) | (dcCoeff & 0x3ff);
                }
                coded =
                    !((pDecContainer->StrmStorage.
                       codedBits[i] >> (5 - j)) & 0x1);
                controlBits |= (coded << asicPosNoRlc[j]);
            }
            *pu32 = controlBits;

            if(intraDcVlc)
            {
                /*MP4DEC_API_DEBUG(("combined ivop DC %d %d  #\n", dcTmp0, dcTmp1)); */
                SHOWBITS32(inputBuffer);
                /* write DC coeffs to interface */
                pDecContainer->MbSetDesc.
                    pDcCoeffDataAddr[i * NBR_DC_WORDS_MB] = dcTmp0;
                pDecContainer->MbSetDesc.
                    pDcCoeffDataAddr[i * NBR_DC_WORDS_MB + 1] = dcTmp1;
            }
        }

        else    /* not coded mb */
        {
            controlBits |= (1 << ASICPOS_MBNOTCODED);
            controlBits |= pDecContainer->StrmStorage.qP << ASICPOS_QP;
            for(h = 0; h < 6; h++)
            {
                controlBits |= (1 << asicPosNoRlc[h]);
            }
            /* write asic control bits */
            *pu32 = controlBits;
            pu32 += NBR_OF_WORDS_MB;

        }

    }
    if(usedBits)
        FLUSHBITS(usedBits);

    if(pDecContainer->Hdrs.reversibleVlc)
    {
        status = StrmDec_DecodeRvlc(pDecContainer, mbNumber, mbCounter);
        if(status != HANTRO_OK)
            return (status);
    }
    else
    {
        for(i = mbNumber; i < (mbNumber + mbCounter); i++)
        {
            if(pDecContainer->StrmStorage.codedBits[i] != MB_NOT_CODED)
            {
                /* initialize mask for block number 0 */
                tmpMask = 0x20;
                for(j = 0; j < 6; j++)
                {
                    if(pDecContainer->StrmStorage.codedBits[i] & tmpMask)
                    {
                        status = StrmDec_DecodeVlcBlock(pDecContainer, i, j);
                        if(status != HANTRO_OK)
                            return (status);
                    }
                    tmpMask >>= 1;
                }
            }
        }
    }

    return (status);

}

/*------------------------------------------------------------------------------

   5.4  Function name: StrmDec_DecodeMotionTexture

        Purpose: Decode data partitioned motion texture data

        Input:
            Pointer to DecContainer structure

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeMotionTexture(DecContainer * pDecContainer)
{

    u32 status = HANTRO_OK;

    if(pDecContainer->Hdrs.dataPartitioned)
    {
        if(pDecContainer->VopDesc.vopCodingType == IVOP)
            status = StrmDec_DecodePartitionedIVop(pDecContainer);
        else
            status = StrmDec_DecodePartitionedPVop(pDecContainer);
    }
    else
    {
        /* TODO REMOVE */
        status = StrmDec_DecodeCombinedMT(pDecContainer);
    }

    return (status);

}

/*------------------------------------------------------------------------------

   5.4  Function name: StrmDec_UseIntraDcVlc

        Purpose: determine whether to use intra DC vlc or not

        Input:
            Pointer to DecContainer structure
                -uses StrmStorage
            u32 MbNumber

        Output:
            0 don't use intra dc vlc
            1 use intra dc vlc

------------------------------------------------------------------------------*/

u32 StrmDec_UseIntraDcVlc(DecContainer * pDecContainer, u32 mbNumber)
{

    u32 qP;

    if(mbNumber == pDecContainer->StrmStorage.vpFirstCodedMb)
    {
        qP = pDecContainer->StrmStorage.qP;
    }
    else
    {
        qP = pDecContainer->StrmStorage.prevQP;
    }

    ASSERT((qP > 0) && (qP < 32));
    ASSERT(pDecContainer->VopDesc.intraDcVlcThr < 8);

    if(qP < intraDcQp[pDecContainer->VopDesc.intraDcVlcThr])
    {
        return (1);
    }
    else
    {
        return (0);
    }

}

/*------------------------------------------------------------------------------

   5.6  Function name:
                    StrmDec_DecodeMb

        Purpose:
                    Decodes Macro block from stream

        Input:
                    pointer to DecContainer

        Output:
                    status (HANTRO_OK/NOK/END_OF_STREAM)

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeMb(DecContainer * pDecContainer, u32 mbNumber)
{
    u32 flushRet = 0;
    u32 btmp = 0;
    u32 asicTmp;
    u32 inputBuffer;
    u32 usedBits;
    u32 *pasicCtrl;
    u32 notCoded;
    u32 acPredFlag;
    u32 status;
    u32 useIntraDcVlc;
    u32 dQuant;
    u32 i;
    i32 qP;
    u32 codedBlock;
    i32 dcCoeff;
    u32 dcTmp0 = 0, dcTmp1 = 0;

    /*MP4DEC_API_DEBUG(("StrmDec_DecodeMb #\n")); */
    pasicCtrl = &(pDecContainer->MbSetDesc.
                      pCtrlDataAddr[NBR_OF_WORDS_MB * mbNumber]);
    SHOWBITS32(inputBuffer);

    if(pDecContainer->VopDesc.vopCodingType == PVOP)
    {
        notCoded = inputBuffer >> 31;
        usedBits = 1;
    }
    else
    {
        notCoded = 0;
        usedBits = 0;
    }

    if(!notCoded)
    {
        status = StrmDec_DecodeMcbpc(pDecContainer, mbNumber,
                                        (inputBuffer << usedBits) >> 23,
                                        &usedBits);
        if(status != HANTRO_OK)
            return (status);
        /* CHANGED BY JANSA 2407 */
        if(pDecContainer->MBDesc[mbNumber].typeOfMB == MB_STUFFING)
        {
            status = StrmDec_FlushBits(pDecContainer, usedBits);
            return (HANTRO_OK);
        }
    }

    if(notCoded)
    {
        FLUSHBITS(usedBits);
        /* 1st in Video Packet? and Inter Mb info */
        if(((mbNumber == pDecContainer->StrmStorage.vpMbNumber) &&
            (pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE)) ||
           !mbNumber)
        {
            asicTmp = ASIC_INTER_VPBI1;
            for(i = 0; i < 6; i++)
            {
                asicTmp |= (1 << asicPosNoRlc[i]);
            }
            /*SET_CONCEALMENT_POINTERS(); */
        }
        else
        {
            asicTmp = ASIC_INTER;
            for(i = 0; i < 6; i++)
            {
                asicTmp |= (1 << asicPosNoRlc[i]);
            }
        }
        if(!mbNumber)
        {

            asicTmp |= (1U << ASICPOS_VPBI);
            /*SET_CONCEALMENT_POINTERS(); */
        }
        if((mbNumber == pDecContainer->StrmStorage.vpMbNumber) &&
           (pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE))
        {

            asicTmp = asicTmp | (1U << ASICPOS_VPBI);
            /*SET_CONCEALMENT_POINTERS(); */

        }
        else if((mbNumber == pDecContainer->StrmStorage.vpMbNumber) &&
                (pDecContainer->StrmStorage.gobResyncFlag == HANTRO_TRUE))
        {

            asicTmp = asicTmp | (1U << ASICPOS_VPBI);
            /*SET_CONCEALMENT_POINTERS(); */
        }

        if(pDecContainer->StrmStorage.vpFirstCodedMb == mbNumber)
            pDecContainer->StrmStorage.vpFirstCodedMb++;
        asicTmp |= (pDecContainer->StrmStorage.qP << ASICPOS_QP);
        asicTmp |= (1 << ASICPOS_MBNOTCODED);

        *pasicCtrl = asicTmp;

        /* write ctrl bits to memory and return(HANTRO_OK) */
        pDecContainer->MbSetDesc.pMvDataAddr[NBR_MV_WORDS_MB * mbNumber] =
            ASIC_ZERO_MOTION_VECTORS_LUM;

        /* this is needed for differential motion vector decoding */
        pDecContainer->StrmStorage.codedBits[mbNumber] = MB_NOT_CODED;
        pDecContainer->MBDesc[mbNumber].typeOfMB = MB_INTER;

        /* check if stuffing macro blocks follow */
        while(StrmDec_ShowBits(pDecContainer, 10) == 0x01)
        {
            flushRet = StrmDec_FlushBits(pDecContainer, 10);
            if(flushRet != HANTRO_OK)
                return HANTRO_NOK;
        }

        return (HANTRO_OK);
    }

    /* svh and mbType 2 */

    if(pDecContainer->StrmStorage.shortVideo &&
       (pDecContainer->MBDesc[mbNumber].typeOfMB == MB_INTER4V))
        return (HANTRO_NOK);

    asicTmp = 0;
    if(MB_IS_INTER(mbNumber))
    {
        asicTmp = ASIC_INTER;

        if(MB_IS_INTER4V(mbNumber))
        {
            asicTmp |= (0x1 << ASICPOS_4MV);
        }
    }
    if(!mbNumber)
    {
        asicTmp = asicTmp | (1U << ASICPOS_VPBI);
        /*SET_CONCEALMENT_POINTERS(); */
    }
    if((mbNumber == pDecContainer->StrmStorage.vpMbNumber) &&
       (pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE))
    {

        asicTmp = asicTmp | (1U << ASICPOS_VPBI);
        /*SET_CONCEALMENT_POINTERS(); */

    }
    else if((mbNumber == pDecContainer->StrmStorage.vpMbNumber) &&
            (pDecContainer->StrmStorage.gobResyncFlag == HANTRO_TRUE))
    {

        asicTmp = asicTmp | (1U << ASICPOS_VPBI);
        /*SET_CONCEALMENT_POINTERS(); */
    }

    if(MB_IS_INTER(mbNumber) || pDecContainer->StrmStorage.shortVideo)
        acPredFlag = 0;
    else
    {
        acPredFlag = (inputBuffer << usedBits) >> 31;
        usedBits++;
    }
    asicTmp = asicTmp | (acPredFlag << ASICPOS_ACPREDFLAG);

#ifdef ASIC_TRACE_SUPPORT
    if (acPredFlag)
        trace_mpeg4DecodingTools.acPred = 1;
#endif

    status = StrmDec_DecodeCbpy(pDecContainer, mbNumber,
                                   (inputBuffer << usedBits) >> 26,
                                   &usedBits);
    if(status != HANTRO_OK)
        return (status);

    pDecContainer->StrmStorage.prevQP = pDecContainer->StrmStorage.qP;
    if(MB_HAS_DQUANT(mbNumber))
    {
        dQuant = (inputBuffer << usedBits) >> 30;
        usedBits += 2;
        qP = (i32) pDecContainer->StrmStorage.qP +
            dquant[dQuant];
        SATURATE(1, qP, 31);
        pDecContainer->StrmStorage.qP = (u32) qP;
    }
    asicTmp =
        asicTmp | (pDecContainer->StrmStorage.qP << ASICPOS_QP);

    if(MB_IS_INTER(mbNumber))
        useIntraDcVlc = 0;
    else
    {
        useIntraDcVlc = StrmDec_UseIntraDcVlc(pDecContainer, mbNumber)
            || pDecContainer->StrmStorage.shortVideo;
        if(useIntraDcVlc)
        {
            useIntraDcVlc = 1;
        }
        asicTmp =
            asicTmp | (useIntraDcVlc << ASICPOS_USEINTRADCVLC);
    }
    FLUSHBITS(usedBits);

    if(MB_IS_INTER(mbNumber))
    {
        status = StrmDec_DecodeMv(pDecContainer, mbNumber);
        if(status != HANTRO_OK)
            return (status);
    }

    *pasicCtrl = asicTmp;
    for(i = 0; i < 6; i++)
    {

        codedBlock = (pDecContainer->StrmStorage.codedBits[mbNumber]
                          & (0x20 >> i)) >> (5 - i);
        if(useIntraDcVlc)
        {
            status =
                StrmDec_DecodeDcCoeff(pDecContainer, mbNumber, i, &dcCoeff);
            if(status != HANTRO_OK)
                return (status);

            if (i < 3)
                dcTmp0 = (dcTmp0 << 10) | (dcCoeff & 0x3ff);
            else
                dcTmp1 = (dcTmp1 << 10) | (dcCoeff & 0x3ff);

        }
        btmp = !(codedBlock & 0x1);
        asicTmp |= (btmp << asicPosNoRlc[i]);
        if(codedBlock)
        {
            status = StrmDec_DecodeVlcBlock(pDecContainer, mbNumber, i);
            if(status != HANTRO_OK)
                return (status);
        }
    }

    *pasicCtrl = asicTmp;
    if(useIntraDcVlc)
    {
        SHOWBITS32(inputBuffer);
        /*MP4DEC_API_DEBUG(("combined ivop DC %d %d  #\n", dcTmp0, dcTmp1)); */
        /* write DC coeffs to interface */
        pDecContainer->MbSetDesc.
            pDcCoeffDataAddr[NBR_DC_WORDS_MB * mbNumber] = dcTmp0;
        pDecContainer->MbSetDesc.
            pDcCoeffDataAddr[NBR_DC_WORDS_MB * mbNumber + 1] = dcTmp1;
    }

    return (HANTRO_OK);
}
