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
--------------------------------------------------------------------------------
--
--  Description : ASIC low level controller
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "enccommon.h"
#include "encasiccontroller.h"
#include "encpreprocess.h"
#include "ewl.h"
#include "encswhwregisters.h"

/*------------------------------------------------------------------------------
    External compiler flags
------------------------------------------------------------------------------*/

extern void h265_EncTraceRegs(const void *ewl, u32 readWriteFlag, u32 mbNum);

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#ifdef ASIC_WAVE_TRACE_TRIGGER
extern i32 trigger_point;    /* picture which will be traced */
#endif

/* Mask fields */
#define mask_2b         (u32)0x00000003
#define mask_3b         (u32)0x00000007
#define mask_4b         (u32)0x0000000F
#define mask_5b         (u32)0x0000001F
#define mask_6b         (u32)0x0000003F
#define mask_7b         (u32)0x0000007F
#define mask_8b         (u32)0x000000FF
#define mask_10b        (u32)0x000003FF
#define mask_11b        (u32)0x000007FF
#define mask_14b        (u32)0x00003FFF
#define mask_16b        (u32)0x0000FFFF

#define HSWREG(n)       ((n)*4)

/* Global variables for test data trace file configuration.
 * These are set in testbench and used in system model test data generation. */
char *H2EncTraceFileConfig = NULL;
int H2EncTraceFirstFrame    = 0;

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Initialize empty structure with default values.
------------------------------------------------------------------------------*/
i32 h2_EncAsicControllerInit(asicData_s *asic)
{
  i32 i;
  ASSERT(asic != NULL);

  /* Initialize default values from defined configuration */
  asic->regs.irqDisable = ENCH2_IRQ_DISABLE;
  asic->regs.inputReadChunk = ENCH2_INPUT_READ_CHUNK;
  asic->regs.asic_axi_readID = ENCH2_AXI_READ_ID;
  asic->regs.asic_axi_writeID = ENCH2_AXI_WRITE_ID;
  asic->regs.asic_stream_swap = ENCH2_OUTPUT_SWAP_8 | (ENCH2_OUTPUT_SWAP_16 << 1) | (ENCH2_OUTPUT_SWAP_32 << 2) | (ENCH2_OUTPUT_SWAP_64 << 3);
  asic->regs.asic_burst_length = ENCH2_BURST_LENGTH & (63);
  asic->regs.asic_burst_scmd_disable = ENCH2_BURST_SCMD_DISABLE & (1);
  asic->regs.asic_burst_incr = (ENCH2_BURST_INCR_TYPE_ENABLED & (1));

  asic->regs.asic_data_discard = ENCH2_BURST_DATA_DISCARD_ENABLED & (1);
  asic->regs.asic_clock_gating = ENCH2_ASIC_CLOCK_GATING_ENABLED & (1);
  asic->regs.asic_axi_dual_channel = ENCH2_AXI_2CH_DISABLE;



  //asic->regs.recWriteBuffer = ENCH2_REC_WRITE_BUFFER & 1;

  /* User must set these */
  asic->regs.inputLumBase = 0;
  asic->regs.inputCbBase = 0;
  asic->regs.inputCrBase = 0;
  for (i = 0; i < ASIC_FRAME_BUF_LUM_MAX; i ++)
  {
    asic->internalreconLuma[i].virtualAddress = NULL;
    asic->internalreconChroma[i].virtualAddress = NULL;
    asic->compressTbl[i].virtualAddress = NULL;
  }
  asic->scaledImage.virtualAddress = NULL;
  asic->sizeTbl.virtualAddress = NULL;
  asic->cabacCtx.virtualAddress = NULL;
  asic->mvOutput.virtualAddress = NULL;
  asic->probCount.virtualAddress = NULL;
  asic->segmentMap.virtualAddress = NULL;
  asic->compress_coeff_SACN.virtualAddress = NULL;


  /* get ASIC ID value */
  asic->regs.asicHwId = h2_EncAsicGetId(asic->ewl);

  /* we do NOT reset hardware at this point because */
  /* of the multi-instance support                  */

  return ENCHW_OK;
}


