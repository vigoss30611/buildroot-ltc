/*! 
*******************************************************************************
\file		system_mem_sim.cpp
\brief		Transif Memory Model for sims with built in memory
\author		Imagination Technologies
\date		02/10/2012
\version	Not externally released.
\details	<b>Copyright 2010 by Imagination Technologies Limited.</b>\n
			All rights reserved.  No part of this software, either
			material or conceptual may be copied or distributed,
			transmitted, transcribed, stored in a retrieval system
			or translated into any human or computer language in any
			form by any means, electronic, mechanical, manual or
			other-wise, or disclosed to third parties without the
			express written permission of Imagination Technologies
			Limited, Unit 8, HomePark Industrial Estate,
			King's Langley, Hertfordshire, WD4 8LZ, U.K.\n
*******************************************************************************/
#include <map>
#include <errno.h>
#include <iostream>

#ifdef WIN32
#	include <windows.h>
#else
#	include <sched.h>
#endif

#include "system_mem_sim.h"
#include "osa.h"

//#define DEBUG_OUTPUT
//#define DELAYED_MEM_REQ_ACK
#define INSTANT_MEM_RESP

using namespace std;

/*!
*******************************************************************************
\brief			:	Class Constructor
\param[in]	 	:	Pointer to the simulator who's model it is	  
*******************************************************************************/
system_mem_sim::system_mem_sim( toIMGIP_if* pSimulator, bool bTimed, IMG_UINT64 ui64SimMemAddr, IMG_UINT64 ui64SimMemSize )
	: system_mem_alloc()
	, m_pSimulator(pSimulator)
	, m_timed(bTimed)
	, m_doWaitResp(false)
	, m_SimMemAddr(ui64SimMemAddr)
	, m_SimMemSize(ui64SimMemSize)
{
	// create a data item to contain the slave data requests
	m_slaveReadData = new IMGBUS_DATA_TYPE(32,8);

	// create thread mutexs for slave access
	OSA_ThreadConditionCreate(&m_slaveResponded);
};

/*!
*******************************************************************************
\brief			:	Class Destructor
*******************************************************************************/
system_mem_sim::~system_mem_sim()
{
	// Delete Slave Read Data
	if (m_slaveReadData) delete m_slaveReadData;
}

/*!
*******************************************************************************
\brief			:	This function returns true if the memory is internal to the sim
\param[in]	 	:	The address of the memory request
\param[in]	 	:	 
\return		 	:	Returns a boolean value
*******************************************************************************/
IMG_BOOL system_mem_sim::test_internal_memory(IMG_UINT64 address)
{
	if((address >= m_SimMemAddr) && (address < m_SimMemAddr + m_SimMemSize))
	{
		return IMG_TRUE;
	}
	return IMG_FALSE;
}

/*!
*******************************************************************************
\brief			:	This function implements a single word write to either
					GRAM or external ram.
\param[in]	 	:	The address of the memory write
\param[in]	 	:	The data to be written
\return		 	:	None
*******************************************************************************/
void system_mem_sim::write(IMG_UINT64 address, IMG_UINT32 uData)
{	
	// Identify UCC Simulator Internal Memory Tag from Base address of request
	IMG_BOOL bSimMem = test_internal_memory(address);

	// Are we accessing the sim's Internal Memory?
	if (bSimMem)
	{
		bool bAck;
		IMG_UINT32 retValue;

		IMGBUS_ADDRESS_TYPE addr;
		addr.address = address;

		// Create Data Structure
		IMGBUS_DATA_TYPE data(32, 1);
		data.copyInFrom((IMG_UINT32*)&uData, 1);

		// Create Control Structure
		imgbus_transaction_control ctrl;

		// Make the slave requeset
		m_pSimulator->SlaveRequest(addr, data, false, ctrl, bAck); 
		
		// Wait for acknowledgement
		WaitForSlaveResponse(retValue);
	}
	else 
	{
		// External RAM Access so use default class implementation
		system_mem_alloc::write(address, uData);
	}
}

