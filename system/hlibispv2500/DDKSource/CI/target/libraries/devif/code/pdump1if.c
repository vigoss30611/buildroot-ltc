/*!
******************************************************************************
 @file   : pdump1if.c

 @brief

 @Author Tom Hollis

 @date   11/12/2007

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
         pdump1 interface for the wrapper for PDUMP tools.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/
/*
******************************************************************************
*/

#include <img_errors.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>
	#ifdef WINCE
		#include <wince_stdio.h>
	#endif
#else
#include <unistd.h>
#endif
#include <tconf.h>

#define MAX_FILENAME		25
#define POLL_TIME_MS		1000
#define POLL_CMD_TIME_MS	100
#define MIN_POLL_TIME_S		300

/*!
******************************************************************************
  Local PDump command type.
******************************************************************************/
typedef enum
{
	PDUMP_CMD_WRITE,
	PDUMP_CMD_LOAD,
	PDUMP_CMD_SAVE,
	PDUMP_CMD_IDLE,
	PDUMP_CMD_POLL,
	PDUMP_CMD_START,
	PDUMP_CMD_END,

} DEVIF_PDumpCmd;

static IMG_VOID PDUMP1IF_commandManager (
			DEVIF_sDeviceCB *	PSDeviceCB,
			DEVIF_PDumpCmd		PDumpCmd,
			IMG_CHAR *			cpVariableStr
);

static IMG_VOID PDUMP1IF_sendCommand(DEVIF_sDeviceCB *	PSDeviceCB);
static IMG_VOID PDUMP1IF_addCommand (IMG_CHAR * caCmdStr);
static IMG_VOID PDUMP1IF_ReadData(DEVIF_sDeviceCB * psDeviceCB, IMG_VOID * vpData, IMG_UINT32 ui32NoBytes);
static IMG_VOID PDUMP1IF_WriteData(IMG_VOID * vpData, IMG_UINT32 ui32NoBytes);
static IMG_VOID PDUMP1IF_OpenDataReturn();

static IMG_CHAR *	PDUMP1IF_caSendData;
static IMG_CHAR *	PDUMP1IF_caReturnData;
static IMG_CHAR *	PDUMP1IF_caCommandFile;

static IMG_UINT32	ui32DataOffset;			/* current offset in the data file */
static IMG_UINT32	ui32CmdNum;				/* Command Counter, from 1 to FFFFFF. 0 = start, FFFFFFFF = end */
static FILE *		fCommandFile;			/* Command Script File */
static FILE *		fSendData;				/* Data File for sending */
static FILE *		fReturnData;			/* Data File for receiving */
static IMG_UINT32	ui32LineCount;			/* Counts number of lines sent */
static IMG_UINT32	ui32Wait;				/* Time to wait for commands and data to return */


/*!
******************************************************************************

 @Function				PDUMP1IF_Initailise

******************************************************************************/
IMG_RESULT PDUMP1IF_Initialise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	DEVIF_sPDump1IFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPDump1IFInfo;			/* Info on interface */
	IMG_CHAR					caCmd[4 + 9 + MAX_FILENAME + 2];


    /* If the device has not been initialised...*/
	if (!psDeviceCB->bInitialised)
	{
		ui32DataOffset = 0;
		ui32CmdNum = 0;
		ui32LineCount = 0;

		
		PDUMP1IF_caCommandFile = IMG_MALLOC(strlen(pDeviceInfo->cpOutputDirectory) + strlen(pDeviceInfo->cpCommandFile) + 1);
		strcpy (PDUMP1IF_caCommandFile, pDeviceInfo->cpOutputDirectory);
		strcat (PDUMP1IF_caCommandFile, pDeviceInfo->cpCommandFile);
		PDUMP1IF_caSendData = IMG_MALLOC(strlen(pDeviceInfo->cpOutputDirectory) + strlen(pDeviceInfo->cpSendData) + 1);
		strcpy (PDUMP1IF_caSendData, pDeviceInfo->cpOutputDirectory);
		strcat (PDUMP1IF_caSendData, pDeviceInfo->cpSendData);
		PDUMP1IF_caReturnData = IMG_MALLOC(strlen(pDeviceInfo->cpInputDirectory) + strlen(pDeviceInfo->cpReturnData) + 1);
		strcpy (PDUMP1IF_caReturnData, pDeviceInfo->cpInputDirectory);
		strcat (PDUMP1IF_caReturnData, pDeviceInfo->cpReturnData);

		/* delete all comms files */
		remove(PDUMP1IF_caReturnData);
		remove(PDUMP1IF_caSendData);
		remove(PDUMP1IF_caCommandFile);

		printf("Creating Pdump Data File %s\n", PDUMP1IF_caSendData);

		do
		{
			fSendData = fopen(PDUMP1IF_caSendData, "wb");
		}while (fSendData == NULL);


		printf("Creating Pdump Script File %s\n", PDUMP1IF_caCommandFile);
		do
		{
			fCommandFile = fopen(PDUMP1IF_caCommandFile, "wb");
		}while (fCommandFile == NULL);

		sprintf(caCmd, "STA %08X %08X %s\n", ui32CmdNum, ui32DataOffset, pDeviceInfo->cpSendData);
		PDUMP1IF_WriteData(&ui32CmdNum,4);
		PDUMP1IF_addCommand(caCmd);

		psDeviceCB->bInitialised = IMG_TRUE;
    }

	return IMG_SUCCESS;

}

