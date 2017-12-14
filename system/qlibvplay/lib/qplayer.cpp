#include <qplayer.h>
#include <string.h>
#include <assert.h>
#include <sys/prctl.h>
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>
#include <qsdk/vplay.h>

#define MAX_CACHE_FRAME_NUMBER  30
#define ADTS_HEADER_SIZE   7
#define AOT_PROFILE_CTRL
#define DEFAULT_FPS 30

//#define  VPLAY_DEBUG_AUDIO_DUMP
//#define  VPLAY_DEBUG_VIDEO_DUMP
//#define  VPLAY_PLAY_VIDEO_ONLY
int aac_decode_extradata(adts_context_t *adts, char *pbuf, int bufsize)

{
    int aot, aotext, samfreindex;
    int i, channelconfig;
    char *p = pbuf;

    if (!adts || !pbuf || bufsize<2)
    {
        return -1;
    }
    aot = (p[0]>>3)&0x1f;
    if (aot == 31)
    {
        aotext = (p[0]<<3 | (p[1]>>5)) & 0x3f;
        aot = 32 + aotext;
        samfreindex = (p[1]>>1) & 0x0f;

        if (samfreindex == 0x0f)
        {
            channelconfig = ((p[4]<<3) | (p[5]>>5)) & 0x0f;
        }
        else
        {
            channelconfig = ((p[1]<<3)|(p[2]>>5)) & 0x0f;
        }
    }
    else
    {
        samfreindex = ((p[0]<<1)|p[1]>>7) & 0x0f;
        if (samfreindex == 0x0f)
        {
            channelconfig = (p[4]>>3) & 0x0f;
        }
        else
        {
            channelconfig = (p[1]>>3) & 0x0f;
        }
    }

#ifdef AOT_PROFILE_CTRL
    if (aot < 2) aot = 2;
#endif
    adts->objecttype = aot-1;
    adts->sample_rate_index = samfreindex;
    adts->channel_conf = channelconfig;
    adts->write_adts = 1;

    return 0;
}
int aac_set_adts_head(adts_context_t *acfg, char *buf, int size)

{
    char byte;

    if (size < ADTS_HEADER_SIZE)
    {
        return -1;
    }
    memset(buf, 0 ,ADTS_HEADER_SIZE);

    buf[0] = 0xff;
    buf[1] = 0xf1;
    byte = 0;
    byte |= (acfg->objecttype & 0x03) << 6;
    byte |= (acfg->sample_rate_index & 0x0f) << 2;
    byte |= (acfg->channel_conf & 0x07) >> 2;
    buf[2] = byte;
    byte = 0;
    byte |= (acfg->channel_conf & 0x07) << 6;
    byte |= (ADTS_HEADER_SIZE + size) >> 11;
    buf[3] = byte;
    byte = 0;
    byte |= (ADTS_HEADER_SIZE + size) >> 3;
    buf[4] = byte;
    byte = 0;
    byte |= ((ADTS_HEADER_SIZE + size) & 0x7) << 5;
#if 0 //0x7FF
    byte |= (0x7ff >> 6) & 0x1f;
    buf[5] = byte;
    byte = 0;
    byte |= (0x7ff & 0x3f) << 2;
    buf[6] = byte;
#else 
    byte |= 1;
    buf[5] = byte;
    byte = 0;
    buf[6] = byte;

#endif

    return 0;
}
void vplay_show_media_info(vplay_media_info_t *info){
    printf("Show vplay media info\n");
    printf("\tfile        ->%s\n", info->file.file);
    printf("\ttime_length ->%8lld\n", info->time_length);
    printf("\ttime_pos    ->%8lld\n", info->time_pos);
    printf("\ttrack_number->%8d\n", info->track_number);
    printf("\tvideo_index ->%8d\n",info->video_index);
    printf("\taudio_index ->%8d\n",info->audio_index);
    printf("\textra_index ->%8d\n",info->extra_index);
    for(int i=0; i < info->track_number; i++){
        printf("\t###stream_index->%d\n", info->track[i].stream_index);
        printf("\t\ttype        ->%d\n", info->track[i].type);
        printf("\t\twidth       ->%d\n", info->track[i].width);
        printf("\t\theight      ->%d\n", info->track[i].height);
        printf("\t\tfps         ->%d\n", info->track[i].fps);
        printf("\t\tsample_rate ->%d\n", info->track[i].sample_rate);
        printf("\t\tchannels    ->%d\n", info->track[i].channels);
        printf("\t\tbit_width   ->%d\n", info->track[i].bit_width);
    }
    printf("\t\n");
}
struct demux_t *open_media_demuxer(vplay_file_info_t *file, vplay_media_info_t *mediaInfo){
    assert(file != NULL);
    assert(mediaInfo != NULL);
    demux_t *pDemux = NULL;
    memset(mediaInfo, 0, sizeof(vplay_media_info_t));
    if ((strncasecmp("http", file->file, 4) !=  0) &&
        (strncasecmp("https", file->file, 5) !=  0))
    {
        if(access(file->file, R_OK) != 0){
            LOGGER_ERR("can not read media file->%s\n",file->file);
            return NULL;
        }
    }
    pDemux =  demux_init(file->file);
    if(pDemux == NULL){
        LOGGER_ERR("init demuxer error->%s\n",file->file);
        return NULL ;
    }
    mediaInfo->file = *file; 
    mediaInfo->time_length = pDemux->time_length;
    mediaInfo->track_number = pDemux->stream_num;
    mediaInfo->video_index = pDemux->video_index;
    mediaInfo->audio_index = pDemux->audio_index;
    mediaInfo->extra_index = pDemux->extra_index;
    mediaInfo->time_pos = 0;
    for(int i = 0; i < pDemux->stream_num; i++){
        stream_info_t  *pStream = pDemux->stream_info + i;
        vplay_track_info_t *pTrack = mediaInfo->track + i;
        pTrack->stream_index = i;
        pTrack->frame_num = pStream->frame_num;

        switch(pStream->codec_id){
            case DEMUX_AUDIO_PCM_ULAW:
                pTrack->type = VPLAY_CODEC_AUDIO_PCM_ULAW;
                pTrack->sample_rate = pStream->sample_rate;
                pTrack->channels = pStream->channels;
                pTrack->bit_width = pStream->bit_width;
                break;
            case DEMUX_AUDIO_PCM_ALAW:
                pTrack->type = VPLAY_CODEC_AUDIO_PCM_ALAW;
                pTrack->sample_rate = pStream->sample_rate;
                pTrack->channels = pStream->channels;
                pTrack->bit_width = pStream->bit_width;
                break;
            case DEMUX_AUDIO_AAC:
                pTrack->type = VPLAY_CODEC_AUDIO_AAC;
                pTrack->sample_rate = pStream->sample_rate;
                pTrack->channels = pStream->channels;
                pTrack->bit_width = pStream->bit_width;
                break;
            case DEMUX_AUDIO_PCM_S32E:
                pTrack->type = VPLAY_CODEC_AUDIO_PCM_S32E;
                pTrack->sample_rate = pStream->sample_rate;
                pTrack->channels = pStream->channels;
                pTrack->bit_width = pStream->bit_width;
                break;
            case DEMUX_AUDIO_PCM_S16E:
                pTrack->type = VPLAY_CODEC_AUDIO_PCM_S16E;
                pTrack->sample_rate = pStream->sample_rate;
                pTrack->channels = pStream->channels;
                pTrack->bit_width = pStream->bit_width;
                break;
            case DEMUX_AUDIO_ADPCM_G726:
                pTrack->type = VPLAY_CODEC_AUDIO_G726;
                pTrack->sample_rate = pStream->sample_rate;
                pTrack->channels = pStream->channels;
                pTrack->bit_width = pStream->bit_width;
                break;
            case DEMUX_VIDEO_H264:
                pTrack->type = VPLAY_CODEC_VIDEO_H264;
                pTrack->width = pStream->width;
                pTrack->height = pStream->height;
                pTrack->fps = pStream->fps;
                break;
            case DEMUX_VIDEO_H265:
                pTrack->type = VPLAY_CODEC_VIDEO_H265;
                pTrack->width = pStream->width;
                pTrack->height = pStream->height;
                pTrack->fps = pStream->fps;
                break;
            case DEMUX_VIDEO_MJPEG:
                pTrack->type = VPLAY_CODEC_VIDEO_MJPEG;
                pTrack->width = pStream->width;
                pTrack->height = pStream->height;
                pTrack->fps = pStream->fps;
                break;
            default :
                LOGGER_WARN("demux stream mayu be no headle ->%d\n", pStream->codec_id);
                continue;
        }
    }
    //vplay_show_media_info(mediaInfo);
    return pDemux;
}
QPlayer::QPlayer(VPlayerInfo *info):ThreadCore(){
    int ret = 0;
//    av_register_all();
    this->PlayerInfo  = *info ;
    this->PFileList = new std::list<vplay_file_info_t>();
    this->PFileList->empty();
    this->DemuxInstance =  NULL;
    this->PVideoFrFifo =  NULL;
    this->PAudioFrFifo =  NULL;
    this->CurFileReadyFlag = false ;
    memset(&this->CurFile , 0, sizeof(vplay_file_info_t));
    memset(&this->CurMediaInfo , 0, sizeof(vplay_media_info_t));
    this->VideoBuffer =  NULL;
    this->VideoBufferMaxLen =  1024*1024 ;
    this->AudioBufferMaxLen =  1024*100 ;
    this->AudioStreamSize =  1024*100;

    this->VideoType  = VIDEO_MEDIA_NONE ;
    this->AudioType  =  AUDIO_CODEC_TYPE_NONE ;
    this->PlaySpeed.store(1);
    this->PAudioCodecs = NULL;
    this->AudioBuffer = NULL ;
    memset(&this->AudioCodecsInfo, 0, sizeof(codec_info_t));
    memset(&this->AudioFrameInfo, 0, sizeof(struct demux_frame_t));
    this->PAudioInfo = NULL;

    this->PSpeedCtrl =  NULL;
    this->PAudioPlayer = NULL;


    this->AudioHeader =  NULL;
    this->AudioHeaderSize = 0;
    this->AudioStream = NULL;
    this->AudioStreamSize = 0;

    this->PDemuxAudioInfo = NULL;
    this->PDemuxVideoInfo = NULL;
    this->PDemuxExtraInfo = NULL;

    this->SetName((char *)"demuxer");
    this->VideoIndex = -1;
    this->AudioIndex = -1;
    this->ExtraIndex = -1;
    int len = strlen(info->media_file);
    if(len < VPLAY_PATH_MAX && len > 0){
        this->AddMediaFile(info->media_file);
    }
    else{
        LOGGER_WARN("media file error, please call queue cmd add file\n");
    }
    PlayerMaxCacheMs = PLAYER_CACHE_SECOND * 1000 - 200;
    PlayerMinCacheMs = PlayerMaxCacheMs / 4;
    LOGGER_DBG("player max cache time ->%2d ms:%4d\n", PLAYER_CACHE_SECOND, PlayerMaxCacheMs);

    this->stat.SetStatType(STAT_TYPE_PLAYBACK);
}
QPlayer::~QPlayer(){
    this->Release();
}
void QPlayer::Release(void){
    //release thread
    if(this->PSpeedCtrl != NULL ){
        delete this->PSpeedCtrl;
        this->PSpeedCtrl = NULL;
    }
    if(this->PAudioPlayer != NULL){
        delete this->PAudioPlayer;
        this->PAudioPlayer = NULL;
    }
    this->SetWorkingState(ThreadingState::Stop);
    if(this->DemuxInstance != NULL){
        demux_deinit(this->DemuxInstance);
        this->DemuxInstance = NULL;
    }
    if(this->PAudioInfo != NULL){
        delete this->PAudioInfo;
        this->PAudioInfo = NULL;
    }
    if(this->PVideoFrFifo != NULL){
        delete this->PVideoFrFifo;
        this->PVideoFrFifo = NULL;
    }
    if(this->PAudioFrFifo != NULL){
        delete this->PAudioFrFifo;
        this->PVideoFrFifo = NULL;
    }
    this->ReleaseAudioCodec();
    this->PFileList->clear();
    delete this->PFileList;
    this->PFileList = NULL;
    FREE_MEM_P(this->VideoBuffer);
    FREE_MEM_P(this->AudioBuffer);
}
void vplay_dump_buffer(char *buf, int size ,const char *name){
    printf("dump %s :", name );
    for(int i = 0 ;i < size; i++){
        if(i %16 == 0 ){
            printf("\n");
        }
        printf("%3X", buf[i]);
    }
    printf("\ndump %s over\n", name);
}
int QPlayer::PhotoWriteStream(const char *chn, void *data, int len)
{
    struct fr_buf_info buf;
    int ret, offset = 0;

    memset((void*)&buf,0,sizeof(buf));
    ret = fr_get_buf_by_name(chn, &buf);
    if(ret < 0) return ret;

    if(len > buf.map_size) {
        LOGGER_ERR("%s: size exceel limit: (0x%x > 0x%x)\n",
            __func__, len, buf.size);
        ret = VBEBADPARAM;
    } else{
        memcpy(buf.virt_addr, data, len);
        buf.size = len;
    }

    fr_put_buf(&buf);
    return ret;
}

