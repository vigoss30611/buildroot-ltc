#ifndef _IROM_TT_H__
#define _IROM_TT_H__

#define TT_RND_POOL		(DRAM_BASE_PA)
#define TT_CMP_BUFF		(TT_RND_POOL + 0x1000000)

extern int tt_rnd_get_int(int a, int b);
extern void * tt_rnd_get_data(int *retlen, int a, int b);
extern int tt_cmp_buffer(uint8_t *b1, uint8_t *b2, int len, int errcount);

#endif /* _IROM_TT_H__ */

