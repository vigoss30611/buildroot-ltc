
#ifndef __IMAPX800_NAND_H__
#define __IMAPX800_NAND_H__

/* NF2 */
#define NF2AFTM				(NF2_BASE_ADDR+0x00)		/* NAND Flash AsynIF timing */
#define NF2SFTM				(NF2_BASE_ADDR+0x04)      	/* NAND Flash SynIF timing */
#define NF2TOUT				(NF2_BASE_ADDR+0x08)      	/* NAND Flash RnB timeout cfg. */
#define NF2STSC				(NF2_BASE_ADDR+0x0C)      	/* NAND Flash status check cfg. */
#define NF2STSR0			(NF2_BASE_ADDR+0x10)      	/* NAND Flash Status register 0, read only */
#define NF2STSR1			(NF2_BASE_ADDR+0x14)      	/* NAND Flash Status register 1, read only */
#define NF2ECCC				(NF2_BASE_ADDR+0x18)       /*  ECC configuration */
#define NF2PGEC				(NF2_BASE_ADDR+0x1C)       /*  Page configuration */
#define NF2RADR0			(NF2_BASE_ADDR+0x20)		/* NAND Flash Row ADDR0 */
#define NF2RADR1			(NF2_BASE_ADDR+0x24)		/* NAND Flash Row ADDR1 */
#define NF2RADR2			(NF2_BASE_ADDR+0x28)		/* NAND Flash Row ADDR2 */
#define NF2RADR3			(NF2_BASE_ADDR+0x2C)		/* NAND Flash Row ADDR3 */
#define NF2CADR				(NF2_BASE_ADDR+0x30)       /* Column Address */
#define NF2AADR				(NF2_BASE_ADDR+0x34)       /* ADMA descriptor address */
#define NF2SADR0			(NF2_BASE_ADDR+0x38)       /* SDMA Ch0 address */
#define NF2SADR1			(NF2_BASE_ADDR+0x3C)       /* SDMA Ch1 address */
#define NF2SBLKS			(NF2_BASE_ADDR+0x40)       /* SDMA block size */
#define NF2SBLKN			(NF2_BASE_ADDR+0x44)       /* SDMA block number */
#define NF2DMAC				(NF2_BASE_ADDR+0x48)       /* DMA Configuration */
#define NF2CSR				(NF2_BASE_ADDR+0x4C)       /* Chip select register */

#define NF2DATC			(NF2_BASE_ADDR+0x50)       /* data interrupt configuration */
#define NF2FFCE			(NF2_BASE_ADDR+0x54)       /* FIFO Configuration enable */
#define NF2AFPT			(NF2_BASE_ADDR+0x58)       /* AFIFO access register */
#define NF2AFST			(NF2_BASE_ADDR+0x5C)       /* AFIFO status register, read only */
#define NF2TFPT			(NF2_BASE_ADDR+0x60)       /* TRX-AFIFO access register */
#define NF2TFST			(NF2_BASE_ADDR+0x64)       /* TRX-AFIFO status register, read only */
#define NF2TFDE			(NF2_BASE_ADDR+0x68)       /* TRX-AFIFO debug enable */
#define NF2DRDE			(NF2_BASE_ADDR+0x6C)       /* DataRAM debug enable */
#define NF2DRPT			(NF2_BASE_ADDR+0x70)       /* DataRAM read access register, read only */
#define NF2ERDE			(NF2_BASE_ADDR+0x74)       /* ECCRAM debug enable */
#define NF2ERPT			(NF2_BASE_ADDR+0x78)       /* ECCRAM access register */
#define NF2FFTH			(NF2_BASE_ADDR+0x7C)       /* FiFo Threshold */

#define NF2SFTR			(NF2_BASE_ADDR+0x80)       /* Soft reset register */
#define NF2STRR			(NF2_BASE_ADDR+0x84)       /* Start register */
#define NF2INTA			(NF2_BASE_ADDR+0x88)       /* interactive interrrupt ack register */
#define NF2INTR			(NF2_BASE_ADDR+0x8C)       /* interrrupt status register */
#define NF2INTE			(NF2_BASE_ADDR+0x90)       /* interrrupt enable register */
#define NF2WSTA			(NF2_BASE_ADDR+0x94)       /* Work status register, read only */
#define NF2SCADR	    (NF2_BASE_ADDR+0x98)       /* SDMA current address, read only */
#define NF2ACADR	    (NF2_BASE_ADDR+0x9C)       /* ADMA current address, read only */
#define NF2IFST			(NF2_BASE_ADDR+0xA0)       /* ISFIFO status register, read only */
#define NF2OFST			(NF2_BASE_ADDR+0xA4)       /* OSFIFO status register, read only */
#define NF2TFST2	    (NF2_BASE_ADDR+0xA8)       /* TRX-FIFO status register2, read only */
#define NF2WDATL	    (NF2_BASE_ADDR+0xAC)       /* Write data low 32 bits. */
#define NF2WDATH	    (NF2_BASE_ADDR+0xB0)       /* Write data high 32 bits. */
#define NF2DATC2	    (NF2_BASE_ADDR+0xB4)       /* data interrupt configuration2 */

