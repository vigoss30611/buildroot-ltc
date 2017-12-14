#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>
#include "pmu_regu.h"

extern uint8_t iicreg_write(uint8_t iicindex, uint8_t *subAddr, uint32_t addr_len, uint8_t *data, uint32_t data_num, uint8_t isstop);
extern uint8_t iicreg_read(uint8_t iicindex, uint8_t *data, uint32_t num);
extern void gpio_reset(void);

static const char *tps65910_regulator[7] = {
	"vdig1",
	"vdig2",
	"vaux1",
	"vaux2",
	"vaux33",
	"vmmc",
	"vdac"
};

static const char *axp152_regulator[6] = {
	"dcdc4",
	"aldo1",
	"aldo2",
	"dldo1",
	"dldo2",
	"ldo0"
};

static const char *axp202_regulator[3] = {
	"ldo3",
	"ldo4",
	"ldoio0"
};

static int regulator_check(const char *str)
{
	int i;

	if(item_exist("pmu.model")){
		if(item_equal("pmu.model", "tps65910", 0)){
			for(i=0;i<7;i++){
				if(!strcmp(str, tps65910_regulator[i])){
					break;
				}
			}
			return (i>=7)?(-1):i;

		}else if(item_equal("pmu.model", "axp152", 0)){
			for(i=0;i<6;i++){
				if(!strcmp(str, axp152_regulator[i])){
					break;
				}
			}
			return (i>=6)?(-1):i;

		}else if(item_equal("pmu.model", "axp202", 0)){
			for(i=0;i<3;i++){
				if(!strcmp(str, axp202_regulator[i])){
					break;
				}
			}
			return (i>=3)?(-1):i;

		}else
			return -1;
	}else
		return -1;
}

static int tps65910_regulator_en(const char *str, int en)
{
	uint8_t addr, val;
	int index;
    int ret;

	index = regulator_check(str);
	if(index == -1)
		return -1;

	switch(index){
		case 0:
			addr = TPS65910_VDIG1;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x1;
//			}else{
//				val &= ~(0x1);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 1:
			addr = TPS65910_VDIG2;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x1;
//			}else{
//				val &= ~(0x1);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 2:
			addr = TPS65910_VAUX1;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x1;
//			}else{
//				val &= ~(0x1);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 3:
			addr = TPS65910_VAUX2;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x1;
//			}else{
//				val &= ~(0x1);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 4:
			addr = TPS65910_VAUX33;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x1;
//			}else{
//				val &= ~(0x1);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 5:
			addr = TPS65910_VMMC;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x1;
//			}else{
//				val &= ~(0x1);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 6:
			addr = TPS65910_VDAC;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x1;
//			}else{
//				val &= ~(0x1);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		default:
			return -1;
	}

__retry__:
    iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
    ret = iicreg_read(0, &val, 0x1);
    if(ret){
        if(en){
            val |= 0x1;
        }else{
            val &= ~(0x1);
        }
        iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
    }else{
        gpio_reset();
        iic_init(0, 1, 0x5a, 0, 1, 0);
        goto __retry__;
    }

	return 0;
}

static int axp152_regulator_en(const char *str, int en)
{
	uint8_t addr, val;
	int index, shift;
    int ret;

	index = regulator_check(str);
	if(index == -1)
		return -1;

	switch(index){
		case 0:
			addr = AXP152_POWER_CTL;
            shift = 4;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x10;
//			}else{
//				val &= ~(0x10);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 1:
			addr = AXP152_POWER_CTL;
            shift = 3;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x08;
//			}else{
//				val &= ~(0x08);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 2:
			addr = AXP152_POWER_CTL;
            shift = 2;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x04;
//			}else{
//				val &= ~(0x04);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 3:
			addr = AXP152_POWER_CTL;
            shift = 1;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x02;
//			}else{
//				val &= ~(0x02);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 4:
			addr = AXP152_POWER_CTL;
            shift = 0;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x01;
//			}else{
//				val &= ~(0x01);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 5:
			addr = AXP152_LDO0;
            shift = 7;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x80;
//			}else{
//				val &= ~(0x80);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		default:
			return -1;
	}

__retry__:
    iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
    ret = iicreg_read(0, &val, 0x1);
    if(ret){
        if(en){
            val |= (1 << shift);
        }else{
            val &= ~(1 << shift);
        }
        iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
    }else{
        gpio_reset();
        iic_init(0, 1, 0x60, 0, 1, 0);
        goto __retry__;
    }

	return 0;
}

