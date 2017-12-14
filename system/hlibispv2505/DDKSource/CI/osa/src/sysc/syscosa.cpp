/*!
******************************************************************************
 @file   :  osa.c

 @brief  OS Abstraction Layer (OSA)

 @Author Imagination Technologies

 @date   03/10/2012

         <b>Copyright 2012 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 <b>Description:</b>\n
         OS Adaption Layer (OSA)

******************************************************************************/
#include <new>
#include <map>

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include "systemc.h"

extern "C" {
#include "osa.h"
#include "img_errors.h"
#include "img_defs.h"
}

/*!
******************************************************************************

 @class osa_sc_semaphore

 Equivalent functionality to sc_semaphore, but allows dynamic creation
 All sc_semaphore objects have to be created during elaboration

******************************************************************************/
class osa_sc_semaphore
{
public:
	osa_sc_semaphore() : count(0) {}

	void wait()
	{
		while(count <= 0)
		{
			sc_core::wait(updated);
		}

		count--;
	}

	int wait(sc_time timeout)
	{
		sc_time absolute_timeout = sc_time_stamp() + timeout;
		while((count <= 0) && (sc_time_stamp() < absolute_timeout))
		{
			sc_core::wait(absolute_timeout - sc_time_stamp(), updated);
		}

		return trywait();
	}

	int trywait()
	{
		if(count <= 0)
			return -1;

		count--;
		return 0;
	}

	void post()
	{
		count++;
		updated.notify(SC_ZERO_TIME);
	}

	int get_value() 
	{
		return count;
	}

private:
	int count;
	sc_event updated;
};

/*!
******************************************************************************

 @class osa_sc_mutex

 Similar to sc_mutex, but allows dynamic creation and recursive locking
 All sc_mutex objects have to be created during elaboration

******************************************************************************/
class osa_sc_mutex
{
public:
	osa_sc_mutex() : locked(0) {}

	void lock()
	{
		while(trylock() < 0)
		{
			sc_core::wait(updated);
		}
	}

	int trylock()
	{
		if(locked && owner.valid() && (owner.get_process_object() != sc_get_current_process_handle().get_process_object()))
			return -1;

		if(!owner.valid())
			locked = 0;

		locked++;
		owner = sc_get_current_process_handle();

		return 0;
	}

	void unlock()
	{
		locked--;
		updated.notify(SC_ZERO_TIME);
	}

private:
	int locked;
	sc_event updated;
	sc_process_handle owner;
};


