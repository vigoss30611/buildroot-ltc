/***************************************************************************** 
** infotm/drivers/adc/adc_imapx800.c 
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Description: File used for the standard function interface of output.
**
** Author:
**     Lei Zhang   <jack.zhang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 1.0  11/15/2012  Jcak Zhang   
*****************************************************************************/ 

#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>
#include <linux/compiler.h>
#include "imapx_adc.h"

static int g_keydown;
static int g_pendown;
static int g_max_rt;

// normal cmd
#define NML_STCOV_BIT			(0)
#define NML_SELREFP_BIT			(1)
#define NML_SELREFN_BIT			(4)
#define NML_SELIN_BIT			(7)
#define NML_XPPSW_BIT			(11)
#define NML_XNNSW_BIT			(12)
#define NML_YPPSW_BIT			(13)
#define NML_YNNSW_BIT			(14)
#define NML_XNPSW_BIT			(15)
#define NML_WPNSW_BIT			(16)
#define NML_YPNSW_BIT			(17)
#define NML_PENIACK_BIT			(18)
#define NML_ADC_OUT_BIT			(19)
#define NML_CNINTR_BIT			(20)
#define NML_AVG_BIT			(21)
#define NML_TSINTR_BIT			(22)
#define NML_CLKNUM_BIT			(23)  // kept-num for kept on, wait for CLKNUM time, then next operator
#define NML_PPOUT_BIT			(25)
#define NML_DMAIF_BIT			(26)

#define INST_TYPE_BIT			(30)

#define NORMAL_INSTR			(0)
#define JMP_INSTR			(1)
#define NOP_INSTR			(2)
#define END_INSTR			(3)

// jump cmd
#define CMD_JMP_COND			(1<<29)
#define CMD_JMP_UNCOND			(0<<29)
#define JMP_ADDR_BIT			(0)
#define JMP_ADDR_MSK			(0xff)
#define CMD_JMP_PENIRQ			(0<<27)
#define CMD_JMP_REP			(1<<27)
#define JMP_CYCLE_BIT			(8)
#define JMP_CYCLE_MSK			(0xff)
#define CMD_INSTR_JMP			(1<<30)

// nop cmd
#define NOP_CYCLE_MSK			(0xff)
#define CMD_INSTR_NOP			(2<<30)

// end cmd
#define CMD_INSTR_END			(3<<30)

#define IN_ADDR(x)			((x)>>2)

#define SAMPLE_DLYCLK			13

#define SZ_PRECHARGE			1
#define SZ_WPENIRQ			4
#define SZ_ENDCODE			1
#define SZ_CHSAMPLE			8
#define SZ_XXSAMPLE			8
#define SZ_YYSAMPLE			8
#define SZ_XPSAMPLE			8
#define SZ_YNSAMPLE			8

#define CMD_DISCHARGE			(CMD_PENIACK | CMD_XPPSW | CMD_YNNSW)

#define CMD_CC_OP(wpn,ynn,ypn,ypp,xnn,xnp,xpp) \
			(((wpn)<<NML_WPNSW_BIT)|((ynn)<<NML_YNNSW_BIT)|((ypn)<<NML_YPNSW_BIT)|((ypp)<<NML_YPPSW_BIT)|((xnn)<<NML_XNNSW_BIT)|((xnp)<<NML_XNPSW_BIT)|((xpp)<<NML_XPPSW_BIT))
#define CMD_SEL_OP(fp,in,fn)\
			(((fp)<<NML_SELREFP_BIT)|((in)<<NML_SELIN_BIT)|((fn)<<NML_SELREFN_BIT))	
#define CMD_PEN_OP(ack) ((ack)<<NML_PENIACK_BIT)

