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
-  $RCSfile: vp8hwd_asic.c,v $
-  $Revision: 1.34 $
-  $Date: 2011/01/25 13:37:58 $
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "dwl.h"
#include "regdrv.h"
#include "vp8hwd_container.h"
#include "vp8hwd_asic.h"
#include "vp8hwd_debug.h"
#include "tiledref.h"

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#include <stdio.h>
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

#ifdef ASIC_TRACE_SUPPORT
#endif

#define SCAN(i)         HWIF_SCAN_MAP_ ## i

static const u32 ScanTblRegId[16] = { 0 /* SCAN(0) */ ,
    SCAN(1), SCAN(2), SCAN(3), SCAN(4), SCAN(5), SCAN(6), SCAN(7), SCAN(8),
    SCAN(9), SCAN(10), SCAN(11), SCAN(12), SCAN(13), SCAN(14), SCAN(15)
};

#define BASE(i)         HWIF_DCT_STRM ## i ## _BASE
static const u32 DctBaseId[] = { HWIF_VP6HWPART2_BASE,
    BASE(1), BASE(2), BASE(3), BASE(4), BASE(5), BASE(6), BASE(7) };

#define OFFSET(i)         HWIF_DCT ## i ## _START_BIT
static const u32 DctStartBit[] = { HWIF_STRM_START_BIT,
    OFFSET(1), OFFSET(2), OFFSET(3), OFFSET(4),
    OFFSET(5), OFFSET(6), OFFSET(7) };

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

static const u32 mcFilter[8][6] =
{
    { 0,  0,  128,    0,   0,  0 },
    { 0, -6,  123,   12,  -1,  0 },
    { 2, -11, 108,   36,  -8,  1 },
    { 0, -9,   93,   50,  -6,  0 },
    { 3, -16,  77,   77, -16,  3 },
    { 0, -6,   50,   93,  -9,  0 },
    { 1, -8,   36,  108, -11,  2 },
    { 0, -1,   12,  123,  -6,  0 } };

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

#define PROB_TABLE_SIZE  (1<<16) /* TODO */

#define DEC_MODE_VP7  9
#define DEC_MODE_VP8 10

static void VP8HwdAsicRefreshRegs(VP8DecContainer_t * pDecCont);
static void VP8HwdAsicFlushRegs(VP8DecContainer_t * pDecCont);
static void vp8hwdUpdateOutBase(VP8DecContainer_t *pDecCont);

/* VP7 QP LUTs */
static const u16 YDcQLookup[128] = 
{
     4,    4,    5,    6,    6,    7,    8,    8,
     9,   10,   11,   12,   13,   14,   15,   16, 
    17,   18,   19,   20,   21,   22,   23,   23,
    24,   25,   26,   27,   28,   29,   30,   31, 
    32,   33,   33,   34,   35,   36,   36,   37,
    38,   39,   39,   40,   41,   41,   42,   43, 
    43,   44,   45,   45,   46,   47,   48,   48,
    49,   50,   51,   52,   53,   53,   54,   56, 
    57,   58,   59,   60,   62,   63,   65,   66,
    68,   70,   72,   74,   76,   79,   81,   84, 
    87,   90,   93,   96,  100,  104,  108,  112,
   116,  121,  126,  131,  136,  142,  148,  154, 
   160,  167,  174,  182,  189,  198,  206,  215,
   224,  234,  244,  254,  265,  277,  288,  301, 
   313,  327,  340,  355,  370,  385,  401,  417,
   434,  452,  470,  489,  509,  529,  550,  572
};

static const u16 YAcQLookup[128] = 
{
     4,    4,    5,    5,    6,    6,    7,    8,
     9,   10,   11,   12,   13,   15,   16,   17, 
    19,   20,   22,   23,   25,   26,   28,   29,
    31,   32,   34,   35,   37,   38,   40,   41, 
    42,   44,   45,   46,   48,   49,   50,   51,
    53,   54,   55,   56,   57,   58,   59,   61, 
    62,   63,   64,   65,   67,   68,   69,   70,
    72,   73,   75,   76,   78,   80,   82,   84, 
    86,   88,   91,   93,   96,   99,  102,  105,
   109,  112,  116,  121,  125,  130,  135,  140, 
   146,  152,  158,  165,  172,  180,  188,  196,
   205,  214,  224,  234,  245,  256,  268,  281, 
   294,  308,  322,  337,  353,  369,  386,  404,
   423,  443,  463,  484,  506,  529,  553,  578, 
   604,  631,  659,  688,  718,  749,  781,  814,
   849,  885,  922,  960, 1000, 1041, 1083, 1127 

};

static const u16 Y2DcQLookup[128] =
{
     7,    9,   11,   13,   15,   17,   19,   21,
    23,   26,   28,   30,   33,   35,   37,   39, 
    42,   44,   46,   48,   51,   53,   55,   57,
    59,   61,   63,   65,   67,   69,   70,   72, 
    74,   75,   77,   78,   80,   81,   83,   84,
    85,   87,   88,   89,   90,   92,   93,   94, 
    95,   96,   97,   99,  100,  101,  102,  104,
   105,  106,  108,  109,  111,  113,  114,  116, 
   118,  120,  123,  125,  128,  131,  134,  137,
   140,  144,  148,  152,  156,  161,  166,  171, 
   176,  182,  188,  195,  202,  209,  217,  225,
   234,  243,  253,  263,  274,  285,  297,  309, 
   322,  336,  350,  365,  381,  397,  414,  432,
   450,  470,  490,  511,  533,  556,  579,  604, 
   630,  656,  684,  713,  742,  773,  805,  838, 
   873,  908,  945,  983, 1022, 1063, 1105, 1148 
};

static const u16 Y2AcQLookup[128] =
{
     7,    9,   11,   13,   16,   18,   21,   24,
    26,   29,   32,   35,   38,   41,   43,   46, 
    49,   52,   55,   58,   61,   64,   66,   69,
    72,   74,   77,   79,   82,   84,   86,   88, 
    91,   93,   95,   97,   98,  100,  102,  104,
   105,  107,  109,  110,  112,  113,  115,  116, 
   117,  119,  120,  122,  123,  125,  127,  128,
   130,  132,  134,  136,  138,  141,  143,  146, 
   149,  152,  155,  158,  162,  166,  171,  175,
   180,  185,  191,  197,  204,  210,  218,  226, 
   234,  243,  252,  262,  273,  284,  295,  308,
   321,  335,  350,  365,  381,  398,  416,  435, 
   455,  476,  497,  520,  544,  569,  595,  622,
   650,  680,  711,  743,  776,  811,  848,  885, 
   925,  965, 1008, 1052, 1097, 1144, 1193, 1244,
  1297, 1351, 1407, 1466, 1526, 1588, 1652, 1719 
};

