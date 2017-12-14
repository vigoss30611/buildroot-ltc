#ifndef _IROM_E_FUSE_H__
#define _IROM_E_FUSE_H__

struct efuse_paras {
	/* config */
	uint32_t	e_config;

	/* base */
	uint8_t		mux_mmc;
	uint8_t		mux_default;
	uint8_t		crystal;

	/* nand */
	uint8_t		tg_timing;

	/* trust */
	uint16_t	trust;

	/* para */
	uint16_t	phy_read;
	uint16_t	phy_delay;
	uint16_t	polynomial;
	uint32_t	seed[4];
	uint16_t	binfo[12];
};

extern void init_efuse(void);
extern int inline ecfg_check_flag(uint32_t flag);
extern struct efuse_paras * ecfg_paras(void);
extern void efuse_info(int human);
extern int tt_efuse_inject(void);
extern uint64_t efuse_mac(void);
extern char * efuse_mac2serial(void);

#define ECFG_TRUST_NUM			0x7259

#define ECFG_MAC_OFFSET			(359 * 4)
#define ECFG_PARA_OFFSET		(365 * 4)

#define ECFG_DISABLE_BURN		(1 << 0)	/* crit */
#define ECFG_DISABLE_ICACHE		(1 << 1)
#define ECFG_DISABLE_MMU		(1 << 2)

#define ECFG_DEFAULT_RANDOMIZER	(1 << 8)	/* crit */
#define ECFG_DEFAULT_TGTIMING	(1 << 9)	/* crit */
#define ECFG_DEFAULT_BINFO		(1 << 10)	/* crit */

#define ECFG_ENABLE_CRYPTO	(1 << 16)		/* crit */
#define ECFG_ENABLE_SATA	(1 << 17)

#define ECFG_ENABLE_PULL_MMC	(1 << 24)
#define ECFG_ENABLE_PULL_SPI	(1 << 25)
#define ECFG_ENABLE_PULL_NAND	(1 << 26)
#define ECFG_ENABLE_PULL_IIC	(1 << 27)
#define ECFG_ENABLE_PULL_UART	(1 << 28)
#define ECFG_ENABLE_PULL_ETH	(1 << 29)

#endif /* _IROM_E_FUSE_H__ */

