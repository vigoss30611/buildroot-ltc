#include <linux/types.h>
#include <common.h>
#include <vstorage.h>
#include <asm/io.h>
#include <bootlist.h>
#include <lowlevel_api.h>
#include <asm/arch/imapx_sata.h>
#include <malloc.h>

//#define SATA_RTL

struct AHCI_H2D_REGISTER_FIS {		/*command FIS*/

	uint8_t FisType;	/*0x27*/
	uint8_t reserved1 :7;
	uint8_t C :1;
	uint8_t Command;
	uint8_t Features;

	uint8_t SectorNumber;
	uint8_t CylLow;
	uint8_t CylHigh;
	uint8_t Dev_Head;

	uint8_t SecNum_Exp;
	uint8_t CylLow_Exp;
	uint8_t CylHigh_Exp;
	uint8_t Features_Exp;

	uint8_t SectorCount;
	uint8_t SectorCount_Exp;
	uint8_t reserved2;
	uint8_t Control;

	uint32_t reserved3;

};

union AHCI_PRDT_DESCRIPTION_INFORMATION {	/*PRD Table Description Information*/

	struct {

		uint32_t DBC :22;	/*data byte count : bit 0 must always be 1 to indicate a even byte count*/
		uint32_t DW3_Reserved :9;
		uint32_t I :1;		/*interrupt on completion : incicate that no data is in the HBA hardware*/

	};

	uint32_t AsUlong;

};

union AHCI_DATA_BASE_ADDRESS {		/*data base address*/

	struct {

		uint32_t DBA :31;	/*data base address : must be word aligned,indicated by bit 0 being reserved*/
		uint32_t DW0_Reserved :1;
	};

	uint32_t AsUlong;

};

struct AHCI_PRDT {	/*PRD table*/

	union AHCI_DATA_BASE_ADDRESS DBA;

	uint32_t DBAU;

	uint32_t reserved;

	union AHCI_PRDT_DESCRIPTION_INFORMATION DI;

};

struct AHCI_COMMAND_TABLE {	/*command table*/

	struct AHCI_H2D_REGISTER_FIS CFIS;

	uint8_t CFIS_Padding[44];

	uint8_t ACMD[16];

	uint8_t reserved1[48];

	struct AHCI_PRDT PRDT[ SAHCI_MAX_PRD_ENTRIES ];

};

union AHCI_COMMAND_HEADER_DESCRIPTION_INFORMATION {	/*Command Header Description Information*/

	struct {

		uint32_t CFL :5;

		uint32_t A :1;

		uint32_t W :1;

		uint32_t P :1;

		uint32_t R :1;

		uint32_t B :1;

		uint32_t C :1;

		uint32_t DW0_Reserved :1;

		uint32_t PMP :4;

		uint32_t PRDTL :16;

	};

	uint32_t AsUlong;

};

union AHCI_COMMAND_TABLE_BASE_ADDRESS {		/*Command Table Base Address*/

	struct {

		uint32_t DW2_Reserved :7;

		uint32_t CTBA :25;

	};

	uint32_t AsUlong;

};

struct AHCI_COMMAND_HEADER {	/*command header*/

	union AHCI_COMMAND_HEADER_DESCRIPTION_INFORMATION DI;

	uint32_t PRDBC; 

	union AHCI_COMMAND_TABLE_BASE_ADDRESS CTBA;

	uint32_t CTBAU;

	uint32_t reserved[4];

};

struct AHCI_D2H_REGISTER_FIS {	/*D2H register FIS*/

	uint8_t FisType;
	uint8_t reserved1 :6;
	uint8_t I:1;
	uint8_t reserved2 :1;
	uint8_t status;
	uint8_t error;

	uint8_t SectorNumber;
	uint8_t CylLow;
	uint8_t CylHigh;
	uint8_t Dev_Head;

	uint8_t SectorNum_Exp;
	uint8_t CylLow_Exp;
	uint8_t CylHigh_Exp;
	uint8_t reserved;

	uint8_t SectorCount;
	uint8_t SectorCount_Exp;
	uint8_t reserved3[2];

	uint8_t reserved4[4];

};

struct AHCI_DMA_SETUP_FIS {	/*DMA setup FIS*/

	uint8_t FisType;
	uint8_t reserved1 :5;
	uint8_t D :1;
	uint8_t I :1;
	uint8_t reserved2 :1;
	uint8_t reserved3[2];

	uint32_t DmaBufferIdentifierLow;
	uint32_t DmaBufferIdentifierHigh;
	uint32_t reserved4;
	uint32_t DmaBufferOffset;
	uint32_t DmaTransferCount;
	uint32_t reserved5;

};

struct AHCI_PIO_SETUP_FIS {	/*PIO setup FIS*/

