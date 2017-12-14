#ifndef _SX2_H
#define _SX2_H

#include <asm/io.h>

//define sx2 base register
#define IMAPX_DVP0_REG_BASE		(0x22080000)
#define IMAPX_DVP1_REG_BASE		(0x22090000)
#define IMAPX_SX2_REG_BASE		(0x220a0000)
#define SYSMGR_SX2_BASE			(0x2D024000)

#define		SX2CTRL0			(0x180)
#define 		SX2STAT0			(0x184)
#define		SX2TGSZ			(0x188)
#define 		SX2TGHS			(0x18c)
#define		SX2TGVS			(0x190)
#define 		SX2S0CH			(0x194)
#define 		SX2S0CV			(0x198)
#define		SX2S1CH			(0x19c)
#define		SX2S1CV			(0x1a0)
#define		SX2OWH			(0x1a4)
#define		SX2OWV			(0x1a8)
#define		SX2OWTP			(0x1ac)
#define		SX2OWTT			(0x1b0)
#define		SX2OWBF0			(0x1b4)
#define		SX2OWBF1			(0x1b8)
#define		SX2VCNT			(0x1bc)

//############################bit offset############################

//SX2CTRL0
#define	SX2CTRL0_MSD				(29)
#define	SX2CTRL0_RDSS				(28)
#define	SX2CTRL0_BLKMASK			(25)
#define	SX2CTRL0_PMO				(24)
#define	SX2CTRL0_SMO				(20)
#define	SX2CTRL0_SMD				(16)
#define	SX2CTRL0_BSM				(12)
#define	SX2CTRL0_SP				(8)
#define	SX2CTRL0_RDEN				(4)
#define	SX2CTRL0_RST				(1)
#define	SX2CTRL0_EN				(0)

//SX2STAT0
#define	SX2STAT0_RDE				(14)
#define	SX2STAT0_RDHSD			(13)
#define	SX2STAT0_RDR				(12)
#define	SX2STAT0_B1UF				(9)
#define	SX2STAT0_B1OV				(8)
#define	SX2STAT0_B0UF				(5)
#define	SX2STAT0_B0OV				(4)
#define	SX2STAT0_C1OV				(1)
#define	SX2STAT0_C0OV				(0)
// SX2TGSZ
#define	SX2TGSZ_H					(16)
#define	SX2TGSZ_W					(0)

// SX2TGHS
#define	SX2TGHS_E					(16)
#define	SX2TGHS_S					(0)
// SX2TGVS
#define	SX2TGVS_E					(16)
#define	SX2TGVS_S					(0)

// SX2S0CH
#define	SX2S0CH_E					(16)
#define	SX2S0CH_S					(0)
// SX2S0CV
#define	SX2S0CV_E					(16)
#define	SX2S0CV_S					(0)

// SX2S1CH
#define	SX2S1CH_E					(16)
#define	SX2S1CH_S					(0)
// SX2S1CV
#define	SX2S1CV_E					(16)
#define	SX2S1CV_S					(0)

// SX2OWH
#define	SX2OWH_E					(16)
#define	SX2OWH_S					(0)

// SX2OWV
#define	SX2OWV_E					(16)
#define	SX2OWV_S					(0)

// SX2OWTP
#define	SX2OWTP_V					(16)
#define	SX2OWTP_H					(0)

// SX2OWTT
#define	SX2OWTT_BS				(16)
#define	SX2OWTT_THR				(0)

// SX2OWBF0
#define	SX2OWBF0_P1				(16)
#define	SX2OWBF0_P0				(0)

// SX2OWBF1
#define	SX2OWBF1_P3				(16)
#define	SX2OWBF1_P2				(0)

// SX2VCNT
#define	SX2VCNT_VCNT1				(16)
#define	SX2VCNT_VCNT0				(0)

//define sx2 clock and gate
#define	ISP_DVP_CFG0				(0x28)
#define	ISP_DVP_CFG1				(0x2c)

