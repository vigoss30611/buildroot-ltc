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
-
-  Description : ...
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp8hwd_decoder.c,v $
-  $Revision: 1.4 $
-  $Date: 2009/12/29 14:18:25 $
-
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Includes
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp8hwd_headers.h"
#include "vp8hwd_probs.h"
#include "vp8hwd_container.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
    Functions
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
    ResetScan

        Reset decoder to initial state
------------------------------------------------------------------------------*/
void vp8hwdPrepareVp7Scan( vp8Decoder_t * dec, u32 * newOrder )
{
    u32 i;
    static const u32 Vp7DefaultScan[] = {
        0,  1,  4,  8,  5,  2,  3,  6,
        9, 12, 13, 10,  7, 11, 14, 15,
    };
        
    if(!newOrder)
    {
        for( i = 0 ; i < 16 ; ++i )
            dec->vp7ScanOrder[i] = Vp7DefaultScan[i];
    }
    else
    {
        for( i = 0 ; i < 16 ; ++i )
            dec->vp7ScanOrder[i] = Vp7DefaultScan[newOrder[i]];
    }
}

/*------------------------------------------------------------------------------
    vp8hwdResetDecoder

        Reset decoder to initial state
------------------------------------------------------------------------------*/
void vp8hwdResetDecoder( vp8Decoder_t * dec, DecAsicBuffers_t *asicBuff )
{
    if (asicBuff->segmentMap.virtualAddress)
        G1DWLmemset(asicBuff->segmentMap.virtualAddress, 
                  0, asicBuff->segmentMapSize);
    vp8hwdResetProbs( dec );
    vp8hwdPrepareVp7Scan( dec, NULL );
}
