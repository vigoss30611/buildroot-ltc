/**








**/

#include <imapx_system.h>
#include <cmnlib.h>
#include <rtl_system.h>
#include <rtl_trace.h>

#include "imapx_dec_irq.h"


//#define POLLING_HW_READY


#define IMAPX800_DEC_INTR_REG_OFFSET 1
#define IMAPX800_PP_INTR_REG_OFFSET 60
//#define VDEC_BUS_ERROR_CONCEALED

volatile unsigned int gVDECIntr = 0;
volatile unsigned int gPpIntr = 0;
static rtl_trace_callback_t gRt={0};

void vdec_irq_init(void)
{
	gVDECIntr = 0;
	gPpIntr = 0;

#ifndef POLLING_HW_READY
	intr_register(IMAPX_VDEC_INTR, (INTR_ISR)vdec_irq, 0);
	intr_clear(IMAPX_VDEC_INTR);
	intr_enable(IMAPX_VDEC_INTR);
#endif
}

void vdec_irq_deinit(void)
{
	intr_disable(IMAPX_VDEC_INTR);

	gRt.read_reg = NULL;
	gRt.write_reg = NULL;
	gRt.enable_hw = NULL;
	gRt.wait_hw_ready = NULL;	
}

void vdec_irq(void)
{
	volatile unsigned int  decVal;
	volatile unsigned int  ppVal;
	volatile unsigned int  *pVal = (volatile unsigned int  *)IMAPX_VDEC_REG_BASE;
	
	intr_mask(IMAPX_VDEC_INTR);

	//log_printf("decVal=0x%x",pVal[IMAPX800_DEC_INTR_REG_OFFSET]);
	
	if(pVal[IMAPX800_DEC_INTR_REG_OFFSET]&0x100){
		pVal[IMAPX800_DEC_INTR_REG_OFFSET] &= (~0x100);
		//log_printf("decVal=0x%x",pVal[IMAPX800_DEC_INTR_REG_OFFSET]);
		gVDECIntr = 1;
	}

	//log_printf("ppVal=0x%x",pVal[IMAPX800_PP_INTR_REG_OFFSET]);
	if(pVal[IMAPX800_PP_INTR_REG_OFFSET]&0x100){
		pVal[IMAPX800_PP_INTR_REG_OFFSET] &= (~0x100);
		//log_printf("ppVal=0x%x",pVal[IMAPX800_PP_INTR_REG_OFFSET]);
		gPpIntr = 1;
	}

	intr_clear(IMAPX_VDEC_INTR);
	intr_unmask(IMAPX_VDEC_INTR);
}

void vdec_irq_wait(void)
{
	volatile unsigned int  *pVal = (volatile unsigned int  *)IMAPX_VDEC_REG_BASE;
	vdec_rt_wait_hw();
#ifdef POLLING_HW_READY 
	while((pVal[IMAPX800_DEC_INTR_REG_OFFSET] & 0x100) == 0);
	pVal[IMAPX800_DEC_INTR_REG_OFFSET] &= (~0x100);
#else
	while(gVDECIntr == 0);
	gVDECIntr = 0;
#endif
}


void pp_irq_wait(void)
{
	volatile unsigned int  *pVal = (volatile unsigned int  *)IMAPX_VDEC_REG_BASE;	
	vdec_rt_wait_hw();
#ifdef POLLING_HW_READY	
	while((pVal[IMAPX800_PP_INTR_REG_OFFSET] & 0x100) == 0);
	pVal[IMAPX800_PP_INTR_REG_OFFSET] &= (~0x100);
#else
	while( gPpIntr == 0 );
	gPpIntr = 0;	
#endif
}

void vdec_register_rt(void *rt)
{
	if(rt != NULL){
		gRt.read_reg = ((rtl_trace_callback_t *)rt)->read_reg;
		gRt.write_reg = ((rtl_trace_callback_t *)rt)->write_reg;
		gRt.enable_hw = ((rtl_trace_callback_t *)rt)->enable_hw;
		gRt.wait_hw_ready = ((rtl_trace_callback_t *)rt)->wait_hw_ready;
	}
}

void vdec_rt_write_reg(unsigned int addr, unsigned value)
{
	if(gRt.write_reg != NULL){
		gRt.write_reg(addr, value);
	}
}

void vdec_rt_read_reg(unsigned int addr, unsigned value)
{
	if(gRt.read_reg != NULL){
		gRt.read_reg(addr, value);
	}
}

void vdec_rt_enable_hw()
{
	if(gRt.enable_hw != NULL){
		gRt.enable_hw();
	}
}

void vdec_rt_disable_hw()
{
}

void vdec_rt_wait_hw()
{
	if(gRt.wait_hw_ready != NULL){
		gRt.wait_hw_ready();
	}
}

