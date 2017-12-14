#include "img_errors.h"
#include "eventTest.h"
#include "osa.h"

#include <stdlib.h>
#include <stdio.h>

#define OSA_TEST_TIMEOUT 100
#define N_THREADS 10
#define N_EVENTS 100

typedef struct{
	IMG_HANDLE hEvent;
	IMG_UINT32 param;
}sEventParams;

IMG_RESULT OSA_TEST_EventCreateAndDestroy()
{
	IMG_HANDLE hEvent;
	IMG_RESULT ret = IMG_SUCCESS;

	if( (ret = OSA_ThreadSyncCreate(&hEvent)) != IMG_SUCCESS)
	{
		fprintf(stderr,"OSA_ThreadSyncCreate failed: %d", ret);
	}

	OSA_ThreadSyncDestroy(hEvent);

	return ret;
}
IMG_RESULT OSA_TEST_EventSignalNoTimeout()
{
	IMG_HANDLE hEvent;
	IMG_RESULT ret = IMG_SUCCESS;

	OSA_ThreadSyncCreate(&hEvent);

	OSA_ThreadSyncSignal(hEvent);
	if( (ret = OSA_ThreadSyncWait(hEvent, OSA_NO_TIMEOUT)) != IMG_SUCCESS)
	{
		fprintf(stderr,"OSA_ThreadSyncWait failed: %d", ret);
	}

	OSA_ThreadSyncDestroy(hEvent);

	return ret;
}

IMG_RESULT OSA_TEST_EventSignalTimeout()
{
	IMG_HANDLE hEvent;
	IMG_RESULT ret = IMG_SUCCESS;

	OSA_ThreadSyncCreate(&hEvent);

	OSA_ThreadSyncSignal(hEvent);
	if(OSA_ThreadSyncWait(hEvent, OSA_TEST_TIMEOUT) != IMG_SUCCESS)
	{
		fprintf(stderr,"OSA_ThreadSyncWait failed: %d", ret);
	}

	OSA_ThreadSyncDestroy(hEvent);

	return ret;
}

IMG_RESULT OSA_TEST_EventSignalTimeoutElapse()
{
	IMG_HANDLE hEvent;
	IMG_RESULT ret = IMG_SUCCESS;

	OSA_ThreadSyncCreate(&hEvent);

	if(OSA_ThreadSyncWait(hEvent, OSA_TEST_TIMEOUT) != IMG_ERROR_TIMEOUT)
	{
		fprintf(stderr,"OSA_ThreadSyncWait failed");
		ret = IMG_ERROR_TIMEOUT;
	}

	OSA_ThreadSyncDestroy(hEvent);

	return ret;
}

static IMG_VOID SignalEvent(IMG_VOID * EventParams)
{
	sEventParams *params = (sEventParams*)EventParams;
	
	OSA_ThreadSleep(params->param);
	OSA_ThreadSyncSignal(params->hEvent);
}

IMG_RESULT OSA_TEST_EventSignalFromOtherThread()
{
	sEventParams params;
	IMG_HANDLE hThread;
	IMG_RESULT ret = IMG_SUCCESS;

	OSA_ThreadSyncCreate(&params.hEvent);
	params.param = 0;

	OSA_ThreadCreateAndStart(SignalEvent,
							 &params,
							 "SignalEventThread",
							 OSA_THREAD_PRIORITY_NORMAL,
							 &hThread);

	if(OSA_ThreadSyncWait(params.hEvent, OSA_NO_TIMEOUT) != IMG_SUCCESS)
	{
		fprintf(stderr,"OSA_ThreadSyncWait failed: %d", ret);
	}

	OSA_ThreadWaitExitAndDestroy(hThread);
	OSA_ThreadSyncDestroy(params.hEvent);

	return ret;
}

IMG_RESULT OSA_TEST_EventSignalThreadTimeout()
{
	sEventParams params;
	IMG_HANDLE hThread;
	IMG_RESULT ret = IMG_SUCCESS;

	OSA_ThreadSyncCreate(&params.hEvent);
	params.param = OSA_TEST_TIMEOUT - 10 >= 0 ? OSA_TEST_TIMEOUT - 10 : 0;

	OSA_ThreadCreateAndStart(SignalEvent,
							 &params,
							 "SignalEventThread",
							 OSA_THREAD_PRIORITY_NORMAL,
							 &hThread);

	if(OSA_ThreadSyncWait(params.hEvent, OSA_NO_TIMEOUT) != IMG_SUCCESS)
	{
		fprintf(stderr,"OSA_ThreadSyncWait failed: %d", ret);
	}

	OSA_ThreadWaitExitAndDestroy(hThread);
	OSA_ThreadSyncDestroy(params.hEvent);

	return ret;
}

