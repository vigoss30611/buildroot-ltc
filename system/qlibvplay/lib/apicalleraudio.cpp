#include <apicalleraudio.h>
#include <unistd.h>
#include <assert.h>
#include <fr/libfr.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define AUDIO_RAW_PCM_BUFFER_SIZE       0x2800     //10k
#define ENCODE_SAMPLES_NUMBER           1024      // encoder need 1024 samples to encode a frame at least
#define SAMPLE_RATE_COUNT               11

static char audioRawBuffer[AUDIO_RAW_PCM_BUFFER_SIZE];
static int sampleRateList[SAMPLE_RATE_COUNT] = {8000, 11025, 12000,16000, 22025, 24000, 32000, 44100, 48000, 96000, 192000};
static char muteBuffer[AUDIO_RAW_PCM_BUFFER_SIZE] = {0};

ThreadingState ApiCallerAudio::WorkingThreadTask(void )
{
    struct fr_buf_info  fr ;
    fr.virt_addr =  audioRawBuffer;
    int ret = 0, encodeCnt = 0, frameSize = 0;;
    codec_addr_t addr;
    uint64_t frameTime = 0;

    ret = audio_get_frame(this->AudioHandler,&this->FrFrame);
    if(ret < 0 ){
        LOGGER_WARN("get audio raw frame error ->%d %d\n",ret,this->AudioHandler);
        usleep(1000);
        return ThreadingState::Keep;
    }
    #if 0  //raw audio crc32 print
    printf("audio raw frame -> %8lld : 0x%8X :%5d\n",this->FrFrame.timestamp, calCrc32((char *)this->FrFrame.virt_addr, this->FrFrame.size), this->FrFrame.size);
    #endif

#if 0 //fix audio timestamp error
    if(this->LastRawTimestamp == MEDIA_MUXER_HUGE_TIMESDTAMP){
        this->LastRawTimestamp =  this->FrFrame.timestamp;
        fr.timestamp =  this->FrFrame.timestamp;
    }
    else {
        if(this->FrFrame.timestamp <= this->LastRawTimestamp ||
                (this->FrFrame.timestamp - this->LastRawTimestamp) > 1000000UL){
            LOGGER_WARN("auido frame timestamp erro->%llu %llu\n",this->FrFrame.timestamp, this->LastRawTimestamp);
            fr.timestamp = this->LastRawTimestamp + 5;
        }
        else{
            fr.timestamp = this->FrFrame.timestamp;
        }
        this->LastRawTimestamp = fr.timestamp;
    }
#else
    fr.timestamp = this->FrFrame.timestamp;
#endif

    encodeCnt = (this->stAudioChannelFmt.sample_size + ENCODE_SAMPLES_NUMBER - 1) / ENCODE_SAMPLES_NUMBER;
    encodeCnt = (encodeCnt <= 0) ? 1 : encodeCnt;
    frameSize = this->FrFrame.size / encodeCnt;
    frameTime = fr.timestamp;

    if (this->MuteEnable.load())
    {
        if (this->MuteBuffer != NULL)
        {
            addr.in = this->MuteBuffer;
        }
        else
        {
            if (frameSize > AUDIO_RAW_PCM_BUFFER_SIZE)
            {
                LOGGER_WARN("mute buffer err: %d\n", ret);
                audio_put_frame(this->AudioHandler, &this->FrFrame);
                return ThreadingState::Pause;
            }
            else
            {
                addr.in = muteBuffer;
            }
        }
    }
    else
    {
        addr.in = this->FrFrame.virt_addr;
    }

    for (int j = 0; j < encodeCnt; j++)
    {
        fr.timestamp = frameTime + j * ENCODE_SAMPLES_NUMBER * 1000 / this->AudioOutInfo.sample_rate;
        addr.in = (void*)((uint)addr.in + j * frameSize);
        addr.len_in = frameSize;
        addr.out = fr.virt_addr;
        addr.len_out = AUDIO_RAW_PCM_BUFFER_SIZE;

        ret = codec_encode(this->AudioCodecs, &addr);
        if (ret < 0) {
            LOGGER_WARN("codec_encode err: %d\n", ret);
            audio_put_frame(this->AudioHandler,&this->FrFrame);
            return ThreadingState::Pause;
        }
        if (this->LastValidEncodeTimestamp == MEDIA_MUXER_HUGE_TIMESDTAMP) {
        //    printf("first timestamp ->%8lld\n",fr.timestamp);
            this->LastValidEncodeTimestamp = fr.timestamp;
        }
        if (ret == 0 ) {
            if(this->LastValidEncodeFlag == true) {
                //last frame data ok
                this->LastValidEncodeTimestamp = fr.timestamp;
            }
            else {
                //last frame data null

            }
            this->LastValidEncodeFlag =  false;
            audio_put_frame(this->AudioHandler,&this->FrFrame);
            return ThreadingState::Keep;
        }
        else {
            if (this->LastValidEncodeFlag == true) {
                //last frame data ok
                this->LastValidEncodeTimestamp =  fr.timestamp;
            }
            else {
                //last frame data null
                 fr.timestamp = this->LastValidEncodeTimestamp;

            }
            this->LastValidEncodeFlag =  true;
        }

        fr.timestamp = this->LastValidEncodeTimestamp;
        fr.size = ret;
#if 0  //audio adts crc32 print
        printf("audio enc frame->%8llu : 0x%8X :%5d %llu %d\n",fr.timestamp, calCrc32((char *)fr.virt_addr, fr.size), fr.size, this->LastValidEncodeTimestamp,ret);
        printf("encode adts size ->%4d ", fr.size);
#endif
        if (this->LastTs == MEDIA_MUXER_HUGE_TIMESDTAMP) {
            this->LastTs = fr.timestamp;
        }
        else {
            if(fr.timestamp <= this->LastTs){
                LOGGER_WARN("###api audio fix timestamp  ->%8lld %8lld\n",
                        fr.timestamp, this->LastTs);
                fr.timestamp = ++this->LastTs;
            }
#if 0
            else{
                if(fr.timestamp - this->LastTs > 1000){
                    LOGGER_WARN("###api audio interval error  ->%8lld %8lld\n",
                            fr.timestamp, this->LastTs);
                }
            }
#endif
        }

        if (this->PFrFifo->PushItem(&fr) < 0) {
            LOGGER_TRC("push audio error\n");
        }
#if 1
        if (this->FifoAutoFlush == true) {
            int queueSize = this->PFrFifo->GetQueueSize();
            if (queueSize > 5) {
                if ((queueSize % 3) == 0)
                LOGGER_INFO("audio FULL->t:%3X ts:%8lld size:%6d ct:%5d queue:%2d\n",
                        this->FrFrame.priv,
                        this->FrFrame.timestamp,
                        this->FrFrame.size,
                        this->PFrFifo->GetCurCacheTimeLen(),
                        queueSize);
            }
        }
#endif
        this->FrameNum++;
        if (this->FrameNum % 240  == 0) {
            LOGGER_INFO("-->audio-> %4d %8lld %6lld %5dKb:%dB\n",
                    this->PFrFifo->GetQueueSize(),
                    fr.timestamp,
                    this->FrameNum,
                    fr.size/1024,
                    fr.size%1024);
        }
    }
    audio_put_frame(this->AudioHandler, &this->FrFrame);

    return ThreadingState::Keep;

}

