#ifndef __SPV_FILE_MANAGER_H__
#define __SPV_FILE_MANAGER_H__

#include "spv_common.h"
#include "file_dir.h"

#define VIDEO_LOCK_PREFIX "LOCK_"

#define MAX_LOCK_VIDEO_COUNT 5
#define MAX_PHOTO_COUNT 1000

typedef enum {
    FM_SCAN_FILE_LIST = 0x0600,
    FM_LOCK_FILE,
    FM_ADD_FILE,
    FM_DELETE_FILE,
    FM_DELETE_TYPE,
    FM_DELETE_ALL,
    FM_CHECK_FREE_SPACE,
} FM_MSG;

typedef enum {
    MEDIA_UNKNOWN = -1,
    MEDIA_VIDEO,
    MEDIA_VIDEO_LOCKED,
    MEDIA_PHOTO,
    MEDIA_COUNT,
    /*MEDIA_TIME_LAPSE,
    MEDIA_SLOW_MOTION,
    MEDIA_CAR,
    MEDIA_NIGHT,
    MEDIA_CONTINUOUS,*/
}MEDIA_TYPE;

typedef struct filenode{
    char fileName[128];
    char folderName[32];
    struct filenode* prev;
    struct filenode* next;
} FileNode;

typedef FileNode* PFileNode;

typedef struct {
    MEDIA_TYPE fileType;
    int fileCount;
    PFileNode pHead;
    PFileNode pTail;
} FileList; 

int SpvScanMediaFiles();
int SpvDeleteSingleFile(char *filename);
int SpvDeleteTypeFile(MEDIA_TYPE type);
int SpvDeleteALLFile();
int SpvAddFile(char *filename);
int SpvCheckFreeSpace();
void GetMediaFileCount(int *videoCount, int *photoCount);

HMSG SpvGetFMHandler();
int SpvInitFileManager();
#endif
