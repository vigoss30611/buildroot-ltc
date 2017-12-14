
#ifndef __IUW_IX_CFG_H__
#define __IUW_IX_CFG_H__

#define IXC_TLB_MAGIC			0x69780800
#define IXC_NAND_ID_COUNT		8

#define SETTINGS_CLKS			1
#define SETTINGS_DRAM			2
#define SETTINGS_NAND			3


struct ixc_dram {
	uint16_t	apll;
	uint16_t	dpll;
	uint16_t	epll;
	uint16_t	vpll;
	uint8_t		cpu_clk[8];
	uint8_t		axi_clk[8];
	uint8_t		apb_clk[8];
	uint8_t		axi_cpu[8];
	uint8_t		apb_cpu[8];
	uint8_t		apb_axi[8];
	int			cpu_pll;
	int			div_len;
};

struct ixc_clks {
	uint32_t	paras;
};

struct ixc_nand {
	uint8_t		id[IXC_NAND_ID_COUNT];
	uint8_t		interface;
	uint8_t		randomizer;
	uint8_t		cycle;
	uint8_t		mecclvl;
	uint8_t		secclvl;
	uint8_t		eccen;
	uint16_t	blockcount;
	uint32_t	badpagemajor;	/* first page offset when checking block validity */
	uint32_t	badpageminor;	/* second page offset when checking block validity */
	uint32_t	sysinfosize;
	uint32_t	pagesize;
	uint32_t	sparesize;
	uint32_t	blocksize;
	uint32_t	timing;
	uint32_t	rnbtimeout;
	uint32_t	phyread;
	uint32_t	phydelay;
	uint32_t	seed[4];
	uint32_t	polynomial;
	uint32_t	busw;
};

struct ixc_table {
	/* magic */
	uint32_t	magic;
	uint32_t	length;
	uint32_t	dcrc;
	uint32_t	hcrc;
	int			clksen;
	int			dramen;
	int			nanden;
	int			nandcount;
	int			spibytes;
	int			iomux;			/* IO MUX mode */
	int			blinfo;			/* new 16-bit bootloader info to reconfig boot devices */
	int			bldevice;		/* boot loader device, -1 means following the config boot loader */
	int			resv[4];
	uint32_t	bloffset;		/* boot loader offset in that device */
	uint32_t	clks_offset;
	uint32_t	dram_offset;
	uint32_t	nand_offset;
};

extern int ixc_gen_table(const char *file, uint8_t *buf, int len);

#endif /* __IUW_IX_CFG_H__ */

