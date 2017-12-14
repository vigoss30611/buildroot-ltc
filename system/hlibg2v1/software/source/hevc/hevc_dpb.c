/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "hevc_cfg.h"
#include "hevc_dpb.h"
#include "hevc_slice_header.h"
#include "hevc_image.h"
#include "hevc_util.h"
#include "basetype.h"
#include "dwl.h"
#include "hevc_container.h"


/* Function style implementation for IS_REFERENCE() macro to fix compiler
 * warnings */
static u32 IsReference(const struct DpbPicture a) {
  return (a.status && a.status != EMPTY);
}

static u32 IsExisting(const struct DpbPicture *a) {
  return (a->status > NON_EXISTING && a->status != EMPTY);
}

static u32 IsShortTerm(const struct DpbPicture *a) {
  return (a->status == NON_EXISTING || a->status == SHORT_TERM);
}

static u32 IsLongTerm(const struct DpbPicture *a) {
  return a->status == LONG_TERM;
}

static void SetStatus(struct DpbPicture *pic, const enum DpbPictureStatus s) {
  pic->status = s;
}

static void SetPoc(struct DpbPicture *pic, const i32 poc) {
  pic->pic_order_cnt = poc;
}

static i32 GetPoc(struct DpbPicture *pic) {
  return (pic->status == EMPTY ? 0x7FFFFFFF : pic->pic_order_cnt);
}

#define IS_REFERENCE(a) IsReference(a)
#define IS_EXISTING(a) IsExisting(&(a))
#define IS_SHORT_TERM(a) IsShortTerm(&(a))
#define IS_LONG_TERM(a) IsLongTerm(&(a))
#define SET_STATUS(pic, s) SetStatus(&(pic), s)
#define SET_POC(pic, poc) SetPoc(&(pic), poc)
#define GET_POC(pic) GetPoc(&(pic))

#define MAX_NUM_REF_IDX_L0_ACTIVE 16

#define MEM_STAT_DPB 0x1
#define MEM_STAT_OUT 0x2
#define INVALID_MEM_IDX 0xFF

static void DpbBufFree(struct DpbStorage *dpb, u32 i);

static i32 FindDpbPic(struct DpbStorage *dpb, i32 poc);
static i32 FindDpbRefPic(struct DpbStorage *dpb, i32 poc, u32 type);

static /*@null@ */ struct DpbPicture *FindSmallestPicOrderCnt(
    struct DpbStorage *dpb);

static u32 OutputPicture(struct DpbStorage *dpb);

static u32 HevcDpbMarkAllUnused(struct DpbStorage *dpb);

static u32 HevcInitDpb(const void *dwl, struct DpbStorage *dpb,
                       struct DpbInitParams *dpb_params);

/* Output pictures if the are more outputs than reorder delay set in sps */
void HevcDpbCheckMaxLatency(struct DpbStorage *dpb, u32 max_latency) {
  u32 i;

  while (dpb->num_out_pics_buffered > max_latency) {
    i = OutputPicture(dpb);
    ASSERT(i == HANTRO_OK);
    (void)i;
  }
}

/* Update list of output pictures. To be called after slice header is decoded
 * and reference pictures determined based on that. */
void HevcDpbUpdateOutputList(struct DpbStorage *dpb) {
  u32 i;

  /* dpb was initialized not to reorder the pictures -> output current
   * picture immediately */
  if (dpb->no_reordering) {
    /* we dont know the output buffer at this point as
     * HevcAllocateDpbImage() has not been called, yet
     */
  } else {
    /* output pictures if buffer full */
    while (dpb->fullness > dpb->real_size) {
      i = OutputPicture(dpb);
      ASSERT(i == HANTRO_OK);
      (void)i;
    }
  }
}

/* Returns pointer to reference picture at location 'index' in the DPB. */
u8 *HevcGetRefPicData(const struct DpbStorage *dpb, u32 index) {

  if (index >= dpb->dpb_size)
    return (NULL);
  else if (!IS_EXISTING(dpb->buffer[index]))
    return (NULL);
  else
    return (u8 *)(dpb->buffer[index].data->virtual_address);
}

