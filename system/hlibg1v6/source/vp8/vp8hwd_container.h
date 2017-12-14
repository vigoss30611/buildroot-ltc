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
--  Abstract : Definition of decContainer_t data structure
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vp8hwd_container.h,v $
--  $Date: 2010/12/01 12:31:05 $
--  $Revision: 1.9 $
--
------------------------------------------------------------------------------*/

#ifndef VP8HWD_CONTAINER_H
#define VP8HWD_CONTAINER_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "deccfg.h"
#include "decppif.h"
#include "dwl.h"
#include "refbuffer.h"

#include "vp8hwd_decoder.h"
#include "vp8hwd_bool.h"
#include "bqueue.h"

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

#define VP8DEC_UNINITIALIZED   0U
#define VP8DEC_INITIALIZED     1U
#define VP8DEC_NEW_HEADERS     3U
#define VP8DEC_DECODING        4U
#define VP8DEC_MIDDLE_OF_PIC   5U


typedef struct
{
    u32 *pPicBufferY;
    u32 picBufferBusAddrY;
    u32 *pPicBufferC;
    u32 picBufferBusAddrC;
} userMem_t;

/* asic interface */
typedef struct DecAsicBuffers
{
    u32 width, height;
    DWLLinearMem_t probTbl;
    DWLLinearMem_t segmentMap;
    DWLLinearMem_t *outBuffer;
    DWLLinearMem_t *prevOutBuffer;
    DWLLinearMem_t *refBuffer;
    DWLLinearMem_t *goldenBuffer;
    DWLLinearMem_t *alternateBuffer;
    DWLLinearMem_t pictures[16];

    /* Indexes for picture buffers in pictures[] array */
    u32 outBufferI;
    u32 refBufferI;
    u32 goldenBufferI;
    u32 alternateBufferI;

    u32 wholePicConcealed;
    u32 disableOutWriting;
    u32 segmentMapSize;
    u32 partition1Base;
    u32 partition1BitOffset;
    u32 partition2Base;
    i32 dcPred[2];
    i32 dcMatch[2];

    userMem_t userMem;
} DecAsicBuffers_t;

typedef struct VP8DecContainer
{
    const void *checksum;
    u32 decMode;
    u32 decStat;
    u32 picNumber;
    u32 asicRunning;
    u32 width;
    u32 height;
    u32 vp8Regs[DEC_X170_REGISTERS];
    DecAsicBuffers_t asicBuff[1];
    const void *dwl;         /* DWL instance */
    u32 refBufSupport;
    refBuffer_t refBufferCtrl;

    vp8Decoder_t decoder;
    vpBoolCoder_t bc;

    struct
    {
        const void *ppInstance;
        void (*PPDecStart) (const void *, const DecPpInterface *);
        void (*PPDecWaitEnd) (const void *);
        void (*PPConfigQuery) (const void *, DecPpQuery *);
        DecPpInterface decPpIf;
        DecPpQuery ppInfo;
    } pp;

    u32 pictureBroken;
    u32 intraFreeze;
    u32 outCount;
    u32 refToOut;
    u32 pendingPicToPp;

    bufferQueue_t bq;
    u32 numBuffers;

    u32 intraOnly;
    u32 userMem;
    u32 sliceHeight;
    u32 totDecodedRows;
    u32 outputRows;

    u32 tiledModeSupport;
    u32 tiledReferenceEnable;

} VP8DecContainer_t;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

#endif /* #ifdef VP8HWD_CONTAINER_H */
