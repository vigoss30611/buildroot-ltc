/* **************************************************************************** 
 * * ** gmac-univ_ethernet.c
 * * ** 
 * * ** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
 * * ** 
 * * ** This program is free software; you can redistribute it and/or modify
 * * ** it under the terms of the GNU General Public License as published by
 * * ** the Free Software Foundation; either version 2 of the License, or
 * * ** (at your option) any later version.
 * * ** 
 * * ** Description:This c file is the driver for DesignWare Cores Ethernet 
 * * ** MAC Universal(GMAC-UNIV for short). 
 * * **
 * * ** Author:
 * * **     Bob.yang<bob.yang@mail.infotmic.com.cn>
 * * **      
 * * ** Revision History: 
 * * ** ----------------- 
 * * ** 0.9  12/17/2009 Bob.yang
 * * *****************************************************************************/

#include <common.h>
#include <command.h>
#include <net.h>
#include <asm/io.h>

#include "imapx200_gmac.h"

#define ETH_BASE_TXDES_PHY (CONFIG_RESV_GMAC_BASE)
#define ETH_BASE_TXBUF_PHY (ETH_BASE_TXDES_PHY + SIZE_OF_TXDES)
#define ETH_BASE_RXDES_PHY (ETH_BASE_TXBUF_PHY + SIZE_OF_TXBUF)
#define ETH_BASE_RXBUF_PHY (ETH_BASE_RXDES_PHY + SIZE_OF_RXDES)

/* define gmac_debug */
#if 0
#define GMAC_DBG(fmt,args...) printf(fmt, ##args)
#define GMAC_DMP_PACKET(packet,length)\
do\
{\
	int i;\
	printf("%s : length: %d\n",__func__, length);\
	for (i=0; i<length; i++)\
	{\
		if (i % 8 == 0)\
		{\
			printf("\n%s: %02x: ", __func__, i);\
		}\
		printf("%02x ", ((unsigned char *) packet)[i]);\
	}\
	printf("\n");\
} while(0)
#else
#define GMAC_DBG(fmt,args...)
#define GMAC_DMP_PACKET(packet,length)
#endif

static u32 *pt_to_last_edgerxdesc;

/* gmac board infomation structure declaration */
typedef struct board_info
{
	struct eth_device netdev;
} board_info_t;
static board_info_t gmac_info;

/* function declaration ------------------------------------- */
void gmac_cfg_gpio(void);
void gmac_cre_def_tx_des(void);
void gmac_cre_def_rx_des(void);
void gmac_cre_cur_tx_des(volatile void *pdata_phyaddr, u32 len);
static int gmac_probe(void);
static u16 gmac_phy_read(int);
static void gmac_phy_write(int, u16);

/* open all GPIO control registers for ethernet */
void gmac_cfg_gpio(void)
{           
	u32 tmp;

#ifdef CONFIG_ETH_RESET_GPIO
	tmp = readl(CONFIG_ETH_RESET_GPIO + 4);
	tmp &= ~(0x3 << (CONFIG_ETH_RESET_GPIO_NUM * 2));
	tmp |= (1 << (CONFIG_ETH_RESET_GPIO_NUM * 2));
	writel(tmp, CONFIG_ETH_RESET_GPIO + 4);

	tmp = readl(CONFIG_ETH_RESET_GPIO);
	tmp |= (1 << CONFIG_ETH_RESET_GPIO_NUM);
	writel(tmp, CONFIG_ETH_RESET_GPIO);
#else
	tmp = readl(GPOCON);
	tmp &= ~(0x3 << 8);
	tmp |= (1 << 8);
	writel(tmp, GPOCON);

	tmp = readl(GPODAT);
	tmp |= (1 << 4);
	writel(tmp, GPODAT);
#endif

	writel(0xAAAAAAAA, GPKCON);

	tmp = readl(GPJCON);
	tmp &= ~(0x3<<16);
	tmp |= 0x2<<16;
	writel(tmp, GPJCON);
}