#define CMD_PRE_CHARGE		CMD_CC_OP(0,0,0,1,0,1,0) | CMD_PEN_OP(0)
#define CMD_TCD_ET		CMD_CC_OP(0,1,0,1,0,1,1) | CMD_PEN_OP(1)
#define CMD_YY_SAMPLE		CMD_CC_OP(0,1,0,0,0,1,1) | CMD_PEN_OP(0) | CMD_SEL_OP(0,0,1) 
#define CMD_XX_SAMPLE		CMD_CC_OP(0,0,0,1,1,1,0) | CMD_PEN_OP(0) | CMD_SEL_OP(1,1,0)
#define CMD_YN_SAMPLE		CMD_CC_OP(0,0,0,0,1,1,1) | CMD_PEN_OP(0) | CMD_SEL_OP(0,3,0)
#define CMD_XP_SAMPLE		CMD_CC_OP(0,0,0,0,1,1,1) | CMD_PEN_OP(0) | CMD_SEL_OP(0,0,0)
#define CMD_CH_SAMPLE(n)	CMD_CC_OP(0,0,0,1,0,1,0) | CMD_PEN_OP(0) | CMD_SEL_OP(4,(n),4)	


//#define ADC_DBG_OUT
#ifdef ADC_DBG_OUT
#define ADC_LOG   printf
#else
#define ADC_LOG(off, x...) do {} while(0)
#endif

#define ADC_DBGCMD_MCGEN
#ifdef ADC_DBGCMD_MCGEN
#ifdef ADC_DBG_OUT
static unsigned cmd_line;
#define ADC_DBGCMD_DUMP(off, x...)  do { \
					ADC_LOG("%x:", cmd_line); \
					ADC_LOG(x); \
					cmd_line += off;\
				} while(0)
#define ADC_DBGCMD_START(addr)  (cmd_line = addr)
#else
#define ADC_DBGCMD_DUMP(off, x...) do {} while(0)
#define ADC_DBGCMD_START(addr) do {} while(0)
#endif
#else
#define ADC_DBGCMD_DUMP(off, x...) do {} while(0)
#define ADC_DBGCMD_START(addr) do {} while(0)
#endif



struct ts_event {
	int     xx;
	int     yy;
	int     xp;
       int	   yn;
};

struct adc_key_dev {
	char	name[16];
	int	code;
	int	ref_val;
	int	state;
};

static struct adc_key_dev key_dev[MAX_KEY] = {
	[0] = {
		.name = "menu",
		.code = KEY_MENU,
		.ref_val = -1000,
		.state = 0,
	},
	[1] = {
		.name = "vol+",
		.code = KEY_VOLUMEUP,//KEY_VOLUMEUP,
		.ref_val = -1000,
		.state = 0,
	},
	[2] = {
		.name = "vol-",
		.code = KEY_VOLUMEDOWN,
		.ref_val = -1000,
		.state = 0,
	},
	[3] = {
		.name = "back",
		.code = KEY_BACK,
		.ref_val = -1000,
		.state = 0,
	},
	[4] = {
		.name = "camera",
		.code = KEY_CAMERA,
		.ref_val = -1000,
		.state = 0,
	}
	,
	[5] = {
		.name = "lock",
		.code = KEY_SCREENLOCK,
		.ref_val = 0x381,
		.state = 0,
	},
};

static void adc_write(u32 regaddr, u32 regdata)
{
	writel(regdata, regaddr);
}

static int adc_read(u32 regaddr)
{
	return readl(regaddr);
}

static u32 _emit_PRECHARGE(unsigned dry_run, volatile u32 buf[])
{
	if(dry_run)
		return SZ_PRECHARGE;

	buf[0] = CMD_PRE_CHARGE;
	ADC_DBGCMD_DUMP(SZ_PRECHARGE, "\tPRECHARGE\n");
	return SZ_PRECHARGE;
}

static u32 _emit_WPENIRQ(unsigned dry_run, volatile u32 buf[])
{
	u32 jmp_addr3 = (u32)&buf[3];
	u32 jmp_addr0 = (u32)&buf[0];

	if(dry_run)
		return SZ_WPENIRQ;
	jmp_addr3 -= (u32)(ADC_TSC_MEM_BASE_ADDR);
	jmp_addr0 -= (u32)(ADC_TSC_MEM_BASE_ADDR);
	ADC_LOG("#### wpenirq = 0x%x 0x%x\n", jmp_addr3, jmp_addr0);
#define WAIT_PEN_TIME		12000	
	/* touch screen detected  */
	buf[0] = CMD_TCD_ET | (1<<NML_TSINTR_BIT);
	// wait for pen_irq, cycle WAIT_PEN_TIME timer
	buf[1] = CMD_INSTR_JMP | CMD_JMP_COND | CMD_JMP_PENIRQ | IN_ADDR(jmp_addr3);
	buf[2] = CMD_INSTR_JMP | CMD_JMP_COND | CMD_JMP_REP | IN_ADDR(jmp_addr0) | (WAIT_PEN_TIME & JMP_CYCLE_MSK)<<JMP_CYCLE_BIT;
	
	// enter pre-charge state, goto end-code
	buf[3] = CMD_PRE_CHARGE;
	ADC_DBGCMD_DUMP(SZ_WPENIRQ, "\tWPENIRQ CYC=%u\n", WAIT_PEN_TIME);

	return SZ_WPENIRQ;
}

