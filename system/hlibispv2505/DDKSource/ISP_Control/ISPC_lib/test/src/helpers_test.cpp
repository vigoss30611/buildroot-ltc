/**
*******************************************************************************
 @file helpers_test.cpp

 @brief Test ISPC helper functions

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
#include <ispc/ISPCDefs.h>
#include <limits>
#include <gtest/gtest.h>

#undef VERBOSE

#ifndef VERBOSE
#undef printf
#define printf(...)
#endif /* VERBOSE */


/**
 * @brief test working of portable ISPC::isnan()
 */
// suppress divide by zero warning in WIN32
#pragma warning(push)
#pragma warning(disable : 4723)
TEST(helpers, isnan)
{
    // check double
    {
        typedef std::numeric_limits<double> limits;
        double zero=0.0f;
        const double inf = limits::infinity();
        EXPECT_TRUE(ISPC::isnan(limits::quiet_NaN()));
        EXPECT_FALSE(ISPC::isnan(inf));
        EXPECT_FALSE(ISPC::isnan(zero));
        EXPECT_FALSE(ISPC::isnan(limits::min()/2.0));
        EXPECT_FALSE(ISPC::isnan(limits::max()+1.0f));
        EXPECT_TRUE(ISPC::isnan(zero/zero));
        EXPECT_TRUE(ISPC::isnan(inf/inf));
    }
    // check float
    {
        typedef std::numeric_limits<float> limits;
        float zero=0.0f;
        const float inf = limits::infinity();
        EXPECT_TRUE(ISPC::isnan(limits::quiet_NaN()));
        EXPECT_FALSE(ISPC::isnan(inf));
        EXPECT_FALSE(ISPC::isnan(zero));
        EXPECT_FALSE(ISPC::isnan(limits::min()/2.0));
        EXPECT_FALSE(ISPC::isnan(limits::max()+1.0f));
        EXPECT_TRUE(ISPC::isnan(zero/zero));
        EXPECT_TRUE(ISPC::isnan(inf/inf));
    }
}

/**
 * @brief test working of portable ISPC::isfinite()
 */
TEST(helpers, isfinite)
{
    // check double
    {
        typedef std::numeric_limits<double> limits;
        const double zero=0.0f;
        const double inf = limits::infinity();
        EXPECT_FALSE(ISPC::isfinite(limits::quiet_NaN()));
        EXPECT_FALSE(ISPC::isfinite(inf));
        EXPECT_TRUE(ISPC::isfinite(zero));
        EXPECT_TRUE(ISPC::isfinite(-limits::max()));
        EXPECT_TRUE(ISPC::isfinite(limits::max()));
        EXPECT_FALSE(ISPC::isfinite(zero/zero));
        EXPECT_FALSE(ISPC::isfinite(inf/inf));
    }
    // check float
    {
        typedef std::numeric_limits<float> limits;
        const float zero=0.0f;
        const float inf = limits::infinity();
        EXPECT_FALSE(ISPC::isfinite(limits::quiet_NaN()));
        EXPECT_FALSE(ISPC::isfinite(inf));
        EXPECT_TRUE(ISPC::isfinite(zero));
        EXPECT_TRUE(ISPC::isfinite(-limits::max()));
        EXPECT_TRUE(ISPC::isfinite(limits::max()));
        EXPECT_FALSE(ISPC::isfinite(zero/zero));
        EXPECT_FALSE(ISPC::isfinite(inf/inf));
    }
}
#pragma warning(pop)

/**
 * @brief test working of ISPC::clip()
 */
TEST(helpers, clip)
{
    EXPECT_EQ(2.0f, ISPC::clip(2.0f, 1.0f, 3.0f));
    EXPECT_EQ(2.0f, ISPC::clip(2.0f, 3.0f, 1.0f));
    EXPECT_EQ(1.0f, ISPC::clip(0.0f, 1.0f, 3.0f));
    EXPECT_EQ(3.0f, ISPC::clip(4.0f, 1.0f, 3.0f));
    EXPECT_EQ(1.0f, ISPC::clip(0.0f, 3.0f, 1.0f));
    EXPECT_EQ(3.0f, ISPC::clip(4.0f, 3.0f, 1.0f));

    EXPECT_EQ(2.0f, ISPC::clip(2.0f, -1.0f, 3.0f));
    EXPECT_EQ(2.0f, ISPC::clip(2.0f, 3.0f, -1.0f));
    EXPECT_EQ(-1.0f, ISPC::clip(-2.0f, -1.0f, 3.0f));
    EXPECT_EQ(3.0f, ISPC::clip(4.0f, -1.0f, 3.0f));
    EXPECT_EQ(-1.0f, ISPC::clip(-2.0f, 3.0f, -1.0f));
    EXPECT_EQ(3.0f, ISPC::clip(4.0f, 3.0f, -1.0f));

    EXPECT_EQ(-1.0f, ISPC::clip(2.0f, -1.0f, -3.0f));
    EXPECT_EQ(-1.0f, ISPC::clip(2.0f, -3.0f, -1.0f));
    EXPECT_EQ(-1.0f, ISPC::clip(0.0f, -1.0f, -3.0f));
    EXPECT_EQ(-3.0f, ISPC::clip(-4.0f, -1.0f, -3.0f));
    EXPECT_EQ(-1.0f, ISPC::clip(0.0f, -3.0f, -1.0f));
    EXPECT_EQ(-3.0f, ISPC::clip(-4.0f, -3.0f, -1.0f));
}
