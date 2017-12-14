/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2007 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : Interface for sequence level header decoding
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_headers.h,v $
--  $Revision: 1.3 $
--  $Date: 2007/12/04 14:44:18 $
--
------------------------------------------------------------------------------*/

#ifndef VC1HWD_HEADERS_H
#define VC1HWD_HEADERS_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1hwd_util.h"
#include "vc1hwd_stream.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/* enumerated sample aspect ratios, ASPECT_RATIO_M_N means M:N */
enum
{
    ASPECT_RATIO_UNSPECIFIED = 0,
    ASPECT_RATIO_1_1,
    ASPECT_RATIO_12_11,
    ASPECT_RATIO_10_11,
    ASPECT_RATIO_16_11,
    ASPECT_RATIO_40_33,
    ASPECT_RATIO_24_11,
    ASPECT_RATIO_20_11,
    ASPECT_RATIO_32_11,
    ASPECT_RATIO_80_33,
    ASPECT_RATIO_18_11,
    ASPECT_RATIO_15_11,
    ASPECT_RATIO_64_33,
    ASPECT_RATIO_160_99,
    ASPECT_RATIO_EXTENDED = 15
};

typedef enum {
    SC_END_OF_SEQ       = 0x0000010A,
    SC_SLICE            = 0x0000010B,
    SC_FIELD            = 0x0000010C,
    SC_FRAME            = 0x0000010D, 
    SC_ENTRY_POINT      = 0x0000010E,
    SC_SEQ              = 0x0000010F,
    SC_SLICE_UD         = 0x0000011B,
    SC_FIELD_UD         = 0x0000011C,
    SC_FRAME_UD         = 0x0000011D,
    SC_ENTRY_POINT_UD   = 0x0000011E,
    SC_SEQ_UD           = 0x0000011F,
    SC_NOT_FOUND        = 0xFFFE
} startCode_e;

typedef enum {
    VC1_SIMPLE,
    VC1_MAIN,
    VC1_ADVANCED
} vc1Profile_e;

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

struct swStrmStorage;
u32 vc1hwdDecodeSequenceLayer( struct swStrmStorage *pStorage,
                               strmData_t *pStrmData );

u32 vc1hwdDecodeEntryPointLayer( struct swStrmStorage *pStorage,
                                 strmData_t *pStrmData );

u32 vc1hwdGetStartCode( strmData_t *pStrmData );

u32 vc1hwdGetUserData( struct swStrmStorage *pStorage,
                       strmData_t *pStrmData );

#endif /* #ifndef VC1HWD_HEADERS_H */

