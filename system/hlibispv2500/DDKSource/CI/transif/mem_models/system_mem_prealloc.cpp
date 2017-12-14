
#include "img_defs.h"
#include "system_mem_prealloc.h"

system_mem_prealloc::system_mem_prealloc(IMG_UINT64 uBaseAddress, IMG_UINT32 uSize) 
	: uiBaseAddress(uBaseAddress)
	, uiSize(uSize)
{
	pMem = new IMG_CHAR[uSize];
}

system_mem_prealloc::~system_mem_prealloc()
{
	if(pMem)
		delete pMem;
}

void system_mem_prealloc::write(IMG_UINT64 address, IMG_UINT32 uData)
{
	*((IMG_UINT32*)(&pMem[address - uiBaseAddress])) = uData;
}

void system_mem_prealloc::block_write(IMG_UINT64 address, IMG_BYTE* const pData, IMG_UINT64 uLength)
{
	memcpy(&pMem[address - uiBaseAddress], pData, (IMG_UINTPTR)uLength);
}

IMG_UINT32 system_mem_prealloc::read(IMG_UINT64 address)
{
	return *((IMG_UINT32*)(&pMem[address - uiBaseAddress]));
}
	
void system_mem_prealloc::block_read(IMG_UINT64 address, IMG_BYTE* pData, IMG_UINT64 uLength)
{
	memcpy(pData, &pMem[address - uiBaseAddress], (IMG_UINTPTR)uLength);
}
