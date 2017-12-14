/**
******************************************************************************
 @file tal_test.h

 @brief The TAL configuration structure and Base test class

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
#include <img_types.h>
#include <img_defs.h>
#include <tal_defs.h>

#ifndef TAL_TEST_H
#define TAL_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#define TAL_TARGET_NAME "MMUDEV"

#define TAL_MEM_SPACE_ARRAY_SIZE 2
#define TAL_DEV_OFF 0x00800000
#define TAL_DEV_DEF_MEM_SIZE 0x10000000
#define TAL_DEV_DEF_MEM_GUARD 0x00000000
#define TAL_REGBANK "REGBANK"
#define TAL_DEVMEM "DEVMEM"

static TAL_sMemSpaceInfo gasMemspace [TAL_MEM_SPACE_ARRAY_SIZE] =
{
	{
		TAL_REGBANK,				/* Memory space name */
		TAL_MEMSPACE_REGISTER,	/* Memory space type: TAL_MEMSPACE_REGISTER or TAL_MEMSPACE_MEMORY */
		{
			0x00000000,				/* Base address of device registers */
			0x10000000,				/* Size of device register block */
			TAL_NO_IRQ			/* The interrupt number - if no interrupt number: TAL_NO_IRQ */
		}
	},
	{
		TAL_DEVMEM,				/* Memory space name */
		TAL_MEMSPACE_MEMORY,	/* Memory space type: TAL_MEMSPACE_REGISTER or TAL_MEMSPACE_MEMORY */
		{
			0x10000000,			/* Base address of memory region */
			0x20000000,			/* Size of memory region - 512MB */
			0x00000000			/* Memory guard band */
		}
	}
};

#ifdef TAL_NORMAL

/**
 * @brief Configure the NULL interface
 */
void setDeviceInfo_NULL(TAL_sDeviceInfo *pDevInfo);

/**
 * @brief Configure a device structure for TCP/IP
 */
void setDeviceInfo_IP(TAL_sDeviceInfo *pDevInfo, IMG_UINT32 port, const char *pszIP);

#endif

#ifdef __cplusplus
}

// some unit test specific stuff

#include <gtest/gtest.h>

/**
 * @brief Base class that can be used to create GTest classes
 */
class TALBase
{
protected:
	/**
	 * @brief Call once create and initialised to populate the tal handles
	 */
	virtual void init(TAL_sMemSpaceInfo *aInfo, size_t nElem);

public:
	IMG_HANDLE *aRegBanks; // array
	size_t nRegBanks;
	IMG_HANDLE *aMemory; // array
	size_t nMemory;

	TAL_sDeviceInfo sDeviceInfo;
	
	TALBase(eDevifTypes eInterface = DEVIF_TYPE_NULL);
	virtual ~TALBase();

	/**
	 * @brief Initialise TAL with a structure
	 */
	virtual IMG_RESULT initialise(TAL_sMemSpaceInfo *aInfo = gasMemspace, size_t nElem = TAL_MEM_SPACE_ARRAY_SIZE);
	/** 
	 * @brief Initialise TAL with config file
	 */
	virtual IMG_RESULT initialise(const char *pszFilename);

	virtual IMG_RESULT finalise();
};

#endif // __cplusplus

#endif // TAL_TEST_H