/*!
*******************************************************************************
\brief			:	This function will write a block of memory to either
					GRAM or external ram
\param[in]	 	:	The address of the write
\param[in]	 	:	A (byte) pointer to the data to write
\param[in]	 	:	The length of data in (bytes) to write
\return		 	:	None
*******************************************************************************/
void system_mem_sim::block_write(IMG_UINT64 address, IMG_BYTE* const pData, IMG_UINT64 uLength)
{	
	IMG_BOOL bSimMem = test_internal_memory(address);
	IMG_UINT32 uLengthInWords = (IMG_UINT32)(uLength + 3) / 4;


	// Determine if we are doing an EXTRAM access or INTERNAL Simulator Memory Access
	if (bSimMem)
	{
		bool bAck;
		IMG_UINT32 retValue;
		IMGBUS_ADDRESS_TYPE addr;
		addr.address = address;

		// Create Data Structure
		IMGBUS_DATA_TYPE data(32, uLengthInWords);
		data.copyInFrom((IMG_UINT32*)pData, uLengthInWords);

		// Create Control Structure
		imgbus_transaction_control ctrl;

		// Make the slave requeset
		m_pSimulator->SlaveRequest(addr, data, false, ctrl, bAck); 
		
		// Wait for acknowledgement
		WaitForSlaveResponse(retValue);
	}
	else
	{
		// External RAM Access so use default class implementation
		system_mem_alloc::block_write(address, pData, uLength);
	}
}

/*!
*******************************************************************************
\brief			:	This function implements a single word read to either
					GRAM or external ram.
\param[in]	 	:	The address of the memory read
\return		 	:	The word of data read 
*******************************************************************************/
IMG_UINT32 system_mem_sim::read(IMG_UINT64 address)
{	
	//IMG_UINT32 uLength = 4;
	IMG_UINT32 uLengthInWords = 1;
	IMG_UINT32 uData;

	IMG_BOOL bSimMem = test_internal_memory(address);
	
	// Determine if we are doing an EXTRAM access or INTERNAL Simulator Memory Access
	if (bSimMem)
	{
		bool bAck;
		IMG_UINT32 retValue;
		IMGBUS_ADDRESS_TYPE addr;
		addr.address = address;

		// Create Data Structure
		IMGBUS_DATA_TYPE data(32, uLengthInWords);

		// Create Control Structure
		imgbus_transaction_control ctrl;

		// Make the slave requeset
		m_pSimulator->SlaveRequest(addr, data, true, ctrl, bAck); 

		// Wait for slave response before copying out the memory (IMPORTANT)
		WaitForSlaveResponse(retValue);

		// Copy data out to calling function pointer
		m_slaveReadData->copyOutTo(&uData, uLengthInWords);
		return uData;
	}
	else
	{
		// External RAM Access so use default class implementation
		return system_mem_alloc::read(address);
	}
}

/*!
*******************************************************************************
\brief			:	This function will read a block of memory from either
					GRAM or external ram
\param[in]	 	:	The address of the read
\param[in]	 	:	A (byte) pointer to where to send the data read
\param[in]	 	:	The length of data in (bytes) to read
\return		 	:	None 
*******************************************************************************/
void system_mem_sim::block_read(IMG_UINT64 address, IMG_BYTE* pData, IMG_UINT64 uLength)
{
	IMG_BOOL bSimMem = test_internal_memory(address);
    IMG_UINT32 uLengthInWords = (IMG_UINT32)(uLength + 3) / 4;

	// Determine if we are doing an EXTRAM access or INTERNAL Simulator Memory Access
	if (bSimMem)
	{
		bool bAck;
		IMG_UINT32 retValue;
		IMGBUS_ADDRESS_TYPE addr;
		addr.address = address;

		// Create Data Structure
		IMGBUS_DATA_TYPE data(32, uLengthInWords);

		// Create Control Structure
		imgbus_transaction_control ctrl;

		// Make the slave requeset
		m_pSimulator->SlaveRequest(addr, data, true, ctrl, bAck); 

		// Wait for slave response before copying out the memory (IMPORTANT)
		WaitForSlaveResponse(retValue);

		// Copy data out to calling function pointer
		m_slaveReadData->copyOutTo((IMG_UINT32*)pData, uLengthInWords);
	}
	else
	{
		// External RAM Access so use default class implementation
		system_mem_alloc::block_read(address, pData, uLength);
	}
}

