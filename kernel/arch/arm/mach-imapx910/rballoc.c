
/* simplest memory alloc method, by warits */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/bootmem.h>
#include <mach/items.h>
#include <mach/rballoc.h>
#include <mach/mem-reserve.h>
#include <mach/imap-iomap.h>
#define rbprintf(x...) pr_err(x)

/* these memorys is not reserved indeed. they contain value created in uboot.
 * driver read & save these values during boot time.
 * In fact, linux do not know we use these memorys for special purpose and is
 * still in charge of managing these memorys.
 *
 * -- by warits, Oct 18, 2012
 */
#define ___RBASE   phys_to_virt(IMAP_SDRAM_BASE + 0x10800000)
#define RB_BLOCK 0x8000
/*
 * |<--- 32K --->|<--- else --->|
 *    head array    buffer repo
 */

#if 0
void rbinit(void)
{
	memset((void *)___RBASE, 0, RB_BLOCK);
}

void *rballoc(const char *owner, uint32_t len)
{
	struct reserved_buffer *p = (void *)___RBASE;
	uint32_t used = 0;

	if (!owner || !len)
		return NULL;

	for (; *p->owner; p++) {
		if (strncmp(p->owner, owner, 32) == 0) {
			rbprintf("rballoc: shared owner (%s) 0x%x@0x%08x\n",
				 owner, p->length, p->start);
			return (void *)p->start;
		}

		used += p->length;
	}

	if (RB_BLOCK + used + len > ___RLENGTH) {
		rbprintf("rballoc: OOM, 0x%x bytes denied.\n", len);
		return NULL;
	}

	strncpy(p->owner, owner, 32);
	p->start = ___RBASE + RB_BLOCK + used;
	p->length = (len + 0xfff) & ~0xfff;

	rbprintf("rballoc: 0x%x@0x%08x allocated for %s\n", p->length,
		 p->start, p->owner);

	return (void *)p->start;
}

void rbsetint(const char *owner, uint32_t val)
{
	uint8_t *p = rballoc(owner, 0x1000);

	if (p)
		*(uint32_t *) p = val;
}
#endif

void *rbget(const char *owner)
{
	struct reserved_buffer *p = (void *)___RBASE;

	if (!owner)
		return NULL;

	for (; *p->owner; p++) {
		if (strncmp(p->owner, owner, 32) == 0)
			return phys_to_virt(p->start);
	}

	return NULL;
}

uint32_t rbgetint(const char *owner)
{
	uint8_t *p = rbget(owner);

	return p ? *(uint32_t *) p : 0;
}

void rblist(void)
{
	struct reserved_buffer *p = (void *)___RBASE;

	for (; *p->owner; p++) {
		rbprintf("%16s 0x%08x=>0x%p\n", p->owner,
			 p->start, phys_to_virt(p->start));
	}
}
