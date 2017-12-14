/* ****************************************************************************** 
 * ** regs-sdhs.h 
 * ** 
 * ** Copyright (c) 2009~2014 ShangHai Infotm .Ltd all rights reserved. 
 * ** 
 * ** Use of Infotm's code is governed by terms and conditions 
 * ** stated in the accompanying licensing statement. 
 * ** 
 * ** Description: Main file of SD/MMC driver.
 * **
 * ** Author:
 * **    Haixu Fu    <haixu_fu@infotm.com>
 * **     
 * ** Revision History: 
 * ** ----------------- 
 * ** 1.1  08/17/2009  Haixu Fu   
 * *****************************************************************************/ 

//#ifndef __ASM_ARCH_REGS_HSMMC_H
//#define __ASM_ARCH_REGS_HSMMC_H __FILE__



/* Controller Registers */

#define         IMAP_SDMASYSAD          0x00  //SDMA address register	              [30:0]
#define         IMAP_BLKSIZE            0x04  //transfer block size                   [15:0]
#define         IMAP_BLKCNT             0x06  //block count for current transfer      [15:0]
#define         IMAP_ARGUMENT           0x08  //command argument register             [31:0]
#define         IMAP_TRNMOD             0x0C  //transfer mode setting reg             [15:0]
#define         IMAP_CMDREG             0x0E  //command register                      [15:0]
#define         IMAP_RSPREG0            0x10  //response register0                    [31:0]
#define         IMAP_RSPREG1            0x14  //response register1                    [31:0]
#define         IMAP_RSPREG2            0x18  //response register2                    [31:0]
#define         IMAP_RSPREG3            0x1C  //response register3                    [31:0]
#define         IMAP_BDATA              0x20  //buffer data port register             [31:0]
#define         IMAP_PRNSTS             0x24  //present state register                [31:0]
#define         IMAP_HOSTCTL            0x28  //host controller register              [7:0]
#define         IMAP_PWRCON             0x29  //power controll register               [7:0]
#define         IMAP_BLKGAP             0x2A  //block gap control register            [7:0]
#define		IMAP_WAKUP		0x2B  //wake up register 		      [7:0]
#define         IMAP_CLKCON             0x2C  //clock controll register               [15:0]
#define         IMAP_TIMEOUTCON         0x2E  //timeout control register              [7:0]
#define         IMAP_SWORST             0x2F  //software reset regidter               [7:0]
#define         IMAP_NORINTSTS          0x30  //normal interrupt status regiter       [15:0]
#define         IMAP_ERRINTSTS          0x32  //error interrupt status register       [15:0]
#define         IMAP_NORINTSTSEN        0x34  //normal interrupt status enable reg    [15:0]
#define         IMAP_ERRINTSTSEN        0x36  //error int status enable reg           [15:0]
#define         IMAP_NORINTSIGEN        0x38  //normal int signal enable reg          [15:0]
#define         IMAP_ERRINTSIGEN        0x3A  //error int signal enable reg           [15:0]
#define         IMAP_ACMD12ERRSTS       0x3C  //auto cmd12 error status reg           [15:0]
#define         IMAP_CAPAREG            0x40  //capabilities register                 [32:0]
#define         IMAP_MAXCURR            0x48  //max current capabilities reg          [7:0]
#define         IMAP_CONTROL2           0x80  //control reg 2                         [31:0]
#define         IMAP_CONTROL3           0x84  //fifo interrput control                [31:0]
#define         IMAP_CONTROL4           0x8C  //control register 4                    [31:0]
#define         IMAP_FEAER              0x50  //forcr event for cmd12 error int reg   [15:0]
#define         IMAP_FEERR              0x52  //FORCE event error int reg error int   [15:0]
#define         IMAP_ADMAERR            0x54  //ADMA error status register            [31:0]
#define         IMAP_ADMASYSADDR        0x58  //ADMA system address   reg             [31:0]
#define		IMAP_SLOTINT		0xFC  //slot int status   reg		      [15:0]
#define         IMAP_HCVER              0xFE  //HOST controller       vesion reg      [15:0]

