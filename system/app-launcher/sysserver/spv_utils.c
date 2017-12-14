#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <regex.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <linux/fb.h>
#include <sys/prctl.h>

#include "spv_utils.h"
#include "spv_config_keys.h"
#include "spv_debug.h"
#include "spv_common.h"
#include "spv_language_res.h"
#include "demuxer_interface.h"
#include "av_demuxer_interface.h"
#include "file_dir.h"
#include "spv_indication.h"

#include "qsdk/sys.h"
#include "qsdk/demux.h"
#include "qsdk/audiobox.h"

#define GSENSOR_ENABLE_NAME "/sys/class/misc/%s/attribute/enable"

#define CODEC_VOLUME_MAX 255

#define LCD_BACKLIGHT_MAX 255

#define BATTERY_FULL			1
#define BATTERY_DISCHARGING		2
#define BATTERY_CHARGING		3

#define SD_DEV_NODE "/dev/mmcblk0"
#define SD_PARTITION_NODE "/dev/mmcblk0p1"


int SpvRunSystemCmd(char *cmd)
{
    pid_t status;  
    INFO("SpvRunSystemCmd, cmd: %s \n", cmd);
    status = system(cmd);

    if (-1 == status) {
        ERROR("system error, cmd: %s\n", cmd);
    } else {
        if (WIFEXITED(status)) {
            if (0 == WEXITSTATUS(status)) {
                return 0;
            } else {
                ERROR("run shell script fail, script exit code: %d\n", WEXITSTATUS(status));  
            }
        } else {   
            ERROR("exit status = [%d]\n", WEXITSTATUS(status));  
        }
    }
    return -1;
}

int SetTimeByRawData(time_t rawtime)
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	tv.tv_sec = rawtime;
	tv.tv_usec = 0;
	int result = SetTimeToSystem(tv, tz);
	return result;
}

int SetTimeToSystem(struct timeval tv, struct timezone tz)
{
    int ret = settimeofday(&tv, &tz);
    ret |= SpvRunSystemCmd("hwclock -w");
    return ret;
}

int SpvGetWifiSSID(char *ssid)
{
#ifdef GUI_WIFI_ENABLE
    if(ssid == NULL)
        return -1;

    return get_wifi_ssid(ssid);
#else
    return -1;
#endif
}

int SpvSetBacklight(char *newvalue)
{
    char value[128] = {0};
    if(newvalue != NULL) {
        strcpy(value , newvalue);
    } else {
        GetConfigValue(GETKEY(ID_SETUP_DISPLAY_BRIGHTNESS), value);
    }
    INFO("spv set backligh, value: %s\n", value);
    int backlight = 0;
    if(!strcasecmp(value, GETVALUE(STRING_HIGH))) {
        backlight = LCD_BACKLIGHT_MAX * 5 / 6;
    } else if(!strcasecmp(value, GETVALUE(STRING_MEDIUM))) {
        backlight = LCD_BACKLIGHT_MAX * 3 / 6;
    } else if(!strcasecmp(value, GETVALUE(STRING_LOW))) {
        backlight = LCD_BACKLIGHT_MAX * 1 / 6;
    }  
    return system_set_backlight(backlight); 
}

int SpvGetBacklight()
{
    return system_get_backlight();
}

int SpvLcdBlank(int blank)
{
    return system_enable_backlight(blank);
}

int SpvSetLed(LED_TYPE led, int time_on, int time_off, int blink_on)
{
    char *ledname = NULL;
    int ret = -1;
    switch(led) {
        case LED_WORK:
            ledname = "led_work";
            break;
        case LED_WORK1:
            ledname = "led_work1";
            break;
        case LED_WIFI:
            ledname = "led_wifi";
            break;
        case LED_BATTERY:
            ledname = "led_battery";
            break;
        case LED_BATTERY_G:
            ledname = "led_battery_g";
            break;
        case LED_BATTERY_B:
            ledname = "led_battery_b";
            break;
        default:
            break;
    }
    if(ledname != NULL) {
        ret = system_set_led(ledname, time_on, time_off, blink_on);
    }

    if(ret) {
        ERROR("set led failed, led:%s\n", ledname);
    }
    return ret;
}

int SpvGetBattery()
{
    battery_info_t bati;
    system_get_battry_info(&bati);
    return bati.capacity;
}

int SpvAdapterConnected()
{
    ac_info_t aci;
    system_get_ac_info(&aci);
    return aci.online;
}

