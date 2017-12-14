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
--  Abstract : 
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_headers.h,v $
--  $Date: 2007/11/26 06:46:48 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of context 

     1. xxx...
   
 
------------------------------------------------------------------------------*/

#ifndef STRMDEC_DECODEHEADERS_H
#define STRMDEC_DECODEHEADERS_H

#include "basetype.h"
#include "mp4dechwd_container.h"

u32 StrmDec_DecodeHdrs(DecContainer * pDecContainer, u32 mode);
u32 StrmDec_DecodeGovHeader(DecContainer *pDecContainer);
void StrmDec_ClearHeaders(DecHdrs * hdrs);
u32 StrmDec_SaveUserData(DecContainer * pDecContainer, u32 u32_mode);
#endif
