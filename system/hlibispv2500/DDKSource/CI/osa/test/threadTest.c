#include "threadTest.h"
#include "osa.h"
#include "img_errors.h"

#include <stdlib.h>
#include <stdio.h>

#define SLEEP_TIME 100
#define N_PRIORITIES 5

static int testValue = 0;
static IMG_HANDLE hMutex;

static IMG_VOID ThreadFunction(IMG_VOID * Params)
{
	OSA_ThreadSleep(SLEEP_TIME);

	OSA_CritSectLock(hMutex);
		testValue += *(int*)Params;
	OSA_CritSectUnlock(hMutex);
}

IMG_RESULT OSA_TEST_ThreadCreateAndDestroy()
{
	int someParam = 1;
	IMG_HANDLE hThread;

	testValue = 0;
	OSA_CritSectCreate(&hMutex);

	if(OSA_ThreadCreateAndStart(ThreadFunction,
		    			     &someParam,
							 "SomeThread",
							 OSA_THREAD_PRIORITY_NORMAL,
							 &hThread) != IMG_SUCCESS)
	{
		fprintf(stderr,"ThreadPriorities failed, cannot create new thread");
		return IMG_ERROR_FATAL;
	}

	OSA_ThreadWaitExitAndDestroy(hThread);

	if(testValue != someParam)
	{
		fprintf(stderr,"ThreadPriorities failed, thread didn't use resource");
		return IMG_ERROR_FATAL;
	}
	OSA_CritSectDestroy(hMutex);

	return IMG_SUCCESS;
}

IMG_RESULT OSA_TEST_ThreadPriorities()
{
	int someParam[N_PRIORITIES] = {1,2,3,4,5};
	int sum=0;
	int i;
	IMG_HANDLE hThread[N_PRIORITIES];

	OSA_CritSectCreate(&hMutex);
	testValue = 0;
			   
	OSA_ThreadCreateAndStart(ThreadFunction,&someParam[0],"LowestPriorityThread", OSA_THREAD_PRIORITY_LOWEST, &hThread[0]);
	OSA_ThreadCreateAndStart(ThreadFunction,&someParam[1],"BelowNormalPriorityThread", OSA_THREAD_PRIORITY_BELOW_NORMAL, &hThread[1]);
	OSA_ThreadCreateAndStart(ThreadFunction,&someParam[2],"NormalPriorityThread", OSA_THREAD_PRIORITY_NORMAL, &hThread[2]);
	OSA_ThreadCreateAndStart(ThreadFunction,&someParam[3],"AboveNormalPriorityThread", OSA_THREAD_PRIORITY_ABOVE_NORMAL, &hThread[3]);
	OSA_ThreadCreateAndStart(ThreadFunction,&someParam[4],"HighestPriorityThread", OSA_THREAD_PRIORITY_HIGHEST, &hThread[4]);


	for(i=0; i<N_PRIORITIES; i++)
	{
		OSA_ThreadWaitExitAndDestroy(hThread[i]);
		sum += someParam[i];
	}

	if(sum != testValue)
	{
		fprintf(stderr,"ThreadPriorities failed, some of thread didn't use resourse");
		return IMG_ERROR_FATAL;
	}

	OSA_CritSectDestroy(hMutex);

	return IMG_SUCCESS;
}
