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
** v1.4.0	leo@2012/08/13: if not in Android system, remove IM_Strcasecmp
**		and IM_Strncasecmp.
**
*****************************************************************************/ 

#ifndef __IM_OSWL_H__
#define __IM_OSWL_H__

class IM_Lock{
private:
#if (TARGET_SYSTEM == FS_ANDROID)
	pthread_mutex_t mMutex;
#elif (TARGET_SYSTEM == FS_WINCE)
	CRITICAL_SECTION mCs;
#endif

public:	
	IM_Lock();
	virtual ~IM_Lock();

	virtual IM_RET lock();
	virtual IM_RET unlock();
#if (TARGET_SYSTEM == FS_ANDROID)
	pthread_mutex_t *getlock(){
		return &mMutex;
	}
#endif
};

class IM_AutoLock{
public:
	IM_AutoLock(){mLck = IM_NULL;}
	IM_AutoLock(IN IM_Lock *lck){
		mLck = lck;
		mLck->lock();
	}

	virtual ~IM_AutoLock(){
		if(mLck != IM_NULL){
			mLck->unlock();
			mLck = IM_NULL;
		}
	}

private:
	IM_Lock *mLck;	
};

class IM_Signal{
private:
#if (TARGET_SYSTEM == FS_ANDROID)
	pthread_cond_t mCond;
	pthread_mutex_t mMutex;
	IM_INT32 mMark;
	IM_BOOL mManualReset;
#elif (TARGET_SYSTEM == FS_WINCE)
	HANDLE  mEvent;	
#endif

public:
	IM_Signal(IN IM_BOOL manualReset=IM_FALSE);
	virtual ~IM_Signal();

	virtual IM_RET set();
	virtual IM_RET reset();
	virtual IM_RET wait(IN IM_Lock *lck, IN IM_INT32 timeout=-1);
};

#define IM_THREAD_PRIO_HIGHEST	1
#define IM_THREAD_PRIO_HIGHER	2
#define IM_THREAD_PRIO_NORMAL	3
#define IM_THREAD_PRIO_LOWER		4
#define IM_THREAD_PRIO_LOWEST	5
class IM_Thread{
private:
#if (TARGET_SYSTEM == FS_ANDROID)
	pthread_t mThread;
#elif (TARGET_SYSTEM == FS_WINCE)
	HANDLE  mThread;	
#endif
	IM_INT32 mPrio;

public:
	IM_Thread();
	virtual ~IM_Thread();

#if (TARGET_SYSTEM == FS_ANDROID)
	typedef void *(*THREAD_ENTRY)(void *);
#elif (TARGET_SYSTEM == FS_WINCE)
	typedef IM_INT32 (*THREAD_ENTRY)(void *);
#endif	
	virtual IM_RET create(IN THREAD_ENTRY entry, IN void *param);
	virtual IM_RET forceTerminate();
	virtual IM_RET setPriority(IM_INT32 prio, IM_INT32 *oldPrio=IM_NULL);
};

class IM_String{
public:
	IM_String(IM_INT32 length){
		mString = new IM_TCHAR[length];
		memset(mString, 0, sizeof(IM_TCHAR) * length);
	}
	IM_String(IM_TCHAR *str);
	virtual ~IM_String();

	IM_TCHAR *string(){return mString;}
	virtual IM_INT32 length();
	virtual IM_INT32 size();
	virtual IM_TCHAR * copyFrom(IM_TCHAR *src);
	virtual IM_TCHAR * copyFrom(IM_String *src){return copyFrom(src->string());}
	virtual IM_TCHAR * copyTo(IM_TCHAR *dst);
	virtual IM_TCHAR * copyTo(IM_String *dst){return copyTo(dst->string());}
	
private:
	IM_TCHAR *mString;
};


void IM_Sleep(IM_INT32 ms);

IM_TCHAR * IM_Strcpy(IM_TCHAR *dst, IM_TCHAR *src);
IM_TCHAR * IM_Strncpy(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n);
IM_INT32 IM_Strlen(IM_TCHAR *str);
IM_TCHAR * IM_Strcat(IM_TCHAR *dst, IM_TCHAR *src);
IM_INT32 IM_Strcmp(IM_TCHAR *dst, IM_TCHAR *src);
IM_INT32 IM_Strncmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n);
#if (TARGET_SYSTEM == FS_ANDROID)
IM_INT32 IM_Strcasecmp(IM_TCHAR *dst, IM_TCHAR *src);
IM_INT32 IM_Strncasecmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n);
#endif
IM_TCHAR * IM_Strchr(IM_TCHAR *s, IM_INT32 c);
IM_TCHAR * IM_Strrchr(IM_TCHAR *s, IM_INT32 c);
IM_INT32 IM_CharString2TCharString(IM_TCHAR *dst, char *src);
IM_INT32 IM_TCharString2CharString(char *dst, IM_INT32 dstSize, IM_TCHAR *src);
IM_INT32 IM_GetSystemTimeMs();

//
//
//
#include <IM_oswl_c.h>

#endif	// __IM_OSWL_H__