int photo_trigger_decode(const char *chn)
{
    bool isDisplay = true;

    videobox_launch_rpc(chn, &isDisplay);
    return _rpc.ret;
}

int photo_decode_finish(const char *chn, bool* IsFinish)
{
    videobox_launch_rpc(chn, IsFinish);
    return _rpc.ret;
}

int QPlayer::PhotoTriggerDecode(const char *chn)
{
    return photo_trigger_decode(chn);
}

int QPlayer::PhotoDecodeFinish(const char *chn, bool* IsFinish)
{
    return photo_decode_finish(chn,IsFinish);
}

ThreadingState QPlayer::WorkingThreadTask(void ){
    assert(this->PVideoFrFifo != NULL);
    demux_frame_t frame;
    memset(&frame ,0,sizeof(struct demux_frame_t));
    struct  fr_buf_info fr;
    bool buffState = true;
    int ret = -1;
    uint64_t curTs = get_timestamp();

#if 0
        printf("fifo is state ,just stop push frame into fifo v:%6d a:%6d\n",
                this->PVideoFrFifo->GetCurCacheTimeLen(),
                this->PAudioFrFifo->GetCurCacheTimeLen());
#endif
    if(this->PDemuxAudioInfo != NULL && this->PDemuxVideoInfo != NULL){
        if(this->PlaySpeed.load() == 1){
            if(this->PAudioFrFifo->GetCurCacheTimeLen() >= PlayerMaxCacheMs ){
                buffState = false;
            }
            if(this->PVideoFrFifo->GetCurCacheTimeLen() >= PlayerMaxCacheMs ){
                buffState = false;
            }
        }
        else{
            if(this->PVideoFrFifo->GetCurCacheTimeLen() >= PlayerMaxCacheMs ){
                buffState = false;
            }
        }
    }
    else if(this->PDemuxAudioInfo != NULL){
        if(this->PlaySpeed.load() == 1){
            if(this->PAudioFrFifo->GetCurCacheTimeLen() >= PlayerMaxCacheMs ){
                buffState = false;
            }
        }
    }
    else if(this->PDemuxVideoInfo  != NULL){
        if(this->PVideoFrFifo->GetCurCacheTimeLen() >= PlayerMaxCacheMs ){
            buffState = false;
        }
    }
    else {
        LOGGER_INFO("no media data in media file ,just exit\n");
        return ThreadingState::Pause;
    }
    if (!this->DemuxInstance)
    {
        if (!this->CurFileReadyFlag)
        {
            vplay_report_event(VPLAY_EVENT_PLAY_FINISH,(char*)"play finish");
        }
        return ThreadingState::Pause;
    }

    this->stat.ShowInfo();
    if(this->PDemuxVideoInfo  != NULL)
        this->stat.UpdateFifoInfo(PACKET_TYPE_VIDEO,this->PVideoFrFifo->GetQueueSize(),this->PVideoFrFifo->GetDropFrameSize());
    if(this->PDemuxAudioInfo != NULL)
        this->stat.UpdateFifoInfo(PACKET_TYPE_AUDIO,this->PAudioFrFifo->GetQueueSize(),this->PAudioFrFifo->GetDropFrameSize());
    if((curTs -  this->LastTs) > 2000){
        LOGGER_INFO("play state ->%4d %4d %8lld\n",
                this->PVideoFrFifo->GetQueueSize(),
                this->PAudioFrFifo->GetQueueSize(),
                this->VPlayTs.GetTs(this->PlaySpeed)
                );
        LOGGER_INFO("buffState:%d A:%p,V:%p,load:%d A:%d %d, V:%d %d\n",
            buffState, this->PDemuxAudioInfo, this->PDemuxVideoInfo,
            this->PlaySpeed.load(),
            this->PAudioFrFifo->GetCurCacheTimeLen(), PlayerMaxCacheMs,
            this->PVideoFrFifo->GetCurCacheTimeLen(), PlayerMaxCacheMs
            );
        this->LastTs =  curTs;
    }

    if(buffState == false ){
#if 0
        printf("fifo is full ,just stop push frame into fifo %d %d\n",
                this->PVideoFrFifo->GetQueueSize(),
                this->PAudioFrFifo->GetQueueSize());
#endif
        if (this->IsEnhance)
        {
            this->EnhancePriority(false);
            this->IsEnhance = false;
        }
        usleep(100);
        return ThreadingState::Keep;
    }
    if ((this->DemuxInstance->frame_counter != 0) || (this->CurMediaInfo.video_index < 0)){
        int fmcnt = 0;

        if ((this->DemuxInstance->frame_counter == 0) && (this->CurMediaInfo.video_index < 0))
        {
            this->VPlayTs.Start();
        }
re_get_frame:
        ret = demux_get_frame(this->DemuxInstance, &frame);
        if(ret > 0){
            fr.virt_addr = frame.data ;
            fr.size = frame.data_size ;
            fr.timestamp = frame.timestamp ;
            fr.priv = frame.flags;
            if(frame.codec_id == DEMUX_VIDEO_H264 || frame.codec_id == DEMUX_VIDEO_H265 ){
                fr.timestamp = fr.timestamp *1000 / this->PDemuxVideoInfo->timescale;
                //LOGGER_DBG("PUT video frame ->%3X %5d %8lld %3X\n", frame.codec_id, fr.size, fr.timestamp, fr.priv);
                this->PVideoFrFifo->PushItem(&fr);
                uint64_t RefTime = this->VPlayTs.GetTs(this->PlaySpeed);
                this->stat.UpdateRefTime(RefTime);
                if ((!this->IsEnhance) && ( RefTime > fr.timestamp + VIDEO_SYNC_THREADHOLD / 2)
                    && (this->PVideoFrFifo->GetQueueSize() <= PlayerMinCacheMs * this->PDemuxVideoInfo->fps / 1000 ))
                {
                    this->EnhancePriority(true);
                    this->IsEnhance = true;
                    LOGGER_WARN("enhance priority %d %d cacheMs:%d A:%d V:%d ref:%llu, VPTS:%llu\n",
                            this->PVideoFrFifo->GetQueueSize(),
                            this->PAudioFrFifo->GetQueueSize(),
                            PlayerMaxCacheMs,
                            this->PAudioFrFifo->GetCurCacheTimeLen(),
                            this->PVideoFrFifo->GetCurCacheTimeLen(),
                            RefTime,
                            fr.timestamp
                            );
                }
                fmcnt ++;
                if ((fmcnt < this->PlaySpeed.load()) && (this->GetWorkingState() != ThreadingState::Stop))
                {
                    demux_put_frame(this->DemuxInstance,&frame);
                    goto re_get_frame;
                }
#if 0
                LOGGER_DBG("push video frame ->%3X %5ds %8lldt %8lldt s%5ds %3Xf\n",
                        frame.codec_id,
                        frame.data_size,
                        frame.timestamp,
                        fr.timestamp,
                        fr.size,
                        this->PVideoFrFifo->GetQueueSize());
#endif
            }
            else if (frame.codec_id == DEMUX_VIDEO_MJPEG)
            {
                //FIXME: ffphoto need codec info to decode, no need demux again
                ret = PhotoWriteStream(this->PlayerInfo.video_channel, frame.data, frame.data_size);
                ret = PhotoTriggerDecode(this->PlayerInfo.video_channel);
                demux_put_frame(this->DemuxInstance,&frame);
                LOGGER_INFO("play photo:set pause!!\n");
                return ThreadingState::Pause;
            }
            else if(frame.codec_id >= DEMUX_AUDIO_PCM_ULAW && frame.codec_id <= DEMUX_AUDIO_MP3 ) {
                //handler audio frame
                if ((this->PlaySpeed.load() != 1) || (this->IsAudioMute) || (this->IsStepDisplay))
                {
                    demux_put_frame(this->DemuxInstance,&frame);
                    this->ReleaseAudioCodec();
                    if (fmcnt < this->PlaySpeed.load() && (this->GetWorkingState() != ThreadingState::Stop))
                    {
                        goto re_get_frame;
                    }
                    return ThreadingState::Keep;
                }
                if(CheckAudioCodecsInstance(&frame) == false ){
                    LOGGER_ERR("check audio codecs inst error\n");
                //    this->SetWorkingState(ThreadingState::Pause);
                    return ThreadingState::Pause;
                }
                //vplay_dump_buffer((char *)frame.data, 16, "audio raw");
                struct codec_addr codecAddr;
                static int aacFd = -1;
                static int pcmFd = -1;
                //save_tmp_file(3,fr.virt_addr,fr.size);
                if(frame.codec_id == DEMUX_AUDIO_AAC){
                    if(this->AudioHeader == NULL || this->AudioStream == NULL){
                        this->AudioHeaderSize = 7;
                        this->AudioHeader = (char *)malloc(this->AudioHeaderSize + 9);
                        if(this->AudioHeader == NULL){
                            LOGGER_ERR("malloc aac audio header error\n");
                            return ThreadingState::Pause;
                        }
                        else{
                            LOGGER_DBG("malloc audio header mem ->%4d\n", this->AudioHeaderSize);
                        }
                        //this->AudioStreamSize = frame.data_size + this->AudioHeaderSize + 10;
                        this->AudioStream = (char *) malloc(this->AudioBufferMaxLen);
                        if(this->AudioStream == NULL){
                            LOGGER_ERR("malloc aac audio header error\n");
                            return ThreadingState::Pause;
                        }
                        else{
                            LOGGER_DBG("malloc audio raw stream mem ->%4d\n", this->AudioBufferMaxLen);
                        }
                        printf("aac extra data -> %d : %2X %2X\n",
                                this->PDemuxAudioInfo->extradata_size,
                                this->PDemuxAudioInfo->extradata[0],
                                this->PDemuxAudioInfo->extradata[1]);
                        if(aac_decode_extradata(&this->AudioAdtsContext, this->PDemuxAudioInfo->extradata, this->PDemuxAudioInfo->extradata_size) < 0){
                            LOGGER_ERR("parse stream extra data error\n");
                            return ThreadingState::Pause;
                        }

                    }
                    if(aac_set_adts_head(&this->AudioAdtsContext, this->AudioHeader, frame.data_size) < 0){
                        LOGGER_ERR("fill aac adts header error\n");
                        return ThreadingState::Pause;
                    }
#if 0
                    else{
                        LOGGER_INFO("aac adts header %5d ->", frame.data_size + 7);
                        for(int i = 0; i < this->AudioHeaderSize; i++){
                            printf("%3X", this->AudioHeader[i]);
                        }
                        printf("\n");
                    }
#endif
                    memcpy(this->AudioStream, this->AudioHeader, 7);
                    memcpy(this->AudioStream + this->AudioHeaderSize, frame.data, frame.data_size);

                    codecAddr.in = this->AudioStream ;
                    codecAddr.len_in = frame.data_size + 7;

                }
                else{
                    codecAddr.in = frame.data ;
                    codecAddr.len_in = frame.data_size ;

                }

#ifdef VPLAY_DEBUG_AUDIO_DUMP
                if(aacFd < 0){
                    aacFd = open("/nfs/stream.wav", O_WRONLY|O_CREAT);
                    if(aacFd < 0 ){
                        LOGGER_ERR("open  file error\n");
                    }
                    else{
                        LOGGER_DBG("open audio stream  temp file\n");
                    }
                }
                if(pcmFd < 0){
                    pcmFd = open("/nfs/pcm.wav", O_WRONLY|O_CREAT);
                    if(pcmFd < 0 ){
                        LOGGER_ERR("open pcm file error\n");
                    }
                    else{
                        LOGGER_DBG("open pcm  temp file\n");
                    }
                }
                if(aacFd >= 0){
                    if(write(aacFd, codecAddr.in, codecAddr.len_in)  != codecAddr.len_in){
                        LOGGER_ERR("write a stream file error\n");
                    }
                }

#endif
                codecAddr.out = this->AudioBuffer;
                codecAddr.len_out = this->AudioBufferMaxLen;
                if (this->PAudioCodecs)
                {
                    fr.size = codec_decode(this->PAudioCodecs, &codecAddr);
                }
                else
                {
                    fr.size = -1;
                }
                if(fr.size <= 0){
                    LOGGER_ERR("decode audio size error ->%d\n", codecAddr.len_in);
                    vplay_dump_buffer((char *)codecAddr.in, 24, "decode auido error");
                    demux_put_frame(this->DemuxInstance,&frame);
                    return ThreadingState::Keep;
                }
                else {
                    fr.timestamp = fr.timestamp *1000 / this->PDemuxAudioInfo->timescale;
                    fr.virt_addr =  this->AudioBuffer;
                   // printf("audio decode ->%d %8lld\n", fr.size, fr.timestamp);
#if 0
                    static int counter = 0;
                    LOGGER_DBG("decode ok ->%3d %5d %8lld\n", counter++, fr.size, fr.timestamp);
                    LOGGER_DBG("audio frame ->%3X %5ds %8lldt %8lldt %5ds\n",
                            frame.codec_id,
                            frame.data_size,
                            frame.timestamp,
                            fr.timestamp,
                            fr.size);
#endif
                    this->PAudioFrFifo->PushItem(&fr);
                    this->stat.UpdatePacketPTS(PACKET_TYPE_AUDIO,this->VPlayTs.GetTs(this->PlaySpeed));
                    this->stat.UpdateFifoInfo(PACKET_TYPE_AUDIO,this->PAudioFrFifo->GetQueueSize()
                        ,this->PAudioFrFifo->GetDropFrameSize());
#ifdef VPLAY_DEBUG_AUDIO_DUMP
                if(pcmFd >= 0){
                    if(write(pcmFd, codecAddr.out, fr.size)  != fr.size){
                        LOGGER_ERR("write pcm file error\n");
                    }
                }
#endif

                }
            }
            else {
                LOGGER_ERR("not support stream ->%d\n", frame.codec_id);
            }
            demux_put_frame(this->DemuxInstance,&frame);
        }
        else if(ret == 0){
            //skip null frame
        }
        else {
            LOGGER_INFO("read frame failed, try play next one\n");
            this->CurFileReadyFlag = false ;
            //find next file and continue play
            //add end frame to all frame fifo

            this->AddStreamEndDummy();
            this->ReleaseAudioCodec();
            if(this->OpenNextMediaDemuxer(false) == false){
                this->FlushAVBuffer(PLAYER_CACHE_SECOND * 1000 + 500);
                LOGGER_ERR("open next file error and stop  demuxer thread\n");
                this->ReleaseAudioCodec();
                vplay_report_event(VPLAY_EVENT_PLAY_FINISH,(char*)"play finish");
                return ThreadingState::Pause;
            }
            else {
                LOGGER_DBG("open next file ok and beging play\n");
            }
        }
        return ThreadingState::Keep;
    }
    else { //just push vidoe header
        this->FlushAVBuffer(500);
        this->PushVideoHeader();
    }
    return ThreadingState::Keep;
}
bool QPlayer::ThreadPreStartHook(void){
    return true ;
}
bool QPlayer::ThreadPostPauseHook(void){
    //this->PAudioPlayer->SetWorkingState(ThreadingState::Pause);
    //this->PSpeedCtrl->SetWorkingState(ThreadingState::Pause);
    return true;
}
bool QPlayer::ThreadPostStopHook(void){
    this->CloseCurMediaDemuxer();
    if(this->PVideoFrFifo != NULL){
        this->PVideoFrFifo->Flush();
    }
    if(this->PAudioFrFifo != NULL){
        this->PAudioFrFifo->Flush();
    }
    this->ReleaseAudioCodec();
    FREE_MEM_P(this->AudioHeader);
    FREE_MEM_P(this->AudioStream);
    this->IsEnhance = false;
    this->IsAudioMute = false;
    return true;
}
bool QPlayer::OpenNextMediaDemuxer(bool first){
    if(this->CurFileReadyFlag == false){
        //open next file and file
        if(this->PFileList->empty() == true ){
            LOGGER_ERR("no file in the play list ,please add more\n");
            //this->SetWorkingState(ThreadingState::Pause);
            return false ;
        }

        if(this->PFileList->empty() == true){
            LOGGER_ERR("play list is empty, please add more\n");
            //this->SetWorkingState(ThreadingState::Pause);
            return false ;
        }
        this->CurFile = this->PFileList->front();
        memset(&this->CurMediaInfo, 0, sizeof(vplay_media_info_t));
        memset(&this->AudioAdtsContext, 0, sizeof(adts_context_t));
        LOGGER_INFO("will play file ->%s\n", this->CurFile.file);

        vplay_file_info_t file = this->PFileList->front();
        this->PFileList->pop_front();

        if(this->DemuxInstance != NULL){
            LOGGER_DBG("close last file instance\n");
            demux_deinit(this->DemuxInstance);
            this->DemuxInstance = NULL ;
        }
        this->DemuxInstance = open_media_demuxer(&this->CurFile,
                &this->CurMediaInfo);
        if(this->DemuxInstance == NULL){
            LOGGER_DBG("open file error\n");
            this->DemuxInstance = NULL ;
            return false;
        }
        this->PDemuxVideoInfo = NULL;
        this->PDemuxAudioInfo = NULL;
        this->PDemuxExtraInfo = NULL;
        this->CurFileReadyFlag = true;
    }
    LOGGER_INFO("demux default index ->v:%d  a:%d e:%d\n",
            this->DemuxInstance->video_index,
            this->DemuxInstance->audio_index,
            this->DemuxInstance->extra_index
            );
    LOGGER_INFO("user define index ->v:%d a:%d e:%d\n",
            this->VideoIndex,
            this->AudioIndex,
            this->ExtraIndex
            );
    if(this->VideoIndex != -1){
        if(this->VideoIndex < this->DemuxInstance->stream_num){
            this->CurMediaInfo.video_index = this->VideoIndex;
        }
        else{
            LOGGER_ERR("current Video index filter error ->%d %d\n",
                    this->VideoIndex, this->DemuxInstance->stream_num);
            return false;
        }
    }
    else{
        this->CurMediaInfo.video_index = this->DemuxInstance->video_index;
    }
    if(this->AudioIndex != -1){
        if(this->AudioIndex < this->DemuxInstance->stream_num){
            this->CurMediaInfo.audio_index = this->AudioIndex;
        }
        else{
            LOGGER_ERR("current Video index filter error ->%d %d\n",
                    this->AudioIndex, this->DemuxInstance->stream_num);
            return false;
        }
    }
    else{
        this->CurMediaInfo.audio_index = this->DemuxInstance->audio_index;
    }
#if 1
    if(this->ExtraIndex != -1){
        if(this->ExtraIndex < this->DemuxInstance->stream_num){
            this->CurMediaInfo.extra_index = this->ExtraIndex;
        }
        else{
            LOGGER_ERR("current Video index filter error ->%d %d\n",
                    this->ExtraIndex, this->DemuxInstance->stream_num);
            return false;
        }
    }
    else{
        this->CurMediaInfo.extra_index = this->DemuxInstance->extra_index;
    }
#endif
    LOGGER_INFO("will set media index->%d %d %d\n",
            this->CurMediaInfo.video_index,
            this->CurMediaInfo.audio_index,
            this->CurMediaInfo.extra_index);
    if(demux_set_stream_filter(this->DemuxInstance,
                this->CurMediaInfo.video_index,
                this->CurMediaInfo.audio_index,
                this->CurMediaInfo.extra_index) < 0){
        LOGGER_INFO("set video index error->%d %d %d\n",
                this->CurMediaInfo.video_index,
                this->CurMediaInfo.audio_index,
                this->CurMediaInfo.extra_index);
    }
    else{
        LOGGER_INFO("set video index ok->%d %d %d\n",
                this->CurMediaInfo.video_index,
                this->CurMediaInfo.audio_index,
                this->CurMediaInfo.extra_index);
        LOGGER_INFO("demux inst index ->%d %d %d\n",
               this->DemuxInstance->video_index,
               this->DemuxInstance->audio_index,
               this->DemuxInstance->extra_index
                );
    }
    if(this->DemuxInstance->video_index != -1){
        this->PDemuxVideoInfo = this->DemuxInstance->stream_info + this->DemuxInstance->video_index;
        if ((this->PDemuxVideoInfo) && (this->PDemuxVideoInfo->fps <= 0))
        {
            LOGGER_WARN("video fps %d error, refine to %d.\n", this->PDemuxVideoInfo->fps, DEFAULT_FPS);
            this->PDemuxVideoInfo->fps = DEFAULT_FPS;
        }
    }
    else{
        this->PDemuxVideoInfo = NULL;
    }
    if(this->DemuxInstance->audio_index != -1){
        this->PDemuxAudioInfo = this->DemuxInstance->stream_info + this->DemuxInstance->audio_index;
    }
    else{
        this->PDemuxAudioInfo = NULL;
    }
    if(this->DemuxInstance->extra_index != -1){
        this->PDemuxExtraInfo = this->DemuxInstance->stream_info + this->DemuxInstance->extra_index;
    }
    else{
        this->PDemuxExtraInfo = NULL;
    }
    LOGGER_INFO("stream info ->v:%p a:%p e:%p\n",
            this->PDemuxVideoInfo,
            this->PDemuxAudioInfo,
            this->PDemuxExtraInfo
            );
    return true;
}
bool QPlayer::CloseCurMediaDemuxer(void){
    if(this->DemuxInstance != NULL){
        demux_deinit(this->DemuxInstance);
        this->DemuxInstance = NULL;
        this->CurFileReadyFlag = false ;
    }
}

