#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "media_play.h"
#include <qsdk/vplay.h>
//#include "common_debug.h"

#define MEDIA_PLAY_DEBUG 1
#define DEBUG_INFO  printf
#define DEBUG_ERR  printf

static pthread_t callback_thread_id;
static int pthread_status = 0;

media_play *media_play_g = NULL;
static VPlayer *player = NULL;
static VPlayerFileInfo vplayFileInfo;
extern vplay_event_info_t vplayState;


int set_file_path(char *filename);

int begin_video();
int end_video();
int is_video_playing();

int resume_video(); 
int pause_video(); 

int prepare_video(); 

int get_video_duration(); 
int get_video_position(); 

int video_speedup(float speed); 

int display_photo();

void init_player_info(VPlayerInfo *info){
	memset(info,0,sizeof(VPlayerInfo));
	strcat(info->video_channel,"dec0-stream");
	strcat(info->audio_channel,"default");
	strcat(info->fs_channel,"fs-out");
}
void show_player_info(VPlayerInfo *info){
	printf("++show playerInfo\n");
	printf("dec channel ->%s<-\n",info->video_channel);
	printf("audio channel ->%s<-\n",info->audio_channel);
	printf("fs channel ->%s<-\n",info->fs_channel);
}



int set_file_path(char *filename)
{
    DEBUG_INFO("enter func %s \n", __func__);

    strcpy(media_play_g->media_filename, filename);

    DEBUG_INFO("leave %s filename is %s \n", __func__, media_play_g->media_filename);
    return 0;
}

void *callback_thread(void *args)
{
    DEBUG_INFO("enter func %s \n", __func__);
    int state_changed = 0;

    while(1) {
		if(NULL == media_play_g->video_callback) {
			DEBUG_INFO("media_play_g->video_callback is NULL \n");
			break;
		}

		if(!state_changed) {
			switch (vplayState.type) {
				case VPLAY_EVENT_PLAY_FINISH:
					DEBUG_INFO("PLAY_STATE_END \n");
					state_changed = 1;
					media_play_g->video_callback(MSG_PLAY_STATE_END);
					break;
				case VPLAY_EVENT_PLAY_ERROR:
					DEBUG_INFO("PLAY_STATE_ERROR \n");
					state_changed = 1;
					media_play_g->video_callback(MSG_PLAY_STATE_ERROR);
					break;
				default:
					break;
			}
		}

		pthread_testcancel();
		usleep(100*1000);
    }
    DEBUG_INFO("leave %s \n", __func__);
}

int begin_video()
{
    DEBUG_INFO("enter func %s \n", __func__);
    int ret = 0;
    int nRet=0;
	pthread_attr_t attr;

	if(player == NULL) {
		DEBUG_ERR("player is NULL\n");
		return -1;
	}

	if(vplay_queue_file(player, media_play_g->media_filename) < 0) {
		DEBUG_ERR("add file %s error\n", media_play_g->media_filename);
		return -1;
	}
	if(vplay_control_player(player,VPLAY_PLAYER_PLAY , 1) < 0) {
		DEBUG_ERR("vplay_control_player VPLAY_PLAYER_PLAY error\n");
		return -1;
	};

	vplayState.type = VPLAY_EVENT_NONE;

    if (!pthread_status) {
	pthread_status = 1;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&callback_thread_id, &attr, callback_thread, NULL);
	pthread_attr_destroy(&attr);
	if (0 != ret) {
	    DEBUG_ERR("pthread_create error \n");
	    return ERROR_CREATE_THREAD;
	}

	usleep(1000);
    }

    DEBUG_INFO("leave %s \n", __func__);
    return 0;
}

int end_video()
{
    DEBUG_INFO("enter func %s \n", __func__);
    int ret = 0;
    int nRet=0;

	vplay_control_player(player,VPLAY_PLAYER_STOP ,1);

    if (pthread_status) {
		pthread_status = 0;
		ret = pthread_cancel(callback_thread_id);
		if (0 != ret) {
		    DEBUG_ERR("pthread_cancel error \n");
		    return ERROR_CANCEL_THREAD;
		}
    }

    DEBUG_INFO("leave %s \n", __func__);
    return 0;
}

int is_video_playing()
{
    DEBUG_INFO("enter func %s \n", __func__);
    DEBUG_INFO("leave %s \n", __func__);
    return 0;
}

int resume_video()
{
    DEBUG_INFO("enter func %s \n", __func__);
	vplay_control_player(player,VPLAY_PLAYER_CONTINUE ,1);
    DEBUG_INFO("leave %s \n", __func__);
    return 0;
} 

