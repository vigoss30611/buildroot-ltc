#include <dramc_init.h>
#include <asm-arm/arch-coronampw/imapx_base_reg.h>
#include <configs/imap_coronampw.h>
#include <asm-arm/io.h>
#include <preloader.h>
#include <items.h>
#include <linux/bitops.h>
#include <dram_reg_fpga.h>

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

/*1: means 32bit, 2 means 16bit*/
#define MEMC_FREQ_RATIO	1
/*
 * author:tsico
 * aim:for ddr3 2T Timing mode
 * *****************************/
#define MEMC_2T_MODE 0
/*******************************/

/*coronampw IRAM test (8*3K*64bit+8KB=200KB), only test critical data*/
#define IRAM_TEST 1
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
		.rank_sel = 1,
		.reduce_flag = 0,
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
	uint32_t rank[][5] = {
		/* size 128MB 256MB 512MB, size 1GB, size 2GB */
		{1, 1, 1, 1, 1 },        /* total width 16bit */
		{1, 1, 1, 1, 1 },        /* total width 32bit */	//1G 32bit width need verify?
		{3, 3, 3, 3, 3 }         /* total width 64bit */
 	};

	/* type : DDR3, LPDDR2S2, LPDDR2S4  add DDR2*/
	//dram.type = LPDDR2S4;
	dram.type = DDR3;

	/* freq */
	dram.freq = 400;

	/* cl : 2 ~ 8 */
	dram.cl = 6;

	/* count: 2, 4, 8 */
#ifdef	CONFIG_IMAPX_FPGATEST
	dram.count = 2;
#else
	dram.count = 4;
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
#ifdef	CONFIG_IMAPX_FPGATEST
	dram.capacity = 512;
#else
	dram.capacity = 256;
#endif
	dram.capacity = ffs(dram.capacity) - 4;

	/* rtt: 0x4, 0x40, 0x44 */
	if(dram.type == DDR3 || dram.type == DDR2) {
		tmp = 4;
		dram.rtt = ((tmp == 0) ? 0x4 : ((tmp == 1) ? 0x40 : 0x44));
	}

	/* driver, for ddr3: 0x0, 0x2; for lpddr2: 1 ~ 7 */
	dram.driver     = ((dram.type == DDR3 || dram.type == DDR2) ? DRIVER_DIV_6 : DS_40_OHM);     /* set default driver first, ddr3 and lpddr2 respectively */

	/*memory.driver DDR3:  "6", "7"  LPDDR: "34.3", "40", "48", "60", "68.6", "resv", "80", "120"*/
	if(dram.type == DDR3)
		tmp = 6;
	else
		tmp = 0; 
	if(dram.driver == DDR3 || dram.type == DDR2)
		dram.driver = ((tmp == 0) ? 0x0 : 0x2);
	else
		dram.driver = tmp + 1;

	/* dram timing trfc_d and tras */
	dram.timing.trfc_d = 64;
	if(dram.type == DDR3 || dram.type == DDR2)
		dram.timing.tras = ((dram.freq < 410) ? 15 : 20);
	else
		dram.timing.tras = division(42 * dram.freq, 1000);              /* set default tras first, if tras is not found in items, use this default value */

	/* dram addr map, UMCTL2 only support 0 mode: bank.row.column*/
	dram.addr_map_flag = 0;

	/* reduce_flag, burst_len, rank_sel and size, they are caculated by other parameters, not read from items.itm */
	dram.reduce_flag = ((1 << (dram.count + dram.width + 2)) < 32);
	//dram.burst_len = ((dram.type == DDR3 || dram.type == DDR2) ? BURST_8 : BURST_4);
	dram.burst_len =  BURST_8;
	tmp = dram.count + dram.capacity;
	if (tmp < 6)
		tmp = 6;
	dram.rank_sel = rank[dram.count + dram.width - 2 ][tmp - 6];
	dram.size = 1 << (dram.count + dram.capacity + 3);
	//printf("count width capacity: %d, %d %d, size 0x%x\n", dram.count, dram.width, dram.capacity, dram.size);

	return 0;
}

