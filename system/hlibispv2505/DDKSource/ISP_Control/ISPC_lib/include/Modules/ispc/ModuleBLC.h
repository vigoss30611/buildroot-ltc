/**
******************************************************************************
 @file ModuleBLC.h

 @brief Declaration of ISPC::ModuleBLC

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
#ifndef ISPC_MODULE_BLC_H_
#define ISPC_MODULE_BLC_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Black Level Correction (BLC) HW configuration
 *
 * Setup function configures the @ref MC_BLC elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * Also modifies the MC_PIPELINE::ui16SystemBlackLevel
 */
class ModuleBLC: public SetupModuleBase<STP_BLC>
{
public:
    /**
     * @brief [4 ints,sensor bit-depth] Sensor black levels R,G,G,B
     *
     * Loaded using @ref BLC_SENSOR_BLACK
     */
    IMG_INT32  aSensorBlack[4];
    /**
     * @brief [int,pipeline reference bit-depth] Internal black level
     *
     * @note among default of 16 for 8b but pipeline is 10b so it is 64
     *
     * Loaded using @ref BLC_SYS_BLACK
     */
    IMG_UINT32  ui32SystemBlack;

//    /**
//     * @brief Structure to fill that contains the Reset Frame information
//     */
//    BLC_RESET_FRAME sResetFrame;
//
//    /**
//     * @brief Structure to fill that contains the Reset Frame information
//     */
//    BLC_RESET_FRAME sFpnFrame;

public:
    ModuleBLC();

    /**
     * @copydoc SetupModule::load()
     */
    virtual IMG_RESULT load(const ParameterList &parameters);
    /**
     * @copydoc SetupModule::save()
     */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;
    /**
     * @copydoc SetupModule::setup()
     */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefArray<int> BLC_SENSOR_BLACK;
    static const ParamDef<int> BLC_SYS_BLACK;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_BLC_H_ */
