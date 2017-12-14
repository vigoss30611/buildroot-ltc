/*!
******************************************************************************
 @file   : wrap.c

 @brief  

 @Author Ray Livesley

 @date   02/04/2008
 
         <b>Copyright 2008 by Imagination Technologies Limited.</b>\n
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
         Wrapper for use with the Portablity Framework.

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/

#include <tal.h>
#include <wrap.h>
#include <wrap_macros.h>
#include <target_config.h>
#include <dman_api.h>

#include <wrap_utils.h>
#include <page_alloc.h>
#include <memory.h>

#ifdef WINCE
	#include <pciddif.h>
	extern HANDLE ghBridge;
#endif

#define MAX_CHECK_MEM_COUNT 100

#define WRAP_PAGE_SIZE			(4*1024)					//<! Supported page size
#define WRAP_NO_DEV_PAGE_ADDRS	(20)						//<! Number of cached page addresses

static IMG_BOOL	    gbWarpInitialised	= IMG_FALSE;		//<! IMG_TRUE when initialised

static TCONF_sWrapperControlInfo	gsWrapperControlInfo;	//<! Wrapper control info
#define WRAP_CONTROL_STUB_OUT_DEVICE	((psDeviceCB->ui32Flags & TCONF_DEVFLAG_STUB_DEVICE_MASK)	>> TCONF_DEVFLAG_STUB_DEVICE_SHIFT)

static IMG_UINT32   gui32AppFlags = 0;						//<! Application flags for controling the wrapper
#define WRAP_STUB_OUT_TARGET    ((gui32AppFlags & WRAPAPP_STUB_OUT_TARGET) != 0)



/*!
******************************************************************************
 This structure contains device information.
******************************************************************************/
typedef struct
{
	IMG_UINT32			ui32ConnId;			//<! DMAN Connection Id
	IMG_UINT32			ui32DeviceId;		//<! DMAN Device Id
	IMG_UINT32			ui32WrapUAttachId;	//<! WRAPU Attachment Id
	IMG_UINT32 *		pui32Registers;		//<! Pointer to registers
	IMG_UINT32 *		pui32SlavePort;		//<! Pointer to slave ports

} WRAP_sDevInfo;

static IMG_INT32 MEMMSP_CopyDeviceVirtToHostRegion (	
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32DevVirtAddr,
	IMG_UINT32						ui32DevVirtQualifier,
	IMG_UINT32						ui32Size,
	IMG_VOID *						pvHostBuf
);
static IMG_INT32 
MEMMSP_MemSpacePollWord (	
	IMG_VOID *						pWRAPContext,
	WRAP_psDeviceMemCB				psWRAPDeviceMemCB,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32FuncId,
	WRAP_pfnCheckFunc				pfnCheckFunc,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Mask,
	IMG_UINT32    					ui32PollCount,
	IMG_UINT32						ui32TimeOut
);




/*!
******************************************************************************

 @Function              WRAP_SetApplicationFlags

******************************************************************************/
IMG_VOID WRAP_SetApplicationFlags(IMG_UINT32    ui32AppFlags)
{
    /* Set local copy of the application flags...*/
    if (ui32AppFlags != 0)
    {
        gui32AppFlags |= ui32AppFlags;
    }
    else
    {
        gui32AppFlags = ui32AppFlags;
    }
}



/*!
******************************************************************************

 @Function              WRAP_GetApplicationFlags

******************************************************************************/
IMG_UINT32 WRAP_GetApplicationFlags(IMG_VOID)
{
    return gui32AppFlags;
}


