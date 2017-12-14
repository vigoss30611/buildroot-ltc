/**
*******************************************************************************
 @file Save.cpp

 @brief Implementation of ISPC::Save

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
#include "ispc/Save.h"

#include <img_defs.h>
#include <savefile.h>
#include <felixcommon/userlog.h>
#include <felixcommon/ci_alloc_info.h>
#include <felixcommon/pixel_format.h>
#define LOG_TAG "ISPC_Save"

#include <string>
#include <cstdio>

#include "ispc/Pipeline.h"
#include "ispc/ModuleOUT.h"
#include "ispc/ISPCDefs.h"

#if defined(WIN32)
#define snprintf _snprintf
#endif

//
// protected
//

int ISPC::Save::openBayer(ePxlFormat fmt, MOSAICType mosaic,
    unsigned int w, unsigned int h)
{
    struct SimImageInfo imageInfo;
    IMG_RESULT ret;
    PIXELTYPE sType;

    ret = PixelTransformBayer(&sType, fmt, mosaic);
    if (ret)
    {
        LOG_ERROR("registered context does not have a correct bayer format:" \
            " %s\n", FormatString(fmt));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (!file)
    {
        LOG_ERROR("file should have been allocated!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    // SaveFile_init(file);
    imageInfo.ui32Width  = w;
    imageInfo.ui32Height = h;

    // data extraction
    imageInfo.stride = imageInfo.ui32Width*8;  // 4*2 bytes per pixel
    imageInfo.ui8BitDepth = sType.ui8BitDepth;
    switch (mosaic)
    {
    case MOSAIC_RGGB:
        imageInfo.eColourModel = SimImage_RGGB;
        break;
    case MOSAIC_GRBG:
        imageInfo.eColourModel = SimImage_GRBG;
        break;
    case MOSAIC_GBRG:
        imageInfo.eColourModel = SimImage_GBRG;
        break;
    case MOSAIC_BGGR:
        imageInfo.eColourModel = SimImage_BGGR;
        break;
    default:  // should not happen
        LOG_ERROR("unsupported bayer format in sensor\n");
        return IMG_ERROR_FATAL;
    }

    ret = SaveFile_openFLX(file, filename.c_str(), &imageInfo);
    if (ret)
    {
        LOG_ERROR("failed to open '%s' as DE output\n", filename.c_str());
    }
    return ret;
}

int ISPC::Save::openTiff(ePxlFormat fmt, MOSAICType mosaic,
    unsigned int w, unsigned int h)
{
    struct SimImageInfo imageInfo;
    IMG_RESULT ret;
    PIXELTYPE sType;

    ret = PixelTransformBayer(&sType, fmt, mosaic);
    if (ret)
    {
        LOG_ERROR("registered context does not have a correct bayer format:" \
            " %s\n", FormatString(fmt));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (!file)
    {
        LOG_ERROR("file should have been allocated!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    // SaveFile_init(file);
    imageInfo.ui32Width  = w;
    imageInfo.ui32Height = h;

    // data extraction of TIFF
    imageInfo.stride = imageInfo.ui32Width*sizeof(IMG_UINT16);
    imageInfo.ui8BitDepth = sType.ui8BitDepth;
    switch (mosaic)
    {
    case MOSAIC_RGGB:
        imageInfo.eColourModel = SimImage_RGGB;
        break;
    case MOSAIC_GRBG:
        imageInfo.eColourModel = SimImage_GRBG;
        break;
    case MOSAIC_GBRG:
        imageInfo.eColourModel = SimImage_GBRG;
        break;
    case MOSAIC_BGGR:
        imageInfo.eColourModel = SimImage_BGGR;
        break;
    default:  // should not happen
        LOG_ERROR("unsupported bayer format in sensor\n");
        return IMG_ERROR_FATAL;
    }

    ret = SaveFile_openFLX(file, filename.c_str(), &imageInfo);
    if (ret)
    {
        LOG_ERROR("failed to open '%s' as DE output\n", filename.c_str());
    }
    return ret;
}

IMG_RESULT ISPC::Save::openRGB(ePxlFormat pxlFmt, unsigned int w,
    unsigned int h)
{
    PIXELTYPE sType;
    struct SimImageInfo imageInfo;
    IMG_RESULT ret;

    ret = PixelTransformRGB(&sType, pxlFmt);
    if (ret)
    {
        LOG_ERROR("registered context does not have a correct RGB format:" \
            " %s\n", FormatString(pxlFmt));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    imageInfo.ui32Width = w;
    imageInfo.ui32Height = h;
    imageInfo.stride =
        imageInfo.ui32Width*sType.ui8PackedStride;
    switch (pxlFmt)
    {
    case BGR_888_24:
    case BGR_888_32:
    case BGR_101010_32:
    case BGR_161616_64:
        imageInfo.isBGR = true;
        break;
    default:
        // true means R is in LSB in output given to FLX
        imageInfo.isBGR = false;
    }

    switch (pxlFmt)
    {
    case RGB_888_24:
    case BGR_888_24:
        imageInfo.ui8BitDepth = 8;
        imageInfo.eColourModel = SimImage_RGB24;
        break;
    case RGB_888_32:
    case BGR_888_32:
        imageInfo.ui8BitDepth = 8;
        imageInfo.eColourModel = SimImage_RGB32;
        break;
    case RGB_101010_32:
    case BGR_101010_32:
        imageInfo.ui8BitDepth = 10;
        imageInfo.eColourModel = SimImage_RGB32;
        break;
    case BGR_161616_64:
        imageInfo.ui8BitDepth = 16;
        imageInfo.eColourModel = SimImage_RGB64;
        break;
    default:  // should not happen as we checked the format before
        LOG_ERROR("unsupported RGB format\n");
        return IMG_ERROR_FATAL;
    }

    ret = SaveFile_openFLX(file, filename.c_str(), &imageInfo);
    if (ret)
    {
        LOG_ERROR("failed to open '%s' as display output\n", filename.c_str());
    }
    return ret;
}

IMG_RESULT ISPC::Save::openBytes()
{
    IMG_RESULT ret;

    if (!file)
    {
        LOG_ERROR("file should have been allocated!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    // SaveFile_init(file);

    ret = SaveFile_open(file, filename.c_str());
    if (ret)
    {
        LOG_ERROR("failed to open '%s' as generic output!\n",
            filename.c_str());
    }
    return ret;
}

IMG_RESULT ISPC::Save::saveBayer(const ISPC::Buffer &bayerBuffer , int nCam)
{
    IMG_UINT16 outSizes[2] = {bayerBuffer.width, bayerBuffer.height};
    IMG_SIZE outsize = 0;
    void *pConverted = NULL;
    PIXELTYPE stype;
    int ret = IMG_SUCCESS;

    if (bayerBuffer.isTiled)
    {
        // should never happen because bayer cannot be tiled
        LOG_ERROR("cannot save tiled bayer buffer\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = PixelTransformBayer(&stype, bayerBuffer.pxlFormat, MOSAIC_RGGB);
    if (ret)
    {
        LOG_ERROR("the given format '%s' is not Bayer\n",
            FormatString(bayerBuffer.pxlFormat));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    printf("bayerbuffer outSizes width = %d = height=%d, the nCam=%d\n", outSizes[0], outSizes[1], nCam);
    for(int i = 0; i < nCam; i++)
    {

        ret = convertToPlanarBayer(outSizes, bayerBuffer.firstData(),
            bayerBuffer.stride, stype.ui8BitDepth,
            static_cast<void**>(&pConverted), &outsize);
        if (ret)
        {
            LOG_ERROR("failed to convert Bayer frame\n");
            return IMG_ERROR_FATAL;
        }
        else
        {
            ret = SaveFile_write(file, pConverted, outsize);
            if (ret)
            {
                LOG_ERROR("failed to write a data-extraction frame!\n");
            }
        }
    }
    if (NULL != pConverted)
    {
        IMG_FREE(pConverted);
        pConverted = NULL;
    }

    return ret;
}

IMG_RESULT ISPC::Save::saveTiff(const ISPC::Buffer &raw2dBuffer)
{
    IMG_UINT16 outSizes[2] = { raw2dBuffer.width, raw2dBuffer.height };
    IMG_SIZE outsize = 0;
    void *pConverted = NULL;
    PIXELTYPE stype;
    int ret = EXIT_SUCCESS;

    if (raw2dBuffer.isTiled)
    {
        // should never happen because we cannot generate tiled TIFF
        LOG_ERROR("cannot save tiled TIFF buffer\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = PixelTransformBayer(&stype, raw2dBuffer.pxlFormat, MOSAIC_RGGB);
    if (ret)
    {
        LOG_ERROR("the given format '%s' is not Bayer TIFF\n",
            FormatString(raw2dBuffer.pxlFormat));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = convertToPlanarBayerTiff(outSizes, raw2dBuffer.firstData(),
        raw2dBuffer.stride, stype.ui8BitDepth, &pConverted, &outsize);
    if (ret)
    {
        LOG_ERROR("failed to convert raw2D Tiff Bayer frame\n");
        return IMG_ERROR_FATAL;
    }
    else
    {
        ret = SaveFile_write(file, pConverted, outsize);
        if (ret)
        {
            LOG_ERROR("failed to write a raw 2D frame!\n");
        }
    }

    if ( pConverted != NULL )
    {
        IMG_FREE(pConverted);
        pConverted = NULL;
    }

    return ret;
}

IMG_RESULT ISPC::Save::saveRGB(const ISPC::Buffer &rgbBuffer)
{
    PIXELTYPE stype;
    IMG_RESULT ret;

    if (rgbBuffer.isTiled)
    {
        LOG_ERROR("cannot save tiled RGB buffer - untile it first!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = PixelTransformRGB(&stype, rgbBuffer.pxlFormat);
    if (ret)
    {
        LOG_ERROR("the given format '%s' is not RGB\n",
            FormatString(rgbBuffer.pxlFormat));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = SaveFile_writeFrame(file, rgbBuffer.firstData(),
    // stride in hw (including extra memory for alignment)
        rgbBuffer.stride,
    // wanted stride for that format (without the extra memory for alignment)
        rgbBuffer.width*stype.ui8PackedStride,
        rgbBuffer.height);
    if (ret)
    {
        LOG_ERROR("failed to write a RGB frame!\n");
    }
    return ret;
}

IMG_RESULT ISPC::Save::saveRGB(const CI_BUFFER &hdfBuffer)
{
    PIXELTYPE stype;
    IMG_RESULT ret;
    const struct SimImageInfo &info = file->pSimImage->info;
    struct CI_SIZEINFO sSize;

    ret = PixelTransformRGB(&stype, BGR_161616_64);
    if (ret)
    {
        LOG_ERROR("the given format '%s' is not RGB\n",
            FormatString(BGR_161616_64));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = CI_ALLOC_RGBSizeInfo(&stype, info.ui32Width, info.ui32Height,
        NULL, &sSize);
    if (ret)
    {
        LOG_ERROR("Failed to get allocation information from '%s'\n",
            FormatString(BGR_161616_64));
        return IMG_ERROR_NOT_SUPPORTED;
    }
    IMG_ASSERT(file->pSimImage->info.isBGR == IMG_TRUE);

    ret = SaveFile_writeFrame(file, hdfBuffer.data,
    // stride in hw (including extra memory for alignment)
        sSize.ui32Stride,
    // wanted stride for that format (without the extra memory for alignment)
        info.ui32Width*stype.ui8PackedStride,
        info.ui32Height);
    if (ret)
    {
        LOG_ERROR("failed to write a RGB frame!\n");
    }
    return ret;
}

IMG_RESULT ISPC::Save::saveYUV(const ISPC::Buffer &yuvBuffer, int nCam)
{
    const int lumaStride = yuvBuffer.stride;
    int lumaWidth = yuvBuffer.width;
    int lumaHeight = yuvBuffer.height;
    const int lumaSize = yuvBuffer.offsetCbCr;
    const int chromaStride = yuvBuffer.strideCbCr;
    int chromaWidth;
    int chromaHeight;
    PIXELTYPE stype;
    IMG_RESULT ret;

    if (yuvBuffer.isTiled)
    {
        LOG_ERROR("cannot saved tiled YUV buffer - untile it first!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = PixelTransformYUV(&stype, yuvBuffer.pxlFormat);
    if (ret)
    {
        LOG_ERROR("the given format '%s' is not YUV\n",
            FormatString(yuvBuffer.pxlFormat));
        return IMG_ERROR_NOT_SUPPORTED;
    }
    IMG_ASSERT(((IMG_UINT32)yuvBuffer.stride*yuvBuffer.height)
        <=yuvBuffer.offsetCbCr);

    // UV in same plane
    chromaWidth = (lumaWidth * 2) / stype.ui8HSubsampling;
    chromaHeight = (lumaHeight) / stype.ui8VSubsampling;

    if ( stype.ui8BitDepth == 10 )
    {
        int bop = lumaWidth / stype.ui8PackedElements;
        if (0 != lumaWidth%stype.ui8PackedElements)
        {
            bop++;
        }

        lumaWidth = (bop * stype.ui8PackedStride);
        // height is (pPipeline->sEncoderScaler.aOutputSize[1] + 1);
        chromaWidth = (lumaWidth * 2) / stype.ui8HSubsampling;
    }

    //printf("### lumaWidth=%d, lumaHeight=%d, lumaStride=%d, lumaSize=%d\n", lumaWidth, lumaHeight, lumaStride, lumaSize);
    //printf("### chromaWidth=%d, chromaHeight=%d, chromaStride=%d\n", chromaWidth, chromaHeight, chromaStride);
    printf("### in saveYUV : nCam = %d\n", nCam);
    for(int i = 0; i < nCam; i++){
        ret = SaveFile_writeFrame(
            file,
            yuvBuffer.firstData() + (lumaWidth *lumaHeight + chromaWidth*chromaHeight)*i,
            lumaStride, lumaWidth, lumaHeight);
        if (ret)
        {
            LOG_ERROR("failed to write YUV's luma frame!\n");
            return EXIT_FAILURE;
        }

        ret = SaveFile_writeFrame(
            file,
            yuvBuffer.firstDataCbCr() + (lumaWidth *lumaHeight + chromaWidth*chromaHeight)*i,
            chromaStride, chromaWidth, chromaHeight);
        if (ret)
        {
            LOG_ERROR("failed to write YUV's chroma frame!\n");
        }
    }
    return ret;
}

IMG_RESULT ISPC::Save::untileYUV(const ISPC::Buffer &yuvBuffer,
    unsigned int tileW, unsigned int tileH, ISPC::Buffer &result)
{
    PIXELTYPE stype;
    IMG_RESULT ret;
    void *memory = 0;
    IMG_UINT32 outputCbCrOffset = 0;

    if (!yuvBuffer.isTiled)
    {
        LOG_ERROR("YUV buffer is not tiled! Cannot untile it!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = PixelTransformYUV(&stype, yuvBuffer.pxlFormat);
    if (ret)
    {
        LOG_ERROR("the given format '%s' is not YUV\n",
            FormatString(yuvBuffer.pxlFormat));
        return IMG_ERROR_NOT_SUPPORTED;
    }
    IMG_ASSERT(((IMG_UINT32)yuvBuffer.stride*yuvBuffer.height)
        <= yuvBuffer.offsetCbCr);

    LOG_INFO("untiling YUV buffer %dx%d - str=%d\n",
        tileW, tileH, yuvBuffer.stride);

    memory = IMG_CALLOC(yuvBuffer.stride
        * (yuvBuffer.vstride+yuvBuffer.vstrideCbCr),
        sizeof(IMG_UINT8));
    if (!memory)
    {
        LOG_ERROR("failed to allocate untiled memory\n");
        return IMG_ERROR_MALLOC_FAILED;
    }

    // detile Y
    ret = BufferDeTile(tileW, tileH, yuvBuffer.stride,
        yuvBuffer.firstData(),
        static_cast<IMG_UINT8*>(memory),
        yuvBuffer.stride, yuvBuffer.vstride);
    if (ret)
    {
        LOG_ERROR("failed to convert the Y tiled buffer!\n");
        IMG_FREE(memory);
        return IMG_ERROR_FATAL;
    }
    // detile CbCr
    outputCbCrOffset = yuvBuffer.stride*yuvBuffer.vstride;
    ret = BufferDeTile(tileW, tileH, yuvBuffer.strideCbCr,
        yuvBuffer.firstDataCbCr(),
        (static_cast<IMG_UINT8*>(memory) + outputCbCrOffset),
        yuvBuffer.strideCbCr, yuvBuffer.vstrideCbCr);
    if (ret)
    {
        LOG_ERROR("failed to convert the CbCr tiled buffer!\n");
        IMG_FREE(memory);
        return IMG_ERROR_FATAL;
    }

    result = yuvBuffer;
    result.isTiled = false;
    result.data = ( IMG_UINT8*)memory;
    result.id = 0;
    result.offset = 0;
    result.offsetCbCr = outputCbCrOffset;

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Save::untileRGB(const ISPC::Buffer &rgbBuffer,
    unsigned int tileW, unsigned int tileH, ISPC::Buffer &result)
{
    PIXELTYPE stype;
    IMG_RESULT ret;
    void *memory = 0;

    if (!rgbBuffer.isTiled)
    {
        LOG_ERROR("RGB buffer is not tiled! Cannot untile it!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = PixelTransformRGB(&stype, rgbBuffer.pxlFormat);
    if (ret)
    {
        LOG_ERROR("the given format '%s' is not RGB\n",
            FormatString(rgbBuffer.pxlFormat));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    LOG_INFO("untiling RGB buffer %dx%d - str=%d\n",
        tileW, tileH, rgbBuffer.stride);

    memory = IMG_CALLOC(rgbBuffer.stride*rgbBuffer.vstride, sizeof(IMG_UINT8));

    if (!memory)
    {
        LOG_ERROR("failed to allocate untiled memory\n");
        return IMG_ERROR_MALLOC_FAILED;
    }

    ret = BufferDeTile(tileW, tileH, rgbBuffer.stride,
        rgbBuffer.firstData(), static_cast<IMG_UINT8*>(memory),
        rgbBuffer.stride, rgbBuffer.vstride);
    if (ret)
    {
        LOG_ERROR("failed to detile RGB buffer\n");
        IMG_FREE(memory);
        return IMG_ERROR_FATAL;
    }

    result = rgbBuffer;
    result.isTiled = false;
    result.data = (IMG_UINT8*)memory;
    result.id = 0;
    result.offset = 0;

    return IMG_SUCCESS;
}

//
// public
//

ISPC::Save::Save()
{
    file = 0;
}

ISPC::Save::~Save()
{
    if (file)
    {
        LOG_DEBUG("Force closing of open file '%s' at destruction\n",
            filename.c_str());
        close();
    }
}

const std::string & ISPC::Save::getFilename() const
{
    return filename;
}

bool ISPC::Save::isOpened() const
{
    return file != 0;
}

ISPC::Save::SaveType ISPC::Save::format() const
{
    return type;
}

IMG_RESULT ISPC::Save::open(SaveType _type, const ISPC::Pipeline &context,
    const std::string &_filename)
{
    int ret = IMG_ERROR_ALREADY_INITIALISED;
    if (!file)
    {
        LOG_DEBUG("create SaveFile\n");
        type = _type;
        filename = _filename;
        file = static_cast<SaveFile*>(IMG_CALLOC(1, sizeof(SaveFile)));
        if (!file)
        {
            LOG_ERROR("failed to allocate SaveFile object\n");
            return IMG_ERROR_MALLOC_FAILED;
        }
        SaveFile_init(file);

        const ISPC::ModuleOUT *glb = context.getModule<ISPC::ModuleOUT>();
        ISPC::Global_Setup size_info = context.getGlobalSetup();

        if (NULL == context.getSensor())
        {
            LOG_ERROR("registered context does not have sensor!\n");
            return IMG_ERROR_FATAL;
        }

        tileScheme = context.getConnection()->sHWInfo.uiTiledScheme;

        switch (type)
        {
        case Bayer:
            ret = openBayer(glb->dataExtractionType,
                context.getSensor()->eBayerFormat,
                size_info.ui32ImageWidth, size_info.ui32ImageHeight);
            break;

        case Bayer_TIFF:
            ret = openTiff(glb->raw2DExtractionType,
                context.getSensor()->eBayerFormat,
                size_info.ui32ImageWidth, size_info.ui32ImageHeight);
            break;

        case YUV:
        case Bytes:
            ret = openBytes();
            break;

        case RGB:
            ret = openRGB(glb->displayType,
                size_info.ui32DispWidth, size_info.ui32DispHeight);
            break;

        case RGB_EXT:
            ret = openRGB(glb->hdrExtractionType,
                size_info.ui32ImageWidth, size_info.ui32ImageHeight);
            break;

        case RGB_INS:
            ret = openRGB(glb->hdrInsertionType,
                size_info.ui32ImageWidth, size_info.ui32ImageHeight);
            break;

        default:
            ret = IMG_ERROR_NOT_INITIALISED;
        }
        if (ret)
        {
            SaveFile_destroy(file);
            delete file;
            file = 0;
        }
    }
    return ret;
}

IMG_RESULT ISPC::Save::close()
{
    if (file)
    {
        SaveFile_destroy(file);  // closes the file if still open
        IMG_FREE(file);
        file = NULL;
        return IMG_SUCCESS;
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT ISPC::Save::save(const ISPC::Shot &shot, int nCam)
{
    int ret = IMG_ERROR_NOT_INITIALISED;
    const unsigned int tileW = tileScheme;
    const unsigned int tileH = 4096 / tileScheme;

    if (file)
    {
        switch (type)
        {
        case Bayer:
            ret = saveBayer(shot.BAYER, nCam);
            break;
        case Bayer_TIFF:
            ret = saveTiff(shot.RAW2DEXT);
            break;
        case YUV:
            if (shot.YUV.isTiled)
            {
                ISPC::Buffer untiled;
                ret = untileYUV(shot.YUV, tileW, tileH, untiled);
                if (IMG_SUCCESS == ret)
                {
                    ret = saveYUV(untiled);
                    IMG_FREE((void*)untiled.data);
                }
            }
            else
            {
                ret = saveYUV(shot.YUV, nCam);
            }
            break;
        case RGB:
            if (shot.RGB.isTiled)
            {
                ISPC::Buffer untiled;
                ret = untileRGB(shot.RGB, tileW, tileH, untiled);
                if (IMG_SUCCESS == ret)
                {
                    ret = saveRGB(untiled);
                    IMG_FREE((void*)untiled.data);
                }
            }
            else
            {
                ret = saveRGB(shot.RGB);
            }
            break;
        case RGB_EXT:
            if (shot.HDREXT.isTiled)
            {
                ISPC::Buffer untiled;
                ret = untileRGB(shot.HDREXT, tileW, tileH, untiled);
                if (IMG_SUCCESS == ret)
                {
                    ret = saveRGB(untiled);
                    IMG_FREE((void*)untiled.data);
                }
            }
            else
            {
                ret = saveRGB(shot.HDREXT);
            }
            break;
        case RGB_INS:
            LOG_DEBUG("Use the appropriate save function when creating a " \
                "HDR insertion file\n");
            ret = IMG_ERROR_NOT_SUPPORTED;
            break;

        case Bytes:  // Bytes should use their own saving
            LOG_DEBUG("Use the appropriate save function when creating a " \
                "Bytes typed file\n");
            ret = IMG_ERROR_NOT_SUPPORTED;
            break;

        default:
            LOG_DEBUG("Unknown type\n");
            ret = IMG_ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        LOG_ERROR("File is not open, cannot save\n");
    }
    return ret;
}

IMG_RESULT ISPC::Save::untile(const ISPC::Shot &shot, ISPC::Buffer &result)
{
    int ret = IMG_ERROR_NOT_INITIALISED;  // not returned ever
    const unsigned int tileW = tileScheme;
    const unsigned int tileH = 4096 / tileScheme;

    switch (type)
    {
    case YUV:
        ret = untileYUV(shot.YUV, tileW, tileH, result);
        break;
    case RGB:
        ret = untileRGB(shot.RGB, tileW, tileH, result);
        break;
    case RGB_EXT:
        ret = untileRGB(shot.HDREXT, tileW, tileH, result);
        break;
    case Bayer:
    case Bayer_TIFF:
    case Bytes:  // Bytes should use their own saving
        LOG_ERROR("Cannot untile the associated save format\n");
    default:
        ret = IMG_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

IMG_RESULT ISPC::Save::saveDPF(const ISPC::Shot &shot)
{
    int ret = IMG_SUCCESS;
    IMG_UINT32 nCorr = shot.DPF.size/shot.DPF.elementSize;

    if (Bytes != type)
    {
        LOG_DEBUG("Wrong type - use generic save() function\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (!file)
    {
        LOG_ERROR("File is not open, cannot save DPF\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("DPF corrected %d and has %d (%d dropped) in the output map" \
        " '%s'\n", shot.metadata.defectiveStats.ui32FixedPixels,
        shot.metadata.defectiveStats.ui32NOutCorrection,
        shot.metadata.defectiveStats.ui32DroppedMapModifications,
        filename.c_str());

    IMG_ASSERT(nCorr ==  shot.metadata.defectiveStats.ui32NOutCorrection);
/*if (nCorr*DPF_MAP_OUTPUT_SIZE > shot.pCIBuffer->uiDPFSize)
{
    LOG_ERROR("nb of correct pixels given by HW does not fit in buffer!\n");
    return EXIT_FAILURE;
}*/

    if (0 < nCorr)
    {
        ret = SaveFile_write(file, shot.DPF.data, shot.DPF.size);
        if (ret)
        {
            LOG_ERROR("failed to save DPF to '%s'!\n", filename.c_str());
        }
    }
    else
    {
        LOG_WARNING("no pixel corrected! '%s' will be empty for that frame!\n",
            filename.c_str());
    }
    return ret;
}

