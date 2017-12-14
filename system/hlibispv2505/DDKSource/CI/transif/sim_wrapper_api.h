/*****************************************************************************
 * Name         : sim_wrapper_api.h
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

#ifndef SIM_WRAPPER_HEADER
#define SIM_WRAPPER_HEADER

#include "img_types.h"

#ifdef DLLEXPORT
#ifdef WIN32
#define DllExport extern "C" __declspec( dllexport)
#else
#define DllExport extern "C"
#endif
#else
#define DllExport
#endif

typedef unsigned int imgbus_transaction_id;

/**
 * Transaction Control Data used in IMGBus
 */
struct imgbus_transaction_control
{
	imgbus_transaction_id	tag_id;

	// sideband_tag_id: additional information (e.q. master id) that must be pipelined
	imgbus_transaction_id	sideband_tag_id;

	int *write_mask_ptr;

	imgbus_transaction_control()
		: tag_id(0)
		, sideband_tag_id(0)
		, write_mask_ptr(0)
	{
	}

};

enum imgbus_status
{ 
	IMGBUS_REQUEST,
	IMGBUS_WAIT,
	IMGBUS_OK,
	IMGBUS_ERROR,
	IMGBUS_ADDRESS_WAIT_OK,		// status after a delayed address phase response
	IMGBUS_ADDRESS_WAIT_ERROR,
	IMGBUS_DATA_WAIT_OK,		// status after a delayed data phase response
	IMGBUS_DATA_WAIT_ERROR,
	IMGBUS_NOT_FOUND			// the return status isn't stored in the internal channel buffers anymore
};

enum IMG_CONTROL_MSG
{
	// setter control messages
	IMG_CONTROL_SET_DEBUG_LEVEL,

	// getter control messages
	IMG_CONTROL_GET_PREFERRED_NUM_CLOCK_CYCLES, 
	IMG_CONTROL_GET_BUS_WIDTH
};

/**
 * Convenience Methods for operating on IMGBUS_ADDRESS structures
 */
struct IMGBUS_ADDRESS_TYPE
{
	IMG_UINT64 address;

	IMGBUS_ADDRESS_TYPE()
	{
	}

	IMGBUS_ADDRESS_TYPE( const IMGBUS_ADDRESS_TYPE& a)
		: address( a.address )
	{
	}

	IMGBUS_ADDRESS_TYPE(IMG_UINT64 a)
		: address( a )
	{
	}

	operator IMG_UINT64() const
	{
		return address;
	}

	IMGBUS_ADDRESS_TYPE& operator++()
	{
		address++;
		return (*this);
	}
};

/**
 * Convenience Methods for operating on IMGBUS_DATA structures
 */
class IMGBUS_DATA_TYPE
{
private:
	IMG_UINT32 dataInternal[8];	// large enough for a single 256-bit word

	IMG_UINT32 numBusWords;
	IMG_UINT32 busWidthInWords;

	// prevent default assignment and copy constructors from being created
	IMGBUS_DATA_TYPE(const IMGBUS_DATA_TYPE &IMGBUS_DATA_TYPE);
	IMGBUS_DATA_TYPE& operator=(const IMGBUS_DATA_TYPE &IMGBUS_DATA_TYPE);

	IMG_UINT32 * initDataArray()
	{
		IMG_UINT32 sizeInWords = numBusWords * busWidthInWords;

		if (sizeInWords * sizeof(IMG_UINT32) > sizeof(dataInternal)) {
			return new IMG_UINT32[sizeInWords];
		}
		return dataInternal;
	}

public:
	IMG_UINT32 *const data;

	// constructor
	IMGBUS_DATA_TYPE(IMG_UINT32 busWidthInBits, IMG_UINT32 numBusWords = 1)
		: numBusWords(numBusWords)
		, busWidthInWords(busWidthInBits / 32)
		, data(initDataArray())
	{
	}

	// copy constructor
	IMGBUS_DATA_TYPE(const IMGBUS_DATA_TYPE *d, IMG_UINT32 numBusWords)
		: numBusWords(numBusWords)
		, busWidthInWords(d->busWidthInWords)
		, data(initDataArray())
	{
		copyInFrom(d->data, numBusWords);
	}

