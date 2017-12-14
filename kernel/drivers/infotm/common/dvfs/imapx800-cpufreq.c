#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/suspend.h>
#include <asm/smp_plat.h>
#include <asm/cpu.h>     
#include <linux/reboot.h>

#include <asm/system.h>

#include <mach/hardware.h>
#include <mach/items.h>
#include <mach/imap-pwm.h>
#include "cpu_fit.h"

//#define CONFIG_CFSHOW_TIME
//#define CPUFREQ_BY_CHANGE_RATIO
#define END_TABLE       -1
#define DEFAULT_FREQ    -1

struct cpufreq_ratio_table {
    unsigned int ratio;
    unsigned int freq;
};

struct imapx910_dvfs_data {
	struct cpufreq_policy *policy;
	struct notifier_block       pm_nb;
	struct notifier_block       reboot_nb;
};

#ifdef CPUFREQ_BY_CHANGE_RATIO
static struct cpufreq_ratio_table imapx800_ratio_table[] = {
    {0x00010306, DEFAULT_FREQ},
    {0x0002060c, DEFAULT_FREQ},
    {0x00030912, DEFAULT_FREQ},
    {0x00040c18, DEFAULT_FREQ},
    {0x00050f1e, DEFAULT_FREQ},
    {0x00061224, DEFAULT_FREQ},
#if 1
    {0x0007152a, DEFAULT_FREQ}, 
    {0x00081830, DEFAULT_FREQ},
    {0x00091b36, DEFAULT_FREQ},
    {0x000a1e3c, DEFAULT_FREQ},
    {0x000b162c, DEFAULT_FREQ},
    {0x000c1830, DEFAULT_FREQ},
    {0x000d1a34, DEFAULT_FREQ},
    {0x000e1c38, DEFAULT_FREQ},
    {0x000f1e3c, DEFAULT_FREQ},
    {0x00102040, DEFAULT_FREQ},
#endif
    {END_TABLE, DEFAULT_FREQ},
};

static struct clk *cpu_clk;
#endif

static int cpufreq_trust_lvl = 14, cpufreq_bench_lvl = 4,
	   cpufreq_top_lvl = 14, // set 804M as our default value
	   cpufreq_fix_lvl = -1, cpufreq_boot_lvl = 14;

static int cpufreq_percpu_trust_freq[4];
int cpufreq_config = 0;

static struct cpufreq_frequency_table imapx800_freq_table[] = {
	{ 0, 1704000},
	{ 1, 1656000},
	{ 2, 1608000},
	{ 3, 1560000},
	{ 4, 1512000},
	{ 5, 1464000},
	{ 6, 1416000},
	{ 7, 1368000},
	{ 8, 1320000},
	{ 9, 1272000},
	{ 10, 1224000},
	{ 11, 1176000},
	{ 12, 1128000},
	{ 13, 1080000},
	{ 14, 1032000},
	{ 15, 996000 },
	{ 16, 948000 },
	{ 17, 900000 },
	{ 18, 852000 },
	{ 19, 804000 },
	{ 20, 756000 },
	{ 21, 696000 },
	{ 22, 660000 },
	{ 23, 612000 },
	{ 24, 564000 },
	{ 25, 516000 },
	{ 26, 468000 },
	{ 27, 420000 },
	{ 28, 372000 },
	{ 29, 324000 },
	{ 30, 276000 },
	{ 31, 228000 },
	{ 32, 180000 },
	{ 33, 132000 },
	{ 34, CPUFREQ_TABLE_END },
};

static struct clk *apll_clk;

static unsigned long target_cpu_speed[NR_CPUS];
static DEFINE_MUTEX(imapx800_cpu_lock);
static bool is_suspended;
bool is_rebooting =  false;

void imapx800_lvl_up(void) {
	printk(KERN_ERR "top lvl increased.\n");
	cpufreq_top_lvl = cpufreq_bench_lvl;
}
void imapx800_lvl_down(void) {
	printk(KERN_ERR "top lvl decreased.\n");
	cpufreq_top_lvl = cpufreq_trust_lvl;
}
EXPORT_SYMBOL(imapx800_lvl_up);
EXPORT_SYMBOL(imapx800_lvl_down);

