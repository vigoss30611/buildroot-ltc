/**
*******************************************************************************
 @file ModuleENS.h

 @brief Declaration of Encode Statistics module class

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
#ifndef ISPC_MODULE_ENS_H_
#define ISPC_MODULE_ENS_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_STATSMODULES
 * @brief Encoder Statistics output (ENS) HW configuration
 *
 * Setup function configures the @ref MC_ENS elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * Statistics are generated in their own output buffer @ref Shot::ENS
 */
class ModuleENS: public SetupModuleBase<STP_ENS>
{
public:  // attributes
    /**
     * @brief Enable generation of statistics
     *
     * Loaded using @ref ENS_ENABLE
     */
    bool bEnable;
    /**
     * @brief Number of lines in each region
     * @warning Must be a power of 2
     *
     * Loaded using @ref ENS_REGION_NUMLINES
     */
    IMG_UINT32 ui32NLines;
    /**
     * @brief horizontal sub-sampling to compute ratio 1:sub-sampling
     * @warning Must be a power of 2
     *
     * Loaded using @ref ENS_SUBS_FACTOR
     */
    IMG_UINT32 ui32SubsamplingFactor;

public:  // methods
    ModuleENS();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<bool> ENS_ENABLE;
    static const ParamDef<int> ENS_REGION_NUMLINES;
    static const ParamDef<int> ENS_SUBS_FACTOR;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_ENS_H_ */
