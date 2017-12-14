/*! 
******************************************************************************
 @file   : sgxmmu.h

 @brief  

 @Author Ray Livesley

 @date   28/11/2006
 
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
         SGXMMU interface for the wrapper for PDUMP tools..

 <b>Platform:</b>\n
	     Platform Independent 

 @Version	
	     1.0 

******************************************************************************/
/* 
******************************************************************************
*/

#if !defined (__SGXMMU_H__)
#define __SGXMMU_H__

#include <tconf.h>

#if (__cplusplus)
extern "C" {
#endif



typedef enum
{
	/*! SGX535/540 type MMU table											*/
	SGXMMU_TYPE_SGX535				= 0x00000001,
	/*! SGX530 type MMU table												*/
	SGXMMU_TYPE_SGX530				= 0x00000002

} SGXMMU_eSGXMMUType;



/*!
******************************************************************************

 @Function				SGXMMU_Initialise
 
 @Description 
 
 This function is used to initilaise the SGX MMU code.
 
 @Input	    None.

 @Return	None.

******************************************************************************/
extern IMG_VOID SGXMMU_Initialise();


/*!
******************************************************************************

 @Function				SGXMMU_SetUpTranslation
 
 @Description 
 
 This function is used to set the device/bank qualifier.
 
 @Input	    psDeviceCB      : A pointer to the devices TCONF_sDeviceCB structure.

 @input     eSGXMMUType             : SGX MMU type.

 @Input	    ui32DevVirtQualifier    : Device/bank qualifier.

 @Return	None.

******************************************************************************/
extern IMG_VOID SGXMMU_SetUpTranslation(
    TAL_psMemSpaceInfo		psMemSpaceInfo, 
    SGXMMU_eSGXMMUType      eSGXMMUType,  
    IMG_UINT32              ui32DevVirtQualifier);


/*!
******************************************************************************

 @Function				SGXMMU_CoreTranslation
 
 @Description 
 
 This function is used to convert a virtual address to a physical address.
 
 @Input	    psDeviceCB      : A pointer to the devices TCONF_sDeviceCB structure.

 @input     eSGXMMUType     : SGX MMU type.
 
 @Input	    uInAddr         : Physical address.

 @Return	None.

******************************************************************************/
extern IMG_UINT32 SGXMMU_CoreTranslation(
    TAL_psMemSpaceInfo		psMemSpaceInfo, 
    SGXMMU_eSGXMMUType      eSGXMMUType, 
    IMG_UINT32              uInAddr);

#if (__cplusplus)
}
#endif
 
#endif /* __SGXMMU_H__	*/


