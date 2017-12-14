/*  
 * pwrapper.h - This file includes the POSIX wrapper's definitions and function prototypes.
 *  
 * Copyrignt (C) 2008 Hikvision Dig Tech Co.,Ltd
 *
 * Modification History
 * 01a,19Feb2008,Hu Jiexun Written
 *
 */

#ifndef __PWRAPPER_H__
#define __PWRAPPER_H__

#ifdef __cplusplus
extern  "C" {
#endif

#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>

/* timeout options */
#define NO_WAIT	0
#define WAIT_FOREVER (-1)

/* message priority options */
#define MQ_PRIO_NORMAL	0
#define MQ_PRIO_URGENT  1

/* semaphore options */
#define SEM_EMPTY 0
#define SEM_FULL  1

/* function pointer */
typedef void *(*FUNCPTR)(void*, void*, void*, void*, void*, void*, void*, void*, void*, void*);
typedef void *(*START_ROUTINE)(void*);

/* function wrapper structure */
typedef struct
{
	FUNCPTR entry; /* execution entry point address for thread */
	void *arg[10]; /* arguments */
}FUNC_WRAPPER2;

typedef struct
{
	int (*entry)(int,int,int,int,int,int,int,int,int,int); /* execution entry point address for thread */
	int parms[10]; /* arguments */
}FUNC_WRAPPER;


/*
 * POSIX thread library wrapper functions prototypes
 */
int pthreadGetPriorityScope(int *minPriority, int *maxPriority);
/* a variadic function to create a pthread */
#if 0
int pthreadSpawn(pthread_t *ptid, int priority, size_t stacksize, void *funcptr, unsigned args, ...);
int pthreadCreate(pthread_t	*ptid, int priority, size_t stacksize,
					void* funcptr,
					int arg0, int arg1, int arg2, int arg3, int arg4,
					int arg5, int arg6, int arg7, int arg8, int arg9);
#endif
pthread_t pthreadSelf();
int pthreadIdVerify(pthread_t tid);
int pthreadSuspend(pthread_t tid);
int pthreadResume(pthread_t tid);
int pthreadCancel(pthread_t tid);
/* pthread mutex */
int pthreadMutexInit(pthread_mutex_t *mutex);
int pthreadMutexLock(pthread_mutex_t *mutex, int wait_ms);
int pthreadMutexUnlock(pthread_mutex_t *mutex);
int pthreadMutexDestroy(pthread_mutex_t *mutex);
int pthreadCondWait(pthread_cond_t *cond, pthread_mutex_t *mutex, int wait_ms);

/*
 * POSIX message queue library wrapper functions prototypes
 */
mqd_t mqOpen(const char *name, int oflag, ...);
int mqSend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, int wait_ms, unsigned msg_prio);
ssize_t mqReceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, int wait_ms, unsigned *msg_prio);
int mqGetattr(mqd_t mqdes, struct mq_attr *mqstat);
int mqSetattr(mqd_t mqdes, const struct mq_attr *mqstat, struct mq_attr *omqstat);
int mqClose(mqd_t mqdes);
int mqUnlink(const char *name);

/*
 * POSIX semaphore library wrapper functions prototypes
 */
int semInit(sem_t *sem, unsigned value);
int semPost(sem_t *sem);
int semWait(sem_t *sem, int wait_ms);
int semDestroy(sem_t *sem);
void setPthreadName(char *name);

#ifdef __cplusplus
}
#endif

#endif//__PWRAPPER_H__

