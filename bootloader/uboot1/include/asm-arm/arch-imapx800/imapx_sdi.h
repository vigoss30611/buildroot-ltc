
#ifndef __IMAPX800_SDI_H__
#define __IMAPX800_SDI_H__

/* SDHC */
#define SDMMC_CTRL		0x00	// Control register
#define SDMMC_PWREN		0x04	// Power-enable register
#define SDMMC_CLKDIV		0x08	// Clock-divider register
#define SDMMC_CLKSRC		0x0c	// Clock-source register
#define SDMMC_CLKENA		0x10	// Clock-enable register
#define SDMMC_TMOUT		0x14	// Time-out register
#define SDMMC_CTYPE		0x18	// Card-type register
#define SDMMC_BLKSIZ		0x1c	// Block-size register
#define SDMMC_BYTCNT		0x20	// Byte-count register
#define SDMMC_INTMASK		0x24	// Interrupt-mask register
#define SDMMC_CMDARG		0x28	// Command-argument register
#define SDMMC_CMD		0x2c	// Command register
#define SDMMC_RESP0		0x30	// Response-0 register
#define SDMMC_RESP1		0x34	// Response-1 register
#define SDMMC_RESP2		0x38	// Response-2 register
#define SDMMC_RESP3		0x3c	// Response-3 register
#define SDMMC_MINTSTS		0x40	// Masked interrupt-status register
#define SDMMC_RINTSTS		0x44	// Raw interrupt-status register
#define SDMMC_STATUS		0x48	// Status register; mainly for debug purposes
#define SDMMC_FIFOTH		0x4c	// FIFO threshold register
#define SDMMC_CDETECT		0x50	// Card-detect register
#define SDMMC_WRTPRT		0x54	// Write-protect register
#define SDMMC_DLYLINE		0x58	// Delay line register
#define SDMMC_TCBCNT		0x5c	//Transferred CIU card byte count
#define SDMMC_TBBCNT		0x60	// Transferred host/DMA to/from BIU-FIFO byte count
#define SDMMC_DEBNCE		0x64	// Card detect debounce register
#define SDMMC_USRID		0x68	// User ID register
#define SDMMC_VERID		0x6c	// Synopsys version ID register
#define SDMMC_HCON		0x70	// Hardware configuration register
#define SDMMC_UHSREG		0x74	// UHS-1 register
#define SDMMC_RSTN		0x78	// Hardware reset register
#define SDMMC_BMOD		0x80	// Bus Mode Register; controls the Host Interface Mode
#define SDMMC_PLDMND		0x84	// Poll Demand Register.
#define SDMMC_DBADDR		0x88	// Descriptor List Base Address Register
#define SDMMC_IDSTS		0x8c	// Internal DMAC Status Register
#define SDMMC_IDINTEN		0x90	// Internal DMAC Interrupt Enable Register
#define SDMMC_DSCADDR		0x94	// Current Host Descriptor Address Register
#define SDMMC_BUFADDR		0x98	// Current Host Buffer Address Register
#define SDMMC_CARDTHRCTL	0x100	// Card Read Threshold Enable
#define SDMMC_BEP		0x104	// Back-end Power
#define SDMMC_DATA		0x200	// Data FIFO read/write

/* Interrupt status & mask register defines */
#define SDMMC_INT_SDIO			(1<<16)
#define SDMMC_INT_EBE			(1<<15)
#define SDMMC_INT_ACD			(1<<14)
#define SDMMC_INT_SBE			(1<<13)
#define SDMMC_INT_HLE			(1<<12)
#define SDMMC_INT_FRUN			(1<<11)
#define SDMMC_INT_HTO			(1<<10)
#define SDMMC_INT_DTO			(1<<9)
#define SDMMC_INT_RTO			(1<<8)
#define SDMMC_INT_DCRC			(1<<7)
#define SDMMC_INT_RCRC			(1<<6)
#define SDMMC_INT_RXDR			(1<<5)
#define SDMMC_INT_TXDR			(1<<4)
#define SDMMC_INT_DATA_OVER	(1<<3)
#define SDMMC_INT_CMD_DONE	(1<<2)
#define SDMMC_INT_RESP_ERR	(1<<1)
#define SDMMC_INT_CD			(1<<0)
#define SDMMC_INT_ERROR		0xbfc2


/* Command register defines */
#define SDMMC_CMD_START		(1<<31)
#define SDMMC_CMD_USEHOLD		(1<<29)
#define SDMMC_CMD_CCS_EXP		(1<<23)
#define SDMMC_CMD_CEATA_RD	(1<<22)
#define SDMMC_CMD_UPD_CLK		(1<<21)
#define SDMMC_CMD_INIT			(1<<15)
#define SDMMC_CMD_STOP		(1<<14)
#define SDMMC_CMD_PRV_DAT_WAIT	(1<<13)
#define SDMMC_CMD_SEND_STOP		(1<<12)
#define SDMMC_CMD_STRM_MODE		(1<<11)
#define SDMMC_CMD_DAT_WR		(1<<10)
#define SDMMC_CMD_DAT_EXP		(1<<9)
#define SDMMC_CMD_RESP_CRC	(1<<8)
#define SDMMC_CMD_RESP_LONG	(1<<7)
#define SDMMC_CMD_RESP_EXP	(1<<6)
#define SDMMC_CMD_INDX(n)		((n) & 0x1F)

#define SDMMC_FIFO_SZ		512
#define SDMMC_GET_FCNT(x)               (((x)>>17) & 0xFFF)


#define DW_MCI_DATA_ERROR_FLAGS (SDMMC_INT_DTO | SDMMC_INT_DCRC | \
		  SDMMC_INT_HTO | SDMMC_INT_SBE  | \
		  SDMMC_INT_EBE)
#define DW_MCI_CMD_ERROR_FLAGS  (SDMMC_INT_RTO | SDMMC_INT_RCRC | \
		  SDMMC_INT_RESP_ERR)


#endif /* __IMAPX800_SDI_H__ */

