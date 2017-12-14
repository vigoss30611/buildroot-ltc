/**
******************************************************************************
 @file ISPCDefs.h

 @brief Declaration of functions globally used in several elements of ISPC

 Legacy from ISPC v1 - needs revision

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
#ifndef ISPC_ISPCDEFS_H_
#define ISPC_ISPCDEFS_H_

#ifdef WIN32
#define NOMINMAX
#endif
#include <cmath>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif
#include <limits>
#include <img_types.h>
#include <felixcommon/pixel_format.h>

namespace ISPC {

/** creates a lot of comparison errors if using std::min */
#define ispc_min(a, b) ((a < b) ? a : b)
/** creates a lot of comparison errors if using std::max */
#define ispc_max(a, b) ((a > b) ? a : b)

/** @brief clip a value between 2 bounds */
template <typename T, typename R>
T clip(T value, R bound1, R bound2)
{
    if (bound2 < bound1)
    {
        const R tmp = bound1;
        bound1 = bound2;
        bound2 = tmp;
    }
    return ispc_min(ispc_max(bound1, value), bound2);
}

// Some versions of VisualStudio don't have log2()
#ifdef USE_MATH_NEON
inline double log2(double val) { return ((double)log10f_neon((float)(val)))/((double)log10f_neon(2.0f)); }
#else
inline double log2(double val) { return log10(val)/log10(2.0); }
#endif

/**
 * @brief Portable NaN check for floats
 * @note for example android does not support std::isnumber()
 */
template<typename T>
inline bool isnan(T x) {
    return !(x == x);
}

/**
 * @brief Portable test for finite and not NaN float
 * @note for example android does not support std::isnumber()
 */
template <typename T>
inline bool isfinite(T x) {
    return (x <= std::numeric_limits<T>::max() &&
            x >= -std::numeric_limits<T>::max());
}

/**
 * @brief Global setup parameters that may be used in several places of the
 * pipeline.
 */
struct Global_Setup
{
    IMG_UINT32 CFA_WIDTH;
    IMG_UINT32 CFA_HEIGHT;

   /**
    * @name Sensor information
    * @{
    */

    /** @brief Input image width */
    IMG_UINT32 ui32Sensor_width;
    /** @brief Input image height */
    IMG_UINT32 ui32Sensor_height;
    /** @brief Input image bitdepth */
    IMG_UINT32 ui32Sensor_bitdepth;
    /** @brief IIF setup, imager ID */
    IMG_UINT32 ui32Sensor_imager;
    /** @brief sensor gain - may change regularly */
    double fSensor_gain;
    IMG_UINT32 ui32Sensor_well_depth;
    /** @brief [enum] Layout of the CFA cells. One of:RGGB,GRBG,GBRG,BGGR. */
    eMOSAIC eSensor_bayer_format;
    double fSensor_read_noise;

    /**
     * @}
     * @name Image size information
     * @note Managed by IIF module
     * @{
     */
    /* should it be removed because managed by IIF? or should IIF become
     * part of global? */

    /**
     * @brief Image width after decimation and cropping
     *
     * @note Derived from result of ModuleIIF::setup()
     */
    IMG_UINT32 ui32ImageWidth;
    /**
     * @brief Image height after decimation and cropping
     *
     * @note Derived from result of ModuleIIF::setup()
     */
    IMG_UINT32 ui32ImageHeight;

    IMG_UINT32 ui32BlackBorder;

    /**
     * @}
     * @name Pipeline output width and height
     * @note Output size after the scaler
     * @{
     */

    /** @brief Encoder output width */
    IMG_UINT32 ui32EncWidth;
    /** @brief Encoder output height */
    IMG_UINT32 ui32EncHeight;

    /** @brief Display output width */
    IMG_UINT32 ui32DispWidth;
    /** @brief Display output height */
    IMG_UINT32 ui32DispHeight;

    /**
     * @}
     * @name Other global parameters
     * @{
     */

    /**
     * @brief desired sysmte black level in pipeline bitdepth
     *
     * Usually 64, Parameters coming from BLC [int,pipeline ref.bd]
     *
     * Internal black level (default 16 ~8-bit). Range:0..1024.
     */
    IMG_UINT32 ui32SystemBlack;

    /**
     * @}
     */
};

/**
 * @brief context status definitions
 */
enum CtxStatus {
    ISPC_Ctx_UNINIT = 0, /**< @brief Context not initialited */
    ISPC_Ctx_INIT, /**< @brief Context initialited */
    ISPC_Ctx_SETUP, /**< @brief Pipeline has been setup */
    ISPC_Ctx_READY, /**< @brief Pipeline is ready for capture */
    ISPC_Ctx_ERROR /**< @brief Pipeline is in error state */
};

enum ScalerRectType {
    SCALER_RECT_CLIP = 0,
    SCALER_RECT_CROP,
    SCALER_RECT_SIZE
};

} /* namespace ISPC */

#endif /* ISPC_ISPCDEFS_H_ */