	// copy constructor
	IMGBUS_DATA_TYPE(const IMGBUS_DATA_TYPE& d, IMG_UINT32 numBusWords)
		: numBusWords(numBusWords)
		, busWidthInWords(d.busWidthInWords)
		, data(initDataArray())
	{
		copyInFrom(d.data, numBusWords);
	}

	// destructor
	~IMGBUS_DATA_TYPE()
	{
		if (data != dataInternal) delete[] data;
	}

	operator IMG_UINT32 *const() const
	{
		return data;
	}

	void copyInFrom(const IMG_UINT32 *srcPtr, IMG_UINT32 numWords)
	{
		IMG_UINT32 *dstPtr = data;

		if (numWords > numBusWords) {
			numWords = numBusWords;
		}

		for (IMG_UINT32 i = 0; i < numWords; i++) {
			for (IMG_UINT32 j = 0; j < busWidthInWords; j++) {
				*dstPtr++ = *srcPtr++;
			}
		}
	}

	void copyOutTo(IMG_UINT32 *dstPtr, IMG_UINT32 numWords) const
	{
		IMG_UINT32 *srcPtr = data;

		if (numWords > numBusWords) {
			numWords = numBusWords;
		}

		for (IMG_UINT32 i = 0; i < numWords; i++) {
			for (IMG_UINT32 j = 0; j < busWidthInWords; j++) {
				*dstPtr++ = *srcPtr++;
			}
		}
	}

	IMG_UINT32 getWidthInWords() const		// this is the number of 32-bit words in a bus word
	{
		return busWidthInWords;
	}
	IMG_UINT32 getBurstLength() const			// this is the number of bus words in this object
	{
		return numBusWords;
	}
};

/**
 * Interface to be implemented by a Client of IMGIP.  
 */
template< typename ADDRESS = IMGBUS_ADDRESS_TYPE, typename DATA = IMGBUS_DATA_TYPE>
class fromIMGIP
{
public:

	/**
	 * Callback from Simulator after a SlaveRequest.  Data is returned in same sequence as Requests
	 *
	 * @param d			- data returned when request is a read, if request was a write, this data can be ignored
	 * @return			- imgbus status
	 */
	virtual imgbus_status SlaveResponse(const DATA& data) = 0;
	
	/**
	 * Callback from Simulator after a Memory Access SlaveRequest.
	 * @param tag_id		- the tag_id from transaction control used in SlaveRequest
	 * @param data			- returned data for read operation, for a write operation, this can be ignored
	 * @param acknowledged	- the callee should set this value to true if data is immediately accepted, <Simulator, currently
	 *						  assumes that that ack is immediate, but may later support delayed acknowledge>
	 */
	virtual void SlaveResponse(IMG_UINT32 tag_id, const DATA& data, bool &acknowledged) = 0;

	/**
	 * Initiates a transaction for memory access.
	 *
	 * @param requestID		- the ID associated with this request, should be used in the response
	 * @param a				- address in Master to access
	 * @param data			- Array is allocated by  ImgIP, and valid only in this call.  For write operation, contains data to write.
	 *						  For a read operation, the Master (callee) may update this structure. 
	 * @param readNotWrite	- true for memory read, else memory write
	 * @param ctrl			-  imgbus control data
	 * @param acknowledged	- if true, IMG IP considers the request accepted
	 * @param responded		- if true, IMG IP considers the request complemeted and data to be valid for a read.
	 */
	virtual void MasterRequest(const ADDRESS addr, DATA* data, bool readNotWrite, const imgbus_transaction_control &ctrl, bool& acknowledged, bool& responded) = 0;
	
	/**
	 * Signals from IMG IP to Master that the previous MasterResponse call has been accepted
	 *
	 * @params tag_id	- ID associated with the original MasterRequest call
	 */
	virtual void MasterResponseAcknowledge(IMG_UINT32 tag_id = 0) = 0;
	
	/**
	 * Receives a dump of data from the slave which, according to keyID, represents some
	 * information about the IMG_IP (state, version ,etc).  This function is called by
	 * the slave whenever it has useful data available.
	 *
	 * @param keyID		- key identifying agreed data
	 * @param data		- this array is only valid during this call
	 * @return			- number of words in the data
	 */
	virtual void SlaveStreamOut( const IMG_UINT32 keyID, const DATA& data) = 0;

