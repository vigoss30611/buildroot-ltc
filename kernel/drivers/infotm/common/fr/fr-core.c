/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Description: ring-buffer of frames management
 *
 * Author:
 *     warits <warits.wang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  02/20/2016 init
 * 1.2  03/10/2016 add statistics
 * 1.3  03/12/2016 add variable buffer, and protection mechanisms
 * 1.4  04/06/2016 support nodrop flag
 * 1.5  11/30/2016 enable protected mode, enhance error treatment
 *
 * TODO: crash ref protection
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fr.h>

#include <asm/ioctl.h>
#include <asm/uaccess.h>
#include <asm/current.h>
#include <asm/div64.h>

#include <uapi/linux/fr-user.h>

#define CONFIG_FR_WAIT_EVENT
#define fr_log_error(x...) do {printk(KERN_ERR "fr-core: (%s): ",  \
					 current->comm); printk(x); } while (0)
#if 0
#define fr_log_info(x...) printk(KERN_ERR "fr-core: " x)
#else
#define fr_log_info(x...)
#endif

#if 0
#define fr_log_debug(x...) do {printk(KERN_ERR "fr-core: (%03d)%s: ",  \
					 __LINE__, __func__); printk(x); } while (0)
#else
#define fr_log_debug(x...)
#endif
#define fr_log(l, x...) fr_log_##l(x)

/* debug stack */
#if 0
DEFINE_SPINLOCK(dslock);
static int ds_buffer[100];
static int ds_cursor = 0;
#define ds_push() do { spin_lock(&dslock);  \
	ds_buffer[ds_cursor++] = __LINE__; ds_cursor %= 100;  \
	spin_unlock(&dslock); } while(0)
static void ds_show(void) { int i = ds_cursor, j = 0;
	do { if(ds_buffer[i]) printk(KERN_ERR "%3d%s", ds_buffer[i],
		 (++j % 16)? " ": "\n"); i = (i+1)%100;} while(i != ds_cursor);
}
#endif

static LIST_HEAD(list_of_frs);
/* the st_switch allows state change from
 * READY to WRITING, st_switch_nodrop does not allow
 * this operation. So allocated fr using st_switch_nodrop
 * will not be overwrited unless they are firstly set to READ
 * (which is achieved by fr_put_ref)
 *            ---> IN   WI   RI   RY   RD <---
 * FIXME: for now, since st_switch do not allow state from
 * READING to WRITING, so the result is similar with
 * st_switch_nodrop, except it returns fr->latest insdead of
 * fr->oldest on first get_ref. So st_switch is currently
 * not used (set to NULL). Thus all state changement is allowed
 * which makes producers the first priority. -- nothing can
 * stop the producer.
 */
static const int st_switch[FR_BUFST_COUNT][FR_BUFST_COUNT] = {
	{-1, 1, 0, -1, -1}, {-1, -1, 0, 1, -1},
	{-1, 0, 1, -1, 1}, {-1, 1, 1, -1, -1},
	{-1, 1, 1, -1, -1},
}, st_switch_nodrop[FR_BUFST_COUNT][FR_BUFST_COUNT] = {
	{-1, 1, 0, -1, -1}, {-1, -1, 0, 1, -1},
	{-1, 0, 1, -1, 1}, {-1, 0, 1, -1, -1},
	{-1, 1, 1, -1, -1},
};

static inline int
fr_content_serial_inc(struct fr *fr)
{
	/* this can keep run for more than
	 * two years, when using 60fps
	 */
	return ++fr->serial_inc;
}

static inline int
fr_content_serial_next(struct fr *fr, int serial)
{
	return ++serial;
}

static inline int
fr_buf_get_switch(struct fr *fr, int st0, int st1)
{
	return *(fr->buf_st_switch + st0
		* FR_BUFST_COUNT + st1);
}

