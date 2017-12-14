/**
******************************************************************************
@file mc_pipeline.c

@brief Implementation of the MC_Pipeline functions

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

#include <img_defs.h>
#include <img_errors.h>
#include <ctx_reg_precisions.h>

#define LOG_TAG "MC_PIPELINE"
#include <felixcommon/userlog.h>

IMG_RESULT MC_PipelineInit(MC_PIPELINE *pMCPipeline, const CI_HWINFO *pHWInfo)
{
	IMG_RESULT ret = IMG_SUCCESS; // IMG_SUCCESS is 0

	pMCPipeline->pHWInfo = pHWInfo;

    if (0 == pMCPipeline->sIIF.ui16ImagerSize[0]
        || 0 == pMCPipeline->sIIF.ui16ImagerSize[1])
	{
        LOG_WARNING("IIF sizes should be configured before running %s\n",
            __FUNCTION__);
	}
    
	pMCPipeline->eEncOutput = PXL_NONE;
	pMCPipeline->eDispOutput = PXL_NONE;
	pMCPipeline->eHDRExtOutput = PXL_NONE;
    pMCPipeline->eRaw2DExtOutput = PXL_NONE;
    pMCPipeline->eHDRInsertion = PXL_NONE;
	pMCPipeline->eDEPoint = CI_INOUT_NONE;
    // default is 16 for 8b - scale to pipeline's bitdepth
    // we also shift to take the fractional bits into account
    if (10 <= pHWInfo->context_aBitDepth[0])
    {
        pMCPipeline->ui16SystemBlackLevel =
            16 << (pHWInfo->context_aBitDepth[0] - 8 - SYS_BLACK_LEVEL_FRAC);
    }
    else
    {
        IMG_ASSERT(pHWInfo->context_aBitDepth[0] == 8);  // 9bits is unusual
        pMCPipeline->ui16SystemBlackLevel = 16;
    }
    // not used in SW and HW testing should enable it manually
    pMCPipeline->bEnableCRC = IMG_FALSE;
	pMCPipeline->bEnableTimestamp = IMG_TRUE;

	// IIF is not setup! it should be done before calling that function!
	ret += MC_BLCInit(&pMCPipeline->sBLC, pHWInfo->config_ui8BitDepth);
    ret += MC_RLTInit(&pMCPipeline->sRLT);
	ret += MC_LSHInit(&pMCPipeline->sLSH);
	ret += MC_WBCInit(&pMCPipeline->sWBC);
	ret += MC_DNSInit(&pMCPipeline->sDNS);
    // ret += MC_LSHGridInit(&sMC_LSHGrid);
	ret += MC_DPFInit(&pMCPipeline->sDPF, pHWInfo, &(pMCPipeline->sIIF));
	ret += MC_LCAInit(&pMCPipeline->sLCA, &(pMCPipeline->sIIF));
	// add DMO
	ret += MC_CCMInit(&pMCPipeline->sCCM);
	ret += MC_MGMInit(&pMCPipeline->sMGM);
	ret += MC_GMAInit(&pMCPipeline->sGMA);
	ret += MC_R2YInit(&pMCPipeline->sR2Y);
	ret += MC_MIEInit(&pMCPipeline->sMIE);
	ret += MC_TNMInit(&pMCPipeline->sTNM, &(pMCPipeline->sIIF), pHWInfo);
	ret += MC_SHAInit(&pMCPipeline->sSHA);
	ret += MC_ESCInit(&pMCPipeline->sESC, &(pMCPipeline->sIIF));
	ret += MC_DSCInit(&pMCPipeline->sDSC, &(pMCPipeline->sIIF));
	ret += MC_Y2RInit(&pMCPipeline->sY2R);
	ret += MC_DGMInit(&pMCPipeline->sDGM);
	// statistics
	ret += MC_EXSInit(&pMCPipeline->sEXS, &(pMCPipeline->sIIF));
	ret += MC_FOSInit(&pMCPipeline->sFOS, &(pMCPipeline->sIIF));
	ret += MC_WBSInit(&pMCPipeline->sWBS, &(pMCPipeline->sIIF));
    ret += MC_AWSInit(&pMCPipeline->sAWS, &(pMCPipeline->sIIF));
	ret += MC_HISInit(&pMCPipeline->sHIS, &(pMCPipeline->sIIF));
	ret += MC_FLDInit(&pMCPipeline->sFLD, &(pMCPipeline->sIIF));
	ret += MC_ENSInit(&pMCPipeline->sENS);

    if (ret != IMG_SUCCESS)
	{
		return IMG_ERROR_FATAL;
	}
	return IMG_SUCCESS;
}

IMG_BOOL8 MC_LastESCIsEqual(MC_ESC *psESC, IMG_BOOL8 bOutput422)
{
    static int count = 0;
    static MC_ESC sLastESC;
    static  IMG_BOOL8 bLastOutput422 = IMG_FALSE;

    count ++;

    if (sLastESC.aPitch[0] != psESC->aPitch[0] ||
        sLastESC.aPitch[1] != psESC->aPitch[1] ||
        sLastESC.aOutputSize[0] != psESC->aOutputSize[0] ||
        sLastESC.aOutputSize[1] != psESC->aOutputSize[1] ||
        sLastESC.aOffset[0] != psESC->aOffset[0] ||
        sLastESC.aOffset[1] != psESC->aOffset[1] ||
        sLastESC.bBypassESC != psESC->bBypassESC ||
        sLastESC.bAdjustCutoff != psESC->bAdjustCutoff ||
        sLastESC.bChromaInter != psESC->bChromaInter||
        bLastOutput422 != bOutput422)
    {
        LOG_DEBUG("%d Last ESC\naPitch[0] = %lf\n"
            "aPitch[1] = %lf\n"
            "aOutputSize[0] = %d\n"
            "aOutputSize[1] = %d\n"
            "aOffset[0] = %d\n"
            "aOffset[1] = %d\n"
            "bBypassESC = %d\n"
            "bAdjustCutoff = %d\n"
            "bChromaInter = %d\n",
            count,
            sLastESC.aPitch[0], sLastESC.aPitch[1],
            sLastESC.aOutputSize[0], sLastESC.aOutputSize[1],
            sLastESC.aOffset[0], sLastESC.aOffset[1],
            sLastESC.bBypassESC, sLastESC.bAdjustCutoff,
            sLastESC.bChromaInter);

        LOG_DEBUG("%d Current ESC\naPitch[0] = %lf\n"
            "aPitch[1] = %lf\n"
            "aOutputSize[0] = %d\n"
            "aOutputSize[1] = %d\n"
            "aOffset[0] = %d\n"
            "aOffset[1] = %d\n"
            "bBypassESC = %d\n"
            "bAdjustCutoff = %d\n"
            "bChromaInter = %d\n",
            count,
            psESC->aPitch[0], psESC->aPitch[1],
            psESC->aOutputSize[0], psESC->aOutputSize[1],
            psESC->aOffset[0], psESC->aOffset[1],
            psESC->bBypassESC, psESC->bAdjustCutoff,
            psESC->bChromaInter);

        IMG_MEMCPY(&sLastESC, psESC, sizeof(MC_ESC));
        bLastOutput422 = bOutput422;
        return IMG_FALSE;
    }

    return IMG_TRUE;
}

IMG_BOOL8 MC_LastDSCIsEqual(MC_DSC *psDSC)
{
    static int count = 0;
    static MC_DSC sLastDSC;

    count ++;

    if (sLastDSC.aPitch[0] != psDSC->aPitch[0] ||
        sLastDSC.aPitch[1] != psDSC->aPitch[1] ||
        sLastDSC.aOutputSize[0] != psDSC->aOutputSize[0] ||
        sLastDSC.aOutputSize[1] != psDSC->aOutputSize[1] ||
        sLastDSC.aOffset[0] != psDSC->aOffset[0] ||
        sLastDSC.aOffset[1] != psDSC->aOffset[1] ||
        sLastDSC.bBypassDSC != psDSC->bBypassDSC ||
        sLastDSC.bAdjustCutoff != psDSC->bAdjustCutoff)
    {
        LOG_DEBUG("%d Last DSC\naPitch[0] = %lf\n"
            "aPitch[1] = %lf\n"
            "aOutputSize[0] = %d\n"
            "aOutputSize[1] = %d\n"
            "aOffset[0] = %d\n"
            "aOffset[1] = %d\n"
            "bBypassDSC = %d\n"
            "bAdjustCutoff = %d\n",
            count,
            sLastDSC.aPitch[0], sLastDSC.aPitch[1],
            sLastDSC.aOutputSize[0], sLastDSC.aOutputSize[1],
            sLastDSC.aOffset[0], sLastDSC.aOffset[1],
            sLastDSC.bBypassDSC, sLastDSC.bAdjustCutoff);

        LOG_DEBUG("%d Current DSC\naPitch[0] = %lf\n"
            "aPitch[1] = %lf\n"
            "aOutputSize[0] = %d\n"
            "aOutputSize[1] = %d\n"
            "aOffset[0] = %d\n"
            "aOffset[1] = %d\n"
            "bBypassDSC = %d\n"
            "bAdjustCutoff = %d\n",
            count,
            psDSC->aPitch[0], psDSC->aPitch[1],
            psDSC->aOutputSize[0], psDSC->aOutputSize[1],
            psDSC->aOffset[0], psDSC->aOffset[1],
            psDSC->bBypassDSC, psDSC->bAdjustCutoff);

        IMG_MEMCPY(&sLastDSC, psDSC, sizeof(MC_DSC));
        return IMG_FALSE;
    }

    return IMG_TRUE;
}

IMG_RESULT MC_PipelineConvert(const MC_PIPELINE *pMCPipeline,
    CI_PIPELINE *pCIPipeline)
{
	IMG_RESULT ret = IMG_SUCCESS; // IMG_SUCCESS is 0
    IMG_BOOL8 bOutput422 = IMG_TRUE;

    if (NULL == pMCPipeline->pHWInfo)
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

    ret |= MC_IIFConvert(&(pMCPipeline->sIIF),
        &(pCIPipeline->sImagerInterface));

    if (pMCPipeline->sESC.aOutputSize[0] > 0)
	{
        /* if display output also enabled should be the max of the 2,
         * triggers the next if which works because DSC_V_LUMA_TAPS
         * is smaller than ESC_V_LUMA_TAPS */
        pCIPipeline->sImagerInterface.ui16ScalerBottomBorder =
            ESC_V_LUMA_TAPS / 2;
	}
    if (pMCPipeline->sDSC.aOutputSize[0] > 0)
	{
        pCIPipeline->sImagerInterface.ui16ScalerBottomBorder =
            DSC_V_LUMA_TAPS / 2;
	}

    /*
    * output format
    */
    // if no output no extra line
    pCIPipeline->sImagerInterface.ui16ScalerBottomBorder = 0;

    IMG_MEMSET(&(pCIPipeline->config.eEncType), 0, sizeof(PIXELTYPE));
    if (pMCPipeline->eEncOutput != PXL_NONE)
    {
        ret = PixelTransformYUV(&(pCIPipeline->config.eEncType),
            pMCPipeline->eEncOutput);
        if (IMG_SUCCESS != ret)
	{
            LOG_ERROR("unexpected YUV format given\n");
            return ret;
	}

        // the maximum output sizes are in pixels
        pCIPipeline->config.ui16MaxEncOutWidth =
            pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH;
        pCIPipeline->config.ui16MaxEncOutHeight =
            pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT;

        // bOutput422 = IMG_TRUE;
        if (1 != pCIPipeline->config.eEncType.ui8VSubsampling)
	{
            bOutput422 = IMG_FALSE;
	}
	}
	else
    {
        // no allocation for the encoder pipeline
        pCIPipeline->config.ui16MaxEncOutWidth = 0;
        pCIPipeline->config.ui16MaxEncOutHeight = 0;
    }

    if (pMCPipeline->eEncOutput != PXL_NONE || pMCPipeline->sENS.bEnable)
	{
        pCIPipeline->sImagerInterface.ui16ScalerBottomBorder =
            (ESC_V_LUMA_TAPS / 2);
	}

    IMG_MEMSET(&(pCIPipeline->config.eDispType), 0, sizeof(PIXELTYPE));
    pCIPipeline->config.eDataExtraction = CI_INOUT_NONE;
    if (pMCPipeline->eDispOutput != PXL_NONE)
	{
        pCIPipeline->config.ui16MaxDispOutWidth =
            pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH;
        pCIPipeline->config.ui16MaxDispOutHeight =
            pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT;

        if (pMCPipeline->eDEPoint == CI_INOUT_NONE)
	{
            // RGB or YUV444 output
			if ( (ret = PixelTransformDisplay(&(pCIPipeline->config.eDispType), pMCPipeline->eDispOutput)) != IMG_SUCCESS )
			{
				LOG_ERROR("unexpected RGB format given\n");
				return ret;
			}
            pCIPipeline->config.eDataExtraction = CI_INOUT_NONE;

            if (pMCPipeline->eEncOutput != PXL_NONE
                || pMCPipeline->sENS.bEnable)
	{
                /* if both enabled the border is
                * max((ESC_V_LUMA_TAPS+1)/2, (DSC_V_LUMA_TAPS+1)/2) */
#if ESC_V_LUMA_TAPS > DSC_V_LUMA_TAPS
                pCIPipeline->sImagerInterface.ui16ScalerBottomBorder =
                    (ESC_V_LUMA_TAPS + 1) / 2;
#else
                pCIPipeline->sImagerInterface.ui16ScalerBottomBorder =
                    (DSC_V_LUMA_TAPS + 1) / 2;
#endif
	}
            else
	{
                pCIPipeline->sImagerInterface.ui16ScalerBottomBorder =
                    (DSC_V_LUMA_TAPS / 2);
            }
	}
        else
        {
            // data extraction output
            ret = PixelTransformBayer(&(pCIPipeline->config.eDispType),
                pMCPipeline->eDispOutput, pMCPipeline->sIIF.eBayerFormat);
            if (IMG_SUCCESS != ret)  // mosaic should not matter here
	{
                LOG_ERROR("unexpected Bayer format given\n");
                return ret;
            }
            pCIPipeline->config.eDataExtraction = pMCPipeline->eDEPoint;
        }
	}
    else
	{
        // no allocation for the display and encoder pipeline
        pCIPipeline->config.ui16MaxDispOutWidth = 0;
        pCIPipeline->config.ui16MaxDispOutHeight = 0;
	}

    IMG_MEMSET(&(pCIPipeline->config.eHDRExtType), 0, sizeof(PIXELTYPE));
    if (pMCPipeline->eHDRExtOutput != PXL_NONE)
    {
        // RGB output
        ret = PixelTransformRGB(&(pCIPipeline->config.eHDRExtType),
            pMCPipeline->eHDRExtOutput);
        if (IMG_SUCCESS != ret)
	{
            LOG_ERROR("unexpected RGB format given for HDR extraction\n");
            return ret;
        }
	}

    IMG_MEMSET(&(pCIPipeline->config.eHDRInsType), 0, sizeof(PIXELTYPE));
    if (pMCPipeline->eHDRInsertion != PXL_NONE)
    {
        // RGB input
        ret = PixelTransformRGB(&(pCIPipeline->config.eHDRInsType),
            pMCPipeline->eHDRInsertion);
        if (IMG_SUCCESS != ret)
	{
            LOG_ERROR("unexpected RGB format given for HDR insertion\n");
            return ret;
        }
	}

    IMG_MEMSET(&(pCIPipeline->config.eRaw2DExtraction), 0, sizeof(PIXELTYPE));
    if (pMCPipeline->eRaw2DExtOutput != PXL_NONE)
	{
        ret = PixelTransformBayer(&(pCIPipeline->config.eRaw2DExtraction),
            pMCPipeline->eRaw2DExtOutput, pMCPipeline->sIIF.eBayerFormat);
        if (IMG_SUCCESS != ret)  // mosaic should not matter here
		{
            LOG_ERROR("unexpected Bayer format given to Raw 2D\n");
			return ret;
		}
        if (PACKED_MSB != pCIPipeline->config.eRaw2DExtraction.ePackedStart)
        {
            LOG_ERROR("format is expected to be TIFF for Raw2D\n");
            return IMG_ERROR_NOT_SUPPORTED;
	}
	}

    /*
     * accumulate results in ret - if it is not 0 in the end then some
     * conversion failed!
     */

    ret |= MC_BLCConvert(&(pMCPipeline->sBLC),
        &(pCIPipeline->sBlackCorrection));
    ret |= MC_RLTConvert(&(pMCPipeline->sRLT), &(pCIPipeline->sRawLUT));
    ret |= MC_WBCConvert(&(pMCPipeline->sWBC), &(pCIPipeline->sWhiteBalance));
    ret |= MC_LSHConvert(&(pMCPipeline->sLSH), &(pCIPipeline->sDeshading));
    ret |= MC_DNSConvert(&(pMCPipeline->sDNS), &(pCIPipeline->sDenoiser));
    ret |= MC_DPFConvert(&(pMCPipeline->sDPF), &(pMCPipeline->sIIF),
        &(pCIPipeline->sDefectivePixels));
    pCIPipeline->config.ui32DPFWriteMapSize = pMCPipeline->sDPF.ui32OutputMapSize;
    ret |= MC_LCAConvert(&(pMCPipeline->sLCA),
        &(pCIPipeline->sChromaAberration));
    // add DMO
    ret |= MC_CCMConvert(&(pMCPipeline->sCCM),
        &(pCIPipeline->sColourCorrection));
    ret |= MC_MGMConvert(&(pMCPipeline->sMGM),
        &(pCIPipeline->sMainGamutMapper));
    ret |= MC_GMAConvert(&(pMCPipeline->sGMA),
        &(pCIPipeline->sGammaCorrection));
    ret |= MC_R2YConvert(&(pMCPipeline->sR2Y), &(pCIPipeline->sRGBToYCC));
    ret |= MC_MIEConvert(&(pMCPipeline->sMIE),
        &(pCIPipeline->sImageEnhancer), pMCPipeline->pHWInfo);
    ret |= MC_TNMConvert(&(pMCPipeline->sTNM), &(pMCPipeline->sIIF),
        pMCPipeline->pHWInfo->config_ui8Parallelism,
        &(pCIPipeline->sToneMapping));
    ret |= MC_SHAConvert(&(pMCPipeline->sSHA), &(pCIPipeline->sSharpening),
        pMCPipeline->pHWInfo);

    if (!MC_LastESCIsEqual(&(pMCPipeline->sESC), bOutput422))
    {
        ret |= MC_ESCConvert(&(pMCPipeline->sESC),
            &(pCIPipeline->sEncoderScaler), bOutput422);
    }

    if (!MC_LastDSCIsEqual(&(pMCPipeline->sDSC)))
    {
        ret |= MC_DSCConvert(&(pMCPipeline->sDSC),
            &(pCIPipeline->sDisplayScaler));
    }

    ret |= MC_Y2RConvert(&(pMCPipeline->sY2R), &(pCIPipeline->sYCCToRGB));
    ret |= MC_DGMConvert(&(pMCPipeline->sDGM),
        &(pCIPipeline->sDisplayGamutMapper));

    /*
     * statistics
     */
    pCIPipeline->config.eOutputConfig = 0;

    if (IMG_SUCCESS != ret)  // IMG_SUCCESS is 0
		{
        LOG_ERROR("failed when converting a main module configuration\n");
        return IMG_ERROR_FATAL;
			}

    if (pMCPipeline->bEnableCRC == IMG_TRUE)
			{
        pCIPipeline->config.eOutputConfig |= CI_SAVE_CRC;
			}
    if (pMCPipeline->bEnableTimestamp == IMG_TRUE)
			{
        LOG_DEBUG("Enabled timestamps for context %d\n",
            pCIPipeline->config.ui8Context);
        pCIPipeline->config.eOutputConfig |= CI_SAVE_TIMESTAMP;
		}
		else
		{
        LOG_DEBUG("Disabled timestamps for context %d\n",
            pCIPipeline->config.ui8Context);
    }

    ret |= MC_EXSConvert(&(pMCPipeline->sEXS), &(pCIPipeline->stats.sExposureStats));
    if (pMCPipeline->sEXS.bRegionEnable == IMG_TRUE)
			{
        pCIPipeline->config.eOutputConfig |= CI_SAVE_EXS_REGION;
			}
    if (pMCPipeline->sEXS.bGlobalEnable == IMG_TRUE)
    {
        pCIPipeline->config.eOutputConfig |= CI_SAVE_EXS_GLOBAL;
		}

    ret |= MC_FOSConvert(&(pMCPipeline->sFOS), &(pCIPipeline->stats.sFocusStats));
    if (pMCPipeline->sFOS.bGlobalEnable == IMG_TRUE)
    {
        pCIPipeline->config.eOutputConfig |= CI_SAVE_FOS_GRID;
	}
    if (pMCPipeline->sFOS.bRegionEnable == IMG_TRUE)
	{
        pCIPipeline->config.eOutputConfig |= CI_SAVE_FOS_ROI;
	}

    ret |= MC_WBSConvert(&(pMCPipeline->sWBS),
        &(pCIPipeline->stats.sWhiteBalanceStats));
    if (pMCPipeline->sWBS.ui8ActiveROI > 0)
    {
        pCIPipeline->config.eOutputConfig |= CI_SAVE_WHITEBALANCE;
    }

    ret |= MC_AWSConvert(&(pMCPipeline->sAWS),
        &(pCIPipeline->stats.sAutoWhiteBalanceStats));
    if (pMCPipeline->sAWS.bEnable)
        {
        pCIPipeline->config.eOutputConfig |= CI_SAVE_WHITEBALANCE_EXT;
        }

    ret |= MC_HISConvert(&(pMCPipeline->sHIS), &(pCIPipeline->stats.sHistStats));
    if (pMCPipeline->sHIS.bRegionEnable == IMG_TRUE)
    {
        pCIPipeline->config.eOutputConfig |= CI_SAVE_HIST_REGION;
    }
    if (pMCPipeline->sHIS.bGlobalEnable == IMG_TRUE)
        {
        pCIPipeline->config.eOutputConfig |= CI_SAVE_HIST_GLOBAL;
        }

    ret |= MC_FLDConvert(&(pMCPipeline->sFLD), &(pCIPipeline->stats.sFlickerStats));
    if (pMCPipeline->sFLD.bEnable == IMG_TRUE)
    {
        pCIPipeline->config.eOutputConfig |= CI_SAVE_FLICKER;
    }

    ret |= MC_ENSConvert(&(pMCPipeline->sENS), &(pCIPipeline->stats.sEncStats));
    if (pMCPipeline->sENS.bEnable == IMG_TRUE)
    {
        pCIPipeline->config.eOutputConfig |= CI_SAVE_ENS;
		}

    if (IMG_SUCCESS != ret)  // IMG_SUCCESS is 0
        {
        LOG_ERROR("failed when converting a statistic configuration\n");
        return IMG_ERROR_FATAL;
        }

    // no shift by SYS_BLACK_LEVEL_FRAC
    pCIPipeline->config.ui16SysBlackLevel = pMCPipeline->ui16SystemBlackLevel;

	return IMG_SUCCESS;
}
