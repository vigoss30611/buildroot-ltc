/**
******************************************************************************
 @file dpfmap.c

 @brief Handle the Defective Pixel map format

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
#include "felixcommon/dpfmap.h"

#include <img_errors.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "DPF_OUT"
#include <felixcommon/userlog.h>

// define to enable DPF output format check
#define DPF_CONV_CHECK_FMT

IMG_RESULT DPF_Load_bin(IMG_UINT16 **ppDefect, IMG_UINT32 *pNDefects, const char *pszFilenane)
{
	int nDefect = 1024;
	int nLoaded = 0;
	IMG_UINT16 *pDefect = NULL;
	IMG_UINT32 value = 0;
	FILE *f = NULL;

    if ( ppDefect == NULL || pNDefects == NULL || pszFilenane == NULL )
    {
        LOG_DEBUG("NULL parameter\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
	if ( *ppDefect != NULL )
	{
        LOG_DEBUG("ppDefect already allocated\n");
		return IMG_ERROR_ALREADY_INITIALISED;
	}

	if ( (f = fopen(pszFilenane, "rb")) == NULL )
	{
		LOG_ERROR("failed to open %s!\n", pszFilenane);
        return IMG_ERROR_FATAL;
	}

	pDefect = (IMG_UINT16*)calloc(2*nDefect, sizeof(IMG_UINT16));
	if ( pDefect == NULL )
	{
		fclose(f);
		LOG_ERROR("failed to allocate intermediate map.\n");
		return IMG_ERROR_MALLOC_FAILED;
	}

	while( fread(&value, DPF_MAP_INPUT_SIZE, 1, f) > 0 )
	{
		pDefect[2*nLoaded + 0] = (IMG_UINT16)((value>>DPF_MAP_COORD_X_COORD_SHIFT)&DPF_MAP_COORD_X_COORD_LSBMASK);
		pDefect[2*nLoaded + 1] = (IMG_UINT16)((value>>DPF_MAP_COORD_Y_COORD_SHIFT)&DPF_MAP_COORD_Y_COORD_LSBMASK);

#ifdef DPF_CONV_CHECK_FMT // test valid format
		{
			int coordMask = DPF_MAP_COORD_X_COORD_MASK|DPF_MAP_COORD_Y_COORD_MASK;

			if ( (value&coordMask) != value )
			{
				LOG_ERROR("binary is corrupted at DPF correction %d (offset=0x%x): coord=0x%08x (coord mask=0x%08x)\n",
					nLoaded, nLoaded*DPF_MAP_OUTPUT_SIZE, value, coordMask);
				fclose(f);
				free(pDefect);
				return EXIT_FAILURE;
			}

		}
#endif

		nLoaded++;

		if ( nLoaded == nDefect )
		{
			// we need more...
			IMG_UINT16 *pDefect2 = NULL;

			pDefect2 = (IMG_UINT16*)calloc(2*(2*nDefect), sizeof(IMG_UINT16));
			if ( pDefect2 == NULL )
			{
				fclose(f);
				LOG_ERROR("failed to allocate intermediate map (%u Bytes).\n", 2*2*nDefect);
				free(pDefect);
				return IMG_ERROR_MALLOC_FAILED;
			}

			memcpy(pDefect2, pDefect, 2*nDefect*sizeof(IMG_UINT16));
			free(pDefect);
			nDefect *= 2;
			pDefect = pDefect2;
		}
	}

	fclose(f);

	*pNDefects = nLoaded;
	*ppDefect = (IMG_UINT16*)malloc(2*nLoaded*sizeof(IMG_UINT16));
	if ( *ppDefect == NULL )
	{
		LOG_ERROR("failed to allocate DPF map for MC\n");
		free(pDefect);
		return IMG_ERROR_MALLOC_FAILED;
	}

	LOG_INFO("INFO: Using DPF input map %s with %d elements\n", pszFilenane, nLoaded);
	memcpy(*ppDefect, pDefect, 2*nLoaded*sizeof(IMG_UINT16));
	free(pDefect);
	return IMG_SUCCESS;
}
