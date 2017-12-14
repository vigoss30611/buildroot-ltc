/**
*******************************************************************************
@file mc_convert.c

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
#include "mc/module_config.h"
#include "ci/ci_modules.h"
#include "ci/ci_api_internal.h"
#include "felix_hw_info.h"  // to get the precisions
#include "ctx_reg_precisions.h"
#include "ctx_reg_precisions_2_4.h"
#include "hw_struct/save.h"
#include <math.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif
#include <img_defs.h>
#include <img_errors.h>

#define LOG_TAG "MC_CONVERT"
#include <felixcommon/userlog.h>

/**
 * @ingroup INT_FCT
 * @brief Converts a 2's complement number it a signed integer
 *
 * @param uData is a 2's complement number
 * @param uBits bits wide.
 *
 * @note Function taken from the csim
 */
static IMG_INT32 IMG_ToSigned(IMG_UINT32 uData, IMG_UINT32 uBits)
{
    IMG_UINT32  uMask;
    IMG_UINT32  uRetval;

    IMG_ASSERT(uBits < 32);

    uMask = (1 << uBits) - 1;

    uRetval = uData & uMask;

    if (uData & (1 << (uBits - 1)))
    {
        uRetval |= ~uMask;
    }

    return (uRetval);
}

static void IMG_range(IMG_INT32 nBits, IMG_BOOL isSigned, IMG_INT32 *pMin,
    IMG_INT32 *pMax)
{
    *pMin = (-1 * isSigned)*(1 << (nBits - isSigned));  // 0 if not signed
    *pMax = (1 << (nBits - isSigned)) - 1;
}

/**
 * @ingroup INT_FCT
 */
IMG_INT32 IMG_clip(IMG_INT32 val, IMG_INT32 nBits, IMG_BOOL isSigned,
    const char *dbg_regname)
{
    IMG_INT32 maxValue, minValue, ret;
    IMG_range(nBits, isSigned, &minValue, &maxValue);
    ret = IMG_MAX_INT(minValue, IMG_MIN_INT(val, maxValue));
    if (ret != val)
    {
        LOG_DEBUG("%s clipped from 0x%x to 0x%x\n", dbg_regname, val, ret);
    }
    return ret;
}

/**
 * @ingroup INT_FCT
 */
IMG_INT32 IMG_Fix_Clip(MC_FLOAT fl, IMG_INT32 intBits, IMG_INT32 fractBits,
    IMG_BOOL isSigned, const char *dbg_regname)
{
    int flSign = 1;
    IMG_INT32 conv;
    IMG_INT32 ret;

    if(isSigned && fl < 0.0)
    {
        flSign = -1;
    }

    if(fractBits < 0)
    {
        conv = (IMG_INT32)fl;
        ret = IMG_clip(conv, intBits + 0, isSigned, dbg_regname);
        ret = ret >> abs(fractBits);
    }
    else
    {
        conv = (IMG_INT32)(fl*(IMG_INT64)(1 << fractBits) + 0.5 * flSign);
        ret = IMG_clip(conv, intBits + fractBits, isSigned, dbg_regname);
    }

    return ret;
}

MC_FLOAT IMG_Fix_Revert(IMG_INT32 reg, IMG_INT32 intBits,
    IMG_INT32 fractBits, IMG_BOOL isSigned, const char *dbg_regname)
{
    MC_FLOAT v = 0;
    int max_v = 1 << (intBits + fractBits);
    /*IMG_INT32 reg_v = IMG_clip(reg, intBits + fractBits,
        isSigned, dbg_regname);*/
    IMG_INT32 reg_v = reg;

    // if negative need to compensate for bits
    if (isSigned && reg > (max_v >> 1))
    {
        reg_v = reg_v - max_v;
    }
    if (fractBits < 0)
    {
        v = ((MC_FLOAT)(reg_v)) * (1 << abs(fractBits));
    }
    else
    {
        v = ((MC_FLOAT)(reg_v)) / (1 << fractBits);
    }

    return v;
}

IMG_RESULT MC_IIFConvert(const MC_IIF *pMC_IIF, CI_MODULE_IIF *pCI_IIF)
{
    pCI_IIF->ui8Imager = pMC_IIF->ui8Imager;
    pCI_IIF->eBayerFormat = pMC_IIF->eBayerFormat;

    pCI_IIF->ui16ImagerOffset[0] = pMC_IIF->ui16ImagerOffset[0];
    pCI_IIF->ui16ImagerOffset[1] = pMC_IIF->ui16ImagerOffset[1];

    pCI_IIF->ui16ImagerSize[0] = pMC_IIF->ui16ImagerSize[0] -1;
    pCI_IIF->ui16ImagerSize[1] = pMC_IIF->ui16ImagerSize[1] -1;

    pCI_IIF->ui16ImagerDecimation[0] = pMC_IIF->ui16ImagerDecimation[0];
    pCI_IIF->ui16ImagerDecimation[1] = pMC_IIF->ui16ImagerDecimation[1];

    pCI_IIF->ui8BlackBorderOffset = pMC_IIF->ui8BlackBorderOffset;

    /* ui16BuffThreshold is recomemended to be the width of the image in
     * pixels
     *
     * -1 beacuse the register masks bottom bits and if the value is maximum
     * size for the context
     *
     * AND that maximum size is a power of 2 then the threshold would be
     * masked and considered 0
     */
    pCI_IIF->ui16BuffThreshold =
        ((pMC_IIF->ui16ImagerSize[0])*CI_CFA_WIDTH)-1;
    // ui16ScalerBottomBorder setup in MC_PipelineConvert

    return IMG_SUCCESS;
}



IMG_RESULT MC_BLCConvert(const MC_BLC *pMC_BLC, CI_MODULE_BLC *pCI_BLC)
{
#ifndef BLC_NOT_AVAILABLE

    IMG_MEMSET(pCI_BLC, 0, sizeof(CI_MODULE_BLC));
    pCI_BLC->bBlackFrame = pMC_BLC->bBlackFrame;
    pCI_BLC->bRowAverage = pMC_BLC->bRowAverage;
    pCI_BLC->ui16PixelMax = (IMG_UINT16)IMG_Fix_Clip(pMC_BLC->ui16PixelMax,
        PREC(BLACK_PIXEL_MAX));


    if ( pMC_BLC->bRowAverage == IMG_TRUE)
    {
        if ( pMC_BLC->fRowAverage != 0 )
        {
            pCI_BLC->ui16RowReciprocal =
                (IMG_UINT16)IMG_Fix_Clip(((MC_FLOAT)1.0)/pMC_BLC->fRowAverage,
                PREC(BLACK_ROW_RECIPROCAL));
        }
        else
        {
            pCI_BLC->ui16RowReciprocal = 0;
        }
    }
    else // using fixed values
    {
        IMG_UINT32 i;
        for ( i = 0 ; i < BLC_OFFSET_NO ; i++ )
        {
            /*pCI_BLC->i8FixedOffset[i] = (IMG_INT8)IMG_Fix_Clip(
                pMC_BLC->aFixedOffset[i], PREC(BLACK_FIXED));*/
            //It is aligned in the module so no need to shift it
            pCI_BLC->i8FixedOffset[i] = pMC_BLC->aFixedOffset[i];
        }
    }

#else
    // remove unused warning
    (void)pMC_BLC;
    (void)pCI_BLC;
#endif // BLC_NOT_AVAILABLE
    return IMG_SUCCESS;
}

IMG_RESULT MC_RLTConvert(const MC_RLT *pMC_RLT, CI_MODULE_RLT *pCI_RLT)
{
#ifndef RLT_NOT_AVAILABLE
    IMG_UINT32 i, j;
    IMG_MEMSET(pCI_RLT, 0, sizeof(CI_MODULE_RLT));

    pCI_RLT->eMode = pMC_RLT->eMode;
    for ( i = 0 ; i < RLT_SLICE_N ; i++ )
    {
        for ( j = 0 ; j < RLT_SLICE_N_POINTS ; j++ )
        {
            pCI_RLT->aPoints[i][j] = (IMG_UINT16)IMG_clip(
                pMC_RLT->aPoints[i][j], PREC_C(RLT_LUT_0_POINTS_ODD_1_TO_15));
        }
    }
#else
    // remove unused warning
    (void)pMC_RLT;
    (void)pCI_RLT;
#endif // RLT_NOT_AVAILABLE
    return IMG_SUCCESS;
}

/**
 * @ingroup INT_FCT
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if the difference are not supported
 */
static IMG_RESULT IMG_LSHConvertGrid(const LSH_GRID *pMC_LSH,
    CI_MODULE_LSH *pCI_LSH)
{
#ifndef LSH_NOT_AVAILABLE
    IMG_INT32 c, y, x;
    IMG_INT32 maxDiff = 0;
    const double maxHWGain = (double)(1<<LSH_VERTEX_INT);

    for (c = 0; c < LSH_MAT_NO; c++)
    {
        if ( pMC_LSH->apMatrix[c] == NULL )
        {
            LOG_ERROR("The given LSH grid is NULL\n");
            return IMG_ERROR_INVALID_PARAMETERS;
        }
    }

    pCI_LSH->ui32ClippedValues = 0;
    pCI_LSH->ui16Width = pMC_LSH->ui16Width;
    pCI_LSH->ui16Height = pMC_LSH->ui16Height;
    if ( CI_PipelineAllocLSH(pCI_LSH) != IMG_SUCCESS )
    {
        LOG_ERROR("Allocation of the converted grid failed\n");
        return IMG_ERROR_MALLOC_FAILED;
    }

    for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        for ( y = 0 ; y < pMC_LSH->ui16Height ; y++ )
        {
            IMG_UINT16 curr, prev;
            IMG_INT32 currDiff;

            pCI_LSH->matrixStart[c][y] = (IMG_UINT16)IMG_Fix_Clip(
                pMC_LSH->apMatrix[c][y*pMC_LSH->ui16Width+0],
                PREC(LSH_VERTEX));
            prev = pCI_LSH->matrixStart[c][y];

            for ( x = 1 ; x < pMC_LSH->ui16Width ; x++ )
            {
                if ( pMC_LSH->apMatrix[c][x] < 0.0f && !LSH_VERTEX_SIGNED )
                {
                    LOG_ERROR("The given LSH matrix has negative values!\n");
                    return IMG_ERROR_NOT_SUPPORTED;
                }
                if ( pMC_LSH->apMatrix[c][y*pMC_LSH->ui16Width+x] > maxHWGain )
                {
                    pCI_LSH->ui32ClippedValues++;
                }
                
                curr = (IMG_UINT16)IMG_Fix_Clip(
                    pMC_LSH->apMatrix[c][y*pMC_LSH->ui16Width+x],
                    PREC(LSH_VERTEX));

                currDiff = curr - prev; // so that curr = prev + diff[i]
                pCI_LSH->matrixDiff[c][y*(pMC_LSH->ui16Width-1)+(x-1)] =
                    currDiff;

#if defined(FELIX_FAKE)
                if ( currDiff >= 0
                    && currDiff > ((1<<(LSH_DELTA_BITS_MAX-1))-1)
                    || currDiff < 0
                    && (-1*currDiff) > (1<<(LSH_DELTA_BITS_MAX-1)) )
                {
                    LOG_WARNING("lsh diff c=%d x=%d,y=%d is abs((%f=>%d)-"\
                        "(%f=>%d)) = %d\n",
                        c, x, y,
                        pMC_LSH->apMatrix[c][y*pMC_LSH->ui16Width+x],
                        curr,
                        pMC_LSH->apMatrix[c][y*pMC_LSH->ui16Width+x-1],
                        prev,
                        currDiff
                    );
                }
#endif

                if ( currDiff < 0 )
                {
                    currDiff = (currDiff*-1) -1;
                }

                maxDiff = IMG_MAX_INT(maxDiff, currDiff);

                prev = curr;
            }
        }
    }

    // fill the CI structure

    // 1 to round up result of log2 (e.g. log2(3)=2)
    pCI_LSH->ui8BitsPerDiff = 1;
    if (maxDiff > 0)
    {
        IMG_UINT32 tmp = maxDiff;
        while(tmp >>= 1) // compute log2()+1
        {
            pCI_LSH->ui8BitsPerDiff++;
        }
    }

    // +1 bit for negative values
    // at least LSH_DELTA_BITS_MIN bits
    pCI_LSH->ui8BitsPerDiff = IMG_MAX_INT(pCI_LSH->ui8BitsPerDiff+1,
        LSH_DELTA_BITS_MIN);