/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
u32 h2_EncAsicGetId(const void *ewl)
{
  return h2_EWLReadReg(ewl, 0x0);
}

/*------------------------------------------------------------------------------
    When the frame is successfully encoded the internal image is recycled
    so that during the next frame the current internal image is read and
    the new reconstructed image is written. Note that this is done for
    the NEXT frame.
------------------------------------------------------------------------------*/
#define NEXTID(currentId, maxId) ((currentId == maxId) ? 0 : currentId+1)
#define PREVID(currentId, maxId) ((currentId == 0) ? maxId : currentId-1)


/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void CheckRegisterValues(regValues_s *val)
{


  ASSERT(val->irqDisable <= 1);
  ASSERT(val->filterDisable <= 2);
  ASSERT(val->qp <= 51);
  ASSERT(val->frameCodingType <= 2);
  ASSERT(val->codingType <= 2);
  ASSERT(val->outputStrmSize <= 0x6000000);
  ASSERT(val->pixelsOnRow >= 16 && val->pixelsOnRow <= 8192); /* max input for cropping */
  ASSERT(val->xFill <= 3);
  ASSERT(val->yFill <= 14 && ((val->yFill & 0x01) == 0));
  ASSERT(val->inputLumaBaseOffset <= 15);
  ASSERT(val->inputChromaBaseOffset <= 15);
  ASSERT(val->inputImageFormat <= ASIC_INPUT_RGB101010);
  ASSERT(val->inputImageRotation <= 2);

  if (val->codingType == ASIC_HEVC)
  {
    ASSERT(val->roi1DeltaQp >= 0 && val->roi1DeltaQp <= 15);
    ASSERT(val->roi2DeltaQp >= 0 && val->roi2DeltaQp <= 15);
  }
  ASSERT(val->cirStart <= 65535);
  ASSERT(val->cirInterval <= 65535);
  ASSERT(val->intraAreaTop <= 255);
  ASSERT(val->intraAreaLeft <= 255);
  ASSERT(val->intraAreaBottom <= 255);
  ASSERT(val->intraAreaRight <= 255);
  ASSERT(val->roi1Top <= 255);
  ASSERT(val->roi1Left <= 255);
  ASSERT(val->roi1Bottom <= 255);
  ASSERT(val->roi1Right <= 255);
  ASSERT(val->roi2Top <= 255);
  ASSERT(val->roi2Left <= 255);
  ASSERT(val->roi2Bottom <= 255);
  ASSERT(val->roi2Right <= 255);
  (void) val;
}

