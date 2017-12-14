/*!
******************************************************************************
 @file   : posted.c

 @brief

 @Author Tom Hollis

 @date   19/11/2008

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

 <b>Description:</b>\n
         Device interface functions for the wrapper.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/

#include <img_errors.h>
#include <reg_io2.h>

#include <stdio.h>

#include "devif_api.h"
#include "posted_regio.h"
#include <tconf.h>
#include "devif_config.h"

#define MAX_DEVICES	(20)
#define SUPPORT_REG_SIZE (0x100)

static DEVIF_sDeviceCB* gasDeviceIds[MAX_DEVICES];
static IMG_UINT32		gui32NoDevices = 0;


/*!
******************************************************************************

 @Function				PSTD_IF_ReadRegister

******************************************************************************/
IMG_UINT32 PSTD_IF_ReadRegister (
	IMG_UINT32	ui32DeviceId,
	IMG_UINT32	ui32Offset
)
{
	DEVIF_sDeviceCB * psDeviceCB = gasDeviceIds[ui32DeviceId];
	DEVIF_sDeviceCB * psSubDeviceCB = (DEVIF_sDeviceCB*)psDeviceCB->pvDevIfInfo;
	IMG_ASSERT(psSubDeviceCB->pfnReadRegister);
	return psSubDeviceCB->pfnReadRegister(psSubDeviceCB, ui32Offset);
}

/*!
******************************************************************************

 @Function				PSTD_IF_WriteRegister

******************************************************************************/
IMG_VOID PSTD_IF_WriteRegister (
	IMG_UINT32	ui32DeviceId,
	IMG_UINT32	ui32Offset,
	IMG_UINT32	ui32Value
)
{
	DEVIF_sDeviceCB * psDeviceCB = gasDeviceIds[ui32DeviceId];
	DEVIF_sDeviceCB * psSubDeviceCB = (DEVIF_sDeviceCB*)psDeviceCB->pvDevIfInfo;
	IMG_ASSERT(psSubDeviceCB->pfnReadRegister);
	psSubDeviceCB->pfnWriteRegister(psSubDeviceCB, ui32Offset, ui32Value);
}

/*!
******************************************************************************

 @Function				posted_GetStatus

******************************************************************************/
IMG_UINT32 posted_GetStatus (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	return REGIO_READ_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_CMD_STATUS);
}

/*!
******************************************************************************

 @Function				posted_CheckCmdBuffer

******************************************************************************/
IMG_UINT32 posted_CheckCmdBuffer (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT32			ui32CommandStatus
)
{
	IMG_UINT32 ui32Overflow;
	(void)psDeviceCB;

	ui32Overflow = REGIO_READ_FIELD(ui32CommandStatus, PSTD_IF, PSTD_CMD_STATUS, CMD_FIFO_OVERFLOW);
	IMG_ASSERT(ui32Overflow == 0);
	return REGIO_READ_FIELD(ui32CommandStatus, PSTD_IF, PSTD_CMD_STATUS, CMD_FIFO_FREE_SPACE);
}

/*!
******************************************************************************

 @Function				posted_CheckReturnBufferSize

******************************************************************************/
IMG_UINT32 posted_CheckReturnBufferSize (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT32			ui32CommandStatus
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	IMG_UINT32 ui32Overflow, ui32Underflow, ui32InFlight, ui32Waiting;

	// Check the buffer has not had an underflow or overflow
	ui32Overflow = REGIO_READ_FIELD(ui32CommandStatus, PSTD_IF, PSTD_CMD_STATUS, RDATA_FIFO_OVERFLOW);
	IMG_ASSERT(ui32Overflow == 0);
	ui32Underflow = REGIO_READ_FIELD(ui32CommandStatus, PSTD_IF, PSTD_CMD_STATUS, RDATA_FIFO_UNDERFLOW);
	IMG_ASSERT(ui32Underflow == 0);
	
	// Check how many items are waiting in the queue or in-flight
	ui32Waiting = REGIO_READ_FIELD(ui32CommandStatus, PSTD_IF, PSTD_CMD_STATUS, PSTD_RDATA_COUNT );
	ui32InFlight = REGIO_READ_FIELD(ui32CommandStatus, PSTD_IF, PSTD_CMD_STATUS, PSTD_READS_IN_FLIGHT);
	
	return pDeviceInfo->ui32BufferSize - ( ui32Waiting + ui32InFlight );
}

