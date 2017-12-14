#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/statfs.h>
#include <sys/wait.h>
#include<fcntl.h>
#include<unistd.h>
#include<regex.h>
#include<pthread.h>
#include<errno.h>

#include "spv_utils.h"
#include "spv_item.h"
#include "spv_language_res.h"
#include "spv_debug.h"
#include "spv_wav_res.h"
#ifndef UBUNTU
#include "demuxer_interface.h"
#include "av_demuxer_interface.h"
#endif


int SpvSetResolution()
{
#if 0//def GUI_SET_RESOLUTION_SEPARATELY
    char value[128] = {0};
    GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), value);
    set_video_resolution(value);
#endif
    return 0;
}

int SetTimeToSystem(struct timeval tv, struct timezone tz)
{
    int ret = settimeofday(&tv, &tz);
    ret |= SpvRunSystemCmd("hwclock -w");
    return ret;
}

int SpvTellSysGoingToUnmount(SYS_COMMAND cmd)
{
    const char *FIFO_NODE = "/tmp/uififo";
    const char *COMMAND = (cmd == CMD_UNMOUNT_BEGIN) ? "1":"0";
    int fd; 
    int nwrite;

    fd=open(FIFO_NODE, O_WRONLY|O_NONBLOCK, 0);
    if(fd==-1) {
        INFO("open fifo error\n");
        if(errno==ENXIO) {
            INFO("open error;no reading process\n");
        }
        return 0;
    }

    if((nwrite=write(fd, COMMAND, 64))==-1)
    {
        INFO("write to fifo error\n");
        if(errno==EAGAIN) {
            ERROR("The FIFO has not been read yet. Please try later\n");
        }
        close(fd);
        return -1;
    } 
    INFO("write %s to the FIFO\n", COMMAND);
    close(fd);
    return 0;
}

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

int SpvGetVideoDuration(char *videoPath)
{
#ifndef UBUNTU
    int ret = 0;
    DemuxerContext demuxer_context;
    StreamInfo  stream_info;
    ret = stream_open(videoPath, &demuxer_context);
    if(ret < 0) {
        INFO("openfile: %s  failed", videoPath);
        return -1;
    }

    stream_get_info(demuxer_context, &stream_info);
    INFO("SpvGetVideoDuration, duration: %lld\n", stream_info.duration);
    if(stream_info.video_info.keyframe_index_entries != NULL) {
        free(stream_info.video_info.keyframe_index_entries);
    }
    stream_close(demuxer_context);
    
    float dur = (float)stream_info.duration / 1000000;
    if(dur > 0 && dur < 1)
        dur = 1.0f;
    return (int)dur;
#else
    return 100;
#endif
}

int SpvGetWifiSSID(char *ssid)
{
    if(ssid == NULL)
        return -1;

    char *prefix = "INFOTM_cardv_";
    char buf[64] = {0};
    char *cmd = "ifconfig |grep wlan0| awk '{print $5}'| awk -F':' '{print $4 $5 $6}'";
    FILE *stream = popen(cmd, "r");
    if(stream != NULL && fgets(buf, sizeof(buf), stream) != NULL) {//wifi mac address, the last 6 bits
        INFO("mac addr: %s\n", buf);
        if(strlen(buf) >= 6) {
            snprintf(ssid, strlen(prefix) + 7, "%s%s", prefix, buf);
            INFO("SpvGetWifiSSID, ssid: %s\n", ssid);
            pclose(stream);
            return 0;
        }
        ERROR("error ssid, wifi may no enable\n");
    } 
    if(stream != NULL)
        pclose(stream);

    return -1;
}

void SpvSetBacklight()
{
    char value[128] = {0};
    GetConfigValue(GETKEY(ID_DISPLAY_BRIGHTNESS), value);
    INFO("spv set backligh, value: %s\n", value);
    if(!strcasecmp(value, GETVALUE(STRING_HIGH))) {
        set_backlight(LCD_BACKLIGHT_MAX * 6/*5*/ / 6); 
    } else if(!strcasecmp(value, GETVALUE(STRING_MEDIUM))) {
        set_backlight(LCD_BACKLIGHT_MAX * 2/*3*/ / 6);// changed by linyun.xiong for reduce power @2015-09-02 
    } else if(!strcasecmp(value, GETVALUE(STRING_LOW))) {
        set_backlight(LCD_BACKLIGHT_MAX * 1 / 6); 
    }  
}

