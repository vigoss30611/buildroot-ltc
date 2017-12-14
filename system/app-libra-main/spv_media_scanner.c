#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

#include<minigui/common.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include"spv_common.h"
#include"spv_utils.h"
#include"spv_debug.h"

FileList g_media_files[MEDIA_COUNT] = {
    {MEDIA_VIDEO, 0, NULL},
    {MEDIA_VIDEO_LOCKED, 0, NULL},
    {MEDIA_PHOTO, 0, NULL},
};

pthread_mutex_t scanMutex = PTHREAD_MUTEX_INITIALIZER;

static void CopyList(FileList *dest, FileList *src)
{
    if(src == NULL || dest == NULL)
        return;

    FreeFileList(dest);
    dest->fileType = src->fileType;

    if(src->fileCount <= 0) {
        return;
    }

    PFileNode pSrcNode = src->pHead;
    PFileNode pDestPrev = NULL;
    while(pSrcNode) {
        PFileNode pTmpNode = (PFileNode) malloc(sizeof(FileNode));
        if(pTmpNode == NULL) {
            ERROR("malloc node failed\n");
            pSrcNode = pSrcNode->next;
            continue;
        }
        memset(pTmpNode, 0, sizeof(FileNode));
        memcpy(pTmpNode, pSrcNode, sizeof(FileNode));
        if(pDestPrev != NULL) {
            pDestPrev->next = pTmpNode;
        } 
        pTmpNode->prev = pDestPrev;
        pTmpNode->next = NULL;
        if(pSrcNode == src->pHead || pDestPrev == NULL) {
            dest->pHead = pTmpNode;
        }

        pDestPrev = pTmpNode;
        dest->fileCount ++;

        pSrcNode = pSrcNode->next;
    }

    if(src->fileCount != dest->fileCount) {
        ERROR("fileCount not equals, what's wrong, src.Count: %d, dest.Count: %d\n", src->fileCount, dest->fileCount);
        //here is unsafeable
        dest->fileCount = src->fileCount;
    }
}

void SyncMediaList(FileList *list)
{
    int i = 0;
    for(i = 0; i < MEDIA_COUNT; i ++) {
        CopyList(&list[i], &g_media_files[i]);
    }
}


/**
 * Sync the dest list with the src list,
 * Make the dest list's content to be the same with src list, but not in the same addr
 **/
static void SyncList(FileList *srcList, FileList *destList)
{
    if(srcList == NULL || destList == NULL) {
        ERROR("list == NULL, return\n");
        return;
    }
    INFO("srcList %x, count: %d, destList %x, count: %d\n", (unsigned int)srcList, srcList->fileCount, (unsigned int)destList, destList->fileCount);
    if(srcList->fileCount <= 0) {
        FreeFileList(destList);
        return;
    }

    PFileNode pSrcNode = srcList->pHead;
    PFileNode pDestNode = destList->pHead;
    PFileNode pDestPrev = NULL;

    while(pSrcNode || pDestNode) {
        int compareRet = CompareFileNode(pSrcNode, pDestNode);
        if(compareRet > 0) {//need to add to the destlist
            PFileNode pTmpNode = (PFileNode) malloc(sizeof(FileNode));
            memset(pTmpNode, 0, sizeof(FileNode));
            memcpy(pTmpNode, pSrcNode, sizeof(FileNode));
            INFO("node need to add, file: %s\n", pTmpNode->fileName);
            if(pDestPrev != NULL) {
                pDestPrev->next = pTmpNode;
            } 
            pTmpNode->prev = pDestPrev;
            if(pDestNode != NULL) {
                pDestNode->prev = pTmpNode;
            }
            pTmpNode->next = pDestNode;
            destList->fileCount ++;
            pDestPrev = pTmpNode;
            if(pSrcNode == srcList->pHead) {
                destList->pHead = pTmpNode;
            }
            pSrcNode = pSrcNode->next;
        } else if(compareRet == 0) { //the same continue
            if(pSrcNode) {
                pSrcNode = pSrcNode->next;
            }
            if(pDestNode) {
                pDestNode = pDestNode->next;
            }
        } else {//node not exist in srclist, del it from the destlist
            INFO("node need to del, file: %s\n", pDestNode->fileName);
            PFileNode pTmpNode = pDestNode->next;
            pDestPrev = pDestNode->prev;
            if(pDestNode == destList->pHead) {
                destList->pHead = pDestNode->next;
            }
            if(pDestPrev != NULL) {
                pDestPrev->next = pTmpNode;
            }
            if(pTmpNode) {
                pTmpNode->prev = pDestPrev;
            }
            free(pDestNode);
            destList->fileCount --;
            pDestNode = pTmpNode;
        }
    }

    if(srcList->fileCount != destList->fileCount) {
        ERROR("fileCount not equals, what's wrong, srcList.Count: %d, destList.Count: %d\n", srcList->fileCount, destList->fileCount);
        //here is unsafeable
        destList->fileCount = srcList->fileCount;
    }
}

static void SyncMediaFiles(FileList* tmp_media_files)
{
    int i = 0;
    for(i = 0; i < MEDIA_COUNT; i ++) {
        SyncList(&tmp_media_files[i], &g_media_files[i]);
    }
    INFO("Sync file list end\n");
}

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

static void ScanFilesThread(BOOL sdInserted)
{
    int vc, pc, i = 0;
    pthread_mutex_lock(&scanMutex);
    INFO("scan start, sdInserted: %d\n", IsSdcardMounted());
    InitMediaFiles();
    if(IsSdcardMounted()) {
        /*FileList tmp_media_files[MEDIA_COUNT];
        memset(tmp_media_files, 0, sizeof(FileList) * MEDIA_COUNT);
        for(i = 0; i < MEDIA_COUNT; i ++) {
            tmp_media_files[i].fileType = (MEDIA_TYPE) i;
        }
        ScanFolders(tmp_media_files, EXTERNAL_MEDIA_DIR);
        SyncMediaFiles(tmp_media_files);
        for(i = 0; i < MEDIA_COUNT; i ++) {
            FreeFileList(&tmp_media_files[i]);
        }*/
        ScanFolders(g_media_files, EXTERNAL_MEDIA_DIR);
    }
    GetMediaFileCount(&vc, &pc);
    INFO("scan files end: video count: %d, picture count: %d\n", vc, pc);
    SendNotifyMessage(GetActiveWindow(), MSG_MEDIA_SCAN_FINISHED, 0, 0);
    pthread_mutex_unlock(&scanMutex);
}

void ScanSdcardMediaFiles(BOOL sdInserted)
{
    INFO("ScanSdcardMediaFiles, sdInserted: %d\n", sdInserted);
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    INFO("pthread_create, ScanSdcardMediaFiles\n");
    pthread_create (&t, &attr, (void *)ScanFilesThread, (void *)sdInserted);
    pthread_attr_destroy (&attr);
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
