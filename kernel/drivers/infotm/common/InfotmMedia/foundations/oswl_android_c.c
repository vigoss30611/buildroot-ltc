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
** v1.0.1	leo@2012/03/15: first commit.
** v1.4.0	leo@2012/08/13: when pass a invalid prio in setPriority(), return
**			IM_RET_FAILED;
** v2.0.2	leo@2012/11/14: fixed im_oswl_signal_wait() cannot timeout bug, 
**				the main reason is timeout<<4 caused.
**
*****************************************************************************/ 

#include <InfotmMedia.h>

IM_RET im_oswl_lock_init(im_lock_t *lck)
{
	pthread_mutex_t *mutex;

	//
	if(lck == IM_NULL){
		return IM_RET_INVALID_PARAMETER;
	}

	//
	mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if(mutex == IM_NULL){
		return IM_RET_FAILED;
	}
	memset((void *)mutex, 0x0, sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, IM_NULL);

	*lck = (im_lock_t)mutex;
	return IM_RET_OK;
}

IM_RET im_oswl_lock_deinit(im_lock_t lck)
{
	pthread_mutex_t *mutex = (pthread_mutex_t *)lck;
	if(mutex == IM_NULL){
		return IM_RET_INVALID_PARAMETER;
	}
	pthread_mutex_destroy(mutex);
	free((void *)mutex);
	return IM_RET_OK;
}

IM_RET im_oswl_lock_lock(im_lock_t lck)
{
	pthread_mutex_t *mutex = (pthread_mutex_t *)lck;
	if(mutex == IM_NULL){
		return IM_RET_INVALID_PARAMETER;
	}
	if(pthread_mutex_lock(mutex) == 0){
		return IM_RET_OK;
	}else{
		return IM_RET_FAILED;
	}
}

IM_RET im_oswl_lock_unlock(im_lock_t lck)
{
	pthread_mutex_t *mutex = (pthread_mutex_t *)lck;
	if(mutex == IM_NULL){
		return IM_RET_INVALID_PARAMETER;
	}
	if(pthread_mutex_unlock(mutex) == 0){
		return IM_RET_OK;
	}else{
		return IM_RET_FAILED;
	}
}

static pthread_mutex_t *get_lock(im_lock_t lck)
{
	return (pthread_mutex_t *)lck;
}


typedef struct{
	pthread_cond_t	cond;
	pthread_mutex_t	mutex;
	IM_INT32 	mark;
	IM_BOOL 	manualReset;
}im_signal_internal_t;

IM_RET im_oswl_signal_init(im_signal_t *sig, IM_BOOL manualReset)
{
	im_signal_internal_t *signal;

	if(sig == IM_NULL){
		return IM_RET_INVALID_PARAMETER;
	}	

	signal = (im_signal_internal_t *)malloc(sizeof(im_signal_internal_t));
	if(signal == IM_NULL){
		return IM_RET_FAILED;
	}

	if(pthread_cond_init(&signal->cond, IM_NULL) != 0){
		goto Fail;
	}
	if(pthread_mutex_init(&signal->mutex, IM_NULL) != 0){
		goto Fail;
	}
	signal->mark = 0;
	signal->manualReset = manualReset;

	*sig = (im_signal_t)signal;
	return IM_RET_OK;
Fail:
	im_oswl_signal_deinit(sig);
	return IM_RET_FAILED;
}

IM_RET im_oswl_signal_deinit(im_signal_t sig)
{
	im_signal_internal_t *signal = (im_signal_internal_t *)sig;
	if(signal == IM_NULL){
		return IM_RET_FAILED;
	}

	pthread_mutex_destroy(&signal->mutex);
	pthread_cond_destroy(&signal->cond);
	free((void *)signal);
	return IM_RET_OK;
}

