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
--  $RCSfile: mpeg2hwd_container.h,v $
--  $Date: 2010/12/01 12:31:04 $
--  $Revision: 1.14 $
--
------------------------------------------------------------------------------*/

#ifndef _MPEG2DECCONTAINER_H_
#define _MPEG2DECCONTAINER_H_

#include "basetype.h"
#include "mpeg2hwd_framedesc.h"
#include "mpeg2hwd_mbsetdesc.h"
#include "mpeg2hwd_strmdesc.h"
#include "mpeg2hwd_hdrs.h"
#include "mpeg2hwd_storage.h"
#include "mpeg2hwd_apistorage.h"
#include "mpeg2hwd_cfg.h"
#include "deccfg.h"
#include "decppif.h"
#include "refbuffer.h"
#include "workaround.h"

typedef struct
{
    u32 mpeg2Regs[DEC_X170_REGISTERS];
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
    u32 keepHwReserved;
    u32 unpairedField;
	u32 mmuEnable;

    workaround_t workarounds;

    const void *ppInstance;
    void (*PPRun) (const void *, DecPpInterface *);
    void (*PPEndCallback) (const void *);
    void (*PPConfigQuery) (const void *, DecPpQuery *);
    void (*PPDisplayIndex)(const void *, u32);
    void (*PPBufferData) (const void *, u32, u32, u32);

} DecContainer;

#endif /* #ifndef _MPEG2DECCONTAINER_H_ */
