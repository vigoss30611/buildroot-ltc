/*****************************************************************************
** drivers/rtc/rtc-imapx800.c
**
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Description: RTC driver for iMAPx800
**
** Author:
**     Warits   <warits.wang@infotm.com>
**
** Revision History:
** -----------------
** 1.1  XXX 09/18/2009 XXX	Initialized
** 1.2  XXX 10/21/2010 XXX	Add PM shutdown function, set default if invalid
*****************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/clk.h>
#include <linux/log2.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/mach/time.h>
#include <mach/imap-rtc.h>
#include <mach/belt.h>
#include <mach/hw_cfg.h>

#define	RTCMSG(x...) pr_err("iMAP_RTC: " x)

#ifdef RTC_DEBUG
#define rtc_dbg(x...) pr_err("RTC_info:" x)
#else
#define rtc_dbg(x...)
#endif

/* I have yet to find an iMAP implementation with more than one
 * of these rtc blocks in */

#define IMAP_RTC_ACCESS_TM	0x01
#define IMAP_RTC_ACCESS_ALM	0x02
#define IMAP_RTC_ACCESS_READ	0x10
#define IMAP_RTC_ACCESS_WRITE	0x20

static struct resource *imapx_rtc_mem;

static void __iomem *imapx_rtc_base;
static int imapx_rtc_alarmno = NO_IRQ;
static struct rtc_device *imapx_rtc;
static int week;

/* FIXME: Workaround for RTC writing delay */
static unsigned long set_jiffies;
static struct rtc_time rtc_buffer;

void rtc_get(void __iomem *base, int *week, int *day, int *hour, int *min,
	     int *sec, int *msec)
{

	int total_sec = 0;
	unsigned int tmp0, tmp1, tmp2;

	tmp0 = readl(base + SYS_RTC_WEEKR0);
	tmp1 = readl(base + SYS_RTC_WEEKR1) & 0xf;
	*week = (tmp1 << 8) | tmp0;

	tmp0 = readl(base + SYS_RTC_SECR0);
	tmp1 = readl(base + SYS_RTC_SECR1);
	tmp2 = readl(base + SYS_RTC_SECR2) & 0xf;
	total_sec = (tmp2 << 16) | (tmp1 << 8) | tmp0;

	tmp0 = readl(base + SYS_RTC_MSECR0);
	tmp1 = readl(base + SYS_RTC_MSECR1) & 0x7f;
	*msec = (tmp1 << 8) | tmp0;

	*sec = total_sec % 60;
	*min = (total_sec % (60 * 60)) / 60;
	*hour = (total_sec % (60 * 60 * 24)) / (60 * 60);
	*day = (total_sec % (60 * 60 * 24 * 7)) / (60 * 60 * 24);

}

int read_rtc_gpx(int io_num)
{
	int err;
	err = readb(IO_ADDRESS(SYSMGR_RTC_BASE) + SYS_RTC_IO_ENABLE);
	err |= (0x1 << io_num);
	writel(err, IO_ADDRESS(SYSMGR_RTC_BASE) + SYS_RTC_IO_ENABLE);
	err = readb(IO_ADDRESS(SYSMGR_RTC_BASE) + SYS_RTC_IO_IN);
	err &= (0x1 << io_num);
	return err;
}
EXPORT_SYMBOL(read_rtc_gpx);

int rtc_pwrwarning_mask(int en)
{
	int err;
	if (en) {
		/* mask */
		err = readb(IO_ADDRESS(SYSMGR_RTC_BASE) + 0x60);
		err |= 0x01;
		writel(err, IO_ADDRESS(SYSMGR_RTC_BASE) + 0x60);
	} else {
		/* unmask */
		err = readb(IO_ADDRESS(SYSMGR_RTC_BASE) + 0x60);
		err &= 0xfe;
		writel(err, IO_ADDRESS(SYSMGR_RTC_BASE) + 0x60);
	}
	return 0;
}
EXPORT_SYMBOL(rtc_pwrwarning_mask);

#ifdef CONFIG_PM_SHUT_DOWN
static void RTCTIME(struct rtc_time *rtc_tm)
{
	rtc_dbg("%04d-%02d-%02d %02d:%02d:%02d\n",
		rtc_tm->tm_year, rtc_tm->tm_mon, rtc_tm->tm_mday,
		rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);
	return;
}
#endif

