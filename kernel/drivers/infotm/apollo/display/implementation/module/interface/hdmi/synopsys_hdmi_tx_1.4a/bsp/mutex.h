/*
 * mutex.h
 *
 *  Created on: Jun 25, 2010
 *      Author: klabadi & dlopo
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#ifdef __XMK__
#include "xmk.h"
#include "pthread.h"
#endif
#include "types.h"
/*
 * The following should be modified if another type of mutex is needed
 */

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @param pHandle pointer to hold the value of the handle
 * @return TRUE if successful
 */
int mutex_Initialize(void* pHandle);
/**
 * @param pHandle pointer mutex handle
 * @return TRUE if successful
 */
int mutex_Destruct(void* pHandle);
/**
 * @param pHandle pointer mutex handle
 * @return TRUE if successful
 */
int mutex_Lock(void* pHandle);
/**
 * @param pHandle pointer mutex handle
 * @return TRUE if successful
 */
int mutex_Unlock(void* pHandle);

#ifdef __cplusplus
}
#endif

#endif /* MUTEX_H_ */
