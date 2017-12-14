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
--  Abstract : Global configurations.
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2hwd_cfg.h,v $
--  $Date: 2007/09/19 10:48:00 $
--  $Revision: 1.6 $
--
------------------------------------------------------------------------------*/

#ifndef _MPEG2DEC_CFG_H
#define _MPEG2DEC_CFG_H

/*
 *  Maximum number of macro blocks in one FRAME
 */
#define MPEG2_MIN_WIDTH   48

#define MPEG2_MIN_HEIGHT  48

#define MPEG2API_DEC_MBS  8160

#define MPEG2DEC_TABLE_SIZE 128

#define MPEG2 1

#define MPEG1 0

#define MAX_OUTPUT_PICS 2

#endif /* already included #ifndef _MPEG2DEC_CFG_H */
