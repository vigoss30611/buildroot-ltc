/*!
******************************************************************************
 @file   : tal_intdefs.h

 @brief	internal definitions and types for the Target Abstraction Layer

 @Author Tom Hollis

 @date   23/03/2011

         <b>Copyright 2011 by Imagination Technologies Limited.</b>\n
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
         Target Abstraction Layer (TAL) Internal Definitions and Types Header File.

 @Platform
	     Platform Independent


******************************************************************************/
#ifndef TAL_INTDEFS_H_
#define TAL_INTDEFS_H_


#define DIRECTORY_SEPARATOR         "/"
#define DIRECTORY_SEPARATOR_IMG_CHAR    '/'

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMG_TAL Target Abstraction Layer
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
 /**
 * @name Internal definitions
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the Internal group
 *------------------------------------------------------------------------*/
#endif

#include "tal_defs.h"

/*!
******************************************************************************
 Defines used for sizing arrays etc...
******************************************************************************/
#define TAL_MAX_LOOP_NESTING_DEPTH	(10)
#define TAL_MAX_SYNC_ID 63
#define TAL_MAX_SEMA_ID 64

#define MAX_DEV_LIST 20

#if defined (__TAL_USE_OSA__)
    #include <osa.h>

    extern IMG_HANDLE ghTalThreadsafeMutex;
    extern IMG_BOOL gbTalMutexInitialised; 

    #define TAL_THREADSAFE_CREATE                  \
    {                                              \
        if(gbTalMutexInitialised == IMG_FALSE )      \
        {                                          \
            OSA_CritSectCreate(&ghTalThreadsafeMutex);\
            gbTalMutexInitialised = IMG_TRUE;      \
        }                                          \
    }                                              

    #define TAL_THREADSAFE_DESTROY                 \
    {                                              \
        if(gbTalMutexInitialised == IMG_TRUE )       \
        {                                          \
            OSA_CritSectDestroy(ghTalThreadsafeMutex);\
            gbTalMutexInitialised = IMG_FALSE;     \
        }                                          \
    }                                              \

    #define TAL_THREADSAFE_INIT
    #define TAL_THREADSAFE_LOCK                    \
    {                                              \
        if(gbTalMutexInitialised == IMG_TRUE )       \
        {                                          \
            OSA_CritSectLock(ghTalThreadsafeMutex);    \
        }                                          \
    }       
    #define TAL_THREADSAFE_UNLOCK                  \
    {                                              \
        if(gbTalMutexInitialised == IMG_TRUE )       \
        {                                          \
            OSA_CritSectUnlock(ghTalThreadsafeMutex); \
        }                                          \
    }       

    #define TAL_SLEEP(ui32Time)      OSA_ThreadSleep(ui32Time)
#elif defined(__TAL_USE_MEOS__)
    #include <krn.h>

    #define TAL_THREADSAFE_CREATE
    #define TAL_THREADSAFE_DESTROY
    #define TAL_THREADSAFE_INIT      KRN_IPL_T           ThreadSafeOldPriority
    #define TAL_THREADSAFE_LOCK      ThreadSafeOldPriority = KRN_raiseIPL()
    #define TAL_THREADSAFE_UNLOCK    KRN_restoreIPL(ThreadSafeOldPriority)

    #define TAL_SLEEP(ui32Time)                    \
    {                                              \
        KRN_TASKQ_T hibernateQ;                    \
	    DQ_init(&hibernateQ);                      \
	    KRN_hibernate(&hibernateQ, ui32Time);      \
    }                                              \

#else
	// dummy declaration so no test are needed in the code
	#define TAL_THREADSAFE_CREATE
    #define TAL_THREADSAFE_DESTROY
    #define TAL_THREADSAFE_INIT
    #define TAL_THREADSAFE_LOCK
    #define TAL_THREADSAFE_UNLOCK

    #define TAL_SLEEP(ui32Time)
#endif

#ifdef __TAL_MAKE_THREADSAFE__
#error "depecrated macro __TAL_MAKE_THREADSAFE__: use __TAL_USE_OSA__ or __TAL_USE_MEOS__ to make TAL threadsafe"
#endif

