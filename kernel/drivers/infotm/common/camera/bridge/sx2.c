#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <mach/pad.h>
#include <linux/camera/csi/csi_reg.h>
#include <linux/camera/csi/csi_core.h>
#include <linux/camera/ddk_sensor_driver.h>
#include <linux/device.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/of_regulator.h>
#include <mach/items.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/errno.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/camera/sx2.h>

#include "bridge.h"
#define APOLLO3_SX2_NAME "apollo3-sx2"

/* #define SX2_DEBUG 1 */
/* #define SX2_NT99142_5FPS_TEST 1 */

#define MIPI_PIXEL_CLK_DEV_ID "imap-mipi-dphy-pix"
#define MIPI_PIXEL_CLK_CON_ID "imap-mipi-dsi"

#if 0
#define sx2_err_log(fmt, a...)	printk("[sx2][err]"fmt, ##a)
#define sx2_info_log(fmt, a...)	printk("[sx2][info]"fmt, ##a)
#define sx2_warn_log(fmt, a...)	printk("[sx2][warn]"fmt, ##a)

#ifdef SX2_DEBUG
#define sx2_dbg_log(fmt, a...)	printk("[sx2][dbg]"fmt, ##a)
#else
#define sx2_dbg_log(fmt, a...)	//printk("[sx2][dbg]"fmt, ##a)
#endif
#endif

static sx2_in_fmt_t sx2_in_fmt  ={
	.h_size =1280,
	.v_size =720,
	.h_blk =320,
	.v_blk =30,
};

static sx2_config_t sx2_config = {
	.stiching_mode   = STICHING_SIDE_BY_SIDE,
	.pixel_mix_order  =PIXEL_RAW,
	.out_timing	={
		.h	=749,
		.w	= 3199,
		.h_end	=0x09C8,
		.h_start	=0,
		.v_end	=0x100,
		.v_start	=0,
	},
	.window	={
		.sen0_h_end	=0x77F,
		.sen0_h_start	=0,
		.sen0_v_end	=0x2CF,
		.sen0_v_start	=0,
		.sen1_h_end	=0x77F,
		.sen1_h_start	=0,
		.sen1_v_end	=0x2CF,
		.sen1_v_start	=0,
		.out_h_end	=0xF01,
		.out_h_start	=2,
		.out_v_end	=0x2CF,
		.out_v_start	=0,
	},
	.trigger ={
		.out_v_pos	=0,
		.out_h_pos	=0,
		.buf_sel		=0,
		.threshold	=360,// 720/2
	},
	.msd_en			=1,
	.rd_sel			=0,
	.blank_mask		=1,
	.stiching_mode_opt=0b0001,
	.sen_buf_sel		=0b10,
	.sen_pol			=0b00,
	.rd_en			=0,
	.blank_fill_p0		=0,
	.blank_fill_p1		=0,
	.blank_fill_p2		=0,
	.blank_fill_p3		=0,
};

static sx2_clk_gate_t	sx2_clk_gate ={
	/* clk mux config */
	.dvp1_clkmux_sel	=0,/*dvp-clk1 mux: 0:dvp1, 1:dvp0*/
	.dvp0_clkmux_sel	=0,/*dvp-clk0 mux: 0:dvp0, 1:dvp1*/
	.parallel_sel1		=0,/*dvp-clk mux: 0: dvp-clk0, 1:dvp-clk1*/
	.parallel_sel0		=0,/*0:MIPI_DVP clk, 1:dvp-clk*/

	/* data mux config */
	.ipi_input_sel1	=0,/*enable MIPI-DVP passed by dvp-wrapper1; 0: disable 1:enable*/
	.ipi_input_sel0	=0,/*enable MIPI-DVP passed by dvp-wrapper0; 0: disable 1:enable*/

	.ipi_direct2isp		=0,/*enable MIPI-DVP passed directly by isp ; 0: disable 1:enable*/
	.dvp_input_sel1	=0,/*dvp_wrapper1 input:0: dvp1-in, 1: dvp0-in */
	.dvp_input_sel0	=0,/*dvp_wrapper0 input:0: dvp0-in, 1: dvp1-in */
	.dvp_bypass_sel1	=0,/*0: seletet to sx2-out, 1: select to single dvp out */
	.dvp_bypass_sel0	=0,/*0: seletet to dvp_wrapper0, 1: select to dvp_wrapper1*/
};

static struct dvp_wrapper_config_t dvp_wrapper_configs[2] ={
	/*dvp0*/
	{
		.debugon = 1,
		.invclk = 0,
		.invvsync = 0,
		.invhsync = 0,
		.hmode = 0,
		.vmode = 0,
		.hnum = 0,
		.vnum = 0,
		.syncmask = 0,
		.syncode0 = 0,
		.syncode1 = 0,
		.syncode2 = 0,
		.syncode3 = 0,
		.hdlycnt = 0,
		.vdlycnt = 0,
	},
	/*dvp1*/
	{
		.debugon = 1,
		.invclk = 0,
		.invvsync = 0,
		.invhsync = 0,
		.hmode = 0,
		.vmode = 0,
		.hnum = 0,
		.vnum = 0,
		.syncmask = 0,
		.syncode0 = 0,
		.syncode1 = 0,
		.syncode2 = 0,
		.syncode3 = 0,
		.hdlycnt = 0,
		.vdlycnt = 0,
	},
};

struct sx2_daul_dvp_bridge {
	uint32_t base;
	uint32_t dvp_wrapper0_base;
	uint32_t dvp_wrapper1_base;

