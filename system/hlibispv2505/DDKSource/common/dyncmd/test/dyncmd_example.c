/**
******************************************************************************
 @file dyncmd_example.c

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
#include <stdio.h>
#include <stdlib.h>

#include <img_types.h>

#include <dyncmd/commandline.h>

/**
 * @file dyncmd_example.c
 * @brief Example of usage of the DynCMD library
 *
 * 
 */
 
int main(int argc, char *argv[]) {
	IMG_UINT uiFormat, uiRet;
	IMG_BOOL8 bMIPILF, bForceQuit = IMG_FALSE;

	// defaults
	uiFormat = 0;
	bMIPILF = IMG_FALSE;

	// add command line
	DYNCMD_AddCommandLine(argc, argv, "-source");

	// parse cmd
	if ( DYNCMD_RegisterParameter("-format", DYNCMDTYPE_UINT, "flag to choose format: 1=MIPI 0=BT656", &uiFormat) != RET_FOUND ) {
		fprintf(stderr, "could not find the format\n");
		bForceQuit = IMG_TRUE;
	}
	if ( uiFormat == 1 ) {// MIPI
		if ( (uiRet = DYNCMD_RegisterParameter("-MIPI_LF", DYNCMDTYPE_BOOL8, "activate MIPI Line Flag - valid only for MIPI format - optional", &bMIPILF)) != RET_FOUND ) {
			if ( uiRet != RET_NOT_FOUND ) { // if error or incorrect
				fprintf(stderr, "invalid parameter for -MIPI_LF\n");
				bForceQuit = IMG_TRUE;
			}
			// not found - it's alright the parameter is optional
		}
	}

	if ( DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND, "print help", NULL) == RET_FOUND || bForceQuit == IMG_TRUE ) {
		DYNCMD_PrintUsage();
		DYNCMD_ReleaseParameters();
		return bForceQuit == IMG_TRUE ? EXIT_FAILURE : EXIT_SUCCESS;
	}

	// now print if some paramters are given but not registered
	DYNCMD_HasUnregisteredElements(&uiRet);

	DYNCMD_ReleaseParameters(); // free mem

	printf ("format to use %s\n  MIPI_LF=%s\n", uiFormat == 1 ? "MIPI" : "BT656", bMIPILF ? "yes" : "no");

	return EXIT_SUCCESS;
}
