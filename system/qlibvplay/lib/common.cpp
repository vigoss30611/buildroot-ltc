#include <define.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <common.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define TIME_INTERVAL 1500

VideoStreamInfo::VideoStreamInfo(){
    this->MutexLock = new std::mutex();
    this->Type = VIDEO_TYPE_H264;
    this->Width = 0;
    this->Height = 0;
    this->Fps = 0;
    this->BitRate = 0;
    this->FrameSize = 0;
    this->PFrame = 0;
    this->GopLength = 0;
    this->HeaderSize = 0;
    this->Update =  false;
}
VideoStreamInfo::~VideoStreamInfo(){
    this->MutexLock->lock();
    this->MutexLock->unlock();
    delete this->MutexLock ;
}
/*
VideoStreamInfo::VideoStreamInfo(const VideoStreamInfo &info):VideoStreamInfo(){
    this->MutexLock = new std::mutex();
    this->Type = info.GetType();
    this->width = info.GetWidth();
    this->Height = info.GetHeight();
    this->Fps = info.GetFps();
    this->BitRate = info.GetBitRate();
    this->FrameSize = info.GetFrameSize();
    this->PFrame = info.GetPFrame();

    this->GopLength = info.GetGopLength();
    info.GetHeader(this->Header, this->HeaderSize);
    this->DynamicUpdate =  true;
}
*/
video_type_t VideoStreamInfo::GetType(){
    video_type_t ret ;
    this->MutexLock->lock();
    ret = this->Type;
    this->MutexLock->unlock();
    return ret;
}
void VideoStreamInfo::SetType(video_type_t dat){
    this->MutexLock->lock();
    this->Type = dat;
    this->MutexLock->unlock();
}
int VideoStreamInfo::GetWidth(){
    int ret = 0;
    this->MutexLock->lock();
    ret = this->Width;
    this->MutexLock->unlock();
    return ret;
}
void VideoStreamInfo::SetWidth(int width){
    this->MutexLock->lock();
    this->Width = width;
    this->MutexLock->unlock();
}
int VideoStreamInfo::GetHeight(){
    int ret = 0;
    this->MutexLock->lock();
    ret = this->Height;
    this->MutexLock->unlock();
    return ret;
}
void VideoStreamInfo::SetHeight(int dat){
    this->MutexLock->lock();
    this->Height = dat;
    this->MutexLock->unlock();
}
int VideoStreamInfo::GetFps(){
    int ret = 0;
    this->MutexLock->lock();
    ret = this->Fps;
    this->MutexLock->unlock();
    return ret;
}
void VideoStreamInfo::SetFps(int dat){
    this->MutexLock->lock();
    this->Fps = dat;
    this->MutexLock->unlock();
}
int VideoStreamInfo::GetBitRate(){
    int ret = 0;
    this->MutexLock->lock();
    ret = this->BitRate;
    this->MutexLock->unlock();
    return ret;
}
void VideoStreamInfo::SetBitRate(int dat){
    this->MutexLock->lock();
    this->BitRate = dat;
    this->MutexLock->unlock();
}
int VideoStreamInfo::GetFrameSize(){
    int ret = 0;
    this->MutexLock->lock();
    ret = this->FrameSize;
    this->MutexLock->unlock();
    return ret;
}
void VideoStreamInfo::SetFrameSize(int dat){
    this->MutexLock->lock();
    this->FrameSize = dat;
    this->MutexLock->unlock();
}
int VideoStreamInfo::GetPFrame(){
    int ret = 0;
    this->MutexLock->lock();
    ret = this->PFrame;
    this->MutexLock->unlock();
    return ret;
}
void VideoStreamInfo::SetPFrame(int dat){
    this->MutexLock->lock();
    this->PFrame = dat;
    this->MutexLock->unlock();
}
int VideoStreamInfo::GetGopLength(){
    int ret = 0;
    this->MutexLock->lock();
    ret = this->GopLength;
    this->MutexLock->unlock();
    return ret;
}
void VideoStreamInfo::SetGopLength(int dat){
    this->MutexLock->lock();
    this->GopLength = dat;
    this->MutexLock->unlock();
}
int VideoStreamInfo::GetHeaderSize(){
    int ret = 0;
    this->MutexLock->lock();
    ret = this->HeaderSize;
    this->MutexLock->unlock();
    return ret;
}

