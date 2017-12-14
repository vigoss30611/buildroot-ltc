/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------*/


//  Abstract : Encoder setup according to a test vector

// header
#include "sw_test_id.h"

#include <stdio.h>
#include <stdlib.h>

// static function
static void HevcFrameQuantizationTest(struct hevc_instance *inst);
static void HevcSliceTest(struct hevc_instance *inst);
static void HevcStreamBufferLimitTest(struct hevc_instance *inst);
static void HevcMbQuantizationTest(struct hevc_instance *inst);
static void HevcFilterTest(struct hevc_instance *inst);
static void HevcUserDataTest(struct hevc_instance *inst);
static void HevcIntra32FavorTest(struct hevc_instance *inst);
static void HevcIntra16FavorTest(struct hevc_instance *inst);
static void HevcInterFavorTest(struct hevc_instance *inst);
static void HevcCroppingTest(struct hevc_instance *inst);
static void HevcRgbInputMaskTest(struct hevc_instance *inst);
static void HevcMadTest(struct hevc_instance *inst);
static void HevcMvTest(struct hevc_instance *inst);
static void HevcDMVPenaltyTest(struct hevc_instance *inst);
static void HevcMaxOverfillMv(struct hevc_instance *inst);
static void HevcRoiTest(struct hevc_instance *inst);
static void HevcIntraAreaTest(struct hevc_instance *inst);
static void HevcCirTest(struct hevc_instance *inst);
static void HevcIntraSliceMapTest(struct hevc_instance *inst);
static void HevcSegmentTest(struct hevc_instance *inst);
static void HevcRefFrameTest(struct hevc_instance *inst);
static void HevcTemporalLayerTest(struct hevc_instance *inst);
static void HevcSegmentMapTest(struct hevc_instance *inst);
static void HevcPenaltyTest(struct hevc_instance *inst);
static void HevcDownscalingTest(struct hevc_instance *inst);


/*------------------------------------------------------------------------------

    TestID defines a test configuration for the encoder. If the encoder control
    software is compiled with INTERNAL_TEST flag the test ID will force the
    encoder operation according to the test vector.

    TestID  Description
    0       No action, normal encoder operation
    1       Frame quantization test, adjust qp for every frame, qp = 0..51
    2       Slice test, adjust slice amount for each frame
    4       Stream buffer limit test, limit=500 (4kB) for first frame
    6       Quantization test, min and max QP values.
    7       Filter test, set disableDeblocking and filterOffsets A and B
    8       Segment test, set segment map and segment qps
    9       Reference frame test, all combinations of reference and refresh.
    10      Segment map test
    11      Temporal layer test, reference and refresh as with 3 layers
    12      User data test
    15      Intra16Favor test, set to maximum value
    16      Cropping test, set cropping values for every frame
    19      RGB input mask test, set all values
    20      MAD test, test all MAD QP change values
    21      InterFavor test, set to maximum value
    22      MV test, set cropping offsets so that max MVs are tested
    23      DMV penalty test, set to minimum/maximum values
    24      Max overfill MV
    26      ROI test
    27      Intra area test
    28      CIR test
    29      Intra slice map test
    31      Non-zero penalty test, don't use zero penalties
    34      Downscaling test

------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------

    HevcConfigureTestBeforeFrame

    Function configures the encoder instance before starting frame encoding

------------------------------------------------------------------------------*/
void HevcConfigureTestBeforeFrame(struct hevc_instance *inst)
{
  ASSERT(inst);

  switch (inst->testId)
  {
    case TID_QP:  HevcFrameQuantizationTest(inst);        break;
    case TID_SLC: HevcSliceTest(inst);                    break;
      //NotSupport case 6: HevcMbQuantizationTest(inst);           break;
    case TID_DBF: HevcFilterTest(inst);                   break;
    case 8: HevcSegmentTest(inst);                  break;
    case 9: HevcRefFrameTest(inst);                 break;
      //NotSupport case 10: HevcSegmentMapTest(inst);              break;
    case 11: HevcTemporalLayerTest(inst);           break;
    case 12: HevcUserDataTest(inst);                break;
    case 16: HevcCroppingTest(inst);                break;
    case 19: HevcRgbInputMaskTest(inst);            break;
    case 20: HevcMadTest(inst);                     break;
    case 22: HevcMvTest(inst);                      break;
    case 24: HevcMaxOverfillMv(inst);               break;
    case TID_IA:  HevcIntraAreaTest(inst);               break;
    case TID_CIR: HevcCirTest(inst);                     break;
      //NotSupport case 29: HevcIntraSliceMapTest(inst);           break;
    case 34: HevcDownscalingTest(inst);             break;
    default: break;
  }
}

/*------------------------------------------------------------------------------

    HevcConfigureTestBeforeStart

    Function configures the encoder instance before starting frame encoding

------------------------------------------------------------------------------*/
void HevcConfigureTestBeforeStart(struct hevc_instance *inst)
{
  ASSERT(inst);

  switch (inst->testId)
  {
    case TID_ROI: HevcRoiTest(inst);                     break;
    case TID_F32: HevcIntra32FavorTest(inst);            break;
    case TID_F16: HevcIntra16FavorTest(inst);            break;
    case TID_INT: HevcStreamBufferLimitTest(inst);        break;
    case 21: HevcInterFavorTest(inst);              break;
    case 23: HevcDMVPenaltyTest(inst);              break;
    case 31: HevcPenaltyTest(inst);                 break;
    default: break;
  }
}

