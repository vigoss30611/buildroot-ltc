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
--  Abstract : Get neighbour blocks
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_neighbour.h,v $
--  $Date: 2008/03/13 12:48:06 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of contents
   
    1. Include headers
    2. Module defines
    3. Data types
    4. Function prototypes

------------------------------------------------------------------------------*/

#ifndef H264HWD_NEIGHBOUR_H
#define H264HWD_NEIGHBOUR_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"

#include "h264hwd_macroblock_layer.h"

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

typedef enum {
    MB_A = 0,
    MB_B,
    MB_C,
    MB_D,
    MB_CURR,
    MB_NA = 0xFF
} neighbourMb_e;

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

typedef struct
{
    neighbourMb_e   mb;
    u8             index;
} neighbour_t;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

void h264bsdInitMbNeighbours(mbStorage_t *pMbStorage, u32 picWidth,
    u32 picSizeInMbs);

/*@null@*/ mbStorage_t* h264bsdGetNeighbourMb(mbStorage_t *pMb,
                                              neighbourMb_e neighbour);

u32 h264bsdIsNeighbourAvailable(mbStorage_t *pMb,
                                 /*@null@*/ mbStorage_t *pNeighbour);

const neighbour_t* h264bsdNeighbour4x4BlockA(u32 blockIndex);
const neighbour_t* h264bsdNeighbour4x4BlockB(u32 blockIndex);
const neighbour_t* h264bsdNeighbour4x4BlockC(u32 blockIndex);
const neighbour_t* h264bsdNeighbour4x4BlockD(u32 blockIndex);

#endif /* #ifdef H264HWD_NEIGHBOUR_H */