static u32 _emit_ENDCODE(unsigned dry_run, volatile u32 buf[])
{
	if(dry_run)
		return SZ_ENDCODE;

	buf[0] = CMD_INSTR_END;
	ADC_DBGCMD_DUMP(SZ_ENDCODE, "\tENDCODE\n");

	return SZ_ENDCODE;
}

static u32 _emit_CNSAMPLE(unsigned dry_run, volatile u32 buf[], u32 ch, int irq_en)
{
	if(dry_run)
		return SZ_CHSAMPLE;
	buf[0] = 0x7fe;
	buf[1] = CMD_CH_SAMPLE(ch) | (1<<NML_STCOV_BIT) | (1<<NML_AVG_BIT);
	buf[2] = CMD_CH_SAMPLE(ch) | (1<<NML_AVG_BIT);
	buf[3] = CMD_INSTR_NOP | (SAMPLE_DLYCLK & JMP_CYCLE_MSK);
	buf[4] = CMD_CH_SAMPLE(ch) | (1<<NML_ADC_OUT_BIT) | (1<<NML_AVG_BIT) | (1<<NML_CNINTR_BIT);
	buf[5] = (1<<NML_PPOUT_BIT) | (1<<NML_CNINTR_BIT);
	buf[6] = CMD_CH_SAMPLE(ch) | (1<<NML_ADC_OUT_BIT) | (0<<NML_AVG_BIT) | (1<<NML_CNINTR_BIT);
	buf[7] = 0x7fe; // | (1<<NML_CNINTR_BIT); 
	ADC_DBGCMD_DUMP(SZ_CHSAMPLE, "\tCHSAMPLE %u\n", ch);
	return SZ_CHSAMPLE;
}


static u32 _emit_XXSAMPLE(unsigned dry_run, volatile u32 buf[])
{
	if(dry_run)
		return SZ_XXSAMPLE;

	buf[0] = 0x7fe;
	buf[1] = CMD_XX_SAMPLE | (1<<NML_STCOV_BIT) | (1<<NML_AVG_BIT);	
	buf[2] = CMD_XX_SAMPLE | (1<<NML_AVG_BIT);
	buf[3] = CMD_INSTR_NOP | (SAMPLE_DLYCLK & JMP_CYCLE_MSK);
	buf[4] = CMD_XX_SAMPLE | (1<<NML_ADC_OUT_BIT) | (1<<NML_AVG_BIT);
	buf[5] = (1<<NML_PPOUT_BIT);
	buf[6] = CMD_XX_SAMPLE | (1<<NML_ADC_OUT_BIT) | (0<<NML_AVG_BIT);
	buf[7] = 0x7fe;

	ADC_DBGCMD_DUMP(SZ_XXSAMPLE, "\tXXSAMPLE\n");
	return SZ_XXSAMPLE;
}

static u32 _emit_YYSAMPLE(unsigned dry_run, volatile u32 buf[])
{
	if(dry_run)
		return SZ_YYSAMPLE;
	buf[0] = 0x7fe;
	buf[1] = CMD_YY_SAMPLE | (1<<NML_STCOV_BIT) | (1<<NML_AVG_BIT);
	buf[2] = CMD_YY_SAMPLE | (1<<NML_AVG_BIT);	
	buf[3] = CMD_INSTR_NOP | (SAMPLE_DLYCLK & JMP_CYCLE_MSK);
	buf[4] = CMD_YY_SAMPLE | (1<<NML_ADC_OUT_BIT) | (1<<NML_AVG_BIT);
	buf[5] = (1<<NML_PPOUT_BIT);
	buf[6] = CMD_YY_SAMPLE | (1<<NML_ADC_OUT_BIT) | (0<<NML_AVG_BIT);
	buf[7] = 0x7fe;

	ADC_DBGCMD_DUMP(SZ_YYSAMPLE, "\tYYSAMPLE\n");
	return SZ_YYSAMPLE;
}

