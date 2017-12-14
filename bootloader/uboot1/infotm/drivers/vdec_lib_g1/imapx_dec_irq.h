
#ifndef _IMAPX_DEC_IRQ_H_
#define _IMAPX_DEC_IRQ_H_

/* 
 *(1)Author:summer
 *(2)Date:2014-8-4
 *(3)Reason:
 *(3.1)iMAPx15 is 0x2500_0000
 *(3.2)iMAPx910 is 0x2810_0000
 * */
#define IMAPX_VDEC_REG_BASE 	(0x25000000)
#define IMAPX_VDEC_INTR		    (128)

void vdec_irq_init(void);
void vdec_irq_deinit(void);
void vdec_irq_wait(void);
void pp_irq_wait(void);
void vdec_irq(void);


#endif
