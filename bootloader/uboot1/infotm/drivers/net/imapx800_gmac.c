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
 * * ** 1.0  08/15/2016 jicky.chen 
 * * **      1:fix bug about bus6 clock to 46.5M must be the same to PHY out clock to mac
 * * **      2:fix bug about module_enable function
 * * **      3:fix bug about gpio setting of mac
 * * **      4:disable cache in uboot0 start.s
 * * *****************************************************************************/

#include <common.h>
#include <command.h>
#include <net.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <gdma.h>
#include <cpuid.h>

#include "imapx800_gmac.h"

static uint8_t *_ethresv;
static uint32_t txdes_base = (u32)0;
static uint32_t rxdes_base = (u32)0;
static uint32_t rxbuf_base = (u32)0;

static u32 *pt_to_last_edgerxdesc;

/* gmac board infomation structure declaration */
static board_info_t gmac_info;

void gmac_meminit(void)
{
    _ethresv = (uint8_t *)malloc(SIZE_OF_TXDES + SIZE_OF_TXBUF + SIZE_OF_RXDES + SIZE_OF_RXBUF);
	if (!_ethresv) {
		return;
	}

    txdes_base = (u32)_ethresv;
    rxdes_base = (u32)(_ethresv + SIZE_OF_TXDES + SIZE_OF_TXBUF);
    rxbuf_base = (u32)(_ethresv + SIZE_OF_TXDES + SIZE_OF_TXBUF + SIZE_OF_RXDES);
}

void gmac_memexit(void)
{
	if (_ethresv) {
		free(_ethresv);
	}
}

/* open all GPIO control registers for ethernet */
void gmac_cfg_gpio(void)
{           
#if 0
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
#endif
}

void gmac_cre_def_tx_des(void)
{
	int i = 0;
	u32 *pointto_cur_des;
	pointto_cur_des = (u32 *)txdes_base;

	GMAC_DBG("enter %s\n", __func__);
	for (i=0; i<NO_OF_TXDES; i++) {
		writel(0, pointto_cur_des); /* at first the Tx descriptor is not owned by DMA control */
		writel(IC | CIC | DC_T | TCH | DP | TTSE | TBS2, pointto_cur_des + 1);      /* Config the TxDES1 */
		writel(txdes_base+(i+1)*SINGLE_TXDES_SIZE, pointto_cur_des + 3);        /* Next Tx descriptor address */
		pointto_cur_des += 4;
	}
	pointto_cur_des -= 4;
	writel(txdes_base, pointto_cur_des + 3);    /* Next Tx descriptor address of last descriptor is the first one */
	//clean_cache(txdes_base, txdes_base + SIZE_OF_TXDES);
}

void gmac_cre_def_rx_des(void)
{
	int i = 0;
	u32 *pointto_cur_des;
	pointto_cur_des = (u32 *)rxdes_base;

	GMAC_DBG("enter %s\n", __func__);
	for (i=0; i<NO_OF_RXDES; i++) {
		writel(RXOWN, pointto_cur_des); /* at first the Rx descriptor is owned by DMA control */
		writel(DINT | RCH | RBS2 | RBS1, pointto_cur_des + 1);      /* Config the RxDES1 */
		writel(rxbuf_base+i*RBS1, pointto_cur_des + 2);     /* Physical address for Rx buffer */
		writel(rxdes_base+(i+1)*SINGLE_RXDES_SIZE, pointto_cur_des + 3);    /* Next Rx descriptor address */
		pointto_cur_des += 4;
	}
	pointto_cur_des -= 4;
	writel(rxdes_base, pointto_cur_des + 3);    /* Next Rx descriptor address of last descriptor is the first one */
	//clean_cache(rxdes_base, rxdes_base + SIZE_OF_RXDES);
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
	//clean_cache((uint32_t)pointto_cur_des, (uint32_t)(pointto_cur_des) + SINGLE_TXDES_SIZE);
//	clean_cache(txdes_base, txdes_base + SIZE_OF_TXDES);
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
	while (readl(BUSMODE) & SWR);/* wait until resetting completion */
}