static u32 _emit_YNSAMPLE(unsigned dry_run, volatile u32 buf[])
{
	if(dry_run)
		return SZ_YNSAMPLE;
	buf[0] = 0x7fe;
	buf[1] = CMD_YN_SAMPLE | (1<<NML_STCOV_BIT) | (1<<NML_AVG_BIT);	
	buf[2] = CMD_YN_SAMPLE | (1<<NML_AVG_BIT);
	buf[3] = CMD_INSTR_NOP | (SAMPLE_DLYCLK & JMP_CYCLE_MSK);
	buf[4] = CMD_YN_SAMPLE | (1<<NML_ADC_OUT_BIT) | (1<<NML_AVG_BIT);
	buf[5] = (1<<NML_PPOUT_BIT);
	buf[6] = CMD_YN_SAMPLE | (1<<NML_ADC_OUT_BIT) | (0<<NML_AVG_BIT);
	buf[7] = 0x7fe;

	ADC_DBGCMD_DUMP(SZ_YNSAMPLE, "\tYNSAMPLE\n");
	return SZ_YNSAMPLE;
}

static u32 _emit_XPSAMPLE(unsigned dry_run, volatile u32 buf[])
{
	if(dry_run)
		return SZ_XPSAMPLE;
	buf[0] = 0x7fe;
	buf[1] = CMD_XP_SAMPLE | (1<<NML_STCOV_BIT) | (1<<NML_AVG_BIT);	
	buf[2] = CMD_XP_SAMPLE | (1<<NML_AVG_BIT);
	buf[3] = CMD_INSTR_NOP | (SAMPLE_DLYCLK & JMP_CYCLE_MSK);
	buf[4] = CMD_XP_SAMPLE | (1<<NML_ADC_OUT_BIT) | (1<<NML_AVG_BIT);
	buf[5] = (1<<NML_PPOUT_BIT);
	buf[6] = CMD_XP_SAMPLE | (1<<NML_ADC_OUT_BIT) | (0<<NML_AVG_BIT);
	buf[7] = 0x7fe;

	ADC_DBGCMD_DUMP(SZ_XPSAMPLE, "\tXPSAMPLE\n");
	return SZ_XPSAMPLE;
}

static u32 _setup_entry_vector(unsigned dry_run, volatile u32 buf[], u32 data[])
{
	int i;

	ADC_LOG("buf[0] = %d\n", buf[0]);
	if(dry_run)
		return 8;
	for(i=0; i<8; i++)
		buf[i] = CMD_INSTR_JMP | CMD_JMP_UNCOND | data[i];
	return 8;
}

static int adc_ctrl_reset(void)
{
	module_enable(ADC_SYS_ADDR);
	
	return 0;
}

static uint32_t adc_channel_en(void)
{
	u32 ret = 0;

	#ifdef ADC_TS_EN
		ret |= 0x1;
	#endif
	if((ADC_KEY_ID > 0) && (ADC_KEY_ID < 12))
		ret |= 0x2;
	if((ADC_BATT_ID > 0) && (ADC_BATT_ID < 12))
		ret |= 0x4;
	if((ADC_JACK_ID > 0) && (ADC_JACK_ID < 12))
		ret |= 0x10;
	if((ADC_MICP_ID > 0) && (ADC_MICP_ID < 12))
		ret |= 0x20;
	if((ADC_MICN_ID > 0) && (ADC_MICN_ID < 12))
		ret |= 0x40;

	return ret;
}

