#include <apicallervideo.h>
#include <assert.h>
#include <qsdk/videobox.h>
#include <qsdk/vplay.h>
#include <string.h>
#include <define.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define VIDEO_SAVE_FRAME_NUMBER 20
void ApiCallerVideo::DebugVideoFrame(struct fr_buf_info *fr, bool newState){
    static int counter = 0;
    static int frameCounter = 0;
    static int fd =-1 ;
    char buf[256];
    int headerSize ;
    int result;
    static bool IFrameReady;
    if(newState == false){
        return;
    }
    if(newState == true && fd < 0 ){
        IFrameReady = false;
        frameCounter = 0;
        sprintf(buf,"/nfs/video_%d.raw",counter);
        fd = open(buf,O_WRONLY|O_CREAT|O_TRUNC,0666);
        if(fd < 0){
            printf("open file error->%s\n",buf);
            IFrameReady = false;
            return ;
        }
        printf("open new save file ->%s\n",buf);
        fchmod(fd,0666);

        //if(this->PVideoInfo->GetHeader(buf, &headerSize) == NULL){
        if(0){
            printf("get video header error, so no header set\n");
        }
        else {

            char preSet[] = {
                0x00,0x00,0x00,0x01,0x27,0x4D,0x60,0x33,0x8D,0x68,
                0x04,0x40,0x22,0x69,0xB2,0x00,0x00,0x03,0x00,0x02,
                0x00,0x00,0x03,0x00,0x3C,0x1E,0x28,0x45,0x40,0x00,
                0x00,0x00,0x01,0x28,0xEE,0x04,0x49,0x20
            };
            printf("write stream header -> 38\n");
            result = write(fd, preSet, 38);
            if(result != 38){
                printf("write header error\n");
            }
        }
        counter++;
    }
    if(IFrameReady == false && fr->priv != VIDEO_FRAME_I){
        printf("skip and wait i frame\n");
        return ;
    }
    IFrameReady =  true;
#if 1
    if(frameCounter> VIDEO_SAVE_FRAME_NUMBER){
        printf("close temp file write ->%d\n", fd);
        close(fd);
        fd = -1;
        this->ThreadPauseFlag =  false;
        return ;
    }
#endif
    printf("Frame: 0x%3X %p %8lld %5d\n", fr->priv, fr->virt_addr,
    fr->timestamp, fr->size);
    //printf("frame->%3X %6d %8lld\n", fr->priv, fr->size, fr->timestamp);
    result = write(fd, fr->virt_addr, fr->size);
    if(result != fr->size){
        printf("write frame error->%x %5d %5d\n",fr->priv,fr->size,result);
    }
    frameCounter++;
    //start save
}
ThreadingState ApiCallerVideo::WorkingThreadTask(void )
{
    int ret = 0 ;
    struct fr_buf_info fr;
    ret = video_get_frame(this->DefChannel,&this->FrFrame);
    if(ret < 0 ){
        LOGGER_WARN("get video raw frame error ->%d %s\n",ret,this->DefChannel);
        return ThreadingState::Keep;
    }
    if(this->NeedTriggerFlag.load() == true){
        video_trigger_key_frame(this->DefChannel);
        this->NeedTriggerFlag.store(false);
    }
    if(this->AutoCheckFps.load() == true){
        UpdateVideoInfo(this->FrFrame.timestamp);
    }
#if 1
    if(this->FifoAutoFlush == true) {// no recorder back mode
        int queueSize = this->PFrFifo->GetQueueSize();
        if(queueSize > 8 ){
            if((queueSize % 3) == 0)
            LOGGER_INFO("video FULL->t:%3X ts:%8lld size:%6d ct:%5d queue:%2d\n",
                    this->FrFrame.priv,
                    this->FrFrame.timestamp,
                    this->FrFrame.size,
                    this->PFrFifo->GetCurCacheTimeLen(),
                    queueSize);
        }
    }
#endif
    if(0 == this->FrFrame.size){
        LOGGER_ERR("video size 0~~~~~~~~~~~~~~~~\n");
    }
    else {
#if 0
        LOGGER_INFO("-->video push ts:%8lld q:%3d size:%5d\n",
                this->FrFrame.timestamp,
                this->PFrFifo->GetQueueSize(),
                this->FrFrame.size);
#endif
        fr =  this->FrFrame;
        if(this->LastTs == MEDIA_MUXER_HUGE_TIMESDTAMP){
            this->LastTs = fr.timestamp;
        }
        else{
            if(fr.timestamp <= this->LastTs){
                LOGGER_WARN("XXXXX  api video interval error  ->ts:%8lld last:%8lld --> -%lld\n",
                        fr.timestamp, this->LastTs, this->LastTs - fr.timestamp);
                fr.timestamp = ++this->LastTs;
            }
#if 1
            else{
                if(fr.timestamp - this->LastTs > 300){
                    LOGGER_WARN("OOOOO  api video interval error  ->ts:%8lld last:%8lld ->%lld\n",
                            fr.timestamp, this->LastTs, fr.timestamp - this->LastTs);
                }
                this->LastTs = fr.timestamp;
            }
#endif
        }
        if( this->PFrFifo->PushItem(&fr) < 0){
            LOGGER_WARN("push video error ->%4X %d\n", this->FrFrame.priv, this->FrFrame.size);
        }

    }
//    printf("video timestamp ->%8lld\n",this->FrFrame.timestamp);
    if(this->FrameNum % 480 == 0) {
        LOGGER_INFO("-->video-> %4d %4d %6lld %5dKb:%dB\n",
                this->PFrFifo->GetQueueSize(),
                this->FrFrame.timestamp,
                this->FrameNum,
                this->FrFrame.size/1024,
                this->FrFrame.size%1024);
    }
    video_put_frame(this->DefChannel,&this->FrFrame);
    this->FrameNum++;
    return  ThreadingState::Keep;
}
bool ApiCallerVideo::Prepare(void) {
    LOGGER_TRC("video stream channel ->%s<-\n",this->DefChannel);
    char headerBuf[128];
    int headerSize;
    int width ,height,fps;
    struct v_basic_info videoBasicInfo ;

    if( video_get_basicinfo(this->DefChannel,&videoBasicInfo) < 0){
        printf("get basic video info error\n");
        return false ;
    }
    LOGGER_TRC("video basic info ->type:%d  profile:%d nals:%d \n",videoBasicInfo.media_type ,
            videoBasicInfo.profile,
            videoBasicInfo.stream_type);
    if ( video_get_resolution( this->DefChannel,&width,&height)  < 0 ){
        printf("get basic video resolution error\n");
        goto Prepare_Error_Handler ;
    }
    fps = video_get_fps(this->DefChannel) ;
    if(fps <= 0 ){
        printf("get basic video info error\n");
        goto Prepare_Error_Handler ;
    }
    struct v_bitrate_info videoBitrateInfo ;
    if( video_get_bitrate(this->DefChannel, &videoBitrateInfo) < 0 ){
        printf("get bitrate error\n");
        goto Prepare_Error_Handler ;
    }
    headerSize = video_get_header(this->DefChannel,headerBuf,128);
    if(headerSize <= 0 ){
        LOGGER_ERR("stream header size(%d) error \n", headerSize);
    }


    this->PVideoInfo->SetPFrame(videoBitrateInfo.p_frame);
    this->PVideoInfo->SetBitRate(videoBitrateInfo.bitrate);
    this->PVideoInfo->SetGopLength(videoBitrateInfo.gop_length);
    this->PVideoInfo->SetHeader(headerBuf, headerSize);

    this->PVideoInfo->SetWidth(width) ;
    this->PVideoInfo->SetHeight(height);
    switch (videoBasicInfo.media_type){
        case VIDEO_MEDIA_MJPEG :
            this->PVideoInfo->SetType(VIDEO_TYPE_MJPEG);
            break;
        case VIDEO_MEDIA_H264 :
            this->PVideoInfo->SetType(VIDEO_TYPE_H264);
            break;
        case VIDEO_MEDIA_HEVC :
            this->PVideoInfo->SetType(VIDEO_TYPE_HEVC);
            break;
        default :
            LOGGER_ERR("get video type not support ->%d\n",videoBasicInfo.media_type);
            goto Prepare_Error_Handler ;
    }
    /*
    if(this->PVideoInfo->GetBitRate() <  500*1024){
        this->PVideoInfo->SetBitRate(800 * 1024);
    }
    else{
        this->PVideoInfo->SetBitRate(this->PVideoInfo->GetBitRate() * 3 / 2);
    }
    */
    this->PVideoInfo->SetFps(fps);
    this->PVideoInfo->SetFrameSize(  this->PVideoInfo->GetBitRate() /( this->PVideoInfo->GetFps()*8));
    //this->PVideoInfo->PrintInfo();
    return true ;
Prepare_Error_Handler :
//    this->SetWorkingState(ThreadingState::Stop);
    return false ;
}
ApiCallerVideo::ApiCallerVideo(char *channel, VideoStreamInfo *info):ThreadCore(){
    assert(channel != NULL);
    assert(info != NULL);
    this->PVideoInfo = info;
    assert(strlen(channel) < STRING_PARA_MAX_LEN  );
    memcpy(this->DefChannel,channel ,strlen(channel) + 1);
    //memset(&this->VideoInfo,0,sizeof(video_info_t));
    fr_INITBUF(&this->FrFrame,NULL);
    this->FrameNum =  0;
//    this->NewDebugStart =  false ;
    this->FifoAutoFlush  =  false;
    this->SetName((char *)"rec_video");
    this->ThreadPauseFlag = true;
    this->FrameCounter = 0;
    this->NeedTriggerFlag.store(true);
    this->AutoCheckFps.store(true);
    this->LastTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->OldestFrame.store(false);
}
ApiCallerVideo::~ApiCallerVideo(){
    LOGGER_DBG("deinit api video\n");
}