	uint32_t mode;
	uint32_t sensor_mode;
	uint32_t dvp1_pad_mode;
	sx2_config_t 	cfg;
	sx2_in_fmt_t in_fmt;
	sx2_clk_gate_t	clk_gate_cfg;
	uint32_t offset;
	struct clk *pclk;
	uint32_t out_freq;
};

static struct sx2_daul_dvp_bridge* sx2 = NULL;
static struct infotm_bridge_dev* dev = NULL;

//define register operation
inline void sx2_write(uint32_t _base, uint32_t _offset, uint32_t _val)
{
	writel(_val, (void*)(_base+_offset));
	bridge_log(B_LOG_DEBUG,"0x%x = 0x%08x\n", _base + _offset, readl((void*)(_base + _offset)));
}

inline uint32_t sx2_read(uint32_t _base, uint32_t _offset)
{
	return readl((void*)(_base + _offset));
}

static void imapx_sx2_set_bit(uint32_t base, uint32_t addr, int bits, int offset, uint32_t value)
{
	uint32_t tmp;
	tmp = sx2_read(base, addr);
	tmp = (value << offset) | (~(((1 << bits)- 1) << offset)  & tmp);
	sx2_write(base, addr, tmp);
}

static uint32_t imapx_sx2_get_bit(uint32_t base,  uint32_t addr, int bits, int offset)
{
	uint32_t tmp;
	tmp = sx2_read(base, addr);
	tmp = (tmp >> offset) & ((1 << bits)- 1) ;
	return  tmp;
}

void sx2_clk_gate_cfg(sx2_clk_gate_t *cfg)
{
	imapx_sx2_set_bit((uint32_t)(IO_ADDRESS(SYSMGR_ISP_BASE)), ISP_DVP_CFG0, 4, 0,
						( (cfg->dvp1_clkmux_sel) <<DVP1_CLKMUX_SEL)|
						( (cfg->dvp0_clkmux_sel) << DVP0_CLKMUX_SEL)|
						( (cfg->parallel_sel1)<<PARALLEL_SEL1)|
						( (cfg->parallel_sel0)<<PARALLEL_SEL0) );
	imapx_sx2_set_bit((uint32_t)(IO_ADDRESS(SYSMGR_ISP_BASE)), ISP_DVP_CFG1, 7, 0,
						( (cfg->ipi_input_sel1) <<IPI_INPUT_SEL1) |
						( (cfg->ipi_input_sel0) << IPI_INPUT_SEL0) |
						( (cfg->ipi_direct2isp)<<IPI_DIRECT2ISP)|
						( (cfg->dvp_input_sel1)<<DVP_INPUT_SEL1) |
						( (cfg->dvp_input_sel0)<<DVP_INPUT_SEL0) |
						( (cfg->dvp_bypass_sel1)<<DVP_BYPASS_SEL1) |
						( (cfg->dvp_bypass_sel0)<<DVP_BYPASS_SEL0));
}

void sx2_clk_gate_dvp0(sx2_clk_gate_t *cfg)
{
	//writel(0x01, IO_ADDRESS(SYSMGR_ISP_BASE) +0x28);
	//writel(0x02, IO_ADDRESS(SYSMGR_ISP_BASE) +0x2c);

	cfg->parallel_sel0 = 1;
	cfg->parallel_sel1 = 0;
	cfg->dvp_bypass_sel1 = 1;
	cfg->dvp_bypass_sel0 = 0;

	sx2_clk_gate_cfg(cfg);
}

void sx2_clk_gate_dvp1(sx2_clk_gate_t *cfg)
{
	//writel(0x03, IO_ADDRESS(SYSMGR_ISP_BASE) +0x28);
	//writel(0x03, IO_ADDRESS(SYSMGR_ISP_BASE) +0x2c);

	cfg->parallel_sel0 = 1;
	cfg->parallel_sel1 = 1;
	cfg->dvp_bypass_sel1 = 1;
	cfg->dvp_bypass_sel0 = 1;

	sx2_clk_gate_cfg(cfg);
}

void sx2_use_cam1_0(void)
{
	//imapx_sx2_set_bit((uint32_t)PAD_BASE,0x3f8,2,6,0b01);
	imapx_pad_init("cam1");
}

void sx2_use_cam1_1(void)
{
	//imapx_sx2_set_bit((uint32_t)PAD_BASE,0x3f8,2,6,0b10);
	imapx_pad_init("cam1-mux");
}

void sx2_fmt_convert_config(sx2_in_fmt_t *in_fmt, sx2_config_t *config)
{
	config->out_timing.h =in_fmt->v_size + in_fmt->v_blk -1;
	config->out_timing.w =(in_fmt->h_size + in_fmt->h_blk)*2 -1;
	config->out_timing.h_end =in_fmt->h_blk*2;
	config->out_timing.h_start =0;

	/* sensor0 config*/
	config->window.sen0_h_end =in_fmt->h_size-1;
	config->window.sen0_h_start =0;
	config->window.sen0_v_end =in_fmt->v_size-1;
	config->window.sen0_v_start =0;

	/* sensor0 config*/
	config->window.sen1_h_end =in_fmt->h_size-1;
	config->window.sen1_h_start =0;
	config->window.sen1_v_end =in_fmt->v_size-1;
	config->window.sen1_v_start =0;

	config->window.out_h_end =in_fmt->h_size*2+1;
	config->window.out_h_start =2;
	config->window.out_v_end =in_fmt->v_size-1;
	config->window.out_v_start =0;

	config->trigger.threshold =in_fmt->h_size/2;
}

