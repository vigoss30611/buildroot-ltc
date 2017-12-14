
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpufreq.h>
#include <linux/cpu.h>
#include <linux/jiffies.h>
#include <linux/kernel_stat.h>
#include <linux/mutex.h>
#include <linux/hrtimer.h>
#include <linux/tick.h>
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/sched.h>

#include <mach/belt.h>
#include <mach/items.h>


extern int imapx800_cpufreq_getval(void);
extern int imapx800_vpufreq_getval(void);
//extern int (*imapx800_gpufreq_getval)(void);

static int __cpu_i15 = 0;
void infotm_freq_message(int module, int freq)
{
	if(module < 0 || module > 3) return ;

/*
 *[KP-peter]:FIXME 暂时注释掉以下打印cpu freq打印语句
 */
//	printk(KERN_DEBUG "f: %4d %4d %4d (%d: %4d)\n",
//			imapx800_cpufreq_getval(),
//			imapx800_gpufreq_getval? imapx800_gpufreq_getval(): -1,
//			imapx800_vpufreq_getval(), module, freq);
}

inline int __get_gpu_max(int scene)
{
    int m = 450000000;

    if(scene & (SCENE_VIDEO_NET | SCENE_VIDEO_LOCAL))
        m = min(m, 150000000);
    else if(scene & SCENE_LOWPOWER)
        m = min(m, 200000000);
    else if(scene & SCENE_PERFORMANCE)
        m = min(m, 450000000);

    if(scene & SCENE_POWERLIMIT_FIX)
        m = min(m, SCENE_PL_GETG(scene)?
            SCENE_PL_GETG(scene) * 50000000:
            200000000);

    return min(m, __cpu_i15? 450000000: 300000000);
}

inline int __get_cpu_max(int scene)
{
    int m = 1032000000;

    if(scene & SCENE_VIDEO_NET)
        m = min(m, 400000000);
    else if(scene & SCENE_LOWPOWER)
        m = min(m, 600000000);

    if(scene & SCENE_POWERLIMIT_FIX)
        m = min(m, SCENE_PL_GETC(scene)?
            SCENE_PL_GETC(scene) * 50000000:
            400000000);

    return m;
}

inline int __fit_cpu(void)
{
	int fgpu;

        if(__cpu_i15) return 0;
//	if(unlikely(!imapx800_gpufreq_getval))
//		return 0;

//	fgpu = imapx800_gpufreq_getval();

	/* linear fit:
	 * Fcpu = 945 - 0.5*Fgpu
	 * --------------
	 *  44 -> 923
	 *  88 -> 901
	 *  111-> 889.5
	 *  222-> 834
	 *  296-> 797
	 *  444-> 723
	 */
	return 945 - (fgpu >> 1);
}

inline int __fit_gpu(void)
{
        if(__cpu_i15) return 0;

	/* linear fit:
	 * Fgpu = 1990 - 2*Fcpu
	 * --------------
	 * 948 -> 94
	 * 900 ->190
	 * 852 ->286
	 * 804 ->382
	 * 756 ->478
	 */
	return  1990 - (imapx800_cpufreq_getval() << 1);
}

static spinlock_t arbi_lock;
int infotm_freq_arbi(int module, int freq)
{
	int farbi = 0, max;
    unsigned long flags;
	return freq;
    if(__cpu_i15){
        return freq;
    }
	spin_lock_irqsave(&arbi_lock, flags);

	switch(module) 
	{
		case 0:
			farbi = __fit_cpu() * 1000000;
			farbi = (farbi && farbi < freq)? farbi: freq;

			max = __get_cpu_max(belt_scene_get());
			farbi = (farbi > max)? max: farbi;
			break;
		case 1:
			farbi = __fit_gpu() * 1000000;
			farbi = (farbi && farbi < freq)? farbi: freq;

			max = __get_gpu_max(belt_scene_get());
			farbi = (farbi > max)?  max: farbi;
			break;
		case 2:
			farbi = freq;
			break;
	}

#if 0
	if(farbi != freq && farbi < 804000000)
		printk(KERN_ERR "arbi: %d: %d->%d\n", module, freq / 1000000,
				farbi / 1000000);
#endif

	spin_unlock_irqrestore(&arbi_lock, flags);
	return farbi;
}
EXPORT_SYMBOL(infotm_freq_arbi);

void infotm_freq_init(void)
{
	spin_lock_init(&arbi_lock);

	if(item_exist("board.cpu") && item_equal("board.cpu", "i15", 0)){
	  __cpu_i15 = 1;
	}
}