static int
fr_buf_set_state(struct fr *fr, struct fr_buf *buf, int state)
{
	int block = 0, state_change = 0, st, sw, ret;

	if(!fr->buf_st_switch) return state;

	spin_lock(&buf->state_lock);
	fr_log(debug, "%s:%p %d->%d\n",
		fr->name, buf, buf->state, state);

	st = buf->state;
	sw = fr_buf_get_switch(fr, st, state);
	if(!sw) block = 1;
	else if(sw == 1) {
		if(state == FR_BUFST_WRITING)
			buf->ref_count = 0;
		if(state == FR_BUFST_READING)
			buf->ref_count++;
		if(state == FR_BUFST_READ && --buf->ref_count);
		else buf->state = state;

		state_change = 1;
	}
	spin_unlock(&buf->state_lock);


	if(block) {
		if (fr->timeout)
		{
			ret = wait_event_interruptible_timeout(buf->state_change, st != buf->state, fr->timeout);
			if (ret > 0)
				return fr_buf_set_state(fr, buf, state);
			else if (ret == 0)
				return FR_TIMEOUT;
		}
		else
		{
			ret = wait_event_interruptible(buf->state_change, st != buf->state);
			if(!ret) return fr_buf_set_state(fr, buf, state);
		}
		/* interrupted (Ctrl-C etc.,) */
		fr_log(error, "%s:%p state change failed %d->%d\n",
		  fr->name, buf, st, state);
		return FR_ERESTARTSYS;
	} else if(state_change) {
		wake_up_interruptible(&buf->state_change);
		return buf->state;
	}

	/* failed */
	fr_log(error, "state from %d to %d illeagal\n", st, state);
	// BUG();
	return FR_EINVOPR;
}

static int __fr_getms(void)
{
	ktime_t a;
	a = ktime_get_boottime();
	do_div(a.tv64, 1000000);
	return (int)a.tv64;
}

static inline void
#define MIN(a, b) ((a > b)? b: a)
#define MAX(a, b) ((a > b)? a: b)
fr_poke_timepin(struct fr_statistics *st, int stage)
{
	if(unlikely(!st)) return;
	st->pid = current->pid;
	st->ti[st->id[stage]][stage] = -1;
	st->tp[stage] = __fr_getms();
}

static inline void
fr_peek_timepin(struct fr_statistics *st, int stage)
{
	int now = __fr_getms();

	if(unlikely(!st || !st->ti ||
		st->ti[st->id[stage]][stage] != -1)) return;

	st->ti[st->id[stage]][stage] = now - st->tp[stage];
	st->id[stage]++;
	st->id[stage] &= (FRSTFRAMES - 1);

	if(stage == FRTIME_ST_PUT)
		st->count++;
}

static struct fr_statistics *
fr_get_ref_st(struct fr *fr)
{
	int i;

	for(i = 0; i < FRSTMAXREF; i++)
		if(fr->st_ref[i].pid == current->pid)
			goto found;

	for(i = 0; i < FRSTMAXREF; i++)
		if(fr->st_ref[i].pid == 0)
			goto found;

	for(i = 0; i < FRSTMAXREF; i++)
		if(!find_task_by_vpid(fr->st_ref[i].pid)) {
			fr_log(info, "warning: pid %i(%s) is removed because"
			 " of the ref process is died.\n",
			 fr->st_ref[i].pid, fr->name);
			memset(&fr->st_ref[i], 0, sizeof(struct fr_statistics));
			break;
		}

found:
	return &fr->st_ref[i];
}

void fr_buf_print(struct fr_buf *buf)
{
	if(!buf) return ;
	fr_log(error, "%p: %6d%10x%10p  %08x%18llx\n", buf,
		buf->serial, buf->size, buf->virt_addr, buf->phys_addr, buf->timestamp);
}

struct fr *
fr_get_fr_by_name(const char *name)
{
	struct fr* fr = NULL;

	if(list_empty(&list_of_frs))
	  goto end;

	list_for_each_entry(fr, &list_of_frs, node)
		if(strcmp(fr->name, name) == 0)
		  goto end;

	fr = NULL;
end:
	return fr;
}

int fr_float_setmax(struct fr *fr, int max)
{
	/* cannot modify float_max while writing */
	mutex_lock(&fr->wlock);

	if(max > fr->size) {
		fr_log(error, "float setmax failed, requesting max(%x)"
		 " larger than fr_size(%x)\n", max, fr->size);
		goto finish;
	}
	fr->float_max = max;
finish:
	mutex_unlock(&fr->wlock);
	return 0;
}

