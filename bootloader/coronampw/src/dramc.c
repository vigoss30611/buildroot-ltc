#include <dramc_init.h>
#include <asm-arm/io.h>
#include <preloader.h>
#include <items.h>
#include <asm-arm/arch-coronampw/imapx_base_reg.h>
#include <asm-arm/arch-coronampw/imapx_dramc.h>
#include <configs/imapx800.h>
#include <linux/bitops.h>
//#define DRAMC_DEBUG
#ifdef DRAMC_DEBUG
#define dramc_debug(x...) spl_printf("[DRAMC_Q3F]" x)
#else
#define dramc_debug(x...)
#endif

#define strcmp(x, y) irf->strcmp(x, y)
#define strncmp(x, y, z) irf->strncmp(x, y, z)
#define DQS_ADDR	0x7FFFE000
#define CRC_MAGIC	0x5a6a7a99

#define MEMC_DRAM_DATA_WIDTH 16
#define MEMC_FREQ_RATIO    2
#define DRAM_NAYA
#define MDLEN 1
//#define BURST 1

//#define RR_MODE

/*
 * author:tsico
 * aim:for ddr3 2T Timing mode
 * *****************************/
//#define MEMC_2T_MODE
/*******************************/

#define IRAM_TEST 1
//#define DRAM_TEST_RTL 
//#define BYTE_TEST

static volatile int dramc_wake_up_flag = 0;

/*
 * division designed by terry using number shift and subtraction, to improve efficiency
 * and in case that the arm-gcc compiler don't support division
 * @v1: dividend
 * @v2: divisor
 *
 * return quotient
 */
uint32_t division(uint32_t v1, uint32_t v2)
{
        int i;
        uint32_t ret = 0;
        uint32_t dividend[2] = { v1, 0 };

        for(i = 0; i < 32; i++) {
                if(dividend[0] & (1 << 31))
                        break;
                else
                        dividend[0] <<= 1;
        }
        for(; i < 32; i++) {
                dividend[1] = (dividend[1] << 1) + (dividend[0] >> 31);
                dividend[0] <<= 1;
                ret <<= 1;
                if(dividend[1] >= v2) {
                        dividend[1] -= v2;
                        ret++;
                }
        }

        return ret;

}

/* describe all the infomation of drams that supported by infotm, we set their default valaues first */
static struct infotm_dram_info dram = {
        .type = DDR3,
        .cl = 6,
        .width = 8,
        .rtt = 0x4,
        .freq = 444,
        .addr_map_flag = 0,     /* 1: screen res high    0: screen res low */
        .timing = {
                .trfc_d = 0,
		//.tras	= 0,	/*defule set in fun: dramc_get_items*/
        },
};

/* describe whether it is a wake up process */
int dramc_set_para(void)
{
	uint32_t tmp;
	
	uint32_t rank[][3] = {
		/* size 512MB, size 1GB, size 2GB */
		{ 1, 1, 1 },        /* total width 16bit */
		{ 1, 1, 1 },        /* total width 32bit */	//1G 32bit width need verify?
		{ 3, 3, 3 }         /* total width 64bit */
	};

	/* type : DDR3, LPDDR2S2, LPDDR2S4  DDR2*/
	dram.type = DDR3;

	/* freq */
	dram.freq = 400;

	/* cl : 2 ~ 8 */
	dram.cl = 6;

	/* count: 2, 4, 8 */
#ifdef DRAM_NAYA
	dram.count = 1;
#else
	dram.count = 2;
#endif
	dram.count = ffs(dram.count) - 1;                       /* we set the registers using its ffs value */

	/* width: 8, 16, 32 */
	dram.width = 16;
	dram.width = ffs(dram.width) - 3;
	
	if((dram.count + dram.width > 4) || (dram.count + dram.width < 2)) {               /* the total width must in the range of 16bit to 64bit */
		dramc_debug("the total dram width is not between 16bit ~ 64bit, dram configuration error!\n");
		return 1;
	}

	/* capacity: 8MB, 16MB, 32MB, 64MB, 128MB, 256MB, 512MB, 1024MB */
#ifdef DRAM_NAYA
	dram.capacity = 128;
#else
	dram.capacity = 512;
#endif
	dram.capacity = ffs(dram.capacity) - 4;

	/* rtt: 0x4, 0x40, 0x44 */
	if(dram.type == DDR3 || dram.type == DDR2) {
		tmp = 44;
		dram.rtt = ((tmp == 0) ? 0x4 : ((tmp == 1) ? 0x40 : 0x44));
	}

	/* driver, for ddr3: 0x0, 0x2; for lpddr2: 1 ~ 7 */
	dram.driver     = ((dram.type == DDR3 || dram.type == DDR2) ? DRIVER_DIV_6 : DS_40_OHM);     /* set default driver first, ddr3 and lpddr2 respectively */
	
	/*memory.driver DDR3:  "6", "7"  LPDDR: "34.3", "40", "48", "60", "68.6", "resv", "80", "120"*/
	if(dram.type == DDR3)
		tmp = 0;
	else
		tmp = 0; 
	if(dram.driver == DDR3 || dram.type == DDR2)
		dram.driver = ((tmp == 0) ? 0x0 : 0x2);
	else
		dram.driver = tmp + 1;

		/*change for aofeng*/
        dram.driver = 0x2;
		dram.rtt = 0x04;
	/* dram timing trfc_d and tras */
	dram.timing.trfc_d = 64;
	if(dram.type == DDR3 || dram.type == DDR2)
		dram.timing.tras = ((dram.freq < 410) ? 15 : 20);
	else
		dram.timing.tras = division(42 * dram.freq, 1000);              /* set default tras first, if tras is not found in items, use this default value */

	/* dram addr map, UMCTL2 only support 0 mode: bank.row.column*/
	dram.addr_map_flag = 0;

	/* reduce_flag, burst_len, rank_sel and size, they are caculated by other parameters, not read from items.itm */
	dram.reduce_flag = ((1 << (dram.count + dram.width + 2)) < MEMC_DRAM_DATA_WIDTH);
	dram.burst_len = ((dram.type == DDR3 || dram.type == DDR2) ? BURST_8 : BURST_4);
	tmp = dram.count + dram.capacity;
	if (tmp < 6)
		tmp = 6;
	dram.rank_sel = rank[dram.count + dram.width - 2 ][tmp - 6];
	dram.size = 1 << (dram.count + dram.capacity + 3);
	dramc_debug("count width capacity: %d, %d %d, size 0x%x\n", dram.count, dram.width, dram.capacity, dram.size);

	return 0;
}

