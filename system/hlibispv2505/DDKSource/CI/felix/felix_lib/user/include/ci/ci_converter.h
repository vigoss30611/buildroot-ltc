/**
*******************************************************************************
 @file ci_converter.h

 @brief User-side frame converter for data-generator

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
#ifndef CI_CONVERTER_H_
#define CI_CONVERTER_H_

#include <img_types.h>

#include "ci/ci_api_structs.h"  // CI_DGFRAME

#ifdef __cplusplus
extern "C" {
#endif

struct _sSimImageIn_;  /* defined in sim_image.h */

/**
 * @name Internal Data Generator frame converter
 * @ingroup CI_API_IIFDG
 * @brief Functions to help convert images to correct pixel format
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IIF Converter functions group
 *---------------------------------------------------------------------------*/

/**
 * @brief Converter object that will convert frames into data-generator
 * formats
 *
 * Can be memset to 0 as an initialisation.
 */
typedef struct CI_CONVERTER
{
    CI_CONV_FMT eFormat;
    IMG_UINT8 ui8FormatBitdepth;

    /**
    * @brief Pointer to one of the conversion function. Should convert 1 line
    * of data
    *
    * @warning Should not be NULL
    *
    * @param[in] privateData from the converter
    * @param[in] inPixels input data (can be NULL when computing sizes)
    * @param[in] inPixelStride distance between 2 pixels in the input in bytes
    * @param[in] NpixelsPerCh number of pixels to convert (width)
    * @param[out] outPixels output data (can be NULL when computing sizes)
    *
    * @return number of written bytes
    */
    IMG_SIZE(*pfnLineConverter)(void *privateData, const IMG_UINT16 *inPixels,
        IMG_UINT32 inPixelStride, IMG_UINT32 NpixelsPerCh,
        IMG_UINT8 *outPixels);

    /**
     * @brief Pointer to a conversion function to write a frame header
     *
     * Currently used to write the MIPI header prior to frame data.
     * Should be NULL for parallel formats.
     *
     * @param[in] privateData from the converter
     * @param[out] out where to write (can be NULL when computing sizes)
     *
     * @return number of written bytes
     */
    IMG_SIZE(*pfnHeaderWriter)(void *privateData, IMG_UINT8 *out);

    /**
     * @brief Pointer to a conversion function to write a frame footer
     *
     * Currently used to write the MIPI footer after the frame data.
     * Should be NULL for parallel formats.
     *
     * @param[in] privateData from the converter
     * @param[out] out where to write (can be NULL when computing sizes)
     */
    IMG_SIZE(*pfnFooterWriter)(void *privateData, IMG_UINT8 *out);

    /**
     * @brief additional field that can be used by the converter function
     *
     * When using MIPI it is @ref CI_CONV_MIPI_PRIV, NULL when using parallel
     */
    void *privateData;
} CI_CONVERTER;

/**
 * @brief When configuring a converter to use MIPI this will be dynamically
 * allocated and attached to the converter.
 *
 * User can change the virtual channel and the frame number that will be
 * encoded in the mipi packets.
 *
 * The @ref CI_ConverterConvertFrame() should update the line number.
 */
typedef struct CI_CONV_MIPI_PRIV
{
    /**
     * @brief using line-start and line-end packets
     *
     * Update at configure time
     */
    IMG_BOOL8 bLongFormat;
    /**
     * @brief associated virtual channel
     *
     * Update at configure time
     */
    IMG_UINT8 ui8VirtualChannel;

    /** @brief MIPI data-format identifier to write in data header */
    IMG_UINT8 ui8DataId;
    /**
     * @brief Similar to @ref CI_CONVERTER::pfnLineConverter but only for
     * MIPI data
     *
     * MIPI line conversion has:
     * @li line header (with optional line start if bLongFormat)
     * @li line data
     * @li line footer (CRC + optional line end if bLongFormat)
     *
     * This function only converts the line data. If NULL is given to
     * outPixels does not write but returns the size.
     */
    IMG_SIZE(*pfnMipiWrite)(const IMG_UINT16 *inPixels,
        IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);

    /**
     * @brief current frame number (should start at 1)
     *
     * Update for every frame.
     */
    IMG_UINT16 ui16FrameNumber;
    /**
     * @brief current line number (1 based) - updated by converter!
     */
    IMG_INT32 i32LineNumber;
} CI_CONV_MIPI_PRIV;

/**
 * @brief Configure the DG frame converter according to the pixel format.
 *
 * Uses functions from ci_converter.c to setup the function pointers.
 *
 * Some format will allocate memory in the privateData field and
 * @ref CI_ConverterClear() should be called to liberate the memory
 */
IMG_RESULT CI_ConverterConfigure(CI_CONVERTER *pConverter,
    CI_CONV_FMT eFormat, IMG_UINT8 ui8Bitdepth);

/**
 * @brief Converts a frame from a loaded image. The converter should already
 * be configured to the correct format.
 *
 * This function can be used as an example on how to convert a frame to the
 * correct format.
 *
 * @param[in] pConverter configured for expected format
 * @param[in] pImage source FLX image
 * @param[in,out] pFrame frame to write to
 */
IMG_RESULT CI_ConverterConvertFrame(CI_CONVERTER *pConverter,
    const struct _sSimImageIn_ *pImage, CI_DG_FRAME *pFrame);

/**
 * @brief Returns the encoded frame size including headers and footers
 *
 * If used with ui32Height = 1 will return the biggest possible stride
 * (it is assumed that the format cannot have both header AND footer
 * on 1 stride).
 *
 * Stride is rounded up to the system alignment and then multiplied by
 * ui32Height to give the size
 *
 * @param[in] pConverter
 * @param[in] ui32Width in pixels
 * @param[in] ui32Height in lines - give 1 to compute the stride
 */
IMG_UINT32 CI_ConverterFrameSize(const CI_CONVERTER *pConverter,
    IMG_UINT32 ui32Width, IMG_UINT32 ui32Height);

/**
 * @brief If converter has been configured has private data it will free it.
 */
IMG_RESULT CI_ConverterClear(CI_CONVERTER *pConverter);

/**
 * @brief Converts a FLX image to the HDR insertion format
 *
 * @note Not part of the internal DG but implemented as such to be with the
 * other loading/conversion from sim_image structures.
 *
 * @param[in] pImage input RGB24 or RGB32 image
 * @param[out] pBuffer to write the converted data
 * @param bRescale rescale input data to HDR Insertion bitdepth
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pImage or pBuffer is NULL
 * @return IMG_ERROR_NOT_SUPPORTED if pImage is not RGB32 or RGB24
 */
IMG_RESULT CI_Convert_HDRInsertion(const struct _sSimImageIn_ *pImage,
    CI_BUFFER *pBuffer, IMG_BOOL8 bRescale);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the IIF Converter functions group
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_CONVERTER_H_ */
