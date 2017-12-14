
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fr/libfr.h>
#include <list.h>
#include <string.h>
#include <errno.h>


#define log(x...) printf("frlib: " x)
/*
used by app that invoke fr_get_buf_by_name or fr_get_ref_by_name
only map and malloc when phy is never mmaped
no unmap and free, resource release depends process exit
may be mem leak default not enable
//#define FR_NEW_MAP 0 make menuconfig set
*/
#ifndef FR_NEW_MAP
#define FR_NEW_MAP 0
#endif
#define assert_fd() do { if(__assert_fd() < 0) return 0;} while(0)

static int fd = -1;
static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct fr_buf_info list_of_bufs;
#if FR_NEW_MAP
ST_BUFFER_MAP_INFO *p_stFloatBufferHeader = NULL;
ST_BUFFER_MAP_INFO *p_stFixedBufferHeader = NULL;
ST_BUFFER_MAP_INFO *p_stVancantBufferHeader = NULL;

extern int fr_get_virtual_address(ST_BUFFER_MAP_INFO **p_stBufferMapHeader, struct fr_buf_info *pBuf, int s32Flag,  unsigned char u8Ref);
#endif

int fr_alloc(struct fr_info *fr, const char *name, int size, int flag)
{
	int ret;

	assert_fd();

	strncpy(fr->name, name, FR_INFO_SZ_NAME);
	fr->flag = flag;
	fr->size = size;

//	log("allocaing: %d\n", fr->size);
	ret = ioctl(fd, FRING_ALLOC, fr);
	if(ret < 0) {
		log("failed to alloc buffer: %s\n", fr->name);
		return ret;
	}

	return 0;
}

int fr_free(struct fr_info *fr)
{
	assert_fd();
	return ioctl(fd, FRING_FREE, fr->fr);
}

int fr_get_buf(struct fr_buf_info *buf)
{
    int ret;

    assert_fd();
    ret = ioctl(fd, FRING_GET_BUF, buf);
    if(ret < 0) return ret;

    if ((buf->flag & FR_FLAG_NEWMAPVIRTUAL) != 0)
    {
        buf->virt_addr = NULL;
        return 0;
    }

    if ((buf->flag & FR_FLAG_VACANT) == 0)
    {
        buf->virt_addr = mmap(0, buf->map_size, PROT_READ | PROT_WRITE,
            MAP_SHARED, fd, buf->phys_addr);
        if (buf->virt_addr == (void *)-1) {
            log("mmap fail buf->phys_addr:%08x, map_size:%08x errono:%s\n",
            buf->phys_addr, buf->map_size, strerror(errno));
        }
    }
    return 0;
}

int fr_put_buf(struct fr_buf_info *buf)
{
    int ret;

    assert_fd();
    ret = ioctl(fd, FRING_PUT_BUF, buf);
    if(ret < 0) {
        log("put buffer failed %d\n", ret);
        return ret;
    }
    if ((buf->flag & FR_FLAG_NEWMAPVIRTUAL) != 0)
    {
        return 0;
    }

    if ((buf->flag & FR_FLAG_VACANT) == 0)
    {
        ret = munmap(buf->virt_addr, buf->map_size);
        if (ret < 0) {
            log("munmap fail buf->phys_addr:%08x, map_size:%08x errono:%s\n",
                buf->phys_addr, buf->map_size, strerror(errno));
        }
    }

    return 0;
}

int fr_get_ref(struct fr_buf_info *ref)
{
    int ret;

    assert_fd();
    ret = ioctl(fd, FRING_GET_REF, ref);
    if(ret < 0) {
        /* set ref handle to NULL if wiped,
         * this enables to get the latest buffer
         * in next try.
         */
        if(ret == FR_EWIPED) ref->buf = NULL;
        return ret;
    }

    if ((ref->flag & FR_FLAG_NEWMAPVIRTUAL) != 0)
    {
       ref->virt_addr = NULL;
       return 0;
    }

    ref->virt_addr = mmap(0, ref->map_size, PROT_READ | PROT_WRITE,
        MAP_SHARED, fd, ref->phys_addr);
    return 0;
}

