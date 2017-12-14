#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/io.h>
#include <linux/regulator/consumer.h>
#include <asm/cpu.h>
#include <mach/items.h>
#include <linux/reboot.h>
#include "cpu_fit.h"

//#define CPU_DVFS_DEBUG                                     
#ifdef CPU_DVFS_DEBUG                                      
#define cd_dbg(x...)  printk("imapx_percpu_dvfs:" x)          
#else                                                     
#define cd_dbg(x...)  do{} while(0)                       
#endif                                                    
                                                          
#define cd_err(x...) printk(KERN_ERR "imapx_percpu_dvfs:" x)  
#define cd_info(x...) printk(KERN_INFO "imapx_percpu_dvfs:" x)

static struct clk *apll_clk;
static int imapx_percpu_freq_num[4];
static int imapx_dvfs_auto_test = 0;
static int imapx_dvfs_auto_test_freq = 804000;
static int imapx_dvfs_auto_test_vol = 1200;
static struct regulator *cpu_reg;
static struct regulator *core_reg;
extern bool is_rebooting;
extern void cpufreq_ondemand_start_timer(void);
extern void cpufreq_ondemand_stop_timer(void);
extern int cpufreq_config;
int num_of_cpus = CONFIG_NR_CPUS;

static struct cpufreq_frequency_table dvfs_test_freq_table[] = {
	{0, 1608000},
	{1, 1560000},
	{2, 1512000},
	{3, 1464000},
	{4, 1416000},
	{5, 1368000},
	{6, 1320000},
	{7, 1272000},
	{8, 1224000},
	{9, 1176000},
	{10, 1128000},
	{11, 1080000},
	{12, 1032000},
	{13, 996000 },
	{14, 948000 },
	{15, 900000 },
	{16, 852000 },
	{17, 804000 },
	{18, 756000 },
	{19, 708000 },
	{20, 660000 },
	{21, 612000 },
	{22, 564000 },
	{23, 516000 },
	{24, 468000 },
	{25, 420000 },
	{26, 372000 },
	{27, 324000 },
	{28, 276000 },
	{29, 228000 },
	{30, 180000 },
	{31, 132000 },
	{32, CPUFREQ_TABLE_END },
};


/*
static int imapx_cpu0_millivolts[MAX_DVFS_FREQS] = { 1200, 1175,
	1150, 1125, 1125, 1125, 1125, 1125, 1125, 1125, 1125, 1100, 1100, 1075, 
	1075, 1050, 1025, 1000, 1000, 1000, 1000, 1000, 1025, 1025, 1025, 1025};
*/

#if defined(CONFIG_ARCH_Q3F)
static int imapx_cpu0_millivolts[MAX_DVFS_FREQS] = { 1100, 1100,
	1100, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050};

static struct cpufreq_frequency_table cpu0_freq_table[] = {
	{0, 708000 },
	{1, 660000 },
	{2, 612000 },
	{3, 564000 },
	{4, 516000 },
	{5, 468000 },
	{6, 420000 },
	{7, 372000 },
	{8, 324000 },
	{9, 276000 },
	{10, 228000 },
	{11, 180000 },
	{12, 132000 },
	{13, 84000 },
	{14, 36000 },
	{15, CPUFREQ_TABLE_END },
};
#endif

#if defined(CONFIG_ARCH_APOLLO3)
static int imapx_cpu0_millivolts[MAX_DVFS_FREQS] = { 1100, 1100,
	1100, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050};

static struct cpufreq_frequency_table cpu0_freq_table[] = {
	{0, 708000 },
	{1, 660000 },
	{2, 612000 },
	{3, 564000 },
	{4, 516000 },
	{5, 468000 },
	{6, 420000 },
	{7, 372000 },
	{8, 324000 },
	{9, 276000 },
	{10, 228000 },
	{11, 180000 },
	{12, 132000 },
	{13, 84000 },
	{14, 36000 },
	{15, CPUFREQ_TABLE_END },
};
#endif

#if defined(CONFIG_ARCH_APOLLO)
static int imapx_cpu0_millivolts[MAX_DVFS_FREQS] = { 1050, 1050,
	1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050, 1050,
	1050, 1050, 1050, 1050, 1050, 1050, 1050};

