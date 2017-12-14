#include <asm-arm/io.h>
#include <bootlist.h>
#include <dramc_init.h>
#include <items.h>
#include <isi.h>
#include <ius.h>
#include <hash.h>
#include <preloader.h>
#include <rballoc.h>
#include <board.h>
#include <rtcbits.h>
#include <drama9.h>
#include <linux/types.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>
#include <asm-arm/setup.h>
#include <asm-arm/types.h>
#include <asm-arm/arch-apollo3/imapx_uart.h>
#include <configs/imap_apollo.h>
#include <asm-arm/arch-apollo3/imapx_pads.h>
#include <asm-arm/arch-apollo3/imapx_spiflash.h>
#include <asm-arm/arch-apollo3/ssp.h>

#include <ssp.h>
#include <image.h>
#ifdef CONFIG_BOARD_HAVE_UDC_BURN
#include <linkpc.h>
#endif

#define UBOOT0_DEBUG_NAND_IN_KERNEL

static const char *part_name[] = {
	"uboot",
	"item",
	"ramdisk",
	"kernel0",
	"kernel1",
	"system",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"none",
	"uboot1",
	"reserved",
	"none"
};

static char *images_parts[IMAGE_NONE + 1] = {
    "part0",   "part1",  "part2",  "part3",
    "part4",   "part5",  "part6",  "part7",
    "part8",   "part9",  "part10", "part11",
    "part12",  "part13", "part13", "part14",
    "part15",  "part16", "part17",
};
#undef IROM_VS_READ

#define PADS_MODE_OUT     2
struct irom_export *irf = NULL;
struct tag *params = (struct tag *) TAG_START;
int boot_dev_id, boot_mode = BOOT_NORMAL;
int offs_image[IMAGE_NONE];
int dram_size = 0;
int loop_per_jiffies = 0;

#define SDMMC_CDETECT   0x50
int is_mmc_present (void) {
	return (readl(SD1_BASE_ADDR + SDMMC_CDETECT) & 0x1) == 0 ? 1 : 0;
}

extern int dramc_init(struct irom_export *irf, int wake_up);
extern void set_up_watchdog(int ms);
extern void imapx_wdt_ctrl(uint32_t mode);

extern int nand_uboot0_init(void);
extern int nand_uboot0_read(unsigned char *buf, int dev_id, int offset, int len);
extern void param_transmit_to_uboot1(void);

void emmc_init(int boot_dev_id) {
#ifdef CONFIG_BOARD_BOOT_DEV_EMMC
	init_mmc();
	mmc_init_param(0);
#else
	irf->vs_assign_by_id(boot_dev_id, 1);
#endif
	return ;
}

int  mmc_read(uint8_t *buf, int start,int length, int extra)
{
#ifdef CONFIG_BOARD_BOOT_DEV_EMMC
	return vs_read_mmc0(buf, start, length, extra);
#else
	return irf->vs_read(buf, start, length, extra);
#endif

}

void flash_init(int boot_dev_id) {
#ifdef CONFIG_BOARD_BOOT_DEV_FLASH
	flash_nand_vs_reset(); // multiple wires reset
#else
	irf->vs_assign_by_id(boot_dev_id, 1);
#endif
	return ;
}

int  flash_read(uint8_t *buf, long start, int length, int extra)
{
#ifdef CONFIG_BOARD_BOOT_DEV_FLASH
	return flash_nand_vs_read(buf, start, length, extra);
#else
	return irf->vs_read(buf, start, length, extra);
#endif

}


//base function
void int_endian_change(unsigned int *out, unsigned int in)
{
	*out = ( in << 24 ) | ( ( in & 0xff00 ) << 8 ) | ( ( in & 0xff0000 ) >> 8 ) | ( in >> 24 );
}

size_t strlen(const char *s)
{
	const char *sc = s;
	while ( *sc++ );
	return sc - s - 1;
}

char *strcpy(char *dst, const char *src)
{
	char *tmp = dst;
	while ( *tmp++ = *src++ );
	return dst;
}

struct FREQ_ST {
	uint32_t freq;
	uint32_t val;
};