/*------------------------------------------------------------------------------
  HevcQuantizationTest
------------------------------------------------------------------------------*/
void HevcFrameQuantizationTest(struct hevc_instance *inst)
{
  i32 vopNum = inst->frameCnt;

  /* Inter frame qp start zero */
  inst->rateControl.qpHdr = MIN(inst->rateControl.qpMax,
                                MAX(inst->rateControl.qpMin, (((vopNum - 1) % 52) << QP_FRACTIONAL_BITS)));
  inst->rateControl.fixedQp = inst->rateControl.qpHdr;

  printf("HevcFrameQuantTest# qpHdr %d\n", inst->rateControl.qpHdr >> QP_FRACTIONAL_BITS);
}

/*------------------------------------------------------------------------------
  HevcSliceTest
------------------------------------------------------------------------------*/
void HevcSliceTest(struct hevc_instance *inst)
{
  i32 vopNum = inst->frameCnt;

  int sliceSize = (vopNum) % inst->ctbPerCol;

  inst->asic.regs.sliceSize = sliceSize;
  inst->asic.regs.sliceNum = (sliceSize == 0) ?  1 : ((inst->ctbPerCol + (sliceSize - 1)) / sliceSize);

  printf("HevcSliceTest# sliceSize %d\n", sliceSize);
}

/*------------------------------------------------------------------------------
  HevcStreamBufferLimitTest
------------------------------------------------------------------------------*/
void HevcStreamBufferLimitTest(struct hevc_instance *inst)
{
  static u32 firstFrame = 1;

  if (!firstFrame)
    return;

  firstFrame = 0;
  inst->asic.regs.outputStrmSize = 4000;

  printf("HevcStreamBufferLimitTest# streamBufferLimit %d bytes\n",
         inst->asic.regs.outputStrmSize);
}

/*------------------------------------------------------------------------------
  HevcQuantizationTest
  NOTE: ASIC resolution for wordCntTarget and wordError is value/4
------------------------------------------------------------------------------*/
void HevcMbQuantizationTest(struct hevc_instance *inst)
{
  i32 vopNum = inst->frameCnt;
  hevcRateControl_s *rc = &inst->rateControl;

  rc->qpMin = (MIN(51, vopNum / 4)) << QP_FRACTIONAL_BITS;
  rc->qpMax = (MAX(0, 51 - vopNum / 4))<< QP_FRACTIONAL_BITS;
  rc->qpMax = (MAX(rc->qpMax, rc->qpMin))<< QP_FRACTIONAL_BITS;

  rc->qpLastCoded = rc->qpTarget = rc->qpHdr =
                                     MIN(rc->qpMax, MAX(rc->qpMin, 26 << QP_FRACTIONAL_BITS));

  printf("HevcMbQuantTest# min %d  max %d  QP %d\n",
         rc->qpMin >> QP_FRACTIONAL_BITS, rc->qpMax >> QP_FRACTIONAL_BITS, rc->qpHdr >> QP_FRACTIONAL_BITS);
}

/*------------------------------------------------------------------------------
  HevcFilterTest
------------------------------------------------------------------------------*/
void HevcFilterTest(struct hevc_instance *inst)
{
  const i32 intra_period = 2;    // need pattern has intraPicRate=2.
  const i32 disable_period = 2;  // <=3 if corss slice deblock flag
  i32 vopNum = inst->frameCnt % (intra_period * disable_period * 13 * 2);
  regValues_s *regs = &inst->asic.regs;
  i32 enableDeblockOverride = inst->enableDeblockOverride;

  if (!enableDeblockOverride)
  {
    printf("HevcFilterTest# invalid deblock_filter_override_enable_flag.\n");
    return;
  }
  regs->slice_deblocking_filter_override_flag = 1;

  //inst->disableDeblocking = (vopNum/2)%3;
  //FIXME: not support disable across slice deblock now.

  inst->disableDeblocking = (vopNum / intra_period) % disable_period;

  if (vopNum == 0)
  {
    inst->tc_Offset = -6;
    inst->beta_Offset = 6;
  }
  else if (vopNum < (intra_period * disable_period * 13))
  {
    if (vopNum % (intra_period * disable_period) == 0)
    {
      inst->tc_Offset += 1;
      inst->beta_Offset -= 1;
    }
  }
  else if (vopNum == (intra_period * disable_period * 13))
  {
    inst->tc_Offset = -6;
    inst->beta_Offset = -6;
  }
  else if (vopNum < (intra_period * disable_period * 13 * 2))
  {
    if (vopNum % (intra_period * disable_period) == 0)
    {
      inst->tc_Offset += 1;
      inst->beta_Offset += 1;
    }
  }

  printf("HevcFilterTest# disableDeblock = %d, filterOffA = %i filterOffB = %i\n",
         inst->disableDeblocking, inst->tc_Offset,
         inst->beta_Offset);
}

