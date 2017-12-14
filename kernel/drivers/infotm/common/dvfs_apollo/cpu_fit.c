#include <linux/kernel.h>       
#include <linux/module.h>       
#include <linux/types.h>        
#include <linux/sched.h>        
#include <linux/cpufreq.h>      
#include <linux/delay.h>        
#include <linux/err.h>          
#include <linux/io.h>           
#include <linux/cpu.h>          
#include <linux/seq_file.h>     
#include <linux/pm_qos.h>
#include <linux/kernel_stat.h>
#include "cpu_fit.h"

//#define CPU_FIT_DEBUG                           
#ifdef CPU_FIT_DEBUG                              
#define cf_dbg(x...)  printk("cpu_fit:" x)
#else                                          
#define cf_dbg(x...)  do{} while(0)          
#endif

#define cf_err(x...) printk(KERN_ERR "cpu_fit:" x)
#define cf_info(x...) printk(KERN_INFO "cpu_fit:" x)
                                           
//static DEFINE_MUTEX(cpu_lock);
static struct mutex *cpu_lock;
static struct workqueue_struct *hotplug_wq;
static struct delayed_work hotplug_work;   
static struct delayed_work hotplug_test_work;   
static unsigned long up_delay;
static unsigned long up0_delay;
static unsigned long down_delay;
#define UP_DELAY_MS	100
#define UP0_DELAY_MS	200
#define DOWN_DELAY_MS	1500
#define UP_RATIO	60
#define DOWN_RATIO	30

#define NR_FSHIFT   2

static int hp_stat;
static int max_cpus = 4;
static int min_cpus = 1;
static unsigned long last_change_time;
static unsigned int rt_profile[] = {
	/*      1,  2,  3,  4 - on-line cpus target */
	2, 9, 13, UINT_MAX
};
static unsigned int nr_run_hysteresis = 2;  /* 0.5 thread */ 
static unsigned int nr_run_last;                             

static int balance_level = 30;

static struct {                                                      
	cputime64_t time_up_total;                                       
	u64 last_update;                                                 
	unsigned int up_down_count;                                      
} hp_stats[CONFIG_NR_CPUS + 1]; /* Append LP CPU entry at the end */ 

static void hp_init_stats(void)                                      
{                                                                    
	int i;                                                           
	u64 cur_jiffies = get_jiffies_64();                              

	for (i = 0; i <= CONFIG_NR_CPUS; i++) {                          
		hp_stats[i].time_up_total = 0;                               
		hp_stats[i].last_update = cur_jiffies;                       

		hp_stats[i].up_down_count = 0;                               
		if ((i < nr_cpu_ids) && cpu_online(i))                   
			hp_stats[i].up_down_count = 1;                       
	}                                                                

}                                                                    

static void hp_stats_update(unsigned int cpu, bool up)               
{                                                                    
	u64 cur_jiffies = get_jiffies_64();                              
	bool was_up = hp_stats[cpu].up_down_count & 0x1;                 

	if (was_up)                                                      
		hp_stats[cpu].time_up_total =  hp_stats[cpu].time_up_total + 
					(cur_jiffies - hp_stats[cpu].last_update);

	if (was_up != up) {                                              
		hp_stats[cpu].up_down_count++;                               
		if ((hp_stats[cpu].up_down_count & 0x1) != up) {             
			/* FIXME: sysfs user space CPU control breaks stats */   
			cf_err("tegra hotplug stats out of sync with %s CPU%d",  
					(cpu < CONFIG_NR_CPUS) ? "G" : "LP",              
					(cpu < CONFIG_NR_CPUS) ?  cpu : 0);               
			hp_stats[cpu].up_down_count ^=  0x1;                     
		}                                                            
	}                                                                
	hp_stats[cpu].last_update = cur_jiffies;                         
}                                                                    

