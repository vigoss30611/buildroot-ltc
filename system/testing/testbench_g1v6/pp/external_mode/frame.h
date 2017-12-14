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
------------------------------------------------------------------------------*/
#ifndef __FRAME_H_INCLUDED_
#define __FRAME_H_INCLUDED_

#include "defs.h"

bool ReadFrame( char * filename, PixelData * p, u32 frame, PixelFormat fmt,
    u32 separateFields );
void WriteFrame( char * filename, PixelData * p, int frame, RgbFormat * r, ByteFormat *bf ) ;
void ReleaseFrame( PixelData * p );
void SetupFrame( PixelData * p );
void AllocFrame( PixelData * p );
void CopyFrame( PixelData * dst, PixelData * src );
void PackRgbaBytes( u8 c0, u8 c1, u8 c2, u8 c3, RgbFormat *fmt, u8 *buf );

#endif /* __FRAME_H_INCLUDED_ */
