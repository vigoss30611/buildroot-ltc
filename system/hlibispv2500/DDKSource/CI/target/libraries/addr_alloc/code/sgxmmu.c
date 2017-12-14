/*! 
******************************************************************************
 @file   : sgxmmu.c

 @brief  

 @Author Bojan Lakicevic

 @date   18/04/2006
 
         <b>Copyright 2006 by Imagination Technologies Limited.</b>\n
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
         SGX MMU code for wrapper for PDUMP tools.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/

#include <sgxmmu.h>
#include <tconf.h>
#include <tal.h>
#include <stdio.h>

	
#define EUR_CR_BIF_DIR_LIST_BASE0   0x0C84
#define EUR_CR_BIF_DIR_LIST_BASE1   0x0C38

#define EUR_CR_BIF_BANK_SET			0x0C74
#define EUR_CR_BIF_BANK0			0x0C78
#define EUR_CR_BIF_BANK1			0x0C7C

#define MASK_CR_BIF_BANK_SET_SELECT		0x00000030
#define SHIFT_CR_BIF_BANK_SET_SELECT	4

#define MASK_CR_BIF_BANK_INDEX			0x00000F00
#define SHIFT_CR_BIF_BANK_INDEX			8


#define MASK_MMU_TABLE_TAG			0xFFC00000
#define SHIFT_MMU_TABLE_TAG			22

#define MASK_MMU_TABLE_INDEX		0x003FF000
#define SHIFT_MMU_TABLE_INDEX		12

#define MASK_MMU_ADDR_OFFSET		0x00000FFF
#define SHIFT_MMU_ADDR_OFFSET		0

#define MASK_MMU_PAGE_START_ADDR	0xFFFFF000

#define F_MASK(basename)  (MASK_##basename)
#define F_SHIFT(basename) (SHIFT_##basename)

#define F_EXTRACT(val,basename) (((val)&(F_MASK(basename)))>>(F_SHIFT(basename)))

static	IMG_BOOL	bIsNewImage = IMG_TRUE;

static	IMG_BOOL	bInitialised = IMG_FALSE;

/*!
******************************************************************************

 @Function				SGXMMU_Initialise

******************************************************************************/
IMG_VOID SGXMMU_Initialise(void)
{
    if (!bInitialised)
    {
        bInitialised = IMG_TRUE;
    }
}

