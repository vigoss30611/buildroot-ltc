/*!
******************************************************************************
 @file   : tal_vmem.h

 @brief	API for the Target Abstraction Layer Device Virtual Memory.

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
         Target Abstraction Layer (TAL) Device Virtual Memory Function Header File.

 @Platform
	     Platform Independent


******************************************************************************/

#if !defined (__TALVMEM_H__)
#define __TALVMEM_H__

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
 * @name Device Virtual Memory functions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the DEVIF group
 *------------------------------------------------------------------------*/
#endif

/*!
******************************************************************************

 @Function				TALVMEM_SetContext

 @Description

 This function is used to set an MMU context - overwriting
 any previous context for for the specified ID.

 @Input		hMemSpace			: The memory space of the device memory.
 @Input		ui32MmuContextId	: The MMU context id (0 to 9)
 @Input		ui32MmuType			: The MMU type
 @Input		hDeviceMem			: A handle for the device memory containing the
									Top Level Page Table.
 @Return	IMG_RESULT			: IMG_SUCCESS on correct setup 
									asserts trap invalid parameters

******************************************************************************/
IMG_RESULT TALVMEM_SetContext(
	IMG_HANDLE				hMemSpace,
    IMG_UINT32              ui32MmuContextId,
    IMG_UINT32              ui32MmuType,
    IMG_HANDLE              hDeviceMem
);

/*!
******************************************************************************

 @Function				TALVMEM_ClearContext

 @Description

 This function is used to clear an MMU context
 NOTE: This clears any tiled regions associated with this context.

 @Input		hMemSpace			: The memory space of the device memory.
 @Input		ui32MmuContextId	: The MMU context id (0 to 9)
 @Return	IMG_RESULT			: IMG_SUCCESS on correct setup 
									asserts trap invalid parameters

******************************************************************************/
IMG_RESULT TALVMEM_ClearContext(
	IMG_HANDLE				hMemSpace,
    IMG_UINT32              ui32MmuContextId
);
#define TALVMEM_ClearContext(hMemSpace,ui32MmuContextId) \
		TALVMEM_SetContext(hMemSpace,ui32MmuContextId,0,IMG_NULL);

/*!
******************************************************************************

 @Function				TALVMEM_SetTiledRegion

 @Description

 This function is used to set a tiled region - overwriting any previous region
 set for the specified table number.

 @Input		hMemSpace			: The memory space of the device memory.
 @Input		ui32MmuContextId	: The ID of the MMU Context.
 @Input		ui32TiledRegionNo	: The Tiled Region number
 @Input		ui64DevVirtAddr		: The device virtual address of the start of the region
 @Input		ui64Size			: The size of the region (in bytes)
 @Input		ui32XTileStride		: The X Tile Stride - should be valid for the
									the ui32MmuType defined in TAL_MmuSetContext()
 @Return	IMG_RESULT			: IMG_SUCCESS on correct setup 
									asserts trap invalid parameters

******************************************************************************/
IMG_RESULT TALVMEM_SetTiledRegion(
	IMG_HANDLE				hMemSpace,
    IMG_UINT32              ui32MmuContextId,
    IMG_UINT32              ui32TiledRegionNo,
	IMG_UINT64				ui64DevVirtAddr,
    IMG_UINT64              ui64Size,
    IMG_UINT32              ui32XTileStride
);

/*!
******************************************************************************

 @Function				TALVMEM_ClearTiledRegion

 @Description

 This function is used to clear a tiled region.

 @Input		hMemSpace			: The memory space of the device memory.
 @Input		ui32MmuContextId	: The ID of the MMU Context.
 @Input		ui32TiledRegionNo	: The Tiled Region number
 @Return	IMG_RESULT			: IMG_SUCCESS on correct setup 
									asserts trap invalid parameters

******************************************************************************/
IMG_RESULT TALVMEM_ClearTiledRegion(
	IMG_HANDLE				hMemSpace,
    IMG_UINT32              ui32MmuContextId,
    IMG_UINT32              ui32TiledRegionNo
);
#define TALVMEM_ClearTiledRegion(hMemSpace,ui32MmuContextId,ui32TiledRegionNo) \
		TALVMEM_SetTiledRegion(hMemSpace,ui32MmuContextId,ui32TiledRegionNo,0,0,0);

