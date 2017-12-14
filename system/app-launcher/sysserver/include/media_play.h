#ifndef MEDIA_PLAY_H
#define MEDIA_PLAY_H

//#include "common_inft.h"

#define ERROR_NULL		-1
#define ERROR_GET_FAIL		-2
#define ERROR_SET_FAIL		-3
#define ERROR_CREATE_THREAD 	-4
#define ERROR_CANCEL_THREAD 	-5

#define MSG_PLAY_STATE_END 	1
#define MSG_PLAY_STATE_ERROR 	2

//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
#endif

typedef int (*video_callback_p)(int flag);

typedef struct media_play {
    char media_filename[256];

    int (*set_file_path)(char *filename);

    int (*begin_video)();
    int (*end_video)();
    int (*is_video_playing)();

    int (*resume_video)(); //继续播放
    int (*pause_video)(); //暂停播放
    int (*prepare_video)(); //显示缩略图

    int (*get_video_duration)(); //获取视频时长
    int (*get_video_position)(); //获取视频当前播放位置 

    int (*video_speedup)(float speed); //快速或者慢速播放，speed可以是0.25,0.5,2,4

    int (*video_callback)(int flag); //主要是给上层上报video是否结束以及各种异常。

    int (*display_photo)();

}media_play;

media_play *media_play_create();
void media_play_destroy(media_play *media_play_p);

void register_video_callback(media_play *media_play_p, video_callback_p callback);

int play_audio(char *filename);

#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif
