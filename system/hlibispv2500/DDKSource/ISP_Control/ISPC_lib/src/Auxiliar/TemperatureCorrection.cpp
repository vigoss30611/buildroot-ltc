/**
*******************************************************************************
 @file TemperatureCorrection.cpp

 @brief ISPC::TemperatureCorrection implementation

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
#include "ispc/TemperatureCorrection.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_TEMPCTRL"

#include <sstream>
#include <string>
#include <algorithm>  // sort
#include <vector>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "ispc/Module.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ModuleWBC.h"
#include "ispc/ControlAWB.h"  // AWB_MAX_TEMP


const ISPC::ParamDef<int> ISPC::TemperatureCorrection::WB_CORRECTIONS(
    "WB_CORRECTIONS", 0, 20, 0);  // there are no real max, 20 is arbitrary

// base for the parameter (nb to load is defined as WB_CORRECTIONS)
const ISPC::ParamDef<double> ISPC::TemperatureCorrection::WB_TEMPERATURE_S(
    "WB_TEMPERATURE", 0.0f, AWB_MAX_TEMP, 0.0f);
const ISPC::ParamDefArray<double> ISPC::TemperatureCorrection::WB_CCM_S(
    "WB_CCM", CCM_MATRIX_MIN, CCM_MATRIX_MAX, CCM_MATRIX_DEF, 9);
const ISPC::ParamDefArray<double> ISPC::TemperatureCorrection::WB_OFFSETS_S(
    "WB_OFFSETS", CCM_OFFSETS_MIN, CCM_OFFSETS_MAX, CCM_OFFSETS_DEF, 3);
const ISPC::ParamDefArray<double> ISPC::TemperatureCorrection::WB_GAINS_S(
    "WB_GAINS", WBC_GAIN_MIN, WBC_GAIN_MAX, WBC_GAIN_DEF, 4);

/**
 * The correlated color temperature for a given RGB colour is calculated by
 * computing the R and B with respect to green and using those ratios as
 * indexes in the correlatedTemperature table.
 * The table indexes ratios rangintg between 0.5 and 2.0 and ratios
 * below/above those limits should be clipped before indexing
 * @brief Lookup table of correlated temperatures corresponding to pairs
 * of R/B gains
 *
 * See @ref correctionGainR
 * See @ref correctionGainB
 */
