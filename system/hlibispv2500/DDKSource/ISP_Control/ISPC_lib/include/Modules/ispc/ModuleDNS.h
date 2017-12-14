/**
*******************************************************************************
 @file ModuleDNS.h

 @brief Declaration of ISPC::ModuleDNS

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
#ifndef ISPC_MODULE_DNS_H_
#define ISPC_MODULE_DNS_H_

#include "ispc/Module.h"
#include "ispc/Parameter.h"

namespace ISPC {

class Sensor;  // defined in ispc/Sensor.h

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Denoise (DNS) HW configuration
 *
 * Setup function configures the @ref MC_DNS elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleDNS: public SetupModuleBase<STP_DNS>
{
public:  // attributes
    /**
     * @brief Combine green channels during denoising
     * 
     * Loaded using @ref DNS_COMBINE
     */
    bool bCombine;
    /**
     * @brief Denoising strength, typically 2-4
     *
     * Loaded using @ref DNS_STRENGTH
     */
    double fStrength;

    /**
     * @brief Pixel threshold - sensitivity to colour differences, typically
     * 0.25
     */
    double fGreyscaleThreshold;

    // loaded from sensor at load time (or parameter file if sensor is 0)
    /**
     * @brief [float,multiplier] Gain applied by the sensor (analogue & 
     * digital), external to ISP
     *
     * Loaded from sensor or using @ref ModuleDNS::DNS_ISOGAIN if sensor is 0
     */
    double fSensorGain;
    /**
     * @brief [int,bits] Bit depth of the sensor
     *
     * Loaded from sensor or using @ref ModuleDNS::DNS_SENSORDEPTH if sensor 
     * is 0
     */
    IMG_UINT32 ui32SensorBitdepth;
    /**
     * @brief [int,electrons] Maximum number of electrons a sensor pixel can 
     * collect before clipping.
     *
     * Loaded from sensor or using @ref ModuleDNS::DNS_WELLDEPTH if sensor is 0
     */
    IMG_UINT32 ui32SensorWellDepth;
    /**
     * @brief [float,electrons] Std.dev of noise when reading pixel value off a
     * sensor.
     *
     * Loaded from sensor or using @ref ModuleDNS::DNS_READNOISE if sensor is 0
     */
    double fSensorReadNoise;

public:  // methods
    ModuleDNS();

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

public:  // parameters
    static const ParamDefSingle<bool> DNS_COMBINE;
    static const ParamDef<double> DNS_STRENGTH;
    /** @note only used by HW from version 2.4 */
    static const ParamDef<double> DNS_GREYSCALE_THRESH;
    /** @warning deprecated - use @ref Sensor::SENSOR_GAIN */
    static const ParamDef<double> DNS_ISOGAIN;
    /** @warning deprecated - use @ref Sensor::SENSOR_BITDEPTH */
    static const ParamDef<int> DNS_SENSORDEPTH;
    /** @warning deprecated - use @ref Sensor::SENSOR_WELLDEPTH */
    static const ParamDef<int> DNS_WELLDEPTH;
    /** @warning deprecated - use @ref Sensor::SENSOR_READNOISE */
    static const ParamDef<double> DNS_READNOISE;

    static ParameterGroup GetGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_DNS_H_ */
