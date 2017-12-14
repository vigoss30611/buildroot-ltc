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

extern void EncTraceRegs(const void *ewl, u32 readWriteFlag, u32 mbNum);

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
#define mask_21b        (u32)0x001FFFFF

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
i32 EncAsicControllerInit(asicData_s *asic)
{
  i32 i;
  ASSERT(asic != NULL);

  /* Initialize default values from defined configuration */
  asic->regs.irqDisable = ENCH2_IRQ_DISABLE;
  asic->regs.inputReadChunk = ENCH2_INPUT_READ_CHUNK;
  asic->regs.asic_axi_readID = ENCH2_AXI_READ_ID;
  asic->regs.asic_axi_writeID = ENCH2_AXI_WRITE_ID;
  asic->regs.asic_stream_swap = ENCH2_OUTPUT_SWAP_8 | (ENCH2_OUTPUT_SWAP_16 << 1) | (ENCH2_OUTPUT_SWAP_32 << 2) | (ENCH2_OUTPUT_SWAP_64 << 3);
  asic->regs.scaledOutSwap = (ENCH2_SCALE_OUTPUT_SWAP_64 << 3) | (ENCH2_SCALE_OUTPUT_SWAP_32 << 2) | (ENCH2_SCALE_OUTPUT_SWAP_16 << 1) | ENCH2_SCALE_OUTPUT_SWAP_8;
  asic->regs.nalUnitSizeSwap = (ENCH2_NALUNIT_SIZE_SWAP_64<< 3) | (ENCH2_NALUNIT_SIZE_SWAP_32 << 2) | (ENCH2_NALUNIT_SIZE_SWAP_16 << 1) | ENCH2_NALUNIT_SIZE_SWAP_8;
  asic->regs.asic_roi_map_qp_delta_swap = ENCH2_ROIMAP_QPDELTA_SWAP_8 | (ENCH2_ROIMAP_QPDELTA_SWAP_16 << 1) | (ENCH2_ROIMAP_QPDELTA_SWAP_32 << 2) | (ENCH2_ROIMAP_QPDELTA_SWAP_64 << 3);
  asic->regs.asic_ctb_rc_mem_out_swap = ENCH2_CTBRC_OUTPUT_SWAP_8 | (ENCH2_CTBRC_OUTPUT_SWAP_16 << 1) | (ENCH2_CTBRC_OUTPUT_SWAP_32 << 2) | (ENCH2_CTBRC_OUTPUT_SWAP_64 << 3);
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
  asic->regs.asicHwId = EWLReadAsicID();

  /* we do NOT reset hardware at this point because */
  /* of the multi-instance support                  */

  return ENCHW_OK;
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
  ASSERT(val->inputImageFormat <= ASIC_INPUT_PACKED_10BIT_Y0L2);
  ASSERT(val->inputImageRotation <= 3);
  ASSERT(val->outputBitWidthLuma <= 2);
  ASSERT(val->outputBitWidthChroma<= 2);

  if (val->codingType == ASIC_HEVC)
  {
    ASSERT(val->roi1DeltaQp >= 0 && val->roi1DeltaQp <= 30);
    ASSERT(val->roi2DeltaQp >= 0 && val->roi2DeltaQp <= 30);
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
    Function name   : EncAsicFrameStart
    Description     :
    Return type     : void
    Argument        : const void *ewl
    Argument        : regValues_s * val
------------------------------------------------------------------------------*/
void EncAsicFrameStart(const void *ewl, regValues_s *val)
{
  i32 i;
  //u32 asicCfgReg = val->asicCfgReg;
  if (val->inputImageFormat < ASIC_INPUT_RGB565) /* YUV input */
    val->asic_pic_swap = ENCH2_INPUT_SWAP_8_YUV | (ENCH2_INPUT_SWAP_16_YUV << 1) | (ENCH2_INPUT_SWAP_32_YUV << 2) | (ENCH2_INPUT_SWAP_64_YUV << 3);
  else if (val->inputImageFormat < ASIC_INPUT_RGB888) /* 16-bit RGB input */
    val->asic_pic_swap = ENCH2_INPUT_SWAP_8_RGB16 | (ENCH2_INPUT_SWAP_16_RGB16 << 1) | (ENCH2_INPUT_SWAP_32_RGB16 << 2) | (ENCH2_INPUT_SWAP_64_RGB16 << 3);
  else if(val->inputImageFormat < ASIC_INPUT_I010)   /* 32-bit RGB input */
    val->asic_pic_swap = ENCH2_INPUT_SWAP_8_RGB32 | (ENCH2_INPUT_SWAP_16_RGB32 << 1) | (ENCH2_INPUT_SWAP_32_RGB32 << 2) | (ENCH2_INPUT_SWAP_64_RGB32 << 3);
  else if(val->inputImageFormat >= ASIC_INPUT_I010&&val->inputImageFormat <= ASIC_INPUT_PACKED_10BIT_Y0L2)
     val->asic_pic_swap = ENCH2_INPUT_SWAP_8_YUV | (ENCH2_INPUT_SWAP_16_YUV << 1) | (ENCH2_INPUT_SWAP_32_YUV << 2) | (ENCH2_INPUT_SWAP_64_YUV << 3);

  //after H2V4, prp swap changed
#if ENCH2_INPUT_SWAP_64_YUV==1
  if (val->asicHwId >= 0x48320400) /* H2V4+ */
    val->asic_pic_swap = ((~val->asic_pic_swap) & 0xf);
#endif

  CheckRegisterValues(val);

  EWLmemset(val->regMirror, 0, sizeof(val->regMirror));
  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_AXI_WR_ID_E, ENCH2_ASIC_AXI_WR_ID_E);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_AXI_RD_ID_E, ENCH2_ASIC_AXI_RD_ID_E);
  /* encoder interrupt */
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_IRQ_DIS, val->irqDisable);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_READ_CHUNK, val->inputReadChunk);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_AXI_READ_ID, val->asic_axi_readID);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_AXI_WRITE_ID, val->asic_axi_writeID);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BURST_DISABLE, val->asic_burst_scmd_disable);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BURST_INCR, val->asic_burst_incr);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DATA_DISCARD, val->asic_data_discard);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CLOCK_GATING, val->asic_clock_gating);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_AXI_DUAL_CH, val->asic_axi_dual_channel);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MODE, val->codingType);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SLICE_SIZE, val->sliceSize);

  /* output stream buffer */
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_OUTPUT_STRM_BASE, val->outputStrmBase);
  /* stream buffer limits */
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_OUTPUT_STRM_BUFFER_LIMIT, val->outputStrmSize);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_STRM_SWAP, val->asic_stream_swap);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_SWAP, val->asic_pic_swap);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI_MAP_QP_DELTA_MAP_SWAP, val->asic_roi_map_qp_delta_swap);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CTB_RC_MEM_OUT_SWAP, val->asic_ctb_rc_mem_out_swap);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUT_SWAP, val->scaledOutSwap);
  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NALUNITSIZE_SWAP, val->nalUnitSizeSwap);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_FRAME_CODING_TYPE, val->frameCodingType);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_STRONG_INTRA_SMOOTHING_ENABLED_FLAG, val->strong_intra_smoothing_enabled_flag);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CONSTRAINED_INTRA_PRED_FLAG, val->constrained_intra_pred_flag);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_POC, val->poc);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_OUTPUT_STRM_MODE, val->hevcStrmMode);

  /* Input picture buffers */
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_Y_BASE, val->inputLumBase);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_CB_BASE, val->inputCbBase);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_CR_BASE, val->inputCrBase);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_Y_BASE, val->reconLumBase);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_CHROMA_BASE, val->reconCbBase);
  //EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_CR_BASE, val->reconCrBase);


  {

    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MIN_CB_SIZE, val->minCbSize - 3);


    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_CB_SIZE, val->maxCbSize - 3);


    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MIN_TRB_SIZE, val->minTrbSize - 2);

    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_TRB_SIZE, val->maxTrbSize - 2);
  }



  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_WIDTH, val->picWidth / 8);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_HEIGHT, val->picHeight / 8);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PPS_DEBLOCKING_FILTER_OVERRIDE_ENABLED_FLAG, val->pps_deblocking_filter_override_enabled_flag);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SLICE_DEBLOCKING_FILTER_OVERRIDE_FLAG, val->slice_deblocking_filter_override_flag);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_INIT_QP, val->picInitQp);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_QP, val->qp);

  //   EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_QP_MAX, val->qpMax);

  //  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_PIC_QP_MIN, val->qpMin);


  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DIFF_CU_QP_DELTA_DEPTH, val->diffCuQpDeltaDepth);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CHROMA_QP_OFFSET, val->cbQpOffset & mask_5b);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SAO_ENABLE, val->saoEnable);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_TRANS_HIERARCHY_DEPTH_INTER, val->maxTransHierarchyDepthInter);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_TRANS_HIERARCHY_DEPTH_INTRA, val->maxTransHierarchyDepthIntra);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CU_QP_DELTA_ENABLED, val->cuQpDeltaEnabled);
  //EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LOG2_PARALLEL_MERGE_LEVEL, val->log2ParellelMergeLevel);
  switch(val->asicHwId)
  {
  case 0x48320101: /* H2V1 rev01 */
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NUM_SHORT_TERM_REF_PIC_SETS, val->numShortTermRefPicSets);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RPS_ID, val->rpsId);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_PIC4X4, val->intraPenaltyPic4x4);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_PIC1, val->intraMPMPenaltyPic1);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_PIC2, val->intraMPMPenaltyPic2);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_PIC8X8, val->intraPenaltyPic8x8);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_PIC16X16, val->intraPenaltyPic16x16);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_PIC32X32, val->intraPenaltyPic32x32);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_PIC3, val->intraMPMPenaltyPic3);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI14X4, val->intraPenaltyRoi14x4);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI11, val->intraMPMPenaltyRoi11);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI12, val->intraMPMPenaltyRoi12);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI18X8, val->intraPenaltyRoi18x8);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI116X16, val->intraPenaltyRoi116x16);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI132X32, val->intraPenaltyRoi132x32);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI13, val->intraMPMPenaltyRoi13);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI24X4, val->intraPenaltyRoi24x4);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI21, val->intraMPMPenaltyRoi21);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI22, val->intraMPMPenaltyRoi22);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI28X8, val->intraPenaltyRoi28x8);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI216X16, val->intraPenaltyRoi216x16);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_PENALTY_ROI232X32, val->intraPenaltyRoi232x32);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MPM_PENALTY_ROI23, val->intraMPMPenaltyRoi23);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMBDA_MOTIONSAD, val->lambda_motionSAD);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_MOTION_SSE_ROI1, val->lamda_motion_sse_roi1);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_MOTION_SSE_ROI2, val->lamda_motion_sse_roi2);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SKIP_CHROMA_DC_THREADHOLD, val->skip_chroma_dc_threadhold);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMBDA_MOTIONSAD_ROI1, val->lambda_motionSAD_ROI1);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMBDA_MOTIONSAD_ROI2, val->lambda_motionSAD_ROI2);

    break;
  case 0x48320201: /* H2V2 rev01 */
  case 0x48320301: /* H2V3 rev01 */
  case 0x48320401: /* H2V4 rev01 */
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NUM_SHORT_TERM_REF_PIC_SETS_V2, val->numShortTermRefPicSets);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RPS_ID_V2, val->rpsId);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SIZE_FACTOR_0, val->intra_size_factor[0]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SIZE_FACTOR_1, val->intra_size_factor[1]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SIZE_FACTOR_2, val->intra_size_factor[2]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SIZE_FACTOR_3, val->intra_size_factor[3]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MODE_FACTOR_0, val->intra_mode_factor[0]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MODE_FACTOR_1, val->intra_mode_factor[1]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_MODE_FACTOR_2, val->intra_mode_factor[2]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_0, val->lambda_satd_me[0]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_1, val->lambda_satd_me[1]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_2, val->lambda_satd_me[2]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_3, val->lambda_satd_me[3]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_4, val->lambda_satd_me[4]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_5, val->lambda_satd_me[5]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_6, val->lambda_satd_me[6]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_7, val->lambda_satd_me[7]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_8, val->lambda_satd_me[8]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_9, val->lambda_satd_me[9]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_10, val->lambda_satd_me[10]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_11, val->lambda_satd_me[11]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_12, val->lambda_satd_me[12]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_13, val->lambda_satd_me[13]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_14, val->lambda_satd_me[14]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_15, val->lambda_satd_me[15]);
#ifdef CTBRC_STRENGTH
    
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_QP_FRACTIONAL, val->qpfrac);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_QP_DELTA_GAIN, val->qpDeltaMBGain);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_COMPLEXITY_OFFSET, val->offsetMBComplexity);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RC_BLOCK_SIZE, val->rcBlockSize);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RC_CTBRC_SLICEQPOFFSET, val->offsetSliceQp & mask_6b);

    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_16, val->lambda_satd_me[16]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_17, val->lambda_satd_me[17]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_18, val->lambda_satd_me[18]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_19, val->lambda_satd_me[19]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_20, val->lambda_satd_me[20]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_21, val->lambda_satd_me[21]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_22, val->lambda_satd_me[22]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_23, val->lambda_satd_me[23]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_24, val->lambda_satd_me[24]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_25, val->lambda_satd_me[25]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_26, val->lambda_satd_me[26]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_27, val->lambda_satd_me[27]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_28, val->lambda_satd_me[28]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_29, val->lambda_satd_me[29]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_30, val->lambda_satd_me[30]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SATD_ME_31, val->lambda_satd_me[31]);
