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
--  $RCSfile: mp4dechwd_generic.c,v $
--  $Date: 2009/05/15 08:35:52 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions
        5.1     StrmDec_DecoderInit
        5.2     StrmDec_Decode
        5.3     StrmDec_PrepareConcealment

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mp4dechwd_strmdec.h"
#include "mp4dechwd_utils.h"
#include "mp4dechwd_headers.h"
#include "mp4dechwd_generic.h"
#include "mp4debug.h"
#include "mp4decdrv.h"

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

   5.1  Function name: StrmDec_DecodeSusi3Header

        Purpose: initialize stream decoding related parts of DecContainer

        Input:
            Pointer to DecContainer structure
                -initializes StrmStorage

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/
u32 StrmDec_DecodeCustomHeaders(DecContainer * pDecContainer)
{
    return DEC_VOP_HDR_RDY_ERROR;
}

/*------------------------------------------------------------------------------

   Function name:
                ProcessUserData

        Purpose:

        Input:
                pointer to DecContainer

        Output:
                status (HANTRO_OK/NOK/END_OF_STREAM)

------------------------------------------------------------------------------*/

void ProcessUserData(DecContainer * pDecContainer)
{

    u32 tmp;
    u32 SusI = 1147762264;
    tmp = StrmDec_ShowBits(pDecContainer, 32);
    if(tmp == SusI)
    {
        pDecContainer->StrmStorage.unsupportedFeaturesPresent = HANTRO_TRUE;
    }

}


/*------------------------------------------------------------------------------

    Function: SetConformanceFlags

        Functional description:
            Set some flags to get best conformance to different Susi versions.

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

void SetConformanceFlags( DecContainer * pDecCont )
{

/* Variables */

/* Code */

    if( pDecCont->StrmStorage.customStrmVer )
        pDecCont->StrmStorage.unsupportedFeaturesPresent =
            HANTRO_TRUE;

}



/*------------------------------------------------------------------------------

    Function: ProcessHwOutput

        Functional description:
            Read flip-flop rounding type for Susi3

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

void ProcessHwOutput( DecContainer * pDecCont )
{
    /* Nothing */
}

/*------------------------------------------------------------------------------

    Function: Susi3Init

        Functional description:

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

void SetCustomInfo(DecContainer * pDecCont, u32 width, u32 height ) 
{

    u32 mbWidth, mbHeight;

    mbWidth = (width+15)>>4;
    mbHeight = (height+15)>>4;

    pDecCont->VopDesc.totalMbInVop = mbWidth * mbHeight;
    pDecCont->VopDesc.vopWidth = mbWidth;
    pDecCont->VopDesc.vopHeight = mbHeight;

    pDecCont->StrmStorage.customStrmVer = CUSTOM_STRM_0;

    SetConformanceFlags( pDecCont );

}


/*------------------------------------------------------------------------------

    Function: SetConformanceRegs

        Functional description:

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/

void SetConformanceRegs( DecContainer * pDecContainer )
{

    /* Set correct transform */
    SetDecRegister(pDecContainer->mp4Regs, HWIF_DIVX_IDCT_E, 0 );

}


/*------------------------------------------------------------------------------

    Function: SetStrmFmt

        Functional description:

        Inputs:
            none

        Outputs:
            none

        Returns:
            API version

------------------------------------------------------------------------------*/
void SetStrmFmt( DecContainer * pDecCont, u32 strmFmt )
{
    switch(strmFmt)
    {
        case MP4DEC_MPEG4:
            /* nothing special here */
            break;
        case MP4DEC_SORENSON:
            pDecCont->StrmStorage.sorensonSpark = 1;
            break;
        default:
            pDecCont->StrmStorage.unsupportedFeaturesPresent = HANTRO_TRUE;
            break;
    }
}

 
