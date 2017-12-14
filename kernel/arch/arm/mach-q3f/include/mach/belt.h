
#ifndef _IMAP_BELT_H_
#define	_IMAP_BELT_H_

//#define belt_dbg(x...) printf(x)
#define belt_dbg(x...)

#define BELT_MAJOR 171
#define BELT_MAGIC 'b'
#define BELT_IOCMAX 10
#define BELT_NAME "belt"
#define BELT_NODE "/dev/belt"

#define BELT_SCENE_GET		_IOR(BELT_MAGIC, 1, unsigned long)
#define BELT_SCENE_SET		_IOW(BELT_MAGIC, 2, unsigned long)
#define BELT_SCENE_UNSET	_IOW(BELT_MAGIC, 3, unsigned long)
#define BATTERY_LEVEL	    _IOW(BELT_MAGIC, 4, unsigned long)
#define BELT_ENV_GET	    _IOW(BELT_MAGIC, 5, unsigned long)
#define BELT_REG            _IOW(BELT_MAGIC, 6, unsigned long)
#define BELT_ENV_SET	    _IOW(BELT_MAGIC, 7, unsigned long)
#define BELT_ENV_CLEAN	    _IOW(BELT_MAGIC, 8, unsigned long)
#define BELT_ENV_SAVE	    _IOW(BELT_MAGIC, 9, unsigned long)

#define SCENE_VIDEO_NET		(1 << 0)
#define SCENE_VIDEO_LOCAL	(1 << 1)
#define SCENE_HDMI		(1 << 2)
#define SCENE_LOWPOWER		(1 << 3)

/* scene GPS is deserted, use SCENE_CPU_MAX instead */
#define SCENE_GPS		(1 << 4)
#define SCENE_CPU_MAX		SCENE_GPS

/* scene CTS is deserted, use SCENE_HARD_FIX instead */
#define SCENE_CTS		(1 << 5)
#define SCENE_HARD_FIX		SCENE_CTS
#define SCENE_BLUETOOTH		(1 << 6)
#define SCENE_PERFORMANCE	(1 << 8)
#define SCENE_POWERLIMIT_FIX	(1 << 9)
#define SCENE_POWERLIMIT(a, b)	((1 << 9) | (a << 16) | (b << 24))

#define SCENE_RTC_WAKEUP	(1 << 10)

#define SCENE_PL_GETC(x)	((x >> 16) & 0xff)
#define SCENE_PL_GETG(x)	((x >> 24) & 0xff)

extern int belt_scene_get(void);
extern int belt_scene_set(int scene);
extern int belt_scene_unset(int scene);
extern int belt_get_rtc(void);

#endif /* _IMAP_BELT_H_ */