void SpvSetIRLED(int closeLed, int isDaytime)
{
    static int IRStatus = -99;
    int status = -100;
    if(!closeLed) {
        char value[128] = {0};
        GetConfigValue(GETKEY(ID_SETUP_IR_LED), value);
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
        fill_light_switch(status);
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

    Inft_gps_setData(pData);
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

static void PlaySoundThread(int soundId)
{
#ifdef GUI_SOUND_BUZZER
    INFO("PlaySound: %s\n", soundId);
    int freq = 0, duration = 0, interval = 0, times = 0;
    switch(soundId) {
        case WAV_BOOTING:
            freq = 200;
            duration = 100;
            interval = 50;
            times = 5;
            break;
        case WAV_ERROR:
            freq = 9;
            duration = 100;
            interval = 0;
            times = 1;
            break;
        case WAV_PHOTO:
            freq = 500;
            duration = 100;
            interval = 0;
            times = 1;
            break;
        case WAV_PHOTO3:
            freq = 500;
            duration = 100;
            interval = 100;
            times = 3;
            break;
        case WAV_LDWS_WARN:
        case WAV_FCWS_WARN:
            freq = 9;
            duration = 100;
            interval = 50;
            times = 7;
            break;
        default://WAV_KEY
            freq = 1;
            duration = 100;
            interval = 0;
            times = 1;
            break;
    }
    PlaySoundByBuzzer(freq, duration, interval, times);
#endif
}


void SpvPlayAudio(const char *wav_path)
{
#ifndef GUI_SOUND_BUZZER
    play_audio(wav_path);
#else
    extern char *SPV_WAV[];
    int soundId = WAV_KEY;
    if(!strcasecmp(wav_path, SPV_WAV[WAV_BOOTING])) {
        soundId = WAV_BOOTING;
    } else if(!strcasecmp(wav_path, SPV_WAV[WAV_ERROR])) {
        soundId = WAV_ERROR;
    } else if(!strcasecmp(wav_path, SPV_WAV[WAV_PHOTO])) {
        soundId = WAV_PHOTO;
    } else if(!strcasecmp(wav_path, SPV_WAV[WAV_PHOTO3])) {
        soundId = WAV_PHOTO3;
    } else if(!strcasecmp(wav_path, SPV_WAV[WAV_LDWS_WARN])) {
        soundId = WAV_LDWS_WARN;
    } else if(!strcasecmp(wav_path, SPV_WAV[WAV_FCWS_WARN])) {
        soundId = WAV_FCWS_WARN;
    }
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    INFO("pthread_create, SpvPlayAudio\n");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &attr, (void *)PlaySoundThread, (void *)soundId);
    pthread_attr_destroy (&attr);
#endif
}

static void TiredAlarmThread(char *wav_path)
{
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
#ifndef UBUNTU
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    INFO("pthread_create, SpvTiredAlarm\n");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &attr, (void *)TiredAlarmThread, (void *)wav_path);
    pthread_attr_destroy (&attr);
#else
    INFO("SpvPlayAudio: %s\n", wav_path);
#endif
}

void SpvShutdown()
{
    int i = 0;
    char cmd[100] = {0};
    char buf[1024] = {0};
    char C_SD_MOUNT_POINT[] = "/mnt/mmc";
    FILE *streamList = NULL;
    FILE *stream = NULL;

    SpvTellSysGoingToUnmount(CMD_UNMOUNT_BEGIN);
    //try to unmount the sdcard 
    sprintf(cmd, "mount | grep %s", C_SD_MOUNT_POINT);
    streamList = popen(cmd, "r");
    if(streamList != NULL && fgets(buf, sizeof(buf), streamList) != NULL) {//mount point exist
        INFO("mount list: %s\n", buf);
        for(i = 0; i < 3; i ++) {
            sprintf(cmd, "umount %s; echo $?", C_SD_MOUNT_POINT);
            stream = popen(cmd, "r");
            if(stream != NULL) {//umount succeed
                if(fgets(buf, sizeof(buf), stream) != NULL && buf[0] == '0') {
                    INFO("umount succeed\n");
                    break;
                } else {
                    INFO("umount failed: %s\n", buf);
                }
            }
        }
    } else {
        INFO("No sdcard mounted, just shutdown\n");
    }

    if(streamList != NULL)
        pclose(streamList);

    if(stream != NULL)
        pclose(stream);

    //umount /mnt/config
    SpvRunSystemCmd("umount /mnt/config/");
    infotm_shutdown();
}

int SpvGetBattery()
{
    int capacity = (check_battery_capacity() - 20) * 10 / 8;
    if(capacity < 0) {
        capacity = 0;
    }
    return capacity;
}

int SpvAdapterConnected()
{
    return is_charging() != BATTERY_DISCHARGING;
}

int SpvPowerByGsensor()
{
    return check_wakeup_state() == 1;
}

int SpvIsCharging()
{
    return is_charging() == BATTERY_CHARGING;
}

int SpvGetSdcardStatus()
{
    int status = 0;
#ifdef GUI_SMALL_SYSTEM_ENABLE
    status = gui_sdcard_status;
#else  
    SD_Status sd_status;
    Inft_sdcard_getStatus(&sd_status);
    status = sd_status.sdStatus;   
#endif 
    INFO("status: %d\n", status);
    return status;
}