static const double correlatedTemperature[16][16] = {
    {6278.381560, 6683.886249, 7140.211576, 7655.915587,
     8241.397259, 8909.385272, 9675.583167, 10559.530165,
     11585.763173, 12500.000000, 12500.000000, 12500.000000,
     12500.000000, 12500.000000, 12500.000000, 12500.000000},
    {5952.722653, 6322.264532, 6737.082209, 7204.621336,
     7733.890399, 8335.865269, 9024.019831, 9815.029205,
     10729.712078, 11794.308725, 12500.000000, 12500.000000,
     12500.000000, 12500.000000, 12500.000000, 12500.000000},
    {5652.727614, 5989.925982, 6367.533164, 6792.042185,
     7271.276671, 7814.726865, 8433.987705, 9143.335643,
     9960.496216, 10907.677208, 12012.976620, 12500.000000,
     12500.000000, 12500.000000, 12500.000000, 12500.000000},
    {5376.021566, 5684.079759, 6028.265060, 6414.252409,
     6848.852772, 7340.293152, 7898.579567, 8535.972031,
     9267.612400, 10112.363401, 11093.943169, 12242.479073,
     12500.000000, 12500.000000, 12500.000000, 12500.000000},
    {5120.486350, 5402.245787, 5716.357322, 6067.791483,
     6462.491257, 6907.606201, 7411.794216, 7985.614115,
     8642.041241, 9397.151796, 10271.041355, 11289.072750,
     12483.593970, 12500.000000, 12500.000000, 12500.000000},
    {4884.228771, 5142.215164, 5429.218448, 5749.601052,
     6108.559966, 6512.323056, 6968.400791, 7485.911838,
     8076.008126, 8752.435301, 9532.279674, 10436.975288,
     11493.678815, 12500.000000, 12500.000000, 12500.000000},
    {4665.553252, 4902.016242, 5164.543592, 5456.971493,
     5783.854087, 6150.628576, 6563.825849, 7031.341452,
     7562.787264, 8169.952310, 8867.412766, 9673.348433,
     10610.648659, 11708.429835, 12500.000000, 12500.000000},
    {4462.938224, 4679.885464, 4920.278294, 5187.496436,
     5485.538680, 5819.162132, 6194.058987, 6617.082755,
     7096.540239, 7642.571825, 8267.651673, 8987.252565,
     9820.739715, 10792.587256, 11934.056148, 12500.000000},
    {4275.015691, 4474.242233, 4694.587360, 4939.033948,
     5211.099864, 5514.955717, 5855.573644, 6238.916761,
     6672.182378, 7164.116956, 7725.427831, 8369.326847,
     9112.255999, 9974.867429, 10983.363802, 12171.357077},
    {4100.553484, 4283.667193, 4485.828090, 4709.673294,
     4958.303241, 5235.381511, 5545.260399, 5893.140045,
     6285.271657, 6729.219261, 7234.199828, 7811.529571,
     8475.215579, 9242.748981, 10136.181140, 11183.603160},
    {3938.439827, 4106.883399, 4292.527171, 4497.706390,
     4725.158363, 4978.107301, 5260.370580, 5576.492765,
     5931.915948, 6333.197965, 6788.294381, 7306.926237,
     7901.064407, 8585.574384, 9379.084530, 10305.169890},
    {3787.669875, 3942.739973, 4113.360698, 4301.603214,
     4509.888264, 4741.058460, 4998.468417, 5286.097953,
     5608.695259, 5971.959402, 6382.774871, 6849.515702,
     7382.443538, 7994.234004, 8700.680389, 9521.645584},
    {3647.333939, 3790.197852, 3947.136834, 4119.990571,
     4310.903278, 4522.385413, 4757.390328, 5019.410121,
     5312.596312, 5641.912916, 6013.332166, 6434.086869,
     6912.998769, 7460.909965, 8091.255688, 8820.833397},
    {3516.607178, 3648.317366, 3792.780734, 3951.633704,
     4126.778453, 4320.435718, 4535.210140, 4774.171613,
     5040.957276, 5339.900273, 5676.193560, 6056.100004,
     6487.224195, 6978.867384, 7542.495634, 8192.363963},
    {3394.740538, 3516.247350, 3649.321410, 3795.420327,
     3956.234029, 4133.730041, 4330.209304, 4548.375393,
     4791.420927, 5063.136168, 5368.046526, 5711.588017,
     6100.333012, 6542.283326, 7047.254335, 7627.383653},
    {3281.052787, 3393.215611, 3515.880244, 3650.346725,
     3798.118517, 3960.941411, 4140.851308, 4340.233251,
     4561.894810, 4809.157917, 5085.974610, 5397.073996,
     5748.150354, 6146.105974, 6599.367511, 7118.302216}
};

ISPC::ParameterGroup ISPC::TemperatureCorrection::getGroup()
{
    int i;
    std::ostringstream parameterName;
    ParameterGroup group;

    group.header = "// Temperature Correction parameters (Multi-CCM for AWB)";

    group.parameters.insert(WB_CORRECTIONS.name);

    for (i = 0 ; i < WB_CORRECTIONS.max ; i++)
    {
        parameterName.str("");
        parameterName << WB_TEMPERATURE_S.name << "_" << i;
        group.parameters.insert(parameterName.str());

        parameterName.str("");
        parameterName << WB_CCM_S.name << "_" << i;
        group.parameters.insert(parameterName.str());

        parameterName.str("");
        parameterName << WB_OFFSETS_S.name << "_" << i;
        group.parameters.insert(parameterName.str());

        parameterName.str("");
        parameterName << WB_GAINS_S.name << "_" << i;
        group.parameters.insert(parameterName.str());
    }

    return group;
}

ISPC::TemperatureCorrection::TemperatureCorrection()
{
    std::sort(temperatureCorrections.begin(), temperatureCorrections.end());
}

