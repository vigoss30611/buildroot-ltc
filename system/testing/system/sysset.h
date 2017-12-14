#ifndef __SYSSET_TEST_H__
#define __SYSSET_TEST_H__

#define SYSSET_DEBUG

#ifdef SYSSET_DEBUG
#define sys_dbg(arg...)		fprintf(stderr, arg)
#else
#define sys_dbg(arg...)
#endif

#define sys_info(arg...)	fprintf(stderr, arg)
#define sys_err(arg...)		fprintf(stderr, arg)

extern int bl_test(int argc, char **argv);
extern int led_test(int argc, char **argv);
extern int gpio_test(int argc, char **argv);
extern int wdt_test(int argc, char **argv);
extern int ac_test(int argc, char **argv);
extern int batt_test(int argc, char **argv);
extern int soc_test(int argc, char **argv);
extern int boot_test(int argc, char **argv);
extern int storage_test(int argc, char **argv);
extern int process_test(int argc, char **argv);
extern int setting_test(int argc, char **argv);

#endif
