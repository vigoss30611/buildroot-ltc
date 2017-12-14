#include <linux/module.h>
#include <linux/io.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <mach/imap-iomap.h>
#include <mach/io.h>

#define EFSUE_BASE   SYSMGR_EFUSE_BASE
#define EFSUE_RD_ADDR0	(EFSUE_BASE + 0x400)
#define EFSUE_RD_ADDR1	(EFSUE_BASE + 0x800)
#define EFUSE_LEN		(128)

struct efuse_info {
	uint32_t chiptype;
	uint32_t mac_l;
	uint16_t mac_h;
} efuse_info_t; 

static int  efuse_parse_ok = 0;

static void efuse_parse(void) {
	int i, retry = 100;
	char efuse[EFUSE_LEN] = {0,};
	/*
	 * Make sure hardware ready for read, but only try retry times
	 */
	while(( __raw_readl(IO_ADDRESS(EFSUE_BASE))  & 0x1) && retry--);
	if(!retry) {
		pr_err("[efuse] failed to read efuse info\n");
		return;
	}

	for ( i = 0; i < EFUSE_LEN / 2; i++) {
		efuse[i] = __raw_readb(IO_ADDRESS(EFSUE_RD_ADDR0) + i * 4);
		efuse[i + 64] = __raw_readb(IO_ADDRESS(EFSUE_RD_ADDR1) + i * 4);
	}
	efuse_info_t.chiptype = (efuse[99] << 24) | (efuse[98] << 16) | (efuse[97] << 8) | efuse[96];
	efuse_info_t.mac_l = (efuse[103] << 24) | (efuse[102] << 16) | (efuse[101] << 8) | efuse[100];
	efuse_info_t.mac_h = (efuse[105] << 8)  | efuse[104];
	efuse_parse_ok = 1;
}


static int efuse_seq_show(struct seq_file *seq, void *offset) {
	if(efuse_parse_ok) {
		seq_printf(seq, "TYPE:%08x\n", efuse_info_t.chiptype);
		seq_printf(seq, "MAC:%04x%08x\n", efuse_info_t.mac_h, efuse_info_t.mac_l);

	} else {
		seq_printf(seq, "TYPE:ERROR\n");
		seq_printf(seq, "MAC:ERROR\n");
	}
	return 0;

}

int efuse_get_efuse_id(uint32_t *efuse_id)
{
	if(efuse_parse_ok) {
		*efuse_id = efuse_info_t.chiptype;
		printk(KERN_ERR"efuse_get_efuse_id id=%08x\n",efuse_info_t.chiptype);
		return 1;
	} else {
		printk(KERN_ERR"efuse_get_efuse_id error.\n");
		return 0;
	}

}EXPORT_SYMBOL(efuse_get_efuse_id);

static int efuse_fs_open(struct inode *inode, struct file *file) {
	return single_open(file, efuse_seq_show, PDE_DATA(inode));
}

static struct file_operations efuse_fops = {
	.owner = THIS_MODULE,
	.open = efuse_fs_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init efuse_seq_init(void) {
	efuse_parse();
	proc_create("efuse", 0, NULL, &efuse_fops);
	return 0;
}

static void __exit efuse_seq_exit(void) {
	remove_proc_entry("efuse", NULL);
}

subsys_initcall(efuse_seq_init);
module_exit(efuse_seq_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Peter <peter.fu@infotm.com>");
MODULE_DESCRIPTION("infotm coronampw-s efuse infotmaiton");
