#include "GXThread.h"
#include <stdio.h>
#include <errno.h>
#include "GXConfig.h"

#include <stdlib.h>
#include <fcntl.h>

/////////////////////////////////////////////////////////////////////////////////////
int GXThreadCreate(pthread_t *thread,const pthread_attr_t *attr, THREAD_RUN_FUN threadRunfun, void *arg)
{
    int nRt = pthread_create(thread, attr, threadRunfun, arg);
    if (0 != nRt) {
        GX_DEBUG("pthread_create error:%d",nRt);
    }
	//usleep(200);
    
    return nRt;
}

int GXThreadCreate2(pthread_t *thread, THREAD_RUN_FUN threadRunfun, void *arg)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int nRt = pthread_create(thread, &attr, threadRunfun, arg);
    pthread_attr_destroy(&attr);
    if (0 != nRt) {
        GX_DEBUG("pthread_create2 error:%d",nRt);
    }
    
    return nRt;
}

void GXThreadExit(void *value_ptr)
{
    pthread_exit(value_ptr);
}

int GXThreadJoin(pthread_t thread, void **value_ptr)
{
    int nRt = pthread_join(thread, value_ptr);
    if (0 != nRt) {
        GX_DEBUG("pthread_join error:%d",nRt);
    }
    
    return nRt;
}