static int store_hotplug_state(struct kobject *a, struct attribute *b,
						const char *buf, size_t count)
{                                                                      
	int ret = 0;                                                       
	int old_state;                                                     
	int stat;

	if (!cpu_lock)                                              
		return ret;                                                    

	mutex_lock(cpu_lock);                                       

	old_state = hp_stat;
	ret = sscanf(buf, "%d", &stat);

	if (ret == 1) {
		hp_stat = stat;
		if ((hp_stat == HP_DISABLED) &&
				(old_state != HP_DISABLED)) {
			mutex_unlock(cpu_lock);
			cancel_delayed_work_sync(&hotplug_work);
			mutex_lock(cpu_lock);
			cf_info("auto-hotplug disabled\n");
		} else if (hp_stat != HP_DISABLED) {
			if (old_state == HP_DISABLED) {
				cf_info("auto-hotplug enabled\n");
				hp_init_stats();
			}
		}
	} else {
		cf_err("%s: unable to set hotplug state %d\n",
			__func__, stat);
		mutex_unlock(cpu_lock);
		return -EINVAL;
	}

	mutex_unlock(cpu_lock);
	return count;
}

static ssize_t show_hotplug_state(struct kobject *a,struct attribute *b,
							const char *buf, size_t count)
{
    return sprintf(buf, "%u\n", hp_stat);
}

static int store_min_cpu(struct kobject *a, struct attribute *b,
						const char *buf, size_t count)
{
	int ret;
	unsigned int nr_cpus = num_online_cpus();                            

	mutex_lock(cpu_lock);                                       
	ret = sscanf(buf, "%d", &min_cpus);
	if (ret == 1) {
		if (min_cpus > max_cpus) {
			cf_err("min cpu set error more than max %d\n",max_cpus);
			mutex_unlock(cpu_lock);
			return -EINVAL;
		}
		if (nr_cpus < min_cpus) {
			hp_stat = HP_UP;
			queue_delayed_work(
					hotplug_wq, &hotplug_work, 0);
		}
	} else {
		cf_err("unable to set min cpu\n");
		mutex_unlock(cpu_lock);
		return -EINVAL;
	}
	mutex_unlock(cpu_lock);
	return count;
}

static ssize_t show_min_cpu(struct kobject *a,struct attribute *b,
							const char *buf, size_t count)
{
    return sprintf(buf, "%u\n", min_cpus);
}

static int store_max_cpu(struct kobject *a, struct attribute *b,
						const char *buf, size_t count)
{
	int ret;
	unsigned int nr_cpus = num_online_cpus();                            
	
	mutex_lock(cpu_lock);                                       
	ret = sscanf(buf, "%d", &max_cpus);
	if (ret == 1) {
		if (max_cpus < min_cpus) {
			cf_err("max cpu set error less than min %d\n",min_cpus);
			mutex_unlock(cpu_lock);
			return -EINVAL;
		}
		if (nr_cpus > max_cpus) {
			hp_stat = HP_DOWN;
			queue_delayed_work(
					hotplug_wq, &hotplug_work, 0);
		}
	} else {
		cf_err("unable to set max cpu\n");
		mutex_unlock(cpu_lock);
		return -EINVAL;
	}
	mutex_unlock(cpu_lock);
	return count;
}

static ssize_t show_max_cpu(struct kobject *a,struct attribute *b,
							const char *buf, size_t count)
{
    return sprintf(buf, "%u\n", max_cpus);
}

define_one_global_rw(hotplug_state);
define_one_global_rw(min_cpu);
define_one_global_rw(max_cpu);

static struct attribute *hp_attributes[] = {
	&hotplug_state.attr,
	&max_cpu.attr,
	&min_cpu.attr,
	NULL
};

static struct attribute_group hp_attr_group = {
	.attrs = hp_attributes,
	.name = "cpu_fit",
};
extern unsigned int __cpu_highest_load(void);
extern unsigned int __count_slow_cpus(unsigned int load);

