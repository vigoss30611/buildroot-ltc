#include <threadcore.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/prctl.h>
#if 0
static void LoopThreadFunction(void *arg){
    ThreadCore *p = (ThreadCore *) arg ;
    printf("begin thread ->%p\n",p);
    try    {
        int ret = 0;
        ret = p->WorkingThreadLoop(p);
        if (ret < 0 ){
            LOGGER_ERR("Working Task error %d\n", ret);
        }
        else if (ret == 0 ){
            LOGGER_WARN("task failed\n");
        }
    }
    catch (const char *err) {
        printf("Working Task exception error: %s\n", err);
}
#else
static void LoopThreadFunction(void *arg){
    ThreadCore *p = (ThreadCore *) arg ;
//    printf("begin thread ->%p\n",p);
    ThreadingState nextState = ThreadingState::Standby;
//    p->SetThreadName();
    p->UpdateThreadName();
    ThreadingState state = ThreadingState::Standby ;
    bool firstPauseFlag = true ;

//    p->SetWorkingState(ThreadingState::Pause) ;
    try    {
        while(1) {
            //Critical region start
            state = p->GetWorkingState();


            if(nextState == state ){
                //do nothin
            }
            else {
                //(nextState == ThreadingState::Keep)
                if(p->PendingStateFlag.load() ==  true){
                    p->SetWorkingStateNoblock(state);
                    p->PendingStateFlag.store(false);
                }
                else { //no extra setting
                    if(nextState != ThreadingState::Keep){
                        p->SetWorkingStateNoblock(nextState);
                        state = p->GetWorkingState();
                    }

                }
            }

            //Critical region end
        //    LOGGER_DBG("task state 2->%d\n",state);
            if(state == ThreadingState::Stop){
                break;
            }
            else if (state == ThreadingState::Pause ){
                if(firstPauseFlag == true){
                    LOGGER_INFO("##%s->run post pause hook start\n",p->Name);
                    p->ThreadPostPauseHook();
                    LOGGER_INFO("##%s->run post pause hook end\n",p->Name);
                }
                firstPauseFlag =  false;
                p->SetThreadStateReady(ThreadingState::Pause);
                usleep(50000);
                continue ;
            }
            else if (state == ThreadingState::Standby)     {
                usleep(50000);
                continue ;
            }

            firstPauseFlag =  true;
            nextState = p->WorkingThreadTask();
            usleep(1000);
            /*
            if(state == nextState ){
                continue ;
            }
            else {
                p->SetWorkingStateNoblock(nextState);
                if(nextState == ThreadingState::Pause){
                    LOGGER_DBG("%s ->channge thread stop state ->%d\n", p->Name, nextState);
                    //p->ThreadPostPauseHook();
                }
                else if(nextState == ThreadingState::Stop){
                    LOGGER_DBG("%s -> channge thread stop state ->%d\n", p->Name, nextState);
                    p->ThreadPostStopHook();
                }
            }
            */

        }

    }
    catch (const char *err) {
        printf("Working Task exception error: %s\n", err);
    }
    LOGGER_TRC("##post stop hook begin ->%s\n",p->Name);
    p->ThreadPostStopHook();
    LOGGER_TRC("##post stop hook end ->%s\n",p->Name);
}
#endif
int ThreadCore::SetWorkingStateNoblock(ThreadingState State){
    this->MutexVarLock.lock();
    this->WorkingThreadState =  State;
    this->MutexVarLock.unlock();
    return 0;
}

int ThreadCore::SetWorkingState(ThreadingState State){
    this->MutexInterfaceLock.lock();
    ThreadingState old = this->GetWorkingState();
    if(old == State && State != ThreadingState::Standby){
        LOGGER_DBG("%s ->thread state already is %d ,no need to change->%p\n",this->Name, State,this);
    }
    else  {
        this->PendingStateFlag.store(true);
        LOGGER_DBG(" %s->change thread state to ->%d\n",this->Name, State);
        switch( State ){
            case ThreadingState::Standby :
                this->SetWorkingStateNoblock(State);
                if(this->PWorkThread == NULL){
                    LOGGER_DBG("%s->set thread to standby\n",this->Name);
                    this->PWorkThread = new std::thread(LoopThreadFunction, this);
                    LOGGER_DBG("%s->standby thread ->%p\n",this->Name, this->PWorkThread);
                    if(this->PWorkThread == NULL){
                        LOGGER_ERR("new thread error,abort\n");
                        return -1;
                    }
                }
                else {
                    //        printf("the thread has started\n");
                }
                break;
            case ThreadingState::Start :
                LOGGER_DBG("%s---->run pre start hook\n",this->Name);
                if( this->ThreadPreStartHook() != true){
                    LOGGER_ERR("run thread pre start hook error & stop run the thread\n");
                }
                this->SetWorkingStateNoblock(State);
                if(this->PWorkThread == NULL){
                    this->PWorkThread = new std::thread(LoopThreadFunction, this);
                    if(this->PWorkThread == NULL){
                        LOGGER_ERR("new thread error,abort\n");
                        return -1;
                    }
                }
                else {
                    //printf("the thread has started\n");
                }
                break;
            case ThreadingState::Stop :
                if(this->PWorkThread != NULL) {
                    LOGGER_DBG("stop %sthread -> %p\n",this->Name, this->PWorkThread);
                    this->SetWorkingStateNoblock(State);
                    this->PWorkThread->join();
                    delete this->PWorkThread ;
                    LOGGER_DBG("stop %s  thread -> ok\n",this->Name);
                    this->PWorkThread =  NULL;
                }
                //LOGGER_DBG("%s---->run post stop hook\n",this->Name);
                //this->ThreadPostStopHook();
                //LOGGER_INFO("thread exit success \n");
                break;
            case ThreadingState::Pause :
                LOGGER_DBG("->%s<-set thread state to pause\n",this->Name);
                this->ResetThreadStateReady();
                this->SetWorkingStateNoblock(State);
                while(true) {
                    if(this->CheckThreadStateReady(ThreadingState::Pause) == true){
                        LOGGER_DBG("%s-> thread has pause ready\n",this->Name);
                        break;
                    }
                    usleep(40000);
                    LOGGER_DBG("wait thread into pause state->%s<- %d\n", this->Name, this->WorkingThreadState);
                }
                //LOGGER_DBG("%s---->run post pause hook\n",this->Name);
                //this->ThreadPostPauseHook();
                //LOGGER_DBG("thread pause - >%p\n",this->PWorkThread);
                break;
            default :
                LOGGER_ERR("not support:%d\n", State);
        }
    }
    this->MutexInterfaceLock.unlock();
    return 1;
}
ThreadingState  ThreadCore::GetWorkingState(void){
    this->MutexVarLock.lock();
    ThreadingState state = this->WorkingThreadState;
    this->MutexVarLock.unlock();
    return state;
}
void ThreadCore::ShowInfo(void){
    LOGGER_TRC("++++++++++\n");
}
bool ThreadCore::SetInFifo(QMediaFifo *frLoop){
    assert(frLoop != NULL);
    if(frLoop == NULL){
        return false ;
    }
    else {
        this->PFrFifoIn =  frLoop ;
        return true;
    }
}
bool ThreadCore::SetOutFifo(QMediaFifo *frLoop){
    assert(frLoop != NULL);
    LOGGER_TRC("set out fr loop ->%p\n",frLoop);
    if(frLoop == NULL){
        return false ;
    }
    else {
        LOGGER_TRC("set out fr loop ->%p\n",frLoop);
        this->PFrFifoOut =  frLoop ;
        LOGGER_TRC("set out fr loop ->%p\n",frLoop);
        return true;
    }
}
ThreadCore::ThreadCore(){
    this->PWorkThread =  NULL ;
    this->PFrFifoIn   =  NULL ;
    this->PFrFifoOut  =  NULL ;
    //this->WorkingThreadState = ThreadingState::Stop;
    this->WorkingThreadState = ThreadingState::Standby;
    memset(this->Name,0,32);

    this->PauseFinished =  false;

    this->CallBackFuncion = NULL;
    this->CallBackArg = NULL;
    this->PendingStateFlag = false;
}
ThreadCore::~ThreadCore(){
    LOGGER_DBG("thread core deinit ->%p %s\n",this,this->Name);
    if(this->PWorkThread != NULL) {
        LOGGER_TRC("stop thread -> %p\n",this->PWorkThread);
        if(this->GetWorkingState() != ThreadingState::Stop){
            this->SetWorkingStateNoblock(ThreadingState::Stop);
            this->PWorkThread->join();
        }
        delete this->PWorkThread ;
        this->PWorkThread =  NULL;
    }
}
void ThreadCore::SetName(char *name){
    int len = strlen(name);
    memset(this->Name,0,32);
    if(len > 31){
        len = 31;
    }
    strncpy(this->Name,name,len);
    LOGGER_DBG("thread name : %s ->%p\n",this->Name,this);
}
void ThreadCore::ResetThreadStateReady()
{
    this->MutexVarLock.lock();
    this->PauseFinished =  false;
    this->MutexVarLock.unlock();
}
void ThreadCore::SetThreadStateReady(ThreadingState State){
//    LOGGER_DBG("work loop set thread to pause->%s\n", this->Name);
    if(State == ThreadingState::Pause){
        this->MutexVarLock.lock();
        this->PauseFinished =  true;
        this->MutexVarLock.unlock();
    }
}
bool ThreadCore::CheckThreadStateReady(ThreadingState State){
    bool ret  = false ;
    if(State == ThreadingState::Pause){
        this->MutexVarLock.lock();
        ret = this->PauseFinished;
        this->MutexVarLock.unlock();
    }
    else {
        ret = true;
    }
    return ret;
}
void ThreadCore::SetErrorCallBack(ErrorCallback callback , void *arg){
    assert(callback != NULL );
    assert(arg != NULL );
    this->CallBackFuncion = callback ;
    this->CallBackArg = arg ;
}
void ThreadCore::RunErrorCallBack(void){
    if(this->CallBackFuncion == NULL || this->CallBackArg == NULL){
        LOGGER_ERR("call back vari not enought\n");
        return;
    }
    this->CallBackFuncion(this->CallBackArg, this);
}
void ThreadCore::UpdateThreadName(void){
    if(strlen(this->Name) != 0)
        prctl(PR_SET_NAME, (unsigned long)this->Name);
    else {
        prctl(PR_SET_NAME, (unsigned long)("undefined"));
    }
}

void ThreadCore::EnhancePriority(bool enhance){
    if (this->PWorkThread != NULL)
    {
        if(enhance)
        {
            struct sched_param sched;
            sched.sched_priority = MAX_THREAD_PRIORITY;
            pthread_setschedparam(this->PWorkThread->native_handle(), SCHED_RR, &sched);
            LOGGER_INFO("Enhance thread priority:%s\n", this->Name);
        }
        else
        {
            struct sched_param sched;
            sched.sched_priority = 0;
            pthread_setschedparam(this->PWorkThread->native_handle(), SCHED_OTHER, &sched);
            LOGGER_INFO("Reset thread priority:%s\n", this->Name);
        }
    }
}
