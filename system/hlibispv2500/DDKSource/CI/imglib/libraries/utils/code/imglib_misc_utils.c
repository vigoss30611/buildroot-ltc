/*! 
******************************************************************************
 @file   : imglib_misc_utils.c

 @brief  

 @Author Tom Whalley

 @date   24/05/2007
 
         <b>Copyright 2003 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 <b>Description:</b>\n
         This module contains various utility functions

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
 *****************************************************************************/

#if !defined(IMG_KERNEL_MODULE)
#include <stdio.h>
#include <string.h>
#endif

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#if defined IMGLIB_UTILS_INCLUDE_IMG_FILE_FUNCTIONS
	#include "pixel_api.h"
#endif

#include "img_utils.h"

#if defined IMGLIB_UTILS_INCLUDE_IMG_FILE_FUNCTIONS
	#define	IMGLIB_UTILS_MAX_SEPARATE_SECTIONS_TO_LOAD_PER_IMAGE	3 /* Luma, Chroma, Alpha */

	typedef struct IMGLIB_UTILS_tag_sImageLoadInfo
	{
		IMG_UINT8 *		pui8DestinationAddress;
		IMG_UINT32		ui32LinesToRead;
		IMG_UINT32		ui32ShorterStride;
		IMG_UINT32		ui32RemainderInFilePerLine;
		IMG_UINT32		ui32RemainderInBufferPerLine;

	} IMGLIB_UTILS_sImageLoadInfo;
#endif

#define REMOVE_LINE_ENDINGS(string)					\
{													\
	IMG_INT64	i64i;								\
													\
	if ( strlen(string)>0 )							\
	{												\
		for ( i64i=strlen(string); i64i>0; i64i-- )	\
		{											\
			if (( string[(i64i-1)] == 0x0A ) ||		\
				( string[(i64i-1)] == 0x0D ))		\
			{										\
				string[(i64i-1)] = 0x00;			\
			}										\
			else									\
			{										\
				break;								\
			}										\
		}											\
	}												\
}

/* Variables for SeedRandGen(), RandGen() */
unsigned long    mt[N]; // the array for the state vector  
int              mti;
unsigned long    mag01[2];

/* X25 CRC bits'n'pieces */
#define CRC_INIT			0xFFFFU
#define CRC_RES				0x1D0FU

/* CRC Calculation table */
const unsigned short X25Table16[16] = {
	0x0000, 0x1021,
	0x2042, 0x3063,
	0x4084, 0x50A5,
	0x60C6, 0x70E7,
	0x8108, 0x9129,
	0xA14A, 0xB16B,
	0xC18C, 0xD1AD,
	0xE1CE, 0xF1EF
};

char	acNameErrorString [] = "<No match found>\0";

unsigned char * FindFirstCharacterNotInSet (IMG_CHAR * pcStringToCheck, IMG_CHAR * pcTestSet)
{
	IMG_UINT32	i, j;
	IMG_BOOL	bFoundIt;

	/* Note : This function does the same thing as the Windows only function '_mbsspnp' */
	/*        (This is a platform independent implementation of the same thing).		*/

	if (( pcStringToCheck == IMG_NULL ) ||
		( pcTestSet == IMG_NULL ))
	{
		return IMG_NULL;
	}

	/* Check every character of the 'string to check' */
	for ( i=0; i<strlen(pcStringToCheck); i++ )
	{
		bFoundIt = IMG_FALSE;

		/* Is this character in the test set ? */
		for ( j=0; j<strlen(pcTestSet); j++ )
		{
			if ( pcStringToCheck[i] == pcTestSet [j] )
			{
				/* This character *is* in the test set */
				bFoundIt = IMG_TRUE;
				break;
			}
		}

		if ( bFoundIt == IMG_FALSE )
		{
			/* This character was not in the test set */
			return (unsigned char *)(&(pcStringToCheck[i]));
		}
	}

	/* All the characters in the string to check were in the test set */
	return IMG_NULL;
}

void * GetThingFromID ( signed long		IDToFind,
						void *			pTable,
						unsigned long	EntrySize,
						unsigned long	TableLengthInBytes )
{
	unsigned int		i;

	char *	pThisID = (char *) pTable;

	if (( TableLengthInBytes < EntrySize ) ||
		(( TableLengthInBytes % EntrySize ) != 0 ) ||
		 ( pTable == IMG_NULL ))
	{
		IMG_ASSERT (0);
	}

	for (i=0; i<(TableLengthInBytes/EntrySize); i++)
	{
		if ( (signed long) *pThisID == IDToFind )
		{
			return ((void *) pThisID);
		}
		else
		{
			pThisID += EntrySize;
		}
	}

	return NULL;
}

char * GetNameFromID (	signed long		IDToFind,
						void *			pTable,
						unsigned long	EntrySize,
						unsigned long	TableLengthInBytes )
{
	ID_Name_Pair *		psThisPair = NULL;

	psThisPair = (ID_Name_Pair *) GetThingFromID ( IDToFind, pTable, EntrySize, TableLengthInBytes );
	if ( psThisPair == NULL )
	{
		return (acNameErrorString);
	}

	return (psThisPair->Name);
}

ID_Name_Pair *	GetThingFromName (	char *			pacNameToFind,
									ID_Name_Pair *	pasTable,
									unsigned long	TableLengthInBytes )
{
	IMG_UINT32		i;

	ID_Name_Pair *	psThisEntry = pasTable;

	if (( TableLengthInBytes < sizeof (ID_Name_Pair) ) ||
		(( TableLengthInBytes % sizeof (ID_Name_Pair) ) != 0 ) ||
		 ( pasTable == IMG_NULL ))
	{
		IMG_ASSERT (0);
	}

	for (i=0; i<(TableLengthInBytes/sizeof (ID_Name_Pair)); i++)
	{
		if ( strcmp ( psThisEntry->Name, pacNameToFind ) == 0 )
		{
			return ((void *) psThisEntry);
		}
		else
		{
			psThisEntry ++;
		}
	}

	return IMG_NULL;
}

void * GetThingFromName2 (	char *			pacNameToFind,
							void *			pTable,
							unsigned long	EntrySize,
							unsigned long	TableLengthInBytes )
{
	unsigned int		i;

	char *	pui8ThisEntry = (char *) pTable;

	if (( TableLengthInBytes < EntrySize ) ||
		(( TableLengthInBytes % EntrySize ) != 0 ) ||
		 ( pTable == IMG_NULL ))
	{
		IMG_ASSERT (0);
	}

	for (i=0; i<(TableLengthInBytes/EntrySize); i++)
	{
		if ( strcmp ( pacNameToFind, ((ID_Name_Pair *) pui8ThisEntry)->Name ) == 0 )
		{
			return ((void *) pui8ThisEntry);
		}
		else
		{
			pui8ThisEntry += EntrySize;
		}
	}

	return NULL;
}

#if !defined(IMG_KERNEL_MODULE)
IMG_RESULT	ParseStringOfTokens (	IMG_CHAR *					pacString,
									IMG_CHAR *					pcCommentCharacter,
									fn_TokenParsingFunction		pfnTokenParsingFunction	)
{
	char *			pcToken;
	IMG_UINT32		ui32TokenCount;
	IMG_RESULT		rParsingResult;

	static	char	acSeparators []	= " \t";

	/* A token parsing function must be provided */
	IMG_ASSERT ( pfnTokenParsingFunction != IMG_NULL );

	/* Remove trailing 0x0Ds and 0x0As, so that UNIX and DOS lines are treated		*/
	/* identically.																	*/
	REMOVE_LINE_ENDINGS(pacString);

	/* Call parsing function with token 0 - this is used to prompt parsing function */
	/* to initialise.																*/
	pfnTokenParsingFunction ( IMG_NULL, 0, IMG_FALSE );

	pcToken = strtok ( pacString, acSeparators );
	ui32TokenCount = 0;
	while ( pcToken != IMG_NULL )
	{
		if ((( pcCommentCharacter != IMG_NULL ) && ( pcToken [0] == pcCommentCharacter[0] ))
			||
			( strlen (pcToken) == 0x00 ))
		{
			/* This line is either blank, or only contains a comment */
			break;
		}
		else
		{
			/* This is a valid token */
			ui32TokenCount ++;
			rParsingResult = pfnTokenParsingFunction ( pcToken, ui32TokenCount, IMG_FALSE );
			if( rParsingResult != IMG_SUCCESS )
			{
				return rParsingResult;
			}
		}

		pcToken = strtok ( IMG_NULL, acSeparators );
	}

	/* Indicate end of string */
	rParsingResult = pfnTokenParsingFunction ( IMG_NULL, ui32TokenCount, IMG_TRUE );
	if( rParsingResult != IMG_SUCCESS )
	{
		return rParsingResult;
	}

	return IMG_SUCCESS;
}
#endif

