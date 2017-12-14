/*!
******************************************************************************
 @file   : tal_mem.h

 @brief	API for the Target Abstraction Layer Device Memory.

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
         Target Abstraction Layer (TAL) Device Memory Function Header File.

 @Platform
	     Platform Independent


******************************************************************************/

#include "tal_defs.h"

#if !defined (__TALMEM_H__)
#define __TALMEM_H__

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
 * @name Memory functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Memory group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function              TALMEM_WriteWordDefineContext32

 @Description

 This function is used to write a 32 bit word to a memory space.
 The Pdump is diverted to an alternative context.

 @Input		hDeviceMem				: The handle to the device memory information
 @Input		ui64Offset				: The offset, in bytes, from the base of the memory block.
 @Input		ui32Value				: The value to be written.
 @Input		hContextMemspace		: A memoryspace in the pdump context to which
									  the command will be redirected
 @Return	IMG_INT32				: IMG_SUCCESS if the memory space is supported by
									  the target. Asserts are use to trap unsupported
									  memory spaces.

******************************************************************************/
IMG_RESULT TALMEM_WriteWordDefineContext32(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32Value,
	IMG_HANDLE						hContextMemspace
);

/*!
******************************************************************************

 @Function              TALMEM_WriteWordDefineContext64

 @Description

 This function is a 64bit version of TALMEM_WriteWordDefineContext32

******************************************************************************/
IMG_RESULT TALMEM_WriteWordDefineContext64(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                      ui64Value,
	IMG_HANDLE						hContextMemspace
);

/*!
******************************************************************************

 @Function              TALMEM_WriteWord32

 @Description

 This function is used to write a 32-bit word to a device memory.

 @Input		hDeviceMem				: The handle to the device memory information
 @Input		ui64Offset				: The offset, in bytes, from the base of the memory block.
 @Input		ui32Value				: The value to be written.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
									  the target. Asserts are use to trap unsupported
									  memory spaces.

******************************************************************************/
IMG_RESULT TALMEM_WriteWord32(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32Value
);

/*!
******************************************************************************

 @Function              TALMEM_WriteWord64

 @Description

 This function is a 64bit version of TALMEM_WriteWord32

******************************************************************************/
IMG_RESULT TALMEM_WriteWord64(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                      ui64Value
);

/*!
******************************************************************************

 @Function              TALMEM_ReadWord32

 @Description

 This function is used to read a 32 bit word value from device memory.

 @Input		hDeviceMem				: The handle to the device memory information
 @Input		ui64Offset				: The offset, in bytes, of the from the base of the memory block.
 @Input		pui32Value				: A pointer used to return the read value.
 @Return	IMG_INT32				: IMG_SUCCESS if the memory space is supported by
									  the target. Asserts are use to trap unsupported
									  memory spaces.
******************************************************************************/
IMG_RESULT TALMEM_ReadWord32(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_PUINT32						pui32Value
);

/*!
******************************************************************************

 @Function              TALMEM_ReadWord64

 @Description

 This function is a 64bit version of TALMEM_ReadWord32

******************************************************************************/
IMG_RESULT TALMEM_ReadWord64(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_PUINT64						pui64Value
);

/*!
******************************************************************************

 @Function				TALMEM_Poll32

 @Description

 This function is used to poll and wait for a value to be read from a word in
 a device memory block. This can be used to force synchronous operation within
 the host and is reflected in the PDUMP output.

 The TAL contains built in check functions to check for =, <, <=, >, and >=.  
 See #TAL_CHECKFUNC_ISEQUAL and associated defines.
 
 @Input		hDeviceMem			: A handle to the referenced device memory block.
 @Input		ui64Offset			: The offset from the base of the memory block.
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
IMG_RESULT TALMEM_Poll32(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT32                      ui32RequValue,
    IMG_UINT32                      ui32Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
);


/*!
******************************************************************************

 @Function				TALMEM_Poll64

 @Description

 This function is the 64bit version of TALMEM_Poll32

******************************************************************************/
IMG_RESULT TALMEM_Poll64(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
);

/*!
******************************************************************************

 @Function              TALMEM_CircBufPoll32

 @Description

 This function is used to poll for space to become availiable in a
 circular buffer.

 @Input		hDeviceMem			: A handle to the referenced device memory block.
 @Input		ui64Offset			: The offset, in bytes, from the base of
									the memory block where circular buffer
									poll roffvalue will be read.
 @Input		ui32WriteOffsetVal	: Circular buffer poll Write offset
 @Input		ui32PacketSize		: The size, in bytes, of the free space required.
 @Input		ui32BufferSize		: The total size, in bytes, of the circular buffer.
 @Input		ui32PollCount		: Number of times to repeat the poll.
 @Input		ui32TimeOut			: The time-out value, in clock cycles, between polls.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces and time-outs.

******************************************************************************/
IMG_RESULT TALMEM_CircBufPoll32(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64                      ui64Offset,
	IMG_UINT32						ui32WriteOffsetVal,
	IMG_UINT32						ui32PacketSize,
	IMG_UINT32						ui32BufferSize,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut
);

