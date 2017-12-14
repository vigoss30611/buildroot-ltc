/* Copyright 2013 Google Inc. All Rights Reserved. */

/* C89 compatible thread-safe, synchronized, generic FIFO queue implementation.
 * */

#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "basetype.h"

/* FIFO_DATATYPE must be defined to hold specific type of objects. If it is not
 * defined, we need to report an error. */
#ifndef FIFO_DATATYPE
#error "You must define FIFO_DATATYPE to use this module."
#endif /* FIFO_DATATYPE */

typedef FIFO_DATATYPE FifoObject;

/* Possible return values. */
enum FifoRet {
  FIFO_OK,             /* Operation was successful. */
  FIFO_ERROR_MEMALLOC, /* Failed due to memory allocation error. */
  FIFO_EMPTY,
  FIFO_FULL,
  FIFO_NOK
};

enum FifoException {
  FIFO_EXCEPTION_DISABLE,
  FIFO_EXCEPTION_ENABLE
};

typedef void* FifoInst;

/* FifoInit initializes the queue.
 * |num_of_slots| defines how many slots to reserve at maximum.
 * |instance| is output parameter holding the instance. */
enum FifoRet FifoInit(u32 num_of_slots, FifoInst* instance);

/* FifoPush pushes an object to the back of the queue. Ownership of the
 * contained object will be moved from the caller to the queue. Returns OK
 * if the object is successfully pushed into fifo.
 *
 * |inst| is the instance push to.
 * |object| holds the pointer to the object to push into queue.
 * |exception_enable| enable FIFO_FULL return value */
enum FifoRet FifoPush(FifoInst inst, FifoObject object,
                      enum FifoException exception_enable);

/* FifoPop returns object from the front of the queue. Ownership of the popped
 * object will be moved from the queue to the caller. Returns OK if the object
 * is successfully popped from the fifo.
 *
 * |inst| is the instance to pop from.
 * |object| holds the pointer to the object popped from the queue.
 * |exception_enable| enable FIFO_EMPTY return value */
enum FifoRet FifoPop(FifoInst inst, FifoObject* object,
                     enum FifoException exception_enable);

/* Ask how many objects there are in the fifo. */
u32 FifoCount(FifoInst inst);

/* FifoRelease releases and deallocated queue. User needs to make sure the
 * queue is empty and no threads are waiting in FifoPush or FifoPop.
 * |inst| is the instance to release. */
void FifoRelease(FifoInst inst);

#endif /* __FIFO_H__ */