/*!
******************************************************************************

 @Function				PDUMP1IF_Deinitailise

******************************************************************************/
IMG_VOID PDUMP1IF_Deinitailise (
	DEVIF_sDeviceCB *   psDeviceCB
)
{
	DEVIF_sPDump1IFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPDump1IFInfo;			/* Info on interface */
	IMG_CHAR	caCmd[4 + 9 + 9 + MAX_FILENAME + 2];

	if (psDeviceCB->bInitialised == IMG_TRUE)
	{
		PDUMP1IF_sendCommand(psDeviceCB);
		rewind(fCommandFile);
		ui32CmdNum = 0xFFFFFFFF;
		sprintf(caCmd, "STA %08X %08X %s\n", ui32CmdNum, ui32DataOffset, pDeviceInfo->cpSendData);
		PDUMP1IF_addCommand(caCmd);
		PDUMP1IF_WriteData(&ui32CmdNum,4);

		PDUMP1IF_sendCommand(psDeviceCB);

		fclose(fReturnData);
		fclose(fSendData);
		fclose(fCommandFile);

		remove(PDUMP1IF_caReturnData);
		remove(PDUMP1IF_caSendData);
		remove(PDUMP1IF_caCommandFile);

		IMG_FREE(PDUMP1IF_caReturnData);
		IMG_FREE(PDUMP1IF_caSendData);
		IMG_FREE(PDUMP1IF_caCommandFile);

		psDeviceCB->bInitialised = IMG_FALSE;
	}
}

/*!
******************************************************************************

 @Function				PDUMP1IF_RegisterPoll

******************************************************************************/
IMG_VOID PDUMP1IF_RegisterPoll (
	DEVIF_sDeviceCB * psDeviceCB,
	IMG_UINT32		ui32Address,
	IMG_UINT32		ui32RequValue,
	IMG_UINT32		ui32Enable,
	IMG_UINT32		ui32FuncId,
	IMG_UINT32		ui32PollCount,
	IMG_UINT32		ui32TimeOut)
{
	DEVIF_PDumpCmd			pdCmdType;									/* holds command type (SAVE) */
	IMG_CHAR				caCmdStr [4 + 5 + 9 + 9 + 9 + 2 + 9 + 9 + 2 ];	/* caBuffer to hold command string :MEM:12345678 12345678 filename */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	pdCmdType = PDUMP_CMD_POLL;

	sprintf(caCmdStr, "POL :REG:%08X %08X %08X %01u %u %u\n", ui32Address, ui32RequValue, ui32Enable, ui32FuncId, ui32PollCount, ui32TimeOut);
	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
}


/*!
******************************************************************************

 @Function				PDUMP1IF_MemoryPoll

******************************************************************************/
IMG_VOID PDUMP1IF_MemoryPoll (
	DEVIF_sDeviceCB * psDeviceCB,
	IMG_UINT32		ui32Address,
	IMG_UINT32		ui32RequValue,
	IMG_UINT32		ui32Enable,
	IMG_UINT32		ui32FuncId,
	IMG_UINT32		ui32PollCount,
	IMG_UINT32		ui32TimeOut)
{
	DEVIF_PDumpCmd			pdCmdType;									/* holds command type (SAVE) */
	IMG_CHAR				caCmdStr [4 + 5 + 9 + 9 + 9 + 2 + 9 + 9 + 2 ];	/* caBuffer to hold command string :MEM:12345678 12345678 filename */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	pdCmdType = PDUMP_CMD_POLL;

	sprintf(caCmdStr, "POL :MEM:%08X %08X %08X %01u %u %u\n", ui32Address, ui32RequValue, ui32Enable, ui32FuncId, ui32PollCount, ui32TimeOut);
	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
}


