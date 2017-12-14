/**
******************************************************************************
 @file dg_datagen_test.cpp

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

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <gzip_fileio.h>

//#include <ci_datagen/dg_datagen.h>
#include <dg/dg_api.h>
#include <dg/dg_api_internal.h>
#include <dg_kernel/dg_camera.h>
#include <sim_image.h>

#include <ci_kernel/ci_hwstruct.h>
#include <ci/ci_converter.h>

#include <tal.h>
#include <registers/ext_data_generator.h>

#include "unit_tests.h"
#include "felix.h"

TEST(DG_Driver, init_deinit)
{
    DG_CONNECTION *pDGConnection = NULL;
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, DG_DriverInit(NULL));
    EXPECT_EQ(IMG_ERROR_UNEXPECTED_STATE, DG_DriverInit(&pDGConnection))
        << "no CI driver";
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, DG_DriverFinalise(pDGConnection));

    {
        Datagen driver;

        ASSERT_EQ(IMG_SUCCESS, driver.configure());

        // can even open another connection and close it
        EXPECT_EQ(IMG_SUCCESS, DG_DriverInit(&pDGConnection));
        EXPECT_EQ(IMG_SUCCESS, DG_DriverFinalise(pDGConnection));
    }
}

TEST(DG_Camera, createDestroy)
{
    Datagen driver;
    DG_CAMERA *pCamera = NULL;
    
    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_CameraCreate(NULL, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, DG_CameraCreate(&pCamera, NULL));

    ASSERT_EQ(IMG_SUCCESS, driver.configure());
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, DG_CameraCreate(NULL,
        driver.getDGConnection()));

    EXPECT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera,
        driver.getDGConnection()));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, DG_CameraCreate(&pCamera,
        driver.getDGConnection()));

    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_CameraDestroy(NULL));
    EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));
}

TEST(DG_Camera, submit_invalid)
{
    DG_CAMERA *pCamera = NULL;
    Datagen driver;
    ASSERT_EQ(IMG_SUCCESS, driver.configure());
    
    EXPECT_EQ(IMG_SUCCESS, DG_CameraCreate(&pCamera,
        driver.getDGConnection()));

    pCamera->eFormat = (CI_CONV_FMT)42; // format undef
    pCamera->ui8FormatBitdepth = 10;

    EXPECT_EQ(IMG_ERROR_FATAL, DG_CameraStart(pCamera));

    pCamera->eFormat = CI_DGFMT_MIPI;

    EXPECT_EQ(IMG_SUCCESS, DG_CameraStart(pCamera));

    // should also stop the camera
    EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));
}

TEST(DG_Camera, addBuffers)
{
    Datagen driver;
    DG_CAMERA *pCamera = NULL;

    ASSERT_EQ(IMG_SUCCESS, driver.configure());

    ASSERT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera,
        driver.getDGConnection()));

    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS,
        DG_CameraAllocateFrame(NULL, 0, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS,
        DG_CameraAllocateFrame(pCamera, 0, NULL));
    EXPECT_EQ(IMG_SUCCESS,
        DG_CameraAllocateFrame(pCamera, SYSMEM_ALIGNMENT, NULL));

    EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));

    // driver deinit here
}

TEST(DG_Camera, usage)
{
    Datagen driver;
    sSimImageIn image;
    DG_CAMERA *pCamera = NULL;
    CI_CONVERTER sFrameConverter;
    CI_GASKET sGasket;
    CI_DG_FRAME *pFrame = NULL;

    ASSERT_EQ(IMG_SUCCESS, driver.configure());
    // need to be after configure
    const CI_HWINFO *pHWInfo = &(driver.getConnection()->sHWInfo);

    // example FLX
    char filename[] = "testdata/balloons_64x48_rggb.flx";

    EXPECT_EQ(IMG_SUCCESS, SimImageIn_init(&image));
    EXPECT_EQ(IMG_SUCCESS, SimImageIn_loadFLX(&image, filename));
    EXPECT_EQ(IMG_SUCCESS, SimImageIn_convertFrame(&image, 0));
    // end of image loading

    EXPECT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera,
        driver.getDGConnection()));

    switch (image.info.eColourModel)
    {
    case SimImage_RGGB:
        pCamera->eBayerOrder = MOSAIC_RGGB;
        break;
    case SimImage_GRBG:
        pCamera->eBayerOrder = MOSAIC_GRBG;
        break;
    case SimImage_GBRG:
        pCamera->eBayerOrder = MOSAIC_GBRG;
        break;
    case SimImage_BGGR:
        pCamera->eBayerOrder = MOSAIC_BGGR;
        break;
    }

    ASSERT_LT(pCamera->ui8Gasket, pHWInfo->config_ui8NImagers);

    if ((pHWInfo->gasket_aType[pCamera->ui8Gasket]&CI_GASKET_MIPI) != 0)
    {
        pCamera->eFormat = CI_DGFMT_MIPI;
    }
    else if ((pHWInfo->gasket_aType[pCamera->ui8Gasket]& CI_GASKET_PARALLEL) != 0)
    {
        pCamera->eFormat = CI_DGFMT_PARALLEL;
    }
    else
    {
        ASSERT_TRUE(0) << "gasket information is confusing!";
    }
    /* other format means it's not a valid gasket - but should not occure here
     *
     * bitdepth should match the gasket one to have correct images with
     * parallel but it's not an issue here
     */
    pCamera->ui8FormatBitdepth = image.info.ui8BitDepth;

    EXPECT_EQ(IMG_SUCCESS, CI_ConverterConfigure(&sFrameConverter,
        pCamera->eFormat, pCamera->ui8FormatBitdepth));

    IMG_UINT32 ui32AllocSize = CI_ConverterFrameSize(&sFrameConverter,
        image.info.ui32Width, image.info.ui32Height);
    EXPECT_TRUE(ui32AllocSize > 0);

    EXPECT_EQ(IMG_SUCCESS, DG_CameraAllocateFrame(pCamera, ui32AllocSize,
        NULL));  // we ignore the frame ID

    EXPECT_EQ(IMG_SUCCESS, CI_GasketInit(&sGasket));

    sGasket.bHSync = pCamera->bParallelActive[0];
    sGasket.bVSync = pCamera->bParallelActive[1];
    sGasket.bParallel = pCamera->eFormat == CI_DGFMT_PARALLEL ? IMG_TRUE : IMG_FALSE;
    sGasket.ui8ParallelBitdepth = pCamera->ui8FormatBitdepth;
    sGasket.uiGasket = pCamera->ui8Gasket;
    sGasket.uiWidth = image.info.ui32Width - 1;
    sGasket.uiHeight = image.info.ui32Height - 1;

    EXPECT_EQ(IMG_SUCCESS, CI_GasketAcquire(&sGasket, driver.getConnection()));

    EXPECT_EQ (IMG_SUCCESS, DG_CameraStart(pCamera));

    /* CI should enqueue buffers before the data-generator or frames will
     * be missed */

    pFrame = DG_CameraGetAvailableFrame(pCamera);
    EXPECT_TRUE(pFrame != NULL);

    EXPECT_EQ(IMG_SUCCESS, CI_ConverterConvertFrame(&sFrameConverter,
        &image, pFrame));

    EXPECT_EQ(IMG_SUCCESS, DG_CameraInsertFrame(pFrame));
    // the kernel-side will add an element to HW and wait for interrupt

    // fake interrupt here so that DG can be stopped
    KRN_DG_CameraFrameCaptured(g_pDGDriver->aActiveCamera[0]);

    EXPECT_EQ(IMG_SUCCESS, DG_CameraDestroy(pCamera));
    EXPECT_EQ(IMG_SUCCESS, CI_ConverterClear(&sFrameConverter));
    EXPECT_EQ(IMG_SUCCESS, SimImageIn_close(&image));
    // drivers are deinit here
}

