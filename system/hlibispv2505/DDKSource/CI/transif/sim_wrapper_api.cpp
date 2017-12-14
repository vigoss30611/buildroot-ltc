/*****************************************************************************
 * Name         : sim_wrapper_api.cpp
 * Title        : Simulation Wrapper, Interface definition to IMG Simulator
 * Author       : Imagination Technologies
 * Created      : 2011-02-14
 *
 * Copyright    : 1998 by Imagination Technologies Limited. All rights reserved.
 * 				  No part of this software, either material or conceptual 
 * 				  may be copied or distributed, transmitted, transcribed,
 * 				  stored in a retrieval system or translated into any 
 * 				  human or computer language in any form by any means,
 * 				  electronic, mechanical, manual or otherwise, or 
 * 				  disclosed to third parties without the express written
 * 				  permission of Imagination Technologies Limited, HomePark
 * 				  Industrial Estate, Kings Langley, WD4 8LZ, UK.
 *
 * Description  : 
 * Platform     : ANSI
 *
 ******************************************************************************/

#ifdef WIN32
#	include <windows.h>
#else
#	include <dlfcn.h>
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include "stdlib.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::setfill;
using std::setprecision;


#include "sim_wrapper_api.h"
// remember to define the library name

// libary handle
#ifdef WIN32
	HINSTANCE g_hinstLib;
#else
	void* g_hinstLib;
#endif	

// functions that are part of the fromIMGIP class, forward declared, implemented and referenced in this file only
extern "C"
{
	imgbus_status	FromIMG_SlaveResponse				(void * hostHandle, const IMGBUS_DATA_TYPE& data);
	void			FromIMG_SlaveResponseIntMem			(void * hostHandle, IMG_UINT32 tag_id, const IMGBUS_DATA_TYPE& data, bool &acknowledged);
	void			FromIMG_MasterRequest				(void * hostHandle, const IMGBUS_ADDRESS_TYPE &addr, IMGBUS_DATA_TYPE* data, bool readNotWrite, const imgbus_transaction_control &ctrl, bool& acknowledged, bool& responded);
	void			FromIMG_MasterResponseAcknowledge	(void * hostHandle, IMG_UINT32 tag_id);
	void			FromIMG_SlaveStreamOut				(void * hostHandle, const IMG_UINT32 keyID, const IMGBUS_DATA_TYPE& data);
	void			FromIMG_MasterInfoRequest			(void * hostHandle, const IMG_UINT32 keyID, IMGBUS_DATA_TYPE *d_ptr, bool& responded);
	void			FromIMG_SlaveInfoResponse			(void * hostHandle, IMG_UINT32 keyID, const IMGBUS_DATA_TYPE *d_ptr);
	void			FromIMG_SlaveInterruptRequest		(void * hostHandle, IMG_UINT32 interruptLines);
	void			FromIMG_EndOfTest					(void * hostHandle);
};

toIMGIP_if* LoadSimulatorLibrary(const char *libName, unsigned int id, fromIMGIP_if* imgIP_client, int argc, char** argv)
{
	if (!imgIP_client) {
		cerr << "Loading " << libName << ": failed due to NULL imgIP_client pointer supplied" << endl;
		return NULL;
	}

	SimWrapperCBridge *pSimWrapper = new SimWrapperCBridge(libName, id, imgIP_client, argc, argv);
	
	if (!pSimWrapper->isValid()) {
		delete pSimWrapper;
		pSimWrapper = NULL;
	}
	
	cerr << "Library " << libName << ": using CBridge API" << endl;

	return pSimWrapper;
}

void UnloadSimulatorLibrary(toIMGIP_if* pSimWrapper)
{
	delete pSimWrapper;
}

/* 
 *	extern "C" style bridge interface to DLL
 */

SimWrapperCBridge::SimWrapperCBridge(const char *libName, unsigned int id, fromIMGIP_if* imgIP_client, int argc, char** argv)
	: bValid(true)
{
	// Get handle to shared library
#ifdef WIN32
	libHandle = LoadLibrary(TEXT(libName));
#else
	libHandle = dlopen(libName, RTLD_LAZY);
#endif
	pFuncDelLib = NULL;

	if (libHandle == NULL)
	{
		bValid = false;
#ifdef WIN32
        cerr << "Cannot load library " << libName << " (" << GetLastError() << ")." << endl;
#else
		cerr << "Cannot load library " << libName << " (" << dlerror() << ")." << endl;
#endif
		return;
	}

	// lookup symbols in the library
	pFuncInitLib = (FPTR_INIT_LIB) lookupSymbol("InitLibrary");
	pFuncDelLib = (FPTR_DEL_LIB) lookupSymbol("DeleteLibrary");

	pFuncReset						= (FPTR_RESET)							lookupSymbol("Reset");
	pFuncInitDevice					= (FPTR_INIT_DEVICE)					lookupSymbol("InitDevice");
	pFuncClock						= (FPTR_CLOCK)							lookupSymbol("Clock");
	pFuncSlaveRequest				= (FPTR_SLAVE_REQUEST)					lookupSymbol("SlaveRequest");
	pFuncMasterResponse				= (FPTR_MASTER_RESPONSE)				lookupSymbol("MasterResponse");
	pFuncMasterRequestAcknowledge	= (FPTR_MASTER_REQUEST_ACKNOWLEDGE)		lookupSymbol("MasterRequestAcknowledge");
	pFuncMasterStreamOut			= (FPTR_MASTER_STREAM_OUT)				lookupSymbol("MasterStreamOut");
	pFuncSlaveControl				= (FPTR_SLAVE_CONTROL)					lookupSymbol("SlaveControl");
	pFuncSlaveInfoRequest			= (FPTR_SLAVE_INFO_REQUEST)				lookupSymbol("SlaveInfoRequest");
	pFuncMasterInfoResponse			= (FPTR_MASTER_INFO_RESPONSE)			lookupSymbol("MasterInfoResponse");
	pFuncMasterInterruptRequest		= (FPTR_MASTER_INTERRUPT_REQUEST)		lookupSymbol("MasterInterruptRequest");
	// extensions for Slave internal memory access
	pFuncSlaveRequestIntMem			= (FPTR_SLAVE_REQUEST_INT_MEM)			lookupSymbol("SlaveRequestIntMem");

	HostCallbacks hcb;
	if (pFuncInitLib) pFuncInitLib(id, imgIP_client, hcb, argc, argv);
}

void * SimWrapperCBridge::lookupSymbol(const char *name)
{
#ifdef WIN32
	void *address = (void *)GetProcAddress((HINSTANCE) libHandle, name);
#else
	void *address = dlsym(libHandle, name);
#endif
	
	if (address == NULL) {
		string funcName(name);
		// allowing for backward compatibility
		if (funcName.compare("SlaveRequestIntMem") != 0)
		{
			cerr << "Cannot find symbol: " << name << endl;
			bValid = false;
		}
		else
		{
			cout << "Library does not support function " << name << endl;
		}
	}
	return address;
}

SimWrapperCBridge::~SimWrapperCBridge()
{
	bValid = false;
	if (pFuncDelLib)
		pFuncDelLib();

	pFuncDelLib						= NULL;
	pFuncInitLib					= NULL;
	pFuncReset						= NULL;
	pFuncClock						= NULL;
	pFuncSlaveRequest				= NULL;
	pFuncMasterResponse				= NULL;
	pFuncMasterRequestAcknowledge	= NULL;
	pFuncMasterStreamOut			= NULL;
	pFuncSlaveControl				= NULL;
	pFuncSlaveInfoRequest			= NULL;
	pFuncMasterInfoResponse			= NULL;
	pFuncMasterInterruptRequest		= NULL;
	pFuncSlaveRequestIntMem			= NULL;

#ifdef WIN32
	FreeLibrary((HINSTANCE) libHandle);
#else
	if (libHandle)
		dlclose(libHandle);
#endif
}

imgbus_status SimWrapperCBridge::Reset()
{
	return pFuncReset();
}

imgbus_status SimWrapperCBridge::InitDevice()
{
	return pFuncInitDevice();
}

imgbus_status SimWrapperCBridge::Clock(const IMG_UINT32 time)
{
	return pFuncClock(time);
}

imgbus_status SimWrapperCBridge::SlaveRequest(const ADDRESS &addr, const DATA &data, bool readNotWrite)
{
	return pFuncSlaveRequest(addr, data, readNotWrite);
}

void SimWrapperCBridge::SlaveRequest(const ADDRESS &addr, const DATA &data, bool readNotWrite, const imgbus_transaction_control &ctrl, bool& acknowledged)
{
	if (pFuncSlaveRequestIntMem == NULL)
	{
		cerr << "CSim Library does not support IMG IP internal memory access, aborting." << endl;
		exit(1);
	}

	pFuncSlaveRequestIntMem(addr, data, readNotWrite, ctrl, acknowledged);
}

