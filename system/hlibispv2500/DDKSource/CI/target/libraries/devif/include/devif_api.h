/*!
******************************************************************************
 @file   : devif_api.h

 @brief	 Device Interface API

 @Author Ray Livesley

 @date   17/10/2006

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
         Functions used for a device interface. The API works by setting up a 
		 DEVIF_sDeviceSB structure using an implementation of the DEVIF_ConfigureDevice
		 function. This function can be replace, however it is recommended that 
		 a new function is written for the new interface and it is added as an option
		 to the existing implementation of DEVIF_ConfigureDevice.
		 Once the configure has set up the device, each function in the API performs
		 a given action on the device.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/


#if !defined (__DEVIF_API_H__)
#define __DEVIF_API_H__

#include <img_errors.h>
#include <img_types.h>
#include "devif_setup.h"

#if defined (__cplusplus)
extern "C" {
#endif

//! Control block definition for setting up this API
typedef struct DEVIF_tag_sDeviceCB		DEVIF_sDeviceCB, *DEVIF_psDeviceCB; //!< Pointer Definition

/*!
******************************************************************************

 @Function				DEVIF_pfnInitalise

 @Description

 This is the function prototype for the initilsation function which sets
 up the device.

 @Input     psDeviceCB  : A pointer to the device control block.
 @Return	IMG_RESULT	: The outcome of the initialisation (IMG_SUCCESS if passed)

******************************************************************************/
typedef IMG_RESULT ( * DEVIF_pfnInitailise) (
	DEVIF_sDeviceCB *   psDeviceCB
);



/*!
******************************************************************************

 @Function				DEVIF_pfnReadRegister

 @Description

 This is the function prototype for the read register function.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64RegAddress	: The register address to be read.
 @Return	IMG_UINT32		: The value read from the register.

******************************************************************************/
typedef IMG_UINT32 ( * DEVIF_pfnReadRegister) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
);


/*!
******************************************************************************

 @Function				DEVIF_pfnWriteRegister

 @Description

 This is the function prototype for the write register function.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64RegAddress	: The register address to be written.
 @Input     ui32Value		: The value to be written.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnWriteRegister) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT32          ui32Value
);

/*!
******************************************************************************

 @Function				DEVIF_pfnReadRegister64

 @Description

 This is the function prototype for the read register function.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64RegAddress	: The register address to be read.
 @Return	IMG_UINT64		: The value read from the register.

******************************************************************************/
typedef IMG_UINT64 ( * DEVIF_pfnReadRegister64) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress
);


/*!
******************************************************************************

 @Function				DEVIF_pfnWriteRegister64

 @Description

 This is the function prototype for the write register function.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64RegAddress	: The register address to be written.
 @Input     ui64Value		: The value to be written.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnWriteRegister64) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64RegAddress,
    IMG_UINT64          ui64Value
);


/*!
******************************************************************************

 @Function				DEVIF_pfnReadMemory

 @Description

 This is the prototype a function to read 32bits of memory.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64DevAddress	: The device address.
 @Return	IMG_UINT32		: The value read from the memory.

******************************************************************************/
typedef IMG_UINT32 ( * DEVIF_pfnReadMemory) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
);

/*!
******************************************************************************

 @Function				DEVIF_pfnWriteMemory

 @Description

 This is the prototype for a function to write 32bit of memory.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64DevAddress	: The device address to be written.
 @Input     ui32Value		: The value to be written.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnWriteMemory) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Value
);

/*!
******************************************************************************

 @Function				DEVIF_pfnReadMemory64

 @Description

 This is the prototype a function to read 64bits of memory.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64DevAddress	: The device address.
 @Return	IMG_UINT64		: The value read from the memory.

******************************************************************************/
typedef IMG_UINT64 ( * DEVIF_pfnReadMemory64) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
);

/*!
******************************************************************************

 @Function				DEVIF_pfnWriteMemory64

 @Description

 This is the prototype for a function to write 64bit of memory.

  @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64DevAddress	: The device address to be written.
 @Input     ui64Value		: The value to be written.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnWriteMemory64) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT64          ui64Value
);

/*!
******************************************************************************

 @Function				DEVIF_pfnMallocMemory

 @Description

 Not Required

 This is the function prototype for the allocate memory function, the
 address of the memory allocation has already been determined so on real
 hardware this function can be ignored.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64DevAddress	: The device address.
 @Input     ui32Size		: The size in bytes.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnMallocMemory) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
);

/*!
******************************************************************************

 @Function				DEVIF_pfnMallocHostVirt

 @Description

 Not Required

 This is the function prototype for the allocate memory function, where the
 address of the memory allocation has not already been determined, allocation
 happens on the device and the address is returned.

 @Input     psDeviceCB			: A pointer to the device control block.
 @Input     ui64Alignment		: The allocation alignment
 @Input     ui64Size			: The size in bytes.
 @Return	IMG_UINT64	: The device address

******************************************************************************/
typedef IMG_UINT64 ( * DEVIF_pfnMallocHostVirt) (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64		ui64Alignment,
    	IMG_UINT64          ui64Size
);



