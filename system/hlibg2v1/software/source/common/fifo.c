/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "fifo.h"

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

/* Container for instance. */
struct Fifo {
  sem_t cs_semaphore;    /* Semaphore for critical section. */
  sem_t read_semaphore;  /* Semaphore for readers. */
  sem_t write_semaphore; /* Semaphore for writers. */
  u32 num_of_slots;
  u32 num_of_objects;
  u32 tail_index;
  FifoObject* nodes;
};

enum FifoRet FifoInit(u32 num_of_slots, FifoInst* instance) {
  struct Fifo* inst = calloc(1, sizeof(struct Fifo));
  if (inst == NULL) return FIFO_ERROR_MEMALLOC;
  inst->num_of_slots = num_of_slots;
  /* Allocate memory for the objects. */
  inst->nodes = calloc(num_of_slots, sizeof(FifoObject));
  if (inst->nodes == NULL) {
    free(inst);
    return FIFO_ERROR_MEMALLOC;
  }
  /* Initialize binary critical section semaphore. */
  sem_init(&inst->cs_semaphore, 0, 1);
  /* Then initialize the read and write semaphores. */
  sem_init(&inst->read_semaphore, 0, 0);
  sem_init(&inst->write_semaphore, 0, num_of_slots);
  *instance = inst;
  return FIFO_OK;
}

enum FifoRet FifoPush(FifoInst inst, FifoObject object, enum FifoException e) {
  struct Fifo* instance = (struct Fifo*)inst;
  int value;

  sem_getvalue(&instance->read_semaphore, &value);
  if ((e == FIFO_EXCEPTION_ENABLE) && (value == instance->num_of_slots) &&
      (instance->num_of_objects == instance->num_of_slots)) {
    return FIFO_FULL;
  }

  sem_wait(&instance->write_semaphore);
  sem_wait(&instance->cs_semaphore);
  instance->nodes[(instance->tail_index + instance->num_of_objects) %
                  instance->num_of_slots] = object;
  instance->num_of_objects++;
  sem_post(&instance->cs_semaphore);
  sem_post(&instance->read_semaphore);
  return FIFO_OK;
}

enum FifoRet FifoPop(FifoInst inst, FifoObject* object, enum FifoException e) {
  struct Fifo* instance = (struct Fifo*)inst;
  int value;

  sem_getvalue(&instance->write_semaphore, &value);
  if ((e == FIFO_EXCEPTION_ENABLE) && (value == instance->num_of_slots) &&
      (instance->num_of_objects == 0)) {
    return FIFO_EMPTY;
  }

  sem_wait(&instance->read_semaphore);
  sem_wait(&instance->cs_semaphore);
  *object = instance->nodes[instance->tail_index % instance->num_of_slots];
  instance->tail_index++;
  instance->num_of_objects--;
  sem_post(&instance->cs_semaphore);
  sem_post(&instance->write_semaphore);
  return FIFO_OK;
}

u32 FifoCount(FifoInst inst) {
  u32 count;
  struct Fifo* instance = (struct Fifo*)inst;
  sem_wait(&instance->cs_semaphore);
  count = instance->num_of_objects;
  sem_post(&instance->cs_semaphore);
  return count;
}

void FifoRelease(FifoInst inst) {
  struct Fifo* instance = (struct Fifo*)inst;
  assert(instance->num_of_objects == 0);
  sem_wait(&instance->cs_semaphore);
  sem_destroy(&instance->cs_semaphore);
  sem_destroy(&instance->read_semaphore);
  sem_destroy(&instance->write_semaphore);
  free(instance->nodes);
  free(instance);
}