bool QPlayer::Prepare(void){
    if(this->PVideoFrFifo == NULL){
        this->PVideoFrFifo = new QMediaFifo((char *)"video_fifo");
        if(this->PVideoFrFifo == NULL){
            LOGGER_ERR("init frfifo error\n");
            goto Prepare_Error_Handler;
        }
        this->PVideoFrFifo->SetMaxCacheTime(PLAYER_CACHE_SECOND * 1000 + 500);
    }
    if(this->PAudioFrFifo == NULL){
        this->PAudioFrFifo = new QMediaFifo((char *)"audio_fifo");
        if(this->PAudioFrFifo == NULL){
            LOGGER_ERR("init frfifo error\n");
            goto Prepare_Error_Handler;
        }
        this->PAudioFrFifo->SetMaxCacheTime(PLAYER_CACHE_SECOND * 1000 + 500);
    }
    this->SetWorkingState(ThreadingState::Standby);
    if(this->VideoBuffer == NULL){
        this->VideoBuffer = (char *)malloc(this->VideoBufferMaxLen);
        if(this->VideoBuffer == NULL){
            goto Prepare_Error_Handler;
        }
    }
    if(this->AudioBuffer == NULL){
        this->AudioBuffer = (char *)malloc(this->AudioBufferMaxLen);
        if(this->AudioBuffer == NULL){
            goto Prepare_Error_Handler;
        }
    }
    this->PAudioInfo = new AudioStreamInfo();
    if(this->PSpeedCtrl == NULL){
        this->PSpeedCtrl = new SpeedCtrl(&this->PlayerInfo);
        this->PSpeedCtrl->PVideoFrFifo = this->PVideoFrFifo ;
        this->PSpeedCtrl->stat = &this->stat;
        if(this->PSpeedCtrl->Prepare() == false){
            LOGGER_ERR("SpeedCtrl Prepare error\n");
            goto Prepare_Error_Handler ;
        }
    }
    if(this->PAudioPlayer == NULL){
        this->PAudioPlayer = new QAudioPlayer(this->PAudioInfo);
        if(this->PAudioPlayer == NULL){
            LOGGER_ERR("init audio player error\n");
            goto Prepare_Error_Handler;
        }
        this->PAudioPlayer->PFrFifo =  this->PAudioFrFifo;
        if(this->PAudioPlayer->Prepare() == false){
            LOGGER_ERR("prepare audio player audio failed\n");
            goto Prepare_Error_Handler;
        }
    }

    LOGGER_DBG("player fifo ->0x%p 0x%p\n",this->PSpeedCtrl->PVideoFrFifo,this->PAudioPlayer->PFrFifo);

    this->PSpeedCtrl->PVPlayTs = &this->VPlayTs;
    this->PAudioPlayer->PVPlayTs = &this->VPlayTs;

    this->VideoIndex = -1;
    this->AudioIndex = -1;
    this->ExtraIndex = -1;
    this->IsAudioMute = false;
    memset(&this->AudioAdtsContext, 0, sizeof(adts_context_t));
    this->IsEnhance = false;
    this->IsStepDisplay = false;

    return true;
Prepare_Error_Handler :
    this->Release();
    return false;
}
bool QPlayer::UnPrepare(void){
    this->Release();
    return true;
}