#ifdef INFOTM_ENABLE_WARNING_DEBUG
    LOG_INFO("Max difference found: %d - bits per diff chosen %d\n",
        maxDiff, (int)pCI_LSH->ui8BitsPerDiff);
#endif //INFOTM_ENABLE_WARNING_DEBUG

    /* if more than LSH_DELTA_BITS_MAX then CI_PipelineVerify() will fail
     * but will not know what is the max difference value */
    if ( pCI_LSH->ui8BitsPerDiff > LSH_DELTA_BITS_MAX )
    {
        LOG_ERROR("LSH matrix %u bits per difference is not supported "\
            "(max abs diff %u, supports up to %u bits (signed))\n",
            pCI_LSH->ui8BitsPerDiff, maxDiff, LSH_DELTA_BITS_MAX);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    LOG_DEBUG("max diff %d - bits per diff %d\n",
        maxDiff, (int)pCI_LSH->ui8BitsPerDiff);

    // no need to round up because ui16TileSize should be a power of 2
    pCI_LSH->ui8TileSizeLog2 = 0;
    {
        IMG_UINT32 tmp = pMC_LSH->ui16TileSize;

        while ( tmp >>= 1 ) // log2()
        {
            pCI_LSH->ui8TileSizeLog2++;
        }

        if ( pMC_LSH->ui16TileSize != (1<<pCI_LSH->ui8TileSizeLog2) )
        {
            LOG_ERROR("LSH matrix tile size has to be a power of 2 "\
                "(%u given)\n", pMC_LSH->ui16TileSize);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }

    if ( pCI_LSH->ui32ClippedValues > 0 )
    {
        LOG_WARNING("LSH matrix %dx%d was loaded with %d clipped values\n",
            pMC_LSH->ui16Width, pMC_LSH->ui16Height,
            pCI_LSH->ui32ClippedValues);
    }

#endif // LSH_NOT_AVAILABLE
    return IMG_SUCCESS;
}

IMG_RESULT MC_LSHPreCheckTest(MC_PIPELINE *pMCPipeline)
{
    IMG_UINT32 c, x, y;
    IMG_ASSERT(pMCPipeline->sLSH.bUseDeshadingGrid != IMG_FALSE);

    // try to fix the matrix
    for ( c = 0 ; c < LSH_GRADS_NO ; c++ )
    {
        for ( y = 0 ; y < pMCPipeline->sLSH.sGrid.ui16Height ; y++ )
        {
            IMG_UINT16 curr, prev;
            IMG_INT32 currDiff;

            prev = (IMG_UINT16)IMG_Fix_Clip(
                pMCPipeline->sLSH.sGrid.apMatrix[c][y*pMCPipeline->\
                sLSH.sGrid.ui16Width+0], PREC(LSH_VERTEX));

            for ( x = 1 ; x < pMCPipeline->sLSH.sGrid.ui16Width ; x++ )
            {
                int flPos = y * pMCPipeline->sLSH.sGrid.ui16Width + x;
                if (pMCPipeline->sLSH.sGrid.apMatrix[c][x] < 0.0f
                    && !LSH_VERTEX_SIGNED )
                {
                    fprintf(stderr, "WARNING: fixing lsh negative value!\n");
                    pMCPipeline->sLSH.sGrid.apMatrix[c][x] = 0.0f;
                }
                
                curr = (IMG_UINT16)IMG_Fix_Clip(
                    pMCPipeline->sLSH.sGrid.apMatrix[c][flPos],
                    PREC(LSH_VERTEX));
                currDiff = curr - prev;

                if ( (currDiff >= 0
                    && currDiff > ((1<<(LSH_DELTA_BITS_MAX - 2)) -1))
                    ||
                    (currDiff < 0
                    && (-1*currDiff) > (1<<(LSH_DELTA_BITS_MAX - 1))) )
                {
                    MC_FLOAT neo = 0;

                    if ( currDiff < 0 )
                    {
                        neo = pMCPipeline->sLSH.sGrid.apMatrix[c][flPos-1]
                            - (1<<(LSH_DELTA_BITS_MAX-1))
                            /(MC_FLOAT)(1<<LSH_VERTEX_FRAC);
                    }
                    else
                    {
                        neo = pMCPipeline->sLSH.sGrid.apMatrix[c][flPos-1]
                            + ((1<<(LSH_DELTA_BITS_MAX-2))-1)
                            /(MC_FLOAT)(1<<LSH_VERTEX_FRAC);
                    }

                    fprintf(stderr, "WARNING: fixing lsh diff c=%d x=%d,"\
                        "y=%d %f => %f (prev=%f)\n",
                        c, x, y,
                        pMCPipeline->sLSH.sGrid.apMatrix[c][flPos], neo,
                        pMCPipeline->sLSH.sGrid.apMatrix[c][flPos-1]
                    );
                    pMCPipeline->sLSH.sGrid.apMatrix[c]\
                        [y*pMCPipeline->sLSH.sGrid.ui16Width+x] =
                        (LSH_FLOAT)neo;
                    x--;  // redo to be sure
                    continue;
                }

                prev = curr;
            }
        }
    }

    /* we have to do it after fixing the grid as it may fail to
     * convert otherwise */
    /* BRN56734 all HW including 2.4 should not use matrices that have
    * a line bigger than the burst size or it may lockup the HW */
    //if (pHWInfo->rev_ui8Major == 2 && pHWInfo->rev_ui8Minor < 4)
    {
        CI_MODULE_LSH converted;
        IMG_RESULT ret;
        unsigned int uiLineSize = 0;

        IMG_MEMSET(&converted, 0, sizeof(CI_MODULE_LSH));

        ret = MC_LSHConvert(&(pMCPipeline->sLSH), &converted);
        if (ret)
        {
            LOG_ERROR("Failed to convert grid to CI format\n");
            return IMG_ERROR_FATAL;
        }

        uiLineSize = (2 + ((converted.ui16Width - 1)
            * converted.ui8BitsPerDiff + 7) / 8);
        uiLineSize = ((uiLineSize + 15) / 16) * 16;

        if (uiLineSize > SYSMEM_ALIGNMENT)
        {
            LOG_ERROR("LSH grid needs %u Bytes to be loaded "\
                "per line while burst size is %d Bytes\n",
                uiLineSize, SYSMEM_ALIGNMENT);
            return IMG_ERROR_NOT_SUPPORTED;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT MC_LSHConvert(const MC_LSH *pMC_LSH, CI_MODULE_LSH *pCI_LSH)
{
#ifndef LSH_NOT_AVAILABLE
    IMG_UINT32 i;
    IMG_RESULT ret;

    // if pCI_LSH has allocated matrix they are lost here
    CI_PipelineFreeLSH(pCI_LSH);

    IMG_MEMSET(pCI_LSH, 0, sizeof(CI_MODULE_LSH));

    for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
    {
        pCI_LSH->aGradientsX[i] = (IMG_UINT16)IMG_Fix_Clip(
            pMC_LSH->aGradients[i][0], PREC(SHADING_GRADIENT_X));
        pCI_LSH->aGradientsY[i] = (IMG_UINT16)IMG_Fix_Clip(
            pMC_LSH->aGradients[i][1], PREC(SHADING_GRADIENT_Y));
    }

    pCI_LSH->ui16SkipX = pMC_LSH->aSkip[0];
    pCI_LSH->ui16SkipY = pMC_LSH->aSkip[1];

    pCI_LSH->ui16OffsetX = pMC_LSH->aOffset[0];
    pCI_LSH->ui16OffsetY = pMC_LSH->aOffset[1];


    pCI_LSH->bUseDeshadingGrid = pMC_LSH->bUseDeshadingGrid;
    if ( pMC_LSH->bUseDeshadingGrid != IMG_FALSE )
    {
        ret = IMG_LSHConvertGrid(&(pMC_LSH->sGrid), pCI_LSH);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to convert the grid!\n");
        }
        return ret;
    }

#endif // LSH_NOT_AVAILABLE

    return IMG_SUCCESS;
}

IMG_RESULT MC_WBCConvert(const MC_WBC *pMC_WBC, CI_MODULE_WBC *pCI_WBC)
{
#ifndef LSH_NOT_AVAILABLE
    IMG_UINT32 i;

    IMG_MEMSET(pCI_WBC, 0, sizeof(CI_MODULE_WBC));

    for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
    {
        pCI_WBC->aGain[i] = (IMG_UINT16)IMG_Fix_Clip(
            pMC_WBC->aGain[i], PREC(WHITE_BALANCE_GAIN_0));
        pCI_WBC->aClip[i] = (IMG_UINT16)IMG_Fix_Clip(
            pMC_WBC->aClip[i], PREC(WHITE_BALANCE_CLIP_0));
    }

#endif // LSH_NOT_AVAILABLE
    return IMG_SUCCESS;
}

IMG_RESULT MC_DPFConvert(const MC_DPF *pMC_DPF, const MC_IIF *pIIF,
    CI_MODULE_DPF *pCI_DPF)
{
#ifndef DPF_NOT_AVAILABLE
    IMG_UINT32 i;

    // if a map is already loaded it is lost here
    if ( pCI_DPF->sInput.apDefectInput )
    {
        IMG_FREE(pCI_DPF->sInput.apDefectInput);
    }

    IMG_MEMSET(pCI_DPF, 0, sizeof(CI_MODULE_DPF));

    pCI_DPF->eDPFEnable = pMC_DPF->eDPFEnable;

    // copy the memory so that the MC_Pipeline objects can be destroyed

    // output map size converted in MC_PipelineConvert!
    // 0 when not used - computed here when used
    pCI_DPF->sInput.ui16InternalBufSize = 0;

    if ( pMC_DPF->apDefectInput != NULL )
    {
        if ( pMC_DPF->ui32NDefect > 0 )
        {
            pCI_DPF->sInput.ui32NDefect = pMC_DPF->ui32NDefect;

            if ( pCI_DPF->sInput.apDefectInput != NULL )
            {
                IMG_FREE(pCI_DPF->sInput.apDefectInput);
            }
            pCI_DPF->sInput.apDefectInput = (IMG_UINT16*)IMG_CALLOC(
                2*pMC_DPF->ui32NDefect, sizeof(IMG_UINT16));
            if ( pCI_DPF->sInput.apDefectInput == NULL )
            {
                return IMG_ERROR_MALLOC_FAILED;
            }
            IMG_MEMCPY(pCI_DPF->sInput.apDefectInput, pMC_DPF->apDefectInput,
                2*pMC_DPF->ui32NDefect*sizeof(IMG_UINT16));

            /* we need to compute how many defects there is by window here
             * - we could spilt it per Contexts */
            /*pCI_DPF->sInput.ui16InternalBufSize =
                pHWInfo->config_ui32DPFInternalSize
                /(pHWInfo->config_ui8NContexts);*/

            /* computes the size needed for this input map in the internal
             * buffer using the maximum number of defect per vertical window */
            // sould have been initialised!
            IMG_ASSERT(pMC_DPF->ui16NbLines > 0);
            if ( pMC_DPF->ui16NbLines > 0 )
            {
                IMG_UINT32 nWindows =
                    (pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT)
                    /(pMC_DPF->ui16NbLines) +1;
                IMG_UINT32 *nDefects = NULL; // nb of defects per window
                IMG_UINT32 i, maxI;

                nDefects = (IMG_UINT32*)IMG_CALLOC(nWindows,
                    sizeof(IMG_UINT32));

                for ( i = 0 ; i < 2*pMC_DPF->ui32NDefect ; i+=2 )
                {
                    // maxI used as current window
                    maxI = pMC_DPF->apDefectInput[i+1]/(pMC_DPF->ui16NbLines);
                    if ( maxI < nWindows )
                    {
                        nDefects[maxI]++;
                    }
                    else
                    {
                        LOG_ERROR("The given DPF map has more lines than "\
                            "the IIF is setup for! Found y=%d while IIF_"\
                            "height=%d\n",
                                 pMC_DPF->apDefectInput[i+1],
                                 pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT);
                        IMG_FREE(nDefects);
                        return IMG_ERROR_NOT_SUPPORTED;
                    }
                }

                maxI = nDefects[0];
                for ( i = 1 ; i < nWindows ; i++ )
                {
                    maxI = IMG_MAX_INT(maxI, nDefects[i]);
                }

                pCI_DPF->sInput.ui16InternalBufSize = maxI*DPF_MAP_INPUT_SIZE;

                /* BRN49116: DPF read map loading in HW has trouble if a
                 * single line uses the whole setup buffer to avoid that we
                 * add an extra element in the buffer size */
                /* (we could also verify that no line is matching that case
                 * but it is unlikely that a single window will use the
                 * whole available buffer) */
                {
                    pCI_DPF->sInput.ui16InternalBufSize+=DPF_MAP_INPUT_SIZE;
                }

                IMG_FREE(nDefects);
            }
        }
        else
        {
            pCI_DPF->sInput.ui32NDefect = 0;
            printf("%s: number of defect in input map > 0 while the map "\
                "is NULL\n", __FUNCTION__);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
    }

    for ( i = 0 ; i < 2 ; i++ )
    {
        pCI_DPF->aSkip[i] = pMC_DPF->aSkip[i];
        pCI_DPF->aOffset[i] = pMC_DPF->aOffset[i];
    }

    pCI_DPF->ui8Threshold = pMC_DPF->ui8Threshold;
    pCI_DPF->ui8Weight = (IMG_UINT8)IMG_Fix_Clip(pMC_DPF->fWeight,
        PREC(DPF_WEIGHT));
    // output thresholds forced to 0 to output everything
    pCI_DPF->ui8PositiveThreshold = 0;
    pCI_DPF->ui8NegativeThreshold = 0;
#endif

    return IMG_SUCCESS;
}

IMG_RESULT MC_LCAConvert(const MC_LCA *pMC_LCA, CI_MODULE_LCA *pCI_LCA)
{
#ifndef LCA_NOT_AVAILABLE
    IMG_UINT32 i, j;

    for ( i = 0 ; i < 2 ; i++ )
    {
        pCI_LCA->aOffsetRed[i] = pMC_LCA->aOffsetRed[i];
        pCI_LCA->aOffsetBlue[i] = pMC_LCA->aOffsetBlue[i];
        pCI_LCA->aShift[i] = pMC_LCA->aShift[i];
        pCI_LCA->aDec[i] = pMC_LCA->aDec[i];
    }

    for ( i = 0 ; i < LCA_COEFFS_NO ; i++ )
    {
        for ( j = 0 ; j < 2 ; j++ )
        {
            // X or Y does not make a difference
            pCI_LCA->aCoeffRed[i][j] = (IMG_UINT16)IMG_Fix_Clip(
                pMC_LCA->aCoeffRed[i][j], PREC(LAT_CA_COEFF_Y_RED));
            pCI_LCA->aCoeffBlue[i][j] = (IMG_UINT16)IMG_Fix_Clip(
                pMC_LCA->aCoeffBlue[i][j], PREC(LAT_CA_COEFF_Y_BLUE));
        }
    }

#else
    // remove unsued warning
    (void)pMC_LCA;
    (void)pCI_LCA;
#endif // LCA_NOT_AVAILABLE
    return IMG_SUCCESS;
}

IMG_RESULT MC_GMAConvert(const MC_GMA *pMC_GMA, CI_MODULE_GMA *pCI_GMA)
{
    IMG_MEMCPY(pCI_GMA, pMC_GMA, sizeof(CI_MODULE_GMA));
    return IMG_SUCCESS;
}

IMG_RESULT MC_DNSConvert(const MC_DNS *pMC_DNS, CI_MODULE_DNS *pCI_DNS)
{
#ifndef DNS_NOT_AVAILABLE
    IMG_UINT32 i;
    double logv = 0;

    pCI_DNS->bCombineChannels = pMC_DNS->bCombineChannels;

    for ( i = 0 ; i < DNS_N_LUT ; i++ )
    {
        pCI_DNS->aPixThresLUT[i] = (IMG_UINT16)IMG_Fix_Clip(
            pMC_DNS->aPixThresLUT[i], PREC(PIX_THRESH_LUT_POINT_20));
    }
#ifdef USE_MATH_NEON
	logv = (double)logf_neon((float)pMC_DNS->fGreyscalePixelThreshold) / (double)logf_neon(2.0f);
#else
	logv = log(pMC_DNS->fGreyscalePixelThreshold) / log(2.0);
#endif
    pCI_DNS->i8Log2PixThresh = (IMG_INT8)IMG_Fix_Clip(logv,
        PREC(LOG2_GREYSC_PIXTHRESH_MULT));

#endif // DNS_NOT_AVAILABLE
    return IMG_SUCCESS;
}

IMG_RESULT MC_CCMConvert(const MC_CCM *pMC_CCM, CI_MODULE_CCM *pCI_CCM)
{
#ifndef CCM_NOT_AVAILABLE

    IMG_UINT32 i, j;

    for ( i = 0 ; i < RGB_COEFF_NO ; i++ )
    {
        for ( j = 0; j < 4 ; j++ )
        {
            // CC_COEFF_0 or CC_COEFF_1 does not matter here
            pCI_CCM->aCoeff[i][j] = (IMG_INT16)IMG_Fix_Clip(
                pMC_CCM->aCoeff[i][j], PREC(CC_COEFF_0));
        }
        
        /* : Application of precisions is avoided here to match current
         * CSIM setup function parameter format. This must be corrected.*/
        pCI_CCM->aOffset[i] = (IMG_INT16)pMC_CCM->aOffset[i];
    }

#else
    // remove unused warning
    (void)pMC_CCM;
    (void)pCI_CCM;
#endif // CCM_NOT_AVAILABLE
    return IMG_SUCCESS;
}

IMG_RESULT MC_R2YConvert(const MC_R2Y *pMC_R2Y, CI_MODULE_R2Y *pCI_R2Y)
{
    IMG_UINT32 i, j;

    for ( i = 0 ; i < RGB_COEFF_NO ; i++ )
    {
        for ( j = 0; j < YCC_COEFF_NO ; j++ )
        {
            // column does not matter here
            pCI_R2Y->aCoeff[i][j] = (IMG_INT16)IMG_Fix_Clip(
                pMC_R2Y->aCoeff[i][j], PREC(RGB_TO_YCC_COEFF_COL_0));
        }
        pCI_R2Y->aOffset[i] = (IMG_INT16)IMG_Fix_Clip(pMC_R2Y->aOffset[i],
            PREC(RGB_TO_YCC_OFFSET));
    }

    return IMG_SUCCESS;
}

IMG_RESULT MC_Y2RConvert(const MC_Y2R *pMC_Y2R, CI_MODULE_Y2R *pCI_Y2R)
{
    IMG_UINT32 i, j;

    for ( i = 0 ; i < RGB_COEFF_NO ; i++ )
    {
        for ( j = 0; j < YCC_COEFF_NO ; j++ )
        {
            // column does not matter here
            pCI_Y2R->aCoeff[i][j] = (IMG_INT16)IMG_Fix_Clip(
                pMC_Y2R->aCoeff[i][j], PREC(YCC_TO_RGB_COEFF_COL_0));
        }
        pCI_Y2R->aOffset[i] = (IMG_INT16)IMG_Fix_Clip(pMC_Y2R->aOffset[i],
            PREC(YCC_TO_RGB_OFFSET));
    }

    return IMG_SUCCESS;
}

IMG_RESULT MC_MIEConvert(const MC_MIE *pMC_MIE, CI_MODULE_MIE *pCI_MIE,
    const CI_HWINFO *pInfo)
{
#ifndef MIE_NOT_AVAILABLE
    IMG_UINT32 i;
    IMG_BOOL8 b2_4HW = IMG_FALSE;
    int yoff_size = MIE_MC_YOFF_INT + MIE_MC_YOFF_FRAC;
    int yoff_sign = MIE_MC_YOFF_SIGNED;
    int cb0_size = MIE_MC_GAUSS_CB0_INT + MIE_MC_GAUSS_CB0_FRAC;
    int cb0_sign = MIE_MC_GAUSS_CB0_SIGNED;
    int cr0_size = MIE_MC_GAUSS_CR0_INT + MIE_MC_GAUSS_CR0_FRAC;
    int cr0_sign = MIE_MC_GAUSS_CR0_SIGNED;
    
    /*
    * note to customer: knowing your HW this could be done directly
    * as const values
    */
    if (2 == pInfo->rev_ui8Major && 4 == pInfo->rev_ui8Minor)
    {
        b2_4HW = IMG_TRUE;

        yoff_size = HW_2_4_MIE_MC_YOFF_INT + HW_2_4_MIE_MC_YOFF_FRAC;
        yoff_sign = HW_2_4_MIE_MC_YOFF_SIGNED;
        cb0_size = HW_2_4_MIE_MC_GAUSS_CB0_INT + HW_2_4_MIE_MC_GAUSS_CB0_FRAC;
        cb0_sign = HW_2_4_MIE_MC_GAUSS_CB0_SIGNED;
        cr0_size = HW_2_4_MIE_MC_GAUSS_CR0_INT + HW_2_4_MIE_MC_GAUSS_CR0_FRAC;
        cr0_sign = HW_2_4_MIE_MC_GAUSS_CR0_SIGNED;
    }

    pCI_MIE->ui16BlackLevel = (IMG_UINT16)IMG_Fix_Clip(pMC_MIE->fBlackLevel,
        PREC(MIE_BLACK_LEVEL));

    pCI_MIE->bVibrancy = pMC_MIE->bVibrancy;

    for ( i = 0 ; i < MIE_VIBRANCY_N ; i++ )
    {
        pCI_MIE->aVibrancySatMul[i] = (IMG_UINT16)IMG_Fix_Clip(
            pMC_MIE->aVibrancySatMul[i], PREC(MIE_VIB_SATMULT_0));
    }

    pCI_MIE->bMemoryColour = pMC_MIE->bMemoryColour;

    for ( i = 0 ; i < MIE_NUM_MEMCOLOURS ; i++ )
    {
        IMG_INT32 regMin = 0, regMax = 0;
        
        
        IMG_range(yoff_size, yoff_sign, &regMin, &regMax);
        
        pCI_MIE->aYOffset[i] = (IMG_INT16)IMG_clip(
            (IMG_INT32)(pMC_MIE->aYOffset[i] * (regMax - regMin) + regMin),
            yoff_size, yoff_sign, "MIE_MC_YOFF");
        //(IMG_INT16)IMG_Fix_Clip(pMC_MIE->aYOffset[i], PREC(MIE_MC_YOFF));
        if (b2_4HW)
        {
            pCI_MIE->aYScale[i] = (IMG_UINT8)IMG_Fix_Clip(pMC_MIE->aYScale[i],
                PREC(HW_2_4_MIE_MC_YSCALE));
        }
        else
        {
            pCI_MIE->aYScale[i] = (IMG_UINT8)IMG_Fix_Clip(pMC_MIE->aYScale[i],
                PREC(MIE_MC_YSCALE));
        }

        pCI_MIE->aCOffset[i] = (IMG_INT16)IMG_Fix_Clip(pMC_MIE->aCOffset[i],
            PREC(MIE_MC_COFF));
        pCI_MIE->aCScale[i] = (IMG_UINT8)IMG_Fix_Clip(pMC_MIE->aCScale[i],
            PREC(MIE_MC_CSCALE));

        pCI_MIE->aRot[2*i + 0] =
            (IMG_INT8)IMG_Fix_Clip(pMC_MIE->aRot[2*i + 0], PREC(MIE_MC_ROT00));
        pCI_MIE->aRot[2*i + 1] =
            (IMG_INT8)IMG_Fix_Clip(pMC_MIE->aRot[2*i + 1], PREC(MIE_MC_ROT01));

        /* these values are given as a normalise value from 0 to 1 of the
         * register range */
        IMG_range(MIE_MC_GAUSS_MINY_FRAC+MIE_MC_GAUSS_MINY_INT,
            MIE_MC_GAUSS_MINY_SIGNED, &regMin, &regMax);
        pCI_MIE->aGaussMinY[i] = (IMG_UINT16)IMG_clip(
            (IMG_INT32)(pMC_MIE->aGaussMinY[i]*((regMax-regMin) + regMin)),
            MIE_MC_GAUSS_MINY_FRAC+MIE_MC_GAUSS_MINY_INT,
            MIE_MC_GAUSS_MINY_SIGNED, "MIE_MC_GAUSS_MINY");

        pCI_MIE->aGaussScaleY[i] = (IMG_UINT8)IMG_Fix_Clip(
            pMC_MIE->aGaussScaleY[i], PREC(MIE_MC_GAUSS_YSCALE));

        IMG_range(cb0_size, cb0_sign, &regMin, &regMax);
        pCI_MIE->aGaussCB[i] = (IMG_INT8)IMG_clip(
            (IMG_INT32)(pMC_MIE->aGaussCB[i]*(regMax-regMin) + regMin),
            cb0_size, cb0_sign, "MIE_MC_GAUSS_CB0");
        IMG_range(cr0_size, cr0_sign, &regMin, &regMax);
        pCI_MIE->aGaussCR[i] = (IMG_INT8)IMG_clip(
            (IMG_INT32)(pMC_MIE->aGaussCR[i]*(regMax-regMin) + regMin),
            cr0_size, cr0_sign, "MIE_MC_GAUSS_CR0");

        pCI_MIE->aGaussRot[2*i + 0] = (IMG_INT8)IMG_Fix_Clip(
            pMC_MIE->aGaussRot[2*i + 0], PREC(MIE_MC_GAUSS_R00));
        pCI_MIE->aGaussRot[2*i + 1] = (IMG_INT8)IMG_Fix_Clip(
            pMC_MIE->aGaussRot[2*i + 1], PREC(MIE_MC_GAUSS_R01));

        pCI_MIE->aGaussKB[i] = (IMG_UINT8)IMG_Fix_Clip(pMC_MIE->aGaussKB[i],
            PREC(MIE_MC_GAUSS_KB));
        pCI_MIE->aGaussKR[i] = (IMG_UINT8)IMG_Fix_Clip(pMC_MIE->aGaussKR[i],
            PREC(MIE_MC_GAUSS_KR));
    }
    for ( i = 0 ; i < MIE_GAUSS_GN_N ; i++ )
    {
        pCI_MIE->aGaussScale[i] = IMG_clip((IMG_INT32)pMC_MIE->aGaussScale[i],
            MIE_MC_GAUSS_S0_FRAC+MIE_MC_GAUSS_S0_INT, MIE_MC_GAUSS_S0_SIGNED,
            "MIE_MC_GAUSS_S0");
        pCI_MIE->aGaussGain[i] = (IMG_UINT8)IMG_Fix_Clip(
            pMC_MIE->aGaussGain[i], PREC(MIE_MC_GAUSS_G0));
    }

#endif // MIE_NOT_AVAILABLE
    return IMG_SUCCESS;
}

IMG_RESULT MC_TNMConvert(const MC_TNM *pMC_TNM, const MC_IIF *pIIF,
    IMG_UINT8 ui8Paralelism, CI_MODULE_TNM *pCI_TNM)
{
#ifndef TNM_NOT_AVAILABLE
    IMG_UINT32 i;
    MC_FLOAT tmp;

    IMG_MEMSET(pCI_TNM, 0, sizeof(CI_MODULE_TNM));
    pCI_TNM->bBypassTNM = pMC_TNM->bBypassTNM;

    // curve
    for ( i = 0 ; i < TNM_CURVE_NPOINTS ; i++ )
    {
        pCI_TNM->aCurve[i] = (IMG_UINT16)IMG_Fix_Clip(pMC_TNM->aCurve[i],
            PREC(TNM_CURVE_POINT_0));
    }

    pCI_TNM->histFlattenMin = (IMG_UINT16)IMG_Fix_Clip(
        pMC_TNM->histFlattenMin, PREC(TNM_HIST_FLATTEN_MIN));
    pCI_TNM->histFlattenThreshold[0] = (IMG_UINT16)IMG_Fix_Clip(
        pMC_TNM->histFlattenThreshold, PREC(TNM_HIST_FLATTEN_THRES));


    if ( pMC_TNM->histFlattenThreshold == 0.0f )
    {
        pCI_TNM->histFlattenThreshold[1] = (IMG_UINT16)IMG_Fix_Clip(1.0,
            PREC(TNM_HIST_FLATTEN_RECIP));
    }
    else
    {
        pCI_TNM->histFlattenThreshold[1] = (IMG_UINT16)IMG_Fix_Clip(
            (MC_FLOAT)1.0/(1.0-pMC_TNM->histFlattenThreshold),
            PREC(TNM_HIST_FLATTEN_RECIP));
    }

    pCI_TNM->localColWidth[0] = (IMG_UINT16)ceil(
        ((MC_FLOAT)pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH)
        /pMC_TNM->ui16LocalColumns);
    if ( pCI_TNM->localColWidth[0] != 0 )
    {
        pCI_TNM->localColWidth[1] = (IMG_UINT16)IMG_Fix_Clip(
            (MC_FLOAT)1.0/pCI_TNM->localColWidth[0],
            PREC(TNM_LOCAL_COL_WIDTH_RECIP));
    }
    else
    {
        pCI_TNM->localColWidth[1] = 0;
    }

    tmp = 1.0/(pCI_TNM->localColWidth[0]*ui8Paralelism);
    pCI_TNM->histNormFactor = (IMG_UINT16)IMG_Fix_Clip(tmp,
        PREC(TNM_HIST_NORM_FACTOR));

    tmp = 1.0/((pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH
        -pCI_TNM->localColWidth[0]*(pMC_TNM->ui16LocalColumns-1))
        *ui8Paralelism);
    pCI_TNM->histNormLastFactor = (IMG_UINT16)IMG_Fix_Clip(tmp,
        PREC(TNM_HIST_NORM_FACTOR_LAST));

    pCI_TNM->chromaSaturationScale = (IMG_UINT16)IMG_Fix_Clip(
        pMC_TNM->chromaSaturationScale, PREC(TNM_CHR_SAT_SCALE));
    pCI_TNM->chromaConfigurationScale = (IMG_UINT16)IMG_Fix_Clip(
        pMC_TNM->chromaConfigurationScale, PREC(TNM_CHR_CONF_SCALE));
    pCI_TNM->chromaIOScale = (IMG_UINT16)IMG_Fix_Clip(pMC_TNM->chromaIOScale,
        PREC(TNM_CHR_IO_SCALE));

    pCI_TNM->localColumns = pMC_TNM->ui16LocalColumns-1;
    pCI_TNM->columnsIdx = pMC_TNM->ui16ColumnsIndex;

    pCI_TNM->localWeights = (IMG_UINT16)IMG_Fix_Clip(pMC_TNM->localWeights,
        PREC(TNM_LOCAL_WEIGHT));
    pCI_TNM->updateWeights = (IMG_UINT16)IMG_Fix_Clip(pMC_TNM->updateWeights,
        PREC(TNM_CURVE_UPDATE_WEIGHT));

    pCI_TNM->inputLumaOffset = (IMG_UINT16)IMG_Fix_Clip(
        pMC_TNM->inputLumaOffset, PREC(TNM_INPUT_LUMA_OFFSET));
    pCI_TNM->inputLumaScale = (IMG_UINT16)IMG_Fix_Clip(
        pMC_TNM->inputLumaScale, PREC(TNM_INPUT_LUMA_SCALE));

    pCI_TNM->outputLumaOffset = (IMG_UINT16)IMG_Fix_Clip(
        pMC_TNM->outputLumaOffset, PREC(TNM_OUTPUT_LUMA_OFFSET));
    pCI_TNM->outputLumaScale = (IMG_UINT16)IMG_Fix_Clip(
        pMC_TNM->outputLumaScale, PREC(TNM_OUTPUT_LUMA_SCALE));

#else
    // to remove warnings
    (void)pMC_TNM;
    (void)pCI_TNM;
#endif // TNM_NOT_AVAILABLE
    return IMG_SUCCESS;
}

/*
 * stolen taps computation code
 */

#define _USE_MATH_DEFINES // for win32 M_PI definition
#include <math.h>
/** Numerator of base phase shift. */
#define SCALER_BASE_PHASE_SHIFT_NUMERATOR 1.0
// A constant 1/16 phase shift is applied on top
// of the vertical/horizontal alignment based phase.
// Denominator of base phase shift.
#define SCALER_BASE_PHASE_SHIFT_DENOMINATOR 16.0

#define NUM_BESSEL_IT 10

/**
 * @ingroup INT_FCT
 *
 * @note from CSim setup functions
 */
static double IMG_ModifiedBessel(double x, IMG_BOOL xIsImaginary)
{
    /* Estimate the "zeroth order modified Bessel function of the first kind".
     * In theory, this is an infinite sum; however in practice, the sum
     * converges quite quickly for small x (say, x < 10).
     * For small values of x, 10 samples is probably plenty to reach
     * convergence to the degree of precision required: */
    const int NUM_ITERATIONS = 10;
    double bessel_I0 = 0;
    double myFactorial = 1; // 0! (factorial of 0) is always equal to 1.
	float temp;
	int m;

    if (x == 0) return 1.0; // easy case

    for (m = 0; m < NUM_ITERATIONS; m++) {
        if (xIsImaginary) {
            // If m is odd, result of the pow() call is negative
            if ((m & 1)==0) {
#ifdef USE_MATH_NEON
				temp = powf_neon((float)x, (float)m);
				bessel_I0 += (double)((temp * temp) / (1 << (2*m))) / (myFactorial*myFactorial);
#else
				bessel_I0 += pow(x/2,(2*m)) / (myFactorial*myFactorial);
#endif
			} else {
#ifdef USE_MATH_NEON
				temp = powf_neon((float)x, (float)m);
				bessel_I0 -= (double)((temp * temp) / (1 << (2*m))) / (myFactorial*myFactorial);
#else
				bessel_I0 -= pow(x/2,(2*m)) / (myFactorial*myFactorial);
#endif
			}
		} else {
#ifdef USE_MATH_NEON
			temp = powf_neon((float)x, (float)m);
			bessel_I0 += (double)((temp * temp) / (1 << (2*m))) / (myFactorial*myFactorial);
			//bessel_I0 += (double)powf_neon((float)x/2,(float)(2*m)) / (myFactorial*myFactorial);
#else
			bessel_I0 += (double)pow(x/2,(2*m)) / (myFactorial*myFactorial);
#endif
        }
        myFactorial = myFactorial * (m+1);
    }

    return bessel_I0;
}

/**
 * @ingroup INT_FCT
 *
 * @note from CSim setup functions
 */
static double IMG_Round(double x) {
    /* Function to do actual rounding, symmetrical around 0. Typically used
     * just before a cast to IMG_INT
     * #define round(x)  ( (x >= 0 ? 1 : -1) * floor(abs(x + 0.5)) )
     * ^ Could have used this macro instead, but behaviour for data types
     * other than double/float is not clear,
     *   so better to type-check.*/
    return (x >= 0 ? 1 : -1) * floor(fabs(x) + 0.5);
}

/**
 * @ingroup INT_FCT
 *
 * @note Stolen from CSim setup functions
 *
 * @param numTaps must be an even number smaller or equal than ESC_H_LUMA_TAPS
 * @param pitch ratio input/output
 * @param coefficientMatrix output array of taps values
 * @param enable_cutoff_frequency_adjustment compute a cutoff frequency or
*  use the default (1.0)
 */
static void IMG_CalculateScalerCoefficients(IMG_UINT8 numTaps,
    double pitch, IMG_INT8 *coefficientMatrix,
    IMG_BOOL8 enable_cutoff_frequency_adjustment)
{
    // According to Scaler functional spec, beta is recommended to be 2 for
    // "medium and high" scaling factors, and 3 for "small scaling factors
    // (below 1.6)".
    // (Scaling factor is assumed to mean pitch, here.)
    double beta;
    double cutoff_frequency_adjustment = 1.0; // default

    // Calculate the required cut-off frequency (in radians per pixel):
    double w_c = 0.0;

    double filterTaps[ESC_H_LUMA_TAPS*SCALER_PHASES/2];
    double sincFcn[ESC_H_LUMA_TAPS*SCALER_PHASES/2];
    double kaiserWindow[ESC_H_LUMA_TAPS*SCALER_PHASES/2];

    double phase;
    double sumOfCoeffs[SCALER_PHASES/2];

    // defined in loops
    int p;
    double t;
    int n;

    // Running sum of all rounding errors for this coefficient set
    double currentRoundingError = 0;
    IMG_INT sumOfCoeffs_int = 0;

    // Number of samples of the Kaiser window
    IMG_INT nKaiserWindow =
        (IMG_INT)SCALER_BASE_PHASE_SHIFT_DENOMINATOR * numTaps;

    if (pitch < 1.6) {
        beta = 3;
    } else {
        beta = 2;
    }

    if (enable_cutoff_frequency_adjustment) {
        // Adapt cutoff_frequency_adjustment to the number of available taps
        // (The given cutoff_frequency_adjustment applies to 16 taps.)
#ifdef USE_MATH_NEON
		cutoff_frequency_adjustment = 1.0/(double)sqrtf_neon((float)pitch);
        cutoff_frequency_adjustment =
            (double)powf_neon((float)cutoff_frequency_adjustment,(float)16.0/numTaps);
#else
		cutoff_frequency_adjustment = 1/sqrt(pitch);
		cutoff_frequency_adjustment =
            pow(cutoff_frequency_adjustment,16.0/numTaps);
#endif
    }

    w_c = M_PI/pitch * cutoff_frequency_adjustment;

    /* Compute numTaps samples of a Kaiser window for each phase
     * The Kaiser window has to be phase-shifted to match the phase of the
     * Sinc function.
     *
     * We need <numTaps> samples of the Kaiser window per phase, and there
     * are SCALER_PHASES phases => numTaps*SCALER_PHASES samples in total.
     * But because the base phase shift = 1/16, we need to twice that many
     * samples of the Kaiser window (2*numTaps*SCALER_PHASES) to get data
     * points on the Kaiser window that correspond to a 1/16 phase offset
     * (even though only every 2nd sample produced would be used, if we
     * actually calculated all of them).
     *
     * Example:
     *    numTaps = 8,
     *    SCALER_PHASES = 8,
     *    base phase shift = 1/16
     *    = SCALER_BASE_PHASE_SHIFT_NUMERATOR
     *        /SCALER_BASE_PHASE_SHIFT_DENOMINATOR
     *      Desired phases: 1/16, 3/16, 5/16, 7/16, 9/16, 11/16, 13/16,
     * 15/16; but 9/16 through 15/16 are mirror images of 1/16 through 7/16.
     *
     * We want 8*8 = 64 samples of a Kaiser window - 8 for each phase.
     * But the 1/16 phase offset means we need to adjust the Kaiser window
     * for the phase offset. The formula for the Kaiser window does
     * not have a variable for phase, but it does allow you to determine
     * the total number of samples (the resolution) of the Kaiser window
     * you want to compute. So we just need to make sure there are 16/8=2
     * times as many samples. So we set up the Kaiser window formula to
     * compute all 16 phase offsets (1/16,2/16,3/16,...,14/16,15/16)
     * and simply don't bother computing the even samples (0/16, 2/16, 4/16,
     * ... , 14/16).
     */

    for(p = 0; p < SCALER_PHASES/2; p++) { // For each phase...
        sumOfCoeffs[p] = 0; // Initialize to 0
        phase = ((double)p)/SCALER_PHASES; // current phase
        if (pitch != 1.0)
        {
            // Unless the pitch is 1.0, the phase is always offset
            phase += SCALER_BASE_PHASE_SHIFT_NUMERATOR
                / SCALER_BASE_PHASE_SHIFT_DENOMINATOR;
        }
        // by 1/16 (this makes it possible to mirror the taps from
        // 4 phases to make 8 different sets of coefficients)

        /* Subtract 0.5 to compensate for the 1/2-pixel offset that is
         * naturally caused by having a filter with an even number of taps */
        phase -= 0.5;
        for (t = -((double)(numTaps/2) - 0.5);
            t <= ((double)(numTaps/2) - 0.5); t += 1.0 ) { // For each tap...
            /* t is the offset of the filter tap relative to the centre of
             * the filter (the centre of the filter is always at t==0).
             * Here, t = ... -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5 ...
             * The 0.5 offset is there because there are an even number of
             * taps.*/
            IMG_INT index = (IMG_INT)(t+((double)(numTaps/2) - 0.5));

#ifdef USE_MATH_NEON
            double bessel_input = 1.0-(double)powf_neon(2.0f*(float)((t-phase)+((numTaps)-1.0f)/2.0f)
                /(float)(numTaps)-1.0f,2.0f);
			//double bessel_input = 1.0 - (double)powf_neon((2.0f*(float)(t-phase)-1.0f)/(float)numTaps, 2.0f);
			kaiserWindow[p*numTaps + index] = IMG_ModifiedBessel(
                beta*(double)sqrtf_neon((float)fabs(bessel_input)), (IMG_BOOL)bessel_input < 0)
                / IMG_ModifiedBessel(beta, beta < 0);
			// Sinc function with a cut-off frequency of w_c
			sincFcn[p*numTaps + index] = (t-phase != 0) ? (double)sinf_neon((float)(w_c*(t-phase))) / (M_PI*(t-phase)) : (w_c/M_PI);
#else
            double bessel_input = 1-pow(2*((t-phase)+((double)(numTaps)-1)/2)
                /(double)(numTaps)-1,2.0);
            kaiserWindow[p*numTaps + index] = IMG_ModifiedBessel(
                beta*sqrt(fabs(bessel_input)), (IMG_BOOL)bessel_input < 0)
                / IMG_ModifiedBessel(beta, beta < 0);
			// Sinc function with a cut-off frequency of w_c
			sincFcn[p*numTaps + index] = (t-phase != 0) ? sin(w_c*(t-phase)) / (M_PI*(t-phase)) : (w_c/M_PI);
#endif
            /* ^ Note that if (t-phase) is ever 0 (e.g. if t=0 and
             * SCALER_BASE_PHASE_SHIFT=0), evaluating sin(w_c*(t-phase))
             * / (PI*(t-phase)) results in a divide by zero. In this case,
             * filterTaps[index] should be set to w_c/pi - this is the
             * mathematical limit of sinc(w) as w (the angular frequency)
             * approaches 0. */

            filterTaps[p*numTaps + index] = sincFcn[p*numTaps + index]
                * kaiserWindow[p*numTaps + index]; // Apply the Kaiser window
            sumOfCoeffs[p] += filterTaps[p*numTaps + index];
        }

        // Normalize the coefficients
        for (n = 0; n < numTaps; n++) {
            filterTaps[p*numTaps + n] =
                filterTaps[p*numTaps + n]/sumOfCoeffs[p];
        }

        /* Round to fixed-point, and attempt to balance the rounding errors
         * (to minimize overall error due to rounding of the coefficients).
         * This helps avoid introducing an unwanted DC offset due to rounding
         * A DC offset is introduced if the majority of the coefficients
         * happen to get rounded up (or down). */

        sumOfCoeffs_int = 0;
        for (n = 0; n < numTaps/2; n++) {
            // ^ Do them in pairs, to avoid skewing the filter shape too much

            // Left-side coefficient
            coefficientMatrix[p*numTaps + n] =
                (IMG_INT)(IMG_Round(filterTaps[p*numTaps + n]
                * (1 << SCALER_TAPS_FRAC) - currentRoundingError));
            currentRoundingError +=
                ((double)(coefficientMatrix[p*numTaps + n])
                - filterTaps[p*numTaps + n]*(1 << SCALER_TAPS_FRAC));

            // Right-side coefficient
            coefficientMatrix[p*numTaps + (numTaps-n-1)] =
                (IMG_INT)(IMG_Round(filterTaps[p*numTaps + (numTaps-n-1)]
                * (1 << SCALER_TAPS_FRAC) - currentRoundingError));
            currentRoundingError +=
                ((double)(coefficientMatrix[p*numTaps + (numTaps-n-1)])
                - filterTaps[p*numTaps + (numTaps-n-1)]
                *(1 << SCALER_TAPS_FRAC));

            sumOfCoeffs_int += coefficientMatrix[p*numTaps + n]
                + coefficientMatrix[p*numTaps + (numTaps-n-1)];
        }
        if (sumOfCoeffs_int != (1 << SCALER_TAPS_FRAC)) {
            /* There's still a DC offset (should be no greater than +/-1)
             * Add/subtract it from one of the central coefficients (e.g.
             * one of the largest coefficients, which will be least
             * affected by the change) */
            coefficientMatrix[p*numTaps + numTaps/2] +=
                (1 << SCALER_TAPS_FRAC) - sumOfCoeffs_int;
        }
    }
}

/*
 * end of csim code
 */

IMG_RESULT MC_ESCConvert(const MC_ESC *pMC_ESC, CI_MODULE_ESC *pCI_ESC)
{
    pCI_ESC->bBypassScaler = pMC_ESC->bBypassESC;

    if ( pMC_ESC->aOutputSize[0] > 1)
    {
        pCI_ESC->aOutputSize[0] = (pMC_ESC->aOutputSize[0]-2)/2;
    }
    else
    {
        pCI_ESC->aOutputSize[0] = 0;
    }

    if ( pMC_ESC->aOutputSize[1] > 0 )
    {
        pCI_ESC->aOutputSize[1] = pMC_ESC->aOutputSize[1]-1;
    }
    else
    {
        pCI_ESC->aOutputSize[1] = 0;
    }

    pCI_ESC->aOffset[0] = pMC_ESC->aOffset[0];
    pCI_ESC->aOffset[1] = pMC_ESC->aOffset[1];

    if ( pMC_ESC->aPitch[0] < 1.0f || pMC_ESC->aPitch[1] < 1.0f )
    {
        LOG_ERROR("upsampling not supported: pitch < 1.0 is not valid "\
            "(given: %f %f)\n", pMC_ESC->aPitch[0], pMC_ESC->aPitch[1]);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pCI_ESC->aPitch[0] = (IMG_UINT32)IMG_Fix_Clip(pMC_ESC->aPitch[0],
        PREC(ESC_H_PITCH));
    pCI_ESC->aPitch[1] = (IMG_UINT32)IMG_Fix_Clip(pMC_ESC->aPitch[1],
        PREC(ESC_V_PITCH));

    pCI_ESC->bChromaInter = pMC_ESC->bChromaInter;
    pCI_ESC->eSubsampling = pMC_ESC->eSubsampling;

    /*
     * compute taps
     */
    if ( pMC_ESC->bBypassESC == IMG_FALSE)
    {
        double mult = 1.0;
        IMG_CalculateScalerCoefficients(ESC_H_LUMA_TAPS, pMC_ESC->aPitch[0],
            pCI_ESC->HLuma, pMC_ESC->bAdjustCutoff);
        IMG_CalculateScalerCoefficients(ESC_V_LUMA_TAPS, pMC_ESC->aPitch[1],
            pCI_ESC->VLuma, pMC_ESC->bAdjustCutoff);

        if ( pMC_ESC->eSubsampling == EncOut420_scaler )
        {
            /* consider V pitch as double to have a correct 420
             * subsampling by the scaler*/
            mult=2.0;
        }

        IMG_CalculateScalerCoefficients(ESC_H_CHROMA_TAPS,
            pMC_ESC->aPitch[0], pCI_ESC->HChroma, pMC_ESC->bAdjustCutoff);
        IMG_CalculateScalerCoefficients(ESC_V_CHROMA_TAPS,
            pMC_ESC->aPitch[1]*mult, pCI_ESC->VChroma,
            pMC_ESC->bAdjustCutoff);


        // verify the taps configuration only
        if (CI_ModuleScaler_verif(pCI_ESC) == IMG_ERROR_FATAL)
        {
            LOG_ERROR("failed to configure ESC taps\n");
            return IMG_ERROR_FATAL;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT MC_DSCConvert(const MC_DSC *pMC_DSC, CI_MODULE_DSC *pCI_DSC)
{
    if ( pMC_DSC->aOutputSize[0] > 1)
    {
        pCI_DSC->aOutputSize[0] = (pMC_DSC->aOutputSize[0]-2)/2;
    }
    else
    {
        /* not an error if not DSC output... may need a better way to find
         * errors */
        //LOG_ERROR("output size is too small! Need at least 2 rows\n");
        //return IMG_ERROR_NOT_SUPPORTED;
        pCI_DSC->aOutputSize[0] = 0;
    }
    if ( pMC_DSC->aOutputSize[1] > 0 )
    {
        pCI_DSC->aOutputSize[1] = pMC_DSC->aOutputSize[1]-1;
    }
    else
    {
        //LOG_ERROR("output size is too small! Need at least 1 line\n");
        //return IMG_ERROR_NOT_SUPPORTED;
        pCI_DSC->aOutputSize[1] = 0;
    }

    pCI_DSC->aOffset[0] = pMC_DSC->aOffset[0];
    pCI_DSC->aOffset[1] = pMC_DSC->aOffset[1];

    pCI_DSC->bBypassScaler = pMC_DSC->bBypassDSC;

    if ( pMC_DSC->aPitch[0] < 1.0f || pMC_DSC->aPitch[1] < 1.0f )
    {
        LOG_ERROR("upsampling not supported: pitch < 1.0 is not valid "\
            "(given: %f %f)\n", pMC_DSC->aPitch[0], pMC_DSC->aPitch[1]);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pCI_DSC->aPitch[0] = (IMG_UINT32)IMG_Fix_Clip(pMC_DSC->aPitch[0],
        PREC(DSC_H_PITCH));
    pCI_DSC->aPitch[1] = (IMG_UINT32)IMG_Fix_Clip(pMC_DSC->aPitch[1],
        PREC(DSC_V_PITCH));

    /*
     * compute taps
     */
    if ( pMC_DSC->bBypassDSC == IMG_FALSE)
    {
        IMG_CalculateScalerCoefficients(DSC_H_LUMA_TAPS, pMC_DSC->aPitch[0],
            pCI_DSC->HLuma, pMC_DSC->bAdjustCutoff);
        IMG_CalculateScalerCoefficients(DSC_V_LUMA_TAPS, pMC_DSC->aPitch[1],
            pCI_DSC->VLuma, pMC_DSC->bAdjustCutoff);

        IMG_CalculateScalerCoefficients(DSC_H_CHROMA_TAPS, pMC_DSC->aPitch[0],
            pCI_DSC->HChroma, pMC_DSC->bAdjustCutoff);
        IMG_CalculateScalerCoefficients(DSC_V_CHROMA_TAPS, pMC_DSC->aPitch[1],
            pCI_DSC->VChroma, pMC_DSC->bAdjustCutoff);

        // verify the taps configuration only
        if (CI_ModuleScaler_verif(pCI_DSC) == IMG_ERROR_FATAL)
        {
            LOG_ERROR("failed to configure DSC taps\n");
            return IMG_ERROR_FATAL;
        }
    }
    return IMG_SUCCESS;
}


IMG_RESULT MC_MGMConvert(const MC_MGM *pMC_MGM, CI_MODULE_MGM *pCI_MGM)
{
#ifndef MGM_NOT_AVAILABLE
    IMG_UINT32 i;

    pCI_MGM->i16ClipMin = (IMG_INT16)IMG_Fix_Clip(pMC_MGM->fClipMin*MGM_MULT,
        PREC(MGM_CLIP_MIN));
    pCI_MGM->ui16ClipMax = (IMG_INT16)IMG_Fix_Clip(pMC_MGM->fClipMax*MGM_MULT,
        PREC(MGM_CLIP_MAX));
    pCI_MGM->ui16SrcNorm = (IMG_UINT16)IMG_Fix_Clip(
        pMC_MGM->fSrcNorm*MGM_MULT, PREC(MGM_SRC_NORM));

    for ( i = 0 ; i < MGM_N_SLOPE ; i++ )
    {
        pCI_MGM->aSlope[i] = (IMG_UINT16)IMG_Fix_Clip(pMC_MGM->aSlope[i],
            PREC(MGM_SLOPE_0));
    }
    for ( i = 0 ; i < MGM_N_COEFF ; i++ )
    {
        pCI_MGM->aCoeff[i] = (IMG_INT16)IMG_Fix_Clip(pMC_MGM->aCoeff[i],
            PREC(MGM_COEFF_0));
    }
#endif /* MGM_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_DGMConvert(const MC_MGM *pMC_DGM, CI_MODULE_MGM *pCI_DGM)
{
#ifndef DGM_NOT_AVAILABLE
    IMG_UINT32 i;

    pCI_DGM->i16ClipMin = (IMG_INT16)IMG_Fix_Clip(pMC_DGM->fClipMin*DGM_MULT,
        PREC(DGM_CLIP_MIN));
    pCI_DGM->ui16ClipMax = (IMG_INT16)IMG_Fix_Clip(pMC_DGM->fClipMax*DGM_MULT,
        PREC(DGM_CLIP_MAX));
    pCI_DGM->ui16SrcNorm = (IMG_UINT16)IMG_Fix_Clip(
        pMC_DGM->fSrcNorm*DGM_MULT, PREC(DGM_SRC_NORM));

    for ( i = 0 ; i < DGM_N_SLOPE ; i++ )
    {
        pCI_DGM->aSlope[i] = (IMG_UINT16)IMG_Fix_Clip(pMC_DGM->aSlope[i],
            PREC(DGM_SLOPE_0));
    }
    for ( i = 0 ; i < DGM_N_COEFF ; i++ )
    {
        pCI_DGM->aCoeff[i] = (IMG_INT16)IMG_Fix_Clip(pMC_DGM->aCoeff[i],
            PREC(DGM_COEFF_0));
    }
#endif /* DGM_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_SHAConvert(const MC_SHA *pMC_SHA, CI_MODULE_SHA *pCI_SHA,
    const CI_HWINFO *pInfo)
{
#ifndef SHA_NOT_AVAILABLE
    IMG_UINT32 i;
    IMG_BOOL8 b2_4HW = IMG_FALSE;

    /*
     * note to customer: knowing your HW this could be done directly
     * as const values
     */
    if (2 == pInfo->rev_ui8Major && 4 == pInfo->rev_ui8Minor)
    {
        b2_4HW = IMG_TRUE;
    }

    pCI_SHA->ui8Threshold = IMG_Fix_Clip(pMC_SHA->ui16Threshold,
        PREC(SHA_THRESH));
    // warning: Precision already applied in setup function
    /*pCI_SHA->ui8Strength = (IMG_UINT8)IMG_clip(
        (IMG_INT32)pMC_SHA->fStrength, PREC(SHA_STRENGTH));*/
    pCI_SHA->ui8Strength = (IMG_UINT8)pMC_SHA->fStrength;
    pCI_SHA->ui8Detail = IMG_Fix_Clip(pMC_SHA->fDetail, PREC(SHA_DETAIL));
    pCI_SHA->bDenoiseBypass = pMC_SHA->bDenoise ? IMG_FALSE : IMG_TRUE;

    for ( i = 0 ; i < SHA_N_WEIGHT ; i++ )
    {
        /* warning: For the particularities of this values we don't apply
         * the precision here but in the setup function in a different way */
        pCI_SHA->aGainWeight[i] = (IMG_UINT8)pMC_SHA->aGainWeigth[i];
    }

    // warning: Precision already applied in setup function
    pCI_SHA->ui8ELWScale = (IMG_UINT8)pMC_SHA->fELWScale;
    pCI_SHA->i8ELWOffset = (IMG_UINT8)pMC_SHA->i16ELWOffset;

    for ( i = 0 ; i < SHA_N_COMPARE ; i++ )
    {
        pCI_SHA->aDNSimil[i] = IMG_Fix_Clip(pMC_SHA->aDNSimil[i],
            PREC(SHA_DN_EDGE_SIMIL_COMP_PTS));
        pCI_SHA->aDNAvoid[i] = IMG_Fix_Clip(pMC_SHA->aDNAvoid[i],
            PREC(SHA_DN_EDGE_AVOID_COMP_PTS));
    }
#endif /* SHA_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

// statistics configuration

IMG_RESULT MC_EXSConvert(const MC_EXS *pMC_EXS, CI_MODULE_EXS *pCI_EXS)
{
#ifndef EXS_NOT_AVAILABLE
    // MC_EXS::eEnable is used in pipeline conversion
    pCI_EXS->ui16Left = pMC_EXS->ui16Left;
    pCI_EXS->ui16Top = pMC_EXS->ui16Top;
    //pCI_EXS->ui16Width = pMC_EXS->ui16Width-1;
    //pCI_EXS->ui16Height = pMC_EXS->ui16Height-1;
    //: Put back the -1 once the CSIM setup function is Fixed
    pCI_EXS->ui16Width = pMC_EXS->ui16Width;
    pCI_EXS->ui16Height = pMC_EXS->ui16Height;
    //: Check if this has to be float, int,...
    pCI_EXS->ui16PixelMax = (IMG_INT16)pMC_EXS->fPixelMax;
#endif /* EXS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_EXSRevert(const CI_MODULE_EXS *pCI_EXS, IMG_UINT32 eConfig,
    MC_EXS *pMC_EXS)
{
#ifndef EXS_NOT_AVAILABLE
    // MC_EXS::eEnable is used in pipeline conversion
    pMC_EXS->bRegionEnable =
        (eConfig&CI_SAVE_EXS_REGION ? IMG_TRUE : IMG_FALSE);
    pMC_EXS->bGlobalEnable =
        (eConfig&CI_SAVE_EXS_GLOBAL ? IMG_TRUE : IMG_FALSE);

    pMC_EXS->ui16Left = pCI_EXS->ui16Left;
    pMC_EXS->ui16Top = pCI_EXS->ui16Top;
    pMC_EXS->ui16Width = pCI_EXS->ui16Width;
    pMC_EXS->ui16Height = pCI_EXS->ui16Height;
    pMC_EXS->fPixelMax = (MC_FLOAT)pCI_EXS->ui16PixelMax;
#endif /* EXS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_FOSConvert(const MC_FOS *pMC_FOS, CI_MODULE_FOS *pCI_FOS)
{
#ifndef FOS_NOT_AVAILABLE
    // MC_FOS::bEnable is used in pipeline

    pCI_FOS->ui16Left = pMC_FOS->ui16Left;
    pCI_FOS->ui16Top = pMC_FOS->ui16Top;

    //  -1 eliminated for compatibility with the CSIM setup function.
    pCI_FOS->ui16Width = IMG_MAX_INT(0, pMC_FOS->ui16Width);
    pCI_FOS->ui16Height = IMG_MAX_INT(0,pMC_FOS->ui16Height);

    pCI_FOS->ui16ROILeft = pMC_FOS->ui16ROILeft;
    pCI_FOS->ui16ROITop = pMC_FOS->ui16ROITop;
    pCI_FOS->ui16ROIRight =  pMC_FOS->ui16ROILeft + pMC_FOS->ui16ROIWidth -1;
    pCI_FOS->ui16ROIBottom = pMC_FOS->ui16ROITop + pMC_FOS->ui16ROIHeight-1;
#endif /* FOS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_FOSRevert(const CI_MODULE_FOS *pCI_FOS, IMG_UINT32 eConfig,
    MC_FOS *pMC_FOS)
{
#ifndef FOS_NOT_AVAILABLE
    // MC_FOS::bEnable is used in pipeline
    pMC_FOS->bRegionEnable = 
        (eConfig&CI_SAVE_FOS_ROI) ? IMG_TRUE : IMG_FALSE;
    pMC_FOS->bGlobalEnable =
        (eConfig&CI_SAVE_FOS_GRID) ? IMG_TRUE : IMG_FALSE;

    pMC_FOS->ui16Left = pCI_FOS->ui16Left;
    pMC_FOS->ui16Top = pCI_FOS->ui16Top;

    pMC_FOS->ui16Width = pCI_FOS->ui16Width;
    pMC_FOS->ui16Height = pCI_FOS->ui16Height;

    pMC_FOS->ui16ROILeft = pCI_FOS->ui16ROILeft;
    pMC_FOS->ui16ROITop = pCI_FOS->ui16ROITop;
    pMC_FOS->ui16ROIWidth = pCI_FOS->ui16ROIRight - pMC_FOS->ui16ROILeft + 1;
    pMC_FOS->ui16ROIHeight = pCI_FOS->ui16ROIBottom - pMC_FOS->ui16ROITop + 1;
#endif /* FOS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_FLDConvert(const MC_FLD *pMC_FLD, CI_MODULE_FLD *pCI_FLD)
{
#ifndef FLD_NOT_AVAILABLE
    // MC_FLD::bEnable is used in pipeline conversion
    MC_FLOAT fFracSetp50 = 0, fFracSetp60 = 0;

    fFracSetp50 = (256.0*50)/(pMC_FLD->ui16VTot*pMC_FLD->fFrameRate);
    fFracSetp60 = (256.0*60)/(pMC_FLD->ui16VTot*pMC_FLD->fFrameRate);
    if ( fFracSetp50 > 1.0 ) fFracSetp50 = 1.0;
    if ( fFracSetp50 < 0.0 ) fFracSetp50 = 0.0;
    if ( fFracSetp60 > 1.0 ) fFracSetp60 = 1.0;
    if ( fFracSetp60 < 0.0 ) fFracSetp60 = 0.0;

    pCI_FLD->ui16FracStep50 = (IMG_UINT16)IMG_Fix_Clip(fFracSetp50,
        PREC(FLD_FRAC_STEP_50));
    pCI_FLD->ui16FracStep60 = (IMG_UINT16)IMG_Fix_Clip(fFracSetp60,
        PREC(FLD_FRAC_STEP_60));
    pCI_FLD->ui16VertTotal = pMC_FLD->ui16VTot;
    pCI_FLD->ui16CoefDiff = pMC_FLD->ui16CoefDiff;
    pCI_FLD->ui16NFThreshold = pMC_FLD->ui16NFThreshold;
    pCI_FLD->ui32SceneChange = pMC_FLD->ui32SceneChange;
    pCI_FLD->ui8RShift = pMC_FLD->ui8RShift;
    pCI_FLD->ui8MinPN = pMC_FLD->ui8MinPN;
    pCI_FLD->ui8PN = pMC_FLD->ui8PN;
    pCI_FLD->bReset = pMC_FLD->bReset;
#endif /* FLD_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_FLDRevert(const CI_MODULE_FLD *pCI_FLD, IMG_UINT32 eConfig,
    MC_FLD *pMC_FLD)
{
#ifndef FLD_NOT_AVAILABLE
    MC_FLOAT fFracSetp50 = 0;  // , fFracSetp60 not needed

    // MC_FLD::bEnable is used in pipeline conversion
    pMC_FLD->bEnable = (eConfig&CI_SAVE_FLICKER) ? IMG_TRUE : IMG_FALSE;

    pMC_FLD->ui16VTot = pCI_FLD->ui16VertTotal;

    fFracSetp50 = IMG_Fix_Revert(pCI_FLD->ui16FracStep50,
        PREC(FLD_FRAC_STEP_50));

    pMC_FLD->fFrameRate = (256.0 * 50) / (fFracSetp50 * pMC_FLD->ui16VTot);

    pMC_FLD->ui16CoefDiff = pCI_FLD->ui16CoefDiff;
    pMC_FLD->ui16NFThreshold = pCI_FLD->ui16NFThreshold;
    pMC_FLD->ui32SceneChange = pCI_FLD->ui32SceneChange;
    pMC_FLD->ui8RShift = pCI_FLD->ui8RShift;
    pMC_FLD->ui8MinPN = pCI_FLD->ui8MinPN;
    pMC_FLD->ui8PN = pCI_FLD->ui8PN;
    pMC_FLD->bReset = pCI_FLD->bReset;
#endif /* FLD_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_HISConvert(const MC_HIS *pMC_HIS, CI_MODULE_HIS *pCI_HIS)
{
#ifndef HIS_NOT_AVAILABLE
    // MC_HIS::eEnable is used in pipeline conversion

    /* : application of precisions have been temporarily disabled to
     * match the CSIM setup */
    pCI_HIS->ui16InputOffset = (IMG_UINT16)pMC_HIS->fInputOffset;
    pCI_HIS->ui16InputScale =  (IMG_UINT16)pMC_HIS->fInputScale;
    pCI_HIS->ui16Left = pMC_HIS->ui16Left;
    pCI_HIS->ui16Top = pMC_HIS->ui16Top;
    pCI_HIS->ui16Width = pMC_HIS->ui16Width;
    pCI_HIS->ui16Height = pMC_HIS->ui16Height;
#endif /* HIS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_HISRevert(const CI_MODULE_HIS *pCI_HIS, IMG_UINT32 eConfig,
    MC_HIS *pMC_HIS)
{
#ifndef HIS_NOT_AVAILABLE
    // MC_HIS::eEnable is used in pipeline conversion
    pMC_HIS->bRegionEnable = 
        (eConfig&CI_SAVE_HIST_REGION) ? IMG_TRUE : IMG_FALSE;
    pMC_HIS->bGlobalEnable =
        (eConfig&CI_SAVE_HIST_GLOBAL) ? IMG_TRUE : IMG_FALSE;

    /* : application of precisions have been temporarily disabled to
    * match the CSIM setup */
    pMC_HIS->fInputOffset = pCI_HIS->ui16InputOffset;
    pMC_HIS->fInputScale = pCI_HIS->ui16InputScale;
    pMC_HIS->ui16Left = pCI_HIS->ui16Left;
    pMC_HIS->ui16Top = pCI_HIS->ui16Top;
    pMC_HIS->ui16Width = pCI_HIS->ui16Width;
    pMC_HIS->ui16Height = pCI_HIS->ui16Height;
#endif /* HIS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_WBSConvert(const MC_WBS *pMC_WBS, CI_MODULE_WBS *pCI_WBS)
{
#ifndef WBS_NOT_AVAILABLE
    int i;

    // MC_WBS::ui8ActiveROI is used in pipeline conversion too
    if (pMC_WBS->ui8ActiveROI > 0)
    {
        pCI_WBS->ui8ActiveROI = pMC_WBS->ui8ActiveROI-1; // 0 based
    }
    else
    {
        pCI_WBS->ui8ActiveROI = 0;
    }
    pCI_WBS->ui16RGBOffset = (IMG_INT16)IMG_Fix_Clip(pMC_WBS->fRGBOffset,
        PREC(WBS_RGB_OFFSET));

    pCI_WBS->ui16YOffset = (IMG_INT16)IMG_Fix_Clip(pMC_WBS->fYOffset,
        PREC(WBS_Y_OFFSET));

    for ( i = 0 ; i < WBS_NUM_ROI ; i++)
    {
        pCI_WBS->aRoiLeft[i] = pMC_WBS->aRoiLeft[i];
        pCI_WBS->aRoiTop[i] = pMC_WBS->aRoiTop[i];
        if((pMC_WBS->aRoiLeft[i] + pMC_WBS->aRoiWidth[i])>0)
        {
            pCI_WBS->aRoiRight[i] = IMG_clip(pMC_WBS->aRoiLeft[i]
                + pMC_WBS->aRoiWidth[i]-1, PREC_C(WBS_ROI_COL_END));
        }
        else
        {
            pCI_WBS->aRoiRight[i] = 0;
            if(i+1<=pCI_WBS->ui8ActiveROI)
            {
                LOG_WARNING("WBS CI::aRoiRigh[%d] forced to 0 because "\
                    "(MC::aRoiLeft[%d] + MC::aRoiWidth[%d]) <= 0\n",
                    i, i, i);
            }
        }
        if((pMC_WBS->aRoiTop[i] + pMC_WBS->aRoiHeight[i])>0)
        {
            pCI_WBS->aRoiBottom[i] = IMG_clip(pMC_WBS->aRoiTop[i]
                + pMC_WBS->aRoiHeight[i]-1, PREC_C(WBS_ROI_COL_END));
        }
        else
        {
            pCI_WBS->aRoiBottom[i] = 0;
            if(i+1<=pCI_WBS->ui8ActiveROI)
            {
                LOG_WARNING("WBS CI::aRoiBottom[%d] forced to 0 because "\
                    "(MC::aRoiTop[%d] + MC::aRoiHeight[%d]) <= 0\n",
                    i, i, i);
            }
        }

        pCI_WBS->aRMax[i] = pMC_WBS->aRMax[i];
        pCI_WBS->aGMax[i] = pMC_WBS->aGMax[i];
        pCI_WBS->aBMax[i] = pMC_WBS->aBMax[i];

        pCI_WBS->aYMax[i] = pMC_WBS->aYMax[i];
    }

#endif /* WBS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_WBSRevert(const CI_MODULE_WBS *pCI_WBS, IMG_UINT32 eConfig,
    MC_WBS *pMC_WBS)
{
#ifndef WBS_NOT_AVAILABLE
    int i;

    // MC_WBS::ui8ActiveROI is used in pipeline conversion too
    if (eConfig&CI_SAVE_WHITEBALANCE)
    {
        pMC_WBS->ui8ActiveROI = pCI_WBS->ui8ActiveROI + 1;  // 0 based in reg
    }
    else
    {
        pMC_WBS->ui8ActiveROI = 0;
    }

    pMC_WBS->fRGBOffset = IMG_Fix_Revert(pCI_WBS->ui16RGBOffset,
        PREC(WBS_RGB_OFFSET));

    pMC_WBS->fYOffset = IMG_Fix_Revert(pCI_WBS->ui16YOffset,
        PREC(WBS_Y_OFFSET));

    for (i = 0; i < WBS_NUM_ROI; i++)
    {
        pMC_WBS->aRoiLeft[i] = pCI_WBS->aRoiLeft[i];
        pMC_WBS->aRoiTop[i] = pCI_WBS->aRoiTop[i];

        if (pCI_WBS->aRoiRight[i] != 0)
        {
            pMC_WBS->aRoiWidth[i] = pCI_WBS->aRoiRight[i] -
                pCI_WBS->aRoiLeft[i] + 1;
        }
        else
        {
            pMC_WBS->aRoiWidth[i] = 0;
#ifdef INFOTM_ENABLE_WARNING_DEBUG			
            LOG_WARNING("WBS MC::aRoiWidth[%d] forced to 0 because "\
                "(MC::aRoiRight[%d] == 0\n",
                i, i);
#endif //INFOTM_ENABLE_WARNING_DEBUG				
        }
        if (pCI_WBS->aRoiBottom[i] != 0)
        {
            pMC_WBS->aRoiHeight[i] = pCI_WBS->aRoiBottom[i]
                - pMC_WBS->aRoiTop[i] + 1;
        }
        else
        {
            pMC_WBS->aRoiHeight[i] = 0;
#ifdef INFOTM_ENABLE_WARNING_DEBUG			
            LOG_WARNING("WBS MC::aRoiHeight[%d] forced to 0 because "\
                "(MC::aRoiBottom[%d] == 0\n",
                i, i);
#endif //INFOTM_ENABLE_WARNING_DEBUG				
        }

        pMC_WBS->aRMax[i] = pCI_WBS->aRMax[i];
        pMC_WBS->aGMax[i] = pCI_WBS->aGMax[i];
        pMC_WBS->aBMax[i] = pCI_WBS->aBMax[i];

        pMC_WBS->aYMax[i] = pCI_WBS->aYMax[i];
    }
#endif /* WBS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_ENSConvert(const MC_ENS *pMC_ENS, CI_MODULE_ENS *pCI_ENS)
{
#ifndef ENS_NOT_AVAILABLE
    IMG_UINT32 tmp = 0;
    IMG_UINT32 clippedNLines = 0;
    // MC_ENS::bEnable is used in pipeline conversion

    clippedNLines = IMG_MIN_INT(ENS_NLINES_MAX,
        IMG_MAX_INT(pMC_ENS->ui32NLines, ENS_NLINES_MIN));
    tmp = clippedNLines;
    /// @ is it better to return an error or do the clipping?
    if ( tmp != pMC_ENS->ui32NLines)
    {
        LOG_WARNING("nb of couples clipped from %d to %d (min=%d, max=%d)\n",
            pMC_ENS->ui32NLines, tmp, ENS_NLINES_MIN, ENS_NLINES_MAX);
    }

    pCI_ENS->ui8Log2NCouples = 0;
    while(tmp >>= 1) // log2
    {
        pCI_ENS->ui8Log2NCouples++;
    }

    if ( clippedNLines != (IMG_UINT32)(1<<pCI_ENS->ui8Log2NCouples) )
    {
        LOG_ERROR("nb of lines should be a power of 2 (%u given)\n",
            clippedNLines);
        return IMG_ERROR_FATAL;
    }
    // because we clipped before or checked and returned!
    IMG_ASSERT(pCI_ENS->ui8Log2NCouples >= ENS_EXP_ADD);
    pCI_ENS->ui8Log2NCouples -= ENS_EXP_ADD;

    tmp = pMC_ENS->ui32KernelSubsampling;
    pCI_ENS->ui8SubExp = 0;
    while(tmp >>= 1) // log2
    {
        pCI_ENS->ui8SubExp++;
    }

    if ( pMC_ENS->ui32KernelSubsampling != 0
        && pMC_ENS->ui32KernelSubsampling
        != (IMG_UINT32)(1<<pCI_ENS->ui8SubExp) )
    {
        LOG_ERROR("kernel subsampling should be a power of 2 (%u given)\n",
            pMC_ENS->ui32KernelSubsampling);
        return IMG_ERROR_FATAL;
    }
#endif /* ENS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}

IMG_RESULT MC_ENSRevert(const CI_MODULE_ENS *pCI_ENS, IMG_UINT32 eConfig,
    MC_ENS *pMC_ENS)
{
#ifndef ENS_NOT_AVAILABLE
    IMG_UINT32 tmp = 0;
    IMG_UINT32 clippedNLines = 0;
    // MC_ENS::bEnable is used in pipeline conversion
    pMC_ENS->bEnable = (eConfig&CI_SAVE_ENS) ? IMG_TRUE : IMG_FALSE;

    pMC_ENS->ui32NLines = 1 << (pCI_ENS->ui8Log2NCouples + ENS_EXP_ADD);

    pMC_ENS->ui32KernelSubsampling = 1 << (pCI_ENS->ui8SubExp);
#endif /* ENS_NOT_AVAILABLE */
    return IMG_SUCCESS;
}