	uint8_t FisType;
	uint8_t reserved1:5;
	uint8_t D :1;
	uint8_t I :1;
	uint8_t reserved2:1;
	uint8_t status;
	uint8_t error;

	uint8_t SectorNumber;
	uint8_t CylLow;
	uint8_t CylHigh;
	uint8_t Dev_Head;

	uint8_t SectorNumb_Exp;
	uint8_t CylLow_Exp;
	uint8_t CylHigh_Exp;
	uint8_t reserved3;

	uint8_t SectorCount;
	uint8_t SectorCount_Exp;
	uint8_t reserved4;
	uint8_t E_Status;

	uint16_t TransferCount;
	uint8_t reserved5[2];

};

struct AHCI_SET_DEVICE_BITS_FIS {	/*set device bits FIS*/

	uint8_t FisType;

	uint8_t reserved1 :6;
	uint8_t I :1;
	uint8_t Reserved2 :1;

	uint8_t status_Lo :3;
	uint8_t Reserved3 :1;
	uint8_t status_Hi :3;
	uint8_t reserved4 :1;

	uint8_t error;

	uint8_t reserved5[4];

};

struct AHCI_UNKNOWN_FIS {	/*unknown FIS*/

	uint8_t Raw[64];

};

struct AHCI_RECEIVED_FIS {	/*received FIS*/

	struct AHCI_DMA_SETUP_FIS DmaSetupFis;

	uint8_t reserved1[4];

	struct AHCI_PIO_SETUP_FIS PioSetupFis;

	uint8_t reserved2[12];

	struct AHCI_D2H_REGISTER_FIS D2hRegisterFis;

	uint8_t reserved3[4];

	struct AHCI_SET_DEVICE_BITS_FIS SetDeviceBitsFis;

	struct AHCI_UNKNOWN_FIS UnknownFis;

	uint8_t reserved4[96];

};

struct AHCI_BIST_ACTIVATE_FIS {

	unsigned char FisType;
	unsigned char reserved1;
	unsigned char V	:1;
	unsigned char reserved2	:1;
	unsigned char P	:1;
	unsigned char F	:1;
	unsigned char L	:1;
	unsigned char S	:1;
	unsigned char A	:1;
	unsigned char T	:1;
	unsigned char reserved3;

	unsigned char Data[8];
};


static void CreateAtaCFIS(uint32_t CmdTableBaseAddress,
			  uint8_t  Cmd,
			  uint32_t Nsect,
			  uint32_t Lba,
			  int isReset,
			  int isSRST)
{
	struct AHCI_COMMAND_TABLE *cmdtable;
	uint8_t *memVa;
	uint32_t i;
	
	cmdtable = (struct AHCI_COMMAND_TABLE *)CmdTableBaseAddress;
	memVa = (uint8_t *)(&cmdtable->CFIS);
	
	for(i = 0;i< sizeof(struct AHCI_H2D_REGISTER_FIS);i++)
	{
		*memVa++ = 0x00;
	}
	
	cmdtable->CFIS.FisType = 0x27;	/*H2D Register FIS*/
	cmdtable->CFIS.reserved1 = 0;
	cmdtable->CFIS.C = (isReset) ? 0 : 1;	/*clear when reset,set when not reset*/
	
	cmdtable->CFIS.Command = Cmd;/*cmd opcode*/
	cmdtable->CFIS.Dev_Head = 0;
	
	/*command is not queued,not ATAPI*/
	cmdtable->CFIS.Features = 0x00;
	cmdtable->CFIS.Features_Exp = 0x00;
	cmdtable->CFIS.SectorCount = (uint8_t)((Nsect >> 0) & 0x000000ff);
	cmdtable->CFIS.SectorCount_Exp = (uint8_t)((Nsect >> 8) & 0x000000ff);
	
	/*sector address to r/w*/
	cmdtable->CFIS.SectorNumber = (uint8_t)((Lba >> 0) & 0x000000ff);
	cmdtable->CFIS.CylLow = (uint8_t)((Lba >> 8) & 0x000000ff);
	cmdtable->CFIS.CylHigh = (uint8_t)((Lba >> 16) & 0x000000ff);
	
	if(Cmd == 0xC8 || Cmd == 0xCA)/*DMA r/w*/
	{
		cmdtable->CFIS.Dev_Head |= (uint8_t)((Lba >> 24) & 0x0000000f);
	}
	else if(Cmd == 0x25 || Cmd == 0x35)/*DMA EXT r/w*/
	{
		cmdtable->CFIS.SecNum_Exp = (uint8_t)((Lba >> 24) & 0x000000ff);
	}
	
	cmdtable->CFIS.CylLow_Exp = 0x00;
	cmdtable->CFIS.CylHigh_Exp = 0x00;
	
	/*the LBA bit should be set 1 to specify the address is an LBA*/
	cmdtable->CFIS.Dev_Head |= (1 << 6);
	
	cmdtable->CFIS.reserved2 = 0;
	cmdtable->CFIS.reserved3 = 0;
	
	/*0 when not SRST,(1 << 2) when SRST*/
	cmdtable->CFIS.Control = (isSRST) ? (1 << 2) : 0;
	
	if(Cmd == 0xEC)cmdtable->CFIS.Dev_Head = 0;/*for Identify Device command*/
}

