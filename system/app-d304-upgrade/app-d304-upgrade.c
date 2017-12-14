#include <unistd.h>
#include <sys/reboot.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/time.h>

#include <qsdk/upgrade.h>
#include "ota.h"

#define MMC_IUS_PATH "/mnt/sd0/ota.ius"
#define OTA_IUS_PATH "/tmp/ota.ius"


struct ota_remote_t *currentRemoteConfig = NULL;
extern char data[ ];
static int currentFlag;

#ifdef UPGRADE_TEST_MOD
int upgradeMode = 0;
#endif

extern const char * setting_get_string(const char *owner, const char *key, char *buf, int len);

int get_qwupgrade_config(struct upgrade_info_t *config)
{
    LOGI("setting_get_string default\n");

    char value[128];
    char *pret;

    system("mkdir /config");
    system("mount -t ext4 /dev/spiblock1p2 /config");
    int ret;
 
    pret = setting_get_string(SETTING_PREFEX, UPDATE_VERSION, value, 128); 
    if (value[0] == '\0') 
    {
    }
    else
    {
        strcpy(config->version, value);
    }
    
    ret = setting_get_int(SETTING_PREFEX, UPDATE_TYPE);
    if (ret == SETTING_EINT)
    {
        config->otaType = -1;
    }
    else
    {
        config->otaType = ret;
    }

    if(config->otaType == OTA_LOCAL)
    {
        setting_get_string(SETTING_PREFEX, UPDATE_MD5, value, 128);
        if (value[0] == '\0') 
        {
            
        }
        else
        {
            strcpy(config->otaInfo.local.md5, value);
        }
    }
    else if(config->otaType == OTA_REMOTE)
    {
        ret = setting_get_int(SETTING_PREFEX, COUNTRY_INFO);
        if (ret == SETTING_EINT)
        {
            config->otaInfo.remote.country_info = def_config.country_info;
        }
        else
        {
            config->otaInfo.remote.country_info = ret;
        }

        setting_get_string(SETTING_PREFEX, DEVICE_ID, value, 128);
        if (value[0] == '\0') 
        {
            strcpy(config->otaInfo.remote.deviceID, def_config.deviceID);
        }
        else
        {
            strcpy(config->otaInfo.remote.deviceID, value);
        }

        setting_get_string(SETTING_PREFEX, URL_PREFIX, value, 128);
        if (value[0] == '\0') 
        {
            strcpy(config->otaInfo.remote.urlPrefix, def_config.urlPrefix);
        }
        else
        {
            strcpy(config->otaInfo.remote.urlPrefix, value);
        }

        setting_get_string(SETTING_PREFEX, BASE_URL, value, 128);
        if (value[0] == '\0') 
        {
            strcpy(config->otaInfo.remote.baseUrl, def_config.baseUrl);
        }
        else
        {
            strcpy(config->otaInfo.remote.baseUrl, value);
        }
   
        setting_get_string(SETTING_PREFEX, INTB_URL, value, 128);
        if (value[0] == '\0') 
        {
            strcpy(config->otaInfo.remote.intbUrl, def_config.intbUrl);
        }
        else
        {
            strcpy(config->otaInfo.remote.intbUrl, value);
        }

        setting_get_string(SETTING_PREFEX, URL_CHECK_UPDATE, value, 128);
        if (value[0] == '\0') 
        {
            strcpy(config->otaInfo.remote.urlCheckUpdate, def_config.urlCheckUpdate);
        }
        else
        {
            strcpy(config->otaInfo.remote.urlCheckUpdate, value);
        }

        setting_get_string(SETTING_PREFEX, URL_SET_UPDATE_STEP, value, 128);
        if (value[0] == '\0') 
        {
            strcpy(config->otaInfo.remote.urlSetUpdateStep, def_config.urlSetUpdateStep);
        }
        else
        {
            strcpy(config->otaInfo.remote.urlSetUpdateStep, value);
        }

        setting_get_string(SETTING_PREFEX, URL_GET_SPEC_VER, value, 128);
        if (value[0] == '\0') 
        {
            strcpy(config->otaInfo.remote.urlGetSpecVer, def_config.urlGetSpecVer);
        }
        else
        {
            strcpy(config->otaInfo.remote.urlGetSpecVer, value);
        }
    }
    
    
#ifdef UPGRADE_TEST_MOD

    struct upgrade_info_t* pConfig = (struct upgrade_info_t*)config;

    if(upgradeMode == 1)
    {
        pConfig->otaType = OTA_LOCAL;
        LOGI("\n QIWO upgrade OTA_LOCAL \n");
    }
    else if(upgradeMode == 2)
    {
        pConfig->otaType = OTA_REMOTE;
        LOGI("\n QIWO upgrade OTA_REMOTE \n");
    }
    else if(upgradeMode == 3)
    {
        pConfig->otaType = -1;
        LOGI("\n QIWO upgrade def \n");
    }
#endif        
    return 0;
}


