#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <sys/types.h>
#include "factandverinfo.h"
#include "sys_fun_interface.h" /* RM#2291: used sys_fun_gethostbyname, henry.li    2017/03/01*/

//added by panhn
int get_color_param_range(QMAPI_NET_COLOR_SUPPORT *color_range)
{
	if(!color_range)
		return -1;

	if(QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_COLOR_SUPPORT, 0,color_range,sizeof(QMAPI_NET_COLOR_SUPPORT)))
		return -2;
	return 0;
}

int get_color_param(int channel,QMAPI_NET_CHANNEL_COLOR *color_config)
{
	if(!color_config)
		return -1;

	if(QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_COLORCFG, channel,color_config,sizeof(QMAPI_NET_CHANNEL_COLOR)))
		return -2;
	return 0;
}

int set_color_param(int channel,QMAPI_NET_CHANNEL_COLOR *color_config)
{
	if(!color_config)
		return -1;

	if(QMapi_sys_ioctrl(0,QMAPI_SYSCFG_SET_COLORCFG, channel,color_config,sizeof(QMAPI_NET_CHANNEL_COLOR)))
		return -2;
	
	return 0;
}

int get_support_fmt(QMAPI_NET_SUPPORT_STREAM_FMT *support_fmt)
{
	if(!support_fmt)
		return -1;

	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_SUPPORT_STREAM_FMT, 0, support_fmt, sizeof(QMAPI_NET_SUPPORT_STREAM_FMT)))
		return -2;
	return 0;
}

int get_enhanced_color(int channel,QMAPI_NET_CHANNEL_ENHANCED_COLOR *enhanced_color)
{
	if(!enhanced_color)
		return -1;

	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ENHANCED_COLOR, channel, enhanced_color, sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR)))
		return -2;
	return 0;
}

//end add

int get_partnumber(char * partnumber)
{
	strcpy(partnumber,"1.1");
	return 0;

}

int get_progver(char * prog0ver)
{
	QMAPI_NET_DEVICE_INFO dms_devinfo;
	dms_devinfo.dwSize = sizeof(QMAPI_NET_DEVICE_INFO);
	QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_DEVICECFG,0,&dms_devinfo,sizeof(QMAPI_NET_DEVICE_INFO));
	unsigned int dwVersion = dms_devinfo.dwSoftwareVersion;
	sprintf(prog0ver,"%d.%d.%d.%d", (dwVersion >> 24) & 0xff, 
							      (dwVersion >> 16) & 0xff,
							      (dwVersion >> 8)  & 0xff,
							      dwVersion & 0xff
								  );

	return 0;
}

int get_video_decoder(void)
{
	return 0;
}


int get_audio_codec(void)
{
	return 0;
}

int get_product_modle(char *product)
{
	QMAPI_NET_DEVICE_INFO dms_devinfo;
	dms_devinfo.dwSize = sizeof(QMAPI_NET_DEVICE_INFO);
	QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_DEVICECFG,0,&dms_devinfo,sizeof(QMAPI_NET_DEVICE_INFO));

	strncpy(product,dms_devinfo.csDeviceName,QMAPI_NAME_LEN);
	return(0);
}


int get_rtsp_port()
{
	int     nRtspPort = RTSP_DEFAULT_SVR_PORT;
	int 	nRes = 0;
	QMAPI_NET_RTSP_CFG     stRtstCfg;
	nRes = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_RTSPCFG,0, &stRtstCfg, sizeof(QMAPI_NET_RTSP_CFG));
	if(nRes != 0)
	{
		nRtspPort = RTSP_DEFAULT_SVR_PORT;
	}
	else
	{
		nRtspPort = stRtstCfg.dwPort;
	}
	return nRtspPort;
}


int get_network_cfg(QMAPI_NET_NETWORK_CFG 	*net_conf)
{
//	memcpy(net_conf,&g_jb_globel_param.Jb_Server_Network,sizeof(JB_SERVER_NETWORK));
	QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_NETCFG,0,net_conf,sizeof(QMAPI_NET_NETWORK_CFG));
	return 0;
}


int set_network_cfg(QMAPI_NET_NETWORK_CFG 	*net_conf)
{
//	memcpy(&g_jb_globel_param.Jb_Server_Network,net_conf,sizeof(JB_SERVER_NETWORK));
	QMapi_sys_ioctrl(0,QMAPI_SYSCFG_SET_NETCFG,0,net_conf,sizeof(QMAPI_NET_NETWORK_CFG));
	return 0;
}


int get_channel_param(int nChannel,VIDEO_CONF *video)
{
	if(nChannel < 0 || nChannel > QMAPI_MAX_CHANNUM || video == NULL)
		return -1;
//	int i = nChannel;
	QMAPI_NET_CHANNEL_PIC_INFO  ChannelPic_Param;
	QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_PICCFG,nChannel,&ChannelPic_Param,sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
//	memcpy(ChannelPic_Param,&g_jb_globel_param.Jb_ChannelPic_Param,sizeof(QMAPI_NET_CHANNEL_PIC_INFO)*QMAPI_MAX_CHANNUM);
//	printf("get_channel_param channel:%d ret:%d wWidth:%d wHeight:%d\n",nChannel,ret,ChannelPic_Param.stRecordPara.wWidth,
//	ChannelPic_Param.stRecordPara.wHeight);
	video->catype = (unsigned char)1;
	video->width = ChannelPic_Param.stRecordPara.wWidth;
	video->height = ChannelPic_Param.stRecordPara.wHeight;
	video->piclevel = (unsigned char)ChannelPic_Param.stRecordPara.dwImageQuality;
	video->cbr= (unsigned char)ChannelPic_Param.stRecordPara.dwRateType;
	
	video->bitrate = ChannelPic_Param.stRecordPara.dwBitRate/1000;
	video->fps = ChannelPic_Param.stRecordPara.dwFrameRate;
	video->gop = ChannelPic_Param.stRecordPara.dwMaxKeyInterval; //I????
	video->entype = ChannelPic_Param.stRecordPara.dwCompressionType; //tt__VideoEncoding__H264
	video->AudioEnable = ChannelPic_Param.stRecordPara.wEncodeAudio;

	video += 1;

	video->catype = (unsigned char)1;
	video->width = ChannelPic_Param.stNetPara.wWidth;
	video->height = ChannelPic_Param.stNetPara.wHeight;
	video->piclevel = (unsigned char)ChannelPic_Param.stNetPara.dwImageQuality;
	video->cbr= (unsigned char)ChannelPic_Param.stNetPara.dwRateType;
	
	video->bitrate = ChannelPic_Param.stNetPara.dwBitRate/1000;
	video->fps = ChannelPic_Param.stNetPara.dwFrameRate;
	video->gop = ChannelPic_Param.stNetPara.dwMaxKeyInterval; //I????
	video->entype = ChannelPic_Param.stNetPara.dwCompressionType; //tt__VideoEncoding__H264
	video->AudioEnable = ChannelPic_Param.stNetPara.wEncodeAudio;

/*
	video += 1;

	video->catype = (unsigned char)1;
	video->width = ChannelPic_Param.stPhonePara.wWidth;
	video->height = ChannelPic_Param.stPhonePara.wHeight;
	video->piclevel = (unsigned char)ChannelPic_Param.stPhonePara.dwImageQuality;
	video->cbr= (unsigned char)ChannelPic_Param.stPhonePara.dwRateType;
	
	video->bitrate = ChannelPic_Param.stPhonePara.dwBitRate/1000;
	video->fps = ChannelPic_Param.stPhonePara.dwFrameRate;
	video->gop = ChannelPic_Param.stPhonePara.dwMaxKeyInterval; //I????
	video->entype = ChannelPic_Param.stPhonePara.dwCompressionType; //tt__VideoEncoding__H264
*/

	return 0;
}


