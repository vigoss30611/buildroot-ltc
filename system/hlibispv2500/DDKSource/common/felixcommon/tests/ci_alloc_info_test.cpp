/**
******************************************************************************
 @file ci_alloc_info_test.cpp

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
#include <gtest/gtest.h>

#include <cmath>
#include <img_types.h>
#include <img_errors.h>
#include <algorithm>

#include "felixcommon/ci_alloc_info.h"

static const int nSizes = 6;
// test VGA, PAL, 720p, 1080p, UWUXGA, 4K
static const IMG_UINT32 width[nSizes]  = {640, 768, 1280, 1920, 2560, 4096};
static const IMG_UINT32 height[nSizes] = {480, 576, 720,  1080, 1080, 2160};

/**
 * @brief Ensure that the compiled sizes are the expected ones
 */
TEST(ALLOCINFO, sizes)
{
    ASSERT_EQ(64, CI_ALLOC_GetSysmemAlignment());
    ASSERT_EQ((1<<(9)), CI_ALLOC_GetMinTilingStride());
    ASSERT_EQ((1<<(9+5)), CI_ALLOC_GetMaxTilingStride());
}

/**
 * @brief Tests that the tiling information is correct
 *
 * Supported tiling configuration is defined by IMG MMU HW block
 */
TEST(ALLOCINFO, tileInfo)
{
    struct CI_TILINGINFO res;
    IMG_UINT32 tiled;

    ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_ALLOC_GetTileInfo(0, &res));

    memset(&res, 0, sizeof(struct CI_TILINGINFO));
    tiled = 256;
    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(tiled, &res));

    EXPECT_EQ(256, res.ui32TilingStride) << "tiled format " << tiled;
    EXPECT_EQ(16, res.ui32TilingHeight) << "tiled format " << tiled;
    EXPECT_EQ(512, res.ui32MinTileStride) << "tiled format " << tiled;
    EXPECT_EQ(16384, res.ui32MaxTileStride) << "tiled format " << tiled;

    memset(&res, 0, sizeof(struct CI_TILINGINFO));
    tiled = 512;
    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(tiled, &res));

    EXPECT_EQ(512, res.ui32TilingStride) << "tiled format " << tiled;
    EXPECT_EQ(8, res.ui32TilingHeight) << "tiled format " << tiled;
    EXPECT_EQ(1024, res.ui32MinTileStride) << "tiled format " << tiled;
    EXPECT_EQ(32768, res.ui32MaxTileStride) << "tiled format " << tiled;
}

/**
 * @brief Test that output size of YUV 8b are correct
 */
TEST(ALLOCINFO, YUV8)
{
    struct CI_SIZEINFO sSize;
    PIXELTYPE fmt;
    
    IMG_UINT32 stride;
    IMG_UINT32 ex_height;
    int eF; // ePxlFormat
    int nTested = 0;
    const IMG_UINT32 SYSMEM_ALIGNMENT = CI_ALLOC_GetSysmemAlignment();
    
    for ( eF = PXL_NONE ; eF < PXL_N ; eF++ )
    {
        if ( PixelTransformYUV(&fmt, (ePxlFormat)eF) == IMG_SUCCESS 
            && fmt.ui8BitDepth == 8 )
        {
            for ( int s = 0 ; s < nSizes ; s++ )
            {
                ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_YUVSizeInfo(&fmt, width[s], 
                    height[s], NULL, &sSize));

                stride = width[s];
                if ( width[s]%SYSMEM_ALIGNMENT )
                {
                    stride += SYSMEM_ALIGNMENT - (width[s]%SYSMEM_ALIGNMENT);
                }
                ex_height = height[s]/fmt.ui8VSubsampling;
                if ( height[s]%fmt.ui8VSubsampling )
                {
                    ex_height++;
                }
            
                EXPECT_EQ(eF, sSize.eFmt);
                EXPECT_EQ(0, sSize.ui32TilingStride);
                EXPECT_EQ(stride, sSize.ui32Stride) 
                    << "wrong Y stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(height[s], sSize.ui32Height) 
                    << "wrong Y height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(stride, sSize.ui32CStride) 
                    << "wrong Cb stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(ex_height, sSize.ui32CHeight) 
                    << "wrong Cb height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);

                nTested++;
            }
        }
    }

    ASSERT_GT(nTested, 0) << "no YUV8 format found!";
}

