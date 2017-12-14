#include <apicalleraudio.h>
#include <apicallervideo.h>
#include <apicallerextra.h>
#include <mediamuxer.h>
#include <qmediafifo.h>
#include <qsdk/vplay.h>
#include <common.h>
#include <mutex>
#ifndef   __Q_RECORDER_H__
#define   __Q_RECORDER_H__

class QRecorder  {
    public :
        MediaMuxer     *Muxer;
        ApiCallerVideo **CallerVideo;
        ApiCallerAudio *CallerAudio;
        ApiCallerExtra *CallerExtra;

        bool Prepare(void);
        bool UnPrepare(void);
        bool SetWorkingState(ThreadingState state);
        ThreadingState GetWorkingState(void);
        QRecorder(VRecorderInfo *info);
        ~QRecorder();
        bool Alert(char *alert);
        void SetMute(VPlayMediaType type, bool enable);
        VRecorderInfo RecInfo ;
        void HandlerErrorCallback(void *);
        int SetMediaUdta(char *buf, int size);
        void SetCaheMode(bool mode);
        bool StartNewSegment(VRecorderInfo *pstInfo);
        bool GetAudioChannelInfo(ST_AUDIO_CHANNEL_INFO *pstInfo);

    private :
        QMediaFifo     **FifoLoopVideo;
        QMediaFifo     *FifoLoopAudio;
        QMediaFifo     *FifoLoopExtra;
        ThreadingState WorkingThreadState;
        VideoStreamInfo **VideoInfo;
        std::mutex     FunctionLock;
        int VideoIndex;
        int VideoCount;


        bool PrepareVideo(void) ;
        bool PrepareAudio(void) ;
        bool PrepareExtra(void) ;
        bool CreateVideoResource(char * video_channel,VideoStreamInfo *video_info);
};
#endif
