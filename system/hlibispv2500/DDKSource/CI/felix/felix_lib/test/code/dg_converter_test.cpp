/**
******************************************************************************
 @file dg_converter_test.cpp

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

#include "unit_tests.h"
#include "felix.h"

#include <dg/dg_api.h>
#include <dg/dg_api_internal.h>
#include <sim_image.h>

/**
 *
 * From CRC examples MIPI/CSI-2.pdf
 */
TEST(MIPI, crc)
{
	// data and crcs from  p58
	IMG_UINT8 data1[24]={ 0xFF, 0x00, 0x00, 0x02, 0xB9, 0xDC, 0xF3, 0x72, 0xBB, 0xD4, 0xB8, 0x5A, 0xC8, 0x75, 0xC2, 0x7C, 0x81, 0xF8, 0x05, 0xDF, 0xFF, 0x00, 0x00, 0x01 };
	IMG_UINT8 data2[24]={ 0xFF, 0x00, 0x00, 0x00, 0x1E, 0xF0, 0x1E, 0xC7, 0x4F, 0x82, 0x78, 0xC5, 0x82, 0xE0, 0x8C, 0x70, 0xD2, 0x3C, 0x78, 0xE9, 0xFF, 0x00, 0x00, 0x01 };
	IMG_UINT16 crc1 = 0x00F0, crc2 = 0xE569;

	EXPECT_EQ (0xFFFF, DG_ConvMIPIGetDataCRC(NULL, 0)) << "MIPI CRC of 0 bytes must be 0xFFFF CSI-2 p56";

	EXPECT_EQ(crc1, DG_ConvMIPIGetDataCRC(&data1[0], 24));
	EXPECT_EQ(crc2, DG_ConvMIPIGetDataCRC(&data2[0], 24));
}

TEST(MIPI, header)
{
	IMG_UINT32 header;

	ASSERT_EQ (4, DG_ConvMIPIWriteHeader(NULL, 0, 0, 0)) << "MIPI data packet size is not correct";
	// table 3 p50
	ASSERT_EQ (0x2b, DG_MIPI_ID_RAW10);
	ASSERT_EQ (0x2c, DG_MIPI_ID_RAW12);
	ASSERT_EQ (0x2d, DG_MIPI_ID_RAW14);

	DG_ConvMIPIWriteHeader((IMG_UINT8*)&header, 1, DG_MIPI_ID_RAW10, 42);
	// dataid = (2-bit virtual channel)<<6 | (data type (0x10..0x37)) 8b
	// 1<<6 + DG_MIPI_ID_RAW10
	EXPECT_EQ (1<<6 | DG_MIPI_ID_RAW10, ((IMG_UINT8*)&header)[0]);
	// nBytes =  (16b) in the middle
	// header >>8 & 0x00FF
	EXPECT_EQ (42, header>>8 & 0x00FF) << "Nbytes is incorrect in the header";
	// ECC table 5 p52-53
	EXPECT_EQ (0x0B, header & 0x000f) << "ECC is not correct";
}

TEST(MIPI, RAW10)
{
	IMG_UINT16 input[4] = { 0x123, 0x167, 0x1AB, 0x1EF };
	IMG_UINT8 verif[5] = { 0x48, 0x59, 0x6A, 0x7B, 0xFF }, output[5];

	IMG_MEMSET(output, 0, 5);
	ASSERT_EQ (5, DG_ConvBayerToMIPI_RAW10(&input[0], &input[1], 2, 2, 2, output));

	EXPECT_EQ (0, IMG_MEMCMP(output, verif, 4)) << "main part is different";
	EXPECT_EQ (verif[4], output[4]) << "mixed byte is different";
}

TEST(MIPI, RAW12)
{
	IMG_UINT16 input[4] = { 0x123, 0x567, 0x9AB, 0xDEF };
	IMG_UINT8 verif[6] = { 0x12, 0x56, 0x73, 0x9A, 0xDE, 0xFB }, output[6];

	IMG_MEMSET(output, 0, 6);
	ASSERT_EQ (6, DG_ConvBayerToMIPI_RAW12(&input[0], &input[1], 2, 2, 2, output));

	EXPECT_EQ (0, IMG_MEMCMP(&output[0], &verif[0], 2)) << "1st main part is different";
	EXPECT_EQ (verif[2], output[2]) << "1st mixed byte is different";
	EXPECT_EQ (0, IMG_MEMCMP(&output[3], &verif[3], 2)) << "2nd main part is different";
	EXPECT_EQ (verif[5], output[5]) << "2nd mixed byte is different";
}

