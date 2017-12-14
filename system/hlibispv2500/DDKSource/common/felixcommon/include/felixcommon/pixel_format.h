/**
******************************************************************************
 @file pixel_format.h

 @brief Definition of pixel formats and associated functions

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
#ifndef FELIX_COMMON_PIXEL
#define FELIX_COMMON_PIXEL

#include <img_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __maybe_unused
#define __maybe_unused
#endif

/**
 * @defgroup FELIX_COMMON Common elements
 * @brief Common elements for different libraries, such as the pixel format, 
 * generic buffer transformation and specialised binary file storage
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in FELIX_COMMON documentation group
 *---------------------------------------------------------------------------*/

/**
 * @brief high level formats
 */
typedef enum pxlFormat
{
    /**
     * @brief Invalid pixel format
     */
    PXL_NONE = 0,
    
    /**
     * @brief 8b 420, VU interleaved - also known as NV21
     */
    YVU_420_PL12_8,
    /**
     * @brief 8b 420, UV interleaved - also known as NV12
     */
    YUV_420_PL12_8,
    /**
     * @brief 8b 422, VU interleaved - also known as NV61
     */
    YVU_422_PL12_8,
    /**
     * @brief 8b 422, UV intereaved - also known as NV16
     */
    YUV_422_PL12_8,
    /**
     * @brief 10b (3 packed in 32b) 420, VU interleaved - also known as 
     * NV21-10bit
     */
    YVU_420_PL12_10,
    /**
     * @brief 10b (3 packed in 32b) 420, UV interleaved - also known as 
     * NV12-10bit
     */
    YUV_420_PL12_10,
    /**
     * @brief 10b (3 packed in 32b) 422, VU interleaved - also known as 
     * NV61-10bit
     */
    YVU_422_PL12_10,
    /**
     * @brief 10b (3 packed in 32b) 422, UV interleaved - also known as 
     * NV16-10bit
     */
    YUV_422_PL12_10,
    
    /**
     * @brief 8b per channel in 24b RGB - also known as BI_RGB24 (B in LSB)
     */
    RGB_888_24,
    /**
     * @brief 8b per channel in 32b - also known as BI_RGB32 or RGBA (with 
     * empty alpha channel and B in LSB)
     */
    RGB_888_32,
    /**
     * @brief 10b per channel in 32b: 0-9 B, 10-19 G 20-29 R
     */
    RGB_101010_32,
    /**
     * @brief 8b per channel in 24b BGR - inverse of @ref RGB_888_24 (R in LSB)
     */
    BGR_888_24,
    /**
     * @brief 8b per channel in 32b BGR - inverse of @ref RGB_101010_32 (R in 
     * LSB)
     */
    BGR_888_32,
    /**
     * @brief 10b per channel 32b: 0-9 R, 10-19 G 20-29 B - inverse of 
     * @ref RGB_101010_32 (R in LSB)
     */
    BGR_101010_32,
    /**
     * @brief 16b per channel 64b: 0-15 R, 16-31 G, 32-47 B - used for HDF 
     * insertion only
     */
    BGR_161616_64,
    
    /**
     * @brief 8b Bayer (2x2 CFA, pattern is RGGB or a variant) - also known as
     * BA81
     */ 
    BAYER_RGGB_8,
    /**
     * @brief 10b Bayer (2x2 CFA, pattern is RGGB or a variant)
     */
    BAYER_RGGB_10,
    /**
     * @brief 12b Bayer (2x2 CFA, pattern is RGGB or a variant)
     */
    BAYER_RGGB_12,
    /**
     * @brief 10b Bayer TIFF order (pattern is RGGB or a variant)
     */
    BAYER_TIFF_10,
    /**
     * @brief 12b Bayer TIFF order (pattern is RGBB or a variant)
     */
    BAYER_TIFF_12,

    /**
     * @brief 8 bit YUV packed - also known as IYU2 ((24 bit YUV)
     *
     * @note represents mode 0 of the IEEE 1394 Digital Camera 1.04 spec
     * or YUV444Packed in IIDC2 Digital Camera Control Specification
     * Ver.1.1.0
     *
     * @note described in 'Imagination Technologies.Video Pixel Formats' doc
     */
    PXL_ISP_444IL3YCrCb8,

    /**
     * @brief 8 bit YVU packed , note different order of U and V
     *
     * @note described in 'Imagination Technologies.Video Pixel Formats' doc
     */
    PXL_ISP_444IL3YCbCr8,

    /**
     * @brief 10 bit YUV packed (3 packed in 32b)
     *
     * @note described in 'Imagination Technologies.Video Pixel Formats' doc
     */
    PXL_ISP_444IL3YCrCb10,

    /**
     * @brief 10 bit YVU packed (3 packed in 32b)
     * note different order of U and V
     *
     * @note described in 'Imagination Technologies.Video Pixel Formats' doc
     */
    PXL_ISP_444IL3YCbCr10,

    /**
     * @brief Number of pixels formats - not a valid pixel format!
     */
    PXL_N,

    /**
     * @brief Invalid pixel format
     */
    PXL_INVALID,


} ePxlFormat;

