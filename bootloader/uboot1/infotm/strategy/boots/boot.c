
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <version.h>
#include <asm/io.h>
#include <vstorage.h>
#include <bootlist.h>
#include <isi.h>
#include <hash.h>
#include <cdbg.h>
#include <efuse.h>
#include <cpuid.h>
#include <rballoc.h>

const uint16_t boot_info_irom_x15[] = {

        /* for A5S, we have 5 virtual boot modes
         * 0: irom debug
         * 1: nand busw=8
         * 2: nand busw=16
         * 3: mmc0
         * 4: spi0
         */

        5,              /* arrary[0] is used to store total number of devices */
        _BI(_TOID(DEV_MMC(0))),                        /* mmc0 */
        _BI(_TOID(DEV_BND)| _TON(1, 5, 0, 0, 1, 15)),  /* legacy nand busw: 8 */
        _BI(_TOID(DEV_BND)| _TON(1, 5, 0, 1, 1, 15)),  /* legacy nand busw: 16 */
        _BI(_TOID(DEV_FLASH)),
        _BI(_TOID(DEV_BND)| _TON(1, 5, 2, 0, 1, 15)),  /* toggle nand busw: 8 */
};

const uint16_t boot_info_irom_x15_new[] = {

        /* for A5S, we have 5 virtual boot modes
         * 0: irom debug
         * 1: nand busw=8
         * 2: nand busw=16
         * 3: mmc0
         * 4: spi0
         */

        5,              /* arrary[0] is used to store total number of devices */
        _BI(_TOID(DEV_MMC(0))),                        /* mmc0 */
        _BI(_TOID(DEV_BND)| _TON(1, 5, 0, 0, 1, 12)),  /* legacy nand busw: 8 */
        _BI(_TOID(DEV_BND)| _TON(1, 5, 0, 1, 1, 12)),  /* legacy nand busw: 16 */
        _BI(_TOID(DEV_FLASH)),
        _BI(_TOID(DEV_BND)| _TON(1, 5, 2, 0, 1, 12)),  /* toggle nand busw: 8 */
};

const uint16_t boot_info_irom[] = {
	_BI(_TOID(DEV_NONE)),						/* 0: irom debug */

	/* MMC 0, 1, 2 */
	_BI(_TOID(DEV_MMC(0))),						/* 1: mmc0 */
	_BI(_TOID(DEV_MMC(1))), 					/* 2: mmc0 */
	_BI(_TOID(DEV_MMC(2))), 					/* 3: mmc0 */

	/* SPI flash ball out, to be changed */
	_BI(_TOID(DEV_FLASH) | _TOF(22)),			/* 4: spi flash, ball out 77~80 */
	_BI(_TOID(DEV_FLASH) | _TOF(28)),			/* 5: spi flash, ball out 124~127 */
	_BI(_TOID(DEV_FLASH) | _TOF(31)),			/* 6: spi flash, ball out 30~33 */

	/* UDISK */
	_BI(_TOID(DEV_UDISK(0))),					/* 7: u-disk */

	/* SATA2 Hard disk */
	_BI(_TOID(DEV_HD)),							/* 8: hard disk */

	/* IIC EEPROM */
	_BI(_TOID(DEV_EEPROM) | _TOE(0x50, 0)),		/* 9: e2p, 2 cycle */

	/* NAND
	   8	0.5	0	4	legacy	0
	   8	2	0	5	legacy	0
	   8	2	16	5	legacy	0
	   8	2	128	5	legacy	1
	   8	4	16	5	legacy	0
	   8	4	24	5	legacy	0
	   8	4	128	5	legacy	1
	   8	4	128	5	toggle	1
	   8	8	24	5	legacy	1
	   8	8	40	5	toggle	1
	   16	2	0	5	legacy	0
	   16	2	0	5	toggle	1
	   16	2	16	5	legacy	0
	   16	2	16	5	toggle	1
	   16	2	128	5	legacy	0
	   16	2	128	5	toggle	1
	   16	4	16	5	legacy	0
	   16	4	16	5	toggle	1
	*/
	_BI(_TOID(DEV_BND)| _TON(0, 0, 0, 0, 0, 0)),			/* 10 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 1, 0, 0)),			/* 11 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 1, 1, 2)),			/* 12 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 1, 1, 15)),			/* 13 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 2, 1, 3)),			/* 14 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 2, 1, 4)),			/* 15 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 2, 1, 15)),			/* 16 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 2, 1, 15)),			/* 17 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 3, 1, 4)),			/* 18 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 3, 1, 6)),			/* 19 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 5, 0, 0)),			/* 20 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 5, 0, 0)),			/* 21 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 5, 1, 2)),			/* 22 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 5, 1, 2)),			/* 23 */	
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 5, 1, 2)),			/* 24 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 5, 1, 15)),			/* 25 */	
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 5, 1, 15)),			/* 26 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 6, 1, 3)),			/* 27 */

	_BI(BI_RESERVED),						/* 28: nor boot */
	_BI(BI_RESERVED),						/* 29: gps boot */
	_BI(BI_RESERVED),						/* 30: non boot */
	_BI(BI_RESERVED),						/* 31: _ic test */
};

