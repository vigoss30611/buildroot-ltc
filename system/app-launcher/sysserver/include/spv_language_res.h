#ifndef __SPV_LANGUAGE_RES_H__
#define __SPV_LANGUAGE_RES_H__

enum {
    STRING_ON = 1,
    STRING_OFF,
    STRING_AUTO,
    STRING_OK,
    STRING_CANCLE,
    STRING_1_SECONDS,
    STRING_2_SECONDS,
    STRING_3_SECONDS,
    STRING_5_SECONDS,
    STRING_10_SECONDS,
    STRING_15_SECONDS,
    STRING_20_SECONDS,
    STRING_30_SECONDS,
    STRING_1_MINUTE,
    STRING_3_MINUTES,
    STRING_5_MINUTES,
    STRING_10_MINUTES,

    //video settings
    STRING_VIDEO_RESOLUTION,
		STRING_2448_30FPS,
		STRING_1080P_60FPS,
		STRING_1080P_30FPS,
		STRING_720P_30FPS,
		STRING_3264,
		STRING_2880,
		STRING_2048,
		STRING_1920,
		STRING_1472,
		STRING_1088,
		STRING_768,
	STRING_VIDEO_RECORDMODE,	//正常，高速摄影， 缩时录影
	STRING_VIDEO_PREVIEW,
		STRING_PANORAMA_ROUND,
		STRING_PANORAMA_EXPAND,
		STRING_TWO_SEGMENT,
		STRING_THREE_SEGMENT,
		STRING_FOUR_SEGMENT,
		STRING_ROUND,
		STRING_VR,
    STRING_LOOP_RECORDING, 		//off, 1min, 3min, 5min
	STRING_TIMELAPSED_IMAGE, 	//缩时录影 off, 1s, 2s, 3s, 5s, 10s
	STRING_HIGH_SPEED, 			//高速摄影 off, on
    STRING_WDR, 				//off, on
    STRING_EV_COMPENSATION,
		STRING_POS_2P0,
		STRING_POS_1P5,
		STRING_POS_1P0,
		STRING_POS_0P5,
		STRING_ZERO,
		STRING_NEG_0P5,
		STRING_NEG_1P0,
		STRING_NEG_1P5,
		STRING_NEG_2P0,
    STRING_DATE_STAMP,			//off, on
    STRING_RESTORE_DEFAULT,		//恢复默认
		STRING_RESTORE_DEFAULT_SETTINGS, //恢复默认设置？
		STRING_RESET,				//重置
    STRING_RECORD_AUDIO,		//off, on

    //Photo Settings
    STRING_PHOTO_RESOLUTION,	//图像解析度
		STRING_16M,
		STRING_10M,
		STRING_8M,
		STRING_5M,
		STRING_4M,
	STRING_PHOTO_CAPTUREMODE,	//正常， 定时， 自动， 连拍
	STRING_PHOTO_PREVIEW, 		//360全景圆形， 360全景展开， 二分割
								//三分割， 四分割， 圆环， VR
    STRING_MODE_TIMER,			//off, 2s, 5s, 10s
	STRING_MODE_AUTO,			//off, 3s, 10s, 15s, 20s, 30s
	STRING_MODE_SEQUENCE,		//off, 3p/s, 5p/s, 7p/s, 10p/s
		STRING_3PS,
		STRING_5PS,
		STRING_7PS,
		STRING_10PS,
    STRING_QUALITY,
		STRING_FINE,
		STRING_NORMAL,			//一般
		STRING_ECONOMY,
    STRING_SHARPNESS,			//一般， 柔和， 强烈
		STRING_STRONG,
		STRING_SOFT,
    STRING_WHITE_BALANCE,
		STRING_DAYLIGHT,
		STRING_CLOUDY,
		STRING_TUNGSTEN,
		STRING_FLUORESCENT,
    STRING_COLOR,				//色彩， 黑白， 棕褐色
    	STRING_BLACK_AND_WHITE,
    	STRING_SEPIA,
    STRING_ISO_LIMIT,
    	STRING_800,
    	STRING_400,
    	STRING_200,
    	STRING_100,
    //STRING_EV_COMPENSATION	//ref to video setting
    STRING_ANTI_SHAKING,		//off, on
    //STRING_DATE_STAMP,		//ref to video setting
    //STRING_RESTORE_DEFAULT,	//ref to video setting

