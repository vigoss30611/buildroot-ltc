

#include "imapx_dec_irq.h"


#define IMAPX800_DEC_INTR_REG_OFFSET 1
#define IMAPX800_PP_INTR_REG_OFFSET 60

#ifdef VDEC_DEBUG_IRQ
#define DEBUG_MSG(fmt...) printf(fmt)
#else
#define DEBUG_MSG(fmt...) 
#endif


void vdec_irq_init(void)
{
}

void vdec_irq_deinit(void)
{
}

void vdec_irq_wait(void)
{
	volatile unsigned int  *pVal = (volatile unsigned int  *)IMAPX_VDEC_REG_BASE;
    DEBUG_MSG("pval[0] = %x \n",pVal[0] );
    DEBUG_MSG("pval[IMAPX800_DEC_INTR_REG_OFFSET] = %x \n",pVal[IMAPX800_DEC_INTR_REG_OFFSET]);
	while((pVal[IMAPX800_DEC_INTR_REG_OFFSET] & 0x100) == 0);

    DEBUG_MSG("xxx pval[IMAPX800_DEC_INTR_REG_OFFSET] = %x \n",pVal[IMAPX800_DEC_INTR_REG_OFFSET]);
	pVal[IMAPX800_DEC_INTR_REG_OFFSET] &= (~0x100);
    udelay(200000);
}

void pp_irq_wait(void)
{
	volatile unsigned int  *pVal = (volatile unsigned int  *)IMAPX_VDEC_REG_BASE;	
    
    DEBUG_MSG("pp pval[0] = %x \n",pVal[0] );
	while((pVal[IMAPX800_PP_INTR_REG_OFFSET] & 0x100) == 0);
	pVal[IMAPX800_PP_INTR_REG_OFFSET] &= (~0x100);
    udelay(5000);
}

