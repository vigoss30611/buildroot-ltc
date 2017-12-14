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
--  Abstract : HEVC Encoder testbench for linux
--
------------------------------------------------------------------------------*/
#ifndef _HEVCTESTBENCH_H_
#define _HEVCTESTBENCH_H_

#include "base_type.h"

/*--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

#define DEFAULT -255
#define MAX_BPS_ADJUST 20
#define MAX_STREAMS 16

/* Structure for command line options */
typedef struct commandLine_s
{
  char *input;
  char *output;
  char *recon;

  char *test_data_files;
  i32 outputRateNumer;      /* Output frame rate numerator */
  i32 outputRateDenom;      /* Output frame rate denominator */
  i32 inputRateNumer;      /* Input frame rate numerator */
  i32 inputRateDenom;      /* Input frame rate denominator */
  i32 firstPic;
  i32 lastPic;

  i32 width;
  i32 height;
  i32 lumWidthSrc;
  i32 lumHeightSrc;

  i32 inputFormat;


  i32 picture_cnt;
  i32 byteStream;


  i32 max_cu_size;    /* Max coding unit size in pixels */
  i32 min_cu_size;    /* Min coding unit size in pixels */
  i32 max_tr_size;    /* Max transform size in pixels */
  i32 min_tr_size;    /* Min transform size in pixels */
  i32 tr_depth_intra;   /* Max transform hierarchy depth */
  i32 tr_depth_inter;   /* Max transform hierarchy depth */

  i32 min_qp_size;

  i32 cabacInitFlag;

  // intra setup
  u32 strong_intra_smoothing_enabled_flag;

  i32 cirStart;
  i32 cirInterval;

  i32 intraAreaEnable;
  i32 intraAreaTop;
  i32 intraAreaLeft;
  i32 intraAreaBottom;
  i32 intraAreaRight;

  i32 roi1AreaEnable;
  i32 roi2AreaEnable;

  i32 roi1AreaTop;
  i32 roi1AreaLeft;
  i32 roi1AreaBottom;
  i32 roi1AreaRight;

  i32 roi2AreaTop;
  i32 roi2AreaLeft;
  i32 roi2AreaBottom;
  i32 roi2AreaRight;

  i32 roi1DeltaQp;
  i32 roi2DeltaQp;


  /* Rate control parameters */
  i32 hrdConformance;
  i32 cpbSize;
  i32 intraPicRate;   /* IDR interval */

  i32 qpHdr;
  i32 qpMin;
  i32 qpMax;
  i32 bitPerSecond;
  
  i32 bitVarRangeI;

  i32 bitVarRangeP;

  i32 bitVarRangeB;

  i32 tolMovingBitRate;/*tolerance of max Moving bit rate */
  i32 monitorFrames;/*monitor frame length for moving bit rate*/
  i32 picRc;
  i32 ctbRc;//CTB_RC
  i32 blockRCSize;
  i32 picSkip;

  i32 gopLength;
  i32 intraQpDelta;
  i32 fixedIntraQp;
  i32 bFrameQpDelta;

  i32 disableDeblocking;

  i32 enableSao;


  i32 tc_Offset;
  i32 beta_Offset;


  i32 chromaQpOffset;

  i32 profile;              /*main profile or main still picture profile*/
  i32 level;              /*main profile level*/

  i32 bpsAdjustFrame[MAX_BPS_ADJUST];
  i32 bpsAdjustBitrate[MAX_BPS_ADJUST];

  i32 sliceSize;

  i32 testId;

  i32 rotation;
  i32 horOffsetSrc;
  i32 verOffsetSrc;
  i32 colorConversion;
  i32 scaledWidth;
  i32 scaledHeight;

  i32 enableDeblockOverride;
  i32 deblockOverride;

  i32 enableScalingList;

  u32 compressor;

  i32 interlacedFrame;
  i32 fieldOrder;
  i32 videoRange;

  i32 sei;
  char *userData;
  u32 gopSize;
  char *gopCfg;
  u32 gopLowdelay;
  i32 outReconFrame;
  
  i32 gdrDuration;
  u32 roiMapDeltaQpBlockUnit;
  u32 roiMapDeltaQpEnable;
  char *roiMapDeltaQpFile;
  i32 outBufSizeMax;
  i32 multimode;  // Multi-stream mode, 0--disable, 1--mult-thread, 2--multi-process
  char *streamcfg[MAX_STREAMS];
  union {
	  int pid[MAX_STREAMS];
	  pthread_t tid[MAX_STREAMS];
  };
  struct commandLine_s *streamcml[MAX_STREAMS];
  i32 nstream;

  char **argv;      /* Command line parameter... */
  i32 argc;
  //WIENER_DENOISE
  i32 noiseReductionEnable;
  i32 noiseLow;
  i32 firstFrameSigma;

  i32 bitDepthLuma;
  i32 bitDepthChroma;

  
} commandLine_s;


#endif