static int axp202_regulator_en(const char *str, int en)
{
	uint8_t addr, val;
	int index;
    uint8_t mask, shift, init;
    int ret;

	index = regulator_check(str);
	if(index == -1)
		return -1;

	switch(index){
		case 0:
			addr = AXP202_LDO234_DC23_CTL;
            mask = 0x40;
            init = 0x40;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x40;
//			}else{
//				val &= ~(0x40);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 1:
			addr = AXP202_LDO234_DC23_CTL;
            mask = 0x08;
            init = 0x08;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val |= 0x08;
//			}else{
//				val &= ~(0x08);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		case 2:
			addr = AXP202_GPIO0_CTL;
            mask = 0x07;
            init = 0x03;
//			iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
//			iicreg_read(0, &val, 0x1);
//			if(en){
//				val &= ~(0x7);
//				val |= 0x3;
//			}else{
//				val &= ~(0x7);
//			}
//			iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
			break;

		default:
			return -1;
	}

__retry__:
    iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
    ret = iicreg_read(0, &val, 0x1);
    if(ret){
        if(en){
            val &= ~mask;
            val |= init;
        }else{
            val &= ~mask;
        }
        iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
    }else{
        char str[64];
        item_string(str, "pmu.mode", 0);

        gpio_reset();
        if(strcmp(str, "mode2") == 0){
            gpio_pull_en(0, 54);
        }
        iic_init(0, 1, 0x68, 0, 1, 0);

        goto __retry__;
    }

	return 0;
}

static int tps65910_regulator_set_voltage(const char *str, int vol_mv)
{
	const uint16_t *table;
	uint8_t addr, val;
	int table_len, i, index;
	int bingo = 0;
    int ret;

	index = regulator_check(str);
	if(index == -1)
		return -1;

	switch(index){
		case 0:
			table = VDIG1_VSEL_table;
			table_len = 4;
			addr = TPS65910_VDIG1;
			break;

		case 1:
			table = VDIG2_VSEL_table;
			table_len = 4;
			addr = TPS65910_VDIG2;
			break;

		case 2:
			table = VAUX1_VSEL_table;
			table_len = 4;
			addr = TPS65910_VAUX1;
			break;

		case 3:
			table = VAUX2_VSEL_table;
			table_len = 4;
			addr = TPS65910_VAUX2;
			break;

		case 4:
			table = VAUX33_VSEL_table;
			table_len = 4;
			addr = TPS65910_VAUX33;
			break;

		case 5:
			table = VMMC_VSEL_table;
			table_len = 4;
			addr = TPS65910_VMMC;
			break;

		case 6:
			table = VDAC_VSEL_table;
			table_len = 4;
			addr = TPS65910_VDAC;
			break;

		default:
			return -1;
	}

	if(vol_mv < table[0] || vol_mv > table[table_len-1])
		return -1;

	for(i=0;i<table_len;i++){
		if(table[i] == vol_mv){
			bingo = 1;
			break;
		}
	}

	if(!bingo)
		return -1;

__retry__:
	iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
	ret = iicreg_read(0, &val, 0x1);
    if(ret){
	    val &= 0xf3;
	    val |= (i<<2);
	    iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
    }else{
        gpio_reset();
        iic_init(0, 1, 0x5a, 0, 1, 0);
        goto __retry__;
    }

	return 1;
}

