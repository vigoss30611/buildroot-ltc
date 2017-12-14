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
--  $RCSfile: vc1hwd_regdrv.h,v $
--  $Revision: 1.1 $
--  $Date: 2007/06/19 13:42:59 $
--
------------------------------------------------------------------------------*/

#ifndef VC1REGDRV_H
#define VC1REGDRV_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "regdrv.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define DEC_X170_MODE_VC1      4

#define DEC_X170_IRQ_DEC_RDY    0x01
#define DEC_X170_IRQ_BUS_ERROR  0x02
#define DEC_X170_IRQ_BUFFER_EMPTY   0x04
#define DEC_X170_IRQ_ASO            0x08
#define DEC_X170_IRQ_STREAM_ERROR   0x10
#define DEC_X170_IRQ_TIMEOUT        0x40
#define DEC_X170_IRQ_CLEAR_ALL  0xFF


#endif /* #ifndef VC1REGDRV_H */
