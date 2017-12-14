#include <apicallerextra.h>
#include <define.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
ThreadingState ApiCallerExtra::WorkingThreadTask(void ) {
    struct fr_buf_info  fr ;
    int ret = 0 ;
    int size = 0;
    char buf[124];
    fr.virt_addr =  buf ;
    fr.timestamp =  get_timestamp();
    sprintf((char *)fr.virt_addr,"gps (+_+)?  info ->%6lld",this->FrameNum);
    LOGGER_TRC("-->%s\n",(char *)fr.virt_addr);
    fr.size = (int )strlen((const char *)fr.virt_addr);
    this->PFrFifo->PushItem(&fr)  ;
    sleep(2);

    this->FrameNum++;
    if(this->FrameNum / 10){
        LOGGER_TRC("extra queue size ->  %d %d\n",this->PFrFifo->GetQueueSize() ,size);
    }
    return  ThreadingState::Keep;
}
bool ApiCallerExtra::Prepare(void) {
    return true ;
}

int ApiCallerExtra::GetFrameRate(void) {
    return 2 ;
}
int ApiCallerExtra::GetFrameSize(void) {
    return 64 ;
}
ApiCallerExtra::ApiCallerExtra():ThreadCore(){
    this->FrameNum =  0;
    this->FifoAutoFlush  =  false;
    this->SetName((char *)"rec_gps");
    printf("thread name 2 : %s ->%p\n",this->Name,this);
}
ApiCallerExtra::~ApiCallerExtra(){
}
bool ApiCallerExtra::ThreadPreStartHook(void){
    LOGGER_DBG("start extra thread hook\n");
//    fr_INITBUF(&this->FrFrame,NULL);

}
bool ApiCallerExtra::ThreadPostPauseHook(void){
    LOGGER_DBG("api video-> %s exec\n",__FUNCTION__);
    if(this->FifoAutoFlush){
        LOGGER_DBG(" clear fifo\n");
        this->PFrFifo->Flush();
    }
}
bool ApiCallerExtra::ThreadPostStopHook(void){

}
void ApiCallerExtra::SetFifoAutoFlush(bool enable){
    this->FifoAutoFlush =  enable ;
}
