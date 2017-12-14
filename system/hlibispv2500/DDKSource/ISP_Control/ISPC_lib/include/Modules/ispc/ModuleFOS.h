/**
*******************************************************************************
 @file ModuleFOS.h

 @brief Declaration of ISPC::ModuleFOS

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
#ifndef ISPC_MODULE_FOS_H_
#define ISPC_MODULE_FOS_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_STATSMODULES
 * @brief Focus Statistics (FOS) HW configuration
 *
 * Setup function configures the @ref MC_FOS elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * Output is generated in @ref Metadata::focusStats
 */
class ModuleFOS: public SetupModuleBase<STP_FOS>
{
public:  // attributes
    /**
     * @brief Enable/Disable ROI-based focus statistics.
     *
     * Loaded using @ref FOS_ROI
     */
    bool bEnableROI;
    /**
     * @brief Sets the position of the top-left corner of the region of
     * interest in pixels.
     *
     * Loaded using @ref FOS_ROISTART
     */
    IMG_UINT32  aRoiStartCoord[2];
    /**
     * @brief Sets the position of the bottom-right corner of the region of
     * interest in pixels.
     *
     * Loaded using @ref FOS_ROIEND
     */
    IMG_UINT32  aRoiEndCoord[2];
    /**
     * @brief Enable/Disable GRID-based focus statistics.
     *
     * Loaded using @ref FOS_GRID
     */
    bool bEnableGrid;
    /**
     * @brief Sets the position of the top-left corner of the grid in pixels.
     *
     * Loaded using @ref FOS_GRIDSTART
     */
    IMG_UINT32  aGridStartCoord[2];
    /**
     * @brief Sets the size of the grid tiles in pixels.
     *
     * Loaded using @ref FOS_GRIDSIZE
     */
    IMG_UINT32  aGridTileSize[2];

public:  // methods
    ModuleFOS();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<bool> FOS_ROI;
    static const ParamDefSingle<bool> FOS_GRID;
    static const ParamDefArray<int> FOS_ROISTART;
    static const ParamDefArray<int> FOS_ROIEND;
    static const ParamDefArray<int> FOS_GRIDSTART;
    static const ParamDefArray<int> FOS_GRIDSIZE;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_FOS_H_ */