/*!
******************************************************************************

 @Function				DEVIF_pfnFreeMemory

 @Description

 Not Required

 This is the function prototype for the device free memory function.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64DevAddress	: The device address.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnFreeMemory) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress
);



/*!
******************************************************************************

 @Function				DEVIF_pfnIdle

 @Description

 Not Required

 This is the function prototype for the idle function. This asks the device to
 wait for a given number of cycles, if this function does not exist then the host
 will perform a wait instead.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui32NumCycles	: Number of cycles to idle for.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnIdle) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumCycles
);

/*!
******************************************************************************

 @Function				DEVIF_pfnAutoIdle

 @Description

 Not Required

 This is the function prototype for the auto idle function which sets the amount
 of time the device should idle for if no command is forthcoming.

 @Input     psDeviceCB  : A pointer to the device control block.
 @Input     ui32NumCycles : Number of cycles to idle for if no
							command is waiting on the socket.

 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnAutoIdle) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT32          ui32NumCycles
);


/*!
******************************************************************************

 @Function				DEVIF_pfnSendComment

 @Description

 Not Required

 This is the function prototype to send a comment to the device. This is intended
 to attach a comment to the subsequent command, useful in RTL to commment each command.

 @Input     psDeviceCB	: A pointer to the device control block.
 @Input     pcComment	: Comment string.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnSendComment) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pcComment
);

/*!
******************************************************************************

 @Function				DEVIF_pfnDevicePrint

 @Description

 Not Required

 This is the function prototype to send a string for printing to the device,
 this is only useful if the device is capable of outputing debug information.

 @Input     psDeviceCB	: A pointer to the device control block.
 @Input     pszString	: string to print.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnDevicePrint) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_CHAR *          pszString
);

/*!
******************************************************************************

 @Function				DEVIF_pfnGetDeviceTime

 @Description
 
 Not Required
 
 This is the function prototype to get the device time.

 @Input     psDeviceCB	: A pointer to the device control block.
 @Return	pui64Time	: The device time.

******************************************************************************/
typedef IMG_UINT32 ( * DEVIF_pfnGetDeviceTime) (
	DEVIF_sDeviceCB *   psDeviceCB,
	IMG_UINT64 *		pui64Time
);

/*!
******************************************************************************

 @Function				DEVIF_pfnCopyDeviceToHost

 @Description

 Not Required

 This is the function prototype for the block copy of memory from the device,
 if not implemented memory copys will be performed word by word.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     ui64DevAddress	: The device address.
 @Input     pvHostAddress	: The host address.
 @Input     ui32Size		: The size in bytes.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnCopyDeviceToHost) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_UINT64          ui64DevAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
);



/*!
******************************************************************************

 @Function				DEVIF_pfnCopyHostToDevice

 @Description

 Not Required

 This is the function prototype for the to block copy of memory to the device,
 if not implemented memory copys will be performed word by word.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input     pvHostAddress	: The host address.
 @Input     ui64DevAddress	: The device address.
 @Input     ui32Size		: The size in bytes.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnCopyHostToDevice) (
	DEVIF_sDeviceCB *   psDeviceCB,
    IMG_VOID *          pvHostAddress,
    IMG_UINT64          ui64DevAddress,
    IMG_UINT32          ui32Size
);


/*!
******************************************************************************

 @Function				DEVIF_pfnDeinitailise

 @Description

 This is the function prototype for the de-initialise function.

 @Input     psDeviceCB  : A pointer to the device control block.
 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnDeinitailise) (
	DEVIF_sDeviceCB *   psDeviceCB
);

/*!
******************************************************************************

 @Function				DEVIF_pfnRegWriteWords

 @Description

 Not Required

 This is the function prototype for a function to write a block of data to 
 a register. If not implemented copys will happen word by word.

 @Input		pWRAPContext	: A pointer to the wrapper context (defined in the
								memory space control block).
 @Input		pui32MemSource	: A pointer to the source of the data
 @Input		ui64RegAddress	: The Register Address.
 @Input		ui32WordCount	: The number of words to write across
 @Return	None

******************************************************************************/
typedef
IMG_VOID
( * DEVIF_pfnRegWriteWords) (
	DEVIF_sDeviceCB		* psDeviceCB,
	IMG_VOID			* pui32MemSource,
	IMG_UINT64          ui64RegAddress,
	IMG_UINT32    		ui32WordCount
);