    //setup
    STRING_LANGUAGE,
    	STRING_ENGLISH,
    	STRING_CHINESE,
    	STRING_CHINESE_TRADITIONAL,
    	STRING_FRENCH,
    	STRING_ESPANISH,
    	STRING_ITALIAN,
    	STRING_PORTUGUESE,
    	STRING_GERMAN,
    	STRING_RUSSIAN,
		STRING_KOREAN,
		STRING_JAPANESE,
    STRING_WIFI_MODE,			//off, direct, router
		STRING_DIRECT,
		STRING_ROUTER,
    STRING_FREQUENCY,
    	STRING_50HZ,
    	STRING_60HZ,
	STRING_LED,					//off, on
	STRING_CARDV_MODE,			//off, on
	STRING_PARKING_GUARD,		//off, on
	STRING_GSENSOR,				//off, high, medium ,low
    STRING_IMAGE_ROTATION,		//off, on
	STRING_TIME_DATE_SETTING,	//时间日期设置
    	STRING_DATE_SETTING,	//日期设置： 月/日/年
    	STRING_TIME_SETTING,	//时间设置
    	STRING_AM,
    	STRING_PM,
    STRING_VOLUME,
    	STRING_10P,
    	STRING_20P,
    	STRING_30P,
    	STRING_40P,
    	STRING_50P,
    	STRING_60P,
    	STRING_70P,
    	STRING_80P,
    	STRING_90P,
    	STRING_100P,
    STRING_KEY_TONE,			//off, on
    STRING_DISPLAY_BRIGHTNESS,   //High, Medium, Low
    STRING_HIGH,
    STRING_MEDIUM,
    STRING_LOW,
    STRING_AUTO_SLEEP,			//自动屏保 off, 10s, 20s, 30s
    STRING_AUTO_POWER_OFF,		//自动关机 off, 1min, 3min, 5min
    STRING_DELAYED_SHUTDOWN,	//off, 10s, 20s, 30s
    STRING_FORMAT,
    	STRING_FORMAT_Q,		//是否格式化存储卡
    STRING_CAMERA_RESET,
    	STRING_RESET_CAMER_Q,
    STRING_VERSION,
		STRING_PRODUCT,			//产品型号：
		STRING_SOFTWARE,		//软件版本
		STRING_UPDATE_TIME,		//更新时间
		STRING_INC,				//生产厂商
	
	//format toast
    STRING_FORMATE_PROG_S,		//正在格式化
    STRING_FORMATE_ERROR,		//格式化出错
    STRING_SD_EMPTY,
    STRING_SD_FULL,
    STRING_SD_NO,
    STRING_SD_ERROR,
    STRING_SD_ERROR_FORMAT,
    STRING_SD_PREPARE,
	STRING_WIFI_DEPLOYING,
	STRING_WIFI_OFF,
	STRING_GOODBYE,
	STRING_CAMERA_BUSY,
	STRING_BATTERY_LOW,

    STRING_END = STRING_BATTERY_LOW,
};


extern const char *language_en[];
extern const char *language_zh[];
extern const char *language_tw[];
extern const char *language_fr[];
extern const char *language_es[];
extern const char *language_it[];
extern const char *language_pt[];
extern const char *language_de[];
extern const char *language_ru[];
extern const char *language_kr[];
extern const char *language_jp[];

extern const char **g_language;

#define GETSTRING(string_id) g_language[string_id - 1]

int GetStringId(const char *value);

#endif //__SPV_LANGUAGE_RES
