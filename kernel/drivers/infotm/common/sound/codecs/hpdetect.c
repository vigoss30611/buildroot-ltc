#include <linux/workqueue.h>
#include <linux/switch.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <asm/mach/irq.h>
#include <mach/pad.h>
#include <mach/hw_cfg.h>
#include <mach/audio.h>
#include "hpdetect.h"

#define TAG	"hpdetect: "

static struct hpdetect_priv hp_cfg;
static int hp_status = UNPLUG;

static void hpdetect_io_init(struct codec_cfg *cfg)
{
#ifdef CONFIG_INFOTM_AUTO_ENABLE_SPK
	gpio_direction_output(cfg->spk_io, 0);
#else
	gpio_direction_output(cfg->spk_io, 1);
#endif
	/*gpio_direction_input(cfg->hp_io);*/
	/*imapx_pad_set_pull(cfg->hp_io, "float");*/
}

static void hpdetect_work(struct work_struct *work)
{
	struct hpdetect_priv *hp =
		container_of(work, struct hpdetect_priv, work.work);
	struct codec_cfg *cfg = hp->platform;
	uint32_t val;

	msleep(250);
	val = gpio_get_value(cfg->hp_io);
	if (cfg->highlevel > 0)
		val = !val;

	if (val)
		hp_status = UNPLUG;
	else
		hp_status = PLUG;

	switch_set_state(&hp_cfg.hp_dev, hp_status);
	enable_irq(hp_cfg.irq);
}

static void hpdetect_check(int ticks)
{
	schedule_delayed_work(&hp_cfg.work, ticks);
}

static irqreturn_t hpdetect_isr(int irq,void *dev_id)
{
	disable_irq_nosync(hp_cfg.irq);
	hpdetect_check(msecs_to_jiffies(100));
	
	return IRQ_HANDLED;
}

static ssize_t print_switch_name(struct switch_dev *sdev, char *buf)
{
	return sprintf(buf, "%s\n", "h2w");
}

static ssize_t print_switch_state(struct switch_dev *sdev, char *buf)
{
	return sprintf(buf, "%d\n", hp_status);
}

static void hpdetect_event_init(void)
{
	struct switch_dev *dev = &hp_cfg.hp_dev;

	dev->name = "h2w";
	dev->print_name = print_switch_name;
	dev->print_state = print_switch_state;
	switch_dev_register(dev);
}

void hpdetect_spk_on(int en)
{
	struct codec_cfg *cfg = hp_cfg.platform;
	
	if (!cfg)
		return;
	/* if hp have inserted, we need close PA */	
	if (hp_status == PLUG)
		en = 0;
	/* if audio don't playback, we need close PA */
//	if (!imapx_audio_get_state())
//		en = 0;
//
	if (en)	
		printk("Hpdetect spk set %d\n", en);
	if (cfg->spk_io >= 0)
		gpio_set_value(cfg->spk_io, en);
}

void hpdetect_suspend(void)
{
    if (!hp_cfg.platform)
        return;
    disable_irq(hp_cfg.irq);
}

void hpdetect_resume(void)
{
	if (!hp_cfg.platform)
		return;
	hpdetect_io_init(hp_cfg.platform);
	irq_set_irq_type(hp_cfg.irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING);
	hpdetect_check(msecs_to_jiffies(1000));
}

int hpdetect_init(struct codec_cfg *cfg)
{
	int err;
	
	memset(&hp_cfg, 0, sizeof(struct hpdetect_priv));

	if (!cfg) {
		pr_err(TAG"cfg data miss\n");
		return -EINVAL;
	}

	if (cfg->spk_io < 0) {
		pr_err(TAG"maybe need another mode\n");
		return -EINVAL;
	}

	if (gpio_is_valid(cfg->spk_io)) {
		err = gpio_request(cfg->spk_io, "spk_io");
		if (err) {
			pr_err("failed request gpio for spk_io\n");
			goto err_request_spk;
		}
	}

	/*if (gpio_is_valid(cfg->hp_io)) {
		err = gpio_request(cfg->hp_io, "hp_io");
		if (err) {
			pr_err("failed request gpio for hp_io\n");
			goto err_request_hp;
		}
	}*/
	hp_cfg.platform = cfg;
	hpdetect_event_init();	
	/* for delayed enable hpdetect */
	/*INIT_DELAYED_WORK(&hp_cfg.work, hpdetect_work);*/
	//hp_cfg.irq = imapx_pad_irq_number(cfg->hp_io);
	/*hp_cfg.irq = gpio_to_irq(cfg->hp_io);*/

	/*err = request_irq(hp_cfg.irq, hpdetect_isr,
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
				"hp detect", &hp_cfg);
	if (err) {
		pr_err(TAG"request irq err\n");
		return -EINVAL;
	}*/

	/*disable_irq(hp_cfg.irq);*/
	hpdetect_io_init(cfg);
	/*hpdetect_check(msecs_to_jiffies(1000));*/
	return 0;

err_request_hp:
	if (gpio_is_valid(cfg->spk_io))
		gpio_free(cfg->spk_io);
err_request_spk:
	return err;
}
