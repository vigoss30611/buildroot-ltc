/*  Copyright 2012 Google Inc. All Rights Reserved. */
/*  Author: vmr@google.com (Ville-Mikko Rautio) */

#ifndef VP9HWD_PICTURE_BUFFER_QUEUE_H_
#define VP9HWD_PICTURE_BUFFER_QUEUE_H_

#include "basetype.h"
#include "dwl.h"

/* BufferQueue is picture queue indexing module, which manages the
 * buffer references on client's behalf. Module will maintain an index of
 * available and used buffers. When free  buffer is not available module
 * will block the thread asking for a buffer index. */

typedef void* BufferQueue; /* Opaque instance variable. */

/* Special value for index to tell it is unset. */
#define REFERENCE_NOT_SET 0xFFFFFFFF

/* Functions to initialize and release the BufferQueue. */
BufferQueue Vp9BufferQueueInitialize(i32 n_buffers);
void Vp9BufferQueueRelease(BufferQueue queue);

i32 Vp9BufferQueueGetRef(BufferQueue queue, u32 index);

/* Functions to manage the reference picture state. These are to be called
 * only from the decoding thread to get and manipulate the current decoder
 * reference buffer status. When a reference is pointing to a specific buffer
 * BufferQueue will automatically increment the reference counter to the given
 * buffer and decrement the reference counter to the previous reference buffer.
 */
void Vp9BufferQueueUpdateRef(BufferQueue queue, u32 ref_flags, i32 buffer);

/* Functions to manage references to the picture buffers. Caller is responsible
 * for calling AddRef when somebody will be using the given buffer and
 * RemoveRef each time somebody stops using a given buffer. When reference count
 * reaches 0, buffer is automatically added to the pool of free buffers. */
void Vp9BufferQueueAddRef(BufferQueue queue, i32 buffer);
void Vp9BufferQueueRemoveRef(BufferQueue queue, i32 buffer);

/* Function to get free buffers from the queue. Returns negative value if the
   free buffer was not found and the buffer count is below the limit. Otherwise
   blocks until the requested buffer is available.  Automatic +1 ref count. */
i32 Vp9BufferQueueGetBuffer(BufferQueue queue, u32 limit);

/* Function to wait until all buffers are in available status. */
void Vp9BufferQueueWaitPending(BufferQueue queue);

void Vp9BufferQueueAddBuffer(BufferQueue queue);

void Vp9BufferQueueResetReferences(BufferQueue queue);

u32 Vp9BufferQueueCountReferencedBuffers(BufferQueue queue);

#endif /* VP9HWD_PICTURE_BUFFER_QUEUE_H_ */