/*!
******************************************************************************

 @Function				wrap_Deinitialise
 

******************************************************************************/
static IMG_UINT32 wrap_Deinitialise ()
{
	if (gbWarpInitialised)
	{
        TCONF_Deinitialise ();
		gbWarpInitialised = IMG_FALSE;
	}
		
	/* Success */
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              wrap_Initialise

******************************************************************************/
static IMG_VOID wrap_Initialise(
    TCONF_sDeviceCB *               psDeviceCB
)
{
	if (!gbWarpInitialised)
	{
        /* Get wrapper control information...*/
        TCONF_GetWrapperControlInfo (&gsWrapperControlInfo);

        /* Set the de-initialise function */
	    TAL_RegisterWrapDeinitialise (wrap_Deinitialise);

		/* We are now initialised...*/
		gbWarpInitialised = IMG_TRUE;
	}
}


/*!
******************************************************************************

 @Function              wrap_Idle

******************************************************************************/
static IMG_INT32 wrap_Idle (	
	TCONF_sDeviceCB *					psDeviceCB,
	IMG_UINT32							ui32Value
)
{
    DEVIF_sDeviceCB *					psDevIfDeviceCB		= &psDeviceCB->sDevIfDeviceCB;
	volatile IMG_UINT32					ui32StartTime;

	/* Wait base on host/target cycle ration...*/
	ui32StartTime = ui32Value * gsWrapperControlInfo.ui32HostTargetCycleRatio;
	while (ui32StartTime != 0)
	{
		ui32StartTime--;
	}

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              REGMSP_GetMemSpaceFlags

******************************************************************************/
IMG_INT32 
REGMSP_GetMemSpaceFlags(	
	IMG_VOID *						pWRAPContext
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB		= (REGMSP_sRegisterContextCB *) pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB			= psRegContextCB->psDeviceCB;
	IMG_UINT32						ui32MemSpaceFlags	= 0;

	/* If device stubbed out...*/
	if (WRAP_CONTROL_STUB_OUT_DEVICE)
	{
		ui32MemSpaceFlags |= TAL_MEMSPFLAGS_STUB_OUT_DEVICE;
	}

	/* Return flags...*/
	return ui32MemSpaceFlags;
}

/*!
******************************************************************************

 @Function              REGMSP_Initialise

******************************************************************************/
IMG_INT32 
REGMSP_Initialise(	
	IMG_VOID *						pWRAPContext
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB		= (REGMSP_sRegisterContextCB *) pWRAPContext;
	TCONF_sDeviceCB *               psDeviceCB			= psRegContextCB->psDeviceCB;
	DEVIF_sDeviceCB *               psDevIfDeviceCB		= &psDeviceCB->sDevIfDeviceCB;
	IMG_UINT32						ui32I;

    /* Check we are initialised...*/
    wrap_Initialise(psDeviceCB);
	
	/* If we have not initilaised...*/
	if (!psRegContextCB->bInitialised) 
	{
		for( ui32I=0; ui32I< TAL_NUM_INT_MASKS; ui32I++ )
		{
			psRegContextCB->aui32intmask[ui32I] = 0;
		}

		ui32I = (psRegContextCB->ui32intnum & ~(0x20-1))>>5;
		if( ui32I < TAL_NUM_INT_MASKS )
		{
			psRegContextCB->aui32intmask[ui32I] = (1<< ((psRegContextCB->ui32intnum)&(0x20-1)) );
		}
		/* Now intialised */
		psRegContextCB->bInitialised = IMG_TRUE;
	}
    
    /* Set base address within the TAL..*/
    TAL_SetBaseAddress (pWRAPContext, psRegContextCB->ui32BaseAddr);

	/* Register additional callback function with the TAL...*/
	TAL_RegisterWrapFunction(TAL_FN_TYPE_DEINIT,			pWRAPContext,(IMG_VOID *)&REGMSP_Deinitialise);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_GET_FLAGS,			pWRAPContext,(IMG_VOID *)&REGMSP_GetMemSpaceFlags);
	

	/* Success */
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              REGMSP_Deinitialise

******************************************************************************/
IMG_INT32
REGMSP_Deinitialise(
	IMG_VOID *						pWRAPContext
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB		= (REGMSP_sRegisterContextCB *) pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB			= psRegContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB     = &psDeviceCB->sDevIfDeviceCB;
	WRAP_sDevInfo *					psDevInfo			= psDevIfDeviceCB->pvDevIfInfo;

	/* Free the device info structure...*/
	if (psDevInfo != IMG_NULL)
	{
		IMG_FREE(psDevInfo);
		psDevIfDeviceCB->pvDevIfInfo = IMG_NULL;
	}

	/* Success */
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              REGMSP_RegWriteWords

******************************************************************************/
IMG_INT32
REGMSP_RegWriteWords (
	IMG_VOID *						pWRAPContext,
	IMG_UINT32 *					pui32MemSource,
	IMG_UINT32						ui32Offset,
	IMG_UINT32    					ui32WordCount
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB		= (REGMSP_sRegisterContextCB *) pWRAPContext;
	TCONF_sDeviceCB *               psDeviceCB			= psRegContextCB->psDeviceCB;
	DEVIF_sDeviceCB *				psDevIfDeviceCB		= &psDeviceCB->sDevIfDeviceCB;
	IMG_UINT32						i;

	IMG_ASSERT(ui32Offset < psRegContextCB->ui32MaxOffset);
	IMG_ASSERT((ui32Offset & 0x3) == 0);

	/* Write words to register...*/
	for (i=0; i<ui32WordCount; i++)
	{
		REGMSP_WriteWord(pWRAPContext, ui32Offset + (i * sizeof(IMG_UINT32)), pui32MemSource[i]);
	}

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              REGMSP_Idle

******************************************************************************/
IMG_INT32 
REGMSP_Idle (	
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32Value
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB		= (REGMSP_sRegisterContextCB *) pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB			= psRegContextCB->psDeviceCB;

	return wrap_Idle(psDeviceCB, ui32Value);
}


/*!
******************************************************************************

 @Function              REGMSP_WriteWord

******************************************************************************/
IMG_INT32 
REGMSP_WriteWord(
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32Value
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB		= (REGMSP_sRegisterContextCB *) pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB			= psRegContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB		= &psDeviceCB->sDevIfDeviceCB;
	WRAP_sDevInfo *					psDevInfo			= psDevIfDeviceCB->pvDevIfInfo;

	IMG_ASSERT(ui32Offset					< psRegContextCB->ui32MaxOffset);
    IMG_ASSERT((ui32Offset & 0x3)			== 0);
	IMG_ASSERT(psDevInfo->pui32Registers	!= IMG_NULL);

	/* Write value to register...*/
	psDevInfo->pui32Registers[(psRegContextCB->ui32DeviceBaseAddr + psRegContextCB->ui32BaseAddr + ui32Offset)>>2] = ui32Value;

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              REGMSP_ReadWord

******************************************************************************/
IMG_INT32 
REGMSP_ReadWord(
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32Offset,
	IMG_PUINT32						pui32Value
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB		= (REGMSP_sRegisterContextCB *) pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB			= psRegContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB		= &psDeviceCB->sDevIfDeviceCB;
	WRAP_sDevInfo *					psDevInfo			= psDevIfDeviceCB->pvDevIfInfo;

	IMG_ASSERT(ui32Offset					< psRegContextCB->ui32MaxOffset);
    IMG_ASSERT((ui32Offset & 0x3)			== 0);
	IMG_ASSERT(psDevInfo->pui32Registers	!= IMG_NULL);

	/* Read register...*/
	*pui32Value = psDevInfo->pui32Registers[(psRegContextCB->ui32DeviceBaseAddr + psRegContextCB->ui32BaseAddr + ui32Offset)>>2];

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              REGMSP_Poll

******************************************************************************/
IMG_INT32 
REGMSP_Poll(
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32Offset,
	WRAP_pfnCheckFunc				pfnCheckFunc,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Mask,
	IMG_UINT32    					ui32PollCount,
	IMG_UINT32						ui32TimeOut
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB		= (REGMSP_sRegisterContextCB *) pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB			= psRegContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB		= &psDeviceCB->sDevIfDeviceCB;
	IMG_UINT32				        ui32RegValue;
	IMG_BOOL				        bResult				= IMG_FALSE;
	IMG_BOOL				        bInfinitePollCount	= IMG_FALSE;

	if (ui32PollCount == TAL_POLLCOUNT_INFINITE)
	{
		bInfinitePollCount = IMG_TRUE;
	}

	/* If we are not stubing out the target or this device...*/
	if ( (!WRAP_STUB_OUT_TARGET) && (!WRAP_CONTROL_STUB_OUT_DEVICE) )
    {
	    while (!bResult && (ui32PollCount > 0))
	    {
		    /* Read value from register */
		    REGMSP_ReadWord(pWRAPContext, ui32Offset, &ui32RegValue);

		    /* Call poll function to evaluate the result */
		    bResult = pfnCheckFunc(pWRAPContext, ui32RegValue, ui32RequValue, ui32Mask);

			/* If there was no match...*/
			if (!bResult)
			{
				/* Wait the specified number of cycles */
				REGMSP_Idle(pWRAPContext, ui32TimeOut);

				if (!bInfinitePollCount)
				{
					ui32PollCount --;
				}
			}
	    }
    }
    else
    {
    	bResult = IMG_TRUE;
    }

	/* If IMG_TRUE...*/
	if (bResult)
	{
		/* Return Success...*/
		return IMG_SUCCESS;
	}

	/* Return time-out */
	return IMG_ERROR_TIMEOUT;
}


/*!
******************************************************************************

 @Function              REGMSP_GetInterruptMask

******************************************************************************/
IMG_INT32 
REGMSP_GetInterruptMask(
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						aui32IntMasks[]
)
{
	REGMSP_sRegisterContextCB *		psRegContextCB = (REGMSP_sRegisterContextCB *) pWRAPContext;
	IMG_UINT32						ui32I;

	for (ui32I = 0; ui32I < TAL_NUM_INT_MASKS; ui32I++)
	{
		aui32IntMasks[ui32I] = psRegContextCB->aui32intmask[ui32I];
	}

	/* Return Success...*/
	return IMG_SUCCESS;

}



/*!
******************************************************************************

 @Function              MEMMSP_GetMemSpaceFlags

******************************************************************************/
IMG_INT32 
MEMMSP_GetMemSpaceFlags(	
	IMG_VOID *						pWRAPContext
)
{
    MEMMSP_sMemoryContextCB *       psMemoryContextCB = pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB        = psMemoryContextCB->psDeviceCB;
	IMG_UINT32						ui32MemSpaceFlags = 0;

	/* If device stubbed out...*/
	if (WRAP_CONTROL_STUB_OUT_DEVICE)
	{
		ui32MemSpaceFlags |= TAL_MEMSPFLAGS_STUB_OUT_DEVICE;
	}

	/* Return flags...*/
	return ui32MemSpaceFlags;
}


/*!
******************************************************************************

 @Function              MEMMSP_Idle

******************************************************************************/
IMG_INT32 
MEMMSP_Idle (	
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32Value
)
{
	MEMMSP_sMemoryContextCB *		psRegContextCB	= (MEMMSP_sMemoryContextCB *) pWRAPContext;
	TCONF_sDeviceCB *               psDeviceCB		= psRegContextCB->psDeviceCB;

	return wrap_Idle(psDeviceCB, ui32Value);
}


/*!
******************************************************************************

 @Function              MEMMSP_Initialise

******************************************************************************/
IMG_INT32 
MEMMSP_Initialise(	
	IMG_VOID *						pWRAPContext
)
{
    MEMMSP_sMemoryContextCB *       psMemoryContextCB = pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB        = psMemoryContextCB->psDeviceCB;

    /* Check we are initialised...*/
    wrap_Initialise(psDeviceCB);
	
	/* Register additional callback function with the TAL...*/
	TAL_RegisterWrapFunction(TAL_FN_TYPE_COPY_DEV_VIRT,		pWRAPContext,(IMG_VOID *)&MEMMSP_CopyDeviceVirtToHostRegion);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_MEMORY_POLL,		pWRAPContext,(IMG_VOID *)&MEMMSP_MemSpacePollWord);;
	TAL_RegisterWrapFunction(TAL_FN_TYPE_MEM_READ_WORD,		pWRAPContext,(IMG_VOID *)&MEMMSP_ReadWord);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_DEINIT,			pWRAPContext,(IMG_VOID *)&MEMMSP_Deinitialise);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_GET_FLAGS,			pWRAPContext,(IMG_VOID *)&MEMMSP_GetMemSpaceFlags);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_IDLE,				pWRAPContext,(IMG_VOID *)&MEMMSP_Idle);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_MEM_WRITE_WORD,	pWRAPContext,(IMG_VOID *)&MEMMSP_WriteWord);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_COPY_DEVICE_TO_HOST, pWRAPContext,(IMG_VOID *)&MEMMSP_CopyDeviceToHostMemRegion);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_COPY_HOST_TO_DEVICE, pWRAPContext,(IMG_VOID *)&MEMMSP_CopyHostToDeviceMemRegion);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_FREE_DEVICE_MEM,	pWRAPContext,(IMG_VOID *)&MEMMSP_FreeDeviceMem);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_MALLOC_DEVICE_MEM,	pWRAPContext,(IMG_VOID *)&MEMMSP_MallocDeviceMem);

    /* If the device memory has not been initalised...*/
    if (!psDeviceCB->bMemInitialised)
    {

        /* Memory now initialised...*/
        psDeviceCB->bMemInitialised = IMG_TRUE;
    }

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MEMMSP_Deinitialise

