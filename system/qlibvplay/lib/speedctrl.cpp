#include <sys/prctl.h>
#include <speedctrl.h>
#include <qsdk/vplay.h>
#include <qsdk/videobox.h>
#include <define.h>
#include <stdlib.h>
#include <string.h>

ThreadingState SpeedCtrl::WorkingThreadTask(void ){
    int ret = 0,ret2;
    if(this->UpdateVideoFrameFlag == true ){
        if(this->PVideoFrFifo->GetItem(&this->VideoFrame) <= 0){
            usleep(5000);
            return ThreadingState::Keep;
        }
    }
    if(this->VideoFrame.priv == MEDIA_END_DUMMY_FRAME){
        LOGGER_INFO("###read the video dummy stream\n");
        this->PVPlayTs->EnableAutoUpdate();
        return ThreadingState::Keep;
    }
#if 0
    if(this->VideoFrame.size == 0){
            usleep(5000);
            return ThreadingState::Keep;

    }
#endif
    //printf("video frame ->%3X %8lld\n", this->VideoFrame.priv, this->VideoFrame.timestamp);
    if(this->VideoFrame.priv == VIDEO_FRAME_HEADER){
        ret = video_set_header(this->PlayerInfo.video_channel,
                    (char *)this->VideoFrame.virt_addr,
                    this->VideoFrame.size);
        this->PVideoFrFifo->PutItem(&this->VideoFrame);
        if(ret < 0){
            LOGGER_ERR("set new video header error\n");
        }
        else {
            LOGGER_INFO("set video header ok\n");
            this->PVPlayTs->Start();
            if (this->AutoUpdateTs.load() == true)
            {
                LOGGER_INFO("Set first pts:%llu\n", this->VideoFrame.timestamp);
                this->PVPlayTs->UpdateTs(this->VideoFrame.timestamp);
            }
        }
        return ThreadingState::Keep;
    }
    if (this->StepDisplay)
    {
        LOGGER_INFO("Put one frame, size:%d\n", this->VideoFrame.size);
        ret = video_write_frame(this->PlayerInfo.video_channel,
                        (char *)this->VideoFrame.virt_addr,
                        this->VideoFrame.size);
        if(ret < 0){
                LOGGER_ERR("write video frame error ->%d->%s\n",this->VideoFrame.size,this->PlayerInfo.video_channel);
        }
        if (this->stat)
        {
            this->stat->UpdatePacketPTS(PACKET_TYPE_VIDEO, this->VideoFrame.timestamp);
            this->stat->UpdateFifoInfo(PACKET_TYPE_VIDEO, this->PVideoFrFifo->GetQueueSize(),
                this->PVideoFrFifo->GetDropFrameSize());
        }

        this->PVideoFrFifo->PutItem(&this->VideoFrame);
        this->UpdateVideoFrameFlag =  true;

        this->StepDisplay = false;
        vplay_report_event(VPLAY_EVENT_STEP_DISPLAY_FINISH,(char*)"step display finish");
        LOGGER_INFO("Step Display finished\n");
        return ThreadingState::Pause;
    }
    //begin check video  info
    //LOGGER_DBG("check video frame ->ts:%8lld  size:%8d\n", this->VideoFrame.timestamp, this->VideoFrame.size);
    ret = this->CheckVideoPlayTimestamp(this->VideoFrame.timestamp,this->VideoFrame.priv);
    if(this->PlaySpeed != 1){
        this->PVPlayTs->UpdateTs(0);
    }
    else if(this->AutoUpdateTs.load() == true){
        this->PVPlayTs->UpdateTs(0);
    }
    if(ret == 1){ //play video
        if(this->VideoFrame.priv == VIDEO_FRAME_HEADER){
            if( video_set_header(this->PlayerInfo.video_channel,
                        (char *)this->VideoFrame.virt_addr,
                        this->VideoFrame.size) < 0){
                LOGGER_ERR("set new video header error\n");
            }
            else {
                LOGGER_DBG("set video header ok\n");
                this->PVPlayTs->Start();
                if (this->stat)
                {
                    this->stat->UpdatePacketPTS(PACKET_TYPE_VIDEO, this->VideoFrame.timestamp);
                    this->stat->UpdateFifoInfo(PACKET_TYPE_VIDEO,this->PVideoFrFifo->GetQueueSize(),
                        this->PVideoFrFifo->GetDropFrameSize());
                }
            }
        }
        else {
           // LOGGER_DBG("write video frame-->%8lld: %8lld %3d\n", this->VideoFrame.timestamp, this->PVPlayTs->GetTs(), this->PVideoFrFifo->GetQueueSize());
#if 1
            uint64_t oldTs = get_timestamp();
         //   printf("write video frame ->%3X %8lld\n", this->VideoFrame.priv, this->VideoFrame.timestamp);
            ret = video_write_frame(this->PlayerInfo.video_channel,
                        (char *)this->VideoFrame.virt_addr,
                        this->VideoFrame.size);
            if(ret < 0){
                vplay_dump_buffer((char*)this->VideoFrame.virt_addr,this->VideoFrame.size,"Video error buffer");
                LOGGER_ERR("write video frame error ->%d->%s ret:%d ts:%llu\n",
                    this->VideoFrame.size, this->PlayerInfo.video_channel, ret, this->VideoFrame.timestamp);
            }
            if (this->stat)
            {
                this->stat->UpdatePacketPTS(PACKET_TYPE_VIDEO, this->VideoFrame.timestamp);
                this->stat->UpdateFifoInfo(PACKET_TYPE_VIDEO, this->PVideoFrFifo->GetQueueSize(),
                    this->PVideoFrFifo->GetDropFrameSize());
            }
            this->PVideoFrFifo->PutItem(&this->VideoFrame);
     //       printf("write video ->ok \n");
            uint64_t newTs = get_timestamp();
            if((newTs - oldTs ) < 5){
                //add custom delay to avoid fr float mode flush bug, should be fixed in videobox
                usleep(15000);
            }
#endif
        }
        this->UpdateVideoFrameFlag =  true;
    }
    else if(ret == 2 ){//just skip
        this->PVideoFrFifo->PutItem(&this->VideoFrame);
        //do nothing and just skip
        this->UpdateVideoFrameFlag =  true;
    }
    else if(ret == 0){//wait for play
        this->UpdateVideoFrameFlag =  false;
        if(this->PVPlayTs->IsStopped() == true){
            LOGGER_INFO("audio stopped and stop video\n");
            this->UpdateVideoFrameFlag =  true;
            return ThreadingState::Pause;
        }
    }
    else { //error happend
        this->PVideoFrFifo->PutItem(&this->VideoFrame);
        LOGGER_ERR("check video timestamp error\n");
        this->SetWorkingState(ThreadingState::Pause);
    }
    return ThreadingState::Keep;
}
SpeedCtrl::SpeedCtrl(VPlayerInfo *info):ThreadCore(){
    this->PlayerInfo = *info ;
//    this->AudioInfo = *audio ;

    this->PVideoFrFifo = NULL;


    this->UpdateVideoFrameFlag = true ;
    this->PlaySpeed = 1;
    this->PVPlayTs = NULL;
    this->CurPFrameNum = 200;//magic number .no video has I/P 200
    this->SkipPFrameIndex = 200;
    this->SetName((char *)"vplayer");
    this->AutoUpdateTs.store(false);
    this->StepDisplay = false;
}
SpeedCtrl::~SpeedCtrl(){
    LOGGER_DBG("deinit vplayer -----\n");
#if 0
    if(this->GetWorkingState() != ThreadingState::Stop){
        this->SetWorkingState(ThreadingState::Stop);
    }
#endif
    this->Release();

}
bool SpeedCtrl::ThreadPreStartHook(void){
    LOGGER_INFO("ThreadPreStartHook!\n");
    return true;
}
bool SpeedCtrl::ThreadPostPauseHook(void){
    LOGGER_INFO("ThreadPostPauseHook!\n");
    return true;
}
bool SpeedCtrl::ThreadPostStopHook(void){
    LOGGER_INFO("ThreadPostStopHook!\n");
    if(this->UpdateVideoFrameFlag == false)
    {
        LOGGER_ERR("drop keep frame!\n");
        this->PVideoFrFifo->PutItem(&this->VideoFrame);
        this->UpdateVideoFrameFlag = true;
    }
    this->StepDisplay = false;
    return true;
}
bool SpeedCtrl::Prepare(void){
    this->SetWorkingState(ThreadingState::Standby);
    if(this->PVideoFrFifo == NULL){
        LOGGER_ERR("video fr fifo is null\n");
        return false ;
    }

    memset(&this->VideoFrame,0,sizeof(struct fr_buf_info));
    return true;
}
bool SpeedCtrl::Release(void){
    this->SetWorkingState(ThreadingState::Stop);
    return true;
}
bool SpeedCtrl::Update(VPlayerInfo *info ,audio_info_t *audio){

    return true;
}

