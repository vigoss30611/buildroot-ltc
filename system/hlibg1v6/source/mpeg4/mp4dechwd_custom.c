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
--  $RCSfile: mp4dechwd_custom.c,v $
--  $Date: 2010/07/07 12:31:49 $
--  $Revision: 1.8 $
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
#include "mp4decapi_internal.h"

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

static u32 StrmDec_DecodeSusi3Header(DecContainer * pDecContainer);
static u32 DecodeC2(DecContainer * pDecContainer);


u32 DecodeC2(DecContainer * pDecContainer)
{
    u32 tmp;
    tmp = StrmDec_GetBits( pDecContainer, 1 );
    if( tmp == END_OF_STREAM )
        return END_OF_STREAM;
    if( tmp )
    {
        tmp = StrmDec_GetBits( pDecContainer, 1 );
        if( tmp == END_OF_STREAM )
            return END_OF_STREAM;
        return 1+tmp;
    }
    return tmp;
}


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
    u32 tmp;

    tmp = StrmDec_DecodeSusi3Header( pDecContainer );
    if( tmp != HANTRO_OK )
        return DEC_VOP_HDR_RDY_ERROR;

    return DEC_VOP_HDR_RDY;
}

/*------------------------------------------------------------------------------

   5.1  Function name: StrmDec_DecodeSusi3Header

        Purpose: initialize stream decoding related parts of DecContainer

        Input:
            Pointer to DecContainer structure
                -initializes StrmStorage

        Output:
            HANTRO_OK/NOK

------------------------------------------------------------------------------*/
u32 StrmDec_DecodeSusi3Header(DecContainer * pDecContainer)
{
    u32 tmp;
    u32 picType;
    u32 sliceCode;
    u32 rlcTableY, rlcTableC;
    u32 dcTable;
    u32 mvTable;
    u32 skipMbCode = 0;

    pDecContainer->StrmStorage.validVopHeader = HANTRO_FALSE;
    tmp = StrmDec_ShowBits(pDecContainer, 32);

    /* picture_type */
    picType = StrmDec_GetBits(pDecContainer, 2);
    if( picType > 1 )
    {
        return HANTRO_NOK;
    }
    pDecContainer->VopDesc.vopCodingType = ( picType ? PVOP : IVOP );

    /* quant_scale */
    pDecContainer->VopDesc.qP = StrmDec_GetBits(pDecContainer, 5);

    if( picType == 0 ) /* Intra picture */
    {
        sliceCode = StrmDec_GetBits(pDecContainer, 5);
        if( sliceCode < 0x17 )
            return HANTRO_NOK;
        sliceCode -= 0x16;
        rlcTableC = DecodeC2( pDecContainer );
        rlcTableY = DecodeC2( pDecContainer );
        tmp = dcTable = StrmDec_GetBits(pDecContainer, 1);
        pDecContainer->VopDesc.vopRoundingType = 1;
    }
    else /* P picture */
    {
        skipMbCode = StrmDec_GetBits(pDecContainer, 1);
        rlcTableC = rlcTableY =
            DecodeC2( pDecContainer );
        dcTable = StrmDec_GetBits(pDecContainer, 1);
        tmp = mvTable = StrmDec_GetBits(pDecContainer, 1);
        if( pDecContainer->Hdrs.flipFlopRounding )
            pDecContainer->VopDesc.vopRoundingType = 1 -
                pDecContainer->VopDesc.vopRoundingType;
        else
        {
            /* should it be 1 or 0 */
        }
    }

    if( tmp == END_OF_STREAM )
        return END_OF_STREAM;

    if( picType == 0 )
    {
        pDecContainer->Hdrs.numRowsInSlice = 
            pDecContainer->VopDesc.vopHeight / sliceCode;
        if( pDecContainer->Hdrs.numRowsInSlice == 0)
        {
            return (HANTRO_NOK);
        }
    }
    pDecContainer->Hdrs.rlcTableY = rlcTableY;
    pDecContainer->Hdrs.rlcTableC = rlcTableC;
    pDecContainer->Hdrs.dcTable = dcTable;
    pDecContainer->Hdrs.mvTable = mvTable;
    pDecContainer->Hdrs.skipMbCode = skipMbCode;

    pDecContainer->StrmStorage.vpMbNumber = 0;
    pDecContainer->StrmStorage.vpFirstCodedMb = 0;

    /* successful decoding -> set valid vop header */
    pDecContainer->StrmStorage.validVopHeader = HANTRO_TRUE;

    return (HANTRO_OK);
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

    u32 i, tmp;
    u32 bytes = 0;
    u32 SusI = 1147762264;
    u32 uild = ('u' << 24) | ('i' << 16) | ('l' << 8) | 'd';
    /* looking for string "SusINNBuildMMp" or "SusINNbMMp", where NN and MM are
     * version and build (any number of characters) */
    tmp = StrmDec_ShowBits(pDecContainer, 32);
    if(tmp == SusI)
    {
        bytes += 4;
        /* version */
        for (i = 0;; i++)
        {
            tmp = StrmDec_ShowBitsAligned(pDecContainer, 8, bytes);
            if (tmp < '0' || tmp > '9')
                break;
            /* Check 1st character of the version */
            if( i == 0 )
            {
                if( tmp <= '4' )
                    pDecContainer->StrmStorage.customStrmVer = CUSTOM_STRM_1;
                else
                    pDecContainer->StrmStorage.customStrmVer = CUSTOM_STRM_2;
            }
            bytes++;
        }
        if (i == 0)
            return;

        /* "b" or "Build" */
        if (tmp == 'b')
            bytes++;
        else if (tmp == 'B' &&
            StrmDec_ShowBitsAligned(pDecContainer, 8, bytes) == uild)
            bytes += 5;
        else
            return;

        /* build */
        for (i = 0;; i++)
        {
            tmp = StrmDec_ShowBitsAligned(pDecContainer, 8, bytes);
            if (tmp < '0' || tmp > '9')
                break;
            /*build = 10*build + (tmp - '0');*/
            bytes++;
        }

        if (i == 0)
            return;

        if (tmp == 'p')
            pDecContainer->packedMode = 1;
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

    DWLHwConfig_t hwCfg;

/* Code */

    DWLReadAsicConfig(&hwCfg);

    /* CUSTOM_STRM_0 needs to have HW support! */
    if(hwCfg.customMpeg4Support == MPEG4_CUSTOM_NOT_SUPPORTED &&
       pDecCont->StrmStorage.customStrmVer == CUSTOM_STRM_0 )
    {
        pDecCont->StrmStorage.unsupportedFeaturesPresent =
            HANTRO_TRUE;
        return;
    }

    if(hwCfg.customMpeg4Support == MPEG4_CUSTOM_NOT_SUPPORTED ||
       pDecCont->StrmStorage.customStrmVer == 0 )
    {
        pDecCont->StrmStorage.customIdct        = 0;
        pDecCont->StrmStorage.customOverfill    = 0;
        pDecCont->StrmStorage.customStrmHeaders = 0;
        return;
    }

    switch( pDecCont->StrmStorage.customStrmVer )
    {
        case CUSTOM_STRM_0:
            pDecCont->StrmStorage.customIdct        = 0;
            pDecCont->StrmStorage.customOverfill    = 0;
            pDecCont->StrmStorage.customStrmHeaders = 1;
            break;
        case CUSTOM_STRM_1:
            pDecCont->StrmStorage.customIdct        = 1;
            pDecCont->StrmStorage.customOverfill    = 1;
            pDecCont->StrmStorage.customStrmHeaders = 0;
            break;
        default: /* CUSTOM_STRM_2 and higher */
            pDecCont->StrmStorage.customIdct        = 1;
            pDecCont->StrmStorage.customOverfill    = 0;
            pDecCont->StrmStorage.customStrmHeaders = 0;
            break;
    }

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

    if(pDecCont->StrmStorage.customStrmVer == CUSTOM_STRM_0 && 
       pDecCont->VopDesc.vopCodingType == IVOP)
    {
        /* use picture information flag to get rounding type */
        pDecCont->Hdrs.flipFlopRounding = 
            GetDecRegister(pDecCont->mp4Regs, HWIF_DEC_PIC_INF);
        SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_PIC_INF, 0);
    }
}

