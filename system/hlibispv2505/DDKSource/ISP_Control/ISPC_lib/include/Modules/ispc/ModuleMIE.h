/**
*******************************************************************************
 @file ModuleMIE.h

 @brief Declaration of ISPC::ModuleMIE

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
#ifndef ISPC_MODULE_MIE_H_
#define ISPC_MODULE_MIE_H_

#include <felix_hw_info.h>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Memory Colour part of the Main Image Enhancer (MIE) HW configuration
 *
 * Setup function configures part of the @ref MC_MIE elements of the 
 * MC_PIPELINE attached to the owner Pipeline.
 *
 * The @ref ModuleVIB configures the vibrancy part of the MIE HW block.
 *
 * @note The MIE block in HW has @ref MIE_NUM_MEMCOLOURS (3) memory colours.
 * Each represent a single colour to enhance.
 * There are @ref MIE_NUM_SLICES (4) slices per memory colours in the luma and 
 * chroma space.
 */
class ModuleMIE: public SetupModuleBase<STP_MIE>
{
public:
    /**
     * @name General parameters
     * @{
     */
    /**
     * @brief [normalised] Black level, specified in normalized 0.0 to 1.0
     * space.
     *
     * E.g. black level = 16/256 = 0.0625 (typical black level in 8 bit)
     *
     * Loaded using @ref MIE_BLC
     *
     * @ Could be loaded from CCM configuration
     */
    double fBlackLevel;

    /**
     * @brief Memory Colours on/off
     *
     * Loaded using @ref MIE_ENABLED_S
     */
    bool bEnabled[MIE_NUM_MEMCOLOURS];

    /**
     * @}
     * @name Luma slice parameters
     * @{
     */

    /**
     * @brief [normalised] Memory colour likelihood gain per luma slice
     *
     * A gain of 0 is considered to have no effect.
     * It is not recommended that the sum of all gains goes above 1.
     *
     * @note @ref MIE_NUM_SLICES slices per memory colours.
     *
     * Loaded using @ref MIE_YGAINS_S base name
     */
    double aYGain[MIE_NUM_MEMCOLOURS][MIE_NUM_SLICES];
    /**
     * @brief [normalised] Memory colour likelihood minimum luma
     *
     * Lower luma value of the @ref MIE_NUM_SLICES slices of luma.
     * Used to compute all slices location with @ref aYMax.
     *
     * Loaded using @ref MIE_YMIN_S
     */
    double aYMin[MIE_NUM_MEMCOLOURS];
    /**
     * @brief [normalised] Memory colour likelihood maximum luma
     *
     * Higher luma value of the @ref MIE_NUM_SLICES slices of luma.
     * Used to compute all slices location with @ref aYMin.
     *
     * Loaded using @ref MIE_YMAX_S
     */
    double aYMax[MIE_NUM_MEMCOLOURS];

    /**
     * @}
     * @name Chroma selection and likelyhood
     * @{
     */

    /**
     * @brief [normalised] Memory colour likelihood Chroma Blue centre
     *
     * Loaded using @ref MIE_CCENTER_S
     */
    double aCbCenter[MIE_NUM_MEMCOLOURS];
    /**
     * @brief [normalised] Memory colour likelihood Chroma Red centre
     *
     * Loaded using @ref MIE_CCENTER_S
     */
    double aCrCenter[MIE_NUM_MEMCOLOURS];
    /**
     * @brief Memory colour likelihood Chroma extension per slice
     *
     * @ref MIE_NUM_SLICES slices per memory colour.
     *
     * Loaded using @ref MIE_CEXTENT_S base name
     */
    double aCExtent[MIE_NUM_MEMCOLOURS][MIE_NUM_SLICES];
    /**
     * @brief Memory colour likelihood ellipsoid aspect ratio (log2) in Chroma
     * plane
     *
     * Loaded using @ref MIE_CASPECT_S
     */
    double aCAspect[MIE_NUM_MEMCOLOURS];
    /**
     * @brief Memory colour likelihood ellipsoid rotation in Chroma plane
     *
     * Loaded using @ref MIE_CROTATION_S
     */
    double aCRotation[MIE_NUM_MEMCOLOURS];

    /**
     * @}
     * @name Output transformation
     * @{
     */

    /** @brief Memory colour brightness modification */
    double aOutBrightness[MIE_NUM_MEMCOLOURS];
    /** @brief Memory colour contrast modification */
    double aOutContrast[MIE_NUM_MEMCOLOURS];
    /** @brief Memory colour saturation modification */
    double aOutStaturation[MIE_NUM_MEMCOLOURS];
    /** @brief Memory colour hue modification */
    double aOutHue[MIE_NUM_MEMCOLOURS];

    /**
     * @}
     */

public:  // methods
    ModuleMIE();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);
    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;
    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

    /**
     * @brief This function transforms higher level parameters extent and
     * aspect into register values kb, kr, scale
     *
     * The high level formula is L = exp(|cb|/sigma_b + |cr|/sigma_r)
     *
     * The low level (register) formula is L = exp(scale*(Kb*|cb| + Kr*|cr|)
     *
     * @param[in] aspect
     * @param[in] extent [MIE_NUM_SLICES]
     * @param[out] kb
     * @param[out] kr
     * @param[out] scale [MIE_NUM_SLICES]
     */
    static void calculateMIEExtentParameters(const double aspect,
        const double *extent,  int &kb,  int &kr, double *scale);

public:  // parameters
    static const ParamDef<double> MIE_BLC;

    /** @brief number of memory colours */
    static const ParamDef<unsigned int> MIE_MEMORY_COLOURS;
    // per memory colour
    static const ParamDefSingle<bool> MIE_ENABLED_S;

    static const ParamDef<double> MIE_YMIN_S;
    static const ParamDef<double> MIE_YMAX_S;
    /** Cb and Cr */
    static const ParamDefArray<double> MIE_CCENTER_S;
    /** 4 slice per memory colour */
    static const ParamDefArray<double> MIE_YGAINS_S;
    /** 4 slice per memory colour */
    static const ParamDefArray<double> MIE_CEXTENT_S;
    static const ParamDef<double> MIE_CASPECT_S;
    static const ParamDef<double> MIE_CROTATION_S;
    static const ParamDef<double> MIE_OUT_BRIGHTNESS_S;
    static const ParamDef<double> MIE_OUT_CONTRAST_S;
    static const ParamDef<double> MIE_OUT_SATURATION_S;
    static const ParamDef<double> MIE_OUT_HUE_S;

    static ParameterGroup GetGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_MIE_H_ */
