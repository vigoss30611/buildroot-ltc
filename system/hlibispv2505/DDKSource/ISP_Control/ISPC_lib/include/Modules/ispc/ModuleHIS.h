/**
******************************************************************************
 @file ModuleHIS.h

 @brief Declaration of ISPC::ModuleHIS

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
#ifndef ISPC_MODULE_HIS_H_
#define ISPC_MODULE_HIS_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_STATSMODULES
 * @brief Histogram Statistics (HIS) HW configuration
 *
 * Setup function configures the @ref MC_HIS elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * Output generated in @ref Metadata::histogramStats
 */
class ModuleHIS: public SetupModuleBase<STP_HIS>
{
public:  // attributes
    /**
     * @brief Enable/disable global histogram statistics.
     *
     * Loaded using @ref HIS_GLOBAL
     */
    bool bEnableGlobal;
    /**
     * @brief Enable/disable regional histogram statistics.
     *
     * Loaded using @ref HIS_GRID
     */
    bool bEnableROI;
    /**
     * @brief The offset to be subtracted from the input luma value in order to
     * remove signal foot room.
     *
     * Loaded using @ref HIS_INPUTOFF
     */
    IMG_UINT32  ui32InputOffset;
    /**
     * @brief The scaling factor used to scale the input luma value according
     * to the desired gamut.
     *
     * Loaded using @ref HIS_INPUTSCALE
     *
     * @ verify why int and not float
     */
    IMG_UINT32  ui32InputScale;
    /**
     * @brief Sets the position of the upper-left corner of the region in
     * pixels.
     *
     * Loaded using @ref HIS_GRIDSTART
     */
    IMG_UINT32  aGridStartCoord[2];
    /**
     * @brief Sets the dimensions of the region in pixels.
     *
     * Loaded using @ref HIS_GRIDSIZE
     */
    IMG_UINT32  aGridTileSize[2];

public:  // methods
    ModuleHIS();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<bool> HIS_GLOBAL;
    static const ParamDefSingle<bool> HIS_GRID;
    static const ParamDef<int> HIS_INPUTOFF;
    static const ParamDef<int> HIS_INPUTSCALE;
    static const ParamDefArray<int> HIS_GRIDSTART;
    static const ParamDefArray<int> HIS_GRIDSIZE;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_HIS_H_ */
