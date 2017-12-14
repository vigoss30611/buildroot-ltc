#ifndef __IMAPX_PWM_H__
#define __IMAPX_PWM_H__

#define TCFG0   (PWM_BASE_REG_PA+0x0)	/* Timer 0 configuration */
#define TCFG1   (PWM_BASE_REG_PA+0x4)	/* Timer 1 configuration */
#define TCON    (PWM_BASE_REG_PA+0x8)	/* Timer control */
#define TCNTB0  (PWM_BASE_REG_PA+0xc)	/* Timer count buffer 0 */
#define TCMPB0  (PWM_BASE_REG_PA+0x10)	/* Timer compare buffer 0 */
#define TCNTO0  (PWM_BASE_REG_PA+0x14)	/* Timer count observation 0 */
#define TCNTB1  (PWM_BASE_REG_PA+0x18)	/* Timer count buffer 1 */
#define TCMPB1  (PWM_BASE_REG_PA+0x1c)	/* Timer compare buffer 1 */
#define TCNTO1  (PWM_BASE_REG_PA+0x20)	/* Timer count observation 1 */
#define TCNTB2  (PWM_BASE_REG_PA+0x24)	/* Timer count buffer 2 */
#define TCMPB2  (PWM_BASE_REG_PA+0x28)	/* Timer compare buffer 2 */
#define TCNTO2  (PWM_BASE_REG_PA+0x2c)	/* Timer count observation 2 */
#define TCNTB3  (PWM_BASE_REG_PA+0x30)	/* Timer count buffer 3 */
#define TCMPB3  (PWM_BASE_REG_PA+0x34)	/* Timer compare buffer 3 */
#define TCNTO3  (PWM_BASE_REG_PA+0x38)	/* Timer count observation 3 */
#define TCNTB4  (PWM_BASE_REG_PA+0x3c)	/* Timer count buffer 4 */
#define TCNTO4  (PWM_BASE_REG_PA+0x40)	/* Timer count observation 4 */

#endif /*__IMAPX_PWM_H__*/
