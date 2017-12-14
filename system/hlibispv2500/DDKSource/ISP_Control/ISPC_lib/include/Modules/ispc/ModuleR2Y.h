/**
*******************************************************************************
 @file ModuleR2Y.h

 @brief Declaration of ISPC::ModuleR2Y

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
#ifndef ISPC_MODULE_R2Y_H_
#define ISPC_MODULE_R2Y_H_

#include <string>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @brief Used by both ModuleR2Y and ModuleY2R
 */
class ModuleR2YBase
{
public:
    /** @brief Brightness (offset) */
    double fBrightness;
    /** @brief Contrast (luma gain) */
    double fContrast;
    /** @brief Saturation (chroma grain) */
    double fSaturation;
    /** @brief [degrees] Hue shift */
    double fHue;
    /** @brief Gain multipliers for Y,Cb,Cr (before output offset) */
    double aRangeMult[3];
    /** @brief Offset applied to U channel */
    double fOffsetU;
    /** @brief Offset applied to V channel */
    double fOffsetV;

#ifdef INFOTM_ISP
    bool bUseCustomR2YMatrix;
    double fCustomBT709Matrix[3][3];
    double fCustomBT601Matrix[3][3];
    double fCustomJFIFMatrix[3][3];

#endif //INFOTM_ISP
    enum StdMatrix {
        BT601 = 0,
        BT709,
        JFIF /**< @warning Not supported by this module at the moment! */
    };
    /** @brief Type of RGB to YCC matrix to use */
    StdMatrix eMatrix;

    /**
     * @brief Configures the given plane converter according to its
     * @ref MC_PLANECONV::eType
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_SUPPORTED if given matrix standard not found
     * @return IMG_ERROR_INVALID_PARAMETERS if given plane has an unknown 
     * @ref MC_PLANECONV::eType
     */
    IMG_RESULT configure(MC_PLANECONV &plane, bool bSwapInput = false,
        bool bSwapOutput = false) const;
};

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief RGB to YCC convertion (R2Y) HW configuration
 *
 * Setup function configures the @ref MC_R2Y elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 */
class ModuleR2Y: public ModuleR2YBase, public SetupModuleBase<STP_R2Y>
{
public:  // methods
    ModuleR2Y();

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
    static const ParamDefSingle<std::string> R2Y_MATRIX_STD;
    static const ParamDef<double> R2Y_BRIGHTNESS;
    static const ParamDef<double> R2Y_CONTRAST;
    static const ParamDef<double> R2Y_SATURATION;
    static const ParamDef<double> R2Y_HUE;
    static const ParamDef<double> R2Y_OFFSETU;
    static const ParamDef<double> R2Y_OFFSETV;

    static const ParamDefArray<double> R2Y_RANGEMULT;

#ifdef INFOTM_ISP
    static const ParamDefSingle<bool> USE_CUSTOM_R2Y_MATRIX;
    static const ParamDefArray<double> CUSTOM_BT709_MATRIX;
    static const ParamDefArray<double> CUSTOM_BT601_MATRIX;
    static const ParamDefArray<double> CUSTOM_JFIF_MATRIX;

#endif //INFOTM_ISP
    static ParameterGroup getGroup();
};

/**
 * @property ModuleR2YBase::fBrightness
 * For ModuleR2Y loaded using @ref ModuleR2Y::R2Y_BRIGHTNESS
 */
/**
 * @property ModuleR2YBase::fContrast
 * For ModuleR2Y loaded using @ref ModuleR2Y::R2Y_CONTRAST
 */
/**
 * @property ModuleR2YBase::fSaturation
 * For ModuleR2Y loaded using @ref ModuleR2Y::R2Y_SATURATION
 */
/**
 * @property ModuleR2YBase::fHue
 * For ModuleR2Y loaded using @ref ModuleR2Y::R2Y_HUE
 */
/**
 * @property ModuleR2YBase::aRangeMult
 * For ModuleR2Y loaded using @ref ModuleR2Y::R2Y_RANGEMULT
 */
/**
 * @property ModuleR2YBase::eMatrix
 * For ModuleR2Y loaded using @ref ModuleR2Y::R2Y_MATRIX_STD
 */

/**
 * @brief Min for ModuleR2Y::R2Y_BRIGHTNESS and ControlLBC::LBC_BRIGHTNESS_S
 */
#define R2Y_BRIGHTNESS_MIN -0.5
/**
 * @brief Max for ModuleR2Y::R2Y_BRIGHTNESS and ControlLBC::LBC_BRIGHTNESS_S
 */
#define R2Y_BRIGHTNESS_MAX 0.5
/**
 * @brief Default for ModuleR2Y::R2Y_BRIGHTNESS and 
 * ControlLBC::LBC_BRIGHTNESS_S
 */
#define R2Y_BRIGHTNESS_DEF 0.0

/** @brief Min for ModuleR2Y::R2Y_CONTRAST and ControlLBC::LBC_CONTRAST_S */
#define R2Y_CONTRAST_MIN 0.0
/** @brief Max for ModuleR2Y::R2Y_CONTRAST and ControlLBC::LBC_CONTRAST_S */
#define R2Y_CONTRAST_MAX 2.0
/**
 * @brief Default for ModuleR2Y::R2Y_CONTRAST and ControlLBC::LBC_CONTRAST_S
 */
#define R2Y_CONTRAST_DEF 1.0

/**
 * @brief Min for ModuleR2Y::R2Y_SATURATION and ControlLBC::LBC_SATURATION_S
 */
#define R2Y_SATURATION_MIN 0.1
/**
 * @brief Max for ModuleR2Y::R2Y_SATURATION and ControlLBC::LBC_SATURATION_S
 */
#define R2Y_SATURATION_MAX 10.0
/**
 * @brief Default for ModuleR2Y::R2Y_SATURATION and 
 * ControlLBC::LBC_SATURATION_S
 */
#define R2Y_SATURATION_DEF 1.0

/** @brief U or V channel minimum offset (normalised) */
#define R2Y_OFFSETUV_MIN -0.5
/** @brief U or V channel maximum offset (normalised) */
#define R2Y_OFFSETUV_MAX 0.5
/** @brief U or V channel default offset (normalised for RGB to YUV) */
#define R2Y_OFFSETUV_DEF 0
/** @brief U or V channel default offset (normalised for YUV to RGB) */
#define Y2R_OFFSETUV_DEF -0.5

} /* namespace ISPC */

#endif /* ISPC_MODULE_R2Y_H_ */
