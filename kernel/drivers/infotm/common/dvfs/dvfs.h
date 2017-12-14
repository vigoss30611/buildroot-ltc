#ifndef __DVFS_H
#define __DVFS_H

#define MHz 1000000
#define KHz 1000
#define MAX_DVFS_FREQS 50 

struct imapx_dvfs_rail {
	const char	*reg_id;
	int	max_millivolt;
	int	min_millivolt;	
	int boot_millivolt;
	struct list_head	attached_dev;
	struct regulator *reg;
	int millivolts;	/*current set voltage in millivolt*/
	int new_millivolts;	/*voltage to be set in millivolt*/
	int step;
	struct timer_list dvfs_timer;
	struct notifier_block   dvfs_notify;
	unsigned int	notify_list;
	int (*registr_dvfs_notifier)(struct notifier_block *nb, unsigned int data);
};

struct imapx_dvfs_dev {
	const char *name;
	unsigned long freqs[MAX_DVFS_FREQS];
	const int *millivolts;
	int delay;
	int cur_millivolt;
	int num_freq;
	int max_millivolt;
	int freq_mult;
	unsigned long cur_rate;
	struct list_head reg_node;
	struct imapx_dvfs_rail	*rail;
};

int imapx_dvfs_set_vol(const char *dvfs_name, unsigned long rate, int delay);

#endif
