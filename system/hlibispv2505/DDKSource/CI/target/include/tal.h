/*!
******************************************************************************
 @file   : tal.h

 @brief	API for the Target Abstraction Layer.

 @Author Ray Livesley

 @date   02/06/2003

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

 \n<b>Description:</b>\n
         Target Abstraction Layer (TAL) Header File.

		 NOTE: The conditional compile flag:

		 -	__TAL_CALLBACK_SUPPORT__ is defined to enable the callback support for register accesses made via the TAL.
		 -	__TAL_NO_OS__ is defined to remove/disable any thread safe OS calls from the TAL and lower-levels.
		 -	__TAL_MAKE_THREADSAFE__ is defined when __TAL_NO_OS__ is not defined to make the TAL APIs thread safe.

 \n<b>Platform:</b>\n
	     Platform Independent

******************************************************************************/

#if !defined (__TAL_H__)
#define __TAL_H__

#include "tal_reg.h"
#include "tal_mem.h"
#include "tal_vmem.h"
#include "tal_debug.h"
#include "tal_intvar.h"
#include "tal_pdump.h"
#include "tal_setup.h"
#include "pdump_cmds.h"

#if defined (__TAL_CALLBACK_SUPPORT__)
#include "tal_callback.h"
#endif

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
 * @name Main functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Main group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TAL_MemSpaceGetId

 @Description

 @Input		psMemSpaceName	: A pointer to a string specifying the required
								memory space.

 @Output    pui32MemSpaceId	: 

 @Return	None

******************************************************************************/
IMG_VOID TAL_MemSpaceGetId (
	const IMG_CHAR *		psMemSpaceName,
    IMG_UINT32*             pui32MemSpaceId
);

/*!
******************************************************************************

 @Function				TAL_GetMemSpaceHandle

 @Description

 This function is used to get the handle of a memory space associated with a
 peripheral.  The handle rather than the name is used in all other functions.

 NOTE: The names used to identify memory spaces are not defined in this
 document.  It requires only that the test application or driver and target
 agree on the naming.  The following are examples of the names that might be
 used for SGX: SGXREG for the regsiters and SGXMEM for the memory

 @Input		psMemSpaceName	: A pointer to a string specifying the required
								memory space.
 @Return	IMG_HANDLE		: Either the handle or IMG_NULL on failure

******************************************************************************/
IMG_HANDLE TAL_GetMemSpaceHandle (
	const IMG_CHAR *		psMemSpaceName
);

/*!
******************************************************************************

 @Function              TAL_CheckMemSpaceEnable

 @Description

 This function is used to check to see if this memory space has been
 disabled by an if operation

 @Input		hMemSpace	: The memory space.
 @Return	IMG_BOOL	: True if the memory space is enabled.

******************************************************************************/
IMG_BOOL TAL_CheckMemSpaceEnable(
    IMG_HANDLE			hMemSpace
);

/*!
******************************************************************************

 @Function				TAL_Wait

 @Description

 This function is used to wait for a number of clock cycles.

 @Input		hMemSpace	: The peripheral memory space.
 @Input		ui32Time	: The time to wait, in clock cycles.
 @Return	IMG_RESULT	: IMG_SUCCESS if the memory space is supported by
						  the target. Asserts are use to trap unsupported
						  memory spaces.

******************************************************************************/
IMG_RESULT TAL_Wait(
    IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Time
);

/*!
******************************************************************************

 @Function              TAL_TestWord

 @Description

 This function Performs the test described by the given function id
 and returns the result of the test

 @Input     ui64ReadValue		: The value to test.
 @Input		ui64TestVal_WrOff	: Either the value to test against or the write offset(CBP)
 @Input		ui64Enable_PackSize	: Either the mask for the test or the packet size(CBP)
 @Input		ui64BufferSize		: The buffer size (CBP Only)
 @Input		ui32CheckFuncId		: The function id(eg TAL_CHECKFUNC_ISEQUAL) 
 @Return	IMG_BOOL			: IMG_TRUE if the test passes

******************************************************************************/
IMG_BOOL TAL_TestWord(
    IMG_UINT64                      ui64ReadValue,
	IMG_UINT64						ui64TestVal_WrOff,
	IMG_UINT64						ui64Enable_PackSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32CheckFuncId
);

#ifdef DOXYGEN_GENERATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Main group
 *------------------------------------------------------------------------*/
 /**
  * @name Loop functions
  * @{
  */
/*-------------------------------------------------------------------------
 * Following elements are in the Loop group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TALLOOP_Open

 @Description

 This function is used to open a new TAL loop construct.

 @Input     hMemSpace	: The memory space in which to run the loop
 @Output	phLoop		: A handle for the loop construct.
 @Return	IMG_RESULT	: IMG_SUCCESS if all runs ok.

******************************************************************************/
IMG_RESULT TALLOOP_Open(
    IMG_HANDLE						hMemSpace,
	IMG_HANDLE *					phLoop
);