/*
 * low level format
 */

/**
 * @brief Buffers that can be used by ISP
 *
 * @note We may decide to replace this with types from img_common.h , but for 
 * now, due to the usage of the DG formats as well as bayer these handled 
 * separately.
 *
 */
typedef enum FORMAT_TYPES
{
    TYPE_NONE = 0,
    TYPE_RGB,   /**< @brief RGB data (used for display/3d core) */
    TYPE_YUV,   /**< @brief PL12 YUV data for use by video hardware */
    TYPE_BAYER,    /**< @brief Bayer format used when doing data extraction */
} eFORMAT_TYPES;

/**
 * @brief Packed data position
 */
typedef enum PACKEDSTART
{
    PACKED_LSB = 0, /**< @brief Least Significant Bits */
    PACKED_MSB         /**< @brief Most Significant Bits */
} ePACKEDSTART;

/**
 * @brief Bayer patterns (2x2 RGGB variants)
 */
typedef enum MOSAICType
{
    MOSAIC_NONE = 0,
    MOSAIC_RGGB,
    MOSAIC_GRBG,
    MOSAIC_GBRG,
    MOSAIC_BGGR
} eMOSAIC;

/**
 * @brief Structure containing the information about a pixel format
 *
 * @li 420 PL12 8b (NV12;NV21):{ YUV,2,2 } + { 8,1,1,LSB }
 * @li 420 PL12 10b (NV12-10;NV21-10): { YUV,2,2 } + { 10,3,4,LSB }
 * @li 422 PL12 8b (NV16;NV61): { YUV,2,1 } + { 8,1,1,LSB }
 * @li 422 PL12 10b (NV16-10;NV61-10): { YUV,1,2 } + { 10,3,4,LSB }
 * @li RGB888-24: { RGB,1,1 } + { 8,1,3,LSB }
 * @li RGB888-32: { RGB,1,1 } + { 8,1,4,LSB } or BGR888-32
 * @li RGB101010: { RGB,1,1 } + { 10,1,4,LSB }
 *
 */
typedef struct PIXELTYPE
{
    /**
     * @brief if copied from a format it is stored here
     */
    enum pxlFormat eFmt;

    // access layer
    /**
     * @brief Buffer type
     */
    eFORMAT_TYPES eBuffer;
    /**
     * @brief Mosaic used - only if the type is Bayer
     */
    eMOSAIC eMosaic;
    /**
     * @brief horizontal subsampling
     */
    IMG_UINT8 ui8HSubsampling;
    /**
     * @brief vertical subsampling
     */
    IMG_UINT8 ui8VSubsampling;

    // packing layer
    /**
     * @brief in bits
     */
    IMG_UINT8 ui8BitDepth;
    /**
     * @brief number of elements represented
     */
    IMG_UINT8 ui8PackedElements;
    /**
     * @brief in Bytes
     */
    IMG_UINT8 ui8PackedStride;
    /**
     * @brief starts LSB or MSB
     */
    ePACKEDSTART ePackedStart;
    
} PIXELTYPE;

/**
 * @name Pixel structure from enum
 * @brief Functions that transform enums to PIXELTYPE structures
 * @{
 */

/**
 * @brief Configures a PIXELTYPE structure using a high-level YUV format
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if the format is not recognised
 */
IMG_RESULT PixelTransformYUV(PIXELTYPE *pType, enum pxlFormat yuv);

/**
 * @brief Configures a PIXELTYPE structure using a high-level RGB format
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if the format is not recognised
 */
IMG_RESULT PixelTransformRGB(PIXELTYPE *pType, enum pxlFormat rgb);

/**
 * @brief Configures a PIXELTYPE structure using a high-level Bayer format 
 * - the mosaic is not used
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if the format is not recognised
 */
IMG_RESULT PixelTransformBayer(PIXELTYPE *pType, enum pxlFormat bayer, 
    enum MOSAICType mosaic);

/**
 * @brief Get the format string (4CC like) for a format
 *
 * @param ePxl pixel format
 */
const char* FormatString(enum pxlFormat ePxl);