int fr_put_ref(struct fr_buf_info *ref)
{
    int ret;

    assert_fd();
    ret = ioctl(fd, FRING_PUT_REF, ref);
    if(ret < 0) return ret;
    if ((ref->flag & FR_FLAG_NEWMAPVIRTUAL) != 0)
    {
       return 0;
    }
    munmap(ref->virt_addr, ref->map_size);

    return 0;
}

int fr_flush_buf(struct fr_buf_info *buf)
{
	int ret;

	assert_fd();
	ret = ioctl(fd, FRING_FLUSH_BUF, buf);
	if(ret < 0) {
		log("flush buffer failed %d\n", ret);
		return ret;
	}

	return 0;
}

int fr_inv_buf(struct fr_buf_info *buf)
{
	int ret;

	assert_fd();
	ret = ioctl(fd, FRING_INV_BUF, buf);
	if(ret < 0) {
		log("inv buffer failed %d\n", ret);
		return ret;
	}

	return 0;
}


int fr_set_timeout(struct fr_info *fr, int timeout_in_ms)
{
    int ret;

    assert_fd();
    fr->timeout = timeout_in_ms;
    ret = ioctl(fd, FRING_SET_TIMEOUT, fr);
    return ret;
}

int fr_test_new(struct fr_buf_info *ref) {

	assert_fd();

	return ioctl(fd, FRING_TST_NEW, ref);
}

int fr_float_setmax(struct fr_info *fr, int max)
{
	int ret;

	assert_fd();
	fr->float_max = max;
	ret = ioctl(fd, FRING_SET_FLOATMAX, fr);
	return ret;
}

int __assert_fd(void)
{
	int ver;
	int ret;
	unsigned long cc = CC_BUFFER;

	if(fd < 0)
	  fd = open(FRING_NODE, O_RDWR);
	else
	  return 0;

	if(fd < 0) {
		log("faild to open %s\n", FRING_NODE);
		return -1;
	}

	ver = ioctl(fd, FRING_GET_VER, NULL);
	if(ver != FRLIB_VERSION) {
		log("frlib version: %d, not compatible with fr driver: %d\n",
					FRLIB_VERSION, ver);
		close(fd);
		return -1;
	}

	list_init(list_of_bufs);
#if defined(FR_CC_BUFFER)
	cc = CC_BUFFER;
#elif defined(FR_CC_WRITETHROUGH)
	cc = CC_WRITETHROUGH;
#elif defined(FR_CC_WRITEBACK)
	cc = CC_WRITEBACK;
#endif
#if defined(FR_CC_BUFFER) || defined(FR_CC_WRITETHROUGH) || defined (FR_CC_WRITEBACK)
	ret = ioctl(fd, FRING_SET_CACHE_POLICY, &cc);
	if (ret != 0) {
		log("frlib set cache policy failed:%d\n", ret);
		return -1;
	}
	//printf("frlib set cache policy %d success\n", cc);
#endif
	return 0;
}

void fr_INITBUF(struct fr_buf_info *buf, struct fr_info *fr)
{
	memset(buf, 0, sizeof(*buf));
	buf->fr = fr? fr->fr: NULL;
	buf->flag = fr? fr->flag: 0;
}

void fr_STAMPBUF(struct fr_buf_info *buf)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	buf->timestamp = ts.tv_sec * 1000ull + ts.tv_nsec / 1000000;
}

void *fr_alloc_single(const char *name, int size, uint32_t *phys)
{
	struct fr_info fr;
	int ret;

	assert_fd();

	ret = fr_alloc(&fr, name, size, FR_FLAG_RING(1));
	if(ret < 0)
	  goto failed;

	struct fr_buf_info *buf = malloc(sizeof(struct fr_buf_info));
	if(!buf) {
		log("failed to alloc fr_buf_info node\n");
		goto failed;
	}

	fr_INITBUF(buf, &fr);
	ret = fr_get_buf(buf);
	if(ret < 0) {
		log("failed to get buffer from fr %s\n", fr.name);
		free(buf);
		goto failed;
	}

	*phys = buf->phys_addr;
	pthread_mutex_lock(&list_mutex);
	list_add_tail(buf, &list_of_bufs);
	pthread_mutex_unlock(&list_mutex);

	return buf->virt_addr;

failed:
	*phys = 0;
	return NULL;
}

