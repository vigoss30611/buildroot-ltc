#ifndef GXTHREAD_H
#define GXTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef void* (*THREAD_RUN_FUN)(void *);

int GXThreadCreate(pthread_t *thread, const pthread_attr_t *attr, THREAD_RUN_FUN threadRunfun, void *arg);
int GXThreadCreate2(pthread_t *thread, THREAD_RUN_FUN threadRunfun, void *arg);
void GXThreadExit(void *value_ptr);
int GXThreadJoin(pthread_t thread, void **value_ptr);

    
#ifdef __cplusplus
}
#endif
    

#endif //GXThread_h
