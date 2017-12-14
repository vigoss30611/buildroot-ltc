/**
******************************************************************************
 @file mc_config.c

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

#include <img_defs.h>
#include <img_errors.h>
#include <felixcommon/pixel_format.h>

#if MGM_N_SLOPE != DGM_N_SLOPE || MGM_N_COEFF != DGM_N_COEFF
#error "MGM != DGM"
#endif

IMG_RESULT MC_IIFInit(MC_IIF *pIIF)
{
	IMG_MEMSET(pIIF, 0, sizeof(MC_IIF));
	pIIF->ui8Imager = CI_N_CONTEXT; // invalid
	pIIF->eBayerFormat = MOSAIC_RGGB;
	return IMG_SUCCESS;
}

IMG_RESULT MC_BLCInit(MC_BLC *pBLC, IMG_UINT8 ui8BitDepth)
{
	IMG_UINT32 i;
	IMG_MEMSET(pBLC, 0, sizeof(MC_BLC));
	pBLC->bRowAverage = IMG_FALSE;
	pBLC->bBlackFrame = IMG_FALSE;
	pBLC->fRowAverage = 0.0f;
	pBLC->ui16PixelMax = (1<<ui8BitDepth) - 1;

	for ( i = 0 ; i < BLC_OFFSET_NO ; i++ )
	{
		pBLC->aFixedOffset[i] = 0;
	}
	return IMG_SUCCESS;
}

IMG_RESULT MC_RLTInit(MC_RLT *pRLT)
{
    IMG_MEMSET(pRLT, 0, sizeof(MC_RLT));

    pRLT->eMode = CI_RLT_DISABLED;

    return IMG_SUCCESS;
}

IMG_RESULT MC_LSHInit(MC_LSH *pLSH)
{
	IMG_MEMSET(pLSH, 0, sizeof(MC_LSH));
    
	// gradients are 0
	// pLSH->bUseDeshadingGrid = IMG_FALSE;

	return IMG_SUCCESS;
}

IMG_RESULT MC_WBCInit(MC_WBC *pWBC)
{
	int i;

	IMG_MEMSET(pWBC, 0, sizeof(MC_WBC));

	for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
	{
		pWBC->aGain[i] = (MC_FLOAT)1.0;
		pWBC->aClip[i] = (MC_FLOAT)512.0;
	}

	return IMG_SUCCESS;
}

IMG_RESULT MC_DPFInit(MC_DPF *pDPF, const CI_HWINFO *pHWInfo, const MC_IIF *pIIF)
{
	IMG_ASSERT(pDPF != NULL);
	IMG_ASSERT(pIIF != NULL);

	IMG_MEMSET(pDPF, 0, sizeof(MC_DPF));

	pDPF->apDefectInput = NULL;
	//pDPF->ui32NDefect = 0;
    pDPF->ui16NbLines = DPF_WINDOW_HEIGHT(pHWInfo->config_ui8Parallelism);

	pDPF->ui32OutputMapSize = pIIF->ui16ImagerSize[1]*DPF_MAP_MAX_PER_LINE * DPF_MAP_OUTPUT_SIZE;

	pDPF->aSkip[0] = (IMG_UINT8)pIIF->ui16ImagerDecimation[0]; // this may be computable
	pDPF->aSkip[1] = (IMG_UINT8)pIIF->ui16ImagerDecimation[1];

	pDPF->aOffset[0] = pIIF->ui16ImagerOffset[0]; // this may be computable
	pDPF->aOffset[1] = pIIF->ui16ImagerOffset[1];

	pDPF->fWeight = 16.0f; // max
	//pDPF->ui8Threshold = 0;

	return IMG_SUCCESS;
}

void MC_DPFFree(MC_DPF *pDPF)
{
	IMG_ASSERT(pDPF != NULL);

	if ( pDPF->apDefectInput != NULL )
	{
		IMG_FREE(pDPF->apDefectInput);
		pDPF->apDefectInput = NULL;
		pDPF->ui32NDefect = 0;
	}
}

IMG_RESULT MC_LCAInit(MC_LCA *pLCA, const MC_IIF *pIIF)
{
	IMG_UINT32 i;
	IMG_UINT32 cfaSize[2] = { CI_CFA_WIDTH, CI_CFA_HEIGHT }; // research divides by 4...

	if ( pIIF->ui16ImagerSize[0] == 0 || pIIF->ui16ImagerSize[1] == 0 )
	{
	    fprintf(stderr, "IIF should be configured before configuring LCA\n");
	    return IMG_ERROR_INVALID_PARAMETERS;
	}

	IMG_MEMSET(pLCA, 0, sizeof(MC_LCA));

	for ( i = 0 ; i < 2 ; i++ )
	{
		pLCA->aOffsetBlue[i] = (pIIF->ui16ImagerSize[i] + pIIF->ui16ImagerOffset[i])/2;
		pLCA->aOffsetRed[i] = pLCA->aOffsetBlue[i];

		if (pIIF->ui16ImagerSize[i]*cfaSize[i] < LCA_CURVE_MAX)
		{
			//LCA_CURVE_MAX-2 - leave a little bit of headroom
			for (pLCA->aShift[i] = 0;
				(pIIF->ui16ImagerSize[i] << (-pLCA->aShift[i]+1)) < LCA_CURVE_MAX;
				pLCA->aShift[i]--);
		}
		else
		{
			//LCA_CURVE_MAX-2 - leave a little bit of headroom
			for (pLCA->aShift[i] = 0;
				(pIIF->ui16ImagerSize[i] >> (pLCA->aShift[i]+1)) > LCA_CURVE_MAX;
				pLCA->aShift[i]++);
		}
	}

	// coefs are 0

	return IMG_SUCCESS;
}

IMG_RESULT MC_DNSInit(MC_DNS *pDNS)
{
	IMG_MEMSET(pDNS, 0, sizeof(MC_DNS));
    pDNS->fGreyscalePixelThreshold = 0.25;
	return IMG_SUCCESS;
}

IMG_RESULT MC_MGMInit(MC_MGM *pMGM)
{
	IMG_MEMSET(pMGM, 0, sizeof(MC_MGM));

	pMGM->fClipMin = -0.5;
	pMGM->fClipMax = 1.5;
	pMGM->fSrcNorm = 1.0;

	pMGM->aSlope[0] = 1.0;
	pMGM->aSlope[1] = 1.0;
	//pMGM->aSlope[2] = 0.0;

	return IMG_SUCCESS;
}

IMG_RESULT MC_GMAInit(MC_GMA *pGMA)
{
	IMG_MEMSET(pGMA, 0, sizeof(MC_GMA));

	//pGMA->bBypass = IMG_FALSE;
	return IMG_SUCCESS;
}

IMG_RESULT MC_CCMInit(MC_CCM *pCCM)
{
	IMG_UINT32 i;

	IMG_MEMSET(pCCM, 0, sizeof(MC_CCM));

	// identity matrix
	for ( i = 0 ; i < RGB_COEFF_NO ; i++ )
	{
		pCCM->aCoeff[i][i] = (MC_FLOAT)1.0;
	}

	return IMG_SUCCESS;
}

IMG_RESULT MC_R2YInit(MC_R2Y *pR2Y)
{
	IMG_MEMSET(pR2Y, 0, sizeof(MC_R2Y));

	// BT.709 (HD) according to research doc
	pR2Y->aCoeff[0][0] = (MC_FLOAT)0.439;
	pR2Y->aCoeff[0][1] = (MC_FLOAT)-0.399;
	pR2Y->aCoeff[0][2] = (MC_FLOAT)-0.040;

	pR2Y->aCoeff[1][0] = (MC_FLOAT)0.183;
	pR2Y->aCoeff[1][1] = (MC_FLOAT)0.614;
	pR2Y->aCoeff[1][2] = (MC_FLOAT)0.062;

	pR2Y->aCoeff[2][0] = (MC_FLOAT)-0.101;
	pR2Y->aCoeff[2][1] = (MC_FLOAT)-0.338;
	pR2Y->aCoeff[2][2] = (MC_FLOAT)0.439;

	pR2Y->aOffset[1] = (MC_FLOAT)0.5; //moving Y half-range

	return IMG_SUCCESS;
}

IMG_RESULT MC_Y2RInit(MC_Y2R *pY2R)
{
    MC_FLOAT Ymult = 1, Cbmul = 1.8556, Crmult = 1.5748; // to rescale the range of the matrix
	IMG_MEMSET(pY2R, 0, sizeof(MC_Y2R));

	pY2R->aOffset[0] = (MC_FLOAT)-128.0;
	pY2R->aOffset[1] = (MC_FLOAT)-16.0;
	pY2R->aOffset[2] = (MC_FLOAT)-128.0;

	// use BT.709 (HD) according to research doc
    pY2R->aCoeff[0][0] = (MC_FLOAT)1.0*Crmult;
	pY2R->aCoeff[0][1] = (MC_FLOAT)1.0*Ymult;
	pY2R->aCoeff[0][2] = (MC_FLOAT)0.0*Cbmul;

	pY2R->aCoeff[1][0] = (MC_FLOAT)-0.2972595*Crmult;
	pY2R->aCoeff[1][1] = (MC_FLOAT)1.0*Ymult;
	pY2R->aCoeff[1][2] = (MC_FLOAT)-0.1009508*Cbmul;

	pY2R->aCoeff[2][0] = (MC_FLOAT)0.0*Crmult;
	pY2R->aCoeff[2][1] = (MC_FLOAT)1.0*Ymult;
    pY2R->aCoeff[2][2] = (MC_FLOAT)1.0*Cbmul;

	return IMG_SUCCESS;
}

IMG_RESULT MC_MIEInit(MC_MIE *pMie)
{
	IMG_MEMSET(pMie, 0, sizeof(MC_MIE));

	return IMG_SUCCESS;
}

IMG_RESULT MC_TNMInit(MC_TNM *pTNM, const MC_IIF *pIIF, const CI_HWINFO *pHWInfo)
{
#ifndef TNM_NOT_AVAILABLE

	IMG_UINT32 i;
	IMG_INT16 minYin = -127,
		maxYin = 127,
		minCin = -64,
		maxCin = 64;

	IMG_MEMSET(pTNM, 0, sizeof(MC_TNM));
	pTNM->bBypassTNM = IMG_FALSE;

	for ( i = 0 ; i < TNM_CURVE_NPOINTS ; i++ )
	{
		pTNM->aCurve[i] = (i+1)/(MC_FLOAT)(TNM_CURVE_NPOINTS+1);
	}

	pTNM->ui16LocalColumns = TNM_NCOLUMNS;

	pTNM->histFlattenThreshold = (MC_FLOAT)0.0;

	pTNM->chromaSaturationScale = (MC_FLOAT)1.0;
	pTNM->chromaConfigurationScale = (MC_FLOAT)1.0f;
	pTNM->chromaIOScale = (TNM_OUT_MAX_C -TNM_OUT_MIN_C)/((MC_FLOAT)(maxCin-minCin));

	pTNM->inputLumaOffset = (MC_FLOAT)-(-128.0 - minYin);
	pTNM->inputLumaScale = ((MC_FLOAT)256.0)/(maxYin-minYin+1);

	pTNM->outputLumaOffset = (MC_FLOAT)TNM_OUT_MIN_Y;
	pTNM->outputLumaScale = ((TNM_OUT_MAX_Y-TNM_OUT_MIN_Y+1)/((MC_FLOAT)256.0));

#else
	// remove warnings
	(void)pTNM;
	(void)pIIF;
	(void)ui8BitDepth;
#endif // TNM_NOT_AVAILABLE
	return IMG_SUCCESS;
}

IMG_RESULT MC_ESCInit(MC_ESC *pESC, const MC_IIF *pIIF)
{
	IMG_UINT32 i;
	IMG_UINT32 cfaSize[2] = { CI_CFA_WIDTH, CI_CFA_HEIGHT };

	IMG_MEMSET(pESC, 0, sizeof(MC_ESC));

	pESC->eSubsampling = EncOut422;
	pESC->bBypassESC = IMG_FALSE;
	pESC->bAdjustCutoff = IMG_TRUE;
	for ( i = 0 ; i < 2 ; i++ )
	{
		pESC->aOutputSize[i] = pIIF->ui16ImagerSize[i]*cfaSize[i]; // no scaling
		pESC->aPitch[i] = (MC_FLOAT)1.0;
	}

	return IMG_SUCCESS;
}

IMG_RESULT MC_DSCInit(MC_DSC *pDSC, const MC_IIF *pIIF)
{
	IMG_UINT32 i;
	IMG_UINT32 cfaSize[2] = { CI_CFA_WIDTH, CI_CFA_HEIGHT };

	IMG_MEMSET(pDSC, 0, sizeof(MC_DSC));

	pDSC->bBypassDSC = IMG_FALSE;
	pDSC->bAdjustCutoff = IMG_TRUE;
	for ( i = 0 ; i < 2 ; i++ )
	{
		pDSC->aOutputSize[i] = pIIF->ui16ImagerSize[i]*cfaSize[i]; // no scaling
		pDSC->aPitch[i] = (MC_FLOAT)1.0;
	}
	return IMG_SUCCESS;
}

IMG_RESULT MC_DGMInit(MC_DGM *pDGM)
{
	IMG_MEMSET(pDGM, 0, sizeof(MC_DGM));

	pDGM->fClipMin = -0.5;
	pDGM->fClipMax = 1.5;
	pDGM->fSrcNorm = 1.0;

	pDGM->aSlope[0] = 1.0;
	pDGM->aSlope[1] = 1.0;
	//pDGM->aSlope[2] = 0.0;

	return IMG_SUCCESS;
}

IMG_RESULT MC_SHAInit(MC_SHA *pSHA)
{
	IMG_MEMSET(pSHA, 0, sizeof(MC_SHA));

	//pSHA->ui8Threshold = 0;
	//pSHA->fStrength = 0;
	pSHA->fDetail = 1.0;
	//pSHA->bDenoise = IMG_FALSE;

	//pSHA->aGainWeigth[0] = 0;
	pSHA->aGainWeigth[1] = 1/3.0;
	pSHA->aGainWeigth[2] = 1/3.0;

	pSHA->fELWScale = 4.0;
	//pSHA->i8ELWOffset = 0;

	// denoiser not used

	return IMG_SUCCESS;
}

// statistics

IMG_RESULT MC_EXSInit(MC_EXS *pEXS, const MC_IIF *pIIF)
{
	IMG_MEMSET(pEXS, 0, sizeof(MC_EXS));

	pEXS->bGlobalEnable = IMG_TRUE;

	// ROI is center
	pEXS->bRegionEnable = IMG_TRUE;
	pEXS->ui16Top = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/EXS_H_TILES/4;
	pEXS->ui16Left = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/EXS_H_TILES/4;
	pEXS->ui16Width = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/EXS_H_TILES/2; // pixels
	if ( pEXS->ui16Width < EXS_MIN_WIDTH )
	{
		pEXS->ui16Width = EXS_MIN_WIDTH;
	}
	pEXS->ui16Height = pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT/EXS_V_TILES/2; // pixels
	if ( pEXS->ui16Height < EXS_MIN_HEIGHT )
	{
		pEXS->ui16Height = EXS_MIN_HEIGHT;
	}
	pEXS->fPixelMax = 255.0;

	return IMG_SUCCESS;
}

IMG_RESULT MC_FOSInit(MC_FOS *pFOS, const MC_IIF *pIIF)
{
	IMG_MEMSET(pFOS, 0, sizeof(MC_FOS));

	pFOS->bGlobalEnable = IMG_TRUE;
	pFOS->ui16Width = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH; // pixels
	pFOS->ui16Height = pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT; // pixels

	// region of interest is center
	pFOS->bRegionEnable = IMG_TRUE;
	pFOS->ui16ROILeft = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/4;
	pFOS->ui16ROITop = pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT/4;
	pFOS->ui16ROIWidth = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/2;
	pFOS->ui16ROIHeight = pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT/2;

	return IMG_SUCCESS;
}

IMG_RESULT MC_FLDInit(MC_FLD *pFLD, const MC_IIF *pIIF)
{
	IMG_MEMSET(pFLD, 0, sizeof(MC_FLD));

	pFLD->bEnable = IMG_TRUE;
	pFLD->ui16VTot = pIIF->ui16ImagerOffset[1]*CI_CFA_HEIGHT;
	pFLD->fFrameRate = 30.0;
	pFLD->ui16CoefDiff = 50;
	pFLD->ui16NFThreshold = 1500;
	pFLD->ui32SceneChange = 300000;
	pFLD->ui8RShift = 10;
	pFLD->ui8MinPN = 4;
	pFLD->ui8PN = 4;

	return IMG_SUCCESS;
}

IMG_RESULT MC_HISInit(MC_HIS *pHIS, const MC_IIF *pIIF)
{
	IMG_MEMSET(pHIS, 0, sizeof(MC_HIS));

	pHIS->bGlobalEnable = IMG_TRUE;

	pHIS->bRegionEnable = IMG_TRUE;
	pHIS->ui16Left = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/4;
	pHIS->ui16Top = pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT/4;
	pHIS->ui16Width = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/2; // pixels
	pHIS->ui16Height = pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT/2; // pixels

	pHIS->fInputScale = 1.0;

	return IMG_SUCCESS;
}

IMG_RESULT MC_WBSInit(MC_WBS *pWBS, const MC_IIF *pIIF)
{
	int i;
	IMG_MEMSET(pWBS, 0, sizeof(MC_WBS));

	pWBS->ui8ActiveROI = 0; // no ROI enabled
	pWBS->fRGBOffset = 0;
	pWBS->fYOffset = -0.5;

	// thresholds of RGB and Y are 75%
	for ( i = 0 ; i < WBS_NUM_ROI ; i++ )
	{
		pWBS->aRMax[i] = 3*12/4; // register is 12b... that's a bit cheating right now
		pWBS->aGMax[i] = 3*12/4; // register is 12b... that's a bit cheating right now
		pWBS->aBMax[i] = 3*12/4; // register is 12b... that's a bit cheating right now

		pWBS->aYMax[i] = 3*12/4; // register is 12b... that's a bit cheating right now
	}

	// region of interest is the middle
	pWBS->aRoiLeft[0] = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/4;
	pWBS->aRoiTop[0] = pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT/4;
	pWBS->aRoiWidth[0] = pIIF->ui16ImagerSize[0]*CI_CFA_WIDTH/2;
	pWBS->aRoiHeight[0] = pIIF->ui16ImagerSize[1]*CI_CFA_HEIGHT/2;

	return IMG_SUCCESS;
}

IMG_RESULT MC_ENSInit(MC_ENS *pENS)
{
	IMG_MEMSET(pENS, 0, sizeof(MC_ENS));
	pENS->ui32NLines = ENS_NLINES_MIN;
	return IMG_SUCCESS;
}