IM_RET im_oswl_signal_set(im_signal_t sig)
{
	im_signal_internal_t *signal = (im_signal_internal_t *)sig;
	if(signal == IM_NULL){
		return IM_RET_FAILED;
	}

	pthread_mutex_lock(&signal->mutex);
	signal->mark = 1;
	if(pthread_cond_signal(&signal->cond) == 0){
		pthread_mutex_unlock(&signal->mutex);
		return IM_RET_OK;
	}else{
		pthread_mutex_unlock(&signal->mutex);
		return IM_RET_FAILED;
	}
}

IM_RET im_oswl_signal_reset(im_signal_t sig)
{
	im_signal_internal_t *signal = (im_signal_internal_t *)sig;
	if(signal == IM_NULL){
		return IM_RET_FAILED;
	}

	pthread_mutex_lock(&signal->mutex);
	signal->mark = 0;
	pthread_mutex_unlock(&signal->mutex);
	return IM_RET_OK;
}

IM_RET im_oswl_signal_wait(im_signal_t sig, im_lock_t lck, IM_INT32 timeout)
{
	IM_INT32 err = 0;
	IM_BOOL usrmark = IM_FALSE;
	pthread_mutex_t *actlock = IM_NULL;	/* Actual lock. */
	IM_RET ret = IM_RET_OK;

	im_signal_internal_t *signal = (im_signal_internal_t *)sig;
	if(signal == IM_NULL){
		return IM_RET_FAILED;
	}

	if (lck != IM_NULL) {
		actlock = get_lock(lck);
		usrmark = IM_TRUE;
	} else {
		actlock = &signal->mutex;
	}
	/* Check *actlock* validation. */
	if (actlock == IM_NULL) {
		return IM_RET_FAILED;
	}

	/* 
	 * If the function caller has lock a mutex lock to protect some private data, 
	 * we should not lock the internal lock inside *im_signal_t*, else some dead
	 * lock problems will appear. 
	 */
	if (!usrmark) {
		pthread_mutex_lock(actlock);
	}

	if (timeout < 0) {
		if (signal->mark > 0) {
			/* Do nothing. */
		} else {
			/* Wait till any set. */
			err = pthread_cond_wait(&signal->cond, actlock);
			if (err != 0) {
				ret = IM_RET_FAILED;
				goto signal_wait_return;
			}
		}
	} else if (timeout == 0) {
		if (signal->mark > 0) {
			/* Do nothing. */
		} else {
			ret = IM_RET_TIMEOUT;
			goto signal_wait_return;
		}
	} else {
		if (signal->mark > 0) {
			/* Do nothing. */
		} else {
			struct timespec ts;
			
			struct timeval t;
			gettimeofday(&t, NULL);

			ts.tv_sec = t.tv_sec + (timeout / 1000);
			ts.tv_nsec= (t.tv_usec * 1000) + ((timeout % 1000) * 1000000);
			if(ts.tv_nsec >= 1000000000){
				ts.tv_nsec -= 1000000000;
				ts.tv_sec += 1;
			}
			err = pthread_cond_timedwait(&signal->cond, actlock, &ts);
			switch (err) {
				case 0:
					/* Normal. */
					break;

				case ETIMEDOUT:
					ret = IM_RET_TIMEOUT;
					goto signal_wait_return;

				case EINVAL:
				case EPERM:
				default:
					ret = IM_RET_FAILED;
					goto signal_wait_return;
			}

		}
	}

	/* Check manual reset flags to decide whether to reset condition mark. */
	if (signal->manualReset) {
		/* Do nothing and caller must reset signal by himself. */
	} else {
		signal->mark = 0;
	}

signal_wait_return:
	if (!usrmark) {
		pthread_mutex_unlock(actlock);
	}

	return ret;
}


typedef struct{
	pthread_t	thread;
	IM_INT32	prio;
}im_thread_internal_t;

