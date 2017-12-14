/**
*******************************************************************************
@file ModuleOUT.cpp

@brief Implementation of ISPC::ModuleOUT

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
#include "ispc/ModuleOUT.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_GLOB"

#include <string>

#include "ispc/Pipeline.h"

// BRN58117 need to access stats modules
#include "ispc/ModuleFOS.h"
#include "ispc/ModuleWBS.h"
#include "ispc/ModuleHIS.h"
#include "ispc/ModuleFLD.h"
#include "ispc/ModuleDPF.h"

#include "ispc/PerfTime.h"

#define PXLNONE_STR "NONE"

#define YUV422PL12YUV8_STR "422PL12YUV8"
#define YUV420PL12YUV8_STR "420PL12YUV8"
#define YUV422PL12YUV10_STR "422PL12YUV10"
#define YUV420PL12YUV10_STR "420PL12YUV10"

static const char *NV61_8 = FormatString(YVU_422_PL12_8);
static const char *NV16_8 = FormatString(YUV_422_PL12_8);
static const char *NV21_8 = FormatString(YVU_420_PL12_8);
static const char *NV12_8 = FormatString(YUV_420_PL12_8);

static const char *NV61_10 = FormatString(YVU_422_PL12_10);
static const char *NV16_10 = FormatString(YUV_422_PL12_10);
static const char *NV21_10 = FormatString(YVU_420_PL12_10);
static const char *NV12_10 = FormatString(YUV_420_PL12_10);

static const char * FMT_444IL3YCrCb8_STR= FormatString(PXL_ISP_444IL3YCrCb8);
static const char * FMT_444IL3YCbCr8_STR = FormatString(PXL_ISP_444IL3YCbCr8);
static const char * FMT_444IL3YCrCb10_STR = FormatString(PXL_ISP_444IL3YCrCb10);
static const char * FMT_444IL3YCbCr10_STR = FormatString(PXL_ISP_444IL3YCbCr10);

#define RGB888_24_STR "RGB888_24"
#define BGR888_24_STR "BGR888_24"
#define RGB888_32_STR "RGB888_32"
#define BGR888_32_STR "BGR888_32"
#define RGB101010_32_STR "RGB101010_32"
#define BGR101010_32_STR "BGR101010_32"
#define BGR161616_64_STR "BGR161616_64"

// #define DE_NONE_STR "NONE"
#define DE_BAYER8_STR "BAYER8"
#define DE_BAYER10_STR "BAYER10"
#define DE_BAYER12_STR "BAYER12"
#define DE_TIFF10_STR "TIFF10"
#define DE_TIFF12_STR "TIFF12"

const ISPC::ParamDefSingle<std::string> ISPC::ModuleOUT::ENCODER_TYPE(
    "OUT_ENC", PXLNONE_STR);
const ISPC::ParamDefSingle<std::string> ISPC::ModuleOUT::DISPLAY_TYPE(
    "OUT_DISP", PXLNONE_STR);
const ISPC::ParamDefSingle<std::string> ISPC::ModuleOUT::DATAEXTRA_TYPE(
    "OUT_DE", PXLNONE_STR);
const ISPC::ParamDef<int> ISPC::ModuleOUT::DATAEXTRA_POINT(
    "OUT_DE_POINT", 1, 3, 1);
const ISPC::ParamDefSingle<std::string> ISPC::ModuleOUT::HDREXTRA_TYPE(
    "OUT_DE_HDF", PXLNONE_STR);
const ISPC::ParamDefSingle<std::string> ISPC::ModuleOUT::HDRINS_TYPE(
    "OUT_DI_HDF", PXLNONE_STR);
const ISPC::ParamDefSingle<std::string> ISPC::ModuleOUT::RAW2DEXTRA_TYPE(
    "OUT_DE_RAW2D", PXLNONE_STR);

ISPC::ParameterGroup ISPC::ModuleOUT::getGroup()
{
    ParameterGroup group;

    group.header = "// Output formats parameters";

    group.parameters.insert(ENCODER_TYPE.name);
    group.parameters.insert(DISPLAY_TYPE.name);
    group.parameters.insert(DATAEXTRA_TYPE.name);
    group.parameters.insert(DATAEXTRA_POINT.name);
    group.parameters.insert(HDREXTRA_TYPE.name);
    group.parameters.insert(HDRINS_TYPE.name);
    group.parameters.insert(RAW2DEXTRA_TYPE.name);

    return group;
}

ePxlFormat ISPC::ModuleOUT::getPixelFormatDisplay(const std::string &stringFormat,
        bool failIfInvalid)
{
    if (0 == stringFormat.compare(FMT_444IL3YCrCb8_STR))
    {
        return PXL_ISP_444IL3YCrCb8;
    }
    else if (0 == stringFormat.compare(FMT_444IL3YCbCr8_STR))
    {
        return PXL_ISP_444IL3YCbCr8;
    }
    else if (0 == stringFormat.compare(FMT_444IL3YCrCb10_STR))
    {
        return PXL_ISP_444IL3YCrCb10;
    }
    else if (0 == stringFormat.compare(FMT_444IL3YCbCr10_STR))
    {
        return PXL_ISP_444IL3YCbCr10;
    }
    // RGB formats
    else if (0 == stringFormat.compare(RGB888_24_STR))
    {
        return RGB_888_24;
    }
    else if (0 == stringFormat.compare(BGR888_24_STR))
    {
        return BGR_888_24;
    }
    else if (0 == stringFormat.compare(RGB888_32_STR))
    {
        return RGB_888_32;
    }
    else if (0 == stringFormat.compare(BGR888_32_STR))
    {
        return BGR_888_32;
    }
    else if (0 == stringFormat.compare(RGB101010_32_STR))
    {
        return RGB_101010_32;
    }
    else if (0 == stringFormat.compare(BGR101010_32_STR))
    {
        return BGR_101010_32;
    }
    else if (0 == stringFormat.compare(BGR161616_64_STR))
    {
        return BGR_161616_64;
    }
    else if (0 == stringFormat.compare(PXLNONE_STR))
    {
        return PXL_NONE;
    }

    /* not a valid pixel format */
    if(failIfInvalid) {
        return PXL_INVALID;
    } else {
        return PXL_NONE;
    }
}