/*------------------------------------------------------------------------------

    Function: CustomInit

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
    u32 ret;

    mbWidth = (width+15)>>4;
    mbHeight = (height+15)>>4;

    pDecCont->VopDesc.totalMbInVop = mbWidth * mbHeight;
    pDecCont->VopDesc.vopWidth = mbWidth;
    pDecCont->VopDesc.vopHeight = mbHeight;

    pDecCont->Hdrs.lowDelay = HANTRO_TRUE;
    pDecCont->StrmStorage.customStrmVer = CUSTOM_STRM_0;

    pDecCont->StrmStorage.resyncMarkerLength = 17;
    pDecCont->StrmStorage.shortVideo = HANTRO_FALSE;

    pDecCont->VopDesc.vopRoundingType = 0;
    pDecCont->VopDesc.fcodeFwd = 1;
    pDecCont->VopDesc.intraDcVlcThr = 0;
    pDecCont->VopDesc.vopCoded = 1;

    pDecCont->Hdrs.videoObjectLayerWidth = width;
    pDecCont->Hdrs.videoObjectLayerHeight = height;

    pDecCont->Hdrs.vopTimeIncrementResolution = 30000;
    pDecCont->Hdrs.dataPartitioned = HANTRO_FALSE;
    pDecCont->Hdrs.resyncMarkerDisable = HANTRO_TRUE;

    pDecCont->Hdrs.colourPrimaries = 1;
    pDecCont->Hdrs.transferCharacteristics = 1;
    pDecCont->Hdrs.matrixCoefficients = 6;

    pDecCont->StrmStorage.videoObjectLayerWidth = width;
    pDecCont->StrmStorage.videoObjectLayerHeight = height;

    SetDecRegister(pDecCont->mp4Regs, HWIF_PIC_MB_WIDTH,
                   pDecCont->VopDesc.vopWidth);
    SetDecRegister(pDecCont->mp4Regs, HWIF_PIC_MB_HEIGHT_P,
                   pDecCont->VopDesc.vopHeight);
    SetDecRegister(pDecCont->mp4Regs, HWIF_DEC_MODE,
                   MP4_DEC_X170_MODE_MPEG4);
    /* Note: Version 3 uses strictly MB boundary overfill */
    SetDecRegister(pDecCont->mp4Regs, HWIF_MB_WIDTH_OFF, 0);
    SetDecRegister(pDecCont->mp4Regs, HWIF_MB_HEIGHT_OFF, 0);

    SetConformanceFlags( pDecCont );

    RefbuInit( &pDecCont->refBufferCtrl, MP4_DEC_X170_MODE_MPEG4, 
               pDecCont->VopDesc.vopWidth,
               pDecCont->VopDesc.vopHeight,
               pDecCont->refBufSupport );
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