/*------------------------------------------------------------------------------
  HevcSegmentTest
------------------------------------------------------------------------------*/
void HevcSegmentTest(struct hevc_instance *inst)
{
#if 0
  u32 frame = (u32)inst->frameCnt;
  u32 *map = inst->asic.segmentMap.virtualAddress;
  u32 mbPerFrame = (inst->mbPerFrame + 7) / 8 * 8; /* Rounded upwards to 8 */
  u32 i, j;
  u32 mask;
  i32 qpSgm[4];

  inst->asic.regs.segmentEnable = 1;
  inst->asic.regs.segmentMapUpdate = 1;
  if (frame < 2)
  {
    for (i = 0; i < mbPerFrame / 8 / 4; i++)
    {
      map[i * 4 + 0] = 0x00000000;
      map[i * 4 + 1] = 0x11111111;
      map[i * 4 + 2] = 0x22222222;
      map[i * 4 + 3] = 0x33333333;
    }
  }
  else
  {
    for (i = 0; i < mbPerFrame / 8; i++)
    {
      mask = 0;
      for (j = 0; j < 8; j++)
        mask |= ((j + (frame - 2) / 2 + j * frame / 3) % 4) << (28 - j * 4);
      map[i] = mask;
    }
  }

  for (i = 0; i < 4; i++)
  {
    qpSgm[i] = (32 + i + frame / 2 + frame * i) % 52;
    qpSgm[i] = MAX(inst->rateControl.qpMin,
                   MIN(inst->rateControl.qpMax, qpSgm[i]));
  }
  inst->rateControl.qpHdr = qpSgm[0];
  inst->rateControl.mbQpAdjustment[0] = MAX(-8, MIN(7, qpSgm[1] - qpSgm[0]));
  inst->rateControl.mbQpAdjustment[1] = qpSgm[2] - qpSgm[0];
  inst->rateControl.mbQpAdjustment[2] = qpSgm[3] - qpSgm[0];

  printf("HevcSegmentTest# enable=%d update=%d map=0x%08x%08x%08x%08x\n",
         inst->asic.regs.segmentEnable, inst->asic.regs.segmentMapUpdate,
         map[0], map[1], map[2], map[3]);
  printf("HevcSegmentTest# qp=%d,%d,%d,%d\n",
         qpSgm[0], qpSgm[1], qpSgm[2], qpSgm[3]);
#endif
}

/*------------------------------------------------------------------------------
  HevcRefFrameTest
------------------------------------------------------------------------------*/
void HevcRefFrameTest(struct hevc_instance *inst)
{
#if 0
  i32 pic = inst->frameCnt;
  picBuffer *picBuffer = &inst->picBuffer;

  /* Only adjust for p-frames */
  if (picBuffer->cur_pic->p_frame)
  {
    picBuffer->cur_pic->ipf = pic & 0x1 ? 1 : 0;
    if (inst->seqParameterSet.numRefFrames > 1)
      picBuffer->cur_pic->grf = pic & 0x2 ? 1 : 0;
    picBuffer->refPicList[0].search = pic & 0x8 ? 1 : 0;
    if (inst->seqParameterSet.numRefFrames > 1)
      picBuffer->refPicList[1].search = pic & 0x10 ? 1 : 0;
  }

  printf("HevcRefFrameTest#\n");
#endif
}

/*------------------------------------------------------------------------------
  HevcTemporalLayerTest
------------------------------------------------------------------------------*/
void HevcTemporalLayerTest(struct hevc_instance *inst)
{
#if 0
  i32 pic = inst->frameCnt;
  picBuffer *picBuffer = &inst->picBuffer;

  /* Four temporal layers, base layer (LTR) every 8th frame. */
  if (inst->seqParameterSet.numRefFrames > 1)
    picBuffer->cur_pic->grf = pic & 0x7 ? 0 : 1;
  if (picBuffer->cur_pic->p_frame)
  {
    /* Odd frames don't update ipf */
    picBuffer->cur_pic->ipf = pic & 0x1 ? 0 : 1;
    /* Frames 1,2,3 & 4,5,6 reference prev */
    picBuffer->refPicList[0].search = pic & 0x3 ? 1 : 0;
    /* Every fourth frame (layers 0&1) reference LTR */
    if (inst->seqParameterSet.numRefFrames > 1)
      picBuffer->refPicList[1].search = pic & 0x3 ? 0 : 1;
  }

  printf("HevcTemporalLayer#\n");
#endif
}

/*------------------------------------------------------------------------------
  HevcSegmentMapTest
------------------------------------------------------------------------------*/
void HevcSegmentMapTest(struct hevc_instance *inst)
{
#if 0
  u32 frame = (u32)inst->frameCnt;
  u32 *map = inst->asic.segmentMap.virtualAddress;
  u32 mbPerFrame = (inst->mbPerFrame + 7) / 8 * 8; /* Rounded upwards to 8 */
  u32 i, j;
  u32 mask;
  i32 qpSgm[4];

  inst->asic.regs.segmentEnable = 1;
  inst->asic.regs.segmentMapUpdate = 1;
  if (frame < 2)
  {
    for (i = 0; i < mbPerFrame / 8; i++)
      map[i] = 0x01020120;
  }
  else
  {
    for (i = 0; i < mbPerFrame / 8; i++)
    {
      mask = 0;
      for (j = 0; j < 8; j++)
        mask |= ((j + frame) % 4) << (28 - j * 4);
      map[i] = mask;
    }
  }

  if (frame < 2)
  {
    qpSgm[0] = inst->rateControl.qpMax;
    qpSgm[1] = qpSgm[2] = qpSgm[3] = inst->rateControl.qpMin;
  }
  else
  {
    for (i = 0; i < 4; i++)
    {
      qpSgm[i] = (32 + i * frame) % 52;
      qpSgm[i] = MAX(inst->rateControl.qpMin,
                     MIN(inst->rateControl.qpMax, qpSgm[i]));
    }
  }
  inst->rateControl.qpHdr = qpSgm[0];
  inst->rateControl.mbQpAdjustment[0] = MAX(-8, MIN(7, qpSgm[1] - qpSgm[0]));
  inst->rateControl.mbQpAdjustment[1] = qpSgm[2] - qpSgm[0];
  inst->rateControl.mbQpAdjustment[2] = qpSgm[3] - qpSgm[0];

  printf("HevcSegmentMapTest# enable=%d update=%d map=0x%08x%08x%08x%08x\n",
         inst->asic.regs.segmentEnable, inst->asic.regs.segmentMapUpdate,
         map[0], map[1], map[2], map[3]);
  printf("HevcSegmentMapTest# qp=%d,%d,%d,%d\n",
         qpSgm[0], qpSgm[1], qpSgm[2], qpSgm[3]);
#endif
}