int dramc_set_addr(void)
{
	uint32_t lpddr2s2_row_table[] = { 12, 12, 13, 13, 14, 15, 14, 15 };
	uint32_t lpddr2s4_row_table[] = { 12, 12, 13, 13, 13, 14, 14, 15 };

	if(dram.type == DDR3 || dram.type == DDR2) {
		if(((dram.width != IO_WIDTH8) && (dram.width != IO_WIDTH16) && (dram.width != IO_WIDTH32)) || (dram.capacity < DRAM_64MB))
			return 1;
		if(dram.capacity == DRAM_1GB) {		//if has, need redefined, col 11 = ?
			dram.bank = 3;
			dram.col = 11;
			dram.row = 16;          // the max number of row line is 16
		} else {
			dram.bank = 3;
			dram.col = 10;
			dram.row = 24 + dram.capacity - dram.width - dram.bank - dram.col;
		}
	} else if(dram.type == LPDDR2S2) {
		dram.bank = ((dram.capacity < DRAM_512MB) ? 2 : 3);
		dram.row = lpddr2s2_row_table[dram.capacity - DRAM_8MB];
		dram.col = 24 + dram.capacity - dram.width - dram.bank - dram.row;
	} else if(dram.type == LPDDR2S4) {
		dram.bank = ((dram.capacity < DRAM_128MB) ? 2 : 3);
		dram.row = lpddr2s4_row_table[dram.capacity - DRAM_8MB];
		dram.col = 24 + dram.capacity - dram.width - dram.bank - dram.row;
	}

	return 0;
}

void dramc_set_frequency(void)
{
	//writel(0x11, 0x2d00a09c);
        irf->set_pll(PLL_V, 1560);
        irf->module_set_clock(DDRPHY_CLK_BASE, PLL_V, 1); //set ddr clk = vpll/4
        //irf->module_set_clock(BUS5_CLK_BASE, PLL_V, 3); //set ddr clk = vpll/4
	//writel(0x3, 0x2d00a170);
	//writel(0x2, 0x2d00a174);
	//writel(0x0, 0x2d00a17c);
        irf->module_set_clock(APB_CLK_BASE, PLL_V, 7); //set apb clk = vpll/16
}
#if 0
static const struct infotm_freq_2_param freq_2_param[] = 
{
	{ 804, 0x0042 }, { 780, 0x0041 }, { 600, 0x0031 }, { 576, 0x002f }, { 552, 0x002d }, { 528, 0x002b }, { 504, 0x0029 }, { 468, 0x104d },
	{ 444, 0x1049 }, { 420, 0x1045 }, { 400, 0x1041 }, { 372, 0x103d }, { 348, 0x1039 }, { 330, 0x1036 },
	{ 312, 0x1033 }, { 288, 0x102f }, { 264, 0x102b },
};


void dramc_set_freqency_phy(uint32_t pll_p, uint32_t phy_div)
{
	pll_set_clock("vpll", 1560000000);  //coronampw VPLL change to 768MHZ, DDR 384MHZ ----> corona vpll 1600, ddr 800MHZ
	module_set_clock("dram", "vpll", pll_p*M_HZ, 0);
	module_set_clock("apb", "vpll", (pll_p & 0xffff) * M_HZ / 8, 0);    //apb is vpll 8 div, 96MHZ
}

