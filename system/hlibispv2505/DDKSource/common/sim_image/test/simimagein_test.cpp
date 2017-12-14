/**
******************************************************************************
@file simimagein_test.cpp

@brief test loading of FLX

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

#include <cstdlib>
#include <cstdio>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <sim_image.h>

/**
 * Verify the initialisation and destruction of the object in different cases
 */
TEST(SIMIMAGEIN, create_destroy)
{
    sSimImageIn image;

    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SimImageIn_init(NULL));

    EXPECT_EQ(IMG_SUCCESS, SimImageIn_init(&image));

    EXPECT_EQ(IMG_SUCCESS, SimImageIn_close(&image));
    // can clean twice, no effect
    EXPECT_EQ(IMG_SUCCESS, SimImageIn_close(&image));

    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SimImageIn_close(NULL));

    // allocate buffers
    EXPECT_EQ(IMG_SUCCESS, SimImageIn_init(&image));
    image.info.ui32Width = 5;
    image.info.ui32Height = 5;

    image.pBuffer = (IMG_UINT16*)IMG_MALLOC(
        image.info.ui32Width*image.info.ui32Height*sizeof(IMG_UINT16));

    EXPECT_EQ(IMG_SUCCESS, SimImageIn_close(&image));
}

/**
 * Load a valid RGGB image
 */
TEST(SIMIMAGE, loadFLX)
{
    sSimImageIn image;
    const char filename[] = "testdata/balloons_64x48_rggb.flx";

    ASSERT_EQ(IMG_SUCCESS, SimImageIn_init(&image));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SimImageIn_loadFLX(NULL, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SimImageIn_loadFLX(NULL,
        filename));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SimImageIn_loadFLX(&image, NULL));

    EXPECT_EQ(IMG_ERROR_NOT_INITIALISED, SimImageIn_convertFrame(&image, 0));

    ASSERT_EQ(IMG_SUCCESS, SimImageIn_loadFLX(&image, filename))
        << filename;

    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SimImageIn_convertFrame(NULL, 0));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SimImageIn_convertFrame(&image,
        -1));
    EXPECT_EQ(IMG_SUCCESS, SimImageIn_convertFrame(&image, 0));

    SimImageIn_close(&image);
}

/**
 * Try to load all the frames from a file - content not verified
 */
IMG_RESULT LoadWholeFLX(const char *filename, sSimImageIn *pImage, std::ostream &os = std::cerr)
{
    IMG_RESULT ret;
    unsigned int i;

    ret = SimImageIn_init(pImage);
    if (ret)
    {
        os << "failed to init";
        return ret;
    }

    ret = SimImageIn_loadFLX(pImage, filename);
    if (ret)
    {
        os << "failed to load flx from '" << filename << "'";
        return ret;
    }

    for (i = 0; i < pImage->nFrames; i++)
    {
        ret = SimImageIn_convertFrame(pImage, i);
        if (ret)
        {
            os << "failed to load frame " << i << " from '" << filename << "'";
            if (SimImageIn_close(pImage))
            {
                os << " - and failed to close";
            }
            return ret;
        }
    }
    return IMG_SUCCESS;
}

/**
 * Try to load invalid multi-segment files
 */
TEST(SIMIMAGE, loadMultiSegment_invalid)
{
    IMG_RESULT ret;
    unsigned int nSeg, nFra;
    sSimImageIn image;
    // invalid files
    std::string filenames[4] = {
        "testdata/20x20_8b_diffbayer_2f.flx",
        "testdata/28x28_8b_difffmt_2f.flx",
        "testdata/20x20_diffbit_2f.flx",
        "testdata/diffsize_8b_2f.flx",
    };

    for (int i = 0; i < 4; i++)
    {
        ret = SimImageIn_checkMultiSegment(filenames[i].c_str(),
            &nSeg, &nFra);
        EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, ret)
            << "should not load multi-segment FLX with different bayer mosaic";
        EXPECT_EQ(2, nSeg);
        EXPECT_EQ(2, nFra);

        ret = LoadWholeFLX(filenames[i].c_str(), &image);
        EXPECT_EQ(IMG_SUCCESS, ret) << "should have loaded frist segment!";
        EXPECT_EQ(1, image.nFrames) << "should have load only 1 frame";

        ret = SimImageIn_close(&image);
        EXPECT_EQ(IMG_SUCCESS, ret) << "should have close the image!";
    }
}

/**
 * Try to load FLX video (multi-segment and single-segment)
 */
TEST(SIMIMAGE, loadvideo)
{
    IMG_RESULT ret;
    unsigned int nSeg, nFra;
    sSimImageIn image;
    std::string filenames[2] = {
        "testdata/40x40_8b_rggb_3f.flx",
        "testdata/40x40_8b_rggb_3f_ms.flx"
    };
    unsigned int nSegExpected[2] = {
        1,
        3
    };

    for (int i = 0; i < 2; i++)
    {
        ret = SimImageIn_checkMultiSegment(filenames[i].c_str(),
            &nSeg, &nFra);
        EXPECT_EQ(IMG_SUCCESS, ret)
            << "should support multi-segment when no parameter change";
        EXPECT_EQ(nSegExpected[i], nSeg);
        EXPECT_EQ(3, nFra);

        ret = LoadWholeFLX(filenames[i].c_str(), &image);
        EXPECT_EQ(IMG_SUCCESS, ret) << "should have loaded whole image!";
        EXPECT_EQ(3, image.nFrames);
    }
}
