#ifndef __IMAPX_CPU_FIT_H
#define __IMAPX_CPU_FIT_H

#include "dvfs.h"

void imapx_set_cpu_dvfs_relation(struct imapx_dvfs_dev *dvfs, 
								unsigned int cpu);
int imapx_auto_dvfs_init(void);
struct cpufreq_frequency_table *get_cpu_freq_table(unsigned int cpu);
int imapx_init_percpu_freq(struct cpufreq_policy *policy);

unsigned int __cpu_highest_speed(void);
//unsigned int __count_slow_cpus(unsigned long speed_limit);
int imapx_auto_dvfs_cpu_fit(struct cpufreq_policy *policy, 
						unsigned int rate, int cpu);
int imapx_cpu_fit_init(struct mutex *lock);
void __cpu_auto_hotplug(unsigned long new, int top_freq);
void imapx_cpu_fit(int up, int cpu);

enum {                       
	IMAPX_CPU_SPEED_BALANCED,
	IMAPX_CPU_SPEED_BIASED,  
	IMAPX_CPU_SPEED_SKEWED,  
};                           

enum {                    
	HP_DISABLED = 0,
	HP_ENABLED,
	HP_NONE,        
	HP_DOWN,        
	HP_UP,          
};                        


#endif