void dramc_set_frequency(void)
{
	uint32_t i;

	for(i = 0; i < FR_NUM; i++) {
		if(dram.freq >= freq_2_param[i].freq) {
			dramc_set_freqency_phy(freq_2_param[i].freq, 1);
			break;
		}
	}

	/* other dram freqencys which are not in freq_val table */
	if(i == FR_NUM) {
		if(dram.freq >= 200)
			dramc_set_freqency_phy(0x1041, 3);
		else if(dram.freq >= 150)
			dramc_set_freqency_phy(0x1031, 3);
		else
			dramc_set_freqency_phy(0x1031, 3);
	}
	dramc_debug("freq: %d, param 0x%x\n", freq_2_param[i].freq, freq_2_param[i].param);
}
#endif
void dramc_set_timing(void)
{
	struct infotm_dram_timing *timing = &dram.timing;
	const uint32_t trefi_param[][8] = {
		{ 78, 78, 78, 78, 78, 78, 78, 78 },      /* ddr3, 78 means 7.8us */
		{ 156, 156, 78, 78, 78, 39, 39, 39},    /* lpddr2S2, lpddr2S4 */
	};
	const uint32_t trfc_param[][8] = {
		{ 90000, 90000, 90000, 90000, 110000, 160000, 300000, 380000 },  /* ddr3 */
		{ 90000, 90000, 90000, 90000, 130000, 130000, 130000, 210000 }, /* lpddr2S2, lpddr2S4 */
	};
	const uint32_t lpddr2_timing[] = { 3, 5, 6, 10, 12, 13 }; /* cl 3 ~ 8 */

	/* trefi set */
	timing->trefi = division(trefi_param[dram.type != DDR3][dram.capacity]* dram.freq, 32 * 10);

	/* trrd, tfaw, faw_eff */
	if((dram.width == IO_WIDTH16) && (dram.freq > 410)) {
		timing->trrd = 6;
		timing->tfaw = 27;
		timing->faw_eff = (1 << 0) | (3 << 16); /* set tfaw = 5 * trrd - 3 */
	} else if ((dram.width == IO_WIDTH16) || (dram.freq > 410)) {
		timing->trrd = 4;
		timing->tfaw = 20;
		timing->faw_eff = 1;    /* set tfaw = 5 * trrd */
	} else {
		timing->trrd = 4;
		timing->tfaw = 16;
		timing->faw_eff = 0;    /* set tfaw = 4 * trrd */
	}

	/* trc */
	timing->trc = timing->tras + dram.cl;

	/* twr 15ns */
	if(dram.freq <= 350)
		timing->twr = 5;
	else if (dram.freq <= 410)
		timing->twr = 6;
	else if (dram.freq <= 475)
		timing->twr = 7;
	else if (dram.freq <= 550)
		timing->twr = 8;
	else if (dram.freq <= 650)
		timing->twr = 10;
	else
		timing->twr = 12;

	/* tcwl */
	if(dram.type == DDR3){
		timing->tcwl = (dram.cl < 7) ? 5 : 6;
		if (dram.cl == 10)
			timing->tcwl = 8;
	}else{
		timing->tcwl = dram.cl / 2;	//for LPDDR2
		if (dram.cl == 7)
			timing->tcwl = 4;
	}

	/* trfc */
	timing->trfc = division(trfc_param[dram.type != DDR3][dram.capacity] * dram.freq, 1000000);

	/* trfc_d, trfcd, trp */
	timing->trfc_d = timing->trfc - timing->trfc_d;
	timing->trcd = (dram.type == DDR3) ? dram.cl : (lpddr2_timing[dram.cl - 3]);
	timing->trp = timing->trcd;
}

/*before memory init need DRAM controler & DLL reset*/
void emif_module_reset(void)
{
//	unsigned long timing = 0;
	unsigned long lock_pll;

	lock_pll = 0x00000002;	//Impedance Calibration Done Flag
	
	/*RTC user GP0 output mode*/
	//if(!dramc_wake_up_flag) {		//RTL no wakeup

	/*emif bus reset*/
	/*bus reset need bufore core, or ddr can not write*/
	writel(0xff, EMIF_SYSM_ADDR_BUS_AND_PHY_PLL_CLK);//enable dram axi port 0-4 axi bus clk, pll phy clk
	writel(0xff, EMIF_SYSM_ADDR_APB_AND_PHY_CLK);//enable dram   phy ctl_clk ctl_hdr_clk

	irf->udelay(10);
	writel(0xff, EMIF_SYSM_ADDR_BUS_RES);
	writel(0x7, EMIF_SYSM_ADDR_CORE_RES);
	irf->udelay(10);
	//udelay(0x10);	//timer need Todo
	writel(0x5, EMIF_SYSM_ADDR_CORE_RES);

	//writel(0x000001, ADDR_PHY_PIR);
	irf->udelay(10);
//	writel(0x38000 | 0x1<<30,      ADDR_PHY_PLLCR);          /* ddr system init */
	/*wait for pll lock*/
	dramc_debug("ADDR_PHY_PGSR00 = 0x%x\n", readl(ADDR_PHY_PGSR));  /*Read General Status Register*/
	while((readl(ADDR_PHY_PGSR) & lock_pll) != lock_pll);
	dramc_debug("ADDR_PHY_PGSR11 = 0x%x\n", readl(ADDR_PHY_PGSR));  /*Read General Status Register*/
	writel(0x38000,      ADDR_PHY_PLLCR);          /* ddr system init */
#if 0
	writel(0x81, ADDR_PHY_PIR);
	while((readl(ADDR_PHY_PGSR) & 0x1) != 0x1);
	writel(0x41, ADDR_PHY_PIR);
	while((readl(ADDR_PHY_PGSR) & 0x1) != 0x1);
#endif
}

/**
 * Program the DWC_ddr_umctl2 registers
 * Ensure that DFIMISC.dfi_init_complete_en is set to 0
 */
