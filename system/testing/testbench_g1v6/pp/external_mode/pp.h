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
--  Abstract :
--
------------------------------------------------------------------------------*/
#ifndef __PP_H_INCLUDED_
#define __PP_H_INCLUDED_

#include "defs.h"

void Rgb2Yuv( PixelData *p, ColorConversion *cc, RgbFormat * rgb );
void Resample( PixelData *p, PixelFormat fmtNew );
void Pip( PixelData * dst, PixelData * src, i32 x0, i32 y0, Area **masks );
void Rotate( PixelData * p, i32 deg );
void Mirror( PixelData * p );
void Flip( PixelData * p );
void Crop( PixelData * dst, PixelData * src, int x0, int y0 );
void GenerateBackground( PixelData * p );
void Masks( PixelData * p, Area **masks );
void VideoRange( PixelData * p, u32 videoRange );
void FirFilter( PixelData *p , FilterData * f);
void TruncateRgb( PixelData *p , RgbFormat *r );
void MapColorConversion( ColorConversion *cc );

#endif /* __PP_H_INCLUDED_ */