bool QPlayer::PushVideoHeader(void){
    //int ret = demux_get_head(this->DemuxInstance ,this->VideoBuffer,128);
    if(this->CurMediaInfo.video_index < 0){
        LOGGER_INFO("no video for play\n");
        return true;
    }
    int ret = demux_get_track_header(this->DemuxInstance, this->CurMediaInfo.video_index, this->VideoBuffer, 128);
    if(ret < 0){
        LOGGER_ERR("read video frame header error, ret:%d\n", ret);
        return false;
    }
    struct fr_buf_info fr  ;
    memset(&fr,0 ,sizeof(struct fr_buf_info ));
    fr.virt_addr= this->VideoBuffer ;
    fr.size = ret ;
    fr.priv =  VIDEO_FRAME_HEADER ;
#if 1
    LOGGER_INFO("get video header ->%d  %d\n", this->CurMediaInfo.video_index, ret);
    vplay_dump_buffer((char *)fr.virt_addr, ret, "header");
#endif
    this->PVideoFrFifo->PushItem(&fr);
    LOGGER_DBG("push video header->%3d %4X  %5d\n",this->PVideoFrFifo->GetQueueSize(),fr.priv,fr.size);
}
bool QPlayer::Play(int speed){
    bool startNewStream = true;
    if(this->GetWorkingState() == ThreadingState::Start){
        LOGGER_DBG("puase player thread\n");
        this->SetWorkingState(ThreadingState::Pause);
        if (speed != this->PlaySpeed.load(std::memory_order_acquire))
        {
            startNewStream = false;
            // Trick mode need mute audio.
            if (speed != 1)
            {
                if (this->PAudioFrFifo)
                {
                    this->PAudioFrFifo->Flush();
                }
            }
        }
        else
        {
            startNewStream = true;
            this->FlushAVBuffer(0);
            this->VPlayTs.Stop();
        }
    }
    this->IsStepDisplay = false;
    if (startNewStream)
    {
        this->CurFileReadyFlag = false;
        if(this->OpenNextMediaDemuxer(true) == false)
        {
            LOGGER_ERR("open next media file error\n");
            return false;
        }
    }
    if (!this->DemuxInstance)
    {
        LOGGER_ERR("Demux open fail.\n");
        return false;
    }

    this->VPlayTs.SetPlaySpeed(speed);

    LOGGER_INFO("play with speed ->%d\n",speed);
    this->PlaySpeed.store(speed);
    this->SetWorkingState(ThreadingState::Start);
#ifndef VPLAY_PLAY_VIDEO_ONLY
    if ((this->PDemuxAudioInfo != NULL) && (!this->IsAudioMute))
    {
        LOGGER_INFO("open aplayer\n" );
        this->PAudioPlayer->SetPlaySpeed(speed);
        this->PAudioPlayer->SetWorkingState(ThreadingState::Start);
        this->PSpeedCtrl->EnableAutoUpdataTs(false);
    }
    else{
        this->PSpeedCtrl->EnableAutoUpdataTs(true);
    }
#endif
    LOGGER_DBG("open vplayer\n" );
    if(this->PDemuxVideoInfo != NULL){
        this->PSpeedCtrl->SetPlaySpeed(speed);
        this->PSpeedCtrl->SetWorkingState(ThreadingState::Start);
    }
    LOGGER_DBG("open aplayer\n" );
    return true ;
}

