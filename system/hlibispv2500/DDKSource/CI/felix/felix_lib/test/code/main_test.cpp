/**
******************************************************************************
 @file main_test.cpp

 @brief Point of entry of the unit tests

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
#include <ostream>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <ci/ci_version.h>

#include "unit_tests.h"

#define VIDEOSOURCE_ARG "--videosource"

IMG_UINT32 gui32AllocFails = 0;

static char def_smb_videosource[] = SAMBA_VIDEOSOURCE;
char *samba_videosource = NULL;

//
// alocate samba_videosource either with latest arg that has a path or with default value SAMBA_VIDEOSOURCE
//
void extractVideosourcePath(int argc, char** argv)
{
	size_t argLen = strlen(VIDEOSOURCE_ARG);

	for ( int i = argc-1 ; i > 0 ; i-- )
	{
		if ( strncmp(argv[i], VIDEOSOURCE_ARG, argLen) == 0 )
		{
			if ( i+1 < argc )
			{
				samba_videosource = argv[i+1];
				return; // path is found - stop here
			}
			else
			{
				std::cerr << "argument " << argv[i] << " is not followed by a path" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}

	// path is not found in parameters put default
	samba_videosource = def_smb_videosource;
}

int main(int argc, char** argv)
{
	int ret = EXIT_SUCCESS;
	bool smbPathFound = false;

	extractVideosourcePath(argc, argv);

	//printf ("CI_LOG_LEVEL=%d\n", CI_LOG_LEVEL);
	printf ("CHANGELIST %d - DATE %s\n", CI_CHANGELIST, CI_DATE_STR);
	printf ("videsource %s\n", samba_videosource);

	::testing::InitGoogleTest(&argc, argv);
	ret = RUN_ALL_TESTS();

	return ret;
}
