#include <training.h>
//#include <dramc_init.h>
#include <items.h>
#include <rballoc.h>
#include <preloader.h>

#define DRAM_DBG

#ifdef DRAM_DBG
#define dram_log(x...) irf->printf(x)
#else
#define dram_log(x...)
#endif

static volatile struct module_work work;
extern int mini_div(int v1, int v2);
extern void dram_fr_set(uint32_t fr, struct irom_export *irf);
extern const uint32_t fr_val[FR_NUM]; 

/*
 * timer config
 */
static inline void timer_config(uint32_t n, uint32_t mask, uint32_t mode)
{
	uint32_t ret = 0;
	
	ret = rTimerControlReg(n);
	ret &= ~((1 << TIMER_INTR_MASK) | (1 << TIMER_MODE));
	ret |= (mask << TIMER_INTR_MASK) | (mode << TIMER_MODE);
	rTimerControlReg(n) = ret;
}

/*
 * timer control
 * 
 * mode : enable or disable
 */
static inline void timer_control(uint32_t n, uint32_t mode)
{
	uint32_t ret = 0;
	
	ret = rTimerControlReg(n);
	ret &= ~(1 << TIMER_ENABLE);
	ret |= (mode << TIMER_ENABLE);
	rTimerControlReg(n) = ret;
}

/*
 * timer count set
 */
static inline void timer_count_set(uint32_t n, uint32_t value)
{
	rTimerLoadCount(n) = value;
}

/*
 * timer count get
 */
static inline uint32_t timer_count_get(uint32_t n)
{
	return rTimerCurrentValue(n);
}
	
static inline void cmn_timer_init(void)
{
	work.timeout_v = 0xffffffff - APB_CLK / TIMEOUT_DIV;
        irf->module_enable(TIMER_SYSM_ADDR); 
}

static inline void timer_set(void)
{   
	timer_config(TIMER0, MASK, FREE_MODE);
	timer_control(TIMER0, ENABLE);
}

static inline void timer_done(void)
{
	timer_control(TIMER0, DISABLE);	
}

static inline void timing_load(struct dram_train *dram)
{
	dram->trefi = rUPCTL_TREFI;
	dram->tzqcsi = rUPCTL_TZQCSI;
	rUPCTL_TREFI = 0;
	rUPCTL_TZQCSI = 0;
}

static inline void timing_restore(struct dram_train *dram)
{
	rUPCTL_TREFI = dram->trefi;
	rUPCTL_TZQCSI = dram->tzqcsi;
}

static void fre_change(const uint32_t fr, const uint32_t cl, struct DRAM_INFO_STRUCT *dr)
{	
	*(volatile unsigned *)(EMIF_SYSM_ADDR) = 7;
        
	dram_fr_set(fr, irf);
	
	dr->cl = cl;
        dr->fr = fr;
        dr->fr_100n = mini_div(fr, 10);
}

static inline void bist_seed_set(uint32_t seed)
{
	rPHY_BISTLSR = seed;
}

static inline void user_pattern_set(uint32_t low, uint32_t high)
{
	rPHY_BISTUDPR = (low & 0xffff) | ((high & 0xffff) << 16);	
}

static inline void dqtr_set(uint32_t byte, uint32_t val)
{   
    rPHY_DXDQTR(byte) = val;
}

static inline void dqstr_dqs_set(uint32_t byte, uint32_t val)
{
    uint32_t tmp = rPHY_DXDQSTR(byte);
    
    tmp &= 0xfc0fffff;
    tmp |= val << 20;
    rPHY_DXDQSTR(byte) = tmp;
}

static inline void byte_pare_set(uint32_t byte, struct byte_parameter *p)
{
	dqtr_set(byte, p->dqtr);
	dqstr_dqs_set(byte, p->dqstr_dqs);
}

/*
 * dram byte bist set
 * 
 * word_n : bist word count should be n * bl / 2
 * b : bank
 * r : row
 * c : col
 * rank : rank
 */