IMG_RESULT OSA_TEST_EventSignalThreadTimeoutElaps()
{
	sEventParams params;
	IMG_HANDLE hThread;
	IMG_RESULT ret = IMG_SUCCESS;

	OSA_ThreadSyncCreate(&params.hEvent);
	params.param = OSA_TEST_TIMEOUT + 10;

	OSA_ThreadCreateAndStart(SignalEvent,
							 &params,
							 "SignalEventThread",
							 OSA_THREAD_PRIORITY_NORMAL,
							 &hThread);

	if(OSA_ThreadSyncWait(params.hEvent, OSA_TEST_TIMEOUT) != IMG_ERROR_TIMEOUT)
	{
		fprintf(stderr,"OSA_ThreadSyncWait failed: %d", ret);
		ret = IMG_ERROR_TIMEOUT;
	}

	OSA_ThreadWaitExitAndDestroy(hThread);
	OSA_ThreadSyncDestroy(params.hEvent);

	return ret;
}

static IMG_VOID UseFakeResource(IMG_VOID * EventParams)
{
	sEventParams *params = (sEventParams*)EventParams;
	
	OSA_ThreadSyncWait(params->hEvent,OSA_NO_TIMEOUT);
	params->param++;
}

IMG_RESULT OSA_TEST_EventWaitForResource()
{
	sEventParams resource;
	IMG_HANDLE hThread;
	IMG_RESULT ret = IMG_SUCCESS;

	resource.param = 0;

	OSA_ThreadSyncCreate(&resource.hEvent);	


	OSA_ThreadCreateAndStart(UseFakeResource,
							 &resource,
							 "FakeResourceThread",
							 OSA_THREAD_PRIORITY_NORMAL,
							 &hThread);



	OSA_ThreadSleep(100);
	if(resource.param > 0)
	{
		fprintf(stderr,"OSA_TEST_EventWaitForResource failed: resource used before appropriate signal");
		ret = IMG_ERROR_FATAL;
	}

	OSA_ThreadSyncSignal(resource.hEvent);
	OSA_ThreadWaitExitAndDestroy(hThread);
	
	if(resource.param != 1)
	{
		fprintf(stderr,"OSA_TEST_EventWaitForResource failed: thread didn't use the resource");
		ret = IMG_ERROR_FATAL;
	}
	
	OSA_ThreadSyncDestroy(resource.hEvent);

	return ret;
}

IMG_RESULT OSA_TEST_EventCreatePlenty()
{
	IMG_HANDLE hEvent[N_EVENTS];
	int i;

	for(i=0; i<N_EVENTS; i++)
	{
		OSA_ThreadSyncCreate(&hEvent[i]);	
	}

	for(i=0; i<N_EVENTS; i++)
	{
		OSA_ThreadSyncSignal(hEvent[i]);	
	}

	for(i=0; i<N_EVENTS; i++)
	{
		OSA_ThreadSyncDestroy(hEvent[i]);
	}
	
	

	return IMG_SUCCESS;
}


typedef struct{

	IMG_HANDLE hEvent;
	IMG_UINT32 ui32MsgCount;
	IMG_HANDLE hMutex;
    IMG_BOOL   bExit;

}MsgStruct;


static IMG_VOID GenerateMessage(IMG_VOID * NewMsg)
{
	MsgStruct * pNewMsg = (MsgStruct*)NewMsg;
	
	OSA_ThreadSleep(30);
	OSA_CritSectLock(pNewMsg->hMutex);
	++pNewMsg->ui32MsgCount;
	OSA_CritSectUnlock(pNewMsg->hMutex);
	OSA_ThreadSyncSignal(pNewMsg->hEvent);
}


