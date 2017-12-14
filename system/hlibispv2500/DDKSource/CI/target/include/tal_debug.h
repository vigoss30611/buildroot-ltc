/*!
******************************************************************************
 @file   : tal_debug.h

 @brief	API for the Target Abstraction Layer Device Debug.

 @Author Tom Hollis

 @date   19/08/2010

         <b>Copyright 2010 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

@Description
         Target Abstraction Layer (TAL) Device Debug Header File.

 @Platform
	     Platform Independent


******************************************************************************/
/*
******************************************************************************
 *****************************************************************************/

#if !defined (__TALDEBUG_H__)
#define __TALDEBUG_H__

#include "tal_defs.h"
#include "img_types.h"
#include "img_defs.h"

#if defined (__cplusplus)
extern "C" {
#endif

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMG_TAL Target Abstraction Layer
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
 /**
 * @name Debug functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Debug group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TALDEBUG_DevComment

 @Description		This function is used to output a comment to the device.

 @Input		hMemSpace		: A memory space on the device.
 @Input		psCommentText	: A pointer to a string of text to be output to the
								device.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALDEBUG_DevComment(
    IMG_HANDLE                      hMemSpace,
    IMG_CONST IMG_CHAR *                psCommentText
);

/*!
******************************************************************************

 @Function				TALDEBUG_DevPrint

 @Description	This function is used to print a string on the device.

 @Input		hMemSpace		: A memory space on the device.
 @Input		pszPrintText	: A pointer to a string of text to be printed by the
								device.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALDEBUG_DevPrint(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pszPrintText
);

/*!
******************************************************************************

 @Function				TALDEBUG_DevPrintf

 @Description	This function is used to print a formatted string on the device.

 @Input		hMemSpace		: A memory space on the device.
 @Input		pszPrintText	: A pointer to a string of formatted text to be
								printed by the device.
 @Input		...				: Variables which fill formatted text (like printf).
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALDEBUG_DevPrintf(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pszPrintText,
	...
);


/*!
******************************************************************************

 @Function              TALDEBUG_GetDevTime

 @Description	This function is used to get the elapsed time on the device.

 @Input		hMemSpace		: A memory space on the device.
 @Input		pui64Time		: A pointer to an integer to hold the time
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALDEBUG_GetDevTime(
	IMG_HANDLE                      hMemSpace,
    IMG_UINT64 *                    pui64Time
);

/*!
******************************************************************************

 @Function				TALDEBUG_AutoIdle

 @Description	This function is used to wait for a number of clock cycles.

 @Input		hMemSpace	: A memory space on the device.
 @Input		ui32Time	: The time to wait, in clock cycles, if no command
							is waiting.
 @Return	IMG_RESULT	: IMG_SUCCESS if the memory space is supported by
							the target. Asserts are use to trap unsupported
							memory spaces.

******************************************************************************/
IMG_RESULT TALDEBUG_AutoIdle(
	IMG_HANDLE                      hMemSpace,
	IMG_UINT32						ui32Time
);


/*!
******************************************************************************

@Function              TALDEBUG_GetDevMemAddress

@Description

Device memory is allocated using TAL_MallocDeviceMem() which provides an IMG_HANDLE to refer
to the memory block when using other TAL functions.  The IMG_HANDLE is deliberately opaque and
the memory addresses being used are not accessible.  That is the way the system is
designed to work.  However, there are rare occasions when the real address of the device
memory is required.  This function exists to fulfill that need.

The address returned by this function should not be written to a hardware register.  Use
TALMEM_WriteDevMemRef32() instead.  The address returned by this function should not be
written to another device memory region either.  Use TALMEM_WriteDevMemRef32()
instead.  The only time this function should be used is when the address obtained is used
with something like memcpy() when simulating the real hardware.

 @Input		hDeviceMem	: A handle to the device memory.

******************************************************************************/
IMG_UINT64 TALDEBUG_GetDevMemAddress(
    IMG_HANDLE      hDeviceMem);

/*!
******************************************************************************

 @Function              TALDEBUG_GetHostMemAddress

 @Description	This function returns the Host address of a TAL handle.

 @Input		hDeviceMem		:	A TAL Memory handle.
 @Return	IMG_VOID*		:	A pointer to host memory.

******************************************************************************/
IMG_VOID * TALDEBUG_GetHostMemAddress( IMG_HANDLE		hDeviceMem );


/*!
******************************************************************************

 @Function				TALDEBUG_ReadMem

 @Description	This function is used to directly read target memory and should only be used
 for debug purposes.

 @Input		ui64MemoryAddress	: The address of the memory to be read.
 @Input		ui32Size			: The size of the memory to be read.
 @Modified	pui8Buffer			: A pointer to the buffer into which the memory is to be read.
 @Return	IMG_INT32	: Returns IMG_SUCCESS.

******************************************************************************/
IMG_RESULT TALDEBUG_ReadMem(
	IMG_UINT64				ui64MemoryAddress,
	IMG_UINT32				ui32Size,
	IMG_PUINT8				pui8Buffer
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Debug group
 *------------------------------------------------------------------------*/
 /**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif 

#if defined (__cplusplus)
}
#endif

#endif //__TALDEBUG_H__

