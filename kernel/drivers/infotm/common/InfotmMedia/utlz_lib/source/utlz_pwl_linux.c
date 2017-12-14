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

#include <linux/semaphore.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/bitops.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <InfotmMedia.h>
#include <IM_utlzapi.h>
#include <utlz_lib.h>
#include <utlz_pwl.h>


#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"UTLZPWL_I:"
#define WARNHEAD	"UTLZPWL_W:"
#define ERRHEAD		"UTLZPWL_E:"
#define TIPHEAD		"UTLZPWL_T:"


IM_RET utlzpwl_init(void)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_OK;
}

IM_RET utlzpwl_deinit(void)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_OK;
}

void *utlzpwl_malloc(IM_UINT32 size)
{
	return kmalloc(size, GFP_KERNEL);
}

void utlzpwl_free(void *p)
{
	if(p != IM_NULL){
		kfree(p);
	}
}

void utlzpwl_memcpy(void *dst, void *src, IM_UINT32 size)
{
	memcpy(dst, src, size);
}

void utlzpwl_memset(void *dst, IM_INT8 c, IM_UINT32 size)
{
	memset(dst, c, size);
}

IM_INT32 utlzpwl_strcmp(IM_TCHAR *dst, IM_TCHAR *src)
{
    return strcmp(dst, src);
}

#if 0
IM_RET utlzpwl_lock_init(utlzpwl_lock_t *lck)
{
	struct mutex *mtx = (struct mutex *)utlzpwl_malloc(sizeof(struct mutex));
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(mtx != IM_NULL);
	mutex_init(mtx);
	*lck = (utlzpwl_lock_t)mtx;
	return IM_RET_OK;
}

IM_RET utlzpwl_lock_deinit(utlzpwl_lock_t lck)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(lck != IM_NULL);
	mutex_destroy((struct mutex *)lck);
	utlzpwl_free((void *)lck);
	return IM_RET_OK;
}

IM_RET utlzpwl_lock(utlzpwl_lock_t lck)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(lck != IM_NULL);
	mutex_lock((struct mutex *)lck);
	return IM_RET_OK;
}

IM_RET utlzpwl_unlock(utlzpwl_lock_t lck)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(lck != IM_NULL);
	mutex_unlock((struct mutex *)lck);
	return IM_RET_OK;
}
#endif
IM_RET utlzpwl_lock_init(utlzpwl_lock_t *lck)
{
    spinlock_t *spinlock = (spinlock_t *)utlzpwl_malloc(sizeof(spinlock_t));
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(spinlock != IM_NULL);
	spin_lock_init(spinlock);
	lck->lock = (void *)spinlock;
	return IM_RET_OK;
}

IM_RET utlzpwl_lock_deinit(utlzpwl_lock_t *lck)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(lck != IM_NULL);
	if(lck->lock)
	{
		utlzpwl_free(lck->lock);
	}
	return IM_RET_OK;
}

IM_RET utlzpwl_lock(utlzpwl_lock_t *lck)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(lck != IM_NULL);
	spin_lock_irqsave((spinlock_t *)lck->lock, lck->flags);
	return IM_RET_OK;
}

IM_RET utlzpwl_unlock(utlzpwl_lock_t *lck)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(lck != IM_NULL);
	spin_unlock_irqrestore((spinlock_t *)lck->lock, lck->flags);
	return IM_RET_OK;
}

typedef struct {
	struct work_struct work;
	struct workqueue_struct *wq;
	void *data;
	utlzpwl_func_thread_entry_t func;
}thread_t;

static void work_handle(struct work_struct *p)
{
	thread_t *thrd = container_of(p, thread_t, work);
	thrd->func(thrd->data);
}

