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
**
*****************************************************************************/ 

#include <InfotmMedia.h>


typedef struct{
	func_mempool_malloc_t	func_malloc;
	func_mempool_free_t	func_free;
}im_mpool_internal_t;

im_mempool_handle_t im_mpool_init(func_mempool_malloc_t func_malloc, func_mempool_free_t func_free)
{
	im_mpool_internal_t *mpool;

	mpool = (im_mpool_internal_t *)func_malloc(sizeof(im_mpool_internal_t));
	if(mpool == IM_NULL){
		return IM_NULL;
	}

	mpool->func_malloc = func_malloc;
	mpool->func_free = func_free;
	return (im_mempool_handle_t)mpool;
}

IM_RET im_mpool_deinit(IN im_mempool_handle_t hmpl)
{
	im_mpool_internal_t *mpool = (im_mpool_internal_t *)hmpl;
	mpool->func_free((void *)mpool);
	return IM_RET_OK;
}

void * im_mpool_alloc(IN im_mempool_handle_t hmpl, IN IM_INT32 size)
{
	im_mpool_internal_t *mpool = (im_mpool_internal_t *)hmpl;
	return mpool->func_malloc(size);
}

IM_RET im_mpool_free(IN im_mempool_handle_t hmpl, void *p)
{
	im_mpool_internal_t *mpool = (im_mpool_internal_t *)hmpl;
	mpool->func_free(p);
	return IM_RET_OK;
}


