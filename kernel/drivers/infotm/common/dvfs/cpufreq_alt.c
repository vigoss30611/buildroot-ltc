/*
 *  drivers/cpufreq/cpufreq_ondemand.c
 *
 *  Copyright (C)  2001 Russell King
 *            (C)  2003 Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>.
 *                      Jun Nakajima <jun.nakajima@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#if 0
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
#include <linux/ktime.h>
#include <linux/sched.h>
#endif

//extern int imapx800_vpufreq_getval(void);
#include <mach/belt.h>
#include <linux/types.h>
#include <linux/cpufreq.h>

#define __ALT_LOADING_N0    60
#define __ALT_LOADING_N     65
#define __ALT_LOADING_N1    80
#define __ALT_LOADING_HIGH  95
#define __ALT_WARP          90

#define __ALT_VPU_GAIN
#define __ALT_VPU_TOP       420000
#define __ALT_VPU_COEFFICIENT 1500

#define __ALT_BT_MIN        800000
extern int infotm_freq_arbi(int module, int freq);
int __alt_freqsc_algo(uint32_t f, uint32_t _1s,
		struct cpufreq_policy *p, int bias)
{
	uint32_t fcpu = 0, gain = 0, scene;
	char _h = '0';

	if((f > __ALT_LOADING_HIGH * p->cur) ||     // loading > 95%
		f > _1s + __ALT_WARP * 100000||     // 100M over average
		(belt_scene_get() & SCENE_GPS))     // GPS is running
	{
//		fcpu = p->cur + p->max / 2;
		fcpu = p->max;
		_h   = (f > __ALT_LOADING_HIGH * p->cur)? '%': '1';
	} else if((f > __ALT_LOADING_N0 * p->cur) &&
		(f < __ALT_LOADING_N1 * p->cur))   // normal loading
	{
		return 0;
	} else {                     // need to change
		fcpu = f / __ALT_LOADING_N;
		_h   = 'c';
	}

    scene = belt_scene_get();
    if(scene & SCENE_CTS)
        fcpu = SCENE_PL_GETC(scene)?
            SCENE_PL_GETC(scene) * 50000:
            fcpu;

#if defined(__ALT_VPU_GAIN)
	if(fcpu < __ALT_VPU_TOP) {     // low cpu frequency, add VPU gain
		gain = imapx800_vpufreq_getval() * (__ALT_VPU_TOP - fcpu) / __ALT_VPU_TOP;
		if(unlikely(gain)) {
			gain = gain * __ALT_VPU_COEFFICIENT;
			printk(KERN_ERR "v:%d: %03lu + %03lu\n", imapx800_vpufreq_getval(),
					fcpu / 1000, gain / 1000);

			fcpu += gain;
		}
	}
#endif

#if defined(__ALT_BT_MIN)
	if((belt_scene_get() & SCENE_BLUETOOTH) && fcpu < __ALT_BT_MIN)
    {
        fcpu = __ALT_BT_MIN;
    }
#endif

	if((p->cur == p->max && fcpu >= p->max) ||
	   (p->cur == p->min && fcpu <= p->min))
		return 0;

//	fcpu = infotm_freq_arbi(0, fcpu * 1000) / 1000;

	if (bias)
		powersave_bias_target(p, fcpu, CPUFREQ_RELATION_L);
	else
		__cpufreq_driver_target(p, fcpu, CPUFREQ_RELATION_H);

#if 0
	printk(KERN_ERR "%c: %3d|%3d|%3d|%4d+%3d\n", _h, f / 100000,
			_1s / 100000, p->cur / 1000, (gain?(fcpu - gain):fcpu) / 1000,
			gain / 1000);
#endif

	return 0;
}
EXPORT_SYMBOL(__alt_freqsc_algo);

