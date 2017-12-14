/**
*******************************************************************************
 @file ModuleESC.h

 @brief Declaration of ISPC::ModuleESC

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
#ifndef ISPC_MODULE_ESC_H_
#define ISPC_MODULE_ESC_H_

#include <string>

#include "ispc/Parameter.h"
#include "ispc/Module.h"
#include "ispc/ISPCDefs.h"  // for enum ScalerRectType when using this class

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Encoder pipeline scaler (ESC) HW configuration
 *
 * Setup function configures the @ref MC_ESC elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleESC: public SetupModuleBase<STP_ESC>
{
public:  // attributes
    /**
     * @brief Adjust cutoff frequency in taps calculation
     *
     * Loaded using @ref ESC_ADJUSTCUTOFF
     */
    bool bAdjustTaps;
    /**
     * @brief Horizontal and vertical downscaling factor
     *
     * Loaded using @ref ESC_PITCH
     */
    double aPitch[2];
    /**
     * @brief Clip=x1, y1, x2, y2; Crop=margins l, t, r, b; Outsize=x1, y1, 
     * outWidth, outHeight
     *
     * Loaded using @ref ESC_RECTTYPE
     *
     * @ RECT_TYPE is not needed - choose one and convert accordingly when
     * loading from file
     */
    enum ScalerRectType eRectType;
    /**
     * @brief [pixels] Area of input image to downscale
     * - different region according to eRectType
     *
     * Loaded using @ref ESC_RECT
     */
    IMG_UINT32 aRect[4];
    /**
     * @brief Horizontal chroma placement relative to luma. 
     * If true interstitial, otherwise co-sited.
     *
     * Loaded using @ref ESC_CHROMA_MODE
     */
    bool bChromaInterstitial;

public:  // methods
    ModuleESC();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<std::string> ESC_RECTTYPE;
    static const ParamDefSingle<bool> ESC_ADJUSTCUTOFF;
    static const ParamDefArray<double> ESC_PITCH;
    static const ParamDefArray<int> ESC_RECT;
    static const ParamDefSingle<std::string> ESC_CHROMA_MODE;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_ESC_H_ */
