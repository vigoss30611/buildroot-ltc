/**
******************************************************************************
 @file mosaic_test.cpp

 @brief test the Bayer mosaic function

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
#include <felixcommon/pixel_format.h>

TEST(Mosaic, flipNone)
{
    ASSERT_EQ(MOSAIC_NONE, MosaicFlip(MOSAIC_NONE, IMG_FALSE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_NONE, MosaicFlip(MOSAIC_NONE, IMG_TRUE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_NONE, MosaicFlip(MOSAIC_NONE, IMG_FALSE, IMG_TRUE));
    ASSERT_EQ(MOSAIC_NONE, MosaicFlip(MOSAIC_NONE, IMG_TRUE, IMG_TRUE));
}

TEST(Mosaic, flipRGGB)
{
    ASSERT_EQ(MOSAIC_RGGB, MosaicFlip(MOSAIC_RGGB, IMG_FALSE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_GRBG, MosaicFlip(MOSAIC_RGGB, IMG_TRUE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_GBRG, MosaicFlip(MOSAIC_RGGB, IMG_FALSE, IMG_TRUE));
    ASSERT_EQ(MOSAIC_BGGR, MosaicFlip(MOSAIC_RGGB, IMG_TRUE, IMG_TRUE));
}

TEST(Mosaic, flipGRBG)
{
    ASSERT_EQ(MOSAIC_GRBG, MosaicFlip(MOSAIC_GRBG, IMG_FALSE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_RGGB, MosaicFlip(MOSAIC_GRBG, IMG_TRUE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_BGGR, MosaicFlip(MOSAIC_GRBG, IMG_FALSE, IMG_TRUE));
    ASSERT_EQ(MOSAIC_GBRG, MosaicFlip(MOSAIC_GRBG, IMG_TRUE, IMG_TRUE));
}

TEST(Mosaic, flipGBRG)
{
    ASSERT_EQ(MOSAIC_GBRG, MosaicFlip(MOSAIC_GBRG, IMG_FALSE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_BGGR, MosaicFlip(MOSAIC_GBRG, IMG_TRUE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_RGGB, MosaicFlip(MOSAIC_GBRG, IMG_FALSE, IMG_TRUE));
    ASSERT_EQ(MOSAIC_GRBG, MosaicFlip(MOSAIC_GBRG, IMG_TRUE, IMG_TRUE));
}

TEST(Mosaic, flipBGGR)
{
    ASSERT_EQ(MOSAIC_BGGR, MosaicFlip(MOSAIC_BGGR, IMG_FALSE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_GBRG, MosaicFlip(MOSAIC_BGGR, IMG_TRUE, IMG_FALSE));
    ASSERT_EQ(MOSAIC_GRBG, MosaicFlip(MOSAIC_BGGR, IMG_FALSE, IMG_TRUE));
    ASSERT_EQ(MOSAIC_RGGB, MosaicFlip(MOSAIC_BGGR, IMG_TRUE, IMG_TRUE));
}
