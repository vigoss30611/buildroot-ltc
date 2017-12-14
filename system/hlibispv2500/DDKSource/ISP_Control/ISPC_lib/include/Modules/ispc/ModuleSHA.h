/**
*******************************************************************************
 @file ModuleSHA.h

 @brief Declaration of ISPC::ModuleSHA

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
#ifndef ISPC_MODULE_SHA_H_
#define ISPC_MODULE_SHA_H_

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

#define SHA_CURVE_NPOINTS 15

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Sharpening (SHA) HW configuration
 *
 * Setup function configures the @ref MC_SHA elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleSHA: public SetupModuleBase<STP_SHA>
{
public:  // attributes
    /**
     * @name Sharpening module
     * @{
     */
    /**
     * @brief [pixels] sharpening radius in pixels
     *
     * Loaded using @ref SHA_RADIUS
     */
    double fRadius;
    /**
     * @brief Sharpening strength
     * 
     * Loaded using @ref SHA_STRENGTH
     */
    double fStrength;
    /**
     * @brief Sharpening mask threshold
     *
     * Loaded using @ref SHA_THRESH
     */
    double fThreshold;
    /**
     * @brief Sharpening texture vs. edges detail parameter
     *
     * Loaded using @ref SHA_DETAIL
     */
    double fDetail;
    /**
     * @brief Sharpening edge likelihood scale parameter. High values produce 
     * abrupt changes in edge/non-edge likelihood map.
     *
     * Loaded using @ref SHA_EDGE_SCALE
     */
    double fEdgeScale;
    /**
     * @brief Sharpening texture vs. edges detail parameter.
     *
     * Loaded using @ref SHA_EDGE_OFFSET
     */
    double fEdgeOffset;

    /**
     * @}
     * @name Secondary denoiser
     * @{
     */

    /**
     * @brief Enable/disable denoiser part.
     *
     * Loaded using @ref SHA_DENOISE_BYPASS
     */
    bool  bBypassDenoise;
    /**
     * @brief Multiplier to apply to the driver-determined sharpening denoiser
     * edge avoid effort (1.0 = no effect, < 1.0 = less edge avoidance, > 1.0
     * = more edge avoidance).
     *
     * Loaded using @ref SHADN_TAU
     */
    double fDenoiseTau;
    /**
     * @brief Multiplier to apply to sharpening denoiser standard deviation
     * (1.0 = no effect, < 1.0 = less denoising, > 1.0 = more denoising).
     *
     * Loaded using @ref SHADN_SIGMA
     */
    double fDenoiseSigma;

#ifdef INFOTM_ISP
    double sampleNoiseIndexes[SHA_CURVE_NPOINTS];    
    double sampleSigmaValues[SHA_CURVE_NPOINTS];
    double sampleTauValues[SHA_CURVE_NPOINTS];
    double sampleSharpenValues[SHA_CURVE_NPOINTS];
    double sampleSigmaAndTauRatioValue[SHA_CURVE_NPOINTS];
    double sampleAlphaValue[SHA_CURVE_NPOINTS];
    double sampleW0[SHA_CURVE_NPOINTS];
    double sampleW1[SHA_CURVE_NPOINTS];
    double sampleW2[SHA_CURVE_NPOINTS];
    
    double fStrengthFactor;
    bool  bEnableSharpenCurve;
    bool bEnableConfig2ndReg;
    int sampleTau0Reg[SHA_CURVE_NPOINTS];
    int sampleTau1Reg[SHA_CURVE_NPOINTS];
    int sampleTau2Reg[SHA_CURVE_NPOINTS];
    int sampleTau3Reg[SHA_CURVE_NPOINTS];
    int sampleTau4Reg[SHA_CURVE_NPOINTS];
    int sampleTau5Reg[SHA_CURVE_NPOINTS];
    int sampleTau6Reg[SHA_CURVE_NPOINTS];

    int sampleSigma0Reg[SHA_CURVE_NPOINTS];
    int sampleSigma1Reg[SHA_CURVE_NPOINTS];
    int sampleSigma2Reg[SHA_CURVE_NPOINTS];
    int sampleSigma3Reg[SHA_CURVE_NPOINTS];
    int sampleSigma4Reg[SHA_CURVE_NPOINTS];
    int sampleSigma5Reg[SHA_CURVE_NPOINTS];
    int sampleSigma6Reg[SHA_CURVE_NPOINTS];
