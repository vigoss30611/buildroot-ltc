/**
*******************************************************************************
 @file ModuleVIB.h

 @brief Declaration of ISPC::ModuleVIB

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
#ifndef ISPC_MODULE_WBC_H_
#define ISPC_MODULE_WBC_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

#include <ci/ci_api_structs.h>  // enum WBC_MODES

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief White Balance Correction configuration, part of the LSH HW module
 *
 * Setup function configures the @ref MC_WBC elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * In HW the white balance correction is applied in the LSH module.
 */
class ModuleWBC: public SetupModuleBase<STP_WBC>
{
protected:
    /** @brief for children classes - does not load defaults */
    ModuleWBC(const std::string &loggingName);
public:  // attributes
    /**
     * @brief White balance gains, one per Bayer channel (always RGGB order)
     *
     * Loaded using @ref WBC_GAIN
     */
    double aWBGain[4];
    /**
     * @brief Output pixel white clipping per channel.
     *
     * Loaded using @ref WBC_CLIP
     */
    double aWBClip[4];

#ifdef INFOTM_SW_AWB_METHOD
	double aWBGain_def[4];
#endif //INFOTM_SW_AWB_METHOD

public:  // methods
    /** @brief create object and loads defaults */
    ModuleWBC();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefArray<double> WBC_GAIN;
    static const ParamDefArray<double> WBC_CLIP;

    static ParameterGroup getGroup();
};

/**
 * @brief Similar to ModuleWBC but has extended White Balance Clipping HW
 * support (available since HW 2.6)
 */
class ModuleWBC2_6: public ModuleWBC
{
public:  // attributes
    double afRGBGain[3];
    double afRGBThreshold[3];
    enum WBC_MODES eRGBThresholdMode;

public:  // methods
    ModuleWBC2_6();

    /** @brief Get enum value from string - returns -1 for unknown mode */
    static enum WBC_MODES getRGBMode(const std::string &param);
    static std::string getRGBModeString(enum WBC_MODES e);

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefArray<double> WBC_RGB_GAIN;
    static const ParamDefArray<double> WBC_RGB_THRESHOLD;
    static const ParamDefSingle<std::string> WBC_RGB_MODE;

    static ParameterGroup getGroup();
};

/**
 * @brief Min for ModuleWBC::WBC_GAIN and TemperatureCorrection::WB_GAINS_S
 */
#define WBC_GAIN_MIN 0.5
/**
 * @brief Max for ModuleWBC::WBC_GAIN and TemperatureCorrection::WB_GAINS_S
 */
#define WBC_GAIN_MAX 8.0
/**
 * @brief Defaults for ModuleWBC::WBC_GAIN and
 * TemperatureCorrection::WB_GAINS_S
 */
static const double WBC_GAIN_DEF[4] = {1.0, 1.0, 1.0, 1.0};

/**
 * @brief Min for ModuleWBC::WBC_GAIN and TemperatureCorrection::WB_GAINS_S
 */
#define WBC_CLIP_MIN 0.5
/*
 * @brief Max for ModuleWBC::WBC_GAIN and TemperatureCorrection::WB_GAINS_S
 */
#define WBC_CLIP_MAX 2.0
/**
 * @brief Default for ModuleWBC::WBC_GAIN and
 * TemperatureCorrection::WB_GAINS_S
 */
static const double WBC_CLIP_DEF[4] = {1.0, 1.0, 1.0, 1.0};

} /* namespace ISPC */

#endif /* ISPC_MODULE_WBC_H_ */