ApiCallerAudio::ApiCallerAudio(char *channel, audio_info_t *info, ST_AUDIO_CHANNEL_INFO *pstChannelInfo):ThreadCore()
{
    COMPILE_ASSERT(ST_AUDIO_CHANNEL_INFO, audio_fmt_t);

    assert(channel != NULL);
    assert(info != NULL);
    assert(strlen(channel) < STRING_PARA_MAX_LEN  );
    assert(pstChannelInfo != NULL);
    memcpy(this->DefChannel,channel ,strlen(channel) + 1);
    this->AudioOutInfo = *info;
    this->stAudioChannelFmt = *(struct audio_format*)pstChannelInfo;
    this->AudioHandler = -1;
    this->AudioCodecs = NULL;
    this->FrameRate = 0 ;
    fr_INITBUF(&this->FrFrame,NULL);
    this->FrameNum =  0;
    this->FifoAutoFlush  =  false;
    this->SetName((char *)"rec_audio");
    this->MuteEnable.store(false);

    this->MuteBuffer = NULL;
    this->LastValidEncodeTimestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastValidEncodeFlag = true;
    this->LastRawTimestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
}

ApiCallerAudio::~ApiCallerAudio()
{
    LOGGER_DBG("%s deinit+++++\n",__FUNCTION__);
    this->UnPrepare();
    if(this->MuteBuffer != NULL){
        free(this->MuteBuffer);
    }
    LOGGER_DBG("deinit audio ->%p\n",this);
}

