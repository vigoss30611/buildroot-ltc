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
-
-  Description : ...
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp6hwd_asic.c,v $
-  $Revision: 1.35 $
-  $Date: 2011/01/25 13:37:58 $
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "dwl.h"
#include "regdrv.h"
#include "vp6hwd_container.h"
#include "vp6hwd_asic.h"
#include "vp6hwd_debug.h"

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#include <stdio.h>
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

#ifdef ASIC_TRACE_SUPPORT
#include "vp6_sw_trace.h"
#endif

#define SCAN(i)         HWIF_SCAN_MAP_ ## i

static const u32 ScanTblRegId[64] = { 0 /* SCAN(0) */ ,
    SCAN(1), SCAN(2), SCAN(3), SCAN(4), SCAN(5), SCAN(6), SCAN(7), SCAN(8),
    SCAN(9), SCAN(10), SCAN(11), SCAN(12), SCAN(13), SCAN(14),
    SCAN(15), SCAN(16), SCAN(17), SCAN(18), SCAN(19), SCAN(20),
    SCAN(21), SCAN(22), SCAN(23), SCAN(24), SCAN(25), SCAN(26),
    SCAN(27), SCAN(28), SCAN(29), SCAN(30), SCAN(31), SCAN(32),
    SCAN(33), SCAN(34), SCAN(35), SCAN(36), SCAN(37), SCAN(38),
    SCAN(39), SCAN(40), SCAN(41), SCAN(42), SCAN(43), SCAN(44),
    SCAN(45), SCAN(46), SCAN(47), SCAN(48), SCAN(49), SCAN(50),
    SCAN(51), SCAN(52), SCAN(53), SCAN(54), SCAN(55), SCAN(56),
    SCAN(57), SCAN(58), SCAN(59), SCAN(60), SCAN(61), SCAN(62), SCAN(63)
};

#define TAP(i, j)       HWIF_PRED_BC_TAP_ ## i ## _ ## j

static const u32 TapRegId[8][4] = {
    {TAP(0, 0), TAP(0, 1), TAP(0, 2), TAP(0, 3)},
    {TAP(1, 0), TAP(1, 1), TAP(1, 2), TAP(1, 3)},
    {TAP(2, 0), TAP(2, 1), TAP(2, 2), TAP(2, 3)},
    {TAP(3, 0), TAP(3, 1), TAP(3, 2), TAP(3, 3)},
    {TAP(4, 0), TAP(4, 1), TAP(4, 2), TAP(4, 3)},
    {TAP(5, 0), TAP(5, 1), TAP(5, 2), TAP(5, 3)},
    {TAP(6, 0), TAP(6, 1), TAP(6, 2), TAP(6, 3)},
    {TAP(7, 0), TAP(7, 1), TAP(7, 2), TAP(7, 3)}
};

#define probSameAsLastOffset                (0)
#define probModeOffset                      (4*8)
#define probMvIsShortOffset                 (38*8)
#define probMvSignOffset                    (probMvIsShortOffset + 2)
#define probMvSizeOffset                    (39*8)
#define probMvShortOffset                   (41*8)

#define probDCFirstOffset                   (43*8)
#define probACFirstOffset                   (46*8)
#define probACZeroRunFirstOffset            (64*8)
#define probDCRestOffset                    (65*8)
#define probACRestOffset                    (71*8)
#define probACZeroRunRestOffset             (107*8)

#define huffmanTblDCOffset                  (43*8)
#define huffmanTblACZeroRunOffset           (huffmanTblDCOffset + 48)
#define huffmanTblACOffset                  (huffmanTblACZeroRunOffset + 48)

static void VP6HwdAsicRefreshRegs(VP6DecContainer_t * pDecCont);
static void VP6HwdAsicFlushRegs(VP6DecContainer_t * pDecCont);

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicInit
    Description     : 
    Return type     : void 
    Argument        : VP6DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP6HwdAsicInit(VP6DecContainer_t * pDecCont)
{

    G1DWLmemset(pDecCont->vp6Regs, 0, sizeof(pDecCont->vp6Regs));

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_MODE, DEC_8190_MODE_VP6);

    /* these parameters are defined in deccfg.h */
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUT_ENDIAN,
                   DEC_X170_OUTPUT_PICTURE_ENDIAN);

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_IN_ENDIAN,
                   DEC_X170_INPUT_DATA_ENDIAN);

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_STRENDIAN_E,
                   DEC_X170_INPUT_STREAM_ENDIAN);

