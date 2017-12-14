/* imap_eth.c
 *
 * InfoTMIC gmac ethernet driver module implementation for Imap
 *
 * Copyright (c) 2014 Shanghai InfoTMIC Co.,Ltd.
 *
 * Version:
 * 1.8	Make sure the eth works at proper mode by auto-negotiation with the end.
 *	david, 12/11/2014 03:35:05 PM
 *
 * 1.7  Check received frame for errors and drop error frame.
 *      Support reset pin and PHY addr setting from items.
 *      xecle, 07/25/2014 10:56:37 AM
 *
 * 1.6  Support multicast via perfect MAC filter, support ethtool ops
 *      and use mii API to implement ioctl. Support RMII 10M.
 *      xecle, 07/02/2014 03:39:09 PM
 *
 * 1.5  Use dynamic check interval and fix MAC recognization from env.
 *      xecle, 05/06/2014 11:20:56 AM
 *
 * 1.4  Add isolate support for power saving when interface down.
 *      The function only work with positive isolate.
 *      xecle, 03/13/2014 03:31:17 PM
 *
 * 1.3  Fix random MAC to match the specification.
 *      xecle, 03/07/2014 04:27:37 PM
 *
 * 1.2  Support NAPI and detach device when suspend.
 *      xecle, 01/15/2014 07:56:09 PM
 *
 * 1.1  Port to Linux 3.10 and change coding style.
 *      xecle, 12/21/2013 04:15:00 PM
 *
 * 1.0  Created.
 *      xecle, 05/03/2013 10:08:13 AM
 *
 */

#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/mii.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>

#include <mach/nvedit.h>
#include <mach/items.h>
#include <mach/pad.h>
#include <mach/power-gate.h>
#include <mach/hw_cfg.h>

#include "imap_eth.h"
#include <linux/clk.h>

#define DEBUG 0

#define TAG	"ImapGMAC"
#define DRV_VER	"1.6"
#define FM_VER	"1.1"
#define DRV_BUS	"bus7"

#if DEBUG
#define DD(x...)	printk(TAG"/D:"x)
#else
#define DD(...)		do {} while (0)
#endif
#define INFO(x...)	printk(TAG"/I:"x)
#define ERR(x...)	printk(TAG"/E:"x)

#define FUNC_INFO	printk(TAG"/F: Function %s called\n", __func__)

/*  Transmit timeout, default 5 seconds. */
static int timeout_tx = 5000;
module_param(timeout_tx, int, 0400);
MODULE_PARM_DESC(timeout_tx, "transmit timeout in milliseconds");

void ig_delay(uint32_t delay)
{
	while (delay--)
		;
}

#ifdef CONFIG_OMC300_ETH_PHY
/*********************************************************/
/**                 For OMC300 ETH                      **/
/**                                                     **/
/*********************************************************/
void correct_Vref(void){
	int16_t val = 0;
	ip2906_read(0x7, &val);
	val &= 0xffc0;
	val |= 0x1e;
	ip2906_write(0x7, val);
}

void correct_Iref(void){
	int16_t val = 0;
	//ip2906_write(0x1, 0x30);
	//ip2906_write(0x4, 0x2);
	//ip2906_write(0x4d, 0xa00e);
	ip2906_read(0x9, &val);
	val &= 0xffc0;
	val |= 0x7;
	ip2906_write(0x9, val);
}
/*write PHY internal register by I2C */
void omc300_phy_inter_write(struct net_device *netdev, int addr, uint8_t reg, uint16_t value)
{
    int16_t val = 0;
    int loop = DEFAULT_LOOP_NUM;

	if(reg > 0x6 && reg != 0x10 && reg != 0x18){
		INFO("OMC300 PHY do not support the register\n");
		return 0;
	}

	if(reg || (!reg && !(value && 0x3100))){
		/* wait for the SMI bus process completion */
		ip2906_read(IP2906_PHY_SMI_STAT, &val);
		while((val & 0x1) && (loop > 0)){
			ip2906_read(IP2906_PHY_SMI_STAT, &val);
			ig_delay(DEFAULT_LOOP_DEALY);
			loop --;
		}
		if(loop == 0){
			INFO("omc300 eth phy internel write timeout!\n");
			return;
		}

		ip2906_write(IP2906_PHY_SMI_DATA, value);
		/* smi write operate,address,phy addr=1 */
		ip2906_write(IP2906_PHY_SMI_CONFIG, 0xc100 | reg);
	}else{
		if(value & BMCR_ANENABLE)
			ip2906_write(IP2906_PHY_CONFIG, 0x0);
		else
			ip2906_write(IP2906_PHY_CONFIG, 0x0);
		if(value & BMCR_SPEED100)
			ip2906_write(IP2906_PHY_CTRL, 0x11);
		else
			ip2906_write(IP2906_PHY_CTRL, 0x10);

		/* reset the eth phy */
		ip2906_write(IP2906_PHY_HW_RST, 0x01);

		/* waite for reset process completion */
		ip2906_read(IP2906_PHY_HW_RST, &val);
		while((val & 0x1) && (loop > 0)){
			ip2906_read(IP2906_PHY_HW_RST, &val);
			ig_delay(DEFAULT_LOOP_DEALY);
			loop --;
		}
		if(loop == 0)
			INFO("omc300 eth phy init failed!\n");
	}
}

/* read PHY internal register by I2C */
int16_t omc300_phy_inter_read(struct net_device *netdev, int addr, uint8_t reg)
{
    int16_t val = 0, val_led = 0;
    int loop = DEFAULT_LOOP_NUM;

	if(reg > 0x6 && reg != 0x10 && reg != 0x18){
		INFO("OMC300 PHY do not support the register\n");
		return 0;
	}

	/* wait for the SMI bus process completion */
	ip2906_read(IP2906_PHY_SMI_STAT, &val);
	while((val & 0x1) && (loop > 0)){
		ip2906_read(IP2906_PHY_SMI_STAT, &val);
		ig_delay(DEFAULT_LOOP_DEALY);
		loop --;
	}
	if(loop == 0){
		INFO("omc300 eth phy internel read timeout!\n");
		return -1;
	}

	/* smi read operate,address,phy addr=1 */
	ip2906_write(IP2906_PHY_SMI_CONFIG, 0x8100 | reg);

	/* wait for the SMI bus process completion */
	loop = DEFAULT_LOOP_NUM;
	ip2906_read(IP2906_PHY_SMI_STAT, &val);
	while((val & 0x1) && (loop > 0)){
		ip2906_read(IP2906_PHY_SMI_STAT, &val);
		ig_delay(DEFAULT_LOOP_DEALY);
		loop --;
	}
	if(loop == 0){
		INFO("omc300 eth phy internel read timeout!\n");
		return -1;
	}

	ip2906_read(IP2906_PHY_SMI_DATA, &val);

	if(!reg){
		ip2906_read(IP2906_PHY_LED, &val_led);
		if(val_led & 0x4)
			val |= 0x80;
		else
			val &= 0xff7f;
		if(!(val_led & 0x10))
			val |= 0x2000;
		else
			val &= 0xdfff;
		if(!(val_led & 0x8))
			val |= 0x100;
		else
			val &= 0xfeff;
	}
	return val;
}

