#include "device_interface_transif.h"


class system_mem_prealloc : public SimpleDevifMemoryModel {

public:
	system_mem_prealloc(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize);

	~system_mem_prealloc();
	
	void write(IMG_UINT64 address, IMG_UINT32 uData);
	void block_write(IMG_UINT64 address, IMG_BYTE* const pData, IMG_UINT64 uLength);
	IMG_UINT32 read(IMG_UINT64 address);
	void block_read(IMG_UINT64 address, IMG_BYTE* pData, IMG_UINT64 uLength);
	IMG_BOOL alloc(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize)
	{
		(void)uBaseAddress; (void)uSize; // unused
		return IMG_TRUE;
	}
	void free(IMG_UINT64 uBaseAddress)
	{ 
		(void)uBaseAddress; // unused 
	}
private:
	system_mem_prealloc();
	IMG_CHAR * pMem;
	IMG_UINT64 uiBaseAddress;
	IMG_UINT32 uiSize;
};