/*!
******************************************************************************

 @Function				PDUMP1IF_CircBuffPoll

******************************************************************************/
IMG_VOID PDUMP1IF_CircBuffPoll (
			DEVIF_sDeviceCB				* psDeviceCB,
			IMG_UINT32	 				ui32Offset,
			IMG_UINT32					ui32WriteOffsetVal,
			IMG_UINT32 					ui32PacketSize,
			IMG_UINT32					ui32BufferSize
)
{
	DEVIF_PDumpCmd			pdCmdType;									/* holds command type (SAVE) */
	IMG_CHAR				caCmdStr [4 + 5 + 9 + 9 + 9 + 9 + 2 ];	/* caBuffer to hold command string */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	pdCmdType = PDUMP_CMD_POLL;

	sprintf(caCmdStr, "CBP :MEM:%08X %08X %08X %08X\n", 
				ui32Offset, ui32WriteOffsetVal, ui32PacketSize, ui32BufferSize);
	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
}



/*!
******************************************************************************

 @Function				PDUMP1IF_CopyDeviceToHost

******************************************************************************/
IMG_VOID PDUMP1IF_CopyDeviceToHost (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
	DEVIF_sPDump1IFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPDump1IFInfo;			/* Info on interface */
	DEVIF_PDumpCmd			pdCmdType;										/* holds command type (SAVE) */
	IMG_CHAR				caCmdStr [4 + 5 + 9 + 9 + MAX_FILENAME + 2];	/* caBuffer to hold command string :MEM:12345678 12345678 filename */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	pdCmdType = PDUMP_CMD_SAVE;
	
	sprintf(caCmdStr, "SAB :MEM:%08X %08X %s\n", IMG_UINT64_TO_UINT32(ui64DevAddress), ui32Size, pDeviceInfo->cpReturnData);

	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
	PDUMP1IF_ReadData(psDeviceCB, pvHostAddress, ui32Size);
}

/*!
******************************************************************************

 @Function				PDUMP1IF_CopyHostToDevice

******************************************************************************/
IMG_VOID PDUMP1IF_CopyHostToDevice (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	DEVIF_sPDump1IFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPDump1IFInfo;			/* Info on interface */
	DEVIF_PDumpCmd			pdCmdType;									/* holds command type (load) */	
	IMG_CHAR				caCmdStr[4 + 5 + 9 + 9 + 9 + MAX_FILENAME + 2];	/* Buffer to hold command string :MEM:12345678 12345678 12345678 filename */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	pdCmdType = PDUMP_CMD_LOAD;

	sprintf(caCmdStr, "LDB :MEM:%08X %08X %08X %s\n", IMG_UINT64_TO_UINT32(ui64DevAddress), ui32DataOffset, ui32Size, pDeviceInfo->cpSendData);
	PDUMP1IF_WriteData(pvHostAddress, ui32Size);

	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);	
}


/*!
******************************************************************************

 @Function				PDUMP1IF_RegLoadWords

******************************************************************************/
IMG_VOID PDUMP1IF_RegLoadWords (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
)
{
	DEVIF_sPDump1IFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPDump1IFInfo;			/* Info on interface */
	DEVIF_PDumpCmd			pdCmdType;										/* holds command type (load) */	
	IMG_CHAR				caCmdStr[4 + 5 + 9 + 9 + 9 + MAX_FILENAME + 2];	/* Buffer to hold command string :MEM:12345678 12345678 12345678 filename */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui64DevAddress & 0x3) == 0);

	pdCmdType = PDUMP_CMD_LOAD;

	sprintf(caCmdStr, "LDW :REG:%016" IMG_I64PR "X %08X %08X %s\n", ui64DevAddress, ui32DataOffset, ui32Size, pDeviceInfo->cpSendData);
	PDUMP1IF_WriteData(pvHostAddress, ui32Size * 4);

	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);	
	
}

