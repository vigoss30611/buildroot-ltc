/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "hevcdecapi.h"
#include "hevc_fb_mngr.h"
#include "hevc_dpb.h"
#include "hevc_storage.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#define FB_UNALLOCATED 0x00U
#define FB_FREE 0x01U
#define FB_ALLOCATED 0x02U
#define FB_OUTPUT 0x04U
#define FB_TEMP_OUTPUT 0x08U

#define FB_HW_ONGOING 0x30U

#ifdef DPB_LOCK_TRACE
#define DPB_TRACE(fmt, ...) \
  fprintf(stderr, "%s(): " fmt, __func__, ##__VA_ARGS__)
#else
#define DPB_TRACE(...) \
  do {                 \
  } while (0)
#endif

u32 InitList(struct FrameBufferList *fb_list) {
  (void)DWLmemset(fb_list, 0, sizeof(*fb_list));

  sem_init(&fb_list->out_count_sem, 0, 0);
  pthread_mutex_init(&fb_list->out_count_mutex, NULL);
  /* CV to be signaled when output  queue is empty */
  pthread_cond_init(&fb_list->out_empty_cv, NULL);

  pthread_mutex_init(&fb_list->ref_count_mutex, NULL);
  /* CV to be signaled when a buffer is not referenced anymore */
  pthread_cond_init(&fb_list->ref_count_cv, NULL);

  /* this CV is used to signal the HW has finished processing a picture
   * that is needed for output ( FB_OUTPUT | FB_HW_ONGOING )
   */
  pthread_cond_init(&fb_list->hw_rdy_cv, NULL);

  fb_list->b_initialized = 1;

  return 0;
}

void ReleaseList(struct FrameBufferList *fb_list) {
  int i;
  if (!fb_list->b_initialized) return;

  for (i = 0; i < MAX_FRAME_BUFFER_NUMBER; i++) {
    /* we shall clean our stuff graciously */
    assert(fb_list->fb_stat[i].n_ref_count == 0);
  }

  assert(fb_list->free_buffers == 0);

  fb_list->b_initialized = 0;

  pthread_mutex_destroy(&fb_list->ref_count_mutex);
  pthread_cond_destroy(&fb_list->ref_count_cv);

  pthread_mutex_destroy(&fb_list->out_count_mutex);
  pthread_cond_destroy(&fb_list->out_empty_cv);
  pthread_cond_destroy(&fb_list->hw_rdy_cv);

  sem_destroy(&fb_list->out_count_sem);
}

u32 AllocateIdUsed(struct FrameBufferList *fb_list, const void *data) {
  u32 id = 0;

  /* find first unallocated ID */
  do {
    if (fb_list->fb_stat[id].b_used == FB_UNALLOCATED) break;
    id++;
  } while (id < MAX_FRAME_BUFFER_NUMBER);

  if (id >= MAX_FRAME_BUFFER_NUMBER) return FB_NOT_VALID_ID;

  fb_list->fb_stat[id].b_used = FB_ALLOCATED;
  fb_list->fb_stat[id].n_ref_count = 0;
  fb_list->fb_stat[id].data = data;

  return id;
}

u32 AllocateIdFree(struct FrameBufferList *fb_list, const void *data) {
  u32 id = 0;

  /* find first unallocated ID */
  do {
    if (fb_list->fb_stat[id].b_used == FB_UNALLOCATED) break;
    id++;
  } while (id < MAX_FRAME_BUFFER_NUMBER);

  if (id >= MAX_FRAME_BUFFER_NUMBER) return FB_NOT_VALID_ID;

  fb_list->free_buffers++;

  fb_list->fb_stat[id].b_used = FB_FREE;
  fb_list->fb_stat[id].n_ref_count = 0;
  fb_list->fb_stat[id].data = data;
  return id;
}

void ReleaseId(struct FrameBufferList *fb_list, u32 id) {
  assert(id < MAX_FRAME_BUFFER_NUMBER);

  /* it is "bad" to release referenced or unallocated buffers */
  assert(fb_list->fb_stat[id].n_ref_count == 0);
  assert(fb_list->fb_stat[id].b_used != FB_UNALLOCATED);

  if (id >= MAX_FRAME_BUFFER_NUMBER) return;

  if (fb_list->fb_stat[id].b_used == FB_FREE) {
    assert(fb_list->free_buffers > 0);
    fb_list->free_buffers--;
  }

  fb_list->fb_stat[id].b_used = FB_UNALLOCATED;
  fb_list->fb_stat[id].n_ref_count = 0;
  fb_list->fb_stat[id].data = NULL;
}

void *GetDataById(struct FrameBufferList *fb_list, u32 id) {
  assert(id < MAX_FRAME_BUFFER_NUMBER);
  assert(fb_list->fb_stat[id].b_used != FB_UNALLOCATED);

  return (void *)fb_list->fb_stat[id].data;
}

u32 GetIdByData(struct FrameBufferList *fb_list, const void *data) {
  u32 id = 0;
  assert(data);

  do {
    if (fb_list->fb_stat[id].data == data) break;
    id++;
  } while (id < MAX_FRAME_BUFFER_NUMBER);

  return id < MAX_FRAME_BUFFER_NUMBER ? id : FB_NOT_VALID_ID;
}
void IncrementRefUsage(struct FrameBufferList *fb_list, u32 id) {
  pthread_mutex_lock(&fb_list->ref_count_mutex);
  fb_list->fb_stat[id].n_ref_count++;
  DPB_TRACE("id = %d rc = %d\n", id, fb_list->fb_stat[id].n_ref_count);
  pthread_mutex_unlock(&fb_list->ref_count_mutex);
}

void DecrementRefUsage(struct FrameBufferList *fb_list, u32 id) {
  struct FrameBufferStatus *bs = fb_list->fb_stat + id;

  pthread_mutex_lock(&fb_list->ref_count_mutex);
  assert(bs->n_ref_count > 0);
  bs->n_ref_count--;

  if (bs->n_ref_count == 0) {
    if (bs->b_used == FB_FREE) {
      fb_list->free_buffers++;
      DPB_TRACE("FREE id = %d\n", id);
    }
    /* signal that this buffer is not referenced anymore */
    pthread_cond_signal(&fb_list->ref_count_cv);
  } else if (bs->b_used == FB_FREE) {
    DPB_TRACE("Free buffer id = %d still referenced\n", id);
  }

  DPB_TRACE("id = %d rc = %d\n", id, fb_list->fb_stat[id].n_ref_count);
  pthread_mutex_unlock(&fb_list->ref_count_mutex);
}

void MarkHWOutput(struct FrameBufferList *fb_list, u32 id, u32 type) {

  pthread_mutex_lock(&fb_list->ref_count_mutex);

  assert(fb_list->fb_stat[id].b_used & FB_ALLOCATED);
  assert(fb_list->fb_stat[id].b_used ^ type);

  fb_list->fb_stat[id].n_ref_count++;
  fb_list->fb_stat[id].b_used |= type;

  DPB_TRACE("id = %d rc = %d\n", id, fb_list->fb_stat[id].n_ref_count);

  pthread_mutex_unlock(&fb_list->ref_count_mutex);
}

void ClearHWOutput(struct FrameBufferList *fb_list, u32 id, u32 type) {
  struct FrameBufferStatus *bs = fb_list->fb_stat + id;

  pthread_mutex_lock(&fb_list->ref_count_mutex);

  assert(bs->b_used & (FB_HW_ONGOING | FB_ALLOCATED));

  bs->n_ref_count--;
  bs->b_used &= ~type;

  if (bs->n_ref_count == 0) {
    if (bs->b_used == FB_FREE) {
      fb_list->free_buffers++;
      DPB_TRACE("FREE id = %d\n", id);
    }
    /* signal that this buffer is not referenced anymore */
    pthread_cond_signal(&fb_list->ref_count_cv);
  }

  if ((bs->b_used & FB_HW_ONGOING) == 0 && (bs->b_used & FB_OUTPUT))
    /* signal that this buffer is done by HW */
    pthread_cond_signal(&fb_list->hw_rdy_cv);

  DPB_TRACE("id = %d rc = %d\n", id, fb_list->fb_stat[id].n_ref_count);

  pthread_mutex_unlock(&fb_list->ref_count_mutex);
}

/* Mark a buffer as a potential (temporal) output. Output has to be marked
 * permanent by FinalizeOutputAll or reverted to non-output by
 * RemoveTempOutputAll.
 */
void MarkTempOutput(struct FrameBufferList *fb_list, u32 id) {
  DPB_TRACE(" id = %d\n", id);
  pthread_mutex_lock(&fb_list->ref_count_mutex);

  assert(fb_list->fb_stat[id].b_used & FB_ALLOCATED);

  fb_list->fb_stat[id].n_ref_count++;
  fb_list->fb_stat[id].b_used |= FB_TEMP_OUTPUT;

  pthread_mutex_unlock(&fb_list->ref_count_mutex);
}

/* Mark all temp output as valid output */
void FinalizeOutputAll(struct FrameBufferList *fb_list) {
  i32 i;
  pthread_mutex_lock(&fb_list->ref_count_mutex);

  for (i = 0; i < MAX_FRAME_BUFFER_NUMBER; i++) {
    if (fb_list->fb_stat[i].b_used & FB_TEMP_OUTPUT) {
      /* mark permanent output */
      fb_list->fb_stat[i].b_used |= FB_OUTPUT;
      /* clean TEMP flag from output */
      fb_list->fb_stat[i].b_used &= ~FB_TEMP_OUTPUT;

      DPB_TRACE("id = %d\n", i);
    }
  }

  pthread_mutex_unlock(&fb_list->ref_count_mutex);
}

void ClearOutput(struct FrameBufferList *fb_list, u32 id) {
  struct FrameBufferStatus *bs = fb_list->fb_stat + id;

  pthread_mutex_lock(&fb_list->ref_count_mutex);

  assert(bs->b_used & (FB_OUTPUT | FB_TEMP_OUTPUT));

  assert(bs->n_ref_count > 0);
  bs->n_ref_count--;

  bs->b_used &= ~(FB_OUTPUT | FB_TEMP_OUTPUT);

  if (bs->n_ref_count == 0) {
    if (bs->b_used == FB_FREE) {
      fb_list->free_buffers++;
      DPB_TRACE("FREE id = %d\n", id);
    }
    /* signal that this buffer is not referenced anymore */
    pthread_cond_signal(&fb_list->ref_count_cv);
  } else if (bs->b_used == FB_FREE) {
    DPB_TRACE("Free buffer id = %d still referenced\n", id);
  }

  DPB_TRACE("id = %d rc = %d\n", id, fb_list->fb_stat[id].n_ref_count);
  pthread_mutex_unlock(&fb_list->ref_count_mutex);
}

u32 PopFreeBuffer(struct FrameBufferList *fb_list) {
  u32 i = 0;
  struct FrameBufferStatus *bs = fb_list->fb_stat;
  do {
    if (bs->b_used == FB_FREE && bs->n_ref_count == 0) {
      bs->b_used = FB_ALLOCATED;
      break;
    }
    bs++;
    i++;
  } while (i < MAX_FRAME_BUFFER_NUMBER);

  assert(i < MAX_FRAME_BUFFER_NUMBER);

  fb_list->free_buffers--;

  DPB_TRACE("id = %d\n", i);

  return i;
}

void PushFreeBuffer(struct FrameBufferList *fb_list, u32 id) {
  assert(id < MAX_FRAME_BUFFER_NUMBER);
  assert(fb_list->fb_stat[id].b_used & FB_ALLOCATED);

  pthread_mutex_lock(&fb_list->ref_count_mutex);

  DPB_TRACE("id = %d\n", id);

  fb_list->fb_stat[id].b_used &= ~FB_ALLOCATED;
  fb_list->fb_stat[id].b_used |= FB_FREE;

  if (fb_list->fb_stat[id].n_ref_count == 0) {
    fb_list->free_buffers++;
    DPB_TRACE("FREE id = %d\n", id);

    /* signal that this buffer is not referenced anymore */
    pthread_cond_signal(&fb_list->ref_count_cv);
  } else
    DPB_TRACE("Free buffer id = %d still referenced\n", id);

  pthread_mutex_unlock(&fb_list->ref_count_mutex);
}

u32 GetFreePicBuffer(struct FrameBufferList *fb_list, u32 old_id) {
  u32 id;

  pthread_mutex_lock(&fb_list->ref_count_mutex);

  /* Wait until a free buffer is available or "old_id"
   * buffer is not referenced anymore */
  while (fb_list->free_buffers == 0 &&
         fb_list->fb_stat[old_id].n_ref_count != 0) {
    DPB_TRACE("NO FREE PIC BUFFER\n");
    pthread_cond_wait(&fb_list->ref_count_cv, &fb_list->ref_count_mutex);
  }

  if (fb_list->fb_stat[old_id].n_ref_count == 0) {
    /*  our old buffer is not referenced anymore => reuse it */
    id = old_id;
  } else {
    id = PopFreeBuffer(fb_list);
  }

  DPB_TRACE("id = %d\n", id);

  pthread_mutex_unlock(&fb_list->ref_count_mutex);

  return id;
}

u32 GetFreeBufferCount(struct FrameBufferList *fb_list) {
  u32 free_buffers;
  pthread_mutex_lock(&fb_list->ref_count_mutex);
  free_buffers = fb_list->free_buffers;
  pthread_mutex_unlock(&fb_list->ref_count_mutex);

  return free_buffers;
}

void SetFreePicBuffer(struct FrameBufferList *fb_list, u32 id) {
  PushFreeBuffer(fb_list, id);
}

void IncrementDPBRefCount(struct DpbStorage *dpb) {
  u32 i;
  DPB_TRACE("\n");
  for (i = 0; i < dpb->dpb_size; i++) {
    IncrementRefUsage(dpb->fb_list, dpb->buffer[i].mem_idx);
    dpb->ref_id[i] = dpb->buffer[i].mem_idx;
  }
}

void DecrementDPBRefCount(struct DpbStorage *dpb) {
  u32 i;
  DPB_TRACE("\n");
  for (i = 0; i < dpb->dpb_size; i++) {
    DecrementRefUsage(dpb->fb_list, dpb->ref_id[i]);
  }
}

u32 IsBufferReferenced(struct FrameBufferList *fb_list, u32 id) {
  int n_ref_count;
  DPB_TRACE(" %d ? ref_count = %d\n", id, fb_list->fb_stat[id].n_ref_count);
  pthread_mutex_lock(&fb_list->ref_count_mutex);
  n_ref_count = fb_list->fb_stat[id].n_ref_count;
  pthread_mutex_unlock(&fb_list->ref_count_mutex);

  return n_ref_count != 0 ? 1 : 0;
}

u32 IsBufferOutput(struct FrameBufferList *fb_list, u32 id) {
  u32 b_output;
  pthread_mutex_lock(&fb_list->ref_count_mutex);
  b_output = fb_list->fb_stat[id].b_used & FB_OUTPUT ? 1 : 0;
  pthread_mutex_unlock(&fb_list->ref_count_mutex);

  return b_output;
}

void MarkOutputPicCorrupt(struct FrameBufferList *fb_list, u32 id, u32 errors) {
  i32 i, rd_id;

  pthread_mutex_lock(&fb_list->out_count_mutex);

  rd_id = fb_list->out_rd_id;

  for (i = 0; i < fb_list->num_out; i++) {
    if (fb_list->out_fifo[rd_id].mem_idx == id) {
      DPB_TRACE("id = %d\n", id);
      fb_list->out_fifo[rd_id].pic.pic_corrupt = errors;
      break;
    }

    rd_id = (rd_id + 1) % MAX_FRAME_BUFFER_NUMBER;
  }

  pthread_mutex_unlock(&fb_list->out_count_mutex);
}

void PushOutputPic(struct FrameBufferList *fb_list,
                   const struct HevcDecPicture *pic, u32 id) {
  if (pic != NULL) {
    pthread_mutex_lock(&fb_list->out_count_mutex);

    assert(IsBufferOutput(fb_list, id));

    while (fb_list->num_out == MAX_FRAME_BUFFER_NUMBER) {
      /* make sure we do not overflow the output */
      pthread_cond_signal(&fb_list->out_empty_cv);
    }

    /* push to tail */
    fb_list->out_fifo[fb_list->out_wr_id].pic = *pic;
    fb_list->out_fifo[fb_list->out_wr_id].mem_idx = id;
    fb_list->num_out++;

    assert(fb_list->num_out <= MAX_FRAME_BUFFER_NUMBER);

    fb_list->out_wr_id++;
    if (fb_list->out_wr_id >= MAX_FRAME_BUFFER_NUMBER) fb_list->out_wr_id = 0;

    pthread_mutex_unlock(&fb_list->out_count_mutex);
  }

  if (pic != NULL)
    DPB_TRACE("num_out = %d\n", fb_list->num_out);
  else
    DPB_TRACE("EOS\n");

  /* pic == NULL signals the end of decoding in which case we just need to
   * wake-up the output thread (potentially sleeping) */
  sem_post(&fb_list->out_count_sem);
}

u32 PeekOutputPic(struct FrameBufferList *fb_list, struct HevcDecPicture *pic) {
  u32 mem_idx;
  struct HevcDecPicture *out;

  sem_wait(&fb_list->out_count_sem);

  pthread_mutex_lock(&fb_list->out_count_mutex);
  if (!fb_list->num_out) {
    pthread_mutex_unlock(&fb_list->out_count_mutex);
    DPB_TRACE("Output empty, EOS\n");
    return 0;
  }

  pthread_mutex_unlock(&fb_list->out_count_mutex);

  out = &fb_list->out_fifo[fb_list->out_rd_id].pic;
  mem_idx = fb_list->out_fifo[fb_list->out_rd_id].mem_idx;

  pthread_mutex_lock(&fb_list->ref_count_mutex);

  while ((fb_list->fb_stat[mem_idx].b_used & FB_HW_ONGOING) != 0)
    pthread_cond_wait(&fb_list->hw_rdy_cv, &fb_list->ref_count_mutex);

  pthread_mutex_unlock(&fb_list->ref_count_mutex);

  /* pop from head */
  (void)DWLmemcpy(pic, out, sizeof(struct HevcDecPicture));

  DPB_TRACE("id = %d\n", mem_idx);

  pthread_mutex_lock(&fb_list->out_count_mutex);

  fb_list->num_out--;
  if (fb_list->num_out == 0) {
    pthread_cond_signal(&fb_list->out_empty_cv);
  }

  /* go to next output */
  fb_list->out_rd_id++;
  if (fb_list->out_rd_id >= MAX_FRAME_BUFFER_NUMBER) fb_list->out_rd_id = 0;

  pthread_mutex_unlock(&fb_list->out_count_mutex);

  return 1;
}

u32 PopOutputPic(struct FrameBufferList *fb_list, u32 id) {
  if (!IsBufferOutput(fb_list, id)) {
    assert(0);
    return 1;
  }

  ClearOutput(fb_list, id);

  return 0;
}

void RemoveTempOutputAll(struct FrameBufferList *fb_list) {
  i32 i;

  for (i = 0; i < MAX_FRAME_BUFFER_NUMBER; i++) {
    if (fb_list->fb_stat[i].b_used & FB_TEMP_OUTPUT) {
      ClearOutput(fb_list, i);
    }
  }
}

u32 IsOutputEmpty(struct FrameBufferList *fb_list) {
  u32 num_out;
  pthread_mutex_lock(&fb_list->out_count_mutex);
  num_out = fb_list->num_out;
  pthread_mutex_unlock(&fb_list->out_count_mutex);

  return num_out == 0 ? 1 : 0;
}

void WaitOutputEmpty(struct FrameBufferList *fb_list) {
  if (!fb_list->b_initialized) return;

  pthread_mutex_lock(&fb_list->out_count_mutex);
  while (fb_list->num_out != 0) {
    pthread_cond_wait(&fb_list->out_empty_cv, &fb_list->out_count_mutex);
  }
  pthread_mutex_unlock(&fb_list->out_count_mutex);
}

void WaitListNotInUse(struct FrameBufferList *fb_list) {
  int i;

  DPB_TRACE("\n");

  if (!fb_list->b_initialized) return;

  for (i = 0; i < MAX_FRAME_BUFFER_NUMBER; i++) {
    pthread_mutex_lock(&fb_list->ref_count_mutex);
    /* Wait until all buffers are not referenced */
    while (fb_list->fb_stat[i].n_ref_count != 0) {
      pthread_cond_wait(&fb_list->ref_count_cv, &fb_list->ref_count_mutex);
    }
    pthread_mutex_unlock(&fb_list->ref_count_mutex);
  }
}
