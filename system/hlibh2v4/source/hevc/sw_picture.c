/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "instance.h"
#include "sw_picture.h"
#include "sw_slice.h"
#include "tools.h"


/*------------------------------------------------------------------------------
  get_picture returns picture based on poc or return the first free
  (ref_cnt == 0) and not used as referenced picture (referenced == 0).
------------------------------------------------------------------------------*/
struct sw_picture *get_picture(struct container *c, i32 poc)
{
  struct node *n;
  struct sw_picture *p;

  for (n = c->picture.tail; n; n = n->next)
  {
    p = (struct sw_picture *)n;
    if (poc < 0)
    {
      if (!p->ref_cnt && !p->reference) return p;
    }
    else
    {
      if (p->poc == poc) return p;
    }
  }
  return NULL;
}

/*------------------------------------------------------------------------------
  init_image
------------------------------------------------------------------------------*/
i32 sw_init_image(struct sw_picture *p, struct sw_data *image,
                  i32 w, i32 h, enum sw_image_type type, bool bPading, i32 padw, i32 padh)
{
  (void)bPading;
  (void)p;
  image->type = type;
  image->lum_width  = w;
  image->lum_height = h;
  image->ch_width   = w / 2;
  image->ch_height  = h / 2;
  image->lum_pad_width = padw;
  image->lum_pad_height = padh;
  image->ch_pad_width = padw / 2;
  image->ch_pad_height = padh / 2;
  return OK;
}


void create_slices_ctrlsw(struct sw_picture *p, struct pps *pps, u32 sliceSize)
{
  //check on valid sliceSize
  //if((u32)p->sliceSize == sliceSize)
  //  return;

  //free old slices
  sw_free_slices((struct queue *)&p->slice);

  //init new slices
  i32 ctbs_per_slice;
  i32 i, num;
  struct slice *slice;

  p->sliceSize = sliceSize;
  int sliceNum = (sliceSize == 0) ?  1 : ((pps->ctb_per_column + (sliceSize - 1)) / sliceSize);
  p->sliceNum = sliceNum;
  ctbs_per_slice = (sliceSize == 0) ? pps->ctb_per_picture : (pps->ctb_per_row * (i32)sliceSize);
  ASSERT(ctbs_per_slice);

  for (i = 0; i < pps->ctb_per_picture; i++)
  {

    num = i / ctbs_per_slice;

    if (!(slice = sw_get_slice(&p->slice, num)))
    {
      if (!(slice = sw_create_slice(num))) goto out;
      queue_put(&p->slice, (struct node *)slice);
    }
    p->sliceInst = slice;
    p->sliceInst->ctb_per_slice++;
    //p->ctbs[i].slice = p->sliceInst;
    //ASSERT(p->ctbs[i].slice);
  }
out:
  return;
}

/*------------------------------------------------------------------------------
  create_picture
------------------------------------------------------------------------------*/
struct sw_picture *create_picture_ctrlsw(struct hevc_instance *hevc_instance, struct vps *vps, struct sps *sps,
    struct pps *pps, u32 sliceSize, u32 srcWidth, u32 srcHeight)
{
  struct sw_picture *p;
  struct slice *slice;
  //struct ctb *ctb;
  //struct cache *ref;
  i32 w, h;
  i32 i, num;
  i32 ctbs_per_slice;
  if (!(p = calloc(1, sizeof(struct sw_picture)))) return NULL;

  //if (!(p->slice = qalloc(&p->memory, 1, sizeof(struct slice)))) goto out;

  /* Slices TODO: this is demo stuff only.... slice and api ??  */
  create_slices_ctrlsw(p, pps, sliceSize);

  /* Active parameter set of this picture */
  p->vps = vps;
  p->sps = sps;
  p->pps = pps;


  /* Input and reconstructed pictures */
  w = (srcWidth + 15) &(~15);
  h = srcHeight;
  if (sw_init_image(p, &p->input, w, h, SW_IMAGE_U8, false, 0, 0)) goto out;
  //w = (sps->width_min_cbs+63);
  //h = sps->height_min_cbs;
  w = pps->ctb_per_row *pps->ctb_size;
  h = pps->ctb_per_column *pps->ctb_size;
  if (sw_init_image(p, &p->recon, w, h, SW_IMAGE_U8, false, 0, 0)) goto out;

  p->picture_memeory_id = hevc_instance->created_pic_num++;
  p->picture_memeory_init = 0;

