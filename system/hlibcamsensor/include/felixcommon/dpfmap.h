/**
******************************************************************************
 @file dpfmap.h

 @brief Handle the Defective Pixel map format

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
#ifndef FELIXCOMMON_DPFMAP_H
#define FELIXCOMMON_DPFMAP_H

#include <img_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup COMMON_DPF Defective pixel loading
 * @brief Accessing files to load some modules
 * @ingroup FELIX_COMMON
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in FELIX_COMMON defined in pixel_format.h
 *------------------------------------------------------------------------*/

// map information: coordinates (input & output map)
#define DPF_MAP_COORD_OFFSET (0x0)

#define DPF_MAP_COORD_X_COORD_MASK (0x00001FFF)
#define DPF_MAP_COORD_X_COORD_LSBMASK (0x00001FFF)
#define DPF_MAP_COORD_X_COORD_SHIFT (0)
#define DPF_MAP_COORD_X_COORD_LENGTH (13)
#define DPF_MAP_COORD_X_COORD_SIGNED_FIELD (IMG_FALSE)

#define DPF_MAP_COORD_Y_COORD_MASK (0x1FFF0000)
#define DPF_MAP_COORD_Y_COORD_LSBMASK (0x00001FFF)
#define DPF_MAP_COORD_Y_COORD_SHIFT (16)
#define DPF_MAP_COORD_Y_COORD_LENGTH (13)
#define DPF_MAP_COORD_Y_COORD_SIGNED_FIELD (IMG_FALSE)

#define DPF_MAP_INPUT_SIZE (4) ///< @brief Size in bytes per DPF input map entry
#define DPF_MAP_OUTPUT_SIZE (8) ///< @brief Size in bytes per DPF output map entry

#define DPF_MAP_MAX_PER_LINE (32) ///< @brief Maximum number of map entries per sensor line

/**
 * @brief Load a map in HW format
 */
IMG_RESULT DPF_Load_bin(IMG_UINT16 **ppDefect, IMG_UINT32 *pNDefects, const char *filename);

 /**
 * @}
 */ 

 /*-------------------------------------------------------------------------
 * End of the FelixCommon documentation module
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // FELIXCOMMON_DPFMAP_H