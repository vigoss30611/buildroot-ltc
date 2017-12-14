/***************************************************************************** 
** infotm/drivers/pmu/pmu_axp202.c 
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Description: File used for the standard function interface of output.
**
** Author:
**     Lei Zhang   <jack.zhang@infotmic.com.cn>
**      
** Revision History: 
** ----------------- 
** 1.0  11/27/2012  Jcak Zhang   
*****************************************************************************/ 

#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>
#include "pmu_axp202.h"

#define AXP202_BAT_DBG
#ifdef AXP202_BAT_DBG
#define AXP202_LOG   printf
#else
#define AXP202_LOG
#endif

#define AXP202_MODE0 0
#define AXP202_MODE1 1
#define AXP202_MODE2 2
#define AXP202_MODE3 3
#define AXP202_MODE4 4
#define AXP202_MODE5 5
#define AXP202_MODE_NULL 6

#define AXP_IX_CPUID_820_0		0x3c00b02c
#define AXP_IX_CPUID_820_1		0x3c00b030
#define AXP_IX_CPUID_X15		0x3c00aca0

#define AXP202_X15_DRVBUS 54

extern void gpio_reset(void);
extern void gpio_mode_set(int, int);
extern void gpio_dir_set(int, int);
extern void gpio_output_set(int, int);
extern void gpio_pull_en(int, int);

extern uint8_t iicreg_write(uint8_t iicindex, uint8_t *subAddr, uint32_t addr_len, uint8_t *data, uint32_t data_num, uint8_t isstop);
extern uint8_t iicreg_read(uint8_t iicindex, uint8_t *data, uint32_t num);

extern int axp202_get_voltage(void);

/* 0 - error */
int axp202_init_ready(int board)
{
	/* imapx15 */
	uint8_t buf[2], addr;
	__u32 tn, to, tst;
	__u32 val;

	/* output 1 in pin COND18*/
	gpio_reset();

	if(item_exist("board.arch") && item_equal("board.arch", "a5pv20", 0))
	{
	    if (iic_init(0, 1, 0x68, 0, 1, 0) == 0) {
		AXP202_LOG("[AXP202]: iic_init() fail!!!\n");
		goto err;
	    }
	    AXP202_LOG("[AXP202]: iic_init() success!!!\n");

	    /* set gpio1 as output mode, and output 1 to enable ctp power supply */
	    addr = 0x83;
	    buf[0] = 0x80;
	    iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
	    addr = 0x92;
	    buf[0] = 0x01;
	    iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
	}
	else
	{
	    if(board == AXP202_MODE2)
	    {
		gpio_pull_en(0, AXP202_X15_DRVBUS);
	    }

	    /* init iic */
	    if((board == AXP202_MODE3)||(board == AXP202_MODE4)||(board == AXP202_MODE5))
	    {
		if(iic_init(0, 1, 0x68, 0, 1, 0) == 0)
		{
		    AXP202_LOG("[AXP202]: iic_init() fail!!!\n");
		    goto err;
		}
		AXP202_LOG("[AXP202]: iic_init() success!!!\n");
	    }
	}
	
	return 1;
err:
	return 0;
}

void axp202_supply_input_mode(int mode)
{
	uint8_t addr, buf[2], pin;
	int cpu_id;

	cpu_id = readl(0x04000000 + 0x48);
	if(cpu_id == AXP_IX_CPUID_X15)
	{
		pin = 61;
	}
	else
	{
		pin = 78;
		printf("cpu is not x15, set pin num 78\n");
	}
	
	if(mode == 0)// 100mA
	{
		addr = 0x30;
		buf[0] = 0x82;
		//iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
		AXP202_LOG("set vbus_in max current 100mA\n");
	}
	else if(mode == 1)// 500mA
	{
		addr = 0x30;
		buf[0] = 0x81;
		//iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
		AXP202_LOG("set vbus_in max current 500mA\n");
	}
	else if(mode == 2)// 900mA
	{
		addr = 0x30;
		buf[0] = 0x80;
		//iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
		AXP202_LOG("set vbus_in max current 900mA\n");
	}
	else if(mode == 3)// no limit
	{
		addr = 0x30;
		buf[0] = 0x83;
		//iicreg_write(0, &addr, 0x01, buf, 0x01, 0x01);
		AXP202_LOG("set vbus_in no limit\n");
	}
	else// close
	{
		gpio_mode_set(1, pin);
		gpio_dir_set(1, pin);
		gpio_output_set(1, pin);
		AXP202_LOG("set vbus_in close\n");
	}
}

int axp202_get_voltage(void)
{
	/* 1.1mV/step */
	uint8_t addr, high, low;
	int adc;
	
	addr = 0x78;
	iicreg_write(0, &addr, 0x01, &high, 0x00, 0x01);
	iicreg_read(0, &high, 0x1);

	addr = 0x79;
	iicreg_write(0, &addr, 0x01, &low, 0x00, 0x01);
	iicreg_read(0, &low, 0x1);
	low &= 0x0f;

	adc = ((high << 4)|low);
	adc = adc * 11 / 10;
	AXP202_LOG("axp202 get voltage is %d\n", adc);

	return adc;
}

