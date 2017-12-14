/**
*******************************************************************************
 @file ModuleCCM.h

 @brief Declaration of ISPC::ModuleCCM

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
#ifndef ISPC_MODULE_CCM_H_
#define ISPC_MODULE_CCM_H_

#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Colour Correction Matrix (CCM) HW configuration
 *
 * Setup function configures the @ref MC_CCM elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleCCM: public SetupModuleBase<STP_CCM>
{
public:  // attributes
    /**
     * @brief [3x3 row-wise] Colour correction matrix
     *
     * Loaded using @ref CCM_MATRIX
     */
    double aMatrix[9];
    /**
     * @brief [3 ints,module out bd] Colour correction output channel offsets
     *
     * Loaded using @ref CCM_OFFSETS
     */
    double aOffset[3];

public:  // methods
    ModuleCCM();

    /**
     * @copydoc ISPC::SetupModule::load()
     */
    virtual IMG_RESULT load(const ParameterList &parameters);
    /**
     * @copydoc ISPC::SetupModule::save()
     */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;
    /**
     * @copydoc ISPC::SetupModule::setup()
     */
    virtual IMG_RESULT setup();

    ModuleCCM& operator= (const ModuleCCM &other);

public:  // parameters
    static const ParamDefArray<double> CCM_MATRIX;
    static const ParamDefArray<double> CCM_OFFSETS;

    static ParameterGroup getGroup();
};

/** @brief Min for ModuleCCM::CCM_MATRIX and TemperatureCorrection::WB_CCM_S */
#define CCM_MATRIX_MIN -3.0
/** @brief Max for ModuleCCM::CCM_MATRIX and TemperatureCorrection::WB_CCM_S */
#define CCM_MATRIX_MAX 3.0
/**
 * @brief Defaults for ModuleCCM::CCM_MATRIX and 
 * TemperatureCorrection::WB_CCM_S
 */
static const double CCM_MATRIX_DEF[9] =
{   1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0
};

/** 
 * @brief Min for ModuleCCM::CCM_OFFSETS and 
 * TemperatureCorrection::WB_OFFSETS_S
 */
#define CCM_OFFSETS_MIN -32768
/** 
 * @brief Max for ModuleCCM::CCM_OFFSETS and 
 * TemperatureCorrection::WB_OFFSETS_S
 */
#define CCM_OFFSETS_MAX 32767
/** 
 * @brief Defaults for ModuleCCM::CCM_OFFSETS and 
 * TemperatureCorrection::WB_OFFSETS_S
 */
static const double CCM_OFFSETS_DEF[3] = { 0, 0, 0 };

} /* namespace ISPC */

#endif /* ISPC_MODULE_CCM_H_ */
