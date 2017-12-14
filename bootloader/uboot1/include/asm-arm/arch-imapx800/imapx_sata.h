#ifndef __IMAPX_SATA_H__
#define __IMAPX_SATA_H__


/* sata for Cortex-A5 */

/*SYSMGR*/
#define ACJT_LVL						(SATA_SYSM_ADDR + 0x20)
#define RX_EQ_VAL_0						(SATA_SYSM_ADDR + 0X24)
#define MPLL_NCY						(SATA_SYSM_ADDR + 0x28)
#define MPLL_NCY5						(SATA_SYSM_ADDR + 0x2C)
#define MPLL_CK_OFF						(SATA_SYSM_ADDR + 0x30)
#define MPLL_PRESCALE						(SATA_SYSM_ADDR + 0X34)
#define MPLL_SS_SEL						(SATA_SYSM_ADDR + 0x38)
#define MPLL_SS_EN						(SATA_SYSM_ADDR + 0x3C)
#define TX_EDGERATE_0						(SATA_SYSM_ADDR + 0x40)
#define LOS_LVL							(SATA_SYSM_ADDR + 0x44)
#define USE_REFCLK_ALT						(SATA_SYSM_ADDR + 0x48)
#define REF_CLK_SEL						(SATA_SYSM_ADDR + 0x4C)
#define TX_LVL							(SATA_SYSM_ADDR + 0x50)
#define TX_ATTEN_0						(SATA_SYSM_ADDR + 0x54)
#define TX_BOOST_0						(SATA_SYSM_ADDR + 0x58)
#define RX_DPLL_MODE_0						(SATA_SYSM_ADDR + 0x5C)
#define RX_ALIGN_EN						(SATA_SYSM_ADDR + 0x60)
#define MPLL_PROP_CTL						(SATA_SYSM_ADDR + 0x64)
#define MPLL_INT_CTL						(SATA_SYSM_ADDR + 0x68)
#define LOS_CTL_0						(SATA_SYSM_ADDR + 0X6C)
#define RX_TERM_EN						(SATA_SYSM_ADDR + 0x70)
#define SATA_SPDMODE_CTL					(SATA_SYSM_ADDR + 0x74)
#define BS_CTL_CFG						(SATA_SYSM_ADDR + 0x78)

/*Generic Host Register*/
#define CAP							(SATA2_BASE_ADDR + 0x00)		/*HBA Capabilities Register*/
#define GHC							(SATA2_BASE_ADDR + 0x04)		/*Global HBA Control Register*/
#define IS							(SATA2_BASE_ADDR + 0x08)		/*Interrupt Status Register*/
#define PI							(SATA2_BASE_ADDR + 0x0C)		/*Port Implemented Register*/
#define VS							(SATA2_BASE_ADDR + 0x10)		/*AHCI Version Register*/
#define CCC_CTL							(SATA2_BASE_ADDR + 0x14)		/*Command Completion Coalescing Control*/
#define CCC_PORTS						(SATA2_BASE_ADDR + 0x18)		/*Command Completion Coalescing Ports*/
#define CAP2							(SATA2_BASE_ADDR + 0x24)		/*HBA Capabilities Extended Register*/
#define BISTAFR							(SATA2_BASE_ADDR + 0xA0)		/*BIST Activate FIS Register*/
#define BISTCR							(SATA2_BASE_ADDR + 0xA4)		/*BIST Control Register*/
#define BISTFCTR						(SATA2_BASE_ADDR + 0xA8)		/*BIST FIS Count Register*/
#define BISTSR							(SATA2_BASE_ADDR + 0xAC)		/*BIST Status Register*/
#define BISTDECR						(SATA2_BASE_ADDR + 0xB0)		/*BIST Dword Error Count Register*/
#define OOBR							(SATA2_BASE_ADDR + 0xBC)		/*OOB Register*/
#define GPCR							(SATA2_BASE_ADDR + 0xD0)		/*General Purpose Control Register*/
#define GPSR							(SATA2_BASE_ADDR + 0xD4)		/*General Purpose Status Register*/
#define TIMER1MS						(SATA2_BASE_ADDR + 0xE0)		/*Timer 1-ms Register*/
#define GPARAM1R						(SATA2_BASE_ADDR + 0xE8)		/*Global Parameter 1 Register*/
#define GPARAM2R						(SATA2_BASE_ADDR + 0xEC)		/*Global Parameter 2 Register*/
#define PPARAM							(SATA2_BASE_ADDR + 0xF0)		/*Port Parameter Register*/
#define TESTR							(SATA2_BASE_ADDR + 0xF4)		/*Test Register*/
#define VERSIONR						(SATA2_BASE_ADDR + 0xF8)		/*Version Register*/
#define IDR							(SATA2_BASE_ADDR + 0xFC)		/*ID Register*/

/*Port Register*/
/*Port0 Register */
#define P0CLB							(SATA2_BASE_ADDR + 0x100)	/*Port0 Command List Base Address Register*/
#define P0CLBU							(SATA2_BASE_ADDR + 0x104)	/*Port0 Command List Base Address Upper 32-bit Register*/
#define P0FB							(SATA2_BASE_ADDR + 0x108)	/*Port0 FIS Base Address Register*/
#define P0FBU							(SATA2_BASE_ADDR + 0x10C)	/*Port0 FIS Base Address Upper 32-bit Register*/
#define P0IS							(SATA2_BASE_ADDR + 0x110)	/*Port0 Interrupt Status Register*/
#define P0IE							(SATA2_BASE_ADDR + 0x114)	/*Port0 Interrupt Enable Register*/
#define P0CMD							(SATA2_BASE_ADDR + 0x118)	/*Port0 Command Register*/
#define P0TFD							(SATA2_BASE_ADDR + 0x120)	/*Port0 Task File Data Register*/
#define P0SIG							(SATA2_BASE_ADDR + 0x124)	/*Port0 Signature Register*/
#define P0SSTS							(SATA2_BASE_ADDR + 0x128)	/*Port0 Serial ATA Status Register*/
#define P0SCTL							(SATA2_BASE_ADDR + 0x12C)	/*Port0 Serial ATA Control Register*/
#define P0SERR							(SATA2_BASE_ADDR + 0x130)	/*Port0 Serial ATA Error Register*/
#define P0SACT							(SATA2_BASE_ADDR + 0x134)	/*Port0 Serial ATA Activate Register*/
#define P0CI							(SATA2_BASE_ADDR + 0x138)	/*Port0 Command Issue Register*/
#define P0SNTF							(SATA2_BASE_ADDR + 0x13C)	/*Port0 Serial ATA Notification Register*/
#define P0DMACR							(SATA2_BASE_ADDR + 0x170)	/*Port0 DMA Control Register*/
#define P0PHYCR							(SATA2_BASE_ADDR + 0x178)	/*Port0 PHY Control Register*/
#define P0PHYSR							(SATA2_BASE_ADDR + 0x17C)	/*Port0 PHY Status Register*/

#define SAHCI_MAX_PRD_ENTRIES (16)


#endif /*__IMAPX_SATA_H__*/