/*!
******************************************************************************

 @Function				posted_ReadWord

******************************************************************************/
IMG_UINT32 posted_ReadWord (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT32			ui32PciBar,
	IMG_UINT32			ui32Address
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	IMG_UINT32 ui32Value, ui32ReturnDataSize, ui32Status, ui32SetVal = 1;
	// Check Command and Return Buffers
	ui32Status = posted_GetStatus(psDeviceCB);
	while (posted_CheckCmdBuffer(psDeviceCB, ui32Status) < 1)
	{
		/// Wait?
		ui32Status = posted_GetStatus(psDeviceCB);
	}
	// There should be no data inflight or in the return buffer
	ui32ReturnDataSize = posted_CheckReturnBufferSize(psDeviceCB, ui32Status);
	IMG_ASSERT(ui32ReturnDataSize == pDeviceInfo->ui32BufferSize);

	// Post Command
	switch(ui32PciBar)
	{
	case 1:
		REGIO_WRITE_FIELD(ui32Address, PSTD_IF, PSTD_RD_ADDR, RD_B1_NB0, ui32SetVal);
	case 0:
		REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_RD_ADDR, ui32Address);
		break;
	case 2:
		REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_RD_B2_ADDR, ui32Address);
		break;
	default:
		printf("PCI Bar Reads limited to Bars 0, 1 and 2\n");
		IMG_ASSERT(IMG_FALSE);
	}
	// Wait for Return Data
	ui32Status = posted_GetStatus(psDeviceCB);
	ui32ReturnDataSize = REGIO_READ_FIELD(ui32Status, PSTD_IF, PSTD_CMD_STATUS, PSTD_RDATA_COUNT );
	while (ui32ReturnDataSize == 0)
	{
		/// Wait?
		ui32Status = posted_GetStatus(psDeviceCB);
		ui32ReturnDataSize = REGIO_READ_FIELD(ui32Status, PSTD_IF, PSTD_CMD_STATUS, PSTD_RDATA_COUNT );
	}

	IMG_ASSERT(ui32ReturnDataSize == 1);
	
	// Read from the data buffer
	ui32Value = REGIO_READ_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_RD_DATA);
	return ui32Value;
}

/*!
******************************************************************************

 @Function				posted_ReadWords

******************************************************************************/
IMG_VOID posted_ReadWords (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT32			ui32PciBar,
	IMG_UINT32			ui32Address,
	IMG_UINT32 *		pui32Data,
	IMG_UINT32			ui32Size
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	IMG_UINT32 ui32ReturnDataSize, ui32CmdBufferSize, ui32DataRead, ui32NoReads, ui32, ui32Status, ui32SetVal = 1;

	// Check Command and Return Buffers
	ui32Status = posted_GetStatus(psDeviceCB);
	ui32CmdBufferSize = posted_CheckCmdBuffer(psDeviceCB, ui32Status);
	while (ui32CmdBufferSize < 1)
	{
		ui32Status = posted_GetStatus(psDeviceCB);
		ui32CmdBufferSize = posted_CheckCmdBuffer(psDeviceCB, ui32Status);
		/// Wait?
	}

	// There should be no data inflight or in the return buffer
	ui32ReturnDataSize = posted_CheckReturnBufferSize(psDeviceCB, ui32Status);
	IMG_ASSERT(ui32ReturnDataSize == pDeviceInfo->ui32BufferSize);

	ui32DataRead = 0;
	while ( ui32DataRead < ui32Size )
	{
		// Get the size of the smallest buffer or the no of items left to read, whichever is smaller
		ui32NoReads = ui32CmdBufferSize < ui32ReturnDataSize ? ui32CmdBufferSize : ui32ReturnDataSize;
		ui32NoReads = ui32NoReads < ui32Size - ui32DataRead ? ui32NoReads : ui32Size - ui32DataRead;
		
		// Post Address Command
		switch(ui32PciBar)
		{
		case 1:
			REGIO_WRITE_FIELD(ui32Address, PSTD_IF, PSTD_WRT_ADDR, WRT_B1_NB0, ui32SetVal);
		case 0:
		case 2:
			break;
		default:
			printf("PCI Bar Reads limited to Bars 0, 1 and 2\n");
			IMG_ASSERT(IMG_FALSE);
		}
		
		for ( ui32 = 0 ; ui32 < ui32NoReads ; ui32++ )
		{
			switch(ui32PciBar)
			{
			case 0:
			case 1:
				REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_RD_ADDR, ui32Address + ui32DataRead);
				break;
			case 2:
				REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_RD_B2_ADDR, ui32Address + ui32DataRead);
				break;
			default:
				printf("PCI Bar Reads limited to Bars 0, 1 and 2\n");
				IMG_ASSERT(IMG_FALSE);
			}
			ui32DataRead++;
		}

		while ( ui32NoReads > 0 )
		{
			// Wait for data to come back
			ui32Status = posted_GetStatus(psDeviceCB);
			ui32ReturnDataSize = REGIO_READ_FIELD(ui32Status, PSTD_IF, PSTD_CMD_STATUS, PSTD_RDATA_COUNT );
			while (ui32ReturnDataSize == 0)
			{
				///Wait?
				ui32Status = posted_GetStatus(psDeviceCB);
				ui32ReturnDataSize = REGIO_READ_FIELD(ui32Status, PSTD_IF, PSTD_CMD_STATUS, PSTD_RDATA_COUNT );
			}
			// Read from the data buffer
			for ( ui32 = 0; ui32 < ui32ReturnDataSize ; ui32++ )
			{
				*pui32Data = REGIO_READ_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_RD_DATA);
				pui32Data++;
				ui32NoReads --;
			}
		}
	}
}

