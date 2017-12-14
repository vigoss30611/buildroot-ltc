/**
*******************************************************************************
@file sim_image.h

@brief CImage C interface for FLX format

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
#include <img_types.h>
#include <img_defs.h>

#ifndef SIM_IMAGE_H
#define SIM_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

enum sSimImageColourModel
{
    /** @brief RGB888 24b */
    SimImage_RGB24,
    /** @brief RGB888 32b or RGB101010 32b */
    SimImage_RGB32,
    /** @brief RGB161616 in 64b (gap for Alpha) not rescaled */
    SimImage_RGB64,
    /** @brief Bayer RGGB any bitdepth */
    SimImage_RGGB,
    /** @brief Bayer GRBG any bitdepth */
    SimImage_GRBG,
    /** @brief Bayer GBRG any bitdepth */
    SimImage_GBRG,
    /** @brief Bayer BGGR any bitdepth */
    SimImage_BGGR,
};

struct SimImageInfo
{
    /** @brief pBuffer's elements bit depths */
    IMG_UINT8 ui8BitDepth;
    /** @brief width in pixel */
    IMG_UINT32 ui32Width;
    /** @brief height in lines */
    IMG_UINT32 ui32Height;
    /** @brief distance between 2 lines in Bytes */
    IMG_SIZE stride;
    /** @brief Colour model used */
    enum sSimImageColourModel eColourModel;
    /**
     * @brief R in LSB. When using RGB image can be used not to swap RGB to
     * BGR (felix outputs standard RGB which is B in LSB but it could have
     * been swapped already)
     */
    IMG_BOOL8 isBGR;
};

typedef struct _sSimImageIn_
{
    struct SimImageInfo info;
    /**
     * @brief Number of frames in file
     * 
     * Populated after loading.
     */
    IMG_UINT32 nFrames;

    /**
     * @brief pointer to CImageFLX used only when loading
     *
     * Done as void* to have C compatibility (CImageFLX is C++).
     *
     * As a public member so that C++ users can access extra information from
     * it if they want to.
     */
    void *loadFromFLX;

    /** @brief pixel buffer - should be non NULL if image was loaded */
    IMG_UINT16 *pBuffer;
} sSimImageIn;

typedef struct _sSimImageOut
{
    struct SimImageInfo info;
    IMG_INT8 nPlanes;

    /**
     * @brief pointer to CImageFlx used only when saving
     * 
     * Done as void* to have C compatibility (CImageFlx is C++).
     * 
     * As a public member so that C++ users can access extra information from
     * it if they want to.
     */
    void *saveToFLX;
    /** @brief pointer CImageFlx::FLXSAVEFORMAT */
    void *saveFormat;
    /** @brief CImage::FLXSAVECONTEXT, but it is private so DO NOT DELETE */
    void *saveContext;

} sSimImageOut;


/**
 * @brief Initialise the value of pSimImage at creation
 */
IMG_RESULT SimImageIn_init(sSimImageIn *pSimImageIn) __attribute__ ((visibility("default")));

/**
 * @brief Open a file for reading
 *
 * Loads either a bayer or a RGB/RGBA FLX image.
 *
 * Supports Multi-segment FLX that do not change the format, bitdepth and size
 * per segment. If a multi-segment image changes these it will load only
 * the frames of the 1st segment.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pSimImageIn or pszFilename is NULL
 * @return IMG_ERROR_ALREADY_INITIALISED if loadFromFLX or pBuffer already
 * initialised (call @ref SimImageIn_close() beforehand)
 * @return IMG_ERROR_MALLOC_FAILED if internal allocations failed
 * @return IMG_ERROR_NOT_SUPPORTED if the given FLX format is not supported
 */
IMG_RESULT SimImageIn_loadFLX(sSimImageIn *pSimImageIn,
    const char *pszFilename) __attribute__ ((visibility("default")));

/**
 * @brief If the given file is multi-segment check that it can be loaded
 *
 * Uses the same function than @ref SimImageIn_loadFLX().
 *
 * @param[in] pszFilename of the FLX file
 * @param[out] nSegments optional - number of segments found
 * @param[out] nTotalFrames optional - total number of frames in all segments
 *
 * @return IMG_SUCCESS if the frame is single-segment or multi-segment
 * with compatible values
 * @return IMG_ERROR_INVALID_PARAMETERS if pszFilename is NULL
 */
IMG_RESULT SimImageIn_checkMultiSegment(const char *pszFilename,
    unsigned int *nSegments, unsigned int *nTotalFrames) __attribute__ ((visibility("default")));

/**
 * @brief Load from an FLX file
 *
 * Converts the given frame index into the buffer.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pSimImageIn is NULL,
 * i32FrameIndex < 0 or i32FrameIndex is too big for the loaded FLX
 * @return IMG_ERROR_NOT_INITIALISED if the file was not open yet
 * @return IMG_ERROR_FATAL if failing to load data
 * @return IMG_ERROR_NOT_SUPPORTED if unsupported formats (should not occure
 * if @ref SimImageIn_loadFLX() succeeded)
 */
IMG_RESULT SimImageIn_convertFrame(sSimImageIn *pSimImageIn,
    IMG_INT32 i32frameIndex) __attribute__ ((visibility("default")));

/**
 * @brief Free the buffer, free CImageFlx and re-init the values of the
 * pSimImageIn.
 *
* The pointer must be freed afterwards or can be reused.
*/
IMG_RESULT SimImageIn_close(sSimImageIn *pSimImageIn) __attribute__ ((visibility("default")));

/**
 * @brief Initialise the value of pSimImage at creation
 */
IMG_RESULT SimImageOut_init(sSimImageOut *pSimImage) __attribute__ ((visibility("default")));

/**
 * @brief Use the sImageOut::info structure to configure the objects for an
 * output file
 */
IMG_RESULT SimImageOut_create(sSimImageOut *pSimImage) __attribute__ ((visibility("default")));

/**
 * @brief Save the header and segment of the file. Initialise the save context
 */
IMG_RESULT SimImageOut_open(sSimImageOut *pSimImage, const char *pszFilename) __attribute__ ((visibility("default")));

/**
 * @brief Add a frame to the image
 */
IMG_RESULT SimImageOut_addFrame(sSimImageOut *pSimImage, const void *pData,
    IMG_SIZE size) __attribute__ ((visibility("default")));

/**
 * @brief Write the data to disk
 */
IMG_RESULT SimImageOut_write(sSimImageOut *pSimImage) __attribute__ ((visibility("default")));

/**
 * @brief Close the open file
 */
IMG_RESULT SimImageOut_close(sSimImageOut *pSimImage) __attribute__ ((visibility("default")));

/**
 * @brief Free the buffer and re-init the values of the pSimImage.
 *
 * The pointer must be freed afterwards or can be reused.
 */
IMG_RESULT SimImageOut_clean(sSimImageOut *pSimImage) __attribute__ ((visibility("default")));

#ifdef __cplusplus
}
#endif

#endif // SIM_IMAGE_H
