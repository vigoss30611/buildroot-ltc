
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <mach/nand.h>

#define ENV_OFFSET 0x3000000
#define ENV_AREA 0x800000

int nand_env_save(uint8_t *data, int len)
{
    struct erase_info instr;
    struct mtd_info *mtd = get_mtd_device_nm("reserved");
    loff_t offs, start = ENV_OFFSET;
    int ret;
	return 0;

    if(!mtd) {
        NAND_DEBUG(KERN_ERR, "store: can not find mtd partition.\n");
        return -1;
    }

    for(offs = 0; offs < ENV_AREA; offs += mtd->erasesize) {

        if(mtd->_block_isbad(mtd, start + offs)) {
            NAND_DEBUG(KERN_ERR, "store: skip bad 0x%llx\n", start + offs);
            continue;
        }

        instr.mtd = mtd;
        instr.addr = start + offs;
        instr.len  = mtd->erasesize;
        instr.callback = NULL;

        ret = mtd->_erase(mtd, &instr);

        if(ret) {
            NAND_DEBUG(KERN_ERR, "store: erase failed 0x%llx\n", instr.addr);
            continue;
        } else break;
    }

    if(offs >= ENV_OFFSET) {
        NAND_DEBUG(KERN_ERR, "store: failed\n");
        return -1;
    }

    /* erase OK, write data */
    return mtd->_write(mtd, instr.addr, len, &ret, data);
}

EXPORT_SYMBOL(nand_env_save);