static struct FREQ_ST fr_para[] = {
	{1992,  0x0052},
	{1944,  0x0050},
	{1896,  0x004e},
	{1848,  0x004c},
	{1800,  0x004a},
	{1752,  0x0048},
	{1704,  0x0046},
	{1656,  0x0044},
	{1608,  0x0042},
	{1560,  0x0040},
	{1512,  0x003e},
	{1464,  0x003c},
	{1416,  0x003a},
	{1368,  0x0038},
	{1320,  0x0036},
	{1272,  0x0034},
	{1224,  0x0032},
	{1176,  0x0030},
	{1128,  0x002e},
	{1080,  0x002c},
	{1032,  0x002a},
	{996,   0x1052},
	{948,   0x104e},
	{900,   0x104a},
	{852,   0x1046},
	{804,   0x1042},
	{756,   0x103e},
	{708,   0x103a},
	{660,   0x1036},
	{612,   0x1032},
	{564,   0x102e},
	{516,   0x102a},
	{468,   0x204d},
	{420,   0x2045},
	{372,   0x203d},
	{324,   0x2035},
	{276,   0x202d},
	{228,   0x304b},
	{180,   0x303b},
	{132,   0x302b},
};
static void reset_APLL(uint32_t idx)
{
	writel(0x00, CLK_SYSM_ADDR + 0x0c);
	/*set freq into sys reg*/
	writel(fr_para[idx].val & 0xff, CLK_SYSM_ADDR + 0x00);
	writel((fr_para[idx].val>>8) & 0xff, CLK_SYSM_ADDR + 0x04);
	writel(0x01, SYS_SYSM_ADDR + 0x18);
	while(readl(SYS_SYSM_ADDR + 0x18) & 0x1);
}


/* x15 */
void clock_config(void)
{
	int i;
	irf->module_set_clock(BUS_CLK_BASE(7), PLL_E, 5);
	irf->module_set_clock(SD_CLK_BASE(0), PLL_E, 30);
	irf->module_set_clock(SD_CLK_BASE(1), PLL_E, 30);
	irf->module_set_clock(NFECC_CLK_BASE, PLL_E, 2);
	irf->set_pll(PLL_E, 0x1062);

	irf->module_set_clock(FABRIC_CLK_BASE, PLL_E, 5);
	irf->module_set_clock(APB_CLK_BASE, PLL_E, 9);

	reset_APLL(25);
	for(i = 1; i < 7; i++) {
		irf->module_set_clock(BUS_CLK_BASE(i), PLL_A, 3);
	}

	irf->module_set_clock(CRYPTO_CLK_BASE, PLL_A,  3);
	irf->module_set_clock(SD_CLK_BASE(2), PLL_A, 19);
}


void cpu_freq_config(void)
{
	int i = 0, j = 0;
	int A_freq = 0;
	/*set CPU freq*/
	if(item_exist("soc.cpu.freq")) {
		A_freq = item_integer("soc.cpu.freq", 0) + 804;         //804: cpu freq base of kernel
		j = sizeof(fr_para)/sizeof(struct FREQ_ST);
		if(A_freq <= fr_para[0].freq && A_freq >= fr_para[j-1].freq){
			//while(i--)
			for(i = 0; i<j; i++)
			{
				if(fr_para[i].freq <= A_freq)
					break;
			}
			reset_APLL(i);
		}
	}
}

void serial_setbrg(void)
{
	int32_t clk;
	int16_t ibr, fbr;
	int32_t baudrate = 115200;

#if defined(CONFIG_IMAPX_FPGATEST)
	clk = 40000000;
#else
	clk = 45692307;
#endif
	ibr = (clk/(16*baudrate)) & 0xffff;
	fbr = (((clk - ibr*(16*baudrate)) *64)/(16*baudrate)) & 0x3f;
	writel(ibr, UART_IBRD);
	writel(fbr, UART_FBRD);
}

//peripheral
int init_serial(void)
{
	int32_t val;

	/* reset uart control*/
	irf->module_enable(UART_SYSM_ADDR);

	/* set format: data->8bit stop->1-bit */
	val = readl(UART_LCRH);
	val &= ~0xff;
	val |= 0x60;
	writel(val, UART_LCRH);
	serial_setbrg();
	/* setup rx||tx */
	val = readl(UART_CR);
	val &=~0xff87;
	val |=0x301;
	writel(val, UART_CR);
	val = readl(UART_LCRH);
	val |= 0x10;
	writel(val, UART_LCRH);

	return 0 ;
}


