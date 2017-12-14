/**
*******************************************************************************
 @file ModuleR2Y.cpp

 @brief Implementation of ISPC::ModuleR2Y

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
#include "ispc/ModuleR2Y.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_R2Y"

#include <cmath>
#include <string>
#include <vector>
#include <ispc/Camera.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "ispc/ModuleOUT.h"
#include "ispc/Pipeline.h"
/** Matrices defined in YCbCr order */
static const double R2Y_BT709[3][3] = {
    { 0.2126,  0.7152,  0.0722},
    {-0.1146, -0.3854,  0.5000},
    { 0.5000, -0.4542, -0.0458}};

static const double R2Y_BT601[3][3] = {
    { 0.2990,  0.5870,  0.1140},
    {-0.1687, -0.3313,  0.5000},
    { 0.5000, -0.4187, -0.0813}};

#ifdef INFOTM_SUPPORT_JFIF
static const double R2Y_JFIF[3][3] = {
    { 0.2990*1.035,  0.5870*1.035,  0.1140*1.035},
    {-0.1687, -0.3313,  0.5000},
    { 0.5000*0.97, -0.4187, -0.0813},
};

#endif
#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
static const double R2Y_FLAT[3][3] = {
    { 1.0000,  1.0000,  1.0000},
    { 0.0000,  0.0000,  0.0000},
    { 0.0000,  0.0000,  0.0000}};
#endif

static const double Y2R_BT709[3][3] = {
    { 1.0000,   0.0000,      1.0000},     // R
    { 1.0000,  -0.1009508,  -0.2972595},  // G
    { 1.0000,   1.0000,      0.0000}};    // B
// Y,Cb,Cr
static const double Y2R_BT709_RANGEMULT[3] = { 1.0, 1.8556, 1.5748 };

static const double Y2R_BT601[3][3] = {
    { 1.0000,   0.0000,      1.0000},     // R
    { 1.0000,  -0.1942078,  -0.5093697},  // G
    { 1.0000,   1.0000,      0.0000}};    // B
// Y, Cb, Cr
static const double Y2R_BT601_RANGEMULT[3] = {1.0, 1.772, 1.402};

#define BT601_STR "BT601"
#define BT709_STR "BT709"
#define JFIF_STR "JFIF"

const ISPC::ParamDefSingle<std::string>
    ISPC::ModuleR2Y::R2Y_MATRIX_STD("R2Y_MATRIX", BT709_STR);
const ISPC::ParamDef<double>
    ISPC::ModuleR2Y::R2Y_BRIGHTNESS("R2Y_BRIGHTNESS", R2Y_BRIGHTNESS_MIN,
    R2Y_BRIGHTNESS_MAX, R2Y_BRIGHTNESS_DEF);
const ISPC::ParamDef<double>
    ISPC::ModuleR2Y::R2Y_CONTRAST("R2Y_CONTRAST", R2Y_CONTRAST_MIN,
    R2Y_CONTRAST_MAX, R2Y_CONTRAST_DEF);
const ISPC::ParamDef<double>
    ISPC::ModuleR2Y::R2Y_SATURATION("R2Y_SATURATION", R2Y_SATURATION_MIN,
    R2Y_SATURATION_MAX, R2Y_SATURATION_DEF);
const ISPC::ParamDef<double>
    ISPC::ModuleR2Y::R2Y_HUE("R2Y_HUE", -30.0, 30.0, 0.0);
const ISPC::ParamDef<double>
    ISPC::ModuleR2Y::R2Y_OFFSETU("R2Y_OFFSETU", R2Y_OFFSETUV_MIN,
    R2Y_OFFSETUV_MAX, R2Y_OFFSETUV_DEF);
const ISPC::ParamDef<double>
    ISPC::ModuleR2Y::R2Y_OFFSETV("R2Y_OFFSETV", R2Y_OFFSETUV_MIN,
    R2Y_OFFSETUV_MAX, R2Y_OFFSETUV_DEF);

const double R2Y_RANGEMULT_DEF[3] = {1.0f, 1.0f, 1.0f};
const ISPC::ParamDefArray<double>
    ISPC::ModuleR2Y::R2Y_RANGEMULT("R2Y_RANGE_MUL", 0.0, 2.0,
    R2Y_RANGEMULT_DEF, 3);

#ifdef INFOTM_ISP
#define R2Y_MATRIX_MIN -3.0
#define R2Y_MATRIX_MAX 3.0