bool SpeedCtrl::SetPlaySpeed(int speed){
    if(speed == 1 ){
        this->PlaySpeed = speed ;
        this->SkipPFrameIndex = this->CurPFrameNum / this->PlaySpeed;
        return true ;

    }
    else {
        int temp =abs(speed);
        if(temp % 2 != 0){
            LOGGER_ERR("play speed can only set 1 ,+2X,-2X\n");
            return false ;
        }
        temp = (temp>> 1) *2 ;
        if(speed < 0){
            this->PlaySpeed = 0- temp ;
        }
        else {
            this->PlaySpeed =  temp ;
        }
        this->SkipPFrameIndex = this->CurPFrameNum / temp;
        LOGGER_DBG("set play speed ->%d\n",this->PlaySpeed);
        return true ;

    }
}
//return 1: play frame
//2: skip frame
//0: wait to play
//-1: error
int SpeedCtrl::CheckVideoPlayTimestamp(uint64_t timestamp,int flags){
    uint64_t  temp ;
    static int pFrameCounter = 0 ;
    static int frameCounter = 0 ;
    static bool skipIFrame =  false ;
    int ret = -1 ;
    static uint64_t lastTimeStamp = 0;
    bool sameFrameFlag = 0;

    if(lastTimeStamp ==  timestamp){
        //same frame ,not add counter
        sameFrameFlag  = true ;
    }
    else {
        sameFrameFlag = false ;
    }

    if(flags == VIDEO_FRAME_HEADER){
        LOGGER_DBG("get new stream header,and reset all timestamp\n");
        this->PVPlayTs->Start();
        //stream header ,always return true
        ret =  1;
    }
    else {
    //    printf("video frame ->%3X\n",flags);
        if(flags == VIDEO_FRAME_I){
            //check i/p frame number
            if(pFrameCounter != 0 ){
                this->CurPFrameNum =  pFrameCounter ;
                pFrameCounter = 0;
                if(this->PlaySpeed > 0 ){
                    this->SkipPFrameIndex = this->CurPFrameNum / this->PlaySpeed;
                }
                LOGGER_TRC("current p frame number ->%d %d %d\n",this->CurPFrameNum,this->SkipPFrameIndex,pFrameCounter);
            }
            if(this->PlaySpeed > 0 ){
                if(this->PlaySpeed > (this->CurPFrameNum+1)){
                    LOGGER_TRC("need drop frame I ->%8lld\n",timestamp);
                    this->SkipPFrameIndex= -1;
                    pFrameCounter = 0;
                    skipIFrame =  true ;
                    int rate = this->PlaySpeed /(this->CurPFrameNum+1) ;
                    if(frameCounter == 0 ){ //first frame ,just do nothing to play
                    }
                    else if(frameCounter <  rate ){
                        ret = 2 ;
                    }
                    else if(frameCounter >= rate){ //
                        frameCounter = 0 ;
                    }
                    else {
                        printf("wrong\n");
                    }

                }
                else {
                    skipIFrame =  false ;
                    LOGGER_TRC("no need drop i frame\n");
                    if(pFrameCounter >= this->SkipPFrameIndex){
                        LOGGER_TRC("skip frame from ->%d %d %d %2X\n",pFrameCounter,this->PlaySpeed,this->CurPFrameNum,flags);
                        ret = 2;
                    }

                }
            }//play speed check end
        }
        else if(flags == VIDEO_FRAME_P){
            if(sameFrameFlag == false ){
                pFrameCounter++;
            }
            if(skipIFrame){
                ret = 2;
            }
        }
        else {

        }
        if(sameFrameFlag == false){
            frameCounter++ ;
        }
        if(this->PlaySpeed > 1){
            //printf("speed play ->%d %4d %4d\n",this->PlaySpeed,pFrameCounter,this->SkipPFrameIndex);
            if(skipIFrame == false){
                if(pFrameCounter >= this->SkipPFrameIndex){
                    //LOGGER_DBG("skip frame from ->%d %d %d %2X :%8lld\n",pFrameCounter,this->PlaySpeed,this->CurPFrameNum,flags,timestamp);
                    ret =  2;
                }
                else {
                    //    LOGGER_DBG("play frame from ->%d %d %d %2X\n",pFrameCounter,this->PlaySpeed,this->CurPFrameNum,flags);
                    //need play
                    //ret = 1;
                }
            }

        }
    }

    if(sameFrameFlag == false){
        lastTimeStamp  = timestamp ;
    }
    if(ret == -1) {
        ret =  this->CheckSpeedPlayTimeStamp(timestamp);
    }
    return ret ;
}

int SpeedCtrl::CheckSpeedPlayTimeStamp(uint64_t timestamp){
    uint64_t refTime = this->PVPlayTs->GetTs(this->PlaySpeed);
    if(timestamp <= refTime) {
        if (refTime - timestamp > VIDEO_SYNC_THREADHOLD)
        {
            LOGGER_WARN("check video frame ->ts:%8lld  size:%8d speed:%d, ref ts:%8lld sys ts:%8lld queue size:%d\n",
                this->VideoFrame.timestamp, this->VideoFrame.size,
                this->PlaySpeed, refTime,
                get_timestamp(),
                this->PVideoFrFifo->GetQueueSize());
            this->PVPlayTs->UpdateTs(timestamp);
        }
        return 1;
    }
    else {
        return 0;
    }
}

bool SpeedCtrl::UpdateCurrentTimestamp(int64_t newTimestamp){
    this->PVPlayTs->UpdateTs(newTimestamp);
    this->UpdateVideoFrameFlag =  true;
    return true;
}
bool SpeedCtrl::EnableAutoUpdataTs(bool state){
    this->AutoUpdateTs.store(state);
}

bool SpeedCtrl::SetStepDisplay()
{
    this->StepDisplay = true;
    return true;
}