/**
******************************************************************************
 @file ModuleFLD.h

 @brief Declaration of ISPC::ModuleFLD

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
#ifndef ISPC_MODULE_FLD_H_
#define ISPC_MODULE_FLD_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_STATSMODULES
 * @brief Flicker Detection Statistics (FLD) HW configuration
 *
 * Setup function configures the @ref MC_FLD elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * Output is generated in @ref Metadata::flickerStats
 */
class ModuleFLD: public SetupModuleBase<STP_FLD>
{
public:
    /**
     * @brief Enable flicker detection
     *
     * Loaded using @ref FLD_ENABLE
     */
    bool bEnable;
    /**
     * @brief Sensor frame rate
     *
     * Loaded using @ref FLD_FRAMERATE
     *
     * @ should be loaded from the sensor
     */
    double fFrameRate;
    /**
     * @brief Total veritcal size of the sensor - including blanking
     *
     * Loaded using @ref FLD_VTOT
     *
     * @ should be loaded from the sensor
     */
    IMG_INT32 iVTotal;
    /**
     * Loaded using @ref FLD_SCENECHANGE
     */
    IMG_INT32 iScheneChangeTH;
    /**
     * Loaded using @ref FLD_MINPN
     */
    IMG_INT32 iMinPN;
    /**
     * Loaded using @ref FLD_PN
     */
    IMG_INT32 iPN;
    /**
     * Loaded using @ref FLD_NFTH
     */
    IMG_INT32 iNF_TH;
    /**
     * Loaded using @ref FLD_COEFDIFF
     */
    IMG_INT32 iCoeffDiffTH;
    /**
     * Loaded using @ref FLD_RSHIFT
     */
    IMG_INT32 iRShift;
    /**
     * Loaded using @ref FLD_RESET
     *
     * @ verify if that should not be something to have a control module for instead of loading from parameters
     */
    bool bReset;

public:  // methods
    ModuleFLD();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /**
     * @copydoc ISPC::SetupModule::setup()
     *
     * @ review and simplify to actually have high level parameters?
     */
    virtual IMG_RESULT setup();

public:  // parameters
    /** @note deprecated use @ref Sensor::SENSOR_FRAMERATE */
    static const ParamDef<double> FLD_FRAMERATE;
    /** @note deprecated use @ref Sensor::SENSOR_VTOT */
    static const ParamDef<int> FLD_VTOT;

    static const ParamDef<int> FLD_SCENECHANGE;
    static const ParamDef<int> FLD_MINPN;
    static const ParamDef<int> FLD_PN;
    static const ParamDef<int> FLD_NFTH;
    static const ParamDef<int> FLD_COEFDIFF;
    static const ParamDef<int> FLD_RSHIFT;
    static const ParamDefSingle<bool> FLD_RESET;
    static const ParamDefSingle<bool> FLD_ENABLE;

    static ParameterGroup GetGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_FLD_H_ */