char *VideoStreamInfo::GetHeader(char *buf, int *size){
    if(buf == NULL || size == NULL)
    {
        LOGGER_ERR("get header error ->%p %p\n",buf,size);
        return NULL;
    }
    
    int ret = 0;
    this->MutexLock->lock();
    if(this->HeaderSize <= 0){
        LOGGER_ERR("header buf not ready,please update it first\n");
        this->MutexLock->unlock();
        *size = 0;
        return NULL;
    }
    else {
        memcpy(buf, this->Header, this->HeaderSize);
        *size = this->HeaderSize;
    }
    this->MutexLock->unlock();
    return buf;
}
char *VideoStreamInfo::SetHeader(char *buf, int size){
    if(buf == NULL || size <= 0)
    {
        LOGGER_ERR("get header error ->%p %p\n",buf,size);
        return NULL;
    }
    this->MutexLock->lock();

    memcpy(this->Header, buf, size);
    this->HeaderSize =  size;

    this->MutexLock->unlock();
}
void VideoStreamInfo::PrintInfo(void){
    int Width;
    int Heigh;
    int Fps;
    int BitRate;
    int FrameSize;
    int PFrame;
    int GopLength;
    char Header[128];
    int HeaderSize;

    printf("VideoStream info -->\n");
    printf("\t Update     -> %d\n",this->GetUpdate());
    printf("\t Type     -> %d\n",this->GetType());
    printf("\t Width     -> %d\n",this->GetWidth());
    printf("\t Height    -> %d\n",this->GetHeight());
    printf("\t Fps       -> %d\n",this->GetFps());
    printf("\t BitRate     -> %d\n",this->GetBitRate());
    printf("\t FrameSize -> %d\n",this->GetFrameSize());
    printf("\t PFrame      -> %d\n",this->GetPFrame());
    printf("\t GopLenght -> %d\n",this->GetGopLength());
    printf("\t HeaderSize-> %d\n",this->GetHeaderSize());
    printf("\t header info begin");
    this->MutexLock->lock();
    for(int i = 0; i < this->HeaderSize; i++){
        if(i % 10 == 0)
            printf("\n\t\t");
        printf("%3X",this->Header[i]);
    }
    printf("\n\theader info end\n");
    this->MutexLock->unlock();
}

void VideoStreamInfo::SetUpdate(bool flag){
    this->MutexLock->lock();
    this->Update = flag;
    this->MutexLock->unlock();
}
bool VideoStreamInfo::GetUpdate(){
    this->MutexLock->lock();
    bool ret = this->Update;
    this->MutexLock->unlock();
    return ret;
}
AudioStreamInfo::AudioStreamInfo(){
    int codec_type;
    int effect;
    int channel;
    int sample_rate;
    int bitwidth;
    int bit_rate;
    codec_fmt_t *fmt = NULL;
    this->MutexLock = new std::mutex();
    fmt = &this->AudioInFmt;
    fmt->codec_type = AUDIO_CODEC_PCM;
    fmt->effect = 0;
    fmt->channel = 2;
    fmt->sample_rate = 16000;
    fmt->bitwidth = 16;//may should use simple format
    CalAudioBitRate(fmt);
    fmt = &this->AudioOutFmt;
    fmt->codec_type = AUDIO_CODEC_PCM;
    fmt->effect = 0;
    fmt->channel = 2;
    fmt->sample_rate = 16000;
    fmt->bitwidth = 16;//may should use simple format
    CalAudioBitRate(fmt);
    this->AudioInExtraDataSize = 0;
    this->AudioOutExtraDataSize = 0;
    this->Update = false;
}
AudioStreamInfo::~AudioStreamInfo(){
    this->MutexLock->lock();
    this->MutexLock->unlock();
    delete this->MutexLock ;
}