/*
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUT_TILED_E,
                   DEC_X170_OUTPUT_FORMAT);
                   */

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_MAX_BURST,
                   DEC_X170_BUS_BURST_LENGTH);

    if ((G1DWLReadAsicID() >> 16) == 0x8170U)
    {
        SetDecRegister(pDecCont->vp6Regs, HWIF_PRIORITY_MODE,
                       DEC_X170_ASIC_SERVICE_PRIORITY);
    }
    else
    {
        SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_SCMD_DIS,
            DEC_X170_SCMD_DISABLE);
    }

    DEC_SET_APF_THRESHOLD(pDecCont->vp6Regs);
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_LATENCY,
                   DEC_X170_LATENCY_COMPENSATION);

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_DATA_DISC_E,
                   DEC_X170_DATA_DISCARD_ENABLE);

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUTSWAP32_E,
                   DEC_X170_OUTPUT_SWAP_32_ENABLE);

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_INSWAP32_E,
                   DEC_X170_INPUT_DATA_SWAP_32_ENABLE);

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_STRSWAP32_E,
                   DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);

#if( DEC_X170_HW_TIMEOUT_INT_ENA  != 0)
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_TIMEOUT_E, 1);
#else
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_TIMEOUT_E, 0);
#endif

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_CLK_GATE_E, 1);
#else
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_CLK_GATE_E, 0);
#endif

#if( DEC_X170_USING_IRQ  == 0)
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_IRQ_DIS, 1);
#else
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_IRQ_DIS, 0);
#endif

    /* set AXI RW IDs */
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_AXI_RD_ID,
        (DEC_X170_AXI_ID_R & 0xFFU));
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_AXI_WR_ID,
        (DEC_X170_AXI_ID_W & 0xFFU));

}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicAllocateMem
    Description     : 
    Return type     : i32 
    Argument        : VP6DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
i32 VP6HwdAsicAllocateMem(VP6DecContainer_t * pDecCont)
{

    const void *dwl = pDecCont->dwl;
    i32 dwl_ret;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    dwl_ret = G1DWLMallocLinear(dwl, (1 + 266) * 8, &pAsicBuff->probTbl);

    if(dwl_ret != DWL_OK)
    {
        goto err;
    }

    SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWPROBTBL_BASE,
                   pAsicBuff->probTbl.busAddress);

    return 0;

  err:
    VP6HwdAsicReleaseMem(pDecCont);
    return -1;
}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicReleaseMem
    Description     : 
    Return type     : void 
    Argument        : VP6DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP6HwdAsicReleaseMem(VP6DecContainer_t * pDecCont)
{
    const void *dwl = pDecCont->dwl;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    if(pAsicBuff->probTbl.virtualAddress != NULL)
    {
        DWLFreeLinear(dwl, &pAsicBuff->probTbl);
        G1DWLmemset(&pAsicBuff->probTbl, 0, sizeof(pAsicBuff->probTbl));
    }
}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicAllocatePictures
    Description     : 
    Return type     : i32 
    Argument        : VP6DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
