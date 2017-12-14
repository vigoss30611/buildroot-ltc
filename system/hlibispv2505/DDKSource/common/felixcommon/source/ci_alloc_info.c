/**
******************************************************************************
 @file ci_alloc_info.c

 @brief Implementation of the allocation size computation

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
#include "felixcommon/ci_alloc_info.h"

#include <img_defs.h>
#include <img_errors.h>

#ifndef IMG_KERNEL_MODULE

#define LOG_TAG "DPF_OUT"
#include <felixcommon/userlog.h>

#else

#define LOG_ERROR(...) printk(KERN_ERR __VA_ARGS__)
#define LOG_WARNING(...) printk(KERN_WARNING __VA_ARGS__)
#define LOG_INFO(...) printk(KERN_INFO __VA_ARGS__)
#ifndef NDEBUG
#define LOG_DEBUG(...) printk(KERN_WARNING __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#endif

/**
 * @see CI_ALLOC_GetMinTilingStride()
 */
static const IMG_UINT32 CI_ALLOC_MIN_TILING_STR = (1<<(9));

IMG_UINT32 CI_ALLOC_GetMinTilingStride(void)
{
    return CI_ALLOC_MIN_TILING_STR;
}

/**
 * @see CI_ALLOC_GetMaxTilingStride()
 *
 * Register in MMU is a 3b logarithm value that gives:
 * tiled stride = 2^(9+register+scheme)
 * with scheme 0 for 256 and 1 for 512.
 *
 * A maximum tiling stride of 16kB should allows a 32b RGB output at a
 * resolution of 4k (256 scheme) or 8k (512 scheme).
 *
 * @note The first virtual address has to be aligned to the start of a tile
 * which varies on the used tiled scheme.
 * HW supports up to 256kB stride (256 scheme) or 512kB stride (512 scheme)
 * but alignment becomes troublesome (8k needs 1st address alignment to
 * 0x20000B, 16k to 0x40000B, etc up to 0x400000B - 4MB - for 256kB stride).
 *
 * @warning Remember to update MMU_MAX_TILING_STR in ci_kernel.h if this value
 * is changed.
 */
static const IMG_UINT32 CI_ALLOC_MAX_TILING_STR = (1<<(9+5));

IMG_UINT32 CI_ALLOC_GetMaxTilingStride(void)
{
    return CI_ALLOC_MAX_TILING_STR;
}

/**
 * @see CI_ALLOC_GetSysmemAlignment()
 */
static const IMG_UINT32 CI_ALLOC_SYSMEM_ALIGNMENT = 64;

IMG_UINT32 CI_ALLOC_GetSysmemAlignment(void)
{
    return CI_ALLOC_SYSMEM_ALIGNMENT;
}

IMG_RESULT CI_ALLOC_GetTileInfo(IMG_UINT32 ui32TiledScheme,
    struct CI_TILINGINFO *pResult)
{
    IMG_RESULT ret = IMG_ERROR_NOT_SUPPORTED;
    IMG_ASSERT(pResult != NULL);

    pResult->ui32TilingStride = ui32TiledScheme;
    pResult->ui32MinTileStride = CI_ALLOC_MIN_TILING_STR;
    pResult->ui32MaxTileStride = CI_ALLOC_MAX_TILING_STR;

    if ( ui32TiledScheme == 256 )
    {
        pResult->ui32TilingHeight = 16; // 256Bx16
        ret = IMG_SUCCESS;
    }
    else if ( ui32TiledScheme == 512 )
    {
        pResult->ui32TilingHeight = 8; // 512Bx8
        pResult->ui32MinTileStride <<= 1;
        pResult->ui32MaxTileStride <<= 1;
        ret = IMG_SUCCESS;
    }
    // we could compute the heigth by being height=4096/ui32TiledScheme but
    // IMG MMU would not suport it

    return ret;
}

/*
 * @brief determines whether pixel format is interleaved YUV 444
 * @note Applicable only to HW 2.x
 */
#define isPackedYcc(pType) \
    ( pType->ui8HSubsampling==1 && \
      pType->ui8VSubsampling==1 && \
      pType->ui8PackedStride>=3 )

