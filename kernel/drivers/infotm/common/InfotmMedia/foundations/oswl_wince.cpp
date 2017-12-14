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
** v1.4.0	leo@2012/08/13: remove IM_Strncmp and IM_Strncasecmp; rewrite
**			IM_Strchr, IM_Strrchr and IM_Strncpy.
**
*****************************************************************************/ 

#include <InfotmMedia.h>


IM_Lock::IM_Lock(){
	InitializeCriticalSection(&mCs);
}

IM_Lock::~IM_Lock(){
	DeleteCriticalSection(&mCs);
}

IM_RET IM_Lock::lock(){
	EnterCriticalSection(&mCs);
	return IM_RET_OK;
}

IM_RET IM_Lock::unlock(){
	LeaveCriticalSection(&mCs);
	return IM_RET_OK;
}


IM_Signal::IM_Signal(IM_BOOL manualReset){
	mEvent = CreateEvent(IM_NULL, manualReset, IM_FALSE, IM_NULL);
	IM_ASSERT(mEvent != IM_NULL);
}

IM_Signal::~IM_Signal(){
	if (mEvent){
		CloseHandle(mEvent);
		mEvent = IM_NULL;
	}
}

IM_RET IM_Signal::set(){
	if(SetEvent(mEvent) == 0){
		return IM_RET_FAILED;
	}else{
		return IM_RET_OK;
	}
}

IM_RET IM_Signal::reset(){
	if(ResetEvent(mEvent) == 0){
		return IM_RET_FAILED;
	}else{
		return IM_RET_OK;
	}
}

IM_RET IM_Signal::wait(IM_Lock *lck, IM_INT32 timeout){
	IM_RET ret;
	
	if(lck != IM_NULL)
		lck->unlock();
	
	DWORD wt = WaitForSingleObject(mEvent, (timeout==-1)?INFINITE:timeout);
	if(wt == WAIT_OBJECT_0){
		ret = IM_RET_OK;
	}else if(wt == WAIT_TIMEOUT){
		ret = IM_RET_TIMEOUT;
	}else{
		ret = IM_RET_FAILED;
	}
	
	if(lck != IM_NULL)
		lck->lock();

	return ret;
}

IM_Thread::IM_Thread() :
	mThread(IM_NULL),
	mPrio(IM_THREAD_PRIO_NORMAL){
}

IM_Thread::~IM_Thread()
{
     HANDLE hThread = (HANDLE)InterlockedExchange((LONG *)&mThread, 0);
     if (hThread) {
         WaitForSingleObject(hThread, INFINITE);
         CloseHandle(hThread);
     }
}

IM_RET IM_Thread::create(THREAD_ENTRY entry, void *param){
	mThread = CreateThread(IM_NULL, 0, (LPTHREAD_START_ROUTINE)entry, param, 0, IM_NULL);
	if(mThread == IM_NULL){
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET IM_Thread::forceTerminate(){
	if(mThread){
		TerminateThread(mThread, 1);
		CloseHandle(mThread);
	}
	return IM_RET_OK;
}

IM_RET IM_Thread::setPriority(IM_INT32 prio, IM_INT32 *oldPrio){
    if(mPrio == prio){
        if(oldPrio != IM_NULL){
            *oldPrio = mPrio;
        }
        return IM_RET_OK;
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
	
	if(!SetThreadPriority(mThread, prio)){
		return IM_RET_FAILED;
	}

	mPrio = prio;
    if(oldPrio != IM_NULL){
        *oldPrio = mPrio;
    }
    
	return IM_RET_OK;
}

IM_String::IM_String(IM_TCHAR *str){
	mString = new IM_TCHAR[_tcslen(str)+2];
	IM_ASSERT(mString != IM_NULL);
	_tcscpy(mString, str);
}

IM_String::~IM_String(){
	if(mString != IM_NULL){
		delete []mString;
	}	
}

IM_INT32 IM_String::length(){
	return _tcslen(mString);
}

IM_INT32 IM_String::size(){
	return length() * sizeof(IM_TCHAR);
}

IM_TCHAR* IM_String::copyFrom(IM_TCHAR *src){
	return _tcscpy(mString, src);
}

IM_TCHAR* IM_String::copyTo(IM_TCHAR *dst){
	return _tcscpy(dst, mString);
}

void IM_Sleep(IM_INT32 ms){
	Sleep(ms);
}

IM_TCHAR * IM_Strcpy(IM_TCHAR *dst, IM_TCHAR *src){
	return _tcscpy(dst, src);
}

IM_TCHAR * IM_Strncpy(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return _tcsncpy(dst, src, n);
}

IM_INT32 IM_Strlen(IM_TCHAR *str){
	return (IM_INT32)_tcslen(str);
}

IM_TCHAR * IM_Strcat(IM_TCHAR *dst, IM_TCHAR *src){
	return _tcscat(dst, src);
}

IM_INT32 IM_Strcmp(IM_TCHAR *dst, IM_TCHAR *src){
	return (IM_INT32)_tcscmp(dst, src);
}

IM_INT32 IM_Strncmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n){
	return (IM_INT32)_tcsncmp(dst, src, n);
}

IM_TCHAR * IM_Strchr(IM_TCHAR *s, IM_INT32 c){
	return _tcschr(s, c);
}

IM_TCHAR * IM_Strrchr(IM_TCHAR *s, IM_INT32 c){
	return _tcsrchr(s, c);
}


IM_INT32 IM_CharString2TCharString(IM_TCHAR *dst, char *src){
	return mbstowcs(dst, src, strlen(src));
}

IM_INT32 IM_TCharString2CharString(char *dst, IM_INT32 dstSize, IM_TCHAR *src){
	return wcstombs(dst, src, dstSize);
}

IM_INT32 IM_GetSystemTimeMs(){
	return GetTickCount();
}


#include "oswl_wince_c.c"