uint32_t sx2_dump(char* buf)
{
	uint32_t val = 0;
	val = sx2_read( sx2->base, SX2CTRL0);
	sprintf(buf, "[0x%x] = 0x%08x\n", SX2CTRL0 , val);
	val = sx2_read( sx2->base, SX2STAT0);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2STAT0 , val);
	val = sx2_read( sx2->base, SX2TGSZ);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2TGSZ , val);
	val = sx2_read( sx2->base, SX2TGHS);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2TGHS , val);
	val = sx2_read( sx2->base, SX2S0CH);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2S0CH , val);
	val = sx2_read( sx2->base, SX2S0CV);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2S0CV , val);
	val = sx2_read( sx2->base, SX2S1CH);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2S1CH , val);
	val = sx2_read( sx2->base, SX2S1CV);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2S1CV , val);
	val = sx2_read( sx2->base, SX2OWH);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2OWH , val);
	val = sx2_read( sx2->base, SX2OWV);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2OWV , val);
	val = sx2_read( sx2->base, SX2OWTP);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2OWTP , val);
	val = sx2_read( sx2->base, SX2OWTT);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2OWTT , val);
	val = sx2_read( sx2->base, SX2OWBF0);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2OWBF0 , val);
	val = sx2_read( sx2->base, SX2OWBF1);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2OWBF1 , val);
	val = sx2_read( sx2->base, SX2VCNT);
	sprintf(buf+strlen(buf), "[0x%x] = 0x%08x\n", SX2VCNT , val);

	return strlen(buf);
}

void sx2_dump_print(void)
{
	char *dump_buf  =(char*) kmalloc(sizeof(char) * 512, GFP_KERNEL);
	if (!dump_buf) {
		bridge_log(B_LOG_ERROR,"alloc dump_buf failed\n");
	}
	memset(dump_buf, 0, 512);

	sx2_dump(dump_buf);
	printk("%s", dump_buf);

	kfree(dump_buf);
}

uint32_t sx2_check_stat(void)
{
	uint32_t stat;
	int err = 0;

	stat =sx2_read( sx2->base,SX2STAT0 );

	if( stat & (1<<SX2STAT0_RDE)) {
		err = -1;
		bridge_log(B_LOG_ERROR,"resolution detect error\n");
	}

	if( stat & (1<<SX2STAT0_RDHSD)) {
		err = -1;
		bridge_log(B_LOG_ERROR,"resolution detect H-sync discontinuity\n");
	}

	if( stat & ( 1<<SX2STAT0_RDR)) {
		err = 0;
		bridge_log(B_LOG_INFO,"resolution detect ready\n");
	}

	if( stat & (1<<SX2STAT0_B1UF)) {
		err = -1;
		bridge_log(B_LOG_ERROR,"buffer 1 underflow\n");
	}

	if( stat & (1<<SX2STAT0_B1OV)) {
		err = -1;
		bridge_log(B_LOG_ERROR,"buffer 1 overflow\n");
	}

	if(stat & (1<<SX2STAT0_B0UF)) {
		err = -1;
		bridge_log(B_LOG_ERROR,"buffer 0 underflow\n");
	}

	if( stat & (1<<SX2STAT0_B0OV)) {
		err = -1;
		bridge_log(B_LOG_ERROR,"buffer 0 overflow\n");
	}

	if( stat & ( 1<<SX2STAT0_C1OV)) {
		bridge_log(B_LOG_ERROR,"channel 1 fifo overflow\n");
	}

	if( stat & (1<<SX2STAT0_C0OV)) {
		err = -1;
		bridge_log(B_LOG_ERROR,"channel 1 fifo overflow\n");
	}

	return err;
}

void __sx2_internal_init(void)
{
	sx2_config_t *cfg = &sx2->cfg;

	/*ctrl*/
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 1, SX2CTRL0_MSD, cfg->msd_en);
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 1, SX2CTRL0_RDSS, cfg->rd_sel);
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 1, SX2CTRL0_BLKMASK, cfg->blank_mask);

	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 1, SX2CTRL0_PMO, cfg->pixel_mix_order);
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 4, SX2CTRL0_SMO, cfg->stiching_mode_opt);
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 3, SX2CTRL0_SMD, cfg->stiching_mode);
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_BSM, cfg->sen_buf_sel);
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_SP, cfg->sen_pol);
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_RDEN, cfg->rd_en);

	/*timing*/
	sx2_write(sx2->base, SX2TGSZ, (cfg->out_timing.h<<SX2TGSZ_H)|(cfg->out_timing.w<<SX2TGSZ_W) );
	sx2_write(sx2->base, SX2TGHS, (cfg->out_timing.h_end<<SX2TGHS_E)|(cfg->out_timing.h_start<<SX2TGHS_S) );
	sx2_write(sx2->base, SX2TGVS, (cfg->out_timing.v_end<<SX2TGVS_E)|(cfg->out_timing.v_start<<SX2TGVS_S) );

	/*window*/
	sx2_write(sx2->base, SX2S0CH, (cfg->window.sen0_h_end<<SX2S0CH_E)|(cfg->window.sen0_h_start<<SX2S0CH_S) );
	sx2_write(sx2->base, SX2S0CV, (cfg->window.sen0_v_end<<SX2S0CV_E)|(cfg->window.sen0_v_start<<SX2S0CV_S) );

	sx2_write(sx2->base, SX2S1CH, (cfg->window.sen1_h_end<<SX2S1CH_E)|(cfg->window.sen1_h_start<<SX2S1CH_S) );
	sx2_write(sx2->base, SX2S1CV, (cfg->window.sen1_v_end<<SX2S1CV_E)|(cfg->window.sen1_v_start<<SX2S1CV_S) );

	sx2_write(sx2->base, SX2OWH, (cfg->window.out_h_end<<SX2OWH_E)|(cfg->window.out_h_start<<SX2OWH_S) );
	sx2_write(sx2->base, SX2OWV, (cfg->window.out_v_end<<SX2OWV_E)|(cfg->window.out_v_start<<SX2OWV_S) );

	/*trigger*/
	sx2_write(sx2->base, SX2OWTP, (cfg->trigger.out_v_pos<<SX2OWTP_V)|(cfg->trigger.out_h_pos<<SX2OWTP_H) );
	sx2_write(sx2->base, SX2OWTT, (cfg->trigger.buf_sel<<SX2OWTT_BS)|(cfg->trigger.threshold<<SX2OWTT_THR) );

	/*blank fill*/
	sx2_write(sx2->base, SX2OWBF0, (cfg->blank_fill_p1<<SX2OWBF0_P1)|(cfg->blank_fill_p0<<SX2OWBF0_P0) );
	sx2_write(sx2->base, SX2OWBF1, (cfg->blank_fill_p3<<SX2OWBF1_P3)|(cfg->blank_fill_p2<<SX2OWBF1_P2) );
}