IMG_RESULT CI_ALLOC_YUVSizeInfo(const PIXELTYPE *pType, IMG_UINT32 ui32Width,
    IMG_UINT32 ui32Height, const struct CI_TILINGINFO *pTileInfo,
    struct CI_SIZEINFO *pResult)
{
    IMG_UINT32 bop = 0;
    IMG_ASSERT(pType != NULL);
    IMG_ASSERT(pResult != NULL);

    bop = ui32Width/pType->ui8PackedElements;
    if ( ui32Width%pType->ui8PackedElements != 0 )
    {
        bop++;
    }

    pResult->eFmt = pType->eFmt;
    pResult->ui32Stride = bop*pType->ui8PackedStride;
    pResult->ui32Height = ui32Height;///pType->sAccess.ui8VSubsampling;

    // if smthg to allocate align stride to system alignment
    if ( pResult->ui32Stride != 0 )
    {
        /// @warning CI_ALLOC_SYSMEM_ALIGNMENT should be a multiple of 2
        pResult->ui32Stride =
            ((pResult->ui32Stride + CI_ALLOC_SYSMEM_ALIGNMENT-1)
                /CI_ALLOC_SYSMEM_ALIGNMENT) *CI_ALLOC_SYSMEM_ALIGNMENT;

        if(!isPackedYcc(pType))
        {
            // use subsampling to know how many lines to add
            pResult->ui32CStride = 2*ui32Width/pType->ui8HSubsampling;

            // apply packing
            bop = (pResult->ui32CStride)/pType->ui8PackedElements;
            if ( (pResult->ui32CStride)%pType->ui8PackedElements != 0 )
            {
                bop++;
            }

            pResult->ui32CStride = bop*pType->ui8PackedStride;
            pResult->ui32CHeight = ui32Height/pType->ui8VSubsampling;

            pResult->ui32CStride =
                ((pResult->ui32CStride + CI_ALLOC_SYSMEM_ALIGNMENT-1)
                    /CI_ALLOC_SYSMEM_ALIGNMENT) *CI_ALLOC_SYSMEM_ALIGNMENT;
        } else {
            // support for YUV_444_XXX
            pResult->ui32CStride = 0; // second buffer not used
            pResult->ui32CHeight = 0;
        }
        pResult->ui32TilingStride = 0;
        if ( pTileInfo )
        {
            IMG_UINT32 StrLog2 = 0; // log2()
            IMG_UINT32 maxLCbStr =
                IMG_MAX_INT(pResult->ui32Stride, pResult->ui32CStride);
            IMG_UINT32 tiledStride = maxLCbStr; // use same stride for luma and chroma
            while ( tiledStride >>= 1 )
            {
                StrLog2++;
            }
            tiledStride = 1<<StrLog2;
            if ( tiledStride < maxLCbStr )
            {
                tiledStride = 1<<(StrLog2+1); // +1 for next value
            }

            tiledStride = IMG_MAX_INT(tiledStride, pTileInfo->ui32MinTileStride);
            IMG_ASSERT(tiledStride >= pResult->ui32Stride);
            IMG_ASSERT(tiledStride >= pResult->ui32CStride);

            if ( tiledStride > pTileInfo->ui32MaxTileStride )
            {
                LOG_ERROR("Trying to use tiling stride of %d (encoder) "\
                    "- maximum supported %d\n", tiledStride,
                    pTileInfo->ui32MaxTileStride);
                return IMG_ERROR_NOT_SUPPORTED;
            }

            pResult->ui32Stride = tiledStride;

            {
                // round up the heights to cope with the tile height
                IMG_UINT32 height = pResult->ui32Height%(pTileInfo->ui32TilingHeight);
                if ( height != 0 )
                {
                    pResult->ui32Height += pTileInfo->ui32TilingHeight - height;
                }
            }
            pResult->ui32TilingStride = pTileInfo->ui32TilingStride;
            if(!isPackedYcc(pType))
            {
                // YUV444i (packed) uses only one plane
                IMG_UINT32 cHeight = pResult->ui32CHeight%(pTileInfo->ui32TilingHeight);
                if ( cHeight != 0 )
                {
                    pResult->ui32CHeight += pTileInfo->ui32TilingHeight - cHeight;
                }
                pResult->ui32CStride = tiledStride;
            }
        }

    }
    return IMG_SUCCESS;
}

#undef isPackedYcc

