/*!
******************************************************************************
 @file   : tal_defs.h

 @brief	definitions and types for the Target Abstraction Layer.

 @Author Tom Hollis

 @date   19/08/2010

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

@details
         Target Abstraction Layer (TAL) Definitions and Types Header File.

******************************************************************************/

#if !defined (__TALDEFS_H__)
#define __TALDEFS_H__

#ifdef TAL_NORMAL
#include "devif_api.h"
#endif
#include "img_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMG_TAL Target Abstraction Layer
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif

#if !defined (TAL_MAX_MEMSPACES)
/*! Max. memory spaces supported by the TAL if __TAL_MALLOC_CBS__ not defined		*/
#define	TAL_MAX_MEMSPACES		(64)
#endif

/*! Defines 4 words of interrupt mask.					*/
#define TAL_NUM_INT_MASKS (4)

/*! Interrupt number assigned when no interrupt exists  */
#define TAL_NO_IRQ  (999)

/*! The number of internal TAL registers */
#if !defined(TAL_NUMBER_OF_INTERNAL_VARS)
#define TAL_NUMBER_OF_INTERNAL_VARS		(64)
#endif

#if !defined(TAL_MAX_PDUMP_CONTEXTS)
	#define TAL_MAX_PDUMP_CONTEXTS  (64)             /*!< Max. number of PDUMP contexts        */
#endif

/* Added here to remove tal_old */


#define TAL_MEMSPACE_ID_ALL		(0)		//!< Used to signal all memory spaces.
#define TAL_MEMSPACE_ID_ANY		(0)		//!< Used to signal any memory spaces.
#define TAL_MEMSPACE_ID_NONE	(0)		//!< Used to signal no memory space.

#define TAL_CHECKFUNC_MASK		(0x0000FFFF)	//!< Mask for bits used to identify the test function

// These variables are needed to allow TAL_ConfigureRdwVerifyIterations to work
#ifndef TAL_RDW_VERIFY_ITERATIONS
#define TAL_RDW_VERIFY_ITERATIONS (2000000)	//!< default number of iterations to put in a pdump when read word verify is used
#endif
#ifndef TAL_RDW_VERIFY_DELAY
#define TAL_RDW_VERIFY_DELAY (1)	//!< default delay between iterations to put in a pdump when read word verify is used
#endif
extern IMG_UINT32 gui32TalRdwVerifyPollIter;	/*!< Read Verify Poll iterations */
extern IMG_UINT32 gui32TalRdwVerifyPollTime;		/*!< Read Verify Poll Loop Time */

/********************************/

/*! Base address undefined.                 */
#define TAL_BASE_ADDR_UNDEFINED (0xFFFFFFFFFFFFFFFFULL)

/*! Null Memory Block										*/
#define TAL_NO_MEM_BLOCK		(0xFFFFFFFF)

/*! Used to signal all offsets.						        */
#define TAL_OFFSET_ALL		(0xFFFFFFFF)


/*! Used to signal a sync command with no sync ID.			*/
#define TAL_NO_SYNC_ID				(0)


/*! Predefined "=" change function.							*/
#define TAL_CHECKFUNC_ISEQUAL		(0x0)
/*! Predefined "<" change function.							*/
#define TAL_CHECKFUNC_LESS			(0x1)
/*! Predefined "<=" change function.						*/
#define TAL_CHECKFUNC_LESSEQ		(0x2)
/*! Predefined ">" change function.							*/
#define TAL_CHECKFUNC_GREATER		(0x3)
/*! Predefined ">=" change function.						*/
#define TAL_CHECKFUNC_GREATEREQ		(0x4)
/*! Predefined "!=" change function.						*/
#define TAL_CHECKFUNC_NOTEQUAL		(0x5)
//! Predefined Circular Buffer Poll function
#define TAL_CHECKFUNC_CBP			(0x10)
		


/*! Signal that the poll does not have a finite timeout	period	*/
#define TAL_POLLCOUNT_INFINITE	(0xFFFFFFFF)

/*! Signal that the loop does not have a limited number of iterations */
#define TAL_LOOPCOUNT_INFINITE	(0xFFFFFFFF)





/*!
*****************************************************************************
	used in virt address qualifier passing indicating "no qualifier"
****************************************************************************/
#define TAL_DEV_VIRT_QUALIFIER_NONE		(0xFFFFFFFF)


/*!
******************************************************************************
  This type defines the state of a loop control
******************************************************************************/
typedef enum
{
	TAL_LOOP_TEST_FALSE,        /*!< The results of the loop test where FALSE   */
	TAL_LOOP_TEST_TRUE,         /*!< The results of the loop test where TRUE    */
	TAL_LOOP_FAILED,            /*!< The loop failed as the loop count expired  */

} TAL_eLoopControl;