/* Returns pointer to location where next picture shall be stored. */
void *HevcAllocateDpbImage(struct DpbStorage *dpb, i32 pic_order_cnt,
                           i32 pic_order_cnt_lsb, u32 is_idr,
                           u32 current_pic_id, u32 is_tsa_ref) {

  u32 i;
  u32 to_be_displayed;
  u32 *p;

  /* find first unused and not-to-be-displayed pic */
  for (i = 0; i <= dpb->dpb_size; i++) {
    if (!dpb->buffer[i].to_be_displayed && !IS_REFERENCE(dpb->buffer[i])) break;
  }

  ASSERT(i <= dpb->dpb_size);
  dpb->current_out = &dpb->buffer[i];
  dpb->current_out_pos = i;
  dpb->current_out->status = EMPTY;

  if (IsBufferReferenced(dpb->fb_list, dpb->current_out->mem_idx)) {
    u32 new_id = GetFreePicBuffer(dpb->fb_list, dpb->current_out->mem_idx);
    if (new_id != dpb->current_out->mem_idx) {
      SetFreePicBuffer(dpb->fb_list, dpb->current_out->mem_idx);
      dpb->current_out->mem_idx = new_id;
      dpb->current_out->data = GetDataById(dpb->fb_list, new_id);
    }
  }

  ASSERT(dpb->current_out->data);

  to_be_displayed = dpb->no_reordering ? HANTRO_FALSE : HANTRO_TRUE;

  {
    struct DpbPicture *current_out = dpb->current_out;

    current_out->is_idr = is_idr;
    current_out->is_tsa_ref = is_tsa_ref;
    current_out->num_err_mbs = 0;
    current_out->pic_num = (i32)current_pic_id;
    current_out->pic_id = (i32)current_pic_id;
    SET_STATUS(*current_out, SHORT_TERM);
    SET_POC(*current_out, pic_order_cnt);
    current_out->pic_order_cnt_lsb = pic_order_cnt_lsb;
    current_out->to_be_displayed = to_be_displayed;
    if (current_out->to_be_displayed) dpb->num_out_pics_buffered++;
    dpb->fullness++;
    dpb->num_ref_frames++;
  }

  /* store POCs of ref pic buffer for current pic */
  p = dpb->current_out->ref_poc;
  for (i = 0; i < dpb->dpb_size; i++) *p++ = dpb->buffer[i].pic_order_cnt;

  return dpb->current_out->data;
}

/* Initializes DPB.  Reserves memories for the buffer, reference picture list
 * and output buffer. dpb_size indicates the maximum DPB size indicated by the
 * level_idc in the stream.  If no_reordering flag is HANTRO_FALSE the DPB
 * stores dpb_size pictures for display reordering purposes. On the other hand,
 * if the flag is HANTRO_TRUE the DPB only stores max_ref_frames reference
 * pictures and outputs all the pictures immediately. */
