#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/reboot.h>
#include "Deamon_msg.h"

#include <sys/prctl.h>
#include <sys/shm.h>
#include <sys/times.h>
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>
#include <qsdk/demux.h>
#include <qsdk/upgrade.h>
#include <qsdk/sys.h>
#include <qsdk/upgrade.h>

#define WATCHDOG_KEY       201612
#define WATCHDOG_SHM_SIZE  128
#define WATCHDOG_TIMEOUT_SECOND   15000
#define NOTIFY_UPGRADE_COMPLETED_FILE "/tmp/Upgrade_completed.wav"
#define NOTIFY_UPGRADE_FAILED_FILE "/tmp/Upgrade_failed.wav"

static int g_upgradeFlag = 0;

typedef struct tagDEAMON_NODE
{
    unsigned long thread_id;
    unsigned int pid;
    char szPidName[64];
    int  iKillAll;//是否要kill掉所有监听的pid
    struct tagDEAMON_NODE *pNext;
}DEAMON_NODE;

static pthread_mutex_t g_mutex;
DEAMON_NODE *g_pHead = NULL;//链表头指针
static int  g_mid = 0;

void SignalHander(int signal)
{
    static int bExit = 0;
    char name[32];
    if(bExit == 0)
    {
        bExit = 1;
        printf("____________Daemon will exit by signal:%d,pid:%d\n",signal,getpid());
        prctl(PR_GET_NAME, (unsigned long)name);
        printf("____________Daemon exit thread name %s\n",name);
		if(g_upgradeFlag == 0)
		{
			fopen("/config/errflag", "w");
			sync();
		}
        if(signal == SIGSEGV)
        {
            SIG_DFL(signal);
        }
        else
        {
            exit(1);
        }
    }
    
}

void CaptureAllSignal()
{
    int i = 0;
    for(i = 0; i < 32; i ++)
    {
        if ( (i == SIGPIPE) || (i == SIGALRM))
        {           
            signal(i, SIG_IGN);     
        }
        else if(i != SIGCHLD)
        {
            signal(i, SignalHander);
        }
    } 
}