/*!
*******************************************************************************
\brief			:	This function shouldn't do anything for internal memory
*******************************************************************************/
IMG_BOOL system_mem_sim::alloc(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize)
{
	// Identify whether to use Simulator Internal Memory from Base address of request
	IMG_BOOL bSimMem = test_internal_memory(uBaseAddress);

	// Determine if we are doing an EXTRAM access or INTERNAL Simulator Memory Access
	if (bSimMem)
	{
		// Do nothing as memory is on chip
		return IMG_TRUE;
	}
	else
	{
		return system_mem_alloc::alloc(uBaseAddress, uSize);
	}
}

/*!
*******************************************************************************
\brief			:	This function shouldn't do anything for internal memory
*******************************************************************************/
void system_mem_sim::free(IMG_UINT64 uBaseAddress)
{
	// Identify whether to use Simulator Internal Memory from Base address of request
	IMG_BOOL bSimMem = test_internal_memory(uBaseAddress);

	// Determine if we are doing an EXTRAM access or INTERNAL Simulator Memory Access
	if (bSimMem)
	{
		return;
		// Do nothing as memory is on chip
	}
	else
	{
		system_mem_alloc::free(uBaseAddress);
	}
}

/*!
*******************************************************************************
\brief			:	This function is called by the parent when a slave request
					has completed.
\param[in]	 	:	Tag ID of the request	 
\param[in]	 	:	Data from the Slave
\return		 	:	None
*******************************************************************************/
void system_mem_sim::csim_mem_access_cb(IMG_UINT32 tag_id, const IMGBUS_DATA_TYPE& data)
{
	(void)tag_id;
	// This function is called when the slave has responded to a request.
	// It must unlock the "WaitForSlaveReponse" function.
  if (m_timed)
  {
	// delete previous pointer
	if (m_slaveReadData) delete m_slaveReadData;

	// create a large enough data item to contain the slave data requests
	m_slaveReadData = new IMGBUS_DATA_TYPE(32, data.getBurstLength());

	// TIMED IMPLEMENTATION
  	m_slaveReadData->copyInFrom(data.data, data.getBurstLength());
	m_doWaitResp = true;
  }
  else
  {  
	OSA_ThreadConditionLock(m_slaveResponded);
	m_slaveReadData->copyInFrom(data.data, data.getBurstLength());
	m_doWaitResp = false;
	OSA_ThreadConditionSignal(m_slaveResponded);
	OSA_ThreadConditionUnlock(m_slaveResponded);
  }
}

/*!
*******************************************************************************
\brief			:	This function will wait until a slave request has returned
\param[in]	 	:	Reference to a return value
\return		 	:	None
*******************************************************************************/
void system_mem_sim::WaitForSlaveResponse(IMG_UINT32& retValue)
{
	if (m_timed)
	{
		// run the simulator until we get a reply
		while (!m_doWaitResp)
		{
			m_pSimulator->Clock(100);
			// needed if Register request blocked on Memory requests
			//ProcessMemoryResponses(1);
					
		}
		retValue = *(IMG_UINT32*)(m_slaveReadData->data);
		m_doWaitResp = false; // for next resp
	}	
	else
	{
		// untimed model, the sim automatically runs, wait until we get a reply	
		OSA_ThreadConditionLock(m_slaveResponded);
		while (m_doWaitResp)
		{
			// might have got response before we get here, so only wait if not got response
			int retVal = OSA_ThreadConditionWait(m_slaveResponded, 2000);
			if (retVal == 0)
			{
				break; // response received
			}
		}
		retValue = m_slaveReadData->data[0];
		m_doWaitResp = true; // next time this is called do wait
		OSA_ThreadConditionUnlock(m_slaveResponded);
	}
}
/*
*******************************************************************************
***************************** E N D  O F  F I L E *****************************
*******************************************************************************
*/