ePxlFormat ISPC::ModuleOUT::getPixelFormatEncoder(const std::string &stringFormat,
        bool failIfInvalid)
{
    // old YUV formats (CSIM internal driver formats)
    if (0 == stringFormat.compare(YUV420PL12YUV8_STR))
    {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
        LOG_WARNING("deprecated %s format used - use %s instead\n",
            YUV422PL12YUV8_STR, NV61_8);
#endif //INFOTM_ENABLE_WARNING_DEBUG
        return YVU_420_PL12_8;
    }
    else if (0 == stringFormat.compare(YUV422PL12YUV8_STR))
    {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
        LOG_WARNING("deprecated %s format used - use %s instead\n",
            YUV422PL12YUV8_STR, NV61_8);
#endif //INFOTM_ENABLE_WARNING_DEBUG
        return YVU_422_PL12_8;
    }
    else if (0 == stringFormat.compare(YUV420PL12YUV10_STR))
    {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
        LOG_WARNING("deprecated %s format used - use %s instead\n",
            YUV420PL12YUV10_STR, NV21_8);
#endif //INFOTM_ENABLE_WARNING_DEBUG
        return YVU_420_PL12_10;
    }
    else if (0 == stringFormat.compare(YUV422PL12YUV10_STR))
    {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
        LOG_WARNING("deprecated %s format used - use %s instead\n",
            YUV422PL12YUV10_STR, NV61_10);
#endif //INFOTM_ENABLE_WARNING_DEBUG
        return YVU_422_PL12_10;
    }
    // YUV formats
    else if (0 == stringFormat.compare(NV21_8))
    {
        return YVU_420_PL12_8;
    }
    else if (0 == stringFormat.compare(NV61_8))
    {
        return YVU_422_PL12_8;
    }
    else if (0 == stringFormat.compare(NV21_10))
    {
        return YVU_420_PL12_10;
    }
    else if (0 == stringFormat.compare(NV61_10))
    {
        return YVU_422_PL12_10;
    }
    else if (0 == stringFormat.compare(NV12_8))
    {
        return YUV_420_PL12_8;
    }
    else if (0 == stringFormat.compare(NV16_8))
    {
        return YUV_422_PL12_8;
    }
    else if (0 == stringFormat.compare(NV12_10))
    {
        return YUV_420_PL12_10;
    }
    else if (0 == stringFormat.compare(NV16_10))
    {
        return YUV_422_PL12_10;
    }
    else if (0 == stringFormat.compare(PXLNONE_STR))
    {
        return PXL_NONE;
    }

    /* not a valid pixel format */
    if(failIfInvalid) {
        return PXL_INVALID;
    } else {
        return PXL_NONE;
    }
}

