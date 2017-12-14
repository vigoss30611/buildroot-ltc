/**
******************************************************************************
@file ci_intdg_test.cpp

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

#include <cstdlib>
#include <cstdio>

#include "unit_tests.h"
#include "felix.h"

#include "ci/ci_api.h"
#include <sim_image.h>


class DGConverter: public ::testing::Test
{
public:
    CI_DG_FRAME sFrame;
    void *cmpData;

    void SetUp()
    {
        memset(&sFrame, 0, sizeof(CI_DG_FRAME));
        cmpData = 0;
    }

    void TearDown()
    {
        if ( sFrame.data )
        {
            free(sFrame.data);
            sFrame.data = 0;
        }
        if ( cmpData )
        {
            free(cmpData);
            cmpData = 0;
        }
    }

    void runCheckParallel(int bitdepth)
    {
        CI_DG_CONVERTER sConverter;
        
        sSimImageIn flx;
        char filename[64]; 
        char filenameCheck[64];
        unsigned expectedsize = 0;
        
        sprintf(filename, "testdata/44x40_%dbit.flx", bitdepth);
        sprintf(filenameCheck, "testdata/44x40_para%d.dat", bitdepth);

        switch(bitdepth)
        {
        case 10:
            ASSERT_EQ(IMG_SUCCESS, CI_ConverterConfigure(&sConverter, CI_DGFMT_PARALLEL_10)) << "could not configure the converter";
            expectedsize = 2560;
            break;
        case 12:
            ASSERT_EQ(IMG_SUCCESS, CI_ConverterConfigure(&sConverter, CI_DGFMT_PARALLEL_12)) << "could not configure the converter";
            expectedsize = 5120;
            break;
        default:
            ASSERT_TRUE(0) << "wrong format bitdepth given!";
        }
        

        ASSERT_EQ (IMG_SUCCESS, SimImageIn_init(&flx));
        ASSERT_EQ (IMG_SUCCESS, SimImageIn_loadFLX(filename, &flx, 0)) << "failed to open " << filename;

        EXPECT_EQ(44, flx.info.ui32Width);
        EXPECT_EQ(40, flx.info.ui32Height);
        EXPECT_EQ(bitdepth, flx.info.ui8BitDepth);

        FILE *f = fopen(filenameCheck, "rb");
        ASSERT_TRUE(f != NULL) << "failed to open checkfile " << filenameCheck;

        // note: for a real convertion the frame should be acquired from a Datagen object
        // but we can also do it this way just to convert a frame

        this->sFrame.ui32AllocSize = CI_ConverterFrameSize(&sConverter, flx.info.ui32Width, flx.info.ui32Height);
        ASSERT_EQ(expectedsize, sFrame.ui32AllocSize);
        
        sFrame.data = malloc(sFrame.ui32AllocSize); // use malloc because CI_ConvertFrame should memset to 0!

        ASSERT_EQ(IMG_SUCCESS, CI_ConverterConvertFrame(&sConverter, &flx, &sFrame)) << "failed to convert the frame!";

        this->cmpData = malloc(sFrame.ui32AllocSize); // use malloc because we should load everything from disk!

        ASSERT_EQ(sFrame.ui32AllocSize, fread(cmpData, 1, sFrame.ui32AllocSize, f)) << "compare " << filenameCheck << " data could not load correctly!";

        EXPECT_EQ(0, memcmp(cmpData, sFrame.data, sFrame.ui32AllocSize)) << "convertion of parallel " << bitdepth << "b is not what is expected!";

        free(cmpData);
        cmpData = 0;

        free(sFrame.data);
        sFrame.data = 0;

        SimImageIn_clean(&flx);
        fclose(f);
    }
};

TEST_F(DGConverter, parallel10)
{
    runCheckParallel(10);
}

TEST_F(DGConverter, parallel12)
{
    runCheckParallel(12);
}
