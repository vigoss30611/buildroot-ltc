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

#include <tal.h>
#include <tal_intdefs.h>
#include <target.h>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#ifndef IMG_KERNEL_MODULE
#include <string.h>
#else
#include <linux/string.h>
#include <linux/delay.h> // for TAL_Wait
#if defined(ANDROID) || 1
#include <asm/io.h>
#endif
#endif

#ifdef WIN32
#define inline
#endif

static IMG_BOOL                         gbInitialised = IMG_FALSE;                                      /*!< Module initiliased                                                                 */
static TAL_sMemSpaceCB          gasMemSpaceCBs[TAL_MAX_MEMSPACES];                      //! Statically allocated list of memory spaces
static IMG_UINT32                       gui32NoMemSpaces = 0;                                           //! No of memory spaces currently in use

static TAL_pfnTAL_MemSpaceRegister gpfnTAL_MemSpaceRegisterOverride = IMG_NULL;  /* Pointer to override MemSpaceRegister function */

/*!
******************************************************************************
##############################################################################
Category:              TAL Other Functions
##############################################################################
******************************************************************************/


// Funcitons which can be mapped straight onto others in TAL Light
inline IMG_RESULT TALMEM_WriteWordDefineContext32(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT32 ui32Value, IMG_HANDLE hContext)
{
    return TALMEM_WriteWord32(hDeviceMem,ui64Offset,ui32Value);
}
inline IMG_RESULT TALMEM_WriteWordDefineContext64(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT64 ui64Value, IMG_HANDLE hContextMemSpace)
{
    return TALMEM_WriteWord64(hDeviceMem,ui64Offset,ui64Value);
}
inline IMG_RESULT TALREG_ReadWordToSAB32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32* pui32Value, IMG_UINT32 ui32Count, IMG_UINT32 ui32Time)
{
    return TALREG_ReadWord32(hMemSpace,ui32Offset,pui32Value);
}
inline IMG_RESULT TALREG_WriteWordDefineContext32(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32 ui32Value, IMG_HANDLE hContext)
{
    return TALREG_WriteWord32(hMemSpace,ui32Offset,ui32Value);
}
inline IMG_RESULT TALREG_WriteWordDefineContext64(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT64 ui64Value, IMG_HANDLE hContext)
{
    return TALREG_WriteWord64(hMemSpace,ui32Offset,ui64Value);
}
// Get rid of those functions which are not appropriate in TAL Light
inline IMG_VOID TALPDUMP_SetPdump1MemName (IMG_CHAR* pszMemName){}
inline IMG_VOID  TALPDUMP_Comment(IMG_HANDLE hMemSpace, const IMG_CHAR* psCommentText){}
inline IMG_VOID  TALPDUMP_MiscOutput(IMG_HANDLE hMemSpace, IMG_CHAR* pOutputText){}
inline IMG_VOID  TALPDUMP_ConsoleMessage(IMG_HANDLE hMemSpace, IMG_CHAR* psCommentText){}
inline IMG_VOID  TALPDUMP_SetConversionSourceData(IMG_UINT32 ui32SourceFileID, IMG_UINT32 ui32SourceLineNum){}
inline IMG_VOID  TALPDUMP_MemSpceCaptureEnable (IMG_HANDLE hMemSpace, IMG_BOOL bEnable, IMG_BOOL* pbPrevState){}
inline IMG_VOID  TALPDUMP_DisableCmds(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Offset, IMG_UINT32 ui32DisableFlags){}
inline IMG_VOID  TALPDUMP_SetFlags(IMG_UINT32 ui32Flags){}
inline IMG_VOID  TALPDUMP_ClearFlags(IMG_UINT32 ui32Flags){}
inline IMG_UINT32  TALPDUMP_GetFlags(void){return 0;}
inline IMG_VOID  TALPDUMP_SetFilename(IMG_HANDLE hMemSpace, TAL_eSetFilename eSetFilename, IMG_CHAR* psFileName){}
inline IMG_CHAR*  TALPDUMP_GetFilename(IMG_HANDLE hMemSpace, TAL_eSetFilename eSetFilename){return IMG_NULL;}
inline IMG_VOID  TALPDUMP_SetFileoffset(IMG_HANDLE hMemSpace, TAL_eSetFilename eSetFilename, IMG_UINT64 ui64FileOffset){}
inline IMG_VOID  TALPDUMP_DefineContext(IMG_CHAR* pszContextName, IMG_CHAR** ppszMemSpaceNames){}
inline IMG_VOID  TALPDUMP_EnableContextCapture(IMG_HANDLE hMemSpace){}
inline IMG_UINT32 TALPDUMP_GetNoContexts(void){return 0;}
inline IMG_VOID TALPDUMP_AddSyncToTDF(IMG_HANDLE hMemSpace, IMG_UINT32 ui32SyncId){}
inline IMG_VOID TALPDUMP_AddCommentToTDF(IMG_CHAR* pcComment){}
inline IMG_VOID TALPDUMP_AddTargetConfigToTDF(IMG_CHAR* pszFilePath){}
inline IMG_VOID TALPDUMP_GenerateTDF(IMG_CHAR* psFilename){}
inline IMG_VOID TALPDUMP_CaptureStart(IMG_CHAR* psTestDirectory){}
inline IMG_VOID TALPDUMP_CaptureStop(void){}
inline IMG_VOID TALPDUMP_ChangeResFileName(IMG_HANDLE hMemSpace,IMG_CHAR* sFileName){}
inline IMG_VOID TALPDUMP_ChangePrmFileName(IMG_HANDLE hMemSpace,IMG_CHAR* sFileName){}
inline IMG_VOID TALPDUMP_ResetResFileName(IMG_HANDLE hMemSpace){}
inline IMG_VOID TALPDUMP_ResetPrmFileName(IMG_HANDLE hMemSpace){}
inline IMG_BOOL TALPDUMP_IsCaptureEnabled(void){return IMG_FALSE;}
inline IMG_UINT32 TALPDUMP_GetSemaphoreId(IMG_HANDLE hSrcMemSpace, IMG_HANDLE hDestMemSpace){return 0;}
inline IMG_VOID TALPDUMP_Lock(IMG_HANDLE hMemSpace, IMG_UINT32 ui32SemaId){}
inline IMG_VOID TALPDUMP_Unlock(IMG_HANDLE hMemSpace, IMG_UINT32 ui32SemaId){}
inline IMG_RESULT TALPDUMP_SyncWithId(IMG_HANDLE hMemSpace, IMG_UINT32 ui32SyncId){return IMG_SUCCESS;}
inline IMG_VOID TALPDUMP_Sync(IMG_HANDLE hMemSpace){}
inline IMG_VOID TALPDUMP_SetDefineStrings(TAL_Strings * psStrings){}
inline IMG_VOID TALPDUMP_If(IMG_HANDLE hMemSpace, IMG_CHAR* pszDefine){}
inline IMG_BOOL TALPDUMP_TestIf(IMG_HANDLE hMemSpace){return IMG_FALSE;}
inline IMG_VOID TALPDUMP_Else(IMG_HANDLE hMemSpace){}
inline IMG_VOID TALPDUMP_EndIf(IMG_HANDLE hMemSpace){}
inline IMG_RESULT TALDEBUG_DevComment(IMG_HANDLE hMemSpace, IMG_CONST IMG_CHAR* psCommentText) {return IMG_SUCCESS;}
inline IMG_RESULT TALDEBUG_DevPrintf(IMG_HANDLE hMemSpace, IMG_CHAR* pszPrintText,...) {return IMG_SUCCESS;}
inline IMG_RESULT TALDEBUG_DevPrint(IMG_HANDLE hMemSpace, IMG_CHAR* pszPrintText) {return IMG_SUCCESS;}
inline IMG_RESULT TALDEBUG_AutoIdle(IMG_HANDLE hMemSpace, IMG_UINT32 ui32Time) {return IMG_SUCCESS;}
inline IMG_RESULT TALDEBUG_GetDevTime(IMG_HANDLE hMemSpace, IMG_UINT64* pui64Time ) {return IMG_SUCCESS;}
inline IMG_RESULT TALMEM_DumpImage(TAL_sImageHeaderInfo* psImageHeaderInfo, IMG_CHAR* psImageFilename,IMG_HANDLE hDeviceMem1,IMG_UINT64 ui64Offset1, IMG_UINT32 ui32Size1,
                                   IMG_HANDLE hDeviceMem2, IMG_UINT64 ui64Offset2, IMG_UINT32 ui32Size2
                                   ,IMG_HANDLE hDeviceMem3, IMG_UINT64 ui64Offset3, IMG_UINT32 ui32Size3) {return IMG_SUCCESS;}
