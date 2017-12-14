/**
*******************************************************************************
 @file ci_cameradefaults.c

 @brief 

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K. 

******************************************************************************/
#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "ci/ci_api.h"
#include "ci/ci_modules.h"
#include "ci/ci_api_internal.h" // LOG_ERROR

#include "felix_hw_info.h"

#define LOG_TAG "CI_API"
#include <felixcommon/userlog.h>

/**
 * @ingroup INT_FCT
 *
 * @param val
 * @param bits
 * @param isSigned 1 if signed (to apply -1 on max value) - 0 when unsigned
 */
static IMG_BOOL verif(IMG_INT32 val, IMG_UINT32 bits, IMG_INT isSigned)
{
    if ( val < -(1<<bits) || val > ((1<<bits)-isSigned) )
        return IMG_FALSE;
    return IMG_TRUE;
}

IMG_RESULT CI_ModuleIIF_verif(const CI_MODULE_IIF *pImagerInterface,
    IMG_UINT8 ctx, const CI_HWINFO *pHWInfo)
{
    if ( pImagerInterface->ui16BuffThreshold
        >= pHWInfo->context_aMaxWidthSingle[ctx] )
    {
        LOG_ERROR("The buffer threshold (%d) cannot be bigger than the "\
            "maximum context width-1 (%d-1)\n",
            pImagerInterface->ui16BuffThreshold,
            pHWInfo->context_aMaxWidthSingle[ctx]);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT CI_ModuleLSH_verif(const CI_MODULE_LSH *pLensShading,
    const CI_MODULE_IIF *pImagerInterface, const CI_HWINFO *pHWInfo)
{
    if ( pLensShading == NULL )
    {
        LOG_ERROR("pLensShading is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

#ifndef LSH_NOT_AVAILABLE
    if ( pLensShading->bUseDeshadingGrid == IMG_TRUE )
    {
        int i;
        IMG_UINT16 uiTileSize;
        for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
        {
            if ( pLensShading->matrixDiff[i] == NULL )
            {
                return IMG_ERROR_NOT_SUPPORTED;
            }
        }
    
        if ( pLensShading->ui8BitsPerDiff < LSH_DELTA_BITS_MIN
            || pLensShading->ui8BitsPerDiff > LSH_DELTA_BITS_MAX )
        {
            LOG_ERROR("LSH %u bits per difference is not supported "\
                "(supports [%u ; %u] bits)\n",
                pLensShading->ui8BitsPerDiff,
                LSH_DELTA_BITS_MIN, LSH_DELTA_BITS_MAX);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        uiTileSize = 1<<pLensShading->ui8TileSizeLog2;
        if ( pLensShading->ui16SkipX+1 > uiTileSize
            || pLensShading->ui16SkipY+1 > uiTileSize )
        {
            LOG_ERROR("LSH: skip values %u,%u is too big for the tile size "\
                "%u (must at least hit 1 element per tile)\n",
                pLensShading->ui16SkipX, pLensShading->ui16SkipY, uiTileSize);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        if ( (pLensShading->ui16Width*uiTileSize)
            < (pImagerInterface->ui16ImagerSize[0]+1)
            ||
            (pLensShading->ui16Height*uiTileSize)
            < (pImagerInterface->ui16ImagerSize[1]+1) )
        {
            LOG_ERROR("LSH: grid size %dx%d with tile %d CFA does not cover "\
                "the image processed by the pipeline (%dx%d CFA)\n", 
                pLensShading->ui16Width, pLensShading->ui16Height, uiTileSize,
                (pImagerInterface->ui16ImagerSize[0]+1),
                (pImagerInterface->ui16ImagerSize[1]+1)
            );
            return IMG_ERROR_NOT_SUPPORTED;
        }

        /* TRM: If max_active_width supported is less than 2000 pixels
         * then tile size should not be set to less 16 CFA. If a tile
         * with of 8 CFAs is programmed then the system may stall causing
         * buffer overflows.
         */
        if (pHWInfo->ui32MaxLineStore < 2000 && uiTileSize < 16)
        {
            LOG_ERROR("LSH: tile size of less than 16 is not available "\
                "when max active width (i.e. max line store size) is "\
                "smaller than 2000 - max active width is %u\n",
                pHWInfo->ui32MaxLineStore);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        /* TRM: The deshading grid must not exceed the maximum image
         * dimensions supported by a context
         *
         * note: but we cannot check if more than one context is running
         * so this is checked in kernel-space when updating LSH
         */

        /* BRN56734 all HW including 2.4 should not use matrices that have
         * a line bigger than the burst size or it may lockup the HW */
        //if (pHWInfo->rev_ui8Major == 2 && pHWInfo->rev_ui8Minor < 4)
        {
            // needed bits is: 16 bits + [(width-1)*bitsPerDiff]
            // need rounding [A] to a byte and the whole result to 16 Bytes
            unsigned int uiLineSize =
                (2 + ((pLensShading->ui16Width - 1)
                *pLensShading->ui8BitsPerDiff + 7) / 8);
            uiLineSize = ((uiLineSize + 15) / 16) * 16;

            if (uiLineSize > SYSMEM_ALIGNMENT)
            {
                LOG_ERROR("LSH grid needs %u Bytes to be loaded "\
                    "per line while burst size is %d Bytes\n",
                    uiLineSize, SYSMEM_ALIGNMENT);
                return IMG_ERROR_NOT_SUPPORTED;
            }
        }
    }
#endif
    return IMG_SUCCESS;
}

#if ESC_V_LUMA_TAPS != 8 || DSC_V_LUMA_TAPS != 4
#error "VLuma changed size"
#endif
#if ESC_H_LUMA_TAPS != 16 || DSC_H_LUMA_TAPS != 8
#error "HLuma changed size"
#endif
#if ESC_V_CHROMA_TAPS != 4 || DSC_V_CHROMA_TAPS != 2
#error "VChroma changed size"
#endif
#if ESC_H_CHROMA_TAPS != 8 || DSC_H_CHROMA_TAPS != 4
#error "HCrhoma changed size"
#endif

#define MAX_NTAPS 16
#define TAP 7 // center taps value
static IMG_RESULT fillTaps(IMG_UINT8 *nBits, IMG_UINT32 nTaps)
{
    IMG_UINT8 start = 0, i = 0;
    IMG_UINT8 base[MAX_NTAPS] = {
        TAP-3, TAP-3, TAP-3, TAP-3,
        TAP-2, TAP-2,
        TAP-1, TAP, TAP, TAP-1,
        TAP-2, TAP-2,
        TAP-3, TAP-3, TAP-3, TAP-3};

    if (nTaps>MAX_NTAPS || nTaps<2 || nTaps%2!=0)
        return IMG_ERROR_NOT_SUPPORTED;

    start=(MAX_NTAPS-nTaps)/2;
    if (nTaps==2) start=(MAX_NTAPS-4)/2;

    while (i<nTaps)
    {
        nBits[i]=base[start+i];
        i++;
    }
    
    return IMG_SUCCESS;
}

/**
 * @ingroup INT_FCT
 */
static IMG_BOOL scalerVerifTaps(const IMG_INT8 *matrix, IMG_UINT32 nTaps)
{
    IMG_UINT8 nBits[MAX_NTAPS];
    IMG_UINT32 i, j;
    IMG_RESULT ret;
    
    ret=fillTaps(nBits, nTaps);
    IMG_ASSERT(ret==IMG_SUCCESS);

    for ( j = 0 ; j < (SCALER_PHASES/2) ; j++ )
    {
        for ( i = 0 ; i < nTaps ; i++ )
        {
            if ( verif(matrix[j*nTaps + i], nBits[i],
                nBits[i] == TAP ? 1 : 0) != IMG_TRUE )
            {
                return IMG_FALSE;
            }
        }
    }
    return IMG_TRUE;
}
#undef MAX_NTAPS

IMG_RESULT CI_ModuleScaler_verif(const struct CI_MODULE_SCALER *pScaler)
{
    IMG_UINT32 div = 1;
    IMG_BOOL result = IMG_TRUE;

    if ( pScaler->bBypassScaler == IMG_TRUE ) return IMG_SUCCESS;

    // output size must always be even (register stores width=2*reg+2)
    if ( (2*pScaler->aOutputSize[0]+2)%2 != 0 )
    {
        LOG_ERROR("output width must be even\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    /* encoder scaler, when 420 (ENC_SCAL_OUTPUT_ROWS+1) must be even
     * (so outputSize[1] must be odd) */
    if ( pScaler->bIsDisplay == IMG_FALSE
        && (pScaler->eSubsampling == EncOut420_scaler
         || pScaler->eSubsampling == EncOut420_vss)
        && pScaler->aOutputSize[1]%2 == 0)
    {
        LOG_ERROR("output height must be even\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    
    // display have half number of taps of encoder
    if ( pScaler->bIsDisplay == IMG_TRUE ) div=2;
    
    result &= scalerVerifTaps(pScaler->VLuma, ESC_V_LUMA_TAPS/div);
    result &= scalerVerifTaps(pScaler->HLuma, ESC_H_LUMA_TAPS/div);
    result &= scalerVerifTaps(pScaler->VChroma, ESC_V_CHROMA_TAPS/div);
    result &= scalerVerifTaps(pScaler->HChroma, ESC_H_CHROMA_TAPS/div);
    
    if ( result == IMG_TRUE )
        return IMG_SUCCESS;
    LOG_ERROR("internal error while computing the scaler taps\n");
    return IMG_ERROR_FATAL;
}

IMG_RESULT CI_ModuleTNM_verif(const struct CI_MODULE_TNM *pTNM)
{
    if ( pTNM->bBypassTNM )
    {
        return IMG_SUCCESS; // in bypass the columns are not used
    }
    if ( pTNM->localColWidth[0] < TNM_MIN_COL_WIDTH
        || pTNM->localColWidth[0] > TNM_MAX_COL_WIDTH )
    {
        LOG_ERROR("number pixels per columns not met "\
            "(val=%u min=%u max=%u)\n",
            pTNM->localColWidth[0], TNM_MIN_COL_WIDTH, TNM_MAX_COL_WIDTH);
        return IMG_ERROR_FATAL;
    }
    if ( pTNM->localColumns > TNM_MAX_COL )
    {
        LOG_ERROR("TNM configured to use %u columns but has only %u\n",
            pTNM->localColumns, TNM_MAX_COL);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT CI_ModuleDPF_verif(const struct CI_MODULE_DPF *pDefectivePixel,
    const CI_HWINFO *pHWInfo)
{
    if ( (pDefectivePixel->eDPFEnable & CI_DPF_WRITE_MAP_ENABLED) != 0
        && (pDefectivePixel->eDPFEnable & CI_DPF_DETECT_ENABLED) == 0 )
    {
        LOG_ERROR("Enabling writing of the DPF map without enabling HW "\
            "DPF is not valid\n");
        return IMG_ERROR_FATAL;
    }
    
    if ( pDefectivePixel->sInput.ui32NDefect > 0
        && pDefectivePixel->sInput.apDefectInput == NULL )
    {
        LOG_ERROR("number of defect > 0 while the defect map "\
            "is not allocated!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pDefectivePixel->sInput.apDefectInput != NULL
        && pDefectivePixel->sInput.ui16InternalBufSize
         > pHWInfo->config_ui32DPFInternalSize )
    {
        LOG_ERROR("cannot ask for DPF read size %d - total "\
            "HW read size is %d\n",
            pDefectivePixel->sInput.ui16InternalBufSize,
            pHWInfo->config_ui32DPFInternalSize);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_StatsEXS_verif(const struct CI_MODULE_EXS *pEXS)
{
    // +1 because register expects -1
    if ( (pEXS->ui16Width+1) < EXS_MIN_WIDTH
        || (pEXS->ui16Height+1) < EXS_MIN_HEIGHT )
    {
        LOG_ERROR("EXS sizes are too small (width %u+1 vs min %u - "\
            "height %u+1 vs min %u)\n",
            pEXS->ui16Width, EXS_MIN_WIDTH, pEXS->ui16Height, EXS_MIN_HEIGHT);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT CI_StatsFLD_verif(const struct CI_MODULE_FLD *pFLD)
{
    if ( pFLD->ui8MinPN%4 != 0 || pFLD->ui8PN%4 != 0
        || pFLD->ui8MinPN > pFLD->ui8PN )
    {
        LOG_ERROR("ui8MinPN(%u) or ui8PN(%u) are not multiple of 4 - "\
            "or ui8MinPN is bigger than ui8PN\n",
            pFLD->ui8MinPN, pFLD->ui8PN);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT CI_StatsWBS_verif(const struct CI_MODULE_WBS *pWBS)
{
    // ui8ActiveROI is nb active roi -1
    if ( pWBS->ui8ActiveROI >= WBS_NUM_ROI )
    {
        LOG_ERROR("ui8ActiveROI(%u) is too big - max nb active "\
            "ROI %u (nb ROI %u)\n",
            pWBS->ui8ActiveROI, WBS_NUM_ROI-1, WBS_NUM_ROI);
        return IMG_ERROR_FATAL;
    }

    /// @ could also verify that ROI fits in IIF setup

    return IMG_SUCCESS;
}

IMG_RESULT CI_StatsENS_verif(const struct CI_MODULE_ENS *pENS)
{
    // 0 is legit because hardware adds some value to it
    /*if ( pENS->ui8Log2NCouples == 0 )
    {
        LOG_ERROR("The ENS values ui8Log2NCouples is 0!\n");
        return IMG_ERROR_FATAL;
    }*/
    /// @ add verification that the value does not overflow the registers!
    return IMG_SUCCESS;
}

