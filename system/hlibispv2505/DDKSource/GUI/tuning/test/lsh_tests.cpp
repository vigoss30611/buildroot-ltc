/**
******************************************************************************
@file lsh_tests.cpp

@brief Test some of the LSHCompute functions

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
#include "lsh/compute.hpp"

#include "mc/module_config.h"

#include <gtest/gtest.h>
#include <list>
#include <cv.h>

// generated with Tile Size 128 but not needed
static const int gridWidth = 6;
static const int gridHeight = 4;

static const LSH_FLOAT channel0[gridWidth * gridHeight] = {
    1.351323f, 1.255654f, 1.185701f, 1.236205f, 1.280843f, 1.325480f,
    1.245854f, 1.208136f, 1.164084f, 1.217350f, 1.222010f, 1.226669f,
    1.296447f, 1.172635f, 1.236205f, 1.172635f, 1.291203f, 1.409771f,
    1.296447f, 1.172635f, 1.236205f, 1.172635f, 1.291203f, 1.409771f
}; 
static const LSH_FLOAT channel1[gridWidth * gridHeight] = {
    1.335144f, 1.291073f, 1.224911f, 1.302801f, 1.283371f, 1.263940f,
    1.326908f, 1.275760f, 1.257122f, 1.249818f, 1.279554f, 1.309289f,
    1.306758f, 1.197634f, 1.204339f, 1.291073f, 1.242599f, 1.194126f,
    1.306758f, 1.197634f, 1.204339f, 1.291073f, 1.242599f, 1.194126f
};
static const LSH_FLOAT channel2[gridWidth * gridHeight] = {
    1.164190f, 1.125941f, 1.098864f, 1.064725f, 1.187726f, 1.310726f,
    1.135266f, 1.135266f, 1.116768f, 1.135266f, 1.180905f, 1.226543f,
    1.187726f, 1.141568f, 1.093023f, 1.170818f, 1.164190f, 1.157561f,
    1.187726f, 1.141568f, 1.093023f, 1.170818f, 1.164190f, 1.157561f
};
static const LSH_FLOAT channel3[gridWidth * gridHeight] = {
    1.194079f, 1.168455f, 1.173491f, 1.114637f, 1.178571f, 1.242506f,
    1.204646f, 1.226351f, 1.133195f, 1.237500f, 1.101112f, 0.964725f,
    1.210000f, 1.114637f, 1.188865f, 1.143908f, 1.204646f, 1.265384f,
    1.210000f, 1.114637f, 1.188865f, 1.143908f, 1.204646f, 1.265384f
};

struct LSH_test : public LSHCompute, public ::testing::Test
{

    int newLSHGrid(LSH_GRID *grid, int tilesize = 128)
    {
        IMG_RESULT ret;
        const size_t sizeofmat = gridWidth*gridHeight*sizeof(LSH_FLOAT);

        ret = LSH_AllocateMatrix(grid, gridWidth, gridHeight, tilesize);
        if (IMG_SUCCESS != ret)
        {
            return EXIT_FAILURE;
        }

        memcpy(grid->apMatrix[0], channel0, sizeofmat);
        memcpy(grid->apMatrix[1], channel1, sizeofmat);
        memcpy(grid->apMatrix[2], channel2, sizeofmat);
        memcpy(grid->apMatrix[3], channel3, sizeofmat);

        return EXIT_SUCCESS;
    }

    int newLSHMat(cv::Mat *matrices)
    {
        const LSH_FLOAT *channels[4] = { channel0, channel1, channel2, channel3 };

        for (int c = 0; c < 4; c++)
        {
            matrices[c] = cv::Mat(gridHeight, gridWidth, CV_32F);

            for (int i = 0; i < gridWidth*gridHeight; i++)
            {
                matrices[c].at<float>(i) = channels[c][i];
            }
        }
        return EXIT_SUCCESS;
    }
};

TEST_F(LSH_test, precision)
{
    lsh_input input(getDefaultVersion());

    ASSERT_EQ(LSH_VERTEX_INT, input.vertex_int);
    ASSERT_EQ(LSH_VERTEX_FRAC, input.vertex_frac);
    ASSERT_EQ(LSH_VERTEX_SIGNED, input.vertex_signed);

    input = lsh_input(HW_2_X);
    ASSERT_EQ(2, input.vertex_int);
    ASSERT_EQ(10, input.vertex_frac);
    ASSERT_EQ(0, input.vertex_signed);

    input = lsh_input(HW_2_4);
    ASSERT_EQ(2, input.vertex_int);
    ASSERT_EQ(10, input.vertex_frac);
    ASSERT_EQ(0, input.vertex_signed);

    input = lsh_input(HW_2_6);
    ASSERT_EQ(3, input.vertex_int);
    ASSERT_EQ(10, input.vertex_frac);
    ASSERT_EQ(0, input.vertex_signed);

    input = lsh_input(HW_2_7);
    ASSERT_EQ(3, input.vertex_int);
    ASSERT_EQ(10, input.vertex_frac);
    ASSERT_EQ(0, input.vertex_signed);
}

TEST_F(LSH_test, bitsPerDiff)
{
    LSH_GRID grid = LSH_GRID();
    cv::Mat matrices[4];
    lsh_input input(getDefaultVersion());  // precision: 10.2 until HW 2.6
    int mc_maxDiff = 0;
    int cmp_maxDiff = 0;

    newLSHGrid(&grid, input.tileSize);
    newLSHMat(matrices);

    unsigned int mc_bits = MC_LSHComputeMinBitdiff(&grid, &mc_maxDiff);
    LSH_Free(&grid);

    unsigned int cmp_bits = LSHCompute::getBitsPerDiff(&input, matrices,
        LSH_DELTA_BITS_MIN, cmp_maxDiff);

    EXPECT_EQ(mc_bits, cmp_bits);
    EXPECT_EQ(mc_maxDiff, cmp_maxDiff);

    // now we test with other precisions
    if (input.hwVersion < HW_2_6)
    {
        // try with u3.10
        input.vertex_int = 3;
        input.vertex_frac = 10;
        input.vertex_signed = 0;

        cmp_bits = LSHCompute::getBitsPerDiff(&input, matrices,
            LSH_DELTA_BITS_MIN, cmp_maxDiff);
        EXPECT_EQ(9, cmp_bits);
        EXPECT_EQ(139, cmp_maxDiff);
    }
    else
    {
        // try with u2.10
        input.vertex_int = 2;
        input.vertex_frac = 10;
        input.vertex_signed = 0;

        cmp_bits = LSHCompute::getBitsPerDiff(&input, matrices,
            LSH_DELTA_BITS_MIN, cmp_maxDiff);
        EXPECT_EQ(9, cmp_bits);
        EXPECT_EQ(139, cmp_maxDiff);
    }
}
