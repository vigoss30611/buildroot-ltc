#ifndef AVI_DEMUX_H
#define AVI_DEMUX_H
/*
 * Copyright (c) 2014 Infotm, Inc.  All rights reserved.
 * 20140520 fengwu create.
 */

#define AVI_LOG(fmt, args...) printf(fmt, ##args)
#define AVI_CHECK_ERR(cond) {\
if(cond) {\
    AVI_LOG("Error! %s:%d, %s\n", __FILE__, __LINE__, #cond); \
    return; \
    }\
}

#define AVI_CHECK_ERR_RET(cond) {\
    if(cond) {\
        AVI_LOG("Error! %s:%d, %s\n", __FILE__, __LINE__, #cond); \
        return -1; \
    }\
}

typedef enum{
    AVI_TYPE_AUDIO,
    AVI_TYPE_VIDEO_I,
    AVI_TYPE_VIDEO_P,
} AVI_DATA_TYPE;


#ifdef __cplusplus 
extern "C" {
#endif
/*
 * Description:
 *      init avi demux content
 * Parameters:
 *      filePathName - avi file name stored in t-card
 *      pKeyFrameCount - key video frame count
 * Return:
 *      NULL - fail
 *      others - handle
 */  
int AviDemuxInit(const char* filePathName, int *pKeyFrameCount, int *pWidth, int *pHeight);

/*
 * Description:
 *      deinit avi demux content
 */  
void AviDemuxDeinit(const long handle);

/*
 * Description:
 *      get next frame data
 * Parameters:
 *      handle - avi handle
 *      pMediaType - pointer to type
 *      bufOut - avi data buffer, caller don't need to allocate
 *      pLen - pointer to length 
 * Return:
 *      0 - succeed
 *      -1 - fail
 */  
int AviDemuxGetNextFrame(const int handle, 
       AVI_DATA_TYPE *pMediaType, unsigned char** pBufOut, int *pLen);


/*
 * Description:
 *      get frame data
 * Parameters:
 *      handle - avi handle
 *      frmIdx - frame index
 *      pMediaType - pointer to type
 *      bufOut - avi data buffer, caller don't need to allocate
 *      pLen - pointer to length 
 * Return:
 *      0 - succeed
 *      -1 - fail
 */
int AviDemuxGetFrame(const int handle, int frmIdx,
       AVI_DATA_TYPE *pMediaType, unsigned char** pBufOut, int *pLen);

/*
 * Description:
 *      get next I video frame data
 * Parameters:
 *      handle - avi handle
 *      bufOut - avi data buffer, caller don't need to allocate
 *      pLen - pointer to length 
 *      index - frame index
 * Return:
 *      0 - succeed
 *      -1 - fail
 */
/*
int AviDemuxGetNextKeyFrame(const int handle, char* bufOut, int *pLen);
int AviDemuxGetPriorKeyFrame(const int handle, char* bufOut, int *pLen);
int AviDemuxGetKeyFrame(const int handle, int index, char* bufOut, int *pLen);
*/

/*
 * Description:
 *      get avi file duration in miliseconds
 */
int AviDemuxGetDurationMs(const long handle);

/*
 * Description:
 *      get display time per frame in microsecond
 */
int AviDemuxGetUsPerFrame(const long handle);

#ifdef __cplusplus 
}
#endif

#endif