bool QPlayer::FlushAVBuffer(int waitTime)
{
    int i = 0;

    while(!(((this->PVideoFrFifo == NULL) || (this->PVideoFrFifo->GetQueueSize() == 0))
        && ((this->PAudioFrFifo == NULL) || (this->PAudioFrFifo->GetQueueSize() == 0))))
    {
        if (i%50 == 0)
        {
            LOGGER_INFO("video fifo ->%d, audio fifo ->%d\n",
                this->PVideoFrFifo->GetQueueSize(),
                this->PAudioFrFifo->GetQueueSize());
        }
        if (i*10 >= waitTime)
        {
            LOGGER_WARN("wait time out(%d), video fifo ->%d, audio fifo ->%d\n",
                waitTime,
                this->PVideoFrFifo->GetQueueSize(),
                this->PAudioFrFifo->GetQueueSize());
            break;
        }
        i ++;
        usleep(10*1000);
    }
    if (this->PVideoFrFifo)
    {
        this->PVideoFrFifo->Flush();
    }
    if (this->PAudioFrFifo)
    {
        this->PAudioFrFifo->Flush();
    }
    return true;
}

int64_t QPlayer::Seek(int whence ,int64_t value){
    this->PSpeedCtrl->SetWorkingState(ThreadingState::Pause);
    this->PAudioPlayer->SetWorkingState(ThreadingState::Pause);
    this->SetWorkingState(ThreadingState::Pause);
    int64_t ret = demux_seek(this->DemuxInstance,value,SEEK_SET);
    if(ret < 0 ){
        LOGGER_ERR("seek file error ->%p %8lld\n",this->DemuxInstance,value);
        return -1;
    }
    this->PVideoFrFifo->Flush();
    this->PAudioFrFifo->Flush();
    this->VPlayTs.Seek(value);
    this->SetWorkingState(ThreadingState::Start);
    if(this->PVideoFrFifo != NULL){
        this->PSpeedCtrl->SetWorkingState(ThreadingState::Start);
    }
    if(this->PAudioFrFifo != NULL){
        this->PAudioPlayer->SetWorkingState(ThreadingState::Start);
    }
    return ret;
}
bool QPlayer::Stop(void){
    if(this->PSpeedCtrl != NULL){
        this->PSpeedCtrl->SetWorkingState(ThreadingState::Stop);
        LOGGER_INFO("stop video player ok\n");
        this->PVideoFrFifo->Flush();
    }
#ifndef VPLAY_PLAY_VIDEO_ONLY
    if(this->PAudioPlayer!= NULL){
        this->PAudioPlayer->SetWorkingState(ThreadingState::Stop);
        LOGGER_INFO("stop audio player ok\n");
        this->PAudioFrFifo->Flush();
    }
#endif
    this->SetWorkingState(ThreadingState::Stop);

    this->VPlayTs.Stop();
    this->IsStepDisplay = false;

    LOGGER_INFO("stop QPlayer ok\n");

    //release other resource
    return true;
}
bool QPlayer::Pause(void){
    LOGGER_DBG("pause play\n");
    if(this->PDemuxVideoInfo != NULL){
        this->PSpeedCtrl->SetWorkingState(ThreadingState::Pause);
    }
    if(PDemuxAudioInfo!= NULL){
        this->PAudioPlayer->SetWorkingState(ThreadingState::Pause);
    }
    this->SetWorkingState(ThreadingState::Pause);
    this->VPlayTs.Pause(true);
    return true;
}
bool QPlayer::Continue(int speed){
    if(this->DemuxInstance == NULL ){
        if(this->OpenNextMediaDemuxer(true) == false ){
            LOGGER_ERR("open next media file error\n");
            return false;
        }
    }
    if (speed == 0)
    {
        LOGGER_WARN("speed incorrect:%d\n", speed);
        speed = 1;
    }
    this->VPlayTs.Pause(false);
    this->SetWorkingState(ThreadingState::Start);
    if(this->PDemuxVideoInfo != NULL){
        this->PSpeedCtrl->SetPlaySpeed(speed);
        this->PSpeedCtrl->SetWorkingState(ThreadingState::Start);
    }
#ifndef VPLAY_PLAY_VIDEO_ONLY
    if(PDemuxAudioInfo!= NULL){
        this->PAudioPlayer->SetPlaySpeed(speed);
        this->PAudioPlayer->SetWorkingState(ThreadingState::Start);
    }
#endif
    return true;
}
bool QPlayer::AddMediaFile(const char *fileName){
    LOGGER_DBG("##%s ->%s\n", __func__, fileName);
    int nameLen = strlen(fileName);
    if(nameLen >= VPLAY_PATH_MAX) {
        LOGGER_ERR("target file path length is exceed the max ->%d %d\n", nameLen, VPLAY_PATH_MAX);
        return false ;
    }
    if ((strncasecmp("http", fileName, 4) !=  0) &&
        (strncasecmp("https", fileName, 5) !=  0))
    {
        if(access(fileName, R_OK) != 0){
            LOGGER_ERR("can not access target file ,add failed\n");
            return false ;
        }
    }
    vplay_file_info_t fileInfo ;
    memset(&fileInfo, 0, sizeof(vplay_file_info_t));
    memcpy(fileInfo.file, fileName, nameLen);
    this->PFileList->push_back(fileInfo);
    LOGGER_INFO("current play list number ->%d, add file:%s\n", this->PFileList->size(), fileName);

    return true ;
}

