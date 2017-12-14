/*  Copyright 2012 Google Inc. All Rights Reserved. */
/*  Author: vmr@google.com (Ville-Mikko Rautio) */

#ifndef __DWL_HW_CORE_ARRAY_H__
#define __DWL_HW_CORE_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "basetype.h"
#include "dwl_hw_core.h"

typedef const void* HwCoreArray;

/* Initializes core array and individual hardware core abstractions. */
HwCoreArray InitializeCoreArray();
/* Releases the core array and hardware core abstractions. */
void ReleaseCoreArray(HwCoreArray inst);

int StopCoreArray(HwCoreArray inst);

/* Get usage rights for single core. Blocks until there is available core. */
Core BorrowHwCore(HwCoreArray inst);
/* Returns previously borrowed |hw_core|. */
void ReturnHwCore(HwCoreArray inst, Core hw_core);

u32 GetCoreCount();

/* Get a reference to the nth core */
Core GetCoreById(HwCoreArray inst, int nth);

/* wait for a core, any core, to Finish processing */
int WaitAnyCoreRdy(HwCoreArray inst);

#ifdef __cplusplus
}
#endif

#endif /* __DWL_HW_CORE_ARRAY_H__ */
