/*! 
******************************************************************************
 @file   : img_utils.h

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
         This module contains the header information for the misc
		 imglib utils source module.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
 *****************************************************************************/

#include <img_types.h>
#include <img_defs.h>

#if !defined (__IMG_UTILS_H__)
#define __IMG_UTILS_H__

#if defined(__cplusplus)
extern "C" {
#endif

#if defined IMGLIB_UTILS_INCLUDE_IMG_FILE_FUNCTIONS
#include "pixel_api.h"
#endif

/* Definitions */
#define	IMGLIB_UTILS_ERROR_BASE							0x80000000
#define	IMGLIB_UTILS_ERROR__END_OF_STRING				(IMGLIB_UTILS_ERROR_BASE + 1)
#define IMGLIB_UTILS_ERROR__FILE_ERROR					(IMGLIB_UTILS_ERROR_BASE + 2)
#define	IMGLIB_UTILS_MAX_FILENAME_LENGTH				500

/* RandGen parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */
#define RANDGEN_MAX 0xffffffff	/* maximum value that can be returned by the RandGen function. */

/* Typedefs */
typedef struct tag_ID_Name_Pair
{
	signed long		ID;
	char *			Name;

} ID_Name_Pair;

typedef IMG_RESULT (* fn_TokenParsingFunction) (char * pacThisToken, IMG_UINT32 ui32TokenNumber, IMG_BOOL bEndOfString);

#if defined IMGLIB_UTILS_INCLUDE_IMG_FILE_FUNCTIONS
	typedef struct tag_IMG_sImageFileHeaderExtended
	{
		char					acFileName [IMGLIB_UTILS_MAX_FILENAME_LENGTH];
		FILE *					fFileHandle; 
		IMG_UINT32				ui32BytesPerImageInFile;
		IMG_UINT32				ui32NoOfImages;
		IMG_UINT32				ui32YBytes;
		IMG_UINT32				ui32UVBytes;
		IMG_UINT32				ui32AlphaBytes;
		const PIXEL_sInfo *		psPixelInfo;

		IMG_sImageFileHeader	sIMGFileHeader;

	} IMG_sImageFileHeaderExtended;
#endif

/* Variables for SeedRandGen(), RandGen() */
extern unsigned long    mt[N]; // the array for the state vector  
extern int              mti;
extern unsigned long    mag01[2];

/* Macros */
#define IMGLIB_UTILS_GENERATE_ID_NAME_PAIR(token) {	token,		#token	},

/* Function prototypes */
void *		GetThingFromID								(	signed long						IDToFind,
															void *							pTable,
															unsigned long					EntrySize,
															unsigned long					TableLengthInBytes			);
char *		GetNameFromID								(	signed long						IDToFind,
															void *							pTable,
															unsigned long					EntrySize,
															unsigned long					TableLengthInBytes			);
ID_Name_Pair *	GetThingFromName						(	char *							pacNameToFind,
															ID_Name_Pair *					pasTable,
															unsigned long					TableLengthInBytes			);
void *		GetThingFromName2							(	char *							pacNameToFind,
															void *							pTable,
															unsigned long					EntrySize,
															unsigned long					TableLengthInBytes			);

unsigned char * FindFirstCharacterNotInSet				(	IMG_CHAR *						pcStringToCheck, 
															IMG_CHAR *						pcTestSet					);

IMG_RESULT	ParseStringOfTokens							(	IMG_CHAR *						pacString,
															IMG_CHAR *						pcCommentCharacter,
															fn_TokenParsingFunction			pfnTokenParsingFunction		);
IMG_VOID	SeedRandGen									(	IMG_UINT32						ui32Seed					);
IMG_UINT32	RandGen										(	IMG_VOID													);
IMG_UINT32	NextHighestPowerOfTwo						(	IMG_UINT32						k							);
IMG_UINT32	WhatPowerOfTwoIsThis						(	IMG_UINT32						k							);

unsigned short BootCRCX25( unsigned char *pData, unsigned int Size );

#if defined IMGLIB_UTILS_INCLUDE_IMG_FILE_FUNCTIONS
	IMG_RESULT	OpenIMGFile								(	IMG_sImageFileHeaderExtended *	psFileInfo					);
	IMG_RESULT	ReadImageFromIMGFile					(	IMG_sImageFileHeaderExtended *	psFileDescriptor,
															IMG_sImageBufCB *				psTargetBuffer,
															IMG_UINT32						ui32ImageIndexToLoad		);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* __IMG_UTILS_H__	*/


