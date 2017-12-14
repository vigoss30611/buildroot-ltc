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
-  $RCSfile: vp6scanorder.c,v $
-  $Revision: 1.1 $
-  $Date: 2008/04/14 10:13:25 $
-
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp6scanorder.h"
#include "vp6dec.h"

/****************************************************************************
* 
*  ROUTINE       :     BuildScanOrder
*
*  INPUTS        :     PB_INSTANCE *pbi : Pointer to instance of a decoder.
*                      u8 *ScanBands : Pointer to array containing band for 
*                                         each DCT coeff position. 
*
*  OUTPUTS       :     None
*
*  RETURNS       :     void
*
*  FUNCTION      :     Builds a custom dct scan order from a set of band data.
*
*  SPECIAL NOTES :     None. 
*
****************************************************************************/
void VP6HWBuildScanOrder(PB_INSTANCE * pbi, u8 * ScanBands)
{
    u32 i, j;
    u32 ScanOrderIndex = 1;
    u32 MaxOffset;

    // DC is fixed
    pbi->ModifiedScanOrder[0] = 0;

    // Create a scan order where within each band the coefs are in ascending order
    // (in terms of their original zig-zag positions).
    for(i = 0; i < SCAN_ORDER_BANDS; i++)
    {
        for(j = 1; j < BLOCK_SIZE; j++)
        {
            if(ScanBands[j] == i)
            {
                pbi->ModifiedScanOrder[ScanOrderIndex] = j;
                ScanOrderIndex++;
            }
        }
    }

    // For each of the positions in the modified scan order work out the 
    // worst case EOB offset in zig zag order. This is used in selecting
    // the appropriate idct variant
    for(i = 0; i < BLOCK_SIZE; i++)
    {
        MaxOffset = 0;

        for(j = 0; j <= i; j++)
        {
            if(pbi->ModifiedScanOrder[j] > MaxOffset)
                MaxOffset = pbi->ModifiedScanOrder[j];
        }

        pbi->EobOffsetTable[i] = MaxOffset;

        if(pbi->Vp3VersionNo > 6)
            pbi->EobOffsetTable[i] = MaxOffset + 1;
    }

    {
        i32 i;

        for(i = 0; i < 64; i++)
        {
            pbi->MergedScanOrder[i] =
                VP6HWtransIndexC[pbi->ModifiedScanOrder[i]];
        }

#if 0   /* TODO: is this needed? */
        // Create Huffman codes for tokens based on tree probabilities
        if(pbi->UseHuffman)
        {
            for(i = 64; i < 64 + 65; i++)
            {
                pbi->MergedScanOrder[i] = VP6HWCoeffToHuffBand[i - 64];
            }
        }
        else
        {
            for(i = 64; i < 64 + 65; i++)
                pbi->MergedScanOrder[i] = VP6HWCoeffToBand[i - 64];
        }
#endif

    }

}
