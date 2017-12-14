#ifndef __IPC_MSG_H__
#define __IPC_MSG_H__

#include <stdio.h>


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */




#define SEED 'g'
#define SYSTEM_MSG_CLIENT	1L
#define SYSTEM_MSG_SERVER	2L
#define SYSTEM_MSG_HEART    3L
#define DEAMON_BUFFERSIZE 1024

#define SYSTEM_MSG_BLOCK_RUN 0//阻塞
#define SYSTEM_MSG_NOBLOCK_RUN 1//非阻塞

typedef struct
{
	long msg_to;
	long msg_fm;
	int	msg_no;						//包序号，Deamon应答的时候原值返回
	int  	msg_flag;                      //是否阻塞运行的标记
	char buffer[DEAMON_BUFFERSIZE];		//Deamon应答的时候原值返回
	int	 result;						//0:表示成功，其他值表示失败
}DMS_DEAMON_MESSAGE;
typedef struct
{
	long msg_to;
	long msg_fm;
	int	msg_no;						//包序号，Deamon应答的时候原值返回
	int msg_flag;                      //是否阻塞运行的标记
	int	 result;						//0:表示成功，其他值表示失败
}DMS_DEAMON_HEART;


int system_msg_init();

int system_msg_snd(int mid, char *snd_cmd, int msg_flag, int msg_no, long msg_fm, long msg_to);

int system_msg_rcv(int mid,long msg_fm);

int system_msg_deinit();

int  SystemCall_msg(char *pmsg,int msg_flag);
int SystemCall_heart(const char *pbuff);
int SystemCall(char *pmsg);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif


