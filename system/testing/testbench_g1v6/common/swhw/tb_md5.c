/*------------------------------------------------------------------------------
--                                                                            --
--           This confidential and proprietary software may be used           --
--              only as authorized by a licensing agreement from              --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--      In the event of publication, the following notice is applicable:      --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--    The entire notice above must be reproduced on all authorized copies.    --
--                                                                            --
------------------------------------------------------------------------------*/

#include "tb_md5.h"
#include "md5.h"

/*------------------------------------------------------------------------------

   <++>.<++>  Function: TBWriteFrameMD5Sum

        Functional description:
          Calculates MD5 check sum for the provided frame and writes 
          the result to provided file.

        Inputs:
          
        Outputs:

------------------------------------------------------------------------------*/

u32 TBWriteFrameMD5Sum(FILE* fOut, u8* pYuv, u32 yuvSize, u32 frameNumber) {
    unsigned char digest[16];
    struct MD5Context ctx;
    int i = 0;
    MD5Init(&ctx);
    MD5Update(&ctx, pYuv, yuvSize);
    MD5Final(digest, &ctx);
/*    fprintf(fOut, "FRAME %d: ", frameNumber);*/
    for (i = 0; i < sizeof digest; i++) {
        fprintf(fOut,"%02X", digest[i]);
    } 
    fprintf(fOut,"\n");
    fflush(fOut);
    return 0;
}
