#ifndef SYSMEMALLOC
#define SYSMEMALLOC


#include "img_defs.h"
#include "SimpleMemoryModel.h"
#include <map>

using namespace std;

template <typename AddressType>
struct system_mem_allocation;

class system_mem_alloc : public SimpleDevifMemoryModel {

public:
	system_mem_alloc(bool forceAlloc = false);

	~system_mem_alloc();
	
	void write(IMG_UINT64 address, IMG_UINT32 uData);
	void block_write(IMG_UINT64 address, IMG_BYTE* const pData, IMG_UINT64 uLength);
	IMG_UINT32 read(IMG_UINT64 address);
	void block_read(IMG_UINT64 address, IMG_BYTE* pData, IMG_UINT64 uLength);
	IMG_BOOL alloc(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize);
	IMG_BOOL map(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize, IMG_VOID* pvBuffer);
	void free(IMG_UINT64 uBaseAddress);

	IMG_PUINT8 getMemPtr(IMG_UINT64 uBaseAddress);
private:

	// system memory uses memory alloc for IMG_UINT64 address, as used in pdump
	typedef system_mem_allocation<IMG_UINT64> Allocation;
	
	Allocation* find_allocation(IMG_UINT64 uAddress, IMG_UINT32 uSize);

	// Simple cache of the last page - optimisation
	Allocation*			pLastAllocation;
	//IMG_UINT64			uLastKey;
	IMG_UINT64			uTotalAllocatedMemory;

	IMG_UINT64			uMaxAddress;
	IMG_UINT64			uMaxAllocation;

	IMG_HANDLE 			m_lastAllocLock; // lock access to pLastAllocation, needs to be serialised
	std::map<IMG_UINT64, Allocation*> memoryAllocationMap;

	bool m_forceAlloc;
};

#endif
