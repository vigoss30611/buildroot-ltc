#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/statfs.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#include <qsdk/videobox.h>
#include "spv_debug.h"
#include "spv_utils.h"
#include "spv_file_receiver.h"

#define BUFFER_SIZE 100*1024


 static int calcCRC(char*buf, int offset, int crcLen,int remain)
	{
		int MASK = 0x0001, CRCSEED = 0x0810;
		int start = offset;
		int end = offset + crcLen;
		
		char val;
		int i,j;
		for (i = start; i < end; i++)
		{
			val = buf[i];
			for (j = 0; j < 8; j++)
			{
				if (((val ^ remain) & MASK) != 0)
				{
					remain ^= CRCSEED;
					remain >>= 1;
					remain |= 0x8000;
				}
				else
				{
					remain >>= 1;
				}
				val >>= 1;
			}
		}
		return remain;
	}



static void FileReader(int sfd)
{
    //char recBuf[BUFFER_SIZE] = {0};
	char * recBuf= (char *)malloc(BUFFER_SIZE);
    char errmsg[128] = {0};
    int recNum = 0;
    int headerfound = 0;
    FRHeader header = {0};
    int filefd = -1;
    int magic = 0;
    int newpackage = 1;
    int writeleft = -1;
    int ret = 0;
    char *writePos = 0;
    int writelen = 0;
    char *filepath = NULL;
    int header_magic_size = sizeof(FRHeader) + 4;
    int remain=0;
	 int remainLeft=2;//two byte crc at last
	 char crc[2]={0};
    while(1) {
        recNum = read(sfd, recBuf, BUFFER_SIZE);
        if(recNum < 0) {
            ERROR("read socket failed, error: %s\n", strerror(errno));
            sprintf(errmsg, "read socket failed, error: %s\n", strerror(errno));
            ret = -1;
            goto socket_err;
        } else if(recNum == 0) {
            usleep(100000);
            continue;
        }

        writePos = recBuf;
        writelen = recNum;

        if(!headerfound) {
            magic = ntohl(*((uint32_t *)recBuf));
            INFO("magic number: %p,recNum=%d\n", magic,recNum);
            if(magic != MAGIC_NUMBER) {
                ERROR("invalid magic number, magic: 0x%p, MAGIC: 0x%p\n", magic, MAGIC_NUMBER);
                sprintf(errmsg, "invalid magic number, magic: 0x%p, MAGIC: 0x%p\n", magic, MAGIC_NUMBER);
                ret = -2;
                goto socket_err;
            }
            memcpy(&header, recBuf + 4, sizeof(FRHeader));
            header.id = ntohl(header.id);
            header.type = ntohl(header.type);
            header.length = ntohl(header.length);
            INFO("header info: flag:%d,id: %d, type: %d, length: %d,sizelen:%d\n",header.flag,header.id, header.type, header.length,sizeof(FRHeader));
            if(header.flag == PACKAGE_START || header.flag == PACKAGE_ALL) {
                headerfound = 1;
            } else {
                ERROR("no header found, discard it\n");
                sprintf(errmsg, "no header found, discard it\n");
                ret = -3;
                goto socket_err;
            }
            writeleft = header.length;
            switch(header.type) {
                case FILE_CALIBRATION:
                    filepath = CALIBRATION_FILE".tmp";
                    break;
                case FILE_UPGRADE:
                    filepath = UPGRADE_FILE".tmp";
                    break;
                case FILE_PCB_TEST:
                    filepath = PCB_TEST_FILE".tmp";
                    break;
                default:
                    INFO("not supported file type yet, %d\n", header.type);
                    sprintf(errmsg, "not supported file type yet, %d\n", header.type);
                    ret = -4;
                    goto socket_err;
                    
            }
            filefd = open(filepath, O_CREAT|O_TRUNC|O_WRONLY, 0777);
            if(filefd < 0) {
                ERROR("open file failed, path: %s\n", filepath);
                sprintf(errmsg, "open file failed, path: %s\n", filepath);
                ret = -5;
                goto socket_err;
            }
            writePos = recBuf + header_magic_size;
            writelen = recNum - header_magic_size;
        }
		if(header.type==FILE_UPGRADE)
		{
		   if(writelen>writeleft)
		   {
		      remainLeft=(writelen-writeleft)>2?2:(writelen-writeleft);
		       memcpy(crc,writePos+writeleft,remainLeft);
			   remainLeft=2-remainLeft;
		   }
		}
        writelen = writeleft > writelen ? writelen : writeleft;
		 if(header.type==FILE_UPGRADE)
		 {
            remain = calcCRC(writePos, 0,writelen,remain);
		 }
        int size = write(filefd, writePos, writelen);
        INFO("recevie file fragment, file_length:%d, writelen: %d, left: %d, size: %d\n", header.length, writelen, writeleft, size);
        writeleft -= writelen;

        if(writeleft <= 0) {
            SpvSetLed(LED_WORK, 1, 0, 0);
            SpvSetLed(LED_WORK1, 1, 0, 0);
            SpvSetLed(LED_WIFI, 1, 0, 0);
            INFO("file reveive SUCCESS\n");
            ret = 0;
            break;
        }
    }

    if(header.type==FILE_UPGRADE)
	{
		  while(remainLeft>0)
		  {
		        recNum = read(sfd, recBuf, BUFFER_SIZE);
		        if(recNum < 0) {
		            ERROR("read socket failed, error: %s\n", strerror(errno));
		            sprintf(errmsg, "read socket failed, error: %s\n", strerror(errno));
                    ret = -6;
		            goto socket_err;
		        } else if(recNum == 0) {
		            usleep(100000);
		            continue;
		        }
				
				writePos = recBuf;
			   writelen = recNum;
			    writelen = remainLeft > writelen ? writelen : remainLeft;
				 memcpy(crc+(2-remainLeft),writePos,writelen);
				remainLeft -= writelen;
		  }
		  INFO("crc,%x,%x\n",crc[0],crc[1]);
		  if(remain!=(int)((crc[1] & 0xff) | ((crc[0] << 8) & 0xff00)))
		  {
		     ERROR("read socket,crc error remain:%x,%x\n",remain,(int)((crc[1] & 0xff) | ((crc[0] << 8) & 0xff00)));
		     sprintf(errmsg, "read socket,crc error remain:%x,%x\n",remain,(int)((crc[1] & 0xff) | ((crc[0] << 8) & 0xff00)));
             ret = -7;
		     goto socket_err;
		  }
	}


   
    if(ret == 0) {
        char newpath[256] = {0};
        strncpy(newpath, filepath, strlen(filepath) - 4);
        rename(filepath, newpath);
        sync();
		if(header.type == FILE_CALIBRATION || header.type == FILE_PCB_TEST) {
			//makesure the calibration data has been written back
			system("umount /mnt/config; mount /dev/spiblock2 /mnt/config");
		}
    }
	 if(filefd >= 0)
       close(filefd); 
    write(sfd, "receive file ok", strlen("receive file ok"));
    sleep(1);
    close(sfd);
	 free(recBuf);
    if(header.type == FILE_CALIBRATION) {
        SpvDelayedShutdown(15*1000);//15s delayed
    }else if(header.type == FILE_UPGRADE)
    {
         INFO("FILE_UPGRADE reveive SUCCESS\n");
		  char cmd[256] = {0};
		  sprintf(cmd,"rm -fr %s",NETWORK_FILE);
		  system(cmd);
		  system("update_arm.sh");
    }
    return;

socket_err:
	 if(filefd >= 0)
       close(filefd); 
     char backmsg[256] = {0};
     sprintf(backmsg, "receive file failed, ret: %d, msg: %s\n", ret, errmsg);
    write(sfd, backmsg, strlen(backmsg));
    sleep(2);
    close(sfd);
	 if(header.type == FILE_UPGRADE)
    {
         INFO("FILE_UPGRADE fail\n");
		  char cmd[256] = {0};
		  sprintf(cmd,"rm -fr %s",UPGRADE_FILE);
		  system(cmd);
    }
	free(recBuf);
    return;
}


