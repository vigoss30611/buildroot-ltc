#include <linux/device.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/syscalls.h>
#include <asm/io.h>
#include <mach/power-gate.h>
#include <mach/items.h>
#include <mach/imap-adc.h>
#include <mach/clk.h>

//#define ADC_DBG_OUT
//#define ADC_DBGCMD_MCGEN

#ifdef ADC_DBG_OUT
#define ADC_LOG(x...)   printk(KERN_ERR "ADC DEBUG:" x)
#else
#define ADC_LOG(x...)
#endif

static int g_battery_value;

#ifdef ADC_DBGCMD_MCGEN
static unsigned cmd_line;
#define ADC_DBGCMD_DUMP(off, x...)  do { \
					printk("%x:", cmd_line); \
					printk(x); \
					cmd_line += off;\
				} while(0)
#define ADC_DBGCMD_START(addr)  (cmd_line = addr)
#else
#define ADC_DBGCMD_DUMP(off, x...) do {} while(0)
#define ADC_DBGCMD_START(addr) do {} while(0)
#endif

#define MAX_10BIT                       ((1 << 10) - 1)

/*
 *   adc cmd for send wave
 */

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

#define PEN_IRQ_MASK			0x1000
#define KEY_IRQ_MASK			(1<<(host->key_id))//0x400
#define RESET_IRQ_MASK          (1<<(host->reset_id))
#define MAX_KEY				6
#define KEY_VAL_CAL			10

struct adc_ctrl {
	spinlock_t              lock;
        struct clk              *clk;
	void __iomem            *regs;
	struct input_dev	*input;
	struct delayed_work	ts_work;
	struct delayed_work     key_work;
	struct delayed_work	battery_work;
	struct delayed_work     jack_work;
	struct delayed_work     micp_work;
	struct delayed_work     micn_work;
    struct delayed_work     reset_work;

	u32			max_rt;
	u32	 		ts_poll_delay;
	u32	 		ts_poll_period;
	bool			pendown;

	u32	 		key_poll_delay;
	u32	 		key_poll_period;
	bool			keydown;
	int			key_cal_value;

    u32         reset_poll_delay;
    u32         reset_poll_period;
    bool        resetdown;
    int         reset_cal_value;

	u32	 		battery_poll_delay;
	u32	 		battery_poll_period;

	u32	 		jack_poll_delay;
	u32	 		jack_poll_period;
	bool			jack_plugin;

	u32	 		micp_poll_delay;
	u32	 		micp_poll_period;
	bool			micp_down;

	u32	 		micn_poll_delay;
	u32	 		micn_poll_period;
	bool			micn_down;

	u32			ts_en;
	u32			key_id;
    u32         reset_id;
	u32			battery_id;
	u32			jack_id;
	u32			micp_id;
	u32			micn_id;
};

struct ts_event {
	u16     xx;
	u16     yy;
	u16     xp;
       	u16	yn;
};

struct adc_key_dev {
	char	name[16];
	int	code;
	int	ref_val;
	bool	state;
};


#define adc_readl(dev, reg)	\
	__raw_readl((dev)->regs + reg)
#define adc_writel(dev, reg, value)	\
	__raw_writel((value), (dev)->regs + reg)
static struct adc_key_dev reset_dev = {
        .name = "reset",
        .code = KEY_ZOOMRESET,
        .ref_val = -1000,
        .state = false,
};

static struct adc_key_dev key_dev[MAX_KEY] = {
	[0] = {
		.name = "menu",
		.code = KEY_MENU,
		.ref_val = -1000,
		.state = false,
	},
	[1] = {
		.name = "vol+",
		.code = KEY_VOLUMEUP,//KEY_VOLUMEUP,
		.ref_val = -1000,
		.state = false,
	},
	[2] = {
		.name = "vol-",
		.code = KEY_VOLUMEDOWN,
		.ref_val = -1000,
		.state = false,
	},
	[3] = {
		.name = "back",
		.code = KEY_BACK,
		.ref_val = -1000,
		.state = false,
	},
	[4] = {
		.name = "camera",
		.code = KEY_CAMERA,
		.ref_val = -1000,
		.state = false,
	}
	,
	[5] = {
		.name = "lock",
		.code = KEY_SCREENLOCK,
		.ref_val = 0x381,
		.state = false,
	},
};