static noinline int __cpu_speed_balance(void)
{
	unsigned int highest_load = __cpu_highest_load();
	unsigned int balanced_load = highest_load * balance_level / 100;
	unsigned int skewed_load = balanced_load / 2;
	unsigned int nr_cpus = num_online_cpus();
	unsigned int avg_nr_run = avg_nr_running();
	unsigned int nr_run;

	for (nr_run = 1; nr_run < ARRAY_SIZE(rt_profile); nr_run++) {
		unsigned int nr_threshold = rt_profile[nr_run - 1];
//		if (nr_run_last <= nr_run)
//			nr_threshold += nr_run_hysteresis;
		if (avg_nr_run <= (nr_threshold << (FSHIFT - NR_FSHIFT)))
			break;
	}
	nr_run_last = nr_run;

	cf_dbg("%s skewed %d balanced %d nr_run %d nr_cpus %d max_cpus %d	\
			min_cpus %d avg_nr_run %d\n", __func__,
			__count_slow_cpus(skewed_load),
			__count_slow_cpus(balanced_load), nr_run, nr_cpus,
			max_cpus, min_cpus, avg_nr_run); 

	if (((__count_slow_cpus(skewed_load) >= 2) ||
				(nr_run < nr_cpus) ||
				(nr_cpus > max_cpus)) &&
			(nr_cpus > min_cpus)) {
		cf_dbg("%s cpu speed skewed\n",__func__);
		return IMAPX_CPU_SPEED_SKEWED;
	}

	if (((__count_slow_cpus(balanced_load) >= 1) ||
				(nr_run <= nr_cpus) ||
				(nr_cpus == max_cpus)) &&
			(nr_cpus >= min_cpus)) {
		cf_dbg("%s cpu speed biased\n",__func__);
		return IMAPX_CPU_SPEED_BIASED;
	}

	cf_dbg("%s cpu speed balanced\n",__func__);
	return IMAPX_CPU_SPEED_BALANCED;
}

static noinline int __get_nr_running(void)
{
	unsigned int nr_cpus = num_online_cpus();
	unsigned int avg_nr_run = avg_nr_running();
	unsigned int nr_run;

	for (nr_run = 1; nr_run < ARRAY_SIZE(rt_profile); nr_run++) {
		unsigned int nr_threshold = rt_profile[nr_run - 1];
//		if (nr_run_last <= nr_run)
//			nr_threshold += nr_run_hysteresis;
		if (avg_nr_run <= (nr_threshold << (FSHIFT - NR_FSHIFT)))
			break;
	}

	if (nr_run < nr_cpus)
		return 0;
	else
		return 1;
}

static int cpu_id = 1;
static int down = 1;
static void imapx_cpufit_test_work(struct work_struct *work)
{
	if (cpu_id > 4)
		cpu_id = 0;

	if (down) {
//		cpu_down(cpu_id);
		cpu_id++;
		if (cpu_id > 3)
			cpu_id = 1;
		down = 0;
	} else {
//		cpu_up(cpu_id);
		cpu_id++;
		if (cpu_id > 3)
			cpu_id = 1;
		down = 1;
	}

	queue_delayed_work(
			hotplug_wq, &hotplug_test_work, msecs_to_jiffies(500));
}

static int down_count = 0;