//DMS_DEAMON_MESSAGE g_msg,g_msg_1;
void* unblock_system_func(void *para)
{
	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, (unsigned long)"DaemonUnblock", 0,0,0);
	DMS_DEAMON_MESSAGE msg;
	memset(&msg, 0, sizeof(msg));
    DMS_DEAMON_MESSAGE *pmsg = (DMS_DEAMON_MESSAGE *)para;
    int ret = 0;
    
    if(NULL == pmsg)
    {
        return NULL;
    }
    //printf("Enter unblock_system_func thread!:: %s \n",pmsg->buffer);
    ret = system(pmsg->buffer);

    msg.result = ret;
    strcpy(msg.buffer,pmsg->buffer);
    msg.msg_to = pmsg->msg_fm;
    msg.msg_fm = pmsg->msg_to;
    ret = msgsnd(g_mid, &msg, sizeof(DMS_DEAMON_MESSAGE), IPC_NOWAIT);
    //printf("Exit unblock_system_func thread!:: %s. %d - %d\n",pmsg->buffer,msg.result, ret);
    free(pmsg);
    return NULL;
}
//返回头结点指针,新的结点插入在原头结点前面
DEAMON_NODE *InsertNode(DEAMON_NODE *pHead, long pid,const char *pbuff)
{
    DEAMON_NODE *pNode = NULL;

    pNode = (DEAMON_NODE*)malloc(sizeof(DEAMON_NODE));
    if(NULL == pNode)
    {
        return pHead;
    }
    memset(pNode, 0, sizeof(DEAMON_NODE));
    pNode->thread_id = (unsigned long)pid;
    memcpy(&pNode->pid, pbuff, sizeof(unsigned int));
    pNode->iKillAll = 0;
    strncpy(pNode->szPidName, pbuff+sizeof(unsigned int),sizeof(pNode->szPidName)-1);
    pthread_mutex_lock(&g_mutex);
    pNode->pNext = pHead;
    pthread_mutex_unlock(&g_mutex);

    printf("Insert a new node(%u--%lu--%s) success!\n", pNode->pid,(unsigned long)pid, pNode->szPidName);
    return pNode;
}
//返回值:0-成功更新结点数据，1:没有值为pid的结点，其它值:失败
int UpDateNode(DEAMON_NODE *pHead, long pid,const char *pbuff)
{
    DEAMON_NODE *pNode = pHead;
    int iRet = 1;

    pthread_mutex_lock(&g_mutex);
    while(NULL != pNode)
    {
        if(pNode->thread_id == (unsigned long)pid)
        {
            pNode->iKillAll = 0;
            pthread_mutex_unlock(&g_mutex);
            return 0;
        }
        else
        {
            pNode = pNode->pNext;
        }
    }
    pthread_mutex_unlock(&g_mutex);

    return iRet;
}
int DeleteNode(DEAMON_NODE *pHead, long pid)
{
    DEAMON_NODE *pNode = pHead;
    int iRet = 1;

    pthread_mutex_lock(&g_mutex);
    while(NULL != pNode)
    {
        if(pNode->pid == pid)
        {
            pNode->iKillAll = 0;
            pthread_mutex_unlock(&g_mutex);
            return 0;
        }
        else
        {
            pNode = pNode->pNext;
        }
    }
    pthread_mutex_unlock(&g_mutex);

    return iRet;
}
int CheckList(DEAMON_NODE *pHead)
{
    DEAMON_NODE *pNode = pHead;
    pthread_mutex_lock(&g_mutex);
    while(NULL != pNode)
    {
        if(1 == pNode->iKillAll)
        {
            printf("%s: pNode->pid=%u,pNode->thread_id=%lu,pNode->iKillAll=%d,pNode->szPidName=%s!\n",__FUNCTION__,pNode->pid,pNode->thread_id,pNode->iKillAll,pNode->szPidName);
            pthread_mutex_unlock(&g_mutex);
            return 1;
        }
        else
        {
            pNode = pNode->pNext;
        }
    }
    pthread_mutex_unlock(&g_mutex);
    return 0;
}
//将所有结点置为退出标记
void SetList(DEAMON_NODE *pHead)
{
    DEAMON_NODE *pNode = pHead;
    pthread_mutex_lock(&g_mutex);
    while(NULL != pNode)
    {
        if(0 == pNode->iKillAll)
        {
            pNode->iKillAll = 1;
        }
        pNode = pNode->pNext;
    }
    pthread_mutex_unlock(&g_mutex);
}
//返回头结点指针
DEAMON_NODE *UpdateList(DEAMON_NODE *pHead, long pid,const char *pbuff)
{
    int iRet = UpDateNode(pHead, pid,pbuff);

    if(iRet == 1)//没有值为pid的结点
    {
        return InsertNode(pHead, pid,pbuff);
    }
    return pHead;
}
//销毁链表
void DestroyList(DEAMON_NODE *pHead)
{
    DEAMON_NODE *pNode = pHead;
    pthread_mutex_lock(&g_mutex);
    while(NULL != pNode)
    {
        DEAMON_NODE *p1 = pNode;
        pNode = pNode->pNext;

        free(p1);
        p1 = NULL;
    }
    pthread_mutex_unlock(&g_mutex);
}
//kill掉所有监控的线程pid
void KillAllPid(DEAMON_NODE *pHead)
{
    DEAMON_NODE *pNode = pHead;
    pthread_mutex_lock(&g_mutex);
    while(NULL != pNode)
    {
        char szCmd[24] = {0};
        
        sprintf(szCmd, "kill -9 %d", pNode->pid);
        printf("%s: %s,pNode->szPidName=%s!\n",__FUNCTION__, szCmd,pNode->szPidName);
        system(szCmd);
        
        pNode = pNode->pNext;
    }
    pthread_mutex_unlock(&g_mutex);
}
void* Deal_Heart_Thread(void *para)
{
    int iRet = 0;
    DMS_DEAMON_MESSAGE msg;

    g_pHead = NULL;
    while(1)
    {
        if((iRet = msgrcv(g_mid, &msg, sizeof(msg), MSG_HEART, MSG_NOERROR)) == -1)
        {
            perror("Deal_Heart_Thread Recv MSG_HEART failure!\n");
			if(EINTR != errno)
			{
				perror("MSG_HEART: msgrcv");
            	exit(1);
			}
        }
        else if (iRet == 0)
        {
            printf("Deal_Heart_Thread msgrcv finished!\n");
            //break;
        }
        else
        {
            g_pHead = UpdateList(g_pHead, msg.msg_fm,msg.buffer);
        }
    }
    DestroyList(g_pHead);
    g_pHead = NULL;
    return NULL;
}
void *Check_Thread(void *para)
{
    struct timeval TimeoutVal; 
    while(1)
    {
        //四分钟检查一次--时间搞长一点，避免升级过程中http无心跳信息导致的未升级完成就重启的问题
        TimeoutVal.tv_sec = 4*60;
        TimeoutVal.tv_usec = 0;

        select(0, NULL, NULL, NULL, &TimeoutVal);

        if(CheckList(g_pHead))
        {
            KillAllPid(g_pHead);
            break;
        }
        SetList(g_pHead);
    }
    return NULL;
}

