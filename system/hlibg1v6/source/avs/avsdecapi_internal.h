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
--  $RCSfile:
--  $Date:
--  $Revision:
------------------------------------------------------------------------------*/

#ifndef _AVSDECAPI_INTERNAL_H_
#define _AVSDECAPI_INTERNAL_H_

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "avs_cfg.h"
#include "avs_utils.h"
#include "avsdecapi.h"

/*------------------------------------------------------------------------------
    2. Internal Definitions
------------------------------------------------------------------------------*/
#define AVS_DEC_X170_MODE      9

#define AVS_DEC_X170_IRQ_DEC_RDY    0x01
#define AVS_DEC_X170_IRQ_BUS_ERROR  0x02
#define AVS_DEC_X170_IRQ_BUFFER_EMPTY   0x04
#define AVS_DEC_X170_IRQ_ASO            0x08
#define AVS_DEC_X170_IRQ_STREAM_ERROR   0x10
#define AVS_DEC_X170_IRQ_TIMEOUT        0x40
#define AVS_DEC_X170_IRQ_CLEAR_ALL  0xFF

/*
 *  Size of internal frame buffers (in 32bit-words) per macro block
 */
#define AVSAPI_DEC_FRAME_BUFF_SIZE  96

#ifndef NULL
#define NULL 0
#endif

#define SWAP_POINTERS(A, B, T) T = A; A = B; B = T;

#define INVALID_ANCHOR_PICTURE ((u32)-1)

/*------------------------------------------------------------------------------
    3. Prototypes of Decoder API internal functions
------------------------------------------------------------------------------*/

void AvsAPI_InitDataStructures(DecContainer * pDecCont);
void AvsDecTimeCode(DecContainer * pDecCont, AvsDecTime * timeCode);
AvsDecRet AvsAllocateBuffers(DecContainer * pDecCont);
AvsDecRet AvsDecCheckSupport(DecContainer * pDecCont);
void AvsDecPreparePicReturn(DecContainer * pDecCont);
void AvsDecAspectRatio(DecContainer * pDecCont, AvsDecInfo * pDecInfo);
void AvsDecBufferPicture(DecContainer * pDecCont, u32 picId, u32 bufferB,
                           u32 isInter, AvsDecRet returnValue, u32 nbrErrMbs);
void AvsFreeBuffers(DecContainer * pDecCont);

#endif /* _AVSDECAPI_INTERNAL_H_ */
