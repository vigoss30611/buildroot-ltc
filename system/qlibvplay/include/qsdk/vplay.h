#ifndef __VPLAY_H__
#define __VPLAY_H__
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* errors */
#define EVPLAY_FAILED   1
#define EVPLAY_SUCCESSFUL  0

/* player */
typedef struct {
    /* VPlayer structure */
    void *inst ;
} VPlayer;

#define VPLAY_PATH_MAX 2048
#define EVENT_BUF_MAX 370
#define VPLAY_CHANNEL_MAX 64
#define STRING_PARA_MAX_LEN   VPLAY_CHANNEL_MAX
#define VPLAY_MAX_TRACK_NUMBER 6
typedef enum {
    VPLAY_MEDIA_TYPE_AUDIO=1,//current support this
    VPLAY_MEDIA_TYPE_VIDEO,
    VPLAY_MEDIA_TYPE_SUBTITLE,
    VPLAY_MEDIA_TYPE_DATA
}VPlayMediaType;
typedef enum{
    VPLAY_CODEC_VIDEO_H264,
    VPLAY_CODEC_VIDEO_H265,
    VPLAY_CODEC_VIDEO_MJPEG,
    VPLAY_CODEC_AUDIO_PCM_ALAW,
    VPLAY_CODEC_AUDIO_PCM_ULAW,
    VPLAY_CODEC_AUDIO_PCM_S8E,
    VPLAY_CODEC_AUDIO_PCM_S16E,
    VPLAY_CODEC_AUDIO_PCM_S32E,
    VPLAY_CODEC_AUDIO_AAC,
    VPLAY_CODEC_AUDIO_G726,
    VPLAY_CODEC_EXTRA_DATA
}vplay_codec_type_t;

typedef struct{
    int stream_index;
    vplay_codec_type_t type;
    int width;
    int height;
    int fps;
    int sample_rate;
    int channels;
    int bit_width;
    int frame_num;
}vplay_track_info_t;

typedef struct {
    char file[VPLAY_PATH_MAX];
} vplay_file_info_t;

typedef struct{
    vplay_file_info_t file;
    int64_t time_length;
    int64_t time_pos;
    int track_number;
    int video_index;
    int audio_index;
    int extra_index;
    vplay_track_info_t track[VPLAY_MAX_TRACK_NUMBER];
}vplay_media_info_t;

#define VPlayerFileInfo  vplay_media_info_t
typedef enum{
    VPLAY_PLAYER_PLAY = 0x1,
    VPLAY_PLAYER_STOP = 0x2,//stop current file and set play pos to 0
    VPLAY_PLAYER_PAUSE,//pause play
    VPLAY_PLAYER_CONTINUE,//continue play
    VPLAY_PLAYER_SEEK,
    VPLAY_PLAYER_QUEUE_FILE,
    VPLAY_PLAYER_QUEUE_CLEAR,
    VPLAY_PLAYER_REMOVE_FILE,
    VPLAY_PLAYER_QUERY_MEDIA_INFO,
    VPLAY_PLAYER_SET_VIDEO_FILTER,
    VPLAY_PLAYER_SET_AUDIO_FILTER,
    VPLAY_PLAYER_STEP_DISPLAY,
    VPLAY_PLAYER_MUTE_AUDIO,
//    VPLAY_VPLAYER_PLAY_NEXT ,
//    VPLAY_VPLAYER_PLAY_LAST ,

    VPLAY_RECORDER_START = 0x1000,
    VPLAY_RECORDER_STOP,
    VPLAY_RECORDER_MUTE,
    VPLAY_RECORDER_UNMUTE,
    VPLAY_RECORDER_SET_UDTA,
    VPLAY_RECORDER_FAST_MOTION,
    VPLAY_RECORDER_SLOW_MOTION,
    VPLAY_RECORDER_FAST_MOTION_EX, //do fast motion by drop frames, rate should be n times of I frame interval
    VPLAY_RECORDER_NEW_SEGMENT,
    VPLAY_RECORDER_GET_AUDIO_CHANNEL_INFO,
    VPLAY_RECORDER_SET_UUID_DATA
}vplay_ctrl_action_t;
typedef struct{
    char video_channel[VPLAY_CHANNEL_MAX];
    char audio_channel[VPLAY_CHANNEL_MAX];
    char media_file[VPLAY_PATH_MAX];
}VPlayerInfo;

#ifdef __GNUC__
#define DLL_EXPORT __attribute__((visibility("default")))   //just for gcc4+
#else
#define DLL_EXPORT  __declspec(dllexport)
#endif

#define vplay_player_inst_t  VPlayer
#define vplay_player_info_t  VPlayerInfo

DLL_EXPORT vplay_player_inst_t * vplay_new_player(vplay_player_info_t *info);
DLL_EXPORT int vplay_delete_player(vplay_player_inst_t *player);
DLL_EXPORT int64_t vplay_control_player(vplay_player_inst_t *player ,vplay_ctrl_action_t action ,void *para);//when speed set ,rate :only support 2x value
DLL_EXPORT int vplay_query_media_info(char *file_name, vplay_media_info_t *info);


#if 1
//no suggest use it
DLL_EXPORT int vplay_queue_file(vplay_player_inst_t *player, const char *file);
DLL_EXPORT int  vplay_update_player(vplay_player_inst_t *info);
DLL_EXPORT int vplay_remove_file(vplay_player_inst_t *player, const char *file);
DLL_EXPORT int vplay_clean_queue(vplay_player_inst_t *player);
DLL_EXPORT const char ** vplay_list_queue(vplay_player_inst_t *player);
DLL_EXPORT void vplay_show_media_info(vplay_media_info_t *info);
#endif

