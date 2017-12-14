#include <qrecorder.h>
#include <assert.h>
#include <define.h>
#include <qsdk/vplay.h>
#include <qsdk/event.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
void error_process_callback(void *arg, void *obj) {
    QRecorder *recorder =( QRecorder *)arg ;
    LOGGER_ERR("system error, will stop all thread \n");
    recorder->HandlerErrorCallback(obj);
}
bool QRecorder::Prepare(void) {
    //basic para check
    if (this->RecInfo.time_segment < 5){
        LOGGER_ERR("min time segment(%d) is 5\n", this->RecInfo.time_segment);
        return false ;
    }
    //audio handler
#if 1  //enable audio function
    if(strlen(this->RecInfo.audio_channel) != 0){
        LOGGER_DBG("prepare api audio\n");
        this->CallerAudio = new ApiCallerAudio(this->RecInfo.audio_channel,\
            &this->RecInfo.audio_format, &this->RecInfo.stAudioChannelInfo);
        if (this->CallerAudio == NULL){
            LOGGER_ERR("init ApiCallerAudio error\n");
            goto prepare_error_handler;
        }
        else {
            if(this->CallerAudio->Prepare() ==  false ){
                LOGGER_ERR("ApiCallerAudio prepare error \n");
                goto prepare_error_handler;
            }
        }
        this->FifoLoopAudio = new QMediaFifo((char *)"AudioFifo");
        if(this->RecInfo.time_backward != 0){
            this->FifoLoopAudio->SetMaxCacheTime( (this->RecInfo.time_backward +2)*1000);
        }
        else{
            this->FifoLoopAudio->SetMaxCacheTime((RECORDER_CACHE_SECOND) * 1000);
        }
        LOGGER_INFO("recorder audio ->time:%8lld\n", this->FifoLoopAudio->GetMaxCacheTime());
        if(this->FifoLoopAudio == NULL){
            LOGGER_ERR("init FifoLoopAudio error\n");
            goto prepare_error_handler ;
        }
        this->CallerAudio->PFrFifo =  this->FifoLoopAudio ;
        if(this->RecInfo.time_backward ==  0)
            this->CallerAudio->SetFifoAutoFlush(true);
        this->CallerAudio->SetWorkingState(ThreadingState::Standby);
    }

#endif
    //video handler
#if 1
    if (strlen(this->RecInfo.video_channel) != 0) {
        this->VideoCount++;
    }
    if (strlen(this->RecInfo.videoextra_channel) != 0) {
        this->VideoCount++;
    }

    CallerVideo = new ApiCallerVideo *[VideoCount];
    VideoInfo = new VideoStreamInfo *[VideoCount];
    FifoLoopVideo = new QMediaFifo *[VideoCount];

    for(int i=0;i<VideoCount;i++) {
        VideoInfo[i] = new VideoStreamInfo();
    }

    if(strlen(this->RecInfo.video_channel) != 0) {
        if (!CreateVideoResource(this->RecInfo.video_channel, VideoInfo[VideoIndex])) {
            LOGGER_ERR("CreateVideoResource failed! vchannel:%s\n", RecInfo.video_channel);
            return false;
        }
        VideoIndex++;
    }

    if(strlen(this->RecInfo.videoextra_channel) != 0) {
        if(!CreateVideoResource(this->RecInfo.videoextra_channel, VideoInfo[VideoIndex])) {
            LOGGER_ERR("CreateVideoResource failed! videoextra_channel:%s\n", RecInfo.videoextra_channel);
            return false;
        }
        VideoIndex++;
    }
#endif
    //gps handler
    if(this->RecInfo.enable_gps ){
    LOGGER_DBG("prepare api extra\n");
        this->CallerExtra = new ApiCallerExtra();
        if (this->CallerExtra == NULL){
            LOGGER_ERR("init apiCallerExtra error\n");
            goto prepare_error_handler;
        }
        else {
            if(this->CallerExtra->Prepare() ==  false ){
                LOGGER_ERR("ApiCallerAudio prepare error \n");
                goto prepare_error_handler;
            }
        }

        this->FifoLoopExtra = new QMediaFifo((char *)"ExtraFifo");
        if(this->RecInfo.time_backward != 0){
            this->FifoLoopExtra->SetMaxCacheTime( (this->RecInfo.time_backward +2)*1000);
        }
        else{
            this->FifoLoopExtra->SetMaxCacheTime( (RECORDER_CACHE_SECOND)*1000);
        }
        if(this->FifoLoopExtra == NULL){
            LOGGER_ERR("init FifoLoopExtra error\n");
            goto prepare_error_handler ;
        }
        this->CallerExtra->PFrFifo =  this->FifoLoopExtra ;
        if(this->RecInfo.time_backward ==  0)
            this->CallerExtra->SetFifoAutoFlush(true);
    }
    if ((this->RecInfo.s32KeepFramesTime > 0) && (this->RecInfo.time_backward == 0)
        && (this->VideoCount == 1))
    {
        if (strlen(this->RecInfo.audio_channel) != 0)
        {
            this->FifoLoopAudio->SetKeepFramesTime(this->RecInfo.s32KeepFramesTime * 1000);
        }
        this->FifoLoopVideo[0]->SetKeepFramesTime(this->RecInfo.s32KeepFramesTime * 1000);
    }
    LOGGER_DBG("prepare MediaMuxer\n");
    //muxer prepare
    this->Muxer = new MediaMuxer(&this->RecInfo, this->VideoInfo, VideoCount);
    this->Muxer->SetErrorCallBack(error_process_callback, this);
    if(this->Muxer == NULL){
        LOGGER_ERR("init MediaMuxer error\n");
        goto prepare_error_handler ;
    }
    if( this->Muxer->SetFrFifo(this->FifoLoopVideo ,
                            this->FifoLoopAudio ,
                            this->FifoLoopExtra )  == false ){
        LOGGER_ERR("set MediaMuxer fifo error\n");
        goto prepare_error_handler ;
    }
    if(this->Muxer->Prepare() == false ){
        LOGGER_ERR("MediaMuxer prepare  error\n");
        goto prepare_error_handler ;

    }
#if 0

    this->Muxer->SetWorkingState(ThreadingState::Standby);
#endif

    if(this->RecInfo.enable_gps ) {
        this->CallerExtra->SetWorkingState(ThreadingState::Standby);
    }

    //standby need thread
    if(this->RecInfo.time_backward > 0 ){
        LOGGER_TRC("time backward mode ,need cache frame \n");
        for(int i=0;i<VideoCount;i++) {
            this->CallerVideo[i]->SetWorkingState(ThreadingState::Start);
            if ((this->FifoLoopVideo) && (this->FifoLoopVideo[i]))
            {
                this->FifoLoopVideo[i]->MonitorDropFrames(false);
            }
        }
        this->CallerAudio->SetWorkingState(ThreadingState::Start);
        if (this->FifoLoopAudio)
        {
            this->FifoLoopAudio->MonitorDropFrames(false);
        }

        if(this->RecInfo.enable_gps ) {
            this->CallerExtra->SetWorkingState(ThreadingState::Start);
        }
    }
    else if (this->RecInfo.time_backward == 0 ){
        LOGGER_TRC("no time_backward ,do not auto cache\n");
    }
    else {
        LOGGER_ERR("time_backward must large or equal 0\n");
        goto prepare_error_handler ;
    }


    LOGGER_DBG("fifo -> %p %p %p\n",
        this->FifoLoopVideo ,
        this->FifoLoopAudio ,
        this->FifoLoopExtra );
    return true ;

prepare_error_handler :
    this->UnPrepare();
    return false ;
}
bool QRecorder::UnPrepare(void){
    if(this->Muxer){
        LOGGER_TRC("release muxer inst ->%p\n", this->Muxer);
        delete this->Muxer ;
        this->Muxer =  NULL;
    }
    if(this->VideoCount > 0){
        for (int i=0;i<VideoCount;i++) {
            LOGGER_TRC("release video inst ->%p\n",this->CallerVideo[i]);
            delete this->CallerVideo[i];
            this->CallerVideo[i] =  NULL;
        }
    }
    if(CallerVideo) {
        delete [] CallerVideo;
        CallerVideo = NULL;
    }
    if(this->CallerAudio){
        LOGGER_TRC("release audio inst ->%p\n", this->CallerAudio);
        delete this->CallerAudio;
        this->CallerAudio =  NULL;
    }
    if(this->CallerExtra){
        LOGGER_TRC("release extra inst ->%p\n", this->CallerExtra);
        delete this->CallerExtra;
        this->CallerExtra=  NULL;
    }
    if(this->FifoLoopAudio){
        LOGGER_DBG("free audio fifo ->%p\n",this->FifoLoopAudio);
        delete this->FifoLoopAudio;
        this->FifoLoopAudio = NULL;
    }
    if(this->VideoCount > 0){
        for(int j=0;j<VideoCount;j++) {
            LOGGER_DBG("free video fifo ->%p\n", this->FifoLoopVideo[j]);
            delete this->FifoLoopVideo[j];
            this->FifoLoopVideo[j] = NULL;
        }
    }
    if(FifoLoopVideo) {
        delete [] FifoLoopVideo;
        FifoLoopVideo = NULL;
    }
    for(int i=0;i<VideoCount;i++) {
        if (VideoInfo[i]) {
            delete VideoInfo[i];
            VideoInfo[i] = NULL;
        }
    }
    if (VideoInfo) {
        delete [] VideoInfo;
        VideoInfo = NULL;
    }
}

