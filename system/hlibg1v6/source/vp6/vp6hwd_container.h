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
--  $RCSfile: vp6hwd_container.h,v $
--  $Date: 2010/12/01 12:31:04 $
--  $Revision: 1.8 $
--
------------------------------------------------------------------------------*/

#ifndef VP6HWD_CONTAINER_H
#define VP6HWD_CONTAINER_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "deccfg.h"
#include "decppif.h"
#include "dwl.h"
#include "vp6dec.h"
#include "refbuffer.h"
#include "bqueue.h"

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

#define VP6DEC_UNINITIALIZED   0U
#define VP6DEC_INITIALIZED     1U
#define VP6DEC_BUFFER_EMPTY    2U
#define VP6DEC_NEW_HEADERS     3U

/* asic interface */
typedef struct DecAsicBuffers
{
    DWLLinearMem_t probTbl;
    DWLLinearMem_t *outBuffer;
    DWLLinearMem_t *prevOutBuffer;
    DWLLinearMem_t *refBuffer;
    DWLLinearMem_t *goldenBuffer;
    DWLLinearMem_t pictures[16];

    /* Indexes for picture buffers in pictures[] array */
    u32 outBufferI;
    u32 refBufferI;
    u32 goldenBufferI;

    u32 width;
    u32 height;
    u32 wholePicConcealed;
    u32 disableOutWriting;
    u32 partition1Base;
    u32 partition1BitOffset;
    u32 partition2Base;
} DecAsicBuffers_t;

typedef struct VP6DecContainer
{
    const void *checksum;
    u32 decStat;
    u32 picNumber;
    u32 asicRunning;
    u32 width;
    u32 height;
    u32 vp6Regs[DEC_X170_REGISTERS];
    DecAsicBuffers_t asicBuff[1];
    const void *dwl;         /* DWL instance */
    u32 refBufSupport;
    u32 tiledModeSupport;
    u32 tiledReferenceEnable;
    refBuffer_t refBufferCtrl;

    PB_INSTANCE pb;          /* SW decoder instance */    

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
    u32 refToOut;
    u32 outCount;

    u32 numBuffers;
    bufferQueue_t bq;

} VP6DecContainer_t;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

#endif /* #ifdef VP6HWD_CONTAINER_H */
