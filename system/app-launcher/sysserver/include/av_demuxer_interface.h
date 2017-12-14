#ifndef __AV_DEMUXER_INTERFACE_H__
#define __AV_DEMUXER_INTERFACE_H__

typedef enum{
    AV_TYPE_AUDIO,
    AV_TYPE_VIDEO_I,
    AV_TYPE_VIDEO_P,
} AV_DATA_TYPE;

typedef enum AV_VIDEO_TYPE{
    AV_VIDEO_NONE,
    AV_VIDEO_H264,
    AV_VIDEO_H265,
}AV_VIDEO_TYPE;

/*
 * Description:
 *      init media demux context
 * Parameters:
 *      filePathName - media file name stored in t-card
 *      pKeyFrameCount - key video frame count
 * Return:
 *      NULL - fail
 *      others - handle
 */  
int Av_DemuxInit(const char* filePathName, int *pKeyFrameCount, int *pWidth, int *pHeight);

/*
 * Description:
 *      deinit media demux context
 */  
void Av_DemuxDeinit(int handle);

/*
 * Description:
 *      get video type
 */
AV_VIDEO_TYPE Av_DemuxGetVideoType(const int handle);

/*
 * Description:
 *      get next frame data
 * Parameters:
 *      handle - media handle
 *      pMediaType - pointer to type
 *      bufOut - media data buffer, caller don't need to allocate
 *      pLen - pointer to length 
 *      timestamp -- frame timestamp
 * Return:
 *      0 - succeed
 *      -1 - fail
 */  
int Av_DemuxGetNextFrame(const int handle, 
       AV_DATA_TYPE *pMediaType, unsigned char** pBufOut, int *pLen);

/*
 * Description:
 *      get frame timestamp, must called after Av_DemuxGetNextFrame
 * Parameters:
 * Return:
 *      timestamp   microsecond
 */
long long Av_DemuxGetFrameTimestamp(const int handle);

/*
 * Description:
 *      get frame data
 * Parameters:
 *      handle - media handle
 *      frmIdx - frame index
 *      pMediaType - pointer to type
 *      bufOut - media data buffer, caller don't need to allocate
 *      pLen - pointer to length 
 * Return:
 *      0 - succeed
 *      -1 - fail
 */
int Av_DemuxGetFrame(const int handle, int frmIdx,
       AV_DATA_TYPE *pMediaType, unsigned char** pBufOut, int *pLen);


/*
 * Description:
 *      get media file duration in miliseconds
 */
int Av_DemuxGetDurationMs(const int handle);

/*
 * Description:
 *      get display time per frame in microsecond
 */
int Av_DemuxGetUsPerFrame(const int handle);


#endif