int QPlayer::ClearPlayList(){
    LOGGER_INFO("stop play and clear play list\n");
    //TODO:
}
int QPlayer::RemoveFileFromPlayList(char *file){
    this->SetWorkingState(ThreadingState::Pause);
    this->PFileList->clear();
}


bool QPlayer::CheckAudioCodecsInstance(struct demux_frame_t  *frame){
    if( (this->AudioFrameInfo.codec_id != frame->codec_id ) ||
            (this->AudioFrameInfo.bitwidth != frame->bitwidth) ||
            (this->AudioFrameInfo.channels != frame->channels) |
            (this->AudioFrameInfo.sample_rate != frame->sample_rate)
      ){
        LOGGER_DBG("audio frame new state change ->%d %d %d %5d\n",
                this->AudioFrameInfo.codec_id,
                this->AudioFrameInfo.bitwidth,
                this->AudioFrameInfo.channels ,
                this->AudioFrameInfo.sample_rate
                );
        LOGGER_DBG("audio frame old state change ->%d %d %d %5d\n",
                frame->codec_id,
                frame->bitwidth,
                frame->channels ,
                frame->sample_rate
                );
        if(this->PAudioCodecs != NULL){
            codec_close(this->PAudioCodecs);
            this->PAudioCodecs = NULL;
        }
    }
    if(this->PAudioCodecs == NULL){
    //    frame->channels = 1;
        this->AudioFrameInfo = *frame ;
        LOGGER_DBG("###########audio frame state change ->type:%d bitwidth:%d ch:%d sam:%5d\n",
                frame->codec_id,
                frame->bitwidth,
                frame->channels ,
                frame->sample_rate
                );
        //new PAudioCodecs instance and update audio play info (sample rate and channel number)
        //TODO :fix me from demuxer
        this->AudioCodecsInfo.in.channel =  frame->channels;
        this->AudioCodecsInfo.in.sample_rate =  frame->sample_rate;
        switch(frame->codec_id){
            case DEMUX_AUDIO_PCM_ULAW :
                this->AudioCodecsInfo.in.codec_type = AUDIO_CODEC_G711U ;
                this->AudioCodecsInfo.in.bitwidth = 16 ;
                this->AudioCodecsInfo.in.bit_rate =
                    this->AudioCodecsInfo.in.channel * this->AudioCodecsInfo.in.sample_rate *
                    GET_BITWIDTH(this->AudioCodecsInfo.in.bitwidth) / 2;
                break;
            case DEMUX_AUDIO_PCM_ALAW :
                this->AudioCodecsInfo.in.codec_type = AUDIO_CODEC_G711A ;
                this->AudioCodecsInfo.in.bitwidth = 16 ;
#if 0
                if(this->AudioCodecsInfo.in.channel == 2){
                    this->AudioCodecsInfo.in.channel =  1;
                    this->AudioCodecsInfo.in.sample_rate *= 2;
                }
#endif
                this->AudioCodecsInfo.in.bit_rate =
                    this->AudioCodecsInfo.in.channel * this->AudioCodecsInfo.in.sample_rate *
                    GET_BITWIDTH(this->AudioCodecsInfo.in.bitwidth) / 2;
                break;
            case DEMUX_AUDIO_AAC :
                this->AudioCodecsInfo.in.codec_type = AUDIO_CODEC_AAC ;
                this->AudioCodecsInfo.in.bitwidth = 16 ;
                this->AudioCodecsInfo.in.bit_rate =
                    this->AudioCodecsInfo.in.channel * this->AudioCodecsInfo.in.sample_rate *
                    GET_BITWIDTH(this->AudioCodecsInfo.in.bitwidth) / 4;
                break;
            case DEMUX_AUDIO_PCM_S32E :
                this->AudioCodecsInfo.in.codec_type = AUDIO_CODEC_PCM ;
                this->AudioCodecsInfo.in.bitwidth = 32 ;
                this->AudioCodecsInfo.in.bit_rate =
                    this->AudioCodecsInfo.in.channel * this->AudioCodecsInfo.in.sample_rate *
                    GET_BITWIDTH(this->AudioCodecsInfo.in.bitwidth);
                break;
            case DEMUX_AUDIO_PCM_S16E :
                this->AudioCodecsInfo.in.codec_type = AUDIO_CODEC_PCM ;
                this->AudioCodecsInfo.in.bitwidth = 16 ;
                this->AudioCodecsInfo.in.bit_rate =
                    this->AudioCodecsInfo.in.channel * this->AudioCodecsInfo.in.sample_rate *
                    GET_BITWIDTH(this->AudioCodecsInfo.in.bitwidth);
                break;
            case DEMUX_AUDIO_MP3:
                this->AudioCodecsInfo.in.codec_type = AUDIO_CODEC_MP3;
                this->AudioCodecsInfo.in.bitwidth = 16;
                this->AudioCodecsInfo.in.bit_rate =
                    this->AudioCodecsInfo.in.channel * this->AudioCodecsInfo.in.sample_rate *
                    GET_BITWIDTH(this->AudioCodecsInfo.in.bitwidth) / 4;
                break;
            default :
                LOGGER_ERR("not supprt codec_type(%d)\n", frame->codec_id);
                return false ;

        }

        this->AudioCodecsInfo.out.codec_type =  AUDIO_CODEC_PCM ;
        this->AudioCodecsInfo.out.bitwidth = 32 ;
        this->AudioCodecsInfo.out.channel =  2;
        this->AudioCodecsInfo.out.sample_rate = 16000;
        this->AudioCodecsInfo.out.bit_rate =
            this->AudioCodecsInfo.out.channel * this->AudioCodecsInfo.out.sample_rate *
                     GET_BITWIDTH(this->AudioCodecsInfo.out.bitwidth);
        codec_fmt_t *fmt = &this->AudioCodecsInfo.in;
        LOGGER_INFO("+++++++++audio in   ->%d %3d %d %5d %6d\n", fmt->codec_type, fmt->bitwidth, fmt->channel, fmt->sample_rate, fmt->bit_rate);
        fmt = &this->AudioCodecsInfo.out;
        LOGGER_INFO("+++++++++audio out  ->%d %3d %d %5d %6d\n", fmt->codec_type, fmt->bitwidth, fmt->channel,fmt->sample_rate, fmt->bit_rate);
        this->PAudioCodecs =  codec_open(&this->AudioCodecsInfo);
        if(this->PAudioCodecs == NULL){
            LOGGER_ERR("init audio codecs error\n");
            return false ;
        }
        LOGGER_DBG("open new codec ->%p\n", this->PAudioCodecs);
        audio_fmt_t audioFmt ;
        audioFmt.channels =  this->AudioCodecsInfo.out.channel ;
        audioFmt.bitwidth =  this->AudioCodecsInfo.out.bitwidth;
        audioFmt.samplingrate = this->AudioCodecsInfo.out.sample_rate ;
        audioFmt.sample_size = 1024;
        LOGGER_DBG("update audio info+++++++++++++++\n");

        this->PAudioInfo->SetInType(&this->AudioCodecsInfo.in);
        this->PAudioInfo->SetOutType(&this->AudioCodecsInfo.out);
        this->PAudioInfo->SetChannel(this->PlayerInfo.audio_channel);
        this->PAudioInfo->SetUpdate(true);
        this->PAudioPlayer->UpdateAudioInfo();
    }
    else{
        //LOGGER_DBG("use audio instance \n");
    }
    return true ;
}
bool QPlayer::CloseCurMediaDemuxer(struct demux_t *demux){
    demux_deinit(demux);
    return true;
}
int QPlayer::QueryMediaFileInfo(char *file,vplay_media_info_t *info){
    LOGGER_TRC("##%s ->%p %p\n", __func__, file, info);
    vplay_file_info_t popFile = {0};
    if(file != NULL){
        LOGGER_INFO("query media file info ->%s\n", file);
        int len =  strlen(file);
        if(len >= VPLAY_PATH_MAX){
            LOGGER_ERR("file path exceed max len ->%d %d\n", len, VPLAY_PATH_MAX);
            return -1;
        }
        memcpy(popFile.file, file, len);
        struct demux_t *demuxer = open_media_demuxer(&popFile, info);
        LOGGER_INFO("query media file info ->%s\n", popFile.file);
        if(demuxer != NULL){
            LOGGER_DBG("open file error\n");
            this->DemuxInstance = NULL ;
            return -1;
        }
        else{
            demux_deinit(demuxer); 
            return 1;
        }
    }
    else{
        if(this->DemuxInstance){
            *info = this->CurMediaInfo; 
            info->time_pos = this->VPlayTs.GetTs(this->PlaySpeed);
            if (info->time_pos > info->time_length)
            {
                info->time_pos = info->time_length;
            }
            if((info->time_pos/1000) % 5 == 0)
                LOGGER_INFO("query playing file info,time:%lld,systime:%llu\n",info->time_pos,get_timestamp());
            return 1;
        }
        else{
            LOGGER_INFO("query first file info\n");
            if(this->PFileList->empty() == true ){
                LOGGER_ERR("no file in the play list ,please add more\n");
                return -1 ;
            }
            else {
                popFile = this->PFileList->front();
            }
            struct demux_t *demuxer = open_media_demuxer(&popFile, info);
            LOGGER_INFO("query media file info ->%s\n", popFile.file);
            if(demuxer == NULL){
                LOGGER_DBG("open file error\n");
                this->DemuxInstance = NULL ;
                return -1;
            }
            demux_deinit(demuxer); 
        }
    }
    vplay_show_media_info(info);
    return 1;
}
int QPlayer::SetVideoFilter(int index){
    if(index < 0 ){
        LOGGER_ERR("vidoe index can be negetive ->%d\n", index);
        return -1;
    }
    this->VideoIndex =  index;
   // if(DemuxInstance){
        LOGGER_INFO("stream index ->v:%d a:%d e:%d\n",
                this->VideoIndex,
                this->AudioIndex,
                this->ExtraIndex);
#if 0
        if(demux_set_stream_filter(this->DemuxInstance,
                this->VideoIndex,
                this->AudioIndex,
                this->ExtraIndex) < 0){
            LOGGER_ERR("set video index error->%d %d %d\n",
                this->VideoIndex,
                this->AudioIndex,
                this->ExtraIndex);
        }
#endif
    //}
    return 1;
}
int QPlayer::SetAudioFilter(int index){
    if(index < 0 ){
        LOGGER_ERR("audio index can be negetive->%d\n", index);
        return -1;
    }
    this->AudioIndex =  index;
    LOGGER_INFO("stream index ->v:%d a:%d e:%d\n",
            this->VideoIndex,
            this->AudioIndex,
            this->ExtraIndex);
    return 1;
}
bool QPlayer::ReleaseAudioCodec(){
    if(this->PAudioCodecs != NULL){
        LOGGER_INFO("release audio codecs\n");
        codec_close(this->PAudioCodecs);
    }
    this->PAudioCodecs = NULL;
    FREE_MEM_P(this->AudioHeader);
    FREE_MEM_P(this->AudioStream);
    memset(&this->AudioAdtsContext, 0, sizeof(adts_context_t));
    return true;
}
void QPlayer::AddStreamEndDummy(){
    //just a empty fr struct
    struct fr_buf_info *dummyFr = NULL;
    char buf[4] ={0};
    struct fr_buf_info fr = {0};
    dummyFr =  &fr;
    dummyFr->virt_addr = buf;
    dummyFr->buf = buf;
    dummyFr->size = 4;
    dummyFr->priv = MEDIA_END_DUMMY_FRAME;
    dummyFr->timestamp = this->VPlayTs.GetTs(this->PlaySpeed);
    LOGGER_DBG("add stream end dummy last ts ->%8ld\n", dummyFr->timestamp);
    if(this->PDemuxVideoInfo != NULL){
        LOGGER_DBG("add video end dummy ->%p\n", this->PVideoFrFifo);
        dummyFr->timestamp += 5;
        this->PVideoFrFifo->PushItem(dummyFr);
        LOGGER_DBG("add video end dummy ok\n");
    }
#if 0
    if(this->PDemuxExtraInfo != NULL){
        LOGGER_DBG("add extra end dummy\n");
        fr.timestamp += 5;
        this->PExtraFrFifo->Push(fr);
    }
#endif
    if(this->PDemuxAudioInfo != NULL){
        LOGGER_DBG("add audio end dummy\n");
        dummyFr->timestamp += 5;
        this->PAudioFrFifo->PushItem(dummyFr);
        LOGGER_DBG("add audio end dummy ok\n");
    }
}

