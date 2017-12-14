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
--  Abstract : API's internal static data storage definition
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: avs_apistorage.h,v $
--  $Date: 2010/02/09 06:44:00 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

#ifndef AVSDECAPISTORAGE_H
#define AVSDECAPISTORAGE_H

#include "dwl.h"

typedef struct
{
    enum
    {
        UNINIT,
        INITIALIZED,
        HEADERSDECODED,
        STREAMDECODING,
        HW_PIC_STARTED,
        HW_STRM_ERROR
    } DecStat;

    enum
    {
        NO_BUFFER = 0,
        BUFFER_0,
        BUFFER_1,
        BUFFER_2
    } bufferForPp;

    u32 ppPicIndex;

    u32 firstHeaders;
    u32 externalBuffers;    /* application gives frame buffers */
    u32 firstField;
    u32 outputOtherField;
    u32 parity;
    u32 ignoreField;

} DecApiStorage;

#endif /* #ifndef AVSDECAPISTORAGE_H */