/*!
******************************************************************************

 @Function				posted_WriteWord

******************************************************************************/
IMG_VOID posted_WriteWord (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT32			ui32PciBar,
    IMG_UINT32          ui32Address,
    IMG_UINT32          ui32Value
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	// Check Command and Return Buffers
	IMG_UINT32 ui32Status = posted_GetStatus(psDeviceCB);
	IMG_UINT32 ui32SetVal = 1;
	while (posted_CheckCmdBuffer(psDeviceCB, ui32Status) < 1)
	{
		/// Wait?
		ui32Status = posted_GetStatus(psDeviceCB);
	}

	// Post Command
	switch(ui32PciBar)
	{
	case 1:
		REGIO_WRITE_FIELD(ui32Address, PSTD_IF, PSTD_WRT_ADDR, WRT_B1_NB0, ui32SetVal);
	case 0:
		REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_WRT_ADDR, ui32Address);
		break;
	case 2:
		REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_WRT_B2_ADDR, ui32Address);
		break;
	default:
		printf("PCI Bar Reads limited to Bars 0, 1 and 2\n");
		IMG_ASSERT(IMG_FALSE);
	}
	REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_WRT_DATA, ui32Value);
}

/*!
******************************************************************************

 @Function				posted_WriteWords

******************************************************************************/
IMG_VOID posted_WriteWords (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT32			ui32PciBar,
    IMG_UINT32          ui32Address,
    IMG_UINT32 *        pui32Values,
	IMG_UINT32			ui32Size
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	IMG_UINT32 ui32Status, ui32Written, ui32Space, ui32SetVal = 1;
	// Check Command and Return Buffers
	ui32Status = posted_GetStatus(psDeviceCB);
	ui32Space = posted_CheckCmdBuffer(psDeviceCB, ui32Status);
	ui32Written = 0;
	
	// Setup address, the rest will follow
	switch(ui32PciBar)
	{
	case 1:
		REGIO_WRITE_FIELD(ui32Address, PSTD_IF, PSTD_WRT_ADDR, WRT_B1_NB0, ui32SetVal);
	case 0:
		REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_WRT_ADDR, ui32Address);
		break;
	case 2:
		REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_WRT_B2_ADDR, ui32Address);
		break;
	default:
		printf("PCI Bar Reads limited to Bars 0, 1 and 2\n");
		IMG_ASSERT(IMG_FALSE);
	}

	while ( ui32Written < ui32Size )
	{
		// Check Command  Buffer
		while (ui32Space < 1)
		{
			ui32Status = posted_GetStatus(psDeviceCB);
			ui32Space = posted_CheckCmdBuffer(psDeviceCB, ui32Status);
			/// Wait?
		} 
		// Get either the buffer size or writes left, whichever is smaller
		ui32Space = ui32Space < ui32Size - ui32Written ? ui32Space : ui32Size - ui32Written;

		while ( ui32Space > 0 )
		{
			// Post Command
			REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_WRT_DATA, *pui32Values++);
			ui32Space--;
			ui32Written++;
		}

	}
	
}