	/**
	 * This is a request from the slave for information about the master platform (state, version ,etc)
	 *
	 * @param keyID		- key identifying agreed data
	 * @param d_ptr		- the requested data (if responded is returned true)
	 * @param responded	- if returned true, the completed request is in data, otherwise
	 *					  the data will be sent in a later call to MasterInfoResponse
	 */
	 virtual void MasterInfoRequest( const IMG_UINT32 keyID, DATA *d_ptr, bool& responded) = 0;

	/**
	 * Callback from Simulator in response to a previous SlaveInfoRequest call
	 *
	 * @param keyID		- key identifying agreed data
	 * @param d_ptr		- Master data. This array is only valid during this call.
	 */
	virtual void SlaveInfoResponse(IMG_UINT32 keyID, const DATA *d_ptr) = 0;

	/**
	 * This function is called by the slave to update the master about the state of up to 32 interrupt lines.
	 */
	virtual void SlaveInterruptRequest(IMG_UINT32 interruptLines) = 0;

	/**
	 * This function is called when Simulation has completed a test, used in the External Memory and Timing model.
	 */
	virtual void EndOfTest() = 0;

	// Destructor
	virtual ~fromIMGIP() {};
};


/**
 * Interface that is implemented by an IMG Bus Slave.  IMG Simulators implement this
 */
template< typename ADDRESS = IMGBUS_ADDRESS_TYPE, typename DATA = IMGBUS_DATA_TYPE>
class toIMGIP
{
public:
	
	/**
	 * Sends a Request to Simulator.  This function only posts queues a request on the Simulator,
	 * and if accepted will return IMGBUS_OK.  When this request is completed, the Requester is
	 * notified by a call to SlaveResponse()
	 *
	 * @param a				- address in Slave to write to
	 * @param d				- structure containing data to write to Slave.  This field is ignored when reading from Simulator
	 * @param readNotWrite	- True, request is to read data, False, request to write data
	 * @return imgbus_status
	 */
	virtual imgbus_status SlaveRequest( const ADDRESS &addr, const DATA &data, bool readNotWrite) = 0;
	
	
	/**
	 * Initiates a transaction for Slave memory access. <-- NOTE: Some simulators DO-NOT support this feature -- >
	 *
	 * @param requestID		- the ID associated with this request, should be used in the response
	 * @param a				- address in slave to access
	 * @param data			- Structure containing data to write to Slave.  Slave does not use this as return value for read operation.
	 *						  Data for read operation is returned in corresponding SlaveResponse() callback.  In both read and write
	 *						  operations, this data should indicate the busWidth and burstLength of the request
	 * @param readNotWrite	- true for memory read, else memory write
	 * @param ctrl			-  imgbus control data
	 * @param acknowledged	- if true, IMG IP has accepted this request
	 */
	virtual void SlaveRequest(const ADDRESS &addr, const DATA &data, bool readNotWrite, const imgbus_transaction_control &ctrl, bool& acknowledged) = 0;

	/**
	 * Reset Slave.  User must not call this function unless the device is idling.
	 */
	virtual imgbus_status Reset() = 0;
	
	/**
	 * Initialise Device
	 */
	virtual imgbus_status InitDevice() = 0;
	
	/**
	 * Instruct the slave to run for the specified number of clock cycles
	 *
	 * @param time		- number of clock cycles
	 */
	virtual imgbus_status Clock(const IMG_UINT32 time = 1) = 0;
	
	/**
	 * Callback to IMG IP that the transcation of the specified ID has completed.
	 *
	 * @param tag_id		- the tag_id from transaction control used in MasterRequest
	 * @param data			- returned data for read operation, this should be allocated by caller, and have sufficient length
	 *						  required by the request's transaction control burst length
	 * @param acknowledged	- IMG IP will set this value to true if data is immediately accepted
	 */
	virtual void MasterResponse(IMG_UINT32 tag_id, const DATA *data, bool& acknowledged) = 0;

