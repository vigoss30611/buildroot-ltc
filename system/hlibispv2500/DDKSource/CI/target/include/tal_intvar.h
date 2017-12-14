/*!
******************************************************************************
 @file   : tal_intvar.h

 @brief	API for the Target Abstraction Layer Internal Variables.

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
         Target Abstraction Layer (TAL) Internal Variable Header File.

 @Platform
	     Platform Independent


******************************************************************************/


#if !defined (__TALINTVAR_H__)
#define __TALINTVAR_H__

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
 * @name Internal variable
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Internal group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TALINTVAR_GetValue

 @Description

 This function is used to read the value in an internal variable

 @Input		hMemSpace			: The memory space handle
 @Input		ui32InternalVarId	: The ID of the internal register to read.
 @Return	IMG_UINT64			: The value of the defined register

******************************************************************************/
extern  IMG_UINT64 TALINTVAR_GetValue (
	IMG_HANDLE                      hMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_WriteToMem32

 @Description

 This function is used to write a 32 bit word from an internal variable to device
 memory.

 @Input		hDeviceMem				: A handle to the device memory block to be updated.
 @Input		ui64Offset				: The offset within the memory block at which to write.
 @Input		hIntVarMemSpace			: Memory space for the internal variable.
 @Input		ui32InternalVarId		: The ID of the internal variable.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToMem32 (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_WriteToMem64

 @Description

 This function is a 64bit version of TALINTVAR_WriteToMem32

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToMem64 (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_ReadFromMem32

 @Description

 This function is used to read a 32 bit word into a internal variable from device
 memory.

 @Input		hDeviceMem				: A handle to the device memory from which to read.
 @Input		ui64Offset				: The offset within the memory block from which to read.
 @Input		hIntVarMemSpace			: Memory space of the internal variable.
 @Input		ui32InternalVarId		: The ID of the internal variable to write to.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromMem32 (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_ReadFromMem64

 @Description

 This function is a 64bit version of TALINTVAR_ReadFromMem32

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromMem64 (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);


/*!
******************************************************************************

 @Function				TALINTVAR_WriteMemRef
 @Description
 This function is used to write a device memory reference (address) into a 
 internal variable.

 @Input		hRefDeviceMem			: A handle to the referenced device memory.
 @Input		ui64RefOffset			: The offset into the device memory.
 @Input		hIntVarMemSpace		: Memory space of the internal variable.
 @Input		ui32InternalVarId		: The ID of the target internal variable.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_WriteMemRef (
    IMG_HANDLE                      hRefDeviceMem,
    IMG_UINT64                      ui64RefOffset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_RunCommand

 @Description

 This function is used to perfrom any internal variable function.

 @Input		ui32CommandId			: The Id of the command to be run.
 @Input		hDestRegMemSpace		: Memory space of the destination internal variable.
 @Input		ui32DestRegId			: The ID of the internal variable where the result will be stored.
 @Input		hOpRegMemSpace			: Memory space of the operand internal register.
										(This variable is ignored in MOV commands)
 @Input		ui32OpRegId				: The ID of the internal register whose contents are to be shifted.
										(This variable is ignored in MOV commands)
 @Input		hLastOpMemSpace			: Memory space of the last operand if it is a register.
										(This variable is ignored in NOT commands)
 @Input		ui64LastOperand			: Depending on the value of bIsRegisterLastOperand, the argument is either
										the ID of an internal register ID or an immediate value to be used to work out
										the number of bits by which ui32OpRegId will be shifted right.
										(This variable is ignored in NOT commands)
 @Input		bIsRegisterLastOperand	: Indicates whether ui32LastOperand is a internal register ID. If equal to IMG_FALSE
										ui64LastOperand is to be treated as immediate data.
										(This variable is ignored in NOT commands)
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
									  the target. Asserts are use to trap unsupported
									  memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_RunCommand(
	IMG_UINT32						ui32CommandId,
	IMG_HANDLE						hDestRegMemSpace,
	IMG_UINT32						ui32DestRegId,
	IMG_HANDLE						hOpRegMemSpace,
	IMG_UINT32                      ui32OpRegId,
	IMG_HANDLE						hLastOpMemSpace,
	IMG_UINT64						ui64LastOperand,
	IMG_BOOL						bIsRegisterLastOperand
);

/*!
******************************************************************************

 @Function				TALINTVAR_WriteToVirtMem32

 @Description

 This function is used to write a 32 bit word from a internal variable to device
 memory, using a virtual address.

 @Input		hMemSpace			: The memory space of the memory.
 @Input		ui64DevVirtAddr		: Device virtual address
 @Input     ui32MmuContextId	: The MMU context id.
 @Input		hIntVarMemSpace	: Memory space of the internal register.
 @Input		ui32InternalVarId	: The ID of the internal register.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToVirtMem32 (
	IMG_HANDLE						hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);


/*!
******************************************************************************

 @Function				TALINTVAR_WriteToVirtMem64

 @Description

 This function is the 64bit equivalent of TALINTVAR_WriteToVirtMem32


******************************************************************************/
IMG_RESULT TALINTVAR_WriteToVirtMem64 (
	IMG_HANDLE						hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_ReadFromVirtMem32

 @Description

 This function is used to read a 32 bit word into an internal variable from virtual
 memory.

 @Input		hMemSpace			: The memory space of the memory.
 @Input		ui64DevVirtAddr		: Device virtual address.
 @Input     ui32MmuContextId	: The MMU context id.
 @Input		hIntVarMemSpace	: Memory space of the internal register.
 @Input		ui32InternalVarId	: The ID of the internal register to write to.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromVirtMem32 (
	IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);


/*!
******************************************************************************

 @Function				TALINTVAR_ReadFromVirtMem64

 @Description

 This function is the 64bit equivalent of TALINTVAR_ReadFromVirtMem32


******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromVirtMem64 (
	IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);


/*!
******************************************************************************

 @Function				TALINTVAR_WriteVirtMemReference

 @Description

 This function is used to read a device memory refernce (derived from the
 virtual address)into a internal variable.

 @Input		hMemSpace			: Memory space of the memory
 @Input		ui64DevVirtAddr		: The virtual address
 @Input		ui32MmuContextId	: the Id of the MMU context.
 @Input		hIntVarMemSpace	: Memory space of the internal register.
 @Input		ui32InternalVarId	: The ID of the internal register to write to.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_WriteVirtMemReference (
	IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_WriteToReg32

 @Description

 This function is used to write a 32 bit word from an internal register to a
 device register.

 @Input		hMemSpace				: Memspace Handle for Register.
 @Input		ui32Offset				: The offset, in bytes, from the base of the register.
 @Input		hIntVarMemSpace			: Memory space of the internal register.
 @Input		ui32InternalVarId		: The ID of the internal register.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToReg32	(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_WriteToReg64

 @Description

 This function is used to write a 64 bit word from an internal register to a
 device register.

 @Input		hMemSpace				: Memspace Handle for Register.
 @Input		ui32Offset				: The offset, in bytes, from the base of the register.
 @Input		hIntVarMemSpace			: Memory space of the internal register.
 @Input		ui32InternalVarId		: The ID of the internal register.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToReg64	(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);


/*!
******************************************************************************

 @Function				TALINTVAR_ReadFromReg32

 @Description

 This function is used to write a 32 bit word to an internal register from a
 device register.

 @Input		hMemSpace				: Memspace Handle for Register.
 @Input		ui32Offset				: The offset, in bytes, from the base of the register.
 @Input		hIntVarMemSpace			: Memory space of the internal variable.
 @Input		ui32InternalVarId		: The ID of the internal variable to write to.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromReg32	(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

/*!
******************************************************************************

 @Function				TALINTVAR_ReadFromReg64

 @Description

 This function is used to write a 64 bit word to an internal register from a
 device register.

 @Input		hMemSpace				: Memspace Handle for Register.
 @Input		ui32Offset				: The offset, in bytes, from the base of the register.
 @Input		hIntVarMemSpace			: Memory space of the internal variable.
 @Input		ui32InternalVarId		: The ID of the internal variable to write to.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
										the target. Asserts are use to trap unsupported
										memory spaces.

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromReg64	(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Internal group
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

#endif //__TALINTVAR_H__