void imapx800_freq_message(int m, int v)
{
	static int a[4] = {0, 0, 0, 0};

	if(m < 0 || m > 3) return ;
	a[m] = v;
	printk(KERN_DEBUG "f%d: %4d %4d %4d\n",
			raw_smp_processor_id(), a[0], a[1], a[2]);
}
EXPORT_SYMBOL(imapx800_freq_message);

void imapx800_cpufreq_fix(int lvl)
{
	if(lvl < 0) lvl = -1;
	else {
		if(lvl > 19) lvl = 19;
		if(lvl < cpufreq_top_lvl) lvl = cpufreq_top_lvl;
	}

	cpufreq_fix_lvl = lvl;
}
EXPORT_SYMBOL(imapx800_cpufreq_fix);
void imapx800_cpufreq_setlvl(int lvl)
{
	if(lvl > 19) lvl = 19;
	if(lvl < 0) imapx800_cpufreq_fix(-1);
	else imapx800_cpufreq_fix(19 - lvl);
}
EXPORT_SYMBOL(imapx800_cpufreq_setlvl);

int imapx800_cpufreq_getval(void)
{
	return (clk_get_rate(apll_clk) / 1000000);
}

int imapx800_verify_speed(struct cpufreq_policy *policy)
{
	if(item_exist("soc.cpu.fit")) {
		return cpufreq_frequency_table_verify(policy, 
					get_cpu_freq_table(policy->cpu));
	} else {	
		return cpufreq_frequency_table_verify(policy, 
					imapx800_freq_table);
	}
}

unsigned int imapx800_getspeed(unsigned int cpu)
{
	if (cpu >= NR_CPUS)
		return 0;
#ifdef CPUFREQ_BY_CHANGE_RATIO
        return (clk_get_rate(cpu_clk) / 1000);
#else
	return (clk_get_rate(apll_clk) / 1000);
#endif
}

#ifdef CONFIG_CFSHOW_TIME
static long long __getns(void)
{
	ktime_t a;
	a = ktime_get_boottime();
	return a.tv64;
}
#endif

#ifdef CPUFREQ_BY_CHANGE_RATIO
static struct cpufreq_ratio_table *imapx800_get_ratio(
        unsigned long rate)
{
        struct cpufreq_ratio_table *ratio = imapx800_ratio_table;
        
        if (rate < 0)
            return NULL;

        while (ratio->ratio != END_TABLE) {
                if (ratio->freq <= rate)
                    return ratio;
                ratio++;
        }
        ratio--;
        return ratio;
}

static int imapx800_update_cpu_ratio(unsigned long rate)
{
        int ret = 0;
        struct cpufreq_freqs freqs;
        unsigned long flags;
        unsigned int cpu_rate = clk_get_rate(cpu_clk) / 1000;
        struct cpufreq_ratio_table *ratio = imapx800_ratio_table;
#ifdef CONFIG_CFSHOW_TIME
        long long a, b;

        a = __getns();
#endif 
        ratio = imapx800_get_ratio(cpu_rate);
        freqs.old = ratio->freq;
        ratio = imapx800_get_ratio(rate);
        freqs.new = ratio->freq;

        if (freqs.old == freqs.new)
            return ret;
        
        for_each_online_cpu(freqs.cpu)
            cpufreq_notify_transition(&freqs, CPUFREQ_PRECHANGE);
        
#ifndef CONFIG_CPU_IMAPX910
        pwm_cpufreq_prechange(freqs.new*1000);
#endif

        ret = clk_set_rate(cpu_clk, ratio->ratio);
#ifndef CONFIG_CPU_IMAPX910
        pwm_cpufreq_trans();
#endif

        if (ret < 0) {
            pr_err("cpu-imapx800: Failed to set cpu frequency to %d kHz\n",
                    freqs.new);
            goto err;
        }

        ret = 0;
err:
        for_each_online_cpu(freqs.cpu)
            cpufreq_notify_transition(&freqs, CPUFREQ_POSTCHANGE);

#ifdef CONFIG_CFSHOW_TIME
        b = __getns();

        a = b - a;
        do_div(a, 1000);

        printk(KERN_ERR "cpuf->%uM %lldus\n", freqs.new / 1000, a);
#else
        //      printk(KERN_DEBUG "cpuf->%uM\n", freqs.new / 1000);
        imapx800_freq_message(0, freqs.new / 1000);
#endif
        return ret;
}
#endif


