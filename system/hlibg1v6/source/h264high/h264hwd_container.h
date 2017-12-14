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
--  $RCSfile: h264hwd_container.h,v $
--  $Date: 2010/12/01 12:31:03 $
--  $Revision: 1.13 $
--
------------------------------------------------------------------------------*/

#ifndef H264HWD_CONTAINER_H
#define H264HWD_CONTAINER_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "h264hwd_storage.h"
#include "h264hwd_util.h"
#include "refbuffer.h"
#include "deccfg.h"
#include "decppif.h"

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

#define H264DEC_UNINITIALIZED   0U
#define H264DEC_INITIALIZED     1U
#define H264DEC_BUFFER_EMPTY    2U
#define H264DEC_NEW_HEADERS     3U

/* asic interface */
typedef struct DecAsicBuffers
{
    u32 buff_status;
    DWLLinearMem_t mbCtrl;
    DWLLinearMem_t mv;
    DWLLinearMem_t intraPred;
    DWLLinearMem_t residual;
    DWLLinearMem_t *outBuffer;
    DWLLinearMem_t cabacInit;
    u32 refPicList[16];
    u32 maxRefFrm;
    u32 filterDisable;
    i32 chromaQpIndexOffset;
    i32 chromaQpIndexOffset2;
    u32 currentMB;
    u32 notCodedMask;
    u32 rlcWords;
    u32 picSizeInMbs;
    u32 wholePicConcealed;
    u32 disableOutWriting;
    u32 enableDmvAndPoc;
} DecAsicBuffers_t;

typedef struct decContainer
{
    const void *checksum;
    u32 decStat;
    u32 picNumber;
    u32 asicRunning;
    u32 rlcMode;
    u32 tryVlc;
    u32 reallocate;
    const u8 *pHwStreamStart;
    u32 hwStreamStartBus;
    u32 hwBitPos;
    u32 hwLength;
    u32 streamPosUpdated;
    u32 nalStartCode;
    u32 modeChange;
    u32 gapsCheckedForThis;
    u32 packetDecoded;
    u32 forceRlcMode;        /* by default stays 0, testing can set it to 1 for RLC mode */

    u32 h264Regs[DEC_X170_REGISTERS];
    storage_t storage;       /* h264bsd storage */
    DecAsicBuffers_t asicBuff[1];
    const void *dwl;         /* DWL instance */
    u32 refBufSupport;
    u32 tiledModeSupport;
    u32 tiledReferenceEnable;
    u32 h264ProfileSupport;
    u32 is8190;
    u32 maxDecPicWidth;
    refBuffer_t refBufferCtrl;

    u32 keepHwReserved;
    u32 skipNonReference;
	u32 mmuEnable;
	u32 processed;
	u32 processedinterval;
    struct pp_
    {
        const void *ppInstance;
        void (*PPDecStart) (const void *, const DecPpInterface *);
        void (*PPDecWaitEnd) (const void *);
        void (*PPConfigQuery) (const void *, DecPpQuery *);
        void (*PPNextDisplayId)(const void *, u32); /* set the next PP outpic ID (multibuffer) */ 
        DecPpInterface decPpIf;
        DecPpQuery ppInfo;
        const DWLLinearMem_t * sentPicToPp[17]; /* list of pictures sent to pp */
        const DWLLinearMem_t * queuedPicToPp[2]; /* queued picture that should be processed next */
        u32 multiMaxId; /* maximum position used in sentPicToPp[] */
    } pp;
} decContainer_t;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

#endif /* #ifdef H264HWD_CONTAINER_H */
