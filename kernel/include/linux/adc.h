#ifndef __LINUX_ADC_H
#define __LINUX_ADC_H

struct adc_device;
struct adc_chip;
struct adc_request;

#ifdef CONFIG_INFOTM_ADC
int adc_chip_add(struct adc_chip *chip);
int adc_chip_remove(struct adc_chip *chip);
struct adc_device *adc_request(struct adc_request *req);
void adc_free(struct adc_device *adc);
int adc_enable(struct adc_device *adc);
void adc_disable(struct adc_device *adc);
int adc_get_irq_state(struct adc_device *adc);
void adc_clear_irq(struct adc_device *adc);
int adc_read_raw(struct adc_device *adc, int *val, int *val1);
int adc_set_irq_watermark(struct adc_device *adc, int watermark);
#else
static int adc_chip_add(struct adc_chip *chip) {
	 return -EINVAL;
}

static int adc_chip_remove(struct adc_chip *chip) {
	 return -EINVAL;
}

static struct adc_device *adc_request(struct adc_request *req) {
	return ERR_PTR(-ENODEV);
}

static int adc_get_irq_state(struct adc_device *adc) {
	return -EINVAL;
}

static void adc_clear_irq(struct adc_device *adc){
	return ;
}

static void adc_free(struct adc_device *adc) {
}

static int adc_enable(struct adc_device *adc) {
	 return -EINVAL;
}

static void adc_disable(struct adc_device *adc) {
	 return -EINVAL;
}
static int adc_read_raw(struct adc_device *adc, int *val, int *val1) {
	return -EINVAL;
}
static int adc_set_irq_watermark(struct adc_device *adc, int watermark) {
	return -EINVAL;
}
#endif


enum {
	ADC_REQUESTED = 1 << 0,
	ADC_ENABLED = 1 << 1,
};
/*
 *  ADC chip support two kinds of mode as below to users
 */

#define ADC_DEV_SAMPLE  	0
#define ADC_DEV_IRQ_SAMPLE	1

#define ADC_REQUEST_SHARED		1
#define ADC_REQUEST_SINGLE		0

struct adc_platform_data {
	unsigned long            chan_bitmap;
};

struct adc_request {
	char		*chip_name;
	int			id;
	unsigned int 		mode;
	char			*label;
	
};

struct adc_device {
	const char              *label;
	unsigned long           flags;
	unsigned int            hwadc;
	unsigned int            adc;
	unsigned int 		mode;
	int			irq;
	struct adc_chip         *chip;
	void                    *chip_data;
};

struct adc_ops {
	int 	(*request)(struct adc_chip *chip,
			struct adc_device *adc);
	void 	(*free)(struct adc_chip *chip,
			struct adc_device *adc);
	int 	(*enable)(struct adc_chip *chip,
			struct adc_device *adc);
	void 	(*disable)(struct adc_chip *chip,
			struct adc_device *adc);
	int 	(*read_raw)(struct adc_chip *chip,
			struct adc_device *adc, int *val, int *val1);
	int 	(*get_irq_state)(struct adc_chip *chip,
			struct adc_device *adc);
	void 	(*clear_irq)(struct adc_chip *chip,
			struct adc_device *adc);
	int 	(*set_irq_watermark)(struct adc_chip *chip,
			struct adc_device *adc, int watermark);
	struct module           *owner;
	
};

struct adc_chip {
	char					*name;
	struct device           *dev;
	struct list_head        list;
	const struct adc_ops    *ops;
	int                     base;
	unsigned int            nadc;
	struct adc_device       *adcs;
	int					priority;
};

#define ADC_VAL_INT 1
#define ADC_VAL_FRACTIONAL 2

#endif



