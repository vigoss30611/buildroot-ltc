#include <string.h>
#include <semaphore.h>
#include <qsdk/upgrade.h>
#include "ota.h"
#include <qsdk/wifi.h>
#include "net_ota.h"

char data[ QWUPGRADE_DATA_LENGTH ];
static volatile long my_timer = 0;
static volatile int timer_start = 0;
static int timeout_s = OTA_TIME_OUT;
static sem_t wifiSem;

extern struct ota_remote_t *currentRemoteConfig;

struct ota_remote_t def_config =
{
    .country_info = 1,
    .deviceID = NULL,
    .urlPrefix = "https://",
    .baseUrl = "sh.qiwocloud1.com",
    .intbUrl = "sh.qiwocloud2.com",
    .urlCheckUpdate = "/v1/gateway/checkUpdate",
    .urlSetUpdateStep = "/v1/gateway/setUpdateStep",
    .urlGetSpecVer = "/v1/gateway/getSpecifiedVersion",
};

int reset_timer() {
    struct timeval tv;
    while(gettimeofday(&tv, NULL)) {
        sleep(10);
    }
    my_timer = tv.tv_sec;

    return SUCCESS;
}

int set_timer(int status) {
    timer_start = status;

    return SUCCESS;
}

void obs_thread_main(void* param) {
    struct timeval tv;
    while(1) {
        if (timer_start && !gettimeofday(&tv, NULL)) {
            if (tv.tv_sec - my_timer > timeout_s ) {
                LOGE("OTA timeout, rebooting\n");
                system_set_led("red", 300, 200, 1);
                sleep(5);
                reboot(LINUX_REBOOT_CMD_RESTART);
            }
        }
        sleep(31);
    }
}

int start_obs_thread() {
    int ret;
    pthread_t thread_id;
    if (ret = pthread_create(&thread_id, NULL, &obs_thread_main, NULL)) {
        LOGE("Create obs thread failed, ret = %d\n", ret);
        return FAILED;
    }
    pthread_detach(thread_id);
    return SUCCESS;
}

int checkImg( char* imgName, char * md5 )
{
    int fd;
    int ret = SUCCESS;
    int len = 0;
    
	fd = open(imgName, O_RDONLY);
	if (fd < 0) {
		ret = FAILED;
        LOGE("Open %s failed with %s.\n", imgName, strerror(errno));
        goto funReturn;
    }

    while((ret = read(fd, data + len, QWUPGRADE_DATA_LENGTH))) {
        if (ret < 0) {
            LOGE("Read %s failed with %s.\n", imgName, strerror(errno));
            goto funReturn;
        }
        len += ret;
        if (len >= QWUPGRADE_DATA_LENGTH) {
            break;
        }
    }

    ret = check_md5sum(data, len, md5);
    
funReturn:
    if(fd >= 0)
    {
        close(fd);
    }
    
    return ret;

}

int getIusInfo( char *path, char *version, char *md5 )
{
	int fd = -1;
	int ret = FAILED;
	int len = 0;
	
	char readBuf[256];

	char *p = NULL;
	char verKeyWord[] = "version:";
	char MD5KeyWord[] = "md5:";

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		ret = FAILED;
        LOGE("Open %s failed with %s.\n", path, strerror(errno));
        goto funReturn;
    }
	
	while((ret = read(fd, readBuf + len, sizeof(readBuf)))) {
        if (ret < 0) {
            LOGE("Read %s failed with %s.\n", path, strerror(errno));
            goto funReturn;
        }
        len += ret;
        if (len >= strlen(readBuf)) {
            break;
        }
    }
	
	p = strstr( readBuf, verKeyWord );
	if( p != NULL )
	{
		strcpy( version, p + strlen( verKeyWord ) );
	}
	else
	{
		LOGE("Read version failed with %s.\n", strerror(errno));
		goto funReturn;
	}

	p = strstr( readBuf, MD5KeyWord );
	if( p != NULL )
	{
		strcpy( md5, p + strlen( MD5KeyWord ) );
	}
	else
	{
		LOGE("Read MD5 failed with %s.\n", strerror(errno));
		goto funReturn;
	}

funReturn:
	if(fd >= 0)
	{
		close(fd);
	}
	
	return ret;

}


int unzipUpdatafile(char* file, char* destFolder)
{
	char cmd[64];
	int ret = 0;
    
	ret = sprintf(cmd, "tar -zxvf %s -C %s", LOCAL_UPDATE_PATH, OTA_IUS_PATH);
	if(ret < 0)
	{
		LOGE("tar -zxvf %s -C %s err with %s.\n", LOCAL_UPDATE_PATH, OTA_IUS_PATH, strerror(errno));
	}
	else
	{
		ret = system(cmd);
	}

	return ret;
}


