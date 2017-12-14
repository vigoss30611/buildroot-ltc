/*
 * An I2C driver for the Philips PCF8563 RTC
 * Copyright 2005-06 Tower Technologies
 *
 * Author: Alessandro Zummo <a.zummo@towertech.it>
 * Maintainers: http://www.nslu2-linux.org/
 *
 * based on the other drivers in this same directory.
 *
 * http://www.semiconductors.philips.com/acrobat/datasheets/PCF8563-04.pdf
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/i2c.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <mach/hw_cfg.h>

#define DRV_VERSION "0.4.3"

#define PCF8563_REG_ST1		0x00	/* status */
#define PCF8563_REG_ST2		0x01

#define PCF8563_REG_SC		0x02	/* datetime */
#define PCF8563_REG_MN		0x03
#define PCF8563_REG_HR		0x04
#define PCF8563_REG_DM		0x05
#define PCF8563_REG_DW		0x06
#define PCF8563_REG_MO		0x07
#define PCF8563_REG_YR		0x08

#define PCF8563_REG_AMN		0x09	/* alarm */
#define PCF8563_REG_AHR		0x0A
#define PCF8563_REG_ADM		0x0B
#define PCF8563_REG_ADW		0x0C

#define PCF8563_REG_CLKO	0x0D	/* clock out */
#define PCF8563_REG_TMRC	0x0E	/* timer control */
#define PCF8563_REG_TMR		0x0F	/* timer */

#define PCF8563_SC_LV		0x80	/* low voltage */
#define PCF8563_MO_C		0x80	/* century */

static struct i2c_driver pcf8563_driver;
static struct i2c_client *i2c_client;
static struct mutex bus_lock;

struct pcf8563 {
	struct rtc_device *rtc;
	/*
	 * The meaning of MO_C bit varies by the chip type.
	 * From PCF8563 datasheet: this bit is toggled when the years
	 * register overflows from 99 to 00
	 *   0 indicates the century is 20xx
	 *   1 indicates the century is 19xx
	 * From RTC8564 datasheet: this bit indicates change of
	 * century. When the year digit data overflows from 99 to 00,
	 * this bit is set. By presetting it to 0 while still in the
	 * 20th century, it will be set in year 2000, ...
	 * There seems no reliable way to know how the system use this
	 * bit.  So let's do it heuristically, assuming we are live in
	 * 1970...2069.
	 */
	/* 0: MO_C=1 means 19xx, otherwise MO_C=1 means 20xx */
	int c_polarity;
};

int pcf8563_i2c_transfer(struct i2c_msg *msgs, int num)
{
	int i, ret;

	for (i = 0; i < num; i++)
		msgs[i].addr = i2c_client->addr;

	ret = i2c_transfer(i2c_client->adapter, msgs, num);

	if (ret < 0) {
		mdelay(5);
		ret = i2c_transfer(i2c_client->adapter, msgs, num);
	}

	return ret;
}

static int pcf8563_i2c_write_bytes(uint8_t *data_buf, uint8_t length,
				   uint8_t reg_addr)
{
	uint8_t buffer[255];
	int ret = 0;
	struct i2c_msg msg;

	/* put reg_addr to the buffer[0] first */
	buffer[0] = reg_addr;
	/* copy other data to buffer+1 addree */
	memcpy(buffer + 1, data_buf, length);

	msg.flags = 0;
	msg.len = length + 1;
	msg.buf = buffer;

	mutex_lock(&bus_lock);
	if (pcf8563_i2c_transfer(&msg, 1) != 1) {
		dev_err(&i2c_client->dev, "%s: i2c_transfer failed\n",
			__func__);
		ret = 1;
	}
	mutex_unlock(&bus_lock);
	return ret;
}

static int pcf8563_i2c_read_bytes(uint8_t *data_buf, uint8_t length,
				  uint8_t reg_addr)
{
	struct i2c_msg msg[2];
	int ret = 0;

	if (data_buf == NULL || length == 0) {
		dev_err(&i2c_client->dev, "%s: Invalid parameters\n", __func__);
		return -EINVAL;
	}

	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &reg_addr;

	msg[1].flags = 1;
	msg[1].len = length;
	msg[1].buf = data_buf;

	mutex_lock(&bus_lock);
	if (pcf8563_i2c_transfer(msg, 2) != 2) {
		dev_err(&i2c_client->dev, "%s: i2c_transfer failed\n",
			__func__);
		ret = 1;
	}
	mutex_unlock(&bus_lock);

	return ret;
}

