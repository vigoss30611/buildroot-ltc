#include <qsdk/vplay.h>
#include <define.h>
#include <common.h>
#include <list>
#include <threadcore.h>
#include <qsdk/demux.h>
#include <qmediafifo.h>
#include <qsdk/videobox.h>
#include <speedctrl.h>
#include <qsdk/codecs.h>
#include <qaudioplayer.h>
#include <atomic>

class QPlayer : public  ThreadCore{
    public :
        QMediaFifo  *PVideoFrFifo ;
        QMediaFifo  *PAudioFrFifo ;

        QPlayer(VPlayerInfo *info);
        ~QPlayer();
        bool Play(int speed);
        int64_t Seek(int whence, int64_t value);
        bool Stop(void);
        bool Pause(void);
        bool Continue(int speed);
        bool AddMediaFile(const char *fileName);

        bool ThreadPreStartHook(void);
        bool ThreadPostPauseHook(void);
        bool ThreadPostStopHook(void);
        ThreadingState WorkingThreadTask(void ); //child must rewrite this function
        bool Prepare(void);
        bool UnPrepare(void);
        int QueryMediaFileInfo(char *file,vplay_media_info_t *info);
        int SetVideoFilter(int index);
        int SetAudioFilter(int index);
        int ClearPlayList();
        int RemoveFileFromPlayList(char *file);
        bool StepDisplay();
        bool AudioMute(int Mute);
    private :
        std::list <vplay_file_info_t> *PFileList;
        std::mutex                     PlayListLock;
        Statistics                     stat;

        bool               CurFileReadyFlag;
        vplay_file_info_t  CurFile;
        vplay_media_info_t CurMediaInfo;

        VPlayerInfo        PlayerInfo;
        std::atomic_int    PlaySpeed;
        SpeedCtrl          *PSpeedCtrl;
        uint64_t           LastTs;
        bool               IsEnhance;
        bool               IsStepDisplay;

        QAudioPlayer       *PAudioPlayer;
        void               *PAudioCodecs;
        codec_info_t       AudioCodecsInfo;
        struct demux_frame_t AudioFrameInfo;
        bool               IsAudioMute;

        enum v_media_type  VideoType;
        audio_codec_type_t AudioType;
        AudioStreamInfo    *PVideoInfo;
        AudioStreamInfo    *PAudioInfo;

        char               *VideoBuffer;
        int                VideoBufferMaxLen;
        char               *AudioBuffer;
        int                AudioBufferMaxLen;
        int                VideoIndex;
        int                AudioIndex;
        int                ExtraIndex;
        int                PlayerMaxCacheMs;
        int                PlayerMinCacheMs;

        char               *AudioHeader;
        int                AudioHeaderSize;
        char               *AudioStream;
        int                AudioStreamSize;
        adts_context_t     AudioAdtsContext;
        stream_info_t      *PDemuxVideoInfo;
        stream_info_t      *PDemuxAudioInfo;
        stream_info_t      *PDemuxExtraInfo;
        VplayTimeStamp     VPlayTs;
        struct demux_t     *DemuxInstance;

        bool OpenNextMediaDemuxer(bool first);
        bool CloseCurMediaDemuxer(void);
        bool CloseCurMediaDemuxer(struct demux_t *demux);
        bool ReleaseAudioCodec();
        void AddStreamEndDummy();
        int  PhotoWriteStream(const char *chn, void *data, int len);
        int  PhotoTriggerDecode(const char *chn);
        int  PhotoDecodeFinish(const char *chn, bool* IsFinish);
        bool FlushAVBuffer(int waitTime);
        bool PushVideoHeader(void);
        bool CheckAudioCodecsInstance(struct demux_frame_t  *frame);
        void Release(void);
};