static u32 addr;
static int _setup_req(unsigned dry_run)
{
	int i,off = 0;
	u32 task_start_addr[8];
	volatile u32 *buf = (volatile u32 *)(ADC_TSC_MEM_BASE_ADDR);
	
	ADC_LOG("_setup_req() \n");
	for(i=0;i<8;i++)
		task_start_addr[i] = 0x40000000;
	off = 0x8;
	/* task_0 : pre-charge + check irq */
	task_start_addr[0] = off;
	off += _emit_PRECHARGE(dry_run, &buf[off]);
	off += _emit_WPENIRQ(dry_run, &buf[off]);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_1: keyboard check */
	task_start_addr[1] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], ADC_KEY_ID, 1);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_2: battery + dc jack check */
	task_start_addr[2] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], ADC_BATT_ID, 0);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_3: touchscreen sample */
	task_start_addr[3] = off;
	off += _emit_XXSAMPLE(dry_run, &buf[off]);
	off += _emit_YYSAMPLE(dry_run, &buf[off]);
	off += _emit_YNSAMPLE(dry_run, &buf[off]);
	off += _emit_XPSAMPLE(dry_run, &buf[off]);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_4: jack check */
	task_start_addr[4] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], ADC_JACK_ID, 1);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_5: mic_p check */
	task_start_addr[5] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], ADC_MICP_ID, 1);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_6: mic_n check */
	task_start_addr[6] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], ADC_MICN_ID, 1);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* setup entry vector */	
	addr = ADC_TSC_MEM_BASE_ADDR;
	_setup_entry_vector(dry_run, (volatile u32 *)addr, &task_start_addr[0]);	

	return 0;
}


static int adc_ctrl_init(void)
{
	int i;
	u32 channel_en;
	u32 sel, div;
	ADC_LOG("adc_ctrl_init()\n");

	sel = readl(0x21e0a330);
	div = readl(0x21e0a334);
	writel(4, 0x21e0a330);
	writel(1, 0x21e0a334);
	//sel = *((unsigned int)*)(0x21e0a330);
	//div = *((unsigned int)*)(0x21e0a334);
	//*((unsigned int)*)(0x21e0a330) = 4;
	//*((unsigned int)*)(0x21e0a334) = 1;

	channel_en = adc_channel_en();
	ADC_LOG("channel_en = %d\n", channel_en);
	/* reset adc ctrl */
	adc_write(ADC_TSC_CNTL, (channel_en<<8));
	_setup_req(0);
	/* set task intel-timer */
	adc_write(ADC_TSC_EVTPAR0, 1000);
	adc_write(ADC_TSC_EVTPAR1, 1000);
	adc_write(ADC_TSC_EVTPAR2, 1000);
	adc_write(ADC_TSC_EVTPAR3, 1000);
	adc_write(ADC_TSC_EVTPAR4, 1000);
	adc_write(ADC_TSC_EVTPAR5, 1000);
	adc_write(ADC_TSC_EVTPAR6, 1000);
	adc_write(ADC_TSC_EVTPAR7, 1000);

	/* adc value calibration, (val-offest)*gain */
	adc_write(ADC_TSC_PP_PAR, 0x0|(0x80<<16));
	/* adc channel, range interrupt enable + channel number */
	adc_write(ADC_TSC_FIFO_TH, 4 | (((1<<ADC_KEY_ID)|(1<<ADC_JACK_ID)|(1<<ADC_MICP_ID)|(1<<ADC_MICN_ID))<<16));
	/* adc channel, rise_edge + fall_edge trigger */
	//adc_write(host, TSC_INT_EN, (1<<host->key_id)|(1<<(16+host->key_id)));

	/* adc channel_x, trigger range value, low-bit is max-value high-bit is min-value */
	adc_write(ADC_TSC_INT_PAR, 0x0390|(0x000<<16));
	/* adc channel interrupt mask */
	//adc_write(host, TSC_INT_MASK, 0x2bff);
	adc_write(ADC_TSC_INT_MASK, ~((1<<ADC_KEY_ID)|(1<<ADC_JACK_ID)|(1<<ADC_MICP_ID)|(1<<ADC_MICN_ID)|(0x1000)));

	/* adc clock source select */
	adc_write(ADC_TSC_PRES_SEL, 20<<8);

	/* setup task 0/1/2 */
	adc_write(ADC_TSC_CNTL, 0x13|(channel_en<<8));
	adc_write(ADC_TSC_CNTL, 0x13|(channel_en<<8));
	adc_write(ADC_TSC_CNTL, 0x17|(channel_en<<8));
	adc_write(ADC_TSC_CNTL, 0x11);
	
	for(i=0; i<800; i++);
	
	adc_write(ADC_TSC_CNTL, 0x13|(channel_en<<8));
	//*((unsigned int)*)(0x21e0a330) = sel;
	//*((unsigned int)*)(0x21e0a334) = div;
	writel(sel, 0x21e0a330);
	writel(div, 0x21e0a334);


	return 0;
}

