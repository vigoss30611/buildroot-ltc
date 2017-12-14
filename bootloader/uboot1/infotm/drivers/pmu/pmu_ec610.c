#include <common.h>
#include <asm/io.h>
#include <imapx_iic.h>

struct ec610_regulator {
	char	name[16];
	uint8_t	vol_reg;
	uint8_t	en_reg;
	int	en_bit;
	uint8_t	mask;
	int	shift;
	int	min_mv;
	int	max_mv;
	int	step_mv;
};

static int ec610_write(uint8_t reg, uint8_t buf)
{
	iicreg_write(0, &reg, 0x01, &buf, 0x01, 0x01);

	return 0;
}

static int ec610_read(uint8_t reg)
{
	uint8_t buf = 0;

	iicreg_write(0, &reg, 0x01, &buf, 0x00, 0x01);
	iicreg_read(0, &buf, 0x1);

	return buf;
}

static struct ec610_regulator ec610_regulator[] = {
	{"dc0", 0x21, 0x20, 0, 0x3e, 1, 600, 1375, 25},
	{"dc1", 0x28, 0x20, 1, 0x3e, 1, 600, 1375, 25},
	{"dc2", 0x2f, 0x20, 2, 0x7e, 1, 600, 2175, 25},
	{"dc3", 0x36, 0x20, 3, 0x7e, 1, 2200, 3500, 25},
	{"ldo0", 0x42, 0x41, 0, 0x7f, 0, 700, 3400, 25},
	{"ldo1", 0x44, 0x41, 1, 0x7f, 0, 700, 3400, 25},
	{"ldo2", 0x46, 0x41, 2, 0x7f, 0, 700, 3400, 25},
	{"ldo4", 0x4a, 0x41, 4, 0x7f, 0, 700, 3400, 25},
	{"ldo5", 0x4c, 0x41, 5, 0x7f, 0, 700, 3400, 25},
	{"ldo6", 0x4e, 0x41, 6, 0x7f, 0, 700, 3400, 25},
};

static struct ec610_regulator *find_regulator(char *s)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ec610_regulator); i++) {
		if (!strcmp(s, &ec610_regulator[i].name))
			return &ec610_regulator[i];
	}

	return NULL;
}

int ec610_set(char *s, int mv)
{
	uint8_t tmp, vsel;
	struct ec610_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ec610 has no regulator named %s\n", s);
		return -1;
	}

	if (mv > p->max_mv || mv < p->min_mv) {
		printf("<%dmv> out range: %s<%dmv, %dmv>\n",
				mv, s, p->min_mv, p->max_mv);
		return -1;
	}

	vsel = (mv - p->min_mv) / p->step_mv;

	tmp = ec610_read(p->vol_reg);
	tmp &= ~(p->mask);
	tmp |= vsel << p->shift;
	ec610_write(p->vol_reg, tmp);

	return 0;
}

int ec610_get(char *s)
{
	uint8_t tmp, vsel;
	struct ec610_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ec610 has no regulator named %s\n", s);
		return -1;
	}

	vsel = ec610_read(p->vol_reg);
	vsel = (vsel & p->mask) >> p->shift;

	return (p->min_mv + vsel * p->step_mv);
}

int ec610_enable(char *s)
{
	uint8_t tmp;
	struct ec610_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ec610 has no regulator named %s\n", s);
		return -1;
	}

	tmp = ec610_read(p->en_reg);
	tmp |= 1 << p->en_bit;
	ec610_write(p->en_reg, tmp);

	return 0;
}

int ec610_disable(char *s)
{
	uint8_t tmp;
	struct ec610_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ec610 has no regulator named %s\n", s);
		return -1;
	}

	tmp = ec610_read(p->en_reg);
	tmp &= ~(1 << p->en_bit);
	ec610_write(p->en_reg, tmp);

	return 0;
}

int ec610_is_enabled(char *s)
{
	uint8_t tmp;
	struct ec610_regulator *p = find_regulator(s);

	if (p == NULL) {
		printf("ec610 has no regulator named %s\n", s);
		return -1;
	}

	tmp = ec610_read(p->en_reg);

	return (tmp & (1 << p->en_bit)) ? 1: 0;
}

int ec610_cmd_write(uint8_t reg, uint8_t buf)
{
	ec610_write(reg, buf);
	return 0;
}

int ec610_cmd_read(uint8_t reg)
{
	return ec610_read(reg);
}

int ec610_help(void)
{
	//printf("+---------------------------------------\n");
	printf("\n");
	printf("|\t\t<Regulator Info>\t\t\n");
	printf("|\n");
	printf("| Name\tUsed\tMin/mv\tMax/mv\tStep/mv\n");
	printf("| dc0\tcore\t600\t1375\t25\n");
	printf("| dc1\tnull\t600\t1375\t25\n");
	printf("| dc2\tddr\t600\t2175\t25\n");
	printf("| dc3\tio33\t2200\t3500\t25\n");
	printf("| ldo0\tpll\t700\t3400\t25\n");
	printf("| ldo1\taudio\t700\t3400\t25\n");
	printf("| ldo2\tcam18\t700\t3400\t25\n");
	printf("| ldo4\tcam28\t700\t3400\t25\n");
	printf("| ldo5\ti80\t700\t3400\t25\n");
	printf("| ldo6\tnull\t700\t3400\t25\n");
	printf("\n");
	//printf("+---------------------------------------\n");
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
