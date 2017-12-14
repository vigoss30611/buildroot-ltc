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

#ifndef INSTANCE_H
#define INSTANCE_H

#include "container.h"
#include "enccommon.h"
#include "encpreprocess.h"
#include "encasiccontroller.h"

#include "hevcencapi.h"     /* Callback type from API is reused */
#include "rate_control_picture.h"


struct hevc_instance
{
  u32 encStatus;
  asicData_s asic;
  i32 vps_id;     /* Video parameter set id */
  i32 sps_id;     /* Sequence parameter set id */
  i32 pps_id;     /* Picture parameter set id */
  i32 rps_id;     /* Reference picture set id */

  struct buffer stream;


  u8 *temp_buffer;      /* Data buffer, user set */
  u32 temp_size;      /* Size (bytes) of buffer */
  u32 temp_bufferBusAddress;

  // SPS&PPS parameters
  i32 max_cu_size;    /* Max coding unit size in pixels */
  i32 min_cu_size;    /* Min coding unit size in pixels */
  i32 max_tr_size;    /* Max transform size in pixels */
  i32 min_tr_size;    /* Min transform size in pixels */
  i32 tr_depth_intra;   /* Max transform hierarchy depth */
  i32 tr_depth_inter;   /* Max transform hierarchy depth */
  i32 width;
  i32 height;
  i32 enableScalingList;  /* */

  // intra setup
  u32 strong_intra_smoothing_enabled_flag;
  u32 constrained_intra_pred_flag;
  i32 enableDeblockOverride;
  i32 disableDeblocking;

  i32 tc_Offset;

  i32 beta_Offset;

  i32 ctbPerFrame;
  i32 ctbPerRow;
  i32 ctbPerCol;

  /* Minimum luma coding block size of coding units that convey
   * cu_qp_delta_abs and cu_qp_delta_sign and default quantization
   * parameter */
  i32 min_qp_size;
  i32 qpHdr;

  i32 levelIdx;   /*level 5.1 =8*/
  i32 level;   /*level 5.1 =8*/
  
  i32 profile;   /**/

  preProcess_s preProcess;

  /* Rate control parameters */
  hevcRateControl_s rateControl;

  
  struct vps *vps;
  struct sps *sps;

  i32 poc;      /* encoded Picture order count */
  u8 *lum;
  u8 *cb;
  u8 *cr;
  i32 chromaQpOffset;
  i32 enableSao;
  i32 output_buffer_over_flow;
  const void *inst;

  HEVCEncSliceReadyCallBackFunc sliceReadyCbFunc;
  void *pAppData;         /* User given application specific data */
  u32 frameCnt;
  u32 videoFullRange;
  u32 sarWidth;
  u32 sarHeight;
  i32 fieldOrder;     /* 1=top first, 0=bottom first */
  u32 interlaced;

  i32 gdrEnabled;
  i32 gdrStart;
  i32 gdrDuration;
  i32 gdrCount;
  i32 gdrAverageMBRows;
  i32 gdrMBLeft;
  i32 gdrFirstIntraFrame;
  u32 roi1Enable;
  u32 roi2Enable;
  u32 ctbRCEnable;
  i32 blockRCSize;
  u32 roiMapEnable;
  u32 numNalus;        /* Amount of NAL units */
  u32 testId;
  i32 created_pic_num; /* number of pictures currently created */
#if USE_TOP_CTRL_DENOISE
    unsigned int uiFrmNum;
    unsigned int uiNoiseReductionEnable;
    int FrmNoiseSigmaSmooth[5];
    int iFirstFrameSigma;
    int iNoiseL;
    int iSigmaCur;
    int iThreshSigmaCur;
    int iThreshSigmaPrev;
    int iSigmaCalcd;
    int iThreshSigmaCalcd;
    int iSliceQPPrev;
#endif

    u32 maxTLayers; /*max temporal layers*/
};

struct instance
{
  struct hevc_instance hevc_instance; /* Visible to user */
  struct hevc_instance *instance;   /* Instance sanity check */
  struct container container;   /* Encoder internal store */
};

struct container *get_container(struct hevc_instance *instance);

#endif
