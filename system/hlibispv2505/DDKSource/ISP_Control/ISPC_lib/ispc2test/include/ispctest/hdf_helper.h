/**
******************************************************************************
 @file hdf_helper.h

 @brief High Definitions Funtions helpers

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
#ifndef ISPCT_HDF_HELPER_H
#define ISPCT_HDF_HELPER_H

#include <ispc/Camera.h>
#include "img_types.h"
#include <ci/ci_api_structs.h>
#include <ispc/Shot.h>

/**
 * @brief Verify that the provided frame is valid for capture of the
 * configured camera
 *
 * @param camera
 * @param pszHDRInsertionFile FLX RGB to load from
 * @param[out] isValid fits for configured Camera
 * @param[out] uiFrame number of frames in FLX
 */
IMG_RESULT IsValidForCapture(ISPC::Camera &camera,
    const char *pszHDRInsertionFile, bool &isValid,
    unsigned int &uiFrame);

/**
 * @brief To help with HDR insertion using an FLX RGB image
 *
 * @param camera to trigger shot on
 * @param pszHDRInsertionFile[in] file to load HDR insertion from - assumed to
 * be of correct size for the configuration in the camera
 * @param bRescale rescale given file to 16b per pixels (upscaling only)
 * @param uiFrame frame to load from given file
 *
 * Delegates to several CI functions.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if a CI function failed.
 */
IMG_RESULT ProgramShotWithHDRIns(ISPC::Camera &camera,
    const char *pszHDRInsertionFile,
    bool bRescale = true, unsigned int uiFrame = 0);

/**
 * @copydoc ProgramShotWithHDRIns
 */
IMG_RESULT ProgramShotWithHDRIns(ISPC::DGCamera &camera,
    const char *pszHDRInsertionFile,
    bool bRescale = true, unsigned int uiFrame = 0);

/**
 * @brief Dummy merge of 2 HDR Extraction buffers into an HDR insertion image
 * 
 * Merge both frame0 and frame1 data as an average of their pixel values.
 * Result is up-scaled from 10b to 16b.
 * 
 * @note Tiled input is not supported
 * 
 * @warning It is assumed that the given frames will be of the correct size
 * setup in the owner of the HDR buffer
 *
 * @param[in] frame0 HDR extraction buffer 0 - should not be tiled and of the
 * BGR_101010_32 format (not tiled) and of the same dimensions than buffer 1
 * @param[in] frame1 HDR extraction buffer 1 - same limitations than frame0
 * @param[out] pHDRBuffer output HDR buffer - should not be NULL
 * 
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if frame0 or frame1 do not respect
 * their limitations or if pHDRBuffer is NULL
 * @return IMG_ERROR_FATAL if pixel format conversion failed
 * @return IMG_ERROR_NOT_SUPPORTED if the given frames will
 * not fit in the given HDR buffer.
 */
IMG_RESULT AverageHDRMerge(const ISPC::Buffer &frame0,
    const ISPC::Buffer &frame1, CI_BUFFER *pHDRBuffer);

/**
 * @brief To know if HDR Libraries are supported to merge frames
 */
bool HDRLIBS_Supported(void);

/**
 * @brief Used as input of @ref HDRLIBS_MergeFrames
 */
struct HDRMergeInput
{
    const ISPC::Pipeline &context;
    const ISPC::Buffer &buffer;
    unsigned long exposure; // in us
    double gain; // sensor gain
    
    HDRMergeInput(const ISPC::Pipeline &ctx, const ISPC::Buffer &buff)
        : context(ctx), buffer(buff){}
};

/**
 * @brief Merge of 2 HDR Extraction buffers into an HDR insertion image
 * 
 * @note Tiled input is not supported
 * 
 * @warning It is assumed that the given frames will be of the correct size
 * setup in the owner of the HDR buffer
 *
 * @param[in] frame0 HDR extraction buffer 0 - should not be tiled and of the
 * BGR_101010_32 format (not tiled) and of the same dimensions than buffer 1
 * @param[in] frame1 HDR extraction buffer 1 - same limitations than frame0
 * @param[out] pHDRBuffer output HDR buffer - should not be NULL
 * 
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if frame0 or frame1 do not respect
 * their limitations or if pHDRBuffer is NULL
 * @return IMG_ERROR_FATAL if pixel format conversion failed
 * @return IMG_ERROR_NOT_SUPPORTED if the given frames will
 * not fit in the given HDR buffer.
 * @return IMG_ERROR_DISABLED if trying to use the function while HDRLIBS is
 * not supported
 */
IMG_RESULT HDRLIBS_MergeFrames(const HDRMergeInput &frame0,
    const HDRMergeInput &frame1, CI_BUFFER *pHDRBuffer);


#endif // ISPCT_HDF_HELPER_H