/* recorder */
typedef struct {
    void *inst;
} VRecorder;

/*
 * @suffix:  %Y 2016, %y 16, %m month 2 digits
 *           %d day, %H, %M, %S
 *           %s seconds since 1970
 *           %c counter
 */



typedef enum  {
    AUDIO_CODEC_TYPE_NONE = -1 ,
    AUDIO_CODEC_TYPE_PCM = 0 ,
    AUDIO_CODEC_TYPE_G711U,
    AUDIO_CODEC_TYPE_G711A,
    AUDIO_CODEC_TYPE_AAC,
    AUDIO_CODEC_TYPE_ADPCM_G726,
    AUDIO_CODEC_TYPE_MP3,
    AUDIO_CODEC_TYPE_SPEEX,
    AUDIO_CODEC_TYPE_MAX
}audio_codec_type_t;

typedef enum {
    AUDIO_SAMPLE_FMT_NONE = -1,
    AUDIO_SAMPLE_FMT_S8 ,
    AUDIO_SAMPLE_FMT_S16 ,
    AUDIO_SAMPLE_FMT_S24 ,
    AUDIO_SAMPLE_FMT_S32 ,
    AUDIO_SAMPLE_FMT_FLTP ,//float
    AUDIO_SAMPLE_FMT_DBLP ,//double
    AUDIO_SAMPLE_FMT_MAX

}sample_format_t ;
typedef struct {
    audio_codec_type_t  type ;
    int sample_rate ;
    int channels ;
    sample_format_t sample_fmt ;
//    int bit_rate ;
    int effect ;
}audio_info_t ;

typedef struct audio_channel_info_t {
    int s32Channels;
    int s32BitWidth;
    int s32SamplingRate;
    int s32SampleSize;
} ST_AUDIO_CHANNEL_INFO;


typedef struct {
    char audio_channel[VPLAY_CHANNEL_MAX]; //empty will not open it
    char video_channel[VPLAY_CHANNEL_MAX]; //empty will not open it
    char videoextra_channel[VPLAY_CHANNEL_MAX];
    int enable_gps;
    int keep_temp; // 0: set temp file name fixed and delete temp media file when start 1: save
    audio_info_t  audio_format ;
    int time_segment;//cut off file time
    int time_backward;//
    char time_format[STRING_PARA_MAX_LEN] ; //%Y:2016,%y 16 %m %d %H %M %S  , %s  seconds since 1970
    char suffix[STRING_PARA_MAX_LEN]; // %ts :start time , %te end_time ,%c :counter,%l :media duration
    char av_format[STRING_PARA_MAX_LEN];
    char dir_prefix[STRING_PARA_MAX_LEN];
    int fixed_fps;
    ST_AUDIO_CHANNEL_INFO stAudioChannelInfo;
    int s32KeepFramesTime;
} VRecorderInfo;

typedef struct {
    char *buf;//memory pointer or file name buf pointer
    int size;//size==0 ,buf is a file path ,size > 0 ,buf is a memory buffer and size is buffer data lenght;
}vplay_udta_info_t;

#define  vplay_recorder_inst_t  VRecorder
#define  vplay_media_type_enum_t VPlayMediaType
#define  vplay_recorder_info_t  VRecorderInfo


DLL_EXPORT vplay_recorder_inst_t *vplay_new_recorder(vplay_recorder_info_t *info);
DLL_EXPORT int vplay_delete_recorder(vplay_recorder_inst_t *rec);
DLL_EXPORT int vplay_control_recorder(vplay_recorder_inst_t *rec, vplay_ctrl_action_t type, void *para);

#if 1
DLL_EXPORT int vplay_start_recorder(vplay_recorder_inst_t *rec);
DLL_EXPORT int vplay_stop_recorder(vplay_recorder_inst_t *rec);
DLL_EXPORT int vplay_mute_recorder(vplay_recorder_inst_t *rec, vplay_media_type_enum_t type, int enable);
DLL_EXPORT int vplay_set_recorder_udta(vplay_recorder_inst_t *rec, char *buf, int size);
DLL_EXPORT void vplay_set_cache_mode(vplay_recorder_inst_t *rec, int mode);
#endif



typedef enum {
    VPLAY_EVENT_NONE = 0 ,
    VPLAY_EVENT_NEWFILE_START ,
    VPLAY_EVENT_NEWFILE_FINISH ,
    VPLAY_EVENT_VIDEO_ERROR,
    VPLAY_EVENT_AUDIO_ERROR,
    VPLAY_EVENT_EXTRA_ERROR ,
    VPLAY_EVENT_DISKIO_ERROR ,
    VPLAY_EVENT_INTERNAL_ERROR ,
    VPLAY_EVENT_PLAY_FINISH ,
    VPLAY_EVENT_PLAY_ERROR ,
    VPLAY_EVENT_STEP_DISPLAY_FINISH,
    VPLAY_EVENT_MAX
}vplay_event_enum_t ;
typedef struct {
    vplay_event_enum_t type ;
    int  index ; //event index
    char buf[EVENT_BUF_MAX];
}vplay_event_info_t ;


#define RECORDER_TEMP_FILE  ".rec_temp"  //will auto add path and type

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif
