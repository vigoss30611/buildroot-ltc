/**
*******************************************************************************
 @file ModuleSHA.cpp

 @brief Implementation of ISPC::ModuleSHA

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
#include "ispc/ModuleSHA.h"

#include <ctx_reg_precisions.h>
#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_SHA"

#include <cmath>

#ifdef USE_MATH_NEON
#include <mneon.h>
#endif


#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

const ISPC::ParamDef<double> ISPC::ModuleSHA::SHA_RADIUS("SHA_RADIUS",
    0.1, 10.0, 2.5);
const ISPC::ParamDef<double> ISPC::ModuleSHA::SHA_STRENGTH("SHA_STRENGTH",
    SHA_STRENGTH_MIN, SHA_STRENGTH_MAX, SHA_STRENGTH_DEF);
const ISPC::ParamDef<double> ISPC::ModuleSHA::SHA_THRESH("SHA_THRESH",
    0.0, 1.0, 0.0);
const ISPC::ParamDef<double> ISPC::ModuleSHA::SHA_DETAIL("SHA_DETAIL",
    0.0, 1.0, 1.0);
const ISPC::ParamDef<double> ISPC::ModuleSHA::SHA_EDGE_SCALE("SHA_EDGE_SCALE",
    0.0, 1.0, 0.25);
const ISPC::ParamDef<double> ISPC::ModuleSHA::SHA_EDGE_OFFSET(
    "SHA_EDGE_OFFSET", -1.0, 1.0, 0.0);

const ISPC::ParamDefSingle<bool> ISPC::ModuleSHA::SHA_DENOISE_BYPASS(
    "SHA_DENOISE_BYPASS", false);
const ISPC::ParamDef<double> ISPC::ModuleSHA::SHADN_TAU(
    "SHA_DN_TAU_MULTIPLIER", 0.0, 16.0, 1.0);
const ISPC::ParamDef<double> ISPC::ModuleSHA::SHADN_SIGMA(
    "SHA_DN_SIGMA_MULTIPLIER", 0.0, 16.0, 1.0);

#ifdef INFOTM_ISP

static const double sampleNoiseIndexes_Def[SHA_CURVE_NPOINTS] = {
    0.5, 0.75, 1, 1.5, 2,
    3, 4, 6, 8, 12,
    16, 24, 32, 64, 128};
    
static const double sampleSigmaValues_Def[SHA_CURVE_NPOINTS] = {
    0.525623, 0.525717, 0.563392, 0.538484, 0.538585,
    0.218964, 0.172387, 0.139622, 0.125163, 0.0808885,
    0.0453782, 0.0442295, 0.0440522, 0.0250851, 0.0159695};
    
static const double sampleTauValues_Def[SHA_CURVE_NPOINTS] = {
    13.998, 16.0988, 14.0367, 19.2767, 14.4795,
    31.1354, 37.9553, 44.1746, 44.8372, 46.4815,
    47.2445, 48.1339, 45.4722, 48.1982, 48.5187};
    
static const double sampleSharpenValues_Def[SHA_CURVE_NPOINTS] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const double sampleSigmaAndTauRatioValues_Def[SHA_CURVE_NPOINTS] = {
    219, 219, 219, 219, 219,
    219, 219, 219, 219, 219,
    219, 219, 219, 219, 219};

static const double sampleAlphaValues_Def[SHA_CURVE_NPOINTS] = {
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5,
    0.5, 0.5, 0.5, 0.5, 0.5};

static const double sampleW0_Def[SHA_CURVE_NPOINTS] = {
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,};

static const double sampleW1_Def[SHA_CURVE_NPOINTS] = {
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,};


static const double sampleW2_Def[SHA_CURVE_NPOINTS] = {
    2, 2, 2, 2, 2,
    2, 2, 2, 2, 2,
    2, 2, 2, 2, 2};


static const int sampleTau_Def[SHA_CURVE_NPOINTS] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0};

static const int sampleSigma_Def[SHA_CURVE_NPOINTS] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0};




const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_NOISE_CURVE("SHA_NOISE_CURVE", 0.0, 256.0, sampleNoiseIndexes_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_SIGMA_CURVE("SHA_SIGMA_CURVE", 0.0, 1.0, sampleSigmaValues_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_TAU_CURVE("SHA_TAU_CURVE", 0.0, 100.0, sampleTauValues_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_SHARPEN_CURVE("SHA_SHARPEN_CURVE", 0.0, 50.0, sampleSharpenValues_Def, SHA_CURVE_NPOINTS);

const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_W0_CURVE("SHA_W0_CURVE", -1000.0, 1000.0, sampleW0_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_W1_CURVE("SHA_W1_CURVE", -1000.0, 1000.0, sampleW1_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_W2_CURVE("SHA_W2_CURVE", -1000.0, 1000.0, sampleW2_Def, SHA_CURVE_NPOINTS);


const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_SIGMA_TAU_RATIO_CURVE("SHA_SIGMA_TAU_RATIO_CURVE", 0.0, 50000, sampleSigmaAndTauRatioValues_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<double> ISPC::ModuleSHA::SHA_ALPHA_CURVE("SHA_ALPHA_CURVE",-1000/*0.0001*/, 1000, sampleAlphaValues_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDef<double> ISPC::ModuleSHA::SHA_STRENGTH_FACTOR("SHA_STRENGTH_FACTOR",SHA_STRENGTH_FACTOR_MIN, SHA_STRENGTH_FACTOR_MAX, SHA_STRENGTH_FACTOR_DEF);

const ISPC::ParamDefSingle<bool> ISPC::ModuleSHA::SHA_SHARPEN_CURVE_ENABLE("SHA_SHARPEN_CURVE_ENABLE", false);


const ISPC::ParamDefSingle<bool> ISPC::ModuleSHA::SHA_CONFIG_SIGMA_TAU_BY_REGISTER_VALUE_ENABLE("SHA_SHARPEN_CURVE_ENABLE", false);