void init_io(void)
{
	char *iob[] = {
		"initio.0", "initio.1",
		"initio.2", "initio.3",
		"initio.4", "initio.5",
		"initio.6", "initio.7",
		"initio.8", "initio.9"
	}, buf[64];
	int i, index, high, delay;

	for(i = 0; i < 10; i++) {
		if(item_exist(iob[i])) {
			item_string(buf, iob[i], 0);
			if(irf->strncmp(buf, "pads", 4) != 0)
				continue;
			index = item_integer(iob[i], 1);
			high  = item_integer(iob[i], 2);
			delay = item_integer(iob[i], 3);

			irf->pads_chmod(index, PADS_MODE_OUT, !!high);

			irf->printf("pads %d set to %d, delaying %dus\n",
					index, !!high, delay);
			irf->udelay(delay);
		} else
			break;
	}

	/* disable rtcgp3: pow_rnd pull down */
	writel(readl(RTC_GPPULL) | (1 << 3), RTC_GPPULL);
}


int calc_dram_M(void)
{
	int cs = 1, ch = 8, ca = 1, rd = 0;
	int sz;

	if(( sz = rbgetint ( "dramsize" )) != 0)
		goto __ok;

	if (item_exist( "memory.size")) {
		sz = item_integer("memory.size", 0) << 20;
		goto __ok;
	}

	if (item_exist("memory.cscount") && item_exist("memory.density") && item_exist( "memory.io_width") && item_exist ("memory.reduce_en")) {
		cs = item_integer ( "memory.cscount", 0 );
		ch = item_integer ( "memory.io_width", 0 );
		rd = item_integer ( "memory.reduce_en", 0 );
		ca = item_integer ( "memory.density", 0 );
		cs = ( cs == 3 ) ? 2 : 1;
	}

	sz = (ca > 20) ?
		(ca << 17) : // MB
		(ca << 27); // GB
	sz *= (ch == 8) ? 4 : 2;
	sz >>= !!rd;
	sz *= cs;

__ok:
	return (int)(( sz >> 20 ) - 1);
}

//flag
int get_boot_mode(void)
{
	int item_size = 0;
	uint8_t *upgrade_flag;

	if (boot_dev_id == DEV_IUS) {
		if (offs_image[IMAGE_RAMDISK] > 0)
			return BOOT_RECOVERY_IUS;
		else
			return BOOT_NORMAL;
	}
	upgrade_flag = (uint8_t *)irf->malloc(UPGRADE_PARA_SIZE);
	if(upgrade_flag == NULL)
		return -1;

	item_size = item_integer("part1", 1);
	if(boot_dev_id == DEV_MMC(0))
		mmc_read(upgrade_flag, offs_image[IMAGE_ITEM] + (item_size << 10), UPGRADE_PARA_SIZE, 0);
	else
		irf->vs_read(upgrade_flag, offs_image[IMAGE_ITEM] + (item_size << 10), UPGRADE_PARA_SIZE, 0);

	if (!irf->strncmp(upgrade_flag, "upgrade_flag=ota", strlen("upgrade_flag=ota"))) {
		//spl_printf("upgrade mode ota\n");
		irf->free(upgrade_flag);
		return BOOT_RECOVERY_OTA;
	} else {
		//spl_printf("boot normal \n");
		irf->free(upgrade_flag);
		return BOOT_NORMAL;
	}
}

//tag
void setup_start_tag(void)
{
	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size(tag_core);
	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;
	params = tag_next( params);
}

void setup_commandline_tag(char *commandline)
{
	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size = (sizeof(struct tag_header) + strlen(commandline) + 1 + 4) >> 2;
	strcpy(params->u.cmdline.cmdline, commandline);
	params = tag_next(params);
}

void setup_initrd_tag(ulong initrd_start, ulong initrd_end)
{
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size(tag_initrd);
	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;
	params = tag_next(params);
}

void setup_end_tag(void)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