static struct cpufreq_frequency_table cpu0_freq_table[] = {
	{0, 1032000},
	{1, 996000 },
	{2, 948000 },
	{3, 900000 },
	{4, 852000 },
	{5, 804000 },
	{6, 756000 },
	{7, 708000 },
	{8, 660000 },
	{9, 612000 },
	{10, 564000 },
	{11, 516000 },
	{12, 468000 },
	{13, 420000 },
	{14, 372000 },
	{15, 324000 },
	{16, 276000 },
	{17, 228000 },
	{18, 180000 },
	{19, 132000 },
	{20, 84000 },
	{21, 36000 },
	{22, CPUFREQ_TABLE_END },
};
#endif

static int imapx_cpu1_millivolts[MAX_DVFS_FREQS] = { /*1250, 1225,*/ 1200, 1175,
1200, 1150, 1125, 1125, 1125, 1125, 1100, 1100, 1100, 1075, 1075, 1050, 1025,
1025, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
/*
static int imapx_cpu1_millivolts[MAX_DVFS_FREQS] = { 1250, 1250, 1250, 1250, 
	1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250,
1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250}; 
*/
static struct cpufreq_frequency_table cpu1_freq_table[] = {
//	{0, 1368000},
//	{1, 1320000},
//	{0, 1272000},
	{0, 1200000},
	{1, 1176000},
	{2, 1128000},
	{3, 1080000},
	{4, 1032000},
	{5, 996000 },
	{6, 948000 },
	{7, 900000 },
	{8, 852000 },
	{9, 804000 },
	{10, 756000 },
	{11, 708000 },
	{12, 660000 },
	{13, 612000 },
	{14, 564000 },
	{15, 516000 },
	{16, 468000 },
	{17, 420000 },
	{18, 372000 },
	{19, 324000 },
	{20, 276000 },
	{21, 228000 },
	{22, 180000 },
	{23, 132000 },
	{24, CPUFREQ_TABLE_END },
};

static int imapx_cpu3_millivolts[MAX_DVFS_FREQS] = {1150, 1150, 1150, 1125,
1125, 1125, 1125, 1125, 1125, 1125, 1125, 1100, 1100, 1100, 1100, 1000, 1125,
1025, 1000, 1000, 1000};
/*
static int imapx_cpu3_millivolts[MAX_DVFS_FREQS] = { 1300, 1300, 1300, 1300, 
1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 1300, 
1300, 1300, 1300, 1300};
*/
static struct cpufreq_frequency_table cpu3_freq_table[] = {
	{ 0, 1080000},
	{ 1, 1032000},
	{ 2, 996000 },
	{ 3, 948000 },
	{ 4, 900000 },
	{ 5, 852000 },
	{ 6, 804000 },
	{ 7, 756000 },
	{ 8, 708000 },
	{ 9, 660000 },
	{ 10, 612000 },
	{ 11, 564000 },
	{ 12, 516000 },
	{ 13, 468000 },
	{ 14, 420000 },
	{ 15, 372000 },
	{ 16, 324000 },
	{ 17, 276000 },
	{ 18, 228000 },
	{ 19, 180000 },
	{ 20, 132000 },
	{ 21, CPUFREQ_TABLE_END },
};

/*
static int imapx_cpu2_millivolts[MAX_DVFS_FREQS] = { 1150, 1125, 1100, 1075,
1050, 1025, 1000, 1000, 1000, 975, 975, 975, 975, 975, 975, 975,
975, 975, 975, 975};
*/
static int imapx_cpu2_millivolts[MAX_DVFS_FREQS] = { 1150, 1150, 1125, 1125,
1125, 1125, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 
1100, 1100, 1100, 1100, 1100};
static struct cpufreq_frequency_table cpu2_freq_table[] = {
	{ 0, 1032000},
	{ 1, 996000 },
	{ 2, 948000 },
	{ 3, 900000 },
	{ 4, 852000 },
	{ 5, 804000 },
	{ 6, 756000 },
	{ 7, 708000 },
	{ 8, 660000 },
	{ 9, 612000 },
	{ 10, 564000 },
	{ 11, 516000 },
	{ 12, 468000 },
	{ 13, 420000 },
	{ 14, 372000 },
	{ 15, 324000 },
	{ 16, 276000 },
	{ 17, 228000 },
	{ 18, 180000 },
	{ 19, 132000 },
	{ 20, CPUFREQ_TABLE_END },
};

static struct cpufreq_frequency_table *imapx_freq_table[] = {
	cpu0_freq_table,
	cpu1_freq_table,
	cpu2_freq_table,
	cpu3_freq_table,
};

static int *imap_cpu_millivolts_table[] = {
	imapx_cpu0_millivolts,
	imapx_cpu1_millivolts,
	imapx_cpu2_millivolts,
	imapx_cpu3_millivolts,
};

static int cur_rate = 0;
int in_hotplug = 0;

struct cpufreq_frequency_table *get_cpu_freq_table(unsigned int cpu)
{
	return imapx_freq_table[cpu];
}

void imapx_set_cpu_dvfs_relation(struct imapx_dvfs_dev *dvfs, unsigned int cpu)
{
	int i = 0;

	if (dvfs == NULL || cpu > CONFIG_NR_CPUS)
		return;

	for (i = 0; imapx_freq_table[cpu][i].frequency !=
			CPUFREQ_TABLE_END; i++)
		dvfs->freqs[i] = imapx_freq_table[cpu][i].frequency;

	dvfs->millivolts = imap_cpu_millivolts_table[cpu];
	dvfs->num_freq = i;
}

static struct cpufreq_policy *g_policy[CONFIG_NR_CPUS];
extern void imapx800_freq_message(int m, int v);

static int imapx_fit_cpufreq(struct cpufreq_policy *policy, 
			unsigned long rate, unsigned int cpu)
{
	int ret = 0;
	int i, j;
	int rate_1;
	struct cpufreq_freqs freqs;
	int flags;

	if (!imapx_dvfs_auto_test) {
		for_each_online_cpu(i) 
			rate = min(rate, imapx_freq_table[i][0].frequency);
	}

	if (!imapx_dvfs_auto_test) {
		freqs.new = rate;
	} else {
		if ((imapx_dvfs_auto_test_freq != 0) && 
				(imapx_dvfs_auto_test_vol != 0)) {
			freqs.new = imapx_dvfs_auto_test_freq;
		} else {
			freqs.new = rate;
		}
	}

	if (g_policy[policy->cpu] == NULL) {
		g_policy[policy->cpu] = policy;
	}

	if (!imapx_dvfs_auto_test) {
		for (i = 0; imapx_freq_table[cpu][i].frequency !=
				CPUFREQ_TABLE_END; i++)
			if (imapx_freq_table[cpu][i].frequency <= freqs.new)
				break;

		if (i == imapx_percpu_freq_num[cpu]) {
			ret = -EINVAL;
			goto err;
		}
	}

	freqs.new = imapx_freq_table[cpu][i].frequency;

	freqs.old = (clk_get_rate(apll_clk) / 1000);
	if ((freqs.old == freqs.new) && (!in_hotplug)) {
		return ret;
	}

//	get_online_cpus();

	for_each_online_cpu(freqs.cpu)
		cpufreq_notify_transition(policy, &freqs, CPUFREQ_PRECHANGE);

	if (!imapx_dvfs_auto_test) {
		cd_dbg("%s rate %uKHz\n",__func__, freqs.new);
		ret = clk_set_rate(apll_clk, freqs.new*1000);
	} else {
		ret = clk_set_rate(apll_clk, imapx_dvfs_auto_test_freq*1000);
	}
	if (ret < 0) {
		cd_err("%s: Failed to set cpu frequency to %d kHz\n",
				__func__, freqs.new);
		goto err;
	}

	if (!imapx_dvfs_auto_test) {
		cur_rate = freqs.new*1000;
	}

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
//	put_online_cpus();
	imapx800_freq_message(0, freqs.new);

err:

	return ret;
}

int imapx_auto_dvfs_cpu_fit(struct cpufreq_policy *policy, 
			unsigned int rate, int cpu)
{
	int ret;
	int top_freq = imapx_freq_table[cpu][0].frequency + cpufreq_config;

 	ret = imapx_fit_cpufreq(policy, rate, cpu);

#ifdef CONFIG_SMP
	if( ret != -1)
		__cpu_auto_hotplug(clk_get_rate(apll_clk) / 1000, top_freq);
#endif

	return ret;
}

int imapx_init_percpu_freq(struct cpufreq_policy *policy)
{
	int i = 0;
#if defined(CONFIG_ARCH_Q3F)
    apll_clk = clk_get(NULL, "apll");
#endif
#if defined(CONFIG_ARCH_APOLLO3) || defined(CONFIG_ARCH_APOLLO)
	apll_clk = clk_get_sys("apll", "apll");
#endif
	if (IS_ERR(apll_clk))
		return PTR_ERR(apll_clk);
	clk_prepare_enable(apll_clk);

	for (i = 0; imapx_freq_table[policy->cpu][i].frequency != 
			CPUFREQ_TABLE_END; i++);

	imapx_percpu_freq_num[policy->cpu] = i;	

	/*
	cpufreq_frequency_table_cpuinfo(policy, imapx_freq_table[policy->cpu]);
	cpufreq_frequency_table_get_attr(imapx_freq_table[policy->cpu], 
															policy->cpu);
	policy->cur = clk_get_rate(apll_clk) / 1000;
	*/
}

static int __set_cpu_vol(int millivolt)
{
	int ret;
	/*in uv*/
	int core_vol = 0;
	int cpu_vol = 0;

	core_vol = regulator_get_voltage(core_reg);
	cpu_vol = regulator_get_voltage(cpu_reg);
	if (abs(core_vol - cpu_vol) > 100 * 1000) {
		if (core_vol > cpu_vol) {
			core_vol -= 100 * 1000;
		} else {
			core_vol += 100 * 1000;
		}
	} else {
		core_vol = 0;
	}

	if (core_vol) {
		ret = regulator_set_voltage(core_reg, core_vol, 1300 * 1000);                      
		if(ret){                                                  
			cd_err("Failed to set regulator %s\n",core_reg);
			return ret;                                               
		}
	}

	ret = regulator_set_voltage(cpu_reg, millivolt * 1000, 1300 * 1000);                      
	if(ret){                                                  
		cd_err("Failed to set regulator %s\n",cpu_reg);
		return ret;                                               
	}                                                         

	return ret;
}

#ifdef CONFIG_SMP
extern int __cpufreq_governor(struct cpufreq_policy *policy,
		                    unsigned int event);
void imapx_dvfs_disable(void)
{
	int cpu_num = num_online_cpus();

	cpufreq_ondemand_stop_timer();
	imapx_fit_cpufreq(g_policy[0],                    
			imapx_freq_table[2][0].frequency + cpufreq_config, 0);     
	if (cpu_num < CONFIG_NR_CPUS) {                   
		switch (cpu_num) {                            
			case 1:                                   
				cpu_up(1);                            
				cpu_up(3);                            
				cpu_up(2);                            
				break;                                
			case 2:                                   
				cpu_up(3);                            
				cpu_up(2);                            
				break;                                
			case 3:                                   
				cpu_up(2);                        
				break;                            
			default :                                 
				break;                            

		}                                             
	}                                                 
}

void imapx_cpu_fit(int up, int cpu)
{
	unsigned int hotplug_freq;
	struct cpufreq_policy p;
	int i;
	int freq_num;

	cd_dbg("----%s %s cpu %d\n", __func__, up?"up":"down", cpu);

	if (is_rebooting == 1) {
		cd_err("system rebooting\n");
		return ;
	}

	in_hotplug = 1;
//	for_each_online_cpu(i) {
//	cpufreq_get_policy(&p, i);
//	__cpufreq_governor(g_policy[0], CPUFREQ_GOV_STOP);
	cpufreq_ondemand_stop_timer();
//	}

	imapx_fit_cpufreq(g_policy[0], 
			imapx_freq_table[2][0].frequency + cpufreq_config, 0);
	if (up == 0) {
		/*it is safe to cpu down*/
//		freq_num = ARRAY_SIZE(cpu0_freq_table);
//		imapx_fit_cpufreq(g_policy[0], 
//				imapx_freq_table[cpu][freq_num - 2].frequency, 0);
		cpu_down(cpu);
		num_of_cpus--;
	} else if (up == 1) {
		/*in case cpu frequecny too high*/
//		if (cur_rate > imapx_freq_table[cpu][0].frequency) {
//			hotplug_freq = imapx_freq_table[cpu][0].frequency;
//			in_hotplug = 1;
//			imapx_fit_cpufreq(g_policy[0], hotplug_freq, cpu);
//		}
		cpu_up(cpu);
		num_of_cpus++;
	}
	cpufreq_ondemand_start_timer();
//	__cpufreq_governor(g_policy[0], CPUFREQ_GOV_START);
	in_hotplug = 0;
	if ((num_of_cpus < 1) || (num_of_cpus > CONFIG_NR_CPUS))
		cd_err("cpu num error %d\n", num_of_cpus);

	cd_dbg("+++%s %s cpu %d\n", __func__, up?"up":"down", cpu);
}
#endif

static ssize_t
test_start_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%d\n", imapx_dvfs_auto_test);
}

	static ssize_t