/*!
******************************************************************************

 @Function                OSA_CritSectCreate

******************************************************************************/
extern "C" IMG_RESULT OSA_CritSectCreate(
    IMG_HANDLE * const  phCritSect
)
{
	osa_sc_mutex* mutex;
	
	try {
		mutex = new osa_sc_mutex;
	} catch(std::bad_alloc) { mutex = NULL; }
	IMG_ASSERT(mutex != NULL);
	if(mutex == NULL) {
		return IMG_ERROR_MALLOC_FAILED;
	}

    //save pinter to critical section as IMG_HANDLE
    *phCritSect = (IMG_HANDLE)mutex;

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                OSA_CritSectDestroy

******************************************************************************/
extern "C" IMG_VOID OSA_CritSectDestroy(
    IMG_HANDLE  hCritSect
)
{
	osa_sc_mutex* mutex = (osa_sc_mutex*)hCritSect;
    IMG_ASSERT(mutex != NULL);

    if(mutex != NULL)
    {
		delete mutex;
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to CritSect\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_CritSectUnlock

******************************************************************************/
extern "C" IMG_VOID OSA_CritSectUnlock(
    IMG_HANDLE  hCritSect
)
{
	osa_sc_mutex* mutex = (osa_sc_mutex*)hCritSect;
    IMG_ASSERT(mutex != NULL);

    if(mutex == NULL)
    {
        fprintf(stderr, "Passed handler is not a handler to CritSect\n");
        abort();
    }

	mutex->unlock();
}


/*!
******************************************************************************

 @Function                OSA_CritSectLock

******************************************************************************/
extern "C" IMG_VOID OSA_CritSectLock(
    IMG_HANDLE  hCritSect
)
{
	osa_sc_mutex* mutex = (osa_sc_mutex*)hCritSect;
    IMG_ASSERT(mutex != NULL);

    if(mutex == NULL)
    {
        fprintf(stderr, "Passed handler is not a handler to CritSect\n");
        abort();
    }

	mutex->lock();
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncCreate

******************************************************************************/
extern "C" IMG_RESULT OSA_ThreadSyncCreate(
    IMG_HANDLE * const  phThreadSync
)
{
	osa_sc_semaphore* semaphore;

	try {
		semaphore = new osa_sc_semaphore;
	} catch(std::bad_alloc) { semaphore = NULL; }
	IMG_ASSERT(semaphore != NULL);
	if(semaphore == NULL) {
		return IMG_ERROR_MALLOC_FAILED;
	}

    *phThreadSync = semaphore;

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncDestroy

******************************************************************************/
extern "C" IMG_VOID OSA_ThreadSyncDestroy(
    IMG_HANDLE  hThreadSync
)
{
    osa_sc_semaphore* semaphore = (osa_sc_semaphore *)hThreadSync;
    IMG_ASSERT(semaphore != NULL);

    if(semaphore != NULL)
    {
		delete semaphore;
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to ThreadSync\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncSignal

******************************************************************************/
extern "C" IMG_VOID OSA_ThreadSyncSignal(
    IMG_HANDLE  hThreadSync
)
{
    osa_sc_semaphore* semaphore = (osa_sc_semaphore *)hThreadSync;
    IMG_ASSERT(semaphore != NULL);

    if(semaphore != NULL)
    {
		semaphore->post();
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to ThreadSync\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSyncWait

******************************************************************************/
extern "C" IMG_RESULT OSA_ThreadSyncWait(
    IMG_HANDLE  hThreadSync,
    IMG_UINT32  ui32TimeoutMs
)
{
    osa_sc_semaphore* semaphore = (osa_sc_semaphore *)hThreadSync;
    IMG_ASSERT(semaphore != NULL);

    if(semaphore != NULL)
    {
		int ret = 0;
		if(ui32TimeoutMs == OSA_NO_TIMEOUT)
			semaphore->wait();
		else
			ret = semaphore->wait(sc_time(ui32TimeoutMs, SC_MS));

		if(ret < 0)
			return IMG_ERROR_TIMEOUT;
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to ThreadSync\n");
        abort();
    }

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                OSA_ThreadCreateAndStart

******************************************************************************/
extern "C" IMG_RESULT OSA_ThreadCreateAndStart(
    OSA_pfnThreadFunc       pfnThreadFunc,
    IMG_VOID *              pvThreadParam,
    const IMG_CHAR * const  pszThreadName,
    OSA_eThreadPriority     eThreadPriority,
    IMG_HANDLE * const      phThread
)
{
	sc_process_handle* thread;

	try {
		thread = new sc_process_handle;
	} catch(std::bad_alloc) { thread = NULL; }
	IMG_ASSERT(thread != NULL);
	if(thread == NULL) {
		return IMG_ERROR_MALLOC_FAILED;
	}

	//Replace characters in name SystemC doesn't like
	std::string name(pszThreadName);
	for(size_t i = 0; i < name.size(); i++)
	{
		if(name[i] == ' ' || name[i] == '.')
			name[i] = '_';
	}

	try {
		*thread = sc_spawn(sc_bind(pfnThreadFunc, pvThreadParam), sc_gen_unique_name(name.c_str(), true));
	} catch(...) { delete thread; thread = NULL; }
	IMG_ASSERT(thread != NULL);
	if(thread == NULL) {
		return IMG_ERROR_FATAL;
	}

    *phThread = thread;

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function                OSA_ThreadWaitExitAndDestroy

******************************************************************************/
extern "C" IMG_VOID OSA_ThreadWaitExitAndDestroy(
    IMG_HANDLE  hThread
)
{
	sc_process_handle* thread = (sc_process_handle*)hThread;
    IMG_ASSERT(thread != NULL);

	if(thread != NULL && thread->valid())
    {
		while(!thread->terminated())
		{
			sc_core::wait(thread->terminated_event());
		}

		delete thread;
    }
    else
    {
        fprintf(stderr, "Passed handler is not a handler to OSA Thread\n");
        abort();
    }
}


/*!
******************************************************************************

 @Function                OSA_ThreadSleep

******************************************************************************/
extern "C" IMG_VOID OSA_ThreadSleep(
    IMG_UINT32  ui32SleepMs
)
{
	sc_core::wait(ui32SleepMs, SC_US);
}


static std::map<IMG_PVOID, IMG_PVOID> threadLocalStorage;

/*!
******************************************************************************

 @Function                OSA_ThreadLocalDataSet

******************************************************************************/
extern "C" IMG_RESULT OSA_ThreadLocalDataSet(
    IMG_PVOID pData
)
{
    //check input
    IMG_ASSERT(pData != NULL);
    if(pData == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

	threadLocalStorage[sc_get_current_process_handle().get_process_object()] = pData;

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function                OSA_ThreadLocalDataGet

******************************************************************************/
IMG_RESULT OSA_ThreadLocalDataGet(
    IMG_PVOID * ppData
)
{
    //check input
    IMG_ASSERT(ppData != NULL);
    if(ppData == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
 
	*ppData = (IMG_PVOID) threadLocalStorage[sc_get_current_process_handle().get_process_object()];

    return IMG_SUCCESS;
}