void dev_read_image(int dev_id, image_type_t image_num, int offset, int size, uint8_t *data)
{

	if (dev_id == DEV_IUS) {
		irf->vs_read(data, offs_image[image_num] + offset, size, 0);
	} else if (dev_id == DEV_MMC(0)) {
		mmc_read(data, EMMC_IMAGE_OFFSET + offs_image[image_num] + offset, size, 0);
		//irf->vs_read(data, EMMC_IMAGE_OFFSET + offs_image[image_num] + offset, size, 0);
	} else if (dev_id == DEV_FLASH) {
		flash_read(data, offs_image[image_num] + offset, size, 0); // multiple wires read
	} else if (dev_id == DEV_SNND) {
		flash_read(data, offs_image[image_num] + offset, size, 0); // multiple wires read
	} else if (dev_id == DEV_NAND) {
        if (image_num == IMAGE_UBOOT1) {
            nand_uboot0_read(data, DEV_UBOOT1, offset, size);
        }
        if (image_num == IMAGE_ITEM) {
            nand_uboot0_read(data, DEV_ITEM, offset, size);
        }
	} else {
		irf->vs_read(data, offs_image[image_num] + offset, size, 0);
	}
	return;
}


/*
    console=ttyAMA3,115200 vmalloc=304M androidboot.platform=imapx800 mem=511M
    androidboot.serialno=iMAPb1c2a1a3 androidboot.hardware=GIX15EVB cma=67108864

    "console=ttyAMA3,115200 mem=511M rootfstype=ext4 root=/dev/mmcblk0p2 rw init=/init"
*/

