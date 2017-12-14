#include <thread>
#include <define.h>
#include <qmediafifo.h>
#include <stdio.h>
#include <mutex>
#include <atomic>
#ifndef __THREADCORE_H__
#define __THREADCORE_H__

#define MAX_THREAD_PRIORITY 90

enum class ThreadingState {
    Keep = 0 ,
    Standby,
    Start ,
//    Pending ,
//    Running ,
    Pause ,
    Stop
};
class ThreadCore {
    private :
//        virtual ThreadState  GetThreadState(void) = 0 ; //child must rewrite this function
        std::thread  *PWorkThread ;
        ThreadingState  WorkingThreadState ;
//        std::atomic_flag  InterfaceLock;
        //pthread_mutex_t   MutexInterfaceLock;
        std::mutex   MutexInterfaceLock;
//        ThreadingState WorkingThreadState ;
        std::mutex   MutexVarLock;
        bool PauseFinished ;

        ErrorCallback CallBackFuncion;
        void *CallBackArg;


    public :
        char Name[32];
        ThreadCore();
        //virtual ~ThreadCore()  {};
        ~ThreadCore();
        int SetWorkingState(ThreadingState State);
        int SetWorkingStateNoblock(ThreadingState State);
        ThreadingState  GetWorkingState(void);
        std::atomic_bool PendingStateFlag;

        void ShowInfo(void);


//        virtual int WorkingThreadLoop(void *arg)= 0 ; //child must rewrite this function
/*
 * return value will change thread state
 * 0  : task run continue
 * 0  :
 * -1 : error and set task to pause
 * -2 : error and set task to stop
 * -3 : error and set task to standy
 *
 */
        virtual ThreadingState WorkingThreadTask(void )= 0 ; //child must rewrite this function
        virtual bool Prepare(void)  = 0;

        virtual bool ThreadPreStartHook(void) = 0 ;
        virtual bool ThreadPostPauseHook(void) = 0 ;
        virtual bool ThreadPostStopHook(void) = 0 ;

        void SetThreadStateReady(ThreadingState State);
        bool CheckThreadStateReady(ThreadingState State);
        void ResetThreadStateReady();

        QMediaFifo *PFrFifoIn ;
        QMediaFifo *PFrFifoOut ;
        bool SetInFifo(QMediaFifo *);
        bool SetOutFifo(QMediaFifo *);
        int  GetFrFrameNum();
        void SetName(char *name);
        void SetErrorCallBack(ErrorCallback callback ,void *arg);
        void RunErrorCallBack(void);
        void UpdateThreadName(void); //do not call me outside directly
        void EnhancePriority(bool enhance);

};
#endif