int pause_video()
{
    DEBUG_INFO("enter func %s \n", __func__);
	vplay_control_player(player,VPLAY_PLAYER_PAUSE ,1);
    DEBUG_INFO("leave %s \n", __func__);
    return 0;
} 

int get_video_duration()
{
	memset(&vplayFileInfo, 0, sizeof(VPlayerFileInfo));
	DEBUG_INFO("duration is %8lld\n", vplayFileInfo.time_length);
	//vplay_player_info(player, &vplayFileInfo);
	DEBUG_INFO("duration is %8lld\n", vplayFileInfo.time_length);
    return (int)(vplayFileInfo.time_length/1000000);
} 

int get_video_position()
{
	memset(&vplayFileInfo, 0, sizeof(VPlayerFileInfo));
	//vplay_player_info(player, &vplayFileInfo);
	DEBUG_INFO("pos is %8lld\n", vplayFileInfo.time_pos);

    return (int)(vplayFileInfo.time_pos/1000000);
} 

int prepare_video()
{
    DEBUG_INFO("enter func %s \n", __func__);

    DEBUG_INFO("leave %s \n", __func__);
    return 0;
} 

int video_speedup(float speed)
{
    DEBUG_INFO("enter func %s \n", __func__);
    int ret = 0;
	vplay_control_player(player, VPLAY_PLAYER_PLAY , speed);
	//vplay_control_player(player, VPLAY_PLAYER_SEEK_START ,value);
    DEBUG_INFO("leave %s \n", __func__);
    return 0;
} 

int display_photo()
{
    DEBUG_INFO("enter func %s \n", __func__);
	int ret = 0;
	ret = jpeg_dec(media_play_g->media_filename);
    DEBUG_INFO("leave %s \n", __func__);
	return ret;
}

/*
int play_audio(char *filename)
{
    DEBUG_INFO("enter func %s \n", __func__);
   
    char buff[256];

    memset(buff, 0, sizeof(buff));
    strcat(buff, "aplay ");
    strcat(buff, filename);

    my_system(buff);
    
    int nRet=0;
    Inft_Action_Attr stActionAttr;

    memset(&stActionAttr, 0, sizeof(Inft_Action_Attr));
    stActionAttr.mode = INFT_ACTION_MODE_AUDIOPLAY;
    stActionAttr.enable = 1;
    strcpy(stActionAttr.args.startTime, filename);
//    nRet = Inft_action_setAttr(&stActionAttr);

    DEBUG_INFO("leave %s \n", __func__);

    return 0;
}
*/

media_play *media_play_create()
{
    DEBUG_INFO("enter func %s \n", __func__);

	VPlayerInfo playerInfo ;
	init_player_info(&playerInfo);
	show_player_info(&playerInfo);

	player =  vplay_new_player(&playerInfo);
	if(player == NULL){
		printf("init vplay player error \n");
		return  0;
	}

    media_play_g = (media_play*)malloc(sizeof(media_play));
    memset(media_play_g, 0, sizeof(media_play));

    media_play_g->set_file_path = set_file_path;

    media_play_g->begin_video = begin_video;
    media_play_g->end_video = end_video;
    media_play_g->is_video_playing = is_video_playing;

    media_play_g->resume_video = resume_video;
    media_play_g->pause_video = pause_video;
    media_play_g->prepare_video = prepare_video;

    media_play_g->get_video_duration = get_video_duration;
    media_play_g->get_video_position = get_video_position;

    media_play_g->video_speedup = video_speedup;

    media_play_g->display_photo = display_photo;

    media_play_g->video_callback = NULL;

    DEBUG_INFO("leave %s \n", __func__);
    return media_play_g;
}

void media_play_destroy(media_play *media_play_p)
{
    int ret = 0;
    DEBUG_INFO("enter func %s \n", __func__);

    if (pthread_status) {
	pthread_status = 0;
	ret = pthread_cancel(callback_thread_id);
	if (0 != ret) {
	    DEBUG_ERR("pthread_cancel error \n");
	}
    }

    free(media_play_g);

    media_play_g = NULL;
    DEBUG_INFO("leave %s \n", __func__);
}

void register_video_callback(media_play *media_play_p, video_callback_p callback)
{
    DEBUG_INFO("enter func %s \n", __func__);
    media_play_p->video_callback = callback;
    DEBUG_INFO("leave %s \n", __func__);
}

