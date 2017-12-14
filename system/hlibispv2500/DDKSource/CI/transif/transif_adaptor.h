#ifndef __TRANSIF_ADAPTOR_H__
#define __TRANSIF_ADAPTOR_H__

#include "system_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*interrupt_request)(void* userData, IMG_UINT32 ui32IntLines);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// only if c++

#include <list>
#include <errno.h>
#include <iostream>
#include <string>
//#include "device_interface_master_transif.h"
#include "sim_wrapper_api.h"

class DeviceInterfaceTransif;
class TransifMemoryResponse;
class TransifDevifHandler;

typedef void (*slave_stream_handler)(void* userData, IMG_UINT32 keyID, const IMGBUS_DATA_TYPE& data);

extern "C" {
struct sHandlerFuncs
{
    interrupt_request pfnInterrupt;
    slave_stream_handler pfnSlaveStream;
    void * userData;
};
}

class TransifAdaptor : public fromIMGIP_if // handles calls from C-sim
{
public:
	// construct with args list, and devifName from pdump config file
	TransifAdaptor(int argc, char **argv, std::string libName = "", std::string devifName = "", bool debug = false, bool forceAlloc = false,
					bool timedModel = true, SimpleDevifMemoryModel* pMem = IMG_NULL, const struct sHandlerFuncs* handlers = IMG_NULL, 
                    SYSMEM_eMemModel memModel = IMG_MEMORY_MODEL_DYNAMIC, IMG_UINT64 ui64BaseAddr = 0, IMG_UINT64 ui64Size = 0 );
	~TransifAdaptor();

public:
	// ####################################################################################################
	// This where we implement the pure virtual methods in the fromIMGIP class

	imgbus_status SlaveResponse(const IMGBUS_DATA_TYPE &d);
	void SlaveResponse(IMG_UINT32 tag_id, const IMGBUS_DATA_TYPE &data, bool &acknowledged);
	
	void MasterRequest(const IMGBUS_ADDRESS_TYPE a, IMGBUS_DATA_TYPE *d_ptr, bool readNotWrite, 
		const imgbus_transaction_control &ctrl, bool &ack , bool &responded);

	void MasterResponseAcknowledge(IMG_UINT32 requestID);

	void SlaveStreamOut(const IMG_UINT32 keyID, const IMGBUS_DATA_TYPE& data);

	void MasterInfoRequest(const IMG_UINT32 keyID, IMGBUS_DATA_TYPE *d_ptr, bool &responded);

	void SlaveInfoResponse(IMG_UINT32 key, const IMGBUS_DATA_TYPE* data);

	void SlaveInterruptRequest(IMG_UINT32 interruptLines);

	void EndOfTest();

private:
	// Internal method to write to memory
	void BlockingMasterWrite(const IMGBUS_ADDRESS_TYPE &a , const IMGBUS_DATA_TYPE *d_ptr, const imgbus_transaction_control &ctrl);

	void BlockingMasterRead(const IMGBUS_ADDRESS_TYPE &a, IMGBUS_DATA_TYPE *d_ptr, const imgbus_transaction_control &ctrl);

	/**
	 * Sends out delayed memory responses, after each call, every queue response is has its delay incremented by 1 clock.
	 * Will send out delays after specified number of clocks has passed, if untimed, the delay is ignored
	 */
	void ProcessMemoryResponses(unsigned int delay);

	SimpleDevifMemoryModel* m_pMem;

	IMG_UINT32 m_busWidth;
	toIMGIP_if *m_simulator;
	// data read from Slave, 
	// access to this data struct not serialised as writing to it only happens
	// when systemC kernel running
	IMGBUS_DATA_TYPE *m_slaveReadData;

	bool m_timedModel;

	// TIMED_MODEL
	bool m_slaveResponded; // this is set by SlaveReponse function, no need to serialise
	int m_numClockCycles; // number of clock cycles to allow sim to run for

	// UNTIMED_MODEL
	// these are used to block slave commands until it has been responded
	IMG_HANDLE m_slaveRespCond;
	bool m_doWaitResp; // true if response has not arrived

	IMG_HANDLE m_delayedRespLock; // to serialise access to delayed memory response list below
	std::list<TransifMemoryResponse *> m_delayedMemResponse;

	friend class TransifDevifHandler; // to enable accessing parent class
	// declare the devifHandler, and transif Devif
	TransifDevifHandler *m_pDevifHandler;
	DeviceInterfaceTransif *m_pDevIFTransif;
    
    // Functions for interuppt and slave stream handling
    sHandlerFuncs m_Handlers;
};

#endif /* only c++ */

#ifdef DLLEXPORT
#ifdef WIN32
#define DllExport __declspec(dllexport)
#else
#define DllExport
#endif
#else
#define DllExport
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
    interrupt_request pfnInterrupt;
    //slave_stream_handler pfnSlaveStream; // not there because it needs a class
    void * userData;
} TransifHandlerFuncs;

typedef struct _TransifAdaptorConfig
{
	const char *pszLibName;
	const char *pszDevifName;
	char **ppszArgs;
	int nArgCount;
	IMG_BOOL bTimedModel;
	IMG_BOOL bDebug;
	SYSMEM_eMemModel eMemType;
	IMG_UINT64 ui64BaseAddr;
	IMG_UINT64 ui64Size;
	IMG_BOOL   bForceAlloc;
	TransifHandlerFuncs sFuncs; // functions to use to handle interrupts
} TransifAdaptorConfig;

DllExport
IMG_HANDLE TransifAdaptor_Create(const TransifAdaptorConfig *pConfig);

DllExport
void TransifAdaptor_Destroy(IMG_HANDLE hAdaptor);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TRANSIF_ADAPTOR_H__ */

