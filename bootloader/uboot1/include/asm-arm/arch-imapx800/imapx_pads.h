#ifndef __IMAPX_PADS_H__
#define __IMAPX_PADS_H__

#define	PADS_SDIO_STRENGTH		(PAD_SYSM_ADDR + 0x004 )	/* 0: 12mA, 1: 16mA */
#define	PADS_SDIO_IN_EN			(PAD_SYSM_ADDR + 0x008 )	/* enable input */
#define	PADS_SDIO_PULL			(PAD_SYSM_ADDR + 0x00c )	/* 0: down, 1: up */
#define	PADS_SDIO_OD_EN			(PAD_SYSM_ADDR + 0x010 )	/* enable open drain */
#define	PADS_PULL(x)			(PAD_SYSM_ADDR + 0x014 + (x << 2))	/* 1 active */
#define	PADS_SOFT(x)			(PAD_SYSM_ADDR + 0x06c + (x << 2))	/* 1 active */
#define	PADS_OUT_EN(x)			(PAD_SYSM_ADDR + 0x0c4 + (x << 2))	/* 0 active */
#define	PADS_OUT_GROUP(x)		(PAD_SYSM_ADDR + 0x11c + (x << 2))
#define	PADS_IN_GROUP(x)		(PAD_SYSM_ADDR + 0x174 + (x << 2))
#define	PADS_PULL_UP(x)			(PAD_SYSM_ADDR + 0x1cc + (x << 2)) /* 1:hi, 0:low */
 #define PADS_SD_JTAG            (PAD_SYSM_ADDR + 0x3e0)     /* 0: jtag/uart interface, 1: sd interface */

#define GPIO_INDAT(x)           (GPIO_BASE_ADDR + (x) * 0x40 + 0x0)
#define GPIO_OUTDAT(x)          (GPIO_BASE_ADDR + (x) * 0x40 + 0x4)
#define GPIO_OUT_EN(x)          (GPIO_BASE_ADDR + (x) * 0x40 + 0x8)

#define PADS_MODE_CTRL			0
#define PADS_MODE_IN			1
#define PADS_MODE_OUT			2

#define PIN_HIGH			1
#define PIN_LOW				0

#define PADS2INDEX(r, p)		((r << 3) + p)
#define PADSRANGE(a, b)			(b << 16 | a)

#endif /*__IMAPX_PADS_H__*/

