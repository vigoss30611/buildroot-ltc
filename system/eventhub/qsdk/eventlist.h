
#ifndef __EVENT_DEV_H
#error YOU_CANNOT_INCLUDE_THIS_FILE_DIRECTLY
#endif

#ifndef __COMMON_EVENTS_H__
#define __COMMON_EVENTS_H__

/* qsdk/common-provider.h */
#define EVENT_KEY "key"
#define EVENT_TP "tp"
#define EVENT_ACCELEROMETER "gsensor"
#define EVENT_PROCESS_END "pend"
#define EVENT_MOUNT "mount"
#define EVENT_UNMOUNT "umount"
#define EVENT_BATTERY_ST "battery_st"
#define EVENT_BATTERY_CAP "battery_cap"
#define EVENT_CHARGING "charging"
#define EVENT_LINEIN "linein"
#define EVENT_LIGHTSENCE_D "ls_dark"
#define EVENT_LIGHTSENCE_B "ls_bright"
#define EVENT_STORAGE_INSERT "insert"
#define EVENT_STORAGE_REMOVE "remove"

/*for boot complete send event handle*/
#define EVENT_BOOT_COMPLETE "boot_complete"

/* qsdk/videobox.h */
#ifndef EVENT_VIDEOBOX
#define EVENT_VIDEOBOX "videobox"
#endif

#ifndef EVENT_VAMOVE
#define EVENT_VAMOVE "vamovement"
#endif

#ifndef EVENT_SMARTRC_CTRL
#define EVENT_SMARTRC_CTRL "smartrc_ctrl"
#endif

#ifndef EVENT_VAMOVE_BLK
#define EVENT_VAMOVE_BLK  "vamovement_blk"
#endif

#ifndef EVENT_MVMOVE
#define EVENT_MVMOVE "mvmovement"
#endif

#ifndef EVENT_MVMOVE_ROI
#define EVENT_MVMOVE_ROI "mvmovement_roi"
#endif

#ifndef EVENT_FODET
#define EVENT_FODET "fodet"
#endif

/* qsdk/vplay.h */

#ifndef EVENT_VPLAY
#define EVENT_VPLAY  "vplay"
#endif

/* orphans */
#define EVENT_NTPOK "step"

#endif /* __COMMON_EVENTS_H__ */