#ifndef USE_EXTERNAL_BUFFER
u32 HevcInitDpb(const void *dwl, struct DpbStorage *dpb,
                struct DpbInitParams *dpb_params) {
  u32 i;
  struct FrameBufferList *fb_list = dpb->fb_list;

  ASSERT(dpb_params->pic_size);
  ASSERT(dpb_params->dpb_size);

  /* we trust our memset; ignore return value */
  (void)DWLmemset(dpb, 0, sizeof(*dpb)); /* make sure all is clean */
  (void)DWLmemset(dpb->pic_buff_id, FB_NOT_VALID_ID, sizeof(dpb->pic_buff_id));

  /* restore value */
  dpb->fb_list = fb_list;

  dpb->pic_size = dpb_params->pic_size;
  dpb->max_long_term_frame_idx = NO_LONG_TERM_FRAME_INDICES;

  dpb->real_size = dpb_params->dpb_size;
  dpb->dpb_size = dpb_params->dpb_size + 1;

  dpb->max_ref_frames = dpb_params->dpb_size;
  dpb->no_reordering = dpb_params->no_reordering;
  dpb->fullness = 0;
  dpb->num_out_pics_buffered = 0;
  dpb->num_ref_frames = 0;
  dpb->prev_ref_frame_num = 0;
  dpb->num_out = 0;
  dpb->out_index_w = 0;
  dpb->out_index_r = 0;
  dpb->prev_out_idx = INVALID_MEM_IDX;

  dpb->tot_buffers = dpb->dpb_size + 2 + dpb_params->n_extra_frm_buffers;
  if (dpb->tot_buffers > MAX_FRAME_BUFFER_NUMBER)
    dpb->tot_buffers = MAX_FRAME_BUFFER_NUMBER;

  dpb->dir_mv_offset = NEXT_MULTIPLE(dpb_params->pic_size * 3 / 2, 16);

  if (dpb_params->tbl_sizey) {
    dpb->cbs_tbl_offsety = dpb_params->buff_size - dpb_params->tbl_sizey - dpb_params->tbl_sizec;
    dpb->cbs_tbl_offsetc = dpb_params->buff_size - dpb_params->tbl_sizec;
  }

  for (i = 0; i < dpb->tot_buffers; i++) {
    /* yuv picture + direct mode motion vectors */
    if (DWLMallocRefFrm(dwl, dpb_params->buff_size, dpb->pic_buffers + i) != 0)
      return (MEMORY_ALLOCATION_ERROR);

    if (i < dpb->dpb_size + 1) {
      u32 id = AllocateIdUsed(dpb->fb_list, dpb->pic_buffers + i);
      if (id == FB_NOT_VALID_ID) return MEMORY_ALLOCATION_ERROR;

      dpb->buffer[i].data = dpb->pic_buffers + i;
      dpb->buffer[i].mem_idx = id;
      dpb->pic_buff_id[i] = id;
    } else {
      u32 id = AllocateIdFree(dpb->fb_list, dpb->pic_buffers + i);
      if (id == FB_NOT_VALID_ID) return MEMORY_ALLOCATION_ERROR;

      dpb->pic_buff_id[i] = id;
    }

    {
      void *base =
          (char *)(dpb->pic_buffers[i].virtual_address) + dpb->dir_mv_offset;
      (void)DWLmemset(base, 0, dpb_params->buff_size - dpb->dir_mv_offset);
    }
  }

  dpb->out_buf = DWLmalloc((MAX_DPB_SIZE + 1) * sizeof(struct DpbOutPicture));

  if (dpb->out_buf == NULL) {
    return (MEMORY_ALLOCATION_ERROR);
  }

  return (HANTRO_OK);
}
#else
u32 HevcInitDpb(const void *dec_inst, struct DpbStorage *dpb,
                struct DpbInitParams *dpb_params) {
  u32 i;
  struct FrameBufferList *fb_list = dpb->fb_list;
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;
  static int memset_done = 0;

  ASSERT(dpb_params->pic_size);
  ASSERT(dpb_params->dpb_size);

  /* we trust our memset; ignore return value */
  /* TODO(min): memset only once. */
  if (!memset_done) {
    (void)DWLmemset(dpb, 0, sizeof(*dpb)); /* make sure all is clean */
    (void)DWLmemset(dpb->pic_buff_id, FB_NOT_VALID_ID, sizeof(dpb->pic_buff_id));
    memset_done = 1;
  }

  /* restore value */
  dpb->fb_list = fb_list;

  dpb->pic_size = dpb_params->pic_size;
  dpb->max_long_term_frame_idx = NO_LONG_TERM_FRAME_INDICES;

  dpb->real_size = dpb_params->dpb_size;
  dpb->dpb_size = dpb_params->dpb_size + 1;

  dpb->max_ref_frames = dpb_params->dpb_size;
  dpb->no_reordering = dpb_params->no_reordering;
  dpb->fullness = 0;
  dpb->num_out_pics_buffered = 0;
  dpb->num_ref_frames = 0;
  dpb->prev_ref_frame_num = 0;
  dpb->num_out = 0;
  dpb->out_index_w = 0;
  dpb->out_index_r = 0;
  dpb->prev_out_idx = INVALID_MEM_IDX;

  dpb->tot_buffers = dpb->dpb_size + 2 + dpb_params->n_extra_frm_buffers;
  if (dpb->tot_buffers > MAX_FRAME_BUFFER_NUMBER)
    dpb->tot_buffers = MAX_FRAME_BUFFER_NUMBER;

  dpb->dir_mv_offset = NEXT_MULTIPLE(dpb_params->pic_size * 3 / 2, 16);

  if (dpb_params->tbl_sizey) {
    dpb->cbs_tbl_offsety = dpb_params->buff_size - dpb_params->tbl_sizey - dpb_params->tbl_sizec;
    dpb->cbs_tbl_offsetc = dpb_params->buff_size - dpb_params->tbl_sizec;
  }
  dpb->dpb_reset = 1;
  memset_done = 0;
  dpb->out_buf = DWLmalloc((MAX_DPB_SIZE + 1) * sizeof(struct DpbOutPicture));

  if (dpb->out_buf == NULL) {
    return (MEMORY_ALLOCATION_ERROR);
  }

  if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER)) {
    for (i = 0; i < dpb->tot_buffers; i++) {
      /* yuv picture + direct mode motion vectors */
      /* TODO(min): request external buffers. */
      if (dpb->pic_buffers[i].virtual_address == NULL) {
        dec_cont->next_buf_size = dpb_params->buff_size;
        dec_cont->buf_to_free = NULL;
        dec_cont->buf_type = REFERENCE_BUFFER;
        dec_cont->buf_num = dpb->tot_buffers;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 1;
#endif
        dec_cont->buffer_index = i;
        return DEC_WAITING_FOR_BUFFER;
      }
    }
  } else {
    for (i = 0; i < dpb->tot_buffers; i++) {
      /* yuv picture + direct mode motion vectors */
      if (DWLMallocRefFrm(dec_cont->dwl, dpb_params->buff_size, dpb->pic_buffers + i) != 0)
        return (MEMORY_ALLOCATION_ERROR);

      if (i < dpb->dpb_size + 1) {
        u32 id = AllocateIdUsed(dpb->fb_list, dpb->pic_buffers + i);
        if (id == FB_NOT_VALID_ID) return MEMORY_ALLOCATION_ERROR;

        dpb->buffer[i].data = dpb->pic_buffers + i;
        dpb->buffer[i].mem_idx = id;
        dpb->pic_buff_id[i] = id;
      } else {
        u32 id = AllocateIdFree(dpb->fb_list, dpb->pic_buffers + i);
        if (id == FB_NOT_VALID_ID) return MEMORY_ALLOCATION_ERROR;

        dpb->pic_buff_id[i] = id;
      }

      {
        void *base =
            (char *)(dpb->pic_buffers[i].virtual_address) + dpb->dir_mv_offset;
        (void)DWLmemset(base, 0, dpb_params->buff_size - dpb->dir_mv_offset);
      }
    }
  }

  return (HANTRO_OK);
}
#endif
/* Reset the DPB. This function should be called when an IDR slice (other than
 * the first) activates new sequence parameter set.  Function calls
 * HevcFreeDpb to free old allocated memories and HevcInitDpb to
 * re-initialize the DPB. Same inputs, outputs and returns as for
 * HevcInitDpb. */