ePxlFormat ISPC::ModuleOUT::getPixelFormatBayer(const std::string &stringFormat,
        bool failIfInvalid)
{
    // Bayer formats
    if (0 == stringFormat.compare(DE_BAYER8_STR))
    {
        return BAYER_RGGB_8;
    }
    else if (0 == stringFormat.compare(DE_BAYER10_STR))
    {
        return BAYER_RGGB_10;
    }
    else if (0 == stringFormat.compare(DE_BAYER12_STR))
    {
        return BAYER_RGGB_12;
    }
    else if (0 == stringFormat.compare(DE_TIFF10_STR))
    {
        return BAYER_TIFF_10;
    }
    else if (0 == stringFormat.compare(DE_TIFF12_STR))
    {
        return BAYER_TIFF_12;
    }
    else if (0 == stringFormat.compare(PXLNONE_STR))
    {
        return PXL_NONE;
    }

    /* not a valid pixel format */
    if(failIfInvalid) {
        return PXL_INVALID;
    } else {
        return PXL_NONE;
    }
}

ePxlFormat ISPC::ModuleOUT::getPixelFormat(const std::string &stringFormat)
{
    ePxlFormat ret;
    ret = getPixelFormatEncoder(stringFormat, false);
    if(PXL_NONE == ret)
    {
        ret = getPixelFormatDisplay(stringFormat, false);
    }
    if(PXL_NONE == ret)
    {
        ret = getPixelFormatBayer(stringFormat, false);
    }
    return ret;
}

std::string ISPC::ModuleOUT::getPixelFormatString(ePxlFormat fmt)
{
    switch (fmt)
    {
        // firstly handle types overridden by ISPC
    case YVU_420_PL12_8:
        return NV21_8;  // legacy used to be YUV420PL12YUV8_STR;
    case YVU_422_PL12_8:
        return NV61_8;  // legacy used to be YUV422PL12YUV8_STR;
    case YVU_420_PL12_10:
        return NV21_10;  // legacy used to be YUV420PL12YUV10_STR;
    case YVU_422_PL12_10:
        return NV61_10;  // legacy used to be YUV422PL12YUV10_STR;
    case YUV_420_PL12_8:
        return NV12_8;
    case YUV_422_PL12_8:
        return NV16_8;
    case YUV_420_PL12_10:
        return NV12_10;
    case YUV_422_PL12_10:
        return NV16_10;

    case RGB_888_24:
        return RGB888_24_STR;
    case BGR_888_24:
        return BGR888_24_STR;
    case RGB_888_32:
        return RGB888_32_STR;
    case BGR_888_32:
        return BGR888_32_STR;
    case RGB_101010_32:
        return RGB101010_32_STR;
    case BGR_101010_32:
        return BGR101010_32_STR;
    case BGR_161616_64:
        return BGR161616_64_STR;

    // Bayer
    case BAYER_RGGB_8:
        return DE_BAYER8_STR;
    case BAYER_RGGB_10:
        return DE_BAYER10_STR;
    case BAYER_RGGB_12:
        return DE_BAYER12_STR;

    // Bayer TIFF
    case BAYER_TIFF_10:
        return DE_TIFF10_STR;
    case BAYER_TIFF_12:
        return DE_TIFF12_STR;

    case PXL_NONE:
        return PXLNONE_STR;

    // determine strings of other native CI formats, if not listed above
    default:
        return FormatString(fmt);
    }
}

