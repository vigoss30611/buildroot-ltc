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
--  Abstract : Motion vector storage definition
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_mvstorage.h,v $
--  $Date: 2007/11/26 08:27:58 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef DECMVSTORAGE_H_DEFINED
#define DECMVSTORAGE_H_DEFINED

#include "basetype.h"

/* 31 macro blocks, 4 motion vectors each, horizontal and vertical */
enum
{
    MV_STORAGE_SIZE = 368
};

typedef struct
{
    i32 motionVectors[MV_STORAGE_SIZE];
} DecMvStorage;

#endif
