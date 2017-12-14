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
--  $RCSfile: avs_strm.c,v $
--  $Date: 2010/02/04 13:47:41 $
--  $Revision: 1.5 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "avs_strm.h"
#include "avs_utils.h"
#include "avs_headers.h"

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

   5.1  Function name: AvsStrmDec_Decode

        Purpose: Decode AVS stream. Continues decoding until END_OF_STREAM
        encountered or whole frame decoded. Returns after decoding of sequence
        layer header.
        
        Input:
            Pointer to DecContainer structure
                -uses and updates StrmStorage
                -uses and updates StrmDesc
                -uses Hdrs

        Output:

------------------------------------------------------------------------------*/

u32 AvsStrmDec_Decode(DecContainer * pDecContainer)
{

    u32 status;
    u32 startCode;

    AVSDEC_DEBUG(("Entry StrmDec_Decode\n"));

    status = HANTRO_OK;

    /* keep decoding till something ready or something wrong */
    do
    {
        startCode = AvsStrmDec_NextStartCode(pDecContainer);

        /* parse headers */
        switch (startCode)
        {
        case SC_SEQUENCE:
            /* Sequence header */
            status = AvsStrmDec_DecodeSequenceHeader(pDecContainer);
            pDecContainer->StrmStorage.validSequence = status == HANTRO_OK;
            break;

        case SC_EXTENSION:
            /* Extension headers */
            status = AvsStrmDec_DecodeExtensionHeader(pDecContainer);
            break;

        case SC_I_PICTURE:
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
                status = AvsStrmDec_DecodeIPictureHeader(pDecContainer);
                if(status != HANTRO_OK)
                    return (DEC_PIC_HDR_RDY_ERROR);
                pDecContainer->StrmStorage.validPicHeader = 1;

            }
            break;

        case SC_PB_PICTURE:
            /* Picture header */
            status = AvsStrmDec_DecodePBPictureHeader(pDecContainer);
            if(status != HANTRO_OK)
                return (DEC_PIC_HDR_RDY_ERROR);
            pDecContainer->StrmStorage.validPicHeader = 1;

            if(pDecContainer->StrmStorage.sequenceLowDelay &&
               pDecContainer->Hdrs.picCodingType == BFRAME)
            {
                return (DEC_PIC_SUPRISE_B);
            }
            break;

        case SC_SLICE:
            /* start decoding picture data (HW) if decoder is in normal
             * decoding state and picture headers have been successfully
             * decoded */
            if (pDecContainer->StrmStorage.strmDecReady == TRUE &&
                pDecContainer->StrmStorage.validPicHeader)
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
