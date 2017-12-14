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
--  Description : Jpeg Decoder utils header file
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: jpegdecutils.h,v $
--  $Revision: 1.1 $
--  $Date: 2007/03/30 05:44:50 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of contents 
   
    1. Include headers
    2. Module defines
    3. Data types
    4. Function prototypes

------------------------------------------------------------------------------*/

#ifndef JPEGDECUTILS_H
#define JPEGDECUTILS_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "jpegdeccontainer.h"
#include "basetype.h"

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/
#define STRM_ERROR 0xFFFFFFFFU

#ifndef OK
#define OK 0
#endif
#ifndef NOK
#define NOK -1
#endif
#ifndef STATIC
#define STATIC static
#endif

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

u32 JpegDecGet2Bytes(StreamStorage * pStream);
u32 JpegDecGetByte(StreamStorage * pStream);
u32 JpegDecShowBits(StreamStorage * pStream);
u32 JpegDecFlushBits(StreamStorage * pStream, u32 bits);

#endif /* #ifdef MODULE_H */