codec_fmt_t  AudioStreamInfo::GetInType(){
    codec_fmt_t fmt ;
    this->MutexLock->lock();
    fmt = this->AudioInFmt;
    this->MutexLock->unlock();
    return fmt;

}
void  AudioStreamInfo::SetInType(codec_fmt_t *fmt){
    this->MutexLock->lock();
    this->AudioInFmt = *fmt;
    this->MutexLock->unlock();
}

codec_fmt_t  AudioStreamInfo::GetOutType(){
    codec_fmt_t fmt ;
    this->MutexLock->lock();
    fmt = this->AudioOutFmt;
    this->MutexLock->unlock();
    return fmt;

}
void  AudioStreamInfo::SetOutType(codec_fmt_t *fmt){
    this->MutexLock->lock();
    this->AudioOutFmt = *fmt;
    this->MutexLock->unlock();
}

int AudioStreamInfo::GetInExtraData(char *buf, int bufSize){
    int size = 0;
    this->MutexLock->lock();
    if(bufSize < this->AudioInExtraDataSize){
        LOGGER_ERR("your extra buffer(%d %d) is too lower\n", bufSize, this->AudioInExtraDataSize);
        size = 0;
    }
    else{
        size = this->AudioInExtraDataSize;
        if(this->AudioInExtraDataSize == 0 ){
            LOGGER_WARN("in audio has no extra data\n");
        }
        else{ 
            memcpy(buf, this->AudioInExtraData, this->AudioInExtraDataSize);
        }
    }
    this->MutexLock->unlock();
    return size;
}
int AudioStreamInfo::SetInExtraData(char *buf, int bufSize){
    int size = 0;
    if(bufSize > AUDIO_EXTRA_DATA_MAX_SIZE){
        LOGGER_ERR("your extra buffer is larget(%d) than reserved space,please modify AUDIO_EXTRA_DATA_MAX_SIZE\n",
            bufSize);
        return 0;
    }
    if(bufSize == 0){
        LOGGER_ERR("your extra buffer can not be 0\n");
        return 0;
    }
    this->MutexLock->lock();
    memcpy(this->AudioInExtraData, buf, bufSize);
    this->AudioInExtraDataSize = bufSize;
    this->MutexLock->unlock();
    return bufSize;
}
int AudioStreamInfo::GetOutExtraData(char *buf, int bufSize){
    int size = 0;
    this->MutexLock->lock();
    if(bufSize < this->AudioOutExtraDataSize){
        LOGGER_ERR("your extra buffer(%d %d) is to lower\n", bufSize, this->AudioOutExtraDataSize);
        size = 0;
    }
    else{
        size = this->AudioOutExtraDataSize;
        if(this->AudioOutExtraDataSize == 0 ){
            LOGGER_WARN("out audio has no extra data\n");
        }
        else{ 
            memcpy(buf, this->AudioOutExtraData, this->AudioOutExtraDataSize);
        }
    }
    this->MutexLock->unlock();
    return size;
}
int AudioStreamInfo::SetOutExtraData(char *buf, int bufSize){
    int size = 0;
    if(bufSize > AUDIO_EXTRA_DATA_MAX_SIZE){
        LOGGER_ERR("extra buffer(%d) is larget than reserved space.\n", bufSize);
        return 0;
    }
    if(bufSize == 0){
        LOGGER_ERR("your extra buffer can not be 0\n");
        return 0;
    }
    this->MutexLock->lock();
    memcpy(this->AudioInExtraData, buf, bufSize);
    this->AudioInExtraDataSize = bufSize;
    this->MutexLock->unlock();
    return bufSize;
}
void AudioStreamInfo::PrintInfo(void){
    codec_fmt_t *fmt = NULL;
    printf("AudioStreamInfo In----->\n");
    fmt = &this->AudioInFmt;
    printf("\t type ->%3d\n",fmt->codec_type);
    printf("\t effect ->%3d\n",fmt->effect);
    printf("\t channel ->%3d\n",fmt->channel);
    printf("\t sample_rate ->%3d\n",fmt->sample_rate);
    printf("\t bitwidth ->%3d\n",fmt->bitwidth);
    printf("\t bit_rate: %2d ->\n", fmt->bit_rate);
    printf("\t extra data: %2d ->", this->AudioInExtraDataSize);
    for(int i = 0; i < this->AudioInExtraDataSize; i++){
        printf("%3X", this->AudioInExtraData[i]);
    }
    printf("\n");
    printf("AudioStreamInfo Out----->\n");
    fmt = &this->AudioInFmt;
    printf("\t type ->%3d\n",fmt->codec_type);
    printf("\t effect ->%3d\n",fmt->effect);
    printf("\t channel ->%3d\n",fmt->channel);
    printf("\t sample_rate ->%3d\n",fmt->sample_rate);
    printf("\t bitwidth ->%3d\n",fmt->bitwidth);
    printf("\t bit_rate: %2d ->\n", fmt->bit_rate);
    printf("\t extra data: %2d ->", this->AudioOutExtraDataSize);
    for(int i = 0; i < this->AudioOutExtraDataSize; i++){
        printf("%3X", this->AudioOutExtraData[i]);
    }
    printf("\n");
}
int  AudioStreamInfo::CalAudioBitRate(codec_fmt_t *fmt){
    fmt->bit_rate = fmt->sample_rate * fmt->channel * fmt->bitwidth;
    return fmt->bit_rate;
}