static const u16 UvDcQLookup[128] = 
{
     4,    4,    5,    6,    6,    7,    8,    8,
     9,   10,   11,   12,   13,   14,   15,   16, 
    17,   18,   19,   20,   21,   22,   23,   23,
    24,   25,   26,   27,   28,   29,   30,   31, 
    32,   33,   33,   34,   35,   36,   36,   37,
    38,   39,   39,   40,   41,   41,   42,   43, 
    43,   44,   45,   45,   46,   47,   48,   48,
    49,   50,   51,   52,   53,   53,   54,   56, 
    57,   58,   59,   60,   62,   63,   65,   66,
    68,   70,   72,   74,   76,   79,   81,   84, 
    87,   90,   93,   96,  100,  104,  108,  112,
   116,  121,  126,  131,  132,  132,  132,  132,  
   132,  132,  132,  132,  132,  132,  132,  132,
   132,  132,  132,  132,  132,  132,  132,  132,  
   132,  132,  132,  132,  132,  132,  132,  132,
   132,  132,  132,  132,  132,  132,  132,  132
};


static const u16 UvAcQLookup[128] = 
{
     4,    4,    5,    5,    6,    6,    7,    8,
     9,   10,   11,   12,   13,   15,   16,   17, 
    19,   20,   22,   23,   25,   26,   28,   29,
    31,   32,   34,   35,   37,   38,   40,   41, 
    42,   44,   45,   46,   48,   49,   50,   51,
    53,   54,   55,   56,   57,   58,   59,   61, 
    62,   63,   64,   65,   67,   68,   69,   70,
    72,   73,   75,   76,   78,   80,   82,   84, 
    86,   88,   91,   93,   96,   99,  102,  105,
   109,  112,  116,  121,  125,  130,  135,  140, 
   146,  152,  158,  165,  172,  180,  188,  196,
   205,  214,  224,  234,  245,  256,  268,  281, 
   294,  308,  322,  337,  353,  369,  386,  404,
   423,  443,  463,  484,  506,  529,  553,  578, 
   604,  631,  659,  688,  718,  749,  781,  814,
   849,  885,  922,  960, 1000, 1041, 1083, 1127
};

#define CLIP3(l, h, v) ((v) < (l) ? (l) : ((v) > (h) ? (h) : (v)))
#define MB_MULTIPLE(x)  (((x)+15)&~15)

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicInit
    Description     : 
    Return type     : void 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdAsicInit(VP8DecContainer_t * pDecCont)
{

    G1DWLmemset(pDecCont->vp8Regs, 0, sizeof(pDecCont->vp8Regs));

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_MODE,
        pDecCont->decMode == VP8HWD_VP7 ?  DEC_MODE_VP7 : DEC_MODE_VP8);

    /* these parameters are defined in deccfg.h */
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_ENDIAN,
                   DEC_X170_OUTPUT_PICTURE_ENDIAN);

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_IN_ENDIAN,
                   DEC_X170_INPUT_DATA_ENDIAN);

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_STRENDIAN_E,
                   DEC_X170_INPUT_STREAM_ENDIAN);

/*
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_TILED_E,
                   DEC_X170_OUTPUT_FORMAT);
                   */

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_MAX_BURST,
                   DEC_X170_BUS_BURST_LENGTH);

    if ((G1DWLReadAsicID() >> 16) == 0x8170U)
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_PRIORITY_MODE,
                       DEC_X170_ASIC_SERVICE_PRIORITY);
    }
    else
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_SCMD_DIS,
            DEC_X170_SCMD_DISABLE);
    }

    DEC_SET_APF_THRESHOLD(pDecCont->vp8Regs);

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_LATENCY,
                   DEC_X170_LATENCY_COMPENSATION);

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_DATA_DISC_E,
                   DEC_X170_DATA_DISCARD_ENABLE);

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUTSWAP32_E,
                   DEC_X170_OUTPUT_SWAP_32_ENABLE);

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_INSWAP32_E,
                   DEC_X170_INPUT_DATA_SWAP_32_ENABLE);

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_STRSWAP32_E,
                   DEC_X170_INPUT_STREAM_SWAP_32_ENABLE);

#if( DEC_X170_HW_TIMEOUT_INT_ENA  != 0)
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_TIMEOUT_E, 1);
#else
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_TIMEOUT_E, 0);
#endif

#if( DEC_X170_INTERNAL_CLOCK_GATING != 0)
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_CLK_GATE_E, 1);
#else
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_CLK_GATE_E, 0);
#endif

#if( DEC_X170_USING_IRQ  == 0)
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_IRQ_DIS, 1);
#else
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_IRQ_DIS, 0);
#endif

    /* set AXI RW IDs */
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_AXI_RD_ID,
        (DEC_X170_AXI_ID_R & 0xFFU));
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_AXI_WR_ID,
        (DEC_X170_AXI_ID_W & 0xFFU));

}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicAllocateMem
    Description     : 
    Return type     : i32 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
i32 VP8HwdAsicAllocateMem(VP8DecContainer_t * pDecCont)
{

    const void *dwl = pDecCont->dwl;
    i32 dwl_ret;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    dwl_ret = G1DWLMallocLinear(dwl, PROB_TABLE_SIZE, &pAsicBuff->probTbl);
    if(dwl_ret != DWL_OK)
    {
        VP8HwdAsicReleaseMem(pDecCont);
        return -1;
    }

    SetDecRegister(pDecCont->vp8Regs, HWIF_VP6HWPROBTBL_BASE,
                   pAsicBuff->probTbl.busAddress);

    return 0;

}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicReleaseMem
    Description     : 
    Return type     : void 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdAsicReleaseMem(VP8DecContainer_t * pDecCont)
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
    Function name   : VP8HwdAsicAllocatePictures
    Description     : 
    Return type     : i32 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
