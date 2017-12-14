/**
*******************************************************************************
 @file ModuleLCA.h

 @brief Declaration of ISPC::ModuleLCA

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
#ifndef ISPC_MODULE_LCA_H_
#define ISPC_MODULE_LCA_H_

#include <felix_hw_info.h>  // LCA_COEFFS_NO

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Lateral Chromatic Aberration (LCA) HW configuration
 *
 * Setup function configures the @ref MC_LCA elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleLCA: public SetupModuleBase<STP_LCA>
{
public:  // attributes
    /**
     * @brief Coefficients of horizontal correction polynomial for red channel 
     * (x,x^2,x^3).
     *
     * Loaded using @ref LCA_REDPOLY_X
     */
    double aRedPoly_X[LCA_COEFFS_NO];
    /**
     * @brief Coefficients of vertical correction polynomial for red channel
     * (x,x^2,x^3).
     *
     * Loaded using @ref LCA_REDPOLY_Y
     */
    double aRedPoly_Y[LCA_COEFFS_NO];
    /**
     * @brief Coefficients of horizontal correction polynomial for blue channel
     * (x,x^2,x^3).
     *
     * Loaded using @ref LCA_BLUEPOLY_X
     */
    double aBluePoly_X[LCA_COEFFS_NO];
    /**
     * @brief Coefficients of vertical correction polynomial for blue channel
     * (x,x^2,x^3).
     *
     * Loaded using @ref LCA_BLUEPOLY_Y
     */
    double aBluePoly_Y[LCA_COEFFS_NO];
    /**
     * @brief [pixels] Position of red channel LCA correction centre, relative
     * to centre of picture.
     *
     * Loaded using @ref LCA_REDCENTER
     */
    IMG_UINT32   aRedCenter[2];
    /**
     * @brief [pixels] Position of blue channel LCA correction centre, relative
     * to centre of picture.
     *
     * Loaded using @ref LCA_BLUECENTER
     */
    IMG_UINT32   aBlueCenter[2];
    /**
     * @brief Right-shift (division) to normalise pre-IIF image width/2 in CFAs
     * to 512. Should be set in tandem with @ref aDecimation
     *
     * Loaded using @ref LCA_SHIFT
     *
     * Could potentially be derived from @ref aDecimation and
     * @ref ModuleIIF::IIF_DECIMATION
     */
    IMG_UINT32   aShift[2];
    /**
     * @brief Decimation compensation multiplier. Should
     * convert image size from post-IIF (decimated) to pre-IIF (sensor).
     * Should be set in tandem with @ref aShift
     *
     * Loaded using @ref LCA_DEC
     *
     * Could potentially be loaded from @ref ModuleIIF::IIF_DECIMATION
     */
    IMG_UINT32   aDecimation[2];

public:  // methods
    ModuleLCA();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameter
    static const ParamDefArray<double> LCA_REDPOLY_X;
    static const ParamDefArray<double> LCA_REDPOLY_Y;
    static const ParamDefArray<double> LCA_BLUEPOLY_X;
    static const ParamDefArray<double> LCA_BLUEPOLY_Y;
    static const ParamDefArray<int> LCA_REDCENTER;
    static const ParamDefArray<int> LCA_BLUECENTER;

    static const ParamDefArray<int> LCA_SHIFT;
    static const ParamDefArray<int> LCA_DEC;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_LCA_H_ */