  /* Two dimensional reference picture pointer list */
  p->rpl = (struct sw_picture ** *)malloc_array(&p->memory, 2, 16,
           sizeof(struct sw_picture *));
  if (!p->rpl) goto out;


  return p;

out:
  sw_free_picture(p);
  return NULL;
}




/*------------------------------------------------------------------------------
  free_picture
------------------------------------------------------------------------------*/
void sw_free_picture(struct sw_picture *p)
{
  sw_free_slices((struct queue *)&p->slice);
  qfree(&p->memory);
  free(p);
}

/*------------------------------------------------------------------------------
  sw_free_pictures
------------------------------------------------------------------------------*/
void sw_free_pictures(struct container *c)
{
  struct node *n;

  while ((n = queue_get(&c->picture)))
  {
    sw_free_picture((struct sw_picture *)n);
  }
}

/*------------------------------------------------------------------------------
  reference_picture_list generates reference picture list using current
  active reference picture set. List is pointers the pictures what
  motion estimation can use. TODO: slice paramaters must take care...
  pic->rpl[0][n] is pointer to picture RefPicList0[n])
  pic->rpl[1][n] is pointer to picture RefPicList1[n])
------------------------------------------------------------------------------*/
void reference_picture_list(struct container *c, struct sw_picture *pic)
{
  struct sw_picture *p;
  struct slice *s;
  struct node *n;
  struct rps *r;
  i32 cnt[2], i, j;
  i32 flag = false;

  r = pic->rps;
  for (i = 0, j = 0; i < r->before_cnt; i++)
  {
    if ((p = get_picture(c, r->before[i])))
    {
      pic->rpl[0][j++] = p;
    }
  }
  for (i = 0; i < r->after_cnt; i++)
  {
    if ((p = get_picture(c, r->after[i])))
    {
      pic->rpl[0][j++] = p;
    }
  }
  cnt[0] = j;

  for (i = 0, j = 0; i < r->after_cnt; i++)
  {
    if ((p = get_picture(c, r->after[i])))
    {
      pic->rpl[1][j++] = p;
    }
  }
  for (i = 0; i < r->before_cnt; i++)
  {
    if ((p = get_picture(c, r->before[i])))
    {
      pic->rpl[1][j++] = p;
    }
  }
  cnt[1] = j;

  //check list1 to support lowdelay B
  if (pic->pps->lists_modification_present_flag)
  {
    pic->sliceInst->ref_pic_list_modification_flag_l0 = 0;
    pic->sliceInst->ref_pic_list_modification_flag_l1 = 0;
    pic->sliceInst->list_entry_l0[0] = 0;
    pic->sliceInst->list_entry_l0[1] = 0;
    pic->sliceInst->list_entry_l1[0] = 0;
    pic->sliceInst->list_entry_l1[1] = 0;

    if (cnt[1] && pic->rpl[1][0]->poc == pic->rpl[0][0]->poc)
    {
      for (i = 1; i < cnt[1]; i ++)
      {
        if (pic->rpl[1][i]->poc != pic->rpl[0][0]->poc)
        {
          pic->sliceInst->ref_pic_list_modification_flag_l1 = 1;
          pic->sliceInst->list_entry_l1[0] = i;
          pic->rpl[1][0] = pic->rpl[1][i];
          break;
        }
      }
    }
  }

#if 1
  // H2V2 only supports 1 reference frame for list0/list1
  for (n = pic->slice.tail; n; n = n->next)
  {
    s = (struct slice *)n;
    s->active_l0_cnt = s->active_l1_cnt = 1;

    if (s->active_l0_cnt != pic->pps->num_ref_idx_l0_default_active ||
        s->active_l1_cnt != pic->pps->num_ref_idx_l1_default_active)
      s->active_override_flag = true;
    else
      s->active_override_flag = false;
  }
#else
  /* Update every slice header what we have in this picture */
  if (cnt[0] != r->before_cnt) flag = true;
  if (cnt[1] != r->after_cnt)  flag = true;

  for (n = pic->slice.tail; n; n = n->next)
  {
    s = (struct slice *)n;
    s->active_override_flag = true || flag || (cnt[0] != s->active_l0_cnt) || (cnt[1] != s->active_l1_cnt);
    s->active_l0_cnt = cnt[0];
    s->active_l1_cnt = cnt[1];
  }
#endif
}
