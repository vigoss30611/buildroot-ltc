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
** v2.0.2	leo@2012/11/14: fixed IM_Signal->wait() cannot timeout bug, the main
**				reason is timeout<<4 caused.
**
*****************************************************************************/ 

#include <InfotmMedia.h>


IM_Lock::IM_Lock(){
	memset(&mMutex, 0x00, sizeof(pthread_mutex_t));
	pthread_mutex_init(&mMutex, IM_NULL);
}

IM_Lock::~IM_Lock(){
	pthread_mutex_destroy(&mMutex);
}

IM_RET IM_Lock::lock(){
	IM_INT32 ret = pthread_mutex_lock(&mMutex);
	if(ret == 0)
		return IM_RET_OK;
	else
		return IM_RET_FAILED;
}

IM_RET IM_Lock::unlock(){
	IM_INT32 ret = pthread_mutex_unlock(&mMutex);
	if(ret == 0)
		return IM_RET_OK;
	else
		return IM_RET_FAILED;
}

IM_Signal::IM_Signal(IM_BOOL manualReset){
	IM_INT32 ret;
	ret = pthread_cond_init(&mCond, IM_NULL);
	IM_ASSERT(ret == 0);
	ret = pthread_mutex_init(&mMutex, IM_NULL);
	IM_ASSERT(ret == 0);
	mMark = 0;
	mManualReset = manualReset;
}

IM_Signal::~IM_Signal(){
	pthread_mutex_destroy(&mMutex);
	pthread_cond_destroy(&mCond);
}

IM_RET IM_Signal::set(){
	pthread_mutex_lock(&mMutex);
	mMark = 1;
	IM_INT32 ret = pthread_cond_signal(&mCond);
	pthread_mutex_unlock(&mMutex);
	if(ret == 0){
		return IM_RET_OK;
	}else{
		return IM_RET_FAILED;
	}
}

IM_RET IM_Signal::reset(){
	pthread_mutex_lock(&mMutex);
	mMark = 0;
	pthread_mutex_unlock(&mMutex);
	return IM_RET_OK;
}

