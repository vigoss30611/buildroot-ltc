#ifndef _IROM_BOOT_LIST_H__
#define _IROM_BOOT_LIST_H__

#include <isi.h>
/* boot device has the following features. 
 * 1. the first three boot from start, others boot from 2MB
 * 2. device from mmc0 to udc can do burn task
 */

#define DEV_BND			0x0
#define DEV_NAND		0x1
#define DEV_FND			0x2
#define DEV_EEPROM		0x3
#define DEV_FLASH		0x4
//#define DEV_HD			0x5
#define DEV_MMC(x)		(0x6 + x)
#define DEV_UDISK(x)	(0x9 + x)
#define DEV_UDC			0xb
#define DEV_ETH			0xc
#define DEV_RAM			0xd

#define DEV_CURRENT		0xe
#define DEV_IUS			0xf
#define DEV_SNND		0x5//spi nand flash 
#define DEV_NONE		0x11

#define _BI(x...) (x)
#define BI_RESERVED		0xffff

#define DEV_UBOOT0		0
#define DEV_UBOOT1		1
#define DEV_ITEM		2
/* shift to device ID */
#define _TOID(x) ((x & 0xf) << 12)

/* shift to NAND parameters
 * a: randomizer [0]
 * b: address cycle [1]
 * c: interface [3:2]
 * d: page size [6:4]
 * e: ECC enable [7]
 * f: ECC level [11:8]
 */
#define _TON(a, b, c, d, e, f)	 \
	((a & 0x1) | ((b & 0x1) << 1) |	 \
	((c & 0x3) << 2) | ((d & 0x7) << 4) |	\
	((e & 0x1) << 7) | ((f & 0xf) << 8))
	
/* shift to SPI flash parameters
 * a: UI MUX value
 */
#define _TOF(a) (a & 0x1f)

/* shift to EEPROM parameters
 * a: i2c address
 * b: address cycles
 */
#define _TOE(a, b)	 \
	((a & 0x7f) | ((b & 0x3) << 7))

#define boot_info_to_id(x) ((x >> 12) & 0xf)

extern const uint16_t  boot_info_irom[];

extern int boot_device(void);
extern uint16_t boot_info(void);
extern void boot_do_boot(struct isi_hdr *hdr, void *data);
extern void set_xom(int info);
extern int get_xom(int mode);
extern void init_xom(void);

#define BL_DEFAULT_LOCATION		0x200000		/* 2m */
#define BL_SIZE_FIXED			0x8000			/* 32k */
#define BL1_SIZE_FIXED			0xc000			/* 512k */
#define BL_PROPERTY_BURN_MASK	0x7fffffff
#define BL_PROPERTY_BURN_VERIFY (1 << 31)
#define BL_SEEK_BOOT			0x1000000		/* seek boot for 16MB */

#endif /* _IROM_BOOT_LIST_H__ */

