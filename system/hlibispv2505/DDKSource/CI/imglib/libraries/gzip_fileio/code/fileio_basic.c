/**
 * @file fileio_basic.c
 *
 * @brief FileIO basic functions implementation.
 * Uses standard C library or GZip Library (depending on FILEIO_USEZLIB flag)
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

#include <stdio.h> // seek and standard functions

#ifdef FILEIO_USEZLIB

#ifdef __APPLE__
#define off64_t off_t
#endif

#include <zlib.h>

#endif // FILEIO_USEZLIB

#ifdef DOXYGEN_CREATE_MODULES

/**
 * @defgroup IMGLIB_FILEIO File IO: standard C and GZIP IO abstraction
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMGLIB_FILEIO documentation module
 *------------------------------------------------------------------------*/

/**
 * @name Internal definitions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMGLIB_FILEIO internal module
 *------------------------------------------------------------------------*/
#endif

#ifdef IMG_FILEIO_DEBUG
#include <varargs.h>

#define IMG_FILEIO_Debug(...) { char message[128]; sprintf(message, __VA_ARGS__); printf("%s:%d %s\n", __FUNCTION__, __LINE__, message); }
#else
#define IMG_FILEIO_Debug(...)
#endif 

static IMG_UINT32 gui32Compression = 6; ///< @brief GZip compression level from 1 to 9

/**
 * @brief Anonymous structure used so that direct calls to std library canno be made
 *
 * 
 */
struct IMG_FILEIO
{
#ifdef FILEIO_USEZLIB
	gzFile gzFile;				///< @brief Used when compression is > 0 otherwise text is written compressed
	IMG_UINT32 ui32Compression; ///< @brief Used only when gzip is available. Value of 0 means no compression used.
#endif

	FILE *pFile; ///< @brief Used when writing non-compressed file
};
 
#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of IMGLIB_FILEIO internal module
 *------------------------------------------------------------------------*/

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of IMGLIB_FILEIO documentation module
 *------------------------------------------------------------------------*/
#endif  // DOXYGEN_CREATE_MODULES 

/*
 * Basic IO functions
 */ 
 
IMG_BOOL IMG_FILEIO_UsesZLib(void)
{
#ifdef FILEIO_USEZLIB
	return IMG_TRUE;
#else
	return IMG_FALSE;
#endif
}

