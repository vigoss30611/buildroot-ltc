/* qiwo_ota.h
 *
 * Image type handler ops and flash type handler ops defination, download
 * image flow control and UI output functions.
 *
 * Copyright © 2014 Shanghai InfoTMIC Co.,Ltd.
 *
 * Version:
 * 1.0  Created.
 *      xecle, 03/15/2014 03:02:06 PM
 *
 */

#ifndef _QIWO_OTA_H_
#define _QIWO_OTA_H_

//#define UPGRADE_TEST_MOD

#define LOCAL_UPDATE_PATH 	"/mnt/sd0/update.zip"
#define LOCAL_UPDATE_CONFIG_PATH 	"/config"
#define OTA_IUS_PATH 		"/tmp/"
#define OTA_IUS_SPECFILE 	"ius.txt"
#define OTA_IUS_NAME 		"ota.ius"
#define SETTING_PREFEX		"qiwo_upgrade"

#define UPDATE_VERSION		    "updata_version"
#define UPDATE_TYPE		        "updata_type"
#define UPDATE_MD5		        "updata_md5"
#define COUNTRY_INFO		    "country_info"
#define DEVICE_ID		        "device_ID"
#define URL_PREFIX		        "url_prefix"
#define BASE_URL		        "base_url"
#define INTB_URL		        "intb_url"
#define URL_CHECK_UPDATE		"url_check_update"
#define URL_SET_UPDATE_STEP		"url_set_update_step"
#define URL_GET_SPEC_VER		"url_get_spec_ver"
#define SETTING_EINT 		    88887777

#define UPDATE_BUFFER_OFFSET 0
#define UPDATE_DATA_OFFSET 3000

#define QWUPGRADE_DATA_LENGTH ( 64 * 1024 * 1024 )


#define NET_COUNT 10
#define DOWNLOAD_COUNT 1
#define NET_RETRY_BROKEN_TIME ( 10 )
#define MAX_IUS_BLOCK 20
#define HEADER_BIAS 100
#define VERSION_STR 20
#define OTA_TIME_OUT (10*60)    //ota time out /s

struct ota_local_t {
	char md5[32];
};
/*
#define URL_PREFIX "https://"
#define URL_CHECK_UPDATE "/v1/gateway/checkUpdate"
#define URL_SET_UPDATE_STEP "/v1/gateway/setUpdateStep"
#define URL_GET_SPEC_VER "/v1/gateway/getSpecifiedVersion"
*/

struct ota_remote_t {
	unsigned int country_info;
    char deviceID[24];
    char urlPrefix[10];
    char baseUrl[64];
	char intbUrl[64];
    char urlCheckUpdate[64];
    char urlSetUpdateStep[64];
    char urlGetSpecVer[64];
};

union ota_info_t{ 
     struct ota_local_t local;
     struct ota_remote_t remote;
}; 

enum ota_type_t{ 
     OTA_LOCAL = 1,
     OTA_REMOTE = 2,
}; 

struct upgrade_info_t {
	char version[16];
	enum ota_type_t otaType;
	union ota_info_t otaInfo;
};


int default_ota_updata( );
int ota_update_local( struct ota_local_t *local );
int ota_update_remote( struct ota_remote_t *config, char * localVersion );
int checkImg( char* imgName, char * md5 );
int checkVersion( char *path, char *version );
int unzipUpdatafile(char* file, char* destFolder);

#include "net_ota.h"

extern struct ota_remote_t def_config;

#endif