static inline void dram_byte_bist_set(uint32_t rank, struct addr_struct *addr, struct bist *bist)
{
    rPHY_BISTWCR    = addr->n_word;                    // BIST Word Count: Indicates the number of words to generate during BIST.
    rPHY_BISTAR0    = (addr->b << 28) | (addr->r << 12) | addr->c; // bank[31:28]=0, row[27:12]=0x100, col[11:0] = 0
    rPHY_BISTAR1    = rank | (bist->max_rank << 2) | (addr->inc << 4);           // cs[1:0] = 0
    rPHY_BISTAR2    = (bist->max_b << 28) | (bist->max_r << 12) | bist->max_c;
    
    rPHY_DX0DLLCR   = 0x40000000;           // set DXnDLLCR
    rPHY_DX1DLLCR   = 0x40000000;           // set DXnDLLCR
    rPHY_DX2DLLCR   = 0x40000000;           // set DXnDLLCR
    rPHY_DX3DLLCR   = 0x40000000;           // set DXnDLLCR
    
}

static inline uint32_t check_bist_done(void)
{
    uint32_t tmp = rPHY_BISTGSR;  // polling bist done flag
    
    if(tmp & 0x1)
    	return 1;
    return 0;
}

#define WALKING_0          0
#define WALKING_1          1
#define PSEUDO_RANDOM      2
#define USER_PRO           3

static uint32_t dram_byte_bist_run(uint32_t byte, uint32_t flag, const struct bist_tst *info, struct bist_result *result)
{
	uint32_t i;
	uint32_t ret;
	uint32_t pattern = info->pattern;
	
	if(pattern == USER_PRO)
		user_pattern_set(info->low, info->high);
	if((pattern == PSEUDO_RANDOM) && info->seed_flag)
		bist_seed_set(info->seed);
	if(flag)
	    rPHY_BISTRR = (byte << 19) | (pattern << 17) | 0x4009;
	else
    	    rPHY_BISTRR = (byte << 19) | (pattern << 17) | 0x6029;
	
	// TODO
	work.timeout_flag = 0;
	timer_set();
	ret = check_bist_done();
	while(!ret){
		i = timer_count_get(TIMER0);
		if(i < work.timeout_v){
			work.timeout_flag = 1;
			break;
		}
		ret = check_bist_done();
	};
	timer_done();
    result->bistgsr = rPHY_BISTGSR;
    result->bistwer = rPHY_BISTWER;
    result->bistber0 = rPHY_BISTBER0;
    result->bistber1 = rPHY_BISTBER1;
    result->bistber2 = rPHY_BISTBER2;
    result->bistwcsr = rPHY_BISTWCSR;
    result->bistfwr0 = rPHY_BISTFWR0;
    result->bistfwr1 = rPHY_BISTFWR1;
    
    rPHY_BISTRR = 0x3;
    for(i = 0; i < 0x10; i++);
    
    return work.timeout_flag;
}

static inline void addr_increase(struct addr_struct *addr, struct bist *bist)
{
    addr->c += addr->inc;
    if(addr->c == bist->max_c){
    	addr->c = 0;
    	addr->b += 1;
    	if(addr->b == bist->max_b){
    		addr->b = 0;
    		addr->r += 1;
    		if(addr->r == bist->max_r)
    			addr->r = 0;
    	}
    }
}

static const struct bist_tst info[5] = {
		SET_BIST_INFO(WALKING_0, 0, 0, 0, 0),
		SET_BIST_INFO(WALKING_1, 0, 0, 0, 0), 
		SET_BIST_INFO(PSEUDO_RANDOM, 0, 0, 0, 0), 
		SET_BIST_INFO(USER_PRO, 0x5555, 0xaaaa, 0, 0), 
		SET_BIST_INFO(USER_PRO, 0xaaaa, 0x5555, 0, 0)
};

volatile unsigned int copy_count = 1;
volatile unsigned int dtuawdt;

