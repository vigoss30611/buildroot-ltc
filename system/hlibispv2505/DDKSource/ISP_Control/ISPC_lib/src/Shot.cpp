/**
******************************************************************************
 @file Shot.cpp

 @brief Implementation of ISPC::Shot

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
#include "ispc/Shot.h"

#include <ci/ci_api.h>
#include <mc/module_config.h>

#include <hw_struct/save.h>

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_SHOT"

#include "ispc/ModuleOUT.h"
#include "ispc/Pipeline.h"

void ISPC::Shot::clear()
{
    IMG_MEMSET(&(BAYER), 0, sizeof(Buffer));
    BAYER.pxlFormat = PXL_NONE;

    IMG_MEMSET(&(DISPLAY), 0, sizeof(Buffer));
    DISPLAY.pxlFormat = PXL_NONE;

    IMG_MEMSET(&(YUV), 0, sizeof(Buffer));
    YUV.pxlFormat = PXL_NONE;

    IMG_MEMSET(&(HDREXT), 0, sizeof(Buffer));
    HDREXT.pxlFormat = PXL_NONE;

    IMG_MEMSET(&(RAW2DEXT), 0, sizeof(Buffer));
    RAW2DEXT.pxlFormat = PXL_NONE;

    pCIBuffer = 0;

    IMG_MEMSET(&(metadata), 0, sizeof(Metadata));

    IMG_MEMSET(&(DPF), 0, sizeof(Statistics));
    IMG_MEMSET(&(ENS), 0, sizeof(Statistics));
}

void ISPC::Shot::configure(CI_SHOT *pCI_Buffer,
    const ModuleOUT &globalConfig, const Global_Setup &globalSetup)
{
    PIXELTYPE stype;
    IMG_RESULT ret;
    this->pCIBuffer = pCI_Buffer;

    // Configure the fields
    ret = PixelTransformDisplay(&stype, globalConfig.displayType);
    if (IMG_SUCCESS == ret)
    {
        // if it is RGB
        this->DISPLAY.data =
            reinterpret_cast<const IMG_UINT8 *>(pCIBuffer->pDisplayOutput);
#ifdef INFOTM_ISP			
		// add physical address	
		this->DISPLAY.phyAddr = pCI_Buffer->pDisplayOutputPhyAddr;
#endif //INFOTM_ISP		

        this->DISPLAY.stride = pCIBuffer->aDispSize[0];
        this->DISPLAY.vstride = pCIBuffer->aDispSize[1];

        this->DISPLAY.height = globalSetup.ui32DispHeight;
        this->DISPLAY.width  = globalSetup.ui32DispWidth;

        this->DISPLAY.pxlFormat = globalConfig.displayType;
        this->DISPLAY.id = pCIBuffer->dispId;
        this->DISPLAY.isTiled = pCIBuffer->bDispTiled == IMG_TRUE;
    }
    else
    {
        ret = PixelTransformBayer(&stype, globalConfig.dataExtractionType,
            MOSAIC_RGGB);
        if (IMG_SUCCESS == ret)
        {
            // if it is bayer
            this->BAYER.data =
                reinterpret_cast<const IMG_UINT8 *>(pCIBuffer->pDisplayOutput);
#ifdef INFOTM_ISP	
			this->BAYER.phyAddr = pCI_Buffer->pDisplayOutputPhyAddr;
#endif //INFOTM_ISP	
	
            this->BAYER.stride = pCIBuffer->aDispSize[0];
            this->BAYER.vstride = pCIBuffer->aDispSize[1];

            this->BAYER.height = globalSetup.ui32ImageHeight;
            this->BAYER.width = globalSetup.ui32ImageWidth;

            this->BAYER.pxlFormat = globalConfig.dataExtractionType;
            this->BAYER.id = pCIBuffer->dispId;
            // although Bayer should never be tiled
            this->BAYER.isTiled = pCIBuffer->bDispTiled == IMG_TRUE;
        }
        // else neither RGB nor Bayer
    }

    if (PXL_NONE != globalConfig.encoderType)
    {
        this->YUV.data =
            reinterpret_cast<const IMG_UINT8 *>(pCIBuffer->pEncoderOutput);
#ifdef INFOTM_ISP		
		this->YUV.phyAddr = pCI_Buffer->pEncoderOutputPhyAddr;
#endif //INFOTM_ISP			

        this->YUV.offset = pCIBuffer->aEncOffset[0];
        this->YUV.stride = pCIBuffer->aEncYSize[0];
        this->YUV.vstride = pCIBuffer->aEncYSize[1];

        this->YUV.strideCbCr = pCIBuffer->aEncCbCrSize[0];
        this->YUV.vstrideCbCr = pCIBuffer->aEncCbCrSize[1];
        this->YUV.offsetCbCr = pCIBuffer->aEncOffset[1];
        // used to be pCIBuffer->aEncYSize[0]*pCIBuffer->aEncYSize[1];

        this->YUV.height = globalSetup.ui32EncHeight;
        this->YUV.width  = globalSetup.ui32EncWidth;

        this->YUV.pxlFormat = globalConfig.encoderType;
        this->YUV.id = pCIBuffer->encId;
        this->YUV.isTiled = pCIBuffer->bEncTiled == IMG_TRUE;
    }

    if (PXL_NONE != globalConfig.hdrExtractionType)
    {
        this->HDREXT.data =
            reinterpret_cast<const IMG_UINT8 *>(pCIBuffer->pHDRExtOutput);
#ifdef INFOTM_ISP		
		this->HDREXT.phyAddr = pCI_Buffer->pHDRExtOutputPhyAddr;
#endif //INFOTM_ISP			

        this->HDREXT.stride = pCIBuffer->aHDRExtSize[0];
        this->HDREXT.vstride = pCIBuffer->aHDRExtSize[1];

        this->HDREXT.height = globalSetup.ui32ImageHeight;
        this->HDREXT.width = globalSetup.ui32ImageWidth;

        this->HDREXT.pxlFormat = globalConfig.hdrExtractionType;
        this->HDREXT.id = pCIBuffer->HDRExtId;
        this->HDREXT.isTiled = pCIBuffer->bHDRExtTiled == IMG_TRUE;
    }

    if (PXL_NONE != globalConfig.raw2DExtractionType)
    {
        this->RAW2DEXT.data =
            reinterpret_cast<const IMG_UINT8 *>(pCIBuffer->pRaw2DExtOutput);
#ifdef INFOTM_ISP	
		this->RAW2DEXT.phyAddr = pCI_Buffer->pRaw2DExtOutputPhyAddr;
#endif //INFOTM_ISP		

        this->RAW2DEXT.stride = pCIBuffer->aRaw2DSize[0];
        this->RAW2DEXT.vstride = pCIBuffer->aRaw2DSize[1];

        this->RAW2DEXT.height = globalSetup.ui32ImageHeight;
        this->RAW2DEXT.width = globalSetup.ui32ImageWidth;

        this->RAW2DEXT.pxlFormat = globalConfig.raw2DExtractionType;
        this->RAW2DEXT.id = pCIBuffer->raw2DExtId;
        this->RAW2DEXT.isTiled = false;  // raw2D cannot be tiled
    }

    // transform to bool
    this->bFrameError = pCIBuffer->bFrameError == IMG_TRUE;
    this->iMissedFrames = pCIBuffer->i32MissedFrames;

    CI_PIPELINE_STATS config;

    // Extract the statistics configuration from the load structure
    CI_PipelineExtractConfig(pCIBuffer, &config);
    
    // Extract metadata from captured buffer
    MC_HISExtract(pCIBuffer->pStatistics, &(metadata.histogramStats));
    MC_HISRevert(&(config.sHistStats), pCIBuffer->eOutputConfig,
        &(metadata.histogramConfig));

    MC_EXSExtract(pCIBuffer->pStatistics, &(metadata.clippingStats));
    MC_EXSRevert(&(config.sExposureStats), pCIBuffer->eOutputConfig,
        &(metadata.clippingConfig));

    MC_FOSExtract(pCIBuffer->pStatistics, &(metadata.focusStats));
    MC_FOSRevert(&(config.sFocusStats), pCIBuffer->eOutputConfig,
        &(metadata.focusConfig));

    MC_WBSExtract(pCIBuffer->pStatistics, &(metadata.whiteBalanceStats));
    MC_WBSRevert(&(config.sWhiteBalanceStats), pCIBuffer->eOutputConfig,
        &(metadata.whiteBalanceConfig));

    MC_AWSExtract(pCIBuffer->pStatistics, &(metadata.autoWhiteBalanceStats));
    MC_AWSRevert(&(config.sAutoWhiteBalanceStats), pCIBuffer->eOutputConfig,
        &(metadata.autoWhiteBalanceConfig));

    MC_FLDExtract(pCIBuffer->pStatistics, &(metadata.flickerStats));
    MC_FLDRevert(&(config.sFlickerStats), pCIBuffer->eOutputConfig,
        &(metadata.flickerConfig));

#ifdef INFOTM_HW_AWB_METHOD
    // cpy HW_AWB statistics to metadata
    IMG_UINT8 * meta_hw_awb_addr = (IMG_UINT8*)pCIBuffer->pStatistics + FELIX_SAVE_BYTE_SIZE;
    memcpy(&metadata.hwAwbConfig, meta_hw_awb_addr, sizeof(MC_HW_AWB));
#endif //INFOTM_HW_AWB_METHOD

    // could also extra ENS config

    MC_TimestampExtract(pCIBuffer->pStatistics, &(metadata.timestamps));
    metadata.timestamps.ui32LinkedListUpdated = pCI_Buffer->ui32LinkedListTS;
    metadata.timestamps.ui32InterruptServiced = pCI_Buffer->ui32InterruptTS;
#ifdef INFOTM_ISP
    metadata.timestamps.ui64CurSystemTS = pCI_Buffer->ui64SystemTS;
#endif

    MC_DPFExtract(pCIBuffer->pStatistics, &(metadata.defectiveStats));

    // Extract additional statistics
    stats.data = reinterpret_cast<const IMG_UINT8 *>(pCIBuffer->pStatistics);
    stats.stride = pCIBuffer->statsSize;
    stats.size = FELIX_SAVE_BYTE_SIZE;
    stats.elementSize = sizeof(IMG_UINT32);

    if (pCIBuffer->uiDPFSize > 0)
    {
        IMG_UINT32 nCorr = metadata.defectiveStats.ui32NOutCorrection;

        if (nCorr*DPF_MAP_OUTPUT_SIZE > pCIBuffer->uiDPFSize)
        {
            LOG_WARNING("nb of correct pixels given by HW does not fit "\
                "in buffer! DPF buffer not loaded\n");
        }
        else
        {
            DPF.data = reinterpret_cast<const IMG_UINT8 *>(pCIBuffer->pDPFMap);
            DPF.stride = pCIBuffer->uiDPFSize;
            DPF.size = nCorr*DPF_MAP_OUTPUT_SIZE;
            // several fields - see HW TRM about DPF extraction
            DPF.elementSize = DPF_MAP_OUTPUT_SIZE;
        }
    }
    if (pCIBuffer->uiENSSize > 0)
    {
        ENS.data = reinterpret_cast<const IMG_UINT8 *>(pCIBuffer->pENSOutput);
        ENS.stride = pCIBuffer->uiENSSize;
        ENS.size = pCIBuffer->uiENSSize;
        // coutner + entropy - see HW TRM about Encoder Statistics
        ENS.elementSize = ENS_OUT_SIZE;
    }
}

/*
 * ISPC::Buffer
 */

const IMG_UINT8* ISPC::Buffer::firstData() const
{
    return data + offset;
}

const IMG_UINT8* ISPC::Buffer::firstDataCbCr() const
{
    PIXELTYPE type;
    IMG_RESULT ret = PixelTransformYUV(&type, pxlFormat);
    if (ret)
    {
        // not a YUV format
        return NULL;
    }
    return data + offsetCbCr;
}

