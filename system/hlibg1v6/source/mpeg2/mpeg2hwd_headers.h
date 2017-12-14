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
--  $RCSfile: mpeg2hwd_headers.h,v $
--  $Date: 2007/06/28 07:27:50 $
--  $Revision: 1.3 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of context 

     1. xxx...
   
 
------------------------------------------------------------------------------*/

#ifndef MPEG2STRMDEC_DECODEHEADERS_H
#define MPEG2STRMDEC_DECODEHEADERS_H

#include "basetype.h"
#include "mpeg2hwd_container.h"

u32 mpeg2StrmDec_DecodeExtensionHeader(DecContainer * pDecContainer);
u32 mpeg2StrmDec_DecodeSequenceHeader(DecContainer * pDecContainer);
u32 mpeg2StrmDec_DecodeGOPHeader(DecContainer * pDecContainer);
u32 mpeg2StrmDec_DecodePictureHeader(DecContainer * pDecContainer);

u32 mpeg2StrmDec_DecodeSeqExtHeader(DecContainer * pDecContainer);
u32 mpeg2StrmDec_DecodeSeqDisplayExtHeader(DecContainer * pDecContainer);
u32 mpeg2StrmDec_DecodeQMatrixExtHeader(DecContainer * pDecContainer);
u32 mpeg2StrmDec_DecodePicCodingExtHeader(DecContainer * pDecContainer);
u32 mpeg2StrmDec_DecodePicDisplayExtHeader(DecContainer * pDecContainer);

#endif /* #ifndef MPEG2STRMDEC_DECODEHEADERS_H */