IM_RET utlzpwl_thread_init(utlzpwl_thread_t *thread, utlzpwl_func_thread_entry_t func, void *data)
{
	int ret = 1;
	thread_t *thrd = IM_NULL;

	IM_INFOMSG((IM_STR("%s()++"), IM_STR(_IM_FUNC_)));
	thrd = (thread_t *)utlzpwl_malloc(sizeof(thread_t));
	if(thrd == IM_NULL){
		IM_ERRMSG((IM_STR("@utlzpwl_thread_init(), utlzpwl_malloc(thread_t) failed!")));
		return IM_RET_FAILED;
	}

	thrd->wq = create_singlethread_workqueue("utlzpwl_wq");
	if(thrd->wq == NULL){
		IM_ERRMSG((IM_STR("@utlzpwl_thread_init(), create_singlethread_workqueue() failed!")));
		utlzpwl_free((void *)thrd);
		return IM_RET_FAILED;
	}

	thrd->data = data;
	thrd->func = func;
	INIT_WORK(&thrd->work, work_handle);

	//INIT_WORK(&thrd->work, func);
	ret = queue_work(thrd->wq, &thrd->work);
	if(ret != 1){
		destroy_workqueue(thrd->wq);
		utlzpwl_free((void *)thrd);
		return IM_RET_FAILED;
	}
	
	*thread = (utlzpwl_thread_t)thrd;
	IM_INFOMSG((IM_STR("%s()--"), IM_STR(_IM_FUNC_)));
	return IM_RET_OK;
}

IM_RET utlzpwl_thread_deinit(utlzpwl_thread_t thread)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(thread != IM_NULL);
	destroy_workqueue(((thread_t *)thread)->wq);
	utlzpwl_free((void *)thread);
	return IM_RET_OK;
}


IM_INT64 utlzpwl_get_time(void)
{
	struct timespec t;
	getnstimeofday(&t);
	return (IM_INT64)timespec_to_ns(&t);
}

IM_UINT32 utlzpwl_clz(IM_UINT32 input)
{
	return 32-fls(input);
}

void utlzpwl_sleep(IM_INT32 ms)
{
	msleep(ms);
}

typedef struct 
{
	struct timer_list timer;
}utlzpwl_timer_t_struct;

typedef void (*timer_timeout_function_t)(IM_UINT32);

IM_UINT32 utlzpwl_mstoticks(IM_UINT32 ms)
{
	return msecs_to_jiffies(ms);
}

void utlzpwl_timer_add(utlzpwl_timer_t tim, IM_UINT32 ticks_to_expire)
{
	utlzpwl_timer_t_struct *t = (utlzpwl_timer_t_struct *)tim;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(tim != IM_NULL);

	t->timer.expires = jiffies + ticks_to_expire;
	add_timer(&(t->timer));
}

void utlzpwl_timer_del(utlzpwl_timer_t tim)
{
	utlzpwl_timer_t_struct *t = (utlzpwl_timer_t_struct *)tim;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(tim != IM_NULL);

	del_timer_sync(&(t->timer));
}

void utlzpwl_timer_term(utlzpwl_timer_t tim)
{
	utlzpwl_timer_t_struct *t = (utlzpwl_timer_t_struct *)tim;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(tim != IM_NULL);

	utlzpwl_free((void *)t);
}

void utlzpwl_timer_setcallback(utlzpwl_timer_t tim, utlzpwl_timer_callback_t callback, void *data)
{
typedef void(*timer_function_t)(unsigned long);

	utlzpwl_timer_t_struct *t = (utlzpwl_timer_t_struct *)tim;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(tim != IM_NULL);

	t->timer.data = (IM_UINT32)data;
	t->timer.function = (timer_function_t)callback;
}
	
IM_RET utlzpwl_timer_init(utlzpwl_timer_t *tim)
{
	utlzpwl_timer_t_struct *t = (utlzpwl_timer_t_struct*)utlzpwl_malloc(sizeof(utlzpwl_timer_t_struct));
	if(t == IM_NULL){
		IM_ERRMSG((IM_STR("@utlzpwl_timer_init(), utlzpwl_malloc(utlzpwl_timer_t) failed!")));
		return IM_RET_FAILED;
	}
	else
	{
		init_timer(&t->timer);
		*tim = (utlzpwl_timer_t)t;
		return IM_RET_OK;
	}
}

