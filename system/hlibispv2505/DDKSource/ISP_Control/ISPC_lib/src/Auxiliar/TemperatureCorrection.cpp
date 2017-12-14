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
#include <cmath>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "ispc/ISPCDefs.h"
#include "ispc/Module.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ModuleWBC.h"
#include "ispc/ModuleAWS.h"

/**
 * If defined, the correlated temperature is clipped to min/max temperature
 * of calibration range.
 * If not defined, the temperature is extrapolated outside calibration range
 */
#undef CLIP_TEMPERATURE_TO_CALIBRATION
/**
 * @brief Max temperature in Kelvin for the AWB algorithm
 * - it is an arbitrary choice
 */
#define WB_MAX_TEMP 100000.0f

const ISPC::ParamDef<int> ISPC::TemperatureCorrection::WB_CORRECTIONS(
    "WB_CORRECTIONS", 0, 20, 0);  // there are no real max, 20 is arbitrary

// base for the parameter (nb to load is defined as WB_CORRECTIONS)
const ISPC::ParamDef<double> ISPC::TemperatureCorrection::WB_TEMPERATURE_S(
    "WB_TEMPERATURE", 0.0f, WB_MAX_TEMP, 0.0f);
const ISPC::ParamDefArray<double> ISPC::TemperatureCorrection::WB_CCM_S(
    "WB_CCM", CCM_MATRIX_MIN, CCM_MATRIX_MAX, CCM_MATRIX_DEF, 9);
const ISPC::ParamDefArray<double> ISPC::TemperatureCorrection::WB_OFFSETS_S(
    "WB_OFFSETS", CCM_OFFSETS_MIN, CCM_OFFSETS_MAX, CCM_OFFSETS_DEF, 3);
const ISPC::ParamDefArray<double> ISPC::TemperatureCorrection::WB_GAINS_S(
    "WB_GAINS", WBC_GAIN_MIN, WBC_GAIN_MAX, WBC_GAIN_DEF, 4);