const uint16_t boot_info_irom_new[] = {
	_BI(_TOID(DEV_NONE)),						/* 0: irom debug */

	/* MMC 0, 1, 2 */
	_BI(_TOID(DEV_MMC(0))),						/* 1: mmc0 */
	_BI(_TOID(DEV_MMC(1))), 					/* 2: mmc0 */
	_BI(_TOID(DEV_MMC(2))), 					/* 3: mmc0 */

	/* SPI flash ball out, to be changed */
	_BI(_TOID(DEV_FLASH) | _TOF(22)),			/* 4: spi flash, ball out 77~80 */
	_BI(_TOID(DEV_FLASH) | _TOF(28)),			/* 5: spi flash, ball out 124~127 */
	_BI(_TOID(DEV_FLASH) | _TOF(31)),			/* 6: spi flash, ball out 30~33 */

	/* UDISK */
	_BI(_TOID(DEV_UDISK(0))),					/* 7: u-disk */

	/* SATA2 Hard disk */
	_BI(_TOID(DEV_HD)),							/* 8: hard disk */

	/* IIC EEPROM */
	_BI(_TOID(DEV_EEPROM) | _TOE(0x50, 0)),		/* 9: e2p, 2 cycle */

	/* NAND
	   8	0.5	0	4	legacy	0
	   8	2	0	5	legacy	0
	   8	2	16	5	legacy	0
	   8	2	128	5	legacy	1
	   8	4	16	5	legacy	0
	   8	4	24	5	legacy	0
	   8	4	128	5	legacy	1
	   8	4	128	5	toggle	1
	   8	8	24	5	legacy	1
	   8	8	40	5	toggle	1
	   16	2	0	5	legacy	0
	   16	2	0	5	toggle	1
	   16	2	16	5	legacy	0
	   16	2	16	5	toggle	1
	   16	2	128	5	legacy	0
	   16	2	128	5	toggle	1
	   16	4	16	5	legacy	0
	   16	4	16	5	toggle	1
	*/
	_BI(_TOID(DEV_BND)| _TON(0, 0, 0, 0, 0, 0)),			/* 10 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 1, 0, 0)),			/* 11 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 1, 1, 2)),			/* 12 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 1, 1, 12)),			/* 13 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 2, 1, 3)),			/* 14 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 2, 1, 4)),			/* 15 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 2, 1, 12)),			/* 16 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 2, 1, 12)),			/* 17 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 3, 1, 4)),			/* 18 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 3, 1, 6)),			/* 19 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 5, 0, 0)),			/* 20 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 5, 0, 0)),			/* 21 */
	_BI(_TOID(DEV_BND)| _TON(0, 1, 0, 5, 1, 2)),			/* 22 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 5, 1, 2)),			/* 23 */	
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 5, 1, 2)),			/* 24 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 5, 1, 12)),			/* 25 */	
	_BI(_TOID(DEV_BND)| _TON(1, 1, 2, 5, 1, 12)),			/* 26 */
	_BI(_TOID(DEV_BND)| _TON(1, 1, 0, 6, 1, 3)),			/* 27 */

	_BI(BI_RESERVED),						/* 28: nor boot */
	_BI(BI_RESERVED),						/* 29: gps boot */
	_BI(BI_RESERVED),						/* 30: non boot */
	_BI(BI_RESERVED),						/* 31: _ic test */
};

static uint16_t xom_info = 0;
int get_xom(int mode)
{
	uint32_t pin_stat = 0;

	if(mode)
	  pin_stat = xom_info;
    else if(IROM_IDENTITY == IX_CPUID_X15 || IROM_IDENTITY == IX_CPUID_X15_NEW || IROM_IDENTITY == IX_CPUID_Q3) {
        return rbgetint("bootxom");
    }
	else {
		pin_stat = readb(PADS_IN_GROUP(18));
		pin_stat <<= 1;
		pin_stat |= (readb(PADS_IN_GROUP(17)) >> 7);
		pin_stat &= 0x1f;
	}

	return pin_stat;
}

void set_xom(int xom)
{
	xom_info = xom & 0x1f;
}

void init_xom(void)
{
	xom_info = get_xom(0);
}

uint16_t boot_info(void)
{
	uint16_t info;
	struct efuse_paras *ecfg = ecfg_paras();

	/* read from _x15 if X15 CPU */
	if(IROM_IDENTITY == IX_CPUID_X15_NEW)
	  return boot_info_irom_x15_new[xom_info % (boot_info_irom_x15_new[0] + 1)];
	if(IROM_IDENTITY == IX_CPUID_X15 )
	  return boot_info_irom_x15[xom_info % (boot_info_irom_x15[0] + 1)];

	if(ecfg_check_flag(ECFG_DEFAULT_BINFO))
	{
		if(IROM_IDENTITY == IX_CPUID_X15_NEW)
			info = boot_info_irom_new[xom_info & 0x1f];
		else
			info = boot_info_irom[xom_info & 0x1f];
	}
	else {
		if(xom_info < 16 || xom_info > 27)
		{
			if(IROM_IDENTITY == IX_CPUID_X15_NEW)
				info = boot_info_irom_new[xom_info & 0x1f];
			else
				info = boot_info_irom[xom_info & 0x1f];
		}
		else
			info = ecfg->binfo[xom_info - 16];
	}

	return info;
}

int boot_device(void) {
	return boot_info_to_id(boot_info());
}

#if 0
void binfo_test(void) {
	int i;

	for(i = 0; i < 32; i++) {
		set_xom(i);
		printf("pin:%d==>0x%04x\n", i, boot_info());
	}
}
#endif

