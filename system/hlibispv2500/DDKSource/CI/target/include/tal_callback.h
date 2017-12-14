/*!
******************************************************************************
 @file   : tal_callback.h

 @brief

 @Author Alejandro Arcos

 @date   07/09/2010

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
         TAL Callback functions Header File.

 <b>Platform:</b>\n
         Platform Independent

 @Version
         1.0

******************************************************************************/
/*
******************************************************************************
*/

#if !defined (__TAL_CALLBACK_H__)
#define __TAL_CALLBACK_H__

#include "cbman.h"
#include "lst.h"
#include <img_types.h>

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMG_TAL Target Abstraction Layer
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
 /**
 * @name Callback
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Callback group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 This type defines the event types for the TAL APIs

******************************************************************************/
typedef enum
{
	/*! Indicates that a register or memory location is about to be read.		*/
	TALCALLBACK_EVENT_PRE_READ_WORD,
	/*! Indicates that a register or memory location has been read.				*/
	TALCALLBACK_EVENT_POST_READ_WORD,
	/*! Indicates that a register or memory location is about to be written.	*/
	TALCALLBACK_EVENT_PRE_WRITE_WORD,
	/*! Indicates that a register or memory location has been written.			*/
	TALCALLBACK_EVENT_POST_WRITE_WORD,

} TALCALLBACK_eEvent;

#define TAL_eEvent TALCALLBACK_eEvent
#define TAL_EVENT_PRE_READ_WORD TALCALLBACK_EVENT_PRE_READ_WORD
#define TAL_EVENT_POST_READ_WORD TALCALLBACK_EVENT_POST_READ_WORD
#define TAL_EVENT_PRE_WRITE_WORD TALCALLBACK_EVENT_PRE_WRITE_WORD
#define TAL_EVENT_POST_WRITE_WORD TALCALLBACK_EVENT_POST_WRITE_WORD


/*!
******************************************************************************

 This structure contains the montor parameters use to define the
 region for TAL memory access events.

 NOTE: This structure MUST be defined in static memory as the structure
 is used by the TAL to "monitor" accesses.

 NOTE: Separate structures MUST be used for each memory access event, even
 if the same region of memory is being monitored.

 NOTE: The current implementation does not support the "monitoring" of
 overlapping regions, the field registered callback will be invoked and
 subsequent callbacks for the overlapping region will not be called.

******************************************************************************/
typedef struct
{
	/*! List link (allows these structure to be part of a MeOS list).  Use
		by the TAL. */
	LST_LINK;
	/*! Start offset (in bytes) for the region of registers or memory that
		is to be "monitored" */
	IMG_UINT32		ui32StartOffset;
	/*! The size (in bytes) for the region of registers or memory that
		is to be "monitored" */
	IMG_UINT32		ui32Size;
	/*! The callback handle returned when the callback is added.  This set
		by the TAl and is used to locate this structure and remove it from
		the list of monitored regions when the call back is removed.	*/
	CBMAN_hCallback	hEventCallback;
	/* The TAL event associated with this instance of the control block.
	   This is set by the TAL.											*/
	TALCALLBACK_eEvent		eEvent;

} TALCALLBACK_sMonitorAccessCB;

#define TAL_sMonitorAccessCB TALCALLBACK_sMonitorAccessCB

/*!
******************************************************************************

 @Function				TALCALLBACK_AddEventCallback

 @Description			This function adds an event callback. 

 The caller provides the address of a
 function that will be called when an event occurs.  The callback is persistent
 and is not removed when the application callback function is invoked.

 The callback function is of type CBMAN_pfnEventCallback.  When invoked it is
 passed:
	- pCallbackParameter
		- A pointer to the application specific data.
	- eEvent
		- An instance of TAL_eEvent, indicates the event which has occurred.
	- ui32Param
		- For events TAL_EVENT_POST_READ_WORD or TAL_EVENT_POST_WRITE_WORD then
		  ui32Param is the offset of the word accessed.
		- Otherwise the value is undefined.
	- pvParam
		- For events TAL_EVENT_POST_READ_WORD or TAL_EVENT_POST_WRITE_WORD then
		  pvParam is the current or new value of the word accessed and should be
		  type case to "IMG_UINT32".
		- Otherwise the value is undefined.

 NOTE: The same callback function can be used to handle several events by
 registering the callback multiple times using TAL_AddEventCallback.

 NOTE: All callbacks are removed by TAL_Reset.

 @Input		hMemSpace			: The handle to the memory space.

 @Input     eEvent				: The event to be signalled to the callback.

 @Input		pEventParameters	: Additional parameter associated with the event.
								  For events TAL_EVENT_PRE_READ_WORD, TAL_EVENT_POST_READ_WORD,
								  TAL_EVENT_PRE_WRITE_WORD or TAL_EVENT_POST_WRITE_WORD
								  then this points to a TAL_sMonitorAccessCB
								  structure defining the region to be "monitored".
								  For all other events pEventParameters is ignored.

 @Input     pfnEventCallback	: A pointer to the application callback function

 @Input     pCallbackParameter	: A pointer to application specific data or IMG_NULL

 @Output    phEventCallback		: A pointer to a handle to which the function
							      assigns a value identifying the callback

 @Return    IMG_RESULT			: This function returns either IMG_SUCCESS or an
                                  error code.

******************************************************************************/
extern
IMG_RESULT TALCALLBACK_AddEventCallback (
	IMG_HANDLE	                hMemSpace,
	TALCALLBACK_eEvent			eEvent,
	IMG_VOID *					pEventParameters,
	CBMAN_pfnEventCallback		pfnEventCallback,
	IMG_VOID *					pCallbackParameter,
	CBMAN_hCallback *			phEventCallback
);