static int get_pendown_state(void)
{
	u32 pen_state;

	pen_state = adc_read(ADC_TSC_INT_CLEAR);
	pen_state &= ADC_PEN_IRQ_MASK;
	
	return pen_state?0x01:0;
}

static int get_ch_state(u32 ch)
{
	u32 pen_state;

	pen_state = adc_read(ADC_TSC_INT_CLEAR);
	pen_state &= (1<<ch);
	if(pen_state)
		adc_write(ADC_TSC_INT_CLEAR, (1<<ch));
	return pen_state?1:0;
}

static void adc_pen_irq_en(int flag)
{
	u32 intr_mask;

	intr_mask = adc_read(ADC_TSC_INT_MASK);
	if(flag)
	{
		intr_mask &= ~ADC_PEN_IRQ_MASK;
	}
	else
	{
		intr_mask |= ADC_PEN_IRQ_MASK;
	}
	adc_write(ADC_TSC_INT_MASK, intr_mask);
}

static void adc_ch_irq_en(u32 ch, int flag)
{
	u32 intr_mask;

	intr_mask = adc_read(ADC_TSC_INT_MASK);
	if(flag)
	{
		intr_mask &= ~(1<<ch);
	}
	else
	{
		intr_mask |= (1<<ch);
	}
	adc_write(ADC_TSC_INT_MASK, intr_mask);
}

static u32 ts_calculate_pressure(struct ts_event *tc)
{
	u32 rt = 0;

	if(tc->xx == MAX_10BIT)
		tc->xx = 0;
	if(likely(tc->xx && tc->xp)) {
		rt = tc->yn - tc->xp;
		rt *= tc->xx;
		//rt *= host->x_plate_ohms;
		rt /= tc->xp;
		rt = (rt+512)>>10;
	}

	return rt;
}

static void ts_read_smaple(struct ts_event *tc)
{
	tc->xx = adc_read(ADC_TSC_PPOUT_0);
	tc->yy = adc_read(ADC_TSC_PPOUT_1);
	tc->xp = adc_read(ADC_TSC_PPOUT_12);
	tc->yn = adc_read(ADC_TSC_PPOUT_4);
}

static int battery_work_func(void)
{
	u32 reg_adc_data, reg_batt_data;

	reg_adc_data = adc_read(ADC_TSC_PPOUT_10);
	reg_batt_data = reg_adc_data * 5000/1024;
	ADC_LOG("[uboot-bat] adc: 0x%d, batt: 0x%d\n", reg_adc_data, reg_batt_data);

	return reg_adc_data;
}

static void ts_work_func(void)
{
	int i;
	struct ts_event tc;
	u32 rt;
	int debounced = 0;

again:
	if(get_pendown_state()) 
	{
		/* penirq irq detect */
		adc_write(ADC_TSC_CNTL, 0x813);
		for(i=0; i<10; i++); 
		adc_write(ADC_TSC_CNTL, 0x713);
		/* read sample value */
		ts_read_smaple(&tc);
		rt = ts_calculate_pressure(&tc);
		if (rt > g_max_rt) {
			//debounced = 0x01;
			goto out;
		}
		if(rt) {
			if(!g_pendown)
			{
				//input_report_key(input, BTN_TOUCH, 1);
				g_pendown = 0x01;
			}
		}
	}	
	else 
	{
		/* pen-up state */
		g_pendown = 0;
	}

out:
	if(g_pendown || debounced)
	{
		udelay(10000);
		goto again;
	}

	return ;
}

