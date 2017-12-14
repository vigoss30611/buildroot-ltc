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
--  $RCSfile: rv_storage.h,v $
--  $Date: 2010/12/01 12:31:04 $
--  $Revision: 1.9 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context 

     1. xxx...
   
 
------------------------------------------------------------------------------*/

#ifndef RV_STRMSTORAGE_H
#define RV_STRMSTORAGE_H

#include "basetype.h"
#include "rv_cfg.h"
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
    RvDecRet retVal;
    u32 sendToPp;
    u32 frameWidth, frameHeight;
    u32 codedWidth, codedHeight;
    u32 tiledMode;
} picture_t;

typedef struct
{
    u32 status;
    u32 strmDecReady;

    picture_t pPicBuf[16];
    picture_t pRprBuf;
    u32 outBuf[16];
    u32 outIndex;
    u32 outCount;
    u32 workOut;
    u32 work0;
    u32 work1;
    u32 latestId;   /* current pic id, used for debug */
    u32 skipB;
    u32 prevPicCodingType;

    u32 pictureBroken;
    u32 intraFreeze;
    u32 rprDetected;
    u32 rprNextPicType;
    u32 previousB;
    u32 previousModeFull;

    u32 isRv8;
    u32 fwdScale;
    u32 bwdScale;
    u32 tr;
    u32 prevTr;
    u32 trb;
    DWLLinearMem_t vlcTables;
    DWLLinearMem_t directMvs;
    DWLLinearMem_t rprWorkBuffer;
    DWLLinearMem_t slices;

    u32 frameCodeLength;
    u32 frameSizes[2*9];
    u32 maxFrameWidth, maxFrameHeight;
    u32 maxMbsPerFrame;

    u32 numSlices;
    u32 rawMode;

    /* to store number of bits needed to indicate rv9 frame size */
    u32 frameSizeBits;

    /* used to compute timestamps for output pictures */
    u32 picId;
    u32 prevPicId;

    u32 prevBIdx;
    bufferQueue_t bq;
    bufferQueue_t bqPp;
    u32 maxNumBuffers;
    u32 numBuffers;
    u32 numPpBuffers;

} DecStrmStorage;

#endif /* #ifndef RV_STRMSTORAGE_H */
