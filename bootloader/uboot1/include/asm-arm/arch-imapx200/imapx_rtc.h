#ifndef __IMAPX_RTC_H__
#define __IMAPX_RTC_H__

#define RTCCON     (RTC_BASE_REG_PA+0x40)	/* RTC control */
#define TICNT      (RTC_BASE_REG_PA+0x44)	/* Tick time count */
#define RTCALM     (RTC_BASE_REG_PA+0x50)	/* RTC alarm control */
#define ALMSEC     (RTC_BASE_REG_PA+0x54)	/* Alarm second */
#define ALMMIN     (RTC_BASE_REG_PA+0x58)	/* Alarm minute */
#define ALMHOUR    (RTC_BASE_REG_PA+0x5c)	/* Alarm Hour */
#define ALMDATE    (RTC_BASE_REG_PA+0x60)	/* Alarm date edited by junon */
#define ALMMON     (RTC_BASE_REG_PA+0x64)	/* Alarm month */
#define ALMYEAR    (RTC_BASE_REG_PA+0x68)	/* Alarm year */
#define RTCRST     (RTC_BASE_REG_PA+0x6c)	/* RTC round reset */
#define BCDSEC     (RTC_BASE_REG_PA+0x70)	/* BCD second */
#define BCDMIN     (RTC_BASE_REG_PA+0x74)	/* BCD minute */
#define BCDHOUR    (RTC_BASE_REG_PA+0x78)	/* BCD hour */
#define BCDDATE    (RTC_BASE_REG_PA+0x7c)	/* BCD date edited by junon */
#define BCDDAY     (RTC_BASE_REG_PA+0x80)	/* BCD day edited by junon */
#define BCDMON     (RTC_BASE_REG_PA+0x84)	/* BCD month */
#define BCDYEAR    (RTC_BASE_REG_PA+0x88)	/* BCD year */
#define ALMDAY     (RTC_BASE_REG_PA+0x8C)	/* ALMDAY */
#define RTCSET     (RTC_BASE_REG_PA+0x90)	/* ALMDAY  */

#endif /*__IMAPX_RTC_H__*/
