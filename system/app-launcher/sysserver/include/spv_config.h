#ifndef __SPV_CONFIG_H__
#define __SPV_CONFIG_H__

#define SPV_DEBUG

//enable colorkey blend method, otherwise use alpha blend between fb0 & fb1
#ifdef GUI_HDMI_ENABLE
#define COLORKEY_ENABLED 0
#else
#define COLORKEY_ENABLED 1
#endif

//use ffmpeg to decode the video's duration
#define USE_THIRD_LIB_CALCULATE_DUEATION 1

//define whether the config can be saved in flash or sdcard
#define SYSTEM_IS_READ_ONLY 0

//use the system setting as the 4th mode
#define USE_SETTINGS_MODE 1
//ignore invalid video, play next video automatically
#define IGNORE_INVALID_VIDEO 1
#ifdef UBUNTU
#define RESOURCE_PATH "./"
#define CONFIG_PATH "./config/"
#define EXTERNAL_STORAGE_PATH "./"
#define EXTERNAL_MEDIA_DIR "./DCIM/"
#define EXTERNAL_CONFIG EXTERNAL_MEDIA_DIR"./.launcher.config"
#else//UBUNTU
#define RESOURCE_PATH "/usr/share/launcher/"
#define CONFIG_PATH "/config/"
#define EXTERNAL_STORAGE_PATH "/mnt/sd0/"
#define EXTERNAL_MEDIA_DIR "/mnt/sd0/DCIM/100CVR/"
#define EXTERNAL_CONFIG EXTERNAL_MEDIA_DIR"./.launcher.config"
#endif//UBUNTU

#define CALIBRATION_FILE "/mnt/config/calibration.data"
#define UPGRADE_FILE "/dev/shm/burn.ius"
#define NETWORK_FILE "/config/network.cfg"
#define NETWORK_FILE_BAK "usr/share/launcher/config/network.cfg"
#define PCB_TEST_FILE "/mnt/config/pcb_test.data"


#define DEFAULT "default"

//#define WIFI_SSID "INFOTM_cardv"
//#define WIFI_PWD  "admin888"

#define SPV_VERSION "V1.4"
#define SPV_PRODUCT "Q360"
#define SPV_INC	"SPORT DV10"

#endif
