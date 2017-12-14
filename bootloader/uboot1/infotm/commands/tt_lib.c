#include <common.h>
#include <command.h>
#include <vstorage.h>
#include <malloc.h>
#include <div64.h>
#include <linux/mtd/compat.h>
#include <bootlist.h>
#include <tt.h>

#define tt_msg(x...) printf("ttlib: " x)
static uint32_t tt_rnd_pos = 0;

#define RND_MAX 0xffff

void tt_rnd_seed(void) {
	tt_rnd_pos += get_timer(0) & 0xff;
	tt_rnd_pos &= 0xfffc;
}

int tt_rnd_get_int(int a, int b)
{
	uint8_t *rndpool = (uint8_t *)TT_RND_POOL;
	uint16_t pos, num;
	int ret;

	/* get random pos */
	pos = *(uint16_t *)(rndpool + tt_rnd_pos) & ~0x3;
	num = *(uint16_t *)(rndpool + pos);

	/* update global pos */
	tt_rnd_pos  = pos;
	tt_rnd_seed();

	ret = (double)num / (double)RND_MAX * (b - a) + a;
//	tt_msg("rnd: a=%d, b=%d, ret=%d\n", a, b, ret);
	return ret;
}

void * tt_rnd_get_data(int *retlen, int a, int b)
{
	uint8_t *rndpool = (uint8_t *)TT_RND_POOL;

	*retlen = tt_rnd_get_int(a, b);
	return rndpool + (tt_rnd_pos & ~0x3);
}

int tt_cmp_buffer(uint8_t *b1, uint8_t *b2, int len, int errcount)
{
	int a, i, err = 0;

	a = len >> 2;
	for(i = 0; i < a; i ++, b1 += 4, b2 += 4)
	  if(*(uint32_t *)b1 != *(uint32_t *)b2) {
		  if(*(b1 + 0) != *(b2 + 0)) err++;
		  if(*(b1 + 1) != *(b2 + 1)) err++;
		  if(*(b1 + 2) != *(b2 + 2)) err++;
		  if(*(b1 + 3) != *(b2 + 3)) err++;
		  if(errcount) {
			tt_msg("bytes diff@0x%x: 0x%08x <=> 0x%08x\n", i << 2,
						*(uint32_t *)(b1), *(uint32_t *)(b2));
			errcount--;
		  }
	  }

	a = len & 0x3;
	for(i = 0; i < a; i++, b1++, b2++)
	  if(*b1 != *b2) { err++;
		  if(errcount) {
			tt_msg("bytes diff@0x%x: 0x%02x <=> 0x%02x\n", (len & ~0x3) + i,
						*b1, *b2);
			errcount--;
		  }
	  }

	return err;
}