/*------------------------------------------------------------------------------
  HevcUserDataTest
------------------------------------------------------------------------------*/
void HevcUserDataTest(struct hevc_instance *inst)
{
#if 0
  static u8 *userDataBuf = NULL;
  i32 userDataLength = 16 + ((inst->frameCnt * 11) % 2000);
  i32 i;

  /* Allocate a buffer for user data, encoder reads data from this buffer
   * and writes it to the stream. TODO: This is never freed. */
  if (!userDataBuf)
    userDataBuf = (u8 *)malloc(2048);

  if (!userDataBuf)
    return;

  for (i = 0; i < userDataLength; i++)
  {
    /* Fill user data buffer with ASCII symbols from 48 to 125 */
    userDataBuf[i] = 48 + i % 78;
  }

  /* Enable user data insertion */
  inst->rateControl.sei.userDataEnabled = ENCHW_YES;
  inst->rateControl.sei.pUserData = userDataBuf;
  inst->rateControl.sei.userDataSize = userDataLength;

  printf("HevcUserDataTest# userDataSize %d\n", userDataLength);
#endif
}

/*------------------------------------------------------------------------------
  HevcIntra16FavorTest
------------------------------------------------------------------------------*/
void HevcIntra16FavorTest(struct hevc_instance *inst)
{
  asicData_s *asic = &inst->asic;

  asic->regs.intraPenaltyPic4x4  = 0x3ff;
  asic->regs.intraPenaltyPic8x8  = 0x1fff;
  //asic->regs.intraPenaltyPic16x16 = HEVCIntraPenaltyTu16[asic->regs.qp];
  //asic->regs.intraPenaltyPic32x32 = HEVCIntraPenaltyTu32[asic->regs.qp];

  asic->regs.intraPenaltyRoi14x4 = 0x3ff;
  asic->regs.intraPenaltyRoi18x8 = 0x1fff;
  //asic->regs.intraPenaltyRoi116x16 = HEVCIntraPenaltyTu16[asic->regs.qp-asic->regs.roi1DeltaQp];
  //asic->regs.intraPenaltyRoi132x32 = HEVCIntraPenaltyTu32[asic->regs.qp-asic->regs.roi1DeltaQp];

  asic->regs.intraPenaltyRoi24x4 = 0x3ff;
  asic->regs.intraPenaltyRoi28x8 = 0x1fff;
  //asic->regs.intraPenaltyRoi216x16 = HEVCIntraPenaltyTu16[asic->regs.qp-asic->regs.roi2DeltaQp];
  //asic->regs.intraPenaltyRoi232x32 = HEVCIntraPenaltyTu32[asic->regs.qp-asic->regs.roi2DeltaQp];

  printf("HevcIntra16FavorTest# intra16Favor. \n");
}

/*------------------------------------------------------------------------------
  HevcIntra32FavorTest
------------------------------------------------------------------------------*/
void HevcIntra32FavorTest(struct hevc_instance *inst)
{
  asicData_s *asic = &inst->asic;

  asic->regs.intraPenaltyPic4x4  = 0x3ff;
  asic->regs.intraPenaltyPic8x8  = 0x1fff;
  asic->regs.intraPenaltyPic16x16 = 0x3fff;
  //asic->regs.intraPenaltyPic32x32 = HEVCIntraPenaltyTu32[asic->regs.qp];

  asic->regs.intraPenaltyRoi14x4 = 0x3ff;
  asic->regs.intraPenaltyRoi18x8 = 0x1fff;
  asic->regs.intraPenaltyRoi116x16 = 0x3fff;
  //asic->regs.intraPenaltyRoi132x32 = HEVCIntraPenaltyTu32[asic->regs.qp-asic->regs.roi1DeltaQp];

  asic->regs.intraPenaltyRoi24x4 = 0x3ff;
  asic->regs.intraPenaltyRoi28x8 = 0x1fff;
  asic->regs.intraPenaltyRoi216x16 = 0x3fff;
  //asic->regs.intraPenaltyRoi232x32 = HEVCIntraPenaltyTu32[asic->regs.qp-asic->regs.roi2DeltaQp];

  printf("HevcIntra32FavorTest# intra32Favor. \n");
}

