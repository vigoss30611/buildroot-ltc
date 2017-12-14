
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<time.h>
#include<unistd.h>
#include<sys/wait.h>
#include<pthread.h>
#include<time.h>
#include<sys/statfs.h>
#include<sys/stat.h>
#include<dirent.h>

#include"spv_media.h"

#include"spv_app.h"
#include"spv_utils.h"

#define BUFFER_SEND 1024
#define BUFFER_SIZE 1024 
#define CONFIG_FILE_SIZE 4096  
#define PIN_VALUE     222222
#define NEW_PIN_FILE "/config/new_pin"//"/etc/network/new_pin"
#define IPADDRFILE "/config/pin"//"/etc/network/pin"
#define ALIVE_PIN_FILE "/mnt/sd0/pin_alive"//"/etc/network/pin_alive"
#define CONFIG_FILE "/config/libramain.config"//"/opt/libra/config/libramain.config"
#define CONFIG_FILE_BAK "/root/libra/config/libramain.config.bak"//"/opt/libra/config/libramain.config.bak"
#define SERVER_PORT 8889 // define the defualt connect port id
#define LENGTH_OF_LISTEN_QUEUE 10 //length of listen queue in server
#define APPPORT 12345
#define PIN_TIME 60
#define KEY_SIZE 30
#define VALUE_SIZE 50
#define SDCARD_PATH "/mnt/sd0/"
#define SAVED_FILE_PATH "/mnt/sd0/DCIM"

//#define DEBUG_SWITCH 0
#ifdef GUI_DEBUG_ENABLE
#define ERROR_SWITCH 1
#define INFO_SWITCH 1
#endif
#define DEBUG_TAG "spv_app_debug"
#define ERROR_TAG "spv_app_error"
#define INFO_TAG "spv_app_info"

#ifdef DEBUG_SWITCH
#define DEBUG_MSG(fmt, args...) printf("%s: ",DEBUG_TAG);printf(fmt, ##args);printf("\n")
#else
#define DEBUG_MSG(fmt, args...)
#endif

#ifdef ERROR_SWITCH
#define ERROR_MSG(fmt, args...) printf("%s: ",ERROR_TAG);printf(fmt, ##args);printf("\n")
#else
#define ERROR_MSG(tag,date)
#endif

#ifdef INFO_SWITCH
#define INFO_MSG(fmt, args...) printf("%s: ",INFO_TAG);printf(fmt, ##args);printf("\n")
#else
#define INFO_MSG(tag,date)
#endif

/*
int send_message_to_gui(MSG_TYPE msg, char *keys[], char *values[], int argc)
{
   notify_app_value_updated(keys, values, argc, 1);
   return 0;
}*/

struct iplist
{
	char pin[20];
	char addr[20];
	int state;
};

struct mypara
{
	int fd;
	char *addr;
	int port;
};

static int seq = 0;
static int expand_buffer = 0;
static int setprop = 0;

/*use to sendmsg after feedback*/

static char *keyarr[20]; 
static char *valuearr[20];

static int cmd_num = 0;

static char file_content[CONFIG_FILE_SIZE];
struct iplist a[10];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define WIFIAP  0
#define WIFISTA 1


static int wifiState =WIFIAP;
	
static char * stristr(const char * str1, const char * str2)
{
	char *cp = (char *) str1;
	char *s1, *s2;

	if ( !*str2 )
		return((char *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;

		while ( *s1 && *s2 && !(tolower(*s1)-tolower(*s2)) )
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);

}


static int check_alive(struct iplist *b)
{
	//DEBUG_MSG("============check alive===============");
	FILE *fp;
	int length;
	char *p = NULL;
	char *buf1,*buf2;
	int len1, len2;
	char tmp[256];
	char result[256];
	int ret;
	ret=pthread_mutex_trylock(&mutex);/*\u5224\u65ad\u4e0a\u9501*/
	if(ret != EBUSY){
		if((fp = fopen(ALIVE_PIN_FILE,"r")) == NULL)
		{
			//INFO_MSG("no app device is online");
			pthread_mutex_unlock(&mutex);
			return -1; 
		}

		fseek(fp,0,SEEK_END);
		length=ftell(fp);
		p=malloc(length+1);
		rewind(fp);
		fread(p, 1, length, fp);

		fclose(fp);
		p[length]='\0';
		//DEBUG_MSG("length = %d, p = %s", length, p);
		pthread_mutex_unlock(&mutex);/*\u89e3\u9501*/
	}else
	{
	   return -1; 
	}

	memset(tmp, 0, sizeof(tmp));
	int alive_num;
	int length_line;
	
	//DEBUG_MSG("p = %s", p);
	if(strncasecmp(p, "alive_num", strlen("alive_num"))==0){
		buf1=stristr(p,"alive_num =");
		if(buf1==NULL){
			ERROR_MSG("cmd_getprop cmd format not fit 0");
			return -1;
		}
		buf2=stristr(p,"\r\n");
		if(buf2==NULL){
			ERROR_MSG("cmd_getprop cmd format not fit 1");
			return 0;
		}
		length_line = buf2 - buf1;
		memcpy(tmp,p+11,length_line-11);
		alive_num = atoi(tmp);

		memset(tmp, 0, sizeof(char)*256);
		DEBUG_MSG("alive_num:=%d",alive_num);
		length-=length_line+2;
		memcpy(tmp,p+length_line+2,length);
		DEBUG_MSG("cmd_content:=%s",tmp);
	}else{
		ERROR_MSG("cmd_getprop cmd format not fit 2");
		return -1;
	}

	//DEBUG_MSG("alive_num:=%d",alive_num);
	buf1 = strstr(tmp, "\r\n");
	length_line = buf1 - tmp;
	memcpy(result, tmp, length_line);
	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, result, length_line);
	memset(result, 0, sizeof(tmp));

	memcpy(result, tmp, 6);
	sprintf(b->pin, result);
	memset(result, 0, sizeof(result));
	memcpy(result, tmp+7, length_line-7);
	sprintf(b->addr, result);
	memset(result, 0, sizeof(result));
	free(p);
	if(length > 5){
		DEBUG_MSG("length = %d, b->addr = %s, b->pin = %s", length, b->addr, b->pin);
		return 0;
	}
	else{
		INFO_MSG("no app device is online");
		return -1;
	}
	return 0;
}

static void* init_my_client(char *clientsendbuffer)
{
    if(clientsendbuffer == NULL)
        return;

	struct iplist on_line_dev;
	char sendbuffer[BUFFER_SIZE];
	DEBUG_MSG("clientsendbuffer = %s", clientsendbuffer);
	//sprintf(sendbuffer,"%s", clientsendbuffer);
    strcpy(sendbuffer, clientsendbuffer);
    free(clientsendbuffer);
	
	if(check_alive(&on_line_dev))
	{
		INFO_MSG("check alive failed");
		return;
	}
	INFO_MSG("init_my_client on_line_dev.addr = %s, on_line_dev.pin = %s", on_line_dev.addr, on_line_dev.pin);

	int send_number = 0;
	DEBUG_MSG("spv_app: now start init_my_client");
	while(1){
                
		if(strlen(on_line_dev.pin) > 1){

			int sockfd;
			char recvbuffer[BUFFER_SIZE];
			struct sockaddr_in client_addr;
			struct hostent *host;
			int portnumber,nbytes;
			char *buf;
			int length;
 
			if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
			{
				fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
				return;
			}
 
			bzero(&client_addr,sizeof(client_addr));
			client_addr.sin_family=AF_INET;
			client_addr.sin_port=htons(APPPORT);
			client_addr.sin_addr.s_addr = inet_addr(on_line_dev.addr);
 
			struct timeval timeo = {10,0};
			if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
			{
				ERROR_MSG("socket option  SO_SNDTIMEO, not support");
				return;
			}
 
			if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo)) == -1)
			{
				ERROR_MSG("socket option  SO_SNDTIMEO, not support");
				return;
			}
	
		if(connect(sockfd,(struct sockaddr *)(&client_addr),sizeof(struct sockaddr))==-1)
			{
                      
				if(errno == EINPROGRESS)
				{
					ERROR_MSG("connect timeout error 1");
					return;
				}
				fprintf(stderr,"Connect error:%s\n",strerror(errno));
				ERROR_MSG("connect timeout error 2");
				return;
			}
                        
			DEBUG_MSG("sendbuffer = %s\n", sendbuffer);
                    
			int send_length;
			send_length = strlen(sendbuffer);
			write(sockfd,sendbuffer,send_length);
			DEBUG_MSG("write over");
			read(sockfd,recvbuffer,sizeof(recvbuffer));
			DEBUG_MSG("recv data is :%s",recvbuffer);
			close(sockfd);
			if(strlen(recvbuffer) == 0 && send_number <= 5)
			{
				INFO_MSG("recv message timeout, retry");
				send_number += 1; 
				continue;
			}
			else if (strlen(recvbuffer) == 0 && send_number > 5){
				INFO_MSG("recv message timeout, exit");
				break;
			}
			else{
				INFO_MSG("client send msg ok" );
				break;
			}
		}
	}
	return;
}