#define TAL_AddEventCallback TALCALLBACK_AddEventCallback

/*!
******************************************************************************

 @Function				TALCALLBACK_RemoveEventCallback

 @Description			This function removes a specified event callback.  

 If the callback is already
 in progress when this function is called then it will complete normally;
 otherwise, the callback will be cancelled and no user callback will be made.

 @Input		hMemSpace		: The handle to the memory space.

 @Modified phEventCallback	: A pointer to the handle value returned by
							  TAL_AddEventCallback.  The handle value will
							  be set to IMG_NULL by this function

 @Return   IMG_RESULT		: This function returns either IMG_SUCCESS or an
                              error code.

******************************************************************************/
extern
IMG_RESULT  TALCALLBACK_RemoveEventCallback (
	IMG_HANDLE	                hMemSpace,
	CBMAN_hCallback *			phEventCallback
);

#define TAL_RemoveEventCallback TALCALLBACK_RemoveEventCallback

/*!
******************************************************************************

 @Function				TALCALLBACK_CallbackReadWord

 @Description			This function is used to read a 32-bits word value from a memory space from
 a callback function.

 @Input		hMemSpace	: The handle to the memory space.

 @Input		ui32Offset	: The offset, in bytes, of the from the base of the memory space.

 @Input		pui32Value	: A pointer used to return the current register setting.

 @Return	IMG_INT32	: IMG_SUCCESS if the memory space is supported by
						  the target. Asserts are use to trap unsupported
						  memory spaces.

******************************************************************************/
extern
IMG_RESULT  TALCALLBACK_CallbackReadWord(
	IMG_HANDLE					    hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32 *					pui32Value
);

IMG_RESULT TALCALLBACK_AddEventCallback (
    IMG_HANDLE	                hMemSpace,
    TALCALLBACK_eEvent          eEvent,
    IMG_VOID *                  pEventParameters,
    CBMAN_pfnEventCallback      pfnEventCallback,
    IMG_VOID *                  pCallbackParameter,
    CBMAN_hCallback *           phEventCallback
);

#define TAL_CallbackReadWord TALCALLBACK_CallbackReadWord
#define TAL_AddEventCallback TALCALLBACK_AddEventCallback

/*!
******************************************************************************

 @Function				TALCALLBACK_CallbackWriteWord

 @Description			This function is used to write a 32-bit word to a memory spacefrom
 a callback function.

 @Input		hMemSpace	: The handle to the memory space.

 @Input		ui32Offset	: The offset, in bytes, at which the value is to be written.

 @Input		ui32Value	: The value to be written.

 @Return	IMG_INT32	: IMG_SUCCESS if the memory space is supported by
						  the target. Asserts are use to trap unsupported
						  memory spaces.

******************************************************************************/
extern
IMG_RESULT  TALCALLBACK_CallbackWriteWord (
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32Value
);


extern
IMG_VOID TALCALLBACK_CheckMemoryAccessCallback (
    IMG_HANDLE                  hMemSpaceId,
    TALCALLBACK_eEvent          eEvent,
    IMG_UINT32                  ui32Offset,
    IMG_UINT64                  ui64Value
);

#define TAL_CallbackWriteWord TALCALLBACK_CallbackWriteWord
#define TAL_CheckMemoryAccessCallback TALCALLBACK_CheckMemoryAccessCallback

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Main group
 *------------------------------------------------------------------------*/
 /**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif 

#endif //__TAL_CALLBACK_H__



