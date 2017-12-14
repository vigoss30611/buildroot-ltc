#ifndef _RTSP_SERVER_H_  
#define _RTSP_SERVER_H_ 

#ifdef __cplusplus  
extern "C" {
#endif 
	/* unsigned short port: rtsp server port,0 for default port 554
	 *  char *file_location: playback file location where your playback files(mkv or mp4 files) are stored
	 					 NULL for current location,
	 *  char *audio_chanID:live audio channel ID,NULL for default channel ID(default_mic)
	 */
	int rtsp_server_init(unsigned short port,char *file_location,char *audio_chanID);
	int rtsp_server_start();
	/* set playback file location,valid at next time playback*/
	int rtsp_server_set_file_location(const char *file_loc);
	int rtsp_server_stop();
	
#ifdef __cplusplus  
}  
#endif  
  
#endif /* _RTSP_SERVER_H_  */  
