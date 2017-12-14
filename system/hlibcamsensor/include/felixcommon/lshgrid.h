/**
*******************************************************************************
@file lshgrid.h

@brief Lens Shading map file handling

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
#ifndef FELIXCOMMON_LSHGRID_H_
#define FELIXCOMMON_LSHGRID_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup COMMON_LSH Deshading grid manipulations
 * @brief Creation and loading of LSH matrices
 * @ingroup FELIX_COMMON
 * @{
 */

/**
 * @name Deshading grid creation
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in FELIX_COMMON defined in pixel_format.h
 *---------------------------------------------------------------------------*/

#include <img_types.h>
#define LSH_MAT_NO 4
typedef float LSH_FLOAT;

typedef struct LSH_GRID
{
    /**
     * @brief tiles are square. width >= imager_width/tiles_size and
     * height >= imager_height/tile_size
     */
    IMG_UINT16 ui16TileSize;
    /** @brief in "tiles" */
    IMG_UINT16 ui16Width;
    /** @brief in rows of tiles */
    IMG_UINT16 ui16Height;

    /** @brief a ui16Width*ui16Height matrix for each channel */
    LSH_FLOAT *apMatrix[LSH_MAT_NO];
} LSH_GRID;

/**
* @name LSH Matrix manipulation
* @brief Function to create, destroy and populate matrices
* @{
*/

/**
 * @brief Create a matrix - does not fill the matrix
 *
 * Allocates the matrix therefore should be freed with @ref LSH_Free().
 *
 * Delegates to @ref LSH_AllocateMatrix()
 *
 * @param pLSH
 * @param ui16Width of the input image, in CFA
 * @param ui16Height of the input image, in CFA
 * @param ui16TileSize in CFA (power of 2 needed)
 */
IMG_RESULT LSH_CreateMatrix(LSH_GRID *pLSH, IMG_UINT16 ui16Width,
    IMG_UINT16 ui16Height, IMG_UINT16 ui16TileSize);

/**
 * @brief Similar to @ref LSH_CreateMatrix() but get sizes for the matrix
 * not in CFA
 *
 * Allocates the matrix therefore should be freed with @ref LSH_Free()
 *
 * @param pLSH
 * @param ui16GridWidth of the input image, in elements for the matrix
 * @param ui16GridHeight of the input image, in elements for the matrix
 * @param ui16TileSize in CFA (power of 2 needed)
 */
IMG_RESULT LSH_AllocateMatrix(LSH_GRID *pLSH, IMG_UINT16 ui16GridWidth,
    IMG_UINT16 ui16GridHeight, IMG_UINT16 ui16TileSize);

/**
 * @brief Fill a matrix channel using linear interpolation of the 4 corners
 *
 * @param pLSH
 * @param channel to fill
 * @param aCorners TL TR BL BR CE points for the channel
 * (T=Top, L=Left, R=Right, B=Bottom, CE=Center). Center point ignored.
 * @note Given 5 corners to be compatible with @ref LSH_FillBowl()
 */
IMG_RESULT LSH_FillLinear(LSH_GRID *pLSH, IMG_UINT8 channel,
    const float aCorners[5]);

/**
 * @brief Fill a matrix channel using an hyperbowl interpolation of 4 corners
 * and a center point.
 *
 * @param pLSH
 * @param channel to fill
 * @param aCorners TL TR BL BR CE points for the channel
 * (T=Top, L=Left, R=Right, B=Bottom, CE=Center). Center point ignored.
 */
IMG_RESULT LSH_FillBowl(LSH_GRID *pLSH, IMG_UINT8 channel,
    const float aCorners[5]);

/** @brief free deshading matrix if allocated */
void LSH_Free(LSH_GRID *pLSH);

/**
 * @}
 */
/**
 * @name LSH file access
 * @brief Accessing files to load some modules
 * @{
 */

/**
 * @brief Save the matrix as a text file containing floating point values
 * (for preview)
 */
IMG_RESULT LSH_Save_txt(const LSH_GRID *pLSH, const char *filename);

/**
 * @brief Save the matrix in binary format containing floating point values
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if NULL parameters are given
 * @return IMG_ERROR_FATAL if writing to the file failed
 * @return IMG_ERROR_INVALID_PARAMETERS if filename cannot be open
 */
IMG_RESULT LSH_Save_bin(const LSH_GRID *pLSH, const char *filename);

/**
 * @brief Load a binary formated file into a matrix
 *
 * Allocates the matrix therefore should be freed with @ref LSH_Free()
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if NULL parameters are given
 * @return IMG_ERROR_FATAL if reading from the file failed
 * @return IMG_ERROR_NOT_SUPPORTED if the LSH format is not supported
 * @return IMG_ERROR_MALLOC_FAILED if the allocation of a matrix failed
 * @return IMG_ERROR_INVALID_PARAMETERS if filename cannot be open
 */
IMG_RESULT LSH_Load_bin(LSH_GRID *pLSH, const char *filename);

/**
 * @brief Save the matrix as PGM to help preview of the values
 */
IMG_RESULT LSH_Save_pgm(const LSH_GRID *pLSH, const char *filename);

/**
 * @}
 */

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the FELIX_COMMON documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FELIXCOMMON_LSHGRID_H_ */