test_start_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	int start;

	if (sscanf(buf, "%d", &start) != 1)
		    return -1;                     

	imapx_dvfs_auto_test = start;

	imapx_dvfs_auto_test_freq = 804000;
	__set_cpu_vol(1200);

	return count;
}

static ssize_t
test_freq_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%u\n", imapx_dvfs_auto_test_freq);
}

static ssize_t
test_freq_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	int freq;

	if (sscanf(buf, "%u", &freq) != 1)
		    return -1;                     

	printk("%s freq %uKHz \n",__func__,freq);
	if (imapx_dvfs_auto_test_freq < 132000 || 
			imapx_dvfs_auto_test_freq > 1608000) {
		imapx_dvfs_auto_test_freq =  804000;
		return -EINVAL;
	}

	imapx_dvfs_auto_test_freq = freq;
	return count;
}

static ssize_t
test_vol_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", imapx_dvfs_auto_test_vol);
}

static ssize_t
test_vol_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	int vol;

	if (sscanf(buf, "%d", &vol) != 1)
		    return -1;                     

	imapx_dvfs_auto_test_vol = vol;

	printk("%s vol %u\n",__func__,imapx_dvfs_auto_test_vol);
	if ((imapx_dvfs_auto_test_vol >= 925) && 
			(imapx_dvfs_auto_test_vol < 1300)) {
		__set_cpu_vol(imapx_dvfs_auto_test_vol);
	} else {
		return -EINVAL;
	}

	return count;
}