void AudioStreamInfo::SetUpdate(bool flag){
    this->MutexLock->lock();
    this->Update = flag;
    this->MutexLock->unlock();
}
bool AudioStreamInfo::GetUpdate(){
    this->MutexLock->lock();
    bool ret = this->Update;
    this->MutexLock->unlock();
    return ret;
}
void AudioStreamInfo::SetChannel(char *channel){
    int len = strlen(channel);
    if(len >= VPLAY_CHANNEL_MAX){
        LOGGER_ERR("audio channel(%d) is too length\n", len);
        assert(1);
    }
    this->MutexLock->lock();
    memset(this->AudioChannel, 0, sizeof(this->AudioChannel));
    memcpy(this->AudioChannel, channel, len);
    this->MutexLock->unlock();
}
void AudioStreamInfo::GetChannel(char *channel, int bufSize){
    int len = strlen(this->AudioChannel);
    if(len >= bufSize){
        LOGGER_ERR("audio channel buffer(%d %d) is not enough\n", len, bufSize);
        assert(1);
    }
    this->MutexLock->lock();
    memset(channel, 0, bufSize);
    memcpy(channel, this->AudioChannel, len);
    this->MutexLock->unlock();
}
VplayTimeStamp::VplayTimeStamp(){
    this->FirstTs = 0;
    this->CurSysTs = 0;
    this->PlayTs = 0;
    this->AutoStartTs = 0;
    this->LastGetTs = 0;
    this->AutoUpdate = false;
    this->Stopped = false;
    this->Rate = 1;
    this->enStatus = TIMESTAMP_STATUS_READY;
}
VplayTimeStamp::~VplayTimeStamp(){

}
void VplayTimeStamp::Start(void){
    this->AccessLock.lock();
    uint64_t ts = get_timestamp();
    LOGGER_INFO("start ts -> %8lld\n", ts);
    if (this->enStatus != TIMESTAMP_STATUS_PAUSE)
    {
        this->FirstTs = ts;
        this->PlayTs = 0;
    }
    this->CurSysTs = ts;
    this->AutoStartTs = 0;
    this->LastGetTs = ts;
    this->Stopped = false;
    this->AutoUpdate = false;
    this->enStatus = TIMESTAMP_STATUS_START;
    this->AccessLock.unlock();
}
void VplayTimeStamp::Stop(void){
    this->AccessLock.lock();
    this->FirstTs = 0;
    this->CurSysTs = 0;
    this->PlayTs = 0;
    this->AutoStartTs = 0;
    this->LastGetTs = 0;
    this->Stopped = true;
    this->AutoUpdate = false;
    this->Rate = 1;
    this->enStatus = TIMESTAMP_STATUS_STOP;
    this->AccessLock.unlock();
}