int ptz_control(int ncmd,int nValue,int nChannel)
{

	QMAPI_NET_PTZ_CONTROL dms_ptz_ctrl;
	dms_ptz_ctrl.dwSize = sizeof(QMAPI_NET_PTZ_CONTROL);
	dms_ptz_ctrl.nChannel = nChannel;
	dms_ptz_ctrl.dwCommand = ncmd;
	dms_ptz_ctrl.dwParam = nValue;

	QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL,nChannel,&dms_ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));

	return 0;
}


int AlarmOut_control(int Sensor_id ,int SensorState, int alarmouttime)
{
	QMAPI_NET_ALARMOUT_CONTROL dms_alarmout;
	dms_alarmout.dwSize = sizeof(QMAPI_NET_ALARMOUT_CONTROL);
	dms_alarmout.nChannel = Sensor_id;
	dms_alarmout.wAction = SensorState;
	dms_alarmout.wDuration = alarmouttime;
	QMapi_sys_ioctrl(0,QMAPI_NET_STA_ALARMOUT_CONTROL,0,&dms_alarmout,sizeof(QMAPI_NET_ALARMOUT_CONTROL));
#if 0
	int type_id;
	if(SensorState == 1)
	{
		JB_Video_IO_Alarm_Out(Sensor_id,alarmouttime);
	}
	else
	{
		type_id = 10 + Sensor_id;
		JB_Video_IO_Alarm_Out(type_id,alarmouttime);
	}
	#endif
	return 0;
}

int get_channel_num()
{
	QMAPI_NET_DEVICE_INFO dms_devinfo;
	dms_devinfo.dwSize = sizeof(QMAPI_NET_DEVICE_INFO);
	if(QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_DEVICECFG,0,&dms_devinfo,sizeof(QMAPI_NET_DEVICE_INFO)) >= 0)
		return dms_devinfo.byVideoInNum;
	else
		return 1;
}

int Service_connect(const char*pSerIP, int inPort, int time_out, int *sock)
{
	struct sockaddr_in 	addr;
	int			nRet = 0;
	unsigned long		ulServer;
	int			hSocket = 0;
	int			nOpt = 0;	
	int			error;

	unsigned long 		non_blocking = 1;
	unsigned long 		blocking = 0;
	struct hostent 		*hostPtr = NULL;

	struct in_addr in;
	char	tmp_ip[32];

	//printf("connect to server %s:%d\n", pSerIP, inPort);
	if(NULL == pSerIP)
	{
		//err();
		return -1;
	}

	if((inPort < 0) || (inPort > 65535))
	{
		//err();
		return -1;
	}

	if (time_out <= 0 || time_out > 65535)
	{
		time_out = 30;
	}

    /* RM#2291: used sys_fun_gethostbyname, henry.li    2017/03/01*/
	//hostPtr = gethostbyname(pSerIP);
	hostPtr = sys_fun_gethostbyname(pSerIP);
	if (NULL == hostPtr)
	{
		printf("gethostbyname error \n");
		return -1;
	}
	else
		memcpy(&ulServer, hostPtr->h_addr_list[0], sizeof(unsigned long));

	in.s_addr = ulServer;
	strcpy(tmp_ip, inet_ntoa(in));
	hSocket = socket(AF_INET,SOCK_STREAM,0);
	if( hSocket <= 0 )
	{
		return -1;
	}

	nRet = setsockopt(hSocket,IPPROTO_TCP,TCP_NODELAY,(char *)&nOpt,sizeof(nOpt));	

	nOpt = TRUE;
	nRet = setsockopt(hSocket,SOL_SOCKET,SO_REUSEADDR,(char *)&nOpt,sizeof(nOpt));
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)inPort);
	addr.sin_addr.s_addr = ulServer;

	ioctl(hSocket,FIONBIO,&non_blocking);
	nRet = connect(hSocket, (struct sockaddr*)&addr, sizeof(addr));
	if (nRet == -1 && errno == EINPROGRESS)
	{
		struct timeval tv; 
		fd_set writefds;

		tv.tv_sec = time_out;//SOCK_CONNECT_OVER
		tv.tv_usec = 0; 
		FD_ZERO(&writefds); 
		FD_SET(hSocket, &writefds); 
		if(select(hSocket+1,NULL,&writefds,NULL,&tv) != 0)
		{ 
			if(FD_ISSET(hSocket,&writefds))
			{
				int len=sizeof(error); 
				if(getsockopt(hSocket, SOL_SOCKET, SO_ERROR,  (void *)&error, (socklen_t *)&len) < 0)
				{
					//err();
					goto error_ret; 
				}

				if(error != 0) 
				{
					//err();
					goto error_ret; 
				}
			}
			else
			{
				//err();
				goto error_ret; /* timeout or error happend  */
			}
		}
		else 
		{
			//err();
			goto error_ret;
		}

		ioctl(hSocket, FIONBIO, &blocking);
		nRet = 0;
	}

	if( nRet != 0 )
	{
		//err();
		goto error_ret;
	}


	*sock = hSocket;
	//printf("connect the remote server ok sock = %d\n", *sock);

	return 0;