static void CreatePrdTable(uint32_t CmdTableBaseAddress,
			   uint32_t DataBaseAddress,	/*DataBaseAddress must be word aligned*/
			   uint32_t DataByteCount,	/*DataByteCount must be even*/
			   uint32_t reqPrdNum)
{
	struct AHCI_COMMAND_TABLE *cmdtable;
	uint8_t *memVa;
	uint32_t i;
	uint16_t prd;
	
	cmdtable = (struct AHCI_COMMAND_TABLE *)CmdTableBaseAddress;
	memVa = (uint8_t *)(&cmdtable->PRDT[0]);
	
	for(i = 0;i < (reqPrdNum * sizeof(struct AHCI_PRDT));i++)
	{
		*memVa++ = 0x00;
	}
	
	for(prd = 0;prd < reqPrdNum;prd++)
	{
		cmdtable->PRDT[prd].DBA.AsUlong = DataBaseAddress;
		cmdtable->PRDT[prd].DBAU = 0;
		cmdtable->PRDT[prd].DI.DBC = DataByteCount - 1;
		cmdtable->PRDT[prd].DI.I = 0;
		
		DataBaseAddress += DataByteCount;
	}

	/*cmdtable->PRDT[reqPrdNum - 1].DI.I = 1;*/
	
}

static void CreateCommandHeader(uint32_t CmdlistBaseAddress,
				uint8_t  CmdHeaderNum,
				uint32_t CmdTableBaseAddress,
				uint16_t Prdtl,
				int isAtapi,
				int isWrite,
				int isBist,
				int isReset)
{
	struct AHCI_COMMAND_HEADER *cmdHeader;
	uint8_t *memVa;
	uint32_t i;
	
	cmdHeader = (struct AHCI_COMMAND_HEADER *)(CmdlistBaseAddress + CmdHeaderNum * (0x20));
	memVa = (uint8_t *)(CmdlistBaseAddress + CmdHeaderNum * (0x20));
	
	for(i = 0;i < sizeof(struct AHCI_COMMAND_HEADER);i++)
	{
		*memVa++ = 0x00;
	}
	
	/*write command table base address to command header*/
	cmdHeader->CTBA.AsUlong = CmdTableBaseAddress;
	cmdHeader->CTBAU = 0;
	
	/*PRDTL determines the number of entries in the PRD Table*/
	cmdHeader->DI.PRDTL = Prdtl;
	
	/*CFL set the DW length of the command CFIS,value between 2 to 16 is valid*/
	if(isBist)
		cmdHeader->DI.CFL = (sizeof(struct AHCI_BIST_ACTIVATE_FIS) / 4)  +  ((sizeof(struct AHCI_BIST_ACTIVATE_FIS) % 4) ? 1 : 0);
	else
		cmdHeader->DI.CFL = (sizeof(struct AHCI_H2D_REGISTER_FIS) / 4)  +  ((sizeof(struct AHCI_H2D_REGISTER_FIS) % 4) ? 1 : 0);
	
	/*A bit set if it is an ATAPI command*/
	cmdHeader->DI.A = (isAtapi) ? 1 : 0;
	
	/*W bit set when it is a write command*/ 
	cmdHeader->DI.W = (isWrite) ? 1 : 0;
	
	/*P bit optionally set,not supported in the current release*/
	cmdHeader->DI.P = 0;
	
	/*PMP field set to the correct Port Multiplier port if a Port Multilier is attached*/
	cmdHeader->DI.PMP = 0;
	
	/*set when BIST FIS*/
	cmdHeader->DI.B = (isBist) ? 1 : 0;
	
	/*set when reset*/
	cmdHeader->DI.R = (isReset) ? 1 : 0;
	cmdHeader->DI.C = (isReset) ? 1 : 0;
	
	/*PRD count byte should be initialized to 0*/
	cmdHeader->PRDBC = 0;
	
	cmdHeader->reserved[0] = 0;
	cmdHeader->reserved[1] = 0;
	cmdHeader->reserved[2] = 0;
	cmdHeader->reserved[3] = 0;
}

