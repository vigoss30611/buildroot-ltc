/**
*******************************************************************************
 @file ModuleDSC.h

 @brief Declaration of ISPC::ModuleDSC

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
#ifndef ISPC_MODULE_DSC_H_
#define ISPC_MODULE_DSC_H_

#include <string>

#include "ispc/Parameter.h"
#include "ispc/Module.h"
#include "ispc/ISPCDefs.h"  // for enum ScalerRectType when using this class

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Display pipeline scaler (DSC) HW configuration
 *
 * Setup function configures the @ref MC_DSC elements of the MC_PIPELINE 
 * attached to the owner Pipeline.
 */
class ModuleDSC: public SetupModuleBase<STP_DSC>
{
public:  // attributes
    /**
     * @brief Adjust cutoff frequency in taps calculation.
     *
     * Loaded using @ref DSC_ADJUSTCUTOFF
     */
    bool bAdjustTaps;
    /**
     * @brief Horizontal and vertical downscaling factor
     *
     * Loaded using @ref DSC_PITCH
     */
    double aPitch[2];
    /**
     * @brief Clip=x1,y1,x2,y2; Crop=margins l, t, r, b; Outsize=x1, y1, 
     * outWidth, outHeight.
     *
     * Loaded using @ref DSC_RECTTYPE
     *
     * @ RECT_TYPE is not needed - choose one and convert accordingly when
     * loading from file
     */
    enum ScalerRectType eRectType;
    /**
     * @brief [pixels] Area of input image to downscale
     * - different region according to eRectType
     *
     * Loaded using @ref DSC_RECT
     */
    IMG_UINT32 aRect[4];

public:  // methods
    ModuleDSC();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<std::string> DSC_RECTTYPE;
    static const ParamDefSingle<bool> DSC_ADJUSTCUTOFF;
    static const ParamDefArray<double> DSC_PITCH;
    static const ParamDefArray<int> DSC_RECT;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_DSC_H_ */
