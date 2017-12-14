/*  Copyright 2012 Google Inc. All Rights Reserved. */
/*  Author: vmr@google.com (Ville-Mikko Rautio) */

#include "vp9hwd_buffer_queue.h"
#include "vp9hwd_decoder.h"
#include "vp9hwd_container.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "fifo.h"
//#define BUFFER_QUEUE_PRINT_STATUS
#ifdef BUFFER_QUEUE_PRINT_STATUS
#define PRINT_COUNTS(x) PrintCounts(x)
#else
#define PRINT_COUNTS(x)
#endif /* BUFFER_QUEUE_PRINT_STATUS */

/* Data structure to hold this picture buffer queue instance data. */
struct BQueue {
  pthread_mutex_t cs; /* Critical section to protect data. */
  i32 n_buffers;      /* Number of buffers contained in total. */
  i32 n_references[VP9DEC_MAX_PIC_BUFFERS]; /* Reference counts on buffers.
                                              Index is buffer#.  */
  i32 ref_status[VP9_REF_LIST_SIZE]; /* Reference status of the decoder. Each
                                       element contains index to buffer to an
                                       active reference. */
  FifoInst empty_fifo; /* Queue holding empty, unreferred buffer indices. */
};

static void IncreaseRefCount(struct BQueue* q, i32 i);
static void DecreaseRefCount(struct BQueue* q, i32 i);
#ifdef BUFFER_QUEUE_PRINT_STATUS
static inline void PrintCounts(struct BQueue* q);
#endif

u32 Vp9BufferQueueCountReferencedBuffers(BufferQueue queue) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  u32 i,j;
  u32 is_referenced, ref_count = 0;
  struct BQueue* q = (struct BQueue*)queue;
  pthread_mutex_lock(&q->cs);


  for (i = 0; i < VP9DEC_MAX_PIC_BUFFERS; i++) {
    is_referenced = 0;

    for (j = 0; j < VP9_REF_LIST_SIZE; j++) {
      if (q->ref_status[j] == i)
        is_referenced++;
    }
    if(is_referenced) ref_count++;
  }
  pthread_mutex_unlock(&q->cs);

#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("# %i\n", ref_count);
#endif /* BUFFER_QUEUE_PRINT_STATUS */

  return ref_count;
}

void Vp9BufferQueueResetReferences(BufferQueue queue) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  i32 i;
  struct BQueue* q = (struct BQueue*)queue;
  pthread_mutex_lock(&q->cs);
  for (i = 0; i < sizeof(q->ref_status) / sizeof(q->ref_status[0]); i++) {
    q->ref_status[i] = REFERENCE_NOT_SET;
  }
  PRINT_COUNTS(q);
  pthread_mutex_unlock(&q->cs);
}

BufferQueue Vp9BufferQueueInitialize(i32 n_buffers) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("(n_buffers=%i)", n_buffers);
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  assert(n_buffers >= 0);
  i32 i;
  FifoObject j;
  enum FifoRet ret;
  struct BQueue* q = (struct BQueue*)calloc(1, sizeof(struct BQueue));
  if (q == NULL) {
    return NULL;
  }

  if (FifoInit(VP9DEC_MAX_PIC_BUFFERS, &q->empty_fifo) != FIFO_OK ||
      pthread_mutex_init(&q->cs, NULL)) {
    Vp9BufferQueueRelease(q);
    return NULL;
  }
  /* Add picture buffers among empty picture buffers. */
  for (i = 0; i < n_buffers; i++) {
    j = (FifoObject)(addr_t)i;
    ret = FifoPush(q->empty_fifo, j, FIFO_EXCEPTION_ENABLE);
    if (ret != FIFO_OK) {
      Vp9BufferQueueRelease(q);
      return NULL;
    }
    q->n_buffers++;
  }
  Vp9BufferQueueResetReferences(q);
  return q;
}

void Vp9BufferQueueRelease(BufferQueue queue) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("()");
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  assert(queue);
  struct BQueue* q = (struct BQueue*)queue;
  if (q->empty_fifo) {/* Empty the fifo before releasing. */
    i32 i;
    FifoObject j;
    enum FifoRet ret;
    for (i = 0; i < q->n_buffers; i++) {
      ret = FifoPop(q->empty_fifo, &j, FIFO_EXCEPTION_ENABLE);
      assert(ret == FIFO_OK || ret == FIFO_EMPTY);
      (void)ret;
    }
    FifoRelease(q->empty_fifo);
  }
  pthread_mutex_destroy(&q->cs);
  free(q);
}

void Vp9BufferQueueUpdateRef(BufferQueue queue, u32 ref_flags, i32 buffer) {
  u32 i = 0;
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("(ref_flags=0x%X, buffer=%i)", ref_flags, buffer);
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  assert(queue);
  struct BQueue* q = (struct BQueue*)queue;
  assert((buffer >= 0 || buffer == REFERENCE_NOT_SET) && buffer < q->n_buffers);
  pthread_mutex_lock(&q->cs);

  for (i = 0; i < VP9_REF_LIST_SIZE; i++) {
    if ((ref_flags & (1 << i)) && buffer != q->ref_status[i]) {
      if (q->ref_status[i] != REFERENCE_NOT_SET) {
        DecreaseRefCount(q, q->ref_status[i]);
      }
      q->ref_status[i] = buffer;
      if (buffer != REFERENCE_NOT_SET)
        IncreaseRefCount(q, buffer);
    }
  }

  PRINT_COUNTS(q);
  pthread_mutex_unlock(&q->cs);
}