void omc300_eth_phy_init()
{
    int16_t val = 0;
    int loop = DEFAULT_LOOP_NUM;
    /* select Ethernet pin ,Reg0x01 bit0 = 0 */
    ip2906_write(IP2906_MFP_PAD, 0x0);

    /* Ethernet Clock Enable, Reg0x04 bit1=1 */
    ip2906_write(IP2906_CMU_CLKEN, 0x2);

    /* reset the eth phy */
    ip2906_write(IP2906_DEV_RST, 0x0);

    /* set the eth phy to the normal module */
    ip2906_write(IP2906_DEV_RST, 0x7);

    /* ethernet pll ldo enable : 0xa204 */
    ip2906_write(IP2906_PHY_PLL_CTL1, 0xa204);

    /* pll enable */
    ip2906_write(IP2906_PHY_PLL_CTL0, 0xa95);

    /* set phy address=  0x01 */
    ip2906_write(IP2906_PHY_ADDR, 0x1);

	/* rmii,100Mbps */
    ip2906_write(IP2906_PHY_CTRL, 0x11);

	/* auto-negotiation with all capabilities enable */
	ip2906_write(IP2906_PHY_CONFIG, 0x0);

    /* reset the eth phy */
    ip2906_write(IP2906_PHY_HW_RST, 0x01);

    /* waite for reset process completion */
    ip2906_read(IP2906_PHY_HW_RST, &val);
    while((val & 0x1) && (loop > 0)){
        ip2906_read(IP2906_PHY_HW_RST, &val);
        ig_delay(DEFAULT_LOOP_DEALY);
        loop --;
    }
	if(loop == 0)
		INFO("omc300 eth phy init failed!\n");
}

/* reset PHY internal */
void omc300_eth_phy_inter_reset(struct net_device *netdev)
{
    int16_t val = 0;
    int loop = DEFAULT_LOOP_NUM;

    val = omc300_phy_inter_read(netdev ,-1, IP2906_PHY_INTER_BMCR);

    omc300_phy_inter_write(netdev, -1, IP2906_PHY_INTER_BMCR, 0x8000 | val);

    /* wait for PHY reset completed */
    val = omc300_phy_inter_read(netdev, -1, IP2906_PHY_INTER_BMCR);
    while((val & 0x8000) && (loop > 0)){
        val = omc300_phy_inter_read(netdev, -1, IP2906_PHY_INTER_BMCR);
        ig_delay(DEFAULT_LOOP_DEALY);
        loop --;
    }
    if(loop == 0)
        INFO("omc300 reset PHY internal timeout\n");
}

/* set omc300 eth phy func */
void omc300_set_eth_phy_func(struct net_device *netdev)
{
    int16_t val = 0;
    int loop = DEFAULT_LOOP_NUM;

    /* set to 1 for fully supported parallel detection function */
    val = omc300_phy_inter_read(netdev, -1, IP2906_PHY_INTER_PHYTC1);
    omc300_phy_inter_write(netdev, -1, IP2906_PHY_INTER_PHYTC1, 0x1000 | val);

    /* set to 2¡¯b11 to prevent compatibility issues */
    val = omc300_phy_inter_read(netdev, -1, IP2906_PHY_INTER_PHYTC2);
    omc300_phy_inter_write(netdev, -1, IP2906_PHY_INTER_PHYTC2, 0x000c | val);

    /* wait for Valid link established */
    val = omc300_phy_inter_read(netdev, -1, IP2906_PHY_INTER_BMSR);
    while(!(val & 0x4) && (loop > 0)){
        val = omc300_phy_inter_read(netdev, -1, IP2906_PHY_INTER_BMSR);
        ig_delay(DEFAULT_LOOP_DEALY);
        loop --;
    }
    if(loop == 0)
        INFO("set omc300 eth phy func timeout\n");
}

void omc300_set_speed_after_link_up(){
    int16_t val = 0;
    int loop = DEFAULT_LOOP_NUM;
	ip2906_read(IP2906_PHY_LED, &val);
	if ((val & 0x60) ==0x20) {         //bit6=0 && bit5=1， link ok
		if (val & 0x10)
			 ip2906_write(IP2906_PHY_CTRL, 0x10);          //10M，negedge clock
		else
			ip2906_write(IP2906_PHY_CTRL, 0x11);          //100M，negedge clock
		/* reset the eth phy */
		ip2906_write(IP2906_PHY_HW_RST, 0x01);

		/* waite for reset process completion */
		ip2906_read(IP2906_PHY_HW_RST, &val);

		while((val & 0x1) && (loop > 0)){
			ip2906_read(IP2906_PHY_HW_RST, &val); 
			ig_delay(DEFAULT_LOOP_DEALY);
			 loop --; 
		}
		if(loop == 0)
			INFO("omc300_set_speed_after_link_up config timeout!\n");
	}else
		INFO("omc300_set_speed_after_link_up failed!\n");
}

static uint8_t reg_addr;
static ssize_t eth_reg_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint16_t val;
	ip2906_read(reg_addr, &val);	
	return sprintf(buf, "REG[0x%02x]=0x%04x\n", reg_addr, val);
}

static ssize_t eth_reg_store(struct class *class,
		struct class_attribute *attr,
		const char *buf, size_t count)
{
	uint16_t val;
	uint32_t tmp;
	tmp = simple_strtoul(buf, NULL, 16);
	if (tmp <= 0xff) {
		if (strncmp(buf, "00", 2))
			reg_addr = tmp;
		else 
			ip2906_write(0x0, tmp);
	} else {
		val = tmp & 0xff;
		reg_addr = (tmp >> 8) & 0xff;
		ip2906_write(reg_addr, val);
	}
	return count;
}

static uint8_t inter_reg_addr;
static struct ig_adapter *gadapter;
static ssize_t eth_inter_reg_show(struct class *class,
		struct class_attribute *attr, char *buf)
{
	uint16_t val;
	val = omc300_phy_inter_read(gadapter->netdev, -1, inter_reg_addr);
	return sprintf(buf, "REG[0x%02x]=0x%04x\n", inter_reg_addr, val);
}

static ssize_t eth_inter_reg_store(struct class *class,
		struct class_attribute *attr,
		const char *buf, size_t count)
{
	uint16_t val;
	uint32_t tmp;
	tmp = simple_strtoul(buf, NULL, 16);
	if (tmp <= 0xff) {
		if (strncmp(buf, "00", 2))
			inter_reg_addr = tmp;
		else 
			omc300_phy_inter_write(gadapter->netdev, -1, IP2906_PHY_INTER_BMCR, 0x8000 | tmp);
	} else {
		val = tmp & 0xffff;
		inter_reg_addr = (tmp >> 16) & 0xff;
		omc300_phy_inter_write(gadapter->netdev, -1, inter_reg_addr, 0x8000 | val);
	}
	return count;
}

static struct class_attribute eth_class_attrs[] = {
	__ATTR(reg, 0644, eth_reg_show, eth_reg_store),
	__ATTR(inter_reg, 0644, eth_inter_reg_show, eth_inter_reg_store),
	__ATTR_NULL
};

static struct class eth_class = {
	.name = "eth-debug",
	.class_attrs = eth_class_attrs,
};
#endif

/* Isolate PHY for power saving */
void ig_isolate(struct ig_adapter *adapter, int enable)
{
	int pin = adapter->isolate;
	if (pin > 0) {
//		imapx_pad_set_mode(MODE_GPIO, 1, pin);
//		imapx_pad_set_dir(DIRECTION_OUTPUT, 1, pin);
//		imapx_pad_set_outdat(enable?OUTPUT_1:OUTPUT_0, 1, pin);
		gpio_direction_output(pin, enable?1:0);
	}
}