bool ApiCallerAudio::CheckAudioFmt(audio_fmt_t *pstInfo)
{
    if (pstInfo == NULL)
        return false;

    if (pstInfo->bitwidth % 8 != 0)
    {
        LOGGER_ERR("Audio bitwidth:%d error!\n", pstInfo->bitwidth);
        return false;
    }

    if ((pstInfo->channels != 1) && (pstInfo->channels != 2))
    {
        LOGGER_ERR("Audio channels:%d error!\n", pstInfo->channels);
        return false;
    }

    if ((pstInfo->sample_size % 1024 != 0) && (pstInfo->sample_size % 1000 != 0))
    {
        LOGGER_ERR("Audio sample size:%d error!\n", pstInfo->sample_size);
        return false;
    }

    for (int i = 0; i < SAMPLE_RATE_COUNT; i++)
    {
        if (pstInfo->samplingrate == sampleRateList[i])
        {
            return true;
        }
    }

    return false;
}

bool ApiCallerAudio::Prepare(void){
    codec_info_t codecfmt;
    int ret = 0 ;
    if (!CheckAudioFmt(&this->stAudioChannelFmt))
    {
        memset(&this->stAudioChannelFmt , 0, sizeof(struct audio_format));
        if(strstr(this->DefChannel,"btcodec") != NULL) {
            this->stAudioChannelFmt.channels     =  1 ;
            this->stAudioChannelFmt.bitwidth     =  16 ;
            this->stAudioChannelFmt.samplingrate =  8000 ;
            this->stAudioChannelFmt.sample_size  =  1000;
        }
        else {
            this->stAudioChannelFmt.channels     =  2 ;
            this->stAudioChannelFmt.bitwidth     =  32 ;
            this->stAudioChannelFmt.samplingrate =  16000 ;
            this->stAudioChannelFmt.sample_size  =  1024;

        }
    }
    LOGGER_TRC("audio channel ->%s\n",this->DefChannel);
    this->AudioHandler = audio_get_channel(this->DefChannel, &this->stAudioChannelFmt, CHANNEL_BACKGROUND);
    if (this->AudioHandler < 0) {
        LOGGER_ERR("Open audio channel error\n");
        goto prepare_error_handler ;
    }
    LOGGER_TRC("audio handler ->%d\n", this->AudioHandler);

    ret = audio_get_format(this->DefChannel, &this->stAudioChannelFmt);
    if (ret <  0) {
        LOGGER_ERR("Get audio format error, ret:%d\n", ret);
        goto prepare_error_handler ;
    }
    LOGGER_TRC("codec info -> %d %d %d %d %d\n",\
            this->AudioOutInfo.type,\
            this->AudioOutInfo.effect,\
            this->AudioOutInfo.channels,\
            this->AudioOutInfo.sample_rate,\
            this->AudioOutInfo.sample_fmt
            );

    LOGGER_INFO("audio channel format ->%d %d %d %d\n",
            this->stAudioChannelFmt.channels ,
            this->stAudioChannelFmt.bitwidth ,
            this->stAudioChannelFmt.samplingrate,
            this->stAudioChannelFmt.sample_size
            );
#if 1
    codecfmt.in.codec_type = AUDIO_CODEC_PCM;
    codecfmt.in.effect = 0;
    codecfmt.in.channel = this->stAudioChannelFmt.channels;
    codecfmt.in.sample_rate = this->stAudioChannelFmt.samplingrate;
    codecfmt.in.bitwidth = this->stAudioChannelFmt.bitwidth;
    codecfmt.in.bit_rate = codecfmt.in.channel * codecfmt.in.sample_rate \
                            * GET_BITWIDTH(codecfmt.in.bitwidth);
/*
 *
    AUDIO_CODEC_TYPE_PCM = 0 ,
    AUDIO_CODEC_TYPE_G711U,
    AUDIO_CODEC_TYPE_G711A,
    AUDIO_CODEC_TYPE_AAC,
    AUDIO_CODEC_TYPE_ADPCM_G726,
    AUDIO_CODEC_TYPE_MP3,
    AUDIO_CODEC_TYPE_SPEEX,
typedef enum AUDIO_CODEC_TYPE {
    AUDIO_CODEC_PCM,
    AUDIO_CODEC_G711U,
    AUDIO_CODEC_G711A,
    AUDIO_CODEC_ADPCM,
    AUDIO_CODEC_G726,
    AUDIO_CODEC_MP3,
    AUDIO_CODEC_SPEEX,
    AUDIO_CODEC_AAC,
    AUDIO_CODEC_MAX
} CODEC_TYPE;
 */
    codecfmt.out.effect = this->AudioOutInfo.effect;
    codecfmt.out.channel = this->AudioOutInfo.channels;
    codecfmt.out.sample_rate = this->AudioOutInfo.sample_rate;
    codecfmt.out.bitwidth = (this->AudioOutInfo.sample_fmt + 1) * 8;        // convert to bitwidth
    switch(this->AudioOutInfo.type){
        case  AUDIO_CODEC_TYPE_PCM :
            LOGGER_DBG("audio encode pcm\n");
            codecfmt.out.codec_type = AUDIO_CODEC_PCM;
            break;
        case  AUDIO_CODEC_TYPE_G711U :
            LOGGER_DBG("audio encode g711u\n");
            codecfmt.out.codec_type = AUDIO_CODEC_G711U;
            codecfmt.out.bit_rate = codecfmt.out.channel * codecfmt.out.sample_rate \
                                    * GET_BITWIDTH(codecfmt.out.bitwidth);
            codecfmt.out.bit_rate /= 2;
            break;
        case  AUDIO_CODEC_TYPE_G711A :
            LOGGER_DBG("audio encode g711a\n");
            codecfmt.out.codec_type = AUDIO_CODEC_G711A;
            codecfmt.out.bit_rate = codecfmt.out.channel * codecfmt.out.sample_rate \
                            * GET_BITWIDTH(codecfmt.out.bitwidth);
            codecfmt.out.bit_rate /= 2;
            break;
        case  AUDIO_CODEC_TYPE_AAC :
            LOGGER_DBG("audio encode aac\n");
            codecfmt.out.codec_type = AUDIO_CODEC_AAC;
            codecfmt.out.bit_rate = codecfmt.out.channel * codecfmt.out.sample_rate \
                                    * GET_BITWIDTH(codecfmt.out.bitwidth);
            codecfmt.out.bit_rate /= 4;
            break;
        case  AUDIO_CODEC_TYPE_ADPCM_G726 :
            LOGGER_DBG("audio encode g726\n");
            codecfmt.out.codec_type = AUDIO_CODEC_G726;
            break;
        case  AUDIO_CODEC_TYPE_MP3 :
            LOGGER_DBG("audio encode mp3\n");
            codecfmt.out.codec_type = AUDIO_CODEC_MP3;
            break;
        case  AUDIO_CODEC_TYPE_SPEEX :
            LOGGER_DBG("audio encode speex\n");
            codecfmt.out.codec_type = AUDIO_CODEC_SPEEX;
            break;

    }
    this->AudioCodecs = codec_open(&codecfmt);
    if (this->AudioCodecs == NULL ){
        LOGGER_ERR("get audio codec error\n");
        goto prepare_error_handler ;
    }

    //this->QAudioCodecs->ShowSupportAudioCodecsInfo();
    int temp;
    temp = this->stAudioChannelFmt.bitwidth == 24 ? 32 : this->stAudioChannelFmt.bitwidth;
    this->FrameRate = (this->stAudioChannelFmt.samplingrate * this->stAudioChannelFmt.channels * temp) / \
        (this->stAudioChannelFmt.sample_size * 8 * 8);
    this->FrameSize = this->stAudioChannelFmt.sample_size * 8;
    LOGGER_TRC("audio frame rate  ->%d\n",this->FrameRate);
#endif
    return true ;

prepare_error_handler :
//    this->SetWorkingState(ThreadingState::Stop);
    if(this->AudioHandler >= 0){
        LOGGER_ERR("close audio channel ->%s : %d\n",this->DefChannel,this->AudioHandler);
        audio_put_channel(this->AudioHandler);
        this->AudioHandler = -1;
    }
    if(this->AudioCodecs ){
        LOGGER_ERR("delete audio codecs\n");
        codec_close(this->AudioCodecs);
        this->AudioCodecs = NULL ;
    }
    return false ;
}
bool ApiCallerAudio::ApiCallerAudio::UnPrepare(void){
    struct fr_buf_info  fr;
//    char audioRawBuffer[AUDIO_RAW_PCM_BUFFER_SIZE];
    fr.virt_addr =  audioRawBuffer;
    int ret = 0, length;
    codec_addr_t addr;

    if(this->AudioHandler >= 0){
        LOGGER_DBG("close audio channel ->%s : %d\n", this->DefChannel, this->AudioHandler);
        audio_put_channel(this->AudioHandler);
        this->AudioHandler = -1;
    }

    if(this->AudioCodecs ){
        LOGGER_DBG("free audio codecs \n");
#if 1
        LOGGER_TRC("delete audio codecs\n");
        addr.out = fr.virt_addr;
        addr.len_out = 0x2000;
        do {
            ret = codec_flush(this->AudioCodecs, &addr, &length);
            if (ret == CODEC_FLUSH_ERR)
                break;
            /* TODO weather you need codec data least in codec list */
        } while (ret == CODEC_FLUSH_AGAIN);
#endif
        codec_close(this->AudioCodecs);
        this->AudioCodecs = NULL;
    }
}
int ApiCallerAudio::GetFrameRate(void) {
    return this->FrameRate ;
}
int ApiCallerAudio::GetFrameSize(void) {
    return this->FrameSize ;
}
bool ApiCallerAudio::ThreadPreStartHook(void){
    LOGGER_DBG("pre start audio thread hook\n");
    fr_INITBUF(&this->FrFrame,NULL);
    this->LastValidEncodeTimestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastValidEncodeFlag = true;
    this->LastRawTimestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
}
bool ApiCallerAudio::ThreadPostPauseHook(void){
    LOGGER_INFO("post pause audio thread hook\n");
    if(this->FifoAutoFlush){
        LOGGER_INFO("*****************audio  clear fifo\n");
        this->PFrFifo->Flush();
        fr_INITBUF(&this->FrFrame,NULL);
    }
    this->LastValidEncodeTimestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastValidEncodeFlag = true;
    this->LastRawTimestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
}
bool ApiCallerAudio::ThreadPostStopHook(void){
    LOGGER_INFO("post stop audio thread hook\n");
    if(this->FifoAutoFlush){
        LOGGER_INFO("audio clear fifo\n");
        this->PFrFifo->Flush();
        fr_INITBUF(&this->FrFrame,NULL);
    }
    this->LastValidEncodeTimestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastValidEncodeFlag = true;
    this->LastRawTimestamp = MEDIA_MUXER_HUGE_TIMESDTAMP;
    this->LastTs = MEDIA_MUXER_HUGE_TIMESDTAMP;
}
void ApiCallerAudio::SetFifoAutoFlush(bool enable){
    this->FifoAutoFlush =  enable ;
}

