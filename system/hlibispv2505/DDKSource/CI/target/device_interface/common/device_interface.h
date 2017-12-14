/*! 
******************************************************************************
 @file   device_interface.h

 @brief  C++ class interface to hardware device
 
 @Author Imagination Technologies

 @date   30/08/2013
 
         <b>Copyright 2013 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.
  
 @details This class is designed to provide an abstraction of a target device
		  the intention is that the calls should be the same whether connecting
		  to RTL Simulation / CSim / FPGA / Real Hardware etc
 


 ******************************************************************************/
#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include <map>
#include <string>

#include "img_types.h"

#include <cstdio>

#ifdef DOXYGEN_CREATE_MODULES

/**
 * @defgroup DEVICE_INTERFACE DeviceInterface
 * @brief c++ class for abstracting hardware devices
 *
 *  @details The device_interface is a c++ class used to access hardware, this interface is a base class on 
 *  which derived classes can be built to access different sorts of hardware model.
 * 
 * 
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the DEVICE_INTERFACE documentation module
 *------------------------------------------------------------------------*/
#endif

using std::map;
using std::string;

/**
 * This class should be used as a base class from which to derive new classes which can connect
 * to a piece of hardware
 */
class DeviceInterface 
{
public:
    typedef void (*InterruptCallback)(void *user, IMG_UINT64 level);
	typedef map<string, DeviceInterface*> DeviceMap;
    
    typedef enum CompareOperationType {
        CMP_OP_TYPE_EQ  = 0, //!< Is equal
        CMP_OP_TYPE_LT  = 1, //!< Is less than
        CMP_OP_TYPE_LTE = 2, //!< Is less than or equal
        CMP_OP_TYPE_GT  = 3, //!< Is greater than
        CMP_OP_TYPE_GTE = 4, //!< Is greater than or equal
        CMP_OP_TYPE_NE  = 5, //!< Is not equal
    } CompareOperationType;
	
    // Callbacks context structure
	struct Context {
        /*! Interrupt callback. It's the device interface implementation
            responsability to call this function on interrupts. */
        InterruptCallback interrupt;
        /*! User pointer */
        void *user;
    };

	public:
//!@name find device functions
//!@{ static functions used to find devices by name
	static void addToSystem(DeviceInterface& device);		//!< This static function is used to add a device interface based class to the map
															//!< This function is automatically called by the DeviceInterface constructor
	static void removeFromSystem(string deviceName);		//!< This static function is used to remove a device from the map
															//!< it is called automatically by the DeviceInterface destructor
	static DeviceInterface* find(string name);				//!< This static function allows a program to search for a named device
//!@}	
	
	DeviceInterface(string name);							//!< Class constructor @Input name : Unique Device Name
	virtual ~DeviceInterface(void) {}						//!< Class destructor
	
//!@name context callback functions
//!@{ Functions to set and retrieve the callbacks context
	virtual const DeviceInterface::Context &context(void) const { return context_; } //!< This function is used to retrieve the callbacks context
    virtual void setContext(const DeviceInterface::Context &context) { context_ = context; } //!< This functions sets the callbacks context
//!@}
    
	//! Ensure device interface has already been initialised
	void checkInitialised(){if (!bInitialised){printf("DeviceInterface not initialised\n"); throw;}}

//!@name essential functions
//!@{ Functions which must be implemented by a derived class	
	virtual void initialise(void) = 0;						//!< Call to setup target hardware
	virtual void deinitialise(void) = 0;					//!< Call to close target hardware
	
	virtual IMG_UINT32 readMemory(IMG_UINT64 deviceAddress) = 0;
	
	//! @brief read 32bits of data from device registers
	//! @param deviceAddress : Address of register to read
	virtual IMG_UINT32 readRegister(IMG_UINT64 deviceAddress) = 0;
	
	//! @brief write 32bits of data to memory
	//! @param deviceAddress : memory address to write to
	//! @param value : data to write
	virtual void writeMemory(IMG_UINT64 deviceAddress, IMG_UINT32 value) = 0;
	
	//! @brief write 32bits of data to device register
	//! @param deviceAddress : Address of register to write
	//! @param value : data to write
	virtual void writeRegister(IMG_UINT64 deviceAddress, IMG_UINT32 value) = 0;

	
	//! @brief block read from device
	//! @param deviceAddress : start address of memory to read
	//! @param hostAddress : buffer to read data into
	//! @param numBytes : size of data block to read (in bytes)
	virtual void copyDeviceToHost(IMG_UINT64 deviceAddress, void *hostAddress, IMG_UINT32 numBytes) = 0;
	