static inline uint32_t dtu_config(uint32_t seed)
{
	uint32_t ret;
	uint32_t i;
	
	// TODO
	rUPCTL_DTUCFG = 0x381 | (0x1f << 1) | (4 << 16);
	rUPCTL_DTUWACTL = 0x0;
	rUPCTL_DTURACTL = 0x0;
	rUPCTL_DTULFSRWD = seed;
	rUPCTL_DTULFSRRD = seed;
	rUPCTL_DTUECTL = 1;
	work.timeout_flag = 0;
	timer_set();
	ret = rUPCTL_DTUECTL;
	while(ret){
		i = timer_count_get(TIMER0);
		if(i < work.timeout_v){
			work.timeout_flag = 1;
			break;
		}
		ret = rUPCTL_DTUECTL;
	};
	timer_done();
	rUPCTL_DTUCFG = 0x0;
	ret = rUPCTL_DTUPDES;
	if(work.timeout_flag)
		return 1;
	if(ret & 0x100)
		return 1;
	return 0;
}

static inline uint32_t copy_time_check(uint32_t seed)
{		
	if(dtu_config(seed))
		return 1;
        return 0;
}

static inline void dtu_awdt_set(void)
{
   	rUPCTL_DTUAWDT = dtuawdt;
}

volatile unsigned int eye[4][32];

static inline void set_eye_bit(uint32_t byte, uint32_t dqs, uint32_t dqtr)
{
    eye[byte][(dqs >> 1)] |= 1 << (dqtr + ((dqs & 1) << 4)); 	
}

static uint32_t byte_bit_bist_test(struct dram_train *tr, uint32_t byte)
{
    uint32_t i;
    uint32_t flag = 0;
    uint32_t ret = 0;
    struct addr_struct addr;
    struct bist_result br[3];

    addr.b = 0;
    addr.c = 0;
    addr.inc = TEST_INC;
    addr.n_word = TEST_WORD_NUM;
    addr.r = 0;
    
    addr.byte = byte;
    addr.b = 0;
    addr.c = 0;
    addr.inc = TEST_INC;
    addr.n_word = TEST_WORD_NUM;
    addr.r = 0;
    Move_to_Config();
    timing_load(tr);
    dram_byte_bist_set(0, &addr, &tr->bist_info);
    if(dram_byte_bist_run(byte, 1, &info[0], &br[0]))
	    flag = 1;
    if(dram_byte_bist_run(byte, 1, &info[1], &br[1]))
	    flag = 1;
    if(dram_byte_bist_run(byte, 1, &info[2], &br[2]))
	    flag = 1;
    timing_restore(tr);
    Move_to_Access();

    if(flag == 0){
        for(i = 0; i < 3; i++){
            flag |= (br[i].bistber2 & 0xffff);
    	    flag |= ((br[i].bistber2 >> 16) & 0xffff);
        }

        for(i = 0; i < 8; i++){
            if(flag & (3 << (i * 2)))
	        ret |= 1 << i;
        }
    }else
	ret = 0xffffffff;

    return ret;
}

static void bist_bit_check(uint32_t byte, uint32_t b_info, struct train_info *t_info)
{
       uint32_t i;
       uint32_t val = 0;
       uint32_t tmp = 0;

       tmp |= b_info & 0xffff;
       tmp |= ((b_info >> 16) & 0xffff);
       for(i = 0; i < 8; i++){
           if(tmp & (3 << (i * 2)))
	       val |= 1 << i;
       }
       t_info->bit |= (val << (byte * 8));
}