/*------------------------------------------------------------------------------
  HevcInterFavorTest
------------------------------------------------------------------------------*/
void HevcInterFavorTest(struct hevc_instance *inst)
{
#if 0
  i32 s;

  /* Force combinations of inter favor and skip penalty values */

  for (s = 0; s < 4; s++)
  {
    if ((inst->frameCnt % 3) == 0)
    {
      inst->asic.regs.pen[s][ASIC_PENALTY_INTER_FAVOR] = 0x7FFF;
    }
    else if ((inst->frameCnt % 3) == 1)
    {
      inst->asic.regs.pen[s][ASIC_PENALTY_SKIP] = 0;
      inst->asic.regs.pen[s][ASIC_PENALTY_INTER_FAVOR] = 0x7FFF;
    }
    else
    {
      inst->asic.regs.pen[s][ASIC_PENALTY_SKIP] = 0;
    }
  }

  printf("HevcInterFavorTest# interFavor %d skipPenalty %d\n",
         inst->asic.regs.pen[0][ASIC_PENALTY_INTER_FAVOR],
         inst->asic.regs.pen[0][ASIC_PENALTY_SKIP]);
#endif
}

/*------------------------------------------------------------------------------
  HevcCroppingTest
------------------------------------------------------------------------------*/
void HevcCroppingTest(struct hevc_instance *inst)
{
  inst->preProcess.horOffsetSrc = (inst->frameCnt % 8) * 2;
  if (EncPreProcessCheck(&inst->preProcess) == ENCHW_NOK)
    inst->preProcess.horOffsetSrc = 0;
  inst->preProcess.verOffsetSrc = (inst->frameCnt / 4) * 2;
  if (EncPreProcessCheck(&inst->preProcess) == ENCHW_NOK)
    inst->preProcess.verOffsetSrc = 0;

  printf("HevcCroppingTest# horOffsetSrc %d  verOffsetSrc %d\n",
         inst->preProcess.horOffsetSrc, inst->preProcess.verOffsetSrc);
}

/*------------------------------------------------------------------------------
  HevcRgbInputMaskTest
------------------------------------------------------------------------------*/
void HevcRgbInputMaskTest(struct hevc_instance *inst)
{
  u32 frameNum = (u32)inst->frameCnt;
  static u32 rMsb = 0;
  static u32 gMsb = 0;
  static u32 bMsb = 0;
  static u32 lsMask = 0;  /* Lowest possible mask position */
  static u32 msMask = 0;  /* Highest possible mask position */

  /* First frame normal
   * 1..29 step rMaskMsb values
   * 30..58 step gMaskMsb values
   * 59..87 step bMaskMsb values */
  if (frameNum == 0)
  {
    rMsb = inst->asic.regs.rMaskMsb;
    gMsb = inst->asic.regs.gMaskMsb;
    bMsb = inst->asic.regs.bMaskMsb;
    lsMask = MIN(rMsb, gMsb);
    lsMask = MIN(bMsb, lsMask);
    msMask = MAX(rMsb, gMsb);
    msMask = MAX(bMsb, msMask);
    if (msMask < 16)
      msMask = 15 - 2;  /* 16bit RGB, 13 mask positions: 3..15  */
    else
      msMask = 31 - 2;  /* 32bit RGB, 29 mask positions: 3..31 */
  }
  else if (frameNum <= msMask)
  {
    inst->asic.regs.rMaskMsb = MAX(frameNum + 2, lsMask);
    inst->asic.regs.gMaskMsb = gMsb;
    inst->asic.regs.bMaskMsb = bMsb;
  }
  else if (frameNum <= msMask * 2)
  {
    inst->asic.regs.rMaskMsb = rMsb;
    inst->asic.regs.gMaskMsb = MAX(frameNum - msMask + 2, lsMask);
    if (inst->asic.regs.inputImageFormat == 4)  /* RGB 565 special case */
      inst->asic.regs.gMaskMsb = MAX(frameNum - msMask + 2, lsMask + 1);
    inst->asic.regs.bMaskMsb = bMsb;
  }
  else if (frameNum <= msMask * 3)
  {
    inst->asic.regs.rMaskMsb = rMsb;
    inst->asic.regs.gMaskMsb = gMsb;
    inst->asic.regs.bMaskMsb = MAX(frameNum - msMask * 2 + 2, lsMask);
  }
  else
  {
    inst->asic.regs.rMaskMsb = rMsb;
    inst->asic.regs.gMaskMsb = gMsb;
    inst->asic.regs.bMaskMsb = bMsb;
  }

  printf("HevcRgbInputMaskTest#  %d %d %d\n", inst->asic.regs.rMaskMsb,
         inst->asic.regs.gMaskMsb, inst->asic.regs.bMaskMsb);
}

