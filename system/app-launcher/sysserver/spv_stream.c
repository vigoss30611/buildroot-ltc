#include <stdio.h>  
#include <errno.h>  
#include <pthread.h>
#include <sys/socket.h>
#include "sys_server.h"
#include <signal.h>
#include <time.h>
#include <netinet/in.h>  


typedef enum _stream_codec_type {
    H264    = 1,
    MPEG    = 2,
    MJPEG   = 3,
    H265    = 4,
    H265_HISILICON    = 5,
    MJPEG_DIFT        = 6,
    G711A   = 101,
    ULAW    = 102,
    G711U   = 103,
    PCM     = 104,
    ADPCM   = 105,
    G721    = 106,
    G723    = 107,
    G726_16 = 108,
    G726_24 = 109,
    G726_32 = 110,
    G726_40 = 111,
    AAC     = 112,
    JPG     = 200,
} stream_t;




typedef struct stream_head_s
{
   int  magic;
   short key;
   short codec;
   int TS;
   int len;
}STREAM_HEAD;


int streamReadFrame(int* mediaType, // H265|264
              int* timeStamp, 
              char* pBuff, 
              int* pSize, // INPUT: BUFF SIZE, OUTPUT: DATA LEN.
              int* pIsKeyFrame, int reset)
{

    int ret = get_p2p_video_frame_with_header(pBuff, pSize, pIsKeyFrame, timeStamp, reset);
    if(ret < 0) {
        printf("get p2p frame failed\n");
        return -1;
    }
    switch(get_p2p_video_stream_type()) {
        case P2P_MEDIA_H264:
            *mediaType = H264;
            break;
        case P2P_MEDIA_H265:
            *mediaType = H265;
            break;
        case P2P_MEDIA_MJPEG:
            *mediaType = MJPEG;
            break;
        default:
            *mediaType = H264;
            break;
    }
    return 0;
}




void* th_stream(int sfd)
{    printf("th_stream sfd=%d\n",sfd);
    int media_type=0;
	int timestamp=0;
	 int isKeyFrame=0;
	 int size=0;
	 int state=1;
	 int reset=1;
    unsigned char *pBuff = NULL;
    pBuff = (unsigned char*) calloc(1, 1024*1024);
    if (NULL == pBuff) {
        printf("th_stream cant't calloc pBuff\n");
        return NULL;
    }  
	 int ret=0;
	 int sendLen=0;
	 int index=0;
	 int head_len=sizeof(STREAM_HEAD);
     start_p2p_stream();
     STREAM_HEAD stream_head;
	 signal(SIGPIPE, SIG_IGN);
	 static int count=0;
	 while(1)
	 {
	           size = 1024*1024-head_len;
	           ret = streamReadFrame(&media_type,  &timestamp, pBuff+head_len, &size, &isKeyFrame, reset);
				if(ret==0)
				{
                    if(reset) {
                        reset = 0;
                    } 
					 if(!isKeyFrame&&(state==1))
					 {
					   printf("skip size=%d\n",size);
					   continue;
					 }
					 state=0;
					 stream_head.magic=htonl(0x12345678);
					 stream_head.key=htons(isKeyFrame);
					 stream_head.codec=htons(media_type);
					 stream_head.TS=htonl(timestamp);
					 stream_head.len=htonl(size);
					 memcpy(pBuff,&stream_head,head_len);
					 sendLen=size+head_len;
					 ret=0;index=0;
					 while(ret!=sendLen)
					 {
						 ret=send(sfd,pBuff+index,sendLen,0);
						// printf("send ret=%d,size=%d,head_len=%d\n",ret,size,head_len);
						 if((ret<0)&&(errno != EINTR) && (errno != EWOULDBLOCK)&&(errno != EAGAIN))
						 {
						   printf("send error:%d\n",errno);
						   break;
						 }else if(ret==sendLen)
						 {
						    break;
						 }
						 else if(ret>0)
						  {
						     printf("send little:ret=%d,size=%d\n",ret,sendLen);
						     index+=ret;
							  sendLen-=ret;   
						  }else
						  {
						     printf("send full:ret=%d,size=%d\n",ret,sendLen); 
						  }
						 usleep(1*1000);
					  }
					  if((ret<0)&&(errno != EINTR) && (errno != EWOULDBLOCK)&&(errno != EAGAIN))
					  {
						   printf("send error:%d,%s\n",errno,strerror(errno));
						   break;
					 }
			      if(((count++)%300)==0)
					{
					   printf("th_stream size=%d,count=%d,ret=%d,timestamp=%ul,codec=%d, key: %d\n",size,count,ret,timestamp,H264, isKeyFrame);
					}
				}else{
			      usleep(10*1000);
				   continue;
				}
				
	 }
	 stop_p2p_stream();
	 if(sfd>=0)
	 { 
	  printf("leave th_stream sfd=%d\n",sfd);
	    close(sfd);
		sfd=-1;
	 }
	 if (pBuff!=NULL) {
		 printf("th_stream exit, free(pBuff)\n");
        free(pBuff);
		 pBuff=NULL;
    }
	
    return NULL;
}


void init_stream_server()
{
    printf("init_stream_server\n");

	int servfd, clifd, on, ret;
	struct sockaddr_in servaddr, cliaddr;
	if ((servfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("create socket error!\n");
		return;
	}

	on = 1;
	ret = setsockopt( servfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    if(ret) {
        printf("setsockopt, SO_REUSEADDR failed\n");
    }

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8891);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	while((bind(servfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0))
	{
		printf("bind to port %d failure!\n", 8891);
		usleep(1*1000*1000);
	}

	struct timeval timeo = {10,0};



	if (listen(servfd, 10) < 0)
	{
		printf("call listen failure!\n");
		return;
	}

	int n, m;
	pid_t pid;

	char pin[6];
	while (1)
	{
		char addr_char[20];
		int port;
		long timestamp;
		//struct mypara para;
		socklen_t length = sizeof(cliaddr);
		clifd = accept(servfd,(struct sockaddr*)&cliaddr,&length);
		if (clifd < 0)
		{
			printf("error comes when call accept!\n");
			break;
		} 
		sprintf(addr_char,"%s",inet_ntoa(cliaddr.sin_addr));
		/*port = ntohs(cliaddr.sin_port);
		
		para.fd = clifd;
		para.addr = addr_char;
		para.port = port;
		DEBUG_MSG("fd = %d, addr = %s, port = %d", para.fd, para.addr, para.port);*/
		//pthread_ret2 = pthread_create(&t2, NULL, process_info, &(para));
		//pthread_join(t2,NULL);
        SpvCreateServerThread((void *)th_stream, (void *)clifd);
	}
	close(servfd);
	return;
}