int SpvPowerByGsensor()
{
    enum boot_reason reason;
    reason = system_get_boot_reason();
    switch(reason) {
        case BOOT_REASON_ACIN:
            INFO("power up by ac in\n");
            break;
        case BOOT_REASON_EXTINT:
            INFO("power up by ac extern irq\n");
            break;
        case BOOT_REASON_POWERKEY:
            INFO("power up by press power key\n");
            break;
        case BOOT_REASON_UNKNOWN:
        default:
            INFO("power up reason unknown\n");
            break;
    }

    return reason == BOOT_REASON_EXTINT;
}

int SpvIsCharging()
{
    int ret = 0;
    battery_info_t bati;
    system_get_battry_info(&bati);

    if (strcasestr(bati.status, "full")) {
        ret = BATTERY_FULL;
    } else if (strcasestr(bati.status, "discharging")){
        ret = BATTERY_DISCHARGING;
    } else if (strcasestr(bati.status, "charging")){
        ret = BATTERY_CHARGING;
    }

    return ret == BATTERY_CHARGING;
}

void SpvSetIRLED(int closeLed, int isDaytime)
{
    static int IRStatus = -99;
    int status = -100;
    if(!closeLed) {
        char value[128] = {0};
    //    GetConfigValue(GETKEY(ID_SETUP_IR_LED), value);
        INFO("spv set ir led, value: %s\n", value);
        if(!strcasecmp(value, GETVALUE(STRING_AUTO))) {
            if(isDaytime) {
                status = 0;
            } else {
                status = 1;
            }
        } else if(!strcasecmp(value, GETVALUE(STRING_ON))) {
            status = 1;
        } else if(!strcasecmp(value, GETVALUE(STRING_OFF))) {
            status = 0;
        } else {
            status = 0;
        }
    } else {
        status = 0;
    }

    if(IRStatus != status) {
        //fill_light_switch(status);
        IRStatus = status;
    }
}

int SpvUpdateGpsData(GPS_Data *data)
{
    GPS_Data defaultData = {0, 0, 0};
    GPS_Data *pData;
    if(data != NULL) {
        pData = data;
    } else {
        pData = &defaultData;
    }

    //Inft_gps_setData(pData);
    return 0;
}

static int get_audio_handle(void)
{
	int handle;
	char pcm_name[32] = "default";
	audio_fmt_t fmt = {
		.channels = 2,
		.bitwidth = 32,
		.samplingrate = 16000,
		.sample_size = 1024,
	};

	handle = audio_get_channel(pcm_name, &fmt, CHANNEL_BACKGROUND);
	if (handle < 0) {
		ERROR("get audio channel err:%d\n", handle);
		return -1;
	}

	return handle;
}

//audiobox_set_volume(struct event_scatter * event)
int SpvSetVolume(int volume_percent)
{
	int handle, ret;

	/*handle = get_audio_handle();
	if (handle < 0) {
		ERROR("[launcher] %s get audio handle failed\n", __func__);
		return -1;
	}

	ret = audio_set_volume(handle, volume);
	if (ret < 0) {
		ERROR("[launcher] set volume failed\n");
	}
	audio_put_channel(handle);*/
    int volume = volume_percent * 3 / 10 + 70;

    INFO("set volume: %d\n", volume_percent);
    ret = audio_set_master_volume("default", volume, 0);
	return ret;
}

int SpvGetVolume(void)
{
    /*int handle, volume;

    handle = get_audio_handle();
    if (handle < 0) {
	printf("%s get audio handle failed\n", __func__);
	return -1;
    }

    volume = audio_get_volume(handle);
    if (volume < 0) {
	    ERROR("get volume: %d\n", volume);
    }

    audio_put_channel(handle);*/
    int volume = audio_get_master_volume("default", 0);
    if(volume < 0) {
        return -1;
    }
    int percent = (volume - 70) * 10 / 3;

    return percent;
}

static int safe_read(int fd, void *buf, size_t count)
{
	int result = 0, res;

	while (count > 0) {
		if ((res = read(fd, buf, count)) == 0) 
			break;
		if (res < 0)
			return result > 0 ? result : res;
		count -= res;
		result += res;
		buf = (char *)buf + res;
	}

	return result;
}

static int get_buffer_size(audio_fmt_t *fmt)
{
	int bitwidth;

	if (!fmt)
		return -1;
	
	bitwidth = ((fmt->bitwidth > 16) ? 32 : fmt->bitwidth);
	return (fmt->channels * (bitwidth >> 3) * fmt->sample_size);
}