	/**
	 * Callback to IMG IP that a memory request has been accepted.  This is only called if the original MasterRequest
	 * was not accepted immediately.
	 *
	 * @param tag_id	- the tag_id from transaction control used in MasterRequest
	 */
	virtual void MasterRequestAcknowledge(IMG_UINT32 tag_id = 0) = 0;
	
	/**
	 * Receives a dump of data from the master which, according to keyID, represents some
	 * information from the customer platform (state, version ,etc)
	 *
	 * @param keyID		- key identifying agreed data
	 * @param data		- this array is only valid during this call
	 */
	virtual void MasterStreamOut( const IMG_UINT32 keyID, const DATA& data) = 0;
	
	/**
	 * Send or receive control information.  For keyID values corresponding to requests
	 * the DATA item will be used to store the information.
	 *
	 * @param keyID		- key identifying agreed data
	 * @param data		- this array is only valid during this call
	 */
	 virtual void SlaveControl( const IMG_UINT32 keyID, DATA *data, bool& responded) = 0;
	
	/**
	 * This is a request from the master for information about the slave simulator (state, version ,etc)
	 *
	 * @param keyID		- key identifying agreed data
	 * @param data		- the requested data (if responded is returned true)
	 * @param responded	- if returned true, the completed request is in data, otherwise
	 *					  the data will be sent in a later call to GetDataResponseToMaster
	 */
	 virtual void SlaveInfoRequest( const IMG_UINT32 keyID, DATA *d_ptr, bool& responded) = 0;

	/**
	 * Callback from customer platform in response to a previous MasterInfoRequest call
	 *
	 * @param keyID		- key identifying agreed data
	 * @param d_ptr		- Slave data.  This array is only valid during this call.
	 */
	virtual void MasterInfoResponse(IMG_UINT32 keyID, const DATA *d_ptr) = 0;

	/**
	 * This function is called by the master to update the slave about the state of up to 32 interrupt lines.
	 */
	virtual void MasterInterruptRequest(IMG_UINT32 interruptLines) = 0;

	// Destructor
	virtual ~toIMGIP() {};
};

typedef toIMGIP<IMGBUS_ADDRESS_TYPE, IMGBUS_DATA_TYPE> toIMGIP_if;
typedef fromIMGIP<IMGBUS_ADDRESS_TYPE, IMGBUS_DATA_TYPE> fromIMGIP_if;

class HostCallbacks;	// forward declare this class

/**
 * Factory Method to return the Slave model (Simulator).
 * Calling Function must deallocate model using 'UnloadSimulatorLibrary'

 * @param lib_name		- name of simulator DLL (i.e. msvdx_transif.dll)
 * @param id			- ID for simulator type to get.  0 for IMGIP only model, 1 for IMGIP + integrated PdumpPlayer
 * @param imgIP_client	- implementation of a client that uses imgIP
 * @param argc			- number of command line args used for invoking Simulator
 * @param argv			- command line args used for invoking Simulator
 * @return				- Interface of simulator, NULL if no such ID, or IP client interface not passed in
 */
DllExport toIMGIP_if* LoadSimulatorLibrary(const char *lib_name,
										   unsigned int id,
										   fromIMGIP_if* imgIP_client,
										   int argc,
										   char** argv);
/**
 * Used for unloading simulator library
 */
DllExport void UnloadSimulatorLibrary(toIMGIP_if* pSimInstance);

/* 
 *	New extern "C" style bridge interface to DLL
 */

DllExport void	InitLibrary(unsigned int id, fromIMGIP_if *hostHandle, const HostCallbacks& hostCallbacks, int argc, char** argv);

// functions that are part of the toIMGIP class
DllExport imgbus_status	Reset();
DllExport imgbus_status	InitDevice();
DllExport imgbus_status	Clock						(const IMG_UINT32 time);
DllExport imgbus_status	SlaveRequest				(const IMGBUS_ADDRESS_TYPE &addr, const IMGBUS_DATA_TYPE &data, bool readNotWrite);
// exported name for slave memory request cannot be overloaded, is c-style
DllExport void			SlaveRequestIntMem			(const IMGBUS_ADDRESS_TYPE &addr, const IMGBUS_DATA_TYPE &data, bool readNotWrite, 
													const imgbus_transaction_control &ctrl, bool& acknowledged);
