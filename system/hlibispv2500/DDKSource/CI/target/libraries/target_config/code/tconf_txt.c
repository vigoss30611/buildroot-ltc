/*!
********************************************************************************
 @file   : tconf_txt.c

 @brief

 @Author Jose Massada

 @date   03/10/2012

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
         Functions used to read a Target Config File in text format.

 <b>Platform:</b>\n
	     Platform Independent
		  
 @Version
	     1.0

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gzip_fileio.h"
#include "tconf_int.h"
#include "tconf_txt.h"
#include "img_errors.h"


#define TCONF_WRAPFLAG_BURNMEM						0x00000001	/*!< Use burn-mem driver (depricated)               */

#define TCONF_WRAPFLAG_SGX_CLKGATECTL_OVERWRITE		0x00000002  /*!< Overwrite accesses to SGX CLKGATECTL register  */
#define TCONF_WRAPFLAG_MSVDX_POL_RENDEC_SPACE			0x00000004  /*!< Execute a POL of MSVDX_RENDEC_SPACE on write to RENDEC_WRITE_DATA */
#define	TCONF_WRAPFLAG_SIF_BUFFER_DISABLE				0x00000008	/*!< Disable Buffering on the socket interface		*/
#define TCONF_WRAPFLAG_MEM_TWIDDLE						0x00000020	/*!< Enable twiddling of memory bits to make 32bit mem appear 36bit / 40bit
																	(defined by TAL_WRAPFLAG_MEM_TWIDDLE_MASK) */
#define TCONF_WRAPFLAG_CHECK_MEM_COPY					0x00000040	/*!< Enable checks to see memory has been written before continuing */
#define TCONF_WRAPFLAG_ADD_DEVMEM_OFFSET_TO_PCI		0x00000080	//!< Include the device memory offset in the calcuation of the address sent to PCI

// Selection of customer reserved interface Ids
#define TCONF_WRAPFLAG_RESERVE1_IF					0x18		//!< Reserved for customer use
#define TCONF_WRAPFLAG_RESERVE2_IF					0x19		//!< Reserved for customer use
#define TCONF_WRAPFLAG_RESERVE3_IF					0x1A		//!< Reserved for customer use
#define TCONF_WRAPFLAG_RESERVE4_IF					0x1B		//!< Reserved for customer use
#define TCONF_WRAPFLAG_RESERVE5_IF					0x1C		//!< Reserved for customer use
#define TCONF_WRAPFLAG_RESERVE6_IF					0x1D		//!< Reserved for customer use
#define TCONF_WRAPFLAG_RESERVE7_IF					0x1E		//!< Reserved for customer use
#define TCONF_WRAPFLAG_RESERVE8_IF					0x1F		//!< Reserved for customer use
	
#define TCONF_WRAPFLAG_MEM_TWIDDLE_MASK				0xF0000
#define TCONF_WRAPFLAG_MEM_TWIDDLE_SHIFT				16
#define TCONF_WRAPFLAG_MEM_TWIDDLE_36BIT_OLD			0x0			//!< Old 36bit mem twiddle
#define TCONF_WRAPFLAG_MEM_TWIDDLE_36BIT				0x1			//!< 36bit mem twiddle algorithm
#define TCONF_WRAPFLAG_MEM_TWIDDLE_40BIT				0x2			//!< 40bit mem twiddle algorithm
#define TCONF_WRAPFLAG_MEM_TWIDDLE_DROPTOP32			0x3			//!< Drop top 32bits of address


#define TCONF_WRAPFLAG_IF_MASK						0x0000FF00	//!< Interface mask                                
#define TCONF_WRAPFLAG_IF_SHIFT					8	        //!< Interface shift                               
#define TCONF_WRAPFLAG_PCI_IF						0x00	    //!< Use PIC interface                             
#define TCONF_WRAPFLAG_BURNMEM_IF					0x01	    //!< Use Burn Mem interface                        
#define TCONF_WRAPFLAG_SOCKET_IF    				0x02	    //!< Use socket interface                          
#define TCONF_WRAPFLAG_PDUMP1_IF    				0x03	    //!< Use PDump1 interface                          
#define TCONF_WRAPFLAG_DASH_IF						0x04		//!< Use Dash interface							   
#define TCONF_WRAPFLAG_DIRECT_IF					0x05		//!< Use Direct connection with device_interface
#define TCONF_WRAPFLAG_TRANSIF_IF					0x06		//!< Use Transif connection with device_interface
#define TCONF_WRAPFLAG_NULL_IF						0x07		//!< Use the null testing interface
#define TCONF_WRAPFLAG_POSTED_IF					0x08		//!< Use the posted testing interface
#define TCONF_WRAPFLAG_DUMMY_IF					0x09		//!< Use the Dummy testing interface

typedef enum
{
	TOKEN_NOT_RECOGNISED,
	TOKEN_WORD,
	TOKEN_NUMBER,
	TOKEN_COLON,
	TOKEN_OR,
	TOKEN_OPEN_BRACE,
	TOKEN_CLOSE_BRACE,
	TOKEN_EOL

} TCONF_eToken;

#define BUFFER_SIZE (4096)

static IMG_BOOL gbPCIAddDevOffset = IMG_FALSE;
static IMG_BOOL gbSocketBuffering = IMG_TRUE;

/*!
******************************************************************************

 @Function				SkipWhiteSpace

******************************************************************************/
static
int
SkipWhiteSpace(
	char *			psBuffer,
	int				i
)
{
	/* While next character is white space...*/
	while (isspace(psBuffer[i]))
	{
		/* Update index */
		i++;
	}

	/* Return updated index */
	return i;
}


/*!
******************************************************************************

 @Function				GetNextToken

******************************************************************************/
static
int
GetNextToken(
	char *			psBuffer,
	char *			psToken,
	IMG_PUINT64		pui64Number,
	TCONF_eToken *	eToken,
	int				i
)
{
	int				j = 0;
	IMG_UINT64		ui64Number = 0;
	IMG_BOOL		bNegNumber = IMG_FALSE;

	/* Skip white space */
	i = SkipWhiteSpace(psBuffer, i);

	/* Check for start of comment...*/
	if ( (psBuffer[i] == '-') && (psBuffer[i+1] == '-') )
	{
		/* Skip to the end of the line...*/
		while (psBuffer[i] != 0)
		{
			i++;
		}
		/* Token is a zero */
		*eToken = TOKEN_EOL;

		/* Null terminate token */
		psToken[j] = 0;

		return i;
	}

	/* If the token starts with an alpha, '_', '\', '/' or '.'...*/
	if (
		(isalpha(psBuffer[i])) || (psBuffer[i] == '_')  ||
		(psBuffer[i] == '%')   || (psBuffer[i] == '-')  ||
		(psBuffer[i] == '\\')  || (psBuffer[i] == '/')  || (psBuffer[i] == '.')
		)
	{
		/* Token is a word */
		*eToken = TOKEN_WORD;

		/* While next character is alpha/numeric, '_', '\', '/' or '.'...*/
		while (
			(isalnum(psBuffer[i])) || (psBuffer[i] == '_')  ||
			(psBuffer[i] == '%')   || (psBuffer[i] == '-')  ||
			(psBuffer[i] == '\\')  || (psBuffer[i] == '/')  || (psBuffer[i] == '.')
			)
		{
			/* Copy character from buffer to token buffer */
			psToken[j++] = psBuffer[i++];
		}
		/* Null terminate token */
		psToken[j] = 0;
	}
	/* Else, if the token starts with an '"'...*/
	else if ( psBuffer[i] == '"' )
	{
		i++;
		/* Token is a word */
		*eToken = TOKEN_WORD;

		/* While next character is alpha/numeric, '_', '\', '/' or '.'...*/
		while (
			(isalnum(psBuffer[i])) || (psBuffer[i] == '_')  ||
			(psBuffer[i] == '%')   || (psBuffer[i] == '-')  ||
			(psBuffer[i] == '\\')  || (psBuffer[i] == '/')  || (psBuffer[i] == '.')
			)
		{
			/* Copy character from buffer to token buffer */
			psToken[j++] = psBuffer[i++];
		}
		IMG_ASSERT(psBuffer[i++] == '"');
		/* Null terminate token */
		psToken[j] = 0;
	}
	/* Else, if the character is a digit, '+' or '-'...*/
	else if ( (isdigit(psBuffer[i])) || (psBuffer[i] == '+') || (psBuffer[i] == '-'))
	{
		/* Token is a number */
		*eToken = TOKEN_NUMBER;

		/* If negative number...*/
		if (psBuffer[i] == '-')
		{
			/* Set flag */
			bNegNumber = IMG_TRUE;
		}

		/* If digit...*/
		if (isdigit(psBuffer[i]))
		{
			/* Update number */
			ui64Number += (ui64Number * 10) + (psBuffer[i] - '0');
		}

		/* Copy character from buffer to token buffer */
		psToken[j++] = psBuffer[i++];

		/* While numeric...*/
		while (isdigit(psBuffer[i]))
		{
			/* Update number */
			ui64Number = (ui64Number * 10) + (psBuffer[i] - '0');

			/* Copy character from buffer to token buffer */
			psToken[j++] = psBuffer[i++];
		}

		/* If the number is hex...*/
		if ( (toupper(psBuffer[i]) == 'X') && (ui64Number == 0) )
		{
			/* Copy character from buffer to token buffer */
			psToken[j++] = psBuffer[i++];

			/* tconf_ProcessMM_ hex number */
			while (
					(isdigit(psBuffer[i])) ||
					(
						(toupper(psBuffer[i]) >= 'A') && (toupper(psBuffer[i]) <= 'F')
					)
				)
			{
				if (isdigit(psBuffer[i]))
				{
					/* Update number */
					ui64Number = (ui64Number * 16) + (psBuffer[i] - '0');
				}
				else
				{
					/* Update number */
					ui64Number = (ui64Number * 16) + (toupper(psBuffer[i]) - 'A' + 10);
				}
				/* Copy character from buffer to token buffer */
				psToken[j++] = psBuffer[i++];
			}
		}

		/* Return number */
		if (bNegNumber)
		{
			*pui64Number = (IMG_UINT64)(-(IMG_INT64)ui64Number);
		}
		else
		{
			*pui64Number = ui64Number;
		}

		/* Null terminate token */
		psToken[j] = 0;
	}
	/* Else, if the charater is a ':'...*/
	else if (psBuffer[i] == ':')
	{
		/* Token is a ':' */
		*eToken = TOKEN_COLON;

		/* Copy character from buffer to token buffer */
		psToken[j++] = psBuffer[i++];

		/* Null terminate token */
		psToken[j] = 0;
	}
	/* Else, if the charater is a '('...*/
	else if (psBuffer[i] == '(')
	{
		/* Token is a '(' */
		*eToken = TOKEN_OPEN_BRACE;

		/* Copy character from buffer to token buffer */
		psToken[j++] = psBuffer[i++];

		/* Null terminate token */
		psToken[j] = 0;
	}
	/* Else, if the charater is a ')'...*/
	else if (psBuffer[i] == ')')
	{
		/* Token is a ')' */
		*eToken = TOKEN_CLOSE_BRACE;

		/* Copy character from buffer to token buffer */
		psToken[j++] = psBuffer[i++];

		/* Null terminate token */
		psToken[j] = 0;
	}
	/* Else, if the charater is a '|'...*/
	else if (psBuffer[i] == '|')
	{
		/* Token is a '|' */
		*eToken = TOKEN_OR;

		/* Copy character from buffer to token buffer */
		psToken[j++] = psBuffer[i++];

		/* Null terminate token */
		psToken[j] = 0;
	}
	/* Else, if the charater is zero...*/
	else if (psBuffer[i] == 0)
	{
		/* Token is a zero */
		*eToken = TOKEN_EOL;

		/* Null terminate token */
		psToken[j] = 0;
	}
	else
	{
		/* Token not recognisd */
		*eToken = TOKEN_NOT_RECOGNISED;
	}

	/* Skip white space */
	i = SkipWhiteSpace(psBuffer, i);

	/* Return updated index */
	return i;
}

/*!
******************************************************************************

 @Function				ConfigFileError

******************************************************************************/
static
void ConfigFileError(
	TCONF_sTargetConf* 			psTargetConf,
	char *						psBuffer,
	int							i,
	char *						psExpect
)
{
	int				j;

	/* ERROR: "xx" Line:xx %s expected but not found */
	printf("%s", psBuffer);
	for (j=0; j<i; j++)
	{
		printf(" ");
	}
	printf("^\n");
	printf("ERROR: \"%s\" Line:%d %s expected but not found \n",
		psTargetConf->pszConfigFile, psTargetConf->ui32LineNo, psExpect);

	/* Assert */
	IMG_ASSERT(IMG_FALSE);
}

/*!
******************************************************************************

 @Function				ValidateNextToken

******************************************************************************/
static
int
ValidateNextToken(
	TCONF_sTargetConf* 			psTargetConf,
	char *						psBuffer,
	int							i,
	char *						psToken,
	IMG_PUINT64					pui64TokenValue,
	TCONF_eToken 				eReqToken,
	char *						psExpect
)
{
	TCONF_eToken 	eToken;
	int				ii;

	/* Get memory space name */
	ii = GetNextToken(psBuffer, psToken, pui64TokenValue, &eToken, i);
	if (eToken != eReqToken)
	{
        ConfigFileError(psTargetConf, psBuffer, i, psExpect);
	}

	/* Return updated index */
	return ii;
}

/*!
******************************************************************************

 @Function				tconf_ProcessMM_DEVICE

 Syntax:

    DEVICE  <device name> <device base addr> <mem base addr> <size> <guard band> <device flags>

******************************************************************************/
static IMG_RESULT tconf_ProcessMM_DEVICE(
  	TCONF_sTargetConf* 			psTargetConf,
	char *						psBuffer,
	int							i
)
{
	char					        sDeviceName[256];
	char					        sGash[256];
	IMG_UINT64				        ui64Gash;
	IMG_UINT64				        ui64RegBaseAddr;
	IMG_UINT64				        ui64MemBaseAddr;
    IMG_UINT64                      ui64MemSize;
    IMG_UINT64                      ui64MemGuardBand;
	IMG_UINT64				        ui64Flags;
	IMG_RESULT						rResult;

	i = ValidateNextToken(psTargetConf, psBuffer, i, sDeviceName, &ui64Gash,          TOKEN_WORD,     "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64RegBaseAddr,   TOKEN_NUMBER,   "<device base addr>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64MemBaseAddr,   TOKEN_NUMBER,   "<mem base addr>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64MemSize,       TOKEN_NUMBER,   "<size>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64MemGuardBand,  TOKEN_NUMBER,   "<guard band>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Flags,         TOKEN_NUMBER,   "<device flags>");

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Gash,        TOKEN_EOL, "End-of-line");

	rResult = TCONF_AddDevice(psTargetConf, sDeviceName, ui64RegBaseAddr);
	if (rResult != IMG_SUCCESS)
		return rResult;
	
	rResult = TCONF_SetDeviceDefaultMem(psTargetConf, sDeviceName, ui64MemBaseAddr, ui64MemSize, ui64MemGuardBand);
	if (rResult != IMG_SUCCESS)
		return rResult;
	
	return TCONF_SetDeviceFlags(psTargetConf, sDeviceName, IMG_UINT64_TO_UINT32(ui64Flags));  
}

/*!
******************************************************************************

 @Function				tconf_ProcessMM_MEMORY_POOL

 Syntax:

    MEMORY_POOL  <pool name> <default memory offset> <default mem size> <guard band> [<memory flags>]

******************************************************************************/
static IMG_RESULT tconf_ProcessMM_MEMORY_POOL(
	TCONF_sTargetConf* 			psTargetConf,
	char *						psBuffer,
	int							i
)
{
	char					        sPoolName[256];
	char					        sGash[256];
	IMG_UINT64				        ui64Gash;
	IMG_UINT64				        ui64MemBaseAddr;
    IMG_UINT64                      ui64MemSize;
    IMG_UINT64                      ui64MemGuardBand;
	IMG_UINT64				        ui64Flags;
	IMG_RESULT						rResult;


	i = ValidateNextToken(psTargetConf, psBuffer, i, sPoolName,	&ui64Gash,          TOKEN_WORD,     "<pool name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64MemBaseAddr,   TOKEN_NUMBER,   "<mem base addr>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64MemSize,       TOKEN_NUMBER,   "<size>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64MemGuardBand,  TOKEN_NUMBER,   "<guard band>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Flags,         TOKEN_NUMBER,   "<pool flags>");

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Gash,        TOKEN_EOL, "End-of-line");

	rResult = TCONF_AddDevice(psTargetConf, sPoolName, 0);
	if (rResult != IMG_SUCCESS)
		return rResult;
	rResult = TCONF_SetDeviceDefaultMem(psTargetConf, sPoolName, ui64MemBaseAddr, ui64MemSize, ui64MemGuardBand);
	if (rResult != IMG_SUCCESS)
		return rResult;
	return TCONF_SetDeviceFlags(psTargetConf, sPoolName, IMG_UINT64_TO_UINT32(ui64Flags));
}