void init_file_receiver_server()
{
    INFO("init_file_receiver_server\n");
    prctl(PR_SET_NAME, __func__);

	int servfd, clifd, on, ret;
	struct sockaddr_in servaddr, cliaddr;
	if ((servfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		ERROR("create socket error!\n");
		return;
	}

	on = 1;
	ret = setsockopt( servfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    if(ret) {
        ERROR("setsockopt, SO_REUSEADDR failed\n");
    }

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(FILE_RECEIVER_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	while((bind(servfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0))
	{
		ERROR("bind to port %d failure!\n", FILE_RECEIVER_PORT);
		usleep(1*1000*1000);
	}

	struct timeval timeo = {10,0};

	/*
	if (setsockopt(servfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
	{
		printf("socket option  SO_SNDTIMEO, not support\n");
		return;
	}
	if (setsockopt(servfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo)) == -1)
	{
		printf("socket option  SO_SNDTIMEO, not support\n");
		return;
	}
	*/

	if (listen(servfd, 10) < 0)
	{
		ERROR("call listen failure!\n");
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
			ERROR("error comes when call accept!\n");
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
        SpvCreateServerThread((void *)FileReader, (void *)clifd);
	}
	close(servfd);
	return;
}


int SpvSendSocketMsg(char *addr, int port, char *msg, int length)
{
    if(length > 1024) {
        ERROR("buf is too big\n");
        return -1;
    }

    int sockfd;
    struct sockaddr_in client_addr;
    char recvbuffer[1024] = {0};
    struct hostent *host;
    int portnumber,nbytes;
    char *buf;

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
        return -2;
    }

    bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(port);
    client_addr.sin_addr.s_addr = inet_addr(addr);

    struct timeval timeo = {10,0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
    {
        ERROR("socket option  SO_SNDTIMEO, not support\n");
        return -3;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo)) == -1)
    {
        ERROR("socket option  SO_SNDTIMEO, not support\n");
        return -4;
    }

    if(connect(sockfd,(struct sockaddr *)(&client_addr),sizeof(struct sockaddr))==-1)
    {

        if(errno == EINPROGRESS)
        {
            ERROR("connect timeout error 1, err: %s\n", strerror(errno));
            return -5;
        }
        ERROR("connect timeout error: %s\n", strerror(errno));
        return -6;
    }

    INFO("sendbuffer = %s\n", msg);

    int send_length;
    send_length = strlen(msg);
    write(sockfd, msg, send_length);
    read(sockfd,recvbuffer,sizeof(recvbuffer));
    INFO("recv data is :%s\n",recvbuffer);
    close(sockfd);
    INFO("client send msg ok\n");
    return 0;
}

static int g_calibrate_sock_fd = -1;
static void PipSigHandler(int32_t sig)
{
    ERROR("catch SIGPIPE\n");
    close(g_calibrate_sock_fd);
    g_calibrate_sock_fd = -1;
}

int SpvCreateSocket(char *addr, int port)
{
    int sockfd = -1;
    struct sockaddr_in client_addr;
    struct hostent *host;
    int portnumber,nbytes;
    int ret = 0;

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
        return -2;
    }

    bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(port);
    client_addr.sin_addr.s_addr = inet_addr(addr);

    struct timeval timeo = {10,0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
    {
        ERROR("socket option  SO_SNDTIMEO, not support\n");
        close(sockfd);
        return -3;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo)) == -1)
    {
        ERROR("socket option  SO_RCVTIMEO, not support\n");
        close(sockfd);
        return -4;
    }

    if(connect(sockfd,(struct sockaddr *)(&client_addr),sizeof(struct sockaddr))==-1)
    {

        if(errno == EINPROGRESS)
        {
            ERROR("connect timeout error 1, err: %s\n", strerror(errno));
            close(sockfd);
            return -5;
        }
        //ERROR("connect timeout error: %s\n", strerror(errno));
        close(sockfd);
        return -6;
    }
    return sockfd;
}