/*---------------------------------------------------------------------------*/
static int pcf8563_read_regs(uint8_t start_reg_index, uint8_t len,
			     uint8_t *reg_value)
{
	/*
	 * To see if the parameters are right
	 */
	if ((reg_value == NULL) | (len <= 0)) {
		dev_err(&i2c_client->dev,
			"paramater reg_value can't be NULL\n");
		return -EINVAL;
	}

	return pcf8563_i2c_read_bytes(reg_value, len, start_reg_index);
}

static int pcf8563_write_regs(uint8_t start_reg_index, uint8_t len,
			      uint8_t *reg_value)
{
	/*
	 * To see if the parameters are right
	 */
	if ((reg_value == NULL) || (len <= 0)) {
		dev_err(&i2c_client->dev, "bad paramaters\n");
		return -EINVAL;
	}

	return pcf8563_i2c_write_bytes(reg_value, len, start_reg_index);
}

int pcf8563_get_alarm_int_state(void)
{
	uint8_t val;

	pcf8563_read_regs(PCF8563_REG_ST2, 1, &val);

	return (val & 0x8) ? 1 : 0;
}
EXPORT_SYMBOL(pcf8563_get_alarm_int_state);

void pcf8563_clear_alarm_int(void)
{
	uint8_t val;

	pcf8563_read_regs(PCF8563_REG_ST2, 1, &val);
	val &= ~(0x1 << 3);
	pcf8563_write_regs(PCF8563_REG_ST2, 1, &val);

	return;
}
EXPORT_SYMBOL(pcf8563_clear_alarm_int);

int pcf8563_rtc_alarmirq(void)
{
	struct pcf8563 *pcf8563 = i2c_get_clientdata(i2c_client);

	rtc_update_irq(pcf8563->rtc, 1, RTC_AF | RTC_IRQF);

	return 0;
}
EXPORT_SYMBOL(pcf8563_rtc_alarmirq);

/*
 * In the routines that deal directly with the pcf8563 hardware, we use
 * rtc_time -- month 0-11, hour 0-23, yr = calendar year-epoch.
 */
static int pcf8563_get_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	struct pcf8563 *pcf8563 = i2c_get_clientdata(client);
	unsigned char buf[13] = { PCF8563_REG_ST1 };
	int err = 0;

	err = pcf8563_read_regs(PCF8563_REG_ST1, 13, buf);
	if (err != 0) {
		dev_err(&client->dev, "%s: read error\n", __func__);
		return -EIO;
	}

	if (buf[PCF8563_REG_SC] & PCF8563_SC_LV)
		dev_info(&client->dev,
			 "low voltage detected, date/time is not reliable.\n");

	dev_info(&client->dev,
		 "%s: raw data is st1=%02x, st2=%02x, sec=%02x, min=%02x, hr=%02x, "
		 "mday=%02x, wday=%02x, mon=%02x, year=%02x\n", __func__,
		 buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
		 buf[8]);

	tm->tm_sec = bcd2bin(buf[PCF8563_REG_SC] & 0x7F);
	tm->tm_min = bcd2bin(buf[PCF8563_REG_MN] & 0x7F);
	tm->tm_hour = bcd2bin(buf[PCF8563_REG_HR] & 0x3F);
	tm->tm_mday = bcd2bin(buf[PCF8563_REG_DM] & 0x3F);
	tm->tm_wday = buf[PCF8563_REG_DW] & 0x07;
	tm->tm_mon = bcd2bin(buf[PCF8563_REG_MO] & 0x1F) - 1;
	tm->tm_year = bcd2bin(buf[PCF8563_REG_YR]);
	if (tm->tm_year < 70)
		tm->tm_year += 100;	/* assume we are in 1970...2069 */
	/* detect the polarity heuristically. see note above. */
	pcf8563->c_polarity = (buf[PCF8563_REG_MO] & PCF8563_MO_C) ?
	    (tm->tm_year >= 100) : (tm->tm_year < 100);

	dev_info(&client->dev, "%s: tm is secs=%d, mins=%d, hours=%d, "
		 "mday=%d, mon=%d, year=%d, wday=%d\n",
		 __func__,
		 tm->tm_sec, tm->tm_min, tm->tm_hour,
		 tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_wday);

	/* the clock can give out invalid datetime, but we cannot return
	 * -EINVAL otherwise hwclock will refuse to set the time on bootup.
	 */
	if (rtc_valid_tm(tm) < 0)
		dev_err(&client->dev, "retrieved date/time is not valid.\n");

	return 0;
}

