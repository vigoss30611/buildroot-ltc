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
--  Abstract : Stream decoding top level functions (interface functions)
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mpeg2hwd_strm.c,v $
--  $Date: 2008/10/31 14:58:49 $
--  $Revision: 1.10 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     mpeg2StrmDec_Decode

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mpeg2hwd_strm.h"
#include "mpeg2hwd_utils.h"
#include "mpeg2hwd_headers.h"
#include "mpeg2hwd_debug.h"
/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

enum
{
    CONTINUE
};

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   5.1  Function name: mpeg2StrmDec_Decode

        Purpose: Decode MPEG2 stream. Continues decoding until END_OF_STREAM
        encountered or whole frame decoded. Returns after decoding of sequence layer header.
        
        Input:
            Pointer to DecContainer structure
                -uses and updates StrmStorage
                -uses and updates StrmDesc
                -uses and updates FrameDesc
                -uses Hdrs
                -uses MbSetDesc

        Output:
            DEC_RDY if everything was ok but no FRAME finished
            DEC_HDR_RDY if headers decoded
            DEC_HDR_RDY_BUF_NOT_EMPTY if headers decoded but buffer not empty
            DEC_PIC_RDY if whole FRAME decoded
            DEC_PIC_RDY_BUF_NOT_EMPTY if whole FRAME decoded but buffer not empty
            DEC_END_OF_STREAM if eos encountered while decoding
            DEC_ERROR if such an error encountered that recovery needs initial
                      headers

------------------------------------------------------------------------------*/

u32 mpeg2StrmDec_Decode(DecContainer * pDecContainer)
{

    u32 status;
    u32 startCode;

    MPEG2DEC_DEBUG(("Entry StrmDec_Decode\n"));

    status = HANTRO_OK;

    /* keep decoding till something ready or something wrong */
    do
    {
        startCode = mpeg2StrmDec_NextStartCode(pDecContainer);

        /* parse headers */
        switch (startCode)
        {
        case SC_SEQUENCE:
            /* Sequence header */
            status = mpeg2StrmDec_DecodeSequenceHeader(pDecContainer);
            pDecContainer->StrmStorage.validSequence = status == HANTRO_OK;
            if( pDecContainer->StrmStorage.newHeadersChangeResolution)
                return DEC_END_OF_STREAM;
            break;

        case SC_GROUP:
            /* GOP header */
            status = mpeg2StrmDec_DecodeGOPHeader(pDecContainer);
            break;

        case SC_EXTENSION:
            /* Extension headers */
            status = mpeg2StrmDec_DecodeExtensionHeader(pDecContainer);
            if(status == DEC_PIC_HDR_RDY_ERROR)
                return DEC_PIC_HDR_RDY_ERROR;
            break;

        case SC_PICTURE:
            /* Picture header */
            /* decoder still in "initialization" phase and sequence headers
             * successfully decoded -> set to normal state */
            if(pDecContainer->StrmStorage.strmDecReady == FALSE &&
               pDecContainer->StrmStorage.validSequence)
            {
                pDecContainer->StrmStorage.strmDecReady = TRUE;
                pDecContainer->StrmDesc.strmBuffReadBits -= 32;
                pDecContainer->StrmDesc.pStrmCurrPos -= 4;
                return (DEC_HDRS_RDY);
            }
            else if(pDecContainer->StrmStorage.strmDecReady)
            {
                status = mpeg2StrmDec_DecodePictureHeader(pDecContainer);
                if(status != HANTRO_OK)
                    return (DEC_PIC_HDR_RDY_ERROR);
                pDecContainer->StrmStorage.validPicHeader = 1;

                if(pDecContainer->Hdrs.lowDelay &&
                   pDecContainer->Hdrs.pictureCodingType == BFRAME)
                {
                    return (DEC_PIC_SUPRISE_B);
                }
            }
            break;

        case SC_SLICE:
            /* start decoding picture data (HW) if decoder is in normal
             * decoding state and picture headers have been successfully
             * decoded */
            if(pDecContainer->StrmStorage.strmDecReady == TRUE &&
               pDecContainer->StrmStorage.validPicHeader &&
               (!pDecContainer->Hdrs.mpeg2Stream ||
                pDecContainer->StrmStorage.validPicExtHeader))
            {
                /* handle stream positions and return */
                pDecContainer->StrmDesc.strmBuffReadBits -= 32;
                pDecContainer->StrmDesc.pStrmCurrPos -= 4;
                return (DEC_PIC_HDR_RDY);
            }
            break;

        case END_OF_STREAM:
            return (DEC_END_OF_STREAM);

        default:
            break;
        }

    }
    /*lint -e(506) */ while(1);

    /* execution never reaches this point (hope so) */
    /*lint -e(527) */ return (DEC_END_OF_STREAM);
    /*lint -restore */
}
