/*
 * Copyright (C) 2007 Atmel Corporation
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>
//#include <asm/arch/clk.h>
//#include <asm/arch/memory-map.h>
struct atmel_spi_slave {
	 struct spi_slave slave;
	 void            *regs;
	 u32             mr;
	 };
 static inline struct atmel_spi_slave *to_atmel_spi(struct spi_slave *slave)
	 {
		  return container_of(slave, struct atmel_spi_slave, slave);
         }

//#include <imapx_spi.h>

#define SPI_BATTERY_READ         0xa5

#ifndef CONFIG_DEFAULT_SPI_BUS
#define CONFIG_DEFAULT_SPI_BUS 0
#endif

#ifndef CONFIG_DEFAULT_SPI_MODE
#define CONFIG_DEFAULT_SPI_MODE SPI_MODE_0
#endif

#define spi_readl(as, reg)                                      \
	readl(as->regs + reg)
#define spi_writel(as, reg, value)                              \
	writel(value, as->regs + reg)


int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs < 2;
}

void spi_cs_activate(struct spi_slave *slave)
{
	int tmp = 0;
	switch(slave->cs) {
		case 1:
			break;
		case 0:
		default:
			{
				tmp = readl(GPEDAT);
				tmp &= ~(1 << 7);
				writel(tmp , GPEDAT);
				break;
			}
	}
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	int tmp = 0 ;
	switch(slave->cs) {
		case 1:
			break;
		case 0:
		default:
			{
				tmp = readl(GPEDAT);
				tmp |= (1 << 7);
				writel(tmp , GPEDAT);
				break;
			}
	}
}

void spi_init()
{
	int gpecon,gpbcon;
	gpecon = readl(GPECON);
	gpecon &= ~((0x3 << 14) | (0x3 << 12) | (0x3 << 10) | (0x3 << 8));
	gpecon |= ((0x1 <<14) | (0x2 << 12) | (0x2 << 10) | (0x2 << 8));
	writel(gpecon,GPECON);

	gpbcon = readl(GPBCON);
	gpbcon &= ~(0x3 << 2);
	writel(gpbcon, GPBCON);

	writel(0, SSI_MST0_BASE_REG_PA+rSSI_ENR_M);
	writel(0, SSI_MST0_BASE_REG_PA+rSSI_IMR_M);
	writel(30, SSI_MST0_BASE_REG_PA+rSSI_BAUDR_M);
	writel(8, SSI_MST0_BASE_REG_PA+rSSI_TXFTLR_M);
	writel(0, SSI_MST0_BASE_REG_PA+rSSI_RXFTLR_M);
	writel(((0x0 << 8) | (0x0 << 7) | (0x0 << 6) | (0x0 << 4) | (0x7 << 0)), SSI_MST0_BASE_REG_PA+rSSI_CTLR0_M);
	writel(1, SSI_MST0_BASE_REG_PA+rSSI_SER_M);
	writel(1, SSI_MST0_BASE_REG_PA+rSSI_ENR_M);
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
			unsigned int max_hz, unsigned int mode)
{
	struct atmel_spi_slave	*as;
	void			*regs;

	if (cs > 3 || !spi_cs_is_valid(bus, cs))
		return NULL;

	switch (bus) {
	case 0:
		regs = (void *)SSI_MST0_BASE_REG_PA;
		break;
#ifdef SPI1_BASE
	case 1:
		regs = (void *)SSI_MST1_BASE_REG_PA;
		break;
#endif
#ifdef SPI2_BASE
	case 2:
		regs = (void *)SSI_MST2_BASE_REG_PA;
		break;
#endif

	default:
		return NULL;
	}
/**********************************************/
//here, we need to set the baud rate
//the mode parameters is useless, we set in the init function

/**********************************************/
	as = malloc(sizeof(struct atmel_spi_slave));
	if (!as)
		return NULL;

	as->slave.bus = bus;
	as->slave.cs = cs;
	as->regs = regs;

	return &as->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct atmel_spi_slave *as = to_atmel_spi(slave);

	free(as);
}

int spi_claim_bus(struct spi_slave *slave)
{
	/*
	struct atmel_spi_slave *as = to_atmel_spi(slave);

	spi_writel(as, CR, ATMEL_SPI_CR_SPIEN);

	spi_writel(as, MR, as->mr);
*/
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	/*
	struct atmel_spi_slave *as = to_atmel_spi(slave);

	spi_writel(as, CR, ATMEL_SPI_CR_SPIDIS);
	*/
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	//struct atmel_spi_slave *as = to_atmel_spi(slave);
	unsigned int	len;
	int		ret;
	u32		status;
	volatile const u8	*txp = dout;
	volatile u8		*rxp = din;
	u8		value;

	ret = 0;
	/*
	if (bitlen == 0)
		goto out;
*/
	len = bitlen / 8;


		if (txp) //send operation
			value = *txp;
		else
			value = 0;

		status = readl(SSI_MST0_BASE_REG_PA +  rSSI_SR_M);
		while (!(status & (1 << 2))) {   //tx fifo ready
			status = readl(SSI_MST0_BASE_REG_PA + rSSI_SR_M);
		}
		status = readl(SSI_MST0_BASE_REG_PA + rSSI_SR_M);//rx fifo not empty
		while (status & (1 << 3)) {
			*rxp= readl(SSI_MST0_BASE_REG_PA + rSSI_DR_M);
			status = readl(SSI_MST0_BASE_REG_PA + rSSI_SR_M);
		}

		writel(value , SSI_MST0_BASE_REG_PA + rSSI_DR_M);
		status = readl(SSI_MST0_BASE_REG_PA + rSSI_SR_M);
		while (!(status & (1 << 2))) {   //tx fifo ready
			status = readl(SSI_MST0_BASE_REG_PA + rSSI_SR_M);
		}

		status = readl(SSI_MST0_BASE_REG_PA + rSSI_RXFLR_M);//rx fifo entry
		while (status < 1) {
			status = readl(SSI_MST0_BASE_REG_PA + rSSI_RXFLR_M);
		}
		*rxp= readl(SSI_MST0_BASE_REG_PA + rSSI_DR_M);

	return 0;
}