int notify_app_value_updated(char *key[], char *value[], int argc, int type)
{
	INFO_MSG("notify_app_value_updated message received");
	struct iplist on_line_dev;
    char *clientsendbuffer = malloc(BUFFER_SIZE);
    if(clientsendbuffer == NULL) {
        ERROR_MSG("malloc for clientsendbuffer failed");
        return -1;
    }
    memset(clientsendbuffer, 0, BUFFER_SIZE);
	if(check_alive(&on_line_dev))
	{
		INFO_MSG("check alive failed");
		return -1;
	}
        
	DEBUG_MSG("on_line_dev.addr = %s, on_line_dev.pin = %s", on_line_dev.addr, on_line_dev.pin);

	sprintf(clientsendbuffer, "");
	if(type==1)
		sprintf(clientsendbuffer, "pin:%s\r\ncmdfrom:phone\r\ncmdseq:%d\r\ncmdtype:setprop\r\ncmd_num:%d\r\n", on_line_dev.pin, seq, argc);
	else
		sprintf(clientsendbuffer, "pin:%s\r\ncmdfrom:spv\r\ncmdseq:%d\r\ncmdtype:setprop\r\ncmd_num:%d\r\n", on_line_dev.pin, seq, argc);
	seq += 1;
        
	char tmp[50];
	int m;
	for(m=0;m<argc;m++)
	{
		sprintf(tmp, "%s:%s;", *(key+m), *(value+m));
		INFO_MSG("tmp=%s\n",tmp);
        strcat(clientsendbuffer, tmp);
	}
       
	strcat(clientsendbuffer, "\r\nend");//end tag

    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&t, &attr, (void *)init_my_client, clientsendbuffer);
    pthread_attr_destroy (&attr);

	return ret;
}


static int check_new_pin(char *pin)
{
	DEBUG_MSG("=============check_new_pin==============");
	FILE *pFile;
	int length;
	pFile=fopen(IPADDRFILE,"r");
	fseek(pFile,0,SEEK_END);
	length=ftell(pFile);

	char *p;
	int j=0;
	int isok=0;
	p=malloc(length+1);
	rewind(pFile);
	fread(p,1,length,pFile);
	fclose(pFile);
	p[length]='\0';

	while(j<=length)
	{
		if((p[j]==pin[0])&&(strncmp(p+j,pin,strlen(pin))==0))
		{
			DEBUG_MSG("pin: %s already exist",pin);
			free(p);
			return 0;//return 1;
		}
		j++;
	}
	INFO_MSG("pin = %s",pin);
	free(p);
	return 0;
}


int new_pin(char *pin)
{
	FILE *fp;
	char buf[6];

	int i,num;
	while(1){	
		num = 0;
		srand((unsigned)time(NULL));
		for(i=0;i<6;i++)
		{
			num = num*10 + rand()%10;
			if(num==0)
				i--;
		}
		num=PIN_VALUE;
		DEBUG_MSG("random num = %d", num);
		memset(buf, 0x00, sizeof(buf));
		sprintf(buf, "%d", num);
		if(!check_new_pin(buf)){
			break;
		}
	}

	if((fp=fopen(NEW_PIN_FILE,"w+"))==NULL)
	{
		ERROR_MSG("open NEW_PIN_FILE error, it may not exist");
		getchar();
		return -1;
	}
	fwrite(buf, strlen(buf), 1, fp);
	fclose(fp);
	sprintf(pin, buf);
	return 0;
}

int delete_pin()
{

	if (access(NEW_PIN_FILE, 0) == 0)
	{
		unlink(NEW_PIN_FILE);
	}else{
		return 0;
	}
	if (access(NEW_PIN_FILE, 0) == 0)
	{
		ERROR_MSG("can not delete NEW_PIN_FILE");
		return -1;
	}

}

static int cmd_set_date(time_t time)
{
	struct timeval tv;
	tv.tv_sec = time;
	tv.tv_usec = 0;
	DEBUG_MSG("now set date\n");
	if(settimeofday(&tv, (struct timezone*)0) < 0)
		return -1;
	INFO_MSG("date set msg deal ok");
	return 0;
}

static int cmd_set_wifi(char *name, char *password)
{
	DEBUG_MSG("==========================cmd_set_wifi========================");
	char na[20];
	char pa[20];
	sprintf(na, name);
	sprintf(pa, password);
	DEBUG_MSG("na = %s, pa = %s",na, pa);

	if(strncasecmp(na, "OFF", strlen("OFF"))==0){
		//stop_softap(void);
		INFO_MSG("wifi_set_OFF");
	}
	else
	{
		//stop_softap(void);
		//set_softap(ssid, pa);

		char syscmd[BUFFER_SIZE];
		//sprintf(syscmd,"/wifi/realtek8188/rtl8188.sh AP %s WEP %s &", name, password);
		sprintf(syscmd,"/wifi/broadcom_ap6210/ap6210_setup.sh AP %s WPA-PSK %s &", name, password); 
		//sprintf(syscmd,"/wifi/realtek8188/rtl8188.sh AP chulong WPA2PSK admin888");
		system(syscmd);
		INFO_MSG("reset wifi msg deal ok");
	}
	if(access(ALIVE_PIN_FILE,0) == 0)
		unlink(ALIVE_PIN_FILE);	
	return 0;
}

static int cmd_shutter(char *smsg,char *value)
{
	keyarr[0] = (char *)malloc(KEY_SIZE);
	valuearr[0] = (char *)malloc(VALUE_SIZE);
	sprintf(keyarr[0], "action.shutter.press");
	sprintf(valuearr[0], value);
    INFO_MSG("cmd_shutter value=%s\n",value);
	//if(notify_app_value_updated(keyarr, valuearr, 1, 1))
	//	printf("msg send error\n");

	int ret = send_message_to_gui(MSG_DO_ACTION, keyarr, valuearr, 1);
	free(keyarr[0]);
	free(valuearr[0]);
	if(ret != 0){
		ERROR_MSG("cmd_shutter: send_message_to_gui return error");
		sprintf(smsg, "msg deal error");
		return -1;
	}
	else{
		INFO_MSG("cmd_shutter message deal ok");
		sprintf(smsg, "shutter send ok");
		return 0;
	}
}

static int cmd_wifi(char *smsg,char *value)
{
	keyarr[0] = (char *)malloc(KEY_SIZE);
	valuearr[0] = (char *)malloc(VALUE_SIZE);
	sprintf(keyarr[0], "action.wifi.set");
	sprintf(valuearr[0], value);
    INFO_MSG("cmd_wifi value=%s\n",value);
	//if(notify_app_value_updated(keyarr, valuearr, 1, 1))
	//	printf("msg send error\n");

	char*pstr=NULL;
	pstr=strstr(value,"STA");
	if(pstr!=NULL)
	{
	   wifiState=WIFISTA;
	}else
	{
	   wifiState=WIFIAP;
	}

	int ret = send_message_to_gui(MSG_DO_ACTION, keyarr, valuearr, 1);
	free(keyarr[0]);
	free(valuearr[0]);
	if(ret != 0){
		ERROR_MSG("cmd_wifi: send_message_to_gui return error");
		sprintf(smsg, "msg deal error");
		return -1;
	}
	else{
		INFO_MSG("cmd_wifi message deal ok");
		sprintf(smsg, "wifi send ok");
		return 0;
	}
}

static int cmd_locate_camera(char *smsg)
{
	

	return 0;
	/*
	keyarr[0] = (char *)malloc(KEY_SIZE);
	valuearr[0] = (char *)malloc(VALUE_SIZE);
	sprintf(keyarr[0], "status.camera.locating");
	sprintf(valuearr[0], "status.camera.locating");

	int ret = send_message_to_gui(MSG_DO_ACTION, keyarr, valuearr, 1);
	if(ret != 0){
		printf("msg deal error\n");
		sprintf(smsg, "status.camera.locating msg send error");
		return -1;
	}
	free(keyarr[0]);
	free(valuearr[0]);
	sprintf(smsg, "status.camera.locating msg send ok");
	return 0;
	*/
}


static int cmd_battery_info(char *smsg)
{
	keyarr[0] = (char *)malloc(KEY_SIZE);
	valuearr[0] = (char *)malloc(VALUE_SIZE);
	sprintf(keyarr[0], "status.camera.battery");
	sprintf(valuearr[0], "status.camera.battery");

	int ret = send_message_to_gui(MSG_DO_ACTION, keyarr, valuearr, 1);
	free(keyarr[0]);
	free(valuearr[0]);
	if(ret != 0){
		ERROR_MSG("cmd_battery_info: send_message_to_gui error\n");
		sprintf(smsg, "status.camera.battery msg send error");
		return -1;
	}

	INFO_MSG("battery info msg deal ok");
	sprintf(smsg, "status.camera.battery msg send ok");
	return 0;
}

static int cmd_sdcard_capacity(char *smsg)
{
	/*
	struct statfs statFS;
	long usedByte = 0;
	long freeByte = 0;
	long totalByte = 0;
	long endSpace = 0;
	long freeKB = 0;

	if(statfs(SDCARD_PATH, &statFS) == -1) 
	{
		printf("statfs failed for this path");
		return -1;
	}
	
	totalByte = statFS.f_blocks * statFS.f_frsize;
	freeByte = statFS.f_bfree * statFS.f_frsize;
	usedByte = totalByte - freeByte;

	printf("usedByte = %ld\n", usedByte);
	printf("freeByte = %ld\n", freeByte);
	printf("totalByte = %ld\n", totalByte);

	freeKB = freeByte/1024;
	sprintf(smsg, "status.camera.capacity=%ld/", freeKB);

	char syscmd[50];
	FILE *stream;
	char buf[1024];
	char sys_result[100];

	sprintf(syscmd,"find ./mnt/sd0/DCIM/100SPV/ -name \"*.MP4\" | wc -l");
	memset(buf, '\0', sizeof(buf));
	stream = popen(syscmd, "r");
	fread(buf, sizeof(char), sizeof(buf), stream);
	sprintf(sys_result, buf);
	printf("sys_result = %s\n", sys_result);
	strcat(smsg,sys_result);
	strcat(smsg,"/");

	sprintf(syscmd,"find ./mnt/sd0/DCIM/100SPV/ -name \"*.JPG\" | wc -l");
	memset(buf, '\0', sizeof(buf));
	stream = popen(syscmd, "r");
	fread(buf, sizeof(char), sizeof(buf), stream);
	sprintf(sys_result, buf);
	printf("sys_result = %s\n", sys_result);
	strcat(smsg,sys_result);

	pclose(stream);

	return 0;
	*/
	keyarr[0] = (char *)malloc(KEY_SIZE);
	valuearr[0] = (char *)malloc(VALUE_SIZE);
	sprintf(keyarr[0], "status.camera.capacity");
	sprintf(valuearr[0], "status.camera.capacity");

	//if(notify_app_value_updated(keyarr, valuearr, 1, 1))
	//	printf("msg send error\n");

	int ret = send_message_to_gui(MSG_DO_ACTION, keyarr, valuearr, 1);
	free(keyarr[0]);
	free(valuearr[0]);
	if(ret != 0){
		ERROR_MSG("msg deal error\n");
		sprintf(smsg, "status.camera.capacity msg send error");
		return -1;
	}
		sprintf(smsg, "status.camera.capacity msg send ok");
	return 0;
}

