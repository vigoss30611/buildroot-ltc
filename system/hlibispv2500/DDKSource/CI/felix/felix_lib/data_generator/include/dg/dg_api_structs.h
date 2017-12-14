/**
 @file dg_api_structs.h

 @brief Data Generator structures

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
#ifndef __DG_API_STRUCTS__
#define __DG_API_STRUCTS__

#include <img_types.h>

#include "felixcommon/pixel_format.h" // bayer format

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup DG_API_Conv
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the DG_API documentation module
 *------------------------------------------------------------------------*/

#define DG_H_BLANKING_SUB (40) ///< @brief HW does blanking = reg + 40

/**
 * @brief supported BT656 and MIPI RAW data formats
 */
typedef enum DG_MEMFMT
{
	DG_MEMFMT_BT656_10,
	DG_MEMFMT_BT656_12,
	DG_MEMFMT_MIPI_RAW10,
	DG_MEMFMT_MIPI_RAW12,
	DG_MEMFMT_MIPI_RAW14
} eDG_MEMFMT;

/**
 * @brief Different types of frame for the MIPI standard
 */
typedef enum DG_MIPI_FRAMETYPE
{
	DG_MIPI_FRAMESTART,
	DG_MIPI_FRAMEEND,
	DG_MIPI_FRAME
} eDG_MIPI_FRAMETYPE;

/// @brief size of the CRC value in the MIPI packet in bytes
#define DG_MIPI_SIZEOF_CRC sizeof(IMG_UINT16)

/**
 * @brief The MIPI packet types Identifiers
 */
typedef enum DG_MIPI_ID
{
	DG_MIPI_ID_FS=0,		/**< @brief short packet - frame start */
	DG_MIPI_ID_FE=1,		/**< @brief short packet - frame end */
	DG_MIPI_ID_LS=2,		/**< @brief short packet - line start */
	DG_MIPI_ID_LE=3,		/**< @brief short packet - line end */
	DG_MIPI_ID_RAW10=0x2b,	/**< @brief long packet - RAW10 data */
	DG_MIPI_ID_RAW12=0x2c,	/**< @brief long packet - RAW12 data */
	DG_MIPI_ID_RAW14=0x2d,	/**< @brief long packet - RAW14 data */
} eDG_MIPI_ID;

/**
 * @brief >= DG_FMT_MIPI is considered MIPI format
 */
typedef enum DG_CONV_FMT
{
	DG_FMT_BT656 = 0,
	DG_FMT_MIPI,
	DG_FMT_MIPI_LF		/**< @brief MIPI - Line Flags */
} DG_CONV_FMT;

/**
 * @brief Converter
 */
typedef struct DG_CONV
{
	DG_CONV_FMT eFormat;			/**< @brief Use BT656, MIPI or MIPI with Line Start and Line End flags - guessed from user format choice in DG_CAMERA */

	eDG_MIPI_ID eMIPIFormat;		/**< @brief Raw MIPI format used - computed according to DG_CAMERA::eBufferFormats */
	IMG_UINT32 ui32FrameNumber;		/**< @brief Current MIPI frame number - 1 based number, 0 at creation */
	IMG_UINT32 ui32LineNumber;		/**< @brief Current MIPI line number - 1 based number, 0 at creation */

	/**
	 * @brief Pointer to one of the convertion function - computed according to DG_CAMERA::eBufferFormats
	 */
	IMG_SIZE (*pfnConverter)(const IMG_UINT16 *inPixels0, const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);
} DG_CONV;

/**
 * @}
 * @ingroup DG_API_Camera
 * @{
 */

typedef struct DG_CAMERA
{
	eDG_MEMFMT eBufferFormats;	///< @brief This can be chosen according to the valid Gasket in Felix's HW info
	eMOSAIC eBayerOrder;		///< @brief Mosaic to use - should be the same in the connected Felix IIF
	IMG_UINT8 ui8DGContext;		/**< @brief Data Gen context to use (0 to CI_N_EXT_DATAGEN-1) */
	IMG_BOOL8 bMIPI_LF;			/**< @brief If MIPI choose to have Line Flags or not */

	IMG_UINT32 uiWidth;		    /**< @brief frame's width in pixels - not used by kernel-side*/
	IMG_UINT32 uiHeight;		/**< @brief frame's height in rows */

	/**
	 * @brief stride in bytes when 1 line is encoded in selected MIPI or BT656
	 * This is computed when registering the Camera.
	 */
	IMG_UINT32 uiStride; 
	/**
	 * @brief Converted width in bytes (because it is different for MIPI than the uiWidth)
	 */
	IMG_UINT32 uiConvWidth;

    /**
     * @brief Horizontal blanking (columns)
     *
     * @note This is a REGISTER value. HW applies blanking as register value + DG_H_BLANKING_SUB
     */
	IMG_UINT16 ui16HoriBlanking;
	IMG_UINT16 ui16VertBlanking; ///< @brief Vertical blanking (lines)

	// these are the values I do not understand yet
	IMG_UINT16 ui16MipiTLPX;
	IMG_UINT16 ui16MipiTHS_prepare;
	IMG_UINT16 ui16MipiTHS_zero;
	IMG_UINT16 ui16MipiTHS_trail;
	IMG_UINT16 ui16MipiTHS_exit;
	IMG_UINT16 ui16MipiTCLK_prepare;
	IMG_UINT16 ui16MipiTCLK_zero;
	IMG_UINT16 ui16MipiTCLK_post;
	IMG_UINT16 ui16MipiTCLK_trail;

	/**
	* @brief Internal converter used to convert input file into DG format
	*
	* @warning User should not modify it.
	*
	* @note Pointer so that kernel-side does not have a copy of it (it is already converted when given to the kernel-side)
	*/
	DG_CONV *pConverter;
} DG_CAMERA;

/**
 * @}
 * @ingroup DG_API_Driver
 * @{
 */

typedef struct DG_HWINFO
{
	IMG_UINT8 config_ui8NDatagen; ///< @brief number of available data generators
} DG_HWINFO;

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the DG_API documentation module
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // __DG_API_STRUCTS__
