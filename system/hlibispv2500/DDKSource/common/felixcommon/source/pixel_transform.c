/**
*******************************************************************************
 @file pixel_transform.c

 @brief

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
#include <felixcommon/pixel_format.h>

#include <img_defs.h>
#include <img_errors.h>

#define LOG_TAG "PIXEL_TRANSFORM"
#include <felixcommon/userlog.h>

// define the transform matrices

const double BT709_coeff[9] = {
    +1.0,+0.0,+1.57492, // = R
    +1.0,-0.1873243,-0.468159932, // = G
    +1.0,+1.8556,+0.0 // = B
};

const double BT709_Inputoff[3] = { -16, -128, -128 };

const double BT601_coeff[9] = {
    +1.164,+0.0,+1.540,
    +1.164,-0.391,-0.813,
    +1.164,+2.018,+0.0
};

const double BT601_Inputoff[3] = { -16, -128, -128 };

const double JFIF_coeff[9] = {
    +1.0,+0.0,+1.402,
    +1.0,-0.34414,-0.71414,
    +1.0,+1.772,+0.0
};

const double JFIF_Inputoff[3] = { 0, -128, -128 };


IMG_RESULT PixelTransformYUV(PIXELTYPE *pType, enum pxlFormat yuv)
{
    if (PXL_NONE == yuv)
    {
        pType->eBuffer = TYPE_NONE;
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pType->eFmt = yuv;
    pType->eBuffer = TYPE_YUV;
    pType->eMosaic = MOSAIC_NONE;

    if (YVU_420_PL12_8 == yuv || YUV_420_PL12_8 == yuv
        || YVU_420_PL12_10 == yuv || YUV_420_PL12_10 == yuv) // 420
    {
        pType->ui8HSubsampling = 2;
        pType->ui8VSubsampling = 2;
    }
    else if (YVU_422_PL12_8 == yuv || YUV_422_PL12_8 == yuv
        || YVU_422_PL12_10 == yuv || YUV_422_PL12_10 == yuv) // 422
    {
        pType->ui8HSubsampling = 2;
        pType->ui8VSubsampling = 1;
    }
    else
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (YVU_420_PL12_8 == yuv || YUV_420_PL12_8 == yuv
        || YVU_422_PL12_8 == yuv || YUV_422_PL12_8 == yuv) // 8b
    {
        pType->ui8BitDepth = 8; // bits

        pType->ui8PackedElements = 1;
        pType->ui8PackedStride = 1; // Bytes
        pType->ePackedStart = PACKED_LSB;
    }
    else if (YVU_420_PL12_10 == yuv || YUV_420_PL12_10 == yuv
        || YVU_422_PL12_10 == yuv || YUV_422_PL12_10 == yuv)// 10b packed
    {
        pType->ui8BitDepth = 10; // bits

        // 3 or 6 because it is {CR0,CB0,CR1,XX}{CB1,CR2,CB2,XX} 
        // for {Y0,Y1,Y2,XX}{Y3,Y4,Y5,XX}
        pType->ui8PackedElements = 6; 
        pType->ui8PackedStride = 8; // Bytes
        pType->ePackedStart = PACKED_LSB;
    }
    else
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT PixelTransformRGB(PIXELTYPE *pType, enum pxlFormat rgb)
{
    if (PXL_NONE == rgb)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pType->eFmt = rgb;
    pType->eBuffer = TYPE_RGB;
    pType->eMosaic = MOSAIC_NONE;
    pType->ui8HSubsampling = 1;
    pType->ui8VSubsampling = 1;

    if (RGB_888_24 == rgb || BGR_888_24 == rgb) // in 24b
    {
        pType->ui8BitDepth = 8;

        pType->ui8PackedElements = 1;
        pType->ui8PackedStride = 3;
        pType->ePackedStart = PACKED_LSB;
    }
    else if (RGB_888_32 == rgb || BGR_888_32 == rgb) // in 32b
    {
        pType->ui8BitDepth = 8;

        pType->ui8PackedElements = 1;
        pType->ui8PackedStride = 4;
        pType->ePackedStart = PACKED_LSB;
    }
    else if (RGB_101010_32 == rgb || BGR_101010_32 == rgb) // 10b in 32b
    {
        pType->ui8BitDepth = 10;

        pType->ui8PackedElements = 1;
        pType->ui8PackedStride = 4;
        pType->ePackedStart = PACKED_LSB;
    }
    else if (BGR_161616_64 == rgb) // 16b in 64b
    {
        pType->ui8BitDepth = 16;

        pType->ui8PackedElements = 1;
        pType->ui8PackedStride = 8;
        pType->ePackedStart = PACKED_LSB;
    }
    else
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

IMG_RESULT PixelTransformBayer(PIXELTYPE *pType, ePxlFormat bayer, 
    enum MOSAICType mosaic)
{
    if (PXL_NONE == bayer)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pType->eFmt = bayer;
    pType->eBuffer = TYPE_BAYER;
    pType->eMosaic = mosaic;

    pType->ui8HSubsampling = 1;
    pType->ui8VSubsampling = 1;

    pType->ePackedStart = PACKED_LSB;
    if (BAYER_RGGB_8 == bayer)
    {
        pType->ui8BitDepth = 8;
        pType->ui8PackedElements = 4;
        pType->ui8PackedStride = 4;
    }
    else if (BAYER_RGGB_10 == bayer)
    {
        pType->ui8BitDepth = 10;
        pType->ui8PackedElements = 3;
        pType->ui8PackedStride = 4;
    }
    else if (BAYER_RGGB_12 == bayer)
    {
        pType->ui8BitDepth = 12;
        pType->ui8PackedElements = 16; // 8 pair of RG/GB in 1 bop
        pType->ui8PackedStride = 24; // 24 Bytes
    }
    else if (BAYER_TIFF_10 == bayer)
    {
        pType->ePackedStart = PACKED_MSB; // TIFF is stored in big endian style
        pType->ui8BitDepth = 10;
        pType->ui8PackedElements = 4;
        pType->ui8PackedStride = 5;
    }
    else if (BAYER_TIFF_12 == bayer)
    {
        pType->ePackedStart = PACKED_MSB; // TIFF is stored in big endian style
        pType->ui8BitDepth = 12;
        pType->ui8PackedElements = 2;
        pType->ui8PackedStride = 3;
    }
    else
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return IMG_SUCCESS;
}

/*
 * buffer transformation
 */