/**
 * @brief Test that output size of YUV 10b are correct
 */
TEST(ALLOCINFO, YUV10)
{
    struct CI_SIZEINFO sSize;
    PIXELTYPE fmt;
    
    IMG_UINT32 stride;
    IMG_UINT32 ex_height;
    int eF; // ePxlFormat
    int nTested = 0;
    const IMG_UINT32 SYSMEM_ALIGNMENT = CI_ALLOC_GetSysmemAlignment();
    
    for ( eF = PXL_NONE ; eF < PXL_N ; eF++ )
    {
        if ( PixelTransformYUV(&fmt, (ePxlFormat)eF) == IMG_SUCCESS 
            && fmt.ui8BitDepth == 10 )
        {
            for ( int s = 0 ; s < nSizes ; s++ )
            {
                ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_YUVSizeInfo(&fmt, width[s], 
                    height[s], NULL, &sSize));

                stride = (width[s]/6)*8; // 6 elements in 8 Bytes
                if ( width[s]%6 )
                {
                    stride += 8;
                }
                if ( stride%SYSMEM_ALIGNMENT )
                {
                    stride += SYSMEM_ALIGNMENT - (stride%SYSMEM_ALIGNMENT);
                }

                ex_height = height[s]/fmt.ui8VSubsampling;
                if ( height[s]%fmt.ui8VSubsampling )
                {
                    ex_height++;
                }
            
                EXPECT_EQ(eF, sSize.eFmt);
                EXPECT_EQ(0, sSize.ui32TilingStride);
                EXPECT_EQ(stride, sSize.ui32Stride) 
                    << "wrong Y stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(height[s], sSize.ui32Height) 
                    << "wrong Y height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(stride, sSize.ui32CStride) 
                    << "wrong Cb stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(ex_height, sSize.ui32CHeight) 
                    << "wrong Cb height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);

                nTested++;
            }
        }
    }

    ASSERT_GT(nTested, 0) << "no YUV10 format found!";
}

/**
 * @brief Test that output size of YUV tiled are correct
 *
 * @note Could be interesting to add another size that makes YUV fail the 
 * maximum tiling stride
 */
