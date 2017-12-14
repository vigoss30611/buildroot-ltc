#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>
#include <imapx_iic.h>

struct ricoh618_regulator {
	char	name[16];
	uint8_t	vol_reg;
	uint8_t	en_reg;
	int	en_bit;
	uint8_t	mask;
	int	shift;
	uint32_t	min_uv;
	uint32_t	max_uv;
	uint32_t	step_uv;
};

static int ricoh618_write(uint8_t reg, uint8_t buf)
{
	iicreg_write(0, &reg, 0x01, &buf, 0x01, 0x01);

	return 0;
}

static int ricoh618_read(uint8_t reg)
{
	uint8_t buf = 0;

	iicreg_write(0, &reg, 0x01, &buf, 0x00, 0x00);
	iicreg_read(0, &buf, 0x1);

	return buf;
}

int ricoh618_cpu_set(uint16_t millivolt_10times)
{
    uint8_t addr, buf;
    int us = millivolt_10times * 100;

    if(us < 600000 || us > 1300000) {
	printf("cpu targer vol error!!!\n");
	return -1;
    }

    iic_init(0, 1, 0x64, 0, 1, 0);
    addr = 0x36;
    buf = (us - 600000)/12500;
    iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);
    printf("cpu vol set OK!\n");

    return 0;
}

int ricoh618_core_set(uint16_t millivolt_10times)
{
    uint8_t addr, buf;
    int us = millivolt_10times * 100;

    if(us < 600000 || us > 1300000) {
	printf("cpu targer vol error!!!\n");
	return -1;
    }

    iic_init(0, 1, 0x64, 0, 1, 0);
    addr = 0x37;
    buf = (us - 600000)/12500;
    iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);
    printf("core vol set OK!\n");

    return 0;
}

int ricoh618_init_ready(int board)
{
    uint8_t val;
    uint8_t addr, buf;

    /*ricoh618 setting*/

    /*set OFF_PRESS_PWRON 8s*/
    addr = 0x10;
    iicreg_write(0, &addr, 0x1, &buf, 0x0, 0x0);
    iicreg_read(0, &buf, 0x1);
    buf &= 0x8f;
    buf |= 0x50;
    iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);

    /*allow change to power off sequence by PWRON longpress during sleep state*/
    addr = 0x0D;
    iicreg_write(0, &addr, 0x1, &buf, 0x0, 0x0);
    iicreg_read(0, &buf, 0x1);
    buf |= 0x1 << 5;
    iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);

    /*set BATDET to 2.7V*/
    addr = 0xb1;
    buf = 0;
    iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);

    /*config ADC, enable VBAT A/D conversion*/
    addr = 0x64;
    buf = 0x2;
    iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);

    addr = 0x66;
    buf = 0x28;
    iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);

    //set cpu vol 1.05V & core vol 1.05V on X9_7.85inch
    if(item_exist("board.hwid") && item_equal("board.hwid", "X9785", 0)) {
		addr = 0x36;
		buf = 0x24;
		iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);
		printf("cpu vol set to 1.05V\n");

		addr = 0x37;
		buf = 0x24;
		iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);
		printf("core vol set to 1.05V\n");
    } else {
		addr = 0x36;
		buf = 0x2C;
		iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);
		printf("cpu vol set to 1.15V\n");

		addr = 0x37;
		buf = 0x2C;
		iicreg_write(0, &addr, 0x1, &buf, 0x1, 0x1);
		printf("core vol set to 1.15V\n");
    }

    return 0;
}

int ricoh618_battery_get_voltage(void)
{
    uint8_t addr, buf;
    uint8_t low, high;
    uint16_t data;
    int vol;

    addr = 0x6A;
    iicreg_write(0, &addr, 0x1, &high, 0x0, 0x0);
    iicreg_read(0, &high, 0x1);

    addr = 0x6B;
    iicreg_write(0, &addr, 0x1, &low, 0x0, 0x0);
    iicreg_read(0, &low, 0x1);

    data = (high<<4) | low;
    vol = data*2*2500/4095;

    return vol;
}