void SetConformanceRegs( DecContainer * pDecContainer )
{
    /* Set correct transform */
    SetDecRegister(pDecContainer->mp4Regs, HWIF_DIVX_IDCT_E, 
        pDecContainer->StrmStorage.customIdct);

    if(pDecContainer->StrmStorage.customStrmVer == CUSTOM_STRM_0)
    {
        SetDecRegister(pDecContainer->mp4Regs, HWIF_DIVX3_E, 1 );
        SetDecRegister(pDecContainer->mp4Regs, HWIF_DIVX3_SLICE_SIZE, 
            pDecContainer->Hdrs.numRowsInSlice);

        SetDecRegister(pDecContainer->mp4Regs, HWIF_TRANSDCTAB,
            pDecContainer->Hdrs.dcTable);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_TRANSACFRM,
            pDecContainer->Hdrs.rlcTableC);
        SetDecRegister(pDecContainer->mp4Regs, HWIF_TRANSACFRM2,
            pDecContainer->Hdrs.rlcTableY);

        if( pDecContainer->VopDesc.vopCodingType == IVOP )
        {
            SetDecRegister(pDecContainer->mp4Regs, HWIF_MVTAB, 0 );
            SetDecRegister(pDecContainer->mp4Regs, HWIF_SKIP_MODE, 0 );
        }
        else
        {
            SetDecRegister(pDecContainer->mp4Regs, HWIF_MVTAB,
                pDecContainer->Hdrs.mvTable);
            SetDecRegister(pDecContainer->mp4Regs, HWIF_SKIP_MODE, 
                pDecContainer->Hdrs.skipMbCode);
        }
        
        SetDecRegister(pDecContainer->mp4Regs, HWIF_TYPE1_QUANT_E, 0 );
        SetDecRegister(pDecContainer->mp4Regs, HWIF_MV_ACCURACY_FWD, 0);
    }
    else
    {
        SetDecRegister(pDecContainer->mp4Regs, HWIF_DIVX3_E, 0 );
    }
       
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
    pDecCont->StrmStorage.sorensonSpark = ( strmFmt == MP4DEC_SORENSON );
    pDecCont->StrmStorage.customStrmVer = ( strmFmt == MP4DEC_CUSTOM_1 ) ? CUSTOM_STRM_1 : 0;
}
