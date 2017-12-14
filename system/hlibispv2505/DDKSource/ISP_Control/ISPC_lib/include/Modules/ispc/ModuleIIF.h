/**
*******************************************************************************
 @file ModuleIIF.h

 @brief Declaration of ISPC::ModuleIIF

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
#ifndef ISPC_MODULE_IFF_H_
#define ISPC_MODULE_IFF_H_

#include <felixcommon/pixel_format.h>

#include <string>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

class Sensor;  // declared in ispc/Sensor.h

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Imager Interface (IIF) HW configuration
 *
 * Setup function configures the @ref MC_IIF elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * The setup function also uses information from the Sensor to use as maximum
 * sizes for the cropping and to configure the needed Bayer mosaic.
 */
class ModuleIIF: public SetupModuleBase<STP_IIF>
{
public:
    /**
     * @brief [CFA cells] Horizontal and vertical ratios for image decimation 
     * (1=every CFA, 2=every other...).
     *
     * Loaded using @ref IIF_DECIMATION
     */
    IMG_UINT32 aDecimation[2];
    /**
     * @brief [Bayer image pixels] Top left coordinate of capture rectangle.
     *
     * Loaded using @ref IIF_CAPRECT_TL and sensor information
     */
    IMG_UINT32 aCropTL[2];
    /**
     * @brief [Bayer image pixels] Bottom right coordinate of capture rectangle
     * (excluding).
     *
     * Loaded using @ref IIF_CAPRECT_BR and sensor information
     */
    IMG_UINT32 aCropBR[2];

private:
    bool initialized;

public:  // methods
    static std::string getMosaicString(eMOSAIC mosaic);

    ModuleIIF();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /**
     * @copydoc ISPC::SetupModule::save()
     *
     * @note Save additional parameter @ref IIF_BAYERFMT that is loaded from
     * the Sensor
     */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    // not used as input
    static const ParamDefSingle<std::string> IIF_BAYERFMT;
    static const ParamDefArray<int> IIF_DECIMATION;
    static const ParamDefArray<int> IIF_CAPRECT_TL;
    static const ParamDefArray<int> IIF_CAPRECT_BR;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_IFF_H_ */
