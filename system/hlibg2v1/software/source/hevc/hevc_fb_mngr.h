/* Copyright 2013 Google Inc. All Rights Reserved. */

#ifndef HEVC_FB_MNGR_H_
#define HEVC_FB_MNGR_H_

#include "basetype.h"
#include "hevcdecapi.h"

#include <pthread.h>
#include <semaphore.h>

#define MAX_FRAME_BUFFER_NUMBER 34
#define FB_NOT_VALID_ID ~0U

#define FB_HW_OUT_FIELD_TOP 0x10U
#define FB_HW_OUT_FIELD_BOT 0x20U
#define FB_HW_OUT_FRAME (FB_HW_OUT_FIELD_TOP | FB_HW_OUT_FIELD_BOT)

struct FrameBufferStatus {
  u32 n_ref_count;
  u32 b_used;
  const void *data;
};

struct OutElement {
  u32 mem_idx;
  struct HevcDecPicture pic;
};

struct FrameBufferList {
  int b_initialized;
  struct FrameBufferStatus fb_stat[MAX_FRAME_BUFFER_NUMBER];
  struct OutElement out_fifo[MAX_FRAME_BUFFER_NUMBER];
  int out_wr_id;
  int out_rd_id;
  int free_buffers;
  int num_out;

  sem_t out_count_sem;
  pthread_mutex_t out_count_mutex;
  pthread_cond_t out_empty_cv;
  pthread_mutex_t ref_count_mutex;
  pthread_cond_t ref_count_cv;
  pthread_cond_t hw_rdy_cv;
};

struct DpbStorage;

u32 InitList(struct FrameBufferList *fb_list);
void ReleaseList(struct FrameBufferList *fb_list);

u32 AllocateIdUsed(struct FrameBufferList *fb_list, const void *data);
u32 AllocateIdFree(struct FrameBufferList *fb_list, const void *data);
void ReleaseId(struct FrameBufferList *fb_list, u32 id);
void *GetDataById(struct FrameBufferList *fb_list, u32 id);
u32 GetIdByData(struct FrameBufferList *fb_list, const void *data);

void IncrementRefUsage(struct FrameBufferList *fb_list, u32 id);
void DecrementRefUsage(struct FrameBufferList *fb_list, u32 id);

void IncrementDPBRefCount(struct DpbStorage *dpb);
void DecrementDPBRefCount(struct DpbStorage *dpb);

void MarkHWOutput(struct FrameBufferList *fb_list, u32 id, u32 type);
void ClearHWOutput(struct FrameBufferList *fb_list, u32 id, u32 type);

void MarkTempOutput(struct FrameBufferList *fb_list, u32 id);
void ClearOutput(struct FrameBufferList *fb_list, u32 id);

void FinalizeOutputAll(struct FrameBufferList *fb_list);
void RemoveTempOutputAll(struct FrameBufferList *fb_list);

u32 GetFreePicBuffer(struct FrameBufferList *fb_list, u32 old_id);
void SetFreePicBuffer(struct FrameBufferList *fb_list, u32 id);
u32 GetFreeBufferCount(struct FrameBufferList *fb_list);

void PushOutputPic(struct FrameBufferList *fb_list,
                   const struct HevcDecPicture *pic, u32 id);
u32 PeekOutputPic(struct FrameBufferList *fb_list, struct HevcDecPicture *pic);
u32 PopOutputPic(struct FrameBufferList *fb_list, u32 id);

void MarkOutputPicCorrupt(struct FrameBufferList *fb_list, u32 id, u32 errors);

u32 IsBufferReferenced(struct FrameBufferList *fb_list, u32 id);
u32 IsOutputEmpty(struct FrameBufferList *fb_list);
void WaitOutputEmpty(struct FrameBufferList *fb_list);
void WaitListNotInUse(struct FrameBufferList *fb_list);

#endif /*  HEVC_FB_MNGR_H_ */
