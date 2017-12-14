#include <mutex>
#include <qsdk/vplay.h>
#include <qsdk/codecs.h>
#include <define.h>
#include <atomic>

#ifndef __COMMON_H__
#define __COMMON_H__

#define VIDEO_SYNC_THREADHOLD 2000
#define TIMER_BEGIN(a) uint64_t a##timer = get_timestamp();
#define TIMER_END(a) {  uint64_t temp = get_timestamp();  \
                            LOGGER_INFO("[%llu][PERFORMANCE][%s] %s:%d cost %llums\n", temp, #a, __FUNCTION__, __LINE__, temp - a##timer);  \
                            a##timer = temp;  \
                     }
#define COMPILE_ASSERT(x, y) {\
                                 int __dummy1[(int)(sizeof(x) - sizeof(y))];\
                                 int __dummy2[(int)(sizeof(y) - sizeof(x))];\
                             }

typedef struct
{
    int write_adts;
    int objecttype;
    int sample_rate_index;
    int channel_conf;
}adts_context_t;

typedef enum {
    STAT_TYPE_PLAYBACK,
    STAT_TYPE_RECORDE,
} Stat_Type;

typedef enum {
    PACKET_TYPE_VIDEO,
    PACKET_TYPE_VIDEOEXTRA,
    PACKET_TYPE_AUDIO,
    PACKET_TYPE_UNDETERMINED,
} Packet_Type;

typedef enum {
    TIMESTAMP_STATUS_READY = 0,
    TIMESTAMP_STATUS_START,
    TIMESTAMP_STATUS_PAUSE,
    TIMESTAMP_STATUS_STOP,
    TIMESTAMP_STATUS_MAX,
} EN_TIMESTAMP_STATUS;

class VideoStreamInfo{
    private:
        //video info
        video_type_t Type;
        int Width;
        int Height;
        int Fps;
        int BitRate;
        int FrameSize;
        int PFrame;
        int GopLength;
        char Header[128];
        int HeaderSize;
        //control info
        std::mutex  *MutexLock ;
        bool Update;
    public:
        VideoStreamInfo();
        //VideoStreamInfo(const VideoStreamInfo &info);
        ~VideoStreamInfo();
        video_type_t GetType();
        void SetType(video_type_t);
        int GetWidth();
        void SetWidth(int);
        int GetHeight();
        void SetHeight(int);
        int GetFps();
        void SetFps(int);
        int GetBitRate();
        void SetBitRate(int);
        int GetFrameSize();
        void SetFrameSize(int);
        int GetPFrame();
        void SetPFrame(int);
        int GetGopLength();
        void SetGopLength(int);
        char *GetHeader(char *, int *);
        char *SetHeader(char *, int);
        int GetHeaderSize();

        void SetUpdate(bool flag);
        bool GetUpdate();

        void PrintInfo(void);

};
#define AUDIO_EXTRA_DATA_MAX_SIZE  32
class AudioStreamInfo{
    private:
        codec_fmt_t AudioInFmt;
        codec_fmt_t AudioOutFmt;
        char AudioInExtraData[AUDIO_EXTRA_DATA_MAX_SIZE];
        char AudioOutExtraData[AUDIO_EXTRA_DATA_MAX_SIZE];
        int  AudioInExtraDataSize;
        int  AudioOutExtraDataSize;
        //control info
        std::mutex  *MutexLock;
        char AudioChannel[VPLAY_CHANNEL_MAX];
        bool Update;
    public:
        AudioStreamInfo();
        ~AudioStreamInfo();

        codec_fmt_t  GetInType();
        void  SetInType(codec_fmt_t *in);

        codec_fmt_t  GetOutType();
        void  SetOutType(codec_fmt_t *in);

        int  CalAudioBitRate(codec_fmt_t *fmt);

        int GetInExtraData(char *buf, int bufSize);
        int SetInExtraData(char *buf, int bufSize);
        int GetOutExtraData(char *buf, int bufSize);
        int SetOutExtraData(char *buf, int bufSize);

        void PrintInfo(void);
        void SetUpdate(bool flag);
        bool GetUpdate();

        void SetChannel(char *channel);
        void GetChannel(char *channel, int bufSize);

};
class VplayTimeStamp{
    public:
        VplayTimeStamp();
        ~VplayTimeStamp();
        void Start(void);
        void Stop(void);
        void UpdateTs(uint64_t ts);
        uint64_t GetTs(int rate);
        void Seek(uint64_t ts);
        bool IsStopped();
        void EnableAutoUpdate();
        void SetPlaySpeed(int rate);
        void Pause(bool bIsPaused);
    private:
        uint64_t AdjustTimeByRate(uint64_t time, int rate);

        uint64_t FirstTs;
        uint64_t CurSysTs;
        uint64_t PlayTs;
        uint64_t AutoStartTs;
        uint64_t LastGetTs;
        int  Rate;
        bool Stopped;
        bool AutoUpdate;
        EN_TIMESTAMP_STATUS enStatus;
        std::mutex  AccessLock;
};
class Statistics{
    public:
        Statistics();
        void ShowInfo(void);
        void SetStatType(Stat_Type StatType);
        bool UpdatePacketInfo(Packet_Type type, int pktSize, unsigned long long pktPTS, int streamIndex);
        bool UpdatePacketPTS(Packet_Type type, uint64_t pktPTS);
        bool UpdateFifoInfo(Packet_Type type, int Qsize, int Dropframes);
        bool UpdateRefTime(uint64_t RefPts);
    private:
        Stat_Type StatType;
        int VideoFrames;
        int AudioFrames;
        uint64_t VideoPTS;
        uint64_t AudioPTS;
        uint64_t RefPTS;

        int VQueueSize;
        int AQueueSize;
        int VDropFrames;
        int ADropFrames;

        uint64_t LastTS;
        uint64_t LastVPTS;
        uint64_t FirstVPTS;
        uint64_t TotalVSize;
        uint64_t PeriodVSize;
        int LastVFrames;

};

#endif
