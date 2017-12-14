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
--  Abstract : Picture level data structure
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_vopdesc.h,v $
--  $Date: 2008/09/03 05:58:25 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

/*****************************************************************************/

#ifndef DECVOPDESC_H_DEFINED
#define DECVOPDESC_H_DEFINED

#include "basetype.h"

typedef struct DecVopDesc_t
{
    u32 vopNumber;
    u32 vopNumberInSeq;
    u32 vopTimeIncrement;
    u32 moduloTimeBase;
    u32 prevVopTimeIncrement;
    u32 prevModuloTimeBase;
    u32 trb;
    u32 trd;
    u32 ticsFromPrev;   /* tics (1/vop_time_increment_resolution
                             * seconds) since previous vop */
    u32 intraDcVlcThr;
    u32 vopCodingType;
    u32 totalMbInVop;
    u32 vopWidth;   /* in macro blocks */
    u32 vopHeight;  /* in macro blocks */
    u32 qP;
    u32 fcodeFwd;
    u32 fcodeBwd;
    u32 vopCoded;
    u32 vopRoundingType;
    /* following three parameters will be read from group of VOPs header
     * and will be updated based on time codes received in VOP header */
    u32 timeCodeHours;
    u32 timeCodeMinutes;
    u32 timeCodeSeconds;
    u32 govCounter; /* number of groups of VOPs */

    /* for interlace support */
    u32 topFieldFirst;
    u32 altVerticalScanFlag;

} DecVopDesc;

#endif