/*!
******************************************************************************
  This type defines the application flags
******************************************************************************/
typedef enum
{
    TALAPP_STUB_OUT_TARGET     = 0x00000001,	    /*!< Stubs out the target    */
	TALAPP_NO_COPY_DEV_TO_HOST = 0x00000002,		/*!< Do not execute code to copy bytes from device to host */

} TAL_eAppFlags;

/*!
******************************************************************************

  The memory space types

******************************************************************************/
typedef enum 
{
	TAL_MEMSPACE_REGISTER,		//<! Memory space is mapped to device registers
	TAL_MEMSPACE_MEMORY			//<! Memory space is mapped to DEVICE memory

} TAL_eMemSpaceType;

/*!
******************************************************************************

  Memory Twiddle Algorithms

******************************************************************************/
typedef enum
{
	TAL_TWIDDLE_NONE,
	TAL_TWIDDLE_36BIT_OLD,										//!< Old 36bit mem twiddle
	TAL_TWIDDLE_36BIT,											//!< 36bit mem twiddle algorithm
	TAL_TWIDDLE_40BIT,											//!< 40bit mem twiddle algorithm
	TAL_TWIDDLE_DROPTOP32,										//!< Drop top 32bits of address
	
}eTAL_Twiddle;

/*!
******************************************************************************
  This struncture contains a list of strings
******************************************************************************/
typedef struct
{
	IMG_CHAR ** ppszStrings;		//!< A list of strings.
	IMG_INT32	i32NumStrings;		//!< The number of strings.
}TAL_Strings;


/*!
******************************************************************************
  This structure contains the Imagination Image Header information.
******************************************************************************/
typedef struct
{
    IMG_UINT32      ui32Signature;      /*!< Signature - set to "IIH0" (will be set by TAL) */
    IMG_UINT32      ui32PixelFormat;    /*!< Pixel format see #IMG_ePixelFormat            */
    IMG_UINT64      ui64Offset1;        /*!< Offset of the first plane of pixel data within
                                             the file pszPixelDataFilename (will be set by TAL) */
    IMG_UINT64      ui64Offset2;        /*!< Offset of the second plane of pixel data within
                                             the file pszPixelDataFilename  (will be set by TAL)*/
    IMG_UINT64      ui64Offset3;        /*!< Offset of the third plane of pixel data within
                                             the file pszPixelDataFilename (will be set by TAL) */
    IMG_UINT32      ui32Width;          /*!< Is the width of the image - in pixels          */
    IMG_UINT32      ui32Height;         /*!< Is the height of the image - in lines          */
    IMG_UINT32      ui32Stride;         /*!< Is the image stride - in bytes                 */
    IMG_UINT32      ui32AddrMode;       /*!< Is the address mode                            */
    IMG_UINT32      ui32Spare1;         /*!< Spare - TBD                                    */
    IMG_UINT32      ui32Spare2;         /*!< Spare - TBD                                    */
    IMG_UINT32      ui32Spare3;         /*!< Spare - TBD                                    */
    IMG_UINT32      ui32Spare4;         /*!< Spare - TBD                                    */

} TAL_sImageHeaderInfo;


//<! These are the flags for the Memspace Info Structure

#define TAL_DEVFLAG_MEM_ALLOC_MASK				(0x0000000F)	/*!< Memory allocation mask				*/
#define TAL_DEVFLAG_MEM_ALLOC_SHIFT				(0)				/*!< Memory allocation shift			*/

#define TAL_DEVFLAG_SEQUENTIAL_ALLOC			(0x0)			//!< (Recommended) Flag for sequential memory allocation
#define TAL_DEVFLAG_4K_PAGED_ALLOC				(0x1)			//!< Flag for sequential 4K page memory allocation (SGX535 and SGX530 only)
#define TAL_DEVFLAG_4K_PAGED_RANDOM_ALLOC		(0x2)			//!< Flag for random 4K page memory allocation (SGX535 and SGX530 only)
#define TAL_DEVFLAG_RANDOM_BLOCK_ALLOC			(0x3)			/*!< Flag for random block memory allocation:
																 random block and sequential base address within the block	*/
#define TAL_DEVFLAG_TOTAL_RANDOM_ALLOC			(0x4)			/*!< Flag for total random memory allocation:	
																 random block and random base address within the block   */
#define TAL_DEVFLAG_DEV_ALLOC					(0x5)			//!< Flag for allocations to happen outside TAL on device or in kernel

#define TAL_DEVFLAG_SGX_VIRT_MEM_MASK			(0x000000F0)	/*!< SGX virtual memory mask            */
#define TAL_DEVFLAG_SGX_VIRT_MEM_SHIFT			(4)				/*!< SGX virtual memory shift           */

