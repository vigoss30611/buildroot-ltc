#include <common.h>
#include <asm/io.h>
#include <div64.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>
#include <board.h>

#define POWER20_BAT_AVERVOL_H8                  (0x78)
#define POWER20_BAT_AVERVOL_L4                  (0x79)
#define POWER20_BAT_AVERCHGCUR_H8               (0x7A)
#define POWER20_BAT_AVERCHGCUR_L5               (0x7B)
#define POWER20_BAT_AVERDISCHGCUR_H8            (0x7C)
#define POWER20_BAT_AVERDISCHGCUR_L5            (0x7D)
#define POWER20_BAT_CHGCOULOMB3                 (0xB0)
#define POWER20_BAT_CHGCOULOMB2                 (0xB1)
#define POWER20_BAT_CHGCOULOMB1                 (0xB2)
#define POWER20_BAT_CHGCOULOMB0                 (0xB3)
#define POWER20_BAT_DISCHGCOULOMB3              (0xB4)
#define POWER20_BAT_DISCHGCOULOMB2              (0xB5)
#define POWER20_BAT_DISCHGCOULOMB1              (0xB6)
#define POWER20_BAT_DISCHGCOULOMB0              (0xB7)
#define AXP_OCV_BUFFER0                         (0xBC)
#define AXP_OCV_BUFFER1                         (0xBD)

#define ABS(x)									((x) > 0 ? (x) : (-x))

extern uint8_t iic_init(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
extern uint8_t iicreg_write(uint8_t, uint8_t *, uint32_t, uint8_t *, uint32_t, uint8_t);
extern uint8_t iicreg_read(uint8_t, uint8_t *, uint32_t);
extern void gpio_reset(void);
extern void batt_shutdown(void);
extern void display_logo(int);

struct batt_info {
	int ocv;
	int coulomb;
	int coulomb_e;
	int energy;
	int offset;
	int current_direction;
	int cap;

	int count;
};

struct charger {
	int vbat;
	int ibat;
	int ocv;
};

static int ocv[16] = {3132, 3273, 3414, 3555, 3625, 3660, 3696, 3731, 
					  3766, 3801, 3836, 3872, 3942, 4012, 4083, 4153};
static int batt_ocv[16];

static int batt_rdc, chg_total, chg_visible, dischg_total, dischg_visible;

static void open_lcd(void)
{
	uint8_t addr, temp;

	if(item_exist("board.arch") && item_equal("board.arch", "a5pv20", 0))
	{
		addr = 0x90;
		temp = 0x3;
		iicreg_write(0, &addr, 0x1, &temp, 0x1, 0x1);

		addr = 0x91;
		temp = 0xf5;
		iicreg_write(0, &addr, 0x1, &temp, 0x1, 0x1);
	}

	display_logo(-1);
}

static void info_struct_init(struct batt_info *info, int index)
{
	int i;

	for(i = 0; i < index; i++)
	{
		info[i].ocv = ocv[i];
		info[i].coulomb		= 0;
		info[i].coulomb_e	= 0;
		info[i].energy		= 0;
		info[i].offset		= 0;
		info[i].cap			= 0;
	}

	return;
}

static void log_result(struct batt_info *info, int index)
{
	int i;

	printf("index\t\tocv\t\tc\t\tc_e\t\toff\t\tcap\n");
	
	for(i = 0; i < index; i++)
	{
		printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", 
				i, info[i].ocv, info[i].coulomb, info[i].coulomb_e, info[i].offset, info[i].cap);
	}

	return;
}

static int get_adc_freq(void)
{
	int ret = 25;
	uint8_t addr, temp;

	addr = 0x84;
	iicreg_write(0, &addr, 0x1, &temp, 0x0, 0x0);
	iicreg_read(0, &temp, 0x1);
	temp &= 0xc0;

	switch(temp >> 6){
		case 0: ret = 25; break;
		case 1: ret = 50; break;
		case 2: ret = 100;break;
		case 3: ret = 200;break;
		default:break;
	}

	return ret;
}

static int vbat_to_mV(uint16_t val)
{
	return (val) * 1100 / 1000;
}

static int ibat_to_mA(uint16_t val)
{
	return (val) * 500 / 1000;
}

static void set_vbus_limit(int limit)
{
	uint8_t addr, val;
	uint8_t tmp;

	switch(limit)
	{
		case 900:tmp = 0;break;
		case 500:tmp = 1;break;
		case 100:tmp = 2;break;
		case 0:tmp = 3;break;
		default:tmp = 0;break;
	}

	addr = 0x30;
	iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
	iicreg_read(0, &val, 0x1);
	val &= 0xfc;
	val |= tmp;
	iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);

	return;
}