/*!
******************************************************************************

 @Function				tconf_ProcessMM_REGISTER

 Syntax:

    REGISTER  <block name>  <device name>  <block offset>  <size>  <irq|"NO_IRQ">

******************************************************************************/
static
IMG_RESULT
tconf_ProcessMM_REGISTER(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	char					        sName[256];
	char					        sDeviceName[256];
	char					        sGash[256];
	IMG_UINT64				        ui64Gash;
    IMG_UINT64                      ui64BlockOffset;
    IMG_UINT64                      ui64Size;
    IMG_UINT64                      ui64Irq;
    int                             j;

	i = ValidateNextToken(psTargetConf, psBuffer, i, sName,       &ui64Gash,          TOKEN_WORD,     "<block name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sDeviceName, &ui64Gash,          TOKEN_WORD,     "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64BlockOffset,   TOKEN_NUMBER,   "<block offset>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Size,          TOKEN_NUMBER,   "<size>");

    if (psBuffer[i] == 'N')
    {
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,   &ui64Gash,    TOKEN_WORD,   "<irq|""NO_IRQ"">");
        if (strcmp(sGash, "NO_IRQ") != 0)
        {
		    /* ERROR: "xx" Line:xx %s expected but not found */
		    printf("%s", psBuffer);
		    for (j=0; j<i; j++)
		    {
			    printf(" ");
		    }
		    printf("^\n");
		    printf("ERROR: \"%s\" Line:%d %s expected but not found \n",
			    psTargetConf->pszConfigFile, psTargetConf->ui32LineNo, "NO_IRQ");

		    /* Assert */
		    IMG_ASSERT(IMG_FALSE);
        }
        ui64Irq = TAL_NO_IRQ;
    }
    else
    {
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,   &ui64Irq,   TOKEN_NUMBER,   "<irq|""NO_IRQ"">");
    }

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");

	return TCONF_AddDeviceRegister(psTargetConf, sDeviceName, sName, ui64BlockOffset, IMG_UINT64_TO_UINT32(ui64Size), IMG_UINT64_TO_UINT32(ui64Irq));
}


/*!
******************************************************************************

 @Function				tconf_ProcessMM_MEMORY

 Syntax:

    MEMORY  <block name|"DEFAULT"> <device name>  [ <mem base addr>  <size>  <guard band> ]

******************************************************************************/
static
IMG_RESULT
tconf_ProcessMM_MEMORY(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	char					        sName[256];
	char					        sDeviceName[256];
	char					        sGash[256];
	IMG_UINT64				        ui64Gash;
	IMG_UINT64				        ui64BaseAddr    = 0;
    IMG_UINT64                      ui64Size        = 0;
    IMG_UINT64                      ui64GuardBand   = 0;
	IMG_RESULT						rReturnCode = IMG_SUCCESS;

	i = ValidateNextToken(psTargetConf, psBuffer, i, sName,       &ui64Gash,    TOKEN_WORD,     "<block name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sDeviceName, &ui64Gash,    TOKEN_WORD,     "<device name>");

    /* If not end-of-line...*/
    if (psBuffer[i] != 0)
    {
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,   &ui64BaseAddr,    TOKEN_NUMBER,   "<mem base addr>");
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,   &ui64Size,        TOKEN_NUMBER,   "<size>");
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,   &ui64GuardBand,   TOKEN_NUMBER,   "<guard band>");

	    /* Check for end-of-line */
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,   &ui64Gash,   TOKEN_EOL, "End-of-line");
    }
    else
    {
	    /* Check for end-of-line */
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,   &ui64Gash,   TOKEN_EOL, "End-of-line");
    }

    /* "DEFAULT" no longer supported...*/
    if (strcmp(sName, "DEFAULT") == 0)
    {
		/* ERROR: DEFAULT must have base address, size etc. */
		printf("ERROR: \"%s\" Line:%d DEFAULT no longer supported\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

		/* Assert */
		IMG_ASSERT(IMG_FALSE);
    }
    
    
    
    rReturnCode = TCONF_AddDeviceMemory(psTargetConf, sDeviceName, sName, ui64BaseAddr, ui64Size, ui64GuardBand);

	
    return rReturnCode;
}

/*!
******************************************************************************

 @Function				tconf_ProcessPI_WRW

 Syntax:

    WRW :<name>:<offset> <value>

******************************************************************************/
static
IMG_RESULT
tconf_ProcessPI_WRW(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	char					        sMemSpaceName[256];
	char					        sGash[256];
	IMG_UINT64				        ui64Gash;
    IMG_UINT64                      ui64Offset;
    IMG_UINT64                      ui64Value;
    IMG_HANDLE                      hMemSpace;

	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Gash,          TOKEN_COLON,    "':'");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sMemSpaceName,&ui64Gash,         TOKEN_WORD,     "<name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Gash,          TOKEN_COLON,    "':'");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Offset,        TOKEN_NUMBER,   "<offset>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Value,         TOKEN_NUMBER,   "<value>");

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");

    /* Write value...*/
	hMemSpace = TAL_GetMemSpaceHandle (sMemSpaceName);
    return TALREG_WriteWord32 (hMemSpace, IMG_UINT64_TO_UINT32(ui64Offset), IMG_UINT64_TO_UINT32(ui64Value));
}

/*!
******************************************************************************

 @Function				tconf_ProcessWR_CONTROL

 Syntax:

    CONTROL <host/target cycle ratio> [<wrapper flags>]

******************************************************************************/
static
IMG_RESULT
tconf_ProcessWR_CONTROL(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	char					         sGash[256];
    IMG_UINT64                      ui64Gash;
    IMG_UINT64                      ui64HostTargetCycleRatio;
    IMG_UINT64                      ui64Flags = 0;
	DEVIF_sWrapperControlInfo 		 sWrapSetup;
	eTAL_Twiddle                    eMemTwiddle = TAL_TWIDDLE_NONE;
	IMG_BOOL						bVerifyMemWrites;
	
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64HostTargetCycleRatio,           TOKEN_NUMBER,   "<host/target cycle ratio>");

    /* If not end-of-line...*/
    if (psBuffer[i] != 0)
    {
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,       &ui64Flags,           TOKEN_NUMBER,   "<wrapper flags>");

	    /* Check for end-of-line */
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");
    }
    else
    {
	    /* Check for end-of-line */
	    i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");
    }
	sWrapSetup.ui32HostTargetCycleRatio = IMG_UINT64_TO_UINT32(ui64HostTargetCycleRatio);
	gbPCIAddDevOffset = (ui64Flags & TCONF_WRAPFLAG_ADD_DEVMEM_OFFSET_TO_PCI) ? IMG_TRUE : IMG_FALSE;
	gbSocketBuffering = (ui64Flags & TCONF_WRAPFLAG_SIF_BUFFER_DISABLE) ? IMG_FALSE : IMG_TRUE;
	bVerifyMemWrites = (ui64Flags & TCONF_WRAPFLAG_CHECK_MEM_COPY) ? IMG_TRUE : IMG_FALSE;
	
	// Work out which interface has been selected
	if (ui64Flags & TCONF_WRAPFLAG_BURNMEM)
		sWrapSetup.eDevifType = DEVIF_TYPE_BMEM;
	else
	{
		switch((ui64Flags & TCONF_WRAPFLAG_IF_MASK) >> TCONF_WRAPFLAG_IF_SHIFT)
		{
		case TCONF_WRAPFLAG_PCI_IF:
			sWrapSetup.eDevifType = DEVIF_TYPE_PCI;
			break;
		case TCONF_WRAPFLAG_BURNMEM_IF:
			sWrapSetup.eDevifType = DEVIF_TYPE_BMEM;
			break;  
		case TCONF_WRAPFLAG_SOCKET_IF:
			sWrapSetup.eDevifType = DEVIF_TYPE_SOCKET;
			break;  
		case TCONF_WRAPFLAG_PDUMP1_IF:
			sWrapSetup.eDevifType = DEVIF_TYPE_PDUMP1;
			break;  
		case TCONF_WRAPFLAG_DASH_IF:
			sWrapSetup.eDevifType = DEVIF_TYPE_DASH;
			break;  
		case TCONF_WRAPFLAG_DIRECT_IF:
			sWrapSetup.eDevifType = DEVIF_TYPE_DIRECT;
			break;  
		case TCONF_WRAPFLAG_NULL_IF:
			sWrapSetup.eDevifType = DEVIF_TYPE_NULL;
			break;  
		case TCONF_WRAPFLAG_POSTED_IF:
			sWrapSetup.eDevifType = DEVIF_TYPE_POSTED;
			break;  
		case TCONF_WRAPFLAG_TRANSIF_IF:
			printf("ERROR, Transif Interface now operates through the DIRECT devif interface\n");
		default:
			IMG_ASSERT(IMG_FALSE);
			return IMG_ERROR_INVALID_PARAMETERS;
		}
	}
	
	// Work out if any memory twiddling has been enabled
	if (ui64Flags & TCONF_WRAPFLAG_MEM_TWIDDLE)
	{
		switch((ui64Flags & TCONF_WRAPFLAG_MEM_TWIDDLE_MASK) >> TCONF_WRAPFLAG_MEM_TWIDDLE_SHIFT)
		{
		case TCONF_WRAPFLAG_MEM_TWIDDLE_36BIT_OLD:
			eMemTwiddle = TAL_TWIDDLE_36BIT_OLD;
			break;
		case TCONF_WRAPFLAG_MEM_TWIDDLE_36BIT:
			eMemTwiddle = TAL_TWIDDLE_36BIT;
			break;
		case TCONF_WRAPFLAG_MEM_TWIDDLE_40BIT:
			eMemTwiddle = TAL_TWIDDLE_40BIT;
			break;
		case TCONF_WRAPFLAG_MEM_TWIDDLE_DROPTOP32:
			eMemTwiddle = TAL_TWIDDLE_DROPTOP32;
			break;
		default:
			IMG_ASSERT(IMG_FALSE);
			return IMG_ERROR_INVALID_PARAMETERS;
		}
	}

	return TCONF_SetWrapperControlInfo(psTargetConf, &sWrapSetup, eMemTwiddle, bVerifyMemWrites);
}