/*!
******************************************************************************

 @Function				TALLOOP_Start

 @Description

 This function is used to denote the start of the loop hLoop. It is to be used
 in conjunction with TALLOOP_End. This function needs to be called at the start
 of every iteration of the loop.

 @Input		hLoop		: A handle for the loop construct.
 @Return	IMG_RESULT	: IMG_SUCCESS if all runs ok.

******************************************************************************/
IMG_RESULT TALLOOP_Start(
	IMG_HANDLE						hLoop
);



/*!
******************************************************************************

 @Function				TALLOOP_TestRegister

 @Description

 This function is used to test the loop condition by reading a register.

 Note that only the built in check functions are supported. See
 #TAL_CHECKFUNC_ISEQUAL and associated defines.

 @Input		hLoop				: A handle for the loop construct.
 @Input		hMemSpace			: The register memory space.
 @Input		ui32Offset			: The offset, in bytes, of the register from the base of
									the memory space.
 @Input		ui32CheckFuncIdExt	: The check function ID
 @Input		ui32RequValue		: The required value.
 @Input		ui32Enable			: A mask to be applied to the value read before
									comparing with the required value.
 @Input		ui32LoopCount		: The number of times to go round the loop
 @Return	IMG_RESULT			: IMG_SUCCESS if all runs ok.

******************************************************************************/
IMG_RESULT TALLOOP_TestRegister(
	IMG_HANDLE						hLoop,
    IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Enable,
	IMG_UINT32						ui32LoopCount
);



/*!
******************************************************************************

 @Function				TALLOOP_TestMemory

 @Description

 This function is used to test the loop condition by reading a memory location.

 Note that only the built in check functions are supported. See
 #TAL_CHECKFUNC_ISEQUAL and associated defines.

 @Input		hLoop				: A handle for the loop construct.
 @Input		hDeviceMem			: A handle to the referenced device memory.
 @Input		ui32Offset			: The offset, in bytes, from the base of
									the memory space.
 @Input		ui32CheckFuncIdExt	: The check function ID
 @Input		ui32RequValue		: The required value.
 @Input		ui32Enable			: A mask to be applied to the value read before
									comparing with the required value.
 @Input		ui32LoopCount		: The number of times to go round the loop
 @Return	IMG_RESULT			: IMG_SUCCESS if all runs ok.

******************************************************************************/
IMG_RESULT TALLOOP_TestMemory(
	IMG_HANDLE						hLoop,
	IMG_HANDLE						hDeviceMem,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT32						ui32RequValue,
	IMG_UINT32						ui32Enable,
	IMG_UINT32						ui32LoopCount
);



/*!
******************************************************************************

 @Function				TALLOOP_TestInternalReg

 @Description

 This function is used to test the loop condition by reading an internal register.

 Note that only the built in check functions are supported. See
 #TAL_CHECKFUNC_ISEQUAL and associated defines.

 @Input		hLoop				: A handle for the loop construct.
 @Input		hMemSpace			: The memory space for the internal register.
 @Input		ui32RegId			: The ID of the internal register.
 @Input		ui32CheckFuncIdExt	: The check function ID
 @Input		ui32RequValue		: The required value.
 @Input		ui32Enable			: A mask to be applied to the value read before
									comparing with the required value.
 @Input		ui32LoopCount		: The number of times to go round the loop
 @Return	IMG_RESULT			: IMG_SUCCESS if all runs ok.

******************************************************************************/
IMG_RESULT TALLOOP_TestInternalReg(
	IMG_HANDLE						hLoop,
    IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32RegId,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Enable,
	IMG_UINT32						ui32LoopCount
);



/*!
******************************************************************************

 @Function				TALLOOP_End

 @Description

 This function denotes the end of the block of instructions that constitute
 the loop defined by hLoop.
 It acts as a means of determining success or failure of the loop condition.
 Each call to the function MUST be precedeed by a call to TALLOOP_Start.

 @Input		hLoop			: Handle to loop control block.
 @Input		peLoopControl	: A pointer used to return the result of the loop test.
 @Return	IMG_RESULT		: IMG_SUCCESS if all runs ok.

******************************************************************************/
IMG_RESULT TALLOOP_End(
	IMG_HANDLE						hLoop,
    TAL_eLoopControl *				peLoopControl
);



/*!
******************************************************************************

 @Function				TALLOOP_Close

 @Description

 This function is to close a loop.

 @Input     hMemSpace 	: The handle to the appropriate memspace

 @Input		phLoop		: A handle for the loop construct.

 @Return	IMG_RESULT	: IMG_SUCCESS if all runs ok.

******************************************************************************/
IMG_RESULT TALLOOP_Close(
	IMG_HANDLE						hMemSpace,
	IMG_HANDLE *					phLoop
);