/*!
******************************************************************************

 @Function				TALVMEM_Poll32

 @Description

 This function is used to poll and wait for a specified value to be read from a word in
 a device memory block. This can be used to force synchronous operation within
 the host and is reflected in the PDUMP output.

 @Input		hMemSpace			: The memory space of the device memory.
 @Input		ui64DevVirtAddr		: Device virtual address
 @Input     ui32MmuContextId	: The MMU context id.
 @Input		ui32CheckFuncId		: The check function ID.
 @Input		ui32RequValue		: The required value.
 @Input		ui32Enable			: A mask to be applied to the value read before
									comparing with the required value.
 @Input		ui32PollCount		: Number of times to repeat the poll.
 @Input		ui32TimeOut			: The time-out value, in clock cycles, between polls.
 @Return	IMG_RESULT			: IMG_SUCCESS on poll passing, other errors are returned
									and asserts trap invalid parameters

******************************************************************************/
IMG_RESULT TALVMEM_Poll32(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
    IMG_UINT32                      ui32CheckFuncId,
    IMG_UINT32                      ui32RequValue,
    IMG_UINT32                      ui32Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
);

/*!
******************************************************************************

 @Function				TALVMEM_Poll64

 @Description

 This function is the 64bit equivalent of TALVMEM_Poll32

******************************************************************************/
IMG_RESULT TALVMEM_Poll64(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
    IMG_UINT32                      ui32CheckFuncId,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
);

/*!
******************************************************************************

 @Function              TALVMEM_ReadWord32

 @Description

 This function is used to read a 32 bit word value from device memory.

 @Input		hMemSpace				: The memory space of the device memory.
 @Input		ui64DevVirtAddr			: Device virtual address
 @Input		ui32MmuContextId		: The MMU context id.
 @Input		pui32Value				: A pointer used to return the read value.
 @Return	IMG_INT32				: IMG_SUCCESS if the memory space is supported by
									  the target. Asserts are use to trap unsupported
									  memory spaces.
******************************************************************************/
IMG_RESULT TALVMEM_ReadWord32(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_PUINT32						pui32Value
);

/*!
******************************************************************************

 @Function              TALVMEM_ReadWord64

 @Description

 This function is a 64bit version of TALVMEM_ReadWord32

******************************************************************************/
IMG_RESULT TALVMEM_ReadWord64(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                      ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_PUINT64						pui64Value
);

/*!
******************************************************************************

 @Function              TALVMEM_WriteWord32

 @Description

 This function is used to write a 32-bit word to a device memory.

 @Input		hMemSpace				: The memory space of the device memory
 @Input		ui64DevVirtAddr			: Device virtual address
 @Input		ui32MmuContextId		: The MMU context id.
 @Input		ui32Value				: The value to be written.
 @Return	IMG_RESULT				: IMG_SUCCESS if the memory space is supported by
									  the target. Asserts are use to trap unsupported
									  memory spaces.

******************************************************************************/
IMG_RESULT TALVMEM_WriteWord32(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
    IMG_UINT32                      ui32Value
);

/*!
******************************************************************************

 @Function              TALVMEM_WriteWord64

 @Description

 This function is a 64bit version of TALVMEM_WriteWord32

******************************************************************************/
IMG_RESULT TALVMEM_WriteWord64(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
    IMG_UINT64                      ui64Value
);

/*!
******************************************************************************

 @Function              TALVMEM_CircBufPoll32

 @Description

 This function is used to poll for space to become available in a
 circular buffer.

 @Input		hMemSpace			: The memory space of the device memory.
 @Input		ui64DevVirtAddr		: Device virtual address
 @Input     ui32MmuContextId	: The MMU context id.
 @Input		ui64WriteOffsetVal	: Circular buffer poll Write offset
 @Input		ui64PacketSize		: The size, in bytes, of the free space required.
 @Input		ui64BufferSize		: The total size, in bytes, of the circular buffer.
 @Input		ui32PollCount		: Number of times to repeat the poll.
 @Input		ui32TimeOut			: The time-out value, in clock cycles, between polls.
 @Return	IMG_RESULT			: IMG_SUCCESS on poll passing, other errors are returned
									and asserts trap invalid parameters

******************************************************************************/
IMG_RESULT TALVMEM_CircBufPoll32(
	IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT64						ui64WriteOffsetVal,
	IMG_UINT64						ui64PacketSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut
);