static uint32_t bist_byte_test(struct dram_train *tr, uint32_t reduce_flag, struct train_info *t_info)
{
    uint32_t i, j;
    uint32_t thr;
    uint32_t byte = 0;
    uint32_t flag;
    struct addr_struct addr;
    struct byte_parameter p;
    struct bist_result br[5];
    
    if(reduce_flag)
	    thr = 2;
    else
	    thr = 4;

    addr.b = 0;
    addr.c = 0;
    addr.inc = TEST_INC;
    addr.n_word = TEST_WORD_NUM;
    addr.r = 0;
    
    p.dqstr_dqs = 0;
    p.dqtr = 0;
    
    for(i = 0; i < thr; i++){
    	for(j = 0; j < 32; j++)
    		eye[i][j] = 0;
    }
    
    for(byte = 0; byte < thr; byte++){
    	addr.byte = byte;
        addr.b = 0;
        addr.c = 0;
        addr.inc = TEST_INC;
        addr.n_word = TEST_WORD_NUM;
        addr.r = 0;
	flag = 0;
        for(i = 0; i < 64; i++){
	    if(flag)
		    continue;
    	    p.dqstr_dqs = i;
    	    for(j = 0; j < 16; j++){
		if(flag)
			continue;
                p.dqtr = j * 0x11111111;
            	Move_to_Config();
            	timing_load(tr);
                byte_pare_set(byte, &p);
                dram_byte_bist_set(0, &addr, &tr->bist_info);
                if(dram_byte_bist_run(byte, 0, &info[0], &br[0])){
//			dram_log("timeout  ..bist \n");
			return 1;
		}
                if(dram_byte_bist_run(byte, 0, &info[1], &br[1])){
//			dram_log("timeout  ..bist \n");
                	return 1;
		}
                if(dram_byte_bist_run(byte, 0, &info[2], &br[2])){
//			dram_log("timeout  ..bist \n");
                	return 1;
		}
                if(dram_byte_bist_run(byte, 0, &info[3], &br[3])){
//			dram_log("timeout  ..bist \n");
                	return 1;
		}
                if(dram_byte_bist_run(byte, 0, &info[4], &br[4])){
//			dram_log("timeout  ..bist \n");
                	return 1;
		}
                timing_restore(tr);
                Move_to_Access();
                addr_increase(&addr, &tr->bist_info);
                if((br[0].bistgsr == 1) && (br[1].bistgsr == 1) && (br[2].bistgsr == 1) && 
            	    	(br[3].bistgsr == 1) && (br[4].bistgsr == 1)){
                	if(!copy_time_check(copy_count++)){
                		set_eye_bit(byte, i, j);
                	}else{
			//	dram_log("dcu error  \n");
				return 1;
                	}
                }else{
//			dram_log("byte %d, bist error \n", byte);
			bist_bit_check(byte, br[0].bistber2, t_info);
			bist_bit_check(byte, br[1].bistber2, t_info);
			bist_bit_check(byte, br[2].bistber2, t_info);
			bist_bit_check(byte, br[3].bistber2, t_info);
			bist_bit_check(byte, br[4].bistber2, t_info);
                	flag = 1;
    	        }
	    }
        }
    }
    for(i = 0; i < thr; i++){
    	for(j = 0; j < 32; j++)
    		if(eye[i][j] != 0xffffffff)
    			return 1;
    }
    return 0;
}

static inline uint32_t bist_test(struct dram_train *tr, struct DRAM_INFO_STRUCT *dr, struct train_info *t_info)
{	
	if(bist_byte_test(tr, dr->reduce_flag, t_info))
		return 1;
	// TODO
	if(bist_byte_test(tr, dr->reduce_flag, t_info))
		return 1;
	
	return 0;
}

static const uint32_t rtt_drv[RTT_DRV_NUM * 2] = {
		RTT_DIV_4, DRIVER_DIV_6,
		RTT_DIV_2, DRIVER_DIV_6, 
		RTT_DIV_6, DRIVER_DIV_6, 
		RTT_DIV_4, DRIVER_DIV_7, 
		RTT_DIV_2, DRIVER_DIV_7, 
		RTT_DIV_6, DRIVER_DIV_7 
};