static void fr_float_buf_init(struct fr *fr, struct fr_buf *buf)
{
    int sn;

    spin_lock(&fr->fvlock);
    /* backup variables */
    sn = buf->ref_serial;
    memset(buf, 0, sizeof(*buf));

    /* serial, node.next, ~node.prev must be zero,
     * but since we did memset, we don't need to
     * set them to zero manually
     */
    init_waitqueue_head(&buf->state_change);
    buf->size = fr->float_max;
    buf->virt_addr = (void *)buf + PAGE_SIZE;
    buf->phys_addr = virt_to_phys(buf->virt_addr);
    buf->node.prev = (void *)~(uint32_t)buf->node.next;
    spin_lock_init(&buf->state_lock);
    buf->map_size = fr->float_max;
    buf->u32BasePhyAddr = fr->ring->phys_addr;
    buf->s32TotalSize = fr->size;

    /* restore */
    buf->ref_serial = sn;
    spin_unlock(&fr->fvlock);
}

static void fr_float_buf_destroy(struct fr *fr, struct fr_buf *buf)
{
	spin_lock(&fr->fvlock);
	fr_log(debug, "%s, %p:%x@%p\n", fr->name, buf, buf->size, buf->virt_addr);
	buf->node.next = buf->node.prev = 0;
	fr_log(debug, "(done)\n");
	spin_unlock(&fr->fvlock);
}

int
__fr_free(struct fr *fr)
{
	struct fr_buf *buf;
	int c = atomic_inc_and_test(&fr->freed); /* first: (-1) +1 == 0 */
	if (!c) {
		fr_log(error,"%s fr(%p) checked reentrant!\n", __func__, fr);
		return 0;
	}

	fr_log(debug, "%s (%p)\n", fr->name, fr);

	/* no ring need to be freed */
	if(!fr->ring) goto free_fr;
	else if(FR_ISFLOAT(fr)) {
		dma_free_coherent(NULL, fr->size, fr->ring, virt_to_phys(fr->ring));
		goto free_fr;
	}

	do {
		buf = list_first_entry(&fr->ring->node, struct fr_buf, node);
		fr_log(debug, "(free buf) %p\n", buf);
		/* yes, no need to free vacant fr */
		if(buf->virt_addr && !FR_ISVACANT(fr)) dma_free_coherent(NULL, fr->size,
				buf->virt_addr, buf->phys_addr);
		list_del(&buf->node);
		kfree(buf);
	} while(buf != fr->ring);

free_fr:
	fr->magic = 0;
	list_del(&fr->node);
	fr_log(debug, "(fr freed) %s (%p)\n", fr->name, fr);
	kfree(fr);
	return 0;
}
EXPORT_SYMBOL(__fr_free);

void __fr_free_fp(void *fp)
{
	struct fr *fr, *tmp;

	fr = list_entry(list_of_frs.next, typeof(*fr), node);
	for (; &fr->node != &list_of_frs;) {
		tmp = fr;
		fr = list_entry(fr->node.next, typeof(*fr), node);
		if(tmp->fp == fp) {
			fr_log(error, "clean-fp(%p) %s (%p)\n",
					tmp->fp, tmp->name, tmp);
			__fr_free(tmp);
		}
	}
}

void __fr_clean(void)
{
	struct task_struct *task;
	struct fr *fr, *tmp;

	fr = list_entry(list_of_frs.next, typeof(*fr), node);
	for (; &fr->node != &list_of_frs;) {
		tmp = fr;
		fr = list_entry(fr->node.next, typeof(*fr), node);
		task = find_task_by_vpid(tmp->owner_pid);
		if(!task) {
			fr_log(error, "clean-pid(%i) %s (%p)\n",
					tmp->owner_pid, tmp->name, tmp);
			/* FIXME: if fr is freed by other process during the
			 * __fr_clean process, the __fr_free() function may
			 * produce panic
			 */
			__fr_free(tmp);
		}
	}
}

