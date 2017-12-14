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
--  Abstract: api internal defines
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rvdecapi_internal.h,v $
--  $Date: 2010/08/17 07:50:09 $
--  $Revision: 1.3 $
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. Internal Definitions
    3. Prototypes of Decoder API internal functions

------------------------------------------------------------------------------*/

#ifndef RV_DECAPI_INTERNAL_H
#define RV_DECAPI_INTERNAL_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "rv_cfg.h"
#include "rv_utils.h"
#include "rvdecapi.h"

/*------------------------------------------------------------------------------
    2. Internal Definitions
------------------------------------------------------------------------------*/

#define RV_DEC_X170_IRQ_DEC_RDY    0x01
#define RV_DEC_X170_IRQ_BUS_ERROR  0x02
#define RV_DEC_X170_IRQ_BUFFER_EMPTY   0x04
#define RV_DEC_X170_IRQ_ASO            0x08
#define RV_DEC_X170_IRQ_STREAM_ERROR   0x10
#define RV_DEC_X170_IRQ_TIMEOUT        0x40
#define RV_DEC_X170_IRQ_CLEAR_ALL  0xFF

#define RV_DEC_X170_MAX_NUM_SLICES  (128)

/*
 *  Size of internal frame buffers (in 32bit-words) per macro block
 */
#define RVAPI_DEC_FRAME_BUFF_SIZE  96

#ifndef NULL
#define NULL 0
#endif

#define SWAP_POINTERS(A, B, T) T = A; A = B; B = T;

#define INVALID_ANCHOR_PICTURE ((u32)-1)

/*------------------------------------------------------------------------------
    3. Prototypes of Decoder API internal functions
------------------------------------------------------------------------------*/

void rvAPI_InitDataStructures(DecContainer * pDecCont);
RvDecRet rvAllocateBuffers(DecContainer * pDecCont);
RvDecRet rvDecCheckSupport(DecContainer * pDecCont);
void rvDecPreparePicReturn(DecContainer * pDecCont);
void rvDecAspectRatio(DecContainer * pDecCont, RvDecInfo * pDecInfo);
void rvDecBufferPicture(DecContainer * pDecCont, u32 picId, u32 bufferB,
                           u32 isInter, RvDecRet returnValue, u32 nbrErrMbs);
void rvFreeBuffers(DecContainer * pDecCont);
void rvInitVlcTables(DecContainer * pDecCont);

#endif /* RV_DECAPI_INTERNAL_H */