i32 VP6HwdAsicAllocatePictures(VP6DecContainer_t * pDecCont)
{
    const void *dwl = pDecCont->dwl;
    i32 dwl_ret, i;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    const u32 pict_buff_size = pAsicBuff->width * pAsicBuff->height * 3 / 2;

    G1DWLmemset(pAsicBuff->pictures, 0, sizeof(pAsicBuff->pictures));

    dwl_ret = BqueueInit(&pDecCont->bq, 
                          pDecCont->numBuffers );
    if( dwl_ret != 0 )
    {
        goto err;
    }

    for (i = 0; i < pDecCont->numBuffers; i++)
    {
        dwl_ret = G1DWLMallocRefFrm(dwl, pict_buff_size, &pAsicBuff->pictures[i]);
        if(dwl_ret != DWL_OK)
        {
            goto err;
        }
    }

    SetDecRegister(pDecCont->vp6Regs, HWIF_PIC_MB_WIDTH, pAsicBuff->width / 16);

    SetDecRegister(pDecCont->vp6Regs, HWIF_PIC_MB_HEIGHT_P,
                   pAsicBuff->height / 16);

    pAsicBuff->outBufferI = BqueueNext( &pDecCont->bq, 
                                        BQUEUE_UNUSED, BQUEUE_UNUSED, 
                                        BQUEUE_UNUSED, 0 );

    pAsicBuff->outBuffer = &pAsicBuff->pictures[pAsicBuff->outBufferI];
    /* These need to point at something so use the output buffer */
    pAsicBuff->refBuffer        = pAsicBuff->outBuffer;
    pAsicBuff->goldenBuffer     = pAsicBuff->outBuffer;
    pAsicBuff->refBufferI       = BQUEUE_UNUSED;
    pAsicBuff->goldenBufferI    = BQUEUE_UNUSED;

    return 0;

  err:
    VP6HwdAsicReleasePictures(pDecCont);
    return -1;
}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicReleasePictures
    Description     : 
    Return type     : void 
    Argument        : VP6DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP6HwdAsicReleasePictures(VP6DecContainer_t * pDecCont)
{
    const void *dwl = pDecCont->dwl;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;
    u32 i;

    BqueueRelease( &pDecCont->bq );

    for( i = 0 ; i < pDecCont->numBuffers ; ++i )
    {
        if(pAsicBuff->pictures[i].virtualAddress != NULL)
        {
            DWLFreeRefFrm(dwl, &pAsicBuff->pictures[i]);
        }
    }

    G1DWLmemset(pAsicBuff->pictures, 0, sizeof(pAsicBuff->pictures));
}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicInitPicture
    Description     : 
    Return type     : void 
    Argument        : VP6DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP6HwdAsicInitPicture(VP6DecContainer_t * pDecCont)
{

    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUT_BASE,
                   pAsicBuff->outBuffer->busAddress);

    /* golden reference */
    SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWGOLDEN_BASE,
                   pAsicBuff->goldenBuffer->busAddress);

    /* previous picture */
    SetDecRegister(pDecCont->vp6Regs, HWIF_REFER0_BASE,
                   pAsicBuff->refBuffer->busAddress);

    /* frame type */
    if(pDecCont->pb.FrameType == BASE_FRAME)
    {
        SetDecRegister(pDecCont->vp6Regs, HWIF_PIC_INTER_E, 0);
    }
    else
    {
        SetDecRegister(pDecCont->vp6Regs, HWIF_PIC_INTER_E, 1);
    }

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUT_DIS, 0);

    /* loop filter */
    if(pDecCont->pb.UseLoopFilter)
    {
        SetDecRegister(pDecCont->vp6Regs, HWIF_FILTERING_DIS, 0);

        /* FLimit for loop filter */
        SetDecRegister(pDecCont->vp6Regs, HWIF_LOOP_FILT_LIMIT,
                       VP6HWDeblockLimitValues[pDecCont->pb.DctQMask]);
    }
    else
    {
        SetDecRegister(pDecCont->vp6Regs, HWIF_LOOP_FILT_LIMIT, 0);
        SetDecRegister(pDecCont->vp6Regs, HWIF_FILTERING_DIS, 1);
    }

    /* multistream and huffman */
    if(pDecCont->pb.MultiStream)
    {
        SetDecRegister(pDecCont->vp6Regs, HWIF_MULTISTREAM_E, 1);

        if(pDecCont->pb.UseHuffman)
        {
            SetDecRegister(pDecCont->vp6Regs, HWIF_HUFFMAN_E, 1);
        }
        else
        {
            SetDecRegister(pDecCont->vp6Regs, HWIF_HUFFMAN_E, 0);
        }

        /* second partiton's base address */
        /*SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWPART2_BASE,
         * pAsicBuff->partition2Base); */
    }
    else
    {
        SetDecRegister(pDecCont->vp6Regs, HWIF_MULTISTREAM_E, 0);

        SetDecRegister(pDecCont->vp6Regs, HWIF_HUFFMAN_E, 0);

        /*SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWPART2_BASE, (u32) (~0)); */
    }

    /* first bool decoder status */
    SetDecRegister(pDecCont->vp6Regs, HWIF_BOOLEAN_VALUE,
                   (pDecCont->pb.br.value >> 24) & (0xFFU));

    SetDecRegister(pDecCont->vp6Regs, HWIF_BOOLEAN_RANGE,
                   pDecCont->pb.br.range & (0xFFU));

    /* interpolation filter type */
    if(pDecCont->pb.PredictionFilterMode == BILINEAR_ONLY_PM)
    {
        /* bilinear only */
        SetDecRegister(pDecCont->vp6Regs, HWIF_BILIN_MC_E, 1);

        SetDecRegister(pDecCont->vp6Regs, HWIF_VARIANCE_TEST_E, 0);

        SetDecRegister(pDecCont->vp6Regs, HWIF_VAR_THRESHOLD, 0);

        SetDecRegister(pDecCont->vp6Regs, HWIF_MV_THRESHOLD, 0);
    }
    else if (pDecCont->pb.PredictionFilterMode == BICUBIC_ONLY_PM)
    {
        /* bicubic only */
        SetDecRegister(pDecCont->vp6Regs, HWIF_BILIN_MC_E, 0);

        SetDecRegister(pDecCont->vp6Regs, HWIF_VARIANCE_TEST_E, 0);

        SetDecRegister(pDecCont->vp6Regs, HWIF_VAR_THRESHOLD, 0);

        SetDecRegister(pDecCont->vp6Regs, HWIF_MV_THRESHOLD, 0);
    }
    else
    {
        /* auto selection bilinear/bicubic */
        SetDecRegister(pDecCont->vp6Regs, HWIF_BILIN_MC_E, 0);

        SetDecRegister(pDecCont->vp6Regs, HWIF_VARIANCE_TEST_E, 1);

        SetDecRegister(pDecCont->vp6Regs, HWIF_VAR_THRESHOLD,
                       pDecCont->pb.PredictionFilterVarThresh & (0x3FFU));

        SetDecRegister(pDecCont->vp6Regs, HWIF_MV_THRESHOLD,
                       pDecCont->pb.PredictionFilterMvSizeThresh & (0x07U));
    }

    /* QP */
    SetDecRegister(pDecCont->vp6Regs, HWIF_INIT_QP, pDecCont->pb.DctQMask);

    /* scan order */
    {
        i32 i;

        for(i = 1; i < BLOCK_SIZE; i++)
        {
            SetDecRegister(pDecCont->vp6Regs, ScanTblRegId[i],
                           pDecCont->pb.MergedScanOrder[i]);
        }
    }

    /* bicubic prediction filter tap */
    if(pDecCont->pb.PredictionFilterMode == AUTO_SELECT_PM ||
       pDecCont->pb.PredictionFilterMode == BICUBIC_ONLY_PM )
    {
        i32 i, j;
        const i32 *bcfs =
            &VP6HW_BicubicFilterSet[pDecCont->pb.PredictionFilterAlpha][0][0];

        for(i = 0; i < 8; i++)
        {
            for(j = 0; j < 4; j++)
            {
                SetDecRegister(pDecCont->vp6Regs, TapRegId[i][j],
                               (bcfs[i * 4 + j] & 0xFF));
            }
        }

    }
    /* FLimit for loop filter */
    SetDecRegister(pDecCont->vp6Regs, HWIF_LOOP_FILT_LIMIT,
                   VP6HWDeblockLimitValues[pDecCont->pb.DctQMask]);

    /* Setup reference picture buffer */
    if( pDecCont->refBufSupport )
    {
        RefbuSetup( &pDecCont->refBufferCtrl, pDecCont->vp6Regs, 
                    REFBU_FRAME, 
                    (pDecCont->pb.FrameType == BASE_FRAME),
                    HANTRO_FALSE, 
                    0, 0, 
                    REFBU_FORCE_ADAPTIVE_SINGLE );
    }

    if( pDecCont->tiledModeSupport)
    {
        pDecCont->tiledReferenceEnable = 
            DecSetupTiledReference( pDecCont->vp6Regs, 
                pDecCont->tiledModeSupport,
                0 /* interlaced content not present */ );
    }
    else
    {
        pDecCont->tiledReferenceEnable = 0;
    }
}