unsigned int __cpu_highest_speed(void)
{
	unsigned long rate = 0;                                   
	int i;                                                    

	for_each_online_cpu(i)
		rate = max(rate, target_cpu_speed[i]);
	return rate;
}

/*
unsigned int __count_slow_cpus(unsigned long speed_limit)
{
	unsigned int cnt = 0;                      
	int i;                                     

	for_each_online_cpu(i)
		if (target_cpu_speed[i] <= speed_limit)
			cnt++;
	return cnt;
}
*/

static int imapx800_update_cpu_speed(struct cpufreq_policy *policy, 
			unsigned int rate, unsigned int cpu) 
{
	int ret = 0;
	int i, j;
	struct cpufreq_freqs freqs;
	unsigned long flags;
	int cpu_num = (num_online_cpus() == 4) ? 2 :                
		((num_online_cpus() == 3) ? 3: (num_online_cpus() - 1));

#ifdef CONFIG_CFSHOW_TIME
	long long a, b;
	
	a = __getns();
#endif

//	for_each_online_cpu(i) 
//			rate = max(rate, target_cpu_speed[i]);

	if(item_exist("soc.cpu.fit")) {
		if (cpufreq_fix_lvl > 0) {
			rate = imapx800_freq_table[cpufreq_fix_lvl].frequency;
		} else {
			rate = min(rate, cpufreq_percpu_trust_freq[cpu_num]);
		}

		ret = imapx_auto_dvfs_cpu_fit(policy, rate, cpu_num);
	} else {
		freqs.old = imapx800_getspeed(0);
		if(cpufreq_fix_lvl > 0)
			freqs.new = imapx800_freq_table[cpufreq_fix_lvl].frequency;
		else
			freqs.new = min(rate, imapx800_freq_table[cpufreq_top_lvl].frequency);
		if (freqs.old == freqs.new)
			return ret;

//		get_online_cpus();

		for_each_online_cpu(freqs.cpu)
			cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);

		for (i=0;imapx800_freq_table[i].frequency != CPUFREQ_TABLE_END;i++)
			if (imapx800_freq_table[i].frequency == freqs.new)
				break;

		if (i == ARRAY_SIZE(imapx800_freq_table)) {
			ret = -1;
			goto err;
		}

#ifndef CONFIG_CPU_IMAPX910
		pwm_cpufreq_prechange(imapx800_freq_table[i].frequency*1000);
#endif

		ret = clk_set_rate(apll_clk, imapx800_freq_table[i].frequency*1000);
#ifndef CONFIG_CPU_IMAPX910
		pwm_cpufreq_trans();
#endif

		if (ret < 0) {
			pr_err("cpu-imapx800: Failed to set cpu frequency to %d kHz\n",
					freqs.new);
			goto err;
		}
		ret = 0;
err:
		for_each_online_cpu(freqs.cpu)
			cpufreq_notify_transition(policy, &freqs, CPUFREQ_POSTCHANGE);

#if CONFIG_SMP
		/*
		 * Note that loops_per_jiffy is not updated on SMP systems in
		 * cpufreq driver. So, update the per-CPU loops_per_jiffy value
		 * on frequency transition. We need to update all dependent CPUs.
		 */
		for_each_possible_cpu(j) {
			per_cpu(cpu_data, j).loops_per_jiffy = loops_per_jiffy;
		}
#endif


		imapx800_freq_message(0, freqs.new / 1000);
//		put_online_cpus();
	}

#ifdef CONFIG_CFSHOW_TIME
	b = __getns();

	a = b - a;
	do_div(a, 1000);
	
	printk(KERN_ERR "cpuf->%uM %lldus\n", freqs.new / 1000, a);
#else
//	printk(KERN_DEBUG "cpuf->%uM\n", freqs.new / 1000);
#endif
    return ret;
}

