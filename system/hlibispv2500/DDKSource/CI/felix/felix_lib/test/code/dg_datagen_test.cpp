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

#include <tal.h>
#include <registers/ext_data_generator.h>

#include "unit_tests.h"
#include "felix.h"

TEST(DG_Driver, init_deinit)
{
	EXPECT_EQ (IMG_ERROR_UNEXPECTED_STATE, DG_DriverInit()) << "no CI driver";
	EXPECT_EQ (IMG_ERROR_NOT_INITIALISED, DG_DriverFinalise());

	{
		Datagen driver;

		ASSERT_EQ(IMG_SUCCESS, driver.configure());
	}
}

/// @ re-enable malloc test when DG is built as part of the unit tests
TEST(DG_Camera, createDestroy)
{
	Datagen driver;
	DG_CAMERA *pCamera = NULL;
	
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_CameraCreate(NULL));
	EXPECT_EQ (IMG_ERROR_NOT_INITIALISED, DG_CameraCreate(&pCamera)) << "not connected";
	ASSERT_EQ(IMG_SUCCESS, driver.configure());
	
	/*gui32AllocFails = 1;
	{
		EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, DG_CameraCreate(&pCamera)) << "malloc should fail";
	}*/

	EXPECT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera));
	EXPECT_EQ (IMG_ERROR_MEMORY_IN_USE, DG_CameraCreate(&pCamera));

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_CameraDestroy(NULL));
	EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera)); // test without linked list - tests with linked list in other functions

}

/// @ DG_CameraVerif
TEST(DG_Camera, submit_invalid)
{
	DG_CAMERA *pCamera = NULL;
	Datagen driver;
	ASSERT_EQ(IMG_SUCCESS, driver.configure());
	
	EXPECT_EQ(IMG_SUCCESS, DG_CameraCreate(&pCamera));

	pCamera->uiStride = 5;
	pCamera->uiWidth = 5;
	pCamera->uiHeight = 5;
	pCamera->eBufferFormats = (eDG_MEMFMT)42; // format undef

	EXPECT_EQ(IMG_ERROR_FATAL, DG_CameraRegister(pCamera));

	//EXPECT_EQ (IMG_ERROR_FATAL, DG_CameraAddPool(pCamera, 1)) << "unsuported format";
	pCamera->eBufferFormats = DG_MEMFMT_MIPI_RAW10;

	EXPECT_EQ(IMG_SUCCESS, DG_CameraRegister(pCamera));

	// lists are not using the special malloc
	//gui32AllocFails = 1; // first list alloc fails
	//EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, DG_CameraSubmitConfiguration(pCamera, 0));

	//gui32AllocFails = 2; // second list alloc fails
	//EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, DG_CameraSubmitConfiguration(pCamera, 0));
		
	//DG_DriverReleaseCamera(pCamera);

	EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));
}

TEST(DG_Camera, addBuffers)
{
	Datagen driver;
	DG_CAMERA *pCamera = NULL;

	ASSERT_EQ(IMG_SUCCESS, driver.configure());

	ASSERT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera));

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_CameraAddPool(NULL, 1));
	EXPECT_EQ (IMG_ERROR_NOT_INITIALISED, DG_CameraAddPool(pCamera, 1)) << "not registered";

	pCamera->uiStride = 5;
	pCamera->uiWidth = 5;
	pCamera->uiHeight = 5;
	pCamera->eBufferFormats = DG_MEMFMT_MIPI_RAW10;

	EXPECT_EQ(IMG_SUCCESS, DG_CameraRegister(pCamera));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_CameraAddPool(pCamera, 0));

	EXPECT_EQ (IMG_SUCCESS, DG_CameraAddPool(pCamera, 1));

	EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));

	// driver deinit here
}

TEST(DG_Camera, usage)
{
	Datagen driver;
	sSimImageIn PGM;
	DG_CAMERA *pCamera = NULL;
	//char *filename = NULL;

	ASSERT_EQ(IMG_SUCCESS, driver.configure());

	// generate PGM
	char filename[] = "testdata/balloons_64x48_rggb.flx";
	
	EXPECT_EQ (IMG_SUCCESS, SimImageIn_init(&PGM));
	EXPECT_EQ (IMG_SUCCESS, SimImageIn_loadFLX(filename, &PGM, 0));
	// end of PGM generation

	EXPECT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera));
	
	pCamera->eBayerOrder = MOSAIC_RGGB;
	pCamera->eBufferFormats = DG_MEMFMT_MIPI_RAW10;
	pCamera->uiWidth = PGM.info.ui32Width;
	pCamera->uiHeight = PGM.info.ui32Height;
	pCamera->bMIPI_LF = IMG_TRUE;

	EXPECT_EQ(IMG_SUCCESS, DG_CameraRegister(pCamera));

	EXPECT_EQ (IMG_SUCCESS, DG_CameraAddPool(pCamera, 2));
	
	EXPECT_EQ (IMG_SUCCESS, DG_CameraStart(pCamera, driver.getConnection()));

	EXPECT_EQ (IMG_SUCCESS, DG_CameraShoot(pCamera, &PGM, NULL));

	EXPECT_EQ (IMG_SUCCESS, SimImageIn_clean(&PGM));
	
	// fake interrupt here so that DG can be stpped
	KRN_DG_CameraFrameCaptured(g_pDGDriver->aActiveCamera[0]);

	EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));
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