static void CreateRecvFIS(uint32_t RecvFISBaseAddress)
{
	struct AHCI_RECEIVED_FIS *recvFIS;
	uint8_t *memVa;
	uint32_t i;
	
	recvFIS = (struct AHCI_RECEIVED_FIS *)RecvFISBaseAddress;
	memVa = (uint8_t *)(&recvFIS->DmaSetupFis);
	for(i = 0;i < sizeof(struct AHCI_RECEIVED_FIS);i++)
	{
		*memVa++ = 0x00;
	}
	
}

static int controlPortIdle(void)
{
	uint32_t cmd;
	int status = 1;

	cmd = readl(P0CMD);

	if(!(cmd & 0x0000C011))
	{
		status = 0;
	}
	else
	{
		cmd = readl(P0CMD);
		cmd &= ~(0x1);
		writel(cmd,P0CMD);/*clear CMD.ST and wait CMD.CR clear*/
		while(1)
		{
			cmd = readl(P0CMD);
			if(!(cmd & 0x00008000))
				break;
		}

		cmd = readl(P0CMD);
		cmd &= ~(1 << 4);
		writel(cmd,P0CMD);/*clear CMD.FRE and wait CMD.FR clear*/
		while(1)
		{
			cmd = readl(P0CMD);
			if(!(cmd & 0x00004000))
				break;
		}

		status = 0;

	}

	return status;
}

#if 0
static int isDevBusy(void)
{
	uint32_t readdata;
	int status = 1;
	int count = 0xffff;
	
	do{
		readdata = readl(P0IS);
		if(readdata & 0x00000001)
		{
			readl(IS);
			status = 0;
			break;
		}
	}while(count--);
	
	return status;
}
#endif

#ifndef SATA_RTL
static int isIdDeviceBusy(void)
{
	uint32_t readdata;
	int status = 1;
	int count = 0xffff;
	
	do{
		readdata = readl(P0IS);
		if(readdata & 0x00000002)
		{
			readl(IS);
			status = 0;
			break;
		}
	}while(count--);
	
	return status;
}
#endif

static int GlobalReset(void)
{
	uint32_t ghc;

	ghc = readl(GHC);
	ghc |= 0x1;
	writel(ghc,GHC);

	while(1)
	{
		ghc = readl(GHC);
		if(!(ghc & 0x00000001))
			break;
	}

	return 0;
}

#if 0
static int find_free_slot(void)
{
	unsigned long tag;
	unsigned long ci;
	unsigned long sact;
	int slotNum;

	slotNum = 0;

	for(tag = 0x1;tag <= 0x80000000;tag = tag << 1)
	{
		ci = readl(P0CI);
		sact = readl(P0SACT);
		if((tag & ci) == 0 && (tag & sact) == 0)
			break;

		slotNum++;
	}

	if(slotNum >= 32)
	{
		return -1;
	}

	return slotNum;
}
#endif

#ifndef SATA_RTL
static void identify_device(uint32_t CmdlistBaseAddress,
			    uint32_t RecvFisBaseAddress,	    
			    uint32_t CmdTableBaseAddress,
			    uint32_t Nsect,
			    uint32_t Lba,
			    uint32_t DataBaseAddress,
			    uint32_t DataByteCount,
			    uint32_t reqPrdNum)
{
	uint32_t cmd;

	CreateAtaCFIS(CmdTableBaseAddress,
		      0xEC,			/*IDENTIFY DEVICE command opcode*/
		      Nsect,
		      Lba,
		      0,			/*not reset*/ 
		      0);			/*not SRST*/
	
	CreatePrdTable(CmdTableBaseAddress,
		       DataBaseAddress,
		       DataByteCount,
		       reqPrdNum);
	
	CreateCommandHeader(CmdlistBaseAddress,
			    0,			/*command header 0*/
			    CmdTableBaseAddress,
			    reqPrdNum,		/*prdtl = reqPrdNum*/
			    0,			/*not ATAPI*/
			    0,			/*read command*/
			    0,			/*not BIST*/
			    0);			/*not reset*/
	
	CreateRecvFIS(RecvFisBaseAddress);
	
	writel(CmdlistBaseAddress,P0CLB);
	writel(RecvFisBaseAddress,P0FB);
	
	writel(0xffffffff,P0IS);
	writel(0xffffffff,P0SERR);
	
	cmd = readl(P0CMD);
	cmd |= (1 << 4);
	writel(cmd,P0CMD);
	
	cmd = readl(P0CMD);
	cmd |= 0x1;
	writel(cmd,P0CMD);
	
	writel(0x1,P0CI);
	
	return;
}
#endif

