#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/prctl.h>

#include "spv_common.h"
#include "spv_utils.h"
#include "spv_debug.h"
#include "spv_message.h"
#include "spv_file_manager.h"

static HMSG g_fm_handler = 0;

FileList g_media_files[MEDIA_COUNT] = {
    {MEDIA_VIDEO, 0, NULL},
    {MEDIA_VIDEO_LOCKED, 0, NULL},
    {MEDIA_PHOTO, 0, NULL},
};
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

static void InitMediaFiles()
{
    int i;
    for(i = 0; i < MEDIA_COUNT; i ++) {
        FreeFileList(&g_media_files[i]);
    }
    memset(g_media_files, 0, sizeof(FileList) * MEDIA_COUNT);
    for(i = 0; i < MEDIA_COUNT; i ++) {
        g_media_files[i].fileType = (MEDIA_TYPE) i;
    }
}


int SpvScanMediaFiles()
{
    return SpvPostMessage(g_fm_handler, FM_SCAN_FILE_LIST, 0, 0);
}

int SpvDeleteSingleFile(char *filename)
{
    return SpvSendMessage(g_fm_handler, FM_DELETE_FILE, (int)filename, 0);
}

int SpvDeleteTypeFile(MEDIA_TYPE type)
{
    return SpvSendMessage(g_fm_handler, FM_DELETE_TYPE, type, 0);
}

int SpvDeleteALLFile()
{
    return SpvSendMessage(g_fm_handler, FM_DELETE_ALL, 0, 0);
}

int SpvAddFile(char *filename)
{
    return SpvSendMessage(g_fm_handler, FM_ADD_FILE, (int)filename, 0);
}

int SpvCheckFreeSpace()
{
    return SpvPostMessage(g_fm_handler, FM_CHECK_FREE_SPACE, 0, 0);
}