/*!
******************************************************************************

 @Function              TALMEM_CircBufPoll64

 @Description

 This function is the 64bit version of TALMEM_CircBufPoll32

******************************************************************************/
IMG_RESULT TALMEM_CircBufPoll64(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64                      ui64Offset,
	IMG_UINT64						ui64WriteOffsetVal,
	IMG_UINT64						ui64PacketSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut
);

/*!
******************************************************************************

 @Function				TALMEM_Malloc

 @Description

 This function is used to allocate device memory.

 @Input		hMemSpace		: MemSpace handle for device memory.
 @Input		pHostMem		: A pointer to the host memory to be shadowed.
 @Input		ui64Size		: The size, in bytes, to be allocated.
 @Input		ui64Alignment	: The required byte alignment (1, 2, 4, 8, 16 etc).
 @Output	phDeviceMem		: A pointer used to return a handle to the device memory block.
 @Input		bUpdateDevice	: IMG_TRUE to cause the device memory to be updated
								from the host, otherwise IMG_FALSE.
 @Input		pszMemName		: String giving the memory block name for pdumping, if NULL
								will autogenerate
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by the
								target. Asserts are use to trap unsupported memory
								spaces and allocation failures.

******************************************************************************/
IMG_RESULT TALMEM_Malloc(
	IMG_HANDLE						hMemSpace,
	IMG_CHAR *						pHostMem,
	IMG_UINT64						ui64Size,
	IMG_UINT64						ui64Alignment,
	IMG_HANDLE *					phDeviceMem,
	IMG_BOOL						bUpdateDevice,
	IMG_CHAR *						pszMemName
);

/*!
******************************************************************************

 @Function				TALMEM_Map

 @Description

 This function is used to map device memory.

 NOTE: This function is normally only used by the PdumpPlayer and other tools
 where the orginal device memory allocation is to be used.
 
 @Input		hMemSpace		: MemSpace handle for device memory.
 @Input		pHostMem		: A pointer to the host memory to be shadowed.
 @Input		ui64DeviceAddr	: The actual device address to be used for this
								allocation.
 @Input		ui64Size		: The size, in bytes, to be allocated.
 @Input		ui64Alignment	: The required byte alignment (1, 2, 4, 8, 16 etc).
 @Output	phDeviceMem		: A pointer used to return a handle to the device memory block.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by the
								target. Asserts are use to trap unsupported memory
								spaces and allocation failures.

******************************************************************************/
IMG_RESULT TALMEM_Map(
	IMG_HANDLE						hMemSpace,
	IMG_CHAR *						pHostMem,
	IMG_UINT64						ui64DeviceAddr,
	IMG_UINT64						ui64Size,
	IMG_UINT64						ui64Alignment,
	IMG_HANDLE *					phDeviceMem
);

/*!
******************************************************************************

 @Function				TALMEM_Free

 @Description

 This function is used to free device memory.

 @Input		phDeviceMem	: A handle to the device memory block.

 @Return	IMG_RESULT	: IMG_SUCCESS if the memory block is recognised
							and can be freed

******************************************************************************/
IMG_RESULT TALMEM_Free(
	IMG_HANDLE *			phDeviceMem
);

