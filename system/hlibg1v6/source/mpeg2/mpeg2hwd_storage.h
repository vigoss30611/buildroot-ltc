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
--  $RCSfile: mpeg2hwd_storage.h,v $
--  $Date: 2010/12/01 12:31:04 $
--  $Revision: 1.19 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context 

     1. xxx...
   
 
------------------------------------------------------------------------------*/

#ifndef MPEG2DECSTRMSTORAGE_H_DEFINED
#define MPEG2DECSTRMSTORAGE_H_DEFINED

#include "basetype.h"
#include "mpeg2hwd_cfg.h"
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
    Mpeg2DecRet retVal;
    u32 sendToPp;
    Mpeg2DecTime timeCode;
    u32 tiledMode;
} picture_t;

typedef struct
{
    u32 status;
    u32 strmDecReady;

    u32 validPicHeader;
    u32 validPicExtHeader;
    u32 validSequence;
    u32 vpQP;

    u32 maxNumBuffers;
    u32 numBuffers;
    u32 numPpBuffers;
    picture_t pPicBuf[16];
    u32 outBuf[16];
    u32 outIndex;
    u32 outCount;
    u32 workOut;
    u32 work0;
    u32 work1;
    u32 workOutPrev;
    u32 latestId;   /* current pic id, used for debug */
    u32 skipB;
    u32 prevPicCodingType;
    u32 prevPicStructure;
    u32 fieldToReturn;  /* 0 = First, 1 second */

    u32 pictureBroken;
    u32 intraFreeze;
    u32 newHeadersChangeResolution;
    u32 previousB;
    u32 previousModeFull;

    u32 prevBIdx;

    bufferQueue_t bq;
    bufferQueue_t bqPp;

    u32 parallelMode2;
    u32 pm2lockBuf; /* "Locked" PP buffer, i.e. future output frame
                     * that shouldn't be overwritten. */
    u32 pm2AllProcessedFlag;
    u32 pm2StartPoint;
    u32 lastBSkipped;

} DecStrmStorage;

#endif /* #ifndef MPEG2DECSTRMSTORAGE_H_DEFINED */