#if 1
void __imapx_rtc_access_debug(struct rtc_time *tm)
{
	void __iomem *base = imapx_rtc_base;
	int msec;
	rtc_get(base, &week, &(tm->tm_wday), &(tm->tm_hour), &(tm->tm_min),
		&(tm->tm_sec), &msec);

}
EXPORT_SYMBOL(__imapx_rtc_access_debug);
#endif

u32 rd_get_time_sec(void)
{
	u32 tmp0, tmp1, tmp2, total_sec;

	void __iomem *base = imapx_rtc_base;

	tmp0 = readl(base + SYS_RTC_SECR0);
	tmp1 = readl(base + SYS_RTC_SECR1);
	tmp2 = readl(base + SYS_RTC_SECR2) & 0xf;
	total_sec = (tmp2 << 16) | (tmp1 << 8) | tmp0;

	return total_sec;
}
EXPORT_SYMBOL(rd_get_time_sec);

/* RTC common Functions for iMAP API */

/*!
 ***********************************************************************
 * -Function:
 *    __imapx_rtc_aie(uint on)
 *
 * -Description:
 *    Turn on/off alarm enable bit.
 *
 * -Input Param
 *    on		Wether enable alarm
 *
 * -Output Param
 *	  None
 *
 * -Return
 *    None
 *
 * -Others
 *    None
 ***********************************************************************
 */
static void __imapx_rtc_aie(uint on)
{
	rtc_dbg("%s: aie=%d\n", __func__, on);
	writeb(!!on, imapx_rtc_base + SYS_ALARM_WEN);
}

void rtc_info_set(int num, int val)
{
	int reg_addr;
	void __iomem *base = imapx_rtc_base;
	rtc_dbg("%s, num = %d, val = %d(0x%x)\n", __func__, num, val, val);
	if ((num >= 0) && (num < 8)) {
		reg_addr = SYS_INFO_0 + (num * 4);
		writel(val, base + reg_addr);
	}
}
EXPORT_SYMBOL(rtc_info_set);

int rtc_info_get(int num)
{
	int reg_addr, reg_data = 0;
	void __iomem *base = imapx_rtc_base;

	if ((num >= 0) && (num < 8)) {
		reg_addr = SYS_INFO_0 + (num * 4);
		reg_data = readl(base + reg_addr);
	}
	rtc_dbg("%s, num = %d, val = %d(0x%x)\n", __func__, num, reg_data,
		reg_data);

	return (int)reg_data;
}
EXPORT_SYMBOL(rtc_info_get);

void rtc_set(void __iomem *base, int week, int day, int hour, int min, int sec,
	     int msec)
{
	int total_sec = 0;

	week = week % 4096;
	total_sec = (((day * 24 + hour) * 60 + min) * 60) + sec;

	writel((total_sec & 0xff), base + SYS_RTC_SEC0);
	writel(((total_sec & 0xff00) >> 8), base + SYS_RTC_SEC1);
	writel(((total_sec & 0xf0000) >> 16), base + SYS_RTC_SEC2);

	writel((week & 0xff), base + SYS_RTC_WEEK0);
	writel(((week & 0xf00) >> 8), base + SYS_RTC_WEEK1);

	writel((msec & 0xff), base + SYS_RTC_MSEC0);
	writel(((msec & 0x7f00) >> 8), base + SYS_RTC_MSEC1);

	writel(0x1, base + SYS_RTC_SECWEN);
	writel(0x1, base + SYS_RTC_WEEKWEN);
	writel(0x1, base + SYS_RTC_MSECWEN);
}



void rtc_alarm_set(void __iomem *base, int week, int day, int hour, int min,
		   int sec)
{
	int total_sec = 0;

	week = week % 4096;
	total_sec = (((day * 24 + hour) * 60 + min) * 60) + sec;

	writel((total_sec & 0xff), base + SYS_ALARM_SEC0);
	writel(((total_sec & 0xff00) >> 8), base + SYS_ALARM_SEC1);
	writel(((total_sec & 0xf0000) >> 16), base + SYS_ALARM_SEC2);

	writel((week & 0xff), base + SYS_ALARM_WEEK0);
	writel(((week & 0xf00) >> 8), base + SYS_ALARM_WEEK1);

	writel(0x1, base + SYS_ALARM_WEN);

}

