/**
*******************************************************************************
 @file ModuleVIB.cpp

 @brief Implementation of ISPC::ModuleVIB

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
#include "ispc/ModuleVIB.h"

#include <ctx_reg_precisions.h>
#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_VIB"

#include <cmath>
#include <string>
#include <vector>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

const ISPC::ParamDefSingle<bool> ISPC::ModuleVIB::VIB_ON("MIE_VIB_ON", false);
static const double gains_def[MIE_SATMULT_NUMPOINTS] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0 };  // not the default actually used

const ISPC::ParamDefArray<double> ISPC::ModuleVIB::VIB_SATURATION_CURVE(
    "MIE_VIB_SAT_CURVE", 0.0, 1.0, gains_def, MIE_SATMULT_NUMPOINTS);

ISPC::ParameterGroup ISPC::ModuleVIB::getGroup()
{
    ParameterGroup group;

    group.header = "// Vibrancy parameters";

    group.parameters.insert(VIB_ON.name);
    group.parameters.insert(VIB_SATURATION_CURVE.name);

    return group;
}

ISPC::ModuleVIB::ModuleVIB(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleVIB::load(const ParameterList &parameters)
{
    LOG_PERF_IN();
    // Generic
    // BLACK_LEVEL loaded in MIE module

    // Vibrancy
    bVibrancyOn = parameters.getParameter(VIB_ON);

    for (int i = 0 ; i < MIE_SATMULT_NUMPOINTS ; i++)
    {
        aSaturationCurve[i] = parameters.getParameter(VIB_SATURATION_CURVE, i);
    }

    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleVIB::save(ParameterList &parameters, SaveType t) const
{
    LOG_PERF_IN();
    static ParameterGroup group;
    static double defCurve[MIE_SATMULT_NUMPOINTS];
    int i;
    std::vector<std::string> values;

    if (0 == group.parameters.size())
    {
        group = ModuleVIB::getGroup();
        toIndentity(defCurve, MIE_SATMULT_NUMPOINTS);
    }

    parameters.addGroup("ModuleVIB", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(VIB_ON, this->bVibrancyOn);

        values.clear();
        for (i = 0 ; i < MIE_SATMULT_NUMPOINTS ; i++)
        {
            values.push_back(toString(aSaturationCurve[i]));
        }
        parameters.addParameter(VIB_SATURATION_CURVE, values);
        break;

    case SAVE_MIN:
        // bool does not have a min
        parameters.addParameterMin(VIB_ON);
        parameters.addParameterMin(VIB_SATURATION_CURVE);
        break;

    case SAVE_MAX:
        // bool does not have a max
        parameters.addParameterMax(VIB_ON);
        parameters.addParameterMax(VIB_SATURATION_CURVE);
        break;

    case SAVE_DEF:
        parameters.addParameterDef(VIB_ON);
        values.clear();
        for (i = 0 ; i < MIE_SATMULT_NUMPOINTS ; i++)
        {
            values.push_back(toString(defCurve[i]));
        }
        parameters.addParameter(VIB_SATURATION_CURVE, values);
        parameters.getParameter(VIB_SATURATION_CURVE.name)->setInfo(
                getParameterInfo(VIB_SATURATION_CURVE));
        break;
    }

    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

void ISPC::ModuleVIB::setupDoublePoints(double in1, double out1,
    double in2, double out2, double *satMult)
{
    // calculate the curve exponent (gamma)
    double vibSatControlPointIn = in1;
    double vibSatControlPointOut = out1;
    double vibSatControlPointIn2 = in2;
    double vibSatControlPointOut2 = out2;

    double x0, y0, x1, y1;
    double scale_a, offset_a,
        scale_b, offset_b,
        scale_c, offset_c;
    int i;
    double satIn;
    double satOut[MIE_SATMULT_NUMPOINTS];
    double filtSatOut[MIE_SATMULT_NUMPOINTS];

    if (vibSatControlPointIn2 < vibSatControlPointIn)
    {
        /*  we need to check that... research setup does not swap
         * points just overrides it... */
        // double tmp = vibSatControlPointIn;
        vibSatControlPointIn2 = vibSatControlPointIn;
        // vibSatControlPointIn2 = tmp;
    }

    /* the first linear segment: from satin = 0
     * to satin = vibSatControlPointIn; */
    x0 = 0;
    x1 = vibSatControlPointIn;
    y0 = 0;
    y1 = vibSatControlPointOut;
    if (x1 > x0)
    {
        scale_a  = (y1 - y0)/(x1- x0);
    }
    else
    {
        scale_a = 0;
    }
    offset_a = y0 - x0*scale_a;

    /* second linear segment from satin = vibSatControlPointIn
     * to satin = vibSatControlPointIn2 */
    x0 = vibSatControlPointIn;
    x1 = vibSatControlPointIn2;
    y0 = vibSatControlPointOut;
    y1 = vibSatControlPointOut2;
    if (x1 > x0)
    {
        scale_b  = (y1 - y0)/(x1- x0);
    }
    else
    {
        scale_b = 0;
    }
    offset_b = y0 - x0*scale_b;

    /* third linear segment from satin = vibSatControlPointIn2
     * to satin = 1 */
    x0 = vibSatControlPointIn2;
    x1 = 1.0;
    y0 = vibSatControlPointOut2;
    y1 = 1.0;
    if (x1 > x0)
    {
        scale_c  = (y1 - y0)/(x1- x0);
    }
    else
    {
        scale_c = 0;
    }
    offset_c = y0 - x0*scale_c;

    // build the saturation curve
    for (i = 0; i < MIE_SATMULT_NUMPOINTS; i++)
    {
        satIn = (static_cast<double>(i) / (MIE_SATMULT_NUMPOINTS - 1));
        if (satIn <= vibSatControlPointIn)
        {
            satOut[i] = scale_a*satIn + offset_a;
        }
        else if (satIn <= vibSatControlPointIn2)
        {
            satOut[i] = scale_b*satIn + offset_b;
        }
        else
        {
            satOut[i] = scale_c*satIn + offset_c;
        }

        filtSatOut[i] = satOut[i];
    }

    // smooth the saturation curve with a 5-point moving average
    for (i = 2; i < (MIE_SATMULT_NUMPOINTS - 2); i++)
    {
        filtSatOut[i] = (satOut[i - 2] + satOut[i - 1] + satOut[i]
            + satOut[i + 1] + satOut[i + 2]) / 5;
    }

    // calculate the multiplier curve
    // used to be in the loop when satIn == 0
    if (vibSatControlPointIn > 0.0)
    {
        satMult[0] = vibSatControlPointOut / vibSatControlPointIn;
    }
    else
    {
        satMult[0] = 1.0;
    }
    for (i = 1; i < MIE_SATMULT_NUMPOINTS; i++)
    {
        satIn = (static_cast<double>(i) / (MIE_SATMULT_NUMPOINTS - 1));
        satMult[i] = filtSatOut[i]/satIn;
    }
}