int sx2_init_manual_det(void)
{
	uint32_t try_cnt = 300;
	volatile uint32_t tmp;

	sx2_fmt_convert_config(&sx2->in_fmt, &sx2->cfg);

	/* 0.RST=0;EN=0 */
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_EN, 0b00);/*Activate reset  [1]:RESET;[0]:EN*/

	/* 1.Initialize all sx2 register settings */
	__sx2_internal_init();

	/* 2.Activate reset: RST=1;EN=0 */
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_EN, 0b10);/*Activate reset  [1]:RESET;[0]:EN*/

	/* 3. Optional:Activate Resolution Detect Mode */
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 1, SX2CTRL0_RDEN, sx2->cfg.rd_en);

	if(1 == sx2->cfg.rd_en) {
		while(try_cnt--) {
			tmp =sx2_read( sx2->base, SX2STAT0);
			if(tmp & (1<<SX2STAT0_RDR))
				break;
			msleep(1);
		}
	}

	if (!try_cnt) {
		bridge_log(B_LOG_ERROR,"Resolution Detect Ready Failed!");
		return -1;
	}

	/* 7-8 RST=0;EN=1 */
	/*Activate reset  [1]:RESET;[0]:EN*/
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_EN, 0b01);
#ifdef SX2_DEBUG
	sx2_dump_print();
#endif

	return 0;
}

int sx2_init_auto_det(void)
{
	uint32_t try_cnt = 300;
	volatile uint32_t tmp;
	uint16_t det_h, det_w;

	sx2->cfg.rd_en = 1;

	/* 0.RST=0;EN=0 */
	/*Activate reset  [1]:RESET;[0]:EN*/
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_EN, 0b00);

	/* 1.Initialize all sx2 register settings */
	__sx2_internal_init();

	/* 2.Activate reset: RST=1;EN=0 */
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_EN, 0b10);/*Activate reset  [1]:RESET;[0]:EN*/

	/* 3. Optional:Activate Resolution Detect Mode */
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 1, SX2CTRL0_RDEN, sx2->cfg.rd_en);

	if(1 == sx2->cfg.rd_en) {
		while(try_cnt--) {
			tmp =sx2_read( sx2->base, SX2STAT0);
			if(tmp & (1<<SX2STAT0_RDR))
				break;
			msleep(1);
		}
	}

	if (!try_cnt) {
		bridge_log(B_LOG_ERROR,"Resolution Detect Ready Failed!");
		return -1;
	}

	sx2_check_stat();

	tmp = sx2_read(sx2->base, SX2TGSZ);
	det_h = ((tmp >> SX2TGSZ_H) & 0xffff);
	det_w = ((tmp >> SX2TGSZ_W) & 0xffff);

	sx2->in_fmt.v_blk  = det_h + 1 - sx2->in_fmt.v_size;
	sx2->in_fmt.h_blk = (det_w+ 1)/2 - sx2->in_fmt.h_size;

	bridge_log(B_LOG_INFO,"auto detect w = %d h = %d\n", det_w, det_h);
	bridge_log(B_LOG_INFO,"h_size = %d, v_size = %d, h_blk = %d, v_blk = %d\n",
		sx2->in_fmt.h_size , sx2->in_fmt.v_size,
		sx2->in_fmt.h_blk, sx2->in_fmt.v_blk);

	sx2_fmt_convert_config(&sx2->in_fmt, &sx2->cfg);
	__sx2_internal_init();

	/* 7-8 RST=0;EN=1 */
	/*Activate reset  [1]:RESET;[0]:EN*/
	imapx_sx2_set_bit(sx2->base,	SX2CTRL0, 2, SX2CTRL0_EN, 0b01);

#ifdef SX2_DEBUG
	sx2_dump_print();
#endif
	return 0;
}