static void imapx_cpufit_work(struct work_struct *work)
{
	int up = -1;
	unsigned long now = jiffies;
	unsigned int cpu = nr_cpu_ids;
	int cpu_num = num_online_cpus();

	if (cpu_num < min_cpus) {
		hp_stat = HP_UP;
	} else if (cpu_num > max_cpus) {
		hp_stat = HP_DOWN;
	}

	if (down_count > 3)
		down_count = 0;

	__get_nr_running();
	mutex_lock(cpu_lock);
	switch(hp_stat) {
		case HP_NONE:
		case HP_UP:
			switch(__cpu_speed_balance()) {
				case IMAPX_CPU_SPEED_BALANCED:
					up = 1;
					down_count = 0;
					break;
				case IMAPX_CPU_SPEED_SKEWED:
					up = 0;
					down_count ++;
					break;
				case IMAPX_CPU_SPEED_BIASED:
				default:
					down_count = 0;
					break;
			}
			queue_delayed_work(                         
					hotplug_wq, &hotplug_work, up0_delay);
			break;
		case HP_DOWN:
			if (__get_nr_running()) {
				up = -1;
			} else {
				up = 0;
				down_count ++;
			}
			queue_delayed_work(                         
					hotplug_wq, &hotplug_work, up0_delay);
			break;
		default:
			cf_err("%s: invalid hp status %d\n",__func__, hp_stat);
			break;
	}

	if (up == 0) {                                              
		if (cpu_num == 1) {                                         
			cpu = nr_cpu_ids;
		} else if (min_cpus >= cpu_num){
			cpu = nr_cpu_ids;
		} else {
			cpu = ((cpu_num == 4)?2:(cpu_num == 3)?3:1);             
			cf_dbg("%s cpu%d->down\n",__func__,cpu);               
		}
	} else if (up == 1) {                                        
		if (cpu_num == 4) {                                         
			cpu = nr_cpu_ids;
		} else if (cpu_num >= max_cpus){
			cpu = nr_cpu_ids;
		} else {
			cpu = ((cpu_num == 3)?2:(cpu_num == 2)?3:1);             
			cf_dbg("%s cpu%d->up\n",__func__,cpu);                 
		}
	}                                                               

	if (((up == 0) && (((now - last_change_time) < down_delay) 
			|| down_count < 3)) 
			|| (up == -1)) {
		cpu = nr_cpu_ids;
	}

	if (cpu < nr_cpu_ids) {       
		last_change_time = now;   
	}                             

	mutex_unlock(cpu_lock);

	if (cpu < nr_cpu_ids) {       
		imapx_cpu_fit(up, cpu);
	}                             
}

//static int __cpu__freq_notify(struct notifier_block *nb,
//		unsigned long val, void *data)
void __cpu_auto_hotplug(unsigned long new, int top_freq)
{
//	struct cpufreq_freqs *freq = data;
//	struct cpufreq_frequency_table *table = 
//						get_cpu_freq_table(freq->cpu);
	struct cpufreq_freqs freq;
	struct cpufreq_frequency_table *table;

	if (hp_stat == HP_DISABLED)
		return;
	
	freq.new = new;
//	table = tab;

	cf_dbg("%s hp_stat %d freq->new %uKHztable_freq %uKHz\n", __func__,
			hp_stat, freq.new, top_freq);
	switch(hp_stat) {
		case HP_NONE:
		case HP_ENABLED:
			if (freq.new * 100 / top_freq >
				   	UP_RATIO) {
				hp_stat = HP_UP;
				queue_delayed_work(
						hotplug_wq, &hotplug_work, up_delay);
			} else if(freq.new * 100 / top_freq <
					DOWN_RATIO) {
				hp_stat = HP_DOWN;
				queue_delayed_work(
				        hotplug_wq, &hotplug_work, up_delay);
			}
			break;
		case HP_UP:
			if(freq.new * 100 / top_freq <
					DOWN_RATIO) {
				hp_stat = HP_DOWN;
				queue_delayed_work(
						hotplug_wq, &hotplug_work, up_delay);
			} else if (freq.new * 100 / top_freq <
					UP_RATIO) {
				hp_stat = HP_NONE;
			}
			break;
		case HP_DOWN:
			if (freq.new * 100 / top_freq >
					UP_RATIO) {
				hp_stat = HP_UP;
				queue_delayed_work(
						hotplug_wq, &hotplug_work, up_delay);
			} else if (freq.new * 100 / top_freq >
					DOWN_RATIO) {
				hp_stat = HP_NONE;
			}
			break;
		default:
			cf_err("%s: invalid hp status %d\n",__func__, hp_stat);
			break;
	}
}

