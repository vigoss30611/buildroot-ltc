#ifndef __SPV_FILE_H__
#define __SPV_FILE_H__

typedef enum {
    MEDIA_UNKNOWN = -1,
    MEDIA_VIDEO,
    MEDIA_VIDEO_LOCKED,
    MEDIA_PHOTO,
    MEDIA_COUNT,
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
} FileList; 


#endif