void DWC_umctl2_reg(void)
{
	int i, tmp, cl;
	cl = dram.cl;
	struct infotm_dram_timing *timing = &dram.timing;

	writel(0x49494949, ADDR_PHY_IOVCR0);
	writel(0x49, ADDR_PHY_IOVCR1);
	writel(0x1, ADDR_UMCTL2_DBG1);
	writel(0x1, ADDR_UMCTL2_PWRCTL);
#ifdef MEMC_2T_MODE
	writel(0x83040001 | 1 << 10,  ADDR_UMCTL2_MSTR);	//0x3040001 ----> fpga 0x4304011(add device width & interleaved mode)
#else
	writel(0x83040001,  ADDR_UMCTL2_MSTR);	//0x3040001 ----> fpga 0x4304011(add device width & interleaved mode)
#endif
	writel(0x3030,		ADDR_UMCTL2_MRCTRL0);
	writel(0x000053d6,	ADDR_UMCTL2_MRCTRL1);
	writel(0x20,		ADDR_UMCTL2_DERATEEN);
	writel(0x88a5afe3,	ADDR_UMCTL2_DERATEINT);
	writel(0xa,			ADDR_UMCTL2_PWRCTL);
	writel(0x000e3202,	ADDR_UMCTL2_PWRTMG);
	writel(0x000b0003,	ADDR_UMCTL2_HWLPCTL);
	writel(0x00202070 ,	ADDR_UMCTL2_RFSHCTL0);
	writel(0x00200009,	ADDR_UMCTL2_RFSHCTL1);
	writel(0x0021c070,	ADDR_UMCTL2_RFSHCTL0);
	writel(0x0019005c,	ADDR_UMCTL2_RFSHCTL1);
	writel(0x00000000,	ADDR_UMCTL2_RFSHCTL3);
	if(dram.freq == 800){
		writel(0x00610068,	ADDR_UMCTL2_RFSHTMG);	//0x610040 ---> 0xc0012 for fpga define (trefi & trfc)
	}else{
		tmp =(((timing->trefi & 0xfff) << 16) | (timing->trfc & 0xff)); 
		writel((tmp+1)/MEMC_FREQ_RATIO, ADDR_UMCTL2_RFSHTMG);
	}

	writel(0x00000000,	ADDR_UMCTL2_CRCPARCTL0);

	writel(0x40030006,	ADDR_UMCTL2_INIT0);
	writel(0x00010105,	ADDR_UMCTL2_INIT1);
	if(dram.freq == 800){
		writel(0x0000fd0a,	ADDR_UMCTL2_INIT2);
		writel(0x0d500000,	ADDR_UMCTL2_INIT3);
		writel(0x00180000,	ADDR_UMCTL2_INIT4);
		writel(0x00090246,	ADDR_UMCTL2_INIT5);
		writel(0x00000264,	ADDR_UMCTL2_RANKCTL);
		writel(0x0c101a0e,	ADDR_UMCTL2_DRAMTMG0);
		writel(0x000a0313,	ADDR_UMCTL2_DRAMTMG1);
		writel(0x04050409,	ADDR_UMCTL2_DRAMTMG2);
	} else {
		writel(0xc06, ADDR_UMCTL2_INIT2);
		if (dram.type == DDR2)
			tmp = 3 | (cl << 4) | ((timing->twr - 1) << 9);
		else{
			if(timing->twr < 10)
				tmp = ((cl - 4) << 4) | ((timing->twr - 4) << 9);
			else
				tmp = ((cl - 4) << 4) | ((timing->twr / 2) << 9);
		}
		writel(tmp<<16 | (dram.rtt | dram.driver),	ADDR_UMCTL2_INIT3);          /* CAS latency, write recovery(use twr)  why not match with INIT3 MR1 & MR0*/
		tmp = ((timing->tcwl - 5) << (3+16));
		writel(tmp, ADDR_UMCTL2_INIT4);      
		tmp = 0x11 << 16 | (dram.type == DDR3 ? 1 : division((dram.freq * 10 +1023), 1024)+1); 
		writel(tmp, ADDR_UMCTL2_INIT5);
		writel((MEMC_FREQ_RATIO == 2) ? 0x000366 : 0x00066b, ADDR_UMCTL2_RANKCTL);
		tmp = (timing->tcwl + 4 + division(15 * dram.freq, 1000) + MEMC_FREQ_RATIO - 1) << 24
			| ((dram.bank - 2)? (timing->tfaw+MEMC_FREQ_RATIO-1):MEMC_FREQ_RATIO) << 16 | (timing->tras + 10)<< 8
			| (timing->tras + MEMC_FREQ_RATIO - 1); //0xf10190f alin 0x0f101a0f
		writel(tmp/MEMC_FREQ_RATIO, ADDR_UMCTL2_DRAMTMG0);
		tmp = 0x8 << 16 | 0x4 << 8 | (timing->trc + MEMC_FREQ_RATIO - 1);
		writel(tmp/MEMC_FREQ_RATIO,         ADDR_UMCTL2_DFITMG1);
		tmp = (timing->tcwl + MEMC_FREQ_RATIO - 1)<< 24 | cl << 16 | (4+cl + 6 -timing->tcwl + MEMC_FREQ_RATIO -1 + ((dram.type == DDR3 || dram.type == DDR2)? 0 : 3)) << 8 | (timing->tcwl + 4 + 8 + MEMC_FREQ_RATIO - 1);
		writel(tmp/MEMC_FREQ_RATIO, ADDR_UMCTL2_DRAMTMG2);
	}
	writel(0x00000000,	ADDR_UMCTL2_DIMMCTL);

	tmp = (dram.type == DDR3 ? 0:((dram.type == LPDDR3) ? 10:5)) << 20 | 0x4/MEMC_FREQ_RATIO << 12 | (dram.type == DDR3 ? (0xd+MEMC_FREQ_RATIO-1)/MEMC_FREQ_RATIO : 0);
	writel(tmp,	ADDR_UMCTL2_DRAMTMG3);
	tmp = 0x00020300 | (timing->trcd << 24) | timing->trp;
	tmp = timing->trcd << 24 | 0x4 << 16 | timing->trrd << 8 | timing->trp; 
	writel(tmp/MEMC_FREQ_RATIO + (MEMC_FREQ_RATIO == 2 ? 1 : 0) ,	ADDR_UMCTL2_DRAMTMG4);
	tmp = (MEMC_FREQ_RATIO == 2) ? 0x1020302: 0x2040503;
	writel(tmp,	ADDR_UMCTL2_DRAMTMG5);
	writel((dram.type == DDR3) ? 0:((MEMC_FREQ_RATIO==1)?0x2020005:0x01010003),  ADDR_UMCTL2_DRAMTMG6);
	writel((dram.type == DDR3) ? 0:0x202/MEMC_FREQ_RATIO,   ADDR_UMCTL2_DRAMTMG7);
	writel((MEMC_FREQ_RATIO==1) ? 0x06061004: 0x03030802, ADDR_UMCTL2_DRAMTMG8);


	writel(0x60800020, 	ADDR_UMCTL2_ZQCTL0);
	writel(0x00000170, 	ADDR_UMCTL2_ZQCTL1);
	writel(0x00000000, 	ADDR_UMCTL2_ZQCTL2);

	if(dram.freq == 800)
		writel(0x02030102,  ADDR_UMCTL2_DFITMG0);
	else{
		tmp = 0x02000000
			|(((cl - 4 + cl%2) >> 1 ) << 16)  //dfi_rddata_en
			|((2 >> 1) << 8)                          //dfi_tphy_wrdata
			|((timing->tcwl - 4 +  (timing->tcwl%2))>>1);
#ifdef RR_MODE
		tmp = 0x03000000
			|(((cl - 4 + cl%2) >> 1 ) << 16)  //dfi_rddata_en
			|((2 >> 1) << 8)                          //dfi_tphy_wrdata
			|((timing->tcwl - 4 +  (timing->tcwl%2))>>1);
#endif
		writel(tmp,     ADDR_UMCTL2_DFITMG0);
		//writel(0x02010101,     ADDR_UMCTL2_DFITMG0);
	}
#ifdef RR_MODE
	writel((0x00030203+0x10001)/MEMC_FREQ_RATIO,  ADDR_UMCTL2_DFITMG1);  //+1 for roundup
#else
	writel(0x00030202/MEMC_FREQ_RATIO,  ADDR_UMCTL2_DFITMG1);
#endif
	writel(0x07217111, 	ADDR_UMCTL2_DFILPCFG0);
	writel(0x00040003,	ADDR_UMCTL2_DFIUPD0);
	writel(0x007f008d,	ADDR_UMCTL2_DFIUPD1);
	writel(0x00000000, 	ADDR_UMCTL2_DFIUPD2);
	writel(0x1, 		ADDR_UMCTL2_DFIMISC);

	/*DDR addr map reg 0~6*/
#if 0
	writel(0x0016,		ADDR_UMCTL2_ADDRMAP0);	//rank defined
	writel(0x080808,	ADDR_UMCTL2_ADDRMAP1);	//band defined
	writel(0,			ADDR_UMCTL2_ADDRMAP2);
	writel(0x00000000, 	ADDR_UMCTL2_ADDRMAP3);
	writel(0x0f0f,		ADDR_UMCTL2_ADDRMAP4);			//col bit12 bit13 not used
	writel(0x07070707, 	ADDR_UMCTL2_ADDRMAP5);
	writel(0x0f070707, 	ADDR_UMCTL2_ADDRMAP6);
	writel(0x07070707, 	ADDR_UMCTL2_ADDRMAP9);
	writel(0x07070707, 	ADDR_UMCTL2_ADDRMAP10);
	writel(0x00000007, 	ADDR_UMCTL2_ADDRMAP11);
#else
	//ddr3 half mode umctl v2.4 , col2-9 adjust to col3-9
	writel((0x1f00 | (dram.row + (!dram.reduce_flag ? 0x7 : 0x6))), ADDR_UMCTL2_ADDRMAP0);    //rank defined
	writel(0x070707 + (!dram.reduce_flag ? 0x010101:0),     ADDR_UMCTL2_ADDRMAP1);        //bank 0~2 always in HIF bit[9~11]

	if(!dram.reduce_flag){
		writel(0,     ADDR_UMCTL2_ADDRMAP2);
		writel(0x00000000,     ADDR_UMCTL2_ADDRMAP3);
	}
	else{
		writel(0x00000000,      ADDR_UMCTL2_ADDRMAP2);
		writel(0x0f000000,     ADDR_UMCTL2_ADDRMAP3);
	}
	writel(0x0f0f,     ADDR_UMCTL2_ADDRMAP4);            //col bit12 bit13 not used
	writel((!dram.reduce_flag ? 0x07070707:0x06060606), ADDR_UMCTL2_ADDRMAP5);

	//b12 base 18, [25] ,program to 7, or 6 (reduced)
	if(dram.row == 12)
		writel(0x0f0f0f0f, ADDR_UMCTL2_ADDRMAP6);
	if(dram.row == 13)
		writel((!dram.reduce_flag ? 0x0f0f0f07:0x0f0f0f06), ADDR_UMCTL2_ADDRMAP6);
	if(dram.row == 14)
		writel((!dram.reduce_flag ? 0x0f0f0707:0x0f0f0606), ADDR_UMCTL2_ADDRMAP6);    /*X9_EVB: full mode, bank 0 in HIF bit[26]: (0x14 + offset:0x6) */
	if(dram.row == 15)
		writel((!dram.reduce_flag ? 0x0f070707:0x0f060606), ADDR_UMCTL2_ADDRMAP6);    /*X9_EVB: full mode, bank 0 in HIF bit[27]: (0x15 + offset:0x6) */
	if(dram.row == 16)
		writel((!dram.reduce_flag ? 0x07070707:0x06060606), ADDR_UMCTL2_ADDRMAP6);    /*X9_EVB: full mode, bank 0 in HIF bit[28]: (0x16 + offset:0x6) */

	//b2 base 8
	writel((!dram.reduce_flag ? 0x07070707:0x06060606), ADDR_UMCTL2_ADDRMAP9);
	writel((!dram.reduce_flag ? 0x07070707:0x06060606), ADDR_UMCTL2_ADDRMAP10);
	writel((!dram.reduce_flag ? 0x00000007:0x00000006), ADDR_UMCTL2_ADDRMAP11);
#endif

	if(dram.freq == 800)
		writel(0x030b0c1c,	ADDR_UMCTL2_ODTCFG);
	else{
		tmp = (0x06000600 | (cl - timing->tcwl) << 2);
		writel(tmp,  ADDR_UMCTL2_ODTCFG);
	}
	writel(0x00000011,	ADDR_UMCTL2_ODTMAP);		//rank otd config
	writel(0x2a861f05,	ADDR_UMCTL2_SCHED);
	writel(0x00000006,	ADDR_UMCTL2_SCHED1);
	writel(0x040000c8,	ADDR_UMCTL2_PERFHPR1);
	writel(0x040000c9,	ADDR_UMCTL2_PERFLPR1);
	writel(0x040000c8,	ADDR_UMCTL2_PERFWR1);
	writel(0x00000764,	ADDR_UMCTL2_PERFVPR1);
	writel(0x00000154,	ADDR_UMCTL2_PERFVPW1);
	writel(0x00000010,	ADDR_UMCTL2_DBG0);
	writel(0x00000000,	ADDR_UMCTL2_DBG1);
	writel(0x0,			ADDR_UMCTL2_DBGCMD);

	writel(0x1,				ADDR_UMCTL2_SWCTL);

	/*for init UMCTL2 port interface*/
	writel(0x10,			ADDR_UMCTL2_PCCFG);
	for(i = 0; i < 4; i++){
		writel(0x00004080 + i, ADDR_UMCTL2_PCFGR(i));
		writel(0x00004080 + i, ADDR_UMCTL2_PCFGW(i));
	}
	for(i = 0; i < 5; i++){
		writel(0x00000001, ADDR_UMCTL2_PCTRL(i));
		writel(0x02100053, ADDR_UMCTL2_PCFGQOS0(i));
		writel(0x00ff00ff, ADDR_UMCTL2_PCFGQOS1(i));
		writel(0x00100005, ADDR_UMCTL2_PCFGWQOS0(i));
		writel(0x000000ff, ADDR_UMCTL2_PCFGWQOS1(i));
	}

	writel(0x0,				ADDR_UMCTL2_RFSHCTL3);
	writel(0x00000000,		ADDR_UMCTL2_DBG1);
	writel(0x8,				ADDR_UMCTL2_PWRCTL);
	writel(0x0,				ADDR_UMCTL2_SWCTL);
	writel(0x0,				ADDR_UMCTL2_DFIMISC);
}

