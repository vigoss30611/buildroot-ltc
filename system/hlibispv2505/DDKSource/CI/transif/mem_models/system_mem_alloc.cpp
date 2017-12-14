#include <map>
#include <errno.h>
#include <iostream>

#ifdef WIN32
#	include <windows.h>
#   define getpagesize() (4 * 1024)
#else
#	include <sched.h>
#	include <unistd.h>
#endif

#include "img_defs.h"
#include "system_mem_alloc.h"
#include "osa.h"

//#define DEBUG_OUTPUT
//#define DELAYED_MEM_REQ_ACK
//#define INSTANT_MEM_RESP


using namespace std;
#ifdef TRANSIF_LOGGING
FILE* Log_file;
#endif
template <typename AddressType>
struct system_mem_allocation {

public:
	IMG_BOOL contains(AddressType Address, IMG_UINT32 uRequestSize = 1) {
		return ((Address >= uBaseAddress) && (Address + uRequestSize <= uBaseAddress + uSize));
	}
	IMG_BOOL intersects(AddressType Address, IMG_UINT32 uRequestSize = 1) {
		return (!((Address + uRequestSize <= uBaseAddress) || (Address >= uBaseAddress + uSize)));
	}

	system_mem_allocation(AddressType uBaseAddress, 
		AddressType uSize, IMG_BYTE* pbBuffer)
		: uBaseAddress(uBaseAddress)
		, uSize(uSize)
        , pMemory(pbBuffer?pbBuffer:new IMG_BYTE[(IMG_UINT32)uSize])
		, bMapped(pbBuffer?IMG_TRUE:IMG_FALSE)
		
	{
		#ifdef TRANSIF_LOGGING
		Log_file=fopen("memory_log.log","a");
		#endif
		//assert((uBaseAddress & (sizeof(IMG_UINT32) -1)) == 0);
	}

	~system_mem_allocation(void)
	{
		#ifdef TRANSIF_LOGGING
		fclose(Log_file);
		#endif
		if (!bMapped)
			delete[] pMemory;
	}

	void write(AddressType address, IMG_UINT32 uData)
	{ 
		address -= uBaseAddress;
		//assert((address < uSize) && (((address & (sizeof(IMG_UINT32) -1)) == 0)));
		reinterpret_cast<IMG_UINT32*>(pMemory)[address / sizeof(IMG_UINT32)] = uData;
		#ifdef TRANSIF_LOGGING
		fprintf(Log_file,"Written 4 bytes of data at 0x%x \n",address+uBaseAddress);
		#endif
	}

	void block_write(AddressType address, IMG_BYTE* const pData, AddressType uLength)
	{
		address -= uBaseAddress;
		memcpy(static_cast<IMG_BYTE*>(pMemory) + address, pData, static_cast<size_t>(uLength));
		#ifdef TRANSIF_LOGGING
		fprintf(Log_file,"Written 0x%x bytes of data at 0x%x \n",uLength,address+uBaseAddress);
		#endif
	}

	IMG_UINT32 read(AddressType address)
	{
		address -= uBaseAddress;
		//assert((address < uSize) && (((address & (sizeof(IMG_UINT32) -1)) == 0)));
		return reinterpret_cast<IMG_UINT32*>(pMemory)[address / sizeof(IMG_UINT32)];
		#ifdef TRANSIF_LOGGING
		fprintf(Log_file,"Read 4 bytes of data at 0x%x \n",address+uBaseAddress);
		#endif	
	}

	void block_read(AddressType address, IMG_BYTE* pData, AddressType uLength)
	{
		address -= uBaseAddress;
		memcpy(pData, static_cast<IMG_BYTE*>(pMemory) + address, static_cast<size_t>(uLength));
		#ifdef TRANSIF_LOGGING
		fprintf(Log_file,"Read 0x%x bytes of data at 0x%x \n",uLength,address+uBaseAddress);
		#endif	
	}

	AddressType uBaseAddress;
	AddressType uSize; // In Bytes

