/*
 * types.h
 *
 *  Created on: Jun 25, 2010
 *      Author: klabadi & dlopo
 */

#ifndef TYPES_H_
#define TYPES_H_

/*
 * types
 * Define basic type optimised for use in the API so that it can be
 * platform-independent.
 */

//#include <stdlib.h>

#ifdef __XMK__
#include "xmk.h"
#include "xutil.h"
#include "pthread.h"
typedef pthread_mutex_t mutex_t;

#else
#include <linux/types.h>
//typedef unsigned char	u8;
//typedef unsigned short	u16;
//typedef unsigned int	u32;
typedef void * mutex_t;
#endif

#include <linux/bitops.h>
//#define BIT(n)		(1 << n)

typedef void (*handler_t)(void *);
typedef void* (*thread_t)(void *);

#define TRUE  1
#define FALSE 0

#endif /* TYPES_H_ */
