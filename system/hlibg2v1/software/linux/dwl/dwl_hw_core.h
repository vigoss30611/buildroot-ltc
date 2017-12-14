/*  Copyright 2012 Google Inc. All Rights Reserved. */
/*  Author: vmr@google.com (Ville-Mikko Rautio) */

#ifndef __DWL_HW_CORE_H__
#define __DWL_HW_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "basetype.h"

#include <pthread.h>
#include <semaphore.h>

typedef const void* Core;

Core HwCoreInit(void);
void HwCoreRelease(Core instance);

int HwCoreTryLock(Core inst);
void HwCoreUnlock(Core inst);

u32* HwCoreGetBaseAddress(Core instance);
void HwCoreSetid(Core instance, int id);
int HwCoreGetid(Core instance);

/* Starts the execution on a core. */
void HwCoreDecEnable(Core instance);
void HwCorePpEnable(Core instance, int start);

/* Tries to interrupt the execution on a core as soon as possible. */
void HwCoreDisable(Core instance);

int HwCoreWaitDecRdy(Core instance);
int HwCoreWaitPpRdy(Core instance);

int HwCorePostPpRdy(Core instance);
int HwCorePostDecRdy(Core instance);

int HwCoreIsDecRdy(Core instance);
int HwCoreIsPpRdy(Core instance);

void HwCoreSetHwRdySem(Core instance, sem_t* rdy);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __DWL_HW_CORE_H__ */