TEST(ALLOCINFO, YUV_tiled)
{
    struct CI_SIZEINFO sSize;
    struct CI_TILINGINFO sTiling[2];
    PIXELTYPE fmt;
    
    IMG_UINT32 stride;
    int eF; // ePxlFormat
    int nTested = 0;
    const IMG_UINT32 SYSMEM_ALIGNMENT = CI_ALLOC_GetSysmemAlignment();
    
    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(256, &(sTiling[0])));
    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(512, &(sTiling[1])));

    for ( eF = PXL_NONE ; eF < PXL_N ; eF++ )
    {
        if ( PixelTransformYUV(&fmt, (ePxlFormat)eF) == IMG_SUCCESS )
        {
            for ( int t = 0 ; t < 2 ; t++ )
            {
                for ( int s = 0 ; s < nSizes ; s++ )
                {
                    IMG_RESULT ex_ret = IMG_SUCCESS;
                    ASSERT_TRUE(fmt.ui8BitDepth == 8 || fmt.ui8BitDepth == 10) 
                        << "strange YUV format " 
                        << FormatString((ePxlFormat)eF);

                    if ( fmt.ui8BitDepth == 8 )
                    {
                        stride = width[s];
                    }
                    else if ( fmt.ui8BitDepth == 10 )
                    {
                        stride = (width[s]/6)*8; // 6 elements in 8 Bytes
                        if ( width[s]%6 )
                        {
                            stride += 8;
                        }
                    }
                
                    if ( stride%SYSMEM_ALIGNMENT )
                    {
                        stride += SYSMEM_ALIGNMENT - (stride%SYSMEM_ALIGNMENT);
                    }

                    // compute tiling changes...
                    double stridelog2 = 
                        std::log((double)stride)/std::log( 2.0 );
                    // because win32 c++ does not have log2()...
                    IMG_UINT32 stridepow2 = 
                        1<<( (IMG_UINT32)std::floor(stridelog2) );
                    if ( stridepow2 < stride )
                    {
                        stridepow2 = 1<<((IMG_UINT32)std::floor(stridelog2+1));
                    }
                    IMG_UINT32 ex_height = height[s];
                    IMG_UINT32 ex_heightCb = height[s]/fmt.ui8VSubsampling;
                    if ( ex_heightCb%sTiling[t].ui32TilingHeight )
                    {
                        ex_heightCb += sTiling[t].ui32TilingHeight 
                            - ex_heightCb%sTiling[t].ui32TilingHeight;
                    }

                    stridepow2 = 
                        std::max(stridepow2, sTiling[t].ui32MinTileStride);
                    if ( height[s]%(sTiling[t].ui32TilingHeight) )
                    {
                        ex_height += sTiling[t].ui32TilingHeight 
                            - height[s]%(sTiling[t].ui32TilingHeight);
                    }

                    if ( stridepow2 > sTiling[t].ui32MaxTileStride )
                    {
                        ex_ret = IMG_ERROR_NOT_SUPPORTED;
                    }
                    
                    ASSERT_EQ(ex_ret, CI_ALLOC_YUVSizeInfo(&fmt, width[s], 
                        height[s], &(sTiling[t]), &sSize)) 
                        << "failed tiling for " << width[s] << "x" 
                        << height[s] << " fmt " << 
                        FormatString((ePxlFormat)eF);

                    if ( ex_ret != IMG_SUCCESS )
                    {
                        nTested++;
                        continue;
                        // move to next because that one was supposed to fail
                    }

                    // verify
                    stride = stridepow2;
                                
                    EXPECT_EQ(eF, sSize.eFmt);
                    EXPECT_EQ(sTiling[t].ui32TilingStride, 
                        sSize.ui32TilingStride);
                    EXPECT_EQ(stride, sSize.ui32Stride) 
                        << "wrong Y stride for size " << width[s] << "x" 
                        << height[s] << " fmt " << 
                        FormatString((ePxlFormat)eF);
                    EXPECT_EQ(ex_height, sSize.ui32Height) 
                        << "wrong Y height for size " << width[s] << "x" 
                        << height[s] << " fmt " << 
                        FormatString((ePxlFormat)eF);
                    EXPECT_EQ(stride, sSize.ui32CStride) 
                        << "wrong Cb stride for size " << width[s] << "x" 
                        << height[s] << " fmt " << 
                        FormatString((ePxlFormat)eF);
                    EXPECT_EQ(ex_heightCb, sSize.ui32CHeight) 
                        << "wrong Cb height for size " << width[s] << "x" 
                        << height[s] << " fmt " << 
                        FormatString((ePxlFormat)eF);

                    nTested++;
                }
            }
        }
    }

    ASSERT_GT(nTested, 0) << "no YUV tiled format tested!";
}

/**
 * @brief Test RGB output sizes are correct
 */