static int cmd_delete_last_file(char *smsg)
{
	if(access(SAVED_FILE_PATH,0) == 0)
	{
		DEBUG_MSG("now start cmd_delete_last_file");
		if(access("/mnt/sd0/infotm/video/19700101080317.avi", 0)==0)
		{
			unlink("/mnt/sd0/infotm/video/19700101080317.avi");
		}

		int n;
		char folderPath[128] = {0};
		char fileName[128] = {0};
		char syscmd[200]; 
		DEBUG_MSG("/mnt/sd0/DCIM exist");
		for(n=0;n<88;n++)
		{
			sprintf(folderPath, "/mnt/sd0/DCIM/10%dSPV", n);
			if(access(folderPath,0) != 0)
				break;
		}
		sprintf(folderPath, "/mnt/sd0/infotm/video");
		//sprintf(folderPath, "/mnt/sd0/DCIM/10%dSPV", n-1);
		if(access(folderPath,0) == 0)
		{
			DEBUG_MSG("/mnt/sd0/infotm/video exist");
			DIR *pDir;
			struct dirent *ent;
			int i = 0;
			char fileName[128] = {0};
			char lastName[128] = {0};
			char tmp[128];
			int serial = 0;
			int biggest = 0;

			pDir = opendir(folderPath);
			if(pDir == NULL)
				return;

			while((ent = readdir(pDir)) != NULL)
			{
				if(!(ent->d_type & DT_DIR))
				{
					strcpy(fileName, ent->d_name);
					if(strlen(fileName) > 12)
						memcpy(tmp, fileName+12, 4);
					else
						memcpy(tmp, fileName+4, 4); 
					
					serial = atoi(tmp);
					memset(tmp, 0, sizeof(char)*128);
					if(serial >= biggest)
					{
						biggest = serial;
						strcpy(lastName, ent->d_name);
					}
				}
			}
			sprintf(fileName, "%s/%s", folderPath, lastName);
			DEBUG_MSG("fileName = %s", fileName);
			if(access(fileName, 0)==0)
				unlink(fileName);
		}


		keyarr[0] = (char *)malloc(KEY_SIZE);
		valuearr[0] = (char *)malloc(VALUE_SIZE);
		sprintf(keyarr[0], "action.file.delete");
		sprintf(valuearr[0], "all");
		int ret = send_message_to_gui(MSG_DO_ACTION, keyarr, valuearr, 1);
		free(keyarr[0]);
		free(valuearr[0]);

		if(ret != 0){
			ERROR_MSG("cmd_delete_last_file: send_message_to_gui error\n");
			sprintf(smsg, "cmd_feedback:last file deleted erro");
			return -1;
		}
		INFO_MSG("delete last msg deal ok");
		sprintf(smsg, "cmd_feedback:last file deleted ok");
		return 0;
	}
	else
	{
		sprintf(smsg, "cmd_feedback:file not access");
		return -1;
	}
}


static int cmd_delete_select_file(char *smsg,char *msg)
{   
    DEBUG_MSG("now start to delete select file");
	if(access("/mnt/sd0/DCIM",0) == 0)
	{
	    DEBUG_MSG("/mnt/sd0/DCIM exist");
		char *buffer1,*buffer2;
		char msgtemp[BUFFER_SIZE]={0};
		char filename[50]={0};
		char tempname[50]={0};
		char type[10]={0};
		buffer1=strstr(msg, ";;");
		if(buffer1==NULL)
		{
				ERROR_MSG("can't find the prop in msg");
				return -1;
		}
		buffer1=buffer1+2;
		buffer2=strstr(buffer1, ";;");
		if(buffer2==NULL)
		{
				ERROR_MSG("can't find the prop in msg");
				return -1;
		}
		strncpy(type, buffer1, buffer2 - buffer1);
		strcpy(msgtemp,buffer2+2);
		
		while(strstr(msgtemp, ";;")!=NULL){
			buffer2=strstr(msgtemp, ";;");
			if(buffer2==NULL)
			{
				ERROR_MSG("can't find the prop in msg");
				return -1;
			}
			memset(filename, 0, sizeof(char)*50);
			strncpy(filename, msgtemp,buffer2-msgtemp);
			strcpy(msgtemp,buffer2+2);
			int n;
		    char docname[100];
			char thumbnail[100];
		    char syscmd[200];
			//delele all Video,Locked or Photo media file
			if(strcasecmp(filename, "all")==0){
				sprintf(docname, "/mnt/sd0/DCIM/100CVR/%s",type);
				if(access(docname,0) == 0){
				DEBUG_MSG("%s exist", docname);
				sprintf(syscmd, "rm -rf %s", docname);
				DEBUG_MSG("syscmd = %s", syscmd);
				INFO_MSG("syscmd is %s\n",syscmd);
				system(syscmd);
				}
				break;
				}
			
			buffer2=strstr(filename, ".");
			if(buffer2==NULL)
			{
				ERROR_MSG("can't find the prop in msg");
				return -1;
			}
			memset(tempname, 0, sizeof(char)*50);
			strncpy(tempname, filename,buffer2-filename);
			//delete the selected media file
			if(strcasecmp(type, "Video")==0){
				buffer2=strstr(filename, ".");
				if(buffer2==NULL)
					{
						ERROR_MSG("can't find the prop in msg");
						return -1;
					}
				memset(tempname, 0, sizeof(char)*50);
				strncpy(tempname, filename,buffer2-filename);
				sprintf(docname, "/mnt/sd0/DCIM/100CVR/Video/%s",filename);
				sprintf(thumbnail, "/mnt/sd0/DCIM/100CVR/Video/.%s.jpg",tempname);
				INFO_MSG("delete vedio docname is %s\n",docname);
				INFO_MSG("delete vedio thumbnail is %s\n",thumbnail);
				if(access(docname,0) == 0){
					if(unlink(docname)){
						DEBUG_MSG("unlink docname fail,  docname path: %s\n", docname);
					}
				}
				if(access(thumbnail,0) == 0){
					if(unlink(thumbnail)){
						DEBUG_MSG("unlink thumbnail fail,  docname path: %s\n", thumbnail);
					}
				}
				
			}
			else if(strcasecmp(type, "Locked")==0){
				buffer1=strstr(filename, "_");
				if(buffer1==NULL)
					{
						ERROR_MSG("can't find the prop in msg");
						return -1;
					}
				buffer1=buffer1+1;
				buffer2=strstr(buffer1, ".");
				if(buffer2==NULL)
					{
						ERROR_MSG("can't find the prop in msg");
						return -1;
					}
				memset(tempname, 0, sizeof(char)*50);
				strncpy(tempname, buffer1,buffer2-buffer1);
				sprintf(docname, "/mnt/sd0/DCIM/100CVR/Locked/%s",filename);
				sprintf(thumbnail, "/mnt/sd0/DCIM/100CVR/Locked/.%s.jpg",tempname);
				INFO_MSG("delete Lock docname is %s\n",docname);
				INFO_MSG("delete lock thumbnail is %s\n",thumbnail);
				if(access(docname,0) == 0){
					if(unlink(docname)){
						DEBUG_MSG("unlink docname fail,  docname path: %s\n", docname);
					}
				}
				if(access(thumbnail,0) == 0){
					if(unlink(thumbnail)){
						DEBUG_MSG("unlink thumbnail fail,  docname path: %s\n", thumbnail);
					}
				}

			}
			else if(strcasecmp(type, "Photo")==0){
				sprintf(docname, "/mnt/sd0/DCIM/100CVR/Photo/%s",filename);
								INFO_MSG("delete vedio docname is %s\n",docname);
				INFO_MSG("delete photo filename is %s\n",filename);
				if(access(docname,0) == 0){
					if(unlink(docname)){
						DEBUG_MSG("unlink docname fail,  docname path: %s\n", docname);
					}
				}

			}else{
				sprintf(smsg, "cmd_feedback:file delete failed");
				return -1;

		    }


			}
			
		ScanSdcardMediaFiles(IsSdcardMounted());

		keyarr[0] = (char *)malloc(KEY_SIZE);
		valuearr[0] = (char *)malloc(VALUE_SIZE);
		sprintf(keyarr[0], "action.file.delete");
		sprintf(valuearr[0], "select");

		int ret = send_message_to_gui(MSG_DO_ACTION, keyarr, valuearr, 1);
		free(keyarr[0]);
		free(valuearr[0]);
		if(ret != 0){
			ERROR_MSG("cmd_delete_select_file: send_message_to_gui error\n");
			sprintf(smsg, "cmd_feedback:file delete failed");
			return -1;
		}
		INFO_MSG("delete select file msg deal ok");
		sprintf(smsg, "cmd_feedback:file delete ok");
		return 0;
	}
	else
	{
		sprintf(smsg, "cmd_feedback:file delete failed");
		return -1;
	}
}


