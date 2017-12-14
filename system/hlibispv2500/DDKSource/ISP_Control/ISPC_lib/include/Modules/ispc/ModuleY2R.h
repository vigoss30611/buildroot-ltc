/**
*******************************************************************************
 @file ModuleY2R.h

 @brief Declaration of ISPC::ModuleY2R

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
#ifndef ISPC_MODULE_Y2R_H_
#define ISPC_MODULE_Y2R_H_

#include <string>

#include "ispc/ModuleR2Y.h"  // for ModuleR2YBase

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief YCC to RGB convertion (Y2R) HW configuration
 *
 * Setup function configures the @ref MC_Y2R elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleY2R: public ModuleR2YBase, public SetupModuleBase<STP_Y2R>
{
public:  // methods
    ModuleY2R();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;
    /**
     * @copydoc ISPC::SetupModule::setup()
     * 
     * Delegates to @ref ISPC::ModuleR2YBase::configure()
     */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<std::string> Y2R_MATRIX_STD;
    static const ParamDef<double> Y2R_BRIGHTNESS;
    static const ParamDef<double> Y2R_CONTRAST;
    static const ParamDef<double> Y2R_SATURATION;
    static const ParamDef<double> Y2R_HUE;
    static const ParamDef<double> Y2R_OFFSETU;
    static const ParamDef<double> Y2R_OFFSETV;

    static const ParamDefArray<double> Y2R_RANGEMULT;

    static ParameterGroup getGroup();
};

/**
 * @property ModuleR2YBase::fBrightness
 * For ModuleY2R loaded using @ref ModuleY2R::Y2R_BRIGHTNESS
 */
/**
 * @property ModuleR2YBase::fContrast
 * For ModuleY2R loaded using @ref ModuleY2R::Y2R_CONTRAST
 */
/**
 * @property ModuleR2YBase::fSaturation
 * For ModuleY2R loaded using @ref ModuleY2R::Y2R_SATURATION
 */
/**
 * @property ModuleR2YBase::fHue
 * For ModuleY2R loaded using @ref ModuleY2R::Y2R_HUE
 */
/**
 * @property ModuleR2YBase::aRangeMult
 * For ModuleY2R loaded using @ref ModuleY2R::Y2R_RANGEMULT
 */
/**
 * @property ModuleR2YBase::eMatrix
 * For ModuleY2R loaded using @ref ModuleY2R::Y2R_MATRIX_STD
 */

} /* namespace ISPC */

#endif /* ISPC_MODULE_Y2R_H_ */