#ifndef USE_EXTERNAL_BUFFER
u32 HevcResetDpb(const void *dwl, struct DpbStorage *dpb,
                 struct DpbInitParams *dpb_params) {

  if (dpb->pic_size == dpb_params->pic_size) {
    dpb->max_long_term_frame_idx = NO_LONG_TERM_FRAME_INDICES;

    dpb->no_reordering = dpb_params->no_reordering;
    dpb->flushed = 0;

    if (dpb->real_size == dpb_params->dpb_size) {
      /* number of pictures and DPB size are not changing */
      /* no need to reallocate DPB */
      return (HANTRO_OK);
    }
  }

  HevcFreeDpb(dwl, dpb);

  return HevcInitDpb(dwl, dpb, dpb_params);
}
#else
u32 HevcResetDpb(const void *dec_inst, struct DpbStorage *dpb,
                 struct DpbInitParams *dpb_params) {
  dpb->dpb_reset = 0;
  if (dpb->pic_size == dpb_params->pic_size) {
    dpb->max_long_term_frame_idx = NO_LONG_TERM_FRAME_INDICES;

    dpb->no_reordering = dpb_params->no_reordering;
    dpb->flushed = 0;

    if (dpb->real_size == dpb_params->dpb_size) {
      /* number of pictures and DPB size are not changing */
      /* no need to reallocate DPB */
      return (HANTRO_OK);
    }
  }

  if (HevcFreeDpbExt(dec_inst, dpb) == DEC_WAITING_FOR_BUFFER)
    return DEC_WAITING_FOR_BUFFER;

  return HevcInitDpb(dec_inst, dpb, dpb_params);
}
#endif
/* Determines picture order counts of the reference pictures (of current
 * and subsequenct pictures) based on delta values in ref pic sets. */