#endif
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_0, val->lambda_sse_me[0]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_1, val->lambda_sse_me[1]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_2, val->lambda_sse_me[2]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_3, val->lambda_sse_me[3]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_4, val->lambda_sse_me[4]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_5, val->lambda_sse_me[5]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_6, val->lambda_sse_me[6]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_7, val->lambda_sse_me[7]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_8, val->lambda_sse_me[8]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_9, val->lambda_sse_me[9]);    
#ifdef CTBRC_STRENGTH
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_16, val->lambda_sse_me[16]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_17, val->lambda_sse_me[17]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_18, val->lambda_sse_me[18]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_19, val->lambda_sse_me[19]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_20, val->lambda_sse_me[20]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_21, val->lambda_sse_me[21]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_22, val->lambda_sse_me[22]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_23, val->lambda_sse_me[23]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_24, val->lambda_sse_me[24]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_25, val->lambda_sse_me[25]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_26, val->lambda_sse_me[26]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_27, val->lambda_sse_me[27]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_28, val->lambda_sse_me[28]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_29, val->lambda_sse_me[29]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_30, val->lambda_sse_me[30]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_31, val->lambda_sse_me[31]);  

    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_16, val->lambda_satd_ims[16]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_17, val->lambda_satd_ims[17]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_18, val->lambda_satd_ims[18]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_19, val->lambda_satd_ims[19]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_20, val->lambda_satd_ims[20]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_21, val->lambda_satd_ims[21]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_22, val->lambda_satd_ims[22]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_23, val->lambda_satd_ims[23]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_24, val->lambda_satd_ims[24]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_25, val->lambda_satd_ims[25]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_26, val->lambda_satd_ims[26]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_27, val->lambda_satd_ims[27]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_28, val->lambda_satd_ims[28]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_29, val->lambda_satd_ims[29]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_30, val->lambda_satd_ims[30]);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_31, val->lambda_satd_ims[31]);
    