static int get_boot_flag(void)
{
    int len = 0;
    int fd;
    char cmd[192];
    int ret;

    fd = open("/proc/cmdline", O_RDONLY);
    if (fd < 0) {
        // TO-DO: implement LOGE, LOGD, LOGI
        LOGE("Open /proc/cmdline failed with %s.\n", strerror(errno));
        return FAILED;
    }
    while((ret = read(fd, cmd + len, sizeof(cmd)))) {
        if (ret == -1) {
            LOGE("Read /proc/cmdline failed with %s.\n", strerror(errno));
            return FAILED;
        }
        len += ret;
        if (len >= sizeof(cmd)) {
            break;
        }
    }
    for (ret = 0; ret < len; ret++) {
        if (cmd[ret] != ' ')
        	continue;
        if (*(int *)(cmd+ret+1) == 'edom') {
            return(*(int*)(cmd+ret+5));
        }
    }

    return FAILED;
}

static void upgrade_state_callback_qiwo(void *arg, int image_num, int state, int state_arg)
{
	struct upgrade_t *upgrade = (struct upgrade_t *)arg;
	int cost_time;

	switch (state) {
		case UPGRADE_START:
			//system_set_led("red", 1000, 800, 1);
			gettimeofday(&upgrade->start_time, NULL);
			printf("QIWO upgrade start \n");
			break;
		case UPGRADE_COMPLETE:
            
			gettimeofday(&upgrade->end_time, NULL);
            cost_time = (upgrade->end_time.tv_sec - upgrade->start_time.tv_sec);
            
            LOGI("\n QIWO upgrade sucess time is %ds \n", cost_time);

            if(currentFlag == 'sui=')
            {
                system_set_led("red", 1, 0, 0);
            }
            else if(currentFlag == 'ato=')
            {
                if(currentRemoteConfig)
                {
                    char value[200];
                    char url[300];
                    char *update_buffer = data + UPDATE_BUFFER_OFFSET;
                    int count = 0;
                    int ret = 0;

                    if (currentRemoteConfig->country_info == 1) 
                    {
                        snprintf(url, strlen(url), "%s%s", currentRemoteConfig->urlPrefix, currentRemoteConfig->baseUrl);
                    }
                    else
                    {
                        snprintf(url, strlen(url), "%s%s", currentRemoteConfig->urlPrefix, currentRemoteConfig->intbUrl);
                    }
                    
                    snprintf(value, strlen(value), SU_QUERY_FORMAT, currentRemoteConfig->deviceID, FINISH_UPDATE);
                    LOGI("The value update is: %s\n", value);
                    
                    while(ret = net_ota_update(update_buffer, value, currentRemoteConfig->urlSetUpdateStep, url)) {
                        LOGI("Update start  status failed.\n");
                        if (count < NET_COUNT) {
                            sleep(NET_RETRY_BROKEN_TIME);
                            ++count;
                        } else {
                            system_set_led("red", 300, 200, 1);
                            break;
                        }
                    }
                }
                system_set_led("red", 1, 0, 0);
                sleep(3);
            }
    
			break;
		case UPGRADE_WRITE_STEP:
			printf("\r QIWO %s image -- %d %% complete", image_name[image_num], state_arg);
			fflush(stdout);
			break;
		case UPGRADE_VERIFY_STEP:
			if (state_arg)
            {         
				printf("\n QIWO %s verify failed %x \n", image_name[image_num], state_arg);
			}
            else
            {         
				printf("\n QIWO %s verify success\n", image_name[image_num]);
            }
            break;
		case UPGRADE_FAILED:
            LOGI("QIWO upgrade %s failed %d \n", image_name[image_num], state_arg);
			
			if(currentFlag == 'sui=')
            {
                system_set_led("red", 300, 200, 1);
                while(1);
            }
            else if(currentFlag == 'ato=')
            {
                system_set_led("red", 300, 200, 1);
                sleep(3);
            }
			break;
	}

	return;
}