void HevcSetRefPicPocList(struct DpbStorage *dpb,
                          struct SliceHeader *slice_header, i32 pic_order_cnt,
                          struct SeqParamSet *sps) {

  u32 i, j, k;
  i32 poc, poc_lsb;
  u32 tot_long_term;
  struct StRefPicSet *ref;

  ref = &slice_header->st_ref_pic_set;

  for (i = 0, j = 0, k = 0; i < ref->num_negative_pics; i++) {
    if (ref->elem[i].used_by_curr_pic)
      dpb->poc_st_curr[j++] = pic_order_cnt + ref->elem[i].delta_poc;
    else
      dpb->poc_st_foll[k++] = pic_order_cnt + ref->elem[i].delta_poc;
  }
  dpb->num_poc_st_curr_before = j;
  for (; i < ref->num_positive_pics + ref->num_negative_pics; i++) {
    if (ref->elem[i].used_by_curr_pic)
      dpb->poc_st_curr[j++] = pic_order_cnt + ref->elem[i].delta_poc;
    else
      dpb->poc_st_foll[k++] = pic_order_cnt + ref->elem[i].delta_poc;
  }
  dpb->num_poc_st_curr = j;
  dpb->num_poc_st_foll = k;

  tot_long_term =
      slice_header->num_long_term_pics + slice_header->num_long_term_sps;
  for (i = 0, j = 0, k = 0; i < tot_long_term; i++) {
    if (i < slice_header->num_long_term_sps)
      poc_lsb = sps->lt_ref_pic_poc_lsb[slice_header->lt_idx_sps[i]];
    else
      poc_lsb = slice_header->poc_lsb_lt[i];
    poc = poc_lsb;

    if (slice_header->delta_poc_msb_present_flag[i]) {
      poc += pic_order_cnt - (i32)slice_header->delta_poc_msb_cycle_lt[i] *
                                 (i32)sps->max_pic_order_cnt_lsb -
             (i32)slice_header->pic_order_cnt_lsb;
    }

    if (slice_header->used_by_curr_pic_lt[i])
      dpb->poc_lt_curr[j++] = poc;
    else
      dpb->poc_lt_foll[k++] = poc;
  }
  dpb->num_poc_lt_curr = j;
  dpb->num_poc_lt_foll = k;
}

void HevcDpbMarkOlderUnused(struct DpbStorage *dpb, i32 pic_order_cnt) {
  u32 i;
  for (i = 0; i < MAX_DPB_SIZE; i++) {

    if (dpb->buffer[i].is_tsa_ref ||
        (IS_REFERENCE(dpb->buffer[i]) &&
         GET_POC(dpb->buffer[i]) <= pic_order_cnt)) {
      SET_STATUS(dpb->buffer[i], UNUSED);
      if (dpb->buffer[i].to_be_displayed) {
        dpb->num_out_pics_buffered--;
        dpb->buffer[i].to_be_displayed = 0;
      }
      DpbBufFree(dpb, i);
    }
  }
  /* output all pictures */
  while (OutputPicture(dpb) == HANTRO_OK)
    ;
}