TEST(MIPI, RAW14)
{
	IMG_UINT16 input[4] = { 0x123, 0x567, 0x9AB, 0xDEF };
	IMG_UINT8 verif[7] = { 0x04, 0x15, 0x26, 0x37, 0xE3, 0xB9, 0xBE }, output[7];

	IMG_MEMSET(output, 0, 7);
	ASSERT_EQ (7, DG_ConvBayerToMIPI_RAW14(&input[0], &input[1], 2, 2, 2, output));

	EXPECT_EQ (0, IMG_MEMCMP(&output[0], &verif[0], 4)) << "main part is different";
	EXPECT_EQ (0, IMG_MEMCMP(&output[4], &verif[4], 3)) << "mixed bytes are different";
}

TEST(BT656, 10)
{
	IMG_UINT16 input[6] = { 0x123, 0x167, 0x1AB, 0x1EF, 0x123, 0x167 };
	IMG_UINT8 verif[8] = { 0x23, 0x9D, 0xB5, 0x1A, 0xEF, 0x8D, 0x74, 0x16 }, output[8];

	IMG_MEMSET(output, 0, 8);
	ASSERT_EQ (8, DG_ConvBayerToBT656_10(&input[0], &input[1], 2, 2, 3, output));

	EXPECT_EQ (0, IMG_MEMCMP(output, verif, 8)) << "packing failed";
}

TEST(BT656, 12)
{
	IMG_UINT16 input[4] = { 0x123, 0x567, 0x9AB, 0xDEF };
	IMG_UINT8 verif[6] = { 0x23, 0x71, 0x56, 0xAB, 0xF9, 0xDE }, output[6];

	IMG_MEMSET(output, 0, 6);
	ASSERT_EQ (6, DG_ConvBayerToBT656_12(&input[0], &input[1], 2, 2, 2, output));

	EXPECT_EQ (0, IMG_MEMCMP(&output[0], &verif[0], 3)) << "1st triplet is wrong";
	EXPECT_EQ (0, IMG_MEMCMP(&output[3], &verif[3], 3)) << "2nd triplet is wrong";
}

TEST(DG_Conv, writeLine_failure)
{
	Datagen driver;
	DG_CAMERA *pCamera = NULL;
	IMG_UINT16 input[32];
	IMG_UINT8 output[64];

	driver.configure();

	ASSERT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera));

	// returns nb of written bytes
	EXPECT_EQ (0, DG_ConvWriteLine(NULL, NULL, 0, NULL, DG_MIPI_FRAMESTART, NULL));
	EXPECT_EQ (0, DG_ConvWriteLine(pCamera->pConverter, NULL, 18, output, DG_MIPI_FRAMESTART, NULL));
	EXPECT_EQ (0, DG_ConvWriteLine(pCamera->pConverter, input, 0, output, DG_MIPI_FRAMESTART, NULL));
	EXPECT_EQ (0, DG_ConvWriteLine(pCamera->pConverter, input, 18, NULL, DG_MIPI_FRAMESTART, NULL));

	EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));
	// driver deinit here
}

TEST(DG_Conv, getConvertedSize_failure)
{
	Datagen driver;
	DG_CAMERA *pCamera = NULL;

	driver.configure();

	ASSERT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera));

	EXPECT_EQ (0, DG_ConvGetConvertedSize(pCamera->pConverter, 1)) << "pfnConverter is NULL";
	EXPECT_EQ (0, DG_ConvGetConvertedSize(NULL, 0));
	EXPECT_EQ (0, DG_ConvGetConvertedSize(pCamera->pConverter, 0));

	EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));
	// driver deinit here
}

TEST(DG_Conv, fromPGM_failure)
{
	Datagen driver;
	DG_CAMERA *pCamera = NULL;
	sSimImageIn sFakePGM;
	IMG_UINT8 output[64];

	driver.configure();
	
	sFakePGM.info.ui32Width = 8;
	sFakePGM.info.ui32Height = 8;
	sFakePGM.pBuffer = NULL;
	sFakePGM.info.ui8BitDepth = 16;

	ASSERT_EQ (IMG_SUCCESS, DG_CameraCreate(&pCamera));

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_ConvSimImage(NULL, NULL, NULL, 0, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_ConvSimImage(pCamera->pConverter, &sFakePGM, output, 0, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_ConvSimImage(pCamera->pConverter, &sFakePGM, NULL, 8, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_ConvSimImage(pCamera->pConverter, NULL, output, 8, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DG_ConvSimImage(NULL, &sFakePGM, output, 8, NULL));

	EXPECT_EQ (IMG_SUCCESS, DG_CameraDestroy(pCamera));
	// driver deinit here
}