void ddr_phy_set(void)
{
	struct infotm_dram_timing *timing = &dram.timing;
	int i, tmp, cl, tmp1;
	cl = dram.cl;

#ifdef RR_MODE
	writel(0x0020643f | (1 << 18),	ADDR_PHY_DSGCR);
#else
	writel(0x0020643f,	ADDR_PHY_DSGCR);
#endif

	writel(0x23, ADDR_PHY_ZQ0PR); 

	writel(0x10c00410, ADDR_PHY_ACIOCR(0));
	writel(0x55555555, ADDR_PHY_ACIOCR(1));
	writel(0xaaaaaaaa, ADDR_PHY_ACIOCR(2));
	for(i = 3; i < 6; i++)
		writel(0x00000000, ADDR_PHY_ACIOCR(i));

	for(i = 0; i < 4; i++){
		writel(0x00000000, ADDR_PHY_DXnGCR1(i));
		writel(0x00000000, ADDR_PHY_DXnGCR2(i));
		writel(0x00000000, ADDR_PHY_DXnGCR3(i));
	}

#ifdef MEMC_2T_MODE
	writel(0x00000003 | 1<<28 | 1 << 3,  ADDR_PHY_DCR);          /* DDR8BNK, DDRMD(DDR3) */
#else
	writel(0x00000003 | 1<<3,  ADDR_PHY_DCR);          /* DDR8BNK, DDRMD(DDR3) */
#endif

	writel(0x00181884 | MDLEN<<2,	ADDR_PHY_DXCCR);

	dramc_debug("====ADDR_PHY_PLLCR= 0x%x\n", readl(ADDR_PHY_PLLCR));  /*Read General Status Register*/
	//writel(0,	ADDR_PHY_PLLCR);
	#define TIMING_PHYRST 0x10
	#define TIMING_PLLGS  4000
	#define TIMING_PLLPD  1000
	#define TIMING_PLLRST  9000
	#define TIMING_PLLLOCK 100000
	#define TIMING_DINIT0  500000
	#define TIMING_DINIT2  200000
	#define TIMING_DINIT3  1000
	#define TIMING_DELAY  30

	tmp = division(TIMING_PLLGS * 78, 1000) + TIMING_DELAY;
	tmp1 = division(TIMING_PLLPD * 78, 1000) + TIMING_DELAY;
	writel(TIMING_PHYRST | tmp << 6 | tmp1 << 21,	ADDR_PHY_PTR0);
	tmp = division(TIMING_PLLRST * 78, 1000) + TIMING_DELAY;
	tmp1 = division(TIMING_PLLLOCK * 78, 1000) + TIMING_DELAY,
	writel(tmp | tmp1 << 16,	ADDR_PHY_PTR1); /* DLL soft reset time, DLL lock time, ITM soft reset time */
	//writel(0x00000d50,	ADDR_PHY_MR0);          /* CAS latency, write recovery(use twr)  why not match with INIT3*/
	if (dram.type == DDR2)
		tmp = 3 | (cl << 4) | ((timing->twr - 1) << 9);
	else{
		if(timing->twr < 10)
			tmp = ((cl - 4) << 4) | ((timing->twr - 4) << 9);
		else
			tmp = ((cl - 4) << 4) | ((timing->twr / 2) << 9);
	}
	writel(tmp ,	ADDR_PHY_MR0);          /* CAS latency, write recovery(use twr)  why not match with INIT3*/
	writel(dram.rtt | dram.driver, ADDR_PHY_MR1);
	writel((timing->tcwl - 5) << 3,	ADDR_PHY_MR2);
	writel(0x00000000,  ADDR_PHY_MR3);
	//twtr[4:7] must max(4tck, 7.5ns)
	writel(0x00100006 | (timing->trcd << 26) | (timing->trrd << 22) |(timing->tras << 16)|(timing->trp << 8) | (8<<4),	ADDR_PHY_DTPR0);
	writel(0x22800000 | (timing->trfc << 11) | (timing->tfaw << 5),	ADDR_PHY_DTPR1);
	writel(0x0010c009 | (timing->trc << 6),	ADDR_PHY_DTPR3);
	writel(0x20021405 | (1 <<30),	ADDR_PHY_DTPR2);
	writel(0x0380c4a0 | MDLEN << 9,	ADDR_PHY_PGCR1);
	//tmp = (BURST+1) * 7.8 * dram.freq - 400*dram.freq/1000;
	tmp = 1 * 78 * dram.freq / 10;
	writel(0x00f00000 | tmp,	ADDR_PHY_PGCR2);
	tmp = division(TIMING_DINIT0 * dram.freq, 1000)+ TIMING_DELAY,
	tmp1 = timing->trfc + division(10 * dram.freq, 1000)+ TIMING_DELAY,   //tDINIT t RFC + 10 ns or 5 tCK, whichever is bigger
	writel(tmp1 <<20 | tmp,	ADDR_PHY_PTR3); /* DLL soft reset time, DLL lock time, ITM soft reset time */
	tmp = division(TIMING_DINIT2 * dram.freq, 1000)+ TIMING_DELAY,
	tmp1 = division(TIMING_DINIT3 * dram.freq, 1000) + TIMING_DELAY,
	writel(tmp1 << 18 | tmp,	ADDR_PHY_PTR4); /* DLL soft reset time, DLL lock time, ITM soft reset time */

	dramc_debug("ADDR_PHY_PGSR = 0x%x\n", readl(ADDR_PHY_PGSR));  /*Read General Status Register*/
	while((readl(ADDR_PHY_PGSR) & 0x1) != 0x1);	/* wait for init done and dll lock done */
	writel(0x00000181, ADDR_PHY_PIR);

	while((readl(ADDR_PHY_PGSR) & 0x8000001f) != 0x8000001f);	/* wait for init done and dll lock done */

	for(i = 0; i < 4; i++)
		writel(0x00006e6e, ADDR_PHY_DXnLCDLR2(i));
		//writel(0x0000006e, ADDR_PHY_DXnLCDLR2(i));

	writel(0x0,				ADDR_UMCTL2_SWCTL);
}

