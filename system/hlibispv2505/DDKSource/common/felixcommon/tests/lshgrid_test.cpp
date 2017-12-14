/**
******************************************************************************
 @file lshgrid_test.cpp

 @brief test LSH functions

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
#include <algorithm>
#include <img_types.h>
#include <img_errors.h>

#include "felixcommon/lshgrid.h"

TEST(LSH, load_bin)
{
    LSH_GRID sLSH;
    double prec = 0.001;

    memset(&sLSH, 0, sizeof(LSH_GRID));

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
	EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, LSH_Load_bin(&(sLSH),
		"testdata/deshading_diag_old.lsh", -1, NULL));
#else
    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, LSH_Load_bin(&(sLSH),
        "testdata/deshading_diag_old.lsh"));
#endif
    ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&(sLSH),
        "testdata/deshading_diag.lsh"));

    EXPECT_EQ(21, sLSH.ui16Width);
    EXPECT_EQ(16, sLSH.ui16Height);
    EXPECT_EQ(16, sLSH.ui16TileSize);
    for (int c = 0; c < 4; c++)
    {
        EXPECT_TRUE(sLSH.apMatrix[c] != NULL) << "channel " << c << " is NULL";
        double base = 1.0*(c + 1);

        for (int y = 0; y < sLSH.ui16Height && !HasFailure(); y++)
        {
            for (int x = 0; x < sLSH.ui16Width && !HasFailure(); x++)
            {
                double expected = std::max(0.0, base - (x)*0.1 - (y)*0.1);
                EXPECT_NEAR(expected, sLSH.apMatrix[c][y*sLSH.ui16Width + x],
                    prec);
            }
        }
    }

    LSH_Free(&(sLSH));
}

/**
 * @brief Tests that saving the LSH grid as binary does not alterate it
 *
 * @note assumes LSH_Load_bin() works
 */
TEST(LSH, save_bin)
{
    int c, y, x;
    // divided by CFA size
    int imgWidth = 640 / 2, imgHeight = 480 / 2;
    LSH_GRID sGrid;

    memset(&sGrid, 0, sizeof(LSH_GRID));

    ASSERT_EQ(IMG_SUCCESS, LSH_CreateMatrix(&sGrid, imgWidth, imgHeight, 16));
    EXPECT_EQ(21, sGrid.ui16Width);
    EXPECT_EQ(16, sGrid.ui16Height);
    EXPECT_EQ(16, sGrid.ui16TileSize);

    for (c = 0; c < 4; c++)
    {
        float base = 1.0f*(c + 1);
        for (y = 0; y < sGrid.ui16Height; y++)
        {
            for (x = 0; x < sGrid.ui16Width; x++)
            {
                sGrid.apMatrix[c][y*sGrid.ui16Width + x] = std::max(0.0f,
                    (float)(base - (x)*0.1 - (y)*0.1));
            }
        }
    }

    EXPECT_EQ(IMG_SUCCESS, LSH_Save_bin(&sGrid, "deshading_out.lsh"));

    if (!HasFailure())
    {
        double prec = 0.001;
        LSH_GRID sCheck;

        memset(&sCheck, 0, sizeof(LSH_GRID));
#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
		ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&(sCheck), "deshading_out.lsh", -1, NULL));
#else
        ASSERT_EQ(IMG_SUCCESS, LSH_Load_bin(&(sCheck), "deshading_out.lsh"));
#endif
        EXPECT_EQ(sGrid.ui16Width, sCheck.ui16Width);
        EXPECT_EQ(sGrid.ui16Height, sCheck.ui16Height);
        EXPECT_EQ(sGrid.ui16TileSize, sCheck.ui16TileSize);

        for (c = 0; c < 4; c++)
        {
            for (y = 0; y < sCheck.ui16Height; y++)
            {
                for (x = 0; x < sCheck.ui16Width && !HasFailure(); x++)
                {
                    EXPECT_NEAR(sGrid.apMatrix[c][y*sGrid.ui16Width + x],
                        sCheck.apMatrix[c][y*sGrid.ui16Width + x], prec)
                        << "c=" << c << " x=" << x << " y=" << y;
                }
            }
        }

        LSH_Free(&sCheck);
    }
    LSH_Free(&sGrid);
}