int ota_update_local( struct ota_local_t *local )
{
	int ret = 0;
	char iusName[128];
	char iusSpac[128];
	char iusVersion[128];
	char iusMD5[32];

	ret = access(LOCAL_UPDATE_PATH, F_OK);
	if(ret != 0)
	{
		LOGE("can not find %s\n", LOCAL_UPDATE_PATH);
		goto funReturn;
	}
	
	ret = unzipUpdatafile( LOCAL_UPDATE_PATH, OTA_IUS_PATH ); 
	if(ret != 0)
	{
		LOGE("unzipUpdatafile faild with ret %d\n", ret);
		goto funReturn;
	}

    
	sprintf(iusSpac, "%s%s", OTA_IUS_PATH, OTA_IUS_SPECFILE);

	ret = getIusInfo(iusSpac, iusVersion, iusMD5);
	if(ret < 0)
	{
		LOGE("getIusInfo faild with ret %d\n", ret);
		goto funReturn;
	}

    printf("getIusInfo iusSpac %s iusVersion %s iusMD5 %s\n", iusSpac, iusVersion, iusMD5);

	sprintf(iusName, "%s%s", OTA_IUS_PATH, OTA_IUS_NAME);
	ret = checkImg( iusName, iusMD5 );
	if(ret != 0)
	{
		LOGE("checkImg faild with ret %d\n", ret);
		goto funReturn;
	}
	
funReturn:

	return ret;

}

static void upgrade_wifi_state_handle(char *event, void *arg)
{
	LOGI("wifi_state handle  %s %s\n", (char *)arg, event);
	if (!strncmp(event, "wifi_state", strlen("wifi_state"))) {

		if (!strncmp((char *)arg, "ap_enabled", strlen("ap_enabled"))) {
			

		} else if (!strncmp((char *)arg, "sta_connected", strlen("sta_connected"))) {

			if (sem_post(&wifiSem) == -1)
            {
                LOGE("sem_post wifiSem faild\n");
            }

		    LOGI("wifi_state changed %s \n", (char *)arg);
		} else if (!strncmp((char *)arg, "scan_complete", strlen("scan_complete"))) {

            if (sem_post(&wifiSem) == -1)
            {
                LOGE("sem_post wifiSem faild\n");
            }
		}
	}

	return;
}


int connectWIFI( void )
{
	int ret = SUCCESS;

    struct timespec ts;
    struct wifi_config_t* wifi_config_connect;
    
#ifdef UPGRADE_TEST_MOD
    struct wifi_config_t tmp = {
        .ssid = "infotm-henry",
        .psk = "1928374655",
    };
    
#endif

    ret = wifi_start_service(upgrade_wifi_state_handle);
    if(ret != SUCCESS)
	{
		LOGE("wifi_start_service faild\n");
		return FAILED;
	}
   
    LOGI("wifi_sta_enable\n");
	ret = wifi_sta_enable();
	if(ret != SUCCESS)
	{
		LOGE("wifi_sta_enable faild\n");
		return FAILED;
	}
    ret = sem_init(&wifiSem, 0, 0);
    if(ret == -1)
	{
		LOGE("wifi_sta_enable faild\n");
		return FAILED;
	}

    gettimeofday(&ts,NULL); 
    
    
    ts.tv_sec=time(NULL)+100;   
    ts.tv_nsec=0;
    
    ret = sem_timedwait(&wifiSem, &ts);
    sem_destroy(&wifiSem);
    if(ret == -1)
	{
		LOGE("sem_timedwait wifiSem faild\n");
		return FAILED;
	}
    
#ifdef UPGRADE_TEST_MOD
    wifi_config_connect = &tmp;    
#else
    LOGI("wifi_sta_get_config\n");
	wifi_config_connect = wifi_sta_get_config();
	if (wifi_config_connect == NULL) 
	{
		LOGE("wifi_sta_get_config faild\n");
		return FAILED;
	}
#endif

    LOGI("wifi_sta_connect ssid %s psk %s\n", wifi_config_connect->ssid, wifi_config_connect->psk);
	ret = wifi_sta_connect(wifi_config_connect);
	if(ret != SUCCESS)
	{
		LOGE("wifi_sta_connect faild\n");
		return FAILED;
	}
	
	return SUCCESS;
}