static u32 _emit_PRECHARGE(unsigned dry_run, volatile u32 buf[])
{
	if(dry_run)
		return SZ_PRECHARGE;

	buf[0] = CMD_PRE_CHARGE;
	ADC_DBGCMD_DUMP(SZ_PRECHARGE, "\tPRECHARGE\n");
	return SZ_PRECHARGE;
}

static u32 _emit_WPENIRQ(struct adc_ctrl *host, unsigned dry_run, volatile u32 buf[])
{
	u32 jmp_addr3 = (u32)&buf[3];
	u32 jmp_addr0 = (u32)&buf[0];

	if(dry_run)
		return SZ_WPENIRQ;
	jmp_addr3 -= (u32)(host->regs + TSC_MEM_BASE_ADDR);
	jmp_addr0 -= (u32)(host->regs + TSC_MEM_BASE_ADDR);
	printk("#### wpenirq = 0x%x 0x%x\n", jmp_addr3, jmp_addr0);
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

	ADC_LOG("buf[0]=%d\n", buf[0]);
	if(dry_run)
		return 8;
	for(i=0; i<8; i++)
		buf[i] = CMD_INSTR_JMP | CMD_JMP_UNCOND | data[i];
	return 8;
}

static int _setup_req(struct adc_ctrl *host, unsigned dry_run)
{
	int i,off = 0;
	u32 task_start_addr[8];
	volatile u32 *buf = (volatile u32 *)(host->regs + TSC_MEM_BASE_ADDR);

	for(i=0;i<8;i++)
		task_start_addr[i] = 0x40000000;
	off = 0x8;
	/* task_0 : pre-charge + check irq */
	task_start_addr[0] = off;
	off += _emit_PRECHARGE(dry_run, &buf[off]);
	off += _emit_WPENIRQ(host, dry_run, &buf[off]);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_1: keyboard check */
	task_start_addr[1] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], host->key_id, 1);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_2: battery + dc jack check */
	task_start_addr[2] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], host->battery_id, 0);
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
	off += _emit_CNSAMPLE(dry_run, &buf[off], host->jack_id, 1);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_5: mic_p check */
	task_start_addr[5] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], host->micp_id, 1);
	off += _emit_ENDCODE(dry_run, &buf[off]);

	/* task_6: mic_n check */
	task_start_addr[6] = off;
	off += _emit_CNSAMPLE(dry_run, &buf[off], host->micn_id, 1);
	off += _emit_ENDCODE(dry_run, &buf[off]);

    /* task_7: reset check */
    task_start_addr[7] = off;
    off += _emit_CNSAMPLE(dry_run, &buf[off], host->reset_id, 1);
    off += _emit_ENDCODE(dry_run, &buf[off]);

	/* setup entry vector */	
	_setup_entry_vector(dry_run, (volatile u32 *)(host->regs + TSC_MEM_BASE_ADDR), &task_start_addr[0]);	

	return 0;
}

static int adc_ctrl_reset(struct adc_ctrl *host)
{
	module_power_on(SYSMGR_ADC_BASE);

	return 0;
}

static uint32_t adc_channel_en(struct adc_ctrl *host)
{
	u32 ret = 0;

	if(host->ts_en)
		ret |= 0x1;
	if((host->key_id > 0) && (host->key_id < 12))
		ret |= 0x2;
	if((host->battery_id > 0) && (host->battery_id < 12))
		ret |= 0x4;
	if((host->jack_id > 0) && (host->jack_id < 12))
		ret |= 0x10;
	if((host->micp_id > 0) && (host->micp_id < 12))
		ret |= 0x20;
	if((host->micn_id > 0) && (host->micn_id < 12))
		ret |= 0x40;
    if((host->reset_id > 0) && (host->reset_id < 12))
        ret |= 0x80;

	return ret;
}


