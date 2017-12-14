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
** v2.0.1	larry@2012/08/16: Add klist support. klist is a list with key 
**		which define the order of node.
**
*****************************************************************************/ 

#ifndef __IM_LIST_C_H__
#define __IM_LIST_C_H__

#ifdef __cplusplus
extern "C"
{
#endif

//
//
//
typedef void * im_list_handle_t;


/**
 * FUNC: initializae the list.
 * PARAMS: dataSize, the size of data of the list node. if it's 0, the list will
 * 	only save the data pointer itself, if it isn't 0, the list first alloc the dataSize
 * 	memory and then copy the data to this memory. So, you should noticed that
 * 	the return pointer meanning by im_list_get/get_index/begin/next/prev/end/erase...
 * RETURN: the list handle, if failed return IM_NULL.
 */
im_list_handle_t im_list_init(IN IM_INT32 dataSize, IN im_mempool_handle_t mpl);
IM_RET im_list_deinit(IN im_list_handle_t hlst);
IM_BOOL im_list_empty(IN im_list_handle_t hlst);/* returns true if the list is empty */
IM_INT32 im_list_size(IN im_list_handle_t hlst);
void * im_list_get(IN im_list_handle_t hlst);
void * im_list_get_index(IN im_list_handle_t hlst, IN IM_INT32 index);
void * im_list_begin(IN im_list_handle_t hlst);
void * im_list_next(IN im_list_handle_t hlst);
void * im_list_prev(IN im_list_handle_t hlst);
void * im_list_end(IN im_list_handle_t hlst);
IM_RET im_list_put_front(IN im_list_handle_t hlst, IN void *data);
IM_RET im_list_put_back(IN im_list_handle_t hlst, IN void *data);
IM_RET im_list_insert(IN im_list_handle_t hlst, IN void *data);// insert to current back, and this element will be current.
void * im_list_erase(IN im_list_handle_t hlst, IN void *data);/* remove one entry; returns iterator at next node*/
void im_list_clear(IN im_list_handle_t hlst);/* remove all contents of the list */

//
// klist is a list with key which define the order of the node. key is a integer
// from 0 to 2^32-1. when put a node, it will insert to a positon that the key of
// former node little or equal then it and the key of latter node bigger then it.
// except key, the others same to list.
//
typedef void * im_klist_handle_t;

im_klist_handle_t im_klist_init(IN IM_INT32 dataSize, IN im_mempool_handle_t mpl);
IM_RET im_klist_deinit(IN im_klist_handle_t hlst);
IM_BOOL im_klist_empty(IN im_klist_handle_t hlst);/* returns true if the list is empty */
IM_INT32 im_klist_size(IN im_klist_handle_t hlst);
void * im_klist_get(IN im_klist_handle_t hlst, OUT IM_UINT32 *key);
void * im_klist_begin(IN im_klist_handle_t hlst, OUT IM_UINT32 *key);
void * im_klist_next(IN im_klist_handle_t hlst, OUT IM_UINT32 *key);
void * im_klist_prev(IN im_klist_handle_t hlst, OUT IM_UINT32 *key);
void * im_klist_end(IN im_klist_handle_t hlst, OUT IM_UINT32 *key);
void * im_klist_key(IN im_klist_handle_t hlst, IN IM_UINT32 key);
IM_RET im_klist_put(IN im_klist_handle_t hlst, IN IM_UINT32 key, IN void *data);
void * im_klist_erase(IN im_klist_handle_t hlst, IN IM_UINT32 key);/* remove one entry; returns iterator at next node*/
void im_klist_clear(IN im_klist_handle_t hlst);/* remove all contents of the list */

//
//
//
typedef void * im_ring_handle_t;

im_ring_handle_t im_ring_init(IN IM_INT32 dataSize, IN IM_INT32 depth, IN im_mempool_handle_t mpl);
IM_RET im_ring_deinit(IN im_ring_handle_t hring);
IM_RET im_ring_put(IN im_ring_handle_t hring, IN void *data, OUT void *overflow);
IM_RET im_ring_get(IN im_ring_handle_t hring, OUT void *data);
IM_INT32 im_ring_size(IN im_ring_handle_t hring);
IM_BOOL im_ring_empty(IN im_ring_handle_t hring);
void im_ring_clear(IN im_ring_handle_t hring);

#ifdef __cplusplus
}
#endif

#endif	// __IM_LIST_C_H__


