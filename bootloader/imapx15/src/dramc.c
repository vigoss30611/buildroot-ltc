#include <dramc_init.h>
#include <asm-arm/io.h>
#include <preloader.h>
#include <items.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>
#include <asm-arm/arch-apollo3/imapx_dramc.h>
#include <configs/imapx800.h>
#include <linux/bitops.h>

//#define DRAMC_DEBUG
#ifdef DRAMC_DEBUG
#define dramc_debug(x...) spl_printf("dramc---" x)
#else
#define dramc_debug(x...)
#endif

#define strcmp(x, y) irf->strcmp(x, y)
#define strncmp(x, y, z) irf->strncmp(x, y, z)
#define DQS_ADDR        0x7FFFE000
#define CRC_MAGIC       0x5a6a7a99

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
        },
};

/* describe whether it is a wake up process */
static volatile int dramc_wake_up_flag = 0;
static volatile int auto_flag = 0;

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

char *dram_item_string_parser[] = {
        "-t", "DDR3", "LPDDR2S2", "LPDDR2S4", "DDR2",
        "-r", "4", "2", "6",
        "-d", "6", "7",
        "-l", "resv", "34.3", "40", "48", "60", "68.6", "resv", "80", "120",
        "-a", "bank.row.column", "row.bank.column",
};
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
/**
 * parse items which have string values
 * use the value if found in items.itm, use default if not found
 * @q: the symbol of one item, such as "-t" for "type"
 * @key: the name of the item
 * @buf: the buffer to store the value of the item
 */
void item_string_parser(const char *q, const char *key, uint32_t *buf)
{
        char str[64], **p = dram_item_string_parser;
        int ret = -2, cnt = 0, i;

        item_string(str, key, 0);

        for(i = 0; i < ARRAY_SIZE(dram_item_string_parser); i++) {
                if(ret == -1) {
                        if(*p[i] == '-')
                                break;

                        if(strncmp(str, p[i], 10) == 0) {
                                *buf = cnt;
                                dramc_debug("%s found in items, the value is %s\n", key, str);
                                return;
                        }
                        cnt++;
                }

                if(strncmp(q, p[i], 10) == 0)
                        ret = -1;
        }
        dramc_debug("%s not found in items, use its default value %d\n", key, *buf);
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
				auto_flag = 1;
			}
			dramc_debug("%s not found in items, use default value %d\n", key, *buf);
		}
}

/**
 * get items from items.itm
 *
 * return 0 if items configuration succeed, return 1 otherwise
 */
int dramc_get_items(void)
{
        uint32_t tmp;
        uint32_t rank[][3] = {
            /* size 512MB, size 1GB, size 2GB */
            { 1, 1, 1 },        /* total width 16bit */
            { 1, 2, 1 },        /* total width 32bit */
            { 3, 3, 3 }         /* total width 64bit */
        };

        /* type : DDR3, LPDDR2S2, LPDDR2S4 */
        item_string_parser("-t", "dram.type", &dram.type);

        /* freq */
        item_integer_parser("dram.freq", &dram.freq);

        /* cl : 2 ~ 8 */
        item_integer_parser("memory.cl", &dram.cl);

        /* count: 2, 4, 8 */
        item_integer_parser("dram.count", &dram.count);
        dram.count = ffs(dram.count) - 1;                       /* we set the registers using its ffs value */

        /* width: 8, 16, 32 */
        item_integer_parser("dram.width", &dram.width);
        dram.width = ffs(dram.width) - 3;
        if((dram.count + dram.width > 4) || (dram.count + dram.width < 2)) {               /* the total width must in the range of 16bit to 64bit */
                dramc_debug("the total dram width is larger than 64bit, dram configuration error!\n");
                return 1;
        }

        /* capacity: 8MB, 16MB, 32MB, 64MB, 128MB, 256MB, 512MB, 1024MB */
        item_integer_parser("dram.capacity", &dram.capacity);
        dram.capacity = ffs(dram.capacity) - 4;

        /* rtt: 0x4, 0x40, 0x44 */
        if(dram.type == DDR3) {
                item_string_parser("-r", "memory.rtt", &tmp);
                dram.rtt = ((tmp == 0) ? 0x4 : ((tmp == 1) ? 0x40 : 0x44));
        }

        /* driver, for ddr3: 0x0, 0x2; for lpddr2: 1 ~ 7 */
        dram.driver     = ((dram.type == DDR3) ? DRIVER_DIV_6 : DS_40_OHM);     /* set default driver first, ddr3 and lpddr2 respectively */
        item_string_parser(((dram.type == DDR3)? "-d": "-l"), "memory.driver", &tmp);
        if(dram.driver == DDR3)
                dram.driver = ((tmp == 0) ? 0x0 : 0x2);
        else
                dram.driver = tmp + 1;

        /* dram timing trfc_d and tras */
        item_integer_parser("memory.trfc", &dram.timing.trfc_d);

        if(dram.type == DDR3)
                dram.timing.tras = ((dram.freq < 410) ? 15 : 20);
        else
                dram.timing.tras = division(42 * dram.freq, 1000);              /* set default tras first, if tras is not found in items, use this default value */
        item_integer_parser("memory.tras", &dram.timing.tras);

        /* dram addr map */
        item_string_parser("-a", "memory.highres", &dram.addr_map_flag);

        /* reduce_flag, burst_len, rank_sel and size, they are caculated by other parameters, not read from items.itm */
        dram.reduce_flag = ((1 << (dram.count + dram.width + 2)) < 32);
        dram.burst_len = ((dram.type == DDR3) ? BURST_8 : BURST_4);
        if(IROM_IDENTITY == IX_CPUID_X15)       /* imapx15 */
                dram.rank_sel = rank[dram.count + dram.width - 2 ][dram.count + dram.capacity - 6];
        else                                    /* imapx820 */
                dram.rank_sel = 1;
        dram.size = 1 << (dram.count + dram.capacity + 3);

        return 0;
}

