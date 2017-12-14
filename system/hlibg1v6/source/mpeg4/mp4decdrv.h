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
--  Description : 
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4decdrv.h,v $
--  $Revision: 1.2 $
--  $Date: 2007/06/05 12:48:06 $
--
------------------------------------------------------------------------------*/

#ifndef MP4REGDRV_H
#define MP4REGDRV_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "regdrv.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define MP4_DEC_X170_MODE_MPEG4      1

#define MP4_DEC_X170_IRQ_DEC_RDY    0x01
#define MP4_DEC_X170_IRQ_BUS_ERROR  0x02
#define MP4_DEC_X170_IRQ_BUFFER_EMPTY   0x04
#define MP4_DEC_X170_IRQ_ASO            0x08
#define MP4_DEC_X170_IRQ_STREAM_ERROR   0x10
#define MP4_DEC_X170_IRQ_TIMEOUT        0x40
#define MP4_DEC_X170_IRQ_CLEAR_ALL  0xFF

#endif /* #ifndef MP4REGDRV_H */
