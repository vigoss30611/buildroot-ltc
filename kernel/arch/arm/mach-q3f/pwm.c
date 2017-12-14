
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/clk.h>
#include <linux/mutex.h>

#include <mach/irqs.h>
#include <linux/io.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
 
#include <mach/imap-pwm.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mach/pad.h>

#include <mach/items.h>
#include <linux/cpufreq.h>
#include <linux/regulator/machine.h>



#ifdef CONFIG_CPU_FREQ

int pwm_cpufreq_prechange(int pre_clk)
{
	pr_info("pwm_cpufreq_prechange has none!\n");
 	return 0;
}

int pwm_cpufreq_trans(void)
{
	pr_info("pwm_cpufreq_trans has none!\n");
 	return 0;
}
#endif
