#ifndef _GPIO_H_
#define _GPIO_H_

extern void gpio_reset(void);
extern void gpio_mode_set(int, int);
extern void gpio_dir_set(int, int);
extern void gpio_output_set(int, int);
extern void gpio_pull_en(int, int);
extern void gpio_status(int, int *, int *, int *, int *);
extern void gpio_help(void);

#endif