/*!
******************************************************************************

 @Function              TALVMEM_CircBufPoll64

 @Description

 This function is a 64bit version of TALVMEM_WriteWord32

******************************************************************************/
IMG_RESULT TALVMEM_CircBufPoll64(
	IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT64						ui64WriteOffsetVal,
	IMG_UINT64						ui64PacketSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut
);

/*!
******************************************************************************

 @Function				TALVMEM_UpdateHost

 @Description

 This function is used to copy a region of device memory using the device's
 virtual address to a host buffer.  If the device accesses the memory through
 an MMU then the memory copied will be as the device would view the memory
 through the MMU.

 @Input		hMemSpace			: The memory space of the device memory.
 @Input		ui64DevVirtAddr		: The device virtual address of the start of
									the region.
 @Input		ui32MmuContextId	: The MMU context id (0 to 9)
 @Input		ui64Size			: The size (in bytes) of the region to be copied.
 @Output	pvHostBuf			: A pointer to the host buffer.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces.

******************************************************************************/
IMG_RESULT TALVMEM_UpdateHost(
	IMG_HANDLE						hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT64						ui64Size,
	IMG_VOID *						pvHostBuf
);


/*!
******************************************************************************

 @Function				TALVMEM_DumpImage

 @Description

 This function is used to dump an image (pixel data and image header
 details).  The buffer pointer to by pvHostBuf is used to to obtain/save
 the pixel data.

 NOTE: The arguments for second and third planes are only required
 for pixel formats having 2 or 3 planes.  These should be set to
 0 or IMG_NULL for planes which are not valid for the pixel format.

 @Input		hMemSpace			: The memory space of the device memory.
 @Input     psImageHeaderInfo	: A pointer to a partially completed
									Image Header structure.
 @Input     psImageFilename		: A pointer to a string containing the image filename/path.
 @Input		ui64DevVirtAddr1	: The virtual address of the first plane of pixel data.
 @Input		ui32MmuContextId1	: The MMU context id (0 to 9) of the first plane.
 @Input		ui64Size1			: The size (in bytes) of the first plane.
 @Output	pvHostBuf1			: A pointer to the host buffer for the first plane.
 @Input		ui64DevVirtAddr2	: The virtual address of the second plane of pixel data.
 @Input		ui32MmuContextId2	: The MMU context id (0 to 9) of the second plane.
 @Input		ui64Size2			: The size (in bytes) of the second plane.
 @Output	pvHostBuf2			: A pointer to the host buffer for the second plane.
 @Input		ui64DevVirtAddr3	: The virtual address of the third plane of pixel data.
 @Input		ui32MmuContextId3	: The MMU context id (0 to 9) of the third plane.
 @Input		ui64Size3			: The size (in bytes) of the third plane.
 @Output	pvHostBuf3			: A pointer to the host buffer for the third plane.
 @Return	IMG_RESULT			: IMG_SUCCESS if the memory space is supported by
									the target. Asserts are use to trap unsupported
									memory spaces.

******************************************************************************/
IMG_RESULT TALVMEM_DumpImage(
	IMG_HANDLE						hMemSpace,
    TAL_sImageHeaderInfo *          psImageHeaderInfo,
    IMG_CHAR *                      psImageFilename,
	IMG_UINT64						ui64DevVirtAddr1,
	IMG_UINT32						ui32MmuContextId1,
	IMG_UINT64						ui64Size1,
	IMG_VOID *						pvHostBuf1,
	IMG_UINT64						ui64DevVirtAddr2,
	IMG_UINT32						ui32MmuContextId2,
	IMG_UINT64						ui64Size2,
	IMG_VOID *						pvHostBuf2,
	IMG_UINT64						ui64DevVirtAddr3,
	IMG_UINT32						ui32MmuContextId3,
	IMG_UINT64						ui64Size3,
	IMG_VOID *						pvHostBuf3
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the DEVIF group
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

#endif //__TALVMEM_H__