IMG_RESULT OSA_TEST_EventSimulateScheduler()
{
	IMG_HANDLE hNewMsgEvent;
	IMG_HANDLE hMutex;
	IMG_UINT32 ui32ReadMessages = 0;
	IMG_HANDLE hThread[N_THREADS];
	MsgStruct sMessage;
	int i;
	
	OSA_CritSectCreate(&hMutex);
	OSA_ThreadSyncCreate(&hNewMsgEvent);	

	sMessage.hEvent = hNewMsgEvent;
	sMessage.ui32MsgCount = 0;
	sMessage.hMutex = hMutex;

	for(i=0; i<N_THREADS; i++)
	{
		OSA_ThreadCreateAndStart(GenerateMessage,
								 &sMessage,
								 "FakeResourceThread",
								 OSA_THREAD_PRIORITY_NORMAL,
								 &hThread[i]);

	}

	while(ui32ReadMessages != N_THREADS)
	{
		OSA_ThreadSyncWait(hNewMsgEvent, OSA_NO_TIMEOUT);

		while(sMessage.ui32MsgCount != 0)
		{
			OSA_ThreadSleep(100);//simulate processing message
			OSA_CritSectLock(hMutex);
			--sMessage.ui32MsgCount;
			OSA_CritSectUnlock(hMutex);
			++ui32ReadMessages;
		}
	}

	for(i=0; i<N_THREADS; i++)
	{
		OSA_ThreadWaitExitAndDestroy(hThread[i]);
	}

	OSA_ThreadSyncDestroy(hNewMsgEvent);
    OSA_CritSectDestroy(hMutex);

	return IMG_SUCCESS;
}

static IMG_VOID Dispatcher(IMG_VOID * NewMsg)
{
	MsgStruct * pNewMsg = (MsgStruct*)NewMsg;
	
    while(pNewMsg->bExit == IMG_FALSE)
    {
        OSA_ThreadSyncWait(pNewMsg->hEvent, OSA_NO_TIMEOUT);

        if(pNewMsg->bExit == IMG_FALSE)
        {
            //process message
            OSA_ThreadSleep(10);
	        OSA_CritSectLock(pNewMsg->hMutex);
	        --pNewMsg->ui32MsgCount;
	        OSA_CritSectUnlock(pNewMsg->hMutex);
        }
    }
}


IMG_RESULT OSA_TEST_EventSimulateDispatcher()
{
	IMG_HANDLE hNewMsgEvent;
	IMG_HANDLE hMutex;
	IMG_HANDLE hThread[N_THREADS];
    IMG_HANDLE hTaskThread;
	MsgStruct sMessage;
	int i;
	
	OSA_CritSectCreate(&hMutex);
	OSA_ThreadSyncCreate(&hNewMsgEvent);	

	sMessage.hEvent = hNewMsgEvent;
	sMessage.ui32MsgCount = 0;
	sMessage.hMutex = hMutex;
    sMessage.bExit = IMG_FALSE;

    //add few messages to check if event really counts
    for(i=0; i<N_THREADS/2; i++)
	{
		OSA_ThreadCreateAndStart(GenerateMessage,
								 &sMessage,
								 "FakeResourceThread",
								 OSA_THREAD_PRIORITY_NORMAL,
								 &hThread[i]);

	}

    OSA_ThreadSleep(10);

	OSA_ThreadCreateAndStart(Dispatcher,
						     &sMessage,
							 "DispatcherThread",
							 OSA_THREAD_PRIORITY_NORMAL,
							 &hTaskThread);

    OSA_ThreadSleep(10);

    //and a bit of usual work
    for(i=N_THREADS/2; i<N_THREADS; i++)
	{
		OSA_ThreadCreateAndStart(GenerateMessage,
								 &sMessage,
								 "FakeResourceThread",
								 OSA_THREAD_PRIORITY_NORMAL,
								 &hThread[i]);

	}

    for(i=0; i<N_THREADS; i++)
	{
		OSA_ThreadWaitExitAndDestroy(hThread[i]);
	}

    while(sMessage.ui32MsgCount != 0)
    {
        OSA_ThreadSleep(10);
    }

    //signal exit
    sMessage.bExit = IMG_TRUE;
    GenerateMessage(&sMessage);

	OSA_ThreadWaitExitAndDestroy(hTaskThread);

	OSA_ThreadSyncDestroy(hNewMsgEvent);
    OSA_CritSectDestroy(hMutex);

	return IMG_SUCCESS;
}
