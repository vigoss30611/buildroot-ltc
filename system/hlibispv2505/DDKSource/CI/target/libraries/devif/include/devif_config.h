/*!
******************************************************************************
 @file   : devif_config.h

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
         configure functions for a range of interfaces for the wrapper for PDUMP tools.

 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     0.1

******************************************************************************/
/*
******************************************************************************
*/


#if !defined (__DEVICECONFIG_H__)
#define __DEVICECONFIG_H__

#include <devif_api.h>

#if (__cplusplus)
extern "C" {
#endif


/*!
******************************************************************************

 @Function				PCIIF_DevIfConfigureDevice

 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID
PCIIF_DevIfConfigureDevice(
    DEVIF_sDeviceCB *		psDeviceCB
);


/*!
******************************************************************************

 @Function				BMEM_DevIfConfigureDevice
  
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID
BMEM_DevIfConfigureDevice(
    DEVIF_sDeviceCB *   psDeviceCB
);


/*!
******************************************************************************

 @Function				DASHIF_DevIfConfigureDevice
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID
DASHIF_DevIfConfigureDevice(
    DEVIF_sDeviceCB *   psDeviceCB);


/*
******************************************************************************

 @Function				PDUMP1IF_DevIfConfigureDevice
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID
PDUMP1IF_DevIfConfigureDevice(
    DEVIF_sDeviceCB *   psDeviceCB);

/*!
******************************************************************************

 @Function				DIRECT_DEVIF_ConfigureDevice
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID
DIRECT_DEVIF_ConfigureDevice(
	DEVIF_sDeviceCB *		psDeviceCB
);


/*!
******************************************************************************

 @Function				POSTED_DEVIF_ConfigureDevice
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID
POSTED_DEVIF_ConfigureDevice(
	DEVIF_sDeviceCB *		psDeviceCB
);

/*!
******************************************************************************

 @Function				DEVIF_DUMMY_ConfigureDevice
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID DEVIF_DUMMY_ConfigureDevice (
	DEVIF_sDeviceCB *		psDeviceCB
);

/*!
******************************************************************************

 @Function				DEVIF_NULL_Setup
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID DEVIF_NULL_Setup (
	DEVIF_sDeviceCB *   psDeviceCB 
);

/*!
******************************************************************************

 @Function				TRANSIF_DEVIF_ConfigureDevice
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID
TRANSIF_DEVIF_ConfigureDevice(
    DEVIF_sDeviceCB *		psDeviceCB
);

/*!
******************************************************************************

 @Function				SIF_DevIfConfigureDevice
 
 @Description 
 
 This function is used to configure the device access functions by filling in
 the elements of the DEVIF_sDeviceCB structure.

 The bInitialised element should be set to IMG_FALSE and used by the 
 DEVIF_pfnInitailise() function to determine whether the device has been initialised.

 The DEVIF_ConfigureDevice() function should then setup the pointers to the
 device access routines.  In a device interface which supports multiple devices
 (for example, an interface to multiple simulator libraries) then the pszDevName 
 can used be used to device and appropriate access functions.

 @Input		psDeviceCB      : A pointer to the device control block.

 @Return	None.

******************************************************************************/
extern IMG_VOID
SIF_DevIfConfigureDevice(
    DEVIF_sDeviceCB *		psDeviceCB
);

/*!
******************************************************************************

 @Function				PCIIF_CopyAbsToHost

 @Description

 This function is used to copy absolute device to host function,
 in the case of a PCI interface.

 NOTE: This function is only needed if the "regtool" is to be supported.

 @Input     ui64AbsAddress  : The absolute memory address.

 @Input     pvHostAddress : The host address.

 @Input     ui32Size    : The size in bytes.

 @Return	None.

******************************************************************************/
extern IMG_VOID PCIIF_CopyAbsToHost (
    IMG_UINT64          ui64AbsAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
);


/*!
******************************************************************************

 @Function				BMEM_CopyAbsToHost

 @Description

 This function is used to copy absolute device to host function,
 in the case of a Burn Mem interface.

 NOTE: This function is only needed if the "regtool" is to be supported.

 @Input     ui64AbsAddress  : The absolute memory address.

 @Input     pvHostAddress : The host address.

 @Input     ui32Size    : The size in bytes.

 @Return	None.

******************************************************************************/
extern IMG_VOID BMEM_CopyAbsToHost (
    IMG_UINT64          ui64AbsAddress,
    IMG_VOID *          pvHostAddress,
    IMG_UINT32          ui32Size
);

/*!
******************************************************************************

 @Function              PCIIF_FillAbs

 @Description

 This function is used to fill absolute device memory with a specified byte
 value, in the case of a PCI interface. This function is used
 for pre-initialising memory while debugging.

 @Input     ui64AbsAddress : The absolute memory address.
 @Input     ui8Value : The fill value.
 @Input     ui32Size : The number of bytes to fill.

 @Return    None.

******************************************************************************/
extern IMG_VOID PCIIF_FillAbs (
    IMG_UINT64          ui64AbsAddress,
    IMG_UINT8           ui8Value,
    IMG_UINT32          ui32Size
);

/*!
******************************************************************************

 @Function              BMEM_FillAbs

 @Description

 This function is used to fill absolute device memory with a specified byte
 value, in the case of a Burn Mem interface. This function is used
 for pre-initialising memory while debugging.

 @Input     ui64AbsAddress : The absolute memory address.
 @Input     ui8Value : The fill value.
 @Input     ui32Size : The number of bytes to fill.

 @Return    None.

******************************************************************************/
extern IMG_VOID BMEM_FillAbs (
    IMG_UINT64          ui64AbsAddress,
    IMG_UINT8           ui8Value,
    IMG_UINT32          ui32Size
);


#if (__cplusplus)
}
#endif

#endif /* __DEVICECONFIG_H__	*/