const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_TAU0_REG_CURVE("SHA_TAU0_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_TAU1_REG_CURVE("SHA_TAU1_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_TAU2_REG_CURVE("SHA_TAU2_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_TAU3_REG_CURVE("SHA_TAU3_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_TAU4_REG_CURVE("SHA_TAU4_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_TAU5_REG_CURVE("SHA_TAU5_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_TAU6_REG_CURVE("SHA_TAU6_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);

const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_SIGMA0_REG_CURVE("SHA_SIGMA0_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_SIGMA1_REG_CURVE("SHA_SIGMA1_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_SIGMA2_REG_CURVE("SHA_SIGMA2_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_SIGMA3_REG_CURVE("SHA_SIGMA3_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_SIGMA4_REG_CURVE("SHA_SIGMA4_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_SIGMA5_REG_CURVE("SHA_SIGMA5_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);
const ISPC::ParamDefArray<int> ISPC::ModuleSHA::SHA_SIGMA6_REG_CURVE("SHA_SIGMA6_REG_CURVE", 0, 65535, sampleTau_Def, SHA_CURVE_NPOINTS);



#else
static const double sampleNoiseIndexes[SHA_CURVE_NPOINTS] = {
    0.5, 0.75, 1, 1.5, 2,
    3, 4, 6, 8, 12,
    16, 24, 32, 64, 128};
    
static const double sampleSigmaValues[SHA_CURVE_NPOINTS] = {
    0.525623, 0.525717, 0.563392, 0.538484, 0.538585,
    0.218964, 0.172387, 0.139622, 0.125163, 0.0808885,
    0.0453782, 0.0442295, 0.0440522, 0.0250851, 0.0159695};
    
static const double sampleTauValues[SHA_CURVE_NPOINTS] = {
    13.998, 16.0988, 14.0367, 19.2767, 14.4795,
    31.1354, 37.9553, 44.1746, 44.8372, 46.4815,
    47.2445, 48.1339, 45.4722, 48.1982, 48.5187};
    
static const double sampleSharpenValues[SHA_CURVE_NPOINTS] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif

ISPC::ParameterGroup ISPC::ModuleSHA::getGroup()
{
    ParameterGroup group;

    group.header = "// Sharpening parameters";

    group.parameters.insert(SHA_RADIUS.name);
    group.parameters.insert(SHA_STRENGTH.name);
    group.parameters.insert(SHA_THRESH.name);
    group.parameters.insert(SHA_DETAIL.name);
    group.parameters.insert(SHA_EDGE_SCALE.name);
    group.parameters.insert(SHA_EDGE_OFFSET.name);

#ifdef INFOTM_ISP
    group.parameters.insert(SHA_DENOISE_BYPASS.name);
	group.parameters.insert(SHADN_TAU.name);
	group.parameters.insert(SHADN_SIGMA.name);

    group.parameters.insert(SHA_NOISE_CURVE.name);
    group.parameters.insert(SHA_SIGMA_CURVE.name);
    group.parameters.insert(SHA_TAU_CURVE.name);
    group.parameters.insert(SHA_SHARPEN_CURVE.name);
    group.parameters.insert(SHA_SIGMA_TAU_RATIO_CURVE.name);
    group.parameters.insert(SHA_ALPHA_CURVE.name);
    group.parameters.insert(SHA_W0_CURVE.name);
    group.parameters.insert(SHA_W1_CURVE.name);
    group.parameters.insert(SHA_W2_CURVE.name);
    group.parameters.insert(SHA_STRENGTH_FACTOR.name);
    group.parameters.insert(SHA_SHARPEN_CURVE_ENABLE.name);

    group.parameters.insert(SHA_CONFIG_SIGMA_TAU_BY_REGISTER_VALUE_ENABLE.name);

    group.parameters.insert(SHA_TAU0_REG_CURVE.name);
    group.parameters.insert(SHA_TAU1_REG_CURVE.name);
    group.parameters.insert(SHA_TAU2_REG_CURVE.name);
    group.parameters.insert(SHA_TAU3_REG_CURVE.name);
    group.parameters.insert(SHA_TAU4_REG_CURVE.name);
    group.parameters.insert(SHA_TAU5_REG_CURVE.name);
    group.parameters.insert(SHA_TAU6_REG_CURVE.name);

    group.parameters.insert(SHA_SIGMA0_REG_CURVE.name);
    group.parameters.insert(SHA_SIGMA1_REG_CURVE.name);
    group.parameters.insert(SHA_SIGMA2_REG_CURVE.name);
    group.parameters.insert(SHA_SIGMA3_REG_CURVE.name);
    group.parameters.insert(SHA_SIGMA4_REG_CURVE.name);
    group.parameters.insert(SHA_SIGMA5_REG_CURVE.name);
    group.parameters.insert(SHA_SIGMA6_REG_CURVE.name);

#endif //INFOTM_ISP	

    return group;
}

ISPC::ModuleSHA::ModuleSHA(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleSHA::load(const ParameterList &parameters)
{
    fRadius = parameters.getParameter(SHA_RADIUS);
    fStrength = parameters.getParameter(SHA_STRENGTH);
    fThreshold = parameters.getParameter(SHA_THRESH);
    fDetail = parameters.getParameter(SHA_DETAIL);
    fEdgeScale = parameters.getParameter(SHA_EDGE_SCALE);
    fEdgeOffset = parameters.getParameter(SHA_EDGE_OFFSET);

    bBypassDenoise = parameters.getParameter(SHA_DENOISE_BYPASS);
    fDenoiseTau = parameters.getParameter(SHADN_TAU);
    fDenoiseSigma = parameters.getParameter(SHADN_SIGMA);

#ifdef INFOTM_ISP
{
    int i;
    for (int i = 0; i < SHA_CURVE_NPOINTS; i++)
    {
        sampleNoiseIndexes[i]          = parameters.getParameter(SHA_NOISE_CURVE, i);

        sampleSigmaValues[i]           = parameters.getParameter(SHA_SIGMA_CURVE, i);
        sampleTauValues[i]             = parameters.getParameter(SHA_TAU_CURVE, i);
        sampleSharpenValues[i]         = parameters.getParameter(SHA_SHARPEN_CURVE, i);
        sampleSigmaAndTauRatioValue[i] = parameters.getParameter(SHA_SIGMA_TAU_RATIO_CURVE, i);
        sampleAlphaValue[i]            = parameters.getParameter(SHA_ALPHA_CURVE, i);

        sampleW0[i] = parameters.getParameter(SHA_W0_CURVE, i);
        sampleW1[i] = parameters.getParameter(SHA_W1_CURVE, i);
        sampleW2[i] = parameters.getParameter(SHA_W2_CURVE, i);
    }
    fStrengthFactor= parameters.getParameter(SHA_STRENGTH_FACTOR);

    bEnableSharpenCurve = parameters.getParameter(SHA_SHARPEN_CURVE_ENABLE);
    
    bEnableConfig2ndReg = parameters.getParameter(SHA_CONFIG_SIGMA_TAU_BY_REGISTER_VALUE_ENABLE);

    for (int i = 0; i < SHA_CURVE_NPOINTS; i++)
    {
        sampleTau0Reg[i] = parameters.getParameter(SHA_TAU0_REG_CURVE, i);
        sampleTau1Reg[i] = parameters.getParameter(SHA_TAU1_REG_CURVE, i);
        sampleTau2Reg[i] = parameters.getParameter(SHA_TAU2_REG_CURVE, i);
        sampleTau3Reg[i] = parameters.getParameter(SHA_TAU3_REG_CURVE, i);
        sampleTau4Reg[i] = parameters.getParameter(SHA_TAU4_REG_CURVE, i);
        sampleTau5Reg[i] = parameters.getParameter(SHA_TAU5_REG_CURVE, i);
        sampleTau6Reg[i] = parameters.getParameter(SHA_TAU6_REG_CURVE, i);

        sampleSigma0Reg[i] = parameters.getParameter(SHA_SIGMA0_REG_CURVE, i);
        sampleSigma1Reg[i] = parameters.getParameter(SHA_SIGMA1_REG_CURVE, i);
        sampleSigma2Reg[i] = parameters.getParameter(SHA_SIGMA2_REG_CURVE, i);
        sampleSigma3Reg[i] = parameters.getParameter(SHA_SIGMA3_REG_CURVE, i);
        sampleSigma4Reg[i] = parameters.getParameter(SHA_SIGMA4_REG_CURVE, i);
        sampleSigma5Reg[i] = parameters.getParameter(SHA_SIGMA5_REG_CURVE, i);
        sampleSigma6Reg[i] = parameters.getParameter(SHA_SIGMA6_REG_CURVE, i);
    }
}
#endif
    return IMG_SUCCESS;
}
IMG_RESULT ISPC::ModuleSHA::save(ParameterList &parameters, SaveType t) const
{
    static ParameterGroup group;

    if ( group.parameters.size() == 0 )
    {
        group = ModuleSHA::getGroup();
    }

    // the denoiser parameters are added to the denoiser group in ModuleDNS
    parameters.addGroup("ModuleSHA", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(SHA_RADIUS, this->fRadius);
        parameters.addParameter(SHA_STRENGTH, this->fStrength);
        parameters.addParameter(SHA_THRESH, this->fThreshold);
        parameters.addParameter(SHA_DETAIL, this->fDetail);
        parameters.addParameter(SHA_EDGE_SCALE, this->fEdgeScale);
        parameters.addParameter(SHA_EDGE_OFFSET, this->fEdgeOffset);

        parameters.addParameter(SHA_DENOISE_BYPASS, this->bBypassDenoise);
        parameters.addParameter(SHADN_TAU, this->fDenoiseTau);
        parameters.addParameter(SHADN_SIGMA, this->fDenoiseSigma);
#ifdef INFOTM_ISP
        {
            int i;
            std::vector<std::string> values;

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleNoiseIndexes[i]));
            }
            parameters.addParameter(SHA_NOISE_CURVE, values);

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleSigmaValues[i]));
            }
            parameters.addParameter(SHA_SIGMA_CURVE, values);

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleTauValues[i]));
            }
            parameters.addParameter(SHA_TAU_CURVE, values);
            
            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleSharpenValues[i]));
            }
            parameters.addParameter(SHA_SHARPEN_CURVE, values);

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleSigmaAndTauRatioValue[i]));
            }
            parameters.addParameter(SHA_SIGMA_TAU_RATIO_CURVE, values);

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleAlphaValue[i]));
            }
            parameters.addParameter(SHA_ALPHA_CURVE, values);

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleW0[i]));
            }
            parameters.addParameter(SHA_W0_CURVE, values);

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleW1[i]));
            }
            parameters.addParameter(SHA_W1_CURVE, values);

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleW2[i]));
            }
            parameters.addParameter(SHA_W2_CURVE, values);

            parameters.addParameter(SHA_STRENGTH_FACTOR, this->fStrengthFactor);
            parameters.addParameter(SHA_SHARPEN_CURVE_ENABLE, this->bEnableSharpenCurve);

            parameters.addParameter(SHA_CONFIG_SIGMA_TAU_BY_REGISTER_VALUE_ENABLE, this->bEnableConfig2ndReg);
{
            std::vector<std::string> values_1;
            std::vector<std::string> values_2;
            std::vector<std::string> values_3;
            std::vector<std::string> values_4;
            std::vector<std::string> values_5;
            std::vector<std::string> values_6;

            values.clear();
            values_1.clear();
            values_2.clear();
            values_3.clear();
            values_4.clear();
            values_5.clear();
            values_6.clear();
            
            for (i = 0; i < SHA_CURVE_NPOINTS; i++)
            {
                values.push_back(toString(sampleTau0Reg[i]));
                values_1.push_back(toString(sampleTau1Reg[i]));
                values_2.push_back(toString(sampleTau2Reg[i]));
                values_3.push_back(toString(sampleTau3Reg[i]));
                values_4.push_back(toString(sampleTau4Reg[i]));
                values_5.push_back(toString(sampleTau5Reg[i]));
                values_6.push_back(toString(sampleTau6Reg[i]));

            }
            parameters.addParameter(SHA_TAU0_REG_CURVE, values);
            parameters.addParameter(SHA_TAU1_REG_CURVE, values_1);
            parameters.addParameter(SHA_TAU2_REG_CURVE, values_2);
            parameters.addParameter(SHA_TAU3_REG_CURVE, values_3);
            parameters.addParameter(SHA_TAU4_REG_CURVE, values_4);
            parameters.addParameter(SHA_TAU5_REG_CURVE, values_5);
            parameters.addParameter(SHA_TAU6_REG_CURVE, values_6);

            values.clear();
            values_1.clear();
            values_2.clear();
            values_3.clear();
            values_4.clear();
            values_5.clear();
            values_6.clear();

            for (i = 0; i < SHA_CURVE_NPOINTS; i++)
            {
                values.push_back(toString(sampleSigma0Reg[i]));
                values_1.push_back(toString(sampleSigma1Reg[i]));
                values_2.push_back(toString(sampleSigma2Reg[i]));
                values_3.push_back(toString(sampleSigma3Reg[i]));
                values_4.push_back(toString(sampleSigma4Reg[i]));
                values_5.push_back(toString(sampleSigma5Reg[i]));
                values_6.push_back(toString(sampleSigma6Reg[i]));
            }
            parameters.addParameter(SHA_SIGMA0_REG_CURVE, values);
            parameters.addParameter(SHA_SIGMA1_REG_CURVE, values_1);
            parameters.addParameter(SHA_SIGMA2_REG_CURVE, values_2);
            parameters.addParameter(SHA_SIGMA3_REG_CURVE, values_3);
            parameters.addParameter(SHA_SIGMA4_REG_CURVE, values_4);
            parameters.addParameter(SHA_SIGMA5_REG_CURVE, values_5);
            parameters.addParameter(SHA_SIGMA6_REG_CURVE, values_6);
}
        }