/**
 * @brief Lookup table of correlated temperatures corresponding to pairs
 * of R/B gains
 *
 * The correlated color temperature for a given RGB colour is calculated by
 * computing the R and B with respect to green and using those ratios as
 * indexes in the correlatedTemperature table.
 * The table indexes ratios ranging between 0.5 and 2.0 and ratios
 * below/above those limits should be clipped before indexing
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
    ParameterGroup group;

    group.header = "// Temperature Correction parameters (Multi-CCM for AWB)";

    group.parameters.insert(WB_CORRECTIONS.name);

    for (i = 0 ; i < WB_CORRECTIONS.max ; i++)
    {
        group.parameters.insert(WB_TEMPERATURE_S.indexed(i).name);
        group.parameters.insert(WB_CCM_S.indexed(i).name);
        group.parameters.insert(WB_OFFSETS_S.indexed(i).name);
        group.parameters.insert(WB_GAINS_S.indexed(i).name);
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

    if (!parameters.exists(WB_CORRECTIONS.name))
    {
        LOG_WARNING("Unable to load temperature corrections from parameters. "\
            "'%s' not defined\n", WB_CORRECTIONS.name.c_str());
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    else  // ready to load the parameters
    {
        int numCorrections = parameters.getParameter(WB_CORRECTIONS);

        this->clearCorrections();

        // Read every correction
        for (i = 0; i < numCorrections; i++)
        {
            ColorCorrection newCorrection;

            newCorrection.temperature =
                    parameters.getParameter(WB_TEMPERATURE_S.indexed(i));

            for (j = 0 ; j < 3 ; j++)
            {
                for (k = 0 ; k < 3 ; k++)
                {
                    newCorrection.coefficients[j][k] =
                        parameters.getParameter(WB_CCM_S.indexed(i), 3*j+k);
                }
            }

            for (j = 0 ; j < 3 ; j++)
            {
                newCorrection.offsets[0][j] =
                        parameters.getParameter(WB_OFFSETS_S.indexed(i), j);
            }

            for (j = 0 ; j < 4 ; j++)
            {
                newCorrection.gains[0][j] =
                        parameters.getParameter(WB_GAINS_S.indexed(i), j);
            }

            LOG_INFO("loaded correction for temperature %.2lf\n",
                newCorrection.temperature);

            newCorrection.setValid();
            addCorrection(newCorrection);
        }

        // need at least two points to form a line
        if(temperatureCorrections.size() > 1)
        {
            std::vector<ColorCorrection>::iterator low =
                    temperatureCorrections.begin();
            std::vector<ColorCorrection>::iterator high =
                    temperatureCorrections.begin();
            double QEr = 1.0f;
            double QEb = 1.0f;

            ColorCorrection ccm = getColorCorrection(6500.0f);
            if(ccm.isValid())
            {
                QEr = ccm.gains[0][0];
                QEb = ccm.gains[0][3];
            }
            // generate line segments using calibration data
            while(true)
            {
                ++low;
                if(low == temperatureCorrections.end())
                {
                    break;
                }
                lines.push_back(LineSegment(
                        ISPC::log2(QEr)-ISPC::log2(high->gains[0][0]), // R
                        ISPC::log2(QEb)-ISPC::log2(high->gains[0][3]), // B
                        ISPC::log2(QEr)-ISPC::log2(low->gains[0][0]), // R
                        ISPC::log2(QEb)-ISPC::log2(low->gains[0][3]), // B
                        high->temperature,
                        low->temperature));
                ++high;
            }
            IMG_ASSERT(lines.size()>0);
#ifndef CLIP_TEMPERATURE_TO_CALIBRATION
            // set borderlines as not bounded above extreme calibration points
            lines.front().lowBound = false;
            lines.back().highBound = false;
#endif
        }
    }
#ifdef INFOTM_SUPPORT_CCM_FACTOR
    {
        std::vector<ColorCorrection>::iterator it;
        for (it = temperatureCorrections.begin(), k = 0;
                it != temperatureCorrections.end(); it++, k++)
        {
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
                    origial_coefficients[k][i][j] = it->coefficients[i][j];
                }
            }

            for (i = 0; i < 4; i++)
            {
                origial_gains[k][i] = it->gains[0][i];
            }

            for (i = 0; i < 3; i++)
            {
                origial_offset[k][i] = it->offsets[0][i];
            }
        }
    }
#endif

    return IMG_SUCCESS;
}

#ifdef INFOTM_SUPPORT_CCM_FACTOR
IMG_RESULT ISPC::TemperatureCorrection::updateCCM(double ccm_factor, double gain_factor)
{
    int i, j, k;
    std::vector<ColorCorrection>::iterator it;

    for (it = temperatureCorrections.begin(), k = 0;
            it != temperatureCorrections.end(); it++, k++)
    {
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                it->coefficients[i][j] = origial_coefficients[k][i][j]*ccm_factor;
            }
        }

        for (i = 0; i < 4; i++)
        {
            it->gains[0][i] = origial_gains[k][i]*gain_factor;
        }
    }

#if 0
    for (it = temperatureCorrections.begin(), k = 0;
            it != temperatureCorrections.end(); it++, k++)
    {
        printf("================\n");
        for (i = 0; i < 3; i++)
        {
            printf("[%f, %f, %f]\n", it->coefficients[i][0], it->coefficients[i][1], it->coefficients[i][2]);
        }

        printf("gain [%f, %f, %f, %f]\n", it->gains[0][0], it->gains[0][1], it->gains[0][2], it->gains[0][3]);
        printf("================\n");
    }
#endif

    return IMG_SUCCESS;
}
#endif

IMG_RESULT ISPC::TemperatureCorrection::saveParameters(
    ParameterList &parameters, ModuleBase::SaveType t) const
{
    int i, j, k;
    std::vector<std::string> values;

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
            parameters.addParameter(WB_CORRECTIONS,
                (int)temperatureCorrections.size());

            std::vector<ColorCorrection>::const_iterator it;

            i = 0;
            for (it = temperatureCorrections.begin() ;
                it != temperatureCorrections.end() ; it++)
            {
                parameters.addParameter(WB_TEMPERATURE_S.indexed(i), it->temperature);

                values.clear();
                for (j = 0 ; j < 3 ; j++)
                {
                    for (k = 0 ; k < 3 ; k++)
                    {
                        values.push_back(toString(it->coefficients[j][k]));
                    }
                }
                parameters.addParameter(WB_CCM_S.indexed(i), values);

                values.clear();
                for (j = 0 ; j < 3 ; j++)
                {
                    values.push_back(toString(it->offsets[0][j]));
                }
                parameters.addParameter(WB_OFFSETS_S.indexed(i), values);

                values.clear();
                for (j = 0 ; j < 4 ; j++)
                {
                    values.push_back(toString(it->gains[0][j]));
                }
                parameters.addParameter(WB_GAINS_S.indexed(i), values);

                i++;
            }
        }
        break;

    case ModuleBase::SAVE_MIN:
        parameters.addParameterMin(WB_CORRECTIONS);

        // for ( i = 0 ; i < WB_CORRECTIONS.min ; i++ )
        // do it only once
        i = 0;
        {
            parameters.addParameterMin(WB_TEMPERATURE_S.indexed(i));
            parameters.addParameterMin(WB_CCM_S.indexed(i));
            parameters.addParameterMin(WB_OFFSETS_S.indexed(i));
            parameters.addParameterMin(WB_GAINS_S.indexed(i));
            i++;
        }
        break;

    case ModuleBase::SAVE_MAX:
        parameters.addParameterMax(WB_CORRECTIONS);

        // for ( i = 0 ; i < WB_CORRECTIONS.min ; i++ )
        // do it only once
        i = 0;
        {
            parameters.addParameterMax(WB_TEMPERATURE_S.indexed(i));
            parameters.addParameterMax(WB_CCM_S.indexed(i));
            parameters.addParameterMax(WB_OFFSETS_S.indexed(i));
            parameters.addParameterMax(WB_GAINS_S.indexed(i));

            i++;
        }
        break;

    case ModuleBase::SAVE_DEF:
        parameters.addParameterDef(WB_CORRECTIONS);

        // for ( i = 0 ; i < WB_CORRECTIONS.min ; i++ )
        // do it only once
        i = 0;
        {
            parameters.addParameterDef(WB_TEMPERATURE_S.indexed(i));
            parameters.addParameterDef(WB_CCM_S.indexed(i));
            parameters.addParameterDef(WB_OFFSETS_S.indexed(i));
            parameters.addParameterDef(WB_GAINS_S.indexed(i));

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

    if (temperatureCorrections.size() < 2)
    {
        LOG_WARNING("Less than 2 calibration points loaded - "
                "can't approximate temperature!\n");
        return resultCorrection;
    }

    // search for the corrections where the requested temperature is located
    std::vector<ColorCorrection>::const_iterator first =
            temperatureCorrections.begin();
    std::vector<ColorCorrection>::const_iterator second =
            temperatureCorrections.begin();
    std::vector<ColorCorrection>::const_iterator it;

    for (it = temperatureCorrections.begin() ;
        it != temperatureCorrections.end() ; it++)
    {
        if (it->temperature <= temperature)
        {
            first = it;
        }

        second = it;

        if (it->temperature >= temperature)
        {
            break;
        }
    }

    double t1 = first->temperature;
    double t2 = second->temperature;

    if (t1 == t2)  // same temperature
    {
        return *first;
    }

    double tDistance = t2 - t1;
    double blendRatio = (temperature - t1) / tDistance;

    resultCorrection = first->blend(*second, blendRatio);
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
double ISPC::TemperatureCorrection::getCorrelatedTemperature_sRGB(
    double r, double g, double b) const
{
    double temperature;
    double ratioR, ratioG, ratioB;
    double indexR, indexB;
    int indexRfloor, indexBfloor, indexRceil, indexBceil;
    double weightR, weightB;

    if (!ISPC::isfinite(g) ||
        0.0 == g)  // set a minimum g value to avoid division by 0
    {
        g = 1.0;
    }

    if(!ISPC::isfinite(r))
    {
        r = 1.0;
    }
    if(!ISPC::isfinite(b))
    {
        b = 1.0;
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

double ISPC::TemperatureCorrection::getCorrelatedTemperature_Calibrated(
    double r, double g, double b) const
{
    double ratioR, ratioB;

    if(lines.empty())
    {
        return 6500.0f;
    }
    if (!ISPC::isfinite(g) ||
        0.0 == g)  // set a minimum g value to avoid division by 0
    {
        g = 1.0;
    }
    if(!ISPC::isfinite(r))
    {
        r = 1.0;
    }
    if(!ISPC::isfinite(b))
    {
        b = 1.0;
    }
    ratioR = ISPC::log2(r / g);
    ratioB = ISPC::log2(b / g);

    return lines.getTemperature(ratioR, ratioB);
}

double ISPC::TemperatureCorrection::getCorrelatedTemperature_McCamy(
    double r, double g, double b, bool gammaCorrection) const
{
    double temperature;

    double var_R,var_G,var_B,X,Y,Z,x,y,n;

    double xe=0.332;
    double ye=0.1858;

    double maxv;

    maxv= ispc_max(ispc_max(r,g),b);

    var_R = ( r / maxv );        //R from 0 to 255
    var_G = ( g / maxv );       //G from 0 to 255
    var_B = ( b / maxv );        //B from 0 to 255
    if(gammaCorrection)
    {
        if ( var_R > 0.04045 )
#ifdef USE_MATH_NEON
                    var_R = (double)powf_neon((float)((var_R + 0.055) / 1.055), (float)2.4f);
#else
                    var_R = pow(( ( var_R + 0.055 ) / 1.055 ) , 2.4);
#endif
        else
            var_R = var_R / 12.92;

        if ( var_G > 0.04045 )
#ifdef USE_MATH_NEON
             var_G = (double)powf_neon((float)((var_G + 0.055) / 1.055), (float)2.4f);
#else
            var_G = pow(( ( var_G + 0.055 ) / 1.055 ) , 2.4);
#endif
        else
            var_G = var_G / 12.92;
        if ( var_B > 0.04045 )
#ifdef USE_MATH_NEON
            var_B = (double)powf_neon((float)((var_B + 0.055) / 1.055), (float)2.4f);
#else
            var_B = pow( (( var_B + 0.055 ) / 1.055 ) , 2.4);
#endif
        else
            var_B = var_B / 12.92;
    }
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

double ISPC::TemperatureCorrection::getCorrelatedTemperature(
        double r, double g, double b) const
{
    if(lines.empty())
    {
        LOG_INFO("No calibration points loaded, using fallback "
                "temperature correlation method!\n");
        return getCorrelatedTemperature_McCamy(r, g, b, true);
    }
    return getCorrelatedTemperature_Calibrated(r, g, b);
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
    if (var_R > 0.04045)
#ifdef USE_MATH_NEON
        var_R = (double)powf_neon((float)((var_R + 0.055) / 1.055), (float)2.4f);
#else
        var_R = pow(((var_R + 0.055) / 1.055), 2.4);
#endif
    else
        var_R = var_R / 12.92;

    if (var_G > 0.04045)
#ifdef USE_MATH_NEON
        var_G = (double)powf_neon((float)((var_G + 0.055) / 1.055), (float)2.4f);
#else
        var_G = pow(((var_G + 0.055) / 1.055), 2.4);
#endif
    else
        var_G = var_G / 12.92;
    if (var_B > 0.04045)
#ifdef USE_MATH_NEON
        var_B = (double)powf_neon((float)((var_B + 0.055) / 1.055), (float)2.4f);
#else
        var_B = pow(((var_B + 0.055) / 1.055), 2.4);
#endif
    else
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