static struct kobj_attribute dvfs_auto_test_start = 
	__ATTR(test_start, 0644, test_start_show, test_start_store);
static struct kobj_attribute dvfs_auto_test_freq = 
	__ATTR(test_freq, 0644, test_freq_show, test_freq_store);
static struct kobj_attribute dvfs_auto_test_vol = 
	__ATTR(test_vol, 0644, test_vol_show, test_vol_store);

const struct attribute *dvfs_auto_attributes[] = {
	&dvfs_auto_test_start.attr,
	&dvfs_auto_test_freq.attr,
	&dvfs_auto_test_vol.attr,
	NULL,
};


static struct kobject *dvfs_auto_kobj;

int imapx_auto_dvfs_init(void)
{
	int ret = 0;
	int i, j;

	if(item_exist("soc.dvfs.autotest")) {
		dvfs_auto_kobj = kobject_create_and_add("imapx_dvfs_test", 
											kernel_kobj);    
		if (!dvfs_auto_kobj) {
			pr_err("imapx_dvfs_auto_test: \
					failed to create sysfs object");   
				return -ENOMEM;
		}                                                               

		if (sysfs_create_files(dvfs_auto_kobj, dvfs_auto_attributes)) {             
			pr_err("imapx_dvfs_auto_test: \
					failed to create sysfs interface");
			return -ENOMEM;
		}                                                               

		cpu_reg = regulator_get(NULL, "dc1");   
		if (IS_ERR(cpu_reg)) {                         
			    pr_err("imapx_dvfs_auto_test: \
						failed to connect regulator\n");
				return ret;
		}else {
			ret = regulator_enable(cpu_reg);                         
			if (ret < 0) {                                             
				pr_err("imapx_dvfs_auto_test: \
						failed on enabling regulator, err %d", ret);                            
					return ret;
			}                                                        
		}
		core_reg = regulator_get(NULL, "dc1");   
		if (IS_ERR(core_reg)) {                         
			    pr_err("imapx_dvfs_auto_test: \
						failed to connect regulator\n");
				return ret;
		}else {
			ret = regulator_enable(core_reg);                         
			if (ret < 0) {                                             
				pr_err("imapx_dvfs_auto_test: \
						failed on enabling regulator, err %d", ret);                            
					return ret;
			}                                                        
		}

	}

	return ret;
}
