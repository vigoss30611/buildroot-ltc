#include <map>
#include <iostream>
#include <devif_api.h>

#ifdef WIN32

extern "C" {
//
// Keep track of virtual address of all known pages, 
// using the physical address of the page as the key. 
//
typedef struct
{
	IMG_PVOID	pvHostAddr;
	IMG_UINT32	uiSize;
} VIRT_ALLOC;

std::map<IMG_UINT64, VIRT_ALLOC> AllocMappings;

// add an entry to the physical->virtual map
// replacing any existing entry.
IMG_BOOL DEVIF_AddPhysToVirtEntry(IMG_UINT64 phys, IMG_UINT32 uiSize, IMG_VOID * virt)
{
	VIRT_ALLOC sVirtAlloc;
	sVirtAlloc.pvHostAddr = virt;
	sVirtAlloc.uiSize = uiSize;

	AllocMappings.insert(std::map<IMG_UINT64, VIRT_ALLOC>::value_type(phys, sVirtAlloc));

	return IMG_TRUE;
}

inline bool FindPhys(std::map<IMG_UINT64, VIRT_ALLOC>::const_iterator &itr, IMG_UINT64 phys)
{
	if(AllocMappings.size() == 0)
	{
		return false;
	}

	itr = AllocMappings.upper_bound(phys);
	itr--;

	if(itr->first <= phys && itr->first + itr->second.uiSize > phys) //check if phys address matches our mapping  
	{
		return true;
	}

	return false;
}

//
// lookup the virtual address, using the physical address as the key.
// return NULL if no such physical address. 
//
IMG_VOID *DEVIF_FindPhysToVirtEntry(IMG_UINT64 phys)
{
	std::map<IMG_UINT64, VIRT_ALLOC>::const_iterator itr;

	if(!FindPhys(itr, phys))
	{
		return NULL;
	}

	return (IMG_VOID *)(((char*)itr->second.pvHostAddr) + ( phys - itr->first ));
}

IMG_VOID DEVIF_RemovePhysToVirtEntry(IMG_UINT64 phys)
{
	std::map<IMG_UINT64, VIRT_ALLOC>::const_iterator itr;
	if(!FindPhys(itr, phys))
	{
		return;
	}

	AllocMappings.erase(itr->first);
}

}

#else

//
// Keep track of virtual address of all known pages, 
// using the physical address of the page as the key. 
//
std::map<IMG_UINT64, IMG_PVOID>phys2virtmap;

// add an entry to the physical->virtual map
// replacing any existing entry.
IMG_BOOL
DEVIF_AddPhysToVirtEntry(IMG_UINT64 phys, IMG_VOID * virt)
{
	phys2virtmap[phys] = virt;
	return IMG_TRUE;
}


//
// lookup the virtual address, using the physical address as the key.
// return NULL if no such physical address. 
//
IMG_VOID * 
DEVIF_FindPhysToVirtEntry(IMG_UINT64 phys)
{
	std::map<IMG_UINT64, IMG_PVOID>::iterator iter = phys2virtmap.find(phys);
	if(iter == phys2virtmap.end())
		return NULL;
	else
		return iter->second;
}

IMG_VOID
DEVIF_RemovePhysToVirtEntry(IMG_UINT64 phys)
{
	phys2virtmap.erase(phys);
}

#endif