//############################bit offset############################

//ISP_DVP_CFG0
#define	DVP1_CLKMUX_SEL			(3)
#define	DVP0_CLKMUX_SEL			(2)
#define	PARALLEL_SEL1				(1)
#define	PARALLEL_SEL0				(0)

//ISP_DVP_CFG1
#define	IPI_INPUT_SEL1				(6)
#define	IPI_INPUT_SEL0				(5)
#define	IPI_DIRECT2ISP				(4)
#define	DVP_INPUT_SEL1				(3)
#define	DVP_INPUT_SEL0				(2)
#define	DVP_BYPASS_SEL1			(1)
#define	DVP_BYPASS_SEL0			(0)

/* ############################dvp wrapper########################*/
#define DVP_WRAPPER_CTRL			(0x00)
#define DVP_WRAPPER_FRM_CNT		(0x24)
#define DVP_WRAPPER_HCNT_VCNT	(0x28)

#define DVP_WRAPPER_CTRL_DEBUG_ON		(3)


enum stiching_fmt {
	STICHING_BYPASS,
	STICHING_PIXEL_MIX,
	STICHING_FRAM_BY_FRAM,
	STICHING_SIDE_BY_SIDE
};

enum pixel_mix_fmt {
	PIXEL_RAW,/* pixel mix ordering. 0:bayer raw order; */
	PIXEL_YUV422/* 1:yuv 422 order */
};

typedef struct _sx2_in_fmt_t{
	uint16_t h_size;
	uint16_t v_size;
	uint16_t h_blk;
	uint16_t v_blk;
}sx2_in_fmt_t;

typedef struct _sx2_out_timing_t{
	uint16_t h;//HI;Timing Generator Output Window height
	uint16_t w;//WD;TIming Generator Output Window width
	uint16_t h_end;//HE;Timing Generator horizontal sync end position
	uint16_t h_start;//HS;Timing Generator horizontal sync start position
	uint16_t v_end;//VE;Timing Generator vertical sync end position
	uint16_t v_start;//VS;Timing Generator vertical sync start position
}sx2_out_timing_t;

typedef struct _sx2_window_t{
	uint16_t sen0_h_end;//S0CHE;sensor0 horizontal capture  end position
	uint16_t sen0_h_start;//S0CHS;sensor0 horizontal capture  start position
	uint16_t sen0_v_end;//S0CVE;sensor0 vertical capture position
	uint16_t sen0_v_start;//S0CVS:sensor0 vertical capture position
	uint16_t sen1_h_end;//S0CHE;sensor1 horizontal capture  end position
	uint16_t sen1_h_start;//S0CHS;sensor1 horizontal capture  start position
	uint16_t sen1_v_end;//S0CVE;sensor1 vertical capture position
	uint16_t sen1_v_start;//S0CVS:sensor1 vertical capture position
	uint16_t out_h_end;//OHE;output window horizontal end position
	uint16_t out_h_start;//OHS;output window horizontal start position
	uint16_t out_v_end;//OVE;output window vertical end position
	uint16_t out_v_start;//OVS;output window vertical start position
}sx2_window_t;

typedef struct _sx2_trigger_t{
	/* IOSV:output window trigger vertical position */
	uint16_t out_v_pos;
	/* IOSH:output window trigger horizontal position*/
	uint16_t out_h_pos;
	uint16_t buf_sel;//BS
	uint16_t threshold;//THR
}sx2_trigger_t;

