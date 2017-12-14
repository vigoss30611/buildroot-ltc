/*
 * Generic adc & dac lib implementation
 *
 */

#include <linux/module.h>
#include <linux/radix-tree.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/adc.h>


#define MAX_ADCS   64

static DEFINE_MUTEX(adc_lock);
static LIST_HEAD(adc_chips);
static DECLARE_BITMAP(allocated_adcs, MAX_ADCS);


static struct adc_device *adc_to_device(struct adc_request *req)
{
	struct adc_chip *entry;                           
	struct adc_chip *chip;                           
	struct list_head *p;
	int i;

	if(req->chip_name){
		list_for_each(p, &adc_chips){                
			entry = list_entry(p, struct adc_chip, list);
			if(!strcmp(entry->name, req->chip_name)){
				chip = entry;
				break;
			}
		} 
	}else{
		list_for_each(p, &adc_chips){                
			entry = list_entry(p, struct adc_chip, list);
			if(entry->priority == 1){
				chip = entry;
				break;
			}
		}
		return NULL;
	}

	for(i = 0; i < chip->nadc; i++){	
		if(req->id == chip->adcs[i].adc)
			return &chip->adcs[i];
	}
}

static void free_adcs(struct adc_chip *chip)
{
	bitmap_clear(allocated_adcs, chip->base, chip->nadc);
	kfree(chip->adcs);
	chip->adcs = NULL;
}

int adc_chip_add(struct adc_chip *chip){
	struct adc_device *adc;
	unsigned int i;
	int ret;
	
	if (!chip || !chip->dev || !chip->ops)
		return -EINVAL;
	
	mutex_lock(&adc_lock);
	chip->adcs = kzalloc(chip->nadc * sizeof(*adc), GFP_KERNEL); 
	if (!chip->adcs) {
		ret = -ENOMEM;
		goto out;
	}
	
	chip->base = 0;
	
	for (i = 0; i < chip->nadc; i++) {
		adc = &chip->adcs[i];
		adc->chip = chip;
		adc->adc = chip->base + i;
		adc->hwadc = i;
	}
	bitmap_set(allocated_adcs, chip->base, chip->nadc);

	INIT_LIST_HEAD(&chip->list);
	list_add(&chip->list, &adc_chips);
	ret = 0;
	
out:
	mutex_unlock(&adc_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(adc_chip_add);

int adc_chip_remove(struct adc_chip *chip){
	unsigned int i;
	int ret = 0;
	
	mutex_lock(&adc_lock);
	
	for (i = 0; i < chip->nadc; i++) {
		struct adc_device *adc = &chip->adcs[i];
		if (test_bit(ADC_REQUESTED, &adc->flags)) {
			ret = -EBUSY;
			goto out;
		}
	}
	list_del_init(&chip->list);
	free_adcs(chip);
out:
	mutex_unlock(&adc_lock);
	return ret;
	
}
EXPORT_SYMBOL_GPL(adc_chip_remove);

static int adc_device_request(struct adc_device *adc, const char *label)
{
	int err;
	if (test_bit(ADC_REQUESTED, &adc->flags))
		return -EBUSY;  

	if (!try_module_get(adc->chip->ops->owner))
		return -ENODEV;

	if (adc->chip->ops->request) {
		err = adc->chip->ops->request(adc->chip, adc);
		if (err) {
            module_put(adc->chip->ops->owner);
			return err;
		}
	}

	set_bit(ADC_REQUESTED, &adc->flags);
	adc->label = label;        

        return 0;                                                       
}

static void adc_put(struct adc_device *adc) 
{
	if(!adc)
		return; 
	mutex_lock(&adc_lock);
	if (!test_and_clear_bit(ADC_REQUESTED, &adc->flags)) {
		pr_warn("ADC device already freed\n");
		goto out;
	}
	
	if (adc->chip->ops->free)
		adc->chip->ops->free(adc->chip, adc);
	adc->label = NULL;
	module_put(adc->chip->ops->owner);
out:
	mutex_unlock(&adc_lock);
} 

int adc_enable(struct adc_device *adc)
{
	int ret = 0;
	if(!adc)
		return -EINVAL;
	ret = !test_and_set_bit(ADC_ENABLED, &adc->flags);
	if (ret)
		return adc->chip->ops->enable(adc->chip, adc);
}
EXPORT_SYMBOL_GPL(adc_enable);

void adc_disable(struct adc_device *adc)
{
	int ret = 0;
	if(!adc)
		return;
	ret = test_and_clear_bit(ADC_ENABLED, &adc->flags);
	if (ret)
		adc->chip->ops->disable(adc->chip, adc);
}
EXPORT_SYMBOL_GPL(adc_disable);

int adc_read_raw(struct adc_device *adc, int *val, int *val1)
{
	if(!adc || !adc->chip->ops) {
		return -EINVAL;
	}
	if (!adc->chip->ops->read_raw) {
		return -ENOSYS;
	}

	return adc->chip->ops->read_raw(adc->chip, adc, val, val1);
}
EXPORT_SYMBOL_GPL(adc_read_raw);

int adc_get_irq_state(struct adc_device *adc) {
	if(!adc || !adc->chip->ops) {
		return -EINVAL;
	}
	if (!adc->chip->ops->get_irq_state) {
		return -ENOSYS;
	}
	return adc->chip->ops->get_irq_state(adc->chip, adc);
}
EXPORT_SYMBOL_GPL(adc_get_irq_state);

void adc_clear_irq(struct adc_device *adc) {
	if(!adc || !adc->chip->ops) {
		return;
	}
	if (!adc->chip->ops->clear_irq) {
		return;
	}
	return adc->chip->ops->clear_irq(adc->chip, adc);
}
EXPORT_SYMBOL_GPL(adc_clear_irq);

int adc_set_irq_watermark(struct adc_device *adc, int watermark) {
	if(!adc || !adc->chip->ops) {
		return -EINVAL;
	}
	if (!adc->chip->ops->set_irq_watermark) {
		return -ENOSYS;
	}
	return adc->chip->ops->set_irq_watermark(adc->chip, adc, watermark);
}
EXPORT_SYMBOL_GPL(adc_set_irq_watermark);

struct adc_device *adc_request(struct adc_request *req)
{
        struct adc_device *dev;
        int err;

        if (req->id < 0 || req->id >= MAX_ADCS)
                return ERR_PTR(-EINVAL);

        mutex_lock(&adc_lock);

        dev = adc_to_device(req);
        if (!dev) { 
                dev = ERR_PTR(-EPROBE_DEFER);
                goto out;
        }

	dev->mode = req->mode;

	err = adc_device_request(dev, req->label);
        if (err < 0)
                dev = ERR_PTR(err); 

out:
        mutex_unlock(&adc_lock);

        return dev;
}
EXPORT_SYMBOL_GPL(adc_request);

void adc_free(struct adc_device *adc)
{
	adc_put(adc);
}
EXPORT_SYMBOL_GPL(adc_free);