void GetMediaFileCount(int *videoCount, int *photoCount)
{
    int v = 0, p = 0, i = 0;
    for(i = 0; i < MEDIA_COUNT; i ++) {
        switch(g_media_files[i].fileType) {
            case MEDIA_VIDEO:
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

/**
 * path name regular: EXTERNAL_MEDIA_DIR + foldername + filename
 **/
static int GetFileInfo(char *pathname, char *foldername, char *filename)
{
    if(pathname == NULL) {
        return -1;
    }

    if(!strstr(pathname, EXTERNAL_MEDIA_DIR)) {
        ERROR("file is not in the media dir, discard it, file: %s, media dir: %s\n", pathname, EXTERNAL_MEDIA_DIR);
        return -1;
    }

    int folder_index = (strstr(pathname, EXTERNAL_MEDIA_DIR) - pathname) + strlen(EXTERNAL_MEDIA_DIR);
    int name_index = (strrchr(pathname, '/') - pathname) + 1;
    int folder_length = name_index - folder_index - 1;
    if(folder_length <= 0) {
        ERROR("folder not found, folder file: %s\n", pathname+folder_index);
        return -1;
    }

    strncpy(foldername, pathname + folder_index, folder_length);
    if(strstr(foldername, "/") || !IsMediaFolder(foldername)) {
        ERROR("folder not matched, folder: %s, fi: %d, ni: %d, fl: %d, name: %s\n", foldername, folder_index, name_index, folder_length, strrchr(pathname, '/'));
        return -1;
    }

    strcpy(filename, pathname + name_index);

    return 0;
}

static int ControlMediaCount(MEDIA_TYPE type)
{
    int maxcount = 0;
    int ret = 0;
    switch(type) {
        case MEDIA_VIDEO_LOCKED:
            maxcount = MAX_LOCK_VIDEO_COUNT;
            break;
        case MEDIA_PHOTO:
            maxcount = MAX_PHOTO_COUNT;
            break;
        default:
            break;
    }
    if(maxcount <= 0)
        return 0;

    FileList *pList = &g_media_files[type];
    while(pList->fileCount > maxcount) {
        PFileNode pNode = pList->pTail;
        RemoveNodeFromList(pList, pNode);
        ret = DeleteFileByNode(pNode);
        free(pNode);
		 pNode=NULL;
        if(ret && ret != ENOENT) {
            ERROR("delete file failed\n");
            ret = -2;
            break;
        }
    }
    return ret;
}

int AddFileToListHead(char *pathname)
{
    char filename[128] = {0};
    char foldername[128] = {0};
    int ret = GetFileInfo(pathname, foldername, filename);
    if(ret) {
        return -1;
    }

    MEDIA_TYPE type = GetFileType(filename);
    if(type == MEDIA_UNKNOWN) {
        ERROR("media type error, filename: %s\n", filename);
        return -1;
    }

    PFileNode pNode = (PFileNode) malloc(sizeof(FileNode));
    if(pNode == NULL) {
        ERROR("malloc file node error, add file %s to list failed!\n", filename);
        return -1;
    }
    memset(pNode, 0, sizeof(FileNode));

    strcpy(pNode->fileName, filename);
    strcpy(pNode->folderName, foldername);
    if(g_media_files[type].fileCount <= 0) {//empty queue
        g_media_files[type].pHead = pNode;
        g_media_files[type].pTail = pNode;
        g_media_files[type].fileCount = 1;
    } else {//add to head
        pNode->next = g_media_files[type].pHead;
        g_media_files[type].pHead->prev = pNode;
        g_media_files[type].pHead = pNode;
        g_media_files[type].fileCount++;
    }

    ControlMediaCount(type);

    return 0;
}

PFileNode FindNodeFromList(FileList *list, char *filename, char *foldername)
{
    PFileNode pCurrentNode = list->pHead;
    while(pCurrentNode) {
        if(!strcasecmp(pCurrentNode->fileName, filename)) {
            return pCurrentNode;
        }
        pCurrentNode = pCurrentNode->next;
    }
    return NULL;
}

int DeleteFileByNode(PFileNode pNode)
{
    int ret = 0;
    if(pNode == NULL)
        return -1;

    char pathname[256] = {0};
    MEDIA_TYPE filetype = GetFileType(pNode->fileName);
    //delete thumbnail
    if(filetype == MEDIA_VIDEO || filetype == MEDIA_VIDEO_LOCKED) {
        char name[128] = {0};
        strncpy(name, pNode->fileName, strchr(pNode->fileName, '.') - pNode->fileName);
        sprintf(pathname, EXTERNAL_MEDIA_DIR"%s/.%s.jpg", pNode->folderName, name);
        ret = SpvUnlinkFile(pathname);
    }

    memset(pathname, 0, 256);
    sprintf(pathname, EXTERNAL_MEDIA_DIR"%s/%s", pNode->folderName, pNode->fileName);
    ret = SpvUnlinkFile(pathname);

    INFO("delete file, %s\n", pathname);
    return ret;
}

int RemoveNodeFromList(FileList *list, PFileNode pNode)
{
    int ret = 0;
    if(list == NULL || list->fileCount <= 0 || pNode == NULL) {
        return -1;
    }
    if(list->pHead == pNode) {
        list->pHead = pNode->next;
    }
    if(list->pTail == pNode) {
        list->pTail = pNode->prev;
    }
    PFileNode pPrev = pNode->prev;
    PFileNode pNext = pNode->next;
    if(pPrev) {
        pPrev->next = pNext;
    }
    if(pNext) {
        pNext->prev = pPrev;
    }
    list->fileCount--;

    return ret;
}

int CheckAndDeleteFiles()
{
    int ret = 0;
    if(!IsSdcardMounted()) {
        return 0;
    }

    unsigned int freeSpace = SpvGetAvailableSpace();//kB
    SpvUpdateSDcardSpace(freeSpace);

    if(!IsLoopingRecord()) {
        SpvSendMessage(GetLauncherHandler(), MSG_CAMERA_CALLBACK, CAMERA_ERROR_SD_FULL, IsSdcardFull());
        return 0;
    }

    while(freeSpace < (SDCARD_FREE_SPACE_MIN_VALUE << 10)) {
        INFO("check free space, freeSpace: %ukB\n", freeSpace);
        PFileNode pNode = g_media_files[MEDIA_VIDEO].pTail;
        if(pNode == NULL) {
            ERROR("no files in video dir, videocount: %d\n", g_media_files[MEDIA_VIDEO].fileCount);
            ret = -1;
            break;
        }

        RemoveNodeFromList(&g_media_files[MEDIA_VIDEO], pNode);
        ret = DeleteFileByNode(pNode);
        free(pNode);
        pNode=NULL;
        if(ret && ret != ENOENT) {
            ERROR("delete file failed\n");
            ret = -2;
            break;
        }

        freeSpace = SpvGetAvailableSpace();
    }

    SpvSendMessage(GetLauncherHandler(), MSG_CAMERA_CALLBACK, CAMERA_ERROR_SD_FULL, IsSdcardFull());

    return ret;
}

static int FileManagerProc(HMSG handler, int msgId, int wParam, long lParam)
{
    int ret = 0;
    if(handler == 0) {
        ERROR("handler == 0\n");
        return -1;
    }
    pthread_mutex_lock(&fileMutex);
    switch(msgId) {
        case FM_SCAN_FILE_LIST:
            {
                int vc, pc, i = 0;
                INFO("scan start, sdInserted: %d\n", IsSdcardMounted());
                InitMediaFiles();
                if(IsSdcardMounted()) {
                    ScanFolders(g_media_files, EXTERNAL_MEDIA_DIR);
                }
                GetMediaFileCount(&vc, &pc);
                INFO("scan files end: video count: %d, picture count: %d\n", vc, pc);
                SpvPostMessage(g_fm_handler, FM_CHECK_FREE_SPACE, 0, 0);
                break;
            }
        case FM_ADD_FILE:
            {
                char pathname[256] = {0};
                if(realpath((char *)wParam, pathname) == NULL) {
                    ERROR("invalid path name, wParam: %s\n", (char *)wParam);
                    ret = -1;
                    break;
                }
                ret = CheckAndDeleteFiles();
                ret = AddFileToListHead(pathname);
                break;
            }
        case FM_DELETE_FILE:
            {
                if(wParam == 0) {
                    ret = -1;
                    break;
                }
                char pathname[256] = {0};
                if(realpath((char *)wParam, pathname) == NULL) {
                    ERROR("invalid file path name, wParam: %s\n", (char *)wParam);
                    ret = -1;
                    break;
                }
                char foldername[128] = {0};
                char filename[128] = {0};
                ret = GetFileInfo(pathname, foldername, filename);
                if(ret) {
                    ERROR("invalid file path name, %s\n", (char *)wParam);
                    break;
                }

                MEDIA_TYPE type = GetFileType(filename);
                if(type == MEDIA_UNKNOWN) {
                    ERROR("meida type is unknown, %s\n", filename);
                    break;
                }

                PFileNode pNode = FindNodeFromList(&g_media_files[type], filename, foldername);
                if(pNode) {
                    ret = RemoveNodeFromList(&g_media_files[type], pNode);
                    ret = DeleteFileByNode(pNode);
                    free(pNode);
					  pNode=NULL;
                } else {
                    INFO("file not found in list, pathname: %s, foldername: %s, filename: %s\n", pathname, foldername, filename);
                }
                break;
            }
        case FM_DELETE_TYPE:
            {
                if(wParam > MEDIA_UNKNOWN && wParam < MEDIA_COUNT) {
                    FreeFileList(&g_media_files[wParam]);

                    char cmd[128] = {0};
                    char *foldername = NULL;
                    switch(wParam) {
                        case MEDIA_VIDEO:
                            foldername = VIDEO_DIR;
                            break;
                        case MEDIA_PHOTO:
                            foldername = PHOTO_DIR;
                            break;
                        case MEDIA_VIDEO_LOCKED:
                            foldername = LOCK_DIR;
                            break;
                        default:
                            foldername = VIDEO_DIR;
                            break;
                    }
                    sprintf(cmd, "rm -rf "EXTERNAL_MEDIA_DIR"%s", foldername);
                    ret = SpvRunSystemCmd(cmd);
                } else {
                    ret = 0;
                }
                break;
            }
        case FM_DELETE_ALL:
            {
                InitMediaFiles();
                char cmd[128] = {0};
                sprintf(cmd, "rm -rf "EXTERNAL_MEDIA_DIR"*");
                ret = SpvRunSystemCmd(cmd);
                break;
            }
        case FM_CHECK_FREE_SPACE:
            ret = CheckAndDeleteFiles();
            break;
        default:
            break;
    }
    pthread_mutex_unlock(&fileMutex);
    SpvPostMessage(GetLauncherHandler(), MSG_MEDIA_UPDATED, 0, 0);
    return ret;
}

HMSG SpvGetFMHandler()
{
    return g_fm_handler;
}

void FileDeleteServer()
{
    prctl(PR_SET_NAME, __func__);
    while(1) {
        SpvPostMessage(g_fm_handler, FM_CHECK_FREE_SPACE, 0, 0);
        sleep(60);
    }
}

int SpvInitFileManager()
{
    g_fm_handler = SpvRegisterHandler(FileManagerProc);
    if(g_fm_handler == 0) {
        ERROR("register file manager handler failed\n");
        return -1;
    }

    SpvCreateServerThread((void *)SpvMessageLoop, (void *)g_fm_handler);
    SpvScanMediaFiles();
    SpvCreateServerThread((void *)FileDeleteServer, NULL);
    //SpvPostMessage(g_fm_handler, FM_CHECK_FREE_SPACE, 0, 0);
    return 0;
}
