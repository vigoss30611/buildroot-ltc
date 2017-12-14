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
--  Description : VLC decoding functionality
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_vlc.h,v $
--  $Revision: 1.1 $
--  $Date: 2007/06/19 13:42:59 $
--
------------------------------------------------------------------------------*/

#ifndef VC1HWD_VLC_H
#define VC1HWD_VLC_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vc1hwd_stream.h"
#include "vc1hwd_picture_layer.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/* Special cases for VLC value */
#define INVALID_VLC_VALUE   (-1)

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

/* Picture layer codewords */
picType_e vc1hwdDecodePtype( strmData_t * const strmData, const u32 advanced,
                             const u16x maxBframes);
mvmode_e vc1hwdDecodeMvMode( strmData_t * const strmData, const u32 bPic,
                             const u16x pquant, u32 *pIntComp );
mvmode_e vc1hwdDecodeMvModeB( strmData_t * const strmData, const u16x pquant);
u16x vc1hwdDecodeTransAcFrm( strmData_t * const strmData );
void vc1hwdDecodeVopDquant( strmData_t * const strmData, const u16x dquant,
                            pictureLayer_t * const pLayer );
u16x vc1hwdDecodeMvRange( strmData_t * const strmData );
bfract_e vc1hwdDecodeBfraction( strmData_t * const strmData,
    i16x * pScaleFactor );

fcm_e vc1hwdDecodeFcm( strmData_t * const strmData );
u16x vc1hwdDecodeCondOver( strmData_t * const strmData );
u16x vc1hwdDecodeRefDist( strmData_t * const strmData );
u16x vc1hwdDecodeDmvRange( strmData_t * const strmData );
intCompField_e vc1hwdDecodeIntCompField( strmData_t * const strmData );

#endif /* #ifndef VC1HWD_VLC_H */