static int get_key_index(u32 sample_value)
{
	int i, ret=-1;
	int val = sample_value;

	for(i=0; i<MAX_KEY; i++)
	{
		if((val > (key_dev[i].ref_val - KEY_CAL_VAL)) && (val < (key_dev[i].ref_val + KEY_CAL_VAL)))
		{
			ret = i;
			break;
		}
	}

	return ret;
}

static void key_work_func(void)
{
	u32 sample_value;
	int debounced = 0;
	int idx;

again:
	if(get_ch_state(ADC_KEY_ID))
	{
		sample_value = adc_read(ADC_TSC_PPOUT_0 + ADC_KEY_ID * 4);
		ADC_LOG("sample_value:0x%x\n", sample_value);

		ADC_LOG("key down\n");
		if(!g_keydown) {
			g_keydown = 0x01;
			idx = get_key_index(sample_value);
			ADC_LOG("idx=%d\n", idx);
			if(idx == -1)
			{
				debounced = 0x01;
				goto out;
			}
			key_dev[idx].state = 0x01;
			//ADC_LOG("key down: 0x%x %s\n", sample_value, key_dev[idx].name);
		}
	}
	else 
	{
		g_keydown = 0;
		for(idx=0; idx<MAX_KEY; idx++)
		{
			if(key_dev[idx].state == 0x01)
			{
				key_dev[idx].state = 0;
				//ADC_LOG("key up: %s\n", key_dev[idx].name);
			}
		}
	}

out:
	if(g_keydown || debounced)
	{
		udelay(10000);// 10ms
		goto again;
	}
}



static int jack_work_func(void)
{
	if(get_ch_state(ADC_JACK_ID))
	{
		return 1;
	}
	else {
		return 0;
	}
}

static int micp_work_func(void)
{
	if(get_ch_state(ADC_MICP_ID))
	{
		return 1;
	}
	else {
		return 0;
	}
}

