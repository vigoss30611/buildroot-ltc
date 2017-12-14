#include <stdio.h>


#define SEED 'g'
#define CLIENT	    1L
#define SERVER	    2L
#define MSG_HEART   3L
#define DEAMON_BUFFERSIZE 1024

#define SYSTEM_MSG_BLOCK_RUN 0//阻塞
#define SYSTEM_MSG_NOBLOCK_RUN 1//非阻塞

typedef struct
{
	long msg_to;
	long msg_fm;
	int	 msg_no;						//包序号，Deamon应答的时候原值返回
	int  msg_flag;                      //是否阻塞运行的标记
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

int ipc_msg_get_mid();
int ipc_msg_snd(int mid, char *snd_cmd);