void boot(void)
{
	__attribute__((noreturn)) void (*jump)(void);
	void (*theKernel)(int zero, int arch, uint params);
	ulong rd_start = 0, rd_end = 0;
	char buf_tmp[sizeof(image_header_t)];
	void *isi = (void *)(DRAM_BASE_PA + 0x8000);
	uint8_t *item_rrtb;
	unsigned int size;
	int image_type, _ms;
	char spi_parts[64], fs_type[16];

	dram_size = calc_dram_M();
	rbsetint ( "dramsize" , dram_size);

	irf->printf("boot retry boot_mode =%d \n", boot_mode);

retry:
    irf->printf("boot regry .... \n");
	if ((boot_mode == BOOT_RECOVERY_IUS) || (boot_mode == BOOT_RECOVERY_OTA) || (boot_mode == BOOT_RECOVERY_DEV))
		image_type = IMAGE_KERNEL_UPGRADE;
	else if (offs_image[IMAGE_UBOOT1] != 0)
		image_type = IMAGE_UBOOT1;
	else
		image_type = IMAGE_KERNEL;

   irf->printf("image_type = %d \n", image_type);

	if (offs_image[IMAGE_UBOOT1] != 0)
		image_type = IMAGE_UBOOT1;

	if (boot_dev_id == DEV_FLASH)
		flash_init(boot_dev_id);

	if (image_type == IMAGE_UBOOT1) {
        irf->printf("THIS IS IMAGE_UBOOT1 \n");
		irf->memset(isi, 0, ISI_SIZE);
		dev_read_image(boot_dev_id, image_type, 0, ISI_SIZE, (uint8_t *)isi);
		if (irf->isi_check_header(isi) != 0) {
			spl_printf("wrong uboot header: 0x%p\n", isi);
			offs_image[IMAGE_UBOOT1] = 0;
			goto retry;
		}

		jump = (void *)isi_get_entry(isi);
		_ms = get_timer(0);
		dev_read_image(boot_dev_id, image_type, 0, isi_get_size(isi) + ISI_SIZE, (uint8_t *)(isi_get_load(isi) - ISI_SIZE));
		spl_printf("uboot1(%d ms)\n", get_timer(_ms));

		image_type = IMAGE_ITEM;
		item_rrtb = rballoc("itemrrtb", ITEM_SIZE_NORMAL);
		dev_read_image(boot_dev_id, image_type, 0, ITEM_SIZE_NORMAL, item_rrtb);

        param_transmit_to_uboot1();

		spl_printf("jump\n");
		asm("dsb");

		(*jump)();

		irf->cdbg_shell();
		for(;;);

	} else {
	    irf->printf("This is not UBOOT-1 \n");
		dev_read_image(boot_dev_id, image_type, 0, sizeof(image_header_t), (uint8_t *)buf_tmp);
		image_header_t *p = (image_header_t *)buf_tmp;
		if(p->ih_magic != KERNEL_MAGIC) {
			spl_printf ( "wrong kernel header:magic 0x%x\n", p->ih_magic );
			if (image_type != IMAGE_KERNEL_UPGRADE) {
				boot_mode = BOOT_RECOVERY_DEV;
				goto retry;
			} else {
				irf->cdbg_shell();
				while ( 1 );
			}
		}

		int_endian_change(&theKernel, p->ih_ep);
		int_endian_change(&size, p->ih_size);

		_ms = get_timer(0);
		dev_read_image(boot_dev_id, image_type, 0, sizeof(image_header_t) + size, (uint8_t *)(theKernel));
		spl_printf("kernel(%d ms, %d K)\n", get_timer(_ms), size >> 10);
		theKernel += KERNEL_INNER_OFFS;

		if ((boot_mode == BOOT_RECOVERY_IUS) || (boot_mode == BOOT_RECOVERY_OTA) || (boot_mode == BOOT_RECOVERY_DEV)) {
			image_type = IMAGE_RAMDISK;
			dev_read_image(boot_dev_id, image_type, 0, sizeof(image_header_t), (uint8_t *)(RAMDISK_START - sizeof(image_header_t)));
			p = (image_header_t *)(RAMDISK_START - sizeof(image_header_t));
			int_endian_change(&size, p->ih_size);
			rd_start = RAMDISK_START;
			rd_end = rd_start + size;
			_ms = get_timer(0);
			dev_read_image(boot_dev_id, image_type, 0, sizeof(image_header_t) + size, (uint8_t *)(RAMDISK_START - sizeof(image_header_t)));
			spl_printf("rd(%d ms)\n", get_timer(_ms));
		}

		image_type = IMAGE_ITEM;
		_ms = get_timer(0);
		item_rrtb = rballoc("itemrrtb", ITEM_SIZE_NORMAL);
		dev_read_image(boot_dev_id, image_type, 0, ITEM_SIZE_NORMAL, item_rrtb);
		spl_printf("item(%d ms)\n", get_timer(_ms));

		//tag
		setup_start_tag();
		if ( rd_start && rd_end)
			setup_initrd_tag(rd_start, rd_end);

		if (boot_mode == BOOT_RECOVERY_IUS) {
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, loop_per_jiffies, dram_size, "ius");
	    }
		else if (boot_mode == BOOT_RECOVERY_OTA) {
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, loop_per_jiffies, dram_size, "ota");
	    }
		else if (boot_mode == BOOT_RECOVERY_DEV) {
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, loop_per_jiffies, dram_size, "dev");
		}
		else {
			if (boot_dev_id == DEV_FLASH) {
				//irf->sprintf(spi_parts, "part%d", IMAGE_SYSTEM);
				if (item_exist(spi_parts)) {
					if (!item_string(fs_type, spi_parts, 3)) {
						irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, loop_per_jiffies, dram_size, fs_type);
				    }
					else {
						irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, loop_per_jiffies, dram_size, "squashfs");
					}
				} else {
					irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, loop_per_jiffies, dram_size, "squashfs");
				}
			} else if (boot_dev_id == DEV_MMC(0)) {
				irf->sprintf(buf_tmp, CMDLINE_EMMC, loop_per_jiffies, dram_size);
			} else {
				if (irf->vs_assign_by_id(DEV_MMC(0), 1) == 0) {
					irf->sprintf(buf_tmp, CMDLINE_TF_CARD_EMMC, loop_per_jiffies, dram_size);
			    }
				else {
					irf->sprintf(buf_tmp, CMDLINE_TF_CARD, loop_per_jiffies, dram_size);
			    }
			}
		}


        #if defined(UBOOT0_DEBUG_NAND_IN_KERNEL)
        /* just for debug nandflash */
        param_transmit_to_uboot1();
        #endif



        irf->printf("cmdline: %s \n", buf_tmp);
		setup_commandline_tag ( buf_tmp );
		setup_end_tag();
		asm ( "dsb" );

		spl_printf("b kernel\n");
		theKernel(0, 0x08f9, TAG_START);
	}
}

