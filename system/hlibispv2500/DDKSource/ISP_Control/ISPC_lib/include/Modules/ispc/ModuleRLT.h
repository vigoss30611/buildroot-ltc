/**
*******************************************************************************
 @file ModuleRLT.h

 @brief Declaration of ISPC::ModuleRLT

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
#ifndef ISPC_MODULE_RLT_H_
#define ISPC_MODULE_RLT_H_

#include <string>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Raw Look Up Table (RLT) HW configuration
 *
 * Setup function configures the @ref MC_RLT elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleRLT: public SetupModuleBase<STP_RLT>
{
public:  // attributes
    enum Mode {
        RLT_DISABLED = 0,
        RLT_LINEAR,
        RLT_CUBIC,
    };

    /** 
     * @brief Correction mode
     *
     * Changes the meaning of the data stored in aLUTPoints
     *
     * Loaded using @ref RLT_CORRECTION_MODE
     */
    enum Mode eMode;
    /**
     * @brief LUT points - meaning varies according to selected mode
     *
     * @li if linear interpolation aLUTPoints_X should be considered as 
     * 4 segments of points allowing the configuration of a curve with
     * different knee points location.
     * @li if cubic interpolation aLUTPoints_X should be considered as 1 curve 
     * per colour channel
     *
     * Loaded using @ref RLT_POINTS_S
     */
    IMG_UINT16 aLUTPoints[RLT_SLICE_N][RLT_SLICE_N_POINTS];

public:  // methods
    ModuleRLT();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

    static enum Mode getRLTMode(const std::string &stringFormat);
    static std::string getRLTModeString(enum Mode mode);

public:  // parameters
    static const ParamDefSingle<std::string> RLT_CORRECTION_MODE;
    static const ParamDefArray<unsigned int> RLT_POINTS_S;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_RLT_H_ */