bool QRecorder::CreateVideoResource(char * video_channel,VideoStreamInfo *video_info) {
    int frameRate, frameSize, frameNum;
    this->CallerVideo[VideoIndex] = new ApiCallerVideo(video_channel, video_info);
    if (this->CallerVideo[VideoIndex] == NULL){
        LOGGER_ERR("init apiCallerVideo error\n");
        this->UnPrepare();
        return false;
    } else {
        if(this->CallerVideo[VideoIndex]->Prepare() ==  false ){
        LOGGER_ERR("ApiCallerVideo prepare error \n");
        this->UnPrepare();
        return false;
        }
    }
    LOGGER_INFO("Has done CallerVideo[VideoIndex]->Prepare()");
    if (RecInfo.fixed_fps){
        this->CallerVideo[VideoIndex]->SetAutoCheckFps(false);
        video_info->SetFps(RecInfo.fixed_fps);
        video_info->SetUpdate(true);
    } else {
        this->CallerVideo[VideoIndex]->SetAutoCheckFps(true);
    }

    //QMediaFifo::QMediaFifo(int bufNum ,int bufSize ,int maxBufSize,char *name) { //usr fr float,must use Prepare
    char buf[32] = {0};
    sprintf(buf, "VideoFifo_%d", VideoIndex);
    this->FifoLoopVideo[VideoIndex] = new QMediaFifo(buf);
    if (this->RecInfo.time_backward != 0) {
        this->FifoLoopVideo[VideoIndex]->SetMaxCacheTime( (this->RecInfo.time_backward +2)*1000);
    } else {
        this->FifoLoopVideo[VideoIndex]->SetMaxCacheTime(RECORDER_CACHE_SECOND *1000);
    }
    if (this->FifoLoopVideo[VideoIndex] == NULL) {
        LOGGER_ERR("init FifoLoopVideo error\n");
        this->UnPrepare();
        return false;
    }

    this->CallerVideo[VideoIndex]->PFrFifo =  this->FifoLoopVideo[VideoIndex] ;
    if (this->RecInfo.time_backward ==  0) {
        this->CallerVideo[VideoIndex]->SetFifoAutoFlush(true);
    }
    this->CallerVideo[VideoIndex]->SetWorkingState(ThreadingState::Standby);
    return true;
}