#endif
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NAL_UNIT_TYPE, val->nal_unit_type);
    EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NUH_TEMPORAL_ID, val->nuh_temporal_id);
  
    break;
  default:
    ASSERT(0);
  }
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NUM_NEGATIVE_PICS, val->numNegativePics);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NUM_POSITIVE_PICS, val->numPositivePics);


  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_DELTA_POC0, val->l0_delta_poc[0] & mask_10b);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_USED_BY_CURR_PIC0, val->l0_used_by_curr_pic[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_DELTA_POC1, val->l0_delta_poc[1] & mask_10b);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_USED_BY_CURR_PIC1, val->l0_used_by_curr_pic[1]);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_DELTA_POC0, val->l1_delta_poc[0] & mask_10b);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_USED_BY_CURR_PIC0, val->l1_used_by_curr_pic[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_DELTA_POC1, val->l1_delta_poc[1] & mask_10b);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_USED_BY_CURR_PIC1, val->l1_used_by_curr_pic[1]);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_Y0, val->pRefPic_recon_l0[0][0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_CHROMA0, val->pRefPic_recon_l0[1][0]);
  //EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_CR0, val->pRefPic_recon_l0[2][0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALING_LIST_ENABLED_FLAG, val->scaling_list_enabled_flag);




  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_Y1, val->pRefPic_recon_l0[0][1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_CHROMA1, val->pRefPic_recon_l0[1][1]);
  //EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_CR1, val->pRefPic_recon_l0[2][1]);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L1_Y0, val->pRefPic_recon_l1[0][0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L1_CHROMA0, val->pRefPic_recon_l1[1][0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L1_Y1, val->pRefPic_recon_l1[0][1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L1_CHROMA1, val->pRefPic_recon_l1[1][1]);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ACTIVE_L0_CNT, val->active_l0_cnt);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ACTIVE_L1_CNT, val->active_l1_cnt);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ACTIVE_OVERRIDE_FLAG, val->active_override_flag);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CIR_START, val->cirStart);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CIR_INTERVAL, val->cirInterval);
  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RCROI_ENABLE, val->rcRoiEnable);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROIMAPDELTAQPADDR, val->roiMapDeltaQpAddr);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_AREA_LEFT, val->intraAreaLeft);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_AREA_RIGHT, val->intraAreaRight);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_AREA_TOP, val->intraAreaTop);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_AREA_BOTTOM, val->intraAreaBottom);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_LEFT, val->roi1Left);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_RIGHT, val->roi1Right);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_TOP, val->roi1Top);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_BOTTOM, val->roi1Bottom);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_LEFT, val->roi2Left);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_RIGHT, val->roi2Right);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_TOP, val->roi2Top);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_BOTTOM, val->roi2Bottom);