static uint32_t dramc_rtt_drv_train(uint32_t mode, struct dram_train *tr, struct DRAM_INFO_STRUCT *dr)
{
	uint32_t i;
	uint32_t p = 0;
	struct train_info *info = &tr->fr_cl[mode];
	
	info->main_p = 0;
	for(i = 0; i < RTT_DRV_NUM; i++){
		dr->rtt = rtt_drv[i * 2];
		dr->driver = rtt_drv[i * 2 + 1];
		if(dramc_init_sub(dr))
			continue;
		dtu_awdt_set();
		if(!bist_test(tr, dr, info))
			p |= 1 << i;
	} 
	if(p & ((1 << RTT_DRV_NUM) - 1)){
		p |= (uint32_t)1 << FR_CL_OK_F;
		info->main_p = p;
		return 0;
	}
	return 1;
	
}

static uint32_t dram_re_init(uint32_t rtt_m, struct DRAM_INFO_STRUCT *dr)
{
	uint32_t ret = 0;
	
	dr->timing.trfc_d = TRFC_D_DEFAULT;
   	dr->timing.tras_d = TRAS_D_DEFAULT;
   	dr->rtt = rtt_drv[rtt_m * 2];
   	dr->driver = rtt_drv[rtt_m * 2 + 1];
   	ret = dramc_init_sub(dr); 
   	dtu_awdt_set();
   	
   	return ret;
}

static inline void dram_set_trfc(uint32_t val)
{
	rUPCTL_TRFC = val;
}

static inline void dram_set_tras(uint32_t val, uint32_t _val)
{
	rUPCTL_TRAS = val;
	rUPCTL_TRC = _val;
}

static inline void dram_set_trefi(uint32_t val)
{
	rUPCTL_TREFI = val;
}

/* 
 * flag : TIM_TRFC_F TIM_TRAS_F TIM_TREFI_F
 * */
static inline uint32_t dram_test_tim_p(uint32_t flag, uint32_t val, uint32_t _val, uint32_t t)
{
	Move_to_Config();
	if(flag == TIM_TRFC_F)
		dram_set_trfc(val);
	else if(flag == TIM_TRAS_F)
		dram_set_tras(val, _val);
	else if(flag == TIM_TREFI_F)
		dram_set_trefi(val);
    Move_to_Access();
    if(copy_time_check(t))
    	return 1;
    return 0;
}

static volatile unsigned seed = 0; 

static void timing_p_train(uint32_t mode, struct dram_train *tr, struct DRAM_INFO_STRUCT *dr)
{
	uint32_t p = tr->fr_cl[mode].main_p;
	uint32_t _p;
	uint32_t rtt_m = 0;
	uint32_t trfc_d;
	uint32_t tras_d;
	uint32_t thr;
	struct dram_timing *tim = &dr->timing;
	uint32_t busrt_l[4] = {2, 4, 1, 8};
	
	tras_d = tim->tras_d;
	tr->fr_cl[mode].info_tim = 0;
	
	while(!(p & 0x1)){
		p >>= 1;
		rtt_m++;
	}
	/* trfc */
	dram_re_init(rtt_m, dr);
	trfc_d = tim->trfc_d + 1;
	p = tim->trfc;
	while(!dram_test_tim_p(TIM_TRFC_F, p - trfc_d, 0, seed++)){
		trfc_d++;
	}
	tr->fr_cl[mode].info_tim |= (p << TIM_TRFC_P_H) | ((p - trfc_d + 8) << TIM_TRFC_P_L);
	/* tras */
	dram_re_init(rtt_m, dr);
	tras_d = tim->tras_d + 1;
	p = tim->tras;
	_p = tim->trc;
	thr = tim->tras - dr->cl - busrt_l[dr->burst_len];
	while(!dram_test_tim_p(TIM_TRAS_F, p - tras_d, _p - tras_d, seed++)){
		if(tras_d == thr){
			break;
		}
		tras_d++;
	}
	tr->fr_cl[mode].info_tim |= (p << TIM_TRAS_P_H) | ((p - tras_d + 3) << TIM_TRAS_P_L);
#if 0
	/* trefi */
	dram_re_init(rtt_m, dr);
	trefi_d = tim->trefi_d + 1;
	p = TREFI_DEFAULT;
	while(!dram_test_tim_p(TIM_TREFI_F, p + trefi_d, 0, seed++)){
		trefi_d++;
	}
#endif
	tr->fr_cl[mode].tim_p = (trfc_d << TIM_TRFC_P) | (tras_d << TIM_TRAS_P);
	tr->fr_cl[mode].tim = (tim->trfc << TIM_TRFC_P) | (tim->tras << TIM_TRAS_P);
	/* TODO */
	dram_re_init(rtt_m, dr);
}

