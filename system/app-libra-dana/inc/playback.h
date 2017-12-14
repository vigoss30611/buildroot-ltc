#ifndef __PLAYBACK_H__
#define __PLAYBACK_H__

typedef enum {
    MEDIA_UNKNOWN = -1,
    MEDIA_VIDEO,
    MEDIA_VIDEO_REAR,
    MEDIA_VIDEO_LOCKED,
    MEDIA_PHOTO,
    MEDIA_COUNT,
}MEDIA_TYPE;

typedef struct filenode{
    char fileName[128];
    char folderName[32];
    unsigned int ts;
    unsigned int len;
    struct filenode* prev;
    struct filenode* next;
} FileNode;

typedef FileNode* PFileNode;

typedef struct {
    MEDIA_TYPE fileType;
    int fileCount;
    PFileNode pHead;
} LocalFileList; 

typedef enum {
    SD_STATUS_UMOUNT,
    SD_STATUS_MOUNTED,
    SD_STATUS_ERROR,
    SD_STATUS_READ_ONLY,
} SD_STATUS;

typedef enum {
    CMD_UNMOUNT_END,
    CMD_UNMOUNT_BEGIN,
} SYS_COMMAND;

MEDIA_TYPE GetFileType(char *fileName);
int IsMediaFolder(char *folderName);

#if 0
unsigned long long GetAvailableSpace();

void TimeToString(int time, char *ts);

void TimeToStringNoHour(int time, char *ts);

int GetPictureCount();

int SubstringIsDigital(char *string, int start, int end);



int ScanFolders(LocalFileList *fileList, char *dirPath);

void FreeLocalFileList(LocalFileList *fileList);

int GetMediaCount(int *pVideoCount, int *pPictureCount);

int CompareFileNode(PFileNode node1, PFileNode node2);

int MakeExternalDir();

int copy_file(const char* src, const char* dst, int use_src_attr);
#endif
#endif //__PLAYBACK_H__