void dvp_wrapper_init(struct dvp_wrapper_config_t * config,void *dvp_base)
{
#if 1
	/* wrapper bypass */
	if(config->hmode != 0x2 && config->hmode != 0x3
			&& config->vmode != 0x1) {
		bridge_log(B_LOG_INFO,"dvp wrapper bypass\n");
		return ;
	}
#endif
	bridge_log(B_LOG_INFO,"dvp wrapper begin to config\n");

	writel((config->debugon << 3)  |
			(config->invclk << 2) |
			(config->invvsync) << 1 |
			config->invhsync , dvp_base + 0);

	writel((config->hmode << 4) |
			config->vmode, dvp_base + 0x4);

	writel(((config->hnum -1) << 16) |
			(config->vnum -1), dvp_base + 0x8);

	writel(config->syncmask, dvp_base + 0xc);
	writel(config->syncode0, dvp_base + 0x10);
	writel(config->syncode1, dvp_base + 0x14);
	writel(config->syncode2, dvp_base + 0x18);
	writel(config->syncode3, dvp_base + 0x1c);

	writel(((config->hdlycnt) << 16) |(config->vdlycnt), dvp_base + 0x20);
}

void sx2_dvp_wrapper_init(struct dvp_wrapper_config_t *config)
{
#if 0
	dvp_wrapper_init(&config[0], (void*)sx2->dvp_wrapper0_base);
	dvp_wrapper_init(& config[1], (void*)sx2->dvp_wrapper1_base);
#else
	imapx_sx2_set_bit(sx2->dvp_wrapper0_base,
		DVP_WRAPPER_CTRL, 1, DVP_WRAPPER_CTRL_DEBUG_ON, 1);
	imapx_sx2_set_bit(sx2->dvp_wrapper1_base,
		DVP_WRAPPER_CTRL, 1, DVP_WRAPPER_CTRL_DEBUG_ON, 1);
#endif
}

static int __dvp_wrapper_res_check(uint32_t base,int w, int h, int idx)
{
	uint32_t val;
	uint16_t hcnt, vcnt;

	val = sx2_read(base, DVP_WRAPPER_HCNT_VCNT);
	hcnt = (uint16_t)(val >> 16);
	vcnt = (uint16_t)(val & 0xffff);

	if (hcnt != w || vcnt !=h) {
		bridge_log(B_LOG_ERROR,"dvp wrapper%d pixel hcnt:%d vcnt:%d "
			"width:%d height:%d\n", idx, hcnt, vcnt, w, h);
		goto err;
	}

	bridge_log(B_LOG_DEBUG, "dvp wrapper%d pixel hcnt:%d vcnt:%d "
		"width:%d height:%d\n", idx, hcnt, vcnt, w, h);

	return 0;
err:
	return -1;
}

static int dvp_wrapper_check(void)
{
	int ret = 0;
	ret = __dvp_wrapper_res_check(sx2->dvp_wrapper0_base,
			sx2->in_fmt.h_size, sx2->in_fmt.v_size, 0);

	ret = __dvp_wrapper_res_check(sx2->dvp_wrapper1_base,
			sx2->in_fmt.h_size, sx2->in_fmt.v_size, 1);

	return ret;
}

int sx2_clk_init(void)
{
	int err;
	char clk_name[20] = {MIPI_PIXEL_CLK_DEV_ID};
	char clk_con_id[20] = {MIPI_PIXEL_CLK_CON_ID};

	sx2->pclk = clk_get_sys(clk_name, clk_con_id);
	if (IS_ERR(sx2->pclk)) {
		pr_err(" %s clock not found! \n",clk_name);
		return -1;
	}

	err = clk_prepare_enable(sx2->pclk);
	if (err) {
		pr_err("%s clock prepare enable fail!\n",clk_name);
		goto unprepare_clk;
	}
	return 0;

unprepare_clk:
	clk_disable_unprepare(sx2->pclk);
	clk_put(sx2->pclk);
	return -1;
}

static int sx2_check_clk(uint32_t freq)
{
	uint32_t clk_f = 0;
	if (!sx2->pclk)
		return -1;

	clk_f = clk_get_rate(sx2->pclk);
	if (freq != clk_f) {
		bridge_log(B_LOG_ERROR,"The autual setting of clk rate is "
			"%u not %u and novalid\n", clk_f, freq);
		return -1;
	}
	return 0;
}

static int sx2_set_clk( unsigned int freq)
{
	int i = 0, ret = 0, try_cnt= 0;
	struct clk *parent;
	unsigned long parent_clk_rate, clk_mod;
	char parent_str[64] = {0};
	const char* pll_parent[] = {"vpll", "dpll", "epll"};
	int pll_total = sizeof(pll_parent)/sizeof(pll_parent[0]);

	if (!sx2->pclk)
		goto err;

	for (i = 0; i < pll_total; i++) {
		strcpy(parent_str, pll_parent[i]);
		bridge_log(B_LOG_DEBUG, "try pll: %s\n", parent_str);
		parent = clk_get_sys(NULL, parent_str);
		if (IS_ERR(parent)) {
			bridge_log(B_LOG_ERROR,"failed to get clk %s, ret=%ld\n", parent_str, PTR_ERR(parent));
			goto err;
		}

		parent_clk_rate = clk_get_rate(parent);
		clk_mod = do_div(parent_clk_rate, freq);
		if (!clk_mod) {
			clk_set_parent(sx2->pclk, parent);
			break;
		} else {
			try_cnt++;
		}
	}

	if (try_cnt == pll_total) {
		bridge_log(B_LOG_ERROR,"system not satisfy destination clock %d for sensor\n", freq);
		goto err;
	}

	ret = clk_set_rate(sx2->pclk, freq);
	if (0 > ret){
		bridge_log(B_LOG_ERROR,"Fail to set mclk output %d clock\n", freq);
		goto err;
	}

	return 0;
err:
	return -1;
}