static int play_audio(char *filename)
{
	int handle;
	int len, fd, bufsize, ret;
	char pcm_name[32] = "default";
	char audiobuf[8192];
	audio_fmt_t real;

    prctl(PR_SET_NAME, __func__);
	handle = get_audio_handle();
	if (handle < 0) {
		ERROR("get audio handle failed\n");
		return -1;
	}

	ret = audio_get_format(pcm_name, &real);
	if (ret < 0) {
		ERROR("get format err:%d\n", ret);
		return -1;
	}
	bufsize = get_buffer_size(&real);
	INFO("[launcher] bufsize : %d\n", bufsize);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		ERROR("open filename : %s failed\n", filename);
		return -1;
	}

	do {
		len = safe_read(fd, audiobuf, bufsize);
		if (len < 0)
			break;

		ret = audio_write_frame(handle, audiobuf, len);
		if (ret < 0)
			break;
	} while (len == bufsize);

	close(fd);
	audio_put_channel(handle);

	return 0;
}


#define PWM_IOCTL_SET_FREQ 1
#define PWM_IOCTL_STOP 0

pthread_mutex_t g_buzzer_lock = PTHREAD_MUTEX_INITIALIZER;

static int PlaySoundByBuzzer(int freq, int duration, int interval, int times)
{
    pthread_mutex_lock(&g_buzzer_lock);
    int m_fd = open("/dev/beep", O_RDONLY);
    if(m_fd < 0) {
        ERROR("open /dev/beep failed\n");
        pthread_mutex_unlock(&g_buzzer_lock);
        return 0;
    }
    int i = 0;

    ioctl(m_fd, PWM_IOCTL_STOP);
    for(i = 0; i < times; i ++) {
        ioctl(m_fd, PWM_IOCTL_SET_FREQ, freq);
        usleep(duration * 1000);
        ioctl(m_fd, PWM_IOCTL_STOP);
        usleep(interval * 1000);
    }
    close(m_fd);
    pthread_mutex_unlock(&g_buzzer_lock);
}

static void PlayBuzzerUnit(BuzzerUnit *unit)
{
#ifdef GUI_SOUND_BUZZER
    if(unit == NULL)
        return;
    prctl(PR_SET_NAME, __func__);
    PlaySoundByBuzzer(unit->freq, unit->duration, unit->interval, unit->times);
    free(unit);
#endif
}


/**
 * Ring tone playing interface
 * if have speaker, the param is WAV ID
 * else the param must be the BuzzerUnit pointer
 **/
void SpvPlayAudio(int param)
{
#ifndef GUI_SOUND_BUZZER
    if(param < WAV_BEGIN || param > WAV_END) {
        ERROR("invalid wav id: %d\n", param);
        return;
    }
    extern const char *SPV_WAV[];
    SpvCreateServerThread((void *)play_audio, SPV_WAV[param]);
#else
    if(param == 0)
        return;

    SpvCreateServerThread((void *)PlayBuzzerUnit, (void *)param);
#endif
}

static void TiredAlarmThread(char *wav_path)
{
    prctl(PR_SET_NAME, __func__);
    int count = 0;
    while(1) {
        if(count == 0) {
            count++;
            sleep(60 * 60);//first 1 hour alarm
        } else {
            sleep(30 * 60);//then every 30 minutes alarm
        }
        INFO("TiredAlarm: %s\n", wav_path);
        int i = 0, j = 0;
        for(i = 0; i < 3; i ++) {
            for(j = 0; j < 3; j ++) {
#ifdef GUI_SOUND_BUZZER
                PlaySoundByBuzzer(1, 200, 0, 1);
#else
                play_audio(wav_path);
#endif
                INFO("%s, i: %d, j: %d\n", wav_path, i, j);
                usleep(300000);
            }
            usleep(500000);
        }
    }
}

void SpvTiredAlarm(const char *wav_path)
{
    SpvCreateServerThread((void *)TiredAlarmThread, (void *)wav_path);
}

unsigned long int SpvCreateServerThread(void (*func(void *arg)), void *args)
{
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    INFO("pthread_create\n");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &attr, func, args);
    pthread_attr_destroy (&attr);
    return t;
}

void SpvShutdown()
{
	reboot(RB_POWER_OFF);
}