/*------------------------------------------------------------------------------
  HevcMadTest
------------------------------------------------------------------------------*/
void HevcMadTest(struct hevc_instance *inst)
{
#if 0
  u32 frameNum = (u32)inst->frameCnt;

  /* All values in range [-8,7] */
  inst->rateControl.mbQpAdjustment[0] = -8 + (frameNum % 16);
  inst->rateControl.mbQpAdjustment[1] = -127 + (frameNum % 254);
  inst->rateControl.mbQpAdjustment[2] = 127 - (frameNum % 254);
  /* Step 256, range [0,63*256] */
  inst->mad.threshold[0] = 256 * ((frameNum + 1) % 64);
  inst->mad.threshold[1] = 256 * ((frameNum / 2) % 64);
  inst->mad.threshold[2] = 256 * ((frameNum / 3) % 64);

  printf("HevcMadTest#  Thresholds: %d,%d,%d  QpDeltas: %d,%d,%d\n",
         inst->asic.regs.madThreshold[0],
         inst->asic.regs.madThreshold[1],
         inst->asic.regs.madThreshold[2],
         inst->rateControl.mbQpAdjustment[0],
         inst->rateControl.mbQpAdjustment[1],
         inst->rateControl.mbQpAdjustment[2]);
#endif
}

/*------------------------------------------------------------------------------
  HevcMvTest
------------------------------------------------------------------------------*/
void HevcMvTest(struct hevc_instance *inst)
{
  u32 frame = (u32)inst->frameCnt;

  /* Set cropping offsets according to max MV length, decrement by frame
   * x = 32, 160, 32, 159, 32, 158, ..
   * y = 48, 80, 48, 79, 48, 78, .. */
  inst->preProcess.horOffsetSrc = 32 + (frame % 2) * 128 - (frame % 2) * (frame / 2);
  if (EncPreProcessCheck(&inst->preProcess) == ENCHW_NOK)
    inst->preProcess.horOffsetSrc = 0;
  inst->preProcess.verOffsetSrc = 48 + (frame % 2) * 32 - (frame % 2) * (frame / 2);
  if (EncPreProcessCheck(&inst->preProcess) == ENCHW_NOK)
    inst->preProcess.verOffsetSrc = 0;

  printf("HevcMvTest# horOffsetSrc %d  verOffsetSrc %d\n",
         inst->preProcess.horOffsetSrc, inst->preProcess.verOffsetSrc);
}

/*------------------------------------------------------------------------------
  HevcDMVPenaltyTest
------------------------------------------------------------------------------*/
void HevcDMVPenaltyTest(struct hevc_instance *inst)
{
#if 0
  u32 frame = (u32)inst->frameCnt;
  i32 s;

  /* Set DMV penalty values to maximum and minimum */
  for (s = 0; s < 4; s++)
  {
    inst->asic.regs.pen[s][ASIC_PENALTY_DMV_4P] = frame % 2 ? 127 - frame / 2 : frame / 2;
    inst->asic.regs.pen[s][ASIC_PENALTY_DMV_1P] = frame % 2 ? 127 - frame / 2 : frame / 2;
    inst->asic.regs.pen[s][ASIC_PENALTY_DMV_QP] = frame % 2 ? 127 - frame / 2 : frame / 2;
  }

  printf("HevcDMVPenaltyTest# penalty4p %d  penalty1p %d  penaltyQp %d\n",
         inst->asic.regs.pen[0][ASIC_PENALTY_DMV_4P],
         inst->asic.regs.pen[0][ASIC_PENALTY_DMV_1P],
         inst->asic.regs.pen[0][ASIC_PENALTY_DMV_QP]);
#endif
}

/*------------------------------------------------------------------------------
  HevcMaxOverfillMv
------------------------------------------------------------------------------*/
void HevcMaxOverfillMv(struct hevc_instance *inst)
{
  u32 frame = (u32)inst->frameCnt;

  /* Set cropping offsets according to max MV length.
   * In test cases the picture is selected so that this will
   * cause maximum horizontal MV to point into overfilled area. */
  inst->preProcess.horOffsetSrc = 32 + (frame % 2) * 128;
  if (EncPreProcessCheck(&inst->preProcess) == ENCHW_NOK)
    inst->preProcess.horOffsetSrc = 0;

  inst->preProcess.verOffsetSrc = 176;
  if (EncPreProcessCheck(&inst->preProcess) == ENCHW_NOK)
    inst->preProcess.verOffsetSrc = 0;

  printf("HevcMaxOverfillMv# horOffsetSrc %d  verOffsetSrc %d\n",
         inst->preProcess.horOffsetSrc, inst->preProcess.verOffsetSrc);
}

