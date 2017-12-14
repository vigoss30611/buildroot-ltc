/**
******************************************************************************
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
#ifndef FELIXCOMMON_LSHGRID_H
#define FELIXCOMMON_LSHGRID_H

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
/*-------------------------------------------------------------------------
 * Following elements are in FELIX_COMMON defined in pixel_format.h
 *------------------------------------------------------------------------*/

#include <img_types.h>
#define LSH_MAT_NO 4
#define LSH_FLOAT float

typedef struct LSH_GRID
{
	IMG_UINT16 ui16TileSize; ///< @brief tiles are square. width >= imager_width/tiles_size and height >= imager_height/tile_size
	IMG_UINT16 ui16Width; ///< @brief in "tiles"
	IMG_UINT16 ui16Height; ///< @brief in rows of tiles

	LSH_FLOAT *apMatrix[LSH_MAT_NO]; ///< @brief a ui16Width*ui16Height matrix for each channel
} LSH_GRID;

/**
 * @name LSH Matrix manipulation
 * @brief Function to create, destroy and populate matrices
 * @{
 */

/**
 * @brief Create a matrix - does not fill the matrix
 *
 * @param pLSH
 * @param ui16Width of the input image, in CFA
 * @param ui16Height of the input image, in CFA
 * @param ui16TileSize in CFA
 */
IMG_RESULT LSH_CreateMatrix(LSH_GRID *pLSH, IMG_UINT16 ui16Width, IMG_UINT16 ui16Height, IMG_UINT16 ui16TileSize);

IMG_RESULT LSH_FillLinear(LSH_GRID *pLSH, IMG_UINT8 channel, const float aCorners[5]);
IMG_RESULT LSH_FillBowl(LSH_GRID *pLSH, IMG_UINT8 channel, const float aCorners[5]);

/// @brief free deshading matrix if allocated
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
 * @brief Save the matrix as a text file containing floating point values (for preview)
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
/*-------------------------------------------------------------------------
 * End of the FELIX_COMMON documentation module
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // FELIXCOMMON_LSHGRID_H
