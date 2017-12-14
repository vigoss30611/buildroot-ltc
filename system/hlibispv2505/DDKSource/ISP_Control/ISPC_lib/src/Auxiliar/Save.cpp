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
#include <ostream>
#include <iomanip>

#include "ispc/Camera.h"
#include "ispc/Pipeline.h"
#include "ispc/ModuleOUT.h"
#include "ispc/ISPCDefs.h"
#include "mc/mc_structs.h"

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

IMG_RESULT ISPC::Save::openDisplayFile(ePxlFormat pxlFmt, unsigned int w,
    unsigned int h)
{
    PIXELTYPE sType;
    struct SimImageInfo imageInfo;
    IMG_RESULT ret;

    // handles packed YUV444 too
    ret = PixelTransformDisplay(&sType, pxlFmt);
    if (ret)
    {
        LOG_ERROR("registered context does not have a correct RGB or YUV444 "
            "format: %s\n", FormatString(pxlFmt));
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if(PixelFormatIsPackedYcc(pxlFmt))
    {
        SaveFile_open(file, filename.c_str());
    }
    else
    {
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
            LOG_ERROR("unsupported format not RGB nor Ycc444 \n");
            return IMG_ERROR_FATAL;
        }

        ret = SaveFile_openFLX(file, filename.c_str(), &imageInfo);
        if (ret)
        {
            LOG_ERROR("failed to open '%s' as display output\n", filename.c_str());
        }
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

IMG_RESULT ISPC::Save::saveBayer(const ISPC::Buffer &bayerBuffer)
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

IMG_RESULT ISPC::Save::saveDisplay(const ISPC::Buffer &displayBuffer)
{
    IMG_RESULT ret;
    if (PixelFormatIsPackedYcc(displayBuffer.pxlFormat))
    {
        ret = saveYUV_Packed(displayBuffer);
    }
    else
    {
        ret = saveRGB(displayBuffer);
    }
    if (ret)
    {
        LOG_ERROR("failed to write a Display frame!\n");
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

IMG_RESULT ISPC::Save::saveYUV_Packed(const ISPC::Buffer &yuvBuffer)
{
    IMG_RESULT ret = IMG_NO_ERRORS;
    int stride = yuvBuffer.stride;
    int width = yuvBuffer.width;
    int height = yuvBuffer.height;

    PIXELTYPE stype;

    ret = PixelTransformDisplay(&stype, yuvBuffer.pxlFormat);

    width *= stype.ui8PackedStride;

    if (IMG_SUCCESS == ret)
    {
        ret = SaveFile_writeFrame(
            file,
            yuvBuffer.firstData(),
            stride, width, height);
        if (ret)
        {
            LOG_ERROR("failed to write YUV's luma frame!\n");
            return EXIT_FAILURE;
        }
    }

    return ret;
}

IMG_RESULT ISPC::Save::saveYUV(const ISPC::Buffer &yuvBuffer)
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

    if (yuvBuffer.isTiled)
    {
        LOG_ERROR("cannot saved tiled YUV buffer - untile it first!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    {
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

        ret = SaveFile_writeFrame(
            file,
            yuvBuffer.firstData(),
            lumaStride, lumaWidth, lumaHeight);
        if (ret)
        {
            LOG_ERROR("failed to write YUV's luma frame!\n");
            return EXIT_FAILURE;
        }

        ret = SaveFile_writeFrame(
            file,
            yuvBuffer.firstDataCbCr(),
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
    result.data = (const IMG_UINT8*)memory;
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

    ret = PixelTransformDisplay(&stype, rgbBuffer.pxlFormat);
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
    result.data = (const IMG_UINT8*)memory;
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

        case Display:
            ret = openDisplayFile(glb->displayType,
                size_info.ui32DispWidth, size_info.ui32DispHeight);
            break;

        case RGB_EXT:
            ret = openDisplayFile(glb->hdrExtractionType,
                size_info.ui32ImageWidth, size_info.ui32ImageHeight);
            break;

        case RGB_INS:
            ret = openDisplayFile(glb->hdrInsertionType,
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

IMG_RESULT ISPC::Save::save(const ISPC::Shot &shot)
{
    int ret = IMG_ERROR_NOT_INITIALISED;
    const unsigned int tileW = tileScheme;
    const unsigned int tileH = 4096 / tileScheme;

    if (file)
    {
        switch (type)
        {
        case Bayer:
            ret = saveBayer(shot.BAYER);
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
                ret = saveYUV(shot.YUV);
            }
            break;
        case Display:
            if (shot.DISPLAY.isTiled)
            {
                ISPC::Buffer untiled;
                ret = untileRGB(shot.DISPLAY, tileW, tileH, untiled);
                if (IMG_SUCCESS == ret)
                {
                    ret = saveDisplay(untiled);
                    IMG_FREE((void*)untiled.data);
                }
            }
            else
            {
                ret = saveDisplay(shot.DISPLAY);
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
    case Display:
        ret = untileRGB(shot.DISPLAY, tileW, tileH, result);
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

IMG_RESULT ISPC::Save::saveTxtStats(const ISPC::Shot &shot)
{
    IMG_RESULT ret = IMG_SUCCESS;
    char name[64];
    char buffer[128];
    int i, j, k;

#define PRINT_BUFF(name) \
    ret = SaveFile_write(file, buffer, strlen(buffer)); \
    if (ret) { \
            LOG_WARNING("failed to write " #name " to file\n"); \
    }

#define PRINT_ELEM_(container, element, elem_name, print) \
    snprintf(buffer, sizeof(buffer), SAVE_A3 "%s = %" print "\n", \
        elem_name, shot.metadata.container.element); \
    PRINT_BUFF(#container);

#define PRINT_ELEM_ARRAY(container, element, elem_name, print, n) \
    snprintf(buffer, sizeof(buffer), SAVE_A3 "%s = (%" print , elem_name, \
         shot.metadata.container.element[0]); \
    PRINT_BUFF(#container); \
    for (i = 1; i < n; i++) { \
        snprintf(buffer, sizeof(buffer), " %" print, \
            shot.metadata.container.element[i]); \
            PRINT_BUFF(#container); \
    } /* for */ \
    snprintf(buffer, sizeof(buffer), ")\n"); \
    PRINT_BUFF(#container);

#define PRINT_ELEM(container, element, print) \
    PRINT_ELEM_(container, element, #element, print)

#define PRINT_ELEM_A(container, element, print, n) \
    PRINT_ELEM_ARRAY(container, element, #element, print, n)
#define PRINT_ELEM_AA(container, element, print, n) \
    snprintf(name, sizeof(name), #element"[%d]", j); \
    PRINT_ELEM_ARRAY(container, element[j], name, print, n)
#define PRINT_ELEM_AAA(container, element, print, n) \
    snprintf(name, sizeof(name), #element"[%d][%d]", j, k); \
    PRINT_ELEM_ARRAY(container, element[j][k], name, print, n)

#define PRINT_ELEM_Ax(container, element, print) \
    snprintf(name, sizeof(name), #element"[%d]", i); \
    PRINT_ELEM_(container, element[i], name, print)
#define PRINT_ELEM_AAx(container, element, print) \
    snprintf(name, sizeof(name), #element"[%d][%d]", j, k); \
    PRINT_ELEM_(container, element[j], name, print)


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

    snprintf(buffer, sizeof(buffer), "Frame:\n");
    PRINT_BUFF("Frame header");

    /*
     * EXS
     */
    snprintf(buffer, sizeof(buffer),
        SAVE_A1 "Exposure Statistics:\n" SAVE_A2 "config:\n");
    PRINT_BUFF("EXS header");
    PRINT_ELEM(clippingConfig, bGlobalEnable, "d");
    PRINT_ELEM(clippingConfig, bRegionEnable, "d");
    PRINT_ELEM(clippingConfig, ui16Left, "u");
    PRINT_ELEM(clippingConfig, ui16Top, "u");
    PRINT_ELEM(clippingConfig, ui16Width, "u");
    PRINT_ELEM(clippingConfig, ui16Height, "u");
    PRINT_ELEM(clippingConfig, fPixelMax, "lf");

    snprintf(buffer, sizeof(buffer), SAVE_A2 "content:\n");
    PRINT_BUFF("EXS header");
    PRINT_ELEM_A(clippingStats, globalClipped, "u", 4);
    snprintf(buffer, sizeof(buffer),
        SAVE_A3 "# regionClipped[vertical][horizontal]\n");
    PRINT_BUFF("EXS comment");
    for (j = 0; j < EXS_V_TILES; j++)
    {
        for (k = 0; k < EXS_H_TILES; k++)
        {
            PRINT_ELEM_AAA(clippingStats, regionClipped, "u", 4);
        }
    }

    /*
     * HIS
     */
    snprintf(buffer, sizeof(buffer),
        "\n" SAVE_A1 "Histogram Statistics:\n" SAVE_A2 "config:\n");
    PRINT_BUFF("HIS header");
    PRINT_ELEM(histogramConfig, bGlobalEnable, "d");
    PRINT_ELEM(histogramConfig, bRegionEnable, "d");
    PRINT_ELEM(histogramConfig, fInputOffset, "lf");
    PRINT_ELEM(histogramConfig, fInputScale, "lf");
    PRINT_ELEM(histogramConfig, ui16Left, "u");
    PRINT_ELEM(histogramConfig, ui16Top, "u");
    PRINT_ELEM(histogramConfig, ui16Width, "u");
    PRINT_ELEM(histogramConfig, ui16Height, "u");

    snprintf(buffer, sizeof(buffer), SAVE_A2 "content:\n");
    PRINT_BUFF("HIS header");
    PRINT_ELEM_A(histogramStats, globalHistogram, "u", HIS_GLOBAL_BINS);
    snprintf(buffer, sizeof(buffer),
        SAVE_A3 "# regionHistograms[vertical][horizontal]\n");
    PRINT_BUFF("HIS comment");
    for (j = 0; j < HIS_REGION_VTILES; j++)
    {
        for (k = 0; k < HIS_REGION_HTILES; k++)
        {
            PRINT_ELEM_AAA(histogramStats, regionHistograms, "u",
                HIS_REGION_BINS);
        }
    }

    /*
     * FOS
     */
    snprintf(buffer, sizeof(buffer),
        "\n" SAVE_A1 "Focus Statistics:\n" SAVE_A2 "config:\n");
    PRINT_BUFF("FOS header");
    PRINT_ELEM(focusConfig, bGlobalEnable, "d");
    PRINT_ELEM(focusConfig, bRegionEnable, "d");
    PRINT_ELEM(focusConfig, ui16Left, "d");
    PRINT_ELEM(focusConfig, ui16Top, "d");
    PRINT_ELEM(focusConfig, ui16Width, "d");
    PRINT_ELEM(focusConfig, ui16Height, "d");
    PRINT_ELEM(focusConfig, ui16ROILeft, "d");
    PRINT_ELEM(focusConfig, ui16ROITop, "d");
    PRINT_ELEM(focusConfig, ui16ROIWidth, "d");
    PRINT_ELEM(focusConfig, ui16ROIHeight, "d");

    snprintf(buffer, sizeof(buffer), SAVE_A2 "content:\n");
    PRINT_BUFF("FOS header");
    PRINT_ELEM(focusStats, ROISharpness, "u");
    snprintf(buffer, sizeof(buffer),
        SAVE_A3 "# gridSharpness[vertical][horizontal]\n");
    PRINT_BUFF("FOS comment");
    for (j = 0; j < FOS_V_TILES; j++)
    {
        for (k = 0; k < FOS_H_TILES; k++)
        {
            PRINT_ELEM_AAx(focusStats, gridSharpness, "u");
        }
    }

    /*
     * WBS
     */
    snprintf(buffer, sizeof(buffer),
        "\n" SAVE_A1 "White Balance Statistics:\n" SAVE_A2 "config:\n");
    PRINT_BUFF("WBS header");
    PRINT_ELEM(whiteBalanceConfig, ui8ActiveROI, "u");
    PRINT_ELEM(whiteBalanceConfig, fRGBOffset, "lf");
    PRINT_ELEM(whiteBalanceConfig, fYOffset, "lf");
    for (i = 0; i < WBS_NUM_ROI; i++)
    {
        PRINT_ELEM_Ax(whiteBalanceConfig, aRoiLeft, "u");
        PRINT_ELEM_Ax(whiteBalanceConfig, aRoiTop, "u");
        PRINT_ELEM_Ax(whiteBalanceConfig, aRoiWidth, "u");
        PRINT_ELEM_Ax(whiteBalanceConfig, aRoiHeight, "u");
        PRINT_ELEM_Ax(whiteBalanceConfig, aRMax, "u");
        PRINT_ELEM_Ax(whiteBalanceConfig, aGMax, "u");
        PRINT_ELEM_Ax(whiteBalanceConfig, aBMax, "u");
        PRINT_ELEM_Ax(whiteBalanceConfig, aYMax, "u");
    }

    snprintf(buffer, sizeof(buffer), SAVE_A2 "content:\n");
    PRINT_BUFF("WBS header");
    for (k = 0; k < WBS_NUM_ROI; k++)
    {
        snprintf(name, sizeof(name), "whitePatch[%d].channelAccumulated", k);
        PRINT_ELEM_ARRAY(whiteBalanceStats, whitePatch[k].channelAccumulated,
            name, "u", 3);
        snprintf(name, sizeof(name), "whitePatch[%d].channelCount", k);
        PRINT_ELEM_ARRAY(whiteBalanceStats, whitePatch[k].channelCount,
            name, "u", 3);

        snprintf(name, sizeof(name), "averageColor[%d].channelCount", k);
        PRINT_ELEM_ARRAY(whiteBalanceStats,
            averageColor[k].channelAccumulated, name, "u", 3);

        snprintf(name, sizeof(name), "highLuminance[%d].channelAccumulated",
            k);
        PRINT_ELEM_ARRAY(whiteBalanceStats,
            highLuminance[k].channelAccumulated, name, "u", 3);
        snprintf(name, sizeof(name), "highLuminance[%d].lumaCount", k);
        PRINT_ELEM_(whiteBalanceStats, highLuminance[k].lumaCount, name, "u");
    }

    //  we may need a way to hide those stats if not available in HW
    /*
     * AWS
     */
    snprintf(buffer, sizeof(buffer),
        "\n" SAVE_A1 "Auto White Balance Statistics:\n" SAVE_A2 "config:\n");
    PRINT_BUFF("AWS header");
    PRINT_ELEM(autoWhiteBalanceConfig, bEnable, "d");
    PRINT_ELEM(autoWhiteBalanceConfig, bDebugBitmap, "d");
    PRINT_ELEM(autoWhiteBalanceConfig, fLog2_R_Qeff, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, fLog2_B_Qeff, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, fRedDarkThresh, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, fBlueDarkThresh, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, fGreenDarkThresh, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, fRedClipThresh, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, fBlueClipThresh, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, fGreenClipThresh, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, fBbDist, "lf");
    PRINT_ELEM(autoWhiteBalanceConfig, ui16GridStartRow, "u");
    PRINT_ELEM(autoWhiteBalanceConfig, ui16GridStartColumn, "u");
    PRINT_ELEM(autoWhiteBalanceConfig, ui16GridTileWidth, "u");
    PRINT_ELEM(autoWhiteBalanceConfig, ui16GridTileHeight, "u");

    //  we may need a way to hide those stats if not available in HW
    PRINT_ELEM_A(autoWhiteBalanceConfig, aCurveCoeffX, "lf", AWS_LINE_SEG_N);
    PRINT_ELEM_A(autoWhiteBalanceConfig, aCurveCoeffY, "lf", AWS_LINE_SEG_N);
    PRINT_ELEM_A(autoWhiteBalanceConfig, aCurveOffset, "lf", AWS_LINE_SEG_N);
    PRINT_ELEM_A(autoWhiteBalanceConfig, aCurveBoundary, "lf", AWS_LINE_SEG_N);

    snprintf(buffer, sizeof(buffer), SAVE_A2 "content:\n");
    PRINT_BUFF("AWS header");

    snprintf(buffer, sizeof(buffer),
        SAVE_A3 "# tileStats[vertical][horizontal]\n");
    PRINT_BUFF("AWS comment");
    for (j = 0; j < AWS_NUM_GRID_TILES_VERT; j++)
    {
        for (k = 0; k < AWS_NUM_GRID_TILES_HORIZ; k++)
        {
            snprintf(name, sizeof(name),
                "autoWhiteBalanceStats.tileStats[%d][%d].fCollectedRed",
                j, k);
            PRINT_ELEM_(autoWhiteBalanceStats,
                tileStats[j][k].fCollectedRed, name, "lf");
            snprintf(name, sizeof(name),
                "autoWhiteBalanceStats.tileStats[%d][%d].fCollectedBlue",
                j, k);
            PRINT_ELEM_(autoWhiteBalanceStats,
                tileStats[j][k].fCollectedBlue, name, "lf");
            snprintf(name, sizeof(name),
                "autoWhiteBalanceStats.tileStats[%d][%d].fCollectedGreen",
                j, k);
            PRINT_ELEM_(autoWhiteBalanceStats,
                tileStats[j][k].fCollectedGreen, name, "lf");
            snprintf(name, sizeof(name),
                "autoWhiteBalanceStats.tileStats[%d][%d].numCFAs",
                j, k);
            PRINT_ELEM_(autoWhiteBalanceStats,
                tileStats[j][k].numCFAs, name, "u");
        }
    }

    /*
     * FLD
     */
    snprintf(buffer, sizeof(buffer),
        "\n" SAVE_A1 "Flicker Detection:\n" SAVE_A2 "config:\n");
    PRINT_BUFF("FLD header");
    PRINT_ELEM(flickerConfig, bEnable, "d");
    PRINT_ELEM(flickerConfig, ui16VTot, "u");
    PRINT_ELEM(flickerConfig, fFrameRate, "lf");
    PRINT_ELEM(flickerConfig, ui16CoefDiff, "u");
    PRINT_ELEM(flickerConfig, ui16NFThreshold, "u");
    PRINT_ELEM(flickerConfig, ui32SceneChange, "u");
    PRINT_ELEM(flickerConfig, ui8RShift, "u");
    PRINT_ELEM(flickerConfig, ui8MinPN, "u");
    PRINT_ELEM(flickerConfig, ui8PN, "u");
    PRINT_ELEM(flickerConfig, bReset, "d");

    snprintf(buffer, sizeof(buffer), SAVE_A2 "content:\n");
    PRINT_BUFF("FLD header");

    snprintf(buffer, sizeof(buffer), SAVE_A3 "# status: %d=WAITING, %d=50Hz, "\
        "%d=60Hz, %d=NO_FLICKER, %d=UNDERTERMINED\n",
        FLD_WAITING, FLD_50HZ, FLD_60HZ, FLD_NO_FLICKER, FLD_UNDETERMINED);
    PRINT_BUFF("FLD comment");
    PRINT_ELEM(flickerStats, flickerStatus, "d");

    /*
     * Timestamps
     */
    snprintf(buffer, sizeof(buffer), "\n" SAVE_A1 "Timestamps:\n" SAVE_A2 "content:\n");
    PRINT_BUFF("TS header");
    PRINT_ELEM(timestamps, ui32LinkedListUpdated, "u");
    PRINT_ELEM(timestamps, ui32StartFrameIn, "u");
    PRINT_ELEM(timestamps, ui32EndFrameIn, "u");
    PRINT_ELEM(timestamps, ui32StartFrameOut, "u");
    PRINT_ELEM(timestamps, ui32EndFrameEncOut, "u");
    PRINT_ELEM(timestamps, ui32EndFrameDispOut, "u");
    PRINT_ELEM(timestamps, ui32InterruptServiced, "u");

    /*
     * DPF
     */
    snprintf(buffer, sizeof(buffer),
        "\n" SAVE_A1 "Defective Pixels:\n" SAVE_A2 "config:\n");
    PRINT_BUFF("DPF header");
    snprintf(buffer, sizeof(buffer), SAVE_A3 "# bitmask of %d=DETECT, "\
        "%d=WRITE, %d=READ\n",
        CI_DPF_DETECT_ENABLED, CI_DPF_WRITE_MAP_ENABLED,
        CI_DPF_READ_MAP_ENABLED);
    PRINT_BUFF("DPF comment");
    PRINT_ELEM(defectiveConfig, eDPFEnable, "u");
    PRINT_ELEM(defectiveConfig, ui32NDefect, "u");
    snprintf(buffer, sizeof(buffer), SAVE_A3 "# pair of coordinate (x, y)\n");
    PRINT_BUFF("DPF comment");
    if (shot.metadata.defectiveConfig.ui32NDefect > 0
        && shot.metadata.defectiveConfig.apDefectInput)
    {
        snprintf(buffer, sizeof(buffer), SAVE_A3 "apDefectInput =");
        PRINT_BUFF("defectiveConfig.apDefectInput");

        for (i = 0; i < (int)shot.metadata.defectiveConfig.ui32NDefect; i++)
        {
            snprintf(buffer, sizeof(buffer), " (%d %d)",
                shot.metadata.defectiveConfig.apDefectInput[2 * i + 0],
                shot.metadata.defectiveConfig.apDefectInput[2 * i + 1]);
            PRINT_BUFF("defectiveConfig.apDefectInput");
        }
    }
    else
    {
        snprintf(buffer, sizeof(buffer), SAVE_A3 "apDefectInput = null\n");
        PRINT_BUFF("defectiveConfig.apDefectInput");
    }
    PRINT_ELEM(defectiveConfig, ui16NbLines, "u");

    PRINT_ELEM_A(defectiveConfig, aSkip, "u", 2);
    PRINT_ELEM_A(defectiveConfig, aOffset, "u", 2);
    PRINT_ELEM(defectiveConfig, ui8Threshold, "u");
    PRINT_ELEM(defectiveConfig, fWeight, "lf");

    snprintf(buffer, sizeof(buffer), SAVE_A2 "content:\n");
    PRINT_BUFF("DPF header");
    PRINT_ELEM(defectiveStats, ui32FixedPixels, "u");
    PRINT_ELEM(defectiveStats, ui32MapModifications, "u");
    PRINT_ELEM(defectiveStats, ui32DroppedMapModifications, "u");

    PRINT_ELEM(defectiveStats, ui32NOutCorrection, "u");
    snprintf(buffer, sizeof(buffer),
        SAVE_A3 "# tuple of (x, y, data, confidence, status)\n");
    PRINT_BUFF("DPF header");
    if (shot.metadata.defectiveStats.ui32NOutCorrection > 0)
    {
        const int coordMask = DPF_MAP_COORD_X_COORD_MASK \
            | DPF_MAP_COORD_Y_COORD_MASK;
        const int dataMask = DPF_MAP_DATA_STATUS_MASK \
            | DPF_MAP_DATA_CONFIDENCE_MASK | DPF_MAP_DATA_PIXEL_MASK;

        const IMG_UINT32 *pDPFOut = (const IMG_UINT32 *)shot.DPF.data;

        snprintf(buffer, sizeof(buffer), SAVE_A3 "apDefectOutput =");
        PRINT_BUFF("DPF header");
        for (i = 0; i < (int)shot.metadata.defectiveStats.ui32NOutCorrection;
            i++)
        {
            int vC, vD;  // coordinate and data
            int x, y, d, c, s;

            vC = pDPFOut[2 * i + DPF_MAP_COORD_OFFSET / 4];
            vD = pDPFOut[2 * i + DPF_MAP_DATA_OFFSET / 4];

            x = (vC&DPF_MAP_COORD_X_COORD_MASK) >> DPF_MAP_COORD_X_COORD_SHIFT;
            y = (vC&DPF_MAP_COORD_Y_COORD_MASK) >> DPF_MAP_COORD_Y_COORD_SHIFT;
            d = (vD&DPF_MAP_DATA_PIXEL_MASK) >> DPF_MAP_DATA_PIXEL_SHIFT;
            c = (vD&DPF_MAP_DATA_CONFIDENCE_MASK)
                >> DPF_MAP_DATA_CONFIDENCE_SHIFT;
            s = (vD&DPF_MAP_DATA_STATUS_MASK) >> DPF_MAP_DATA_STATUS_SHIFT;

            snprintf(buffer, sizeof(buffer), " (%d %d %d %d %d)",
                x, y, d, c, s);
            PRINT_BUFF("DPF.data");
        }
        snprintf(buffer, sizeof(buffer), "\n");
        PRINT_BUFF("DPF.data");
    }
    else
    {
        snprintf(buffer, sizeof(buffer), SAVE_A3 "apDefectOutput = null\n");
        PRINT_BUFF("defectiveConfig.apDefectInput");
    }

    /*
     * ENS
     */
    snprintf(buffer, sizeof(buffer),
        "\n" SAVE_A1 "Encoder Statistics\n" SAVE_A2 "config:\n");
    PRINT_BUFF("ENS header");

    PRINT_ELEM(encodeStatsConfig, bEnable, "d");
    PRINT_ELEM(encodeStatsConfig, ui32NLines, "u");
    PRINT_ELEM(encodeStatsConfig, ui32KernelSubsampling, "u");

    snprintf(buffer, sizeof(buffer),
        SAVE_A2 "content:\n");
    PRINT_BUFF("ENS header");
    int nEntries = 0;
    if (shot.ENS.elementSize > 0)
    {
        nEntries = shot.ENS.size / shot.ENS.elementSize;
    }
    snprintf(buffer, sizeof(buffer), SAVE_A3 "ui32NOutElements = %d\n", nEntries);
    PRINT_BUFF("ENS.ui32NOutElements");

    snprintf(buffer, sizeof(buffer), SAVE_A3 "# pair of (counter, entropy)\n");
    PRINT_BUFF("ENS comment");
    if (shot.ENS.size > 0 && shot.ENS.elementSize > 0)
    {
        const IMG_UINT32 *pENSOut = (const IMG_UINT32 *)(shot.ENS.data);

        snprintf(buffer, sizeof(buffer), SAVE_A3 "apStatsOutput =");
        PRINT_BUFF("ENS.data");

        for (i = 0; i < nEntries; i++)
        {
            int c, e;  // count, entropy

            c = pENSOut[2 * i + 0];
            e = pENSOut[2 * i + 1];

            snprintf(buffer, sizeof(buffer), " (%d %d)", c, e);
            PRINT_BUFF("ENS.ui32NOutElements");
        }

        snprintf(buffer, sizeof(buffer), "\n");
        PRINT_BUFF("ENS.data");
    }
    else
    {
        snprintf(buffer, sizeof(buffer),
            SAVE_A3 "ui32NOutElements = 0\n" SAVE_A3 "apStatsOutput = null\n");
        PRINT_BUFF("ENS.data");
    }

    return IMG_SUCCESS;

#undef PRINT_ELEM_AAx
#undef PRINT_ELEM_Ax
#undef PRINT_ELEM_AAA
#undef PRINT_ELEM_AA
#undef PRINT_ELEM_A
#undef PRINT_ELEM
#undef PRINT_ELEM_
#undef PRINT_BUFF
}

IMG_RESULT ISPC::Save::saveCameraState(const ISPC::Camera &camera)
{
    std::ostringstream out;
    std::string res;
    IMG_RESULT ret;

    out << camera.state << std::endl;
    res = out.str();

    ret = SaveFile_write(file, res.c_str(), res.size());
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to save camera state\n");
    }
    return ret;
}

//
// public static
//

std::string ISPC::Save::getEncoderFilename(const ISPC::Pipeline &context)
{
    std::stringstream filename;
    PIXELTYPE fmt;
    const ISPC::ModuleOUT *glb = context.getModule<ISPC::ModuleOUT>();

    if(PixelTransformYUV(&fmt, glb->encoderType) != IMG_SUCCESS)
    {
        LOG_ERROR("Invalid encoder format!");
        return "";
    }
    filename << "encoder" << (unsigned int)context.ui8ContextNumber << "_" <<
        context.getMCPipeline()->sESC.aOutputSize[0] << "x" <<
        context.getMCPipeline()->sESC.aOutputSize[1] << "_" <<
        FormatStringForFilename(glb->encoderType) << "_align" <<
        (unsigned int)fmt.ui8PackedStride << ".yuv";

    return filename.str();
}

std::string ISPC::Save::getDisplayFilename(const ISPC::Pipeline &context)
{
    std::stringstream filename;
    PIXELTYPE fmt;
    const ISPC::ModuleOUT *glb = context.getModule<ISPC::ModuleOUT>();

    if(PixelTransformDisplay(&fmt, glb->displayType) != IMG_SUCCESS)
    {
        LOG_ERROR("Invalid display format!");
        return "";
    }

    filename << "display" << (unsigned int)context.ui8ContextNumber;
    if(IMG_FALSE == PixelFormatIsPackedYcc(glb->displayType))
    {
        filename << ".flx";
    } else {
        filename << "_" <<
            context.getMCPipeline()->sDSC.aOutputSize[0] << "x" <<
            context.getMCPipeline()->sDSC.aOutputSize[1] << "_" <<
            FormatStringForFilename(glb->displayType) << "_align" <<
            (unsigned int)fmt.ui8PackedStride << ".yuv";
    }

    return filename.str();
}

IMG_RESULT ISPC::Save::Single(const ISPC::Pipeline &context,
    const ISPC::Shot &shot, ISPC::Save::SaveType type,
    const std::string &filename)
{
    IMG_RESULT ret;
    if (Save::Bytes == type)
    {
        LOG_ERROR("ISPC::Save::Bytes is not supported - use a specific" \
            " function\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    Save outputFile;

    ret = outputFile.open(type, context, filename);
    if (IMG_SUCCESS == ret)
    {
        ret = outputFile.save(shot);
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

IMG_RESULT ISPC::Save::SingleTxtStats(const ISPC::Camera &camera,
    const ISPC::Shot &shot, const std::string &filename)
{
    IMG_RESULT ret = IMG_SUCCESS;
    Save outputFile;
    const Pipeline &context = *camera.getPipeline();

    ret = outputFile.open(Save::Bytes, context, filename);
    if (IMG_SUCCESS == ret)
    {
        ret = outputFile.saveCameraState(camera);
        if (ret)
        {
            LOG_ERROR("failed to save camera state to '%s'\n",
                filename.c_str());
        }
        else
        {
            ret = outputFile.saveTxtStats(shot);
            if (ret)
            {
                LOG_ERROR("failed to save text statistics output to '%s'\n",
                    filename.c_str());
            }
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

std::ostream& operator<<(std::ostream &os, const CI_HWINFO &info)
{
    char prev;
    int s;
    os << "HW Information:" << std::endl;
    os << SAVE_A1 << "version = "
        << (int)info.rev_ui8Major << "." << (int)info.rev_ui8Minor << "."
        << (int)info.rev_ui8Maint << std::endl;
    prev = os.fill('0');
    os << SAVE_A1 << "config = " << std::setw(3)
        << info.ui16ConfigId << std::endl;
    os << SAVE_A1 << "uid = " << std::setw(8) << info.rev_uid << std::endl;
    os.fill(prev);

    os << SAVE_A1 << "parallelism = "
        << (int)info.config_ui8Parallelism << std::endl;
    os << SAVE_A1 << "pixels per clock = "
        << (int)info.config_ui8PixPerClock << std::endl;
    os << SAVE_A1 << "bit depth = "
        << (int)info.config_ui8BitDepth << std::endl;
    os << SAVE_A1 << "contexts = "
        << (int)info.config_ui8NContexts << std::endl;
    os << SAVE_A1 << "gaskets = "
        << (int)info.config_ui8NImagers << std::endl;
    os << SAVE_A1 << "iif datagen = "
        << (int)info.config_ui8NIIFDataGenerator << std::endl;
    os << SAVE_A1 << "max linestore = "
        << info.ui32MaxLineStore << std::endl;
    os << SAVE_A1 << "DPF internal = "
        << info.config_ui32DPFInternalSize << std::endl;
    os << SAVE_A1 << "LSH RAM size = "
        << info.ui32LSHRamSizeBits << " bits" << std::endl;
    os << SAVE_A1 << "features =";

    if (info.eFunctionalities == CI_INFO_SUPPORTED_NONE)
    {
        os << " none";
    }
    if (info.eFunctionalities & CI_INFO_SUPPORTED_TILING)
    {
        os << " TILING";
    }
    if (info.eFunctionalities & CI_INFO_SUPPORTED_HDR_EXT)
    {
        os << " HDR_EXT";
    }
    if (info.eFunctionalities & CI_INFO_SUPPORTED_HDR_INS)
    {
        os << " HDR_INS";
    }
    if (info.eFunctionalities & CI_INFO_SUPPORTED_RAW2D_EXT)
    {
        os << " RAW2D_EXT";
    }
    if (info.eFunctionalities & CI_INFO_SUPPORTED_IIF_DATAGEN)
    {
        os << " IIF_DG";
    }
    if (info.eFunctionalities & CI_INFO_SUPPORTED_IIF_PARTERNGEN)
    {
        os << " IIF_DG_PATTERN";
    }
    os << std::endl;
#if defined(FELIX_FAKE)
    os << SAVE_A1 << "# using driver for CSIM - clock is not real"
        << std::endl;
#endif
    os << SAVE_A1 << "ref clock = "
        << info.ui32RefClockMhz << " Mhz" << std::endl;

    for (s = 0; s < info.config_ui8NContexts; s++)
    {
        os << std::endl;
        os << SAVE_A1 << "Context " << s << ":" << std::endl;
        os << SAVE_A2 << "bit depth = "
            << (int)info.context_aBitDepth[s] << std::endl;
        os << SAVE_A2 << "max size (single) = "
            << info.context_aMaxWidthSingle[s] << "x"
            << info.context_aMaxHeight[s] << std::endl;
        os << SAVE_A2 << "max size (multi) = "
            << info.context_aMaxWidthSingle[s] << "x"
            << info.context_aMaxHeight[s] << std::endl;
    }

    for (s = 0; s < info.config_ui8NImagers; s++)
    {
        os << std::endl;
        os << SAVE_A1 << "Gasket " << s << ":" << std::endl;
        os << SAVE_A2 << "bit depth = "
            << (int)info.gasket_aBitDepth[s] << std::endl;
        os << SAVE_A2 << "format = ";
        if (info.gasket_aType[s] == CI_GASKET_NONE)
        {
            os << " none";
        }
        if (info.gasket_aType[s] & CI_GASKET_MIPI)
        {
            os << " MIPI";
        }
        if (info.gasket_aType[s] & CI_GASKET_PARALLEL)
        {
            os << " PARALLEL";
        }
        os << std::endl;

        if (info.gasket_aType[s] & CI_GASKET_MIPI)
        {
            os << SAVE_A2 << "MIPI lanes = "
                << (int)info.gasket_aImagerPortWidth[s] << std::endl;
        }
    }
    os << std::endl;
    os << "MMU version " << (int)info.mmu_ui8RevMajor << "."
        << (int)info.mmu_ui8RevMinor << "." << (int)info.mmu_ui8RevMaint
        << std::endl;

    os << std::endl;
    os << "SW Information:" << std::endl;
    os << SAVE_A1 << "changelist = " << info.uiChangelist << std::endl;
    os << SAVE_A1 << "mmucontrol = " << (int)info.mmu_ui8Enabled << std::endl;
    os << SAVE_A1 << "tiling scheme = "
        << info.uiTiledScheme << "x" << 4096 / info.uiTiledScheme << std::endl;
    os << SAVE_A1 << "mmu page size = " << info.mmu_ui32PageSize << std::endl;

    os << std::endl;
    return os;
}
