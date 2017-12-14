/*!
******************************************************************************
 @file   : tal_reg.h

 @brief	API for the Target Abstraction Layer Registers.

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
         Target Abstraction Layer (TAL) Register Function Header File.

 @Platform
	     Platform Independent


******************************************************************************/



#if !defined (__TALREG_H__)
#define __TALREG_H__

#include "tal_defs.h"

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
 * @name Register functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Register group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TALREG_WriteWord32

 @Description

 This function is used to write a 32-bit word to a register defined
 by the memspace id and offset.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		ui32Value		: The value to be written.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_WriteWord32(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT32		ui32Value);

/*!
******************************************************************************

 @Function				TALREG_WriteWord64

 @Description

 This function is used to write a 64-bit word to a register defined
 by the memspace id and offset.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		ui64Value		: The value to be written.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_WriteWord64(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT64		ui64Value);

/*!
******************************************************************************

 @Function				TALREG_WriteWords32

 @Description

 This function is used to write multiple 32-bit words to a register defined
 by the memspace id and offset.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		ui32WordCount	: The number of words values to be written.
 @Input		pui32Value		: A pointer to an array containing the values to be written.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_WriteWords32(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT32		ui32WordCount,
	IMG_UINT32 *	pui32Value
);

/*!
******************************************************************************

 @Function				TALREG_WriteWordDefineContext32

 @Description

 This function is used to write a 32-bit word to a regsiter defined
 by the memspace id and offset.
 The Pdump is diverted to an alternative context.

 @Input		hMemSpace				: Memspace Handle for Register.
 @Input		ui32Offset				: The offset, in bytes, from the base of the register.
 @Input		ui32Value				: The value to be written.
 @Input		hContextMemSpace		: A memoryspace in the pdump context to which
										the command will be redirected
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALREG_WriteWordDefineContext32(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT32		ui32Value,
	IMG_HANDLE		hContextMemSpace);

/*!
******************************************************************************

 @Function				TALREG_WriteWordDefineContext64

 @Description

 This function is used to write a 64-bit word to a regsiter defined
 by the memspace id and offset.
 The Pdump is diverted to an alternative context.

 @Input		hMemSpace				: Memspace Handle for Register.
 @Input		ui32Offset				: The offset, in bytes, from the base of the register.
 @Input		ui64Value				: The value to be written.
 @Input		hContextMemSpace		: A memoryspace in the pdump context to which
										the command will be redirected
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALREG_WriteWordDefineContext64(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT64		ui64Value,
	IMG_HANDLE		hContextMemSpace);

/*!
******************************************************************************

 @Function				TALREG_WriteDevMemRef32

 @Description
 This function is used to write a "reference to a device memory address" to
 a register defined by the memspace id and offset.
 The address of the (source) device memory base plus the offset is written
 to the (destination) register.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		hRefDeviceMem	: A handle to the referenced (source) device
								memory.
 @Input		ui64RefOffset	: The offset to be applied, in bytes, from the
								base address of the referenced (source) device
								memory.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by the
								target. Asserts are use to trap unsupported memory
								spaces.

******************************************************************************/
IMG_RESULT TALREG_WriteDevMemRef32(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_HANDLE		hRefDeviceMem,
	IMG_UINT64		ui64RefOffset);

/*!
******************************************************************************

 @Function				TALREG_WriteDevMemRef64

 @Description
 This function is used to write a "reference to a device memory address" to
 a 64bit register defined by the memspace id and offset.
 The address of the (source) device memory base plus the offset is written
 to the (destination) register.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		hRefDeviceMem	: A handle to the referenced (source) device
								memory.
 @Input		ui64RefOffset	: The offset to be applied, in bytes, from the
								base address of the referenced (source) device
								memory.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by the
								target. Asserts are use to trap unsupported memory
								spaces.

******************************************************************************/
IMG_RESULT TALREG_WriteDevMemRef64(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_HANDLE		hRefDeviceMem,
	IMG_UINT64		ui64RefOffset);