static const char *rtt_driver_name[] = {
        "rtt RZQ/4, driver RZQ/6",
        "rtt RZQ/2, driver RZQ/6",
        "rtt RZQ/6, driver RZQ/6",
        "rtt RZQ/4, driver RZQ/7",
        "rtt RZQ/2, driver RZQ/7",
        "rtt RZQ/6, driver RZQ/7"
};

static void train_log(uint32_t mode, struct dram_train *tr, struct DRAM_INFO_STRUCT *dr)
{
       uint32_t i; 
       uint32_t j;
       uint32_t main_p = tr->fr_cl[mode].main_p;
       uint32_t tras_d = (tr->fr_cl[mode].tim_p >> TIM_TRAS_P) & 0xff;
       uint32_t trfc_d = ((tr->fr_cl[mode].tim_p >> TIM_TRFC_P) & 0xff) - 6; 
       struct dram_timing *tim = &dr->timing;

       if(main_p & 0x80000000){
           for(i = 0; i < RTT_DRV_NUM; i++){
	       if(main_p & (1 << i)){
	           dram_log("%s is ok  \n", rtt_driver_name[i]);
	       }
	   }
	   j = tras_d >> 1;
	   dram_log("tras may choose :");
           for(i = 0; i < j; i++){
	       dram_log("%2.2d ", tim->tras - i * 2);
	   }
	   dram_log("\n");
	   j = trfc_d >> 3;
	   dram_log("trfc may choose :");
           for(i = 0; i < j; i++){
	       dram_log("%2.2d ", tim->trfc - i * 8);
	   }
	   dram_log("\n");
       }else{
           dram_log("can not use... \n");
       }
}

static uint32_t get_max_rtt(uint32_t n, struct dram_train *tr)
{
	uint32_t i, j;
	uint32_t main_p;
	uint32_t max = 0;
        uint32_t flag[6];
     

	for(i = 0; i < 6; i++){
	    flag[i] = 0;
	}
	for(i = 0; i < n; i++){
	    main_p = tr->fr_cl[i].main_p;
	    for(j = 0; j < 6; j++)
		    if(main_p & (1 << j))
			    flag[j]++;
	}
        main_p = tr->fr_cl[n].main_p;
        for(j = 0; j < 6; j++){
		if(!(main_p & (1 << j)))
			flag[j] = 0;
		if(max < flag[j]){
		    max = flag[j];
		    i = j;
		}
	}
	return i;
}

#define FR_SCALE    15

