#include <mediamuxer.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <qsdk/event.h>
#include <qsdk/videobox.h>
#include <qsdk/codecs.h>
#include <h264_stream.h>
#include <bs.h>
#include <h264_sei.h>

#define  MUXER_DISPLAT_INTERVAL 50

#define MEDIA_MUXER_INFO_FIFO       0x1
#define MEDIA_MUXER_INFO_FRAMENUM   0x10
#define MEDIA_MUXER_INFO_FILENUM    0x100

#define MEDIA_MUXER_RETRY_MAX_NUM   20000
#ifdef  FRAGMENTED_MP4_SUPPORT
#pragma message("############  fragmented mp4 enabled")
#else
#pragma message("############  fragmented mp4 disabled")
#endif
#define MEDIA_MUXER_SLOW_FAST_GLOBAL  0
#define MAX_RETRY_CNT 50
std::atomic_bool IsVideoboxRun;

void recorder_listener(char *event, void *arg)
{
    char* msg = (char*)arg;
    if ((!strcmp(event, EVENT_VIDEOBOX)) && (msg))
    {
        printf("get videobox msg:%s\n", msg);
        if ((!strcmp(msg, "force_close")) ||
            (!strcmp(msg, "exiting")) ||
            (!strcmp(msg, "exit")))
        {
            IsVideoboxRun.store(false);
        }
    }

}

ThreadingState MediaMuxer::WorkingThreadTask(void ){
    static int tryCounter = 0;
    int ret = 0;
    bool retBool;
    ThreadingState  realTimeState = ThreadingState::Keep;

    if (strlen(RecInfo.video_channel))
        stat.UpdateFifoInfo(PACKET_TYPE_VIDEO,this->PFrFifoVideo[0]->GetQueueSize(),this->PFrFifoVideo[0]->GetDropFrameSize());
    if (this->MediaEnableAudio)
        stat.UpdateFifoInfo(PACKET_TYPE_AUDIO,this->PFrFifoAudio->GetQueueSize(),this->PFrFifoAudio->GetDropFrameSize());
    stat.ShowInfo();

    if (!IsVideoboxRun.load())
    {
        this->RunErrorCallBack();
        return ThreadingState::Pause;
    }

    if(this->NewFileFlags.load() == true){ //wait for i slice and then begin
        //update new file
        LOGGER_INFO("start create new file\n");
        if(this->StartNewMediaFile(true) == false){
            LOGGER_ERR("create new media file error\n");
            vplay_report_event(VPLAY_EVENT_DISKIO_ERROR ,(char *)"start new media error true");
            return ThreadingState::Pause;
        }
        vplay_report_event(VPLAY_EVENT_NEWFILE_START ,(char *)this->CurrentMediatFileName);
        LOGGER_TRC("Start New meidia file ok\n");
        //try open new muxer
        if (this->MediaEnableVideo){
            //wait for i
            LOGGER_DBG("new file and wait first I frame\n");
            LOGGER_INFO("new file and wait first I frame VideoCount:%d RecInfo.video_channel:%s RecInfo.videoextra_channel:%s\n",
                VideoCount , RecInfo.video_channel, RecInfo.videoextra_channel);
            if (!this->IsKeepFrame[0] || !this->IsKeepFrame[1])
            {
                LOGGER_INFO("trigger I frame!\n");
                if (strlen(RecInfo.video_channel)) {
                    video_trigger_key_frame(RecInfo.video_channel);
                }

                if (strlen(RecInfo.videoextra_channel)) {
                     video_trigger_key_frame(RecInfo.videoextra_channel);
                }
                if (this->NextStartTime == 0)
                {
                    time(&this->NextStartTime);
                }
            }
            if ((this->bRestoreBuf.load()) && (this->VideoCount == 1))
            {
                this->bRestoreBuf.store(false);
                if (this->FrKeepVideo[0].buf != NULL)
                {
                    this->PFrFifoVideo[0]->PutItem(&this->FrKeepVideo[0]);
                    this->FrKeepVideo[0].buf = this->FrKeepVideo[0].virt_addr = NULL;
                }

                uint64_t u64CacheTime = this->PFrFifoVideo[0]->GetMaxCacheTime();
                if (u64CacheTime < this->RecInfo.s32KeepFramesTime * 1000)
                {
                    this->PFrFifoVideo[0]->SetMaxCacheTime(u64CacheTime + this->RecInfo.s32KeepFramesTime * 1000);
                }
                u64CacheTime = this->PFrFifoAudio->GetMaxCacheTime();
                if (u64CacheTime < this->RecInfo.s32KeepFramesTime * 1000)
                {
                    this->PFrFifoAudio->SetMaxCacheTime(u64CacheTime + this->RecInfo.s32KeepFramesTime * 1000);
                }

                this->PFrFifoVideo[0]->RestoreKeepFrames();
                this->PFrFifoAudio->RestoreKeepFrames();
                this->IsKeepFrame[0] = false;
                LOGGER_INFO("Restore keep frames OK.\n");
            }

            for (int index=0;index<VideoCount;index++) {
                if (!this->IsKeepFrame[index])
                {
                    LOGGER_INFO("Enter PopFirstIFrame\n");
                    realTimeState = this->PopFirstIFrame(PFrFifoVideo[index], &FrVideo, &ret);
                    LOGGER_INFO("Out of PopFirstIFrame\n");
                    LOGGER_INFO("ret:%d realTimeState:%d\n", ret, realTimeState);
                    if (ret == 0) {
                        this->PFrFifoVideo[index]->PutItem(&this->FrVideo);
                        LOGGER_INFO("===>Can not pop I frame index:%d\n",index);
                        this->NewFileFlags.store(true);
                        usleep(5000);
                        return realTimeState;
                    }
                }
                else
                {
                    this->FrVideo = this->FrKeepVideo[index];
                    if (index == VideoCount-1)
                    {
                        this->IsKeepFrame[index] = false;
                    }
                }

                if(index == 0) {
                    //memcpy(&FrVideoTmp, &FrVideo, sizeof(fr_buf_info));
                    this->NewFileFlags.store(false);
                    //update all flags
                    this->NeedUpdateFrameVideo = true ;
                    this->NeedUpdateFrameAudio = this->MediaEnableAudio ? true :false;
                    this->NeedUpdateFrameExtra = this->MediaEnableExtra ? true :false;
                    //update first timestamp
                    this->FirstTimeStamp =  this->FrVideo.timestamp ;
                    if (this->FirstStartTime == 0)
                    {
                        this->FirstStartTime = this->FirstTimeStamp;
                    }
                    this->StartTime = this->NextStartTime;
                    LOGGER_INFO("+++++start video info ->v%d a%d e%d\n",this->NeedUpdateFrameVideo, this->NeedUpdateFrameAudio, this->NeedUpdateFrameExtra);
                    LOGGER_INFO("start recorder time ->%8ld\n",this->StartTime);
                    //update Last timestamp
                    this->LastTimeStamp =   this->FrVideo.timestamp ;
                    this->LastVideoTimeStamp = this->FrVideo.timestamp;
                    LOGGER_INFO("Video FirstTimeStamp:%lld LastTimeStamp:%lld NewFileFlags:%d\n",FirstTimeStamp, LastTimeStamp, NewFileFlags.load());
                }
                //LOGGER_INFO("========>Has found I Frame index:%d this->FirstTimeStamp:%lld FrVideo.timestamp:%lld\n\n",index, this->FirstTimeStamp, FrVideo.timestamp);
                LOGGER_INFO("Has found first I frame and record now NewFileFlags:%d index:%d\n", NewFileFlags.load(), index);
                if (this->FrVideo.timestamp >= this->FirstTimeStamp) {
                    this->AddVideoFrFrame(&this->FrVideo, index);
                } else {
                    index--;
                    if (strlen(RecInfo.video_channel)) {
                        video_trigger_key_frame(RecInfo.video_channel);
                    }
                    if (strlen(RecInfo.videoextra_channel)) {
                        video_trigger_key_frame(RecInfo.videoextra_channel);
                    }
                }
                this->PFrFifoVideo[index]->PutItem(&this->FrVideo);
            }
        }
 #if 1
        else if (this->MediaEnableAudio){
            //just read and storage
            while(true) {
                ret = this->PFrFifoAudio->GetItem(&this->FrAudio);
                if(ret  <= 0){
                    //LOGGER_DBG(" get out audio error ->%p %d\n",this->PFrFifoAudio,this->PFrFifoAudio->GetFifoSize());
                    usleep(10000);
                    tryCounter++;
                    if(tryCounter > MEDIA_MUXER_RETRY_MAX_NUM){
                        vplay_report_event(VPLAY_EVENT_AUDIO_ERROR,(char *)"no audio frame");
                        LOGGER_WARN("no audio stream ,just stop muxer\n");
                        vplay_report_event(VPLAY_EVENT_AUDIO_ERROR ,(char *)"no audio stream ,pause muxer\n");
                        return ThreadingState::Pause;
                    }
                    if(tryCounter % 3 == 0){
                        realTimeState = this->GetWorkingState();
                        if(realTimeState != ThreadingState::Start ){
                            return  realTimeState;
                        }
                    }
                    continue ;
                }
                else {
                    tryCounter = 0;
                    break ;
                }
            }
            //update read flags
            this->NeedUpdateFrameVideo = false ;
            this->NeedUpdateFrameAudio = true ;
            this->NeedUpdateFrameExtra = this->MediaEnableExtra ? true :false ;
            //update first timestamp
            this->FirstTimeStamp =  this->FrAudio.timestamp ;
            //update Last timestamp
            this->LastTimeStamp =  this->FrAudio.timestamp;
            //store audio stream
            this->AddAudioFrFrame(&this->FrAudio);
            if(this->PFrFifoAudio->PutItem(&this->FrAudio) <= 0){
                LOGGER_ERR("free fifo buffer error\n");
            }
        }
 #endif
    }
    else {
        //if need ,loop read audio utils large this->LastTimeStamp
        if(this->MediaEnableVideo ){
            if (this->EnhanceThreadPriority == false)
            {
                bool bBufferFull = false;
                for (int index=0; index<this->VideoCount; index++)
                {
                    if(this->PFrFifoVideo[index]->GetQueueSize() > MAX_BUFFER_COUNT)
                    {
                        bBufferFull = true;
                        LOGGER_INFO("buffer is full, enhance priority(index:%d,count:%d).\n",
                            index, this->PFrFifoVideo[index]->GetQueueSize());
                        break;
                    }
                }
                if (bBufferFull)
                {
                    this->EnhancePriority(bBufferFull);
                    this->EnhanceThreadPriority = true;
                }
            }

            //check if large LastTimeStamp
            for (int index=0;index<this->VideoCount;index++) {
                ret = this->PFrFifoVideo[index]->GetItem(&this->FrVideo);
                if(ret <= 0){
                    //LOGGER_INFO("video ->%8lld  ->%d\n",this->FrVideo.timestamp- this->FirstTimeStamp, ret);
                    //printf("pop video frame error index:%d !\n", index);
                }
                else {
                    if (this->FastMotionByPlayer)
                    {
                        if ((this->VideoSkipFrameNum > 0) || (this->FrVideo.priv != VIDEO_FRAME_I))
                        {
                            this->PFrFifoVideo[index]->PutItem(&this->FrVideo);
                            this->VideoSkipFrameNum -- ;
                            continue;
                        }
                        this->VideoSkipFrameNum = 0 - this->SlowFastMotionRate.load() - 1;
                    }
                    if ((this->CheckIFrame[index]) && (this->FrVideo.priv == VIDEO_FRAME_I))
                    {
                        ret = this->KeepIFrame(index);
                        if (!ret)
                        {
                            LOGGER_ERR("ret:%d\n", ret);
                            return ThreadingState::Pause;
                        }
                        break;
                    }
                    else
                    {
                        retBool = this->AddVideoFrFrame(&this->FrVideo, index);
                        this->PFrFifoVideo[index]->PutItem(&this->FrVideo);
                        if(retBool == false){
                            LOGGER_ERR("write video error,thread pause\n");
                            vplay_report_event(VPLAY_EVENT_VIDEO_ERROR ,(char *)"write video stream error,pause muxer\n");
                            this->RunErrorCallBack();
                            return ThreadingState::Pause;
                        }
                        else {
                            //Caculate_FPS();
                            this->LastTimeStamp = this->FrVideo.timestamp;
                            this->LastVideoTimeStamp = this->FrVideo.timestamp;
                        }
                    }
                }
            }
        }

        if(this->MediaEnableAudio){
            //check if large LastTimeStamp
            if(this->NeedUpdateFrameAudio){
                ret = this->PFrFifoAudio->GetItem(&this->FrAudio);
                if(ret <= 0){
                    //LOGGER_WARN("get out audio error\n");
                    this->NeedUpdateFrameAudio = true ;
                }
                else {
                    if(this->FrAudio.timestamp < this->FirstTimeStamp){
                        LOGGER_WARN("just drop old audio frame 1 -> %llu\n", this->FrAudio.timestamp);
                        this->PFrFifoAudio->PutItem(&this->FrAudio);
                        this->NeedUpdateFrameAudio = true ;
                    }
                    else if(this->FrAudio.timestamp < this->LastVideoTimeStamp){

                        retBool = this->AddAudioFrFrame(&this->FrAudio);
                        this->PFrFifoAudio->PutItem(&this->FrAudio);
                        if(retBool == false ){
                            LOGGER_ERR("write audio error,thread pause\n");
                            vplay_report_event(VPLAY_EVENT_AUDIO_ERROR ,(char *)"write audio stream error,pause muxer\n");
                            this->RunErrorCallBack();
                            return ThreadingState::Pause;
                        }
                        //LastTimeStamp should keep lagerest timestamp
                        if (this->FrAudio.timestamp > this->LastTimeStamp) {
                            this->LastTimeStamp =  this->FrAudio.timestamp;
                        }
                        this->NeedUpdateFrameAudio = true ;
                    }
                    else{
                        this->NeedUpdateFrameAudio = false ;
                    }
                }
            }
            else {
                //printf("--- audio ->video:%8lld:%8lld\n",this->LastVideoTimeStamp,this->FrAudio.timestamp);
                if(this->FrAudio.timestamp < this->FirstTimeStamp){
                    LOGGER_WARN("just drop old audio frame 2 -> %lld\n", this->FrAudio.timestamp);
                    if(this->PFrFifoAudio->PutItem(&this->FrAudio) <= 0){
                        LOGGER_WARN("free audio buf error\n");
                    }
                    this->NeedUpdateFrameAudio = true ;
                }
                else if(this->FrAudio.timestamp < this->LastVideoTimeStamp){

                    retBool = this->AddAudioFrFrame(&this->FrAudio);
                    this->PFrFifoAudio->PutItem(&this->FrAudio);
                    if(retBool == false ){
                        LOGGER_ERR("write audio error,thread pause\n");
                        vplay_report_event(VPLAY_EVENT_AUDIO_ERROR ,(char *)"write audio stream error,pause muxer\n");
                        this->RunErrorCallBack();
                        return ThreadingState::Pause;
                    }
                    //LastTimeStamp should keep lagerest timestamp
                    if (this->FrAudio.timestamp > this->LastTimeStamp) {
                        this->LastTimeStamp = this->FrAudio.timestamp;
                    }
                    this->NeedUpdateFrameAudio = true ;
                }
                else if (this->FrAudio.timestamp > this->LastVideoTimeStamp + 10000)
                {
                    LOGGER_INFO("Audio frame time stamp error:%llu LastVideoTimeStamp:%llu LastTimeStamp:%llu\n",
                        this->FrAudio.timestamp, this->LastVideoTimeStamp, this->LastTimeStamp);
                    this->FrAudio.timestamp = this->LastTimeStamp + 5;
                    this->NeedUpdateFrameAudio = false ;
                }
                else
                {
                    this->NeedUpdateFrameAudio = false ;
                }
            }
        }
        //usleep(5000);
        return ThreadingState::Keep;
        //disable

        //if need loop read extra utils large this->LastTimeStamp
        while(this->NeedUpdateFrameExtra ){
            //check if large LastTimeStamp
            ret = this->PFrFifoExtra->GetItem(&this->FrExtra);
            if(ret <= 0){
                //    LOGGER_DBG("get out extra error\n");
                usleep(10000);
                tryCounter++;
                if(tryCounter > MEDIA_MUXER_RETRY_MAX_NUM/10){
                    LOGGER_ERR("get extra frame error\n");
                    vplay_report_event(VPLAY_EVENT_EXTRA_ERROR,(char *)"no gps frame");
                    return ThreadingState::Pause;
                }
                if(tryCounter % 3 == 0){
                    realTimeState = this->GetWorkingState();
                    if(realTimeState != ThreadingState::Start ){
                        return  realTimeState;
                    }
                }
                continue ;
            }
            tryCounter = 0 ;
            if (this->FrExtra.timestamp >= this->LastTimeStamp){
                //
                break ;
            }
        }
        this->NeedUpdateFrameExtra = false ;
        //switch(this->ChooseStream(&this->FrVideo,&this->FrAudio,&this->FrExtra) ) {
        switch(this->ChooseStream(&this->FrVideo,&this->FrAudio,&this->FrExtra) ) {
            case 0 :
                //add video stream
                if( this->AddVideoFrFrame(&this->FrVideo, ExtraStreamIndex) == false ){
                    this->PFrFifoVideo[ExtraStreamIndex]->PutItem(&this->FrVideo);
                    LOGGER_ERR("write video error,thread pause\n");
                    vplay_report_event(VPLAY_EVENT_VIDEO_ERROR ,(char *)"write video stream error,pause muxer\n");
                    this->RunErrorCallBack();
                    return ThreadingState::Pause;
                //    this->InternalReleaseMuxerFile();
                }
                else{
                    this->PFrFifoVideo[ExtraStreamIndex]->PutItem(&this->FrVideo);
                }
                this->NeedUpdateFrameVideo =  true ;
                this->LastTimeStamp =  this->FrVideo.timestamp;
                this->LastVideoTimeStamp = this->FrVideo.timestamp;
                break;
            case 1 :
                if(this->AddAudioFrFrame(&this->FrAudio) == false ){
                    this->PFrFifoAudio->PutItem(&this->FrAudio);
                    LOGGER_ERR("write audio error,thread pause\n");
                    vplay_report_event(VPLAY_EVENT_AUDIO_ERROR ,(char *)"write audio stream error,pause muxer\n");
                    this->RunErrorCallBack();
                    return ThreadingState::Pause;
                }
                else{
                    this->PFrFifoAudio->PutItem(&this->FrAudio);
                }
                this->NeedUpdateFrameAudio =  true ;
                this->LastTimeStamp =  this->FrAudio.timestamp;
                //add audio stream
                break;
            case 2 :
                if(this->AddExtraFrFrame(&this->FrExtra) == false ){
                    this->PFrFifoExtra->PutItem(&this->FrAudio);
                    //usleep(1000);
                    LOGGER_ERR("write extra error,thread pause\n");
                    vplay_report_event(VPLAY_EVENT_EXTRA_ERROR ,(char *)"write extra stream error,pause muxer\n");
                    this->RunErrorCallBack();
                    return ThreadingState::Pause;
                }
                else{
                    this->PFrFifoExtra->PutItem(&this->FrAudio);
                }
                this->NeedUpdateFrameExtra =  true ;
                this->LastTimeStamp =  this->FrExtra.timestamp;
                //add extra stream
                break;
            default :
                LOGGER_TRC("choose stream error,may be no stream ready\n");
                //usleep(1000);
                break ;
        }
//        printf("flags ->%d %d %d\n",this->NeedUpdateFrameVideo,this->NeedUpdateFrameAudio,this->NeedUpdateFrameExtra);
    //    printf("fifo size->%d %d\n",this->PFrFifoVideo->GetQueueSize(),this->PFrFifoAudio->GetQueueSize());
    }
    /*
     * safe exit \n
     */
    return ThreadingState::Keep;

}

