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
--  Description : Debug Trace funtions
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg4asicdbgtrace.h,v $
--  $Revision: 1.2 $
--  $Date: 2007/09/17 13:25:34 $
--
------------------------------------------------------------------------------*/

#ifndef __MP4DBGTRACE_H__
#define __MP4DBGTRACE_H__
#include <stdio.h>

#include "basetype.h"

void MP4RegDump(const void *dwl);
void WriteAsicCtrl(DecContainer * pDecContainer);
void WriteAsicRlc(DecContainer * pDecContainer, u32 * halves, u32 * numAddr);
void writePictureCtrl(DecContainer * pDecContainer, u32 * halves,
                      u32 * numAddr);
void writePictureCtrlHex( DecContainer *pDecContainer, u32 rlcMode);
#ifdef _DEC_PP_USAGE
void MP4DecPpUsagePrint(DecContainer *pDecCont,
                      u32 ppmode,
                      u32 picIndex,
                      u32 decStatus,
                      u32 picId);
#endif

#endif /* __DBGTRACE_H__ */
