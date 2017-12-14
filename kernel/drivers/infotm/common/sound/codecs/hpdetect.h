#ifndef __HPDETECT_H__
#define __HPDETECT_H__
#include <linux/switch.h>

struct hpdetect_priv {
	struct delayed_work work;
	struct codec_cfg *platform;
	struct switch_dev hp_dev;
	int irq;
};

enum {
	UNPLUG = 0,
    PLUG_MIC,   //headset with mic
	PLUG,       //headset without mic
};

extern int hpdetect_init(struct codec_cfg *cfg);
extern void hpdetect_resume(void);
extern void hpdetect_suspend(void);
extern void hpdetect_spk_on(int en);
#endif
