/*!
******************************************************************************
 @file   : mmu.h

 @brief  API for virtual memory decoding

 @Author Tom Hollis

 @date   24/04/2009

         <b>Copyright 2009 by Imagination Technologies Limited.</b>\n
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
         This header file contains virtual memory/address access functions

		 NOTE: This API is not thread safe, it does not protect itself
		 using mutexes etc. for performance reasons.  It assumes that
		 the caller or the context in which it is use will provded the
		 necessary protection.

 <b>Platform:</b>\n
	     Implementations will be platform dependent.

 @Version
	     -	1.0

******************************************************************************/
/*
******************************************************************************
*/

#if !defined (__MMU_H__)
#define __MMU_H__

#if defined (__cplusplus)
extern "C" {
#endif

#include <img_types.h>

/*! The number of Page Directory Tables */
#define MMU_NUMBER_OF_CONTEXTS (32)

//! Details of the type of mmu and tiling mode combined
//! These two should be seperate, but for historical reasons are combined
typedef enum 
{
	MMU_TYPE_NONE,							//!< This will simply return the given address
	MMU_TYPE_4K_PAGES_32BIT_256B_TILING,	//!< Fixed 4K page size MMU
	MMU_TYPE_VARIABLE_PAGE_SIZE_32BIT,		//!< Variable page size MMU
	MMU_TYPE_4K_PAGES_36BIT_512B_TILING,	//!< Fixed 4K page size MMU with 36bit addresses and 512B Tiling
	MMU_TYPE_4K_PAGES_32BIT_512B_TILING,	//!< Fixed 4K page size but with 512B Tiling
	MMU_TYPE_4K_PAGES_36BIT_256B_TILING,	//!< Fixed 4K page size MMU with 36bit addresses and 256B Tiling
	MMU_TYPE_VP_40BIT,						//!< Variable page size MMU with 40bit virtual and device addressing
	MMU_TYPE_4K_PAGES_40BIT_512B_TILING,	//!< Fixed 4K page size MMU with 40bit addresses and 512B Tiling
	MMU_TYPE_4K_PAGES_40BIT_256B_TILING,	//!< Fixed 4K page size MMU with 40bit addresses and 256B Tiling

	/* Placeholder */
	MMU_NO_TYPES
}MMU_Types;

//! For backwards compatibility
#define MMU_TYPE_INVALID  MMU_NO_TYPES

/*!
******************************************************************************

 @Function				MMU_pfnReadMem

 @Description

 This function prototype reads data from a device.

 @Input		ui64Addr	: The address to read.

 @Input		ui32Size	: The amount of data to read in bytes.

 @Input		pui8Buffer	: A pointer to a buffer into which to write the data.

 @Return	IMG_RESULT	: This function returns either IMG_SUCCESS or an
                          error code.

******************************************************************************/
typedef IMG_RESULT ( * MMU_pfnReadMem ) (
	IMG_UINT64						ui64Addr,
	IMG_UINT32						ui32Size,
	IMG_UINT8 *						pui8Buffer
);
/*!
******************************************************************************

 @Function				MMU_Initialise

 @Description

 This function initialises the MMU data.

 @Return	none.

******************************************************************************/
extern IMG_VOID MMU_Initialise();

/*!
******************************************************************************

 @Function				MMU_ReadVmem

 @Description

 This function reads the data from a virtual memory location.

 @Input		ui64VirtAddr: A virtual address to read.

 @Input		ui64Size	: The amount of data to read in bytes.

 @Input		pui32Values	: A pointer to a buffer into which to write the data.

 @Input		ui32MMUId	: The Id of the MMU, which must be already set up.

 @Return	IMG_RESULT	: This function returns either IMG_SUCCESS or an
                          error code.

******************************************************************************/
extern IMG_RESULT MMU_ReadVmem (
	IMG_UINT64						ui64VirtAddr,
	IMG_UINT64						ui64Size,
	IMG_UINT32 *					pui32Values,
	IMG_UINT32						ui32MMUId
);

/*!
******************************************************************************

 @Function				MMU_VmemToPhysMem

 @Description

 This function returns the real address of a virtual address.

 @Input		ui64VirtAddr: A virtual address to translate.

 @Input		pui64Value	: A pointer to a long word into which to write the address.

 @Input		ui32MMUId	: The Id of the MMU, which must be already set up.

 @Return	IMG_RESULT	: This function returns either IMG_SUCCESS or an
                          error code.

******************************************************************************/
extern IMG_RESULT MMU_VmemToPhysMem (
	IMG_UINT64						ui64VirtAddr,
	IMG_UINT64 *					pui64Value,
	IMG_UINT32						ui32MMUId
);

/*!
******************************************************************************

 @Function				MMU_GetPCEntryAddress

 @Description

 This function decodes a virtual address to give the Address
 of the page Catalogue entry.

 @Input		ui32MMUId		: The Id of the MMU context.

 @Input		ui64DevVirtAddr	: The virtual address to decode.

 @Return	IMG_UINT64		: The physical address of the page
								directory entry.

******************************************************************************/
extern IMG_UINT64 MMU_GetPCEntryAddress (
	IMG_UINT32				ui32MMUId,
	IMG_UINT64              ui64DevVirtAddr
);

/*!
******************************************************************************

 @Function				MMU_GetPageDirEntryAddress

 @Description

 This function decodes a virtual address to give the Address
 of the page directory entry.

 @Input		ui32MMUId		: The Id of the MMU context.

 @Input		ui64DevVirtAddr	: The virtual address to decode.

 @Input		ui64PCEntry	: The entry from the Page Directory
								Pointer Table (ignored if not req.)

 @Return	IMG_UINT64		: The physical address of the page
								directory entry.

******************************************************************************/
extern IMG_UINT64 MMU_GetPageDirEntryAddress (
	IMG_UINT32				ui32MMUId,
	IMG_UINT64              ui64DevVirtAddr,
	IMG_UINT64				ui64PCEntry
);

/*!
******************************************************************************

 @Function				MMU_GetPageTableEntryAddress

 @Description

 This function decodes a virtual address and page directory
 entry to give the Address of the page table entry.

 @Input		ui32MMUId		: The Id of the MMU context.

 @Input		ui64DevVirtAddr	: The virtual address to decode.

 @Input		ui64PageDirEntry: The associate Page directory entry.

 @Return	IMG_UINT64		: The physical address of the page
								table entry.

******************************************************************************/
extern IMG_UINT64 MMU_GetPageTableEntryAddress (
	IMG_UINT32				ui32MMUId,
	IMG_UINT64              ui64DevVirtAddr,
	IMG_UINT64				ui64PageDirEntry
);

/*!
******************************************************************************

 @Function				MMU_GetPageAddress

 @Description

 This function decodes a virtual address and page table
 entry to give the physical translation of a virtual address.

 @Input		ui32MMUId		: The Id of the MMU context.

 @Input		ui64DevVirtAddr	: The virtual address to decode.

 @Input		ui64PageTabEntry: The associate Page table entry.

 @Return	IMG_UINT64		: The physical translation of the
								virtual address.

******************************************************************************/
extern IMG_UINT64 MMU_GetPageAddress (
	IMG_UINT32				ui32MMUId,
	IMG_UINT64              ui64DevVirtAddr,
	IMG_UINT64				ui64PageTabEntry
);

/*!
******************************************************************************

 @Function				MMU_IsTiled

 @Description

 This function checks to see if a given virtual address is in a tiled region.

 @Input		ui32MMUId		: The Id of the MMU context.

 @Input		ui64DevVirtAddr	: The virtual address to check.

 @Return	IMG_BOOL		: True if the virtual address is in a tiled region.

******************************************************************************/
extern IMG_BOOL MMU_IsTiled(
    IMG_UINT32				ui32MMUId,
    IMG_UINT64              ui64DevVirtAddr
);

/*!
******************************************************************************

 @Function				MMU_SetTiledRegion
 
 @Description

 This function defines a region of memory as tiled.

 @Input		ui32MMUId			: The MMU identifier.

 @Input		ui32TiledRegionNo	: The Tiled region identifier.

 @Input		ui64DevVirtAddr		: The start address of the region.

 @Input		ui64Size			: The size of the region in bytes.

 @Input		ui32XTileStride		: The X tile stride of the region.

 @Return	none.

******************************************************************************/

extern IMG_VOID MMU_SetTiledRegion(
	IMG_UINT32				ui32MMUId,
    IMG_UINT32              ui32TiledRegionNo,
	IMG_UINT64				ui64DevVirtAddr,
    IMG_UINT64              ui64Size,
    IMG_UINT32              ui32XTileStride
);

/*!
******************************************************************************

 @Function				MMU_QuickTileAddress

 @Description

 This function takes a virtual address and returns the tile address.

 @Input		ui32XTileStride		: The X Tile Stride

 @Input		ui32MMUType			: The MMU Type.

 @Input		ui64DevVirtAddr		: The Address to be tiled.

 @Return	IMG_UINT32			: The tiled address.

******************************************************************************/
IMG_UINT64  MMU_QuickTileAddress(
    IMG_UINT32				ui32XTileStride,
	IMG_UINT32				ui32MMUType,
    IMG_UINT64              ui64DevVirtAddr
);

/*!
******************************************************************************

 @Function				MMU_TileAddress

 @Description

 This function takes a virtual address and returns the tile address.

 @Input		ui32MMUId			: The MMU identifier.

 @Input		ui64DevVirtAddr		: The virtual address to be tiled.

 @Return	IMG_UINT32			: The tiled virtual address.

******************************************************************************/
extern IMG_UINT64  MMU_TileAddress(
    IMG_UINT32				ui32MMUId,
    IMG_UINT64              ui64DevVirtAddr
);

/*!
******************************************************************************

 @Function				MMU_TileSize

 @Description

 This calculates the size of tiles for a given virtual address.

 @Input		ui32MMUId			: The MMU identifier.

 @Input		ui64DevVirtAddr		: The virtual address.

 @Return	IMG_UINT32			: The tile size or 0 if no tiling is found.

******************************************************************************/
extern IMG_UINT32  MMU_TileSize(
    IMG_UINT32				ui32MMUId,
    IMG_UINT64              ui64DevVirtAddr
);

/*!
******************************************************************************

 @Function				MMU_ContextsDefined

 @Description

 This function returns the number of MMU contexts currently defined.

 @Return	IMG_UINT32		: The number of currently defined MMU contexts.

******************************************************************************/
extern IMG_UINT32 MMU_ContextsDefined ();

/*!
******************************************************************************

 @Function				MMU_Setup

 @Description

 This function stores the settings for an MMU.

 @Input		ui32MMUId		: The Id of the MMU.

 @Input		ui64PCAddress	: The address of the Page Directory or Page Catalogue.

 @Input		ui32MMUType		: The MMU Type.

 @Input		pfnReadMem		: A Read function for reading device memory.

 @Return	IMG_RESULT		: This function returns a result.

******************************************************************************/
extern IMG_RESULT MMU_Setup (
	IMG_UINT32		ui32MMUId,
	IMG_UINT64		ui64PCAddress,
	IMG_UINT32		ui32MMUType,
	MMU_pfnReadMem	pfnReadMem
);

/*!
******************************************************************************

 @Function				MMU_Clear

 @Description

 This function clears the settings for an MMU.

 @Input		ui32MMUId		: The Id of the MMU to Clear.

 @Return	none.

******************************************************************************/
extern IMG_VOID MMU_Clear (
	IMG_UINT32		ui32MMUId
);

/*!
******************************************************************************

 @Function				MMU_PageReadCallback

 @Description

 This function allows a seperate page read callback to be added
 (This function was added for the purposes of providing a list 
 of the reads needed for a given memory read without having
 to perform the read).

 @Input		ui32MMUId		: The Id of the MMU.

 @Input		pfnReadMem		: A Read function for reading device memory.

 @Return	IMG_RESULT		: This function returns a result.

******************************************************************************/
extern IMG_RESULT MMU_PageReadCallback (
	IMG_UINT32		ui32MMUId,
	MMU_pfnReadMem	pfnReadMem
);



#if defined (__cplusplus)
}
#endif


#endif