IM_RET IM_Signal::wait(IM_Lock *lck, IM_INT32 timeout){
	IM_INT32 err = 0;
	IM_BOOL usrmark = IM_FALSE;
	pthread_mutex_t *actlock = IM_NULL;	/* Actual lock. */
	IM_RET ret = IM_RET_OK;

	if (lck != IM_NULL) {
		actlock = lck->getlock();
		usrmark = IM_TRUE;
	} else {
		actlock = &mMutex;
	}
	/* Check *actlock* validation. */
	if (actlock == IM_NULL) {
		return IM_RET_FAILED;
	}

	/* 
	 * If the function caller has lock a mutex lock to protect some private data, 
	 * we should not lock the internal lock inside *IM_Signal*, else some dead
	 * lock problems will appear. 
	 */
	if (!usrmark) {
		pthread_mutex_lock(actlock);
	}

	if (timeout < 0) {
		if (mMark > 0) {
			/* Do nothing. */
		} else {
			/* Wait till any set. */
			err = pthread_cond_wait(&mCond, actlock);
			if (err != 0) {
				ret = IM_RET_FAILED;
				goto signal_wait_return;
			}
		}
	} else if (timeout == 0) {
		if (mMark > 0) {
			/* Do nothing. */
		} else {
			ret = IM_RET_TIMEOUT;
			goto signal_wait_return;
		}
	} else {
		if (mMark > 0) {
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
			err = pthread_cond_timedwait(&mCond, actlock, &ts);
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
	if (mManualReset) {
		/* Do nothing and caller must reset signal by himself. */
	} else {
		mMark = 0;
	}

signal_wait_return:
	if (!usrmark) {
		pthread_mutex_unlock(actlock);
	}

	return ret;
}

IM_Thread::IM_Thread(){
	memset(&mThread, 0x00, sizeof(pthread_t));
}

IM_Thread::~IM_Thread(){
	pthread_join(mThread, IM_NULL);
}

IM_RET IM_Thread::create(THREAD_ENTRY entry, void *param){
	if(pthread_create(&mThread, IM_NULL, entry, param) == 0){
		return IM_RET_OK;
	}else{
		return IM_RET_FAILED;
	}
}

IM_RET IM_Thread::forceTerminate(){
	//ANDROID  NOT SUPPORT  KILL THREAD
	//SO, HERE DO NOTHING 
	//if(pthread_kill(mThread, SIGKILL) == 0){
		return IM_RET_OK;
	//}else{
	//	return IM_RET_FAILED;
	//}
}

IM_RET IM_Thread::setPriority(IM_INT32 prio, IM_INT32 *oldPrio){
    if(mPrio == prio){
        if(oldPrio != IM_NULL){
            *oldPrio = mPrio;
        }
        return IM_RET_OK;
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

	mPrio = prio;
    if(oldPrio != IM_NULL){
        *oldPrio = mPrio;
    }
    
	return IM_RET_OK;
}

IM_String::IM_String(IM_TCHAR *str){
	mString = new IM_TCHAR[strlen(str)+2];
	IM_ASSERT(mString != IM_NULL);
	strcpy(mString, str);
}

IM_String::~IM_String(){
	if(mString != IM_NULL){
		delete []mString;
	}
}

IM_INT32 IM_String::length(){
	return strlen(mString);
}

IM_INT32 IM_String::size(){
	return length() * sizeof(IM_TCHAR);
}

IM_TCHAR * IM_String::copyFrom(IM_TCHAR *src){
	return strcpy(mString, src);
}

IM_TCHAR * IM_String::copyTo(IM_TCHAR *dst){
	return strcpy(dst, mString);
}

void IM_Sleep(IM_INT32 ms){
	usleep(ms * 1000);
}

IM_TCHAR * IM_Strcpy(IM_TCHAR *dst, IM_TCHAR *src){
	return strcpy(dst, src);
}

IM_TCHAR * IM_Strncpy(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return strncpy(dst, src, n);
}

IM_INT32 IM_Strlen(IM_TCHAR *str){
	return (IM_INT32)strlen(str);
}

IM_TCHAR * IM_Strcat(IM_TCHAR *dst, IM_TCHAR *src){
	return strcat(dst, src);
}

IM_INT32 IM_Strcmp(IM_TCHAR *dst, IM_TCHAR *src){
	return (IM_INT32)strcmp(dst, src);
}

IM_INT32 IM_Strncmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return (IM_INT32)strncmp(dst, src, n);
}

IM_INT32 IM_Strcasecmp(IM_TCHAR *dst, IM_TCHAR *src){
	return (IM_INT32)strcasecmp(dst, src);
}

IM_INT32 IM_Strncasecmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return (IM_INT32)strncasecmp(dst, src, n);
}

IM_TCHAR * IM_Strchr(IM_TCHAR *s, IM_INT32 c){
	return strchr(s, c);
}

IM_TCHAR * IM_Strrchr(IM_TCHAR *s, IM_INT32 c){
	return strrchr(s, c);
}


IM_INT32 IM_CharString2TCharString(IM_TCHAR *dst, char *src){
	strcpy(dst, src);
	return (IM_INT32)strlen(dst);
}

IM_INT32 IM_TCharString2CharString(char *dst, IM_INT32 dstSize, IM_TCHAR *src){
	strcpy(dst, src);
	return (IM_INT32)strlen(dst);
}

IM_INT32 IM_GetSystemTimeMs(){
	struct timeval tv;
	gettimeofday(&tv, IM_NULL);
	return (IM_INT32)((IM_INT64)tv.tv_usec / 1000LL) + (IM_INT32)(tv.tv_sec * 1000);
}


#include "oswl_android_c.c"



