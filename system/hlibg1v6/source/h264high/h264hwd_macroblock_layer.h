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
--  Abstract : Macroblock level stream decoding and macroblock reconstruction
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_macroblock_layer.h,v $
--  $Date: 2008/03/13 12:48:06 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef H264HWD_MACROBLOCK_LAYER_H
#define H264HWD_MACROBLOCK_LAYER_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"

#include "h264hwd_stream.h"
#include "h264hwd_slice_header.h"

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/* Macro to determine if a mb is an intra mb */
#define IS_INTRA_MB(a) ((a).mbType > 5)

/* Macro to determine if a mb is an I_PCM mb */
#define IS_I_PCM_MB(a) ((a).mbType == 31)

typedef enum mbType
{
    P_Skip = 0,
    P_L0_16x16 = 1,
    P_L0_L0_16x8 = 2,
    P_L0_L0_8x16 = 3,
    P_8x8 = 4,
    P_8x8ref0 = 5,
    I_4x4 = 6,
    I_16x16_0_0_0 = 7,
    I_16x16_1_0_0 = 8,
    I_16x16_2_0_0 = 9,
    I_16x16_3_0_0 = 10,
    I_16x16_0_1_0 = 11,
    I_16x16_1_1_0 = 12,
    I_16x16_2_1_0 = 13,
    I_16x16_3_1_0 = 14,
    I_16x16_0_2_0 = 15,
    I_16x16_1_2_0 = 16,
    I_16x16_2_2_0 = 17,
    I_16x16_3_2_0 = 18,
    I_16x16_0_0_1 = 19,
    I_16x16_1_0_1 = 20,
    I_16x16_2_0_1 = 21,
    I_16x16_3_0_1 = 22,
    I_16x16_0_1_1 = 23,
    I_16x16_1_1_1 = 24,
    I_16x16_2_1_1 = 25,
    I_16x16_3_1_1 = 26,
    I_16x16_0_2_1 = 27,
    I_16x16_1_2_1 = 28,
    I_16x16_2_2_1 = 29,
    I_16x16_3_2_1 = 30,
    I_PCM = 31
} mbType_e;

typedef enum subMbType
{
    P_L0_8x8 = 0,
    P_L0_8x4 = 1,
    P_L0_4x8 = 2,
    P_L0_4x4 = 3
} subMbType_e;

typedef enum mbPartMode
{
    MB_P_16x16 = 0,
    MB_P_16x8,
    MB_P_8x16,
    MB_P_8x8
} mbPartMode_e;

typedef enum subMbPartMode
{
    MB_SP_8x8 = 0,
    MB_SP_8x4,
    MB_SP_4x8,
    MB_SP_4x4
} subMbPartMode_e;

typedef enum mbPartPredMode
{
    PRED_MODE_INTRA4x4 = 0,
    PRED_MODE_INTRA16x16,
    PRED_MODE_INTER
} mbPartPredMode_e;

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

typedef struct mv
{
    i16 hor;
    i16 ver;
} mv_t;

typedef struct mbPred
{
    u32 prevIntra4x4PredModeFlag[16];
    u32 remIntra4x4PredMode[16];
    u32 intraChromaPredMode;
} mbPred_t;

typedef struct subMbPred
{
    subMbType_e subMbType[4];
} subMbPred_t;

typedef struct
{
    u16 rlc[468];
    u8 totalCoeff[28];
} residual_t;

typedef struct macroblockLayer
{
    /*u32 disableDeblockingFilterIdc; */
    i32 filterOffsetA;
    i32 filterOffsetB;
    u32 disableDeblockingFilterIdc;
    mbType_e mbType;
    u32 codedBlockPattern;
    i32 mbQpDelta;
    mbPred_t mbPred;
    subMbPred_t subMbPred;
    residual_t residual;
} macroblockLayer_t;

typedef struct mbStorage
{
    mbType_e mbType;
    mbType_e mbType_asic;
    u32 sliceId;
    /*u32 disableDeblockingFilterIdc; */
    /*i32 filterOffsetA; */
    /*i32 filterOffsetB; */
    u32 qpY;
    /*i32 chromaQpIndexOffset; */
    u8 totalCoeff[24];
    u8 intra4x4PredMode[16];
    u8 intra4x4PredMode_asic[16];
    /* u32 refPic[4]; */
    u8 refIdxL0[4];
    u8 refID[4];
    mv_t mv[16];
    u32 decoded;
    struct mbStorage *mbA;
    struct mbStorage *mbB;
    struct mbStorage *mbC;
    struct mbStorage *mbD;
} mbStorage_t;

struct cabac_s;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

u32 h264bsdDecodeMacroblockLayerCavlc(strmData_t * pStrmData,
                                      macroblockLayer_t * pMbLayer,
                                      mbStorage_t * pMb,
                                      const sliceHeader_t * pSliceHdr );

u32 h264bsdDecodeMacroblockLayerCabac(strmData_t * pStrmData,
                                      macroblockLayer_t * pMbLayer,
                                      mbStorage_t * pMb,
                                      const sliceHeader_t * pSliceHdr,
                                      struct cabac_s * pCabac );

u32 h264bsdNumSubMbPart(subMbType_e subMbType);

subMbPartMode_e h264bsdSubMbPartMode(subMbType_e subMbType);

u32 h264bsdPredModeIntra16x16(mbType_e mbType);

mbPartPredMode_e h264bsdMbPartPredMode(mbType_e mbType);

#endif /* #ifdef H264HWD_MACROBLOCK_LAYER_H */