/*!
******************************************************************************

 @Function				PDUMP1IF_ReadRegister

******************************************************************************/
IMG_UINT32 PDUMP1IF_ReadRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
)
{
	DEVIF_PDumpCmd			pdCmdType;								/* holds command type (read) */
	IMG_CHAR				caCmdStr[4 + 5 + 9 + 9 + MAX_FILENAME + 2];	/* Buffer to hold address string :MEM:12345678 */
	IMG_UINT32				ui32ReadValue = 0;/*, ui32WordsRead = 0;*/	/* value read back from file and number of characters read */
	DEVIF_sPDump1IFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPDump1IFInfo;			/* Info on interface */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui64RegAddress & 0x3) == 0);

	pdCmdType = PDUMP_CMD_SAVE;

	sprintf(caCmdStr, "SAW :REG:%08X %08X %s\n", IMG_UINT64_TO_UINT32(ui64RegAddress), 1, pDeviceInfo->cpReturnData);

	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
	PDUMP1IF_ReadData(psDeviceCB, &ui32ReadValue, 4);
	return ui32ReadValue;
}

/*!
******************************************************************************

 @Function				PDUMP1IF_WriteRegister

******************************************************************************/
IMG_VOID PDUMP1IF_WriteRegister (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
)
{
	DEVIF_PDumpCmd	pdCmdType;						/* holds command type (write) */
	IMG_CHAR		caCmdStr [4 + 5 + 9 + 9 + 2];	/* Buffer to hold address and value string :REG:12345678 12345678 */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui64RegAddress & 0x3) == 0);

	pdCmdType = PDUMP_CMD_WRITE;

	sprintf(caCmdStr, "WRW :REG:%08X %08X\n", IMG_UINT64_TO_UINT32(ui64RegAddress), ui32Value);
	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
}

/*!
******************************************************************************

 @Function				PDUMP1IF_ReadMemory

******************************************************************************/
IMG_UINT32 PDUMP1IF_ReadMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
)
{
	DEVIF_PDumpCmd			pdCmdType;								/* holds command type (read) */
	IMG_CHAR				caCmdStr[4 + 5 + 9 + 9 + MAX_FILENAME + 2];	/* Buffer to hold address string :MEM:12345678 */
	IMG_UINT32				ui32ReadValue = 0;			/* value read back from file */
	DEVIF_sPDump1IFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPDump1IFInfo;			/* Info on interface */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui64DevAddress & 0x3) == 0);

	pdCmdType = PDUMP_CMD_SAVE;

	sprintf(caCmdStr, "SAW :MEM:%08X %08X %s\n", IMG_UINT64_TO_UINT32(ui64DevAddress), 1, pDeviceInfo->cpReturnData);

	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
	PDUMP1IF_ReadData(psDeviceCB, &ui32ReadValue, 4);
	return ui32ReadValue;
}



/*!
******************************************************************************

 @Function				PDUMP1IF_WriteMemory

******************************************************************************/
IMG_VOID PDUMP1IF_WriteMemory (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
)
{
	DEVIF_PDumpCmd	pdCmdType;						/* holds command type (write) */
	IMG_CHAR		caCmdStr [4 + 5 + 9 + 9 + 2];	/* caBuffer to hold address and value string :MEM:12345678 12345678 */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	IMG_ASSERT((ui64DevAddress & 0x3) == 0);

	pdCmdType = PDUMP_CMD_WRITE;

	sprintf(caCmdStr, "WRW :MEM:%08X %08X\n", IMG_UINT64_TO_UINT32(ui64DevAddress), ui32Value);

	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
}

/*!
******************************************************************************

 @Function				PDUMP1IF_Idle

******************************************************************************/
IMG_VOID PDUMP1IF_Idle (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT32			ui32Cycles
)
{
	DEVIF_PDumpCmd	pdCmdType;						/* holds command type (write) */
	IMG_CHAR		caCmdStr [4 + 8 + 2];			/* Buffer to hold command string */

	IMG_ASSERT(psDeviceCB->bInitialised == IMG_TRUE);
	pdCmdType = PDUMP_CMD_IDLE;

	sprintf(caCmdStr, "IDL %08X\n", ui32Cycles);

	PDUMP1IF_commandManager(psDeviceCB, pdCmdType, caCmdStr);
}