void gmac_cre_def_tx_des(void)
{
	int i = 0;
	u32 *pointto_cur_des;
	pointto_cur_des = (u32 *)ETH_BASE_TXDES_PHY;

	GMAC_DBG("enter %s\n", __func__);
	for (i=0; i<NO_OF_TXDES; i++)
	{
		writel(0, pointto_cur_des); /* at first the Tx descriptor is not owned by DMA control */
		writel(IC | CIC | DC_T | TCH | DP | TTSE | TBS2, pointto_cur_des + 1);      /* Config the TxDES1 */
		writel(ETH_BASE_TXDES_PHY+(i+1)*SINGLE_TXDES_SIZE, pointto_cur_des + 3);        /* Next Tx descriptor address */
		pointto_cur_des += 4;
	}
	pointto_cur_des -= 4;
	writel(ETH_BASE_TXDES_PHY, pointto_cur_des + 3);    /* Next Tx descriptor address of last descriptor is the first one */
	clean_cache(ETH_BASE_TXDES_PHY, ETH_BASE_TXDES_PHY + SIZE_OF_TXDES);
}

void gmac_cre_def_rx_des(void)
{
	int i = 0;
	u32 *pointto_cur_des;
	pointto_cur_des = (u32 *)ETH_BASE_RXDES_PHY;

	GMAC_DBG("enter %s\n", __func__);
	for (i=0; i<NO_OF_RXDES; i++)
	{
		writel(RXOWN, pointto_cur_des); /* at first the Rx descriptor is owned by DMA control */
		writel(DINT | RCH | RBS2 | RBS1, pointto_cur_des + 1);      /* Config the RxDES1 */
		writel(ETH_BASE_RXBUF_PHY+i*RBS1, pointto_cur_des + 2);     /* Physical address for Rx buffer */
		writel(ETH_BASE_RXDES_PHY+(i+1)*SINGLE_RXDES_SIZE, pointto_cur_des + 3);    /* Next Rx descriptor address */
		pointto_cur_des += 4;
	}
	pointto_cur_des -= 4;
	writel(ETH_BASE_RXDES_PHY, pointto_cur_des + 3);    /* Next Rx descriptor address of last descriptor is the first one */
	clean_cache(ETH_BASE_RXDES_PHY, ETH_BASE_RXDES_PHY + SIZE_OF_RXDES);
}

void gmac_cre_cur_tx_des(volatile void *pdata_phyaddr, u32 len)
{
	u32 *pointto_cur_des;
	pointto_cur_des = (u32 *)readl(CurrHostTxDescriptor) ;

	GMAC_DBG("enter %s\n", __func__);
	writel(TXOWN, pointto_cur_des);     /* this Tx descriptor is owned by DMA control now */
	writel(IC | CIC | DC_T | TCH | DP | TTSE | TBS2, pointto_cur_des + 1);      /* Config the TxDES1 */
	writel((u32)pdata_phyaddr, pointto_cur_des + 2); /* set physical address for Tx buffer into TxDES2 */

	*(pointto_cur_des + 1) |= FS | TXLS | len;     /* the first and last segment of a frame is in one descriptor */
	writel(~TXOWN, pointto_cur_des + 4);     /* Next Tx descriptor is not owned by DMA control now */
	clean_cache((uint32_t)pointto_cur_des, (uint32_t)(pointto_cur_des) + SINGLE_TXDES_SIZE);
//	clean_cache(ETH_BASE_TXDES_PHY, ETH_BASE_TXDES_PHY + SIZE_OF_TXDES);
}

/* Search Version of gmac board and open the GPIO control for gmac */
int gmac_probe()
{
	gmac_cfg_gpio();
	GMAC_DBG("The Version of Gmac board is %x\n", readl(IdeftifiesVersion));
	return 0;
}

/* Gmac reset routine */
static void gmac_reset(void)
{
	u32 tmp;
	GMAC_DBG("Resetting GMAC\n");

	/* Software reset */
	tmp = readl(BUSMODE);
	tmp |= SWR;
	writel(tmp, BUSMODE);
	while (readl(BUSMODE) & SWR)
	{
		;/* wait until resetting completion */
	}
}