int daul_dvp_bridge_init(int mode)
{
	static int first =1;
	int ret = 0;

	if (mode != SX2_SENSOR_DEFUALT)
		sx2->sensor_mode = mode;

	/* enable dual dvp0 */
	if (sx2->sensor_mode == SX2_SENSOR_DVP0) {

		/*enable dvp0*/
		bridge_log(B_LOG_INFO,"config to dvp0 mode\n");
		sx2->clk_gate_cfg.parallel_sel0 = 1;/*dvp-clk*/
		sx2->clk_gate_cfg.parallel_sel1 = 0;/*dvp-clk0*/
		sx2->clk_gate_cfg.dvp_bypass_sel0 = 0;/*dvp_wrapper0*/
		sx2->clk_gate_cfg.dvp_bypass_sel1 = 1;/*dvp-out*/
		sx2_clk_gate_cfg(&sx2->clk_gate_cfg);
		/* writel(0x01, IO_ADDRESS(SYSMGR_ISP_BASE) +0x28); */
		/* writel(0x02, IO_ADDRESS(SYSMGR_ISP_BASE) +0x2c); */

	} else if (sx2->sensor_mode == SX2_SENSOR_DVP1) {

		/*enable dvp1*/
		bridge_log(B_LOG_INFO,"config to dvp1 mode\n");
		sx2->clk_gate_cfg.parallel_sel0 = 1;/*dvp-clk*/
		sx2->clk_gate_cfg.parallel_sel1 = 1;/*dvp-clk1*/
		sx2->clk_gate_cfg.dvp_bypass_sel0 = 1;/*dvp_wrapper1*/
		sx2->clk_gate_cfg.dvp_bypass_sel1 = 1;/*dvp-out*/
		sx2_clk_gate_cfg(&sx2->clk_gate_cfg);

		/* writel(0x03, IO_ADDRESS(SYSMGR_ISP_BASE) +0x28); */
		/* writel(0x03, IO_ADDRESS(SYSMGR_ISP_BASE) +0x2c); */

	} else if (sx2->sensor_mode == SX2_SENSOR_DUAP_DVP){
		bridge_log(B_LOG_INFO,"config to dual dvp mode\n");

		sx2_clk_gate_cfg(&sx2->clk_gate_cfg);

		if (first) {
#if 0
			/* set dvp1 to cam1-1 */
			if (sx2->dvp1_pad_mode)
				apollo3_sx2_use_cam1_1();
			else
				apollo3_sx2_use_cam1_0();
#endif
			ret = sx2_clk_init();
			if (ret < 0)
				bridge_log(B_LOG_ERROR,"2x pclk init failed\n");
			first = 0;
		}

		bridge_log(B_LOG_INFO,"set dual bridge clk: %d\n", sx2->out_freq);
		if (sx2->pclk)
			clk_set_rate(sx2->pclk, sx2->out_freq); /*12M*/

#ifdef SX2_NT99142_5FPS_TEST
		/* set MIPI_PIXEL_CLK to 127 clk div, 12Mhz, in DPLL */
		writel(0x7f, IO_ADDRESS(0x2d00a304));
#endif

		/* dvp wrapper debug on */
		sx2_dvp_wrapper_init(dvp_wrapper_configs);

		bridge_log(B_LOG_INFO,"h_size = %d, v_size = %d, h_blk = %d, v_blk = %d\n",
			sx2->in_fmt.h_size , sx2->in_fmt.v_size,
			sx2->in_fmt.h_blk, sx2->in_fmt.v_blk);

		memcpy(&sx2->clk_gate_cfg, &sx2_clk_gate, sizeof(sx2_clk_gate));
		ret = sx2_init_manual_det();
		if (ret < 0) {
			bridge_log(B_LOG_ERROR,"%s failed\n", __func__);
		}

	} else {
		bridge_log(B_LOG_ERROR,"no valid sensor mode %d\n", sx2->sensor_mode );
	}

	return ret;
}
EXPORT_SYMBOL(daul_dvp_bridge_init);

void daul_dvp_bridge_destroy(void)
{
	//TODO
}

static int __sx2_get_cfg(unsigned long arg)
{
	struct sx2_init_config cfg;

	bridge_log(B_LOG_DEBUG,"SX2_G_CFG\n");
	cfg.mode = sx2->mode;
	cfg.freq = sx2->out_freq;
	memcpy(&cfg.in_fmt, &sx2->in_fmt, sizeof(cfg.in_fmt));
	if (copy_to_user((void *)arg, &cfg, sizeof(cfg)))
		return -EFAULT;
	return 0;
}

