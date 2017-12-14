/*
 * system.c
 *
 *  Created on: Jun 25, 2010
 *      Author: klabadi & dlopo
 * 
 * 
 * 	@note: this file should be re-written to match the environment the 
 * 	API is running on
 */
#ifdef __cplusplus
extern "C"
{
#endif
#include "system.h"
#include "log.h"
#include "error.h"
#ifdef __XMK__
#include "xmk.h"
#include "xparameters.h"
#include "xstatus.h"
#include "sys/time.h"
#include "sys/init.h"
#include "sys/intr.h"
#include "pthread.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "errno.h"
void xilkernel_init();
void xilkernel_start();
#else
#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
//#include <unistd.h>
#endif
#endif

void system_SleepMs(unsigned ms)
{
	return;
}

int system_ThreadCreate(void* handle, thread_t pFunc, void* param)
{
	return TRUE;
}
int system_ThreadDestruct(void* handle)
{
	return TRUE;
}

int system_Start(thread_t thread)
{
	return 0;
}

int system_InterruptDisable(interrupt_id_t id)
{
	return TRUE;
}

int system_InterruptEnable(interrupt_id_t id)
{
	return TRUE;
}

int system_InterruptAcknowledge(interrupt_id_t id)
{
	return TRUE;
}
int system_InterruptHandlerRegister(interrupt_id_t id, handler_t handler,
		void * param)
{
	return TRUE;
}

int system_InterruptHandlerUnregister(interrupt_id_t id)
{
	return TRUE;
}

#ifdef __cplusplus
}
#endif