static void duplicate8(IMG_UINT8 *input, IMG_UINT8 *output, size_t outStride, 
    IMG_UINT8 ui8Hsub, IMG_UINT8 ui8Vsub)
{
    int i, j;
    for ( i = 0 ; i < ui8Vsub ; i++ )
    {
        for ( j = 0 ; j < ui8Hsub ; j++ )
        {
            output[i*outStride+j] = input[0];
        }
    }
}

static void duplicate16(IMG_UINT8 *input8, IMG_UINT8 *output8, 
    size_t outStride, IMG_UINT8 ui8Hsub, IMG_UINT8 ui8Vsub)
{
    IMG_UINT16 *input = (IMG_UINT16*)input8;
    IMG_UINT16 *output = (IMG_UINT16*)output8;

    int i, j;
    for ( i = 0 ; i < ui8Vsub ; i++ )
    {
        for ( j = 0 ; j < ui8Hsub ; j++ )
        {
            // stride/2 because stride is in bytes not in word
            output[i*(outStride/2)+j] = input[0];
        }
    }
}

unsigned char* BufferTransformYUVTo444(IMG_UINT8 *bufferA, IMG_BOOL8 bUV, 
    ePxlFormat eTypeA, size_t inWidth, size_t inHeight, size_t outWidth, 
    size_t outHeight)
{
    IMG_UINT8 *result = NULL;
    IMG_UINT8 *originalUV = NULL,
        *outputU = NULL,
        *outputV = NULL;
    PIXELTYPE type;
    size_t sizeofPixel = 1;
    size_t i = 0, j = 0;
    void (*pfnUpsampler)(IMG_UINT8 *in, IMG_UINT8 *out, size_t outStride, 
        IMG_UINT8 Hsub, IMG_UINT8 Vsub) = &duplicate8;

    if (0 == inWidth*inHeight || outWidth > inWidth || outHeight > inHeight )
    {
        return NULL;
    }

    if(PixelTransformYUV(&type, eTypeA)!=IMG_SUCCESS) {
        return NULL;
    }

    if (8 < type.ui8BitDepth)
    {
        pfnUpsampler = &duplicate16;
        return NULL; // not supported yet for 10b formats
    }

    result = (IMG_UINT8*)IMG_MALLOC(3* outWidth*outHeight *sizeofPixel);
    if (!result)
    {
        return NULL;
    }

    originalUV = bufferA + inWidth*inHeight *sizeofPixel;
    if (bUV)
    {
        outputU = result + outWidth*outHeight *sizeofPixel;
        outputV = outputU + outWidth*outHeight *sizeofPixel;
    }
    else
    {
        outputV = result + outWidth*outHeight *sizeofPixel;
        outputU = outputV + outWidth*outHeight *sizeofPixel;
    }

    // Y does not change
    if (inWidth == outWidth && inHeight == outHeight)
    {
        memcpy(result, bufferA, inWidth*inHeight *sizeofPixel);
    }
    else // different stride means we have to do line by line
    {
        for ( i = 0 ; i < outHeight ; i++ )
        {
            memcpy(result + (i*outWidth*sizeofPixel), bufferA + 
                (i*inWidth*sizeofPixel), (outWidth*sizeofPixel));
        }
    }

    // process U and V - deinterlace
    for ( i = 0 ; i < outHeight/type.ui8VSubsampling ; i++ )
    {
        // *2 because it is U and V in the same plane
        for ( j = 0 ; j < (outWidth/type.ui8HSubsampling)*2 ; j+=2 )
        {
            // copy U
            pfnUpsampler(&(originalUV[i*inWidth + j]), 
                &(outputU[type.ui8VSubsampling*i*outWidth 
                + (type.ui8HSubsampling*j)/2]), outWidth*sizeofPixel, 
                type.ui8HSubsampling, type.ui8VSubsampling);
            // copy V
            pfnUpsampler(&(originalUV[i*inWidth + j+1]), 
                &(outputV[type.ui8VSubsampling*i*outWidth 
                + (type.ui8HSubsampling*j)/2]), outWidth*sizeofPixel, 
                type.ui8HSubsampling, type.ui8VSubsampling);
        }
    }

    return result;
}

