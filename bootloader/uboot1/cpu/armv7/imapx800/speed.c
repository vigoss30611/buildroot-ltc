
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <version.h>
#include <asm/io.h>
#include <vstorage.h>
#include <bootlist.h>
#include <isi.h>
#include <hash.h>
#include <cdbg.h>
#include <efuse.h>

int dram_enabled = 1; 

#define UART_CLK    40000000 
#define SD_CLK      40000000
#define USB_CLK     48000000
#define APB_CLK     10000000
#define AXI_CLK     20000000

#if defined(CONFIG_IMAPX_FPGATEST)
int module_get_clock(uint32_t base)
{
	switch(base)
	{
		case UART_CLK_BASE:
			return UART_CLK;
		case SD0_CLK_BASE:
		case SD1_CLK_BASE:
		case SD2_CLK_BASE:
			return SD_CLK;
		case USBREF_CLK_BASE:
			return USB_CLK;
		case APB_CLK_BASE:
		case APB_CLK_BASE_NEW:
		case CPUP_CLK_BASE:
			return APB_CLK;
		case AXI_CLK_BASE:
			return AXI_CLK;
		default:
			return 0;
	}
}
#else
int module_get_clock(uint32_t base)
{
	int clk = 0;

	switch(base)
	{
		case BUS1_CLK_BASE:
		case BUS2_CLK_BASE:
		case BUS3_CLK_BASE:
		case BUS4_CLK_BASE:
		case BUS5_CLK_BASE:
		case BUS6_CLK_BASE:
		case BUS7_CLK_BASE:
		case CRYPTO_CLK_BASE:
			clk = 201000000;
			break;
		case SD0_CLK_BASE:
		case SD1_CLK_BASE:
		case SD2_CLK_BASE:
			clk = 40000000;
			break;
		case USBREF_CLK_BASE:
			clk = 24000000;
			break;
		case UART_CLK_BASE:
			clk = 45692307;
			//clk = 46153846;
			break;
		case NFECC_CLK_BASE:
			clk = 400000000;
			break;
		case SATAPHY_CLK_BASE:
			clk = 100000000;
			break;
		case CLKOUT0_CLK_BASE:
			clk = 50000000;
			break;
		case APB_CLK_BASE:
			clk = 134000000;
			break;
		case APB_CLK_BASE_NEW:
			clk = 134000000;
			break;
		case CPUP_CLK_BASE:
			clk = 87000000;// cpup freq / cpu freq = 1:8 = 87000000/696000000
			//clk = 100500000;
			break;
		case AXI_CLK_BASE:
			clk = 268000000;
			break;
		default:
			;
	}

	return clk;
}
#endif


int print_cpuinfo(void)
{
	return 0;
}

void set_mux(uint32_t mux) {
    	writel(mux, PAD_SYSM_ADDR);
}

#if 0
int spi_pads_init(void)
{
	uint32_t tmp;

	/* hold */
	tmp = readl(PADS_SOFT(8)) | (1 << 6);
	writel(tmp, PADS_SOFT(8));
	tmp = readl(PADS_OUT_EN(8)) & ~(1 << 6);
	writel(tmp, PADS_OUT_EN(8));
	tmp = readl(PADS_OUT_GROUP(8)) | (1 << 6);
	writel(tmp, PADS_OUT_GROUP(8));

	/* wp */
	tmp = readl(PADS_SOFT(9)) | (1 << 1);
	writel(tmp, PADS_SOFT(9));
	tmp = readl(PADS_OUT_EN(9)) & ~(1 << 1);
	writel(tmp, PADS_OUT_EN(9));
	tmp = readl(PADS_OUT_GROUP(9)) & ~(1 << 1);
	writel(tmp, PADS_OUT_GROUP(9));

	return 0;
}
#endif

#if 0
void set_mux(uint32_t mux) {
    	writel(mux, PAD_SYSM_ADDR);
}

int efuse_inject(void) {
	struct efuse_paras efuse_demo = {
		.e_config    = 0,

		.mux_mmc     = 23,
		.mux_default = 22,
		.crystal     = 0x87,

		.tg_timing   = 0x51,
		.trust       = 0x7259,
		.phy_read    = 0x4023,
		.phy_delay   = 0x700,
		.polynomial  = 0x994,
		.seed        = {
			0x1c723550, 0x20fe074e, 0x1698347e,
			0x3fff2c80},
		.binfo       = {
			_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 1, 1, 15)), // 0
			_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 2, 1, 15)), // 1
			_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 2, 1, 15)), // 2
			_BI(_TOID(DEV_BND)| _TON(0, 1, 2, 2, 1, 15)), // 3
			_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 2, 1, 15)), // 4
			_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 3, 1, 4)), // 5
			_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 3, 1, 4)), // 6
			_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 3, 1, 15)), // 7
			_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 3, 1, 15)), // 8
			/* IIC EEPROM */
			_BI(_TOID(DEV_EEPROM) | _TOE(0x50, 0)), // 9
			_BI(_TOID(DEV_EEPROM) | _TOE(0x50, 1)), // 10

			_BI(BI_RESERVED), // 11
		}
	};
	int i, a = ECFG_PARA_OFFSET;
	uint8_t *d = (uint8_t *)&efuse_demo;
	uint8_t key256[32] = {
		0x03, 0x81, 0xaa, 0x26, 0x88, 0x66, 0x1d, 0x15,
		0x53, 0xe1, 0x55, 0x20, 0x67, 0x5b, 0xbf, 0xd2,
		0xfe, 0x79, 0xdb, 0x44, 0x35, 0x49, 0x5a, 0xab,
		0xc3, 0x4a, 0x4b, 0xc6, 0x2e, 0xb9, 0xc0, 0xcc
	};

	/* write table into e-fuse */
	for(i = 0; i < sizeof(struct efuse_paras); i++, a += 4)
		writel(d[i], EFUSE_WEMU_ADDR + a);

	/* enable modules configured by e-fuse */
	a = 351 * 4;
	for(i = 0; i < 8; i++, a += 4)
		writel(0x00, EFUSE_WEMU_ADDR + a);

	/* write ISK & default key */
	a = 287 * 4;
	for(i = 0; i < 32; i++, a += 4)
		writel(key256[i], EFUSE_WEMU_ADDR + a);

	a = 319 * 4;
	for(i = 0; i < 32; i++, a += 4)
		writel(1, EFUSE_WEMU_ADDR + a);

	a = 359 * 4; /* 0x77811f352c07 */
	for(i = 0; i < 6; i++, a += 4)
		writel(key256[19 - i], EFUSE_WEMU_ADDR + a);

	/* send sysmgr to re-read e-fuse info */
	writel(1, EFUSE_SYSM_ADDR);

	/* re-init the e-fuse paras in iROM */
	init_efuse();

	return 0;
}
#endif

