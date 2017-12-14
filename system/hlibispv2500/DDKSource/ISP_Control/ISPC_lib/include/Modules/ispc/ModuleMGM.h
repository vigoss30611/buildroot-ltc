/**
*******************************************************************************
 @file ModuleMGM.h

 @brief Declaration of ISPC::ModuleMGM

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
#ifndef ISPC_MODULE_MGM_H_
#define ISPC_MODULE_MGM_H_

#include <felix_hw_info.h>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @brief Used by both ModuleMGM and ModuleDGM
 */
class ModuleGMBase
{
public:  // attributes
    /** @brief [6 floats,real] Main Gamut Map Coeffs */
    double aCoeff[MGM_N_COEFF];
    /** @brief [3 floats,real] Main Gamut Map Slope */
    double aSlope[MGM_N_SLOPE];
    /** @brief [float,real] Main Gamut Clip Min */
    double fClipMin;
    /** @brief [float,real] Main Gamut Source Normal */
    double fSrcNorm;
    /** @brief @brief [float,real] Main Gamut Clip Max */
    double fClipMax;

    /**
     * @brief Configure given gamut block
     * - both MC_PIPELINE::sMGM and MC_PIPELINE::sDGM are that data-type
     */
    IMG_RESULT configure(MC_GAMUTMAPPER &gamut);
};

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Main Gamut Mapper (MGM) HW configuration
 *
 * Setup function configures the MC_MGM elements of the MC_PIPELINE attached
 * to the owner Pipeline
 */
class ModuleMGM: public ModuleGMBase, public SetupModuleBase<STP_MGM>
{
public:  // methods
    ModuleMGM();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /**
     * @copydoc ISPC::SetupModule::setup()
     *
     * Delegates to @ref ISPC::ModuleGMBase::configure()
     */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefArray<double> MGM_COEFF;
    static const ParamDefArray<double> MGM_SLOPE;
    static const ParamDef<double> MGM_SRC_NORM;
    static const ParamDef<double> MGM_CLIP_MIN;
    static const ParamDef<double> MGM_CLIP_MAX;

    static ParameterGroup getGroup();
};
/**
 * @property ModuleGMBase::aCoeff
 * For MGM loaded using @ref ModuleMGM::MGM_COEFF
 */
/**
 * @property ModuleGMBase::aSlope
 * For MGM loaded using @ref ModuleMGM::MGM_SLOPE
 */
/**
 * @property ModuleGMBase::fSrcNorm
 * For MGM loaded using @ref ModuleMGM::MGM_SRC_NORM
 */
/**
 * @property ModuleGMBase::fClipMin
 * For MGM loaded using @ref ModuleMGM::MGM_CLIP_MIN
 */
/**
 * @property ModuleGMBase::fClipMax
 * For MGM loaded using @ref ModuleMGM::MGM_CLIP_MAX
 */

} /* namespace ISPC */

#endif /* ISPC_MODULE_MGM_H_ */