TEST(ALLOCINFO, RGB)
{
    struct CI_SIZEINFO sSize;
    PIXELTYPE fmt;
    
    IMG_UINT32 stride;
    int eF; // ePxlFormat
    int nTested = 0;
    const IMG_UINT32 SYSMEM_ALIGNMENT = CI_ALLOC_GetSysmemAlignment();
    
    for ( eF = PXL_NONE ; eF < PXL_N ; eF++ )
    {
        if ( PixelTransformRGB(&fmt, (ePxlFormat)eF) == IMG_SUCCESS )
        {
            for ( int s = 0 ; s < nSizes ; s++ )
            {
                ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_RGBSizeInfo(&fmt, width[s], 
                    height[s], NULL, &sSize));

                EXPECT_TRUE(fmt.ui8PackedStride==3||fmt.ui8PackedStride==4
                    ||fmt.ui8PackedStride==8) 
                    << "RGB format " << FormatString((ePxlFormat)eF) 
                    << " has an unexpected packed size!";

                // 3 elements per pixel in 4 Bytes for 32b based RGB
                // or 3 bytes for 24b based RGB
                // or 8 bytes for 16b in 64b HDR insertion
                stride = width[s]*fmt.ui8PackedStride; 
                if ( stride%SYSMEM_ALIGNMENT )
                {
                    stride += SYSMEM_ALIGNMENT - (stride%SYSMEM_ALIGNMENT);
                }
            
                EXPECT_EQ(eF, sSize.eFmt);
                EXPECT_EQ(0, sSize.ui32TilingStride);
                EXPECT_EQ(stride, sSize.ui32Stride) 
                    << "wrong stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(height[s], sSize.ui32Height) 
                    << "wrong height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(0, sSize.ui32CStride) 
                    << "wrong Cb stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(0, sSize.ui32CHeight) 
                    << "wrong Cb height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);

                nTested++;
            }
        }
    }

    ASSERT_GT(nTested, 0) << "no RGB format found!";
}

/**
 * @brief Test that tiled RGB output sizes are correct
 *
 * @note Some large images are too big to be tiled with our MMU - the stride 
 * would need to be broken into several chunks
 */