ISPC::ModuleOUT::ModuleOUT(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleOUT::load(const ParameterList &parameters)
{
    encoderType = getPixelFormatEncoder(parameters.getParameter(ENCODER_TYPE));

    displayType = getPixelFormatDisplay(parameters.getParameter(DISPLAY_TYPE));

    dataExtractionType =
        getPixelFormat(parameters.getParameter(DATAEXTRA_TYPE));
    dataExtractionPoint = static_cast<CI_INOUT_POINTS>(
        parameters.getParameter(DATAEXTRA_POINT) - 1);
    if (PXL_NONE == dataExtractionType
        || CI_INOUT_NONE == dataExtractionPoint)
    {
        if (PXL_NONE != dataExtractionType
            || CI_INOUT_NONE != dataExtractionPoint)
        {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            MOD_LOG_WARNING("DE point %d forced to NONE and DE format %s "\
                "forced to NONE because one of them is NONE\n",
                dataExtractionPoint, FormatString(dataExtractionType));
#endif //INFOTM_ENABLE_WARNING_DEBUG
        }
        dataExtractionType = PXL_NONE;
        dataExtractionPoint = CI_INOUT_NONE;
    }

    hdrExtractionType =
        getPixelFormat(parameters.getParameter(HDREXTRA_TYPE));
    hdrInsertionType =
        getPixelFormat(parameters.getParameter(HDRINS_TYPE));
    raw2DExtractionType =
        getPixelFormat(parameters.getParameter(RAW2DEXTRA_TYPE));

    if (PXL_INVALID == encoderType ||
        PXL_INVALID == displayType ||
        PXL_INVALID == dataExtractionType ||
        PXL_INVALID == hdrExtractionType ||
        PXL_INVALID == hdrInsertionType ||
        PXL_INVALID == raw2DExtractionType)
    {
        LOG_ERROR("INVALID PIXEL FORMAT \n");
        IMG_ASSERT(1==0);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleOUT::save(ParameterList &parameters, SaveType t) const
{
    LOG_PERF_IN();
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleOUT::getGroup();
    }

    parameters.addGroup("ModuleOUT", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(ENCODER_TYPE,
            getPixelFormatString(this->encoderType));
        parameters.addParameter(DISPLAY_TYPE,
            getPixelFormatString(this->displayType));
        parameters.addParameter(DATAEXTRA_TYPE,
            getPixelFormatString(this->dataExtractionType));
        parameters.addParameter(DATAEXTRA_POINT,
            static_cast<int>(this->dataExtractionPoint) + 1);
        parameters.addParameter(HDREXTRA_TYPE,
            getPixelFormatString(this->hdrExtractionType));
        parameters.addParameter(HDRINS_TYPE,
            getPixelFormatString(this->hdrInsertionType));
        parameters.addParameter(RAW2DEXTRA_TYPE,
            getPixelFormatString(this->raw2DExtractionType));
        break;

    case SAVE_MIN:
        parameters.addParameterMin(ENCODER_TYPE);  // pixel format does not have a min
        parameters.addParameterMin(DISPLAY_TYPE);  // pixel format does not have a min
        parameters.addParameterMin(DATAEXTRA_TYPE);  // pixel format does not have a min
        parameters.addParameterMin(DATAEXTRA_POINT);
        parameters.addParameterMin(HDREXTRA_TYPE);  // pixel format does not have a min
        // pixel format does not have a min
        parameters.addParameterMin(HDRINS_TYPE);
        parameters.addParameterMin(RAW2DEXTRA_TYPE);  // pixel format does not have a min
        break;

    case SAVE_MAX:
        parameters.addParameterMax(ENCODER_TYPE);  // pixel format does not have a max
        parameters.addParameterMax(DISPLAY_TYPE);  // pixel format does not have a max
        parameters.addParameterMax(DATAEXTRA_TYPE);  // pixel format does not have a max
        parameters.addParameterMax(DATAEXTRA_POINT);
        parameters.addParameterMax(HDREXTRA_TYPE);  // pixel format does not have a max
        // pixel format does not have a max
        parameters.addParameterMax(HDRINS_TYPE);
        parameters.addParameterMax(RAW2DEXTRA_TYPE);  // pixel format does not have a max
        break;

    case SAVE_DEF:
        {
            std::ostringstream defComment;

            defComment.str("");
            defComment  << "{" << getPixelFormatString(PXL_NONE)
                << ", " << getPixelFormatString(YVU_420_PL12_8)
                << ", " << getPixelFormatString(YUV_420_PL12_8)
                << ", " << getPixelFormatString(YVU_422_PL12_8)
                << ", " << getPixelFormatString(YUV_422_PL12_8)
                << ", " << getPixelFormatString(YVU_420_PL12_10)
                << ", " << getPixelFormatString(YUV_420_PL12_10)
                << ", " << getPixelFormatString(YVU_422_PL12_10)
                << ", " << getPixelFormatString(YUV_422_PL12_10)
                << "}";
            parameters.addParameterDef(ENCODER_TYPE);
            parameters.getParameter(ENCODER_TYPE.name)->setInfo(
                    defComment.str());

            defComment.str("");
            defComment  << "{" << getPixelFormatString(PXL_NONE)
                << ", " << getPixelFormatString(RGB_888_24)
                << ", " << getPixelFormatString(RGB_888_32)
                << ", " << getPixelFormatString(RGB_101010_32)
                << ", " << getPixelFormatString(BGR_888_24)
                << ", " << getPixelFormatString(BGR_888_32)
                << ", " << getPixelFormatString(BGR_101010_32)
                // packed YUV 444
                << ", " << getPixelFormatString(PXL_ISP_444IL3YCrCb8)
                << ", " << getPixelFormatString(PXL_ISP_444IL3YCbCr8)
                << ", " << getPixelFormatString(PXL_ISP_444IL3YCrCb10)
                << ", " << getPixelFormatString(PXL_ISP_444IL3YCbCr10)
                << "}";
            parameters.addParameterDef(DISPLAY_TYPE);
            parameters.getParameter(DISPLAY_TYPE.name)->setInfo(
                                defComment.str());

            defComment.str("");
            defComment  << "{" << getPixelFormatString(PXL_NONE)
                << ", " << getPixelFormatString(BAYER_RGGB_8)
                << ", " << getPixelFormatString(BAYER_RGGB_10)
                << ", " << getPixelFormatString(BAYER_RGGB_12)
                << "}";
            parameters.addParameterDef(DATAEXTRA_TYPE);
            parameters.getParameter(DATAEXTRA_TYPE.name)->setInfo(
                                defComment.str());

            parameters.addParameterDef(DATAEXTRA_POINT);

            defComment.str("");
            defComment  << "{" << getPixelFormatString(PXL_NONE)
                << ", " << getPixelFormatString(BGR_101010_32)
                << "}";
            parameters.addParameterDef(HDREXTRA_TYPE);
            parameters.getParameter(HDREXTRA_TYPE.name)->setInfo(
                                defComment.str());

            defComment.str("");
            defComment << "{" << getPixelFormatString(PXL_NONE)
                << ", " << getPixelFormatString(BGR_161616_64)
                << "}";
            parameters.addParameterDef(HDRINS_TYPE);
            parameters.getParameter(HDRINS_TYPE.name)->setInfo(
                                defComment.str());

            defComment.str("");
            defComment  << "{" << getPixelFormatString(PXL_NONE)
                << ", " << getPixelFormatString(BAYER_TIFF_10)
                << ", " << getPixelFormatString(BAYER_TIFF_12)
                << "}";
            parameters.addParameterDef(RAW2DEXTRA_TYPE);
            parameters.getParameter(RAW2DEXTRA_TYPE.name)->setInfo(
                                defComment.str());
        }
        break;
    }

    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleOUT::setup()
{
    LOG_PERF_IN();
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_RESULT ret;
    if (!pipeline)
    {
        MOD_LOG_ERROR("pipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    pMCPipeline = pipeline->getMCPipeline();
    if (!pMCPipeline)
    {
        MOD_LOG_ERROR("pMCPipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    const CI_HWINFO &sHWInfo = pipeline->getConnection()->sHWInfo;
    PIXELTYPE sType;

    pMCPipeline->eEncOutput = encoderType;

    // If RGB is asked it has priority over Bayer extraction
    pMCPipeline->eDispOutput = PXL_NONE;
    pMCPipeline->eDEPoint = CI_INOUT_NONE;
    ret = PixelTransformDisplay(&sType, this->displayType);
    if (IMG_SUCCESS == ret)
    {
        pMCPipeline->eDispOutput = this->displayType;
    }
    else if (CI_INOUT_NONE != this->dataExtractionPoint
        && PXL_NONE != this->dataExtractionType)
    {
        pMCPipeline->eDEPoint = this->dataExtractionPoint;
        pMCPipeline->eDispOutput = this->dataExtractionType;
    }

    if (sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT)
    {
        pMCPipeline->eHDRExtOutput = this->hdrExtractionType;
    }
    else if (PXL_NONE != hdrExtractionType)
    {
        MOD_LOG_WARNING("HDR Extraction format ignored because HW %d.%d "\
            "does not support it\n",
            sHWInfo.rev_ui8Major, sHWInfo.rev_ui8Minor);
        pMCPipeline->eHDRExtOutput = PXL_NONE;
    }

    if (sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_INS)
    {
        pMCPipeline->eHDRInsertion = this->hdrInsertionType;
    }
    else if (PXL_NONE != hdrInsertionType)
    {
        MOD_LOG_WARNING("HDR Insertion format ignored because HW %d.%d "\
            "does not support it\n",
            sHWInfo.rev_ui8Major, sHWInfo.rev_ui8Minor);
        pMCPipeline->eHDRInsertion = PXL_NONE;
    }

    if (sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT)
    {
        pMCPipeline->eRaw2DExtOutput = this->raw2DExtractionType;
    }
    else if (PXL_NONE != this->raw2DExtractionType)
    {
        MOD_LOG_WARNING("RAW2D Extraction format ignored because HW %d.%d "\
            "does not support it\n",
            sHWInfo.rev_ui8Major, sHWInfo.rev_ui8Minor);
        pMCPipeline->eRaw2DExtOutput = PXL_NONE;
    }

    /* BRN58117: if DE point 2 is the only one active and memory is
     * stalled HW can lock up. Work arround is to enable statistics
     * output later on. */
    if (CI_INOUT_FILTER_LINESTORE == this->dataExtractionPoint
        && PXL_NONE != this->dataExtractionType
        && PXL_NONE == this->encoderType
        && PXL_NONE == this->hdrExtractionType
        && PXL_NONE == this->raw2DExtractionType)
    {
        // we need to ensure that at least 1 stat is enabled
        ModuleFOS *pFOS = pipeline->getModule<ModuleFOS>();
        ModuleHIS *pHIS = pipeline->getModule<ModuleHIS>();
        ModuleWBS *pWBS = pipeline->getModule<ModuleWBS>();
        ModuleFLD *pFLD = pipeline->getModule<ModuleFLD>();
        ModuleDPF *pDPF = pipeline->getModule<ModuleDPF>();

        bool stats_on = false;

        if (pFOS)
        {
            stats_on |= pFOS->bEnableGrid | pFOS->bEnableROI;
        }
        if (pHIS)
        {
            stats_on |= pHIS->bEnableGlobal | pHIS->bEnableROI;
        }
        if (pWBS)
        {
            stats_on |= pWBS->ui32NROIEnabled > 0;
        }
        if (pFLD)
        {
            stats_on |= pFLD->bEnable;
        }
        if (pDPF)
        {
            stats_on |= pDPF->bWrite;
        }

        if (!stats_on)
        {
            MOD_LOG_WARNING("BRN58117: enabling FOS because only DE "\
                "point 2 enabled.\n");
            pFOS->bEnableROI = true;
            pFOS->aRoiStartCoord[0] = 0;
            pFOS->aRoiStartCoord[1] = 0;
            // could also use Global_Setup but we have MCPipeline already
            pFOS->aRoiEndCoord[0] =
                pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH;
            pFOS->aRoiEndCoord[1] =
                pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT;

            /* OUT is a global module - it is updated before any other
             * module therefore FOS should be updated */
            pFOS->requestUpdate();
        }
    }

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