bool QRecorder::SetWorkingState(ThreadingState state){
    //NOTE : the pause recorder action just means : muxer pause
    //  the stop recorder action means delete all resource
    this->FunctionLock.lock();
    LOGGER_INFO("%s ->%d\n",__FUNCTION__,state);
    ThreadingState threadState ;
    int motionRate = this->Muxer->GetMotionRate();
    switch(state){
        case ThreadingState::Start :
            LOGGER_INFO("start QRecorder\n");
            //    this->CallerVideo->SetWorkingState(ThreadingState::Start);
            //    this->CallerExtra->SetWorkingState(ThreadingState::Start);
            if(this->RecInfo.time_backward == 0 ){
                //cache mode ,the api thread is already ready
                if(strlen( this->RecInfo.audio_channel ) > 0 ){
                    if(motionRate == 1)
                        this->CallerAudio->SetWorkingState(ThreadingState::Start);
                    else{
                        this->CallerAudio->SetWorkingState(ThreadingState::Pause);
                    }
                }
                if(this->VideoCount > 0) {
                    for(int i=0;i<VideoCount;i++) {
                        this->CallerVideo[i]->SetWorkingState(ThreadingState::Start);
                    }
                }
                if(this->RecInfo.enable_gps ) {
                    this->CallerExtra->SetWorkingState(ThreadingState::Start);
                }
            }
            else
            {
                if(strlen( this->RecInfo.audio_channel ) > 0 ){
                    if (this->FifoLoopAudio)
                    {
                        this->FifoLoopAudio->MonitorDropFrames(true);
                    }
                }
                if(this->VideoCount > 0) {
                    for(int i=0;i<VideoCount;i++) {
                        if ((this->FifoLoopVideo) && (this->FifoLoopVideo[i]))
                        {
                            this->FifoLoopVideo[i]->MonitorDropFrames(true);
                        }
                    }
                }
            }
            LOGGER_INFO("start muxer api thread\n");
            this->Muxer->SetWorkingState(ThreadingState::Start);
            break;
        case ThreadingState::Stop :
        {
            LOGGER_INFO("stop QRecorder\n");
#if 0
            threadState = this->Muxer->GetWorkingState() ;
            if(threadState != ThreadingState::Pause){
                this->Muxer->SetWorkingState(ThreadingState::Pause);
                LOGGER_TRC("stop QRecorder\n");
                //this->Muxer->ReleaseMuxerFile();
            }
#endif
#if 1
            this->Muxer->SetWorkingState(ThreadingState::Stop);

#endif
            if (this->Muxer->GetMotionRate() != 1)
            {
                this->Muxer->SetSlowMotion(1);
            }
            if(this->VideoCount > 0) {
                for(int i=0;i<VideoCount;i++) {
                    this->CallerVideo[i]->SetWorkingState(ThreadingState::Stop);
                    if ((this->FifoLoopVideo) && (this->FifoLoopVideo[i]))
                    {
                        this->FifoLoopVideo[i]->MonitorDropFrames(true);
                    }
                }
            }
            if(strlen(this->RecInfo.audio_channel) != 0)
            {
                this->CallerAudio->SetWorkingState(ThreadingState::Stop);
                if (this->FifoLoopAudio)
                {
                    this->FifoLoopAudio->MonitorDropFrames(true);
                }
            }
            if(this->RecInfo.enable_gps)
                this->CallerExtra->SetWorkingState(ThreadingState::Stop);
        }
            break;
        case ThreadingState::Pause :
            LOGGER_INFO("pause QRecorder\n");
            this->Muxer->SetWorkingState(ThreadingState::Pause);
        //    return true;
            if (this->Muxer->GetMotionRate() != 1)
            {
                this->Muxer->SetSlowMotion(1);
            }
            if(this->RecInfo.time_backward == 0 ){
                //no cache state ,just stop all
                if(strlen( this->RecInfo.audio_channel ) > 0 ){
                    LOGGER_DBG("pause audio api thread\n");
                    this->CallerAudio->SetWorkingState(ThreadingState::Pause);
                }
                if(this->VideoCount > 0) {
                    for(int i=0;i<VideoCount;i++) {
                        this->CallerVideo[i]->SetWorkingState(ThreadingState::Pause);
                    }
                }
                if(this->RecInfo.enable_gps != 0 ){
                    LOGGER_DBG("pause extra api thread\n");
                    this->CallerExtra->SetWorkingState(ThreadingState::Pause);
                }
            }
            else
            {
                if(strlen( this->RecInfo.audio_channel ) > 0 ){
                    if (this->FifoLoopAudio)
                    {
                        this->FifoLoopAudio->MonitorDropFrames(false);
                    }
                }
                if(this->VideoCount > 0) {
                    for(int i=0;i<VideoCount;i++) {
                        if ((this->FifoLoopVideo) && (this->FifoLoopVideo[i]))
                        {
                            this->FifoLoopVideo[i]->MonitorDropFrames(false);
                        }
                    }
                }
            }
            break;
        default :
            LOGGER_ERR("not support state ->%d\n",state);
    }
    this->WorkingThreadState =  state ;
    this->FunctionLock.unlock();
    return true;
}
ThreadingState  QRecorder::GetWorkingState(void){

    return this->WorkingThreadState ;
}
QRecorder::QRecorder(VRecorderInfo *info){
    assert(info != NULL);
    int i = 0;
    this->RecInfo = *info ;
    this->CallerVideo = NULL;
    this->FifoLoopVideo = NULL;
    this->VideoInfo = NULL;
    this->CallerAudio = NULL;
    this->CallerExtra = NULL;
    this->FifoLoopAudio = NULL;
    this->FifoLoopExtra = NULL;
    this->VideoIndex = 0;
    this->VideoCount = 0;
    this->Muxer = NULL;
}
QRecorder::~QRecorder(){
    this->UnPrepare();
}
bool QRecorder::PrepareVideo(void) {
    return true ;
}
bool QRecorder::PrepareAudio(void) {
    return true ;
}
bool QRecorder::PrepareExtra(void) {
    return true ;
}