/*!
******************************************************************************

 @Function				TALREG_ReadWord32

 @Description

 This function is used to read a 32-bit word value from a Register defined
 by the memspace id and offset.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		pui32Value		: A pointer used to return the current register setting.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_ReadWord32(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT32 *	pui32Value);


/*!
******************************************************************************

 @Function				TALREG_ReadWord64

 @Description

 This function is used to read a 64-bit word value from a Register defined
 by the memspace id and offset.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		pui64Value		: A pointer used to return the current register setting.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_ReadWord64(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT64 *	pui64Value);


/*!
******************************************************************************

 @Function				TALREG_ReadWords32

 @Description

 This function is used to read multiple 32-bits word value from Register defined
 by the memspace id and offset.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		ui32WordCount	: The number of words to be read.
 @Input		pui32Value		: A pointer used to an array used to return the
								values read.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_ReadWords32(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32WordCount,
	IMG_UINT32 *					pui32Value
);

/*!
******************************************************************************

 @Function				TALREG_ReadWordToPoll32

 @Description

 This function is used to read a 32-bits word value from a register which is 
 previously unknown and if PDUMP is enabled to poll the value read on 
 subsequent verification.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		pui32Value		: A pointer used to return the current register value.
 @Input		ui32Count		: The count of poll loops to perform during verification.
 @Input		ui32Time		: The waiting time between poll loops during verification.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_ReadWordToPoll32(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT32 *	pui32Value,
	IMG_UINT32		ui32Count,
	IMG_UINT32		ui32Time);

/*!
******************************************************************************

 @Function				TALREG_ReadWordToPoll64

 @Description

 This function is used to read a 64-bit word value from a register which is 
 previously unknown and if PDUMP is enabled to poll the value read on 
 subsequent verification.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		pui64Value		: A pointer used to return the current register value.
 @Input		ui32Count		: The count of poll loops to perform during verification.
 @Input		ui32Time		: The waiting time between poll loops during verification.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_ReadWordToPoll64(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT64 *	pui64Value,
	IMG_UINT32		ui32Count,
	IMG_UINT32		ui32Time);


/*!
******************************************************************************

 @Function				TALREG_ReadWordToSAB32

 @Description

 This function is used to read a 32-bits word value from a register which is 
 previously unknown and if PDUMP is enabled to poll the value read on 
 subsequent verification.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		pui32Value		: A pointer used to return the current register value.
 @Input		ui32Count		: The count of poll loops to perform during verification.
 @Input		ui32Time		: The waiting time between poll loops during verification.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_ReadWordToSAB32(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT32 *	pui32Value,
	IMG_UINT32		ui32Count,
	IMG_UINT32		ui32Time);

/*!
******************************************************************************

 @Function				TALREG_ReadWordToSAB64

 @Description

 This function is used to read a 64-bit word value from a register which is 
 previously unknown and if PDUMP is enabled to verify the value read on 
 subsequent playback.

 @Input		hMemSpace		: Memspace Handle for Register.
 @Input		ui32Offset		: The offset, in bytes, from the base of the register.
 @Input		pui64Value		: A pointer used to return the current register value.
 @Input		ui32Count		: The count of poll loops to perform during verification.
 @Input		ui32Time		: The waiting time between poll loops during verification.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALREG_ReadWordToSAB64(
	IMG_HANDLE		hMemSpace,
	IMG_UINT32		ui32Offset,
	IMG_UINT64 *	pui64Value,
	IMG_UINT32		ui32Count,
	IMG_UINT32		ui32Time);

/*!
******************************************************************************

 @Function				TALREG_Poll32

 @Description

 This function is used to poll for a given value against a register.
 This can be used to force synchronous operation within the host and
 is reflected in the PDUMP output. This function locks all threads
 entering the TAL until the poll is complete.

 The TAL contains built in check functions to check for =, <, <=,
 >, and >=.  See #TAL_CHECKFUNC_ISEQUAL and associated defines.

 @Input		hMemSpace			: Memspace Handle for Register.
 @Input		ui32Offset			: The offset, in bytes, from the base of the register.
 @Input		ui32CheckFuncIdExt	: The check function ID.
 @Input		ui32RequValue		: The required value.
 @Input		ui32Enable			: A mask to be applied to the value read before
									comparing with the required value.
 @Input		ui32PollCount		: Number of times to repeat the poll.
 @Input		ui32TimeOut			: The time-out value, in clock cycles, between polls.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces and time-outs.

******************************************************************************/
IMG_RESULT TALREG_Poll32(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Enable,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut
);

/*!
******************************************************************************

 @Function				TALREG_Poll64

 @Description

 This function is used to poll for a given value against a 64bit register.
 This can be used to force synchronous operation within the host and
 is reflected in the PDUMP output. This function locks all threads
 entering the TAL until the poll is complete.

 The TAL contains built in check functions to check for =, <, <=,
 >, and >=.  See #TAL_CHECKFUNC_ISEQUAL and associated defines.

 @Input		hMemSpace			: Memspace Handle for Register.
 @Input		ui32Offset			: The offset, in bytes, from the base of the register.
 @Input		ui32CheckFuncIdExt	: The check function ID.
 @Input		ui64RequValue		: The required value.
 @Input		ui64Enable			: A mask to be applied to the value read before
									comparing with the required value.
 @Input		ui32PollCount		: Number of times to repeat the poll.
 @Input		ui32TimeOut			: The time-out value, in clock cycles, between polls.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces and time-outs.

******************************************************************************/
IMG_RESULT TALREG_Poll64(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT64    					ui64RequValue,
	IMG_UINT64    					ui64Enable,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut
);

/*!
******************************************************************************

 @Function              TALREG_CircBufPoll32

 @Description

 This function is used to poll for space to become availiable in a
 circular buffer.

 @Input		hMemSpace			: Memspace Handle for Register.
 @Input		ui32Offset			: The offset, in bytes, from the base of the
									register where roffvalue will be read.
 @Input		ui32WriteOffsetVal	: Circular buffer poll Write offset
 @Input		ui32PacketSize		: The size, in bytes, of the free space required.
 @Input		ui32BufferSize		: The total size, in bytes, of the circular buffer.
 @Input		ui32PollCount		: The number of times to loop before failing
 @Input		ui32TimeOut			: The number of clock cycles to wait between polls
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces and time-outs.

******************************************************************************/
IMG_RESULT TALREG_CircBufPoll32(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT32                      ui32Offset,
	IMG_UINT32						ui32WriteOffsetVal,
	IMG_UINT32						ui32PacketSize,
	IMG_UINT32						ui32BufferSize,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Register group
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

#endif //__TALREG_H__
