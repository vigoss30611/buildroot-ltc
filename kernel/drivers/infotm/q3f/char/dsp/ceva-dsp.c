#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/major.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/firmware.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>	/* For put_user and get_user */
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <mach/imap-iomap.h>
#include <mach/ceva-dsp.h>
#include <mach/power-gate.h>
#include <linux/hrtimer.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

//#define TL421_AEC_PROFILING
//#define TL421_AEC_DEBUG		//verbose printing
//#define TL421_AEC_DUMP_INOUT
//#define TL421_AEC_DUMP_MEM	//Be careful, may lead to kernal panic

#define CODE_INT (0)
#define CODE_EXT (1)
#define DATA_INT (2)
#define DATA_EXT (3)

//timeout for while & check mode
#define TIMEOUT_1S		1000000		//1s in us for init & wait_cmd timeout
#define INIT_SLP_MIN	1000		//1000us for every usleep
#define INIT_SLP_MAX	1200		//1200us for every usleep
#define WAIT_CMD_SLP	1000        //1000us for every usleep
#define TASK_TIMEOUT_MS	50			//50ms for task timeout

#ifdef TL421_AEC_PROFILING
#define AEC_AVG_NUM (32)
static char aec_full = 0;
static char aec_index = 0;
static long long aec_spend[AEC_AVG_NUM];
static long long aec_avg_spend = 0;
#endif

static DEFINE_MUTEX(ceva_dsp_mutex);
static struct class *ceva_dsp_class;
static struct dsp_device *tl421 = NULL;
static int request_reload = 1;

static int tl421_upload_firmware(struct firmware_info *info);