typedef struct _sx2_config_t{
	enum stiching_fmt 		stiching_mode;
	enum pixel_mix_fmt		pixel_mix_order;
	sx2_out_timing_t			out_timing;
	sx2_window_t			window;
	sx2_trigger_t				trigger;
	uint8_t					msd_en;//MSD.minimize sensor delay

	/* RDSS. resolution detect sensor select.
	* 0:use sensor 0 to detect the resolution;
	* 1:user sensor 1 to detect the resolution
	*/
	uint8_t					rd_sel;
	uint8_t					blank_mask;
	uint8_t					stiching_mode_opt;

	/* BSM.[0b00]:sensor0->buf0,sensor1->buf1;
	*[0b01]:sensor0->buf1,sensor1->buf0;
	*[0b1x]:automatic selection mode
	*/
	uint8_t					sen_buf_sel;

	/*	Sync Polarity. [0]:hsync negative,vsync negative;
	*				[1]:hsync negative,vsync positive;
	*				[2]:hsync positive,vsync negative;
	*				[3]:hsync positive,vsync positive;
	*/
	uint8_t					sen_pol;

	/* resolution detect enable */
	uint8_t					rd_en;
	uint16_t					blank_fill_p0;
	uint16_t					blank_fill_p1;
	uint16_t					blank_fill_p2;
	uint16_t					blank_fill_p3;
}sx2_config_t;

/*Note: if you know detail, please read apollo3_isp_dvp_mux_03.pdf*/
typedef struct _sx2_clk_gate_t{
	/*clk mux config*/
	uint8_t			dvp1_clkmux_sel;/*dvp-clk1 mux: 0:dvp1, 1:dvp0*/
	uint8_t			dvp0_clkmux_sel;/*dvp-clk0 mux: 0:dvp0, 1:dvp1*/
	uint8_t			parallel_sel1;/*dvp-clk mux: 0: dvp-clk0, 1:dvp-clk1*/
	uint8_t			parallel_sel0;/*0:MIPI_DVP clk, 1:dvp-clk*/

	/* data mux config */
	uint8_t			ipi_input_sel1;/*enable MIPI-DVP passed by dvp-wrapper1; 0: disable 1:enable*/
	uint8_t			ipi_input_sel0;/*enable MIPI-DVP passed by dvp-wrapper0; 0: disable 1:enable*/

	uint8_t			ipi_direct2isp;/*enable MIPI-DVP passed directly by isp ; 0: disable 1:enable*/
	uint8_t			dvp_input_sel1;/*dvp_wrapper1 input:0: dvp1-in, 1: dvp0-in */
	uint8_t			dvp_input_sel0;/*dvp_wrapper0 input:0: dvp0-in, 1: dvp1-in */
	uint8_t			dvp_bypass_sel1;/*0: seletet to sx2-out, 1: select to single dvp out */
	uint8_t			dvp_bypass_sel0;/*0: seletet to dvp_wrapper0, 1: select to dvp_wrapper1*/
}sx2_clk_gate_t;

struct dvp_wrapper_config_t {
	uint8_t debugon;
	uint8_t invclk;
	uint8_t invvsync;
	uint8_t invhsync;
	uint32_t hmode;
	uint32_t vmode;
	uint32_t hnum;
	uint32_t vnum;
	uint8_t  syncmask;
	uint8_t  syncode0;
	uint8_t  syncode1;
	uint8_t  syncode2;
	uint8_t  syncode3;
	uint32_t hdlycnt;
	uint32_t vdlycnt;
};

enum SX2_SENSOR_MODE {
	SX2_SENSOR_DUAP_DVP =0,
	SX2_SENSOR_DVP0 = 1,
	SX2_SENSOR_DVP1 = 2,
	SX2_SENSOR_DEFUALT = 0xff,
};

enum SX2_MODE {
	SX2_MANUAL_MODE = 0x00,
	SX2_AUTO_MODE = 0x01,
};

struct sx2_init_config {
	uint32_t mode;
	uint32_t freq;
	struct _sx2_in_fmt_t in_fmt;
};

#define SX2_CTRL_MAGIC	('S')

#define SX2_G_CFG	_IOW(CTRL_MAGIC, 0x32, struct sx2_init_config)
#define SX2_S_CFG	_IOW(CTRL_MAGIC, 0x33, struct sx2_init_config)

int daul_dvp_bridge_init(int mode);

#endif

