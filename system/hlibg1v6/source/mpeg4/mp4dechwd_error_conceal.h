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
--  $RCSfile: mp4dechwd_error_conceal.h,v $
--  $Date: 2007/11/26 07:51:35 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

#ifndef ERROR_CONCEALMENT_H_DEFINED
#define ERROR_CONCEALMENT_H_DEFINED

#include "mp4dechwd_container.h"

enum
{
    EC_OK,
    EC_NOK
};

u32 StrmDec_ErrorConcealment(DecContainer * pDecCont, u32 start, u32 end);

#endif
