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
** v3.0.0   leo@2013/12/26: add IM_Buffer2 structure.
** v4.0.0   leo@2013/12/27: remove IM_Buffer2 structure, and move it's uid and dev_addr
**              to IM_Buffer. So IM_Buffer will has the features before owned by IM_Buffer2.
**              re-design IM_Buffer related things.
** v4.1.0   leo@2014/01/17: add "priv" member in IM_Buffer for private data, and change some
**              macro defination which used for helping IM_Buffer operations.
**
*****************************************************************************/ 

#ifndef __IM_COMMON_H__
#define __IM_COMMON_H__

//
//
//
#define IM_SAFE_DELETE(ptr)     if(ptr != IM_NULL){delete(ptr); ptr=IM_NULL;}
#define IM_SAFE_RELEASE(ptr)    if(ptr != IM_NULL){ptr->release(); ptr=IM_NULL;}

//
// version.
//
#define IM_VERSION_STRING_LEN_MAX       16
#define IM_MAKE_VERSION(major, minor, patch)	(((major & 0xff) << 16) | ((minor & 0xff)<<8) | (patch & 0xff))
#define IM_VER_MAJOR(ver)	((ver >> 16) & 0xff)
#define IM_VER_MINOR(ver)	((ver >> 8) & 0xff)
#define IM_VER_PATCH(ver)	(ver & 0xff)

//
//
//
#define IM_JIF(exp)     \
	if(IM_FAILED(exp)){     \
		IM_ERRMSG((IM_STR("IM_JIF failed")));	\
		goto Fail;	\
	}


//
// IM_Buffer. flag [31:16] reserved, [15:8] attributes of this buffer, [7:0] indicate valid members.
//
#define IM_BUFFER_FLAG_PHY			    (1<<0)	// linear physical memory, recommend use IM_BUFFER_FLAG_PHYADDR.
#define IM_BUFFER_FLAG_PHY_NONLINEAR	(1<<1)	// !!!DEPRECATED!!!
#define IM_BUFFER_FLAG_PHYADDR			(1<<0)	// linear physical memory, equal to IM_BUFFER_FLAG_PHY.
#define IM_BUFFER_FLAG_DEVADDR			(1<<2)	// this buffer has dev-mmu address.

#define IM_BUFFER_FLAG_DMABUF           (1<<8)  // this indicated the buffer allocated from dma-buf.

#define IM_BUFFER_PRIV_LEN              (4)
typedef struct{
    IM_INT32    uid;        // universal buffer id, negative is invalid.
	void *      vir_addr;
	IM_UINT32   phy_addr;   // valid only when flag has bit of IM_BUFFER_FLAG_PHYADDR.   
    IM_UINT32   dev_addr;   // valid only when flag has bit of IM_BUFFER_FLAG_DEVADDR.
	IM_UINT32   size;
	IM_UINT32   flag;

    // private data, its meaning depends on flag, it would means nothing if it's bits has not clearly declared below.
    // IM_BUFFER_FLAG_DMABUF, priv[0] is the dma_buf_fd of this buffer, remains unused.
    IM_INT32    priv[IM_BUFFER_PRIV_LEN];
}IM_Buffer;

#define IM_BUFFER_CLEAR(buffer)     \
    do{ \
        memset((void *)&buffer, sizeof(IM_Buffer), 0);  \
        buffer.uid = -1;    \
        buffer.dev_addr = 0xbeadadde;   \
    }while(0)

#define IM_pBUFFER_CLEAR(buffer)   \
    do{ \
        memset((void *)buffer, sizeof(IM_Buffer), 0);  \
        buffer->uid = -1;    \
        buffer->dev_addr = 0xbeadadde;  \
    }while(0)

#define IM_BUFFER_COPYTO_BUFFER(dst, src)       memcpy((void *)&dst, (void *)&src, sizeof(IM_Buffer))
#define IM_BUFFER_COPYTO_pBUFFER(dst, src)      memcpy((void *)dst, (void *)&src, sizeof(IM_Buffer))
#define IM_pBUFFER_COPYTO_BUFFER(dst, src)      memcpy((void *)&dst, (void *)src, sizeof(IM_Buffer))
#define IM_pBUFFER_COPYTO_pBUFFER(dst, src)     memcpy((void *)dst, (void *)src, sizeof(IM_Buffer))


#endif	// __IM_COMMON_H__