int fr_free_single(void *virt, uint32_t phys)
{
	struct fr_buf_info *buf, *tmp;

	assert_fd();

	pthread_mutex_lock(&list_mutex);

	list_for_del(buf, list_of_bufs) {

		list_for_del_next(buf, tmp);
		if((phys && (phys == tmp->phys_addr)) ||
			(virt && (virt == tmp->virt_addr))) {
			fr_put_buf(tmp);
			ioctl(fd, FRING_FREE, tmp->fr);
			list_del(tmp);
			free(tmp);
		}
	}

	pthread_mutex_unlock(&list_mutex);
	return 0;
}

int fr_get_buf_by_name(const char *name, struct fr_buf_info *buf) {
    struct fr_info fr;
    int ret;
    ST_BUFFER_MAP_INFO **pstHeader = NULL;

    assert_fd();

    strcpy(fr.name, name);
    ret = ioctl(fd, FRING_GET_FR, &fr);
    if(ret < 0) {
        log("%s not exist\n", name);
        return ret;
    }

    fr_INITBUF(buf, &fr);
    #if FR_NEW_MAP
    buf->flag |= FR_FLAG_NEWMAPVIRTUAL;
    #endif
    ret = fr_get_buf(buf);
    #if FR_NEW_MAP
    if(ret == 0)
    {
        if(buf->flag & FR_FLAG_FLOAT)
        {
            pstHeader = &p_stFloatBufferHeader;
        }
        else  if((buf->flag & FR_FLAG_VACANT) == 0 )
        {
            pstHeader = &p_stFixedBufferHeader;
        }
        else
        {
            //vancat not process
            pstHeader = NULL;
        }
        if(pstHeader != NULL)
        {
            if(fr_get_virtual_address(pstHeader,buf,buf->flag, 0) == -1)
            {
                return -1;
            }
        }
    }
    #endif
    return ret;
}

int fr_get_ref_by_name(const char *name, struct fr_buf_info *ref) {
    struct fr_info fr;
    int ret;
    int flag = ref->oldest_flag;
    ST_BUFFER_MAP_INFO **pstHeader = NULL;
    assert_fd();

    if(!ref->fr) {
        strcpy(fr.name, name);
        ret = ioctl(fd, FRING_GET_FR, &fr);
        if(ret < 0) {
            log("%s not exist\n", name);
            return ret;
        }
        fr_INITBUF(ref, &fr);
    }
    ref->oldest_flag = flag;
    #if FR_NEW_MAP
    ref->flag |= FR_FLAG_NEWMAPVIRTUAL;
    #endif
    ret = fr_get_ref(ref);
    #if FR_NEW_MAP
    if(ret == 0)
    {
        if(ref->flag & FR_FLAG_FLOAT)
        {
            pstHeader = &p_stFloatBufferHeader;
        }
        else if((ref->flag & FR_FLAG_VACANT) == 0)
        {
            pstHeader = &p_stFixedBufferHeader;
        }
        else
        {
            pstHeader = &p_stVancantBufferHeader;
        }
        if(fr_get_virtual_address(pstHeader,ref,ref->flag, 1) == -1)
        {
            return -1;
        }
    }
    #endif
    return ret;
}