/*------------------------------------------------------------------------------
    Function name : VP6HwdAsicStrmPosUpdate
    Description   : Set stream base and length related registers

    Return type   :
    Argument      : container
------------------------------------------------------------------------------*/
void VP6HwdAsicStrmPosUpdate(VP6DecContainer_t * pDecCont)
{
    u32 tmp;
    u32 hwBitPos;

    DEBUG_PRINT(("VP6HwdAsicStrmPosUpdate:\n"));

    pDecCont->asicBuff->partition1BitOffset =
        (pDecCont->pb.strm.bitsConsumed) +
        ((pDecCont->pb.br.pos - 3) * 8) + (8 - pDecCont->pb.br.count);

    pDecCont->asicBuff->partition1Base +=
        pDecCont->asicBuff->partition1BitOffset / 8;
    pDecCont->asicBuff->partition1BitOffset =
        pDecCont->asicBuff->partition1BitOffset & 7;

    /* bit offset if base is unaligned */
    tmp = (pDecCont->asicBuff->partition1Base & DEC_8190_ALIGN_MASK) * 8;

    hwBitPos = tmp + pDecCont->asicBuff->partition1BitOffset;

    DEBUG_PRINT(("\tStart bit pos %8d\n", hwBitPos));

    tmp = pDecCont->asicBuff->partition1Base;   /* unaligned base */
    tmp &= (~DEC_8190_ALIGN_MASK);  /* align the base */

    SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWPART1_BASE, (u32)(~0));
    SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWPART2_BASE, (u32)(~0));
    SetDecRegister(pDecCont->vp6Regs, HWIF_STRM1_START_BIT, 0);
    SetDecRegister(pDecCont->vp6Regs, HWIF_STRM_START_BIT, 0);

    /* Set stream pointer */
    if(pDecCont->pb.MultiStream)
    {
        /* Multistream; set both partition pointers. */
        DEBUG_PRINT(("\tStream base 1 %08x\n", tmp));
        SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWPART1_BASE, tmp);
        SetDecRegister(pDecCont->vp6Regs, HWIF_STRM1_START_BIT, hwBitPos);        

        DEBUG_PRINT(("\tStream base 2 %08x\n", tmp));
        tmp = pDecCont->asicBuff->partition2Base + pDecCont->pb.Buff2Offset;
        SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWPART2_BASE, 
            tmp & (~DEC_8190_ALIGN_MASK));

        /* bit offset if base is unaligned */
        tmp = (tmp & DEC_8190_ALIGN_MASK) * 8;

        SetDecRegister(pDecCont->vp6Regs, HWIF_STRM_START_BIT, tmp);

        /* calculate strm 1 length */
        tmp = pDecCont->asicBuff->partition2Base -
              pDecCont->asicBuff->partition1Base + pDecCont->pb.Buff2Offset +
              hwBitPos/8;
        SetDecRegister(pDecCont->vp6Regs, HWIF_STREAM1_LEN, tmp );
    }
    else
    {
        /* Single stream; */
        SetDecRegister(pDecCont->vp6Regs, HWIF_STREAM1_LEN, 0);
        DEBUG_PRINT(("\tStream base 1 %08x\n", tmp));
        SetDecRegister(pDecCont->vp6Regs, HWIF_VP6HWPART2_BASE, tmp);
        SetDecRegister(pDecCont->vp6Regs, HWIF_STRM_START_BIT, hwBitPos);
    }

    tmp = pDecCont->pb.strm.amountLeft; /* unaligned stream */
    tmp += hwBitPos / 8;    /* add the alignmet bytes */

    DEBUG_PRINT(("\tStream length 1 %8d\n", tmp));
    SetDecRegister(pDecCont->vp6Regs, HWIF_STREAM_LEN, tmp);

}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicRefreshRegs
    Description     :
    Return type     : void
    Argument        : decContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP6HwdAsicRefreshRegs(VP6DecContainer_t * pDecCont)
{
    i32 i;
    u32 offset = 0x0;

    u32 *decRegs = pDecCont->vp6Regs;

    for(i = DEC_X170_REGISTERS; i > 0; --i)
    {
        *decRegs++ = G1DWLReadReg(pDecCont->dwl, offset);
        offset += 4;
    }
}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicFlushRegs
    Description     :
    Return type     : void
    Argument        : decContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP6HwdAsicFlushRegs(VP6DecContainer_t * pDecCont)
{
    i32 i;
    u32 offset = 0x04;
    const u32 *decRegs = pDecCont->vp6Regs + 1;

#ifdef TRACE_START_MARKER
    /* write ID register to trigger logic analyzer */
    G1DWLWriteReg(pDecCont->dwl, 0x00, ~0);
#endif

    for(i = DEC_X170_REGISTERS; i > 1; --i)
    {
        G1DWLWriteReg(pDecCont->dwl, offset, *decRegs);
        decRegs++;
        offset += 4;
    }
}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicRun
    Description     : 
    Return type     : u32 
    Argument        : VP6DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