int ota_update_remote( struct ota_remote_t *remoteConfig, char * localVersion )
{
    char value[200];
    char url[300];
    char iusName[128];
    
	int ret = SUCCESS;
    int count = 0;
    int fd;
    int len = 0;
    int offset = 0;
    int findex = 0;
    
    void* ctx = NULL;
    char* FPOS[2];
    char* current_api = NULL;
    
    ret = connectWIFI();
	if(ret != SUCCESS)
	{
		LOGE("connectWIFI faild\n");
        goto funReturn;
	}

    if (remoteConfig->country_info == 1) 
    {
        snprintf(url, 300, "%s%s", remoteConfig->urlPrefix, remoteConfig->baseUrl);
    }
    else
    {
        snprintf(url, 300, "%s%s", remoteConfig->urlPrefix, remoteConfig->intbUrl);
    }
    LOGI("url %s\n", url);
    
    snprintf(value, 200, SU_QUERY_FORMAT, remoteConfig->deviceID, START_UPDATE);
    reset_timer();
    set_timer(1);
    
    //start update
    LOGI("The value update is: %s\n", value);
    char* update_buffer = data + UPDATE_BUFFER_OFFSET;
    while(ret = net_ota_update(update_buffer, value, remoteConfig->urlSetUpdateStep, url)) {
        LOGI("Update start  status failed.\n");
        if (count < NET_COUNT) {
            sleep(NET_RETRY_BROKEN_TIME);
            ++count;
        } else {
            ret = FAILED;
            goto funReturn;
        }
    }
    LOGI("malloc ctx\n");
    ctx = malloc(sizeof(struct net_ota_qiwo));
    if (ctx == NULL) {
        LOGE("Alloc memory to struct net_pta failed.\n");
        ret = FAILED;
        goto funReturn;
    }
    
    // get the remote version
    // If we retrieve ota information failed we wait 10 seconds.

    sprintf(value, CU_QUERY_FORMAT, remoteConfig->deviceID);
    LOGI("The value get is: %s\n", value);
    count = 0;
    current_api = remoteConfig->urlCheckUpdate;
    while (ret = net_ota_get(ctx, data+UPDATE_DATA_OFFSET, value, current_api, url)) {
        LOGI("net_ota_get failed %d times.\n", count);
        if (count < NET_COUNT) {
            sleep(NET_RETRY_BROKEN_TIME);
            ++count;
        } else {
            ret = FAILED;
            goto funReturn;
        }
    }
    
    offset = ((struct net_ota_qiwo*)ctx)->offset;
    struct net_ota_qiwo* net_ctx = (struct net_ota_qiwo*)ctx;
    if (!(net_ctx->new_version)) {
        LOGI("There is no new version for now.\n");
        ret = SUCCESS;
        goto funReturn;
    }
    
    
    LOGI("DOWNLOAD START!\n");
    count = 0;
    while(FAILED == (ret = net_ota_read(net_ctx->url[findex], net_ctx->md5[findex], data+UPDATE_DATA_OFFSET+offset, &(FPOS[findex])))) {
        if (count < DOWNLOAD_COUNT) {
            sleep(NET_RETRY_BROKEN_TIME);
            ++count;
        } else {
            ret = FAILED;
            goto funReturn;
        }
    }
    net_ctx->length[findex] = ret;
    LOGI("DOWNLOAD DONE!\n");
    
    reset_timer();
    snprintf(value, strlen(value), SU_QUERY_FORMAT, remoteConfig->deviceID, FINISH_DOWNLOAD);
    LOGI("The value update is: %s\n", value);
    count = 0;
    while(ret = net_ota_update(update_buffer, value, remoteConfig->urlSetUpdateStep, url)) {
        LOGI("Update start  status failed.\n");
        if (count < NET_COUNT) {
            sleep(NET_RETRY_BROKEN_TIME);
            ++count;
        } else {
            ret = FAILED;
            goto funReturn;
        }
    }
    
//Create ius file
    sprintf(iusName, "%s%s", OTA_IUS_PATH, OTA_IUS_NAME);
    
	fd = open(iusName, O_WRONLY);
	if (fd < 0) {
		ret = FAILED;
        LOGE("Open %s failed with %s.\n", iusName, strerror(errno));
        goto funReturn;
    }

    while((ret = write(fd, FPOS[findex], net_ctx->length[findex]))) {
        if (ret < 0) {
            LOGE("write %s failed with %s.\n", iusName, strerror(errno));
            goto funReturn;
        }
        len += ret;
        if (len >= QWUPGRADE_DATA_LENGTH) {
            break;
        }
    }
    
    reset_timer();
    set_timer(0);
    
    ret = SUCCESS;

funReturn:
    if(ctx)
    {
        free(ctx);
    }
    if(fd >= 0)
    {
        close(fd);
    }

    return ret;
}

int default_ota_updata(struct upgrade_info_t *config)
{
    int ret = SUCCESS;
 //   struct ota_remote_t def_config;
    
    if( access(LOCAL_UPDATE_PATH, F_OK) == 0 )
    {
        LOGI("default_ota_updata ota_update_local\n");
        ret = ota_update_local( NULL );
    }
    else
    {
        LOGI("default_ota_updata ota_update_remote\n");
        ret = ota_update_remote(&(config->otaInfo.remote), NULL);
    }

    return ret;
}