/* Initialize gmac board */
static int gmac_init(struct eth_device *dev, bd_t *bd)
{
	u32 i = 0, tmp, eth_speed;
	uchar enetaddr[6] = {0x00, 0x1B, 0xFC, 0xEB, 0x92, 0xFA};

	GMAC_DBG("%s\n", __func__);

	gmac_probe();
	gmac_reset();

	/* Set Node address */
	eth_setenv_enetaddr("ethaddr", enetaddr);
	for (i=0; i<6; i++)
	{
		dev->enetaddr[i] = enetaddr[i];
		GMAC_DBG("%x ", enetaddr[i]);
	}
	GMAC_DBG("\n");

	/* fill device MAC address registers */
	writel(enetaddr[5]<<8 | enetaddr[4], MACAddr0H);		/* mac high addr */
	writel(enetaddr[3]<<24 | enetaddr[2]<<16 | enetaddr[1]<<8 | enetaddr[0], MACAddr0L );	/* mac low addr */
	writel(0xffffffff, HashTableH);
	writel(0xffffffff, HashTableL);

	eth_speed = NORMAL_SPEED;

	if (eth_speed == NORMAL_SPEED)
	{
		/* only test in fpga */
		gmac_phy_write(4, 0x1e1);			/* support 10M,100M F/HD auto*/
//		gmac_phy_write(0, 0x1100);            /* Full-duplex and autoneg enabled */
//		gmac_phy_write(0, 0x8000);            /* Full-duplex and autoneg enabled */
		i = 0;
		while (!(gmac_phy_read(1) & 0x20)) { /* autonegation complete bit */
			udelay(1000);
			i++;
			if (i == 2000) {
				printf("could not establish link\n");
				return 0;
			}
		}

//		printf("phyreg0 is %x\n", gmac_phy_read(0));
//		printf("phyreg4 is %x\n", gmac_phy_read(4));
//		printf("phyreg25 is %x\n", gmac_phy_read(25));
	}
	else
	{
		//gmac_phy_write(0, 0x1 << 15);			/* autoneg enabled */
	}

	/* config the DMA BUSMODE register */
	writel(AAL|FBPL|USP|RXPBL|FB|PBL|DSL|DA, BUSMODE);

	/* config the DMA TxDescriptorListAddr and RxDescriptorListAddr register */
	writel(ETH_BASE_TXDES_PHY, TxDescriptorListAddr);
	writel(ETH_BASE_RXDES_PHY, RxDescriptorListAddr);

	/* config the DMA OperationMode register */
	writel(FEF | RFA | RSF, OperationMode);

	/* config the GMAC MACConfig register */
	writel(TC|WD|JD|BE|JE|IFG|DCRS|PS|FES|DO|LM|DM|IPC|DR|LUD|ACS|BL|DC|TE|RE, MACConfig);

	writel(0xff, Status);    /* clear all interrupt bits */

	/* enable TI and RI interrupt mask */
//	writel(NIE|RIE, InterruptEnable);

	gmac_cre_def_tx_des();
	gmac_cre_def_rx_des();

	pt_to_last_edgerxdesc = (u32 *)ETH_BASE_RXDES_PHY;

	/* start Tx and Rx */
	tmp= readl(OperationMode);
	tmp |= (ST | SR);
	writel(tmp, OperationMode);

	return 0;
}

/* send a frame to media from the upper layer */
static int gmac_send(struct eth_device *netdev, volatile void *packet, int length)
{
	int tmo;

	GMAC_DMP_PACKET(packet, length);

	gmac_cre_cur_tx_des(packet, length);
	/* Everytime after last Tx, TU will be set automatically. So we should clear it */
	if (readl(Status) & TU)
	{
		writel(TU, Status);
	}

	clean_cache((uint32_t)packet, (uint32_t)(packet) + length);
	/* Issue Tx polling command */
	writel(0x1, TxPollDemand);
	
	/* wait for end of transmission */
	tmo = get_timer(0) + 500 * CONFIG_SYS_HZ;
	while(!(readl(Status) & TI))
	{
		if (get_timer(0) >= tmo) 
		{
			printf("transmission timeout\n");
		}
		;		/* wait until TI set by DMA */
	}
	writel(TI, Status);

	GMAC_DBG("transmit done\n\n");
	return 0;
}
	
/* Stop the interface */
static void gmac_halt(struct eth_device *netdev)
{
	u32 tmp;
    	GMAC_DBG("%s\n", __func__);
	/* reset device */
//	gmac_phy_write(0, (0x1 << 15)); //PHY RESET
//	gmac_phy_write(MII_BMCR, BMCR_PDOWN); //Power-Down PHY

	/* disable all interrupt mask */
	writel(0, InterruptEnable);

	/* stop Tx and Rx */
	tmp= readl(OperationMode);
	tmp &= ~ST;
	tmp &= ~SR;
	writel(tmp, OperationMode);
}