static int imapx800_target(struct cpufreq_policy *policy,
		       unsigned int target_freq,
		       unsigned int relation)
{
	int idx;
	unsigned int freq;
	int ret = 0;
	struct cpufreq_frequency_table *table;
	struct cpufreq_policy *p;
	int i;

	if (is_suspended || is_rebooting) {
		return -EBUSY;
	}

	mutex_lock(&imapx800_cpu_lock);
	
	if (!strcmp(policy->governor->name, "performance")) {	//we do this on booting
		target_freq = imapx800_freq_table[cpufreq_boot_lvl].frequency;
	}

	for_each_online_cpu(i) {
		p = cpufreq_cpu_get(i);
		if (p){
			target_freq= max(target_freq, p->min);
			target_freq = min(target_freq, p->max);
			cpufreq_cpu_put(p);
		}
	}

	if(item_exist("soc.cpu.fit")) {
		table = get_cpu_freq_table(policy->cpu);
		cpufreq_frequency_table_target(policy, table, 
				target_freq, relation, &idx);
		freq = table[idx].frequency;
	} else {
		cpufreq_frequency_table_target(policy, imapx800_freq_table, 
				target_freq, relation, &idx);

		freq = imapx800_freq_table[idx].frequency;
	}

	target_cpu_speed[policy->cpu] = freq;
#ifdef CPUFREQ_BY_CHANGE_RATIO
        ret = imapx800_update_cpu_ratio(freq);
#else
	ret = imapx800_update_cpu_speed(policy, freq, policy->cpu);
#endif
out:
	mutex_unlock(&imapx800_cpu_lock);
	return ret;
}

static int imapx800_pm_notify(struct notifier_block *nb, unsigned long event,
	void *dummy)
{
	int i;
	struct imapx910_dvfs_data *data = container_of(nb, 
								struct imapx910_dvfs_data, pm_nb);

	mutex_lock(&imapx800_cpu_lock);
	if (event == PM_SUSPEND_PREPARE || event == PM_RESTORE_PREPARE) {
		is_suspended = true;
		imapx800_update_cpu_speed(data->policy, 
				imapx800_freq_table[cpufreq_top_lvl].frequency, 0);
	} else if (event == PM_POST_SUSPEND) {
		is_suspended = false;
	}
	mutex_unlock(&imapx800_cpu_lock);

	return NOTIFY_OK;
}

static int imapx800_reboot_notify(struct notifier_block *nb, 
		unsigned long priority, void * arg)
{
	struct imapx910_dvfs_data *data = container_of(nb, 
								struct imapx910_dvfs_data, reboot_nb);

	printk("%s rebooting\n",__func__);

	mutex_lock(&imapx800_cpu_lock);
	imapx800_update_cpu_speed(data->policy, 
			imapx800_freq_table[cpufreq_top_lvl].frequency, 0);
	is_rebooting = true;
	mutex_unlock(&imapx800_cpu_lock);

	return NOTIFY_OK;
}

/*
static struct notifier_block imapx800_cpu_pm_notifier = {
	.notifier_call = imapx800_pm_notify,
};
*/

#ifdef CPUFREQ_BY_CHANGE_RATIO
static void imapx800_set_apll_table(void)
{
        struct cpufreq_ratio_table *ratio;
        unsigned int apll_rate = imapx800_getspeed(0);
        int i;

        for (i=0;imapx800_freq_table[i].frequency != CPUFREQ_TABLE_END;i++)
            if (imapx800_freq_table[i].frequency == apll_rate)
                break;
        if (imapx800_freq_table[i].frequency == CPUFREQ_TABLE_END) {
            printk("apll do not match freq table: %d\n", apll_rate);
            return;
        }

        ratio = imapx800_ratio_table;
        for (;ratio->ratio != END_TABLE;i++)
        {
            imapx800_freq_table[i].index = i;
            imapx800_freq_table[i].frequency = ratio->freq;
            ratio++;
        }
        imapx800_freq_table[i].index = i;
        imapx800_freq_table[i].frequency = CPUFREQ_TABLE_END;

#if 1
        i = 0;
        printk("cpufreq apll table:\n");
        while (imapx800_freq_table[i].frequency != CPUFREQ_TABLE_END) {
            printk("index: 0x%x, freq: %d\n",
                    imapx800_freq_table[i].index, imapx800_freq_table[i].frequency);
            i++;
        }       

#endif
}
#endif