IMG_RESULT IMG_FILEIO_SetCompressionLevel(IMG_UINT32 ui32CompressionLevel)
{
	if ( ui32CompressionLevel < 1 || ui32CompressionLevel > 9 )
	{
		IMG_FILEIO_Debug("Compression level %d too high", ui32CompressionLevel);
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	gui32Compression = ui32CompressionLevel;
	return IMG_SUCCESS;
}

static IMG_RESULT IMG_FileIO_Open(struct IMG_FILEIO *pFile, const IMG_CHAR *pszFileName, const IMG_CHAR *pszMode, IMG_BOOL bCompression)
{
	int err;

#ifdef FILEIO_USEZLIB
	pFile->gzFile = Z_NULL;

	if ( bCompression == IMG_TRUE )
	{
		const char *errMess = NULL;
		char compressedOpenMode[16];
		IMG_SIZE len;

		// when compressing we have to create an open mode string

		if ( (len=strlen(pszMode)) > 14 ) // 2 digits needed, 1 for open mode format max (1 to 9), 1 for \0
		{
			(void)errMess;
			IMG_FILEIO_Debug("Open mode is longer than 14 digits...");
			IMG_FREE(pFile);
			return IMG_ERROR_INVALID_PARAMETERS;
		}

#ifdef WIN32
		sprintf(compressedOpenMode, "%s%d", pszMode, gui32Compression);
#else
		snprintf(compressedOpenMode, 16, "%s%d", pszMode, gui32Compression);
#endif

		pFile->ui32Compression = gui32Compression;
		pFile->gzFile = gzopen(pszFileName, compressedOpenMode);

		if ( pFile->gzFile == Z_NULL )
		{
			IMG_FREE(pFile);
			return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
		}

		errMess = gzerror(pFile->gzFile, &err);

		if ( err != 0 )
		{
			IMG_FILEIO_Debug("gzerror returned %d: %s", err, errMess);
			gzclose(pFile->gzFile);
			IMG_FREE(pFile);
			return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
		}
	}
	else
#endif // FILEIO_USEZLIB
	{
		pFile->pFile = fopen(pszFileName, pszMode);
	
		if ( pFile->pFile == NULL )
		{
			IMG_FREE(pFile);
			return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
		}

		if ( (err=ferror(pFile->pFile)) != 0 )
		{
			IMG_FILEIO_Debug("ferror returned %d\n", err);
			fclose(pFile->pFile);
			IMG_FREE(pFile);
			return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
		}
	}

	return IMG_SUCCESS;
}

static IMG_RESULT IMG_FileIO_Close(struct IMG_FILEIO *pFile)
{
	int err;
#ifdef FILEIO_USEZLIB
	
	if ( pFile->ui32Compression > 0 )
	{
		// closing the file flushes it
		if ( (err=gzclose(pFile->gzFile)) != Z_OK )
		{
			IMG_FILEIO_Debug("gzClose failed returning %d (Z_STREAM_ERROR=%d,Z_ERRNO=%d,Z_BUF_ERROR=%d)", err, Z_STREAM_ERROR, Z_ERRNO, Z_BUF_ERROR);
			return IMG_ERROR_FATAL;
		}
		pFile->gzFile = Z_NULL;
	}
	else
#endif // FILEIO_USEZLIB
	{
		if ( (err=fclose(pFile->pFile)) != 0 )
		{
			IMG_FILEIO_Debug("fclose failed returning %d (Z_STREAM_ERROR=%d,Z_ERRNO=%d,Z_BUF_ERROR=%d)", err, Z_STREAM_ERROR, Z_ERRNO, Z_BUF_ERROR);
			return IMG_ERROR_FATAL;
		}
		pFile->pFile = NULL;
	}

	return IMG_SUCCESS;
}

IMG_RESULT IMG_FILEIO_OpenFile(const IMG_CHAR *pszFileName, const IMG_CHAR *pszMode, IMG_HANDLE *phFile, IMG_BOOL bCompression)
{
	struct IMG_FILEIO *pFile = NULL;
	IMG_RESULT ret;

	if ( pszFileName == NULL || pszMode == NULL || phFile == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	// this is not enforced because most of the legacy code
	if ( *phFile != NULL )
	{
		//IMG_FILEIO_Debug("*phFile is not NULL");
		//return IMG_ERROR_MEMORY_IN_USE;
		fprintf(stderr, "WARNING %s: given phFile handle is not initialised to NULL\n", __FUNCTION__);
	}

#ifdef FILEIO_USEZLIB
	// prints a warning that gzip will not support that
	if (  bCompression == IMG_TRUE && strlen(pszMode) > 1 )
	{
		if ( pszMode[1] == '+' )
		{
			fprintf(stderr, "WARNING %s: given mode '%s' is not supported by gzip (gzip does not support read AND write)\n", __FUNCTION__, pszMode);
			// this will certainly fail at gzopen
		}
	}
#else // FILEIO_USEZLIB
	if ( bCompression == IMG_TRUE )
	{
		fprintf(stderr, "WARNING %s: using compressed option without gzip has no effect\n", __FUNCTION__);
	}
#endif // FILEIO_USEZLIB

	IMG_FILEIO_Debug("(%s, %s, %d)", psCaptureFileName, psOpenMode, bCompression);

	pFile = (struct IMG_FILEIO*)IMG_CALLOC(1, sizeof(struct IMG_FILEIO));
	if ( pFile == NULL )
	{
		IMG_FILEIO_Debug("failed to allocate %" IMG_SIZEPR "u bytes for file descriptor", sizeof(struct IMG_FILEIO));
		return IMG_ERROR_MALLOC_FAILED;
	}
	
	if ( (ret=IMG_FileIO_Open(pFile, pszFileName, pszMode, bCompression)) == IMG_SUCCESS )
	{
		*phFile = (IMG_HANDLE)pFile;
	}
	
	return ret;
}

/// @ we could change that to use freopen or gzclose and gzopen whitout re-allocating the struct IMG_FILEIO associated with the file
IMG_RESULT IMG_FILEIO_ReopenFile(IMG_HANDLE *phFile, const IMG_CHAR *pszFilename, const IMG_CHAR *pszMode)
{
	struct IMG_FILEIO *pFile = NULL;
	IMG_BOOL bCompressed = IMG_FALSE;
	IMG_RESULT ret;
	
	if ( phFile == NULL || pszFilename == NULL || pszMode == NULL || *phFile == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	
	pFile = (struct IMG_FILEIO*)(*phFile);

#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		bCompressed = IMG_TRUE;
	}
#endif // FILEIO_USEZLIB
	
	if ( (ret=IMG_FileIO_Close(pFile)) != IMG_SUCCESS )
	{
		IMG_FILEIO_Debug("failed to close");
		return ret;
	}
	if ( (ret=IMG_FileIO_Open(pFile, pszFilename, pszMode, bCompressed)) != IMG_SUCCESS )
	{
		IMG_FILEIO_Debug("failed to reopen %s (%s%s)", pszFilename, pszMode, bCompressed==IMG_TRUE? ",compressed","");
	}
	
	return ret;
}

IMG_RESULT IMG_FILEIO_CloseFile(IMG_HANDLE	hFile)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	IMG_RESULT ret;
	
	if ( hFile == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	
	if ( (ret = IMG_FileIO_Close(pFile)) == IMG_SUCCESS )
	{
		IMG_FREE(pFile);
	}
	
	return ret;
}

/// @warning Does NOT flush anymore according to the value of NO_PDUMP_FLUSH
IMG_RESULT IMG_FILEIO_WriteToFile(IMG_HANDLE hFile, const IMG_VOID *pBuffer, IMG_UINT32 ui32Size)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	IMG_SIZE returned = 0;
	
	if ( hFile == NULL || pBuffer == NULL || ui32Size == 0 )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	
#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		if ( (returned = gzwrite(pFile->gzFile, pBuffer, ui32Size)) != ui32Size )
		{
			IMG_FILEIO_Debug("gzwrite failed returning %" IMG_SIZEPR "d", returned);
			return IMG_ERROR_FATAL;
		}
	}
	else
#endif // FILEIO_USEZLIB
	{
		if ( (returned = fwrite(pBuffer, 1, ui32Size, pFile->pFile)) != ui32Size )
		{
			IMG_FILEIO_Debug("fwrite failed returning %" IMG_SIZEPR "d", returned);
			return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}

IMG_RESULT IMG_FILEIO_ReadFromFile(IMG_HANDLE hFile, IMG_UINT32 ui32Size, IMG_VOID *pBuffer, IMG_UINT32 *pBytesRead)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	
	if ( hFile == NULL || pBuffer == NULL || ui32Size == 0 || pBytesRead == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	
#ifdef FILEIO_USEZLIB
	
	if ( pFile->ui32Compression > 0 )
	{
		*pBytesRead = gzread(pFile->gzFile, pBuffer, ui32Size);
	}
	else
#endif // FILEIO_USEZLIB
	{
		*pBytesRead = (IMG_UINT32)fread(pBuffer, 1, ui32Size, pFile->pFile);
	}

	if ( *pBytesRead == ui32Size )
	{
		return IMG_SUCCESS;
	}
	return IMG_ERROR_FATAL;
}

IMG_RESULT IMG_FILEIO_SeekFile(IMG_HANDLE hFile, IMG_UINT64 ui64FileOffset, IMG_INT whence)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	
	if ( hFile == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	
#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		IMG_ASSERT(whence!=SEEK_END); 
		// in case assert is not exiting the seek will fail
	
		if ( gzseek(pFile->gzFile, (long)ui64FileOffset, whence) == -1 )
		{
			IMG_FILEIO_Debug("gzseek failed");
			return IMG_ERROR_FATAL;
		}
	}
	else
#endif // FILEIO_USEZLIB
	{
		int ret;
		
#ifdef NO_FSEEK64
		ret = fseek(pFile->pFile, IMG_UINT64_TO_UINT32(ui64FileOffset), whence);
#else
		ret = IMG_FSEEK64(pFile->pFile, ui64FileOffset, whence);
#endif
	
		if ( ret != 0 )
		{
			IMG_FILEIO_Debug("fseek failed returning %d", ret);
			return IMG_ERROR_FATAL;
		}
	}
	
	return IMG_SUCCESS;
}

IMG_RESULT IMG_FILEIO_Tell(IMG_HANDLE hFile, IMG_UINT64 *pCurrentPos)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	
	if ( hFile == NULL || pCurrentPos == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	
#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		z_off_t ret;

		if ( (ret = gztell(pFile->gzFile)) == -1 )
		{
			IMG_FILEIO_Debug("gztell failed");
			return IMG_ERROR_FATAL;
		}
		
		*pCurrentPos = (IMG_UINT64)ret;
	}
	else
#endif // FILEIO_USEZLIB
	{
		long ret;
	
#ifdef IMG_NO_FTELL64
		ret = ftell(pFile->pFile);
#else
		ret = (long)IMG_FTELL64(pFile->pFile);
#endif

		if ( ret == -1 )
		{
			IMG_FILEIO_Debug("ftell failed");
			return IMG_ERROR_FATAL;
		}
		
		*pCurrentPos = (IMG_UINT64)ret;
	}

	return IMG_SUCCESS;
}