bool ApiCallerVideo::ThreadPreStartHook(void){
    LOGGER_DBG("start video thread hook\n");
    fr_INITBUF(&this->FrFrame,NULL);
    this->FrFrame.oldest_flag = OldestFrame.load();
    this->NeedTriggerFlag.store(true);
    this->FrameCounter = 0;
    this->LastTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
}
bool ApiCallerVideo::ThreadPostPauseHook(void){
    LOGGER_INFO("++++++pause post hook video channel->%s<-\n",this->DefChannel);
//    this->NewDebugStart =  true ;
    if(this->FifoAutoFlush){
        LOGGER_INFO(" video pause clear fifo\n");
        this->PFrFifo->Flush();
    }
    this->FrameCounter = 0;
    fr_INITBUF(&this->FrFrame,NULL);
    this->PVideoInfo->SetUpdate(false);
    this->ThreadPauseFlag = true;
    this->LastTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
}
bool ApiCallerVideo::ThreadPostStopHook(void){
    LOGGER_INFO("++++++stop post hook video channel->%s<-\n",this->DefChannel);
    if(this->FifoAutoFlush){
        LOGGER_INFO(" video stop clear fifo\n");
        this->PFrFifo->Flush();
    }
    fr_INITBUF(&this->FrFrame,NULL);
    this->PVideoInfo->SetUpdate(false);
    this->ThreadPauseFlag = true;
    this->FrameCounter = 0;
    this->LastTs = MEDIA_MUXER_HUGE_TIMESDTAMP;

}
void ApiCallerVideo::SaveDebugFrame(struct fr_buf_info *pFr){

}
void ApiCallerVideo::SetFifoAutoFlush(bool enable){
    this->FifoAutoFlush =  enable ;
}
void ApiCallerVideo::UpdateVideoInfo(uint64_t timestamp){
    int ret  = 0;
    static uint32_t checkInterval = 3;
    static uint64_t lastTimestamp = 0;

    if(this->FrameCounter == 0){
        checkInterval = 3;
        lastTimestamp = 0;
    }

    if(lastTimestamp == 0 ){
        lastTimestamp =  timestamp;
    }

    if(this->FrameCounter % checkInterval == 0)
    {
        if (timestamp > lastTimestamp)
        {
            uint32_t interval = timestamp - lastTimestamp ;
            interval = interval / checkInterval;
            uint32_t fps = 1000 / interval;
            if(fps == 0){
                LOGGER_WARN("++++++frame fps == 0??,abort setting, interval:%lu\n", interval);
            }
            else
            {
                uint32_t left = 1000 % fps ;
                if((left *2 ) > interval){
                    fps++;
                }
                //LOGGER_INFO("update fps ->%4d %4d %4d\n", fps, interval,left);
                this->PVideoInfo->SetFps(fps);
                this->PVideoInfo->SetUpdate(true);
            }
            lastTimestamp = timestamp;

            if(this->FrameCounter == 30){
                checkInterval = 15;
            }
        }
    }
    this->FrameCounter++;
}
void ApiCallerVideo::SetAutoCheckFps(bool check){
    this->AutoCheckFps.store(check);
}

void ApiCallerVideo::SetCaheMode(bool mode) {
    OldestFrame.store(mode);
}