/*------------------------------------------------------------------------------
    Function name   : h2_EncAsicFrameStart
    Description     :
    Return type     : void
    Argument        : const void *ewl
    Argument        : regValues_s * val
------------------------------------------------------------------------------*/
void h2_EncAsicFrameStart(const void *ewl, regValues_s *val)
{
  i32 i;
  //u32 asicCfgReg = val->asicCfgReg;
  if (val->inputImageFormat < ASIC_INPUT_RGB565) /* YUV input */
    val->asic_pic_swap = ENCH2_INPUT_SWAP_8_YUV | (ENCH2_INPUT_SWAP_16_YUV << 1) | (ENCH2_INPUT_SWAP_32_YUV << 2) | (ENCH2_INPUT_SWAP_64_YUV << 3);
  else if (val->inputImageFormat < ASIC_INPUT_RGB888) /* 16-bit RGB input */
    val->asic_pic_swap = ENCH2_INPUT_SWAP_8_RGB16 | (ENCH2_INPUT_SWAP_16_RGB16 << 1) | (ENCH2_INPUT_SWAP_32_RGB16 << 2) | (ENCH2_INPUT_SWAP_64_RGB16 << 3);
  else   /* 32-bit RGB input */
    val->asic_pic_swap = ENCH2_INPUT_SWAP_8_RGB32 | (ENCH2_INPUT_SWAP_16_RGB32 << 1) | (ENCH2_INPUT_SWAP_32_RGB32 << 2) | (ENCH2_INPUT_SWAP_64_RGB32 << 3);

  CheckRegisterValues(val);

  h2_EWLmemset(val->regMirror, 0, sizeof(val->regMirror));

  /* encoder interrupt */
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_IRQ_DIS, val->irqDisable);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_READ_CHUNK, val->inputReadChunk);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_AXI_READ_ID, val->asic_axi_readID);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_AXI_WRITE_ID, val->asic_axi_writeID);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BURST_DISABLE, val->asic_burst_scmd_disable);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BURST_INCR, val->asic_burst_incr);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DATA_DISCARD, val->asic_data_discard);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CLOCK_GATING, val->asic_clock_gating);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_AXI_DUAL_CH, val->asic_axi_dual_channel);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MODE, val->codingType);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SLICE_SIZE, val->sliceSize);

  /* output stream buffer */
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_OUTPUT_STRM_BASE, val->outputStrmBase);
  /* stream buffer limits */
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_OUTPUT_STRM_BUFFER_LIMIT, val->outputStrmSize);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_STRM_SWAP, val->asic_stream_swap);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_SWAP, val->asic_pic_swap);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_FRAME_CODING_TYPE, val->frameCodingType);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_STRONG_INTRA_SMOOTHING_ENABLED_FLAG, val->strong_intra_smoothing_enabled_flag);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CONSTRAINED_INTRA_PRED_FLAG, val->constrained_intra_pred_flag);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_POC, val->poc);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_OUTPUT_STRM_MODE, val->hevcStrmMode);

  /* Input picture buffers */
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_Y_BASE, val->inputLumBase);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_CB_BASE, val->inputCbBase);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_CR_BASE, val->inputCrBase);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_Y_BASE, val->reconLumBase);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_CHROMA_BASE, val->reconCbBase);
  //h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_CR_BASE, val->reconCrBase);


  {

    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MIN_CB_SIZE, val->minCbSize - 3);


    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_CB_SIZE, val->maxCbSize - 3);


    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MIN_TRB_SIZE, val->minTrbSize - 2);

    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_TRB_SIZE, val->maxTrbSize - 2);
  }



  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_WIDTH, val->picWidth / 8);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_HEIGHT, val->picHeight / 8);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PPS_DEBLOCKING_FILTER_OVERRIDE_ENABLED_FLAG, val->pps_deblocking_filter_override_enabled_flag);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SLICE_DEBLOCKING_FILTER_OVERRIDE_FLAG, val->slice_deblocking_filter_override_flag);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_INIT_QP, val->picInitQp);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_QP, val->qp);

  //   h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_QP_MAX, val->qpMax);

  //  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_QP_MIN, val->qpMin);


  //h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DIFF_CU_QP_DELTA_DEPTH, val->diffCuQpDeltaDepth);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CHROMA_QP_OFFSET, val->cbQpOffset & mask_5b);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SAO_ENABLE, val->saoEnable);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_TRANS_HIERARCHY_DEPTH_INTER, val->maxTransHierarchyDepthInter);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_TRANS_HIERARCHY_DEPTH_INTRA, val->maxTransHierarchyDepthIntra);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CU_QP_DELTA_ENABLED, val->cuQpDeltaEnabled);
  //h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LOG2_PARALLEL_MERGE_LEVEL, val->log2ParellelMergeLevel);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NUM_SHORT_TERM_REF_PIC_SETS, val->numShortTermRefPicSets);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RPS_ID, val->rpsId);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NUM_NEGATIVE_PICS, val->numNegativePics);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NUM_POSITIVE_PICS, val->numPositivePics);


  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DELTA_POC0, val->delta_poc0 & mask_10b);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_USED_BY_CURR_PIC0, val->used_by_curr_pic0);


  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DELTA_POC1, val->delta_poc1 & mask_10b);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_USED_BY_CURR_PIC1, val->used_by_curr_pic1);


  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_Y0, val->pRefPic_recon_l0[0][0]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_CHROMA0, val->pRefPic_recon_l0[1][0]);
  //h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_CR0, val->pRefPic_recon_l0[2][0]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALING_LIST_ENABLED_FLAG, val->scaling_list_enabled_flag);




  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_Y1, val->pRefPic_recon_l0[0][1]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_CHROMA1, val->pRefPic_recon_l0[1][1]);
  //h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_CR1, val->pRefPic_recon_l0[2][1]);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ACTIVE_L0_CNT, val->active_l0_cnt);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ACTIVE_OVERRIDE_FLAG, val->active_override_flag);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CIR_START, val->cirStart);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CIR_INTERVAL, val->cirInterval);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_AREA_LEFT, val->intraAreaLeft);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_AREA_RIGHT, val->intraAreaRight);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_AREA_TOP, val->intraAreaTop);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_AREA_BOTTOM, val->intraAreaBottom);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_LEFT, val->roi1Left);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_RIGHT, val->roi1Right);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_TOP, val->roi1Top);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_BOTTOM, val->roi1Bottom);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_LEFT, val->roi2Left);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_RIGHT, val->roi2Right);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_TOP, val->roi2Top);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_BOTTOM, val->roi2Bottom);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_DELTA_QP, MIN((i32)val->qp, val->roi1DeltaQp));
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_DELTA_QP, MIN((i32)val->qp, val->roi2DeltaQp));


  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DEBLOCKING_FILTER_CTRL, val->filterDisable);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DEBLOCKING_TC_OFFSET, (val->tc_Offset / 2)&mask_4b);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DEBLOCKING_BETA_OFFSET, (val->beta_Offset / 2)&mask_4b);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_PIC4X4, val->intraPenaltyPic4x4);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_PIC8X8, val->intraPenaltyPic8x8);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_PIC16X16, val->intraPenaltyPic16x16);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_PIC32X32, val->intraPenaltyPic32x32);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_PIC1, val->intraMPMPenaltyPic1);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_PIC2, val->intraMPMPenaltyPic2);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_PIC3, val->intraMPMPenaltyPic3);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI14X4, val->intraPenaltyRoi14x4);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI18X8, val->intraPenaltyRoi18x8);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI116X16, val->intraPenaltyRoi116x16);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI132X32, val->intraPenaltyRoi132x32);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI11, val->intraMPMPenaltyRoi11);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI12, val->intraMPMPenaltyRoi12);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI13, val->intraMPMPenaltyRoi13);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI24X4, val->intraPenaltyRoi24x4);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI28X8, val->intraPenaltyRoi28x8);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI216X16, val->intraPenaltyRoi216x16);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI232X32, val->intraPenaltyRoi232x32);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI21, val->intraMPMPenaltyRoi21);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI22, val->intraMPMPenaltyRoi22);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI23, val->intraMPMPenaltyRoi23);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SIZE_TBL_BASE, val->sizeTblBase);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NAL_SIZE_WRITE, val->sizeTblBase != 0);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SAO_LUMA, val->lamda_SAO_luma);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SAO_CHROMA, val->lamda_SAO_chroma);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_MOTION_SSE, val->lamda_motion_sse);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_TU_SPLIT_PENALTY, val->bits_est_tu_split_penalty);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_8, val->bits_est_bias_intra_cu_8);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_16, val->bits_est_bias_intra_cu_16);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_32, val->bits_est_bias_intra_cu_32);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_64, val->bits_est_bias_intra_cu_64);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTER_SKIP_BIAS, val->inter_skip_bias);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_1N_CU_PENALTY, val->bits_est_1n_cu_penalty);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMBDA_MOTIONSAD, val->lambda_motionSAD);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_MOTION_SSE_ROI1, val->lamda_motion_sse_roi1);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_MOTION_SSE_ROI2, val->lamda_motion_sse_roi2);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SKIP_CHROMA_DC_THREADHOLD, val->skip_chroma_dc_threadhold);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMBDA_MOTIONSAD_ROI1, val->lambda_motionSAD_ROI1);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMBDA_MOTIONSAD_ROI2, val->lambda_motionSAD_ROI2);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_FORMAT, val->inputImageFormat);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_ROTATION, val->inputImageRotation);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFA, val->colorConversionCoeffA);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFB, val->colorConversionCoeffB);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFC, val->colorConversionCoeffC);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFE, val->colorConversionCoeffE);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFF, val->colorConversionCoeffF);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RMASKMSB, val->rMaskMsb);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_GMASKMSB, val->gMaskMsb);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BMASKMSB, val->bMaskMsb);


  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CHROFFSET, val->inputChromaBaseOffset);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LUMOFFSET, val->inputLumaBaseOffset);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROWLENGTH, val->pixelsOnRow);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_XFILL, val->xFill);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_YFILL, val->yFill);


  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALE_MODE, val->scaledWidth > 0 ? 2 : 0);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BASESCALEDOUTLUM, val->scaledLumBase);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUTWIDTH, val->scaledWidth);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUTWIDTHRATIO, val->scaledWidthRatio);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUTHEIGHT, val->scaledHeight);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUTHEIGHTRATIO, val->scaledHeightRatio);
  val->scaledOutSwap = (ENCH2_SCALE_OUTPUT_SWAP_64 << 3) | (ENCH2_SCALE_OUTPUT_SWAP_32 << 2) | (ENCH2_SCALE_OUTPUT_SWAP_16 << 1) | ENCH2_SCALE_OUTPUT_SWAP_8;
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUT_SWAP, val->scaledOutSwap);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CHROMA_SWAP, val->chromaSwap);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_COMPRESSEDCOEFF_BASE, val->compress_coeff_scan_base);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CABAC_INIT_FLAG, val->cabac_init_flag);

  //compress
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_LUMA_COMPRESSOR_ENABLE, val->recon_luma_compress);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_CHROMA_COMPRESSOR_ENABLE, val->recon_chroma_compress);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF0_LUMA_COMPRESSOR_ENABLE, val->ref_l0_luma_compressed[0]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF0_CHROMA_COMPRESSOR_ENABLE, val->ref_l0_chroma_compressed[0]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF1_LUMA_COMPRESSOR_ENABLE, val->ref_l0_luma_compressed[1]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF1_CHROMA_COMPRESSOR_ENABLE, val->ref_l0_chroma_compressed[1]);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_LUMA_COMPRESS_TABLE_BASE, val->recon_luma_compress_tbl_base);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_CHROMA_COMPRESS_TABLE_BASE, val->recon_chroma_compress_tbl_base);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF0_LUMA_COMPRESS_TABLE_BASE, val->ref_l0_luma_compress_tbl_base[0]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF0_CHROMA_COMPRESS_TABLE_BASE, val->ref_l0_chroma_compress_tbl_base[0]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF1_LUMA_COMPRESS_TABLE_BASE, val->ref_l0_luma_compress_tbl_base[1]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF1_CHROMA_COMPRESS_TABLE_BASE,  val->ref_l0_chroma_compress_tbl_base[1]);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_LUMA_4N_BASE,  val->reconL4nBase);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_4N0_BASE,  val->pRefPic_recon_l0_4n[0]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_4N1_BASE,  val->pRefPic_recon_l0_4n[1]);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_TIMEOUT_INT, ENCH2_TIMEOUT_INTERRUPT & 1);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SLICE_INT, val->sliceReadyInterrupt);

  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_BURST, ENCH2_AXI40_BURST_LENGTH);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_TIMEOUT_OVERRIDE_E, ENCH2_ASIC_TIMEOUT_OVERRIDE_ENABLE);
  h2_EncAsicSetRegisterValue(val->regMirror, HWIF_TIMEOUT_CYCLES, ENCH2_ASIC_TIMEOUT_CYCLES);
