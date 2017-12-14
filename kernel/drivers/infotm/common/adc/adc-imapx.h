#ifndef __ADC_IMAPX_H
#define __ADC_IMAPX_H
	


#define NUM_ADC_CHNS 	12

struct imapx_adc_chip {
	struct adc_chip         chip;
	struct device           *dev;
	struct clk              *clk;
	struct clk              *bus_clk;
	unsigned long           chan_bitmap;
	unsigned long           event_bitmap;
	int                     irq;
	void __iomem            *base;
};

#define adc_writel(_host, _off, _val) writel(_val, (_host)->base + (_off))
#define adc_readl(_chip, _off)    readl((_chip)->base + (_off))

//normal cmd
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
#define NML_CLKNUM_BIT			(23)
#define NML_PPOUT_BIT			(25)
#define NML_DMAIF_BIT			(26)
#define INST_TYPE_BIT			(30)


#define NORMAL_INSTR			(0)
#define JMP_INSTR			(1)
#define NOP_INSTR			(2)
#define END_INSTR			(3)

//jump cmd

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


#define CHN_SENS0 	5
#define CHN_SENS1 	6
#define CHN_SENS2 	7
#define CHN_SENS3 	8
#define CHN_INAUX0	 9
#define CHN_INAUX1 	10
#define CHN_INAUX2 	11
#endif