inline IMG_VOID TALSETUP_SetTargetApplicationFlags(IMG_UINT32 ui32TargetAppFlags){}
inline IMG_RESULT TALMEM_UpdateHost(IMG_HANDLE hDeviceMem) {return IMG_SUCCESS;}
inline IMG_RESULT TALMEM_UpdateHostRegion(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT64 ui64Size) {return IMG_SUCCESS;}
inline IMG_RESULT TALMEM_UpdateDevice(IMG_HANDLE hDeviceMem) {return IMG_SUCCESS;}
inline IMG_RESULT TALMEM_UpdateDeviceRegion(IMG_HANDLE hDeviceMem, IMG_UINT64 ui64Offset, IMG_UINT64 ui64Size) {return IMG_SUCCESS;}
inline IMG_RESULT TALMEM_Free(IMG_HANDLE* phDeviceMem) {return IMG_SUCCESS;}
inline IMG_VOID TALSETUP_RegisterPollFailFunc(TAL_pfnPollFailFunc pfnFunc){}

/*!
******************************************************************************

@Function              TAL_CheckMemSpaceEnable

******************************************************************************/
IMG_BOOL TAL_CheckMemSpaceEnable(
    IMG_HANDLE hMemSpace
    )
{
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

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Check TAL has been initialised */
    IMG_ASSERT(gbInitialised);


#ifdef IMG_KERNEL_MODULE
    // when using the kernel use time as time in micro-seconds
    mdelay(ui32Time);
#elif !defined(__TAL_NOT_THREADSAFE__)
    TAL_SLEEP(ui32Time);
#else
    {
        IMG_UINT32 ui32StartTime;
        /* Wait for specified no. of cycles */
        ui32StartTime = ui32Time;
        while (ui32StartTime != 0)
        {
            ui32StartTime--;
        }
    }
#endif

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

@Function              TALLOOP_Open

******************************************************************************/
IMG_RESULT TALLOOP_Open(
    IMG_HANDLE                                          hMemSpace,
    IMG_HANDLE *                                        phLoop
    )
{
    //Not Supported in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}



/*!
******************************************************************************

@Function              TALLOOP_Start

******************************************************************************/

IMG_RESULT TALLOOP_Start(
    IMG_HANDLE                                          hLoop
    )
{
    //Not Supported in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}



/*!
******************************************************************************

@Function              TALLOOP_TestRegister

******************************************************************************/
IMG_RESULT TALLOOP_TestRegister(
    IMG_HANDLE                                          hLoop,
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT32                                          ui32Offset,
    IMG_UINT32                                          ui32CheckFuncIdExt,
    IMG_UINT32                                          ui32RequValue,
    IMG_UINT32                                          ui32Enable,
    IMG_UINT32                                          ui32LoopCount
    )
{
    //Not Supported in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}


/*!
******************************************************************************

@Function              TALLOOP_TestMemory

******************************************************************************/
IMG_RESULT TALLOOP_TestMemory(
    IMG_HANDLE                                          hLoop,
    IMG_HANDLE                                          hDeviceMem,
    IMG_UINT32                                          ui32Offset,
    IMG_UINT32                                          ui32CheckFuncIdExt,
    IMG_UINT32                                          ui32RequValue,
    IMG_UINT32                                          ui32Enable,
    IMG_UINT32                                          ui32LoopCount
    )
{
    //Not Supported in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}


/*!
******************************************************************************

@Function              TALLOOP_TestInternalReg

******************************************************************************/
IMG_RESULT TALLOOP_TestInternalReg(
    IMG_HANDLE                                          hLoop,
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT32                                          ui32RegId,
    IMG_UINT32                                          ui32CheckFuncIdExt,
    IMG_UINT32                                          ui32RequValue,
    IMG_UINT32                                          ui32Enable,
    IMG_UINT32                                          ui32LoopCount
    )
{
    //Not Supported in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}


/*!
******************************************************************************

@Function              TALLOOP_End

******************************************************************************/
IMG_RESULT TALLOOP_End(
    IMG_HANDLE                                          hLoop,
    TAL_eLoopControl *                          peLoopControl
    )
{
    //Not Supported in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}


/*!
******************************************************************************

@Function              TALLOOP_Close

******************************************************************************/
IMG_RESULT TALLOOP_Close(
    IMG_HANDLE                                          hMemSpace,
    IMG_HANDLE *                                        phLoop
    )
{
    //Not Supported in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}

/*!
******************************************************************************

@Function              TAL_GetLoopControl

******************************************************************************/
TAL_eLoopControl TALLOOP_GetLoopControl(
    IMG_HANDLE hMemSpace
    )
{
    return TAL_LOOP_TEST_TRUE;
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
IMG_RESULT TALSETUP_Initialise(void)
{
    IMG_RESULT          rResult = IMG_SUCCESS;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_CREATE;
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    if (!gbInitialised)
    {
        gbInitialised = IMG_TRUE;
        rResult = TARGET_Initialise ((TAL_pfnTAL_MemSpaceRegister) &TALSETUP_MemSpaceRegister);
    }

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALSETUP_Deinitialise

******************************************************************************/
IMG_RESULT TALSETUP_Deinitialise (void)
{
    IMG_RESULT                  rResult = IMG_SUCCESS;

    TAL_THREADSAFE_INIT;

    TALSETUP_Reset();

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_LOCK;

    /* If we are not initilised...*/
    if (gbInitialised)
    {
        gbInitialised = IMG_FALSE;
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
IMG_BOOL TALSETUP_IsInitialised (void)
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
    TAL_sMemSpaceCB     *                       psMemSpaceCB;
    IMG_UINT32                                  ui32I;

    if (gpfnTAL_MemSpaceRegisterOverride != IMG_NULL)
        gpfnTAL_MemSpaceRegisterOverride(psMemSpaceInfo);

    IMG_ASSERT(gui32NoMemSpaces < TAL_MAX_MEMSPACES);
    // Check that this memory space has not already been registered
    for  (ui32I = 0; ui32I < gui32NoMemSpaces; ui32I++)
    {
        if(strcmp(psMemSpaceInfo->pszMemSpaceName, gasMemSpaceCBs[ui32I].psMemSpaceInfo->pszMemSpaceName) == 0)
        {
            return IMG_ERROR_ALREADY_COMPLETE;
        }
    }
    psMemSpaceCB = &gasMemSpaceCBs[gui32NoMemSpaces++];

    /* Copy information relating to this memory space */
    IMG_MEMSET(psMemSpaceCB, 0, sizeof(TAL_sMemSpaceCB));
    psMemSpaceCB->psMemSpaceInfo = psMemSpaceInfo;

    /* If register memory space...*/
    if (psMemSpaceInfo->eMemSpaceType == TAL_MEMSPACE_REGISTER)
    {
        /* Set point to memory space block...*/
        psMemSpaceCB->pui32RegisterSpace = (IMG_PUINT32)(IMG_UINTPTR)psMemSpaceInfo->sRegInfo.ui64RegBaseAddr;


        for( ui32I=0; ui32I< TAL_NUM_INT_MASKS; ui32I++ )
        {
            psMemSpaceCB->aui32intmask[ui32I] = 0;
        }

        ui32I = (psMemSpaceInfo->sRegInfo.ui32intnum & ~(0x20-1))>>5;
        if( ui32I < TAL_NUM_INT_MASKS )
        {
            psMemSpaceCB->aui32intmask[ui32I] = (1<< ((psMemSpaceInfo->sRegInfo.ui32intnum)&(0x20-1)) );
        }
    }

    /* Success */
    return IMG_SUCCESS;
}

/*!
******************************************************************************

@Function                               TAL_GetMemSpaceHandle

******************************************************************************/
IMG_HANDLE TAL_GetMemSpaceHandle (
    const IMG_CHAR *            psMemSpaceName
    )
{
    IMG_UINT32                                  ui32I;

    // Check that this memory space has not already been registered
    for  (ui32I = 0; ui32I < gui32NoMemSpaces; ui32I++)
    {
        if(strcmp(psMemSpaceName, gasMemSpaceCBs[ui32I].psMemSpaceInfo->pszMemSpaceName) == 0)
        {
            return (IMG_HANDLE)&gasMemSpaceCBs[ui32I];
        }
    }
    return IMG_NULL;
}

/*!
******************************************************************************

@Function              TALSETUP_Reset

******************************************************************************/
IMG_RESULT TALSETUP_Reset (void)
{
    IMG_RESULT  rResult = IMG_SUCCESS;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    gui32NoMemSpaces = 0;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************
##############################################################################
Category:              TAL Register Functions
##############################################################################
******************************************************************************/

/*!
******************************************************************************

@Function              TALREG_WriteWord32

******************************************************************************/
IMG_RESULT TALREG_WriteWord32 (
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32                      ui32Value
    )
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    TAL_THREADSAFE_INIT;

    /* Write register...*/
#ifdef IMG_KERNEL_MODULE
	if ( ui32Offset < psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui32RegSize )
	{
		iowrite32(ui32Value, (void*)((IMG_UINT32)psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset));
	}
	else
	{
		printk(KERN_DEBUG "WARNING: %s cancelled (0x%x is outside register bank %s)\n", __FUNCTION__, ui32Offset, psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName);
	}
#else
    psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2] = ui32Value;
#endif

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALREG_WriteWord64

******************************************************************************/
IMG_RESULT TALREG_WriteWord64 (
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT64                      ui64Value
    )
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    TAL_THREADSAFE_INIT;

	/// @ WriteWord64 in kernel
	
    /* Write register...*/
    ((IMG_PUINT64)psMemSpaceCB->pui32RegisterSpace)[ui32Offset>>3] = ui64Value;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

@Function              TALREG_WriteDevMemRef32

******************************************************************************/
IMG_RESULT TALREG_WriteDevMemRef32(
    IMG_HANDLE              hMemSpace,
    IMG_UINT32              ui32Offset,
    IMG_HANDLE              hRefDeviceMem,
    IMG_UINT64              ui64RefOffset
    )
{
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hRefDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    TAL_THREADSAFE_INIT;
    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_LOCK;

#ifdef IMG_KERNEL_MODULE
    if ( ui32Offset < psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui32RegSize )
    {
        IMG_UINT32 ui32Value = (IMG_UINT32)(pui8HostMem + ui64RefOffset);
        
        iowrite32(ui32Value, (void*)((IMG_UINT32)psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset));
    }
    else
    {
        printk(KERN_DEBUG "WARNING: %s cancelled (0x%x is outside register bank %s)\n", __FUNCTION__, ui32Offset, psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName);
    }
#else
    psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2] = (IMG_UINT32)(IMG_UINTPTR)(pui8HostMem + ui64RefOffset);
#endif

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

@Function              TALREG_ReadWord32

******************************************************************************/
IMG_RESULT TALREG_ReadWord32(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32 *                    pui32Value
    )
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    /* Read register...*/
#ifdef IMG_KERNEL_MODULE
	if ( ui32Offset < psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui32RegSize )
	{
		*pui32Value = ioread32((void*)((IMG_UINT32)psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui64RegBaseAddr + ui32Offset));
	}
	else
	{
		printk(KERN_DEBUG "WARNING: %s cancelled (0x%x is outside register bank %s)\n", __FUNCTION__, ui32Offset, psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName);
	}
#else
    *pui32Value = psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2];
#endif

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALREG_ReadWord64

******************************************************************************/
IMG_RESULT TALREG_ReadWord64(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT64 *                    pui64Value
    )
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/// @ ReadWord64 in kernel
	
    /* Read register...*/
    *pui64Value = ((IMG_PUINT64)psMemSpaceCB->pui32RegisterSpace)[ui32Offset>>3];

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

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
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/// @ WriteWords32 in kernel
    IMG_MEMCPY((IMG_VOID*)&psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2], pui32Values, ui32WordCount*4);

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
    IMG_UINT32 *                    pui32Values
    )
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

	/// @ ReadWords32 in kernel
    IMG_MEMCPY(pui32Values, (IMG_VOID*)&psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2], ui32WordCount*4);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALREG_Poll32

******************************************************************************/
IMG_RESULT TALREG_Poll32(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT32                      ui32RequValue,
    IMG_UINT32                      ui32Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
    )
{
    IMG_UINT32                                  ui32Count, ui32ReadVal;
    IMG_RESULT                                  rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *                   psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
    IMG_BOOL                                    bPass = IMG_FALSE;

#ifdef IMG_KERNEL_MODULE
	if ( ui32Offset >= psMemSpaceCB->psMemSpaceInfo->sRegInfo.ui32RegSize )
	{
		printk(KERN_DEBUG "WARNING: %s cancelled (0x%x is outside register bank %s)\n", __FUNCTION__, ui32Offset, psMemSpaceCB->psMemSpaceInfo->pszMemSpaceName);
		return IMG_ERROR_FATAL;
	}
#endif

    /* Test the value */
    for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
    {
        TAL_THREADSAFE_INIT;
        TAL_THREADSAFE_LOCK;

        // Read from the device
        ui32ReadVal = psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2];
        bPass = TAL_TestWord(ui32ReadVal, ui32RequValue, ui32Enable, 0, ui32CheckFuncIdExt);

        /* Allow API to be re-entered */
        TAL_THREADSAFE_UNLOCK;

        // Check whether we meet the exit criteria
        if( !bPass )
        {
            TAL_Wait(hMemSpace, ui32TimeOut);
        }
        else
            break;
    }
    if (ui32Count >= ui32PollCount)     rResult = IMG_ERROR_TIMEOUT;
    return rResult;
}

/*!
******************************************************************************

@Function              TALREG_Poll64

******************************************************************************/
IMG_RESULT TALREG_Poll64(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
    )
{
    IMG_UINT32                                  ui32Count;
    IMG_UINT64                                  ui64ReadVal;
    IMG_RESULT                                  rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *                   psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
    IMG_BOOL                                    bPass = IMG_FALSE;

	/// @ Poll64 in kernel

    /* Test the value */
    for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
    {
        TAL_THREADSAFE_INIT;
        TAL_THREADSAFE_LOCK;

        // Read from the device
        ui64ReadVal = ((IMG_PUINT64)psMemSpaceCB->pui32RegisterSpace)[ui32Offset>>3];
        bPass = TAL_TestWord(ui64ReadVal, ui64RequValue, ui64Enable, 0, ui32CheckFuncIdExt);

        /* Allow API to be re-entered */
        TAL_THREADSAFE_UNLOCK;

        // Check whether we meet the exit criteria
        if( !bPass )
        {
            TAL_Wait(hMemSpace, ui32TimeOut);
        }
        else
            break;
    }
    if (ui32Count >= ui32PollCount)     rResult = IMG_ERROR_TIMEOUT;
    return rResult;
}

/*!
******************************************************************************

@Function              TALREG_CircBufPoll32

******************************************************************************/
IMG_RESULT TALREG_CircBufPoll32(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT32                      ui32Offset,
    IMG_UINT32                                          ui32WriteOffsetVal,
    IMG_UINT32                                          ui32PacketSize,
    IMG_UINT32                                          ui32BufferSize,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
    )
{
    IMG_RESULT                  rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *                   psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
    IMG_BOOL                                    bPass;
    IMG_UINT32                                  ui32Count, ui32ReadVal;

	/// @ CircBufPoll32 in kernel

    for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
    {
        TAL_THREADSAFE_INIT;
        TAL_THREADSAFE_LOCK;

        ui32ReadVal = psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2];
        bPass = TAL_TestWord(ui32ReadVal, ui32WriteOffsetVal, ui32PacketSize, ui32BufferSize, TAL_CHECKFUNC_CBP);

        /* Allow API to be re-entered */
        TAL_THREADSAFE_UNLOCK;

        // Check whether we meet the exit criteria
        if(!bPass)
        {
            TAL_Wait(hMemSpace, ui32TimeOut);
        }
        else
            break;
    }
    if (ui32Count >= ui32PollCount)     rResult = IMG_ERROR_TIMEOUT;

    return rResult;
}


/*!
******************************************************************************

@Function              TALINTVAR_WriteToReg32

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToReg32       (
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT32                                          ui32Offset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_RESULT                          rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
    TAL_sMemSpaceCB *           psIntVarMemSpaceCB = (TAL_sMemSpaceCB*)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2] = (IMG_UINT32)psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId];

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALINTVAR_WriteToReg64

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToReg64       (
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT32                                          ui32Offset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_RESULT                          rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
    TAL_sMemSpaceCB *           psIntVarMemSpaceCB = (TAL_sMemSpaceCB*)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    ((IMG_PUINT64)psMemSpaceCB->pui32RegisterSpace)[ui32Offset>>3] = psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId];

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALINTVAR_ReadFromReg32

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromReg32      (
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT32                                          ui32Offset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_RESULT                  rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *                   psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
    TAL_sMemSpaceCB *                   psIntVarMemSpaceCB = (TAL_sMemSpaceCB*)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId] = psMemSpaceCB->pui32RegisterSpace[ui32Offset>>2];

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALINTVAR_ReadFromReg64

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromReg64      (
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT32                                          ui32Offset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_RESULT                  rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *                   psMemSpaceCB = (TAL_sMemSpaceCB*)hMemSpace;
    TAL_sMemSpaceCB *                   psIntVarMemSpaceCB = (TAL_sMemSpaceCB*)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId] = ((IMG_PUINT64)psMemSpaceCB->pui32RegisterSpace)[ui32Offset>>3];

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function                               TALINTVAR_WriteToVirtMem

******************************************************************************/
/* comment for FELIX */
/*IMG_RESULT TALINTVAR_WriteToVirtMem (
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId,
    IMG_UINT32                                          ui32Flags
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}*/


/*!
******************************************************************************

@Function                               TALINTVAR_ReadFromVirtMem

******************************************************************************/
/* commented for FELIX */
/*IMG_RESULT TALINTVAR_ReadFromVirtMem (
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId,
    IMG_UINT32                                          ui32Flags
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}*/


/*!
******************************************************************************

@Function                               TALINTVAR_WriteVirtMemReference

******************************************************************************/
IMG_RESULT TALINTVAR_WriteVirtMemReference (
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}
/*!
******************************************************************************
##############################################################################
Category:              TAL Memory Functions
##############################################################################
******************************************************************************/

/*!
******************************************************************************

@Function              TALMEM_WriteWord32

******************************************************************************/
IMG_RESULT TALMEM_WriteWord32(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32Value
    )
{
    IMG_RESULT                  rResult = IMG_SUCCESS;
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hDeviceMem;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    *(IMG_PUINT32)(pui8HostMem + ui64Offset) = ui32Value;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALMEM_WriteWord64

******************************************************************************/
IMG_RESULT TALMEM_WriteWord64(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                      ui64Value
    )
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hDeviceMem;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    *(IMG_PUINT64)(pui8HostMem + ui64Offset) = ui64Value;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALMEM_ReadWord32

******************************************************************************/
IMG_RESULT TALMEM_ReadWord32(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_PUINT32                                         pui32Value
    )
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hDeviceMem;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    *pui32Value = *(IMG_PUINT32)(pui8HostMem + ui64Offset);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALMEM_ReadWord64

******************************************************************************/
IMG_RESULT TALMEM_ReadWord64(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_PUINT64                                         pui64Value
    )
{
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hDeviceMem;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    *pui64Value = *(IMG_PUINT64)(pui8HostMem + ui64Offset);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALMEM_Poll32

******************************************************************************/
IMG_RESULT TALMEM_Poll32(
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT32                      ui32RequValue,
    IMG_UINT32                      ui32Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
    )
{
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_BOOL                            bPass;
    IMG_UINT32                          ui32Count;
    IMG_UINT32                          ui32ReadVal;


    /* Test the value */
    for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
    {
        TAL_THREADSAFE_INIT;
        TAL_THREADSAFE_LOCK;

        ui32ReadVal = *(IMG_PUINT32)(pui8HostMem + ui64Offset);
        bPass = TAL_TestWord(ui32ReadVal, ui32RequValue, ui32Enable, 0, ui32CheckFuncIdExt);

        TAL_THREADSAFE_UNLOCK;

        // Check whether we meet the exit criteria
        if( !bPass )
        {
            TAL_Wait(hDeviceMem, ui32TimeOut);
        }
        else
            break;
    }
    if (ui32Count >= ui32PollCount)     rResult = IMG_ERROR_TIMEOUT;

    return rResult;
}

/*!
******************************************************************************

@Function              TALMEM_Poll64

******************************************************************************/
IMG_RESULT TALMEM_Poll64(
    IMG_HANDLE                                          hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT32                      ui32CheckFuncIdExt,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
    )
{
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hDeviceMem;
    IMG_RESULT              rResult = IMG_SUCCESS;
    IMG_BOOL                            bPass;
    IMG_UINT32                          ui32Count;
    IMG_UINT64                          ui64ReadVal;


    /* Test the value */
    for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
    {
        TAL_THREADSAFE_INIT;
        TAL_THREADSAFE_LOCK;

        ui64ReadVal = *(IMG_PUINT64)(pui8HostMem + ui64Offset);
        bPass = TAL_TestWord(ui64ReadVal, ui64RequValue, ui64Enable, 0, ui32CheckFuncIdExt);

        TAL_THREADSAFE_UNLOCK;

        // Check whether we meet the exit criteria
        if( !bPass )
        {
            TAL_Wait(hDeviceMem, ui32TimeOut);
        }
        else
            break;
    }
    if (ui32Count >= ui32PollCount)     rResult = IMG_ERROR_TIMEOUT;

    return rResult;
}


/*!
******************************************************************************

@Function              TALMEM_CircBufPoll32

******************************************************************************/
IMG_RESULT TALMEM_CircBufPoll32(
    IMG_HANDLE                                          hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT32                                          ui32WriteOffsetVal,
    IMG_UINT32                                          ui32PacketSize,
    IMG_UINT32                                          ui32BufferSize,
    IMG_UINT32                                          ui32PollCount,
    IMG_UINT32                                          ui32TimeOut
    )
{
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hDeviceMem;
    IMG_RESULT                          rResult = IMG_SUCCESS;
    IMG_UINT32                          ui32Count;
    IMG_BOOL                            bPass;
    IMG_UINT32                          ui32ReadVal;

    for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
    {
        TAL_THREADSAFE_INIT;
        TAL_THREADSAFE_LOCK;

        ui32ReadVal = *(IMG_PUINT32)(pui8HostMem + ui64Offset);
        bPass = TAL_TestWord(ui32ReadVal, ui32WriteOffsetVal, ui32PacketSize, ui32BufferSize, TAL_CHECKFUNC_CBP);

        /* Allow API to be re-entered */
        TAL_THREADSAFE_UNLOCK;

        // Check whether we meet the exit criteria
        if(!bPass)
        {
            TAL_Wait(hDeviceMem, ui32TimeOut);
        }
        else
            break;
    }

    return rResult;
}

/*!
******************************************************************************

@Function              TALMEM_CircBufPoll64

******************************************************************************/
IMG_RESULT TALMEM_CircBufPoll64(
    IMG_HANDLE                                          hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_UINT64                                          ui64WriteOffsetVal,
    IMG_UINT64                                          ui64PacketSize,
    IMG_UINT64                                          ui64BufferSize,
    IMG_UINT32                                          ui32PollCount,
    IMG_UINT32                                          ui32TimeOut
    )
{
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hDeviceMem;
    IMG_RESULT                          rResult = IMG_SUCCESS;
    IMG_UINT32                          ui32Count;
    IMG_BOOL                            bPass;
    IMG_UINT64                          ui64ReadVal;

    for( ui32Count = 0; ui32Count < ui32PollCount; ui32Count++ )
    {
        TAL_THREADSAFE_INIT;
        TAL_THREADSAFE_LOCK;

        ui64ReadVal = *(IMG_PUINT64)(pui8HostMem + ui64Offset);
        bPass = TAL_TestWord(ui64ReadVal, ui64WriteOffsetVal, ui64PacketSize, ui64BufferSize, TAL_CHECKFUNC_CBP);

        /* Allow API to be re-entered */
        TAL_THREADSAFE_UNLOCK;

        // Check whether we meet the exit criteria
        if(!bPass)
        {
            TAL_Wait(hDeviceMem, ui32TimeOut);
        }
        else
            break;
    }

    return rResult;
}


/*!
******************************************************************************

@Function              TALMEM_Malloc

******************************************************************************/
IMG_RESULT TALMEM_Malloc(
    IMG_HANDLE                                          hMemSpace,
    IMG_CHAR *                                          pHostMem,
    IMG_UINT64                                          ui64Size,
    IMG_UINT64                                          ui64Alignment,
    IMG_HANDLE *                                        phDeviceMem,
    IMG_BOOL                                            bUpdateDevice,
    IMG_CHAR *                                          pszMemName
    )
{
    *phDeviceMem = pHostMem;

    return IMG_SUCCESS;
}

/*
******************************************************************************

@Function                               TALMEM_Map

******************************************************************************/
IMG_RESULT TALMEM_Map(
    IMG_HANDLE                                          hMemSpace,
    IMG_CHAR *                                          pHostMem,
    IMG_UINT64                                          ui64DeviceAddr,
    IMG_UINT64                                          ui64Size,
    IMG_UINT64                                          ui64Alignment,
    IMG_HANDLE *                                        phDeviceMem
    )
{
    // On an embedded system the device address and host address should be the same
    IMG_ASSERT((IMG_CHAR*)(IMG_UINTPTR)ui64DeviceAddr == pHostMem);

    *phDeviceMem = pHostMem;

    return IMG_SUCCESS;
}

/*!
******************************************************************************

@Function              TALMEM_WriteDevMemRef32

******************************************************************************/
IMG_RESULT TALMEM_WriteDevMemRef32(
    IMG_HANDLE                                          hDeviceMem,
    IMG_UINT64                                          ui64Offset,
    IMG_HANDLE                                          hRefDeviceMem,
    IMG_UINT64                                          ui64RefOffset
    )
{
    IMG_UINT32* pui32TargetMem = (IMG_UINT32*)((IMG_UINT8*)hDeviceMem + ui64Offset);
    IMG_RESULT rResult = IMG_SUCCESS;
    IMG_UINT32 ui32Value = (IMG_UINT32)(hRefDeviceMem);
       
    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    *pui32TargetMem = ui32Value;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALMEM_WriteDevMemRef64

******************************************************************************/
IMG_RESULT TALMEM_WriteDevMemRef64(
    IMG_HANDLE                                          hDeviceMem,
    IMG_UINT64                                          ui64Offset,
    IMG_HANDLE                                          hRefDeviceMem,
    IMG_UINT64                                          ui64RefOffset
    )
{
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hRefDeviceMem;
    IMG_UINT64*                         pui64TargetMem = (IMG_UINT64*)((IMG_UINT8*)hDeviceMem + ui64Offset);
    IMG_RESULT              rResult = IMG_SUCCESS;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    *pui64TargetMem = (IMG_UINTPTR)(pui8HostMem + ui64RefOffset);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALINTVAR_WriteToMem32

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToMem32 (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_UINT32*                         pui32TargetMem = (IMG_UINT32*)((IMG_UINT8*)hDeviceMem + ui64Offset);
    IMG_RESULT                          rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psIntVarMemSpaceCB = (TAL_sMemSpaceCB *)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    // Write the word to memory
    *pui32TargetMem = (IMG_UINT32)psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId];

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function              TALINTVAR_WriteToMem64

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToMem64 (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_UINT64*                         pui64TargetMem = (IMG_UINT64*)((IMG_UINT8*)hDeviceMem + ui64Offset);
    IMG_RESULT                          rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psIntVarMemSpaceCB = (TAL_sMemSpaceCB *)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    // Write the word to memory
    *pui64TargetMem = psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId];

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}


/*!
******************************************************************************

@Function              TALINTVAR_ReadFromMem32

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromMem32 (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_UINT32*                         pui32TargetMem = (IMG_UINT32*)((IMG_UINT8*)hDeviceMem + ui64Offset);
    IMG_RESULT                          rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psIntVarMemSpaceCB = (TAL_sMemSpaceCB *)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId] = *pui32TargetMem;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function                               TALINTVAR_ReadFromMem64

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromMem64 (
    IMG_HANDLE                      hDeviceMem,
    IMG_UINT64                      ui64Offset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_UINT64*                         pui64TargetMem = (IMG_UINT64*)((IMG_UINT8*)hDeviceMem + ui64Offset);
    IMG_RESULT                          rResult = IMG_SUCCESS;
    TAL_sMemSpaceCB *           psIntVarMemSpaceCB = (TAL_sMemSpaceCB *)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    psIntVarMemSpaceCB->aui64InternalVariables[ui32InternalVarId] = *pui64TargetMem;

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return rResult;
}

/*!
******************************************************************************

@Function                               TALINTVAR_WriteToVirtMem32

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToVirtMem32 (
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}


/*!
******************************************************************************

@Function                               TALINTVAR_WriteToVirtMem64

******************************************************************************/
IMG_RESULT TALINTVAR_WriteToVirtMem64 (
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}

/*!
******************************************************************************

@Function                               TALINTVAR_ReadFromVirtMem32

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromVirtMem32 (
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}

/*!
******************************************************************************

@Function                               TALINTVAR_ReadFromVirtMem64

******************************************************************************/
IMG_RESULT TALINTVAR_ReadFromVirtMem64 (
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}


/*!
******************************************************************************

@Function                               talintvar_WriteMemRef

******************************************************************************/
IMG_RESULT TALINTVAR_WriteMemRef (
    IMG_HANDLE                      hRefDeviceMem,
    IMG_UINT64                      ui64RefOffset,
    IMG_HANDLE                                          hIntVarMemSpace,
    IMG_UINT32                                          ui32InternalVarId
    )
{
    IMG_UINT8*                          pui8HostMem = (IMG_UINT8*)hRefDeviceMem;
    TAL_sMemSpaceCB *           psRegMemSpaceCB = (TAL_sMemSpaceCB *)hIntVarMemSpace;

    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    psRegMemSpaceCB->aui64InternalVariables[ui32InternalVarId] = (IMG_UINT32)(IMG_UINTPTR)(pui8HostMem + ui64RefOffset);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return IMG_SUCCESS;
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
    IMG_MEMCPY(pui8Buffer, (IMG_CHAR*)(IMG_UINTPTR)ui64MemoryAddress, ui32Size);
    return IMG_SUCCESS;
}

/*!
******************************************************************************
##############################################################################
Category:              TAL Virtual Memory Functions
##############################################################################
******************************************************************************/

// Virtual Address functions are currently not implemented in TAL Light


/*!
******************************************************************************

@Function                               TALVMEM_SetContext

******************************************************************************/

IMG_RESULT TALVMEM_SetContext(
    IMG_HANDLE                          hMemSpace,
    IMG_UINT32              ui32MmuContextId,
    IMG_UINT32              ui32MmuType,
    IMG_HANDLE              hDeviceMem
    )
{
    return IMG_SUCCESS;
}

/*!
******************************************************************************

@Function                               TALVMEM_SetTiledRegion

******************************************************************************/

IMG_RESULT TALVMEM_SetTiledRegion(
    IMG_HANDLE                          hMemSpace,
    IMG_UINT32              ui32MmuContextId,
    IMG_UINT32              ui32TiledRegionNo,
    IMG_UINT64                          ui64DevVirtAddr,
    IMG_UINT64              ui64Size,
    IMG_UINT32              ui32XTileStride
    )
{
    return IMG_SUCCESS;
}

/*!
******************************************************************************

@Function                               TALVMEM_Poll32

******************************************************************************/
IMG_RESULT TALVMEM_Poll32(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_UINT32                      ui32CheckFuncId,
    IMG_UINT32                      ui32RequValue,
    IMG_UINT32                      ui32Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}

/*!
******************************************************************************

@Function                               TALVMEM_Poll64

******************************************************************************/
IMG_RESULT TALVMEM_Poll64(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_UINT32                      ui32CheckFuncId,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
    IMG_UINT32                      ui32PollCount,
    IMG_UINT32                      ui32TimeOut
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}

/*!
******************************************************************************

@Function              TALVMEM_CircBufPoll32

******************************************************************************/
IMG_RESULT TALVMEM_CircBufPoll32(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_UINT64                                          ui64WriteOffsetVal,
    IMG_UINT64                                          ui64PacketSize,
    IMG_UINT64                                          ui64BufferSize,
    IMG_UINT32                                          ui32PollCount,
    IMG_UINT32                                          ui32TimeOut
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}

/*!
******************************************************************************

@Function              TALVMEM_CircBufPoll64

******************************************************************************/
IMG_RESULT TALVMEM_CircBufPoll64(
    IMG_HANDLE                      hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_UINT64                                          ui64WriteOffsetVal,
    IMG_UINT64                                          ui64PacketSize,
    IMG_UINT64                                          ui64BufferSize,
    IMG_UINT32                                          ui32PollCount,
    IMG_UINT32                                          ui32TimeOut
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}

/*!
******************************************************************************

@Function                               TALVMEM_UpdateHost

******************************************************************************/
IMG_RESULT TALVMEM_UpdateHost(
    IMG_HANDLE                                          hMemSpace,
    IMG_UINT64                                          ui64DevVirtAddr,
    IMG_UINT32                                          ui32MmuContextId,
    IMG_UINT64                                          ui64Size,
    IMG_VOID *                                          pvHostBuf
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}


/*!
******************************************************************************

@Function                               TALVMEM_DumpImage

******************************************************************************/
IMG_RESULT TALVMEM_DumpImage(
    IMG_HANDLE                                          hMemSpace,
    TAL_sImageHeaderInfo *          psImageHeaderInfo,
    IMG_CHAR *                      psImageFilename,
    IMG_UINT64                                          ui64DevVirtAddr1,
    IMG_UINT32                                          ui32MmuContextId1,
    IMG_UINT64                                          ui64Size1,
    IMG_VOID *                                          pvHostBuf1,
    IMG_UINT64                                          ui64DevVirtAddr2,
    IMG_UINT32                                          ui32MmuContextId2,
    IMG_UINT64                                          ui64Size2,
    IMG_VOID *                                          pvHostBuf2,
    IMG_UINT64                                          ui64DevVirtAddr3,
    IMG_UINT32                                          ui32MmuContextId3,
    IMG_UINT64                                          ui64Size3,
    IMG_VOID *                                          pvHostBuf3
    )
{
    // Virtual Memory functions not currently implemented in TAL Light
    return IMG_ERROR_NOT_SUPPORTED;
}


/*!
******************************************************************************
##############################################################################
Category:          TAL Internal Variable Functions
##############################################################################
******************************************************************************/


/*!
******************************************************************************

@Function                               TALINTVAR_RunCommand

******************************************************************************/
IMG_RESULT TALINTVAR_RunCommand(
    IMG_UINT32                                          ui32CommandId,
    IMG_HANDLE                                          hDestRegMemSpace,
    IMG_UINT32                                          ui32DestRegId,
    IMG_HANDLE                                          hOpRegMemSpace,
    IMG_UINT32                      ui32OpRegId,
    IMG_HANDLE                                          hLastOpMemSpace,
    IMG_UINT64                                          ui64LastOperand,
    IMG_BOOL                                            bIsRegisterLastOperand
    )
{
    /* Make API serially re-enterant - use IPL rather than semaphore to prevent deadlocks under Windows */
    TAL_THREADSAFE_INIT;
    TAL_THREADSAFE_LOCK;

    TALINTVAR_ExecCommand(ui32CommandId, hDestRegMemSpace, ui32DestRegId, hOpRegMemSpace,
                          ui32OpRegId, hLastOpMemSpace, ui64LastOperand, bIsRegisterLastOperand);

    /* Allow API to be re-entered */
    TAL_THREADSAFE_UNLOCK;

    return IMG_SUCCESS;
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
    IMG_HANDLE          hDeviceMem
    )
{
    return hDeviceMem;
}

/*!
******************************************************************************

@Function              TALDEBUG_GetDevMemAddress

******************************************************************************/
IMG_UINT64 TALDEBUG_GetDevMemAddress(
    IMG_HANDLE          hDeviceMem
    )
{
    return (IMG_UINTPTR)hDeviceMem;
}


/*!
******************************************************************************

@Function              TALDEBUG_ReadMem

******************************************************************************/
IMG_RESULT TALDEBUG_ReadMem(
    IMG_UINT64                          ui64MemoryAddress,
    IMG_UINT32                          ui32Size,
    IMG_PUINT8                          pui8Buffer
    )
{
    IMG_MEMCPY(pui8Buffer, (IMG_CHAR*)(IMG_UINTPTR)ui64MemoryAddress, ui32Size);
    return IMG_SUCCESS;
}