/*for COMMAND */
#define 	IMAP_MAKE_CMD(c, f) (((c & 0xff) << 8) | (f & 0xff)) 
#define		IMAP_CMD_RESPNONE		0x00
#define		IMAP_CMD_RESPLONG		0x01
#define		IMAP_CMD_RESPSHORT		0x02
#define		IMAP_CMD_RESPSHORTBUSY		0x03
#define		IMAP_CMD_CRC			0x08
#define		IMAP_CMD_INDEX			0x10
#define		IMAP_CMD_DATA			0x20


/* for  PRNSTS */
#define         IMAP_PRNSTS_CMDINHIBIT          0x00000001
#define		IMAP_PRNSTS_DATAINHIBIT		0x00000002
#define		IMAP_PRNSTS_SPACE		0x00000400
#define		IMAP_PRNSTS_DATA		0x00000800
#define		IMAP_PRNSTS_CARDIN		16
#define		IMAP_PRNSTS_WPROTECT		0x00080000
/*for   CLKCON */
#define		IMAP_CLKCON_INTEN		0x0001
#define		IMAP_CLKCON_INTSTABLE		0x0002
#define		IMAP_CLKCON_CARDEN		0x0004
#define		IMAP_CLKCON_SHIFT		8
/*for PWRCON */
#define		IMAP_PWRCON_PWRON		0x01
#define		IMAP_PWRCON_POWER_180		0x0A
#define		IMAP_PWRCON_POWER_300		0x0C
#define		IMAP_PWRCON_POWER_330		0x0E

/* for SWRST */
#define 	IMAP_SWORST_ALL   		0x01  	
#define		IMAP_SWORST_CMD			0x02
#define		IMAP_SWORST_DATA		0x04

/* for NORINTSTS  and  ERRINTSTS */
#define		IMAP_NORINTSTS_CMDCMP		0x00000001
#define		IMAP_NORINTSTS_TRNCMP		0x00000002
#define		IMAP_NORINTSTS_DMACMP		0x00000008
#define		IMAP_NORINTSTS_SPACE		0x00000010
#define		IMAP_NORINTSTS_DATA		0x00000020
#define		IMAP_NORINTSTS_CARDIN		0x00000040
#define		IMAP_NORINTSTS_CARDRM		0x00000080
#define		IMAP_NORINTSTS_CARD		0x00000100
#define		IMAP_NORINTSTS_ERR		0x00008000
#define		IMAP_ERRINTSTS_TOUT		0x00010000
#define		IMAP_ERRINTSTS_CRC		0x00020000
#define		IMAP_ERRINTSTS_ENDBIT		0x00040000	
#define		IMAP_ERRINTSTS_INDEX		0x00080000
#define		IMAP_ERRINTSTS_TRNTOUT		0x00100000
#define		IMAP_ERRINTSTS_TRNCRC		0x00200000
#define		IMAP_ERRINTSTS_TRNENDBIT	0x00400000
#define		IMAP_ERRINTSTS_BUSPOWER		0x00800000
#define		IMAP_ERRINTSTS_ADMAERR		0x02000000

#define		IMAP_CMD_MASK	(IMAP_NORINTSTS_CMDCMP | IMAP_ERRINTSTS_TOUT | \
			IMAP_ERRINTSTS_ENDBIT | IMAP_ERRINTSTS_CRC | IMAP_ERRINTSTS_INDEX)

#define		IMAP_DATA_MASK  (IMAP_NORINTSTS_TRNCMP | IMAP_NORINTSTS_DMACMP | IMAP_NORINTSTS_SPACE | \
			IMAP_NORINTSTS_DATA | IMAP_ERRINTSTS_TRNTOUT | IMAP_ERRINTSTS_TRNCRC | IMAP_ERRINTSTS_TRNENDBIT | \
			IMAP_ERRINTSTS_ADMAERR)

#define		IMAP_INT_ALL_MASK		((unsigned int)-1)
/* for IMAP_BLKSIZE */

 #define  IMAP_MAKE_BLKSZ(dma, blksz) (((dma & 0x7) << 12) | (blksz & 0xFFF))