/*!
******************************************************************************

 @Function				PDUMP1IF_commandManager

 @Description			Generates PDump1 command strings
						and creates command batches.

 @Input					PSDeviceCB: A pointer to the current device information

 @Input					PDumpCmd: The type of PDump Command requested

 @Input					cpVariableStr: A string containing the command details,
						the address, value

 @Return				If there is a read command the value read will be returned
						otherwise 0

******************************************************************************/

IMG_VOID PDUMP1IF_commandManager (
	DEVIF_sDeviceCB *	PSDeviceCB,
	DEVIF_PDumpCmd		PDumpCmd,
	IMG_CHAR *			cpVariableStr
)
{

	switch (PDumpCmd) {
		case PDUMP_CMD_WRITE:
		case PDUMP_CMD_LOAD:
		case PDUMP_CMD_IDLE:
		case PDUMP_CMD_POLL:
			PDUMP1IF_addCommand(cpVariableStr);
			break;
		case PDUMP_CMD_SAVE:
			PDUMP1IF_addCommand(cpVariableStr);
			PDUMP1IF_sendCommand(PSDeviceCB);
			break;
		default:
			printf("DEVIF Unknown PDUMP command\n");
			IMG_ASSERT(IMG_FALSE);
	}
}

/*!
******************************************************************************

 @Function				PDUMP1IF_addCommand

 @Description			Adds a command to the pending command file.

 @Input					caCmdStr: the command string

******************************************************************************/

IMG_VOID PDUMP1IF_addCommand (IMG_CHAR * caCmdStr)
{
	IMG_UINT32	ui32CharsWritten = 0, ui32CmdLength = 0;

	ui32CmdLength = (IMG_UINT32) strlen(caCmdStr);
	ui32LineCount ++;
	//printf("%s", caCmdStr);

	ui32CharsWritten = (IMG_UINT32) fwrite (caCmdStr, sizeof(IMG_CHAR), ui32CmdLength, fCommandFile);
	IMG_ASSERT(ui32CharsWritten == ui32CmdLength);
}

/*!
******************************************************************************

 @Function				PDUMP1IF_sendCommand

 @Description			Copies the pending file to the command file and creates the signal file.

 @Return				read value if applicable otherwise 0

******************************************************************************/

IMG_VOID PDUMP1IF_sendCommand(DEVIF_sDeviceCB *	psDeviceCB)
{
	DEVIF_sPDump1IFInfo*	pDeviceInfo = &psDeviceCB->sDevIfSetup.u.sPDump1IFInfo;			/* Info on interface */
	IMG_CHAR	caCmd[4 + 9 + 9 + MAX_FILENAME + 2];
	IMG_UINT32	ui32ReadCmdNo;

	sprintf(caCmd, "FIN %08X\n", ui32CmdNum);
	PDUMP1IF_addCommand(caCmd);
	//printf("sending command %08X\n", ui32CmdNum);
	fflush(fCommandFile);
	fflush(fSendData);

	ui32Wait = ui32LineCount * POLL_CMD_TIME_MS;
	if (ui32Wait < MIN_POLL_TIME_S * 1000)
		ui32Wait = MIN_POLL_TIME_S * 1000;
	ui32LineCount = 0;

	PDUMP1IF_ReadData(psDeviceCB, &ui32ReadCmdNo, 4);
	IMG_ASSERT(ui32ReadCmdNo == ui32CmdNum);

	ui32CmdNum++;
	if (ui32CmdNum > 0xFFFFFF)
		ui32CmdNum = 1;
	rewind(fCommandFile);
	//rewind(fSendData);
	//ui32DataOffset = 0;

	sprintf(caCmd, "STA %08X %08X %s\n", ui32CmdNum, ui32DataOffset, pDeviceInfo->cpSendData);
	PDUMP1IF_addCommand(caCmd);

 	PDUMP1IF_WriteData(&ui32CmdNum,4);


}

/*!
******************************************************************************

 @Function				PDUMP1IF_ReadData

 @Description			Reads Data back from the return data file

 @Input					vpData, Pointer to the location to which data will be read

 @Input					ui32NoBytes, number of bytes to read from the data file

******************************************************************************/