error_ret:
		shutdown(hSocket, 2);
		close(hSocket);
		//err();
		return -1;
}


int Service_tcp_send(int sock, void *pBuf, int nLen)
{
	int ret = 0;
	int sendsize = 0;

	while(sendsize < nLen)
	{
		ret = send(sock, pBuf+sendsize, nLen - sendsize, 0);
		if (ret == 0 && errno == EINTR)
		{
			//err();
			continue;
		}
		if(ret < 1)
		{
			//err();
			printf("%s  %d, send error!err:%d(%s)\n",__FUNCTION__, __LINE__, errno, strerror(errno));
			return ret;
		}
		sendsize = sendsize + ret;
	}
	
	return sendsize;
}


int Service_tcp_recv(int hSock, char *pBuffer, unsigned long in_size, unsigned long *out_size)
{
	int		ret = 0;
	fd_set           fset;
	unsigned long	dwRecved = 0;
	struct timeval 	to;
	int		nSize = in_size;

	FD_ZERO(&fset);
	FD_SET(hSock, &fset);
	to.tv_sec = 3;
	to.tv_usec = 0;	

   		ret = select(hSock+1, &fset, NULL, NULL, &to);  //&to
	if (ret == 0)
	{
		//err();
		return -1;
	}
	if (ret < 0 && errno == EINTR)
	{
		//err();
		printf("%s  %d, select error!err:%d(%s)\n",__FUNCTION__, __LINE__, errno, strerror(errno));
		return -2;
	}
	if (ret < 0)
	{
		//err();
		printf("%s  %d, select error!err:%d(%s)\n",__FUNCTION__, __LINE__, errno, strerror(errno));
		return -3;
	}

	if(!FD_ISSET(hSock, &fset))
	{
		//err();
		printf("select FD_ISSET error!\n");
		return -4;
	}
	ret = recv(hSock, pBuffer + dwRecved, nSize - dwRecved, 0);
	/*
	if (ret < 0 && (errno == EINTR || errno == EAGAIN))
	{
		err();
		return -1;
	}
	*/
	
	if (ret < 0)
	{
		//err();
		printf("%s  %d, recv error!err:%d(%s)\n",__FUNCTION__, __LINE__, errno, strerror(errno));
		return -5;
	}
	else if(ret == 0)
	{
		printf("%s recv connect down!\n",__FUNCTION__);
		return -6;
	}
	dwRecved += ret;
	
	*out_size = dwRecved;
	return 0;
}