#endif
        break;

    case SAVE_MIN:
        parameters.addParameterMin(SHA_RADIUS);
        parameters.addParameterMin(SHA_STRENGTH);
        parameters.addParameterMin(SHA_THRESH);
        parameters.addParameterMin(SHA_DETAIL);
        parameters.addParameterMin(SHA_EDGE_SCALE);
        parameters.addParameterMin(SHA_EDGE_OFFSET);

        parameters.addParameterMin(SHA_DENOISE_BYPASS);  // bool does not have a min
        parameters.addParameterMin(SHADN_TAU);
        parameters.addParameterMin(SHADN_SIGMA);

#ifdef INFOTM_ISP
        parameters.addParameterMin(SHA_NOISE_CURVE);
        parameters.addParameterMin(SHA_SIGMA_CURVE);
        parameters.addParameterMin(SHA_TAU_CURVE);
        parameters.addParameterMin(SHA_SHARPEN_CURVE);
        parameters.addParameterMin(SHA_SIGMA_TAU_RATIO_CURVE);
        parameters.addParameterMin(SHA_ALPHA_CURVE);
        parameters.addParameterMin(SHA_W0_CURVE);
        parameters.addParameterMin(SHA_W1_CURVE);
        parameters.addParameterMin(SHA_W2_CURVE);
        parameters.addParameterMin(SHA_STRENGTH_FACTOR);
        parameters.addParameterMin(SHA_SHARPEN_CURVE_ENABLE);

        parameters.addParameterMin(SHA_CONFIG_SIGMA_TAU_BY_REGISTER_VALUE_ENABLE);
        parameters.addParameterMin(SHA_TAU0_REG_CURVE);
        parameters.addParameterMin(SHA_TAU1_REG_CURVE);
        parameters.addParameterMin(SHA_TAU2_REG_CURVE);
        parameters.addParameterMin(SHA_TAU3_REG_CURVE);
        parameters.addParameterMin(SHA_TAU4_REG_CURVE);
        parameters.addParameterMin(SHA_TAU5_REG_CURVE);
        parameters.addParameterMin(SHA_TAU6_REG_CURVE);
        parameters.addParameterMin(SHA_SIGMA0_REG_CURVE);
        parameters.addParameterMin(SHA_SIGMA1_REG_CURVE);
        parameters.addParameterMin(SHA_SIGMA2_REG_CURVE);
        parameters.addParameterMin(SHA_SIGMA3_REG_CURVE);
        parameters.addParameterMin(SHA_SIGMA4_REG_CURVE);
        parameters.addParameterMin(SHA_SIGMA5_REG_CURVE);
        parameters.addParameterMin(SHA_SIGMA6_REG_CURVE);