	IMG_BYTE *const pMemory;

private:
	const system_mem_allocation &operator=(const system_mem_allocation &);
	IMG_BOOL bMapped;
};


system_mem_alloc::system_mem_alloc(bool forceAlloc)
	: pLastAllocation(NULL)
	//, uLastKey(~0)
	, uTotalAllocatedMemory(0)
	, uMaxAddress(0)
	, uMaxAllocation(0)
	, m_forceAlloc(forceAlloc)

{
    OSA_CritSectCreate(&m_lastAllocLock);
    IMG_ASSERT(m_lastAllocLock != NULL);
}

system_mem_alloc::~system_mem_alloc()
{
	//SimOutput("Closing down memory system.\n");
	//SimOutput("Max Address used = 0x%08X.\n", uMaxAddress);
	//SimOutput("Max concurrent allocaton used = 0x%08X.\n", uMaxAllocation);
	OSA_CritSectDestroy(m_lastAllocLock);
}

void system_mem_alloc::write(IMG_UINT64 address, IMG_UINT32 uData)
{	
	Allocation* pAllocation = find_allocation(address, 4);
	pAllocation->write(address , uData);
}

void system_mem_alloc::block_write(IMG_UINT64 address, IMG_BYTE* const pData, IMG_UINT64 uLength)
{	
	Allocation* pAllocation = find_allocation(address, (IMG_UINT32)uLength);
	if(pAllocation==NULL)
	{
		fprintf(stdout, "Bad write to address 0x%" IMG_I64PR "X\n (invalid address)",address);
	}
	else{
		pAllocation->block_write(address , pData, uLength);
	}
}

IMG_UINT32 system_mem_alloc::read(IMG_UINT64 address)
{	
	Allocation* pAllocation = find_allocation(address, 4);
	return pAllocation->read(address);
}

void system_mem_alloc::block_read(IMG_UINT64 address, IMG_BYTE* pData, IMG_UINT64 uLength)
{
	Allocation* pAllocation = find_allocation(address, (IMG_UINT32)uLength);
	if(pAllocation==NULL)
	{
		fprintf(stdout,"Bad Memory Read\n");
		return ;
	}
	else{
		return pAllocation->block_read(address, pData, uLength);
	}
}

IMG_BOOL system_mem_alloc::map(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize, IMG_VOID* pvBuffer)
{
	//SimAssert(uSize, "Attempt to allocate zero bytes\n");
	std::map<IMG_UINT64, Allocation*>::iterator i;
	
	if (m_forceAlloc)
	{
	    IMG_UINT64 nMask;
	
	    /* Page align */
	    nMask = ~((IMG_UINT64)getpagesize() - 1);
	    uBaseAddress = uBaseAddress & nMask;
	    
	    /* Allocate at least a page */
	    if (uSize < (IMG_UINT32)getpagesize())
	        uSize = (IMG_UINT32)getpagesize();
    }

	//Check no intersection with allocations who's start address >= uBaseAddress
	i = memoryAllocationMap.lower_bound(uBaseAddress);
	if((i != memoryAllocationMap.end()) && (*i).second->intersects(uBaseAddress,uSize))
	{
	    printf("0x%016" IMG_I64PR "x 0x%08x 0x%016" IMG_I64PR "x\n", uBaseAddress, uSize, (*i).first);
		return IMG_FALSE;
	}
	
	//And no intersection with allocations whos's start address < uBaseAddress
	if((i != memoryAllocationMap.begin()) && (*(--i)).second->intersects(uBaseAddress, uSize))
	{
	    printf("2\n");
		return IMG_FALSE;
    }
    
	//Ok to allocate the memory
	memoryAllocationMap[uBaseAddress] = new Allocation(uBaseAddress, uSize, (IMG_BYTE*)pvBuffer);
	uTotalAllocatedMemory+= uSize;
#if VERBOSE_ALLOCS
	printf("Alloc %d at 0x%08x, Total system memory allocation = %d\n", uSize, uBaseAddress, uTotalAllocatedMemory);
#endif
	if (uTotalAllocatedMemory > uMaxAllocation) uMaxAllocation = uTotalAllocatedMemory;
	if (uBaseAddress + uSize - 1 > uMaxAddress) uMaxAddress = uBaseAddress + uSize - 1;

	return IMG_TRUE;
}



