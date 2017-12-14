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
--  Abstract : algorithm header file
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_mbsetdesc.h,v $
--  $Date: 2007/11/26 07:05:05 $
--  $Revision: 1.1 $
------------------------------------------------------------------------------*/

#ifndef DECMBSETDESC_DEFINED
#define DECMBSETDESC_DEFINED

#include "basetype.h"
#include "dwl.h"
#include "mp4decapi.h"

typedef struct DecMbSetDesc_t
{
    u32 *pCtrlDataAddr; /* pointer to asic control bits */
    DWLLinearMem_t ctrlDataMem;
    u32 *pRlcDataAddr;  /* pointer to beginning of asic rlc data */
    DWLLinearMem_t rlcDataMem;
    u32 *pRlcDataCurrAddr;  /* current write address */
    u32 *pRlcDataVpAddr;    /* pointer to rlc data buffer in the
                             * beginning of current video packet */
    u32 *pMvDataAddr;   /* pointer to motion vector data */
    DWLLinearMem_t mvDataMem;

    u32 *pDcCoeffDataAddr;  /* pointer to separately coded DC coeffs */
    DWLLinearMem_t DcCoeffMem;

    u32 rlcDataBufferSize;  /* size of rlc data buffer (u32)  */
    u32 oddRlc; /* half-word left empty from last rlc */
    u32 oddRlcVp;               /* half-word left empty from last rlc */
    
    MP4DecOutput outData;   /* Return PIC info */

} DecMbSetDesc;

#endif