static int adc_ctrl_init(struct adc_ctrl *host)
{
	int i;
	u32 channel_en;

	channel_en = adc_channel_en(host);
	/* reset adc ctrl */
	adc_writel(host, TSC_CNTL, (channel_en<<8));
	_setup_req(host, 0);
	/* set task intel-timer */
	adc_writel(host, TSC_EVTPAR0, 1000);
	adc_writel(host, TSC_EVTPAR1, 1000);
	adc_writel(host, TSC_EVTPAR2, 1000);
	adc_writel(host, TSC_EVTPAR3, 1000);
	adc_writel(host, TSC_EVTPAR4, 1000);
	adc_writel(host, TSC_EVTPAR5, 1000);
	adc_writel(host, TSC_EVTPAR6, 1000);
	adc_writel(host, TSC_EVTPAR7, 1000);

	/* adc value calibration, (val-offest)*gain */
	adc_writel(host, TSC_PP_PAR, 0x0|(0x80<<16));
	/* adc channel, range interrupt enable + channel number */
	adc_writel(host, TSC_FIFO_TH, 4 | (((1<<host->key_id)|(1<<host->reset_id)|(1<<host->jack_id)|(1<<host->micp_id)|(1<<host->micn_id))<<16));
	/* adc channel, rise_edge + fall_edge trigger */
	//adc_writel(host, TSC_INT_EN, (1<<host->key_id)|(1<<(16+host->key_id)));

	/* adc channel_x, trigger range value, low-bit is max-value high-bit is min-value */
	adc_writel(host, TSC_INT_PAR, 0x0390|(0x000<<16));
    //FIXME  add by peter to suport I15
    if(item_equal("board.cpu", "x15", 0))
    {
        adc_writel(host, TSC_INT_PAR1, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR2, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR3, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR4, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR5, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR6, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR7, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR8, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR9, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR10, 0x0390|(0x000<<16));
        adc_writel(host, TSC_INT_PAR11, 0x0390|(0x000<<16));
    }
	/* adc channel interrupt mask */
	//adc_writel(host, TSC_INT_MASK, 0x2bff);
	adc_writel(host, TSC_INT_MASK, ~((1<<host->key_id)|(1<<host->reset_id)|(1<<host->jack_id)|(1<<host->micp_id)|(1<<host->micn_id)|(0x1000)));
    //adc_writel(host, TSC_INT_MASK, ~((1<<host->key_id)|(1<<host->jack_id)|(1<<host->micp_id)|(1<<host->micn_id)|(0x1000)));
	/* adc clock source select */
	adc_writel(host, TSC_PRES_SEL, 20<<8);

	/* setup task 0/1/2 */
	adc_writel(host, TSC_CNTL, 0x13|(channel_en<<8));
	adc_writel(host, TSC_CNTL, 0x13|(channel_en<<8));
	adc_writel(host, TSC_CNTL, 0x17|(channel_en<<8));
	adc_writel(host, TSC_CNTL, 0x11);
	for(i=0; i<800; i++);
	adc_writel(host, TSC_CNTL, 0x13|(channel_en<<8));
	
	return 0;
}

static bool get_pendown_state(struct adc_ctrl *host)
{
	u32 pen_state;

	pen_state = adc_readl(host, TSC_INT_CLEAR);
	pen_state &= PEN_IRQ_MASK;
	
	return pen_state?true:false;
}

static int get_ch_state(struct adc_ctrl *host, u32 ch)
{
	u32 pen_state;

	pen_state = adc_readl(host, TSC_INT_CLEAR);
	pen_state &= (1<<ch);
	if(pen_state)
		adc_writel(host, TSC_INT_CLEAR, (1<<ch));
	return pen_state?1:0;
}


static void ts_send_up_event(struct adc_ctrl *host)
{
	struct input_dev *input = host->input;

	input_report_key(input, BTN_TOUCH, 0);
	input_report_abs(input, ABS_PRESSURE, 0);
	input_sync(input);	
}