void SimWrapperCBridge::MasterResponse(IMG_UINT32 tag_id, const DATA *data, bool& acknowledged)
{
	pFuncMasterResponse(tag_id, data, acknowledged);
}

void SimWrapperCBridge::MasterRequestAcknowledge(IMG_UINT32 tag_id)
{
	pFuncMasterRequestAcknowledge(tag_id);
}

void SimWrapperCBridge::MasterStreamOut(const IMG_UINT32 keyID, const DATA& data)
{
	pFuncMasterStreamOut(keyID, data);
}

void SimWrapperCBridge::SlaveControl(const IMG_UINT32 keyID, DATA *data, bool& responded)
{
	pFuncSlaveControl(keyID, data, responded);
}

void SimWrapperCBridge::SlaveInfoRequest(const IMG_UINT32 keyID, DATA *data, bool& responded)
{
	pFuncSlaveInfoRequest(keyID, data, responded);
}

void SimWrapperCBridge::MasterInfoResponse(IMG_UINT32 keyID, const DATA *data)
{
	pFuncMasterInfoResponse(keyID, data);
}

void SimWrapperCBridge::MasterInterruptRequest(IMG_UINT32 interruptLines)
{
	pFuncMasterInterruptRequest(interruptLines);
}

// ################################################################################

HostCallbacks::HostCallbacks()
{
	pFuncSlaveResponse				= FromIMG_SlaveResponse;
	pFuncMasterRequest				= FromIMG_MasterRequest;
	pFuncMasterResponseAcknowledge	= FromIMG_MasterResponseAcknowledge;
	pFuncSlaveStreamOut				= FromIMG_SlaveStreamOut;
	pFuncMasterInfoRequest			= FromIMG_MasterInfoRequest;
	pFuncSlaveInfoResponse			= FromIMG_SlaveInfoResponse;
	pFuncSlaveInterruptRequest		= FromIMG_SlaveInterruptRequest;
	pFuncEndOfTest					= FromIMG_EndOfTest;
	pFuncSlaveResponseIntMem			= FromIMG_SlaveResponseIntMem;
}

imgbus_status FromIMG_SlaveResponse(void *hostHandle, const IMGBUS_DATA_TYPE& data)
{
	return ((fromIMGIP_if *)(hostHandle))->SlaveResponse(data);
}

void FromIMG_MasterRequest(void *hostHandle, const IMGBUS_ADDRESS_TYPE &addr, IMGBUS_DATA_TYPE* data, bool readNotWrite, const imgbus_transaction_control &ctrl, bool& acknowledged, bool& responded)
{
	((fromIMGIP_if *)(hostHandle))->MasterRequest(addr, data, readNotWrite, ctrl, acknowledged, responded);
}

void FromIMG_MasterResponseAcknowledge(void *hostHandle, IMG_UINT32 tag_id)
{
	((fromIMGIP_if *)(hostHandle))->MasterResponseAcknowledge(tag_id);
}

void FromIMG_SlaveStreamOut(void *hostHandle, const IMG_UINT32 keyID, const IMGBUS_DATA_TYPE& data)
{
	((fromIMGIP_if *)(hostHandle))->SlaveStreamOut(keyID, data);
}

void FromIMG_MasterInfoRequest(void *hostHandle, const IMG_UINT32 keyID, IMGBUS_DATA_TYPE *d_ptr, bool& responded)
{
	((fromIMGIP_if *)(hostHandle))->MasterInfoRequest(keyID, d_ptr, responded);
}

void FromIMG_SlaveInfoResponse(void *hostHandle, IMG_UINT32 keyID, const IMGBUS_DATA_TYPE *d_ptr)
{
	((fromIMGIP_if *)(hostHandle))->SlaveInfoResponse(keyID, d_ptr);
}

void FromIMG_SlaveInterruptRequest(void *hostHandle, IMG_UINT32 interruptLines)
{
	((fromIMGIP_if *)(hostHandle))->SlaveInterruptRequest(interruptLines);
}

void FromIMG_EndOfTest(void *hostHandle)
{
	((fromIMGIP_if *)(hostHandle))->EndOfTest();
}

void FromIMG_SlaveResponseIntMem(void * hostHandle, IMG_UINT32 tag_id, const IMGBUS_DATA_TYPE& data, bool &acknowledged)
{
	((fromIMGIP_if *)(hostHandle))->SlaveResponse(tag_id, data, acknowledged);
}

	
