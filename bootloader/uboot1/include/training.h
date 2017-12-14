#ifndef __TRAINING_H__
#define __TRAINING_H__

#include "dramc_init.h"
//#include <imapx_cmn_timer.h>
#include <common.h>

#define APB_CLK            134000000
#define TIMEOUT_DIV        100
//#define DRAM_FLAG       1

struct module_work{
	uint32_t timeout_v;
	uint32_t timeout_flag;
};

struct bist_result{
	uint32_t bistgsr;
	uint32_t bistwer;
	uint32_t bistber0;
	uint32_t bistber1;
	uint32_t bistber2;
	uint32_t bistwcsr;
	uint32_t bistfwr0;
	uint32_t bistfwr1;
};

struct addr_struct{
	uint32_t b;
	uint32_t r;
	uint32_t c;
	uint32_t n_word;
	uint32_t inc;
	uint32_t byte;
};

struct bist_tst{
	uint32_t pattern;
	uint32_t low;
	uint32_t high;
	uint32_t seed_flag;
	uint32_t seed;
};

#define SET_BIST_INFO(_p, _l, _h, _flag, _seed) \
{ \
	_p, \
	_l, \
	_h, \
	_flag, \
	_seed, \
}

struct byte_parameter{
	uint32_t dqtr;
	uint32_t dqstr_dqs;
};


struct bist{
	uint32_t max_b;
	uint32_t max_c;
	uint32_t max_r;
	uint32_t max_rank;
};

struct train_info{
	volatile uint32_t main_p;
	uint32_t tim_p;
	uint32_t tim;
	uint32_t info_tim;
	uint32_t info0;
	uint32_t info1;
	uint32_t bit;                   /* 32 bit 0 :ok  1: error */
};

#define TRAIN_INFO_LEN  28

/*  tim_p */
#define TIM_TRFC_P      0
#define TIM_TRAS_P      8
#define TIM_TREFI_P     16

/* info */
#define INFO0_FR            0       /* dram phy fr */
#define INFO0_CL           24       /* cl value */

#define INFO1_CHOOSE        0       /* for ddr3 rtt driver */

/*  tim_p */
#define TIM_TRFC_P_L        0       /* trfc lowest value */ 
#define TIM_TRFC_P_H        8       /* trfc highest value */

#define TIM_TRAS_P_L       16       /* tras lowest value */
#define TIM_TRAS_P_H       24       /* tras highest value */

#define TIM_TRFC_F      0
#define TIM_TRAS_F      1
#define TIM_TREFI_F     2

/* main p */
#define FR_CL_OK_F      31

#define RTT_4_DRV_6     0
#define RTT_2_DRV_6     1
#define RTT_6_DRV_6     2
#define RTT_4_DRV_7     3
#define RTT_2_DRV_7     4
#define RTT_6_DRV_7     5
#define RTT_DRV_NUM     6

#define FR_1200_CL_7        0
#define FR_1200_CL_8        1
#define FR_1152_CL_7        2
#define FR_1152_CL_8        3
#define FR_1104_CL_6        4
#define FR_1104_CL_7        5
#define FR_1104_CL_8        6
#define FR_1066_CL_6        7
#define FR_1066_CL_7        8
#define FR_1066_CL_8        9
#define FR_984_CL_5         10
#define FR_984_CL_6         11
#define FR_984_CL_7         12
#define FR_888_CL_5         13
#define FR_888_CL_6         14
#define FR_800_CL_5         15
#define FR_800_CL_6         16
#define FR_744_CL_5         17
#define FR_744_CL_6         18
#define FR_667_CL_5         19
#define FR_667_CL_6         20
#define FR_533_CL_5         21
#define FR_CL_CHOOSE_NUM    45

#define TRAIN_FIRST         FR_1200_CL_7

struct dram_train{
    uint32_t dram_type;
    uint32_t trefi;
    uint32_t tzqcsi;
    uint32_t choosenum;
    uint32_t t_mode;
    struct train_info fr_cl[FR_CL_CHOOSE_NUM];
    struct bist bist_info;
};

#define DQDLY_MAX          0x10
#define DQSDLY_MAX         0x8
#define DQSNDLY_MAX        0x8
#define DMDLY_MAX          0x10

#define FR_1200       600
#define FR_1152       576
#define FR_1104       552
#define FR_1066       528      
#define FR_984        492
#define FR_888        444
#define FR_800        396
#define FR_744        372
#define FR_667        330
#define FR_533        264

#define FR_NUM        15

/* */

#define TIMER_ENABLE       0
#define TIMER_MODE         1
#define TIMER_INTR_MASK    2

#define TIMER0             0
#define TIMER1             1

#define UNMASK             0
#define MASK               1

#define FREE_MODE          0
#define COUNT_MODE         1

#define DISABLE            0
#define ENABLE             1

extern uint32_t dramc_training(struct DRAM_INFO_STRUCT *dr);
extern void dramc_bit_check(uint32_t byte, struct DRAM_INFO_STRUCT *dr);

#endif /* training.h */