static void train_result_log(uint32_t i_s, struct dram_train *tr, struct DRAM_INFO_STRUCT *dr)
{
	uint32_t i, j;
	uint32_t t, k;
	volatile unsigned int p;
	uint32_t main_p;
	uint32_t timing;
	uint32_t fr_cl_f[FR_SCALE] = {2, 4, 6, 9, 12, 15, 17, 19, 21, 23, 25, 27,
	                              28, 29, 30};	
        uint32_t flag[FR_SCALE];

	for(i = 0; i < FR_SCALE; i++)
		flag[i] = 0;
	j = i_s;
	p = 1;
//	dram_log("i_s : %d \n", i_s);
	if(i_s)
		i_s = fr_cl_f[i_s - 1];
	for(i = 0; i < FR_CL_CHOOSE_NUM; i++){
	    dram_log("tr->fr_cl[%2.2d].main %x  \n", i, tr->fr_cl[i].main_p);
	    if(!(tr->fr_cl[i].main_p & 0x80000000))
		    p = 0;
	    if((i + 1 + i_s) == fr_cl_f[j]){
		    flag[j] = p;
		    j++;
		    p = 1;
	    }
	}
	if(tr->fr_cl[fr_cl_f[FR_SCALE - 1] - 1 - i_s].main_p & 0x80000000){
		flag[FR_SCALE - 1] = 1;
	}

	p = 0;
	t = FR_SCALE + 1 - dr->tr_pre_lv;
//	dram_log("t %d \n", t);
	for(i = 0; i < t; i++){
	    main_p = 1;
	    for(k = i; k < dr->tr_pre_lv + i; k++){
	        if(!flag[k]){
			main_p = 0;
			break;
		}
	    }
	    if(main_p){
	        j = i + dr->tr_pre_lv - 1;
		p = 1;
		break;
	    }
	}
	if((!p) && (flag[FR_SCALE - 1])){
	    j = FR_SCALE - 1;
	    p = 1;
	}     
	if(p){
	    if((j == (FR_SCALE - 1)) || (j == FR_SCALE - 2))
                p = fr_cl_f[FR_SCALE - 1] - 1 - i_s;
	    else
    	        p = fr_cl_f[j- 1] + 1 - i_s;
	    main_p = tr->fr_cl[p].main_p;
	    timing = tr->fr_cl[p].tim;
	    tr->choosenum = p;
            i = get_max_rtt(p, tr);
	    tr->fr_cl[p].info1 = i;
	    dram_log("Recommend that use fr %3.3d, cl %d %s, ", (tr->fr_cl[p].info0 & 0xffffff),
			    (tr->fr_cl[p].info0 >> INFO0_CL), rtt_driver_name[i]);
            dram_log("tras : %d  trfc : %d  \n", (timing >> TIM_TRAS_P) & 0xff, (timing >> TIM_TRFC_P) & 0xff);
	}else{
	    tr->choosenum = 44;    
	    dram_log("please choose parameters depend on log  \n");
	}

}

static inline uint32_t dramc_fr_cl_train(struct dram_train *tr, struct DRAM_INFO_STRUCT *dr)
{
	uint32_t mode = 0;
        uint32_t fr = dr->tr_fr_max;
        uint32_t i;
	uint32_t i_s;
	uint32_t j;
        uint32_t cl[4] = {5, 6, 7, 8};
	uint32_t cl_s;
	uint32_t cl_e;
	uint32_t flag;
	uint32_t count = 0;

	for(i = 0; i < FR_NUM; i++){
	    if(fr < fr_val[i]){
//		    dram_log("fr_val[%d] %d \n", i, fr_val[i]);
            }else{
		    i_s = i;
		    break;
	    }
	}
        for(i = i_s; i < FR_NUM; i++){
	    fr = fr_val[i];
	    if(fr > 550){
	        cl_s = 2;
		cl_e = 4;
	    }else if(fr >= 450){
	        cl_s = 1;
		cl_e = 4;
	    }else if(fr >= 320){
	        cl_s = 0;
		cl_e = 2;
	    }else{
	        cl_s = 0;
		cl_e = 1;
	    }
	    for(j = cl_s; j < cl_e; j++){
		flag = 1;
	        tr->fr_cl[mode].info0 = (fr << INFO0_FR) | (cl[j] << INFO0_CL);
		tr->fr_cl[mode].bit = 0;
		tr->fr_cl[mode].info1 = 0;
		/*  change fr and cl */
		fre_change(fr, cl[j], dr);
		dram_log("test : fr %3.3d, cl %d  \n", fr, cl[j]);
		if(!dramc_rtt_drv_train(mode, tr, dr))
			timing_p_train(mode, tr, dr);
		else
			flag = 0;
		train_log(mode, tr, dr);
		mode++;
	    }
	    if(flag){
	        count++;
		if(count == (dr->tr_pre_lv + 1)){
		    dram_log("done     \n");
		    break;
		}
	    }else{
	        if(count)
	            count = 0;
	    }
	}
        tr->t_mode = mode;
	train_result_log(i_s, tr, dr);

        return 0;	
}