IM_RET im_oswl_thread_init(im_thread_t *thrd, THREAD_ENTRY entry, void *p)
{
	im_thread_internal_t *inst;
	if(thrd == IM_NULL){
		return IM_RET_INVALID_PARAMETER;
	}

	//
	inst = (im_thread_internal_t *)malloc(sizeof(im_thread_internal_t));	
	if(inst == IM_NULL){
		return IM_RET_FAILED;
	}
	memset((void *)&inst->thread, 0x0, sizeof(pthread_t));
	
	if(pthread_create(&inst->thread, IM_NULL, entry, p) != 0){
		free((void *)inst);
		return IM_RET_FAILED;
	}

	*thrd = (im_thread_t)inst;
	return IM_RET_OK;
}

IM_RET im_oswl_thread_terminate(im_thread_t thrd)
{
	im_thread_internal_t *inst = (im_thread_internal_t *)thrd;
	if(inst == IM_NULL){
		return IM_RET_FAILED;
	}
	
	pthread_join(inst->thread, IM_NULL);
	free((void *)inst);
	return IM_RET_OK;
}

IM_RET im_oswl_thread_set_priority(im_thread_t thrd, IM_INT32 prio, IM_INT32 *oldPrio)
{
	im_thread_internal_t *inst = (im_thread_internal_t *)thrd;
	if(inst == IM_NULL){
		return IM_RET_FAILED;
	}

	if(prio == IM_THREAD_PRIO_HIGHEST){
		prio = -20;
	}else if(prio == IM_THREAD_PRIO_HIGHER){
		prio = -16;
	}else if(prio == IM_THREAD_PRIO_NORMAL){
		prio = 0;
	}else if(prio == IM_THREAD_PRIO_LOWER){
		prio = 2;
	}else if(prio == IM_THREAD_PRIO_LOWEST){
		prio = 10;
	}else{
		return IM_RET_FAILED;
	}
	
/*	if(setPriority(prio) == -1){
		return IM_RET_FAILED;
	}*/

	inst->prio = prio;
	if(oldPrio != IM_NULL){
		*oldPrio = prio;
	}

	return IM_RET_OK;
}


void im_oswl_sleep(IM_INT32 ms){
	usleep(ms * 1000);
}

IM_TCHAR * im_oswl_strcpy(IM_TCHAR *dst, IM_TCHAR *src){
	return strcpy(dst, src);
}

IM_TCHAR * im_oswl_strncpy(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return strncpy(dst, src, n);
}

IM_INT32 im_oswl_strlen(IM_TCHAR *str){
	return (IM_INT32)strlen(str);
}

IM_TCHAR * im_oswl_strcat(IM_TCHAR *dst, IM_TCHAR *src){
	return strcat(dst, src);
}

IM_INT32 im_oswl_strcmp(IM_TCHAR *dst, IM_TCHAR *src){
	return (IM_INT32)strcmp(dst, src);
}

IM_INT32 im_oswl_strncmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return (IM_INT32)strncmp(dst, src, n);
}

IM_INT32 im_oswl_strcasecmp(IM_TCHAR *dst, IM_TCHAR *src){
	return (IM_INT32)strcasecmp(dst, src);
}

IM_INT32 im_oswl_strncasecmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return (IM_INT32)strncasecmp(dst, src, n);
}

IM_TCHAR * im_oswl_strchr(IM_TCHAR *s, IM_INT32 c){
	return strchr(s, c);
}

IM_TCHAR * im_oswl_strrchr(IM_TCHAR *s, IM_INT32 c){
	return strrchr(s, c);
}


IM_INT32 im_oswl_CharString2TCharString(IM_TCHAR *dst, char *src){
	strcpy(dst, src);
	return (IM_INT32)strlen(dst);
}

IM_INT32 im_oswl_TCharString2CharString(char *dst, IM_INT32 dstSize, IM_TCHAR *src){
	strcpy(dst, src);
	return (IM_INT32)strlen(dst);
}

IM_INT32 im_oswl_GetSystemTimeMs(){
	struct timeval tv;
	gettimeofday(&tv, IM_NULL);
	return (IM_INT32)((IM_INT64)tv.tv_usec / 1000LL) + (IM_INT32)(tv.tv_sec * 1000);
}