#ifdef SUPPORT_VP9
  if (val->codingType == ASIC_VP9)
  {
    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VP9PARTITION1_BASE, val->partitionBase[0]);
    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VP9PARTITION2_BASE, val->partitionBase[1]);
    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VP9PARTITION3_BASE, val->partitionBase[2]);
    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VP9PARTITION4_BASE, val->partitionBase[3]);
    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VP9CABACCTX_BASE, val->cabacCtxBase);

    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VP9PROBCOUNT_BASE, val->probCountBase);
    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VP9MVREFIDX1, val->mvRefIdx[0]);
    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VP9MVREFIDX2, val->mvRefIdx[1]);
    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REF2ENABLE, val->ref2Enable);

    h2_EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_IPOLFILTERMODE, val->ipolFilterMode);
  }
#endif  
  {
    for (i = 1; i < ASIC_SWREG_AMOUNT; i++)
      h2_EWLWriteReg(ewl, HSWREG(i), val->regMirror[i]);
  }

#ifdef TRACE_REGS
  h265_EncTraceRegs(ewl, 0, 0);
#endif
  /* Register with enable bit is written last */
  val->regMirror[ASIC_REG_INDEX_STATUS] |= ASIC_STATUS_ENABLE;

  h2_EWLEnableHW(ewl, HSWREG(ASIC_REG_INDEX_STATUS), val->regMirror[ASIC_REG_INDEX_STATUS]);
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void EncAsicFrameContinue(const void *ewl, regValues_s *val)
{
  /* clear status bits, clear IRQ => HW restart */
  u32 status = val->regMirror[1];

  status &= (~ASIC_STATUS_ALL);
  status &= ~ASIC_IRQ_LINE;

  val->regMirror[1] = status;

  /*CheckRegisterValues(val); */

  /* Write only registers which may be updated mid frame */
  // h2_EWLWriteReg(ewl, HSWREG(24), (val->rlcLimitSpace / 2));

  //val->regMirror[5] = val->rlcBase;
  h2_EWLWriteReg(ewl, HSWREG(5), val->regMirror[5]);

#ifdef TRACE_REGS
  h265_EncTraceRegs(ewl, 0, h2_EncAsicGetRegisterValue(ewl, val->regMirror, HWIF_ENC_ENCODED_CTB_NUMBER));
#endif

  /* Register with status bits is written last */
  h2_EWLEnableHW(ewl, HSWREG(4), val->regMirror[4]);

}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void h2_EncAsicGetRegisters(const void *ewl, regValues_s *val)
{
  (void)ewl;
  (void)val;

  /* HW output stream size, bits to bytes */
  int i;


  for (i = 1; i < ASIC_SWREG_AMOUNT; i++)
  {
    val->regMirror[i] = h2_EWLReadReg(ewl, HSWREG(i));
  }

#ifdef TRACE_REGS
  h265_EncTraceRegs(ewl, 1, h2_EncAsicGetRegisterValue(ewl, val->regMirror, HWIF_ENC_ENCODED_CTB_NUMBER));
#endif
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void h2_EncAsicStop(const void *ewl)
{
  h2_EWLDisableHW(ewl, HSWREG(4), 0);
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
u32 h2_EncAsicGetStatus(const void *ewl)
{
  return h2_EWLReadReg(ewl, HSWREG(1));
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

void h2_EncAsicClearStatus(const void *ewl,u32 value)
{
  h2_EWLWriteReg(ewl, HSWREG(1),value);
}

/*------------------------------------------------------------------------------
    h2_EncAsicGetMvOutput
        Return encOutputMbInfo_s pointer to beginning of macroblock mbNum
------------------------------------------------------------------------------*/
u32 *h2_EncAsicGetMvOutput(asicData_s *asic, u32 mbNum)
{
  u32 *mvOutput = asic->mvOutput.virtualAddress;
  u32 traceMbTiming = 0;//asic->regs.traceMbTiming;

  if (traceMbTiming)    /* encOutputMbInfoDebug_s */
  {
    if (asic->regs.asicHwId <= ASIC_ID_EVERGREEN_PLUS)
    {
      mvOutput += mbNum * 40 / 4;
    }
    else
    {
      mvOutput += mbNum * 56 / 4;
    }
  }
  else        /* encOutputMbInfo_s */
  {
    if (asic->regs.asicHwId <= ASIC_ID_EVERGREEN_PLUS)
    {
      mvOutput += mbNum * 32 / 4;
    }
    else
    {
      mvOutput += mbNum * 48 / 4;
    }
  }

  return mvOutput;
}
