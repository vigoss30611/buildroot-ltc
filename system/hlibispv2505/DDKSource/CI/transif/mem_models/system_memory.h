#ifndef __SYSTEM_MEMORY_H__
#define __SYSTEM_MEMORY_H__

#include "img_types.h"

typedef enum
{
	IMG_MEMORY_MODEL_DYNAMIC,
	IMG_MEMORY_MODEL_PREALLOC,
	IMG_MEMORY_MODEL_SIM,
	
	// Placeholder
	MAX_MEMORY_MODELS
}SYSMEM_eMemModel;


#ifdef __cplusplus

class SimpleDevifMemoryModel; //#include "device_interface_transif.h"
#include "sim_wrapper_api.h" // toIMGIP_if cannot be forward declared because it is a typedef

typedef struct _SYSMEM_sPrealloc
{
	IMG_UINT64 ui64BaseAddr;		//!< Base address of preallocated memory
	IMG_UINT64 ui64Size;			//!< Size of preallocated memory
}SYSMEM_sPrealloc;

typedef struct _SYSMEM_sSim
{
	toIMGIP_if*	pSim;				//!< Pointer to Sim Wrapper Class
	IMG_UINT64	ui64BaseAddr;		//!< Base address of sim memory
	IMG_UINT64  ui64Size;			//!< size of sim memory
	IMG_BOOL	bTimed;				//!< Indicates if sim is timed / untimed
}SYSMEM_sSim;


SimpleDevifMemoryModel* getMemoryHandle(SYSMEM_eMemModel eMemModel, void *pParam);

#endif // __cplusplus

#endif //__SYSTEM_MEMORY_H__