/*------------------------------------------------------------------------------
  HevcRoiTest
------------------------------------------------------------------------------*/
void HevcRoiTest(struct hevc_instance *inst)
{
  regValues_s *regs = &inst->asic.regs;
  u32 frame = (u32)inst->frameCnt;
  u32 ctbPerRow = inst->ctbPerRow;
  u32 ctbPerCol = inst->ctbPerCol;
  u32 frames = MIN(ctbPerRow, ctbPerCol);
  u32 loop = frames * 3;

  /* Loop after this many encoded frames */
  frame = frame % loop;

  regs->roi1DeltaQp = (frame % 15) + 1;
  regs->roi2DeltaQp = 15 - (frame % 15);
  regs->roiUpdate = 1;

  /* Set two ROI areas according to frame dimensions. */
  if (frame < frames)
  {
    /* ROI1 in top-left corner, ROI2 in bottom-right corner */
    regs->roi1Left = regs->roi1Top = 0;
    regs->roi1Right = regs->roi1Bottom = frame;
    regs->roi2Left = ctbPerRow - 1 - frame;
    regs->roi2Top = ctbPerCol - 1 - frame;
    regs->roi2Right = ctbPerRow - 1;
    regs->roi2Bottom = ctbPerCol - 1;
  }
  else if (frame < frames * 2)
  {
    /* ROI1 gets smaller towards top-right corner,
     * ROI2 towards bottom-left corner */
    frame -= frames;
    regs->roi1Left = frame;
    regs->roi1Top = 0;
    regs->roi1Right = ctbPerRow - 1;
    regs->roi1Bottom = ctbPerCol - 1 - frame;
    regs->roi2Left = 0;
    regs->roi2Top = frame;
    regs->roi2Right = ctbPerRow - 1 - frame;
    regs->roi2Bottom = ctbPerCol - 1;
  }
  else if (frame < frames * 3)
  {
    /* 1x1/2x2 ROIs moving diagonal across frame */
    frame -= frames * 2;
    regs->roi1Left = frame - frame % 2;
    regs->roi1Right = frame;
    regs->roi1Top = frame - frame % 2;
    regs->roi1Bottom = frame;
    regs->roi2Left = frame - frame % 2;
    regs->roi2Right = frame;
    regs->roi2Top = ctbPerCol - 1 - frame;
    regs->roi2Bottom = ctbPerCol - 1 - frame + frame % 2;
  }

  printf("HevcRoiTest# ROI1:%d x%dy%d-x%dy%d  ROI2:%d x%dy%d-x%dy%d\n",
         regs->roi1DeltaQp, regs->roi1Left, regs->roi1Top,
         regs->roi1Right, regs->roi1Bottom,
         regs->roi2DeltaQp, regs->roi2Left, regs->roi2Top,
         regs->roi2Right, regs->roi2Bottom);
}

/*------------------------------------------------------------------------------
  HevcIntraAreaTest
------------------------------------------------------------------------------*/
void HevcIntraAreaTest(struct hevc_instance *inst)
{
  regValues_s *regs = &inst->asic.regs;
  u32 frame = (u32)inst->frameCnt;
  u32 ctbPerRow = inst->ctbPerRow;
  u32 ctbPerCol = inst->ctbPerCol;
  u32 frames = MIN(ctbPerRow, ctbPerCol);
  u32 loop = frames * 3;

  /* Loop after this many encoded frames */
  frame = frame % loop;

  if (frame < frames)
  {
    /* Intra area in top-left corner, gets bigger every frame */
    regs->intraAreaLeft = regs->intraAreaTop = 0;
    regs->intraAreaRight = regs->intraAreaBottom = frame;
  }
  else if (frame < frames * 2)
  {
    /* Intra area gets smaller towards top-right corner */
    frame -= frames;
    regs->intraAreaLeft = frame;
    regs->intraAreaTop = 0;
    regs->intraAreaRight = ctbPerRow - 1;
    regs->intraAreaBottom = ctbPerCol - 1 - frame;
  }
  else if (frame < frames * 3)
  {
    /* 1x1/2x2 Intra area moving diagonal across frame */
    frame -= frames * 2;
    regs->intraAreaLeft = frame - frame % 2;
    regs->intraAreaRight = frame;
    regs->intraAreaTop = frame - frame % 2;
    regs->intraAreaBottom = frame;
  }

  printf("HevcIntraAreaTest# x%dy%d-x%dy%d\n",
         regs->intraAreaLeft, regs->intraAreaTop,
         regs->intraAreaRight, regs->intraAreaBottom);
}

/*------------------------------------------------------------------------------
  HevcCirTest
------------------------------------------------------------------------------*/
void HevcCirTest(struct hevc_instance *inst)
{
  regValues_s *regs = &inst->asic.regs;
  u32 frame = (u32)inst->frameCnt;
  u32 ctbPerRow = inst->ctbPerRow;
  u32 ctbPerFrame = inst->ctbPerFrame;
  u32 loop = inst->ctbPerFrame + 6;

  /* Loop after this many encoded frames */
  frame = frame % loop;

  switch (frame)
  {
    case 0:
    case 1:
      regs->cirStart = 0;
      regs->cirInterval = 1;
      break;
    case 2:
      regs->cirStart = 0;
      regs->cirInterval = 2;
      break;
    case 3:
      regs->cirStart = 0;
      regs->cirInterval = 3;
      break;
    case 4:
      regs->cirStart = 0;
      regs->cirInterval = ctbPerRow;
      break;
    case 5:
      regs->cirStart = 0;
      regs->cirInterval = ctbPerRow + 1;
      break;
    case 6:
      regs->cirStart = 0;
      regs->cirInterval = ctbPerFrame - 1;
      break;
    case 7:
      regs->cirStart = ctbPerFrame - 1;
      regs->cirInterval = 1;
      break;
    default:
      regs->cirStart = frame - 7;
      regs->cirInterval = (ctbPerFrame - frame) % (ctbPerRow * 2);
      break;
  }

  printf("HevcCirTest# start:%d interval:%d\n",
         regs->cirStart, regs->cirInterval);
}