static int __sx2_set_cfg(unsigned long arg)
{
	int ret = 0;
	struct sx2_init_config cfg;

	bridge_log(B_LOG_DEBUG,"SX2_S_CFG\n");
	if (copy_from_user(&cfg, (void*)arg, sizeof(cfg)))
		return -EFAULT;

	bridge_log(B_LOG_DEBUG, "mode = %d freq= %d h_size = %d, "
				"v_size = %d, h_blk = %d, v_blk = %d\n",
				cfg.mode, cfg.freq,
				cfg.in_fmt.h_size , cfg.in_fmt.v_size,
				cfg.in_fmt.h_blk, cfg.in_fmt.v_blk);

	if (sx2->sensor_mode != SX2_SENSOR_DUAP_DVP) {
		bridge_log(B_LOG_ERROR,"sensor mode is not daul dvp mode\n");
		goto err;
	}

	if (sx2->pclk) {
		sx2->out_freq = cfg.freq;
		/* clk_set_rate(sx2->pclk, cfg.freq);*//*72M*/
		sx2_set_clk(cfg.freq);
	} else {
		bridge_log(B_LOG_ERROR,"sx2 bridge pclk is not to init\n");
		goto err;
	}

	if (sx2_check_clk(cfg.freq) < 0) {
		bridge_log(B_LOG_WARN, "sx2 bridge of clk is not in accordance "
						"with the rules of the clock\n");
	}

	sx2->mode = cfg.mode;
	memcpy(&sx2->in_fmt, &cfg.in_fmt, sizeof(cfg.in_fmt));
#ifdef SX2_DEBUG
	sx2_dump_print();
#endif
	if (sx2->mode == SX2_AUTO_MODE) {
		ret = sx2_init_auto_det();
		if(ret < 0)
			goto err;
	} else if (sx2->mode == SX2_MANUAL_MODE) {
		ret = sx2_init_manual_det();
		if(ret < 0)
			goto err;
	}
	dvp_wrapper_check();

	return 0;
err:
	return -1;
}

static long sx2_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		case SX2_G_CFG:
			return __sx2_get_cfg(arg);

		case SX2_S_CFG:
			return __sx2_set_cfg(arg);

		default:
			break;
	}
	return 0;
}

static int sx2_open(struct inode *inode , struct file *filp)
{
	/* apollo3_sx2_init(SX2_SENSOR_DUAP_DVP); */
	return 0;
}

static int sx2_close(struct inode *inode , struct file *filp)
{
	/* apollo3_sx2_destroy(); */
	return 0;
}

static struct file_operations sx2_bridge_fops = {
	.owner = THIS_MODULE,
	.open  = sx2_open,
	.release = sx2_close,
	.unlocked_ioctl = sx2_ioctl,
};

static struct miscdevice sx2_bridge_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "sx2_bridge",
	.fops = &sx2_bridge_fops
};

static ssize_t sx2_dump_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sx2_dump(buf);
}

static ssize_t sx2_reg_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	uint32_t val;
	val = simple_strtoul(buf, NULL, 16);
	if (sx2->offset >= 0 && sx2->offset <= SZ_4K) {
		sx2_write(sx2->base, sx2->offset, val);
	} else {
		sx2_write(sx2->offset, 0, val);
	}
	return size;
}

static ssize_t sx2_reg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	uint32_t val;

	if (sx2->offset >= 0 && sx2->offset <= SZ_4K) {
		val = sx2_read(sx2->base, sx2->offset);
	} else {
		/* access any register */
		val = sx2_read(sx2->offset, 0);
	}
	return sprintf(buf, "[0x%08x] = 0x%08x\n", sx2->offset, val);
}

static ssize_t sx2_offset_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	(void)dev;
	(void)attr;

	bridge_log(B_LOG_INFO,"dvp bridge base:\t0x%08x\n", sx2->base);
	bridge_log(B_LOG_INFO,"dvp wrapper0 base:\t0x%08x\n", sx2->dvp_wrapper0_base);
	bridge_log(B_LOG_INFO,"dvp wrapper1 base:\t0x%08x\n", sx2->dvp_wrapper1_base);
	bridge_log(B_LOG_INFO,"dvp sysmgr base:\t0x%08x\n", (uint32_t)IO_ADDRESS(SYSMGR_ISP_BASE));
	bridge_log(B_LOG_INFO,"dvp pad base:\t\t0x%08x\n", (uint32_t)PAD_BASE);

	return sprintf(buf, "0x%08x\n", sx2->offset);
}

static ssize_t sx2_offset_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	sx2->offset = simple_strtoul(buf, NULL, 16);
	return size;
}

static ssize_t sx2_freq_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	(void)dev;
	(void)attr;

	return sprintf(buf, "freq: %d\n", sx2->offset);
}

static ssize_t sx2_freq_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	sx2->out_freq = simple_strtoul(buf, NULL, 10);
	//apollo3_sx2_init(SX2_SENSOR_DEFUALT);
	return size;
}

DEVICE_ATTR(reg, S_IWUSR | S_IRUGO, sx2_reg_show, sx2_reg_store);
DEVICE_ATTR(offset, S_IWUSR | S_IRUGO, sx2_offset_show, sx2_offset_store);
DEVICE_ATTR(dump, S_IRUGO, sx2_dump_show, NULL);
DEVICE_ATTR(freq, S_IWUSR | S_IRUGO, sx2_freq_show, sx2_freq_store);

static int sx2_create_dev_sysfs(void)
{
	int ret = 0;

	/* sysfs for debug */
	ret = device_create_file(sx2_bridge_dev.this_device, &dev_attr_reg);
	if(ret) {
		goto err;
	}

	ret = device_create_file(sx2_bridge_dev.this_device, &dev_attr_offset);
	if(ret) {
		goto err1;
	}

	ret = device_create_file(sx2_bridge_dev.this_device, &dev_attr_dump);
	if(ret) {
		goto err2;
	}

	ret = device_create_file(sx2_bridge_dev.this_device, &dev_attr_freq);
	if(ret) {
		goto err3;
	}

	return ret;

err3:
	device_remove_file(sx2_bridge_dev.this_device, &dev_attr_freq);
err2:
	device_remove_file(sx2_bridge_dev.this_device, &dev_attr_offset);
err1:
	device_remove_file(sx2_bridge_dev.this_device, &dev_attr_reg);
err:
	return ret;
}

