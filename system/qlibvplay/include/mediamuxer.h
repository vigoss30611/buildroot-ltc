#include <threadcore.h>
#include <fr/libfr.h>
#include <qsdk/vplay.h>
#include <atomic>
#include <common.h>
//#include <bs.h>
extern "C"
{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavformat/url.h>
}

typedef struct {
    VPlayMediaType media_type;
    int64_t last_dts;
    uint64_t last_timestamp;
    uint64_t frame_num;
}stream_channel_info_t;
#ifndef __MEDIA_MUXER_H__
#define __MEDIA_MUXER_H__

#define MAX_BUFFER_COUNT 30
#define MAX_VIDEO_COUNT  2
#define MAX_UUID_SIZE    1024

class MediaMuxer : public ThreadCore {
    private :
        //should add static var to counte inst number ,and delelte useless temp file
        //
        int StreamChannelNum;
        stream_channel_info_t *PStreamChannel;
        bool PrepareReady ;
        std::atomic_bool  WorkTaskRunning;
        std::atomic_bool MediaFileOpenState;

        std::mutex  FunctionLock;


        QMediaFifo  *PFrFifoAudio ;
        QMediaFifo  **PFrFifoVideo;
        QMediaFifo  *PFrFifoExtra ;

        bool ValidMediaFile;

        VRecorderInfo  RecInfo;
        VRecorderInfo  NewSegInfo;
        int AudioIndex;
        int ExtraStreamIndex;


        bool MediaEnableVideo ;
        bool MediaEnableAudio ;
        bool MediaEnableExtra ;

        AVOutputFormat *MediaFileFormat;
        AVFormatContext *MediaFileFormatContext;
        AVCodec *AudioCodec ;
        AVCodec *VideoCodec;
        AVCodec *ExtraCodec ;

        AVCodecID AudioCodecID ;
        AVCodecID VideoCodecID;
        AVCodecID ExtraCodecID ;

        AVStream *VideoStream;
        AVStream *AudioStream;
        AVStream *ExtraStream;

        struct fr_buf_info FrVideo;
        struct fr_buf_info FrAudio;
        struct fr_buf_info FrExtra;
        struct fr_buf_info FrKeepVideo[MAX_VIDEO_COUNT];

        Statistics stat;

        VideoStreamInfo **PVideoInfo;
        int Fps;
        int VideoCount;


        char CurrentMediatFileName[VPLAY_PATH_MAX];
        int  CurrentMediaFileIndex ;
        int TimeSuffixLen ;
        time_t LastTimer ;
        std::atomic_bool NewFileFlags ;

        uint64_t FirstTimeStamp;
        uint64_t LastTimeStamp;
        uint64_t LastVideoTimeStamp;
        uint64_t LastAudioTimeStamp;
        uint64_t FirstStartTime;
        int64_t  StartDelayTime;
        time_t StartTime;
        time_t NextStartTime;

        char *MediaUdtaBuffer;
        int MediaUdtaBufferSize;
        std::mutex MediaUdtaMutexLock;
        std::atomic_bool MediaUdtaBufferReady;
        std::atomic_int  SlowFastMotionRate;
        std::atomic_bool StartNewSeg;
        std::atomic_bool bRestoreBuf;
        int VideoTimeScaleRate;
        int VideoSkipFrameNum;
        char pu8UUIDData[MAX_UUID_SIZE];

        bool     NeedUpdateFrameVideo;
        bool     NeedUpdateFrameAudio;
        bool     NeedUpdateFrameExtra;

        bool     EnhanceThreadPriority;
        bool     FastMotionByPlayer;
        bool     IsKeepFrame[MAX_VIDEO_COUNT];
        bool     CheckIFrame[MAX_VIDEO_COUNT];

        void  LinkVideoStreamInfo(VideoStreamInfo **);



        int ChooseStream(struct fr_buf_info *frv ,struct fr_buf_info *fra ,struct fr_buf_info *fre);

        bool AddVideoFrFrame(struct fr_buf_info *frame, int stream_index);
        bool AddAudioFrFrame(struct fr_buf_info * );
        bool AddExtraFrFrame(struct fr_buf_info * );
        bool CheckMediaSegment(struct fr_buf_info *fr,int type) ;

        bool StartNewMediaFile(bool firstFlags);
        bool StopMediaFile(void) ;
        int  TryGetLocalIndex(char *);
        bool GetTimeString(char *suffix, char *buf, uint32_t len, bool tempFlag);
        bool GetTimeString(char *suffix, char *buf, uint32_t len,time_t *ref, bool utc);
        bool UpdateMediaFile(void);
        bool GetFinalFileName(char *buf,int bufSize, bool tempFlag);
        bool GetTempFileName(bool keepTemp);
        bool InternalReleaseMuxerFile(void);
        ThreadingState PopFirstIFrame(QMediaFifo  *vfifo, struct fr_buf_info *fr_video, int *ret_value);

        bool MediaFileDirectIO;
        bool FragementedMediaSupport;

        AVBitStreamFilterContext* PBsfFilter;


        //uint32_t VideoFrameCounter ;
        //uint32_t AudioFrameCounter ;
        void InitStreamChannel(void);
        int64_t  FrameDtsAutoReCal(int streamIndex, uint64_t timestamp, int64_t dts);
        bool UpdateDirectIOFlags(void);
        bool SyncStorage(void);
        void UpdateMediaFileContextUdta(void);
        uint32_t  GetMotionPlayTime(uint32_t timestamp);
        bool UpdateSpsFrameRate(uint8_t *nalu, int *headerSize, int frameRate, int frameBase);
        bool UpdateSegmentInfo();
        /*
        * @brief keep I frame for next fragment.
        * @param index: which track get I frame.
        * @return >0: Success.
        * @return 0: Failure.
        */
        int KeepIFrame(int index);

    public :
        ThreadingState WorkingThreadTask(void) ;//child must rewrite this function ;
        bool Prepare(void) ;

        MediaMuxer(VRecorderInfo *rec ,VideoStreamInfo **info, int video_count);
        ~MediaMuxer();

        bool SetFrFifo(QMediaFifo **v ,QMediaFifo *a ,QMediaFifo *e);
    //    bool ReleaseMuxerFile(void);

        void ShowInfo(int32_t flags);

        void SetThreadName(void) ;
        bool Alert(char *alert);
        bool CheckFileSystem(void) ;
        bool ThreadPreStartHook(void);
        bool ThreadPostPauseHook(void);
        bool ThreadPostStopHook(void);

        void SetMute(VPlayMediaType type, bool enable);
        bool GetMute(VPlayMediaType type);
        int  SetMediaUdta(char *buf, int size);
        bool SetSlowMotion(int rate);
        bool SetFastMotion(int rate);
        int GetMotionRate(void);
        bool StartNewSegment(VRecorderInfo * info);
        /*
        Do fast motion by recorder droping frames.
        */
        bool SetFastMotionEx(int rate);

        /*
        * @brief set UUID data.
        * @param pu8UUID: the uuid data.
        * @return true: Success.
        * @return false: Failure.
        */
        bool SetUUIDData(char* pu8UUID);

};
#endif
