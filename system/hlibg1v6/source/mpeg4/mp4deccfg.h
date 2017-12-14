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
--  $RCSfile: mp4deccfg.h,v $
--  $Date: 2007/05/28 08:04:41 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

#ifndef _DEC_CFG_H
#define _DEC_CFG_H

/*
 *  Maximum number of macro blocks in one VOP
 */
#define MP4_MIN_WIDTH   48

#define MP4_MIN_HEIGHT  48

#define MP4API_DEC_MBS  8160

/*
 * Size of RLC buffer per macro block
 */
#ifndef _MP4_RLC_BUFFER_SIZE
    #define _MP4_RLC_BUFFER_SIZE 384
#endif


#endif /* already included */
