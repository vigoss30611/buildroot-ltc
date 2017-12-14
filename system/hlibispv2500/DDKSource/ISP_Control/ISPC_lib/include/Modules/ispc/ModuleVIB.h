/**
*******************************************************************************
 @file ModuleVIB.h

 @brief Declaration of ISPC::ModuleVIB

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
#ifndef ISPC_MODULE_VIB_H_
#define ISPC_MODULE_VIB_H_

#include <felix_hw_info.h>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Vibrancy part of the Main Image Enhancer (MIE) HW block
 *
 * Setup function configures the vibrancy related elements of @ref MC_MIE
 * from the MC_PIPELINE attached to the owner Pipeline.
 *
 * The setup also uses the information from @ref ModuleMIE::MIE_BLC
 */
class ModuleVIB: public SetupModuleBase<STP_VIB>
{
public:  // attributes
    /**
     * @brief Vibrancy on/off
     *
     * Loaded using @ref VIB_ON
     */
    bool bVibrancyOn;
    /**
     * @brief The vibrancy saturation curve
     *
     * Loaded using @ref VIB_SATURATION_CURVE
     */
    double aSaturationCurve[MIE_SATMULT_NUMPOINTS];

public:  // methods
    ModuleVIB();

    /**
     * @brief Generate a vibrancy cuve using 2 in/out points
     *
     * This code is from CSIM setup function and is not used by the driver
     * but demonstrate how a curve of gains could be computed from a saturation
     * curve defined by 4 points (0;0)-(in1;out1)-(in2;out2)-(1;1)
     *
     * @param in1 in (0..1)
     * @param out1 in (0..1)
     * @param in2 in (0..1)
     * @param out2 in (0..1)
     * @param[out] curve [MIE_SATMULT_NUMPOINTS]
     */
    static void setupDoublePoints(double in1, double out1, double in2,
        double out2, double *curve);

    /**
     * @brief Example of generating a non-filtered saturation curve using 2
     * points (assuming existance of (0.0;0.0) and (1.0;1.0)
     *
     * Generate a 3 linear segments curve from (0.0;0.0) to point 1, point 1
     * to point 2 and point 2 to (1.0;1.0).
     *
     * If in2 > in1 the values will be swapped.
     *
     * @param in1 point 1 input position (0..1)
     * @param out1 point 1 output position (0..1)
     * @param in2 point 2 input position (0..1)
     * @param out2 point 2 output position (0..1)
     * @param[out] satCurve [MIE_SATMULT_NUMPOINTS]
     */
    static void saturationCurve(double in1, double out1, double in2,
        double out2, double *satCurve);

    /**
     * @brief Transform a saturation curve into gains (satOut/satIn=gain)
     *
     * The expected satCurve should be from (0.0;0.0) to (1.0;1.0)
     *
     * @param[in] satCurve [MIE_SATMULT_NUMPOINTS] satOut values evenly sampled
     * from [0..1]
     * @param[out] satGains [MIE_SATMULT_NUMPOINTS] of gains for given input
     */
    static void gainsFromSaturationCurve(const double *satCurve,
        double *satGains);

    /** @brief Set a curve of s points to identity 1:1 */
    static void toIndentity(double *curve, int s);

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDefSingle<bool> VIB_ON;
    static const ParamDefArray<double> VIB_SATURATION_CURVE;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_VIB_H_ */
