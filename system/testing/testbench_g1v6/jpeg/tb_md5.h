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
--  Abstract : post-processor external mode testbench
--
------------------------------------------------------------------------------*/

#ifndef TB_MD5_H
#define TB_MD5_H

#include "basetype.h"
#include <stdio.h>

extern u32 TBWriteFrameMD5Sum(FILE* fOut, u8* pYuv, u32 yuvSize, u32 frameNumber);

#endif
