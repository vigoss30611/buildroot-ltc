/*  Copyright 2012 Google Inc. All Rights Reserved. */
/*  Author: vmr@google.com (Ville-Mikko Rautio) */

#include "asic.h"
#include "dwl.h"
#include "dwl_hw_core_array.h"

#include <assert.h>
#include <semaphore.h>
#include <stdlib.h>

#ifdef CORES
#if CORES > MAX_ASIC_CORES
#error Too many cores! Check max number of cores in <decapicommon.h>
#else
#define HW_CORE_COUNT CORES
#endif
#else
#define HW_CORE_COUNT 1
#endif

struct HwCoreContainer {
  Core core;
};

struct HwCoreArrayInstance {
  u32 num_of_cores;
  struct HwCoreContainer* cores;
  sem_t core_lock;
  sem_t core_rdy;
};

HwCoreArray InitializeCoreArray() {
  u32 i;
  struct HwCoreArrayInstance* array =
      malloc(sizeof(struct HwCoreArrayInstance));
  array->num_of_cores = GetCoreCount();
  sem_init(&array->core_lock, 0, array->num_of_cores);

  sem_init(&array->core_rdy, 0, 0);

  array->cores = calloc(array->num_of_cores, sizeof(struct HwCoreContainer));
  assert(array->cores);
  for (i = 0; i < array->num_of_cores; i++) {
    array->cores[i].core = HwCoreInit();
    assert(array->cores[i].core);

    HwCoreSetid(array->cores[i].core, i);
    HwCoreSetHwRdySem(array->cores[i].core, &array->core_rdy);
  }
  return array;
}

void ReleaseCoreArray(HwCoreArray inst) {
  u32 i;

  struct HwCoreArrayInstance* array = (struct HwCoreArrayInstance*)inst;

  /* TODO(vmr): Wait for all cores to Finish. */
  for (i = 0; i < array->num_of_cores; i++) {
    HwCoreRelease(array->cores[i].core);
  }

  free(array->cores);
  sem_destroy(&array->core_lock);
  sem_destroy(&array->core_rdy);
  free(array);
}

Core BorrowHwCore(HwCoreArray inst) {
  u32 i = 0;
  struct HwCoreArrayInstance* array = (struct HwCoreArrayInstance*)inst;

  sem_wait(&array->core_lock);

  while (!HwCoreTryLock(array->cores[i].core)) {
    i++;
  }

  return array->cores[i].core;
}

void ReturnHwCore(HwCoreArray inst, Core core) {
  struct HwCoreArrayInstance* array = (struct HwCoreArrayInstance*)inst;

  HwCoreUnlock(core);

  sem_post(&array->core_lock);
}

u32 GetCoreCount() {
  /* TODO(vmr): implement dynamic mechanism for calculating. */
  return HW_CORE_COUNT;
}

Core GetCoreById(HwCoreArray inst, int nth) {
  struct HwCoreArrayInstance* array = (struct HwCoreArrayInstance*)inst;

  assert(nth < (int)GetCoreCount());

  return array->cores[nth].core;
}

int WaitAnyCoreRdy(HwCoreArray inst) {
  struct HwCoreArrayInstance* array = (struct HwCoreArrayInstance*)inst;

  return sem_wait(&array->core_rdy);
}

int StopCoreArray(HwCoreArray inst) {
  struct HwCoreArrayInstance* array = (struct HwCoreArrayInstance*)inst;
  return sem_post(&array->core_rdy);
}
