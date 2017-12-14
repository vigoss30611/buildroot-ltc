#ifndef _IROM_CDBG_H_
#define _IROM_CDBG_H_
#include <linux/types.h>
struct cdbg_cfg_dram {
	uint32_t	paras;
};

struct cdbg_cfg_clks {
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

#define CDBG_NAND_ID_COUNT		8
struct cdbg_cfg_nand {
	uint8_t		id[CDBG_NAND_ID_COUNT];
	uint8_t		interface;
	uint8_t		randomizer;
	uint8_t		cycle;
	uint8_t		mecclvl;
	uint8_t		secclvl;
	uint8_t		eccen;
	uint16_t	blockcount;		/* this is currently only used for across bad blocking checking */
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
#if !defined(CONFIG_PRELOADER)
	uint32_t	nand_param0;
	uint32_t	nand_param1;
	uint32_t	nand_param2;
	uint32_t	read_retry;
	uint32_t	retry_level;
#endif	
};

struct cdbg_cfg_table {
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

#define IR_CFG_TLB_MAGIC	0x69780800


extern int  cdbg_jump(void * address);
extern int  cdbg_do_config(void *table, int c);
extern int  cdbg_config_status(uint32_t *stat);
extern void cdbg_shell(void);
extern int  cdbg_dram_enabled(void);
extern int  cdbg_is_wakeup(void);
extern void cdbg_restore_os(void);
extern void cdbg_log_toggle(int en);
extern void cdbg_boot_redirect(int *id, loff_t *offs);
extern int cdbg_config_isi(void *data);

extern int      cdbg_verify_burn(void);
extern void     cdbg_verify_burn_enable(int en, const uint8_t *key);
extern uint8_t *cdbg_verify_burn_key(void);

extern uint8_t __irom_ver[];

extern struct cdbg_cfg_nand infotm_nand_idt[];
extern int infotm_nand_count(void);

#define addr_is_dram(x)		(((uint32_t)x) >= DRAM_BASE_PA)


#endif /* _IROM_CDBG_H_ */