int SpvGetSdcardStatus()
{
    int status = SD_STATUS_UMOUNT;
    disk_info_t info;
    info.name = SD_DEV_NODE;
    int ret = system_get_disk_status(&info);
    if(!ret) {
        if(info.mounted) {
            status = SD_STATUS_MOUNTED;
        } else if(info.inserted) {
            status = SD_STATUS_ERROR;
        }
    }

    INFO("status: %d\n", status);
    return status;
}

int SpvFormatSdcard()
{
    int ret = 0;
    char mount_point[128] = {0};
    realpath(EXTERNAL_STORAGE_PATH, mount_point);
    if(access(SD_DEV_NODE, F_OK)) {
        ERROR("sd node not exist, %s\n", SD_DEV_NODE);
        return -1;
    }

    char cmd[256] = {0};
    //sprintf(cmd, "umount %s; dd if=/dev/zero of=%s bs=1M count=3 seek=15;sync;busybox fdisk %s << EOF\no\nn\np\n1\n\n\nt\nb\nw\nEOF;busybox mkfs.vfat %s; mount %s %s", mount_point, SD_DEV_NODE, SD_DEV_NODE, SD_PARTITION_NODE, SD_PARTITION_NODE, mount_point);
    sprintf(cmd, "umount %s; busybox mkfs.vfat %s; mount %s %s", mount_point, SD_PARTITION_NODE, SD_PARTITION_NODE, mount_point);
    ret = SpvRunSystemCmd(cmd);

    //int ret = system_format_storage(mount_point);
    INFO("Format Sdcard, mount_point: %s, ret: %d\n", mount_point, ret);
    return ret;
}

int SpvUnlinkFile(char *filename)
{
    int ret = unlink(filename);
    if(ret) {
        ERROR("unlink file failed, file: %s, err: %s\n", filename, strerror(errno));
        return errno;
    }
    return ret;
}

int IsCardvEnable()
{
    char value[128] = {0};
    GetConfigValue(GETKEY(ID_SETUP_CARDV_MODE), value);
    int cardvEnable = strcasecmp(value, GETVALUE(STRING_ON)) ? 0 : 1;
    return cardvEnable;
}

int SpvSetParkingGuard(char *newvalue)
{
    char value[128] = {0};
    int enable_gsensor = 0;
    if(IsCardvEnable()) {
        if(newvalue != NULL) {
            strcpy(value, newvalue);
        } else {
            GetConfigValue(GETKEY(ID_SETUP_PARKING_GUARD), value);
        }
        INFO("SpvSetParkingGuard, enable: %d\n", strcasecmp(value, GETVALUE(STRING_OFF)));
        enable_gsensor = strcasecmp(value, GETVALUE(STRING_OFF)) ? 1 : 0;
    }

    enum externel_device_type dev_type = EXT_DEVICE_GSENSOR;
    char *sensor = getenv("GSENSOR_MODEL");
    if(sensor == NULL) {
        ERROR("get gsensor mode failed\n");
        return;
    }

    //sprintf(dev_name, GSENSOR_ENABLE_NAME, sensor);
    int ret = system_enable_externel_powerup(dev_type, sensor, enable_gsensor);

    if(ret) {
	    ERROR("SpvSetParkingGuard failed, devname: %s, enable: %d!\n", sensor, enable_gsensor);
    }

    return ret;
}

//Get available space, in KByte
unsigned int SpvGetAvailableSpace()
{
    if(!IsSdcardMounted()) {
        return 0;
    }

    storage_info_t sinfo = {0};
    char mount_point[128] = {0};
    realpath(EXTERNAL_STORAGE_PATH, mount_point);
    system_get_storage_info(mount_point, &sinfo);

    return sinfo.free;
}

//Get video time remain, in Seconds
int GetVideoTimeRemain(long availableSpace, char *resolution)
{
    /*float mps = MPS_1080FHD;
    if(!strcasecmp(GETVALUE(STRING_2448_30FPS), resolution)) {
        mps = MPS_2448_30FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_1080P_60FPS))) {
        mps = MPS_1080P_60FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_1080P_30FPS))) {
        mps = MPS_1080P_30FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_720P_30FPS))) {
        mps = MPS_720P_30FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_3264))) {
        mps = MPS_3264;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_2880))) {
        mps = MPS_2880;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_2048))) {
        mps = MPS_2048;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_1920))) {
        mps = MPS_1080P_60FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_1472))) {
        mps = MPS_1080P_30FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_1088))) {
        mps = MPS_720P_30FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_768))) {
        mps = MPS_720P_30FPS;
    }*/
    int kBps = get_recorder_bitrate() >> 13;
    return availableSpace / kBps;
}