#endif

        break;

    case SAVE_MAX:
        parameters.addParameterMax(SHA_RADIUS);
        parameters.addParameterMax(SHA_STRENGTH);
        parameters.addParameterMax(SHA_THRESH);
        parameters.addParameterMax(SHA_DETAIL);
        parameters.addParameterMax(SHA_EDGE_SCALE);
        parameters.addParameterMax(SHA_EDGE_OFFSET);

        parameters.addParameterMax(SHA_DENOISE_BYPASS);  // bool does not have a max
        parameters.addParameterMax(SHADN_TAU);
        parameters.addParameterMax(SHADN_SIGMA);

#ifdef INFOTM_ISP
        parameters.addParameterMax(SHA_NOISE_CURVE);
        parameters.addParameterMax(SHA_SIGMA_CURVE);
        parameters.addParameterMax(SHA_TAU_CURVE);
        parameters.addParameterMax(SHA_SHARPEN_CURVE);
        parameters.addParameterMax(SHA_SIGMA_TAU_RATIO_CURVE);
        parameters.addParameterMax(SHA_ALPHA_CURVE);
        parameters.addParameterMax(SHA_W0_CURVE);
        parameters.addParameterMax(SHA_W1_CURVE);
        parameters.addParameterMax(SHA_W2_CURVE);
        parameters.addParameterMax(SHA_STRENGTH_FACTOR);
        parameters.addParameterMax(SHA_SHARPEN_CURVE_ENABLE);

        parameters.addParameterMax(SHA_CONFIG_SIGMA_TAU_BY_REGISTER_VALUE_ENABLE);
        parameters.addParameterMax(SHA_TAU0_REG_CURVE);
        parameters.addParameterMax(SHA_TAU1_REG_CURVE);
        parameters.addParameterMax(SHA_TAU2_REG_CURVE);
        parameters.addParameterMax(SHA_TAU3_REG_CURVE);
        parameters.addParameterMax(SHA_TAU4_REG_CURVE);
        parameters.addParameterMax(SHA_TAU5_REG_CURVE);
        parameters.addParameterMax(SHA_TAU6_REG_CURVE);
        parameters.addParameterMax(SHA_SIGMA0_REG_CURVE);
        parameters.addParameterMax(SHA_SIGMA1_REG_CURVE);
        parameters.addParameterMax(SHA_SIGMA2_REG_CURVE);
        parameters.addParameterMax(SHA_SIGMA3_REG_CURVE);
        parameters.addParameterMax(SHA_SIGMA4_REG_CURVE);
        parameters.addParameterMax(SHA_SIGMA5_REG_CURVE);
        parameters.addParameterMax(SHA_SIGMA6_REG_CURVE);