bool QRecorder::Alert(char *alert){
    this->Muxer->Alert(alert);
    return true ;
}


void QRecorder::SetMute(VPlayMediaType type, bool enable){
    this->FunctionLock.lock();
    if(strlen(this->RecInfo.audio_channel) > 0 ){
        switch(type){
            case VPLAY_MEDIA_TYPE_AUDIO:
                LOGGER_DBG("mute audio %d\n", enable);
                this->CallerAudio->SetMute(enable);
                break;
            case VPLAY_MEDIA_TYPE_VIDEO:
                LOGGER_WARN("mute video %d,not support now\n", enable);
                break;
            case VPLAY_MEDIA_TYPE_SUBTITLE:
                LOGGER_WARN("mute subs %d,not support now\n", enable);
                break;
            case VPLAY_MEDIA_TYPE_DATA:
                LOGGER_WARN("mute data %d,not support now\n", enable);
                break;
            default :
                LOGGER_WARN("mute not support media %d: %d\n", type, enable);
                break;
        }
    }
    else{
        LOGGER_ERR("audio channel is disabled, no need to mute\n");
    }
    this->FunctionLock.unlock();
}

void QRecorder::HandlerErrorCallback(void *obj){
    if(this->Muxer == obj){
        LOGGER_ERR("muxer error happened\n");
        if(this->RecInfo.time_backward == 0){
            if(this->VideoCount > 0) {
                for(int i=0;i<VideoCount;i++) {
                    this->CallerVideo[i]->SetWorkingState(ThreadingState::Pause);
                }
            }
            if(this->CallerAudio){
                LOGGER_INFO("pause audio thread\n");
                this->CallerAudio->SetWorkingState(ThreadingState::Pause);
            }
            if(this->CallerExtra){
                LOGGER_INFO("pause extra thread\n");
                this->CallerExtra->SetWorkingState(ThreadingState::Pause);
            }
        }
    }
}
int QRecorder::SetMediaUdta(char *buf, int size){
    int ret =0;
    this->FunctionLock.lock();
    if(size == 0){
        if(access(buf,R_OK) == 0){
            LOGGER_INFO("use udta file setting\n");
        }
        else {
            LOGGER_ERR("recorder udta file can not read ->%s", buf);
            ret = -1;
        }
    }
    ret = this->Muxer->SetMediaUdta(buf, size);
    this->FunctionLock.unlock();
    return ret;
}

