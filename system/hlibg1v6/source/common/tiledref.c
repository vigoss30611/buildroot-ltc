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
--  Abstract : Stream decoding utilities
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: tiledref.c,v $
--  $Date: 2011/01/17 13:17:06 $
--  $Revision: 1.4 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "tiledref.h"
#include "regdrv.h"

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    DecSetupTiledReference
        Enable/disable tiled reference mode on HW. See inside function for
        disable criterias.

        Returns tile mode.
------------------------------------------------------------------------------*/
u32 DecSetupTiledReference( u32 *regBase, u32 tiledModeSupport, u32 interlacedStream )
{
    u32 tiledAllowed = 1;
    u32 mode = 0;

    if(!tiledModeSupport)
    {
        SetDecRegister(regBase, HWIF_TILED_MODE_MSB, 0 );
        SetDecRegister(regBase, HWIF_TILED_MODE_LSB, 0 );
        return TILED_REF_NONE;
    }

    /* disable for interlaced streams */
    if(interlacedStream)
        tiledAllowed = 0;

    /* if tiled mode allowed, pick a tile mode that suits us best (pretty easy
     * as we currently only support 8x4 */
    if(tiledAllowed)
    {
        if(tiledModeSupport == 1 )
        {
            mode = TILED_REF_8x4;
        }
    }
    else
    {
        mode = TILED_REF_NONE;
    }

    /* Setup HW registers */
    SetDecRegister(regBase, HWIF_TILED_MODE_MSB, (mode >> 1) & 0x1 );
    SetDecRegister(regBase, HWIF_TILED_MODE_LSB, mode & 0x1 );
    
    return mode;
}


