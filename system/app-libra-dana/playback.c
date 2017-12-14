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

#include "playback.h"
#include "alarm_db_common.h"

#define isdigit(a) ((unsigned char)((a) - '0') <= 9)

#ifdef SPV_DEBUG
#define INFO(format,...) if(1) printf("[P2P-PLAYBACK]: "format"", ##__VA_ARGS__)
#else
#define INFO(format,...)
#endif

#define ERROR(format,...) printf("[P2P-PLAYBACK]("__FILE__"#%s): "format"", __func__, ##__VA_ARGS__)

#define EXTERNAL_STORAGE_PATH "/mnt/mmc/"
#define EXTERNAL_MEDIA_DIR "/mnt/mmc/DCIM/"

extern LocalFileList g_media_files[MEDIA_COUNT] = {
    {MEDIA_VIDEO, 0, NULL},
    {MEDIA_VIDEO_LOCKED, 0, NULL},
    {MEDIA_PHOTO, 0, NULL},
};

extern int av_call_AviDemuxInit(const char* filePathName, 
            int *pKeyFrameCount, int *pWidth, int *pHeight);
extern int av_call_aviDemuxGetDurationMs(const long handle);
extern void av_call_AviDemuxDeinit(const long handle);

static int getFileDurationByName(char* file)
{
   int ret =0;
   if(file==NULL)
   {
      return ret;
   }
   char* videoName=NULL;
   if(strstr(file,"rear"))
   {
      videoName = strrchr(file, '.') -9; 
   }else
   {
      videoName = strrchr(file, '.') -4;
   }
   if(videoName!=NULL)
	  {
	    sscanf(videoName,"%4d",&ret);
	  }
   return ret;
}


static int getFileDuration(char* file)
{
    //if (g_fileDuration) return g_fileDuration;
    
    int handle = 0;
    int duration = 0;
    
    // fetch avi info
    int keyFrames;
    unsigned int aviWidth, aviHeight;
    handle = av_call_AviDemuxInit(file, &keyFrames, &aviWidth, &aviHeight);
    if (0 != handle) {
        duration = av_call_aviDemuxGetDurationMs(handle)/1000;        
    }

    printf("---%s duration %d sec, w:%d, h:%d\n", file, duration, aviWidth, aviHeight);

    // if demux handler has created once, delete and recreate again since avi file may changed
    if (handle != 0) {
        av_call_AviDemuxDeinit(handle);
        handle = 0;
    }

    return duration;
}



//Get available space, in KByte
static unsigned long long GetAvailableSpace()
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

//Convert the time in seconds to string format HH:MM:SS
static void TimeToString(int time, char *ts)
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

static void TimeToStringNoHour(int time, char *ts)
{
    int m = 0;
    int s = 0;

    if(ts == NULL)
        return;

    s = time % 60;
    m = time / 60;
    sprintf(ts, "%02d:%02d", m, s);
}

static int listDir(char *path) 
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

static int GetPictureCount()
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

static int GetMediaCount(int *pVideoCount, int *pPictureCount)
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
static int CompareFileNode(PFileNode node1, PFileNode node2)
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

static unsigned int convert2Timestamp(
            unsigned int year, 
            unsigned int month, 
            unsigned int day, 
            unsigned int hour, 
            unsigned int min, 
            unsigned int sec)
{
    struct tm tm;
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;

    time_t timet = mktime(&tm);
    return (unsigned int)timet;
}


/**
 * add file to sorted list
 **/
static int AddFile(LocalFileList *list, char *fileName, char *folderName,MEDIA_TYPE fileType)
{
    if(list == NULL || fileName == NULL || strlen(fileName) <= 0 
            || folderName == NULL || strlen(folderName) <= 0 || strlen(folderName) >= 16) {
        ERROR("error file name or folder name, filename: %s, foldername:%s\n", fileName, folderName);
        return -1;
    }

    char sbuf[256];
    snprintf(sbuf, 256-1, "%s/%s", "/mnt/mmc/DCIM/100CVR", fileName);
    int fileLen = 0;
	if(fileType==MEDIA_PHOTO)
	{
	   fileLen = 0;
	}
	else
	{
       fileLen = getFileDurationByName(sbuf); 
	   if (fileLen <= 0) return -1;
	}   
   

    PFileNode pNode = (PFileNode) malloc(sizeof(FileNode));
    if(pNode == NULL) {
        ERROR("malloc file node error, add file %s to list failed!\n", fileName);
        return -1;
    }
    memset(pNode, 0, sizeof(FileNode));  

    pNode->len = fileLen;
   // printf("***Mason: foldName: %s, fileName: %s, len:%d\n", folderName, fileName, pNode->len);

    strcpy(pNode->fileName, fileName);
    strcpy(pNode->folderName, folderName);
    pNode->prev = NULL;
    pNode->next = NULL; 
	char *pbuf=fileName;
    if(strstr(fileName,"LOCK_"))
	{
	  pbuf=fileName+5;
	}
    unsigned int y,mo,d,h,m,s;
    //"2007_0112_104214_0002.mp4";
    sscanf(pbuf,"%4d_%2d%2d_%2d%2d%2d", 
           &y, &mo, &d, &h, &m, &s);
    pNode->ts = convert2Timestamp(y,mo,d, h,m,s);
 //   printf("**Mason: %s, %d.%d.%d|%d:%d:%d,ts:%u\n",
    //      pbuf, y,mo,d,h,m,s,pNode->ts);

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

static int SubstringIsDigital(char *string, int start, int end)
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
    if(strlen(newName) < 21 || 
            (*(newName + 4) != '_' && *(newName + 9) != '_' && *(newName + 16) != '_' && SubstringIsDigital(newName, 1, 4) && 
             SubstringIsDigital(newName, 6, 9) && SubstringIsDigital(newName, 11, 16) && SubstringIsDigital(newName, 18, 21)))
        return MEDIA_UNKNOWN;

    if(!strcasecmp("jpg", suffix)) {
        return MEDIA_PHOTO;
    } else if(!strcasecmp("mp4", suffix)) {
        //if(*(name + 21) == 's' || *(name + 21) == 'S') {
        if(strstr(name, "rear")) {
            return MEDIA_VIDEO_REAR;
        } else if(!strncasecmp(name, "LOCK_", 5)) {
            return MEDIA_VIDEO_LOCKED;
        } else{
            return MEDIA_VIDEO;
        }
    } 
    return MEDIA_UNKNOWN;
}