void umctl2_to_normal(void)
{
#ifdef	CONFIG_COMPILE_FPGA
	writel(0x0,				ADDR_UMCTL2_SWCTL);
	writel(0x00000008,		ADDR_UMCTL2_PWRCTL);
	writel(0x0,				ADDR_UMCTL2_DBG1);
#endif
	writel(0x1,				ADDR_UMCTL2_DFIMISC);

	//#ifdef	CONFIG_COMPILE_FPGA
	writel(0x1,				ADDR_UMCTL2_SWCTL);
	//#endif
	while((readl(ADDR_UMCTL2_SWSTAT) & 0x1) != 0x1);

	/*Wait for DWC_ddr_umctl2 to move to “normal”*/
	while((readl(ADDR_UMCTL2_STAT) & 0x11) != 0x01);
#ifndef	CONFIG_COMPILE_FPGA
	writel(0x8,				ADDR_UMCTL2_PWRCTL);
	writel(0x0,				ADDR_UMCTL2_DFIMISC);
#endif
}

void DCU_load_mode(void)
{
	writel(0x00000007,	ADDR_PHY_DCUTPR);
	writel(0x00000400,	ADDR_PHY_DCUAR);
	writel(0x00001511,	ADDR_PHY_DCUDR);

	/*Trigger DCU Run*/
	writel(0x00000001,	ADDR_PHY_DCURR);
	dramc_debug("ADDR_PHY_DCUSR0 = 0x%x\n", readl(ADDR_PHY_DCUSR0));  /*Read General Status Register*/
	while((readl(ADDR_PHY_DCUSR0) & 0x1) != 0x1);	/* wait for DCU run done */

	if((readl(ADDR_PHY_DCUSR0) & 0x00000003) != 0x00000001)
		dramc_debug("WARNING DCU Load Mode FAILED! Expected   => 0x00000001\n");	/* wait for init done and dll lock done */


}

