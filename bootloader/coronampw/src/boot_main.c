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
#include <asm-arm/arch-coronampw/imapx_base_reg.h>
#include <asm-arm/setup.h>
#include <asm-arm/types.h>
#include <asm-arm/arch-coronampw/imapx_uart.h>
#include <configs/imap_coronampw.h>
#include <asm-arm/arch-coronampw/imapx_spiflash.h>
#include <ssp.h>


typedef enum {
	IMAGE_UBOOT = 0,
	IMAGE_ITEM = 1,
	IMAGE_RAMDISK = 2,
	IMAGE_KERNEL = 3,
	IMAGE_KERNEL_UPGRADE = 4,
	IMAGE_SYSTEM = 5,
	IMAGE_UBOOT1 = 16,
	IMAGE_RESERVED = 17,
	IMAGE_NONE = 18,
}image_type_t;

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

typedef struct image_header {
  uint32_t ih_magic;     // Image Header Magic Number
  uint32_t ih_hcrc;      // Image Header CRC Checksum
  uint32_t ih_time;      // Image Creation Timestamp
  uint32_t ih_size;      // Image Data Size
  uint32_t ih_load;      // Data  Load Address
  uint32_t ih_ep;        // Entry Point Address
  uint32_t ih_dcrc;      // Image Data CRC Checksum
  uint8_t  ih_os;        // Operating System
  uint8_t  ih_arch;      // CPU architecture
  uint8_t  ih_type;      // Image Type
  uint8_t  ih_comp;      // Compression Type
  uint8_t  ih_name[32];  // Image Name
} image_header_t;

#undef IROM_VS_READ

#define readl(a)    (*(volatile unsigned int *)(a))
#define readb(a)    (*(volatile unsigned char *)(a))
#define writel(v,a)   (*(volatile unsigned int *)(a) = (v))
#define isb() __asm__ __volatile__ ("" : : : "memory")
#define nop() __asm__ __volatile__("mov\tr0,r0\t@ nop\n\t");

#define UPGRADE_PARA_SIZE	0x400
#define RESERVED_ENV_SIZE	0xfc00
#define EMMC_IMAGE_OFFSET	0x200000
#define DEFAULT_ITEM_OFFSET 0xC000
#define DEFAULT_UBOOT1_OFFSET 0x20000
#define KERNEL_MAGIC	0x56190527
#define KERNEL_INNER_OFFS	64
#define FLAG_ADDR 0x21e09e30
#define CONFIG_SYS_SDRAM_BASE 0x40000000
#define TAG_START (CONFIG_SYS_SDRAM_BASE+0x100)
#define RAMDISK_START (CONFIG_SYS_SDRAM_BASE+(dram_size <<20)-0x400000)
#define CMDLINE_RECOVERY "console=ttyAMA3,115200 mem=%dM mode=%s"
#define CMDLINE_SPI_FLASH "console=ttyAMA3,115200 mem=%dM rootfstype=%s root=/dev/spiblock1 rw"
#define CMDLINE_SPI_NAND_FLASH "console=ttyAMA3,115200 mem=%dM rootfstype=ext4 root=/dev/spiblock1p1 rw"
#define CMDLINE_TF_CARD "console=ttyAMA3,115200 mem=%dM rootfstype=ext4 root=/dev/mmcblk0p2 rw init=/init"
#define CMDLINE_TF_CARD_EMMC "console=ttyAMA3,115200 mem=%dM rootfstype=ext4 root=/dev/mmcblk1p2 rw init=/init"
#define CMDLINE_EMMC "console=ttyAMA3,115200 mem=%dM rootfstype=ext4 root=/dev/mmcblk0p1 rw"

#define PADS_MODE_OUT     2
#if defined(CONFIG_IMAPX_FPGATEST)
#define clk 24000000
#else
#define clk 99000000
#endif
#define baudrate 115200
#define ibr ( clk / ( 16 * baudrate ) ) & 0xffff
#define fbr ( ( ( clk - ibr * ( 16 * baudrate ) ) * 64 ) / ( 16 * baudrate ) ) & 0x3f

#if defined(CONFIG_IMAPX_FPGATEST)
struct irom_export *irf = &irf_coronampw;
#else
struct irom_export *irf = &irf_corona;
#endif
struct tag *params = (struct tag *) TAG_START;
int boot_dev_id, boot_mode = BOOT_NORMAL;
int offs_image[IMAGE_NONE];
int dram_size = 0;

