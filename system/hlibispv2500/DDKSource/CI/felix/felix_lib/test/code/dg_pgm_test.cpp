/**
******************************************************************************
 @file dg_pgm_test.cpp

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

#include <sim_image.h>

#include "unit_tests.h"

TEST(SIMIMAGEIN, create_destroy)
{
	sSimImageIn PGM;

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SimImageIn_init(NULL));

	// sSIMIMAGE is in fact hiding for sSIMIMAGE that does not use the malloc that can be forced to fail
	/*gui32AllocFails = 1;
	EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, SimImageIn_create(&pPGM));*/
	EXPECT_EQ (IMG_SUCCESS, SimImageIn_init(&PGM));
	//EXPECT_EQ (IMG_ERROR_MEMORY_IN_USE, SimImageIn_init(&PGM));

	EXPECT_EQ (IMG_SUCCESS, SimImageIn_clean(&PGM));
	// can clean twice, no effect
	EXPECT_EQ (IMG_SUCCESS, SimImageIn_clean(&PGM));
	//pPGM = NULL;
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SimImageIn_clean(NULL));

	// allocate buffers
	EXPECT_EQ (IMG_SUCCESS, SimImageIn_init(&PGM));
	PGM.info.ui32Width = 5;
	PGM.info.ui32Height = 5;

	PGM.pBuffer = (IMG_UINT16*)IMG_MALLOC(PGM.info.ui32Width*PGM.info.ui32Height*sizeof(IMG_UINT16));

	EXPECT_EQ (IMG_SUCCESS, SimImageIn_clean(&PGM));
}

//#define FLX_FILE "/Uncompressed_FLX/RGGB_video/fight_960x540-12b-91.flx"
TEST(SIMIMAGE, loadFLX)
{
	sSimImageIn PGM;
	char filename[] = "testdata/balloons_64x48_rggb.flx"; // = SAMBA_PATH("Uncompressed_FLX/fight_960x540-12b-91.flx");
	/*IMG_ASSERT(1024>=strlen(samba_videosource)+strlen(FLX_FILE));
	sprintf(filename, "%s%s", samba_videosource, FLX_FILE);*/

	ASSERT_EQ (IMG_SUCCESS, SimImageIn_init(&PGM));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SimImageIn_loadFLX(NULL, NULL, 0));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SimImageIn_loadFLX(filename, NULL, 0));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SimImageIn_loadFLX(NULL, &PGM, 0));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SimImageIn_loadFLX(filename, &PGM, -1));

	ASSERT_EQ (IMG_SUCCESS, SimImageIn_loadFLX(filename, &PGM, 0)) << filename;

	SimImageIn_clean(&PGM);
}
