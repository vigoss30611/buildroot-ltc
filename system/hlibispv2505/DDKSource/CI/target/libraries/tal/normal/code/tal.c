/*!
******************************************************************************
 @file   : tal.c

 @brief

 @Author Ray Livesley

 @date   02/06/2003

         <b>Copyright 2003 by Imagination Technologies Limited.</b>\n
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
         Target Abrstraction Layer (TAL) C Source File.

 <b>Platform:</b>\n
         Platform Independent

 @Version
         1.0

******************************************************************************/

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <tal.h>

#include <tal_intdefs.h>
#include <pdump_cmds.h>
#include <target.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <mmu.h>
#include <sgxmmu.h>
#include <pagemem.h>
#include <addr_alloc.h>
#if defined (__TAL_CALLBACK_SUPPORT__)
#include "cbman.h"
#endif

#ifdef __TAL_USE_OSA__
	#include <osa.h>
    IMG_HANDLE ghTalThreadsafeMutex;
    IMG_BOOL gbTalMutexInitialised = IMG_FALSE; 
#endif
#ifdef __TAL_USE_MEOS__
	#include <krn.h>
	#include <lst.h>
#endif

// Windows can't manage inlines
#ifdef WIN32
#define inline
#endif


#define TAL_MEM_TWIDDLE(ui64Address, eTwiddle) ((eTwiddle) != TAL_TWIDDLE_NONE ? talmem_Twiddle(eTwiddle, ui64Address) : ui64Address)


static IMG_BOOL				gbInitialised = IMG_FALSE;					/*!< Module initiliased								        */

static TAL_sPdumpContext   gsPdumpContext;                              /*!< Global PDUMP context - used when no contexts defined    */
static TAL_sPdumpContext   gasPdumpContexts[TAL_MAX_PDUMP_CONTEXTS];    /*!< Defined PDUMP contexts                                  */
static IMG_UINT32          gui32NoPdumpContexts = 0;                    /*!< No. of PDUMP contexts (0 if global context is being used */
static IMG_BOOL			   gbForceSingleContext = IMG_FALSE;


static const IMG_CHAR * gpsDefaultBlockPrefix = "BLOCK_"; /*!< Prefix for block in PDUMP2 scripts           */

static IMG_UINT32	gui32TargetAppFlags = 0;        /*!< Target flags                                       */

static 	TAL_pfnPollFailFunc gpfnPollFailFunc = IMG_NULL; /*!<Poll fail callback function                    */

static TAL_Strings *	gpsIfStrings = IMG_NULL;		/* Array of defined strings for ifs */

static TAL_DeviceList * gpsDeviceList = IMG_NULL;		/* List of recently used devices */
static TAL_DeviceList   gasDeviceList[MAX_DEV_LIST];	/* List of recently used devices */
static IMG_BOOL			gbDevListInit = IMG_FALSE;		/* Has list array been initialised? */

static TAL_sSemaphoreCB gasSemaphoreIds[TAL_MAX_SEMA_ID];	/* Array or semaphore details */

TAL_pfnTAL_MemSpaceRegister gpfnTAL_MemSpaceRegisterOverride = IMG_NULL;  /* Pointer to override MemSpaceRegister function */

IMG_UINT32 gui32TalRdwVerifyPollIter = TAL_RDW_VERIFY_ITERATIONS;	/*!< Read Verify Poll iterations */
IMG_UINT32 gui32TalRdwVerifyPollTime = TAL_RDW_VERIFY_DELAY;		/*!< Read Verify Poll Loop Time */

/*!
******************************************************************************
 Intenal function prototypes...
******************************************************************************/
static IMG_VOID tal_LoopNormalCommand(
    TAL_sMemSpaceCB *			psMemSpaceCB
);

static IMG_UINT64 talmem_GetDeviceMemRef(
	IMG_HANDLE                      hRefDeviceMem,
    IMG_UINT64                      ui64RefOffset
);

static IMG_VOID talpdump_InitContext(
    IMG_CHAR *                  pszContextName,
    TAL_sPdumpContext *         psPdumpContext
);

static IMG_VOID talpdump_DeinitContext(
    TAL_sPdumpContext *         psPdumpContext
);
static IMG_RESULT talvmem_GetPageAndOffset(
    IMG_HANDLE              hMemSpace,
    IMG_UINT64              ui64DevVirtAddr,
    IMG_UINT32              ui32MmuContextId,
    IMG_HANDLE *            phDeviceMem,
    IMG_UINT64 *            pui64Offset
);
static IMG_RESULT talreg_ReadWord (
    TAL_sMemSpaceCB *				psMemSpaceCB,
    IMG_UINT32                      ui32Offset,
    IMG_PVOID                       pvValue,
	IMG_UINT32						ui32Flags
);
static IMG_RESULT talmem_ReadWord(
    TAL_psDeviceMemCB               psDeviceMemCB,
    IMG_UINT64                      ui64Offset,
	IMG_PVOID						pvValue,
	IMG_UINT32						ui32Flags
);
static IMG_RESULT talmem_FlushLoadBuffer(
	TAL_sPdumpContext *     psPdumpContext
);

/*!
******************************************************************************
##############################################################################
 Category:              TAL Other Functions
##############################################################################
******************************************************************************/

/*!
******************************************************************************

 @Function              talpdump_Check

  @Description			This function checks to see whether we  want to pdump.
						This could be because we are in a loop and
						not on the first cycle or in a poll.

******************************************************************************/
static
IMG_BOOL talpdump_Check(
    TAL_sPdumpContext *	psPdumpContext
)
{
	return ( (!psPdumpContext->bInsideLoop) ||
			(psPdumpContext->bInsideLoop && psPdumpContext->psCurLoopContext->bFirstIteration
				&& psPdumpContext->psCurLoopContext->bFirstEncounter && psPdumpContext->psCurLoopContext->eLoopControl != TAL_LOOP_TEST_FALSE)
			);
}

/*!
******************************************************************************

 @Function              tal_Idle

******************************************************************************/
IMG_RESULT tal_Idle(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Time
)
{
	IMG_RESULT              rResult = IMG_SUCCESS;
		
	IMG_ASSERT(gbInitialised);

	if (hMemSpace == NULL)
	{
	    TAL_SLEEP(ui32Time); // instead of an active wait loop...
	}
	else if ( TAL_CheckMemSpaceEnable(hMemSpace) )
	{
	    TAL_sMemSpaceCB *	psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	    DEVIF_sDeviceCB *	psDevIfDeviceCB = &psMemSpaceCB->psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;

	    if ( psDevIfDeviceCB->pfnIdle )
	    {
			psDevIfDeviceCB->pfnIdle(psDevIfDeviceCB, ui32Time);
	    }
	    else
	    {
			TAL_SLEEP(ui32Time);
	    }
	}
	return rResult;
}


/*!
******************************************************************************

 @Function              TAL_CheckMemSpaceEnable

******************************************************************************/
IMG_BOOL TAL_CheckMemSpaceEnable(
    IMG_HANDLE hMemSpace
)
{
	TAL_sMemSpaceCB *	psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	if (hMemSpace == IMG_NULL) return IMG_FALSE;

	// Check to see if an IF has occured and has disabled commands
	if (psMemSpaceCB->psPdumpContext->ui32Disable > 0) return IMG_FALSE;

	// If all targets are stubbed out don't run anything
	if (gui32TargetAppFlags & TALAPP_STUB_OUT_TARGET) return IMG_FALSE;

	// If this device is stubbed out we can not run commands
	if (psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_STUB_OUT) return IMG_FALSE;

	if (psMemSpaceCB->psPdumpContext->psCurLoopContext != IMG_NULL)
	{
		if (psMemSpaceCB->psPdumpContext->psCurLoopContext->eLoopControl == TAL_LOOP_TEST_FALSE) return IMG_FALSE;
	}

	return IMG_TRUE;
}

/*!
******************************************************************************

 @Function              TAL_Wait

******************************************************************************/
IMG_RESULT TAL_Wait(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Time
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;

    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
    TAL_THREADSAFE_INIT;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

	if (hMemSpace)
		psPdumpContext = psMemSpaceCB->psPdumpContext;
	else
		psPdumpContext = &gsPdumpContext;
	IMG_ASSERT(psPdumpContext);
		
	tal_Idle(hMemSpace, ui32Time);
	
	
	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    TAL_THREADSAFE_LOCK;

    /* If we are not inside a loop or on the first iteration */
    if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_sTarget sTarget;

		sTarget.hMemSpace = (hMemSpace != IMG_NULL) ? psMemSpaceCB->hPdumpMemSpace : 0;
		sTarget.ui32BlockId = 0;
		sTarget.ui64Value = ui32Time;
		sTarget.ui64BlockAddr = 0;
		sTarget.eMemSpT = PDUMP_MEMSPACE_VALUE;

		PDUMP_CMD_Idle(sTarget);
    }
    
    TAL_THREADSAFE_UNLOCK;
    return rResult;
}

/*!
******************************************************************************

 @Function              tal_LoopNormalCommand

******************************************************************************/
static IMG_VOID
tal_LoopNormalCommand(
    TAL_sMemSpaceCB *			psMemSpaceCB
)
{
    TAL_sPdumpContext *         psPdumpContext;
	TAL_sLoopContext *          psLoopContext;
	IMG_ASSERT(psMemSpaceCB);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	IMG_ASSERT(psPdumpContext->bInsideLoop);
	IMG_ASSERT(psPdumpContext->psCurLoopContext);

    /* Get access to current loop context...*/
    psLoopContext = psPdumpContext->psCurLoopContext;

    /* If type unknown...*/
	if (psLoopContext->eLoopType == TAL_LOOPTYPE_UNKNOWN)
	{
		if ((psLoopContext->bFirstIteration == IMG_TRUE)				&&
			(psLoopContext->eLoopState == TAL_LOOPSTATE_START_DETECTED)
		)
		{
			psLoopContext->eLoopType = TAL_LOOPTYPE_DOW;
			psLoopContext->eLoopState = TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED;
		}
		else
		{
			/* We only expect to be in an unknown state on first iteration of a DOW loop,
			   on the very first command inside the loop body */
			IMG_ASSERT(IMG_FALSE);
		}
	}
	else
	{
		if (psLoopContext->eLoopState != TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED)
		{
			psLoopContext->eLoopState = TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED;
		}
	}
}


/*!
******************************************************************************

 @Function              TALLOOP_Open

******************************************************************************/
IMG_RESULT TALLOOP_Open(
    IMG_HANDLE						hMemSpace,
	IMG_HANDLE *					phLoop
)
{
	TAL_sLoopContext *      psNewLoopContext;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	if (hMemSpace)
    {
        psPdumpContext = psMemSpaceCB->psPdumpContext;
		IMG_ASSERT(psPdumpContext);
    }
    else
    {
        /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
    }

	TAL_ASSERT(psPdumpContext->ui32LoopNestingDepth < TAL_MAX_LOOP_NESTING_DEPTH, "Maximum Loop Nesting Depth Exceeded");

	/* allocate a new loop context */
	psNewLoopContext = &psPdumpContext->asLoopContexts[psPdumpContext->ui32LoopNestingDepth];

	psNewLoopContext->bFirstIteration = IMG_TRUE;
	psNewLoopContext->ui32IterationNumber = 0;
	psNewLoopContext->eLoopControl = TAL_LOOP_TEST_TRUE;
	psNewLoopContext->eLoopState = TAL_LOOPSTATE_OPEN;
	psNewLoopContext->eLoopType = TAL_LOOPTYPE_UNKNOWN;

	/* If we are inside another loop then remember the parent loop information */
	if (psPdumpContext->bInsideLoop)
	{
		IMG_ASSERT(psPdumpContext->psCurLoopContext);
		psNewLoopContext->psParentLoopContext = psPdumpContext->psCurLoopContext;
		/* We are encountering this loop for the first time only if the parent is on its first iteration and has been encountered once */
		psNewLoopContext->bFirstEncounter = (psPdumpContext->psCurLoopContext->bFirstEncounter && psPdumpContext->psCurLoopContext->bFirstIteration);
	}
	else
	{
		psNewLoopContext->psParentLoopContext = IMG_NULL;
		/* If this loop is not nested then this must be the first encounter with it */
		psNewLoopContext->bFirstEncounter = IMG_TRUE;
	}

	/* Set the global loop variables */
	psPdumpContext->bInsideLoop = IMG_TRUE;
	psPdumpContext->psCurLoopContext = psNewLoopContext;
	psPdumpContext->ui32LoopNestingDepth++;

	*phLoop = psNewLoopContext;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return IMG_SUCCESS;
}



/*!
******************************************************************************

 @Function              TALLOOP_Start

******************************************************************************/

IMG_RESULT TALLOOP_Start(
	IMG_HANDLE						hLoop
)
{
	TAL_sLoopContext *		psLoopContext;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(hLoop);

	psLoopContext = (TAL_sLoopContext *) (hLoop);

	if ((psLoopContext->eLoopState == TAL_LOOPSTATE_OPEN) || (psLoopContext->eLoopState == TAL_LOOPSTATE_END_DETECTED))
	{
		psLoopContext->eLoopState = TAL_LOOPSTATE_START_DETECTED;
	}
	else
	{
		IMG_ASSERT(IMG_FALSE);	/* We are not expecting a start now */
	}

	 /* If we are not inside a loop or on the first iteration */
    if (
			(psLoopContext->bFirstIteration) && (psLoopContext->bFirstEncounter)
        )
    {
		PDUMP_CMD_sTarget stSrc = {0, 0, 0, 0, PDUMP_MEMSPACE_VALUE};
		PDUMP_CMD_LoopCommand(stSrc, 0, 0, 0, 0, PDUMP_LOOP_COMMAND_SDO);
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return IMG_SUCCESS;
}



/*!
******************************************************************************

 @Function              TALLOOP_TestRegister

******************************************************************************/
IMG_RESULT TALLOOP_TestRegister(
	IMG_HANDLE						hLoop,
    IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Enable,
	IMG_UINT32						ui32LoopCount
)
{
	TAL_sLoopContext *		psLoopContext;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	IMG_BOOL				bPass = IMG_FALSE;
	IMG_RESULT				rResult = IMG_SUCCESS;
	IMG_UINT32				ui32ReadVal;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;

	TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(hMemSpace);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);
	IMG_ASSERT(hLoop);

	psLoopContext = (TAL_sLoopContext *) (hLoop);
	IMG_ASSERT(psLoopContext == psPdumpContext->psCurLoopContext);

	/* At this point we MUST be able to determine if this is a WDO or a SDO loop */

	if (psLoopContext->eLoopType == TAL_LOOPTYPE_UNKNOWN)
	{
		IMG_ASSERT(psLoopContext->bFirstIteration == IMG_TRUE);

		if (psLoopContext->eLoopState == TAL_LOOPSTATE_START_DETECTED)
		{
			/* Loop test immediately following loop start is interpreted as a WDO loop */
			psLoopContext->eLoopType = TAL_LOOPTYPE_WDO;
		}
		/* Change the state here rather than on detection of normal commands */
		else if (psLoopContext->eLoopState == TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED)
		{
			/* Loop test following normal commands will be interpreted as a SDO loop */
			psLoopContext->eLoopType = TAL_LOOPTYPE_DOW;
		}
		else
		{
			/* It is not allowed to be in any other state at this point */
			IMG_ASSERT(IMG_FALSE);
		}
	}
	/* Check loop states in the case when we have already determined the loop type */
	else
	{
		if (psLoopContext->eLoopType == TAL_LOOPTYPE_WDO)
		{
			IMG_ASSERT(psLoopContext->bFirstIteration == IMG_FALSE);
			IMG_ASSERT((psLoopContext->eLoopState == TAL_LOOPSTATE_START_DETECTED));
		}
		/* TAL_LOOPTYPE_DOW */
		else
		{
			IMG_ASSERT((psLoopContext->eLoopState == TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED));
		}
	}

	/* Change state to "test detected" */
	psLoopContext->eLoopState = TAL_LOOPSTATE_TEST_DETECTED;

	/* Ensure that capture is only performed on the very first iteration of the loop */

    /* If we are not inside a loop or on the first iteration */
    if (
			(psLoopContext->bFirstIteration) && (psLoopContext->bFirstEncounter)
        )
    {
        PDUMP_CMD_sTarget stSrc;

		stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
		stSrc.ui64Value = ui32Offset;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_REGISTER;

		if (psLoopContext->eLoopType == TAL_LOOPTYPE_WDO)
			PDUMP_CMD_LoopCommand(stSrc, ui32CheckFuncIdExt, ui32RequValue, ui32Enable, ui32LoopCount, PDUMP_LOOP_COMMAND_WDO);
		else if (psLoopContext->eLoopType == TAL_LOOPTYPE_DOW)
			PDUMP_CMD_LoopCommand(stSrc, ui32CheckFuncIdExt, ui32RequValue, ui32Enable, ui32LoopCount, PDUMP_LOOP_COMMAND_DOW);
    }

	/* Check this type of access is valid for this memory space...*/
	IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);

	rResult = talreg_ReadWord(psMemSpaceCB, ui32Offset, &ui32ReadVal, TAL_WORD_FLAGS_32BIT);
	if (rResult == IMG_SUCCESS)
		bPass = TAL_TestWord(ui32ReadVal, ui32RequValue, ui32Enable, 0, ui32CheckFuncIdExt);

	/* Test if the test was successful, or if this is not an infinite loopcount has the iteration limit been reached */
	if (	(bPass)		||
			((ui32LoopCount != TAL_LOOPCOUNT_INFINITE) && ((psLoopContext->ui32IterationNumber + 1) >= ui32LoopCount))
	)
	{
		/* Success - set the loop control to TRUE */
		psLoopContext->eLoopControl = TAL_LOOP_TEST_TRUE;
	}
	else
	{
		/* Unsuccessful test */
		psLoopContext->eLoopControl = TAL_LOOP_TEST_FALSE;
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}


/*!
******************************************************************************

 @Function              TALLOOP_TestMemory

******************************************************************************/
IMG_RESULT TALLOOP_TestMemory(
	IMG_HANDLE						hLoop,
	IMG_HANDLE						hDeviceMem,
	IMG_UINT32						ui32Offset,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT32						ui32RequValue,
	IMG_UINT32						ui32Enable,
	IMG_UINT32						ui32LoopCount
)
{
	TAL_sLoopContext *		psLoopContext;
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB;
	TAL_psDeviceMemCB		psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
	IMG_BOOL				bTestPass = IMG_FALSE;
	IMG_UINT32				ui32ReadVal;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;

	/* Check that the word offset does not exceed the block size */
	TAL_ASSERT(ui32Offset + 4 <= psDeviceMemCB->ui64Size, "Memory read outside allocated block");

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);
	IMG_ASSERT(hLoop);

	psLoopContext = (TAL_sLoopContext *) (hLoop);
	IMG_ASSERT(psLoopContext == psPdumpContext->psCurLoopContext);

	/* At this point we MUST be able to determine if this is a WDO or a SDO loop */

	if (psLoopContext->eLoopType == TAL_LOOPTYPE_UNKNOWN)
	{
		IMG_ASSERT(psLoopContext->bFirstIteration == IMG_TRUE);

		if (psLoopContext->eLoopState == TAL_LOOPSTATE_START_DETECTED)
		{
			/* Loop test immediately following loop start is interpreted as a WDO loop */
			psLoopContext->eLoopType = TAL_LOOPTYPE_WDO;
		}
		/* Change the state here rather than on detection of normal commands */
		else if (psLoopContext->eLoopState == TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED)
		{
			/* Loop test following normal commands will be interpreted as a SDO loop */
			psLoopContext->eLoopType = TAL_LOOPTYPE_DOW;
		}
		else
		{
			/* It is not allowed to be in any other state at this point */
			IMG_ASSERT(IMG_FALSE);
		}
	}
	/* Check loop states in the case when we have already determined the loop type */
	else
	{
		if (psLoopContext->eLoopType == TAL_LOOPTYPE_WDO)
		{
			IMG_ASSERT(psLoopContext->bFirstIteration == IMG_FALSE);
			IMG_ASSERT((psLoopContext->eLoopState == TAL_LOOPSTATE_START_DETECTED));
		}
		/* TAL_LOOPTYPE_DOW */
		else
		{
			IMG_ASSERT((psLoopContext->eLoopState == TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED));
		}
	}

	/* Change state to "test detected" */
	psLoopContext->eLoopState = TAL_LOOPSTATE_TEST_DETECTED;

    /* If we are not inside a loop or on the first iteration */
    if (
			(psLoopContext->bFirstIteration) && (psLoopContext->bFirstEncounter)
        )
    {
		/// Pdump Loop output

		PDUMP_CMD_sTarget stSrc;
		
		stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
		stSrc.ui64Value = ui32Offset;
		stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
		stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;
		
		if (psLoopContext->eLoopType == TAL_LOOPTYPE_WDO)
			PDUMP_CMD_LoopCommand(stSrc, ui32CheckFuncIdExt, ui32RequValue, ui32Enable, ui32LoopCount, PDUMP_LOOP_COMMAND_WDO);
		else if (psLoopContext->eLoopType == TAL_LOOPTYPE_DOW)
			PDUMP_CMD_LoopCommand(stSrc, ui32CheckFuncIdExt, ui32RequValue, ui32Enable, ui32LoopCount, PDUMP_LOOP_COMMAND_DOW);
    }

	if (TAL_CheckMemSpaceEnable(psMemSpaceCB))
	{
		rResult = talmem_ReadWord(psDeviceMemCB, ui32Offset, &ui32ReadVal, TAL_WORD_FLAGS_32BIT);
		if (rResult == IMG_SUCCESS)
			bTestPass = TAL_TestWord(ui32ReadVal, ui32RequValue, ui32Enable, 0, ui32CheckFuncIdExt);
		if( bTestPass )
		{
			/* Success - set the loop control to TRUE */
			psLoopContext->eLoopControl = TAL_LOOP_TEST_TRUE;
		}
		else
		{
			/* Unsuccessful test */
			psLoopContext->eLoopControl = TAL_LOOP_TEST_FALSE;
		}
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}