/* linux workqueue */
struct utlzpwl_workqueue_t {
    struct workqueue_struct *workqueue;
};

struct utlzpwl_work_t {
    utlzpwl_work_handle_t work_handle;
    void *data;
    struct delayed_work work;
};

#define UTLZ_CONTAINER_OF(ptr, type, member) \
                       ((type *)( ((char *)ptr) - offsetof(type,member) ))

static void utlzpwl_delayed_work_func(struct work_struct *work)
{
	struct utlzpwl_work_t *utlz_work = container_of(work, struct utlzpwl_work_t, work.work);
	utlz_work->work_handle(utlz_work->data);
}

IM_RET utlzpwl_workqueue_init(utlzpwl_workqueue_t *workqueue)
{
    struct utlzpwl_workqueue_t *_workqueue = (struct utlzpwl_workqueue_t *)utlzpwl_malloc(sizeof(struct utlzpwl_workqueue_t));
    if(_workqueue == NULL) {
        IM_ERRMSG((IM_STR("@utlzpwl_workqueue_init(), utlzpwl_workqueue_t malloc failed!")));
        return IM_RET_FAILED;
    }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
    _workqueue->workqueue = alloc_workqueue("utlzpwl", WQ_UNBOUND, 0);
#else
    _workqueue->workqueue = create_workqueue("utlzpwl");
#endif
    if(_workqueue->workqueue == NULL) {
        IM_ERRMSG((IM_STR("@utlzpwl_workqueue_init(), alloc_workqueue failed!")));
        utlzpwl_free(_workqueue);
        return IM_RET_FAILED;
    }

    *workqueue = _workqueue;

    return IM_RET_OK;
}

utlzpwl_work_t utlzpwl_create_work(utlzpwl_work_handle_t handler, void *data)
{
    struct utlzpwl_work_t *_work = (struct utlzpwl_work_t *)utlzpwl_malloc(sizeof(struct utlzpwl_work_t));
    if(_work == NULL) {
        IM_ERRMSG((IM_STR("@utlzpwl_create_work(), utlzpwl_work_t malloc failed!")));
        return NULL;
    }

    _work->work_handle = handler;
    _work->data = data;
    INIT_DELAYED_WORK(&_work->work, utlzpwl_delayed_work_func);

    return _work;
}

void utlzpwl_delete_work(utlzpwl_work_t work)
{
    struct utlzpwl_work_t *_work = (struct utlzpwl_work_t *)work;

    utlzpwl_free(_work);
    return;
}

void utlzpwl_queue_work(utlzpwl_workqueue_t workqueue, utlzpwl_work_t work, u32 delay)
{
    struct utlzpwl_workqueue_t *_workqueue = (struct utlzpwl_workqueue_t *)workqueue;
    struct utlzpwl_work_t *_work = (struct utlzpwl_work_t *)work;
    queue_delayed_work(_workqueue->workqueue, &_work->work, delay);
    return;
}

void utlzpwl_cancel_work(utlzpwl_work_t work, bool sync)
{
    struct utlzpwl_work_t *_work = (struct utlzpwl_work_t *)work;
    if(sync)
        cancel_delayed_work_sync(&_work->work);
    else
        cancel_delayed_work(&_work->work);
}

void utlzpwl_workqueue_deinit(utlzpwl_workqueue_t workqueue)
{
    struct utlzpwl_workqueue_t *_workqueue = (struct utlzpwl_workqueue_t *)workqueue;

    flush_workqueue(_workqueue->workqueue);
    destroy_workqueue(_workqueue->workqueue);

    utlzpwl_free(_workqueue);
    return;
}