/* Determines reference pictures for current and subsequent pictures. */
void HevcSetRefPics(struct DpbStorage *dpb, struct SliceHeader *slice_header,
                    i32 pic_order_cnt, struct SeqParamSet *sps, u32 is_idr,
                    u32 is_cra) {

  u32 i;
  i32 idx = 0;
  u32 st_count[MAX_DPB_SIZE + 1] = {0};
  u32 lt_count[MAX_DPB_SIZE + 1] = {0};

  /* TODO: skip for IDR/BLA */
  if (is_idr) {
    (void)HevcDpbMarkAllUnused(dpb);

    /* if no_output_of_prior_pics_flag was set -> the pictures preceding the
     * IDR picture shall not be output -> set output buffer empty */
    if (slice_header->no_output_of_prior_pics_flag) {
      RemoveTempOutputAll(dpb->fb_list);
      dpb->num_out = 0;
      dpb->out_index_w = dpb->out_index_r = 0;
    }

    (void)DWLmemset(dpb->poc_lt_curr, 0, sizeof(dpb->poc_lt_curr));
    (void)DWLmemset(dpb->poc_lt_foll, 0, sizeof(dpb->poc_lt_foll));
    (void)DWLmemset(dpb->poc_st_curr, 0, sizeof(dpb->poc_st_curr));
    (void)DWLmemset(dpb->poc_st_foll, 0, sizeof(dpb->poc_st_foll));
    (void)DWLmemset(dpb->ref_pic_set_lt, 0, sizeof(dpb->ref_pic_set_lt));
    (void)DWLmemset(dpb->ref_pic_set_st, 0, sizeof(dpb->ref_pic_set_st));
    dpb->num_poc_lt_curr = 0;
    dpb->num_poc_st_curr = 0;
    return;
  } else if (is_cra) {
    (void)HevcDpbMarkOlderUnused(dpb, pic_order_cnt);
    return;
  }

  HevcSetRefPicPocList(dpb, slice_header, pic_order_cnt, sps);

  /* TODO: delta poc msb present */
  for (i = 0; i < dpb->num_poc_lt_curr; i++) {
    /* TODO: is it possible to have both long and short term with same
     * poc (at least lsb)? If not, should remove the flag from FindDpbPic */
    idx = FindDpbRefPic(dpb, dpb->poc_lt_curr[i], 1);
    if (idx < 0) idx = FindDpbRefPic(dpb, dpb->poc_lt_curr[i], 0);
    if (idx < 0) idx = FindDpbPic(dpb, dpb->poc_lt_curr[i]);
    dpb->ref_pic_set_lt[i] = idx;
    if (idx >= 0) lt_count[idx]++;
  }
  for (i = 0; i < dpb->num_poc_lt_foll; i++) {
    idx = FindDpbRefPic(dpb, dpb->poc_lt_foll[i], 1);
    if (idx < 0) idx = FindDpbRefPic(dpb, dpb->poc_lt_foll[i], 0);
    if (idx < 0) idx = FindDpbPic(dpb, dpb->poc_lt_foll[i]);
    if (idx >= 0) lt_count[idx]++;
  }

  /* both before and after */
  for (i = 0; i < dpb->num_poc_st_curr; i++) {
    idx = FindDpbRefPic(dpb, dpb->poc_st_curr[i], 0);
    if (idx < 0) idx = FindDpbPic(dpb, dpb->poc_st_curr[i]);
    /*
    if (idx < 0)
        idx = i ? dpb->ref_pic_set_lt[i-1] : 0;
    */
    dpb->ref_pic_set_st[i] = idx;
    if (idx >= 0) st_count[idx]++;
  }
  for (i = 0; i < dpb->num_poc_st_foll; i++) {
    idx = FindDpbRefPic(dpb, dpb->poc_st_foll[i], 0);
    if (idx >= 0) st_count[idx]++;
  }

  /* mark pics in dpb */
  for (i = 0; i <= dpb->dpb_size; i++) {
    if (st_count[i])
      SET_STATUS(dpb->buffer[i], SHORT_TERM);
    else if (lt_count[i])
      SET_STATUS(dpb->buffer[i], LONG_TERM);
    /* picture marked as not used */
    else if (IS_REFERENCE(dpb->buffer[i])) {
      SET_STATUS(dpb->buffer[i], UNUSED);
      DpbBufFree(dpb, i);
    }
  }
  /* if last element (index dpb_size) is used as reference -> swap with an
   * unused element earlier in the list (probably not necessary if dpb_size
   * less than 16, but done anyway) */
  if (IS_REFERENCE(dpb->buffer[dpb->dpb_size])) {
    for (i = 0; i < dpb->dpb_size; i++) {
      if (!IS_REFERENCE(dpb->buffer[i])) {
        idx = i;
        break;
      }
    }
    ASSERT(idx < dpb->dpb_size);
    {
      struct DpbPicture tmp_pic = dpb->buffer[i];

      dpb->buffer[idx] = dpb->buffer[dpb->dpb_size];
      dpb->buffer[dpb->dpb_size] = tmp_pic;
    }
    /* check ref pic lists */
    for (i = 0; i < dpb->num_poc_lt_curr; i++) {
      if (dpb->ref_pic_set_lt[i] == dpb->dpb_size) dpb->ref_pic_set_lt[i] = idx;
    }
    for (i = 0; i < dpb->num_poc_st_curr; i++) {
      if (dpb->ref_pic_set_st[i] == dpb->dpb_size) dpb->ref_pic_set_st[i] = idx;
    }
  }
}

/* Find a picture from the buffer. The picture to be found is
 * identified by poc */
static i32 FindDpbPic(struct DpbStorage *dpb, i32 poc) {

  u32 i = 0;

  while (i <= dpb->dpb_size) {
    if ((dpb->buffer[i].pic_order_cnt == poc ||
         dpb->buffer[i].pic_order_cnt_lsb == poc) &&
        IS_REFERENCE(dpb->buffer[i])) {
      return i;
    }
    i++;
  }

  return (-1);
}

/* Find a reference picture from the buffer. The picture to be found is
 * identified by poc and long_term flag. */