struct fr*
__fr_alloc(const char *name, int size, int flag)
{
	struct fr *fr;
	struct fr_buf *buf;
	int i;
    int align_flag = (flag&FR_FLAG_NOALIGN)?__GFP_NOWARN:0;

	fr_log(debug, "0x%x, F:%x for %s\n", size, flag, name);
	if(!flag) return NULL;

	fr = kzalloc(sizeof(struct fr), GFP_KERNEL);
	if(!fr) return NULL;

	strncpy(fr->name, name, FR_SZ_NAME);
	fr->flag = flag;
	fr->size = size;
	INIT_LIST_HEAD(&fr->node);

	if(FR_ISFLOAT(fr)) {
		if((FR_GETFLOATMAX(fr) + PAGE_SIZE) * 3 > size) {
			fr_log(error, "insufficient size %s, 0x%lx at least is required\n",
			fr->name, (FR_GETFLOATMAX(fr) + PAGE_SIZE) * 3);
			goto free;
		}

		/* FIXME: we don't journal the phys for float
		 * buffer, 'cause the phys can be converted from
		 * the virt
		 */
		fr->ring = dma_alloc_coherent(NULL, fr->size,
				&i, GFP_KERNEL|align_flag);
		if(!fr->ring) {
			fr_log(error, "alloc float failed for %s\n", fr->name);
			goto free;
		}
		fr->ring->phys_addr = virt_to_phys(fr->ring);
		fr_float_buf_init(fr, fr->ring);
		fr->oldest = NULL;
		goto finish;
	}

	for(i = 0; i < FR_GETCOUNT(fr); i++)
	{
		buf = kzalloc(sizeof(struct fr_buf), GFP_KERNEL);
		if(!buf) {
			fr_log(error, "alloc fr_buf structure failed\n");
			goto free;
		}

		if(unlikely(FR_ISVACANT(fr))) goto vacant_fr;
		if (i > 0) align_flag = __GFP_NOWARN;
		buf->virt_addr = dma_alloc_coherent(NULL, fr->size,
				&buf->phys_addr, GFP_KERNEL|align_flag);
		if(!buf->virt_addr) {
			fr_log(error, "alloc buffer failed for %s\n", fr->name);
			goto free;
		}

vacant_fr:
		buf->size = fr->size;
		buf->map_size = fr->size;
		spin_lock_init(&buf->state_lock);
		init_waitqueue_head(&buf->state_change);
		if(!i) {
			fr->ring = buf;
			INIT_LIST_HEAD(&buf->node);
		} else
			list_add_tail(&buf->node, &fr->ring->node);
	}

finish:
	fr->cur = fr->ring;
	fr->owner_pid = current->pid;
	fr->buf_st_switch = (fr->flag & FR_FLAG_NODROP)? (int *)st_switch_nodrop:
		(fr->flag & FR_FLAG_PROTECTED)? (int *)st_switch: NULL;
	fr->float_max = FR_GETFLOATMAX(fr);

	mutex_init(&fr->wlock);
	spin_lock_init(&fr->fvlock);
	atomic_set(&fr->freed, -1);

	fr_log(info, "fr allocated: %s (%p) 0x%x (%s:%x)\n",
		fr->name, fr, fr->size, FR_ISFLOAT(fr)?"float":"ring",
		FR_ISFLOAT(fr)?FR_GETFLOATMAX(fr): FR_GETCOUNT(fr));
#ifdef CONFIG_FR_WAIT_EVENT
	init_waitqueue_head(&fr->serial_update);
	init_waitqueue_head(&fr->new_float);
#endif
	fr->magic = FR_MAGIC;
	list_add_tail(&fr->node, &list_of_frs);
	return fr;

free:
	__fr_free(fr);
	return NULL;
}
EXPORT_SYMBOL(__fr_alloc);

static int fr_get_float(struct fr *fr, struct fr_buf** ref)
{
	struct fr_buf *buf = fr->cur, *btm;
	void *loc;
	int ret;

	if(buf->serial) { /* move to next buffer */
		loc = fr->cur->virt_addr + fr->cur->size + (PAGE_SIZE - 1);
		loc = (void *)((uint32_t)loc & ~(PAGE_SIZE - 1));
		fr_log(debug, "(move to next) %p\n", loc);

		if(loc + PAGE_SIZE + fr->float_max >
			 (void *)fr->ring + fr->size) {
			loc = fr->ring;
			fr_log(debug, "(reloc to head) %p\n", loc);
		}

		/* check and move bottom */
		for(btm = fr->oldest; fr_float_is_overlap(fr, btm, loc);) {
			struct fr_buf *tmp = btm;
			btm = (struct fr_buf *)btm->node.next;
			if((ret = fr_buf_set_state(fr, tmp, FR_BUFST_WRITING)) < 0)
				return ret;
			fr->oldest = btm;
			fr_float_buf_destroy(fr, tmp);
		}

		/* link last buffer */
		spin_lock(&fr->fvlock);
		buf->node.next = loc;
		buf->node.prev = (void *)(~(uint32_t)loc);
		spin_unlock(&fr->fvlock);

		buf = loc;
	}
	else fr_log(debug, "(using last buffer) %p\n", buf);

	fr_float_buf_init(fr, buf);
	if(fr_buf_set_state(fr, buf, FR_BUFST_WRITING) < 0)
		BUG();
	fr->cur = buf;
	wake_up_interruptible(&fr->new_float);

	*ref = buf;
	return 0;
}

