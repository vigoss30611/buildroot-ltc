#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>
#include <threadcore.h>
#include <atomic>
#include "common.h"

#ifndef __API_CALLER_AUDIO_H__
#define  __API_CALLER_AUDIO_H__
class ApiCallerAudio : public ThreadCore {
    private :
        int AudioHandler ;
        char  DefChannel[STRING_PARA_MAX_LEN];
        audio_info_t AudioOutInfo;
        void  *AudioCodecs;
        int FrameRate;
        int FrameSize;
        struct fr_buf_info FrFrame ;
        uint64_t FrameNum;
        bool PrepareReady;
        bool FifoAutoFlush;
        std::atomic_bool MuteEnable;
        char *MuteBuffer;

        uint64_t LastValidEncodeTimestamp;
        bool  LastValidEncodeFlag;
        uint64_t LastRawTimestamp;
        uint64_t LastTs;
        struct audio_format stAudioChannelFmt;

        bool CheckAudioFmt(audio_fmt_t *pstInfo);
        int OpenAudioChannel(void);

    public :
        ThreadingState WorkingThreadTask(void ) ;
        //boot GetAudioInfo(audio_info_t *in ,audio_info_t *out);
        bool Prepare(void);
        bool UnPrepare(void);
        ApiCallerAudio(char *channel, audio_info_t *info, audio_channel_info_t *pstChannelInfo);
        ~ApiCallerAudio();
        int GetFrameRate(void);
        int GetFrameSize(void);
        QMediaFifo *PFrFifo ;
        bool ThreadPreStartHook(void);
        bool ThreadPostPauseHook(void);
        bool ThreadPostStopHook(void);
        void SetFifoAutoFlush(bool enable);
        void SetMute(bool enable);
        bool GetAudioChannelInfo(audio_channel_info_t *pstInfo);
};


#endif