static i32 FindDpbRefPic(struct DpbStorage *dpb, i32 poc, u32 long_term) {

  u32 i = 0;

  while (i <= dpb->dpb_size) {
    if (dpb->buffer[i].pic_order_cnt == poc ||
        (long_term && dpb->buffer[i].pic_order_cnt_lsb == poc)) {
      if (!long_term ? IS_SHORT_TERM(dpb->buffer[i])
                     : IS_LONG_TERM(dpb->buffer[i]))
        return i;
    }
    i++;
  }

  return (-1);
}

/* Finds picture with smallest picture order count. This will be the next
 * picture in display order. */
struct DpbPicture *FindSmallestPicOrderCnt(struct DpbStorage *dpb) {

  u32 i;
  i32 pic_order_cnt;
  struct DpbPicture *tmp;

  ASSERT(dpb);

  pic_order_cnt = 0x7FFFFFFF;
  tmp = NULL;

  for (i = 0; i <= dpb->dpb_size; i++) {
    /* TODO: currently only outputting frames, asssumes that fields of a
     * frame are output consecutively */
    if (dpb->buffer[i].to_be_displayed &&
        GET_POC(dpb->buffer[i]) < pic_order_cnt) {
      tmp = dpb->buffer + i;
      pic_order_cnt = GET_POC(dpb->buffer[i]);
    }
  }

  return (tmp);
}

/* Function to put next display order picture into the output buffer. */
u32 OutputPicture(struct DpbStorage *dpb) {

  struct DpbPicture *tmp;
  struct DpbOutPicture *dpb_out;

  ASSERT(dpb);

  if (dpb->no_reordering) return (HANTRO_NOK);

  tmp = FindSmallestPicOrderCnt(dpb);

  /* no pictures to be displayed */
  if (tmp == NULL) return (HANTRO_NOK);

  /* if output buffer full -> ignore oldest */
  if (dpb->num_out == dpb->dpb_size + 1) {
    // TODO(atna) figure out how to safely handle output overflow

    /* it seems that output overflow can occur with corrupted streams
     * and display smoothing mode
     */

    ClearOutput(dpb->fb_list, dpb->out_buf[dpb->out_index_r].mem_idx);

    dpb->out_index_r++;
    if (dpb->out_index_r == dpb->dpb_size + 1) dpb->out_index_r = 0;
    dpb->num_out--;
  }
  /* remove it from DPB */
  tmp->to_be_displayed = HANTRO_FALSE;
  dpb->num_out_pics_buffered--;

  /* updated output list */
  dpb_out = &dpb->out_buf[dpb->out_index_w]; /* next output */
  dpb_out->data = tmp->data;
  dpb_out->is_idr = tmp->is_idr;
  dpb_out->is_tsa_ref = tmp->is_tsa_ref;
  dpb_out->pic_id = tmp->pic_id;
  dpb_out->num_err_mbs = tmp->num_err_mbs;
  dpb_out->mem_idx = tmp->mem_idx;
  dpb_out->tiled_mode = tmp->tiled_mode;
  dpb_out->cycles_per_mb = tmp->cycles_per_mb;

  dpb_out->pic_width = dpb->pic_width;
  dpb_out->pic_height = dpb->pic_height;
  dpb_out->crop_params = dpb->crop_params;
  dpb_out->bit_depth_luma = dpb->bit_depth_luma;
  dpb_out->bit_depth_chroma = dpb->bit_depth_chroma;

  dpb->num_out++;
  dpb->out_index_w++;
  if (dpb->out_index_w == dpb->dpb_size + 1) dpb->out_index_w = 0;

  if (!IS_REFERENCE(*tmp)) {
    dpb->fullness--;
  }

  MarkTempOutput(dpb->fb_list, tmp->mem_idx);

  return (HANTRO_OK);
}

/* Get next display order picture from the output buffer. */
struct DpbOutPicture *HevcDpbOutputPicture(struct DpbStorage *dpb) {

  u32 tmp_idx;
  DEBUG_PRINT(("HevcDpbOutputPicture: index %d outnum %d\n",
               (dpb->num_out -
                    ((dpb->out_index_w - dpb->out_index_r + dpb->dpb_size + 1) %
                     (dpb->dpb_size + 1)),
                dpb->num_out)));
  if (dpb->num_out) {
    tmp_idx = dpb->out_index_r++;
    if (dpb->out_index_r == dpb->dpb_size + 1) dpb->out_index_r = 0;
    dpb->num_out--;
    dpb->prev_out_idx = dpb->out_buf[tmp_idx].mem_idx;

    return (dpb->out_buf + tmp_idx);
  } else
    return (NULL);
}