//Get picture number remain, in Piece
int GetPictureRemain(long availableSpace, char *resolution)
{
    float megapixel;
    /*if(!strcasecmp(resolution, GETVALUE(STRING_16M))) {
        megapixel = MPP_16M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_10M))) {
        megapixel = MPP_10M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_8M))) {
        megapixel = MPP_8M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_4M))) {
        megapixel = MPP_4M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_1088))) {
        megapixel = MPP_4M;
    }*/
    megapixel = atoi(resolution) * 0.1f;
    int piece = 0;
    piece = (int)((double)(availableSpace >> 10) / megapixel);
    return piece;
}

//Convert the time in seconds to string format HH:MM:SS
void TimeToString(int time, char *ts)
{
    int h = 0;
    int m = 0;
    int s = 0;
    int tmp = 0;

    if(ts == NULL)
        return;

    s = time % 60;
    tmp = time / 60;
    m = tmp % 60;
    h = tmp / 60;
    sprintf(ts, "%03d:%02d:%02d", h, m, s);
}

void TimeToStringNoHour(int time, char *ts)
{
    int m = 0;
    int s = 0;

    if(ts == NULL)
        return;

    s = time % 60;
    m = time / 60;
    sprintf(ts, "%02d:%02d", m, s);
}

int listDir(char *path) 
{ 
    DIR *pDir ; 
    struct dirent *ent  ; 
    int i=0  ; 
    char childpath[512] = {0};
    int fileCount = 0;

    pDir=opendir(path); 
    if(pDir == NULL)
        return 0;

    memset(childpath,0,sizeof(childpath)); 

    while((ent=readdir(pDir))!=NULL) 
    { 

        if(ent->d_type & DT_DIR) 
        { 

            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) 
                continue; 

            sprintf(childpath,"%s/%s",path,ent->d_name); 
            //INFO("path:%s\n",childpath); 

            fileCount += listDir(childpath); 

        } 
        else
        {
            MEDIA_TYPE type = GetFileType(ent->d_name);
            if(type == MEDIA_PHOTO) {
                //INFO("picture file found: %s\n", ent->d_name);
                fileCount ++;
            }
        }
    }
    closedir(pDir);
    //INFO("found %d pictures in %s\n", fileCount, path);
    return fileCount;
}  

int GetPictureCount()
{
    return listDir(EXTERNAL_MEDIA_DIR);
}

static void CountMediaFiles(char *folderPath, int *v, int *p)
{
    DIR *pDir;
    struct dirent *ent;
    int i = 0;
    char fileName[128] = {0};
    int fileCount = 0;
    MEDIA_TYPE fileType = MEDIA_UNKNOWN;

    *v = *p = 0;

    pDir = opendir(folderPath);
    if(pDir == NULL)
        return;

    while((ent = readdir(pDir)) != NULL)
    {
        if(!(ent->d_type & DT_DIR))
        {
            strcpy(fileName, ent->d_name);
            fileType = GetFileType(fileName);
            switch(fileType) {
                case MEDIA_VIDEO:
                case MEDIA_VIDEO_LOCKED:
                /*case MEDIA_TIME_LAPSE:
                case MEDIA_SLOW_MOTION:
                case MEDIA_CAR:*/
                    *v = *v + 1;
                    break;
                case MEDIA_PHOTO:
                //case MEDIA_CONTINUOUS:
                //case MEDIA_NIGHT:
                    *p = *p + 1;
                    break;
                default:
                    break;
            }

        }
    }
    closedir(pDir);
}

int GetMediaCount(int *pVideoCount, int *pPictureCount)
{
    char *dirPath = EXTERNAL_MEDIA_DIR;
    DIR *pDir ; 
    struct dirent *ent  ; 
    int i=0  ; 
    char childpath[128] = {0};

    *pVideoCount = *pPictureCount = 0;

    int v = 0, p = 0; 

    pDir=opendir(dirPath); 
    if(pDir == NULL)
        return -1;

    memset(childpath, 0, sizeof(childpath)); 

    while((ent=readdir(pDir))!=NULL) 
    { 

        if(ent->d_type & DT_DIR) 
        { 

            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) 
                continue; 

            if(IsMediaFolder(ent->d_name))
            {
                sprintf(childpath,"%s/%s",dirPath,ent->d_name); 
                v = p = 0;
                CountMediaFiles(childpath, &v, &p);
                *pVideoCount += v;
                *pPictureCount += p;
            }
        } 
    }

    closedir(pDir);
}
/**
 * file node compare.
 * if node1 == node2, return 0.
 * if node1 > node2, return positive.
 * if node1 < node2, return negative.
 **/