int SpvCalibrate(char *addr, int port, char *msg, int length)
{
    char recvbuffer[1024] = {0};
    char url[128] = {0};
    int id = 0;
	int file_fd = -1;
	struct fr_buf_info buf = FR_BUFINITIALIZER;
    char filename[128];
    int ret = 0;
    INFO("enable_calibration\n");
    sleep(1);
    signal(SIGPIPE, PipSigHandler);
    while(GetSpvState()->calibration) {
        usleep(100000);
        if(g_calibrate_sock_fd < 0) {
            g_calibrate_sock_fd = SpvCreateSocket(addr, port);
        }
        if(g_calibrate_sock_fd < 0) {
            usleep(1000000);
            continue;
        }
#define CALCHN "jpeg-out"
        sprintf(filename, "/mnt/http/mmc/%d.jpg", id);
        sprintf(url, "http://172.3.0.2/mmc/%d.jpg", id);
        //ret = video_get_frame(CALCHN, &buf);
        ret = video_get_snap(CALCHN, &buf);
        if(ret < 0) {
            ERROR("video_get_snap failed, ret: %d\n", ret);
            continue;
        }

        INFO("sendbuffer = %s\n", url);
        id++;
        id = id%10;

        file_fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        if(-1 != file_fd) {
            int writesize = write(file_fd, buf.virt_addr, buf.size); //将其写入文件中
            if(writesize <0) {
                ERROR("write snapshot data to file fail, %d\n",ret );
                ret = -1;
            }
            close(file_fd);
        } else {
            ERROR("create %s file failed  \n", filename);
            ret = -1;
        }

        write(g_calibrate_sock_fd, url, strlen(url));
    }

    close(g_calibrate_sock_fd);
    g_calibrate_sock_fd = -1;
    INFO("client send msg ok\n");
    return 0;
}