#ifdef CPUFREQ_BY_CHANGE_RATIO
static void imapx800_set_ratio_table(void)
{
        struct cpufreq_ratio_table *ratio;
        unsigned int last_ratio = 0, next_ratio;

        ratio = imapx800_ratio_table;
        while (ratio->ratio != END_TABLE) {
                if (ratio->freq != DEFAULT_FREQ)
                    continue;

                next_ratio = (ratio->ratio >> 16) & 0xff;
                if (next_ratio < 0 || next_ratio < last_ratio) {
                        printk("ratio table err: %d <==> %d\n", 
                                next_ratio, last_ratio);
                        return;
                }
                ratio->freq = imapx800_getspeed(0) / next_ratio;

                last_ratio = next_ratio;
                ratio++;
        }
#if 1
        ratio = imapx800_ratio_table;
        printk("cpufreq ratio table:\n");
        while (ratio->ratio != END_TABLE) {
            printk("ratio: 0x%x, freq: %d\n",
                    ratio->ratio, ratio->freq);
                ratio++;
        }
#endif
}
#endif

static int imapx800_cpu_init(struct cpufreq_policy *policy)
{
	int i = 0;
	struct cpufreq_frequency_table *table;
	struct imapx910_dvfs_data *data = NULL;

	if (policy->cpu >= NR_CPUS)
		return -EINVAL;

	data = kmalloc(sizeof(*data), GFP_KERNEL);
	if (!data) {
		return -ENOMEM;
	}

	apll_clk = clk_get(NULL, "apll");
	if (IS_ERR(apll_clk))
		return PTR_ERR(apll_clk);
	clk_prepare_enable(apll_clk);

	if(item_exist("soc.cpu.fit")) {
		imapx_init_percpu_freq(policy);
		/*
		table = get_cpu_freq_table(policy->cpu);
		cpufreq_percpu_trust_freq[policy->cpu] += table[0].frequency;
		for(i = 0; table[i].frequency
				!= CPUFREQ_TABLE_END; i++)
			if( cpufreq_percpu_trust_freq[policy->cpu] >= 
					table[i].frequency) {
				cpufreq_percpu_trust_freq[policy->cpu] = table[i].frequency;
				break;
			}
		*/

		cpufreq_frequency_table_cpuinfo(policy, 
				get_cpu_freq_table(policy->cpu));
		cpufreq_frequency_table_get_attr(get_cpu_freq_table(policy->cpu), 
				policy->cpu);
		policy->cur = imapx800_getspeed(policy->cpu);
	} else {
		apll_clk = clk_get(NULL, "apll");
		if (IS_ERR(apll_clk))
			return PTR_ERR(apll_clk);
		clk_prepare_enable(apll_clk);
#ifdef CPUFREQ_BY_CHANGE_RATIO
		cpu_clk = clk_get(NULL, "cpu-clk");
		if (IS_ERR(cpu_clk))
			return PTR_ERR(cpu_clk);
		clk_prepare_enable(cpu_clk);
		if (policy->cpu == 0) {
			imapx800_set_ratio_table();
			imapx800_set_apll_table();
		}
#endif
		cpufreq_frequency_table_cpuinfo(policy, imapx800_freq_table);
		cpufreq_frequency_table_get_attr(imapx800_freq_table, 
				policy->cpu);
		policy->cur = imapx800_getspeed(policy->cpu);
	}
	target_cpu_speed[policy->cpu] = policy->cur;

	/* FIXME: what's the actual transition time? */
	policy->cpuinfo.transition_latency = 100 * 1000;

//	policy->shared_type = CPUFREQ_SHARED_TYPE_ALL;
//	cpumask_copy(policy->related_cpus, cpu_online_mask);
	if (policy->cpu == 0) {
		data->policy = policy;
		data->pm_nb.notifier_call = &imapx800_pm_notify;
		register_pm_notifier(&data->pm_nb);
	//	data->reboot_nb.notifier_call = &imapx800_reboot_notify;
	//	register_reboot_notifier(&data->reboot_nb);
	//	register_pm_notifier(&imapx800_cpu_pm_notifier);
	}

	return 0;
}