/*!
******************************************************************************

 @Function				tconf_ProcessWR_PCI_CARD_INFO

 Syntax:

    PCI_CARD_INFO  <vendor id>  <device id>

******************************************************************************/
static IMG_RESULT tconf_ProcessWR_PCI_CARD_INFO(
	TCONF_sTargetConf* 				psTargetConf,
	char *							psBuffer,
	int								i)
{
    IMG_UINT64 ui64Gash, ui64VendorId, ui64DeviceId;
	IMG_CHAR szGash[256], szName[256];

	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &ui64VendorId, TOKEN_NUMBER, "<vendor id>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &ui64DeviceId, TOKEN_NUMBER, "<device id>");
	i = SkipWhiteSpace(psBuffer, i);

	/* Check for end-of-line */
	if (psBuffer[i] != '\0')
		i = ValidateNextToken(psTargetConf, psBuffer, i, szName,	&ui64Gash, TOKEN_WORD, "<interface name>");
	else
		szName[0] = '\0'; /* No interface name specified */

	/* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &ui64Gash, TOKEN_EOL, "End-of-line");

	return TCONF_AddPciInterface(psTargetConf, szName, IMG_UINT64_TO_UINT32(ui64VendorId), IMG_UINT64_TO_UINT32(ui64DeviceId));
}

/*!
******************************************************************************

 @Function				tconf_ProcessWR_PCI_MEMORY_BASE

 Syntax:

    PCI_MEMORY_BASE  [<device name>] <bar>  <offset>  <size>

******************************************************************************/
static
IMG_RESULT
tconf_ProcessWR_PCI_MEMORY_BASE(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	IMG_CHAR szDeviceName[256], szGash[256], szInterfaceName[256];
    IMG_UINT64 nGash, nBar, nBaseAddr, nSize;	
    /* If this looks like a word...*/
    i = SkipWhiteSpace(psBuffer, i);
	if (isalpha(psBuffer[i]) || 
		psBuffer[i] == '_' || psBuffer[i] == '%' || psBuffer[i] == '-' ||
		psBuffer[i] == '\\' || psBuffer[i] == '/' || psBuffer[i] == '.'
	)
	    i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
    else
        szDeviceName[0] = 0;

	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nBar, TOKEN_NUMBER, "<bar>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nBaseAddr, TOKEN_NUMBER, "<pci mem base addr>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nSize, TOKEN_NUMBER, "<size>");

	/* Check for end-of-line */
	if (psBuffer[i] != '\0')
		i = ValidateNextToken(psTargetConf, psBuffer, i, szInterfaceName, &nGash, TOKEN_WORD, "<interface name>");
	else
		szInterfaceName[0] = '\0'; /* No interface name specified, use empty string */

	/* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");

	return TCONF_AddPciInterfaceMemoryBase(
		psTargetConf,
		szDeviceName, 
		IMG_UINT64_TO_UINT32(nBar),
		IMG_UINT64_TO_UINT32(nBaseAddr),
		IMG_UINT64_TO_UINT32(nSize));
}

/*!
******************************************************************************

 @Function				tconf_ProcessWR_PCI_DEVICE_BASE

 Syntax:

 PCI_DEVICE_BASE  [<device name>] <bar> <pci dev base addr> <size>

******************************************************************************/
static IMG_RESULT tconf_ProcessWR_PCI_DEVICE_BASE(	
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	IMG_CHAR szDeviceName[256], szGash[256], szInterfaceName[256];
	IMG_UINT64 nGash, nBar, nBaseAddr, nSize;

    /* If this looks like a word...*/
    i = SkipWhiteSpace(psBuffer, i);
	if (isalpha(psBuffer[i]) ||
		psBuffer[i] == '_'  || psBuffer[i] == '%' || psBuffer[i] == '-' ||
		psBuffer[i] == '\\' || psBuffer[i] == '/' || psBuffer[i] == '.')
	{
	    i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
    }
    else
        szDeviceName[0] = '\0';

	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nBar, TOKEN_NUMBER, "<bar>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nBaseAddr, TOKEN_NUMBER, "<pci dev base addr>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nSize, TOKEN_NUMBER, "<size>");
	
	/* Check for end-of-line */
	if (psBuffer[i] != '\0')
		i = ValidateNextToken(psTargetConf, psBuffer, i, szInterfaceName, &nGash, TOKEN_WORD, "<interface name>");
	else	
		strcpy(szInterfaceName, "defaultName$"); /* No interface name specified, use a default name */

	/* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");


	return TCONF_AddPciInterfaceDeviceBase(
		psTargetConf,
		szDeviceName, 
		IMG_UINT64_TO_UINT32(nBar), 
		IMG_UINT64_TO_UINT32(nBaseAddr), 
		IMG_UINT64_TO_UINT32(nSize),
		gbPCIAddDevOffset);
}

/*!
******************************************************************************

 @Function				tconf_ProcessWR_BURN_MEMORY

 Syntax:

 BURN_MEMORY <device name> <device id> <mem start offset> <size>

******************************************************************************/
static IMG_RESULT tconf_ProcessWR_BURN_MEMORY(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	IMG_CHAR szDeviceName[256], szDeviceId[256], szGash[256];
    IMG_UINT64 nGash, nOffset, nSize;

    /* If we are not getting the burn-mem info...*/
	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceId, &nGash, TOKEN_WORD, "<device id>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nOffset, TOKEN_NUMBER, "<mem start offset>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nSize, TOKEN_NUMBER, "<size>");

	/* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");

	return TCONF_AddBurnMem(
		psTargetConf,
		szDeviceName,
		szDeviceId[0],
		IMG_UINT64_TO_UINT32(nOffset),
		IMG_UINT64_TO_UINT32(nSize));
}

/*!
******************************************************************************

 @Function				tconf_ProcessDASH_INFO

 Syntax:

 DASH_INFO <device name> <dash name>

 ******************************************************************************/
static IMG_RESULT tconf_ProcessDASH_INFO(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	IMG_CHAR szDeviceName[256], szGash[256];
	IMG_UINT64 nGash, nId;

	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nId, TOKEN_NUMBER, "<dash num>");

    /* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");

	return TCONF_AddDashInterface(psTargetConf, szDeviceName, IMG_UINT64_TO_UINT32(nId));
}

/*!
******************************************************************************

 @Function				tconf_ProcessDASH_DEVICE_BASE

 Syntax:

 DASH_DEVICE_BASE <device name> <dev address> <dev size>

 ******************************************************************************/
static IMG_RESULT tconf_ProcessDASH_DEVICE_BASE(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	IMG_CHAR szDeviceName[256], szGash[256];
    IMG_UINT64 nGash, nBaseAddr, nSize;

	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nBaseAddr, TOKEN_NUMBER, "<dev address>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nSize, TOKEN_NUMBER, "<dev size>");

    /* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");

	return TCONF_SetDashInterfaceDeviceBase(
		psTargetConf,
		szDeviceName, 
		IMG_UINT64_TO_UINT32(nBaseAddr), 
		IMG_UINT64_TO_UINT32(nSize));
}

/*!
******************************************************************************

 @Function				tconf_ProcessDASH_MEMORY_BASE

 Syntax:

 DASH_MEMORY_BASE <device name> <mem address> <mem size>

 ******************************************************************************/
static IMG_RESULT tconf_ProcessDASH_MEMORY_BASE(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	IMG_CHAR szDeviceName[256], szGash[256];
    IMG_UINT64 nGash, nBaseAddr, nSize;

	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nBaseAddr, TOKEN_NUMBER, "<mem address>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nSize, TOKEN_NUMBER, "<mem size>");

    /* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");


	return TCONF_SetDashInterfaceMemoryBase(
		psTargetConf,
		szDeviceName, 
		IMG_UINT64_TO_UINT32(nBaseAddr), 
		IMG_UINT64_TO_UINT32(nSize));
}

/*!
******************************************************************************

 @Function				tconf_ProcessPOSTED_SETUP

 Syntax:

 POSTED_SETUP <device name> <posted_offset>

 ******************************************************************************/
static IMG_RESULT tconf_ProcessPOSTED_SETUP(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	IMG_CHAR szDeviceName[256], szGash[256];
	IMG_UINT64 nOffset, nGash;

	/* Get values */
	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nOffset, TOKEN_NUMBER, "<posted_offset>");

    /* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");

	return TCONF_AddPostedInterface(psTargetConf, szDeviceName, IMG_UINT64_TO_UINT32(nOffset));
}

/*!
******************************************************************************

 @Function				tconf_ProcessPDUMP1IF_INFO

 Syntax:

 PDUMP1IF_INFO <device name> <infolder> <outfolder> <cmd file> <send data file> <receive data file>

 ******************************************************************************/
static IMG_RESULT tconf_ProcessPDUMP1IF_INFO(
	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	IMG_CHAR szDeviceName[256], szInputDir[256], szOutputDir[256];
	IMG_CHAR szCommandFile[256], szSendFile[256];
	IMG_CHAR szReceiveFile[256], szGash[256];
	IMG_UINT64 nGash;

	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szInputDir, &nGash, TOKEN_WORD, "<infolder>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szOutputDir, &nGash, TOKEN_WORD, "<outfolder>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szCommandFile, &nGash, TOKEN_WORD, "<cmd file>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szSendFile, &nGash, TOKEN_WORD, "<send data file>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szReceiveFile, &nGash, TOKEN_WORD, "<receive data file>");

    /* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");

	return TCONF_AddPdump1IF(
		psTargetConf,
		szDeviceName,
		szInputDir,
		szOutputDir,
		szCommandFile,
		szSendFile,
		szReceiveFile);
}

/*!
******************************************************************************

 @Function				TCONF_ProcessWR_SOCKET_INFO

 Syntax:

 SOCKET_INFO <device name> [ <port> <ip address> ]

 ******************************************************************************/
IMG_RESULT tconf_ProcessWR_SOCKET_INFO(
	TCONF_sTargetConf* psTargetConf,
	char *psBuffer,
	int i
)
{
	IMG_CHAR szGash[256], szDeviceName[256], szHostname[256], szParentDeviceName[256];
	IMG_UINT64 nGash, nPort = 0, nIp1 = 0, nIp2 = 0, nIp3 = 0, nIp4 = 0;
	const IMG_CHAR *pszParentDeviceName = NULL, *pszHostname = NULL;
	TCONF_eToken token;

	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	
	// Next is a port number or parent device
    i = SkipWhiteSpace(psBuffer, i);
	i = GetNextToken(psBuffer, szParentDeviceName, &nPort, &token, i);

	switch(token)
	{
	case TOKEN_WORD: /* Parent device name */
		pszParentDeviceName = szParentDeviceName;
		break;
	case TOKEN_NUMBER: /* Port */
		/* Look for hostname */
		i = SkipWhiteSpace(psBuffer, i);
		if (psBuffer[i] != 0)
		{
			pszHostname = szHostname;

			i = GetNextToken(psBuffer, szHostname, &nIp1, &token, i);
			switch (token)
			{
			case TOKEN_WORD: /* Remote machine name */
				break;
			case TOKEN_NUMBER: /* Ip address */
				i = SkipWhiteSpace(psBuffer, i);
				if (psBuffer[i] != '.')
					ConfigFileError(psTargetConf, psBuffer, i, "<ip address>");
				else
					i++;

				i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nIp2, TOKEN_NUMBER, "<ip address>");
				i = SkipWhiteSpace(psBuffer, i);
				if (psBuffer[i] != '.')
					ConfigFileError(psTargetConf, psBuffer, i, "<ip address>");
				else
					i++;

				i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nIp3, TOKEN_NUMBER, "<ip address>");
				i = SkipWhiteSpace(psBuffer, i);
				if (psBuffer[i] != '.')
					ConfigFileError(psTargetConf, psBuffer, i, "<ip address>");
				else
					i++;

				i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nIp4, TOKEN_NUMBER, "<ip address>");
                
				sprintf(szHostname, "%u.%u.%u.%u", (IMG_UINT32)nIp1, (IMG_UINT32)nIp2, (IMG_UINT32)nIp3, (IMG_UINT32)nIp4);
				break;
			default:
				printf("Invalid Network Description on SOCKET_INFO config line, should be host name or ip address for device %s\n", szDeviceName);
				IMG_ASSERT(IMG_FALSE);
				break;
			}
		}
		break;
	default:
		printf("Invalid Port Number or Parent Device on SOCKET_INFO config line for device %s\n", szDeviceName);
		IMG_ASSERT(IMG_FALSE);
		break;
	}
    
	/* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");

	if (pszParentDeviceName != NULL)
	{
		/* Connect using parent device */
		return TCONF_AddDeviceIp(
				psTargetConf,
				szDeviceName,
				pszParentDeviceName,
				NULL,
				0,
				gbSocketBuffering);
	}
	else
	{
		/* Connect using hostname and port */
		return TCONF_AddDeviceIp(
				psTargetConf,
				szDeviceName,
				NULL,
				pszHostname,
				(IMG_UINT16)nPort,
				gbSocketBuffering);
	}
}

