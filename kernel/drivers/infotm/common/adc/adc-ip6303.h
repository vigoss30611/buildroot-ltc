/*************************************************************************
    > File Name: drivers/infotm/common/adc/adc-ip6303.h
    > Author: tsico
    > Mail: can.chai@infotm.com 
    > Created Time: 2017年09月29日 星期五 09时34分37秒
 ************************************************************************/
#ifndef __ADC_IP6303_H
#define __ADC_IP6303_H

extern int ip6303_write(struct device *dev, uint8_t reg, uint8_t val);
extern int ip6303_read(struct device *dev, uint8_t reg, uint8_t *val);

#define IP6303_ADC_CHNS_NUM    2

#define IP6303_ADC_KEY_IRQ              (213)

#define ip6303_adc_writel(_dev, _reg, _val) ip6303_write(_dev, _reg, _val)
#define ip6303_adc_readl(_dev, _reg, _val)    ip6303_read(_dev, _reg, _val)

struct ip6303_adc_chip {

	struct adc_chip         chip;        
	struct device           *dev;        
	unsigned long           chan_bitmap; 
	unsigned long           event_bitmap;
	int                     irq;         
};       

#endif