static void adc_pen_irq_en(struct adc_ctrl *host, int flag)
{
	u32 intr_mask;

	intr_mask = adc_readl(host, TSC_INT_MASK);
	if(flag)
	{
		intr_mask &= ~PEN_IRQ_MASK;
	}
	else
	{
		intr_mask |= PEN_IRQ_MASK;
	}
	adc_writel(host, TSC_INT_MASK, intr_mask);
}

static void adc_ch_irq_en(struct adc_ctrl *host, u32 ch, int flag)
{
	u32 intr_mask;

	intr_mask = adc_readl(host, TSC_INT_MASK);
	if(flag)
	{
		intr_mask &= ~(1<<ch);
	}
	else
	{
		intr_mask |= (1<<ch);
	}
	adc_writel(host, TSC_INT_MASK, intr_mask);
}

static u32 ts_calculate_pressure(struct adc_ctrl *host, struct ts_event *tc)
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

static void ts_read_smaple(struct adc_ctrl *host, struct ts_event *tc)
{
	tc->xx = adc_readl(host, TSC_PPOUT_0);
	tc->yy = adc_readl(host, TSC_PPOUT_1);
	tc->xp = adc_readl(host, TSC_PPOUT_12);
	tc->yn = adc_readl(host, TSC_PPOUT_4);
}

static void battery_work_func(struct work_struct *work)
{
	struct adc_ctrl *host = container_of(to_delayed_work(work), struct adc_ctrl, battery_work);

	g_battery_value = adc_readl(host, TSC_PPOUT_10);
	//printk("bat: 0x%x\n", g_battery_value);

	schedule_delayed_work(&host->battery_work, msecs_to_jiffies(host->battery_poll_period));
}

static void ts_work_func(struct work_struct *work)
{
	struct adc_ctrl *host = container_of(to_delayed_work(work), struct adc_ctrl, ts_work);
	int i;
	struct ts_event tc;
	u32 rt;
	bool debounced = false;


	if(get_pendown_state(host)) 
	{
		/* penirq irq detect */
		adc_writel(host, TSC_CNTL, 0x813);
		for(i=0; i<10; i++); 
		adc_writel(host, TSC_CNTL, 0x713);
		/* read sample value */
		ts_read_smaple(host, &tc);
		rt = ts_calculate_pressure(host, &tc);
		if (rt > host->max_rt) {
			debounced = true;
			goto out;
		}
		if(rt) {
			struct input_dev *input = host->input;	
			
			if(!host->pendown) {
				input_report_key(input, BTN_TOUCH, 1);
				host->pendown = true;
			}
			input_report_abs(input, ABS_X, tc.xx);
			input_report_abs(input, ABS_Y, tc.yy);
			input_report_abs(input, ABS_PRESSURE, rt);

			input_sync(input);
		}
	}	
	else {
		/* pen-up state */
		host->pendown = false;
		ts_send_up_event(host);
	}

out:
	if(host->pendown || debounced)
	{
		schedule_delayed_work(&host->ts_work, msecs_to_jiffies(host->ts_poll_period));
	}
	else
	{
		adc_pen_irq_en(host, 1);
	}

	return ;
}

static int get_key_index(struct adc_ctrl *host, u32 sample_value)
{
	int i, ret=-1;
	int val = sample_value;

	for(i=0; i<MAX_KEY; i++)
	{
		if((val > (key_dev[i].ref_val - host->key_cal_value)) && (val < (key_dev[i].ref_val + host->key_cal_value)))
		{
			ret = i;
			break;
		}
	}

	return ret;
}

static int get_reset_index(struct adc_ctrl *host, u32 sample_value)
{
    int ret = -1;

    if ((sample_value > (reset_dev.ref_val - host->reset_cal_value)) && (sample_value < (reset_dev.ref_val + host->reset_cal_value)))
        ret = 0;
    
    return ret;
}

void __attribute__((weak)) imapfb_shutdown(void)
{
    printk("imapfb_shutdown dummy func \n");
}
static void reset_work_func(struct work_struct *work) 
{
    struct adc_ctrl *host = container_of(to_delayed_work(work), struct adc_ctrl, reset_work);
    u32 sample_value;
    bool debounced = false;
    int idx;
    
    printk("reset_work_func\n");
    if(get_ch_state(host, host->reset_id))
    {
        sample_value = adc_readl(host, (TSC_PPOUT_0 + host->reset_id * 4));
        ADC_LOG("sample_value:0x%x\n", sample_value);

        if (!host->resetdown)
        {
		host->resetdown = true;
		idx = get_reset_index(host, sample_value);
		if (idx == -1) {
			debounced = true;
			goto out;
		}
        }

	reset_dev.state = true;
	ADC_LOG("#### 0001\n");
	input_event(host->input, EV_KEY, reset_dev.code, 1);
	ADC_LOG("#### 0002\n");
    } else {
        host->resetdown = false;
        if(reset_dev.state == true)
        {
            reset_dev.state = false;
            input_event(host->input, EV_KEY, reset_dev.code, 0);
            input_sync(host->input);
            printk("key up: %s\n", reset_dev.name);

	    /* the reset functions */
	    sys_sync();
	    imapfb_shutdown();
	    __raw_writel(0x2, IO_ADDRESS(SYSMGR_RTC_BASE));        
	    __raw_writel(0xff, IO_ADDRESS(SYSMGR_RTC_BASE + 0x28));
	    __raw_writel(0, IO_ADDRESS(SYSMGR_RTC_BASE + 0x7c));   
        }
    }
    printk("xxxxxxxxxxxxxxxxxxx\n");

out:
    if(host->resetdown || debounced)
    {
        schedule_delayed_work(&host->reset_work, msecs_to_jiffies(host->reset_poll_period));
    }
    else
    {
        adc_ch_irq_en(host, host->reset_id, 1);
    }
}

static void key_work_func(struct work_struct *work)
{
	struct adc_ctrl *host = container_of(to_delayed_work(work), struct adc_ctrl, key_work);
	u32 sample_value;
	bool debounced = false;
	int idx;

	if(get_ch_state(host, host->key_id))
	{
		sample_value = adc_readl(host, (TSC_PPOUT_0 + host->key_id * 4));
		ADC_LOG("sample_value:0x%x\n", sample_value);

		ADC_LOG("key down\n");
		if(!host->keydown) {
			host->keydown = true;
			idx = get_key_index(host, sample_value);
			ADC_LOG("idx=%d\n", idx);
			if(idx == -1)
			{
				debounced = true;
				goto out;
			}
			key_dev[idx].state = true;
			ADC_LOG("#### 0001\n");
			input_event(host->input, EV_KEY, key_dev[idx].code, 1);
			ADC_LOG("#### 0002\n");
			input_sync(host->input);
			//printk("key down: 0x%x %s\n", sample_value, key_dev[idx].name);
		}
	}
	else {
		host->keydown = false;
		for(idx=0; idx<MAX_KEY; idx++)
		{
			if(key_dev[idx].state == true)
			{
				key_dev[idx].state = false;
				input_event(host->input, EV_KEY, key_dev[idx].code, 0);
				input_sync(host->input);
				//printk("key up: %s\n", key_dev[idx].name);
			}
		}
	}

out:
	if(host->keydown || debounced)
	{
		schedule_delayed_work(&host->key_work, msecs_to_jiffies(host->key_poll_period));
	}
	else
	{
		adc_ch_irq_en(host, host->key_id, 1);
	}
}

static int battery_get_value(void)
{
    	//printk("g_battery_value:%d in %s\n",g_battery_value,__func__);
	return g_battery_value;
}

static void jack_work_func(struct work_struct *work)
{
	struct adc_ctrl *host = container_of(to_delayed_work(work), struct adc_ctrl, jack_work);
	bool debounced = false;
	
	if(get_ch_state(host, host->jack_id))
	{
		if(!host->jack_plugin) {
			host->jack_plugin = true;
		}
	}
	else {
		host->jack_plugin = false;
	}

	if(host->jack_plugin || debounced)
	{
		schedule_delayed_work(&host->jack_work, msecs_to_jiffies(host->jack_poll_period));
	}
	else
	{
		adc_ch_irq_en(host, host->jack_id, 1);
	}
}

