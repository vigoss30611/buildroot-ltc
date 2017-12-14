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
-
-  Description : ...
-
--------------------------------------------------------------------------------
-
-  Version control information, please leave untouched.
-
-  $RCSfile: vp8hwd_headers.h,v $
-  $Revision: 1.3 $
-  $Date: 2010/04/21 13:23:24 $
-
------------------------------------------------------------------------------*/

#ifndef __VP8_HEADERS_H__
#define __VP8_HEADERS_H__

#include "basetype.h"

#include "vp8hwd_decoder.h"
#include "vp8hwd_bool.h"

void vp8hwdDecodeFrameTag( u8 *pStrm, vp8Decoder_t* dec );
u32 vp8hwdSetPartitionOffsets( u8 *stream, u32 len, vp8Decoder_t *dec );
u32 vp8hwdDecodeFrameHeader( u8 *pStrm, u32 strmLen, vpBoolCoder_t*bc, 
                             vp8Decoder_t* dec );

#endif /* __VP8_BOOL_H__ */
