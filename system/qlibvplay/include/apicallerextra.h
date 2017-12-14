
#include <threadcore.h>
#include <fr/libfr.h>
#ifndef __API_CALLER_EXTRA_H__
#define  __API_CALLER_EXTRA_H__

class ApiCallerExtra : public ThreadCore {
    private :
        uint64_t FrameNum ;
        bool PrepareReady ;
        bool FifoAutoFlush ;
    public :
        //int  WorkingThreadLoop(void *arg)  ;//child must rewrite this function
//        ApiCallerAudio();

        ThreadingState WorkingThreadTask(void ) ;
        bool Prepare(void) ;
        QMediaFifo *PFrFifo ;
        ApiCallerExtra();
        ~ApiCallerExtra();
        int GetFrameRate(void);
        int GetFrameSize(void);
        bool ThreadPreStartHook(void);
        bool ThreadPostPauseHook(void);
        bool ThreadPostStopHook(void);
        void SetFifoAutoFlush(bool enable);
};

#endif