static void micp_work_func(struct work_struct *work)
{
	struct adc_ctrl *host = container_of(to_delayed_work(work), struct adc_ctrl, micp_work);
	bool debounced = false;

	if(get_ch_state(host, host->micp_id))
	{
		if(!host->micp_down) {
			host->micp_down = true;
		}
	}
	else {
		host->micp_down = false;
	}

	if(host->micp_down || debounced)
	{
		schedule_delayed_work(&host->micp_work, msecs_to_jiffies(host->micp_poll_period));
	}
	else
	{
		adc_ch_irq_en(host, host->micp_id, 1);
	}

}

static void micn_work_func(struct work_struct *work)
{
	struct adc_ctrl *host = container_of(to_delayed_work(work), struct adc_ctrl, micn_work);
	bool debounced = false;

	if(get_ch_state(host, host->micn_id))
	{
		if(!host->micn_down) {
			host->micn_down = true;
		}
	}
	else {
		host->micn_down = false;
	}

	if(host->micn_down || debounced)
	{
		schedule_delayed_work(&host->micn_work, msecs_to_jiffies(host->micn_poll_period));
	}
	else
	{
		adc_ch_irq_en(host, host->micn_id, 1);
	}

}

static irqreturn_t adc_irq_handle(int irq, void *dev_id)
{
	struct adc_ctrl *host = dev_id;
	u32 intr_state, intr_mask, intr_pend;

	intr_state = adc_readl(host, TSC_INT_CLEAR);
	intr_mask = adc_readl(host, TSC_INT_MASK);

	intr_pend = intr_state & (~intr_mask);
	//printk("intr_pend:0x%x\n", intr_pend);
	/* get pen_down state */
	if(intr_pend & PEN_IRQ_MASK)
	{
		/* disable pen-irq interrupt */
		adc_pen_irq_en(host, 0);
		schedule_delayed_work(&host->ts_work, msecs_to_jiffies(host->ts_poll_delay));
	}

	/* key interrupt */
	if(intr_pend & KEY_IRQ_MASK)
	{
		/* disable key-irq interrup */
		adc_writel(host, TSC_INT_CLEAR, KEY_IRQ_MASK);
		adc_ch_irq_en(host, host->key_id, 0);
		schedule_delayed_work(&host->key_work, msecs_to_jiffies(host->key_poll_delay));
	}
	
	/* jack interrupt */
	if(intr_pend & (1<<host->jack_id))
	{
		/* disable jack-irq interrupt */
		adc_writel(host, TSC_INT_CLEAR, (1<<host->jack_id));
		adc_ch_irq_en(host, host->jack_id, 0);
		schedule_delayed_work(&host->jack_work, msecs_to_jiffies(host->jack_poll_delay));
	}

	/* mic_p interrupt */
	if(intr_pend & (1<<host->micp_id))
	{
		/* disable micp-irq interrupt */
		adc_writel(host, TSC_INT_CLEAR, (1<<host->micp_id));
		adc_ch_irq_en(host, host->micp_id, 0);
		schedule_delayed_work(&host->micp_work, msecs_to_jiffies(host->micp_poll_delay));
	}

	/* mic_n interrupt */
	if(intr_pend & (1<<host->micn_id))
	{
		/* disable micp-irq interrupt */
		adc_writel(host, TSC_INT_CLEAR, (1<<host->micn_id));
		adc_ch_irq_en(host, host->micn_id, 0);
		schedule_delayed_work(&host->micn_work, msecs_to_jiffies(host->micn_poll_delay));
	}

    if (intr_pend & RESET_IRQ_MASK)
    {
        adc_writel(host, TSC_INT_CLEAR, RESET_IRQ_MASK);                                 
        adc_ch_irq_en(host, host->reset_id, 0);                                          
        schedule_delayed_work(&host->reset_work, msecs_to_jiffies(host->reset_poll_delay));
    }

	return IRQ_HANDLED;
}

