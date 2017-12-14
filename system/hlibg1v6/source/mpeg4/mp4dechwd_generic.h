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
--  $RCSfile: mp4dechwd_generic.h,v $
--  $Date: 2009/05/15 08:35:52 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of context 


------------------------------------------------------------------------------*/

#ifndef STRMDEC_SUSI_H
#define STRMDEC_SUSI_H

#include "basetype.h"
#include "mp4dechwd_container.h"

u32 StrmDec_DecodeCustomHeaders(DecContainer * pDecContainer);
void ProcessUserData(DecContainer * pDecContainer);
void ProcessHwOutput( DecContainer * pDecCont );
void SetStrmFmt( DecContainer * pDecCont, u32 strmFmt );
void SetConformanceFlags( DecContainer * pDecCont );
void SetConformanceRegs( DecContainer * pDecCont );
void SetCustomInfo(DecContainer * pDecCont, u32 width, u32 height ) ;

#endif /* STRMDEC_SUSI_H */