int SpvFormatSdcard()
{
    SpvTellSysGoingToUnmount(CMD_UNMOUNT_BEGIN);
#ifndef GUI_SMALL_SYSTEM_ENABLE
    char cmd[256] = {0};
#define TF_IMAGE_PATH  "/dev/mmcblk0"
    if(access(TF_IMAGE_PATH, F_OK) == 0) {
        sprintf(cmd, "umount /mnt/mmc; dd if=/dev/zero of=%s bs=1M count=18;sync;busybox fdisk %s << EOF\no\nn\np\n1\n\n\nt\nb\nw\nEOF;busybox mkfs.vfat /dev/mmcblk0p1", TF_IMAGE_PATH, TF_IMAGE_PATH);
        system(cmd);
        INFO("clear partitions, %s\n", cmd);
    }
#endif
    int ret = sd_format();
    SpvTellSysGoingToUnmount(CMD_UNMOUNT_END);
    INFO("Format Sdcard, ret: %d\n", ret);
    return ret;
}

void SpvSetParkingGuard()
{
    char value[128] = {0};
    GetConfigValue(GETKEY(ID_SETUP_PARKING_GUARD), value);
    INFO("SpvSetParkingGuard, enable: %d\n", strcasecmp(value, GETVALUE(STRING_OFF)));
    enable_gsensor(strcasecmp(value, GETVALUE(STRING_OFF)) ? 1 : 0);
}

//Get available space, in KByte
unsigned long long GetAvailableSpace()
{
    struct statfs dirInfo;
    unsigned long long availablesize = 0;
    if(statfs(EXTERNAL_STORAGE_PATH, &dirInfo)) {
        ERROR("statfs failed, path: %s\n", EXTERNAL_STORAGE_PATH);
        return 0;
    }

    unsigned long long blocksize = dirInfo.f_bsize;
    //unsigned long long totalsize = dirInfo.f_blocks * blocksize;
    //INFO("total: %lld, blocksize: %lld\n", totalsize >>20, blocksize);
    availablesize = dirInfo.f_bavail * blocksize >> 10;
    INFO("available space: %lluMB\n", availablesize>>10);
    return availablesize;
}

//Get video time remain, in Seconds
int GetVideoTimeRemain(long availableSpace, char *resolution)
{
    float mps = MPS_1080FHD;
    if(!strcasecmp(GETVALUE(STRING_1080FHD), resolution)) {
        mps = MPS_1080FHD;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_720P_60FPS))) {
        mps = MPS_720P_60FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_720P_30FPS))) {
        mps = MPS_720P_30FPS;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_WVGA))) {
        mps = MPS_WVGA;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_VGA))) {
        mps = MPS_VGA;
    }
    return (int)((double)(availableSpace >> 10) / mps);
}


//Get picture number remain, in Piece
int GetPictureRemain(long availableSpace, char *resolution)
{
    float megapixel;
    if(!strcasecmp(resolution, GETVALUE(STRING_12M))) {
        megapixel = MPP_12M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_10M))) {
        megapixel = MPP_10M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_8M))) {
        megapixel = MPP_8M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_5M))) {
        megapixel = MPP_5M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_3M))) {
        megapixel = MPP_3M;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_2MHD))) {
        megapixel = MPP_2MHD;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_VGA))) {
        megapixel = MPP_VGA;
    } else if(!strcasecmp(resolution, GETVALUE(STRING_1P3M))) {
        megapixel = MPP_1P3M;
    }
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
    sprintf(ts, "%02d:%02d:%02d", h, m, s);
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

    char *pNameEnd = fileName + strlen(fileName) - 1;
    while(pNameEnd > fileName) {
        if(*pNameEnd == '.')
            break;

        pNameEnd --;
    }

    if(pNameEnd == fileName)// '.' not found
        return MEDIA_UNKNOWN;

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
			INFO("file name is %s\n", fileName);
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



int IsMediaFolder(char *folderName)
{
    if(folderName == NULL)
        return 0;

    if((!strcasecmp(folderName,VIDEO_DIR))||(!strcasecmp(folderName,LOCK_DIR))||(!strcasecmp(folderName,PHOTO_DIR)))
	{
	    return 1;
	}
        

    return 0;
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
            INFO("folder: %s\n", ent->d_name);

            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) 
                continue; 
            if(IsMediaFolder(ent->d_name)) {
                sprintf(childpath,"%s/%s",dirPath,ent->d_name); 
                INFO("path:%s\n",childpath); 
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

    FileNode *pCur;
    while(pNode) {
        pCur = pNode;
        pNode = pNode->next;
        free(pCur);
    }

    fileList->fileCount = 0;
}

/**
 * mkdir for external media directory.
 * 0 succeed, other failed
 **/
int MakeExternalDir()
{
    if(access(EXTERNAL_MEDIA_DIR, F_OK)) {
        INFO("%s not exist, create it!\n", EXTERNAL_MEDIA_DIR);
        return mkdir(EXTERNAL_MEDIA_DIR, 0755) < 0;
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