extern int dramc_coronampw_fpga_init(struct irom_export *irf, int);

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

void set_pll(int addr,  int freq)
{
        int i = 1, para = 64;

        if (freq == 594) {
                i = 4;
                para = 99;
        }

        if(freq<800){           //Fvco must > 800
                writel(0x12, addr+0xc);
                freq *= 2;
        } else {
                writel(0x11, addr+0xc);
        }

        writel(i & 0xff, addr);

        writel(para & 0xff, addr + 0x4);
        writel((para >> 8) | 0x10,	addr + 0x8);

        /* prepare: disable load, close gate */
        writel(1, addr + 0x1c);

        /* disable pll */
        writel(0, addr + 0x20);

        /* out=pll, src=osc */
        writel(2, addr + 0x24);

        /* enable load */
        writel(3, addr + 0x1c);

        /* enable pll */
        writel(1, addr + 0x20);
        /* wait pll stable:
     ** according to RTL simulation, PLLs need 200us before stable
     ** the poll register cycle is about 0.75us, so 266 cycles
     ** is needed.
     ** we put 6666 cycle here to perform a 5ms timeout.
     */
    for( i = 0; (i < 6666) &&
            !(readb(addr + 0x28) & 1);
            i++ );

    /* open gate */
    writel(2, addr + 0x1c);

}

//clock tree
void clock_set ( void )
{
    set_pll(0x2d00a030, 1536);
    //set_pll(0x2d00a060, 594);
	//irf->set_pll ( PLL_D, 1536 );                       // set DPLL to 1536M
	irf->set_pll ( PLL_E, 1188 );                       // set EPLL to 1188M  --->594M
#if 1
	irf->module_set_clock ( BUS6_CLK_BASE, /*PLL_E*/PLL_D, /*5*/8);
	writel(0, BUS6_CLK_BASE + 0x4);
	irf->module_set_clock ( APB_CLK_BASE_NEW, PLL_D, 17 ); // set spi_clk from 27M to 43M, set pbus_clk to 118M, special --->99M
	writel(8, BUS6_CLK_BASE + 0x4);
#endif
	irf->module_set_clock ( SSP_CLK_BASE, PLL_E, 3);
	irf->module_set_clock ( UART_CLK_BASE,    PLL_E, 5 ); // need change uart clk, set serial clk to 118M  --->99M
	irf->module_set_clock ( SD_CLK_BASE ( 0 ), PLL_E, 14 );
	irf->module_set_clock ( SD_CLK_BASE ( 1 ), PLL_E, 14 );
	irf->module_set_clock ( SD_CLK_BASE ( 2 ), PLL_E, 14 );
}

