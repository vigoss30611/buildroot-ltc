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
--  Description : Container for picture and field information
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_picture.h,v $
--  $Revision: 1.7 $
--  $Date: 2010/12/01 12:31:04 $
--
------------------------------------------------------------------------------*/

#ifndef VC1HWD_PICTURE_H
#define VC1HWD_PICTURE_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "dwl.h"
#include "vc1decapi.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

typedef enum 
{
    NONE = 0,
    PIPELINED = 1,
    PARALLEL = 2,
    STAND_ALONE = 3
} decPpStatus_e;

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

/* Part of picture buffer descriptor. This informaton is unique 
 * for each field of the frame. */
typedef struct field 
{
    intCompField_e intCompF;
    i32 iScaleA;            /* A used for TOP */
    i32 iShiftA;
    i32 iScaleB;            /* B used for BOTTOM */
    i32 iShiftB;
    
    picType_e type;
    u32 picId;

    u32 ppBufferIndex;
    decPpStatus_e decPpStat; /* pipeline / parallel ... */
    VC1DecRet returnValue;
} field_t;


/* Picture buffer descriptor. Used for anchor frames
 * and work buffer and output buffer. */
typedef struct picture 
{
    DWLLinearMem_t data;

    fcm_e fcm;              /* frame coding mode */
    
    u16x codedWidth;        /* Coded height in pixels */
    u16x codedHeight;       /* Coded widht in pixels */
    u16x keyFrame;          /* for field pictures both must be Intra */

    u16x rangeRedFrm;       /* Main profile range reduction information */
    u32 rangeMapYFlag;      /* Advanced profile range mapping parameters */
    u32 rangeMapY;
    u32 rangeMapUvFlag;
    u32 rangeMapUv;
    
    u32 isFirstField;       /* Is current field first or second */
    u32 isTopFieldFirst;    /* Which field is first */
    u32 rff;                /* repeat first field */
    u32 rptfrm;             /* repeat frame count */
    u32 tiledMode;

    field_t field[2];       /* 0 = first; 1 = second */
} picture_t;

/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

#endif /* #ifndef VC1SWD_PICTURE_H */