/*!
******************************************************************************

 @Function				POSTED_Initialise

******************************************************************************/
IMG_RESULT POSTED_Initialise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	DEVIF_sDeviceCB * psSubDeviceCB = (DEVIF_sDeviceCB*)psDeviceCB->pvDevIfInfo;
	IMG_UINT32 ui32RegValue, ui32CommandStatus, ui32SetVal = 1;

	if (psDeviceCB->bInitialised == IMG_FALSE)
	{
		if (psSubDeviceCB->pfnInitailise && psSubDeviceCB->bInitialised == IMG_FALSE)
			psSubDeviceCB->pfnInitailise(psSubDeviceCB);

		IMG_MEMCPY(&gasDeviceIds[pDeviceInfo->ui32DeviceId], psDeviceCB, sizeof(DEVIF_sDeviceCB));
		// Reset the post FIFO etc
		ui32RegValue = 0;

		REGIO_WRITE_FIELD(ui32RegValue, PSTD_IF, PSTD_LOGIC_RST, DUT_RESET, ui32SetVal); // Added because we can't do DUT reset here in testing
	
		REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_LOGIC_RST, ui32RegValue);

		REGIO_WRITE_FIELD(ui32RegValue, PSTD_IF, PSTD_LOGIC_RST, LOGIC_RESET, ui32SetVal);
		//REGIO_WRITE_FIELD(ui32RegValue, PSTD_IF, PSTD_LOGIC_RST, DUT_RESET, 1);

		REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_LOGIC_RST, ui32RegValue);

		// Read the command status register and get the buffer size
		ui32CommandStatus = REGIO_READ_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_CMD_STATUS);
		pDeviceInfo->ui32BufferSize = REGIO_READ_FIELD(ui32CommandStatus, PSTD_IF, PSTD_CMD_STATUS, CMD_FIFO_FREE_SPACE);
		IMG_ASSERT(pDeviceInfo->ui32BufferSize > 0);
		psDeviceCB->bInitialised = IMG_TRUE;
	}
	return IMG_SUCCESS;
}

/*!
******************************************************************************
s
 @Function				POSTED_ReadRegister

******************************************************************************/
IMG_UINT32 POSTED_ReadRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	return posted_ReadWord(psDeviceCB, pDeviceInfo->ui32RegBar, IMG_UINT64_TO_UINT32(ui64RegAddress) + pDeviceInfo->ui32RegOffset);
}



/*!
******************************************************************************

 @Function				POSTED_WriteRegister

******************************************************************************/
IMG_VOID POSTED_WriteRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	posted_WriteWord(psDeviceCB, pDeviceInfo->ui32RegBar, IMG_UINT64_TO_UINT32(ui64RegAddress) + pDeviceInfo->ui32RegOffset, ui32Value);
}



/*!
******************************************************************************

 @Function				POSTED_ReadMemory

******************************************************************************/
IMG_UINT32 POSTED_ReadMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	return posted_ReadWord(psDeviceCB, pDeviceInfo->ui32MemBar, IMG_UINT64_TO_UINT32(ui64DevAddress) + pDeviceInfo->ui32MemOffset);
}



/*!
******************************************************************************

 @Function				POSTED_WriteMemory

******************************************************************************/
IMG_VOID POSTED_WriteMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	posted_WriteWord(psDeviceCB, pDeviceInfo->ui32MemBar, IMG_UINT64_TO_UINT32(ui64DevAddress) + pDeviceInfo->ui32MemOffset, ui32Value);
}

/*!
******************************************************************************

 @Function				POSTED_CopyDeviceToHost

******************************************************************************/
IMG_VOID POSTED_CopyDeviceToHost (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	// Check that the write rounds to a whole number of words
	IMG_ASSERT((ui32Size & 3) == 0);
	posted_ReadWords(psDeviceCB, pDeviceInfo->ui32MemBar, IMG_UINT64_TO_UINT32(ui64DevAddress) + pDeviceInfo->ui32MemOffset, (IMG_PUINT32)pvHostAddress, ui32Size / 4);
}