/*!
******************************************************************************

 @Function              SGXMMU_SetUpTranslation

******************************************************************************/
IMG_VOID SGXMMU_SetUpTranslation(
    TAL_psMemSpaceInfo  psMemSpaceInfo, 
    SGXMMU_eSGXMMUType  eSGXMMUType, 
    IMG_UINT32          ui32DevVirtQualifier
)
{
    IMG_UINT32              ui32RegBaseAddr = IMG_UINT64_TO_UINT32(psMemSpaceInfo->sRegInfo.ui64RegBaseAddr);
    DEVIF_sDeviceCB *       psDevIfDeviceCB = &psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;

	if (eSGXMMUType == SGXMMU_TYPE_SGX535)
	{
		if (ui32DevVirtQualifier != TAL_DEV_VIRT_QUALIFIER_NONE)
		{
			IMG_UINT32	ta_3d_banks, bank_set_select, bank_set_select0, bank_set_select1, bank_set_select2, indices0, indices1, host_bank, bank, bank_bits, index, new_indices; 

			/* Read bank status register for TA and 3D banks */
            ta_3d_banks = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (0x00000CB4 + ui32RegBaseAddr));

			/* Read bank_set_select register */
			bank_set_select = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (0x00000C74 + ui32RegBaseAddr));
			
			bank_set_select0 =	bank_set_select & 0x0000000F;		/* bits 0-3  */
			bank_set_select1 = (bank_set_select>>4) & 0x0000000F;	/* bits 4-7  */
			bank_set_select2 = (bank_set_select>>8) & 0x0000000F;	/* bits 8-11 */


			/* Read Index fields for each bank */
			indices0 = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (0x00000C78 + ui32RegBaseAddr));
			indices1 = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (0x00000C7C + ui32RegBaseAddr));

			host_bank = (bank_set_select1 & 0x00000002)>>1;		/* bit 1 */
				
			switch (ui32DevVirtQualifier)
			{
				/* read necessary bif registers to determine how to set up registers for host access */
				case 0:
				{ /* Vertex(TA) */
					
					bank_bits =	ta_3d_banks & 0x0000000F;		/* bits 0-3  */
					bank = (bank_bits & 0x00000001);			/* bit 0 */
					
					if (bank == 0)
					{
						index = (indices0>>4) & 0x0000000F;		/* bits 4-7 */
					} 
					else 
					{  
						index = (indices1>>4) & 0x0000000F;		/* bits 4-7 */
					}
					break;
				}
				case 1: 
				{ /* Pixel(3D) */

					bank_bits =	ta_3d_banks & 0x0000000F;		/* bits 0-3  */
					bank = (bank_bits & 0x00000002)>>1;			/* bit 1 */

					if (bank == 0) 
					{
						index = (indices0>>12) & 0x0000000F;	/* bits 12-15 */
					} 
					else 
					{ 
						index = (indices1>>12) & 0x0000000F;	/* bits 12-15 */
					}
					break;
				}
				case 2:
				{ /* 2D */

					bank = (bank_set_select0 & 0x00000002)>>1;	/* bit 1 */

					if (bank == 0) 
					{
						index = (indices0>>16) & 0x0000000F;	/* bits 16-19 */
					} 
					else 
					{ 
						index = (indices1>>16) & 0x0000000F;	/* bits 16-19 */
					}
					break;
				}
				case 3: 
				{ /* EDM */

					bank = bank_set_select2 & 0x00000001;		/* bit 0 */

					if (bank == 0) 
					{
						index = indices0 & 0x0000000F;			/* bits 0-3 */
					} 
					else 
					{  
						index = indices1 & 0x0000000F;			/* bits 0-3 */
					}
					break;
				}
				case 4: 
				{ /* HOST */
					if (host_bank == 0)
					{
						index = (indices0>>8) & 0x0000000F;		/* bits 8-11 */
					} 
					else 
					{ 
						index = (indices1>>8) & 0x0000000F;		/* bits 8-11 */
					}
					break;
				}
				default: 
				{
					index = 0;
				}
			}  /* end of Switch */

			/* Write host index and bank to ensure the desired directory base address is selected */
			if (host_bank == 0) 
			{			
				new_indices = (indices0 & 0x0000F000) | ((index & 0x0000000F)<<8) | (indices0 & 0x000000FF);
				psDevIfDeviceCB->pfnWriteRegister(psDevIfDeviceCB, (0x00000C78 + ui32RegBaseAddr), new_indices);
			} 
			else 
			{	  
				new_indices = (indices1 & 0x0000F000) | ((index & 0x0000000F)<<8) | (indices1 & 0x000000FF);
				psDevIfDeviceCB->pfnWriteRegister(psDevIfDeviceCB, (0x00000C7C + ui32RegBaseAddr), new_indices);
			}
		}
	}

	bIsNewImage = IMG_TRUE; /* Reset the new image flag... this signals the MMU translation s/w that the translation is to be started for a new image */
}


