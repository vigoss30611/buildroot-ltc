/**








**/

#ifndef _IMAPX_DEC_IRQ_H_
#define _IMAPX_DEC_IRQ_H_

#include <imapx_system.h>
#include <interrupt.h>

#define IMAPX_VDEC_REG_BASE 	(VDEC_BASE_ADDR)
#define IMAPX_VDEC_INTR		VDEC_INT_ID	//SD1_INT_ID//UART3_INT_ID//(VDEC_INT_ID)

void vdec_irq_init(void);
void vdec_irq_deinit(void);
void vdec_irq_wait(void);
void pp_irq_wait(void);
void vdec_irq(void);


void vdec_register_rt(void *rt);
void vdec_rt_write_reg(unsigned int addr, unsigned value);
void vdec_rt_read_reg(unsigned int addr, unsigned value);
void vdec_rt_enable_hw();
void vdec_rt_disable_hw();
void vdec_rt_wait_hw();


#endif
