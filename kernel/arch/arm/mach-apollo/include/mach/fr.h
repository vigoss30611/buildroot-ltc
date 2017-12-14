#ifndef __FRING_H__
#define __FRING_H__

enum fr_timest_stage {
	FRTIME_ST_GET = 0,
	FRTIME_ST_WAIT,
	FRTIME_ST_DEAL,
	FRTIME_ST_PUT,
	FRTIME_ST_COUNT,
};

#define FRSTFRAMES 64
#define FRSTMAXREF 4

struct fr_statistics {
	pid_t pid;

	int tp[FRTIME_ST_COUNT];
	int id[FRTIME_ST_COUNT];
	int ti[FRSTFRAMES][FRTIME_ST_COUNT];
	int count;
};

struct fr_buf {
	int serial;
	int size;
	void * virt_addr;
	dma_addr_t phys_addr;
	uint64_t timestamp;
	int ref_serial;
	int priv;

	spinlock_t state_lock;
	int state;
	int ref_count;
	wait_queue_head_t state_change;
	struct list_head node;
};

enum fr_buf_states {
	FR_BUFST_INIT = 0,
	FR_BUFST_WRITING,
	FR_BUFST_READING,
	FR_BUFST_READY,
	FR_BUFST_READ,
	FR_BUFST_COUNT, /* tatal states count */
};

#define FR_SZ_NAME 32
struct fr {
	char name[FR_SZ_NAME];
	int flag;
	int size;
	int float_max;
	int serial_inc;
	int *buf_st_switch;
	struct fr_buf *ring;
	struct fr_buf *latest;
	struct fr_buf *oldest;
	struct fr_buf *cur;
	struct list_head node;
	wait_queue_head_t serial_update;
	wait_queue_head_t new_float;

	struct fr_statistics st_buf;
	struct fr_statistics st_ref[FRSTMAXREF];

	spinlock_t fvlock; /* float variables lock */
	struct mutex wlock;
	int state;

	pid_t owner_pid;
	void *fp;
};
#define	FR_FLAG_RING(n) (n & 0xff)
#define	FR_FLAG_MAX(n) (n & 0xffffff)
#define	FR_FLAG_FLOAT (1 << 24)
#define FR_FLAG_NODROP (1 << 25)
#define FR_FLAG_VACANT (1 << 26)

#define FR_ISVACANT(f) (f->flag & FR_FLAG_VACANT)
#define FR_ISFLOAT(f) (f->flag & FR_FLAG_FLOAT)
#define FR_GETCOUNT(f) (f->flag & 0xff)
#define FR_GETFLOATMAX(f) (f->flag & 0xffffff)

#define FRING_MAJOR 173
#define FRING_NAME "fring"
#define FRING_VERSION 9

extern struct fr* __fr_alloc(const char *, int, int);

extern int __fr_free(struct fr *);
extern void __fr_free_fp(void *);
extern void __fr_clean(void);
extern void fr_listall(void);
extern void fr_buf_print(struct fr_buf *buf);
extern void fr_print(struct fr *fr);
extern struct fr * fr_get_fr_by_name(const char *name);

extern struct fr_buf* fr_get_buf(struct fr* fr);
extern int fr_put_buf(struct fr* fr, struct fr_buf* buf);
extern struct fr_buf* fr_get_ref(struct fr* fr, struct fr_buf* ref, int *ret);
extern int fr_put_ref(struct fr *fr, struct fr_buf *buf);
extern int fr_float_setmax(struct fr *fr, int max);
#define fr_float_valid(f, fb) (((uint32_t)fb >= (uint32_t)f->ring)  \
 && ((uint32_t)fb < (uint32_t)f->ring + f->size)  \
 && !((uint32_t)fb & 0x3)  \
 && ((uint32_t)fb->node.next == ~(uint32_t)fb->node.prev))
static inline int fr_float_is_overlap(struct fr *fr, struct fr_buf *buf,
			void *loc) {
	uint32_t s1 = (uint32_t)buf, e1 = (uint32_t)(buf->virt_addr + buf->size);
	uint32_t s2 = (uint32_t)loc,
			 e2 = (uint32_t)(loc + PAGE_SIZE + fr->float_max);

	return (s1 < e2 && s2 < e1);
}

#endif /* __FRING_H__ */
