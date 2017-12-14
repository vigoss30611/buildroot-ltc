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
     * Populated after loading
     * 
     * @note Only loads 1st segment
     */
    IMG_UINT32 nFrames;

    /** @brief pixel buffer */
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
IMG_RESULT SimImageIn_init(sSimImageIn *pSimImage);

/**
 * @brief Free the buffer and re-init the values of the pSimImage.
 *
 * The pointer must be freed afterwards or can be reused.
 */
IMG_RESULT SimImageIn_clean(sSimImageIn *pSimImage);

IMG_RESULT SimImageIn_loadFLX(const char *pszFilename,
    sSimImageIn *sSimImageIn, IMG_INT32 i32frameIndex);


/**
 * @brief Initialise the value of pSimImage at creation
 */
IMG_RESULT SimImageOut_init(sSimImageOut *pSimImage);

/**
 * @brief Use the sImageOut::info structure to configure the objects for an
 * output file
 */
IMG_RESULT SimImageOut_create(sSimImageOut *pSimImage);

/**
 * @brief Save the header and segment of the file. Initialise the save context
 */
IMG_RESULT SimImageOut_open(sSimImageOut *pSimImage, const char *pszFilename);

/**
 * @brief Add a frame to the image
 */
IMG_RESULT SimImageOut_addFrame(sSimImageOut *pSimImage, const void *pData,
    IMG_SIZE size);

/**
 * @brief Write the data to disk
 */
IMG_RESULT SimImageOut_write(sSimImageOut *pSimImage);

/**
 * @brief Close the open file
 */
IMG_RESULT SimImageOut_close(sSimImageOut *pSimImage);

/**
 * @brief Free the buffer and re-init the values of the pSimImage.
 *
 * The pointer must be freed afterwards or can be reused.
 */
IMG_RESULT SimImageOut_clean(sSimImageOut *pSimImage);

#ifdef __cplusplus
}
#endif

#endif // SIM_IMAGE_H