#ifndef SATA_RTL
static int get_max_lba(uint32_t *buf)
{
	uint32_t tfd;
	uint32_t CmdlistBaseAddress;
	uint32_t RecvFisBaseAddress;	    
	uint32_t CmdTableBaseAddress;
	uint32_t Nsect;
	uint32_t Lba;
	uint32_t DataBaseAddress;
	uint32_t DataByteCount;
	uint32_t reqPrdNum;
	uint32_t *IdInfo;
	uint32_t maxlba_upper,maxlba_lower;
	uint64_t maxlba;

	CmdlistBaseAddress = (uint32_t)buf;
	RecvFisBaseAddress = CmdlistBaseAddress + 1024;
	CmdTableBaseAddress = CmdlistBaseAddress + 1280;;
	DataBaseAddress = CmdlistBaseAddress + 2048;
	DataByteCount = 512;
	Nsect = 1;
	Lba = 0;
	reqPrdNum = 1;

	identify_device(CmdlistBaseAddress,
			RecvFisBaseAddress,	    
			CmdTableBaseAddress,
			Nsect,
			Lba,
			DataBaseAddress,
			DataByteCount,
			reqPrdNum);

	while(isIdDeviceBusy());

	writel(0xffffffff,P0IS);
	writel(0xffffffff,IS);

	IdInfo = (uint32_t *)DataBaseAddress;

	maxlba_upper = *(IdInfo + 51);/*word 102-103*/
	maxlba_lower = *(IdInfo + 50);/*word 100-101*/

	maxlba = maxlba_upper * 4294967296ll + maxlba_lower;

	controlPortIdle();
	
	while(1)
	{
		tfd = readl(P0TFD);
		if(!(tfd & 0x00000088))
			break;
	}

	return maxlba;
}
#endif

static int spin_up_device(uint32_t *buf)
{
	uint32_t CmdlistBaseAddress;
	uint32_t RecvFisBaseAddress;
	uint32_t cap,ssts,tfd,sctl,cmd;
	int status;

	cap = readl(CAP);
	if(cap & 0x08000000)/*ensure support stagged spin-up*/
	{
		controlPortIdle();

		sctl = readl(P0SCTL);
		sctl &= 0xfffffff0;
		writel(sctl,P0SCTL);/*set SCTL.DET = 0*/

		CmdlistBaseAddress = (uint32_t)buf;
		RecvFisBaseAddress = CmdlistBaseAddress + 0x400;
		writel(CmdlistBaseAddress,P0CLB);
		writel(RecvFisBaseAddress,P0FB);

		cmd = readl(P0CMD);
		cmd |= (1 << 4);
		writel(cmd,P0CMD);/*set CMD.FRE = 1*/

		cmd = readl(P0CMD);
		cmd |= (1 << 1);
		writel(cmd,P0CMD);/*set CMD.SUD = 1*/

		while(1)
		{
			ssts = readl(P0SSTS);
			if(((ssts & 0x0000000f) == 0x1) || ((ssts & 0x0000000f) == 0x3))
				break;
		}

		writel(0xffffffff,P0SERR);

		while(1)
		{
			tfd = readl(P0TFD);
			if(!(tfd & 0x00000089))
				break;
		}
		
		status = 0;

	}else{
		printf("Staggered spin-up is not support.\n");
		status = 1;
	}

	return status;
}

int sata_vs_align(void) {
	return 512;
}

void init_sata(void)
{
}

/*
 *Function Name	: sata_vs_reset
 *Parameter	: void
 *Description	: 
 *Return:	: successful:  0
 *		  error	    : -1
 */
 
