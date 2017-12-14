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
--  Abstract : Stream decoding top header file
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_strmdec.h,v $
--  $Date: 2007/11/26 07:51:36 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. xxx...


------------------------------------------------------------------------------*/

#ifndef STRMDEC_H_DEFINED
#define STRMDEC_H_DEFINED

#include "mp4dechwd_container.h"

/* StrmDec_Decode() function return values
 * DEC_RDY -> everything ok but no VOP finished
 * DEC_VOP_RDY -> vop finished
 * DEC_VOP_RDY_BUF_NOT_EMPTY -> vop finished but stream buffer not empty
 * DEC_ERROR -> nothing finished, decoder not ready, cannot continue decoding
 *      before some headers received
 * DEC_ERROR_BUF_NOT_EMPTY -> same as above but stream buffer not empty
 * DEC_END_OF_STREAM -> nothing finished, no data left in stream
 * DEC_OUT_OF_BUFFER -> out of rlc buffer
 * DEC_VOS_END -> vos end code encountered, stopping
 * DEC_HDRS_RDY -> either vol header decoded or short video source format
 *      determined
 * DEC_HDRS_RDY_BUF_NOT_EMPTY -> same as above but stream buffer not empty
 *
 * Bits in the output have following meaning:
 * 0    ready
 * 1    vop ready
 * 2    buffer not empty
 * 3    error
 * 4    end of stream
 * 5    out of buffer
 * 6    video object sequence end
 * 7    headers ready
 */
enum
{
    DEC_RDY = 0x01,
    DEC_VOP_RDY = 0x03,
    DEC_VOP_RDY_BUF_NOT_EMPTY = 0x07,
    DEC_ERROR = 0x08,
    DEC_ERROR_BUF_NOT_EMPTY = 0x0C,
    DEC_END_OF_STREAM = 0x10,
    DEC_OUT_OF_BUFFER = 0x20,
    DEC_VOS_END = 0x40,
    DEC_HDRS_RDY = 0x80,
    DEC_HDRS_RDY_BUF_NOT_EMPTY = 0x84,
    DEC_VOP_HDR_RDY = 0x100,
    DEC_VOP_HDR_RDY_ERROR = 0x108,
    DEC_VOP_SUPRISE_B = 0x1000

};

/* function prototypes */
void StrmDec_DecoderInit(DecContainer * pDecContainer);
u32 StrmDec_Decode(DecContainer * pDecContainer);

#endif