int main(int argc, char** argv)
{
	unsigned int flag;
	char *path = NULL;
	char cmd[256];
	int ret = SUCCESS;

	struct upgrade_info_t config;

	flag = get_boot_flag();

#ifdef UPGRADE_TEST_MOD

    if(argc > 1)
    {
        if (!strcmp(argv[1], "ius"))
        {
            flag = 'sui=';
            upgradeMode = 0;
            LOGI("qwupgrade ius\n");
        }
        else if ( !strcmp(argv[1], "local") )
        {
            flag = 'ato=';
            upgradeMode = 1;
            LOGI("qwupgrade local\n");
        }
        else if ( !strcmp(argv[1], "remote") )
        {
            flag = 'ato=';
            upgradeMode = 2;
            LOGI("qwupgrade remote\n");
        } 
       // else if ( !strcmp(argv[1], "default") )
        else
        {
            flag = 'ato=';
            upgradeMode = 3;
            LOGI("qwupgrade default\n");
        }
    }
    else
    {
        flag = 'ato=';
        upgradeMode = 3;
        LOGI("qwupgrade default\n");
    }
    
#endif

    currentFlag = flag;

    switch(flag) {
		case 'sui=':
			path = NULL;
            LOGI("qwupgrade ius\n");
            system_set_led("red", 800, 500, 1);
			break;
			
		case 'ato=':
            system_set_led("green", 1, 0, 0);
			ret = get_qwupgrade_config(&config);
			if(ret != 0)
			{
                system_set_led("red", 300, 200, 1);
                sleep(3);
			}
			else
			{
			    system("mkdir /mnt/sd0");
                system("mount /dev/mmcblk0p1 /mnt/sd0");
    
				switch(config.otaType)
				{
					case OTA_LOCAL:
                        LOGI("qwupgrade OTA_LOCAL\n");
						ret = ota_update_local(&(config.otaInfo.local));
						break;
					case OTA_REMOTE:
                        LOGI("qwupgrade OTA_REMOTE\n");
						ret = ota_update_remote(&(config.otaInfo.remote), config.version);
                        currentRemoteConfig = &(config.otaInfo.remote);
                        break;
						
					default:
                        LOGI("qwupgrade default_ota_updata\n");
				        ret = default_ota_updata(&config);
						break;
				};
			}
            if(access(OTA_IUS_PATH, F_OK) == 0)
            {
				path = OTA_IUS_PATH;
            }
            else
            {
                LOGI("can not find %s, ota faild\n", OTA_IUS_PATH);
            }
			break;
		default:
			if (item_exist("board.disk") && item_equal("board.disk", "emmc", 0)) {
				sprintf(cmd, "mount -t vfat /dev/mmcblk1p1 /mnt/mmc");
			} else {
				sprintf(cmd, "mount -t vfat /dev/mmcblk0p1 /mnt/mmc");
			}
			system(cmd);
			if(access(MMC_IUS_PATH, F_OK) == 0)
				path = MMC_IUS_PATH;
			break;
	}

    if(ret != SUCCESS)
    {
        return FAILED;
    }

	system_update_upgrade(path, upgrade_state_callback_qiwo, NULL);

    switch(flag) {
		case 'sui=':
			printf("\n Remove ius card to enter the new system.\n");
			break;
		default:
			printf("\n Remove mmc card to enter the new system.\n");
			break;
	}
	if (item_exist("board.disk") && item_equal("board.disk", "emmc", 0)) {
		while(!access("/dev/mmcblk1p1", F_OK));
	} else {
		while(!access("/dev/mmcblk0p1", F_OK));
	}
	system_update_clear_flag(UPGRADE_FLAGS_OTA);
    
    ret = access("/mnt/sd0/update.zip", F_OK);
	if(ret == 0)
	{
		system("rm /mnt/sd0/update.zip");
	}
    
	reboot(LINUX_REBOOT_CMD_RESTART);

	return SUCCESS;
}

