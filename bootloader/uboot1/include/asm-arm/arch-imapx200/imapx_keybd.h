#ifndef __IMAPX_KEYBOARD_H__
#define __IMAPX_KEYBOARD_H__

#define KBCON		(KEYBD_BASE_REG_PA+0x00)     /* KB Control Register*/
#define KBCKD		(KEYBD_BASE_REG_PA+0x04)     /* KB Clock Divider Register*/
#define KBDCNT		(KEYBD_BASE_REG_PA+0x08)     /* Debouncing Filter Counter*/
#define KBINT		(KEYBD_BASE_REG_PA+0x0C)     /* Interrupt Status Register*/
#define KBCOLD		(KEYBD_BASE_REG_PA+0x10)     /* Key Column Data Register*/
#define KBCOEN		(KEYBD_BASE_REG_PA+0x14)     /* Column Output Enable Register*/
#define KBRPTC		(KEYBD_BASE_REG_PA+0x18)     /* repeat scan period Register*/
#define KBROWD0		(KEYBD_BASE_REG_PA+0x1C)     /* Row Data Register 0*/
#define KBROWD1		(KEYBD_BASE_REG_PA+0x20)     /* Row Data Register 1*/
#define KBROWD2		(KEYBD_BASE_REG_PA+0x24)     /* Row Data Register 2*/
#define KBROWD3		(KEYBD_BASE_REG_PA+0x28)     /* Row Data Register 3*/
#define KBROWD4		(KEYBD_BASE_REG_PA+0x2C)     /* Row Data Register 4*/

#define KBCON_MSCAN	(1 << 8)
#define KBCON_RPTEN	(1 << 7)
#define KBCON_SCANST	(1 << 6)
#define KBCON_CLKSEL	(1 << 5)
#define KBCON_FCEN	(1 << 4)
#define KBCON_DFEN	(1 << 3)
#define KBCON_DRDYINTEN	(1 << 2)
#define KBCON_PINTEN	(1 << 1)
#define KBCON_FINTEN	(1 << 0)

#define KBDCNT_DRDYINT	(1 <<16)

#define KBCOEN_COLNUM	(17 << 20)
#define KBCOEN_COLOEN	( 0 )

#endif /*__IMAPX_KEYBOARD_H__*/
