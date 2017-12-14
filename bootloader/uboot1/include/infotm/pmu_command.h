#ifndef __PMU_COMMAND_H__
#define __PMU_COMMAND_H__

extern int axp202_set(char *s, int mv);
extern int axp202_get(char *s);
extern int axp202_enable(char *s);
extern int axp202_disable(char *s);
extern int axp202_is_enabled(char *s);
extern int axp202_cmd_write(uint8_t reg, uint8_t buf);
extern int axp202_cmd_read(uint8_t reg);
extern int axp202_help(void);

extern int ricoh618_set(char *s, int mv);
extern int ricoh618_get(char *s);
extern int ricoh618_enable(char *s);
extern int ricoh618_disable(char *s);
extern int ricoh618_is_enabled(char *s);
extern int ricoh618_cmd_write(uint8_t reg, uint8_t buf);
extern int ricoh618_cmd_read(uint8_t reg);
extern int ricoh618_help(void);

extern int ec610_set(char *s, int mv);
extern int ec610_get(char *s);
extern int ec610_enable(char *s);
extern int ec610_disable(char *s);
extern int ec610_is_enabled(char *s);
extern int ec610_cmd_write(uint8_t reg, uint8_t buf);
extern int ec610_cmd_read(uint8_t reg);
extern int ec610_help(void);

extern void pmu_scan(void);
extern int pmu_set_vol(char *s, int mv);
extern int pmu_get_vol(char *s);
extern int pmu_enable(char *s);
extern int pmu_disable(char *s);
extern int pmu_is_enabled(char *s);
extern int pmu_write(uint8_t reg, uint8_t buf);
extern int pmu_read(uint8_t reg);
extern int pmu_help_info(void);

#endif