#ifndef CTBRC_STRENGTH

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_DELTA_QP, MIN((i32)val->qp, val->roi1DeltaQp));
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_DELTA_QP, MIN((i32)val->qp, val->roi2DeltaQp));

#else  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI1_DELTA_QP_RC, val->roi1DeltaQp);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROI2_DELTA_QP_RC, val->roi2DeltaQp);

  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_QP_MIN, val->qpMin);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_QP_MAX, val->qpMax);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RC_QPDELTA_RANGE, val->rcQpDeltaRange);

#endif
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DEBLOCKING_FILTER_CTRL, val->filterDisable);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DEBLOCKING_TC_OFFSET, (val->tc_Offset / 2)&mask_4b);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_DEBLOCKING_BETA_OFFSET, (val->beta_Offset / 2)&mask_4b);

  



  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SIZE_TBL_BASE, val->sizeTblBase);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NAL_SIZE_WRITE, val->sizeTblBase != 0);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SAO_LUMA, val->lamda_SAO_luma);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SAO_CHROMA, val->lamda_SAO_chroma);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_MOTION_SSE, val->lamda_motion_sse);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_TU_SPLIT_PENALTY, val->bits_est_tu_split_penalty);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_8, val->bits_est_bias_intra_cu_8);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_16, val->bits_est_bias_intra_cu_16);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_32, val->bits_est_bias_intra_cu_32);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_64, val->bits_est_bias_intra_cu_64);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTER_SKIP_BIAS, val->inter_skip_bias);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITS_EST_1N_CU_PENALTY, val->bits_est_1n_cu_penalty);


  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_FORMAT, val->inputImageFormat);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_OUTPUT_BITWIDTH_CHROMA, val->outputBitWidthChroma);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_OUTPUT_BITWIDTH_LUM, val->outputBitWidthLuma);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INPUT_ROTATION, val->inputImageRotation);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFA, val->colorConversionCoeffA);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFB, val->colorConversionCoeffB);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFC, val->colorConversionCoeffC);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFE, val->colorConversionCoeffE);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RGBCOEFFF, val->colorConversionCoeffF);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RMASKMSB, val->rMaskMsb);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_GMASKMSB, val->gMaskMsb);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BMASKMSB, val->bMaskMsb);


  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CHROFFSET, val->inputChromaBaseOffset);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LUMOFFSET, val->inputLumaBaseOffset);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_ROWLENGTH, val->pixelsOnRow);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_XFILL, val->xFill);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_YFILL, val->yFill);


  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALE_MODE, val->scaledWidth > 0 ? 2 : 0);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BASESCALEDOUTLUM, val->scaledLumBase);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUTWIDTH, val->scaledWidth);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUTWIDTHRATIO, val->scaledWidthRatio);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUTHEIGHT, val->scaledHeight);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDOUTHEIGHTRATIO, val->scaledHeightRatio);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDSKIPLEFTPIXELCOLUMN, val->scaledSkipLeftPixelColumn);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDSKIPTOPPIXELROW, val->scaledSkipTopPixelRow);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_VSCALE_WEIGHT_EN, val->scaledVertivalWeightEn);
  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDHORIZONTALCOPY, val->scaledHorizontalCopy);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SCALEDVERTICALCOPY, val->scaledVerticalCopy);
  

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CHROMA_SWAP, val->chromaSwap);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_COMPRESSEDCOEFF_BASE, val->compress_coeff_scan_base);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CABAC_INIT_FLAG, val->cabac_init_flag);

  //compress
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_LUMA_COMPRESSOR_ENABLE, val->recon_luma_compress);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_CHROMA_COMPRESSOR_ENABLE, val->recon_chroma_compress);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF0_LUMA_COMPRESSOR_ENABLE, val->ref_l0_luma_compressed[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF0_CHROMA_COMPRESSOR_ENABLE, val->ref_l0_chroma_compressed[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF1_LUMA_COMPRESSOR_ENABLE, val->ref_l0_luma_compressed[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF1_CHROMA_COMPRESSOR_ENABLE, val->ref_l0_chroma_compressed[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_REF0_LUMA_COMPRESSOR_ENABLE, val->ref_l1_luma_compressed[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_REF0_CHROMA_COMPRESSOR_ENABLE, val->ref_l1_chroma_compressed[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_REF1_LUMA_COMPRESSOR_ENABLE, val->ref_l1_luma_compressed[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_REF1_CHROMA_COMPRESSOR_ENABLE, val->ref_l1_chroma_compressed[1]);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_LUMA_COMPRESS_TABLE_BASE, val->recon_luma_compress_tbl_base);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_CHROMA_COMPRESS_TABLE_BASE, val->recon_chroma_compress_tbl_base);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF0_LUMA_COMPRESS_TABLE_BASE, val->ref_l0_luma_compress_tbl_base[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF0_CHROMA_COMPRESS_TABLE_BASE, val->ref_l0_chroma_compress_tbl_base[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF1_LUMA_COMPRESS_TABLE_BASE, val->ref_l0_luma_compress_tbl_base[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L0_REF1_CHROMA_COMPRESS_TABLE_BASE,  val->ref_l0_chroma_compress_tbl_base[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_REF0_LUMA_COMPRESS_TABLE_BASE, val->ref_l1_luma_compress_tbl_base[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_REF0_CHROMA_COMPRESS_TABLE_BASE, val->ref_l1_chroma_compress_tbl_base[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_REF1_LUMA_COMPRESS_TABLE_BASE, val->ref_l1_luma_compress_tbl_base[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_L1_REF1_CHROMA_COMPRESS_TABLE_BASE,  val->ref_l1_chroma_compress_tbl_base[1]);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_RECON_LUMA_4N_BASE,  val->reconL4nBase);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_4N0_BASE,  val->pRefPic_recon_l0_4n[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L0_4N1_BASE,  val->pRefPic_recon_l0_4n[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L1_4N0_BASE,  val->pRefPic_recon_l1_4n[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REFPIC_RECON_L1_4N1_BASE,  val->pRefPic_recon_l1_4n[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_TIMEOUT_INT, ENCH2_TIMEOUT_INTERRUPT & 1);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SLICE_INT, val->sliceReadyInterrupt);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_TARGETPICSIZE, val->targetPicSize);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MINPICSIZE, val->minPicSize);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAXPICSIZE, val->maxPicSize);

  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CTBRCBITMEMADDR_CURRENT, val->ctbRcBitMemAddrCur);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CTBRCBITMEMADDR_PREVIOUS, val->ctbRcBitMemAddrPre);
  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CTBRCTHRDMIN, val->ctbRcThrdMin);
  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CTBRCTHRDMAX, val->ctbRcThrdMax);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CTBBITSMIN, val->ctbBitsMin);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_CTBBITSMAX, val->ctbBitsMax);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_TOTALLCUBITS, val->totalLcuBits);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_BITSRATIO, val->bitsRatio);

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_MAX_BURST, ENCH2_AXI40_BURST_LENGTH);
  EncAsicSetRegisterValue(val->regMirror, HWIF_TIMEOUT_OVERRIDE_E, ENCH2_ASIC_TIMEOUT_OVERRIDE_ENABLE);
  EncAsicSetRegisterValue(val->regMirror, HWIF_TIMEOUT_CYCLES, ENCH2_ASIC_TIMEOUT_CYCLES);

  //reference pic lists modification
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LISTS_MODI_PRESENT_FLAG, val->lists_modification_present_flag);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REF_PIC_LIST_MODI_FLAG_L0, val->ref_pic_list_modification_flag_l0);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LIST_ENTRY_L0_PIC0, val->list_entry_l0[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LIST_ENTRY_L0_PIC1, val->list_entry_l0[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_REF_PIC_LIST_MODI_FLAG_L1, val->ref_pic_list_modification_flag_l1);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LIST_ENTRY_L1_PIC0, val->list_entry_l1[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LIST_ENTRY_L1_PIC1, val->list_entry_l1[1]);




  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_10, val->lambda_sse_me[10]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_11, val->lambda_sse_me[11]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_12, val->lambda_sse_me[12]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_13, val->lambda_sse_me[13]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_14, val->lambda_sse_me[14]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_LAMDA_SSE_ME_15, val->lambda_sse_me[15]);

  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_0, val->lambda_satd_ims[0]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_1, val->lambda_satd_ims[1]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_2, val->lambda_satd_ims[2]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_3, val->lambda_satd_ims[3]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_4, val->lambda_satd_ims[4]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_5, val->lambda_satd_ims[5]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_6, val->lambda_satd_ims[6]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_7, val->lambda_satd_ims[7]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_8, val->lambda_satd_ims[8]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_9, val->lambda_satd_ims[9]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_10, val->lambda_satd_ims[10]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_11, val->lambda_satd_ims[11]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_12, val->lambda_satd_ims[12]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_13, val->lambda_satd_ims[13]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_14, val->lambda_satd_ims[14]);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_INTRA_SATD_LAMDA_15, val->lambda_satd_ims[15]);
  
  //for noise reduction

  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NOISE_REDUCTION_ENABLE, val->noiseReductionEnable);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NOISE_LOW, val->noiseLow);
  //EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_FIRST_FRAME_SIGMA, val->firstFrameSigma);
  #ifdef ME_MEMSHARE_CHANGE
  val->nrMbNumInvert = (1<<MB_NUM_BIT_WIDTH)/(((((val->picWidth + 31) >> 5) << 5) * (((val->picHeight + 31) >> 5) << 5))/(16*16));
  #else  
  val->nrMbNumInvert = (1<<MB_NUM_BIT_WIDTH)/(((((val->picWidth + 63) >> 6) << 6) * (((val->picHeight + 63) >> 6) << 6))/(16*16));
  #endif
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_NR_MBNUM_INVERT_REG, val->nrMbNumInvert);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SLICEQP_PREV, val->nrSliceQPPrev& mask_6b);
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_THRESH_SIGMA_CUR, val->nrThreshSigmaCur& mask_21b);
  
  EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_SIGMA_CUR, val->nrSigmaCur& mask_16b);
  //EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_FRAME_SIGMA_CALCED, val->nrThreshSigmaCalced);
  //EncAsicSetRegisterValue(val->regMirror, HWIF_ENC_THRESH_SIGMA_CALCED, val->nrFrameSigmaCalced);

  val->regMirror[80]= EWLReadReg(ewl,80*4);
  {
    for (i = 1; i < ASIC_SWREG_AMOUNT; i++)
      EWLWriteReg(ewl, HSWREG(i), val->regMirror[i]);
  }

