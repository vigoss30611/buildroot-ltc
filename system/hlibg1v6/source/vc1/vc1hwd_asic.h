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
--  Description : interface for interaction to hw
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_asic.h,v $
--  $Revision: 1.2 $
--  $Date: 2007/09/03 12:07:45 $
--
------------------------------------------------------------------------------*/

#ifndef VC1HWD_ASIC_H
#define VC1HWD_ASIC_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vc1hwd_container.h"
#include "vc1hwd_stream.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define X170_DEC_TIMEOUT        0xFFU
#define X170_DEC_SYSTEM_ERROR   0xFEU
#define X170_DEC_HW_RESERVED    0xFDU

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

u32 VC1RunAsic(decContainer_t *pDecCont, strmData_t *pStrmData,
    u32 strmBusAddr);

void Vc1DecPpSetPpOutpStandalone( vc1DecPp_t *pDecPp, DecPpInterface *pc );

void Vc1DecPpNextInput( vc1DecPp_t *pDecPp, u32 framePic );

#ifdef _DEC_PP_USAGE
void PrintDecPpUsage( decContainer_t *pDecCont, 
                      u32 ff, 
                      u32 picIndex, 
                      u32 decStatus,
                      u32 picId);
#endif

#endif /* #ifndef VC1HWD_ASIC_H */
