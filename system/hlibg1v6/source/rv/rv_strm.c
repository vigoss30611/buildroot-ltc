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
--  Abstract : Stream decoding top level functions (interface functions)
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rv_strm.c,v $
--  $Date: 2009/03/11 14:00:13 $
--  $Revision: 1.1 $
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

#include "rv_strm.h"
#include "rv_utils.h"
#include "rv_headers.h"
#include "rv_debug.h"

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

enum
{
    CONTINUE
};

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   5.1  Function name: rvStrmDec_Decode

        Purpose: Decode RV stream
        
        Input:

        Output:

------------------------------------------------------------------------------*/

u32 rv_StrmDecode(DecContainer * pDecContainer)
{

    u32 type;
    u32 status = HANTRO_OK;

    RVDEC_DEBUG(("Entry rv_StrmDecode\n"));

    /* keep decoding till something ready or something wrong */
    do
    {
        /* TODO: mistä tyyppi, mitä dekoodataan jne? */
        type = RV_SLICE;

        /* parse headers */
        switch (type)
        {
            case RV_SLICE:
                status = rv_DecodeSliceHeader(pDecContainer);
                if (!pDecContainer->StrmStorage.strmDecReady)
                {
                    if (status == HANTRO_OK &&
                        pDecContainer->Hdrs.horizontalSize &&
                        pDecContainer->Hdrs.verticalSize)
                    {
                        pDecContainer->StrmStorage.strmDecReady = HANTRO_TRUE;
                        return DEC_HDRS_RDY;
                    }
                    /* else try next slice or what */
                }
                else if( pDecContainer->StrmStorage.rprDetected)
                {
                    /*return DEC_HDRS_RDY;*/
                    return DEC_PIC_HDR_RDY_RPR;
                }
                else if (status == HANTRO_OK)
                    return DEC_PIC_HDR_RDY;
                else
                    return DEC_PIC_HDR_RDY_ERROR;

                break;

            default:
                break;
        }

    }
    while(0);

    return (DEC_RDY);

}