static int cmd_delete_all_file(char *smsg)
{
	DEBUG_MSG("now start to delete all file");
	if(access("/mnt/sd0/DCIM",0) == 0)
	{
		int n;
		char docname[100];
		char syscmd[200]; 
		DEBUG_MSG("/mnt/sd0/DCIM exist");
			sprintf(docname, "/mnt/sd0/DCIM/100CVR/");
			if(access(docname,0) == 0){
				DEBUG_MSG("%s exist", docname);
				sprintf(syscmd, "rm -rf %s", docname);
				DEBUG_MSG("syscmd = %s", syscmd);
				system(syscmd);
			}
		system("mkdir -p /mnt/sd0/DCIM/100CVR/");
		ScanSdcardMediaFiles(IsSdcardMounted());

		keyarr[0] = (char *)malloc(KEY_SIZE);
		valuearr[0] = (char *)malloc(VALUE_SIZE);
		sprintf(keyarr[0], "action.file.delete");
		sprintf(valuearr[0], "last");

		int ret = send_message_to_gui(MSG_DO_ACTION, keyarr, valuearr, 1);
		free(keyarr[0]);
		free(valuearr[0]);
		if(ret != 0){
			ERROR_MSG("cmd_delete_all_file: send_message_to_gui error\n");
			sprintf(smsg, "cmd_feedback:file delete failed");
			return -1;
		}
		INFO_MSG("delete all file msg deal ok");
		sprintf(smsg, "cmd_feedback:file delete ok");
		return 0;
	}
	else
	{
		sprintf(smsg, "cmd_feedback:file delete failed");
		return -1;
	}
}
static int action_event(char *backmsg, char *smsg ,int lensmsg)
{
	DEBUG_MSG("action event!");
	DEBUG_MSG("=======now get cmd_content========");
	char msg[BUFFER_SIZE];
	char cmd_content[BUFFER_SIZE];
	char cmd_content2[BUFFER_SIZE];
	char tmp[BUFFER_SIZE];
	memset(msg, 0, sizeof(char)*BUFFER_SIZE);
	memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
	memset(cmd_content2, 0, sizeof(char)*BUFFER_SIZE);
	memset(tmp, 0, sizeof(char)*BUFFER_SIZE);
	memcpy(msg,smsg,lensmsg);
	char *buf0, *buf1, *buf2, *buf3;


	int length;
	buf0 = stristr(msg, "\r\n");
	length = buf0 - msg + 2;
	length = strlen(msg) - length;
	memcpy(tmp, buf0+2, length);
	sprintf(msg, tmp);
	DEBUG_MSG("msg = %s\n", msg);


	if(strncasecmp(msg, "action.time.set", strlen("action.time.set"))==0){
		DEBUG_MSG("=======msg= %s=======", msg);
		buf1=stristr(msg,"action.time.set:");
		if(buf1==NULL){
			ERROR_MSG("msg format not fit: can't find pin");
			return -1;
		}
		buf2=stristr(msg,"\r\n");
		if(buf2==NULL){
			ERROR_MSG("msg format not fit: can't find '\r\n'");
			return -1;
		}
		length = buf2 - buf1;
		memcpy(cmd_content,msg+16,length-17);
		DEBUG_MSG("cmd_content :=%s\n",cmd_content);
		time_t time = atoi(cmd_content);
		if(cmd_set_date(time))
		{
			sprintf(backmsg, "time set error");
			return -1;
		}
		sprintf(backmsg, "time set ok");
	}
	else if(strncasecmp(msg, "action.wifi.set", strlen("action.wifi.set"))==0){
		buf0=strchr(msg,':');
		buf0++;
		buf1=stristr(buf0,"\r\n");
		if(buf1==NULL)
		{
		   ERROR_MSG("can not find rn in string wifi\n");
		}else
		{
		   buf1[0]=0;
		}
		if(cmd_wifi(backmsg,buf0))
			return -1;
	}
	else if(strncasecmp(msg, "action.shutter.press", strlen("action.shutter.press"))==0){
        buf0=strchr(msg,':');
		buf0++;
		buf1=strchr(msg,';');
		buf1[0]=0;
		if(cmd_shutter(backmsg,buf0))
			return -1;
	}
	else if(strncasecmp(msg, "status.camera.capacity", strlen("status.camera.capacity"))==0){
		if(cmd_sdcard_capacity(backmsg))
			return -1;
	}
	else if(strncasecmp(msg, "action.file.delete:last", strlen("action.file.delete:last"))==0){
		if(cmd_delete_last_file(backmsg))
			return -1;
	}
	else if(strncasecmp(msg, "action.file.delete:all", strlen("action.file.delete:all"))==0){
		if(cmd_delete_all_file(backmsg))
			return -1;
	}else if(strncasecmp(msg, "action.file.delete:select", strlen("action.file.delete:select"))==0){
		if(cmd_delete_select_file(backmsg,msg))
			return -1;
	}else if(strncasecmp(msg, "status.camera.battery", strlen("status.camera.battery"))==0){
		if(cmd_battery_info(backmsg))
			return -1;
	}
	else if(strncasecmp(msg, "status.camera.locating", strlen("status.camera.locating"))==0){
		if(cmd_locate_camera(backmsg))
			return -1;
	} else if(strncasecmp(msg, "action.setup.format", strlen("action.setup.format"))==0) {
        int ret = SpvFormatSdcard();
        if(!ret) {
            GoingToScanMedias(GetMainWindow(), 10);
            sprintf(backmsg, "sdcard format ok");
        } else {
            sprintf(backmsg, "sdcard format failed");
            return -1;
        }
	} else if(strncasecmp(msg, "action.setup.reset", strlen("action.setup.reset"))==0) {
        ResetCamera();
        sprintf(backmsg, "camera reset ok");
	}else{
		ERROR_MSG("msg format not fit: wrong action type........");
		sprintf(backmsg, "cmd_feedback:cmd not recognized");
		return -1;
	}
	return 0;
}

static int is_prop_valid(char *key, char *value)
{
	DEBUG_MSG("cmd_find_content!");
	char msg[CONFIG_FILE_SIZE];
	char key_content[BUFFER_SIZE];
	char value_content1[BUFFER_SIZE];
	char value_content2[BUFFER_SIZE];
	char value_content3[BUFFER_SIZE];
	char value_content4[BUFFER_SIZE];
	memset(key_content, 0, sizeof(char)*BUFFER_SIZE);
	memset(value_content1, 0, sizeof(char)*BUFFER_SIZE);
	memset(value_content2, 0, sizeof(char)*BUFFER_SIZE);
	memset(value_content3, 0, sizeof(char)*BUFFER_SIZE);
	memset(value_content4, 0, sizeof(char)*BUFFER_SIZE);
	memset(msg, 0, sizeof(char)*BUFFER_SIZE);
	sprintf(key_content, key);
	sprintf(value_content1, " %s ", value);
	sprintf(value_content2, "#%s ", value);
	sprintf(value_content3, " (%s) ", value);
	sprintf(value_content3, "#(%s) ", value);
	char *buf1, *buf2, *buf3, *buf4, *buf5, *buf6;

	if(access(CONFIG_FILE_BAK,0) == 0)
	{
		DEBUG_MSG("CONFIG_FILE_BAK still exist");
		FILE *pFile;
		int length;
		pFile=fopen(CONFIG_FILE_BAK,"r");
		fseek(pFile,0,SEEK_END);
		length=ftell(pFile);

		char *p;
		int j=0;
		int isok=0;
		p=malloc(length+1);
		rewind(pFile);
		fread(p,1,length,pFile);
		fclose(pFile);
		p[length]='\0';
		memcpy(msg,p,length);
		free(p);

		if(stristr(msg, key_content) != NULL){
			buf1=stristr(msg, key_content);
			if(buf1==NULL){
				ERROR_MSG("can't find the prop in CONFIG_FILE_BAK");
				return -1;
			}
			DEBUG_MSG("find the content:%s", key_content);
			int len = buf1 - msg;
			memset(key_content, 0, sizeof(char)*BUFFER_SIZE);
			memcpy(key_content, buf1, BUFFER_SIZE);

			buf2=stristr(key_content, "\n");
			if(buf2==NULL){
				ERROR_MSG("can't find '\n', the prop is not valid");
				return -1;
			}
			len = buf2 - key_content;
			memset(msg, 0, sizeof(char)*CONFIG_FILE_SIZE);
			memcpy(msg, key_content, len);
			DEBUG_MSG("msg = %s", msg);
			strcat(msg," ");

			buf3 = stristr(msg, value_content1);
			buf4 = stristr(msg, value_content2);
			buf5 = stristr(msg, value_content3);
			buf6 = stristr(msg, value_content4);
			if( buf3!= NULL || buf4 != NULL || buf5!= NULL || buf6 != NULL)
			{
				DEBUG_MSG("this prop is valid");
				return 0;
			}else{
				ERROR_MSG("prop invalid");
				return -1;
			}
		}else
		{
			ERROR_MSG("can't find cmd_content in CONFIG_FILE_BAK");
			return -1;
		}
	}
	else
	{
		ERROR_MSG("CONFIG_FILE_BAK file not exist");
		return -1;
	}

}