static int pcf8563_set_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	struct pcf8563 *pcf8563 = i2c_get_clientdata(client);
	int err = 0;
	unsigned char buf[9];

	dev_info(&client->dev, "%s: secs=%d, mins=%d, hours=%d, "
		 "mday=%d, mon=%d, year=%d, wday=%d\n",
		 __func__,
		 tm->tm_sec, tm->tm_min, tm->tm_hour,
		 tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_wday);

	/* hours, minutes and seconds */
	buf[PCF8563_REG_SC] = bin2bcd(tm->tm_sec);
	buf[PCF8563_REG_MN] = bin2bcd(tm->tm_min);
	buf[PCF8563_REG_HR] = bin2bcd(tm->tm_hour);

	buf[PCF8563_REG_DM] = bin2bcd(tm->tm_mday);

	/* month, 1 - 12 */
	buf[PCF8563_REG_MO] = bin2bcd(tm->tm_mon + 1);

	/* year and century */
	buf[PCF8563_REG_YR] = bin2bcd(tm->tm_year % 100);
	if (pcf8563->c_polarity ? (tm->tm_year >= 100) : (tm->tm_year < 100))
		buf[PCF8563_REG_MO] |= PCF8563_MO_C;

	buf[PCF8563_REG_DW] = tm->tm_wday & 0x07;

	dev_info(&client->dev,
		 "raw data to write: sec:%02x, min:%02x, hr:%02x,md:%02x,mon:%02x,year:%02x, wd:%02x",
		 buf[2], buf[3], buf[4], buf[5], buf[7], buf[8], buf[6]);

	err = pcf8563_write_regs(PCF8563_REG_SC, 7, &buf[2]);
	if (err != 0) {
		dev_err(&client->dev, "%s failed!\n", __func__);
		err = -EIO;
	}
#if 0
	/* write register's data */
	for (i = 0; i < 7; i++) {
		unsigned char data[2] = { PCF8563_REG_SC + i,
			buf[PCF8563_REG_SC + i]
		};

		err = i2c_master_send(client, data, sizeof(data));
		if (err != sizeof(data)) {
			dev_err(&client->dev,
				"%s: err=%d addr=%02x, data=%02x\n",
				__func__, err, data[0], data[1]);
			return -EIO;
		}
	};
#endif
	return err;
}

static int pcf8563_get_alarmtime(struct i2c_client *client,
				 struct rtc_wkalrm *alarm)
{
	struct rtc_time *alm_tm = &alarm->time;
	unsigned char buf[13] = { PCF8563_REG_ST1 };
	int err = 0;

	err = pcf8563_read_regs(PCF8563_REG_ST1, 13, buf);
	if (err != 0) {
		dev_err(&client->dev, "%s: read error\n", __func__);
		return -EIO;
	}

	if (buf[PCF8563_REG_SC] & PCF8563_SC_LV)
		dev_info(&client->dev,
			 "low voltage detected, date/time is not reliable.\n");

