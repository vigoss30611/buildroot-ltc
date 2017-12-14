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
#include <linux/types.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>
#include <asm-arm/setup.h>
#include <asm-arm/types.h>
#include <asm-arm/arch-apollo3/imapx_uart.h>
#include <configs/imap_apollo.h>
#include <asm-arm/arch-apollo3/imapx_spiflash.h>
#include <ssp.h>
#include <image.h>
#ifdef CONFIG_BOARD_HAVE_UDC_BURN
#include <linkpc.h>
#endif

static const char *part_name[] = {
	"uboot",
    "item",
    "ramdisk",
    "kernel",
    "kernel1",
    "system",
    "ramdisk1",
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

#undef IROM_VS_READ

#define readl(a)    (*(volatile unsigned int *)(a))
#define writel(v,a)   (*(volatile unsigned int *)(a) = (v))
#define isb() __asm__ __volatile__ ("" : : : "memory")
#define nop() __asm__ __volatile__("mov\tr0,r0\t@ nop\n\t");

#define PADS_MODE_OUT     2
#if defined(CONFIG_IMAPX_FPGATEST)
#define clk 40000000
#else
#define clk 99000000
#endif
#define baudrate 115200
#define ibr ( clk / ( 16 * baudrate ) ) & 0xffff
#define fbr ( ( ( clk - ibr * ( 16 * baudrate ) ) * 64 ) / ( 16 * baudrate ) ) & 0x3f

struct irom_export *irf = &irf_apollo3;
static char *upgrade = NULL;
struct tag *params = (struct tag *) TAG_START;
int boot_dev_id, boot_mode = BOOT_NORMAL;
int offs_image[IMAGE_NONE];
int dram_size = 0;
int loop_per_jiffies = 0;

extern int dramc_apollo3_fpga_init(struct irom_export *irf, int);

#define SDMMC_CDETECT   0x50
int is_mmc_present (void) {
	return (readl(SD1_BASE_ADDR + SDMMC_CDETECT) & 0x1) == 0 ? 1 : 0;
}

static void emmc_init(int reset) 
{
	#ifdef CONFIG_BOARD_ENABLE_MMC_DRIVER
	mmc_init_param(0);
	vs_mmc_assign_by_devnum(0, 0);
	#else
	irf->vs_assign_by_id(DEV_MMC(0), reset);
	#endif
	return;
}

static void mmc1_init(int reset) 
{
	#ifdef CONFIG_BOARD_ENABLE_MMC_DRIVER
	vs_mmc_assign_by_devnum(1, reset);
	#else
	irf->vs_assign_by_id(DEV_MMC(1), reset);
	#endif
	return;
}

static int  mmc_read(uint8_t *buf, int start,int length, int extra)
{
	#ifdef CONFIG_BOARD_ENABLE_MMC_DRIVER
	return support_vs_read(buf, start, length, extra);
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

//clock tree
void clock_set ( void )
{
	irf->set_pll ( PLL_D, 0x003f );                       // set DPLL to 1536M
	irf->set_pll ( PLL_E, 0x2062 );                       // set EPLL to 1188M  --->594M
#if 1
	irf->module_set_clock ( BUS6_CLK_BASE, /*PLL_E*/PLL_D, /*5*/8);
	writel(0, BUS6_CLK_BASE + 0x4);
	irf->module_set_clock ( APB_CLK_BASE_NEW, PLL_D, 17 ); // set spi_clk from 27M to 43M, set pbus_clk to 118M, special --->99M
	writel(8, BUS6_CLK_BASE + 0x4);
#endif
	irf->module_set_clock ( SSP_CLK_BASE, PLL_E, 3);
	irf->module_set_clock ( UART_CLK_BASE,    PLL_E, 5 ); // need change uart clk, set serial clk to 118M  --->99M
	irf->module_set_clock ( SD_CLK_BASE ( 0 ), PLL_E, 11 );
	irf->module_set_clock ( SD_CLK_BASE ( 1 ), PLL_E, 14 );
	irf->module_set_clock ( SD_CLK_BASE ( 2 ), PLL_E, 14 );
}

struct FREQ_ST {
	uint32_t freq;
	uint32_t val;
	int lpj;
} fr_para[] = {
	{1992, 0x0052, 0}, {1944, 0x0050, 0}, {1896, 0x004e, 0}, {1848, 0x004c, 0}, {1800, 0x004a, 0}, {1752, 0x0048, 0}, {1704, 0x0046, 0}, {1656, 0x0044, 0},
	{1608, 0x0042, 0}, {1560, 0x0040, 0}, {1512, 0x003e, 0}, {1464, 0x003c, 0}, {1416, 0x003a, 0}, {1368, 0x0038, 0}, {1320, 0x0036, 0}, {1272, 0x0034, 0},
	{1224, 0x0032, 0}, {1176, 0x0030, 0}, {1128, 0x002e, 0}, {1080, 0x002c, 0}, {1032, 0x002a, 0}, { 996, 0x1052, 0}, { 948, 0x104e, 0}, { 900, 0x104a, 0},
	{ 852, 0x1046, 0}, { 804, 0x1042, 0}, { 756, 0x103e, 0}, { 696, 0x1039, 0}, { 660, 0x1036, 0}, { 612, 0x1032, 672768}, { 564, 0x102e, 621568}, { 516, 0x102a, 567808},
	{ 468, 0x204d, 516608}, { 420, 0x2045, 462848}, { 372, 0x203d, 409088}, { 324, 0x2035, 356352}, { 276, 0x202d, 303104}, { 228, 0x304b, 0}, { 180, 0x303b, 0}, { 132, 0x302b, 0}
};

void reset_APLL(uint32_t idx)
{
	writel(0x00, CLK_SYSM_ADDR + 0x0c);
	writel(fr_para[idx].val & 0xff, CLK_SYSM_ADDR + 0x00);
	writel((fr_para[idx].val >> 8) & 0xff, CLK_SYSM_ADDR + 0x04);
	writel(0x01, SYS_SYSM_ADDR + 0x18);
	while(readl ( SYS_SYSM_ADDR + 0x18) & 0x1);
}


void cpu_freq_config(void)
{
	int i, A_freq;

	if ( item_exist ( "soc.cpu.freq" ) )
		A_freq = item_integer ( "soc.cpu.freq", 0 ) + 1032;
	else
		A_freq = 696;

	i = sizeof ( fr_para ) / sizeof ( struct FREQ_ST );
	while ( fr_para[--i].freq <= A_freq );
	//spl_printf("cpu freq = %d\n", fr_para[i+1].freq);
	loop_per_jiffies = fr_para[i+1].lpj;
	reset_APLL ( i + 1 );
}

//peripheral
void init_serial(void)
{
	int32_t val;

	irf->module_enable ( UART_SYSM_ADDR );
	val = readl ( UART_LCRH );
	val &= ~0xff;
	val |= 0x60;
	writel ( val, UART_LCRH );
	writel ( ibr, UART_IBRD );
	writel ( fbr, UART_FBRD );
	val = readl ( UART_CR );
	val &= ~0xff87;
	val |= 0x301;
	writel ( val, UART_CR );
	val = readl ( UART_LCRH );
	val |= 0x10;
	writel ( val, UART_LCRH );
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
	if (item_exist(iob[i])) {
		item_string(buf, iob[i], 0);
		if (irf->strncmp ( buf, "pads", 4) != 0)
			continue;
		index = item_integer(iob[i], 1);
		high = item_integer(iob[i], 2);
		delay = item_integer(iob[i], 3);
		irf->pads_chmod(index, PADS_MODE_OUT, !!high);
		irf->udelay(delay);
	} else
		break;
	}
	writel(readl(RTC_GPPULL) | ( 1 << 3 ), RTC_GPPULL);
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

static int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

int system_check_upgrade(char *str)
{                                  
	int i, j; 
	char c, buf[256];
	char *p = upgrade + 4;	// Because lack of mem in uboot,  we only operate RESERVED_ENV_UBOOT_SIZE envdata, skip crc
	
	i = 0;          
	buf[255] = '\0';
	
	//spl_printf("%s\n", str);
	while (p[i] != '\0') {
		j = 0;                                       
		do {                                         
			if (i >= RESERVED_ENV_UBOOT_SIZE) // do not find "str"
				return -1;

			buf[j++] = c = p[i++];
			if (j == sizeof(buf) - 1) {              
				spl_printf("%s\n", __func__);         
				j = 0;                               
			}                                        
		} while (c != '\0');                         

		if (j > 1) {                                        
			if (!irf->strncmp(buf, str, max(strlen(str), strlen(buf)))) {
				//spl_printf("get %s\n", buf);
				return 0;
			}
		}
	}

	return -1;
} 

//flag
int get_boot_mode(void)
{
	int flag, item_size = 0;

	if (boot_dev_id == DEV_IUS) {
		if (offs_image[IMAGE_RAMDISK] > 0)
			return BOOT_RECOVERY_IUS;
		else
			return BOOT_NORMAL;
	}
	upgrade = (char *)irf->malloc(RESERVED_ENV_UBOOT_SIZE);
	if(upgrade == NULL)
		return -1;

	item_size = item_integer("part1", 1);
	if(boot_dev_id == DEV_MMC(0))
		mmc_read(upgrade, offs_image[IMAGE_ITEM] + (item_size << 10) + UPGRADE_PARA_SIZE, RESERVED_ENV_UBOOT_SIZE, 0);
	else
		irf->vs_read(upgrade, offs_image[IMAGE_ITEM] + (item_size << 10) + UPGRADE_PARA_SIZE, RESERVED_ENV_UBOOT_SIZE, 0);
	
	if (!system_check_upgrade("upgrade_flag=ota"))
		return BOOT_RECOVERY_OTA;
	else
		return BOOT_NORMAL;
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
		mmc_read(data, offs_image[image_num] + offset, size, 0);
	} else if (dev_id == DEV_MMC(0)) {
		mmc_read(data, EMMC_IMAGE_OFFSET + offs_image[image_num] + offset, size, 0);	
	} else if (dev_id == DEV_FLASH) {
		flash_read(data, offs_image[image_num] + offset, size, 0); // multiple wires read
	} else if (dev_id == DEV_SNND) {
		flash_read(data, offs_image[image_num] + offset, size, 0); // multiple wires read
	} else {
		irf->vs_read(data, offs_image[image_num] + offset, size, 0);
	}
	return;
}

int check_image(void *buf, image_type_t image_num)
{
	image_header_t *p = NULL;
	char str[64];
	int ret;
	
	switch (image_num) {
		case IMAGE_KERNEL:
		case IMAGE_KERNEL_UPGRADE:
			if (boot_dev_id != DEV_IUS) {
				irf->sprintf(str, "%s=%s", part_name[image_num], "ready");
				ret = system_check_upgrade(str);		
				if (ret < 0)
					return -1;
			}

			p = (image_header_t *)buf;
			if(!p || p->ih_magic != KERNEL_MAGIC) {
				spl_printf("wrong kernel header:magic 0x%x\n", p->ih_magic);
				return -1;
			}
			break;
		case IMAGE_RAMDISK:
		case IMAGE_RAMDISK_UPGRADE:
			if (boot_dev_id != DEV_IUS) {
				irf->sprintf(str, "%s=%s", part_name[image_num], "ready");
				ret = system_check_upgrade(str);		
				if (ret < 0)
					return -1;
			}
			break;
		default:
			spl_printf("Now do not support check %s(%d) image\n", part_name[image_num], image_num);
			return -1;
	}
	return 0;
}

void boot(void)
{
	__attribute__((noreturn)) void (*jump)(void);
	void (*theKernel)(int zero, int arch, uint params);
	ulong rd_start, rd_end;
	char buf_tmp[sizeof(image_header_t)];
	void *isi = (void *)(DRAM_BASE_PA + 0x8000);
	uint8_t *item_rrtb;
	unsigned int size;
	int image_type, _ms;
	char spi_parts[64], fs_type[16];
	int ret;

	dram_size = calc_dram_M();
	rbsetint ( "dramsize" , dram_size);
retry:
	if (offs_image[IMAGE_UBOOT1] != 0)
		image_type = IMAGE_UBOOT1;
	else
		image_type = IMAGE_KERNEL;

	if (offs_image[IMAGE_UBOOT1] != 0)
		image_type = IMAGE_UBOOT1;
	if (boot_dev_id == DEV_FLASH)
		flash_init(boot_dev_id); // multiple wires reset
	if (image_type == IMAGE_UBOOT1) {
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

		spl_printf("jump\n");
		asm("dsb");

		(*jump)();

__boot_failed__:
		irf->cdbg_shell();
		for(;;);

	} else {
retry_uimage:
		dev_read_image(boot_dev_id, image_type, 0, sizeof(image_header_t), (uint8_t *)buf_tmp);
		image_header_t *p = (image_header_t *)buf_tmp;

		ret = check_image(buf_tmp, image_type);
		if (ret < 0) {
			//spl_printf ("image_type %d \n", image_type);
			if (image_type != IMAGE_KERNEL_UPGRADE) {
				image_type = IMAGE_KERNEL_UPGRADE;
				goto retry_uimage;
			} else {
#if 0
				spl_printf ("Wrong Kernel\n");
				irf->cdbg_shell();
				while ( 1 );
#else
				/* for compatible with old ota version */
				/* try to start kernel */
				spl_printf("Try to ");
				image_type = IMAGE_KERNEL;
				dev_read_image(boot_dev_id, image_type, 0, sizeof(image_header_t), (uint8_t *)buf_tmp);
				image_header_t *p = (image_header_t *)buf_tmp;
				if(!p || p->ih_magic != KERNEL_MAGIC) {                       
					spl_printf("wrong kernel header:magic 0x%x\n", p->ih_magic);
					irf->cdbg_shell();
					while ( 1 );      
				}                                                             
#endif
			}
		}
		spl_printf("start %s\n", part_name[image_type]);
		int_endian_change(&theKernel, p->ih_ep);
		int_endian_change(&size, p->ih_size);

		_ms = get_timer(0);
		dev_read_image(boot_dev_id, image_type, 0, sizeof(image_header_t) + size, (uint8_t *)(theKernel));
		spl_printf("kernel(%d ms, %d K)\n", get_timer(_ms), size >> 10);
	  	theKernel += KERNEL_INNER_OFFS;

			if ((boot_mode == BOOT_RECOVERY_IUS) || (boot_mode == BOOT_RECOVERY_OTA) || (boot_mode == BOOT_RECOVERY_DEV)) {
				image_type = IMAGE_RAMDISK;
retry_ramdisk:
				ret = check_image(NULL, image_type);
				if (ret < 0) {
					//spl_printf ("image_type %d \n", image_type);
					if (image_type != IMAGE_RAMDISK_UPGRADE) {
						image_type = IMAGE_RAMDISK_UPGRADE;
						goto retry_ramdisk;
					} else {
#if 0
						spl_printf ("Wrong Ramdisk\n");
						irf->cdbg_shell();
						while ( 1 );
#else
						spl_printf ("Try to ");
						image_type = IMAGE_RAMDISK;
#endif
					}
				}
				spl_printf("start %s\n", part_name[image_type]);
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

		if (boot_mode == BOOT_RECOVERY_IUS)
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, loop_per_jiffies, dram_size, "ius");
		else if (boot_mode == BOOT_RECOVERY_OTA)
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, loop_per_jiffies, dram_size, "ota");
		else if (boot_mode == BOOT_RECOVERY_DEV)
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, loop_per_jiffies, dram_size, "dev");
		else {
			if (boot_dev_id == DEV_FLASH) {
				irf->sprintf(spi_parts, "part%d", IMAGE_SYSTEM);
				if (item_exist(spi_parts)) {
					if (!item_string(fs_type, spi_parts, 3))
						irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, loop_per_jiffies, dram_size, fs_type);
					else
						irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, loop_per_jiffies, dram_size, "squashfs");
				} else {
					irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, loop_per_jiffies, dram_size, "squashfs");
				}
			} else if (boot_dev_id == DEV_MMC(0)) {
				irf->sprintf(buf_tmp, CMDLINE_EMMC, loop_per_jiffies, dram_size);
			} else {
				if (irf->vs_assign_by_id(DEV_MMC(0), 1) == 0)
					irf->sprintf(buf_tmp, CMDLINE_TF_CARD_EMMC, loop_per_jiffies, dram_size);
				else
					irf->sprintf(buf_tmp, CMDLINE_TF_CARD, loop_per_jiffies, dram_size);
			}
		}

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

	if(sd_present) {
		mmc1_init(0);
		mmc_read((uint8_t *)ius, IUS_DEFAULT_LOCATION, sizeof(struct ius_hdr), 0);
		if (ius->magic == IUS_MAGIC) {
			count = ius_get_count(ius);
			ret = 0;
			for (i = 0; i < count; i++) {
				desc = ius_get_desc (ius, i);
				type = ius_desc_offo(desc);
				size = ius_desc_length(desc);

				if ((size > 0) && (type < IMAGE_NONE))
					offs_image[type] = ius_desc_offi(desc);
			}
			if (offs_image[IMAGE_KERNEL_UPGRADE] == 0)
				offs_image[IMAGE_KERNEL_UPGRADE] = offs_image[IMAGE_KERNEL];
			spl_printf("ius: %x/%x/%x/%x/%x \n", offs_image[IMAGE_ITEM], offs_image[IMAGE_RAMDISK], offs_image[IMAGE_KERNEL], offs_image[IMAGE_UBOOT1], offs_image[IMAGE_KERNEL_UPGRADE]);
		}
	}

	irf->free(ius);
	return ret;
}