int CompareFileNode(PFileNode node1, PFileNode node2)
{
    if(node1 == NULL && node2 != NULL) {
        return -1;
    } else if(node1 == NULL && node2 == NULL) {
        return 0;
    } else if(node2 == NULL) {
        return 1;
    }
    //INFO("folder:%s, filename: %s, folder:%s, filename2:%s\n",node1->folderName, node1->fileName,node2->folderName,  node2->fileName);
    int foldercmp = strcasecmp(node1->folderName, node2->folderName);
    //INFO("foldercmp: %d\n", foldercmp);
    if(!foldercmp) {
        return strcasecmp(node1->fileName, node2->fileName);//ordered by name
    }

    return foldercmp;
}

/**
 * add file to sorted list
 **/
static int AddFile(FileList *list, char *fileName, char *folderName)
{
    if(list == NULL || fileName == NULL || strlen(fileName) <= 0 
            || folderName == NULL || strlen(folderName) <= 0 || strlen(folderName) >= 16) {
        ERROR("error file name or folder name, filename: %s, foldername:%s\n", fileName, folderName);
        return -1;
    }

    PFileNode pNode = (PFileNode) malloc(sizeof(FileNode));
    if(pNode == NULL) {
        ERROR("malloc file node error, add file %s to list failed!\n", fileName);
        return -1;
    }
    memset(pNode, 0, sizeof(FileNode));

    strcpy(pNode->fileName, fileName);
    strcpy(pNode->folderName, folderName);
    pNode->prev = NULL;
    pNode->next = NULL;

    /**
     * add file name orderly
     **/
    if(list->pHead == NULL) {//empty list
        list->pHead = pNode;
        list->pTail = pNode;
        list->fileCount = 1;
        return 0;
    }

    PFileNode pPrevNode = NULL;
    PFileNode pCurrentNode = list->pHead;

    while(pCurrentNode){
        int ret = CompareFileNode(pNode, pCurrentNode);
        if(!ret) {
            INFO("file:%s exist, no need to add!\n", pNode->fileName);
            free(pNode);
            return;
        } else if(ret > 0) {//position found
            if(pPrevNode == NULL) {
                list->pHead = pNode;
            } else {
                pPrevNode->next = pNode;
            }
            pNode->prev = pPrevNode;
            pNode->next = pCurrentNode;
            pCurrentNode->prev = pNode;
            list->fileCount ++; 
            return 0;
        } else {//current node > added node, continue
            pPrevNode = pCurrentNode;
            pCurrentNode = pCurrentNode->next;
        }
    } 

    //list tail, added
    pPrevNode->next = pNode;
    pNode->prev = pPrevNode;
    list->pTail = pNode;
    list->fileCount ++; 
    return 0;
}

int SubstringIsDigital(char *string, int start, int end)
{
    if(string == NULL)
        return 0;
    int len = strlen(string);
    if(len < (start - 1) || len < end || start < 1)
        return 0;
    int i = 0;
    for(i = start - 1; i < end; i ++) {
        if(!isdigit(*(string + i)))
            return 0;
    }
    return 1;
}


int StringRegularMatch(char *string, char *pattern)
{
    if(string == NULL || pattern == NULL || strlen(string) <= 0 || strlen(pattern) <= 0)
        return -1;

    char errbuf[1024] = {0};
    char match[100] = {0};

    regex_t reg;
    int err,nm = strlen(string);
    regmatch_t pmatch[nm];

    if(regcomp(&reg, pattern, REG_EXTENDED) < 0){
        regerror(err,&reg,errbuf,sizeof(errbuf));
        ERROR("err:%s\n",errbuf);
    }

    err = regexec(&reg, string, strlen(string), pmatch, 0);
    if(err == REG_NOMATCH){
        return REG_NOMATCH;
    }else if(err){
        regerror(err,&reg,errbuf,sizeof(errbuf));
        ERROR("err:%s\n",errbuf);
        return -1;
    }

    return 0;
}