static unsigned char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Onvifbase64encode(const unsigned char *input, int input_length, unsigned char *output, int output_length)
{
	int	i = 0, j = 0;
	int	pad;

	//assert(output_length >= (input_length * 4 / 3));

	while (i < input_length) {
		pad = 3 - (input_length - i);
		if (pad == 2) {
			output[j  ] = basis_64[input[i]>>2];
			output[j+1] = basis_64[(input[i] & 0x03) << 4];
			output[j+2] = '=';
			output[j+3] = '=';
		} else if (pad == 1) {
			output[j  ] = basis_64[input[i]>>2];
			output[j+1] = basis_64[((input[i] & 0x03) << 4) | ((input[i+1] & 0xf0) >> 4)];
			output[j+2] = basis_64[(input[i+1] & 0x0f) << 2];
			output[j+3] = '=';
		} else{
			output[j  ] = basis_64[input[i]>>2];
			output[j+1] = basis_64[((input[i] & 0x03) << 4) | ((input[i+1] & 0xf0) >> 4)];
			output[j+2] = basis_64[((input[i+1] & 0x0f) << 2) | ((input[i+2] & 0xc0) >> 6)];
			output[j+3] = basis_64[input[i+2] & 0x3f];
		}
		i += 3;
		j += 4;
	}
	return j;
}

/* This assumes that an unsigned char is exactly 8 bits. Not portable code! :-) */
static unsigned char index_64[128] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,   62, 0xff, 0xff, 0xff,   63,
      52,   53,   54,   55,   56,   57,   58,   59,   60,   61, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
      15,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
      41,   42,   43,   44,   45,   46,   47,   48,   49,   50,   51, 0xff, 0xff, 0xff, 0xff, 0xff
};

#define char64(c)  ((c > 127) ? 0xff : index_64[(c)])

int Onvifbase64decode(const unsigned char *input, int input_length, unsigned char *output, int output_length)
{
	int		i = 0, j = 0, pad;
	unsigned char	c[4];

	//assert(output_length >= (input_length * 3 / 4));
	//assert((input_length % 4) == 0);
	while ((i + 3) < input_length) {
		pad  = 0;
		c[0] = char64(input[i  ]); pad += (c[0] == 0xff);
		c[1] = char64(input[i+1]); pad += (c[1] == 0xff);
		c[2] = char64(input[i+2]); pad += (c[2] == 0xff);
		c[3] = char64(input[i+3]); pad += (c[3] == 0xff);
		if (pad == 2) {
			output[j++] = (c[0] << 2) | ((c[1] & 0x30) >> 4);
			output[j]   = (c[1] & 0x0f) << 4;
		} else if (pad == 1) {
			output[j++] = (c[0] << 2) | ((c[1] & 0x30) >> 4);
			output[j++] = ((c[1] & 0x0f) << 4) | ((c[2] & 0x3c) >> 2);
			output[j]   = (c[2] & 0x03) << 6;
		} else {
			output[j++] = (c[0] << 2) | ((c[1] & 0x30) >> 4);
			output[j++] = ((c[1] & 0x0f) << 4) | ((c[2] & 0x3c) >> 2);
			output[j++] = ((c[2] & 0x03) << 6) | (c[3] & 0x3f);
		}
		i += 4;
	}
	return j;
}

static signed char* Pack_init(unsigned char Pack[], unsigned char Byte);
static signed char* Pack_byte(signed char* Count, unsigned char Byte);
static unsigned char* End_byte(signed char* Count);

int  tiff6_PackBits(unsigned char array[], int count, unsigned char Pack[])
{
    int i = 0;
    signed char* Count = Pack_init(Pack, array[i]); i++;
    for (; i < count; i++) Count = Pack_byte(Count, array[i]);
    unsigned char* End = End_byte(Count); *End = '\0';
    return (End - Pack);
}
static int unPack_count(char Pack[], int count);
int  tiff6_unPackBits(char Pack[], int count, unsigned char array[] /*= NULL*/)
{
    if (!array) return unPack_count(Pack, count);
    int nRes = 0;
    signed char* Count = (signed char*)Pack;
    while ((char*)Count < (Pack+count))
    {
        int c = *Count;
        if (c<0)
        {
            int n = (1-c);
            memset(&(array[nRes]), Count[1], n);
            nRes += n;
        }
        else
        {
            int n = (1+c);
            memcpy(&(array[nRes]), &Count[1], n);
            nRes += n;
        }
        Count = (signed char*)End_byte(Count);
    }
    return nRes;
}