#ifdef TRACE_REGS
  EncTraceRegs(ewl, 0, 0);
#endif
  /* Register with enable bit is written last */
  val->regMirror[ASIC_REG_INDEX_STATUS] |= ASIC_STATUS_ENABLE;

  EWLEnableHW(ewl, HSWREG(ASIC_REG_INDEX_STATUS), val->regMirror[ASIC_REG_INDEX_STATUS]);
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
  // EWLWriteReg(ewl, HSWREG(24), (val->rlcLimitSpace / 2));

  //val->regMirror[5] = val->rlcBase;
  EWLWriteReg(ewl, HSWREG(5), val->regMirror[5]);

#ifdef TRACE_REGS
  EncTraceRegs(ewl, 0, EncAsicGetRegisterValue(ewl, val->regMirror, HWIF_ENC_ENCODED_CTB_NUMBER));
#endif

  /* Register with status bits is written last */
  EWLEnableHW(ewl, HSWREG(4), val->regMirror[4]);

}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void EncAsicGetRegisters(const void *ewl, regValues_s *val)
{
  (void)ewl;
  (void)val;

  /* HW output stream size, bits to bytes */
  int i;


  for (i = 1; i < ASIC_SWREG_AMOUNT; i++)
  {
    val->regMirror[i] = EWLReadReg(ewl, HSWREG(i));
  }

#ifdef TRACE_REGS
  EncTraceRegs(ewl, 1, EncAsicGetRegisterValue(ewl, val->regMirror, HWIF_ENC_ENCODED_CTB_NUMBER));
#endif
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void EncAsicStop(const void *ewl)
{
  EWLDisableHW(ewl, HSWREG(4), 0);
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
u32 EncAsicGetPerformance(const void *ewl)
{
  return EWLReadReg(ewl, HSWREG(82));
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
u32 EncAsicGetStatus(const void *ewl)
{
  return EWLReadReg(ewl, HSWREG(1));
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

void EncAsicClearStatus(const void *ewl,u32 value)
{
  EWLWriteReg(ewl, HSWREG(1),value);
}

/*------------------------------------------------------------------------------
    EncAsicGetMvOutput
        Return encOutputMbInfo_s pointer to beginning of macroblock mbNum
------------------------------------------------------------------------------*/
u32 *EncAsicGetMvOutput(asicData_s *asic, u32 mbNum)
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
