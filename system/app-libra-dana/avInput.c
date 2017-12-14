#include <dlfcn.h>
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>

//#include "mediatyp.h"

#include <sys/time.h>
#include "avi_demux.h"

#define LOG_265_MP4_ERR "---ERR-H265_MP4: "
#define LOG_265_MP4_INFO "***INFO-H265_MP4: "

#ifdef LOGE_VID
#undef LOGE_VID
#endif
//#define LOGE_VID(...)// LOG_WCLASS(LOG_CLS_VIDEO, LOG_LVL_E, LOG_265_MP4_ERR, __VA_ARGS__)
#define LOGE_VID printf(LOG_265_MP4_ERR);printf

#ifdef LOGI_VID
#undef LOGI_VID
#endif
#define LOGI_VID(...) //LOG_WCLASS(LOG_CLS_VIDEO, LOG_LVL_E, LOG_265_MP4_INFO, __VA_ARGS__)
//#define LOGI_VID printf(LOG_265_MP4_INFO);printf

#define LOG_AV_SYNC(...)// printf("AV_SYNC: ");printf

#ifdef LOGV_VID
#undef LOGV_VID
#endif
#define LOGV_VID(...) //LOG_WCLASS(LOG_CLS_VIDEO, LOG_LVL_E, LOG_265_MP4_INFO, __VA_ARGS__)
//#define LOGV_VID printf(LOG_265_MP4_INFO);printf

extern int av_call_AviDemuxInit(const char* filePathName, 
     int *pKeyFrameCount, 
     int *pWidth, 
     int *pHeight);

extern int av_call_AviDemuxGetUsPerFrame(const long handle);

extern int av_call_aviDemuxGetDurationMs(const long handle);

extern int av_call_isH265(const int handle);

extern void av_call_AviDemuxDeinit(const long handle);

extern long long mp4_call_Av_DemuxGetFrameTimestamp(const int handle);

extern int av_call_Av_DemuxGetFrame(const int handle, int frmIdx,
       AVI_DATA_TYPE *pMediaType, unsigned char** pBufOut, int *pLen);

extern int av_call_AviDemuxGetNextFrame(const int handle,
       int *pMediaType, unsigned char** pBufOut, int *pLen);

extern long long mp4_call_Av_DemuxGetFrameTimestamp(const int handle);

typedef enum AV_VIDEO_TYPE{
    AV_VIDEO_NONE,
    AV_VIDEO_H264 = 1,
    AV_VIDEO_H265 = 2,
}AV_VIDEO_TYPE;


///////////////////// begin ///////////////////////
static char* pVideoBuf = NULL;
#define VIDEO_BUF_SIZE (1024*1024)
static int gMsecPerFrame = 0;

extern int initDemux(char* fileName, 
                     int* keyFrameCount,
                     int* mediaWidth,
                     int* mediaHeight,
                     int* secs)
{
    int msecPerFrame = 0;
    int fileDuration = 0;

    if (!mediaWidth || !mediaHeight || !keyFrameCount || !fileName || !secs)
        return 0;
    
    int demuxerHandle = av_call_AviDemuxInit(fileName, 
        keyFrameCount, mediaWidth, mediaHeight);
    if (0 != demuxerHandle) {
        msecPerFrame = av_call_AviDemuxGetUsPerFrame(demuxerHandle);
        printf("demux created: width %d, height %d, msecPerFrame %d ms, keyFrameCount %d\n", 
            *mediaWidth, *mediaHeight, 
            msecPerFrame, *keyFrameCount);

        fileDuration = av_call_aviDemuxGetDurationMs(demuxerHandle);

        if ((msecPerFrame==0) && *keyFrameCount) {
            msecPerFrame = fileDuration/(*keyFrameCount)/5/4;
        }

        *secs = fileDuration/1000;

        printf("avi input init ok.\n");

        gMsecPerFrame = msecPerFrame;
        
        return demuxerHandle;
    }

    return 0;
}

#define FILE_HEAD_LEN (100*1024)
static char* gFileHeadData = NULL;
static int gFileHeadLen = 0;