int fr_test_new_by_name(const char *name, struct fr_buf_info *ref) {
	struct fr_info fr;
	struct fr_buf_info fr_buf;
	int ret;
	assert_fd();

	if(!ref || !ref->fr) {
		strcpy(fr.name, name);
		ret = ioctl(fd, FRING_GET_FR, &fr);
		if(ret < 0) {
			log("%s not exist\n", name);
			return ret;
		}

		if(!ref) ref = &fr_buf;
		fr_INITBUF(ref, &fr);
	}

	return fr_test_new(ref);
}
#if FR_NEW_MAP
int fr_get_virtual_address(ST_BUFFER_MAP_INFO **p_stBufferMapHeader, struct fr_buf_info *pBuf, int s32Flag, unsigned char u8Ref)
{
    ST_BUFFER_MAP_INFO *pstTemp = *p_stBufferMapHeader, *pstPre = NULL;
    unsigned char u8NeedNewNode = 0;
    int s32MapSize = 0;
    uint32_t u32MapPhyAddr = 0;

    if(s32Flag & FR_FLAG_FLOAT)
    {
        if(pstTemp == NULL)
        {
            u8NeedNewNode = 1;
            //log("float buffer  first time to mmap phy:0x%x size:0x%x\n",pBuf->u32BasePhyAddr, pBuf->s32TotalSize);
        }
        else
        {
            for(pstTemp = *p_stBufferMapHeader; pstTemp != NULL; pstTemp = pstTemp->next)
            {
                if(pBuf->phys_addr >= pstTemp->u32PhyAddr && pBuf->phys_addr < (pstTemp->u32PhyAddr + pstTemp->s32size))
                {
                    if((pBuf->phys_addr + pBuf->map_size) <=  (pstTemp->u32PhyAddr + pstTemp->s32size))
                    {
                        pBuf->virt_addr = pstTemp->pVirtualAddr + pBuf->phys_addr - pstTemp->u32PhyAddr;
                        return 0;
                    }
                    else
                    {
                        //log("float buffer changed Get(phy:0x%x map_size:0x:%x)  PreviousBase(phy:0x%x totalsize:0x%x) GetBase(phy:0x%x totalsize:0x%x)\n",
                        //    pBuf->phys_addr, pBuf->map_size, pstTemp->u32PhyAddr, pstTemp->s32size, pBuf->u32BasePhyAddr, pBuf->s32TotalSize);
                        if(fr_munmap(pstTemp->pVirtualAddr, pstTemp->s32size) != 0)
                        {
                             log("error: float buffer fr_munmap error\n");
                        }
                        pstTemp->pVirtualAddr = fr_mmap(pBuf->u32BasePhyAddr, pBuf->s32TotalSize);
                        if (pstTemp->pVirtualAddr == (void *)-1)
                        {
                            pstPre->next = pstTemp->next;//fr_mmap error delete this node
                            free(pstPre);
                            log("fatal error: float buffer fr_mmap error\n");
                            return -1;
                        }
                        pstTemp->u32PhyAddr = pBuf->u32BasePhyAddr;
                        pBuf->virt_addr= pstTemp->pVirtualAddr + pBuf->phys_addr - pstTemp->u32PhyAddr;
                        pstTemp->s32size = pBuf->s32TotalSize;
                        return 0;
                    }
                }
                pstPre = pstTemp;
            }
            if(pstTemp  == NULL)
            {
                 u8NeedNewNode = 1;
                 //log("float buffer  add new mmap phy:0x%x size:0x%x\n",pBuf->u32BasePhyAddr, pBuf->s32TotalSize);
            }
        }
        s32MapSize = pBuf->s32TotalSize;
        u32MapPhyAddr = pBuf->u32BasePhyAddr;
    }
    else if(((s32Flag & FR_FLAG_VACANT) == 0) ||(u8Ref == 1 && (s32Flag & FR_FLAG_VACANT)))
    {
        if(pstTemp == NULL)
        {
            //log("vancant buffer(%x)  first time to mmap phy:0x%x size:0x%x\n", s32Flag & FR_FLAG_VACANT, pBuf->phys_addr, pBuf->map_size);
            u8NeedNewNode = 1;
        }
        else
        {
             for(pstTemp = *p_stBufferMapHeader; pstTemp != NULL; pstTemp = pstTemp->next)
             {
                 if(pstTemp->u32PhyAddr == pBuf->phys_addr)
                 {
                     if(pstTemp->s32size == pBuf->map_size)
                     {
                         pBuf->virt_addr= pstTemp->pVirtualAddr;
                         return 0;
                     }
                     else
                     {
                         //log("vancant buffer(%x) changed Get(phy:0x%x map_size:0x:%x)  PreviousBase(phy:0x%x totalsize:0x%x)\n",
                         //   s32Flag & FR_FLAG_VACANT, pBuf->phys_addr, pBuf->map_size, pstTemp->u32PhyAddr, pstTemp->s32size);
                         if(fr_munmap(pstTemp->pVirtualAddr, pstTemp->s32size) != 0)
                         {
                              log("error: fixed buffer fr_munmap error\n");
                         }
                         pstTemp->pVirtualAddr = fr_mmap(pBuf->phys_addr, pBuf->map_size);
                         if (pstTemp->pVirtualAddr == (void *)-1)
                         {
                             pstPre->next = pstTemp->next;//fr_mmap error delete this node
                             free(pstPre);
                             log("size change fatal error: fixed buffer fr_mmap error\n");
                             return -1;
                         }
                         pstTemp->u32PhyAddr = pBuf->phys_addr;
                         pBuf->virt_addr= pstTemp->pVirtualAddr;
                         pstTemp->s32size = pBuf->map_size;
                         return 0;
                     }
                 }
                 pstPre = pstTemp;
             }
             if(pstTemp == NULL)
             {
                 u8NeedNewNode = 1;
                 //log("vancant buffer(%x)  Add new to mmap phy:0x%x size:0x%x\n", s32Flag & FR_FLAG_VACANT, pBuf->phys_addr, pBuf->map_size);
            }
        }
        s32MapSize = pBuf->map_size;
        u32MapPhyAddr = pBuf->phys_addr;
    }
    if(u8NeedNewNode == 1)
    {
        pstTemp = (ST_BUFFER_MAP_INFO *)malloc(sizeof(ST_BUFFER_MAP_INFO));
        if(pstTemp == NULL)
        {
            log("fatal error: buffer node malloc error\n");
            return -1;
        }
        if(pstPre != NULL)
        {
            pstPre->next = pstTemp;
        }
        else
        {
            *p_stBufferMapHeader = pstTemp;
        }
        memset(pstTemp, 0, sizeof(ST_BUFFER_MAP_INFO));
        pstTemp->next = NULL;
        pstTemp->pVirtualAddr = fr_mmap(u32MapPhyAddr, s32MapSize);
        if (pstTemp->pVirtualAddr == (void *)-1)
        {
            log("new node fatal error: buffer fr_mmap error\n");
            return -1;
        }
        //log("buffer flag(%x) new_node_map(phy:0x%x map_size:0x:%x)\n", s32Flag, u32MapPhyAddr, s32MapSize);

        pstTemp->u32PhyAddr = u32MapPhyAddr;
        pstTemp->u8status = 1;
        pstTemp->s32size = s32MapSize;
        if(s32Flag & FR_FLAG_FLOAT)
        {
            pBuf->virt_addr = pstTemp->pVirtualAddr + pBuf->phys_addr - pstTemp->u32PhyAddr;
        }
        else
        {
            pBuf->virt_addr = pstTemp->pVirtualAddr;
        }
    }
    return 0;
}
#endif

/*
 * @brief   mmap Physical Address to Virtual Address to Enable CPU Access
                Only for Debug  and Must be In Pair With  fr_munmap
 * @param u32PhysAddr -IN: Physical Address that need Mmap
 * @param u32Size -IN: Size of Physical Address
 * @return void* Virtual Address that mmap
 */
void *fr_mmap(uint32_t u32PhysAddr, uint32_t u32Size)
{
    assert_fd();
    return mmap(0, u32Size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, u32PhysAddr);
}

/*
 * @brief   munmap Previous mmap Virtual Address to Release Source
                Only for Debug and Must be In Pair With  fr_mmap
 * @param pVirtAddr -IN: Virtual Address that need munmap
 * @param u32Size -IN: Size of Virtual Address
 * @return int  0:Sucess -1 Fail
 */
int fr_munmap(void *pVirtAddr, uint32_t u32Size)
{
    assert_fd();
    return munmap(pVirtAddr, u32Size);
}