IMG_VOID PDUMP1IF_ReadData(
	DEVIF_sDeviceCB * psDeviceCB,
	IMG_VOID * vpData,
	IMG_UINT32 ui32NoBytes
)
{
	IMG_UINT32	ui32BytesRead;
	IMG_UINT32	 ui32PollCount = 0;

	PDUMP1IF_OpenDataReturn(psDeviceCB);
	//printf("Waiting for data ...");
	//fflush (stdin);

	ui32BytesRead = (IMG_UINT32) fread (vpData, sizeof(IMG_CHAR), ui32NoBytes, fReturnData);
	while (ui32BytesRead != ui32NoBytes)
	{

		fseek(fReturnData, ui32BytesRead, SEEK_CUR);
#ifdef WIN32
		Sleep(POLL_TIME_MS);		/*windows specific delay!*/
#else
        usleep(1000 * POLL_TIME_MS);	/* linux delay */
#endif
		ui32PollCount ++;
		IMG_ASSERT (ui32Wait > ui32PollCount * POLL_TIME_MS);
		ui32BytesRead = (IMG_UINT32) fread (vpData, sizeof(IMG_CHAR), ui32NoBytes, fReturnData);
	}
	//printf("data read\n");
}


/*!
******************************************************************************

 @Function				PDUMP1IF_WriteData

 @Description			Writes Data to the send data file

 @Input					vpData, Pointer to the location from which data will be written

 @Input					ui32NoBytes, number of bytes to write to the data file

******************************************************************************/

IMG_VOID PDUMP1IF_WriteData(IMG_VOID * vpData, IMG_UINT32 ui32NoBytes)
{
	IMG_UINT32	ui32BytesWritten;

	IMG_ASSERT(fSendData);

	ui32BytesWritten = (IMG_UINT32) fwrite (vpData, sizeof(IMG_CHAR), ui32NoBytes, fSendData);
	IMG_ASSERT(ui32BytesWritten == ui32NoBytes);
	ui32DataOffset += ui32NoBytes;
}


/*!
******************************************************************************

 @Function				PDUMP1IF_OpenDataReturn
 @Description			if not already open, opens data return file

******************************************************************************/

IMG_VOID PDUMP1IF_OpenDataReturn()
{
	IMG_UINT32	 ui32PollCount = 0;

	if (fReturnData == NULL)
	{
		printf("Waiting for Pdump Return Data File...%s\n", PDUMP1IF_caReturnData);
		fflush ( stdin );
		fReturnData = fopen(PDUMP1IF_caReturnData, "rb");
		while (fReturnData == NULL)
		{
#ifdef WIN32
			Sleep(POLL_TIME_MS);		/*windows specific delay!*/
#else

        	usleep(1000 * POLL_TIME_MS);
#endif
			ui32PollCount ++;
			IMG_ASSERT (ui32Wait > ui32PollCount * POLL_TIME_MS);
			fReturnData = fopen(PDUMP1IF_caReturnData, "rb");
		}
		printf("File Found\n");
	}
}

/*!
******************************************************************************

 @Function				PDUMP1IF_DevIfConfigureDevice
 
******************************************************************************/
IMG_VOID
PDUMP1IF_DevIfConfigureDevice(
    DEVIF_sDeviceCB *   psDeviceCB)
{   
	psDeviceCB->pvDevIfInfo = IMG_NULL;
	psDeviceCB->bInitialised = IMG_FALSE;

	/* Use "PDump" driver...*/
	psDeviceCB->pfnInitailise			= PDUMP1IF_Initialise;
	psDeviceCB->pfnReadRegister			= PDUMP1IF_ReadRegister;
	psDeviceCB->pfnWriteRegister		= PDUMP1IF_WriteRegister;
	psDeviceCB->pfnReadMemory			= PDUMP1IF_ReadMemory;
	psDeviceCB->pfnWriteMemory			= PDUMP1IF_WriteMemory;
	psDeviceCB->pfnCopyDeviceToHost		= PDUMP1IF_CopyDeviceToHost;
	psDeviceCB->pfnCopyHostToDevice		= PDUMP1IF_CopyHostToDevice;
	psDeviceCB->pfnDeinitailise			= PDUMP1IF_Deinitailise;
	psDeviceCB->pfnIdle					= PDUMP1IF_Idle;
	//psDeviceCB->pfnPDump1RegisterPoll	= PDUMP1IF_RegisterPoll;
	psDeviceCB->pfnRegWriteWords		= PDUMP1IF_RegLoadWords;
	//psDeviceCB->pfnPDump1MemoryPoll		= PDUMP1IF_MemoryPoll;
	//psDeviceCB->pfnCircBuffPoll			= PDUMP1IF_CircBuffPoll;
}
