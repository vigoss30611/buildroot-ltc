/**
*******************************************************************************
 @file ControlLBC.h

 @brief Declaration of the ISPC::ControlDNS class

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
#ifndef ISPC_CONTROL_LBC_H_
#define ISPC_CONTROL_LBC_H_

#include <string>
#include <vector>
#include <ostream>

#include "ispc/Module.h"
#include "ispc/Parameter.h"
#include "ispc/LightCorrection.h"

namespace ISPC {

#define LBC_LIGHT_MIN 0.0
#define LBC_LIGHT_MAX 100000000.0
#define LBC_LIGHT_DEF 0.0

/**
 * @ingroup ISPC2_CONTROLMODULES
 * @brief Light Based Control (LBC) which is in charge on modifying different
 * parameters of the pipeline according to the amount of light captured by the 
 * sensor.
 *
 * The amount of light is estimated based on the capture metadata as well as
 * the sensor settings.
 * The main purpose is to control the pipeline configuration the image quality
 * is not impacted by different settings in the capture.
 * The module allows to define an arbitrary number of configurations, each one
 * associated to a particular light level.
 * Once running, the actual captured light level is measured and the
 * configuration to apply is interpolated from the set of predefined
 * configurations, using the ones defined with the closest light levels.
 *
 * Uses the Sensor and ModuleHIS statistics to update some of the ModuleSHA
 * and ModuleR2Y.
 */
class ControlLBC: public ControlModuleBase<CTRL_LBC>
{
protected:
    /**
     * @brief A vector containing the different configurations for different
     * light levels
     */
    std::vector<LightCorrection> configurations;
    /** @brief The configuration that is being currently applied */
    LightCorrection currentConfiguration;

    /**
     * @brief Speed to update the light metering so we have a smooth
     * transition between configurations
     */
    double updateSpeed;

    /** @brief Measured light level using sensor information and HIS */
    double meteredLightLevel;

    /** 
     * @brief Measurement of the light level captured by the sensor taking
     * measured light level and update speed into account 
     */
    double lightLevel;

    // Store defaults
    double defSharpness; /**< @brief Default sharpness value */
    double defSaturation; /**< @brief Default image saturation */
    double defBrightness; /**< @brief Default image brightness */
    double defContrast; /**< @brief Default image contrast */

    /** @brief Is this module responsible to configure the HIS? */
    bool configureHis;

public:  // methods
    /** @brief Constructor initializing with defaults */
    explicit ControlLBC(const std::string &logname = "ISPC_CTRL_LBC");

    virtual ~ControlLBC() {}

    /** @brief Clear all the stored LBC configurations */
    void clearConfigurations();

    /**
     * @brief Add a new configuration to the collections
     * @param newConfiguration New configuration to be added
     */
    void addConfiguration(const LightCorrection &newConfiguration);

    /** @copydoc ControlModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ControlModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ControlModule::update() */
    virtual IMG_RESULT update(const Metadata &metadata);

    /**
     * @brief Set the value for the update speed, which controls the speed
     * update for the estimated light measure
     * @param value New speed value to be set (between 0.0 and 1.0)
     * @see getUpdateSpeed
     */
    void setUpdateSpeed(double value);

    /**
     * @brief Get the currently applied update speed
     * @see setUpdateSpeed
     */
    double getUpdateSpeed() const;

    /**
     * @brief Use to allow the ControlLBC to setup the global HIS in
     * configureStatistics()
     *
     * This is useful if no other Control algorithms are running.
     *
     * But if using other algorithms (e.g. ControlAE) this should be disabled.
     */
    void setAllowHISConfig(bool enable);

    /**
     * @brief To know if configureStatistics() will modify HIS global
     * histogram configuration
     */
    bool getAllowHISConfig() const;

    /**
     * @brief Computes an interpolated configuration based on the requested
     * light level and the stored configuration for different light levels
     *
     * @param lightLevel the light level we are requesting a configuration for
     * @return An interpolated configuration, based on interpolating the
     * stored configurations whose light level is closer the the requested one
     */
    LightCorrection getLumaConfiguration(double lightLevel);

    /**
     * @brief Get light level used to choose the LightCorrection
     * (takes metered ligh level and update speed into account)
     */
    double getLightLevel() const;

    /** @brief Access to the currently applied correction */
    LightCorrection getCurrentCorrection() const;

    /** @brief Access to the metered light level */
    double getMeteredLightLevel() const;

    /** @brief Number of loaded LightCorrection */
    int getConfigurationsNumber() const;

    /** @brief Access to the correction in the list */
    LightCorrection getCorrection(int index) const;

    /** @warning not using a convergence algorithm so is always true */
    virtual bool hasConverged() const;

    virtual std::ostream& printState(std::ostream &os) const;
	
    // added by linyun.xiong @2015-07-14
#ifdef INFOTM_ISP_TUNING
    IMG_VOID change_def_sharpness(double sharpness);
#endif //INFOTM_ISP_TUNING	

protected:
    /**
     * @copydoc ControlModule::configureStatistics()
     *
     * if configureHis enables the global HIS
     */
    virtual IMG_RESULT configureStatistics();

    /**
     * @copydoc ControlModule::programCorrection()
     *
     * Programs SHA and R2Y
     */
    virtual IMG_RESULT programCorrection();

public:
    /**
     * @name Auxiliary functions
     * @brief Contains the algorithms knowledge
     * @{
     */

    /**
     * @brief Calculate the average image brightness based on the previous
     * frame metadata.
     * The brightness is calculated from the histogram statistics (HIS module).
     *
     * @param metadata Metadata from a previously captured shot, as returned
     * from the pipeline
     * @return Average image brightness value
     */
    static double calculateBrightness(const Metadata &metadata);

    /**
     * @}
     */

public:  // parameters
    static const ParamDef<double> LBC_UPDATE_SPEED;
    static const ParamDef<int> LBC_CONFIGURATIONS;

    // one per found configuration
    static const ParamDef<double> LBC_LIGHT_LEVEL_S;
    // one per found configuration
    static const ParamDef<double> LBC_SHARPNESS_S;
    // one per found configuration
    static const ParamDef<double> LBC_SATURATION_S;
    // one per found configuration
    static const ParamDef<double> LBC_BRIGHTNESS_S;
    // one per found configuration
    static const ParamDef<double> LBC_CONTRAST_S;

#ifdef INFOTM_ISP
    static const ParamDefSingle<bool> ENABLE_DO_LBC;

#endif //INFOTM_ISP
    static ParameterGroup getGroup();
#ifdef INFOTM_ISP
    bool doLBC;
    bool monoChrome;
#endif //INFOTM_ISP
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_LBC_H_ */
