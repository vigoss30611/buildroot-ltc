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
--  $RCSfile: mp4dechwd_container.h,v $
--  $Date: 2010/12/01 12:31:04 $
--  $Revision: 1.15 $
--
------------------------------------------------------------------------------*/

#ifndef _DECCONTAINER_H_
#define _DECCONTAINER_H_

#include "basetype.h"
#include "mp4dechwd_vopdesc.h"
#include "mp4dechwd_mbsetdesc.h"
#include "mp4dechwd_strmdesc.h"
#include "mp4dechwd_mbdesc.h"
#include "mp4dechwd_hdrs.h"
#include "mp4dechwd_svdesc.h"
#include "mp4dechwd_storage.h"
#include "mp4dechwd_mvstorage.h"
#include "mp4decapihwd_storage.h"
#include "mp4deccfg.h"
#include "deccfg.h"
#include "decppif.h"
#include "refbuffer.h"
#include "workaround.h"

typedef struct DecContainer_t
{
    u32 mp4Regs[DEC_X170_REGISTERS];
    DecVopDesc VopDesc;         /* VOP description */
    DecMbSetDesc MbSetDesc;     /* Mb set descriptor */
    DecMBDesc MBDesc[MP4API_DEC_MBS];
    DecStrmDesc StrmDesc;
    DecStrmStorage StrmStorage; /* StrmDec storage */
    DecHdrs Hdrs;
    DecHdrs tmpHdrs;
    DecSvDesc SvDesc;   /* Short video descriptor */
    DecApiStorage ApiStorage;  /* Api's internal data storage */
    DecPpInterface ppControl;
    DecPpQuery ppConfigQuery; /* Decoder asks pp info about setup, info stored here */
    u32 ppStatus;
    u32 asicRunning;
    u32 rlcMode;
    const void *dwl;
    u32 refBufSupport;
    u32 tiledModeSupport;
    u32 tiledReferenceEnable;
    refBuffer_t refBufferCtrl;
    workaround_t workarounds;
    u32 packedMode;
	u32 use_mmu;

    const void *ppInstance;
    void (*PPRun) (const void *, DecPpInterface *);
    void (*PPEndCallback) (const void *);
    void  (*PPConfigQuery)(const void *, DecPpQuery *);
    void (*PPDisplayIndex)(const void *, u32);
    void (*PPBufferData) (const void *, u32, u32, u32);

} DecContainer;

#endif /* _DECCONTAINER_H_ */
