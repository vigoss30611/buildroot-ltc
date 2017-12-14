/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/05/04: first commit.
**
*****************************************************************************/ 
#ifndef __UTLZ_PWL_H__
#define __UTLZ_PWL_H__

/*we must define WORK_QUEUE in Linux3.10,
 * timer can not be used 
 * */
#define WORK_QUEUE
IM_RET utlzpwl_init(void);
IM_RET utlzpwl_deinit(void);

void *utlzpwl_malloc(IM_UINT32 size);
void utlzpwl_free(void *p);
void utlzpwl_memcpy(void *dst, void *src, IM_UINT32 size);
void utlzpwl_memset(void *dst, IM_INT8 c, IM_UINT32 size);

IM_INT32 utlzpwl_strcmp(IM_TCHAR *dst, IM_TCHAR *src);

typedef struct _utlzpwl_lock
{
	void *lock;
	unsigned long flags;
}utlzpwl_lock_t;

IM_RET utlzpwl_lock_init(utlzpwl_lock_t *lck);
IM_RET utlzpwl_lock_deinit(utlzpwl_lock_t *lck);
IM_RET utlzpwl_lock(utlzpwl_lock_t *lck);
IM_RET utlzpwl_unlock(utlzpwl_lock_t *lck);

typedef void * utlzpwl_thread_t;
typedef void (*utlzpwl_func_thread_entry_t)(void *data);
IM_RET utlzpwl_thread_init(utlzpwl_thread_t *thread, utlzpwl_func_thread_entry_t func, void *p);
IM_RET utlzpwl_thread_deinit(utlzpwl_thread_t thread);

IM_INT64 utlzpwl_get_time(void);
IM_UINT32 utlzpwl_mstoticks(IM_UINT32 ms);
void utlzpwl_sleep(IM_INT32 ms);

typedef void * utlzpwl_timer_t;
typedef void (*utlzpwl_timer_callback_t)(void * arg );
void utlzpwl_timer_add(utlzpwl_timer_t tim, IM_UINT32 ticks_to_expire);
void utlzpwl_timer_del(utlzpwl_timer_t tim);
void utlzpwl_timer_term(utlzpwl_timer_t tim);
void utlzpwl_timer_setcallback(utlzpwl_timer_t tim, utlzpwl_timer_callback_t callback, void *data);
IM_RET utlzpwl_timer_init(utlzpwl_timer_t *tim);

IM_UINT32 utlzpwl_clz(IM_UINT32 input);

typedef void *utlzpwl_workqueue_t;
typedef void *utlzpwl_work_t;
typedef void (*utlzpwl_work_handle_t)(void *data); 

/* init a workqueue */
IM_RET utlzpwl_workqueue_init(utlzpwl_workqueue_t *workqueue);

/* deinit the workqueue */
void  utlzpwl_workqueue_deinit(utlzpwl_workqueue_t workqueue);

/* create a work */
utlzpwl_work_t utlzpwl_create_work(utlzpwl_work_handle_t handler, void *data);

/* queue a work to workqueue, delay is jiffies */
void utlzpwl_queue_work(utlzpwl_workqueue_t workqueue, utlzpwl_work_t work, u32 delay);

/* cancel work from workqueue, if sync is true, wait until work is canceled */
void utlzpwl_cancel_work(utlzpwl_work_t work, bool sync);

/* delete the work */
void utlzpwl_delete_work(utlzpwl_work_t work);

#endif	// __UTLZ_PWL_H__

