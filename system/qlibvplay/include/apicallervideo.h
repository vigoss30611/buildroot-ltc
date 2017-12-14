
#include <threadcore.h>
#include <fr/libfr.h>
#include <common.h>
#include <atomic>
#ifndef __API_CALLER_VIDEO_H__
#define  __API_CALLER_VIDEO_H__


class ApiCallerVideo : public ThreadCore {
    private :
        VideoStreamInfo  *PVideoInfo ;
        char  DefChannel[STRING_PARA_MAX_LEN];
        struct fr_buf_info FrFrame ;
        uint64_t FrameNum ;
        bool PrepareReady ;
        std::atomic_bool NeedTriggerFlag;
        void SaveDebugFrame(struct fr_buf_info *);
    //    bool NewDebugStart ;
        bool FifoAutoFlush;
        void UpdateVideoInfo(uint64_t timestamp);
        uint32_t  FrameCounter;
        uint64_t LastTs;
        bool ThreadPauseFlag;
        void DebugVideoFrame(struct fr_buf_info *fr, bool newState);
        std::atomic_bool AutoCheckFps;
        std::atomic_bool OldestFrame;

    public :
        ThreadingState WorkingThreadTask(void ) ;
        bool Prepare(void) ;
        ApiCallerVideo(char *channel,VideoStreamInfo *info);
        ~ApiCallerVideo();
        QMediaFifo *PFrFifo;
        bool ThreadPreStartHook(void);
        bool ThreadPostPauseHook(void);
        bool ThreadPostStopHook(void);
        void SetFifoAutoFlush(bool enable);
        void SetAutoCheckFps(bool check);
        void SetCaheMode(bool mode);
};
#endif
