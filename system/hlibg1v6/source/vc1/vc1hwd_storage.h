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
--  Description : Storage handling functionality
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_storage.h,v $
--  $Revision: 1.17 $
--  $Date: 2010/10/26 07:20:02 $
--
------------------------------------------------------------------------------*/

#ifndef VC1HWD_STORAGE_H
#define VC1HWD_STORAGE_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1hwd_picture_layer.h"
#include "vc1hwd_picture.h"
#include "vc1decapi.h"
#include "bqueue.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define     MB_SKIPPED          4
#define     MB_4MV              2
#define     MB_AC_PRED          4
#define     MB_DIRECT           2
#define     MB_FIELD_TX         1
#define     MB_OVERLAP_SMOOTH   2
#define     MB_FORWARD_MB       4

#define     MAX_OUTPUT_PICS    (16)

typedef enum
{
    /* Headers */
    HDR_SEQ = 1,
    HDR_ENTRY = 2,
    /* Combinations */
    HDR_BOTH = HDR_SEQ | HDR_ENTRY
} hdr_e;

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

typedef struct vc1DecPpBuffer_s
{
    u32 dec;
    u32 pp;
    u32 processed;
    u32 framePic;
    u32 bFrame;
} vc1DecPpBuffer_t;

typedef struct vc1DecPp_s
{
    vc1DecPpBuffer_t decOut;
    vc1DecPpBuffer_t ppProc;
    vc1DecPpBuffer_t anchor;
    vc1DecPpBuffer_t ppOut;

    u32         anchorPpIdx;

    u32 fieldOutput;
    bufferQueue_t *bqPp;
} vc1DecPp_t;

typedef struct swStrmStorage
{
    u16x maxCodedWidth;         /* Maximum horizontal size in pixels */
    u16x maxCodedHeight;        /* Maximum vertical size in pixels */
    u16x curCodedWidth;
    u16x curCodedHeight;

    u16x picWidthInMbs;
    u16x picHeightInMbs;
    u16x numOfMbs;

    u16x loopFilter;            /* Is loop filter used */
    u16x multiRes;              /* Can resolution change */
    u16x fastUvMc;              /* Fast motion compensation */
    u16x extendedMv;            /* Extended motion vectors used */
    u16x maxBframes;            /* Maximum number of B frames [0,7] */
    u16x dquant;                /* Can quantization step vary */
    u16x rangeRed;              /* Is range reduction used */

    u16x vsTransform;           /* Variable sized transform */
    u16x overlap;               /* Overlap smooting enabled */
    u16x syncMarker;            /* Sync markers present */
    u16x frameInterpFlag;       /* Is frame interpolation hints present */
    u16x quantizer;             /* Quantizer used for the sequence */

    const picture_t* pPicBuf;    /* Picture descriptors for required
                                  * reference frames and work buffers */
    
    u16x outpBuf[MAX_OUTPUT_PICS];
    u16x outPicId[2][MAX_OUTPUT_PICS];
    u16x outErrMbs[MAX_OUTPUT_PICS];
    u32 fieldToReturn;          /* 0 = First, 1 second */
    u16x outpIdx;
    u16x prevOutpIdx;
    u16x outpCount;
    u32 minCount;               /* for vc1hwdNextPicture */
    u32 fieldCount;

    u32 maxNumBuffers;
    u32 numPpBuffers;
    u16x workBufAmount;         /* Amount of work buffers */
    u16x workOut;               /* Index for output */
    u16x workOutPrev;;          /* Index for previous output */
    u16x work0;                 /* Index for last anchor frame */
    u16x work1;                 /* Index for previous to last anchor frame */
    u16x prevBIdx;

    pictureLayer_t picLayer;

    u16x rnd;

    u8 *pMbFlags;

    /* Sequence layer */
    u32 profile;
    u32 level;
    u32 colorDiffFormat;        /* color-difference/luma format (1 = 4:2:0) */
    u32 frmrtqPostProc;
    u32 bitrtqPostProc;
    u32 postProcFlag;
    u32 pullDown;
    u32 interlace;
    u32 tfcntrFlag;
    u32 finterpFlag;
    u32 psf;
    u32 dispHorizSize;
    u32 dispVertSize;
    u32 aspectHorizSize;
    u32 aspectVertSize;

    u32 frameRateFlag;
    u32 frameRateInd;
    u32 frameRateNr;
    u32 frameRateDr;

    u32 colorFormatFlag;
    u32 colorPrim;
    u32 transferChar;
    u32 matrixCoef;
    
    u32 hrdParamFlag;
    u32 hrdNumLeakyBuckets;
    u32 bitRateExponent;
    u32 bufferSizeExponent;
    u32* hrdRate;
    u32* hrdBuffer;

    /* entry-point */
    u32 brokenLink;
    u32 closedEntry;
    u32 panScanFlag;
    u32 refDistFlag;
    u32* hrdFullness;
    u32 extendedDmv;

    u32 rangeMapYFlag;
    u32 rangeMapY;
    u32 rangeMapUvFlag;
    u32 rangeMapUv;

    u32 anchorInter[2]; /* [0] top field / frame, [1] bottom field */

    u32 skipB;
    u32 resolutionChanged;
    strmData_t tmpStrmData;
    u32 prevDecResult;
    u32 slice;
    u32 firstFrame;
    u32 ffStart;        /* picture layer of the first field processed */
    u32 missingField;
    
    hdr_e hdrsDecoded;  /* Contains info of decoded headers */
    u32 pictureBroken;
    u32 intraFreeze;
    u32 previousB;
    u32 previousModeFull;

    u32 keepHwReserved;

    bufferQueue_t bq;
    bufferQueue_t bqPp;

    u32 parallelMode2;
    u32 pm2lockBuf;
    u32 pm2AllProcessedFlag;
    u32 pm2StartPoint;
    vc1DecPp_t decPp;

} swStrmStorage_t;

/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

#endif /* #ifndef VC1HWD_STORAGE_H */

