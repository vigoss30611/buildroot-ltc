/**
*******************************************************************************
 @file ModuleGMA.h

 @brief Declaration of ISPC::ModuleGMA

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
#ifndef ISPC_MODULE_GMA_H_
#define ISPC_MODULE_GMA_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Gamma Look Up Table control (GMA) HW configuration
 *
 * Setup function configures the @ref MC_GMA elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleGMA: public SetupModuleBase<STP_GMA>
{
public:  // attributes
    /**
     * @brief Enable/disable use of gamma LUT
     *
     * Loaded using @ref GMA_BYPASS
     */
    bool bBypass;
    bool useCustomGam;
    IMG_UINT16 customGamCurve[63];
public:  // methods
    ModuleGMA();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<bool> GMA_BYPASS;
    static const ParamDefSingle<bool> USE_CUSTOM_GAM;
    static const ParamDefArray<IMG_UINT16> CUSTOM_GAM_CURVE;
    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_GMA_H_ */
