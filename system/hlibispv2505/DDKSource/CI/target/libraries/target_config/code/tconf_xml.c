/*!
********************************************************************************
 @file   : tconf_xml.c

 @brief

 @Author Jose Massada

 @date   03/10/2012

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
         Functions used to read a Target Config File in XML format.

 <b>Platform:</b>\n
	     Platform Independent
		  
 @Version
	     1.0

*******************************************************************************/
#include <setjmp.h>
#include "mxml.h"
#include "tconf_int.h"
#include "tconf_xml.h"
#include "img_errors.h"

#ifdef _MSC_VER
#define strtoll _strtoi64
#define strtoull _strtoui64
#define strcasecmp stricmp
#endif /* _MSC_VER */


#define TCONF_XML_NOT_ENOUGH_MEM		(1)
#define TCONF_XML_PARSING_FAILED		(2)
#define TCONF_XML_UNDEFINED_ATTR		(3)
#define TCONF_XML_UNDEFINED_ELEMENT		(4)
#define TCONF_XML_FAILED_CONVERTION		(5)
#define TCONF_XML_INVALID_VALUE			(6)
#define TCONF_XML_DUPLICATE_ELEMENT		(7)
#define TCONF_XML_UNEXPECTED_ELEMENT	(8)

typedef struct TCONF_XMLConf_tag
{
	DEVIF_sWrapperControlInfo sWrapperControlInfo;	// Devif Setup
	eTAL_Twiddle eMemTwiddle;						// Memory Twiddle Algorithm for > 32bit testing on FPGA / Emulator#
	IMG_UINT32 nDevFlags;							// Device Setup Flags
	IMG_BOOL	bVerifyMemWrites;					// Set to verify memory writes
} TCONF_XMLConf;

static TCONF_XMLConf _xmlConf = { 0 };
static jmp_buf _jmpBuffer;

/* 
 * tconf_xml_GetAttrAs<Boolean | Integer | UnsignedInteger | String> functions
 * return the attribute value or the third parameter (xDefault) if not found. If
 * the last parameter (bRequired) is set and the attribute is not found it will
 * fail and longjmp to the error handling location.
 *
 * tconf_xml_GetAttrAs<Integer | UnsignedInteger> will longjmp if the conversion
 * from string to number fails.
 */
static IMG_BOOL tconf_xml_GetAttrAsBoolean(mxml_node_t *pNode, const IMG_CHAR *pszAttrName, IMG_BOOL bDefault, IMG_BOOL bRequired)
{
	IMG_BOOL bValue = bDefault;
	const IMG_CHAR *pszValue;

	pszValue = mxmlElementGetAttr(pNode, pszAttrName);
	if (pszValue != NULL && *pszValue)
	{
		if (strcmp(pszValue, "0") == 0 || strcasecmp(pszValue, "off") == 0 || strcasecmp(pszValue, "no") == 0)
			bValue = IMG_FALSE;
		else
			bValue = IMG_TRUE;
	}
	else if (bRequired)
	{
		printf("ERROR: expected attribute '%s' not defined in element '%s'\n", pszAttrName, mxmlGetElement(pNode));
		longjmp(_jmpBuffer, TCONF_XML_UNDEFINED_ATTR);
	}
	return bValue;
}

/*
 * Unused functions. Might be useful in the future.
 *
static IMG_INT32 tconf_xml_GetAttrAsInt32(mxml_node_t *pNode, const IMG_CHAR *pszAttrName, IMG_INT32 nDefault, IMG_BOOL bRequired)
{
	IMG_INT32 nValue = nDefault;
	const IMG_CHAR *pszValue;

	pszValue = mxmlElementGetAttr(pNode, pszAttrName);
	if (pszValue != NULL && *pszValue)
	{
		IMG_CHAR *pszEnd;

		errno = 0;
		nValue = strtol(pszValue, &pszEnd, 0);
		if (errno != 0)
		{
			printf("ERROR: failed to convert attribute '%s' to 32-bits integer\n", pszAttrName);
			longjmp(_jmpBuffer, TCONF_XML_FAILED_CONVERTION);
		}
	}
	else if (bRequired)
	{
		printf("ERROR: expected attribute '%s' not defined in element '%s'\n", pszAttrName, mxmlGetElement(pNode));
		longjmp(_jmpBuffer, TCONF_XML_UNDEFINED_ATTR);
	}
	return nValue;
}

static IMG_INT64 tconf_xml_GetAttrAsInt64(mxml_node_t *pNode, const IMG_CHAR *pszAttrName, IMG_INT64 nDefault, IMG_BOOL bRequired)
{
	IMG_INT64 nValue = nDefault;
	const IMG_CHAR *pszValue;

	pszValue = mxmlElementGetAttr(pNode, pszAttrName);
	if (pszValue != NULL && *pszValue)
	{
		IMG_CHAR *pszEnd;

		errno = 0;
		nValue = strtoll(pszValue, &pszEnd, 0);
		if (errno != 0)
		{
			printf("ERROR: failed to convert attribute '%s' to 64-bits integer\n", pszAttrName);
			longjmp(_jmpBuffer, TCONF_XML_FAILED_CONVERTION);
		}
	}
	else if (bRequired)
	{
		printf("ERROR: expected attribute '%s' not defined in element '%s'\n", pszAttrName, mxmlGetElement(pNode));
		longjmp(_jmpBuffer, TCONF_XML_UNDEFINED_ATTR);
	}
	return nValue;
}
*/

static IMG_UINT32 tconf_xml_GetAttrAsUnsignedInt32(mxml_node_t *pNode, const IMG_CHAR *pszAttrName, IMG_UINT32 nDefault, IMG_BOOL bRequired)
{
	IMG_UINT32 nValue = nDefault;
	const IMG_CHAR *pszValue;

	pszValue = mxmlElementGetAttr(pNode, pszAttrName);
	if (pszValue != NULL && *pszValue)
	{
		IMG_CHAR *pszEnd;

		errno = 0;
		nValue = (IMG_UINT32)strtoul(pszValue, &pszEnd, 0);
		if (errno != 0)
		{
			printf("ERROR: failed to convert attribute '%s' to 32-bits unsigned integer\n", pszAttrName);
			longjmp(_jmpBuffer, TCONF_XML_FAILED_CONVERTION);
		}
	}
	else if (bRequired)
	{
		printf("ERROR: expected attribute '%s' not defined in element '%s'\n", pszAttrName, mxmlGetElement(pNode));
		longjmp(_jmpBuffer, TCONF_XML_UNDEFINED_ATTR);
	}
	return nValue;
}