static int micn_work_func(void)
{
	if(get_ch_state(ADC_MICN_ID))
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

#if 0
static void adc_irq_polling(void)
{
	u32 intr_state, intr_mask, intr_pend;

	intr_state = adc_read(ADC_TSC_INT_CLEAR);
	intr_mask = adc_read(ADC_TSC_INT_MASK);

	intr_pend = intr_state & (~intr_mask);
	//ADC_LOG("intr_pend:0x%x\n", intr_pend);
	
	/* get pen_down state */
	if(intr_pend & ADC_PEN_IRQ_MASK)
	{
		/* disable pen-irq interrupt */
		adc_pen_irq_en(0);
		ts_work_func();
	}

	/* key interrupt */
	if(intr_pend & ADC_KEY_IRQ_MASK)
	{
		/* disable key-irq interrup */
		adc_write(ADC_TSC_INT_CLEAR, ADC_KEY_IRQ_MASK);
		adc_ch_irq_en(ADC_KEY_IRQ_MASK, 0);
		key_work_func();
	}
	
	/* jack interrupt */
	if(intr_pend & ADC_JACK_IRQ_MASK)
	{
		/* disable jack-irq interrupt */
		adc_write(ADC_TSC_INT_CLEAR, ADC_JACK_IRQ_MASK);
		adc_ch_irq_en(ADC_JACK_IRQ_MASK, 0);
		jack_work_func();
	}

	/* mic_p interrupt */
	if(intr_pend & ADC_MICP_IRQ_MASK)
	{
		/* disable micp-irq interrupt */
		adc_write(ADC_TSC_INT_CLEAR, ADC_MICP_IRQ_MASK);
		adc_ch_irq_en(ADC_MICP_IRQ_MASK, 0);
		micp_work_func();
	}

	/* mic_n interrupt */
	if(intr_pend & ADC_MICN_IRQ_MASK)
	{
		/* disable micp-irq interrupt */
		adc_write(ADC_TSC_INT_CLEAR, ADC_MICN_IRQ_MASK);
		adc_ch_irq_en(ADC_MICN_IRQ_MASK, 0);
		micn_work_func();
	}

	if(intr_pend & ADC_BATT_IRQ_MASK)
	{
		adc_write(ADC_TSC_INT_CLEAR, ADC_BATT_IRQ_MASK);
		adc_ch_irq_en(ADC_MICN_IRQ_MASK, 0);
		g_battery_adc = battery_work_func();
	}
}
#endif

void adc_debug(void)
{
	int tst, addr;
	ADC_LOG("adc_debug()");

	tst = readl(ADC_TSC_CNTL);
	ADC_LOG("ADC_TSC_CNTL = %d\n", tst);
	tst = readl(ADC_TSC_INT_EN);
	ADC_LOG("ADC_TSC_INT_EN = %d\n", tst);

	tst = readl(ADC_TSC_INT_MASK);
	ADC_LOG("ADC_TSC_INT_MASK = %d\n", tst);

	tst = readl(ADC_TSC_INT_CLEAR);
	ADC_LOG("ADC_TSC_INT_CLEAR = %d\n", tst);

}

int adc_batt_run(void)
{
	int state, count, intr_state, reg_val;

	ADC_LOG("adc_batt_run() \n");
	count = 0;
	state = 1;

	adc_debug();

	adc_ch_irq_en(ADC_BATT_ID, 1);
	udelay(10000);
	
	do
	{
		intr_state = adc_read(ADC_TSC_INT_CLEAR);
		ADC_LOG("intr_state = 0x%x\n", intr_state);
		//if(intr_state & ADC_BATT_IRQ_MASK)
		//{
			//adc_write(ADC_TSC_INT_CLEAR, ADC_BATT_IRQ_MASK);
			reg_val = battery_work_func();
			state = 0;
		//}
		//else
		//{
		//	udelay(1000);
		//	count ++;
		//}
		
	}while((state) && (count <=10));
	adc_ch_irq_en(ADC_BATT_ID, 0);
	
	ADC_LOG("reg_val = %d, state=%d, count=%d\n", reg_val, state, count);
	return reg_val;
}


void adc_init(void)
{
	int val, tst;
	char buf[ITEM_MAX_LEN];
	
	ADC_LOG("adc_init() \n");
	//commit: clock has init in spl part
	tst = readl(ADC_CLK_BASE);
	ADC_LOG("tst clock addr is 0x%x = 0x%x\n", ADC_CLK_BASE, tst);
	tst = readl(ADC_CLK_BASE+4);
	ADC_LOG("tst clock addr is 0x%x = 0x%x\n", ADC_CLK_BASE+4, tst);
	tst = readl(ADC_CLK_BASE+8);
	ADC_LOG("tst clock addr is 0x%x = 0x%x\n", ADC_CLK_BASE+8, tst);
	tst = readl(ADC_CLK_BASE+12);
	ADC_LOG("tst clock addr is 0x%x = 0x%x\n", ADC_CLK_BASE+12, tst);
	
	if(item_exist("keys.menu"))
	{
		item_string(buf, "keys.menu", 0);
		if(!strncmp(buf,"adc",3))
		{
		val = item_integer("keys.menu",1);
		key_dev[0].ref_val = val*41;
		}
	}
	if(item_exist("keys.volup"))
	{
		item_string(buf, "keys.volup", 0);
		if(!strncmp(buf,"adc",3))
		{
			val  = item_integer("keys.volup",1);
			key_dev[1].ref_val = val*41;
		}
        }
        if(item_exist("keys.voldown"))
        {
		item_string(buf, "keys.voldown", 0);
		if(!strncmp(buf,"adc",3))
		{
			val = item_integer("keys.voldown",1);
			key_dev[2].ref_val = val*41;
		}
	}
	if(item_exist("keys.back"))
	{
		item_string(buf, "keys.back", 0);
		if(!strncmp(buf,"adc",3))
		{
			val = item_integer("keys.back",1);
                	key_dev[3].ref_val = val*41;
		}
	}
	if(item_exist("keys.camera"))
	{
		item_string(buf, "keys.camera", 0);
		if(!strncmp(buf,"adc",3))
		{
               	val = item_integer("keys.camera",1);
			ADC_LOG(">>>>>>>>imapx800-ads--key.camera");
			key_dev[4].ref_val = val*41;
		}		
	}

	adc_ctrl_reset();
	udelay(1000);
	adc_ctrl_init();
	udelay(1000);
}

int battery_get_value(void)
{
	return adc_batt_run();
}
