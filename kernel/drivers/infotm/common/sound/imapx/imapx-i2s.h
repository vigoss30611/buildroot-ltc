#ifndef __IMAPX_IIS_H__
#define __IMAPX_IIS_H__

struct i2s_reg {
	uint32_t ier;
	uint32_t irer;
	uint32_t iter;
	uint32_t cer;
	uint32_t ccr;
	uint32_t mcr;
	uint32_t bcr;
	uint32_t i2smode;
	uint32_t ter;
	uint32_t rer;
	uint32_t rcr;
	uint32_t tcr;
	uint32_t imr;
	uint32_t rfcr;
	uint32_t tfcr;
};

struct imapx_i2s_info {
	void __iomem    *regs;
	void __iomem    *mclk;
	void __iomem    *bclk;
	void __iomem    *interface;
	struct clk  *clk;
	unsigned int    rate;
	int master;
	struct i2s_reg  registers;
};
#endif