static const double CUSTOM_BT709_DEF[9] =
{
	R2Y_BT709[0][0], R2Y_BT709[0][1], R2Y_BT709[0][2],
	R2Y_BT709[1][0], R2Y_BT709[1][1], R2Y_BT709[1][2],
	R2Y_BT709[2][0], R2Y_BT709[2][1], R2Y_BT709[2][2],
};

static const double CUSTOM_BT601_DEF[9] =
{
	R2Y_BT601[0][0], R2Y_BT601[0][1], R2Y_BT601[0][2],
	R2Y_BT601[1][0], R2Y_BT601[1][1], R2Y_BT601[1][2],
	R2Y_BT601[2][0], R2Y_BT601[2][1], R2Y_BT601[2][2],
};

#ifdef INFOTM_SUPPORT_JFIF
static const double CUSTOM_JFIF_DEF[9] =
{
	R2Y_JFIF[0][0], R2Y_JFIF[0][1], R2Y_JFIF[0][2],
	R2Y_JFIF[1][0], R2Y_JFIF[1][1], R2Y_JFIF[1][2],
	R2Y_JFIF[2][0], R2Y_JFIF[2][1], R2Y_JFIF[2][2],
};
#endif

const ISPC::ParamDefSingle<bool> ISPC::ModuleR2Y::USE_CUSTOM_R2Y_MATRIX("USE_CUSTOM_R2Y_MATRIX", false);
const ISPC::ParamDefArray<double> ISPC::ModuleR2Y::CUSTOM_BT709_MATRIX("CUSTOM_BT709_MATRIX", R2Y_MATRIX_MIN, R2Y_MATRIX_MAX, CUSTOM_BT709_DEF, 9);
const ISPC::ParamDefArray<double> ISPC::ModuleR2Y::CUSTOM_BT601_MATRIX("CUSTOM_BT601_MATRIX", R2Y_MATRIX_MIN, R2Y_MATRIX_MAX, CUSTOM_BT601_DEF, 9);
#ifdef INFOTM_SUPPORT_JFIF
const ISPC::ParamDefArray<double> ISPC::ModuleR2Y::CUSTOM_JFIF_MATRIX("CUSTOM_JFIF_MATRIX", R2Y_MATRIX_MIN, R2Y_MATRIX_MAX, CUSTOM_JFIF_DEF, 9);
#endif

#endif //INFOTM_ISP
ISPC::ParameterGroup ISPC::ModuleR2Y::getGroup()
{
    ParameterGroup group;

    group.header = "// RGB to YUV parameters";

    group.parameters.insert(R2Y_MATRIX_STD.name);
    group.parameters.insert(R2Y_BRIGHTNESS.name);
    group.parameters.insert(R2Y_CONTRAST.name);
    group.parameters.insert(R2Y_SATURATION.name);
    group.parameters.insert(R2Y_HUE.name);
    group.parameters.insert(R2Y_RANGEMULT.name);
    group.parameters.insert(R2Y_OFFSETU.name);
    group.parameters.insert(R2Y_OFFSETV.name);

    return group;
}

