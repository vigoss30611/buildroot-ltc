/**
******************************************************************************
 @file mc_extract.c

 @brief implementation of the extraction functions

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

#include <img_errors.h>

// statistics result

IMG_RESULT MC_FOSExtract(const void *blob, MC_STATS_FOS *pFOSStats)
{
	int row,column;
	unsigned int position;

	IMG_UINT32 *pROISharpness=	  (IMG_UINT32 *) ((IMG_UINT8 *)blob+FELIX_SAVE_FOS_ROI_OFFSET);
	IMG_UINT32 *pGridSharpness  = (IMG_UINT32 *) ((IMG_UINT8 *)blob+FELIX_SAVE_FOS_TILE_0_OFFSET);

	unsigned char stride		 = (FELIX_SAVE_EXS_TILE_CC01_0_STRIDE+FELIX_SAVE_EXS_REGION_RESERVED_0_STRIDE)/4;

	unsigned char strideTile	 = FELIX_SAVE_FOS_TILE_0_STRIDE/4;															//divided by 4 to express it in 32bit words
	unsigned char strideRow		 = FELIX_SAVE_FOS_TILE_0_STRIDE*FOS_H_TILES/4;												//divided by 4 to express it in 32bit words
	unsigned char strideReserved = FELIX_SAVE_FOS_GRID_RESERVED_0_STRIDE*FELIX_SAVE_FOS_GRID_RESERVED_0_NO_ENTRIES/4;		//divided by 4 to express it in 32bit words

    IMG_ASSERT(pFOSStats);
    IMG_ASSERT(blob);

	pFOSStats->ROISharpness =  *pROISharpness;

	for(row = 0;row<FOS_V_TILES;row++)
	{
		for(column=0;column<FOS_V_TILES;column++)
		{
				position = row*(strideRow + strideReserved) + column*strideTile;
				pFOSStats->gridSharpness[row][column] = pGridSharpness[position];
		}
	}

	return IMG_SUCCESS;
}


IMG_RESULT MC_HISExtract(const void *blob, MC_STATS_HIS *pHISStats)
{
	int row,column;
	IMG_UINT32 *pGlobalHistBins= (IMG_UINT32 *) ((IMG_UINT8 *)blob+FELIX_SAVE_HIS_GLOBAL_BINS_OFFSET);
	IMG_UINT32 *pRegionHistBins= (IMG_UINT32 *) ((IMG_UINT8 *)blob+FELIX_SAVE_HIS_REGION_0_0_BINS_OFFSET);

    IMG_ASSERT(pHISStats);
    IMG_ASSERT(blob);

	IMG_MEMCPY(pHISStats->globalHistogram, pGlobalHistBins, HIS_GLOBAL_BINS*sizeof(IMG_UINT32));

#if 1
    for(row = 0;row<HIS_REGION_VTILES;row++)
	{
	    for(column=0;column<HIS_REGION_HTILES;column++)
		{
            unsigned int position = ((row*HIS_REGION_HTILES)+column)*HIS_REGION_BINS;
            IMG_MEMCPY(pHISStats->regionHistograms[row][column], &pRegionHistBins[position], HIS_REGION_BINS*sizeof(IMG_UINT32));
		}
	}
#else
    // this memcpy could be used if we are sure that the static array has the same alignment (it is a lot faster!)
    IMG_MEMCPY(pHISStats->regionHistograms, pRegionHistBins, HIS_REGION_HTILES*HIS_REGION_VTILES*HIS_REGION_BINS*sizeof(IMG_UINT32));
#endif

	return IMG_SUCCESS;
}

IMG_RESULT MC_EXSExtract(const void *blob, MC_STATS_EXS *pEXSStats)
{
	int row,column;
	int channel;
	int numChannels = 4;
	IMG_UINT32 *pGlobalEXS= (IMG_UINT32 *) ((IMG_UINT8 *)blob+FELIX_SAVE_EXS_GLOBAL_CC_OFFSET);
	IMG_UINT16 *pRegionEXS= (IMG_UINT16 *) ((IMG_UINT8 *)blob+FELIX_SAVE_EXS_TILE_CC01_0_OFFSET);
	unsigned char stride = (FELIX_SAVE_EXS_TILE_CC01_0_STRIDE+FELIX_SAVE_EXS_REGION_RESERVED_0_STRIDE)/2;

	unsigned char strideTile	 = FELIX_SAVE_EXS_TILE_CC01_0_STRIDE/2; //divided by 2 to express it in 16bit words
	unsigned char strideRow		 = FELIX_SAVE_EXS_TILE_CC01_0_STRIDE*HIS_REGION_HTILES/2;//divided by 2 to express it in 16bit words
	unsigned char strideReserved = FELIX_SAVE_EXS_REGION_RESERVED_0_STRIDE*FELIX_SAVE_EXS_REGION_RESERVED_0_NO_ENTRIES/2; //divided by 2 to express it in 16bit words

    IMG_ASSERT(pEXSStats);
    IMG_ASSERT(blob);

    IMG_MEMCPY(pEXSStats->globalClipped, pGlobalEXS, numChannels*sizeof(IMG_UINT32));

	for(row = 0;row<EXS_V_TILES;row++)
	{
		for(column=0;column<EXS_H_TILES;column++)
		{
            unsigned int position = row*(strideRow + strideReserved) + column*strideTile;
			for(channel =0; channel < numChannels; channel++)
			{
				pEXSStats->regionClipped[row][column][channel] = pRegionEXS[position + channel];
			}
		}
	}

	return IMG_SUCCESS;
}


IMG_RESULT MC_WBSExtract(const void *blob, MC_STATS_WBS *pWBSStats)
{
	int roi;
    IMG_ASSERT(pWBSStats);
    IMG_ASSERT(blob);

	for(roi = 0; roi< WBS_NUM_ROI;roi++)
	{

		pWBSStats->averageColor[roi].channelAccumulated[0]	= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_AC_RED_ACC_OFFSET));
		pWBSStats->averageColor[roi].channelAccumulated[1]	= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_AC_GRN_ACC_OFFSET));
		pWBSStats->averageColor[roi].channelAccumulated[2]	= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_AC_BLU_ACC_OFFSET));

		pWBSStats->whitePatch[roi].channelAccumulated[0]		= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_WP_RED_ACC_OFFSET));
		pWBSStats->whitePatch[roi].channelAccumulated[1]		= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_WP_GRN_ACC_OFFSET));
		pWBSStats->whitePatch[roi].channelAccumulated[2]		= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_WP_BLU_ACC_OFFSET));
		pWBSStats->whitePatch[roi].channelCount[0]			= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_WP_RED_CNT_OFFSET));
		pWBSStats->whitePatch[roi].channelCount[1]			= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_WP_GRN_CNT_OFFSET));
		pWBSStats->whitePatch[roi].channelCount[2]			= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_WP_BLU_CNT_OFFSET));

		pWBSStats->highLuminance[roi].channelAccumulated[0]	= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_HLW_RED_ACC_OFFSET));
		pWBSStats->highLuminance[roi].channelAccumulated[1]	= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_HLW_GRN_ACC_OFFSET));
		pWBSStats->highLuminance[roi].channelAccumulated[2]	= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_HLW_BLU_ACC_OFFSET));
		pWBSStats->highLuminance[roi].lumaCount				= *((IMG_UINT32 *) ((IMG_UINT8 *)blob+ roi*FELIX_SAVE_WBS_AC_RED_ACC_STRIDE+FELIX_SAVE_WBS_HLW_LUM_CNT_OFFSET));
	}

	return IMG_SUCCESS;
}

IMG_RESULT MC_FLDExtract(const void *blob, MC_STATS_FLD *pFLDStats)
{
    IMG_ASSERT(pFLDStats);
    IMG_ASSERT(blob);

	pFLDStats->flickerStatus = (FLD_STATUS) (*((IMG_UINT8 *)blob + FELIX_SAVE_FLICKER_OFFSET) & FELIX_SAVE_FLICKER_FLICKER_MASK);

	return IMG_SUCCESS;
}

IMG_RESULT MC_TimestampExtract(const void *blob, MC_STATS_TIMESTAMP *pTimestamps)
{
    const IMG_UINT32 *pBlob = (const IMG_UINT32*)blob;

    IMG_ASSERT(pTimestamps);
    IMG_ASSERT(blob);

    pTimestamps->ui32LinkedListUpdated = 0; // not in the HW data
    pTimestamps->ui32StartFrameIn = pBlob[FELIX_SAVE_TIME_STAMP_0_OFFSET/4];
    pTimestamps->ui32EndFrameIn = pBlob[FELIX_SAVE_TIME_STAMP_1_OFFSET/4];
    pTimestamps->ui32StartFrameOut = pBlob[FELIX_SAVE_TIME_STAMP_2_OFFSET/4];
    pTimestamps->ui32EndFrameEncOut = pBlob[FELIX_SAVE_TIME_STAMP_3_OFFSET/4];
    pTimestamps->ui32EndFrameDispOut = pBlob[FELIX_SAVE_TIME_STAMP_4_OFFSET/4];
    pTimestamps->ui32InterruptServiced = 0; // not in the HW data

    return IMG_SUCCESS;
}

IMG_RESULT MC_DPFExtract(const void *blob, MC_STATS_DPF *pDPFStats)
{
    const IMG_UINT32 *pBlob = (const IMG_UINT32*)blob;

    IMG_ASSERT(pDPFStats);
    IMG_ASSERT(blob);

    pDPFStats->ui32FixedPixels = pBlob[FELIX_SAVE_DPF_FIXED_PIXELS_OFFSET/4];
    pDPFStats->ui32MapModifications = pBlob[FELIX_SAVE_DPF_MAP_MODIFICATIONS_OFFSET/4];
    pDPFStats->ui32DroppedMapModifications = pBlob[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET/4];

    // computed values
    pDPFStats->ui32NOutCorrection = pDPFStats->ui32MapModifications - pDPFStats->ui32DroppedMapModifications;

    return IMG_SUCCESS;
}
