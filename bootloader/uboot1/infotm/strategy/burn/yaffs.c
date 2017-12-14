
#include <common.h>
#include <vstorage.h>
#include <malloc.h>
#include <iuw.h>
#include <ius.h>
#include <hash.h>
#include <net.h>
#include <udc.h>
#include <led.h>
#include <nand.h>
#include <cdbg.h>
#include <crypto.h>
#include <sparse.h>
#include <irerrno.h>
#include <storage.h>
#include <yaffs.h>

struct yaffs_conv_info conv;

loff_t yaffs_offs;
int yaffs_part;

int yaffs_put_page(uint8_t *buf)
{
	int block;
	struct nand_config *nc;

	if(yaffs_part <= 0) {
		printf("no space left for yaffs page (%d).\n", yaffs_part);
		return -1;
	}

#if 0	
	nc = nand_get_config(NAND_CFG_NORMAL);
	if(!nc) {
		printf("%s: failed to get nand_cfg\n", __func__);
		return -1;
	}

	block = nc->blocksize;
#endif

	vs_assign_by_id(DEV_FND, 0);

#if 0
	if(!(yaffs_offs & (block - 1))) {
		for(; vs_isbad(yaffs_offs); ) {
			if(yaffs_part > block) {
				printf("burn skip bad block at 0x%llx\n", yaffs_offs);
				yaffs_offs += block;
				yaffs_part -= block;
			} else {
				printf("no more good block availible\n");
				return -EBADBLK;
			}
		}
	}
#endif

	vs_write(buf, yaffs_offs, fnd_align(), 0);
	yaffs_offs += vs_align(DEV_FND);
	yaffs_part -= vs_align(DEV_FND);

	return 0;
}

int yaffs_burn(loff_t offs, int part_size,
			int (* get_page)(uint8_t *buf)) {
	int ret;

	printf("%s: to 0x%llx, part: 0x%x\n",
				__func__, offs, part_size);
	vs_assign_by_id(DEV_FND, 1);

	conv.om = vs_align(DEV_FND);
	conv.os = fnd_align() - conv.om;
	conv.oo = fnd_align();

	conv.im = 2048;
	conv.is = 16;
	conv.ii = conv.im + conv.is;

	conv.iodiv = conv.om / conv.im;

	conv.get_page = get_page;
	conv.put_page = yaffs_put_page;
	conv.buf = malloc(fnd_align() * 2);

	vs_erase(offs, part_size);

	yaffs_offs = offs;
	yaffs_part = part_size;
	ret = yaffs_conv(&conv);

	free(conv.buf);

	return ret;
}