static int isValidVideFrame(int handle, 
                          AVI_DATA_TYPE mediaType,
                          unsigned char* aviData,
                          int aviLen)
{
    switch(mediaType) {
        case AVI_TYPE_AUDIO: { 
            // get next a/v frame, try this until we find video frame
            LOGV_VID("find AUDIO FRAME.\n");
            return 1;
        } break;

        case AVI_TYPE_VIDEO_I:
            //return 1;
            
        if (av_call_isH265(handle) && (aviLen>5)) {
            int i = 0;
            unsigned char tag = 0;
            int isFindHead = 0;
            for (i=0; i<aviLen; i++) {
                int isFindBegin = 0;                
                int hLen = 0;
                if ((aviData[i]==0) && (aviData[i+1]==0) &&
                    (((aviData[i+2]==0) && (aviData[i+3]==1)) ||
                    (aviData[i+2]==1))) {
                    if (aviData[i+2]==1) {
                        tag = aviData[i+3];
                    }else {
                        tag = aviData[i+4];
                    }

                    hLen = i;

                    switch (tag) {
                        case 0x40:
                            printf("h265 Find VPS: 0x%x, aviLen:%d\n", tag, aviLen);
                            isFindHead = 1;  
                            gFileHeadLen = 0;
                            break;
                        case 0x42:
                            printf("h265 Find SPS: 0x%x, aviLen:%d\n", tag, aviLen);
                            isFindHead = 1;
                            gFileHeadLen = 0;
                            break;
                        case 0x44:
                            printf("h265 Find PPS: 0x%x, aviLen:%d\n", tag, aviLen);
                            isFindHead = 1;
                            gFileHeadLen = 0;
                            break;
                        default:
                            printf("h265 I frame begin with: 0x%x, aviLen:%d\n", tag, aviLen);
                            isFindBegin = 1;
                            break;
                    }
                }

                if (isFindBegin) {
                    printf("****gFileHeadLen = %d, isFindHead: %d, headLen: %d\n", 
                        hLen, isFindHead, gFileHeadLen);
                    if (isFindHead && hLen) {
                         
                        if (gFileHeadData == NULL || FILE_HEAD_LEN < hLen) {
                            int size = 0;
                            if (NULL != gFileHeadData) free(gFileHeadData);
                            if(FILE_HEAD_LEN > hLen) size = FILE_HEAD_LEN;
                            else size = 2*hLen;

                            gFileHeadData = malloc(size);
                            assert(gFileHeadData);                            
                        }

                        memcpy(gFileHeadData, aviData, hLen);
                        gFileHeadLen = hLen;
                        isFindHead = 0;
                        return 1;
                    } else {
                        return 2;
                    }
                    
                } else if (isFindHead){ 
                    //isFindHead = 0;
                    continue;
                }
            }
        } 
        return 1;
        break;
        case AVI_TYPE_VIDEO_P:{
            LOGV_VID("find P frame: 0x%x 0x%x 0x%x 0x%x 0x%x.\n",
                aviData[0], aviData[1],aviData[2],aviData[3],aviData[4]);
            return 1;
        } break;
    }

    printf("===== INVALID FRAME.\n");
    return 0;
}

extern int deinitDemux(const int handle)
{    
    if (handle != 0) {
        av_call_AviDemuxDeinit(handle);
        return 1;
    }

    return 0;
}

int readVideoFrame(int handle,
                   int* timeStamp, 
                   char* pOBuff, 
                   int* pSize, // INPUT: BUFF SIZE, OUTPUT: DATA LEN.
                   int* pIsKeyFrame,AVI_DATA_TYPE *frameType)
{   
    *frameType = AVI_TYPE_VIDEO_I;
    char* pVideoBuf = NULL;
    int len = 0;
    int ret = 0;
    char* pBuff = NULL;

NEXT_LOOP:    

    if (handle) {       
        
        // 0: ok, otherwize: fail.
        ret = av_call_AviDemuxGetNextFrame(handle, 
                                     (int*)frameType,
                                     &pVideoBuf,&len);        
        if (ret) {
            return -1;
        } 

        if(0 == (ret = isValidVideFrame(handle, 
                         *frameType,
                         pVideoBuf,
                         len))) goto NEXT_LOOP;

    }

    if (timeStamp) 
        *timeStamp = (unsigned int)(mp4_call_Av_DemuxGetFrameTimestamp(handle)/1000);
    
    pBuff = pOBuff;

    if (pSize) *pSize = 0;
    
    if (pOBuff) {         
         if(ret==2 && gFileHeadData && gFileHeadLen>0) {
            
             memcpy(pBuff, gFileHeadData, gFileHeadLen);
             pBuff+=gFileHeadLen;
             if (pSize) *pSize += gFileHeadLen;
         }

         memcpy(pBuff, pVideoBuf, len);
    }
    if (pSize) *pSize += len;
    if (*frameType==AVI_TYPE_VIDEO_I) *pIsKeyFrame = 1;
    else *pIsKeyFrame = 0;

    if (0) printf("timeStamp:%u, buffHeadLen:%d, size:%d, len:%d, isKeyFrame:%d\n", *((unsigned int*)timeStamp),
        gFileHeadLen,*pSize, len, *pIsKeyFrame);

    return 0;
}