/*!
******************************************************************************
 This structure is used to return the device mapping.
******************************************************************************/
typedef struct
{
	IMG_UINT32 *	pui32Registers;		//!< A pointer to the device registers - IMG_NULL if not defined
	IMG_UINT32 		ui32RegSize;		//!< The size of the register block (0 if not known)
	IMG_UINT32 *	pui32SlavePorts;	//!< A pointer to the device slave ports - IMG_NULL if not defined
	IMG_UINT32 		ui32SpSize;			//!< The size of the slave ports (0 if not known)
	IMG_VOID *		pui32Memory;		//!< A pointer to the device memory - IMG_NULL if not defined
	IMG_UINT32 		ui32MemSize;		//!< The size of the device memory (0 if not known)
	IMG_UINT32		ui32DevMemoryBase;	//!<The base address of the device's view of memory
	
} DEVIF_sDevMapping;

/*!
******************************************************************************

 @Function				DEVIF_pfnGetDeviceMapping

 @Description

 Not Required

 This is the function prototype for the get access to the device mapping;
 the mapping of registers, memory etc. into the host memory map.

 @Input     psDeviceCB		: A pointer to the device control block.
 @Input		psDevMapping	: A pointer used to return the mapping information.

 @Return	None

******************************************************************************/
typedef IMG_VOID ( * DEVIF_pfnGetDeviceMapping) (
	DEVIF_sDeviceCB	*			psDeviceCB,
	DEVIF_sDevMapping *			psDevMapping
);

/*!
******************************************************************************
 The device information structure
  All not required (req) functions can be left as NULL
******************************************************************************/
struct DEVIF_tag_sDeviceCB
{
    IMG_BOOL                    bInitialised;				//!< Indicates if the device is initialised
    DEVIF_pfnInitailise         pfnInitailise;				//!< Pointer to device initialisation function (req)
    DEVIF_pfnReadRegister       pfnReadRegister;			//!< Pointer to device read register function (req)
    DEVIF_pfnWriteRegister      pfnWriteRegister;			//!< Pointer to device write register function (req)
    DEVIF_pfnReadRegister64     pfnReadRegister64;			//!< Pointer to device read 64bit register function
    DEVIF_pfnWriteRegister64    pfnWriteRegister64;			//!< Pointer to device write 64bit register function
    DEVIF_pfnReadMemory         pfnReadMemory;				//!< Pointer to read a word from device memory function
    DEVIF_pfnWriteMemory        pfnWriteMemory;				//!< Pointer to write a word from device memory function
	DEVIF_pfnReadMemory64       pfnReadMemory64;			//!< Pointer to read 64bit word from device memory function
    DEVIF_pfnWriteMemory64      pfnWriteMemory64;			//!< Pointer to write 64bit word from device memory function
	DEVIF_pfnCopyDeviceToHost   pfnCopyDeviceToHost;		//!< Pointer to copy device to host memory function (req)
    DEVIF_pfnCopyHostToDevice   pfnCopyHostToDevice;		//!< Pointer to copy host to device memory function (req)
    DEVIF_pfnMallocMemory       pfnMallocMemory;			//!< Pointer to a device malloc memory function where the address is supplied in the function
	DEVIF_pfnMallocHostVirt		pfnMallocHostVirt;			//!< Pointer to a Function used to allocate memory and supply a host virtual address
    DEVIF_pfnFreeMemory         pfnFreeMemory;				//!< Pointer to a device free memory function
    DEVIF_pfnIdle				pfnIdle;					//!< Pointer to a device idle function
	DEVIF_pfnAutoIdle			pfnAutoIdle;				//!< Pointer to a function to set the auto idle parameter (time slave idles for if no command is waiting)
	DEVIF_pfnSendComment		pfnSendComment;				//!< Pointer to a send comment function
	DEVIF_pfnDevicePrint		pfnDevicePrint;				//!< Pointer to an device print function
	DEVIF_pfnGetDeviceTime		pfnGetDeviceTime;			//!< Pointer to a get device time function
	DEVIF_pfnRegWriteWords		pfnRegWriteWords;			//!< Pointer to a writewords function only for registers
	DEVIF_pfnDeinitailise       pfnDeinitailise;			//!< Pointer to a device de-initialisation function
	DEVIF_pfnGetDeviceMapping   pfnGetDeviceMapping;		//!< Pointer to a device a function to get the device mapping
    IMG_VOID *					pvDevIfInfo;				//!< A pointer free for use by the DEVIF code. If required this can be used to point to some additional context
    DEVIF_sFullInfo				sDevIfSetup;				//!< Details of device setup supplied to devif
};

