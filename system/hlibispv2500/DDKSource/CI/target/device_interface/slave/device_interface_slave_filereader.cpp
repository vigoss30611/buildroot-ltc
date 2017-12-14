/*!
******************************************************************************
 @file   : devif_slave.cpp

 @brief  

 @Author Imagination Technologies

 @date   15/09/2005
 
         <b>Copyright 2005 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 \n<b>Description:</b>\n
        this file contains the implementation of a devif_slave - a moudule that listens over tcpip and passes 
		transactions on to a DeviceInterface instance


 \n<b>Platform:</b>\n
	     PC 

******************************************************************************/
#include "device_interface_slave_filereader.h"
#include "img_defs.h"

DevIfSlaveFilereader::DevIfSlaveFilereader(DeviceInterface& device, 
					  const char* pInputFilename,
					  const char* pResultsFilename,
					 IMG_BOOL bVerbose,
					 IMG_CHAR* sDumpFile)
	: DeviceInterfaceSlave(device, bVerbose, sDumpFile, false)
	, pInputFile(NULL)
	, pResultsFile(NULL)
	, uResultsOffset(0)
{
	assert(pInputFilename);
	printf("DevIfSlaveFilereader: Open stimulus file %s\n", pInputFilename);
	pInputFile=fopen(pInputFilename, "rb");
	assert(pInputFile);
	if (pResultsFilename)
	{
		printf("DevIfSlaveFilereader: Open results file %s for verification\n", pResultsFilename);
		pResultsFile=fopen(pResultsFilename, "rb");
		assert(pResultsFile);
	}
}

DevIfSlaveFilereader::~DevIfSlaveFilereader()
{
	if (pResultsFile) 
	{
		fclose (pResultsFile);
		pResultsFile = NULL;
	}
	if (pInputFile) 
	{
		fclose (pInputFile);
		pInputFile = NULL;
	}
}
void DevIfSlaveFilereader::recvData()
{
	size_t rval= fread(pCmdBuffer, 1, i32BufSize, pInputFile);
	if (rval == 0)
	{
		printf("ERROR: DevIfSlaveFilereader::recv - end of data\n");
	}

	i32ReadyCount = (IMG_INT32)rval;
	i32ReadOffset = 0;
}

void DevIfSlaveFilereader::sendData(IMG_INT32 i32NumBytes, const void* pData)
{
	if (pResultsFile){
		char* pTmp = new char[i32NumBytes];
		size_t nRead = fread(pTmp, 1, i32NumBytes, pResultsFile);
		(void)nRead; // Unused in Release Version
		assert(nRead == (IMG_UINT32)i32NumBytes);
		if (memcmp(pData, pTmp, i32NumBytes))
		{
			printf("ERROR: DevIfSlaveFilereader::sendData - mismatch between reference file and output data in block at offset %08x\n", uResultsOffset);
		}
		delete[] pTmp;
	}
	uResultsOffset += i32NumBytes;
}



void DevIfSlaveFilereader::devif_ntohll(IMG_UINT64& val)
{
#if IMG_BYTE_ORDER == IMG_LITTLE_ENDIAN
	IMG_UINT32 ui32High, ui32Low;
	ui32High = *((IMG_UINT32*)&val);
	val >>= 32;
	ui32Low = *((IMG_UINT32*)&val);
	ntohl(ui32High);
	ntohl(ui32Low);
	val = (((IMG_UINT64)ui32High) << 32) | ui32Low;
#endif
}

void DevIfSlaveFilereader::devif_htonll(IMG_UINT64& val)
{
	devif_ntohll(val);
}