TEST(ALLOCINFO, RGB_tiled)
{
    struct CI_SIZEINFO sSize;
    struct CI_TILINGINFO sTiling[2];
    PIXELTYPE fmt;
    
    IMG_UINT32 stride;
    int eF; // ePxlFormat
    int nTested = 0;
    const IMG_UINT32 SYSMEM_ALIGNMENT = CI_ALLOC_GetSysmemAlignment();
    
    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(256, &(sTiling[0])));
    ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(512, &(sTiling[1])));

    for ( eF = PXL_NONE ; eF < PXL_N ; eF++ )
    {
        // BGR_161616_64 is insertion only - cannot be tiled
        if ( PixelTransformRGB(&fmt, (ePxlFormat)eF) == IMG_SUCCESS 
            && eF != BGR_161616_64 )
        {
            for ( int t = 0 ; t < 2 ; t++ )
            {
                for ( int s = 0 ; s < nSizes ; s++ )
                {
                    IMG_RESULT ex_ret = IMG_SUCCESS;
                    // 3 elements per pixel in 4 Bytes for 32b based RGB 
                    // or 3 bytes for 24b based RGB
                    stride = width[s]*fmt.ui8PackedStride; 
                    if ( stride%SYSMEM_ALIGNMENT )
                    {
                        stride += SYSMEM_ALIGNMENT - (stride%SYSMEM_ALIGNMENT);
                    }
            
                    // compute tiling changes...
                    double stridelog2 = 
                        std::log((double)stride)/std::log( 2.0 ); 
                    // because win32 c++ does not have log2()...
                    IMG_UINT32 stridepow2 = 
                        1<<((IMG_UINT32)std::floor(stridelog2));
                    if ( stridepow2 < stride )
                    {
                        stridepow2 = 
                            1<<((IMG_UINT32)std::floor(stridelog2+1));
                    }
                    IMG_UINT32 ex_height = height[s];
                    
                    stridepow2 = 
                        std::max(stridepow2, sTiling[t].ui32MinTileStride);
                    if ( height[s]%(sTiling[t].ui32TilingHeight) )
                    {
                        ex_height += sTiling[t].ui32TilingHeight 
                            - height[s]%(sTiling[t].ui32TilingHeight);
                    }

                    if ( stridepow2 > sTiling[t].ui32MaxTileStride )
                    {
                        ex_ret = IMG_ERROR_NOT_SUPPORTED;
                    }

                    ASSERT_EQ(ex_ret, CI_ALLOC_RGBSizeInfo(&fmt, width[s], 
                        height[s], &(sTiling[t]), &sSize)) 
                        << "failed tiling for " << width[s] << "x" 
                        << height[s] << " fmt " << FormatString((ePxlFormat)eF)
                        << " tiling " << sTiling[t].ui32TilingStride << "x" 
                        << sTiling[t].ui32TilingHeight;

                    if ( ex_ret != IMG_SUCCESS )
                    {
                        std::cout << "INFO: tiling " 
                            << sTiling[t].ui32TilingStride << "x" 
                            << sTiling[t].ui32TilingHeight << " for " 
                            << width[s] << "x" << height[s] << " fmt " 
                            << FormatString((ePxlFormat)eF) 
                            << " is bigger than maximum suppoted" << std::endl;
                        nTested++;
                        continue;
                        // move to next because that one was supposed to fail
                    }

                    EXPECT_TRUE(fmt.ui8PackedStride==3||fmt.ui8PackedStride==4)
                        << "RGB format " << FormatString((ePxlFormat)eF) 
                        << " has a strange packed size!";
                                        
                    // verify
                    stride = stridepow2;
                                
                    EXPECT_EQ(eF, sSize.eFmt);
                    EXPECT_EQ(sTiling[t].ui32TilingStride, 
                        sSize.ui32TilingStride);
                    EXPECT_EQ(stride, sSize.ui32Stride) 
                        << "wrong stride for size " << width[s] << "x" 
                        << height[s] << " fmt " 
                        << FormatString((ePxlFormat)eF);
                    EXPECT_EQ(ex_height, sSize.ui32Height) 
                        << "wrong height for size " << width[s] << "x" 
                        << height[s] << " fmt " 
                        << FormatString((ePxlFormat)eF);
                    EXPECT_EQ(0, sSize.ui32CStride) 
                        << "wrong Cb stride for size " << width[s] << "x" 
                        << height[s] << " fmt " 
                        << FormatString((ePxlFormat)eF);
                    EXPECT_EQ(0, sSize.ui32CHeight) 
                        << "wrong Cb height for size " << width[s] << "x" 
                        << height[s] << " fmt " 
                        << FormatString((ePxlFormat)eF);

                    nTested++;
                }
            }
        }
    }

    ASSERT_GT(nTested, 0) << "no RGB tiled format tested!";
}

TEST(ALLOCINFO, Bayer)
{
    struct CI_SIZEINFO sSize;
    PIXELTYPE fmt;
    
    IMG_UINT32 stride;
    int eF; // ePxlFormat
    int nTested = 0;
    const IMG_UINT32 SYSMEM_ALIGNMENT = CI_ALLOC_GetSysmemAlignment();

    for ( eF = PXL_NONE ; eF < PXL_N ; eF++ )
    {
        IMG_RESULT ret = 
            PixelTransformBayer(&fmt, (ePxlFormat)eF, MOSAIC_RGGB);
        if ( ret == IMG_SUCCESS && fmt.ePackedStart != PACKED_MSB )
        {
            for ( int s = 0 ; s < nSizes ; s++ )
            {
                ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_RGBSizeInfo(&fmt, width[s], 
                    height[s], NULL, &sSize));

                ASSERT_TRUE(fmt.ui8BitDepth == 8 || fmt.ui8BitDepth == 10 
                    || fmt.ui8BitDepth == 12);

                if ( fmt.ui8BitDepth == 8 )
                {
                    stride = width[s]; // 1B per element
                }
                else if ( fmt.ui8BitDepth == 10 )
                {
                    stride = width[s]/3*4; // 4B for 3 elements
                    if ( width[s]%3 )
                    {
                        stride += 4;
                    }
                }
                else if ( fmt.ui8BitDepth == 12 )
                {
                    stride = width[s]/16*24; // 24B for 16 elements
                    if ( width[s]%16 )
                    {
                        stride += 24;
                    }
                }

                if ( stride%SYSMEM_ALIGNMENT )
                {
                    stride += SYSMEM_ALIGNMENT - stride%SYSMEM_ALIGNMENT;
                }
            
                EXPECT_EQ(eF, sSize.eFmt);
                EXPECT_EQ(0, sSize.ui32TilingStride);
                EXPECT_EQ(stride, sSize.ui32Stride) 
                    << "wrong Y stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(height[s], sSize.ui32Height) 
                    << "wrong Y height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(0, sSize.ui32CStride) 
                    << "wrong Cb stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(0, sSize.ui32CHeight) 
                    << "wrong Cb height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);

                nTested++;
            }
        }
    }

    ASSERT_GT(nTested, 0) << "no Bayer format found!";
}