static IMG_UINT64 tconf_xml_GetAttrAsUnsignedInt64(mxml_node_t *pNode, const IMG_CHAR *pszAttrName, IMG_UINT64 nDefault, IMG_BOOL bRequired)
{
	IMG_INT64 nValue = nDefault;
	const IMG_CHAR *pszValue;

	pszValue = mxmlElementGetAttr(pNode, pszAttrName);
	if (pszValue != NULL && *pszValue)
	{
		IMG_CHAR *pszEnd;

		errno = 0;
		nValue = strtoull(pszValue, &pszEnd, 0);
		if (errno != 0)
		{
			printf("ERROR: failed to convert attribute '%s' to 64-bits unsigned integer\n", pszAttrName);
			longjmp(_jmpBuffer, TCONF_XML_FAILED_CONVERTION);
		}
	}
	else if (bRequired)
	{
		printf("ERROR: expected attribute '%s' not defined in element '%s'\n", pszAttrName, mxmlGetElement(pNode));
		longjmp(_jmpBuffer, TCONF_XML_UNDEFINED_ATTR);
	}
	return nValue;
}

static const IMG_CHAR *tconf_xml_GetAttrAsString(mxml_node_t *pNode, const IMG_CHAR *pszAttrName, const IMG_CHAR *pszDefault, IMG_BOOL bRequired)
{
	const IMG_CHAR *pszValue;

	pszValue = mxmlElementGetAttr(pNode, pszAttrName);
	if (pszValue != NULL && *pszValue)
	{
		return pszValue;
	}
	else if (bRequired)
	{
		printf("ERROR: expected attribute '%s' not defined in element '%s'\n", pszAttrName, mxmlGetElement(pNode));
		longjmp(_jmpBuffer, TCONF_XML_UNDEFINED_ATTR);
	}
	return pszDefault;
}