void QRecorder::SetCaheMode(bool mode) {
    if(this->VideoCount > 0) {
        for(int i=0;i<VideoCount;i++) {
            this->CallerVideo[i]->SetCaheMode(mode);
        }
    }
}

bool QRecorder::StartNewSegment(VRecorderInfo *pstInfo)
{
    return this->Muxer->StartNewSegment(pstInfo);
}

bool QRecorder::GetAudioChannelInfo(ST_AUDIO_CHANNEL_INFO *pstInfo)
{
    return this->CallerAudio->GetAudioChannelInfo(pstInfo);
}

void vplay_show_recorder_info(VRecorderInfo *info){
//    return ;
    printf("show recorder info start -> \n");
    printf("\t video_channel    ->%s<-\n",info->video_channel);
    printf("\t videoextra_channel    ->%s<-\n",info->videoextra_channel);
    printf("\t audio_channel    ->%s<-\n",info->audio_channel);
    printf("\t enable gps       ->%d<-\n",info->enable_gps);
    printf("\t keep temp        ->%d<-\n",info->keep_temp);
    printf("\t audio type       ->%d<-\n",info->audio_format.type);
    printf("\t audio sample_rate->%d<-\n",info->audio_format.sample_rate);
    printf("\t audio sample_fmt ->%d<-\n",info->audio_format.sample_fmt);
    printf("\t audio channels   ->%d<-\n",info->audio_format.channels);
    printf("\t audio effect     ->%d<-\n",info->audio_format.effect);
    printf("\t time_segment     ->%d<-\n",info->time_segment );
    printf("\t time_backward    ->%d<-\n",info->time_backward );
    printf("\t keep frame time  ->%d<-\n", info->s32KeepFramesTime);
    printf("\t time_format      ->");
    puts(info->time_format);
    printf("\t suffix           ->");
    puts(info->suffix);
    printf("\t av_format        ->");
    puts(info->av_format);
    printf("\t dir_prefix       ->");
    puts(info->dir_prefix);
    printf("recorder info over\n");
}
int vplay_report_event (vplay_event_enum_t event , char *dat)  {
    int len = strlen(dat);
    if(len >= EVENT_BUF_MAX  || event >= VPLAY_CHANNEL_MAX || event <= VPLAY_EVENT_NONE ){
        printf("report event error ->%d %s\n",event ,dat);
        return -1;
    }
    static int index = 0 ;
    vplay_event_info_t  eventInfo ;
    memset(&eventInfo , 0, sizeof(vplay_event_info_t));
    memcpy( eventInfo.buf , dat ,len+1);
    eventInfo.index = index++;
    eventInfo.type =  event ;
    return event_send(EVENT_VPLAY ,(char *)&eventInfo,sizeof(vplay_event_info_t) );
}
VRecorder *vplay_new_recorder(VRecorderInfo *info){
    TIMER_BEGIN(recoder);
    assert(info != NULL);
    vplay_show_recorder_info(info);
    VRecorder *recorder =(VRecorder *) malloc(sizeof(VRecorder));
    if(recorder == NULL)
        return NULL;
    QRecorder *recInst = new QRecorder(info);
    if (recInst == NULL ){
        LOGGER_ERR("Get recorder inst error\n");
        free(recorder);
        return NULL;
    }
    if (recInst->Prepare() == false){
        LOGGER_ERR("Get recorder inst error\n");
        free(recorder);
        return NULL;
    }
    recorder->inst = (void *) recInst ;
    TIMER_END(recoder);
    return recorder ;
}
int vplay_delete_recorder(VRecorder *rec){
    TIMER_BEGIN(recoder);
    assert(rec != NULL);
    QRecorder *recInst = (QRecorder *)rec->inst ;
    recInst->SetWorkingState(ThreadingState::Stop);
    LOGGER_TRC("delete recorder inst\n");
    delete recInst ;
    free(rec);
    TIMER_END(recoder);
    return 1;
}
int vplay_start_recorder(VRecorder *rec){
    TIMER_BEGIN(recoder);
    assert(rec != NULL);
    QRecorder *recInst = (QRecorder *)rec->inst ;
    LOGGER_WARN("recorder current state->%d\n",recInst->GetWorkingState());
    recInst->SetWorkingState(ThreadingState::Start);
    LOGGER_WARN("recorder start finish ->%d\n",recInst->GetWorkingState());
    TIMER_END(recoder);
    return 1;
}
int vplay_stop_recorder(VRecorder *rec){
    TIMER_BEGIN(recoder);
    assert(rec != NULL);
    QRecorder *recInst = (QRecorder *)rec->inst ;
    recInst->SetWorkingState(ThreadingState::Pause);
    LOGGER_INFO("recorder stop finish ->%d\n",recInst->GetWorkingState());
    TIMER_END(recoder);
    return 1;
}

