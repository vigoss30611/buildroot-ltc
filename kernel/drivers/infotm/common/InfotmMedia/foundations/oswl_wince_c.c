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
** v1.4.0	leo@2012/08/13: remove im_oswl_strncmp and im_oswl_strncasecmp; rewrite
**			im_oswl_strchr, im_oswl_strrchr and im_oswl_strncpy.
**
*****************************************************************************/ 

#include <InfotmMedia.h>

IM_RET im_oswl_lock_init(im_lock_t *lck)
{
	CRITICAL_SECTION *cs;
	if(lck == IM_NULL){
		return IM_RET_INVALID_PARAMETER;
	}
	
	cs = (CRITICAL_SECTION *)malloc(sizeof(CRITICAL_SECTION));
	if(cs == IM_NULL){
		return IM_RET_FAILED;
	}

	InitializeCriticalSection(cs);
	*lck = (im_lock_t)cs;
	return IM_RET_OK;
}

IM_RET im_oswl_lock_deinit(im_lock_t lck)
{
	CRITICAL_SECTION *cs = lck;
	if(cs == IM_NULL){
		return IM_RET_FAILED;
	}
	DeleteCriticalSection(cs);
	return IM_RET_OK;
}

IM_RET im_oswl_lock_lock(im_lock_t lck)
{
	CRITICAL_SECTION *cs = lck;
	if(cs == IM_NULL){
		return IM_RET_FAILED;
	}
	EnterCriticalSection(cs);
	return IM_RET_OK;
}

IM_RET im_oswl_lock_unlock(im_lock_t lck)
{
	CRITICAL_SECTION *cs = lck;
	if(cs == IM_NULL){
		return IM_RET_FAILED;
	}
	LeaveCriticalSection(cs);
	return IM_RET_OK;
}

IM_RET im_oswl_signal_init(im_signal_t *sig, IM_BOOL manualReset)
{
	HANDLE event;

	if(sig == IM_NULL){
		return IM_RET_FAILED;
	}

	event = CreateEvent(IM_NULL, manualReset, IM_FALSE, IM_NULL);
	if(event == IM_NULL){
		return IM_RET_FAILED;
	}

	*sig = (im_signal_t)event;
	return IM_RET_OK;
}

IM_RET im_oswl_signal_deinit(im_signal_t sig)
{
	HANDLE event = (HANDLE)sig;
	if(event == IM_NULL){
		return IM_RET_FAILED;
	}

	CloseHandle(event);
	event = IM_NULL;
	return IM_RET_OK;
}

IM_RET im_oswl_signal_set(im_signal_t sig)
{
	HANDLE event = (HANDLE)sig;
	if(event == IM_NULL){
		return IM_RET_FAILED;
	}

	if(SetEvent(event) == 0){
		return IM_RET_FAILED;
	}else{
		return IM_RET_OK;
	}
}

IM_RET im_oswl_signal_reset(im_signal_t sig)
{
	HANDLE event = (HANDLE)sig;
	if(event == IM_NULL){
		return IM_RET_FAILED;
	}

	if(ResetEvent(event) == 0){
		return IM_RET_FAILED;
	}else{
		return IM_RET_OK;
	}
}

IM_RET im_oswl_signal_wait(im_signal_t sig, im_lock_t lck, IM_INT32 timeout)
{
	IM_RET ret;
	DWORD wt;
	HANDLE event = (HANDLE)sig;
	if(event == IM_NULL){
		return IM_RET_FAILED;
	}

	if(lck != IM_NULL){
		im_oswl_lock_unlock(lck);
	}
	
	wt = WaitForSingleObject(event, (timeout==-1)?INFINITE:timeout);
	if(wt == WAIT_OBJECT_0){
		ret = IM_RET_OK;
	}else if(wt == WAIT_TIMEOUT){
		ret = IM_RET_TIMEOUT;
	}else{
		ret = IM_RET_FAILED;
	}
	
	if(lck != IM_NULL){
		im_oswl_lock_lock(lck);
	}

	return ret;
}