/*------------------------------------------------------------------------------
  HevcIntraSliceMapTest
------------------------------------------------------------------------------*/
void HevcIntraSliceMapTest(struct hevc_instance *inst)
{
#if 0
  u32 frame = (u32)inst->frameCnt;
  u32 mbPerCol = inst->mbPerCol;

  frame = frame % (mbPerCol * 10);

  if (frame <= 1)
  {
    inst->slice.sliceSize = inst->mbPerRow;
    inst->intraSliceMap[0] = 0x55555555;
    inst->intraSliceMap[1] = 0x55555555;
    inst->intraSliceMap[2] = 0x55555555;
  }
  else if (frame < mbPerCol + 1)
  {
    inst->slice.sliceSize = inst->mbPerRow * (frame - 1);
    inst->intraSliceMap[0] = 0xAAAAAAAA;
    inst->intraSliceMap[1] = 0xAAAAAAAA;
    inst->intraSliceMap[2] = 0xAAAAAAAA;
  }
  else
  {
    inst->slice.sliceSize = inst->mbPerRow * (frame % mbPerCol);
    inst->intraSliceMap[0] += frame;
    inst->intraSliceMap[1] += frame;
    inst->intraSliceMap[2] += frame;
  }

  printf("HevcIntraSliceMapTest# "
         "sliceSize: %d  map1: 0x%x  map2: 0x%x  map3: 0x%x \n",
         inst->slice.sliceSize, inst->intraSliceMap[0],
         inst->intraSliceMap[1], inst->intraSliceMap[2]);
#endif
}

/*------------------------------------------------------------------------------
  HevcPenaltyTest
------------------------------------------------------------------------------*/
void HevcPenaltyTest(struct hevc_instance *inst)
{
#if 0
  i32 s;

  inst->asic.regs.inputReadChunk = 1;

  /* Set non-zero values */
  for (s = 0; s < 4; s++)
  {
    inst->asic.regs.pen[s][ASIC_PENALTY_SPLIT16x8] = 1;
    inst->asic.regs.pen[s][ASIC_PENALTY_SPLIT8x8] = 2;
    inst->asic.regs.pen[s][ASIC_PENALTY_SPLIT8x4] = 3;
    inst->asic.regs.pen[s][ASIC_PENALTY_SPLIT4x4] = 4;
    inst->asic.regs.pen[s][ASIC_PENALTY_SPLIT_ZERO] = 5;
  }

  printf("HevcPenaltyTest# splitPenalty %d %d %d %d %d\n",
         inst->asic.regs.pen[0][ASIC_PENALTY_SPLIT16x8],
         inst->asic.regs.pen[0][ASIC_PENALTY_SPLIT8x8],
         inst->asic.regs.pen[0][ASIC_PENALTY_SPLIT8x4],
         inst->asic.regs.pen[0][ASIC_PENALTY_SPLIT4x4],
         inst->asic.regs.pen[0][ASIC_PENALTY_SPLIT_ZERO]);
#endif
}

/*------------------------------------------------------------------------------
  HevcDownscaling
------------------------------------------------------------------------------*/
void HevcDownscalingTest(struct hevc_instance *inst)
{
  u32 frame = (u32)inst->frameCnt;
  u32 xy = MIN(inst->preProcess.lumWidth, inst->preProcess.lumHeight);

  if (!frame) return;

  if (frame <= xy / 2)
  {
    inst->preProcess.scaledWidth = inst->preProcess.lumWidth - (frame / 2) * 2;
    inst->preProcess.scaledHeight = inst->preProcess.lumHeight - frame * 2;
  }
  else
  {
    u32 i, x, y;
    i = frame - xy / 2;
    x = i % (inst->preProcess.lumWidth / 8);
    y = i / (inst->preProcess.lumWidth / 8);
    inst->preProcess.scaledWidth = inst->preProcess.lumWidth - x * 2;
    inst->preProcess.scaledHeight = inst->preProcess.lumHeight - y * 2;
  }

  if (!inst->preProcess.scaledWidth)
    inst->preProcess.scaledWidth = inst->preProcess.lumWidth - 8;

  if (!inst->preProcess.scaledHeight)
    inst->preProcess.scaledHeight = inst->preProcess.lumHeight - 8;

  printf("HevcDownscalingTest# %dx%d => %dx%d\n",
         inst->preProcess.lumWidth, inst->preProcess.lumHeight,
         inst->preProcess.scaledWidth, inst->preProcess.scaledHeight);
}


#if 0
/*------------------------------------------------------------------------------
  MbPerInterruptTest
------------------------------------------------------------------------------*/
void MbPerInterruptTest(trace_s *trace)
{
  if (trace->testId != 3)
  {
    return;
  }

  trace->control.mbPerInterrupt = trace->vopNum;
}


/*------------------------------------------------------------------------------
HevcSliceQuantTest()  Change sliceQP from min->max->min.
------------------------------------------------------------------------------*/
void HevcSliceQuantTest(trace_s *trace, slice_s *slice, mb_s *mb,
                        rateControl_s *rc)
{
  if (trace->testId != 8)
  {
    return;
  }

  rc->vopRc   = NO;
  rc->mbRc    = NO;
  rc->picSkip = NO;

  if (mb->qpPrev == rc->qpMin)
  {
    mb->qpLum = rc->qpMax;
  }
  else if (mb->qpPrev == rc->qpMax)
  {
    mb->qpLum = rc->qpMin;
  }
  else
  {
    mb->qpLum = rc->qpMax;
  }

  mb->qpCh  = qpCh[MIN(51, MAX(0, mb->qpLum + mb->chromaQpOffset))];
  rc->qp = rc->qpHdr = mb->qpLum;
  slice->sliceQpDelta = mb->qpLum - slice->picInitQpMinus26 - 26;
}

#endif

