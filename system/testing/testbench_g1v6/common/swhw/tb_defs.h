/*------------------------------------------------------------------------------
--                                                                            --
--           This confidential and proprietary software may be used           --
--              only as authorized by a licensing agreement from              --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--      In the event of publication, the following notice is applicable:      --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--    The entire notice above must be reproduced on all authorized copies.    --
--                                                                            --
------------------------------------------------------------------------------*/
#ifndef TB_DEFS_H
#define TB_DEFS_H

#ifdef _ASSERT_USED
#include <assert.h>
#endif

#include <stdio.h>

#include "basetype.h"

/*------------------------------------------------------------------------------
    Generic data type stuff
------------------------------------------------------------------------------*/


typedef enum {
    TB_FALSE = 0,
    TB_TRUE  = 1
} TBBool;

/*------------------------------------------------------------------------------
    Defines
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
    Test bench configuration    u32 strideEnable;

------------------------------------------------------------------------------*/
typedef struct {
    char packetByPacket[9];
    char nalUnitStream[9];
    u32 seedRnd;
    char streamBitSwap[24];
    char streamBitLoss[24];
    char streamPacketLoss[24];
    char streamHeaderCorrupt[9];
    char streamTruncate[9];
    char sliceUdInPacket[9];
    u32 firstTraceFrame;

    u32 memoryPageSize;
    i32 refFrmBufferSize;

} TBParams;

typedef struct {
    char outputPictureEndian[14];
    u32 busBurstLength;
    u32 asicServicePriority;
    char outputFormat[12];
    u32 latencyCompensation;
    char clockGating[9];
    char dataDiscard[9];

    char memoryAllocation[9];
    char rlcModeForced[9];
    char errorConcealment[15];

    u32 jpegMcusSlice;
    u32 jpegInputBufferSize;

    u32 refbuEnable;
    u32 refbuDisableInterlaced;
    u32 refbuDisableDouble;
    u32 refbuDisableEvalMode;
    u32 refbuDisableCheckpoint;
    u32 refbuDisableOffset;
    u32 refbuDataExcessMaxPct;
    u32 refbuDisableTopBotSum;

    u32 mpeg2Support;
    u32 vc1Support;
    u32 jpegSupport;
    u32 mpeg4Support;
    u32 customMpeg4Support;
    u32 h264Support;
    u32 vp6Support;
    u32 vp7Support;
    u32 vp8Support;
    u32 pJpegSupport;
    u32 sorensonSupport;
    u32 avsSupport;
    u32 rvSupport;
    u32 mvcSupport;
    u32 webpSupport;
    u32 ecSupport;
    u32 maxDecPicWidth;
    u32 hwVersion;
    u32 hwBuild;
    u32 busWidth64bitEnable;
    u32 latency;
    u32 nonSeqClk;
    u32 seqClk;
    u32 supportNonCompliant;    
    u32 jpegESupport;

    u32 forceMpeg4Idct;
    u32 ch8PixIleavOutput;

    u32 refBufferTestModeOffsetEnable;
    i32 refBufferTestModeOffsetMin;
    i32 refBufferTestModeOffsetMax;
    i32 refBufferTestModeOffsetStart;
    i32 refBufferTestModeOffsetIncr;

    u32 apfThresholdDisable;
    i32 apfThresholdValue;

    u32 tiledRefSupport;
    u32 strideSupport;
    i32 fieldDpbSupport;

    u32 serviceMergeDisable;

} TBDecParams;

typedef struct {
    char outputPictureEndian[14];
    char inputPictureEndian[14];
    char wordSwap[9];
    char wordSwap16[9];
    u32 busBurstLength;
    char clockGating[9];
    char dataDiscard[9];
    char multiBuffer[9];

    u32 maxPpOutPicWidth;
    u32 ppdExists;
    u32 ditheringSupport;
    u32 scalingSupport;
    u32 deinterlacingSupport;
    u32 alphaBlendingSupport;
    u32 ablendCropSupport;
    u32 ppOutEndianSupport;
    u32 tiledSupport;
    u32 tiledRefSupport;

    i32 fastHorDownScaleDisable;
    i32 fastVerDownScaleDisable;
    i32 vertDownScaleStripeDisableSupport;
    u32 pixAccOutSupport;

} TBPpParams;

typedef struct {
    TBParams tbParams;
    TBDecParams decParams;
    TBPpParams ppParams;
} TBCfg;


#endif /* TB_DEFS_H */