bool QPlayer::StepDisplay()
{
    LOGGER_INFO("Start step display!\n");
    if(this->GetWorkingState() == ThreadingState::Start)
    {
        this->SetWorkingState(ThreadingState::Pause);
    }
    else if(this->OpenNextMediaDemuxer(true) == false)
    {
        LOGGER_ERR("open next media file error\n");
        return false;
    }
    this->IsStepDisplay = true;
    this->SetWorkingState(ThreadingState::Start);

#ifndef VPLAY_PLAY_VIDEO_ONLY
    if(this->PDemuxAudioInfo != NULL)
    {
        if (this->PAudioPlayer->GetWorkingState() == ThreadingState::Start)
        {
            LOGGER_INFO("Step display, not enable audio!\n");
            this->PAudioPlayer->SetWorkingState(ThreadingState::Pause);
        }
    }
#endif
    if(this->PDemuxVideoInfo != NULL)
    {
        this->PSpeedCtrl->SetStepDisplay();
        this->PSpeedCtrl->SetWorkingState(ThreadingState::Start);
    }
    else
    {
        LOGGER_ERR("Can not find video info!\n");
        return false;
    }
    return true;
}

bool QPlayer::AudioMute(int Mute)
{
    LOGGER_INFO("Set Audio mute:%d\n", Mute);
    if (this->GetWorkingState() == ThreadingState::Start)
    {
        LOGGER_WARN("Not Supoort mute, when after play.\n");
        return false;
    }
    if (Mute > 0)
    {
        this->IsAudioMute = true;
    }
    else
    {
        this->IsAudioMute = false;
    }
    return true;
}