ssize_t spi_read (uchar *addr, int alen, uchar *buffer, int len)
{
	struct spi_slave *slave;
	u8 cmd = SPI_BATTERY_READ;
	unsigned short tmp = 0;

	slave = spi_setup_slave(CONFIG_DEFAULT_SPI_BUS, 2, 1000000,0);

	spi_cs_activate(slave);

	if(spi_xfer(slave, 8, &cmd, buffer, SPI_XFER_BEGIN))
		return -1;

	if(spi_xfer(slave, 8, NULL, buffer, 0))
		return -1;
	tmp = (0x00ff & (unsigned short)*buffer) << 8; 

	if(spi_xfer(slave, 8, NULL, buffer, SPI_XFER_END))
		return -1;
	tmp |= (0x00ff & (unsigned short)*buffer);
	tmp &= 0x7fff;
	tmp = (tmp >> 3);

	printf("Battery: %d\n, AdapterIn: %d\n", tmp, ((readl(GPBDAT) >> 1) & 0x1));
/*
#ifdef CONFIG_POWER_GPIO
	if(!(readl(CONFIG_POWER_GPIO) & CONFIG_POWER_GPIO_NUM) )
	{
		if (tmp < CONFIG_LOW_BATTERY_VAL)
			udelay(1000000);
	}		power_off();
#endif
		if (readl(POW_STB) & 0x4)
		{
		//	printf("123456789\n");
			udelay(1000000);
			power_off();
		}

		if (tmp < CONFIG_LOW_BATTERY_VAL && tmp > CONFIG_LOGO_BATTERY_VAL) 
		{
			printf("aaaaaa\n");
			oem_clear_screen (0);
			oem_mid("WARNING , LOW POWER ...");
			printf("okkkkk123456789\n");
			udelay(5000000);
			printf("1234567890123456789\n");
			power_off();
		}
		else if (tmp < CONFIG_LOGO_BATTERY_VAL)
			{
				power_off();
			}
*/
	spi_cs_deactivate(slave);
	spi_free_slave(slave);
	return tmp;
}


/* Removed by warits, Dec 6, 2010 */
#if 0
void warning_print()
{	oem_fb_switch(1);
	oem_clear_screen (0);
	oem_mid("Low battery ...");
	lcd_win_alpha_set(1, 15);
	lcd_win_onoff(1, 1);
	//udelay(5000000);
}
#endif 

void power_off(void)
{
	writel(0x4 , SYSMGR_BASE_REG_PA + 0x208);
	writel(0xff, SYSMGR_BASE_REG_PA + 0x204);
	writel(0x3f, SYSMGR_BASE_REG_PA + 0x218);
	writel(0x0 , SYSMGR_BASE_REG_PA + 0x210);
	writel(0x4 , SYSMGR_BASE_REG_PA + 0x200);
}

ssize_t spi_write (uchar *addr, int alen, uchar *buffer, int len)
{
//	struct spi_slave *slave;
//	char buf[3];
/*
	slave = spi_setup_slave(CONFIG_DEFAULT_SPI_BUS, 1, 1000000,
			1);
	spi_claim_bus(slave);

	buf[0] = SPI_EEPROM_WREN;
	if(spi_xfer(slave, 8, buf, NULL, SPI_XFER_BEGIN | SPI_XFER_END))
		return -1;

	buf[0] = SPI_EEPROM_WRITE;

	if (alen == 3) {
		alen--;
		addr++;
	}

	memcpy(buf + 1, addr, alen);
	if(spi_xfer(slave, 24, buf, NULL, SPI_XFER_BEGIN))
		return -1;
	if(spi_xfer(slave, len * 8, buffer, NULL, SPI_XFER_END))
		return -1;	reset_timer_masked();
	do {
		buf[0] = SPI_EEPROM_RDSR;
		buf[1] = 0;
		spi_xfer(slave, 16, buf, buf, SPI_XFER_BEGIN | SPI_XFER_END);

		if (!(buf[1] & 1))
			break;

	} while (get_timer_masked() < CONFIG_SYS_SPI_WRITE_TOUT);

	if (buf[1] & 1)
		printf ("*** spi_write: Time out while writing!\n");

	spi_release_bus(slave);
	spi_free_slave(slave);
	return len;
	*/
	return 0;
}