void dis_auto_fresh(void)
{
	writel(0x1, 		ADDR_UMCTL2_RFSHCTL3);
	writel(0x0, 		ADDR_UMCTL2_PWRCTL);
	writel(0x0, 		ADDR_UMCTL2_DFIMISC);
}

int ddr_training(void)
{
	writel(0x3C000305 | MDLEN << 30 | 1<<8, ADDR_PHY_DX0GCR0);
	writel(0x3C000305| MDLEN <<30 | 1<<8, ADDR_PHY_DX1GCR0);

	writel(0x01000001, ADDR_PHY_DTCR);
	writel(0x00000401, ADDR_PHY_PIR);

	while((readl(ADDR_PHY_PGSR) & 0x8000005F) != 0x8000005F);     /* wait for Data init Done */
	dramc_debug("ADDR_PHY_PGSR = 0x%x\n", readl(ADDR_PHY_PGSR));  /*Read General Status Register*/
	if((readl(ADDR_PHY_PGSR) & 0x00400000) == 0x00400000){
		dramc_debug("DQS training err\n");  /*Read General Status Register*/
		return 1;
	}

#ifndef	CONFIG_COMPILE_FPGA
	writel(0x1,				ADDR_UMCTL2_SWCTL);
#endif
	
	//writel(0x7ffff000, 	ADDR_PHY_DTAR0); 	/*data training address, column, row and bank address*/

#ifdef	CONFIG_COMPILE_FPGA
	dramc_debug("ADDR_PHY_PGSR = 0x%x\n", readl(ADDR_PHY_PGSR));  /*Read General Status Register*/
	if((readl(ADDR_PHY_PGSR) & 0x8000005F) != 0x8000005F) 
		dramc_debug("DQS Gate Training Error\n");
#endif
	return 0;
}

