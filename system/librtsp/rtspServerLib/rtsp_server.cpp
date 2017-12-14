#include <pthread.h>
#include "rtsp_server.h"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "DynamicRTSPServer.hh"
#include "version.hh"
#include "types.h"

RTSP_SERVER_INFO g_rtsp_server_info;

int rtsp_server_init(unsigned short port,char *file_location,char *audio_chanID)
{
	int ret = 0;
	
	if(port ==0)
		g_rtsp_server_info.port = 554;
	else
		g_rtsp_server_info.port = port;

	if(file_location == NULL)
		strcpy(g_rtsp_server_info.file_location,".");
	else
		strcpy(g_rtsp_server_info.file_location,file_location);

	if(audio_chanID == NULL)
		strcpy(g_rtsp_server_info.audio_chanID,"default_mic");
	else
		strcpy(g_rtsp_server_info.audio_chanID,audio_chanID);

	
	  // Begin by setting up our usage environment:
	  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	  g_rtsp_server_info.scheduler = scheduler;
	  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
	  g_rtsp_server_info.env = env;
	
	  // Create the RTSP server.  Try first with the default port number (554),
	  // and then with the alternative port number (8554):
	  RTSPServer* rtspServer;
	  portNumBits rtspServerPortNum = g_rtsp_server_info.port;
	  rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, NULL);

	  if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		return -1;
	  }

	  g_rtsp_server_info.rtspServer = rtspServer;
	
	  *env << "LIVE555 Media Server\n";
	  *env << "\tversion " << MEDIA_SERVER_VERSION_STRING
		   << " (LIVE555 Streaming Media library version "
		   << LIVEMEDIA_LIBRARY_VERSION_STRING << ").\n";
	
	  char* urlPrefix = rtspServer->rtspURLPrefix();
	  *env << "Play streams from this server using the URL\n\t"
		   << urlPrefix << "<filename>\nwhere <filename> is a file present in the current directory or video channel ID.\n";
	  *env << "Each file's type is inferred from its name and each live stream uses its video channel ID\n";
	  *env << "\t\"*-stream(such as enc1080p-stream)\" => H264 Live Video and G711A Live audio\n";
	  *env << "\t\".mkv\" => a Matroska audio+video file\n";
	  *env << "See http://www.live555.com/mediaServer/ for additional documentation.\n";
	
	  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
	  // Try first with the default HTTP port (80), and then with the alternative HTTP
	  // port numbers (8000 and 8080).
	
	  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
		*env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)\n";
	  } else {
		*env << "(RTSP-over-HTTP tunneling is not available.)\n";
	  }

	return ret;
}

void * thread_func(void *arg)  
{  
	g_rtsp_server_info.on_off = 0;
	g_rtsp_server_info.env->taskScheduler().doEventLoop(&g_rtsp_server_info.on_off);
}  


int rtsp_server_start()
{
	int ret = 0;
	ret=pthread_create(&(g_rtsp_server_info.thread_id),NULL,thread_func,NULL);  
    if(ret!=0)  
    {  
        *(g_rtsp_server_info.env) << "Create pthread error!\n";  
        ret = -1;  
    } 

	return ret;
}

int rtsp_server_set_file_location(const char *file_loc)
{	
	int ret = 0;
	if(file_loc == NULL)
	{
		*(g_rtsp_server_info.env) << "file_loc is NULL\n";
		ret = -1;
	}
	
	strcpy(g_rtsp_server_info.file_location,file_loc);

	return ret;
}

int rtsp_server_stop()
{
	int ret = 0;
	g_rtsp_server_info.on_off = 1;
	TaskScheduler* scheduler = g_rtsp_server_info.scheduler;
	UsageEnvironment* env = g_rtsp_server_info.env;
	RTSPServer* rtspServer = g_rtsp_server_info.rtspServer;
	if((ret = pthread_join(g_rtsp_server_info.thread_id,NULL)) != 0)
	{
		*env << "pthread_join error:" << ret << "\n";
		ret = -1;
	}

	Medium::close(rtspServer);

	if(scheduler != NULL)
	{
		delete scheduler;
		g_rtsp_server_info.scheduler = NULL;
	}

	if(!env->reclaim())
	{
		*env << "destruct UsageEnvironment error \n";
		ret = -1;
	}
	
	return ret;
}