	dev_info(&client->dev,
		 "%s: raw data is st1=%02x, st2=%02x, sec=%02x, min=%02x, hr=%02x, "
		 "mday=%02x, wday=%02x, mon=%02x, year=%02x, alarm_min = %02x, alarm_hr = %02x, alarm_mday = %02x, alarm_wday = %02x\n",
		 __func__, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5],
		 buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12]);

	alm_tm->tm_sec = bcd2bin(buf[PCF8563_REG_SC] & 0x7F);
	alm_tm->tm_min = bcd2bin(buf[PCF8563_REG_AMN] & 0x7F);
	alm_tm->tm_hour = bcd2bin(buf[PCF8563_REG_AHR] & 0x3F);
	alm_tm->tm_mday = bcd2bin(buf[PCF8563_REG_ADM] & 0x3F);
	alm_tm->tm_wday = buf[PCF8563_REG_ADW] & 0x07;
	alm_tm->tm_mon = bcd2bin(buf[PCF8563_REG_MO] & 0x1F) - 1;
	alm_tm->tm_year = bcd2bin(buf[PCF8563_REG_YR]);

	dev_info(&client->dev, "%s: tm is secs=%d, mins=%d, hours=%d, "
		 "mday=%d, mon=%d, year=%d, wday=%d\n",
		 __func__,
		 alm_tm->tm_sec, alm_tm->tm_min, alm_tm->tm_hour,
		 alm_tm->tm_mday, alm_tm->tm_mon, alm_tm->tm_year,
		 alm_tm->tm_wday);

	/* the clock can give out invalid datetime, but we cannot return
	 * -EINVAL otherwise hwclock will refuse to set the time on bootup.
	 */
	if (rtc_valid_tm(alm_tm) < 0)
		dev_err(&client->dev, "retrieved date/time is not valid.\n");

	alarm->enabled = (buf[PCF8563_REG_ST2] & 0x2) ? 1 : 0;

	return 0;
}

static int pcf5863_rtc_setalarm(struct i2c_client *client,
				struct rtc_wkalrm *alrm)
{
	struct rtc_time *alarm_tm = &alrm->time;
	int err = 0;
	unsigned char buf[5];
	unsigned long time;

	rtc_tm_to_time(alarm_tm, &time);
	time += 59;
	rtc_time_to_tm(time, alarm_tm);

	dev_info(&client->dev, "%s: secs=%d, mins=%d, hours=%d,"
			"mday=%d, mon=%d, year=%d, wday=%d, alarm %s\n",
		 __func__,
		 alarm_tm->tm_sec, alarm_tm->tm_min, alarm_tm->tm_hour,
		 alarm_tm->tm_mday, alarm_tm->tm_mon, alarm_tm->tm_year,
		 alarm_tm->tm_wday, alrm->enabled ? "enabled" : "disabled");

	buf[0] = bin2bcd(alarm_tm->tm_min);
	buf[1] = bin2bcd(alarm_tm->tm_hour);
	buf[2] = bin2bcd(alarm_tm->tm_mday);
	buf[3] = alarm_tm->tm_wday & 0x07;
	dev_info(&client->dev,
		 "%s: raw data, amin = %02x, ahr = %02x, amday = %02x, awday = %02x\n",
		 __func__, buf[0], buf[1], buf[2], buf[3]);

	err = pcf8563_write_regs(PCF8563_REG_AMN, 4, buf);
	if (err != 0) {
		dev_err(&client->dev, "%s failed!\n", __func__);
		err = -EIO;
	}

	pcf8563_read_regs(PCF8563_REG_ST2, 1, &buf[4]);
	if (alrm->enabled)
		buf[4] |= (0x1 << 1);
	else
		buf[4] &= ~(0x1 << 1);
	err = pcf8563_write_regs(PCF8563_REG_ST2, 1, &buf[4]);

	return err;
}

static int pcf8563_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	return pcf8563_get_datetime(to_i2c_client(dev), tm);
}

static int pcf8563_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	return pcf8563_set_datetime(to_i2c_client(dev), tm);
}

static int pcf8563_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{

	return pcf8563_get_alarmtime(to_i2c_client(dev), alarm);
}

static int pcf8563_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	return pcf5863_rtc_setalarm(to_i2c_client(dev), alarm);
}

static const struct rtc_class_ops pcf8563_rtc_ops = {
	.read_time = pcf8563_rtc_read_time,
	.set_time = pcf8563_rtc_set_time,
	.read_alarm = pcf8563_rtc_read_alarm,
	.set_alarm = pcf8563_rtc_set_alarm,
};