int sata_vs_reset(void)
{
	uint32_t *new_mem;
	uint32_t *ptr;
	uint32_t CmdlistBaseAddress;
	uint32_t RecvFisBaseAddress;
	uint32_t ssts,tfd,cap,cmd,ghc;

	module_enable(SATA_SYSM_ADDR);
#if 1
	writel(1,MPLL_CK_OFF);
	writel(0x10,ACJT_LVL);
	writel(0,RX_EQ_VAL_0);
	writel(0x6,MPLL_NCY);
	writel(0x2,MPLL_NCY5);
	writel(0x2,MPLL_PRESCALE);
	writel(0,MPLL_SS_SEL);
	writel(0,MPLL_SS_EN);
	writel(0,TX_EDGERATE_0);
	writel(0x12,LOS_LVL);
	writel(0x1,USE_REFCLK_ALT);
	writel(0,REF_CLK_SEL);
	writel(0x9,TX_LVL);
	writel(0,TX_ATTEN_0);
	writel(0xA,TX_BOOST_0);
	writel(0x3,RX_DPLL_MODE_0);
	writel(0x1,RX_ALIGN_EN);
	writel(0,MPLL_PROP_CTL);
	writel(0,MPLL_INT_CTL);
	writel(0x2,LOS_CTL_0);
	writel(0x1,RX_TERM_EN);
	writel(0x3,SATA_SPDMODE_CTL);
	writel(0x10,BS_CTL_CFG);
	writel(0,MPLL_CK_OFF);

#ifdef SATA_RTL
	udelay(1000);
#else
	udelay(20000);
#endif
#endif
	GlobalReset();

	cap = readl(CAP);
	cap |= (0x3 << 27);
	writel(cap,CAP);

	writel(0x1,PI);

	cmd = readl(P0CMD);
	cmd |= (0x7 << 18);
	writel(cmd,P0CMD);

	new_mem = (uint32_t *)malloc(3072);/*2048+1024*/
	if(new_mem == NULL)
	{
		printf("Alloc memory fail in sata init.\n");
		return -1;
	}

	ptr = (uint32_t *)(((uint32_t)new_mem + 0x3ff)&(~(0x3ff)));

	if(ptr == new_mem)ptr += 1024;

	if(spin_up_device(ptr))
	{
		printf("spin-up device fail.\n");
		free(new_mem);
		return -1;
	}

	controlPortIdle();

	CmdlistBaseAddress = (uint32_t)ptr;
	RecvFisBaseAddress = CmdlistBaseAddress + 0x400;

	writel(CmdlistBaseAddress,P0CLB);

	writel(RecvFisBaseAddress,P0FB);

	cmd = readl(P0CMD);
	cmd |= (1 << 4);
	writel(cmd,P0CMD);

	writel(0xffffffff,P0SERR);
	writel(0xffffffff,P0IS);
	writel(0xffffffff,IS);
	writel(0xff7fffff,P0IE);

	ghc = readl(GHC);
	ghc |= (1 << 1);
	writel(ghc,GHC);

	while(1)
	{
		ssts = readl(P0SSTS);
		tfd = readl(P0TFD);
		if(((ssts & 0x0000000f) == 0x3) && (!(tfd & 0x00000089)))
			break;
	}

	free(new_mem);

	printf("SATA is ready.\n");

	return 0;
}

/*
 *Function Name	: sata_vs_read
 *Parameter	:
 *Description	: 
 *Return:	: successful: actual length
 *		  error	    : -1
 */