DllExport void			MasterResponse				(IMG_UINT32 tag_id, const IMGBUS_DATA_TYPE *data, bool& acknowledged);
DllExport void			MasterRequestAcknowledge	(IMG_UINT32 tag_id);
DllExport void			MasterStreamOut				(const IMG_UINT32 keyID, const IMGBUS_DATA_TYPE& data);
DllExport void			SlaveControl				(const IMG_UINT32 keyID, IMGBUS_DATA_TYPE *d_ptr, bool& responded);
DllExport void			SlaveInfoRequest			(const IMG_UINT32 keyID, IMGBUS_DATA_TYPE *d_ptr, bool& responded);
DllExport void			MasterInfoResponse			(IMG_UINT32 keyID, const IMGBUS_DATA_TYPE *d_ptr);
DllExport void			MasterInterruptRequest		(IMG_UINT32 interruptLines);

class SimWrapperCBridge : public toIMGIP_if
{
	public:

	typedef IMGBUS_ADDRESS_TYPE		ADDRESS;
	typedef IMGBUS_DATA_TYPE		DATA;

	SimWrapperCBridge(const char *libName, unsigned int id, fromIMGIP_if* imgIP_client, int argc, char** argv);
	~SimWrapperCBridge();

	bool	isValid() { return bValid; }

	// need to implement these functions for the toIMGIP_if part of the class
	virtual imgbus_status Reset();
	virtual imgbus_status InitDevice();
	virtual imgbus_status Clock				(const IMG_UINT32 time = 1);
	virtual imgbus_status SlaveRequest		(const ADDRESS &addr, const DATA &data, bool readNotWrite);
	// this one is extension for Slave memory access
	virtual void SlaveRequest				(const ADDRESS &addr, const DATA &data, bool readNotWrite, const imgbus_transaction_control &ctrl, bool& acknowledged);
	virtual void MasterResponse				(IMG_UINT32 tag_id, const DATA *data, bool& acknowledged);
	virtual void MasterRequestAcknowledge	(IMG_UINT32 tag_id = 0);
	virtual void MasterStreamOut			(const IMG_UINT32 keyID, const DATA& data);
	virtual void SlaveControl				(const IMG_UINT32 keyID, DATA *d_ptr, bool& responded);
	virtual void SlaveInfoRequest			(const IMG_UINT32 keyID, DATA *d_ptr, bool& responded);
	virtual void MasterInfoResponse			(IMG_UINT32 keyID, const DATA *d_ptr);
	virtual void MasterInterruptRequest		(IMG_UINT32 interruptLines);

	private:

	void *lookupSymbol(const char *name);

	// function pointer to initialise the library
	typedef void (*FPTR_INIT_LIB)(unsigned int id, const fromIMGIP_if * hostHandle, const HostCallbacks& hostCallbacks, int argc, char** argv);
	// function to deallocate library
	typedef void (*FPTR_DEL_LIB)();

	FPTR_INIT_LIB		pFuncInitLib;
	FPTR_DEL_LIB		pFuncDelLib;

	// one function pointer for each virtual functions in the toIMGIP_if part of the class
	typedef imgbus_status (*FPTR_RESET)						();
	typedef imgbus_status (*FPTR_INIT_DEVICE)				();
	typedef imgbus_status (*FPTR_CLOCK)						(const IMG_UINT32 time);
	typedef imgbus_status (*FPTR_SLAVE_REQUEST)				(const ADDRESS &addr, const DATA &data, bool readNotWrite);
	typedef void	(*FPTR_MASTER_RESPONSE)					(IMG_UINT32 tag_id, const DATA *data, bool& acknowledged);
	typedef void	(*FPTR_MASTER_REQUEST_ACKNOWLEDGE)		(IMG_UINT32 tag_id);
	typedef void	(*FPTR_MASTER_STREAM_OUT)				(const IMG_UINT32 keyID, const DATA& data);
	typedef void	(*FPTR_SLAVE_CONTROL)					(const IMG_UINT32 keyID, DATA *d_ptr, bool& responded);
	typedef void	(*FPTR_SLAVE_INFO_REQUEST)				(const IMG_UINT32 keyID, DATA *d_ptr, bool& acknowledged);
	typedef void	(*FPTR_MASTER_INFO_RESPONSE)			(IMG_UINT32 keyID, const DATA *d_ptr);
	typedef void	(*FPTR_MASTER_INTERRUPT_REQUEST)		(IMG_UINT32 interruptLines);
	// extensions for Slave internal memory access
	typedef void	(*FPTR_SLAVE_REQUEST_INT_MEM)			(const ADDRESS &addr, const DATA &data, bool readNotWrite,
															const imgbus_transaction_control &ctrl, bool& acknowledged);