void s2x_release_dev_sysfs(void)
{
	device_remove_file(sx2_bridge_dev.this_device, &dev_attr_reg);
	device_remove_file(sx2_bridge_dev.this_device, &dev_attr_offset);
	device_remove_file(sx2_bridge_dev.this_device, &dev_attr_dump);
	device_remove_file(sx2_bridge_dev.this_device, &dev_attr_freq);
}

int sx2_control(unsigned int cmd,unsigned int arg)
{
	switch(cmd) {
	case BRIDGE_G_CFG:
		bridge_log(B_LOG_DEBUG,"%s BRIDGE_G_CFG\n", __func__);
		return __sx2_get_cfg(arg);

	case BRIDGE_S_CFG:
		bridge_log(B_LOG_DEBUG,"%s BRIDGE_S_CFG\n", __func__);
		return __sx2_set_cfg(arg);

	default:
		break;
	}

	return 0;
}

int sx2_init(struct _infotm_bridge*bridge, unsigned int mode)
{
	(void)bridge;
	(void)mode;
	bridge_log(B_LOG_INFO,"%s\n", __func__);
	daul_dvp_bridge_init(SX2_SENSOR_DUAP_DVP);
}

int sx2_exit(unsigned int mode)
{
	(void)mode;
	daul_dvp_bridge_destroy();
}

extern int bridge_dev_register(struct infotm_bridge_dev* dev);
extern int bridge_dev_unregister(struct infotm_bridge_dev *udev);

static int __init sx2_driver_init(void)
{
	int ret = 0;
	ret = misc_register(&sx2_bridge_dev);
	if(ret) {
		goto err;
	}

	sx2_create_dev_sysfs();

	sx2 = kmalloc(sizeof(struct sx2_daul_dvp_bridge), GFP_KERNEL);
	if (!sx2) {
		bridge_log(B_LOG_ERROR,"alloc sx2 failed\n");
	}

	/* set to default value */
	memcpy(&sx2->in_fmt, &sx2_in_fmt, sizeof(sx2_in_fmt));
	memcpy(&sx2->cfg, &sx2_config, sizeof(sx2_config));
	memcpy(&sx2->clk_gate_cfg, &sx2_clk_gate, sizeof(sx2_clk_gate));

	sx2->base = (uint32_t)ioremap_nocache(IMAPX_SX2_REG_BASE, SZ_4K);
	sx2->dvp_wrapper0_base= (uint32_t)ioremap_nocache(IMAPX_DVP0_REG_BASE, SZ_4K);
	sx2->dvp_wrapper1_base= (uint32_t)ioremap_nocache(IMAPX_DVP1_REG_BASE, SZ_4K);

#ifdef SX2_DEBUG
	bridge_log(B_LOG_INFO,"dvp bridge base:0x%08x\n", sx2->base);
	bridge_log(B_LOG_INFO,"dvp wrapper0 base:0x%08x\n", sx2->dvp_wrapper0_base);
	bridge_log(B_LOG_INFO,"dvp wrapper1 base:0x%08x\n", sx2->dvp_wrapper1_base);
	bridge_log(B_LOG_INFO,"isp sysmgr base:0x%08x\n", (uint32_t)IO_ADDRESS(SYSMGR_ISP_BASE));
	bridge_log(B_LOG_INFO,"dvp pad base:0x%08x\n", (uint32_t)PAD_BASE);
#endif

	/**
	*	h_size = 1280 + 320 = 1600
	*	v_size = 720 + 30 = 750
	*/
	sx2->dvp1_pad_mode = 1; /* use cam1-1 pad*/
	sx2->in_fmt.h_size =1280;
	sx2->in_fmt.v_size =720;
	sx2->in_fmt.h_blk =320;
	sx2->in_fmt.v_blk =30;
	sx2->sensor_mode = SX2_SENSOR_DUAP_DVP;
	sx2->offset = 0;
#ifdef SX2_NT99142_5FPS_TEST
	sx2->out_freq = 12 * 1000 * 1000; /*12Mhz@6fps*/
#else
	sx2->out_freq = 72 * 1000 * 1000; /*72Mhz@30fps*/
#endif
	sx2->pclk = NULL;
	sx2->mode = SX2_MANUAL_MODE;

	dev = kmalloc(sizeof(struct infotm_bridge_dev), GFP_KERNEL);
	if (!dev) {
		printk("alloc infotm bridge dev  failed\n");
	}

	strncpy(dev->name, APOLLO3_SX2_NAME, MAX_STR_LEN);
	dev->init = sx2_init;
	dev->exit = sx2_exit;
	dev->control = sx2_control;
	bridge_dev_register(dev);

	return ret;

err:
	misc_deregister(&sx2_bridge_dev);

	return -ENODEV;
}

static void __exit sx2_driver_exit(void)
{
	bridge_dev_unregister(dev);
	kfree(dev);

	s2x_release_dev_sysfs();
	misc_deregister(&sx2_bridge_dev);
	kfree(sx2);
}

module_init(sx2_driver_init);
module_exit(sx2_driver_exit);

MODULE_AUTHOR("Davinci.Liang");
MODULE_DESCRIPTION("sx2 dual dvp bridge");
MODULE_LICENSE("Dual BSD/GPL");