u32 VP6HwdAsicRun(VP6DecContainer_t * pDecCont)
{
    u32 asic_status = 0;
    i32 ret;

    ret = G1DWLReserveHw(pDecCont->dwl);
    if(ret != DWL_OK)
    {
        return VP6HWDEC_HW_RESERVED;
    }

    pDecCont->asicRunning = 1;

#ifdef ASIC_TRACE_SUPPORT
    {
        const PB_INSTANCE *pbi = &pDecCont->pb;
        static u8 bicubicFilterAlphaPrev = 255;

        if(bicubicFilterAlphaPrev == 255)
            bicubicFilterAlphaPrev = pbi->PredictionFilterAlpha;

        if(pbi->RefreshGoldenFrame)
            trace_DecodingToolUsed(TOOL_GOLDEN_FRAME_UPDATES);
        if(pbi->PredictionFilterAlpha != bicubicFilterAlphaPrev)
            trace_DecodingToolUsed(TOOL_MULTIPLE_FILTER_ALPHA);
        if(pbi->probModeUpdate)
            trace_DecodingToolUsed(TOOL_MODE_PROBABILITY_UPDATES);
        if(pbi->probMvUpdate)
            trace_DecodingToolUsed(TOOL_MV_TREE_UPDATES);
        if(pbi->scanUpdate)
            trace_DecodingToolUsed(TOOL_CUSTOM_SCAN_ORDER);
        if(pbi->probDcUpdate)
            trace_DecodingToolUsed(TOOL_COEFF_PROBABILITY_UPDATES_DC);
        if(pbi->probAcUpdate)
            trace_DecodingToolUsed(TOOL_COEFF_PROBABILITY_UPDATES_AC);
        if(pbi->probZrlUpdate)
            trace_DecodingToolUsed(TOOL_COEFF_PROBABILITY_UPDATES_ZRL);

        if(pbi->HFragments != pbi->OutputWidth ||
           pbi->VFragments != pbi->OutputHeight)
            trace_DecodingToolUsed(TOOL_OUTPUT_SCALING);
    }
#endif

    if(pDecCont->pp.ppInstance != NULL &&
       pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING)
    {
        DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;
        DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

        TRACE_PP_CTRL("VP6HwdAsicRun: PP Run\n");

        decPpIf->inwidth  = pDecCont->pb.HFragments * 8;
        decPpIf->inheight = pDecCont->pb.VFragments * 8;
        decPpIf->croppedW = decPpIf->inwidth;
        decPpIf->croppedH = decPpIf->inheight;
        decPpIf->tiledInputMode = pDecCont->tiledReferenceEnable;
        decPpIf->progressiveSequence = 1;

        decPpIf->picStruct = DECPP_PIC_FRAME_OR_TOP_FIELD;
        decPpIf->littleEndian =
            GetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUT_ENDIAN);
        decPpIf->wordSwap =
            GetDecRegister(pDecCont->vp6Regs, HWIF_DEC_OUTSWAP32_E);

        if(decPpIf->usePipeline)
        {
            decPpIf->inputBusLuma = decPpIf->inputBusChroma = 0;
        }
        else /* parallel processing */
        {
            decPpIf->inputBusLuma = pAsicBuff->refBuffer->busAddress;
            decPpIf->inputBusChroma =
                decPpIf->inputBusLuma + decPpIf->inwidth * decPpIf->inheight;
        }

        pDecCont->pp.PPDecStart(pDecCont->pp.ppInstance, decPpIf);
    }

    VP6HwdAsicFlushRegs(pDecCont);

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_E, 1);
    DWLEnableHW(pDecCont->dwl, 4 * 1, pDecCont->vp6Regs[1]);

    ret = G1DWLWaitHwReady(pDecCont->dwl, (u32) DEC_X170_TIMEOUT_LENGTH);

    if(ret != DWL_HW_WAIT_OK)
    {
        ERROR_PRINT("G1DWLWaitHwReady");
        DEBUG_PRINT(("G1DWLWaitHwReady returned: %d\n", ret));

        /* reset HW */
        SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_IRQ_STAT, 0);
        SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_IRQ, 0);

        DWLDisableHW(pDecCont->dwl, 4 * 1, pDecCont->vp6Regs[1]);

        /* Wait for PP to end also */
        if(pDecCont->pp.ppInstance != NULL &&
           pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING)
        {
            pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;

            TRACE_PP_CTRL("VP6HwdAsicRun: PP Wait for end\n");

            pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);

            TRACE_PP_CTRL("VP6HwdAsicRun: PP Finished\n");
        }

        pDecCont->asicRunning = 0;
        G1G1DWLReleaseHw(pDecCont->dwl);

        return (ret == DWL_HW_WAIT_ERROR) ?
            VP6HWDEC_SYSTEM_ERROR : VP6HWDEC_SYSTEM_TIMEOUT;
    }

    VP6HwdAsicRefreshRegs(pDecCont);

    /* React to the HW return value */

    asic_status = GetDecRegister(pDecCont->vp6Regs, HWIF_DEC_IRQ_STAT);

    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_IRQ_STAT, 0);
    SetDecRegister(pDecCont->vp6Regs, HWIF_DEC_IRQ, 0); /* just in case */

    /* HW done, release it! */
    DWLDisableHW(pDecCont->dwl, 4 * 1, pDecCont->vp6Regs[1]);
    pDecCont->asicRunning = 0;

    /* Wait for PP to end also, this is pipeline case */
    if(pDecCont->pp.ppInstance != NULL &&
       pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING)
    {
        pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;

        TRACE_PP_CTRL("VP6HwdAsicRun: PP Wait for end\n");

        pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);

        TRACE_PP_CTRL("VP6HwdAsicRun: PP Finished\n");
    }

    G1G1DWLReleaseHw(pDecCont->dwl);

    if( pDecCont->refBufSupport &&
        (asic_status & DEC_8190_IRQ_RDY) &&
        pDecCont->asicRunning == 0 )
    {
        RefbuMvStatistics( &pDecCont->refBufferCtrl, 
                            pDecCont->vp6Regs,
                            NULL, HANTRO_FALSE,
                            pDecCont->pb.FrameType == BASE_FRAME );
    }

    return asic_status;
}