static void axp202_read(uint8_t reg, uint8_t *val)
{
    uint8_t addr = reg;

    iicreg_write(0, &addr, 0x01, val, 0x00, 0x01);
    iicreg_read(0, val, 0x1);

    return;
}

static void axp202_write(uint8_t reg, uint8_t val)
{
    uint8_t addr = reg;
    uint8_t reg_val = val;

    iicreg_write(0, &addr, 0x01, &reg_val, 0x01, 0x01);

    return;
}

void axp202_clear_data_buffer(void)
{
    int i;

    for (i = 0x04; i <= 0x0f; i++)
	axp202_write(i, 0x0);

    return;
}

int axp202_mode_ext(int board)
{
    if (board == AXP202_MODE2) {
	gpio_pull_en(0, AXP202_X15_DRVBUS);
    }

    return 0;
}

/* power on source, 0: onkey,  1:charger */
int axp202_get_boot_reason(void)
{
    uint8_t addr, reg = 0;

    addr = PMU_AXP202_POWER_SUPPLY_ST;
    iicreg_write(0, &addr, 0x01, &reg, 0x00, 0x01);
    iicreg_read(0, &reg, 0x1);
    AXP202_LOG("axp202 reg, offset 0x%02x value is 0x%02x\n", addr, reg);

    return (reg & AXP202_POWER_SUPPLY_IS_ACIN_VBUS);
}

int axp202_get_pwrkey(void)
{
    uint8_t addr, reg = 0;

    addr = 0x4a;
    iicreg_write(0, &addr, 0x01, &reg, 0x00, 0x01);
    iicreg_read(0, &reg, 0x1);
    AXP202_LOG("axp202 reg, offset 0x%02x value is 0x%02x\n", addr, reg);

    return (reg & 0x1);
}

void axp202_charger_mask(void)
{
    uint8_t addr, reg = 0;

    addr = 0x33;
    iicreg_write(0, &addr, 0x01, &reg, 0x00, 0x01);
    iicreg_read(0, &reg, 0x1);
    reg &= ~0x80;
    iicreg_write(0, &addr, 0x01, &reg, 0x01, 0x01);
    AXP202_LOG("axp202 reg, offset 0x%02x value is 0x%02x\n", addr, reg);

    return;
}


void axp202_charger_unmask()
{
    uint8_t addr, reg = 0;

    addr = 0x33;
    iicreg_write(0, &addr, 0x01, &reg, 0x00, 0x01);
    iicreg_read(0, &reg, 0x1);
    reg |= 0x80;
    iicreg_write(0, &addr, 0x01, &reg, 0x01, 0x01);
    AXP202_LOG("axp202 reg, offset 0x%02x value is 0x%02x\n", addr, reg);

    return;
}

int axp202_get_charger()
{
    uint8_t addr, reg = 0;

    addr = PMU_AXP202_POWER_SUPPLY_ST;
    iicreg_write(0, &addr, 0x01, &reg, 0x00, 0x01);
    iicreg_read(0, &reg, 0x1);
    AXP202_LOG("axp202 reg, offset 0x%02x value is 0x%02x\n", addr, reg);

    return ((reg & AXP202_POWER_SUPPLY_ACIN_EXIST
		|| reg & AXP202_POWER_SUPPLY_VBUS_EXIST) ? 1 : 0);
}

void axp202_shutdown_system()
{
    uint8_t addr, reg;

    /* record status of shutdown, distinguish between shutdown and sleep */
    addr = 0xf;
    reg = 0;
    iicreg_write(0, &addr, 0x01, &reg, 0x01, 0x01);

    /* shut down system */
    addr = 0x32;
    iicreg_write(0, &addr, 0x01, &reg, 0x00, 0x01);
    iicreg_read(0, &reg, 0x1);
    reg |= 0x1 << 2;
    iicreg_write(0, &addr, 0x01, &reg, 0x01, 0x01);
    reg |= 0x1 << 7;
    iicreg_write(0, &addr, 0x01, &reg, 0x01, 0x01);

    return;
}

struct axp202_regulator {
	char    name[16];
	uint8_t vol_reg;
	uint8_t en_reg;
	int     en_bit;
	uint8_t mask;
	int     shift;
	int     min_mv;
	int     max_mv;
	int     step_mv;
	int	*vol_avail;
};

static int ldo4_vsel[16] = {
	1250, 1300, 1400, 1500, 1600, 1700, 1800, 1900,
	2000, 2500, 2700, 2800, 3000, 3100, 3200, 3300,
};

static struct axp202_regulator axp202_regulator[] = {
	{"dc2", 0x23, 0x12, 4, 0x3f, 0, 700, 2275, 25, NULL},
	{"dc3", 0x27, 0x12, 1, 0x7f, 0, 700, 3500, 25, NULL},
	{"ldo2", 0x28, 0x12, 2, 0xf0, 4, 1800, 3300, 100, NULL},
	{"ldo3", 0x29, 0x12, 6, 0x7f, 0, 700, 2275, 25, NULL},
	{"ldo4", 0x28, 0x12, 3, 0x0f, 0, 1250, 3300, -1, &ldo4_vsel},
	{"gpio0", 0x91, 0x90, 2, 0xf0, 4, 1800, 3300, 100, NULL},
};

