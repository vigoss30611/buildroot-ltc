/**
******************************************************************************
 @file pdump_test.cpp

 @brief Test some pdump functions

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
#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <gtest/gtest.h>

#include "tal_test.h"
#include "target.h"

#ifdef WIN32
#include <Windows.h>
#endif

#define SINGLE_FOLDER "pdump_single"
#define MULTI_FOLDER "pdump_multi"

struct Pdump: public ::testing::Test, public TALBase
{
	void SetUp()
	{
		initialise(); // using structure
	}
	
	void TearDown()
	{
		finalise();
	}
};

/**
 * @brief Used to fake some operation to create some pdumps elements
 */
void fakeActions(Pdump &tal)
{
	for ( size_t i = 0 ; i < tal.nRegBanks ; i++ )
	{
		TALREG_WriteWord32(tal.aRegBanks[i], 0x8, 0x42);
	}
	for ( size_t i = 0 ; i < tal.nMemory ; i++ )
	{
		char *tmp = (char*)calloc(32, sizeof(char));
		IMG_HANDLE sDevMem;
		TALMEM_Malloc(tal.aMemory[i], tmp, 32, 8, &sDevMem, IMG_FALSE, NULL);

		TALMEM_WriteWord32(sDevMem, 0x8, 0x42);

		TALMEM_Free(&sDevMem);
		free(tmp);
	}
}

void destroyDir(const char* name)
{
#ifdef WIN32
	RemoveDirectory(name);
#else
	// a bit dirty...
	char cmd[256];
	sprintf(cmd, "rm -rf %s", name);
	system(cmd);
#endif
}

/**
 * @brief Checks that single pdump creates the correct files
 */
TEST_F(Pdump, single_pdump)
{
	// initialised before that

	// enable pdump
	for ( size_t i = 0 ; i < this->nRegBanks ; i++ )
	{
		TALPDUMP_MemSpceCaptureEnable(this->aRegBanks[i], IMG_TRUE, NULL);
	}
	for ( size_t i = 0 ; i < this->nMemory ; i++ )
	{
		TALPDUMP_MemSpceCaptureEnable(this->aMemory[i], IMG_TRUE, NULL);
	}
	
	destroyDir(SINGLE_FOLDER);

	TALPDUMP_SetFlags(TAL_PDUMP_FLAGS_PDUMP2|TAL_PDUMP_FLAGS_PDUMP1|TAL_PDUMP_FLAGS_PRM|TAL_PDUMP_FLAGS_RES); // TAL_PDUMP_FLAGS_GZIP
	TALPDUMP_CaptureStart(SINGLE_FOLDER);
	fakeActions(*this);
	TALPDUMP_CaptureStop();

	// check that the pdump files and folder were created
	FILE *f = NULL;

	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out2.txt", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out2.res", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out2.prm", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out.txt", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out.res", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out.prm", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	// we could now clean the directory...

	// finalised after that
}

/**
 * @brief Checks that multi pdump creates the correct files
 */
TEST_F(Pdump, multi_pdump)
{
	// initialised before that
	char name[256];

	// enable pdump
	for ( size_t i = 0 ; i < this->nRegBanks ; i++ )
	{
		sprintf(name, "reg%d", i);
		TALPDUMP_AddMemspaceToContext(name, this->aRegBanks[i]);
		TALPDUMP_MemSpceCaptureEnable(this->aRegBanks[i], IMG_TRUE, NULL);
	}
	for ( size_t i = 0 ; i < this->nMemory ; i++ )
	{
		sprintf(name, "mem%d", i);
		TALPDUMP_AddMemspaceToContext(name, this->aMemory[i]);
		TALPDUMP_MemSpceCaptureEnable(this->aMemory[i], IMG_TRUE, NULL);
	}
	
	destroyDir(MULTI_FOLDER);

	TALPDUMP_SetFlags(TAL_PDUMP_FLAGS_PDUMP2|TAL_PDUMP_FLAGS_PDUMP1|TAL_PDUMP_FLAGS_PRM|TAL_PDUMP_FLAGS_RES); // TAL_PDUMP_FLAGS_GZIP
	TALPDUMP_CaptureStart(MULTI_FOLDER);
	fakeActions(*this);
	TALPDUMP_CaptureStop();

	// check that the pdump files and folder were created
	FILE *f = NULL;

	EXPECT_TRUE ((f =  fopen(MULTI_FOLDER "/auto.tdf", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	for ( size_t i = 0 ; i < this->nRegBanks ; i++ )
	{
		sprintf(name, "%s/reg%d_out2.txt", MULTI_FOLDER, i);
		EXPECT_TRUE ((f =  fopen(name, "r")) != NULL) << "fopen should not have failed!";
		if ( f != NULL ) fclose(f);

		sprintf(name, "%s/reg%d_out2.res", MULTI_FOLDER, i);
		EXPECT_TRUE ((f =  fopen(name, "r")) != NULL) << "fopen should not have failed!";
		if ( f != NULL ) fclose(f);

		sprintf(name, "%s/reg%d_out2.prm", MULTI_FOLDER, i);
		EXPECT_TRUE ((f =  fopen(name, "r")) != NULL) << "fopen should not have failed!";
		if ( f != NULL ) fclose(f);
	}

	for ( size_t i = 0 ; i < this->nMemory ; i++ )
	{
		sprintf(name, "%s/mem%d_out2.txt", MULTI_FOLDER, i);
		EXPECT_TRUE ((f =  fopen(name, "r")) != NULL) << "fopen should not have failed!";
		if ( f != NULL ) fclose(f);

		sprintf(name, "%s/mem%d_out2.res", MULTI_FOLDER, i);
		EXPECT_TRUE ((f =  fopen(name, "r")) != NULL) << "fopen should not have failed!";
		if ( f != NULL ) fclose(f);

		sprintf(name, "%s/mem%d_out2.prm", MULTI_FOLDER, i);
		EXPECT_TRUE ((f =  fopen(name, "r")) != NULL) << "fopen should not have failed!";
		if ( f != NULL ) fclose(f);
	}

	// pdump 1 does not support multiple file!
	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out.txt", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out.res", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	EXPECT_TRUE ((f =  fopen(SINGLE_FOLDER "/out.prm", "r")) != NULL) << "fopen should not have failed!";
	if ( f != NULL ) fclose(f);

	// we could now clean the directory...

	// finalised after that
}