/* for NORINTSTSEN */
#define 	IMAP_NORINTSTSEN_CMD		0x0001
#define		IMAP_NORINTSTSEN_DATA		0x0002
#define		IMAP_NORINTSTSEN_DMA		0x0008
#define		IMAP_NORINTSTSEN_BUFFRD 	0x0010
#define		IMAP_NORINTSTSEN_BUFFWR		0x0020
#define		IMAP_NORINTSTSEN_CARDIN		0x0040
#define		IMAP_NORINTSTSEN_CARDRM		0x0080

/*for ERRINTSTSEN */

#define 	IMAP_ERRINTSTSEN_TIMEOUT	0x0001
#define		IMAP_ERRINTSTSEN_CRC		0x0002
#define		IMAP_ERRINTSTSEN_ENDBIT		0x0004
#define		IMAP_ERRINTSTSEN_INDEX		0x0008
#define		IMAP_ERRINTSTSEN_DATATIMEOUT	0x0010
#define		IMAP_ERRINTSTSEN_DATACRC	0x0020
#define		IMAP_ERRINTSTSEN_DATAENDBIT	0x0040
#define		IMAP_ERRINTSTSEN_BUSPOWER	0x0080
 
/* for   TRNMOD */
#define		IMAP_TRNMOD_ENDMA		0x01
#define		IMAP_TRNMOD_ENBLKCNT		0x02
#define		IMAP_TRNMOD_ENACMD12		0x04
#define		IMAP_TRNMOD_READ		0x10
#define		IMAP_TRNMOD_MUTI		0x20


/* for  CMDREG */
#define		IMAP_CMDREG_RESPNONE		0x0000
#define		IMAP_CMDREG_RESPLONG		0x0001
#define		IMAP_CMDREG_RESPSHORT		0x0002
#define		IMAP_CMDREG_RESPBUSY		0x0003		
#define		IMAP_CMDREG_CRC			0x0008
#define		IMAP_CMDREG_INDEX		0x0010
#define		IMAP_CMDREG_DATA		0x0020
#define		IMAP_CMDREG_ACMD12		0x00C0	

/* for HOSTCTL */
#define		IMAP_HOSTCTL_LED		0x01
#define 	IMAP_HOSTCTL_4BIT		0x02
#define		IMAP_HOSTCTL_HS			0x04
#define		IMAP_HOSTCTL_SDMA		0x00
#define		IMAP_HOSTCTL_ADMA32		0x10
#define		IMAP_HOSTCTL_ADMA64		0x18
#define		IMAP_HOSTCTL_8BIT		0x20
#define		IMAP_HOSTCTL_DMAMASK		0x18

/* for CAPAREG */
#define		IMAP_CAPAREG_TOUTUNIT		0x00000080
#define 	IMAP_CAPAREG_ADMA2		0x00080000		
#define		IMAP_CAPAREG_HS			0x00200000
#define		IMAP_MAX_BLOCK_MASK		0x00030000
#define		IMAP_MAX_BLOCK_SHIFT		16
#define		IMAP_CAPAREG_DMA		0x00400000
#define		IMAP_CAPAREG_SR			0x00800000
#define		IMAP_CAPAREG_330		0x01000000
#define		IMAP_CAPAREG_300		0x02000000
#define		IMAP_CAPAREG_180		0x04000000


/* for HCVER */
#define		IMAP_SPEC_MASK			0x00FF
#define		IMAP_SPEC_SHIFT			0
#define		IMAP_SPEC_100			0
#define		IMAP_SPEC_200			1

/* for  CONTROLL2 */
#define		IMAP_CONTROL2_SELBASECLK_MASK	(0x3 << 4)
#define		IMAP_CONTROL2_ORIGINAL		0xC0004100
#define		IMAP_CONTROL2_SELBASECLK_SHIFT	(4)
/* for  CONTROLL3 */
#define		IMAP_CONTROL3_FCSEL3            (1 << 31)
#define		IMAP_CONTROL3_FCSEL2		(1 << 23)
#define		IMAP_CONTROL3_FCSEL1		(1 << 15)
#define		IMAP_CONTROL3_FCSEL0		(1 << 7)
/* for  CONTROLL4 */
#define		IMAP_CONTROL4_2MA		(0x0 << 16)
#define		IMAP_CONTROL4_4MA		(0x1 << 16)
#define		IMAP_CONTROL4_7MA		(0x2 << 16)
#define		IMAP_CONTROL4_9MA		(0x3 << 16)