void ISPC::ModuleVIB::saturationCurve(double in1, double out1,
    double in2, double out2, double *satCurve)
{
    double vibSatControlPointIn = in1;
    double vibSatControlPointOut = out1;
    double vibSatControlPointIn2 = in2;
    double vibSatControlPointOut2 = out2;

    if (in1 > in2)
    {
        vibSatControlPointIn = in2;
        vibSatControlPointOut = out2;

        vibSatControlPointIn2 = in1;
        vibSatControlPointOut2 = out1;
    }

    double x0, y0, x1, y1;
    double scale_a, offset_a,
        scale_b, offset_b,
        scale_c, offset_c;

    /* the first linear segment: from satin = 0
     * to satin = vibSatControlPointIn; */
    x0 = 0;
    x1 = vibSatControlPointIn;
    y0 = 0;
    y1 = vibSatControlPointOut;
    if (x1 > x0)
    {
        scale_a = (y1 - y0) / (x1 - x0);
    }
    else
    {
        scale_a = 0;
    }
    offset_a = y0 - x0*scale_a;

    /* second linear segment from satin = vibSatControlPointIn
     * to satin = vibSatControlPointIn2 */
    x0 = vibSatControlPointIn;
    x1 = vibSatControlPointIn2;
    y0 = vibSatControlPointOut;
    y1 = vibSatControlPointOut2;
    if (x1 > x0)
    {
        scale_b = (y1 - y0) / (x1 - x0);
    }
    else
    {
        scale_b = 0;
    }
    offset_b = y0 - x0*scale_b;

    /* third linear segment from satin = vibSatControlPointIn2
     * to satin = 1 */
    x0 = vibSatControlPointIn2;
    x1 = 1.0;
    y0 = vibSatControlPointOut2;
    y1 = 1.0;
    if (x1 > x0)
    {
        scale_c = (y1 - y0) / (x1 - x0);
    }
    else
    {
        scale_c = 0;
    }
    offset_c = y0 - x0*scale_c;

    // compute the curve
    for (int i = 0; i < MIE_SATMULT_NUMPOINTS; i++)
    {
        double satIn = static_cast<double>(i) / (MIE_SATMULT_NUMPOINTS - 1);
        if (satIn <= vibSatControlPointIn)
        {
            satCurve[i] = scale_a*satIn + offset_a;
        }
        else if (satIn <= vibSatControlPointIn2)
        {
            satCurve[i] = scale_b*satIn + offset_b;
        }
        else
        {
            satCurve[i] = scale_c*satIn + offset_c;
        }
    }
}