struct FREQ_ST {
	uint32_t freq;
	uint32_t val;
} fr_para[] = {
	{1992, 0x0052}, {1944, 0x0050}, {1896, 0x004e}, {1848, 0x004c}, {1800, 0x004a}, {1752, 0x0048}, {1704, 0x0046}, {1656, 0x0044},
	{1608, 0x0042}, {1560, 0x0040}, {1512, 0x003e}, {1464, 0x003c}, {1416, 0x003a}, {1368, 0x0038}, {1320, 0x0036}, {1272, 0x0034},
	{1224, 0x0032}, {1176, 0x0030}, {1128, 0x002e}, {1080, 0x002c}, {1032, 0x002a}, { 996, 0x1052}, { 948, 0x104e}, { 900, 0x104a},
	{ 852, 0x1046}, { 804, 0x1042}, { 756, 0x103e}, { 696, 0x1039}, { 660, 0x1036}, { 612, 0x1032}, { 564, 0x102e}, { 516, 0x102a},
	{ 468, 0x204d}, { 420, 0x2045}, { 372, 0x203d}, { 324, 0x2035}, { 276, 0x202d}, { 228, 0x304b}, { 180, 0x303b}, { 132, 0x302b}
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
	irf->printf("cpu freq = %d\n", fr_para[i].freq);
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

//flag
int get_boot_mode(void)
{
	int flag, item_size = 0;
	char *upgrade_flag;

	if (boot_dev_id == DEV_IUS) {
		if (offs_image[IMAGE_RAMDISK] > 0)
			return BOOT_RECOVERY_IUS;
		else
			return BOOT_NORMAL;
	}
	upgrade_flag = (char *)irf->malloc(UPGRADE_PARA_SIZE);
	if(upgrade_flag == NULL)
		return -1;
	item_size = item_integer("part1", 1);
	irf->vs_read(upgrade_flag, offs_image[IMAGE_ITEM] + (item_size << 10), UPGRADE_PARA_SIZE, 0);
	if (!irf->strncmp(upgrade_flag, "upgrade_flag=ota", strlen("upgrade_flag=ota"))) {
		spl_printf("upgrade mode ota\n");
		irf->free(upgrade_flag);
		return BOOT_RECOVERY_OTA;
	} else {
		spl_printf("boot normal \n");
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
		irf->vs_read(data, EMMC_IMAGE_OFFSET + offs_image[image_num] + offset, size, 0);
	} else if (dev_id == DEV_FLASH) {
#ifndef IROM_VS_READ
		flash_nand_vs_read(data,offs_image[image_num] + offset,size, 0); // multiple wires read
		spl_printf("dev read image irom_vs_read not define\n");
		//flash_vs_read(data,offs_image[image_num] + offset,size,0); // one wire polling read
		//flash_read(data,offs_image[image_num] + offset,size,0); // one wire DMA read
#else
		irf->vs_read(data, offs_image[image_num] + offset, size, 0);
#endif
	} else if (dev_id == DEV_SNND) {
		flash_nand_vs_read(data,offs_image[image_num] + offset,size,0);
	} else {
		irf->vs_read(data, offs_image[image_num] + offset, size, 0);
	}
	return;
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

	dram_size = calc_dram_M();
	rbsetint ("dramsize" , dram_size);
retry:
	if ((boot_mode == BOOT_RECOVERY_IUS) || (boot_mode == BOOT_RECOVERY_OTA) || (boot_mode == BOOT_RECOVERY_DEV))
		image_type = IMAGE_KERNEL_UPGRADE;
	else if (offs_image[IMAGE_UBOOT1] != 0)
		image_type = IMAGE_UBOOT1;
	else
		image_type = IMAGE_KERNEL;

	if (offs_image[IMAGE_UBOOT1] != 0)
		image_type = IMAGE_UBOOT1;
#ifndef IROM_VS_READ
	if (boot_dev_id == DEV_FLASH){
		flash_nand_vs_reset(); // multiple wires reset
		spl_printf("boot nand reset irom_vs_read not define\n");
	}
		//flash_vs_reset(); // one wires polling reset
		//flash_reset(); // one wire DMA reset
#endif
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

		if (boot_mode == BOOT_RECOVERY_IUS)
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, dram_size, "ius");
		else if (boot_mode == BOOT_RECOVERY_OTA)
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, dram_size, "ota");
		else if (boot_mode == BOOT_RECOVERY_DEV)
			irf->sprintf(buf_tmp, CMDLINE_RECOVERY, dram_size, "dev");
		else {
			if (boot_dev_id == DEV_FLASH) {
				irf->sprintf(spi_parts, "part%d", IMAGE_SYSTEM);
				if (item_exist(spi_parts)) {
					if (!item_string(fs_type, spi_parts, 3))
						irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, dram_size, fs_type);
					else
						irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, dram_size, "squashfs");
				} else {
					irf->sprintf(buf_tmp, CMDLINE_SPI_FLASH, dram_size, "squashfs");
				}
			} else if (boot_dev_id == DEV_MMC(0)) {
				irf->sprintf(buf_tmp, CMDLINE_EMMC, dram_size);
			} else {
				if (irf->vs_assign_by_id(DEV_MMC(0), 1) == 0)
					irf->sprintf(buf_tmp, CMDLINE_TF_CARD_EMMC, dram_size);
				else
					irf->sprintf(buf_tmp, CMDLINE_TF_CARD, dram_size);
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
	struct ius_desc *desc;
	uint32_t size;

	struct ius_hdr *ius = irf->malloc(sizeof(struct ius_hdr));
	if ( !ius )
		return ret;

	irf->memset(ius, 0, sizeof(struct ius_hdr));
	irf->memset(offs_image, 0, IMAGE_NONE * sizeof(int));
	irf->vs_assign_by_id(DEV_MMC(1), 1);
	irf->vs_read ((uint8_t *)ius, IUS_DEFAULT_LOCATION, sizeof(struct ius_hdr), 0);
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
		spl_printf("ius: %x/%x/%x/%x \n", offs_image[IMAGE_ITEM], offs_image[IMAGE_RAMDISK], offs_image[IMAGE_KERNEL], offs_image[IMAGE_UBOOT1]);
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
				irf->sprintf(spi_parts, "part%d", i);
				if (item_exist(spi_parts)) {
					item_string(item_part_name, spi_parts, 0);
					part_size = item_integer(spi_parts, 1);
					item_string(part_type, spi_parts, 2);
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

void init_config_item(void)
{
	void *cfg = (void *)irf->malloc(ITEM_SIZE_NORMAL);
	if(cfg == NULL)
		return;

	if (boot_dev_id == DEV_IUS) {
		irf->vs_assign_by_id(DEV_MMC(1), 1);
	} else if (boot_dev_id == DEV_FLASH) {
		irf->vs_assign_by_id(boot_dev_id, 1);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET;
	}else if (boot_dev_id == DEV_MMC(0)) {
		irf->vs_assign_by_id(boot_dev_id, 1);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET + EMMC_IMAGE_OFFSET;
	}else if (boot_dev_id == DEV_SNND ){
		irf->vs_assign_by_id(boot_dev_id, 1);
		offs_image[IMAGE_ITEM] = DEFAULT_ITEM_OFFSET ;
	}
	irf->vs_read(cfg, offs_image[IMAGE_ITEM], ITEM_SIZE_NORMAL, 0);
	item_init(cfg, ITEM_SIZE_NORMAL);
	irf->free(cfg);
	return;
}

extern int dramc_set_para(void);
extern int dramc_set_addr(void);
extern void dramc_set_frequency(void);
extern void dramc_set_timing(void);
extern int dramc_init_memory(void);

int init_dram(void)
{
	int reset_flag = 0;

	irf->printf("coronampw Bare machine code DDR init\n");

	if(dramc_set_para())
		return 1;
	if(dramc_set_addr())
		return 1;
	dramc_set_frequency();
	dramc_set_timing();
	
	//reset_flag = (readl(SYS_RTC_RESETSTS) & RESET_WAKEUP_FLAG);
	if(dramc_init_memory()) {
		irf->printf("memory init error!\n");
	}

	/*  will overwrite soc start addr's instruction
	if(dramc_verify_size()) {
		dramc_debug("memory total size verify error!\n");
		return 1;
	}*/

	/*
	if(reset_flag == RESET_WAKEUP_FLAG){
		writel(3, SYS_RTC_RSTCLR);
		__attribute__((noreturn)) void (*wake)(void) = (void *)readl(0x42980000);
		wake();
	}
	*/

	return 0;
}

void boot_main ( void )
{
	int ret = 0;
	int tmp = 0;

	spl_printf ( " coronampw 1--\n" );
#ifdef CONFIG_IMAPX_FPGATEST 
	writel(0, 0x2d009000);
#else
	tmp = readl(0x2d00902c);
	tmp &= ~(0x3 << 3);
	writel(tmp, 0x2d00902c);

	writel(0xc, 0x2b009000);
#endif

	irf->cdbg_log_toggle ( 1 );
	clock_set();
	irf->init_timer();
	irf->module_enable ( GPIO_SYSM_ADDR );
	irf->module_enable ( PWM_SYSM_ADDR );
	//irf->module_enable ( UART_SYSM_ADDR );
	init_serial();


	if (get_offset_ius())
		boot_dev_id = irf->boot_device();
	else
		boot_dev_id = DEV_IUS;
	rtcbit_init();
	
	init_config_item();
	boot_mode = get_boot_mode();
	led_color(boot_mode);
	spl_printf("dev/mode: %d/%d\n", boot_dev_id, boot_mode);
#ifdef CONFIG_IMAPX_FPGATEST 
#else
	tmp = readl(0x2d009000);
	tmp |= 0x1 << 7;
	writel(tmp, 0x2d009000);
#endif
	
	//cpu_freq_config();
	init_io();
	asm ( "dsb" );

//	board_init_i2c();
	board_binit();
#ifdef CONFIG_IMAPX_FPGATEST 
	//ret = dramc_fpga_init(irf, 0 );
	init_dram();
#else
	//dramc_init(irf, 0);
	init_dram();
#endif
	asm ( "dsb" );

	if ( ret ) {
		spl_printf ( "dram error\n" );
		irf->cdbg_shell();
		while ( 1 );
	}

	rbinit();
	rtcbit_set_rbmem();
	rbsetint ("dramsize", dram_size_check());
	rbsetint ("bootmode", boot_mode);
	get_image_offset();
	spl_printf ( " coronampw go boot--\n" );

	boot();
}