ISPC::ModuleR2Y::ModuleR2Y(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleR2Y::load(const ParameterList &parameters)
{
    int i;
#ifdef INFOTM_ISP
    int j, k;
#endif //INFOTM_ISP
    std::string conversionMatrix = parameters.getParameterString(
        R2Y_MATRIX_STD.name, 0, R2Y_MATRIX_STD.def);

    if (0 == conversionMatrix.compare(BT601_STR))
    {
        eMatrix = BT601;
    }
    else if (0 == conversionMatrix.compare(BT709_STR))
    {
        eMatrix = BT709;
    }
    // JFIF not supported
#ifdef INFOTM_SUPPORT_JFIF // ??? for test
    else if (conversionMatrix.compare(JFIF_STR)==0)
    {
        eMatrix = JFIF;
    }
#else
    /*else if (conversionMatrix.compare(JFIF_STR)==0) 
    {
        MATRIX = JFIF;
    } */
#endif
    else
    {
        MOD_LOG_ERROR("Invalid matrix conversion format: '%s'\n",
            conversionMatrix.c_str());
        return IMG_ERROR_FATAL;
    }

    fBrightness = parameters.getParameter(R2Y_BRIGHTNESS);
    fContrast = parameters.getParameter(R2Y_CONTRAST);
    fSaturation = parameters.getParameter(R2Y_SATURATION);
    fHue = parameters.getParameter(R2Y_HUE);
    fOffsetU = parameters.getParameter(R2Y_OFFSETU);
    fOffsetV = parameters.getParameter(R2Y_OFFSETV);

    for (i = 0 ; i < 3 ; i++)
    {
        aRangeMult[i] = parameters.getParameter(R2Y_RANGEMULT, i);
    }

#ifdef INFOTM_ISP
    bUseCustomR2YMatrix = parameters.getParameter(USE_CUSTOM_R2Y_MATRIX);

    for (i = 0; i < 9; i++)
    {
        j = i / 3;
        k = i % 3;
        fCustomBT709Matrix[j][k] = parameters.getParameter(CUSTOM_BT709_MATRIX, i);
        fCustomBT601Matrix[j][k] = parameters.getParameter(CUSTOM_BT601_MATRIX, i);
        fCustomJFIFMatrix[j][k] = parameters.getParameter(CUSTOM_JFIF_MATRIX, i);
    }
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleR2Y::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (group.parameters.size() == 0)
    {
        group = ModuleR2Y::getGroup();
    }

    parameters.addGroup("ModuleR2Y", group);

    switch (t)
    {
    case SAVE_VAL:
        if (BT601 == eMatrix)
        {
            parameters.addParameter(Parameter(R2Y_MATRIX_STD.name,
                BT601_STR));
        }
        else if (BT709 == eMatrix)
        {
            parameters.addParameter(Parameter(R2Y_MATRIX_STD.name,
                BT709_STR));
        }
        else if (JFIF == eMatrix)
        {
            parameters.addParameter(Parameter(R2Y_MATRIX_STD.name,
                JFIF_STR));
        }

        parameters.addParameter(
            Parameter(R2Y_BRIGHTNESS.name, toString(this->fBrightness)));
        parameters.addParameter(
            Parameter(R2Y_CONTRAST.name, toString(this->fContrast)));
        parameters.addParameter(
            Parameter(R2Y_SATURATION.name, toString(this->fSaturation)));
        parameters.addParameter(
            Parameter(R2Y_HUE.name, toString(this->fHue)));
        parameters.addParameter(
            Parameter(R2Y_OFFSETU.name, toString(this->fOffsetU)));
        parameters.addParameter(
            Parameter(R2Y_OFFSETV.name, toString(this->fOffsetV)));

        values.clear();
        for (i = 0 ; i < 3 ; i++)
        {
            values.push_back(toString(this->aRangeMult[i]));
        }
        parameters.addParameter(Parameter(R2Y_RANGEMULT.name, values));
        break;

    case SAVE_MIN:
        // matrix std does not have a min
        parameters.addParameter(
            Parameter(R2Y_MATRIX_STD.name, R2Y_MATRIX_STD.def));
        parameters.addParameter(
            Parameter(R2Y_BRIGHTNESS.name, toString(R2Y_BRIGHTNESS.min)));
        parameters.addParameter(
            Parameter(R2Y_CONTRAST.name, toString(R2Y_CONTRAST.min)));
        parameters.addParameter(
            Parameter(R2Y_SATURATION.name, toString(R2Y_SATURATION.min)));
        parameters.addParameter(
            Parameter(R2Y_HUE.name, toString(R2Y_HUE.min)));
        parameters.addParameter(
            Parameter(R2Y_OFFSETU.name, toString(R2Y_OFFSETU.min)));
        parameters.addParameter(
            Parameter(R2Y_OFFSETV.name, toString(R2Y_OFFSETV.min)));

        values.clear();
        for (i = 0 ; i < 3 ; i++)
        {
            values.push_back(toString(R2Y_RANGEMULT.min));
        }
        parameters.addParameter(Parameter(R2Y_RANGEMULT.name, values));
        break;

    case SAVE_MAX:
        // matrix std does not have a max
        parameters.addParameter(
            Parameter(R2Y_MATRIX_STD.name, R2Y_MATRIX_STD.def));
        parameters.addParameter(
            Parameter(R2Y_BRIGHTNESS.name, toString(R2Y_BRIGHTNESS.max)));
        parameters.addParameter(
            Parameter(R2Y_CONTRAST.name, toString(R2Y_CONTRAST.max)));
        parameters.addParameter(
            Parameter(R2Y_SATURATION.name, toString(R2Y_SATURATION.max)));
        parameters.addParameter(
            Parameter(R2Y_HUE.name, toString(R2Y_HUE.max)));
        parameters.addParameter(
            Parameter(R2Y_OFFSETU.name, toString(R2Y_OFFSETU.max)));
        parameters.addParameter(
            Parameter(R2Y_OFFSETV.name, toString(R2Y_OFFSETV.max)));

        values.clear();
        for (i = 0 ; i < 3 ; i++)
        {
            values.push_back(toString(R2Y_RANGEMULT.max));
        }
        parameters.addParameter(Parameter(R2Y_RANGEMULT.name, values));
        break;

    case SAVE_DEF:
        {
            std::ostringstream defComment;

            defComment.str("");
            defComment << "// {"
                << BT601_STR
                << ", " << BT709_STR
                << ", " << JFIF_STR
                << "}";

            values.clear();
            values.push_back(R2Y_MATRIX_STD.def);
            values.push_back(defComment.str());

            parameters.addParameter(Parameter(R2Y_MATRIX_STD.name, values));
        }
        parameters.addParameter(
            Parameter(R2Y_BRIGHTNESS.name, toString(R2Y_BRIGHTNESS.def)
            + "// " + getParameterInfo(R2Y_BRIGHTNESS)));
        parameters.addParameter(
            Parameter(R2Y_CONTRAST.name, toString(R2Y_CONTRAST.def)
            + "// " + getParameterInfo(R2Y_CONTRAST)));
        parameters.addParameter(
            Parameter(R2Y_SATURATION.name, toString(R2Y_SATURATION.def)
            + "// " + getParameterInfo(R2Y_SATURATION)));
        parameters.addParameter(
            Parameter(R2Y_HUE.name, toString(R2Y_HUE.def)
            + "// " + getParameterInfo(R2Y_HUE)));
        parameters.addParameter(
            Parameter(R2Y_OFFSETU.name, toString(R2Y_OFFSETU.def)
            + "// " + getParameterInfo(R2Y_OFFSETU)));
        parameters.addParameter(
            Parameter(R2Y_OFFSETV.name, toString(R2Y_OFFSETV.def)
            + "// " + getParameterInfo(R2Y_OFFSETV)));

        values.clear();
        for (i = 0 ; i < 3 ; i++)
        {
            values.push_back(toString(R2Y_RANGEMULT.def[i]));
        }
        values.push_back("// " + getParameterInfo(R2Y_RANGEMULT));
        parameters.addParameter(Parameter(R2Y_RANGEMULT.name, values));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleR2Y::setup()
{
    IMG_RESULT ret;
    MC_PIPELINE *pMCPipeline = NULL;
    const ModuleOUT *pOut = NULL;
    bool bSwap = false;

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

    pOut = pipeline->getModule<const ModuleOUT>();
    bSwap = (YUV_420_PL12_8 == pOut->encoderType
       || YUV_420_PL12_10 == pOut->encoderType
       || YUV_422_PL12_8 == pOut->encoderType
       || YUV_420_PL12_10 == pOut->encoderType);

    pMCPipeline->sR2Y.eType = RGB_TO_YCC;
    ret = configure(pMCPipeline->sR2Y, false, bSwap);

    if (IMG_SUCCESS == ret)
    {
        this->setupFlag = true;
    }
    return ret;
}

//
// ModuleR2YBase
//

IMG_RESULT ISPC::ModuleR2YBase::configure(MC_PLANECONV &plane,
    bool bSwapInput, bool bSwapOutput) const
{
    //  use M_PI
    static const double dPI = 3.14159265358;
#ifdef USE_MATH_NEON
    double dHueSin = (double)sinf_neon((float)this->fHue * 2.0f * (float)dPI / 360.0f);
    double dHueCos = (double)cosf_neon((float)this->fHue * 2.0f * (float)dPI / 360.0f);
#else
    double dHueSin = sin(this->fHue * 2 * dPI / 360.0);
    double dHueCos = cos(this->fHue * 2 * dPI / 360.0);
#endif
    double contrast = this->fContrast;
    double dMulY = 1.0, dMulCb = 1.0, dMulCr = 1.0;
    double pM[3][3];
    unsigned int i, j;

    if (YCC_TO_RGB == plane.eType)
    {
        if (BT709 == eMatrix)
        {
            dMulY = aRangeMult[0] * Y2R_BT709_RANGEMULT[0];
            dMulCb = aRangeMult[1] * Y2R_BT709_RANGEMULT[1];
            dMulCr = aRangeMult[2] * Y2R_BT709_RANGEMULT[2];
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
                    pM[i][j] = Y2R_BT709[i][j];
                }
            }
        }
        else if (BT601 == eMatrix)
        {
            dMulY = aRangeMult[0] * Y2R_BT601_RANGEMULT[0];
            dMulCb = aRangeMult[1] * Y2R_BT601_RANGEMULT[1];
            dMulCr = aRangeMult[2] * Y2R_BT601_RANGEMULT[2];
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
                    pM[i][j] = Y2R_BT601[i][j];
                }
            }
        }
        else
        {
            LOG_ERROR("Invalid matrix conversion format: %d\n", (int)eMatrix);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        for (i = 0; i < 3; i++)
        {
            plane.aCoeff[i][1] = this->fContrast*pM[i][0] * dMulY;  // Y
            // Cb
            plane.aCoeff[i][2] = this->fContrast * this->fSaturation
                * (pM[i][1] * dHueCos * dMulCb - pM[i][2] * dHueSin * dMulCr);
            // Cr
            plane.aCoeff[i][0] = this->fContrast * this->fSaturation
                * (pM[i][1] * dHueSin * dMulCb + pM[i][2] * dHueCos * dMulCr);
        }
        if (bSwapInput)
        {
            MC_FLOAT tmp;
            for (i = 0; i < 3; i++)
            {
                tmp = plane.aCoeff[i][2];
                plane.aCoeff[i][2] = plane.aCoeff[i][0];
                plane.aCoeff[i][0] = tmp;
            }
        }

        if (0.0 == contrast)
        {
            contrast = 1.0;  // Avoid division by 0
        }

        /* : this is typically 16 but must match the output black
         * level of the TNM */
        double dOffset = -16.0;
        plane.aOffset[1] = dOffset + (this->fBrightness / contrast) * 256;

        // 16 for default unsigned, biased YCC; 0 for signed unbiased YCC
        // if swap and different values make sure they are swapped here too
        plane.aOffset[0] = this->fOffsetU * 256;
        plane.aOffset[2] = this->fOffsetV * 256;

        if (bSwapOutput)
        {
            MC_FLOAT tmp = 0.0;
            LOG_DEBUG("swap output of Y2R\n");
            for (i = 0 ; i < 3 ; i++)
            {
                tmp = plane.aCoeff[0][i];
                plane.aCoeff[0][i] = plane.aCoeff[2][i];
                plane.aCoeff[2][i] = tmp;
            }

            tmp = plane.aOffset[0];
            plane.aOffset[0] = plane.aOffset[2];
            plane.aOffset[2] = tmp;
        }

        return IMG_SUCCESS;
    }
    else if (RGB_TO_YCC == plane.eType)
    {
        dMulY = aRangeMult[0];
        dMulCb = aRangeMult[1];
        dMulCr = aRangeMult[2];

#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
        if (FlatModeStatusGet())
        {
  #ifdef INFOTM_ISP
            if (bUseCustomR2YMatrix)
            {
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
                        pM[i][j] = fCustomBT709Matrix[i][j]; // ????
                    }
                }
            }
            else
            {
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
    #if 0
                        pM[i][j] = R2Y_FLAT[i][j];
    #else
                        pM[i][j] = R2Y_BT709[i][j];
    #endif
                    }
                }
            }
  #else
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
    #if 0
                    pM[i][j] = R2Y_FLAT[i][j];
    #else
                    pM[i][j] = R2Y_BT709[i][j];
    #endif
                }
            }
  #endif //INFOTM_ISP
        }
        else if (eMatrix == BT709)
