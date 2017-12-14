/**
******************************************************************************
 @file tiling_test.cpp

 @brief test the de-tiling function

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

#include "felixcommon/pixel_format.h"
#include "felixcommon/ci_alloc_info.h"

struct Detiling: ::testing::Test
{
    IMG_UINT8 *indata;
    IMG_UINT8 *outdata;
    void SetUp()
    {
        indata = NULL;
        outdata = NULL;
    }

    void TearDown()
    {
        if ( indata )
        {
            free(indata);
            indata = NULL;
        }
        if ( outdata )
        {
            free(outdata);
            outdata = NULL;
        }
    }

    /**
     * @brief test untiling yuv file (NV21)
     */
    void testDetileNV12(const char *tiledfile, const char *goldenFile, int inW, int inH, int tW, int tH)
    {
        int totalsize = 0;
        int totalRead = 0;

        struct CI_TILINGINFO sTileInfo;
        struct CI_SIZEINFO sSizeInfo;
        PIXELTYPE sType;

        ASSERT_EQ(IMG_SUCCESS, PixelTransformYUV(&sType, YVU_420_PL12_8));
        ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_GetTileInfo(tW, &sTileInfo));
        ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_YUVSizeInfo(&sType, inW, inH, &sTileInfo, &sSizeInfo));
        
        totalsize = sSizeInfo.ui32Stride*sSizeInfo.ui32Height +
            sSizeInfo.ui32CStride*sSizeInfo.ui32CHeight;
        ASSERT_EQ(sSizeInfo.ui32CStride, sSizeInfo.ui32Stride);
        
        indata = (IMG_UINT8*)calloc(1, totalsize);
        outdata = (IMG_UINT8*)calloc(1, totalsize);

        FILE *f = fopen(tiledfile, "rb");

        ASSERT_TRUE(f != NULL) << "failed to open " << tiledfile;
        totalRead = fread(indata, 1, totalsize, f);
        fclose(f);
        ASSERT_EQ(totalsize, totalRead) << "not enough data read from input file " << tiledfile;

        // detile Y
        EXPECT_EQ(IMG_SUCCESS, BufferDeTile(tW, tH, sSizeInfo.ui32Stride, 
            indata, 
            outdata, 
            sSizeInfo.ui32Stride, sSizeInfo.ui32Height)
            ) << "failed to detile Y from " << tiledfile;

        // detile CbCr
        EXPECT_EQ(IMG_SUCCESS, BufferDeTile(tW, tH, sSizeInfo.ui32CStride, 
            &(indata[sSizeInfo.ui32Stride*sSizeInfo.ui32Height]), 
            &(outdata[sSizeInfo.ui32Stride*sSizeInfo.ui32Height]), 
            sSizeInfo.ui32CStride, sSizeInfo.ui32CHeight)
            ) << "failed to detile CbCr from " << tiledfile;

        std::ostringstream filename;
        filename << ::testing::UnitTest::GetInstance()->current_test_info()->name() 
            << "_" << inW << "x" << inH
            << "_detiled.yuv";
        f = fopen(filename.str().c_str(), "wb");

        ASSERT_TRUE(f != NULL) << "failed to open " << filename.str();
    


        totalRead = 0;
        // write Y
        totalRead += fwrite(outdata, 1, sSizeInfo.ui32Stride*sSizeInfo.ui32Height, f);
        // write CbCr
        totalRead += fwrite(&(outdata[sSizeInfo.ui32Stride*sSizeInfo.ui32Height]), 1, sSizeInfo.ui32CStride*sSizeInfo.ui32CHeight, f);

        fclose(f);
        ASSERT_EQ(totalsize, totalRead) << "not enough data written as detiled output " << filename.str();
    
        if ( goldenFile )
        {
            struct CI_SIZEINFO sGoldenInfo;
            int totalGolden = 0;

            ASSERT_EQ(IMG_SUCCESS, CI_ALLOC_YUVSizeInfo(&sType, inW, inH, NULL, &sGoldenInfo));

            totalGolden = sGoldenInfo.ui32Stride*sGoldenInfo.ui32Height + sGoldenInfo.ui32CStride*sGoldenInfo.ui32CHeight;

            f = fopen(goldenFile, "rb");
            ASSERT_TRUE(f != NULL) << "failed to open " << goldenFile;

            totalRead = fread(indata, 1, totalGolden, f);
            fclose(f);
            ASSERT_EQ(totalGolden, totalRead) << "not enough data read from golden " << goldenFile;

            // compare line by line
            for ( unsigned int l = 0 ; l < sGoldenInfo.ui32Height ; l++ )
            {
                ASSERT_EQ(0, memcmp(
                    &indata[sGoldenInfo.ui32Stride*l], 
                    &outdata[sSizeInfo.ui32Stride*l], 
                    sGoldenInfo.ui32Stride)
                    ) << "untiled image Y different at line " << l << " from original " << filename.str() << " vs golden=" << goldenFile;
            }
            for ( unsigned int l = 0 ; l < sGoldenInfo.ui32CHeight ; l++)
            {                
                ASSERT_EQ(0, memcmp(
                    &indata[sGoldenInfo.ui32Stride*sGoldenInfo.ui32Height + sGoldenInfo.ui32CStride*l], 
                    &outdata[sSizeInfo.ui32Stride*sSizeInfo.ui32Height + sSizeInfo.ui32CStride*l], 
                    sGoldenInfo.ui32CStride)
                    ) << "untiled image CbCr different at line " << l << " from original " << filename.str() << " vs golden=" << goldenFile;
            }
        }

        free(indata);
        indata = NULL;
        free(outdata);
        outdata = NULL;
    }
};

/**
 * @brief Tries invalid calls
 */
TEST_F(Detiling, invalid)
{
    indata = (IMG_UINT8*)calloc(16, 256);
    outdata = (IMG_UINT8*)calloc(16, 256);
    
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(256, 16, 256, NULL, outdata, 256, 16));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(256, 16, 256, indata, NULL, 256, 16));
    
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(0, 16, 256, indata, outdata, 256, 16));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(256, 0, 256, indata, outdata, 256, 16));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(256, 16, 0, indata, outdata, 256, 16));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(256, 16, 256, indata, outdata, 0, 16));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(256, 16, 256, indata, outdata, 256, 0));

    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(256, 16, 512, indata, outdata, 256, 16)) << "tile stride > output stride";
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, BufferDeTile(512, 16, 256, indata, outdata, 256, 16)) << "tile width > tile stride";

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, BufferDeTile(256, 16, 256, indata, outdata, 300, 16)) << "output stride not multiple of tile width";
    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, BufferDeTile(256, 16, 300, indata, outdata, 512, 16)) << "tile stride not a multiple of tile width";
    //EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, BufferDeTile(256, 16, 256, indata, outdata, 256, 16)) << "output stride not a multiple of tile stride";

    // destroying test object frees memory
}

TEST_F(Detiling, scheme_256x16)
{
    testDetileNV12("testdata/tiled-1024x32_(256x16-1024)-NV12.yuv", 
        "testdata/original-1024x32-NV12.yuv",
        1024, 32,
        256, 16);

    testDetileNV12("testdata/tiled-640x480_(256x16-1024)-NV12.yuv", 
        "testdata/original-640x480-NV21.yuv",
        640, 480,
        256, 16);

    // destruction of the test object frees memory
}

TEST_F(Detiling, scheme_512x8)
{
    testDetileNV12("testdata/tiled-1024x32_(512x8-1024)-NV12.yuv",
        "testdata/original-1024x32-NV12.yuv",
        1024, 32,
        512, 8);
        
    // destruction of the test object frees memory
}