/*!
******************************************************************************

 @Function              TALLOOP_TestInternalReg

******************************************************************************/
IMG_RESULT TALLOOP_TestInternalReg(
	IMG_HANDLE						hLoop,
    IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32RegId,
	IMG_UINT32						ui32CheckFuncIdExt,
	IMG_UINT32    					ui32RequValue,
	IMG_UINT32    					ui32Enable,
	IMG_UINT32						ui32LoopCount
)
{
	TAL_sLoopContext *		psLoopContext;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	IMG_BOOL				bPass;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;

	TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	/* Check the validity of the internal register */
	TAL_ASSERT(ui32RegId < TAL_NUMBER_OF_INTERNAL_VARS, "Internal Variable Limit Exceeded");
	IMG_ASSERT(hLoop);

	psLoopContext = (TAL_sLoopContext *) (hLoop);
	IMG_ASSERT(psLoopContext == psPdumpContext->psCurLoopContext);

	/* At this point we MUST be able to determine if this is a WDO or a SDO loop */

	if (psLoopContext->eLoopType == TAL_LOOPTYPE_UNKNOWN)
	{
		IMG_ASSERT(psLoopContext->bFirstIteration == IMG_TRUE);

		if (psLoopContext->eLoopState == TAL_LOOPSTATE_START_DETECTED)
		{
			/* Loop test immediately following loop start is interpreted as a WDO loop */
			psLoopContext->eLoopType = TAL_LOOPTYPE_WDO;
		}
		/* Change the state here rather than on detection of normal commands */
		else if (psLoopContext->eLoopState == TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED)
		{
			/* Loop test following normal commands will be interpreted as a SDO loop */
			psLoopContext->eLoopType = TAL_LOOPTYPE_DOW;
		}
		else
		{
			/* It is not allowed to be in any other state at this point */
			IMG_ASSERT(IMG_FALSE);
		}
	}
	/* Check loop states in the case when we have already determined the loop type */
	else
	{
		if (psLoopContext->eLoopType == TAL_LOOPTYPE_WDO)
		{
			IMG_ASSERT(psLoopContext->bFirstIteration == IMG_FALSE);
			IMG_ASSERT((psLoopContext->eLoopState == TAL_LOOPSTATE_START_DETECTED));
		}
		/* TAL_LOOPTYPE_DOW */
		else
		{
			IMG_ASSERT((psLoopContext->eLoopState == TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED));
		}
	}

	/* Change state to "test detected" */
	psLoopContext->eLoopState = TAL_LOOPSTATE_TEST_DETECTED;

    /* If we are not inside a loop or on the first iteration */
    if (
			(psLoopContext->bFirstIteration) && (psLoopContext->bFirstEncounter)
        )
    {
		PDUMP_CMD_sTarget stSrc;

		stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
		stSrc.ui64Value = ui32RegId;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_INT_VAR;

		if (psLoopContext->eLoopType == TAL_LOOPTYPE_WDO)
			PDUMP_CMD_LoopCommand(stSrc, ui32CheckFuncIdExt, ui32RequValue, ui32Enable, ui32LoopCount, PDUMP_LOOP_COMMAND_WDO);
		else if (psLoopContext->eLoopType == TAL_LOOPTYPE_DOW)
			PDUMP_CMD_LoopCommand(stSrc, ui32CheckFuncIdExt, ui32RequValue, ui32Enable, ui32LoopCount, PDUMP_LOOP_COMMAND_DOW);
    }

	/* Check this type of access is valid for this memory space...*/
	switch (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType)
	{
	case TAL_MEMSPACE_REGISTER:
	case TAL_MEMSPACE_MEMORY:

		if (!(gui32TargetAppFlags & TALAPP_STUB_OUT_TARGET))
		{
			bPass = TAL_TestWord(psMemSpaceCB->aui64InternalVariables[ui32RegId], ui32RequValue, ui32Enable, 0, ui32CheckFuncIdExt);
			/* Test if the test is successful, or if this is not an infinite loopcount has the iteration limit been reached */
			if (	(bPass)		||
					((ui32LoopCount != TAL_LOOPCOUNT_INFINITE) && ((psLoopContext->ui32IterationNumber + 1) >= ui32LoopCount))
			)
			{
				/* Success - set the loop control to TRUE */
				psLoopContext->eLoopControl = TAL_LOOP_TEST_TRUE;
			}
			else
			{
				/* Unsuccessful test */
				psLoopContext->eLoopControl = TAL_LOOP_TEST_FALSE;
			}
		}
		else
		{
			/* The target is stubbed out... set result to true to exit loop */
			psLoopContext->eLoopControl = TAL_LOOP_TEST_TRUE;
		}
        break;

    default:
        /* Assert */
        IMG_ASSERT(IMG_FALSE);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              TALLOOP_End

******************************************************************************/
 IMG_RESULT TALLOOP_End(
	IMG_HANDLE						hLoop,
    TAL_eLoopControl *				peLoopControl
)
{
	TAL_sLoopContext		*psLoopContext;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	IMG_ASSERT(hLoop);
	psLoopContext = (TAL_sLoopContext *) (hLoop);


	if (psLoopContext->eLoopState == TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED)
	{
		IMG_ASSERT(psLoopContext->eLoopType == TAL_LOOPTYPE_WDO);
	}
	else if (psLoopContext->eLoopState == TAL_LOOPSTATE_TEST_DETECTED)
	{
		/* If the previous command was a test then it is possible it is a WDO loop with no normal commands inside it */
		IMG_ASSERT((psLoopContext->eLoopType == TAL_LOOPTYPE_DOW) || (psLoopContext->eLoopType == TAL_LOOPTYPE_WDO));
	}
	else
	{
		IMG_ASSERT(IMG_FALSE);
	}


	/* If this is a WDO loop, at this point it is a candidate for output of a EDO syntax element */
	if (
			(psLoopContext->bFirstIteration) && (psLoopContext->bFirstEncounter) &&
			(psLoopContext->eLoopType == TAL_LOOPTYPE_WDO)						/* Only capture at the end of WDO commands */
        )
    {
		PDUMP_CMD_sTarget stSrc = {0, 0, 0, 0, PDUMP_MEMSPACE_VALUE};
		PDUMP_CMD_LoopCommand(stSrc, 0, 0, 0, 0, PDUMP_LOOP_COMMAND_SDO);
    }

	/* Change the loop state */
	psLoopContext->eLoopState = TAL_LOOPSTATE_END_DETECTED;

	/* This is cannot be the first iteration any more */
	psLoopContext->bFirstIteration = IMG_FALSE;

	/* Increment the iteration counter */
	psLoopContext->ui32IterationNumber++;

	/* Set the loop status return variables */
	*peLoopControl = psLoopContext->eLoopControl;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return IMG_SUCCESS;
 }


/*!
******************************************************************************

 @Function              TALLOOP_Close

******************************************************************************/
IMG_RESULT TALLOOP_Close(
	IMG_HANDLE						hMemSpace,
	IMG_HANDLE *					phLoop
)
{
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	TAL_sLoopContext *      psLoopContext;
    TAL_sPdumpContext *     psPdumpContext;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);
	psPdumpContext = psMemSpaceCB->psPdumpContext;

	IMG_ASSERT(phLoop && *phLoop);

	psLoopContext = (TAL_sLoopContext *) (*phLoop);

	IMG_ASSERT(psLoopContext == psPdumpContext->psCurLoopContext);

	if (psLoopContext->psParentLoopContext)
	{
		psPdumpContext->psCurLoopContext = psLoopContext->psParentLoopContext;
		psPdumpContext->bInsideLoop = IMG_TRUE;
	}
	else
	{
		psPdumpContext->psCurLoopContext = IMG_NULL;
		psPdumpContext->bInsideLoop = IMG_FALSE;
	}

	psPdumpContext->ui32LoopNestingDepth--;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              TAL_GetLoopControl

******************************************************************************/
TAL_eLoopControl TALLOOP_GetLoopControl(
    IMG_HANDLE hMemSpace
)
{
	TAL_sMemSpaceCB *	psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	if (hMemSpace == IMG_NULL) IMG_ASSERT(IMG_FALSE);

	if (psMemSpaceCB->psPdumpContext->psCurLoopContext != IMG_NULL)
	{
		return psMemSpaceCB->psPdumpContext->psCurLoopContext->eLoopControl;
	}
	else
	{
		return TAL_LOOP_TEST_TRUE;
	}
}



/*!
******************************************************************************
##############################################################################
 Category:              TAL Setup Functions
##############################################################################
******************************************************************************/

/*!
******************************************************************************

 @Function              TALSETUP_Initialise

******************************************************************************/
IMG_RESULT TALSETUP_Initialise ()
{
    IMG_RESULT          rResult = IMG_SUCCESS;
    IMG_UINT32          ui32I;

    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_CREATE;
    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
	TAL_THREADSAFE_LOCK;

	if (!gbInitialised)
	{
		/* Initialise the global PDUMP context...*/
		talpdump_InitContext(IMG_NULL, &gsPdumpContext);

		/* Initialise the Pdump Commands Recorder */
		PDUMP_CMD_Initialise();

		/* Initialise the MMU manager */
		MMU_Initialise();

		/* Initialise the Semaphore Id Info */
		for (ui32I=0; ui32I < TAL_MAX_SEMA_ID; ui32I++)
		{
			gasSemaphoreIds[ui32I].hSrcMemSpace = IMG_NULL;
			gasSemaphoreIds[ui32I].hDestMemSpace = IMG_NULL;
		}

		gbInitialised = IMG_TRUE;

#ifdef TAL_DEINIT_ON_EXIT
		/* Call TALSETUP_Deinitialise on exit */
		atexit((IMG_VOID(*)(IMG_VOID))TALSETUP_Deinitialise);
#endif
		rResult = TARGET_Initialise ((TAL_pfnTAL_MemSpaceRegister) &TALSETUP_MemSpaceRegister);

		// Initialise all of the device interfaces

	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function              talsetup_CleanupAddrContext

******************************************************************************/
IMG_RESULT talsetup_CleanupAddrContext (ADDR_sContext * psAddrContext)
{
	// deinitialise the address allocator
	ADDR_CxDeinitialise(psAddrContext);

	// Free any default region control block
	if (psAddrContext->psDefaultRegion != IMG_NULL)
	{
		IMG_FREE(psAddrContext->psDefaultRegion);
	}

	// Free any named region control blocks
	while (psAddrContext->psRegions != IMG_NULL)
	{
		ADDR_sRegion *                  psRegion;
		psRegion = psAddrContext->psRegions->psNextRegion;

		IMG_FREE(psAddrContext->psRegions);
		psAddrContext->psRegions = psRegion;
	}

	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              TALSETUP_Deinitialise

******************************************************************************/
IMG_RESULT TALSETUP_Deinitialise ()
{
    IMG_RESULT                  rResult = IMG_SUCCESS;
    IMG_UINT32                  i;
	TAL_sMemSpaceCB *			psMemSpaceCB;
	TAL_psMemSpaceInfo			psMemSpaceInfo;
	IMG_HANDLE					hMemSpItr;

    TAL_THREADSAFE_INIT;

	TALSETUP_Reset();

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
	TAL_THREADSAFE_LOCK;

    /* If we are not initilised...*/
    if (gbInitialised)
	{
		/* Call any memspace de-initialised functions...*/
    	psMemSpaceCB = (TAL_psMemSpaceCB)TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hMemSpItr);
		while (psMemSpaceCB)
		{
			psMemSpaceInfo = psMemSpaceCB->psMemSpaceInfo;

			// de-initialise device interface
			if (TAL_CheckMemSpaceEnable(psMemSpaceCB) && psMemSpaceInfo->psDevInfo->sDevIfDeviceCB.pfnDeinitailise != IMG_NULL)
			{
				psMemSpaceInfo->psDevInfo->sDevIfDeviceCB.pfnDeinitailise(&psMemSpaceInfo->psDevInfo->sDevIfDeviceCB);
			}
			
			if (psMemSpaceCB->pui32RegisterSpace)
			{
				IMG_FREE((IMG_PVOID)psMemSpaceCB->pui32RegisterSpace);
				psMemSpaceCB->pui32RegisterSpace = IMG_NULL;				
			}

			// de-initialise memory allocator
			if (psMemSpaceCB->hMemAllocator)
			{
				switch(TAL_MEMORY_ALLOCATION_CONTROL)
				{
				case TAL_DEVFLAG_4K_PAGED_ALLOC:
				case TAL_DEVFLAG_4K_PAGED_RANDOM_ALLOC:
					if(psMemSpaceInfo->sMemInfo.ui64MemSize > 0)
					{
						/* Free/deinitalise the page memory system...*/
						PAGEMEM_CxDeinitialise((PAGEMEM_sContext *)psMemSpaceCB->hMemAllocator);
						// Free the allocation context
						IMG_FREE(psMemSpaceCB->hMemAllocator);
						psMemSpaceCB->hMemAllocator = IMG_NULL;
					}
					if (psMemSpaceInfo->psDevInfo->hMemAllocator)
					{
						/* Free/deinitalise the page memory system...*/
						PAGEMEM_CxDeinitialise((PAGEMEM_sContext *)psMemSpaceInfo->psDevInfo->hMemAllocator);
						IMG_FREE(psMemSpaceInfo->psDevInfo->hMemAllocator);
						psMemSpaceInfo->psDevInfo->hMemAllocator = IMG_NULL;
					}
					break;
				default:
					if (psMemSpaceInfo->sMemInfo.ui64MemSize > 0)
					{
						if (psMemSpaceCB->hMemAllocator != psMemSpaceInfo->psDevInfo->hMemAllocator)
						{
							talsetup_CleanupAddrContext((ADDR_sContext *)psMemSpaceCB->hMemAllocator);
							// Free the allocation context
							IMG_FREE(psMemSpaceCB->hMemAllocator);
						}
						psMemSpaceCB->hMemAllocator = IMG_NULL;
					}
					break;
				}
			}

            if (psMemSpaceCB->pui32RegisterSpace)
            {
                IMG_FREE((IMG_VOID *)psMemSpaceCB->pui32RegisterSpace);
                psMemSpaceCB->pui32RegisterSpace = IMG_NULL;
            }

#if defined (__TAL_CALLBACK_SUPPORT__)
			if (psMemSpaceCB->bCbSlotAllocated)
			{
				CBMAN_UnregisterMyCBSystem(psMemSpaceCB->ui32CbManSlot);
			}
#endif
			psMemSpaceCB = TALITERATOR_GetNext(hMemSpItr);
		}
		TALITERATOR_Destroy(hMemSpItr);

		// Loop through the memspaces again and deinitialise the device (default) memory allocators
        // and lock (if it is used)
    	psMemSpaceCB = TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hMemSpItr);
		while (psMemSpaceCB)
		{
			psMemSpaceInfo = psMemSpaceCB->psMemSpaceInfo;
			switch(TAL_MEMORY_ALLOCATION_CONTROL)
			{
				case TAL_DEVFLAG_4K_PAGED_ALLOC:
				case TAL_DEVFLAG_4K_PAGED_RANDOM_ALLOC:
					break;
				default:
					if (psMemSpaceInfo->psDevInfo->hMemAllocator)
					{
						talsetup_CleanupAddrContext((ADDR_sContext *)psMemSpaceInfo->psDevInfo->hMemAllocator);
						IMG_FREE(psMemSpaceInfo->psDevInfo->hMemAllocator);
						psMemSpaceInfo->psDevInfo->hMemAllocator = IMG_NULL;
					}
			}
            //deinitialize lock
#if defined(__TAL_USE_OSA__)
            OSA_CritSectDestroy(psMemSpaceCB->hCbMutex);
            OSA_CritSectDestroy(psMemSpaceCB->hMonitoredRegionsMutex);
            OSA_CritSectDestroy(psMemSpaceCB->hDevMemListMutex);
#endif
			psMemSpaceCB = TALITERATOR_GetNext(hMemSpItr);
		}
		TALITERATOR_Destroy(hMemSpItr);
		TAL_DestroyMemSpaceCBs();

		/* Free global context */
		talpdump_DeinitContext(&gsPdumpContext);

		/* Free any additional contexts */
		for (i=0; i<gui32NoPdumpContexts; i++)
		{
			talpdump_DeinitContext(&gasPdumpContexts[i]);
		}

		gui32NoPdumpContexts = 0;

		gbInitialised = IMG_FALSE;

		PDUMP_CMD_Deinitialise();
		TARGET_Deinitialise();
	}
    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;
    TAL_THREADSAFE_DESTROY;

	return rResult;
}

/*!
******************************************************************************

 @Function              TALSETUP_IsInitialised

******************************************************************************/
IMG_BOOL TALSETUP_IsInitialised ()
{
	return (gbInitialised);
}

 /*!
******************************************************************************

 @Function              TALSETUP_OverrideMemSpaceSetup

******************************************************************************/
IMG_VOID TALSETUP_OverrideMemSpaceSetup (
	TAL_pfnTAL_MemSpaceRegister pfnTAL_MemSpaceRegisterOverride
)
{
	gpfnTAL_MemSpaceRegisterOverride = pfnTAL_MemSpaceRegisterOverride;
}


 /*!
******************************************************************************

 @Function              TALSETUP_MemSpaceRegister

******************************************************************************/
IMG_RESULT TALSETUP_MemSpaceRegister (
    TAL_psMemSpaceInfo          psMemSpaceInfo
)
{
	TAL_sMemSpaceCB	*			psMemSpaceCB;
	TAL_sMemSpaceCB	*			psMSpaceCBCheck;
	IMG_HANDLE					hItr;
	IMG_UINT32					ui32I;
	IMG_RESULT					rResult = IMG_SUCCESS;

	if (gpfnTAL_MemSpaceRegisterOverride != IMG_NULL)
		gpfnTAL_MemSpaceRegisterOverride(psMemSpaceInfo);

	// Check that this memory space has not already been registered
	psMSpaceCBCheck = (TAL_sMemSpaceCB *)TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hItr);
	while (psMSpaceCBCheck)
    {
        if(strcmp(psMemSpaceInfo->pszMemSpaceName, psMSpaceCBCheck->psMemSpaceInfo->pszMemSpaceName) == 0)
        {
        	TALITERATOR_Destroy(hItr);
			return IMG_ERROR_ALREADY_COMPLETE;
        }
		psMSpaceCBCheck = (TAL_sMemSpaceCB*)TALITERATOR_GetNext(hItr);
    }
    TALITERATOR_Destroy(hItr);
	psMemSpaceCB = TAL_GetFreeMemSpaceCB();

	// Copy information relating to this memory space
	psMemSpaceCB->psMemSpaceInfo = psMemSpaceInfo;

#if defined(__TAL_USE_OSA__)
    OSA_CritSectCreate(&psMemSpaceCB->hCbMutex);
    OSA_CritSectCreate(&psMemSpaceCB->hMonitoredRegionsMutex);
    OSA_CritSectCreate(&psMemSpaceCB->hDevMemListMutex);
#elif defined(__TAL_USE_MEOS__)
    KRN_initLock(&psMemSpaceCB->sCbLock);
#endif

	/* Set up Pdump */
	psMemSpaceCB->hPdumpMemSpace = PDUMP_CMD_MemSpaceRegister(psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName);
	
	TALPDUMP_AddMemspaceToContext(psMemSpaceInfo->pszPdumpContext, (IMG_HANDLE)psMemSpaceCB);

	switch(psMemSpaceInfo->eMemSpaceType)
	{
	case TAL_MEMSPACE_REGISTER:
		// Setup the register interrupts
		for( ui32I=0; ui32I< TAL_NUM_INT_MASKS; ui32I++ )
		{
			psMemSpaceCB->aui32intmask[ui32I] = 0;
		}

		ui32I = (psMemSpaceInfo->sRegInfo.ui32intnum & (IMG_UINT32)(~(0x20 - 1)) >> 5);
		if( ui32I < TAL_NUM_INT_MASKS )
		{
			psMemSpaceCB->aui32intmask[ui32I] = (1<< ((psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui32intnum)&(0x20-1)) );
		}

		// Set base address for pdump1
		PDUMP_CMD_SetBaseAddress(psMemSpaceCB->hPdumpMemSpace, psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr + psMemSpaceInfo->sRegInfo.ui64RegBaseAddr);

		// In the case of a stubbed out device setup dummy registers
		if ((gui32TargetAppFlags & TALAPP_STUB_OUT_TARGET) ||
			(psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_STUB_OUT))
		{
			psMemSpaceCB->pui32RegisterSpace = (IMG_PUINT32)IMG_MALLOC(psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui32RegSize);
		}

		break;
	case TAL_MEMSPACE_MEMORY:
		// Set base address for pdump1
		if (psMemSpaceInfo->sMemInfo.ui64MemSize == 0)
			PDUMP_CMD_SetBaseAddress (psMemSpaceCB->hPdumpMemSpace, psMemSpaceInfo->psDevInfo->ui64MemBaseAddr);
		else
			PDUMP_CMD_SetBaseAddress (psMemSpaceCB->hPdumpMemSpace, psMemSpaceInfo->sMemInfo.ui64MemBaseAddr);

		// Setup the device memory
		if (psMemSpaceCB->hMemAllocator == IMG_NULL)
		{
			switch (TAL_MEMORY_ALLOCATION_CONTROL)
			{
				case TAL_DEVFLAG_4K_PAGED_ALLOC:
				case TAL_DEVFLAG_4K_PAGED_RANDOM_ALLOC:
				{
					PAGEMEM_sContext * psPageMemContext;
					/* Guard bands and memory twiddle not supported for paged memory allocation */
					TAL_ASSERT(psMemSpaceInfo->sMemInfo.ui64MemGuardBand == 0, "Guard bands not supported in SGX 4K fixed allocation");
					TAL_ASSERT ((psMemSpaceInfo->psDevInfo->bConfigured == TAL_TWIDDLE_NONE), "Memory Twiddle not supported in SGX 4K fixed allocation");

					// Setup device allocation context if required
					if (psMemSpaceInfo->psDevInfo->ui64DefMemSize > 0 && psMemSpaceInfo->psDevInfo->hMemAllocator == IMG_NULL)
					{
						psPageMemContext = (PAGEMEM_sContext *)IMG_MALLOC(sizeof(PAGEMEM_sContext));
						psMemSpaceInfo->psDevInfo->hMemAllocator = psPageMemContext;
						// Initialise the device memory
						PAGEMEM_CxInitialise(psPageMemContext, IMG_UINT64_TO_UINT32(psMemSpaceInfo->psDevInfo->ui64MemBaseAddr),
											IMG_UINT64_TO_UINT32(psMemSpaceInfo->psDevInfo->ui64DefMemSize),(0x1000));
					}

					// Setup memspace allocation context if required
					if (psMemSpaceInfo->sMemInfo.ui64MemSize > 0)
					{
						psPageMemContext = (PAGEMEM_sContext *)IMG_MALLOC(sizeof(PAGEMEM_sContext));
						psMemSpaceCB->hMemAllocator = psPageMemContext;
						/* Initialise the paged memory allocation system...*/
						PAGEMEM_CxInitialise(psPageMemContext, IMG_UINT64_TO_UINT32(psMemSpaceInfo->sMemInfo.ui64MemBaseAddr),
											IMG_UINT64_TO_UINT32(psMemSpaceInfo->sMemInfo.ui64MemSize), (0x1000));
					}
					else
					{
						psMemSpaceCB->hMemAllocator = psMemSpaceInfo->psDevInfo->hMemAllocator;
					}

					break;
				}
				default:
				{
					ADDR_sRegion *                  psRegion;
					ADDR_sContext *                 psAddrContext;
					/* Allocate a context structure for device memory */
					if (psMemSpaceInfo->psDevInfo->ui64DefMemSize > 0 && psMemSpaceInfo->psDevInfo->hMemAllocator == IMG_NULL)
					{
						psAddrContext = (ADDR_sContext *)IMG_MALLOC(sizeof(ADDR_sContext));
						psMemSpaceInfo->psDevInfo->hMemAllocator = psAddrContext;
						ADDR_CxInitialise(psAddrContext);
						psAddrContext->bUseRandomBlocks = (TAL_MEMORY_ALLOCATION_CONTROL == TAL_DEVFLAG_RANDOM_BLOCK_ALLOC);
						psAddrContext->bUseRandomAllocation = ( TAL_MEMORY_ALLOCATION_CONTROL == TAL_DEVFLAG_TOTAL_RANDOM_ALLOC);
						/* Create and setup memory region...*/
						psRegion                = (ADDR_sRegion *)IMG_MALLOC(sizeof(ADDR_sRegion));
						psRegion->pszName       = IMG_NULL;
						psRegion->ui64BaseAddr  = psMemSpaceInfo->psDevInfo->ui64MemBaseAddr;
						psRegion->ui64Size      = psMemSpaceInfo->psDevInfo->ui64DefMemSize;
						psRegion->ui32GuardBand = IMG_UINT64_TO_UINT32(psMemSpaceInfo->psDevInfo->ui64MemGuardBand);
						ADDR_CxDefineMemoryRegion(psAddrContext, psRegion);
					}

					if (psMemSpaceInfo->sMemInfo.ui64MemSize == 0)
					{
						psMemSpaceCB->hMemAllocator = psMemSpaceInfo->psDevInfo->hMemAllocator;
//						psMemSpaceInfo->sMemInfo.ui64MemSize = psMemSpaceInfo->psDevInfo->ui64DefMemSize;
//						psMemSpaceInfo->sMemInfo.ui64MemBaseAddr = psMemSpaceInfo->psDevInfo->ui64MemBaseAddr;
//						psMemSpaceInfo->sMemInfo.ui64MemGuardBand = psMemSpaceInfo->psDevInfo->ui64MemGuardBand;
					}
					else
					{
						if (psMemSpaceInfo->psDevInfo->hMemAllocator == IMG_NULL)
						{
							psAddrContext = (ADDR_sContext *)IMG_MALLOC(sizeof(ADDR_sContext));
							psMemSpaceCB->hMemAllocator = psAddrContext;
							ADDR_CxInitialise(psAddrContext);
							psAddrContext->bUseRandomBlocks = (TAL_MEMORY_ALLOCATION_CONTROL == TAL_DEVFLAG_RANDOM_BLOCK_ALLOC);
							psAddrContext->bUseRandomAllocation = ( TAL_MEMORY_ALLOCATION_CONTROL == TAL_DEVFLAG_TOTAL_RANDOM_ALLOC);
						}
						else
						{
							psAddrContext = (ADDR_sContext*)psMemSpaceInfo->psDevInfo->hMemAllocator;
							psMemSpaceCB->hMemAllocator = psMemSpaceInfo->psDevInfo->hMemAllocator;
						}
						/* Create and setup memory region...*/
						psRegion                = (ADDR_sRegion *)IMG_MALLOC(sizeof(ADDR_sRegion));
						psRegion->pszName       = IMG_NULL;
						psRegion->ui64BaseAddr  = psMemSpaceInfo->sMemInfo.ui64MemBaseAddr;
						psRegion->ui64Size      = psMemSpaceInfo->sMemInfo.ui64MemSize;
						psRegion->ui32GuardBand = IMG_UINT64_TO_UINT32(psMemSpaceInfo->sMemInfo.ui64MemGuardBand);
						psRegion->pszName		= psMemSpaceInfo->pszMemSpaceName;
						ADDR_CxDefineMemoryRegion(psAddrContext, psRegion);
					}
				}
			}
		}
		break;
	default:
		TAL_ASSERT(IMG_FALSE, "unknown memspace type");
	}

	// Initialise the device assuming it is not stubbed out
	if (!(gui32TargetAppFlags & TALAPP_STUB_OUT_TARGET) && !(psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_STUB_OUT))
	{
		if (psMemSpaceInfo->psDevInfo->sDevIfDeviceCB.bInitialised == IMG_FALSE)
		{
			TAL_psSubDeviceCB psSubDevCB = psMemSpaceInfo->psDevInfo->psSubDeviceCB;
			if (psMemSpaceInfo->psDevInfo->bConfigured == IMG_FALSE)
	    	{
				DEVIF_ConfigureDevice(&psMemSpaceInfo->psDevInfo->sDevIfDeviceCB);

				while  (psSubDevCB)
				{
					DEVIF_ConfigureDevice(&psSubDevCB->sDevIfDeviceCB);
					psSubDevCB->sDevIfDeviceCB.pfnInitailise(&psSubDevCB->sDevIfDeviceCB);
					psSubDevCB = psSubDevCB->pNext;
				}
				psMemSpaceInfo->psDevInfo->sDevIfDeviceCB.pfnInitailise(&psMemSpaceInfo->psDevInfo->sDevIfDeviceCB);
				psMemSpaceInfo->psDevInfo->bConfigured = IMG_TRUE;
	    	}
		}
	}
#if defined (__TAL_CALLBACK_SUPPORT__)
	LST_init(&psMemSpaceCB->sMonitoredRegions);
#endif

	return rResult;
}

/*!
******************************************************************************

 @Function				TALSETUP_RegisterPollFailFunc

******************************************************************************/

IMG_VOID TALSETUP_RegisterPollFailFunc (
	TAL_pfnPollFailFunc         pfnPollFailFunc
)
{
    gpfnPollFailFunc = pfnPollFailFunc;
}

/*!
******************************************************************************

 @Function              TALSETUP_Reset

******************************************************************************/
IMG_RESULT TALSETUP_Reset ()
{
    TAL_psDeviceMemCB      psDeviceMemCB;
	TAL_psMemSpaceCB	   psMemSpaceCB;
	IMG_RESULT				rResult = IMG_SUCCESS;
	IMG_HANDLE				hItr;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	if (gbInitialised)
    {
        /* Loop over memory spaces...*/
		psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hItr);
        while (psMemSpaceCB)
        {
            if (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY)
            {
                /* While there are device memory blocks....*/
                while (psMemSpaceCB->psDevMemList != IMG_NULL)
                {
                    /* Free the device memory block...*/
                    psDeviceMemCB = psMemSpaceCB->psDevMemList;
                    rResult = TALMEM_Free((IMG_HANDLE *)&psDeviceMemCB);
					if (rResult != IMG_SUCCESS) break;
                }
                psMemSpaceCB->ui32NextBlockId = 0;
            }

#if defined (__TAL_CALLBACK_SUPPORT__)
            /*If a callback slot has been allocated then remove all the callbacks */
            if (psMemSpaceCB->bCbSlotAllocated)
            {
                CBMAN_RemoveAllCallbacksOnSlot(psMemSpaceCB->ui32CbManSlot);
            }

            /* Remove any monitored regions */
            LST_init(&psMemSpaceCB->sMonitoredRegions);
#endif
			psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetNext(hItr);
        }
        TALITERATOR_Destroy(hItr);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

 @Function              TALSETUP_SetTargetApplicationFlags

******************************************************************************/
IMG_VOID TALSETUP_SetTargetApplicationFlags(
	IMG_UINT32					ui32TargetAppFlags
)
{
	gui32TargetAppFlags	= ui32TargetAppFlags;
}

/*!
******************************************************************************
##############################################################################
 Category:              TAL Register Functions
##############################################################################
******************************************************************************/

// Some functions need to be diverted to a common function, inlines are used here to do that.

IMG_RESULT talreg_WriteDeviceMemRef(
    IMG_HANDLE              hMemSpace,
    IMG_UINT32              ui32Offset,
    IMG_HANDLE              hRefDeviceMem,
    IMG_UINT64              ui64RefOffset,
    IMG_BOOL                bCallback,
    IMG_UINT32				ui32Flags
);


IMG_RESULT TALREG_Poll(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut,
    IMG_UINT32						ui32Flags
);

inline IMG_RESULT TALREG_Poll32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32 ui32CheckFuncIdExt, IMG_UINT32 ui32RequValue, IMG_UINT32 ui32Enable,
						 IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALREG_Poll(hMemSpace,ui32Offset,ui32CheckFuncIdExt,ui32RequValue,ui32Enable,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALREG_Poll64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32 ui32CheckFuncIdExt, IMG_UINT64 ui64RequValue, IMG_UINT64 ui64Enable,
						 IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALREG_Poll(hMemSpace,ui32Offset,ui32CheckFuncIdExt,ui64RequValue,ui64Enable,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALREG_ReadWord32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32* pui32Value)
{
	return TALREG_ReadWord(hMemSpace,ui32Offset,pui32Value,IMG_FALSE,IMG_FALSE,0,0,IMG_FALSE,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALREG_ReadWord64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT64* pui64Value)
{
	return TALREG_ReadWord(hMemSpace,ui32Offset,pui64Value,IMG_FALSE,IMG_FALSE,0,0,IMG_FALSE,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALREG_ReadWordToPoll32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32* pui32Value, IMG_UINT32 ui32Count, IMG_UINT32 ui32Time)
{
	return TALREG_ReadWord(hMemSpace,ui32Offset,pui32Value,IMG_TRUE,IMG_FALSE,ui32Count,ui32Time,IMG_FALSE,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALREG_ReadWordToPoll64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT64* pui64Value, IMG_UINT32 ui32Count, IMG_UINT32 ui32Time)
{
	return TALREG_ReadWord(hMemSpace,ui32Offset,pui64Value,IMG_TRUE,IMG_FALSE,ui32Count,ui32Time,IMG_FALSE,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALREG_ReadWordToSAB32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32* pui32Value, IMG_UINT32 ui32Count, IMG_UINT32 ui32Time)
{
	return TALREG_ReadWord(hMemSpace,ui32Offset,pui32Value,IMG_FALSE,IMG_TRUE,ui32Count,ui32Time,IMG_FALSE,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALREG_ReadWordToSAB64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT64* pui64Value, IMG_UINT32 ui32Count, IMG_UINT32 ui32Time)
{
	return TALREG_ReadWord(hMemSpace,ui32Offset,pui64Value,IMG_FALSE,IMG_TRUE,ui32Count,ui32Time,IMG_FALSE,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALREG_WriteWord32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset,IMG_UINT32 ui32Value)
{
	return TALREG_WriteWord(hMemSpace,ui32Offset,ui32Value,IMG_NULL,IMG_FALSE,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALREG_WriteWord64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset,IMG_UINT64 ui64Value)
{
	return TALREG_WriteWord(hMemSpace,ui32Offset,ui64Value,IMG_NULL,IMG_FALSE,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALREG_WriteWordDefineContext32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32 ui32Value, IMG_HANDLE hContextMemSpace)
{
	return TALREG_WriteWord(hMemSpace,ui32Offset,ui32Value,hContextMemSpace,IMG_FALSE,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALREG_WriteWordDefineContext64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT64 ui64Value, IMG_HANDLE hContextMemSpace)
{
	return TALREG_WriteWord(hMemSpace,ui32Offset,ui64Value,hContextMemSpace,IMG_FALSE,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALREG_WriteDevMemRef32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_HANDLE hRefDeviceMem, IMG_UINT64 ui64RefOffset)
{
	return talreg_WriteDeviceMemRef(hMemSpace,ui32Offset,hRefDeviceMem,ui64RefOffset,IMG_FALSE,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALREG_WriteDevMemRef64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_HANDLE hRefDeviceMem, IMG_UINT64 ui64RefOffset)
{
	return talreg_WriteDeviceMemRef(hMemSpace,ui32Offset,hRefDeviceMem,ui64RefOffset,IMG_FALSE,TAL_WORD_FLAGS_64BIT);
}

/*!
******************************************************************************

 @Function              talreg_GetDevice

******************************************************************************/
static DEVIF_psDeviceCB talreg_GetDevice (
    TAL_sMemSpaceCB *				psMemSpaceCB,
    IMG_UINT32 *                    pui32Offset
)
{
	TAL_psSubDeviceCB	psSubDeviceCB;
	TAL_psDeviceInfo	psDevInfo = psMemSpaceCB->psMemSpaceInfo->psDevInfo;
	IMG_ASSERT(psMemSpaceCB);

	/* Check to see if we should be using a sub device */
	psSubDeviceCB = psDevInfo->psSubDeviceCB;
	while (psSubDeviceCB != IMG_NULL)
	{
		if (psSubDeviceCB->ui64RegStartAddress <= *pui32Offset
			&& (psSubDeviceCB->ui64RegStartAddress + psSubDeviceCB->ui32RegSize > *pui32Offset))
		{
			*pui32Offset -= IMG_UINT64_TO_UINT32(psSubDeviceCB->ui64RegStartAddress);
			return &psSubDeviceCB->sDevIfDeviceCB;
		}
		psSubDeviceCB = psSubDeviceCB->pNext;
	}
	/* If we haven't found a subdevice, return the main device */
	return &psDevInfo->sDevIfDeviceCB;
}

/*!
******************************************************************************

 @Function              talreg_CheckOffset

******************************************************************************/
static IMG_RESULT talreg_CheckOffset (
    TAL_sMemSpaceCB *				psMemSpaceCB,
    IMG_UINT32						ui32Offset
)
{
	IMG_ASSERT(psMemSpaceCB);
	TAL_ASSERT(ui32Offset < psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui32RegSize,
		"ERROR: Register offset outside register size %s:0x%08x\n", psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName, ui32Offset);
    TAL_ASSERT((ui32Offset & 0x3) == 0, "ERROR: Register action not word aligned: %s:0x%08x\n", psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName, ui32Offset);
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              talreg_WriteWord

******************************************************************************/
static IMG_RESULT talreg_WriteWord (
    TAL_sMemSpaceCB *				psMemSpaceCB,
    IMG_UINT32                      ui32Offset,
    IMG_UINT64                      ui64Value,
	IMG_UINT32						ui32Flags
)
{
    DEVIF_psDeviceCB		psDevifCB = talreg_GetDevice(psMemSpaceCB, &ui32Offset);
	IMG_RESULT				rResult = IMG_SUCCESS;
	IMG_ASSERT(psMemSpaceCB);

	IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);

#ifdef TAL_DEBUG_DEVIF
    printf("WriteReg 0x%x:0x%lx\n", ui32Offset, ui64Value);
#endif

	// Flush write coalesce buffer if it is in use
	talmem_FlushLoadBuffer(psMemSpaceCB->psPdumpContext);

	// Check offset
	rResult = talreg_CheckOffset(psMemSpaceCB, ui32Offset);
	if (rResult != IMG_SUCCESS) return rResult;

	// Call on to devif.
	if (ui32Flags & TAL_WORD_FLAGS_32BIT)
	{
		psDevifCB->pfnWriteRegister(psDevifCB, psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
									psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset, IMG_UINT64_TO_UINT32(ui64Value));
	}
	else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
	{
		if(psDevifCB->pfnWriteRegister64)
		{
			psDevifCB->pfnWriteRegister64(psDevifCB, psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
										psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset, ui64Value);
		}
		else
		{
			psDevifCB->pfnWriteRegister(psDevifCB, psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
										psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset, (IMG_UINT32)(ui64Value & 0xFFFFFFFF));
			psDevifCB->pfnWriteRegister(psDevifCB, psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
										psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset + 4, (IMG_UINT32)(ui64Value >> 32));
		}
	}
	else
		IMG_ASSERT(IMG_FALSE);

	// Devif needs return codes in the future
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              talreg_ReadWord

******************************************************************************/
static IMG_RESULT talreg_ReadWord (
    TAL_sMemSpaceCB *				psMemSpaceCB,
    IMG_UINT32                      ui32Offset,
    IMG_PVOID						pvValue,
	IMG_UINT32						ui32Flags
)
{
    DEVIF_psDeviceCB		psDevifCB = talreg_GetDevice(psMemSpaceCB, &ui32Offset);
	IMG_RESULT				rResult = IMG_SUCCESS;
	IMG_PUINT64				pui64Value = pvValue;
	IMG_PUINT32				pui32Value = pvValue;

	IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);

	// Check offset
	rResult = talreg_CheckOffset(psMemSpaceCB, ui32Offset);
	if (rResult != IMG_SUCCESS) return rResult;

	/* Read word via devif */
	if (ui32Flags & TAL_WORD_FLAGS_32BIT)
	{
		*pui32Value = psDevifCB->pfnReadRegister(psDevifCB, psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
											psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset);
	}
	else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
	{
		if(psDevifCB->pfnReadRegister64)
		{
			*pui64Value = psDevifCB->pfnReadRegister64(psDevifCB, psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
											psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset);
		}
		else
		{
			IMG_UINT32 ui32Result = 0;
			ui32Result = psDevifCB->pfnReadRegister(psDevifCB, psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
										psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset + 4);
			*pui64Value = ui32Result;
			*pui64Value <<= 32;
			ui32Result = psDevifCB->pfnReadRegister(psDevifCB, psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
										psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset);
			*pui64Value += ui32Result;
		}
	}
	else
		IMG_ASSERT(IMG_FALSE);


	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              TALREG_WriteWord

******************************************************************************/
IMG_RESULT TALREG_WriteWord (
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT64						ui64Value,
	IMG_HANDLE						hContextMemspace,
	IMG_BOOL						bCallback,
	IMG_UINT32						ui32Flags

)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	TAL_sMemSpaceCB *		psContextMemSpaceCB = (TAL_sMemSpaceCB*)hContextMemspace;
	IMG_HANDLE				hPdumpMemSpace = psContextMemSpaceCB ? psContextMemSpaceCB->hPdumpMemSpace : NULL;

    TAL_THREADSAFE_INIT;

    /* If this is not a call from a callback...*/
    if (!bCallback)
    {
        /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
        TAL_THREADSAFE_LOCK;

#if defined (__TAL_CALLBACK_SUPPORT__)
        /* Check for callback...*/
        TALCALLBACK_CheckMemoryAccessCallback (hMemSpace, TAL_EVENT_PRE_WRITE_WORD, ui32Offset, ui64Value);
#endif
    }

    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	// Check that this context is not excluded by an if statement
	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		// Perform the write word
		rResult = talreg_WriteWord(psMemSpaceCB, ui32Offset, ui64Value, ui32Flags);
	}
	// In the case of a stubbed out device setup dummy registers
	else if ((gui32TargetAppFlags & TALAPP_STUB_OUT_TARGET) ||
			(psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_STUB_OUT))
	{
		if (ui32Flags & TAL_WORD_FLAGS_32BIT)
		{
			psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2] = (IMG_UINT32)ui64Value;
		}
		else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
		{
			*((IMG_PUINT64)(&psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2])) = ui64Value;
		}
		else
			IMG_ASSERT(IMG_FALSE);
		rResult = IMG_SUCCESS;
	}
	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    
    if (!bCallback)
    {
        /* If we are not inside a loop or on the first iteration */
        if (talpdump_Check(psPdumpContext))
        {
			PDUMP_CMD_sTarget stSrc, stDst;

			stSrc.hMemSpace = hPdumpMemSpace;
			stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
			stSrc.ui64Value = ui64Value;
			stSrc.ui64BlockAddr = 0;
			stSrc.eMemSpT = PDUMP_MEMSPACE_VALUE;

			stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
			stDst.ui64Value = ui32Offset;
			stDst.ui64BlockAddr = 0;
			stDst.eMemSpT = PDUMP_MEMSPACE_REGISTER;

			if (ui32Flags & TAL_WORD_FLAGS_32BIT)
				PDUMP_CMD_WriteWord32(stSrc, stDst, IMG_NULL, 0, 0);
			else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
				PDUMP_CMD_WriteWord64(stSrc, stDst, IMG_NULL, 0, 0);
			else
				IMG_ASSERT(IMG_FALSE);
		}

#if defined (__TAL_CALLBACK_SUPPORT__)
        /* Check for callback...*/
        TALCALLBACK_CheckMemoryAccessCallback(hMemSpace, TAL_EVENT_POST_WRITE_WORD, ui32Offset, ui64Value);
#endif

        /* Allow API to be re-entered */
        TAL_THREADSAFE_UNLOCK;
	}
    return rResult;
}


/*!
******************************************************************************

 @Function              talreg_WriteDeviceMemRef

******************************************************************************/
IMG_RESULT talreg_WriteDeviceMemRef(
    IMG_HANDLE              hMemSpace,
    IMG_UINT32              ui32Offset,
    IMG_HANDLE              hRefDeviceMem,
    IMG_UINT64              ui64RefOffset,
    IMG_BOOL                bCallback,
    IMG_UINT32				ui32Flags
    )
{
	TAL_psDeviceMemCB		psRefDeviceMemCB = (TAL_psDeviceMemCB)hRefDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_UINT64              ui64Value;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	TAL_sMemSpaceCB *		psRefMemSpaceCB = (TAL_sMemSpaceCB*)psRefDeviceMemCB->hMemSpace;

    TAL_THREADSAFE_INIT;

    /* If this is not a call from a callback...*/
    if (!bCallback)
    {
        /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
        TAL_THREADSAFE_LOCK;
	}

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Calculate memory address */
    ui64Value = talmem_GetDeviceMemRef(hRefDeviceMem, ui64RefOffset);

    /* If this is not a call from a callback...*/
    if (!bCallback)
    {
#if defined (__TAL_CALLBACK_SUPPORT__)
        /* Check for callback...*/
        TALCALLBACK_CheckMemoryAccessCallback (hMemSpace, TAL_EVENT_PRE_WRITE_WORD, ui32Offset, ui64Value);
#endif
    }

	// Check that this context is not excluded by an if statement
	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		// Write to the device
		rResult = talreg_WriteWord(psMemSpaceCB, ui32Offset, ui64Value, ui32Flags);
	}

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}
    /* If this is not a call from a callback...*/
    if (!bCallback)
    {
        /* If we are not inside a loop or on the first iteration */
        if (talpdump_Check(psPdumpContext))
        {
			PDUMP_CMD_sTarget stSrc, stDst;

			stSrc.hMemSpace = psRefMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = psRefDeviceMemCB->ui32MemBlockId;
			stSrc.ui64Value = ui64RefOffset;
			stSrc.ui64BlockAddr = 0;
			stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;

			stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
			stDst.ui64Value = ui32Offset;
			stDst.ui64BlockAddr = 0;
			stDst.eMemSpT = PDUMP_MEMSPACE_REGISTER;

			if (ui32Flags & TAL_WORD_FLAGS_32BIT)
				PDUMP_CMD_WriteWord32(stSrc, stDst, IMG_NULL, 0, IMG_UINT64_TO_UINT32(ui64Value));
			else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
				PDUMP_CMD_WriteWord64(stSrc, stDst, IMG_NULL, 0, ui64Value);
			else
				IMG_ASSERT(IMG_FALSE);
        }

    #if defined (__TAL_CALLBACK_SUPPORT__)
        /* Check for callback...*/
        TALCALLBACK_CheckMemoryAccessCallback (hMemSpace, TAL_EVENT_POST_WRITE_WORD, ui32Offset, ui64Value);
    #endif

        /* Allow API to be re-entered */
        TAL_THREADSAFE_UNLOCK;
    }
    return rResult;
}


/*!
******************************************************************************

 @Function              TALREG_ReadWord

******************************************************************************/
IMG_RESULT TALREG_ReadWord(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_PVOID                       pvValue,
    IMG_BOOL                        bToPoll,
	IMG_BOOL						bToSAB,
	IMG_UINT32						ui32VerCount,
	IMG_UINT32						ui32VerTime,
    IMG_BOOL                        bCallback,
	IMG_UINT32						ui32Flags
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

        TAL_THREADSAFE_INIT;

    /* If this is not a call from a callback...*/
    if (!bCallback)
    {
		/* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
        TAL_THREADSAFE_LOCK;
#if defined (__TAL_CALLBACK_SUPPORT__)
        /* Check for callback...*/
        TALCALLBACK_CheckMemoryAccessCallback (hMemSpace, TAL_EVENT_PRE_READ_WORD, ui32Offset, 0);
#endif
    }

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	// Check that this context is not excluded by an if statement
	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		// Read from the device
		rResult = talreg_ReadWord(psMemSpaceCB, ui32Offset, pvValue, ui32Flags);
	}
	// In the case of a stubbed out device setup dummy registers
	else if ((gui32TargetAppFlags & TALAPP_STUB_OUT_TARGET) ||
			(psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_STUB_OUT))
	{
		if (ui32Flags & TAL_WORD_FLAGS_32BIT)
		{
			*((IMG_PUINT32)pvValue) = psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2];
		}
		else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
		{
			*((IMG_PUINT64)pvValue) = *((IMG_PUINT64)(&psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2]));
		}
		else
			IMG_ASSERT(IMG_FALSE);
		rResult = IMG_SUCCESS;
	}

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    /* If this is not a call from a callback...*/
    if (!bCallback)
    {
		IMG_UINT64 ui64Value = 0;

		if (ui32Flags & TAL_WORD_FLAGS_32BIT)
			ui64Value = *(IMG_PUINT32)pvValue;
		else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			ui64Value = *(IMG_PUINT64)pvValue;
		else
			IMG_ASSERT(IMG_FALSE);

        /* If we are not inside a loop or on the first iteration */
        if (talpdump_Check(psPdumpContext))
        {			
			PDUMP_CMD_sTarget stSrc, stDst;

			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
			stSrc.ui64Value = ui32Offset;
			stSrc.ui64BlockAddr = 0;
			stSrc.eMemSpT = PDUMP_MEMSPACE_REGISTER;

			stDst.hMemSpace = NULL;
			stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
			stDst.ui64Value = (IMG_UINT32)ui64Value;
			stDst.ui64BlockAddr = 0;
			stDst.eMemSpT = PDUMP_MEMSPACE_VALUE;

			if (bToPoll)
			{
				if (ui32VerCount == 0) ui32VerCount = gui32TalRdwVerifyPollIter;
				if (ui32VerTime == 0) ui32VerTime = gui32TalRdwVerifyPollTime;
				
				if (ui32Flags & TAL_WORD_FLAGS_32BIT)
					PDUMP_CMD_Poll32(stSrc, (IMG_UINT32)ui64Value, 0xFFFFFFFF, ui32VerCount, ui32VerTime, 0, IMG_NULL);
				else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
					PDUMP_CMD_Poll64(stSrc, ui64Value, 0xFFFFFFFFFFFFFFFFULL, ui32VerCount, ui32VerTime, 0, IMG_NULL);
				else
					IMG_ASSERT(IMG_FALSE);
			}
			else
			{
				if (ui32Flags & TAL_WORD_FLAGS_32BIT)
					PDUMP_CMD_ReadWord32(stSrc, stDst, bToSAB);
				else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
				{
					stDst.ui64Value = ui64Value;
					PDUMP_CMD_ReadWord64(stSrc, stDst, bToSAB);
				}
				else
					IMG_ASSERT(IMG_FALSE);
			}
        }

#if defined (__TAL_CALLBACK_SUPPORT__)
        /* Check for callback...*/
        TALCALLBACK_CheckMemoryAccessCallback (hMemSpace, TAL_EVENT_POST_READ_WORD, ui32Offset, ui64Value);
#endif

        /* Allow API to be re-entered */
        TAL_THREADSAFE_UNLOCK;
    }

    return rResult;
}


/*!
******************************************************************************

 @Function              TALREG_WriteWords32

******************************************************************************/
 IMG_RESULT TALREG_WriteWords32 (
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32                      ui32WordCount,
    IMG_UINT32 *                    pui32Values
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_UINT32              ui32Index;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	DEVIF_psDeviceCB		psDevifCB;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	// Flush write coalesce buffer if it is in use
		talmem_FlushLoadBuffer(psPdumpContext);

	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		psDevifCB = &psMemSpaceCB->psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;

		if (psDevifCB->pfnRegWriteWords != IMG_NULL)
		{
			IMG_UINT64 ui64RegAddr = ui32Offset + psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui64DeviceBaseAddr +
					psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr;
			psDevifCB->pfnRegWriteWords (psDevifCB,pui32Values,ui64RegAddr,ui32WordCount);
		}
		else
		{
			for (ui32Index = 0; (ui32Index <ui32WordCount ) && (IMG_SUCCESS == rResult ) ; ui32Index++)
			{
				rResult = talreg_WriteWord(psMemSpaceCB, ui32Offset + ui32Index *sizeof (IMG_UINT32), pui32Values[ui32Index], TAL_WORD_FLAGS_32BIT);
				if (rResult != IMG_SUCCESS) break;
			}
		}
	}

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stDst;

		stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
		stDst.ui64Value = ui32Offset;
		stDst.ui64BlockAddr = 0;
		stDst.eMemSpT = PDUMP_MEMSPACE_REGISTER;

		PDUMP_CMD_LoadWords(stDst, pui32Values, ui32WordCount);
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

 @Function              TALREG_ReadWords32

******************************************************************************/
IMG_RESULT TALREG_ReadWords32(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32                      ui32WordCount,
    IMG_UINT32 *                    pui32Value
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_UINT32              ui32Index;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		for (ui32Index = 0; (ui32Index <ui32WordCount ); ui32Index++)
		{
			// Read from the device
			rResult = talreg_ReadWord(psMemSpaceCB, ui32Offset+ ui32Index *sizeof (IMG_UINT32), pui32Value + ui32Index, TAL_WORD_FLAGS_32BIT);
			if (IMG_SUCCESS != rResult) return rResult;
		}
	}

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stSrc;

		stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
		stSrc.ui64Value = ui32Offset;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_REGISTER;

        PDUMP_CMD_SaveWords(stSrc, pui32Value, ui32WordCount);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

 @Function              TALREG_Poll

******************************************************************************/
IMG_RESULT TALREG_Poll(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut,
    IMG_UINT32						ui32Flags
)
{
	TAL_sPdumpContext *			psPdumpContext;
	IMG_UINT32					ui32Count;
	IMG_UINT64					ui64ReadVal = 0;
	IMG_RESULT					rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *			psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	IMG_BOOL					bPass = IMG_FALSE;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		/* Test the value */
		for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
		{
			TAL_THREADSAFE_INIT;
			TAL_THREADSAFE_LOCK;

			// Read from the device
			rResult = talreg_ReadWord(psMemSpaceCB, ui32Offset, &ui64ReadVal, ui32Flags);
			if (rResult == IMG_SUCCESS)
				bPass = TAL_TestWord(ui64ReadVal, ui64RequValue, ui64Enable, 0, ui32CheckFuncIdExt);

			/* Allow API to be re-entered */
			TAL_THREADSAFE_UNLOCK;

			// Check result
			if (IMG_SUCCESS != rResult) break;

			// Check whether we meet the exit criteria
			if( !bPass )
			{
				tal_Idle(hMemSpace, ui32TimeOut);
			}
			else
				break;
		}
		if (ui32Count >= ui32PollCount)	rResult = IMG_ERROR_TIMEOUT;
	}

	{
		TAL_THREADSAFE_INIT;
		TAL_THREADSAFE_LOCK;

		/* If we are not inside a loop or on the first iteration */
		if (talpdump_Check(psPdumpContext))
		{
			PDUMP_CMD_sTarget stSrc;

			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
			stSrc.ui64Value = ui32Offset;
			stSrc.ui64BlockAddr = 0;
			stSrc.eMemSpT = PDUMP_MEMSPACE_REGISTER;

			if (ui32Flags & TAL_WORD_FLAGS_32BIT)
				PDUMP_CMD_Poll(stSrc, IMG_UINT64_TO_UINT32(ui64RequValue), IMG_UINT64_TO_UINT32(ui64Enable), ui32PollCount, ui32TimeOut, ui32CheckFuncIdExt & TAL_CHECKFUNC_MASK, IMG_NULL);
			else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
				PDUMP_CMD_Poll64(stSrc, ui64RequValue, ui64Enable, ui32PollCount, ui32TimeOut, ui32CheckFuncIdExt & TAL_CHECKFUNC_MASK, IMG_NULL);
			else
				IMG_ASSERT(IMG_FALSE);
		}
		/* Allow API to be re-entered */
		TAL_THREADSAFE_UNLOCK;
	}
	return rResult;
}

/*!
******************************************************************************

 @Function              TALREG_CircBufPoll32

******************************************************************************/
IMG_RESULT TALREG_CircBufPoll32(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT32                      ui32Offset,
	IMG_UINT32						ui32WriteOffsetVal,
	IMG_UINT32						ui32PacketSize,
	IMG_UINT32						ui32BufferSize,
	IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
)
{
    TAL_sPdumpContext *			psPdumpContext;
    IMG_RESULT                  rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *			psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	IMG_BOOL					bPass = IMG_FALSE;
	IMG_UINT32					ui32Count, ui32ReadVal;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	/* This command is only valid for register type memory spaces */
	IMG_ASSERT(psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		for (ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
		{
			TAL_THREADSAFE_INIT;
			TAL_THREADSAFE_LOCK;

			rResult = talreg_ReadWord(psMemSpaceCB, ui32Offset, &ui32ReadVal, TAL_WORD_FLAGS_32BIT);
			if (rResult == IMG_SUCCESS)
				bPass = TAL_TestWord(ui32ReadVal, ui32WriteOffsetVal, ui32PacketSize, ui32BufferSize, TAL_CHECKFUNC_CBP);

			/* Allow API to be re-entered */
			TAL_THREADSAFE_UNLOCK;

			if (IMG_SUCCESS != rResult) break;

			// Check whether we meet the exit criteria
			if (!bPass)
				tal_Idle(hMemSpace, ui32TimeOut);
			else
				break;
		}
		if (ui32Count >= ui32PollCount)	rResult = IMG_ERROR_TIMEOUT;
	}

	{
		TAL_THREADSAFE_INIT;
		TAL_THREADSAFE_LOCK;

		/* If we are not inside a loop or on the first iteration */
		if (talpdump_Check(psPdumpContext))
		{
			PDUMP_CMD_sTarget stSrc;

			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
			stSrc.ui64Value = ui32Offset;
			stSrc.ui64BlockAddr = 0;
			stSrc.eMemSpT = PDUMP_MEMSPACE_REGISTER;

			PDUMP_CMD_CircBufPoll(stSrc, ui32WriteOffsetVal, ui32PacketSize, ui32BufferSize, ui32PollCount, ui32TimeOut);
		}

		/* Allow API to be re-entered */
		TAL_THREADSAFE_UNLOCK;
	}
    return rResult;
}

/*!
******************************************************************************
##############################################################################
 Category:              TAL Memory Functions
##############################################################################
******************************************************************************/

// Some functions need to be diverted to a common function, inlines are used here to do that.
IMG_RESULT TALMEM_WriteWord(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                      ui64Value,
	IMG_HANDLE						hContextMemspace,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALMEM_ReadWord(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_PVOID						puiValue,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALMEM_Poll(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALMEM_CircBufPoll(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64                      ui64Offset,
	IMG_UINT64						ui64WriteOffsetVal,
	IMG_UINT64						ui64PacketSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut,
	IMG_UINT32						ui32Flags
);
IMG_RESULT talmem_Malloc(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pHostMem,
    IMG_UINT64                      ui64Size,
    IMG_UINT64                      ui64Alignment,
    IMG_HANDLE *                    phDeviceMem,
    IMG_BOOL                        bUpdateDevice,
	IMG_CHAR*						pszMemName,
	IMG_UINT64						ui64DeviceAddr
);
IMG_RESULT talmem_CopyToFromHost(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                      ui64Size,
	IMG_BOOL						bToHost
);
IMG_RESULT talmem_WriteDeviceMemRef(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_HANDLE                      hRefDeviceMem,
    IMG_UINT64                      ui64RefOffset,
	IMG_UINT32						ui32Flags
);
inline IMG_RESULT TALMEM_WriteDevMemRef32(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_HANDLE hRefDeviceMem, IMG_UINT64 ui64RefOffset)
{
	return talmem_WriteDeviceMemRef(hDeviceMem,ui64Offset,hRefDeviceMem,ui64RefOffset,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALMEM_WriteDevMemRef64(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_HANDLE hRefDeviceMem, IMG_UINT64 ui64RefOffset)
{
	return talmem_WriteDeviceMemRef(hDeviceMem,ui64Offset,hRefDeviceMem,ui64RefOffset,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALMEM_UpdateDevice(IMG_HANDLE hDeviceMem)
{
	return talmem_CopyToFromHost(hDeviceMem,0,0,IMG_TRUE);
}
inline IMG_RESULT TALMEM_UpdateDeviceRegion(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT64 ui64Size)
{
	return talmem_CopyToFromHost(hDeviceMem,ui64Offset,ui64Size,IMG_TRUE);
}
inline IMG_RESULT TALMEM_UpdateHost(IMG_HANDLE hDeviceMem)
{
	return talmem_CopyToFromHost(hDeviceMem,0,0,IMG_FALSE);
}
inline IMG_RESULT TALMEM_UpdateHostRegion(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT64 ui64Size)
{
	return talmem_CopyToFromHost(hDeviceMem,ui64Offset,ui64Size,IMG_FALSE);
}
inline IMG_RESULT TALMEM_Malloc(IMG_HANDLE hMemSpace, IMG_CHAR* pHostMem, IMG_UINT64 ui64Size, IMG_UINT64 ui64Alignment,
								IMG_HANDLE* phDeviceMem, IMG_BOOL bUpdateDevice, IMG_CHAR* pszMemName)
{
	return talmem_Malloc(hMemSpace,pHostMem,ui64Size,ui64Alignment,phDeviceMem,bUpdateDevice,pszMemName,TAL_BASE_ADDR_UNDEFINED);
}
inline IMG_RESULT TALMEM_Map(IMG_HANDLE hMemSpace, IMG_CHAR* pHostMem, IMG_UINT64 ui64DeviceAddr, IMG_UINT64 ui64Size,
							 IMG_UINT64 ui64Alignment, IMG_HANDLE* phDeviceMem)
{
	return talmem_Malloc(hMemSpace,pHostMem,ui64Size,ui64Alignment,phDeviceMem,IMG_FALSE,IMG_NULL,ui64DeviceAddr);
}
inline IMG_RESULT TALMEM_CircBufPoll32(IMG_HANDLE hDeviceMem,IMG_UINT64 ui64Offset,IMG_UINT32 ui32WriteOffsetVal, IMG_UINT32 ui32PacketSize,
									   IMG_UINT32 ui32BufferSize, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALMEM_CircBufPoll(hDeviceMem,ui64Offset,ui32WriteOffsetVal,ui32PacketSize,ui32BufferSize,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALMEM_CircBufPoll64(IMG_HANDLE hDeviceMem,IMG_UINT64 ui64Offset, IMG_UINT64 ui64WriteOffsetVal, IMG_UINT64 ui64PacketSize,
									   IMG_UINT64 ui64BufferSize, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALMEM_CircBufPoll(hDeviceMem,ui64Offset,ui64WriteOffsetVal,ui64PacketSize,ui64BufferSize,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALMEM_Poll32(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT32 ui32CheckFuncIdExt, IMG_UINT32 ui32RequValue,
								IMG_UINT32 ui32Enable, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALMEM_Poll(hDeviceMem,ui64Offset,ui32CheckFuncIdExt,ui32RequValue,ui32Enable,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALMEM_Poll64(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT32 ui32CheckFuncIdExt, IMG_UINT64 ui64RequValue,
								IMG_UINT64 ui64Enable, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALMEM_Poll(hDeviceMem,ui64Offset,ui32CheckFuncIdExt,ui64RequValue,ui64Enable,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALMEM_ReadWord32(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT32* pui32Value)
{
	return TALMEM_ReadWord(hDeviceMem,ui64Offset,pui32Value,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALMEM_ReadWord64(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT64* pui64Value)
{
	return TALMEM_ReadWord(hDeviceMem,ui64Offset,pui64Value,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALMEM_WriteWordDefineContext32(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT32 ui32Value, IMG_HANDLE hContextMemspace)
{
	return TALMEM_WriteWord(hDeviceMem,ui64Offset,ui32Value,hContextMemspace,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALMEM_WriteWordDefineContext64(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT64 ui64Value, IMG_HANDLE hContextMemspace)
{
	return TALMEM_WriteWord(hDeviceMem,ui64Offset,ui64Value,hContextMemspace,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALMEM_WriteWord32(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT32 ui32Value)
{
	return TALMEM_WriteWord(hDeviceMem,ui64Offset,ui32Value,IMG_NULL,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALMEM_WriteWord64(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT64 ui64Value)
{
	return TALMEM_WriteWord(hDeviceMem,ui64Offset,ui64Value,IMG_NULL,TAL_WORD_FLAGS_64BIT);
}

/*!
******************************************************************************
##############################################################################
 Category:              TAL Memory Functions
##############################################################################
******************************************************************************/

// These memory twiddling functions are for testing 36 and 40bit functionality on a 32bit FPGA or 28bit Emulator
/*!
******************************************************************************

 @Function              talmem_Twiddle

******************************************************************************/
static IMG_UINT64 talmem_Twiddle(eTAL_Twiddle eMemTwiddle, IMG_UINT64 ui64Address)
{
	IMG_UINT64 ui64TwiddledAddr = 0;
	IMG_ASSERT(eMemTwiddle != TAL_TWIDDLE_NONE);

	switch(eMemTwiddle)
	{
		case TAL_TWIDDLE_36BIT_OLD:
			IMG_ASSERT((ui64Address & TAL_OLD_36BIT_TWIDDLE_MASK_LOW) == 0);
			// Takes bits 32-35 and returns them to 25-28
			ui64TwiddledAddr = ((ui64Address & TAL_OLD_36BIT_TWIDDLE_MASK_HIGH) >> TAL_OLD_36BIT_TWIDDLE_SHIFT) | (ui64Address & (~TAL_OLD_36BIT_TWIDDLE_MASK_HIGH));
			break;
		case TAL_TWIDDLE_36BIT:
			IMG_ASSERT((ui64Address & TAL_36BIT_TWIDDLE_MASK_LOW) == 0);
			// Takes bits 32-35 and returns them to 25-28
			ui64TwiddledAddr = ((ui64Address & TAL_36BIT_TWIDDLE_MASK_HIGH) >> TAL_36BIT_TWIDDLE_SHIFT) | (ui64Address & (~TAL_36BIT_TWIDDLE_MASK_HIGH));
			break;
		case TAL_TWIDDLE_40BIT:
			IMG_ASSERT((ui64Address & TAL_40BIT_TWIDDLE_MASK_LOW) == 0);
			// Takes bits 32-35 and returns them to 25-28
			ui64TwiddledAddr = ((ui64Address & TAL_40BIT_TWIDDLE_MASK_HIGH) >> TAL_40BIT_TWIDDLE_SHIFT) | (ui64Address & (~TAL_40BIT_TWIDDLE_MASK_HIGH));
			break;
		case TAL_TWIDDLE_DROPTOP32:
			// Drops the top 32bits
			ui64TwiddledAddr = ui64Address & TAL_TWIDDLE_DROP32_MASK;
			break;
		default:
			printf("ERROR: Invalid mem twiddling value set\n");
			IMG_ASSERT(IMG_FALSE);
			break;
	}
	return ui64TwiddledAddr;
}


 /*!
******************************************************************************

 @Function				talmem_GetDevice

******************************************************************************/
static DEVIF_sDeviceCB * talmem_GetDevice (
	TAL_sMemSpaceCB *				psMemSpaceCB,
	IMG_UINT64 *					pui64Offset,
	IMG_UINT64 *					pui64Size
)
{
	TAL_psSubDeviceCB	psSubDeviceCB;
	TAL_psDeviceInfo	psDevInfo = psMemSpaceCB->psMemSpaceInfo->psDevInfo;

	/* Check to see if we should be using a sub device */
	psSubDeviceCB = psDevInfo->psSubDeviceCB;
	while (psSubDeviceCB != IMG_NULL)
	{
		if (psSubDeviceCB->ui64MemStartAddress <= *pui64Offset
			&& (psSubDeviceCB->ui64MemStartAddress + psSubDeviceCB->ui64MemSize > *pui64Offset))
		{
			*pui64Offset -= psSubDeviceCB->ui64MemStartAddress;
			if (psSubDeviceCB->ui64MemSize - *pui64Offset < *pui64Size)
			{
				*pui64Size = psSubDeviceCB->ui64MemSize - *pui64Offset;
			}
			return &psSubDeviceCB->sDevIfDeviceCB;
		}
		psSubDeviceCB = psSubDeviceCB->pNext;
	}
	/* If we haven't found a subdevice, return the main device */
	return &psDevInfo->sDevIfDeviceCB;
}

/*!
******************************************************************************

 @Function				talmem_LocateBlock

******************************************************************************/
static TAL_sDeviceMemCB * talmem_LocateBlock(
    IMG_UINT64              ui64DeviceMem
)
{
    IMG_UINT32                  i;
    TAL_psDeviceMemCB           psDeviceMemCB = IMG_NULL;
	TAL_DeviceList *			psDeviceListIter, *psPrevDevItem;
	TAL_sMemSpaceCB *			psMemSpaceCB;
	IMG_HANDLE					hMemSpItr;

	//create a structure in which to store memory blocks we have recently used
	//to try and reduce access time
	if(gbDevListInit == IMG_FALSE)
	{
		memset(gasDeviceList, 0, sizeof(TAL_DeviceList) * MAX_DEV_LIST);
		gbDevListInit = IMG_TRUE;
	}

	psDeviceListIter = gpsDeviceList;
	psPrevDevItem = IMG_NULL;
	while (psDeviceListIter)
	{
		/* If the required device memory address is in this block...*/
        if (
                (ui64DeviceMem >= psDeviceListIter->psDeviceMemCB->ui64DeviceMem) &&
                (ui64DeviceMem < (psDeviceListIter->psDeviceMemCB->ui64DeviceMem + psDeviceListIter->psDeviceMemCB->ui64Size))
            )
		{
			break;
		}
		psPrevDevItem = psDeviceListIter;
		psDeviceListIter = psDeviceListIter->pdlNextDevice;
	}

	// If we found the right control block, remove it from its current list position
	if (psDeviceListIter)
	{
		// If there is no previous, the next item belongs at the head of the list
		if (psPrevDevItem == IMG_NULL)
		{
			gpsDeviceList = psDeviceListIter->pdlNextDevice;
		}
		else
		{
			psPrevDevItem->pdlNextDevice = psDeviceListIter->pdlNextDevice;
		}
		psDeviceMemCB = psDeviceListIter->psDeviceMemCB;
		psDeviceListIter->psDeviceMemCB = IMG_NULL;
	}
	// Otherwise search the whole structure for it
	else
	{
		psMemSpaceCB = TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hMemSpItr);
		while (psMemSpaceCB)
		{
			/* If this is a memory memory space...*/
			if (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY)
			{
				psDeviceMemCB = psMemSpaceCB->psDevMemList;
				while (psDeviceMemCB != IMG_NULL)
				{
					/* If the required device memory address is in this block...*/
					if (
							(ui64DeviceMem >= psDeviceMemCB->ui64DeviceMem) &&
							(ui64DeviceMem < (psDeviceMemCB->ui64DeviceMem + psDeviceMemCB->ui64Size))
					)
					{
						break;
					}
					/* Move to next block...*/
					psDeviceMemCB = psDeviceMemCB->psNextDeviceMemCB;
				}
				if (psDeviceMemCB != IMG_NULL)
					break;
			}
			psMemSpaceCB = TALITERATOR_GetNext(hMemSpItr);
		}
		TALITERATOR_Destroy(hMemSpItr);
	}
    if(psDeviceMemCB == IMG_NULL)
		return IMG_NULL;

	// Add located item to top of list

	// Find a free list item
    for ( i = 0; i < MAX_DEV_LIST; i++ )
	{
		if(gasDeviceList[i].psDeviceMemCB == IMG_NULL)
			break;
	}
	// If the list is full, remove the last element
	if ( i >= MAX_DEV_LIST )
	{
		psDeviceListIter = gpsDeviceList;
		while (psDeviceListIter->pdlNextDevice != IMG_NULL)
		{
			psPrevDevItem = psDeviceListIter;
			psDeviceListIter = psDeviceListIter->pdlNextDevice;
		}
		// Remove the last item from the list
		psPrevDevItem->pdlNextDevice = IMG_NULL;
	}
	else
	{
		psDeviceListIter = &gasDeviceList[i];
	}

	// Setup our new list element
	memset(psDeviceListIter, 0, sizeof(TAL_DeviceList));
	psDeviceListIter->psDeviceMemCB = psDeviceMemCB;
	psDeviceListIter->pdlNextDevice = gpsDeviceList;
	gpsDeviceList = psDeviceListIter;



    return psDeviceMemCB;
}

/*!
******************************************************************************

 @Function				talmem_UpdateHost

******************************************************************************/
static IMG_RESULT talmem_UpdateHost(
	TAL_psDeviceMemCB               psDeviceMemCB,
    IMG_UINT64                      ui64Offset,
	IMG_UINT64                      ui64Size
)
{
	TAL_psMemSpaceCB		psMemSpaceCB = (TAL_psMemSpaceCB)psDeviceMemCB->hMemSpace;
	IMG_UINT64				ui64DevSize, ui64Remaining, ui64DeviceMem, ui64CurOffset;
	DEVIF_psDeviceCB		psDevifCB;

	if (gui32TargetAppFlags & TALAPP_NO_COPY_DEV_TO_HOST) return IMG_SUCCESS;

	ui64Remaining = ui64Size;
	ui64CurOffset = ui64Offset;
	do
	{
		ui64DevSize = ui64Remaining;
		ui64DeviceMem = psDeviceMemCB->ui64DeviceMem + ui64CurOffset;
		psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64DeviceMem, &ui64DevSize);
		/* Copy device to host...*/
		if (psDevifCB->pfnCopyDeviceToHost)
		{
		  psDevifCB->pfnCopyDeviceToHost(psDevifCB,
										  TAL_MEM_TWIDDLE(ui64DeviceMem, psMemSpaceCB->psMemSpaceInfo->psDevInfo->eTALTwiddle),  
										  psDeviceMemCB->pHostMem+ui64CurOffset, 
										  IMG_UINT64_TO_UINT32(ui64DevSize));
		} 
		else
		{
			IMG_INT i;
			for (i = 0; i < ui64DevSize; i+=4)
			{
				*(IMG_PUINT32)(psDeviceMemCB->pHostMem + ui64CurOffset + i) = psDevifCB->pfnReadMemory(psDevifCB, 
										  TAL_MEM_TWIDDLE(ui64DeviceMem + i, psMemSpaceCB->psMemSpaceInfo->psDevInfo->eTALTwiddle));
			}
		}
		ui64Remaining -= ui64DevSize;
		ui64CurOffset += ui64DevSize;
	}while (ui64Remaining > 0);
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				talmem_UpdateDevice

******************************************************************************/
static IMG_RESULT talmem_UpdateDevice(
	TAL_psMemSpaceCB				psMemSpaceCB,
	IMG_PUINT8						pui8Host,
	IMG_UINT64               		ui64BaseDevMem,
    IMG_UINT64                      ui64Offset,
	IMG_UINT64                      ui64Size
)
{
	DEVIF_psDeviceCB		psDevifCB;
	IMG_UINT64 ui64Remaining = ui64Size;
	IMG_UINT64 ui64CurOffset = ui64Offset;
	IMG_UINT64 ui64DeviceMem, ui64DevSize;

	do
	{
		ui64DevSize = ui64Remaining;
		ui64DeviceMem = ui64BaseDevMem + ui64CurOffset;
		psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64DeviceMem, &ui64DevSize);
		/* Copy device to host...*/
		if (psDevifCB->pfnCopyHostToDevice)
		{
			psDevifCB->pfnCopyHostToDevice(psDevifCB,
											(IMG_PVOID)(pui8Host+ui64CurOffset),
											TAL_MEM_TWIDDLE(ui64DeviceMem, psMemSpaceCB->psMemSpaceInfo->psDevInfo->eTALTwiddle),
											IMG_UINT64_TO_UINT32(ui64DevSize));
		}
		else
		{
			IMG_INT i;
			for (i = 0; i < ui64DevSize; i+=4)
			{
				psDevifCB->pfnWriteMemory(psDevifCB, 
										  TAL_MEM_TWIDDLE(ui64DeviceMem + i, psMemSpaceCB->psMemSpaceInfo->psDevInfo->eTALTwiddle),
										  *(IMG_PUINT32)(pui8Host+ui64CurOffset + i));
			}
		}
		ui64Remaining -= ui64DevSize;
		ui64CurOffset += ui64DevSize;
	}while (ui64Remaining > 0);
	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				talmem_GetDeviceMemRef

******************************************************************************/
static IMG_UINT64 talmem_GetDeviceMemRef(
	IMG_HANDLE                      hRefDeviceMem,
    IMG_UINT64                      ui64RefOffset
)
{
	TAL_psDeviceMemCB		psRefDeviceMemCB = (TAL_psDeviceMemCB)hRefDeviceMem;
	TAL_sMemSpaceCB *		psMemSpaceCB	 = (TAL_sMemSpaceCB *)psRefDeviceMemCB->hMemSpace;
	
	(void)psMemSpaceCB; // Unused in Release Version
	IMG_ASSERT(psMemSpaceCB);

	IMG_ASSERT(psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);
	TAL_ASSERT((ui64RefOffset+4) <= psRefDeviceMemCB->ui64Size, "Given offset outside of allocated block");

	return psRefDeviceMemCB->ui64DeviceMem + ui64RefOffset;
}

/*!
******************************************************************************

 @Function              talmem_ReadWord

******************************************************************************/
static IMG_RESULT talmem_ReadWord(
    TAL_psDeviceMemCB              psDeviceMemCB,
    IMG_UINT64                      ui64Offset,
	IMG_PVOID						puiValue,
	IMG_UINT32						ui32Flags
)
{
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	IMG_UINT64				ui64DeviceMem = psDeviceMemCB->ui64DeviceMem + ui64Offset;
	IMG_UINT64				ui64Size = (ui32Flags & TAL_WORD_FLAGS_64BIT) ? 8 : 4;
	DEVIF_psDeviceCB		psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64DeviceMem, &ui64Size);
	TAL_uint64				ui64 = { 0 };
	IMG_RESULT              rResult = IMG_SUCCESS;

	IMG_ASSERT(psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);
	// Twiddle the memory address if required
	ui64DeviceMem = TAL_MEM_TWIDDLE(ui64DeviceMem, psMemSpaceCB->psMemSpaceInfo->psDevInfo->eTALTwiddle);

	/* Call on to device interface */
	if (psDevifCB->pfnReadMemory == IMG_NULL)
	{
		// Use the block read if no word read exists
		if (psDevifCB->pfnCopyDeviceToHost)
			psDevifCB->pfnCopyDeviceToHost(psDevifCB, ui64DeviceMem, psDeviceMemCB->pHostMem+ui64Offset, (IMG_UINT32)ui64Size);
		ui64.Low = ((IMG_UINT32*)psDeviceMemCB->pHostMem)[ui64Offset>>2];
		if(ui32Flags & TAL_WORD_FLAGS_64BIT)
		{
			ui64.High = ((IMG_UINT32*)psDeviceMemCB->pHostMem)[(ui64Offset+4)>>2];
		}
	}
	else
	{
		if (ui32Flags & TAL_WORD_FLAGS_32BIT)
		{
			ui64.Low = psDevifCB->pfnReadMemory(psDevifCB, ui64DeviceMem);
		}
		else if(ui32Flags & TAL_WORD_FLAGS_64BIT)
		{
			if (psDevifCB->pfnReadMemory64 == IMG_NULL)
			{
				// If a 64 version of the ReadWord function does not exist, the operation is divided in two 32 reads
				ui64.Low = psDevifCB->pfnReadMemory(psDevifCB, ui64DeviceMem);
				ui64.High = psDevifCB->pfnReadMemory(psDevifCB, ui64DeviceMem + 4);
			}
			else
			{
				*(IMG_PUINT64)(&ui64) = psDevifCB->pfnReadMemory64(psDevifCB, ui64DeviceMem);
			}
		}
		else
		{
			IMG_ASSERT(IMG_FALSE);
		}
	}

	if (ui32Flags & TAL_WORD_FLAGS_64BIT)
		*(IMG_PUINT64)puiValue = *(IMG_PUINT64)(&ui64);
	else if(ui32Flags & TAL_WORD_FLAGS_32BIT)
		*(IMG_PUINT32)puiValue = ui64.Low;
	
	return rResult;
}

/*!
******************************************************************************

 @Function              talmem_WriteWord

******************************************************************************/
static
IMG_RESULT talmem_WriteWord(
    TAL_psDeviceMemCB              psDeviceMemCB,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                      ui64Value,
	IMG_UINT32						ui32Flags
)
{
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	IMG_UINT64				ui64DeviceMem = psDeviceMemCB->ui64DeviceMem + ui64Offset;
	IMG_UINT64				ui64Size;
	DEVIF_psDeviceCB		psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64DeviceMem, &ui64Size);
	IMG_RESULT              rResult = IMG_SUCCESS;

	IMG_ASSERT(psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);
	// Twiddle the memory address if required
	ui64DeviceMem = TAL_MEM_TWIDDLE(ui64DeviceMem, psMemSpaceCB->psMemSpaceInfo->psDevInfo->eTALTwiddle);

#if TAL_DEBUG_DEVIF
    printf("WriteMemWord off=0x%lx V=0x%lx f=0x%x\n", ui64Offset, ui64Value, ui32Flags);
#endif

	// Check that the write is within the memory bounds and update the host memory
	if(ui32Flags & TAL_WORD_FLAGS_32BIT)
	{
		TAL_ASSERT ((ui64Offset+4) <= psDeviceMemCB->ui64Size, "ERROR: Write out of range of memory block, offset:0x%" IMG_I64PR "X , size:0x%" IMG_I64PR "X\n", ui64Offset, ui64Size);
		*(IMG_PUINT32)(psDeviceMemCB->pHostMem + ui64Offset) = IMG_UINT64_TO_UINT32(ui64Value);
		ui64Size = 4;
	}
	else if(ui32Flags & TAL_WORD_FLAGS_64BIT)
	{
		TAL_ASSERT((ui64Offset+8) <= psDeviceMemCB->ui64Size, "ERROR: Write out of range of memory block, offset:0x%" IMG_I64PR "X , size:0x%" IMG_I64PR "X\n", ui64Offset, ui64Size);

#ifdef META
		/* Meta doesn't handle unaligned 64 bits writes */
		*(IMG_PUINT32)(psDeviceMemCB->pHostMem + ui64Offset) = (IMG_UINT32)(ui64Value & 0xFFFFFFFF);
		*(IMG_PUINT32)(psDeviceMemCB->pHostMem + ui64Offset + 4) = (IMG_UINT32)((ui64Value >> 32) & 0xFFFFFFFF);
#else /* META */
		*(IMG_PUINT64)(psDeviceMemCB->pHostMem + ui64Offset) = ui64Value;
#endif /* META */

		ui64Size = 8;
	}
	else
		TAL_ASSERT(IMG_FALSE, "ERROR: Unrecognised word length flag");

	if (TAL_CheckMemSpaceEnable(psMemSpaceCB))
	{
		if (psDevifCB->pfnWriteMemory == IMG_NULL)
		{
			// Do a bulk copy
			psDevifCB->pfnCopyHostToDevice(psDevifCB, psDeviceMemCB->pHostMem+ui64Offset, ui64DeviceMem, (IMG_UINT32)ui64Size);
		}
		else
		{
			if(ui32Flags & TAL_WORD_FLAGS_32BIT)
			{
				// write through devif
				psDevifCB->pfnWriteMemory(psDevifCB, ui64DeviceMem, IMG_UINT64_TO_UINT32(ui64Value));
			}
			else if(ui32Flags & TAL_WORD_FLAGS_64BIT)
			{
				// Use 64bit write if poss, otherwise use two 32bit writes
				if (psDevifCB->pfnWriteMemory64 == IMG_NULL)
				{
					// Use two 32bit writes through devif
					psDevifCB->pfnWriteMemory(psDevifCB, ui64DeviceMem, (IMG_UINT32)(ui64Value & 0xFFFFFFFF));				
					psDevifCB->pfnWriteMemory(psDevifCB, ui64DeviceMem + 4, (IMG_UINT32)(ui64Value >> 32));
				}
				else
				{
					// Use one 64bit write through devif
					psDevifCB->pfnWriteMemory64(psDevifCB, ui64DeviceMem, ui64Value);
				}
			}
		}
	}
	return rResult;
}

/*!
******************************************************************************

 @Function              TALMEM_WriteWord

******************************************************************************/
IMG_RESULT TALMEM_WriteWord(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                      ui64Value,
	IMG_HANDLE						hContextMemspace,
	IMG_UINT32						ui32Flags
)
{
    TAL_psDeviceMemCB      psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	TAL_sMemSpaceCB *		psContextMemSpaceCB = (TAL_sMemSpaceCB *)hContextMemspace;
	IMG_HANDLE				hPdumpMemSpace = psContextMemSpaceCB ? psContextMemSpaceCB->hPdumpMemSpace : NULL;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psDeviceMemCB != IMG_NULL);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	rResult = talmem_WriteWord(psDeviceMemCB, ui64Offset, ui64Value, ui32Flags);

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    /* If we are not inside a loop or on the first iteration */
    if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_sTarget stSrc, stDest;
	
		stSrc.hMemSpace = hPdumpMemSpace;
		stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
		stSrc.ui64Value = ui64Value;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_VALUE;

		stDest.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stDest.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
		stDest.ui64Value = ui64Offset;
		stDest.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
		stDest.eMemSpT = PDUMP_MEMSPACE_MEMORY;

		if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			PDUMP_CMD_WriteWord64(stSrc, stDest, NULL, 0, 0);
		else
			PDUMP_CMD_WriteWord32(stSrc, stDest, NULL, 0, 0);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function              TALMEM_ReadWord

******************************************************************************/
IMG_RESULT TALMEM_ReadWord(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_PVOID						puiValue,
	IMG_UINT32						ui32Flags
)
{
    TAL_psDeviceMemCB      psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
    IMG_RESULT				rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	IMG_ASSERT (psDeviceMemCB != IMG_NULL);
    /* Ensure that the region is within the block */
	if(ui32Flags & TAL_WORD_FLAGS_32BIT)
	{
		TAL_ASSERT((ui64Offset+4) <= psDeviceMemCB->ui64Size, "Given offset outside of allocated block");
	}
	else if(ui32Flags & TAL_WORD_FLAGS_64BIT)
	{
		IMG_ASSERT((ui64Offset+8) <= psDeviceMemCB->ui64Size);
	}
	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (TAL_CheckMemSpaceEnable(psMemSpaceCB))
		rResult = talmem_ReadWord(psDeviceMemCB, ui64Offset, puiValue, ui32Flags);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stSrc, stDest;

		stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
		stSrc.ui64Value = ui64Offset;
		stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
		stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;

		stDest.hMemSpace = NULL;
		stDest.ui32BlockId = TAL_NO_MEM_BLOCK;
		stDest.ui64Value = 0;
		stDest.ui64BlockAddr = 0;
		stDest.eMemSpT = PDUMP_MEMSPACE_VALUE;

		if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			PDUMP_CMD_ReadWord64(stSrc, stDest, IMG_FALSE);
		else
			PDUMP_CMD_ReadWord32(stSrc, stDest, IMG_FALSE);
	}
    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function              TALMEM_Poll

******************************************************************************/
IMG_RESULT TALMEM_Poll(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut,
	IMG_UINT32						ui32Flags
)
{
	TAL_psDeviceMemCB		psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
	IMG_BOOL				bPass = IMG_FALSE;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	IMG_UINT32				ui32Count;
	IMG_UINT64				ui64ReadVal;

    IMG_ASSERT(psMemSpaceCB);
	IMG_ASSERT(gbInitialised);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (TAL_CheckMemSpaceEnable(psMemSpaceCB))
	{
		/* Test the value */
		for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
		{
			TAL_THREADSAFE_INIT;
			TAL_THREADSAFE_LOCK;

			rResult = talmem_ReadWord(psDeviceMemCB, ui64Offset, &ui64ReadVal, ui32Flags);
			if (rResult == IMG_SUCCESS)
				bPass = TAL_TestWord(ui64ReadVal, ui64RequValue, ui64Enable, 0, ui32CheckFuncIdExt);

			TAL_THREADSAFE_UNLOCK;

			// Check result
			if (IMG_SUCCESS != rResult)break;

			// Check whether we meet the exit criteria
			if (!bPass)
				tal_Idle(psDeviceMemCB->hMemSpace, ui32TimeOut);
			else
				break;
		}
		if (ui32Count >= ui32PollCount)	rResult = IMG_ERROR_TIMEOUT;
	}

	{
		TAL_THREADSAFE_INIT;
		TAL_THREADSAFE_LOCK;

		/* If we are not inside a loop or on the first iteration */
		if (talpdump_Check(psPdumpContext))
		{
			PDUMP_CMD_sTarget stSrc;
			
			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
			stSrc.ui64Value = ui64Offset;
			stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;
			
			if (ui32Flags & TAL_WORD_FLAGS_32BIT)
				PDUMP_CMD_Poll32(stSrc, IMG_UINT64_TO_UINT32(ui64RequValue), IMG_UINT64_TO_UINT32(ui64Enable), ui32PollCount, ui32TimeOut, ui32CheckFuncIdExt & TAL_CHECKFUNC_MASK, IMG_NULL);
			else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
				PDUMP_CMD_Poll64(stSrc, ui64RequValue, ui64Enable, ui32PollCount, ui32TimeOut, ui32CheckFuncIdExt & TAL_CHECKFUNC_MASK, IMG_NULL);
		}

		TAL_THREADSAFE_UNLOCK;
	}
	return rResult;
}


/*!
******************************************************************************

 @Function              TALMEM_CircBufPoll

******************************************************************************/
IMG_RESULT TALMEM_CircBufPoll(
	IMG_HANDLE						hDeviceMem,
	IMG_UINT64                      ui64Offset,
	IMG_UINT64						ui64WriteOffsetVal,
	IMG_UINT64						ui64PacketSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut,
	IMG_UINT32						ui32Flags
)
{
    TAL_psDeviceMemCB      psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
    IMG_RESULT				rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	IMG_UINT32				ui32Count;
	TAL_sMemSpaceCB *		psMemSpaceCB;
	IMG_BOOL				bPass = IMG_FALSE;
	IMG_UINT64				ui64ReadVal;

    /* Ensure that the region is within the block */
	IMG_ASSERT(hDeviceMem);
    TAL_ASSERT((ui64Offset+4) <= psDeviceMemCB->ui64Size, "Given offset outside of allocated block");
	psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);

	if (TAL_CheckMemSpaceEnable(psMemSpaceCB))
	{
		for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
		{
			TAL_THREADSAFE_INIT;
			TAL_THREADSAFE_LOCK;

			rResult = talmem_ReadWord(psDeviceMemCB, ui64Offset, &ui64ReadVal, ui32Flags);
			if (rResult == IMG_SUCCESS)
				bPass = TAL_TestWord(ui64ReadVal, ui64WriteOffsetVal, ui64PacketSize, ui64BufferSize, TAL_CHECKFUNC_CBP);

			/* Allow API to be re-entered */
			TAL_THREADSAFE_UNLOCK;

			if (IMG_SUCCESS != rResult) break;

			// Check whether we meet the exit criteria
			if (!bPass)
				tal_Idle(psDeviceMemCB->hMemSpace, ui32TimeOut);
			else
				break;
		}
		if (ui32Count >= ui32PollCount)	rResult = IMG_ERROR_TIMEOUT;
	}

	{
		TAL_THREADSAFE_INIT;
		TAL_THREADSAFE_LOCK;

		// Pdump
		if (talpdump_Check(psPdumpContext) )
		{
			PDUMP_CMD_sTarget stSrc;
				
			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
			stSrc.ui64Value = ui64Offset;
			stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;
				
			if (ui32Flags & TAL_WORD_FLAGS_32BIT)
				PDUMP_CMD_CircBufPoll32(stSrc, IMG_UINT64_TO_UINT32(ui64WriteOffsetVal), IMG_UINT64_TO_UINT32(ui64PacketSize), IMG_UINT64_TO_UINT32(ui64BufferSize), ui32PollCount, ui32TimeOut);
			else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
				PDUMP_CMD_CircBufPoll64(stSrc, ui64WriteOffsetVal, ui64PacketSize, ui64BufferSize, ui32PollCount, ui32TimeOut);
		}
		/* Allow API to be re-entered */
		TAL_THREADSAFE_UNLOCK;
	}
	return rResult;
}

/*!
******************************************************************************

 @Function              talmem_Malloc

******************************************************************************/
IMG_RESULT talmem_Malloc(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pHostMem,
    IMG_UINT64                      ui64Size,
    IMG_UINT64                      ui64Alignment,
    IMG_HANDLE *                    phDeviceMem,
    IMG_BOOL                        bUpdateDevice,
	IMG_CHAR*						pszMemName,
	IMG_UINT64						ui64DeviceAddr
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_psDeviceMemCB       psDeviceMemCB;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
    TAL_sPdumpContext *     psPdumpContext;
	DEVIF_psDeviceCB		psDevifCB;
	IMG_UINT64				ui64Offset;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT( psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);
	/* Ensure that the size of the device memory is non-zero */
    IMG_ASSERT(ui64Size != 0);

    /* Create device memory control block */
	psDeviceMemCB = (TAL_psDeviceMemCB)IMG_MALLOC(sizeof(TAL_sDeviceMemCB));

    IMG_ASSERT(psDeviceMemCB != IMG_NULL);
    IMG_MEMSET(psDeviceMemCB, 0, sizeof(TAL_sDeviceMemCB));

    psDeviceMemCB->pHostMem					= pHostMem;
    psDeviceMemCB->ui64Size					= ui64Size;
    psDeviceMemCB->ui64Alignment			= ui64Alignment;
    psDeviceMemCB->hMemSpace				= hMemSpace;
    psDeviceMemCB->bBlockUpdatedToByHost    = IMG_FALSE;
    psDeviceMemCB->ui32MemBlockId			= psMemSpaceCB->ui32NextBlockId++;
    psMemSpaceCB->ui32NoMemCBs++;
	
#if defined (__TAL_USE_OSA__)
    //OSA_CritSectCreate(&psDeviceMemCB->hListMutex); 
#endif

	if( pszMemName )
	{
		psDeviceMemCB->pszMemBlockName = IMG_STRDUP(pszMemName );
	}
	else
	{
		/* Create a unique block name */
		const IMG_UINT32 uiMaxBlockIdLen = 11; // Maximum string length of the block ID
		psDeviceMemCB->pszMemBlockName = IMG_MALLOC(strlen(gpsDefaultBlockPrefix) + uiMaxBlockIdLen);
		sprintf( psDeviceMemCB->pszMemBlockName ,"%s%d",
					gpsDefaultBlockPrefix, psDeviceMemCB->ui32MemBlockId );
	}

	/* If device address specified...*/
	if (ui64DeviceAddr != TAL_BASE_ADDR_UNDEFINED)
	{
		psDeviceMemCB->ui64DeviceMem = ui64DeviceAddr;
		psDeviceMemCB->bMappedMem = IMG_TRUE;
	}
	else
	{
		switch (TAL_MEMORY_ALLOCATION_CONTROL)
		{
		case TAL_DEVFLAG_DEV_ALLOC:
			psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64Offset, &ui64Size);
			IMG_ASSERT(psDevifCB->pfnMallocHostVirt); // Ensure the required function exists
			psDeviceMemCB->ui64DeviceMemBase = psDevifCB->pfnMallocHostVirt(psDevifCB, psDeviceMemCB->ui64Alignment, psDeviceMemCB->ui64Size);
			psDeviceMemCB->ui64DeviceMem = psDeviceMemCB->ui64DeviceMemBase;
			break;
		case TAL_DEVFLAG_4K_PAGED_ALLOC:
		case TAL_DEVFLAG_4K_PAGED_RANDOM_ALLOC:
			{
				/* check that the address assignment is valid */
				IMG_UINT32	uin32Shift = 0;
				IMG_BOOL	bValidAlignment = IMG_FALSE;
				IMG_UINT64	ui64AlignHold   = psDeviceMemCB->ui64Alignment;

				if (ui64AlignHold != 0)
				{
					while(!(ui64AlignHold & 0x01)){ui64AlignHold >>= 1;uin32Shift++;}

					if (ui64AlignHold>>1 == 0)
						bValidAlignment = IMG_TRUE;
				}

				TAL_ASSERT (bValidAlignment,"MEMMSP_MallocDeviceMem - Invalid alignments\n");

				/* Use old SGX allocator */
				psDeviceMemCB->ui64DeviceMemBase = PAGEMEM_CxAllocateMemoryPage ((PAGEMEM_sContext *)psMemSpaceCB->hMemAllocator, IMG_UINT64_TO_UINT32(psDeviceMemCB->ui64Size),
																(TAL_MEMORY_ALLOCATION_CONTROL == TAL_DEVFLAG_4K_PAGED_RANDOM_ALLOC));

				psDeviceMemCB->ui64DeviceMem	  = psDeviceMemCB->ui64DeviceMemBase;

				if (psDeviceMemCB->ui64Alignment > 0)
				{
					IMG_UINT64 ui64AlignMask = 0x0FFFFFFFFULL * psDeviceMemCB->ui64Alignment;
					psDeviceMemCB->ui64DeviceMem = (psDeviceMemCB->ui64DeviceMemBase + ~ui64AlignMask) & ui64AlignMask;
				}
				break;
			}
		default:
			/* Allocate memory (include space for alignment) */
			psDeviceMemCB->ui64DeviceMemBase = ADDR_CxMalloc1((ADDR_sContext *)psMemSpaceCB->hMemAllocator,
																psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName,
																psDeviceMemCB->ui64Size, psDeviceMemCB->ui64Alignment);
			if (psDeviceMemCB->ui64DeviceMemBase == (IMG_UINT64)-1LL) return IMG_ERROR_MALLOC_FAILED;
			psDeviceMemCB->ui64DeviceMem = psDeviceMemCB->ui64DeviceMemBase;
			break;
		}
	}

	if (TAL_CheckMemSpaceEnable(hMemSpace) && TAL_MEMORY_ALLOCATION_CONTROL != TAL_DEVFLAG_DEV_ALLOC)
	{
		IMG_UINT64 ui64Remaining = psDeviceMemCB->ui64Size;
		ui64Offset = psDeviceMemCB->ui64DeviceMem;
		psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64Offset, &ui64Size);

		// Loop round running for each device covered by the malloc (memory can be divided between different devices)
		do
		{
			ui64Size = ui64Remaining;
			psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64Offset, &ui64Size);

			if (psDevifCB->pfnMallocMemory)
			{
				psDevifCB->pfnMallocMemory(psDevifCB, TAL_MEM_TWIDDLE(ui64Offset, psMemSpaceCB->psMemSpaceInfo->psDevInfo->eTALTwiddle),
											IMG_UINT64_TO_UINT32(ui64Size));
			}
			ui64Remaining -= ui64Size;
			ui64Offset = psDeviceMemCB->ui64DeviceMem + psDeviceMemCB->ui64Size - ui64Remaining;
		} while (ui64Remaining > 0);
	}

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stTarget;
		
		stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stTarget.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
		stTarget.ui64Value = 0;
		stTarget.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
		stTarget.eMemSpT = PDUMP_MEMSPACE_MEMORY;

		PDUMP_CMD_Malloc(stTarget, IMG_UINT64_TO_UINT32(ui64Size), IMG_UINT64_TO_UINT32(ui64Alignment), psDeviceMemCB->pszMemBlockName);
    }

    /* Add device memory control block to the list */
    {
#if defined(__TAL_USE_OSA__)
        OSA_CritSectLock(psMemSpaceCB->hDevMemListMutex );
#endif
#if defined(__TAL_USE_MEOS__)
        KRN_IPL_T   OldIPL;
        OldIPL = KRN_raiseIPL();
#endif
		
        psDeviceMemCB->psPrevDeviceMemCB = IMG_NULL;
        psDeviceMemCB->psNextDeviceMemCB = IMG_NULL;
        if (psMemSpaceCB->psDevMemList == IMG_NULL)
        {
            psMemSpaceCB->psDevMemList = psDeviceMemCB;
        }
        else
        {
            psMemSpaceCB->psDevMemList->psPrevDeviceMemCB = psDeviceMemCB;
            psDeviceMemCB->psNextDeviceMemCB = psMemSpaceCB->psDevMemList;
        }
        psMemSpaceCB->psDevMemList = psDeviceMemCB;
#if defined(__TAL_USE_OSA__)
        OSA_CritSectUnlock(psMemSpaceCB->hDevMemListMutex );
#endif
#if defined(__TAL_USE_MEOS__)
        KRN_restoreIPL(OldIPL);
#endif

    }

    /* Return the address of the device memory control block as the handle */
    *phDeviceMem = (IMG_HANDLE)psDeviceMemCB;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    /* If we are to update the target... (as this calls another TAL function, it must   */
    /* be performed outside the threadsafe section of the calling TAL function).        */
    if (bUpdateDevice)
    {
        /* Update the target from the host */
        return TALMEM_UpdateDevice((IMG_HANDLE) psDeviceMemCB);
    }

    return rResult;
}


/*!
******************************************************************************

 @Function              TALMEM_Free

******************************************************************************/
IMG_RESULT TALMEM_Free(
    IMG_HANDLE *                     phDeviceMem
)
{
    TAL_psDeviceMemCB		psDeviceMemCB = (TAL_psDeviceMemCB)*phDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
    TAL_sPdumpContext *     psPdumpContext;
	DEVIF_psDeviceCB		psDevifCB;
	IMG_UINT64 				ui64DeviceMem, ui64Size;
	
	/* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT( psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);
	IMG_ASSERT(*phDeviceMem != (IMG_HANDLE)IMG_NULL);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
	{
		PDUMP_CMD_sTarget stTarget;

		stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stTarget.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
		stTarget.ui64Value = 0;
		stTarget.ui64BlockAddr = 0;
		stTarget.eMemSpT = PDUMP_MEMSPACE_MEMORY;

		PDUMP_CMD_Free(stTarget);
    }

	/* If this is not a mapped call...*/
	if (psDeviceMemCB->bMappedMem != IMG_TRUE)
	{
		switch (TAL_MEMORY_ALLOCATION_CONTROL)
		{
		case TAL_DEVFLAG_DEV_ALLOC:
			ui64DeviceMem = psDeviceMemCB->ui64DeviceMem;
			psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64DeviceMem, &ui64Size);
			IMG_ASSERT(psDevifCB->pfnFreeMemory); // Ensure the required function exists
			psDevifCB->pfnFreeMemory(psDevifCB, psDeviceMemCB->ui64DeviceMemBase);
			break;
		case TAL_DEVFLAG_4K_PAGED_ALLOC:
		case TAL_DEVFLAG_4K_PAGED_RANDOM_ALLOC:
			/* Free memory page */
			PAGEMEM_CxFreeMemoryPage((PAGEMEM_sContext *)psMemSpaceCB->hMemAllocator, IMG_UINT64_TO_UINT32(psDeviceMemCB->ui64DeviceMemBase));
			break;
		default:
			// Free memory block
			ADDR_CxFree((ADDR_sContext *)psMemSpaceCB->hMemAllocator, psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName,
								psDeviceMemCB->ui64DeviceMemBase);
			break;
		}
	}

	// Run the frees on the device if available
	if (TAL_CheckMemSpaceEnable(psMemSpaceCB) && TAL_MEMORY_ALLOCATION_CONTROL != TAL_DEVFLAG_DEV_ALLOC )
	{
		IMG_UINT64 ui64Remaining = psDeviceMemCB->ui64Size;
		ui64DeviceMem = psDeviceMemCB->ui64DeviceMem;
		// Loop round running for each device covered by the malloc (memory can be divided between different devices)
		do
		{
			ui64Size = ui64Remaining;
			psDevifCB = talmem_GetDevice(psMemSpaceCB, &ui64DeviceMem, &ui64Size);
			/* If the device interface has a "free" function...*/
			if (psDevifCB->pfnFreeMemory != IMG_NULL)
			{
					psDevifCB->pfnFreeMemory(psDevifCB, TAL_MEM_TWIDDLE(ui64DeviceMem, psMemSpaceCB->psMemSpaceInfo->psDevInfo->eTALTwiddle));
			}
			ui64Remaining -= ui64Size;
			ui64DeviceMem = psDeviceMemCB->ui64DeviceMem + psDeviceMemCB->ui64Size - ui64Remaining;
		}while (ui64Remaining > 0);
	}


    /* Remove device memory control block from the lists */
    {
		TAL_DeviceList *psDeviceListIter, *psPrevDevItem;
#if defined(__TAL_USE_OSA__)
        OSA_CritSectLock(psMemSpaceCB->hDevMemListMutex );
#endif
#if defined(__TAL_USE_MEOS__)
        KRN_IPL_T   OldIPL;
        OldIPL = KRN_raiseIPL();
#endif

		// Remove block from recently used list
		psDeviceListIter = gpsDeviceList;
		psPrevDevItem = IMG_NULL;
		while (psDeviceListIter)
		{
			// If the freed block is in this list then remove it
			if (psDeviceMemCB == psDeviceListIter->psDeviceMemCB)
			{
				if (psPrevDevItem == IMG_NULL)
				{
					gpsDeviceList = psDeviceListIter->pdlNextDevice;
				}
				else
				{
					psPrevDevItem->pdlNextDevice = psDeviceListIter->pdlNextDevice;
				}
				psDeviceListIter->psDeviceMemCB = IMG_NULL;
				break;
			}
			psPrevDevItem = psDeviceListIter;
			psDeviceListIter = psDeviceListIter->pdlNextDevice;
		}

		// Remove block from main list
        if (psDeviceMemCB->psPrevDeviceMemCB == IMG_NULL)
        {
			psMemSpaceCB->psDevMemList = psDeviceMemCB->psNextDeviceMemCB;
        }
        else
        {
            psDeviceMemCB->psPrevDeviceMemCB->psNextDeviceMemCB = psDeviceMemCB->psNextDeviceMemCB;
        }
        if (psDeviceMemCB->psNextDeviceMemCB != IMG_NULL)
        {
            psDeviceMemCB->psNextDeviceMemCB->psPrevDeviceMemCB = psDeviceMemCB->psPrevDeviceMemCB;
        }
#if defined(__TAL_USE_OSA__)
        OSA_CritSectUnlock(psMemSpaceCB->hDevMemListMutex );
#endif
#if defined(__TAL_USE_MEOS__)
        KRN_restoreIPL(OldIPL);
#endif
    }

    /* Free control block and clear the address pointered to by the handle */
    IMG_FREE(psDeviceMemCB->pszMemBlockName);
	IMG_FREE(psDeviceMemCB);
    *phDeviceMem = (IMG_HANDLE)IMG_NULL;

    /* Update the no. of device memory CBs allocated */
    IMG_ASSERT(psMemSpaceCB->ui32NoMemCBs > 0);
    psMemSpaceCB->ui32NoMemCBs--;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}


/*!
******************************************************************************

 @Function              talmem_FlushLoadBuffer

******************************************************************************/
IMG_RESULT talmem_FlushLoadBuffer(TAL_sPdumpContext *     psPdumpContext)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *		psMemSpaceCB;
	TAL_psMemCoalesce		psMemInfo;

	IMG_ASSERT(psPdumpContext != IMG_NULL);
	psMemInfo = &psPdumpContext->sLoadCoalesce;
	psMemSpaceCB = (TAL_sMemSpaceCB *)psMemInfo->hMemSpace;

	// A size of 0 indicates that the buffer is empty
	if (psMemInfo->ui64Size == 0) return IMG_SUCCESS;

	talmem_UpdateDevice(psMemSpaceCB, psMemInfo->pui8HostMem, psMemInfo->ui64StartAddr, 0, psMemInfo->ui64Size);

	// If we need to check the copy, read and test the last word
	if (psMemSpaceCB->psMemSpaceInfo->psDevInfo->bVerifyMemWrites)
	{
		TAL_psDeviceMemCB psDeviceMemCB;
		IMG_UINT32 ui32LastWord = *(IMG_PUINT32)(psMemInfo->pui8HostMem + psMemInfo->ui64Size - 4);
		IMG_UINT32 ui32DevCopy;
		IMG_UINT64 ui64Offset = psMemInfo->ui64StartAddr + psMemInfo->ui64Size - 4; // Offset is current device address
		psDeviceMemCB = talmem_LocateBlock(ui64Offset);
		ui64Offset -= psDeviceMemCB->ui64DeviceMem;						// Take away base address of block to get block offset
		talmem_ReadWord(psDeviceMemCB, ui64Offset, &ui32DevCopy, TAL_WORD_FLAGS_32BIT);
		TAL_ASSERT(ui32DevCopy == ui32LastWord, "Memory Check Failed, memory copies not performing correctly");
	}

	IMG_FREE(psPdumpContext->sLoadCoalesce.pui8HostMem);
	IMG_MEMSET(&psPdumpContext->sLoadCoalesce, 0, sizeof(TAL_sMemCoalesce));
	return rResult;
}

/*!
******************************************************************************

 @Function              talmem_CopyToFromHost

******************************************************************************/
IMG_RESULT talmem_CopyToFromHost(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                      ui64Size,
	IMG_BOOL						bToDev
)
{
    TAL_psDeviceMemCB		psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	IMG_BOOL				bDataCopied = IMG_FALSE;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	IMG_ASSERT(psDeviceMemCB != IMG_NULL);
	// A size of 0 indicates that the whole block should be updated
	if (ui64Size == 0)	ui64Size = psDeviceMemCB->ui64Size;
    /* Ensure that the region is within the block */
    TAL_ASSERT((ui64Offset+ui64Size) <= psDeviceMemCB->ui64Size, "Given offset outside of allocated block");
	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT( psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);
	
	if (bToDev)
	{
		/* If we are not inside a loop or on the first iteration */
		if (talpdump_Check(psPdumpContext))
		{
			PDUMP_CMD_sTarget stDst;
			
			stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stDst.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
			stDst.ui64Value = ui64Offset;
			stDst.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stDst.eMemSpT = PDUMP_MEMSPACE_MEMORY;
			
			PDUMP_CMD_LoadBytes(stDst, (IMG_PUINT32)(psDeviceMemCB->pHostMem + ui64Offset), IMG_UINT64_TO_UINT32(ui64Size));
		}

		if (TAL_CheckMemSpaceEnable(psMemSpaceCB))
		{
			if (psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_COALESCE_LOADS)
			{
				if (psPdumpContext->sLoadCoalesce.ui64Size == 0)
				{
					// If there is nothing in the coalesce buffer fill it in
					psPdumpContext->sLoadCoalesce.hMemSpace = psMemSpaceCB;
					psPdumpContext->sLoadCoalesce.pui8HostMem = (IMG_PUINT8)IMG_MALLOC((IMG_UINTPTR)ui64Size);
					psPdumpContext->sLoadCoalesce.ui64Size = ui64Size;
					psPdumpContext->sLoadCoalesce.ui64StartAddr = psDeviceMemCB->ui64DeviceMem + ui64Offset;
					IMG_MEMCPY(psPdumpContext->sLoadCoalesce.pui8HostMem, psDeviceMemCB->pHostMem + ui64Offset, (IMG_UINTPTR)ui64Size);
					bDataCopied = IMG_TRUE;
				}
				else if(psDeviceMemCB->ui64DeviceMem + ui64Offset == psPdumpContext->sLoadCoalesce.ui64StartAddr + ui64Size)
				{
					// If the new block fits onto the end of the last block fill it in
					if(psPdumpContext->sLoadCoalesce.hMemSpace != psMemSpaceCB)
						talmem_FlushLoadBuffer(psPdumpContext);

					psPdumpContext->sLoadCoalesce.pui8HostMem = (IMG_PUINT8)IMG_REALLOC(psPdumpContext->sLoadCoalesce.pui8HostMem, (IMG_UINTPTR)psPdumpContext->sLoadCoalesce.ui64Size + (IMG_UINTPTR)ui64Size);
					IMG_MEMCPY(psPdumpContext->sLoadCoalesce.pui8HostMem + psPdumpContext->sLoadCoalesce.ui64Size,
										psDeviceMemCB->pHostMem + ui64Offset, (IMG_UINTPTR)ui64Size);
					psPdumpContext->sLoadCoalesce.ui64Size += ui64Size;
					bDataCopied = IMG_TRUE;
				}
				else
				{
					talmem_FlushLoadBuffer(psPdumpContext);
				}
			}
			if (!bDataCopied)
			{
				talmem_UpdateDevice(psMemSpaceCB, (IMG_PUINT8)psDeviceMemCB->pHostMem, psDeviceMemCB->ui64DeviceMem, ui64Offset, ui64Size);
				// If we need to check the copy, read and test the last word
				if (psMemSpaceCB->psMemSpaceInfo->psDevInfo->bVerifyMemWrites)
				{
					IMG_UINT32 ui32LastWord = *(IMG_PUINT32)(psDeviceMemCB->pHostMem + ui64Offset + ui64Size - 4);
					IMG_UINT32 ui32DevCopy;
					talmem_ReadWord(psDeviceMemCB, ui64Offset + ui64Size - 4, &ui32DevCopy, TAL_WORD_FLAGS_32BIT);
					TAL_ASSERT(ui32DevCopy == ui32LastWord, "Memory Check Failed, memory copies not performing correctly");
				}
			}
		}
	}
	else
	{

		if (TAL_CheckMemSpaceEnable(psMemSpaceCB))
		{
			// Flush write coalesce buffer if it is in use
			talmem_FlushLoadBuffer(psPdumpContext);

			talmem_UpdateHost(psDeviceMemCB, ui64Offset, ui64Size);
			/* Block has been updated by the host */
			psDeviceMemCB->bBlockUpdatedToByHost = IMG_TRUE;
		}
		/* If we are not inside a loop or on the first iteration */
		if (talpdump_Check(psPdumpContext))
		{
			PDUMP_CMD_sTarget stSrc;
			
			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
			stSrc.ui64Value = ui64Offset;
			stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;
			
			PDUMP_CMD_SaveBytes(stSrc, (IMG_PUINT8)psDeviceMemCB->pHostMem+ui64Offset, IMG_UINT64_TO_UINT32(ui64Size));
		}
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function              talmem_WriteDeviceMemRef

******************************************************************************/
IMG_RESULT talmem_WriteDeviceMemRef(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_HANDLE                      hRefDeviceMem,
    IMG_UINT64                      ui64RefOffset,
	IMG_UINT32						ui32Flags
)
{
    TAL_psDeviceMemCB      psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
    TAL_psDeviceMemCB      psRefDeviceMemCB = (TAL_psDeviceMemCB)hRefDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_UINT64              ui64Value;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	TAL_sMemSpaceCB *		psRefMemSpaceCB = (TAL_sMemSpaceCB *)psRefDeviceMemCB->hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

#ifdef TAL_DEBUG_DEVIF
    printf("WriteDevMemRef off=0x%lx Roff=0x%lx\n", ui64Offset, ui64RefOffset);
#endif
	IMG_ASSERT(psDeviceMemCB != IMG_NULL);
	IMG_ASSERT(psRefDeviceMemCB != IMG_NULL);
	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT( psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);
    /* Range check offset into */
    TAL_ASSERT ((ui64Offset+4) <= psDeviceMemCB->ui64Size, "Given offset outside of allocated block");

    /* Calculate memory address */
    ui64Value = talmem_GetDeviceMemRef(hRefDeviceMem, ui64RefOffset);

	// Write the word to memory
	rResult = talmem_WriteWord(psDeviceMemCB, ui64Offset, ui64Value, ui32Flags);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);
	
    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stSrc, stDst;
        
		stSrc.hMemSpace = psRefMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = psRefDeviceMemCB->ui32MemBlockId;
		stSrc.ui64Value = ui64RefOffset;
		stSrc.ui64BlockAddr = psRefDeviceMemCB->ui64DeviceMem;
		stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;

		stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stDst.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
		stDst.ui64Value = ui64Offset;
		stDst.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
		stDst.eMemSpT = PDUMP_MEMSPACE_MEMORY;

		/* If this is not the "null" mamgler...*/
		if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			PDUMP_CMD_WriteWord64(stSrc, stDst, IMG_NULL, 0, ui64Value);
		else
			PDUMP_CMD_WriteWord32(stSrc, stDst, IMG_NULL, 0, ui64Value);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

 @Function				TALMEM_DumpImage

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
)
{
    TAL_psDeviceMemCB      psDeviceMemCB1 = (TAL_psDeviceMemCB)hDeviceMem1;
    TAL_psDeviceMemCB      psDeviceMemCB2 = (TAL_psDeviceMemCB)hDeviceMem2;
    TAL_psDeviceMemCB      psDeviceMemCB3 = (TAL_psDeviceMemCB)hDeviceMem3;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB1->hMemSpace;
	IMG_RESULT				rResult = IMG_SUCCESS;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	/* Check this type of access is valid for this memory space...*/
    IMG_ASSERT( psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);

    /* We now need to dump the data for the planes to the results file*/
	PDUMP_CMD_CheckDumpImage(psMemSpaceCB->hPdumpMemSpace, ui32Size1 + ui32Size2 + ui32Size3);

    /* Dump the first plane to the results file...*/
    IMG_ASSERT(ui32Size1 != 0);
	rResult = talmem_UpdateHost(psDeviceMemCB1, ui64Offset1, ui32Size1);

    /* If the size of the second plane is not 0 then dump the second plane to the results file...*/
    if (ui32Size2 != 0 && rResult == IMG_SUCCESS)
    {
		rResult = talmem_UpdateHost(psDeviceMemCB2, ui64Offset2, ui32Size2);
    }

    /* If the size of the third plane is not 0 then dump the third plane to the results file...*/
    if (ui32Size3 != 0 && rResult == IMG_SUCCESS)
    {
        IMG_ASSERT(ui32Size2 != 0);
		rResult = talmem_UpdateHost(psDeviceMemCB3, ui64Offset3, ui32Size3);
    }

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

	/* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sImage siImg1, siImg2, siImg3;

		siImg1.stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		siImg1.stTarget.ui32BlockId = psDeviceMemCB1->ui32MemBlockId;
		siImg1.stTarget.ui64Value = ui64Offset1;
		siImg1.stTarget.ui64BlockAddr = psDeviceMemCB1->ui64DeviceMem;
		siImg1.stTarget.eMemSpT = PDUMP_MEMSPACE_MEMORY;
		siImg1.pui8ImageData = (IMG_PUINT8)psDeviceMemCB1->pHostMem + ui64Offset1;
		siImg1.ui32Size = ui32Size1;

		siImg2.stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		siImg2.stTarget.ui32BlockId = 0;
		siImg2.stTarget.ui64Value = ui64Offset2;
		siImg2.stTarget.ui64BlockAddr = 0;
		siImg2.stTarget.eMemSpT = PDUMP_MEMSPACE_MEMORY;
		siImg2.pui8ImageData = IMG_NULL;
		siImg2.ui32Size = ui32Size2;

		siImg3.stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		siImg3.stTarget.ui32BlockId = 0;
		siImg3.stTarget.ui64Value = ui64Offset3;
		siImg3.stTarget.ui64BlockAddr = 0;
		siImg3.stTarget.eMemSpT = PDUMP_MEMSPACE_MEMORY;
		siImg3.pui8ImageData = IMG_NULL;
		siImg3.ui32Size = ui32Size3;

		if (psDeviceMemCB2)
		{
			siImg2.stTarget.ui32BlockId = psDeviceMemCB2->ui32MemBlockId;
			siImg2.stTarget.ui64BlockAddr = psDeviceMemCB2->ui64DeviceMem;
			siImg2.pui8ImageData = (IMG_PUINT8)psDeviceMemCB2->pHostMem + ui64Offset2;
		}
		if (psDeviceMemCB3)
		{
			siImg3.stTarget.ui32BlockId = psDeviceMemCB3->ui32MemBlockId;
			siImg3.stTarget.ui64BlockAddr = psDeviceMemCB3->ui64DeviceMem;
			siImg3.pui8ImageData = (IMG_PUINT8)psDeviceMemCB3->pHostMem + ui64Offset3;
		}

		PDUMP_CMD_DumpImage(psImageFilename, siImg1, siImg2, siImg3, psImageHeaderInfo->ui32PixelFormat, psImageHeaderInfo->ui32Width,
							psImageHeaderInfo->ui32Height, psImageHeaderInfo->ui32Stride, psImageHeaderInfo->ui32AddrMode);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    IMG_ASSERT(rResult == IMG_SUCCESS);
    return rResult;
}

/*!
******************************************************************************

 @Function              TALMEM_ReadFromAddress

******************************************************************************/
IMG_RESULT TALMEM_ReadFromAddress(
    IMG_UINT64              ui64MemoryAddress,
    IMG_UINT32              ui32Size,
    IMG_UINT8 *             pui8Buffer
)
{
    IMG_RESULT				rResult = IMG_SUCCESS;
	TAL_psDeviceMemCB		psPageDeviceMemCB;
    IMG_UINT64				ui64OffsetWithinBlock;
    
    /* MMU will call this function with a NULL buffer on page faults */
    if (pui8Buffer == IMG_NULL)
        return IMG_SUCCESS;

    /* Locate the device memory control block containing the page...*/
    psPageDeviceMemCB = talmem_LocateBlock(ui64MemoryAddress);
	if (psPageDeviceMemCB == IMG_NULL)
		return IMG_ERROR_INVALID_PARAMETERS;

	// and the offset within that block
    ui64OffsetWithinBlock = ui64MemoryAddress - psPageDeviceMemCB->ui64DeviceMem;

	if (ui32Size == 4)
	{
		talmem_ReadWord(psPageDeviceMemCB, ui64OffsetWithinBlock, (IMG_VOID*)pui8Buffer, TAL_WORD_FLAGS_32BIT);
	}
	else if (ui32Size == 8)
	{
		talmem_ReadWord(psPageDeviceMemCB, ui64OffsetWithinBlock, (IMG_VOID*)pui8Buffer, TAL_WORD_FLAGS_64BIT);
	}
	else
	{
		// Change the hostmem over to be the buffer from this function
		IMG_CHAR * pTempHostMem = psPageDeviceMemCB->pHostMem;
		psPageDeviceMemCB->pHostMem = (IMG_CHAR *)pui8Buffer;
		// Move the device address to the start of the memory we want to copy so there is no offset
		psPageDeviceMemCB->ui64DeviceMem += ui64OffsetWithinBlock;

		rResult = talmem_UpdateHost(psPageDeviceMemCB, 0, ui32Size);

		// Restore the memory control block
		psPageDeviceMemCB->pHostMem = pTempHostMem;
		psPageDeviceMemCB->ui64DeviceMem -= ui64OffsetWithinBlock;
	}


    return rResult;
}

/*!
******************************************************************************
##############################################################################
 Category:              TAL Virtual Memory Functions
##############################################################################
******************************************************************************/

// Converts simple headers into more general functions using inline
IMG_RESULT TALVMEM_Poll(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALVMEM_CircBufPoll(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT64						ui64WriteOffsetVal,
	IMG_UINT64						ui64PacketSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALVMEM_ReadWord(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_PVOID						puiValue,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALVMEM_WriteWord(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
    IMG_UINT64                      ui64Value,
	IMG_HANDLE						hContextMemspace,
	IMG_UINT32						ui32Flags
);
inline IMG_RESULT TALVMEM_WriteWord32(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId, IMG_UINT32 ui32Value)
{
	return TALVMEM_WriteWord(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,ui32Value,IMG_NULL,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALVMEM_WriteWord64(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId, IMG_UINT64 ui64Value)
{
	return TALVMEM_WriteWord(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,ui64Value,IMG_NULL,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALVMEM_ReadWord32(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId, IMG_UINT32* pui32Value)
{
	return TALVMEM_ReadWord(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,pui32Value,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALVMEM_ReadWord64(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId, IMG_UINT64* pui64Value)
{
	return TALVMEM_ReadWord(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,pui64Value,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALVMEM_CircBufPoll32(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId, IMG_UINT64 ui64WriteOffsetVal,
										IMG_UINT64 ui64PacketSize, IMG_UINT64 ui64BufferSize, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALVMEM_CircBufPoll(hMemSpace,ui64DevVirtAddr, ui32MmuContextId,ui64WriteOffsetVal,ui64PacketSize,ui64BufferSize,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALVMEM_CircBufPoll64(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId, IMG_UINT64 ui64WriteOffsetVal,
										IMG_UINT64 ui64PacketSize, IMG_UINT64 ui64BufferSize, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALVMEM_CircBufPoll(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,ui64WriteOffsetVal,ui64PacketSize,ui64BufferSize,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALVMEM_Poll32(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId, IMG_UINT32 ui32CheckFuncId,
								 IMG_UINT32 ui32RequValue, IMG_UINT32 ui32Enable, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALVMEM_Poll(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,ui32CheckFuncId,ui32RequValue,ui32Enable,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALVMEM_Poll64(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId, IMG_UINT32 ui32CheckFuncId,
								 IMG_UINT64 ui64RequValue, IMG_UINT64 ui64Enable, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32TimeOut)
{
	return TALVMEM_Poll(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,ui32CheckFuncId,ui64RequValue,ui64Enable,ui32PollCount,ui32TimeOut,TAL_WORD_FLAGS_64BIT);
}
/*!
******************************************************************************

 @Function				talvmem_GetPageAndOffset

******************************************************************************/
static IMG_RESULT talvmem_GetPageAndOffset(
    IMG_HANDLE              hMemSpace,
    IMG_UINT64              ui64DevVirtAddr,
    IMG_UINT32              ui32MmuContextId,
    IMG_HANDLE *            phDeviceMem,
    IMG_UINT64 *            pui64Offset
)
{
    TAL_psDeviceMemCB      psPageDeviceMemCB;
    IMG_UINT64				ui64PhysAddress;
    IMG_UINT64				ui64OffsetWithinPage;
    IMG_RESULT              returnValue;

    /* Can only be used with page table...*/

    TAL_ASSERT(ui32MmuContextId < MMU_NUMBER_OF_CONTEXTS, "MMU Context Id exceeds limit");
	(void)hMemSpace;

    returnValue = MMU_VmemToPhysMem(ui64DevVirtAddr, &ui64PhysAddress, ui32MmuContextId);
    TAL_ASSERT ( returnValue == IMG_SUCCESS, "Cannot decode virtual address 0x%" IMG_I64PR "X", ui64DevVirtAddr);

	/* Locate the device memory control block containing the page...*/
	psPageDeviceMemCB = talmem_LocateBlock(ui64PhysAddress);
	TAL_ASSERT(psPageDeviceMemCB != IMG_NULL, "No Allocated Block at address 0x%" IMG_I64PR "X, decoded from Virtual Address 0x%" IMG_I64PR "X", ui64PhysAddress, ui64DevVirtAddr);

	ui64OffsetWithinPage = ui64PhysAddress - psPageDeviceMemCB->ui64DeviceMem;

    /* Return the handle of the page control block and the offset within the page...*/
    *phDeviceMem = psPageDeviceMemCB;
    *pui64Offset = ui64OffsetWithinPage;
	if (returnValue != IMG_SUCCESS)
		return returnValue;
	if (psPageDeviceMemCB == IMG_NULL)
		return IMG_ERROR_INVALID_PARAMETERS;
	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				TALVMEM_SetContext

******************************************************************************/

IMG_RESULT TALVMEM_SetContext(
	IMG_HANDLE				hMemSpace,
    IMG_UINT32              ui32MmuContextId,
    IMG_UINT32              ui32MmuType,
    IMG_HANDLE              hDeviceMem
)
{
    TAL_psDeviceMemCB      psDeviceMemCB;
    TAL_sPdumpContext *     psPdumpContext;
	IMG_RESULT				rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(hMemSpace);
	TAL_ASSERT(ui32MmuContextId < MMU_NUMBER_OF_CONTEXTS, "MMU Context Id exceeds limit");

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    // This must be memory for it to be a valid memory space
    IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);

    // Check this section is not in if, but still run if stubbed out
	if (psMemSpaceCB->psPdumpContext->ui32Disable == 0)
	{
		/* Set MMU context parameters...*/
		if (ui32MmuType == 0)
		{
			IMG_ASSERT(hDeviceMem == IMG_NULL);
			MMU_Clear(ui32MmuContextId);
		}
		else
		{
			IMG_ASSERT(hDeviceMem  != IMG_NULL);
			psDeviceMemCB = (TAL_psDeviceMemCB) hDeviceMem;
			// Otherwise we need to read the host memory copies
			rResult = MMU_Setup( ui32MmuContextId, psDeviceMemCB->ui64DeviceMem, ui32MmuType, TALMEM_ReadFromAddress );
		}
	}


	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    /* If we are not inside a loop or on the first iteration */
    if ( talpdump_Check(psPdumpContext) )
	{
		PDUMP_CMD_sTarget stTarget, stPageTable;

		stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stTarget.ui32BlockId = ui32MmuContextId;
		stTarget.ui64Value = 0;
		stTarget.ui64BlockAddr = 0;
		stTarget.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;

		stPageTable.hMemSpace = 0;
		stPageTable.ui32BlockId = 0;
		stPageTable.ui64Value = 0;
		stPageTable.ui64BlockAddr = 0;
		stPageTable.eMemSpT = PDUMP_MEMSPACE_MEMORY;

		psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
		if (psDeviceMemCB != IMG_NULL)
		{
			TAL_sMemSpaceCB *psDevMemSpaceCB;

			psDevMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
			stPageTable.hMemSpace = psDevMemSpaceCB->hPdumpMemSpace;
			stPageTable.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
		}
		PDUMP_CMD_SetMMUContext(stTarget, stPageTable, ui32MmuType);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function				TALVMEM_SetTiledRegion

******************************************************************************/

IMG_RESULT TALVMEM_SetTiledRegion(
	IMG_HANDLE				hMemSpace,
    IMG_UINT32              ui32MmuContextId,
    IMG_UINT32              ui32TiledRegionNo,
	IMG_UINT64				ui64DevVirtAddr,
    IMG_UINT64              ui64Size,
    IMG_UINT32              ui32XTileStride
)
{
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT (psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);

    // Check this section is not in if, but still run if stubbed out
	if (psMemSpaceCB->psPdumpContext->ui32Disable == 0)
	{
		MMU_SetTiledRegion(ui32MmuContextId, ui32TiledRegionNo,
							ui64DevVirtAddr, ui64Size, ui32XTileStride);
	}

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    /* If we are not inside a loop or on the first iteration */
    if ( talpdump_Check(psPdumpContext) )
	{
		/* Pdump Output */
		PDUMP_CMD_sTarget stTarget;

		stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stTarget.ui32BlockId = ui32MmuContextId;
		stTarget.ui64Value = ui64DevVirtAddr;
		stTarget.ui64BlockAddr = 0;
		stTarget.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;

		PDUMP_CMD_TiledRegion(stTarget, ui32TiledRegionNo, ui64Size, ui32XTileStride);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return IMG_SUCCESS;
}



/*!
******************************************************************************

 @Function				TALVMEM_ReadWord

******************************************************************************/
IMG_RESULT TALVMEM_ReadWord(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_PVOID						puiValue,
	IMG_UINT32						ui32Flags
)
{
	IMG_RESULT				rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB;
	TAL_psDeviceMemCB		psDeviceMemCB;
	IMG_UINT64              ui64Offset;

	TAL_THREADSAFE_INIT;

    TAL_ASSERT((ui64DevVirtAddr & 0x3) == 0, "TALVMEM_ReadWord - Virtual Address not word aligned");
    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    /* Get device memory handle and offset from device virtual address and page table directory ...*/
    rResult = talvmem_GetPageAndOffset(hMemSpace, ui64DevVirtAddr, ui32MmuContextId, (IMG_HANDLE*)&psDeviceMemCB, &ui64Offset);
	if (rResult != IMG_SUCCESS) return rResult;


	psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	IMG_ASSERT(psMemSpaceCB);


    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_LOCK;

	IMG_ASSERT (psDeviceMemCB != IMG_NULL);
    /* Ensure that the region is within the block */
	if(ui32Flags & TAL_WORD_FLAGS_32BIT)
	{
		IMG_ASSERT((ui64Offset+4) <= psDeviceMemCB->ui64Size);
	}
	else if(ui32Flags & TAL_WORD_FLAGS_64BIT)
	{
		IMG_ASSERT((ui64Offset+8) <= psDeviceMemCB->ui64Size);
	}
	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (TAL_CheckMemSpaceEnable(hMemSpace))
		rResult = talmem_ReadWord(psDeviceMemCB, ui64Offset, puiValue, ui32Flags);
	
	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stSrc, stDst;

		stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = ui32MmuContextId;
		stSrc.ui64Value = ui64DevVirtAddr;
		stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
		stSrc.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;

		stDst.hMemSpace = NULL;
		stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
		stDst.ui64Value = 0;
		stDst.ui64BlockAddr = 0;
		stDst.eMemSpT = PDUMP_MEMSPACE_VALUE;

		if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			PDUMP_CMD_ReadWord64(stSrc, stDst, IMG_FALSE);
		else
			PDUMP_CMD_ReadWord32(stSrc, stDst, IMG_FALSE);
	}
    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;

}


/*!
******************************************************************************

 @Function              TALVMEM_WriteWord

******************************************************************************/
IMG_RESULT TALVMEM_WriteWord(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
    IMG_UINT64                      ui64Value,
	IMG_HANDLE						hContextMemspace,
	IMG_UINT32						ui32Flags
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB;
	TAL_psDeviceMemCB		psDeviceMemCB;
	TAL_sMemSpaceCB *		psContextMemSpaceCB = (TAL_sMemSpaceCB *)hContextMemspace;
	IMG_UINT64              ui64Offset;

	TAL_THREADSAFE_INIT;

	IMG_ASSERT((ui64DevVirtAddr & 0x3) == 0);
    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    /* Get device memory handle and offset from device virtual address and page table directory ...*/
    rResult = talvmem_GetPageAndOffset(hMemSpace, ui64DevVirtAddr, ui32MmuContextId, (IMG_HANDLE*)&psDeviceMemCB, &ui64Offset);
	if (rResult != IMG_SUCCESS) return rResult;


	IMG_ASSERT(gbInitialised);
	psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	IMG_ASSERT(psMemSpaceCB);

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psDeviceMemCB != IMG_NULL);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	rResult = talmem_WriteWord(psDeviceMemCB, ui64Offset, ui64Value, ui32Flags);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stSrc, stDst;
		
		stSrc.hMemSpace = psContextMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
		stSrc.ui64Value = ui64Value;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_VALUE;

		stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stDst.ui32BlockId = ui32MmuContextId;
		stDst.ui64Value = ui64DevVirtAddr;
		stDst.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
		stDst.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;

		if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			PDUMP_CMD_WriteWord64(stSrc, stDst, NULL, 0, 0);
		else
			PDUMP_CMD_WriteWord32(stSrc, stDst, NULL, 0, 0);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}



/*!
******************************************************************************

 @Function				TALVMEM_Poll

******************************************************************************/
IMG_RESULT TALVMEM_Poll(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut,
	IMG_UINT32						ui32Flags
)
{
	IMG_UINT64              ui64Offset, ui64ReadVal;
	IMG_RESULT				rResult = IMG_SUCCESS;
	TAL_psDeviceMemCB		psDeviceMemCB;
	IMG_BOOL				bPass = IMG_FALSE;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB;
	IMG_UINT32				ui32Count = 0;

    IMG_ASSERT((ui64DevVirtAddr & 0x3) == 0);
    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    /* Get device memory handle and offset from device virtual address and page table directory ...*/
    rResult = talvmem_GetPageAndOffset(hMemSpace, ui64DevVirtAddr, ui32MmuContextId, (IMG_HANDLE*)&psDeviceMemCB, &ui64Offset);
	if (rResult != IMG_SUCCESS) return rResult;


	IMG_ASSERT(gbInitialised);
	psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		/* Test the value */
		for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
		{
			TAL_THREADSAFE_INIT;
			TAL_THREADSAFE_LOCK;

			rResult = talmem_ReadWord(psDeviceMemCB, ui64Offset, &ui64ReadVal, ui32Flags);
			if (rResult == IMG_SUCCESS)
				bPass = TAL_TestWord(ui64ReadVal, ui64RequValue, ui64Enable, 0, ui32CheckFuncIdExt);

			TAL_THREADSAFE_UNLOCK;

			// Check result
			if (IMG_SUCCESS != rResult)break;

			// Check whether we meet the exit criteria
			if (!bPass)
				tal_Idle(hMemSpace, ui32TimeOut);
			else
				break;
		}
		if (ui32Count >= ui32PollCount)	rResult = IMG_ERROR_TIMEOUT;
	}

	{
		TAL_THREADSAFE_INIT;
		TAL_THREADSAFE_LOCK;

		/* If we are not inside a loop or on the first iteration */
		if (talpdump_Check(psPdumpContext))
		{
			PDUMP_CMD_sTarget stSrc;
			
			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
			stSrc.ui64Value = ui64Offset;
			stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;
			
			if (ui32Flags & TAL_WORD_FLAGS_32BIT)
				PDUMP_CMD_Poll32(stSrc, IMG_UINT64_TO_UINT32(ui64RequValue), IMG_UINT64_TO_UINT32(ui64Enable), ui32PollCount, ui32TimeOut, ui32CheckFuncIdExt & TAL_CHECKFUNC_MASK, IMG_NULL);
			else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
				PDUMP_CMD_Poll64(stSrc, ui64RequValue, ui64Enable, ui32PollCount, ui32TimeOut, ui32CheckFuncIdExt & TAL_CHECKFUNC_MASK, IMG_NULL);
		}
		TAL_THREADSAFE_UNLOCK;
	}
	return rResult;
}

/*!
******************************************************************************

 @Function              TALVMEM_CircBufPoll

******************************************************************************/
IMG_RESULT TALVMEM_CircBufPoll(
    IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT64						ui64WriteOffsetVal,
	IMG_UINT64						ui64PacketSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32PollCount,
	IMG_UINT32						ui32TimeOut,
	IMG_UINT32						ui32Flags
)
{
	TAL_psDeviceMemCB		psDeviceMemCB;
	TAL_sMemSpaceCB *		psMemSpaceCB;
	TAL_sPdumpContext *		psPdumpContext;
	IMG_UINT64              ui64Offset, ui64ReadVal;
	IMG_RESULT				rResult = IMG_SUCCESS;
	IMG_UINT32				ui32Count;
	IMG_BOOL				bPass = IMG_FALSE;

    IMG_ASSERT((ui64DevVirtAddr & 0x3) == 0);
    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    /* Get device memory handle and offset from device virtual address and page table directory ...*/
    rResult = talvmem_GetPageAndOffset(hMemSpace, ui64DevVirtAddr, ui32MmuContextId, (IMG_HANDLE*)&psDeviceMemCB, &ui64Offset);
	if (rResult != IMG_SUCCESS) return rResult;

	 /* Ensure that the region is within the block */
	IMG_ASSERT(psDeviceMemCB);
    TAL_ASSERT((ui64Offset+4) <= psDeviceMemCB->ui64Size, "Given offset outside of allocated block");
	psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
		{
			TAL_THREADSAFE_INIT;
			TAL_THREADSAFE_LOCK;

			rResult = talmem_ReadWord(psDeviceMemCB, ui64Offset, &ui64ReadVal, ui32Flags);
			if (rResult == IMG_SUCCESS)
				bPass = TAL_TestWord(ui64ReadVal, ui64WriteOffsetVal, ui64PacketSize, ui64BufferSize, TAL_CHECKFUNC_CBP);

			/* Allow API to be re-entered */
			TAL_THREADSAFE_UNLOCK;

			if (IMG_SUCCESS != rResult) break;
			// Check whether we meet the exit criteria
			if(!bPass)
			{
				tal_Idle(hMemSpace, ui32TimeOut);
			}
			else
				break;
		}
		if (ui32Count >= ui32PollCount)	rResult = IMG_ERROR_TIMEOUT;
	}

	{
		TAL_THREADSAFE_INIT;
		TAL_THREADSAFE_LOCK;

		// Pdump
		if (talpdump_Check(psPdumpContext))
		{
			PDUMP_CMD_sTarget stSrc;
			
			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = ui32MmuContextId;
			stSrc.ui64Value = ui64Offset;
			stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stSrc.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;

			if (ui32Flags & TAL_WORD_FLAGS_32BIT)
				PDUMP_CMD_CircBufPoll32(stSrc, IMG_UINT64_TO_UINT32(ui64WriteOffsetVal), IMG_UINT64_TO_UINT32(ui64PacketSize), IMG_UINT64_TO_UINT32(ui64BufferSize), ui32PollCount, ui32TimeOut);
			else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
				PDUMP_CMD_CircBufPoll64(stSrc, ui64WriteOffsetVal, ui64PacketSize, ui64BufferSize, ui32PollCount, ui32TimeOut);
		}
		/* Allow API to be re-entered */
		TAL_THREADSAFE_UNLOCK;
	}
	return rResult;
 }

/*!
******************************************************************************

 @Function				TALVMEM_UpdateHost

******************************************************************************/
IMG_RESULT TALVMEM_UpdateHost(
	IMG_HANDLE						hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT64						ui64Size,
	IMG_VOID *						pvHostBuf
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT(psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);

    if (MMU_ContextsDefined())
    {
        // use TAL MMU context information
		if (TAL_CheckMemSpaceEnable(hMemSpace)
			&& (!(gui32TargetAppFlags & TALAPP_STUB_OUT_TARGET)) )
		{
			rResult = MMU_ReadVmem(ui64DevVirtAddr, ui64Size, (IMG_UINT32*)pvHostBuf, ui32MmuContextId);            
			switch (rResult)
			{
			case IMG_SUCCESS:
				break;
			case IMG_ERROR_INVALID_ID:
				TAL_ASSERT(IMG_FALSE, "ERROR: Invalid MMU Id %d\n", ui32MmuContextId);
				break;
			case IMG_ERROR_INVALID_PARAMETERS:
				TAL_ASSERT(IMG_FALSE, "ERROR: Unable to read page directory or page table entry using virtual address 0x%" IMG_I64PR "X\n", ui64DevVirtAddr);
				break;
			case IMG_ERROR_MMU_PAGE_TABLE_FAULT:
                if (psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_SKIP_PAGE_FAULTS)
                    printf("WARNING: Invalid page table entry for virtual address 0x%" IMG_I64PR "X\n", ui64DevVirtAddr);
                else
                    TAL_ASSERT(IMG_FALSE, "ERROR: Invalid page table entry for virtual address 0x%" IMG_I64PR "X\n", ui64DevVirtAddr);
				break;
			case IMG_ERROR_MMU_PAGE_DIRECTORY_FAULT:
				TAL_ASSERT(IMG_FALSE, "ERROR: Invalid page directory entry for virtual address 0x%" IMG_I64PR "X\n", ui64DevVirtAddr);
				break;
			case IMG_ERROR_MMU_PAGE_CATALOGUE_FAULT:
				TAL_ASSERT(IMG_FALSE, "ERROR: Invalid page catalogue entry for virtual address 0x%" IMG_I64PR "X\n", ui64DevVirtAddr);
				break;
			default:
				TAL_ASSERT(IMG_FALSE, "ERROR: Unable to read from virtual address 0x%" IMG_I64PR "X\n", ui64DevVirtAddr);
			}
		}
    }
    else
    {
    	if (TAL_CheckMemSpaceEnable(hMemSpace))
    	{
			SGXMMU_eSGXMMUType		eSGXMMUType = SGXMMU_TYPE_SGX530;
			switch(TAL_SGX_VIRTUAL_MEMORY_CONTROL)
			{
			case TAL_DEVFLAG_SGX535_VIRT_MEMORY:
				eSGXMMUType = SGXMMU_TYPE_SGX535;
			case TAL_DEVFLAG_SGX530_VIRT_MEMORY:
				/* Ensure thar the code is initialised...*/
				SGXMMU_Initialise();
				/*  work out which directory base address to use from the value of ui32DevVirtQualifier,
					and write this to the host field of the index register so that the subsequent
					host access will use this directory base address for its translation */
				SGXMMU_SetUpTranslation(psMemSpaceCB->psMemSpaceInfo, eSGXMMUType, ui32MmuContextId);

				{
					IMG_UINT32			ui32DevVirtAddr = IMG_UINT64_TO_UINT32(ui64DevVirtAddr);
					IMG_UINT32			ui32Size = IMG_UINT64_TO_UINT32(ui64Size);
					IMG_UINT32			ui32NumExtractedBytes = 0;
					IMG_UINT32			ui32NumOfBytesToExtract = 0x1000 - (ui32DevVirtAddr & 0x00000FFF);
					DEVIF_sDeviceCB *	psDevIfDeviceCB = &psMemSpaceCB->psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;

					IMG_UINT8 * pi8HostBuffer = (IMG_UINT8 *)pvHostBuf;

					while (ui32NumExtractedBytes < ui32Size)
					{
						/* Obtain the base device address for this page */
						IMG_UINT32	ui32DeviceAddress = SGXMMU_CoreTranslation(psMemSpaceCB->psMemSpaceInfo, eSGXMMUType, ui32DevVirtAddr + ui32NumExtractedBytes);

						if ((ui32NumOfBytesToExtract + ui32NumExtractedBytes) > ui32Size)
						{
							ui32NumOfBytesToExtract = ui32Size - ui32NumExtractedBytes;
							IMG_ASSERT(ui32NumOfBytesToExtract < 0x1000);
						}

						/* Copy all the necessary bytes from this page */
						psDevIfDeviceCB->pfnCopyDeviceToHost(psDevIfDeviceCB, ui32DeviceAddress, pi8HostBuffer, ui32NumOfBytesToExtract);

						/* Update the address and no. of bytes extracted...*/
						pi8HostBuffer += ui32NumOfBytesToExtract;
						ui32NumExtractedBytes += ui32NumOfBytesToExtract;

						/* Work out the number of bytes to extract on the next iteration */
						ui32NumOfBytesToExtract = 0x1000 - ((ui32DevVirtAddr + ui32NumExtractedBytes) & 0x00000FFF);
					}
				}
				break;
			default:
				/* Not supported */
				TAL_ASSERT(IMG_FALSE,"MEMMSP_CopyDeviceVirtToHostRegion - missing MMU setup information\n");
				break;
			}
    	}
    }

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stSrc;

		stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = ui32MmuContextId;
		stSrc.ui64Value = ui64DevVirtAddr;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;

		PDUMP_CMD_SaveBytes(stSrc, (IMG_PUINT8)pvHostBuf, IMG_UINT64_TO_UINT32(ui64Size));
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function				TALVMEM_DumpImage

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
)
{
    TAL_sPdumpContext *     psPdumpContext;
	IMG_BOOL				bPrevState;
	IMG_RESULT				rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);


    /* Check this type of access is valid for this memory space...*/
    IMG_ASSERT(psMemSpaceCB->psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_MEMORY);

    /* Check if the file needs to be split...*/
	PDUMP_CMD_CheckDumpImage(psMemSpaceCB->hPdumpMemSpace, IMG_UINT64_TO_UINT32(ui64Size1+ui64Size2+ui64Size3));

	/* Disable Pdumping while we do the page by page transfer */
	PDUMP_CMD_CaptureMemSpaceEnable(psMemSpaceCB->hPdumpMemSpace, IMG_FALSE, &bPrevState);

	rResult = TALVMEM_UpdateHost(hMemSpace, ui64DevVirtAddr1, ui32MmuContextId1, ui64Size1, pvHostBuf1);

	/* If the size of the second plane is not 0 then dump the second plane to the results file...*/
	if (ui64Size2 != 0 && rResult == IMG_SUCCESS)
	{
		rResult = TALVMEM_UpdateHost(hMemSpace, ui64DevVirtAddr2, ui32MmuContextId2, ui64Size2, pvHostBuf2);
	}

	/* If the size of the third plane is not 0 then dump the third plane to the results file...*/
	if (ui64Size3 != 0 && rResult == IMG_SUCCESS)
	{
		IMG_ASSERT(ui64Size2 != 0);
		rResult = TALVMEM_UpdateHost(hMemSpace, ui64DevVirtAddr3, ui32MmuContextId3, ui64Size3, pvHostBuf3);
	}

	PDUMP_CMD_CaptureMemSpaceEnable(psMemSpaceCB->hPdumpMemSpace, bPrevState, IMG_NULL);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If we are not inside a loop or on the first iteration */
    if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sImage siImg1, siImg2, siImg3;

		siImg1.stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		siImg1.stTarget.ui32BlockId = ui32MmuContextId1;
		siImg1.stTarget.ui64Value = ui64DevVirtAddr1;
		siImg1.stTarget.ui64BlockAddr = 0;
		siImg1.stTarget.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;
		siImg1.pui8ImageData = (IMG_PUINT8)pvHostBuf1;
		siImg1.ui32Size = IMG_UINT64_TO_UINT32(ui64Size1);

		siImg2.stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		siImg2.stTarget.ui32BlockId = ui32MmuContextId2;
		siImg2.stTarget.ui64Value = ui64DevVirtAddr2;
		siImg2.stTarget.ui64BlockAddr = 0;
		siImg2.stTarget.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;
		siImg2.pui8ImageData = (IMG_PUINT8)pvHostBuf2;
		siImg2.ui32Size = IMG_UINT64_TO_UINT32(ui64Size2);

		siImg3.stTarget.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		siImg3.stTarget.ui32BlockId = ui32MmuContextId3;
		siImg3.stTarget.ui64Value = ui64DevVirtAddr3;
		siImg3.stTarget.ui64BlockAddr = 0;
		siImg3.stTarget.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;
		siImg3.pui8ImageData = (IMG_PUINT8)pvHostBuf3;
		siImg3.ui32Size = IMG_UINT64_TO_UINT32(ui64Size3);

		PDUMP_CMD_DumpImage(psImageFilename, siImg1, siImg2, siImg3, psImageHeaderInfo->ui32PixelFormat, psImageHeaderInfo->ui32Width,
                            psImageHeaderInfo->ui32Height, psImageHeaderInfo->ui32Stride, psImageHeaderInfo->ui32AddrMode);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return  rResult;
}


/*!
******************************************************************************
##############################################################################
 Category:              TAL Memory Space Functions
##############################################################################
******************************************************************************/

 /*!
******************************************************************************

 @Function				talpdump_GetMemspace

******************************************************************************/
static
IMG_HANDLE talpdump_GetMemspace(IMG_HANDLE hMemSpace)
{
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	if (hMemSpace == NULL)
	{
		return NULL;
	}
	else
	{
		return psMemSpaceCB->hPdumpMemSpace;
	}
}


/*!
******************************************************************************
##############################################################################
 Category:          TAL Internal Variable Functions
##############################################################################
******************************************************************************/

IMG_RESULT TALINTVAR_ReadFromMem (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
    IMG_BOOL                        bVirtAddr,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALINTVAR_WriteToMem (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
    IMG_BOOL                        bVirtAddr,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALINTVAR_WriteToVirtMem (
	IMG_HANDLE						hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
	IMG_UINT32						ui32Flags
);
IMG_RESULT TALINTVAR_ReadFromVirtMem (
	IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
	IMG_UINT32						ui32Flags
);
IMG_RESULT talintvar_WriteMemRef (
    IMG_HANDLE                      hRefDeviceMem,
    IMG_UINT64                      ui64RefOffset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
    IMG_BOOL                        bPollVirtAddr,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId
);
IMG_RESULT talintvar_WriteToReg	(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
	IMG_UINT32						ui32Flags
);
IMG_RESULT talintvar_ReadFromReg(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
	IMG_UINT32						ui32Flags
);
inline IMG_RESULT TALINTVAR_ReadFromReg32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return talintvar_ReadFromReg(hMemSpace,ui32Offset,hIntVarMemSpace,ui32InternalVarId,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALINTVAR_ReadFromReg64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return talintvar_ReadFromReg(hMemSpace,ui32Offset,hIntVarMemSpace,ui32InternalVarId,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALINTVAR_WriteToReg32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return talintvar_WriteToReg(hMemSpace,ui32Offset,hIntVarMemSpace,ui32InternalVarId,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALINTVAR_WriteToReg64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return talintvar_WriteToReg(hMemSpace,ui32Offset,hIntVarMemSpace,ui32InternalVarId,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALINTVAR_WriteToVirtMem32(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId,
											 IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return TALINTVAR_WriteToVirtMem(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,hIntVarMemSpace,ui32InternalVarId,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALINTVAR_WriteToVirtMem64(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId,
											 IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return TALINTVAR_WriteToVirtMem(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,hIntVarMemSpace,ui32InternalVarId,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALINTVAR_WriteToMem32(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return TALINTVAR_WriteToMem(hDeviceMem,ui64Offset,hIntVarMemSpace,ui32InternalVarId,IMG_FALSE,0,0,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALINTVAR_WriteToMem64(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return TALINTVAR_WriteToMem(hDeviceMem,ui64Offset,hIntVarMemSpace,ui32InternalVarId,IMG_FALSE,0,0,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALINTVAR_ReadFromMem32(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset,IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return TALINTVAR_ReadFromMem(hDeviceMem,ui64Offset,hIntVarMemSpace,ui32InternalVarId,IMG_FALSE,0,0,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALINTVAR_ReadFromMem64(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset,IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return TALINTVAR_ReadFromMem(hDeviceMem,ui64Offset,hIntVarMemSpace,ui32InternalVarId,IMG_FALSE,0,0,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALINTVAR_ReadFromVirtMem32(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId,
											  IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return TALINTVAR_ReadFromVirtMem(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,hIntVarMemSpace,ui32InternalVarId,TAL_WORD_FLAGS_32BIT);
}
inline IMG_RESULT TALINTVAR_ReadFromVirtMem64(IMG_HANDLE hMemSpace, IMG_UINT64 ui64DevVirtAddr, IMG_UINT32 ui32MmuContextId,
											  IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return TALINTVAR_ReadFromVirtMem(hMemSpace,ui64DevVirtAddr,ui32MmuContextId,hIntVarMemSpace,ui32InternalVarId,TAL_WORD_FLAGS_64BIT);
}
inline IMG_RESULT TALINTVAR_WriteMemRef(IMG_HANDLE hRefDeviceMem, IMG_UINT64 ui64RefOffset, IMG_HANDLE hIntVarMemSpace, IMG_UINT32 ui32InternalVarId)
{
	return talintvar_WriteMemRef(hRefDeviceMem,ui64RefOffset,hIntVarMemSpace,ui32InternalVarId,IMG_FALSE,0,0);
}


/*!
******************************************************************************

 @Function              talintvar_WriteToReg

******************************************************************************/
IMG_RESULT talintvar_WriteToReg	(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
	IMG_UINT32						ui32Flags
)
{
	IMG_RESULT				rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	TAL_sMemSpaceCB *		psIntVarMemSpaceCB = (TAL_sMemSpaceCB*)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

	/* Check the validity of the internal variable ID */
	TAL_ASSERT(ui32InternalVarId < TAL_NUMBER_OF_INTERNAL_VARS, "Internal Variable Id limit exceeded");
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    IMG_ASSERT(psIntVarMemSpaceCB != IMG_NULL);
	if (psIntVarMemSpaceCB == IMG_NULL) return IMG_ERROR_INVALID_PARAMETERS;

	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		rResult = talreg_WriteWord(psMemSpaceCB, ui32Offset, psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId], ui32Flags);
	}

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    /* If capture is enabled and it is not disabled for the internal register commands */
	if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_sTarget stSrc, stDst;
		
		stSrc.hMemSpace = psIntVarMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
		stSrc.ui64Value = ui32InternalVarId;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_INT_VAR;

		stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
		stDst.ui64Value = ui32Offset;
		stDst.ui64BlockAddr = 0;
		stDst.eMemSpT = PDUMP_MEMSPACE_REGISTER;

		if (ui32Flags & TAL_WORD_FLAGS_32BIT)
			PDUMP_CMD_WriteWord32(stSrc, stDst, IMG_NULL, 0, IMG_UINT64_TO_UINT32(psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId]));
		else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			PDUMP_CMD_WriteWord64(stSrc, stDst, IMG_NULL, 0, IMG_UINT64_TO_UINT32(psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId]));
		else
			IMG_ASSERT(IMG_FALSE);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

 @Function              talintvar_ReadFromReg

******************************************************************************/
IMG_RESULT talintvar_ReadFromReg(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
	IMG_UINT32						ui32Flags
)
{
    IMG_RESULT                  rResult = IMG_SUCCESS;
    TAL_sPdumpContext *         psPdumpContext;
	TAL_sMemSpaceCB *			psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	TAL_sMemSpaceCB *			psIntVarMemSpaceCB = (TAL_sMemSpaceCB*)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	/* Check the validity of the internal variable ID */
    TAL_ASSERT(ui32InternalVarId < TAL_NUMBER_OF_INTERNAL_VARS, "Internal Variable Id limit exceeded");
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	IMG_ASSERT(psIntVarMemSpaceCB != IMG_NULL);
    psPdumpContext = psMemSpaceCB->psPdumpContext;

	if (TAL_CheckMemSpaceEnable(hMemSpace))
	{
		//  This currently only works on little endian machines
		IMG_UINT64      ui64Value = 0;
		// Read from the device
		rResult = talreg_ReadWord(psMemSpaceCB, ui32Offset, &ui64Value, ui32Flags);
		psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId] = ui64Value;
	}


	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

	/* If capture is enabled and it is not disabled for the internal register commands */
	if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_sTarget stSrc, stDst;
		
		stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
		stSrc.ui64Value = ui32Offset;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_REGISTER;

		stDst.hMemSpace = psIntVarMemSpaceCB->hPdumpMemSpace;
		stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
		stDst.ui64Value = ui32InternalVarId;
		stDst.ui64BlockAddr = 0;
		stDst.eMemSpT = PDUMP_MEMSPACE_INT_VAR;

		if (ui32Flags & TAL_WORD_FLAGS_32BIT)
			PDUMP_CMD_ReadWord32(stSrc, stDst, IMG_FALSE);
		else if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			PDUMP_CMD_ReadWord64(stSrc, stDst, IMG_FALSE);
		else
			IMG_ASSERT(IMG_FALSE);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function				TALINTVAR_WriteToVirtMem

******************************************************************************/
 IMG_RESULT TALINTVAR_WriteToVirtMem (
	IMG_HANDLE						hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
	IMG_UINT32						ui32Flags
)
{
	IMG_HANDLE						hDeviceMem;
	IMG_UINT64                      ui64Offset;
	IMG_RESULT						rResult = IMG_SUCCESS;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    IMG_ASSERT((ui64DevVirtAddr & 0x3) == 0);

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	IMG_ASSERT(hMemSpace);

    /* Get device memory handle and offset from device virtual address and page table directory ...*/
    rResult = talvmem_GetPageAndOffset(hMemSpace, ui64DevVirtAddr, ui32MmuContextId, &hDeviceMem, &ui64Offset);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    if (rResult != IMG_SUCCESS) return rResult;

    /* Call on to normal write mem function...*/
    return TALINTVAR_WriteToMem (hDeviceMem, ui64Offset, hIntVarMemSpace, ui32InternalVarId, IMG_TRUE, ui64DevVirtAddr, ui32MmuContextId, ui32Flags);

}


/*!
******************************************************************************

 @Function				TALINTVAR_ReadFromVirtMem

******************************************************************************/

IMG_RESULT TALINTVAR_ReadFromVirtMem (
	IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
	IMG_UINT32						ui32Flags
)
{
	IMG_HANDLE						hDeviceMem;
	IMG_UINT64                      ui64Offset;
	IMG_RESULT						rResult = IMG_SUCCESS;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    IMG_ASSERT((ui64DevVirtAddr & 0x3) == 0);

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    /* Get device memory handle and offset from device virtual address and page table directory ...*/
    rResult = talvmem_GetPageAndOffset(hMemSpace, ui64DevVirtAddr, ui32MmuContextId, &hDeviceMem, &ui64Offset);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	if (rResult != IMG_SUCCESS) return rResult;

    /* Call on to normal function...*/
    return TALINTVAR_ReadFromMem(hDeviceMem, ui64Offset, hIntVarMemSpace, ui32InternalVarId,
                    IMG_TRUE, ui64DevVirtAddr, ui32MmuContextId, ui32Flags);

}


/*!
******************************************************************************

 @Function				TALINTVAR_WriteVirtMemReference

******************************************************************************/
 IMG_RESULT TALINTVAR_WriteVirtMemReference (
	IMG_HANDLE                      hMemSpace,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId
)
{
	IMG_HANDLE						hDeviceMem;
	IMG_UINT64                      ui64Offset;
	IMG_RESULT						rResult = IMG_SUCCESS;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    IMG_ASSERT((ui64DevVirtAddr & 0x3) == 0);

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    /* Get device memory handle and offset from device virtual address and page table directory ...*/
    rResult = talvmem_GetPageAndOffset(hMemSpace, ui64DevVirtAddr, ui32MmuContextId, &hDeviceMem, &ui64Offset);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	if (rResult != IMG_SUCCESS) return rResult;

    return talintvar_WriteMemRef (hDeviceMem, ui64Offset, hIntVarMemSpace, ui32InternalVarId,
                    IMG_TRUE, ui64DevVirtAddr, ui32MmuContextId);

}

/*!
******************************************************************************

 @Function              TALINTVAR_WriteToMem

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToMem (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
    IMG_BOOL                        bVirtAddr,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT32						ui32Flags
)
{
    TAL_psDeviceMemCB		psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
	IMG_RESULT				rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	TAL_sMemSpaceCB *		psIntVarMemSpaceCB = (TAL_sMemSpaceCB *)hIntVarMemSpace;

	/* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	 /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	/* Check the validity of the internal register ID */
	TAL_ASSERT(ui32InternalVarId < TAL_NUMBER_OF_INTERNAL_VARS, "Internal variable Id exceeds limit");
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);
	IMG_ASSERT(psIntVarMemSpaceCB != IMG_NULL);
	IMG_ASSERT(psDeviceMemCB != IMG_NULL);


	// Write the word to memory
	rResult = talmem_WriteWord(psDeviceMemCB, ui64Offset, psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId], ui32Flags);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

    /* If capture is enabled and it is not disabled for the internal register commands */
	if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stSrc, stDst;
        
		stSrc.hMemSpace = psIntVarMemSpaceCB->hPdumpMemSpace;
		stSrc.ui32BlockId = TAL_NO_MEM_BLOCK;
		stSrc.ui64Value = ui32InternalVarId;
		stSrc.ui64BlockAddr = 0;
		stSrc.eMemSpT = PDUMP_MEMSPACE_INT_VAR;
		
		if (!bVirtAddr)
        {
			stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stDst.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
			stDst.ui64Value = ui64Offset;
			stDst.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stDst.eMemSpT = PDUMP_MEMSPACE_MEMORY;
        }
        else
        {
			IMG_ASSERT(ui32MmuContextId != TAL_DEV_VIRT_QUALIFIER_NONE);

			stDst.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stDst.ui32BlockId = ui32MmuContextId;
			stDst.ui64Value = ui64DevVirtAddr;
			stDst.ui64BlockAddr = 0;
			stDst.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;
		}

		if (ui32Flags & TAL_WORD_FLAGS_64BIT)
			PDUMP_CMD_WriteWord64(stSrc, stDst, IMG_NULL, 0, psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId]);
		else
			PDUMP_CMD_WriteWord32(stSrc, stDst, IMG_NULL, 0, IMG_UINT64_TO_UINT32(psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId]));
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function				talintvar_WriteMemRef

******************************************************************************/
IMG_RESULT talintvar_WriteMemRef (
    IMG_HANDLE                      hRefDeviceMem,
    IMG_UINT64                      ui64RefOffset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
    IMG_BOOL                        bPollVirtAddr,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId
)
{
    TAL_psDeviceMemCB		psRefDeviceMemCB = (TAL_psDeviceMemCB)hRefDeviceMem;
    IMG_UINT64              ui64Value;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB;
	TAL_sMemSpaceCB *		psRegMemSpaceCB = (TAL_sMemSpaceCB *)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

	/* Check the validity of the internal register ID */
	TAL_ASSERT(ui32InternalVarId < TAL_NUMBER_OF_INTERNAL_VARS, "Given Internal Variable Id exceeds limit");
	IMG_ASSERT (psRefDeviceMemCB != IMG_NULL);
	psMemSpaceCB = (TAL_sMemSpaceCB *)psRefDeviceMemCB->hMemSpace;
	IMG_ASSERT(psMemSpaceCB != IMG_NULL);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

    /* Calculate memory address */
    ui64Value = talmem_GetDeviceMemRef(hRefDeviceMem, ui64RefOffset);

    // Check this section is not in if, but still run if stubbed out
	if (psMemSpaceCB->psPdumpContext->ui32Disable == 0)
	{
		psRegMemSpaceCB->aui64InternalVariables[ui32InternalVarId] = ui64Value;
	}

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

	/* If capture is enabled and it is not disabled for the internal register commands */
	if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_sTarget stSrc, stDst;

		stDst.hMemSpace = psRegMemSpaceCB->hPdumpMemSpace;
		stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
		stDst.ui64Value = ui32InternalVarId;
		stDst.ui64BlockAddr = 0;
		stDst.eMemSpT = PDUMP_MEMSPACE_INT_VAR;

        if (!bPollVirtAddr)
        {
			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = psRefDeviceMemCB->ui32MemBlockId;
			stSrc.ui64Value = ui64RefOffset;
			stSrc.ui64BlockAddr = psRefDeviceMemCB->ui64DeviceMem;
			stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;
        }
        else
        {
			IMG_ASSERT(ui32MmuContextId != TAL_DEV_VIRT_QUALIFIER_NONE);

			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = ui32MmuContextId;
			stSrc.ui64Value = ui64DevVirtAddr;
			stSrc.ui64BlockAddr = 0;
			stSrc.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;
		}

		PDUMP_CMD_WriteWord32(stSrc, stDst, IMG_NULL, 0, ui64Value);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function              TALINTVAR_ReadFromMem

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromMem (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
	IMG_HANDLE						hIntVarMemSpace,
	IMG_UINT32						ui32InternalVarId,
    IMG_BOOL                        bVirtAddr,
	IMG_UINT64						ui64DevVirtAddr,
	IMG_UINT32						ui32MmuContextId,
	IMG_UINT32						ui32Flags
)
{
    TAL_psDeviceMemCB		psDeviceMemCB = (TAL_psDeviceMemCB)hDeviceMem;
	IMG_RESULT				rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)psDeviceMemCB->hMemSpace;
	TAL_sMemSpaceCB *		psIntVarMemSpaceCB = (TAL_sMemSpaceCB *)hIntVarMemSpace;

	/* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);
	/* Check the validity of the internal register ID */
	TAL_ASSERT(ui32InternalVarId < TAL_NUMBER_OF_INTERNAL_VARS, "Given Internal Variable Id exceeds limit");
	IMG_ASSERT(psMemSpaceCB);

	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	/* Check internal register memory space Id */
    IMG_ASSERT(psIntVarMemSpaceCB != IMG_NULL);
    IMG_ASSERT (psDeviceMemCB != IMG_NULL);

	if (TAL_CheckMemSpaceEnable(psMemSpaceCB))
		psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId] = 0;
		rResult = talmem_ReadWord(psDeviceMemCB, ui64Offset, &psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId], ui32Flags);

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psMemSpaceCB);

	/* If capture is enabled and it is not disabled for the internal register commands */
	if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stSrc, stDst;
		
		stDst.hMemSpace = psIntVarMemSpaceCB->hPdumpMemSpace;
		stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
		stDst.ui64Value = ui32InternalVarId;
		stDst.ui64BlockAddr = 0;
		stDst.eMemSpT = PDUMP_MEMSPACE_INT_VAR;
		
		if (!bVirtAddr)
        {
			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = psDeviceMemCB->ui32MemBlockId;
			stSrc.ui64Value = ui64Offset;
			stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stSrc.eMemSpT = PDUMP_MEMSPACE_MEMORY;
		}
		else
		{
			stSrc.hMemSpace = psMemSpaceCB->hPdumpMemSpace;
			stSrc.ui32BlockId = ui32MmuContextId;
			stSrc.ui64Value = ui64DevVirtAddr;
			stSrc.ui64BlockAddr = psDeviceMemCB->ui64DeviceMem;
			stSrc.eMemSpT = PDUMP_MEMSPACE_VIRTUAL;
		}
		PDUMP_CMD_ReadWord32(stSrc, stDst, IMG_FALSE);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

/*!
******************************************************************************

 @Function				TALINTVAR_RunCommand

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
)
{
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psDestMemSpaceCB = (TAL_sMemSpaceCB *)hDestRegMemSpace;
	TAL_sMemSpaceCB *		psOpMemSpaceCB = (TAL_sMemSpaceCB *)hOpRegMemSpace;
	TAL_sMemSpaceCB *		psLastMemSpaceCB = (TAL_sMemSpaceCB *)hLastOpMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

	TAL_ASSERT(ui32DestRegId < TAL_NUMBER_OF_INTERNAL_VARS, "Internal Variable Id exceeds limit");
	TAL_ASSERT(ui32OpRegId < TAL_NUMBER_OF_INTERNAL_VARS, "Internal Variable Id exceeds limit");
	IMG_ASSERT(psDestMemSpaceCB);
	psPdumpContext = psDestMemSpaceCB->psPdumpContext;

    // Check this section is not in if, but still run if stubbed out
	if (psDestMemSpaceCB->psPdumpContext->ui32Disable == 0)
	{
		TALINTVAR_ExecCommand(ui32CommandId, hDestRegMemSpace, ui32DestRegId, hOpRegMemSpace,
				ui32OpRegId, hLastOpMemSpace, ui64LastOperand, bIsRegisterLastOperand);
	}

	if (psPdumpContext->bInsideLoop)
		tal_LoopNormalCommand(psDestMemSpaceCB);

    /* If capture is enabled and it is not disabled for the internal register commands */
	if (talpdump_Check(psPdumpContext))
    {
		PDUMP_CMD_sTarget stDst, stOpreg1, stOpreg2;

		stDst.hMemSpace = psDestMemSpaceCB->hPdumpMemSpace;
		stDst.ui32BlockId = TAL_NO_MEM_BLOCK;
		stDst.ui64Value = ui32DestRegId;
		stDst.ui64BlockAddr = 0;
		stDst.eMemSpT = PDUMP_MEMSPACE_INT_VAR;

		stOpreg1.hMemSpace = NULL;
		stOpreg1.ui32BlockId = TAL_NO_MEM_BLOCK;
		stOpreg1.ui64Value = ui32OpRegId;
		stOpreg1.ui64BlockAddr = 0;
		stOpreg1.eMemSpT = PDUMP_MEMSPACE_INT_VAR;

		if (ui32CommandId != TAL_PDUMPL_INTREG_MOV)
		{
			IMG_ASSERT(psOpMemSpaceCB);
			stOpreg1.hMemSpace = psOpMemSpaceCB->hPdumpMemSpace;
		}

		stOpreg2.hMemSpace = NULL;
		stOpreg2.ui32BlockId = TAL_NO_MEM_BLOCK;
		stOpreg2.ui64Value = ui64LastOperand;
		stOpreg2.ui64BlockAddr = 0;
		stOpreg2.eMemSpT = PDUMP_MEMSPACE_INT_VAR;

		if (!bIsRegisterLastOperand)
		{
			stOpreg2.eMemSpT = PDUMP_MEMSPACE_VALUE;
		}
		else
		{
			IMG_ASSERT(psLastMemSpaceCB);
			stOpreg2.hMemSpace = psLastMemSpaceCB->hPdumpMemSpace;
		}
		
		PDUMP_CMD_RegisterCommand(ui32CommandId, stDst, stOpreg1, stOpreg2);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

	return IMG_SUCCESS;
}


/*!
******************************************************************************
##############################################################################
 Category:              TAL Pdump Functions
##############################################################################
******************************************************************************/

IMG_HANDLE talpdump_GetMemspace(IMG_HANDLE hMemSpace);

// Most Pdump Functions are passed on to the Pdump Commands Interface
inline IMG_VOID TALPDUMP_SetPdump1MemName(IMG_CHAR* pszMemName)
{
	PDUMP_CMD_SetPdump1MemName(pszMemName);
}
inline IMG_VOID TALPDUMP_Comment(IMG_HANDLE hMemSpace, const IMG_CHAR* pszCommentText)
{
	PDUMP_CMD_PdumpComment(talpdump_GetMemspace(hMemSpace), pszCommentText);
}
IMG_VOID TALPDUMP_MiscOutput(IMG_HANDLE hMemSpace, IMG_CHAR* pOutputText)
{
	PDUMP_CMD_PdumpMiscOutput(talpdump_GetMemspace(hMemSpace),pOutputText);
}
inline IMG_VOID TALPDUMP_ConsoleMessage(IMG_HANDLE hMemSpace, IMG_CHAR* psCommentText)
{
	PDUMP_CMD_PdumpConsoleMessage(talpdump_GetMemspace(hMemSpace),psCommentText);
}
inline IMG_VOID TALPDUMP_SetConversionSourceData(IMG_UINT32 ui32SourceFileID, IMG_UINT32 ui32SourceLineNum)
{
	PDUMP_CMD_SetPdumpConversionSourceData(ui32SourceFileID, ui32SourceLineNum);
}
inline IMG_VOID TALPDUMP_MemSpceCaptureEnable(IMG_HANDLE hMemSpace, IMG_BOOL bEnable, IMG_BOOL* pbPrevState)
{
	PDUMP_CMD_CaptureMemSpaceEnable(talpdump_GetMemspace(hMemSpace),bEnable,pbPrevState);
}
inline IMG_VOID TALPDUMP_DisableCmds(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32 ui32DisableFlags)
{
	PDUMP_CMD_CaptureDisableCmds(talpdump_GetMemspace(hMemSpace),ui32Offset,ui32DisableFlags);
}
inline IMG_VOID TALPDUMP_SetFlags(IMG_UINT32 ui32Flags)
{
	PDUMP_CMD_SetFlags(ui32Flags);
}
inline IMG_VOID TALPDUMP_ClearFlags(IMG_UINT32 ui32Flags)
{
	PDUMP_CMD_ClearFlags(ui32Flags);
}
inline IMG_UINT32 TALPDUMP_GetFlags()
{
	return PDUMP_GetFlags();
}
inline IMG_VOID TALPDUMP_SetFilename(IMG_HANDLE hMemSpace, TAL_eSetFilename eSetFilename, IMG_CHAR* psFileName)
{
	PDUMP_CMD_CaptureSetFilename(talpdump_GetMemspace(hMemSpace),eSetFilename,psFileName);
}
inline IMG_CHAR* TALPDUMP_GetFilename(IMG_HANDLE hMemSpace, TAL_eSetFilename eSetFilename)
{
	return PDUMP_CMD_CaptureGetFilename(talpdump_GetMemspace(hMemSpace),eSetFilename);
}
inline IMG_VOID TALPDUMP_SetFileoffset(IMG_HANDLE hMemSpace, TAL_eSetFilename eSetFilename, IMG_UINT64 ui64FileOffset)
{
	PDUMP_CMD_CaptureSetFileoffset(talpdump_GetMemspace(hMemSpace),eSetFilename,IMG_UINT64_TO_UINT32(ui64FileOffset));
}
inline IMG_VOID TALPDUMP_EnableContextCapture(IMG_HANDLE hMemSpaceCB)
{
	TAL_psMemSpaceCB psMemSpace = (TAL_psMemSpaceCB)hMemSpaceCB;
	PDUMP_CMD_CaptureEnablePdumpContext(psMemSpace->psPdumpContext->hPdumpCmdContext);
}
inline IMG_VOID TALPDUMP_DisableContextCapture (IMG_HANDLE hMemSpaceCB)
{
	TAL_psMemSpaceCB psMemSpace = (TAL_psMemSpaceCB)hMemSpaceCB;
	PDUMP_CMD_CaptureDisablePdumpContext(psMemSpace->psPdumpContext->hPdumpCmdContext);
}
inline IMG_VOID TALPDUMP_AddSyncToTDF(IMG_HANDLE hMemSpace, IMG_UINT32 ui32SyncId)
{
	PDUMP_CMD_AddSyncToTDF(ui32SyncId, talpdump_GetMemspace(hMemSpace));
}
inline IMG_VOID TALPDUMP_AddCommentToTDF(IMG_CHAR * pcComment)
{
	PDUMP_CMD_AddCommentToTDF(pcComment);
}
inline IMG_VOID TALPDUMP_AddTargetConfigToTDF(IMG_CHAR * pszFilePath)
{
	PDUMP_CMD_AddTargetConfigToTDF(pszFilePath);
}
inline IMG_VOID TALPDUMP_GenerateTDF(IMG_CHAR * psFilename)
{
	PDUMP_CMD_GenerateTDF(psFilename);
}
inline IMG_VOID TALPDUMP_CaptureStart(IMG_CHAR* psTestDirectory)
{
	PDUMP_CMD_CaptureStart(psTestDirectory);
}
inline IMG_VOID TALPDUMP_CaptureStop()
{
	PDUMP_CMD_CaptureStop();
}
inline IMG_VOID TALPDUMP_ChangeResFileName(IMG_HANDLE hMemSpace, IMG_CHAR* sFileName)
{
	PDUMP_CMD_SetResPrmFileName(talpdump_GetMemspace(hMemSpace),0,sFileName,"out2_");
}
inline IMG_VOID TALPDUMP_ChangePrmFileName(IMG_HANDLE hMemSpace, IMG_CHAR* sFileName)
{
	PDUMP_CMD_SetResPrmFileName(talpdump_GetMemspace(hMemSpace),1,sFileName,"out2_");
}
inline IMG_VOID TALPDUMP_ResetResFileName(IMG_HANDLE hMemSpace)
{
	PDUMP_CMD_ResetResPrmFileName(talpdump_GetMemspace(hMemSpace),0);
}
inline IMG_VOID TALPDUMP_ResetPrmFileName(IMG_HANDLE hMemSpace)
{
	PDUMP_CMD_ResetResPrmFileName(talpdump_GetMemspace(hMemSpace),1);
}
inline IMG_BOOL TALPDUMP_IsCaptureEnabled()
{
	return PDUMP_CMD_IsCaptureEnabled();
}
inline IMG_VOID TALPDUMP_Sync(IMG_HANDLE hMemSpace)
{
	TALPDUMP_SyncWithId(hMemSpace, TAL_NO_SYNC_ID);
}
inline IMG_VOID TALPDUMP_Barrier(IMG_VOID)
{
	TALPDUMP_BarrierWithId(TAL_NO_SYNC_ID);
}

/*!
******************************************************************************

 @Function              talpdump_InitContext

******************************************************************************/
static IMG_VOID talpdump_InitContext(
    IMG_CHAR *                  pszContextName,
    TAL_sPdumpContext *         psPdumpContext
)
{
    if (pszContextName == IMG_NULL)
    {
        strcpy (psPdumpContext->pszContextName, "");
    }
    else
    {
        strcpy(psPdumpContext->pszContextName, pszContextName);
    }
    psPdumpContext->ui32LoopNestingDepth = 0;
    psPdumpContext->bInsideLoop      = IMG_FALSE;
    psPdumpContext->psCurLoopContext = IMG_NULL;
}

/*!
******************************************************************************

 @Function              talpdump_DeinitContext

******************************************************************************/
static IMG_VOID talpdump_DeinitContext(
    TAL_sPdumpContext *         psPdumpContext
    )
{
    strcpy(psPdumpContext->pszContextName, "");
}

/*!
******************************************************************************

 @Function              TALPDUMP_SyncWithId

******************************************************************************/
 IMG_RESULT TALPDUMP_SyncWithId(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SyncId
)
{
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	IMG_RESULT				rResult = IMG_SUCCESS;

    // Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	TAL_ASSERT(TAL_MAX_SYNC_ID >= ui32SyncId, "Sync Id exceeds limit");

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    if ( hMemSpace )
    {
        /* Check memory space Id */
		IMG_ASSERT(psMemSpaceCB);
		psPdumpContext = psMemSpaceCB->psPdumpContext;
		IMG_ASSERT(psPdumpContext);

		if (TAL_CheckMemSpaceEnable(hMemSpace))
		{
			// a real sync could be implemented here for live runs
		}

	}
    else
    {
        /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
    }

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    /* If we are not inside a loop or on the first iteration */
    if ( talpdump_Check(psPdumpContext) )
    {
		IMG_HANDLE hPdumpMemSpace = psMemSpaceCB ? psMemSpaceCB->hPdumpMemSpace : NULL;
		PDUMP_CMD_Sync(hPdumpMemSpace, ui32SyncId);
    }

    // Allow API to be re-entered
    TAL_THREADSAFE_UNLOCK;

	return rResult;
}

IMG_VOID TALPDUMP_AddBarrierToTDF(IMG_UINT32 ui32SyncId)
{
	TAL_sPdumpContext *     psPdumpContext = NULL;
	IMG_UINT32 i;

    // Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	TAL_ASSERT(TAL_MAX_SYNC_ID >= ui32SyncId, "Sync Id exceeds limit");

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

	for ( i = 0 ; i < gui32NoPdumpContexts ; i++ )
	{
		TAL_sMemSpaceCB *		psMemSpaceCB = NULL;
		IMG_HANDLE			hMemSpItr = NULL;

		psPdumpContext = &gasPdumpContexts[i];

		if ( psPdumpContext->bPdumpDisable == IMG_TRUE )
		{
			continue; // next
		}

		// find one memspace that uses this context
		psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hMemSpItr);
		while(psMemSpaceCB)
		{
			if ( psMemSpaceCB->psPdumpContext == psPdumpContext )
			{
				break;
			}
			psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetNext(hMemSpItr);
		}
		if ( psMemSpaceCB == NULL ) // no memspace found for this pdump context
		{
			continue; // try next pdump context
		}

		PDUMP_CMD_AddSyncToTDF(ui32SyncId, psMemSpaceCB->hPdumpMemSpace);
	}

	// Allow API to be re-entered
    TAL_THREADSAFE_UNLOCK;
}

IMG_VOID TALPDUMP_BarrierWithId(IMG_UINT32 ui32SyncId)
{
    TAL_sPdumpContext *     psPdumpContext = NULL;
	IMG_UINT32 i;

    // Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	TAL_ASSERT(TAL_MAX_SYNC_ID >= ui32SyncId, "Sync Id exceeds limit");

	/* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    for ( i = 0 ; i < gui32NoPdumpContexts ; i++ )
	{
		TAL_sMemSpaceCB *		psMemSpaceCB = NULL;
		IMG_HANDLE			hMemSpItr = NULL;

		psPdumpContext = &gasPdumpContexts[i];

		if ( psPdumpContext->bPdumpDisable == IMG_TRUE )
		{
			continue; // next
		}

		// find one memspace that uses this context
		psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetFirst(TALITR_TYP_MEMSPACE, &hMemSpItr);
		while(psMemSpaceCB)
		{
			if ( psMemSpaceCB->psPdumpContext == psPdumpContext )
			{
				break;
			}
			psMemSpaceCB = (TAL_sMemSpaceCB *)TALITERATOR_GetNext(hMemSpItr);
		}
		if ( psMemSpaceCB == NULL ) // no memspace found for this pdump context
		{
			continue; // try next pdump context
		}

		if (psPdumpContext->bInsideLoop)
		{
			tal_LoopNormalCommand(psMemSpaceCB);
		}

		/* If we are not inside a loop or on the first iteration */
		if ( talpdump_Check(psPdumpContext) )
		{
			IMG_HANDLE hPdumpMemSpace = psMemSpaceCB ? psMemSpaceCB->hPdumpMemSpace : NULL;
			PDUMP_CMD_Sync(hPdumpMemSpace, ui32SyncId);
		}
	}

    // Allow API to be re-entered
    TAL_THREADSAFE_UNLOCK;
}

/*!
******************************************************************************

 @Function				TALPDUMP_GetSemaphoreId

******************************************************************************/
IMG_UINT32 TALPDUMP_GetSemaphoreId(
	IMG_HANDLE						hSrcMemSpace,
	IMG_HANDLE						hDestMemSpace
)
{
	IMG_UINT32 ui32SemaphoreId;

	// Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	IMG_ASSERT(hSrcMemSpace || hDestMemSpace);

    // Look for existing semaphores
	for (ui32SemaphoreId = 0; ui32SemaphoreId < TAL_MAX_SEMA_ID; ui32SemaphoreId++)
	{
		// If destination memspace is null it is a free semaphore
		if (gasSemaphoreIds[ui32SemaphoreId].hDestMemSpace == IMG_NULL)
			break;
		// Check to see if the semaphores match
		if (gasSemaphoreIds[ui32SemaphoreId].hSrcMemSpace == hSrcMemSpace
			&& gasSemaphoreIds[ui32SemaphoreId].hDestMemSpace == hDestMemSpace)
			break;
	}

	TAL_ASSERT(ui32SemaphoreId < TAL_MAX_SEMA_ID, "ERROR: Insufficient TAL Semaphores, change TAL_MAX_SEMA_ID\n");

	gasSemaphoreIds[ui32SemaphoreId].hSrcMemSpace = hSrcMemSpace;
	gasSemaphoreIds[ui32SemaphoreId].hDestMemSpace = hDestMemSpace;

    // Allow API to be re-entered
    TAL_THREADSAFE_UNLOCK;

	return ui32SemaphoreId;
}

/*!
******************************************************************************

 @Function              TALPDUMP_Lock

******************************************************************************/
 IMG_VOID TALPDUMP_Lock(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SemaId
)
{
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    // Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	TAL_ASSERT(ui32SemaId < TAL_MAX_SEMA_ID, "ERROR: Insufficient TAL Semaphores, change TAL_MAX_SEMA_ID\n");

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    if ( hMemSpace )
    {
        /* Check memory space Id */
		IMG_ASSERT(psMemSpaceCB);
		psPdumpContext = psMemSpaceCB->psPdumpContext;
		IMG_ASSERT(psPdumpContext);

		if (TAL_CheckMemSpaceEnable(hMemSpace))
		{
/*#if !defined (__TAL_NO_OS__)
			// sanity check for lock/unlock pairs
			// if decode locks here, pdump will also lock
			KRN_testSemaphore( &asLockSem[ui32SemaId], 1, KRN_INFWAIT );
#endif*/
		}

	}
    else
    {
        /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
    }

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    /* If we are not inside a loop or on the first iteration */
    if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_Lock(psMemSpaceCB->hPdumpMemSpace, ui32SemaId);
    }

    // Allow API to be re-entered
    TAL_THREADSAFE_UNLOCK;
}

/*!
******************************************************************************

 @Function              TALPDUMP_Unlock

******************************************************************************/
IMG_VOID TALPDUMP_Unlock(
	IMG_HANDLE						hMemSpace,
	IMG_UINT32						ui32SemaId
)
{
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    // Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    TAL_ASSERT(ui32SemaId < TAL_MAX_SEMA_ID, "ERROR: Insufficient TAL Semaphores, change TAL_MAX_SEMA_ID\n");

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    if ( hMemSpace )
    {
        /* Check memory space Id */
		IMG_ASSERT(psMemSpaceCB);
		psPdumpContext = psMemSpaceCB->psPdumpContext;
		IMG_ASSERT(psPdumpContext);

		if (TAL_CheckMemSpaceEnable(hMemSpace))
		{
/*#if !defined (__TAL_NO_OS__)
				KRN_setSemaphore( &asLockSem[ui32SemaId], 1 );
#endif*/
		}
	}
    else
    {
        /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
    }

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_Unlock(psMemSpaceCB->hPdumpMemSpace, ui32SemaId);
    }

    // Allow API to be re-entered
    TAL_THREADSAFE_UNLOCK;
}


/*!
******************************************************************************

 @Function				TALPDUMP_SetDefineStrings

******************************************************************************/
IMG_VOID TALPDUMP_SetDefineStrings(
    TAL_Strings *  psStrings
)
{
	gpsIfStrings = psStrings;
}

/*!
******************************************************************************

 @Function				TALPDUMP_If

******************************************************************************/
IMG_VOID TALPDUMP_If(
    IMG_HANDLE						hMemSpace,
	IMG_CHAR*						pszDefine
)
{
	TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	IMG_INT32 i;
	IMG_ASSERT( hMemSpace != IMG_NULL );
    TAL_THREADSAFE_INIT;

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (psPdumpContext->ui32Disable > 0)
		psPdumpContext->ui32Disable ++;
	else
	{
		// Check the given define
		if (gpsIfStrings)
		{
			psPdumpContext->ui32Disable = 1;
			for(i = 0; i < gpsIfStrings->i32NumStrings; i++)
			{
				if ( strcmp(pszDefine, gpsIfStrings->ppszStrings[i]) == 0 )
				{
					psPdumpContext->ui32Disable = 0;
					break;
				}
			}
		}
		else
		{
			psPdumpContext->ui32Disable = 1;
		}
	}

	TAL_THREADSAFE_LOCK;
    if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_If(psMemSpaceCB->hPdumpMemSpace, pszDefine);
    }
    TAL_THREADSAFE_UNLOCK;
}

/*!
******************************************************************************

 @Function				TALPDUMP_Else

******************************************************************************/
IMG_VOID TALPDUMP_Else(
    IMG_HANDLE						hMemSpace
)
{
	TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	IMG_ASSERT( hMemSpace != IMG_NULL );
    TAL_THREADSAFE_INIT;

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	switch (psPdumpContext->ui32Disable)
	{
	case 0:
		psPdumpContext->ui32Disable = 1;
		break;
	case 1:
		psPdumpContext->ui32Disable = 0;
		break;
	}

	TAL_THREADSAFE_LOCK;
	if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_Else(psMemSpaceCB->hPdumpMemSpace);
    }
    TAL_THREADSAFE_UNLOCK;
}

/*!
******************************************************************************

 @Function				TALPDUMP_EndIf

******************************************************************************/
IMG_VOID TALPDUMP_EndIf(
    IMG_HANDLE						hMemSpace
)
{
	TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	IMG_ASSERT( hMemSpace != IMG_NULL );
    TAL_THREADSAFE_INIT;

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	if (psPdumpContext->ui32Disable > 0)
		psPdumpContext->ui32Disable --;

    TAL_THREADSAFE_LOCK;
	if ( talpdump_Check(psPdumpContext) )
    {
		PDUMP_CMD_Endif(psMemSpaceCB->hPdumpMemSpace);
    }
    TAL_THREADSAFE_UNLOCK;
}

/*!
******************************************************************************

 @Function				TALPDUMP_TestIf

******************************************************************************/
IMG_BOOL TALPDUMP_TestIf(
    IMG_HANDLE						hMemSpace
)
{
	TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	IMG_ASSERT( hMemSpace != IMG_NULL );

    psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);

	return (psPdumpContext->ui32Disable > 0);
}


/*!
******************************************************************************

 @Function				TALPDUMP_AddMemspaceToContext

******************************************************************************/
void TALPDUMP_AddMemspaceToContext (
	IMG_CHAR *	 		pszContextName,
	IMG_HANDLE			hMemSpaceCB
)
{
	TAL_sMemSpaceCB *	psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpaceCB;
	IMG_UINT32			i;
	TAL_sPdumpContext *	psPdumpContext = IMG_NULL;
	IMG_HANDLE			hPdumpContext;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	hPdumpContext = PDUMP_CMD_AddMemspaceToPdumpContext(pszContextName, psMemSpaceCB->hPdumpMemSpace);
	
	/* If we are forcing a single context...*/
	if (gbForceSingleContext == IMG_TRUE || pszContextName == NULL)
	{	
		psMemSpaceCB->psPdumpContext = &gsPdumpContext;
		/* Allow API to be re-entered */
		TAL_THREADSAFE_UNLOCK;
		return;
	}

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);


    /* Check to see if context already defined...*/
    for (i=0; i<gui32NoPdumpContexts; i++)
    {
        if (strcmp(pszContextName, gasPdumpContexts[i].pszContextName) == 0)
        {
            psPdumpContext = &gasPdumpContexts[i];
            break;
        }
    }

	// Create a new context
	if (psPdumpContext == IMG_NULL)
	{
		psPdumpContext = &gasPdumpContexts[gui32NoPdumpContexts++];
		/* Initialise the context...*/
		talpdump_InitContext(pszContextName, psPdumpContext);
	}

	/* Set the memspace PDUMP context/...*/
	psMemSpaceCB->psPdumpContext = psPdumpContext;

	// Setup Context in Pdumping code
	psMemSpaceCB->psPdumpContext->hPdumpCmdContext = hPdumpContext;
	
    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;
}

/*!
******************************************************************************

 @Function				TALPDUMP_GetNoContexts

******************************************************************************/

IMG_UINT32 TALPDUMP_GetNoContexts ()
{
	return gui32NoPdumpContexts;
}

/*!
******************************************************************************
##############################################################################
 Category:              TAL Debug Functions
##############################################################################
******************************************************************************/


/*!
******************************************************************************

 @Function              TALDEBUG_GetHostMemAddress

******************************************************************************/
IMG_VOID * TALDEBUG_GetHostMemAddress(
    IMG_HANDLE		hDeviceMem
)
{
    TAL_psDeviceMemCB psDeviceMemCB = (TAL_psDeviceMemCB) hDeviceMem;
	return (IMG_VOID *) (psDeviceMemCB->pHostMem);
}

/*!
******************************************************************************

@Function              TALDEBUG_GetDevMemAddress

******************************************************************************/
IMG_UINT64 TALDEBUG_GetDevMemAddress(
    IMG_HANDLE		hDeviceMem
)
{
    TAL_psDeviceMemCB psDeviceMemCB = (TAL_psDeviceMemCB) hDeviceMem;
	return psDeviceMemCB->ui64DeviceMem;
}

/*!
******************************************************************************

 @Function              TALDEBUG_DevComment

******************************************************************************/
IMG_RESULT TALDEBUG_DevComment(
    IMG_HANDLE                      hMemSpace,
    const IMG_CHAR *                psCommentText
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	DEVIF_sDeviceCB *		psDevIfDeviceCB;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

   /* Check memory space Id */
    if (hMemSpace == IMG_NULL)
	{
		rResult = IMG_ERROR_INVALID_PARAMETERS;
		goto devcomment_return;
	}
	psPdumpContext = psMemSpaceCB->psPdumpContext;
	IMG_ASSERT(psPdumpContext);


	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

	psDevIfDeviceCB = &psMemSpaceCB->psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;
    /* Call on to device */
	if (psDevIfDeviceCB->pfnSendComment)
	{
	  psDevIfDeviceCB->pfnSendComment(psDevIfDeviceCB, (IMG_CHAR*)psCommentText);
	}

devcomment_return:
    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

 @Function              TALDEBUG_DevPrintf

******************************************************************************/
IMG_RESULT TALDEBUG_DevPrintf(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pszPrintText,
	...
)
{
	IMG_CHAR cBuffer[512];
	va_list args;
	va_start (args, pszPrintText);
	vsprintf (cBuffer,pszPrintText, args);
	va_end (args);
	return TALDEBUG_DevPrint(hMemSpace, cBuffer);
}

/*!
******************************************************************************

 @Function              TALDEBUG_DevPrint

******************************************************************************/
IMG_RESULT TALDEBUG_DevPrint(
    IMG_HANDLE                      hMemSpace,
    IMG_CHAR *                      pszPrintText
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sPdumpContext *     psPdumpContext;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	DEVIF_sDeviceCB *		psDevIfDeviceCB = IMG_NULL;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    if (psMemSpaceCB)
    {
        /* Check memory space Id */
		IMG_ASSERT(psMemSpaceCB);
		psPdumpContext = psMemSpaceCB->psPdumpContext;
		psDevIfDeviceCB = &psMemSpaceCB->psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;
		IMG_ASSERT(psPdumpContext);
    }
    else
    {
        /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
    }

	if (psPdumpContext->bInsideLoop)
	{
		tal_LoopNormalCommand(psMemSpaceCB);
	}

    /* Call on to device interface */
	if (psDevIfDeviceCB != IMG_NULL && psDevIfDeviceCB->pfnDevicePrint)
	{
		psDevIfDeviceCB->pfnDevicePrint(psDevIfDeviceCB, pszPrintText);
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

 @Function              TALDEBUG_AutoIdle

******************************************************************************/
IMG_RESULT TALDEBUG_AutoIdle(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Time
)
{
    IMG_RESULT              rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	DEVIF_sDeviceCB *		psDevIfDeviceCB;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

	if (hMemSpace)
	{
		psDevIfDeviceCB = &psMemSpaceCB->psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;

		if (psDevIfDeviceCB->pfnAutoIdle != IMG_NULL)
		{
			/* Call on to device interface */
			psDevIfDeviceCB->pfnAutoIdle(psDevIfDeviceCB, ui32Time);
		}
	}

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

 @Function              TALDEBUG_GetDevTime

******************************************************************************/
IMG_RESULT TALDEBUG_GetDevTime(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64 *                    pui64Time
)
{
	IMG_RESULT              rResult = IMG_SUCCESS;
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
	DEVIF_sDeviceCB *		psDevIfDeviceCB = IMG_NULL;
    TAL_sPdumpContext *     psPdumpContext;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);

    if (hMemSpace)
    {
        /* Check memory space Id */
		IMG_ASSERT(psMemSpaceCB);
		psPdumpContext = psMemSpaceCB->psPdumpContext;
		psDevIfDeviceCB = &psMemSpaceCB->psMemSpaceInfo->psDevInfo->sDevIfDeviceCB;
		IMG_ASSERT(psPdumpContext);
    }
    else
    {
        /* Ensure that we are using the global context - otherwise we need to have a memory space to
           obtain the PDUMP context...*/
        IMG_ASSERT(gui32NoPdumpContexts == 0);
        psPdumpContext = &gsPdumpContext;
    }

    /* Call on to device interface */
	if (psDevIfDeviceCB && psDevIfDeviceCB->pfnGetDeviceTime)
	{
		rResult = (IMG_RESULT)psDevIfDeviceCB->pfnGetDeviceTime(psDevIfDeviceCB, pui64Time);
		if (*pui64Time == 0)
		{
			rResult = IMG_ERROR_GENERIC_FAILURE;
		}
	}
	else
		rResult = IMG_ERROR_NOT_SUPPORTED;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

 @Function              TALDEBUG_ReadMem

******************************************************************************/
IMG_RESULT TALDEBUG_ReadMem(
	IMG_UINT64				ui64MemoryAddress,
	IMG_UINT32				ui32Size,
	IMG_PUINT8				pui8Buffer
)
{
	DEVIF_CopyAbsToHost(ui64MemoryAddress, pui8Buffer, ui32Size);
	return IMG_SUCCESS;
}