int ig_hw_init(struct ig_adapter *adapter, int reset)
{
	int loop;

	/* disable isolate */
	ig_isolate(adapter, 0);

	/* power open in system*/
	module_power_on(SYSMGR_MAC_BASE);
	/* pads config in system*/
#ifdef CONFIG_OMC300_ETH_PHY
	imapx_pad_init("phy-mux");
#else
	imapx_pad_init("phy");
#endif

	/*RMII enable and set to 100M*/
	writel(0x3, IO_ADDRESS(SYSMGR_MAC_RMII));

	if (reset) {
		/* reset MAC in system control*/
		module_reset(SYSMGR_MAC_BASE, 1);
	}

	/* wait until GMAC clock up */
	for (loop = 0; loop < DEFAULT_LOOP_NUM; loop++) {
		if (readl(adapter->base_addr+VERR) == 0)
			ig_delay(DEFAULT_LOOP_DEALY);
		else
			break;
	}

	return 0;
}

void ig_hw_close(struct ig_adapter *adapter)
{
	ig_isolate(adapter, 1);
	/* power down in system */
	module_power_down(SYSMGR_MAC_BASE);
}

int ig_phy_read(struct net_device *netdev, int addr, int reg)
{
	int loop;
	uint32_t phy_addr;
	uint32_t phy_data = 0;
	struct ig_adapter *adapter = netdev_priv(netdev);
	void *base = adapter->base_addr;

	phy_addr = ((addr<<GMIIPHYSHIFT) & GMIIPHYMASK) |
	    ((reg<<GMIIREGSHIFT) & GMIIREGMASK) | CLK_CSR_4;

	/* wait for GMII free*/
	for (loop = 0; loop < DEFAULT_LOOP_NUM; loop++) {
		if (!(readl(base+GAR) & GMIIBUSY))
			break;
		else
			ig_delay(DEFAULT_LOOP_DEALY);
	}

	/* write Phy addr and set busy bit*/
	writel(phy_addr|GMIIBUSY, base+GAR);

	/* wait for GMII free*/
	for (loop = 0; loop < DEFAULT_LOOP_NUM; loop++) {
		if (!(readl(base+GAR) & GMIIBUSY))
			break;
		else
			ig_delay(DEFAULT_LOOP_DEALY);
	}

	if (loop < DEFAULT_LOOP_NUM) {
		/* read data from GMII data*/
		phy_data = readl(base+GDR);
	} else {
		ERR("Phy read addr %08x timeout\n", phy_addr);
		return 0;
	}
	return *((int*)&phy_data);

}

void ig_phy_write(struct net_device *netdev, int addr, int reg, int val)
{
	int loop;
	uint32_t phy_addr;
	struct ig_adapter *adapter = netdev_priv(netdev);
	void *base = adapter->base_addr;
	phy_addr = ((addr<<GMIIPHYSHIFT) & GMIIPHYMASK) |
		((reg<<GMIIREGSHIFT) & GMIIREGMASK) | CLK_CSR_4 | GMIIWRITE;

	/* wait for GMII free*/
	for (loop = 0; loop < DEFAULT_LOOP_NUM; loop++) {
		if (!(readl(base+GAR) & GMIIBUSY))
			break;
		else
			ig_delay(DEFAULT_LOOP_DEALY);
	}

	/* write data to GMII data*/
	writel(val, base+GDR);
	/* write Phy addr and set busy bit*/
	writel(phy_addr|GMIIBUSY, base+GAR);
	/* wait for GMII free*/
	for (loop = 0; loop < DEFAULT_LOOP_NUM; loop++) {
		if (!(readl(base+GAR) & GMIIBUSY))
			break;
		else
			ig_delay(DEFAULT_LOOP_DEALY);
	}

	if (loop >= DEFAULT_LOOP_NUM) {
		ERR("Phy write %08x to addr %08x not responding\n",
			val, phy_addr);
		return;
	}
}

uint32_t ig_gmac_version(struct ig_adapter *adapter)
{
	uint32_t tmp;
	tmp = readl(adapter->base_addr+VERR);
	return tmp;
}

uint32_t ig_phy_id(struct ig_adapter *adapter)
{
	int tmp1, tmp2;

	tmp1 = ig_phy_read(adapter->netdev, adapter->phydev, MII_PHYSID1);
	tmp2 = ig_phy_read(adapter->netdev, adapter->phydev, MII_PHYSID2);
	INFO("PHY ID is %x:%x\n", tmp1, tmp2);

	/*
	 * The PHY IDs of IP101A-LF is 0x0243:0x0C54, but we should think
	 * for other PHY chip. 16bit and positive may be good.
	 */
	if (tmp1 <= 0 || tmp2 <= 0)
		return 0;
	else
		return ((tmp1&0xFFFF) << 16) | (tmp2&0xFFFF);
}

static void ig_update_speed_encode(struct ig_adapter *adapter)
{
	int speed, duplex;
	uint32_t mcr;

	mcr = readl(adapter->base_addr+MCR);
	mcr &= ~(FES|DM);

#ifdef CONFIG_OMC300_ETH_PHY
	int16_t val = 0;
	ip2906_read(IP2906_PHY_LED, &val);
	if ((val & 0x60) ==0x20) {         //bit6=0 && bit5=1， link ok 
		if (val & 0x10) {      
			speed = 0;   
			if(val & 0x8)    
				duplex = 0;       
			else
				duplex = 1;       
		}
		else if(!(val & 0x10)){
			speed = 1;        
			if(val & 0x8)    
				duplex = 0;       
			else
				duplex = 1;       
		}
	}else
		printk("No valid link established\n"); 
#else
	uint32_t tmp;
	int bmcr, bmsr, adv, lpa;
	bmcr = ig_phy_read(adapter->netdev, adapter->phydev, MII_BMCR);
	bmsr = ig_phy_read(adapter->netdev, adapter->phydev, MII_BMSR);
	adv = ig_phy_read(adapter->netdev, adapter->phydev, MII_ADVERTISE);
	lpa = ig_phy_read(adapter->netdev, adapter->phydev, MII_LPA);
	INFO("bmcr:%x, bmsr:%x, adv:%x, lpa:%x\n", bmcr,bmsr,adv,lpa);

	tmp = adv & lpa;

	if (bmcr & BMCR_ANENABLE) {
		if ((bmsr & BMSR_ANEGCAPABLE) && (bmsr & BMSR_ANEGCOMPLETE)) {
			if (tmp & LPA_100FULL) {
				speed = SPEED_100M;
				duplex = DUPLEX_FULL;
			} else if ((tmp & LPA_100HALF) || (tmp & LPA_100BASE4)) {
				speed = SPEED_100M;
				duplex = DUPLEX_HALF;
			} else if (tmp & LPA_10FULL) {
				speed = SPEED_10M;
				duplex = DUPLEX_FULL;
			} else if (tmp & LPA_10HALF) {
				speed = SPEED_10M;
				duplex = DUPLEX_HALF;
			}
		} else if (!(bmsr & BMSR_ANEGCAPABLE)) {
			printk("The end don't support auto-negotiation\n");
			if (tmp & LPA_100HALF)
				speed = SPEED_100M;
			else
				speed = SPEED_10M;

			duplex = DUPLEX_HALF;
		}
	} else {
		printk("PHY disable auto-negotiation func\n");
		speed = (bmcr & (1<<13)) ? SPEED_100M : SPEED_10M;
		duplex = (bmcr & (1<<8)) ? DUPLEX_FULL : DUPLEX_HALF;
	}
#endif

	if (speed)
		writel(0x3, IO_ADDRESS(SYSMGR_MAC_RMII));
	else
		writel(0x1, IO_ADDRESS(SYSMGR_MAC_RMII));

	mcr |= speed << 14;
	mcr |= duplex << 11;
	writel(mcr, adapter->base_addr+MCR);
	INFO("Speed %dM, Duplex %s\n", speed ? 100 : 10, duplex ? "Full": "Half");
}

