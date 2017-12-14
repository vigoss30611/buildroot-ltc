/**
*******************************************************************************
 @file ci_alloc_info.h

 @brief Functions and structures to compute the allocation size for pixel 
 formats

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
#ifndef CI_ALLOC_INFO_H
#define CI_ALLOC_INFO_H

#include <img_types.h>
#include "felixcommon/pixel_format.h"

#ifdef __cplusplus
extern "C" {
#endif

// FELIX_COMMON defined in pixel_format.h
/**
 * @defgroup CI_ALLOC CI Allocation information
 * @brief Allocation information (size and strides) per format
 * @ingroup FELIX_COMMON
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in CI_ALLOC documentation group
 *---------------------------------------------------------------------------*/

/**
 * @note total allocation = ui32Stride*ui32Height + ui32CbStride*ui32CbHeight
 */
struct CI_SIZEINFO
{
    enum pxlFormat eFmt;
    /** @brief buffer stride in Bytes (in YUV case is Y buffer stride) */
    IMG_UINT32 ui32Stride;
    /** @brief buffer height in lines (in YUV case is Y buffer height) */
    IMG_UINT32 ui32Height;

    /** @brief Tiling scheme used - 0 if no tiling */
    IMG_UINT32 ui32TilingStride;

    /** @brief != 0 in YUV case only - combined Cb/Cr buffer stride in Bytes */
    IMG_UINT32 ui32CStride;
    /** @brief != 0 in YUV case only - combined Cb/Cr buffer height in lines */
    IMG_UINT32 ui32CHeight;
};

struct CI_TILINGINFO
{
    /** @brief Tile width in Bytes */
    IMG_UINT32 ui32TilingStride;
    /** @brief Tileheight in lines */
    IMG_UINT32 ui32TilingHeight;
    /** @brief Minimum tiled buffer stride supported */
    IMG_UINT32 ui32MinTileStride;
    /** @brief Maximum tiled buffer stride supported */
    IMG_UINT32 ui32MaxTileStride;
};

/**
 * @brief Access the compile time value for system memory alignment
 *
 * @return [bytes] data structures in system memory must be aligned to an 
 * address which is a multiple of this
 */
IMG_UINT32 CI_ALLOC_GetSysmemAlignment(void);

/**
 * @brief Access the compile time value for minimum tiling stride
 *
 * @return Minimum supported tiling stride for 256x16 scheme (x2 if tiling 
 * scheme is 512x8)
 */
IMG_UINT32 CI_ALLOC_GetMinTilingStride(void);

/**
 * @brief Access the compile time value for maximum tiling stride
 *
 * @return Maximum supported tiling stride for 256x16 scheme (x2 if tiling 
 * scheme is 512x8)
 */
IMG_UINT32 CI_ALLOC_GetMaxTilingStride(void);

/**
 * @brief Get tiling information relative to the used tiling scheme
 *
 * IMG MMU tiling schemes are:
 * @li 256B x 16 lines
 * @li 512B x 8 lines
 *
 * @param[in] ui32TiledScheme the width of the tiling scheme in bytes 
 * (e.g. 256)
 * @param[out] pResult scheme structure to fill - asserts it is not NULL
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if the tiling scheme is not suported!
 */
IMG_RESULT CI_ALLOC_GetTileInfo(IMG_UINT32 ui32TiledScheme, 
    struct CI_TILINGINFO *pResult);

/**
 * @brief Get YUV buffer size information
 *
 * @param[in] pType format - it is not checked if it is YUV
 * @param[in] ui32Width in pixels of the buffer
 * @param[in] ui32Height in lines of the buffer
 * @param[in] pTileInfo tiling information - NULL if not used
 * @param[out] pResult output information about the buffer
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if selected tiling setup needed is not 
 * possible
 */
IMG_RESULT CI_ALLOC_YUVSizeInfo(const PIXELTYPE *pType, IMG_UINT32 ui32Width, 
    IMG_UINT32 ui32Height, const struct CI_TILINGINFO *pTileInfo, 
    struct CI_SIZEINFO *pResult);

/**
 * @brief Get RGB buffer size information
 *
 * @param[in] pType format - it is not checked if it is RGB
 * @note HDR insertion only supports @ref BGR_161616_64 and cannot be tiled
 * @param[in] ui32Width in pixels of the buffer
 * @param[in] ui32Height in lines of the buffer
 * @param[in] pTileInfo tiling information - NULL if not used
 * @warning TILING is not supported by HDR insertion - will return
 * IMG_ERROR_NOT_SUPPORTED if pTileInfo isn't NULL while using 
 * @ref BGR_161616_64!
 * @param[out] pResult output information about the buffer
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if selected tiling setup needed is not 
 * possible
 *
 * @note also works for Bayer (RGGB) formats and HDR extraction (special RGB)
 */
IMG_RESULT CI_ALLOC_RGBSizeInfo(const PIXELTYPE *pType, IMG_UINT32 ui32Width, 
    IMG_UINT32 ui32Height, const struct CI_TILINGINFO *pTileInfo, 
    struct CI_SIZEINFO *pResult);

/**
 * @brief Get Raw 2D buffer size information
 *
 * @param[in] pType format - it is not checked if it is RGB
 * @param[in] ui32Width in pixels of the buffer
 * @param[in] ui32Height in lines of the buffer
 * @param[in] pTileInfo tiling information - NULL if not used
 * @warning TILING is not supported by this format - will return 
 * IMG_ERROR_NOT_SUPPORTED if pTileInfo isn't NULL!
 * @param[out] pResult output information about the buffer
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if selected tiling setup needed is not 
 * possible
 *
 * @note special format which is byte aligned in HW and cannot be tiled
 */
IMG_RESULT CI_ALLOC_Raw2DSizeInfo(const PIXELTYPE *pType, IMG_UINT32 ui32Width,
    IMG_UINT32 ui32Height, const struct CI_TILINGINFO *pTileInfo, 
    struct CI_SIZEINFO *pResult);

/*-----------------------------------------------------------------------------
 * End of CI_ALLOC documentation group
 *---------------------------------------------------------------------------*/
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // CI_ALLOC_INFO_H
