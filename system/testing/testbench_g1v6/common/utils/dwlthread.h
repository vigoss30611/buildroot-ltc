/* Copyright 2013 Google Inc. All rights reserved. */

#ifndef __DWL_THREAD_H__
#define __DWL_THREAD_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* This header file is the POSIX pthread.h and semaphore.h entry point
 * for the entire decoder. If pthread cannot be used the decoder can be
 * directed to use a replacing implementation via this interface. */

/* Undefine _HAVE_PTHREAD_H to replace POSIX pthread.h and semaphore.h */
#define _HAVE_PTHREAD_H

#ifdef _HAVE_PTHREAD_H
  #include <pthread.h>
  #include <semaphore.h>
#else

/* The following error check can be removed when inplementation available. */
#error "Threading and semaphore interface not implemented."

#define  DWL_PLACEHOLDER_VALUE 0
typedef void * DWL_PLACEHOLDER_TYPE;

#define PTHREAD_MUTEX_INITIALIZER DWL_PLACEHOLDER_VALUE
#define PTHREAD_CREATE_JOINABLE DWL_PLACEHOLDER_VALUE

typedef DWL_PLACEHOLDER_TYPE sem_t ;
typedef DWL_PLACEHOLDER_TYPE pthread_t ;
typedef DWL_PLACEHOLDER_TYPE pthread_attr_t;

typedef DWL_PLACEHOLDER_TYPE pthread_cond_t;
typedef DWL_PLACEHOLDER_TYPE pthread_condattr_t;

typedef DWL_PLACEHOLDER_TYPE pthread_mutex_t;
typedef DWL_PLACEHOLDER_TYPE pthread_mutexattr_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_post(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_destroy(sem_t *sem);

int pthread_mutex_init(pthread_mutex_t *mutex,
            const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

int pthread_cond_init(pthread_cond_t *cond,
                        const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond,
                       pthread_mutex_t *mutex);

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                    void *(*start_routine) (void *), void *arg);
int pthread_join(pthread_t thread, void **retval);

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);

#endif /* _POSIX_THREADS */
#endif /* __DWL_THREAD_H__ */
