/*
 * mutex.c
 *
 *  Created on: Jun 25, 2010
 *      Author: klabadi & dlopo
 */

#include "mutex.h"
#include "log.h"
#include "error.h"
#ifdef __XMK__
#include <sys/process.h>
#include "errno.h"
#endif

int mutex_Initialize(void* pHandle)
{
	return TRUE;
}

int mutex_Destruct(void* pHandle)
{
	return TRUE;
}

int mutex_Lock(void* pHandle)
{
	return TRUE;
}

int mutex_Unlock(void* pHandle)
{
	return TRUE;
}