void VplayTimeStamp::Pause(bool bIsPaused)
{
    this->AccessLock.lock();
    if (bIsPaused)
    {
        this->enStatus = TIMESTAMP_STATUS_PAUSE;
    }
    else
    {
        if (this->enStatus == TIMESTAMP_STATUS_PAUSE)
        {
            this->LastGetTs = get_timestamp();
        }
        this->enStatus = TIMESTAMP_STATUS_START;
    }
    this->AccessLock.unlock();
}

void VplayTimeStamp::UpdateTs(uint64_t ts){
    this->AccessLock.lock();
    uint64_t SysTime = get_timestamp();
    if(ts == 0){
        uint64_t IntervalTime = 0;
        if (this->LastGetTs > this->CurSysTs)
        {
            IntervalTime = SysTime - this->LastGetTs;
        }
        else
        {
            IntervalTime = SysTime - this->CurSysTs;
        }
        IntervalTime = AdjustTimeByRate(IntervalTime, this->Rate);
        ts = this->PlayTs + IntervalTime;
        //LOGGER_INFO("update ts -> %8llu playts -> %8llu interval -> %8llu SysTime -> %8llu CurSysTs -> %8llu\n",
        //    ts, this->PlayTs, IntervalTime, SysTime, this->CurSysTs);
    }
    this->CurSysTs = SysTime;
    this->PlayTs = ts;
    this->AutoUpdate = false;
    this->AutoStartTs = 0;
    this->LastGetTs = SysTime;
    this->AccessLock.unlock();
}

uint64_t VplayTimeStamp::GetTs(int rate){
    this->AccessLock.lock();
    uint64_t ts = this->PlayTs;
    uint64_t SysTime = get_timestamp();
    if (this->AutoUpdate)
    {
        ts = this->PlayTs + (SysTime - this->AutoStartTs);
    }
    else
    {
        if (this->enStatus == TIMESTAMP_STATUS_START)
        {
            ts += AdjustTimeByRate(SysTime - this->LastGetTs, rate);
            //LOGGER_INFO("get play ts -> %8llu ts -> %8llu sys -> %8llu last -> %8llu jetter -> %8llu rate -> %d\n",
            //    this->PlayTs, ts ,SysTime, this->LastGetTs, AdjustTimeByRate(SysTime - this->LastGetTs, rate), rate);
            this->PlayTs = ts;
            this->LastGetTs = SysTime;
        }
    }
    this->AccessLock.unlock();
    return ts;
}

bool VplayTimeStamp::IsStopped(){
    bool state = true;
    this->AccessLock.lock();
    state = this->Stopped;
    this->AccessLock.unlock();
    return state;
}

void VplayTimeStamp::Seek(uint64_t ts){
    this->AccessLock.lock();
    this->CurSysTs = get_timestamp();
    //this->FirstTs =  this->CurSysTs - this->PlayTs;
    this->PlayTs = ts;
    this->AutoUpdate = false;
    this->AutoStartTs = 0;
    this->AccessLock.unlock();
}

void VplayTimeStamp::EnableAutoUpdate()
{
    this->AccessLock.lock();
    this->AutoUpdate = true;
    this->AutoStartTs = get_timestamp();
    this->AccessLock.unlock();
}

Statistics::Statistics()
{
    this->StatType = STAT_TYPE_PLAYBACK;
    this->LastTS = 0;

    this->VideoFrames = 0;
    this->VideoPTS = 0;
    this->VQueueSize = 0;
    this->VDropFrames = 0;

    this->AudioFrames = 0;
    this->AudioPTS = 0;
    this->AQueueSize = 0;
    this->ADropFrames = 0;

    this->LastVFrames = 0;
    this->LastVPTS = 0;
    this->FirstVPTS = 0;
    this->PeriodVSize = 0;
    this->TotalVSize = 0;
}

void Statistics::SetStatType(Stat_Type StatType)
{
    this->StatType = StatType;
}

