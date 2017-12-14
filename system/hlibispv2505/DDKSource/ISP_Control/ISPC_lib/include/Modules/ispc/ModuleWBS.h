/**
*******************************************************************************
 @file ModuleWBS.h

 @brief Declaration of ISPC::ModuleWBS

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
#ifndef ISPC_MODULE_WBS_H_
#define ISPC_MODULE_WBS_H_

#include <felix_hw_info.h>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_STATSMODULES
 * @brief White Balance Statistics (WBS) HW configuration
 *
 * Setup function configures the @ref MC_WBS elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * Output generated in @ref Metadata::whiteBalanceStats
 */
class ModuleWBS: public SetupModuleBase<STP_WBS>
{
public:  // attributes
    /**
     * @brief Number of enabled regions of interest to use
     *
     * Loaded using @ref WBS_ROI
     */
    IMG_UINT32 ui32NROIEnabled;

    /**
     * @brief White patch red channel threshold
     *
     * Loaded using @ref WBS_R_MAX
     */
    double  aRedMaxTH[WBS_NUM_ROI];
    /**
     * @brief White patch green channel threshold
     *
     * Loaded using @ref WBS_G_MAX
     */
    double  aGreenMaxTH[WBS_NUM_ROI];
    /**
     * @brief White patch blue channel threshold
     *
     * Loaded using @ref WBS_B_MAX
     */
    double  aBlueMaxTH[WBS_NUM_ROI];
    /**
     * @brief High luminance white, luma channel threshold
     *
     * Loaded using @ref WBS_Y_HLW
     */
    double  aYHLWTH[WBS_NUM_ROI];

    /**
     * @brief Sets the position of the top-left corner of the regions of
     * interest in pixels.
     *
     * Loaded using @ref WBS_ROISTART
     */
    IMG_UINT32 aROIStartCoord[WBS_NUM_ROI][2];
    /**
     * @brief Sets the position of the bottom-right corner of the regions of 
     * interest in pixels.
     *
     * Loaded using @ref WBS_ROIEND
     */
    IMG_UINT32 aROIEndCoord[WBS_NUM_ROI][2];

public:  // methods
    ModuleWBS();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDef<int> WBS_ROI;
    // 1 value duplicate per Region of Interest
    static const ParamDefArray<double> WBS_R_MAX;
    // 1 value duplicate per Region of Interest
    static const ParamDefArray<double> WBS_G_MAX;
    // 1 value duplicate per Region of Interest
    static const ParamDefArray<double> WBS_B_MAX;
    // 1 value duplicate per Region of Interest
    static const ParamDefArray<double> WBS_Y_HLW;
    // 2 values duplicate per Region of Interest
    static const ParamDefArray<int> WBS_ROISTART;
    // 2 values duplicate per Region of Interest
    static const ParamDefArray<int> WBS_ROIEND;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_WBS_H_ */
