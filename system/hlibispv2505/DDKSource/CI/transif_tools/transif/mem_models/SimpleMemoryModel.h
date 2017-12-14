#ifndef __SIMPLEMEMORYMODEL_H__
#define __SIMPLEMEMORYMODEL_H__

#include "img_include.h"
#include "sim_wrapper_api.h"

// this is the interface class representing SIMLE memory IO
class SimpleDevifMemoryModel {
public:
    virtual ~SimpleDevifMemoryModel(void) { }
	// called by DeviceInterface::freeMem
	virtual void free(IMG_UINT64 uBaseAddress) = 0;
	// called by DeviceInterface::allocMem
	virtual IMG_BOOL alloc(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize) = 0;
	// called by DeviceInteface::readMemory
	virtual IMG_UINT32 read(IMG_UINT64 address) = 0;
	// called by DeviceInterface::writeMemory
	virtual void write(IMG_UINT64 address, IMG_UINT32 data) = 0;
	// called by DeviceInterface::copyHostToDevice
	virtual void block_write(IMG_UINT64 address, IMG_BYTE* const pData, IMG_UINT64 uLength) = 0;
	// called by DeviceInterface::copyDeviceToHost
	virtual void block_read(IMG_UINT64 address, IMG_BYTE* pData, IMG_UINT64 uLength) = 0;
    
    /*
    *This is only ever called if CSim implements internal memory access, default no implementation
    *@param tag_id - tag ID used in request from host/driver
    *@data - for read operation, this is data read from CSim, can ignore if operation was Write access
    */
    virtual void csim_mem_access_cb( toIMGIP_if* pSimulator, IMG_UINT32 tag_id, const IMGBUS_DATA_TYPE& data)
    {
        // default unsupported
        (void)pSimulator; (void)tag_id; (void)data; // unused
    }
};

#endif /* __SIMPLEMEMORYMODEL_H__ */