/*------------------------------------------------------------------------------
    Function name   : VP6HwdAsicProbUpdate
    Description     : 
    Return type     : void 
    Argument        : VP6DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP6HwdAsicProbUpdate(VP6DecContainer_t * pDecCont)
{
    u8 *dst;
    const u8 *src;
    u32 i, j, z, q;
    i32 r;
    PB_INSTANCE *pbi = &pDecCont->pb;

    u32 *asicProbBase = pDecCont->asicBuff->probTbl.virtualAddress;

    /* prob mode same as last */
    dst = ((u8 *) asicProbBase) + probSameAsLastOffset;

    r = 3;
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 10; j++)
        {
            dst[r] = pbi->probModeSame[i][j];
            r--;
            if( r < 0 )
            {
                r = 3;
                dst += 4;
            }
        }
    }

    /* prob mode */
    dst = ((u8 *) asicProbBase) + probModeOffset;

    r = 3;
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 10; j++)
        {
            for(z = 0; z < 8; z++)
            {
                dst[r] = pbi->probMode[i][j][z];
                r--;
                if( r < 0 )
                {
                    r = 3;
                    dst += 4;
                }
            }
        }
    }

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 10; j++)
        {
            dst[r] = pbi->probMode[i][j][8];
            r--;
            if( r < 0 )
            {
                r = 3;
                dst += 4;
            }
        }
    }

    /* prob is MV short */
    dst = ((u8 *) asicProbBase) + probMvIsShortOffset;

    dst[3] = pbi->IsMvShortProb[0];
    dst[2] = pbi->IsMvShortProb[1];

    /* prob MV sign */
    /*dst = ((u8 *) asicProbBase) + probMvSignOffset;*/

    dst[1] = pbi->MvSignProbs[0];
    dst[0] = pbi->MvSignProbs[1];

    /* prob MV size */
    dst = ((u8 *) asicProbBase) + probMvSizeOffset;

    r = 3;
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 8; j++)
        {
            dst[r] = pbi->MvSizeProbs[i][j];
            r--;
            if( r < 0 )
            {
                r = 3;
                dst += 4;
            }
        }
    }

    /* prob MV short */
    dst = ((u8 *) asicProbBase) + probMvShortOffset;

    for(j = 0; j < 4; j++)
        dst[3-j] = pbi->MvShortProbs[0][j];
    dst += 4;
    for(j = 4; j < 7; j++)
        dst[7-j] = pbi->MvShortProbs[0][j];
    dst += 4;  /* enhance to new 8 aligned address */
    for(j = 0; j < 4; j++)
        dst[3-j] = pbi->MvShortProbs[1][j];
    dst += 4;
    for(j = 4; j < 7; j++)
        dst[7-j] = pbi->MvShortProbs[1][j];

    /* DCT coeff probs */
    if(pbi->UseHuffman)
    {
        dst = ((u8 *) asicProbBase) + huffmanTblDCOffset;
        r = 2;
        for( i = 0 ; i < 2 ; ++i )
        {
            for( j = 0 ; j < 12 ; ++j )
            {
                dst[ r ] = pbi->huff->DcHuffLUT[i][j] & 0xFF;
                dst[r+1] = pbi->huff->DcHuffLUT[i][j] >> 8;
                r -= 2;
                if( r )
                {
                    r = 2;
                    dst += 4;
                }
            }
        }
        dst = ((u8 *) asicProbBase) + huffmanTblACZeroRunOffset;
        r = 2;
        for( i = 0 ; i < 2 ; ++i )
        {
            for( j = 0 ; j < 12 ; ++j )
            {
                dst[ r ] = pbi->huff->ZeroHuffLUT[i][j] & 0xFF;
                dst[r+1] = pbi->huff->ZeroHuffLUT[i][j] >> 8;
                r -= 2;
                if( r )
                {
                    r = 2;
                    dst += 4;
                }
            }
        }
        dst = ((u8 *) asicProbBase) + huffmanTblACOffset;
        r = 2;
        for( i = 0 ; i < 2 ; ++i )
        {
            for( j = 0 ; j < 3 ; ++j )
            {
                for( z = 0 ; z < 4 ; ++z )
                {
                    for( q = 0 ; q < 12 ; ++q )
                    {
                        dst[ r ] = pbi->huff->AcHuffLUT[i][j][z][q] & 0xFF;
                        dst[r+1] = pbi->huff->AcHuffLUT[i][j][z][q] >> 8;
                        r -= 2;
                        if( r )
                        {
                            r = 2;
                            dst += 4;
                        }
                    }
                }
            }
        }
        return;
    }

    /* prob DC context (first 4 probs) */
    dst = ((u8 *) asicProbBase) + probDCFirstOffset;
    src = pbi->DcNodeContexts;

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 3; j++)
        {
            dst[3] = src[0];
            dst[2] = src[1];
            dst[1] = src[2];
            dst[0] = src[3];
            dst += 4;
            src += 5;
        }
    }

    /* prob AC (first 4 probs) */
    dst = ((u8 *) asicProbBase) + probACFirstOffset;
    src = pbi->AcProbs;

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 3; j++)
        {
            for(z = 0; z < 6; z++)
            {
                dst[3] = src[0];
                dst[2] = src[1];
                dst[1] = src[2];
                dst[0] = src[3];
                dst += 4;
                src += 4+7;
            }

        }
    }

    /* prob AC zero run (first 4 probs) */
    dst = ((u8 *) asicProbBase) + probACZeroRunFirstOffset;
    for(i = 0; i < 2; i++)
    {
        dst[3] = pbi->ZeroRunProbs[i][0];
        dst[2] = pbi->ZeroRunProbs[i][1];
        dst[1] = pbi->ZeroRunProbs[i][2];
        dst[0] = pbi->ZeroRunProbs[i][3];
        dst += 4;
    }

    /* prob DC context (last 7 probs)  */
    dst = ((u8 *) asicProbBase) + probDCRestOffset;
    src = pbi->DcNodeContexts;

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 3; j++)
        {
            src += 4;   /* last prob */

            dst[3] = src[0];
            dst[2] = pbi->DcProbs[i * 11 + 5 + 0];
            dst[1] = pbi->DcProbs[i * 11 + 5 + 1];
            dst[0] = pbi->DcProbs[i * 11 + 5 + 2];
            dst[7] = pbi->DcProbs[i * 11 + 5 + 3];
            dst[6] = pbi->DcProbs[i * 11 + 5 + 4];
            dst[5] = pbi->DcProbs[i * 11 + 5 + 5];
            dst += 8;
            src++;
        }
    }

    /* prob AC (last 7 probs) */
    dst = ((u8 *) asicProbBase) + probACRestOffset;
    src = pbi->AcProbs;

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 3; j++)
        {
            for(z = 0; z < 6; z++)
            {
                src += 4;   /* last 7 probs */
                dst[3] = src[0];
                dst[2] = src[1];
                dst[1] = src[2];
                dst[0] = src[3];
                dst[7] = src[4];
                dst[6] = src[5];
                dst[5] = src[6];
                src += 7;
                dst += 8;
            }

        }
    }

    /* prob AC zero run (last 10 probs) */
    dst = ((u8 *) asicProbBase) + probACZeroRunRestOffset;

    r = 3;
    for(j = 4; j < 14; j++)
    {
        dst[r] = pbi->ZeroRunProbs[0][j];
        r--;
        if( r < 0 )
        {
            r = 3;
            dst += 4;
        }
    }

    dst += 8;   /* enhance to new 8 aligned address */
    r = 3;
    for(j = 4; j < 14; j++)
    {
        dst[r] = pbi->ZeroRunProbs[1][j];
        r--;
        if( r < 0 )
        {
            r = 3;
            dst += 4;
        }
    }
}