#endif
        break;

    case SAVE_DEF:
        parameters.addParameterDef(SHA_RADIUS);
        parameters.addParameterDef(SHA_STRENGTH);
        parameters.addParameterDef(SHA_THRESH);
        parameters.addParameterDef(SHA_DETAIL);
        parameters.addParameterDef(SHA_EDGE_SCALE);
        parameters.addParameterDef(SHA_EDGE_OFFSET);

        parameters.addParameterDef(SHA_DENOISE_BYPASS);
        parameters.addParameterDef(SHADN_TAU);
        parameters.addParameterDef(SHADN_SIGMA);
#ifdef INFOTM_ISP
        {
            int i;
            std::vector<std::string> values;

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleNoiseIndexes_Def[i]));
            }
            parameters.addParameter(SHA_NOISE_CURVE, values);
            parameters.getParameter(SHA_NOISE_CURVE.name)->setInfo(getParameterInfo(SHA_NOISE_CURVE));

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleSigmaValues_Def[i]));
            }
            parameters.addParameter(SHA_SIGMA_CURVE, values);
            parameters.getParameter(SHA_SIGMA_CURVE.name)->setInfo(getParameterInfo(SHA_SIGMA_CURVE));

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleTauValues_Def[i]));
            }
            parameters.addParameter(SHA_TAU_CURVE, values);
            parameters.getParameter(SHA_TAU_CURVE.name)->setInfo(getParameterInfo(SHA_TAU_CURVE));
            
            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleSharpenValues_Def[i]));
            }
            parameters.addParameter(SHA_SHARPEN_CURVE, values);
            parameters.getParameter(SHA_SIGMA_TAU_RATIO_CURVE.name)->setInfo(getParameterInfo(SHA_SIGMA_TAU_RATIO_CURVE));

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleSigmaAndTauRatioValues_Def[i]));
            }
            parameters.addParameter(SHA_SIGMA_TAU_RATIO_CURVE, values);
            parameters.getParameter(SHA_SIGMA_TAU_RATIO_CURVE.name)->setInfo(getParameterInfo(SHA_SIGMA_TAU_RATIO_CURVE));

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleAlphaValues_Def[i]));
            }
            parameters.addParameter(SHA_ALPHA_CURVE, values);
            parameters.getParameter(SHA_ALPHA_CURVE.name)->setInfo(getParameterInfo(SHA_ALPHA_CURVE));

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleW0_Def[i]));
            }
            parameters.addParameter(SHA_W0_CURVE, values);
            parameters.getParameter(SHA_W0_CURVE.name)->setInfo(getParameterInfo(SHA_W0_CURVE));

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleW1_Def[i]));
            }
            parameters.addParameter(SHA_W1_CURVE, values);
            parameters.getParameter(SHA_W1_CURVE.name)->setInfo(getParameterInfo(SHA_W1_CURVE));

            values.clear();
            for (i = 0 ; i < SHA_CURVE_NPOINTS ; i++)
            {
                values.push_back(toString(sampleW2_Def[i]));
            }
            parameters.addParameter(SHA_W2_CURVE, values);
            parameters.getParameter(SHA_W2_CURVE.name)->setInfo(getParameterInfo(SHA_W2_CURVE));

            parameters.addParameterDef(SHA_STRENGTH_FACTOR);
            parameters.addParameterDef(SHA_SHARPEN_CURVE_ENABLE);

            parameters.addParameterDef(SHA_CONFIG_SIGMA_TAU_BY_REGISTER_VALUE_ENABLE);
{
            std::vector<std::string> values_1;
            std::vector<std::string> values_2;
            std::vector<std::string> values_3;
            std::vector<std::string> values_4;
            std::vector<std::string> values_5;
            std::vector<std::string> values_6;

            values.clear();
            values_1.clear();
            values_2.clear();
            values_3.clear();
            values_4.clear();
            values_5.clear();
            values_6.clear();
            
            for (i = 0; i < SHA_CURVE_NPOINTS; i++)
            {
                values.push_back(toString(sampleTau_Def[i]));
                values_1.push_back(toString(sampleTau_Def[i]));
                values_2.push_back(toString(sampleTau_Def[i]));
                values_3.push_back(toString(sampleTau_Def[i]));
                values_4.push_back(toString(sampleTau_Def[i]));
                values_5.push_back(toString(sampleTau_Def[i]));
                values_6.push_back(toString(sampleTau_Def[i]));

            }
            parameters.addParameter(SHA_TAU0_REG_CURVE, values);
            parameters.addParameter(SHA_TAU1_REG_CURVE, values_1);
            parameters.addParameter(SHA_TAU2_REG_CURVE, values_2);
            parameters.addParameter(SHA_TAU3_REG_CURVE, values_3);
            parameters.addParameter(SHA_TAU4_REG_CURVE, values_4);
            parameters.addParameter(SHA_TAU5_REG_CURVE, values_5);
            parameters.addParameter(SHA_TAU6_REG_CURVE, values_6);

            values.clear();
            values_1.clear();
            values_2.clear();
            values_3.clear();
            values_4.clear();
            values_5.clear();
            values_6.clear();

            for (i = 0; i < SHA_CURVE_NPOINTS; i++)
            {
                values.push_back(toString(sampleSigma_Def[i]));
                values_1.push_back(toString(sampleSigma_Def[i]));
                values_2.push_back(toString(sampleSigma_Def[i]));
                values_3.push_back(toString(sampleSigma_Def[i]));
                values_4.push_back(toString(sampleSigma_Def[i]));
                values_5.push_back(toString(sampleSigma_Def[i]));
                values_6.push_back(toString(sampleSigma_Def[i]));
            }
            parameters.addParameter(SHA_SIGMA0_REG_CURVE, values);
            parameters.addParameter(SHA_SIGMA1_REG_CURVE, values_1);
            parameters.addParameter(SHA_SIGMA2_REG_CURVE, values_2);
            parameters.addParameter(SHA_SIGMA3_REG_CURVE, values_3);
            parameters.addParameter(SHA_SIGMA4_REG_CURVE, values_4);
            parameters.addParameter(SHA_SIGMA5_REG_CURVE, values_5);
            parameters.addParameter(SHA_SIGMA6_REG_CURVE, values_6);
}

        }