static int axp152_regulator_set_voltage(const char *str, int vol_mv)
{
	const uint16_t *table;
	uint8_t addr, val, mask;
	int table_len, i, shift, index;
	int bingo = 0;
    int ret;

	index = regulator_check(str);
	if(index == -1)
		return -1;

	switch(index){
		case 0:
			table = DCDC4_VSEL_table;
			table_len = 113;
			addr = AXP152_DCDC4;
			mask = 0x7f;
			shift = 0;
			break;

		case 1:
			table = ALDO1_VSEL_table;
			table_len = 16;
			addr = AXP152_ALDO;
			mask = 0xf0;
			shift = 4;
			break;

		case 2:
			table = ALDO2_VSEL_table;
			table_len = 16;
			addr = AXP152_ALDO;
			mask = 0x0f;
			shift = 0;
			break;

		case 3:
			table = DLDO1_VSEL_table;
			table_len = 29;
			addr = AXP152_DLDO1;
			mask = 0x1f;
			shift = 0;
			break;

		case 4:
			table = DLDO2_VSEL_table;
			table_len = 113;
			addr = AXP152_DLDO2;
			mask = 0x1f;
			shift = 0;
			break;

		case 5:
			table = LDO0_VSEL_table;
			table_len = 4;
			addr = AXP152_LDO0;
			mask = 0x30;
			shift = 4;
			break;

		default:
			return -1;
	}

	if(vol_mv < table[0] || vol_mv > table[table_len-1])
		return -1;

	for(i=0;i<table_len;i++){
		if(table[i] == vol_mv){
			bingo = 1;
			break;
		}
	}

	if(!bingo)
		return -1;

__retry__:
	iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
	ret = iicreg_read(0, &val, 0x1);
    if(ret){
	val &= ~mask;
	val |= (i<<shift);
	iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
    }else{
        gpio_reset();
        iic_init(0, 1, 0x60, 0, 1, 0);
        goto __retry__;
    }

	return 0;
}

static int axp202_regulator_set_voltage(const char *str, int vol_mv)
{
	const uint16_t *table;
	uint8_t addr, val, mask;
	int table_len, i, shift, index;
	int bingo = 0;
    int ret;

	index = regulator_check(str);
	if(index == -1)
		return -1;

	switch(index){
		case 0:
			table = AXP202_LDO3_VSEL_table;
			table_len = 128;
			addr = AXP202_LDO3OUT_VOL;
			mask = 0x7f;
			shift = 0;
			break;

		case 1:
			table = AXP202_LDO4_VSEL_table;
			table_len = 16;
			addr = AXP202_LDO24OUT_VOL;
			mask = 0x0f;
			shift = 0;
			break;

		case 2:
			table = AXP202_LDOIO0_VSEL_table;
			table_len = 16;
			addr = AXP202_GPIO0_VOL;
			mask = 0xf0;
			shift = 4;
			break;

		default:
			return -1;
	}

	if(vol_mv < table[0] || vol_mv > table[table_len-1])
		return -1;

	for(i=0;i<table_len;i++){
		if(table[i] == vol_mv){
			bingo = 1;
			break;
		}
	}

	if(!bingo)
		return -1;

__retry__:
	iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
	ret = iicreg_read(0, &val, 0x1);
    if(ret){
        val &= ~mask;
        val |= (i<<shift);
        iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);
    }else{
        char str[64];
        item_string(str, "pmu.mode", 0);

        gpio_reset();
        if(strcmp(str, "mode2") == 0){
            gpio_pull_en(0, 54);
        }
        iic_init(0, 1, 0x68, 0, 1, 0);

        goto __retry__;
    }

	return 0;
}

int pmu_regulator_en(const char *str, int en)
{
	if(item_exist("pmu.model")){
		if(item_equal("pmu.model", "tps65910", 0)){
			return tps65910_regulator_en(str, en);
		}else if(item_equal("pmu.model", "axp152", 0)){
			return axp152_regulator_en(str, en);
		}else if(item_equal("pmu.model", "axp202", 0)){
			return axp202_regulator_en(str, en);
		}else
			return -1;
	}else
		return -1;
}

int pmu_regulator_set_voltage(const char *str, int vol_mv)
{
	if(item_exist("pmu.model")){
		if(item_equal("pmu.model", "tps65910", 0)){
			return tps65910_regulator_set_voltage(str, vol_mv);
		}else if(item_equal("pmu.model", "axp152", 0)){
			return axp152_regulator_set_voltage(str, vol_mv);
		}else if(item_equal("pmu.model", "axp202", 0)){
			return axp202_regulator_set_voltage(str, vol_mv);
		}else
			return -1;
	}else
		return -1;
}
