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
--  $RCSfile: mp4dechwd_strmdesc.h,v $
--  $Date: 2007/11/26 09:13:40 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

/*****************************************************************************/

#ifndef DECSTRMDESC_H_DEFINED
#define DECSTRMDESC_H_DEFINED

#include "basetype.h"

typedef struct DecStrmDesc_t
{
    const u8 *pStrmBuffStart; /* pointer to start of stream buffer */
    const u8 *pStrmCurrPos;   /* current read addres in stream buffer */
    u32 bitPosInWord;   /* bit position in stream buffer */
    u32 strmBuffSize;   /* size of stream buffer (bytes) */
    u32 strmBuffReadBits;   /* number of bits read from stream buffer */


    u8 *pUserDataVOS;    /* pointer to VOS user data */
    u32 userDataVOSLen; /* length of VOS user data */
    u32 userDataVOSMaxLen;  /* number of bytes allocated for
                                 * VOS user data */
    u8 *pUserDataVO;
    u32 userDataVOLen;
    u32 userDataVOMaxLen;
    u8 *pUserDataVOL;
    u32 userDataVOLLen;
    u32 userDataVOLMaxLen;
    u8 *pUserDataGOV;
    u32 userDataGOVLen;
    u32 userDataGOVMaxLen;

} DecStrmDesc;

#endif