void Statistics::ShowInfo()
{
    uint64_t systemTime = get_timestamp();
    if (systemTime - this->LastTS > TIME_INTERVAL)
    {
        int videoFrames = this->VideoFrames, videoDropFrames = this->VDropFrames;
        int audioFrames = this->AudioFrames, audioDropFrames = this->ADropFrames;
        uint64_t videoPTS = this->VideoPTS, audioPTS = this->AudioPTS, refTime = this->RefPTS;
        int videoQuqueSize = this->VQueueSize, audioQueueSize = this->AQueueSize;
        int videoFrameRate = 0;

        videoFrameRate = (videoFrames - this->LastVFrames) * 1000 /  (systemTime - this->LastTS);
        if (this->StatType == STAT_TYPE_RECORDE)
        {
            uint64_t videoBiteRate = 0, avgVideoBiteRate = 0;

            if (videoPTS - this->LastVPTS > 0)
            {
                videoBiteRate = this->PeriodVSize * 1000 / (videoPTS - this->LastVPTS);
            }
            if (videoPTS - this->FirstVPTS > 0)
            {
                avgVideoBiteRate = this->TotalVSize * 1000 / (videoPTS - this->FirstVPTS);
            }
            LOGGER_INFO("[%p]VFr:%d(%d), AFr:%d(%d),VPTS:%llu,APTS:%llu,STC:%llu,VQ:%d,AQ:%d,FPS:%d,VRate:%lluBps,A-VRate:%lluBps\n",
                this,
                videoFrames, videoDropFrames, audioFrames, audioDropFrames,
                videoPTS, audioPTS, systemTime,
                videoQuqueSize, audioQueueSize,
                videoFrameRate, videoBiteRate, avgVideoBiteRate);
            this->LastTS = systemTime;
            this->LastVFrames = videoFrames;
            this->PeriodVSize = 0;
            this->LastVPTS = videoPTS;
        }
        else if(this->StatType == STAT_TYPE_PLAYBACK)
        {
            audioFrames = this->AudioFrames - this->VDropFrames - this->AQueueSize;
            LOGGER_INFO("[%p]VFr:%d(%d), AFr:%d(%d),REF:%llu,VPTS:%llu,APTS:%llu,STC:%llu,VQ:%d,AQ:%d,FPS:%d\n",
                this,
                videoFrames, videoDropFrames, audioFrames, audioDropFrames,
                refTime, videoPTS, audioPTS, systemTime,
                videoQuqueSize, audioQueueSize,
                videoFrameRate);
            this->LastTS = systemTime;
            this->LastVFrames = videoFrames;
            this->PeriodVSize = 0;
            this->LastVPTS = videoPTS;
        }
    }
}

bool Statistics::UpdatePacketInfo(Packet_Type type, int pktSize, unsigned long long pktPTS, int streamIndex)
{
    if (this->StatType == STAT_TYPE_RECORDE)
    {
        if ((type == PACKET_TYPE_VIDEO) && (streamIndex == 0))
        {
            this->VideoFrames ++;
            this->VideoPTS = pktPTS;
            if (this->FirstVPTS == 0)
            {
                this->FirstVPTS = pktPTS;
            }
            this->PeriodVSize += pktSize;
            this->TotalVSize += pktSize;
        }
        else if(type == PACKET_TYPE_AUDIO)
        {
            this->AudioFrames ++;
            this->AudioPTS = pktPTS;
        }
        return true;
    }
    else if (this->StatType == STAT_TYPE_PLAYBACK)
    {
        //playback should not enter this...
    }
    return false;
}

bool Statistics::UpdatePacketPTS(Packet_Type type, uint64_t pktPTS)
{
    if (this->StatType == STAT_TYPE_PLAYBACK)
    {
        if (type == PACKET_TYPE_AUDIO)
        {
            this->AudioFrames ++;
            this->AudioPTS = pktPTS;
        }
        else if (type == PACKET_TYPE_VIDEO)
        {
            this->VideoFrames ++;
            this->VideoPTS = pktPTS;
            if (this->FirstVPTS == 0)
            {
                this->FirstVPTS = pktPTS;
            }
        }
        return true;
    }
    return false;
}

