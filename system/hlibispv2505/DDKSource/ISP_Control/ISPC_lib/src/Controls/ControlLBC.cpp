/**
******************************************************************************
@file ControlLBC.cpp

@brief ISPC::ControlLBC implementation

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
#include "ispc/ControlLBC.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL_LBC"

#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <ostream>

#include "ispc/ModuleSHA.h"
#include "ispc/ModuleR2Y.h"
#include "ispc/ModuleHIS.h"
#include "ispc/Sensor.h"
#include "ispc/Pipeline.h"
#include "ispc/Save.h"

#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

const ISPC::ParamDef<double> ISPC::ControlLBC::LBC_UPDATE_SPEED(
    "LBC_UPDATE_SPEED", 0.0, 1.0, 0.1);
const ISPC::ParamDef<int> ISPC::ControlLBC::LBC_CONFIGURATIONS(
    "LBC_CONFIGURATIONS", 0, 20, 0);

const ISPC::ParamDef<double> ISPC::ControlLBC::LBC_LIGHT_LEVEL_S(
    "LBC_LIGHT_LEVEL", LBC_LIGHT_MIN, LBC_LIGHT_MAX, LBC_LIGHT_DEF);
const ISPC::ParamDef<double> ISPC::ControlLBC::LBC_SHARPNESS_S(
    "LBC_SHARPNESS", SHA_STRENGTH_MIN, SHA_STRENGTH_MAX, SHA_STRENGTH_DEF);
const ISPC::ParamDef<double> ISPC::ControlLBC::LBC_SATURATION_S(
    "LBC_SATURATION", R2Y_SATURATION_MIN, R2Y_SATURATION_MAX,
    R2Y_SATURATION_DEF);
const ISPC::ParamDef<double> ISPC::ControlLBC::LBC_BRIGHTNESS_S(
    "LBC_BRIGHTNESS", R2Y_BRIGHTNESS_MIN, R2Y_BRIGHTNESS_MAX,
    R2Y_BRIGHTNESS_DEF);
const ISPC::ParamDef<double> ISPC::ControlLBC::LBC_CONTRAST_S(
    "LBC_CONTRAST", R2Y_CONTRAST_MIN, R2Y_CONTRAST_MAX, R2Y_CONTRAST_DEF);
#ifdef INFOTM_ISP
const ISPC::ParamDefSingle<bool> ISPC::ControlLBC::ENABLE_DO_LBC(
    "ENABLE_DO_LBC", false);
#endif //INFOTM_ISP

ISPC::ParameterGroup ISPC::ControlLBC::getGroup()
{
    int i;
    ParameterGroup group;

    group.header = "// Light Based Controls parameters";

    group.parameters.insert(LBC_CONFIGURATIONS.name);
    group.parameters.insert(LBC_UPDATE_SPEED.name);

    for (i = 0; i < LBC_CONFIGURATIONS.max; i++)
    {
        group.parameters.insert(LBC_LIGHT_LEVEL_S.indexed(i).name);
        group.parameters.insert(LBC_SHARPNESS_S.indexed(i).name);
        group.parameters.insert(LBC_SATURATION_S.indexed(i).name);
        group.parameters.insert(LBC_BRIGHTNESS_S.indexed(i).name);
        group.parameters.insert(LBC_CONTRAST_S.indexed(i).name);
    }

#ifdef INFOTM_ISP
    group.parameters.insert(ENABLE_DO_LBC.name);
#endif //INFOTM_ISP

    return group;
}

//
// public
//

ISPC::ControlLBC::ControlLBC(const std::string &logname)
    : ControlModuleBase(logname), updateSpeed(LBC_UPDATE_SPEED.def),
    lightLevel(1.0), meteredLightLevel(0.0), configureHis(false)
{
    defSharpness = 0.0;
    defSaturation = 0.0;
    defContrast = 0.0;
    defBrightness = 0.0;
#ifdef INFOTM_ISP
    this->doLBC = true;
    this->monoChrome = false;
#endif //INFOTM_ISP
}

void ISPC::ControlLBC::clearConfigurations()
{
    configurations.clear();
}

void ISPC::ControlLBC::addConfiguration(
    const LightCorrection &newConfiguration)
{
    configurations.push_back(newConfiguration);
    std::sort(configurations.begin(), configurations.end());
}

IMG_RESULT ISPC::ControlLBC::load(const ParameterList &parameters)
{
    // Load defaults
    if (parameters.exists(ModuleR2Y::R2Y_BRIGHTNESS.name))
    {
        defBrightness = parameters.getParameter(ModuleR2Y::R2Y_BRIGHTNESS);
    }

    if (parameters.exists(ModuleR2Y::R2Y_CONTRAST.name))
    {
        defContrast = parameters.getParameter(ModuleR2Y::R2Y_CONTRAST);
    }

    if (parameters.exists(ModuleR2Y::R2Y_SATURATION.name))
    {
        defSaturation = parameters.getParameter(ModuleR2Y::R2Y_SATURATION);
    }

    if (parameters.exists(ModuleSHA::SHA_STRENGTH.name))
    {
        defSharpness = parameters.getParameter(ModuleSHA::SHA_STRENGTH);
    }

    this->updateSpeed = parameters.getParameter(LBC_UPDATE_SPEED);

    int numConfigurations = parameters.getParameter(LBC_CONFIGURATIONS);

    // clear the default configuration
    this->clearConfigurations();

    // Read every correction
    for (int i = 0; i < numConfigurations; i++)
    {
        LightCorrection newConfiguration;

        newConfiguration.lightLevel =
            parameters.getParameter(LBC_LIGHT_LEVEL_S.indexed(i));
        newConfiguration.sharpness =
            parameters.getParameter(LBC_SHARPNESS_S.indexed(i));
        newConfiguration.saturation =
            parameters.getParameter(LBC_SATURATION_S.indexed(i));
        newConfiguration.brightness =
            parameters.getParameter(LBC_BRIGHTNESS_S.indexed(i));
        newConfiguration.contrast =
            parameters.getParameter(LBC_CONTRAST_S.indexed(i));

        addConfiguration(newConfiguration);

#if VERBOSE_CONTROL_MODULES
        LOG_INFO("loaded configuration for %lf\n",
            newConfiguration.lightLevel);
#endif
    }
#ifdef INFOTM_ISP
    this->doLBC = parameters.getParameter(ENABLE_DO_LBC);
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlLBC::save(ParameterList &parameters, SaveType t) const
{
    int i;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ControlLBC::getGroup();
    }

    parameters.addGroup("ControlLBC", group);

    switch (t)
    {
    case SAVE_VAL:
    {
        parameters.addParameter(LBC_CONFIGURATIONS, int(configurations.size()));
        parameters.addParameter(LBC_UPDATE_SPEED, updateSpeed);

        std::vector<LightCorrection>::const_iterator it =
            configurations.begin();
        i = 0;
        while (it != configurations.end())
        {
            parameters.addParameter(LBC_LIGHT_LEVEL_S.indexed(i), it->lightLevel);
            parameters.addParameter(LBC_SHARPNESS_S.indexed(i), it->sharpness);
            parameters.addParameter(LBC_SATURATION_S.indexed(i), it->saturation);
            parameters.addParameter(LBC_BRIGHTNESS_S.indexed(i), it->brightness);
            parameters.addParameter(LBC_CONTRAST_S.indexed(i), it->contrast);

            i++;
            it++;
        }
#ifdef INFOTM_ISP
        parameters.addParameter(ENABLE_DO_LBC, doLBC);
#endif //INFOTM_ISP
    }
    break;

    case SAVE_MIN:
        parameters.addParameterMin(LBC_CONFIGURATIONS);
        parameters.addParameterMin(LBC_UPDATE_SPEED);

        i = 0;
        // while ( it != configurations.end() ) // do at least 1
        {
            parameters.addParameterMin(LBC_LIGHT_LEVEL_S.indexed(i));
            parameters.addParameterMin(LBC_SHARPNESS_S.indexed(i));
            parameters.addParameterMin(LBC_SATURATION_S.indexed(i));
            parameters.addParameterMin(LBC_BRIGHTNESS_S.indexed(i));
            parameters.addParameterMin(LBC_CONTRAST_S.indexed(i));

            i++;
        }
#ifdef INFOTM_ISP
        parameters.addParameterMin(ENABLE_DO_LBC);
#endif //INFOTM_ISP
        break;

    case SAVE_MAX:
        parameters.addParameterMax(LBC_CONFIGURATIONS);
        parameters.addParameterMax(LBC_UPDATE_SPEED);

        i = 0;
        // while ( it != configurations.end() ) // do at least 1
        {
            parameters.addParameterMax(LBC_LIGHT_LEVEL_S.indexed(i));
            parameters.addParameterMax(LBC_SHARPNESS_S.indexed(i));
            parameters.addParameterMax(LBC_SATURATION_S.indexed(i));
            parameters.addParameterMax(LBC_BRIGHTNESS_S.indexed(i));
            parameters.addParameterMax(LBC_CONTRAST_S.indexed(i));

            i++;
        }
#ifdef INFOTM_ISP
        parameters.addParameterMax(ENABLE_DO_LBC);
#endif //INFOTM_ISP
        break;

    case SAVE_DEF:
        parameters.addParameterDef(LBC_CONFIGURATIONS);
        parameters.addParameterDef(LBC_UPDATE_SPEED);

        i = 0;
        // while ( it != configurations.end() ) // do at least 1
        {
            parameters.addParameterDef(LBC_LIGHT_LEVEL_S.indexed(i));
            parameters.addParameterDef(LBC_SHARPNESS_S.indexed(i));
            parameters.addParameterDef(LBC_SATURATION_S.indexed(i));
            parameters.addParameterDef(LBC_BRIGHTNESS_S.indexed(i));
            parameters.addParameterDef(LBC_CONTRAST_S.indexed(i));

            i++;
        }
#ifdef INFOTM_ISP
        parameters.addParameterDef(ENABLE_DO_LBC);
#endif //INFOTM_ISP
        break;
    }

    return IMG_SUCCESS;
}

// added by linyun.xiong for support change sharpness by ui @2015-07-14
#ifdef INFOTM_ISP_TUNING
IMG_VOID ISPC::ControlLBC::change_def_sharpness(double sharpness)
{
    defSharpness = sharpness;
    //LOG_INFO("--->sharpness is %f\n", defSharpness);
    return;
}
#endif //INFOTM_ISP_TUNING

//update configuration based on a previous shot metadata
IMG_RESULT ISPC::ControlLBC::update(const Metadata &metadata)
{
    double avgBrightness = 0;
    meteredLightLevel = 0;
    const Sensor *sensor = getSensor();

    if (!sensor)
    {
        MOD_LOG_ERROR("ControlLBC has no sensor!\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    avgBrightness = calculateBrightness(metadata);

    // configureStatistics();

    if (avgBrightness == 0.0f)
    {
        LOG_WARNING("Imgage average brightness is 0, cannot compute LBC!\n");
        return IMG_SUCCESS;
    }
    meteredLightLevel = ((sensor->getExposure() / 1000000.0)
        * sensor->getGain()) / avgBrightness;
    // update the light level based on the update speed
    lightLevel = lightLevel * (1.0 - updateSpeed)
        + meteredLightLevel * updateSpeed;
    currentConfiguration = getLumaConfiguration(lightLevel);

#ifdef INFOTM_ISP
    //add monoChrome function add set sharpness
    currentConfiguration.sharpness = defSharpness;
    if(this->monoChrome)
    {
    	currentConfiguration.saturation = 0.0f;
    }
#endif //INFOTM_ISP

#if VERBOSE_CONTROL_MODULES
    LOG_INFO("metered light level %lf - config %lf (sharpness %lf, "\
        "brightness %lf, contrast %lf, saturation %lf)\n",
        meteredLightLevel, currentConfiguration.lightLevel,
        currentConfiguration.sharpness, currentConfiguration.brightness,
        currentConfiguration.contrast, currentConfiguration.saturation);
#endif

#ifdef INFOTM_ISP
    //add control LBC
    if (doLBC)
    {
        programCorrection();
    }
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

void ISPC::ControlLBC::setUpdateSpeed(double value)
{
    if (value < 0.0 || value > 1.0)
    {
        LOG_ERROR("Update speed value must be between 0.0 and 1.0 "\
            "(received %f)\n", value);
        return;
    }

    updateSpeed = value;
}

double ISPC::ControlLBC::getUpdateSpeed() const
{
    return updateSpeed;
}

void ISPC::ControlLBC::setAllowHISConfig(bool enable)
{
    configureHis = enable;
}

bool ISPC::ControlLBC::getAllowHISConfig() const
{
    return configureHis;
}

ISPC::LightCorrection ISPC::ControlLBC::getLumaConfiguration(
    double lightLevel)
{
    LightCorrection resultConfiguration;

    if (configurations.empty())
    {
#ifdef INFOTM_ISP
		resultConfiguration.brightness = this->defBrightness;
		resultConfiguration.contrast = this->defContrast;
		resultConfiguration.saturation = this->defSaturation;
#endif
        LOG_DEBUG("Configuration list is empty.\n");
        return resultConfiguration;
    }

    // search for the configuration where the requested luma is located
    LightCorrection first = configurations[0];
    LightCorrection second = configurations[0];

    /* Look for the configurations in the collection which are just above
     * and below the requested value (if they exist) */
    for (std::vector<LightCorrection>::iterator it = configurations.begin();
        it != configurations.end(); it++)
    {
        if (it->lightLevel <= lightLevel)
        {
            first = *it;
        }

        second = *it;

        if (it->lightLevel >= lightLevel)
        {
            break;
        }
    }

    double l1 = first.lightLevel;
    double l2 = second.lightLevel;

    if (l1 == l2)  // same luma level
    {
        return first;  // return any of the two
    }

    double lDistance = l2 - l1;
    double blendRatio = (lightLevel - l1) / lDistance;

    // blend the two configurations
    resultConfiguration = first.blend(second, blendRatio);
    return resultConfiguration;
}