void ApiCallerAudio::SetMute(bool enable)
{
    if (enable == true)
    {
        int s32BufferSize = this->stAudioChannelFmt.channels *\
                this->stAudioChannelFmt.bitwidth / 8 * this->stAudioChannelFmt.sample_size;
        if ((s32BufferSize > AUDIO_RAW_PCM_BUFFER_SIZE) &&\
               (this->MuteBuffer == NULL))
        {
            LOGGER_INFO("malloc mute buffer:%d\n", s32BufferSize);
            this->MuteBuffer =  (char *)malloc(s32BufferSize);
            if (this->MuteBuffer == NULL)
            {
                LOGGER_ERR("can not malloc mute buffer, set mute failed, buf size:%d\n",
                    s32BufferSize);
                return;
            }
            memset(this->MuteBuffer, 0, s32BufferSize);
        }
        else if (this->MuteBuffer)
        {
            LOGGER_INFO("mute buffer already ready\n");
        }
    }
    LOGGER_INFO("mute audio ->%d\n", enable);
    this->MuteEnable.store(enable);
}

bool ApiCallerAudio::GetAudioChannelInfo(ST_AUDIO_CHANNEL_INFO *pstInfo)
{
    if (pstInfo == NULL)
    {
        return false;
    }
    if (!CheckAudioFmt(&this->stAudioChannelFmt))
    {
        return false;
    }
    memcpy(pstInfo, &this->stAudioChannelFmt, sizeof(this->stAudioChannelFmt));
    return true;
}