int fr_get_buf(struct fr* fr, struct fr_buf **buf)
{
	struct fr_buf *buf_try = NULL;
	int ret;

	if(!buf) return -EINVAL;

	fr_peek_timepin(&fr->st_buf, FRTIME_ST_GET);
	fr_poke_timepin(&fr->st_buf, FRTIME_ST_GET);
	fr_poke_timepin(&fr->st_buf, FRTIME_ST_WAIT);
	mutex_lock(&fr->wlock);

	if(unlikely(FR_ISFLOAT(fr))) {
		ret = fr_get_float(fr, &buf_try);
	} else {
		/* move cur to next entry */
		buf_try = list_first_entry(&fr->cur->node, struct fr_buf, node);
		if((ret = fr_buf_set_state(fr, buf_try, FR_BUFST_WRITING)) < 0)
			return ret;
		if(buf_try == fr->oldest)
			fr->oldest = list_first_entry(&buf_try->node, struct fr_buf, node);
		fr->cur = buf_try;
	}

	fr_peek_timepin(&fr->st_buf, FRTIME_ST_WAIT);
	fr_poke_timepin(&fr->st_buf, FRTIME_ST_DEAL);

	*buf = buf_try;
	return ret;
}
EXPORT_SYMBOL(fr_get_buf);

int fr_put_buf(struct fr* fr, struct fr_buf* buf)
{
	fr_log(debug, "%p\n", buf);
	if(fr->cur != buf) {
		fr_log(error, "%s putting wrong buffer:\n"
				"putting: %p(v) %08x(p)\n"
				"wantted: %p(v) %08x(p)\n", fr->name,
				buf->virt_addr, buf->phys_addr,
				fr->cur->virt_addr, fr->cur->phys_addr);
		BUG();
	}

	/* check float exceed */
	if(FR_ISFLOAT(fr) && buf->size > fr->float_max) {
		fr_log(error, "access over float buffer limit:\n");
		fr_log(error, "fr->ring: 0x%x@%p\n",
				fr->size, fr->ring);
		fr_log(error, "access: 0x%x@%p\n", buf->size,
				buf->virt_addr);
		BUG();
	}

	buf->serial = fr_content_serial_inc(fr);
	fr_log(debug, "(set serial: %p: %d\n", buf, buf->serial);
	if(fr_buf_set_state(fr, buf, FR_BUFST_READY) < 0)
		BUG();
	fr->latest = buf;
	if(!fr->oldest) fr->oldest = buf;

#ifdef CONFIG_FR_WAIT_EVENT
	wake_up_interruptible(&fr->serial_update);
#endif
	mutex_unlock(&fr->wlock);

	fr_peek_timepin(&fr->st_buf, FRTIME_ST_DEAL);
	fr_peek_timepin(&fr->st_buf, FRTIME_ST_PUT);
	fr_poke_timepin(&fr->st_buf, FRTIME_ST_PUT);
	return 0;
}
EXPORT_SYMBOL(fr_put_buf);

