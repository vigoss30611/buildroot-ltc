#ifndef _TYPES_H_  
#define _TYPES_H_ 

#define FILE_PATH_MAXLEN (256 - 2)

typedef struct _RTSP_SERVER_INFO
{
	char on_off;
	char file_location[FILE_PATH_MAXLEN];
	unsigned short port;
	char audio_chanID[64];
	TaskScheduler* scheduler;
	UsageEnvironment* env;
	RTSPServer* rtspServer;
	pthread_t thread_id;
}RTSP_SERVER_INFO;

#endif /* _TYPES_H_  */  
