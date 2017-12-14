#ifndef __FR_USER_H__
#define __FR_USER_H__
#ifndef __KERNEL__
#include <stdint.h>
#endif

#define FR_INFO_SZ_NAME 32
#define FRING_COUNT_VARIABLE -1
struct fr_info {
    char name[FR_INFO_SZ_NAME];
    int size;
    int flag;
    int float_max;
    int timeout;
    uint32_t u32BasePhyAddr;
    void *fr;
};

typedef struct fr_attr
{
    int s32Width;
    int s32Height;
}ST_FR_ATTR;

struct fr_buf_info {
    ST_FR_ATTR stAttr;
    void *fr;
    void *buf;

    void *   virt_addr;
    uint32_t phys_addr;
    uint64_t timestamp;
    int size;
    int map_size;
    int serial;
    int priv;
    int oldest_flag; //0:disable 1:enable
    int flag;
    uint32_t u32BasePhyAddr; //only folat buffer type valid
    int s32TotalSize; //only folat buffer type valid

    /* DO NOT MODIFY */
    struct fr_buf_info *next, *prev;
};

enum {
	CC_BUFFER=0,
	CC_WRITETHROUGH,
	CC_WRITEBACK,
};

struct fr_priv_data{
	unsigned long cache_policy;
};

#define FRING_MAGIC 'r'
#define FRING_NODE "/dev/fring"

#define FRING_ALLOC		_IOW(FRING_MAGIC, 1, unsigned long)
#define FRING_FREE		_IOW(FRING_MAGIC, 2, unsigned long)
#define FRING_GET_BUF	_IOW(FRING_MAGIC, 3, unsigned long)
#define FRING_PUT_BUF	_IOW(FRING_MAGIC, 4, unsigned long)
#define FRING_GET_REF	_IOR(FRING_MAGIC, 5, unsigned long)
#define FRING_GET_REF_LATEST	_IOR(FRING_MAGIC, 6, unsigned long)
#define FRING_PUT_REF	_IOR(FRING_MAGIC, 7, unsigned long)
#define FRING_SET_FLOATMAX	_IOR(FRING_MAGIC, 8, unsigned long)
#define FRING_GET_FR	_IOR(FRING_MAGIC, 9, unsigned long)
#define FRING_TST_NEW	_IOR(FRING_MAGIC, 10, unsigned long)
#define FRING_GET_VER	_IOR(FRING_MAGIC, 11, unsigned long)
#define FRING_SET_TIMEOUT _IOR(FRING_MAGIC, 12, unsigned long)
#define FRING_FLUSH_BUF	_IOW(FRING_MAGIC, 13, unsigned long)
#define FRING_INV_BUF	_IOW(FRING_MAGIC, 14, unsigned long)
#define FRING_SET_CACHE_POLICY	_IOW(FRING_MAGIC, 15, unsigned long)
#define FRING_IOCMAX 15

#define FR_EHANDLE -20002
#define FR_EWIPED -20004
#define FR_ENOBUFFER -20005
#define FR_EINVOPR -20006
#define FR_TIMEOUT -20007
#define FR_MAPERROR -20008
#define FR_ERESTARTSYS -20512

#ifndef __KERNEL__
#define	FR_FLAG_RING(n) (n & 0xff)
#define	FR_FLAG_MAX(n) (n & 0xffffff)
#define	FR_FLAG_FLOAT (1 << 24)
#define FR_FLAG_NODROP (1 << 25)
#define FR_FLAG_VACANT (1 << 26)
#define FR_FLAG_PROTECTED (1 << 27)
#define FR_FLAG_UNPROTECTED (0 << 0)
#define FR_FLAG_NEWMAPVIRTUAL (1 << 28)
#endif

#endif /* __FR_USER_H__ */