/*!
******************************************************************************

 @Function				TALMEM_UpdateDevice

 @Description

 This function is used to update the device memory (the current
 contents of the host buffer are transferred to the device).

 @Input		hDeviceMem		: A handle to the device memory block.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALMEM_UpdateDevice(
	IMG_HANDLE				hDeviceMem
);

/*!
******************************************************************************

 @Function				TALMEM_UpdateDeviceRegion

 @Description

 This function is used to update a part of a region of the device memory
 (the current contents of the region in the host buffer are transferred to the device).
  
 @Input		hDeviceMem		: A handle to the device memory block.
 @Input		ui64Offset		: The offset within the memory block of the region to be updated.
 @Input		ui64Size		: The size (in bytes) of the region to be updated.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALMEM_UpdateDeviceRegion(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64						ui64Offset,
	IMG_UINT64						ui64Size
);

/*!
******************************************************************************

 @Function				TALMEM_UpdateHost

 @Description

 This function is used to update the host buffer from the device memory
 (the current contents of the device memory are transferred to the
 host buffer).

 @Input		hDeviceMem		: A handle to the device memory block.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALMEM_UpdateHost(
	IMG_HANDLE						hDeviceMem
);


/*!
******************************************************************************

 @Function				TALMEM_UpdateHostRegion

 @Description

 This function is used to update a region of the host buffer from the device
 memory on (the current contents of the device memory are transferred to the
 host buffer).

 @Input		hDeviceMem		: A handle to the device memory block.
 @Input		ui64Offset		: The offset within the memory block of the region to be updated.
 @Input		ui64Size		: The size (in bytes) of the region to be updated.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALMEM_UpdateHostRegion(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64						ui64Offset,
	IMG_UINT64						ui64Size
);

/*!
******************************************************************************

 @Function				TALMEM_WriteDevMemRef32

 @Description

 Write the device address of a given memory block to a 32bit location in memory.

 @Input		hDeviceMem		: A handle to the device memory block to be updated.
 @Input		ui64Offset		: The offset in the destination memory block.
 @Input		hRefDeviceMem	: A handle to the referenced device memory block.
 @Input		ui64RefOffset	: The offset within the reference memory block.
 @Return	IMG_RESULT		: IMG_SUCCESS if the memory space is supported by
								the target. Asserts are use to trap unsupported
								memory spaces.

******************************************************************************/
IMG_RESULT TALMEM_WriteDevMemRef32(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64						ui64Offset,
	IMG_HANDLE						hRefDeviceMem,
	IMG_UINT64						ui64RefOffset
);

/*!
******************************************************************************

 @Function				TALMEM_WriteDevMemRef64

 @Description

 This function is a 64bit version of TALMEM_WriteDevMemRef32

******************************************************************************/
IMG_RESULT TALMEM_WriteDevMemRef64(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64						ui64Offset,
	IMG_HANDLE						hRefDeviceMem,
	IMG_UINT64						ui64RefOffset
);



/*!
******************************************************************************

 @Function				TALMEM_DumpImage

 @Description

 This function is used to dump an image (pixel data and image header
 details).  The host buffers referenced by hDeviceMem1, hDeviceMem2
 and hDeviceMem3 will be updated.

 NOTE: The arguments for second and third planes are only required
 for pixel formats having 2 or 3 planes.  These should be set to
 0 or IMG_NULL for planes which are not valid for the pixel format.

 @Input     psImageHeaderInfo	: A pointer to a partially completed
									Image Header structure.
 @Input     psImageFilename		: A pointer to a string containing the image filename/path.
 @Input		hDeviceMem1			: A handle to the device memory block containing the
									first plane of pixel data.
 @Input		ui64Offset1			: The offset of the pixel data within the block.
 @Input		ui32Size1			: The size (in bytes) of the pixel data.
 @Input		hDeviceMem2			: A handle to the device memory block containing the
									second plane of pixel data.
 @Input		ui64Offset2			: The offset of the pixel data within the block.
 @Input		ui32Size2			: The size (in bytes) of the pixel data.
 @Input		hDeviceMem3			: A handle to the device memory block containing the
									third plane of pixel data.
 @Input		ui64Offset3			: The offset of the pixel data within the block.
 @Input		ui32Size3			: The size (in bytes) of the pixel data.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces.

******************************************************************************/
IMG_RESULT TALMEM_DumpImage(
    TAL_sImageHeaderInfo *          psImageHeaderInfo,
    IMG_CHAR *                      psImageFilename,
	IMG_HANDLE						hDeviceMem1,
	IMG_UINT64						ui64Offset1,
	IMG_UINT32						ui32Size1,
	IMG_HANDLE						hDeviceMem2,
	IMG_UINT64						ui64Offset2,
	IMG_UINT32						ui32Size2,
	IMG_HANDLE						hDeviceMem3,
	IMG_UINT64						ui64Offset3,
	IMG_UINT32						ui32Size3
);


/*!
******************************************************************************

 @Function				TALMEM_ReadFromAddress

 @Description

 This function is used to read target memory, by looking up the memory block
 and using the wrapper normally.

 @Input		ui64MemoryAddress	: The address of the memory to be read.
 @Input		ui32Size			: The size of the memory to be read.
 @Modified	pui8Buffer			: A pointer to the buffer into which the memory is to be read.
 @Return	IMG_RESULT			: Returns IMG_SUCCESS if the block can be found and the memory read.

******************************************************************************/
IMG_RESULT TALMEM_ReadFromAddress(
	IMG_UINT64				ui64MemoryAddress,
	IMG_UINT32				ui32Size,
	IMG_UINT8 *				pui8Buffer
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Memory group
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

#endif //__TALMEM_H__