/* Received a packet and pass to upper layer */
static int gmac_rx(struct eth_device *netdev)
{
	u8 *rdptr = (u8 *) NetRxPackets[0], *pointto_cur_buf;
	u32 RxStatus, RxLen = 0;
	u32 *pointto_cur_des;

	GMAC_DBG("%s\n", __func__);

	/* Check packet ready or not, we must check RI bit in Status register */
	if (!(readl(Status) & RI))
	{
		return 0;
	}

	writel(RI, Status);		/* clear RI bit */
	GMAC_DBG("RI is ok!\n");
	/* There is _at least_ 1 package in the fifo, read them all */
	pointto_cur_des = pt_to_last_edgerxdesc;
	while(1)
//	if (readl(pointto_cur_des) & RXLS)
	{
		inv_cache((uint32_t)pointto_cur_des, (uint32_t)pointto_cur_des + SINGLE_RXDES_SIZE);
		if(!(readl(pointto_cur_des) & RXLS))
		  break;
		//rdptr = (u8 *) (ETH_BASE_RXBUF_PHY + (pointto_cur_des - (u32 *)ETH_BASE_RXDES_PHY) / 4 *RBS1);
		pointto_cur_buf	= (u8 *)readl(pointto_cur_des + 2);
		RxLen = (readl(pointto_cur_des) >> 16) & 0x3fff;

		inv_cache((uint32_t)pointto_cur_buf, (uint32_t)(pointto_cur_buf + RxLen));
		memcpy(rdptr, pointto_cur_buf, RxLen);

		GMAC_DBG("RXLS is ok!\n");
		RxStatus = readl(Status);
		if ((RxStatus & FBI) || (RxLen < MIN_SIZE) || (RxLen > MAX_MTU))
		{
			if (RxStatus & FBI)
			{
				printf("fatal bus error!\n");
			}
			
			if (RxLen < MIN_SIZE)
			{
				printf("size too small!\n");
			}

			if (RxLen > MAX_MTU)
			{
				printf("size too big!\n");
			}
		}
		else
		{
			GMAC_DMP_PACKET(rdptr, RxLen);

		 	GMAC_DBG("passing packet to upper layer\n");
			NetReceive(NetRxPackets[0], RxLen);
		}
		writel(RXOWN, pointto_cur_des);
		clean_cache((uint32_t)pointto_cur_des, (uint32_t)pointto_cur_des + SINGLE_RXDES_SIZE);

		pointto_cur_des += 4;       //goto next RxDes
		if (pointto_cur_des > ((u32 *)ETH_BASE_RXDES_PHY + (NO_OF_RXDES - 1) * 4))
		{
			pointto_cur_des = (u32 *)ETH_BASE_RXDES_PHY;
		}
	}

	pt_to_last_edgerxdesc = pointto_cur_des;
 	GMAC_DBG("gmac_rx has been done\n");

	return RxLen;
}

/* Read a word from phy */
static u16 gmac_phy_read(int reg)
{
	u16 ret;
	writel((0x1<<11) | (reg<<6) | (0x1<<2) | (0x0<<1) | (0x1<<0), GMIIAddr);
	while(readl(GMIIAddr) & PHYBUSY)
	{
		;	/* wait until reading completion */
	}
	ret = readl(GMIIData);

	return ret;
}

/* Write a word to phy */
void gmac_phy_write(int reg, u16 value)
{
	writel(value, GMIIData);
	writel((0x1<<11) | (reg<<6) | (0x1<<2) | (0x1<<1) | (0x1<<0), GMIIAddr);
	while(readl(GMIIAddr) & PHYBUSY)
	{
		;	/* wait until writing completion */
	}
}

int gmac_initialize(bd_t *bis)
{
	struct eth_device *dev = &(gmac_info.netdev);

	dev->init = gmac_init;
	dev->halt = gmac_halt;
	dev->send = gmac_send;
	dev->recv = gmac_rx;
	sprintf(dev->name, "GMAC");

	eth_register(dev);
//	printf("after register\n");

	return 0;
}