static int adc_clk_gen_init(int en)
{
	void __iomem *clk_gen = IO_ADDRESS(SYSMGR_CLKGEN_BASE);

	if (en) {
		writel(0x4, clk_gen + BUS_CLK_SRC(32));
		writel(0x1, clk_gen + BUS_CLK_DIVI_R(32));
		writel(0, clk_gen + BUS_CLK_GATE(32));
	} else {
		writel(1, clk_gen + BUS_CLK_GATE(32));
	}

	return 0;
}

static int adc_imap_probe(struct platform_device *pdev)
{
	struct resource *regs; 
	int irq, ret, idx, ii;
	struct input_dev *input;
	struct adc_ctrl *host;
	char buf[ITEM_MAX_LEN];

	ADC_LOG("++ %s ++\n", __func__);
	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs) 
		return -ENXIO;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	host = kzalloc(sizeof(struct adc_ctrl), GFP_KERNEL);
	if(!host)
		return -ENOMEM;

	spin_lock_init(&host->lock);
	ret = -ENOMEM;
	host->regs = ioremap(regs->start, resource_size(regs));
	if (!host->regs)
		goto err_freehost;

	input = input_allocate_device();
	if(!input) {
		ret = -ENOMEM;
		goto err_unmap;
        }

        /*host->clk = clk_get(NULL, "touch screen");
        ret = clk_enable(host->clk);

        if(ret) {
            printk("touch screen clk disable \n");
            goto err_unmap;
        }*/

	adc_clk_gen_init(1);

        if(item_exist("keys.reset")) {
            item_string(buf, "keys.reset", 0);
            if (!strncmp(buf,"adc",3)) {
                ii = item_integer("keys.reset", 2);
                reset_dev.ref_val = ii*41;

                host->reset_id = item_integer("keys.reset",1);
                INIT_DELAYED_WORK(&host->reset_work, reset_work_func);
                host->reset_poll_delay = 10;
                host->reset_poll_period = 100;
                host->resetdown = false;
                host->reset_cal_value = 50;
            }
        }

        if(item_exist("keys.menu"))
        {
			item_string(buf, "keys.menu", 0);
            if(!strncmp(buf,"adc",3))
            {
				ii = item_integer("keys.menu",2);
                key_dev[0].ref_val = ii*41;
			}
        }
        if(item_exist("keys.volup"))
        {
			item_string(buf, "keys.volup", 0);
            if(!strncmp(buf,"adc",3))
            {
                ii  = item_integer("keys.volup",2);
                key_dev[1].ref_val = ii*41;
			}
        }
        if(item_exist("keys.voldown"))
        {
			item_string(buf, "keys.voldown", 0);
            if(!strncmp(buf,"adc",3))
            {
                ii = item_integer("keys.voldown",2);
                key_dev[2].ref_val = ii*41;
            }
        }
        if(item_exist("keys.back"))
        {
			item_string(buf, "keys.back", 0);
            if(!strncmp(buf,"adc",3))
            {
                ii = item_integer("keys.back",2);
                key_dev[3].ref_val = ii*41;
            }
        }
        if(item_exist("keys.camera"))
        	{
        item_string(buf, "keys.camera", 0);
            if(!strncmp(buf,"adc",3))
            {
                ii = item_integer("keys.camera",2);
                printk(">>>>>>>>imapx800-ads--key.camera");
                key_dev[4].ref_val = ii*41;
            }	
        }
		

        for(idx=0; idx<MAX_KEY; idx++)
                input_set_capability(input, EV_KEY, key_dev[idx].code);

    input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);

    input_set_abs_params(input, ABS_X, 0, MAX_10BIT, 0, 0);
    input_set_abs_params(input, ABS_Y, 0, MAX_10BIT, 0, 0);
	input_set_abs_params(input, ABS_PRESSURE, 0, MAX_10BIT, 0, 0);

	input->name = "imap-adc";
	ret = input_register_device(input);
	if(ret) {
		goto err_unmap;
	}

	INIT_DELAYED_WORK(&host->ts_work, ts_work_func);
	INIT_DELAYED_WORK(&host->battery_work, battery_work_func);
	INIT_DELAYED_WORK(&host->jack_work, jack_work_func);
	INIT_DELAYED_WORK(&host->micp_work, micp_work_func);
	INIT_DELAYED_WORK(&host->micn_work, micn_work_func);
    
    /* volup channel is instead of keys.channel  */
	if(item_exist("keys.volup"))
		host->key_id = item_integer("keys.volup",1);
	else if(item_exist("keys.camera"))
		host->key_id = item_integer("keys.camera",1);
	else if(item_exist("keys.menu"))
		host->key_id = item_integer("keys.menu",1);
	else if(item_exist("keys.voldown"))
		host->key_id = item_integer("keys.voldown",1);
	else
		printk(KERN_ERR "Warning: ADC key channel is not found.\n");

    INIT_DELAYED_WORK(&host->key_work, key_work_func);
    host->key_poll_delay = 10;
    host->key_poll_period = 100;
    host->keydown = false;
    host->key_cal_value = 50;

	host->battery_id = 10;
	host->battery_poll_delay = 10;
	host->battery_poll_period = 1000;

	host->jack_id = 7;
	host->jack_poll_delay = 10;
	host->jack_poll_period = 500;
	host->jack_plugin = false;

	host->micp_id = 6;
	host->micp_poll_delay = 10;
	host->micp_poll_period = 100;
	host->micp_down = false;

	host->micn_id = 5;
	host->micn_poll_delay = 10;
	host->micn_poll_period = 100;
	host->micn_down = false;

	host->input = input;
	adc_ctrl_reset(host);	
	adc_clk_gen_init(0);

        platform_set_drvdata(pdev, host);
	/* battery function */
	g_battery_value = 0;
	__imapx_register_batt(battery_get_value);

	ret = request_irq(irq, adc_irq_handle, 0, "imap-adc", host);
	if(ret) {
		goto err_unmap;
	}

	adc_ctrl_init(host);
	schedule_delayed_work(&host->battery_work, msecs_to_jiffies(host->battery_poll_delay));
	ADC_LOG("-- %s --\n", __func__);
	return 0;

