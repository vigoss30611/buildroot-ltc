/***********************************************

************************************************/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/msg.h>
#include <pthread.h>
#include "system_msg.h"
#include <sys/types.h>
#include <sys/syscall.h> 

static pthread_mutex_t g_mutex;
static int g_init = 0;

pid_t gettid() //获取线程ID
{
     return syscall(SYS_gettid);  
}
int system_msg_init()
{
	key_t key;
	int  mid;

	if((key = ftok("/etc/systemmsg", SEED)) == -1)
	{
		perror("#### SYSTEM_MSG_CLIENT:key generation");
		return -1;
	}

	/*Create Messg queue*/
	if((mid = msgget(key, IPC_CREAT | 0666)) == -1)
	{
		printf("#### SYSTEM_MSG_CLIENT: create mes err:%d %s\n", errno, strerror(errno));
		return -2;
	}

	return mid;
}
int system_msg_snd(int mid, char *snd_cmd, int msg_flag, int msg_no, long msg_fm, long msg_to)
{
	DMS_DEAMON_MESSAGE msg;

	if(NULL == snd_cmd)
	{
		return -1;
	}
    if(msg_flag != SYSTEM_MSG_BLOCK_RUN && msg_flag != SYSTEM_MSG_NOBLOCK_RUN)
    {
        msg_flag = 0;
    }
	//注意:这里msg_to必须是SYSTEM_MSG_SERVER,因为Daemon接收只接收SYSTEM_MSG_SERVER的信息
	msg.msg_to = msg_to;//long int ,value is 1
	msg.msg_fm = msg_fm;
    msg.msg_flag = msg_flag;
    msg.msg_no = msg_no;
	
	memset(msg.buffer, 0x0, DEAMON_BUFFERSIZE);
	memcpy(msg.buffer, snd_cmd, strlen(snd_cmd));

    //printf("system_msg_snd-msg.msg_fm=%ld!\n", msg.msg_fm);
	if(msgsnd(mid, &msg, sizeof(DMS_DEAMON_MESSAGE), IPC_NOWAIT) == -1)
	{
		printf("#### SYSTEM_MSG_CLIENT: msgsend err:%d %s\n", errno, strerror(errno));
		return -1;
	}

	return 0;
}

static int system_msg_snd_ex(int mid, char *snd_cmd, int msg_flag, int msg_no, long msg_fm, long msg_to)
{
	DMS_DEAMON_MESSAGE msg;
	pid_t cli_pid = gettid();

	if(NULL == snd_cmd)
	{
		return -1;
	}
    if(msg_flag != SYSTEM_MSG_BLOCK_RUN && msg_flag != SYSTEM_MSG_NOBLOCK_RUN)
    {
        msg_flag = 0;
    }
	//注意:这里msg_to必须是SYSTEM_MSG_SERVER,因为Daemon接收只接收SYSTEM_MSG_SERVER的信息
	msg.msg_to = msg_to;//long int ,value is 1
	msg.msg_fm = msg_fm;
    msg.msg_flag = msg_flag;
    msg.msg_no = msg_no;
	
	memset(msg.buffer, 0x0, DEAMON_BUFFERSIZE);
	memcpy(msg.buffer, &cli_pid, sizeof(pid_t));
	memcpy(msg.buffer+sizeof(pid_t), snd_cmd, strlen(snd_cmd));

    //printf("system_msg_snd-msg.msg_fm=%ld!\n", msg.msg_fm);
	if(msgsnd(mid, &msg, sizeof(DMS_DEAMON_MESSAGE), IPC_NOWAIT) == -1)
	{
		printf("#### SYSTEM_MSG_CLIENT: msgsend err:%d %s\n", errno, strerror(errno));
		return -1;
	}

	return 0;
}

/* 用于接收消息响应! */
int system_msg_rcv(int mid,long msg_fm)
{
	DMS_DEAMON_MESSAGE msg;
	int ret;
    //注意:msg_fm必须与接收的msg中msg_to一样。
    ret = msgrcv(mid, &msg, sizeof(DMS_DEAMON_MESSAGE), msg_fm, MSG_NOERROR);
	if(ret == -1)
	{
		printf("#### SYSTEM_MSG_CLIENT: msgrcv err:%d %s\n", errno, strerror(errno));
		return -1;
	}
	ret = msg.result;
	//printf("#### SYSTEM_MSG_CLIENT msgrcv result:%d, buffer:%s\n", ret, msg.buffer);

	return ret;
}

int system_msg_deinit()
{
	/** 一般情况下不调用此接口**/
	return 0;
}

int  SystemCall_msg(char *pmsg,int msg_flag)
{
    pid_t cli_pid;
   // static unsigned int count = 0;
    if (pmsg == NULL)
    {
        return -1;
    }

    int nret = -1;
    
    int mid = system_msg_init();
    if (mid < 0)
    {
        return -1;
    }
    cli_pid = gettid();
    //cli_pid = SYSTEM_MSG_CLIENT;
    //printf("SystemCall_msg cli_pid %d,cmd:%s\n" , gettid(),pmsg);
    //printf("SystemCall_msg%d: %s.\n",count,pmsg);
    nret = system_msg_snd(mid,pmsg,msg_flag,0,cli_pid, SYSTEM_MSG_SERVER);

    if(0 == nret)
    {
	    nret = system_msg_rcv(mid,cli_pid);
    }
    else
    {
        nret = -1;
    }
   // printf("SystemCall_msg%d: %d.\n",count,nret);
   // count++;
  return nret;
}

int SystemCall_heart(const char *pbuff)
{
    return 0;
    int iRet = -1;
    char *pXXX = (char*)pbuff;
    //pid_t cli_pid = getpid();
    int mid = system_msg_init();
    pthread_t cli_pid = gettid();
    if (mid < 0)
    {
        return -1;
    }

    if(!g_init)
    {
        pthread_mutex_init(&g_mutex,NULL);
        g_init = 1;
    }

    pthread_mutex_lock(&g_mutex);
    //iRet = system_msg_snd(mid,(char*)pbuff,SYSTEM_MSG_BLOCK_RUN,0,cli_pid, SYSTEM_MSG_HEART);
    iRet = system_msg_snd_ex(mid,pXXX,SYSTEM_MSG_BLOCK_RUN,0,cli_pid, SYSTEM_MSG_HEART);
    pthread_mutex_unlock(&g_mutex);
    return iRet;
}