static int imapx800_cpu_exit(struct cpufreq_policy *policy)
{
	if(item_exist("soc.cpu.fit")) {
		cpufreq_frequency_table_cpuinfo(policy, get_cpu_freq_table(policy->cpu));
	} else {
		cpufreq_frequency_table_cpuinfo(policy, imapx800_freq_table);
	}
	clk_disable_unprepare(apll_clk);
    clk_put(apll_clk);
#ifdef CPUFREQ_BY_CHANGE_RATIO
        clk_disable_unprepare(cpu_clk);
        clk_put(cpu_clk);
#endif
	return 0;
}

static struct freq_attr *imapx800_cpufreq_attr[] = {
	&cpufreq_freq_attr_scaling_available_freqs,
	NULL,
};

static struct cpufreq_driver imapx800_cpufreq_driver = {
	//.flags		= CPUFREQ_CONST_LOOPS,
	.verify		= imapx800_verify_speed,
	.target		= imapx800_target,
	.get		= imapx800_getspeed,
	.init		= imapx800_cpu_init,
	.exit		= imapx800_cpu_exit,
	.name		= "imapx800",
	.attr		= imapx800_cpufreq_attr,
};

extern void infotm_freq_init(void);
static int __init imapx800_cpufreq_init(void)
{
	int i, j;
	struct cpufreq_frequency_table *table;

	infotm_freq_init();
	if(item_exist("soc.cpu.freq")) {
		int f0 = item_integer("soc.cpu.freq", 0) * 1000,
		    f1 = item_integer("soc.cpu.freq", 1) * 1000, i;
		printk(KERN_ERR "CPU new frequency gain from item: %d.%d\n",
				f0 / 1000, f1 / 1000);


		if(item_exist("soc.cpu.fit")) {
			/*use the same lock in hotplug in case livelock*/
			if(!item_exist("soc.dvfs.autotest")) {
				imapx_cpu_fit_init(&imapx800_cpu_lock);
			}

			cpufreq_config = f0;
			for_each_online_cpu(i) {
				cpufreq_percpu_trust_freq[i] = f0;
				table = get_cpu_freq_table(i);
				cpufreq_percpu_trust_freq[i] += table[0].frequency;
				for(j = 0; table[j].frequency
						!= CPUFREQ_TABLE_END; j++)
					if( cpufreq_percpu_trust_freq[i] >= 
							table[j].frequency) {
						cpufreq_percpu_trust_freq[i] = table[j].frequency;
						break;
					}
			}
		}

		f0 += imapx800_freq_table[cpufreq_trust_lvl].frequency;
		f1 += imapx800_freq_table[cpufreq_trust_lvl].frequency;

		if(item_exist("soc.dvfs.autotest")) {
			if (imapx_auto_dvfs_init()) {
				printk("dvfs auto test init failed\n");
			}
		}

		for(i = 0; imapx800_freq_table[i].frequency
				!= CPUFREQ_TABLE_END; i++)
			if(f0 >= imapx800_freq_table[i].frequency) {
				cpufreq_trust_lvl = i;
				break;
			}

		for(i = 0; imapx800_freq_table[i].frequency
				!= CPUFREQ_TABLE_END; i++)
			if(f1 >= imapx800_freq_table[i].frequency) {
				cpufreq_bench_lvl = i;
				break;
			}

	} else printk(KERN_ERR "CPU scaling frequency: (default)\n");

	if(cpufreq_bench_lvl > cpufreq_trust_lvl)
		cpufreq_bench_lvl = cpufreq_trust_lvl;

	cpufreq_top_lvl = cpufreq_trust_lvl;
	printk(KERN_ERR "CPU scaling frequency: %d.%d\n", cpufreq_trust_lvl, cpufreq_bench_lvl);

    return cpufreq_register_driver(&imapx800_cpufreq_driver);
}
module_init(imapx800_cpufreq_init);

static void __exit imapx800_cpufreq_exit(void)
{
    cpufreq_unregister_driver(&imapx800_cpufreq_driver);
}
module_exit(imapx800_cpufreq_exit);

MODULE_AUTHOR("Sun");
MODULE_DESCRIPTION("cpufreq driver for imapx800");
MODULE_LICENSE("GPL");