/*!
******************************************************************************

 @Function				TCONF_ProcessWR_DIRECT_INFO

 Syntax:

 DIRECT <device name> [ <parent name> ]

 ******************************************************************************/
static IMG_RESULT tconf_ProcessWR_DIRECT_INFO(
	TCONF_sTargetConf* psTargetConf,
	char *psBuffer,
	int i
)
{
	IMG_CHAR szDeviceName[256], szParentDeviceName[256], szGash[256];
	IMG_CHAR *pszParentDeviceName = IMG_NULL;
	TCONF_eToken token;
	IMG_UINT64 nGash;

	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>"); 

    i = SkipWhiteSpace(psBuffer, i);
	i = GetNextToken(psBuffer, szParentDeviceName, &nGash, &token, i);

	switch(token)
	{
	case TOKEN_WORD: /* Parent name */
		pszParentDeviceName = szParentDeviceName;
		/* Check for end-of-line */
		ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");
		break;
	case TOKEN_EOL:
		break;
	default:
		printf("Invalid DIRECT Device Interface definition: %s\n", psBuffer);
		IMG_ASSERT(IMG_FALSE);
		break;
	}

	return TCONF_AddDirectInterface(psTargetConf, szDeviceName, pszParentDeviceName);
}

/*!
******************************************************************************

 @Function				tconf_ProcessWR_BASE_NAME

 Syntax:

 BASE_NAME <device name> [<base reg name>]:[<base mem name>]

 ******************************************************************************/
static IMG_RESULT tconf_ProcessWR_BASE_NAME(
	TCONF_sTargetConf* psTargetConf,
	char * psBuffer,
	int i
)
{
	IMG_CHAR szDeviceName[256], szRegisterName[256], szMemoryName[256], szGash[256];
    IMG_CHAR *pszRegisterName = IMG_NULL, *pszMemoryName = IMG_NULL;
	IMG_UINT64 nGash;

	/* Get values */
	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	i = SkipWhiteSpace(psBuffer, i);

	/* Check the base name syntax */
	if (psBuffer[i] == ':')
	{
		i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_COLON, ":");
		i = ValidateNextToken(psTargetConf, psBuffer, i, szMemoryName, &nGash, TOKEN_WORD, "[<base mem name>]");
		pszMemoryName = szMemoryName;
	}
	else
	{
		i = ValidateNextToken(psTargetConf, psBuffer, i, szRegisterName, &nGash, TOKEN_WORD, "[<base reg name>]");
		i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_COLON, ":");
		pszRegisterName = szRegisterName;

		if (psBuffer[i] != 0)
		{
			i = ValidateNextToken(psTargetConf, psBuffer, i, szMemoryName, &nGash, TOKEN_WORD, "[<base mem name>]");
			pszMemoryName = szMemoryName;
		}
	}

	/* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");

	return TCONF_AddBaseName(psTargetConf, szDeviceName, pszRegisterName, pszMemoryName);
}

/*!
******************************************************************************

 @Function				tconf_ProcessWR_SUBDEVICE

 Syntax:

 SUB_DEVICE <sub device name> <device name> <device type M/R> <base address> <size>

 ******************************************************************************/
static IMG_RESULT tconf_ProcessWR_SUBDEVICE(
	TCONF_sTargetConf* psTargetConf,
	char * psBuffer,
	int i
)
{
	IMG_CHAR szDeviceName[256], szSubDeviceName[256], szType[256], szGash[256];
	IMG_UINT64 nBaseAddr, nSize, nGash;	
	
	/* Get values */
	i = ValidateNextToken(psTargetConf, psBuffer, i, szSubDeviceName, &nGash, TOKEN_WORD, "<sub device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szDeviceName, &nGash, TOKEN_WORD, "<device name>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szType, &nGash, TOKEN_WORD, "<device type M/R>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nBaseAddr, TOKEN_NUMBER, "<base address>");
	i = ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nSize, TOKEN_NUMBER, "<size>");

	/* Check for end-of-line */
	ValidateNextToken(psTargetConf, psBuffer, i, szGash, &nGash, TOKEN_EOL, "End-of-line");


	/* Validate device type */
	if (szType[0] != 'M' || szType[0] != 'R')
	{
		printf("ERROR: TARGET_CONFIG Sub Device Type not recognised in line %s\n", psBuffer);
		IMG_ASSERT(IMG_FALSE);
		return IMG_ERROR_INVALID_PARAMETERS;
	}


	/* Add sub device */
	return TCONF_AddDeviceSubDevice(
		psTargetConf,
		szDeviceName,
		szSubDeviceName,
		szType[0],
		nBaseAddr,
		nSize);
}

