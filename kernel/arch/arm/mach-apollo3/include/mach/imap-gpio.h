#ifndef __IMAPX_GPIO_H__
#define __IMAPX_GPIO_H__

#define INTTYPE_LOW     (0)
#define INTTYPE_HIGH    (1)
#define INTTYPE_FALLING (2)
#define INTTYPE_RISING  (4)
#define INTTYPE_BOTH    (6)

int ix_get_gpio_num(const char *gpio);
int ix_get_gpio_irq(int num);
void ix_set_gpio_cfg(int num,int type,unsigned int flt);
int ix_get_gpio_val(const char *gpio);
int ix_check_gpio_pending(const char *gpio,int clear);
unsigned int irq_type_to_flag(int type);

#endif
