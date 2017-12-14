#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <mach/gpio.h>
//#include <mach/regs-gpio.h>
//#include <plat/gpio-cfg.h>
#include <linux/semaphore.h>
#include <mach/items.h>

#define DEVICE_NAME                                "beep"
#define PWM_IOCTL_SET_FREQ                1
#define PWM_IOCTL_STOP                        0
#define NS_IN_1HZ                                (1000000000UL)
#define BUZZER_PWM_ID                        1
//#define BUZZER_PMW_GPIO                        S5PV210_GPD0(0)

static struct pwm_device *pwm4buzzer;
static struct semaphore lock;

static unsigned long current_freq = 0;
static int curret_status = 1; //stop

static int pwm_id = 1;

static void pwm_set_freq(unsigned long freq) {
        int period_ns = NS_IN_1HZ / freq;
        pwm_config(pwm4buzzer, period_ns/2, period_ns);
        pwm_enable(pwm4buzzer);
}

static void pwm_stop(void) {
        pwm_config(pwm4buzzer, 0, NS_IN_1HZ / 100);
        pwm_disable(pwm4buzzer);
}

static int my_pwm_open(struct inode *inode, struct file *file) {
        if (!down_trylock(&lock))
                return 0;
        else
                return -EBUSY;
}

static int my_pwm_close(struct inode *inode, struct file *file) {
        up(&lock);
        return 0;
}

static long my_pwm_ioctl(struct file *filep, unsigned int cmd,
                unsigned long arg)
{
        switch (cmd) {
                case PWM_IOCTL_SET_FREQ:
                        if (arg == 0)
                                return -EINVAL;
			    printk(KERN_ERR"start beep\nmy_pwm_ioctl=%d\n",arg);			
                        pwm_set_freq(arg);
                        break;

                case PWM_IOCTL_STOP:
                default:
			    printk(KERN_ERR"stop beep\n");		
                        pwm_stop();
                        break;
        }

        return 0;
}
static ssize_t beep_freq_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long data;
	int error;

	error = strict_strtoul(buf, 10, &data);
	if (error)
	{
		printk(KERN_ERR "%s: strict_strtoul failed, error=0x%x\n", __func__, error);
		return error;
	}
	//printk(KERN_ERR"start beep and set freq=%d\n",data);
	pwm_set_freq(data);
	current_freq = data;
	curret_status = 0;
		
	return count;
}

static ssize_t beep_freq_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,  "%d\n", current_freq);
}

static ssize_t beep_stop_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long data;
	int error;

	error = strict_strtoul(buf, 10, &data);
	if (error)
	{
		printk(KERN_ERR "%s: strict_strtoul failed, error=0x%x\n", __func__, error);
		return error;
	}
	
	if(curret_status == 1)
	{
		printk(KERN_ERR"beep has beeped stoped.\n");
		return count;
	}
	
	if(data == 1)//stop
	{
		pwm_stop();
		curret_status = 1;
		//printk(KERN_ERR"stop beep.\n");
	}
	else
		printk(KERN_ERR"invalud argument, beep curret_status =%d\n",curret_status);
	
	return count;
}

static ssize_t beep_stop_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,  "%d\n", curret_status);
}


static DEVICE_ATTR(beep_freq, 0666, beep_freq_show, beep_freq_store);
static DEVICE_ATTR(beep_stop, 0666, beep_stop_show, beep_stop_store);


static struct attribute *imapx_beep_attribute[] = {
    &dev_attr_beep_freq.attr,
     &dev_attr_beep_stop.attr,		
    NULL
};

static const struct attribute_group imapx_beep_attr_group = {
    .attrs = imapx_beep_attribute,
};

static struct file_operations my_pwm_ops = {
        .owner                        = THIS_MODULE,
        .open                        = my_pwm_open,
        .release                = my_pwm_close,
        .unlocked_ioctl        = my_pwm_ioctl,
};

static struct miscdevice my_misc_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = DEVICE_NAME,
        .fops = &my_pwm_ops,
};

static int __init my_pwm_dev_init(void) {
	int ret;
	printk(DEVICE_NAME " my_pwm_dev_init\n");
	/*
	   ret = gpio_request(BUZZER_PMW_GPIO, DEVICE_NAME);
	   if (ret) {
	   printk("request GPIO %d for pwm failed\n", BUZZER_PMW_GPIO);
	   return ret;
	   }

	   gpio_set_value(BUZZER_PMW_GPIO, 0);
	   s3c_gpio_cfgpin(BUZZER_PMW_GPIO, S3C_GPIO_OUTPUT);
	 */

	if (item_exist("beep.ctrl")) {
		pwm_id = item_integer("beep.ctrl", 1);
	}

	pwm4buzzer = pwm_request(BUZZER_PWM_ID, DEVICE_NAME);
	if (IS_ERR(pwm4buzzer)) {
		printk("request pwm %d for %s failed\n", BUZZER_PWM_ID, DEVICE_NAME);
		return -ENODEV;
	}

	pwm_stop();
	//  s3c_gpio_cfgpin(BUZZER_PMW_GPIO, S3C_GPIO_SFN(2));
	//     gpio_free(BUZZER_PMW_GPIO);
	sema_init(&lock,1);
	ret = misc_register(&my_misc_dev);

	ret = sysfs_create_group(&pwm4buzzer->chip->dev->kobj, &imapx_beep_attr_group);
	if (ret)
		printk("Creat imapx beep attr  failed\n");

	return ret;
}

static void __exit my_pwm_dev_exit(void) {
	printk(DEVICE_NAME " my_pwm_dev_exit\n");
	sysfs_remove_group(&pwm4buzzer->chip->dev->kobj, &imapx_beep_attr_group);	
	pwm_stop();
	misc_deregister(&my_misc_dev);
}

module_init(my_pwm_dev_init);
module_exit(my_pwm_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("biao.xiong@infotm.com");
MODULE_DESCRIPTION("beep Driver");

