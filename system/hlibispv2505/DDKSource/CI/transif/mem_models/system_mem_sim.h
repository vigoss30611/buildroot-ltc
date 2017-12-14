/*! 
*******************************************************************************
\file		system_mem_sim.h
\brief		UCC Memory Model for PDUMPPlayer
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

#ifndef _SYSTEM_MEM_SIM_INCLUDED_H_
#define _SYSTEM_MEM_SIM_INCLUDED_H_

#include "system_mem_alloc.h"

using namespace std;
/*
*******************************************************************************
\brief		:	system_mem_sim
				This class models the UCC memory for PDUMP. It is required
				because the UCC contains internal memory that can be accessed
				through the HOST PORT
*******************************************************************************/
template <typename AddressType>
struct system_mem_allocation;

class system_mem_sim : public system_mem_alloc {

public:
	system_mem_sim( toIMGIP_if* pSimulator, bool bTimed, IMG_UINT64 ui64SimMemAddr, IMG_UINT64 ui64SimMemSize );

	~system_mem_sim();
	
	// override base system_mem_alloc
	virtual void write(IMG_UINT64 address, IMG_UINT32 uData);
	virtual void block_write(IMG_UINT64 address, IMG_BYTE* const pData, IMG_UINT64 uLength);
	virtual IMG_UINT32 read(IMG_UINT64 address);
	virtual void block_read(IMG_UINT64 address, IMG_BYTE* pData, IMG_UINT64 uLength);
	virtual IMG_BOOL alloc(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize);
	virtual void free(IMG_UINT64 uBaseAddress);

	// override base SimpleDevifMemoryModel
	virtual void csim_mem_access_cb(IMG_UINT32 tag_id, const IMGBUS_DATA_TYPE& data);


	void WaitForSlaveResponse(IMG_UINT32& retValue);

private:
	toIMGIP_if*			m_pSimulator; // The Simulator

	// Returns true if write is internal to sim, false if external
	virtual IMG_BOOL test_internal_memory(IMG_UINT64 address);

	// these are used to block slave commands until it has been responded
	IMGBUS_DATA_TYPE *m_slaveReadData; 
	IMG_HANDLE m_slaveResponded;

	bool m_timed;
	bool m_doWaitResp; // true if response has not arrived

	// Current a 1:1 mapping of address and only 1 mem region assumed
	// This functionality could be extended if necessary
	IMG_UINT64 m_SimMemAddr;
	IMG_UINT64 m_SimMemSize;
	

};

#endif //system_mem_sim_INCLUDED_H
/*
*******************************************************************************
***************************** E N D  O F  F I L E *****************************
*******************************************************************************
*/