err_unmap:
	iounmap(host->regs);
err_freehost:
	kfree(host);
	return ret;
}

#ifdef CONFIG_PM
static int adc_imap_suspend(struct platform_device *dev, pm_message_t state)
{
        struct adc_ctrl *host = platform_get_drvdata(dev);

        cancel_delayed_work_sync(&host->battery_work);
	return 0;
}

static int adc_imap_resume(struct platform_device *dev)
{
        //int ret;
        struct adc_ctrl *host = platform_get_drvdata(dev);

        printk("%s | resume\n", __func__);
        //ret = clk_enable(host->clk);
	adc_clk_gen_init(1);
	adc_ctrl_reset(host);	
	adc_clk_gen_init(0);
	adc_ctrl_init(host);
	schedule_delayed_work(&host->battery_work, msecs_to_jiffies(host->battery_poll_delay));
	return 0;
}

#else
#define adc_imap_suspend	NULL
#define adc_imap_resume		NULL
#endif


static struct platform_driver adc_imap_driver = {
	.suspend        = adc_imap_suspend,
	.resume         = adc_imap_resume,
	.driver         = {
		.name           = "imap-adc",
	},
};


static int __init adc_imap_init(void)
{
	return platform_driver_probe(&adc_imap_driver, adc_imap_probe);
}
//module_init(adc_imap_init);
subsys_initcall(adc_imap_init);

static void __exit adc_imap_exit(void)
{
}
module_exit(adc_imap_exit);


MODULE_AUTHOR("Infotm");
MODULE_DESCRIPTION("Infotm TouchScreen Driver");
MODULE_LICENSE("GPL");

