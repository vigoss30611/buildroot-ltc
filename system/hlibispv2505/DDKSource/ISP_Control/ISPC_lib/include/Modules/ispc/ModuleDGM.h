/**
******************************************************************************
 @file ModuleDGM.h

 @brief Declaration of ISPC::ModuleDGM

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
#ifndef ISPC_MODULE_DGM_H_
#define ISPC_MODULE_DGM_H_

#include "ispc/ModuleMGM.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Display Gamut Mapper (DGM) HW configuration
 *
 * Setup function configures the @ref MC_DGM elements of the MC_PIPELINE
 * attached to the owner Pipeline
 */
class ModuleDGM: public ModuleGMBase, public SetupModuleBase<STP_DGM>
{
public:  // methods
    ModuleDGM();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;
    /**
     * @copydoc ISPC::SetupModule::setup()
     *
     * Delegates to @ref ModuleGMBase::configure()
     */
    virtual IMG_RESULT setup();

public:  // parameter
    static const ParamDefArray<double> DGM_COEFF;
    static const ParamDefArray<double> DGM_SLOPE;
    static const ParamDef<double> DGM_SRC_NORM;
    static const ParamDef<double> DGM_CLIP_MIN;
    static const ParamDef<double> DGM_CLIP_MAX;

    static ParameterGroup getGroup();
};

/**
 * @property ModuleGMBase::aCoeff
 * For DGM loaded using @ref ModuleDGM::DGM_COEFF
 */
/**
 * @property ModuleGMBase::aSlope
 * For DGM loaded using @ref ModuleDGM::DGM_SLOPE
 */
/**
 * @property ModuleGMBase::fSrcNorm
 * For DGM loaded using @ref ModuleDGM::DGM_SRC_NORM
 */
/**
 * @property ModuleGMBase::fClipMin
 * For DGM loaded using @ref ModuleDGM::DGM_CLIP_MIN
 */
/**
 * @property ModuleGMBase::fClipMax
 * For DGM loaded using @ref ModuleDGM::DGM_CLIP_MAX
 */

} /* namespace ISPC */

#endif /* ISPC_MODULE_DGM_H_ */