#define TAL_DEVFLAG_NO_SGX_VIRT_MEMORY			(0x0)			//!< (Recommended) No specific VMem support
#define TAL_DEVFLAG_SGX535_VIRT_MEMORY			(0x1)			//!< Virtual Mem support specific to SGX535
#define TAL_DEVFLAG_SGX530_VIRT_MEMORY			(0x2)			//!< Virtual Mem support specific to SGX530


#define TAL_DEVFLAG_STUB_OUT					0x00000100		//!< Stub out this device
#define TAL_DEVFLAG_SKIP_PAGE_FAULTS            0x00000200      //!< Ignore page faults
#define TAL_DEVFLAG_COALESCE_LOADS				0x00000800		//!< Attempt to Coalesce block memory loads to improve speed on some RTL systems


												 

#ifdef TAL_NORMAL											

//! This structure contains all information about a sub-device
typedef struct TAL_sSubDeviceCB_tag
{
    DEVIF_sDeviceCB			sDevIfDeviceCB;			//<! Device control block for devif_api
	IMG_CHAR *				pszRegName;				//<! Register Sub-Device Name
	IMG_UINT64				ui64RegStartAddress;	//<! Register start address
	IMG_UINT32				ui32RegSize;			//<! Register Sub-Device Size
	IMG_CHAR *				pszMemName;				//<! Memory Sub-Device Name
	IMG_UINT64				ui64MemStartAddress;	//<! Memory start address
	IMG_UINT64				ui64MemSize;			//<! Memory Sub-Device Size

	struct TAL_sSubDeviceCB_tag * pNext;			//<! Pointer to next sub-device
}TAL_sSubDeviceCB, * TAL_psSubDeviceCB;

//! This structure contains all information about a device
typedef struct
{
	IMG_CHAR *					pszDeviceName;					//!< Device name
	IMG_UINT64					ui64DeviceBaseAddr;				//!< Base address of the device registers.
	IMG_UINT64					ui64MemBaseAddr;				//!< Base address of the default memory region.
	IMG_UINT64					ui64DefMemSize;					//!< Size of the default memory region.
	IMG_UINT64					ui64MemGuardBand;				//!< Size of the default memory guard band.
	IMG_HANDLE					hMemAllocator;					//!< Handle to the memory allocation context
	DEVIF_sDeviceCB				sDevIfDeviceCB;					//!< Device control block for devif_api
	IMG_CHAR *					pszMemArenaName;				//!< Name of the memory arena to which the 
	IMG_UINT32					ui32DevFlags;					//!< Device Flags
	TAL_psSubDeviceCB			psSubDeviceCB;					//!< Sub-Device list
	IMG_BOOL					bConfigured;					//!< Gets set when the device is configured
	eTAL_Twiddle				eTALTwiddle;					//!< TAL Memory Twiddle algorithm
	IMG_BOOL					bVerifyMemWrites;				//<! Read back and verify all writes to memory
} TAL_sDeviceInfo, *TAL_psDeviceInfo;

#elif defined(TAL_LIGHT)

typedef struct
{
	IMG_UINT64					ui64DeviceBaseAddr;				//!< Base address of the device registers.
	IMG_UINT64					ui64MemBaseAddr;				//!< Base address of the default memory region.
} TAL_sDeviceInfo, *TAL_psDeviceInfo;

#endif

//! This structure stores register setup details
typedef struct
{
	IMG_UINT64					ui64RegBaseAddr;				//!< Base address of device registers
	IMG_UINT32					ui32RegSize;					//!< Size of device register block
	IMG_UINT32					ui32intnum;						//!< The interrupt number
} TAL_sRegInfo, * TAL_psRegInfo;	//!< register setup structure typedef

//! This structure stores memory setup details
typedef struct
{
	IMG_UINT64					ui64MemBaseAddr;				//!< Base address of memory region
	IMG_UINT64					ui64MemSize;					//!< Size of memory region
	IMG_UINT64					ui64MemGuardBand;				//!< Memory guard band
} TAL_sMemInfo, * TAL_psMemInfo;	//!< register setup structure typedef

//! This structure contains all information about the memory space
typedef struct
{
	char *						pszMemSpaceName;				//!< Memory space name
	TAL_eMemSpaceType			eMemSpaceType;					//!< Memory space type
	union
	{
		TAL_sRegInfo sRegInfo;
		TAL_sMemInfo sMemInfo;
#ifndef _MSC_VER
	};
#else // fix compiler warning
    } u;
#define sRegInfo u.sRegInfo
#define sMemInfo u.sMemInfo
#endif /* _MSC_VER */