void rtc_alarm_set_ext(u32 type, u32 data)
{
	void __iomem *base = imapx_rtc_base;

	if (type == 1) {
		writel((data & 0xff), base + SYS_ALARM_SEC0);
		writel(((data & 0xff00) >> 8), base + SYS_ALARM_SEC1);
		writel(((data & 0xff0000) >> 16), base + SYS_ALARM_SEC2);
	}

	if (type == 2) {
		writel((data & 0xff), base + SYS_ALARM_WEEK0);
		writel(((data & 0xff00) >> 8), base + SYS_ALARM_WEEK1);
	}
}
EXPORT_SYMBOL(rtc_alarm_set_ext);

void rtc_alarm_get(void __iomem *base, int *week, int *day, int *hour,
		   int *min, int *sec)
{
	int total_sec = 0;
	unsigned int tmp0, tmp1, tmp2;

	*week = (*week) % 4096;
	total_sec = ((((*day) * 24 + *hour) * 60 + *min) * 60) + *sec;

	tmp0 = readl(base + SYS_ALARM_SEC0);
	tmp1 = readl(base + SYS_ALARM_SEC1);
	tmp2 = readl(base + SYS_ALARM_SEC2);
	total_sec = (tmp2 << 16) | (tmp1 << 8) | tmp0;

	tmp0 = readl(base + SYS_ALARM_WEEK0);
	tmp1 = readl(base + SYS_ALARM_WEEK1);
	*week = (tmp1 << 8) | tmp0;

	*sec = total_sec % 60;
	*min = (total_sec % (60 * 60)) / 60;
	*hour = (total_sec % (60 * 60 * 24)) / (60 * 60);
	*day = (total_sec % (60 * 60 * 24 * 7)) / (60 * 60 * 24);

}

u32 rtc_alarm_get_ext(u32 type)
{
	u32 total_sec = 0;
	unsigned int tmp0 = 0, tmp1 = 0, tmp2 = 0;
	void __iomem *base = imapx_rtc_base;

	if (type == 1) {
		tmp0 = readl(base + SYS_ALARM_SEC0);
		tmp1 = readl(base + SYS_ALARM_SEC1);
		tmp2 = readl(base + SYS_ALARM_SEC2);
	}

	if (type == 2) {
		tmp0 = readl(base + SYS_ALARM_WEEK0);
		tmp1 = readl(base + SYS_ALARM_WEEK1);
		tmp2 = 0;
	}

	total_sec = (tmp2 << 16) | (tmp1 << 8) | tmp0;

	return total_sec;
}
EXPORT_SYMBOL(rtc_alarm_get_ext);
/*!
 ***********************************************************************
 * -Function:
 *    __imapx_rtc_access(struct rtc_time *tm, uint acc)
 *
 * -Description:
 *	  Access RTC registers to set or read time/alarm.
 *
 * -Input Param
 *	  tm, the structure to store time values.
 *	  acc,	access descriptor
 *	  0x00 write time
 *	  0x01 write alarm
 *	  0x10 read time
 *	  0x11 read alarm
 *
 * -Output Param
 *	tm, the structure to store time values.
 *
 * -Return
 *	None
 *
 * -Others
 *	None
 ***********************************************************************
 */
void __imapx_rtc_access(struct rtc_time *tm, uint acc)
{
	void __iomem *base = imapx_rtc_base;
	int msec;

	if (likely(acc & IMAP_RTC_ACCESS_READ)) {
		if (likely(acc & IMAP_RTC_ACCESS_TM)) {
			rtc_get(base, &week, &(tm->tm_wday), &(tm->tm_hour),
				&(tm->tm_min), &(tm->tm_sec), &msec);
		} else {
			rtc_alarm_get(base, &week, &(tm->tm_wday),
				      &(tm->tm_hour), &(tm->tm_min),
				      &(tm->tm_sec));
		}
	} else {		/* Write process */
		if (likely(acc & IMAP_RTC_ACCESS_TM)) {
			rtc_set(base, week, tm->tm_wday, tm->tm_hour,
				tm->tm_min, tm->tm_sec, 0);
		} else {
			rtc_alarm_set(base, week, tm->tm_wday, tm->tm_hour,
				      tm->tm_min, tm->tm_sec);
		}
	}
	return;
}

