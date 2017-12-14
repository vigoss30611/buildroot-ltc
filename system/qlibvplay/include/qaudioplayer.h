#include <common.h>
#include <define.h>
#include <qsdk/vplay.h>
#include <threadcore.h>
#include <qsdk/audiobox.h>
#include <stdlib.h>
#ifndef __QPLAY_AUDIO_H__
#define __QPLAY_AUDIO_H__
class QAudioPlayer : public ThreadCore {
    private :
        int AudioHander;
        int AudioChannelHander;
        AudioStreamInfo *AudioInfo;
        int PlaySpeed;
        struct fr_buf_info FrFrame;
        void Release(void);
    public :
        QAudioPlayer(AudioStreamInfo *info);
        ~QAudioPlayer();
        bool ThreadPreStartHook(void);
        bool ThreadPostPauseHook(void);
        bool ThreadPostStopHook(void);
        ThreadingState WorkingThreadTask(void ); //child must rewrite this function
        bool Prepare(void);
        bool UnPrepare(void);
        bool UpdateAudioInfo(void);
        bool SetPlaySpeed(int speed);
        QMediaFifo  *PFrFifo;
        VplayTimeStamp *PVPlayTs;
};
#endif