MEDIA_TYPE GetFileType(char *fileName)
{
    if(fileName == NULL || strlen(fileName) <= 0)
        return MEDIA_UNKNOWN;

    char suffix[16] = {0};
    char name[128] = {0};

    if(*fileName == '.')
        return MEDIA_UNKNOWN;

    /*char *pNameEnd = fileName + strlen(fileName) - 1;
    while(pNameEnd > fileName) {
        if(*pNameEnd == '.')
            break;

        pNameEnd --;
    }

    if(pNameEnd == fileName)// '.' not found
        return MEDIA_UNKNOWN;*/
    char *pNameEnd = strrchr(fileName, '.');
    if(pNameEnd == NULL) {// '.' not found
        return MEDIA_UNKNOWN;
    }

    strcpy(suffix, pNameEnd+1);

    strncpy(name, fileName, pNameEnd - fileName);
    char *newName = name;
    if(!strncasecmp(name, "LOCK_", 5)) {
        newName = name + 5;
    }
    //if(strlen(newName) < 21 || 
    //        (*(newName + 4) != '_' && *(newName + 9) != '_' && *(newName + 16) != '_' && SubstringIsDigital(newName, 1, 4) && 
    //         SubstringIsDigital(newName, 6, 9) && SubstringIsDigital(newName, 11, 16) && SubstringIsDigital(newName, 18, 21)))
    //    return MEDIA_UNKNOWN;

    if(!strcasecmp("jpg", suffix)) {
        return MEDIA_PHOTO;
    } else if(!strcasecmp("mp4", suffix) || !strcasecmp("mkv", suffix)) {
        //if(*(name + 21) == 's' || *(name + 21) == 'S') {
        if(!strncasecmp(name, "LOCK_", 5)) {
            return MEDIA_VIDEO_LOCKED;
        } else {
            return MEDIA_VIDEO;
        }
    } 
    return MEDIA_UNKNOWN;

    /*    if(!StringRegularMatch(fileName, "^[0-9]{4}_[0-9]{4}_[0-9]{6}_[0-9]{4}[0-9]*.(jpg|JPG)")) {
          return MEDIA_PHOTO;
          } else if(!StringRegularMatch(fileName, "^[0-9]{4}_[0-9]{4}_[0-9]{6}_[0-9]{4}[0-9]*.(mp4|MP4|avi|AVI)")) {
          return MEDIA_VIDEO;
          } else if(!StringRegularMatch(fileName, "^[0-9]{4}_[0-9]{4}_[0-9]{6}_[0-9]{4}[sS].(mp4|MP4|avi|AVI)")) {
          return MEDIA_VIDEO_LOCKED;
          } else {
          return MEDIA_UNKNOWN;
          }
          if(!StringRegularMatch(fileName, ".*.(jpg|JPG)")) {
          return MEDIA_PHOTO;
          } else if(!StringRegularMatch(fileName, "^[0-9]{4}_[0-9]{4}_[0-9]{6}_[0-9]{4}[sS].(mp4|MP4|avi|AVI)")) {
          return MEDIA_VIDEO_LOCKED;
          } else if(!StringRegularMatch(fileName, ".*.(mp4|MP4|avi|AVI)")) {
          return MEDIA_VIDEO;
          } else {
          return MEDIA_UNKNOWN;
          }*/

}


int ScanMediaFiles(FileList *fileList, char *folderPath, char *folderName)
{
    DIR *pDir;
    struct dirent *ent;
    int i = 0;
    char fileName[128] = {0};
    int fileCount = 0;
    MEDIA_TYPE fileType = MEDIA_UNKNOWN;

    pDir = opendir(folderPath);
    if(pDir == NULL)
        return;
    memset(fileName, 0, sizeof(fileName));

    while((ent = readdir(pDir)) != NULL)
    {
        if(!(ent->d_type & DT_DIR))
        {
            strcpy(fileName, ent->d_name);
            fileType = GetFileType(fileName);
            if(fileType != MEDIA_UNKNOWN)
            {
                //if(fileType == MEDIA_NIGHT)
                //    fileType = MEDIA_PHOTO;
                //INFO("media file found, file: %s, folder:%s\n", ent->d_name, folderName);
                AddFile(&fileList[fileType], ent->d_name, folderName);
            }

        }
    }
    closedir(pDir);
}

MEDIA_TYPE GetMediaFolderType(char *folderName)
{
    MEDIA_TYPE type = MEDIA_UNKNOWN;
    if(folderName == NULL)
        return MEDIA_UNKNOWN;

    if(!strcasecmp(folderName, VIDEO_DIR)) {
        type = MEDIA_VIDEO;
    } else if(!strcasecmp(folderName,LOCK_DIR)) {
        type = MEDIA_VIDEO_LOCKED;
    } else if(!strcasecmp(folderName,PHOTO_DIR)) {
        type = MEDIA_PHOTO;
    }

    return type;
}

