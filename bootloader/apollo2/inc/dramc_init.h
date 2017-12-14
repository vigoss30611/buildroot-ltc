#ifndef __INFOTM_DRAMC_INIT_H__
#define __INFOTM_DRAMC_INIT_H__

#include<linux/types.h>

/* DRAM type */
#define DDR3            0
#define LPDDR2S2        1
#define LPDDR2S4        2
#define DDR2            3
#define LPDDR3		4

/* driver for ddr3 */
#define DRIVER_DIV_6    0x0
#define DRIVER_DIV_7    0x2

/* drive strength for lpddr2 */
#define DS_34P3_OHM     1    /* 34.3-ohm */
#define DS_40_OHM       2    /*   40-ohm */
#define DS_48_OHM       3    /*   48-ohm */
#define DS_60_OHM       4    /*   60-ohm */
#define DS_68P6_OHM     5    /* 68.6-ohm  reserve */
#define DS_80_OHM       6    /*   80-ohm */
#define DS_120_OHM      7    /*  120-ohm */

/* dram io width */
#define IO_WIDTH8                     1
#define IO_WIDTH16                    2
#define IO_WIDTH32                    3 

/* dram capacity */
#define DRAM_8MB                     0
#define DRAM_16MB                    1
#define DRAM_32MB                    2
#define DRAM_64MB                    3
#define DRAM_128MB                      4
#define DRAM_256MB                      5
#define DRAM_512MB                      6
#define DRAM_1GB                      7

/* burst len */
#define BURST_2         0x1
#define BURST_4         0x2
#define BURST_8         0x4
#define BURST_16        0x8

/* training */
#define DRAM_TR_PRE_LV               5
#define DRAM_TR_FR_MAX               600
#define FR_NUM  15

/* STAT */
#define STAT_INIT_MEM                 0
#define STAT_CONFIG                   1
#define STAT_CONFIG_REQ               2
#define STAT_ACCESS                   3
#define STAT_ACCESS_REQ               4
#define STAT_LOW_POWER                5
#define STAT_LOW_POWER_ENTRY_REQ      6
#define STAT_LOW_POWER_EXIT_REQ       7

/* SCTL   ; stat change */
#define INT             0          /* move to Init_mem from Config */
#define CFG             1          /* move to Config from Init_mem or Access */
#define GO              2          /* move to Access from Config */
#define SLEEP           3          /* move to Low_power from Access */
#define WAKEUP          4          /* move to Access from Low_power */

/* dram size */
#define SIZE_1GB        0x40000000
#define SIZE_512MB      0x20000000

/* struct infotm_dram_timing - describes all the timings used in memory timing configuration
 * @tras: active to precharge command delay
 * @trrd: active to activate command delay(different banks)
 * @tfaw: 4-bank activate period
 * @trfc: refresh to refresh delay, or refresh to command delay
 * @trc:  active to activate command delay(same bank)
 * @twr:  write recovery time
 * @tcwl: CAS write latency
 * @trcd: row to column delay
 * @trp:  precharge period in memory clock cycles
 * @faw_eff: coefficient to calculate tfaw, tfaw = (faw_eff & 0x3 + 4) * trrd - (faw_eff >> 16)
 * @trfc_d: used to calculate trfc, I don't konw whether we must use it or not
 * @trefi: time period(in 100ns units) of refresh interval
 *
 */
struct infotm_dram_timing {
	uint32_t tras;
	uint32_t trrd;
	uint32_t tfaw;
	uint32_t trfc;
	uint32_t trc;
	uint32_t twr;
	uint32_t tcwl;
	uint32_t trcd;
	uint32_t trp;
	uint32_t faw_eff;
//        uint32_t tras_d;
	uint32_t trfc_d;
	uint32_t trefi;
};

/**
 * struct infotm_dram_info - describes all the dramc and memory parameters
 * @type: type of memory, including LPDDR2 and DDR3
 * @freq: dramc frequency
 * @cl: memory CAS latency, its range is 5 ~ 11, usually 7
 * @burst_len: memory burst length, effective values are BURST_2, BURST_4, BURST_8, and BURST_16
 * @rank_sel: cs count of dramc, we calculate it using width, count, capacity and CPUID
 * @count: number of memory chips, effective values are COUTN2, COUNT4, and COUNT8
 * @width: the data interface width of single memory chip, effective values are IO_WIDTH8, IO_WIDTH16, and IO_WIDTH32
 * @capacity: the capacity of single memory chip
 * @rtt: the value of resistance for ODT(on-die termination)
 * @driver: memory driving ability
 * @bank: bank address of memory
 * @row: row address of memory
 * @col: colomn address of memory
 * @reduce_flag: indicate whether we use partially populated memory, if count * width < 32, we set it
 * @addr_map_flag: to determin the bank position is before row position or not
 * @tr_fr_max: the max and beginning dram training frequency when using dram training
 * @tr_pre_lv: using in dram training
 * @size: total memory size, equals to (count * capacity)
 * @timing: dramc timing parameters
 *
 */ 
struct infotm_dram_info {
        uint32_t type;
        uint32_t freq;
        uint32_t cl;
        uint32_t burst_len;
        uint32_t rank_sel;
        uint32_t count;
        uint32_t width;
        uint32_t capacity;
        uint32_t rtt;
        uint32_t driver;
        uint32_t bank;
        uint32_t row;
        uint32_t col;
        uint32_t reduce_flag;
        uint32_t addr_map_flag;
        uint32_t tr_fr_max;
        uint32_t tr_pre_lv;
        uint32_t size;
        struct infotm_dram_timing timing;
        uint32_t dll_bypass;
};

/**
 * struct infotm_nif_info - infotm custom-designed memory management units
 * @bank: 0: 4 bank   1 : 8 bank
 * @row : 0 ~ 5 indicate 11 ~ 16
 * @col:  0 ~ 5 indicate 7 ~ 12 
 * @io: total io bit  0 : 8, 1: 16, 2 : 32 
 * @burst: 2'd0: BL2; 2'd1: BL4; 2'd2: BL8; 2'd3: BL16
 * @map:  b000: {rank,bank,row,column} b001: {rank,row,bank,column} b010: {bank,row,rank,column}
 *
 */
struct infotm_nif_info {
        uint32_t bank;
        uint32_t row;
        uint32_t col;
        uint32_t io;
        uint32_t burst;
        uint32_t map;
};

/**
 * struct infotm_freq_2_param - map of dram frequency and its value set in the frequency phy
 * @freq: frequency
 * @param: value which will be set in the frequency phy
 *
 */
struct infotm_freq_2_param {
        int freq;
        int param;
	int div;
};

extern uint32_t dram_size_check(void);
extern int dramc_init_sub(void);
extern void move_to_config(void);
extern void move_to_access(void);

#endif /* INFOTM_DRAMC_H */


