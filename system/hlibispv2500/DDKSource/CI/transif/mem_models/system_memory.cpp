
#include "system_memory.h"
#include "system_mem_prealloc.h"
#include "system_mem_alloc.h"
#include "system_mem_sim.h"


SimpleDevifMemoryModel* getMemoryHandle(SYSMEM_eMemModel eMemModel, void *pParam)
{
	switch (eMemModel)
	{
	case IMG_MEMORY_MODEL_DYNAMIC:
		if(pParam)
			return new system_mem_alloc(*(bool *)pParam);
		else
			return new system_mem_alloc();
	case IMG_MEMORY_MODEL_PREALLOC:
		{
			if (pParam == IMG_NULL)
				return IMG_NULL;
			SYSMEM_sPrealloc* psMemInfo = (SYSMEM_sPrealloc*)pParam;
			return new system_mem_prealloc((IMG_UINT32)psMemInfo->ui64BaseAddr, (IMG_UINT32)psMemInfo->ui64Size);
		}
	case IMG_MEMORY_MODEL_SIM:
		{
			if (pParam == IMG_NULL)
				return IMG_NULL;
			SYSMEM_sSim* psMemInfo = (SYSMEM_sSim*)pParam;
			bool timed = psMemInfo->bTimed ? true : false;
			return new system_mem_sim(psMemInfo->pSim, timed, psMemInfo->ui64BaseAddr, psMemInfo->ui64Size);
		}
	default:
		return IMG_NULL;
	}
}