IMG_RESULT ISPC::Save::saveENS(const ISPC::Shot &shot)
{
    int ret = IMG_SUCCESS;
    IMG_UINT32 nCorr = shot.ENS.size/shot.ENS.elementSize;

    if (Bytes != type)
    {
        LOG_DEBUG("Wrong type - use generic save() function\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (!file)
    {
        LOG_ERROR("File is not open, cannot save ENS\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_INFO("ENS %d pairs saved to output map '%s'\n", nCorr,
        filename.c_str());

    if (0 < nCorr)
    {
        ret = SaveFile_write(file, shot.ENS.data, shot.ENS.size);
        if (ret)
        {
            LOG_ERROR("failed to save ENS to '%s'!\n", filename.c_str());
        }
    }
    else
    {
        LOG_WARNING("no ENS stats generated! '%s' will be empty for that" \
            " frame!\n", filename.c_str());
    }
    return ret;
}

IMG_RESULT ISPC::Save::saveStats(const ISPC::Shot &shot)
{
    IMG_RESULT ret = IMG_SUCCESS;

    if (Bytes != type)
    {
        LOG_DEBUG("Wrong type - use generic save() function\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (!file)
    {
        LOG_ERROR("File is not open, cannot save DPF\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    char header[64];

    snprintf(header, sizeof(header), "#stats-%dB#", shot.stats.size);

    ret = SaveFile_write(file, header, strlen(header));
    if (ret)
    {
        LOG_WARNING("failed to write the stats 'header'\n");
    }

    ret = SaveFile_write(file, shot.stats.data, shot.stats.size);
    if (ret)
    {
        LOG_WARNING("failed to write the stats\n");
    }

    snprintf(header, sizeof(header), "#stats-done#");
    ret = SaveFile_write(file, header, strlen(header));
    if (ret)
    {
        LOG_WARNING("failed to write the stats 'footer'\n");
    }

    return IMG_SUCCESS;
}

//
// public static
//

std::string ISPC::Save::getEncoderFilename(const ISPC::Pipeline &context)
{
    /// @ use a isstringstream
    char *filename = NULL;
    std::string res;
    const int sSize = 64;
    PIXELTYPE fmt;
    int align = 0;

    const ISPC::ModuleOUT *glb = context.getModule<ISPC::ModuleOUT>();

    PixelTransformYUV(&fmt, glb->encoderType);
    align = fmt.ui8PackedStride;
    // 20 for base-name
    // format string is 10 max
    // 9999 width and 9999 size would take 16 characters
    // alignment should be less than 10 (4 digit alignment for block of pixel)
    // so 46 would be enough - take 64 for headroom

    filename = static_cast<char*>(malloc(sSize * sizeof(char)));
    if (filename)
    {
        snprintf(filename, sSize, "encoder%d_%dx%d_%s_align%d.yuv",
            context.ui8ContextNumber,
            context.getMCPipeline()->sESC.aOutputSize[0],
            context.getMCPipeline()->sESC.aOutputSize[1],
            FormatString(glb->encoderType),
            align);
        res = filename;
        free(filename);
    }
    return res;
}

IMG_RESULT ISPC::Save::Single(const ISPC::Pipeline &context,
    const ISPC::Shot &shot, ISPC::Save::SaveType type,
    const std::string &filename)
{
    IMG_RESULT ret;
    if (Save::Bytes == type)
    {
        LOG_DEBUG("ISPC::Save::Bytes is not supported - use a specific" \
            " function\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    Save outputFile;

    ret = outputFile.open(type, context, filename);
    if (IMG_SUCCESS == ret)
    {
        ret = outputFile.save(shot, context.getCIPipeline()->uiTotalPipeline);
        if (ret)
        {
            LOG_ERROR("failed to save output to '%s'\n", filename.c_str());
        }

        outputFile.close();
    }

    return ret;
}

IMG_RESULT ISPC::Save::SingleDPF(const ISPC::Pipeline &context,
    const ISPC::Shot &shot, const std::string &filename)
{
    IMG_RESULT ret;
    Save outputFile;

    ret = outputFile.open(Save::Bytes, context, filename);
    if (IMG_SUCCESS == ret)
    {
        ret = outputFile.saveDPF(shot);
        if (ret)
        {
            LOG_ERROR("failed to save DPF output to '%s'\n", filename.c_str());
        }

        outputFile.close();
    }

    return ret;
}

IMG_RESULT ISPC::Save::SingleENS(const ISPC::Pipeline &context,
    const ISPC::Shot &shot, const std::string &filename)
{
    IMG_RESULT ret;
    Save outputFile;

    ret = outputFile.open(Save::Bytes, context, filename);
    if (IMG_SUCCESS == ret)
    {
        ret = outputFile.saveENS(shot);
        if (ret)
        {
            LOG_ERROR("failed to save ENS output to '%s'\n", filename.c_str());
        }

        outputFile.close();
    }

    return ret;
}

IMG_RESULT ISPC::Save::SingleStats(const ISPC::Pipeline &context,
    const ISPC::Shot &shot, const std::string &filename)
{
    IMG_RESULT ret;
    Save outputFile;

    ret = outputFile.open(Save::Bytes, context, filename);
    if (IMG_SUCCESS == ret)
    {
        ret = outputFile.saveStats(shot);
        if (ret)
        {
            LOG_ERROR("failed to save statistics output to '%s'\n",
                filename.c_str());
        }

        outputFile.close();
    }

    return ret;
}

IMG_RESULT ISPC::Save::SingleHDF(const ISPC::Pipeline &context,
    const CI_BUFFER &hdrBuffer, const std::string &filename)
{
    IMG_RESULT ret;
    Save outputFile;

    ret = outputFile.open(Save::RGB_INS, context, filename);
    if (IMG_SUCCESS == ret)
    {
        ret = outputFile.saveRGB(hdrBuffer);
        if (ret)
        {
            LOG_ERROR("failed to save HDR insertion to '%s'\n",
                filename.c_str());
        }

        outputFile.close();
    }

    return ret;
}
//begin added by linyun.xiong@2015-12-11
#ifdef ISP_SUPPORT_SCALER
static ISPC::Buffer EscImgBuffer = {0};

int ESC_writeFrame(const IMG_UINT8 * dst, const IMG_UINT8 * src, size_t stride, size_t size, IMG_UINT32 lines)
{
    int ret = 0;
    IMG_UINT32 i;
    IMG_UINT8 *dst_mem = (IMG_UINT8 *)dst, *src_mem = (IMG_UINT8 *)src;
    if ( dst != NULL )
    {
        for ( i = 0 ; i < lines ; i++ )
        {
            IMG_MEMCPY(dst_mem, src_mem, size);
            src_mem += stride;
            dst_mem += size;
            ret += size;
        }
    }
	//printf("-->ESC_writeFrame %d\n", ret);
    return ret;
}

IMG_RESULT ISPC::Save::fetchEscYUV(/*const */ISPC::Buffer &yuvBuffer, ISPC::Buffer &result)
{
    const int lumaStride = yuvBuffer.stride;
    int lumaWidth = yuvBuffer.width;
    const int lumaHeight = yuvBuffer.height;
    const int lumaSize = yuvBuffer.offsetCbCr;
    const int chromaStride = yuvBuffer.strideCbCr;
    int chromaWidth;
    int chromaHeight;
    PIXELTYPE stype;
    IMG_RESULT ret;
    void *memory = (void*)result.data;
    int esc_luma_size = 0, esc_chroma_size = 0;

    if(yuvBuffer.isTiled)
    {
        printf("cannot saved tiled YUV buffer - untile it first!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = PixelTransformYUV(&stype, yuvBuffer.pxlFormat);
    if (IMG_SUCCESS != ret)
    {
        printf("the given format '%s' is not YUV\n", 
            FormatString(yuvBuffer.pxlFormat));
        return IMG_ERROR_NOT_SUPPORTED;
    }
    IMG_ASSERT(((IMG_UINT32)yuvBuffer.stride*yuvBuffer.height)<=yuvBuffer.offsetCbCr);

    chromaWidth = (lumaWidth * 2) / stype.ui8HSubsampling; // UV in same plane
    chromaHeight = (lumaHeight) / stype.ui8VSubsampling;

	if ( stype.ui8BitDepth == 10 )
	{
		int bop = lumaWidth/stype.ui8PackedElements;
		if ( lumaWidth%stype.ui8PackedElements != 0 )
		{
			bop++;
		}

		lumaWidth = (bop*stype.ui8PackedStride); 
        //height is (pPipeline->sEncoderScaler.aOutputSize[1]+1);
		chromaWidth = (lumaWidth * 2) / stype.ui8HSubsampling;
	}

    //printf("### lumaWidth=%d, lumaHeight=%d, lumaStride=%d, lumaSize=%d\n", lumaWidth, lumaHeight, lumaStride, lumaSize);
    //printf("### chromaWidth=%d, chromaHeight=%d, chromaStride=%d\n", chromaWidth, chromaHeight, chromaStride);

	/// @ check 10b support
#if 0
    ret = SaveFile_writeFrame(
        file,
        memory,
        lumaStride, lumaWidth, lumaHeight
    );
	if (IMG_SUCCESS != ret)
    {
        LOG_FATAL("failed to write YUV's luma frame!\n");
        return EXIT_FAILURE;
    }

    ret = SaveFile_writeFrame(
        file,
        (IMG_UINT8 *)(yuvBuffer.data + lumaSize),
        chromaStride, chromaWidth, chromaHeight
    );
    if (IMG_SUCCESS != ret)
    {
        LOG_FATAL("failed to write YUV's chroma frame!\n");
    }
#endif
#if 0
    if (!memory)
    {
        memory = IMG_CALLOC(yuvBuffer.stride*yuvBuffer.height*1.5, sizeof(IMG_UINT8));
        if(!memory)
        {
            printf("-->failed to allocate untiled memory\n");
            return IMG_ERROR_MALLOC_FAILED;
        }
    }
    
    esc_luma_size = ESC_writeFrame(memory, (const void *)yuvBuffer.data, lumaStride, lumaWidth, lumaHeight);
    esc_chroma_size = ESC_writeFrame((IMG_UINT8 *)memory + esc_luma_size, (IMG_UINT8 *)(yuvBuffer.data + lumaSize), chromaStride, chromaWidth, chromaHeight);

	IMG_MEMCPY((void*)yuvBuffer.data, memory, esc_luma_size + esc_chroma_size);
	yuvBuffer.height = lumaHeight;
	yuvBuffer.width = lumaWidth;
	yuvBuffer.stride = lumaWidth;
	yuvBuffer.offsetCbCr = esc_luma_size;
	yuvBuffer.strideCbCr = lumaWidth;
#endif
    result = yuvBuffer;
    result.stride = lumaWidth;//lumaStride;//yuvBuffer.width;
    result.strideCbCr = chromaWidth;//chromaStride;//yuvBuffer.wtdth;
    result.data = (const IMG_UINT8*)memory;
	return ret;
}

IMG_RESULT ISPC::Save::GetEscImg(ISPC::ScalerImg *EscImg, const ISPC::Pipeline &context, const ISPC::Shot &shot)
{
    IMG_RESULT ret = IMG_SUCCESS;
    Save outputFile;
    //printf("-->GetEscImg EscImgBuffer.data %x\n", EscImgBuffer.data);
    if(EscImg != NULL)
    {
        //if (EscImgBuffer.data != 0)
        //{
        //    IMG_FREE((void*)EscImgBuffer.data);
        //    EscImgBuffer.data = 0;
        //}
        
        //ret = outputFile.untileYUV(shot.YUV, EscImgBuffer);
        ret = outputFile.fetchEscYUV((ISPC::Buffer &)shot.YUV, EscImgBuffer);
        if(IMG_SUCCESS == ret)
        {
            EscImg->data = EscImgBuffer.data;
            EscImg->width = EscImgBuffer.width;
            EscImg->height = EscImgBuffer.height;
            EscImg->stride = EscImgBuffer.width;//EscImgBuffer.stride;
//                             ret = saveYUV(untiled);
        }
        else
        {
            ret = IMG_ERROR_FATAL;
        }
    }
    else
    {
        ret = IMG_ERROR_FATAL;
    }
    return ret;
}
#endif
//end added by linyun.xiong@2015-12-11