static int cmd_find_content(char *content, int contentlen, char *result)
{
	DEBUG_MSG("cmd_find_content!");
	char msg[CONFIG_FILE_SIZE];
	char cmd_content[BUFFER_SIZE];
	memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
	memset(msg, 0, sizeof(char)*BUFFER_SIZE);
	memcpy(cmd_content,content,contentlen);
	char *buf1, *buf2, *buf3, *buf4;

	if(access(CONFIG_FILE,0) == 0)
	{
		DEBUG_MSG("CONFIG_FILE exist");
		FILE *pFile;
		int length;
		pFile=fopen(CONFIG_FILE,"r");
		fseek(pFile,0,SEEK_END);
		length=ftell(pFile);

		char *p;
		int j=0;
		int isok=0;
		p=malloc(length+1);
		rewind(pFile);
		fread(p,1,length,pFile);
		fclose(pFile);
		p[length]='\0';
		memcpy(msg,p,length);
		free(p);
		DEBUG_MSG("cmd_content = %s", cmd_content);

		if(stristr(msg, cmd_content) != NULL){
			buf1=stristr(msg, cmd_content);
			if(buf1==NULL){
				ERROR_MSG("can't find the prop in CONFIG_FILE");
				return -1;
			}
			INFO_MSG("find the content:%s\n", cmd_content);
			int len = buf1 - msg;
			memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
			memcpy(cmd_content, buf1, BUFFER_SIZE);

			buf2=stristr(cmd_content,"=");
			if(buf2==NULL){
				ERROR_MSG("can't find '=', the prop is not valid");
				return -1;
			}
			buf3=stristr(cmd_content,"\n");
			if(buf3==NULL){
				ERROR_MSG("can't find \'n', the prop is not valid");
				return -1;
			}
			len = buf3 - buf2;
			memset(msg, 0, sizeof(char)*CONFIG_FILE_SIZE);
			memcpy(msg,buf2+1, len-1);

			buf4=stristr(msg,"#");
			if(buf4!=NULL)
			{
				char tmp[BUFFER_SIZE];
				memset(tmp, 0, sizeof(char)*BUFFER_SIZE);
				memcpy(tmp, msg, buf4-msg);
				memset(msg, 0, sizeof(char)*CONFIG_FILE_SIZE);
				sprintf(msg, tmp);
				DEBUG_MSG("msg = %s", msg);

			}
		 	len = strlen(msg);
			while(msg[len-1] == ' ')
			{
				msg[len-1] = '\0';
				//printf("msg = %s\n", msg);
				len = strlen(msg);
				//printf("len = %d\n", len);
			}
			sprintf(result, "%s", msg);
			DEBUG_MSG("result = %s", result);

			return 0;
		}else
		{
			ERROR_MSG("can't find the prop in CONFIG_FILE");
			return -1;
		}
	}
	else
	{
		ERROR_MSG("spv config file not exist");
		return -1;
	}

}

static int cmd_setprop(char *backmsg, char *smsg ,int lensmsg)
{
	DEBUG_MSG("=======now in cmd_setprop========");
	char msg[BUFFER_SIZE];
	char cmd_content[BUFFER_SIZE];
	char key[BUFFER_SIZE];
	char value[BUFFER_SIZE];
	memset(msg, 0, sizeof(char)*BUFFER_SIZE);
	memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
	memset(key, 0, sizeof(char)*BUFFER_SIZE);
	memset(value, 0, sizeof(char)*BUFFER_SIZE);
	memcpy(msg,smsg,lensmsg);
	char *buf1, *buf2, *buf3;
	int length;
	int cmd_number;
	int lenmsg = lensmsg;

	sprintf(backmsg, "cmd_feedback: properties send error");
	if(strncasecmp(msg, "cmd_num", strlen("cmd_num"))==0){
		buf1=stristr(msg,"cmd_num:");
		if(buf1==NULL){
			ERROR_MSG("cmd_setprop cmd format not fit 1");
			return -1;
		}
		buf2=stristr(msg,"\r\n");
		if(buf2==NULL){
			ERROR_MSG("cmd_setprop cmd format not fit 2");
			return -1;
		}
		length = buf2 - buf1;
		memcpy(cmd_content,msg+8,length-8);
		cmd_number = atoi(cmd_content);

		memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
		DEBUG_MSG("cmd_num:=%d",cmd_number);
		lenmsg-=length+2;
		memcpy(cmd_content,msg+length+2,lenmsg);
		memset(msg, 0, sizeof(char)*BUFFER_SIZE);
		memcpy(msg, cmd_content, lenmsg);
		DEBUG_MSG("cmd_content:=%s",msg);
		memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
	}else{
		ERROR_MSG("cmd_setprop cmd format not fit 0");
		return -1;
	}

	int i;
	int len;
	sprintf(backmsg, "");
	char send_msg[BUFFER_SIZE];

	cmd_num = cmd_number;
	for (i=0;i<cmd_number;i++)
	{
		buf1=stristr(msg,";");
		if(buf1==NULL){
			ERROR_MSG("cmd_setprop cmd format not fit 3");
			return -1;
		}
		memcpy(cmd_content,msg,buf1-msg);
		len = buf1 -msg;
		len = strlen(msg) -len;
		memcpy(msg,buf1+1,len);

		buf2=stristr(cmd_content,":");
		if(buf2==NULL){
			ERROR_MSG("cmd_setprop cmd format not fit 4");
			return -1;
		}
		memcpy(key, cmd_content, buf2-cmd_content);
		len = buf2-cmd_content;
		len = strlen(cmd_content) - len;
		memcpy(value, buf2+1, len);
		DEBUG_MSG("key[%d] = %s, value[%d] = %s", i, key, i, value);

		if(is_prop_valid(key, value)){
			sprintf(backmsg, "cmd_feedback: properties get error");
			cmd_num = 0;
			ERROR_MSG("properties get error, it may be not valid");
			return -1;
        		}
                INFO_MSG("value=%s\n",value);
		keyarr[i] = (char *)malloc(KEY_SIZE);
		valuearr[i] = (char *)malloc(VALUE_SIZE);
		sprintf(keyarr[i], "%s",key);
		sprintf(valuearr[i], "%s",value);
                INFO_MSG("valuearr[0=%s]\n",valuearr[0]);
		strcat(backmsg, key);
		strcat(backmsg, ":");
		strcat(backmsg, value);
		strcat(backmsg, ";");
		DEBUG_MSG("backmsg = %s", backmsg);

		memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
		memset(key, 0, sizeof(char)*BUFFER_SIZE);
		memset(value, 0, sizeof(char)*BUFFER_SIZE);
	}	
	setprop = 1;
	INFO_MSG("set prop msg deal ok");
	return 0;
}

static int cmd_getprop(char *backmsg, char *smsg ,int lensmsg)
{
	DEBUG_MSG("=======now in cmd_getprop========");
	char msg[BUFFER_SIZE];
	char cmd_content[BUFFER_SIZE];
	char cmd_result[BUFFER_SIZE];
	memset(msg, 0, sizeof(char)*BUFFER_SIZE);
	memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
	memset(cmd_result, 0, sizeof(char)*BUFFER_SIZE);
	memcpy(msg,smsg,lensmsg);
	char *buf1, *buf2, *buf3;
	int length;
	int cmd_number;
	int lenmsg = lensmsg;

	sprintf(backmsg, "cmd_feedback: properties get error");
	if(strncasecmp(msg, "cmd_num", strlen("cmd_num"))==0){
		buf1=stristr(msg,"cmd_num:");
		if(buf1==NULL){
			ERROR_MSG("cmd_getprop cmd format not fit 0");
			return -1;
		}
		buf2=stristr(msg,"\r\n");
		if(buf2==NULL){
			ERROR_MSG("cmd_getprop cmd format not fit 1");
			return -1;
		}
		length = buf2 - buf1;
		memcpy(cmd_content,msg+8,length-8);
		cmd_number = atoi(cmd_content);

		memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
		DEBUG_MSG("cmd_num:=%d",cmd_number);
		lenmsg-=length+2;
		memcpy(cmd_content,msg+length+2,lenmsg);
		memset(msg, 0, sizeof(char)*BUFFER_SIZE);
		memcpy(msg, cmd_content, lenmsg);
		DEBUG_MSG("cmd_content:=%s",msg);
		memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
	}else{
		ERROR_MSG("cmd_getprop cmd format not fit 2");
		return -1;
	}

	int i;
	int len;
	sprintf(backmsg, "");
	for (i=0;i<cmd_number;i++)
	{
		buf1=stristr(msg,";");
		if(buf1==NULL){
			ERROR_MSG("cmd_getprop cmd format not fit 3");
			sprintf(backmsg, "cmd_feedback: properties get error");
			return -1;
		}
		memcpy(cmd_content,msg,buf1-msg);
		if(cmd_find_content(cmd_content, strlen(cmd_content), cmd_result)){
			ERROR_MSG("properties get error, it may be not valid");
			sprintf(backmsg, "cmd_feedback: properties get error");
			return -1;
		}
		len = buf1 -msg;
		len = strlen(msg) -len;
		memcpy(msg,buf1+1,len);
		strcat(backmsg, cmd_content);
		strcat(backmsg,":");
		strcat(backmsg, cmd_result);
		strcat(backmsg, ";");
		DEBUG_MSG("backmsg = %s", backmsg);
		memset(cmd_content, 0, sizeof(char)*BUFFER_SIZE);
		memset(cmd_result, 0, sizeof(char)*BUFFER_SIZE);

	}	
	INFO_MSG("get prop msg deal ok");
	return 0;
}

