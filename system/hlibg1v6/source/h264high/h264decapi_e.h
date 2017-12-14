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
--  Description : API extension of H.264 Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264decapi_e.h,v $
--  $Date: 2009/06/08 12:21:05 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

#ifndef __H264DECAPI_E_H__
#define __H264DECAPI_E_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "h264decapi.h"

/*------------------------------------------------------------------------------
    Prototypes of Decoder API extension functions
------------------------------------------------------------------------------*/

    H264DecRet H264DecNextChPicture(H264DecInst decInst,
                                    u32 **pOutputPicture,
                                    u32 *outputPictureBusAddress);


#ifdef __cplusplus
}
#endif

#endif                       /* __H264DECAPI_E_H__ */
