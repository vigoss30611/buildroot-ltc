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
--  $RCSfile: rv_strm.h,v $
--  $Date: 2009/03/11 14:00:12 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context 

     1. xxx...
   
 
------------------------------------------------------------------------------*/

#ifndef RV_STRMDEC_H
#define RV_STRMDEC_H

#include "rv_container.h"

enum
{
    DEC_RDY,
    DEC_HDRS_RDY,
    DEC_PIC_HDR_RDY,
    DEC_PIC_HDR_RDY_RPR,
    DEC_PIC_HDR_RDY_ERROR,
    DEC_ERROR,
    DEC_END_OF_STREAM
};

/* function prototypes */
u32 rv_StrmDecode(DecContainer * pDecContainer);

#endif /* #ifndef RV_STRMDEC_H */
