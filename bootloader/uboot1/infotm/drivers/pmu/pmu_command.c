#include <common.h>
#include <asm/io.h>
#include <bootlist.h>
#include <dramc_init.h>
#include <items.h>
#include <nand.h>
#include <preloader.h>
#include <rballoc.h>
#include <rtcbits.h>
#include <pmu_command.h>

struct pmu_func {
	char name[16];
	int (*set)(char *s, int mv);
	int (*get)(char *s);
	int (*enable)(char *s);
	int (*disable)(char *s);
	int (*is_enabled)(char *s);
	int (*write)(uint8_t reg, uint8_t buf);
	int (*read)(uint8_t reg);
	int (*help)(void);
};

static int null_set(char *s, int mv) { return 0; }
static int null_get(char *s) { return 0; }
static int null_enable(char *s) { return 0; }
static int null_disable(char *s) { return 0; }
static int null_is_enabled(char *s) { return 0; }
static int null_cmd_write(uint8_t reg, uint8_t buf) { return 0; }
static int null_cmd_read(uint8_t reg) { return 0; }
static int null_help(void) { return 0; }

static struct pmu_func pmu_funcs[] = {
	{
		"null",
		null_set,
		null_get,
		null_enable,
		null_disable,
		null_is_enabled,
		null_cmd_write,
		null_cmd_read,
		null_help,
	}, {
		"axp202",
		axp202_set,
		axp202_get,
		axp202_enable,
		axp202_disable,
		axp202_is_enabled,
		axp202_cmd_write,
		axp202_cmd_read,
		axp202_help,
	}, {
		"ricoh618",
		ricoh618_set,
		ricoh618_get,
		ricoh618_enable,
		ricoh618_disable,
		ricoh618_is_enabled,
		ricoh618_cmd_write,
		ricoh618_cmd_read,
		ricoh618_help,
	}, {
		"ec610",
		ec610_set,
		ec610_get,
		ec610_enable,
		ec610_disable,
		ec610_is_enabled,
		ec610_cmd_write,
		ec610_cmd_read,
		ec610_help,
	},
};

static struct pmu_func *pmu_fp = pmu_funcs;

void pmu_scan(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(pmu_funcs); i++)
		if(item_equal("pmu.model", pmu_funcs[i].name, 0))
			pmu_fp = &pmu_funcs[i];

	printf("PMU model is set to: %s\n", pmu_fp->name);
}

int pmu_set_vol(char *s, int mv)
{
	return pmu_fp->set(s, mv);
}

int pmu_get_vol(char *s)
{
	return pmu_fp->get(s);
}

int pmu_enable(char *s)
{
	return pmu_fp->enable(s);
}

int pmu_disable(char *s)
{
	return pmu_fp->disable(s);
}

int pmu_is_enabled(char *s)
{
	return pmu_fp->is_enabled(s);
}

int pmu_write(uint8_t reg, uint8_t buf)
{
	return pmu_fp->write(reg, buf);
}

int pmu_read(uint8_t reg)
{
	return pmu_fp->read(reg);
}

int pmu_help_info(void)
{
	return pmu_fp->help();
}
