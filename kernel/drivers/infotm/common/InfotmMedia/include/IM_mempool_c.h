/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/03/15: first commit.
** v1.3.0	leo@2012/03/29: Add extern "C" for C++ calling.
**
*****************************************************************************/ 

#ifndef __IM_MEMPOOL_C_H__
#define __IM_MEMPOOL_C_H__

#ifdef __cplusplus
extern "C"
{
#endif

//
// handle.
//
typedef void * im_mempool_handle_t;

typedef void *(*func_mempool_malloc_t)(IM_INT32 size);
typedef void (*func_mempool_free_t)(void *p);

//
// interface.
//
im_mempool_handle_t im_mpool_init(func_mempool_malloc_t func_malloc, func_mempool_free_t func_free);
IM_RET im_mpool_deinit(IN im_mempool_handle_t hmpl);
void * im_mpool_alloc(IN im_mempool_handle_t hmpl, IN IM_INT32 size);
IM_RET im_mpool_free(IN im_mempool_handle_t hmpl, void *p);

#ifdef __cplusplus
}
#endif

#endif	// __IM_MEMPOOL_C_H__


