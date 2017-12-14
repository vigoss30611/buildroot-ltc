
#ifndef __FACTANDVERINFO_H__
#define __FACTANDVERINFO_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <stdio.h>
#include <unistd.h>
#include <errno.h> 
#include <fcntl.h>
#include <string.h>
//#include <sysconf.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include "sysconf.h"

#define RTSP_DEFAULT_SVR_PORT			554

/*
extern int GetPTZCommand2(unsigned char *lpcsPTZData, int nPTZDataSize, 
				 unsigned long dwPTZCmd, unsigned long dwValue, int nPTZSpeed, unsigned char byAddress, 
				 char *lpOut, int nSize, unsigned long *lpWait);
*/

int get_partnumber(char * partnumber);
int get_progver(char * prog0ver);
int get_video_decoder(void);
int get_audio_codec(void);
int get_product_modle(char *product);

int get_network_cfg(DMS_NET_NETWORK_CFG 	*net_conf);
int set_network_cfg(DMS_NET_NETWORK_CFG 	*net_conf);

int get_channel_param(int nChannel,VIDEO_CONF *video);
int ptz_control(int ncmd,int nValue,int nChannel);
int AlarmOut_control(int Sensor_id ,int SensorState, int alarmouttime);
int get_channel_num();

int Service_connect(const char*pSerIP, int inPort, int time_out, int *sock);
int Service_tcp_send(int sock, void *pBuf, int nLen);
int Service_tcp_recv(int hSock, char *pBuffer, unsigned long in_size, unsigned long *out_size);

int get_rtsp_port();

//added by panhn
int get_color_param(int channel,DMS_NET_CHANNEL_COLOR *color_config);
int set_color_param(int channel,DMS_NET_CHANNEL_COLOR *color_config);
int get_color_param_range(DMS_NET_COLOR_SUPPORT *color_range);
int get_support_fmt(DMS_NET_SUPPORT_STREAM_FMT *support_fmt);
int get_enhanced_color(int channel,DMS_NET_CHANNEL_ENHANCED_COLOR *enhanced_color);
//end add

int get_user(ONVIF_USER_INFO *pstUserInfo);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  //__FACTANDVERINFO_H__