i32 Vp9BufferQueueGetRef(BufferQueue queue, u32 index) {
  struct BQueue* q = (struct BQueue*)queue;
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("(%u)", index);
  printf(" # %d\n", q->ref_status[index]);
  PRINT_COUNTS(q);
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  return q->ref_status[index];
}

void Vp9BufferQueueAddRef(BufferQueue queue, i32 buffer) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("(buffer=%i)", buffer);
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  struct BQueue* q = (struct BQueue*)queue;
  assert(buffer >= 0 && buffer < q->n_buffers);
  pthread_mutex_lock(&q->cs);
  IncreaseRefCount(q, buffer);
  pthread_mutex_unlock(&q->cs);
}

void Vp9BufferQueueRemoveRef(BufferQueue queue, i32 buffer) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("(buffer=%i)", buffer);
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  struct BQueue* q = (struct BQueue*)queue;
  assert(buffer >= 0 && buffer < q->n_buffers);
  pthread_mutex_lock(&q->cs);
  DecreaseRefCount(q, buffer);
  pthread_mutex_unlock(&q->cs);
}

i32 Vp9BufferQueueGetBuffer(BufferQueue queue, u32 limit) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("()");
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  i32 i;
  FifoObject j;
  enum FifoRet ret;
  struct BQueue* q = (struct BQueue*)queue;
  assert(q->empty_fifo);
  ret = FifoPop(q->empty_fifo, &j, FIFO_EXCEPTION_ENABLE);
  if (ret == FIFO_EMPTY) {
    if (q->n_buffers < limit) {/* return and allocate new buffer */
      return -1;
    } else {/* wait for free one */
      ret = FifoPop(q->empty_fifo, &j, FIFO_EXCEPTION_DISABLE);
    }
  }
  assert(ret == FIFO_OK);
  i = *(addr_t *)&j;
  pthread_mutex_lock(&q->cs);
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("# %i\n", i);
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  IncreaseRefCount(q, i);
  pthread_mutex_unlock(&q->cs);
  return i;
}

void Vp9BufferQueueWaitPending(BufferQueue queue) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("()");
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  assert(queue);
  struct BQueue* q = (struct BQueue*)queue;
  /* TODO(mheikkinen): cherry-pick non-busyloop implementation from g1. */
  while (FifoCount(q->empty_fifo) != (u32)q->n_buffers) pthread_yield();
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("#\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
}

void Vp9BufferQueueAddBuffer(BufferQueue queue) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("()");
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  assert(queue);
  enum FifoRet ret;
  struct BQueue* q = (struct BQueue*)queue;
  FifoObject j;
  pthread_mutex_lock(&q->cs);
  /* Add one picture buffer among empty picture buffers. */
  j = (FifoObject)(addr_t)q->n_buffers;
  ret = FifoPush(q->empty_fifo, j, FIFO_EXCEPTION_ENABLE);
  assert(ret == FIFO_OK);
  (void)ret;
  q->n_buffers++;
  pthread_mutex_unlock(&q->cs);

#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("#\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
}

static void IncreaseRefCount(struct BQueue* q, i32 i) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("(buffer=%i)", i);
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  q->n_references[i]++;
  assert(q->n_references[i] >= 0); /* No negative references. */
  PRINT_COUNTS(q);
}

static void DecreaseRefCount(struct BQueue* q, i32 i) {
#ifdef BUFFER_QUEUE_PRINT_STATUS
  printf(__FUNCTION__);
  printf("(buffer=%i)", i);
  printf("\n");
#endif /* BUFFER_QUEUE_PRINT_STATUS */
  enum FifoRet ret;
  FifoObject j;
  q->n_references[i]--;
  assert(q->n_references[i] >= 0); /* No negative references. */
  PRINT_COUNTS(q);
  if (q->n_references[i] == 0) {
/* Once picture buffer is no longer referred to, it can be put to
   the empty fifo. */
#ifdef BUFFER_QUEUE_PRINT_STATUS
    printf("Buffer #%i put to empty pool\n", i);
#endif /* BUFFER_QUEUE_PRINT_STATUS */
    j = (FifoObject)(addr_t)i;
    ret = FifoPush(q->empty_fifo, j, FIFO_EXCEPTION_ENABLE);
    assert(ret == FIFO_OK);
    (void)ret;
  }
}

#ifdef BUFFER_QUEUE_PRINT_STATUS
static inline void PrintCounts(struct BQueue* q) {
  i32 i = 0;
  for (i = 0; i < q->n_buffers; i++) printf("%u", q->n_references[i]);
  printf("|");
  for (i = 0; i < sizeof(q->ref_status) / sizeof(q->ref_status[0]); i++)
    printf("[%u]:%i|", i, q->ref_status[i]);
  printf("\n");
}
#endif /* BUFFER_QUEUE_PRINT_STATUS */
