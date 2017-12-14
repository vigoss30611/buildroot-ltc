/**
*******************************************************************************
 @file ModuleMIE.cpp

 @brief Implementation of ISPC::ModuleMIE

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
#include "ispc/ModuleMIE.h"

#include <ctx_reg_precisions.h>
#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_MIE"

#include <cmath>
#include <string>
#include <vector>
#include <list>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "ispc/ModuleOUT.h"
#include "ispc/Pipeline.h"

const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_BLC(
    "MIE_BLACK_LEVEL",  0.0, 1.0, 0.0625);

const ISPC::ParamDef<unsigned int> ISPC::ModuleMIE::MIE_MEMORY_COLOURS(
    "MIE_MEMORY_COLOURS", 0, 3 * MIE_NUM_MEMCOLOURS, MIE_NUM_MEMCOLOURS);

// 1 value per MC as in MIE_ENALED_0 MIE_ENALED_1 etc
const ISPC::ParamDefSingle<bool> ISPC::ModuleMIE::MIE_ENABLED_S(
    "MIE_ENABLED", false);

// 1 value per MC
const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_YMIN_S(
    "MIE_YMIN", 0.0, 1.0, 0.0);

// 1 value per MC
const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_YMAX_S(
    "MIE_YMAX", 0.0, 2.0, 1.0);

#define MIE_ROTCENTER_MIN -0.5
#define MIE_ROTCENTER_MAX 0.5

// 2 values per MC: Cb and Cr
static const double mie_ccenter_def[2] = { 0.0, 0.0 };
const ISPC::ParamDefArray<double> ISPC::ModuleMIE::MIE_CCENTER_S(
    "MIE_CCENTER", MIE_ROTCENTER_MIN, MIE_ROTCENTER_MAX, mie_ccenter_def, 2);

// 4 slices per MC
static const double mie_ygains_def[MIE_NUM_SLICES] = { 0.0, 0.0, 0.0, 0.0 };
const ISPC::ParamDefArray<double> ISPC::ModuleMIE::MIE_YGAINS_S(
    "MIE_YGAINS", 0.0, 1.0, mie_ygains_def, MIE_NUM_SLICES);

// 4 slices per MC
static const double mie_cextent_def[MIE_NUM_SLICES] = { 0.5, 0.5, 0.5, 0.5 };
const ISPC::ParamDefArray<double> ISPC::ModuleMIE::MIE_CEXTENT_S(
    "MIE_CEXTENT", 0.0039, 4.0, mie_cextent_def, MIE_NUM_SLICES);

// 1 value per MC
const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_CASPECT_S(
    "MIE_CASPECT", -2.0, 2.0, 0.0);

// 1 value per MC
const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_CROTATION_S(
    "MIE_CROTATION", -1.0, 1.0, 0.0);

// 1 value per MC
const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_OUT_BRIGHTNESS_S(
    "MIE_OUT_BRIGHTNESS", -1.0, 1.0, 0.0);

// 1 value per MC
const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_OUT_CONTRAST_S(
    "MIE_OUT_CONTRAST", 0.0, 4.0, 1.0);

// 1 value per MC
const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_OUT_SATURATION_S(
    "MIE_OUT_SATURATION", 0.0, 4.0, 1.0);

// 1 value per MC
const ISPC::ParamDef<double> ISPC::ModuleMIE::MIE_OUT_HUE_S(
    "MIE_OUT_HUE", -1.0, 1.0, 0.0);

ISPC::ParameterGroup ISPC::ModuleMIE::GetGroup()
{
    unsigned int mc;
    std::ostringstream paramname;
    ParameterGroup group;

    group.header = "// Image Enhancer parameters";

    group.parameters.insert(MIE_BLC.name);
    group.parameters.insert(MIE_MEMORY_COLOURS.name);

    for (mc = 0 ; mc < MIE_MEMORY_COLOURS.max ; mc++)
    {
        paramname.str("");
        paramname << MIE_ENABLED_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_YMIN_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_YMAX_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_CCENTER_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_YGAINS_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_CEXTENT_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_CASPECT_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_CROTATION_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_OUT_BRIGHTNESS_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_OUT_CONTRAST_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_OUT_SATURATION_S.name << "_" << mc;
        group.parameters.insert(paramname.str());

        paramname.str("");
        paramname << MIE_OUT_HUE_S.name << "_" << mc;
        group.parameters.insert(paramname.str());
    }

    return group;
}

void ISPC::ModuleMIE::calculateMIEExtentParameters(const double aspect,
    const double *extent, int &kb, int &kr, double *scale)
{
    /* these are equivalent to "classic" gaussian standard deviation
     * parameters. large sigma value = great extension (wider gaussian) */
    double sigma_b[MIE_NUM_SLICES];
    double sigma_r[MIE_NUM_SLICES];
    double err;
    double min_err[MIE_NUM_SLICES];
    double sum_err = 50000;  // current error
    double opt_err = 50000;  // best error
    int i, k, s, slice;

    /* target_sigma  is the sigma (sigma_b, sigma_r, depending on the
     * aspect ratio, hence the pointer) to be optimised. this pointer is a
     * convenience pointer to avoid writing the code two times */
    double *target_sigma;
    double target_max;  // stores the maximum value for the target_sigma
    int chosen_s[MIE_NUM_SLICES];
    int chosen_k = 0;  // ancillary variables for optimization
    double minCalcSigma;

    int max_val_int;

    const int MAX_SCALE_INT =
        (1 << (MIE_MC_GAUSS_S0_INT + MIE_MC_GAUSS_S0_FRAC)) - 1;
    const int MAX_MIE_MC_GAUSS_S0 =
        (1 << (MIE_MC_GAUSS_S0_INT + MIE_MC_GAUSS_S0_FRAC)) - 1;
    const int MAX_MIE_MC_GAUSS_KB =
        (1 << (MIE_MC_GAUSS_KR_INT + MIE_MC_GAUSS_KR_FRAC)) - 1;
    /* this is the minimum sigma value reachable by K and S. The formula
     * is sigma = MAX_S/(K*S) so MIN_SIGMA = MAX_S/(MAX_K*MAX_S) = 1/MAX_K */
    const double minSigma = 1.0 / (MAX_MIE_MC_GAUSS_KB);

    // this is the temporary minimum sigma value
    minCalcSigma = 2.0*(MAX_MIE_MC_GAUSS_S0);

    for (i = 0; i < MIE_NUM_SLICES; i++)
    {
        /* positive aspect means sigma_b > sigma_r => the gaussian is
         * more extended on the b-axis */
#ifdef USE_MATH_NEON
		sigma_b[i] = extent[i] * (double)sqrtf_neon((float)powf_neon(2.0f, (float)aspect));
        sigma_r[i] = extent[i] * (double)sqrtf_neon((float)powf_neon(2.0f, (float)-aspect));
#else
		sigma_b[i] = extent[i] * sqrt(pow(2.0, aspect));
        sigma_r[i] = extent[i] * sqrt(pow(2.0, -aspect));
#endif

        minCalcSigma = minCalcSigma > sigma_b[i] ? sigma_b[i] : minCalcSigma;
        minCalcSigma = minCalcSigma > sigma_r[i] ? sigma_r[i] : minCalcSigma;
    }

    /* renormalize sigma. if any of the sigma is < minSigma, everything
     * should be scaled accordingly */
    if (minCalcSigma < minSigma)
    {
        for (i = 0; i < MIE_NUM_SLICES; i++)
        {
            sigma_b[i] = minSigma*sigma_b[i] / minCalcSigma;
            sigma_r[i] = minSigma*sigma_r[i] / minCalcSigma;
        }
    }

    // decide which axis to optimize
    if (aspect >= 0)
    {
        /* if aspect >= 0, sigma_b is greater (or equal) than sigma_r.
         * Therefore Kb is <= Kr. */
        target_sigma = sigma_b;
#ifdef USE_MATH_NEON
		target_max = powf_neon(2.0, -aspect);
#else
		target_max = pow(2.0, -aspect);
#endif
        /* if we optimize kb in [0...pow(2.0, -aspect)], then we are
         * sure that kr is in [0 ... 1] */
    }
    else
    {
        target_sigma = sigma_r;
#ifdef USE_MATH_NEON
		target_max = (double)powf_neon(2.0f, (float)aspect);
#else
		target_max = pow(2.0, aspect);
#endif
    }

    /* convert maxima into their integer equivalent using the target
     * registers (so to have the same granularity) */
    max_val_int =
        static_cast<int>(floor(target_max
        * (1 << (MIE_MC_GAUSS_KR_INT + MIE_MC_GAUSS_KR_FRAC)))) - 1;
    /* used to be 1<<MIE_MC_GAUSS_KR_INT+MIE_MC_GAUSS_KR_FRAC
     * (i.e. without parenthesis on the +) */

    /* optimization: for each possible value k, we find the optima scale s
     * for each slice. the combination of k and scales s that holds the
     * smallest total error is the chosen one
     * this optimization cycle has the same k granularity as the target
     * register */
    for (k = 0; k < max_val_int; k++)
    {
        /*for (slice = 0; slice < MIE_NUM_SLICES; slice ++)
        min_err[slice] = 500000000.0; // very large numbers*/

        /* given k in [0... max_val_int - 1], find the best scales s for
         * each slice */
        for (slice = 0; slice < MIE_NUM_SLICES; slice++)
        {
            min_err[slice] = fabs((0.0 - static_cast<double>(MAX_SCALE_INT)
                / target_sigma[slice]));
            chosen_s[slice] = 0;
            for (s = 1; s < MAX_SCALE_INT; s++)
            {
                err = fabs((static_cast<double>(k*s)
                    - static_cast<double>(MAX_SCALE_INT)
                    / target_sigma[slice]));
                /* MAX_SCALE_INT normalization. the formula is
                 * 1/target_sigma  = (K*SCALE) / MAX_SCALE_INT
                 * = ((K*SCALE) >> MIE_SCL_FRAC)
                 * (see implementation document) */
                if (err < min_err[slice])
                {
                    min_err[slice] = err;
                    chosen_s[slice] = s;
                }
            }
        }

        sum_err = 0;
        for (slice = 0; slice < MIE_NUM_SLICES; slice++)
        {
            sum_err += min_err[slice];
        }

        /* if the current k and SCALE combination is better than the
        * previous one, then chose them as chosen_k */
        if (sum_err < opt_err || 0 == k)
        {
            opt_err = sum_err;

            for (i = 0; i < MIE_NUM_SLICES; i++)
            {
                /* this is directly mapped into the output.
                 * The best scale associated to the current k value,
                 * is the scale. */
                scale[i] = static_cast<double>(chosen_s[i]);
            }

            chosen_k = k;
        }
        else
        {
            chosen_k = 0;
            /* static function cannot use MOD_WARNING as this->logName
             * does not exists
             * MOD_LOG_WARNING("Using default chosen_k value (%d)\n",
             * chosen_k); */
        }
    }

    if (aspect >= 0)
    {
        kb = chosen_k;
#ifdef USE_MATH_NEON
		kr = static_cast<int>(floor((float)chosen_k*powf_neon(2.0f, (float)aspect)));
#else
		kr = static_cast<int>(floor(chosen_k*pow(2.0, aspect)));
#endif
    }
    else
    {
        kr = chosen_k;
#ifdef USE_MATH_NEON
		kb = static_cast<int>(floor((float)chosen_k*powf_neon(2.0f, (float)-aspect)));
#else
		kb = static_cast<int>(floor(chosen_k*pow(2.0, -aspect)));
#endif
    }
}

