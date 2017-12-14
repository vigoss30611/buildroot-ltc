/**
*******************************************************************************
 @file ModuleEXS.h

 @brief Declaration of ISPC::ModuleEXS

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
#ifndef ISPC_MODULE_EXS_H_
#define ISPC_MODULE_EXS_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_STATSMODULES
 * @brief Exposure Statistics (EXS) HW configuration
 *
 * Setup function configures the @ref MC_EXS elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * Output is generated in @ref Metadata::clippingStats
 */
class ModuleEXS: public SetupModuleBase<STP_EXS>
{
public:  // attributes
    /**
     * @brief Enable/disable global exposure clip statistics.
     *
     * Loaded using @ref EXS_GLOBAL
     */
    bool bEnableGlobal;
    /**
     * @brief Enable/disable regional exposure clip statistics.
     *
     * Loaded using @ref EXS_REGIONAL
     */
    bool bEnableRegion;
    /**
     * @brief Sets the position of the upper-left corner of the region in 
     * pixels. Must be CFA aligned.
     *
     * Loaded using @ref EXS_GRIDSTART
     */
    IMG_UINT32 aGridStart[2];
    /**
     * @brief Sets the dimensions of the region in pixels. Must be CFA aligned
     *
     * Loaded using @ref EXS_GRIDTILE
     */
    IMG_UINT32 aGridTileSize[2];
    /**
     * @brief Sets the threshold value for exposure clip statistics calculation
     *
     * Loaded using @ref EXS_PIXELMAX
     */
    IMG_INT i32PixelMax;

public:  // methods
    ModuleEXS();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<bool> EXS_GLOBAL;
    static const ParamDefSingle<bool> EXS_REGIONAL;
    static const ParamDefArray<int> EXS_GRIDSTART;
    static const ParamDefArray<int> EXS_GRIDTILE;
    static const ParamDef<int> EXS_PIXELMAX;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_EXS_H_ */