i32 VP8HwdAsicAllocatePictures(VP8DecContainer_t * pDecCont)
{
    const void *dwl = pDecCont->dwl;
    i32 dwl_ret;
    u32 i, count;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    u32 pict_buff_size;

    u32 numMbs;
    u32 memorySize;

    /* allocate segment map */

    numMbs = (pAsicBuff->width>>4)*(pAsicBuff->height>>4);
    memorySize = (numMbs+3)>>2; /* We fit 4 MBs on data into every full byte */
    memorySize = 64*((memorySize + 63) >> 6); /* Round up to next multiple of 64 bytes */

    dwl_ret = G1DWLMallocLinear(dwl, memorySize, &pAsicBuff->segmentMap);

    if(dwl_ret != DWL_OK)
    {
        VP8HwdAsicReleasePictures(pDecCont);
        return -1;
    }

    pAsicBuff->segmentMapSize = memorySize;
    SetDecRegister(pDecCont->vp8Regs, HWIF_SEGMENT_BASE,
                   pAsicBuff->segmentMap.busAddress);
    G1DWLmemset(pAsicBuff->segmentMap.virtualAddress, 
              0, pAsicBuff->segmentMapSize);

    G1DWLmemset(pAsicBuff->pictures, 0, sizeof(pAsicBuff->pictures));

    count = pDecCont->numBuffers;

    if (!pDecCont->sliceHeight)
        pict_buff_size = pAsicBuff->width * pAsicBuff->height * 3 / 2;
    else
        pict_buff_size = pAsicBuff->width *
            (pDecCont->sliceHeight + 1) * 16 * 3 / 2;

    dwl_ret = BqueueInit(&pDecCont->bq, 
                          pDecCont->numBuffers );
    if( dwl_ret != HANTRO_OK )
    {
        VP8HwdAsicReleasePictures(pDecCont);
        return -1;
    }

    if (pDecCont->pp.ppInstance != NULL)
    {
        pDecCont->pp.ppInfo.tiledMode =
            pDecCont->tiledReferenceEnable;
        pDecCont->pp.PPConfigQuery(pDecCont->pp.ppInstance,
                                &pDecCont->pp.ppInfo);
    }
    if (!pDecCont->userMem &&
        !(pDecCont->intraOnly && pDecCont->pp.ppInfo.pipelineAccepted))
    {
        for (i = 0; i < count; i++)
        {
            dwl_ret = G1DWLMallocRefFrm(dwl, pict_buff_size, &pAsicBuff->pictures[i]);
            if(dwl_ret != DWL_OK)
            {
                VP8HwdAsicReleasePictures(pDecCont);
                return -1;
            }
        }
    }

    /* webp/snapshot -> allocate memory for HW above row storage */
    if (pDecCont->intraOnly)
    {
        u32 tmpW = 0;
        ASSERT(count == 1);

        /* width of the memory in macroblocks, has to be multiple of amount
         * of mbs stored/load per access */
        /* TODO: */
        while (tmpW < MAX_SNAPSHOT_WIDTH) tmpW += 120;

        /* TODO: check amount of memory per macroblock for each HW block */
        memorySize = tmpW /* num mbs */ *
                     (  2 + 2 /* streamd (cbf + intra pred modes) */ +
                       16 + 2*8 /* intra pred (luma + cb + cr) */ +
                       16*8 + 2*8*4 /* filterd */ );
        dwl_ret = G1DWLMallocLinear(dwl, memorySize, &pAsicBuff->pictures[1]);
        if(dwl_ret != DWL_OK)
        {
            VP8HwdAsicReleasePictures(pDecCont);
            return -1;
        }
        G1DWLmemset(pAsicBuff->pictures[1].virtualAddress, 0, memorySize);
        SetDecRegister(pDecCont->vp8Regs, HWIF_REFER6_BASE, 
            pAsicBuff->pictures[1].busAddress);
        SetDecRegister(pDecCont->vp8Regs, HWIF_REFER2_BASE, 
            pAsicBuff->pictures[1].busAddress + 4*tmpW);
        SetDecRegister(pDecCont->vp8Regs, HWIF_REFER3_BASE, 
            pAsicBuff->pictures[1].busAddress + 36*tmpW);
    }

    SetDecRegister(pDecCont->vp8Regs, HWIF_PIC_MB_WIDTH, (pAsicBuff->width / 16)&0x1FF);
    SetDecRegister(pDecCont->vp8Regs, HWIF_PIC_MB_HEIGHT_P, (pAsicBuff->height / 16)&0xFF);
    SetDecRegister(pDecCont->vp8Regs, HWIF_PIC_MB_W_EXT, (pAsicBuff->width / 16)>>9);
    SetDecRegister(pDecCont->vp8Regs, HWIF_PIC_MB_H_EXT, (pAsicBuff->height / 16)>>8);

    pAsicBuff->outBufferI = BqueueNext( &pDecCont->bq, 
                                        BQUEUE_UNUSED, BQUEUE_UNUSED, 
                                        BQUEUE_UNUSED, 0 );

    pAsicBuff->outBuffer = &pAsicBuff->pictures[pAsicBuff->outBufferI];
    /* These need to point at something so use the output buffer */
    pAsicBuff->refBuffer        = pAsicBuff->outBuffer;
    pAsicBuff->goldenBuffer     = pAsicBuff->outBuffer;
    pAsicBuff->alternateBuffer  = pAsicBuff->outBuffer;
    pAsicBuff->refBufferI       = BQUEUE_UNUSED;
    pAsicBuff->goldenBufferI    = BQUEUE_UNUSED;
    pAsicBuff->alternateBufferI = BQUEUE_UNUSED;

    return 0;

}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicReleasePictures
    Description     : 
    Return type     : void 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdAsicReleasePictures(VP8DecContainer_t * pDecCont)
{
    u32 i, count;
    const void *dwl = pDecCont->dwl;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    BqueueRelease( &pDecCont->bq );

    count = pDecCont->numBuffers;
    for (i = 0; i < count; i++)
    {
        if(pAsicBuff->pictures[i].virtualAddress != NULL)
        {
            DWLFreeRefFrm(dwl, &pAsicBuff->pictures[i]);
        }
    }

    if (pDecCont->intraOnly)
    {
        if(pAsicBuff->pictures[1].virtualAddress != NULL)
        {
            DWLFreeLinear(dwl, &pAsicBuff->pictures[1]);
        }
    }

    G1DWLmemset(pAsicBuff->pictures, 0, sizeof(pAsicBuff->pictures));

    if(pAsicBuff->segmentMap.virtualAddress != NULL)
    {
        DWLFreeLinear(dwl, &pAsicBuff->segmentMap);
        G1DWLmemset(&pAsicBuff->segmentMap, 0, sizeof(pAsicBuff->segmentMap));
    }

}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicInitPicture
    Description     : 
    Return type     : void 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdAsicInitPicture(VP8DecContainer_t * pDecCont)
{

    vp8Decoder_t *dec = &pDecCont->decoder;
    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

#ifdef SET_EMPTY_PICTURE_DATA   /* USE THIS ONLY FOR DEBUGGING PURPOSES */
    {
        i32 bgd = SET_EMPTY_PICTURE_DATA;

        G1DWLmemset(pAsicBuff->outBuffer->virtualAddress,
                  bgd, (dec->width * dec->height * 3) / 2);
    }
#endif

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_DIS, 0);

    if (pDecCont->intraOnly && pDecCont->pp.ppInfo.pipelineAccepted)
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_DIS, 1);
    }
    else if (!pDecCont->userMem && !pDecCont->sliceHeight)
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_BASE,
                       pAsicBuff->outBuffer->busAddress);

        if (!dec->keyFrame)
            /* previous picture */
            SetDecRegister(pDecCont->vp8Regs, HWIF_REFER0_BASE,
                           pAsicBuff->refBuffer->busAddress);
        else /* chroma output base address */
        {
            SetDecRegister(pDecCont->vp8Regs, HWIF_REFER0_BASE,
                pAsicBuff->outBuffer->busAddress +
                pAsicBuff->width * pAsicBuff->height);
        }
    }
    else
    {
        u32 sliceHeight;

        if (pDecCont->sliceHeight*16 > pAsicBuff->height)
            sliceHeight = pAsicBuff->height/16;
        else
            sliceHeight = pDecCont->sliceHeight;

        SetDecRegister(pDecCont->vp8Regs, HWIF_JPEG_SLICE_H, sliceHeight);

        vp8hwdUpdateOutBase(pDecCont);
    }

    /* golden reference */
    SetDecRegister(pDecCont->vp8Regs, HWIF_REFER4_BASE,
                   pAsicBuff->goldenBuffer->busAddress);
    SetDecRegister(pDecCont->vp8Regs, HWIF_GREF_SIGN_BIAS,
                   dec->refFrameSignBias[0]);

    /* alternate reference */
    SetDecRegister(pDecCont->vp8Regs, HWIF_REFER5_BASE,
                   pAsicBuff->alternateBuffer->busAddress);
    SetDecRegister(pDecCont->vp8Regs, HWIF_AREF_SIGN_BIAS,
                   dec->refFrameSignBias[1]);

    SetDecRegister(pDecCont->vp8Regs, HWIF_PIC_INTER_E, !dec->keyFrame);


    /* mb skip mode [Codec X] */
    SetDecRegister(pDecCont->vp8Regs, HWIF_SKIP_MODE, !dec->coeffSkipMode );

    /* loop filter */
    SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_TYPE, 
        dec->loopFilterType);
    SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_SHARPNESS, 
        dec->loopFilterSharpness);

    if (!dec->segmentationEnabled)
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_0, 
            dec->loopFilterLevel);
    else if (dec->segmentFeatureMode) /* absolute mode */
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_0,
            dec->segmentLoopfilter[0]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_1,
            dec->segmentLoopfilter[1]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_2,
            dec->segmentLoopfilter[2]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_3,
            dec->segmentLoopfilter[3]);
    }
    else /* delta mode */
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_0,
            CLIP3(0, 63, dec->loopFilterLevel + dec->segmentLoopfilter[0]));
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_1,
            CLIP3(0, 63, dec->loopFilterLevel + dec->segmentLoopfilter[1]));
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_2,
            CLIP3(0, 63, dec->loopFilterLevel + dec->segmentLoopfilter[2]));
        SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_LEVEL_3,
            CLIP3(0, 63, dec->loopFilterLevel + dec->segmentLoopfilter[3]));
    }

    SetDecRegister(pDecCont->vp8Regs, HWIF_SEGMENT_E, dec->segmentationEnabled );
    SetDecRegister(pDecCont->vp8Regs, HWIF_SEGMENT_UPD_E, dec->segmentationMapUpdate );

    /* TODO: seems that ref dec does not disable filtering based on version,
     * check */
    /*SetDecRegister(pDecCont->vp8Regs, HWIF_FILTERING_DIS, dec->vpVersion >= 2);*/
    SetDecRegister(pDecCont->vp8Regs, HWIF_FILTERING_DIS,
        dec->loopFilterLevel == 0);

    /* full pell chroma mvs for VP8 version 3 */
    SetDecRegister(pDecCont->vp8Regs, HWIF_CH_MV_RES,
        dec->decMode == VP8HWD_VP7 || dec->vpVersion != 3);

    SetDecRegister(pDecCont->vp8Regs, HWIF_BILIN_MC_E,
        dec->decMode == VP8HWD_VP8 && (dec->vpVersion & 0x3));

    /* first bool decoder status */
    SetDecRegister(pDecCont->vp8Regs, HWIF_BOOLEAN_VALUE,
                   (pDecCont->bc.value >> 24) & (0xFFU));

    SetDecRegister(pDecCont->vp8Regs, HWIF_BOOLEAN_RANGE,
                   pDecCont->bc.range & (0xFFU));

    /* QP */
    if (pDecCont->decMode == VP8HWD_VP7)
    {
        /* LUT */
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_0, YDcQLookup[dec->qpYDc]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_1, YAcQLookup[dec->qpYAc]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_2, Y2DcQLookup[dec->qpY2Dc]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_3, Y2AcQLookup[dec->qpY2Ac]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_4, UvDcQLookup[dec->qpChDc]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_5, UvAcQLookup[dec->qpChAc]);
    }
    else
    {
        if (!dec->segmentationEnabled)
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_0, dec->qpYAc);
        else if (dec->segmentFeatureMode) /* absolute mode */
        {
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_0, dec->segmentQp[0]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_1, dec->segmentQp[1]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_2, dec->segmentQp[2]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_3, dec->segmentQp[3]);
        }
        else /* delta mode */
        {
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_0,
                CLIP3(0, 127, dec->qpYAc + dec->segmentQp[0]));
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_1,
                CLIP3(0, 127, dec->qpYAc + dec->segmentQp[1]));
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_2,
                CLIP3(0, 127, dec->qpYAc + dec->segmentQp[2]));
            SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_3,
                CLIP3(0, 127, dec->qpYAc + dec->segmentQp[3]));
        }
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_DELTA_0, dec->qpYDc);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_DELTA_1, dec->qpY2Dc);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_DELTA_2, dec->qpY2Ac);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_DELTA_3, dec->qpChDc);
        SetDecRegister(pDecCont->vp8Regs, HWIF_QUANT_DELTA_4, dec->qpChAc);

        if (dec->modeRefLfEnabled)
        {
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_REF_ADJ_0,  dec->mbRefLfDelta[0]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_REF_ADJ_1,  dec->mbRefLfDelta[1]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_REF_ADJ_2,  dec->mbRefLfDelta[2]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_REF_ADJ_3,  dec->mbRefLfDelta[3]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_MB_ADJ_0,  dec->mbModeLfDelta[0]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_MB_ADJ_1,  dec->mbModeLfDelta[1]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_MB_ADJ_2,  dec->mbModeLfDelta[2]);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_MB_ADJ_3,  dec->mbModeLfDelta[3]);
        }
        else
        {
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_REF_ADJ_0,  0);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_REF_ADJ_1,  0);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_REF_ADJ_2,  0);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_REF_ADJ_3,  0);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_MB_ADJ_0,  0);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_MB_ADJ_1,  0);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_MB_ADJ_2,  0);
            SetDecRegister(pDecCont->vp8Regs, HWIF_FILT_MB_ADJ_3,  0);
        }
    }

    /* scan order */
    if (pDecCont->decMode == VP8HWD_VP7)
    {
        i32 i;

        for(i = 1; i < 16; i++)
        {
            SetDecRegister(pDecCont->vp8Regs, ScanTblRegId[i],
                           pDecCont->decoder.vp7ScanOrder[i]);
        }
    }

    /* prediction filter taps */
    /* normal 6-tap filters */
    if ((dec->vpVersion & 0x3) == 0 || dec->decMode == VP8HWD_VP7)
    {
        i32 i, j;

        for(i = 0; i < 8; i++)
        {
            for(j = 0; j < 4; j++)
            {
                SetDecRegister(pDecCont->vp8Regs, TapRegId[i][j],
                    mcFilter[i][j+1]);
            }
            if (i == 2)
            {
                SetDecRegister(pDecCont->vp8Regs, HWIF_PRED_TAP_2_M1,
                    mcFilter[i][0]);
                SetDecRegister(pDecCont->vp8Regs, HWIF_PRED_TAP_2_4,
                    mcFilter[i][5]);
            }
            else if (i == 4)
            {
                SetDecRegister(pDecCont->vp8Regs, HWIF_PRED_TAP_4_M1,
                    mcFilter[i][0]);
                SetDecRegister(pDecCont->vp8Regs, HWIF_PRED_TAP_4_4,
                    mcFilter[i][5]);
            }
            else if (i == 6)
            {
                SetDecRegister(pDecCont->vp8Regs, HWIF_PRED_TAP_6_M1,
                    mcFilter[i][0]);
                SetDecRegister(pDecCont->vp8Regs, HWIF_PRED_TAP_6_4,
                    mcFilter[i][5]);
            }
        }
    }
    /* TODO: tarviiko tapit bilinear caselle? */

    if (dec->decMode == VP8HWD_VP7)
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_INIT_DC_COMP0,
            pAsicBuff->dcPred[0]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_INIT_DC_COMP1,
            pAsicBuff->dcPred[1]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_INIT_DC_MATCH0,
            pAsicBuff->dcMatch[0]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_INIT_DC_MATCH1,
            pAsicBuff->dcMatch[1]);
        SetDecRegister(pDecCont->vp8Regs, HWIF_VP7_VERSION,
            dec->vpVersion != 0);
    }

    /* Setup reference picture buffer */
    if( pDecCont->refBufSupport && !pDecCont->intraOnly )
    {
        u32 cntLast = 0, 
            cntGolden = 0, 
            cntAlt = 0;
        u32 cntBest;
        u32 mul = 0;
        u32 allMbs = 0;
        u32 bufPicId = 0;
        u32 forceDisable = 0;
        u32 tmp;
        u32 cov;
        u32 intraMbs;
        u32 flags = 0;
        u32 threshold;

        if(!dec->keyFrame)
        {            
            /* Calculate most probable reference frame */

#define CALCULATE_SHARE(range, prob) \
    (((range)*(prob-1))/254)

            allMbs = mul = dec->width * dec->height >> 8;        /* All mbs in frame */

            /* Deduct intra MBs */
            intraMbs = CALCULATE_SHARE(mul, dec->probIntra);
            mul = allMbs - intraMbs;

            cntLast = CALCULATE_SHARE(mul, dec->probRefLast);
            if( pDecCont->decMode == VP8HWD_VP8 )
            {
                mul -= cntLast;     /* What's left is mbs either Golden or Alt */
                cntGolden = CALCULATE_SHARE(mul, dec->probRefGolden);
                cntAlt = mul - cntGolden;
            }
            else
            {
                cntGolden = mul - cntLast; /* VP7 has no Alt ref frame */
                cntAlt = 0;
            }

#undef CALCULATE_SHARE

            /* Select best reference frame */

            if(cntLast > cntGolden)
            {
                tmp = (cntLast > cntAlt);
                bufPicId = tmp ? 0 : 5;
                cntBest  = tmp ? cntLast : cntAlt;
            }
            else
            {
                tmp = (cntGolden > cntAlt);
                bufPicId = tmp ? 4 : 5;
                cntBest  = tmp ? cntGolden : cntAlt;
            }

            /* Check against refbuf-calculated threshold value; if  it 
             * seems we'll miss our target, then don't bother enabling
             * the feature at all... */
            threshold = RefbuGetHitThreshold(&pDecCont->refBufferCtrl);
            threshold *= (dec->height/16);
            threshold /= 4;
            if(cntBest < threshold)
                forceDisable = 1;

            /* Next frame has enough reference picture hits, now also take
             * actual hits and intra blocks into calculations... */
            if(!forceDisable)
            {
                cov = RefbuVpxGetPrevFrameStats(&pDecCont->refBufferCtrl);

                /* If we get prediction for coverage, we can disable checkpoint
                 * and do calculations here */
                if( cov > 0 )
                {
                    /* Calculate percentage of hits from last frame, multiply 
                     * predicted reference frame referrals by it and compare.
                     * Note: if refbuffer was off for previous frame, we might
                     *    not get an accurate estimation... */

                    tmp = dec->refbuPredHits;
                    if(tmp)     cov = (256*cov)/tmp;
                    else        cov = 0;
                    cov = (cov*cntBest)>>8;

                    if( cov < threshold)
                        forceDisable = 1;
                    else
                        flags = REFBU_DISABLE_CHECKPOINT;
                }
            }
            dec->refbuPredHits = cntBest;
        }
        else
        {
            dec->refbuPredHits = 0;
        }

        RefbuSetup( &pDecCont->refBufferCtrl, pDecCont->vp8Regs, 
                    REFBU_FRAME, 
                    dec->keyFrame || forceDisable,
                    HANTRO_FALSE, 
                    bufPicId, 0, 
                    flags );
    }

    if( pDecCont->tiledModeSupport && !pDecCont->intraOnly)
    {
        pDecCont->tiledReferenceEnable = 
            DecSetupTiledReference( pDecCont->vp8Regs, 
                pDecCont->tiledModeSupport,
                0 );
    }
    else
    {
        pDecCont->tiledReferenceEnable = 0;
    }
}