#ifdef __TAL_CALLBACK_SUPPORT__
#include <lst.h>
#endif

#ifdef TAL_USE_RETURN_CODES
#define TAL_ASSERT(bTest,pszFmt,...) (!(bTest) && return IMG_ERROR_FATAL)
#else
#if defined _MSC_VER && _MSC_VER < 1400

#include <stdarg.h>

// Full function needs to be implemented prior to VS2005
static void TAL_ASSERT(const IMG_BOOL bTest, const IMG_CHAR *pcReason, ... )
{
	va_list		params;

	if (!bTest)
	{
		va_start( params, pcReason );
		printf(pcReason, params);
		IMG_ASSERT(bTest);
	}
}
#else
//simple define for assert using variadic macro for newer compilers
#define TAL_ASSERT(bTest,pszFmt,...) \
	(!(bTest) ? printf(pszFmt,##__VA_ARGS__) : (bTest), IMG_ASSERT(bTest))
#endif
#endif

// memory twiddling for 36bit and 40bit testing, not to be confused with address twiddling for images
#define TAL_OLD_36BIT_TWIDDLE_MASK_LOW	(0x7ULL << 25)
#define TAL_OLD_36BIT_TWIDDLE_SHIFT		(7)
#define TAL_OLD_36BIT_TWIDDLE_MASK_HIGH	(TAL_OLD_36BIT_TWIDDLE_MASK_LOW << TAL_OLD_36BIT_TWIDDLE_SHIFT)

#define TAL_40BIT_TWIDDLE_MASK_LOW		(0xFULL << 24)
#define TAL_40BIT_TWIDDLE_SHIFT			(12)
#define TAL_40BIT_TWIDDLE_MASK_HIGH		(TAL_40BIT_TWIDDLE_MASK_LOW << TAL_40BIT_TWIDDLE_SHIFT)

#define TAL_36BIT_TWIDDLE_MASK_LOW		(0xFULL << 24)
#define TAL_36BIT_TWIDDLE_SHIFT			(8)
#define TAL_36BIT_TWIDDLE_MASK_HIGH	(	TAL_36BIT_TWIDDLE_MASK_LOW << TAL_36BIT_TWIDDLE_SHIFT)

#define TAL_TWIDDLE_DROP32_MASK			(0xFFFFFFFFULL)

// Flags reading
#define TAL_MEMORY_ALLOCATION_CONTROL ((psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_MEM_ALLOC_MASK) >> TAL_DEVFLAG_MEM_ALLOC_SHIFT)
#define TAL_SGX_VIRTUAL_MEMORY_CONTROL ((psMemSpaceCB->psMemSpaceInfo->psDevInfo->ui32DevFlags & TAL_DEVFLAG_SGX_VIRT_MEM_MASK)	>> TAL_DEVFLAG_SGX_VIRT_MEM_SHIFT)
/*!
******************************************************************************
 Loop state...
******************************************************************************/
typedef enum TAL_eLoopState_tag
{
	TAL_LOOPSTATE_INACTIVE,
	TAL_LOOPSTATE_OPEN,
	TAL_LOOPSTATE_START_DETECTED,
	TAL_LOOPSTATE_NORMAL_COMMAND_DETECTED,
	TAL_LOOPSTATE_TEST_DETECTED,
	TAL_LOOPSTATE_END_DETECTED

} TAL_eLoopState;

/*!
******************************************************************************
 Loop types...
******************************************************************************/
typedef enum TAL_eLoopType_tag
{
	TAL_LOOPTYPE_UNKNOWN,
	TAL_LOOPTYPE_WDO,
	TAL_LOOPTYPE_DOW
} TAL_eLoopType;

/*!
******************************************************************************
 Structure used to retain loop context information...
******************************************************************************/
typedef struct TAL_sLoopContext_tag
{
	TAL_eLoopState					eLoopState;
	TAL_eLoopType					eLoopType;
	IMG_BOOL						bFirstEncounter;
	IMG_BOOL						bFirstIteration;
	IMG_UINT32						ui32IterationNumber;
	TAL_eLoopControl				eLoopControl;
	struct TAL_sLoopContext_tag *	psParentLoopContext;

} TAL_sLoopContext;


/*!
******************************************************************************

  The device memory control block.  Each block of device memory has an
  associated control block which contains information relating to the block.

******************************************************************************/
typedef struct TAL_tag_sDeviceMemCB
{
	IMG_CHAR *						pHostMem;				//<! A pointer to the host copy of this memory
	IMG_HANDLE						hMemSpace;				//<! The handle of the memory space to which the block belongs
#ifdef TAL_NORMAL
	struct TAL_tag_sDeviceMemCB *	psNextDeviceMemCB;		//<! Pointer to the next block in the list
	struct TAL_tag_sDeviceMemCB *	psPrevDeviceMemCB;		//<! Pointer to the previous block in the list
	IMG_UINT32						ui32MemBlockId;			//<! The ID of this memory block
	IMG_CHAR *						pszMemBlockName;		//<! The unique name of this memory block
	IMG_UINT64						ui64Size;				//<! The size of the block (in bytes)
	IMG_UINT64						ui64Alignment;			//<! The block alignment
	IMG_BOOL						bBlockUpdatedToByHost;	//<! Flag to indicate that block has been updated by the host
	IMG_UINT64						ui64DeviceMemBase;		//<! The base address of the block in the device memmory
	IMG_UINT64						ui64DeviceMem;			//<! The aligned address of the block in the device memmory
	IMG_BOOL						bMappedMem;				//<! IMG_TRUE if the memory is to be mapped rather than allocated
	IMG_VOID *						pMemSpaceParam;			//<! Free for use by the TAL functions
#endif
	
} TAL_sDeviceMemCB, *TAL_psDeviceMemCB;

/*!
******************************************************************************
 Structure used to retain a list of recently used device memory
******************************************************************************/
typedef struct TAL_DeviceList_tag
{
	TAL_psDeviceMemCB			psDeviceMemCB;
	struct TAL_DeviceList_tag * pdlNextDevice;
} TAL_DeviceList;

// Data structure used to store current coalesced Block Memory Loads
typedef struct
{
	IMG_HANDLE		hMemSpace;				//<! The handle of the memory space to which the block belongs
	IMG_UINT64		ui64StartAddr;
	IMG_UINT64		ui64Size;
	IMG_PUINT8		pui8HostMem;
}TAL_sMemCoalesce, *TAL_psMemCoalesce;

/*!
******************************************************************************
 Structure used to retain PDUMP context information...
******************************************************************************/
typedef struct TAL_sPdumpContext_tag
{
    IMG_CHAR			pszContextName[128];     /*!< Context name                       */
    IMG_UINT32			ui32LoopNestingDepth;   /*!< Current loop depth                 */
    IMG_BOOL			bInsideLoop;            /*!< IMG_TRUE if inside loop            */
    TAL_sLoopContext *  psCurLoopContext;   /*!< Current loop context               */
    TAL_sLoopContext    asLoopContexts[TAL_MAX_LOOP_NESTING_DEPTH]; /*!< Array of loop contexts */
	IMG_UINT32			ui32Disable;		/*<! Used for keeping track of disable level during ifs */
	IMG_BOOL			bPdumpDisable;
	TAL_sMemCoalesce	sLoadCoalesce;
	IMG_HANDLE			hPdumpCmdContext;
} TAL_sPdumpContext;




/*!
******************************************************************************
  This structure contains the memory space control block.
******************************************************************************/
typedef struct TAL_sMemSpaceCB_tag
{
	TAL_psMemSpaceInfo			psMemSpaceInfo;					//<! Memspace information
	volatile IMG_UINT32 *		pui32RegisterSpace;				//! Pointer to register base						    */
	IMG_UINT64					aui64InternalVariables[TAL_NUMBER_OF_INTERNAL_VARS];	/*! Internal registers array for this memory space		*/

	// Interrupt parameters
	IMG_UINT32					aui32intmask[TAL_NUM_INT_MASKS];//!< The interrupt masks
	TAL_pfnCheckInterruptFunc	apfnCheckInterruptFunc;			/*! Interrupt check functions.							*/
	IMG_VOID *					apCallbackParameter;			/*! Application callback parameter.						*/
#if !defined (IMG_KERNEL_MODULE)
    #if defined(__TAL_USE_OSA__)
    IMG_HANDLE                  hCbMutex;                       /*! Lock to ensure interrupt check callback is not removed whilst being called (Osa Layer).  */
    IMG_HANDLE                  hMonitoredRegionsMutex;         /*! Sync access to Monitored Regions list. */
    IMG_HANDLE                  hDevMemListMutex;               /*! Sync access to list of allocated memory blocks. */
    #elif defined(__TAL_USE_MEOS__)
    KRN_LOCK_T                  sCbLock;                        /*! Lock to ensure interrupt check callback is not removed whilst being called (KRN).  */
    #endif
#endif
#if defined (__TAL_CALLBACK_SUPPORT__)
	// Callback Parameters
	IMG_BOOL					bCbSlotAllocated;				/*! Indicates whether or not a callback manager slot has been allocated */
	IMG_UINT32					ui32CbManSlot;					/*! Allocated callback manager slot						*/
	LST_T						sMonitoredRegions;				/*! List of monitored regions							*/
	IMG_VOID *					pFilterWriteCallbackParam;		/*! App filter callback parameter						*/
#endif
#ifdef TAL_NORMAL
    TAL_sPdumpContext *			psPdumpContext;					/*! Pdump context for this memory space   */
	IMG_HANDLE					hMemAllocator;					//<! Handle to the memory allocation control block
	IMG_UINT32					ui32NoMemCBs;					//<! No. of device memory CBs							*/
	IMG_UINT32					ui32NextBlockId;				//<! Number of next Block Id 
	TAL_psDeviceMemCB			psDevMemList;					//<! List of allocated memory blocks
	IMG_HANDLE					hPdumpMemSpace;					//<! Memspace handle for pdump commands API
#endif
} TAL_sMemSpaceCB, *TAL_psMemSpaceCB;

typedef struct
{
	TAL_psMemSpaceCB psMemSpaceCB;
} TALITR_sMemSpace;

typedef struct
{
	IMG_HANDLE hMemSpaceIter;
} TALITR_sInterrupt;

typedef union
{
	TALITR_sMemSpace	itrMemSpce;
	TALITR_sInterrupt	itrInterrupt;
}TALITR_uInfo;

typedef struct
{
	TALITERATOR_eTypes	eType;
	TALITR_uInfo		uInfo;
}TALITR_sIter, *TALITR_psIter;



/*!
******************************************************************************
 Structure containing Semaphore Id information
******************************************************************************/
typedef struct
{
	IMG_HANDLE hSrcMemSpace;
	IMG_HANDLE hDestMemSpace;
}TAL_sSemaphoreCB;

/*!
******************************************************************************
 Structure containing an UINT64 represented by two UINT32s
 -Low (less significant bits) -High (most significant bits)
******************************************************************************/

typedef struct
{
	IMG_UINT32		Low;
	IMG_UINT32		High;
}TAL_uint64;

typedef IMG_BOOL ( * TAL_pfnCheckFunc) (
	IMG_UINT64    					ui64ReadValue,				//!< The value read from memory / register
	IMG_UINT64    					ui64TestVal_WrOff,			//!< Either the test value or the write offset
	IMG_UINT64    					ui64Enable_PackSize,		//!< Either the enable mask or the packet size
	IMG_UINT64						ui64BufSize					//!< The buffer size (not required for comparison operations)
);

/*!
******************************************************************************

 Internal memory space functions

******************************************************************************/
TAL_psMemSpaceCB TAL_GetFreeMemSpaceCB(IMG_VOID);
IMG_VOID TAL_DestroyMemSpaceCBs(IMG_VOID);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the Interanl group
 *------------------------------------------------------------------------*/
 /**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif 

#endif /* TAL_INTDEFS_H_ */