/* IRQ Handlers */
/*!
 ***********************************************************************
 * -Function:
 *    imapx_rtc_alarmirq(int irq, void *id)
 *
 * -Description:
 *    Interrupt handler when RTCALARM INT is generated.
 *
 * -Input Param
 *    irq		The irq number
 *    id		The private device pointer
 *
 * -Output Param
 *	  None
 *
 * -Return
 *    IRQ_HANDLED
 *
 * -Others
 *    None
 ***********************************************************************
 */
static irqreturn_t imapx_rtc_alarmirq(int irq, void *id)
{
	struct rtc_device *rdev = id;
	void __iomem *base = imapx_rtc_base;
	rtc_dbg("---%s invoked---\n", __func__);

	writel(0x80, base + SYS_INT_CLR);
	rtc_update_irq(rdev, 1, RTC_AF | RTC_IRQF);

	return IRQ_HANDLED;
}

static int imapx_rtc_aie(struct device *dev, unsigned int enabled)
{
	__imapx_rtc_aie(enabled);
	return 0;
}

/* Time read/write */
/*!
 ***********************************************************************
 * -Function:
 *    imapx_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm)
 *
 * -Description:
 *	  Read the current time from RTC registers.
 *	  The date and time value is store in BCD format in BCDSEC~BCDYEAR
 *
 * -Input Param
 *    dev		Standard function interface required, but not used here
 *
 * -Output Param
 *	  rtc_tm	This is a pointer to a time values container structure,
 *				which carries the read values.
 *
 * -Return
 *	  0 on success
 *
 * -Others
 *	  None
 ***********************************************************************
 */
int imapx_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm)
{
	unsigned long sum_sec;
	struct rtc_time tm;
	unsigned long sec_delta;
	rtc_dbg("---%s invoked, %p---\n", __func__, dev);

	if (set_jiffies && time_before(jiffies, set_jiffies + (1 * HZ))) {
		rtc_tm->tm_sec = rtc_buffer.tm_sec;
		rtc_tm->tm_min = rtc_buffer.tm_min;
		rtc_tm->tm_hour = rtc_buffer.tm_hour;
		rtc_tm->tm_mday = rtc_buffer.tm_mday;
		rtc_tm->tm_mon = rtc_buffer.tm_mon;
		rtc_tm->tm_year = rtc_buffer.tm_year;
	} else {
		week = 0;

		__imapx_rtc_access(rtc_tm,
				   IMAP_RTC_ACCESS_READ | IMAP_RTC_ACCESS_TM);

		/* the only way to work out wether the system war mid-update
		 * when we read it is to check the second counter, and if it
		 * is zero, then we re-try the entire read
		 */
		if (!rtc_tm->tm_sec)
			__imapx_rtc_access(rtc_tm,
					   IMAP_RTC_ACCESS_READ |
					   IMAP_RTC_ACCESS_TM);

		sec_delta = mktime(2007, 1, 1, 0, 0, 0);
		sum_sec =
		    ((((week * 7 + rtc_tm->tm_wday) * 24 +
		       rtc_tm->tm_hour) * 60 + rtc_tm->tm_min) * 60 +
		     rtc_tm->tm_sec) + sec_delta;
		rtc_time_to_tm(sum_sec, &tm);

		rtc_tm->tm_sec = tm.tm_sec;
		rtc_tm->tm_min = tm.tm_min;
		rtc_tm->tm_hour = tm.tm_hour;
		rtc_tm->tm_mday = tm.tm_mday;
		rtc_tm->tm_mon = tm.tm_mon;
		rtc_tm->tm_year = tm.tm_year;
	}

	rtc_dbg("rtc get time %04d-%02d-%02d  %02d:%02d:%02d\n",
		rtc_tm->tm_year + 1900, rtc_tm->tm_mon + 1, rtc_tm->tm_mday,
		rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);

	return 0;
}
EXPORT_SYMBOL(imapx_rtc_gettime);