typedef struct{
	HANDLE  thread;
	IM_INT32 prio;
}im_thread_internal_t;

IM_RET im_oswl_thread_init(im_thread_t *thrd, THREAD_ENTRY entry, void *p)
{
	im_thread_internal_t *inst;
	if(thrd == IM_NULL){
		return IM_RET_INVALID_PARAMETER;
	}

	inst = (im_thread_internal_t *)malloc(sizeof(im_thread_internal_t));
	if(inst == IM_NULL){
		return IM_RET_FAILED;
	}
	inst->prio = IM_THREAD_PRIO_NORMAL;
	inst->thread = CreateThread(IM_NULL, 0, (LPTHREAD_START_ROUTINE)entry, p, 0, IM_NULL);
	if(inst->thread == IM_NULL){
		free((void *)inst);
		return IM_RET_FAILED;
	}
	
	return IM_RET_OK;
}

IM_RET im_oswl_thread_terminate(im_thread_t thrd)
{
	im_thread_internal_t *inst = (im_thread_internal_t *)thrd;
	HANDLE hThread;
	if(inst == IM_NULL){
		return IM_RET_FAILED;
	}

	hThread = (HANDLE)InterlockedExchange((LONG *)&inst->thread, 0);
	if(hThread){
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	
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
		prio = THREAD_PRIORITY_HIGHEST;
	}else if(prio == IM_THREAD_PRIO_HIGHER){
		prio = THREAD_PRIORITY_ABOVE_NORMAL;
	}else if(prio == IM_THREAD_PRIO_NORMAL){
		prio = THREAD_PRIORITY_NORMAL;
	}else if(prio == IM_THREAD_PRIO_LOWER){
		prio = THREAD_PRIORITY_BELOW_NORMAL;
	}else if(prio == IM_THREAD_PRIO_LOWEST){
		prio = THREAD_PRIORITY_LOWEST;
	}else{
		return IM_RET_FAILED;
	}
	
	if(!SetThreadPriority(inst->thread, prio)){
		return IM_RET_FAILED;
	}

	if(oldPrio != IM_NULL){
		*oldPrio = inst->prio;
	}
	inst->prio = prio;

	return IM_RET_OK;
}

void im_oswl_sleep(IM_INT32 ms){
	Sleep(ms);
}

IM_TCHAR * im_oswl_strcpy(IM_TCHAR *dst, IM_TCHAR *src){
	return _tcscpy(dst, src);
}

IM_TCHAR * im_oswl_strncpy(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 size){
	return _tcsncpy(dst, src, size);
}

IM_INT32 im_oswl_strlen(IM_TCHAR *str){
	return (IM_INT32)_tcslen(str);
}

IM_TCHAR * im_oswl_strcat(IM_TCHAR *dst, IM_TCHAR *src){
	return _tcscat(dst, src);
}

IM_INT32 im_oswl_strcmp(IM_TCHAR *dst, IM_TCHAR *src){
	return (IM_INT32)_tcscmp(dst, src);
}

IM_INT32 im_oswl_strncmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return (IM_INT32)_tcsncmp(dst, src, n);
}

IM_TCHAR * im_oswl_strchr(IM_TCHAR *s, IM_INT32 c){
	return _tcschr(s, c);
}

IM_TCHAR * im_oswl_strrchr(IM_TCHAR *s, IM_INT32 c){
	return _tcsrchr(s, c);
}


IM_INT32 im_oswl_CharString2TCharString(IM_TCHAR *dst, char *src){
	return mbstowcs(dst, src, strlen(src));
}

IM_INT32 im_oswl_TCharString2CharString(char *dst, IM_INT32 dstSize, IM_TCHAR *src){
	return wcstombs(dst, src, dstSize);
}

IM_INT32 im_oswl_GetSystemTimeMs(){
	return GetTickCount();
}

