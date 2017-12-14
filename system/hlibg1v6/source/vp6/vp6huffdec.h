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
-  $RCSfile: vp6huffdec.h,v $
-  $Revision: 1.4 $
-  $Date: 2008/04/28 13:25:25 $
-
------------------------------------------------------------------------------*/

#ifndef __VP6HUFFDEC_H__
#define __VP6HUFFDEC_H__

// VP6 hufman table AC bands
#define VP6HWAC_BANDS			6

// Tokens                               Value       Extra Bits (range + sign)
#define ZERO_TOKEN              0   //0         Extra Bits 0+0
#define ONE_TOKEN               1   //1         Extra Bits 0+1
#define TWO_TOKEN               2   //2         Extra Bits 0+1
#define THREE_TOKEN             3   //3         Extra Bits 0+1
#define FOUR_TOKEN              4   //4         Extra Bits 0+1
#define DCT_VAL_CATEGORY1       5   //5-6       Extra Bits 1+1
#define DCT_VAL_CATEGORY2		6   //7-10      Extra Bits 2+1
#define DCT_VAL_CATEGORY3		7   //11-26     Extra Bits 4+1
#define DCT_VAL_CATEGORY4		8   //11-26     Extra Bits 5+1
#define DCT_VAL_CATEGORY5		9   //27-58     Extra Bits 5+1
#define DCT_VAL_CATEGORY6		10  //59+       Extra Bits 11+1
#define DCT_EOB_TOKEN           11  //EOB       Extra Bits 0+0
#define MAX_ENTROPY_TOKENS      (DCT_EOB_TOKEN + 1)
#define ILLEGAL_TOKEN			255

#define DC_TOKEN_CONTEXTS		3   // 00, 0!0, !0!0
#define CONTEXT_NODES			(MAX_ENTROPY_TOKENS-7)

#define PREC_CASES				3
#define ZERO_RUN_PROB_CASES     14

#define DC_PROBABILITY_UPDATE_THRESH	100

#define ZERO_CONTEXT_NODE		0
#define EOB_CONTEXT_NODE		1
#define ONE_CONTEXT_NODE		2
#define LOW_VAL_CONTEXT_NODE	3
#define TWO_CONTEXT_NODE		4
#define THREE_CONTEXT_NODE		5
#define HIGH_LOW_CONTEXT_NODE	6
#define CAT_ONE_CONTEXT_NODE	7
#define CAT_THREEFOUR_CONTEXT_NODE	8
#define CAT_THREE_CONTEXT_NODE	9
#define CAT_FIVE_CONTEXT_NODE	10

#define PROB_UPDATE_BASELINE_COST	7

#define MAX_PROB				254
#define DCT_MAX_VALUE			2048

#define ZRL_BANDS				2
#define ZRL_BAND2				6

#define SCAN_ORDER_BANDS		16
#define SCAN_BAND_UPDATE_BITS   4

#define HUFF_LUT_LEVELS         6

typedef struct _tokenorptr 
{
    u16 selector:1;         // 1 bit selector 0->ptr, 1->token
    u16 value:7;
} tokenorptr;

typedef struct _dhuffnode 
{
    union 
    {
        i8 l;
        tokenorptr left;
    } leftunion;
    union 
    {
        i8 r;
        tokenorptr right;
    } rightunion;
} HUFF_NODE;
typedef struct _HUFF_TABLE_NODE 
{
    u16 flag:1;             // bit 0: 1-Token, 0-Index
    u16 value:5;             // value: the value of the Token or the Index to the huffman tree
    u16 unused:6;            // not used for now
    u16 length:4;            // Huffman code length of the token
} HUFF_TABLE_NODE;

typedef struct HUFF_INSTANCE
{

    /* Huffman code tables for DC, AC & Zero Run Length */
    /*u32 DcHuffCode[2][MAX_ENTROPY_TOKENS]; */

    /*u8 DcHuffLength[2][MAX_ENTROPY_TOKENS]; */

    u32 DcHuffProbs[2][MAX_ENTROPY_TOKENS];

    HUFF_NODE DcHuffTree[2][MAX_ENTROPY_TOKENS];

    /*u32 AcHuffCode[PREC_CASES][2][VP6HWAC_BANDS][MAX_ENTROPY_TOKENS]; */

    /*u8 AcHuffLength[PREC_CASES][2][VP6HWAC_BANDS][MAX_ENTROPY_TOKENS]; */

    u32 AcHuffProbs[PREC_CASES][2][VP6HWAC_BANDS][MAX_ENTROPY_TOKENS];

    HUFF_NODE AcHuffTree[PREC_CASES][2][VP6HWAC_BANDS][MAX_ENTROPY_TOKENS];

    /*u32 ZeroHuffCode[ZRL_BANDS][ZERO_RUN_PROB_CASES]; */

    /*u8 ZeroHuffLength[ZRL_BANDS][ZERO_RUN_PROB_CASES]; */

    u32 ZeroHuffProbs[ZRL_BANDS][ZERO_RUN_PROB_CASES];

    HUFF_NODE ZeroHuffTree[ZRL_BANDS][ZERO_RUN_PROB_CASES];

#if 0
    /* FAST look-up-table for huffman Trees */
    u16 DcHuffLUT[2][1 << HUFF_LUT_LEVELS];

    u16 AcHuffLUT[PREC_CASES][2][VP6HWAC_BANDS][1 << HUFF_LUT_LEVELS];

    u16 ZeroHuffLUT[ZRL_BANDS][1 << HUFF_LUT_LEVELS];

    /* Counters for runs of zeros at DC & EOB at first AC position in Huffman mode */
    i32 CurrentDcRunLen[2];

    i32 CurrentAc1RunLen[2];
#endif

    u16 DcHuffLUT[2][12];

    u16 AcHuffLUT[2][3][/*6*/4][12];

    u16 ZeroHuffLUT[2][12];

} HUFF_INSTANCE;

void VP6HW_BoolTreeToHuffCodes(const u8 * BoolTreeProbs, u32 * HuffProbs);
void VP6HW_ZerosBoolTreeToHuffCodes(const u8 * BoolTreeProbs, u32 * HuffProbs);

void VP6HW_BuildHuffTree(HUFF_NODE * hn, u32 * counts, i32 values);
void VP6HW_CreateHuffmanLUT(const HUFF_NODE * hn, u16 * HuffTable, i32 values);

#endif /* __VP6HUFFDEC_H__ */
