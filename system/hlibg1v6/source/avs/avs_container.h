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
--  $RCSfile: avs_container.h,v $
--  $Date: 2010/12/01 12:30:57 $
--  $Revision: 1.4 $
--
------------------------------------------------------------------------------*/

#ifndef _AVSDECCONTAINER_H_
#define _AVSDECCONTAINER_H_

#include "basetype.h"
#include "avs_strmdesc.h"
#include "avs_hdrs.h"
#include "avs_storage.h"
#include "avs_apistorage.h"
#include "avs_cfg.h"
#include "deccfg.h"
#include "decppif.h"
#include "refbuffer.h"

typedef struct
{
    u32 avsRegs[DEC_X170_REGISTERS];
    DecStrmDesc StrmDesc;
    DecStrmStorage StrmStorage; /* StrmDec storage */
    DecHdrs Hdrs;
    DecHdrs tmpHdrs;    /* for decoding of repeated headers */
    DecApiStorage ApiStorage;   /* Api's internal data storage */
    DecPpInterface ppControl;
    DecPpQuery ppConfigQuery;   /* Decoder asks pp info about setup, info stored here */

    AvsDecOutput outData;
    u32 ppStatus;
    u32 asicRunning;
    const void *dwl;
    u32 refBufSupport;
    refBuffer_t refBufferCtrl;

    u32 keepHwReserved;

    u32 tiledModeSupport;
    u32 tiledReferenceEnable;

    const void *ppInstance;
    void (*PPRun) (const void *, DecPpInterface *);
    void (*PPEndCallback) (const void *);
    void (*PPConfigQuery) (const void *, DecPpQuery *);
    void (*PPDisplayIndex)(const void *, u32);
    void (*PPBufferData) (const void *, u32, u32, u32);

} DecContainer;

#endif /* #ifndef _AVSDECCONTAINER_H_ */
