/**
 * @file gzip_specialio.c
 *
 * @brief FileIO implementation of special functions (using basic io ones)
 *
 * <b>Copyright 2003 by Imagination Technologies Limited.</b>\n
 * All rights reserved.  No part of this software, either
 * material or conceptual may be copied or distributed,
 * transmitted, transcribed, stored in a retrieval system
 * or translated into any human or computer language in any
 * form by any means, electronic, mechanical, manual or
 * other-wise, or disclosed to third parties without the
 * express written permission of Imagination Technologies
 * Limited, Unit 8, HomePark Industrial Estate,
 * King's Langley, Hertfordshire, WD4 8LZ, U.K.
 *
 *
 */

#include "gzip_fileio.h"

#include <img_defs.h>
#include <img_errors.h> 

#include <stdarg.h> // for va_args

IMG_RESULT IMG_FILEIO_WriteWordToFile(IMG_HANDLE hFile, IMG_UINT32 ui32Word)
{
	IMG_UINT8 ui8Byte[4];

	ui8Byte[3] = (IMG_UINT8)((ui32Word >> 24) & 0xFF);
	ui8Byte[2] = (IMG_UINT8)((ui32Word >> 16) & 0xFF);
	ui8Byte[1] = (IMG_UINT8)((ui32Word >> 8) & 0xFF);
	ui8Byte[0] = (IMG_UINT8)(ui32Word & 0xFF);

	return IMG_FILEIO_WriteToFile(hFile, (&ui8Byte[0]), 4);
}

IMG_RESULT IMG_FILEIO_Write64BitWordToFile(IMG_HANDLE hFile, IMG_UINT64 ui64Word)
{
	IMG_RESULT rResult = IMG_FILEIO_WriteWordToFile(hFile, IMG_UINT64_TO_UINT32(ui64Word & 0xFFFFFFFF));
	if (rResult != IMG_SUCCESS)
		return rResult;

	rResult = IMG_FILEIO_WriteWordToFile(hFile, IMG_UINT64_TO_UINT32(ui64Word >> 32));
	return rResult;
}

IMG_RESULT IMG_FILEIO_ReadWordFromFile(IMG_HANDLE hFile, IMG_UINT32 *pui32Word, IMG_BOOL *pbDataRead)
{
	IMG_UINT8 ui8Byte[4];
	IMG_RESULT ret;
	IMG_UINT32 ui32Read;

	if ( pui32Word == NULL || pbDataRead == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	ret = IMG_FILEIO_ReadFromFile(hFile, 4, (&ui8Byte[0]), &ui32Read);
	
	if (ui32Read==4)
	{
		*pbDataRead = IMG_TRUE;
		*pui32Word = (ui8Byte[3] << 24) | (ui8Byte[2] << 16) | (ui8Byte[1] << 8) | ui8Byte[0];
	}

	return ret;
}

IMG_RESULT IMG_FILEIO_Read64BitWordFromFile(IMG_HANDLE hFile, IMG_UINT64 *pui64Word, IMG_BOOL *pbDataRead)
{
	IMG_RESULT ret;
	IMG_UINT32 msb, lsb;

	if ( (ret = IMG_FILEIO_ReadWordFromFile(hFile, &msb, pbDataRead)) != IMG_SUCCESS )
	{
		return ret;
	}

	if ( (ret = IMG_FILEIO_ReadWordFromFile(hFile, &lsb, pbDataRead)) != IMG_SUCCESS )
	{
		return ret;
	}

	*pui64Word = msb;
	*pui64Word = (*pui64Word)<<32 | lsb;

	return IMG_SUCCESS;	
}
