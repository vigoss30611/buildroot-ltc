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
** v1.1.0	leo@2012/03/17: first commit.
** v1.2.2	leo@2012/04/13: remove alc_map_useraddr() and alc_unmap_useraddr(), because these don't tested.
** v1.2.2	leo@2012/05/02: 
** 				1. support and tested alc_map_useraddr() and alc_unmap_useraddr() ok. 
**				2. add alc_flush_buffer() function.
** v1.2.4	leo@2012/05/16: add ALC_FLAG_CACHE to support alloc physical(linear/nonliear) buffer with cache.
** v1.2.9   leo@2012/09/10: add big-memory support, ALC_FLAG_BIGMEM.
** v1.2.12  leo@2012/11/30: modify alc_init_bigmem() interface.
** v2.1.0   leo@2013/11/11: add ALC_BUFFER_ATTRI_REFCNT in attri of alc_buffer_t to protect buffer from unsafe
**              released, the implementation please see pmm_lib.c.
** v3.0.0   leo@2013/12/23: remove support for bigmem, map_useraddr, statc. add support for "uid", which is 
**              system-wide identity for every buffer, can operate through alc_get_uid() and alc_map_uid().
**              Be careful, that only ALC_FLAG_[PHY_MUST/DEVADDR/PHYADDR] has uid, in other words, the pure-virtual
**              buffer has no "uid".
**          leo@2013/12/25: add alc_alloc2() to get buffer, devaddr, uid all in one time.
**              add output parameter of "devAddr" in alc_map_uid() to get the devAddr with the buffer together.
** v4.0.0   leo@2013/12/27: for IM_Buffer has been enhanced, remove those spare interfaces.
**          leo@2013/12/30: add flag in alc_map_uid() to set the properties of mapping.
**
*****************************************************************************/ 

#ifndef __IM_BUFFALLOCAPI_H__
#define __IM_BUFFALLOCAPI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WINCE_)
#ifdef BUFFALLOC_EXPORTS
	#define BUFFALLOC_API		__declspec(dllexport)	/* For dll lib */
#else
	#define BUFFALLOC_API		__declspec(dllimport)	/* For application */
#endif
#else
	#define BUFFALLOC_API
#endif	
//#############################################################################
// buffalloc context type.
typedef void * ALCCTX;


//
// allocate properties.
//
#define ALC_FLAG_DEVADDR            (1<<0)  // request the devaddr, used for dev-mmu.
#define ALC_FLAG_PHYADDR            (1<<1)  // a physical linear memory block, will get from IM_Buffer.phy_addr.

#define ALC_FLAG_ALIGN_4BYTES		(1<<4)
#define ALC_FLAG_ALIGN_8BYTES		(1<<5)
#define ALC_FLAG_ALIGN_32BYTES		(1<<6)
#define ALC_FLAG_ALIGN_64BYTES		(1<<7)

//
#define ALC_DEVADDR_INVALID         (0xbeadadde)


/*============================Interface API==================================*/
/* 
 * FUNC: get conert version.
 * PARAMS: ver_string, save this version string.
 * RETURN: [31:16] major version number, [15:0] minor version number.
 */
BUFFALLOC_API IM_UINT32 alc_version(OUT IM_TCHAR *ver_string);

/* 
 * FUNC: open a buffer allocator.
 * PARAMS:
 * 	inst, save buffalloc context.
 * 	owner, owner of this buffalloc, max ALC_OWNER_LEN_MAX char.
 * RETURN: IM_RET_OK is successful, else failed.
 */
#define ALC_OWNER_LEN_MAX	16
BUFFALLOC_API IM_RET alc_open(OUT ALCCTX *inst, IN IM_TCHAR *owner);

/* 
 * FUNC: close buffalloc.
 * PARAMS: inst, buffalloc context created by alc_open().
 * RETURN: IM_RET_OK is successful, else failed.
 */
BUFFALLOC_API IM_RET alc_close(IN ALCCTX inst);

/*
 * FUNC: allocate a buffer.
 * PARAMS: inst, buffalloc context created by alc_open();
 * 	size, requested size.
 *	flag, allocate properties.
 *	buffer, allocated buffer infomation.
 * RETURN: IM_RET_OK is successful, else failed.
 */
BUFFALLOC_API IM_RET alc_alloc(IN ALCCTX inst, IN IM_INT32 size, IN IM_INT32 flag, OUT IM_Buffer *buffer);

/*
 * FUNC: free buffer.
 * PARAMS: inst, buffalloc context created by alc_open();
 *	buffer, previously allocated or mapped buffer.
 * RETURN: IM_RET_OK is successful, else failed. 
 */
BUFFALLOC_API IM_RET alc_free(IN ALCCTX inst, IN IM_Buffer *buffer);

/*
 * FUNC: map a universal id to get it's buffer.
 * PARAMS: inst, buffalloc instance.
 *  buf_uid, buffer's universal id got by alc_alloc() before from same process or other process.
 *  flag, map properties, now support ALC_FLAG_DEVADDR and ALC_FLAG_PHYADDR.
 *  buffer, will save the buffer of the uid.
 * RETURN: IM_RET_OK is successful, else failed.
 * NOTE: in end using of this buffer, it must be freed by alc_unmap_uid(buf_uid) or alc_free(buffer).
 *  the mapped buffer may be not fully satisfied the request of "flag", user must check the "buffer->flag" again.
 */
BUFFALLOC_API IM_RET alc_map_uid(IN ALCCTX inst, IN IM_INT32 buf_uid, IN IM_INT32 flag, OUT IM_Buffer *buffer);
BUFFALLOC_API IM_RET alc_unmap_uid(IN ALCCTX inst, IN IM_INT32 buf_uid);


//#############################################################################
#ifdef __cplusplus
}
#endif

#endif	// __IM_BUFFALLOCAPI_H__

