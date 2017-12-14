#if !defined(__PWM_H__)
#define __PMW_H__

extern int pwm_init(int channel);
extern int pwm_set(int freq, int duty);

#endif /* __PWM_H__ */