static void send_file_list_to_app(char *addr) {
    extern FileList g_media_files[MEDIA_COUNT];
    char *filebuf = NULL;
    int bufSize = 4096;
    int filecount = 0;
    int i = 0, j = 0;
    for(i = 0; i < MEDIA_COUNT; i++) {
        filecount += g_media_files[i].fileCount;
    }
    bufSize = filecount * 40 + strlen("filelist, count:, list:") + 40;
    filebuf = (char *)malloc(bufSize);
    if(filebuf == NULL) {
        ERROR_MSG("malloc for file buf failed, bufSize: %d\n", bufSize);
        return;
    }
    memset(filebuf, 0, sizeof(bufSize));
    sprintf(filebuf, "filelist, count:%d, list:", filecount);

    for(i = 0; i < MEDIA_COUNT; i++) {
        PFileNode pNode = g_media_files[i].pHead;
        for(j = 0; pNode != NULL && j < g_media_files[i].fileCount; j++) {
            strcat(filebuf, pNode->fileName);
            strcat(filebuf, "/");
            pNode = pNode->next;
        }
    }
    strcat(filebuf, "\r\nend");//end tag

    int sockfd;
    char recvbuffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    struct hostent *host;
    int portnumber,nbytes;
    char *buf;
    int length;

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
        return;
    }

    bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(APPPORT);
    client_addr.sin_addr.s_addr = inet_addr(addr);

    struct timeval timeo = {10,0};
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
    {
        ERROR_MSG("socket option  SO_SNDTIMEO, not support");
        return;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo)) == -1)
    {
        ERROR_MSG("socket option  SO_SNDTIMEO, not support");
        return;
    }

    if(connect(sockfd,(struct sockaddr *)(&client_addr),sizeof(struct sockaddr))==-1)
    {
        if(errno == EINPROGRESS)
        {
            ERROR_MSG("connect timeout error 1");
            return;
        }
        fprintf(stderr,"Connect error:%s\n",strerror(errno));
        ERROR_MSG("connect timeout error 2");
        return;
    }

    int s_length;
    socklen_t optl = sizeof(int);
    getsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,&s_length,&optl);
    INFO_MSG("send buffer = %d",s_length); // 发送缓冲区的大小


    int send_length = strlen(filebuf);
    INFO_MSG("spv____ begin send, length: %d, bufsize: %d, filecount: %d, buf: %s", send_length, bufSize, filecount, filebuf);

#if 0
    //every times, we send less than BUFFER_SEND buff
    int sendTimes = send_length / BUFFER_SEND + 1;
    int sendsize = 0, send_num = 0, sendCount = 0;
    char *sendbuf = NULL;
    do {
        sendbuf = filebuf + sendCount * BUFFER_SEND;
        sendsize = strlen(sendbuf) > BUFFER_SEND ? BUFFER_SEND : strlen(sendbuf);
        send_num = write(sockfd, sendbuf, sendsize);//, MSG_DONTROUTE
        sendCount ++;
        DEBUG_MSG("spv___ send end, send_num: %d",send_num);
    } while(sendCount < sendTimes);
#else
    int send_num = write(sockfd, filebuf, send_length);
    if(send_num != send_length) {
        ERROR_MSG("send file list failed, send_length: %d, send_num: %d", send_length, send_num);
    }
#endif

    read(sockfd,recvbuffer,sizeof(recvbuffer));
    DEBUG_MSG("recv data is :%s",recvbuffer);
    close(sockfd);

    if(addr != NULL) {
        free(addr);
        addr = NULL;
    }
    free(filebuf);
    filebuf = NULL;


}

static int cmd_getfilelist(char *smsg, char *addr)
{
    sprintf(smsg, "get file list send failed");
    char *appAddr = malloc(strlen(addr) + 1);
    if(appAddr == NULL) {
        ERROR_MSG("malloc for appAddr failed");
        return -1;
    }
    strcpy(appAddr, addr);

    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&t, &attr, (void *)send_file_list_to_app, appAddr);
    if(!ret) {
        sprintf(smsg, "get file list send ok");
    }
    pthread_attr_destroy (&attr);
    return ret;
}

static int init_event(char *msg)
{
	DEBUG_MSG("init event");
	char spvinfo[64];
	if(access(CONFIG_FILE,0) == 0)
	{
		DEBUG_MSG("CONFIG_FILE exist");
		FILE *pFile;
		int length;
		pFile=fopen(CONFIG_FILE,"r");
		fseek(pFile,0,SEEK_END);
		length=ftell(pFile);

		char *p;
		int j=0;
		int isok=0;
		p=malloc(length+1);
		rewind(pFile);
		fread(p,1,length,pFile);
		fclose(pFile);
		p[length]='\0';
		memcpy(msg,p,length);
		free(p);
	}
	else
	{
		ERROR_MSG("CONFIG_FILE not exist\n");
		return -1;
	}


	strcat(msg,"\r\n");

	int ret = GetConfigValue("status.camera.capacity", spvinfo);
	DEBUG_MSG("spvinfo = %s", spvinfo);
	strcat(msg, "status.camera.capacity=");
	strcat(msg, spvinfo);
	strcat(msg,"\r\n");

	ret = GetConfigValue("status.camera.battery", spvinfo);
	DEBUG_MSG("spvinfo = %s", spvinfo);
	strcat(msg, "status.camera.battery=");
	strcat(msg, spvinfo);
	strcat(msg,"\r\n");


	ret = GetConfigValue("status.camera.working", spvinfo);
	DEBUG_MSG("spvinfo = %s", spvinfo);
	strcat(msg, "status.camera.working=");
	strcat(msg, spvinfo);
	strcat(msg,"\r\n");

#ifdef GUI_CAMERA_REAR_ENABLE
	ret = GetConfigValue("status.camera.rear", spvinfo);
	DEBUG_MSG("spvinfo = %s", spvinfo);
	strcat(msg, "status.camera.rear=");
	strcat(msg, spvinfo);
	strcat(msg,"\r\n");
#endif

	INFO_MSG("init msg deal ok");
	return 0;
}

static int pairing_event(char *pin)
{
	char buf[10];
	sprintf(buf,"%s\r\n",pin);
	DEBUG_MSG("pairing event!");
	if(access(NEW_PIN_FILE,0) == 0)
	{
		DEBUG_MSG("NEW_PIN_FILE  exist\n");
		FILE *pFile;
		int length;
		pFile=fopen(NEW_PIN_FILE,"r");
		fseek(pFile,0,SEEK_END);
		length=ftell(pFile);

		char *p;
		int j=0;
		int isok=0;
		p=malloc(length+1);
		rewind(pFile);
		fread(p,1,length,pFile);
		fclose(pFile);
		p[length]='\0';

		while(j<=length)
		{
			if((p[j]==pin[0])&&(strncmp(p+j,pin,strlen(pin))==0))
			{
				DEBUG_MSG("pin: %s exist, continue!",pin);
				//delete the file and add to file pin
				//unlink(NEW_PIN_FILE);
				
				FILE *fp;
				if((fp = fopen(IPADDRFILE,"a+")) == NULL)
				{
					ERROR_MSG("open NEW_PIN_FILE error");
					free(p);
					return -1;
				}
				fwrite(buf, strlen(buf), 1, fp);
				fclose(fp);
				free(p);
				INFO_MSG("pairing msg deal ok");
				return 0;
			}
			j++;
		}
		ERROR_MSG("pin: %s not exist, cannot do any operation!",pin);
		free(p);
		return -1;
	}
	else
	{
		ERROR_MSG("NEW_PIN_FILE not exist\n");
		return -1;
	}
}

static int old_check_pin(char *pin, char *addr)
{
	DEBUG_MSG("=============check_pin==============");
	FILE *pFile;
	int length;
	pFile=fopen(IPADDRFILE,"r");
	fseek(pFile,0,SEEK_END);
	length=ftell(pFile);

	char *p;
	int j=0;
	int isok=0;
	p=malloc(length+1);
	rewind(pFile);
	fread(p,1,length,pFile);
	fclose(pFile);
	p[length]='\0';

	while(j<=length)
	{
		if((p[j]==pin[0])&&(strncmp(p+j,pin,strlen(pin))==0))
		{
			DEBUG_MSG("pin: %s exist, continue!",pin);
			isok = 1;
			break;
		}
		j++;
	}
	free(p);
	if(isok == 0){
		ERROR_MSG("pin: %s not exist, cannot do any operation!",pin);
		return -1;
	}
	isok = 0;
	
	int k=0;
	int stop = -1;
	int ret;
	while(1)
	{
		ret=pthread_mutex_trylock(&mutex);/*\u5224\u65ad\u4e0a\u9501*/
		if(ret != EBUSY){
			for(k=0;k<10;k++)
			{
				//printf("a[k] = %s %s %d\n", a[k].pin, a[k].addr, a[k].state);
				if(!strcmp(a[k].pin, pin))
				{
					sprintf(a[k].addr,"%s", addr);
					a[k].state = PIN_TIME;
					isok = 1;
				}
				if(strlen(a[k].pin) == 0 && stop < 0)
					stop = k;
			}
			if(!isok){
				DEBUG_MSG("break; stop = %d ",stop);
				sprintf(a[stop].pin, "%s", pin);
				sprintf(a[stop].addr,"%s", addr);
				a[stop].state = PIN_TIME;
				DEBUG_MSG("a[%d] = %s %s %d", stop,a[stop].pin, a[stop].addr, a[stop].state);

				char buf[100];
				sprintf(buf, "%s\t%s\r\n", a[stop].pin, a[stop].addr);

				FILE *fp;
				if((fp = fopen(ALIVE_PIN_FILE,"w+")) == NULL)
				{
					ERROR_MSG("open ALIVE_PIN_FILE error when check pin");
					return -1;
				}
				fwrite(buf, strlen(buf), 1, fp);
				fclose(fp);
			}

			/*
			printf("after modified\n\n");
			for(k=0;k<10;k++)
				printf("a[%d] : %s %s %d\n", k, a[k].pin, a[k].addr, a[k].state);
			printf("\n\n");
			*/
			pthread_mutex_unlock(&mutex);/*\u89e3\u9501*/
			break;
		}
		sleep(1);
	}
	return 0;
}