IMG_BOOL IMG_FILEIO_Eof(IMG_HANDLE hFile)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	IMG_BOOL bRet = IMG_FALSE;
	
	if ( hFile == NULL )
	{
		IMG_FILEIO_Debug("invalid file!");
		return IMG_TRUE;
	}
	
#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		bRet = gzeof(pFile->gzFile); // gzeof returns 0 when not EOF and 1 when EOF
	}
	else
#endif // FILEIO_USEZLIB
	{
		if ( feof(pFile->pFile) ) // foef returns non 0 when reaching EOF
		{
			bRet = IMG_TRUE;
		}
	}

	return bRet;
}

IMG_BOOL IMG_FILEIO_Error(IMG_HANDLE hFile)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	IMG_BOOL bRet = IMG_FALSE;
	
	if ( hFile == NULL )
	{
		IMG_FILEIO_Debug("invalid file!");
		return IMG_TRUE;
	}
	
#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		int err;
		const char *errMess = NULL;
		
		errMess = gzerror(pFile->gzFile, &err);
		if ( err != 0 )
		{
			(void)errMess;
			IMG_FILEIO_Debug("gzerror returned %d:%s", err, errMess);
			bRet = IMG_TRUE;
		}
	}
	else
#endif // FILEIO_USEZLIB
	{
		if (ferror(pFile->pFile)) // ferror returns non zero if error bit is set
		{
			bRet = IMG_TRUE;
		}
	}
	
	return bRet;
}