#define NF2PBSTL		(NF2_BASE_ADDR+0xC0)       /* PHY burst length, byte unit */
#define NF2PSYNC	    (NF2_BASE_ADDR+0xC4)       /* PHY Sync mode cfg. */
#define NF2PCLKM	    (NF2_BASE_ADDR+0xC8)       /* PHY Clock mode cfg. */
#define NF2PCLKS	    (NF2_BASE_ADDR+0xCC)       /* PHY Clock STOP cfg. */
#define NF2PRDC			(NF2_BASE_ADDR+0xD0)       /* PHY Read cfg. */
#define NF2PDLY			(NF2_BASE_ADDR+0xD4)       /* PHY Delay line cfg. */
#define NF2RESTCNT		(NF2_BASE_ADDR+0xD8)
#define NF2TRXST		(NF2_BASE_ADDR+0xDC)

#define NF2STSR2			(NF2_BASE_ADDR+0xE0)      	/* NAND Flash Status register 2, read only */
#define NF2STSR3			(NF2_BASE_ADDR+0xE4)      	/* NAND Flash Status register 3, read only */
#define NF2ECCINFO0			(NF2_BASE_ADDR+0xE8)		/* 0-3 block ecc */
#define NF2ECCINFO1			(NF2_BASE_ADDR+0xEC)		/* 4-7 block ecc */
#define NF2ECCINFO2			(NF2_BASE_ADDR+0xF0)		/* 8-11 block ecc */
#define NF2ECCINFO3			(NF2_BASE_ADDR+0xF4)		/* 12-15 block ecc */
#define NF2ECCINFO4			(NF2_BASE_ADDR+0xF8)		/* 16-19 block ecc */
#define NF2ECCINFO5			(NF2_BASE_ADDR+0xFC)		/* 20-23 block ecc */
#define NF2ECCINFO6			(NF2_BASE_ADDR+0x100)		/* 24-27 block ecc */
#define NF2ECCINFO7			(NF2_BASE_ADDR+0x104)		/* 28-31 block ecc */
#define NF2ECCINFO8			(NF2_BASE_ADDR+0x108)		/* ECC unfixed info */
#define NF2ECCINFO9			(NF2_BASE_ADDR+0x10C)		/* secc ecc info, bit 7 vailed, bit 6 unfixed, 3-0 bit errors */

#define NF2ECCLEVEL			(NF2_BASE_ADDR+0x11C)
#define NF2SECCDBG0			(NF2_BASE_ADDR+0x120)
#define NF2SECCDBG1			(NF2_BASE_ADDR+0x124)
#define NF2SECCDBG2			(NF2_BASE_ADDR+0x128)
#define NF2SECCDBG3			(NF2_BASE_ADDR+0x12C)
#define NF2DATSEED0			(NF2_BASE_ADDR+0x140)
#define NF2DATSEED1			(NF2_BASE_ADDR+0x144)
#define NF2DATSEED2			(NF2_BASE_ADDR+0x148)
#define NF2DATSEED3			(NF2_BASE_ADDR+0x14C)
#define NF2ECCSEED0			(NF2_BASE_ADDR+0x150)
#define NF2ECCSEED1			(NF2_BASE_ADDR+0x154)
#define NF2ECCSEED2			(NF2_BASE_ADDR+0x158)
#define NF2ECCSEED3			(NF2_BASE_ADDR+0x15C)
#define NF2RANDMP			(NF2_BASE_ADDR+0x160)
#define NF2RANDME			(NF2_BASE_ADDR+0x164)
#define NF2DEBUG0			(NF2_BASE_ADDR+0x168)
#define NF2DEBUG1			(NF2_BASE_ADDR+0x16c)
#define NF2DEBUG2			(NF2_BASE_ADDR+0x170)
#define NF2DEBUG3			(NF2_BASE_ADDR+0x174)
#define NF2DEBUG4			(NF2_BASE_ADDR+0x178)
#define NF2DEBUG5			(NF2_BASE_ADDR+0x17c)
#define NF2DEBUG6			(NF2_BASE_ADDR+0x180)
#define NF2DEBUG7			(NF2_BASE_ADDR+0x184)
#define NF2DEBUG8			(NF2_BASE_ADDR+0x188)
#define NF2DEBUG9			(NF2_BASE_ADDR+0x18c)
#define NF2DEBUG10			(NF2_BASE_ADDR+0x190)
#define NF2DEBUG11			(NF2_BASE_ADDR+0x194)

#define NF2BSWMODE			(NF2_BASE_ADDR+0x1B8) //bit[0] = b'1' (16bit mode), bit[0] = b'0' (8bit mode)
#define NF2BSWF0			(NF2_BASE_ADDR+0x1BC)
#define NF2BSWF1			(NF2_BASE_ADDR+0x1C0)

#define NF2RAND0			(NF2_BASE_ADDR+0x1C4) //bit[0:15] = seed, bit[16:31] = cycles
#define NF2RAND1			(NF2_BASE_ADDR+0x1C8) //bit[0] = start, frist wirte bit[0] = b'1' then write bit[0] = b'0'
#define NF2RAND2			(NF2_BASE_ADDR+0x1CC) //bit[16] = state [0: finished] [1: busy], bit[0:15] = result

#define DIVCFG4_ECC			(SYSMGR_BASE_ADDR+0x060)

#endif /* __IMAPX800_NAND_H__ */