	//! @brief block write to device
	//! @param hostAddress : buffer containing data to write
	//! @param deviceAddress : start address of memory to write
	//! @param numBytes : size of data block to write (in bytes)
	virtual void copyHostToDevice(const void *hostAddress, IMG_UINT64 deviceAddress, IMG_UINT32 numBytes) = 0;
	
//!@}
	
//!@name extra functions
//!@{ Functions which can optionally be implented in a derived class to add functionality
	//! @brief read 64bits of data from memory
	//! @param deviceAddress : Address of physical memory to read
	virtual IMG_UINT64 readMemory64(IMG_UINT64 deviceAddress);
	
		//! @brief read 64bits of data from device registers
	//! @param deviceAddress : Address of register to read
	virtual IMG_UINT64 readRegister64(IMG_UINT64 deviceAddress);
	
	//! @brief write 64bits of data to memory
	//! @param deviceAddress : memory address to write to
	//! @param value : data to write
	virtual void writeMemory64(IMG_UINT64 deviceAddress, IMG_UINT64 value);
	
	//! @brief write 64bits of data to device register
	//! @param deviceAddress : Address of register to write
	//! @param value : data to write
	virtual void writeRegister64(IMG_UINT64 deviceAddress, IMG_UINT64 value);
	
	//! @brief block load to register
	//! @param hostAddress : buffer containing data to write
	//! @param regOffset : start address of register to write
	//! @param numWords : length of block to write (in 32bit words)
	virtual void loadRegister(const void *hostAddress, IMG_UINT64 regOffset, IMG_UINT32 numWords);
	
	//! @brief get the current simulation time for the device
	//! @param pui64Time : time value (filled in by function call)
	//! @return IMG_RESULT : IMG_SUCCESS if time is retrieved correctly
	virtual IMG_RESULT getDeviceTime(IMG_UINT64 * pui64Time);
	
	//! @brief Pass information about a memory block which has been allocated to the device
	//! This function is useful for simulations rather than real hardware
	//! @param memAddress : Address of physical memory which has been allocated
	//! @param numBytes : Size of allocation in bytes
	virtual void allocMem(IMG_UINT64 memAddress, IMG_UINT32 numBytes);
	
	//! @brief Request memory allocation from device
	//! 		useful when running on a system which uses main system memory
	//! @param alignment : Requested alignment of allocation
	//! @param numBytes : Size of allocation in bytes
	//! @return IMG_UINT64 : Address of memory allocated
	virtual IMG_UINT64 allocMemOnSlave(IMG_UINT64 alignment, IMG_UINT32 numBytes);
	
	//! @brief Inform device of memory block no longer required
	//! @param memAddress :  Address of memory previously allocated	(with either allocation function)
	virtual void freeMem(IMG_UINT64 memAddress);
	
		
	//! @brief Wait the desired number of cycles on the device
	//! @param numCycles : number of cycles to wait before returning
	virtual void idle(IMG_UINT32 numCycles);
	
	//! @brief This function is here to allow speeding up of simulations
	//!			particularly when the communication is slow to the device
	//!			If there are no commands waiting for the device it should
	//!			automatically allow this number of cycles to pass before
	//!			looking for a command again. Set to 0 to only run when a
	//!			command is passed.
	//! @param numCycles : number of cycles to pass while waiting for commands
	virtual void setAutoIdle(IMG_UINT32 numCycles){(void)numCycles;}
	
	//! @brief Attach a comment to the next command, this is a debugging aid
	//! @param comment : comment to attach to next command
	virtual void sendComment(const IMG_CHAR* comment){(void)comment;}
	
	//! @brief print a given string on the device output screen / log
	//! 		this is for debugging / progress reports
	//! @param string : string to print
	virtual void devicePrint(const IMG_CHAR* string){(void)string;}
    