#ifdef CONFIG_Q3F_DSP_SUPPORT_AEC
static struct firmware_info aec_fw[] = {
	/*AEC FW*/
	{"vcp_code_int.bin",	0,	BIG_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"vcp_code_ext.bin",	0,	BIG_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"vcp_data_int.bin",	0,	LITTLE_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"vcp_data_ext.bin",	0x10f8,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
};
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE

struct completion dsp_complete;

static struct firmware_info aac_enc_fw[] = {
	/*AAC ENCODE FW*/
	{"AACENC_code_int.bin",		0,	BIG_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"AACENC_code_ext.bin",		0,	BIG_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACENC_data_int.bin",		0,	LITTLE_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"AACENC_data_ext.bin",		0,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
};
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_DECODE
static struct firmware_info aac_dec_fw[] = {
	/*AAC DECODE FW*/
	{"AACDEC_code_int.bin",		0,	BIG_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"AACDEC_code_ext.bin",		0,	BIG_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_int_p1.bin",	0,	LITTLE_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"AACDEC_data_int_p2.bin",	0x1b20,	LITTLE_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"AACDEC_data_int_p3.bin",	0x1c2c,	LITTLE_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"AACDEC_data_int_p4.bin",	0x2434,	LITTLE_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"AACDEC_data_int_p5.bin",	0x2a00,	LITTLE_ENDIAN,	IMG_INTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p1.bin",	0,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p2.bin",	0x4098,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p3.bin",	0x9000,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p4.bin",	0xad09,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p5.bin",	0xaf0a,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p6.bin",	0xbd28,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p7.bin",	0xc3ff,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p8.bin",	0xc600,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p9.bin",	0xcc60,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p10.bin",	0xcf08,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p11.bin",	0xcfee,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p12.bin",	0xd00e,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p13.bin",	0xd01e,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p14.bin",	0xd036,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p15.bin",	0xd0fa,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p16.bin",	0xd11c,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p17.bin",	0xd7a4,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p18.bin",	0xd7d2,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
	{"AACDEC_data_ext_p19.bin",	0xd7f4,	LITTLE_ENDIAN,	IMG_EXTERNAL,   0,      NULL,   0},
};
#endif

static struct dsp_mem_region dsp_mem_region[] = {
	{"dsp_sysm", SYSMGR_DSP_BASE, SZ_1K, NULL, NULL},
	{"dsp_ptcm", IMAP_DSP_PTCM_BASE, IMAP_DSP_PTCM_SIZE, NULL, NULL},
	{"dsp_ptcm_ext", 0, 0x80000, NULL, NULL},
	{"dsp_dtcm", IMAP_DSP_DTCM_BASE, IMAP_DSP_DTCM_SIZE, NULL, NULL},
	{"dsp_dtcm_ext", 0, 0x100000, NULL, NULL},
	{"dsp_ipc", IMAP_DSP_IPC_BASE, IMAP_DSP_IPC_SIZE, NULL, NULL},
};

static long long tl421_get_ns(void) {

	ktime_t a;
	a = ktime_get_boottime();
	return a.tv64;
}

static void tl421_ext_wait(int val)
{
	void __iomem *base = tl421->sysm.base; 

	writel(val, base + DSP_SYSM_EXTERNAL_WAIT);
}

static int tl421_dump_memory(char mem_type, unsigned int offset, unsigned int len)
{
	int i, n, err = 0;
	uint16_t *dst = NULL;
	
	switch (mem_type) {
		case CODE_INT:
			dst = (uint16_t *)(tl421->ptcm.base);
			break;

		case CODE_EXT:
			dst = (uint16_t *)(tl421->ptcm_ext.base);
			break;

		case DATA_INT:
			dst = (uint16_t *)(tl421->dtcm.base);
			break;

		case DATA_EXT:
			dst = (uint16_t *)(tl421->dtcm_ext.base);
			break;

	}

	//move to offset
	dst += (offset);
	//aligned to 2 bytes
	dst = (uint16_t *)(((unsigned int)dst) & ~(1U));

	pr_err("printing 0x%x: \n", offset);

	//dump len data
	for (i = 0; i < (len/2); i+=16) {
		printk(KERN_ERR "@0x%p: ", dst);
		for(n = 0; n < 8; n++) {
			printk(KERN_ERR "0x%04x  ", *dst++);
		}
		printk(KERN_ERR "\n");
	}

	return err;
}

static int tl421_printf_dump(char * file_path, unsigned int offset, unsigned int len) {

	struct file* fp;
	mm_segment_t fs;
	loff_t pos;
	int i, n, err = 0;
	uint16_t buf = 0;

	fp = filp_open(file_path, O_RDONLY, 0644);
	if(IS_ERR(fp)) {
		pr_err("reading dump file: open file fail\n");
		return -1;
	}

	fs = get_fs();
	set_fs(KERNEL_DS);

	//move to offset
	//aligned to 2 bytes
	pos = ((unsigned int)offset) & ~(1U);

	pr_err("printing 0x%x: \n", offset/2);

	//dump len data
	for (i = 0; i < (len); i+=16) {
		printk(KERN_ERR "@0x%x: ", (offset+i)/2);
		for(n = 0; n < 8; n++) {
			vfs_read(fp, (char *)&buf, sizeof(uint16_t), &pos);
			printk(KERN_ERR "0x%04x  ", buf);
		}
		printk(KERN_ERR "\n");
	}

	filp_close(fp, NULL);
	set_fs(fs);

	return err;
}

static int tl421_dump_memory_to_file(char * file_path,char mem_type, unsigned int offset, unsigned int len)
{
	struct file* fp;
	mm_segment_t fs;
	loff_t pos;
	int i, err = 0;
	uint16_t *dst = NULL;
	uint16_t buf = 0;

	fp = filp_open(file_path, O_RDWR | O_CREAT, 0644);
	if(IS_ERR(fp)) {
		pr_err("open file fail\n");
		return -1;
	}

	fs = get_fs();
	set_fs(KERNEL_DS);

	switch (mem_type) {
		case CODE_INT:
			dst = (uint16_t *)(tl421->ptcm.base);
			break;

		case CODE_EXT:
			dst = (uint16_t *)(tl421->ptcm_ext.base);
			break;

		case DATA_INT:
			dst = (uint16_t *)(tl421->dtcm.base);
			break;

		case DATA_EXT:
			dst = (uint16_t *)(tl421->dtcm_ext.base);
			break;

	}

	//move to offset
	dst += (offset);
	//aligned to 2 bytes
	dst = (uint16_t *)(((unsigned int)dst) & ~(1U));

	pr_err("saving 0x%p: \n", dst);

	//dump len data
	pos = 0;
	for (i = 0; i < (len/2); i++) {
		buf = *dst;
		vfs_write(fp, (char *)&buf, sizeof(uint16_t), &pos);
		dst++;
	}
	filp_close(fp, NULL);
	set_fs(fs);

	return err;
}



static int tl421_module_poweron(void)
{
	void __iomem *base = tl421->sysm.base;
	unsigned int timeout = 0;

	/* disable bus output */
	writel(0, base + DSP_SYSM_BUS_CTL);

	/* pull up reset */
	writel(0x7, base + DSP_SYSM_RESET);

	/* dsp core halt for load img */
	writel(0x1, base + DSP_SYSM_EXTERNAL_WAIT);

	/* enable clock */
	writel(0xff, base + DSP_SYSM_CLK_GATE);

	/* power on */
	writel(0x11, base + DSP_SYSM_POW_CTL);

	/* wait for powe on ack */
	while (1) {
		if (readl(base + DSP_SYSM_POW_CTL) & 0x2)
			break;
		timeout++;
		if (timeout == 0xff) {
			pr_err("tl421_module_powerup time out\n");
			return -1;
		}
	}

	/* enable module output */
	writel(readl(base+DSP_SYSM_POW_CTL) & ~0x10, base+DSP_SYSM_POW_CTL);

	/* disable IO mask */
	writel(0xff, base + DSP_SYSM_IF_ISO);

	/* config mem pd */
	writel(0, base + DSP_SYSM_MEM_PD);

	/* set clk divider */
	writel(0x1, base + DSP_SYSM_CLK_RATIO);

	/* pull down reset */
	writel(0, base + DSP_SYSM_RESET);

	/* enable bus output */
	writel(0xff, base + DSP_SYSM_BUS_CTL);

	return 0;
}

static int tl421_module_poweroff(void)
{
	void __iomem *base = tl421->sysm.base;

	/* dsp core should be halt? */
	writel(0x1, base + DSP_SYSM_EXTERNAL_WAIT);

	return module_power_down(SYSMGR_DSP_BASE);
}

static int tl421_wait_mission_complete(void)
{
	void __iomem *base = tl421->ipc.base;
	int timeout = 200000;
	struct codec_format *args_in = &tl421->info->in;

	if (args_in->type != 0) {
		while (!readl(base + IPC_SEM0S)) {
			if (timeout <= 0) {
				pr_err("timeout waiting for dsp decoder\n");
				return -ETIMEDOUT;
			}
			timeout--;
			udelay(50);
		}
	} else {
		while (!readl(base + IPC_REP0)) {
			if (timeout <= 0) {
				pr_err("timeout waiting for dsp encoder\n");
				return -ETIMEDOUT;
			}
			timeout--;
			usleep_range(10, 50);
		}
	}

	return 0;
}

#ifdef CONFIG_Q3F_DSP_SUPPORT_AEC

static irqreturn_t dsp_isr(int irq, void *data)
{
	void __iomem *base = tl421->ipc.base;
	int ret, mask;

	tl421->aec.status = ret = readl(base + IPC_SEM0S);
	mask = readl(base + IPC_MCU_MASK0);

	//reset all set bits of sem0s to prevent the irq triggered again
	writel(readl(base + IPC_SEM0S) & ~(ret & mask), base + IPC_SEM0S);

#ifdef TL421_AEC_DEBUG
	pr_err("%s: interrupt mask: 0x%x, status: 0x%x\n", __func__, mask, ret);
#endif

	//when done bit set, cmd complete whether it's successed or failed
	if(ret & mask & (1 << AEC_DONE_BIT)) {
		wake_up_interruptible(&tl421->aec.dsp_respond);
#ifdef TL421_AEC_DEBUG
		pr_err("%s: interrupt AEC_DONE: 0x%x\n", __func__, ret);
#endif
	}

	return IRQ_HANDLED;
}

static int tl421_aec_wait_initiate_complete(void)
{
	void __iomem *base = tl421->ipc.base;
	int ret;
	int timeout = TIMEOUT_1S / INIT_SLP_MIN;
#ifdef TL421_AEC_PROFILING
	long long ts, te;
	ts = tl421_get_ns();
#endif

	while (((ret = readl(base + IPC_SEM0S)) & ((1 << AEC_ERROR_BIT) | (1 << AEC_RUNNING_BIT))) == 0x0) {
		if (timeout <= 0) {
#ifdef TL421_AEC_PROFILING
			te = tl421_get_ns();
			pr_err("DSP initiate timeout: %lldns elapsed\n",  (te - ts));
#endif
			pr_err("DSP initiating timeout, IPC: sem0s = 0x%x, sem1s = 0x%x, sem2s = 0x%x, sem0c = 0x%x, sem1c = 0x%x, sem2c = 0x%x, com0 = 0x%x, com1 = 0x%x, com2 = 0x%x, rep0 = 0x%x, rep1 = 0x%x, rep2 = 0x%x\n", readl(base + IPC_SEM0S), readl(base + IPC_SEM1S), readl(base + IPC_SEM2S), readl(base + IPC_SEM0C), readl(base + IPC_SEM1C), readl(base + IPC_SEM2C),readl(base + IPC_COM0), readl(base + IPC_COM1), readl(base + IPC_COM2), readl(base + IPC_REP0), readl(base + IPC_REP1), readl(base + IPC_REP2));

#ifdef TL421_AEC_DUMP_MEM
			tl421_dump_memory_to_file("/mnt/sd1/data_int.bin", DATA_INT, 0x0, 2 * 24 * 1024);
			pr_err("spk:");
			tl421_dump_memory(DATA_INT, 0x3f82, 128);
			pr_err("mic:");
			tl421_dump_memory(DATA_INT, 0x3fc2, 128);
			pr_err("mout:");
			tl421_dump_memory(DATA_INT, 0x3f42, 128);
#endif

			return -ETIMEDOUT;
		}

		timeout--;
		usleep_range(INIT_SLP_MIN, INIT_SLP_MAX);
	}

#ifdef TL421_AEC_PROFILING
	te = tl421_get_ns();
	pr_err("DSP initiate complete: %lldns elapsed\n",  (te - ts));
#endif

#ifdef TL421_AEC_DEBUG
	pr_err("DSP initiate complete, IPC: sem0s = 0x%x, sem1s = 0x%x, sem2s = 0x%x, sem0c = 0x%x, sem1c = 0x%x, sem2c = 0x%x, com0 = 0x%x, com1 = 0x%x, com2 = 0x%x, rep0 = 0x%x, rep1 = 0x%x, rep2 = 0x%x\n", readl(base + IPC_SEM0S), readl(base + IPC_SEM1S), readl(base + IPC_SEM2S), readl(base + IPC_SEM0C), readl(base + IPC_SEM1C), readl(base + IPC_SEM2C),readl(base + IPC_COM0), readl(base + IPC_COM1), readl(base + IPC_COM2), readl(base + IPC_REP0), readl(base + IPC_REP1), readl(base + IPC_REP2));
#endif

	/* DSP state */
	ret = readl(base + IPC_SEM0S);
	//Oops! AEC initiate error
	if(ret & (1 << AEC_ERROR_BIT)) {
		pr_err("dsp initiate fail with error = %d \n", readl(base + IPC_REP2));
		return -EAGAIN;
	}
	//Good! AEC running
	else if(ret & (1 << AEC_RUNNING_BIT)) {
		pr_err("dsp initiate successfully\n");
	}
	//else? should not happen!
	else {
		return -EAGAIN;
	}

	return 0;
}


static int tl421_aec_wait_mission_complete(void)
{
	void __iomem *base = tl421->ipc.base;
	int ret;

#ifdef TL421_AEC_PROFILING
	long long ts, te;
	ts = tl421_get_ns();
#endif

	tl421->aec.status = 0;
	/* wait for DSP completing mission */
	ret = wait_event_interruptible_timeout(tl421->aec.dsp_respond, (tl421->aec.status & (1 << AEC_DONE_BIT)), tl421->aec.timeout);

	//wait_event timeout
	if (ret == 0)
	{
#ifdef TL421_AEC_PROFILING
		te = tl421_get_ns();
		pr_err("DSP mission timeout: %lldns elapsed\n",  (te - ts));
#endif

		pr_err("DSP mission timeout, IPC: sem0s = 0x%x, sem1s = 0x%x, sem2s = 0x%x, sem0c = 0x%x, sem1c = 0x%x, sem2c = 0x%x, com0 = 0x%x, com1 = 0x%x, com2 = 0x%x, rep0 = 0x%x, rep1 = 0x%x, rep2 = 0x%x\n", readl(base + IPC_SEM0S), readl(base + IPC_SEM1S), readl(base + IPC_SEM2S), readl(base + IPC_SEM0C), readl(base + IPC_SEM1C), readl(base + IPC_SEM2C),readl(base + IPC_COM0), readl(base + IPC_COM1), readl(base + IPC_COM2), readl(base + IPC_REP0), readl(base + IPC_REP1), readl(base + IPC_REP2));

#ifdef TL421_AEC_DUMP_MEM
			tl421_dump_memory_to_file("/mnt/sd1/data_int.bin", DATA_INT, 0x0, 2 * 24 * 1024);
			pr_err("spk:");
			tl421_dump_memory(DATA_INT, 0x3f82, 128);
			pr_err("mic:");
			tl421_dump_memory(DATA_INT, 0x3fc2, 128);
			pr_err("mout:");
			tl421_dump_memory(DATA_INT, 0x3f42, 128);
#endif

		return -ETIMEDOUT;
	}
	//wait_event interrupted
	else if (ret < 0)
	{
		pr_err("mission interrupted, ret = %d \n", (readl(base + IPC_SEM0S)));
		return -EINTR;
	}
	//wait_event is waked up with condition is met
#ifdef TL421_AEC_DEBUG
	else {
		pr_err("DSP wait_event: %dms elapsed\n", TASK_TIMEOUT_MS - jiffies_to_msecs(ret));
	}
#endif

	/* DSP state */
	ret = readl(base + IPC_REP2);
	//Oops! DSP mission error
	if(ret) {
		pr_err("mission complete with error, ret = %d \n", ret);
		//return -EAGAIN;
	}
	//Good! DSP mission completed
#ifdef TL421_AEC_DEBUG
	else {
		pr_err("mission complete successfully\n");
	}
#endif

#ifdef TL421_AEC_PROFILING
	int i;
	te = tl421_get_ns();
	pr_err("DSP mission complete: %lldns elapsed\n", te - ts);

	aec_spend[aec_index] = (te - ts);
	if(aec_index >= (AEC_AVG_NUM-1))
		aec_full = 1;
	if(aec_full) {
		aec_avg_spend = 0;
		for(i = 0; i < AEC_AVG_NUM; i++) {
			aec_avg_spend += aec_spend[i];
		}
	}
	pr_err("DSP mission complete avg spend: %lldns\n", (aec_avg_spend/AEC_AVG_NUM));
	aec_index = (++aec_index) % AEC_AVG_NUM;
#endif

#ifdef TL421_AEC_DUMP_MEM
	pr_err("DSP mission complete, IPC: sem0s = 0x%x, sem1s = 0x%x, sem2s = 0x%x, sem0c = 0x%x, sem1c = 0x%x, sem2c = 0x%x, com0 = 0x%x, com1 = 0x%x, com2 = 0x%x, rep0 = 0x%x, rep1 = 0x%x, rep2 = 0x%x\n", readl(base + IPC_SEM0S), readl(base + IPC_SEM1S), readl(base + IPC_SEM2S), readl(base + IPC_SEM0C), readl(base + IPC_SEM1C), readl(base + IPC_SEM2C),readl(base + IPC_COM0), readl(base + IPC_COM1), readl(base + IPC_COM2), readl(base + IPC_REP0), readl(base + IPC_REP1), readl(base + IPC_REP2));
	tl421_dump_memory_to_file("/mnt/sd1/data_int.bin", DATA_INT, 0x0, 2 * 24 * 1024);
	pr_err("spk:");
	tl421_printf_dump("/mnt/sd1/data_int.bin", 0x3ef8 * 2, 128);
	pr_err("mic:");
	tl421_printf_dump("/mnt/sd1/data_int.bin", 0x3f78 * 2, 128);
	pr_err("mout:");
	tl421_printf_dump("/mnt/sd1/data_int.bin", 0x3e78 * 2, 128);
	pr_err("reg:");
	tl421_printf_dump("/mnt/sd1/data_int.bin", 0x3ff8 * 2, 128);
#endif

	return 0;
}
#endif

static int tl421_set_format(unsigned long arg)
{
	int err = 0;
	int i, ret;
	struct codec_format *args_in, *args_out;

	if (copy_from_user(tl421->info, (void __user *)arg, 
				sizeof(struct codec_info))) {
		pr_err("failed to copy codec_info from user\n");
		return -EFAULT;
	}

	args_in = &tl421->info->in;
	args_out = &tl421->info->out;

	if (((args_in->type == 0) && (args_out->type != 7)) 
		|| ((args_in->type == 0) && (args_out->type != 7))) {
		pr_err("Error code type: in %d, out %d\n", args_in->type, args_out->type);
		return -EFAULT;
	}

	if (memcmp(tl421->info, tl421->pre_info, sizeof(struct codec_info))) {
		memcpy(tl421->pre_info, tl421->info, sizeof(struct codec_info));
		request_reload = 1;
	}

	if (args_in->type != 0) {//0 is PCM, input PCM means Encode
#ifdef CONFIG_Q3F_DSP_SUPPORT_DECODE
		tl421->dec->ac = dma_alloc_coherent(NULL, sizeof(struct audio_codec), 
							&tl421->dec->p_ac, GFP_KERNEL);
		if (tl421->dec->ac == NULL) {
			pr_err("failed to alloc for tl421->dec->ac\n");
			err = -ENOMEM;
			goto fail_exit;
		}

		tl421->dec->in = dma_alloc_coherent(NULL, 1550*4*sizeof(uint16_t), 
							&tl421->dec->p_in, GFP_KERNEL);
		if (tl421->dec->in == NULL) {
			pr_err("failed to alloc for tl421->dec->in\n");
			err = -ENOMEM;
			goto fail_exit;
		}

		tl421->dec->out = dma_alloc_coherent(NULL, 2048*4*sizeof(uint16_t), 
							&tl421->dec->p_out, GFP_KERNEL);
		if (tl421->dec->out == NULL) {
			pr_err("failed to alloc for tl421->dec->out\n");
			err = -ENOMEM;
			goto fail_exit;
		}

		tl421->dec->ac->src = tl421->dec->p_in;
		tl421->dec->ac->dst = tl421->dec->p_out;
		tl421->dec->ac->len = 0;
#endif
	} else {
#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE
		tl421->enc->ac = dma_alloc_coherent(NULL, sizeof(struct audio_codec), 
							&tl421->enc->p_ac, GFP_KERNEL);
		if (tl421->enc->ac == NULL) {
			pr_err("failed to alloc for tl421->enc->ac\n");
			err = -ENOMEM;
			goto fail_exit;
		}

		tl421->enc->in = dma_alloc_coherent(NULL, AACENC_IN_SIZE*sizeof(uint8_t), 
							&tl421->enc->p_in, GFP_KERNEL);
		if (tl421->enc->in == NULL) {
			pr_err("failed to alloc for tl421->enc->in\n");
			err = -ENOMEM;
			goto fail_exit;
		}

		tl421->enc->out = dma_alloc_coherent(NULL, AACENC_OUT_SIZE*sizeof(uint8_t), 
							&tl421->enc->p_out, GFP_KERNEL);
		if (tl421->enc->out == NULL) {
			pr_err("failed to alloc for tl421->enc->out\n");
			err = -ENOMEM;
			goto fail_exit;
		}

		tl421->enc->ac->src = tl421->enc->p_in;
		tl421->enc->ac->dst = tl421->enc->p_out;
		tl421->enc->ac->len = 0;

		if (request_reload) {
			tl421_module_poweron();

			for (i = 0; i < ARRAY_SIZE(aac_enc_fw); i++) {
				ret = tl421_upload_firmware(&aac_enc_fw[i]);
				if (ret)
					goto fail_exit;
			}

			tl421_ext_wait(0);

			request_reload = 0;
		}
#endif
	}

	return 0;

fail_exit:
#ifdef CONFIG_Q3F_DSP_SUPPORT_DECODE
	if (tl421->dec->ac) {
		dma_free_coherent(NULL, sizeof(struct audio_codec), 
					tl421->dec->ac, tl421->dec->p_ac);
		tl421->dec->ac = NULL;
	}
	if (tl421->dec->in) {
		dma_free_coherent(NULL, 1550*4*sizeof(uint16_t), 
					tl421->dec->in, tl421->dec->p_in);
		tl421->dec->in = NULL;
	}
	if (tl421->dec->out) {
		dma_free_coherent(NULL, 2048*4*sizeof(uint16_t), 
					tl421->dec->out, tl421->dec->p_out);
		tl421->dec->out = NULL;
	}
#endif
#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE
	if (tl421->enc->ac) {
		dma_free_coherent(NULL, sizeof(struct audio_codec), 
					tl421->enc->ac, tl421->enc->p_ac);
		tl421->enc->ac = NULL;
	}
	if (tl421->enc->in) {
		dma_free_coherent(NULL, AACENC_IN_SIZE*sizeof(uint8_t), 
					tl421->enc->in, tl421->enc->p_in);
		tl421->enc->in = NULL;
	}
	if (tl421->enc->out) {
		dma_free_coherent(NULL, AACENC_OUT_SIZE*sizeof(uint8_t), 
					tl421->enc->out, tl421->enc->p_out);
		tl421->enc->out = NULL;
	}
#endif

	return err;
}

#ifdef CONFIG_Q3F_DSP_SUPPORT_AEC
static int tl421_aec_set_dbg_format(unsigned long arg)
{
	int err = 0;
	struct aec_dbg_info info;
	void __iomem *base = tl421->ipc.base;

	if (copy_from_user(&info, (struct aec_dbg_info *)arg,
				sizeof(struct aec_dbg_info))) {
		pr_err("copy argument failed\n");
		return -EFAULT;
	}

	info.buf_addr.aec_dbg_out_virt = dma_alloc_coherent(NULL, info.buf_addr.dbg_len, &info.buf_addr.aec_dbg_out_phy, GFP_KERNEL);

	if (info.buf_addr.aec_dbg_out_virt == NULL) {
		pr_err("failed to alloc for tl421->aec.dbg.buf_addr.aec_out_virt\n");
		err = -ENOMEM;
		goto fail_exit;
	}
	memcpy(&(tl421->aec.dbg), &(info), sizeof(struct aec_dbg_info));

	if (copy_to_user((struct aec_dbg_info *)arg, &info,
				sizeof(struct aec_dbg_info))) {
		pr_err("copy argument failed\n");
		err =  -EFAULT;
		goto fail_exit;
	}
	pr_err("alloc dbg_len(%d) dbg_addr(virt: 0x%p, phy:0x%x) for dbg_mode(%d)\n", info.buf_addr.dbg_len, info.buf_addr.aec_dbg_out_virt, info.buf_addr.aec_dbg_out_phy, info.fmt.dbg_mode);

	return 0;

fail_exit:
	if (tl421->aec.dbg.buf_addr.aec_dbg_out_virt) {
		dma_free_coherent(NULL, tl421->aec.dbg.buf_addr.dbg_len,
				  tl421->aec.dbg.buf_addr.aec_dbg_out_virt, tl421->aec.dbg.buf_addr.aec_dbg_out_phy);
	}
	memset(&(tl421->aec.dbg), 0, sizeof(struct aec_dbg_info));

	return err;
}

static int tl421_aec_set_format(unsigned long arg)
{
	int err = 0, i, val;
	uint data_len = 0;
	struct aec_info info;
	void __iomem *base = tl421->ipc.base;
	int mode = 0;

	if (copy_from_user(&info, (struct aec_info *)arg,
				sizeof(struct aec_info))) {
		pr_err("copy argument failed\n");
		return -EFAULT;
	}
	//pr_err("\nfmt.channel = %d, fmt.sample_rate = %d, fmt.bitwidth = %d\n", info.fmt.channel, info.fmt.sample_rate, info.fmt.bitwidth);

	memcpy(&(tl421->aec.fmt), &(info.fmt), sizeof(struct aec_format));
	data_len = tl421->aec.buf_addr.data_len = info.fmt.sample_size;
	pr_err("data_len(%d) for sample_rate(%d)\n", data_len, info.fmt.sample_rate);

	tl421->aec.buf_addr.spk_in_virt = dma_alloc_coherent(NULL, data_len, &tl421->aec.buf_addr.spk_in_phy, GFP_KERNEL);
	//pr_err("alloc for tl421->aec.buf_addr.spk_in_phy = %p\n", tl421->aec.buf_addr.spk_in_phy);

	if (tl421->aec.buf_addr.spk_in_virt == NULL) {
		pr_err("failed to alloc for tl421->aec.buf_addr.spk_in_virt\n");
		err = -ENOMEM;
		goto fail_exit;
	}

	tl421->aec.buf_addr.mic_in_virt = dma_alloc_coherent(NULL, data_len, &tl421->aec.buf_addr.mic_in_phy, GFP_KERNEL);
	//pr_err("alloc for tl421->aec.buf_addr.mic_in_phy = %p\n", tl421->aec.buf_addr.mic_in_phy);

	if (tl421->aec.buf_addr.mic_in_virt == NULL) {
		pr_err("failed to alloc for tl421->aec.buf_addr.mic_in_virt\n");
		err = -ENOMEM;
		goto fail_exit;
	}

	tl421->aec.buf_addr.aec_out_virt = dma_alloc_coherent(NULL, data_len, &tl421->aec.buf_addr.aec_out_phy, GFP_KERNEL);
	//pr_err("alloc for tl421->aec.buf_addr.aec_out_phy = %p\n", tl421->aec.buf_addr.aec_out_phy);

	if (tl421->aec.buf_addr.aec_out_virt == NULL) {
		pr_err("failed to alloc for tl421->aec.buf_addr.aec_out_virt\n");
		err = -ENOMEM;
		goto fail_exit;
	}

	memcpy(&(info.buf_addr), &(tl421->aec.buf_addr), sizeof(struct aec_buf));
	if (copy_to_user((struct aec_info *)arg, &info,
				sizeof(struct aec_info))) {
		pr_err("copy argument failed\n");
		err =  -EFAULT;
		goto fail_exit;
	}

	pr_err(" tl421_module_poweron() \n");
	tl421_module_poweron();

	for (i = 0; i < ARRAY_SIZE(aec_fw); i++) {
		err = tl421_upload_firmware(&aec_fw[i]);
		if (err) {
			err = -EFAULT;
			goto init_fail;
		}
	}
	pr_err(" upload firmware successed! \n");

	init_waitqueue_head(&tl421->aec.dsp_respond);
	tl421->aec.timeout = msecs_to_jiffies(TASK_TIMEOUT_MS);
	err = request_irq(TOARM_INT, dsp_isr,
				IRQF_TRIGGER_HIGH,
				"dsp_toarm_intr", &tl421->pdev->dev);
	if(err < 0) {
		err = -EBUSY;
		goto init_fail;
	}
	tl421->aec.irq = TOARM_INT;
	writel(0, base + IPC_MCU_MASK0);
	pr_err(" request_irq success! \n");

//	pr_err(" IPC_SEM0S, flag = %d\n", 0);
	//writel(0x0, base + IPC_SEM0S);
	if(tl421->aec.dbg.fmt.dbg_mode == AEC_LOGGER_DBG && (tl421->aec.dbg.buf_addr.dbg_len) && (tl421->aec.dbg.buf_addr.aec_dbg_out_phy)) {
		mode = AEC_LOGGER_DBG_MODE;
		writel(tl421->aec.dbg.buf_addr.aec_dbg_out_phy, base + IPC_SEM1S);
	}
	else if(tl421->aec.dbg.fmt.dbg_mode == AEC_CI_DBG && (tl421->aec.dbg.buf_addr.dbg_len) && (tl421->aec.dbg.buf_addr.aec_dbg_out_phy)) {
		mode = AEC_CI_DBG_MODE;
		writel(tl421->aec.dbg.buf_addr.aec_dbg_out_phy, base + IPC_SEM1S);
	}
	else {
		mode = AEC_LOW_PROFILE_MODE;
	}
	val = readl(base + IPC_SEM0S);
	writel((val & ~(AEC_MODE_MASK)) | (mode << AEC_MODE_BIT), base + IPC_SEM0S);
//	pr_err(" IPC_COM0, cmd = CMD_NONE(%d)\n", 0);
	writel(0x0, base + IPC_COM0);
//	pr_err(" init IPC_REP0 to 0\n");
	writel(0, base + IPC_REP0);
//	pr_err(" init IPC_REP1 to 0\n");
	writel(0, base + IPC_REP1);
//	pr_err(" init IPC_REP2 to 0\n");
	writel(0, base + IPC_REP2);

	//in case that firmware are not prepared
	udelay(50);

	tl421_ext_wait(0);
	pr_err("dsp start running...\n");
	err = tl421_aec_wait_initiate_complete();
	if(err < 0) {
		err = -EIO;
		goto free_irq;
	}

	/* mask & unmask toasm interrupt for SEM0S bit set */
	writel((1 << AEC_DONE_BIT), base + IPC_MCU_MASK0);

	return 0;

free_irq:
	if(tl421->aec.irq) {
		free_irq(tl421->aec.irq, &tl421->pdev->dev);
		tl421->aec.irq = 0;
	}

init_fail:
	tl421_module_poweroff();

fail_exit:
	if (tl421->aec.buf_addr.spk_in_virt) {
		dma_free_coherent(NULL, data_len,
				  tl421->aec.buf_addr.spk_in_virt, tl421->aec.buf_addr.spk_in_phy);
		tl421->aec.buf_addr.spk_in_virt = NULL;
		tl421->aec.buf_addr.spk_in_phy = 0;
	}
	if (tl421->aec.buf_addr.mic_in_virt) {
		dma_free_coherent(NULL, data_len,
				  tl421->aec.buf_addr.mic_in_virt, tl421->aec.buf_addr.mic_in_phy);
		tl421->aec.buf_addr.mic_in_virt = NULL;
		tl421->aec.buf_addr.mic_in_phy = 0;
	}
	if (tl421->aec.buf_addr.aec_out_virt) {
		dma_free_coherent(NULL, data_len,
				  tl421->aec.buf_addr.aec_out_virt, tl421->aec.buf_addr.aec_out_phy);
		tl421->aec.buf_addr.aec_out_virt = NULL;
		tl421->aec.buf_addr.aec_out_phy = 0;
	}
	if (tl421->aec.dbg.buf_addr.aec_dbg_out_virt) {
		dma_free_coherent(NULL, tl421->aec.dbg.buf_addr.dbg_len,
				  tl421->aec.dbg.buf_addr.aec_dbg_out_virt, tl421->aec.dbg.buf_addr.aec_dbg_out_phy);
		tl421->aec.dbg.buf_addr.aec_dbg_out_virt = NULL;
		tl421->aec.dbg.buf_addr.aec_dbg_out_phy = 0;
	}

	return err;
}
#else
static int tl421_aec_set_dbg_format(unsigned long arg)
{
	return 0;
}
static int tl421_aec_set_format(unsigned long arg)
{
	return 0;
}
#endif

static int tl421_fw_cb(struct firmware_info *info)
{
	struct platform_device *pdev = tl421->pdev;
	const struct firmware *fw;
	int err = 0;
	
	//pr_err("########## tl421_fw_cb: %s ##########\n", info->name);
	err = request_firmware(&fw, info->name, &pdev->dev);
	if (err) {
		pr_err("Failed to request firmware '%s', err=%d\n", info->name, err);
		return -1;
	}

	info->data = kzalloc((fw->size)*sizeof(uint8_t), GFP_KERNEL);
	if (info->data == NULL) {
		pr_err("Failed to malloc data for %s\n", info->name);
		return -1;
	}

	memcpy(info->data, fw->data, fw->size);
	info->size = fw->size;
	info->load = 1;

	release_firmware(fw);

	return 0;
}

static int tl421_upload_firmware(struct firmware_info *info)
{
	int i, err = 0;
	uint16_t *tmp = NULL;
	uint16_t *dst = NULL;
	
	if (info->endian == LITTLE_ENDIAN) {
		dst = (info->loc == IMG_EXTERNAL) ? 
			(uint16_t *)(tl421->dtcm_ext.base + info->off * 2) :
				(uint16_t *)(tl421->dtcm.base + info->off * 2);
	} else {
		dst = (info->loc == IMG_EXTERNAL) ? 
			(uint16_t *)(tl421->ptcm_ext.base + info->off * 2) :
				(uint16_t *)(tl421->ptcm.base + info->off * 2);
	}

	tmp = (uint16_t *)info->data;

	for (i = 0; i < (info->size/2); i++) {
		*dst++ = *tmp++;
	}

	return err;
}

#ifdef CONFIG_Q3F_DSP_SUPPORT_DECODE
static int tl421_decode_aac(unsigned long arg)
{
	int i, out_samples, ret = 0;
	struct audio_codec temp;
	void __iomem *base = tl421->ipc.base;
	uint8_t *in = tl421->dec->in;
	uint8_t *out = tl421->dec->out;

	if (copy_from_user(&temp, (void __user *)arg, 
				sizeof(struct audio_codec))) {
		pr_err("copy argument failed\n");
		return -EFAULT;
	}

	if (temp.len > 12400) {/*that is 1550*4*2*/
		pr_err("ERROR: input data len larger than malloc\n");
		return -ENOMEM;
	}

	tl421_module_poweron();

	writel(tl421->dec->p_in, base + IPC_COM0);
	writel(tl421->dec->p_out, base + IPC_COM1);
	temp.len = (temp.len + 1) / 2;
	writel(temp.len, base + IPC_COM2);

	if (copy_from_user(in, (void __user *)temp.src,
				temp.len * sizeof(uint16_t))) {
		pr_err("copy_from_user failed\n");
		return -EFAULT;
	}

	for (i = 0; i < ARRAY_SIZE(aac_dec_fw); i++) {
		ret = tl421_upload_firmware(&aac_dec_fw[i]);
		if (ret)
			return -EFAULT;
	}

	tl421_ext_wait(0);
	//pr_err("dsp start running...\n");

	ret = tl421_wait_mission_complete();
	if (ret)
		return ret;
	tl421_ext_wait(1);

	//pr_err("SEM0=0x%x, SEM1=%d, REP0=%d\n",
	//		readl(base + IPC_SEM0S), readl(base + IPC_SEM1S), readl(base + IPC_REP0));

	out_samples = readl(base + IPC_REP0);

	if (copy_to_user((void __user *)temp.dst, out, 
				out_samples * sizeof(uint16_t))) {
		pr_err("copy_to_user failed\n");
		return -EFAULT;
	}

	return (out_samples * sizeof(uint16_t));
}
#else
static int tl421_decode_aac(unsigned long arg)
{
	return 0;
}
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE
static void aacenc_process_input(unsigned int br,
				 unsigned int sr,
				 int enc_pattern,
				 int enc_mode,
				 int format,
				 int input_bytes)
{
	uint32_t command = 0;
	void __iomem *base = tl421->ipc.base;

	command |= br / 1000;
	command |= (sr / 1000) << 9;
	command |= enc_pattern << 15;
	command &= ~(0x3 << 16);
	if (enc_mode == 2)
		command |= 0x1 << 16;
	command |= 0x2 << 18;
	command |= (input_bytes / 4096) << 20;

	writel(tl421->enc->p_in, base + IPC_COM0);
	writel(tl421->enc->p_out, base + IPC_COM1);
	writel(command, base + IPC_COM2);
}

static void aacenc_process_output(uint8_t *encode_status, 
				  uint8_t *out_ch, 
				  uint8_t *out_sr, 
				  uint8_t *dummy, 
				  uint8_t *dummy_bytes, 
				  uint16_t *samples_to_read, 
				  uint32_t *total_buf_bytes, 
				  uint16_t *valid_buf_bytes, 
				  uint16_t *rsvd)
{
	void __iomem *base = tl421->ipc.base;
	uint32_t rep0 = readl(base + IPC_REP0);
	uint32_t rep1 = readl(base + IPC_REP1);
	uint32_t rep2 = readl(base + IPC_REP2);

	*encode_status = rep0 & 0x7;
	*out_ch = (rep0 & 0x8) ? 2 : 1;
	*out_sr = (rep0 & 0x30) >> 4;
	*dummy = (rep0 & 0x100) >> 8;
	*dummy_bytes = (rep0 & 0xfe00) >> 9;
	*samples_to_read = (rep0 >> 16) & 0xffff;
	*total_buf_bytes = rep1;
	*valid_buf_bytes = rep2 & 0xffff;
	*rsvd = (rep2 >> 16) & 0xffff;

	writel(0, base + IPC_REP0);
	writel(0, base + IPC_REP1);
	writel(0, base + IPC_REP2);
}

static irqreturn_t dsp_toarm_isr(int irq, void *data)
{
	void __iomem *base = tl421->ipc.base;
	uint32_t status = 0;

	writel(0, base + IPC_INT_MASK);
	status = readl(base + IPC_STATUS);
	writel(status, base + IPC_STATUS);

	if (status & (1 << 3))
		complete(&dsp_complete);

	return IRQ_HANDLED;
}

static int32_t tl421_encode_aac(unsigned long arg)
{
	int ret=0;
	int32_t out_bytes = 0;
	struct audio_codec temp;
	uint8_t *in = tl421->enc->in;
	uint8_t *out = tl421->enc->out;
	uint8_t encode_status=0, out_ch=0, out_sr=0, dummy=0, dummy_bytes=0;
	uint16_t sample_read=0, valid_bytes=0, rsvd=0;
	uint32_t total_bytes=0;
	void __iomem *base = tl421->ipc.base;
	struct codec_format *args_in = &tl421->info->in;
	struct codec_format *args_out = &tl421->info->out;
	int encode_debug = 0;
	uint32_t status = 0;

	if (args_in->sr != 48000) {
		pr_err("AAC Encoder error: input stream must be 48KHz sampling rate\n");
		return -EINVAL;
	}

	if (copy_from_user(&temp, (void __user *)arg, 
				sizeof(struct audio_codec))) {
		pr_err("Failed to copy argument from user\n");
		return -EFAULT;
	}
	
	if (temp.len != 4096) {
		pr_err("ERROR: input data length error\n");
		return -ENOMEM;
	}

	INIT_COMPLETION(dsp_complete);

	aacenc_process_input(16000/*args_out->br*/, 
			     args_in->sr, 
			     0, 
			     args_in->channel, 
			     2, 
			     temp.len);

	if (copy_from_user(in, (void __user *)temp.src,
					temp.len * sizeof(uint8_t))) {
		pr_err("Failed to copy source data from user\n");
		return -EFAULT;
	}

	status = readl(base + IPC_STATUS);
	writel(status, base + IPC_STATUS);
	writel(0x8, base + IPC_INT_MASK);
	writel(0x1, base + IPC_SEM0S);

	/*ret = tl421_wait_mission_complete();
	if (ret)
		return ret;*/
	ret = wait_for_completion_timeout(&dsp_complete, HZ);
	if (!ret) {
		pr_err("dsp timed out\n");
		return -1;
	}
	
	aacenc_process_output(&encode_status, 
			      &out_ch, 
			      &out_sr, 
			      &dummy, 
			      &dummy_bytes, 
			      &sample_read, 
			      &total_bytes, 
			      &valid_bytes, 
			      &rsvd);
	if (encode_debug) {
		pr_err("\n"
			"encode_status	: %d\n"
			"out_ch		: %d\n"
			"out_sr		: %d\n"
			"dummy		: %d\n"
			"dummy_bytes	: %d\n"
			"sample_read	: %d\n"
			"total_bytes	: %d\n"
			"valid_bytes	: %d\n"
			"rsvd		: %d\n"
			"\n",
			encode_status, out_ch, out_sr, dummy, dummy_bytes,
			sample_read, total_bytes, valid_bytes, rsvd);
	}

	if ((encode_status != 0) && (encode_status != 6)) {
		pr_err("%s: Encode status error: %d\n", __func__, encode_status);
		return -EFAULT;
	}

	out_bytes = valid_bytes;

	ret = copy_to_user((void __user *)temp.dst,
				out, out_bytes * sizeof(uint8_t));
	if (ret) {
		pr_err("Failed to copy encode data to user\n");
		return -EFAULT;
	}
	
	return (out_bytes * sizeof(uint8_t));
}
#else
static int32_t tl421_encode_aac(unsigned long arg)
{
	return 0;
}
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_AEC
static int tl421_aec_get_buf(unsigned long arg)
{
	if (copy_to_user((void __user *)arg, &tl421->aec.buf_addr,
				sizeof(struct aec_buf))) {
		pr_err("copy argument failed\n");
		return -EFAULT;
	}

	return 0;
}

static int tl421_aec_process(unsigned long arg)
{
	int m;
	int out_samples, ret = 0;
	int cmd = 0;
	struct aec_codec temp;
	void __iomem *base = tl421->ipc.base;
	uint8_t *spk_in = tl421->aec.buf_addr.spk_in_virt;
	uint8_t *mic_in = tl421->aec.buf_addr.mic_in_virt;
	uint8_t *out = tl421->aec.buf_addr.aec_out_virt;
	int logger_len;
	uint8_t *logger_out = tl421->aec.dbg.buf_addr.aec_dbg_out_virt;
	uint8_t test = 0;
	int timeout = TIMEOUT_1S / WAIT_CMD_SLP;

#ifdef TL421_AEC_PROFILING
	long long ts, te;
	ts = tl421_get_ns();
#endif

	if(spk_in == 0 || mic_in == 0 || out == 0) {
		pr_err("dsp initiate abnormally! illegal spk/mic/out addr\n");
		return -EFAULT;
	}

	if (copy_from_user(&temp, (struct aec_codec *)arg,
				sizeof(struct aec_codec))) {
		pr_err("copy argument failed\n");
		return -EFAULT;
	}

	if (temp.len > tl421->aec.buf_addr.data_len) {
		pr_err("ERROR: input data len(%d) larger than malloc(%d)\n", temp.len, tl421->aec.buf_addr.data_len);
		return -ENOMEM;
	}

#if 0
	//test 1 data from spk or mic
	if (copy_from_user(&test, (void __user *)temp.spk_src,
				1)) {
		pr_err("illegal argument: spk virtual addr\n");
		return -EINVAL;
	}

	if (copy_from_user(&test, (void __user *)temp.mic_src,
				1)) {
		pr_err("illegal argument: mic virtual addr\n");
		return -EINVAL;
	}

	if (copy_to_user((void __user *)temp.dst, &test,
				1)) {
		pr_err("illegal argument: out virtual addr\n");
		return -EINVAL;
	}
#endif


#ifdef TL421_AEC_DUMP_INOUT
	for(m = 0; m < temp.len; m++)
		printk(KERN_ERR"spk_in[%d] = 0x%02x ", m, *(spk_in + m));

	for(m = 0; m < temp.len; m++)
		printk(KERN_ERR"mic_in[%d] = 0x%02x ", m, *(mic_in + m));
#endif

	while(1) {

		cmd = readl(base + IPC_COM0);
		if(cmd == AEC_CMD_NONE)
			break;

		usleep_range(WAIT_CMD_SLP, WAIT_CMD_SLP);
		if(timeout-- < 0) {
			pr_err("timeout waiting for last cmd completed\n");
			return -EBUSY;
		}
	}


//	pr_err(" IPC_MB_MGST0, mesg0 cnt = 0x%x\n", readl(base + IPC_MB_MGST0) & IPC_MEST0_CNT_MASK);
	writel(0x0, base + IPC_MB_MGST0);
	writel(0x0, base + IPC_MB_MGST0);
	writel(0x0, base + IPC_MB_MGST0);
	writel(0x0, base + IPC_MB_MGST0);

//	pr_err(" IPC_MB_MGST1, mesg1 cnt = 0x%x\n", readl(base + IPC_MB_MGST1) & IPC_MEST1_CNT_MASK);
	writel(0x0, base + IPC_MB_MGST1);
	writel(0x0, base + IPC_MB_MGST1);
	writel(0x0, base + IPC_MB_MGST1);
	writel(0x0, base + IPC_MB_MGST1);

//	pr_err(" IPC_MB_MESG0, spk_buf = 0x%x\n", tl421->aec.buf_addr.spk_in_phy);
	writel(tl421->aec.buf_addr.spk_in_phy, base + IPC_MB_MESG0);
//	pr_err(" IPC_MB_MESG0, mic_buf = 0x%x\n", tl421->aec.buf_addr.mic_in_phy);
	writel(tl421->aec.buf_addr.mic_in_phy, base + IPC_MB_MESG0);
//	pr_err(" IPC_MB_MESG0, sout_buf = 0x%x\n", 0);
//	writel(tl421->aec.buf_addr.aec_out_phy, base + IPC_MB_MESG0);
//	pr_err(" IPC_MB_MESG0, mout_buf = 0x%x\n", tl421->aec.buf_addr.aec_out_phy);
	writel(tl421->aec.buf_addr.aec_out_phy, base + IPC_MB_MESG0);

//	pr_err(" IPC_MB_MESG1, len = %d\n", temp.len);
	writel(temp.len, base + IPC_MB_MESG0);
	//writel(temp.len, base + IPC_MB_MESG0);



	//pr_err(" init IPC_REP0 to 0\n");
	writel(0, base + IPC_REP0);
	//pr_err(" init IPC_REP1 to 0\n");
	writel(0, base + IPC_REP1);
	//pr_err(" init IPC_REP2 to 0\n");
	writel(0, base + IPC_REP2);

//	pr_err(" IPC_COM0, cmd = CMD_START_AEC(%d)\n", 0x1);
	writel(AEC_CMD_START_AEC_TX, base + IPC_COM0);

	ret = tl421_aec_wait_mission_complete();

	if(tl421->aec.dbg.fmt.dbg_mode == AEC_LOGGER_DBG) {
		logger_len = readl(base + IPC_REP1);
		temp.dbg_len = logger_len;
		pr_err("logger_len = %d\n", logger_len);
#ifdef TL421_AEC_DUMP_INOUT
		for(m = 0; m < 16; m++)
			printk(KERN_ERR"logger_head[%d] = 0x%02x ", m, *(logger_out + m));

		printk("\n\n");

		for(m = 0; m < (logger_len>64?64:logger_len); m++)
			printk(KERN_ERR"logger_out[%d] = 0x%02x ", m, *((logger_out+16) + m));
#endif
	}

	out_samples = readl(base + IPC_REP0);

#ifdef TL421_AEC_DUMP_INOUT
	pr_err("out_sample = %d\n", out_samples);
	for(m = 0; m < out_samples; m++)
		printk(KERN_ERR"out[%d] = 0x%02x ", m, *(out + m));
#endif


	if(ret < 0)
		return ret;

	temp.len = out_samples;
	if (copy_to_user((void __user *)arg, &temp,
				sizeof(struct aec_codec))) {
		pr_err("copy_to_user failed\n");
		return -EFAULT;
	}

#ifdef TL421_AEC_PROFILING
	te = tl421_get_ns();
	pr_err("tl421_aec_process complete, spend %lldns\n", (te - ts));
#endif
	return (out_samples);
}
#else
static int tl421_aec_get_buf(unsigned long arg)
{
	return 0;
}

static int tl421_aec_process(unsigned long arg)
{
	return 0;
}
#endif

static int ceva_dsp_open(struct inode *inode, struct file *file)
{
	int dev = iminor(inode) & 0x0f;
	int ret = 0;

	mutex_lock(&ceva_dsp_mutex);

	switch (dev) {
	case CEVA_TL421:
		if (test_and_set_bit(0, &tl421->in_use)) {
			pr_err("ceva_dsp_open failed: device is busy\n");
			ret = -EBUSY;
			goto out;
		}
		break;

	default:
		ret = -ENODEV;
		break;
	}

out:
	mutex_unlock(&ceva_dsp_mutex);
	
	return ret;
}

static int ceva_dsp_release(struct inode *inode, struct file *file)
{
	int dev = iminor(inode) & 0x0f;
	int ret = 0;

	mutex_lock(&ceva_dsp_mutex);
	switch (dev) {
		case CEVA_TL421:
#ifdef CONFIG_Q3F_DSP_SUPPORT_DECODE
			if (tl421->dec->ac) {
				dma_free_coherent(NULL, sizeof(struct audio_codec), 
							tl421->dec->ac, tl421->dec->p_ac);
				tl421->dec->ac = NULL;
			}
			if (tl421->dec->in) {
				dma_free_coherent(NULL, 1550*4*sizeof(uint16_t), 
							tl421->dec->in, tl421->dec->p_in);
				tl421->dec->in = NULL;
			}
			if (tl421->dec->out) {
				dma_free_coherent(NULL, 2048*4*sizeof(uint16_t), 
							tl421->dec->out, tl421->dec->p_out);
				tl421->dec->out = NULL;
			}
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE
			if (tl421->enc->ac) {
				dma_free_coherent(NULL, sizeof(struct audio_codec), 
							tl421->enc->ac, tl421->enc->p_ac);
				tl421->enc->ac = NULL;
			}
			if (tl421->enc->in) {
				dma_free_coherent(NULL, AACENC_IN_SIZE*sizeof(uint8_t), 
							tl421->enc->in, tl421->enc->p_in);
				tl421->enc->in = NULL;
			}
			if (tl421->enc->out) {
				dma_free_coherent(NULL, AACENC_OUT_SIZE*sizeof(uint8_t), 
							tl421->enc->out, tl421->enc->p_out);
				tl421->enc->out = NULL;
			}
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_AEC
			if (tl421->aec.buf_addr.spk_in_virt) {
				dma_free_coherent(NULL, tl421->aec.buf_addr.data_len,
						  tl421->aec.buf_addr.spk_in_virt, tl421->aec.buf_addr.spk_in_phy);
				tl421->aec.buf_addr.spk_in_virt = NULL;
				tl421->aec.buf_addr.spk_in_phy = 0;
			}
			if (tl421->aec.buf_addr.mic_in_virt) {
				dma_free_coherent(NULL, tl421->aec.buf_addr.data_len,
						  tl421->aec.buf_addr.mic_in_virt, tl421->aec.buf_addr.mic_in_phy);
				tl421->aec.buf_addr.mic_in_virt = NULL;
				tl421->aec.buf_addr.mic_in_phy = 0;
			}
			if (tl421->aec.buf_addr.aec_out_virt) {
				dma_free_coherent(NULL, tl421->aec.buf_addr.data_len,
						  tl421->aec.buf_addr.aec_out_virt, tl421->aec.buf_addr.aec_out_phy);
				tl421->aec.buf_addr.aec_out_virt = NULL;
				tl421->aec.buf_addr.aec_out_phy = 0;
			}
			if (tl421->aec.dbg.buf_addr.aec_dbg_out_virt) {
				dma_free_coherent(NULL, tl421->aec.dbg.buf_addr.dbg_len,
						  tl421->aec.dbg.buf_addr.aec_dbg_out_virt, tl421->aec.dbg.buf_addr.aec_dbg_out_phy);
				tl421->aec.dbg.buf_addr.aec_dbg_out_virt = NULL;
				tl421->aec.dbg.buf_addr.aec_dbg_out_phy = 0;
			}
			if (tl421->aec.irq != 0) {
				free_irq(tl421->aec.irq, &tl421->pdev->dev);
				tl421->aec.irq = 0;
			}
#endif
			request_reload = 1;

			tl421_module_poweroff();

			clear_bit(0, &tl421->in_use);

			break;

	default:
		pr_err("CEVA DSP driver: Unknown minor device: %d\n", dev);
		ret = -ENXIO;
		break;
	}
	mutex_unlock(&ceva_dsp_mutex);

	return ret;
}

static ssize_t ceva_dsp_read(struct file *file, char __user *buf,
					size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t ceva_dsp_write(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	return 0;
}

static int ceva_dsp_mmap(struct file *fp, struct vm_area_struct *vma)
{
	vma->vm_page_prot = pgprot_dmacoherent(vma->vm_page_prot);

	//printk(KERN_INFO "remaping from phy addr(%p) to virt addr (%08lx~%08lx)\n", vma->vm_pgoff<<12, vma->vm_start, vma->vm_end);
	return remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
			vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

static long 
ceva_dsp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int dev = iminor(file_inode(file)) & 0x0f;
	long ret = 0;

	if (_IOC_TYPE(cmd) != TL421_MAGIC 
			|| _IOC_NR(cmd) > TL421_IOCMAX)
		return -ENOTTY;

	switch (dev) {
	case CEVA_TL421:
		switch (cmd) {
		case TL421_GET_VER:
			return TL421_VER;
		case TL421_SET_FMT:
			mutex_lock(&ceva_dsp_mutex);
			ret = tl421_set_format(arg);
			mutex_unlock(&ceva_dsp_mutex);
			break;
		case TL421_GET_FMT:
			break;
		case TL421_ENCODE:
			mutex_lock(&ceva_dsp_mutex);
			ret = tl421_encode_aac(arg);
			mutex_unlock(&ceva_dsp_mutex);
			break;
		case TL421_DECODE:
			mutex_lock(&ceva_dsp_mutex);
			ret = tl421_decode_aac(arg);
			mutex_unlock(&ceva_dsp_mutex);
			break;
		case TL421_COMP_RATIO:
			break;
		case TL421_CONVERT:
			break;
		case TL421_IOTEST:
			break;
		case TL421_AEC_SET_FMT:
			mutex_lock(&ceva_dsp_mutex);
			ret = tl421_aec_set_format(arg);
			mutex_unlock(&ceva_dsp_mutex);
			break;
		case TL421_AEC_GET_BUF:
			mutex_lock(&ceva_dsp_mutex);
			ret = tl421_aec_get_buf(arg);
			mutex_unlock(&ceva_dsp_mutex);
			break;
		case TL421_AEC_PROCESS:
			mutex_lock(&ceva_dsp_mutex);
			ret = tl421_aec_process(arg);
			mutex_unlock(&ceva_dsp_mutex);
			break;

		case TL421_AEC_SET_DBG_FMT:
			mutex_lock(&ceva_dsp_mutex);
			ret = tl421_aec_set_dbg_format(arg);
			mutex_unlock(&ceva_dsp_mutex);
			break;

		case TL421_AEC_RELEASE:
			ceva_dsp_release(file_inode(file), file);
			break;

		default:
			pr_err("CEVA DSP driver: Unknown command %d\n", cmd);
			return -EINVAL;
		}
		break;

	default:
		pr_err("CEVA DSP driver: Unknown minor device: %d\n", dev);
		return -ENXIO;
	}

	return ret;
}

static struct file_operations ceva_dsp_fops = {
	.owner		= THIS_MODULE,
	.read		= ceva_dsp_read,
	.write		= ceva_dsp_write,
	.unlocked_ioctl	= ceva_dsp_ioctl,
	.open		= ceva_dsp_open,
	.release	= ceva_dsp_release,
	.llseek		= noop_llseek,
	.mmap		= ceva_dsp_mmap,
};

static void mem_region_identify(struct dsp_mem_region *mem)
{
	if (!strcmp(mem->name, "dsp_sysm")) {
		tl421->sysm.res = mem->res;
		tl421->sysm.base = mem->iobase;
	} else if (!strcmp(mem->name, "dsp_ptcm")) {
		tl421->ptcm.res = mem->res;
		tl421->ptcm.base = mem->iobase;
	} else if (!strcmp(mem->name, "dsp_ptcm_ext")) {
		tl421->ptcm_ext.res = mem->res;
		tl421->ptcm_ext.base = mem->iobase;
	} else if (!strcmp(mem->name, "dsp_dtcm")) {
		tl421->dtcm.res = mem->res;
		tl421->dtcm.base = mem->iobase;
	} else if (!strcmp(mem->name, "dsp_dtcm_ext")) {
		tl421->dtcm_ext.res = mem->res;
		tl421->dtcm_ext.base = mem->iobase;
	} else if (!strcmp(mem->name, "dsp_ipc")) {
		tl421->ipc.res = mem->res;
		tl421->ipc.base = mem->iobase;
	}
}

static void dsp_mem_region_deinit(void)
{
	if (tl421->sysm.res)
		release_resource(tl421->sysm.res);
	if (tl421->sysm.base)
		iounmap(tl421->sysm.base);
	if (tl421->ptcm.res)
		release_resource(tl421->ptcm.res);
	if (tl421->ptcm.base)
		iounmap(tl421->ptcm.base);
	if (tl421->ptcm_ext.res)
		release_resource(tl421->ptcm_ext.res);
	if (tl421->ptcm_ext.base)
		iounmap(tl421->ptcm_ext.base);
	if (tl421->dtcm.res)
		release_resource(tl421->dtcm.res);
	if (tl421->dtcm.base)
		iounmap(tl421->dtcm.base);
	if (tl421->dtcm_ext.res)
		release_resource(tl421->dtcm_ext.res);
	if (tl421->dtcm_ext.base)
		iounmap(tl421->dtcm_ext.base);
	if (tl421->ipc.res)
		release_resource(tl421->ipc.res);
	if (tl421->ipc.base)
		iounmap(tl421->ipc.base);
}

static int dsp_mem_region_init(void)
{
	int i;
	struct dsp_mem_region *mem;

	for (i = 0; i < ARRAY_SIZE(dsp_mem_region); i++) {
		mem = &dsp_mem_region[i];

		if (!strcmp(mem->name, "dsp_ptcm_ext")) {
			mem->start = dsp_reserve_start;
			//pr_err("dsp_reserve_start=0x%x\n", dsp_reserve_start);
		} else if (!strcmp(mem->name, "dsp_dtcm_ext")) {
			mem->start = dsp_reserve_start + 0x80000;
		}

		mem->res = request_mem_region(mem->start, mem->size, mem->name);
		if (mem->res == NULL) {
			pr_err("failed to request mem region %s for dsp\n", mem->name);
			goto err;
		}

		mem->iobase = ioremap_nocache(mem->start, mem->size);
		if (mem->iobase == NULL) {
			pr_err("failed to remap mem region %s for dsp\n", mem->name);
			goto err;
		}

		mem_region_identify(mem);
	}

	return 0;

err:
	dsp_mem_region_deinit();
	return -1;
}

static int ceva_dsp_init_driver(void)
{
	int i, err = 0;

	pr_err("CEVA TL421 driver installed ... ...\n");

	tl421 = kzalloc(sizeof(struct dsp_device), GFP_KERNEL);
	if (tl421 == NULL) {
		pr_err("Failed to alloc mem for tl421\n");
		err = -ENOMEM;
		goto err_alloc;
	}

	tl421->pdev = platform_device_register_simple("cevadsp", 0, NULL, 0);
	if (IS_ERR(tl421->pdev)) {
		pr_err("Failed to register device for ceva-tl421\n");
		err = -EINVAL;
		goto err_pdev;
	}

	err = register_chrdev(CEVA_TL421_MAJOR, "cevadsp", &ceva_dsp_fops);
	if (err) {
		pr_err("CEVA TL421 driver: Unable to register driver\n");
		err = -ENODEV;
		goto err_chrdev;
	}

	ceva_dsp_class = class_create(THIS_MODULE, "cevadsp");
	if (IS_ERR(ceva_dsp_class)) {
		err = PTR_ERR(ceva_dsp_class);
		goto err_class;
	}
	device_create(ceva_dsp_class, NULL, MKDEV(CEVA_TL421_MAJOR, 0), NULL, "cevadsp-tl421");

	tl421->clk = clk_get_sys("imap-dsp", "q3f-dsp");
	if (IS_ERR(tl421->clk)) {
		pr_err("Failed to get dsp clk\n");
		goto err_clk;
	}
	clk_set_rate(tl421->clk, 256000000);
	//pr_err("dsp core clk = %ld\n", clk_get_rate(tl421->clk));
	clk_prepare_enable(tl421->clk);

	err = dsp_mem_region_init();
	if (err < 0)
		goto err_mem_region;

#ifdef CONFIG_Q3F_DSP_SUPPORT_AEC
	for (i = 0; i < ARRAY_SIZE(aec_fw); i++) {
		err = tl421_fw_cb(&aec_fw[i]);
		if (err)
			goto err_aec_fw;
	}
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_DECODE
	tl421->dec = kzalloc(sizeof(struct codec_phys), GFP_KERNEL);
	if (tl421->dec == NULL) {
		pr_err("Failed to alloc mem for tl421->dec\n");
		err = -ENOMEM;
		goto err_aacdec_fw;
	}

	for (i = 0; i < ARRAY_SIZE(aac_dec_fw); i++) {
		err = tl421_fw_cb(&aac_dec_fw[i]);
		if (err)
			goto err_aacdec_fw;
	}
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE
	tl421->enc = kzalloc(sizeof(struct codec_phys), GFP_KERNEL);
	if (tl421->enc == NULL) {
		pr_err("Failed to alloc mem for tl421->enc\n");
		err = -ENOMEM;
		goto err_aacenc_fw;
	}

	for (i = 0; i < ARRAY_SIZE(aac_enc_fw); i++) {
		err = tl421_fw_cb(&aac_enc_fw[i]);
		if (err)
			goto err_aacenc_fw;
	}
#endif

	tl421->info = kzalloc(sizeof(struct codec_info), GFP_KERNEL);
	if (tl421->info == NULL) {
		pr_err("Failed to alloc mem for tl421->info\n");
		err = -ENOMEM;
		goto err_malloc_info;
	}

	tl421->pre_info = kzalloc(sizeof(struct codec_info), GFP_KERNEL);
	if (tl421->pre_info == NULL) {
		pr_err("Failed to alloc mem for tl421->pre_info\n");
		err = -ENOMEM;
		goto err_malloc_info;
	}

#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE
	err = request_irq(TOARM_INT, dsp_toarm_isr, 0, "dsp-toarm", &tl421->pdev->dev);
	if (err < 0) {
		pr_err("failed to request irq %d for dsp\n", TOARM_INT);
		goto err_malloc_info;
	}
	init_completion(&dsp_complete);
#endif

	pr_err("CEVA TL421 driver installed OK!!!\n");
	return 0;

err_malloc_info:
	if (tl421->info)
		kfree(tl421->info);

#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE
err_aacenc_fw:
	for (i = 0; i < ARRAY_SIZE(aac_enc_fw); i++) {
		if (aac_enc_fw[i].data)
			kfree(aac_enc_fw[i].data);
	}
	if (tl421->enc)
		kfree(tl421->enc);
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_DECODE
err_aacdec_fw:
	for (i = 0; i < ARRAY_SIZE(aac_dec_fw); i++) {
		if (aac_dec_fw[i].data)
			kfree(aac_dec_fw[i].data);
	}
	if (tl421->dec)
		kfree(tl421->dec);
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_AEC
err_aec_fw:
	for (i = 0; i < ARRAY_SIZE(aec_fw); i++) {
		if (aec_fw[i].data)
			kfree(aec_fw[i].data);
	}
#endif

err_mem_region:
	dsp_mem_region_deinit();
	clk_disable_unprepare(tl421->clk);
	clk_put(tl421->clk);
	tl421->clk = NULL;
err_clk:
	class_destroy(ceva_dsp_class);
err_class:
	unregister_chrdev(CEVA_TL421_MAJOR, "cevadsp");
err_chrdev:
	platform_device_unregister(tl421->pdev);
err_pdev:
	kfree(tl421);
err_alloc:
	pr_err("CEVA TL421 driver error exit!!!\n");
	return err;
}
module_init(ceva_dsp_init_driver);

static void ceva_dsp_remove_driver(void)
{
	int i;

	kfree(tl421->pre_info);
	kfree(tl421->info);

#ifdef CONFIG_Q3F_DSP_SUPPORT_AEC
	for (i = 0; i < ARRAY_SIZE(aec_fw); i++) {
		if (aec_fw[i].data)
			kfree(aec_fw[i].data);
	}

	if (tl421->aec.buf_addr.spk_in_virt) {
		dma_free_coherent(NULL, tl421->aec.buf_addr.data_len,
				  tl421->aec.buf_addr.spk_in_virt, tl421->aec.buf_addr.spk_in_phy);
		tl421->aec.buf_addr.spk_in_virt = NULL;
		tl421->aec.buf_addr.spk_in_phy = 0;
	}
	if (tl421->aec.buf_addr.mic_in_virt) {
		dma_free_coherent(NULL, tl421->aec.buf_addr.data_len,
				  tl421->aec.buf_addr.mic_in_virt, tl421->aec.buf_addr.mic_in_phy);
		tl421->aec.buf_addr.mic_in_virt = NULL;
		tl421->aec.buf_addr.mic_in_phy = 0;
	}
	if (tl421->aec.buf_addr.aec_out_virt) {
		dma_free_coherent(NULL, tl421->aec.buf_addr.data_len,
				  tl421->aec.buf_addr.aec_out_virt, tl421->aec.buf_addr.aec_out_phy);
		tl421->aec.buf_addr.aec_out_virt = NULL;
		tl421->aec.buf_addr.aec_out_phy = 0;
	}
	if (tl421->aec.dbg.buf_addr.aec_dbg_out_virt) {
		dma_free_coherent(NULL, tl421->aec.dbg.buf_addr.dbg_len,
				  tl421->aec.dbg.buf_addr.aec_dbg_out_virt, tl421->aec.dbg.buf_addr.aec_dbg_out_phy);
		tl421->aec.dbg.buf_addr.aec_dbg_out_virt = NULL;
		tl421->aec.dbg.buf_addr.aec_dbg_out_phy = 0;
	}
#endif

#ifdef CONFIG_Q3F_DSP_SUPPORT_DECODE
	for (i = 0; i < ARRAY_SIZE(aac_dec_fw); i++) {
		if (aac_dec_fw[i].data)
			kfree(aac_dec_fw[i].data);
	}

	if (tl421->dec->ac)
		dma_free_coherent(NULL, sizeof(struct audio_codec), 
					tl421->dec->ac, tl421->dec->p_ac);
	if (tl421->dec->in)
		dma_free_coherent(NULL, 1550*4*sizeof(uint16_t), 
					tl421->dec->in, tl421->dec->p_in);
	if (tl421->dec->out)
		dma_free_coherent(NULL, 2048*4*sizeof(uint16_t), 
					tl421->dec->out, tl421->dec->p_out);
	kfree(tl421->dec);
#endif
	
#ifdef CONFIG_Q3F_DSP_SUPPORT_ENCODE
	for (i = 0; i < ARRAY_SIZE(aac_enc_fw); i++) {
		if (aac_enc_fw[i].data)
			kfree(aac_enc_fw[i].data);
	}

	if (tl421->enc->ac)
		dma_free_coherent(NULL, sizeof(struct audio_codec), 
					tl421->enc->ac, tl421->enc->p_ac);
	if (tl421->enc->in)
		dma_free_coherent(NULL, AACENC_IN_SIZE*sizeof(uint8_t), 
					tl421->enc->in, tl421->enc->p_in);
	if (tl421->enc->out)
		dma_free_coherent(NULL, AACENC_OUT_SIZE*sizeof(uint8_t), 
					tl421->enc->out, tl421->enc->p_out);
	kfree(tl421->enc);
#endif

	dsp_mem_region_deinit();
	clk_disable_unprepare(tl421->clk);
	clk_put(tl421->clk);
	tl421->clk = NULL;
	device_destroy(ceva_dsp_class, MKDEV(CEVA_TL421_MAJOR, 0));
	class_destroy(ceva_dsp_class);
	unregister_chrdev(CEVA_TL421_MAJOR, "cevadsp");
	platform_device_unregister(tl421->pdev);
	kfree(tl421);
}
module_exit(ceva_dsp_remove_driver);

MODULE_DESCRIPTION("CEVA TL421 dsp driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("david <david.sun@infotm.com>");