static void set_charging_current(int cur)
{
	uint8_t addr, val;
	uint8_t tmp;

	tmp = (cur-300)/100;

	addr = 0x33;
	iicreg_write(0, &addr, 0x1, &val, 0x0, 0x0);
	iicreg_read(0, &val, 0x1);
	val &= 0xf0;
	val |= tmp;
	iicreg_write(0, &addr, 0x1, &val, 0x1, 0x1);

	return;
}

static int get_batt_ocv(void)
{
	uint8_t addr;
	uint8_t v[2];
	int ocv;

	addr = AXP_OCV_BUFFER0;
	iicreg_write(0, &addr, 0x1, &v[0], 0x0, 0x0);
	iicreg_read(0, &v[0], 0x1);
	addr = AXP_OCV_BUFFER1;
	iicreg_write(0, &addr, 0x1, &v[1], 0x0, 0x0);
	iicreg_read(0, &v[1], 0x1);
	ocv = ((v[0] << 4) + (v[1] & 0x0f)) * 11 / 10;

	return ocv;
}

static int get_batt_coulomb(void)
{
	uint8_t addr;
	uint8_t v[8];
	int64_t rValue1, rValue2, rValue;
	int coulomb, m;

	addr = POWER20_BAT_CHGCOULOMB3;
	iicreg_write(0, &addr, 0x1, &v[0], 0x0, 0x0);
	iicreg_read(0, &v[0], 0x1);
	addr = POWER20_BAT_CHGCOULOMB2;
	iicreg_write(0, &addr, 0x1, &v[1], 0x0, 0x0);
	iicreg_read(0, &v[1], 0x1);
	addr = POWER20_BAT_CHGCOULOMB1;
	iicreg_write(0, &addr, 0x1, &v[2], 0x0, 0x0);
	iicreg_read(0, &v[2], 0x1);
	addr = POWER20_BAT_CHGCOULOMB0;
	iicreg_write(0, &addr, 0x1, &v[3], 0x0, 0x0);
	iicreg_read(0, &v[3], 0x1);
	addr = POWER20_BAT_DISCHGCOULOMB3;
	iicreg_write(0, &addr, 0x1, &v[4], 0x0, 0x0);
	iicreg_read(0, &v[4], 0x1);
	addr = POWER20_BAT_DISCHGCOULOMB2;
	iicreg_write(0, &addr, 0x1, &v[5], 0x0, 0x0);
	iicreg_read(0, &v[5], 0x1);
	addr = POWER20_BAT_DISCHGCOULOMB1;
	iicreg_write(0, &addr, 0x1, &v[6], 0x0, 0x0);
	iicreg_read(0, &v[6], 0x1);
	addr = POWER20_BAT_DISCHGCOULOMB0;
	iicreg_write(0, &addr, 0x1, &v[7], 0x0, 0x0);
	iicreg_read(0, &v[7], 0x1);

	rValue1 = ((v[0] << 24) + (v[1] << 16) + (v[2] << 8) + v[3]);
	rValue2 = ((v[4] << 24) + (v[5] << 16) + (v[6] << 8) + v[7]);
	rValue = (ABS(rValue1 - rValue2)) * 4369;

	m = get_adc_freq() * 480;
	do_div(rValue, m);
	if(rValue1 >= rValue2)
		coulomb = (int)rValue;
	else
		coulomb = (int)(0 - rValue);

	return coulomb;
}

static int get_current_direction(void)
{
	uint8_t addr, tmp;

	addr = 0x00;
	iicreg_write(0, &addr, 0x1, &tmp, 0x0, 0x0);
	iicreg_read(0, &tmp, 0x1);

	return (tmp & 0x04) ? 1 : 0;
}

static void charger_update(struct charger *charger)
{
	uint8_t tmp[6];
	uint8_t addr;
	uint16_t ichar_res, idischar_res, vbat_res;

	addr = POWER20_BAT_AVERVOL_H8;
	iicreg_write(0, &addr, 0x1, &tmp[0], 0x0, 0x0);
	iicreg_read(0, &tmp[0], 0x1);
	addr = POWER20_BAT_AVERVOL_L4;
	iicreg_write(0, &addr, 0x1, &tmp[1], 0x0, 0x0);
	iicreg_read(0, &tmp[1], 0x1);
	addr = POWER20_BAT_AVERCHGCUR_H8;
	iicreg_write(0, &addr, 0x1, &tmp[2], 0x0, 0x0);
	iicreg_read(0, &tmp[2], 0x1);
	addr = POWER20_BAT_AVERCHGCUR_L5;
	iicreg_write(0, &addr, 0x1, &tmp[3], 0x0, 0x0);
	iicreg_read(0, &tmp[3], 0x1);
	addr = POWER20_BAT_AVERDISCHGCUR_H8;
	iicreg_write(0, &addr, 0x1, &tmp[4], 0x0, 0x0);
	iicreg_read(0, &tmp[4], 0x1);
	addr = POWER20_BAT_AVERDISCHGCUR_L5;
	iicreg_write(0, &addr, 0x1, &tmp[5], 0x0, 0x0);
	iicreg_read(0, &tmp[5], 0x1);

	vbat_res = ((uint16_t) tmp[0] << 4 )| (tmp[1] & 0x0f);
	ichar_res = ((uint16_t) tmp[2] << 4 )| (tmp[3] & 0x0f);
	idischar_res = ((uint16_t) tmp[4] << 5 )| (tmp[5] & 0x1f);
	charger->vbat = vbat_to_mV(vbat_res);
	charger->ibat = ABS(ibat_to_mA(ichar_res) - ibat_to_mA(idischar_res));
	charger->ocv = get_batt_ocv();

	return;
}