static IMG_RESULT tconf_xml_ProcessGlobalOptions(mxml_node_t *pRoot)
{
	mxml_node_t *pNode;

	/* Get element */
	pNode = mxmlFindElement(pRoot, pRoot, "options", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		const IMG_CHAR *pszValue;
		IMG_UINT32 nValue;

		/* Memory allocation (default: user) */
		pszValue = tconf_xml_GetAttrAsString(pNode, "mem_alloc", "user", IMG_FALSE);

		if (strcasecmp(pszValue, "user") == 0)
			
			nValue = TAL_DEVFLAG_SEQUENTIAL_ALLOC;
		else if (strcasecmp(pszValue, "4kfixed") == 0)
			nValue = TAL_DEVFLAG_4K_PAGED_ALLOC;
		else if (strcasecmp(pszValue, "4krandom") == 0)
			nValue = TAL_DEVFLAG_4K_PAGED_RANDOM_ALLOC;
		else if (strcasecmp(pszValue, "randomblock") == 0)
			nValue = TAL_DEVFLAG_RANDOM_BLOCK_ALLOC;
		else if (strcasecmp(pszValue, "random") == 0)
			nValue = TAL_DEVFLAG_TOTAL_RANDOM_ALLOC;
		else if (strcasecmp(pszValue, "device") == 0)
			nValue = TAL_DEVFLAG_DEV_ALLOC;
		else
		{
			printf("ERROR: invalid memory allocation value \"%s\"\n", pszValue);
			longjmp(_jmpBuffer, TCONF_XML_INVALID_VALUE);
		}

		_xmlConf.nDevFlags |= (nValue << TAL_DEVFLAG_MEM_ALLOC_SHIFT) & TAL_DEVFLAG_MEM_ALLOC_MASK;

		/* SGX virtual memory support (default: off) */
		pszValue = tconf_xml_GetAttrAsString(pNode, "sgx_virtual_mem", "off", IMG_FALSE);

		if (strcasecmp(pszValue, "off") == 0)
			nValue = TAL_DEVFLAG_NO_SGX_VIRT_MEMORY;
		else if (strcasecmp(pszValue, "sgx535") == 0)
			nValue = TAL_DEVFLAG_SGX535_VIRT_MEMORY;
		else if (strcasecmp(pszValue, "sgx530") == 0)
			nValue = TAL_DEVFLAG_SGX530_VIRT_MEMORY;
		else
		{
			printf("ERROR: invalid SGX virtual memory support value \"%s\"\n", pszValue);
			longjmp(_jmpBuffer, TCONF_XML_INVALID_VALUE);
		}

		_xmlConf.nDevFlags |= (nValue << TAL_DEVFLAG_SGX_VIRT_MEM_SHIFT) & TAL_DEVFLAG_SGX_VIRT_MEM_MASK;

		/* Memory twiddle algorithm (default: off) */
		pszValue = tconf_xml_GetAttrAsString(pNode, "mem_twiddle", "off", IMG_FALSE);

		if (strcasecmp(pszValue, "off") == 0)
		{
			_xmlConf.eMemTwiddle = TAL_TWIDDLE_NONE;	
		}
		else
		{
			if (strcasecmp(pszValue, "orig") == 0)
				_xmlConf.eMemTwiddle = TAL_TWIDDLE_36BIT_OLD;
			else if (strcasecmp(pszValue, "36bit") == 0)
				_xmlConf.eMemTwiddle = TAL_TWIDDLE_36BIT;
			else if (strcasecmp(pszValue, "40bit") == 0)
				_xmlConf.eMemTwiddle = TAL_TWIDDLE_40BIT;
			else if (strcasecmp(pszValue, "drop32") == 0)
				_xmlConf.eMemTwiddle = TAL_TWIDDLE_DROPTOP32;
			else
			{
				printf("ERROR: invalid SGX virtual memory support value \"%s\"\n", pszValue);
				longjmp(_jmpBuffer, TCONF_XML_INVALID_VALUE);
			}
		}
		
		/* Memory write check (default: false) */
		_xmlConf.bVerifyMemWrites = tconf_xml_GetAttrAsBoolean(pNode, "mem_write_check", IMG_FALSE, IMG_FALSE);

		/* Host target cycle ratio */
		_xmlConf.sWrapperControlInfo.ui32HostTargetCycleRatio = tconf_xml_GetAttrAsUnsignedInt32(pNode, "host_target_cycle_ratio", 0, IMG_TRUE);

		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pRoot, "options", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate element 'options' found in 'target_config'\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return IMG_SUCCESS;
}

static IMG_RESULT tconf_xml_ProcessDeviceOptions(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDevice, const IMG_CHAR* pszName)
{
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get element */
	pNode = mxmlFindElement(pDevice, pDevice, "options", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		IMG_UINT32 nDevFlags;

		/* Assign global flags (memory allocation and SGX virtual memory support) to device */
		/* These are parsed in the global options (tconf_xml_ProccessGlobalOptions) */
		nDevFlags = _xmlConf.nDevFlags;

		/* Device stub out (default: no) */
		if (tconf_xml_GetAttrAsBoolean(pNode, "stub_out", IMG_FALSE, IMG_FALSE))
			nDevFlags |= TAL_DEVFLAG_STUB_OUT;

		/* Device coalesce loads (default: no) */
		if (tconf_xml_GetAttrAsBoolean(pNode, "coalesce_loads", IMG_FALSE, IMG_FALSE))
			nDevFlags |= TAL_DEVFLAG_COALESCE_LOADS;
        
        /* Device skip page faults (default: no) */
		if (tconf_xml_GetAttrAsBoolean(pNode, "skip_page_faults", IMG_FALSE, IMG_FALSE))
			nDevFlags |= TAL_DEVFLAG_SKIP_PAGE_FAULTS;

		/* Set the flags */
		rResult = TCONF_SetDeviceFlags(psTargetConf, pszName, nDevFlags);

		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pDevice, "options", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate element 'options' found in 'device'\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessDeviceDefaultMem(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDevice, const IMG_CHAR *pszName)
{
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get element */
	pNode = mxmlFindElement(pDevice, pDevice, "default_mem", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		IMG_UINT64 nBaseAddr, nSize, nGuardBand;

		/* Default memory base address */
		nBaseAddr = tconf_xml_GetAttrAsUnsignedInt64(pNode, "base_addr", 0, IMG_TRUE);

		/* Default memory size */
		nSize = tconf_xml_GetAttrAsUnsignedInt64(pNode, "size", 0, IMG_TRUE);

		/* Default memory guard band */
		nGuardBand = tconf_xml_GetAttrAsUnsignedInt64(pNode, "guard_band", 0, IMG_TRUE);

		rResult = TCONF_SetDeviceDefaultMem(psTargetConf, pszName, nBaseAddr, nSize, nGuardBand);

		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pDevice, "default_mem", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate element 'default_mem' found in 'device'\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessDeviceRegisters(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDevice, const IMG_CHAR* pszDevName)
{
	const IMG_CHAR *pszIrq, *pszName;
	IMG_UINT32 nSize, nIrq;
	IMG_UINT64 nOffset;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get first element */
	pNode = mxmlFindElement(pDevice, pDevice, "register", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* Register name */
		pszName = tconf_xml_GetAttrAsString(pNode, "name", NULL, IMG_TRUE);

		/* Register offset */
		nOffset = tconf_xml_GetAttrAsUnsignedInt64(pNode, "offset", 0, IMG_TRUE);

		/* Register size */
		nSize = tconf_xml_GetAttrAsUnsignedInt32(pNode, "size", 0, IMG_TRUE);

		/* Register IRQ (default: null) */
		pszIrq = tconf_xml_GetAttrAsString(pNode, "irq", NULL, IMG_FALSE);
		if (pszIrq == NULL)
			nIrq = TAL_NO_IRQ;
		else
			nIrq = tconf_xml_GetAttrAsUnsignedInt32(pNode, "irq", 0, IMG_TRUE);

		/* Add to device */
		rResult = TCONF_AddDeviceRegister(psTargetConf, pszDevName, pszName, nOffset, nSize, nIrq);
		
		/* Get next element */
		pNode = mxmlFindElement(pNode, pDevice, "register", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessDeviceMemory(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDevice, const IMG_CHAR* pszDevName)
{
	IMG_UINT64 nBaseAddr, nSize, nGuardBand;
	const IMG_CHAR *pszName;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	pNode = mxmlFindElement(pDevice, pDevice, "memory", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* Memory name */
		pszName = tconf_xml_GetAttrAsString(pNode, "name", NULL, IMG_TRUE);

		/* Memory base address (default: 0) */
		nBaseAddr = tconf_xml_GetAttrAsUnsignedInt64(pNode, "base_addr", 0, IMG_FALSE);

		/* Memory size (default: 0) */
		nSize = tconf_xml_GetAttrAsUnsignedInt64(pNode, "size", 0, IMG_FALSE);

		/* Memory guard band */
		nGuardBand = tconf_xml_GetAttrAsUnsignedInt64(pNode, "guard_band", 0, IMG_FALSE);

		/* Add to device */
		rResult = TCONF_AddDeviceMemory(psTargetConf, pszDevName, pszName, nBaseAddr, nSize, nGuardBand);

		/* Get next element */
		pNode = mxmlFindElement(pNode, pDevice, "memory", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessDeviceSubDevice(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDevice, const IMG_CHAR* pszDevName)
{
	const IMG_CHAR *pszName, *pszType;
	IMG_UINT64 nBaseAddr, nSize;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get first element */
	pNode = mxmlFindElement(pDevice, pDevice, "sub_device", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* Sub device name */
		pszName = tconf_xml_GetAttrAsString(pNode, "name", NULL, IMG_TRUE);

		/* Sub device type */
		pszType = tconf_xml_GetAttrAsString(pNode, "type", NULL, IMG_TRUE);
		if (*pszType != 'M' && *pszType != 'R')
		{
			printf("ERROR: invalid sub device type \"%s\"\n", pszType);
			longjmp(_jmpBuffer, TCONF_XML_INVALID_VALUE);
		}

		/* Sub device base address (default: 0) */
		nBaseAddr = tconf_xml_GetAttrAsUnsignedInt64(pNode, "base_addr", 0, IMG_FALSE);

		/* Sub device size (default: 0) */
		nSize = tconf_xml_GetAttrAsUnsignedInt64(pNode, "size", 0, IMG_FALSE);

		/* Add sub device */
		rResult = TCONF_AddDeviceSubDevice(psTargetConf, pszDevName, pszName, *pszType, nBaseAddr, nSize);

		/* Get next element */
		pNode = mxmlFindElement(pNode, pDevice, "sub_device", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessDeviceBaseName(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDevice, const IMG_CHAR* pszDevName)
{
	const IMG_CHAR *pszRegisterName, *pszMemoryName;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get first element */
	pNode = mxmlFindElement(pDevice, pDevice, "base_name", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* Register name */
		pszRegisterName = tconf_xml_GetAttrAsString(pNode, "register_name", NULL, IMG_FALSE);

		/* Memory name */
		pszMemoryName = tconf_xml_GetAttrAsString(pNode, "memory_name", NULL, IMG_FALSE);

		/* Add base name */
		rResult = TCONF_AddBaseName(psTargetConf, pszDevName, pszRegisterName, pszMemoryName);
		
		/* Get next element */
		pNode = mxmlFindElement(pNode, pDevice, "base_name", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfacePciDeviceBase(TCONF_sTargetConf* psTargetConf, mxml_node_t *pPci, const IMG_CHAR* pszDevName)
{
	IMG_UINT32 nBar, nBaseAddr, nSize;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get first element */
	pNode = mxmlFindElement(pPci, pPci, "device_base", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* PCI bar */
		nBar = tconf_xml_GetAttrAsUnsignedInt32(pNode, "bar", 0, IMG_TRUE);

		/* Base address */
		nBaseAddr = tconf_xml_GetAttrAsUnsignedInt32(pNode, "base_addr", 0, IMG_TRUE);

		/* Size */
		nSize = tconf_xml_GetAttrAsUnsignedInt32(pNode, "size", 0, IMG_TRUE);

		/* Add to PCI interface */
		rResult = TCONF_AddPciInterfaceDeviceBase(psTargetConf, pszDevName, nBar, nBaseAddr, nSize, IMG_FALSE);
		
		/* Get next element */
		pNode = mxmlFindElement(pNode, pPci, "device_base", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfacePciMemoryBase(TCONF_sTargetConf* psTargetConf, mxml_node_t *pPci, const IMG_CHAR* pszDevName)
{
	IMG_UINT32 nBar, nBaseAddr, nSize;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get first element */
	pNode = mxmlFindElement(pPci, pPci, "memory_base", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* PCI bar */
		nBar = tconf_xml_GetAttrAsUnsignedInt32(pNode, "bar", 0, IMG_TRUE);

		/* Base address */
		nBaseAddr = tconf_xml_GetAttrAsUnsignedInt32(pNode, "base_addr", 0, IMG_TRUE);

		/* Size */
		nSize = tconf_xml_GetAttrAsUnsignedInt32(pNode, "size", 0, IMG_TRUE);

		/* Add to PCI interface */
		rResult = TCONF_AddPciInterfaceMemoryBase(psTargetConf, pszDevName, nBar, nBaseAddr, nSize);
		
		/* Get next element */
		pNode = mxmlFindElement(pNode, pPci, "memory_base", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfacePci(TCONF_sTargetConf* psTargetConf, mxml_node_t *pInterface, const IMG_CHAR* pszDevName)
{
	IMG_UINT32 nVendorId, nDeviceId;
	IMG_RESULT rResult = IMG_SUCCESS;
	mxml_node_t *pNode;

	/* Get first element */
	pNode = mxmlFindElement(pInterface, pInterface, "pci", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* Vendor id */
		nVendorId = tconf_xml_GetAttrAsUnsignedInt32(pNode, "vendor_id", 0, IMG_TRUE);

		/* Device id */
		nDeviceId = tconf_xml_GetAttrAsUnsignedInt32(pNode, "device_id", 0, IMG_TRUE);

		/* Create device */
		rResult = TCONF_AddPciInterface(psTargetConf, pszDevName, nVendorId, nDeviceId);
		if (rResult != IMG_SUCCESS)
			return  rResult;

		/* Process device base */
		rResult = tconf_xml_ProcessInterfacePciDeviceBase(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return  rResult;

		/* Process memory base */
		rResult = tconf_xml_ProcessInterfacePciMemoryBase(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return  rResult;

		/* Get next element */
		pNode = mxmlFindElement(pNode, pInterface, "pci", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfaceBurnMem(TCONF_sTargetConf* psTargetConf, mxml_node_t *pInterface, const IMG_CHAR* pszDevName)
{
	const IMG_CHAR *pszDeviceId;
	IMG_UINT32 nOffset, nSize;	
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get first element */
	pNode = mxmlFindElement(pInterface, pInterface, "burn_mem", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* Device id */
		pszDeviceId = tconf_xml_GetAttrAsString(pNode, "device_id", NULL, IMG_TRUE);

		/* Offset */
		nOffset = tconf_xml_GetAttrAsUnsignedInt32(pNode, "offset", 0, IMG_TRUE);

		/* Size */
		nSize = tconf_xml_GetAttrAsUnsignedInt32(pNode, "size", 0, IMG_TRUE);

		rResult = TCONF_AddBurnMem(psTargetConf, pszDevName, *pszDeviceId, nOffset, nSize);

		/* Get next element */
		pNode = mxmlFindElement(pNode, pInterface, "burn_mem", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfaceSocket(TCONF_sTargetConf* psTargetConf, mxml_node_t *pInterface, const IMG_CHAR* pszDevName)
{
	const IMG_CHAR *pszParentDeviceName, *pszHostname;
	IMG_BOOL bBuffering;	
	mxml_node_t *pNode;
	IMG_UINT32 nPort;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get element */
	pNode = mxmlFindElement(pInterface, pInterface, "socket", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		pszParentDeviceName = tconf_xml_GetAttrAsString(pNode, "device_name", NULL, IMG_FALSE);
		if (pszParentDeviceName != NULL)
		{
			/* Connect using parent device */
			rResult = TCONF_AddDeviceIp(
				psTargetConf,
				pszDevName,
				pszParentDeviceName,
				NULL,
				0,
				IMG_TRUE); //	Buffering is hard coded as it will inherit from the parent
		}
		else
		{
			/* Connect using hostname and port */

			/* Hostname (default: local machine) */
			pszHostname = tconf_xml_GetAttrAsString(pNode, "hostname", NULL, IMG_FALSE);

			/* Port */
			nPort = tconf_xml_GetAttrAsUnsignedInt32(pNode, "port", 0, IMG_TRUE);

			/* Port is 16 bits (not 32...) */
			if ((nPort >> 16) != 0)
			{
				printf("ERROR: invalid socket port \"%u\"\n", nPort);
				longjmp(_jmpBuffer, TCONF_XML_INVALID_VALUE);
			}

			/* Buffering (default: true) */
			bBuffering = tconf_xml_GetAttrAsBoolean(pNode, "buffering", IMG_TRUE, IMG_FALSE);

			rResult = TCONF_AddDeviceIp(
				psTargetConf,
				pszDevName,
				NULL,
				pszHostname,
				(IMG_UINT16)nPort,
				bBuffering);
		}
		
		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pInterface, "socket", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate socket interface found\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfacePdump1(TCONF_sTargetConf* psTargetConf, mxml_node_t *pInterface, const IMG_CHAR* pszDevName)
{
	const IMG_CHAR *pszInputDir, *pszOutputDir, *pszCommandFile;
	const IMG_CHAR *pszSendFile, *pszReceiveFile;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get element */
	pNode = mxmlFindElement(pInterface, pInterface, "pdump1", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		/* Input directory */
		pszInputDir = tconf_xml_GetAttrAsString(pNode, "input_dir", NULL, IMG_TRUE);

		/* Output directory */
		pszOutputDir = tconf_xml_GetAttrAsString(pNode, "output_dir", NULL, IMG_TRUE);

		/* Command file */
		pszCommandFile = tconf_xml_GetAttrAsString(pNode, "command_file", NULL, IMG_TRUE);

		/* Send data file */
		pszSendFile = tconf_xml_GetAttrAsString(pNode, "send_file", NULL, IMG_TRUE);

		/* Receive data file */
		pszReceiveFile = tconf_xml_GetAttrAsString(pNode, "receive_file", NULL, IMG_TRUE);

		rResult = TCONF_AddPdump1IF(
			psTargetConf,
			pszDevName,
			pszInputDir,
			pszOutputDir,
			pszCommandFile,
			pszSendFile,
			pszReceiveFile);

		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pInterface, "pdump1", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate Pdump1 interface found\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfaceDashDeviceBase(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDash, const IMG_CHAR* pszDevName)
{
	IMG_UINT32 nBaseAddr, nSize;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;
	
	/* Get element */
	pNode = mxmlFindElement(pDash, pDash, "device_base", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		/* Base address */
		nBaseAddr = tconf_xml_GetAttrAsUnsignedInt32(pNode, "base_addr", 0, IMG_TRUE);

		/* Size */
		nSize = tconf_xml_GetAttrAsUnsignedInt32(pNode, "size", 0, IMG_TRUE);

		/* Update dash interface */
		rResult = TCONF_SetDashInterfaceDeviceBase(psTargetConf, pszDevName, nBaseAddr, nSize);
		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pDash, "device_base", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate dash interface device base found\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfaceDashMemoryBase(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDash, const IMG_CHAR* pszDevName)
{
	IMG_UINT32 nBaseAddr, nSize;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get element */
	pNode = mxmlFindElement(pDash, pDash, "memory_base", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		/* Base address */
		nBaseAddr = tconf_xml_GetAttrAsUnsignedInt32(pNode, "base_addr", 0, IMG_TRUE);

		/* Size */
		nSize = tconf_xml_GetAttrAsUnsignedInt32(pNode, "size", 0, IMG_TRUE);

		/* Update dash interface */
		rResult = TCONF_SetDashInterfaceMemoryBase(psTargetConf, pszDevName, nBaseAddr, nSize);

		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pDash, "memory_base", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate dash interface memory base found\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfaceDash(TCONF_sTargetConf* psTargetConf, mxml_node_t *pInterface, const IMG_CHAR* pszDevName)
{
	mxml_node_t *pNode;
	IMG_UINT32 nId;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get first element */
	pNode = mxmlFindElement(pInterface, pInterface, "dash", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		/* Id */
		nId = tconf_xml_GetAttrAsUnsignedInt32(pNode, "id", 0, IMG_TRUE);

		/* Create dash interface */
		rResult = TCONF_AddDashInterface(psTargetConf, pszDevName, nId);
		if (rResult != IMG_SUCCESS)
		{
			printf("WARNING, Dash Interface Setup Failed on device %s\n", pszDevName);
		}

		/* Process device base */
		rResult = tconf_xml_ProcessInterfaceDashDeviceBase(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Process memory base */
		rResult = tconf_xml_ProcessInterfaceDashMemoryBase(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Get next element */
		pNode = mxmlFindElement(pNode, pInterface, "dash", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessInterfaceDirect(TCONF_sTargetConf* psTargetConf, mxml_node_t *pInterface, const IMG_CHAR* pszDevName)
{
	const IMG_CHAR *pszParentDeviceName;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get element */
	pNode = mxmlFindElement(pInterface, pInterface, "direct", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		/* Parent device name (default: NULL) */
		pszParentDeviceName = tconf_xml_GetAttrAsString(pNode, "device_name", NULL, IMG_FALSE);

		rResult = TCONF_AddDirectInterface(psTargetConf, pszDevName, pszParentDeviceName);

		if (rResult != IMG_SUCCESS)
		{
			printf("WARNING: Direct Interface prcoessing failed on device %s\n", pszDevName);
		}

		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pInterface, "direct", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate direct interface found\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return IMG_SUCCESS;
}

static IMG_RESULT tconf_xml_ProcessInterfacePosted(TCONF_sTargetConf* psTargetConf, mxml_node_t *pInterface, const IMG_CHAR* pszDevName)
{
	IMG_UINT32 nOffset;
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;
	
	/* Get element */
	pNode = mxmlFindElement(pInterface, pInterface, "posted", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		/* Offset */
		nOffset = tconf_xml_GetAttrAsUnsignedInt32(pNode, "offset", 0, IMG_TRUE);

		/* Create posted PCI interface */
		rResult = TCONF_AddPostedInterface(psTargetConf, pszDevName, nOffset);
		if (rResult != IMG_SUCCESS)
		{
			printf("WARNING: Posted Interface Setup invalid for device %s\n", pszDevName);
		}

		/* Don't allow duplicate elements */
		pNode = mxmlFindElement(pNode, pInterface, "direct", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate posted interface found\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return IMG_SUCCESS;
}

static IMG_RESULT tconf_xml_ProcessDeviceInterface(TCONF_sTargetConf* psTargetConf, mxml_node_t *pDevice, const IMG_CHAR* pszDevName)
{
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get element */
	pNode = mxmlFindElement(pDevice, pDevice, "interface", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		const IMG_CHAR *pszValue;

		/* Interface selection (default: pci) */
		pszValue = tconf_xml_GetAttrAsString(pNode, "default", "pci", IMG_FALSE);

		if (strcasecmp(pszValue, "pci") == 0)
			_xmlConf.sWrapperControlInfo.eDevifType = DEVIF_TYPE_PCI;
		else if (strcasecmp(pszValue, "burn_mem") == 0)
			_xmlConf.sWrapperControlInfo.eDevifType = DEVIF_TYPE_BMEM;
		else if (strcasecmp(pszValue, "socket") == 0)
			_xmlConf.sWrapperControlInfo.eDevifType = DEVIF_TYPE_SOCKET;
		else if (strcasecmp(pszValue, "pdump1") == 0)
			_xmlConf.sWrapperControlInfo.eDevifType = DEVIF_TYPE_PDUMP1;
		else if (strcasecmp(pszValue, "dash") == 0)
			_xmlConf.sWrapperControlInfo.eDevifType = DEVIF_TYPE_DASH;
		else if (strcasecmp(pszValue, "direct") == 0)
			_xmlConf.sWrapperControlInfo.eDevifType = DEVIF_TYPE_DIRECT;
		else if (strcasecmp(pszValue, "null") == 0)
			_xmlConf.sWrapperControlInfo.eDevifType = DEVIF_TYPE_NULL;
		else if (strcasecmp(pszValue, "posted") == 0)
			_xmlConf.sWrapperControlInfo.eDevifType = DEVIF_TYPE_POSTED;
		else
		{
			printf("ERROR: invalid interface default value \"%s\"\n", pszValue);
			longjmp(_jmpBuffer, TCONF_XML_INVALID_VALUE);
		}
		
		/* Set wrapper control information */
		rResult = TCONF_SetWrapperControlInfo(psTargetConf, &_xmlConf.sWrapperControlInfo, _xmlConf.eMemTwiddle, _xmlConf.bVerifyMemWrites);
		if (rResult != IMG_SUCCESS)
			return rResult;

		/* Process PCI interface */
		rResult = tconf_xml_ProcessInterfacePci(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return rResult;

		/* Process burn memory interface */
		rResult = tconf_xml_ProcessInterfaceBurnMem(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return rResult;

		/* Process socket interface */
		rResult = tconf_xml_ProcessInterfaceSocket(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return rResult;

		/* Process Pdump1 interface */
		rResult = tconf_xml_ProcessInterfacePdump1(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return rResult;

		/* Process dash interface */
		rResult = tconf_xml_ProcessInterfaceDash(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return rResult;

		/* Process direct device interface */
		rResult = tconf_xml_ProcessInterfaceDirect(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return rResult;

		/* Process posted PCI interface */
		rResult = tconf_xml_ProcessInterfacePosted(psTargetConf, pNode, pszDevName);
		if (rResult != IMG_SUCCESS)
			return rResult;

		/* Don't allow duplicate element */
		pNode = mxmlFindElement(pNode, pDevice, "interface", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate element 'interface' found in 'device' element\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessDevices(TCONF_sTargetConf* psTargetConf, mxml_node_t *pRoot)
{
	IMG_RESULT rResult = IMG_SUCCESS;
	const IMG_CHAR *pszName;
	IMG_UINT64 nBaseAddr;
	mxml_node_t *pNode;

	/* Get first device */
	pNode = mxmlFindElement(pRoot, pRoot, "device", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode == NULL)
	{
		printf("ERROR: no 'device' element found in 'target_config'\n");
		longjmp(_jmpBuffer, TCONF_XML_UNDEFINED_ELEMENT);
	}

	while (pNode != NULL)
	{
		/* Device name */
		pszName = tconf_xml_GetAttrAsString(pNode, "name", NULL, IMG_TRUE);

		/* Device base address of the register bank */
		nBaseAddr = tconf_xml_GetAttrAsUnsignedInt64(pNode, "base_addr", 0, IMG_TRUE);

		/* Create device */
		rResult = TCONF_AddDevice(psTargetConf, pszName, nBaseAddr);
		if (rResult == IMG_ERROR_OUT_OF_MEMORY)
		{
			printf("ERROR: not enough memory\n");
			longjmp(_jmpBuffer, TCONF_XML_NOT_ENOUGH_MEM);
		}
		else if (rResult == IMG_ERROR_ALREADY_COMPLETE)
		{
			printf("ERROR: Duplicate Device %s\n", pszName);
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
		else if (rResult != IMG_SUCCESS)
			break;
		
		/* Process options */
		rResult = tconf_xml_ProcessDeviceOptions(psTargetConf, pNode, pszName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Process default memory */
		rResult = tconf_xml_ProcessDeviceDefaultMem(psTargetConf, pNode, pszName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Process registers */
		rResult = tconf_xml_ProcessDeviceRegisters(psTargetConf, pNode, pszName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Process memory */
		rResult = tconf_xml_ProcessDeviceMemory(psTargetConf, pNode, pszName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Process sub device */
		rResult = tconf_xml_ProcessDeviceSubDevice(psTargetConf, pNode, pszName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Process base name */
		rResult = tconf_xml_ProcessDeviceBaseName(psTargetConf, pNode, pszName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Process interface */
		rResult = tconf_xml_ProcessDeviceInterface(psTargetConf, pNode, pszName);
		if (rResult != IMG_SUCCESS)
			break;

		/* Get next device */
		pNode = mxmlFindElement(pNode, pRoot, "device", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessPdumpContext(TCONF_sTargetConf* psTargetConf, mxml_node_t *pRoot)
{
	mxml_node_t *pNode;
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get first element */
	pNode = mxmlFindElement(pRoot, pRoot, "pdump_context", NULL, NULL, MXML_DESCEND_FIRST);
	while (pNode != NULL)
	{
		const IMG_CHAR *pszContextName, *pszMemSpace;
		mxml_node_t *pNext;	

		/* Pdump context name */
		pszContextName = tconf_xml_GetAttrAsString(pNode, "name", NULL, IMG_TRUE);

		/* Get first element */
		pNext = mxmlGetFirstChild(pNode);
		while (pNext != NULL)
		{
			/* Nothing else but text expected within element */
			if (mxmlGetType(pNext) != MXML_TEXT)
			{
				printf("ERROR: unexpected element within 'pdump_context' element\n");
				longjmp(_jmpBuffer, TCONF_XML_UNEXPECTED_ELEMENT);
			}

			/* Get memory space name string */
			pszMemSpace = mxmlGetText(pNext, NULL);
			IMG_ASSERT(pszMemSpace != NULL);

			rResult = TCONF_AddPdumpContext(psTargetConf, pszMemSpace, pszContextName);
			if (rResult != IMG_SUCCESS) return rResult;

			/* Get next element */
			pNext = mxmlGetNextSibling(pNext);			
		}
		/* Get next element */
		pNode = mxmlFindElement(pNode, pRoot, "pdump_context", NULL, NULL, MXML_NO_DESCEND);
	}
	return rResult;
}

static IMG_RESULT tconf_xml_ProcessPlatformInitCommand(mxml_node_t *pCommand)
{
	mxml_node_t *pNode;

	/* Get element */
	pNode = mxmlGetFirstChild(pCommand);
	if (pNode != NULL && mxmlGetType(pNode) == MXML_TEXT)
	{
		IMG_CHAR szRegAddr[256 + 128], *pszMemSpace, *pszOffset;
		const IMG_CHAR *pszCommand, *pszRegAddr, *pszValue;
		IMG_UINT32 nOffset, nValue;

		/* Get command */
		pszCommand = mxmlGetText(pNode, NULL);
		IMG_ASSERT(pszCommand != NULL);

		/* Only WRW supported */
		if (strcmp(pszCommand, "WRW") != 0)
		{
			printf("ERROR: unsupported command \"%s\" within platform init command\n", pszCommand);
			longjmp(_jmpBuffer, TCONF_XML_INVALID_VALUE);
		}

		/* Get register address ":block:address" */
		pNode = mxmlGetNextSibling(pNode);
		if (mxmlGetType(pNode) != MXML_TEXT)
		{
			printf("ERROR: invalid element within platform init command\n");
			longjmp(_jmpBuffer, TCONF_XML_UNEXPECTED_ELEMENT);
		}

		pszRegAddr = mxmlGetText(pNode, NULL);
		IMG_ASSERT(pszRegAddr != NULL);

		strncpy(szRegAddr, pszRegAddr, sizeof(szRegAddr) - 1);
		szRegAddr[sizeof(szRegAddr) - 1] = '\0';

		/* Process register address */
		if (szRegAddr[0] != ':')
		{
			printf("ERROR: invalid register address \"%s\"\n", szRegAddr);
			longjmp(_jmpBuffer, TCONF_XML_PARSING_FAILED);
		}

		/* Get memory space name */
		pszMemSpace = strtok(szRegAddr + 1, ":");
		if (pszMemSpace == NULL)
		{
			printf("ERROR: invalid register address \"%s\"\n", szRegAddr);
			longjmp(_jmpBuffer, TCONF_XML_PARSING_FAILED);
		}

		/* Get offset */
		pszOffset = strtok(NULL, ":");
		if (pszOffset == NULL)
		{
			printf("ERROR: invalid register address \"%s\"\n", szRegAddr);
			longjmp(_jmpBuffer, TCONF_XML_PARSING_FAILED);
		}

		errno = 0;
		nOffset = (IMG_UINT32)strtoul(pszOffset, NULL, 0);
		if (errno != 0)
		{
			printf("ERROR: failed to convert register address offset \"%s\" to 32-bits unsigned integer\n", pszOffset);
			longjmp(_jmpBuffer, TCONF_XML_FAILED_CONVERTION);
		}

		/* Get value */
		pNode = mxmlGetNextSibling(pNode);
		if (mxmlGetType(pNode) != MXML_TEXT)
		{
			printf("ERROR: invalid element within platform init command\n");
			longjmp(_jmpBuffer, TCONF_XML_UNEXPECTED_ELEMENT);
		}

		pszValue = mxmlGetText(pNode, NULL);
		IMG_ASSERT(pszValue != NULL);

		errno = 0;
		nValue = (IMG_UINT32)strtoul(pszValue, NULL, 0);
		if (errno != 0)
		{
			printf("ERROR: failed to convert register address value \"%s\" to 32-bits unsigned integer\n", pszValue);
			longjmp(_jmpBuffer, TCONF_XML_FAILED_CONVERTION);
		}

		/* Write word */
		if (!TCONF_WriteWord32(pszMemSpace, nOffset, nValue))
		{
			printf("ERROR: platform init command \"%s %s %u\" failed\n", pszCommand, pszRegAddr, nValue);
			longjmp(_jmpBuffer, TCONF_XML_PARSING_FAILED);
		}
	}
	else
	{
		printf("ERROR: invalid element within platform init command\n");
		longjmp(_jmpBuffer, TCONF_XML_UNEXPECTED_ELEMENT);
	}
	return IMG_SUCCESS;
}

static IMG_RESULT tconf_xml_ProcessPlatformInit(mxml_node_t *pRoot)
{
	mxml_node_t *pNode;

	/* Get element */
	pNode = mxmlFindElement(pRoot, pRoot, "platform_init", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode != NULL)
	{
		const IMG_CHAR *pszName;
		mxml_node_t *pCommand;
		IMG_UINT32 nCount = 0;		

		/* Get first element */
		pCommand = mxmlGetFirstChild(pNode);
		while (pCommand != NULL)
		{
			/* Ignore all nodes not of element type */
			if (mxmlGetType(pCommand) == MXML_ELEMENT)
			{
				/* Check that element name is "command" */
				pszName = mxmlGetElement(pCommand);
				if (strcmp(pszName, "command") != 0)
				{
					printf("ERROR: invalid element \"%s\" within platform init\n", pszName);
					longjmp(_jmpBuffer, TCONF_XML_UNEXPECTED_ELEMENT);
				}

				/* Process command */
				tconf_xml_ProcessPlatformInitCommand(pCommand);
				nCount++;
			}

			/* Get next element */
			pCommand = mxmlGetNextSibling(pCommand);
		}

		if (nCount == 0)
			printf("WARNING: no platform init commands defined\n");

		/* Don't allow duplicate platform_init */
		pNode = mxmlFindElement(pNode, pRoot, "platform_init", NULL, NULL, MXML_NO_DESCEND);
		if (pNode != NULL)
		{
			printf("ERROR: duplicate element <platform_init> found in <target_config>\n");
			longjmp(_jmpBuffer, TCONF_XML_DUPLICATE_ELEMENT);
		}
	}
	return IMG_SUCCESS;
}

static IMG_RESULT tconf_xml_ProcessRoot(TCONF_sTargetConf* psTargetConf, mxml_node_t *pTree, TAL_pfnTAL_MemSpaceRegister pfnTAL_MemSpaceRegister)
{
	TCONF_sMemSpItem *pMemSpItem;
	IMG_UINT64 nVersion;
	mxml_node_t *pNode;	
	IMG_RESULT rResult = IMG_SUCCESS;

	/* Get root */
	pNode = mxmlFindElement(pTree, pTree, "target_config", NULL, NULL, MXML_DESCEND_FIRST);
	if (pNode == NULL)
	{
		printf("ERROR: root element \"target_config\" not found in file\n");
		longjmp(_jmpBuffer, TCONF_XML_UNDEFINED_ELEMENT);
	}

	/* Get version */
	nVersion = tconf_xml_GetAttrAsUnsignedInt64(pNode, "version", 1, IMG_FALSE);
	if (nVersion != 1)
	{
		printf("ERROR: unsupported XML configuration format version \"%" IMG_I64PR "u\"\n", nVersion);
		longjmp(_jmpBuffer, TCONF_XML_INVALID_VALUE);
	}

	/* Process options */
	rResult = tconf_xml_ProcessGlobalOptions(pNode);
	if (rResult != IMG_SUCCESS)
		return rResult;

	/* Process devices */
	rResult = tconf_xml_ProcessDevices(psTargetConf, pNode);
	if (rResult != IMG_SUCCESS)
		return rResult;

	/* Setup TAL memory spaces */
	pMemSpItem = (TCONF_sMemSpItem *)LST_first(&psTargetConf->sMemSpItemList);
    while (pMemSpItem != IMG_NULL)
    {
    	rResult = pfnTAL_MemSpaceRegister(&pMemSpItem->sTalMemSpaceCB);
		if (rResult != IMG_SUCCESS)
			return rResult;
    	pMemSpItem = (TCONF_sMemSpItem *)LST_next(pMemSpItem);
    }

	/* Process platform init */
	rResult = tconf_xml_ProcessPlatformInit(pNode);
	if (rResult != IMG_SUCCESS)
		return rResult;

	/* Process Pdump Context */
	rResult = tconf_xml_ProcessPdumpContext(psTargetConf, pNode);
	return rResult;
}

static void tconf_xml_ParserError(const char *pszError)
{
	printf("ERROR: %s\n", pszError);
}

IMG_RESULT TCONF_XML_TargetConfigure(TCONF_sTargetConf* psTargetConf, TAL_pfnTAL_MemSpaceRegister pfnTAL_MemSpaceRegister)
{
	IMG_RESULT nResult = IMG_ERROR_FATAL;
	mxml_node_t *pTree = NULL;
	FILE *pFile = NULL;	

	/* Error jump */
	if (setjmp(_jmpBuffer) == 0)
	{
		/* Open file */
		pFile = fopen(psTargetConf->pszConfigFile, "r");
		if (pFile == NULL)
		{
			printf("ERROR: Failed to open file \"%s\"\n", psTargetConf->pszConfigFile);
			return IMG_ERROR_FATAL;
		}

		/* Set error callback */
		mxmlSetErrorCallback(tconf_xml_ParserError);

		/* Parse XML */
		pTree = mxmlLoadFile(NULL, pFile, MXML_TEXT_CALLBACK);
		if (pTree == NULL)
			longjmp(_jmpBuffer, TCONF_XML_PARSING_FAILED);
   
		/* Process config */
		tconf_xml_ProcessRoot(psTargetConf, pTree, pfnTAL_MemSpaceRegister);
		nResult = IMG_SUCCESS;
	}
	else
		IMG_ASSERT(IMG_FALSE);

	/* Cleanup */
	if (pTree != NULL)
		mxmlDelete(pTree);

	if (pFile != NULL)
		fclose(pFile);
	
	return nResult;
}