static inline uint32_t dramc_ddr3_train(struct dram_train *tr, struct DRAM_INFO_STRUCT *dr)
{ 
	dramc_fr_cl_train(tr, dr);
	
	return 0;	
}

static inline uint32_t bist_info_init(struct bist *bist, struct DRAM_INFO_STRUCT *dr)
{
	uint32_t count;
	uint32_t rank = dr->rank_sel;
	
	count = (rank & 0x1) + ((rank >> 1) & 1) + ((rank >> 2) & 1) + ((rank >> 3) & 1) - 1;
	
	bist->max_rank = count;
	bist->max_c = ((1 << (dr->col)) - 1) & (~(TEST_INC - 1));
	bist->max_r =  (1 << dr->row) - 1;
	bist->max_b =  (1 << dr->bank) - 1;
	
	return 0;
}

static inline void dtu_limit_init(struct DRAM_INFO_STRUCT *dr)
{
	uint32_t count;
	uint32_t rank = dr->rank_sel;
	
	count = (rank & 0x1) + ((rank >> 1) & 1) + ((rank >> 2) & 1) + ((rank >> 3) & 1) - 1;
	/* for ddr3 bank is 8 and col is 10 or 11(regard as 10) */ 
	dtuawdt = (count << 9) | ((dr->row - 13) << 6) | 0xb;
	rUPCTL_DTUAWDT = dtuawdt; 
}

void dramc_bit_check(uint32_t byte, struct DRAM_INFO_STRUCT *dr)
{
    uint32_t i;
    uint32_t bit = 0;
    struct dram_train tr;

    if(dr->dram_type == DDR3){
        bist_info_init(&tr.bist_info, dr);
	cmn_timer_init();
	bit = byte_bit_bist_test(&tr, byte);
	if(bit == 0xffffffff)
		dram_log("time out ...can not check which bit is incorrect.  \n");
	else if(bit){
		dram_log("     bit");
	        for(i = 0; i < 8; i++){
		    if(bit & (1 << i))
			    dram_log(" %d", i);
		}
		dram_log(" error  \n");
	}
    }
}

static inline void train_info_copy(struct train_info *s, struct train_info *d)
{
        d->main_p    = s->main_p;
	d->tim_p     = s->tim_p; 
	d->tim       = s->tim; 
	d->info_tim  = s->info_tim; 
	d->info0     = s->info0; 
	d->info1     = s->info1; 
	d->bit       = s->bit; 
}

/*  */
static uint32_t memory_set(struct dram_train *tr)
{
	uint32_t i;
	uint32_t total = tr->t_mode;
//	uint32_t t = tr->choosenum;
	struct train_info *info;
        uint8_t *dtra = rballoc("dramdtra", 0x4000);

	*(volatile unsigned int *)(dtra + 0x0) = 0xff;
	*(volatile unsigned int *)(dtra + 0x4) = 0xff00;
	*(volatile unsigned int *)(dtra + 0x8) = tr->choosenum;
	*(volatile unsigned int *)(dtra + 0xc) = tr->t_mode;

#if 0
        for(i = 0; i < t; i++){
	    tr->fr_cl[t].bit |= tr->fr_cl[i].bit;
	}
#endif

	for(i = 0; i < total; i++){
	    info = (struct train_info *)(dtra + 0x10 + i * TRAIN_INFO_LEN);
	    train_info_copy(&tr->fr_cl[i], info);
	}

	return 0;
}

uint32_t dramc_training(struct DRAM_INFO_STRUCT *dr)
{
#ifdef DRAM_FLAG
	struct dram_train tr;

        dram_log("dram training     \n");	
	if(dr->dram_type != DDR3)
		return 1;
    
	if(bist_info_init(&tr.bist_info, dr))
		return 1;

	dtu_limit_init(dr);
	cmn_timer_init();
//	dram_log("time out %x     \n", work.timeout_v);
	if(dramc_ddr3_train(&tr, dr))
		return 1;
	dram_log("dram training done, please change the items depend on the training result \n");

        /* write in */
	memory_set(&tr);

	return 0;
#endif
}

