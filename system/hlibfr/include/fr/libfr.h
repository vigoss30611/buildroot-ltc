#ifndef __FR_LIB_H__
#define __FR_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include <fr/fr-user.h>
#define FRLIB_VERSION 18

typedef struct _buffer_map_info
{
    unsigned char u8status; // 0: unmapped 1:maped
    int s32size;
    void *pVirtualAddr;
    uint32_t u32PhyAddr;
   struct _buffer_map_info *next;
}ST_BUFFER_MAP_INFO;

typedef void * fr_handle;

extern int fr_alloc(struct fr_info *fr, const char *name, int size, int flag);
extern int fr_free(struct fr_info *fr);
extern int fr_get_buf(struct fr_buf_info *fbi);
extern int fr_put_buf(struct fr_buf_info *fbi);
extern int fr_flush_buf(struct fr_buf_info *fbi);
extern int fr_inv_buf(struct fr_buf_info *fbi);
extern int fr_get_ref(struct fr_buf_info *ref);
extern int fr_put_ref(struct fr_buf_info *ref);
extern int fr_set_timeout(struct fr_info *fr, int timeout_in_ms);
extern int fr_test_new(struct fr_buf_info *ref);
extern void fr_INITBUF(struct fr_buf_info *buf, struct fr_info *fr);
extern void fr_STAMPBUF(struct fr_buf_info *buf);
extern void *fr_alloc_single(const char *name, int size, uint32_t *phys);
extern int fr_free_single(void *virt, uint32_t phys);
extern int fr_float_setmax(struct fr_info *fr, int max);
extern int fr_get_buf_by_name(const char *name, struct fr_buf_info *buf);
extern int fr_get_ref_by_name(const char *name, struct fr_buf_info *ref);
extern int fr_test_new_by_name(const char *name, struct fr_buf_info *ref);
extern void *fr_mmap(uint32_t u32PhysAddr, uint32_t u32Size);
extern int fr_munmap(void *pVirtAddr, uint32_t u32Size);


#define FR_BUFINITIALIZER { .fr = NULL, .buf = NULL, .serial = 0, .priv = 0}


#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __FR_LIB_H__ */

