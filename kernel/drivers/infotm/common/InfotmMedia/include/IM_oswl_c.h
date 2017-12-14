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
** v1.3.0	leo@2012/03/29: Add extern "C" for C++ calling.
**
*****************************************************************************/ 

#ifndef __IM_OSWL_C_H__
#define __IM_OSWL_C_H__

#ifdef __cplusplus
extern "C"
{
#endif

//
// lock
//
typedef void *	im_lock_t;
IM_RET im_oswl_lock_init(im_lock_t *lck);
IM_RET im_oswl_lock_deinit(im_lock_t lck);
IM_RET im_oswl_lock_lock(im_lock_t lck);
IM_RET im_oswl_lock_unlock(im_lock_t lck);

//
// signal
//
typedef void * im_signal_t;
IM_RET im_oswl_signal_init(im_signal_t *sig, IM_BOOL manualReset);
IM_RET im_oswl_signal_deinit(im_signal_t sig);
IM_RET im_oswl_signal_set(im_signal_t sig);
IM_RET im_oswl_signal_reset(im_signal_t sig);
IM_RET im_oswl_signal_wait(im_signal_t sig, im_lock_t lck, IM_INT32 timeout);

//
// thread
//
typedef void * im_thread_t;

#define IM_THREAD_PRIO_HIGHEST	1
#define IM_THREAD_PRIO_HIGHER	2
#define IM_THREAD_PRIO_NORMAL	3
#define IM_THREAD_PRIO_LOWER	4
#define IM_THREAD_PRIO_LOWEST	5

#if (TARGET_SYSTEM == FS_ANDROID)
	typedef void *(*THREAD_ENTRY)(void *);
#elif (TARGET_SYSTEM == FS_WINCE)
	typedef IM_INT32 (*THREAD_ENTRY)(void *);
#endif	

IM_RET im_oswl_thread_init(im_thread_t *thrd, THREAD_ENTRY entry, void *p);
IM_RET im_oswl_thread_terminate(im_thread_t thrd);
IM_RET im_oswl_thread_set_priority(im_thread_t thrd, IM_INT32 prio, IM_INT32 *oldPrio);

//
//
//
void im_oswl_sleep(IM_INT32 ms);
IM_INT32 im_oswl_GetSystemTimeMs();

IM_TCHAR * im_oswl_strcpy(IM_TCHAR *dst, IM_TCHAR *src);
IM_TCHAR * im_oswl_strncpy(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n);
IM_INT32 im_oswl_strlen(IM_TCHAR *str);
IM_TCHAR * im_oswl_strcat(IM_TCHAR *dst, IM_TCHAR *src);
IM_INT32 im_oswl_strcmp(IM_TCHAR *dst, IM_TCHAR *src);
IM_INT32 im_oswl_strcasecmp(IM_TCHAR *dst, IM_TCHAR *src);
IM_INT32 im_oswl_strncmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n);
IM_INT32 im_oswl_strncasecmp(IM_TCHAR *dst, IM_TCHAR *src, IM_INT32 n);
IM_TCHAR * im_oswl_strchr(IM_TCHAR *s, IM_INT32 c);
IM_TCHAR * im_oswl_strrchr(IM_TCHAR *s, IM_INT32 c);
IM_INT32 im_oswl_CharString2TCharString(IM_TCHAR *dst, char *src);
IM_INT32 im_oswl_TCharString2CharString(char *dst, IM_INT32 dstSize, IM_TCHAR *src);


#ifdef __cplusplus
}
#endif

#endif	// __IM_OSWL_C_H__