/*!
******************************************************************************

 @Function              SGXMMU_CoreTranslation

******************************************************************************/
IMG_UINT32 SGXMMU_CoreTranslation(
    TAL_psMemSpaceInfo      psMemSpaceInfo, 
    SGXMMU_eSGXMMUType      eSGXMMUType, 
    IMG_UINT32              uInAddr
)
{
	static	IMG_UINT32	    uiDirectoryAddrBase = 0;
    IMG_UINT32              ui32RegBaseAddr = IMG_UINT64_TO_UINT32(psMemSpaceInfo->sRegInfo.ui64RegBaseAddr);
    DEVIF_sDeviceCB *       psDevIfDeviceCB = &psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;

	IMG_UINT32 uOutAddr, uTableIndex, uTableTag, uAddrOffset, uiDirectoryAddr, uTableAddr, uTableEntry;
	IMG_UINT32 ui32Temp, uiDirListIndex = 0, uiBankSelect;

	/* Get the tag, index and offset values. */
	uTableTag = F_EXTRACT(uInAddr, MMU_TABLE_TAG);
	uTableIndex = F_EXTRACT(uInAddr, MMU_TABLE_INDEX);
	uAddrOffset = uInAddr & MASK_MMU_ADDR_OFFSET;

	if (bIsNewImage)
	{
		if (eSGXMMUType == SGXMMU_TYPE_SGX535)
		{
			/* Work out which directory list to use. */
			ui32Temp = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (EUR_CR_BIF_BANK_SET + ui32RegBaseAddr));
			uiBankSelect = F_EXTRACT(ui32Temp, CR_BIF_BANK_SET_SELECT);

			if (uiBankSelect == 0)
			{
				ui32Temp = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (EUR_CR_BIF_BANK0 + ui32RegBaseAddr));
				uiDirListIndex = F_EXTRACT(ui32Temp, CR_BIF_BANK_INDEX);
			}
			else if (uiBankSelect == 3)
			{
				ui32Temp = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (EUR_CR_BIF_BANK1 + ui32RegBaseAddr));
				uiDirListIndex = F_EXTRACT(ui32Temp, CR_BIF_BANK_INDEX);
			}
			else
			{
				IMG_ASSERT(0);
			}

			/* Read the directory list start address from the register. */
			if (uiDirListIndex == 0)
				uiDirectoryAddrBase = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (EUR_CR_BIF_DIR_LIST_BASE0 + ui32RegBaseAddr));
			else
				uiDirectoryAddrBase  = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, ((EUR_CR_BIF_DIR_LIST_BASE1 + ((uiDirListIndex - 1) << 2)) + ui32RegBaseAddr));
		}
		else
		{
			/* Read the directory list start address from the register. */
			uiDirectoryAddrBase = psDevIfDeviceCB->pfnReadRegister(psDevIfDeviceCB, (EUR_CR_BIF_DIR_LIST_BASE0 + ui32RegBaseAddr));
		}

		bIsNewImage = IMG_FALSE;
	}
		
	/* Add tag to the directory list start address */
	uiDirectoryAddr = uiDirectoryAddrBase + uTableTag * sizeof(IMG_UINT32);

	/* Read the page table start address. */
//	uTableAddr = PCIIF_DeviceRead32(DEVICE_MEMORY_BASE_HOST + uiDirectoryAddr);
    uTableAddr = psDevIfDeviceCB->pfnReadMemory(psDevIfDeviceCB, uiDirectoryAddr);

	/*
		Check for bit 0 valid.
	*/
	if(!(uTableAddr & 0x1))
	{
		printf("ERROR: CoreMMUTranslation(VirtAddr = 0x%08X) - Page table start address check failed, DirectoryAddr = 0x%08X, TableAddr = 0x%08X\n", uInAddr, uiDirectoryAddr, uTableAddr);
		IMG_ASSERT(0);
	}

	/*
		Removed bottom nibble as these are control data.
	*/
	uTableAddr &= (IMG_UINT32)(~0x0F);

	/*
		Add in offset.
	*/
	uTableAddr += uTableIndex * sizeof(IMG_UINT32);

	/* Read the translated address from the memory of page table. */
//	uTableEntry = PCIIF_DeviceRead32(DEVICE_MEMORY_BASE_HOST + uTableAddr);
    uTableEntry = psDevIfDeviceCB->pfnReadMemory(psDevIfDeviceCB, uTableAddr);

	/*
		Check for bit 0 valid.
	*/
	if(!(uTableEntry & 0x1))
	{
		printf("ERROR: CoreMMUTranslation(VirtAddr = 0x%08X) - Table entry check failed, TableAddr = 0x%08X, TableEntry = 0x%08X\n", uInAddr, uTableAddr, uTableEntry);
		IMG_ASSERT(0);
	}
	
	uOutAddr = (uTableEntry & MASK_MMU_PAGE_START_ADDR) + uAddrOffset;

	return uOutAddr;
}