int dramc_init_memory(void)
{
	/*before: emif & dll module reset*/
	emif_module_reset();

	/*step1: Program the DWC_ddr_umctl2 registers*/
	DWC_umctl2_reg();

	/*step2: Deassert soft reset signal RST.soft_rstb, reset_dut*/
	writel(0x0, EMIF_SYSM_ADDR_CORE_RES);
	writel(0x0, EMIF_SYSM_ADDR_BUS_RES);

	/*step3 & step4:Start PHY initialization by accessing relevant*/
	ddr_phy_set();

	/*corona add: uMCTL2 from init to Noraml*/
	umctl2_to_normal();
	
#ifdef	CONFIG_COMPILE_FPGA
	/*corona add: PUB / DCU Load Mode*/
	DCU_load_mode();
#endif

	/*step4: Disable auto-refreshes and powerdown by setting*/
	//dis_auto_fresh();

	/*least all: Perform PUBL training as required*/
	if(ddr_training())
		return 1;
#if 0
       dramc_debug("vpll 0x%x 0x%x, 0x%x, 0x%x\n", readl(0x2d00a090), readl(0x2d00a094), readl(0x2d00a098), readl(0x2d00a09c));	
       dramc_debug("vpl1 0x%x 0x%x, 0x%x, 0x%x\n", readl(0x2d00a0a0), readl(0x2d00a0a4), readl(0x2d00a0a8), readl(0x2d00a0ac));	
       dramc_debug("vpl2 0x%x 0x%x, 0x%x, 0x%x\n", readl(0x2d00a0b0), readl(0x2d00a0b4), readl(0x2d00a0b8), readl(0x2d00a0bc));	
       dramc_debug("dram 0x%x 0x%x, 0x%x, 0x%x\n", readl(0x2d00a310), readl(0x2d00a314), readl(0x2d00a318), readl(0x2d00a31c));	
       dramc_debug("apb  0x%x 0x%x, 0x%x, 0x%x\n", readl(0x2d00a360), readl(0x2d00a364), readl(0x2d00a368), readl(0x2d00a36c));	
#define DDR_CMP_SIZE (127*0x100000/4) 
#define DDR_TEST_START 0x40000000       
	int t1 = 0, count=0;
	for(count=0; count<200; count++){
		dramc_debug("FPGA%d . write data : 1~0x%x from addr 0x%x\n", count, DDR_CMP_SIZE, DDR_TEST_START); 
		for(t1 = 0; t1 < DDR_CMP_SIZE; t1++)    
		{       
			writel(t1, (DDR_TEST_START + (t1<<2))); 
		}       
		dramc_debug("FPGA . read data : 1~0x%x from addr 0x%x\n", DDR_CMP_SIZE, DDR_TEST_START);  
		for(t1 = 0; t1 < DDR_CMP_SIZE; t1++)    
		{     
			if(t1 != readl(DDR_TEST_START + (t1<<2)))       
			{
				dramc_debug("FPGA . cmp original data 0x%x and  get value 0x%x @ addr 0x %x fail! the result is err!\n", t1, *(volatile unsigned int *)(DDR_TEST_START + (t1<<2)), (t1<<2));     
				while(1);
			}
		}
	}
	dramc_debug("dram test end\n");
#endif

	return 0;
}

int dramc_verify_size(void)
{
	unsigned long checkaddr1=0, checkaddr2=0,checkaddr3=0;

	checkaddr1 = 0x80000000;
	checkaddr2 = checkaddr1 + (dram.size << 19);
	checkaddr3 = ((checkaddr1 >> 1) + (checkaddr2 >> 1));
	dramc_debug("checkaddr1 = 0x%x, checkaddr2 = 0x%x, checkaddr3 = 0x%x\n", checkaddr1, checkaddr2, checkaddr3);
	writel(0x87654321, checkaddr2);
	writel(0x12345678, checkaddr1);
	writel(0x87654321, checkaddr3);

	return ((readl(checkaddr2) != 0x87654321) ? 1:(readl(checkaddr1) != 0x12345678));
}

/**
 * parse items which have integer values,
 * use the value if found in items.itm, use default if not found
 * @key: the name of the item
 * @buf: the buffer to store the value of the item
 */
void item_integer_parser(const char *key, uint32_t *buf)
{
        uint32_t tmp;

        if((tmp = item_integer(key, 0)) != -ITEMS_EINT) {
                *buf = tmp;
                dramc_debug("%s found in items, its value is %d\n", key, *buf);
        } else
		{
			if(strncmp(key, "dram.capacity", sizeof("dram.capacity"))==0){
				dramc_debug("auto identify dram size\n");
				//auto_flag = 1;
			}
			dramc_debug("%s not found in items, use default value %d\n", key, *buf);
		}
}

/**
 * the API called by some functions in other files
 *
 * return the size of dram
 */
uint32_t dram_size_check(void)
{
        item_integer_parser("dram.size", &dram.size);   /* just for debug, if dram.size exist in items, use it,
                                                           otherwise use the value calculate by count and capacity
                                                           maybe it is not a good place to put this code here  */
	spl_printf("dram.size %d\n", dram.size);
        return (dram.size << 20);         /* the unit of dram size we used is MB, so we must transfer it to byte */
}