/*------------------------------------------------------------------------------
    Function name : VP8HwdAsicStrmPosUpdate
    Description   : Set stream base and length related registers

    Return type   :
    Argument      : container
------------------------------------------------------------------------------*/
void VP8HwdAsicStrmPosUpdate(VP8DecContainer_t * pDecCont, u32 strmBusAddress)
{
    u32 i, tmp;
    u32 hwBitPos;
    u32 tmpAddr;
    u32 tmp2;
    u32 byteOffset;
    u32 extraBytesPacked = 0;
    vp8Decoder_t *dec = &pDecCont->decoder;

    DEBUG_PRINT(("VP8HwdAsicStrmPosUpdate:\n"));

    /* TODO: miksi bitin tarkkuudella (count) kun kuitenki luetaan tavu
     * kerrallaan? Vai lukeeko HW eri lailla? */
    /* start of control partition */
    tmp = (pDecCont->bc.pos) * 8 + (8 - pDecCont->bc.count);

    if (dec->frameTagSize == 4) tmp+=8;

    if(pDecCont->decMode == VP8HWD_VP8 &&
       pDecCont->decoder.keyFrame)
        extraBytesPacked += 7;

    tmp += extraBytesPacked*8;

    tmpAddr = strmBusAddress + tmp/8;
    hwBitPos = (tmpAddr & DEC_8190_ALIGN_MASK) * 8;
    tmpAddr &= (~DEC_8190_ALIGN_MASK);  /* align the base */

    hwBitPos += tmp & 0x7;

    /* control partition */
    SetDecRegister(pDecCont->vp8Regs, HWIF_VP6HWPART1_BASE, tmpAddr);
    SetDecRegister(pDecCont->vp8Regs, HWIF_STRM1_START_BIT, hwBitPos);

    /* total stream length */
    /*tmp = pDecCont->bc.streamEndPos - (tmpAddr - strmBusAddress);*/

    /* calculate dct partition length here instead */
    tmp = pDecCont->bc.streamEndPos + dec->frameTagSize - dec->dctPartitionOffsets[0];
    tmp += (dec->nbrDctPartitions-1)*3;
    tmp2 = strmBusAddress + extraBytesPacked + dec->dctPartitionOffsets[0];
    tmp += (tmp2 & 0x7);

    SetDecRegister(pDecCont->vp8Regs, HWIF_STREAM_LEN, tmp);

    /* control partition length */
    tmp = dec->offsetToDctParts + dec->frameTagSize - (tmpAddr - strmBusAddress - extraBytesPacked);
    if(pDecCont->decMode == VP8HWD_VP7) /* give extra byte for VP7 to pass test cases */
        tmp ++;

    SetDecRegister(pDecCont->vp8Regs, HWIF_STREAM1_LEN, tmp);

    /* number of dct partitions */
    SetDecRegister(pDecCont->vp8Regs, HWIF_COEFFS_PART_AM, 
        dec->nbrDctPartitions-1);

    /* base addresses and bit offsets of dct partitions */
    for (i = 0; i < dec->nbrDctPartitions; i++)
    {
        tmpAddr = strmBusAddress + extraBytesPacked + dec->dctPartitionOffsets[i];
        byteOffset = tmpAddr & 0x7;
        SetDecRegister(pDecCont->vp8Regs, DctBaseId[i], tmpAddr & 0xFFFFFFF8);
        SetDecRegister(pDecCont->vp8Regs, DctStartBit[i], byteOffset * 8);
    }


}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicRefreshRegs
    Description     :
    Return type     : void
    Argument        : decContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdAsicRefreshRegs(VP8DecContainer_t * pDecCont)
{
    i32 i;
    u32 offset = 0x0;

    u32 *decRegs = pDecCont->vp8Regs;

    for(i = DEC_X170_REGISTERS; i > 0; --i)
    {
        *decRegs++ = G1DWLReadReg(pDecCont->dwl, offset);
        offset += 4;
    }
}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicFlushRegs
    Description     :
    Return type     : void
    Argument        : decContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdAsicFlushRegs(VP8DecContainer_t * pDecCont)
{
    i32 i;
    u32 offset = 0x04;
    const u32 *decRegs = pDecCont->vp8Regs + 1;

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
    Function name   : VP8HwdAsicRun
    Description     : 
    Return type     : u32 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
u32 VP8HwdAsicRun(VP8DecContainer_t * pDecCont)
{
    u32 asic_status = 0;
    i32 ret;

    if (!pDecCont->asicRunning)
    {
        ret = G1DWLReserveHw(pDecCont->dwl);
        if(ret != DWL_OK)
        {
            return VP8HWDEC_HW_RESERVED;
        }

        pDecCont->asicRunning = 1;

        if(pDecCont->pp.ppInstance != NULL &&
           pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING)
        {
            DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;
            DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

            TRACE_PP_CTRL("VP8HwdAsicRun: PP Run\n");

            decPpIf->inwidth  = MB_MULTIPLE(pDecCont->width);
            decPpIf->inheight = MB_MULTIPLE(pDecCont->height);
            decPpIf->croppedW = decPpIf->inwidth;
            decPpIf->croppedH = decPpIf->inheight;

            /* forward tiled mode */
            decPpIf->tiledInputMode = pDecCont->tiledReferenceEnable;
            decPpIf->progressiveSequence = 1;

            decPpIf->picStruct = DECPP_PIC_FRAME_OR_TOP_FIELD;
            decPpIf->littleEndian =
                GetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_ENDIAN);
            decPpIf->wordSwap =
                GetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUTSWAP32_E);

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
        VP8HwdAsicFlushRegs(pDecCont);

        SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_E, 1);
        DWLEnableHW(pDecCont->dwl, 4 * 1, pDecCont->vp8Regs[1]);
    }
    else
    {
        G1DWLWriteReg(pDecCont->dwl, 4 * 13, pDecCont->vp8Regs[13]);
        G1DWLWriteReg(pDecCont->dwl, 4 * 14, pDecCont->vp8Regs[14]);
        G1DWLWriteReg(pDecCont->dwl, 4 * 15, pDecCont->vp8Regs[15]);
        G1DWLWriteReg(pDecCont->dwl, 4 * 1, pDecCont->vp8Regs[1]);
    }


    ret = G1DWLWaitHwReady(pDecCont->dwl, (u32) DEC_X170_TIMEOUT_LENGTH);

    if(ret != DWL_HW_WAIT_OK)
    {
        ERROR_PRINT("G1DWLWaitHwReady");
        DEBUG_PRINT(("G1DWLWaitHwReady returned: %d\n", ret));

        /* reset HW */
        SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_IRQ_STAT, 0);
        SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_IRQ, 0);

        DWLDisableHW(pDecCont->dwl, 4 * 1, pDecCont->vp8Regs[1]);

        /* Wait for PP to end also */
        if(pDecCont->pp.ppInstance != NULL &&
           pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING)
        {
            pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;

            TRACE_PP_CTRL("VP8HwdAsicRun: PP Wait for end\n");

            pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);

            TRACE_PP_CTRL("VP8HwdAsicRun: PP Finished\n");
        }

        pDecCont->asicRunning = 0;
        G1G1DWLReleaseHw(pDecCont->dwl);

        return (ret == DWL_HW_WAIT_ERROR) ?
            VP8HWDEC_SYSTEM_ERROR : VP8HWDEC_SYSTEM_TIMEOUT;
    }

    VP8HwdAsicRefreshRegs(pDecCont);

    /* React to the HW return value */

    asic_status = GetDecRegister(pDecCont->vp8Regs, HWIF_DEC_IRQ_STAT);

    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_IRQ_STAT, 0);
    SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_IRQ, 0); /* just in case */

    if (pDecCont->decoder.decMode == VP8HWD_VP7)
    {
        pDecCont->asicBuff->dcPred[0] =
            GetDecRegister(pDecCont->vp8Regs, HWIF_INIT_DC_COMP0);
        pDecCont->asicBuff->dcPred[1] =
            GetDecRegister(pDecCont->vp8Regs, HWIF_INIT_DC_COMP1);
        pDecCont->asicBuff->dcMatch[0] =
            GetDecRegister(pDecCont->vp8Regs, HWIF_INIT_DC_MATCH0);
        pDecCont->asicBuff->dcMatch[1] =
            GetDecRegister(pDecCont->vp8Regs, HWIF_INIT_DC_MATCH1);
    }
    if (!(asic_status & DEC_8190_IRQ_SLICE)) /* not slice interrupt */
    {
        /* HW done, release it! */
        DWLDisableHW(pDecCont->dwl, 4 * 1, pDecCont->vp8Regs[1]);
        pDecCont->asicRunning = 0;

        /* Wait for PP to end also, this is pipeline case */
        if(pDecCont->pp.ppInstance != NULL &&
           pDecCont->pp.decPpIf.ppStatus == DECPP_RUNNING)
        {
            pDecCont->pp.decPpIf.ppStatus = DECPP_PIC_READY;

            TRACE_PP_CTRL("VP8HwdAsicRun: PP Wait for end\n");

            pDecCont->pp.PPDecWaitEnd(pDecCont->pp.ppInstance);

            TRACE_PP_CTRL("VP8HwdAsicRun: PP Finished\n");
        }

        G1G1DWLReleaseHw(pDecCont->dwl);

        if( pDecCont->refBufSupport &&
            (asic_status & DEC_8190_IRQ_RDY) &&
            pDecCont->asicRunning == 0 )
        {
            RefbuMvStatistics( &pDecCont->refBufferCtrl, 
                                pDecCont->vp8Regs,
                                NULL, HANTRO_FALSE,
                                pDecCont->decoder.keyFrame );
        }
    }

    return asic_status;
}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicProbUpdate
    Description     : 
    Return type     : void 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdAsicProbUpdate(VP8DecContainer_t * pDecCont)
{
    u8 *dst;
    const u8 *src;
    u32 i, j, k, l;
    i32 r;
    u32 *asicProbBase = pDecCont->asicBuff->probTbl.virtualAddress;

    /* first probs */
    dst = ((u8 *) asicProbBase);

    dst[3] = pDecCont->decoder.probMbSkipFalse;
    dst[2] = pDecCont->decoder.probIntra;
    dst[1] = pDecCont->decoder.probRefLast;
    dst[0] = pDecCont->decoder.probRefGolden;
    dst[7] = pDecCont->decoder.probSegment[0];
    dst[6] = pDecCont->decoder.probSegment[1];
    dst[5] = pDecCont->decoder.probSegment[2];
    dst[4] = 0; /*unused*/

    dst += 8;
    dst[3] = pDecCont->decoder.entropy.probLuma16x16PredMode[0];
    dst[2] = pDecCont->decoder.entropy.probLuma16x16PredMode[1];
    dst[1] = pDecCont->decoder.entropy.probLuma16x16PredMode[2];
    dst[0] = pDecCont->decoder.entropy.probLuma16x16PredMode[3];
    dst[7] = pDecCont->decoder.entropy.probChromaPredMode[0];
    dst[6] = pDecCont->decoder.entropy.probChromaPredMode[1];
    dst[5] = pDecCont->decoder.entropy.probChromaPredMode[2];
    dst[4] = 0; /*unused*/

    /* mv probs */
    dst += 8;
    dst[3] = pDecCont->decoder.entropy.probMvContext[0][0]; /* is short */
    dst[2] = pDecCont->decoder.entropy.probMvContext[1][0];
    dst[1] = pDecCont->decoder.entropy.probMvContext[0][1]; /* sign */
    dst[0] = pDecCont->decoder.entropy.probMvContext[1][1];
    dst[7] = pDecCont->decoder.entropy.probMvContext[0][8+9];
    dst[6] = pDecCont->decoder.entropy.probMvContext[0][9+9];
    dst[5] = pDecCont->decoder.entropy.probMvContext[1][8+9];
    dst[4] = pDecCont->decoder.entropy.probMvContext[1][9+9];
    dst += 8;
    for( i = 0 ; i < 2 ; ++i )
    {
        for( j = 0 ; j < 8 ; j+=4 )
        {
            dst[3] = pDecCont->decoder.entropy.probMvContext[i][j+9+0];
            dst[2] = pDecCont->decoder.entropy.probMvContext[i][j+9+1];
            dst[1] = pDecCont->decoder.entropy.probMvContext[i][j+9+2];
            dst[0] = pDecCont->decoder.entropy.probMvContext[i][j+9+3];
            dst += 4;
        }
    }
    for( i = 0 ; i < 2 ; ++i )
    {
        dst[3] = pDecCont->decoder.entropy.probMvContext[i][0+2];
        dst[2] = pDecCont->decoder.entropy.probMvContext[i][1+2];
        dst[1] = pDecCont->decoder.entropy.probMvContext[i][2+2];
        dst[0] = pDecCont->decoder.entropy.probMvContext[i][3+2];
        dst[7] = pDecCont->decoder.entropy.probMvContext[i][4+2];
        dst[6] = pDecCont->decoder.entropy.probMvContext[i][5+2];
        dst[5] = pDecCont->decoder.entropy.probMvContext[i][6+2];
        dst[4] = 0; /*unused*/
        dst += 8;
    }

    /* coeff probs (header part) */
    dst = ((u8 *) asicProbBase) + 8*7; 
    for( i = 0 ; i < 4 ; ++i )
    {
        for( j = 0 ; j < 8 ; ++j )
        {
            for( k = 0 ; k < 3 ; ++k )
            {
                dst[3] = pDecCont->decoder.entropy.probCoeffs[i][j][k][0];
                dst[2] = pDecCont->decoder.entropy.probCoeffs[i][j][k][1];
                dst[1] = pDecCont->decoder.entropy.probCoeffs[i][j][k][2];
                dst[0] = pDecCont->decoder.entropy.probCoeffs[i][j][k][3];
                dst += 4;
            }
        }
    }

    /* coeff probs (footer part) */
    dst = ((u8 *) asicProbBase) + 8*55; 
    for( i = 0 ; i < 4 ; ++i )
    {
        for( j = 0 ; j < 8 ; ++j )
        {
            for( k = 0 ; k < 3 ; ++k )
            {
                dst[3] = pDecCont->decoder.entropy.probCoeffs[i][j][k][4];
                dst[2] = pDecCont->decoder.entropy.probCoeffs[i][j][k][5];
                dst[1] = pDecCont->decoder.entropy.probCoeffs[i][j][k][6];
                dst[0] = pDecCont->decoder.entropy.probCoeffs[i][j][k][7];
                dst[7] = pDecCont->decoder.entropy.probCoeffs[i][j][k][8];
                dst[6] = pDecCont->decoder.entropy.probCoeffs[i][j][k][9];
                dst[5] = pDecCont->decoder.entropy.probCoeffs[i][j][k][10];
                dst[4] = 0; /*unused*/
                dst += 8;
            }
        }
    }


}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdUpdateRefs
    Description     : 
    Return type     : void 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdUpdateRefs(VP8DecContainer_t * pDecCont, u32 corrupted)
{

    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    if (pDecCont->decoder.copyBufferToAlternate == 1)
    {
        pAsicBuff->alternateBuffer = pAsicBuff->refBuffer;
        pAsicBuff->alternateBufferI = pAsicBuff->refBufferI;
    }
    else if (pDecCont->decoder.copyBufferToAlternate == 2)
    {
        pAsicBuff->alternateBuffer = pAsicBuff->goldenBuffer;
        pAsicBuff->alternateBufferI = pAsicBuff->goldenBufferI;
    }

    if (pDecCont->decoder.copyBufferToGolden == 1)
    {
        pAsicBuff->goldenBuffer = pAsicBuff->refBuffer;
        pAsicBuff->goldenBufferI = pAsicBuff->refBufferI;
    }
    else if (pDecCont->decoder.copyBufferToGolden == 2)
    {
        pAsicBuff->goldenBuffer = pAsicBuff->alternateBuffer;
        pAsicBuff->goldenBufferI = pAsicBuff->alternateBufferI;
    }


    if (!corrupted)
    {
        if(pDecCont->decoder.refreshGolden)
        {
            pAsicBuff->goldenBuffer = pAsicBuff->outBuffer;
            pAsicBuff->goldenBufferI = pAsicBuff->outBufferI;
        }

        if(pDecCont->decoder.refreshAlternate)
        {
            pAsicBuff->alternateBuffer = pAsicBuff->outBuffer;
            pAsicBuff->alternateBufferI = pAsicBuff->outBufferI;
        }

        if (pDecCont->decoder.refreshLast)
        {
            pAsicBuff->refBuffer = pAsicBuff->outBuffer;
            pAsicBuff->refBufferI = pAsicBuff->outBufferI;
        }
    }

}