int MediaMuxer::KeepIFrame(int index)
{
    int ret = 1;

    this->IsKeepFrame[index] = true;
    this->FrKeepVideo[index] = this->FrVideo;
    if (this->VideoCount > 1)
    {
        int s32VideoTrack = 0;
        int s32GetCnt = 0;
        struct fr_buf_info stVideoFrame;

        memset(&stVideoFrame, 0, sizeof(struct fr_buf_info));
        if (index == 0)
        {
            s32VideoTrack = 1;
        }
        else
        {
            s32VideoTrack = 0;
        }

        while (this->CheckIFrame[s32VideoTrack])
        {
            if (s32GetCnt > MAX_RETRY_CNT)
            {
                this->IsKeepFrame[s32VideoTrack] = false;
                this->CheckIFrame[s32VideoTrack] = false;
                LOGGER_ERR("Can not get I Frame.\n");
                break;
            }

            ret = this->PFrFifoVideo[s32VideoTrack]->GetItem(&stVideoFrame);
            LOGGER_INFO("Get Video cnt %d s32VideoTrack %d %d\n", s32GetCnt, s32VideoTrack, stVideoFrame.priv);
            s32GetCnt++;
            if (ret > 0)
            {
                if (stVideoFrame.priv == VIDEO_FRAME_I)
                {
                    this->IsKeepFrame[s32VideoTrack] = true;
                    this->FrKeepVideo[s32VideoTrack] = stVideoFrame;
                    this->CheckIFrame[s32VideoTrack] = false;
                }
                else
                {
                    ret = this->AddVideoFrFrame(&stVideoFrame, s32VideoTrack);
                    this->PFrFifoVideo[s32VideoTrack]->PutItem(&stVideoFrame);
                }

                if (!ret)
                {
                    LOGGER_ERR("write video error,thread pause\n");
                    vplay_report_event(VPLAY_EVENT_VIDEO_ERROR, (char *)"write video stream error,pause muxer\n");
                    this->RunErrorCallBack();
                    return ret;//ThreadingState::Pause;
                }
            }
            else
            {
                printf("Get Video fail %d\n", s32GetCnt);
                usleep(30*1000);
            }
        }
    }
    else
    {
        this->CheckIFrame[1] = false;
    }
    this->CheckIFrame[index] = false;
    this->NewFileFlags.store(true);
    this->StartDelayTime = this->FirstStartTime + this->RecInfo.time_segment * 1000 - this->FrKeepVideo[index].timestamp;
    LOGGER_INFO("index:%d frame:%p time:%llu, size:%d\n",
        index,
        this->FrKeepVideo[index].virt_addr,
        this->FrKeepVideo[index].timestamp,
        this->FrKeepVideo[index].size);
    LOGGER_INFO("FirstStartTime:%llu time:%llu, StartDelayTime:%lld, Segment:%d\n",
        this->FirstStartTime,
        this->FrKeepVideo[index].timestamp,
        this->StartDelayTime,
        this->RecInfo.time_segment);
    this->FirstStartTime = this->FirstStartTime + this->RecInfo.time_segment * 1000;

    return ret;
}
bool MediaMuxer::Prepare(void) {

    char temp[VPLAY_PATH_MAX];
    memset(temp ,0 , VPLAY_PATH_MAX);
    if(this->GetFinalFileName(temp, VPLAY_PATH_MAX, true) == false ){
        LOGGER_ERR("recorder info file name para error,please check it \n");
        goto Prepare_Error_Handler ;
    }
    else {
        LOGGER_DBG("file name template example ->%s\n",temp);
    }
    /*
    switch(this->RecInfo->audio_format->type){
        case AUDIO
    }*/
    switch(this->RecInfo.audio_format.type ) {
        case  AUDIO_CODEC_TYPE_PCM:
            switch (this->RecInfo.audio_format.sample_fmt ){
                case AUDIO_SAMPLE_FMT_S16 :
                    this->AudioCodecID =  AV_CODEC_ID_PCM_S16LE ;
                    break;
                case AUDIO_SAMPLE_FMT_S32 :
                    this->AudioCodecID =  AV_CODEC_ID_PCM_S32LE ;
                    break ;
                default :
                    printf("pcm not support other sample format ->%d\n",this->RecInfo.audio_format.sample_fmt);
                    goto Prepare_Error_Handler ;

            }
            break;
        case  AUDIO_CODEC_TYPE_SPEEX:
            this->AudioCodecID =  AV_CODEC_ID_SPEEX ;
            break;
        case  AUDIO_CODEC_TYPE_G711U:
            this->AudioCodecID =  AV_CODEC_ID_PCM_MULAW ;
            break;
        case  AUDIO_CODEC_TYPE_G711A:
            this->AudioCodecID =  AV_CODEC_ID_PCM_ALAW ;
            break;
        case  AUDIO_CODEC_TYPE_ADPCM_G726:
            this->AudioCodecID =  AV_CODEC_ID_ADPCM_G726;
            break;
        case  AUDIO_CODEC_TYPE_MP3:
            this->AudioCodecID =  AV_CODEC_ID_MP3 ;
            break;
        case  AUDIO_CODEC_TYPE_AAC:
            this->AudioCodecID =  AV_CODEC_ID_AAC ;
            break;
        default :
            printf("not support audio codec format ");
            goto Prepare_Error_Handler ;
    }
    //    this->ExtraCodecId = this->AudioCodecID ;
    //
    memset(&this->FrVideo ,0 ,sizeof(struct fr_buf_info));
    memset(&this->FrAudio ,0 ,sizeof(struct fr_buf_info));
    memset(&this->FrExtra ,0 ,sizeof(struct fr_buf_info));

    this->PStreamChannel =  (stream_channel_info_t *) malloc(sizeof(stream_channel_info_t)*this->StreamChannelNum);
    if(this->PStreamChannel == NULL){
        LOGGER_ERR("malloc stream channel info error\n");
        goto Prepare_Error_Handler;
    }
    LOGGER_DBG("PStreamChannel malloc->%p\n",this->PStreamChannel);
    memset(this->PStreamChannel, 0, sizeof(stream_channel_info_t) * this->StreamChannelNum);

    this->PStreamChannel[0].media_type = VPLAY_MEDIA_TYPE_VIDEO;
    this->PStreamChannel[1].media_type = VPLAY_MEDIA_TYPE_AUDIO;
    this->PStreamChannel[2].media_type = VPLAY_MEDIA_TYPE_DATA;

    this->InitStreamChannel();
    for(int i = 0;i <strlen(this->RecInfo.av_format); i++){
        this->RecInfo.av_format[i] = tolower(this->RecInfo.av_format[i]);
    }
#ifdef FRAGMENTED_MP4_SUPPORT
    if(strncmp(this->RecInfo.av_format, "mp4", 3) == 0 || strncmp(this->RecInfo.av_format, "mov", 3) == 0){
        this->FragementedMediaSupport = true;
    }
    else {
        this->FragementedMediaSupport = false;
    }
#endif
    LOGGER_INFO("target format ->%s %d\n",this->RecInfo.av_format, this->FragementedMediaSupport);
    return true ;

Prepare_Error_Handler :
    this->SetWorkingState(ThreadingState::Stop);
    return true ;
}
MediaMuxer::MediaMuxer(VRecorderInfo *rec ,VideoStreamInfo **info, int video_count):ThreadCore(){
    int ret = 0;
    assert(rec!= NULL);
    this->RecInfo = *rec ;
    this->VideoCount = video_count;
    if (info != NULL && strlen(rec->video_channel ) !=   0){
        this->MediaEnableVideo = true ;
        this->PVideoInfo = info ;
    }
    else{
        this->MediaEnableVideo = false ;
    }
    if( strlen(rec->audio_channel) != 0 ){
        this->MediaEnableAudio  = true ;
    }
    else {
        this->MediaEnableAudio = false ;
    }
    this->MediaEnableExtra =  rec->enable_gps == 0 ? false :true ;
    if(this->MediaEnableExtra ){
    }

    //LOGGER_TRC("muxer info -> v%d a%d e%d\n",this->MediaEnableVideo,this->MediaEnableAudio,this->MediaEnableExtra);
    this->PFrFifoVideo = NULL ;
    this->PFrFifoAudio = NULL ;
    this->PFrFifoExtra = NULL ;
    this->MediaFileFormatContext =  NULL ;

    this->NewFileFlags.store(true) ;


    memset(&this->FrVideo ,0 ,sizeof(struct fr_buf_info));
    memset(&this->FrAudio ,0 ,sizeof(struct fr_buf_info));
    memset(&this->FrAudio ,0 ,sizeof(struct fr_buf_info));

    this->FirstTimeStamp = 0 ;
    this->LastTimeStamp  = 0 ;

    this->PBsfFilter =  NULL;
    av_register_all();
    this->SetName((char *)"rec_muxer");
    this->LastTimeStamp = 0;
    this->LastVideoTimeStamp = 0;
    this->StreamChannelNum = 3;
    this->PStreamChannel = NULL;
    this->VideoTimeScaleRate = 1;
    this->AudioIndex = 0;
    this->EnhanceThreadPriority = false;

    this->FirstTimeStamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastTimeStamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastVideoTimeStamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastAudioTimeStamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    //this->VideoFrameCounter = 0;
    //this->AudioFrameCounter = 0;
    this->StartTime = 0;
    this->NextStartTime = 0;
    this->NeedUpdateFrameVideo = false;
    this->NeedUpdateFrameAudio = false;
    this->NeedUpdateFrameExtra = false;

    //this->MediaFileOpenState.store(false);
    this->MediaFileDirectIO =  false;
    this->ValidMediaFile = false;

    this->MediaUdtaBuffer = NULL;
    this->MediaUdtaBufferSize = 0;
    this->MediaUdtaBufferReady.store(false);
    this->FragementedMediaSupport = false;

    this->SlowFastMotionRate.store(1);
    this->FastMotionByPlayer = false;
    this->VideoSkipFrameNum = 0;
    this->IsKeepFrame[0] = false;
    this->IsKeepFrame[1] = false;
    this->CheckIFrame[0] = false;
    this->CheckIFrame[1] = false;
    this->FirstStartTime = 0;
    this->StartDelayTime = 0;
    this->StartNewSeg.store(false);
    this->bRestoreBuf.store(false);
    memset(&this->NewSegInfo, 0, sizeof(NewSegInfo));
    memset(pu8UUIDData, 0, MAX_UUID_SIZE);

    this->stat.SetStatType(STAT_TYPE_RECORDE);

    IsVideoboxRun.store(true);
    ret = event_register_handler(EVENT_VIDEOBOX, 0, recorder_listener);
    if (ret < 0)
    {
        LOGGER_WARN("Register videobox handler error!\n");
    }
}
MediaMuxer::~MediaMuxer(){
    event_unregister_handler(EVENT_VIDEOBOX, recorder_listener);
    if(this->GetWorkingState() != ThreadingState::Stop){
        this->SetWorkingState(ThreadingState::Stop);
    }
    if(this->PStreamChannel){
        LOGGER_DBG("PStreamChannel free->%p\n",this->PStreamChannel);
        free(this->PStreamChannel);
    }

    if(this->MediaUdtaBuffer){
        free(this->MediaUdtaBuffer);
    }

}
bool MediaMuxer::SetFrFifo(QMediaFifo **v ,QMediaFifo *a ,QMediaFifo *e){
    LOGGER_TRC("frfifo ->v%p a%p e%p\n",v,a,e);
    if(this->MediaEnableVideo && v != NULL){
        this->PFrFifoVideo =  v ;
    }
    else {
        LOGGER_ERR("video fifo args is error ->%d %p\n",this->MediaEnableVideo ,v);
        return false ;
    }

    for(int i=0;i<this->VideoCount;i++){
        if(PFrFifoVideo[i] == NULL) {
            LOGGER_ERR("video fifo not is error ->index:%d en:%d pfifoAdr:%p\n",i ,this->MediaEnableVideo ,PFrFifoVideo[i]);
            return false;
        }
    }

    if(this->MediaEnableAudio ){

        if( a != NULL){
            this->PFrFifoAudio =  a ;
        }
        else {
            LOGGER_ERR("audio fifo not is error ->%d %p\n",this->MediaEnableAudio ,a);
            return false ;
        }
    }
    else {
        LOGGER_INFO("audio no enable\n");
    }
    if(this->MediaEnableExtra){

        if(e != NULL){
            this->PFrFifoExtra =  e ;
        }
        else {
            LOGGER_ERR("extra fifo not is error ->%d %p\n",this->MediaEnableExtra ,e);
            return false ;
        }
    }
    LOGGER_TRC("muxer info -> v%d a%d e%d\n",this->MediaEnableVideo,this->MediaEnableAudio,this->MediaEnableExtra);
    return true ;
}
void gen_extra_data(uint32_t sample_rate, uint32_t channel_configuration, uint8_t* dsi )
{
    uint32_t sampling_frequency_index = 0;
    switch (sample_rate) {
        case 96000:
            sampling_frequency_index = 0;
            break;
        case 88200:
            sampling_frequency_index = 1;
            break;
        case 64000:
            sampling_frequency_index = 2;
            break;
        case 48000:
            sampling_frequency_index = 3;
            break;
        case 44100:
            sampling_frequency_index = 4;
            break;
        case 32000:
            sampling_frequency_index = 5;
            break;
        case 24000:
            sampling_frequency_index = 6;
            break;
        case 22050:
            sampling_frequency_index = 7;
            break;
        case 16000:
            sampling_frequency_index = 8;
            break;
        case 12000:
            sampling_frequency_index = 9;
            break;
        case 11025:
            sampling_frequency_index = 10;
            break;
        case 8000:
            sampling_frequency_index = 11;
            break;
        case 7350:
            sampling_frequency_index = 12;
            break;
        default:
            LOGGER_ERR("not support target sampling rate ->%d\n", sample_rate);
            assert(1);
    }
    unsigned int object_type = 2; // AAC LC by default
    dsi[0] = (object_type<<3) | (sampling_frequency_index>>1);
    dsi[1] = ((sampling_frequency_index&1)<<7) | (channel_configuration<<3);
}

