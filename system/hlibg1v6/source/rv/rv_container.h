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
--  Abstract : algorithm header file
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rv_container.h,v $
--  $Date: 2010/12/01 12:31:04 $
--  $Revision: 1.3 $
--
------------------------------------------------------------------------------*/

#ifndef RV_CONTAINER_H
#define RV_CONTAINER_H

#include "basetype.h"
#include "rv_framedesc.h"
#include "rv_mbsetdesc.h"
#include "rv_strmdesc.h"
#include "rv_hdrs.h"
#include "rv_storage.h"
#include "rv_apistorage.h"
#include "rv_cfg.h"
#include "deccfg.h"
#include "decppif.h"
#include "refbuffer.h"
#include "workaround.h"

typedef struct
{
    u32 rvRegs[DEC_X170_REGISTERS];
    DecFrameDesc FrameDesc; /* Frame description */
    DecMbSetDesc MbSetDesc; /* Mb set descriptor */
    DecStrmDesc StrmDesc;
    DecStrmStorage StrmStorage; /* StrmDec storage */
    DecHdrs Hdrs;
    DecHdrs tmpHdrs;    /* for decoding of repeated headers */
    DecApiStorage ApiStorage;   /* Api's internal data storage */
    DecPpInterface ppControl;
    DecPpQuery ppConfigQuery;   /* Decoder asks pp info about setup, info stored here */
    u32 ppStatus;
    u32 asicRunning;
    u32 mbErrorConceal;
    const void *dwl;
    u32 refBufSupport;
    u32 tiledModeSupport;
    u32 tiledReferenceEnable;
    refBuffer_t refBufferCtrl;
    workaround_t workarounds;

    const void *ppInstance;
    void (*PPRun) (const void *, DecPpInterface *);
    void (*PPEndCallback) (const void *);
    void (*PPConfigQuery) (const void *, DecPpQuery *);
    void (*PPDisplayIndex)(const void *, u32);
    void (*PPBufferData) (const void *, u32, u32, u32);

} DecContainer;

#endif /* #ifndef RV_CONTAINER_H */