/*!
******************************************************************************

 @Function              TALLOOP_GetLoopControl

 @Description

 This function is used to get the loop state of the current loop.

 @Input		hMemSpace			: The memory space.
 @Return	TAL_eLoopControl	: TAL_LOOP_TEST_FALSE if the loop was tested false,
								  TAL_LOOP_TEST_TRUE if the loop was tested true 
									or there is not a current loop.

******************************************************************************/
TAL_eLoopControl TALLOOP_GetLoopControl(
    IMG_HANDLE hMemSpace
);

#ifdef DOXYGEN_GENERATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Loop group
 *------------------------------------------------------------------------*/
 /**
  * @name Interupt functions
  * @{
  */
/*-------------------------------------------------------------------------
 * Following elements are in the Interupt group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TALINTERRUPT_UnRegisterCheckFunc

 @Description

 This function unregisters the interrupt check function, such that on return
 no further checks will be performed by the checking task.

 @Input		hMemSpace	: The memory space handle.
 @Return	IMG_INT32	: IMG_SUCCESS if the memory space is supported by
						  the target. Asserts are use to trap unsupported
						  memory spaces.

*******************************************************************************/
IMG_RESULT TALINTERRUPT_UnRegisterCheckFunc(IMG_HANDLE hMemSpace);


/*!
******************************************************************************

 @Function              TALINTERRUPT_RegisterCheckFunc

 @Description

 This function is used to get the address of a check interrupt function associated
 with the memory space.

 @Input     hMemSpace				: The memory space handle.
 @Input     pfnCheckInterruptFunc	: A pointer to the check function.
 @Input     pCallbackParameter		: A pointer to application specific data or IMG_NULL.
 @Return    IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

*******************************************************************************/
IMG_RESULT TALINTERRUPT_RegisterCheckFunc(
    IMG_HANDLE                      hMemSpace,
    TAL_pfnCheckInterruptFunc       pfnCheckInterruptFunc,
    IMG_VOID *                      pCallbackParameter
);

/*!
******************************************************************************

 @Function				TALINTERRUPT_GetMask

 @Description

 This function is used to get the the interrupt masks for a memory space interrupt.

 @Input		hMemSpace		: The memory space to use.
 @Output	aui32IntMasks	: Array used to return the interrupt masks.
 @Return	IMG_RESULT		: This function returns either IMG_SUCCESS or an
                          error code.

******************************************************************************/
IMG_RESULT TALINTERRUPT_GetMask(
    IMG_HANDLE						hMemSpace,
	IMG_UINT32						aui32IntMasks[]
);


/*!
******************************************************************************

 @Function              TALINTERRUPT_GetNumber

 @Description

 This function is used to get the the interrupt number for a memory space interrupt.

 @Input		hMemSpace		: The memory space to use.
 @Output    pui32IntNum		: Array used to return the interrupt number.
 @Return	IMG_RESULT		: This function returns either IMG_SUCCESS or an
								error code 

******************************************************************************/
IMG_RESULT TALINTERRUPT_GetNumber(
    IMG_HANDLE						hMemSpace,
    IMG_UINT32 *                    pui32IntNum
);

#ifdef DOXYGEN_GENERATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Interupt group
 *------------------------------------------------------------------------*/
 /**
  * @name Iterator functions
  * @{
  */
/*-------------------------------------------------------------------------
 * Following elements are in the Iterator group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function              TALITERATOR_GetFirst

 @Description

 This function is used to start an iterator and return the first item.

 @Input		eItrType		: The iterator type.
 @Output	phIterator		: A handle for the iterator (IMG_NULL if less than 2 entries)
 @Return	IMG_PVOID		: This function returns either the first entry of the 
								given type or IMG_NULL if there are none.

******************************************************************************/
IMG_PVOID TALITERATOR_GetFirst(
    TALITERATOR_eTypes	eItrType,
	IMG_HANDLE *		phIterator
);

/*!
******************************************************************************

 @Function              TALITERATOR_GetNext

 @Description

 This function is used to get the next entry in an iteration, TALITERATOR_Start
 must be run first.

 @Input		hIterator		: The iterator handle created using TALITERATOR_Start.
 @Return	IMG_PVOID		: This function returns either the next entry of the 
								given type or IMG_NULL if there are no more.

******************************************************************************/
IMG_PVOID TALITERATOR_GetNext(IMG_HANDLE hIterator);

/*!
******************************************************************************

 @Function              TALITERATOR_Destroy

  @Description

 This function is used to tidy up an iterator after it has been used.
 This is not required if the iterator has run all the way through but can be called.

 @Input		hIterator		: The iterator handle created using TALITERATOR_Start.
 @Return	None.

******************************************************************************/
IMG_VOID TALITERATOR_Destroy(IMG_HANDLE hIterator);

// This file is included at the end as it contains various #defines to override functions

#ifndef DOXYGEN_WILL_SEE_THIS
#include "tal_internal.h"
#endif

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Iterator group
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


#endif /* __TAL_H__	*/