/*
static struct notifier_block cpu_freq_notifier = {
	.notifier_call = __cpu__freq_notify,
};

static struct dentry *hp_debugfs_root;
                                       
struct pm_qos_request_list min_cpu_req;
struct pm_qos_request_list max_cpu_req;

static int min_cpus_get(void *data, u64 *val)
{
	*val = pm_qos_request(PM_QOS_MIN_ONLINE_CPUS);
	return 0;
}

static int min_cpus_set(void *data, u64 val)
{
	pm_qos_update_request(&min_cpu_req, (s32)val);
	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(min_cpus_fops, min_cpus_get, min_cpus_set, "%llu\n");

static int max_cpus_get(void *data, u64 *val)
{
	*val = pm_qos_request(PM_QOS_MAX_ONLINE_CPUS);
	return 0;
}

static int max_cpus_set(void *data, u64 val)                                 
{                                                                            
	pm_qos_update_request(&max_cpu_req, (s32)val);                           
	return 0;                                                                
}                                                                            
DEFINE_SIMPLE_ATTRIBUTE(max_cpus_fops, max_cpus_get, max_cpus_set, "%llu\n");

static int __cpu_fit_debug_init(void)               
{                                                                   
	hp_debugfs_root = debugfs_create_dir("imapx_cpu_fit", NULL);    
	if (!hp_debugfs_root)                                           
		return -ENOMEM;                                             

	pm_qos_add_request(&min_cpu_req, PM_QOS_MIN_ONLINE_CPUS,        
			PM_QOS_DEFAULT_VALUE);                               
	pm_qos_add_request(&max_cpu_req, PM_QOS_MAX_ONLINE_CPUS,        
			PM_QOS_DEFAULT_VALUE);                               

	if (!debugfs_create_file(                                       
				"min_cpus", S_IRUGO, hp_debugfs_root, NULL, &min_cpus_fops))
		goto err_out;                                               

	if (!debugfs_create_file(                                       
				"max_cpus", S_IRUGO, hp_debugfs_root, NULL, &max_cpus_fops))
		goto err_out;                                               

	if (!debugfs_create_file(                                       
				"stats", S_IRUGO, hp_debugfs_root, NULL, &hp_stats_fops))   
		goto err_out;                                               

	if (!debugfs_create_file(                                       
				"core_bias", S_IRUGO, hp_debugfs_root, NULL, &rt_bias_fops))
		goto err_out;                                               

	return 0;                                                       

err_out:                                                            
	debugfs_remove_recursive(hp_debugfs_root);                      
	pm_qos_remove_request(&min_cpu_req);                            
	pm_qos_remove_request(&max_cpu_req);                            
	return -ENOMEM;                                                 
}                                                                   
*/

int imapx_cpu_fit_init(struct mutex *lock)
{
	int ret = 0; 

	hotplug_wq = alloc_workqueue(                                  
			"cpu_fit", WQ_UNBOUND | __WQ_ORDERED | WQ_FREEZABLE, 1);  
	if (!hotplug_wq)                                               
		return -ENOMEM;                                            

	INIT_DELAYED_WORK(&hotplug_work, imapx_cpufit_work);
	INIT_DELAYED_WORK(&hotplug_test_work, imapx_cpufit_test_work);

	up_delay = msecs_to_jiffies(UP_DELAY_MS);                     
	down_delay = msecs_to_jiffies(DOWN_DELAY_MS);                       
	up0_delay = msecs_to_jiffies(UP0_DELAY_MS);

	cpu_lock = lock;

	hp_stat = HP_DISABLED;
	hp_init_stats();

	ret = sysfs_create_group(cpufreq_global_kobject,
			            &hp_attr_group);
	if (ret) {
		cf_err("%s create sysfs group failed\n",__func__);
		return ret;
	}

//	queue_delayed_work(
//			hotplug_wq, &hotplug_test_work, 0);
	/*
	if (cpufreq_register_notifier(&cpu_freq_notifier, 
				CPUFREQ_TRANSITION_NOTIFIER)) {
		cf_err("%s: Failed to register cpu freq notifier\n");
		return -EINVAL; 
	}
	*/

	/*
	ret = __cpu_fit_debug_init();
	if (ret) {
		cf_err("%s: Failed to create debug fs\n");
		return ret;
	}
	*/

	return ret;                                                           
}