IMG_RESULT CI_ALLOC_RGBSizeInfo(const PIXELTYPE *pType, IMG_UINT32 ui32Width,
    IMG_UINT32 ui32Height, const struct CI_TILINGINFO *pTileInfo,
    struct CI_SIZEINFO *pResult)
{
    IMG_UINT32 bop = 0;
    IMG_ASSERT(pType != NULL);
    IMG_ASSERT(pResult != NULL);

    if (BGR_161616_64 == pType->eFmt && pTileInfo)
    {
        LOG_ERROR("Cannot insert tiled HDF buffer!\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    bop = ui32Width/pType->ui8PackedElements;
    if ( ui32Width%pType->ui8PackedElements != 0 )
    {
        bop++;
    }

    pResult->eFmt = pType->eFmt;
    pResult->ui32Stride = bop*pType->ui8PackedStride;
    pResult->ui32Height = ui32Height; // for even and odd rows
    pResult->ui32CStride = 0; // second buffer not used
    pResult->ui32CHeight = 0;

    // if smthg to allocate align stride to system alignment
    if ( pResult->ui32Stride != 0 )
    {

        pResult->ui32Stride =
            ((pResult->ui32Stride+ CI_ALLOC_SYSMEM_ALIGNMENT-1)
                /CI_ALLOC_SYSMEM_ALIGNMENT) *CI_ALLOC_SYSMEM_ALIGNMENT;

        pResult->ui32TilingStride = 0;
        if ( pTileInfo )
        {
            IMG_UINT32 StrLog2 = 0; // log2()
            IMG_UINT32 tmp = pResult->ui32Stride;

            while ( tmp >>= 1 )
            {
                StrLog2++;
            }
            tmp = 1<<StrLog2;
            if ( tmp < pResult->ui32Stride )
            {
                tmp = 1<<(StrLog2+1); // log2()+1 for next power of 2
            }
            tmp = IMG_MAX_INT(tmp, pTileInfo->ui32MinTileStride);
            IMG_ASSERT(tmp >= pResult->ui32Stride);

            if ( tmp > pTileInfo->ui32MaxTileStride )
            {
                LOG_ERROR("Trying to use tiling stride of %d (display) "\
                    "- maximum supported %d\n", tmp,
                    pTileInfo->ui32MaxTileStride);
                return IMG_ERROR_NOT_SUPPORTED;
            }

            pResult->ui32Stride = tmp;

            // now align the height to the tile height

            tmp = pResult->ui32Height%(pTileInfo->ui32TilingHeight);
            if ( tmp != 0 )
            {
                pResult->ui32Height += pTileInfo->ui32TilingHeight - tmp;
            }

            pResult->ui32TilingStride = pTileInfo->ui32TilingStride;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_ALLOC_Raw2DSizeInfo(const PIXELTYPE *pType, IMG_UINT32 ui32Width,
    IMG_UINT32 ui32Height, const struct CI_TILINGINFO *pTileInfo,
    struct CI_SIZEINFO *pResult)
{
    IMG_UINT32 bop = 0;
    IMG_ASSERT(pType != NULL);

    if ( pTileInfo )
    {
        LOG_ERROR("Cannot configure tiled RAW2D format in current HW\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    bop = (ui32Width)/pType->ui8PackedElements;

    // uses size from the imager because Raw2D is done before scaling
    if ( ui32Width%pType->ui8PackedElements != 0 )
    {
        bop++;
    }

    pResult->eFmt = pType->eFmt;
    pResult->ui32Stride = bop*pType->ui8PackedStride;
    pResult->ui32Height = ui32Height; // for even and odd rows
    pResult->ui32CStride = 0; // not used
    pResult->ui32CHeight = 0;
    pResult->ui32TilingStride = 0;


    // RAW 2D output format is TIFF which is byte aligned and therefore does
    // not need system alignment
    //if ( pResult->ui32Stride != 0 )
    //{
    //  pResult->ui32Stride =
    //        ((pResult->ui32Stride + CI_ALLOC_SYSMEM_ALIGNMENT-1)
    //            /CI_ALLOC_SYSMEM_ALIGNMENT) *CI_ALLOC_SYSMEM_ALIGNMENT;
    //}

    return IMG_SUCCESS;
}
