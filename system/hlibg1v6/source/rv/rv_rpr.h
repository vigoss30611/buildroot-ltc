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
--  Abstract: api internal defines
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: rv_rpr.h,v $
--  $Date: 2010/12/13 11:50:41 $
--  $Revision: 1.2 $
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. Internal Definitions
    3. Prototypes of Decoder API internal functions

------------------------------------------------------------------------------*/

#ifndef RV_RPR_H
#define RV_RPR_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "rv_utils.h"

/*------------------------------------------------------------------------------
    2. Internal Definitions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Prototypes of functions
------------------------------------------------------------------------------*/

void rvRpr( picture_t *pSrc,
               picture_t *pDst,
               DWLLinearMem_t *rprWorkBuffer,
               u32 round,
               u32 newCodedWidth, u32 newCodedHeight,
               u32 tiledMode );

#endif /* RV_RPR_H */