void ISPC::TemperatureCorrection::addCorrection(
    const ColorCorrection &newCorrection)
{
    temperatureCorrections.push_back(newCorrection);
    std::sort(temperatureCorrections.begin(), temperatureCorrections.end());
}

int ISPC::TemperatureCorrection::size() const
{
    return temperatureCorrections.size();
}

void ISPC::TemperatureCorrection::clearCorrections()
{
    temperatureCorrections.clear();
}

IMG_RESULT ISPC::TemperatureCorrection::loadParameters(
    const ParameterList &parameters)
{
    int i, j, k;
    std::stringstream parameterName;

    if (!parameters.exists(WB_CORRECTIONS.name))
    {
        LOG_WARNING("Unable to load temperature corrections from parameters. "\
            "'%s' not defined\n", WB_CORRECTIONS.name.c_str());
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    else  // ready to load the parameters
    {
        int numCorrections = parameters.getParameter(WB_CORRECTIONS);
        if (numCorrections < 1)
        {
            LOG_ERROR("Invalid number of WB_CORRECTIONS defined (at least "\
                "one correction is required, read: %d)\n", numCorrections);
            return IMG_ERROR_INVALID_PARAMETERS;
        }

        this->clearCorrections();

        // Read every correction
        for (i = 0; i < numCorrections; i++)
        {
            ColorCorrection newCorrection;

            parameterName.str("");
            parameterName << WB_TEMPERATURE_S.name << "_" << i;
            newCorrection.temperature =
                parameters.getParameter<double>(parameterName.str(), 0,
                WB_TEMPERATURE_S.min, WB_TEMPERATURE_S.max,
                WB_TEMPERATURE_S.def);

            parameterName.str("");
            parameterName << WB_CCM_S.name << "_" << i;

            for (j = 0 ; j < 3 ; j++)
            {
                for (k = 0 ; k < 3 ; k++)
                {
                    newCorrection.coefficients[j][k] =
                        parameters.getParameter<double>(parameterName.str(),
                        3*j+k,
                        WB_CCM_S.min, WB_CCM_S.max, WB_CCM_S.def[3*j+k]);
                }
            }

            parameterName.str("");
            parameterName <<WB_OFFSETS_S.name << "_" << i;
            for (j = 0 ; j < 3 ; j++)
            {
                newCorrection.offsets[0][j] =
                    parameters.getParameter<double>(parameterName.str(),
                    j,
                    WB_OFFSETS_S.min, WB_OFFSETS_S.max, WB_OFFSETS_S.def[j]);
            }

            parameterName.str("");
            parameterName <<WB_GAINS_S.name << "_" << i;
            for (j = 0 ; j < 4 ; j++)
            {
                newCorrection.gains[0][j] =
                    parameters.getParameter<double>(parameterName.str(),
                    j,
                    WB_GAINS_S.min, WB_GAINS_S.max, WB_GAINS_S.def[j]);
            }
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_INFO("loaded correction for temperature %.2lf\n",
                newCorrection.temperature);
#endif //INFOTM_ENABLE_WARNING_DEBUG

            addCorrection(newCorrection);
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::TemperatureCorrection::saveParameters(
    ParameterList &parameters, ModuleBase::SaveType t) const
{
    int i, j, k;
    std::vector<std::string> values;
    std::stringstream parameterName;

    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = TemperatureCorrection::getGroup();
    }

    parameters.addGroup("TemperatureCorrection", group);

    switch (t)
    {
    case ModuleBase::SAVE_VAL:
        {
            parameters.addParameter(Parameter(WB_CORRECTIONS.name,
                toString(temperatureCorrections.size())));

            std::vector<ColorCorrection>::const_iterator it;

            i = 0;
            for (it = temperatureCorrections.begin() ;
                it != temperatureCorrections.end() ; it++)
            {
                parameterName.str("");

                parameterName << WB_TEMPERATURE_S.name << "_" << i;
                parameters.addParameter(Parameter(parameterName.str(),
                    toString(it->temperature)));

                parameterName.str("");
                parameterName << WB_CCM_S.name << "_" << i;

                values.clear();
                for (j = 0 ; j < 3 ; j++)
                {
                    for (k = 0 ; k < 3 ; k++)
                    {
                        values.push_back(toString(it->coefficients[j][k]));
                    }
                }
                parameters.addParameter(Parameter(parameterName.str(),
                    values));

                parameterName.str("");
                parameterName << WB_OFFSETS_S.name << "_" << i;
                values.clear();
                for (j = 0 ; j < 3 ; j++)
                {
                    values.push_back(toString(it->offsets[0][j]));
                }
                parameters.addParameter(Parameter(parameterName.str(),
                    values));

                parameterName.str("");
                parameterName << WB_GAINS_S.name << "_" << i;
                values.clear();
                for (j = 0 ; j < 4 ; j++)
                {
                    values.push_back(toString(it->gains[0][j]));
                }
                parameters.addParameter(Parameter(parameterName.str(),
                    values));

                i++;
            }
        }
        break;

    case ModuleBase::SAVE_MIN:
        parameters.addParameter(Parameter(WB_CORRECTIONS.name,
            toString(WB_CORRECTIONS.min)));

        // for ( i = 0 ; i < WB_CORRECTIONS.min ; i++ )
        // do it only once
        i = 0;
        {
            parameterName.str("");

            parameterName << WB_TEMPERATURE_S.name << "_" << i;
            parameters.addParameter(Parameter(parameterName.str(),
                toString(WB_TEMPERATURE_S.min)));

            parameterName.str("");
            parameterName << WB_CCM_S.name << "_" << i;

            values.clear();
            for (j = 0 ; j < 3 ; j++)
            {
                for (k = 0 ; k < 3 ; k++)
                {
                    values.push_back(toString(WB_CCM_S.min));
                }
            }
            parameters.addParameter(Parameter(parameterName.str(), values));

            parameterName.str("");
            parameterName << WB_OFFSETS_S.name << "_" << i;
            values.clear();
            for (j = 0 ; j < 3 ; j++)
            {
                values.push_back(toString(WB_OFFSETS_S.min));
            }
            parameters.addParameter(Parameter(parameterName.str(), values));

            parameterName.str("");
            parameterName << WB_GAINS_S.name << "_" << i;
            values.clear();
            for (j = 0 ; j < 4 ; j++)
            {
                values.push_back(toString(WB_GAINS_S.min));
            }
            parameters.addParameter(Parameter(parameterName.str(), values));

            i++;
        }
        break;

    case ModuleBase::SAVE_MAX:
        parameters.addParameter(Parameter(WB_CORRECTIONS.name,
            toString(WB_CORRECTIONS.max)));

        // for ( i = 0 ; i < WB_CORRECTIONS.min ; i++ )
        // do it only once
        i = 0;
        {
            parameterName.str("");

            parameterName << WB_TEMPERATURE_S.name << "_" << i;
            parameters.addParameter(Parameter(parameterName.str(),
                toString(WB_TEMPERATURE_S.max)));

            parameterName.str("");
            parameterName << WB_CCM_S.name << "_" << i;

            values.clear();
            for (j = 0 ; j < 3 ; j++)
            {
                for (k = 0 ; k < 3 ; k++)
                {
                    values.push_back(toString(WB_CCM_S.max));
                }
            }
            parameters.addParameter(Parameter(parameterName.str(), values));

            parameterName.str("");
            parameterName << WB_OFFSETS_S.name << "_" << i;
            values.clear();
            for (j = 0 ; j < 3 ; j++)
            {
                values.push_back(toString(WB_OFFSETS_S.max));
            }
            parameters.addParameter(Parameter(parameterName.str(), values));

            parameterName.str("");
            parameterName << WB_GAINS_S.name << "_" << i;
            values.clear();
            for (j = 0 ; j < 4 ; j++)
            {
                values.push_back(toString(WB_GAINS_S.max));
            }
            parameters.addParameter(Parameter(parameterName.str(), values));

            i++;
        }
        break;

    case ModuleBase::SAVE_DEF:
        parameters.addParameter(Parameter(WB_CORRECTIONS.name,
            toString(WB_CORRECTIONS.def)
            + " // " + getParameterInfo(WB_CORRECTIONS)));

        // for ( i = 0 ; i < WB_CORRECTIONS.min ; i++ )
        // do it only once
        i = 0;
        {
            parameterName.str("");

            parameterName << WB_TEMPERATURE_S.name << "_" << i;
            parameters.addParameter(Parameter(parameterName.str(),
                toString(WB_TEMPERATURE_S.def)
                + " // " + getParameterInfo(WB_TEMPERATURE_S)));

            parameterName.str("");
            parameterName << WB_CCM_S.name << "_" << i;

            values.clear();
            for (j = 0 ; j < 3 ; j++)
            {
                for (k = 0 ; k < 3 ; k++)
                {
                    values.push_back(toString(WB_CCM_S.def[3 * j + k]));
                }
            }
            values.push_back("// " + getParameterInfo(WB_CCM_S));
            parameters.addParameter(Parameter(parameterName.str(), values));

            parameterName.str("");
            parameterName << WB_OFFSETS_S.name << "_" << i;
            values.clear();
            for (j = 0 ; j < 3 ; j++)
            {
                values.push_back(toString(WB_OFFSETS_S.def[j]));
            }
            values.push_back("// " + getParameterInfo(WB_OFFSETS_S));
            parameters.addParameter(Parameter(parameterName.str(), values));

            parameterName.str("");
            parameterName << WB_GAINS_S.name << "_" << i;
            values.clear();
            for (j = 0 ; j < 4 ; j++)
            {
                values.push_back(toString(WB_GAINS_S.def[j]));
            }
            values.push_back("// " + getParameterInfo(WB_GAINS_S));
            parameters.addParameter(Parameter(parameterName.str(), values));

            i++;
        }
        break;
    }

    return IMG_SUCCESS;
}

int ISPC::TemperatureCorrection::getCorrectionIndex(double temperature) const
{
    int i = 0;
    std::vector<ColorCorrection>::const_iterator it;

    for (it = temperatureCorrections.begin() ;
        it != temperatureCorrections.end() ; it++)
    {
        if (it->temperature == temperature)
        {
            return i;
        }
        i++;
    }
    return -1;
}

ISPC::ColorCorrection ISPC::TemperatureCorrection::getColorCorrection(
    double temperature) const
{
    ColorCorrection resultCorrection;

    if (temperatureCorrections.empty())
    {
        LOG_ERROR("correction set is empty.\n");
        return resultCorrection;
    }

    // search for the corrections where the requested temperature is located
    ColorCorrection first = temperatureCorrections[0];
    ColorCorrection second = temperatureCorrections[0];
    std::vector<ColorCorrection>::const_iterator it;

    for (it = temperatureCorrections.begin() ;
        it != temperatureCorrections.end() ; it++)
    {
        if (it->temperature <= temperature)
        {
            first = *it;
        }

        second = *it;

        if (it->temperature >= temperature)
        {
            break;
        }
    }

    double t1 = first.temperature;
    double t2 = second.temperature;

    if (t1 == t2)  // same temperature
    {
        return first;
    }

    double tDistance = t2 - t1;
    double blendRatio = (temperature - t1) / tDistance;

    resultCorrection = first.blend(second, blendRatio);
    return resultCorrection;
}

ISPC::ColorCorrection ISPC::TemperatureCorrection::getCorrection(
    unsigned int i) const
{
    if (i < temperatureCorrections.size())
    {
        return temperatureCorrections[i];
    }
    return ColorCorrection();
}

#if defined(INFOTM_ISP)
ISPC::ColorCorrection ISPC::TemperatureCorrection::getCorrection(
    unsigned int i
    )
{
    if (i < temperatureCorrections.size()) {
        return temperatureCorrections[i];
    }

    return ColorCorrection();
}

IMG_RESULT ISPC::TemperatureCorrection::setCorrection(
    unsigned int i,
    ColorCorrection cc
    )
{
    if (i < temperatureCorrections.size()) {
        temperatureCorrections[i] = cc;

        return IMG_SUCCESS;
    }

    return IMG_ERROR_INVALID_PARAMETERS;
}

#endif //#if defined(INFOTM_ISP)
double ISPC::TemperatureCorrection::getCorrelatedTemperature(double r,
    double g, double b) const
{
    double temperature;
    double ratioR, ratioG, ratioB;
    double indexR, indexB;
    int indexRfloor, indexBfloor, indexRceil, indexBceil;
    double weightR, weightB;

    if (0.0 == g)  // set a minimum g value to avoid division by 0
    {
        g = 1.0;
    }

    ratioR = r / g;
    ratioG = 1.0;  // g/g
    ratioB = b / g;

    ratioR = std::min(std::max(ratioR, 0.5), 2.0);
    ratioB = std::min(std::max(ratioB, 0.5), 2.0);

    indexR = ((ratioR - 0.5) / 1.5) * 15;
    indexB = ((ratioB - 0.5) / 1.5) * 15;
	indexRfloor = static_cast<int>(floor(indexR));
	indexBfloor = static_cast<int>(floor(indexB));

	indexRceil = static_cast<int>(ceil(indexR));
	indexBceil = static_cast<int>(ceil(indexB));

    weightR = indexR-indexRfloor;
    weightB = indexB-indexBfloor;

    temperature =
        (correlatedTemperature[indexRfloor][indexBfloor] * (1.0 - weightB)
        + correlatedTemperature[indexRfloor][indexBceil] * weightB)
        * (1.0 - weightR)
        + (correlatedTemperature[indexRceil][indexBfloor] * (1.0 - weightB)
        + correlatedTemperature[indexRceil][indexBceil] * weightB)
        * weightR;

    return temperature;
}

#if defined(INFOTM_ISP) && defined(INFOTM_AWB_PATCH_GET_CCT)
#define MAX(X,Y) (X>Y?X:Y)

double ISPC::TemperatureCorrection::getCCT(double R, double G, double B) const
{
    double temperature;
    double var_R,var_G,var_B,X,Y,Z,x,y,n;
    double xe = 0.332;
    double ye = 0.1858;
    double maxv;

    maxv = MAX(MAX(R, G), B);
#if 0
    var_R = (R / maxv);         //R from 0 to 255
    var_G = (G / maxv);         //G from 0 to 255
    var_B = (B / maxv);         //B from 0 to 255
#else
    var_R = (R / maxv);         //R from 0 to 255
    var_G = (G / maxv);         //G from 0 to 255
    var_B = (B / maxv);         //B from 0 to 255

    if (var_R > 0.04045){
#ifdef USE_MATH_NEON
		var_R = (double)powf_neon((float)((var_R + 0.055) / 1.055), (float)2.4f);
#else
		var_R = pow(((var_R + 0.055) / 1.055), 2.4);
#endif
    }else{
        var_R = var_R / 12.92;
	}

	if (var_G > 0.04045){
#ifdef USE_MATH_NEON
		var_G = (double)powf_neon((float)((var_G + 0.055) / 1.055), (float)2.4f);
#else
		var_G = pow(((var_G + 0.055) / 1.055), 2.4);
#endif
    }else
        var_G = var_G / 12.92;

	if (var_B > 0.04045){
#ifdef USE_MATH_NEON
		 var_B = (double)powf_neon((float)((var_B + 0.055) / 1.055), (float)2.4f);
#else
		 var_B = pow(((var_B + 0.055) / 1.055), 2.4);
#endif
    }else
        var_B = var_B / 12.92;
#endif
    var_R = var_R * 100;
    var_G = var_G * 100;
    var_B = var_B * 100;

    //Observer. = 2 , Illuminant = D65, sRGB
    X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
    Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
    Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

    x= X/(X+Y+Z);
    y= Y/(X+Y+Z);
    n = (x - xe)/(y - ye);

    temperature = -449*n*n*n + 3525*n*n - 6823*n + 5520;

    return temperature;
}
#endif