static void info_update(struct batt_info *info)
{
	info->ocv = get_batt_ocv();
	info->coulomb = get_batt_coulomb();
	info->current_direction = get_current_direction();
}

static void calc_rdc(void)
{
	int rdc[5];
	int i;
	struct charger charger[2];
	printf("%s run!\n", __func__);

	set_vbus_limit(0);

	for(i = 0; i < 5; i++)
	{
		set_charging_current(500);
		udelay(1000000);
		charger_update(&charger[0]);

		set_charging_current(1500);
		udelay(1000000);
		charger_update(&charger[1]);

		rdc[i] = ((charger[1].vbat - charger[0].vbat) - (charger[1].ocv - charger[0].ocv)) * 1000 / (charger[1].ibat - charger[0].ibat);
		printf("\n");
		printf("v_l,	v_h,	i_l,	i_h,	rdc_%d\n", i);
		printf("%d,	%d,	%d,	%d,	%d\n", 
				charger[0].vbat, charger[1].vbat, charger[0].ibat, charger[1].ibat, rdc[i]);
	}
	printf("\n");
	batt_rdc = (rdc[0]+rdc[1]+rdc[2]+rdc[3]+rdc[4])/5;
	printf("RDC = [%d]\n", batt_rdc);
	printf("\n\n");
}

static void calc_chg_curve(void)
{
	int i, c, tick, cnt;
	struct batt_info chg_info[16];
	struct batt_info tmp_info;
	struct batt_info start;
	printf("%s run!\n", __func__);

	tick = 0;
	cnt = 0;
	chg_total = 0;
	chg_visible = 0;
	info_struct_init(chg_info, 16);
	info_update(&start);
	set_charging_current(1000);

	do{
		if((tick % 600) == 0 && tick > 0)
		{
			printf("about %d Minutes passed...\n", (tick*10/600));
		}

		info_update(&tmp_info);

		if(tmp_info.current_direction == 1)/*charging*/
		{
			for(i = 0; i < 16; i++)
			{
				if((tmp_info.ocv >= (chg_info[i].ocv - 4)) && (tmp_info.ocv <= (chg_info[i].ocv + 4)))
				{
					chg_info[i].coulomb = (tmp_info.coulomb < 0) ? 0 : tmp_info.coulomb;
					chg_info[i].energy = chg_info[i].ocv * chg_info[i].coulomb;
					chg_info[i].coulomb_e = chg_info[i].energy / 3700;
				}
			}
		}
		else
		{
			cnt++;
			if(cnt >= 300)
			{
				printf("charing calc over!\n");
				break;
			}
		}

		udelay(1000000);
		tick++;

	}while(1);

	chg_total = tmp_info.ocv * tmp_info.coulomb - start.ocv * start.coulomb;
	chg_visible = chg_info[15].coulomb_e - chg_info[3].coulomb_e;
	for(i = 3; i < 16; i++)
	{
		chg_info[i].offset = chg_info[i].coulomb_e - chg_info[3].coulomb_e;
		if(i == 3)
		{
			chg_info[i].cap = 0;
		}
		else if(i == 15)
		{
			chg_info[i].cap = 100;
		}
		else
		{
			chg_info[i].cap = chg_info[i].offset * 100 / chg_visible;
		}
	}

	for(i = 0; i < 16; i++)
	{
		batt_ocv[i] = chg_info[i].cap;
	}

	printf("\n");
	printf("================================ RESULT FOR CHARGE ===============================\n");
	log_result(chg_info, 16);
	printf("\n\n");
	//printf("Total charge energy : %d(mV * mAh) = %dmAh@3700mV\n", chg_total, (chg_total/3700));
	//printf("Energy visible : %dmAh@3700mV, percent:%d\n\n", chg_visible, (chg_visible*3700*100/chg_total));
}