void get_image_offset()
{
	char spi_parts[64], item_part_name[16], part_type[16];
	int i = 0, j, part_size = 0, offset = 0;
	int temp_offs_image[IMAGE_NONE];

	if (boot_dev_id == DEV_IUS) {
		if (item_exist("part0")) {
			irf->memcpy(temp_offs_image, offs_image, IMAGE_NONE * sizeof(int));
			irf->memset(offs_image, 0, IMAGE_NONE * sizeof(int));
		}
		while (i < IMAGE_NONE) {
			irf->sprintf(spi_parts, "part%d", i);
			if (item_exist(spi_parts)) {
				item_string(item_part_name, spi_parts, 0);
				part_size = item_integer(spi_parts, 1);
				item_string(part_type, spi_parts, 2);
				if (!irf->strncmp(part_type, "boot", strlen((const char*)"boot"))) {
					for (j=0; j<IMAGE_NONE; j++) {
						if (!irf->strncmp(item_part_name, part_name[j],
									max(strlen(item_part_name), strlen(part_name[j]))))
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
				irf->sprintf(spi_parts, "part%d", i);
				if (item_exist(spi_parts)) {
					item_string(item_part_name, spi_parts, 0);
					part_size = item_integer(spi_parts, 1);
					item_string(part_type, spi_parts, 2);
					if (!irf->strncmp(part_type, "boot", strlen((const char*)"boot"))) {
						for (j=0; j<IMAGE_NONE; j++) {
							if (!irf->strncmp(item_part_name, part_name[j],
										max(strlen(item_part_name), part_name[j])))
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
	spl_printf("images: %x/%x/%x/%x/%x on %d\n", offs_image[IMAGE_ITEM], offs_image[IMAGE_RAMDISK], offs_image[IMAGE_KERNEL], offs_image[IMAGE_UBOOT1], boot_dev_id);
	return;
}

void init_config_item(void)
{
	void *cfg = (void *)irf->malloc(ITEM_SIZE_NORMAL);
	if(cfg == NULL)
		return;

	if (boot_dev_id == DEV_IUS) {
		mmc1_init(0);
		mmc_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	} else if (boot_dev_id == DEV_FLASH) {
		irf->vs_assign_by_id(boot_dev_id, 1);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET;
		irf->vs_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	}else if (boot_dev_id == DEV_MMC(0)) {
		emmc_init(0);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET + EMMC_IMAGE_OFFSET;
		mmc_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	}else if (boot_dev_id == DEV_SNND ){
		irf->vs_assign_by_id(boot_dev_id, 1);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET ;
		irf->vs_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	}
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

void boot_main ( void )
{
	int ret, bootst;
	int tmp = 0;
	uint8_t val = 0;
	
	irf->cdbg_log_toggle ( 1 );
	clock_set();
	irf->init_timer();
	irf->module_enable ( GPIO_SYSM_ADDR );
	irf->module_enable ( PWM_SYSM_ADDR );
	init_serial();

	#ifdef CONFIG_BOARD_ENABLE_MMC_DRIVER
	init_mmc();
	/*init mmc host for card detection*/
	vs_mmc_init_controller(1);
	/*int mmc card*/
	if(is_mmc_present()){
		mmc1_init(1);
	}
	#endif

	if (get_offset_ius())
		boot_dev_id = irf->boot_device();
	else
		boot_dev_id = DEV_IUS;

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
	led_color(boot_mode);
	spl_printf("dev/mode: %d/%d\n", boot_dev_id, boot_mode);
	/*set rtc gp0 ouput model*/
	val = readl(RTC_GPDIR);
	val &= ~(1 << 0);
	writel(val, RTC_GPDIR);
	
	/*set gp0 ouput high*/
	writel(0x1, 0x2d009c7c);

	tmp = readl(0x2d009000);
	tmp |= 0x1 << 7;
	tmp = writel(tmp, 0x2d009000);

	cpu_freq_config();
	init_io();
	asm ( "dsb" );

#ifdef CONFIG_IIC
	board_init_i2c();
#endif

	board_binit();
	bootst = apollo3_bootst();
	tmp = (bootst == BOOT_ST_RESUME);
	if ((bootst == BOOT_ST_RESUME) && (boot_dev_id == DEV_IUS)){
		spl_printf("IUS card in, but resume not go mmc1\n");
	}
#ifdef CONFIG_IMAPX_FPGATEST 
	ret = dramc_q3f_fpga_init(irf, 0 );
#else
	dramc_init(irf, !!tmp);
#endif
	asm ( "dsb" );

	if ( ret ) {
		spl_printf ( "dram error\n" );
		irf->cdbg_shell();
		while ( 1 );
	}

	if(tmp){
		__attribute__((noreturn)) void (*wake)(void) = (void *)readl(CONFIG_SYS_RESUME_BASE);
		wake = wake + 1;
		asm volatile(
				"bx %0"
				:
				:"r"(wake)
				:);
	}

	rbinit();
	rtcbit_set_rbmem();
	rbsetint ("dramsize", dram_size_check());
	rbsetint ("bootmode", boot_mode);
	get_image_offset();

	boot();
}