unsigned char clip(double v)
{
    if (255 < v) v = 255;
    if (0 > v) v = 0;
    return (unsigned char)v;
}

unsigned char* BufferTransformYUV444ToRGB(IMG_UINT8 *bufferA, IMG_BOOL8 bUV, 
    ePxlFormat rgb, IMG_BOOL8 bBGR, size_t width, size_t height, 
    const double convMatrix[9], const double convInputOff[3])
{
    IMG_UINT8 *result = NULL,
        *inputU, *inputV;
    PIXELTYPE rgbType;
    IMG_UINT32 bop = 0;
    size_t allocWidth = 0;
    size_t i, j, k;
    int kOut[3] = { 0, 1, 2 };
    IMG_RESULT ret;

    ret = PixelTransformRGB(&rgbType, rgb);
    if(IMG_SUCCESS!=ret) {
        return NULL;
    }

    bop = width/rgbType.ui8PackedElements;
    if (0 != width%rgbType.ui8PackedElements)
    {
        bop++;
    }

    allocWidth = bop*rgbType.ui8PackedStride;

    result = (IMG_UINT8*)IMG_CALLOC(bop*height, rgbType.ui8PackedStride);
    if (!result)
    {
        return NULL;
    }

    if (bUV)
    {
        inputU = bufferA + width*height;
        inputV = inputU + width*height;
    }
    else
    {
        inputV = bufferA + width*height;
        inputU = inputV + width*height;
    }
    if (bBGR)
    {
        kOut[0] = 2;
        kOut[2] = 0;
    }

    // that is output RGB or BGR in function of what is in kOut[]
    for ( j = 0 ; j < height ; j++ )
    {
        for ( i = 0 ; i < width ; i++ )
        {
            for( k = 0 ; k < 3 ; k++ )
            {
                double v;
                v = (bufferA[j*width + i]+convInputOff[0])*convMatrix[3*k+0] 
                    + (inputU[j*width + i]+convInputOff[1])*convMatrix[3*k+1] 
                    + (inputV[j*width + i]+convInputOff[2])*convMatrix[3*k+2];
                result[j*allocWidth + (rgbType.ui8PackedStride*i)+kOut[k]]
                    = clip(v);
            }
        }
    }

    return result;
}