static void calc_dischg_curve(void)
{
	int i, v, c, b, tick;
	struct batt_info dischg_info[16];
	struct batt_info tmp_info;
	struct batt_info start;
	printf("%s run!\n", __func__);

	tick = 0;
	dischg_total = 0;
	dischg_visible = 0;
	info_struct_init(dischg_info, 16);
	info_update(&start);

	for(i = 0; i < 16; i++)
	{
		if(start.ocv > dischg_info[15].ocv)
		{
			v = 15;
			break;
		}
		else if(start.ocv >= dischg_info[i].ocv && start.ocv <= dischg_info[i+1].ocv)
		{
			v = i;
			break;
		}
	}

	do{
		if((tick % 600) == 0 && tick > 0)
		{
			printf("about %d Minutes passed...\n", (tick*10/600));
		}

		info_update(&tmp_info);

		if(tmp_info.ocv <= 3350)
		{
			printf("dischaging calc over!\n");
			break;
		}

		if(tmp_info.current_direction == 0)/*discharging*/
		{
			for(i = 0; i < (v+1); i++)
			{
				if(tmp_info.ocv >= (dischg_info[i].ocv-4) && tmp_info.ocv <= (dischg_info[i].ocv+4))
				{
					dischg_info[i].coulomb = tmp_info.coulomb;
					dischg_info[i].energy = start.ocv * start.coulomb - dischg_info[i].ocv * dischg_info[i].coulomb;
					dischg_info[i].coulomb_e = dischg_info[i].energy / 3700;
				}
			}
		}

		udelay(1000000);
		tick++;

	}while(1);

	dischg_total = start.ocv * start.coulomb - tmp_info.ocv * tmp_info.coulomb;
	if(dischg_info[2].coulomb_e - dischg_info[3].coulomb_e > 400)
	{
		b = 2;
	}
	else
	{
		b = 3;
	}
	dischg_visible = dischg_info[b].coulomb_e - dischg_info[v].coulomb_e;

	for(i = 2; i < 16; i++)
	{
		if(i >= v)
		{
			dischg_info[i].cap = 100;
		}
		else if(i <= b)
		{
			dischg_info[i].cap = 0;
		}
		else
		{
			dischg_info[i].offset = dischg_info[i].coulomb_e - dischg_info[v].coulomb_e;
			dischg_info[i].cap = 100 - dischg_info[i].offset * 100 / dischg_visible;
		}
	}

	printf("\n");
	printf("============================== RESULT FOR DISCHARGE =============================\n");
	log_result(dischg_info, 16);
	printf("\n\n");
	//printf("Energy discharge visible : %dmAh@3700mAh\n", dischg_visible);
	//printf("Charging efficient:%d\n\n", dischg_visible/chg_visible);
}

void batt_calc(void)
{
	int c;
	int count;

	if(item_exist("pmu.model") && item_equal("pmu.model", "axp202", 0))
	{
		gpio_reset();
		iic_init(0, 1, 0x68, 0, 1, 0);

		printf("================================ NOTICE ================================\n"
				"Battery calibration will take several hours to finish. Before calibrate,\n"
				"please MAKE SURE each condition of the calibration is all achieved.\n"
				"'R' = [R]un calibration, \n"
				"'Q' = [Q]uit. Enter Your Choice:\n");

		for(;;)
		{
			c = getc();

			switch(c)
			{
				case 'R':
				case 'r':
					printf("\nYou choose [R]un calibration\n");
					goto run;

				case 'Q':
				case 'q':
					printf("\nYou choice is [Quit]\n");
					goto quit;

				default:
					continue;
			}
		}

run:
		if(c == 'R' || c == 'r')
		{
			count = 0;

			printf("\nFirst...");
			calc_rdc();

			printf("\nSecond...");
			while(board_acon() == 0);
			calc_chg_curve();

#if 0
			while(1)
			{
				if((count % 60) == 0)
					printf("[%d]Minute after charging calc, please unplug the adapter.\n", count/60);

				if(get_charger_exist() == 0)
					break;

				count++;
				udelay(1000000);
			}

			open_lcd();
			printf("\nThird...");
			calc_dischg_curve();
#endif

			printf("Battery Calibration Over!\n");
			printf("We recommend items config as follow:\n");
			printf("batt.rdc	%d\n", batt_rdc);
			printf("power.curve.ocv	%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d\n",
					batt_ocv[0], batt_ocv[1], batt_ocv[2], batt_ocv[3], batt_ocv[4], batt_ocv[5], 
					batt_ocv[6], batt_ocv[7], batt_ocv[8], batt_ocv[9], batt_ocv[10], batt_ocv[11], 
					batt_ocv[12], batt_ocv[13], batt_ocv[14], batt_ocv[15], batt_ocv[16]);

			goto quit;
		}
		else if(c == 'Q' || c == 'q')
		{
			printf("Quit Battery Calibration!\n");
			goto quit;
		}
	}
	else
	{
		printf("PMU isn't AXP202, quit!\n");
	}

quit:
	board_shut();
}