int unPack_count(char Pack[], int count)
{
    int nRes = 0;
    signed char* Count = (signed char*)Pack;
    while ((char*)Count < (Pack+count))
    {
        int c = *Count;
        if (c<0)
            nRes += (1-c);
        else
            nRes += (1+c);
        Count = (signed char*)End_byte(Count);
    }
    return nRes;
}
signed char* Pack_init( unsigned char Pack[], unsigned char Byte )
{
    signed char* Cnt = (signed char*)Pack;
    *Cnt = 0; Pack[1] = Byte;
    return Cnt;
}
unsigned char* End_byte( signed char* Count )
{
    unsigned char* Pack = (unsigned char*)(Count+1);
    signed char c = *Count;
    if (c >0)
         Pack = &(Pack[c+1]);
    else Pack = &(Pack[1]);
    return Pack;
}
signed char* Pack_byte( signed char* Count, unsigned char Byte )
{
    signed char c = *Count;
    unsigned char* End = End_byte(Count);
    if (127 == c || c == -127)
        return Pack_init(End, Byte);
    else
    {
        unsigned char* Pack = End-1;
        if (*(Pack) == Byte)
        {
            if (c >0)
            {
                (*Count) = c-1;
                Count = Pack_byte(Pack_init(Pack, Byte), Byte);
            }
            else (*Count)--;
        }
        else
        {
            if (c >= 0)
            {
                *End = Byte;
                (*Count)++;
            }
            else Count = Pack_init(End, Byte);
        }
    }
    return Count;
}


int get_cellmotion_mask(int channel,unsigned char *pmask ,  int *masklen)
{
	if(!pmask)
		return -1;
	QMAPI_NET_CHANNEL_MOTION_DETECT stMotionDetect;
	memset(&stMotionDetect , 0 , sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));
	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, channel, &stMotionDetect, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT)))
		return -2;
		
	int motionearlen = QMAPI_MD_STRIDE_SIZE;
	if(*masklen < motionearlen)
		return -3;
		
	unsigned char lowArea[50];
	int lowAreaLen = 50;
	memset(lowArea , 0 , sizeof(lowArea));
	int i = 0 ;
	int index = 0;
	for(i = 0; i < QMAPI_MD_STRIDE_WIDTH * QMAPI_MD_STRIDE_HEIGHT; i++)
	{
		if(( stMotionDetect.byMotionArea[i/8] >> (i%8)) & 0x01)
		{
			index = (i/(QMAPI_MD_STRIDE_WIDTH * 2)  * 22) + (i % 44 / 2);
			lowArea[index/8] |= (0x80 >> (index%8));
		}
		
	}
	

	unsigned char src[256];
	memset(src , 0 , sizeof(src));
	unsigned int u32PackBitsLen = 0;
	u32PackBitsLen = tiff6_PackBits((unsigned char *)lowArea, lowAreaLen , src);
	memset(pmask , 0 , *masklen);
	*masklen = Onvifbase64encode(src , u32PackBitsLen , pmask , *masklen);
	
	return 0;
}


