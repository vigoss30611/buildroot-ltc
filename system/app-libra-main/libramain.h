#ifndef __SPV_CONFIG_H__
#define __SPV_CONFIG_H__

#define SPV_DEBUG

//enable colorkey blend method, otherwise use alpha blend between fb0 & fb1
#define COLORKEY_ENABLED 1

//show the left space in UI 
#define SD_VOLUME_ESTIMATE_ENABLED 0

//use ffmpeg to decode the video's duration
#define USE_THIRD_LIB_CALCULATE_DUEATION 1

//use the secondarydc feature to defense flicker
#ifdef GUI_ROTATE_SCREEN
#define SECONDARYDC_ENABLED 0
#else
#define SECONDARYDC_ENABLED 1
#endif

//define whether the config can be saved in flash or sdcard
#ifdef UBUNTU
#define SYSTEM_IS_READ_ONLY 0
#else
#define SYSTEM_IS_READ_ONLY 0
#endif

//use the system setting as the 4th mode
#define USE_SETTINGS_MODE 0

//enable the shutdown logo, otherwise will show text 'Goodbye'
//#define SHUTDOWN_LOGO

//ignore invalid video, play next video automatically
#define IGNORE_INVALID_VIDEO 1

#if 0
#define Q3_EVB
//device width & height in pixels
#ifndef Q3_EVB
#define DEVICE_WIDTH 720
#define DEVICE_HEIGHT 576

//viewport width & heiht in pixels
#define VIEWPORT_WIDTH 360
#define VIEWPORT_HEIGHT 288

#else
#define DEVICE_WIDTH 360
#define DEVICE_HEIGHT 240
#define VIEWPORT_WIDTH 360
#define VIEWPORT_HEIGHT 240
#endif

#define SCALE_FACTOR (DEVICE_WIDTH / VIEWPORT_WIDTH)
#endif

#define FOLDER_ROW 2
#define FOLDER_COLUMN 3

//scale the ui to automatically adapt different screen size
#ifdef GUI_LARGE_SCREEN
#define SCALE_ENABLED 0
#else
#define SCALE_ENABLED 1
#endif

#ifdef UBUNTU
#define RESOURCE_PATH "./"
#define CONFIG_PATH "./config/"
#define EXTERNAL_STORAGE_PATH "./"
#define EXTERNAL_MEDIA_DIR "./DCIM/"
#define EXTERNAL_CONFIG EXTERNAL_MEDIA_DIR"./.libramain.config"
#else//UBUNTU
#define RESOURCE_PATH "/root/libra/"
#define CONFIG_PATH "/root/libra/config/"
#define EXTERNAL_STORAGE_PATH "/mnt/sd0/"
#define EXTERNAL_MEDIA_DIR "/mnt/sd0/DCIM/100CVR"
#define EXTERNAL_CONFIG EXTERNAL_MEDIA_DIR"./.libramain.config"
#endif//UBUNTU

//#define WIFI_SSID "INFOTM_cardv"
//#define WIFI_PWD  "admin888"

#define DEFAULT "default"

#define SPV_VERSION "V1.1.4"

#endif