int fr_get_ref(struct fr* fr, struct fr_buf** ref)
{
	struct fr_buf *buf, *_ref = *ref;
	int next_serial, ret = FR_ENOBUFFER, ret_wait;

	fr_log(debug, "%p\n", _ref);
	fr_peek_timepin(fr_get_ref_st(fr), FRTIME_ST_GET);
	fr_poke_timepin(fr_get_ref_st(fr), FRTIME_ST_GET);
	fr_poke_timepin(fr_get_ref_st(fr), FRTIME_ST_WAIT);

	if(!_ref) {
        if (fr->en_oldest) {
            printk("=====================>get oldest buffer! fr->en_oldest:%d\n",fr->en_oldest);
            buf = fr->oldest;
        } else {
		    buf = (fr->flag & FR_FLAG_NODROP)? fr->oldest: fr->latest;
        }
    }
	else {
		if(FR_ISFLOAT(fr)) {
			if(!fr_float_valid(fr, _ref)) {
				if(fr->flag & FR_FLAG_NODROP) {
					buf = fr->oldest;
					fr_log(debug, "(return oldest) %p\n", buf);
					goto check_buf;
				} else {
					fr_log(error, "%s (%p:%p) is wiped\n", fr->name,
						_ref, _ref->node.next);
					ret = FR_EWIPED;
					goto out;
				}
			}

			fr_log(debug, "(wait) %p: %p\n", _ref, _ref->node.next);
#ifdef CONFIG_FR_WAIT_EVENT
			if (fr->timeout)
			{
				ret_wait = wait_event_interruptible_timeout(fr->new_float, _ref->node.next, fr->timeout);
				if (ret_wait == 0)
				{
					fr_log(info, "get_ref 1 timeout, %p\n", _ref);
					ret = FR_TIMEOUT;
					goto out;
				}
				else if (ret_wait < 0)
				{
					fr_log(info, "get_ref 11 interrupted, %p\n", _ref);
					ret = FR_ERESTARTSYS;
					goto out;
				}
			}
			else
			{
				ret_wait = wait_event_interruptible(fr->new_float, _ref->node.next);
				if(ret_wait) {
					fr_log(info, "get_ref 1 interrupted, %p\n", _ref);
					ret = FR_ERESTARTSYS;
					goto out;
				}
			}
#else
			while(!(volatile void *)_ref->node.next)
				schedule();
#endif
			buf = (struct fr_buf *)_ref->node.next;
			fr_log(debug, "(ready) %p:%p\n", _ref, buf);

		} else /* for fixed buf */
			buf = list_first_entry(&_ref->node, struct fr_buf, node);
		next_serial = fr_content_serial_next(fr, _ref->ref_serial);

		fr_log(debug, "(wait serial) %p:%p, %d->%d\n", _ref->virt_addr,
			buf->virt_addr, next_serial, buf->serial);

		/* wait until next buffer updated */
#ifdef CONFIG_FR_WAIT_EVENT
		if (fr->timeout)
		{
			ret_wait = wait_event_interruptible_timeout(fr->serial_update, buf->serial >= next_serial, fr->timeout);
			if (ret_wait == 0)
			{
				fr_log(info, "get_ref 2 timeout, %d->%d\n",
							buf->serial, next_serial);
				ret = FR_TIMEOUT;
				goto out;
			}
			else if (ret_wait < 0)
			{
				fr_log(info, "get_ref 22 interrupted, %d->%d\n",
							buf->serial, next_serial);
				ret = FR_ERESTARTSYS;
				goto out;
			}
		}
		else
		{
			ret_wait = wait_event_interruptible(fr->serial_update, buf->serial >= next_serial);
			if(ret_wait) {
				fr_log(info, "get_ref 2 interrupted, %d->%d\n",
							buf->serial, next_serial);
				ret = FR_ERESTARTSYS;
				goto out;
			}
		}
#else
		while((volatile int)buf->serial != next_serial)
			schedule();
#endif
		fr_log(debug, "(done waiting)\n");
	}

check_buf:
	if(buf) {
		if((ret = fr_buf_set_state(fr, buf, FR_BUFST_READING)) < 0)
			goto out;
		buf->ref_serial = buf->serial;
		fr_poke_timepin(fr_get_ref_st(fr), FRTIME_ST_DEAL);
		*ref = buf;
	}
out:
	fr_peek_timepin(fr_get_ref_st(fr), FRTIME_ST_WAIT);
	return ret;
}
EXPORT_SYMBOL(fr_get_ref);

int fr_put_ref(struct fr *fr, struct fr_buf *buf)
{
	fr_log(debug, "%p\n", buf);
	if(fr_buf_set_state(fr, buf, FR_BUFST_READ) < 0)
		BUG();
	fr_peek_timepin(fr_get_ref_st(fr), FRTIME_ST_DEAL);
	fr_peek_timepin(fr_get_ref_st(fr), FRTIME_ST_PUT);
	fr_poke_timepin(fr_get_ref_st(fr), FRTIME_ST_PUT);
	return 0;
}
EXPORT_SYMBOL(fr_put_ref);

void fr_set_timeout(struct fr *fr, int timeout_in_ms)
{
	if (timeout_in_ms <= 0)
	{
		fr_log(error, "timeout value error: %d\n", timeout_in_ms);
		return;
	}
	fr->timeout = msecs_to_jiffies(timeout_in_ms);
}
EXPORT_SYMBOL(fr_set_timeout);


#ifdef CONFIG_INFOTM_FR_STATISTICS
#include "fr-procfs.c"
#endif