#ifdef INFOTM_ISP
//by dirac
const char* FormatString_two_params(enum pxlFormat ePxl, IMG_BOOL8 bVU);
#endif //INFOTM_ISP

/**
 * @brief Get the format string for a mosaic
 *
 * @param mos pixel mosaic
 */
const char* MosaicString(enum MOSAICType mos);

eMOSAIC MosaicFlip(eMOSAIC input, IMG_BOOL8 bFlipH, IMG_BOOL8 bFlipV);

/**
 * @}
 */
/**
 * @name Buffer transformations
 * @brief Transform buffers to different formats
 * @{
 */

/**
 * @brief Dummy up-sampler (and de-interlace chroma) a YUV format to YUV 444 
 * (duplicate pixels)
 */
unsigned char* BufferTransformYUVTo444(IMG_UINT8 *bufferA, IMG_BOOL8 bUV, 
    ePxlFormat eTypeA, size_t inWidth, size_t inHeight, size_t outWidth, 
    size_t outHeight);

// 
// conversion matrix & offset
// each line:
// out = coeff[0]*(Y+offset[0]) + coeff[1]*(Cb+offset[1]) 
// + coeff[2]*(Cr+offset[2])
//

/**
 * @brief BT 709 YUV to RGB conversion matrix
 * @note From: BT.709 from Video Demystified 3rd edition, Keith Jack
 * @see BT709_Inputoff[] populated in pixel_transform.c
 */
extern const double BT709_coeff[9];

/**
 * @brief BT 709 YUV to RGB conversion offsets
 * @note From: BT.709 from Video Demystified 3rd edition, Keith Jack
 * @see BT709_coeff populated in pixel_transform.c
 */
extern const double BT709_Inputoff[3];

/**
 * @brief BT 601 YUV to RGB conversion matrix
 * @note From: BT.601 from Video Demystified 3rd edition, Keith Jack
 * @see BT601_Inputoff populated in pixel_transform.c
 */
extern const double BT601_coeff[9];

/**
 * @brief BT 601 YUV to RGB conversion offsets
 * @note From: BT.601 from Video Demystified 3rd edition, Keith Jack
 * @see BT601_coeff populated in pixel_transform.c
 */
extern const double BT601_Inputoff[3];

/**
 * @brief JFIF YUV to RGB conversion matrix
 * @note From: JFIF from http://www.w3.org/Graphics/JPEG/jfif3.pdf (JPEG File 
 * Exchange Format, version 1.02, Eric Hamilton)
 * @see JFIF_Inputoff populated in pixel_transform.c
 */
extern const double JFIF_coeff[9];

/**
 * @brief JFIF YUV to RGB conversion offsets
 * @note From: JFIF from http://www.w3.org/Graphics/JPEG/jfif3.pdf (JPEG File 
 * Exchange Format, version 1.02, Eric Hamilton)
 * @see JFIF_coeff populated in pixel_transform.c
 */
extern const double JFIF_Inputoff[3];

/**
 * @brief Transformation of YUV 444 to RGB using a matrix
 *
 * @see JFIF_coeff, BT601_coeff or BT709_coeff as standard matrices
 */
unsigned char* BufferTransformYUV444ToRGB(
    IMG_UINT8 *bufferA, IMG_BOOL8 bUV, enum pxlFormat rgb, IMG_BOOL8 bBGR, 
    size_t width, size_t height, const double convMatrix[9], 
    const double convInputOff[3]);

/**
 * @brief De-tile a buffer tiled by HW
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pInput or pOutput is NULL
 * @return IMG_ERROR_INVALID_PARAMETERS if ui32TileWidth, ui32TileHeight, 
 * ui32TileStride, ui32OutputHStride or ui32OutputVStride is 0
 * @return IMG_ERROR_INVALID_PARAMETERS if ui32TileStride > ui32OutputHStride 
 * or ui32TileWidth > ui32TileStride
 * @return IMG_ERROR_NOT_SUPPORTED if ui32OutputHStride is not a multiple of 
 * ui32TileWidth
 * @return IMG_ERROR_NOT_SUPPORTED if ui32TileStride is not a multiple of 
 * ui32TileWidth or ui32OutputHStride is not a multiple of ui32TileStride
 */
IMG_RESULT BufferDeTile(IMG_UINT32 ui32TileWidth, IMG_UINT32 ui32TileHeight, 
    IMG_UINT32 ui32TileStride, const IMG_UINT8 *pInput, IMG_UINT8 *pOutput, 
    IMG_UINT32 ui32OutputHStride, IMG_UINT32 ui32OutputVStride);

/**
 * @}
 */

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the FelixCommon documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif // FELIX_COMMON_PIXEL
