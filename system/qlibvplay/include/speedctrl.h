#include <define.h>
#include <common.h>
#include <qsdk/vplay.h>
#include <threadcore.h>
#include <qsdk/audiobox.h>
#include <atomic>
#include <qmediafifo.h>

class SpeedCtrl : public ThreadCore {
    public :
        SpeedCtrl(VPlayerInfo *info);
        ~SpeedCtrl();
        QMediaFifo  *PVideoFrFifo;

        bool ThreadPreStartHook(void);
        bool ThreadPostPauseHook(void);
        bool ThreadPostStopHook(void);
        ThreadingState WorkingThreadTask(void ); //child must rewrite this function
        bool Prepare(void);
        bool Release(void);
        bool Update(VPlayerInfo *info ,audio_info_t *audio);
        bool SetPlaySpeed(int speed);
        bool UpdateCurrentTimestamp(int64_t timestamp);
        VplayTimeStamp *PVPlayTs;
        Statistics *stat;
        bool EnableAutoUpdataTs(bool state);
        bool SetStepDisplay();

    private :
        VPlayerInfo  PlayerInfo;
        long Timestamp ;
        int  SpeedRate ;
        uint64_t CurTimestamp ;
        uint64_t FirstTimestamp ;

        int PlaySpeed ;

        struct fr_buf_info VideoFrame ;

        bool UpdateVideoFrameFlag ;
        bool StepDisplay;

        int  CheckVideoPlayTimestamp(uint64_t timestamp,int flags);
        int  CheckSpeedPlayTimeStamp(uint64_t timestamp);

        int  CurPFrameNum;
        int  SkipPFrameIndex;
      atomic_bool AutoUpdateTs;

};