int sata_vs_read(uint8_t *buf, loff_t offs, int len, uint32_t extra)
{
	uint32_t *new_mem;
	uint32_t *ptr;
	uint32_t CmdlistBaseAddress;
	uint32_t RecvFisBaseAddress;
	uint32_t CmdTableBaseAddress;
	uint32_t DataBaseAddress;
	uint32_t DataByteCount;
	uint32_t Lba;
	uint32_t tfd;
	uint32_t cmd;
	uint32_t is;
	int reqPrdNum;
	int Nsect;
	int rest_len;
	int actual_len;
#ifndef SATA_RTL
	uint64_t maxlba;
#endif

	new_mem = malloc(3584);/*2560+1024*/
	if(new_mem == NULL)
	{
		printf("Alloc memory fail for sata read.\n");
		return -1;
	}

	ptr = (uint32_t *)(((uint32_t)new_mem + 0x3ff)&(~(0x3ff)));

	if(ptr == new_mem)ptr += 1024;

#ifdef SATA_RTL

	Lba = (offs / 512);

	if((len % 512)||(offs % 512)||((uint32_t)buf % 2))
#else
	maxlba = get_max_lba(ptr);

	Lba = (offs / 512);

	if((len % 512)||(offs % 512)||((uint32_t)buf % 2)||(Lba >= maxlba)||
	   (len >= 0x2000000 && (maxlba - Lba) < 65536)||(len >= 0x1000000 && (maxlba - Lba) < 32768)||(len >= 0x800000 && (maxlba - Lba) < 16384)||
	   (len >= 0x400000 && (maxlba - Lba) < 8192)||(len >= 0x200000 && (maxlba - Lba) < 4096)||(len >= 0x100000 && (maxlba - Lba) < 2048)||
	   (len >= 0x80000 && (maxlba - Lba) < 1024)||(len >= 0x40000 && (maxlba - Lba) < 512)||(len >= 0x20000 && Lba > (maxlba - 256))||
	   (len >= 0x10000 && (maxlba - Lba) < 128)||(len >= 0x8000 && (maxlba - Lba) < 64)||(len >= 0x4000 && (maxlba - Lba) < 32)||
	   (len >= 0x2000 && (maxlba - Lba) < 16)||(len >= 0x1000 && (maxlba- Lba) < 8)||(len >= 0x800 && (maxlba - Lba) < 4)||
	   (len >= 0x400 && (maxlba - Lba) < 2)||(len >= 0x200 && (maxlba - Lba) < 1))
#endif
	{
		printf("Can't Write because of violation.\n");
		free(new_mem);
		return -1;
	}

	/*if((cmdslotNum = find_free_slot()) == -1)
	 {
	  	printf("Can't find free command slot to set command.\n");
	  	return -1;
	 }*/


	rest_len = len;

	CmdlistBaseAddress = (uint32_t)ptr;
	RecvFisBaseAddress = CmdlistBaseAddress + 1024;
	CmdTableBaseAddress = CmdlistBaseAddress + 1280;
	DataBaseAddress = (uint32_t)buf;

	while(1)
	{
		if(rest_len >= 0x2000000)
		{
			Nsect = 65536;
			DataByteCount = 0x400000;
			reqPrdNum = 8;
		}
		else if(rest_len >= 0x1000000)
		{
			Nsect = 32768;
			DataByteCount = 0x400000;
			reqPrdNum = 4;
		}
		else if(rest_len >= 0x800000)
		{
			Nsect = 16384;
			DataByteCount = 0x400000;
			reqPrdNum = 2;
		}
		else if(rest_len >= 0x400000)
		{
			Nsect = 8192;
			DataByteCount = 0x400000;
			reqPrdNum = 1;
		}
		else if(rest_len < 0x400000 && rest_len >= 0x200)
		{
			Nsect = rest_len/512;
			DataByteCount = Nsect * 512;
			reqPrdNum = 1;
		}
		else
		{
			goto END;
		}

		CreateAtaCFIS(CmdTableBaseAddress,
			      0x25,
			      Nsect,
			      Lba,
			      0,
			      0);

		CreatePrdTable(CmdTableBaseAddress,
			       DataBaseAddress,
			       DataByteCount,
			       reqPrdNum);

		CreateCommandHeader(CmdlistBaseAddress,
		    		    0,			/*find command slot*/
		    		    CmdTableBaseAddress,
		    		    reqPrdNum,		
		    		    0,			
		    		    0,			/*read*/
		    		    0,			
		    		    0);			

		CreateRecvFIS(RecvFisBaseAddress);

		writel(CmdlistBaseAddress,P0CLB);
		writel(RecvFisBaseAddress,P0FB);

		writel(0xffffffff,P0IS);
		writel(0xffffffff,P0SERR);

		cmd = readl(P0CMD);
		cmd |= (1 << 4);
		writel(cmd,P0CMD);

		cmd = readl(P0CMD);
		cmd |= 0x1;
		writel(cmd,P0CMD);

		writel(0x1,P0CI);

		while(1)
		{
			is = readl(P0IS);
			if(is & 0x1)
				break;
		}

		writel(0xffffffff,P0IS);
		writel(0xffffffff,IS);

		controlPortIdle();

		while(1)
		{
			tfd = readl(P0TFD);
			if(!(tfd & 0x00000088))
				break;
		}

		Lba += Nsect;
		DataBaseAddress += DataByteCount * reqPrdNum;
		rest_len -= DataByteCount * reqPrdNum;
#ifdef SATA_RTL
		if(rest_len == 0)
			break;
#else
		if((rest_len == 0) || ((Lba + (rest_len / 512) + ((rest_len % 512)?1 : 0)) >= maxlba))
			break;
#endif
	
	}

END:	actual_len = len - rest_len;

	free(new_mem);	

	return actual_len;

}

/*
 *Function Name	: sata_vs_write
 *Parameter	:
 *Description	: 
 *Return:	: successful: actual length
 *		  error	    : -1
 */