/**
 * dram addr(bank, row, col) setting
 *
 * return 0 when initialize successfully, return 1 otherwise
 */
int dramc_set_addr(void)
{
        uint32_t lpddr2s2_row_table[] = { 12, 12, 13, 13, 14, 15, 14, 15 };
        uint32_t lpddr2s4_row_table[] = { 12, 12, 13, 13, 13, 14, 14, 15 };

        if(dram.type == DDR3) {
                if(((dram.width != IO_WIDTH8) && (dram.width != IO_WIDTH16)) || (dram.capacity < DRAM_64MB))
                        return 1;
                if(dram.capacity == DRAM_1GB) {
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

static const struct infotm_freq_2_param freq_2_param[] = {
        { 600, 0x0031 }, { 576, 0x002f }, { 552, 0x002d }, { 528, 0x002b }, { 504, 0x0029 }, { 468, 0x104d },
        { 444, 0x1049 }, { 420, 0x1045 }, { 396, 0x1041 }, { 372, 0x103d }, { 348, 0x1039 }, { 330, 0x1036 },
        { 312, 0x1033 }, { 288, 0x102f }, { 264, 0x102b },
};

void dramc_set_freqency_phy(uint32_t pll_p, uint32_t phy_div)
{
        irf->set_pll(PLL_V, (pll_p & 0xffff));
        irf->module_set_clock(DDRPHY_CLK_BASE, PLL_V, phy_div);
}

/**
 * set dram frequency using the max one in freq_val table which are less than dram.freq
 */
void dramc_set_frequency(void)
{
        uint32_t i;

        for(i = 0; i < FR_NUM; i++) {
                if(dram.freq >= freq_2_param[i].freq) {
                        dramc_set_freqency_phy(freq_2_param[i].param, 1);
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
}

/**
 * set parameter in infotm_timing struct for dramc timing setting
 */
static void dramc_set_timing(void)
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
        timing->trefi = trefi_param[dram.type != DDR3][dram.capacity];

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
       timing->tcwl = (dram.cl < 7) ? 5 : 6;

       /* trfc */
       timing->trfc = division(trfc_param[dram.type != DDR3][dram.capacity] * dram.freq, 1000000);

       /* trfc_d, trfc, trp */
       timing->trfc_d = timing->trfc - timing->trfc_d;
       timing->trcd = (dram.type == DDR3) ? dram.cl : (lpddr2_timing[dram.cl - 3]);
       timing->trp = timing->trcd;
}

/**
 * NIF: custom-designed memory management units
 */
void dramc_init_nif(void)
{
        struct infotm_nif_info nif_info;
        uint32_t burst[] = {1, 2, 0, 3};

        nif_info.bank = dram.bank - 2;
        nif_info.row = dram.row - 11;
        nif_info.col = dram.col - 7;
        nif_info.io = ((dram.reduce_flag) ? 1 : 2);
        nif_info.burst = burst[dram.burst_len];
        nif_info.map = dram.addr_map_flag ? 0 : 1;

        /* nif reset for development board */
        writel(0xff, NIF_SYSM_ADDR + 0x0);
        writel(0xff, NIF_SYSM_ADDR + 0x4);
        writel(0x01, NIF_SYSM_ADDR + 0x8);
        writel(0xff, NIF_SYSM_ADDR + 0xc);
        writel(0xff, NIF_SYSM_ADDR + 0x18);
        writel(0xfe, NIF_SYSM_ADDR + 0x0);

        writel((0x2<<3) | (0x1<<2) | (0x1<<1) | 0x1,
                        0x29000000);
        writel((nif_info.map << 10) | (nif_info.bank << 8) | (nif_info.row << 5) |(nif_info.col << 2) | nif_info.io,
                        0x29000004);

        if(IROM_IDENTITY == IX_CPUID_X15)
                writel((0x10 <<4) | (nif_info.burst <<2) | (0x0<<1) | 0x0,
                                0x29000008);
        else
                writel((nif_info.burst <<2) | (0x0<<1) | 0x0,
                                0x29000008);

        writel(0, NIF_SYSM_ADDR  + 0x0);
        writel(0, EMIF_SYSM_ADDR + 0x20);
}

/**
 * assert and release uPCTL reset pins and MCFG configuration
 */
void memory_init_uPCTL(void)
{
        uint32_t tmp = 0;
        uint32_t burst_len = dram.burst_len;
        uint32_t bl_value[4] = {1, 2, 0, 3}; /* only for lpddr2 */

        /* Assert and release uPCTL reset pins */
        writel(dram.freq,       ADDR_UPCTL_TOGCNT1U);   /* 1us delay, number of internal clock , it has the same value of frequency */
        writel(0xf0,            ADDR_UPCTL_TINIT); /* time period(us) during memory power up, at least 200us, in Verilog simulations may set 1 */
        writel((dram.type == DDR3) ? 0x200 : 0,
                        ADDR_UPCTL_TRSTH);         /* time period(us) during ddr3 power up, at least 500us, in verilog simulations may set 1 */
        writel(division(dram.freq, 10),
                        ADDR_UPCTL_TOGCNT100N);    /* 100ns delay, number of internal clock */

        /* uPCTL MCFG config */
        if(dram.type == DDR3)
                tmp = (((dram.timing.faw_eff & 0x3) << 18) | (1 << 17) | (1 << 5) | (1 << 0)); /* tFAW, fast exit, DDR3 support, BL8 */
        else
                tmp = ((3 << 22) | ((bl_value[burst_len] & 0x3) << 20) | (1 << 18) | (1 << 17)
                                | ((dram.type == LPDDR2S4) << 6));      /* lpddr2 enable, burst length, tFAW, fast exit, lpddr2S4 enable */
        writel(tmp,     ADDR_UPCTL_MCFG);

        writel(readl(ADDR_UPCTL_MCFG1) | (((dram.timing.faw_eff & 0x7) >> 16) << 8),
                        ADDR_UPCTL_MCFG1);      /* fine tune the tFAW setting in MCFG */

#ifdef FPGA_S2C_TEST
        writel(0x018c2e02,      ADDR_PHY_PGCR); /* passive for fpga */
#elif (defined FPGA_HAPS_TEST)
        writel(0x018c2e02,      ADDR_PHY_PGCR);
#endif

}

static const uint32_t wl[6] = {1, 2, 2, 3, 4, 4};  /* write latency, relate to read latency for lpddr2, ?? some diffs from spec */

/**
 * DFI timing configuration
 */
void memory_set_DFI_timing(void)
{
         uint32_t cl = dram.cl;

         writel(0x0,    ADDR_UPCTL_DFITCTRLDELAY);      /* the number of DFI clock cycles */
         writel(0x2,    ADDR_UPCTL_DFITCTRLUPDMIN);     /* enable ODT for rank 0 */
         writel(0x4,    ADDR_UPCTL_DFITCTRLUPDMAX);     /* max number of DFI clock cycles that the dfi_ctrlupd_req signal can assert */
         writel(0x0,    ADDR_UPCTL_DFITCTRLUPDDLY);     /* Delay in DFI clock cycles of uPCTL init updated */

         if(dram.type == DDR3) {
                 if(dram.rank_sel == 1)
                         writel(0x8,    ADDR_UPCTL_DFIODTCFG);      /* rank0_odt_write enable */
                 else if(dram.rank_sel == 2)
                         writel(0x800,  ADDR_UPCTL_DFIODTCFG);    /* rank1_odt_write enable */
                 else
                         writel(0x0505, ADDR_UPCTL_DFIODTCFG);   /* two rank */

                 writel(((6 << 24) | (6 << 16) | ((cl - dram.timing.tcwl) << 8)),
                                        ADDR_UPCTL_DFIODTCFG1); /* ODT length for BL8 read and write transfers, ODT latency for reads */
                 writel((dram.timing.tcwl == 5) ? 4 : 5,
                                        ADDR_UPCTL_DFITPHYWRLAT);       /* time between write cmd sent and signal asserted */
                 writel(cl - 2,         ADDR_UPCTL_DFITRDDATAEN);       /* time between read cmd sent and signal asserted */
         } else {
                 writel(0x0,            ADDR_UPCTL_DFIODTCFG);
                 writel(wl[cl - 3],     ADDR_UPCTL_DFITPHYWRLAT);
                 writel(cl - 1,         ADDR_UPCTL_DFITRDDATAEN);
         }

         writel(0x0,    ADDR_UPCTL_DFIUPDCFG);  /* disable PHY and PCTL init update */
}

/**
 * PHY timing configuration and PHY initialization
 */
void memory_set_PHY(void)
{
        uint32_t cl = dram.cl;
        struct infotm_dram_timing *timing = &dram.timing;

        /* PHY timing set */
        if(dram.type == DDR3) {
                writel((0x01802E06 | (dram.rank_sel << 18)),
                                ADDR_PHY_PGCR);         /* DQSCFG, DFTCMP, CKEN, CKDV, ZCKSEL, PDDISDX */
                writel(0xb,     ADDR_PHY_DCR);          /* DDR8BNK, DDRMD(DDR3) */
                writel(((cl - 4) << 4) | ((timing->twr - 4) << 9),
                                ADDR_PHY_MR0);          /* CAS latency, write recovery(use twr) */
                writel((dram.rtt | dram.driver),
                                ADDR_PHY_MR1);          /* RTT, DIC */
                writel((timing->tcwl - 5) << 3,
                                ADDR_PHY_MR2);          /* CWL(CAS write latency, use tcwl) */
                writel(0x0,     ADDR_PHY_MR3);
                writel((4 << 2) | (4 << 5) | (cl << 8) | (cl << 12) | (timing->tras << 16) | (timing->trrd << 21) | (timing->trc << 25),
                                ADDR_PHY_DTPR0);        /* tRTP, tWTR, tRP, tRCD, tRAS, tRRD, tRC */
                writel((timing->tfaw << 3) | (timing->trfc << 16),
                                ADDR_PHY_DTPR1);        /* tFAW, tRFC */
                writel(0x10022c00,
                                ADDR_PHY_DTPR2);
                writel((0x1b << 0) | (0x14 << 6) | (0x8 << 18),      ADDR_PHY_PTR0); /* DLL soft reset time, DLL lock time, ITM soft reset time */
        } else {
                writel(0x01802E02 | (dram.rank_sel << 18),
                                ADDR_PHY_PGCR);
                writel(0x4 | ((dram.bank - 2) << 3) | ((dram.type != LPDDR2S4) << 8),
                                ADDR_PHY_DCR);
                writel((dram.burst_len + 2) | ((timing->twr - 2) << 5),
                                ADDR_PHY_MR1);  /* not consider burst 16 */
                writel(cl - 2,  ADDR_PHY_MR2);
                writel(dram.driver,     ADDR_PHY_MR3);
                writel(0x80 | (timing->trp << 8) | (timing->trcd << 12) | (timing->tras << 16)
                                                | (timing->trrd << 21) | (timing->trc << 25),
                                        ADDR_PHY_DTPR0);
                writel(0x19000000 | (timing->trfc << 16) | (timing->tfaw << 3),
                                ADDR_PHY_DTPR1);
                writel(0x0647a0c8,      ADDR_PHY_DTPR2);
                writel(0x0020051b,      ADDR_PHY_PTR0);
                writel(0x00000910,      ADDR_PHY_DXCCR);
                writel(0xfa00013f,      ADDR_PHY_DSGCR);
        }

        if(dram.rank_sel != 0x3)
                writel(0x0,     ADDR_PHY_ODTCR);        /* disable ODT in other banks when WR in a particular bank */

        /* PHY init */
        if(dramc_wake_up_flag) {
                writel(0x40000001,      ADDR_PHY_PIR);  	/* ddr system init */
                while((readl(ADDR_PHY_PGSR) & 0x1) == 0x1);     /* wait for init done */
                writel(0x1000014a,      ADDR_PHY_ZQ0CR0);       /* impendance control and calibration */
                writel(0xe,             0x21e09c4c);
                writel(0x1,             0x21e09c7c);
                writel(0x0000014a,      ADDR_PHY_ZQ0CR0);
                writel(0x00000009,      ADDR_PHY_PIR);          /* impendance calibrate */
                while((readl(ADDR_PHY_PGSR) & 0x5) == 0x5);     /* wait for init done and impendance calibration done */
        } else {
                writel(0x0000000f,      ADDR_PHY_PIR);  /* ddr system init, dll soft reset, dll lock and impendance calibrate */
                while((readl(ADDR_PHY_PGSR) & 0x7) == 0x7);     /* wait for init done, dll lock done and impendance calibration done */
        }
        writel(0x0000011,       ADDR_PHY_PIR);          /* interface timing module soft reset */
        while((readl(ADDR_PHY_PGSR) & 0x3) == 0x3);     /* wait for init done and dll lock done */
}

/**
 * wait DFI interface init complete, and power up
 */
void memory_start_power_up(void)
{
        /* wait for DFI init complete */
        while(!(readl(ADDR_UPCTL_DFISTSTAT0) & 0x1));   /* wait for DFI init complete */
        writel(0x00040001,      ADDR_PHY_PIR);          /* indicate that dram init will be performed by the controller */

        /* start power up by setting power_up_start , monitor power up status   power_up_done */
        if(!dramc_wake_up_flag) {
                writel(0x1,     ADDR_UPCTL_POWCTL);     /* Start the memory power up sequence */
                while(!(readl(ADDR_UPCTL_POWSTAT) & 0x1));      /* Returns the status of the memory power-up sequence */
        }
}

/**
 * set memory timing, ddr3 and lpddr2 respectively
 */
void memory_set_timing(void)
{
	uint32_t cl = dram.cl;
	struct infotm_dram_timing *timing = &dram.timing;

	if(dram.type == DDR3){
    	        writel(0x00000004,      ADDR_UPCTL_TMRD);                       /* tmrd, MRS to MRS command time */
    	        writel(timing->trfc - timing->trfc_d,   ADDR_UPCTL_TRFC);       /* trfc, refresh to refresh, or refresh to command delay */
    	        writel(cl,              ADDR_UPCTL_TRP);                        /* trp, precharge period */
    	        writel(cl + 6 - timing->tcwl,           ADDR_UPCTL_TRTW);       /* trtw, read to write turn around time */
                writel(timing->tcwl,    ADDR_UPCTL_TCWL);                       /* tcwl, CAS write latency */
    	        writel(cl,              ADDR_UPCTL_TRCD);                       /* trcd, row to column delay */
    	        writel(0x00000004,      ADDR_UPCTL_TRTP);                       /* trtp, read to precharge time */
    	        writel(0x00000004,      ADDR_UPCTL_TWTR);                       /* twtr, write to read turn around time */
    	        writel(0x00000200,      ADDR_UPCTL_TEXSR);                      /* texsr, eixt self refresh to first command delay, 512 for ddr3 */
    	        writel(0x0000003f,      ADDR_UPCTL_TXPDLL);                     /* txpdll, exit power down to first valid CMD delay when dll is on */
    	        writel(0x00000040,      ADDR_UPCTL_TZQCS);                      /* tzqcs, SDRAM ZQ calibration short period */
    	        writel(0x00000000,      ADDR_UPCTL_TZQCSI);                     /* tzqcsi, SDRAM ZQCS interval */
    	        writel(0x00000005,      ADDR_UPCTL_TCKSRE);                     /* tcksre, only ddr3, time after self refresh entry that CKE is held high before going low */
    	        writel(0x00000005,      ADDR_UPCTL_TCKSRX);                     /* tcksrx, only ddr3, time that CKE is maintained high before issuing SRX */
    	        writel(0x00000003,      ADDR_UPCTL_TCKE);                       /* tcke, CKE min pulse width */
    	        writel(0x0000000c,      ADDR_UPCTL_TMOD);                       /* tmod, only ddr3, time from MRS to any valid non-MRS CMD */
    	        writel(0x00000040,      ADDR_UPCTL_TRSTL);                      /* trstl, memory reset low time */
    	        writel(0x00000210,      ADDR_UPCTL_TZQCL);                      /* tzqcl, ZQ calibration long period */
    	        writel(0x00000004,      ADDR_UPCTL_TCKESR);                     /* tckesr, min CKE low width for self refresh entry to exit timing */
        } else {
    	        writel(0x00000005,      ADDR_UPCTL_TMRD);                       /* tmrd */
    	        writel(timing->trfc - timing->trfc_d,   ADDR_UPCTL_TRFC);       /* trfc */
    	        writel(0x00020000 | (timing->trp),      ADDR_UPCTL_TRP);        /* trp */
    	        writel(0x00000004,      ADDR_UPCTL_TRTW);                       /* trtw */
	        writel(wl[cl - 3],      ADDR_UPCTL_TCWL);                       /* tcwl */
    	        writel(timing->trcd,    ADDR_UPCTL_TRCD);                       /* trcd */
    	        writel(0x00000000,      ADDR_UPCTL_TRTP);                       /* trtp */
    	        writel(0x00000004,      ADDR_UPCTL_TWTR);                       /* twtr */
    	        writel(0x0000006E,      ADDR_UPCTL_TEXSR);                      /* texsr  max 117 */
    	        writel(0x00000000,      ADDR_UPCTL_TXPDLL);                     /* txpdll */
                writel(division(75 * dram.freq, 1000),
                                        ADDR_UPCTL_TZQCS);                      /* tzqcs, 15~48*/
    	        writel((dram.type == LPDDR2S4) ? 0x0000a000 : 0x00000000,
                                        ADDR_UPCTL_TZQCSI);                     /* tzqcsi */
                writel(division(32 * dram.freq, 1000),
                                        ADDR_UPCTL_TZQCL);                      /* tzqcl */
    	        writel(0x00000005,      ADDR_UPCTL_TCKESR);                     /* tckesr */
    	        writel(0x00000005,      ADDR_UPCTL_TDPD);                       /* tdpd, min deep power down time (in us unit) */
        }

	writel(timing->trefi,           ADDR_UPCTL_TREFI);                      /* trefi, time period(in 100ns unit) of refresh interval */
	writel(0x00000000,              ADDR_UPCTL_TAL);                        /* tal, additive latency */
	writel(cl,                      ADDR_UPCTL_TCL);                        /* tcl, CAS latency */
	writel(timing->tras,            ADDR_UPCTL_TRAS);                       /* tras, active to precharge CMD delay */
	writel(timing->trc,             ADDR_UPCTL_TRC);                        /* trc, active to activate CMD delay(same bank), tras + trp */
	writel(timing->trrd,            ADDR_UPCTL_TRRD);                       /* trrd, active to activate CMD delay(different bank) */
	writel(timing->twr,             ADDR_UPCTL_TWR);                        /* twr, write recovery time */
	writel(0x00000006,              ADDR_UPCTL_TXP);                        /* txp, exit power down to first valid CMD delay when dll is on */
	writel(0x00000002,              ADDR_UPCTL_TDQS);                       /* tdqs, additional data turnaround time for accesses to different ranks */

        writel(0x00000401,   ADDR_UPCTL_SCFG);  /* enable hardware low-power interface */
}

/*
 * MCMD send command
 *
 *******************************************************
 *MCMD register format
 *   [31]        [27:24]     [23:20]    [19:17]   [16:4]     [3:0]
 *-----------------------------------------------------------------------
 * start_cmd   cmd_add_del   rank_sel   bank_addr cmd_addr  cmd_opcode
 *
 * start_cmd: when set, he command operation defined in the cmd_opcode field is started, auto cleared when cmd finished
 * cmd_add_del: the additional delay associated with each command to 2^n internal timers clock cycles
 * rank_sel: rank0 [1] rank1 [2] rank2 [4] rank3[8]
 * bank_addr: Mode Register address, MR0(0), MR1(1), MR2(2), MR3(3), if LPDDR2, these two fields are merged into cmd_addr
 * cmd_addr: for MR cmd, Mode Register value driven on the memory address bits, for others, ignored
 * cmd_opcode: deselect 0         this is only for timing purpose no actual direct
 *                prea  1         precharge all
 *                 ref  2         refresh
 *                 mrs  3         mode register set
 *                zqcs  4         zq calibration shot      only applies to LPDDR2/DDR3
 *                zqcl  5         zq calibration long      only applies to LPDDR2/DDR3
 *                rstl  6         software driven reset    only applies to DDR3
 *                      7         reserved
 *                 mrr  8         mode register read
 *                dpde  9         deep power down entry    only applies to mDDR/LPDDR2
 */
void MCMD_Send_Command(int cmd)
{
        writel(cmd,     ADDR_UPCTL_MCMD);
        while(readl(ADDR_UPCTL_MCMD) & 0x80000000);
}

#define CMD(cmd_add_del, bank_addr, cmd_addr, cmd_opcode) \
                ((1 << 31) | ((cmd_add_del) << 24) | (0xf << 20) | ((bank_addr) << 17) \
                                        | ((cmd_addr) << 4) | ((cmd_opcode) << 0))
/**
 * send CMD to init memory, ddr3 and lpddr2, respectively
 */
void memory_init_DWC(void)
{
        uint32_t i;
        uint32_t cmd_map[][5] = {
                {       /* DDR3 CMD */
                        CMD(0, 2, ((dram.timing.tcwl - 5) << 3), 3),                            /* MR2 cmd */
                        CMD(0, 3, 0, 3),                                                        /* MR3 cmd */
                        CMD(0, 1, (dram.rtt | dram.driver), 3),                                 /* MR1 cmd */
                        CMD(0, 0, ((dram.cl - 4) << 4) | ((dram.timing.twr - 4) << 9), 3),      /* MR0 cmd */
                        CMD(0, 0, 0, 5),                                                        /* ZQCL cmd */
                },
                {       /* LPDDR2 CMD */
                        CMD(1, 0, 0xc30a, 3),                                                   /* MRW(reset) cmd */
                        (dram.type == LPDDR2S4) ? CMD(9, 0, 0xfff0a, 3) : CMD(0, 0, 0, 0),       /* ZQCL cmd, only for lpddr2s4 */
                        CMD(0, 0, ((((dram.burst_len + 2) | ((dram.timing.twr - 2) << 5)) << 8) | (1 << 4)), 3),        /* MR1 cmd */
                        CMD(0, 0, (((dram.cl - 2) << 8) | (2 << 4)), 3),                        /* MR2 cmd */
                        CMD(0, 0, ((dram.driver << 8) | (3 << 4)), 3),                          /* MR3 cmd */
                },
        };

        MCMD_Send_Command(CMD(0, 0, 0, 0));      /* deselect command, nop */
        MCMD_Send_Command(CMD(6, 0, 0, 0));      /* additional delay, at least tXP, here set 6, for all ranks */
        if(!dramc_wake_up_flag) {
                for(i = 0; i < 5; i++)
                        MCMD_Send_Command(cmd_map[dram.type != DDR3][i]);
        }
}

/**
 * memory state transiton
 * @dest_state: the destination state you want to transition
 */
void memory_set_state(int dest_state)
{
        int state;
        int table[][2] = {
                { 0, CFG },             /* STAT_INIT_MEM */
                { 0, GO },              /* STAT_CONFIG */
                { 0, 0 },               /* STAT_CONFIG_REQ */
                { CFG, SLEEP },         /* STAT_ACCESS */
                { 0, 0 },               /* STAT_ACCESS_REQ */
                { WAKEUP, 0 },          /* STAT_LOW_POWER */
                { 0, 0 },               /* STAT_LOW_POWER_ENTRY_REQ */
                { 0, 0 },               /* STAT_LOW_POWER_EXIT_REQ */
        };

        while((state = (readl(ADDR_UPCTL_STAT) & 0x7)) != dest_state) {
                if(table[state][state < dest_state] != 0)
                        writel(table[state][state < dest_state],     ADDR_UPCTL_SCTL);
        }
}

/**
 * memory upctl partially populated memories configure and uMCTL configuration
 */
void memory_set_uMCTL(void)
{
        uint32_t i;

        /* upctl partially populated memories configure */
        if(dram.reduce_flag) {
                writel(0x1d,    ADDR_UPCTL_PPCFG);      /* Partially Populated Memories Configuration Register */
                writel(readl(ADDR_PHY_DX2GCR) & (~1),
                                ADDR_PHY_DX2GCR);      /* clear the bit0 */
                writel(readl(ADDR_PHY_DX3GCR) & (~1),
                                ADDR_PHY_DX3GCR);
        } else
                writel(0x0,     ADDR_UPCTL_PPCFG);

        /* umctl configure */
        for(i = 0; i < 8; i++)
                writel(0x00100130,      ADDR_UMCTL_PCFG(i));    /* port n cfg */
        writel(0x10000018,      ADDR_UMCTL_CCFG);               /* controller cfg */
        writel(0x044320c8,      ADDR_UMCTL_CCFG1);

#ifdef FPGA_S2C_TEST
        writel(0x00000111,      ADDR_UMCTL_DCFG);
#elif (defined FPGA_HAPS_TEST)
        writel(0x00000112,      ADDR_UMCTL_DCFG);
#else
        writel((dram.width << 0) | (dram.capacity << 2) | ((dram.type == LPDDR2S4) << 6)
                        | (!dram.addr_map_flag << 8),
                                ADDR_UMCTL_DCFG);                       /* dram cfg */
#endif

        /* Enable CMDTSTAT register by writing cmd_tstat_en = 1 Monitor command timers expiration by polling CMDTSTAT cmd_tstat = 1 */
        writel(0x1,     ADDR_UPCTL_CMDTSTATEN);                         /* cmd_tstat_en */
        while(!(readl(ADDR_UPCTL_CMDTSTAT) & 0x1));                     /* All command timers have expired */
}

/**
 * the training of DQS gating during reads, the PHY trained for optimum operating timing margins
 * using built-in DQS gate training
 */
int memory_set_data_training(void)
{
#ifdef FPGA_S2C_TEST
/* for fpga test, default set dq */
        writel(0x33333333, ADDR_PHY_DX0DQTR);
        writel(0x33333333, ADDR_PHY_DX1DQTR);
        writel(0x33333333, ADDR_PHY_DX2DQTR);
        writel(0x33333333, ADDR_PHY_DX3DQTR);
#elif (defined FPGA_HPAS_TEST)
        writel(0x33333333, ADDR_PHY_DX0DQTR);
        writel(0x33333333, ADDR_PHY_DX1DQTR);
        writel(0x33333333, ADDR_PHY_DX2DQTR);
        writel(0x33333333, ADDR_PHY_DX3DQTR);
#else
	if (dram.rank_sel != 3) {
		//      writel(0x7ffff000,      ADDR_PHY_DTAR); /* data training address, column, row and bank address */
		writel(0x7ffff000,      ADDR_PHY_DTAR); /* data training address, column, row and bank address */
		writel(0x00000081,      ADDR_PHY_PIR);  /* read dqs training: execute a PUBL training routine */
		while(!(readl(ADDR_PHY_PGSR) & 0x10));   /* wait until data training done */
		dramc_debug("ADDR_PHY_PGSR = 0x%x\n", readl(ADDR_PHY_PGSR));
		if(readl(ADDR_PHY_PGSR) & 0x20) {       /* test whether dramc training is error */
			return 1;
		}
	} else {
		if (dramc_wake_up_flag)
			writel(0x01882E06, ADDR_PHY_PGCR);
		writel(DQS_ADDR+0x1000, ADDR_PHY_DTAR);	/*data training address, column, row and bank address*/
		writel(0x181, ADDR_PHY_PIR);		/*PHY Initialization: Read DQS Training & Read Valid Training*/
		while((readl(ADDR_PHY_PGSR) & 0x10) != 0x10);	/* wait for Data Training Done */
		dramc_debug("ADDR_PHY_PGSR = 0x%x\n", readl(ADDR_PHY_PGSR));	/*Read General Status Register*/
	}
#endif
        return 0;
}

static void dqs_training_step_two(void)
{
	int i = 0;
	int crc_dqs = 0;

#if (!defined(FPGA_S2C_TEST) && !defined(FPGA_HPAS_TEST))
	if (dram.rank_sel == 3) {
		if (!dramc_wake_up_flag) {
			for (i = 0; i < 4; i++)
				writel(readl(ADDR_PHY_DXDQSTR(i)), DQS_ADDR+4*i);
			writel(CRC_MAGIC, (DQS_ADDR+4*4));

			irf->hash_init(0, 20);
			irf->hash_data(DQS_ADDR, 20);
			irf->hash_value(&crc_dqs);
			writel(crc_dqs, (DQS_ADDR+4*5));
		} else {
			irf->hash_init(0, 20);
			irf->hash_data(DQS_ADDR, 20);
			irf->hash_value(&crc_dqs);

			if ((readl(DQS_ADDR+4*4) == CRC_MAGIC) && (crc_dqs == readl(DQS_ADDR+4*5))) {
				for (i = 0; i < 4; i++)
					writel((readl(DQS_ADDR+4*i) & 0x3007) | readl(ADDR_PHY_DXDQSTR(i)), ADDR_PHY_DXDQSTR(i));
			}
			writel(0x18c2e06, ADDR_PHY_PGCR);
		}

		if (readl(ADDR_PHY_PGSR) & 0x20) {	/* test whether DQS gate training is error */
			spl_printf("DQS Gate Training Error\n");
			/*DQS err than judge which bit is err*/
			for (i = 0; i < (dram.reduce_flag ? 2 : 4); i++) {
				if(readl(ADDR_PHY_DXGSR0(i)) & 0xf0)
					spl_printf("dram train byte%d error \n", i);
				else
					spl_printf("dram train byte%d Ok \n", i);
			}
			return 1;
		}

		if ((readl(ADDR_PHY_PGSR) & 0x100) == 0x100) {	/* test whether dramc Read Valid Training is error */
			dramc_debug("Read Valid Training Error\n");
			return 1;
		}
	}
#endif
}

/**
 * dramc memory init with PUBL
 */
int dramc_init_memory(void)
{
       irf->module_enable(EMIF_SYSM_ADDR);

       if(!dramc_wake_up_flag) {
               writel(0xe,      0x21e09c4c);    //?? use macro
               writel(0x1,      0x21e09c7c);
       } else
           spl_printf("dram wake up  \n");

        /* assert and release upctl reset pins,  and upctl MCFG config */
        memory_init_uPCTL();

        /* DFI timing set */
        memory_set_DFI_timing();

        /* PHY timing set and init */
        memory_set_PHY();

        /* start power up */
        memory_start_power_up();

        /* memory timing configuration */
        memory_set_timing();

        /* memory initialization procedure */
        memory_init_DWC();

        /* uPCTL moves to Config state and sets STAT.ctl_stat = Config when completed */
        memory_set_state(STAT_CONFIG);

        /* set uMCTL */
        memory_set_uMCTL();

        /* SDRAM init complete . Perform PUBL training as required */
        if(memory_set_data_training())
                return 1;

	/*test resume memory*/

        /* write GO into SCTL state_cmd register and poll STAT ctl_stat = Access */
        memory_set_state(STAT_ACCESS);

	dqs_training_step_two();

        return 0;
}

/**
 * check if the automatically calculated dram size is correct
 * As uboot0 is running in SRAM, so I delete the read-and-write-back codes to decrease the size of dramc.o
 *
 * return 0 if correct, return 1 otherwise
 */
int dramc_verify_size(void)
{
        int checkaddr1, checkaddr2;

        if(IROM_IDENTITY == IX_CPUID_X15 && (dram.rank_sel == 1 || dram.rank_sel == 3))
                checkaddr1 = 0x40000000;
        else
                checkaddr1 = 0x80000000;
        checkaddr2 = checkaddr1 + (dram.size << 19);
        dramc_debug("checkaddr1 = 0x%x, checkaddr2 = 0x%x\n", checkaddr1, checkaddr2);
        writel(87654321, checkaddr2);
        writel(12345678, checkaddr1);

        return (readl(checkaddr2) != 87654321);
}
/**
 * the whole flow of initializing for one dram type
 *
 * return 0 when succeed, return 1 otherwise
 */
int dramc_init_one_type(void)
{
        /* get items parameters for infotm dram info struct */
        dramc_debug("\ndramc init start\n");
        if(dramc_get_items()) {
                dramc_debug("dramc get items error\n");
                return 1;
        }

        /* initialize for other parameters in infotm_dram_info struct */
        if(dramc_set_addr()) {
                dramc_debug("dram info default init error!\n");
                return 1;
        }

        /* initialize frequency, timing and PCTL for dram */
        dramc_set_frequency();
        dramc_set_timing();
        dramc_init_nif();
        if(dramc_init_memory()) {
                dramc_debug("memory init error!\n");
                return 1;
	}
	if(auto_flag == 1) {
		if (dramc_verify_size()) {
			dramc_debug("memory total size error!\n");
			return 1;
		}
	}

        return 0;
}

/* dram count and capacity possible group for detect dram count and capacity automatically
 * we put the common used parameter groups before others to decrease to time of automatical detection */
static const uint32_t dram_param[2][9][2] = {
    {   //x820
        { 4, 256 },             // count = 4, capacity = 256MB, size = 1GB
        { 2, 512 },             // count = 2, capacity = 512MB, size = 1GB
        { 4, 128 },             // count = 4, capacity = 128MB, size = 512MB
        { 2, 256 },             // count = 2, capacity = 256MB, size = 512MB
    },
    {   //x15
        { 4, 512 },             // count = 4, capacity = 512MB, size = 2GB
        { 8, 256 },             // count = 8, capacity = 256MB, size = 2GB
        { 4, 256 },             // count = 4, capacity = 256MB, size = 1GB
        { 8, 128 },             // count = 8, capacity = 128MB, size = 1GB
        { 2, 512 },             // count = 2, capacity = 512MB, size = 1GB
        { 4, 128 },             // count = 4, capacity = 128MB, size = 512MB
        { 2, 256 },             // count = 2, capacity = 256MB, size = 512MB
        { 1, 512 },             // count = 1, capacity = 512MB, size = 512MB, not common
    },
};
/**
 * the enter point of dramc.c, describe the whole dram initialize process,
 * dram count and dram capacity can be detected automatically by this program
 * @irf: to call irom funcitons, we can delete it because it is useless,
 *      but we must modify this API in other files meanwhile.
 * @wake_up: to describe whether it is a boot process or a wake up process
 *
 * return 0 when dramc initialize done and successfully, return 1 when failed
 */
int dramc_init(struct irom_export *irf, int wake_up)
{
        int i;

        dramc_debug("DDR V5.0: running time: %s\n", __TIME__);
        dramc_wake_up_flag = wake_up;
        for(i = 0; i < ARRAY_SIZE(dram_param[0]) && dram_param[IROM_IDENTITY == IX_CPUID_X15][i][0] != 0; i++) {
                dram.count = dram_param[IROM_IDENTITY == IX_CPUID_X15][i][0];
                dram.capacity = dram_param[IROM_IDENTITY == IX_CPUID_X15][i][1];
                if(!dramc_init_one_type()) {
                        dramc_debug("dramc init succeed and finished\n");
                        return 0;
                }
        }
        spl_printf("the dram count and capacity is not suported so far, please check it!\n");
        return 1;
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