int get_offset_ius(void)
{
	int ret = 1, count, type, i;
	int sd_present;
	struct ius_desc *desc;
	uint32_t size;

	struct ius_hdr *ius = irf->malloc(sizeof(struct ius_hdr));
	if ( !ius )
		return ret;

	irf->memset(ius, 0, sizeof(struct ius_hdr));
	irf->memset(offs_image, 0, IMAGE_NONE * sizeof(int));
	sd_present = is_mmc_present();

    if (sd_present) {
        irf->printf("sd card is present !!!!");
    } else {
        irf->printf("sd card is not present \n");
    }

	if(sd_present) {
		irf->vs_assign_by_id(DEV_MMC(1), 1);
		irf->vs_read ((uint8_t *)ius, IUS_DEFAULT_LOCATION, sizeof(struct ius_hdr), 0);
		if (ius->magic == IUS_MAGIC) {
			count = ius_get_count(ius);
			ret = 0;
            irf->printf("ius_get_count = %d \n", count);
			for (i = 0; i < count; i++) {
				desc = ius_get_desc (ius, i);
				type = ius_desc_offo(desc);
				size = ius_desc_length(desc);

				if ((size > 0) && (type < IMAGE_NONE))
					offs_image[type] = ius_desc_offi(desc);

                irf->printf("ius: ===> type(%d)  offset(%#x \n) \n", type, offs_image[type]);
			}

			if (offs_image[IMAGE_KERNEL_UPGRADE] == 0)
				offs_image[IMAGE_KERNEL_UPGRADE] = offs_image[IMAGE_KERNEL];
			spl_printf("ius: %x/%x/%x/%x \n", offs_image[IMAGE_ITEM], offs_image[IMAGE_RAMDISK], offs_image[IMAGE_KERNEL], offs_image[IMAGE_UBOOT1]);
		}
	}

	irf->free(ius);
	return ret;
}

void get_image_offset(void)
{
	char item_part_name[ITEM_MAX_LEN + 1], part_type[ITEM_MAX_LEN + 1];
	int i = 0, j, part_size = 0, offset = 0, err = 0;
	int temp_offs_image[IMAGE_NONE];

	if (boot_dev_id == DEV_IUS) {
		if (item_exist("part0")) {
			irf->memcpy(temp_offs_image, offs_image, IMAGE_NONE * sizeof(int));
			irf->memset(offs_image, 0, IMAGE_NONE * sizeof(int));
		}
		while (i < IMAGE_NONE) {
			if (item_exist(images_parts[i])) {
				irf->memset(item_part_name, 0, sizeof(item_part_name));
				err = item_string(item_part_name, images_parts[i], 0);
                if (err < 0) {
                    irf->printf("item_part_name error \n ");
                    //continue;
                } else {
                     irf->printf("item_part_name = %s \n", item_part_name);
                }

				part_size = item_integer(images_parts[i], 1);
                if (part_size < 0) {
                    irf->printf("part_size < 0 =>%#x \n", part_size);
                    //continue;
                } else {
                    irf->printf("part_size = %#x \n", part_size);
                }

				irf->memset(part_type, 0, sizeof(part_type));
				err = item_string(part_type, images_parts[i], 2);
                if (err < 0) {
                    irf->printf("part_type error \n");
                } else {
                    irf->printf("part_type %s \n", part_type);
                }

				if (!irf->strncmp(part_type, "boot", strlen((const char*)"boot"))) {
					for (j=0; j<IMAGE_NONE; j++) {
						if (!irf->strncmp(item_part_name, part_name[j], strlen(item_part_name)))
							break;
					}
					if (j >= IMAGE_NONE) {
						i++;
						continue;
					}

					offs_image[j] = temp_offs_image[i];
				}
			}
			i++;
		}
		if (offs_image[IMAGE_KERNEL_UPGRADE] == 0)
			offs_image[IMAGE_KERNEL_UPGRADE] = offs_image[IMAGE_KERNEL];
		if (offs_image[IMAGE_RAMDISK] == 0)
			offs_image[IMAGE_RAMDISK] = temp_offs_image[IMAGE_RAMDISK];
	} else {
		irf->memset(offs_image, 0, IMAGE_NONE * sizeof(int));
		if (item_exist("part0")) {
			while (i < IMAGE_NONE) {
				if (item_exist(images_parts[i])) {
                    irf->memset(part_type, 0, sizeof(part_type));
                    irf->memset(item_part_name, 0, sizeof(item_part_name));

					err = item_string(item_part_name, images_parts[i], 0);
                    if (err < 0) {
                        irf->printf(" in func %s ,line %d  item_string error \n", __func__, __LINE__);
                        continue;
                    }

					part_size = item_integer(images_parts[i], 1);
                    if (part_size < 0) {
                        irf->printf(" in func %s ,line %d item_integer error \n", __func__, __LINE__);
                        continue;
                    }

					err = item_string(part_type, images_parts[i], 2);
                    if (err < 0) {
                        irf->printf(" in func %s ,line %d  item_string error \n", __func__, __LINE__);
                        continue;
                    }

					if (!irf->strncmp(part_type, "boot", strlen((const char*)"boot"))) {
						for (j=0; j<IMAGE_NONE; j++) {
							if (!irf->strncmp(item_part_name, part_name[j], strlen(item_part_name)))
								break;
						}
						if (j >= IMAGE_NONE) {
							i++;
							continue;
						}

						offs_image[j] = offset;
						offset += (part_size << 10);
						if (j == IMAGE_ITEM)
							offset += (UPGRADE_PARA_SIZE + RESERVED_ENV_SIZE);
					}
				}
				i++;
			}
			if (offs_image[IMAGE_KERNEL_UPGRADE] == 0)
				offs_image[IMAGE_KERNEL_UPGRADE] = offs_image[IMAGE_KERNEL];
		} else {
			offs_image[IMAGE_UBOOT] = 0;
			offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET;
			offs_image[IMAGE_UBOOT1] = DEFAULT_UBOOT1_OFFSET;
		}
		if (item_exist("board.disk") && item_equal("board.disk", "flash", 0))
			boot_dev_id = DEV_FLASH;
		else if (item_exist("board.disk") && item_equal("board.disk", "emmc", 0))
			boot_dev_id = DEV_MMC(0);
	}

	spl_printf("images: %x/%x/%x/%x on %d\n", offs_image[IMAGE_ITEM], offs_image[IMAGE_RAMDISK], offs_image[IMAGE_KERNEL], offs_image[IMAGE_UBOOT1], boot_dev_id);
	return;
}

