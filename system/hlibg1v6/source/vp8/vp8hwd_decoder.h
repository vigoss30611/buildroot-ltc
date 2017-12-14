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
-  $RCSfile: vp8hwd_decoder.h,v $
-  $Revision: 1.9 $
-  $Date: 2010/10/20 07:29:05 $
-
------------------------------------------------------------------------------*/

#ifndef __VP8_DECODER_H__
#define __VP8_DECODER_H__

#include "basetype.h"

#define VP8HWD_VP7             1U
#define VP8HWD_VP8             2U

#define DEC_8190_ALIGN_MASK         0x07U
#define DEC_8190_MODE_VP8           0x09U

#define VP8HWDEC_HW_RESERVED         0x0100
#define VP8HWDEC_SYSTEM_ERROR        0x0200
#define VP8HWDEC_SYSTEM_TIMEOUT      0x0300


#define MAX_NBR_OF_DCT_PARTITIONS       (8)

#define MAX_NBR_OF_SEGMENTS             (4)
#define MAX_NBR_OF_MB_REF_LF_DELTAS     (4)
#define MAX_NBR_OF_MB_MODE_LF_DELTAS    (4)

#define MAX_NBR_OF_VP7_MB_FEATURES      (4)

#define MAX_SNAPSHOT_WIDTH  1024 
#define MAX_SNAPSHOT_HEIGHT 1024

#define VP7_MV_PROBS_PER_COMPONENT      (17)
#define VP8_MV_PROBS_PER_COMPONENT      (19)

enum
{
    HANTRO_NOK =  1,
    HANTRO_OK   = 0
};

#ifndef HANTRO_FALSE
#define HANTRO_FALSE    (0)
#endif 

#ifndef HANTRO_TRUE
#define HANTRO_TRUE     (1)
#endif 

enum {
    VP8_SEG_FEATURE_DELTA,
    VP8_SEG_FEATURE_ABS
};

typedef enum 
{
    VP8_YCbCr_BT601,
    VP8_CUSTOM
} vpColorSpace_e;

typedef struct vp8EntropyProbs_s
{
    u8              probLuma16x16PredMode[4];
    u8              probChromaPredMode[3];
    u8              probMvContext[2][VP8_MV_PROBS_PER_COMPONENT];
    u8              probCoeffs[4][8][3][11];
} vp8EntropyProbs_t;

typedef struct vp8Decoder_s
{
    u32             decMode;

    /* Current frame dimensions */
    u32             width;
    u32             height;
    u32             scaledWidth;
    u32             scaledHeight;   

    u32             vpVersion;
    u32             vpProfile;

    u32             keyFrame;

    u32             coeffSkipMode;

    /* DCT coefficient partitions */
    u32             offsetToDctParts;
    u32             nbrDctPartitions;
    u32             dctPartitionOffsets[MAX_NBR_OF_DCT_PARTITIONS];

    vpColorSpace_e  colorSpace; 
    u32             clamping;   
    u32             showFrame;  


    u32             refreshGolden; 
    u32             refreshAlternate; 
    u32             refreshLast;
    u32             refreshEntropyProbs;
    u32             copyBufferToGolden;
    u32             copyBufferToAlternate;

    u32             refFrameSignBias[2];
    u32             useAsReference;
    u32             loopFilterType;
    u32             loopFilterLevel;
    u32             loopFilterSharpness;

    /* Quantization parameters */
    i32             qpYAc, qpYDc, qpY2Ac, qpY2Dc, qpChAc, qpChDc;

    /* From here down, frame-to-frame persisting stuff */

    u32             vp7ScanOrder[16];
    u32             vp7PrevScanOrder[16];
    
    /* Probabilities */
    u32             probIntra;
    u32             probRefLast;
    u32             probRefGolden;
    u32             probMbSkipFalse;
    u32             probSegment[3];
    vp8EntropyProbs_t entropy,
                      entropyLast;

    /* Segment and macroblock specific values */
    u32             segmentationEnabled;
    u32             segmentationMapUpdate;
    u32             segmentFeatureMode; /* delta/abs */
    i32             segmentQp[MAX_NBR_OF_SEGMENTS];
    i32             segmentLoopfilter[MAX_NBR_OF_SEGMENTS];
    u32             modeRefLfEnabled;
    i32             mbRefLfDelta[MAX_NBR_OF_MB_REF_LF_DELTAS];
    i32             mbModeLfDelta[MAX_NBR_OF_MB_MODE_LF_DELTAS];

    u32             frameTagSize;

    /* Value to remember last frames prediction for hits into most
     * probable reference frame */
    u32             refbuPredHits;

} 
vp8Decoder_t;

struct DecAsicBuffers;

void vp8hwdResetDecoder( vp8Decoder_t * dec, struct DecAsicBuffers *asicBuff );
void vp8hwdPrepareVp7Scan( vp8Decoder_t * dec, u32 * newOrder );

#endif /* __VP8_BOOL_H__ */
