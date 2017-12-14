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
--  Abstract : Stream decoding storage definition
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: avs_storage.h,v $
--  $Date: 2010/12/01 12:30:58 $
--  $Revision: 1.9 $
--
------------------------------------------------------------------------------*/

#ifndef AVSDECSTRMSTORAGE_H_DEFINED
#define AVSDECSTRMSTORAGE_H_DEFINED

#include "basetype.h"
#include "dwl.h"
#include "avs_cfg.h"
#include "avsdecapi.h"
#include "bqueue.h"

typedef struct
{
    DWLLinearMem_t data;
    u32 picType;
    u32 picId;
    u32 tf;
    u32 ff[2];
    u32 rff;
    u32 rfc;
    u32 isInter;
    u32 nbrErrMbs;
    u32 pictureDistance;
    AvsDecRet retVal;
    u32 sendToPp;
    AvsDecTime timeCode;

    u32 tiledMode;
} picture_t;

typedef struct
{
    u32 status;
    u32 strmDecReady;

    u32 validPicHeader;
    u32 validSequence;

    picture_t pPicBuf[16];
    u32 outBuf[16];
    u32 outIndex;
    u32 outCount;
    u32 workOut;
    u32 work0;
    u32 work1;
    u32 latestId;   /* current pic id, used for debug */
    u32 skipB;
    u32 prevPicCodingType;
    u32 prevPicStructure;
    u32 fieldToReturn;  /* 0 = First, 1 second */
    
    u32 fieldIndex;
    i32 fieldOutIndex;

    u32 frameNumber;
    u32 frameWidth;
    u32 frameHeight;
    u32 totalMbsInFrame;

    DWLLinearMem_t directMvs;

    u32 pictureBroken;
    u32 intraFreeze;

    u32 previousB;
    u32 previousModeFull;

    u32 sequenceLowDelay;

    u32 newHeadersChangeResolution;

    u32 maxNumBuffers;
    u32 numBuffers;
    u32 numPpBuffers;
    bufferQueue_t bq;
    bufferQueue_t bqPp;
} DecStrmStorage;

#endif /* #ifndef AVSDECSTRMSTORAGE_H_DEFINED */
