/**
*******************************************************************************
 @file ModuleTNM.cpp

 @brief Implementation of ISPC::ModuleTNM

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
#include "ispc/ModuleTNM.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_TNM"

#include <cmath>
#include <string>
#include <vector>

#include "ispc/Pipeline.h"

static const double TNM_IN_Y_DEF[2] = {-64.0, 64.0};
const ISPC::ParamDefArray<double> ISPC::ModuleTNM::TNM_IN_Y(
    "TNM_IN_Y", -127.0, 127.0, TNM_IN_Y_DEF, 2);

#ifdef INFOTM_ISP
static const double TNM_OUT_Y_DEF[2] = {16, 235};
const ISPC::ParamDefArray<double> ISPC::ModuleTNM::TNM_OUT_Y(
    "TNM_OUT_Y", 0, 255.0, TNM_OUT_Y_DEF, 2);

static const double TNM_OUT_C_DEF[2] = {10, 250};
const ISPC::ParamDefArray<double> ISPC::ModuleTNM::TNM_OUT_C(
    "TNM_OUT_C", 0, 255.0, TNM_OUT_C_DEF, 2);

#endif //INFOTM_ISP
const ISPC::ParamDef<double> ISPC::ModuleTNM::TNM_WEIGHT_LOCAL(
    "TNM_WEIGHT_LOCAL", 0.0, 1.0, 0.0);
const ISPC::ParamDef<double> ISPC::ModuleTNM::TNM_WEIGHT_LINE(
    "TNM_WEIGHT_LINE", 0.0, 1.0, 0.0);
const ISPC::ParamDef<double> ISPC::ModuleTNM::TNM_FLAT_FACTOR(
    "TNM_FLAT_FACTOR", 0.0, 1.0, 0.0);
const ISPC::ParamDef<double> ISPC::ModuleTNM::TNM_FLAT_MIN(
    "TNM_FLAT_MIN", 0.0, 1.0, 0.0);
const ISPC::ParamDef<double> ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE(
    "TNM_COLOUR_CONFIDENCE", 0.0, 64.0, 1.0);
const ISPC::ParamDef<double> ISPC::ModuleTNM::TNM_COLOUR_SATURATION(
    "TNM_COLOUR_SATURATION", 0.0, 16.0, 1.0);

// not the actual default
static const double TNM_CURVE_DEF[TNM_CURVE_NPOINTS] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0};
const ISPC::ParamDefArray<double> ISPC::ModuleTNM::TNM_CURVE(
    "TNM_CURVE", 0.0, 1.0, TNM_CURVE_DEF, TNM_CURVE_NPOINTS);

#ifdef INFOTM_ISP
const ISPC::ParamDefSingle<bool> ISPC::ModuleTNM::TNM_STATIC_CURVE(
    "TNM_STATIC_CURVE", false);
#endif //INFOTM_ISP

// default is compued for TNM_CURVE_NPOINTS+2 (first = 0.0f, last = 1.0f)
static const double TNM_CURVE_FIRST = 0.0;  // used for computation of default
static const double TNM_CURVE_LAST = 1.0;

const ISPC::ParamDefSingle<bool> ISPC::ModuleTNM::TNM_BYPASS(
    "TNM_BYPASS", false);

ISPC::ParameterGroup ISPC::ModuleTNM::getGroup()
{
    ParameterGroup group;

    group.header = "// Tone Mapper parameters";

    group.parameters.insert(TNM_BYPASS.name);
    group.parameters.insert(TNM_IN_Y.name);
#ifdef INFOTM_ISP
    group.parameters.insert(TNM_OUT_Y.name);
    group.parameters.insert(TNM_OUT_C.name);
#endif //INFOTM_ISP
    group.parameters.insert(TNM_WEIGHT_LOCAL.name);
    group.parameters.insert(TNM_WEIGHT_LINE.name);
    group.parameters.insert(TNM_FLAT_FACTOR.name);
    group.parameters.insert(TNM_FLAT_MIN.name);
    group.parameters.insert(TNM_COLOUR_CONFIDENCE.name);
    group.parameters.insert(TNM_COLOUR_SATURATION.name);
    group.parameters.insert(TNM_CURVE.name);
#ifdef INFOTM_ISP
    group.parameters.insert(TNM_STATIC_CURVE.name);
#endif //INFOTM_ISP

    return group;
}

std::vector<double> ISPC::ModuleTNM::computeDefaultCurve()
{
    std::vector<double> curve;
    int i;
    const double slope = (TNM_CURVE_LAST - TNM_CURVE_FIRST)
        / (TNM_CURVE_NPOINTS + 2);
    // first point and last point are known: 0.0 and 1.0
    // all others are interpolated to have an identity curve

    for (i = 0; i < TNM_CURVE_NPOINTS; i++)
    {
        curve.push_back((i + 1)*slope);
    }

    return curve;
}

ISPC::ModuleTNM::ModuleTNM(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleTNM::load(const ParameterList &parameters)
{
    int i;
    for (i = 0 ; i < 2 ; i++)
    {
        aInY[i] = parameters.getParameter(TNM_IN_Y, i);
#ifdef INFOTM_ISP
        aOutY[i] = parameters.getParameter(TNM_OUT_Y, i);
        aOutC[i] = parameters.getParameter(TNM_OUT_C, i);
#endif //INFOTM_ISP
    }

    fWeightLocal = parameters.getParameter(TNM_WEIGHT_LOCAL);
    fWeightLine = parameters.getParameter(TNM_WEIGHT_LINE);
    fFlatFactor = parameters.getParameter(TNM_FLAT_FACTOR);
    fFlatMin = parameters.getParameter(TNM_FLAT_MIN);
    fColourConfidence = parameters.getParameter(TNM_COLOUR_CONFIDENCE);
    fColourSaturation = parameters.getParameter(TNM_COLOUR_SATURATION);

    std::vector<double> defaultCurve = computeDefaultCurve();

    for (int i = 0; i < TNM_CURVE_NPOINTS; i++)
    {
        aCurve[i] = parameters.getParameter<double>(TNM_CURVE.name, i,
            TNM_CURVE.min, TNM_CURVE.max, defaultCurve[i]);
    }

#ifdef INFOTM_ISP
    bStaticCurve = parameters.getParameter(TNM_STATIC_CURVE);
#endif //INFOTM_ISP

    bBypass = parameters.getParameter(TNM_BYPASS);
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleTNM::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleTNM::getGroup();
    }

    parameters.addGroup("ModuleTNM", group);

    switch (t)
    {
    case SAVE_VAL:
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aInY[i]));
        }
        parameters.addParameter(Parameter(TNM_IN_Y.name, values));

        parameters.addParameter(Parameter(TNM_WEIGHT_LOCAL.name,
            toString(this->fWeightLocal)));
        parameters.addParameter(Parameter(TNM_WEIGHT_LINE.name,
            toString(this->fWeightLine)));
        parameters.addParameter(Parameter(TNM_FLAT_FACTOR.name,
            toString(this->fFlatFactor)));
        parameters.addParameter(Parameter(TNM_FLAT_MIN.name,
            toString(this->fFlatMin)));
        parameters.addParameter(Parameter(TNM_COLOUR_CONFIDENCE.name,
            toString(this->fColourConfidence)));
        parameters.addParameter(Parameter(TNM_COLOUR_SATURATION.name,
            toString(this->fColourSaturation)));
        parameters.addParameter(Parameter(TNM_BYPASS.name,
            toString(this->bBypass)));

#ifdef INFOTM_ISP
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aOutY[i]));
        }
        parameters.addParameter(Parameter(TNM_OUT_Y.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(this->aOutC[i]));
        }
        parameters.addParameter(Parameter(TNM_OUT_C.name, values));

#endif //INFOTM_ISP
        values.clear();
        for (i = 0 ; i < TNM_CURVE_NPOINTS ; i++)
        {
            values.push_back(toString(this->aCurve[i]));
        }
        parameters.addParameter(Parameter(TNM_CURVE.name, values));
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(TNM_STATIC_CURVE.name, toString(this->bStaticCurve)));
#endif //INFOTM_ISP
        break;

    case SAVE_MIN:
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_IN_Y.min));
        }
        parameters.addParameter(Parameter(TNM_IN_Y.name, values));

#ifdef INFOTM_ISP
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_OUT_Y.min));
        }
        parameters.addParameter(Parameter(TNM_OUT_Y.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_OUT_C.min));
        }
        parameters.addParameter(Parameter(TNM_OUT_C.name, values));

#endif //INFOTM_ISP
        parameters.addParameter(Parameter(TNM_WEIGHT_LOCAL.name,
            toString(TNM_WEIGHT_LOCAL.min)));
        parameters.addParameter(Parameter(TNM_WEIGHT_LINE.name,
            toString(TNM_WEIGHT_LINE.min)));
        parameters.addParameter(Parameter(TNM_FLAT_FACTOR.name,
            toString(TNM_FLAT_FACTOR.min)));
        parameters.addParameter(Parameter(TNM_FLAT_MIN.name,
            toString(TNM_FLAT_MIN.min)));
        parameters.addParameter(Parameter(TNM_COLOUR_CONFIDENCE.name,
            toString(TNM_COLOUR_CONFIDENCE.min)));
        parameters.addParameter(Parameter(TNM_COLOUR_SATURATION.name,
            toString(TNM_COLOUR_SATURATION.min)));
        parameters.addParameter(Parameter(TNM_BYPASS.name,
            toString(TNM_BYPASS.def)));  // bool does not have a min

        values.clear();
        for (i = 0 ; i < TNM_CURVE_NPOINTS ; i++)
        {
            values.push_back(toString(TNM_CURVE.min));
        }
        parameters.addParameter(Parameter(TNM_CURVE.name, values));
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(TNM_STATIC_CURVE.name,
            toString(TNM_STATIC_CURVE.def)));  // bool does not have a min
#endif //INFOTM_ISP
        break;

    case SAVE_MAX:
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_IN_Y.max));
        }
        parameters.addParameter(Parameter(TNM_IN_Y.name, values));

#ifdef INFOTM_ISP
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_OUT_Y.max));
        }
        parameters.addParameter(Parameter(TNM_OUT_Y.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_OUT_C.max));
        }
        parameters.addParameter(Parameter(TNM_OUT_C.name, values));

#endif //INFOTM_ISP
        parameters.addParameter(Parameter(TNM_WEIGHT_LOCAL.name,
            toString(TNM_WEIGHT_LOCAL.max)));
        parameters.addParameter(Parameter(TNM_WEIGHT_LINE.name,
            toString(TNM_WEIGHT_LINE.max)));
        parameters.addParameter(Parameter(TNM_FLAT_FACTOR.name,
            toString(TNM_FLAT_FACTOR.max)));
        parameters.addParameter(Parameter(TNM_FLAT_MIN.name,
            toString(TNM_FLAT_MIN.max)));
        parameters.addParameter(Parameter(TNM_COLOUR_CONFIDENCE.name,
            toString(TNM_COLOUR_CONFIDENCE.max)));
        parameters.addParameter(Parameter(TNM_COLOUR_SATURATION.name,
            toString(TNM_COLOUR_SATURATION.max)));
        parameters.addParameter(Parameter(TNM_BYPASS.name,
            toString(TNM_BYPASS.def)));  // bool does not have a max

        values.clear();
        for (i = 0 ; i < TNM_CURVE_NPOINTS ; i++)
        {
            values.push_back(toString(TNM_CURVE.max));
        }
        parameters.addParameter(Parameter(TNM_CURVE.name, values));
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(TNM_STATIC_CURVE.name,
            toString(TNM_STATIC_CURVE.def)));  // bool does not have a max
#endif //INFOTM_ISP
        break;

    case SAVE_DEF:
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_IN_Y.def[i]));
        }
        values.push_back("// " + getParameterInfo(TNM_IN_Y));
        parameters.addParameter(Parameter(TNM_IN_Y.name, values));

#ifdef INFOTM_ISP
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_OUT_Y.def[i]));
        }
        values.push_back("// " + getParameterInfo(TNM_OUT_Y));
        parameters.addParameter(Parameter(TNM_OUT_Y.name, values));
        values.clear();
        for (i = 0 ; i < 2 ; i++)
        {
            values.push_back(toString(TNM_OUT_C.def[i]));
        }
        values.push_back("// " + getParameterInfo(TNM_OUT_C));
        parameters.addParameter(Parameter(TNM_OUT_C.name, values));

#endif //INFOTM_ISP
        parameters.addParameter(Parameter(TNM_WEIGHT_LOCAL.name,
            toString(TNM_WEIGHT_LOCAL.def)
            + " // " + getParameterInfo(TNM_WEIGHT_LOCAL)));
        parameters.addParameter(Parameter(TNM_WEIGHT_LINE.name,
            toString(TNM_WEIGHT_LINE.def)
            + " // " + getParameterInfo(TNM_WEIGHT_LINE)));
        parameters.addParameter(Parameter(TNM_FLAT_FACTOR.name,
            toString(TNM_FLAT_FACTOR.def)
            + " // " + getParameterInfo(TNM_FLAT_FACTOR)));
        parameters.addParameter(Parameter(TNM_FLAT_MIN.name,
            toString(TNM_FLAT_MIN.def)
            + " // " + getParameterInfo(TNM_FLAT_MIN)));
        parameters.addParameter(Parameter(TNM_COLOUR_CONFIDENCE.name,
            toString(TNM_COLOUR_CONFIDENCE.def)
            + " // " + getParameterInfo(TNM_COLOUR_CONFIDENCE)));
        parameters.addParameter(Parameter(TNM_COLOUR_SATURATION.name,
            toString(TNM_COLOUR_SATURATION.def)
            + " // " + getParameterInfo(TNM_COLOUR_SATURATION)));
        parameters.addParameter(Parameter(TNM_BYPASS.name,
            toString(TNM_BYPASS.def)
            + " // " + getParameterInfo(TNM_BYPASS)));

        std::vector<double> defcurve = computeDefaultCurve();
        values.clear();
        for (i = 0 ; i < TNM_CURVE_NPOINTS ; i++)
        {
            values.push_back(toString(defcurve[i]));
        }
        values.push_back("// " + getParameterInfo(TNM_CURVE));
        parameters.addParameter(Parameter(TNM_CURVE.name, values));
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(TNM_STATIC_CURVE.name,
            toString(TNM_STATIC_CURVE.def)
            + " // " + getParameterInfo(TNM_STATIC_CURVE)));
#endif //INFOTM_ISP
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleTNM::setup()
{
    IMG_INT16 i;
    MC_PIPELINE *pMCPipeline = NULL;
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

    if (0 == pMCPipeline->sIIF.ui16ImagerSize[0] ||
        0 == pMCPipeline->sIIF.ui16ImagerSize[1])
    {
        MOD_LOG_ERROR("IIF should be setup beforehand!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // signal rescaling part (even if bypass HW will do that)
#ifdef INFOTM_ISP
    pMCPipeline->sTNM.chromaIOScale = static_cast<double>(aOutC[1] - aOutC[0]) / (TNM_IN_MAX_C - TNM_IN_MIN_C);
#else
    pMCPipeline->sTNM.chromaIOScale =
        static_cast<double>(TNM_OUT_MAX_C - TNM_OUT_MIN_C)
        / (TNM_IN_MAX_C - TNM_IN_MIN_C);
#endif //INFOTM_ISP

    pMCPipeline->sTNM.inputLumaScale = 256.0 / (aInY[1] - aInY[0] + 1);
    pMCPipeline->sTNM.inputLumaOffset = -(-128.0 - aInY[0]);

#ifdef INFOTM_ISP
    pMCPipeline->sTNM.outputLumaScale = (aOutY[1] - aOutY[0] + 1)
        / 256.0;
    pMCPipeline->sTNM.outputLumaOffset = aOutY[0];
#else
    pMCPipeline->sTNM.outputLumaScale = (TNM_OUT_MAX_Y - TNM_OUT_MIN_Y + 1)
        / 256.0;
    pMCPipeline->sTNM.outputLumaOffset = TNM_OUT_MIN_Y;
#endif //INFOTM_ISP

    // tone mapping part (only if not in bypass)
    pMCPipeline->sTNM.bBypassTNM = bBypass ? IMG_TRUE : IMG_FALSE;

    pMCPipeline->sTNM.updateWeights = fWeightLine;
    pMCPipeline->sTNM.localWeights = fWeightLocal;

    pMCPipeline->sTNM.histFlattenThreshold = fFlatFactor;
    pMCPipeline->sTNM.histFlattenMin = fFlatMin;

    pMCPipeline->sTNM.chromaConfigurationScale = fColourConfidence;
    pMCPipeline->sTNM.chromaSaturationScale = fColourSaturation;

    for (i = 0; i < TNM_CURVE_NPOINTS; i++)
    {
        pMCPipeline->sTNM.aCurve[i] = aCurve[i];
    }

    if (pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH / TNM_NCOLUMNS
        >= TNM_MIN_COL_WIDTH)
    {
        pMCPipeline->sTNM.ui16LocalColumns = TNM_NCOLUMNS;
    }
    else if (!bBypass)
    {
        if (pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH
            < TNM_MIN_COL_WIDTH)
        {
            MOD_LOG_ERROR("The given input size %d for the imager is "\
                "too small for the tone mapper to work (min=%d)! Disable "\
                "TNM or use a bigger image setup\n",
                pMCPipeline->sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH,
                TNM_MIN_COL_WIDTH);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        pMCPipeline->sTNM.ui16LocalColumns = static_cast<IMG_UINT16>(
            floor(pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH
            / static_cast<double>(TNM_MIN_COL_WIDTH)));
        MOD_LOG_WARNING("Invalid number of columns. Number of "\
            "columns set to %d\n", pMCPipeline->sTNM.ui16LocalColumns);
    }
    else  // in bypass we don't care what the column size is
    {
        pMCPipeline->sTNM.ui16LocalColumns = TNM_NCOLUMNS;
    }

    pMCPipeline->sTNM.ui16ColumnsIndex = 0;

    this->setupFlag = true;
    return IMG_SUCCESS;
}
