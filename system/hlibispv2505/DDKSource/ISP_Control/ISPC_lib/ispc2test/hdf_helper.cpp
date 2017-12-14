/**
******************************************************************************
@file hdf_helper.cpp

@brief Implementation of HDF helper functions

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
#include "ispctest/hdf_helper.h"

#include <sim_image.h>

#include <ispc/Pipeline.h>
#include <ispc/Save.h>

#include <ci/ci_api.h>

#include <img_errors.h>

#define LOG_TAG "HDF_HELPER"
#include <felixcommon/userlog.h>
#include <felixcommon/pixel_format.h>
#include <felixcommon/ci_alloc_info.h>
#include <ci/ci_converter.h>

static IMG_RESULT IMG_ProgramShotWithHDRIns(ISPC::Camera &camera,
    CI_BUFFID &ids,
    const char *pszHDRInsertionFile,
    bool bRescale, unsigned int ui32Frame)
{
    CI_BUFFER HDRInsBuffer;
    IMG_RESULT ret;
    sSimImageIn sHDRIns;
    ISPC::Pipeline *pPipeline = camera.getPipeline();
    CI_PIPELINE *pCIPipeline = camera.getPipeline()->getCIPipeline();

    
    ret = pPipeline->getFirstAvailableBuffers(ids);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to find available buffers (returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }

    ret = pPipeline->getAvailableHDRInsertion(HDRInsBuffer);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to find available HDR Insertion buffer "\
            "(returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }

    ids.idHDRIns = HDRInsBuffer.id;

    // now convert HDR insertion

    SimImageIn_init(&(sHDRIns));

    ret = SimImageIn_loadFLX(&(sHDRIns), pszHDRInsertionFile);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to load open HDR Insertion FLX "\
            "(returned %d)\n", ui32Frame, ret);
        SimImageIn_close(&(sHDRIns));
        return IMG_ERROR_FATAL;
    }

    ret = SimImageIn_convertFrame(&(sHDRIns), ui32Frame);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to load frame %d from HDR Insertion FLX "\
            "(returned %d)\n", ui32Frame, ret);
        SimImageIn_close(&(sHDRIns));
        return IMG_ERROR_FATAL;
    }

    ret = CI_Convert_HDRInsertion(&(sHDRIns), &HDRInsBuffer,
        bRescale ? IMG_TRUE : IMG_FALSE);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to convert frame %d from HDR Insertion FLX "\
            "(rescale=%d, returned %d)",
            ui32Frame, (int)bRescale, ret);
        SimImageIn_close(&(sHDRIns));
        return IMG_ERROR_FATAL;
    }

#if 1
    LOG_INFO("saving converted.flx\n");
    ret = ISPC::Save::SingleHDF(*pPipeline, HDRInsBuffer, "converted.flx");
    if (IMG_SUCCESS != ret)
    {
        LOG_WARNING("failed to save converted.flx\n");
    }
#endif
    
    SimImageIn_close(&(sHDRIns));
    return IMG_SUCCESS;
}

IMG_RESULT IsValidForCapture(ISPC::Camera &camera,
    const char *pszHDRInsertionFile, bool &isValid,
    unsigned int &uiFrame)
{
    sSimImageIn image;
    IMG_RESULT ret;
    ISPC::Global_Setup set = camera.getPipeline()->getGlobalSetup(&ret);

    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get global setup information!\n");
        return IMG_ERROR_FATAL;
    }

    isValid = true; // assumes it is valid

    // check sizes of provided image are big enough

    SimImageIn_init(&image);

    ret = SimImageIn_loadFLX(&image, pszHDRInsertionFile);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to load frame 1st of HDR Insertion '%s'\n",
            pszHDRInsertionFile);
        return EXIT_FAILURE;
    }
    
    if (image.info.ui32Width != set.ui32ImageWidth
        || image.info.ui32Height != set.ui32ImageHeight)
    {
        LOG_ERROR("Provided HDR Insertion image is %dx%d while Pipeline "\
            "image after IIF is %dx%d - they should be of the same size\n",
            image.info.ui32Width, image.info.ui32Height,
            set.ui32ImageWidth, set.ui32ImageHeight);
        isValid = false;
        SimImageIn_close(&image);
        return EXIT_FAILURE;
    }

    if (image.info.ui8BitDepth > 16)
    {
        LOG_ERROR("Provided HDR Insertion image has a bitdepth of %d "\
            "while the maximum is 16\n",
            image.info.ui8BitDepth);
        isValid = false;
        SimImageIn_close(&image);
        return EXIT_FAILURE;
    }

    uiFrame = image.nFrames;
    SimImageIn_close(&image);

    return IMG_SUCCESS;
}

IMG_RESULT ProgramShotWithHDRIns(ISPC::Camera &camera,
    const char *pszHDRInsertionFile,
    bool bRescale, unsigned int ui32Frame)
{
    CI_BUFFID ids;
    IMG_RESULT ret = IMG_ProgramShotWithHDRIns(camera, ids,
        pszHDRInsertionFile, bRescale, ui32Frame);

    ret = camera.enqueueSpecifiedShot(ids);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to trigger shot\n");
    }
    
    return ret;
}

IMG_RESULT ProgramShotWithHDRIns(ISPC::DGCamera &camera,
    const char *pszHDRInsertionFile,
    bool bRescale, unsigned int ui32Frame)
{
    CI_BUFFID ids;
    IMG_RESULT ret = IMG_ProgramShotWithHDRIns(camera, ids,
        pszHDRInsertionFile, bRescale, ui32Frame);

    ret = camera.enqueueSpecifiedShot(ids);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to trigger shot\n");
    }

    return ret;
}

IMG_RESULT AverageHDRMerge(const ISPC::Buffer &frame0,
    const ISPC::Buffer &frame1, CI_BUFFER *pHDRBuffer)
{
    IMG_UINT16 *pBuffData = (IMG_UINT16*)pHDRBuffer->data;
    IMG_INT32 neededSize = 0;
    unsigned i = 0, j = 0;
    unsigned inStride = 0, outStride = 0;
    struct CI_SIZEINFO sizeInfo;
    PIXELTYPE pixelType;
    IMG_RESULT ret;
    
    // insertion buffer bitdepth = 16b - HDR output is 10b
    // merging the two frame0 and frame1 needs a rescaling of the values
    const int input_pixel_bit = 10;
    const int input_mask = 0x3FF;
    const int r = 2, g = 1, b = 0; // channel order
    const int shift = 16 - input_pixel_bit; 
    const int mask = (1 << (shift + 1))-1;
    
    LOG_DEBUG("using dummy frame merge\n");
    if (frame0.width != frame1.width
        || frame0.height != frame1.height
        || frame0.stride != frame1.stride 
        || frame0.pxlFormat != frame1.pxlFormat)
    {
        LOG_ERROR("frame0 and frame1 are not from the same source\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (frame0.isTiled || frame1.isTiled)
    {
        LOG_ERROR("algorithm does not support tiled input\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (frame0.pxlFormat != BGR_101010_32)
    {
        LOG_ERROR("frame0 and frame1 format is not BGR 10b 32\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (!pHDRBuffer)
    {
        LOG_ERROR("pHDRBuffer is NULL!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    
    ret = PixelTransformRGB(&pixelType, BGR_161616_64);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get information about %s",
            FormatString(BGR_161616_64));
        return IMG_ERROR_FATAL;
    }

    ret = CI_ALLOC_RGBSizeInfo(&pixelType, frame0.width,
        frame0.height, NULL, &sizeInfo);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get allocation information about %s for "\
            "image size %dx%d",
            FormatString(BGR_161616_64), frame0.width,
            frame0.height);
        return IMG_ERROR_FATAL;
    }
    
    neededSize = sizeInfo.ui32Stride*sizeInfo.ui32Height;
    
    if ((IMG_UINT32)neededSize > pHDRBuffer->ui32Size)
    {
        LOG_ERROR("a buffer of %u Bytes is too small - %u needed\n",
            pHDRBuffer->ui32Size, neededSize);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    
    IMG_ASSERT(sizeInfo.ui32Stride % sizeof(IMG_UINT16) == 0);
    IMG_ASSERT(frame0.stride % sizeof(IMG_UINT32) == 0);
    outStride = sizeInfo.ui32Stride / sizeof(IMG_UINT16);
    inStride = frame0.stride/sizeof(IMG_UINT32);
    
    const IMG_UINT32 *pFrame0 = (IMG_UINT32*)frame0.data;
    const IMG_UINT32 *pFrame1 = (IMG_UINT32*)frame1.data;
    const IMG_UINT32 maxPix = (1<<16) -1;
    
    for (j = 0; j < frame0.height; j++)
    {
        for (i = 0; i < frame0.width; i++)
        {
            int out = j*outStride + i*4,
                in = j*inStride + i;
            
            // low exposure
            int r0 = (pFrame0[in]>>(r*input_pixel_bit))&input_mask,
                g0 = (pFrame0[in]>>(g*input_pixel_bit))&input_mask,
                b0 = (pFrame0[in]>>(b*input_pixel_bit))&input_mask;
                
            // high exposure
            int r1 = (pFrame1[in]>>(r*input_pixel_bit))&input_mask,
                g1 = (pFrame1[in]>>(g*input_pixel_bit))&input_mask,
                b1 = (pFrame1[in]>>(b*input_pixel_bit))&input_mask;
            
            // red
            IMG_UINT32 pix = (r0+r1)/2;
            pix = pix<<shift |
                (pix>>(input_pixel_bit - shift))&mask;
            pBuffData[out + r] = IMG_MIN_INT(pix, maxPix);
                
            // green
            pix = (g0+g1)/2;
            pix = pix<<shift |
                (pix>>(input_pixel_bit - shift))&mask;
            pBuffData[out + g] = IMG_MIN_INT(pix, maxPix);
                
            // blue
            pix = (b0+b1)/2;
            pix = pix<<shift |
                (pix>>(input_pixel_bit - shift))&mask;
            pBuffData[out + b] = IMG_MIN_INT(pix, maxPix);

            pBuffData[out + 3] = 0; // blank space
        }
    }
    
    return IMG_SUCCESS;
}