int vplay_alert_recorder(VRecorder *rec,char *alert){
    assert(rec != NULL);
    QRecorder *recInst = (QRecorder *)rec->inst ;
    if( recInst->Alert(alert) )
        return 1;
    else {
        return -1;
    }
}
int vplay_mute_recorder(VRecorder *rec, VPlayMediaType type, int enable)
{
    assert(rec != NULL);
    QRecorder *recInst = (QRecorder *)rec->inst ;
    if(enable ==1 ){
        LOGGER_DBG("mute media ->%d\n",type);
        recInst->SetMute(type,true);
    }
    else {
        LOGGER_DBG("unmute media ->%d\n",type);
        recInst->SetMute(type,false);
    }
    return 1;
}
int vplay_set_recorder_udta(VRecorder *rec, char *buf, int size){
    assert(rec != NULL);
    assert(buf != NULL);
    assert(size >= 0);
    int ret = 0;
    QRecorder *recInst = (QRecorder *)rec->inst ;
    return recInst->SetMediaUdta(buf,size);
}
int vplay_control_recorder(vplay_recorder_inst_t *rec, vplay_ctrl_action_t action, void *para){
    assert(rec != NULL);
    int ret = 0;
    QRecorder *recInst = (QRecorder *)rec->inst ;
    ThreadingState curState = recInst->GetWorkingState();
    switch(action){
        case VPLAY_RECORDER_SLOW_MOTION:
            if(curState != ThreadingState::Stop && curState != ThreadingState::Pause){
                LOGGER_ERR("you must stop or pause rocord process first\n");
            }
            LOGGER_INFO("slow motion  mode\n");
            ret = recInst->Muxer->SetSlowMotion(*(int *)para);
            break;
        case VPLAY_RECORDER_FAST_MOTION:
            if(curState != ThreadingState::Stop && curState != ThreadingState::Pause){
                LOGGER_WARN("you must stop or pause rocord process first\n");
            }
            LOGGER_INFO("fast motion mode\n");
            ret = recInst->Muxer->SetFastMotion(*(int *)para);
            break;
        case VPLAY_RECORDER_START:
            LOGGER_INFO("recorder current state->%d\n", recInst->GetWorkingState());
            ret = recInst->SetWorkingState(ThreadingState::Start);
            LOGGER_INFO("recorder start finish ->%d\n", recInst->GetWorkingState());

            break;
        case VPLAY_RECORDER_STOP:
            ret = recInst->SetWorkingState(ThreadingState::Pause);
            LOGGER_INFO("recorder stop finish ->%d\n", recInst->GetWorkingState());
            break;
        case VPLAY_RECORDER_MUTE:
            recInst->SetMute(VPLAY_MEDIA_TYPE_AUDIO, true);
            ret = 1;
            break;
        case VPLAY_RECORDER_UNMUTE:
            recInst->SetMute(VPLAY_MEDIA_TYPE_AUDIO, false);
            ret = 1;
            break;
        case VPLAY_RECORDER_SET_UDTA:
            ret = recInst->SetMediaUdta(((vplay_udta_info_t *)para)->buf, ((vplay_udta_info_t *)para)->size);
            break;
        case VPLAY_RECORDER_FAST_MOTION_EX:
            if(curState == ThreadingState::Start){
                LOGGER_WARN("you must stop or pause rocord process first, curState:%d\n", curState);
            }
            if (para == NULL)
            {
                LOGGER_ERR("Set Fast Motion By Player para error!\n");
                break;
            }
            LOGGER_INFO("fast motion mode by player ->%d\n", *(int *)para);
            ret = recInst->Muxer->SetFastMotionEx(*(int *)para);
            break;
        case VPLAY_RECORDER_NEW_SEGMENT:
            if(curState == ThreadingState::Start){
                ret = recInst->StartNewSegment((VRecorderInfo *)para);
                break;
            }
            LOGGER_ERR("Set new segment error!\n");
            break;
        case VPLAY_RECORDER_GET_AUDIO_CHANNEL_INFO:
            {
                if (para == NULL)
                {
                    LOGGER_ERR("Get audio channel info error!\n");
                    break;
                }
                ret = recInst->GetAudioChannelInfo((ST_AUDIO_CHANNEL_INFO *)para);
                break;
            }
        case VPLAY_RECORDER_SET_UUID_DATA:
            {
                if (para == NULL)
                {
                    LOGGER_ERR("Set uuid data error!\n");
                    break;
                }
                ret = recInst->Muxer->SetUUIDData((char*)para);
                break;
            }
        default:
            break;
    }
    return ret;
}

void vplay_set_cache_mode(vplay_recorder_inst_t *rec, int mode) {
    assert(rec != NULL);
    QRecorder *recInst = (QRecorder *)rec->inst ;
    if(!mode)
        recInst->SetCaheMode(false);
    else
        recInst->SetCaheMode(true);
}