/*!
 ***********************************************************************
 * -Function:
 *    imapx_rtc_settime(struct device *dev, struct rtc_time *tm)
 *
 * -Description:
 *	  Store the values in tm structure into RTC registers.
 *
 * -Input Param
 *    dev		Standard function interface required, but not used here
 *	  rtc_tm	This is a pointer to a time values container structure,
 *		values in this structure will be wrote into RTC regs.
 *
 * -Output Param
 *	  None
 *
 * -Return
 *	  0		    on success
 *	  -EINVAL	on failure
 *
 * -Others
 *	  None
 ***********************************************************************
 */
static int imapx_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	struct rtc_time ttm;
	unsigned long sum_sec;
	unsigned long sec_delta;
	rtc_dbg("---%s invoked, %p---\n", __func__, dev);

	memcpy(&ttm, tm, sizeof(struct rtc_time));

	if ((ttm.tm_year < 107) || (ttm.tm_year > 184)) {
		rtc_dbg("RTC settime out of range.\n");
		return -EINVAL;
	}

	rtc_dbg("rtc set time %04d-%02d-%02d  %02d:%02d:%02d\n",
		ttm.tm_year + 1900, ttm.tm_mon + 1, ttm.tm_mday,
		ttm.tm_hour, ttm.tm_min, ttm.tm_sec);

	week = 0;
	sec_delta = mktime(2007, 1, 1, 0, 0, 0);

	sum_sec =
	    mktime(ttm.tm_year + 1900, ttm.tm_mon + 1, ttm.tm_mday, ttm.tm_hour,
		   ttm.tm_min, ttm.tm_sec) - sec_delta;
	week = sum_sec / (60 * 60 * 24 * 7);
	sum_sec -= week * 60 * 60 * 24 * 7;
	ttm.tm_sec = sum_sec % 60;
	ttm.tm_min = (sum_sec % (60 * 60)) / 60;
	ttm.tm_hour = (sum_sec % (60 * 60 * 24)) / (60 * 60);
	ttm.tm_wday = sum_sec / (60 * 60 * 24);

	__imapx_rtc_access(&ttm, IMAP_RTC_ACCESS_WRITE | IMAP_RTC_ACCESS_TM);

	set_jiffies = jiffies;
	rtc_buffer.tm_sec = tm->tm_sec;
	rtc_buffer.tm_min = tm->tm_min;
	rtc_buffer.tm_hour = tm->tm_hour;
	rtc_buffer.tm_mday = tm->tm_mday;
	rtc_buffer.tm_wday = tm->tm_wday;
	rtc_buffer.tm_yday = tm->tm_yday;
	rtc_buffer.tm_mon = tm->tm_mon;
	rtc_buffer.tm_year = tm->tm_year;

	return 0;
}

/*!
 ***********************************************************************
 * -Function:
 *    imapx_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
 *
 * -Description:
 *	  Read the current alarm from RTC registers.
 *	  The date and time value is store in BCD format in ALMSEC~ALMYEAR
 *
 * -Input Param
 *    dev		Standard function interface required, but not used here
 *
 * -Output Param
 *	  alrm		Alarm structure, containning:
 *			enabled;	0 = alarm disabled, 1 = alarm enabled
 *			pending;	0 = alarm not pending, 1 = alarm pending
 *			time;		time the alarm is set to
 *	The read values is stored in alrm.time, if the relative
 *  alarm bit is not enabled, 0xff will be read.
 *
 * -Return
 *	  0 on success
 *
 * -Others
 *	  None
 ***********************************************************************
 */
static int imapx_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_time *alm_tm = &alrm->time, tm, ttm;
	void __iomem *base = imapx_rtc_base;
	uint alm_en;
	unsigned long sum_sec;
	unsigned long sec_delta = mktime(2007, 1, 1, 0, 0, 0);
	rtc_dbg("---%s invoked, %p---\n", __func__, dev);

	week = 0;

	__imapx_rtc_access(&tm, IMAP_RTC_ACCESS_READ | IMAP_RTC_ACCESS_ALM);

	sum_sec =
	    ((((week * 7 + tm.tm_wday) * 24 + tm.tm_hour) * 60 +
	      tm.tm_min) * 60 + tm.tm_sec) + sec_delta;
	rtc_time_to_tm(sum_sec, &ttm);

	alm_tm->tm_sec = ttm.tm_sec;
	alm_tm->tm_min = ttm.tm_min;
	alm_tm->tm_hour = ttm.tm_hour;
	alm_tm->tm_mday = ttm.tm_mday;
	alm_tm->tm_mon = ttm.tm_mon;
	alm_tm->tm_year = ttm.tm_year;
	alm_tm->tm_wday = ttm.tm_wday;
	alm_tm->tm_yday = ttm.tm_yday;

	alm_en = readb(base + SYS_ALARM_WEN);
	alrm->enabled = (alm_en) ? 1 : 0;

	rtc_dbg("rtc get alarm %02d, %04d-%02d-%02d  %02d:%02d:%02d\n",
		alm_en,
		alm_tm->tm_year + 1900, alm_tm->tm_mon + 1, alm_tm->tm_mday,
		alm_tm->tm_hour, alm_tm->tm_min, alm_tm->tm_sec);

	return 0;
}

