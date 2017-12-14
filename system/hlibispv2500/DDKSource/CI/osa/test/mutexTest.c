#include "img_errors.h"
#include "mutexTest.h"
#include "osa.h"

#include <stdlib.h>
#include <stdio.h>

#define USE_TIME 100 //time that is needed to use fake resource 
#define N_THREADS 10
#define N_MUTEXES 101

IMG_RESULT OSA_TEST_MutexCreateAndDestroy()
{
	IMG_HANDLE hMutex;
	IMG_RESULT ret = IMG_SUCCESS;

	if( (ret = OSA_CritSectCreate(&hMutex)) != IMG_SUCCESS)
	{
		fprintf(stderr,"OSA_CritSectCreate failed: %d", ret);
	}

	OSA_CritSectDestroy(hMutex);

	return ret;
}

IMG_RESULT OSA_TEST_MutexLockUnlock()
{
	IMG_HANDLE hMutex;

	OSA_CritSectCreate(&hMutex);
	OSA_CritSectLock(hMutex);
	OSA_CritSectUnlock(hMutex);
	OSA_CritSectDestroy(hMutex);

	return IMG_SUCCESS;
}
static void NestedLock(IMG_HANDLE hMutex)
{
    OSA_CritSectLock(hMutex);

    OSA_ThreadSleep(1);

    OSA_CritSectUnlock(hMutex);
}

IMG_RESULT OSA_TEST_MutexNested()
{
	IMG_HANDLE hMutex;

	OSA_CritSectCreate(&hMutex);
	OSA_CritSectLock(hMutex);

    NestedLock(hMutex);

    OSA_CritSectUnlock(hMutex);
	OSA_CritSectDestroy(hMutex);

	return IMG_SUCCESS;
}

IMG_RESULT OSA_TEST_MutexCreatePlenty()
{
	IMG_HANDLE hMutex[N_MUTEXES];
	//IMG_HANDLE hThread[N_MUTEXES];
	int i;

	for(i=0; i<N_MUTEXES; i++)
	{
		OSA_CritSectCreate(&hMutex[i]);	
	}

	/*for(i=0; i<N_MUTEXES; i++)
	{
		if( OSA_ThreadCreateAndStart(UseFakeResource,
								 &hMutex,
								 "FakeResourceThread",
								 OSA_THREAD_PRIORITY_NORMAL,
								 &hThread[i]) != IMG_SUCCESS)
		{
			fprintf(stderr,"MutexWaitForResource cannot create new thread");
			return IMG_ERROR_FATAL;
		}

	}*/

	for(i=0; i<N_MUTEXES; i++)
	{
		OSA_CritSectDestroy(hMutex[i]);
	}
	
	

	return IMG_SUCCESS;
}

static int FakeResource()
{
	static int users = 0;

	users++;

	OSA_ThreadSleep(USE_TIME);
	users--;

	return users;
}

static IMG_VOID UseFakeResource(IMG_VOID * hFakeResourceMutex)
{
	IMG_HANDLE *hMutex = (IMG_HANDLE*)hFakeResourceMutex;
	
	OSA_CritSectLock(*hMutex);

	if(FakeResource() != 0)
	{
		fprintf(stderr,"UseFakeResource failed, more than one thread use resource in the same time");
		exit(-1);
	}

	OSA_CritSectUnlock(*hMutex);
}

IMG_RESULT OSA_TEST_MutexWaitForResource()
{
	IMG_HANDLE hMutex;
	IMG_HANDLE hThread[N_THREADS];
	int i;

	OSA_CritSectCreate(&hMutex);	

	for(i=0; i<N_THREADS; i++)
	{
		if( OSA_ThreadCreateAndStart(UseFakeResource,
								 &hMutex,
								 "FakeResourceThread",
								 OSA_THREAD_PRIORITY_NORMAL,
								 &hThread[i]) != IMG_SUCCESS)
		{
			fprintf(stderr,"MutexWaitForResource cannot create new thread");
			return IMG_ERROR_FATAL;
		}

	}

	for(i=0; i<N_THREADS; i++)
	{
		OSA_ThreadWaitExitAndDestroy(hThread[i]);
	}
	
	OSA_CritSectDestroy(hMutex);

	return IMG_SUCCESS;
}

