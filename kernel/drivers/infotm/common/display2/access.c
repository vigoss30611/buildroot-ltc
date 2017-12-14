/* display register access driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/display_device.h>
#include <mach/imap-iomap.h>
#include "display_register.h"

#define DISPLAY_MODULE_LOG		"access"

int __overlay_OVCWx(int overlay, enum overlay_registers enum_reg)
{
	int reg;
	
	if (overlay == 0) {
		switch (enum_reg) {
			case EOVCWxCR:		reg = OVCW0CR; break;
			case EOVCWxPCAR:	reg = OVCW0PCAR; break;
			case EOVCWxPCBR:	reg = OVCW0PCBR; break;
			case EOVCWxVSSR:	reg = OVCW0VSSR; break;
			case EOVCWxCMR:		reg = OVCW0CMR; break;
			case EOVCWxB0SAR:	reg = OVCW0B0SAR; break;
			case EOVCWxB1SAR:	reg = OVCW0B1SAR; break;
			case EOVCWxB2SAR:	reg = OVCW0B2SAR; break;
			case EOVCWxB3SAR:	reg = OVCW0B3SAR; break;
			/* invalid enum reg */
			case EOVCWxCKCR:
			case EOVCWxCKR:
			case EOVCWxPCCR:
			default:
				printk(KERN_ERR "[ids|register]: Invalid overlay %d enum_reg %d\n", overlay, enum_reg);
				return -EINVAL;
		}
	}
	else if (overlay == 1) {
		switch (enum_reg) {
			case EOVCWxCR:		reg = OVCW1CR; break;
			case EOVCWxPCAR:	reg = OVCW1PCAR; break;
			case EOVCWxPCBR:	reg = OVCW1PCBR; break;
			case EOVCWxPCCR:	reg = OVCW1PCCR; break;
			case EOVCWxVSSR:	reg = OVCW1VSSR; break;
			case EOVCWxCKCR:	reg = OVCW1CKCR; break;
			case EOVCWxCKR:		reg = OVCW1CKR; break;
			case EOVCWxCMR:		reg = OVCW1CMR; break;
			case EOVCWxB0SAR:	reg = OVCW1B0SAR; break;
			case EOVCWxB1SAR:	reg = OVCW1B1SAR; break;
			case EOVCWxB2SAR:	reg = OVCW1B2SAR; break;
			case EOVCWxB3SAR:	reg = OVCW1B3SAR; break;
			default:
				printk(KERN_ERR "[ids|register]: Invalid overlay %d enum_reg %d\n", overlay, enum_reg);
				reg = -EINVAL;
		}
	}
// TODO: multiple overlay
#ifdef CONFIG_ARCH_CRONA
	else if (overlay == 2) {
		printk(KERN_ERR "[ids|register]: To be implement !\n");
	}
#endif
	else {
		printk(KERN_ERR "[ids|register]: Invalid overlay=%d\n", overlay);
		reg = -EINVAL;
	}

	return reg;
}

int __ids_sysmgr(int path)
{
	int reg;

	switch (path) {
		case 0:
#ifdef CONFIG_ARCH_APOLLO
			reg = SYSMGR_IDS1_BASE;
#else
			reg = SYSMGR_IDS0_BASE;
#endif
			break;
#ifdef CONFIG_ARCH_CRONA
		case 1:
			reg = SYSMGR_IDS1_BASE;
			break;
		// TODO: multiple path
#endif
		default:
			dlog_err("Invalid path %d\n", path);
			reg = -EINVAL;
			break;
	}
	return reg;
}

static unsigned char __iomem *base_addr[IDS_PATH_NUM] = {0};
static unsigned char __iomem *sysmgr_ids = 0;

int ids_access_init(int path, struct resource *res)
{
	struct resource *res_mem;

	if (path >= IDS_PATH_NUM) {
		dlog_err("Invalid path %d\n", path);
		return -EINVAL;
	}

	res_mem = request_mem_region(res->start, res->end-res->start+1, res->name);
	if (!res_mem) {
		dlog_err("request ids mem region failed.\n");
		return -EINVAL;
	}

	base_addr[path] = ioremap_nocache(res_mem->start, res_mem->end - res_mem->start+1);
	if (!base_addr[path]) {
		dlog_err("path%d ioremap failed\n", path);
		return -EINVAL;
	}

	dlog_dbg("base_addr[%d] = 0x%X\n", path, (int)base_addr[path]);

	if (!sysmgr_ids) {
		sysmgr_ids = ioremap_nocache(0x2D024C00, 32);
		if (!sysmgr_ids) {
			dlog_err("ioremap sysmgr_ids failed\n");
			return -EINVAL;
		}
		iowrite32(0xF0, sysmgr_ids+0x10);		// Read QoS
	}
	

	return 0;
}

unsigned int ids_readword(int path, unsigned int addr)
{
	return ioread32(base_addr[path] + addr);
}

int ids_writeword(int path, unsigned int addr, unsigned int val)
{
	iowrite32(val, base_addr[path] + addr);
	return 0;
}

int ids_write(int path, unsigned int addr, int bit, int width, unsigned int val)
{
	unsigned int tmp;
	if (bit > 31 || width > 32) {
		dlog_err("Invalid bit %d or width %d\n", bit, width);
		return -EINVAL;
	}
	tmp = ids_readword(path, addr);
	barrier();
	val &= ((1<<width)-1);
	tmp &= ~(((0xFFFFFFFF << (32 - width)) >> (32 - width)) << bit);
	tmp |= (val << bit);
	ids_writeword(path, addr, tmp);
	return 0;
}

MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display register access driver");
MODULE_LICENSE("GPL");