#ifdef TAL_NORMAL
	TAL_psDeviceInfo			psDevInfo;						//!< Information about the relevant device
	char *						pszPdumpContext;				//!< Pdump Context Name
#endif
} TAL_sMemSpaceInfo, *TAL_psMemSpaceInfo;	//!< register setup structure typedef


//! MEMORY WORD SIZE FLAGS
typedef enum
{
	TAL_WORD_FLAGS_32BIT	=		(0x0000001),	//!< Write a 32bit Word
	TAL_WORD_FLAGS_64BIT	=		(0x0000002),	//!< Write a 64bit Word
}TAL_Word_Flags;

/*!
******************************************************************************

 @Function				TAL_pfnPollFailFunc

 @Description

 This is the function prototype for the "poll" fail functions.

 @Input     None.

 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * TAL_pfnPollFailFunc) (void);

/*!
******************************************************************************
  This type defines the registered function type
******************************************************************************/
typedef enum
{
	TAL_FN_TYPE_REG_WRITE_WORDS,		//!< LDW function for registers
	TAL_FN_TYPE_DEINIT,					//!< De-initalise function
	TAL_FN_TYPE_COPY_DEV_VIRT,			//!< Copy device virtual to host function
	TAL_FN_TYPE_SYNC,					//!< Sync function
	TAL_FN_TYPE_LOCK,					//!< Lock function
	TAL_FN_TYPE_UNLOCK,					//!< Unlock function
	TAL_FN_TYPE_MEM_READ_WORD,			//!< Device memory read 32bit word function
	TAL_FN_TYPE_MEM_READ_WORD64,		//!< Device memory read 64bit word function
	TAL_FN_TYPE_MEM_WRITE_WORD,			//!< Device memory write 32bit word function
	TAL_FN_TYPE_MEM_WRITE_WORD64,		//!< Device memory write 64bit word function
	TAL_FN_TYPE_COPY_HOST_TO_DEVICE,	//!< Copy from the device host to device memory
	TAL_FN_TYPE_COPY_DEVICE_TO_HOST,	//!< Copy from the device memory to host memory

	TAL_FN_TYPE_FREE_DEVICE_MEM,		//!< Free device memory for a memory space
	TAL_FN_TYPE_MALLOC_DEVICE_MEM,		//!< Allocate device memory
	TAL_FN_TYPE_GET_FLAGS,				//!< Get flags function
	TAL_FN_TYPE_IDLE,					//!< Idle function
	TAL_FN_TYPE_SEND_COMMENT,			//!< Function to send a comment to the device
	TAL_FN_TYPE_GET_DEVICE_TIME,		//!< Function to get the device time
	TAL_FN_TYPE_DEVICE_PRINT,			//!< Function to tell device to print a string
	TAL_FN_TYPE_AUTOIDLE				//!< Function to tell device how long to idle for if no commands are waiting

} TAL_eRegisteredFnType;

//! These are the types for the iterator functions
typedef enum
{
	TALITR_TYP_INTERRUPT,				//!< Iterates through the check interrupt functions and returns pending interrupts

	// All iteraters from here are internal to TAL and should not be considered part of the API
	TALITR_TYP_NONE,					//!< Null iterater
	TALITR_TYP_MEMSPACE,				//!< Iterates through the TAL memspaces

}TALITERATOR_eTypes;

/*!
******************************************************************************

 @Function				TAL_pfnCheckInterruptFunc

 @Description

 This is the function prototype for the check interrupt functions.

 When using a s/w simulator there is a background task which is used to
 poll/check for interrupts.  This task calls the registered check functions
 to determine whether or not the device interrupt status bits are set and
 whether an interrupt should be "simulated".

 Note: Even though the API or test application may have registered a check
 function it will only be used if the target is a s/w simulator.

 The check should:
	-	Check that the device interrupts are enabled
	-	If interrupts are enabled and the device interrupt status bits indicate
		that an interrupt is pending then the function should return IMG_TRUE
		otherwise it should return IMG_FALSE.

 @Input		hMemSpace : The ID of the memory space.

 @Input     pCallbackParameter	: A pointer to application specific data or IMG_NULL.

 @Return	IMG_BOOL	:	Returns IMG_TRUE if an interrupt is to be "simulated"
							otherwise IMG_FALSE.

******************************************************************************/
typedef IMG_BOOL ( * TAL_pfnCheckInterruptFunc) (
	IMG_HANDLE						hMemSpace,
	IMG_VOID *						pCallbackParameter
);

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the IMG_TAL documentation module
 *------------------------------------------------------------------------*/
#endif 

#if defined (__cplusplus)
}
#endif

#endif //__TALDEFS_H__