/**
 * @brief Test that TIFF formats have expected output size
 */
TEST(ALLOCINFO, Raw2D)
{
    struct CI_SIZEINFO sSize;
    PIXELTYPE fmt;
    
    IMG_UINT32 stride;
    int eF; // ePxlFormat
    int nTested = 0;

    for ( eF = PXL_NONE ; eF < PXL_N ; eF++ )
    {
        IMG_RESULT ret = 
            PixelTransformBayer(&fmt, (ePxlFormat)eF, MOSAIC_RGGB);
        if ( ret == IMG_SUCCESS && fmt.ePackedStart == PACKED_MSB )
        {
            for ( int s = 0 ; s < nSizes ; s++ )
            {
                ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_Raw2DSizeInfo(&fmt, width[s], 
                    height[s], NULL, &sSize));

                ASSERT_TRUE(fmt.ui8BitDepth == 10 || fmt.ui8BitDepth == 12) 
                    << "unknown TIFF format " << FormatString((ePxlFormat)eF);

                if ( fmt.ui8BitDepth == 10 )
                {
                    stride = width[s]/4*5;
                    if ( width[s]%4 )
                    {
                        stride+=5;
                    }
                    // no system alignment for TIFF formats!
                }
                else if ( fmt.ui8BitDepth == 12 )
                {
                    stride = width[s]/2*3;
                    if ( width[s]%2 )
                    {
                        stride+=3;
                    }
                    // no system alignment for TIFF formats!
                }
            
                EXPECT_EQ(eF, sSize.eFmt);
                EXPECT_EQ(0, sSize.ui32TilingStride);
                EXPECT_EQ(stride, sSize.ui32Stride) 
                    << "wrong stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(height[s], sSize.ui32Height) 
                    << "wrong height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(0, sSize.ui32CStride) 
                    << "wrong Cb stride for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);
                EXPECT_EQ(0, sSize.ui32CHeight) 
                    << "wrong Cb height for size " << width[s] << "x" 
                    << height[s] << " fmt " << FormatString((ePxlFormat)eF);

                nTested++;
            }
        }
    }

    ASSERT_GT(nTested, 0) << "no Raw2D format found!";
}

/**
 * @brief Ensure that TIFF formats cannot be configured as tiled
 */
TEST(ALLOCINFO, Raw2D_invalid)
{
    struct CI_SIZEINFO sSize;
    struct CI_TILINGINFO sTiling;
    PIXELTYPE fmt;
    
    ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_ALLOC_Raw2DSizeInfo(&fmt, width[0], 
        height[0], &sTiling, &sSize));
}

/**
 * @brief Ensure that HDF cannot be configured as tiled or with other input 
 * formats
 */
TEST(ALLOCINFO, HDF_invalid)
{
    struct CI_SIZEINFO sSize;
    struct CI_TILINGINFO sTiling;
    PIXELTYPE fmt;

    PixelTransformRGB(&fmt, BGR_161616_64);

    ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_ALLOC_RGBSizeInfo(&fmt, width[0], 
        height[0], &sTiling, &sSize));
}