#endif

        break;
    }

    return IMG_SUCCESS;
}

#define DEFAULT_BASE_ISO 100
IMG_RESULT ISPC::ModuleSHA::setup()
{
    LOG_PERF_IN();
    double sensorGain, wellDepth, noiseIndex;
    double sigma = 0.0;
    double sharpBaseStrength = 0.0;
    double tau = 0.0;
    double weight = 0.0;
    int n, k;
#ifdef INFOTM_ISP
    int curve_index;
    double L;
#else
    IMG_INT32 L;
#endif
    double ALPHA;

    // sharpening
    // these are used to convert from setup parameter value to register value
    double  max_register;
    double sharpeningRadius;
    double w0, w1, w2, weight_norm;
    double sharpeningThreshold;
    double sharpeningStrength;
    double sharpeningDetail;
    double sharpeningEdgeScale;
    double sharpeningEdgeOffset;
    double pipelineNorm;
    int DE;
    double bias_thres;

    MC_PIPELINE *pMCPipeline = NULL;
    const Sensor *sensor = NULL;
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
    sensor = pipeline->getSensor();
    if (!sensor)
    {
        MOD_LOG_ERROR("Pipeline does not have a sensor to get "\
            "information from!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    sensorGain = sensor->getGain();
    wellDepth = sensor->uiWellDepth;

    noiseIndex = wellDepth / (100 * sensorGain);

#ifdef INFOTM_ISP
    curve_index = (sensor->getGain()*(SHA_CURVE_NPOINTS-1))/sensor->getMaxGain();
/*	
    printf("&&&&noiseIndex = %f, sensorGain =%f, wellDepth = %f, curve_index=%d, L=%f,alpha=%f,sharpen_factor=%f\nsampleW0=%f, sampleW1=%f, sampleW2=%f\n", 
           noiseIndex, sensorGain, wellDepth, 
           curve_index,
           sampleSigmaAndTauRatioValue[curve_index], 
           sampleAlphaValue[curve_index], 
           sampleSharpenValues[curve_index],
           sampleW0[curve_index],
           sampleW1[curve_index],
           sampleW2[curve_index]);
*/
#endif

    if (noiseIndex >= sampleNoiseIndexes[SHA_CURVE_NPOINTS - 1]) {
        sigma = sampleSigmaValues[SHA_CURVE_NPOINTS - 1];
        tau = sampleTauValues[SHA_CURVE_NPOINTS - 1];
        sharpBaseStrength = sampleSharpenValues[14];
    } else if (noiseIndex <= sampleNoiseIndexes[0]) {
        sigma = sampleSigmaValues[0];
        tau = sampleTauValues[0];
        sharpBaseStrength = sampleSharpenValues[0];
    } else {
        for (n = 0; n < SHA_CURVE_NPOINTS - 1; n++) {
            if (noiseIndex >= sampleNoiseIndexes[n]
                && noiseIndex < sampleNoiseIndexes[n+1]) {
                // Do log2 interpolation between sample points
                weight = (ISPC::log2(noiseIndex) - ISPC::log2(sampleNoiseIndexes[n])) / (ISPC::log2(sampleNoiseIndexes[n+1]) - ISPC::log2(sampleNoiseIndexes[n]));
                sigma = weight * sampleSigmaValues[n + 1] + (1 - weight) * sampleSigmaValues[n];
                tau = weight * sampleTauValues[n + 1] + (1 - weight) * sampleTauValues[n];
                sharpBaseStrength = weight * sampleSharpenValues[n + 1] + (1 - weight) * sampleSharpenValues[n];
                break;
            }
        }
    }

    tau = tau * fDenoiseTau;
    sigma = sigma * fDenoiseSigma;

#ifndef INFOTM_ISP
    L = 219;     // For explanation, see "Appendix to 1.2" in functional spec
    ALPHA = 0.5; // "Edge contrast normalize factor"
	
    for (k = 0; k < SHA_N_COMPARE; k++) {
#ifdef USE_MATH_NEON
        pMCPipeline->sSHA.aDNSimil[k] = static_cast<IMG_INT32>(0.5 + (L* powf_neon((2 * powf_neon(sigma, 2.0) * ((SHA_N_COMPARE + 1) - k - 1.5)), (1.0 / (2 * ALPHA)))));
#else
        pMCPipeline->sSHA.aDNSimil[k] = static_cast<IMG_INT32>(0.5 + (L* pow((2 * pow(sigma, 2.0) * ((SHA_N_COMPARE + 1) - k - 1.5)), (1.0 / (2 * ALPHA)))));
#endif
        pMCPipeline->sSHA.aDNAvoid[k] = static_cast<IMG_INT32>(0.5 + (L / tau* ((SHA_N_COMPARE + 1) - k - 1.5)));
        //printf("L %f, ALPHA %f, sigma %f, tau %f, <%d,%d>\n", L, ALPHA, sigma, tau, pMCPipeline->sSHA.aDNSimil[k], pMCPipeline->sSHA.aDNAvoid[k]);
    }
#else
    /* Now that we have values for sigma and tau, we calculate the
     * comparison table entries */
    if (!bEnableConfig2ndReg)
    {
        L = sampleSigmaAndTauRatioValue[curve_index]; // For explanation, see "Appendix to 1.2" in functional spec
        ALPHA = sampleAlphaValue[curve_index];        // "Edge contrast normalize factor"
        if (ALPHA == 0)
        {
            ALPHA = 0.0000000001;
        }
        //printf("==>sha_sigma_tau \n");
        for (k = 0; k < SHA_N_COMPARE; k++) {
#ifdef USE_MATH_NEON
            pMCPipeline->sSHA.aDNSimil[k] = static_cast<IMG_INT32>(0.5 + (L* powf_neon((2 * powf_neon(sigma, 2.0) * ((SHA_N_COMPARE + 1) - k - 1.5)), (1.0 / (2 * ALPHA)))));
#else
            pMCPipeline->sSHA.aDNSimil[k] = static_cast<IMG_INT32>(0.5 + (L* pow((2 * pow(sigma, 2.0) * ((SHA_N_COMPARE + 1) - k - 1.5)), (1.0 / (2 * ALPHA)))));
#endif
            pMCPipeline->sSHA.aDNAvoid[k] = static_cast<IMG_INT32>(0.5 + (L / tau* ((SHA_N_COMPARE + 1) - k - 1.5)));
            //printf("L %f, ALPHA %f, sigma %f, tau %f, <%d,%d>\n", L, ALPHA, sigma, tau, pMCPipeline->sSHA.aDNSimil[k], pMCPipeline->sSHA.aDNAvoid[k]);
        }
        //printf("<==\n");
    }
    else
    {
        pMCPipeline->sSHA.aDNSimil[0] = sampleSigma0Reg[curve_index];
        pMCPipeline->sSHA.aDNSimil[1] = sampleSigma1Reg[curve_index];
        pMCPipeline->sSHA.aDNSimil[2] = sampleSigma2Reg[curve_index];
        pMCPipeline->sSHA.aDNSimil[3] = sampleSigma3Reg[curve_index];
        pMCPipeline->sSHA.aDNSimil[4] = sampleSigma4Reg[curve_index];
        pMCPipeline->sSHA.aDNSimil[5] = sampleSigma5Reg[curve_index];
        pMCPipeline->sSHA.aDNSimil[6] = sampleSigma6Reg[curve_index];
        
        pMCPipeline->sSHA.aDNAvoid[0] = sampleTau0Reg[curve_index];
        pMCPipeline->sSHA.aDNAvoid[1] = sampleTau1Reg[curve_index];
        pMCPipeline->sSHA.aDNAvoid[2] = sampleTau2Reg[curve_index];
        pMCPipeline->sSHA.aDNAvoid[3] = sampleTau3Reg[curve_index];
        pMCPipeline->sSHA.aDNAvoid[4] = sampleTau4Reg[curve_index];
        pMCPipeline->sSHA.aDNAvoid[5] = sampleTau5Reg[curve_index];
        pMCPipeline->sSHA.aDNAvoid[6] = sampleTau6Reg[curve_index];
/*
        printf("***Tau_reg<%d, %d, %d, %d, %d, %d, %d>, Sigma_reg<%d, %d, %d, %d, %d, %d, %d>***\n", 
               pMCPipeline->sSHA.aDNSimil[0], pMCPipeline->sSHA.aDNSimil[1], pMCPipeline->sSHA.aDNSimil[2], 
               pMCPipeline->sSHA.aDNSimil[3], pMCPipeline->sSHA.aDNSimil[4], pMCPipeline->sSHA.aDNSimil[5],
               pMCPipeline->sSHA.aDNSimil[6],
               pMCPipeline->sSHA.aDNAvoid[0], pMCPipeline->sSHA.aDNAvoid[1], pMCPipeline->sSHA.aDNAvoid[2],
               pMCPipeline->sSHA.aDNAvoid[3], pMCPipeline->sSHA.aDNAvoid[4], pMCPipeline->sSHA.aDNAvoid[5],
               pMCPipeline->sSHA.aDNAvoid[6]);
*/
    }
#endif
    // sharpening

    // radius
    sharpeningRadius = ispc_min(10.0f, this->fRadius);

#ifdef INFOTM_ISP
    w0 = sampleW0[curve_index];
    w1 = exp(-((sampleW1[curve_index] * sampleW1[curve_index]) / (sharpeningRadius*sharpeningRadius)));
    w2 = exp(-((sampleW2[curve_index] * sampleW2[curve_index]) / (sharpeningRadius*sharpeningRadius)));
#else
    w0 = 1;
    w1 = exp(-((1 * 1) / (sharpeningRadius*sharpeningRadius)));
    w2 = exp(-((2 * 2) / (sharpeningRadius*sharpeningRadius)));
#endif
    weight_norm = w0 + w1 + w2;

    pipelineNorm = static_cast<double>(1 << (SHA_GWEIGHT_0_INT + SHA_GWEIGHT_0_FRAC)) - 1;

#ifdef INFOTM_ISP
    pMCPipeline->sSHA.aGainWeigth[0] = sampleW0[curve_index];//floor(pipelineNorm) - floor(pipelineNorm*w0 / weight_norm);
    pMCPipeline->sSHA.aGainWeigth[1] = sampleW1[curve_index];//floor(pipelineNorm*w1 / weight_norm);
    pMCPipeline->sSHA.aGainWeigth[2] = sampleW2[curve_index];//floor(pipelineNorm*w2 / weight_norm);

    pMCPipeline->sSHA.aGainWeigth[3] = sampleW2[curve_index] - 2;
    if (pMCPipeline->sSHA.aGainWeigth[3] < 0)
    {
        pMCPipeline->sSHA.aGainWeigth[3] = 0;
    }
    
    pMCPipeline->sSHA.aGainWeigth[4] = pMCPipeline->sSHA.aGainWeigth[3] - 2;
    if (pMCPipeline->sSHA.aGainWeigth[4] < 0)
    {
        pMCPipeline->sSHA.aGainWeigth[4] = 0;
    }

    pMCPipeline->sSHA.aGainWeigth[5] = pMCPipeline->sSHA.aGainWeigth[4] - 2;
    if (pMCPipeline->sSHA.aGainWeigth[5] < 0)
    {
        pMCPipeline->sSHA.aGainWeigth[5] = 0;
    }
/*
    printf("&&&&SHA_Gain<%f, %f, %f, %f, %f, %f>, W<%f, %f, %f>, pipelineNorm=%f&&&&\n", 
           pMCPipeline->sSHA.aGainWeigth[0], 
           pMCPipeline->sSHA.aGainWeigth[1], 
           pMCPipeline->sSHA.aGainWeigth[2], 
           pMCPipeline->sSHA.aGainWeigth[3], 
           pMCPipeline->sSHA.aGainWeigth[4], 
           pMCPipeline->sSHA.aGainWeigth[5], w0, w1, w2, pipelineNorm);
*/
#else
    pMCPipeline->sSHA.aGainWeigth[0] = floor(pipelineNorm) - floor(pipelineNorm*w0 / weight_norm);
    pMCPipeline->sSHA.aGainWeigth[1] = floor(pipelineNorm*w1 / weight_norm);
    pMCPipeline->sSHA.aGainWeigth[2] = floor(pipelineNorm*w2 / weight_norm);

    DE = static_cast<int>(pMCPipeline->sSHA.aGainWeigth[0] - pMCPipeline->sSHA.aGainWeigth[1] - pMCPipeline->sSHA.aGainWeigth[2]);
    if (DE > 0)
    {
        pMCPipeline->sSHA.aGainWeigth[2] += DE;
    }
    else
    {
        pMCPipeline->sSHA.aGainWeigth[0] += DE;
    }
#endif
    // threshold
    // this is the register value corresponding to the maximum setup value
    max_register = static_cast<double>((1 << (SHA_THRESH_INT)));
    sharpeningThreshold = 0.125*this->fThreshold;

    // minimum possible value to add with the SHA_THRES precision
    bias_thres = 1.0 / (1 << SHA_THRESH_FRAC);

    if (sharpeningThreshold > 1.0) {
        sharpeningThreshold = 1.0;
    }
    pMCPipeline->sSHA.ui16Threshold = static_cast<IMG_INT32>(bias_thres
        + sharpeningThreshold * max_register);
    if (pMCPipeline->sSHA.ui16Threshold > max_register) {
        pMCPipeline->sSHA.ui16Threshold = static_cast<int>(max_register);
    }

    // strength
    /** @ should be using SHA_STRENGTH precision instead of the
     * threshold one for the strength? */
    // this is the register value corresponding to the maximum setup value
    max_register = static_cast<double>((1 << (SHA_THRESH_INT)) - 1);

#ifdef INFOTM_ISP
    if (!bEnableSharpenCurve)
    {
        sharpeningStrength = fStrengthFactor * this->fStrength;
    }
    else
    {
        sharpeningStrength = sampleSharpenValues[curve_index] * this->fStrength;
    }
#else
    sharpeningStrength = 0.56 * this->fStrength;
#endif

    if (sharpeningStrength > 1) {  // clip if above maximum
        sharpeningStrength = 1.0;
    }
    pMCPipeline->sSHA.fStrength = (sharpeningStrength * max_register);

    // detail
    // this is the register value corresponding to the maximum setup value
    max_register = static_cast<double>((1 << (SHA_DETAIL_INT)));

    sharpeningDetail = this->fDetail;
    if (sharpeningDetail > 1.0) {  // clip if above maximum
        sharpeningDetail = 1.0;
    }
    pMCPipeline->sSHA.fDetail = (sharpeningDetail * max_register);

    // edge scale
    // this is the register value corresponding to the maximum setup value
    max_register = static_cast<double>((1 << (SHA_ELW_SCALE_INT + SHA_ELW_SCALE_FRAC)) - 1);

    sharpeningEdgeScale = this->fEdgeScale;
    if (sharpeningEdgeScale > 1.0) {  // clip if above maximum
        sharpeningEdgeScale = 1.0;
    }
    pMCPipeline->sSHA.fELWScale = (sharpeningEdgeScale * max_register);

    // edge offset  signed: min_register < 0
    // this is the register value corresponding to the maximum setup value
    max_register = static_cast<double>((1 << (SHA_ELW_OFFS_INT)));

    sharpeningEdgeOffset = this->fEdgeOffset;
    if (sharpeningEdgeOffset > 1.0) {  // clip if above maximum
        sharpeningEdgeOffset = 1.0;
    }
    if (sharpeningEdgeOffset < -1.0) {  // clip if above maximum
        sharpeningEdgeOffset = -1.0;
    }

    pMCPipeline->sSHA.i16ELWOffset =
        static_cast<IMG_INT32>(sharpeningEdgeOffset*max_register / 2.0);

    // inveresed
    pMCPipeline->sSHA.bDenoise =  this->bBypassDenoise ? IMG_FALSE : IMG_TRUE;

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