int sata_vs_write(uint8_t *buf, loff_t offs, int len, uint32_t extra)
{
	uint32_t *new_mem;
	uint32_t *ptr;
	uint32_t CmdlistBaseAddress;
	uint32_t RecvFisBaseAddress;
	uint32_t CmdTableBaseAddress;
	uint32_t DataBaseAddress;
	uint32_t DataByteCount;
	uint32_t Lba;
	uint32_t tfd;
	uint32_t cmd;
	uint32_t is;
	int reqPrdNum;
	int Nsect;
	int rest_len;
	int actual_len;
#ifndef SATA_RTL
	uint64_t maxlba;
#endif

	new_mem = malloc(3584);/*2560+1024*/
	if(new_mem == NULL)
	{
		printf("Alloc memory fail for sata write.\n");
		return -1;
	}

	ptr = (uint32_t *)(((uint32_t)new_mem + 0x3ff)&(~(0x3ff)));

	if(ptr == new_mem)ptr += 1024;

#ifdef SATA_RTL

	Lba = (offs / 512);

	if((len % 512)||(offs % 512)||((uint32_t)buf % 2))

#else
	maxlba = get_max_lba(ptr);

	Lba = (offs / 512);

	if((len % 512)||(offs % 512)||((uint32_t)buf % 2)||(Lba >= maxlba)||
	   (len >= 0x2000000 && (maxlba - Lba) < 65536)||(len >= 0x1000000 && (maxlba - Lba) < 32768)||(len >= 0x800000 && (maxlba - Lba) < 16384)||
	   (len >= 0x400000 && (maxlba - Lba) < 8192)||(len >= 0x200000 && (maxlba - Lba) < 4096)||(len >= 0x100000 && (maxlba - Lba) < 2048)||
	   (len >= 0x80000 && (maxlba - Lba) < 1024)||(len >= 0x40000 && (maxlba - Lba) < 512)||(len >= 0x20000 && Lba > (maxlba - 256))||
	   (len >= 0x10000 && (maxlba - Lba) < 128)||(len >= 0x8000 && (maxlba - Lba) < 64)||(len >= 0x4000 && (maxlba - Lba) < 32)||
	   (len >= 0x2000 && (maxlba - Lba) < 16)||(len >= 0x1000 && (maxlba- Lba) < 8)||(len >= 0x800 && (maxlba - Lba) < 4)||
	   (len >= 0x400 && (maxlba - Lba) < 2)||(len >= 0x200 && (maxlba - Lba) < 1))
#endif
	{
		printf("Can't Write because of violation.\n");
		free(new_mem);
		return -1;
	}

	/*if((cmdslotNum = find_free_slot()) == -1)
	 {
	  	printf("Can't find free command slot to set command.\n");
	  	return -1;
	 }*/

	rest_len = len;

	CmdlistBaseAddress = (uint32_t)ptr;
	RecvFisBaseAddress = CmdlistBaseAddress + 1024;
	CmdTableBaseAddress = CmdlistBaseAddress + 1280;
	DataBaseAddress = (uint32_t)buf;

	while(1)
	{
		if(rest_len >= 0x2000000)
		{
			Nsect = 65536;
			DataByteCount = 0x400000;
			reqPrdNum = 8;
		}
		else if(rest_len >= 0x1000000)
		{
			Nsect = 32768;
			DataByteCount = 0x400000;
			reqPrdNum = 4;
		}
		else if(rest_len >= 0x800000)
		{
			Nsect = 16384;
			DataByteCount = 0x400000;
			reqPrdNum = 2;
		}
		else if(rest_len >= 0x400000)
		{
			Nsect = 8192;
			DataByteCount = 0x400000;
			reqPrdNum = 1;
		}
		else if(rest_len < 0x400000 && rest_len >= 0x200)
		{
			Nsect = rest_len/512;
			DataByteCount = Nsect * 512;
			reqPrdNum = 1;
		}
		else
		{
			goto END;
		}


		CreateAtaCFIS(CmdTableBaseAddress,
			      0x35,
			      Nsect,
			      Lba,
			      0,
			      0);

		CreatePrdTable(CmdTableBaseAddress,
			       DataBaseAddress,
			       DataByteCount,
			       reqPrdNum);

		CreateCommandHeader(CmdlistBaseAddress,
		    		    0,			/*find command slot*/
		    		    CmdTableBaseAddress,
		    		    reqPrdNum,		
		    		    0,			
		    		    1,			/*write*/
		    		    0,			
		    		    0);			

		CreateRecvFIS(RecvFisBaseAddress);

		writel(CmdlistBaseAddress,P0CLB);
		writel(RecvFisBaseAddress,P0FB);

		writel(0xffffffff,P0IS);
		writel(0xffffffff,P0SERR);

		cmd = readl(P0CMD);
		cmd |= (1 << 4);
		writel(cmd,P0CMD);

		cmd = readl(P0CMD);
		cmd |= 0x1;
		writel(cmd,P0CMD);

		writel(0x1,P0CI);

		while(1)
		{
			is = readl(P0IS);
			if(is & 0x1)
				break;
		}

		writel(0xffffffff,P0IS);
		writel(0xffffffff,IS);

		controlPortIdle();

		while(1)
		{
			tfd = readl(P0TFD);
			if(!(tfd & 0x00000088))
				break;
		}

		Lba += Nsect;
		DataBaseAddress += DataByteCount * reqPrdNum;
		rest_len -= DataByteCount * reqPrdNum;
#ifdef SATA_RTL
		if(rest_len == 0)
			break;
#else
		if((rest_len == 0) || ((Lba + (rest_len / 512) + ((rest_len % 512)?1 : 0)) >= maxlba))
			break;
#endif
	}

END:	actual_len = len - rest_len;

	free(new_mem);

	return actual_len;
}