static void ig_phy_check(struct ig_adapter *adapter)
{
	int link_up;
#ifdef CONFIG_OMC300_ETH_PHY
	int data = omc300_phy_inter_read(adapter->netdev, -1, IP2906_PHY_INTER_BMSR);
#else
	int data = ig_phy_read(adapter->netdev, adapter->phydev, MII_BMSR);
#endif
	if(data == 0)
		return;
	if ((data & BMSR_LSTATUS) &&
		(!(data & BMSR_ANEGCAPABLE) || (data & BMSR_ANEGCOMPLETE)))
		link_up = 1;
	else
		link_up = 0;

#ifdef CONFIG_OMC300_ETH_PHY
	if(adapter->first_time_link_up == 1)
		omc300_set_speed_after_link_up();
#endif

	if (link_up != adapter->link_up) {
		INFO("Link %s, data %08x\n", link_up ? "UP" : "Down", data);
		adapter->link_up = link_up;
		adapter->delay_time = 1;
		adapter->delay_num = 0;
		if (link_up) {
			netif_carrier_on(adapter->netdev);
			ig_update_speed_encode(adapter);
#ifdef CONFIG_OMC300_ETH_PHY
			adapter->first_time_link_up ++;
#endif
		} else {
			netif_carrier_off(adapter->netdev);
#ifdef CONFIG_OMC300_ETH_PHY
			adapter->first_time_link_up ++;
#endif
		}
	}
}

static void ig_cable_plug_check(struct work_struct *work)
{
	struct delayed_work *dw = container_of(work, struct delayed_work, work);
	struct ig_adapter *adapter =
		container_of(dw, struct ig_adapter, work_queue);

	ig_phy_check(adapter);

	if(adapter->delay_num <= 5) {
		adapter->delay_num += 1;
		if(adapter->delay_num == 5)
			adapter->delay_time = 3;
	}
	if(netif_running(adapter->netdev))
		schedule_delayed_work(&adapter->work_queue,
				(adapter->delay_time*HZ));
}