double ISPC::ControlLBC::getLightLevel() const
{
    return lightLevel;
}

ISPC::LightCorrection ISPC::ControlLBC::getCurrentCorrection() const
{
    return currentConfiguration;
}

double ISPC::ControlLBC::getMeteredLightLevel() const
{
    return meteredLightLevel;
}

int ISPC::ControlLBC::getConfigurationsNumber() const
{
    return configurations.size();
}

ISPC::LightCorrection ISPC::ControlLBC::getCorrection(int index) const
{
    return configurations[index];
}

bool ISPC::ControlLBC::hasConverged() const
{
    return true;
}

std::ostream& ISPC::ControlLBC::printState(std::ostream &os) const
{
    os << SAVE_A1 << getLoggingName() << ":" << std::endl;

    os << SAVE_A2 << "config:" << std::endl;
    os << SAVE_A3 << "enabled = " << enabled << std::endl;
    os << SAVE_A3 << "updateSpeed = " << updateSpeed << std::endl;

    os << SAVE_A2 << "state:" << std::endl;
    os << SAVE_A3 << "meteredLightLevel = " << meteredLightLevel << std::endl;
    os << SAVE_A3 << "lightLevel = " << lightLevel << std::endl;

    return os;
}

//
// public static
//

double ISPC::ControlLBC::calculateBrightness(const Metadata &metadata)
{
    double weightedTotal = 0;
    double histogramTotal = 0;
    // Get the average brightness from the received histogram
    for (int i = 0; i < 64; i++)
    {
        weightedTotal += static_cast<double>(
            metadata.histogramStats.globalHistogram[i]) * i;
        histogramTotal += metadata.histogramStats.globalHistogram[i];
    }

    return (weightedTotal / histogramTotal) / 63.0;
}