static int ScanMediaFiles(LocalFileList *fileList, char *folderPath, char *folderName)
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
                AddFile(&fileList[fileType], ent->d_name, folderName,fileType);
            }

        }
    }
    closedir(pDir);
}

int IsMediaFolder(char *folderName)
{
    if(folderName == NULL || strlen(folderName) != 6)
        return 0;

    if(SubstringIsDigital(folderName, 1, 3) && !strncasecmp(folderName + 3, "CVR", 3))
        return 1;

    return 0;
}

static int ScanFolders(LocalFileList *fileList, char *dirPath)
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

static void FreeLocalFileList(LocalFileList *fileList)
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
static int MakeExternalDir()
{
    if(access(EXTERNAL_MEDIA_DIR, F_OK)) {
        INFO("%s not exist, create it!\n", EXTERNAL_MEDIA_DIR);
        return mkdir(EXTERNAL_MEDIA_DIR, 0755) < 0;
    }
    return 0;
}

static int copy_file(const char* src, const char* dst, int use_src_attr)
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

static void InitMediaFiles()
{
    int i;
    for(i = 0; i < MEDIA_COUNT; i ++) {
        FreeLocalFileList(&g_media_files[i]);
    }
    memset(g_media_files, 0, sizeof(LocalFileList) * MEDIA_COUNT);
    for(i = 0; i < MEDIA_COUNT; i ++) {
        g_media_files[i].fileType = (MEDIA_TYPE) i;
    }
}

static void GetMediaFileCount(int *videoCount, int *photoCount)
{
    int v = 0, p = 0, i = 0;
    for(i = 0; i < MEDIA_COUNT; i ++) {
        switch(g_media_files[i].fileType) {
            case MEDIA_VIDEO:
			case MEDIA_VIDEO_REAR:
            case MEDIA_VIDEO_LOCKED:
                v += g_media_files[i].fileCount;
                break;
            case MEDIA_PHOTO:
                p += g_media_files[i].fileCount;
                break;
        }
    }

    *videoCount = v;
    *photoCount = p;
}

extern int getFileTotal(MEDIA_TYPE fileType)
{

    return g_media_files[fileType].fileCount;
}

extern int AcountMediaFiles()
{
    int vc, pc, i = 0;
    InitMediaFiles();       
    if (0 > ScanFolders(g_media_files, 
                        EXTERNAL_MEDIA_DIR))
        return 0;
    GetMediaFileCount(&vc, &pc);
    INFO("scan files end: video count: %d, picture count: %d\n", vc, pc);

    return vc+pc;
}

extern int getAlarms(T_AlarmDBRecord* alarms,MEDIA_TYPE fileType)
{
   int index = 0;
   PFileNode pItem = g_media_files[fileType].pHead;
   int j = 0;
   for (j = 0; j<g_media_files[fileType].fileCount; j++) {
      alarms[index].occurTime = alarms[index].fileKey = pItem->ts;
      alarms[index].deleted = pItem->len;
      alarms[index].eventType = 0;
      index++;
      
      pItem=pItem->next;
    }
   
}

extern int findPlaybackFile(char* request, char* fileName,MEDIA_TYPE fileType)
{
   int i = 0;
   
   if (!request || !fileName) {
       INFO("***Mason: ERROR: request: %x, fileName:%x\n", 
            request, fileName);
       return 0;
   }


       PFileNode pItem = g_media_files[fileType].pHead;
       int j = 0;
       for (j = 0; j<g_media_files[fileType].fileCount; j++) {
          if (NULL == pItem) return 0;
          
          if (strstr(pItem->fileName, request)){
              strcpy(fileName, pItem->fileName);
              return 1;
          }
          pItem=pItem->next;
        }
   

   return 0;
}