void ISPC::ModuleVIB::gainsFromSaturationCurve(const double *satCurve,
    double *satGains)
{
    int i;
    double satIn;

    // satIn == 0 - multiplier is 1.0 as explained in HW TRM
    satGains[0] = 1.0;

    for (i = 1; i < MIE_SATMULT_NUMPOINTS; i++)
    {
        satIn = static_cast<double>(i) / (MIE_SATMULT_NUMPOINTS - 1);
        satGains[i] = satCurve[i]/satIn;
    }
}

IMG_STATIC_ASSERT(MIE_VIBRANCY_N == MIE_SATMULT_NUMPOINTS, VibCurveChanged);

IMG_RESULT ISPC::ModuleVIB::setup()
{
    LOG_PERF_IN();
    MC_PIPELINE *pMCPipeline = NULL;
    if (!pipeline)
    {
        MOD_LOG_ERROR("pipeline not set!\n");
        LOG_PERF_OUT();
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    pMCPipeline = pipeline->getMCPipeline();
    if (!pMCPipeline)
    {
        LOG_PERF_OUT();
        MOD_LOG_ERROR("pMCPipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    MC_MIE &MC_MIE = pMCPipeline->sMIE;

    // black level
    // MC is written in ModuleMIE and not used here
    /* fBlackLevel =
     * (this->BLACK_LEVEL + y_bot_range)*(1 << (MIE_BLACK_LEVEL_INT)); */

    MC_MIE.bVibrancy = this->bVibrancyOn;

    if (sizeof(MC_MIE.aVibrancySatMul[0]) == sizeof(double))
    {
        gainsFromSaturationCurve(aSaturationCurve, MC_MIE.aVibrancySatMul);
    }
    else
    {
        double gains[MIE_SATMULT_NUMPOINTS];
        unsigned int i;

        gainsFromSaturationCurve(aSaturationCurve, gains);

        for (i = 0; i < MIE_SATMULT_NUMPOINTS; i++)
        {
            MC_MIE.aVibrancySatMul[i] = gains[i];
        }
    }

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

void ISPC::ModuleVIB::toIndentity(double *curve, int s)
{
    curve[0] = 0.0;
    for (int i = 1 ; i < s ; i++)
    {
        curve[i] = static_cast<double>(i) / (s - 1);
    }
    // curve[s-1] = 1.0;
}