//
// protected
//

IMG_RESULT ISPC::ControlLBC::configureStatistics()
{
    ModuleHIS *pHIS = NULL;

    if (getPipelineOwner())
    {
        pHIS = getPipelineOwner()->getModule<ModuleHIS>();
    }
    else
    {
        MOD_LOG_ERROR("ControlLBC has no pipeline owner! Cannot "\
            "configure statistics.\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (pHIS)
    {
        if (!pHIS->bEnableGlobal)
        {
            if (configureHis)
            {
                /* Configure statistics to make the 7x7 grid cover as
                 * much image as possible */
                pHIS->bEnableGlobal = true;

                pHIS->ui32InputOffset = ModuleHIS::HIS_INPUTOFF.def;
                pHIS->ui32InputScale = ModuleHIS::HIS_INPUTSCALE.def;

                pHIS->requestUpdate();
            }
            else
            {
                /* we cannot change the configuration, but we'll check it
                 * to make sure it's available */
                LOG_WARNING("Global Histograms in HIS are not enabled! "\
                    "The LBC will not be able to compute the estimated "\
                    "brightness.\n");
            }
        }

        return IMG_SUCCESS;
    }
    else
    {
        MOD_LOG_ERROR("ControlLBC cannot find HIS module\n");
    }

    return IMG_ERROR_UNEXPECTED_STATE;
}

IMG_RESULT ISPC::ControlLBC::programCorrection()
{
    std::list<Pipeline *>::iterator it;

    for (it = pipelineList.begin(); it != pipelineList.end(); it++)
    {
        /* Write the requested configuration in the corresponding
         * pipeline modules */
        ModuleSHA *sha = (*it)->getModule<ModuleSHA>();
        ModuleR2Y *r2y = (*it)->getModule<ModuleR2Y>();

        if (sha && r2y)
        {
            sha->fStrength = currentConfiguration.sharpness;

            r2y->fBrightness = currentConfiguration.brightness;
            r2y->fContrast = currentConfiguration.contrast;
            r2y->fSaturation = currentConfiguration.saturation;

            sha->requestUpdate();
            r2y->requestUpdate();
        }
#if VERBOSE_CONTROL_MODULES
        else
        {
            MOD_LOG_WARNING("one pipeline is missing either R2Y or "\
                "SHA modules\n");
        }
#endif
    }

    return IMG_SUCCESS;
}
