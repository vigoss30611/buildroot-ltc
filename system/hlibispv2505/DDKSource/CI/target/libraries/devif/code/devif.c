/*! 
******************************************************************************
 @file   : devif.c

 @brief  

 @Author Ray Livesley

 @date   15/05/2007
 
         <b>Copyright 2007 by Imagination Technologies Limited.</b>\n
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
         This file contains generic DEVIF functions.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/



#ifdef WIN32
	#include <windows.h>
	#include <winioctl.h>
#else  //Linux
	#include <fcntl.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/mman.h>
	#include <errno.h>
	#include <string.h>
	#include <sys/ioctl.h>
#ifdef ENABLE_PCI_DEVIF
	#include <imgpcidd.h>
#endif
#endif

#include <tconf.h>
#include <stdio.h>
#include "devif_config.h"

#ifdef WIN32

/* Global List device(s).  */
//extern TCONF_sDeviceCB *     gpsListDeviceCB;

#endif

/*!
******************************************************************************
  This type defines the mode in which the functions are working
******************************************************************************/
typedef enum
{
    DEVIF_MODE_PCI,
    DEVIF_MODE_BMEM,
	DEVIF_MODE_SOCKET,
	DEVIF_MODE_DIRECT,

} DEVIF_eMode;

DEVIF_eMode geMode;


/*!
******************************************************************************

 @Function				DEVIF_ConfigureDevice

******************************************************************************/
IMG_VOID
#if !defined (DEVIF_DEFINE_DEVIF1_FUNCS)
DEVIF_ConfigureDevice(
#else
DEVIF1_ConfigureDevice(
#endif
    DEVIF_sDeviceCB *   psDeviceCB)
{
    DEVIF_sWrapperControlInfo*      psWrapperControlInfo = &psDeviceCB->sDevIfSetup.sWrapCtrlInfo;
    

    /* Start interface according to type...*/
    switch (psWrapperControlInfo->eDevifType)
    {
    case DEVIF_TYPE_PCI:
#ifdef ENABLE_PCI_DEVIF
		PCIIF_DevIfConfigureDevice(psDeviceCB);
        geMode = DEVIF_MODE_PCI;
#else
        printf("PCI interface not enabled in this build\n");
        IMG_ASSERT(IMG_FALSE);
#endif
        break;

    case DEVIF_TYPE_BMEM:
#ifdef ENABLE_BURNMEM_DEVIF
        BMEM_DevIfConfigureDevice(psDeviceCB);
        geMode = DEVIF_MODE_BMEM;
#else
        printf("Burnmem interface not enabled in this build\n");
        IMG_ASSERT(IMG_FALSE);
#endif
        break;

    case DEVIF_TYPE_SOCKET:
#ifdef ENABLE_SOCKET_DEVIF
		SIF_DevIfConfigureDevice(psDeviceCB);
		geMode = DEVIF_MODE_SOCKET;		
#else
        printf("Socket interface not enabled in this build\n");
        IMG_ASSERT(IMG_FALSE);
#endif
		break;
	case DEVIF_TYPE_DASH:
#ifdef ENABLE_DASH_DEVIF
		DASHIF_DevIfConfigureDevice(psDeviceCB);
#else
		printf("Dash interface not enabled in this build\n");
        IMG_ASSERT(IMG_FALSE);
#endif
		break;

	case DEVIF_TYPE_PDUMP1:
#ifdef ENABLE_PDUMP1_DEVIF
        PDUMP1IF_DevIfConfigureDevice(psDeviceCB);
#else
        printf("PDump1 feed interface not enabled in this build\n");
        IMG_ASSERT(IMG_FALSE);
#endif
		break;
	case DEVIF_TYPE_DIRECT:
#ifdef ENABLE_DIRECT_DEVIF
        DIRECT_DEVIF_ConfigureDevice(psDeviceCB);
#else
        printf("Direct Interface feed interface not enabled in this build\n");
        IMG_ASSERT(IMG_FALSE);
#endif
		geMode = DEVIF_MODE_DIRECT;
		break;

	case DEVIF_TYPE_NULL:
#ifdef ENABLE_NULL_DEVIF

        DEVIF_NULL_Setup(psDeviceCB);
#else
        printf("Null interface not enabled in this build\n");
        IMG_ASSERT(IMG_FALSE);
#endif
		break;    
		
		case DEVIF_TYPE_POSTED:
#if ((defined ENABLE_POSTED_DEVIF) && (defined ENABLE_PCI_DEVIF))
		POSTED_DEVIF_ConfigureDevice(psDeviceCB);
#else
        printf("Posted Interface not enabled in this build\n");
        IMG_ASSERT(IMG_FALSE);
#endif
		break;
    default:
		printf("ERROR: Device interface not recognised\n");
        IMG_ASSERT(IMG_FALSE);
    }

#ifdef DEVIF_DEBUG
	DEVIF_AddDebugOutput(psDeviceCB);
#endif
}


/*!
******************************************************************************

 @Function				DEVIF_CopyAbsToHost

******************************************************************************/
IMG_VOID DEVIF_CopyAbsToHost (
    IMG_UINT64          ui64AbsAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
)
{
#ifdef WIN32
    switch (geMode)
    {
    case DEVIF_MODE_PCI:
        {
#ifdef ENABLE_PCI_DEVIF
			PCIIF_CopyAbsToHost(ui64AbsAddress, pvHostAddress, ui32Size);
#endif
        }
        break;
    case DEVIF_MODE_BMEM:
        {
#ifdef ENABLE_BURNMEM_DEVIF
			BMEM_CopyAbsToHost(ui64AbsAddress, pvHostAddress, ui32Size);
#endif
        }
        break;
    default:
		printf("ERROR: DEVIF_CopyAbsToHost not through this device interface\n");
        IMG_ASSERT(IMG_FALSE);
    }
#else
		IMG_ASSERT(IMG_FALSE);
#endif
}

/*!
******************************************************************************

 @Function              DEVIF_FillAbs

******************************************************************************/
IMG_VOID DEVIF_FillAbs (
    IMG_UINT64          ui64AbsAddress,
    IMG_UINT8           ui8Value,
    IMG_UINT32          ui32Size
)
{
#ifdef WIN32

    switch (geMode)
    {
        case DEVIF_MODE_PCI:
#ifdef ENABLE_PCI_DEVIF
			PCIIF_FillAbs(ui64AbsAddress, ui8Value, ui32Size);
#endif
            break;

        case DEVIF_MODE_BMEM:
#ifdef ENABLE_BURNMEM_DEVIF
            BMEM_FillAbs(ui64AbsAddress, ui8Value, ui32Size);
#endif
            break;
        default:
            printf("Memory Fill not available through this device interface.\n");
            IMG_ASSERT(IMG_FALSE);
    }
#else
		IMG_ASSERT(IMG_FALSE);
#endif
}