static void ldo4_vsel_show(void)
{
	printf("| <note> ldo4 available voltage is as follow:\n");
	printf("|\n");
	printf("|\t1250, 1300, 1400, 1500, 1600, 1700, 1800, 1900,\n");
	printf("|\t2000, 2500, 2700, 2800, 3000, 3100, 3200, 3300.\n");
}

static struct axp202_regulator *find_regulator(char *s)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(axp202_regulator); i++) {
		if (!strcmp(s, &axp202_regulator[i].name))
			return &axp202_regulator[i];
	}

	return NULL;
}

int axp202_set(char *s, int mv)
{
	uint8_t tmp, vsel;
	struct axp202_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("axp202 has no regulator named %s\n", s);
		return -1;
	}

	if (mv > p->max_mv || mv < p->min_mv) {
		printf("<%dmv> out range: %s<%dmv, %dmv>\n",
				mv, s, p->min_mv, p->max_mv);
		return -1;
	}

	if (p->step_mv > 0) {
		vsel = (mv - p->min_mv) / p->step_mv;
	} else {
		for (vsel = 0; vsel < ARRAY_SIZE(ldo4_vsel); vsel++) {
			if (mv == p->vol_avail[vsel])
				break;
		}

		if (vsel >= ARRAY_SIZE(ldo4_vsel)) {
			printf("error: please check available vol for ldo4\n");
			return -1;
		}
	}

	axp202_read(p->vol_reg, &tmp);
	tmp &= ~(p->mask);
	tmp |= vsel << p->shift;
	axp202_write(p->vol_reg, tmp);

	return 0;
}

int axp202_get(char *s)
{
	int tmp;
	uint8_t vsel;
	struct axp202_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("axp202 has no regulator named %s\n", s);
		return -1;
	}

	axp202_read(p->vol_reg, &vsel);
	vsel = (vsel & p->mask) >> p->shift;

	if (p->step_mv > 0)
		tmp = p->min_mv + vsel * p->step_mv;
	else
		tmp = p->vol_avail[vsel];

	return tmp;
}

int axp202_enable(char *s)
{
	uint8_t tmp;
	struct axp202_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("axp202 has no regulator named %s\n", s);
		return -1;
	}

	axp202_read(p->en_reg, &tmp);
	if (strncmp(p->name, "gpio0", 6)) {
		tmp |= 1 << p->en_bit;
	} else {
		tmp &= ~0x7;
		tmp |= 0x3;
	}
	axp202_write(p->en_reg, tmp);

	return 0;
}

int axp202_disable(char *s)
{
	uint8_t tmp;
	struct axp202_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("axp202 has no regulator named %s\n", s);
		return -1;
	}

	axp202_read(p->en_reg, &tmp);
	if (strncmp(p->name, "gpio0", 6)) {
		tmp &= ~(1 << p->en_bit);
	} else {
		tmp |= 0x7;
	}
	axp202_write(p->en_reg, tmp);

	return 0;
}

int axp202_is_enabled(char *s)
{
	uint8_t tmp;
	struct axp202_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("axp202 has no regulator named %s\n", s);
		return -1;
	}

	axp202_read(p->en_reg, &tmp);
	if (strncmp(p->name, "gpio0", 6))
		tmp = (tmp & (1 << p->en_bit)) ? 1: 0;
	else
		tmp = ((tmp & 0x7) == 0x3) ? 1: 0;

	return tmp;
}

int axp202_cmd_write(uint8_t reg, uint8_t buf)
{
	axp202_write(reg, buf);
	return 0;
}

int axp202_cmd_read(uint8_t reg)
{
	uint8_t tmp;

	axp202_read(reg, &tmp);
	return tmp;
}

int axp202_help(void)
{
	printf("\n");
	printf("|\t\t<Regulator Info>\t\t\n");
	printf("|\n");
	printf("| Name\tUsed\tMin/mv\tMax/mv\tStep/mv\n");
	printf("| dc2\tddr\t700\t2275\t25\n");
	printf("| dc3\tio33\t700\t3500\t25\n");
	printf("| ldo2\tpll\t1800\t3300\t100\n");
	printf("| ldo3\tcam28\t700\t2275\t25\n");
	printf("| ldo4\tcam18\t1250\t3300\t<note>\n");
	printf("| gpio0\taudio\t1800\t3300\t100\n");
	printf("\n");
	ldo4_vsel_show();
	printf("\n");
	printf("|\t\t<Usage>\t\t\n");
	printf("|\n");
	printf("| pmu set [name] [voltage/mv]\n");
	printf("| pmu get [name]\n");
	printf("| pmu enable [name]\n");
	printf("| pmu disable [name]\n");
	printf("| pmu status [name]\n");
	printf("| pmu help, printf help info\n");
	printf("\n");

	return 0;
}
