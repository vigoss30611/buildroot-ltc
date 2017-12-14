#include <qaudioplayer.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define   VPLAY_PLAYER_AUDIO_RETRY_TIMES     1000
#define   AUDIO_RESYNC_THREADHOLD            2000
#define   BUFFER_THRESHOLD_FOR_WAIT          10

ThreadingState QAudioPlayer::WorkingThreadTask(void ){
    int ret = 0;

    if(this->PlaySpeed == 0){
        LOGGER_WARN("speed play, audio abort\n");
        return ThreadingState::Pause;
    }
    if(this->AudioChannelHander < 0 ){
        usleep(1000);
        return ThreadingState::Keep;
    }
    if(this->PFrFifo->GetItem(&this->FrFrame) <= 0){
        //LOGGER_DBG("no audio data in fifo\n");
        usleep(5000);
        return ThreadingState::Keep;
    }
#if 0
    LOGGER_DBG("get audio stream ->%5d %8lld %8lld %3d\n", this->FrFrame.serial, this->FrFrame.timestamp, this->PVPlayTs->GetTs(),
            this->PFrFifo->GetQueueSize());
#endif
    if(this->FrFrame.size <= 0 || this->FrFrame.virt_addr == NULL){
        //this->PFrFifo->PutItem(&this->FrFrame);
        usleep(1000);
        return ThreadingState::Keep;
    }
    if(this->FrFrame.priv == MEDIA_END_DUMMY_FRAME){
        this->PFrFifo->PutItem(&this->FrFrame);
        LOGGER_INFO("###read the audio dummy stream\n");
        this->PVPlayTs->EnableAutoUpdate();
        return ThreadingState::Keep;
    }

    if (this->FrFrame.timestamp + AUDIO_RESYNC_THREADHOLD < this->PVPlayTs->GetTs(this->PlaySpeed))
    {
        this->PFrFifo->PutItem(&this->FrFrame);
        LOGGER_WARN("drop expired audio frame fr:%llu,play position:%llu\n",
            this->FrFrame.timestamp,
            this->PVPlayTs->GetTs(this->PlaySpeed));
        return ThreadingState::Keep;
    }
    this->PVPlayTs->UpdateTs(this->FrFrame.timestamp);
#if 0
    LOGGER_INFO("write audio stream ->%5d %8llu %8llu %3d\n", this->FrFrame.serial, this->FrFrame.timestamp, this->PVPlayTs->GetTs(this->PlaySpeed),
            this->PFrFifo->GetQueueSize());
#endif
    uint64_t oldTs = get_timestamp();
    ret = audio_write_frame(this->AudioChannelHander,\
            (char *)this->FrFrame.virt_addr,
            this->FrFrame.size);
    uint64_t newTs =  get_timestamp();
    this->PFrFifo->PutItem(&this->FrFrame);
    if(ret < 0){
        LOGGER_WARN("write audio frame error\n");
        this->AudioChannelHander = -1;
        this->AudioInfo->SetUpdate(true);
        this->UpdateAudioInfo();
        usleep(1000);
        return ThreadingState::Keep;
    }
    if (((newTs - oldTs) <= 5) && (this->PFrFifo->GetQueueSize() > BUFFER_THRESHOLD_FOR_WAIT)){//if necessary ,monitor every audio frame
        //first write audio buffer 
        codec_fmt_t  audioCodecFmt;
        audioCodecFmt = this->AudioInfo->GetOutType();
        uint64_t interval = this->FrFrame.size * 1000 * 8 /
            (audioCodecFmt.sample_rate * audioCodecFmt.channel * audioCodecFmt.bitwidth);
        interval = interval - (newTs - oldTs);
        interval = interval *7 /10;
        //LOGGER_DBG("!!!!!!!!!!!!!wait to feed audio data ->%d\n", interval);
        usleep(interval * 1000);
    }
#if 0
    printf("++++++++write audio time ->%8lld  %d %d  %8lld\n", newTs - oldTs,
            this->AudioChannelHander,
            this->FrFrame.size,
            this->FrFrame.timestamp);
#endif
    return ThreadingState::Keep;
}
QAudioPlayer::QAudioPlayer(AudioStreamInfo *info):ThreadCore(){
    this->AudioChannelHander = -1;
    this->AudioInfo = info;
    this->PlaySpeed = 1;
    this->SetName((char *)"aplayer");
    this->PFrFifo =  NULL;
    memset(&this->FrFrame, 0, sizeof(struct fr_buf_info));
}
QAudioPlayer::~QAudioPlayer(){
    LOGGER_DBG("deinit audio player -----\n");
    if(this->GetWorkingState() != ThreadingState::Stop){
        LOGGER_DBG("release audio player thread\n");
        this->SetWorkingState(ThreadingState::Stop);
    }
    this->Release();
}
bool QAudioPlayer::ThreadPreStartHook(void){
    return true;
}
bool QAudioPlayer::ThreadPostPauseHook(void){
    return true;
}
bool QAudioPlayer::ThreadPostStopHook(void){
    if(this->AudioChannelHander >= 0){
        LOGGER_INFO("release audio channel\n");
        audio_put_channel(this->AudioChannelHander);
        this->AudioChannelHander = -1;
    }
    return true;
}
bool QAudioPlayer::Prepare(void){
    if(this->PFrFifo == NULL){
        LOGGER_ERR("audio fifo is null, fault error\n");
        assert(1);
    }
    memset(&this->FrFrame,0,sizeof(struct fr_buf_info));

    return true;
}
bool QAudioPlayer::UnPrepare(void){
    return true;
}
void QAudioPlayer::Release(void){
    if(this->AudioChannelHander >= 0){
        audio_put_channel(this->AudioChannelHander);
        this->AudioChannelHander = -1;
    }
}
bool QAudioPlayer::UpdateAudioInfo(void){
    LOGGER_DBG("audio update audio info\n");
    codec_fmt_t audioCodecFmt;
    while(this->AudioInfo->GetUpdate() == false){
        LOGGER_INFO("wait for audio codec info\n")
        usleep(10000);
    }
    audioCodecFmt = this->AudioInfo->GetOutType();
    audio_fmt_t  audioChannelFmt;
    audioChannelFmt.channels = audioCodecFmt.channel;
    audioChannelFmt.bitwidth = audioCodecFmt.bitwidth;
    audioChannelFmt.sample_size = 1024;
    audioChannelFmt.samplingrate  = audioCodecFmt.sample_rate;

    char channel[VPLAY_CHANNEL_MAX];
    this->AudioInfo->GetChannel(channel, VPLAY_CHANNEL_MAX-1);
    int len = strlen(channel);
    if(len >= VPLAY_CHANNEL_MAX){
        LOGGER_ERR("audio channel(%d) is too long,error\n", len);
        return false ;
    }
#if 0
    if(this->AudioChannelHander >= 0){
        audio_put_channel(this->AudioChannelHander);
        this->AudioChannelHander = -1;
    }
#endif
    if(this->AudioChannelHander < 0){
        this->AudioChannelHander =
            audio_get_channel(channel, &audioChannelFmt, CHANNEL_BACKGROUND);
        if(this->AudioChannelHander <  0){
            LOGGER_ERR("open audio play channel error\n");
            return false ;
        }
        else{
            LOGGER_INFO("open audio channel success ->%d %s\n",
                    this->AudioChannelHander,
                    channel);
            audio_get_format(channel, &audioChannelFmt);
            LOGGER_DBG("get audio format ->ch:%d bt:%d sam:%d size:%d\n",
                    audioChannelFmt.channels,
                    audioChannelFmt.bitwidth,
                    audioChannelFmt.samplingrate,
                    audioChannelFmt.sample_size);
        }
    }

}
bool QAudioPlayer::SetPlaySpeed(int speed){
    if(speed == 1 ){
        this->PlaySpeed = speed ;
        return true ;
    }
    else {
        LOGGER_WARN("audio not support speed play\n");
        this->PlaySpeed = 0;
    }
}
