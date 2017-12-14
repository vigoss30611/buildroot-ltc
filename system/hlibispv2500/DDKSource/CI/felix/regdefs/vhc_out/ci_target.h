/**
*******************************************************************************
 @file ci_target.h

 @brief Configuration of the TAL light for the Felix HW

 @copyright Imagination Technologies Ltd. All Rights Reserved.

 @license Strictly Confidential.
   No part of this software, either material or conceptual may be copied or
   distributed, transmitted, transcribed, stored in a retrieval system or
   translated into any human or computer language in any form by any means,
   electronic, mechanical, manual or other-wise, or disclosed to third
   parties without the express written permission of
   Imagination Technologies Limited,
   Unit 8, HomePark Industrial Estate,
   King's Langley, Hertfordshire,
   WD4 8LZ, U.K.

******************************************************************************/
#ifndef CI_TARGET_H_
#define CI_TARGET_H_

#include <tal_defs.h>  // NOLINT

#define TAL_TARGET_NAME "FELIX"

/** @brief number of memory spaces defined in this file */
#define TAL_MEM_SPACE_ARRAY_SIZE \
    (sizeof(gasMemSpaceInfo)/sizeof(gasMemSpaceInfo[0]))

#ifndef INFOTM_ISP
#define NO_IRQ TAL_NO_IRQ
#endif //INFOTM_ISP
/*
 * doxygen sublist: 4 spaces before - or -# and last line is a .
 */
/**
 * @brief The definition of the memory space of the device
 *
 * Memory space definition:
 * -# Memory space name.
 * -# Memory space type:
 * @li if register: TAL_MEMSPACE_REGISTER
 * @li if memory: TAL_MEMSPACE_MEMORY
 * -# Memory space info struct:
 * @li if register:
 *    -# Base address of device registers
 *    -# Size of device register block
 *    -# The interrupt number. If no interrupt number: TAL_NO_IRQ
 *    .
 * @li if memory:
 *    -# Base address of memory region
 *    -# Size of memory region
 *    -# Memory guard band
 *    .
 */
static TAL_sMemSpaceInfo gasMemSpaceInfo[] =
{
    {
        "REG_FELIX_CORE",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x0000000000,  /* Base address of device registers */
            0x00000007FF,  /* Size of device register block */
            0  /* The interrupt number - or TAL_NO_IRQ */
        	}
		}
    },

    {
        "REG_FELIX_GMA_LUT",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x0000000C00,  /* Base address of device registers */
            0x00000003FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */ /// @ change when IRQ number is known
        	}
		}
    },

    // contexts
    {
        "REG_FELIX_CONTEXT_0",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type: */
        {
			{
            0x0000001000,  /* Base address of device registers */
            0x0000000FFF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },

    {
        "REG_FELIX_CONTEXT_1",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type: */
        {
			{
            0x0000002000,  /* Base address of device registers */
            0x0000000FFF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },

    // MMU inside Felix
    {
        "REG_FELIX_MMU",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x0000009000,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
            0  /* The interrupt number - or TAL_NO_IRQ */
        	}
		}
    },

    // gaskets receiving data from external DG/imagers
    {
        "REG_FELIX_GASKET_0",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000C000,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },
    {
        "REG_FELIX_GASKET_1",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000C200,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },
    {
        "REG_FELIX_GASKET_2",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000C400,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },
    {
        "REG_FELIX_GASKET_3",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000C600,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },

    // internal data generator, including internal gasket connected to it
    {
        "REG_FELIX_DG_IIF_0",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000A000,  /* Base address of device registers */
            0x00000000FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },

    // SCB (I2C)
    {
        "REG_FELIX_SCB",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000B000,  /* Base address of device registers */
            0x00000000FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },

    // external data generators (outside Felix, debug only)
    {
        "REG_TEST_DG_0",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000E000,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */ /// @ change when IRQ number is known
        	}
		}
    },

    {
        "REG_TEST_DG_1",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000E200,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */ /// @ change when IRQ number is known
        	}
		}
    },

    {
        "REG_TEST_DG_2",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000E400,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */ /// @ change when IRQ number is known
        	}	
		}
    },

    {
        "REG_TEST_DG_3",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x000000E600,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */ /// @ change when IRQ number is known
        	}
		}
    },

    // external MMU for external data generators (outside Felix, debug only)
    {
        "REG_TEST_MMU",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x0000010000,  /* Base address of device registers */
            0x00000001FF,  /* Size of device register block */
            0  /* The interrupt number - or TAL_NO_IRQ */
        	}
		}
    },

    // external Test IO registers
    {
        "REG_TEST_IO",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x0000012000,  /* Base address of device registers */
            0x00000000FF,  /* Size of device register block */
            0  /* The interrupt number - or TAL_NO_IRQ */
        	}
		}
    },

    // PDP registers - offsets for FAKE interface
    {
        "REG_FPGA_BAR0_PDP",  /* Memory space name */
        TAL_MEMSPACE_REGISTER,  /* Memory space type */
        {
			{
            0x0000020000,  /* Base address of device registers */
            0x000000FFFF,  /* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
        	}
		}
    },

    {
        "SYSMEM",  /* Memory space name */
        TAL_MEMSPACE_MEMORY,  /* Memory space type */
        {
			{
            0x0000000000,  /* Base address of memory region */
            0x10000000,  /* Size of memory region - 256MB */
            0x0000000000  /* Memory guard band */
        	}
    	}
	}
};

#ifdef TAL_NORMAL
/**
 * @brief Configure a device structure for TCP/IP (Only to use with normal TAL)
 */
void setDeviceInfo_IP(TAL_sDeviceInfo *pDevInfo, IMG_UINT32 port,
    const char *pszIP)
{
    IMG_MEMSET(pDevInfo, 0, sizeof(TAL_sDeviceInfo));
    pDevInfo->pszDeviceName = TAL_TARGET_NAME;
    pDevInfo->ui64MemBaseAddr = 0x0000000000;
    pDevInfo->ui64DefMemSize = 0x10000000;
    pDevInfo->ui64MemGuardBand = 0x0000000000;

    // if TCP/IP
    pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType =
        DEVIF_TYPE_SOCKET;
    pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.\
        ui32HostTargetCycleRatio = 20000;
    pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszDeviceName =
        TAL_TARGET_NAME;
    pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.ui32PortId = port;
    pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszRemoteName =
        (char*)pszIP;
    pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.bUseBuffer = IMG_TRUE;
}
#endif

#endif /* CI_TARGET_H_ */
