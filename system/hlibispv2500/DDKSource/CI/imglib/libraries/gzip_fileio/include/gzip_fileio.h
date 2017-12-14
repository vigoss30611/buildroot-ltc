/*! 
******************************************************************************
 @file   gzip_fileio.h

 @brief  GZip and standard C file manipulation abstraction

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

  <b>Platform:</b>\n
	     Platform Independent 

 ******************************************************************************/

#include "img_types.h"

#include <stdio.h> // for whence value of fseek

#ifndef __GZIP_FILEIO_H__
#define __GZIP_FILEIO_H__

#if (__cplusplus)
extern "C" {
#endif

#define FILEIO_EXTERN extern

#ifdef DOXYGEN_CREATE_MODULES

/**
 * @defgroup IMGLIB_FILEIO File IO: standard C and GZIP IO abstraction

 The IMG GZIP File IO library is an abstraction of the standard C IO library (stdio) and the Zlib library.
 It provides a set of functions that are covering both libraries.

 When building with CMake you can configure the behaviour of the FILE IO library:
 @li by default it is going to look for the GZip library, and if found it is going to use it
 @li but you can force the library not to use it by setting IMGLIB_GZIPIO_FORCE_NO_ZLIB to true 

 The GZip library is not natively available on windows, but the win32 version is available on perforce at //powervr/swvideo/components/tools/external/zlib-1.2.6/

 For your CMake to be platform independent you need to considere the Linux case (when the ZLib is available on the system includes) and for example have a similar code in your top-level CMakeLists.txt:
 @verbatim
 project(toplevel) # to have the top-level source path without using CMAKE_SOURCE_PATH or CMAKE_CURRENT_SOURCE_PATH

 if (NOT WIN32)
    option(USE_LOCAL_ZLIB "Use the local ZLib instead of the system one" ON)
 else()
    set(USE_LOCAL_ZLIB TRUE)
 endif()

 if (${USE_LOCAL_ZLIB})
    set (ZLIB_DIR ${toplevel_SOURCE_DIR}/tools/zlib-1.2.6) # to use local ZLib instead of system one when doing find_package()
	set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${toplevel_SOURCE_DIR}/tools/zlib-1.2.6)
 endif()

 # and later
 find_package(ZLIB QUIET) # quiet won't print error if not found - when this dependency is optional @endverbatim

 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMGLIB_FILEIO documentation module
 *------------------------------------------------------------------------*/

/**
 * @name Basic IO functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMGLIB_FILEIO basic IO module
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @brief To know if the ZLib is available (i.e. if compression is available)

 @return IMG_TRUE or IMG_FALSE (depending on compilation time choice)

******************************************************************************/
FILEIO_EXTERN IMG_BOOL IMG_FILEIO_UsesZLib(void);

/*!
******************************************************************************

 @brief This function sets the compression level to use when generating gzip files.
 
 @Input ui32CompressionLevel Compression level between 1 and 9.
 
 @return IMG_SUCCESS
 @return IMG_ERROR_INVALID_PARAMETERS if the compression level is not valid.

 @see IMG_FILEIO_UsesZLib()
 
******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_SetCompressionLevel(
	IMG_UINT32						ui32CompressionLevel
);

/*!
******************************************************************************

 @brief This function is used to open a file.
 
 @Input psCaptureFileName A pointer to the file/path.
 @Input psOpenMode Open mode.
 @Output phFile A pointer used to return the file handle. This is only modified if returned with success.
 @warning phFile is modified regardless of its value
 @Input bCompression IMG_TRUE to open compressed/zipped file.

 Open modes are (from fopen manual - verify behaviour for the version you currently use):
 @li "r" - open for reading (at begining of file). If file does not exists it fails.
 @li "w" - truncate file to zero length or create text file for writing (at begining of file).
 @li "a" - open for appending (writing at end of file). If file does not exists it is created.

 All open mode implying reading and writing are not supported by GZip:
 @li "r+" - open for reading and writing (at begining of file). If file does not exists it fails.
 @li "w+" - open for reading and writing (at begining of file). The file is created if it does not exist, otherwise it is truncated.
 @li "a+" - open for reading and appending (reading at begining of file, writing ALWAYS at end of file). The file is created if it does not exists.

 Extended modes (from Windows) should be supported:
 @li "b" added to a mode to open the file in binary (untranslated) mode.

 @return IMG_SUCCESS
 @return IMG_ERROR_INVALID_PARAMETERS if the input parameters are wrong (NULL pointers)
 @return IMG_ERROR_MEMORY_IN_USE if *phFile is not NULL
 @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if the file cannot be open

 @see IMG_FILEIO_ReopenFile() IMG_FILEIO_CloseFile()

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_OpenFile(
	const IMG_CHAR *					psCaptureFileName,
	const IMG_CHAR *					psOpenMode,
	IMG_HANDLE *						phFile,
	IMG_BOOL							bCompression
);

/*!
******************************************************************************

 @brief Reopen a file with different open modes. Latest compression value is used.

 Close the file and call open with the new parameters.

 Fails if the file is not already open.

 @param[in,out] phFile a pointer to close and reopen
 @Input pszFilename filename of the file to open for this descriptor
 @Input psOpenMode open mode string

 @return IMG_SUCCESS
 @return IMG_ERROR_INVALID_PARAMETERS if one of the parameters is NULL
 @return error code from delegate function
 
 @see Delegates to IMG_FILEIO_OpenFile() and IMG_FILEIO_CloseFile()

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_ReopenFile(
	IMG_HANDLE *						phFile,
	const IMG_CHAR *					pszFilename,
	const IMG_CHAR *					psOpenMode
);

/*!
******************************************************************************

 @brief This function is used to close a file.
 
 @Input hFile The file handle.

 @return IMG_SUCCESS
 @return IMG_ERROR_INVALID_PARAMETERS if the file handle is not valid
 @return IMG_ERROR_FATAL if couldn't close the file handle

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_CloseFile(
	IMG_HANDLE						hFile
);

/*!
******************************************************************************

 @brief This function is used to write to a file.
 
 @Input hFile The file handle.
 @Input psBuffer A pointer to the data to be written.
 @Input ui32Size The size of the data to be written (in bytes) - not size_t or i64 because Zlib only supports 32b

 @return IMG_SUCCESS
 @return IMG_ERROR_FATAL if failed to write the correct number of bytes
 @return IMG_ERROR_INVALID_PARAMETERS if one of the parameters is NULL or ui32Size is 0

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_WriteToFile(
	IMG_HANDLE						hFile,
	const IMG_VOID *				psBuffer,
	IMG_UINT32						ui32Size
);

/*!
******************************************************************************

 @brief This function is used to read from a file.
  
 @Input hFile The file handle.
 @Input ui32Size The size of the data to be read (in bytes) - not size_t nor i64 because Zlib doesn't support it
 @Output psBuffer A pointer to a buffer into which the data is to be read.
 @Output pBytesRead The number of bytes read.

 @return IMG_SUCCESS
 @return IMG_ERROR_FATAL if couldn't read enough data
 @return IMG_ERROR_INVALID_PARAMETERS if one of its output pointers is NULL or if ui32Size is 0

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_ReadFromFile(
	IMG_HANDLE						hFile,
	IMG_UINT32						ui32Size,
	IMG_VOID *						psBuffer,
	IMG_UINT32 *					pBytesRead
);

/*!
******************************************************************************

 @brief This function is used to seek to a location in a file.
 
 @Input hFile The file handle.
 @Input ui64FileOffset The file offset to seek to
 @Input whence set to SEEK_SET, SEEK_CUR, or SEEK_END, the offset is relative to the start of the file, the current position indicator, or end-of-file, respectively.
 @note SEEK_END is not supported by Zlib for gzseek (it will assert)

 @return IMG_SUCCESS
 @return IMG_ERROR_FATAL if the seek failed
 @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if temporary files cannot be open

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_SeekFile(
	IMG_HANDLE						hFile,
	IMG_UINT64						ui64FileOffset,
	IMG_INT							whence
);

/*!
******************************************************************************

 @brief This function is used to get the starting position for the next read or write operation
 
 @Input hFile The file handle.
 @Output pCurrFilePos The position of the file

 @return IMG_SUCCESS
 @return IMG_ERROR_FATAL if failed to find position in file
 @return IMG_ERROR_INVALID_PARAMETERS if parameters are NULL

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_Tell(
	IMG_HANDLE						hFile,
	IMG_UINT64 *					pCurrFilePos
);

/*!
******************************************************************************

 @brief This function is used to check whether the End-of-File indicator associated with stream is set
 
 @Input hFile The file handle.

 @return IMG_TRUE if EOF is reached, IMG_FALSE otherwise (returns IMG_TRUE if hFile is invalid).

******************************************************************************/
FILEIO_EXTERN IMG_BOOL IMG_FILEIO_Eof(
	IMG_HANDLE						hFile
);

/*!
******************************************************************************

 @brief This function checks if the error indicator associated with the file is set
 
 @Input	hFile The file handle.

 @return IMG_TRUE If there is an error, IMG_FALSE otherwise (returns IMG_TRUE if hFile is invalid).

******************************************************************************/
FILEIO_EXTERN IMG_BOOL IMG_FILEIO_Error(
	IMG_HANDLE						hFile
);

/*!
******************************************************************************

 @brief This function is used to flush a file.
 
 @Input hFile The file handle.

 @note If Zlib is used the gzflush parameter is Z_FULL_FLUSH

 @return IMG_SUCCESS
 @return IMG_ERROR_INVALID_PARAMETERS if the file handle is not valid
 @return IMG_FATAL_ERROR if couldn't flush the file handle

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_Flush(
	IMG_HANDLE						hFile
);

/*!
******************************************************************************

 @brief This function is used to read from a file, until a number of bytes are read, or a newline is read, or an end-of-file contidition is encountered.
 
 @Input hFile The file handle.
 @Input ui32Size The size of the data to be read (in bytes) - not size_t because zlib does not support it
 @Output pBuffer A pointer to a buffer into which the data is to be read.
 
 @return IMG_TRUE or IMG_FALSE (if the stream is at end-of-file).

******************************************************************************/
FILEIO_EXTERN IMG_BOOL IMG_FILEIO_Gets(
	IMG_HANDLE						hFile,
	IMG_UINT32						ui32Size,
	IMG_CHAR *						pBuffer
);

/*!
******************************************************************************

 @brief This function returns the character currently pointed by the stream.
 
 @Input hFile The file handle.

 @return This function returns the IMG_CHAR currently pointed.

******************************************************************************/
FILEIO_EXTERN IMG_INT32 IMG_FILEIO_Getc(
	IMG_HANDLE						hFile
);

/*!
******************************************************************************

 @brief This function writes a character to the stream and advances the position indicator
 
 @Input hFile The file handle.
 @Input uCharacter unsigned char casted to int

 @return On success, this function returns the IMG_CHAR written, otherwise -1.

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_Putc(
	IMG_HANDLE						hFile,
	IMG_INT						uCharacter
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of IMGLIB_FILEIO basic IO module
 *------------------------------------------------------------------------*/

/**
 * @name Special IO functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMGLIB_FILEIO special IO module
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @brief This function is used to write a "32-bit word" to a file.
 
 @Input hFile The file handle.
 @Input ui32Word The "word" value to be written.

 @return deletages to IMG_FILEIO_WriteToFile()

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_WriteWordToFile(
	IMG_HANDLE						hFile,
	IMG_UINT32						ui32Word
);

/*!
******************************************************************************

 @brief This function is used to write a "64-bit word" to a file.
 
 @Input hFile The file handle.
 @Input ui64Word The "word" value to be written.

 @return delegates to IMG_FILEIO_WriteToFile()

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_Write64BitWordToFile(
	IMG_HANDLE						hFile,
	IMG_UINT64						ui64Word
);

/*!
******************************************************************************

 @brief This function is used to read a "32-bit word" from a file.
 
 @Input hFile The file handle.
 @Output pui32Word A pointer used to return the "word" value read.
 @Output pbDataRead IMG_TRUE if a word was read. Otherwise IMG_FALSE.

 @return delegates to IMG_FILEIO_ReadFromFile()
 @return IMG_ERROR_INVALID_PARAMETERS if one of the output pointer is NULL

******************************************************************************/
FILEIO_EXTERN IMG_RESULT IMG_FILEIO_ReadWordFromFile(
	IMG_HANDLE						hFile,
	IMG_UINT32 *					pui32Word,
	IMG_BOOL *						pbDataRead
);

/*!
******************************************************************************

 @brief This function is used to read a "64-bit word" from a file.
 
 @Input hFile The file handle.
 @Output pui64Word A pointer used to return the "word" value read.
 @Output pbDataRead IMG_TRUE if a word was read. Otherwise IMG_FALSE.

 @return delegates to IMG_FILEIO_ReadWordFromFile()
 @return IMG_ERROR_INVALID_PARAMETERS if one of the output pointer is NULL

******************************************************************************/
IMG_RESULT IMG_FILEIO_Read64BitWordFromFile(
	IMG_HANDLE hFile,
	IMG_UINT64 *pui64Word,
	IMG_BOOL *pbDataRead
);

#undef FILEIO_EXTERN

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of IMGLIB_FILEIO special IO module
 *------------------------------------------------------------------------*/

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMGLIB_FILEIO documentation module
 *------------------------------------------------------------------------*/
#endif 

#if (__cplusplus)
}
#endif
 
#endif /* __GZIP_FILEIO_H__	*/