void dump_buf(char *buf, unsigned int len)
{
    unsigned int i = 0;
    for (i = 0; i < len; i++) {
        if ((i & 15) == 0) {
            irf->printf(" \n ");
        }
        irf->printf("%c", buf[i]);
    }
}

void init_config_item(void)
{
	void *cfg = (void *)irf->malloc(ITEM_SIZE_NORMAL);
	if(cfg == NULL) {
        irf->printf("init_config_item error, not has memory \n");
		return;
    }

	if (boot_dev_id == DEV_IUS) {
        irf->printf(" DEV_IUS read items config .... \n");
		irf->vs_assign_by_id(DEV_MMC(1), 1);
		irf->vs_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	} else if (boot_dev_id == DEV_FLASH) {
	    irf->printf(" DEV_FLASH read items config .... \n");
		irf->vs_assign_by_id(boot_dev_id, 1);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET;
		irf->vs_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	}else if (boot_dev_id == DEV_MMC(0)) {
	    irf->printf(" DEV_MMC read items config .... \n");
		emmc_init(boot_dev_id);
		//	irf->vs_assign_by_id(boot_dev_id, 1);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET + EMMC_IMAGE_OFFSET;
		mmc_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	}else if (boot_dev_id == DEV_SNND ){
	    irf->printf(" DEV_SNND read items config .... \n");
		irf->vs_assign_by_id(boot_dev_id, 1);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET ;
		irf->vs_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	} else if (boot_dev_id == DEV_BND || boot_dev_id == DEV_NAND) {
        irf->printf(" nandFlash read items config .... \n");
	     nand_uboot0_read(cfg, DEV_ITEM, 0, ITEM_SIZE_NORMAL);
	} else {
	    irf->printf(" nothing read items config .... \n");
	}

    //dump_buf(cfg, ITEM_SIZE_NORMAL);
	item_init(cfg, ITEM_SIZE_NORMAL);
	irf->free(cfg);
	return;
}

int apollo3_bootst(void){
	int tmp = rtcbit_get("sleeping");
	int flag = -1;

#if !defined(CONFIG_PRELOADER)
	flag = rbgetint("bootstats");
#else
	spl_printf("boot state(%d)\n", tmp);
	switch(tmp){
		case BOOT_ST_RESUME:
			flag = tmp;
			break;
		default:
			spl_printf("warning: invalid boot state\n", tmp);
	}
#endif
	return flag;
}

char spiparts[64 + 1];
char partsname[ITEM_MAX_LEN + 1];
char partstype[ITEM_MAX_LEN + 1];