ISPC::ModuleMIE::ModuleMIE(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

void checkDeprecated(const ISPC::ParameterList &parameters)
{
    static std::list<std::string> deprecated;

    if (0 == deprecated.size())
    {
        deprecated.push_back("MIE_MC_ON");
        deprecated.push_back("MIE_MC_YMIN");
        deprecated.push_back("MIE_MC_YMAX");
        deprecated.push_back("MIE_MC_YGAIN_1");
        deprecated.push_back("MIE_MC_YGAIN_2");
        deprecated.push_back("MIE_MC_YGAIN_3");
        deprecated.push_back("MIE_MC_YGAIN_4");
        deprecated.push_back("MIE_MC_CB0");
        deprecated.push_back("MIE_MC_CR0");
        deprecated.push_back("MIE_MC_CEXTENT_1");
        deprecated.push_back("MIE_MC_CEXTENT_2");
        deprecated.push_back("MIE_MC_CEXTENT_3");
        deprecated.push_back("MIE_MC_CEXTENT_4");
        deprecated.push_back("MIE_MC_CASPECT");
        deprecated.push_back("MIE_MC_CROTATION");
        deprecated.push_back("MIE_MC_BRIGHTNESS");
        deprecated.push_back("MIE_MC_CONTRAST");
        deprecated.push_back("MIE_MC_SATURATION");
        deprecated.push_back("MIE_MC_HUE");
    }

    std::list<std::string>::const_iterator it = deprecated.begin();
    while (it != deprecated.end())
    {
        if (parameters.exists(*it))
        {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_WARNING("%s is deprecated! Update your MIE settings.\n",
                (*it).c_str());
#endif //INFOTM_ENABLE_WARNING_DEBUG
        }
        it++;
    }
}

IMG_RESULT ISPC::ModuleMIE::load(const ParameterList &parameters)
{
    std::ostringstream paramname;
    int mc, s, numMem, numEnabled;

    // Generic
    fBlackLevel = parameters.getParameter(MIE_BLC);

    checkDeprecated(parameters);

    numMem = parameters.getParameter(MIE_MEMORY_COLOURS);

    /* the number of memory colours in the parameter file can be bigger than
     * the number of memory colours in HW therefore we use numEnabled to
     * store the memory colour id.
     *
     * example: 4 MC in param (0 enabled, 1 disabled, 2 enabled, 3 disabled):
     * will be loaded HW MC0 = param MC0, HW MC1 = param MC2, HW MC2 disabled
     */
    numEnabled = 0;  // mc used to store the parameters
    for (mc = 0; mc < MIE_NUM_MEMCOLOURS; mc++)
    {
        // first assumes that all MC are disabled
        bEnabled[mc] = IMG_FALSE;  // MIE_ENABLED_S.def

        // load all other defaults because if all are disabled defaults
        // will not be loaded
        aCbCenter[mc] = MIE_CCENTER_S.def[0];
        aCrCenter[mc] = MIE_CCENTER_S.def[1];

        for (s = 0; s < MIE_NUM_SLICES; s++)
        {
            aYGain[mc][s] = MIE_YGAINS_S.def[s];
            aCExtent[mc][s] = MIE_CEXTENT_S.def[s];
        }
        aYMin[mc] = MIE_YMIN_S.def;
        aYMax[mc] = MIE_YMAX_S.def;
        aCAspect[mc] = MIE_CASPECT_S.def;
        aCRotation[mc] = MIE_CROTATION_S.def;
        aOutBrightness[mc] = MIE_OUT_BRIGHTNESS_S.def;
        aOutContrast[mc] = MIE_OUT_CONTRAST_S.def;
        aOutStaturation[mc] = MIE_OUT_SATURATION_S.def;
        aOutHue[mc] = MIE_OUT_HUE_S.def;
    }
    // for each memory colour in the parameters
    for (mc = 0; mc < numMem; mc++)
    {
        bool enable = false;

        paramname.str("");
        paramname << MIE_ENABLED_S.name << "_" << mc;
        enable = parameters.getParameter<bool>(paramname.str(), 0,
            MIE_ENABLED_S.def);

        if (enable && numEnabled < MIE_NUM_MEMCOLOURS)
        {
            bEnabled[numEnabled] = IMG_TRUE;
        }
        else
        {
            /* if enable is true it means we already loaded all that the hw
             * can cope with */
            if (enable) numEnabled++;
            continue;  // try next one
        }

        // now we store the result in HW memory colour numEnabled
        // and load it from parameter memory clour mc

        paramname.str("");
        paramname << MIE_CCENTER_S.name << "_" << mc;
        s = 0;  // cb
        aCbCenter[numEnabled] = parameters.getParameter<double>(
            paramname.str(), s,
            MIE_CCENTER_S.min, MIE_CCENTER_S.max, MIE_CCENTER_S.def[s]);
        s = 1;  // cr
        aCrCenter[numEnabled] = parameters.getParameter<double>(
            paramname.str(), s,
            MIE_CCENTER_S.min, MIE_CCENTER_S.max, MIE_CCENTER_S.def[s]);

        paramname.str("");
        paramname << MIE_YGAINS_S.name << "_" << mc;
        for (s = 0; s < MIE_NUM_SLICES; s++)
        {
            aYGain[numEnabled][s] = parameters.getParameter<double>(
                paramname.str(), s,
                MIE_YGAINS_S.min, MIE_YGAINS_S.max, MIE_YGAINS_S.def[s]);
        }

        paramname.str("");
        paramname << MIE_CEXTENT_S.name << "_" << mc;
        for (s = 0; s < MIE_NUM_SLICES; s++)
        {
            aCExtent[numEnabled][s] = parameters.getParameter<double>(
                paramname.str(), s,
                MIE_CEXTENT_S.min, MIE_CEXTENT_S.max, MIE_CEXTENT_S.def[s]);
        }

        paramname.str("");
        paramname << MIE_YMIN_S.name << "_" << mc;
        aYMin[numEnabled] =
            parameters.getParameter<double>(paramname.str(), 0,
            MIE_YMIN_S.min, MIE_YMIN_S.max, MIE_YMIN_S.def);

        paramname.str("");
        paramname << MIE_YMAX_S.name << "_" << mc;
        aYMax[numEnabled] = parameters.getParameter<double>(
            paramname.str(), 0,
            MIE_YMAX_S.min, MIE_YMAX_S.max, MIE_YMAX_S.def);

        paramname.str("");
        paramname << MIE_CASPECT_S.name << "_" << mc;
        aCAspect[numEnabled] = parameters.getParameter<double>(
            paramname.str(), 0,
            MIE_CASPECT_S.min, MIE_CASPECT_S.max, MIE_CASPECT_S.def);

        paramname.str("");
        paramname << MIE_CROTATION_S.name << "_" << mc;
        aCRotation[numEnabled] = parameters.getParameter<double>(
            paramname.str(), 0,
            MIE_CROTATION_S.min, MIE_CROTATION_S.max, MIE_CROTATION_S.def);

        // output parameters
        paramname.str("");
        paramname << MIE_OUT_BRIGHTNESS_S.name << "_" << mc;
        aOutBrightness[numEnabled] = parameters.getParameter<double>(
            paramname.str(), 0,
            MIE_OUT_BRIGHTNESS_S.min, MIE_OUT_BRIGHTNESS_S.max,
            MIE_OUT_BRIGHTNESS_S.def);

        paramname.str("");
        paramname << MIE_OUT_CONTRAST_S.name << "_" << mc;
        aOutContrast[numEnabled] = parameters.getParameter<double>(
            paramname.str(), 0,
            MIE_OUT_CONTRAST_S.min, MIE_OUT_CONTRAST_S.max,
            MIE_OUT_CONTRAST_S.def);

        // parameters.exists("MIE_MC_CONTRAST_MID")  // not used

        paramname.str("");
        paramname << MIE_OUT_SATURATION_S.name << "_" << mc;
        aOutStaturation[numEnabled] = parameters.getParameter<double>(
            paramname.str(), 0,
            MIE_OUT_SATURATION_S.min, MIE_OUT_SATURATION_S.max,
            MIE_OUT_SATURATION_S.def);

        paramname.str("");
        paramname << MIE_OUT_HUE_S.name << "_" << mc;
        aOutHue[numEnabled] = parameters.getParameter<double>(
            paramname.str(), 0,
            MIE_OUT_HUE_S.min, MIE_OUT_HUE_S.max, MIE_OUT_HUE_S.def);

        numEnabled++;
    }

    if (numEnabled > MIE_NUM_MEMCOLOURS)
    {
        MOD_LOG_WARNING("Number of enabled memory colours in parameters is "\
            "%d while HW only supports %d - the first %d were enabled "\
            "the other ignored.\n",
            numEnabled, MIE_NUM_MEMCOLOURS, MIE_NUM_MEMCOLOURS);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleMIE::save(ParameterList &parameters, SaveType t) const
{
    std::ostringstream paramname;
    std::vector<std::string> values;
    int mc, s;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleMIE::GetGroup();
    }

    parameters.addGroup("ModuleMIE", group);

    switch (t)
    {
    case SAVE_VAL:
        // Generic
        parameters.addParameter(Parameter(MIE_BLC.name,
            toString(this->fBlackLevel)));
        parameters.addParameter(Parameter(MIE_MEMORY_COLOURS.name,
            toString(MIE_NUM_MEMCOLOURS)));

        // memory colour
        for (mc = 0 ; mc < MIE_NUM_MEMCOLOURS ; mc++)
        {
            paramname.str("");
            paramname << MIE_ENABLED_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(bEnabled[mc])));

            /*if (!bEnabled[mc])
            {
                continue;  // no need to save other parameters
            }*/

            paramname.str("");
            paramname << MIE_YMIN_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(aYMin[mc])));

            paramname.str("");
            paramname << MIE_YMAX_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(aYMax[mc])));

            paramname.str("");
            paramname << MIE_CCENTER_S.name << "_" << mc;
            values.clear();
            values.push_back(toString(aCbCenter[mc]));
            values.push_back(toString(aCrCenter[mc]));
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_YGAINS_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                values.push_back(toString(aYGain[mc][s]));
            }
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_CEXTENT_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                values.push_back(toString(aCExtent[mc][s]));
            }
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_CASPECT_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(aCAspect[mc])));

            paramname.str("");
            paramname << MIE_CROTATION_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(aCRotation[mc])));

            paramname.str("");
            paramname << MIE_OUT_BRIGHTNESS_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(aOutBrightness[mc])));

            paramname.str("");
            paramname << MIE_OUT_CONTRAST_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(aOutContrast[mc])));

            paramname.str("");
            paramname << MIE_OUT_SATURATION_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(aOutStaturation[mc])));

            paramname.str("");
            paramname << MIE_OUT_HUE_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(aOutHue[mc])));
        }
        break;

    case SAVE_MIN:
        // Generic
        parameters.addParameter(Parameter(MIE_BLC.name,
            toString(MIE_BLC.min)));
        parameters.addParameter(Parameter(MIE_MEMORY_COLOURS.name,
            toString(MIE_MEMORY_COLOURS.min)));

        // memory colour
        for (mc = 0; mc < 1; mc++)
        {
            paramname.str("");
            paramname << MIE_ENABLED_S.name << "_" << mc;
            // boolean do not have min
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_ENABLED_S.def)));

            /*if (!bEnabled[mc])
            {
            continue;  // no need to save other parameters
            }*/

            paramname.str("");
            paramname << MIE_YMIN_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_YMIN_S.min)));

            paramname.str("");
            paramname << MIE_YMAX_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_YMAX_S.min)));

            paramname.str("");
            paramname << MIE_CCENTER_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < 2; s++)
            {
                values.push_back(toString(MIE_CCENTER_S.min));
            }
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_YGAINS_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                values.push_back(toString(MIE_YGAINS_S.min));
            }
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_CEXTENT_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                values.push_back(toString(MIE_CEXTENT_S.min));
            }
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_CASPECT_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_CASPECT_S.min)));

            paramname.str("");
            paramname << MIE_CROTATION_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_CROTATION_S.min)));

            paramname.str("");
            paramname << MIE_OUT_BRIGHTNESS_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_BRIGHTNESS_S.min)));

            paramname.str("");
            paramname << MIE_OUT_CONTRAST_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_CONTRAST_S.min)));

            paramname.str("");
            paramname << MIE_OUT_SATURATION_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_SATURATION_S.min)));

            paramname.str("");
            paramname << MIE_OUT_HUE_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_HUE_S.min)));
        }
        break;

    case SAVE_MAX:
        // Generic
        parameters.addParameter(Parameter(MIE_BLC.name,
            toString(MIE_BLC.min)));
        parameters.addParameter(Parameter(MIE_MEMORY_COLOURS.name,
            toString(MIE_MEMORY_COLOURS.min)));

        // memory colour
        for (mc = 0; mc < 1; mc++)
        {
            paramname.str("");
            paramname << MIE_ENABLED_S.name << "_" << mc;
            // boolean do not have max
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_ENABLED_S.def)));

            /*if (!bEnabled[mc])
            {
            continue;  // no need to save other parameters
            }*/

            paramname.str("");
            paramname << MIE_YMIN_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_YMIN_S.max)));

            paramname.str("");
            paramname << MIE_YMAX_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_YMAX_S.max)));

            paramname.str("");
            paramname << MIE_CCENTER_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < 2; s++)
            {
                values.push_back(toString(MIE_CCENTER_S.max));
            }
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_YGAINS_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                values.push_back(toString(MIE_YGAINS_S.max));
            }
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_CEXTENT_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                values.push_back(toString(MIE_CEXTENT_S.max));
            }
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_CASPECT_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_CASPECT_S.max)));

            paramname.str("");
            paramname << MIE_CROTATION_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_CROTATION_S.max)));

            paramname.str("");
            paramname << MIE_OUT_BRIGHTNESS_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_BRIGHTNESS_S.max)));

            paramname.str("");
            paramname << MIE_OUT_CONTRAST_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_CONTRAST_S.max)));

            paramname.str("");
            paramname << MIE_OUT_SATURATION_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_SATURATION_S.max)));

            paramname.str("");
            paramname << MIE_OUT_HUE_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_HUE_S.max)));
        }
        break;

    case SAVE_DEF:
        // Generic
        parameters.addParameter(Parameter(MIE_BLC.name,
            toString(MIE_BLC.def)
            + "// " + getParameterInfo(MIE_BLC)));
        parameters.addParameter(Parameter(MIE_MEMORY_COLOURS.name,
            toString(MIE_MEMORY_COLOURS.def)
            + "// " + getParameterInfo(MIE_MEMORY_COLOURS)));

        // memory colour
        for (mc = 0; mc < 1; mc++)
        {
            paramname.str("");
            paramname << MIE_ENABLED_S.name << "_" << mc;
            // boolean do not have max
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_ENABLED_S.def)
                + "// " + getParameterInfo(MIE_ENABLED_S)));

            /*if (!bEnabled[mc])
            {
            continue;  // no need to save other parameters
            }*/

            paramname.str("");
            paramname << MIE_YMIN_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_YMIN_S.def)
                + "// " + getParameterInfo(MIE_YMIN_S)));

            paramname.str("");
            paramname << MIE_YMAX_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_YMAX_S.def)
                + "// " + getParameterInfo(MIE_YMAX_S)));

            paramname.str("");
            paramname << MIE_CCENTER_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < 2; s++)
            {
                values.push_back(toString(MIE_CCENTER_S.def[s]));
            }
            values.push_back("// " + getParameterInfo(MIE_CCENTER_S));
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_YGAINS_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                values.push_back(toString(MIE_YGAINS_S.def[s]));
            }
            values.push_back("// " + getParameterInfo(MIE_YGAINS_S));
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_CEXTENT_S.name << "_" << mc;
            values.clear();
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                values.push_back(toString(MIE_CEXTENT_S.def[s]));
            }
            values.push_back("// " + getParameterInfo(MIE_CEXTENT_S));
            parameters.addParameter(Parameter(paramname.str(), values));

            paramname.str("");
            paramname << MIE_CASPECT_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_CASPECT_S.def)
                + "// " + getParameterInfo(MIE_CASPECT_S)));

            paramname.str("");
            paramname << MIE_CROTATION_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_CROTATION_S.def)
                + "// " + getParameterInfo(MIE_CROTATION_S)));

            paramname.str("");
            paramname << MIE_OUT_BRIGHTNESS_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_BRIGHTNESS_S.def)
                + "// " + getParameterInfo(MIE_OUT_BRIGHTNESS_S)));

            paramname.str("");
            paramname << MIE_OUT_CONTRAST_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_CONTRAST_S.def)
                + "// " + getParameterInfo(MIE_OUT_CONTRAST_S)));

            paramname.str("");
            paramname << MIE_OUT_SATURATION_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_SATURATION_S.def)
                + "// " + getParameterInfo(MIE_OUT_SATURATION_S)));

            paramname.str("");
            paramname << MIE_OUT_HUE_S.name << "_" << mc;
            parameters.addParameter(Parameter(paramname.str(),
                toString(MIE_OUT_HUE_S.def)
                + "// " + getParameterInfo(MIE_OUT_HUE_S)));
        }
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleMIE::setup()
{
    IMG_INT16 i;
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
    MC_MIE &MC_MIE = pMCPipeline->sMIE;
    pOut = pipeline->getModule<const ModuleOUT>();

    // HW expets YVU but we want YUV
    bSwap = (YUV_420_PL12_8 == pOut->encoderType
       || YUV_420_PL12_10 == pOut->encoderType
       || YUV_422_PL12_8 == pOut->encoderType
       || YUV_420_PL12_10 == pOut->encoderType);

    //----------
    // double multip;

    // const values are defined values for the parameters
    /* this is the pipeline offset, accounting for the effective dynamic
     * range used. e.g. for a pipeline [-2048:2047] effective range [B:W]
     * level is [-1024:1023] so pipelineOffset = ((-1024)/-2048) = 0.5 */
    // mie upper band of the Y range at its input (0..1)
    static const double y_top_range = 0.75,
        y_bot_range = 0.25;
    // mie upper band of the CbCr range at its input (-0.5..0.5)
    static const double c_top_range = -0.25,
        c_bot_range = 0.25;

    // COMMON

    // black level
    MC_MIE.fBlackLevel = (this->fBlackLevel + y_bot_range)
        *(1 << (MIE_BLACK_LEVEL_INT));

    // MEMORY COLOUR

    // const values are defined values for the parameters
    static const double rot_min =
        -static_cast<double>(1 << MIE_MC_ROT00_FRAC);  // -1.0;
    static const double rot_max =
        static_cast<double>(1 << MIE_MC_ROT00_FRAC) - 1;  // 1.0
    static const double rot_A = (rot_max - rot_min) / 2;
    static const double rot_B = (rot_max + rot_min) / 2;
    // also knowm as Yoffset in registers
    static const double bright_min = -1.0;
    // aslo known as Yoffset in registers
    static const double bright_max = 1.0;

    //  use M_PI
    static const double PI = 3.14159;

    double doub = 0.0, ymin = 0.0, ymax = 0.0;
    int temp = 0;
    int kb, kr;
    double scale[MIE_NUM_SLICES];
    int mc, s;  // iteration on memory colours and slices
    double extent[MIE_NUM_SLICES], aspect;

    MC_MIE.bMemoryColour = IMG_FALSE;

    for (mc = 0; mc < MIE_NUM_MEMCOLOURS; mc ++)
    {
        MC_MIE.bMemoryColour |= bEnabled[mc] ? IMG_TRUE : IMG_FALSE;

        // double contrastMidpoint;
        //--------------------------------------------------------------
        /* memory colours - output curves.
         * ref: Keith Jack, "Video Demystified" 5th ed. pgg. 198-199
         * contrastMidpoint = (y_top_range - y_bot_range)
         * *setup->MC_CONTRAST_MID[mc] + y_bot_range; */

        // contrast mid point affects brightness
        /* MC_MIE.aYOffset[mc] = contrastMidpoint
         * *(1.0 - setup->MC_CONTRAST[mc])/(y_top_range - y_bot_range)
         * + setup->MC_BRIGHTNESS[mc]; */
        MC_MIE.aYOffset[mc] = this->aOutBrightness[mc];
        // normalise Yoffset in range -1..1 to 0..1
        MC_MIE.aYOffset[mc] = (MC_MIE.aYOffset[mc] - bright_min)
            / (bright_max - bright_min);

        // contrast
        MC_MIE.aYScale[mc]  = this->aOutContrast[mc];

        // saturation
        MC_MIE.aCScale[mc] = this->aOutStaturation[mc]*this->aOutContrast[mc];

        // hue
        /* divide by precision because precision is applied in MC
         * but CSIM reference gives this value strait to the register */
#ifdef USE_MATH_NEON
		MC_MIE.aRot[mc * 2 + 0] =
		ceil((float)rot_A*cosf_neon((float)this->aOutHue[mc] * (float)PI) + (float)rot_B);
		MC_MIE.aRot[mc * 2 + 0] = MC_MIE.aRot[mc * 2 + 0]
		/ static_cast<double>(1 << MIE_MC_ROT00_FRAC);
		MC_MIE.aRot[mc * 2 + 1] =
		ceil((float)rot_A*sinf_neon((float)this->aOutHue[mc] * (float)PI) + (float)rot_B);
		MC_MIE.aRot[mc * 2 + 1] = MC_MIE.aRot[mc * 2 + 1]
		/ static_cast<double>(1 << MIE_MC_ROT00_FRAC);
#else
		MC_MIE.aRot[mc * 2 + 0] =
		ceil(rot_A*cos(this->aOutHue[mc] * PI) + rot_B);
		MC_MIE.aRot[mc * 2 + 0] = MC_MIE.aRot[mc * 2 + 0]
		/ static_cast<double>(1 << MIE_MC_ROT00_FRAC);
		MC_MIE.aRot[mc * 2 + 1] =
		ceil(rot_A*sin(this->aOutHue[mc] * PI) + rot_B);
		MC_MIE.aRot[mc * 2 + 1] = MC_MIE.aRot[mc * 2 + 1]
		/ static_cast<double>(1 << MIE_MC_ROT00_FRAC);
#endif

        //--------------------------------------------------------------
        // memory colours - colour space definition.
        if (!bEnabled[mc])
        {
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                MC_MIE.aGaussGain[mc*MIE_NUM_SLICES + s] = 0.0;
            }
        }
        else
        {
            for (s = 0; s < MIE_NUM_SLICES; s++)
            {
                MC_MIE.aGaussGain[mc*MIE_NUM_SLICES + s] =
                    this->aYGain[mc][s];
            }
        }

        // warning assumes MC_YMIN[mc] is 0..1
        ymin = this->aYMin[mc] + this->fBlackLevel;
        // as a normalised range of the register
        MC_MIE.aGaussMinY[mc] = (y_top_range-y_bot_range)*ymin + y_bot_range;

        /* y-max i.e. y-value of the top slice. this does get mapped into the
         * y-scale register
         * warning assumes MC_YMAX[mc] is 0..1 */
        // (setup function in CSIM also has +0.0000000000000001)
        ymax = (y_top_range-y_bot_range)*this->aYMax[mc] + y_bot_range;

        if (ymax > ymin)
        {
            MC_MIE.aGaussScaleY[mc] = (1.0 - MC_MIE.aGaussMinY[mc]) /
                (ymax - MC_MIE.aGaussMinY[mc]);
        }
        else
        {
            // max value
            MC_MIE.aGaussScaleY[mc] =
                static_cast<double>(1 << (MIE_MC_GAUSS_YSCALE_INT));
        }

        // gaussian colour distribution, cb-cr centroid

        // converts the Cb/Cr center to the register range
        /* converts the -0.5..0.5 of the parameter to 0..1 - do not use the
         * CbCr rescale like the setup function */
        MC_MIE.aGaussCB[mc] = this->aCbCenter[mc] + 0.5;
        /* converts the -0.5..0.5 of the parameter to 0..1 - do not use the
         * CbCr rescale like the setup function */
        MC_MIE.aGaussCR[mc] = this->aCrCenter[mc] + 0.5;

        // gaussian colour distribution, cb-cr plane rotation
        /* divide by precision because precision is applied in MC but CSIM
         * reference gives this value strait to the register */
#ifdef USE_MATH_NEON
		MC_MIE.aGaussRot[mc * 2] =
			ceil((float)rot_A*cosf_neon((float)this->aCRotation[mc] * (float)PI) + (float)rot_B);
		MC_MIE.aGaussRot[mc * 2] = MC_MIE.aGaussRot[mc * 2]
			/ static_cast<double>(1 << MIE_MC_GAUSS_R00_FRAC);
		// used to have ceil() before multiplication
		MC_MIE.aGaussRot[mc * 2 + 1] =
			ceil((float)rot_A*sinf_neon((float)this->aCRotation[mc] * (float)PI) + (float)rot_B);
#else
		MC_MIE.aGaussRot[mc * 2] =
			ceil(rot_A*cos(this->aCRotation[mc] * PI) + rot_B);
		MC_MIE.aGaussRot[mc * 2] = MC_MIE.aGaussRot[mc * 2]
			/ static_cast<double>(1 << MIE_MC_GAUSS_R00_FRAC);
		// used to have ceil() before multiplication
		MC_MIE.aGaussRot[mc * 2 + 1] =
			ceil(rot_A*sin(this->aCRotation[mc] * PI) + rot_B);
#endif
        MC_MIE.aGaussRot[mc * 2 + 1] = MC_MIE.aGaussRot[mc * 2 + 1]
            / static_cast<double>(1 << MIE_MC_GAUSS_R01_FRAC);

        // gaussian colour distribution, Kb, Kr, s0, s1.
        // c-extent: this parameter is not directly interfaced to the registers
        for (s = 0 ; s < MIE_NUM_SLICES ; s++)
        {
            extent[s] = this->aCExtent[mc][s];
        }
        aspect = this->aCAspect[mc];

        // convert aspect and extent to Kb, Kr, scale
        calculateMIEExtentParameters(aspect, extent, kb, kr, scale);

        MC_MIE.aGaussKB[mc] = kb;
        MC_MIE.aGaussKR[mc] = kr;

        for (i  = 0; i < MIE_NUM_SLICES; i ++)
        {
            MC_MIE.aGaussScale[mc*MIE_NUM_SLICES+i]= scale[i];
        }
    }

    // if we have a YUV input instead of YVU
    if (bSwap)
    {
        MC_FLOAT tmp = 0;
        for (i = 0 ; i < MIE_NUM_SLICES ; i++)
        {
            // MC_MIE.aRot[2*i+0] // no change
            MC_MIE.aRot[2 * i + 1] = MC_MIE.aRot[2 * i + 1] * (-1.0);

            tmp = MC_MIE.aGaussCB[i];
            MC_MIE.aGaussCB[i] = MC_MIE.aGaussCR[i];
            MC_MIE.aGaussCR[i] = tmp;

            // MC_MIE.aGaussRot[2*i+0] no change
            MC_MIE.aGaussRot[2 * i + 1] = MC_MIE.aGaussRot[2 * i + 1] * (-1.0);

            tmp = MC_MIE.aGaussKB[i];
            MC_MIE.aGaussKB[i] = MC_MIE.aGaussKR[i];
            MC_MIE.aGaussKR[i] = tmp;
        }
    }

    this->setupFlag = true;
    return IMG_SUCCESS;
}
