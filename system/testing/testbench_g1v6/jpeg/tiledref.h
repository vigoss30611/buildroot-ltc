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
--  Abstract : Header file for stream decoding utilities
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: tiledref.h,v $
--  $Date: 2010/12/01 12:31:03 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef TILEDREF_H_DEFINED
#define TILEDREF_H_DEFINED

#include "basetype.h"

#define TILED_REF_NONE  (0)
#define TILED_REF_8x4   (1)

u32 DecSetupTiledReference( u32 *regBase, u32 tiledModeSupport, u32 interlacedStream );

#endif /* TILEDREF_H_DEFINED */