/* Initialize gmac board */
static int gmac_init(struct eth_device *dev)
{
	u32 i = 0, tmp, eth_speed;
	uchar enetaddr[6] ;

	GMAC_DBG("%s\n", __func__);
	
	enetaddr[0] = 0x00;
	enetaddr[1] = 0x1b;
	enetaddr[2] = 0xfc;
	enetaddr[3] = 0xeb;
	enetaddr[4] = 0x92;
	enetaddr[5] = 0xfa;

	gmac_probe();
	gmac_reset();

	/* Set Node address */
	for (i = 0; i < 6; i++) {
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
	if (eth_speed == NORMAL_SPEED) {
#if 0
		/* only test in fpga */
		gmac_phy_write(4, 0x1e1);			  /* support 10M,100M F/HD auto*/
		
		i = 0;
		while (!(gmac_phy_read(1) & 0x20)) { /* autonegation complete bit */
			udelay(1000);
			i++;
			if (i == 2000) {
				printf("could not establish link\n");
				return 0;
			}
		}

#endif
	} else {
		//gmac_phy_write(0, 0x1 << 15);			/* autoneg enabled */
	}

	/* config the DMA BUSMODE register */
	writel(AAL|FBPL|USP|RXPBL|FB|PBL|DSL|DA, BUSMODE);

	/* config the DMA TxDescriptorListAddr and RxDescriptorListAddr register */
	writel(txdes_base, TxDescriptorListAddr);
	writel(rxdes_base, RxDescriptorListAddr);

	/* config the DMA OperationMode register */
	writel(FEF | RFA | RSF, OperationMode);

	/* config the GMAC MACConfig register */
	writel(TC|WD|JD|BE|JE|IFG|DCRS|PS|FES|DO|LM|DM|IPC|DR|LUD|ACS|BL|DC|TE|RE, MACConfig);

	writel(0xff, Status);    /* clear all interrupt bits */

	/* enable TI and RI interrupt mask */
//	writel(NIE|RIE, InterruptEnable);

	gmac_cre_def_tx_des();
	gmac_cre_def_rx_des();

	pt_to_last_edgerxdesc = (u32 *)rxdes_base;

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
	
	int count;

	GMAC_DMP_PACKET(packet, length);

	gmac_cre_cur_tx_des(packet, length);
	/* Everytime after last Tx, TU will be set automatically. So we should clear it */
	if (readl(Status) & TU) {
		writel(TU, Status);
	}

	//clean_cache((uint32_t)packet, (uint32_t)(packet) + length);
	/* Issue Tx polling command */
	writel(0x1, TxPollDemand);
	
	/* wait 1000ms for end of transmission */
	tmo = get_timer(0) + 1000;
	while(!(readl(Status) & TI)) { /* wait until TI set by DMA */
		if (get_timer(0) >= tmo) {
			printf("transmission timeout\n");
			count++;
		}	
		if(count >= 100) {
			break;
		}
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
	u8 *rdptr = (u8 *)gmac_info.netdev.recv_packets[0], 
	   *pointto_cur_buf = NULL;
	u32 rx_status = 0, 
	    rx_len = 0;
	u32 *pointto_cur_des = 0;

	GMAC_DBG("%s\n", __func__);

	/* Check packet ready or not, we must check RI bit in Status register */
	if (!(readl(Status) & RI)) {
		return 0;
	}

	writel(RI, Status);		/* clear RI bit */
	GMAC_DBG("RI is ok!\n");
	/* There is _at least_ 1 package in the fifo, read them all */
	pointto_cur_des = pt_to_last_edgerxdesc;
	while(1) {
		//inv_cache((uint32_t)pointto_cur_des, (uint32_t)pointto_cur_des + SINGLE_RXDES_SIZE);
		if(!(readl(pointto_cur_des) & RXLS)) {
			break;
		}
		//rdptr = (u8 *) (rxbuf_base + (pointto_cur_des - (u32 *)rxdes_base) / 4 *RBS1);
		pointto_cur_buf	= (u8 *)readl(pointto_cur_des + 2);
		rx_len = (readl(pointto_cur_des) >> 16) & 0x3fff;

		//inv_cache((uint32_t)pointto_cur_buf, (uint32_t)(pointto_cur_buf + rx_len));
		memcpy(rdptr, pointto_cur_buf, rx_len);

		GMAC_DBG("RXLS is ok!\n");
		rx_status = readl(Status);
		if ((rx_status & FBI) || (rx_len < MIN_SIZE) || (rx_len > MAX_MTU)) {
			if (rx_status & FBI) {
				printf("fatal bus error!\n");
			}
			
			if (rx_len < MIN_SIZE) {
				printf("size too small!\n");
			}

			if (rx_len > MAX_MTU) {
				printf("size too big!\n");
			}
		} else {
			GMAC_DMP_PACKET(rdptr, rx_len);

		 	GMAC_DBG("passing packet to upper layer\n");
			gmac_info.netdev.recv_process(gmac_info.netdev.recv_packets[0], rx_len);
		}
		writel(RXOWN, pointto_cur_des);
		//clean_cache((uint32_t)pointto_cur_des, (uint32_t)pointto_cur_des + SINGLE_RXDES_SIZE);

		pointto_cur_des += 4;       //goto next RxDes
		if (pointto_cur_des > ((u32 *)rxdes_base + (NO_OF_RXDES - 1) * 4)) {
			pointto_cur_des = (u32 *)rxdes_base;
		}
	}

	pt_to_last_edgerxdesc = pointto_cur_des;
 	GMAC_DBG("gmac_rx has been done\n");

	return rx_len;
}

#if 0
/* Read a word from phy */
static u16 gmac_phy_read(int reg)
{
	u16 ret;
	writel((0x1<<11) | (reg<<6) | (0x1<<2) | (0x0<<1) | (0x1<<0), GMIIAddr);
	while(readl(GMIIAddr) & PHYBUSY);	/* wait until reading completion */
	ret = readl(GMIIData);

	return ret;
}

/* Write a word to phy */
void gmac_phy_write(int reg, u16 value)
{
	writel(value, GMIIData);
	writel((0x1<<11) | (reg<<6) | (0x1<<2) | (0x1<<1) | (0x1<<0), GMIIAddr);
	while(readl(GMIIAddr) & PHYBUSY);	/* wait until writing completion */
}
#endif

unsigned char g_recv_packets_buff[2000];
unsigned char * g_recv_packets_ptr = g_recv_packets_buff;

int gmac_initialize(void)
{
	struct eth_device *dev = &(gmac_info.netdev);

	if (IROM_IDENTITY==IX_CPUID_Q3F || IROM_IDENTITY==IX_CPUID_APOLLO3)
		pads_chmod(PADSRANGE( 57,  65), 2, 0x1ff);
	else
		pads_chmod(PADSRANGE( 115,  123),2,0x1ff);

	gmac_meminit();

	module_enable(MAC_SYSM_ADDR);

	if (IROM_IDENTITY==IX_CPUID_Q3F || IROM_IDENTITY==IX_CPUID_APOLLO3) {
		pads_chmod(PADSRANGE( 57,  65), PADS_MODE_CTRL, 0);
		pads_pull(PADSRANGE( 57,  65), 0x01ff, 0x01ff);
	} else {
		pads_chmod(PADSRANGE( 115,  123), PADS_MODE_CTRL, 0);
		pads_pull(PADSRANGE( 115,  123), 0x01ff, 0x01ff);
	}

	/* configure eth to 100MHZ */
	writel(0x3, MAC_SYSM_RMII);
	writel(0xFF, MAC_SYSM_RESET);
	udelay(5); /* FIXME: ??? */
	writel(0x00, MAC_SYSM_RESET);
	
	dev->init = gmac_init;
	dev->halt = gmac_halt;
	dev->send = gmac_send;
	dev->recv = gmac_rx;
	dev->recv_process = NetReceive;
	sprintf(dev->name, "imapx_gmac");

	eth_register(dev);
	gmac_info.netdev.recv_packets = &g_recv_packets_ptr;
	
	return 0;
}