static int check_pin(char *pin, char *addr)
{
	DEBUG_MSG("=============check_pin==============");
	FILE *pFile;
	int length;
	pFile=fopen(IPADDRFILE,"r");
	fseek(pFile,0,SEEK_END);
	length=ftell(pFile);

	char *p;
	int j=0;
	int isok=0;
	p=malloc(length+1);
	rewind(pFile);
	fread(p,1,length,pFile);
	fclose(pFile);
	p[length]='\0';

	while(j<=length)
	{
		if((p[j]==pin[0])&&(strncmp(p+j,pin,strlen(pin))==0))
		{
			DEBUG_MSG("pin: %s exist, continue!",pin);
			isok = 1;
			break;
		}
		j++;
	}
	free(p);
	if(isok == 0){
		ERROR_MSG("pin: %s not exist, cannot do any operation!",pin);
		return -1;
	}
	isok = 0;
	
	int k=0;
	int stop = -1;
	int ret;
	ret=pthread_mutex_trylock(&mutex);/*\u5224\u65ad\u4e0a\u9501*/
	if(ret != EBUSY){
		for(k=0;k<10;k++)
		{
			//printf("a[k] = %s %s %d\n", a[k].pin, a[k].addr, a[k].state);
			if(!strcmp(a[k].pin, pin))
			{
			    if(!strcmp(a[k].addr, addr))
				{
				   	sprintf(a[k].addr,"%s", addr);
					a[k].state = PIN_TIME;
					isok = 1;
				}else
				{
				    INFO_MSG("new addr a[k].addr=%s >> addr:%s",a[k].addr,addr);
					stop = k;
				}
				
			}
			if(strlen(a[k].pin) == 0 && stop < 0)
				stop = k;
		}
		if(!isok){
			DEBUG_MSG("break; stop = %d ",stop);
			sprintf(a[stop].pin, "%s", pin);
			sprintf(a[stop].addr,"%s", addr);
			a[stop].state = PIN_TIME;
			DEBUG_MSG("a[%d] = %s %s %d", stop,a[stop].pin, a[stop].addr, a[stop].state);

			char tmp[50];
			char buf[256];
			int alive_count = 0;
			for(k=0;k<10;k++)
			{
				if(strlen(a[k].pin) > 0)
					alive_count++;
			}
			sprintf(buf, "alive_num = %d\r\n", alive_count);

			for(k=0;k<10;k++)
			{
				if(strlen(a[k].pin) > 0){
					sprintf(tmp, "%s\t%s\r\n", a[stop].pin, a[stop].addr);
					strcat(buf, tmp);
				}
			}
			DEBUG_MSG("file_content = \n%s", buf);
			FILE *fp;
			if((fp = fopen(ALIVE_PIN_FILE,"w+")) == NULL)
			{
				ERROR_MSG("open ALIVE_PIN_FILE error when check pin");
				return -1;
			}
			fwrite(buf, strlen(buf), 1, fp);
			fclose(fp);
		}
		pthread_mutex_unlock(&mutex);/*\u89e3\u9501*/
	}
	return 0;
}

static int msg_deal(char *smsg, int len, char *addr, char *recv_buf, int fd, int port) 
{
	char msg[BUFFER_SIZE];
	char option[BUFFER_SIZE];
	char backmsg[BUFFER_SIZE];
	char *buf1, *buf2;
	int ret=0;
	int length;
	int lenmsg = len; 

	memset(msg, 0, sizeof(char)*BUFFER_SIZE);
	memset(option, 0, sizeof(char)*BUFFER_SIZE);
	memcpy(msg,smsg,lenmsg);


	char pin[20];
	char cmdfrom[20];
	char cmdseq[20];
	char cmd_type[20];
	char cmd_num[20];
	char cmd_content[BUFFER_SIZE];

	memset(pin,0,sizeof(char)*20);
	memset(cmdfrom,0,sizeof(char)*20);
	memset(cmdseq,0,sizeof(char)*20);
	memset(cmd_type,0,sizeof(char)*20);
	memset(cmd_num,0,sizeof(char)*20);
	memset(cmd_content,0,sizeof(char)*BUFFER_SIZE);
	
	sprintf(smsg, "cmd_feedback: msg not fit the format");

	if(strncasecmp(msg, "pin", strlen("pin"))==0){
		DEBUG_MSG("=======now get pin========");
		DEBUG_MSG("=======msg= %s=======", msg);
		buf1=stristr(msg,"pin:");
		if(buf1==NULL){
			ERROR_MSG("msg not fit the format, can't find pin");
			return -1;
		}
		buf2=stristr(msg,"\r\n");
		if(buf2==NULL){
			ERROR_MSG("msg not fit the format, can't find pin end");
			return -1;
		}
		length = buf2 - buf1;
		memcpy(option,msg+4,length-4);
		memcpy(pin, option, length-4);
		memset(option, 0, sizeof(char)*BUFFER_SIZE);
		DEBUG_MSG("pin:=%s",pin);
		lenmsg-=length+2;
		memcpy(option,msg+length+2,lenmsg);
		memset(msg, 0, sizeof(char)*BUFFER_SIZE);
		memcpy(msg, option, lenmsg);
		memset(option, 0, sizeof(char)*BUFFER_SIZE);
	}else
	{
		ERROR_MSG("msg not fit the format, can't find pin in the head");
		return -1;
	}

	if(strncasecmp(msg, "cmdfrom", strlen("cmdfrom"))==0){
		DEBUG_MSG("=======now get cmdfrom========");
		buf1=stristr(msg,"cmdfrom:");
		if(buf1==NULL){
			ERROR_MSG("msg not fit the format, can't find cmdfrom");
			return -1;
		}
		buf2=stristr(msg,"\r\n");
		if(buf2==NULL){
			ERROR_MSG("msg not fit the format, can't find cmdfrom end");
			return -1;
		}
		length = buf2 - buf1;
		memcpy(option,msg+8,length-8);
		memcpy(cmdfrom, option, length-8);
		memset(option, 0, sizeof(char)*BUFFER_SIZE);
		DEBUG_MSG("cmdfrom:=%s",cmdfrom);
		lenmsg-=length+2;
		memcpy(option,msg+length+2,lenmsg);
		memset(msg, 0, sizeof(char)*BUFFER_SIZE);
		memcpy(msg, option, lenmsg);
		memset(option, 0, sizeof(char)*BUFFER_SIZE);
	}else
	{
		ERROR_MSG("msg not fit the format, can't find cmdfrom in the head");
		return -1;
	}

	if(strncasecmp(msg, "cmdseq", strlen("cmdseq"))==0){
		DEBUG_MSG("=======now get cmdseq========");
		buf1=stristr(msg,"cmdseq:");
		if(buf1==NULL){
			ERROR_MSG("msg not fit the format, can't find cmdseq");
			return -1;
		}
		buf2=stristr(msg,"\r\n");
		if(buf2==NULL){
			ERROR_MSG("msg not fit the format, can't find cmdseq end");
			return -1;
		}
		length = buf2 - buf1;
		memcpy(option,msg+7,length-7);
		memcpy(cmdseq, option, length-7);
		memset(option, 0, sizeof(char)*BUFFER_SIZE);
		DEBUG_MSG("cmdseq:=%s",cmdseq);
		lenmsg-=length+2;
		memcpy(option,msg+length+2,lenmsg);
		memset(msg, 0, sizeof(char)*BUFFER_SIZE);
		memcpy(msg, option, lenmsg);
		memset(option, 0, sizeof(char)*BUFFER_SIZE);
	}else
	{
		ERROR_MSG("msg not fit the format, can't find cmdseq in the head");
		return -1;
	}

	if(strncasecmp(msg, "cmd_type", strlen("cmd_type"))==0){
		DEBUG_MSG("=======now get cmd_type========");
		buf1=stristr(msg,"cmd_type:");
		if(buf1==NULL){
			ERROR_MSG("msg not fit the format, can't find cmd_type");
			return -1;
		}
		buf2=stristr(msg,"\r\n");
		if(buf2==NULL){
			ERROR_MSG("msg not fit the format, can't find cmd_type end");
			return -1;
		}
		length = buf2 - buf1;
		memcpy(option,msg+9,length-9);
		memcpy(cmd_type, option, length-9);
		memset(option, 0, sizeof(char)*BUFFER_SIZE);
		DEBUG_MSG("cmd_type:=%s",cmd_type);
		lenmsg-=length+2;
		memcpy(option,msg+length+2,lenmsg);
		memset(msg, 0, sizeof(char)*BUFFER_SIZE);
		memcpy(msg, option, lenmsg);
		memset(option, 0, sizeof(char)*BUFFER_SIZE);
	}else
	{
		ERROR_MSG("msg not fit the format, can't find cmd_type in the head");
		return -1;
	}

	if(strncasecmp(cmd_type, "pairing", strlen("pairing"))==0){
		INFO_MSG("msg type is pairing......");
		if(!pairing_event(pin))
		{
			sprintf(smsg, "cmd_feedback:pairing ok");

			keyarr[0] = (char *)malloc(KEY_SIZE);
			valuearr[0] = (char *)malloc(VALUE_SIZE);
			sprintf(keyarr[0], "pairing_ok");
			sprintf(valuearr[0], "pairing_ok");

			int ret = send_message_to_gui(MSG_PIN_SUCCEED, keyarr, valuearr, 1);
			if(ret != 0){
				ERROR_MSG("pairing msg send_message_to_gui deal error\n");
				sprintf(backmsg, "msg deal error");
			}
			free(keyarr[0]);
			free(valuearr[0]);
		}else{
			ERROR_MSG("pairing msg deal error\n");
			sprintf(smsg, "cmd_feedback:pairing error");
		}
	}else if(strncasecmp(cmd_type, "keepalive", strlen("keepalive"))==0){
		INFO_MSG("msg type is keepalive......");
		if(check_pin(pin, addr)){
			ERROR_MSG("when keepalive, pin not correct");
			sprintf(smsg, "cmd_feedback:pin numer error");
			return -1;
		}
		DEBUG_MSG("keepalive msg deal ok");
		sprintf(smsg, "cmd_feedback:keepalive ok");
	}else if(strncasecmp(cmd_type, "setprop", strlen("setprop"))==0){
		INFO_MSG("msg type is setprop......");
		if(check_pin(pin, addr)){
			ERROR_MSG("when setprop, pin not correct");
			sprintf(smsg, "cmd_feedback:pin numer error");
			return -1;
		}
		if(cmd_setprop(smsg, msg, lenmsg)){
			sprintf(smsg, "cmd_feedback:setprop received error");
			return -1;
		}
		else{
			sprintf(smsg, "cmd_feedback:setprop received ok");
		}
	}
	else if(strncasecmp(cmd_type, "init", strlen("init"))==0){
		if(check_pin(pin, addr)){
			ERROR_MSG("when init, pin not correct");
			sprintf(smsg, "cmd_feedback:pin numer error");
			return -1;
		}
		INFO_MSG("msg type is init......");
		memset(file_content, 0, sizeof(char)*CONFIG_FILE_SIZE);
		if(init_event(file_content)){
			sprintf(smsg, "cmd_feedback:init file not exist");
			return -1;
		}
		DEBUG_MSG("file content:= %s", file_content);
		expand_buffer = 1;

	}
	else if(strncasecmp(cmd_type, "getprop", strlen("getprop"))==0){
		if(check_pin(pin, addr)){
			ERROR_MSG("when getprop, pin not correct");
			sprintf(smsg, "cmd_feedback:pin numer error");
			return -1;
		}
		INFO_MSG("msg type is getprop......");
		if(cmd_getprop(smsg, msg, lenmsg))
			return -1;
	}
	else if(strncasecmp(cmd_type, "action", strlen("action"))==0){
		if(check_pin(pin, addr)){
			ERROR_MSG("when action, pin not correct");
			sprintf(smsg, "cmd_feedback:pin numer error");
			return -1;
		}
		INFO_MSG("msg type is action......");
		if(action_event(smsg, msg ,lenmsg))
			return -1;
	} else if (strncasecmp(cmd_type, "getfilelist", strlen("getfilelist")) == 0) {
        if(cmd_getfilelist(smsg, addr)) {
            return -1;
        }
    } else{
		ERROR_MSG("wrong command type.........\n");
		sprintf(smsg, "cmd_feedback:cmd do not fit");
		return -1;
	}
	return 0;
}