/*
 *
 * KRN_DG_FRAME
 *
 */

/// @ re-enable
TEST(KRN_DG_FRAME, createDestroy)
{
    /*KRN_DG_FRAME *pBuffer = NULL;

    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, KRN_DG_FrameCreate(NULL, NULL));

    gui32AllocFails = 1;
    EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, DG_ShotCreate(&pBuffer));

    EXPECT_EQ (IMG_SUCCESS, DG_ShotCreate(&pBuffer));
    EXPECT_EQ (IMG_ERROR_MEMORY_IN_USE, DG_ShotCreate(&pBuffer));

    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_ShotDestroy(NULL));
    EXPECT_EQ (IMG_SUCCESS, DG_ShotDestroy(pBuffer));*/
}

/// @ re-enable
TEST(DG_Shot, mallocBuffers)
{
    /*Felix driver;
    DG_SHOT *pBuffer = NULL;

    driver.configure();

    EXPECT_EQ (IMG_SUCCESS, DG_ShotCreate(&pBuffer));

    EXPECT_EQ (IMG_ERROR_NOT_INITIALISED, DG_ShotMallocBuffers(pBuffer));

    pBuffer->ui32Stride = SYSMEM_ALIGNMENT-1;
    pBuffer->ui32Height = 16;

    EXPECT_EQ (IMG_ERROR_NOT_SUPPORTED, DG_ShotMallocBuffers(pBuffer));

    pBuffer->ui32Stride = SYSMEM_ALIGNMENT;

    EXPECT_EQ (IMG_SUCCESS, DG_ShotMallocBuffers(pBuffer));

    EXPECT_EQ (IMG_SUCCESS, DG_ShotDestroy(pBuffer));
    */
    // drivers are deinit here
}