static struct ricoh618_regulator ricoh618_regulator[] = {
	{"dc1", 0x36, 0x2c, 0, 0xff, 0, 600000, 3500000, 12500},
	{"dc2", 0x37, 0x2e, 0, 0xff, 0, 600000, 3500000, 12500},
	{"dc3", 0x38, 0x30, 0, 0xff, 0, 600000, 3500000, 12500},
	{"ldo1", 0x4c, 0x44, 0, 0x7f, 0, 900000, 3500000, 25000},
	{"ldo2", 0x4d, 0x44, 1, 0x7f, 0, 900000, 3500000, 25000},
	{"ldo3", 0x4e, 0x44, 2, 0x7f, 0, 600000, 3500000, 25000},
	{"ldo4", 0x4f, 0x44, 3, 0x7f, 0, 900000, 3500000, 25000},
	{"ldo5", 0x50, 0x44, 4, 0x7f, 0, 900000, 3500000, 25000},
};

static struct ricoh618_regulator *find_regulator(char *s)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ricoh618_regulator); i++) {
		if (!strcmp(s, &ricoh618_regulator[i].name))
			return &ricoh618_regulator[i];
	}

	return NULL;
}

int ricoh618_set(char *s, int mv)
{
	uint8_t tmp, vsel;
	struct ricoh618_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ricoh618 has no regulator named %s\n", s);
		return -1;
	}

	mv = mv * 1000;

	if (mv > p->max_uv || mv < p->min_uv) {
		printf("<%duv> out range: %s<%duv, %duv>\n",
				mv, s, p->min_uv, p->max_uv);
		return -1;
	}

	vsel = (mv - p->min_uv) / p->step_uv;

	tmp = ricoh618_read(p->vol_reg);
	tmp &= ~(p->mask);
	tmp |= vsel << p->shift;
	ricoh618_write(p->vol_reg, tmp);

	return 0;
}

int ricoh618_get(char *s)
{
	int tmp;
	uint8_t vsel;
	struct ricoh618_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ricoh618 has no regulator named %s\n", s);
		return -1;
	}

	vsel = ricoh618_read(p->vol_reg);
	vsel = (vsel & p->mask) >> p->shift;

	return (p->min_uv + vsel * p->step_uv) / 1000;
}

int ricoh618_enable(char *s)
{
	uint8_t tmp;
	struct ricoh618_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ricoh618 has no regulator named %s\n", s);
		return -1;
	}

	tmp = ricoh618_read(p->en_reg);
	tmp |= 1 << p->en_bit;
	ricoh618_write(p->en_reg, tmp);

	return 0;
}

int ricoh618_disable(char *s)
{
	uint8_t tmp;
	struct ricoh618_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ricoh618 has no regulator named %s\n", s);
		return -1;
	}

	tmp = ricoh618_read(p->en_reg);
	tmp &= ~(1 << p->en_bit);
	ricoh618_write(p->en_reg, tmp);

	return 0;
}

int ricoh618_is_enabled(char *s)
{
	uint8_t tmp;
	struct ricoh618_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ricoh618 has no regulator named %s\n", s);
		return -1;
	}

	tmp = ricoh618_read(p->en_reg);

	return (tmp & (1 << p->en_bit)) ? 1: 0;
}

int ricoh618_cmd_write(uint8_t reg, uint8_t buf)
{
	ricoh618_write(reg, buf);
	return 0;
}

int ricoh618_cmd_read(uint8_t reg)
{
	return ricoh618_read(reg);
}

int ricoh618_help(void)
{
	printf("\n");
	printf("|\t\t<Regulator Info>\t\t\n");
	printf("|\n");
	printf("| Name\tUsed\tMin/mv\tMax/mv\tStep/mv\n");
	printf("| dc1\tcpu\t600\t3500\t25\n");
	printf("| dc2\tcore\t600\t3500\t25\n");
	printf("| dc3\tio33\t600\t3500\t25\n");
	printf("| ldo1\tpll\t900\t3500\t25\n");
	printf("| ldo2\twifi\t900\t3500\t25\n");
	printf("| ldo3\tcam18\t600\t3500\t25\n");
	printf("| ldo4\tcam28\t900\t3500\t25\n");
	printf("| ldo5\taudio\t900\t3500\t25\n");
	printf("\n");
	printf("|\t\t<Usage>\t\t\n");
	printf("|\n");
	printf("| pmu set [name] [voltage/mv]\n");
	printf("| pmu get [name]\n");
	printf("| pmu enable [name]\n");
	printf("| pmu disable [name]\n");
	printf("| pmu status [name]\n");
	printf("| pmu help, printf help info\n");
	printf("|\n");

	return 0;
}