/*!
******************************************************************************

 @Function				POSTED_CopyHostToDevice

******************************************************************************/
IMG_VOID POSTED_CopyHostToDevice (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	// Check that the write rounds to a whole number of words
	IMG_ASSERT((ui32Size & 3) == 0);
	posted_WriteWords(psDeviceCB, pDeviceInfo->ui32MemBar, IMG_UINT64_TO_UINT32(ui64DevAddress) + pDeviceInfo->ui32MemOffset, (IMG_PUINT32)pvHostAddress, ui32Size / 4);

}

/*!
******************************************************************************

 @Function				POSTED_Deinitialise

******************************************************************************/
IMG_VOID POSTED_Deinitialise (
	DEVIF_sDeviceCB *   psDeviceCB
	)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	DEVIF_sDeviceCB * psSubDeviceCB = (DEVIF_sDeviceCB*)psDeviceCB->pvDevIfInfo;
	IMG_UINT32 ui32RegValue = 0, ui32SetVal = 1, ui32ClearVal = 0;
	
	// Reset the post FIFO etc
	REGIO_WRITE_FIELD(ui32RegValue, PSTD_IF, PSTD_LOGIC_RST, LOGIC_RESET, ui32ClearVal);
	REGIO_WRITE_FIELD(ui32RegValue, PSTD_IF, PSTD_LOGIC_RST, DUT_RESET, ui32SetVal);

	REGIO_WRITE_REGISTER(pDeviceInfo->ui32DeviceId, PSTD_IF, PSTD_LOGIC_RST, ui32RegValue);

	if (psSubDeviceCB->pfnDeinitailise && psSubDeviceCB->bInitialised == IMG_TRUE)
		psSubDeviceCB->pfnDeinitailise(psDeviceCB);
}

/*!
******************************************************************************

 @Function				POSTED_RegWriteWords

******************************************************************************/
IMG_VOID POSTED_RegWriteWords (
	DEVIF_sDeviceCB		* psDeviceCB,
	IMG_VOID			* pui32MemSource,
	IMG_UINT32			ui32Offset,
	IMG_UINT32    		ui32WordCount
)
{
	DEVIF_sPostIFInfo * pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
	// Check that the write rounds to a whole number of words
	posted_WriteWords(psDeviceCB, pDeviceInfo->ui32RegBar, ui32Offset + pDeviceInfo->ui32RegOffset, (IMG_PUINT32)pui32MemSource, ui32WordCount);
}

/*!
******************************************************************************

 @Function				POSTED_DEVIF_ConfigureDevice

******************************************************************************/
IMG_VOID POSTED_DEVIF_ConfigureDevice(
    DEVIF_sDeviceCB *		psDeviceCB
)
{
	// Prepare PCI subdevice
	DEVIF_sDeviceCB * psSubDeviceCB = (DEVIF_sDeviceCB*)IMG_MALLOC(sizeof(DEVIF_sDeviceCB));
	DEVIF_sPciInfo * psPciInfo = &psSubDeviceCB->sDevIfSetup.u.sPciInfo;
	DEVIF_sPostIFInfo * psPostIFInfo = &psDeviceCB->sDevIfSetup.u.sPostIFInfo;
		
	IMG_MEMSET(psPciInfo, 0, sizeof(DEVIF_sPciInfo));
	// Set up posted registers
	psPciInfo->ui32RegBar = 0;
	psPciInfo->ui32RegOffset = psPostIFInfo->ui32PostedIfOffset;
	psPciInfo->ui32RegSize = SUPPORT_REG_SIZE;
	
	
	PCIIF_DevIfConfigureDevice(psSubDeviceCB);
	psDeviceCB->pvDevIfInfo = (IMG_VOID*)psSubDeviceCB;

	psDeviceCB->bInitialised        = IMG_FALSE;  
	psDeviceCB->pfnInitailise       = POSTED_Initialise;
	psDeviceCB->pfnDeinitailise     = POSTED_Deinitialise;
	psDeviceCB->pfnReadRegister     = POSTED_ReadRegister;
	psDeviceCB->pfnWriteRegister    = POSTED_WriteRegister;
	psDeviceCB->pfnReadMemory       = POSTED_ReadMemory;
	psDeviceCB->pfnWriteMemory      = POSTED_WriteMemory;
	psDeviceCB->pfnCopyDeviceToHost = POSTED_CopyDeviceToHost;
	psDeviceCB->pfnCopyHostToDevice = POSTED_CopyHostToDevice;

	psPostIFInfo->ui32DeviceNo = gui32NoDevices;
	gasDeviceIds[gui32NoDevices++] = psDeviceCB;

}