IMG_VOID	SeedRandGen ( IMG_UINT32 ui32Seed )
{
	mt[0]= ui32Seed & 0xffffffffUL;
    
	mti=N+1;    // mti==N+1 means mt[N] is not initialized 

	for (mti=1; mti<N; mti++) {
		mt[mti] = 
 		(1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		mt[mti] &= 0xffffffffUL;
		/* for >32 bit machines */
	}

	mag01[0]  = 0x0UL;
	mag01[1] =  MATRIX_A;
}

IMG_UINT32	RandGen	( IMG_VOID )
{
    IMG_UINT32 y;

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if SeedRandGen() has not been called, */
          SeedRandGen(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

IMG_UINT32 NextHighestPowerOfTwo (IMG_UINT32 k) 
{
	IMG_UINT32	i;

	k--;
	for (i=1; i<sizeof(IMG_UINT32)*8; i=i*2)
	{
		k = k | k >> i;
	}
       
	return k+1;
}

IMG_UINT32 WhatPowerOfTwoIsThis (IMG_UINT32 k)
{
	IMG_UINT32	i=0;

	/* Check that this is a power of two */
	IMG_ASSERT ( (k&(k-1)) == 0 );

	while ( k > 0 )
	{
		i++;
		k = k>>1;
	}

	IMG_ASSERT (i>0);
	return (i-1);
}

/******************************************************************************
* Name		: CrcX25Core( char *pData, unsigned int Size )
*
*			  Main loop common to both CRC functions
*
* Arguments	: pData    - Base address from which to checksum data
*             Size     - Size in bytes to checksum
*
* Returns	: X25 value of vector
*			  pData points to the location after the data
*
******************************************************************************/
unsigned short CrcX25Core( unsigned char **pData, unsigned int Count )
{
	const unsigned short *pX25 = X25Table16;
    unsigned short Data, Temp, CrcReg = CRC_INIT;

    do
    {
    	/* Get data */
    	Data = (unsigned short) (*(*pData)++);
    	
	    /* Use 4 bits out of the data/polynomial at a time */
	    Temp = (unsigned short) (CrcReg >> 12u);
	    Temp ^= Data >> 4u;	/* xor data (MS 4 bits) with the MS 4 bits */
		Temp &= 0xf;			/* of the CRC reg to be used as index in array */
		CrcReg = (unsigned short)((CrcReg << 4u) ^ pX25[Temp]);

		/* Now do second half of byte */
		Temp = (unsigned short) (CrcReg >> 12u);
		Temp ^= Data;		/* xor data with the 4 LS bits of the CRC reg */
		Temp &= 0xf;		/* to be used as index in array */
		CrcReg = (unsigned short)((CrcReg << 4u) ^ pX25[Temp]);
    }
    while ( --Count != 0 );

	return CrcReg;
}

/******************************************************************************
* Name		: BootCRCX25( char *pData, unsigned int Size )
*
*			  Compute CRC according to the Slave Boot spec
*
* Arguments	: pData    - Base address from which to checksum data
*             Size     - Size in bytes to checksum
*
* Returns   : X25 value of vector xored with fixed non-zero 'expected' result
*
* Effects	: Assumes the CRC field is at the end of the area so does not
*             need to waste time checking it.
*
******************************************************************************/
unsigned short BootCRCX25( unsigned char *pData, unsigned int Size )
{
	unsigned short CRC, CrcReg;

	CrcReg = CrcX25Core( &pData, Size-2 );

	/* Extract big-endian CRC at the end - set to zero when encoding */
	CRC = (unsigned short) ( (((unsigned short) pData[0])<<8) +
	                          ((unsigned short) pData[1])       );

	/* For a balanced packet we should get CRC_RES; hence return delta */
	return ((unsigned short) (CrcReg ^ CRC_RES ^ CRC));
}

#if defined IMGLIB_UTILS_INCLUDE_IMG_FILE_FUNCTIONS	/* Require PIXEL library to be built as well */
	IMG_RESULT OpenIMGFile ( IMG_sImageFileHeaderExtended * psFileInfo )
	{
		IMG_UINT32		ui32FileEnd;

		PIXEL_sInfo *	psPixelInfo = IMG_NULL;

		IMG_ASSERT ( psFileInfo != IMG_NULL );

		/* Make sure file is not currently open */
		if ( psFileInfo->fFileHandle != IMG_NULL )
		{
			printf ( "ERROR in 'OpenIMGFile': Specified file is already open\n" );
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		/* Open file */
		psFileInfo->fFileHandle = fopen ( psFileInfo->acFileName, "rb" );
		if ( psFileInfo->fFileHandle == IMG_NULL )
		{
			printf ( "ERROR in 'OpenIMGFile': Could not open file '%s'\n", psFileInfo->acFileName );
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		/* Establish file size */
		if ( fseek ( psFileInfo->fFileHandle,
					0,
					SEEK_END ) != 0 )
		{
			printf ( "ERROR in 'OpenIMGFile': Unable to establish file size\n" );
			fclose (psFileInfo->fFileHandle);
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}
		else
		{
			ui32FileEnd = ftell (psFileInfo->fFileHandle);
			/* Rewind to start of file */
			if ( fseek ( psFileInfo->fFileHandle,
						0,
						SEEK_SET ) != 0 )
			{
				printf ( "ERROR - Failed to return to file start\n" );
				fclose (psFileInfo->fFileHandle);
				return IMGLIB_UTILS_ERROR__FILE_ERROR;
			}
		}

		if ( ui32FileEnd < sizeof (IMG_sImageFileHeader) )
		{
			printf ( "ERROR in 'OpenIMGFile': File is not long enough to contain an IMG file header\n" );
			fclose (psFileInfo->fFileHandle);
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		/* Read IMG header */
		if ( fread ( &psFileInfo->sIMGFileHeader, sizeof(IMG_sImageFileHeader), 1, psFileInfo->fFileHandle ) != 1 )
		{
			printf ( "ERROR in 'OpenIMGFile': failed to read IMG header\n" );
			fclose (psFileInfo->fFileHandle);
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		if (( psFileInfo->sIMGFileHeader.pszId[0] != 'I' ) ||
			( psFileInfo->sIMGFileHeader.pszId[1] != 'M' ) ||
			( psFileInfo->sIMGFileHeader.pszId[2] != 'G' ) ||
			( psFileInfo->sIMGFileHeader.pszId[3] != '\0' ))
		{
			printf ( "ERROR in 'OpenIMGFile': This is not a valid IMG file ('IMG' tag is not present)\n" );
			fclose (psFileInfo->fFileHandle);
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		if ( psFileInfo->sIMGFileHeader.ui32HdrVersion != 1 )
		{
			printf ( "ERROR in 'OpenIMGFile': IMG header (%i) version is incompatible with this tool (which works with version 1 only)\n", psFileInfo->sIMGFileHeader.ui32HdrVersion );
			fclose (psFileInfo->fFileHandle);
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}	

		if (( psFileInfo->sIMGFileHeader.ui32Width == 0 ) ||
			( psFileInfo->sIMGFileHeader.ui32Height == 0 ) ||
			( psFileInfo->sIMGFileHeader.ui32Stride == 0 ))
		{
			printf ( "ERROR in 'OpenIMGFile': Width, height and stride specified in IMG header must all be non zero\n" );
			fclose (psFileInfo->fFileHandle);
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		psFileInfo->psPixelInfo = PIXEL_GetBufferInfoFromPixelColourFormat ( (IMG_ePixelFormat) psFileInfo->sIMGFileHeader.ePixelFormat );
		if ( psFileInfo->psPixelInfo == IMG_NULL )
		{
			printf ( "ERROR in 'OpenIMGFile': Pixel colour format was not recognised\n" );
			fclose (psFileInfo->fFileHandle);
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		/* Calculate bytes per image */
		psFileInfo->ui32YBytes = psFileInfo->sIMGFileHeader.ui32Stride * psFileInfo->sIMGFileHeader.ui32Height;
		psFileInfo->ui32UVBytes = (psFileInfo->psPixelInfo->bIsPlanar ? 
			((psFileInfo->psPixelInfo->bUVStrideHalved ? (psFileInfo->sIMGFileHeader.ui32Stride/2) : psFileInfo->sIMGFileHeader.ui32Stride)
			*
			(psFileInfo->psPixelInfo->bUVHeightHalved ? (psFileInfo->sIMGFileHeader.ui32Height/2) : psFileInfo->sIMGFileHeader.ui32Height))
			:
			0);
		psFileInfo->ui32AlphaBytes = (psFileInfo->psPixelInfo->bHasAlpha ? psFileInfo->ui32YBytes : 0 );

		psFileInfo->ui32BytesPerImageInFile = psFileInfo->ui32YBytes + psFileInfo->ui32UVBytes + psFileInfo->ui32AlphaBytes;

		/* How many images does this file contain? */
		if ( ((ui32FileEnd - sizeof(IMG_sImageFileHeader)) % (psFileInfo->ui32BytesPerImageInFile)) != 0 )
		{
			printf ( "ERROR in 'OpenIMGFile': File does not contain an integer number of images, as described in header\n" );
			fclose (psFileInfo->fFileHandle);
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}
		else
		{
			psFileInfo->ui32NoOfImages = ((ui32FileEnd - sizeof(IMG_sImageFileHeader)) / (psFileInfo->ui32BytesPerImageInFile));
		}

		return IMG_SUCCESS;
	}

	IMG_RESULT ReadImageFromIMGFile (	IMG_sImageFileHeaderExtended *	psFileDescriptor,
										IMG_sImageBufCB *				psTargetBuffer,
										IMG_UINT32						ui32ImageIndexToLoad	)
	{
		IMG_UINT32						ui32ShorterStride;
		IMG_UINT32						ui32RemainderInFilePerLine;
		IMG_UINT32						ui32RemainderInBufferPerLine;
		IMG_UINT32						ui32ImageSectionsToLoad;
		IMG_UINT32						ui32ThisSection;
		IMG_UINT32						ui32ThisLine;
		IMG_UINT8 *						pui8ThisDestinationAddress;

		IMGLIB_UTILS_sImageLoadInfo		asImageLoadInfo [IMGLIB_UTILS_MAX_SEPARATE_SECTIONS_TO_LOAD_PER_IMAGE];

		IMG_ASSERT ( psFileDescriptor != IMG_NULL );
		IMG_ASSERT ( psTargetBuffer != IMG_NULL );

		/* Sanity check file descriptor */
		if ( psFileDescriptor->fFileHandle == IMG_NULL )
		{
			printf ( "ERROR in 'ReadImageFromIMGFile': File descriptor does not describe an open file\n" );
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		/* Skip to start of requested image */
		if ( ui32ImageIndexToLoad >= psFileDescriptor->ui32NoOfImages )
		{
			printf ( "ERROR in 'ReadImageFromIMGFile': Invalid image index requested\n" );
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}
		else
		{
			if ( fseek (	psFileDescriptor->fFileHandle,
							((ui32ImageIndexToLoad * psFileDescriptor->ui32BytesPerImageInFile) + sizeof(IMG_sImageFileHeader)),
							SEEK_SET	) != 0 )
			{
				printf ( "ERROR in 'ReadImageFromIMGFile': Error seeking requested image index\n" );
				return IMGLIB_UTILS_ERROR__FILE_ERROR;
			}
		}

		/* Check consistency of buffer with IMG file header */
		/* (Stride mismatch is allowed - read process will account for this) */
		if (( psTargetBuffer->ePixelFormat		!= psFileDescriptor->sIMGFileHeader.ePixelFormat )	||
			( psTargetBuffer->eBufferFormat		!= psFileDescriptor->sIMGFileHeader.eBufferFormat )	||
			( psTargetBuffer->bPlanar			!= psFileDescriptor->psPixelInfo->bIsPlanar )		||
			( psTargetBuffer->bAlpha			!= psFileDescriptor->psPixelInfo->bHasAlpha )		||
			( psTargetBuffer->bUVHeightHalved	!= psFileDescriptor->psPixelInfo->bUVHeightHalved )	||
			( psTargetBuffer->bUVStrideHalved	!= psFileDescriptor->psPixelInfo->bUVStrideHalved )	||
			( psTargetBuffer->ui32ImageWidth	!= psFileDescriptor->sIMGFileHeader.ui32Width )		||
			( psTargetBuffer->ui32ImageHeight	!= psFileDescriptor->sIMGFileHeader.ui32Height ))
		{
			printf ( "ERROR in 'ReadImageFromIMGFile': Target buffer is not of the right size and / or type for images in IMG file\n" );
			return IMGLIB_UTILS_ERROR__FILE_ERROR;
		}

		/* Establish which stride is the shorter */
		if ( psFileDescriptor->sIMGFileHeader.ui32Stride > psTargetBuffer->ui32ImageStride )
		{
			/* Lines in file are longer than lines in buffer */
			ui32ShorterStride = psTargetBuffer->ui32ImageStride;
			ui32RemainderInFilePerLine = (psFileDescriptor->sIMGFileHeader.ui32Stride) - (psTargetBuffer->ui32ImageStride);
			ui32RemainderInBufferPerLine = 0;;
		}
		else
		{
			/* Lines in buffer are longer than lines in file */
			ui32ShorterStride = psFileDescriptor->sIMGFileHeader.ui32Stride;
			ui32RemainderInFilePerLine = 0;
			ui32RemainderInBufferPerLine = (psTargetBuffer->ui32ImageStride) - (psFileDescriptor->sIMGFileHeader.ui32Stride);
		}

		/* Luma / RGB */
		asImageLoadInfo[0].pui8DestinationAddress		= (IMG_UINT8 *) psTargetBuffer->pvYBufAddr;
		asImageLoadInfo[0].ui32LinesToRead				= psTargetBuffer->ui32ImageHeight;
		asImageLoadInfo[0].ui32ShorterStride			= ui32ShorterStride;
		asImageLoadInfo[0].ui32RemainderInFilePerLine	= ui32RemainderInFilePerLine;
		asImageLoadInfo[0].ui32RemainderInBufferPerLine	= ui32RemainderInBufferPerLine;
		ui32ImageSectionsToLoad							= 1;

		/* Chroma */
		if ( psTargetBuffer->bPlanar == IMG_TRUE )
		{
			if ( psTargetBuffer->bUVHeightHalved == IMG_TRUE )
			{
				IMG_ASSERT (( psTargetBuffer->ui32ImageHeight % 2 ) == 0);
				asImageLoadInfo[ui32ImageSectionsToLoad].ui32LinesToRead = (psTargetBuffer->ui32ImageHeight / 2);
			}
			else
			{
				asImageLoadInfo[ui32ImageSectionsToLoad].ui32LinesToRead = psTargetBuffer->ui32ImageHeight;
			}

			if ( psTargetBuffer->bUVStrideHalved == IMG_TRUE )
			{
				IMG_ASSERT (( ui32ShorterStride % 2 ) == 0);
				IMG_ASSERT (( ui32RemainderInFilePerLine % 2 ) == 0);
				IMG_ASSERT (( ui32RemainderInBufferPerLine % 2 ) == 0);

				asImageLoadInfo[ui32ImageSectionsToLoad].ui32ShorterStride				= (ui32ShorterStride / 2);
				asImageLoadInfo[ui32ImageSectionsToLoad].ui32RemainderInFilePerLine		= (ui32RemainderInFilePerLine / 2);
				asImageLoadInfo[ui32ImageSectionsToLoad].ui32RemainderInBufferPerLine	= (ui32RemainderInBufferPerLine / 2);
			}
			else
			{
				asImageLoadInfo[ui32ImageSectionsToLoad].ui32ShorterStride				= ui32ShorterStride;
				asImageLoadInfo[ui32ImageSectionsToLoad].ui32RemainderInFilePerLine		= ui32RemainderInFilePerLine;
				asImageLoadInfo[ui32ImageSectionsToLoad].ui32RemainderInBufferPerLine	= ui32RemainderInFilePerLine;
			}

			asImageLoadInfo[ui32ImageSectionsToLoad].pui8DestinationAddress	= (IMG_UINT8 *) psTargetBuffer->pvUVBufAddr;
			ui32ImageSectionsToLoad ++;
		}

		/* Alpha */
		if ( psTargetBuffer->bPlanar == IMG_TRUE &&
             psTargetBuffer->pvAlphaBufAddr )
		{
			asImageLoadInfo[ui32ImageSectionsToLoad].pui8DestinationAddress			= (IMG_UINT8 *) psTargetBuffer->pvAlphaBufAddr;
			asImageLoadInfo[ui32ImageSectionsToLoad].ui32LinesToRead				= psTargetBuffer->ui32ImageHeight;
			asImageLoadInfo[ui32ImageSectionsToLoad].ui32ShorterStride				= ui32ShorterStride;
			asImageLoadInfo[ui32ImageSectionsToLoad].ui32RemainderInFilePerLine		= ui32RemainderInFilePerLine;
			asImageLoadInfo[ui32ImageSectionsToLoad].ui32RemainderInBufferPerLine	= ui32RemainderInBufferPerLine;
			ui32ImageSectionsToLoad	++;
		}

		/* Now load data */
		for ( ui32ThisSection=0; ui32ThisSection<ui32ImageSectionsToLoad; ui32ThisSection++ )
		{
			for ( ui32ThisLine=0; ui32ThisLine<asImageLoadInfo[ui32ThisSection].ui32LinesToRead; ui32ThisLine++ )
			{
				pui8ThisDestinationAddress = &(asImageLoadInfo[ui32ThisSection].pui8DestinationAddress 
												[((asImageLoadInfo[ui32ThisSection].ui32ShorterStride + 
												   asImageLoadInfo[ui32ThisSection].ui32RemainderInBufferPerLine) * 
												   ui32ThisLine)]);

				if ( fread (	pui8ThisDestinationAddress,
						 		1,
								ui32ShorterStride,
								psFileDescriptor->fFileHandle ) != ui32ShorterStride )
				{
					fclose ( psFileDescriptor->fFileHandle );
					printf ( "ERROR in 'ReadImageFromIMGFile': Error reading from file\n" );
					return IMGLIB_UTILS_ERROR__FILE_ERROR;
				}
				else
				{
					/* Skip any remaining data for this line in the file */
					if (( asImageLoadInfo[ui32ThisSection].ui32RemainderInFilePerLine) > 0)
					{
						/* Skip extra bytes in file */
						if ( fseek (	psFileDescriptor->fFileHandle,
										asImageLoadInfo[ui32ThisSection].ui32RemainderInFilePerLine,
										SEEK_CUR	) != 0 )
						{
							fclose ( psFileDescriptor->fFileHandle );
							printf ( "ERROR in 'ReadImageFromIMGFile': Error skipping data in file\n" );
							return IMGLIB_UTILS_ERROR__FILE_ERROR;
						}						
					}
				}
			}
		}

		return IMG_SUCCESS;
	}
#endif