IMG_RESULT IMG_FILEIO_Flush(IMG_HANDLE hFile)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	
	if ( hFile == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	
#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		if ( gzflush(pFile->gzFile, Z_FULL_FLUSH) != Z_OK )
		{
			return IMG_ERROR_FATAL;
		}
	}
	else
#endif // FILEIO_USEZLIB
	{
		if ( fflush(pFile->pFile) != 0 )
		{
			return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;	
}

IMG_BOOL IMG_FILEIO_Gets(IMG_HANDLE hFile, IMG_UINT32 ui32Size, IMG_CHAR *pBuffer)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;
	IMG_BOOL bRet = IMG_TRUE;
	
	if ( hFile == NULL )
	{
		IMG_FILEIO_Debug("invalid file!");
		return IMG_FALSE;
	}
	
#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		if ( gzgets(pFile->gzFile, pBuffer, ui32Size) == Z_NULL )
		{
			bRet = IMG_FALSE;
		}
	}
	else
#endif // FILEIO_USEZLIB
	{
		if ( fgets(pBuffer, ui32Size, pFile->pFile) == NULL )
		{
			bRet = IMG_FALSE;
		}
	}
	
	return bRet;
}

IMG_INT32 IMG_FILEIO_Getc(IMG_HANDLE hFile)
{
	IMG_INT32 ret;
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;

	if ( hFile == NULL )
	{
		IMG_FILEIO_Debug("invalid file!");
		return -1;
	}
	
#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		ret = gzgetc(pFile->gzFile);
	}
	else
#endif // FILEIO_USEZLIB
	{
		if ( (ret = fgetc(pFile->pFile)) == EOF )
		{
			ret = -1;
		}
	}

	return ret;
}

IMG_RESULT IMG_FILEIO_Putc(IMG_HANDLE hFile, IMG_INT uCharacter)
{
	struct IMG_FILEIO *pFile = (struct IMG_FILEIO*)hFile;

	if ( hFile == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

#ifdef FILEIO_USEZLIB
	if ( pFile->ui32Compression > 0 )
	{
		if ( gzputc(pFile->gzFile, uCharacter) == -1 )
		{
			return IMG_ERROR_FATAL;
		}
	}
	else
#endif // FILEIO_USEZLIB
	{
		if ( putc(uCharacter, pFile->pFile) == EOF )
		{
			return IMG_ERROR_FATAL;
		}
	}

	return IMG_SUCCESS;
}