#endif
    /**
     * @}
     */

public:  // methods
    ModuleSHA();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

public:  // parameters
    static const ParamDef<double> SHA_RADIUS;
    static const ParamDef<double> SHA_STRENGTH;
    static const ParamDef<double> SHA_THRESH;
    static const ParamDef<double> SHA_DETAIL;
    static const ParamDef<double> SHA_EDGE_SCALE;
    static const ParamDef<double> SHA_EDGE_OFFSET;

    static const ParamDefSingle<bool> SHA_DENOISE_BYPASS;
    static const ParamDef<double> SHADN_TAU;
    static const ParamDef<double> SHADN_SIGMA;
#ifdef INFOTM_ISP
    static const ParamDefArray<double> SHA_NOISE_CURVE;
    static const ParamDefArray<double> SHA_SIGMA_CURVE;
    static const ParamDefArray<double> SHA_TAU_CURVE;
    static const ParamDefArray<double> SHA_SHARPEN_CURVE;

    static const ParamDefArray<double> SHA_W0_CURVE;
    static const ParamDefArray<double> SHA_W1_CURVE;
    static const ParamDefArray<double> SHA_W2_CURVE;
    
    static const ParamDefArray<double> SHA_SIGMA_TAU_RATIO_CURVE;
    static const ParamDefArray<double> SHA_ALPHA_CURVE;
    static const ParamDef<double> SHA_STRENGTH_FACTOR;
    static const ParamDefSingle<bool> SHA_SHARPEN_CURVE_ENABLE;

    static const ParamDefSingle<bool> SHA_CONFIG_SIGMA_TAU_BY_REGISTER_VALUE_ENABLE;
    static const ParamDefArray<int> SHA_TAU0_REG_CURVE;
    static const ParamDefArray<int> SHA_TAU1_REG_CURVE;
    static const ParamDefArray<int> SHA_TAU2_REG_CURVE;
    static const ParamDefArray<int> SHA_TAU3_REG_CURVE;
    static const ParamDefArray<int> SHA_TAU4_REG_CURVE;
    static const ParamDefArray<int> SHA_TAU5_REG_CURVE;
    static const ParamDefArray<int> SHA_TAU6_REG_CURVE;

    static const ParamDefArray<int> SHA_SIGMA0_REG_CURVE;
    static const ParamDefArray<int> SHA_SIGMA1_REG_CURVE;
    static const ParamDefArray<int> SHA_SIGMA2_REG_CURVE;
    static const ParamDefArray<int> SHA_SIGMA3_REG_CURVE;
    static const ParamDefArray<int> SHA_SIGMA4_REG_CURVE;
    static const ParamDefArray<int> SHA_SIGMA5_REG_CURVE;
    static const ParamDefArray<int> SHA_SIGMA6_REG_CURVE;
#endif
    static ParameterGroup getGroup();
};

/** @brief Min for ModuleSHA::SHA_STRENGTH and ControlLBC::LBC_SHARPNESS_S */
#define SHA_STRENGTH_MIN 0.0
/** @brief Min for ModuleSHA::SHA_STRENGTH and ControlLBC::LBC_SHARPNESS_S */
#define SHA_STRENGTH_MAX 1.0
/** @brief Min for ModuleSHA::SHA_STRENGTH and ControlLBC::LBC_SHARPNESS_S */
#define SHA_STRENGTH_DEF 0.4

#ifdef INFOTM_ISP
#define SHA_STRENGTH_FACTOR_MIN 0.0
#define SHA_STRENGTH_FACTOR_MAX 1.0
#define SHA_STRENGTH_FACTOR_DEF 0.56
#endif
} /* namespace ISPC */

#endif /* ISPC_MODULE_SHA_H_ */