	FPTR_RESET							pFuncReset;
	FPTR_INIT_DEVICE					pFuncInitDevice;
	FPTR_CLOCK							pFuncClock;
	FPTR_SLAVE_REQUEST					pFuncSlaveRequest;
	FPTR_MASTER_RESPONSE				pFuncMasterResponse;
	FPTR_MASTER_REQUEST_ACKNOWLEDGE		pFuncMasterRequestAcknowledge;
	FPTR_MASTER_STREAM_OUT				pFuncMasterStreamOut;
	FPTR_SLAVE_CONTROL					pFuncSlaveControl;
	FPTR_SLAVE_INFO_REQUEST				pFuncSlaveInfoRequest;
	FPTR_MASTER_INFO_RESPONSE			pFuncMasterInfoResponse;
	FPTR_MASTER_INTERRUPT_REQUEST		pFuncMasterInterruptRequest;
	FPTR_SLAVE_REQUEST_INT_MEM			pFuncSlaveRequestIntMem;

	bool	bValid;
	void * libHandle;
};

// ################################################################################

/* classes for wrapping the fromIMGIP interface */

class HostCallbacks
{
	public:

	typedef IMGBUS_ADDRESS_TYPE		ADDRESS;
	typedef IMGBUS_DATA_TYPE		DATA;

	HostCallbacks();
	
	typedef imgbus_status (*PFUNC_SLAVE_RESPONSE)		(void * hostHandle, const IMGBUS_DATA_TYPE& data);
	typedef void (*PFUNC_MASTER_REQUEST)				(void * hostHandle, const ADDRESS &addr, DATA* data, bool readNotWrite, const imgbus_transaction_control &ctrl, bool& acknowledged, bool& responded);
	typedef void (*PFUNC_MASTER_RESPONSE_ACKNOWLEDGE)	(void * hostHandle, IMG_UINT32 tag_id);
	typedef void (*PFUNC_SLAVE_STREAM_OUT)				(void * hostHandle, const IMG_UINT32 keyID, const DATA& data);
	typedef void (*PFUNC_MASTER_INFO_REQUEST)			(void * hostHandle, const IMG_UINT32 keyID, IMGBUS_DATA_TYPE *d_ptr, bool& responded);
	typedef void (*PFUNC_SLAVE_INFO_RESPONSE)			(void * hostHandle, IMG_UINT32 keyID, const DATA *d_ptr);
	typedef void (*PFUNC_SLAVE_INTERRUPT_REQUEST)	(void * hostHandle, IMG_UINT32 interruptLines);
	typedef void (*PFUNC_END_OF_TEST)					(void * hostHandle);
	typedef void (*PFUNC_SLAVE_RESPONSE_INT_MEM)		(void * hostHandle, IMG_UINT32 tag_id, const DATA& data, bool &acknowledged);

	PFUNC_SLAVE_RESPONSE				pFuncSlaveResponse;
	PFUNC_MASTER_REQUEST				pFuncMasterRequest;
	PFUNC_MASTER_RESPONSE_ACKNOWLEDGE	pFuncMasterResponseAcknowledge;
	PFUNC_SLAVE_STREAM_OUT				pFuncSlaveStreamOut;
	PFUNC_MASTER_INFO_REQUEST			pFuncMasterInfoRequest;
	PFUNC_SLAVE_INFO_RESPONSE			pFuncSlaveInfoResponse;
	PFUNC_SLAVE_INTERRUPT_REQUEST		pFuncSlaveInterruptRequest;
	PFUNC_END_OF_TEST					pFuncEndOfTest;
	PFUNC_SLAVE_RESPONSE_INT_MEM		pFuncSlaveResponseIntMem;

	private:

};

#endif // SIM_WRAPPER_HEADER