/*!
 ***********************************************************************
 * -Function:
 *    imapx_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
 *
 * -Description:
 *	  Store the values in tm structure into RTC registers.
 *
 * -Input Param
 *    dev		Standard function interface required, but not used here
 *	  alrm		values in alrm.time will be stored into RTC ALM regs,
 *				and alarm will be enabled.
 *
 * -Output Param
 *	  None
 *
 * -Return
 *	  0		    on success
 *	  -EINVAL	on failure
 *
 * -Others
 *	  None
 ***********************************************************************
 */
static int imapx_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_time ttm;
	unsigned long sum_sec;
	unsigned long sec_delta;
	rtc_dbg("---%s invoked, %p---\n", __func__, dev);

	memcpy(&ttm, &alrm->time, sizeof(struct rtc_time));

	if ((ttm.tm_year < 107) || (ttm.tm_year > 184)) {
		rtc_dbg("RTC setalarm out of range.\n");
		return -EINVAL;
	}

	rtc_dbg("rtc set alarm: %d, %04d-%02d-%02d  %02d:%02d:%02d\n",
		alrm->enabled,
		ttm.tm_year + 1900, ttm.tm_mon + 1, ttm.tm_mday,
		ttm.tm_hour, ttm.tm_min, ttm.tm_sec);

	week = 0;
	sec_delta = mktime(2007, 1, 1, 0, 0, 0);

	sum_sec =
	    mktime(ttm.tm_year + 1900, ttm.tm_mon + 1, ttm.tm_mday, ttm.tm_hour,
		   ttm.tm_min, ttm.tm_sec) - sec_delta;
	week = sum_sec / (60 * 60 * 24 * 7);
	sum_sec -= week * 60 * 60 * 24 * 7;
	ttm.tm_sec = sum_sec % 60;
	ttm.tm_min = (sum_sec % (60 * 60)) / 60;
	ttm.tm_hour = (sum_sec % (60 * 60 * 24)) / (60 * 60);
	ttm.tm_wday = sum_sec / (60 * 60 * 24);

	__imapx_rtc_access(&ttm, IMAP_RTC_ACCESS_WRITE | IMAP_RTC_ACCESS_ALM);

	__imapx_rtc_aie(alrm->enabled);

	return 0;
}

void rtc_set_delayed_alarm(u32 delay)
{
	struct rtc_time tm;
	struct timespec time;
	struct rtc_wkalrm alm_tm;

	if (!imapx_rtc_base)
		return;

	imapx_rtc_gettime(NULL, &tm);
	rtc_tm_to_time(&tm, &time.tv_sec);

	rtc_time_to_tm(time.tv_sec + delay, &tm);

	memcpy(&alm_tm.time, &tm, sizeof(struct rtc_time));

	alm_tm.enabled = 1;
	imapx_rtc_setalarm(NULL, &alm_tm);

}
EXPORT_SYMBOL(rtc_set_delayed_alarm);

static int imapx_rtc_proc(struct device *dev, struct seq_file *seq)
{
	if (!dev || !dev->driver)
		return 0;

	return seq_printf(seq, "name\t\t: %s\n", dev_name(dev));
}

static int imapx_rtc_open(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct rtc_device *rtc_dev = platform_get_drvdata(pdev);
	int ret;
	rtc_dbg("---%s invoked by %p---\n", __func__, dev);

	ret = request_irq(imapx_rtc_alarmno, imapx_rtc_alarmirq,
			  IRQF_DISABLED, "imap-rtc alarm", rtc_dev);

	if (ret)
		dev_err(dev, "IRQ%d error %d\n", imapx_rtc_alarmno, ret);

	return ret;
}

