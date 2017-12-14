/******************************************************************************
 * Name         : device_interface_master_transif.h
 * Title        : Master Device Interface for Transaction Interface
 * Author       : James Brodie
 * Created      : 09/03/2010
 *
 * Copyright    : 1997 by VideoLogic Limited. All rights reserved.
 * 				  No part of this software, either material or conceptual 
 * 				  may be copied or distributed, transmitted, transcribed,
 * 				  stored in a retrieval system or translated into any 
 * 				  human or computer language in any form by any means,
 * 				  electronic, mechanical, manual or other-wise, or 
 * 				  disclosed to third parties without the express written
 * 				  permission of VideoLogic Limited, Unit 8, HomePark
 * 				  Industrial Estate, King's Langley, Hertfordshire,
 * 				  WD4 8LZ, U.K.
 *
 * Description  : Master Device Interface for Transaction Interface
 *
 ******************************************************************************/
#ifndef DEVICE_INTERFACE_MASTER_TRANSIF
#define DEVICE_INTERFACE_MASTER_TRANSIF

#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))

#include "device_interface.h"
#include <string>
#include "device_interface_message.h"
#include "sim_wrapper_api.h"
#include "SimpleMemoryModel.h"

#ifdef MEOS_MUTEX
#include "krn.h"
#endif



// this is adaptation of C-sim that serialises register I/O
class TransifDevifModel
{
public:
	
	/**
	 * Constructor
	 * @param toIMGIP - the transif toIMGIP_if
	 * @param busWidth - transaction data bus width
	 */
	TransifDevifModel(toIMGIP_if* toIMGIP, IMG_UINT32 busWidth) : m_pIMGIP(toIMGIP), m_busWidth(busWidth)
	{ /* nothing to do */ }

	// Called by DeviceInterface::write/readRegister
	IMG_UINT32 RegisterIO( const IMGBUS_ADDRESS_TYPE &addr, const IMGBUS_DATA_TYPE &data, bool readNotWrite)
	{
		m_pIMGIP->SlaveRequest(addr, data, readNotWrite);
		IMG_UINT32 retValue;
		WaitForSlaveResponse(retValue);
		// by this stage is okay to access reg response value
		return retValue;
	}

	// Can be overwriten if not appropriate
	virtual void InitDevice()
	{
		m_pIMGIP->InitDevice();
	}

	// Can be overwriten if not appropriate
	virtual void DeinitDevice()
	{
		m_pIMGIP->Reset();
	}

	// need to implement this
	virtual void Idle(IMG_UINT32 numCycles) = 0;

	IMG_UINT32 GetTXBusWidth()
	{
		return m_busWidth;
	}

protected:
	// ptr to transif toIMGIP_if, which is returned by DLL when LoadSimulatorLibrary is called
	toIMGIP_if* m_pIMGIP;

	IMG_UINT32 m_busWidth;
	/**
	 * Must implement this, setting m_regResponseValue
	 * @param retValue - must be set if register write
	 */
	virtual void WaitForSlaveResponse(IMG_UINT32& retValue) = 0;

private:
	// no default or copy
	TransifDevifModel();
	TransifDevifModel(const TransifDevifModel&);
	TransifDevifModel& operator=(const TransifDevifModel&);

};
	


class DeviceInterfaceTransif : public DeviceInterface
{
	public:
	/**
	 * Constructor
	 * @param devifName - 'DEVICE' name if pdump config file
	 * @param pMemModel - ptr to memory model
	 * @params pSimulator - ptr to C-sim
	 */
	DeviceInterfaceTransif(string devifName, SimpleDevifMemoryModel* pMemModel, TransifDevifModel* pSimulator );

	/**
	 * Destructor
	 */
	virtual ~DeviceInterfaceTransif();

	virtual void initialise(void);
	virtual void deinitialise(void);
	virtual IMG_UINT32 readMemory(IMG_UINT64 deviceAddress);
	virtual IMG_UINT64 readMemory64(IMG_UINT64 deviceAddress);
	virtual IMG_UINT32 readRegister(IMG_UINT64 deviceAddress);
	virtual IMG_UINT64 readRegister64(IMG_UINT64 deviceAddress);
	virtual IMG_RESULT getDeviceTime(IMG_UINT64 * pui64Time);
	virtual void writeMemory(IMG_UINT64 deviceAddress, IMG_UINT32 value);
	virtual void writeMemory64(IMG_UINT64 deviceAddress, IMG_UINT64 value);
	virtual void writeRegister(IMG_UINT64 deviceAddress, IMG_UINT32 value);
	virtual void writeRegister64(IMG_UINT64 deviceAddress, IMG_UINT64 value);
	virtual void copyDeviceToHost(IMG_UINT64 deviceAddress, void *hostAddress, IMG_UINT32 numBytes);
	virtual void copyHostToDevice(const void *hostAddress, IMG_UINT64 deviceAddress, IMG_UINT32 numBytes);
	virtual void loadRegister(const void *hostAddress, IMG_UINT64 regOffset, IMG_UINT32 numWords);
	virtual void allocMem(IMG_UINT64 memAddress, IMG_UINT32 numBytes);
	virtual void freeMem(IMG_UINT64 memAddress);
	virtual void idle(IMG_UINT32 numCycles);
	virtual void setAutoIdle(IMG_UINT32 numCycles);
	virtual void sendComment(const IMG_CHAR* comment);
	virtual void devicePrint(const IMG_CHAR* string);

	virtual DeviceInterface* addNewDevice(std::string name, IMG_BOOL useBuffer)
	{
		(void)name; (void)useBuffer; // unused
		return IMG_NULL;
	}
	virtual void setBaseNames(std::string sRegName, std::string sMemName);

	private:
	
	DeviceInterfaceTransif();	// prevent default constructor

	// prevent default assignment and copy constructors from being created
	DeviceInterfaceTransif (const DeviceInterfaceTransif &foo);
	DeviceInterfaceTransif& operator=(const DeviceInterfaceTransif &foo);

protected:

	TransifDevifModel* m_pSimulator; // this is ptr to c-sim device
	SimpleDevifMemoryModel* m_pMemModel;
	
#ifdef MEOS_MUTEX
	KRN_IPL_T threadSafeOldPriority;
#endif

};

#endif // DEVICE_INTERFACE_MASTER