/*!
******************************************************************************

 @Function				tconf_ProcessMEMORY_MAP


******************************************************************************/
static
IMG_RESULT
tconf_ProcessMEMORY_MAP(
	TCONF_sTargetConf* 				psTargetConf,
    IMG_HANDLE	                    hFile,
	char *						    psBuffer,
	int							    i
)
{
	char					        sGash[256];
	char			                sCommand[BUFFER_SIZE];
	IMG_UINT64				        ui64Gash;
    int                             iSize;
	TCONF_eToken 	                eToken;
	IMG_RESULT						rReturnCode = IMG_SUCCESS;

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");

    /* Loop till end of file...*/
	do
    {
	    /* Read next line from config file */
	    if(IMG_FILEIO_Gets(hFile, BUFFER_SIZE, psBuffer) == IMG_FALSE)
	    {
            /* Close file...*/
			IMG_FILEIO_CloseFile(hFile);

			/* ERROR: No END_MEMORY_MAP */
			printf("ERROR: \"%s\" Line:%d No END_MEMORY_MAP\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

			return IMG_ERROR_FATAL;
	    }

        /* Update the line number...*/
        psTargetConf->ui32LineNo++;

	    /* Get length of string */
	    iSize = (int) strlen(psBuffer);

	    /* Skip white space */
	    i = SkipWhiteSpace(psBuffer, 0);

	    /* Look for blank line */
	    if (iSize == i)
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

	    /* Look for comment line */
	    if (
			    (iSize > i+1) &&
			    (psBuffer[i] == '-') && (psBuffer[i+1] == '-')
		    )
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

		/* Get command key word */
		i = GetNextToken(psBuffer, sCommand, &ui64Gash, &eToken, i);

		/* DEVICE ? */
		if (strcmp("DEVICE", sCommand) == 0)
		{
			rReturnCode = tconf_ProcessMM_DEVICE(psTargetConf, &psBuffer[0], i);
		}
		/* MEMORY_POOL ? */
		else if (strcmp("MEMORY_POOL", sCommand) == 0)
		{
			rReturnCode = tconf_ProcessMM_MEMORY_POOL(psTargetConf, &psBuffer[0], i);
		}
		/* REGISTER ? */
		else if (strcmp("REGISTER", sCommand) == 0)
		{
			rReturnCode = tconf_ProcessMM_REGISTER(psTargetConf, &psBuffer[0], i);
		}
		/* MEMORY ? */
		else if (strcmp("MEMORY", sCommand) == 0)
		{
			rReturnCode = tconf_ProcessMM_MEMORY(psTargetConf, &psBuffer[0], i);
		}
		// SLAVE PORT
		else if (strcmp("SLAVE_PORT", sCommand) == 0)
		{
			printf("WARNING: Target Config - Slave Ports have been deprecated");
		}
		/* END_MEMORY_MAP ? */
		else if (strcmp("END_MEMORY_MAP", sCommand) == 0)
		{
	        /* Check for end-of-line */
	        i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");
			return rReturnCode;
		}
		else
		{
			/* ERROR: Command not recognised */
			printf("%s\n", psBuffer);
			printf("ERROR: \"%s\" Line:%d Command not recognised \n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

			/* Assert */
			IMG_ASSERT(IMG_FALSE);
			rReturnCode = IMG_ERROR_GENERIC_FAILURE;
			break;
		}
    }
    while( IMG_SUCCESS == rReturnCode );

	return rReturnCode;
}


/*!
******************************************************************************

 @Function				tconf_ProcessPLATFORM_INIT

******************************************************************************/
static
IMG_RESULT
tconf_ProcessPLATFORM_INIT(
	TCONF_sTargetConf* 				psTargetConf,
    IMG_HANDLE 						hFile,
	char *						    psBuffer,
	int							    i
)
{
	char					        sGash[256];
	char			                sCommand[BUFFER_SIZE];
	IMG_UINT64				        ui64Gash;
    int                             iSize;
	TCONF_eToken 	                eToken;
	IMG_RESULT						rResult = IMG_SUCCESS;

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");

    /* Loop till end of file...*/
    do
    {
	    /* Read next line from config file */
	    if (IMG_FILEIO_Gets(hFile, BUFFER_SIZE, psBuffer) == IMG_FALSE)
	    {
            /* Close file...*/
			IMG_FILEIO_CloseFile(hFile);

			/* ERROR: No END_PLATFORM_INIT */
			printf("ERROR: \"%s\" Line:%d No END_PLATFORM_INIT\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

			return IMG_ERROR_FATAL;
	    }

        /* Update the line number...*/
        psTargetConf->ui32LineNo++;

	    /* Get length of string */
	    iSize = (int) strlen(psBuffer);

	    /* Skip white space */
	    i = SkipWhiteSpace(psBuffer, 0);

	    /* Look for blank line */
	    if (iSize == i)
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

	    /* Look for comment line */
	    if (
			    (iSize > i+1) &&
			    (psBuffer[i] == '-') && (psBuffer[i+1] == '-')
		    )
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

		/* Get command key word */
		i = GetNextToken(psBuffer, sCommand, &ui64Gash, &eToken, i);

		/* WRW ? */
		if (strcmp("WRW", sCommand) == 0)
		{
			rResult = tconf_ProcessPI_WRW(psTargetConf, &psBuffer[0], i);
		}
		/* END_PLATFORM_INIT ? */
		else if (strcmp("END_PLATFORM_INIT", sCommand) == 0)
		{
	        /* Check for end-of-line */
	        i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");
			break;
		}

		else
		{
			/* ERROR: Command not recognised */
			printf("%s\n", psBuffer);
			printf("ERROR: \"%s\" Line:%d Command not recognised \n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

			rResult = IMG_ERROR_INVALID_PARAMETERS;
		}
    }while (rResult == IMG_SUCCESS);
	return rResult;
}




/*!
******************************************************************************

 @Function				tconf_ProcessPDUMP_CONTEXT

 Syntax:

    CONTEXT <context name> <block name> [<block name>...]

******************************************************************************/
static
IMG_RESULT
tconf_ProcessPDUMP_CONTEXT(
  	TCONF_sTargetConf* 				psTargetConf,
	char *						    psBuffer,
	int							    i
)
{
	char					        sContextName[256];
	char					        sBlockName[256];
    IMG_UINT64                      ui64Gash;
	IMG_RESULT						 rResult = IMG_SUCCESS;
	TCONF_eToken 					eToken = TOKEN_WORD;

    /* Get context name...*/
	i = ValidateNextToken(psTargetConf, psBuffer, i, sContextName, &ui64Gash,   TOKEN_WORD,     "<context>");

	i = ValidateNextToken(psTargetConf, psBuffer, i, sBlockName, &ui64Gash,   TOKEN_WORD,     "<block name>");
    /* Get block names associated with this context...*/
   	do
    {
		rResult = TCONF_AddPdumpContext(psTargetConf, sBlockName, sContextName);
		if (rResult != IMG_SUCCESS) return rResult;
	    i = GetNextToken(psBuffer, sBlockName, &ui64Gash, &eToken, i);
    }while(eToken == TOKEN_WORD);
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				tconf_ProcessPDUMP

******************************************************************************/
static
IMG_RESULT
tconf_ProcessPDUMP(
    TCONF_sTargetConf* 				psTargetConf,
    IMG_HANDLE	                    hFile,
	char *						    psBuffer,
	int							    i
)
{
	char					        sGash[256];
	char			                sCommand[BUFFER_SIZE];
	IMG_UINT64				        ui64Gash;
    int                             iSize;
	TCONF_eToken 	                eToken;
	IMG_RESULT						rResult = IMG_SUCCESS;

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");

    /* Loop till end of file...*/
    do
    {
	    /* Read next line from config file */
	    if (IMG_FILEIO_Gets(hFile, BUFFER_SIZE, psBuffer) == IMG_FALSE)
	    {
            /* Close file...*/
			IMG_FILEIO_CloseFile(hFile);

			/* ERROR: No END_PDUMP */
			printf("ERROR: \"%s\" Line:%d No END_PDUMP\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

			return IMG_ERROR_FATAL;
	    }

        /* Update the line number...*/
        psTargetConf->ui32LineNo++;

	    /* Get length of string */
	    iSize = (int) strlen(psBuffer);

	    /* Skip white space */
	    i = SkipWhiteSpace(psBuffer, 0);

	    /* Look for blank line */
	    if (iSize == i)
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

	    /* Look for comment line */
	    if (
			    (iSize > i+1) &&
			    (psBuffer[i] == '-') && (psBuffer[i+1] == '-')
		    )
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

		/* Get command key word */
		i = GetNextToken(psBuffer, sCommand, &ui64Gash, &eToken, i);

		/* WRW ? */
		if (strcmp("CONTEXT", sCommand) == 0)
		{
			rResult = tconf_ProcessPDUMP_CONTEXT(psTargetConf, psBuffer, i);
		}
		/* END_PDUMP ? */
		else if (strcmp("END_PDUMP", sCommand) == 0)
		{
	        /* Check for end-of-line */
	        i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");
			break;
		}

		else
		{
			/* ERROR: Command not recognised */
			printf("%s\n", psBuffer);
			printf("ERROR: \"%s\" Line:%d Command not recognised \n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

			return IMG_ERROR_INVALID_PARAMETERS;
		}
    }while (rResult == IMG_SUCCESS);
	return rResult;
}


/*!
******************************************************************************

 @Function				TCONF_ProcessOverride

******************************************************************************/
IMG_RESULT TCONF_ProcessOverride(TCONF_sTargetConf *psTargetConf, eDevifTypes *peDevifType )
{
	IMG_CHAR*			pszCommand, *pszOverrideTemp;
	IMG_RESULT			rResult = IMG_SUCCESS;
		
	// Get command key word
	pszOverrideTemp = IMG_STRDUP(psTargetConf->pszOverride);
	pszCommand = strtok(pszOverrideTemp, " ");

	if (strcmp("SOCKET_INFO", pszCommand) == 0)
	{
		rResult = tconf_ProcessWR_SOCKET_INFO(psTargetConf, (char*)psTargetConf->pszOverride, (int)strlen(pszCommand));
		*peDevifType = DEVIF_TYPE_SOCKET;
	}
	else if(strcmp("DIRECT", pszCommand) == 0)
	{
		rResult = tconf_ProcessWR_DIRECT_INFO(psTargetConf, (char*)psTargetConf->pszOverride, (int)strlen(pszCommand));
		*peDevifType = DEVIF_TYPE_DIRECT;
	}
	else if (strcmp("DLL", pszCommand) == 0)
	{
		printf("ERROR: Use -sim... commands to enable transif interface\n");
	}
	else if (strcmp("NULL", pszCommand) == 0)
	{
		*peDevifType = DEVIF_TYPE_NULL;
	}
	else
	{
		printf("Wrapper Type not supported in command line override\n");
		rResult = IMG_ERROR_NOT_SUPPORTED;
	}
	IMG_FREE(pszOverrideTemp);
	return rResult;
}


/*!
******************************************************************************

 @Function				tconf_ProcessWRAPPER

******************************************************************************/
static
IMG_RESULT
tconf_ProcessWRAPPER(
    TCONF_sTargetConf* 				psTargetConf,
    IMG_HANDLE	                    hFile,
	char *						    psBuffer,
	int							    i
)
{
	char					        sGash[256];
	char			                sCommand[BUFFER_SIZE];
	IMG_UINT64				        ui64Gash;
    int                             iSize;
	TCONF_eToken 	                eToken;
	IMG_RESULT						rResult = IMG_SUCCESS;

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");
    
	/* Loop till end of file...*/
    do
    {
	    /* Read next line from config file */
	    if(IMG_FILEIO_Gets(hFile, BUFFER_SIZE, psBuffer) == IMG_FALSE)
	    {
            /* Close file...*/
			IMG_FILEIO_CloseFile(hFile);

			/* ERROR: No END_MEMORY_MAP */
			printf("ERROR: \"%s\" Line:%d No END_WRAPPER\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

			return IMG_ERROR_INVALID_PARAMETERS;
	    }

        /* Update the line number...*/
        psTargetConf->ui32LineNo++;

	    /* Get length of string */
	    iSize = (int) strlen(psBuffer);

	    /* Skip white space */
	    i = SkipWhiteSpace(psBuffer, 0);

	    /* Look for blank line */
	    if (iSize == i)
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

	    /* Look for comment line */
	    if (
			    (iSize > i+1) &&
			    (psBuffer[i] == '-') && (psBuffer[i+1] == '-')
		    )
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

		/* Get command key word */
		i = GetNextToken(psBuffer, sCommand, &ui64Gash, &eToken, i);

		/* CONTROL ? */
		if (strcmp("CONTROL", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_CONTROL(psTargetConf, psBuffer, i);
		}
		/* PCI_CARD_INFO ? */
		else if (strcmp("PCI_CARD_INFO", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_PCI_CARD_INFO(psTargetConf, psBuffer, i);
		}
		/* PCI_MEMORY_BASE ? */
		else if (strcmp("PCI_MEMORY_BASE", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_PCI_MEMORY_BASE(psTargetConf, psBuffer, i);
		}
		/* PCI_DEVICE_BASE ? */
		else if (strcmp("PCI_DEVICE_BASE", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_PCI_DEVICE_BASE(psTargetConf, psBuffer, i);
		}
		/* BURN_MEMORY ? */
		else if (strcmp("BURN_MEMORY", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_BURN_MEMORY(psTargetConf, psBuffer, i);
		}
		/* SOCKET_INFO ? */
		else if (strcmp("SOCKET_INFO", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_SOCKET_INFO(psTargetConf, psBuffer, i);
		}
		else if (strcmp("DIRECT", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_DIRECT_INFO(psTargetConf, psBuffer, i);
		}
		/* base_name ? */
		else if (strcmp("BASE_NAME", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_BASE_NAME(psTargetConf, psBuffer, i);
		}
		/* SUBDEVICE ? */
		else if (strcmp("SUBDEVICE", sCommand) == 0)
		{
			rResult = tconf_ProcessWR_SUBDEVICE(psTargetConf, psBuffer, i);
		}
		/* PDump1IF folders */
		else if (strcmp("PDUMP1IF_INFO", sCommand) == 0)
		{
			rResult = tconf_ProcessPDUMP1IF_INFO(psTargetConf, psBuffer, i);
		}
		/* Dash Info */
		else if (strcmp("DASH_INFO", sCommand) == 0)
		{
			rResult = tconf_ProcessDASH_INFO(psTargetConf, psBuffer, i);
		}
		/* Dash Info */
		else if (strcmp("DASH_DEVICE_BASE", sCommand) == 0)
		{
			rResult = tconf_ProcessDASH_DEVICE_BASE(psTargetConf, psBuffer, i);
		}
		/* Dash Info */
		else if (strcmp("DASH_MEMORY_BASE", sCommand) == 0)
		{
			rResult = tconf_ProcessDASH_MEMORY_BASE(psTargetConf, psBuffer, i);
		}
		else if (strcmp("POSTED_SETUP", sCommand) == 0)
		{
			rResult = tconf_ProcessPOSTED_SETUP(psTargetConf, psBuffer, i);
		}
		/* END_WRAPPER ? */
		else if (strcmp("END_WRAPPER", sCommand) == 0)
		{
	        /* Check for end-of-line */
	        i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");
			break;
		}
		else
		{
			/* ERROR: Command not recognised */
			printf("%s\n", psBuffer);
			printf("ERROR: \"%s\" Line:%d Command not recognised \n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);

			rResult = IMG_ERROR_INVALID_PARAMETERS;
		}
    }while (rResult == IMG_SUCCESS);
	return rResult;
}

/*!
******************************************************************************

 @Function				tconf_SkipTo

******************************************************************************/
static
IMG_RESULT
tconf_SkipTo(
    TCONF_sTargetConf* 				psTargetConf,
    IMG_HANDLE	                    hFile,
	char *						    psBuffer,
	int							    i,
	char *						    pszEnd
)
{
	char					        sGash[256];
	char			                sCommand[BUFFER_SIZE];
	IMG_UINT64				        ui64Gash;
    int                             iSize;
	TCONF_eToken 	                eToken;

	/* Check for end-of-line */
	i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");

    /* Loop till end of file...*/
    for(;;)
    {
	    /* Read next line from config file */
	    if( IMG_FILEIO_Gets(hFile, BUFFER_SIZE, psBuffer) == IMG_FALSE)
	    {
            /* Close file...*/
			IMG_FILEIO_CloseFile(hFile);

			/* ERROR: No END_MEMORY_MAP */
			printf("ERROR: \"%s\" Line:%d No %s\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo, pszEnd);

			return IMG_ERROR_INVALID_PARAMETERS;
	    }

        /* Update the line number...*/
        psTargetConf->ui32LineNo++;

	    /* Get length of string */
	    iSize = (int) strlen(psBuffer);

	    /* Skip white space */
	    i = SkipWhiteSpace(psBuffer, 0);

	    /* Look for blank line */
	    if (iSize == i)
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

	    /* Look for comment line */
	    if (
			    (iSize > i+1) &&
			    (psBuffer[i] == '-') && (psBuffer[i+1] == '-')
		    )
	    {
		    /* Continue and tconf_ProcessMM_ next line */
		    continue;
	    }

		/* Get command key word */
		i = GetNextToken(psBuffer, sCommand, &ui64Gash, &eToken, i);

		/* pszEnd ? */
		if (strcmp(pszEnd, sCommand) == 0)
		{
	        /* Check for end-of-line */
	        i = ValidateNextToken(psTargetConf, psBuffer, i, sGash,          &ui64Gash,   TOKEN_EOL, "End-of-line");
			break;
		}
    }
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				TCONF_TXT_TargetConfigure

******************************************************************************/
IMG_RESULT TCONF_TXT_TargetConfigure(
	TCONF_sTargetConf* psTargetConf,
	TAL_pfnTAL_MemSpaceRegister pfnTAL_MemSpaceRegister
)
{
    IMG_HANDLE			hFile = NULL;
	char			    sBuffer[BUFFER_SIZE];
	char			    sCommand[BUFFER_SIZE];
    int                 iSize;
    int                 i;
	TCONF_eToken 	    eToken;
	IMG_UINT64		    ui64Gash;
    IMG_BOOL            bMemoryMapProcessed = IMG_FALSE;
    IMG_BOOL            bWrapper = IMG_FALSE;
    IMG_BOOL            bPlatformInit = IMG_FALSE;
    IMG_BOOL            bPdump = IMG_FALSE;
	IMG_RESULT			rReturnCode = IMG_SUCCESS;
	TCONF_psMemSpItem	psMemSpListItem;

	/* Open the config file...*/
	if ( IMG_FILEIO_OpenFile(psTargetConf->pszConfigFile, "r", &hFile, IMG_FALSE) != IMG_SUCCESS )
	{
		// First check for gzipped version of file
		IMG_CHAR* pszZippedFile;
		pszZippedFile = IMG_MALLOC(strlen(psTargetConf->pszConfigFile) + 4);
		sprintf(pszZippedFile, "%s.gz", psTargetConf->pszConfigFile);
		if (IMG_FILEIO_OpenFile(pszZippedFile, "r", &hFile, IMG_TRUE) != IMG_SUCCESS)
		{
			printf("ERROR: Failed to open file ""%s""\n", psTargetConf->pszConfigFile);
			return IMG_ERROR_FATAL;
		}
		IMG_FREE(pszZippedFile);		
	}
		
    /* Loop till end of file - process the MEMORY_MAP section...*/
	do
    {
	    /* Read next line from config file */
	    if (IMG_FILEIO_Gets(hFile, sizeof(sBuffer), sBuffer) == IMG_FALSE)
	    {
            /* Close file...*/
			IMG_FILEIO_CloseFile(hFile);
			hFile = NULL;

		    /* We have finished...*/
		    break;
	    }

        /* Update the line number...*/
        psTargetConf->ui32LineNo++;

	    /* Get length of string */
	    iSize = (int) strlen(sBuffer);

	    /* Skip white space */
	    i = SkipWhiteSpace(sBuffer, 0);

	    /* Look for blank line */
	    if (iSize == i)
	    {
		    /* Continue and process next line */
		    continue;
	    }

	    /* Look for comment line */
	    if (
			    (iSize > i+1) &&
			    (sBuffer[i] == '-') && (sBuffer[i+1] == '-')
		    )
	    {
		    /* Continue and process next line */
		    continue;
	    }

		/* Get command key word */
		i = GetNextToken(sBuffer, sCommand, &ui64Gash, &eToken, i);

 		/* MEMORY_MAP section ? */
		if (strcmp("MEMORY_MAP", sCommand) == 0)
		{
            /* Only allow one instance of a section...*/
            if (bMemoryMapProcessed)
            {
			    /* ERROR: 2nd %s encountered */
			    printf("%s\n", sBuffer);
			    printf("ERROR: \"%s\" Line:%d 2nd %s encountered\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo, sCommand);
				return IMG_ERROR_FATAL;
            }
            bMemoryMapProcessed = IMG_TRUE;
			rReturnCode = tconf_ProcessMEMORY_MAP(psTargetConf, hFile, &sBuffer[0], i);
        }
		/* WRAPPER ? */
		else if (strcmp("WRAPPER", sCommand) == 0)
		{
            /* Only allow one instance of a section...*/
            if (bWrapper)
            {
			    /* ERROR: 2nd %s encountered */
			    printf("%s\n", sBuffer);
			    printf("ERROR: \"%s\" Line:%d 2nd %s encountered\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo, sCommand);
				return IMG_ERROR_FATAL;
            }
            if (!bMemoryMapProcessed)
            {
            	printf("ERROR: \"%s\" Line:%d Wrapper Section must come after Memory Map\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);
            	IMG_ASSERT(bMemoryMapProcessed);
            }
            bWrapper = IMG_TRUE;
            rReturnCode = tconf_ProcessWRAPPER(psTargetConf, hFile, &sBuffer[0], i);
		}
		/* PLATFORM_INIT ? */
		else if (strcmp("PLATFORM_INIT", sCommand) == 0)
		{
            /* Only allow one instance of a section...*/
            if (bPlatformInit)
            {
			    /* ERROR: 2nd %s encountered */
			    printf("%s\n", sBuffer);
			    printf("ERROR: \"%s\" Line:%d 2nd %s encountered\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo, sCommand);

			    /* Assert */
			    IMG_ASSERT(IMG_FALSE);	
				return IMG_ERROR_FATAL;

            }
            bPlatformInit = IMG_TRUE;
            rReturnCode = tconf_SkipTo(psTargetConf, hFile, &sBuffer[0], i, "END_PLATFORM_INIT");
		}
		/* PDUMP ? */
		else if (strcmp("PDUMP", sCommand) == 0)
		{
            /* Only allow one instance of a section...*/
            if (bPdump)
            {
			    /* ERROR: 2nd %s encountered */
			    printf("%s\n", sBuffer);
			    printf("ERROR: \"%s\" Line:%d 2nd %s encountered\n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo, sCommand);

			    /* Assert */
			    IMG_ASSERT(IMG_FALSE);
				return IMG_ERROR_FATAL;
            }         
            bPdump = IMG_TRUE;
            rReturnCode = tconf_ProcessPDUMP(psTargetConf, hFile, &sBuffer[0], i);
		}

		else
		{
			/* ERROR: Section not recognised */
			printf("%s\n", sBuffer);
			printf("ERROR: \"%s\" Line:%d Section not recognised \n", psTargetConf->pszConfigFile, psTargetConf->ui32LineNo);
			return IMG_ERROR_FATAL;
		}

    }
	while( IMG_SUCCESS == rReturnCode );

	/* return now if an error has occured */
	if( IMG_SUCCESS != rReturnCode )
	{
		return rReturnCode;
	}


	// Setup TAL Memspaces
	psMemSpListItem = (TCONF_psMemSpItem)LST_first(&psTargetConf->sMemSpItemList);
    while (psMemSpListItem != IMG_NULL)
    {
    	pfnTAL_MemSpaceRegister(&psMemSpListItem->sTalMemSpaceCB);
    	psMemSpListItem = (TCONF_psMemSpItem)LST_next(psMemSpListItem);
    }

    /* If platform init section found...*/
    if (bPlatformInit)
    {
        	/* Open the config file...*/
		if ( IMG_FILEIO_OpenFile(psTargetConf->pszConfigFile, "r", &hFile, IMG_FALSE) != IMG_SUCCESS )
		{
			// First check for gzipped version of file
			IMG_CHAR* pszZippedFile;
			pszZippedFile = IMG_MALLOC(strlen(psTargetConf->pszConfigFile) + 4);
			sprintf(pszZippedFile, "%s.gz", psTargetConf->pszConfigFile);
			if (IMG_FILEIO_OpenFile(pszZippedFile, "r", &hFile, IMG_TRUE) != IMG_SUCCESS)
			{
				printf("ERROR: Failed to open file ""%s""\n", psTargetConf->pszConfigFile);
				return IMG_ERROR_FATAL;
			}
			IMG_FREE(pszZippedFile);		
		}
        psTargetConf->ui32LineNo = 0;

        /* Loop till end of file - process the PLATFORM_INIT section...*/
        do
        {
	        /* Read next line from config file */
			if(IMG_FILEIO_Gets(hFile, sizeof(sBuffer), sBuffer) == IMG_FALSE)
	        {
                /* Close file...*/
				IMG_FILEIO_CloseFile(hFile);

		        /* We have finished...*/
		        break;
	        }

            /* Update the line number...*/
            psTargetConf->ui32LineNo++;

	        /* Get length of string */
	        iSize = (int) strlen(sBuffer);

	        /* Skip white space */
	        i = SkipWhiteSpace(sBuffer, 0);

	        /* Look for blank line */
	        if (iSize == i)
	        {
		        /* Continue and process next line */
		        continue;
	        }

	        /* Look for comment line */
	        if (
			        (iSize > i+1) &&
			        (sBuffer[i] == '-') && (sBuffer[i+1] == '-')
		        )
	        {
		        /* Continue and process next line */
		        continue;
	        }

	        /* Get command key word */
	        i = GetNextToken(sBuffer, sCommand, &ui64Gash, &eToken, i);

 	        /* PLATFORM_INIT section ? */
	        if (strcmp("PLATFORM_INIT", sCommand) == 0)
	        {
                rReturnCode = tconf_ProcessPLATFORM_INIT(psTargetConf, hFile, &sBuffer[0], i);
            }
        }while (rReturnCode == IMG_SUCCESS);
    } 

	return rReturnCode;
}