int IsMediaFolder(char *folderName)
{
    return GetMediaFolderType != MEDIA_UNKNOWN;
}

int ScanFolders(FileList *fileList, char *dirPath)
{
    DIR *pDir ; 
    struct dirent *ent  ; 
    int i=0  ; 
    char childpath[128] = {0};
    int fileCount = 0;

    pDir=opendir(dirPath); 
    if(pDir == NULL)
        return -1;

    memset(childpath, 0, sizeof(childpath)); 

    struct timeval tm, tm1;
    gettimeofday(&tm, 0);

    while((ent=readdir(pDir))!=NULL) 
    { 

        if(ent->d_type & DT_DIR) 
        { 
            //INFO("folder: %s\n", ent->d_name);

            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) 
                continue; 
            if(IsMediaFolder(ent->d_name)) {
                sprintf(childpath,"%s/%s",dirPath,ent->d_name); 
                //INFO("path:%s\n",childpath); 
                ScanMediaFiles(fileList, childpath, ent->d_name);
            }
        } 
    }
    gettimeofday(&tm1, 0);
    INFO("scan media files, time lapse: %f seconds\n", ((tm1.tv_sec - tm.tv_sec) + (float)(tm1.tv_usec - tm.tv_usec) / 1000000));
    closedir(pDir);
    return 0;
}


void FreeFileList(FileList *fileList)
{
    if(fileList == NULL)
        return;

    FileNode *pNode = fileList->pHead;
    fileList->pHead = NULL;
    fileList->pTail = NULL;

    FileNode *pCur;
    while(pNode) {
        pCur = pNode;
        pNode = pNode->next;
        free(pCur);
    }

    fileList->fileCount = 0;
    memset(fileList, 0, sizeof(fileList));
}

/**
 * mkdir for external media directory.
 * 0 succeed, other failed
 **/
int MakeDirRecursive(char *dirpath)
{
    int i = 0;
    INFO("dirpath: %s\n", dirpath);
    for(i=1; i < strlen(dirpath); i++) {
        if(dirpath[i]=='/') {
            dirpath[i] = 0;
            if(access(dirpath, F_OK) != 0) {
                if(mkdir(dirpath, 0755) == -1) {
                    perror("mkdir error");
                    return -1;
                }
            }
            dirpath[i] = '/';
        }
    }
    return 0;
}


int copy_file(const char* src, const char* dst, int use_src_attr)
{
    static int errorCount;
    char buff[4096] = {0};
    int fd_s = open(src, O_RDONLY);
    int fd_d;
    if(fd_s < 0){
        ERROR("cp_file, open src file '%s' failed .\n", src);
        goto err_open_src;
    }
    fd_d = open(dst, O_WRONLY|O_TRUNC|O_CREAT,0777);
    if(fd_d < 0){
        ERROR("cp_file, open dst file '%s' failed .\n", dst);
        goto err_open_dst;
    }
    int ret_r, ret_w;
    while((ret_r=read(fd_s, buff, 4096)) > 0){
        ret_w = write(fd_d, buff, ret_r);
        if(ret_w != ret_r){
            errorCount ++;
            ERROR("*****ERROR*********, copy file %s to %s, try again\n", src, dst);
            goto err_rw;
        }
    }
    close(fd_d);
    close(fd_s);
    errorCount = 0;
//    if(use_src_attr)
//        cp_attr(src, dst);
    return 0;
err_rw:
    close(fd_d);
    if(errorCount < 1)
        copy_file(src, dst, 1);
//    unlink(dst);
err_open_dst:
    close(fd_s);
err_open_src:
    return -1;
}

int SpvGetVersion(char *version)
{
    if(version == NULL) {
        ERROR("version == NULL\n");
        return -1;
    }

    char tmpVersion[128] = {0};
    int ret = system_get_version(tmpVersion);
    if(ret) {
        strcpy(version, SPV_VERSION);
    } else {
        sscanf(tmpVersion, "%s", version);
    }
    INFO("version: %s\n", version);
    return ret;
}

int SpvEnterCalibrationMode()
{
    system("echo 0 > /sys/bus/platform/drivers/dwc_otg/is_host");
	SpvIndication(IND_DEVICE_MODE, 0, 0);
}
