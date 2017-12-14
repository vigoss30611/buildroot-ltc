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
--  Description : Debug Trace funtions
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg4asicdbgtrace.c,v $
--  $Revision: 1.6 $
--  $Date: 2008/04/21 07:22:05 $
--
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "mp4dechwd_container.h"
#include "mp4deccfg.h"
#include "mp4decapi.h"
#include "mp4decdrv.h"
#include "mp4debug.h"

#include "mp4dechwd_utils.h"

#include <stdio.h>
#include <assert.h>

void MP4RegDump(const void *dwl)
{
#if 0
    MP4DEC_DEBUG(("GetMP4DecId   %x\n", GetMP4DecId(dwl)));
    MP4DEC_DEBUG(("GetMP4DecPictureWidth  %d\n", GetMP4DecPictureWidth(dwl)));
    MP4DEC_DEBUG(("GetMP4DecPictureHeight   %d\n", GetMP4DecPictureHeight(dwl)));
    MP4DEC_DEBUG(("GetMP4DecondingMode   %d\n", GetMP4DecDecondingMode(dwl)));
    MP4DEC_DEBUG(("GetMP4RoundingCtrl   %d\n", GetMP4DecRoundingCtrl(dwl)));
    MP4DEC_DEBUG(("GetMP4DecDisableOutputWrite   %d\n",
           GetMP4DecDisableOutputWrite(dwl)));
    MP4DEC_DEBUG(("GetMP4DecFilterDisable   %d\n", GetMP4DecFilterDisable(dwl)));
    MP4DEC_DEBUG(("GetMP4DecChromaQpOffset  %d\n", GetMP4DecChromaQpOffset(dwl)));
/*    MP4DEC_DEBUG(("GetMP4DecIrqEnable   %d\n",GetMP4DecIrqEnable(dwl)));*/
    MP4DEC_DEBUG(("GetMP4DecIrqStat   %d\n", GetMP4DecIrqStat(dwl)));
    MP4DEC_DEBUG(("GetMP4DecIrqLine   %d\n", GetMP4DecIrqLine(dwl)));
    MP4DEC_DEBUG(("GetMP4DecEnable   %d\n", GetMP4DecEnable(dwl)));
    MP4DEC_DEBUG(("GetMP4DecMbCtrlBaseAddress   %x\n",
           GetMP4DecMbCtrlBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecMvBaseAddress   %x\n", GetMP4DecMvBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecDcCompBaseAddress   %x\n",
           GetMP4DecDcCompBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecRlcBaseAddress   %x\n", GetMP4DecRlcBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecRefPictBaseAddress   %x\n",
           GetMP4DecRefPictBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecRlcAmount   %d\n", GetMP4DecRlcAmount(dwl)));
    MP4DEC_DEBUG(("GetMP4DecOutputPictBaseAddress   %x\n",
           GetMP4DecOutputPictBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecVopFCode   %d\n", GetMP4DecVopFCode(dwl)));
    MP4DEC_DEBUG(("GetMP4DecOutputPictEndian   %d\n", GetMP4DecOutputPictEndian(dwl)));
    MP4DEC_DEBUG(("GetMP4DecAmbaBurstLength   %d\n", GetMP4DecAmbaBurstLength(dwl)));
    MP4DEC_DEBUG(("GetMP4DecAsicServicePriority   %d\n",
           GetMP4DecAsicServicePriority(dwl)));
#endif
}

void WriteAsicCtrl(DecContainer * pDecContainer)
{
    FILE *fctrl = 0;
    FILE *fctrla = 0;
    FILE *fmv = 0;
    FILE *fdc = 0;
    FILE *fdcHex = 0;
    FILE *hexMotionVectors = 0;

    u32 i, j, tmp2, tmp = 0;
    i32 tmp3 = 0, horMv = 0, verMv = 0;
    u32 *pMv, *pDc;
    static u32 qpArray[MP4API_DEC_MBS];
    extern const u8 asicPosNoRlc[6];

    printf("AsicCtrl \n");
    fflush(stdout);
    fctrl = fopen("mbcontrol.hex", "at");
    fctrla = fopen("mbcontrol.trc", "at");
    fmv = fopen("motion_vectors.trc", "at");
    hexMotionVectors = fopen("motion_vectors.hex", "at");
    fdc = fopen("dc_separate_coeffs.trc", "at");
    fdcHex = fopen("dc_separate_coeffs.hex", "at");
    if(fctrl == NULL || fctrla == NULL ||
       fmv == NULL || fdc == NULL || hexMotionVectors == NULL || fdcHex == NULL)
    {
        return;
    }

    if(fctrl != NULL)
    {
        /*pDc = pDecContainer->MbSetDesc.pDcCoeffDataAddr;
         * pMv = pDecContainer->MbSetDesc.pMvDataAddr; */
        for(j = 0; j < pDecContainer->VopDesc.totalMbInVop; j++)
        {

            pDc = &pDecContainer->MbSetDesc.
                pDcCoeffDataAddr[NBR_DC_WORDS_MB * j];
            pMv = &pDecContainer->MbSetDesc.
                pMvDataAddr[NBR_MV_WORDS_MB * j];

            for(tmp = 0; tmp > 1000; tmp++)
            {
                *(pDecContainer->MbSetDesc.
                  pDcCoeffDataAddr + tmp) = 0;
            }
            /* HEX */

            /* invert bits for no RLC data */

            tmp = pDecContainer->MbSetDesc.
                pCtrlDataAddr[NBR_OF_WORDS_MB * j];

            fprintf(fctrl, "%08x\n",
                    pDecContainer->MbSetDesc.
                    pCtrlDataAddr[NBR_OF_WORDS_MB * j]);

            /* invert all block no rlc bits in a loop for each block */

            for(i = 0; i < 6; i++)
            {
                tmp ^= (0x1 << asicPosNoRlc[i]);

            }

            /* DEC */

            tmp2 = pDecContainer->MbSetDesc.
                pCtrlDataAddr[NBR_OF_WORDS_MB * j];

            /* NOTE! Also prints the separ DC / 4MV info */
            fprintf(fctrla, "%-3d", (tmp2 >> ASICPOS_4MV) & 0x3);
            fprintf(fctrla, "%-3d", (tmp2 >> ASICPOS_VPBI) & 0x1);
            fprintf(fctrla, "%-3d", (tmp2 >> ASICPOS_ACPREDFLAG) & 0x1);

            for(i = 0; i < 6; i++)
            {
                fprintf(fctrla, "%-3d", ((tmp2 >> asicPosNoRlc[i]) & 0x1));
            }

            fprintf(fctrla, "%-3d", (tmp2 >> ASICPOS_QP) & 0x1F);
            /*fprintf(fctrla, "%d", (tmp2) & 0x1FF); */

            fprintf(fctrla, "%-3d", (tmp2 >> ASICPOS_CONCEAL) & 0x1);

            fprintf(fctrla, "%-3d", (tmp2 >> ASICPOS_MBNOTCODED) & 0x1);

            if(pDecContainer->VopDesc.vopCoded)
                qpArray[j] = (tmp2 >> ASICPOS_QP) & 0x1F;

            fprintf(fctrla, "  picture = %d, mb = %d\n",
                    pDecContainer->VopDesc.vopNumber - 1, j);

            if((tmp2 & (1 << ASICPOS_USEINTRADCVLC)) && MB_IS_INTRA(j))
            {

                /* Separate DC */
                for(i = 0; i < 2; i++)
                {   /* TODO 1ff instead of 3ff ?  */
                    tmp = *pDc++;
                    fprintf(fdcHex, "%x\n",  tmp);
                    /* trick for the correct sign */
                    tmp3 = (tmp >> 20) & 0x1FF;
                    tmp3 <<= 23;
                    tmp3 >>= 23;
                    fprintf(fdc, "%-3d ", tmp3);
                    tmp3 = (tmp >> 10) & 0x1FF;
                    tmp3 <<= 23;
                    tmp3 >>= 23;
                    fprintf(fdc, "%-3d ", tmp3);
                    tmp3 = (tmp & 0x1FF);
                    tmp3 <<= 23;
                    tmp3 >>= 23;
                    fprintf(fdc, "%-3d ", tmp3);

                }
                fprintf(fdc, " picture = %d, mb = %-3d\n",
                        pDecContainer->VopDesc.vopNumber - 1, j);
            }
            else
            {
                for(i = 0; i < 6; i++)
                {
                    fprintf(fdc, "%-3d ", 0);
                    if(i&2)/* Do twice*/
                        fprintf(fdcHex, "%x\n",  0);

                }
                fprintf(fdc, " picture = %d, mb = %-3d\n",
                        pDecContainer->VopDesc.vopNumber - 1, j);
            }

            if(fmv && MB_IS_INTER(j))
            {

                if(MB_IS_INTER4V(j))
                {

                    /*MP4DEC_DEBUG(("mv pointer pMv %x\n", pMv)); */
                    for(i = 0; i < 4; i++)
                    {

                        if(hexMotionVectors)
                        {
                            fprintf(hexMotionVectors, "%x\n", *pMv);
                        }
                        tmp = *pMv++;

                        tmp2 = (tmp >> 17) & 0x7FFF;
                        if(tmp2 > 0x1FFF)
                        {
                            horMv = (i32) (((~0) << 14) | tmp2);
                        }
                        else
                        {
                            horMv = (i32) tmp2;
                        }

                        tmp2 = (tmp >> 4) & 0x1FFF;
                        if(tmp2 > 0x7FF)
                        {
                            verMv = (i32) (((~0) << 12) | tmp2);
                        }
                        else
                        {
                            verMv = (i32) tmp2;
                        }

                        fprintf(fmv, "%-3d ", horMv);
                        fprintf(fmv, "%-3d 0 ", verMv);
                        fprintf(fmv, "picture = %d, mb = %d\n",
                                pDecContainer->VopDesc.vopNumber - 1, j);

                    }
                }
                else
                {
                    for(i = 0; i < 1; i++)
                    {

                        tmp = *pMv++;

                        tmp2 = (tmp >> 17) & 0x7FFF;
                        if(tmp2 > 0x1FFF)
                        {
                            horMv = (i32) (((~0) << 14) | tmp2);
                        }
                        else
                        {
                            horMv = (i32) tmp2;
                        }

                        tmp2 = (tmp >> 4) & 0x1FFF;
                        if(tmp2 > 0x7FF)
                        {
                            verMv = (i32) (((~0) << 12) | tmp2);
                        }
                        else
                        {
                            verMv = (i32) tmp2;
                        }
                        for(i = 0; i < 4; i++)
                        {
                            if(hexMotionVectors)
                            {
                                fprintf(hexMotionVectors, "%x\n", *(pMv - 1));
                            }
                            fprintf(fmv, "%-3d ", horMv);
                            fprintf(fmv, "%-3d 0 ", verMv);
                            fprintf(fmv, "picture = %d, mb = %d\n",
                                    pDecContainer->VopDesc.vopNumber - 1,
                                    j);
                        }
                    }
                }

            }
            else
            {
                /* intra mb, write zero vectors */
                for(i = 0; i < 4; i++)
                {
                    if(hexMotionVectors)
                    {
                        fprintf(hexMotionVectors, "0\n");
                    }
                    fprintf(fmv, "0 ");
                    fprintf(fmv, "  0   0 ");
                    fprintf(fmv, "picture = %d, mb = %d\n",
                            pDecContainer->VopDesc.vopNumber - 1, j);
                }
            }
        }
    }
    if(fctrl)
        fclose(fctrl);
    if(fctrla)
        fclose(fctrla);
    if(fmv)
        fclose(fmv);
    if(fdc)
        fclose(fdc);
    if(fdcHex)
        fclose(fdcHex);
    if(hexMotionVectors)
        fclose(hexMotionVectors);
}

void WriteAsicRlc(DecContainer * pDecContainer, u32 * phalves, u32 * pnumAddr)
{
    FILE *frlc;
    FILE *frlca;

    u32 tmp, tmp2, i, coded = 0;
    u32 *ptr;
    static u32 count = 0;
    u32 putComment = 1;
    u32 mb = 0, block = 0;
    u32 checkDisable = 0;
    u32 halves = 0;
    u32 numAddr = 0;
    extern const u8 asicPosNoRlc[6];

    frlc = fopen("rlc.hex", "at");
    frlca = fopen("rlc.trc", "at");

    if(frlc == NULL || frlca == NULL)
    {
        return;
    }

    printf("AsicRlc\n");
    fflush(stdout);

    numAddr = 0;
    halves = 0;
    ptr = pDecContainer->MbSetDesc.pRlcDataAddr;

    /* compute number of addresses to be written */
/*    for (i=0;i<6*pDecContainer->MbSetDesc.
            nbrOfMbInMbSet;i++)
    {
        numAddr += (pDecContainer->ApiStorage.pNbrRlc[i])&0x3F;
    }*/
#if 0
    fprintf(frlc, "Picture = %d\n",
            /*
             * tmp2 & 0xFFFF, */
            pDecContainer->VopDesc.vopNumber - 1);
#endif

    numAddr = (numAddr + 0xf) & ~(0x7);

/*    for(i=1000;i<3500;i++){
        *(pDecContainer->MbSetDesc.pRlcDataAddr +i) = 0;
    }*/

    count += pDecContainer->MbSetDesc.
        pRlcDataCurrAddr - ptr;
    /*printf("Writing %d (%d) words, %d\n",pDecContainer->MbSetDesc.pRlcDataCurrAddr-ptr,
     * numAddr,count); */
    /*printf("addrs %d\nn",
     * (pDecContainer->MbSetDesc.pRlcDataCurrAddr+1 -ptr )); */

    while(ptr < pDecContainer->MbSetDesc.
          pRlcDataCurrAddr + 1)
        /*for (i = 0; i < (pDecContainer->MbSetDesc.pRlcDataCurrAddr-ptr)+3&3; i++)
         * for (i = 0; i < numAddr; i++) */
    {
        tmp2 = *ptr++;
        tmp2 = (tmp2 << 16) | (tmp2 >> 16);
        if(frlc != NULL &&
           ((((((u32) pDecContainer->MbSetDesc.
                pRlcDataCurrAddr -
                (u32) pDecContainer->MbSetDesc.
                pRlcDataAddr + 3) & (~3)) >> 1))))
        {
#if 0
            if(tmpmb != mb)
            {

                fprintf(frlc, "Picture = %d, mb %d\n",
                        pDecContainer->VopDesc.vopNumber - 1, mb);
                tmpmb = mb;
            }
#endif
            fprintf(frlc, "%x\n", (tmp2 << 16) | (tmp2 >> 16));
        }
        if(frlca != NULL &&
           ((((((u32) pDecContainer->MbSetDesc.
                pRlcDataCurrAddr -
                (u32) pDecContainer->MbSetDesc.
                pRlcDataAddr + 3) & (~3)) >> 1))))
        {

            if(putComment)
            {
                coded =
                    !((pDecContainer->MbSetDesc.
                       pCtrlDataAddr[NBR_OF_WORDS_MB *
                                     mb] >> asicPosNoRlc[block]) & 0x1);
                if(mb == pDecContainer->VopDesc.totalMbInVop)
                {
                    coded = 0;

                    /*fprintf(frlca, "make it zero\n"); */
                }   /*fprintf(frlca, "coded 1 %d\n", coded); */
            }

            while(!coded)
            {
                block++;
                if(block == 6)
                {
                    block = 0;
                    mb++;
                    /*fprintf(frlca, "mb++ %d\n", mb); */
                }
                coded =
                    !((pDecContainer->MbSetDesc.
                       pCtrlDataAddr[NBR_OF_WORDS_MB *
                                     mb] >> asicPosNoRlc[block]) & 0x1);

                /*fprintf(frlca, "coded2 %d\n", coded); */
                if(mb == pDecContainer->VopDesc.totalMbInVop)
                {
                    coded = 0;
                }

                if(mb == pDecContainer->VopDesc.totalMbInVop + 1)
                {
                    /*fprintf(frlca,"break1\n"); */
                    break;
                }
            }

            if(mb == pDecContainer->VopDesc.totalMbInVop + 1)
            {
                /*fprintf(frlca, "break2\n"); */
                break;
            }
            /* new block, put comment */
            if(putComment)
            {
                halves++;
                fprintf(frlca, "%-8d Picture = %d, MB = %d, block = %d\n", tmp2 & 0xFFFF,   /*
                                                                                             * tmp2 & 0xFFFF, */
                        pDecContainer->VopDesc.vopNumber - 1, mb, block++);

                putComment = 0;

                if(block == 6)
                {
                    block = 0;
                    mb++;
                    /*fprintf(frlca, "mb++ %d\n", mb); */
                }

            }
            else
            {

                halves++;
                fprintf(frlca, "%d\n", tmp2 & 0xFFFF /*, tmp2 & 0xFFFF */ );
            }

            /* if last, print comment next time */
            if((tmp2 & 0x8000) && (checkDisable == 0))
            {
                putComment = 1;
            }

            if(!(tmp2 & 0x1FF))
            {
                checkDisable = 1;
                /*fprintf(frlca, "Check Disable1\n"); */
            }
            else
            {
                checkDisable = 0;
            }

            if(putComment)
            {
                coded =
                    !((pDecContainer->MbSetDesc.
                       pCtrlDataAddr[NBR_OF_WORDS_MB *
                                     mb] >> asicPosNoRlc[block]) & 0x1);
                /*fprintf(frlca, "coded3 %d\n", coded); */
                if(mb == pDecContainer->VopDesc.totalMbInVop)
                {
                    coded = 0;

                    /*fprintf(frlca, "make it zero\n"); */
                }
            }

            while(!coded)
            {
                block++;
                if(block == 6)
                {
                    block = 0;
                    mb++;
                    /*fprintf(frlca, "mb++ %d\n", mb); */
                }
                coded =
                    !((pDecContainer->MbSetDesc.
                       pCtrlDataAddr[NBR_OF_WORDS_MB *
                                     mb] >> asicPosNoRlc[block]) & 0x1);

                /*fprintf(frlca, "coded4 %d\n", coded); */
                if(mb == pDecContainer->VopDesc.totalMbInVop)
                {
                    coded = 0;

                }
                if(mb == pDecContainer->VopDesc.totalMbInVop + 1)
                {

                    /*fprintf(frlca, "break3 mb%d\n", mb); */
                    break;
                }
            }
            if(mb == pDecContainer->VopDesc.totalMbInVop + 1)
            {
                /*fprintf(frlca, "break4, mb %d\n", mb); */
                break;
            }
            /* new block, put comment */
            if(putComment)
            {
                halves++;
                fprintf(frlca, "%-8d Picture = %d, MB = %d, block = %d\n", (tmp2 >> 16) & 0xFFFF,   /*
                                                                                                     * (tmp2 >> 16) & 0xFFFF, */
                        pDecContainer->VopDesc.vopNumber - 1, mb, block++);

                putComment = 0;

                if(block == 6)
                {
                    block = 0;
                    mb++;
                    /*fprintf(frlca, "mb++ %d\n", mb); */
                }
            }
            else
            {
                halves++;
                fprintf(frlca, "%d\n", (tmp2 >> 16) & 0xFFFF    /*,
                                                                 * (tmp2 >> 16) & 0xFFFF */ );
            }
            /* if last, print comment next time */
            if((tmp2 & 0x80000000) && (checkDisable == 0))
            {
                putComment = 1;
            }
            /* if big level, disable last check next time */
            if(!(tmp2 & 0x1FF0000))
            {
                checkDisable = 1;
                /*    fprintf(frlca, "Check Disable2\n"); */
            }
            else
            {
                checkDisable = 0;
            }

        }
/*            *(pDecContainer->MbSetDesc.pRlcDataCurrAddr-1) = 0;
            *(pDecContainer->MbSetDesc.pRlcDataCurrAddr-2) = 0;
            *(pDecContainer->MbSetDesc.pRlcDataCurrAddr-3) = 0;
            for(tmp =0; tmp < 3900 ; tmp++){
               * (pDecContainer->MbSetDesc.pRlcDataAddr+tmp) = 0;
            }
            *(pDecContainer->MbSetDesc.pRlcDataCurrAddr-4) = 0;*/
    }

    /* fprintf(frlc,"VOP end %d \n", pDecContainer->VopDesc.vopNumber - 1); */
/*printf("pDecContainer->VopDesc.totalMbInVop %d \n", pDecContainer->VopDesc.totalMbInVop);*/
    tmp = (halves + 3) & (~3);
    tmp2 = tmp - halves;

    /*printf("tmp2 %d, halves %d\n", tmp2, halves); */
    for(i = 0; i < tmp2; i++)
    {
        fprintf(frlca, "0\n");
    }
    halves = tmp;
    /*printf("numadd %d i %d\n", numAddr, i); */
    ASSERT(block < 6);
/*    ASSERT(mb <  pDecContainer->VopDesc.totalMbInVop);*/

    *phalves = halves;
    *pnumAddr = numAddr;
    /*printf("*tmp2 %x tmp2 %x\n", ptr, *ptr); */
    if(frlc)
        fclose(frlc);
    if(frlca)
        fclose(frlca);
}

void writePictureCtrl(DecContainer * pDecContainer, u32 * phalves,
                      u32 * pnumAddr)
{
#if 0
    u32 tmp = 0;

    u32 offset[2] = { 0, 0 };

    FILE *fid = NULL;

    u32 halves = *phalves;

    printf("pcitureCtrl\n");
    fflush(stdout);
    fid = fopen("picture_ctrl_dec.trc", "at");

    if(fid == NULL)
    {
        return;
    }
    if(fid != NULL)
    {
        offset[1] = pDecContainer->VopDesc.totalMbInVop * 384;

        fprintf(fid, "------------------picture = %-3d "
                "-------------------------------------\n",
                pDecContainer->VopDesc.vopNumber - 1);

        fprintf(fid, "%d        decoding mode (h264/mpeg4/h263/jpeg)\n",
                pDecContainer->StrmStorage.shortVideo + 1);

        fprintf(fid, "%d        Picture Width in macro blocks\n",
                pDecContainer->VopDesc.vopWidth);
        fprintf(fid, "%d        Picture Height in macro blocks\n",
                pDecContainer->VopDesc.vopHeight);

        fprintf(fid, "%d        Rounding control bit for MPEG4\n",
                pDecContainer->VopDesc.vopRoundingType);
        fprintf(fid, "%d        Filtering disable\n",
                pDecContainer->ApiStorage.disableFilter);

        fprintf(fid, "%-3d      Chrominance QP filter offset\n", 0);
        fprintf(fid, "%-3d      Amount of motion vectors / current picture\n",
                NBR_MV_WORDS_MB * pDecContainer->VopDesc.totalMbInVop);

        /* IF thr == 7, use ac tables */
        tmp = (pDecContainer->VopDesc.intraDcVlcThr == 7) ? 0 : 1;

        fprintf(fid, "%-3d      h264:amount of 4x4 mb/"
                " mpeg4:separate dc mb / picture\n", tmp);

        /* Number of 16 bit rlc words, rounded up to be divisible by 4 */
        fprintf(fid, "%-8d h264:amount of P_SKIP"
                " mbs mpeg4jpeg: amount of rlc/pic\n", halves
                /*      (((((u32)pDecContainer->MbSetDesc.pRlcDataCurrAddr
                 * - (u32)pDecContainer->MbSetDesc.pRlcDataAddr+3)&(~3))>>1))
                 */
                /* (pDecContainer->MbSetDesc.pRlcDataCurrAddr
                 * -pDecContainer->MbSetDesc.pRlcDataAddr+3)&3 *//*(((numAddr)<<1)+3)&~(3), numAddr */
                );
        fprintf(fid, "%-8d Base address offset for reference index 0\n" "0        Base address offset for reference index 1\n" "0        Base address offset for reference index 2\n" "0        Base address offset for reference index 3\n" "0        Base address offset for reference index 4\n" "0        Base address offset for reference index 5\n" "0        Base address offset for reference index 6\n" "0        Base address offset for reference index 7\n" "0        Base address offset for reference index 8\n" "0        Base address offset for reference index 9\n" "0        Base address offset for reference index 10\n" "0        Base address offset for reference index 11\n" "0        Base address offset for reference index 12\n" "0        Base address offset for reference index 13\n" "0        Base address offset for reference index 14\n" "0        Base address offset for reference index 15\n" "%-8d Base address offset for decoder output picture\n", offset[(pDecContainer->VopDesc.vopNumber - 1) & 1], /* odd or even? */
                offset[(pDecContainer->VopDesc.vopNumber) & 1]);
        fprintf(fid, "0        decoutDisable\n");

        fprintf(fid,
                "%-3d      Vop Fcode Forward (MPEG4)\n",
                pDecContainer->VopDesc.fcodeFwd);
    }
    if(fid)
        fclose(fid);
#endif
}

u32 rlcBufferLoad(u32 used, u32 size, u32 print, u32 voptype, u32 vop)
{
#if 0
    u32 tmp = 0;
    static u32 load = 0, biggestused = 0, biggestsize = 0, type = 0, vopnum = 0;

    if(!size)
        size = 1;

    if(print)
    {

        printf
            ("MAXIMUM RLC BUFFER LOAD: %d percent (used %d  max %d) type %d number %d\n",
             load, biggestused, biggestsize, type, vopnum);

    }
    else
    {

        tmp = (used * 100) / size;

        if(tmp > load)
        {
            load = tmp;
            biggestused = used;
            biggestsize = size;
            type = voptype;
            vopnum = vop;

        }

    }
#endif
}
void writePictureCtrlHex( DecContainer *pDecContainer, u32 rlcMode)
{



    /* Variables */

    u32 offset[2]={0,0};
    u32 i;
    u32 tmp;
    u32 * regBase = pDecContainer->mp4Regs;
    static u32 memoryStartPointer = 0xFFFFFFFF;
    FILE *pcHex = NULL;

    /* Code */
    pcHex = fopen("picture_ctrl_dec.hex", "at");

    offset[1] = pDecContainer->VopDesc.totalMbInVop * 384;

#if 0
    if(fid != NULL)
    {
        fprintf(fid, "----------------picture = %u -------------------\n",
            pDecContainer->VopDesc.vopNumber - 1);

        fprintf(fid, "%u\tdecoding mode (h264/mpeg4/h263/jpeg)\n",
                 pDecContainer->StrmStorage.shortVideo+1);
        fprintf(fid, "%u\tRLC mode enable\n",
                 rlcMode);

         fprintf(fid, "%u\tpicture width in macro blocks\n",
                 pDecContainer->VopDesc.vopWidth);
         fprintf(fid, "%u\tpicture height in macro blocks\n",
                 pDecContainer->VopDesc.vopHeight);

        fprintf(fid, "%u\trounding control bit\n",
                    pDecContainer->VopDesc.vopRoundingType);
         fprintf(fid, "%u\tfiltering disable\n",
                    !pDecContainer->ApiStorage.deblockEna);

        fprintf(fid, "%u\tchrominance QP filter offset\n", 0);
        fprintf(fid, "%u\tamount of mvs / picture\n",
                    NBR_MV_WORDS_MB * pDecContainer->VopDesc.totalMbInVop);

        /* IF thr == 7, use ac tables */
        tmp = (pDecContainer->VopDesc.intraDcVlcThr == 7)   ? 0 : 1;

        fprintf(fid, "%u\tseparate dc mb / picture\n", tmp);

        /* Number of 16 bit rlc words, rounded up to be divisible by 4 */
        fprintf(fid, "%u\tamount of rlc / picture\n", halves);
        fprintf(fid, "%u\tbase address offset for reference pic\n",
            offset[(pDecContainer->VopDesc.vopNumber - 1)&1]);
        fprintf(fid, "%u\tbase address offset for decoder output pic\n",
            offset[(pDecContainer->VopDesc.vopNumber)&1]);
        fprintf(fid, "0\tdecoutDisable\n");
        fprintf(fid, "%u\tvop fcode forward\n",
            pDecContainer->VopDesc.fcodeFwd);
        fprintf(fid, "%u\tintra DC VLC threshold\n",
            pDecContainer->VopDesc.intraDcVlcThr);
        fprintf(fid, "%u\tvop type, '1' = inter, '0' = intra\n",
            pDecContainer->VopDesc.vopCodingType);
        fprintf(fid, "%u\tvideo packet enable, '1' = enabled, '0' = disabled\n",
            pDecContainer->Hdrs.resyncMarkerDisable ? 0 : 1);
        fprintf(fid, "%u\tvop time increment resolution\n",
            pDecContainer->Hdrs.vopTimeIncrementResolution);
        fprintf(fid, "%u\tinitial value for QP\n",
            pDecContainer->VopDesc.qP);
    }
#endif
        if(pcHex)
            fclose(pcHex);

#if 0
    MP4DEC_DEBUG(("GetMP4DecId   %x\n", GetMP4DecId(dwl)));
    MP4DEC_DEBUG(("GetMP4DecPictureWidth  %d\n", GetMP4DecPictureWidth(dwl)));
    MP4DEC_DEBUG(("GetMP4DecPictureHeight   %d\n", GetMP4DecPictureHeight(dwl)));
    MP4DEC_DEBUG(("GetMP4DecondingMode   %d\n", GetMP4DecDecondingMode(dwl)));
    MP4DEC_DEBUG(("GetMP4RoundingCtrl   %d\n", GetMP4DecRoundingCtrl(dwl)));
    MP4DEC_DEBUG(("GetMP4DecDisableOutputWrite   %d\n",
           GetMP4DecDisableOutputWrite(dwl)));
    MP4DEC_DEBUG(("GetMP4DecFilterDisable   %d\n", GetMP4DecFilterDisable(dwl)));
    MP4DEC_DEBUG(("GetMP4DecChromaQpOffset  %d\n", GetMP4DecChromaQpOffset(dwl)));
/*    MP4DEC_DEBUG(("GetMP4DecIrqEnable   %d\n",GetMP4DecIrqEnable(dwl)));*/
    MP4DEC_DEBUG(("GetMP4DecIrqStat   %d\n", GetMP4DecIrqStat(dwl)));
    MP4DEC_DEBUG(("GetMP4DecIrqLine   %d\n", GetMP4DecIrqLine(dwl)));
    MP4DEC_DEBUG(("GetMP4DecEnable   %d\n", GetMP4DecEnable(dwl)));
    MP4DEC_DEBUG(("GetMP4DecMbCtrlBaseAddress   %x\n",
           GetMP4DecMbCtrlBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecMvBaseAddress   %x\n", GetMP4DecMvBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecDcCompBaseAddress   %x\n",
           GetMP4DecDcCompBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecRlcBaseAddress   %x\n", GetMP4DecRlcBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecRefPictBaseAddress   %x\n",
           GetMP4DecRefPictBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecRlcAmount   %d\n", GetMP4DecRlcAmount(dwl)));
    MSetMP4DecDecodingModeP4DEC_DEBUG(("GetMP4DecOutputPictBaseAddress   %x\n",
           GetMP4DecOutputPictBaseAddress(dwl)));
    MP4DEC_DEBUG(("GetMP4DecVopFCode   %d\n", GetMP4DecVopFCode(dwl)));
    MP4DEC_DEBUG(("GetMP4DecOutputPictEndian   %d\n", GetMP4DecOutputPictEndian(dwl)));
    MP4DEC_DEBUG(("GetMP4DecAmbaBurstLength   %d\n", GetMP4DecAmbaBurstLength(dwl)));
    MP4DEC_DEBUG(("GetMP4DecAsicServicePriority   %d\n",
           GetMP4DecAsicServicePriority(dwl)));
#endif

}
/*------------------------------------------------------------------------------

    Function name: PrintDecPpUsage

        Functional description:

        Inputs:
            pDecCont    Pointer to decoder structure
            ff          Is current field first or second field of the frame
            picIndex    Picture buffer index
            decStatus   DEC / PP print
            picId       Picture ID of the field/frame

        Outputs:
            None

        Returns:
            None

------------------------------------------------------------------------------*/

#ifdef _DEC_PP_USAGE
void MP4DecPpUsagePrint(DecContainer *pDecCont,
                      u32 ppmode,
                      u32 picIndex,
                      u32 decStatus,
                      u32 picId)
{
    FILE *fp;
    picture_t *pPic;
    pPic = (picture_t*)pDecCont->StrmStorage.pPicBuf;

    fp = fopen("dec_pp_usage.txt", "at");
    if (fp == NULL)
        return;

    if (decStatus)
    {

            fprintf(fp, "\n================================================================================\n");

            fprintf(fp, "%10s%10s%10s%10s%10s%10s%10s%10s\n",
                    "Component", "PicId", "PicType", "Fcm", "Field",
                    "PPMode", "BuffIdx", "MultiBuf");

        /* Component and PicId */
        fprintf(fp, "\n%10.10s%10d", "DEC", picId);
        /* Pictype */
        switch (pDecCont->VopDesc.vopCodingType)
        {
            case PVOP: fprintf(fp, "%10s","P"); break;
            case IVOP: fprintf(fp, "%10s","I"); break;
            case BVOP: fprintf(fp, "%10s","B"); break;
            default: ASSERT(0); break;
        }
        /* Frame coding mode */
        switch (pDecCont->Hdrs.interlaced)
        {
            case 0: fprintf(fp, "%10s","PR"); break;
            default: fprintf(fp, "%10s","FR"); break;
        }
        /* Field */
        if ( pDecCont->Hdrs.interlaced)
            if(pDecCont->VopDesc.topFieldFirst)
                fprintf(fp, "%10s","TOP 1ST");
            else
                fprintf(fp, "%10s","BOT 1ST");
        else

                fprintf(fp, "%10s","PROG");
        /* PPMode and BuffIdx */
        fprintf(fp, "%10s%10d%10s\n", "---",picIndex, "---");



    }
    else /* pp status */
    {
        /* Component and PicId + pic type */
        /* for pipelined, the picture has not been indexed to
         * buffer yet, so you don't get correct values from pPic */
        if(ppmode == DECPP_PIPELINED)
        {
            fprintf(fp, "%10s%10s", "PP", "''");
            fprintf(fp, "%10s","''");
        }
        else{
            fprintf(fp, "%10s%10d", "PP", picId);
            switch (pPic[picIndex].picType)
            {
                case PVOP: fprintf(fp, "%10s","P"); break;
                case IVOP: fprintf(fp, "%10s","I"); break;
                case BVOP: fprintf(fp, "%10s","B"); break;
                default: ASSERT(0); break;
            }
        }

        /* Frame coding mode */
        switch (pDecCont->ppControl.picStruct)
        {
            case DECPP_PIC_FRAME_OR_TOP_FIELD: fprintf(fp, "%10s","PR"); break;
            case DECPP_PIC_TOP_FIELD_FRAME: fprintf(fp, "%10s","FR FIO TOP"); break;
            case DECPP_PIC_BOT_FIELD_FRAME: fprintf(fp, "%10s","FR FIO BOT"); break;
            case DECPP_PIC_TOP_AND_BOT_FIELD_FRAME: fprintf(fp, "%10s","FR FRO"); break;
        }
        /* Field */
        fprintf(fp, "%10s","-");

        /* PPMode and BuffIdx */
        switch (ppmode)
        {
            case DECPP_STAND_ALONE:
                fprintf(fp, "%10s%10d", "STAND",picIndex);
                break;
            case DECPP_PARALLEL:
                fprintf(fp, "%10s%10d", "PARAL",picIndex);
                break;
            case DECPP_PIPELINED:
                fprintf(fp, "%10s%10d", "PIPEL",picIndex);
                break;
            default:
                break;
        }
        switch (pDecCont->ppControl.multiBufStat)
        {
            case MULTIBUFFER_SEMIMODE:
                fprintf(fp, "%10s\n", "SEMI");
                break;
            case MULTIBUFFER_FULLMODE:
                fprintf(fp, "%10s\n", "FULL");
                break;
            case MULTIBUFFER_DISABLED:
                fprintf(fp, "%10s\n", "DISA");
                break;
            default:
                break;
        }
    }

    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }
}
#endif