int set_cellmotion_mask(int channel,unsigned char *pmask , unsigned int masklen)
{
	if(!pmask)
		return -1;
	QMAPI_NET_CHANNEL_MOTION_DETECT stMotionDetect;
	memset(&stMotionDetect , 0 , sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));
	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, channel, &stMotionDetect, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT)))
		return -2;
	int motionearlen = 22 * 18;
	if(masklen > motionearlen)
		return -3;
	unsigned char tmp[256];
	memset(tmp , 0 , sizeof(tmp));
	unsigned char lowArea[256];	
	int len = 256; 
	len = Onvifbase64decode(pmask , masklen , tmp , len);
	int i = 0;
	len = tiff6_unPackBits((char *)tmp , len , lowArea);
	int index = 0;

	memset(stMotionDetect.byMotionArea , 0   , sizeof(stMotionDetect.byMotionArea));
	for(i = 0 ; i < motionearlen ; i ++)
	{
		if(( lowArea[i/8] ) & (0x80  >> (i%8)))
		{
			index = (i / 22 ) * QMAPI_MD_STRIDE_WIDTH * 2 + 2*(i % 22);
			stMotionDetect.byMotionArea[index/8] |= (0x01 << (index%8));
			index = (i / 22 ) * QMAPI_MD_STRIDE_WIDTH * 2 + 2*(i % 22) + 1;
			stMotionDetect.byMotionArea[index/8] |= (0x01 << (index%8));
			index = (i / 22 ) * QMAPI_MD_STRIDE_WIDTH * 2 + 2*(i % 22) + 44;
			stMotionDetect.byMotionArea[index/8] |= (0x01 << (index%8));
			index = (i / 22 ) * QMAPI_MD_STRIDE_WIDTH * 2 + 2*(i % 22) + 45;
			stMotionDetect.byMotionArea[index/8] |= (0x01 << (index%8));
			
		}
	}
	
	//??????
	stMotionDetect.bEnable = 1;
	for(i = 0 ; i < 7 ; i ++)
	{
		stMotionDetect.stScheduleTime[i][0].byStartHour = 0;
		stMotionDetect.stScheduleTime[i][0].byStartMin = 0;
		stMotionDetect.stScheduleTime[i][0].byStopHour = 23;
		stMotionDetect.stScheduleTime[i][0].byStopMin = 59;
	}

	stMotionDetect.stHandle.wDuration = 1;
	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_MOTIONCFG, channel, &stMotionDetect, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT)))
		return -4;


	return 0 ;
}


int get_cellmotion_sensitive(int channel,int * sensitive)
{
	
	if(!sensitive)
		return -1;
		
	QMAPI_NET_CHANNEL_MOTION_DETECT stMotionDetect;
	memset(&stMotionDetect , 0 , sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));
	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, channel, &stMotionDetect, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT)))
		return -2;
	
	*sensitive  = stMotionDetect.dwSensitive ;
		
	return 0;
}

int set_cellmotion_sensitive(int channel,int  sensitive)
{
	

	QMAPI_NET_CHANNEL_MOTION_DETECT stMotionDetect;
	memset(&stMotionDetect , 0 , sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT));
	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, channel, &stMotionDetect, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT)))
		return -2;
		
	stMotionDetect.dwSensitive = sensitive;
	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_MOTIONCFG, channel, &stMotionDetect, sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT)))
			return -4;
			
	return 0;
}


int set_videosource_rotate(int channel )
{

	int ret = 0;
	QMAPI_NET_CHANNEL_ENHANCED_COLOR stEnhancedColor;
	memset(&stEnhancedColor , 0 , sizeof(stEnhancedColor));
	ret = get_enhanced_color(channel, &stEnhancedColor);
	if(ret != 0)
	{
		return ret;
	}
	QMAPI_NET_CHANNEL_COLOR_SINGLE stColorSingle;
	memset(&stColorSingle , 0 , sizeof(stColorSingle));
	stColorSingle.dwSaveFlag = 1;
	stColorSingle.dwSetFlag = QMAPI_COLOR_SET_MIRROR;
	stColorSingle.dwSize = sizeof(stColorSingle);
	stColorSingle.nValue = (stEnhancedColor.nMirror + 1) % 4;
	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_COLORCFG_SINGLE, channel, &stColorSingle, sizeof(QMAPI_NET_CHANNEL_COLOR_SINGLE)))
			return -2;
	return 0;
}


int set_systemrestore_soft()
{

	QMAPI_NET_CHANNEL_COLOR stColorInfo;
	memset(&stColorInfo , 0 , sizeof(stColorInfo));
	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEF_COLORCFG, 0, &stColorInfo, sizeof(QMAPI_NET_CHANNEL_COLOR)))
	{
		return -1;
	}

	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_COLORCFG, 0, &stColorInfo, sizeof(QMAPI_NET_CHANNEL_COLOR)))
	{
		return -2;
	}

	return 0;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