******************************************************************************/
IMG_INT32 
MEMMSP_Deinitialise(	
	IMG_VOID *						pWRAPContext
)
{
    MEMMSP_sMemoryContextCB *       psMemoryContextCB   = pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB          = psMemoryContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB     = &psDeviceCB->sDevIfDeviceCB;
	WRAP_sDevInfo *					psDevInfo			= psDevIfDeviceCB->pvDevIfInfo;

	/* Free the device info structure...*/
	if (psDevInfo != IMG_NULL)
	{
		IMG_FREE(psDevInfo);
		psDevIfDeviceCB->pvDevIfInfo = IMG_NULL;
	}

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MEMMSP_MallocDeviceMem

******************************************************************************/
IMG_INT32 
MEMMSP_MallocDeviceMem(	
	IMG_VOID *						pWRAPContext,
	WRAP_psDeviceMemCB				psWRAPDeviceMemCB
)
{
    MEMMSP_sMemoryContextCB *       psMemoryContextCB   = pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB          = psMemoryContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB     = &psDeviceCB->sDevIfDeviceCB;
	WRAP_sDevInfo *					psDevInfo			= psDevIfDeviceCB->pvDevIfInfo;

	/* Ensure that we are initialised */

	IMG_ASSERT(gbWarpInitialised);

	/* Ensure size and alignment are correct....*/
	IMG_ASSERT(psWRAPDeviceMemCB->ui32Size		== WRAP_PAGE_SIZE);
	IMG_ASSERT(psWRAPDeviceMemCB->ui32Alignment == WRAP_PAGE_SIZE);
	IMG_ASSERT((((IMG_UINTPTR)psWRAPDeviceMemCB->pHostMem) & (WRAP_PAGE_SIZE-1)) == 0);

	/* Obtain the device physical page address...*/
	psWRAPDeviceMemCB->ui64DeviceMemBase = PALLOC_CpuUmAddrToDevPAddr(psDevInfo->ui32DeviceId, psWRAPDeviceMemCB->pHostMem);
	psWRAPDeviceMemCB->ui64DeviceMem = psWRAPDeviceMemCB->ui64DeviceMemBase;

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MEMMSP_FreeDeviceMem

******************************************************************************/
IMG_INT32 
MEMMSP_FreeDeviceMem(	
	IMG_VOID *						pWRAPContext,
	WRAP_psDeviceMemCB				psWRAPDeviceMemCB
)
{
	/* Ensure that we are initialised */
	IMG_ASSERT(gbWarpInitialised);

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MEMMSP_ReadWord

******************************************************************************/
IMG_INT32 
MEMMSP_ReadWord(
	IMG_VOID *						pWRAPContext,
	WRAP_psDeviceMemCB				psWRAPDeviceMemCB,
	IMG_UINT32						ui32Offset,
	IMG_PUINT32						pui32Value
)
{
    MEMMSP_sMemoryContextCB *       psMemoryContextCB   = pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB          = psMemoryContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB     = &psDeviceCB->sDevIfDeviceCB;

    IMG_ASSERT((ui32Offset & 0x3) == 0);

	/* Read word from host/device memory (unified memory)...*/
    *pui32Value = ((IMG_UINT32*)psWRAPDeviceMemCB->pHostMem)[ui32Offset>>2];

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MEMMSP_WriteWord

******************************************************************************/
IMG_INT32 
MEMMSP_WriteWord (	
	IMG_VOID *						pWRAPContext,
	WRAP_psDeviceMemCB				psWRAPDeviceMemCB,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32Value
)
{
    MEMMSP_sMemoryContextCB *       psMemoryContextCB   = pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB          = psMemoryContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB     = &psDeviceCB->sDevIfDeviceCB;

	/* Ensure that we are initialised */
	IMG_ASSERT(gbWarpInitialised);

	/* Ensure that the offset is word aligned...*/
    IMG_ASSERT((ui32Offset & 0x3) == 0);

	/* Write word into host/device memory (unified memory)...*/
    ((IMG_UINT32*)psWRAPDeviceMemCB->pHostMem)[ui32Offset>>2] = ui32Value;

	/* Check the memory has been set */
	if (gsWrapperControlInfo.ui32Flags & TCONF_WRAPFLAG_CHECK_MEM_COPY)
	{
		IMG_UINT32 ui32, ui32ReadWord;
		for (ui32 = 0; ui32 < MAX_CHECK_MEM_COUNT; ui32++)
		{
			MEMMSP_ReadWord(pWRAPContext, psWRAPDeviceMemCB, ui32Offset, &ui32ReadWord);
			if (ui32ReadWord == ui32Value)
				break;
		}
		IMG_ASSERT (ui32 < MAX_CHECK_MEM_COUNT);
	}

	/* Success */
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              MEMMSP_MemSpacePollWord

******************************************************************************/
static IMG_INT32 
MEMMSP_MemSpacePollWord (	
	IMG_VOID *						pWRAPContext,
	WRAP_psDeviceMemCB				psWRAPDeviceMemCB,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32FuncId,
	WRAP_pfnCheckFunc				pfnCheckFunc,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Mask,
	IMG_UINT32    					ui32PollCount,
	IMG_UINT32						ui32TimeOut
)
{
    MEMMSP_sMemoryContextCB *       psMemoryContextCB   = pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB          = psMemoryContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB     = &psDeviceCB->sDevIfDeviceCB;
	IMG_BOOL				        bResult             = IMG_FALSE;
	IMG_BOOL				        bInfinitePollCount  = IMG_FALSE;
	IMG_UINT32				        ui32MemValue;

	
	if (ui32PollCount == TAL_POLLCOUNT_INFINITE)
	{
		bInfinitePollCount = IMG_TRUE;
	}

	/* If we are not stubing out the target or this device...*/
	if ( (!WRAP_STUB_OUT_TARGET) && (!WRAP_CONTROL_STUB_OUT_DEVICE) )
    {

		while (!bResult && (ui32PollCount > 0))
		{
			/* Read value from memory */
			MEMMSP_ReadWord(pWRAPContext, psWRAPDeviceMemCB, ui32Offset, &ui32MemValue);

			/* Wait the specified number of cycles */
			MEMMSP_Idle(pWRAPContext, ui32TimeOut);

			/* Call poll function to evaluate the result */
			bResult = pfnCheckFunc(pWRAPContext, ui32MemValue, ui32RequValue, ui32Mask);

			if (!bInfinitePollCount)
			{
				ui32PollCount --;
			}
		}
    }
    else
    {
    	bResult = IMG_TRUE;
    }

	/* If IMG_TRUE...*/
	if (bResult)
	{
		/* Return Success...*/
		return IMG_SUCCESS;
	}

	/* Return time-out */
	return IMG_ERROR_TIMEOUT;
}


/*!
******************************************************************************

 @Function              MEMMSP_CopyHostToDeviceMemRegion

******************************************************************************/
IMG_INT32 
MEMMSP_CopyHostToDeviceMemRegion (	
	IMG_VOID *						pWRAPContext,
	WRAP_psDeviceMemCB				psWRAPDeviceMemCB,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32Size
)
{	
	IMG_UINT8 ui8LastByte = *(IMG_UINT8*)(psWRAPDeviceMemCB->pHostMem + ui32Offset + ui32Size - 1);

	/* Ensure that we are initialised */
	IMG_ASSERT(gbWarpInitialised);

	/* Unified memory - nothing to do...*/

	/* Check the memory write has occured */
	if (gsWrapperControlInfo.ui32Flags & TCONF_WRAPFLAG_CHECK_MEM_COPY)
	{
		IMG_UINT32 ui32;
		for (ui32 = 0; ui32 < MAX_CHECK_MEM_COUNT; ui32++)
		{
			MEMMSP_CopyDeviceToHostMemRegion(pWRAPContext, psWRAPDeviceMemCB, ui32Offset + ui32Size - 1, 1);
			if (ui8LastByte == *(IMG_UINT8*)(psWRAPDeviceMemCB->pHostMem + ui32Offset + ui32Size - 1))
				break;
		}
		if (ui32 >= MAX_CHECK_MEM_COUNT)
		{
			printf("ERROR: Memory write check failed\n");
			IMG_ASSERT(IMG_FALSE);
		}
	}

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MEMMSP_CopyDeviceToHostMemRegion

******************************************************************************/
IMG_INT32 
MEMMSP_CopyDeviceToHostMemRegion (	
	IMG_VOID *						pWRAPContext,
	WRAP_psDeviceMemCB				psWRAPDeviceMemCB,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32Size
)
{
	/* Ensure that we are initialised */
	IMG_ASSERT(gbWarpInitialised);

	/* Unified memory - nothing to do...*/

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              MEMMSP_CopyDeviceVirtToHostRegion

******************************************************************************/
static IMG_INT32 MEMMSP_CopyDeviceVirtToHostRegion (	
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32DevVirtAddr,
	IMG_UINT32						ui32DevVirtQualifier,
	IMG_UINT32						ui32Size,
	IMG_VOID *						pvHostBuf
)
{
	/* Ensure that we are initialised */
	IMG_ASSERT(gbWarpInitialised);
	
	IMG_ASSERT(IMG_FALSE);			// Not supported

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              WRAP_ReadMemory

******************************************************************************/
IMG_INT32 
WRAP_ReadMemory(
	IMG_UINT64				ui64MemoryAddress,
	IMG_UINT32				ui32Size,
	IMG_UINT8 *				pui8Buffer
)
{
	/* Keep compiler happy */
	ui64MemoryAddress = ui64MemoryAddress;

	/* Ensure that we are initialised */
	IMG_ASSERT(gbWarpInitialised);
	
	IMG_ASSERT(IMG_FALSE);			// Not supported

	/* Success */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              SLPMSP_GetMemSpaceFlags

******************************************************************************/
IMG_INT32 
SLPMSP_GetMemSpaceFlags(	
	IMG_VOID *						pWRAPContext
)
{
	SLPMSP_sSlavePortContextCB *		psSlavePortContextCB	= (SLPMSP_sSlavePortContextCB *) pWRAPContext;
    TCONF_sDeviceCB *					psDeviceCB				= psSlavePortContextCB->psDeviceCB;
    DEVIF_sDeviceCB *					psDevIfDeviceCB			= &psDeviceCB->sDevIfDeviceCB;
	IMG_UINT32							ui32MemSpaceFlags		= 0;

	
	/* If device stubbed out...*/
	if (WRAP_CONTROL_STUB_OUT_DEVICE)
	{
		ui32MemSpaceFlags |= TAL_MEMSPFLAGS_STUB_OUT_DEVICE;
	}

	/* Return flags...*/
	return ui32MemSpaceFlags;
}


/*!
******************************************************************************

 @Function              SLPMSP_Idle

******************************************************************************/
IMG_INT32 
SLPMSP_Idle (	
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32Value
)
{
	SLPMSP_sSlavePortContextCB *		psSlavePortContextCB	= (SLPMSP_sSlavePortContextCB *) pWRAPContext;
	TCONF_sDeviceCB *					psDeviceCB				= psSlavePortContextCB->psDeviceCB;
	
	return wrap_Idle(psDeviceCB, ui32Value);
}


/*!
******************************************************************************

 @Function              SLPMSP_Initialise

******************************************************************************/
IMG_INT32 
SLPMSP_Initialise(	
	IMG_VOID *						pWRAPContext
)
{
	SLPMSP_sSlavePortContextCB *		psSlavePortContextCB	= (SLPMSP_sSlavePortContextCB *) pWRAPContext;
    TCONF_sDeviceCB *					psDeviceCB				= psSlavePortContextCB->psDeviceCB;
    DEVIF_sDeviceCB *					psDevIfDeviceCB			= &psDeviceCB->sDevIfDeviceCB;
 	
    /* Set base address within the TAL..*/
    TAL_SetBaseAddress (pWRAPContext, psSlavePortContextCB->ui32BaseAddr);

	/* Register additional callback function with the TAL...*/
	TAL_RegisterWrapFunction(TAL_FN_TYPE_DEINIT,	pWRAPContext,(IMG_VOID *)&SLPMSP_Deinitialise);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_GET_FLAGS, pWRAPContext,(IMG_VOID *)&SLPMSP_GetMemSpaceFlags);
	TAL_RegisterWrapFunction(TAL_FN_TYPE_IDLE,		pWRAPContext,(IMG_VOID *)&SLPMSP_Idle);

	/* Return wrapper result */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              SLPMSP_Deinitialise

******************************************************************************/
IMG_INT32 
SLPMSP_Deinitialise(	
	IMG_VOID *						pWRAPContext
)
{
	SLPMSP_sSlavePortContextCB *	psSlavePortContextCB	= (SLPMSP_sSlavePortContextCB *) pWRAPContext;
    TCONF_sDeviceCB *				psDeviceCB				= psSlavePortContextCB->psDeviceCB;
    DEVIF_sDeviceCB *				psDevIfDeviceCB			= &psDeviceCB->sDevIfDeviceCB;
	WRAP_sDevInfo *					psDevInfo				= psDevIfDeviceCB->pvDevIfInfo;

	/* Free the device info structure...*/
	if (psDevInfo != IMG_NULL)
	{
		IMG_FREE(psDevInfo);
		psDevIfDeviceCB->pvDevIfInfo = IMG_NULL;
	}


	/* Return wrapper result */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              SLPMSP_WriteWord

******************************************************************************/
IMG_INT32 
SLPMSP_WriteWord (	
	IMG_VOID *						pWRAPContext,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32Value
)
{
	SLPMSP_sSlavePortContextCB *	psSlavePortContextCB	= (SLPMSP_sSlavePortContextCB *) pWRAPContext;
    TCONF_sDeviceCB *               psDeviceCB				= psSlavePortContextCB->psDeviceCB;
    DEVIF_sDeviceCB *               psDevIfDeviceCB			= &psDeviceCB->sDevIfDeviceCB;
	WRAP_sDevInfo *					psDevInfo				= psDevIfDeviceCB->pvDevIfInfo;

	IMG_ASSERT(ui32Offset					< psSlavePortContextCB->ui32WindowSize);
    IMG_ASSERT((ui32Offset & 0x3)			== 0);
	IMG_ASSERT(psDevInfo->pui32SlavePort	!= IMG_NULL);

	/* Write value to slaveport...*/
	psDevInfo->pui32SlavePort[(psSlavePortContextCB->ui32BaseAddr + ui32Offset)>>2] = ui32Value;

	/* Return wrapper result */
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              DEVIF_ConfigureDevice

******************************************************************************/
IMG_VOID DEVIF_ConfigureDevice(
    IMG_CHAR *				pszDevName,
    DEVIF_sDeviceCB *		psDeviceCB
)
{
	IMG_UINT32					ui32Result;
	WRAP_sDevInfo *				psDevInfo;

	/* Allocate wrapper device info structure...*/
	psDevInfo = IMG_MALLOC(sizeof(*psDevInfo));
	IMG_ASSERT(psDevInfo != IMG_NULL);
	IMG_MEMSET(psDevInfo, 0, sizeof(*psDevInfo));

	/* Set pointer to to device info structure in device control block...*/
	psDeviceCB->pvDevIfInfo = psDevInfo;

	/* Initialise the device manager...*/
	ui32Result = DMAN_Initialise();
	IMG_ASSERT(ui32Result == IMG_SUCCESS);

	/* Open the device...*/
	ui32Result = DMAN_OpenDevice(pszDevName, DMAN_OMODE_EXCLUSIVE, &psDevInfo->ui32ConnId);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);

	/* Open the device...*/
	ui32Result = DMAN_GetDeviceId(psDevInfo->ui32ConnId, &psDevInfo->ui32DeviceId);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);

	/* Attach to the wrapper utilities to this device connection...*/
	ui32Result = WRAPU_AttachToConnection(psDevInfo->ui32ConnId, &psDevInfo->ui32WrapUAttachId);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);

	/* Get access to register and slave ports...*/
	ui32Result = WRAPU_GetCpuUmAddr(psDevInfo->ui32WrapUAttachId, WRAPU_REGID_REGISTERS, (IMG_VOID **)&psDevInfo->pui32Registers);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);
//	psDevInfo->pui32SlavePort = WRAPU_GetCpuUmAddr(psDevInfo->ui32WrapUAttachId, WRAPU_REGID_SLAVE_PORT);
}