IMG_BOOL system_mem_alloc::alloc(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize)
{
	return map(uBaseAddress, uSize, IMG_NULL);
}

/////////Mem
void system_mem_alloc::free(IMG_UINT64 uBaseAddress)
{
	std::map<IMG_UINT64, Allocation*>::iterator allocation;
	allocation = memoryAllocationMap.find(uBaseAddress);

	if(allocation != memoryAllocationMap.end())
	{
		uTotalAllocatedMemory -= (*allocation).second->uSize;
#if VERBOSE_ALLOCS
		printf("Free %d at 0x%08x, Total system memory allocation = %d\n", (*allocation).second->uSize, uBaseAddress, uTotalAllocatedMemory);
#endif
		if(pLastAllocation == (*allocation).second)
			pLastAllocation = NULL;

		delete((*allocation).second);
		memoryAllocationMap.erase(allocation);
		return;
	}
}

system_mem_alloc::Allocation* system_mem_alloc::find_allocation(IMG_UINT64 uAddress, IMG_UINT32 uSize)
{
	OSA_CritSectLock(m_lastAllocLock);
	
	if (pLastAllocation == NULL || !pLastAllocation->contains(uAddress, uSize))
	{
		pLastAllocation = NULL;
		std::map<IMG_UINT64, Allocation*>::iterator i;
		
		//Get first allocation starting at >=uAddress
		i = memoryAllocationMap.lower_bound(uAddress);
		if ((i != memoryAllocationMap.end()) && (*i).second->contains(uAddress)) {
			pLastAllocation = (*i).second;
		}
		else if (i != memoryAllocationMap.begin())
		{
			//Go backwards in tree to find last allocation that starts <uAddress
			--i;
			if ((*i).second->contains(uAddress)) {
				pLastAllocation = (*i).second;
			}
		}
 	} // if (pLastAllocation == NULL || !pLastAllocation->contains(uAddress))
	
	if (pLastAllocation == NULL)
	{
		if (m_forceAlloc)
		{
			if (alloc(uAddress, uSize))
			{
                OSA_CritSectUnlock(m_lastAllocLock);
				pLastAllocation = find_allocation(uAddress, uSize);
				OSA_CritSectLock(m_lastAllocLock);
			}
		}
		else
		{
			printf("Address 0x%016" IMG_I64PR "X is not in any allocated buffer\n", uAddress);

			for(std::map<IMG_UINT64, Allocation*>::iterator i = memoryAllocationMap.begin();
				i != memoryAllocationMap.end();
				i++)
			{
				printf("\t0x%016" IMG_I64PR "X to 0x%016" IMG_I64PR "X   size:  0x%016" IMG_I64PR "X  (decimal %" IMG_I64PR "d bytes)\n", 
					(*i).second->uBaseAddress, 
					(*i).second->uBaseAddress+(*i).second->uSize -1, 
					(*i).second->uSize, (*i).second->uSize);
			}
			exit(-1);
		}
	}
	Allocation *returnedAlloc = pLastAllocation;
	if(pLastAllocation->uSize < uSize)
	{
		printf("ERROR: Tried to use memory block of size 0x%" IMG_I64PR "X for read / write of block size 0x%X" IMG_I64PR "\n", pLastAllocation->uSize, uSize);
		returnedAlloc = IMG_NULL;
	}
	OSA_CritSectUnlock(m_lastAllocLock);
	return returnedAlloc;
}

IMG_PUINT8 system_mem_alloc::getMemPtr(IMG_UINT64 uBaseAddress)
{
	std::map<IMG_UINT64, Allocation*>::iterator iter;
	Allocation *allocation;

	iter = memoryAllocationMap.find(uBaseAddress);
	allocation = (Allocation *)((*iter).second);

	return (allocation != NULL) ? allocation->pMemory : NULL;
}