/* Flush the DPB. Function puts all pictures needed for display into the
 * output buffer. This function shall be called in the end of the stream to
 * obtain pictures buffered for display re-ordering purposes. */
void HevcFlushDpb(struct DpbStorage *dpb) {

  dpb->flushed = 1;
  /* output all pictures */
  while (OutputPicture(dpb) == HANTRO_OK)
    ;
}

/* Frees the memories reserved for the DPB. */
#ifndef USE_EXTERNAL_BUFFER
void HevcFreeDpb(const void *dwl, struct DpbStorage *dpb) {

  u32 i;

  ASSERT(dpb);

  for (i = 0; i < dpb->tot_buffers; i++) {
    if (dpb->pic_buffers[i].virtual_address != NULL) {
      DWLFreeRefFrm(dwl, dpb->pic_buffers + i);
      if (dpb->pic_buff_id[i] != FB_NOT_VALID_ID)
        ReleaseId(dpb->fb_list, dpb->pic_buff_id[i]);
    }
  }

  if (dpb->out_buf != NULL) {
    DWLfree(dpb->out_buf);
    dpb->out_buf = NULL;
  }
}
#else
i32 HevcFreeDpbExt(const void *dec_inst, struct DpbStorage *dpb) {

  u32 i;
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;

  ASSERT(dpb);

  if (IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER)) {
    for (i = 0; i < dpb->tot_buffers; i++) {
      if (dpb->pic_buffers[i].virtual_address != NULL) {
        if (dpb->pic_buff_id[i] != FB_NOT_VALID_ID)
          ReleaseId(dpb->fb_list, dpb->pic_buff_id[i]);
        dec_cont->buf_to_free = dpb->pic_buffers + i;
        dec_cont->next_buf_size = 0;
#ifdef ASIC_TRACE_SUPPORT
        dec_cont->is_frame_buffer = 1;
#endif
        return DEC_WAITING_FOR_BUFFER;
      }
    }
  } else {
    for (i = 0; i < dpb->tot_buffers; i++) {
      if (dpb->pic_buffers[i].virtual_address != NULL) {
        DWLFreeRefFrm(dec_cont->dwl, dpb->pic_buffers + i);
        if (dpb->pic_buff_id[i] != FB_NOT_VALID_ID)
          ReleaseId(dpb->fb_list, dpb->pic_buff_id[i]);
      }
    }
  }

  if (dpb->out_buf != NULL) {
    DWLfree(dpb->out_buf);
    dpb->out_buf = NULL;
  }

  return 0;
}

i32 HevcFreeDpb(const void *dec_inst, struct DpbStorage *dpb) {

  u32 i;
  struct HevcDecContainer *dec_cont = (struct HevcDecContainer *)dec_inst;

  ASSERT(dpb);

  for (i = 0; i < dpb->tot_buffers; i++) {
    if (dpb->pic_buffers[i].virtual_address != NULL) {
      if (!IS_EXTERNAL_BUFFER(dec_cont->ext_buffer_config, REFERENCE_BUFFER))
        DWLFreeRefFrm(dec_cont->dwl, dpb->pic_buffers + i);
      if (dpb->pic_buff_id[i] != FB_NOT_VALID_ID)
        ReleaseId(dpb->fb_list, dpb->pic_buff_id[i]);
    }
  }

  if (dpb->out_buf != NULL) {
    DWLfree(dpb->out_buf);
    dpb->out_buf = NULL;
  }

  return 0;
}
#endif

/* Updates DPB ref frame count (and fullness) after marking a ref pic as
 * unused */
void DpbBufFree(struct DpbStorage *dpb, u32 i) {

  dpb->num_ref_frames--;

  if (!dpb->buffer[i].to_be_displayed) dpb->fullness--;
}

/* Marks all reference pictures as unused and outputs all the pictures. */
u32 HevcDpbMarkAllUnused(struct DpbStorage *dpb) {

  u32 i;

  for (i = 0; i < MAX_DPB_SIZE; i++) {
    if (IS_REFERENCE(dpb->buffer[i])) {
      SET_STATUS(dpb->buffer[i], UNUSED);
      DpbBufFree(dpb, i);
    }
  }

  /* output all pictures */
  while (OutputPicture(dpb) == HANTRO_OK)
    ;
  dpb->num_ref_frames = 0;
  dpb->max_long_term_frame_idx = NO_LONG_TERM_FRAME_INDICES;
  dpb->prev_ref_frame_num = 0;

  return (HANTRO_OK);
}