void init_mac_addr(struct ig_adapter *adapter)
{
	uint8_t mac[6];
	uint8_t prefix[6] = IMAP_ETH_MAC_PREFIX;
	int i;
	int mac_src = 0;
	char *mac_env, *p;
	uint32_t num;

#ifdef CONFIG_INFOTM_ENV
	mac_env = infotm_getenv(MAC_ENV);
	if (mac_env) {
		mac_src = 1;
		for (i = 0; i < 6; i++) {
			num = simple_strtoul(mac_env, &p, 16);
			mac[i] = num & 0xFF;
			mac_env = p + 1;
		}
		WARN((mac[0] & 0x1), "Multicast MAC address for ethernet");
	}
#endif
	if (mac_src == 0) {
		mac_src = 2;
		for (i = 0; i < 6; i++)
			mac[5-i] = readl(IMAP_ETH_MAC_ADDR + i*4) & 0xFF;
		for (i = 0; i < 6; i++) {
			if (prefix[i] != 0 && prefix[i] != mac[i]) {
				mac_src = 0;
				break;
			}
		}
	}
	if (mac_src == 0) {
		get_random_bytes(adapter->mac_addr, 6);
		adapter->mac_addr[0] = prefix[0] + 2;
		INFO("Golden MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
			adapter->mac_addr[0], adapter->mac_addr[1],
			adapter->mac_addr[2], adapter->mac_addr[3],
			adapter->mac_addr[4], adapter->mac_addr[5]);
	} else {
		for (i = 0; i < 6; i++)
			adapter->mac_addr[i] = mac[i];
		INFO("%s MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
				(mac_src == 2) ? "Chip" : "Env",
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
}

void ig_set_mac_addr(struct ig_adapter *adapter)
{
	uint32_t tmp;
	uint8_t *mac = adapter->netdev->dev_addr;
	tmp = mac[5]<<8 | mac[4];
	DD("MAC address H %08x\n", tmp);
	writel(tmp, adapter->base_addr+MAHR0);
	tmp = mac[3]<<24 | mac[2]<<16 | mac[1]<<8 | mac[0];
	DD("MAC address L %08x\n", tmp);
	writel(tmp, adapter->base_addr+MALR0);
}

void ig_mac_reset(struct ig_adapter *adapter)
{
	uint32_t tmp;
	int loop;
	tmp = readl(adapter->base_addr+BMR);
	tmp |= SWR;
	writel(tmp, adapter->base_addr+BMR);
	for (loop = 0; loop < DEFAULT_LOOP_NUM; loop++) {
		if (!(readl(adapter->base_addr+BMR) & SWR))
			break;
		else
			ig_delay(DEFAULT_LOOP_DEALY);
	}
	if (loop >= DEFAULT_LOOP_NUM)
		ERR("reset GMAC timeout\n");
}

void ig_mac_init(struct ig_adapter *adapter)
{
	uint32_t tmp;
	/* Config the DMA Bus Mode Register */
	writel(ALL|PBLx8|FB|((DEFAULT_PBL<<PBLSHIFT)&PBLMASK)|
		((DEFAULT_DSL<<DSLSHIFT)&DSLMASK), adapter->base_addr+BMR);
	/* Config the DMA Tx and Rx Descriptor list */
	writel(adapter->tx_dma_base, adapter->base_addr+TDLAR);
	writel(adapter->rx_dma_base, adapter->base_addr+RDLAR);
	/* Config the DMA operation Mode Register */
	writel(TTC1|FEF|RTC3, adapter->base_addr+OMR);

	/* Config the GMAC MAC Config Register */
	writel(TC|WD|BE|JE|IFG7|FES|DM|ACS|BL1|TE|RE, adapter->base_addr+MCR);

	/* Config the Flow Control */
	writel(((DEFAULT_PT<<PTSHIFT)&PTMASK)|RFE|TFE, adapter->base_addr+FCR);

	/* Clear DMA Interrupt Status */
	writel(0xFFFFFFFF, adapter->base_addr+DMASR);

	/* Set MAC Address */
	ig_set_mac_addr(adapter);

	/* start Rx and Tx DMA */
	tmp = readl(adapter->base_addr+OMR);
	tmp |= (ST|SR);
	writel(tmp, adapter->base_addr+OMR);
	DD("MAC Config Reg %08x\n", readl(adapter->base_addr+MCR));
}

void ig_init_dma_tx_ring(struct ig_dma_des *des, int num)
{
	int i;
	for (i = 0; i < num; i++) {
		des->status = 0;
		des->length = TIC|TCIC3|TTSE;
		des->data1 = 0;
		des->data2 = 0;
		/*des->buff1 = 0;*/
		/*des->buff2 = 0;*/
		des++;
	}
	des--;
	des->length = TIC|TCIC3|TTSE|TER;

}

void ig_setup_rx_des(struct ig_adapter *adapter, struct ig_dma_des *des)
{
	uint32_t tmp;
	struct sk_buff *skb;
	dma_addr_t dma;
	uint32_t len = DEFAULT_MTU;
	/* alloc a ip aligned skb buff*/
	skb = netdev_alloc_skb_ip_align(adapter->netdev, len);
	if (!skb) {
		ERR("alloc skb failed, no memory\n");
		return;
	}

	/* map the skb data and set to DMA descriptor */
	dma = dma_map_single(adapter->dev, skb->data, len, DMA_FROM_DEVICE);
	if (!dma) {
		ERR("setup rx descriptor failed!\n");
		return;
	}
	tmp = des->length&RER;  /* save RER bit*/
	des->length = tmp | ((len<<RBS1SHIFT)&RBS1MASK);
	des->data1 = dma;
	/*des->data2 = 0;*/
	des->buff1 = (void *)skb;
	/*des->buff2 = 0;*/
	des->status = ROWN;
}

void ig_free_rx_ring(struct ig_adapter *adapter, struct ig_dma_des *des,
		int num)
{
	int i;
	for (i = 0; i < num; i++) {
		dma_unmap_single(adapter->dev, des->data1,
			(des->length&RBS1MASK)>>RBS1SHIFT, DMA_FROM_DEVICE);
		dev_kfree_skb((struct sk_buff *)des->buff1);
		des++;
	}
}

void ig_init_dma_rx_ring(struct ig_adapter *adapter, struct ig_dma_des *des,
		int num)
{
	int i;
	for (i = 0; i < num; i++) {
		ig_setup_rx_des(adapter, des);
		des++;
	}
	des--;
	des->length |= RER;

}

int ig_tx_clean(struct ig_adapter *adapter)
{
	uint32_t entry;
	struct ig_dma_des *des;
	struct sk_buff *skb;
	struct net_device *netdev = adapter->netdev;
	int work_done = 0;

	entry = adapter->current_tx_entry & (RING_TX_LENGTH-1);
	des = adapter->tx_des_base + entry;
	while (adapter->current_tx_entry < adapter->tx_entry &&
			!(des->status&TOWN)) {
		skb = adapter->tx_skb[entry].skb;
		if (des->status&TES) {
			ERR("Transmit Package error %08x\n",
					des->status);
			netdev->stats.tx_errors++;
		} else {
			netdev->stats.tx_packets++;
			dma_unmap_single(adapter->dev, des->data1,
					skb->len, DMA_TO_DEVICE);
			dev_kfree_skb_irq(skb);
		}
		adapter->current_tx_entry++;
		entry = adapter->current_tx_entry & (RING_TX_LENGTH-1);
		des = adapter->tx_des_base + entry;
		work_done++;
	}
	if (adapter->tx_entry-adapter->current_tx_entry < MAX_TX_FRAME)
		netif_wake_queue(netdev);
	return work_done;
}

static irqreturn_t ig_handler(int irq, void *dev)
{
	uint32_t	status;
	uint32_t	tmp;
	struct net_device *netdev = dev;
	struct ig_adapter *adapter = netdev_priv(netdev);
	//	INFO(" IRQ : Enter\n");

	/* get interrupt status */
	status = readl(adapter->base_addr + DMASR);
	/* writel to clear interrupt status, TI, RI, NIS */
	writel(status&(TI|RI|NIS), adapter->base_addr + DMASR);

	/* Check for errors */
	if (status&(TTI|GPI|GMI|GLI))
		INFO("GMAC core error, status %08x\n", status);
	if (status&AIS) {
		INFO("DMA Abnormal error, status %08x\n", status);
		/* Clear all DMA status */
		writel(0xFFFFFFFF, adapter->base_addr + DMASR);
	}

	/* Handle Transmit Interrupt */
	if (status&TI)
		ig_tx_clean(adapter);

	/* Handle Receive Interrupt */
	if (status&RI) {
		/*
		 * disable normal interrupt, handle receive and transmit in
		 * poll method.
		 */
	//	INFO(" IRQ : call napi\n");
		tmp = readl(adapter->base_addr + IER);
		tmp &= ~(NIE|RIE|TIE);
		writel(tmp, adapter->base_addr + IER);
		napi_schedule(&adapter->napi);
	}
	//	INFO(" IRQ : DONE\n");

	return IRQ_HANDLED;

}

int ig_napi_poll(struct napi_struct *napi, int budget)
{
	int		ret;
	uint32_t	tmp;
	uint32_t	len;
	uint32_t	des_status;
	struct ig_dma_des *des;
	struct sk_buff *skb;
	struct ig_adapter *adapter =
		container_of(napi, struct ig_adapter, napi);
	struct net_device *netdev = adapter->netdev;
	int work_done = 0;

	/* handle transmit */
	ig_tx_clean(adapter);

	/* Handle Receive Interrupt */
	des = adapter->current_rx_des;
	while (!((des_status = des->status)&ROWN)) {
		/* get skb and set skb info */
		skb = des->buff1;
		len = (des_status&RFLMASK)>>RFLSHIFT;

		dma_unmap_single(adapter->dev, des->data1,
				(DEFAULT_MTU), DMA_FROM_DEVICE);
		if (des_status&RES) {
			INFO(" R : Receive a package with errors, status %08x\n", des_status);
			dev_kfree_skb_any(skb);
			netdev->stats.rx_errors++;
			ret = NET_RX_DROP;
		} else if (len > skb_tailroom(skb)) {
			/* bad length for skb_put, drop this frame */
			INFO(" Too long length for skb received\n");
			dev_kfree_skb_any(skb);
			ret = NET_RX_DROP;
		} else {
			skb_put(skb, len);
			skb->protocol = eth_type_trans(skb, netdev);

			/* send skb to up layer */
			ret = netif_receive_skb(skb);
			netdev->last_rx = jiffies;
			netdev->stats.rx_bytes += len;
		}

		if (des_status & RLS) {
			if (ret == NET_RX_SUCCESS)
				netdev->stats.rx_packets++;
			else
				netdev->stats.rx_dropped++;
		}

		/* resetup the Descriptor, not clear the RER bit */
		ig_setup_rx_des(adapter, des);

		if (des->length&RER)
			des = adapter->rx_des_base;
		else
			des++;
		/* check budget */
		work_done++;
		if (work_done >= budget)
			break;
	}
	/* update current Rx descriptor */
	adapter->current_rx_des = des;

	if (work_done < budget) {
		/* stop napi poll */
		napi_complete(napi);
		/* enable normal interrupt */
		tmp = readl(adapter->base_addr + IER);
		tmp |= (NIE|RIE|TIE);
		writel(tmp, adapter->base_addr + IER);
	}
	return work_done;

}

/*********************************************************/
/**                                                     **/
/**                 NETWORK DEVICE HOOKS                **/
/**                                                     **/
/*********************************************************/
static int ig_open(struct net_device *netdev)
{
	struct ig_adapter *adapter = netdev_priv(netdev);

#ifdef CONFIG_OMC300_ETH_PHY
    /* init omc300 eth phy */
    omc300_eth_phy_init();
#endif

	/* init GMAC hardware in system control */
	ig_hw_init(adapter, 1);
	DD("init gmac hw in system control\n");

	/* request system irq for handle */
	if (request_irq(netdev->irq, &ig_handler, IRQF_DISABLED,
		    netdev->name, netdev)) {
		ERR("request irq %d failed\n", netdev->irq);
		return -EAGAIN;
	}

	/* enable napi poll */
	napi_enable(&adapter->napi);

	/* reset GMAC */
	ig_mac_reset(adapter);

	/* init GMAC */
	ig_mac_init(adapter);

#ifdef CONFIG_OMC300_ETH_PHY
    /* reset omc300 eth PHY internal */
    omc300_eth_phy_inter_reset(netdev);
#else
	/* reset IP101 PHY */
	ig_phy_write(netdev, adapter->phydev, MII_BMCR, BMCR_RESET);
#endif

#ifdef CONFIG_OMC300_ETH_PHY
    omc300_set_eth_phy_func(netdev);
#endif

	/* carrier default state */
	adapter->link_up = netif_carrier_ok(netdev);
	/* check phy state */
	ig_phy_check(adapter);

	/* save Rx current Descriptor for receive */
	adapter->current_rx_des = adapter->rx_des_base +
	    (readl(adapter->base_addr+CHRDR)-adapter->rx_dma_base)/
	    sizeof(struct ig_dma_des);

	adapter->tx_entry = (readl(adapter->base_addr+CHTDR) -
		adapter->tx_dma_base)/sizeof(struct ig_dma_des);
	adapter->current_tx_entry = adapter->tx_entry;
	memset(adapter->tx_skb, 0, sizeof(struct ig_skb_entry)*RING_TX_LENGTH);

	adapter->start = 1;
	netif_start_queue(netdev);
	adapter->delay_time = 1;
	adapter->delay_num = 0;
	schedule_delayed_work(&adapter->work_queue, adapter->delay_time*HZ);

	/* enable interrupt */
	writel(NIE|RIE|TIE|AIE|FBE|RWE|RSE|TSE|TJE, adapter->base_addr + IER);

	return 0;
}

static int ig_stop(struct net_device *netdev)
{
	uint32_t	entry;
	struct ig_dma_des *des;
	struct sk_buff *skb;
	struct ig_adapter *adapter = netdev_priv(netdev);

	cancel_delayed_work_sync(&adapter->work_queue);
	netif_stop_queue(netdev);
	napi_disable(&adapter->napi);
	netif_carrier_off(netdev);

	/* disable all interrupt */
	writel(0, adapter->base_addr + IER);

	/* Stop DMA */
	writel(0, adapter->base_addr + OMR);

	/* clear tx ring */
	entry = adapter->current_tx_entry & (RING_TX_LENGTH-1);
	des = adapter->tx_des_base + entry;
	while (adapter->current_tx_entry < adapter->tx_entry) {
		skb = adapter->tx_skb[entry].skb;
		dma_unmap_single(adapter->dev, des->data1, skb->len,
			DMA_TO_DEVICE);
		dev_kfree_skb(skb);
		adapter->current_tx_entry++;
		entry = adapter->current_tx_entry & (RING_TX_LENGTH-1);
		des = adapter->tx_des_base + entry;
	}

	/* clear rx ring */
	des = adapter->current_rx_des;
	while (!(des->status&ROWN)) {
		des->status = ROWN;
		if (des->length&RER)
			des = adapter->rx_des_base;
		else
			des++;
	}

	/* clear state */
	adapter->start = 0;

	/* free interrupt */
	free_irq(netdev->irq, netdev);

	ig_phy_write(netdev, adapter->phydev, MII_BMCR, BMCR_PDOWN);
	/* close GMAC power */
	ig_hw_close(adapter);
	return 0;
}

static int ig_start_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	uint32_t tmp;
	uint32_t entry;
	struct ig_dma_des *des;
	struct ig_adapter *adapter = netdev_priv(netdev);

	if (skb_is_nonlinear(skb) || (skb->len > (TBS1MASK>>TBS1SHIFT))) {
		dev_kfree_skb(skb);
		netdev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	tmp = adapter->tx_entry - adapter->current_tx_entry + 1;
	if (tmp >= MAX_TX_FRAME) {
		netif_stop_queue(netdev);
		if (tmp > MAX_TX_FRAME) {
			return NETDEV_TX_BUSY;
		}
	}

	/* set skb to DMA descriptor */
	entry = adapter->tx_entry & (RING_TX_LENGTH-1);
	des = adapter->tx_des_base + entry;

	/* count tx bytes */
	netdev->stats.tx_bytes += skb->len;

	/* fill in Tx descriptor */
	/* set data length */
	des->length = (((entry+1)&(RING_TX_LENGTH-1)) ? 0 : TER) |
	    TIC|TLS|TFS|TCIC3|TTSE|((skb->len << TBS1SHIFT) & TBS1MASK);
	des->data1 = dma_map_single(adapter->dev, skb->data, skb->len,
		DMA_TO_DEVICE);
	/* set owner to DMA */
	des->status = TOWN;
	adapter->tx_skb[entry].skb = skb;

	adapter->tx_entry++;

	wmb();

	/* clear TU|ETI status, it's auto set by dma */
	writel(TU|ETI, adapter->base_addr + DMASR);

	/* start DMA transmit poll */
	writel(0x01, adapter->base_addr + TPDR);
	netdev->trans_start = jiffies;

	return NETDEV_TX_OK;
}

static void ig_tx_timeout(struct net_device *netdev)
{
	uint32_t	tmp;
	struct ig_adapter *adapter = netdev_priv(netdev);

	INFO("Tx timeout\n");
	tmp = adapter->tx_entry - adapter->current_tx_entry + 1;
	if (tmp < MAX_TX_FRAME) {
		netif_wake_queue(adapter->netdev);
	} else {
		ig_stop(netdev);
		ig_open(netdev);
	}
}

static void set_hw_addr_to_mac_filter(struct netdev_hw_addr *ha, void *reg)
{
	int i;
	uint32_t mah, mal;
	uint32_t tmp[6];
	for(i = 0; i<6; i++)
		tmp[i] = ha->addr[i] & 0xFF;
	mah = MF_AE | (tmp[5]<<8) | tmp[4];
	mal = (tmp[3]<<24) | (tmp[2]<<16) | (tmp[1]<<8) | tmp[0];
	writel(mah, reg);
	writel(mal, reg+4);
}

static void ig_set_rx_mode(struct net_device *netdev)
{
	uint32_t mode;
	int index, i;
	struct netdev_hw_addr *ha;
	struct ig_adapter *adapter = netdev_priv(netdev);

	mode = readl(adapter->base_addr + MFF);
	if (netdev->flags & IFF_PROMISC) {
		/* Promiscuous Mode */
		mode = PR;
	} else if ((netdev_mc_count(netdev) > MA_MAX) ||
			(netdev->flags & IFF_ALLMULTI)){
		/* Pass All Multicast */
		mode = PM;
	} else {
		/* Perfect Filter */
		mode = 0;
		index = 1;
		netdev_for_each_mc_addr(ha, netdev) {
			set_hw_addr_to_mac_filter(ha,
					adapter->base_addr + MA_REG(index));

			index++;
		}
		for(i = index; i <= MA_MAX; i++) {
			/*reset unused mac*/
			writel(0x0000ffff, adapter->base_addr + MA_REG(index));
			writel(0xffffffff, adapter->base_addr + MA_REG(index) + 0x4);
		}
	}
	writel(mode, adapter->base_addr + MFF);
}

static int ig_ioctl(struct net_device *netdev, struct ifreq *req, int cmd)
{
	struct ig_adapter *adapter = netdev_priv(netdev);
	if (!netif_running(netdev))
		return -EINVAL;
	return generic_mii_ioctl(&adapter->mii, if_mii(req), cmd, NULL);
}

static int ig_change_mtu(struct net_device *netdev, int newmtu)
{
	if (newmtu < 68 || newmtu > ETH_DATA_LEN)
		return -EINVAL;
	netdev->mtu = newmtu;
	return 0;
}

static int ig_set_features(struct net_device *netdev,
		netdev_features_t features)
{
	DD("Request feature %016llx\n", features);
	return 0;
}

static int ig_set_mac_address(struct net_device *netdev, void *p)
{
	struct sockaddr *addr = (struct sockaddr *)p;
	struct ig_adapter *adapter = netdev_priv(netdev);
	if (netif_running(netdev))
		return -EBUSY;
	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;
	memcpy(netdev->dev_addr, addr->sa_data, 6);
	/* set mac addr to GMAC */
	ig_set_mac_addr(adapter);
	return 0;
}

static const struct net_device_ops imap_netdev_ops = {
	.ndo_open		= ig_open,
	.ndo_stop		= ig_stop,
	.ndo_start_xmit		= ig_start_xmit,
	.ndo_tx_timeout		= ig_tx_timeout,
	.ndo_set_rx_mode	= ig_set_rx_mode,
	.ndo_do_ioctl		= ig_ioctl,
	.ndo_change_mtu		= ig_change_mtu,
	.ndo_set_features	= ig_set_features,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_mac_address	= ig_set_mac_address,
	/*.ndo_get_stats = ig_get_stats,*/
};

static int ig_get_settings(struct net_device *netdev, struct ethtool_cmd *cmd)
{
	struct ig_adapter *adapter = netdev_priv(netdev);
	return mii_ethtool_gset(&adapter->mii, cmd);
}

static int ig_set_settings(struct net_device *netdev, struct ethtool_cmd *cmd)
{
	struct ig_adapter *adapter = netdev_priv(netdev);
	return mii_ethtool_sset(&adapter->mii, cmd);
}

static void ig_get_drvinfo(struct net_device *netdev,
		struct ethtool_drvinfo *info)
{
	strlcpy(info->driver, TAG, sizeof(info->driver));
	strlcpy(info->version, DRV_VER, sizeof(info->version));
	strlcpy(info->fw_version, FM_VER, sizeof(info->fw_version));
	strlcpy(info->bus_info, DRV_BUS, sizeof(info->bus_info));
}

static uint32_t ig_get_link(struct net_device *netdev)
{
	struct ig_adapter *adapter = netdev_priv(netdev);
	return mii_link_ok(&adapter->mii);
}

static const struct ethtool_ops imap_ethtool_ops = {
	.get_settings		= ig_get_settings,
	.set_settings		= ig_set_settings,
	.get_drvinfo		= ig_get_drvinfo,
	.get_link		= ig_get_link,
};

/*********************************************************/
/**                                                     **/
/**               NORMAL DRIVER INTERFACE               **/
/**                                                     **/
/*********************************************************/

static int ig_probe(struct platform_device *pdev)
{
	int err;
	struct ig_adapter *adapter;
	struct net_device *netdev;
	struct mii_if_info *mii;
	unsigned long base, size;
	struct eth_cfg *item_cfg = pdev->dev.platform_data;
	struct clk *clk;
	struct clk *oclk;
	struct clk *pclk;
	int level;

	/*
	 * <1>'rtl8201f' and 'ip101' phy need set reset-ping high
	 * in 'rtl8201f' named 'PHYRSTB'.
	 * <2>rtl8201f: if reset, pull down(up) 10ms,then pull up(down)
	 * */
	if(!strcmp(item_cfg->model, "rtl8201f") || !strcmp(item_cfg->model, "ip101")){
		if (item_cfg->reset_gpio != -1) {
			level = (item_cfg->reset_gpio < 0) ? 0 : 1;
			if (gpio_is_valid(item_cfg->reset_gpio)) {
				err = gpio_request(item_cfg->reset_gpio, "eth_reset");
				if (err) {
					ERR("[eth0] failed request gpio for eth_reset.\n");
					return err;
				}
				else{
					DD("[eth0] set phy_reset high\n");
					gpio_direction_output(item_cfg->reset_gpio, level);
					gpio_set_value(item_cfg->reset_gpio, !level);
					msleep(10);
					gpio_set_value(item_cfg->reset_gpio, level);
				}
			}else{
				DD("eth phy reset gpio is invalid\n");
				return -ENOMEM; 
			}
		}else{
			DD("eth phy reset gpio do not set yet, set it first\n");
			return -ENOMEM;
		}
	}

	/*
	 * judge if the sys or Crystal oscillator provide the phy clk
	 * 1: sys provide the clk
	 * 0: Crystal oscillator provide the clk
	 * */
	if(item_cfg->clk_provide){
#if defined(CONFIG_ARCH_Q3F) || defined(CONFIG_ARCH_APOLLO3)
		/*
		 * set the reference gpio to 'clkout1' function 
		 * */
		gpio_request(114, "oclk_pin");
		imapx_pad_set_mode(114, "function");
		oclk = clk_get_sys("imap-clk1", "imap-clk1");
		if (IS_ERR(oclk)) {
			ERR("[eth] Failed to get phy clk\n");
			return -ENOMEM;
		}
#else
		/*
		 * set the reference gpio to 'clkout0' function 
		 * */
		gpio_request(112, "oclk_pin");
		imapx_pad_set_mode(112, "function");
		oclk = clk_get_sys("imap-clk0", "imap-clk0");
		if (IS_ERR(oclk)) {
			ERR("[eth] Failed to get phy clk\n");
			return -ENOMEM;
		}
#endif
		pclk = clk_get_sys("vpll", "vpll");
		if (IS_ERR(pclk)) {
			ERR("[eth] Failed to get vpll clk\n");
			return -ENOMEM;
		}
		err = clk_set_parent(oclk, pclk);
		if(err){
			ERR("[eth] Failed to phy clk parent to vpll\n");
				return -ENOMEM;
		}
		err = clk_set_rate(oclk, 50000000);
		if (err < 0) {
			ERR("[eth] Failed to set phy clk to 50MHZ\n");
			return -ENOMEM;
		}
		err = clk_prepare_enable(oclk);
		if(err){
			ERR("[eth] Failed to prepare phy clk\n");
			goto clk_unprepare;
		}
	}

	netdev = alloc_etherdev(sizeof(struct ig_adapter));
	if (!netdev) {
		ERR("alloc netdev failed\n");
		err = -ENOMEM;
		goto err_alloc_etherdev;
	}

	SET_NETDEV_DEV(netdev, &pdev->dev);
	adapter = netdev_priv(netdev);
	memset(adapter, 0, sizeof(struct ig_adapter));
	adapter->netdev = netdev;
	init_mac_addr(adapter);
	adapter->phydev = DEFAULT_PHY_DEV;
	adapter->start = 0;
	adapter->dev = &pdev->dev;
#ifdef CONFIG_OMC300_ETH_PHY
	adapter->first_time_link_up = 0;
#endif

	mii = &adapter->mii;
	mii->dev = netdev;
	mii->phy_id = adapter->phydev;
	mii->phy_id_mask = 0x1F; /* 5bit */
	mii->reg_num_mask = 0x1F; /* 5bit */
	mii->force_media = 0;
	/*mii->full_duplex = 0;*/
#ifdef CONFIG_OMC300_ETH_PHY
	class_register(&eth_class);
	gadapter = adapter;
	mii->mdio_read = omc300_phy_inter_read;
	mii->mdio_write = omc300_phy_inter_write;
#else
	mii->mdio_read = ig_phy_read;
	mii->mdio_write = ig_phy_write;
#endif
	
	adapter->isolate = item_cfg ? item_cfg->isolate : -1;
	if (gpio_is_valid(adapter->isolate)) {
		err = gpio_request(adapter->isolate, "eth-isolate");
		if (err) {
			pr_err("failed request gpio for eth-isolate\n");
			goto err_request_gpio;
		}
	}
	clk = clk_get_sys("imap-mac",NULL);
	if (IS_ERR(clk)) {
		printk("get net clk err!\n");
		goto err_request_gpio;
	}
	err = clk_prepare_enable(clk);
	if(err){
		printk("prepare net clk err!\n");
		goto clk_unprepare;
	}
	base = pdev->resource[0].start;
	size = pdev->resource[0].end - pdev->resource[0].start;
	adapter->base_addr = ioremap(base, size);
	if (adapter->base_addr == NULL) {
		ERR("ioremap failed!\n");
		err = -EINVAL;
		goto err_ioremap;
	}
	netdev->base_addr = base;
	netdev->irq = pdev->resource[1].start;
	DD("base io addr %p, length 0x%08lx , irq %d, isolate %d\n",
		adapter->base_addr, size, netdev->irq, adapter->isolate);

	/* Check GMAC version and PHY chip */
	ig_hw_init(adapter, 0);
#ifndef CONFIG_OMC300_ETH_PHY
	if (ig_phy_id(adapter) == 0) {
		ERR("Can not read PHY ID, no PHY chip!\n");
		goto err_phy;
	}
	ig_phy_write(netdev, adapter->phydev, MII_BMCR, BMCR_PDOWN);
#endif
	ig_hw_close(adapter);

	netif_napi_add(netdev, &adapter->napi, ig_napi_poll, NAPI_WEIGHT);

	/* init a work queue for cable pluged state*/
	INIT_DELAYED_WORK(&adapter->work_queue, ig_cable_plug_check);

	/* request and map DMA memory */
	adapter->tx_des_base = dma_alloc_coherent(adapter->dev,
		RING_TX_LENGTH*sizeof(struct ig_dma_des), &adapter->tx_dma_base,
		GFP_KERNEL);
	adapter->rx_des_base = dma_alloc_coherent(adapter->dev,
		RING_RX_LENGTH*sizeof(struct ig_dma_des), &adapter->rx_dma_base,
		GFP_KERNEL);
	DD("DMA descriptor Tx: %p -> %08x,  Rx %p -> %08x\n",
			adapter->tx_des_base, adapter->tx_dma_base,
			adapter->rx_des_base, adapter->rx_dma_base);
	/* init DMA descriptor ring */
	ig_init_dma_tx_ring(adapter->tx_des_base, RING_TX_LENGTH);
	ig_init_dma_rx_ring(adapter, adapter->rx_des_base, RING_RX_LENGTH);

	netdev->netdev_ops = &imap_netdev_ops;
	netdev->ethtool_ops = &imap_ethtool_ops;
	netdev->watchdog_timeo = msecs_to_jiffies(timeout_tx);

	memcpy(netdev->dev_addr, adapter->mac_addr, 6);

	platform_set_drvdata(pdev, netdev);

	err = register_netdev(netdev);
	if (err) {
		ERR("register net dev failed %d\n", err);
		goto err_register;
	}

	DD("init imap gmac ethernet finished\n");

	return 0;
err_register:
	ig_free_rx_ring(adapter, adapter->rx_des_base, RING_RX_LENGTH);
	dma_free_coherent(adapter->dev,
		RING_RX_LENGTH*sizeof(struct ig_dma_des),
		adapter->rx_des_base, adapter->rx_dma_base);
	dma_free_coherent(adapter->dev,
		RING_TX_LENGTH*sizeof(struct ig_dma_des),
		adapter->tx_des_base, adapter->tx_dma_base);
	netif_napi_del(&adapter->napi);
clk_unprepare:
	clk_disable_unprepare(clk);
	clk_put(clk);
err_phy:
	iounmap(adapter->base_addr);
err_ioremap:
	free_netdev(netdev);
err_request_gpio:
	if (gpio_is_valid(adapter->isolate))
		gpio_free(adapter->isolate);
err_alloc_etherdev:
	return -EPERM;
}

static int ig_remove(struct platform_device *pdev)
{
	struct net_device *netdev = platform_get_drvdata(pdev);
	struct ig_adapter *adapter = netdev_priv(netdev);
	DD("Remove IMAP GMAC driver.\n");

	netif_napi_del(&adapter->napi);

	/* unregister driver */
	unregister_netdev(netdev);
	platform_set_drvdata(pdev, NULL);

	/* unmap io */
	iounmap(adapter->base_addr);

	/* free Rx buff */
	ig_free_rx_ring(adapter, adapter->rx_des_base, RING_RX_LENGTH);
	/* free Rx and Tx descriptor ring */
	dma_free_coherent(adapter->dev,
		RING_RX_LENGTH*sizeof(struct ig_dma_des),
		adapter->rx_des_base, adapter->rx_dma_base);
	dma_free_coherent(adapter->dev,
		RING_TX_LENGTH*sizeof(struct ig_dma_des),
		adapter->tx_des_base, adapter->tx_dma_base);

	free_netdev(netdev);
	return 0;
}

static int ig_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct net_device *netdev = platform_get_drvdata(pdev);
	struct ig_adapter *adapter = netdev_priv(netdev);

	netif_device_detach(netdev);

	if (adapter->start) {
		/* stop dev */
		ig_stop(netdev);
		/* keep state */
		adapter->start = 1;
	}
	return 0;
}

static int ig_resume(struct platform_device *pdev)
{
	struct net_device *netdev = platform_get_drvdata(pdev);
	struct ig_adapter *adapter = netdev_priv(netdev);

	netif_device_attach(netdev);

	if (adapter->start)
		ig_open(netdev);
	return 0;
}

static struct platform_driver ig_driver = {
	.driver = {
		.name   =   "imap-mac",
		.owner  =   THIS_MODULE,
	},
	.probe  =   ig_probe,
	.remove =   ig_remove,
#ifdef CONFIG_PM
	.suspend =   ig_suspend,
	.resume =   ig_resume,
#endif
};

static int __init ig_init(void)
{
	DD("Init infotm ethernet driver\n");
	return platform_driver_register(&ig_driver);
}

static void __exit ig_exit(void)
{
	platform_driver_unregister(&ig_driver);
}

module_init(ig_init);
module_exit(ig_exit);

MODULE_AUTHOR("xecle.zhang@infotmic.com.cn");
MODULE_DESCRIPTION("IMAP GMAC Network Driver");
MODULE_LICENSE("GPL");