void boot_main ( void )
{
	uint8_t val = 0;
	int32_t ret, bootst, i, tmp = 0;
	uint32_t flag =0;

	switch (IROM_IDENTITY) {
		case IX_CPUID_820_0:
			irf = &irf_2885;
			break;
		case IX_CPUID_820_1:
			irf = &irf_2939;
			break;
		case IX_CPUID_X15:
			irf = &irf_2974;
			break;
		case IX_CPUID_X9:
			irf = &irf_c31c431;
			break;
		case IX_CPUID_X9_NEW:
			irf = &irf_4eadb12;
			flag = 1;
			break;
		default:
			irf = &irf_2974;
			break;
	}

	writel(0xff,0x21E09C18); /* rtc ?? */
	writel(0x0, 0x21E093C0); /* close JTAG pins */
	writel(0x0, 0x21E093C4);
	writel(0x0, 0x21E093C8);
	irf->cdbg_log_toggle(1);

	clock_config();
    irf->ss_jtag_en(1);
	irf->init_timer();

	irf->module_enable ( GPIO_SYSM_ADDR );
	irf->module_enable ( PWM_SYSM_ADDR );

	init_serial();
	irf->printf("\n");
	spl_printf(" imapx15_start %#x \n", IROM_IDENTITY);

	//set mode
	writel(0xff, 0x21E090A4);  /* as gpio mode */
	writel(0x00, 0x21E09054);  /* pull disable */
	//131 red output value and dircet set
	writel(0x0,  0x20F020C4);
	writel(0x18,  0x20F020C8);
	//132 blue output value and dircet set
	writel(0x0,  0x20F02104);
	writel(0x18,  0x20F02108);

	if(IROM_IDENTITY == IX_CPUID_X15) {
		// We cofig all RGB signals input mode and disable pull up/down
		for(i=66; i<94 ; i++)
		{
			irf->pads_chmod(i,1,0);
			if(i <76)
				irf->pads_pull(i,0,0);
			else
				irf->pads_pull(i,0,1);
		}
	}

    ret = get_offset_ius(); /* get ius from mmc/sdcard */
	if (ret) {
		boot_dev_id = irf->boot_device();
    } else {
		boot_dev_id = DEV_IUS;
    }
	irf->printf("====boot_dev_id = %#x =======\n", boot_dev_id);

    nand_uboot0_init();

	rtcbit_init();

#ifdef CONFIG_BOARD_HAVE_UDC_BURN
	if ((boot_dev_id!=DEV_IUS && linkpc_necessary())) {
		linkpc_hook();
	}
#endif

	init_config_item();

#ifdef CONFIG_BOARD_HAVE_UDC_BURN
	if (item_exist("linkpc.gpio")) {
		tmp = item_integer("linkpc.gpio", 0);
		if (linkpc_manual_forced(tmp)) {
			linkpc_hook();
		}
	}
#endif

	boot_mode = get_boot_mode();
	spl_printf("dev/mode: %d/%d\n", boot_dev_id, boot_mode);
	cpu_freq_config();
	init_io();
	asm ( "dsb" );

	board_scan_arch();
	board_init_i2c();
	board_binit();

	/*jtag sd user same pins, this is switch, 1: jtag user 0: sd user*/
	if(flag == 1)
		writel(1, 0x21e093e0);

	bootst = board_bootst();
	tmp = (bootst == BOOT_ST_RESUME);
	if ((bootst == BOOT_ST_RESUME) && (boot_dev_id == DEV_IUS)){
		spl_printf("IUS card in, but resume not go mmc1\n");
	}
	spl_printf("tmp=%d \n", tmp);

#if !defined(CONFIG_IMAPX_FPGATEST)
	ret = dramc_init(irf, !!tmp);
#else
	drama9_init();
	ret = 0;
#endif
	asm ( "dsb" );
	if ( ret ) {
		spl_printf ( "dram error\n" );
		irf->cdbg_shell();
		while ( 1 );
	}

	if(tmp) {
		/*
		 * set watchdog up, if 16s later, and
		 * system is not resume, system will reboot
		 * time can be set in 1s, 2s, 4s, 8s, 16s
		 */
		if (item_integer("config.wdt", 0) != 0) {
			irf->module_enable(WDT_SYSM_ADDR);
			set_up_watchdog(16);
		}
		imapx_wdt_ctrl(DISABLE);
		__attribute__((noreturn)) void (*wake)(void) = (void *)readl(CONFIG_SYS_RESUME_BASE);
		wake();
	}

	rbinit();
	rtcbit_set_rbmem();
	rbsetint ("dramsize", dram_size_check());
	rbsetint ("bootmode", boot_mode);

	get_image_offset();

	boot();
}