#else
        if (eMatrix == BT709)
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE
        {
  #ifdef INFOTM_ISP
            if (bUseCustomR2YMatrix)
            {
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
                        pM[i][j] = fCustomBT709Matrix[i][j];
                    }
                }
            }
            else
            {
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
                        pM[i][j] = R2Y_BT709[i][j];
                    }
                }
            }
  #else
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
                    pM[i][j] = R2Y_BT709[i][j];
                }
            }
  #endif //INFOTM_ISP
        }
        else if (eMatrix == BT601)
        {
  #ifdef INFOTM_ISP
            if (bUseCustomR2YMatrix)
            {
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
                        pM[i][j] = fCustomBT601Matrix[i][j];
                    }
                }
            }
            else
            {
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
                        pM[i][j] = R2Y_BT601[i][j];
                    }
                }
            }
  #else
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
                    pM[i][j] = R2Y_BT601[i][j];
                }
            }
  #endif //INFOTM_ISP
        }
#ifdef INFOTM_SUPPORT_JFIF
        else if (eMatrix == JFIF)
        {
            if (bUseCustomR2YMatrix)
            {
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
                        pM[i][j] = fCustomJFIFMatrix[i][j];
                    }
                }
            }
            else
            {
                for (i = 0; i < 3; i++)
                {
                    for (j = 0; j < 3; j++)
                    {
                        pM[i][j] = R2Y_JFIF[i][j];
                    }
                }
            }
        }
