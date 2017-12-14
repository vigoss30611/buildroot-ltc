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
--  $RCSfile: rv_strmdesc.h,v $
--  $Date: 2009/03/11 14:00:12 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

/*****************************************************************************/

#ifndef RV_STRMDESC_H
#define RV_STRMDESC_H

#include "basetype.h"

typedef struct
{
    u8 *pStrmBuffStart; /* pointer to start of stream buffer */
    u8 *pStrmCurrPos;   /* current read addres in stream buffer */
    u32 bitPosInWord;   /* bit position in stream buffer */
    u32 strmBuffSize;   /* size of stream buffer (bytes) */
    u32 strmBuffReadBits;   /* number of bits read from stream buffer */

} DecStrmDesc;

#endif /* #ifndef RV_STRMDESC_H */
