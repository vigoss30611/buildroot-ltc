/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : Converts data to tiled format.
--
------------------------------------------------------------------------------*/
#include <stdio.h>
#include "basetype.h"

extern void TbWriteTiledOutput(FILE *file, u8 *data, u32 mbWidth, u32 mbHeight, 
        u32 picNum, u32 md5SumOutput, u32 inputFormat);
        
extern void TbChangeEndianess(u8 *data, u32 dataSize);

void TBTiledToRaster( u32 tileMode, u32 dpbMode, u8 *pIn, u8 *pOut, u32 picWidth, 
                    u32 picHeight );
#ifdef LUMA_CHROMA_SEPERATED
void TBFieldDpbToFrameDpb2( u32 dpbMode, u8 *pIn, u8 *pCIn, u8 *pOut, u32 monochrome,
                           u32 frameStride, u32 picWidth, u32 picHeight );
#endif
void TBFieldDpbToFrameDpb( u32 dpbMode, u8 *pIn, u8 *pOut, u32 monochrome,
                           u32 picWidth, u32 picHeight );