static void* process_info(void *args)
{
	DEBUG_MSG("in process_info");
	struct mypara *para1;
	para1 = (struct mypara *)args;
	int s = (*para1).fd;
	int port = (*para1).port;
	char *addr_char = NULL;
	addr_char = (*para1).addr;

	DEBUG_MSG("in process, fd = %d, addr = %s, port = %d", s, addr_char, port);
	int recv_num;
	int send_num;
	int send_length;
	char recv_buf[BUFFER_SIZE];
	char deal_buf[BUFFER_SIZE];

	INFO_MSG("begin recv:");
	memset(recv_buf, 0, sizeof(char)*BUFFER_SIZE);
	memset(deal_buf, 0, sizeof(char)*BUFFER_SIZE);
	recv_num = read(s,recv_buf,sizeof(recv_buf));
	if(recv_num <0){
		perror("recv");
		exit(1);
	} else {
		recv_buf[recv_num] = '\0';
		DEBUG_MSG("recv sucessful:\n%s",recv_buf);
	}

	memcpy(deal_buf, recv_buf, recv_num);

	if(msg_deal(deal_buf, recv_num, addr_char, recv_buf, s, port))
	{
		ERROR_MSG("msg_deal error");
	}

	if(expand_buffer)
	{
		char recv_large_buf[CONFIG_FILE_SIZE];
		sprintf(recv_large_buf, recv_buf);
		strcat(recv_large_buf,file_content);
		strcat(recv_large_buf, "\r\nend");
		INFO_MSG("begin send");
		send_length = strlen(recv_large_buf);

		send_num = write(s,recv_large_buf, send_length);
		DEBUG_MSG("send_num = %d",send_num);
		expand_buffer = 0;
	}
	else{
		strcat(recv_buf, deal_buf);
		strcat(recv_buf, "\r\nend");

		DEBUG_MSG("begin send");
		//printf("recv_buf = %s strlen = %d\n", recv_buf, strlen(recv_buf));
		send_length = strlen(recv_buf);

		send_num = write(s,recv_buf,send_length);
		DEBUG_MSG("send_num = %d",send_num);
	}
	if (send_num < 0){
		perror("send");
		exit(1);
	} else {
		INFO_MSG("msg sendback sucess");
	}
	close(s);
	
	if(setprop && (cmd_num > 0))
	{
		//if(notify_app_value_updated(keyarr, valuearr, cmd_num, 1))
		//	printf("msg send error\n");
		INFO_MSG("before send ui\n");
		int ret = send_message_to_gui(MSG_VALUE_UPDATED, keyarr, valuearr, cmd_num);
	        INFO_MSG("after send ui\n");
        	if(ret != 0){
			ERROR_MSG("setprop send_message_to_gui error");
		}

		int i;
		for(i=0;i<cmd_num;i++)
		{
			free(keyarr[i]);
			free(valuearr[i]);
		}
		setprop = 0;
		cmd_num = 0;
	}
	pthread_exit(0);  
}

static void check_state()
{
    char recv_buf[BUFFER_SIZE];
	char deal_buf[BUFFER_SIZE];
	while(1)
	{
		sleep(1);
		int k;
		int ret;
		ret=pthread_mutex_trylock(&mutex);/*\u5224\u65ad\u4e0a\u9501*/
		if(ret != EBUSY){
			for(k=0;k<10;k++)
			{
				if(strlen(a[k].pin) != 0 && a[k].state != 0 ){
					a[k].state -= 1;
					DEBUG_MSG("a[%d] : %s %s %d", k, a[k].pin, a[k].addr, a[k].state);
					if((a[k].state==1)&&(wifiState==WIFISTA))
					{
					   INFO_MSG("sta connect timeout change to ap\n");
					   memset(deal_buf,0, sizeof(char)*BUFFER_SIZE);
					   memset(recv_buf,0, sizeof(char)*BUFFER_SIZE);
					   //SpvGetWifiSSID(recv_buf);
                       char ssid[128] = {0}, pswd[128] = {0};
                       GetWifiApInfo(ssid, pswd);
					   snprintf(deal_buf,BUFFER_SIZE,"AP;;%s;;%s;;WPA-PSK;;", ssid, pswd);
					   cmd_wifi(recv_buf,deal_buf);
					}
				}
				if(strlen(a[k].pin) != 0 && a[k].state == 0 ){
					memset(a[k].pin, 0, sizeof(char)*20);
					memset(a[k].addr, 0, sizeof(char)*20);

					char buf[100];
					sprintf(buf, "%s\t%s\r\n", a[k].pin, a[k].addr);

					FILE *fp;
					if((fp = fopen(ALIVE_PIN_FILE,"w+")) == NULL)
					{
						ERROR_MSG("when check_state, open ALIVE_PIN_FILE error");
					}
					fwrite(buf, strlen(buf), 1, fp);
					fclose(fp);
				}
			}
			pthread_mutex_unlock(&mutex);/*\u89e3\u9501*/
		}
	}

}

static void init_my_server()
{
    INFO_MSG("init_my_server\n");
	if(access(ALIVE_PIN_FILE,0) == 0)
		unlink(ALIVE_PIN_FILE);
	
	int i;
	for(i=0;i<10;i++)
	{
		memset(a[i].pin, 0, sizeof(char)*20);
		memset(a[i].addr, 0, sizeof(char)*20);
		a[i].state = 0;
	}


	FILE *fp;
	if((fp=fopen(IPADDRFILE,"a+"))==NULL) 
	{ 
		ERROR_MSG("init_my_server open IPADDRFILE error"); 
		getchar(); 
		return;
	}
	fclose(fp);

	INFO_MSG("Welcome! This is a SPV_SERVER, I can only received message from client and reply with the dealed message");


	int servfd, clifd, on, ret;
	struct sockaddr_in servaddr, cliaddr;
	if ((servfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		ERROR_MSG("create socket error!");
		return;
	}

	on = 1;
	ret = setsockopt( servfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	while((bind(servfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0))
	{
		ERROR_MSG("bind to port %d failure!",SERVER_PORT);
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

	if (listen(servfd,LENGTH_OF_LISTEN_QUEUE) < 0)
	{
		ERROR_MSG("call listen failure!");
		return;
	}

	int n, m;
	char addr_char[20];
	pid_t pid;

	pthread_t t1, t2;
	int pthread_ret1, pthread_ret2;
	pthread_mutex_init(&mutex,NULL);
	pthread_ret1 = pthread_create(&t1, NULL, (void *) check_state, NULL);
	//pthread_join(t1,NULL);

     INFO_MSG("init_my_server while\n");
	char pin[6];
	new_pin(pin);
    INFO_MSG("init_my_server inn pin=%s\n",pin);
        pairing_event(pin);
	INFO_MSG("now start to listen:===========================================");
	while (1)
	{
		char addr_char[20];
		int port;
		long timestamp;
		struct mypara para;
		socklen_t length = sizeof(cliaddr);
		clifd = accept(servfd,(struct sockaddr*)&cliaddr,&length);
		if (clifd < 0)
		{
			ERROR_MSG("error comes when call accept!");
			break;
		} 
		sprintf(addr_char,"%s",inet_ntoa(cliaddr.sin_addr));
		port = ntohs(cliaddr.sin_port);
		
		para.fd = clifd;
		para.addr = addr_char;
		para.port = port;
		DEBUG_MSG("fd = %d, addr = %s, port = %d", para.fd, para.addr, para.port);
		pthread_ret2 = pthread_create(&t2, NULL, process_info, &(para));
		pthread_join(t2,NULL);
               
	}
	close(servfd);
	pthread_mutex_destroy(&mutex);
	return;

}

int init_socket_server()
{
	pthread_t socketThread;
	return pthread_create(&socketThread, NULL, (void *) init_my_server, NULL);
}
/*
void main()
{
  init_socket_server();
   while(1)
   {
     usleep(1000*1000*1000);
}
   return;
}*/