#endif
        else
        {
            LOG_ERROR("Invalid matrix conversion format: %d\n", (int)eMatrix);
            return IMG_ERROR_UNEXPECTED_STATE;
        }

        if (bSwapInput)
        {
            LOG_WARNING("Swaping input to R2Y has no effects!\n");
        }

        for (i = 0 ; i < 3 ; i++)
        {
            plane.aCoeff[1][i] = pM[0][i] * this->fContrast*dMulY;  // Y
            plane.aCoeff[2][i] = this->fContrast * this->fSaturation * dMulCb
                * (pM[1][i] * dHueCos + pM[2][i] * dHueSin);  // Cb
            plane.aCoeff[0][i] = this->fContrast * this->fSaturation * dMulCr
                * (pM[2][i] * dHueCos - pM[1][i] * dHueSin);  // Cr
        }

        plane.aOffset[0] = this->fOffsetU * 256;     // Cr
        plane.aOffset[1] = this->fBrightness * 256;  // Y
        plane.aOffset[2] = this->fOffsetV * 256;     // Cb

        if (bSwapOutput)
        {
            MC_FLOAT tmp = 0.0;
            LOG_DEBUG("swap output of R2Y\n");
            for (i = 0 ; i < 3 ; i++)
            {
                tmp = plane.aCoeff[0][i];
                plane.aCoeff[0][i] = plane.aCoeff[2][i];
                plane.aCoeff[2][i] = tmp;
            }

            tmp = plane.aOffset[0];
            plane.aOffset[0] = plane.aOffset[2];
            plane.aOffset[2] = tmp;
        }

        return IMG_SUCCESS;