/*!
******************************************************************************
* @brief This function is used to configure the device access functions by filling in
* the elements of the DEVIF_sDeviceCB structure.
* The bInitialised element should be set to IMG_FALSE and used by the
* DEVIF_pfnInitailise() function to determine whether the device has been initialised.
* 
* The DEVIF_ConfigureDevice() function should then setup the pointers to the
* device access routines.  In a device interface which supports multiple devices
* (for example, an interface to multiple simulator libraries) then the pszDevName
* can used be used to device and appropriate access functions.
* @Input		psDeviceCB      : A pointer to the device control block.
******************************************************************************/
IMG_VOID DEVIF_ConfigureDevice(DEVIF_sDeviceCB *   psDeviceCB);


//!@name special functions
//!@{ These are extra functions used to specific customisations, they are not required for an
//! implementation of this API

/*!
******************************************************************************

 @Function				DEVIF1_ConfigureDevice

 @Description

 This is an alternative defintion of DEVIF_ConfigureDevice() and is used
 when there is a need to have DEVIF_ConfigureDevice() defined at one level
 and yet still access the "normal" DEVIF_ConfigureDevice() - renamed to
 DEVIF1_ConfigureDevice() at another level.  Normally used in conjuctions
 with the macro DEVIF_DEFINE_DEVIF1_FUNCS

******************************************************************************/
extern IMG_VOID 
DEVIF1_ConfigureDevice(
    DEVIF_sDeviceCB *   psDeviceCB);

/*!
******************************************************************************
* @brief This function is used to copy absolute device to host function.
* Absolute, because the device is not specified.  This function is used by the
* register/memory access tool "regtool" to examine memory.  It is up to the
* device interface to determine how this maps to device memory...
* NOTE: This function is only needed if the "regtool" is to be supported.
* @Input     ui64AbsAddress  : The absolute memory address.
* @Input     pvHostAddress : The host address.
* @Input     ui32Size    : The size in bytes.
******************************************************************************/
extern IMG_VOID
DEVIF_CopyAbsToHost (
    IMG_UINT64          ui64AbsAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
);

/*!
******************************************************************************
 @brief This function is used to fill absolute device memory with a specified byte
 value. Absolute, because the device is not specified. This function is used
 for pre-initialising memory while debugging. It is up to the device interface
 to determine how this maps to device memory.
 @Input     ui64AbsAddress : The absolute memory address.
 @Input     ui8Value : The fill value.
 @Input     ui32Size : The number of bytes to fill.
******************************************************************************/
IMG_VOID DEVIF_FillAbs (
    IMG_UINT64          ui64AbsAddress,
    IMG_UINT8           ui8Value,
    IMG_UINT32          ui32Size
);

/*!
******************************************************************************
* @brief This function is used to output the burn-mem information
* NOTE: This function is only needed for the PdumpPlayer when used to obtain
* the burn-mem info.
* @param     ui32BurnmemSize : The size to be allocated.
******************************************************************************/
IMG_VOID BMEM_BurnMemInfo (IMG_UINT32 ui32BurnmemSize);
#define DEVIF_BurnMemInfo BMEM_BurnMemInfo

/*!
******************************************************************************
* @brief This function is used to output debug information for each command
* @warning ** This function must be run after the control block has been set up
* @param     psDeviceCB : The device control block to be modified
******************************************************************************/
IMG_VOID DEVIF_AddDebugOutput ( DEVIF_sDeviceCB *   psDeviceCB );
//!@} special functions

//!@name virtual / physical address map
//!@{ functions used to store and find virtual addresses using phyical addresses
//! This is used when the linux kernel driver is doing the memory allocations

//! @brief keep track of mapping between physical address of a page and virtual address
#ifdef WIN32
extern IMG_BOOL
DEVIF_AddPhysToVirtEntry(IMG_UINT64 phys, IMG_UINT32 uiSize ,IMG_VOID * virt);	// In Windows allocation sizes vary.
#else
extern IMG_BOOL
DEVIF_AddPhysToVirtEntry(IMG_UINT64 phys, IMG_VOID * virt); // In linux all allocations are of a single page.
#endif

//! @brief removes a virtual address from the search structure
IMG_VOID DEVIF_RemovePhysToVirtEntry(IMG_UINT64 phys);

//! @brief finds a virtual address from a physical one
IMG_VOID * DEVIF_FindPhysToVirtEntry(IMG_UINT64 phys);

//!@} virtual / physical address map

#if defined (__cplusplus)
}
#endif

#endif /* __DEVIF_API_H__	*/



