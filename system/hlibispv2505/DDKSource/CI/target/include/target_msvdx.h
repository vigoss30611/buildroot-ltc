/*!
******************************************************************************
 @file   : target_msvdx.h

 @brief

 @Author Imagination Technologies

 @date   03/11/2011

         <b>Copyright 2009 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to the third parties without the
         express written permission of Imagination Technologies
         Limited, Home Park Estate, Kings Langley, Hertfordshire,
         WD4 8LZ, U.K.

 \n<b>Description:</b>\n
         This is a configurable header file which sets up the memory
		 spaces for a fixed device.

 \n<b>Platform:</b>\n
         Platform Independent

 @Version
    -   1.0 1st Release

******************************************************************************/

#include <tal_defs.h>

#define TAL_TARGET_NAME "MSVDX"

/*****************************************************************************
	NUMBER OF MEMORY SPACES DEFINED IN THIS FILE
	(This number must be equal to the number of TAL_sMemSpaceInfo structs
	 initialised)
*****************************************************************************/
#define TAL_MEM_SPACE_ARRAY_SIZE 3


/*****************************************************************************
	MEMORY SPACE DEFINITIONS
	1. Memory space name.
	2. Memory space type:
		if register:	TAL_MEMSPACE_REGISTER
		if memory:		TAL_MEMSPACE_MEMORY
	3. Memory space info struct:
		if register:
			3.1. Base address of device registers
			3.2. Size of device register block
			3.3. The interrupt number
				if no interrupt number: TAL_NO_IRQ
		if memory:
			3.1. Base address of memory region
			3.2. Size of memory region
			3.3. Memory guard band
*****************************************************************************/
TAL_sMemSpaceInfo gasMemSpaceInfo [TAL_MEM_SPACE_ARRAY_SIZE] =
{
	{
		"REG_MSVDX_VEC_RAM",		/*1*/
		TAL_MEMSPACE_REGISTER,		/*2*/
		{
            {
                0x00002000,				/*3.1*/
                4096,					/*3.2*/
                TAL_NO_IRQ				/*3.2*/
            }
        }
	},

	{
		"MEMDMAC_02",
		TAL_MEMSPACE_MEMORY,
		{
            {
                0x00000000,
                0x10000000,
                0x000000000
            }
		}
	},

	{
		"REG_MSVDX_VDMC",
		TAL_MEMSPACE_REGISTER,
		{
			{
                0x00000400,
                128,
                TAL_NO_IRQ
            }
		}
	}
};



/*****************************************************************************
	DEVICE DEFINITION (Only to use with normal TAL)
	- Device name
	- Base address of the device registers.
	- Base address of the default memory region.
	- Size of the default memory region.
	- Size of the default memory guard band.
	- Handle to the memory allocation context
	- Device control block for devif_api
	- Name of the memory arena for the device
	- Device Flags
	- Sub-Device list
	- Gets set when the device is configured
*****************************************************************************/
#ifndef TAL_LIGHT
TAL_sDeviceInfo	sDevInfo =
{
	"MSVDX",
	0,
	0,
	0x10000000,
	0,
	IMG_NULL,
	IMG_NULL,
	IMG_NULL,
	0x00000000,
	IMG_NULL,
	IMG_FALSE
};
#endif