bool Statistics::UpdateFifoInfo(Packet_Type type, int Qsize, int Dropframes)
{
    if (type == PACKET_TYPE_VIDEO)
    {
        this->VQueueSize = Qsize;
        this->VDropFrames = Dropframes;
    }
    else if(type == PACKET_TYPE_AUDIO)
    {
        this->AQueueSize = Qsize;
        this->ADropFrames = Dropframes;
    }

    return true;
}

bool Statistics::UpdateRefTime(uint64_t Pts)
{
    this->RefPTS = Pts;
    return true;
}

void VplayTimeStamp::SetPlaySpeed(int rate)
{
    this->AccessLock.lock();
    this->Rate = rate;
    this->CurSysTs = get_timestamp();
    this->AccessLock.unlock();
}

uint64_t VplayTimeStamp::AdjustTimeByRate(uint64_t time, int rate)
{
    uint64_t ts = time;
    uint64_t speed = 0;

    if (rate == 0)
        return ts;
    // low speed play
    if(rate < 0 ){
        speed = abs(rate);
        ts = ts / speed;
    }
    //high speed play
    else {
        speed = rate ;
        ts = ts * speed ;
    }
    return ts;
}

int save_tmp_file(int index, void *buf, int size, char *prefix){
    static int flag = 0 ;
    static int fd_list[5] = {-1};
    static int counter_list[5] = {0};
    if(flag == 0 ){
        memset(fd_list,-1,sizeof(int)*5 );
        flag = 1;
    }
    if(index >=5 ){
        LOGGER_ERR("exceet the max fd list %d\n", index);
        return -1;
    }
    char temp[32];
    if(access("/tmp/debug",R_OK) != 0 ){
//        printf("not debug state ,no save \n");
        return 0;
    }
    sprintf(temp,"/tmp/rec_%d",index);
    if(access(temp,R_OK) != 0 ){
        return 0;
    }
    else 
    {    
        sprintf(temp,"/tmp/del_%d",index);
        if(access(temp,R_OK) == 0 ){
            if(fd_list[index] != -1 ){
                close(fd_list[index]);
                fd_list[index] = -1;
            }
            remove(temp);
        }

            //remove(temp);
    }
#if 1
    sprintf(temp,"/nfs/frame_%d_%3d", index, counter_list[index]++);    
    if(prefix != NULL){
        strcat(temp, prefix);
    }
    for(int i =0;temp[i] != 0;i++){
        if(temp[i] == ' '){
            temp[i]= '0';
        }
    }
    int fd =  open(temp,O_WRONLY|O_CREAT);
    if(fd < 0 ){
        printf("open frame file error ->%s\n\n", temp);
        return 0;
    }
    write(fd,buf,size);
    close(fd);

    return 1;
#else 
    if ( fd_list[index ] == -1){
        LOGGER_ERR("++++++++++++++++++++++++++++\n");
        sprintf(temp,"/nfs/temp_%d",index);
        if(prefix != NULL){
            strcat(temp, prefix);
        }
        fd_list[index] =  open(temp,O_WRONLY|O_CREAT);
        printf("start new record,and delete flag file\n");
        if(fd_list < 0){
            LOGGER_ERR("open temp file error ->%d %s\n",index,temp);
            return -1;
        }
    //    LOGGER_ERR("open trace temp file open ->%2d %2d->%s\n",index,fd_list[index],temp);
    }
    int result  = write(fd_list[index],buf,size);
    if(result !=   size ){
        LOGGER_ERR("write error ->%2d %4d %4d\n",index,size,result);
    }
    return result ;
#endif
}
uint64_t  get_timestamp(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return  ts.tv_sec * 1000ull + ts.tv_nsec / 1000000;
}
static const uint32_t crc32tab[] = {
 0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
 0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
 0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
 0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
 0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
 0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
 0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
 0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
 0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
 0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};



uint32_t calCrc32(  char *buf, uint32_t size)
{
     uint32_t i, crc;
     crc = 0xFFFFFFFF;
     for (i = 0; i < size; i++)
      crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
     return crc^0xFFFFFFFF;
}