int dramc_set_addr(void)
{
	uint32_t lpddr2s2_row_table[] = { 12, 12, 13, 13, 14, 15, 14, 15 };
	uint32_t lpddr2s4_row_table[] = { 12, 12, 13, 13, 13, 14, 14, 15 };

	if(dram.type == DDR3 || dram.type == DDR2) {
		if(((dram.width != IO_WIDTH8) && (dram.width != IO_WIDTH16)) || (dram.capacity < DRAM_64MB))
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

	/* coronampw ddr configuring
	 * ddr ce2 & A15
	 * start is ce2, if ddr capacity use A15, change this configure LPDDR2 512*16 need row 16
	 */
	if (dram.row == 16)
		writel(0<<7, EMIF_SYSM_ADDR_CE2_A15);

	//printf("rcb: %d %d %d\n", dram.row, dram.col, dram.bank);
	return 0;
}

static const struct infotm_freq_2_param freq_2_param[] =
{
	{ 600, 0x0031 }, { 576, 0x002f }, { 552, 0x002d }, { 528, 0x002b }, { 504, 0x0029 }, { 468, 0x104d },
	{ 444, 0x1049 }, { 420, 0x1045 }, { 396, 0x1041 }, { 372, 0x103d }, { 348, 0x1039 }, { 330, 0x1036 },
	{ 312, 0x1033 }, { 288, 0x102f }, { 264, 0x102b },
};

void dramc_set_freqency_phy(uint32_t pll_p, uint32_t phy_div)
{
	//pll_set_clock("vpll", (pll_p & 0xffff) * M_HZ * 2);  //ddr freq: 533, but vpll need 533*2?
    //irf->pll_set_clock("vpll", 800000000);  //ddr freq: 533, but vpll need 533*2 ---->VPLL change to 768MHZ, DDR 384MHZ
    //irf->module_set_clock("dram", "vpll", 400000000, 0);
	//irf->module_set_clock("apb", "vpll", 100000000, 0);    //apb is 75MHZ, for post rtl sim
        irf->set_pll(PLL_V, 800);
        irf->module_set_clock(DDRPHY_CLK_BASE, PLL_V, 1); //set ddr clk = vpll/4
        irf->module_set_clock(APB_CLK_BASE, PLL_V, 7); //set apb clk = vpll/16
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
	else
		timing->twr = 8;

	/* tcwl */
	if(dram.type == DDR3)
		timing->tcwl = (dram.cl < 7) ? 5 : 6;
	else{
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
	uint32_t timing;
	uint32_t lock_pll;

	timing = 0;
	lock_pll = 0x4;	//Impedance Calibration Done Flag

	/*RTC user GP0 output mode*/
    /*
	if(!dramc_wake_up_flag) {		//RTL no wakeup
		writel(readl(SYS_RTC_GPMODE) & ~0x1,	0x2d009c4c);
		writel(0x1,      0x2d009c7c);
	} else{
		readl(RTC_INFO8);
		dramc_debug("dram wake up  \n");
	}
    */

	/*emif bus reset*/
	/*bus reset need bufore core, or ddr can not write*/
	writel(0xff, EMIF_SYSM_ADDR_BUS_RES);
	writel(0x7, EMIF_SYSM_ADDR_CORE_RES);
#ifndef CONFIG_COMPILE_RTL
    irf->udelay(0x10);
#else
	int i;
    for(i = 0; i<0x100; i++) ;
#endif
	writel(0x1, EMIF_SYSM_ADDR_CORE_RES);
	writel(0x0, EMIF_SYSM_ADDR_BUS_RES);

#ifdef	CONFIG_IMAPX_FPGATEST
	/*DLL reset*/
	writel(0x0, ADDR_PHY_ACDLLCR);
#ifndef CONFIG_COMPILE_RTL
    irf->udelay(0x10);
#else
    for(i = 0; i<0x100; i++) ;
#endif
	writel(0x40000000, ADDR_PHY_ACDLLCR);	//DLL soft reset

	/*wait for pll lock*/
	while((readl(ADDR_PHY_PGSR) & lock_pll) != lock_pll)
#ifndef CONFIG_COMPILE_RTL
    irf->udelay(0x10);
#else
    for(i = 0; i<0x100; i++) ;
#endif

#ifdef	CONFIG_IMAPX_FPGATEST
	if (dram.type == DDR2 || dram.type == DDR3)
		//timing = 0x22222222;	//for x9 old fpga
		timing = 0x11111111;	//for x9 corona new fpga
	else if (dram.type == LPDDR3)		//LPDDR2S2 ??
		timing = 0x33333333;
#endif
	writel(timing, ADDR_PHY_DX0DQTR);
	writel(timing, ADDR_PHY_DX1DQTR);
	writel(timing, ADDR_PHY_DX2DQTR);
	writel(timing, ADDR_PHY_DX3DQTR);
#endif


}

/**
 * Program the DWC_ddr_umctl2 registers
 * Ensure that DFIMISC.dfi_init_complete_en is set to 0
 */
void DWC_umctl2_reg(void)
{
	uint32_t tmp = 0;
	struct infotm_dram_timing *timing = &dram.timing;
	uint32_t cl = dram.cl;

	writel(0x0, ADDR_UMCTL2_DFIMISC);

	/*cfg MSTR reg*/
#if MEMC_2T_MODE
	tmp = (((dram.rank_sel & 0x3) << 24) | ((dram.burst_len & 0xf) << 16) | ((dram.reduce_flag & 0x3) << 12) | (1 << 10)
		| ((dram.type == LPDDR3) << 3) | ((dram.type == LPDDR2S4 || dram.type ==LPDDR2S2) << 2) |((dram.type == DDR3) << 0));
	writel(tmp, ADDR_UMCTL2_MSTR);  //0x3040001
#else
	tmp = (((dram.rank_sel & 0x3) << 24) | ((dram.burst_len & 0xf) << 16) | ((dram.reduce_flag & 0x3) << 12)
		| ((dram.type == LPDDR3) << 3) | ((dram.type == LPDDR2S4 || dram.type ==LPDDR2S2) << 2) |((dram.type == DDR3) << 0));
	writel(tmp, ADDR_UMCTL2_MSTR);	//0x3040001
#endif

	tmp =(((timing->trefi & 0xfff) << 16) | (timing->trfc & 0xff));	//0x610040 alin define 0x00620070
#ifdef	CONFIG_IMAPX_FPGATEST
	writel(0x00080030/MEMC_FREQ_RATIO, ADDR_UMCTL2_RFSHTMG);
#else
	writel(tmp/MEMC_FREQ_RATIO, ADDR_UMCTL2_RFSHTMG);
#endif

#ifdef CONFIG_COMPILE_RTL
		writel((dram.type == DDR3) ? 0x00010001/MEMC_FREQ_RATIO : 0x00010001/MEMC_FREQ_RATIO, ADDR_UMCTL2_INIT0); /* time period(us) during memory power up, DDR3 at least 500us*/
#else
		writel((dram.type == DDR3) ? 0x00010150/MEMC_FREQ_RATIO : 0x000100a0/MEMC_FREQ_RATIO, ADDR_UMCTL2_INIT0); /* time period(us) during memory power up, DDR3 at least 500us*/
#endif

#ifdef CONFIG_COMPILE_RTL
		writel((dram.type == DDR3) ? 0x00020101 : 0x00010101, ADDR_UMCTL2_INIT1);         /* time period(us) during ddr3 power up, at least 200us */
#else
		writel((dram.type == DDR3) ? 0x00f0010f : 0x01000208, ADDR_UMCTL2_INIT1);         /* time period(us) during ddr3 power up, at least 200us */
#endif

	if(dram.type != DDR3)
		writel(0xc06, ADDR_UMCTL2_INIT2);

	if(dram.type == DDR3){
		tmp = ((cl - 4) << (4+16)) | ((timing->twr - 4) << (9+16) | (1 << (8+16)) | (dram.rtt & 0xff) | dram.driver);	/*MR0 & MR1*/ //0x4200004, alin define 0x5200004
		writel(tmp, ADDR_UMCTL2_INIT3);
		tmp = ((timing->tcwl - 5) << (3+16));
		writel(tmp, ADDR_UMCTL2_INIT4);  					/*MR2 & MR3*/
	}else{		//LPDDR2 user
		tmp = ((timing->twr - 2) << (5+16)) | 0x3<<16 | (cl -2);	//BL=8 fixed
		writel(tmp, ADDR_UMCTL2_INIT3);
		tmp = (dram.driver << 16);
		writel(tmp, ADDR_UMCTL2_INIT4);  					/*MR2 & MR3*/
	}

	tmp = 0x11 << 16 | (dram.type == DDR3 ? 1 : division((dram.freq * 10 +1023), 1024));		//tmp 0x100001, alin define 0x00110001
	writel(tmp, ADDR_UMCTL2_INIT5);

	writel((MEMC_FREQ_RATIO == 2) ? 0x000366 : 0x00066b, ADDR_UMCTL2_RANKCTL);

	/*need use cl*/
	tmp = (timing->tcwl + 4 + division(15 * dram.freq, 1000)) << 24
		| ((dram.bank - 2)? 0x10:0) << 16 | (timing->tras + 10)<< 8 | timing->tras;	//0xf10190f alin 0x0f101a0f
	writel(tmp/MEMC_FREQ_RATIO, ADDR_UMCTL2_DRAMTMG0);

	tmp = 0x8 << 16 | 0x4 << 8 | (timing->trc >> 1);	//8040a alin 0x00080416
	writel(tmp/MEMC_FREQ_RATIO, ADDR_UMCTL2_DRAMTMG1);

	tmp = timing->tcwl << 24 | cl << 16 | (cl + 6 -timing->tcwl + (dram.type == DDR3 ? 0 : 3)) << 8 | (timing->tcwl + 4 + 4);	//set twrt cl?=rl wl 0x506070d alin 0x0506070d
	writel(tmp/MEMC_FREQ_RATIO, ADDR_UMCTL2_DRAMTMG2);

	tmp = (dram.type == DDR3 ? 0:5) << 20 | 0x4/MEMC_FREQ_RATIO << 12 | (dram.type == DDR3 ? 0xd/MEMC_FREQ_RATIO : 0);	//set tmdr tmod 0x400d alin 0x04010
	writel(tmp,    ADDR_UMCTL2_DRAMTMG3);

	tmp = timing->trcd << 24 | 0x4 << 16 | timing->trrd << 8 | timing->trp;	//t_rcd, t_ccd, t_rrd, t_rp 0x6040406 alin 0x06040406
	writel(tmp/MEMC_FREQ_RATIO + (MEMC_FREQ_RATIO == 2 ? 1 : 0), ADDR_UMCTL2_DRAMTMG4);

	if (dram.type == DDR3)
		tmp = 5 << 24 | division(10 * dram.freq, 1000)  << 16 |  0x4 << 8 | 0x3 ;	//0x5050403 alin 0x5040403
	else
		tmp = 0x2020503;
	writel(tmp/MEMC_FREQ_RATIO,  ADDR_UMCTL2_DRAMTMG5);

	writel((dram.type == DDR3) ? 0:0x2020005/MEMC_FREQ_RATIO,  ADDR_UMCTL2_DRAMTMG6);
	writel((dram.type == DDR3) ? 0:202/MEMC_FREQ_RATIO, 	ADDR_UMCTL2_DRAMTMG7);
	writel(0x3c/MEMC_FREQ_RATIO, 	ADDR_UMCTL2_DRAMTMG8);

	writel((dram.type == DDR3) ? (MEMC_FREQ_RATIO == 2 ? 0x1000020 : 0x1000040) : (MEMC_FREQ_RATIO == 2 ? 0x403c000f : 0x4078001E), 	ADDR_UMCTL2_ZQCTL0);
	writel((dram.type == DDR3) ? (MEMC_FREQ_RATIO == 2 ? 0x1000100 : 0x2000100) : (MEMC_FREQ_RATIO == 2 ? 0x0900070: 0x1100070), 	ADDR_UMCTL2_ZQCTL1);

	tmp = 0x02000100 | (cl - 2) << 16 | (timing->tcwl - 1 + (dram.type == DDR3 ? 0:1) );	//0x2040104 alin 0x02040104, need test, ddr3: cwl -1,lpddr2: cwl
	writel(tmp, 	ADDR_UMCTL2_DFITMG0);

	writel(0x30202, 	ADDR_UMCTL2_DFITMG1);

	writel((dram.type == DDR3) ? 0x07012000 : 0x07C10021, 	ADDR_UMCTL2_DFILPCFG0);
	writel(0x40003, 	ADDR_UMCTL2_DFIUPD0);
	writel(0x00c300f0,	ADDR_UMCTL2_DFIUPD1);
	writel(0x866c006c, 	ADDR_UMCTL2_DFIUPD2);
	writel(0x0f76046a, 	ADDR_UMCTL2_DFIUPD3);

	if(dram.col == 11 && dram.type == LPDDR2S4){
		/*addr map(row14, bank8, col11)	ADDR_UMCTL2_ADDRMAP0 ~ ADDR_UMCTL2_ADDRMAP6*/
		writel((0x1f00 | (dram.row + 0x7)), ADDR_UMCTL2_ADDRMAP0);	//rank defined
		writel(0x070707 | (!dram.reduce_flag ? 0x010101:0),     ADDR_UMCTL2_ADDRMAP1);
		if(!dram.reduce_flag){
			writel(0, 	ADDR_UMCTL2_ADDRMAP2);
			writel(0x00000000, 	ADDR_UMCTL2_ADDRMAP3);
		}
		else{
			writel(0x00000000,      ADDR_UMCTL2_ADDRMAP2);
			writel(0x0f000000, 	ADDR_UMCTL2_ADDRMAP3);
		}
		writel(0x0f0f, 	ADDR_UMCTL2_ADDRMAP4);			//col bit12 bit13 not used
		writel((!dram.reduce_flag ? 0x07070707:0x06060606), ADDR_UMCTL2_ADDRMAP5);
		if(dram.row == 12)
			writel(0x0f0f0f0f, ADDR_UMCTL2_ADDRMAP6);
		if(dram.row == 13)
			writel((!dram.reduce_flag ? 0x0f0f0f07:0x0f0f0f06), ADDR_UMCTL2_ADDRMAP6);
		if(dram.row == 14)
			writel((!dram.reduce_flag ? 0x0f0f0707:0x0f0f0606), ADDR_UMCTL2_ADDRMAP6);	/*X9_EVB: full mode, bank 0 in HIF bit[26]: (0x14 + offset:0x6) */
		if(dram.row == 15)
			writel((!dram.reduce_flag ? 0x0f070707:0x0f060606), ADDR_UMCTL2_ADDRMAP6);	/*X9_EVB: full mode, bank 0 in HIF bit[27]: (0x15 + offset:0x6) */
		if(dram.row == 16)
			writel((!dram.reduce_flag ? 0x07070707:0x06060606), ADDR_UMCTL2_ADDRMAP6);	/*X9_EVB: full mode, bank 0 in HIF bit[28]: (0x16 + offset:0x6) */
	} else {
		/*addr map(row14, bank8, col10)	ADDR_UMCTL2_ADDRMAP0 ~ ADDR_UMCTL2_ADDRMAP6*/
		writel((0x1f00 | (dram.row + 0x6)), ADDR_UMCTL2_ADDRMAP0);	//rank defined
		writel(0x060606 | (!dram.reduce_flag ? 0x010101:0), 	ADDR_UMCTL2_ADDRMAP1);		//bank 0~2 always in HIF bit[9~11]
		if(!dram.reduce_flag){
			writel(0, 	ADDR_UMCTL2_ADDRMAP2);
			writel(0x0f000000, 	ADDR_UMCTL2_ADDRMAP3);
		}
		else{
			writel(0x00000000,      ADDR_UMCTL2_ADDRMAP2);
			writel(0x0f0f0000, 	ADDR_UMCTL2_ADDRMAP3);
		}
		writel(0x0f0f, 	ADDR_UMCTL2_ADDRMAP4);			//col bit12 bit13 not used
		writel((!dram.reduce_flag ? 0x06060606:0x05050505), ADDR_UMCTL2_ADDRMAP5);
		if(dram.row == 12)
			writel(0x0f0f0f0f, ADDR_UMCTL2_ADDRMAP6);
		if(dram.row == 13)
			writel((!dram.reduce_flag ? 0x0f0f0f06:0x0f0f0f05), ADDR_UMCTL2_ADDRMAP6);
		if(dram.row == 14)
			writel((!dram.reduce_flag ? 0x0f0f0606:0x0f0f0505), ADDR_UMCTL2_ADDRMAP6);	/*X9_EVB: full mode, bank 0 in HIF bit[26]: (0x14 + offset:0x6) */
		if(dram.row == 15)
			writel((!dram.reduce_flag ? 0x0f060606:0x0f050505), ADDR_UMCTL2_ADDRMAP6);	/*X9_EVB: full mode, bank 0 in HIF bit[27]: (0x15 + offset:0x6) */
		if(dram.row == 16)
			writel((!dram.reduce_flag ? 0x06060606:0x05050505), ADDR_UMCTL2_ADDRMAP6);	/*X9_EVB: full mode, bank 0 in HIF bit[28]: (0x16 + offset:0x6) */
	}

	tmp = (0x04000400 | (cl - timing->tcwl) << 2);		//0x04000404, alin 0x04000404;
	writel(tmp, ADDR_UMCTL2_ODTCFG);

	/*DEL ODT, power consumption*/
	//writel(0x0022 | ((dram.rank_sel > 1) ? 0x1100:0), ADDR_UMCTL2_ODTMAP);		//rank otd config
	writel(0, ADDR_UMCTL2_ODTMAP);		//rank otd config

	writel(0x7fc80f01, ADDR_UMCTL2_SCHED);
	if(MEMC_FREQ_RATIO == 1)
		writel(0xffff00c8, ADDR_UMCTL2_PERFHPR0);
	writel(0x40000c8, ADDR_UMCTL2_PERFHPR1);
	if(MEMC_FREQ_RATIO == 1)
		writel(0xffff00ff, ADDR_UMCTL2_PERFLPR0);
	writel(0x40000c9, ADDR_UMCTL2_PERFLPR1);
	if(MEMC_FREQ_RATIO == 1)
		writel(0xffff00c9, ADDR_UMCTL2_PERFWR0);
	writel(0x40000ca, ADDR_UMCTL2_PERFWR1);

	writel(0x12000000, ADDR_UMCTL2_DBG0);
	writel(0xff000000, ADDR_UMCTL2_DBG1);

	writel(0xff000000, ADDR_UMCTL2_PCCFG);
	writel(0x0f00c083, ADDR_UMCTL2_PCFGR_0);
	writel(0x1f004083, ADDR_UMCTL2_PCFGW_0);
	writel(0xff00c082, ADDR_UMCTL2_PCFGR_1);
	writel(0xff004082, ADDR_UMCTL2_PCFGW_1);
	writel(0xff004081, ADDR_UMCTL2_PCFGR_2);
	writel(0xff004081, ADDR_UMCTL2_PCFGW_2);
	writel(0x0f004084, ADDR_UMCTL2_PCFGR_3);
	writel(0x1f004084, ADDR_UMCTL2_PCFGW_3);
	writel(0x0f004085, ADDR_UMCTL2_PCFGR_4);
	writel(0x1f004085, ADDR_UMCTL2_PCFGW_4);
	writel(0x0f004086, ADDR_UMCTL2_PCFGR_5);
	writel(0x1f004086, ADDR_UMCTL2_PCFGW_5);
}

void ddr_phy_set(void)
{
	uint32_t cl = dram.cl;
	struct infotm_dram_timing *timing = &dram.timing;
	int tmp = 0;

	if(dram.reduce_flag)
	{
		writel(0x1B6CCE80, ADDR_PHY_DX2GCR);
		writel(0x1B6CCE80, ADDR_PHY_DX3GCR);
	}

	tmp = 0x01802E02 | (dram.rank_sel << 18) | (dram.type == DDR3 ? 0x4 : 0);	//0x18c2e06 alin 0x18c2e06
	writel(tmp, ADDR_PHY_PGCR);
#if MEMC_2T_MODE
	writel((0xb + (dram.type == DDR3?0:1)) | (dram.type == LPDDR2S2 ? (0x1<<8):0) | (1 << 28),     ADDR_PHY_DCR);          /* DDR8BNK, DDRMD(DDR3) */
#else
	writel((0xb + (dram.type == DDR3?0:1)) | (dram.type == LPDDR2S2 ? (0x1<<8):0),     ADDR_PHY_DCR);/* DDR8BNK,DDRMD(DDR3) */
#endif
	tmp = ((cl - 4) << 4) | ((timing->twr - 4) << 9);
	writel(tmp,  ADDR_PHY_MR0);          /* CAS latency, write recovery(use twr) , LPDDR2 not use*/
	if(dram.type == DDR3){
		writel((dram.rtt | dram.driver),	            ADDR_PHY_MR1);          /* RTT, DIC */
		writel((timing->tcwl - 5) << 3, 	            ADDR_PHY_MR2);          /* CWL(CAS write latency, use tcwl) */
		writel(0x0,     				    ADDR_PHY_MR3);
	} else {		//LPDDR2 user
		writel(0x3 |((timing->twr - 2) << 5), ADDR_PHY_MR1);
		writel(cl -2, ADDR_PHY_MR2);
		writel(dram.driver,    ADDR_PHY_MR3);
	}
	tmp = 0x3 | (4 << 2) | (4 << 5) | (cl << 8) | (timing->trcd << 12) | (timing->tras << 16) | (timing->trrd << 21) | (timing->trc << 25); //0x2a8f6690 alin 0x2c906693-->tras=15
	writel(tmp, ADDR_PHY_DTPR0);

	tmp = (timing->tfaw << 3) | (timing->trfc << 16 | (dram.type == DDR3 ? 0: (0x11<<24)));	//0x4000a0 alin 0x2a2c6080
#ifdef	CONFIG_IMAPX_FPGATEST
	writel(0x09220050,  ADDR_PHY_DTPR1);        /* tFAW, tRFC */
#else
	writel(tmp,  ADDR_PHY_DTPR1);        /* tFAW, tRFC */
#endif
	//writel(0x10022c00,                                  ADDR_PHY_DTPR2);
	writel(0x1001b00c,                                  ADDR_PHY_DTPR2);

	tmp = (0x1b << 0) | (0x14 << 6) | (0x8 << 18); /* DLL soft reset time, DLL lock time, ITM soft reset time */ //0x20051b alin 0x0020051b
	writel(tmp,				    ADDR_PHY_PTR0); /* DLL soft reset time, DLL lock time, ITM soft reset time */
	if(dram.type == LPDDR2S4 || dram.type == LPDDR2S2)
		writel(0x00000c40, ADDR_PHY_DXCCR);
	writel(0xfa00001f | (dram.type == DDR3 ? 0: (0x2<<5 | 0x2<<8)),                                      ADDR_PHY_DSGCR);
	while((readl(ADDR_PHY_PGSR) & 0x3) != 0x3);	/* wait for init done and dll lock done */

	if(dramc_wake_up_flag) {							//RTL no wakeup
	//if(0) {
#ifndef	CONFIG_IMAPX_FPGATEST
		writel(0x40000001,      ADDR_PHY_PIR);          /* ddr system init */
		while((readl(ADDR_PHY_PGSR) & 0x1) == 0x1);     /* wait for init done */
		writel(0x1000014a,      ADDR_PHY_ZQ0CR0);       /* impendance control and calibration */
//		writel(readl(SYS_RTC_GPMODE) & ~0x1,             0x2d009c4c);
		writel(0x1,             0x2d009c7c);
		writel(0x0000014a,      ADDR_PHY_ZQ0CR0);
		writel(0x00000009,      ADDR_PHY_PIR);          /* impendance calibrate */
		while((readl(ADDR_PHY_PGSR) & 0x5) == 0x5);     /* wait for init done and impendance calibration done */
#else
		writel(0x00040001, ADDR_PHY_PIR);
		while((readl(ADDR_PHY_PGSR) & 0x3) != 0x3);	/* wait for init done and dll lock done */
#endif
	}else{
#ifndef	CONFIG_IMAPX_FPGATEST
        #if 0
		writel(0x0000000f, ADDR_PHY_PIR);
		while((readl(ADDR_PHY_PGSR) & 0x7) != 0x7);	/* wait for init done and dll lock done */
        #else
		writel(0x00000003, ADDR_PHY_PIR);
		while((readl(ADDR_PHY_PGSR) & 0x3) != 0x3);	/* wait for init done and dll lock done */
        #endif
#else
		writel(0x00040001, ADDR_PHY_PIR);
		while((readl(ADDR_PHY_PGSR) & 0x3) != 0x3);	/* wait for init done and dll lock done */
#endif
	}

	/*Start PHY initialization*/
	writel(0x0000011,       ADDR_PHY_PIR);
	while((readl(ADDR_PHY_PGSR) & 0x3) != 0x3);	/* wait for init done and dll lock done */
#ifndef	CONFIG_IMAPX_FPGATEST
	writel(0x00040001, ADDR_PHY_PIR);
#endif
	if(MEMC_FREQ_RATIO == 2)
		writel(0x0,				ADDR_UMCTL2_SWCTL);
	writel(0x1, 		ADDR_UMCTL2_DFIMISC);
	if(MEMC_FREQ_RATIO == 2){
		writel(0x1,				ADDR_UMCTL2_SWCTL);
		while((readl(ADDR_UMCTL2_SWSTAT) & 0x1) != 0x1);
	}
	/*Wait for DWC_ddr_umctl2 to move to “normal”*/
	while((readl(ADDR_UMCTL2_STAT) & 0x3) != 0x1);
}

void dis_auto_fresh(void)
{
	writel(0x1, 		ADDR_UMCTL2_RFSHCTL3);
	writel(0x0, 		ADDR_UMCTL2_PWRCTL);
	writel(0x0, 		ADDR_UMCTL2_DFIMISC);
}

int ddr_training(void)
{

	//if(dramc_wake_up_flag && dram.rank_sel == 3)	//RTL no wakeup
#ifndef	CONFIG_IMAPX_FPGATEST
	int i;
	if(0 && dram.rank_sel == 3)
		writel(0x01882E06, ADDR_PHY_PGCR);
	writel(DQS_ADDR+0x1000, 	ADDR_PHY_DTAR); 	/*data training address, column, row and bank address*/
	writel(0x181, 		ADDR_PHY_PIR);		/*PHY Initialization: Read DQS Training & Read Valid Training*/
	while((readl(ADDR_PHY_PGSR) & 0x10) != 0x10);     /* wait for Data Training Done */
	dramc_debug("ADDR_PHY_PGSR = 0x%x\n", readl(ADDR_PHY_PGSR));  /*Read General Status Register*/
	if(readl(ADDR_PHY_PGSR) & 0x20) {       /* test whether DQS gate training is error */
		dramc_debug("DQS Gate Training Error\n");
		/*DQS err than judge which bit is err*/
		for(i=0; i<(dram.reduce_flag ? 2 : 4); i++)
		{
			if(readl(ADDR_PHY_DXGSR0(i)) & 0xf0)
				dramc_debug("dram train byte%d error \n", i);
			else
				dramc_debug("dram train byte%d Ok \n", i);
		}
		return 1;
	}
#if 1
	if((readl(ADDR_PHY_PGSR) & 0x100) == 0x100) {       /* test whether dramc Read Valid Training is error */
		dramc_debug("Read Valid Training Error\n");
		return 1;
	}
#endif
#endif

	writel(0x0, 		ADDR_UMCTL2_RFSHCTL3);
	return 0;
}

/***************************************
 * V0.1 2015.4.7   q3e version test
 * V0.1 2015.4.24  q3e support 32bit & 16bit
 *
 *
 *
 *************************************/
int dramc_init_memory(int suspend_resume)
{
//	printf("DDR version V0.1, %s\n", __TIME__);
	//irf->module_enable(EMIF_SYSM_ADDR);
	/*before: emif & dll module reset*/
	dramc_wake_up_flag = suspend_resume;

	emif_module_reset();

	/*step1: Program the DWC_ddr_umctl2 registers*/
	DWC_umctl2_reg();

	/*step2: Deassert soft reset signal RST.soft_rstb, reset_dut*/
	writel(0x0, EMIF_SYSM_ADDR_CORE_RES);

	/*step3 & step4:Start PHY initialization by accessing relevant*/
	ddr_phy_set();

	/*step4: Disable auto-refreshes and powerdown by setting*/
	dis_auto_fresh();

	/*least all: Perform PUBL training as required*/
	if(ddr_training())
		return 1;
	
/*	if(dram.type == LPDDR2S4){
		writel(0x33445577, 0x46000000);
		readl(0x46000000);
		writel(0x11223344, 0x49000000);
		readl(0x49000000);
		writel(0x22334455, 0x59000000);
		readl(0x59000000);
		writel(0x33445566, 0x69000000);
		readl(0x69000000);
		writel(0x22334455, 0x79000000);
		readl(0x79000000);
		writel(0x12345678, 0x89000000);
		readl(0x89000000);
		writel(0x23456789, 0x99000000);
		readl(0x99000000);
		writel(0x3456789a, 0xa9000000);
		readl(0xa9000000);
		writel(0x456789ab, 0xb9000000);
		readl(0xb9000000);
	}*/
	/*ddr test*/
#if 0		
	//#define DDR_CMP_SIZE (511*0x100000/4)	
	#define DDR_TEST_START 0x44000000	
	int t1 = 0, DDR_CMP_SIZE=0;
	DDR_CMP_SIZE = dram.size;
	dramc_debug("FPGA . write data : 1~0x%x from addr 0x44000000\n", DDR_CMP_SIZE);	
	for(t1 = 0; t1 < DDR_CMP_SIZE; t1++)	
	{	
		writel(t1, (DDR_TEST_START + (t1<<2)));	
	}	
	dramc_debug("FPGA . read data : 1~0x%x from addr 0x44000000\n", DDR_CMP_SIZE);	
	for(t1 = 0; t1 < DDR_CMP_SIZE; t1++)	
	{	
		if(t1 != readl(DDR_TEST_START + (t1<<2)))	
		{
			dramc_debug("FPGA . cmp original data 0x%x and  get value 0x%x @ addr 0x %x fail! the result is err!\n",
				   	t1, *(volatile unsigned int *)(DDR_TEST_START + (t1<<2)), (t1<<2));	
			while(1);
		}
 	}
//#else	
	//if(dram.type == LPDDR2S4){
	if(dram.type == DDR3){
		dramc_debug("RTL . (LP)DDR%d read data and write data test\n",dram.type);	
			writel(0x00000042, 0x42000000);
			readl(0x42000000);
			writel(0x00000044, 0x44000000);
			readl(0x44000000);
			writel(0x00000047, 0x47000000);
			readl(0x47000000);
			writel(0x00000052, 0x52000000);
			readl(0x52000000);
			writel(0x00000054, 0x54000000);
			readl(0x54000000);
			writel(0x00000057, 0x57000000);
			readl(0x57000000);
			writel(0x0000005a, 0x5a000000);
			readl(0x5a000000);
			writel(0x0000005c, 0x5c000000);
			readl(0x5c000000);
			writel(0x0000005f, 0x5f000000);
			readl(0x5f000000);
			writel(0x00000062, 0x62000000);
			readl(0x62000000);
			writel(0x00000064, 0x64000000);
			readl(0x64000000);
			writel(0x00000067, 0x67000000);
			readl(0x67000000);
			writel(0x0000006a, 0x6a000000);
			readl(0x6a000000);
			writel(0x0000006c, 0x6c000000);
			readl(0x6c000000);
			writel(0x0000006f, 0x6f000000);
			readl(0x6f000000);
			writel(0x00000072, 0x72000000);
			readl(0x72000000);
			writel(0x00000074, 0x74000000);
			readl(0x74000000);
			writel(0x00000077, 0x77000000);
			readl(0x77000000);
			writel(0x0000007a, 0x7a000000);
			readl(0x7a000000);
			writel(0x0000007c, 0x7c000000);
			readl(0x7c000000);
			writel(0x0000007f, 0x7f000000);
			readl(0x7f000000);
			writel(0x00000082, 0x82000000);
			readl(0x82000000);
			readl(0x84000000);
			readl(0x87000000);
			readl(0x8a000000);
			readl(0x8c000000);
			readl(0x8f000000);
			writel(0x00000092, 0x92000000);
			readl(0x92000000);
			readl(0x94000000);
			readl(0x97000000);
			readl(0x9a000000);
			readl(0x9c000000);
			readl(0x9f000000);
			writel(0x000000a2, 0xa2000000);
			readl(0xa2000000);
			readl(0xa4000000);
			readl(0xa7000000);
			readl(0xaa000000);
			readl(0xac000000);
			readl(0xaf000000);
			writel(0x000000b2, 0xb2000000);
			readl(0xb2000000);
			readl(0xb4000000);
			readl(0xb7000000);
			readl(0xba000000);
			readl(0xbc000000);
			readl(0xbf000000);
	}
#endif

	return 0;
}

int dramc_verify_size(void)
{
	unsigned int checkaddr1=0, checkaddr2=0,checkaddr3=0;

	checkaddr1 = 0x40000000;
	checkaddr2 = checkaddr1 + (dram.size << 19);
	checkaddr3 = ((checkaddr1 >> 1) + (checkaddr2 >> 1));
	dramc_debug("checkaddr1 = 0x%x, checkaddr2 = 0x%x, checkaddr3 = 0x%x\n", checkaddr1, checkaddr2, checkaddr3);
	writel(0x87654321, checkaddr2);
	writel(0x12345678, checkaddr1);
	writel(0x87654321, checkaddr3);

#if IRAM_TEST
	int i;
	int test, test1;
	writel(0x12345678, 0x3c000000);
	writel(0x23456789, 0x3c005ffc);
	writel(0x23456789, 0x3c006000);
	writel(0x3456789a, 0x3c00bffc);
	writel(0x3456789a, 0x3c00c000);
	writel(0x456789ab, 0x3c011ffc);
	writel(0x456789ab, 0x3c012000);
	writel(0x56789abc, 0x3c017ffc);
	writel(0x56789abc, 0x3c018000);
	writel(0x6789abcd, 0x3c01effc);
	writel(0x6789abcd, 0x3c01f000);
	writel(0x789abcde, 0x3c023ffc);
	writel(0x789abcde, 0x3c024000);
	writel(0x89abcdef, 0x3c029ffc);
	writel(0x89abcdef, 0x3c02a000);
	writel(0x9abcdef0, 0x3c02fffc);
	writel(0x9abcdef0, 0x3c030000);
	writel(0xabcdef01, 0x3c031ffc);
	readl(0x3c000000);
	for(i=1; i<=8; i++){
		readl(0x3c000000 + i*0x6000 -0x4);
		readl(0x3c000000 + i*0x6000);
	}
	readl(0x3c031ff8);

	for(i=0; i<=7; i++){
		test = 0x55555555 + 0x11111111*i;
		test1 = 0x3c005fffd + i;
		writel(test, test1);
		test = 0x3c005fffd + i;
		readl(test);
	}

	for(i=0; i<=15; i++){
		test = 0x0 + 0x11*i;
		test1 = 0x3c005fff7 + i;
		writeb(test, test1);
		test = 0x3c005fff7 + i;
		readb(test);
	}

#endif
	/*1GB 32bit 0x40000000--->0x80000000*/
	if(dram.size == 1024 && dram.rank_sel == 1){
		dramc_debug("capacity 1024MB need cs0 remap cs1\n");
		writel(0x1, EMIF_SYSM_ADDR_MAP_REG);
	}
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