bool MediaMuxer::UpdateSegmentInfo()
{
    if (this->StartNewSeg.load())
    {
        this->FirstStartTime = 0;
        this->StartDelayTime = 0;
        this->StartNewSeg.store(false);
        if ((this->RecInfo.s32KeepFramesTime > 0) &&
            (this->NewSegInfo.s32KeepFramesTime >= 0))
        {
            this->bRestoreBuf.store(true);
        }
        if (strlen(this->NewSegInfo.dir_prefix) > 0)
        {
            memcpy(this->RecInfo.dir_prefix, this->NewSegInfo.dir_prefix, STRING_PARA_MAX_LEN);
            LOGGER_INFO("New prefix:%s\n", this->RecInfo.dir_prefix);
        }
        if (strlen(this->NewSegInfo.suffix) > 0)
        {
            memcpy(this->RecInfo.suffix, this->NewSegInfo.suffix, STRING_PARA_MAX_LEN);
            LOGGER_INFO("New suffix:%s\n", this->RecInfo.suffix);
        }
    }
}

bool MediaMuxer::StartNewMediaFile(bool firstFlags){
    int ret = 0;
    uint8_t aacExtradata[8];
    int aacExtradataLen = 0;
    int maxWidth =0,maxHeight = 0, maxResTrack = 0;
    int waitCnt = 0;

    if(this->MediaFileFormatContext ) {
        //old file exist ,just release it
        LOGGER_INFO("relase old file\n");
        this->ShowInfo(MEDIA_MUXER_INFO_FIFO  );
        if( this->InternalReleaseMuxerFile() == false ){
            LOGGER_ERR("stop media file error,but not exit the thread\n:");
        }
        LOGGER_INFO("relase old file ok\n");
        this->ShowInfo(MEDIA_MUXER_INFO_FIFO );
    }
    UpdateSegmentInfo();
    //prepare basic data
    LOGGER_INFO("do init stream info ->%d\n",this->GetWorkingState());
    //fflush(stdout);
    //usleep(1000);
    this->InitStreamChannel();
    LOGGER_INFO("do init frame info\n");
    memset(&this->FrVideo ,0 ,sizeof(struct fr_buf_info));
    memset(&this->FrAudio ,0 ,sizeof(struct fr_buf_info));
    memset(&this->FrExtra ,0 ,sizeof(struct fr_buf_info));

    if(this->SlowFastMotionRate.load() != 1){
        this->MediaEnableAudio = false;
        this->MediaEnableExtra = false;
    }

    LOGGER_TRC("create new media file\n");

    //this->MediaFileFormatContext = NULL ;
    /* allocate the output media context */
    if(this->RecInfo.keep_temp == 1) {
        this->GetTempFileName(true);
    }
    else {
        remove(this->CurrentMediatFileName);
        this->GetTempFileName(false);
    }
    this->MediaFileFormatContext =  NULL ;
#if 0
    avformat_alloc_output_context2(&this->MediaFileFormatContext, NULL, NULL, this->CurrentMediatFileName);
    if (!this->MediaFileFormatContext) {
        LOGGER_ERR("Could not deduce output format from file extension: using ->%s\n",this->RecInfo.av_format);
        avformat_alloc_output_context2(&this->MediaFileFormatContext, NULL, this->RecInfo.av_format, this->CurrentMediatFileName);
    }
    if (!this->MediaFileFormatContext) {
        this->MediaFileFormatContext = NULL ;
        return false;
    }
    this->MediaFileFormat = this->MediaFileFormatContext->oformat;
#else
    this->MediaFileFormat =  av_guess_format(0,this->CurrentMediatFileName,0);
    this->MediaFileFormatContext =  avformat_alloc_context();
    this->MediaFileFormatContext->oformat =  this->MediaFileFormat ;
    strcpy(this->MediaFileFormatContext->filename,(const char *)this->CurrentMediatFileName);

#endif
//    this->MediaFileFormat->flags |=  AVFMT_NOTIMESTAMPS;
//    this->MediaFileFormat->flags |=  AVFMT_TS_NONSTRICT;
    //this->MediaFileFormat->flags |=  AVFMT_FLAG_BITEXACT;
    /* find the encoder */
    //this->MediaFileFormatContext->oformat->flags !=  AVFMT_NOTIMESTAMPS;
    //this->MediaFileFormatContext->iformat->flags !=  AVFMT_NOTIMESTAMPS;
    AVCodecContext *codecContext = NULL;
    if (this->MediaEnableVideo) {
        for (int index=0;index<this->VideoCount;index++) {
            VideoStream = avformat_new_stream(this->MediaFileFormatContext, NULL);
            if  (this->RecInfo.fixed_fps == 0)
            {
                if (this->SlowFastMotionRate.load() < 0)
                {
                    printf("video fps:%d\n", video_get_fps(this->RecInfo.video_channel));
                    this->PVideoInfo[index]->SetFps(video_get_fps(this->RecInfo.video_channel));
                }
                else
                {
                    while(this->PVideoInfo[index]->GetUpdate() == false )
                    {
                        LOGGER_WARN("wait for fps data index:%d\n",index);
                        usleep(20000);
                        waitCnt ++;
                        if (waitCnt >= MAX_RETRY_CNT)
                        {
                            LOGGER_ERR("wait timeout index:%d!\n", index);
                            return false;
                        }
                    }
                }
            }
            switch(this->PVideoInfo[index]->GetType()){
                case VIDEO_TYPE_MJPEG :
                    this->VideoCodecID =  AV_CODEC_ID_MJPEG;
                    break ;
                case VIDEO_TYPE_H264 :
                    this->VideoCodecID =  AV_CODEC_ID_H264;
                    break ;
                case VIDEO_TYPE_HEVC :
                    this->VideoCodecID =  AV_CODEC_ID_HEVC;
                    break ;
                default:
                    return false;
            }
            codecContext= VideoStream->codec ;
            codecContext->width =  this->PVideoInfo[index]->GetWidth();
            codecContext->height = this->PVideoInfo[index]->GetHeight();
            codecContext->codec_type =  AVMEDIA_TYPE_VIDEO ;
            codecContext->codec_id = this->VideoCodecID ;
            codecContext->bit_rate = this->PVideoInfo[index]->GetBitRate();
            codecContext->time_base.num = 1;
            VideoStream->time_base.num = 1 ;
            codecContext->gop_size = this->PVideoInfo[index]->GetGopLength();
            codecContext->pix_fmt =  AV_PIX_FMT_YUV420P ;
            this->Fps =  this->PVideoInfo[index]->GetFps();
            VideoStream->time_base.den = this->Fps ;
            codecContext->time_base.den =  this->Fps;
            if(codecContext->codec_id  == AV_CODEC_ID_H264  || codecContext->codec_id == AV_CODEC_ID_HEVC){
                codecContext->max_b_frames = 2;
            }

            if(codecContext->width > maxWidth &&  codecContext->height > maxHeight)
            {
                maxWidth = codecContext->width;
                maxHeight = codecContext->height;
                maxResTrack = index;
            }

            int headerSize = this->PVideoInfo[index]->GetHeaderSize();
            char headerBuf[128] = {0};
            if(this->PVideoInfo[index]->GetHeader(headerBuf, &headerSize) == NULL){
                LOGGER_ERR("fill video header error\n");
            }
#if 1
            printf("\norigin stream header -> %d\n",headerSize);
            for(int i = 0 ;i < headerSize ;i++){
                if(i%10 == 0  ){
                    printf("\n\t"); //save it for more debug ,do not -1
                }
                printf("%3X", headerBuf[i]);
            }
            printf("\n\theader over\n");
#endif
            if(this->VideoCodecID == AV_CODEC_ID_H264){
                if(this->UpdateSpsFrameRate((uint8_t *)headerBuf, &headerSize, this->Fps * 2, 1) ==false){
                        LOGGER_ERR("update h264 sps error\n");
                        return false;
                }
#if 1
                printf("\nnew stream header -> %d\n",headerSize);
                for(int i = 0 ;i < headerSize ;i++){
                    if(i%10 == 0  ){
                        printf("\n\t"); //save it for more debug ,do not -1
                    }
                    printf("%3X", headerBuf[i]);
                }
                printf("\n\theader over\n");
#endif
            }
            VideoStream->codec->extradata = (uint8_t *)av_mallocz(headerSize + FF_INPUT_BUFFER_PADDING_SIZE);
            memcpy(this->VideoStream->codec->extradata, headerBuf, headerSize);
            VideoStream->codec->extradata_size =  headerSize;

            if (this->MediaFileFormat->flags & AVFMT_GLOBALHEADER ){
                if(this->MediaEnableVideo)
                    VideoStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER ;
            }
            //save_tmp_file(1, VideoStream->codec->extradata, headerSize, (char *)"_header");
            LOGGER_DBG("time base ->%d :%d\n",this->VideoStream->time_base.num, this->VideoStream->time_base.den);
        }

        this->MediaFileFormatContext->private_control_flag = 0;
        for (int index=0;index<this->VideoCount;index++)
        {
            //it will change fourcc "trak" to "infv" if set
            if(this->VideoCount > 1 && index != maxResTrack)
            {
                this->MediaFileFormatContext->private_control_flag += (1<<index);
            }
        }
    }

    if (this->MediaEnableAudio) {
        AudioStream = avformat_new_stream(this->MediaFileFormatContext, NULL);
        AudioIndex = AudioStream->index;
        codecContext= AudioStream->codec ;
        codecContext->codec_type =  AVMEDIA_TYPE_AUDIO ;
        codecContext->codec_id = this->AudioCodecID ;
        switch ( this->AudioCodecID ){
            case AV_CODEC_ID_PCM_MULAW :
                LOGGER_DBG("muxer audio mulaw\n");
                codecContext->sample_fmt =  AV_SAMPLE_FMT_S16 ;
                codecContext->sample_rate= this->RecInfo.audio_format.sample_rate;
                codecContext->channels = this->RecInfo.audio_format.channels ;
                codecContext->bit_rate  = this->RecInfo.audio_format.channels * this->RecInfo.audio_format.sample_rate \
                                        * GET_BITWIDTH(this->RecInfo.audio_format.channels);
                codecContext->bit_rate /= 2;
                AudioStream->time_base.den = this->RecInfo.audio_format.sample_rate ;
                AudioStream->time_base.num = 1 ;
                break;
            case AV_CODEC_ID_PCM_ALAW :
                LOGGER_DBG("muxer audio alaw\n");
                codecContext->sample_fmt =  AV_SAMPLE_FMT_S16 ;
                codecContext->sample_rate= this->RecInfo.audio_format.sample_rate;
                codecContext->channels = this->RecInfo.audio_format.channels ;
                codecContext->bit_rate  = this->RecInfo.audio_format.channels * this->RecInfo.audio_format.sample_rate \
                                        * GET_BITWIDTH(this->RecInfo.audio_format.channels);
                codecContext->bit_rate /= 2;
                AudioStream->time_base.den = this->RecInfo.audio_format.sample_rate ;
                AudioStream->time_base.num = 1 ;
                break;
            case AV_CODEC_ID_AAC :
                codecContext->sample_fmt =  AV_SAMPLE_FMT_S16 ;
                codecContext->sample_rate= this->RecInfo.audio_format.sample_rate;
                codecContext->channels = av_get_default_channel_layout(this->RecInfo.audio_format.channels);
                codecContext->channels = this->RecInfo.audio_format.channels;
                codecContext->bit_rate  = this->RecInfo.audio_format.channels * this->RecInfo.audio_format.sample_rate \
                                        * 16;
                //* GET_BITWIDTH(this->RecInfo.audio_format.sample_fmt);
                codecContext->bit_rate /= 4;
                AudioStream->time_base.den = this->RecInfo.audio_format.sample_rate;
                AudioStream->time_base.num = 1;
                //codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
                LOGGER_INFO("audio channel ->%d\n",codecContext->channels);
                LOGGER_INFO("audio sample_rate ->%d\n",codecContext->sample_rate);

                if(this->PBsfFilter != NULL){
                    av_bitstream_filter_close(this->PBsfFilter);
                }
                this->PBsfFilter =  av_bitstream_filter_init("aac_adtstoasc");
                if(this->PBsfFilter == NULL){
                    LOGGER_ERR("init aac bsf filter error\n");
                    return false ;
                }
                #if 1
                gen_extra_data(codecContext->sample_rate, codecContext->channels, aacExtradata);
                aacExtradataLen = 2;
                printf("new extra data ->%x %x\n", aacExtradata[0], aacExtradata[1]);
                //if(aacExtradataLen != 0 && strstr((const char *)this->RecInfo.av_format, "mkv") != NULL){
                LOGGER_INFO("add extra data for mkv\n");
                codecContext->extradata_size =  aacExtradataLen;
                codecContext->extradata = (uint8_t *)av_mallocz(codecContext->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                memcpy(codecContext->extradata, aacExtradata, codecContext->extradata_size);
                //                }
                #endif
                LOGGER_DBG("muxer use bsf filer -> aac_adtstoasc ->%p\n",this->PBsfFilter);
                break;
            case AV_CODEC_ID_PCM_S16LE    :
                LOGGER_DBG("muxer audio S16E\n");
                codecContext->sample_fmt =  AV_SAMPLE_FMT_S16 ;
                codecContext->sample_rate= this->RecInfo.audio_format.sample_rate;
                codecContext->channels = this->RecInfo.audio_format.channels ;
                codecContext->bit_rate  = this->RecInfo.audio_format.channels * this->RecInfo.audio_format.sample_rate \
                                        * GET_BITWIDTH(this->RecInfo.audio_format.channels);
                AudioStream->time_base.den = this->RecInfo.audio_format.sample_rate ;
                AudioStream->time_base.num = 1 ;
                break;
            case AV_CODEC_ID_ADPCM_G726    :
                LOGGER_DBG("muxer audio g726\n");
                break;
            default :
                LOGGER_ERR("not support current audio codec %d in muxer %d\n",\
                        this->AudioCodecID,this->RecInfo.av_format);
                //need to release memory
                return false ;


        }


        //codecContext->profile = FF_PROFILE_AAC_HE;
#if 0
        codecContext->time_base.den =  this->PVideoInfo.fps;
        codecContext->time_base.num = 1;
#endif
        //    this->AudioStream->id =  this->MediaFileFormatContext->nb_streams-1;
    }
    if(this->MediaEnableExtra){
        ExtraStream = avformat_new_stream(this->MediaFileFormatContext, NULL);
        ExtraStreamIndex = ExtraStream->index;
        codecContext= ExtraStream->codec ;
        codecContext->sample_rate= 8000 ;
        codecContext->bit_rate  =  44100;
        codecContext->channels = 2 ;
        codecContext->codec_type =  AVMEDIA_TYPE_AUDIO ;
        codecContext->codec_id = this->AudioCodecID ;
        //    this->ExtraStream->id =  this->MediaFileFormatContext->nb_streams-1;
    }

#if 0
    for (uint32_t i = 0; i <this->MediaFileFormatContext->nb_streams; i++) {
        AVStream *out_stream = this->MediaFileFormatContext->streams[i];
        printf("..............>%d %d\n",out_stream->index , out_stream->id);
    }
#endif
    if (this->MediaFileFormat->flags & AVFMT_GLOBALHEADER ){
        //printf("muxer use global header\n");
        if(this->MediaEnableVideo)
            VideoStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER ;
        if(this->MediaEnableAudio)
            AudioStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER ;
    }
    else {
        printf("muxer not use global header\n");

    }
    //printf("output format flags ->%X\n",this->MediaFileFormat->flags);

    av_dump_format(this->MediaFileFormatContext, 0, this->CurrentMediatFileName, 1);
    //sprintf(this->MediaFileFormatContext->filename,"%s",(const char *)this->CurrentMediatFileName);
    //this->UpdateMediaUdta
    if (!(this->MediaFileFormat->flags & AVFMT_NOFILE)) {
        int ioFlags = 0;
        if(this->MediaFileDirectIO){
            ioFlags = AVIO_FLAG_WRITE|AVIO_FLAG_DIRECT;
        }
        else {
            ioFlags = AVIO_FLAG_WRITE;
        }
        if (avio_open(&this->MediaFileFormatContext->pb, this->CurrentMediatFileName, ioFlags) < 0) {
            //use avio_open2 to open file and add atom moov and IO error callback
            LOGGER_ERR( "Could not open output file ->%s<-\n", this->CurrentMediatFileName);
            this->MediaFileFormatContext->pb = NULL;
            return false ;
        }
        this->UpdateDirectIOFlags();
        this->MediaFileFormatContext->flags &= ~AVFMT_FLAG_FLUSH_PACKETS;
        //this->MediaFileOpenState.store(true);
    }

    if(this->MediaEnableAudio) {
        //try add global header
    }

    if(this->MediaFileFormatContext != NULL ){
        AVDictionary *aVOpts = NULL;
        if(this->FragementedMediaSupport == true ) {
            //av_dict_set(&AVOpts, "movflags","empty_moov+faststart", 0);
            //av_dict_set(&AVOpts, "movflags","frag_keyframe+empty_moov", 0);
            av_dict_set(&aVOpts, "movflags","empty_moov+faststart+separate_moof", 0);
            av_dict_set(&aVOpts, "use_edit_list", "0", 0);
            av_dict_set(&aVOpts, "min_frag_duration", "1", 0);
            av_dict_set(&aVOpts, "frag_duration", "10000", 0);
        }
        if (strlen(this->pu8UUIDData) > 0)
        {
            av_dict_set(&aVOpts, "uuid_data", this->pu8UUIDData, 0);
        }
        int timescale =  1000;
        this->VideoTimeScaleRate = this->SlowFastMotionRate.load();
#if MEDIA_MUXER_SLOW_FAST_GLOBAL == 1
        LOGGER_INFO("use global timescale to control motion mode\n");
        int rate = this->VideoTimeScaleRate;
        if(rate > 1){
            timescale /= rate;
            LOGGER_INFO("fast timescale info ->%d %d\n", rate, timescale);
        }
        else if(rate < -1){
            rate = 0 - rate ;
            timescale *= rate;
            LOGGER_INFO("slow timescale info ->%d %d\n", rate, timescale);
        }
        else if(rate == 1){
            //keep old timestamp
        }
        else{
            LOGGER_WARN("unsupprt lapse info \n");
        }
#endif
        char buf[12] ={0};
        sprintf(buf, "%d", timescale);
        //if(rate != 1){
            av_dict_set(&aVOpts, "video_track_timescale", buf, 0);
        //}
        this->MediaUdtaMutexLock.lock();
        this->UpdateMediaFileContextUdta();
 
        ret = avformat_write_header(this->MediaFileFormatContext, &aVOpts);
        this->MediaUdtaMutexLock.unlock();
        if(this->FragementedMediaSupport || aVOpts != NULL){
            av_dict_free(&aVOpts);
        }
        if (ret < 0) {
            printf( "Error occurred when writing header -> %d\n",ret);
            return false ;
        }
        else{
            //update file write method
            //this->UpdateDirectIOFlags();
        }
    }
    else {
        LOGGER_ERR("!!!! media file has been deinit\n");
        return false;
    }

    this->NewFileFlags.store(true) ;
    return true ;
}
bool MediaMuxer::StopMediaFile(){
    int ret = 0 ;
    if(this->MediaFileFormatContext == NULL){
        return true ;
    }
    if(access(this->CurrentMediatFileName,W_OK) == 0
    && this->MediaFileFormatContext != NULL){
        //just reduce the crash rate ,not all
        LOGGER_DBG("write media file trailer\n");
        ret = av_write_trailer(this->MediaFileFormatContext);
        if (ret <  0){
            LOGGER_ERR("write media file trailer error, ret:%d.\n", ret);
            //        return false ;
        }
        LOGGER_INFO("write media file trailer ok \n");
        this->SyncStorage();
    }
    else {
        LOGGER_WARN("file not exist,no write trailer\n");
    }
    LOGGER_INFO("try close ->%p\n",this->MediaFileFormatContext->pb);
    if (!(this->MediaFileFormat->flags & AVFMT_NOFILE) &&
            this->MediaFileFormatContext->pb != NULL &&
        (&this->MediaFileFormatContext->pb != NULL )   ) {
        avio_flush(this->MediaFileFormatContext->pb);
        LOGGER_INFO("avio flush ok  ->%p %p\n",this->MediaFileFormatContext->pb,
                &this->MediaFileFormatContext->pb);
        ret = avio_closep(&this->MediaFileFormatContext->pb); //close Last file
        if(ret < 0 ){
            LOGGER_ERR("avio close error ->%d\n", ret);
            //            return false ;
        }
        else {
            LOGGER_INFO("avio close ok\n");
        }
    //    this->MediaFileFormatContext->pb = NULL; //can not set manually,which will lead unexpected error
    }
    LOGGER_TRC("free avformat context\n");
    if(this->MediaFileFormatContext != NULL){
        avformat_free_context(this->MediaFileFormatContext);
    }
    LOGGER_TRC("free avformat context ok\n");
    this->MediaFileFormatContext =  NULL ;
    //this->MediaFileOpenState.store(false);
    if (this->EnhanceThreadPriority)
    {
        this->EnhancePriority(false);
        this->EnhanceThreadPriority = false;
    }
    return true ;
}

bool MediaMuxer::GetTimeString(char *suffix, char *buf, uint32_t len, time_t *ref, bool utc){
    struct tm *t_tm ;
    if(ref == NULL ){
        time_t now ;
        time(&now);
        ref = &now ;
        t_tm= utc ? gmtime(&now) : localtime(&now);
    }
    else {
        //t_tm= localtime(ref);
        t_tm= utc ? gmtime(ref) : localtime(ref);
    }
    size_t bufLen = 0 ;
    char *p = suffix ;
    char temp[64];
    for(size_t i = 0;i < strlen(suffix) ;i++) {
        if(p[i] == '%' ) {
            //    printf("try handle->%c %c \n",p[i],p[i+1]);
            if(  i >= (len-1) ){
                printf("wrong suffix format->%s",p);
                return false ;
            }
            else {
                i++;
                memset(temp,0,64);
                switch(p[i]){
                    case 'Y' :
                        sprintf(temp,"%4d",t_tm->tm_year+1900);
                        break;
                    case 'y' :
                        sprintf(temp,"%2d",(t_tm->tm_year+1900)%100);
                        break;
                    case 'm' :
                        sprintf(temp,"%2d",t_tm->tm_mon+1);
                        break;
                    case 'd' :
                        sprintf(temp,"%2d",t_tm->tm_mday);
                        break;
                    case 'H' :
                        sprintf(temp,"%2d",t_tm->tm_hour);
                        break;
                    case 'M' :
                        sprintf(temp,"%2d",t_tm->tm_min);
                        break;
                    case 'S' :
                        sprintf(temp,"%2d",t_tm->tm_sec);
                        break;
                    case 's' :
                        sprintf(temp,"%10ld",*ref);
                        break;
                    default :
                        printf("%s:%d suffix format ->%c\n",__FILE__,__LINE__,p[i]);
                        return false ;
                }
                for(int  j = 0;temp[j]!=0 ;j++){
                    if(temp[j] == ' '){
                        temp[j] = '0';
                    }
                }
                if (strlen(buf)+ strlen(temp)  >= len ){
                    printf("->%s:%d target buf is too short for time format \n",__FILE__,__LINE__ );
                    return false ;
                }
                strcat(buf,temp);
            }

        }
        else {
            //just copy
            bufLen = strlen(buf);
            if ((bufLen+ 1)  >= len ){
                printf("->%s:%d target buf is too short for time format \n",__FILE__,__LINE__ );
                return false ;
            }
            buf[bufLen] = p[i];
            bufLen++;
        }
    }
//    this->TimeSuffixLen = strlen(buf);
    return true ;
}

bool MediaMuxer::GetTempFileName(bool keepTemp){
    size_t len = 0 ;
    int fileNameLength = strlen(this->RecInfo.dir_prefix);
    if(fileNameLength >= VPLAY_PATH_MAX){
        printf("file name exceed max lenght\n");
        return false;
    }
    memset(this->CurrentMediatFileName,0, VPLAY_PATH_MAX);
    strcat(this->CurrentMediatFileName,this->RecInfo.dir_prefix);
    if(this->CurrentMediatFileName[fileNameLength-1] != '/' ){
        this->CurrentMediatFileName[fileNameLength] = '/';
        fileNameLength++;
        this->CurrentMediatFileName[fileNameLength] = 0;
    }
    this->CurrentMediaFileIndex = this->TryGetLocalIndex(this->CurrentMediatFileName);

    memset(this->CurrentMediatFileName,0, VPLAY_PATH_MAX);
    if(keepTemp == false){
        strcat(this->CurrentMediatFileName,this->RecInfo.dir_prefix);
        if(this->CurrentMediatFileName[fileNameLength-1] != '/' ){
            this->CurrentMediatFileName[fileNameLength] = '/';
            fileNameLength++;
            this->CurrentMediatFileName[fileNameLength] = 0;
        }
        strcat(this->CurrentMediatFileName,RECORDER_TEMP_FILE);
        strcat(this->CurrentMediatFileName,".");
        strcat(this->CurrentMediatFileName, this->RecInfo.av_format);
    }
    else {
        //get start temp file name .end time set 0
        this->GetFinalFileName(this->CurrentMediatFileName, VPLAY_PATH_MAX, true);
    }

    LOGGER_TRC("get file name -> %s %4d %d\n",this->CurrentMediatFileName,this->CurrentMediaFileIndex,strlen(this->CurrentMediatFileName));
    return true ;
}
bool MediaMuxer::GetFinalFileName(char *buf, int bufSize, bool tempFlag){//tempFlag , get temp file name ,te,ue fill 0, ts,us current time
    assert(buf != NULL);
    char temp[STRING_PARA_MAX_LEN];
    char *fileNameTmp =  buf;
    int fileNameLength;
    int len;
    int seg,temp_seg;
    time_t now ;

    fileNameLength = strlen(this->RecInfo.dir_prefix);
    if(fileNameLength >= bufSize){
        LOGGER_ERR("file name(%d %d) exceed max lenght\n", fileNameLength, bufSize);
        return false;
    }

    memset(fileNameTmp, 0, bufSize);
    strcat(fileNameTmp,this->RecInfo.dir_prefix);
    if(fileNameTmp[fileNameLength-1] != '/' ){
            fileNameTmp[fileNameLength] = '/';
            fileNameLength++;
            fileNameTmp[fileNameLength] = 0;

    }
    len= strlen(this->RecInfo.suffix);
    if(len == 0 ){
        strcat(fileNameTmp,"demo");
    }
    else {
        unsigned char *p =  (unsigned char *)this->RecInfo.suffix ;
        len = strlen((char *)p);
        for(size_t i =0; i< len; i++){
            if(p[i] == '%' ) {
            //    printf("try handle->%c %c \n",p[i],p[i+1]);
                if(  i >= (len-1) ){
                    LOGGER_ERR("wrong suffix format->%s",p);
                    return false ;
                }
                else {
                    i++;
                    memset(temp,0,STRING_PARA_MAX_LEN);
                    switch(p[i]){
                        case 'c' :
                            sprintf(temp,"%5d",this->CurrentMediaFileIndex);
                            for(int j = 0;temp[j]!=0 ;j++){
                                if(temp[j] == ' '){
                                    temp[j] = '0';
                                }
                            }
                            break;
                        case 'l' :
                            //add 500ms, Math.Round for seg
                            memset(temp, 0, STRING_PARA_MAX_LEN);
                            temp_seg = (this->LastTimeStamp + 500 - this->FirstTimeStamp)/1000;
                            seg = this->GetMotionPlayTime(temp_seg);
                            sprintf(temp, "%4d", seg);
                            if(tempFlag == true){
                                memset(temp,'0',strlen(temp));
                            }
                            break;
                        case 't' :
                            i++ ;
                            if(p[i] == 'e' ){
                                //add end time
                            //    time(&now);
                                if ( this->GetTimeString(this->RecInfo.time_format, temp,
                                            STRING_PARA_MAX_LEN,NULL, false) == false ){
                                    return false ;
                                }
                                if(tempFlag == true){
                                    memset(temp,'0',strlen(temp));
                                }
                            //    this->TimeSuffixLen = strlen(temp);
                            }
                            else if(p[i] == 's' ){
                                //add start time
                                time(&now);
                                if(tempFlag == true){
                                    LOGGER_DBG("get temp file name ,just current time\n");
                                }
                                else {
                                    now =  this->StartTime;
                                }

                                if ( this->GetTimeString(this->RecInfo.time_format, temp,
                                            STRING_PARA_MAX_LEN, &now, false) == false ){
                                    return false ;
                                }
                            //    this->TimeSuffixLen = strlen(temp);
                            }
                            else {
                                printf("->%s : %d format error\n",__FILE__,__LINE__);
                                return false ;
                            }
                            break;
                        case 'u' :
                            i++ ;
                        //    this->TimeSuffixLen = strlen(temp);
                            if(p[i] == 'e' ){
                                //add end time
                                if ( this->GetTimeString(this->RecInfo.time_format, temp,
                                            STRING_PARA_MAX_LEN, NULL, true) == false ){
                                    return false ;
                                }
                                if(tempFlag == true){
                                    memset(temp,'0',strlen(temp));
                                }
                            }
                            else if(p[i] == 's' ){
                                //add start time
                                time(&now);
                                if(tempFlag == true){
                                    LOGGER_DBG("get temp file name ,just current time us\n");
                                }
                                else {
                                    now =  this->StartTime;
                                }

                                if ( this->GetTimeString(this->RecInfo.time_format, temp,
                                            STRING_PARA_MAX_LEN, &now, true) == false ){
                                    return false ;
                                }
                            }
                            else {
                                printf("->%s : %d format error\n",__FILE__,__LINE__);
                                return false ;
                            }
                            break;
                        default :
                            printf("suffix error ->%c\n",p[i]);
                            return false ;
                    }
                    if (strlen(fileNameTmp)+ strlen(temp)  >= bufSize ){
                        printf("->%s:%d target buf is too short for time format \n",__FILE__,__LINE__ );
                        return false ;
                    }
                    for(int i = 0; i < strlen(temp); i++){
                        if(temp[i] == ' '){
                            temp[i] = '0';
                        }
                    }
                    strcat(fileNameTmp,temp);
                }
            }
            else {
                //just copy
                if (strlen(fileNameTmp)+ 1  >= bufSize ){
                    printf("->%s:%d target buf is too short for time format \n",__FILE__,__LINE__ );
                    return false ;
                }
                fileNameLength = strlen(fileNameTmp);
                fileNameTmp[fileNameLength] = p[i];
                fileNameLength++;
            }
        }
    }
    /*
     *
    memset(temp , 0, STRING_PARA_MAX_LEN);
    sprintf(temp , "_%lld", (this->LastTimeStamp - this->FirstTimeStamp) / 1000);
    strcat(fileNameTmp, temp);

     */
    len = strlen(fileNameTmp);
    fileNameTmp[len] = '.';
    strcat(fileNameTmp,this->RecInfo.av_format);
    len = strlen(fileNameTmp);
    LOGGER_TRC("Final file name -> %s %d\n",fileNameTmp,len);

    return true ;
}
bool MediaMuxer::UpdateMediaFile(void) {
    char newFileName[VPLAY_PATH_MAX];
    memset(newFileName, 0, VPLAY_PATH_MAX);

    if(this->GetFinalFileName(newFileName, VPLAY_PATH_MAX, false) == false) {
        LOGGER_ERR("Get target file name error,abort\n");
        return false ;
    }
    LOGGER_INFO("ren fr->%s\n",this->CurrentMediatFileName);
    LOGGER_INFO("ren to->%s\n",newFileName);
    if(rename(this->CurrentMediatFileName,newFileName) != 0  ){
        LOGGER_ERR("Temp file -> %s\n",this->CurrentMediatFileName);
        LOGGER_ERR("Rename error-> %s\n",newFileName);
        return false ;
    }
    else
    {
        LOGGER_INFO("Rename ok->%s\n",newFileName);
        chmod(newFileName, 0666);
        if(this->ValidMediaFile == true){
            this->ValidMediaFile =  false;
            if ( vplay_report_event (VPLAY_EVENT_NEWFILE_FINISH ,newFileName)  <  0){
                LOGGER_ERR("event send error\n");
                return false ;
            }

        }
        else {
            this->ValidMediaFile =  false;
            if(this->SlowFastMotionRate.load() == 1){
                LOGGER_INFO("current file is invalid ,just delete->%s\n",newFileName);
                remove(newFileName);
            }
        }
        return true;
    }
    return true ;
}
int MediaMuxer::TryGetLocalIndex(char *base){
    char buf[VPLAY_PATH_MAX] ;
    int index  = 1;
    memset(buf ,0,VPLAY_PATH_MAX);
    sprintf(buf,"%s/.flags",base);
    int fd =  open(buf,O_RDWR|O_CREAT);
    if (fd < 0 ){
        LOGGER_ERR("can not open flags files ->%s\n", buf);
    }
    else {
        int size = read(fd ,buf,64);
        if (size <= 0 ){
            LOGGER_ERR("not find \n");
            index = 0 ;
        }
        else {
            index = atoi(buf);
        }
        lseek(fd, 0L, SEEK_SET);
        memset(buf ,0,VPLAY_PATH_MAX);
        sprintf(buf,"%6d",index+1);
        size = write(fd , buf,6 );
        if (size <= 0 ){
            LOGGER_ERR("write flags file error\n");
        }
        close(fd);
    }
    return index ;
}
/* /
 *Get the next fr buf for write
 * */
bool MediaMuxer::AddVideoFrFrame(struct fr_buf_info *frame, int stream_index){
    int ret = 0;
    assert(frame != NULL );
    AVPacket pkt;
    static int i = 0 ;
    i++;
    //LOGGER_INFO("AddVideoFrFrame frame->timestamp:%lld, this->FirstTimeStamp:%lld deta:%lld\n",frame->timestamp, this->FirstTimeStamp, frame->timestamp - this->FirstTimeStamp);
    av_init_packet(&pkt);
    if (frame->priv ==  VIDEO_FRAME_I )
    {
        pkt.flags |= AV_PKT_FLAG_KEY;
    }
    else if  ((frame->priv == VIDEO_FRAME_P) || (frame->priv == VIDEO_FRAME_HEADER))
    {
    }
    else
    {
        LOGGER_WARN("video frame type error++++ ->%6d  %6d??????????????? priv:%d\n",i,frame->size,frame->priv);
    }

    if(frame->priv == VIDEO_FRAME_I){
        save_tmp_file(3, frame->virt_addr, frame->size, (char *)"_I");
    }
    else if(frame->priv == VIDEO_FRAME_P){
        save_tmp_file(3, frame->virt_addr, frame->size, (char *)"_P");
    }
    else{
        LOGGER_WARN("unsupport frame type->%3X\n",frame->priv);
    }
    pkt.stream_index = stream_index;
    pkt.data = (uint8_t *)frame->virt_addr;
    pkt.size = frame->size;

    uint64_t timestamp = frame->timestamp - this->FirstTimeStamp ;
    //LOGGER_INFO("Previous timestamp:%lld VideoTimeScaleRate:%d\n",timestamp, VideoTimeScaleRate);

#if MEDIA_MUXER_SLOW_FAST_GLOBAL == 0
    int rate = this->VideoTimeScaleRate;
    if(rate > 1){
        timestamp *= (uint64_t)rate;
    }
    else if(rate < -1){
        rate = 0 - rate ;
        timestamp /= (uint64_t)rate;
    }
    else if(rate == 1){
        //keep old timestamp
    }
    else{
        LOGGER_WARN("unsupprt lapse info \n");
    }
#endif
        //LOGGER_INFO("After timestamp:%lld VideoTimeScaleRate:%d\n",timestamp, VideoTimeScaleRate);
#if 0
    pkt.duration = 0;
    pkt.pos = -1;
    pkt.pts = pkt.dts = av_rescale_q(timestamp, (AVRational){1, 1000}, this->VideoStream->time_base);
#else
    pkt.duration = 0;
    //pkt.duration = 1000/this->Fps;
    pkt.pos = -1;
    pkt.pts = pkt.dts =  timestamp;
    //LOGGER_INFO("===========>AddVideoFrFrame.ts:%lld\n",timestamp);
#endif

    pkt.pts = pkt.dts = FrameDtsAutoReCal(pkt.stream_index, frame->timestamp, pkt.dts);
#if 0
    printf("video dts  ->%8lld %5lld %5lld :%5d :%5d %d\n",
                timestamp,
                pkt.pts,
                pkt.dts,
                pkt.size,pkt.duration);
#endif
    if(this->FragementedMediaSupport == true && frame->priv == VIDEO_FRAME_I  && this->ValidMediaFile==true){
        ret = 0;
        //  ret = av_write_frame(this->MediaFileFormatContext, NULL);
        if(ret < 0){
            LOGGER_ERR("+++add moov moof data error\n");
        }
        //LOGGER_DBG("++++++++++++force add moov data ->%d\n", ret);
    }
    this->stat.UpdatePacketInfo(PACKET_TYPE_VIDEO, pkt.size, pkt.pts, stream_index);
    ret = av_write_frame(this->MediaFileFormatContext, &pkt);
    //LOGGER_INFO("===========> FrameDtsAutoReCal AddVideoFrFrame.ts:%d\n",timestamp);
    if (ret <  0  ){
        LOGGER_ERR("write video frame error %d\n",ret );
        return false ;
    }
    this->CheckMediaSegment(frame,0);
    this->ValidMediaFile = true;
    return true ;
}

bool MediaMuxer::AddAudioFrFrame(struct fr_buf_info *frame ){
    assert(frame != NULL );
    AVPacket pkt;
    static int i = 0 ;
    i++;
    av_init_packet(&pkt);

    pkt.stream_index = AudioIndex;

    pkt.data = (uint8_t *)frame->virt_addr;
    pkt.size = frame->size;
    /*  write the compressed frame in the media file */
    int ret = 0 ;
    if(this->PBsfFilter != NULL){
        av_bitstream_filter_filter(this->PBsfFilter, this->AudioStream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
    }

    uint64_t timestamp = frame->timestamp - this->FirstTimeStamp  ;
#if 1
    pkt.pts = pkt.dts = av_rescale_q(timestamp, (AVRational){1, 1000}, this->AudioStream->time_base);
#else
    pkt.pts = pkt.dts = timestamp;
#endif
    pkt.duration = 0;
    pkt.pts = pkt.dts = FrameDtsAutoReCal(pkt.stream_index, frame->timestamp, pkt.dts);

    //printf("audio dts  ->%8lld %5lld %5lld :%5d :%5d\n", timestamp, pkt.pts,pkt.dts,pkt.size,pkt.duration);
    int64_t audioPTS = 0;
    audioPTS = av_rescale_q(pkt.pts, this->AudioStream->time_base, (AVRational){1, 1000});
    this->stat.UpdatePacketInfo(PACKET_TYPE_AUDIO, pkt.size, audioPTS, 0);
    ret = av_write_frame(this->MediaFileFormatContext, &pkt);
    if (ret <  0  ){
        LOGGER_ERR("write audio frame error %d\n",ret );
        return false ;
    }

    this->CheckMediaSegment(frame,1);
    return true ;
}
bool MediaMuxer::AddExtraFrFrame(struct fr_buf_info *frame ){
    this->CheckMediaSegment(frame,2);
    return true ;
}
int MediaMuxer::ChooseStream(struct fr_buf_info *frv ,struct fr_buf_info *fra ,struct fr_buf_info *fre){
    int ret =  -1;
    struct fr_buf_info *target = NULL;
    if(this->MediaEnableVideo){
        target  = frv ;
        ret = 0;
    }
    if(this->MediaEnableAudio) {
        if(target != NULL){
            if(target->timestamp > fra->timestamp ){
                target = fra ;
                ret = 1;
            }
        }
    }
    if(this->MediaEnableExtra ) {
        if (target != NULL ){
            if(target->timestamp > fre->timestamp ){
                target = fre ;
                ret = 2 ;
            }
        }
    }
#if 1
//        printf("choose stream -> %6lld %lld -> %d\n",frv->timestamp,fra->timestamp,ret);
#endif
    return ret;
}
bool MediaMuxer::CheckMediaSegment(struct fr_buf_info *frame,int type){
    static int index  = 0 ;
    static int ia = 0;
    static int iv = 0;
    static int ie = 0;
    int seg = 0;
    int temp_seg = 0;
    if(ia >= MUXER_DISPLAT_INTERVAL ){
        ia = 0;
        seg =  ( frame->timestamp - this->FirstTimeStamp)  /1000  ;
        LOGGER_INFO("time stamp a -> %4d\n",seg);
    }
    if(iv >= MUXER_DISPLAT_INTERVAL ){
        iv = 0;
        temp_seg =  ( frame->timestamp - this->FirstTimeStamp)  /1000  ;
        seg = this->GetMotionPlayTime(temp_seg);
        LOGGER_INFO("time stamp v -> %4d  %4d\n",seg,temp_seg);
    }
    if(ia >= MUXER_DISPLAT_INTERVAL ){
        ie = 0;
        seg =  ( frame->timestamp - this->FirstTimeStamp)  /1000  ;
        LOGGER_INFO("time stamp e -> %4d\n",seg);
    }
    if(type == 0 ){
        iv++;
    }
    else if(type == 1 ){
        ia++;
    }
    else if(type == 2 ){
        ie++;
    }
//    return true ;
    if(index++ > 2 || this->VideoTimeScaleRate < 1){
        index = 0;
        seg =  frame->timestamp - this->FirstTimeStamp;
        if(type == 0){
            seg = this->GetMotionPlayTime(seg);
        }
        if ((!this->CheckIFrame[0]) && ((!this->CheckIFrame[1])))
        {
            if ((seg >= this->RecInfo.time_segment * 1000 + this->StartDelayTime)
                || (this->StartNewSeg.load())){
                if (strlen(RecInfo.video_channel)) {
                    video_trigger_key_frame(RecInfo.video_channel);
                }

                if (strlen(RecInfo.videoextra_channel)) {
                     video_trigger_key_frame(RecInfo.videoextra_channel);
                }
                this->CheckIFrame[0] = true;
                this->CheckIFrame[1] = true;
                time(&this->NextStartTime);
                LOGGER_INFO("will create new file type:%d seg:%d frame->timestamp:%lld FirstTimeStamp:%lld\n", type, seg, frame->timestamp, this->FirstTimeStamp);
            }
        }
    }
    return true ;
}

bool MediaMuxer::InternalReleaseMuxerFile(void){
    this->FunctionLock.lock();
    LOGGER_TRC("%s begin\n",__FUNCTION__);
    time_t now ;
    time(&now);
    LOGGER_INFO("-->real time cost ->%ld\n",now - this->StartTime);
    bool ret = true ;
    if(this->MediaEnableAudio && this->FrAudio.virt_addr != NULL){
        LOGGER_WARN("release the last audio frame\n");
    //    this->AddAudioFrFrame(&this->FrAudio);
        if(this->PFrFifoAudio->PutItem(&this->FrAudio) <= 0){
            LOGGER_ERR("free fifo buffer error\n");
        }
    }
    this->NewFileFlags.store(true);
    if(this->PBsfFilter != NULL){
        LOGGER_DBG("release aac filter\n");
        av_bitstream_filter_close(this->PBsfFilter);
        this->PBsfFilter = NULL;
    }
    if(this->MediaFileFormatContext ){
        LOGGER_DBG("try free MediaFileFormatContext\n");
        if( this->StopMediaFile() == false ){
            LOGGER_ERR("stop media file muxer error\n");
            ret = false ;
        }
        else {
            if(    this->UpdateMediaFile() == false ){
                LOGGER_ERR("rename media file error\n");
                ret = false ;
            }
        }
#if 0
        uint64_t tsOld,tsNew;
        tsOld = get_timestamp();
        sync();
        tsNew = get_timestamp();
        LOGGER_INFO("sync file time cost ->%8lld\n", tsNew - tsOld);
#endif
    }
    else{
        LOGGER_WARN("media format context is null\n");
    }
    this->MediaFileFormatContext = NULL;
    LOGGER_DBG("%s exit\n",__FUNCTION__);
//    this->MediaFileFormatContext->pb = NULL;
    //this->MediaFileOpenState.store(false);
    this->ValidMediaFile =  false;
    this->FunctionLock.unlock();
    return ret;
}

void MediaMuxer::ShowInfo(int32_t flags){
    if(flags & MEDIA_MUXER_INFO_FIFO ){
        LOGGER_DBG("show fifo  info ->\n");
        if(this->MediaEnableVideo ) {
            for(int i=0;i<this->VideoCount;i++) {
                LOGGER_TRC("\t--->index:%d v:%d \n",i ,this->PFrFifoVideo[i]->GetQueueSize());
            }
        }
        if(this->MediaEnableAudio )
            LOGGER_TRC("\t--->a:%d \n",this->PFrFifoAudio->GetQueueSize() );
        if(this->MediaEnableExtra )
            LOGGER_TRC("\t--->e:%d \n",this->PFrFifoExtra->GetQueueSize() );
    }
    if(flags & MEDIA_MUXER_INFO_FRAMENUM ){
        LOGGER_DBG("display frame info ->\n");
    }
}

bool MediaMuxer::Alert(char *alert){
    printf("alert file ->%s\n",alert);
    return true ;
}
bool MediaMuxer::CheckFileSystem(void){
    return true ;
}
bool MediaMuxer::ThreadPreStartHook(void){
    this->ValidMediaFile = false;
    //this->MediaFileOpenState.store(false);
    IsVideoboxRun.store(true);
}
bool MediaMuxer::ThreadPostPauseHook(void){
    LOGGER_DBG("Post Pause Hook begin\n");
    this->InternalReleaseMuxerFile();
    this->InitStreamChannel();
    LOGGER_DBG("Post Pause Hook end\n");
}
bool MediaMuxer::ThreadPostStopHook(void){
    LOGGER_DBG("Post Pause Hook begin\n");
    this->InternalReleaseMuxerFile();
    this->InitStreamChannel();
    LOGGER_DBG("Post Pause Hook end\n");
    memset(&this->FrVideo ,0 ,sizeof(struct fr_buf_info));
    memset(&this->FrAudio ,0 ,sizeof(struct fr_buf_info));
    memset(&this->FrAudio ,0 ,sizeof(struct fr_buf_info));
    memset(pu8UUIDData, 0, MAX_UUID_SIZE);
    this->FastMotionByPlayer = false;
    this->VideoSkipFrameNum = 0;
    this->IsKeepFrame[0] = false;
    this->IsKeepFrame[1] = false;
    this->CheckIFrame[0] = false;
    this->CheckIFrame[1] = false;
}

void  MediaMuxer::LinkVideoStreamInfo(VideoStreamInfo **info){
    this->PVideoInfo = info;
}
void MediaMuxer::InitStreamChannel(void){
    for(int i = 0; i < this->StreamChannelNum; i++){
        this->PStreamChannel[i].last_timestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
        this->PStreamChannel[i].last_dts = 0ll;
        this->PStreamChannel[i].frame_num = 0ll;
    }
}
int64_t  MediaMuxer::FrameDtsAutoReCal(int streamIndex, uint64_t timestamp, int64_t dts){
    stream_channel_info_t *streamChannel = this->PStreamChannel + streamIndex;
    streamChannel->frame_num++;
    if(streamChannel->last_timestamp == MEDIA_MUXER_HUGE_TIMESDTAMP){
        //first init
        LOGGER_DBG("first init stream timestamp ->%d %8lld\n", streamIndex, timestamp-this->FirstTimeStamp);
        streamChannel->last_dts = dts;
        streamChannel->last_timestamp = timestamp - this->FirstTimeStamp;
    }
    else {
        if(dts <= streamChannel->last_dts){
            LOGGER_WARN("TS->%2d l:c  dts->%6lld:%6lld ts->%7lld:%7lld\n",
                    streamIndex,
                    streamChannel->last_dts,
                    dts,
                    streamChannel->last_timestamp ,
                    timestamp - this->FirstTimeStamp
                    );
            streamChannel->last_dts = streamChannel->last_dts + 1;
        }
        else {
            streamChannel->last_dts = dts;
        }
    }
    streamChannel->last_timestamp = timestamp - this->FirstTimeStamp;
    return streamChannel->last_dts;
}
bool MediaMuxer::UpdateDirectIOFlags(void){
    #if 0
    if(this->MediaFileDirectIO){
        AVIOContext *pb = this->MediaFileFormatContext->pb;
        URLContext *url = (URLContext *)pb->opaque;
        int fileFd = url->prot->url_get_file_handle(url);
        if(fileFd >= 0){
            int oldFileFlags = fcntl(fileFd, F_GETFL);
            int newFileFlags = oldFileFlags | O_DIRECT;
            int fcntlRet = fcntl(fileFd, F_SETFL, newFileFlags);
            int realFileFlags = fcntl(fileFd, F_GETFL);
            LOGGER_INFO("##Direct IO->fd:%2d ret:%2d old:0x%X  new:0x%X real:0x%X\n",
        fileFd, fcntlRet, oldFileFlags, newFileFlags, realFileFlags);
        //sleep(10);
            return true;
        }
        else{
            LOGGER_ERR("!!!get file fd error\n");
            return false;
        }
    }
    #endif
}
bool MediaMuxer::SyncStorage(void){
    return true;
    AVIOContext *pb = this->MediaFileFormatContext->pb;
    URLContext *url = (URLContext *)pb->opaque;
    int fileFd = url->prot->url_get_file_handle(url);
    LOGGER_INFO("syc storage ->%d\n", fileFd);
    uint64_t tsOld,tsNew;
    tsOld = get_timestamp();
    if(fileFd >= 0){
        fsync(fileFd);
        tsNew = get_timestamp();
        LOGGER_INFO("sync file time cost ->%8lld\n", tsNew - tsOld);
        return true;
    }
    else {
        return false;
    }
}
int MediaMuxer::SetMediaUdta(char *buf, int size){
    int ret = 0;
    int fd = -1;
    int result;
    if(size == 0){
        fd = open(buf, O_RDONLY);
        if(fd < 0){
            LOGGER_ERR("open media udta data error->%s\n", buf);
            this->MediaUdtaBufferReady.store(false);
            return -1;
        }
        else {
            size =  lseek(fd, 0, SEEK_END);
            if(size <= 1){
                LOGGER_ERR("media udta file is empty ->%s\n", buf);
                this->MediaUdtaBufferReady.store(false);
                return -1;
            }
            LOGGER_INFO("udta file size -> %d %d\n", fd, size);
            lseek(fd, 0, SEEK_SET);
        }
    }
    this->MediaUdtaMutexLock.lock();
    if(this->MediaUdtaBuffer != NULL){
        this->MediaUdtaBufferReady.store(false);
        free(this->MediaUdtaBuffer);
        this->MediaUdtaBuffer = NULL;
        this->MediaUdtaBufferSize = 0;
    }
    if(this->MediaUdtaBuffer == NULL){
        this->MediaUdtaBuffer = (char *)malloc(size);
        if(this->MediaUdtaBuffer == NULL){
            LOGGER_ERR("malloc media udta buffer error ->%d\n", size);
            this->MediaUdtaBufferReady.store(false);
            //error and go end
        }
        else {
            this->MediaUdtaBufferSize =  size;
        LOGGER_INFO("malloc udta buffer ->%p  %5d\n", this->MediaUdtaBuffer, this->MediaUdtaBufferSize);
        }
    }
    if(this->MediaUdtaBuffer != NULL && this->MediaUdtaBufferSize > 0) {
        if(fd >= 0){
            result = read(fd, this->MediaUdtaBuffer, size);
            if(result != size ){
                LOGGER_ERR("read udta size error->result:%5d size:%5dd\n", result, size);
                ret = -1;;
            }
            LOGGER_INFO("load udta data from file ->%p %d\n", buf, result);
            close(fd);
        }
        else{
            LOGGER_INFO("load udta data from memory->%p %d\n", buf, result);
            memcpy(this->MediaUdtaBuffer, buf, size);
        }
        LOGGER_DBG("add udata ->%5d\n", this->MediaUdtaBufferSize);
        if(ret < 0){
            this->MediaUdtaBufferReady.store(false);
        }
        else{
            this->MediaUdtaBufferReady.store(true);
        }
    }
    else {
        size = -1;
    }
    this->MediaUdtaMutexLock.unlock();
    return size;
}
void MediaMuxer::UpdateMediaFileContextUdta(void){
    if(this->MediaUdtaBufferReady.load()){
        this->MediaFileFormatContext->user_data =  this->MediaUdtaBuffer;
        this->MediaFileFormatContext->user_data_size = this->MediaUdtaBufferSize;
        LOGGER_INFO("write user custom udta ->%d\n", this->MediaFileFormatContext->user_data_size);
    }
    else {
        LOGGER_INFO("use default udta\n");
        this->MediaFileFormatContext->user_data = NULL;
        this->MediaFileFormatContext->user_data_size = 0;
    }
}
bool MediaMuxer::SetSlowMotion(int rate){
    int fps = video_get_fps(this->RecInfo.video_channel);
    v_frc_info info = {0};
    info.framebase = 1;
    info.framerate = fps;
    video_set_frc(this->RecInfo.video_channel, &info);
    if (strlen(this->RecInfo.videoextra_channel) != 0)
    {
        video_set_frc(this->RecInfo.videoextra_channel, &info);
    }
    this->SlowFastMotionRate.store(rate);//rate > 0 .timestamp *= rate
    printf("slow rate -> %d\n", this->SlowFastMotionRate.load());
    return true;
}
/*
 *framerate/framebase = targetFps
 *targetFps/sourceFps = 1/rate
 * */
bool MediaMuxer::SetFastMotion(int rate){
    int fps = video_get_fps(this->RecInfo.video_channel);
    v_frc_info info = {0};
    if (rate <= 0)
    {
        LOGGER_ERR("set fast rate error-> %d\n", rate);
        return false;
    }
    info.framebase = rate;
    info.framerate = fps;
    LOGGER_DBG("set frc data -> %d:%d\n", info.framerate, info.framebase);
    video_set_frc(this->RecInfo.video_channel, &info);
    if (strlen(this->RecInfo.videoextra_channel) != 0)
    {
        video_set_frc(this->RecInfo.videoextra_channel, &info);
    }
    this->SlowFastMotionRate.store(0 - rate);
    LOGGER_INFO("fast rate -> %d\n", this->SlowFastMotionRate.load());
    return true;
}

bool MediaMuxer::StartNewSegment(VRecorderInfo * info){
    this->StartNewSeg.store(true);
    if (info != NULL)
    {
        this->NewSegInfo = *info;
    }
    LOGGER_INFO("Start new segment!\n");
    return true;
}

bool MediaMuxer::SetFastMotionEx(int rate){
    if (rate <= 0)
    {
        LOGGER_ERR("set fast motion error -> %d\n", rate);
        return false;
    }
    this->FastMotionByPlayer = true;
    this->VideoSkipFrameNum = rate -1;
    this->SlowFastMotionRate.store(0 - rate);
    LOGGER_INFO("fast rate -> %d\n", this->SlowFastMotionRate.load());
    return true;
}

bool MediaMuxer::SetUUIDData(char* pu8UUID)
{
    if ((pu8UUID == NULL) || (strlen(pu8UUID) == 0) || (strlen(pu8UUID) >= MAX_UUID_SIZE))
    {
        LOGGER_ERR("set uuid error(%p)\n", pu8UUID);
        return false;
    }
    memset(this->pu8UUIDData, 0, MAX_UUID_SIZE);
    memcpy(this->pu8UUIDData, pu8UUID, strlen(pu8UUID));
    LOGGER_INFO("Set uuid -> %d\n", strlen(this->pu8UUIDData));
    return true;
}

int MediaMuxer::GetMotionRate(void){
    return this->SlowFastMotionRate.load();
}
uint32_t  MediaMuxer::GetMotionPlayTime(uint32_t timestamp){
    int seg = 0;
    if(this->VideoTimeScaleRate >  1){
        //slow motion
        seg  = timestamp * this->SlowFastMotionRate;
    }
    else if(this->VideoTimeScaleRate < -1){
        seg  = timestamp / (0 - this->SlowFastMotionRate);
        seg = seg == 0 ?1:seg;
    }
    else if (this->VideoTimeScaleRate == 1){
        seg = timestamp;
    }
    else{
        LOGGER_ERR("motion value is error ->%d\n", this->VideoTimeScaleRate);
    }
    return seg;
}
bool MediaMuxer::UpdateSpsFrameRate(uint8_t *header, int *headerSize, int frameRate, int frameBase){
    h264_stream_t *h = h264_new();
    if( h == NULL){
        LOGGER_ERR("malloc h264 stream error\n");
        return false;
    }
    uint8_t *p = header;
    int nal_start, nal_end;
    uint8_t spsBuf[64] = {0};
    uint8_t newHeaderBuf[128] = {0};
    int index = 0;
    int len  = 0;
    while(find_nal_unit(p, *headerSize, &nal_start, &nal_end) > 0){
        p += nal_start;
        read_nal_unit(h, p, nal_end - nal_start);
        printf("\nfind nal ->%d %d %d\n", nal_start, nal_end, h->nal->nal_unit_type);
        if(h->nal->nal_unit_type == NAL_UNIT_TYPE_SPS){
          //  debug_nal(h, h->nal);
            h->sps->vui.time_scale = frameRate;
            h->sps->vui.num_units_in_tick = frameBase;
            int ret = write_nal_unit(h, spsBuf, 64);
            memcpy(newHeaderBuf+index, p-4, 5);
            index += 5;
            memcpy(newHeaderBuf + index, spsBuf+1 , ret - 1);
            index += (ret-1);

        }
        else{
            memcpy(newHeaderBuf + index, p - nal_start, nal_end - nal_start + 4);
            index += (nal_end - nal_start + 4);
        }
        p += (nal_end - nal_start);
    }
    h264_free(h);
    *headerSize = index;
    memcpy(header, newHeaderBuf, index);
    return true;
}

ThreadingState MediaMuxer::PopFirstIFrame(QMediaFifo  *vfifo, struct fr_buf_info *fr_video, int *ret_value){
    static int tryCounter = 0;
    int ret = 0;
    ThreadingState realTimeState;
    *ret_value = 0;
    while(true){
        ret = vfifo->GetItem(fr_video);
        if(ret <= 0){
            LOGGER_INFO(" get out video error ->%p\n",this->PFrFifoVideo);
            usleep(10000);
            tryCounter++;
            if(this->GetWorkingState() != ThreadingState::Start){
                return ThreadingState::Keep;
            }
            if(tryCounter > MEDIA_MUXER_RETRY_MAX_NUM){
                LOGGER_ERR("get video frame error\n");
                vplay_report_event(VPLAY_EVENT_VIDEO_ERROR,(char *)"no video frame");
                this->RunErrorCallBack();
                //this->InternalReleaseMuxerFile();
                LOGGER_ERR("no vidoe stream ,just stop muxer\n");
                vplay_report_event(VPLAY_EVENT_VIDEO_ERROR ,(char *)"no video stream ,pause muxer\n");
                return ThreadingState::Pause;
            }
            continue;
        }

        if(fr_video->priv == VIDEO_FRAME_I ){
            LOGGER_INFO("\n-->get I Frame ->%d  %8lld \n", fr_video->size, fr_video->timestamp);
            *ret_value = 1;
            realTimeState = this->GetWorkingState();
            tryCounter = 0;
            return realTimeState;
        }
        if(fr_video->priv == VIDEO_FRAME_P ){
            LOGGER_INFO(" P:%d \n",vfifo->GetQueueSize());
        }else{
            LOGGER_INFO("-->get wrong frame ->%d %d\n",fr_video->size,fr_video->priv);
        }
        if(tryCounter >= 50){
            realTimeState = this->GetWorkingState();
            if(realTimeState != ThreadingState::Start) {
                return realTimeState;
            }
        }
        if(vfifo->PutItem(fr_video) <= 0){
            LOGGER_ERR("put video no i frame\n");
        }
    }
}