int vplay_remove_file(VPlayer *player, const char *file){
    assert(player != NULL);
    QPlayer *qPlayer  = (QPlayer *)player->inst;

    return 1;
}
int vplay_clean_queue(VPlayer *player){
    assert(player != NULL);
    QPlayer *qPlayer  = (QPlayer *)player->inst;

    return 1;
}
const char ** vplay_list_queue(VPlayer *player){
    assert(player != NULL);
    QPlayer *qPlayer  = (QPlayer *)player->inst;

    return NULL;
}
VPlayer * vplay_new_player(VPlayerInfo *info){
    QPlayer *qPlayer  = new QPlayer(info);
    if(qPlayer == NULL){
        LOGGER_ERR("init qplayer instance error\n");
        return NULL ;
    }
    LOGGER_INFO("new player instance %p\n", qPlayer);
    if(qPlayer->Prepare() == false ){
        delete qPlayer ;
        return NULL;
    }
    VPlayer *player = (VPlayer *) malloc(sizeof(VPlayer));
    if (player == NULL){
        delete qPlayer ;
        LOGGER_ERR("malloc vplayer mem error\n");
    }
    player->inst =  (void *)qPlayer ;
    return player;
}
int  vplay_update_player(VPlayerInfo *info){
    return 1;
}
int vplay_delete_player(VPlayer *player){
    assert(player != NULL);
    assert(player->inst != NULL);
    QPlayer * qplayer = (QPlayer *)player->inst;
    delete qplayer;
    free(player);
    player->inst = NULL;
    return 1;
}

int64_t  vplay_control_player(vplay_player_inst_t *player, vplay_ctrl_action_t action, void *para){//rate :only support 2x value
    assert(player != NULL);
    QPlayer *qPlayer  = (QPlayer *)player->inst;
    int64_t  ret = -1;
    int value;
    if (action != VPLAY_PLAYER_QUERY_MEDIA_INFO)
        LOGGER_INFO("vplay player control ->%d\n", action);
    switch(action){
        case VPLAY_PLAYER_PLAY:
            ret = qPlayer->Play(*(int *)para);
            break;
        case VPLAY_PLAYER_STOP :
            ret = qPlayer->Stop();
            break;
        case VPLAY_PLAYER_PAUSE :
            ret = qPlayer->Pause();
            break;
        case VPLAY_PLAYER_CONTINUE :
            ret = qPlayer->Continue(*(int *)para);
            break;
        case VPLAY_PLAYER_SEEK :
            ret = qPlayer->Seek(SEEK_SET,*(uint64_t *)para);
            break;
        case VPLAY_PLAYER_QUEUE_FILE :
            ret = qPlayer->AddMediaFile((char *)para);
            break;
        case VPLAY_PLAYER_QUERY_MEDIA_INFO :
            ret = qPlayer->QueryMediaFileInfo(NULL, (vplay_media_info_t *)para);
            break;
        case VPLAY_PLAYER_SET_VIDEO_FILTER :
            value = *(int32_t *)para;
            ret = qPlayer->SetVideoFilter(*(int32_t *)para);
            break;
        case VPLAY_PLAYER_SET_AUDIO_FILTER :
            value = *(int32_t *)para;
            ret = qPlayer->SetAudioFilter(*(int32_t *)para);
            break;
        case VPLAY_PLAYER_QUEUE_CLEAR:
            ret = qPlayer->ClearPlayList();
            break;

        case VPLAY_PLAYER_REMOVE_FILE:
            if(para == NULL){
                LOGGER_ERR("remove file from list ,the para must be file path, can not be null\n");
                ret = -1;
            }
            else{
                ret = qPlayer->RemoveFileFromPlayList((char *)para);
            }
            break;
        case VPLAY_PLAYER_STEP_DISPLAY:
            {
                ret = qPlayer->StepDisplay();
                break;
            }
        case VPLAY_PLAYER_MUTE_AUDIO:
            {
                ret = qPlayer->AudioMute(*(int *)para);
                break;
            }
        default :
            LOGGER_DBG("vplay play action no handle\n");
            break;
    }
    return ret;
}
int vplay_audio_alert(char *audio_channel, char file_or_buffer, audio_fmt_t *fmt){
    if(fmt != NULL){
        LOGGER_INFO("vplay play raw audio pcm\n");
    }
    else{
        LOGGER_INFO("vplay play media audio file\n");
    }
}
int vplay_query_media_info(char *file_name, vplay_media_info_t *info){
    if(file_name == NULL){
        LOGGER_ERR("file path can be null\n");
        return -1;
    }
    if(info == NULL){
        LOGGER_ERR("file info can be null\n");
        return -1;
    }
    vplay_file_info_t file = {0};
    int len = strlen(file_name);
    if(len >= VPLAY_PATH_MAX){
        LOGGER_ERR("file path is too long exteed ->%d\n", len, VPLAY_PATH_MAX);
        return -1;
    }
    memcpy(file.file, file_name, len);
    demux_t *demuxer = open_media_demuxer(&file, info);
    if(demuxer == NULL){
        LOGGER_ERR("open media file error ->%s\n", file.file);
        return -1;
    }
    demux_deinit(demuxer);
    return 1;

}