IMG_RESULT BufferDeTile(IMG_UINT32 ui32TileWidth, IMG_UINT32 ui32TileHeight, 
    IMG_UINT32 ui32TileStride, const IMG_UINT8 *pInput, IMG_UINT8 *pOutput, 
    IMG_UINT32 ui32OutputHStride, IMG_UINT32 ui32OutputVStride)
{
    IMG_UINT32 nTiles_vertical = 0, nTiles_horizontal = 0;
    IMG_UINT32 ipixel = 0, ti = 0, j = 0, tj = 0;
    IMG_UINT32 buffPos = 0;

    if (!pInput || !pOutput)
    {
        LOG_ERROR("pInput or pOutput is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (0 == ui32TileWidth || 0 == ui32TileHeight || 0 == ui32TileStride
        || 0 == ui32OutputHStride || 0 == ui32OutputVStride)
    {
        LOG_ERROR("one of the size is 0 (ui32TileWidth=%d, ui32TileHeight=%d,"\
            " ui32TileStride=%d, ui32OutputHStride=%d, ui32OutputVStride=%d)\n"
            , ui32TileWidth, ui32TileHeight, ui32TileStride, ui32OutputHStride,
            ui32OutputVStride);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (ui32TileStride > ui32OutputHStride || ui32TileWidth > ui32TileStride)
    {
        LOG_ERROR("ui32TileStride is bigger than ui32OutputHStride (%d>%d) or"\
            " tile width than tile stride (width %d)\n", 
            ui32TileStride, ui32OutputHStride, ui32TileWidth);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (0 != ui32OutputHStride%ui32TileWidth)
    {
        LOG_ERROR("strides are not divisible by the tile sizes\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (0 != ui32TileStride%ui32TileWidth
        || 0 != ui32OutputHStride%ui32TileStride)
    {
        LOG_ERROR("tile stride is not compatible with the output size or the" \
            " tile width (ui32TileStride%cui32TileWidth=%d," \
            " ui32OutputHStride%cui32TileStride=%d)\n",
            '%', ui32TileStride%ui32TileWidth, '%', 
            ui32OutputHStride%ui32TileStride);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if ( ui32TileStride < ui32OutputHStride )
    {
        // bad to modify parameters but they are on stack
        ui32OutputVStride *= (ui32OutputHStride/ui32TileStride); 
        // bad to modify parameters but they are on stack
        ui32OutputHStride = ui32TileStride; 
    }

    nTiles_horizontal = ui32TileStride/ui32TileWidth;
    nTiles_vertical = ui32OutputVStride/ui32TileHeight;

    //round up when bottom tiles are not full height
    if(0 != ui32OutputVStride%ui32TileHeight)
    {
        nTiles_vertical++;
    }

    j= 0; // Y position in output buffer current tile
    ti = 0; // tile X id (0 to nTiles_horizontal)
     tj = 0; // tile Y id (0 to nTiles_vertical)
    ipixel = 0;
    for ( tj = 0 ; tj < nTiles_vertical ; tj++ )
    {
        for ( ti = 0 ; ti < nTiles_horizontal ; ti++ )
        {
            for ( j = 0 ; 
                j < ui32TileHeight && tj*ui32TileHeight+j < ui32OutputVStride;
                j++ )
            {

                buffPos = (tj*ui32TileHeight+j)*ui32OutputHStride 
                    + (ti*ui32TileWidth);

                IMG_ASSERT(ipixel+ui32TileWidth
                    <=ui32OutputHStride*ui32OutputVStride);
                IMG_ASSERT(buffPos+ui32TileWidth
                    <=ui32OutputHStride*ui32OutputVStride);

                IMG_MEMCPY(&(pOutput[buffPos]), &(pInput[ipixel]), 
                    ui32TileWidth);

                ipixel+=ui32TileWidth;
            }
        }
    }

    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
const char* FormatString_two_params(enum pxlFormat ePxl, IMG_BOOL8 bVU)
{
	switch(ePxl)
	{
	// YUV
	case YUV_420_PL12_8:
		return bVU ? "NV21" : "NV12";
	case YUV_422_PL12_8:
		return bVU ? "NV61" : "NV16";
	case YUV_420_PL12_10:
		return bVU ? "NV21-10bit" : "NV12-10bit";
	case YUV_422_PL12_10:
		return bVU ? "NV61-10bit" : "NV16-10bit";

	// RGB
	case RGB_888_24:
		return "BI_RGB24";
    case BGR_888_24:
        return "BI_BGR24";
	case RGB_888_32:
		return "BI_RGB32";
    case BGR_888_32:
		return "BI_BGR32";
	case RGB_101010_32:
		return "BI_RGB32-10bit";
    case BGR_101010_32:
        return "BI_BGR32-10bit";

    // Bayer
    case BAYER_RGGB_8:
        return "RGGB8"; // should be BA81 but it is a bit obscure
    case BAYER_RGGB_10:
        return "RGGB10";
    case BAYER_RGGB_12:
        return "RGGB12";

    // Bayer TIFF
    case BAYER_TIFF_10:
        return "TIFF10";
    case BAYER_TIFF_12:
        return "TIFF12";

    case PXL_NONE:
        return "NONE";

	default:
		return "?fmt?";
	}
}
#endif //INFOTM_ISP

const char* FormatString(enum pxlFormat ePxl)
{
    switch(ePxl)
    {
    // YUV
    case YVU_420_PL12_8:
        return "NV21";
    case YUV_420_PL12_8:
        return "NV12";
    case YVU_422_PL12_8:
        return "NV61";
    case YUV_422_PL12_8:
        return "NV16";
    case YVU_420_PL12_10:
        return "NV21-10bit";
    case YUV_420_PL12_10:
        return "NV12-10bit";
    case YVU_422_PL12_10:
        return "NV61-10bit";
    case YUV_422_PL12_10:
        return "NV16-10bit";

    // RGB
    case RGB_888_24:
        return "BI_RGB24";
    case BGR_888_24:
        return "BI_BGR24";
    case RGB_888_32:
        return "BI_RGB32";
    case BGR_888_32:
        return "BI_BGR32";
    case RGB_101010_32:
        return "BI_RGB32-10bit";
    case BGR_101010_32:
        return "BI_BGR32-10bit";
    case BGR_161616_64:
        return "BI_BGR64";

    // Bayer
    case BAYER_RGGB_8:
        return "RGGB8"; // should be BA81 but it is a bit obscure
    case BAYER_RGGB_10:
        return "RGGB10";
    case BAYER_RGGB_12:
        return "RGGB12";

    // Bayer TIFF
    case BAYER_TIFF_10:
        return "TIFF10";
    case BAYER_TIFF_12:
        return "TIFF12";

    case PXL_NONE:
        return "NONE";

    default:
        return "?fmt?";
    }
}

const char* MosaicString(enum MOSAICType mos)
{
	switch(mos)
	{
    case MOSAIC_RGGB:
        return "MOSAIC_RGGB";
    case MOSAIC_GRBG:
        return "MOSAIC_GRBG";
	case MOSAIC_GBRG:
        return "MOSAIC_GBRG";
	case MOSAIC_BGGR:
        return "MOSAIC_BGGR";

    case MOSAIC_NONE:
        return "NONE";

	default:
		return "?mos?";
	}
}

eMOSAIC MosaicFlip(eMOSAIC input, IMG_BOOL8 bFlipH, IMG_BOOL8 bFlipV)
{
    // entry 0 no flip, 1 flipH, 2 flipV, 3 flipH+flipV
    const eMOSAIC rggb_flip[4] = 
        { MOSAIC_RGGB, MOSAIC_GRBG, MOSAIC_GBRG, MOSAIC_BGGR};
    const eMOSAIC grbg_flip[4] = 
        { MOSAIC_GRBG, MOSAIC_RGGB, MOSAIC_BGGR, MOSAIC_GBRG};
    const eMOSAIC gbrg_flip[4] = 
        { MOSAIC_GBRG, MOSAIC_BGGR, MOSAIC_RGGB, MOSAIC_GRBG};
    const eMOSAIC bggr_flip[4] = 
        { MOSAIC_BGGR, MOSAIC_GBRG, MOSAIC_GRBG, MOSAIC_RGGB};

    int idx = bFlipH + (bFlipV<<1);
    eMOSAIC res = MOSAIC_NONE;
    
    IMG_ASSERT(bFlipH<=1 && bFlipV<=1);
    IMG_ASSERT(idx<4);

    switch(input)
    {
    case MOSAIC_RGGB:
        res = rggb_flip[idx];
        break;
    case MOSAIC_GRBG:
        res =  grbg_flip[idx];
        break;
    case MOSAIC_GBRG:
        res =  gbrg_flip[idx];
        break;
    case MOSAIC_BGGR:
        res =  bggr_flip[idx];
        break;
    }

    return res;
}