    //! @brief Poll a 32-bit register for a specified value change
    //! @param ui64RegAddr : register address
    //! @param eCmpOpType : compare operation type
    //! @param ui32ReqVal : requested value
    //! @param ui32EnableMask : enable mask
    //! @param ui32PollCount : number of times to poll for a match
    //! @param ui32CycleCount : number of cycles to wait between polls
    //! @return IMG_RESULT : IMG_SUCCESS if poll is successful,
    //!                      IMG_ERROR_TIMEOUT if poll times out,
    //!                      IMG_ERROR_NOT_SUPPORTED if poll to device is not supported or
    //!                      IMG_ERROR_GENERIC_FAILURE for an unspecified failure
    virtual IMG_RESULT pollRegister(IMG_UINT64 ui64RegAddr, CompareOperationType eCmpOpType,
        IMG_UINT32 ui32ReqVal, IMG_UINT32 ui32EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount);
    
    //! @brief Poll a 64-bit register for a specified value change
    //! @param ui64RegAddr : register address
    //! @param eCmpOpType : compare operation type
    //! @param ui64ReqVal : requested value
    //! @param ui64EnableMask : enable mask
    //! @param ui32PollCount : number of times to poll for a match
    //! @param ui32CycleCount : number of cycles to wait between polls
    //! @return IMG_RESULT : IMG_SUCCESS if poll is successful,
    //!                      IMG_ERROR_TIMEOUT if poll times out,
    //!                      IMG_ERROR_NOT_SUPPORTED if poll to device is not supported or
    //!                      IMG_ERROR_GENERIC_FAILURE for an unspecified failure
    virtual IMG_RESULT pollRegister(IMG_UINT64 ui64RegAddr, CompareOperationType eCmpOpType,
        IMG_UINT64 ui64ReqVal, IMG_UINT64 ui64EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount);
    
    //! @brief Poll a 32-bit memory address for a specified value change
    //! @param ui64MemAddr : memory address
    //! @param eCmpOpType : compare operation type
    //! @param ui32ReqVal : requested value
    //! @param ui32EnableMask : enable mask
    //! @param ui32PollCount : number of times to poll for a match
    //! @param ui32CycleCount : number of cycles to wait between polls
    //! @return IMG_RESULT : IMG_SUCCESS if poll is successful,
    //!                      IMG_ERROR_TIMEOUT if poll times out,
    //!                      IMG_ERROR_NOT_SUPPORTED if poll to device is not supported or
    //!                      IMG_ERROR_GENERIC_FAILURE for an unspecified failure
    virtual IMG_RESULT pollMemory(IMG_UINT64 ui64MemAddr, CompareOperationType eCmpOpType,
        IMG_UINT32 ui32ReqVal, IMG_UINT32 ui32EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount);
    
    //! @brief Poll a 64-bit memory address for a specified value change
    //! @param ui64MemAddr : memory address
    //! @param eCmpOpType : compare operation type
    //! @param ui64ReqVal : requested value
    //! @param ui64EnableMask : enable mask
    //! @param ui32PollCount : number of times to poll for a match
    //! @param ui32CycleCount : number of cycles to wait between polls
    //! @return IMG_RESULT : IMG_SUCCESS if poll is successful,
    //!                      IMG_ERROR_TIMEOUT if poll times out,
    //!                      IMG_ERROR_NOT_SUPPORTED if poll to device is not supported or
    //!                      IMG_ERROR_GENERIC_FAILURE for an unspecified failure
    virtual IMG_RESULT pollMemory(IMG_UINT64 ui64MemAddr, CompareOperationType eCmpOpType,
        IMG_UINT64 ui64ReqVal, IMG_UINT64 ui64EnableMask, IMG_UINT32 ui32PollCount, IMG_UINT32 ui32CycleCount);

	string&		getName(){return name;} 

	virtual DeviceInterface* addNewDevice(std::string name, IMG_BOOL useBuffer){(void)useBuffer;return IMG_NULL;}
	virtual void setBaseNames(std::string sRegName, std::string sMemName){}

//!@}	

	string		name;					//!< Device name given in constructor
	Context     context_;               //!< Callbacks context
	bool		bInitialised;			//!< Interface is initialised?
	private:

	DeviceInterface();	//!< prevent default constructor

	//! prevent default assignment and copy constructors from being created
	DeviceInterface (const DeviceInterface &foo);
	DeviceInterface& operator=(const DeviceInterface &foo);
	
	static DeviceMap devices;
};

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the DEVICE_INTERFACE documentation module
 *------------------------------------------------------------------------*/
#endif

#endif
