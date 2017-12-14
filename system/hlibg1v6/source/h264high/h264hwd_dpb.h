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
--  Abstract : Decoded Picture Buffer (DPB) handling
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_dpb.h,v $
--  $Date: 2010/12/01 12:31:03 $
--  $Revision: 1.14 $
--
------------------------------------------------------------------------------*/

#ifndef H264HWD_DPB_H
#define H264HWD_DPB_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"

#include "h264hwd_slice_header.h"
#include "h264hwd_image.h"

#include "dwl.h"

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

/* enumeration to represent status of buffered image */
typedef enum
{
    UNUSED = 0,
    NON_EXISTING,
    SHORT_TERM,
    LONG_TERM,
    EMPTY
} dpbPictureStatus_e;

/* structure to represent a buffered picture */
typedef struct dpbPicture
{
    u32 memIdx;
    DWLLinearMem_t *data;
    i32 picNum;
    u32 frameNum;
    i32 picOrderCnt[2];
    dpbPictureStatus_e status[2];
    u32 toBeDisplayed;
    u32 picId;
    u32 numErrMbs;
    u32 isIdr;
    u32 isFieldPic;
    u32 tiledMode;
} dpbPicture_t;

/* structure to represent display image output from the buffer */
typedef struct
{
    u32 memIdx;
    DWLLinearMem_t *data;
    u32 picId;
    u32 numErrMbs;
    u32 isIdr;
    u32 interlaced;
    u32 fieldPicture;
    u32 topField;
    u32 tiledMode;
} dpbOutPicture_t;

/* structure to represent DPB */
typedef struct dpbStorage
{
    dpbPicture_t buffer[16 + 1];
    u32 list[16 + 1];
    dpbPicture_t *currentOut;
    u32 currentOutPos;
    dpbOutPicture_t *outBuf;
    u32 numOut;
    u32 outIndexW;
    u32 outIndexR;
    u32 maxRefFrames;
    u32 dpbSize;
    u32 maxFrameNum;
    u32 maxLongTermFrameIdx;
    u32 numRefFrames;
    u32 fullness;
    u32 prevRefFrameNum;
    u32 lastContainsMmco5;
    u32 noReordering;
    u32 flushed;
    u32 picSizeInMbs;
    u32 dirMvOffset;
    DWLLinearMem_t poc;
    u32 delayedOut;
    u32 delayedId;
    u32 interlaced;
    u32 ch2Offset;
    
    u32 numFreeBuffers;
    u32 freeBuffers[16+1];

    u32 memStat[16+1+16+1];
    u32 totBuffers;
    DWLLinearMem_t picBuffers[16+1+16+1];

    /* flag to prevent output when display smoothing is used and second field
     * of a picture was just decoded */
    u32 noOutput;

    u32 prevOutIdx;
} dpbStorage_t;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

u32 h264bsdInitDpb(const void *dwl,
                   dpbStorage_t * dpb,
                   u32 picSizeInMbs,
                   u32 dpbSize,
                   u32 numRefFrames, u32 maxFrameNum, u32 noReordering,
                   u32 displaySmoothing,
                   u32 monoChrome, u32 isHighSupported, u32 enable2ndChroma,
                   u32 multiBuffPp);

u32 h264bsdResetDpb(const void *dwl,
                    dpbStorage_t * dpb,
                    u32 picSizeInMbs,
                    u32 dpbSize,
                    u32 numRefFrames, u32 maxFrameNum, u32 noReordering,
                    u32 displaySmoothing,
                    u32 monoChrome, u32 isHighSupported, u32 enable2ndChroma,
                    u32 multiBuffPp);

void h264bsdInitRefPicList(dpbStorage_t * dpb);

void *h264bsdAllocateDpbImage(dpbStorage_t * dpb);

i32 h264bsdGetRefPicData(const dpbStorage_t * dpb, u32 index);
u8 *h264bsdGetRefPicDataVlcMode(const dpbStorage_t * dpb, u32 index,
    u32 fieldMode);

u32 h264bsdReorderRefPicList(dpbStorage_t * dpb,
                             refPicListReordering_t * order,
                             u32 currFrameNum, u32 numRefIdxActive);

u32 h264bsdMarkDecRefPic(dpbStorage_t * dpb,
                         /*@null@ */ const decRefPicMarking_t * mark,
                         const image_t * image, u32 frameNum, i32 *picOrderCnt,
                         u32 isIdr, u32 picId, u32 numErrMbs, u32 tiledMode );

u32 h264bsdCheckGapsInFrameNum(dpbStorage_t * dpb, u32 frameNum, u32 isRefPic,
    u32 gapsAllowed);

/*@null@*/ dpbOutPicture_t *h264bsdDpbOutputPicture(dpbStorage_t * dpb);

void h264bsdFlushDpb(dpbStorage_t * dpb);

void h264bsdFreeDpb(const void *dwl, dpbStorage_t * dpb);

void ShellSort(dpbStorage_t * dpb, u32 *list, u32 type, i32 par);
void ShellSortF(dpbStorage_t * dpb, u32 *list, u32 type, /*u32 parity,*/ i32 par);

void SetPicNums(dpbStorage_t * dpb, u32 currFrameNum);

void h264DpbUpdateOutputList(dpbStorage_t * dpb, const image_t * image);
void h264DpbAdjStereoOutput(dpbStorage_t * dpb, u32 targetCount);

#endif /* #ifdef H264HWD_DPB_H */