void vp8hwdUpdateOutBase(VP8DecContainer_t *pDecCont)
{

    DecAsicBuffers_t *pAsicBuff = pDecCont->asicBuff;

    ASSERT(pDecCont->intraOnly);

    if (pDecCont->userMem)
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_BASE,
                       pAsicBuff->userMem.picBufferBusAddrY);
        SetDecRegister(pDecCont->vp8Regs, HWIF_REFER0_BASE,
                       pAsicBuff->userMem.picBufferBusAddrC);
    }
    else/* if (startOfPic)*/
    {
        SetDecRegister(pDecCont->vp8Regs, HWIF_DEC_OUT_BASE,
                       pAsicBuff->outBuffer->busAddress);

        SetDecRegister(pDecCont->vp8Regs, HWIF_REFER0_BASE,
            pAsicBuff->outBuffer->busAddress +
            pAsicBuff->width *
            (pDecCont->sliceHeight ? (pDecCont->sliceHeight + 1)*16 :
             pAsicBuff->height));
    }
}

/*------------------------------------------------------------------------------
    Function name   : VP8HwdAsicContPicture
    Description     : 
    Return type     : void 
    Argument        : VP8DecContainer_t * pDecCont
------------------------------------------------------------------------------*/
void VP8HwdAsicContPicture(VP8DecContainer_t * pDecCont)
{

    u32 sliceHeight;

    /* update output picture buffer if not pipeline */
    if(pDecCont->pp.ppInstance == NULL ||
      !pDecCont->pp.decPpIf.usePipeline)
        vp8hwdUpdateOutBase(pDecCont);

    /* slice height */
    if (pDecCont->totDecodedRows + pDecCont->sliceHeight >
        pDecCont->asicBuff->height/16)
        sliceHeight = pDecCont->asicBuff->height/16 - pDecCont->totDecodedRows;
    else
        sliceHeight = pDecCont->sliceHeight;

    SetDecRegister(pDecCont->vp8Regs, HWIF_JPEG_SLICE_H, sliceHeight);
    
}