#define AUDIO_BUF_SIZE			1024
void reboot_system(void)
{
	printf("system reboot now!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	sync();
    reboot(RB_AUTOBOOT);
}

static int PlayAudio(char *pFileName)
{
	struct demux_frame_t demux_frame = {0};
	struct demux_t *demux = NULL;
	struct codec_info play_info;
	struct codec_addr play_addr;
	int play_handle = -1, ret, len, frame_size, offset, length = 0;
	char play_buf[AUDIO_BUF_SIZE * 16 * 6];
	void *codec = NULL;
	audio_fmt_t play_fmt = {.channels=2, .bitwidth=32, .samplingrate=16000, .sample_size=1024};

	frame_size = play_fmt.sample_size;
	demux = demux_init(pFileName);
	if (demux) {
		do {
			ret = demux_get_frame(demux, &demux_frame);
			if (((ret < 0) || (demux_frame.data_size == 0)) && (play_handle >= 0)) {
				printf("demux_get_frame return\n");
				break;
			}

			if (play_handle < 0) {
				printf("audio play %s %d %d %d \n", pFileName, play_fmt.channels, play_fmt.samplingrate, play_fmt.bitwidth);
				frame_size = play_fmt.sample_size;
				if (demux_frame.bitwidth == 16)
					frame_size = play_fmt.sample_size * 2;
				if (play_fmt.channels > demux_frame.channels)
					frame_size *= (play_fmt.channels / demux_frame.channels);
				if (play_fmt.samplingrate > demux_frame.sample_rate)
					frame_size *= (play_fmt.samplingrate / demux_frame.sample_rate);

				play_handle = audio_get_channel("default", &play_fmt, CHANNEL_BACKGROUND);
				if (play_handle < 0) {
					printf("audio play get default channel failed %d \n", play_handle);
					break;
				}
			}

			if (!codec) {
				if (demux_frame.codec_id == DEMUX_AUDIO_PCM_ALAW)
					play_info.in.codec_type = AUDIO_CODEC_G711A;
				else
					play_info.in.codec_type = AUDIO_CODEC_PCM;

				play_info.in.channel = demux_frame.channels;
				play_info.in.sample_rate = demux_frame.sample_rate;
				play_info.in.bitwidth = demux_frame.bitwidth;
				play_info.in.bit_rate = play_info.in.channel * play_info.in.sample_rate \
										* GET_BITWIDTH(play_info.in.bitwidth);
				play_info.out.codec_type = AUDIO_CODEC_PCM;
				play_info.out.channel = play_fmt.channels;
				play_info.out.sample_rate = play_fmt.samplingrate;
				play_info.out.bitwidth = play_fmt.bitwidth;
				play_info.out.bit_rate = play_info.out.channel * play_info.out.sample_rate \
										* GET_BITWIDTH(play_info.out.bitwidth);

				codec = codec_open(&play_info);
				if (!codec) {
					printf("audio play open codec decoder failed \n");
					break;
				}
			}
			memset(play_buf, 0, AUDIO_BUF_SIZE * 16 * 6);
			play_addr.in = demux_frame.data;
			play_addr.len_in = demux_frame.data_size;
			play_addr.out = play_buf;
			play_addr.len_out = AUDIO_BUF_SIZE * 16 * 6;

			ret = codec_decode(codec, &play_addr);

			demux_put_frame(demux, &demux_frame);
			if (ret < 0) {
				printf("audio play codec decoder failed %d \n",	ret);
				break;
			}
			len = ret;
			if (len < frame_size)
				len = frame_size;
			offset = 0;
			do {
				ret = audio_write_frame(play_handle, play_buf + offset, frame_size);
				if (ret < 0)
					printf("audio play codec failed %d \n",	ret);
				offset += frame_size;
			} while (offset < len);

			usleep(30000);
		} while (ret >= 0);
		do {
			ret = codec_flush(codec, &play_addr, &length);
			if (ret == CODEC_FLUSH_ERR)
				break;

			/* TODO you need least data or not ?*/
		} while (ret == CODEC_FLUSH_AGAIN);
		codec_close(codec);
		codec = NULL;
		if (play_handle >= 0) {
			usleep(150000);
			audio_put_channel(play_handle);
		}
		demux_deinit(demux);
	}

	return 0;
}


static void upgrade_state_callback(void *arg, int image_num, int state, int state_arg)
{
	struct upgrade_t *upgrade = (struct upgrade_t *)arg;
	int cost_time;

	switch (state) {
		case UPGRADE_START:
			gettimeofday(&upgrade->start_time, NULL);
			printf("q3 upgrade start \n");
			if (system_update_check_flag() == UPGRADE_IUS)
				system_update_clear_flag_all();
			break;
		case UPGRADE_COMPLETE:
			gettimeofday(&upgrade->end_time, NULL);
			cost_time = (upgrade->end_time.tv_sec - upgrade->start_time.tv_sec);
			if (system_update_check_flag() == UPGRADE_OTA)
				system_update_clear_flag(UPGRADE_FLAGS_OTA);
	        printf("\n q3 system upgrade sucess time is %ds \n", cost_time);
			PlayAudio(NOTIFY_UPGRADE_COMPLETED_FILE);
            printf("system upgrade complete.........");
			reboot_system();
			break;
		case UPGRADE_START_WRITE:
			system_update_clear_flag(image_num);
			break;
		case UPGRADE_WRITE_STEP:
			printf("\r q3 %s image -- %d %% complete", image_name[image_num], state_arg);
			fflush(stdout);
			break;
		case UPGRADE_VERIFY_STEP:
			if (state_arg)
				printf("\n q3 %s verify failed %x \n", image_name[image_num], state_arg);
			else {
				system_update_set_flag(image_num);
				printf("\n q3 %s verify success\n", image_name[image_num]);
			}
			break;
		case UPGRADE_FAILED:
			printf("q3 upgrade %s failed %d \n", image_name[image_num], state_arg);
			PlayAudio(NOTIFY_UPGRADE_FAILED_FILE);
			reboot_system();
			break;
	}

	return;
}

#if 0
void* watchdog_thread(void *arg)
{
	int ret = 0;
	int mid = -1;
	int *pAppFlag = NULL;
	int appValueBackup = 0;
	int debugMode = 0;
	prctl(PR_SET_NAME, (unsigned long)"DaemonWatchdog", 0,0,0);

	while(mid<0)
    {
        sleep(1);
        mid = shmget(WATCHDOG_KEY, 0, 0);
    }

    char *pMem = (char *)shmat(mid, 0, 0);
    pAppFlag = (int *)pMem;

	ret = system_enable_watchdog();
	if(ret)
	{
		printf("%s  %d, enable watch dog failed!!\n",__FUNCTION__, __LINE__);
		return (void *)0;
	}

	ret = system_set_watchdog(WATCHDOG_TIMEOUT_SECOND);
	if(ret)
	{
		printf("%s  %d, set watch dog failed!!\n",__FUNCTION__, __LINE__);
		return (void *)0;
	}

	int upgradeCnt = 0;
	while(1)
	{
		appValueBackup = *pAppFlag;
		
		time_t timep;
		time (&timep);
		printf("feed dog(%s)\n", ctime(&timep));
		system_feed_watchdog();
		sleep(4);
		if(g_upgradeFlag == 1)
		{
			if(upgradeCnt++>30)
			{
				reboot_system();
				break;
			}
		}
		
		if(debugMode == 0)
		{
			if(access("/mnt/mmc1/debugmode", F_OK)==0)
			{
				debugMode = 1;
				continue;
			}
			
			if(g_upgradeFlag ==0 && appValueBackup == *pAppFlag)
			{
				printf("mainApp have no response!watchdog exit!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				break;
			}
		}
	}
	printf("FUNC:%s(%d), End.\n",__FUNCTION__, __LINE__);

	return (void *)0;
}
#endif

void* watchdog_thread(void *arg)
{
	int ret = 0;
	int NotFound = 0;
	int debugMode = 0;
	prctl(PR_SET_NAME, (unsigned long)"DaemonWatchdog", 0,0,0);

	char *pCmd = "ps | grep mainApp | grep -v grep 1>/dev/null";

	while(1)
    {
        sleep(1);
		if(system(pCmd) == 0)
			break;
    }

	printf("FUNC:%s(%d), watchdog start!\n",__FUNCTION__, __LINE__);

	ret = system_enable_watchdog();
	if(ret)
	{
		printf("%s  %d, enable watch dog failed!!\n",__FUNCTION__, __LINE__);
		return (void *)0;
	}

	ret = system_set_watchdog(WATCHDOG_TIMEOUT_SECOND);
	if(ret)
	{
		printf("%s  %d, set watch dog failed!!\n",__FUNCTION__, __LINE__);
		return (void *)0;
	}

	int upgradeCnt = 0;
	while(1)
	{
		
		time_t timep;
		time (&timep);
		//printf("feed dog(%s)\n", ctime(&timep));
		system_feed_watchdog();
		sleep(4);
		if(g_upgradeFlag == 1)
		{
			if(upgradeCnt++>40)
			{
				reboot_system();
				break;
			}
		}
		
		if(debugMode == 0)
		{
			if(access("/mnt/mmc1/debugmode", F_OK)==0)
			{
				debugMode = 1;
				continue;
			}
			
			if(g_upgradeFlag ==0 && system(pCmd) != 0)
			{
				NotFound++;
			}
			else
			{
				NotFound = 0;
			}

			if(NotFound > 2)
			{
				printf("mainApp have no response!watchdog exit!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				break;
			}
		}
	}
	printf("FUNC:%s(%d), End.\n",__FUNCTION__, __LINE__);

	return (void *)0;
}


void upgrade_system(char *pFilePath)
{
	//system("killall mainApp");
	prctl(PR_SET_NAME, (unsigned long)"DaemonUpgrade", 0,0,0);
	sleep(1);
	if(access(pFilePath, F_OK)!=0)
	{
		printf("upgrade file:%s not exist!!!!!!!!!!!!\n", pFilePath);
		reboot_system();
	}

	system_update_upgrade(pFilePath, upgrade_state_callback, NULL);

	while(1)
	{
		sleep(1);
	}
	exit(0);;
}

int main(void)
{
    key_t key;
    pid_t cli_pid;
    int  n = 0, ret = 0;
    DMS_DEAMON_MESSAGE msg, msg_s;
    cli_pid = getpid();

	prctl(PR_SET_NAME, (unsigned long)"DaemonMain", 0,0,0);

    if((key = ftok("/etc/systemmsg", SEED)) == -1)
    {
        perror("Server:key generation");
        exit(1);
    }

    /*Create Messg queue*/
    if((g_mid = msgget(key, IPC_CREAT | 0666)) == -1)
    {
        exit(1);
    }

	CaptureAllSignal();

	pthread_t tid;
	pthread_create(&tid, NULL, watchdog_thread, NULL);

    msg_s.msg_to = CLIENT;//long int ,value is 1
    msg_s.msg_fm = cli_pid;
    memset(msg_s.buffer, 0x0, DEAMON_BUFFERSIZE);
    
    //g_msg_1.msg_to = CLIENT;
    //g_msg_1.msg_fm = cli_pid;

    pthread_mutex_init(&g_mutex,NULL);
    //start heart thread
    //{
        //pthread_t tid = 0, tid2 = 0;
        //pthread_t tid2 = 0
        //pthread_create(&tid2, NULL, Check_Thread, NULL);
    //}
    //parent process
    while(1)
    {
        //if((n = msgrcv(mid, &msg, sizeof(msg), SERVER, 0)) == -1)
			if((n = msgrcv(g_mid, &msg, sizeof(DMS_DEAMON_MESSAGE), SERVER, MSG_NOERROR)) == -1)
			{
				if(EINTR != errno)
				{
					perror("SERVER: msgrcv");
					exit(2);
				}
			}
			else if (n == 0)
			{
				printf("#### msgrcv finished\n");
				//break;
			}
			else
			{
				char *p = strstr(msg.buffer, "upgrade_file:");
				if(p)
				{
					msg_s.result = 0;
					msg_s.msg_to = msg.msg_fm;
					msg_s.msg_fm = msg.msg_to;
					msgsnd(g_mid, &msg_s, sizeof(DMS_DEAMON_MESSAGE), IPC_NOWAIT);
					g_upgradeFlag = 1;
					p += strlen("upgrade_file:");
					upgrade_system(p);
				}

				//printf("#### SERVER %d. msgrcv:%s, msg.msg_flag:%d count:%u. \n", n,msg.buffer,msg.msg_flag, msgcount++);
				if(msg.msg_flag == SYSTEM_MSG_NOBLOCK_RUN)//非阻塞
				{
					pthread_t tid = 0;
					DMS_DEAMON_MESSAGE *pMsg = (DMS_DEAMON_MESSAGE *)malloc(sizeof(DMS_DEAMON_MESSAGE));
					if (pMsg)
					{
						memcpy(pMsg, &msg, sizeof(DMS_DEAMON_MESSAGE));
						if (pthread_create(&tid, NULL, unblock_system_func, pMsg))
						{
							perror("deamon pthread_create ");
							free(pMsg);
						}
					}
					else
					{
						perror("malloc error...");
					}
					continue;
				}
				else
				{
					ret = 0;
					if(0 < strlen(msg.buffer))
					{
						//printf("deamon :: %s. \n",msg.buffer);
						ret = system(msg.buffer);
						//printf("deamon :: %s. ret:%d.\n",msg.buffer,ret);
					}

					if (ret == 0)
					{
						msg_s.result = 0;
					}
					else
					{
						msg_s.result = ret;
					}
					msg_s.msg_to = msg.msg_fm;
					msg_s.msg_fm = msg.msg_to;

					memset(msg_s.buffer, 0x00, DEAMON_BUFFERSIZE);
					strcpy(msg_s.buffer, msg.buffer);

					//printf("#### SERVER msgsnd msg_to:%d, result:%d\n", msg_s.msg_to, msg_s.result);
					//if(msgsnd(mid, &msg_s, sizeof(msg_s), 0) == -1)
					if(msgsnd(g_mid, &msg_s, sizeof(DMS_DEAMON_MESSAGE), IPC_NOWAIT) == -1)
					{
						if(EINTR != errno)
						{
							printf("SERVER: msgsend");
							exit(5);
						}
					}
				}
			}
		}

		//delete msg queue
		msgsnd(g_mid, &msg, 0, 0);
		pthread_mutex_destroy(&g_mutex);
		exit(0);
}