static void imapx_rtc_release(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct rtc_device *rtc_dev = platform_get_drvdata(pdev);

	/* do not clear AIE here, it may be needed for wake */

	free_irq(imapx_rtc_alarmno, rtc_dev);
}

static const struct rtc_class_ops imapx_rtcops = {
	.open = imapx_rtc_open,
	.release = imapx_rtc_release,
	.ioctl = NULL,
	.read_time = imapx_rtc_gettime,
	.set_time = imapx_rtc_settime,
	.read_alarm = imapx_rtc_getalarm,
	.set_alarm = imapx_rtc_setalarm,
	.proc = imapx_rtc_proc,
	.alarm_irq_enable = imapx_rtc_aie,
};

/*!
 ***********************************************************************
 * -Function:
 *    imapx_rtc_probe(struct platform_device *pdev)
 *
 * -Description:
 *	  Platform device probe.
 *
 * -Input Param
 *	  pdev		platform device pointer provided by system
 *
 * -Output Param
 *	  None
 *
 * -Return
 *	  0			On success
 *	  -ENOENT	If resource is not availiable
 *	  -EINVAL	If resource is invalid
 *
 * -Others
 *	  None
 ***********************************************************************
 */
static int __init imapx_rtc_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct rtc_time tm;
	struct rtc_cfg *pdata = pdev->dev.platform_data;
	int ret;
#if 0
	if (strcmp(pdata->model_name, "imapx")) {
		pr_err("imapx rtc driver probe error,hw rtc is %s\n",
		       pdata->model_name);
		return -1;
	}
#endif
	RTCMSG("+++%s: probe=%p+++\n", __func__, pdev);

	/* find the IRQs */

	/*tckint has been removed in imapx800 */

	imapx_rtc_alarmno = platform_get_irq(pdev, 0);
	if (imapx_rtc_alarmno < 0) {
		RTCMSG("no irq for alarm\n");
		return -ENOENT;
	}

	rtc_dbg("Alarm IRQ = %d\n", imapx_rtc_alarmno);

	/* get the memory region */

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		RTCMSG("failed to get memory region resource\n");
		return -ENOENT;
	}

	imapx_rtc_mem = request_mem_region(res->start,
					   res->end - res->start + 1,
					   pdev->name);

	if (imapx_rtc_mem == NULL) {
		RTCMSG("failed to reserve memory region\n");
		ret = -ENOENT;
		goto err_nores;
	}

	imapx_rtc_base = ioremap(res->start, res->end - res->start + 1);
	if (imapx_rtc_base == NULL) {
		RTCMSG("failed ioremap()\n");
		ret = -EINVAL;
		goto err_nomap;
	}

	device_init_wakeup(&pdev->dev, 1);

	/* check to see if everything is setup correctly */

	/* register RTC and exit */

	imapx_rtc =
	    rtc_device_register("imap-rtc", &pdev->dev, &imapx_rtcops,
				THIS_MODULE);

	if (IS_ERR(imapx_rtc)) {
		RTCMSG("cannot attach rtc\n");
		ret = PTR_ERR(imapx_rtc);
		goto err_nortc;
	}

	imapx_rtc_gettime(&pdev->dev, &tm);
	if (rtc_valid_tm(&tm)) {
		/* invalid RTC time, set RTC to 2010/09/22 */
		RTCMSG("Invalid RTC time detected, setting to default.\n");
		rtc_time_to_tm(mktime(2010, 9, 22, 0, 0, 0), &tm);
		imapx_rtc_settime(&pdev->dev, &tm);
	}

	/* open int mask */
	writel(readb(imapx_rtc_base + SYS_INT_MASK) & (~0x8),
	       imapx_rtc_base + SYS_INT_MASK);
	platform_set_drvdata(pdev, imapx_rtc);
	return 0;

err_nortc:
	iounmap(imapx_rtc_base);

err_nomap:
	release_resource(imapx_rtc_mem);

err_nores:
	return ret;
}

static int imapx_rtc_remove(struct platform_device *pdev)
{
	struct rtc_device *rtc = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);
	rtc_device_unregister(rtc);

	imapx_rtc_aie(&pdev->dev, 0);

	iounmap(imapx_rtc_base);
	release_resource(imapx_rtc_mem);
	kfree(imapx_rtc_mem);

	return 0;
}