static int __init pcf8563_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	struct pcf8563 *pcf8563;
	struct rtc_time tm;
	unsigned char val;
	int err = 0;

	dev_dbg(&client->dev, "%s\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	pcf8563 = kzalloc(sizeof(struct pcf8563), GFP_KERNEL);
	if (!pcf8563)
		return -ENOMEM;

	i2c_client = client;
	mutex_init(&bus_lock);

	dev_info(&client->dev, "chip found, driver version " DRV_VERSION "\n");

	i2c_set_clientdata(client, pcf8563);

	device_init_wakeup(&client->dev, 1);

	pcf8563->rtc = rtc_device_register(pcf8563_driver.driver.name,
					   &client->dev, &pcf8563_rtc_ops,
					   THIS_MODULE);

	if (IS_ERR(pcf8563->rtc)) {
		err = PTR_ERR(pcf8563->rtc);
		goto exit_kfree;
	}

	/* prevent to entering override mode */
	pcf8563_read_regs(PCF8563_REG_ST1, 1, &val);
	val &= ~0x8;
	pcf8563_write_regs(PCF8563_REG_ST1, 1, &val);

	/* disable timer interrupt and enable alarm interrupt */
	pcf8563_read_regs(PCF8563_REG_ST2, 1, &val);
	val &= ~0x1;
	val |= 0x2;
	pcf8563_write_regs(PCF8563_REG_ST2, 1, &val);
	pcf8563_read_regs(PCF8563_REG_ST2, 1, &val);
	pr_err("-----%s, reg st2 val is 0x%02x\n", __func__, val);

	/* clear irq */
	pcf8563_clear_alarm_int();

	pcf8563_rtc_read_time(&client->dev, &tm);
	if (rtc_valid_tm(&tm)) {
		/* invalid RTC time, set RTC to 2010/09/22 */
		pr_err("Invalid RTC time detected, setting to default.\n");
		rtc_time_to_tm(mktime(2013, 5, 10, 0, 0, 0), &tm);
		pcf8563_rtc_set_time(&client->dev, &tm);
	}

	return 0;

exit_kfree:
	kfree(pcf8563);

	return err;
}

static int pcf8563_remove(struct i2c_client *client)
{
	struct pcf8563 *pcf8563 = i2c_get_clientdata(client);

	if (pcf8563->rtc)
		rtc_device_unregister(pcf8563->rtc);

	kfree(pcf8563);

	return 0;
}

static const struct i2c_device_id pcf8563_id[] = {
	{"rtc-pcf8563", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, pcf8563_id);

static struct i2c_driver pcf8563_driver = {
	.driver = {
		   .name = "rtc-pcf8563",
		   .owner = THIS_MODULE,
		   },
	.probe = pcf8563_probe,
	.remove = pcf8563_remove,
	.id_table = pcf8563_id,
};

static int pcf8563_plat_probe(struct platform_device *pdev)
{
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct rtc_cfg *pdata = pdev->dev.platform_data;
#if 0
	if (strcmp(pdata->model_name, "pcf8563")) {
		pr_err("pcf8563 probe error,hw rtc is %s\n", pdata->model_name);
		return -1;
	}
#endif

	memset(&info, 0, sizeof(struct i2c_board_info));
	strcpy(info.type, "rtc-pcf8563");
	info.addr = 0x51;

	adapter = i2c_get_adapter(0);
	if (!adapter)
		pr_err("*******get_adapter error!\n");
	i2c_new_device(adapter, &info);

	return i2c_add_driver(&pcf8563_driver);
}

static int __exit pcf8563_plat_remove(struct platform_device *pdev)
{
	i2c_del_driver(&pcf8563_driver);

	return 0;
}

static struct platform_driver pcf8563_rtc_driver = {
	.driver = {
		   .name = "pcf8563-rtc",
		   .owner = THIS_MODULE,
		   },
	.probe = pcf8563_plat_probe,
	.remove = pcf8563_plat_remove,
};

static int __init pcf8563_init(void)
{
	return platform_driver_register(&pcf8563_rtc_driver);
}

static void __exit pcf8563_exit(void)
{
	platform_driver_unregister(&pcf8563_rtc_driver);
}

module_init(pcf8563_init);
module_exit(pcf8563_exit);

MODULE_AUTHOR("infotmic");
MODULE_DESCRIPTION("Philips PCF8563RTC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