#ifdef INFORM_ISP
    } else if(YCC_TO_YCC == plane.eType)
    {
        dMulY = aRangeMult[0];
        dMulCb = aRangeMult[1];
        dMulCr = aRangeMult[2];
        // make identity matrix for YUV passtrough
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                plane.aCoeff[i][j] = 0.0;
            }
        }
        {
            // Y
            plane.aCoeff[2][1] = dMulY;
#if(0)
            // HSC controls disabled, use Y2R controls for common regulation
            // Cb,V
            plane.aCoeff[1][0] = dHueSin * dMulCb;
            plane.aCoeff[1][2] = dHueCos * dMulCr;
            // Cr,U
            plane.aCoeff[0][2] = dHueCos * dMulCr;
            plane.aCoeff[0][0] = -dHueSin * dMulCb;
#else
            // Cb,V
            plane.aCoeff[1][0] = dMulCb;
            // Cr,U
            plane.aCoeff[0][2] = dMulCr;
#endif
        }
        if (0.0 == contrast)
        {
            contrast = 1.0;  // Avoid division by 0
        }

        plane.aOffset[1] = (this->fBrightness / contrast) * 256;
        plane.aOffset[0] = 0.0;
        plane.aOffset[2] = 0.0;

        // swap if UV order different on input than on output
        if (bSwapOutput)
        {
            MC_FLOAT tmp = 0.0;
            LOG_DEBUG("swap output of Y2R\n");
            // swap columns
            for (i = 0 ; i < 3 ; i++)
            {
                tmp = plane.aCoeff[i][0];
                plane.aCoeff[i][0] = plane.aCoeff[i][2];
                plane.aCoeff[i][2] = tmp;
            }
            tmp = plane.aOffset[0];
            plane.aOffset[0] = plane.aOffset[2];
            plane.aOffset[0] = tmp;
        }

        return IMG_SUCCESS;
#endif //INFOTM_ISP
    }

    LOG_ERROR("Given plane is neiher YUV to RGB nor RGB to YUV\n");
    return IMG_ERROR_INVALID_PARAMETERS;;
}