#ifdef CONFIG_PM
/* RTC Power management control */
/*!
 ***********************************************************************
 * -Function:
 *    imapx_rtc_suspend
 *	  imapx_rtc_resume
 *
 * -Description:
 *	  RTC PM functions, RTC should be disabled before suspend,
 *	  and re-enabled after resuem. Alarm state should be leave alone.
 *
 * -Input Param
 *	  pdev		platform device pointer provided by system
 *    state		Standard function interface required, but not used here
 *
 * -Output Param
 *	  None
 *
 * -Return
 *	  0			On success
 *
 * -Others
 *	  None
 ***********************************************************************
 */
static int imapx_rtc_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct rtc_time tm;
	struct timespec time;
#ifdef CONFIG_PM_SHUT_DOWN
	struct rtc_wkalrm alrm;

	void __iomem *base = imapx_rtc_base;
#endif
	uint32_t val;

	time.tv_nsec = 0;

	imapx_rtc_gettime(&pdev->dev, &tm);
	rtc_tm_to_time(&tm, &time.tv_sec);
	
	val = readl(imapx_rtc_base + SYS_WAKEUP_MASK);
	val &= ~(0x1 << 4);
	writel(val, imapx_rtc_base + SYS_WAKEUP_MASK);
#ifdef CONFIG_PM_SHUT_DOWN
	if (rtc_valid_tm(&tm)) {
		/* The time in RTC is invalid */
		RTCMSG("Time in RTC is invalid!\n");
	} else {
		RTCMSG("Current time is:\n");
		RTCTIME(&tm);

		rtc_time_to_tm(time.tv_sec + CONFIG_PM_SHUT_DOWN_SEC,
			       &alrm.time);
		alrm.enabled = 1;

		RTCMSG("Setting alarm to shut down:\n");
		RTCTIME(&alrm.time);
		imapx_rtc_setalarm(&pdev->dev, &alrm);

		writel(0x1e, base + SYS_WAKEUP_MASK);
	}
#endif

	return 0;
}

static int imapx_rtc_resume(struct platform_device *pdev)
{
	struct rtc_time tm = {.tm_sec = 0, };
	struct timespec time;
#ifdef CONFIG_PM_SHUT_DOWN
	int wp_count = 0;

	void __iomem *base = imapx_rtc_base;
#endif

	time.tv_nsec = 0;

#ifdef CONFIG_PM_SHUT_DOWN
	/* Check if RTC wake up, if so close the Machine */
	if (readl(base + SYS_INT_ST) & (1 << 7)) {
		/* first clear this wp status */
		writel((1 << 7), base + SYS_INT_CLR);

		if (CONFIG_PM_SHUT_DOWN_SEC == 13)
			RTCMSG("Wakeup by RTC, wake up count %d\n", ++wp_count);
		else {
			RTCMSG
			    ("Wakeup by RTC it is time to shutdown machine.\n");
			/* shut down */
			writel(0x1f, base + SYS_WAKEUP_MASK);
			writel(0x2, base + SYS_CFG_CMD);
		}
	}
#endif

	while (!tm.tm_sec)
		imapx_rtc_gettime(&pdev->dev, &tm);
	rtc_tm_to_time(&tm, &time.tv_sec);

	return 0;
}
#else
#define imapx_rtc_suspend NULL
#define imapx_rtc_resume	 NULL
#endif

static struct platform_driver imapx_rtc_driver = {
	.probe = imapx_rtc_probe,
	.remove = imapx_rtc_remove,
	.suspend = imapx_rtc_suspend,
	.resume = imapx_rtc_resume,
	.driver = {
		   .name = "imap-rtc",
		   .owner = THIS_MODULE,
		   },
};

static int __init imapx_rtc_init(void)
{
	return platform_driver_register(&imapx_rtc_driver);
}

static void __exit imapx_rtc_exit(void)
{
	platform_driver_unregister(&imapx_rtc_driver);
}

module_init(imapx_rtc_init);
module_exit(imapx_rtc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("warits <warits.wang@infotm.com>");
MODULE_DESCRIPTION("InfoTM iMAPx800 RTC driver");
