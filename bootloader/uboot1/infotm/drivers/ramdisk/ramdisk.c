#include <linux/types.h>
#include <common.h>
#include <vstorage.h>
#include <asm/io.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <ius.h>

int ram_vs_read(uint8_t *buf, loff_t offs, int len, uint32_t extra)
{
	uint8_t *src = (uint8_t *)(uint32_t)(offs & 0xffffffff)
		+ (uint32_t)DRAM_BASE_PA - (uint32_t)IUS_DEFAULT_LOCATION;

	if(offs < IUS_DEFAULT_LOCATION) {
		printf("access to low %dM of ramdisk dennied.\n", IUS_DEFAULT_LOCATION >> 20);
		return 0;
	}

#if 0
	if(addr_is_dram(src) && !cdbg_dram_enabled())
	  return -EFAILED;
#endif

	memcpy(buf, src, len);
	return len;
}

int ram_vs_write(uint8_t *buf, loff_t offs, int len, uint32_t extra)
{
	uint8_t *dst = (uint8_t *)(uint32_t)(offs & 0xffffffff)
		+ (uint32_t)DRAM_BASE_PA - (uint32_t)IUS_DEFAULT_LOCATION;

	if(offs < IUS_DEFAULT_LOCATION) {
		printf("access to low %dM of ramdisk dennied.\n", IUS_DEFAULT_LOCATION >> 20);
		return 0;
	}

#if 0
	if(addr_is_dram(dst) && !cdbg_dram_enabled())
	  return -EFAILED;
#endif

	memcpy(dst, buf, len);
	return len;
}

int ram_vs_align(void) {
	return 0x200;
}

int ram_vs_reset(void)
{
#if 0
	if(addr_is_dram(src) && !cdbg_dram_enabled())
	  return -EFAILED;
#endif

	return 0;
}

