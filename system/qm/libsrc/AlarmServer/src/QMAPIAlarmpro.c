/******************************************************************************
******************************************************************************/

#include "QMAPIAlarmpro.h"
#include <qsdk/event.h>

#include <semaphore.h>
#include <time.h>
#include <sys/prctl.h>
#include <errno.h>
#include "tl_common.h"
//#include "tlsmtp.h"

#include "systime.h"
#include "QMAPI.h"
#include "librecord.h"
#include "q3_gpio.h"

//int TL_Video_Motion_Alarm_Set(char *Buffer );

//extern int tlx_producer_send(int hSock, void *data, int len);
//extern BOOL IsSupportRecord();
//extern int TL_Video_Picture_SnapShot(MSG_HEAD *clientMsg , int SnapShotType);
//extern int SendEmail(DWORD dwCommand, int channelId, char *attachBuf, int size);
//extern void Local_Record_Channel_Alarm(int nChannel);

pthread_mutex_t             g_addAlarmMutex;
sem_t                   g_NVSalarmSem;

TL_VIDEO_ALARM_QUEUE    g_TL_Server_Message;
TL_VIDEO_IO_ALARM_DYNAMIC_MANAGE	g_tl_ioalarm_dync_manage;

static int g_motion_status[QMAPI_MAX_CHANNUM];
static int gMotionDetectFlag = 0;  /* RM#4336: report the motion event only once in 30 seconds.   henry.li    2017/06/14 */
#define RESEND_MOTION_EVENT_TIMER 30  /* RM#4336: report the motion event only once in 30 seconds.   henry.li    2017/06/14 */

extern int qmapi_alarm_run;
extern int qmapi_alarm_count;

void AlarmSleep(unsigned int mSecond)
{
	struct timeval tv;
	tv.tv_sec = mSecond / 1000;
	tv.tv_usec = (mSecond % 1000) * 1000;

	select(0, NULL, NULL, NULL, &tv);
}


///TL Video New Protocol Add Alarm 
int TL_addAlarm(int Alarm_Msg , int channelId, QMAPI_SYSTEMTIME *systime)
{
	pthread_mutex_lock(&g_addAlarmMutex);
	if(g_TL_Server_Message.index>10)
	{
		g_TL_Server_Message.index=0;	
	}
	g_TL_Server_Message.servermsg[g_TL_Server_Message.index].dwMsg=Alarm_Msg;
	g_TL_Server_Message.servermsg[g_TL_Server_Message.index].dwChannel=channelId;
	memcpy(&g_TL_Server_Message.servermsg[g_TL_Server_Message.index].st, systime, sizeof(QMAPI_SYSTEMTIME));
	g_TL_Server_Message.index++;
	pthread_mutex_unlock(&g_addAlarmMutex);
	sem_post(&g_NVSalarmSem);
	return 0;//TL_SUCCESS;
}

///TL Video New Protocol Alarm Queue Inital
///liu Wen Gao 2007-06-08 Write input_status [
int TL_addAlarm_Initial()
{
	memset(&g_TL_Server_Message , 0 , sizeof(TL_VIDEO_ALARM_QUEUE));
	g_TL_Server_Message.index=0;
	return 0;//TL_SUCCESS;
}

///TL Video New Protocol
void TL_Alarm_Thread()
{
    int  i=0;
    TL_SERVER_MSG *msg;

	TL_addAlarm_Initial();
	
    prctl(PR_SET_NAME, (unsigned long)"AlarmMng", 0,0,0);
	pthread_detach(pthread_self());
		
    sem_init(&g_NVSalarmSem, 0, 0);
    pthread_mutex_init(&g_addAlarmMutex, NULL);

    sleep(5);
	
    while (qmapi_alarm_run)
    {
        //printf("[1] Sending Alarm .......................\n");
        sem_wait(&g_NVSalarmSem);
        for(i=0;i<g_TL_Server_Message.index;i++)
        {
            msg = &g_TL_Server_Message.servermsg[i];
            msg->dwDataLen = 0;
#ifdef JIUAN_SUPPORT
            if(g_tl_globel_param.stConsumerInfo.stPrivacyInfo[g_tl_globel_param.stConsumerInfo.bPrivacyMode].bMotdectReportEnable)
                QMapi_sys_send_alarm(msg->dwChannel, msg->dwMsg, (QMAPI_SYSTEMTIME *)&msg->st);
#else
            QMapi_sys_send_alarm(msg->dwChannel, msg->dwMsg, (QMAPI_SYSTEMTIME *)&msg->st);
#endif
        }
        pthread_mutex_lock(&g_addAlarmMutex);
        g_TL_Server_Message.index=0;
        pthread_mutex_unlock(&g_addAlarmMutex);
    }
	qmapi_alarm_count--;
	
    return;
}

static int API_Motion_Alarm_Set_Sensitive(int sensitive)
{
    if (sensitive<0 || sensitive>100)
    {
        printf("%s failed: sensitive(%d) out of range\n", __FUNCTION__, sensitive);
        return -1;
    }

    if (sensitive < 35)
    {
        sensitive = va_move_set_sensitivity("vam", 2);
    }
    else if (sensitive < 70)
    {
        sensitive = va_move_set_sensitivity("vam", 1);
    }
    else
    {
        sensitive = va_move_set_sensitivity("vam", 0);
    }

    if (sensitive<0)
    {
        printf("%s failed: ret=%d\n", __FUNCTION__, sensitive);
    }

    return sensitive;
}

static int API_Motion_Alarm_Set(char *Buffer)
{
	int 				TL_Channel=0;
	int                         i = 0;
	BOOL                    bSelect = FALSE;
	PTL_CHANNEL_MOTION_DETECT	motion_set=NULL;
    int sensitive;

	if(Buffer == NULL)
	{
		//err();
		return -1;//TLERR_INVALID_PARAM;	
	}
	motion_set = (PTL_CHANNEL_MOTION_DETECT)Buffer;

	if(motion_set->dwChannel<0 || motion_set->dwChannel >= QMAPI_MAX_CHANNUM || motion_set->dwSensitive<0 || motion_set->dwSensitive>100)
	{
		//err();
		return -1;//TLERR_INVALID_PARAM;	
	}

	TL_Channel=motion_set->dwChannel;

    sensitive = API_Motion_Alarm_Set_Sensitive(motion_set->dwSensitive);
    if (sensitive < 0)
    {
        //FIXME: should be set config failed;
        //err();
        return -1;//TLERR_GETCONFIG_FAILD;
    }

   	for(i=0; i < 44*36; i++)
	{
		if(motion_set->pbMotionArea[i]>0)
		{
			bSelect=TRUE;
			break;
		}
	}
	
    motion_set->bEnable = bSelect;
    memcpy(&g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel], motion_set, sizeof(TL_CHANNEL_MOTION_DETECT));
    extern int tl_write_Channel_Motion_Detect( void );
    tl_write_Channel_Motion_Detect();
    return 0;//TL_SUCCESS;	
}


int TL_Video_Motion_Alarm_Set(char *Buffer )
{
	pthread_mutex_lock(&g_tl_codec_setting_mutex);
	API_Motion_Alarm_Set(Buffer);
	pthread_mutex_unlock(&g_tl_codec_setting_mutex);	
	return 0;
}

int TL_Video_Motion_Alarm_Initial(int TL_Channel)
{
	int i;
	BOOL bSelect=FALSE;
    int sensitive;

    //FIXME: what should we do if setting failed?
    API_Motion_Alarm_Set_Sensitive(g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel].dwSensitive);

	for(i=0;i<44*36;i++)
	{
		if(g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel].pbMotionArea[i]>0)
		{
			bSelect=TRUE;
			break;
		}	
	}


	return 0;//TL_SUCCESS;	
}

#if 0
int TL_Video_Motion_Alarm_Timer(int TL_Channel )
{
	TL_SCHED_TIME *lpschedTime;
	TL_SCHED_TIME *lpEvenDayTime;
	TL_SCHED_TIME *lpTime;

	int nOpen=FALSE, EvenDayOpen=FALSE, Enable;
	int fCurrenttimes ;
	int nBegin_times , nEnd_times;
	QMAPI_SYSTEMTIME systime;
	
	if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_GET_SYSTIME, 0, &systime, sizeof(QMAPI_SYSTEMTIME)) != QMAPI_SUCCESS)
	{
		err();
	}
     Enable        = &g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel].bEnable;
	lpschedTime   = &g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel].tlScheduleTime[systime.wDayOfWeek];
	lpEvenDayTime = &g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel].tlScheduleTime[7];

	nOpen = lpschedTime->bEnable;
	EvenDayOpen = lpEvenDayTime->bEnable;

	if (EvenDayOpen==FALSE && nOpen==FALSE && Enable==FALSE)
		return 0;
	
	fCurrenttimes = systime.wHour;
	fCurrenttimes *= 60;
	fCurrenttimes += systime.wMinute;

	if (nOpen || Enable)
	{
		lpTime = lpschedTime;

		nBegin_times	= lpTime->BeginTime1.bHour * 60 + lpTime->BeginTime1.bMinute;
		nEnd_times	= lpTime->EndTime1.bHour * 60 + lpTime->EndTime1.bMinute;

		if(fCurrenttimes >= nBegin_times && fCurrenttimes <= nEnd_times)
		{
			return 1;
		}

		nBegin_times	=lpTime->BeginTime2.bHour * 60 + lpTime->BeginTime2.bMinute;
		nEnd_times		=lpTime->EndTime2.bHour * 60 + lpTime->EndTime2.bMinute;

		if(fCurrenttimes >= nBegin_times && fCurrenttimes <= nEnd_times)
		{
			return 1;	
		}
	}

	if(EvenDayOpen)
	{
		lpTime = lpEvenDayTime;

		nBegin_times	= lpTime->BeginTime1.bHour * 60 + lpTime->BeginTime1.bMinute;
		nEnd_times	= lpTime->EndTime1.bHour * 60 + lpTime->EndTime1.bMinute;

		if(fCurrenttimes >= nBegin_times && fCurrenttimes <= nEnd_times)
		{
			return 1;
		}

		nBegin_times	=lpTime->BeginTime2.bHour * 60 + lpTime->BeginTime2.bMinute;
		nEnd_times		=lpTime->EndTime2.bHour * 60 + lpTime->EndTime2.bMinute;

		if(fCurrenttimes >= nBegin_times && fCurrenttimes <= nEnd_times)
		{
			return 1;	
		}
	}

	return 0;
}
#endif

/* RM#4255: check current time is in schedule time or not.   henry.li    2017/06/08 */
int TL_Video_Motion_Alarm_Timer(int TL_Channel )
{
	TL_SCHED_TIME *lpschedTime;
	TL_SCHED_TIME *lpEvenDayTime;
	TL_SCHED_TIME *lpTime;

	int nOpen=FALSE, EvenDayOpen=FALSE, Enable;
	int fCurrenttimes ;
	int nBegin_times , nEnd_times;
	QMAPI_SYSTEMTIME systime;

	if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_GET_SYSTIME, 0, &systime, sizeof(QMAPI_SYSTEMTIME)) != QMAPI_SUCCESS)
	{
		err();
	}
    Enable        = g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel].bEnable;
	lpschedTime   = &g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel].tlScheduleTime[systime.wDayOfWeek];
	lpEvenDayTime = &g_tl_globel_param.TL_Channel_Motion_Detect[TL_Channel].tlScheduleTime[7];

	nOpen = lpschedTime->bEnable;
	EvenDayOpen = lpEvenDayTime->bEnable;

    //printf("[%s] [%d] lpschedTime day is [%d], Enable=[%d], nOpen=[%d], EvenDayOpen=[%d]!\n", __FUNCTION__, __LINE__,
    //        systime.wDayOfWeek, Enable, nOpen, EvenDayOpen);

    if (Enable == FALSE)
    {
        //printf("Motion Alert Enable==FALSE!\n");
        return 0;
    }

    if (EvenDayOpen==FALSE && nOpen==FALSE)
    {
        //printf("EvenDayOpen==FALSE && nOpen==FALSE!\n");
        return 0;
    }

	fCurrenttimes = systime.wHour;
	fCurrenttimes *= 60;
	fCurrenttimes += systime.wMinute;

	if (nOpen)
	{
		lpTime = lpschedTime;

		nBegin_times	= lpTime->BeginTime1.bHour * 60 + lpTime->BeginTime1.bMinute;
		nEnd_times	= lpTime->EndTime1.bHour * 60 + lpTime->EndTime1.bMinute;

        //printf("1: cur=[%d], begin=[%d], end=[%d]\n", fCurrenttimes, nBegin_times, nEnd_times);
		if(fCurrenttimes >= nBegin_times && fCurrenttimes <= nEnd_times)
		{
			return 1;
		}

		nBegin_times	=lpTime->BeginTime2.bHour * 60 + lpTime->BeginTime2.bMinute;
		nEnd_times		=lpTime->EndTime2.bHour * 60 + lpTime->EndTime2.bMinute;

        //printf("2: cur=[%d], begin=[%d], end=[%d]\n", fCurrenttimes, nBegin_times, nEnd_times);
		if(fCurrenttimes >= nBegin_times && fCurrenttimes <= nEnd_times)
		{
			return 1;
		}
	}

	if(EvenDayOpen)
	{
		lpTime = lpEvenDayTime;

		nBegin_times	= lpTime->BeginTime1.bHour * 60 + lpTime->BeginTime1.bMinute;
		nEnd_times	= lpTime->EndTime1.bHour * 60 + lpTime->EndTime1.bMinute;

        //printf("everyDay1: cur=[%d], begin=[%d], end=[%d]\n", fCurrenttimes, nBegin_times, nEnd_times);
		if(fCurrenttimes >= nBegin_times && fCurrenttimes <= nEnd_times)
		{
			return 1;
		}

		nBegin_times	=lpTime->BeginTime2.bHour * 60 + lpTime->BeginTime2.bMinute;
		nEnd_times		=lpTime->EndTime2.bHour * 60 + lpTime->EndTime2.bMinute;

        //printf("everyDay2: cur=[%d], begin=[%d], end=[%d]\n", fCurrenttimes, nBegin_times, nEnd_times);
		if(fCurrenttimes >= nBegin_times && fCurrenttimes <= nEnd_times)
		{
			return 1;
		}
	}

	return 0;
}


int checkScheduleTime(QMAPI_SCHEDTIME *pScheduleTime)
{
	unsigned char  j ;
	struct tm tm={0};
	time_t now;
	unsigned char  u8ScheduleTime = 0;
	QMAPI_SCHEDTIME *pTem = NULL;
	now = time(NULL);
	localtime_r(&now, &tm);
	unsigned char week;
	if( 0 == tm.tm_wday )
		week = 6;
	else
		week = tm.tm_wday -1;		

    //判断是否在布防时间内-4个时间段
    for(j = 0; j < 4; j++)
    {
        int iCurTime = (int)tm.tm_hour*60 + tm.tm_min;

		pTem = pScheduleTime + (week*4 + j);
        int iStaTime = (int)pTem->byStartHour*60 + pTem->byStartMin;
        int iStoTime = (int)pTem->byStopHour*60 + pTem->byStopMin;
		//printf("week %d star %d stop %d cur %d \n" , week , iStaTime , iStoTime);
		if(iStaTime==iStoTime) continue;//20131120 起止时间相同，不报警
        if(iCurTime >= iStaTime && iCurTime <= iStoTime)
        {
            u8ScheduleTime = 1;
            break;
        }
    }

	return u8ScheduleTime;
}

static void motion_detect_handler(char *event, void *arg)
{
	//printf("#### %s %d, event:%s\n", __FUNCTION__, __LINE__, event);
	//g_motion_status[0] = QMAPI_TRUE;
	gMotionDetectFlag = 1; /* RM#4336: report the motion event only once in 30 seconds.   henry.li    2017/06/14 */

	return;
}


/* RM#4336: report the motion event only once in 30 seconds.   henry.li    2017/06/15 */
void Motion_Event_Thread()
{
    unsigned char select_count = 0;
    unsigned int motion_timer = 0;

    prctl(PR_SET_NAME, (unsigned long)"MotionEvent", 0,0,0);
    pthread_detach(pthread_self());

    while (qmapi_alarm_run)
    {
        if ((select_count % 10) == 0)
        {
            if (gMotionDetectFlag == 1)
            {
                //printf("Flag 1: motion timer is [%d]\n", motion_timer);
                if (motion_timer == 0)
                {
                    //printf("First move or keep moveing 30s, Set new Motion flag!\n");
                    g_motion_status[0] = QMAPI_TRUE;
                    motion_timer = RESEND_MOTION_EVENT_TIMER;
                }

                motion_timer --;
                gMotionDetectFlag = 0;
            }
            else
            {
                //printf("Flag 0: motion timer is [%d]\n", motion_timer);
                if (motion_timer > 0)
                {
                    motion_timer --;
                }
            }
        }

        select_count ++;
        AlarmSleep(100);
	}
	qmapi_alarm_count--;

	return ;
}


void TL_Video_Alarm_Thread()
{
	int nRet , i;
	QMAPI_SYSTEMTIME systime;
	unsigned char  select_count = 0;
	int move_timer[QMAPI_MAX_CHANNUM], move_resume[QMAPI_MAX_CHANNUM];

	prctl(PR_SET_NAME, (unsigned long)"VideoAlarmDetect", 0,0,0);
	pthread_detach(pthread_self());

    /* RM#4336: report the motion event only once in 30 seconds.   henry.li    2017/06/14 */
    pthread_t motionEvent;
    if (pthread_create(&motionEvent, NULL, (void *)&Motion_Event_Thread , NULL) == 0)
    {
        qmapi_alarm_count ++;
    }

	event_register_handler(EVENT_VAMOVE, 0, motion_detect_handler);

	for(i=0; i<QMAPI_MAX_CHANNUM; i++)
	{
		move_timer[i]  = 0;
		move_resume[i] = FALSE;
	}

    while(qmapi_alarm_run)
    {
        if ((select_count % 10) == 0)
        {
            for(i=0; i<QMAPI_MAX_CHANNUM; i++)
            {
                ///Motion Alarm 
				nRet = TL_Video_Motion_Alarm_Timer(i);
				if (nRet)
				{
				    /* RM#4255: check current time is in schedule time or not.   henry.li    2017/06/08 */
//					if ((g_motion_status[0] == TRUE) &&
//					    (g_tl_globel_param.TL_Channel_Motion_Detect[i].bEnable)) /*RM#2836: send motion alarm message when the motion detect's config is enabled.   henry.li    2017/05/09*/
					if (g_motion_status[0] == TRUE)
					{
						//printf("current move_timer[%d]=[%d]\n", i, move_timer[i]);
						if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_GET_SYSTIME, 0, &systime, sizeof(QMAPI_SYSTEMTIME)) != QMAPI_SUCCESS)
						{
							err();
						}
						//if (move_timer[i] == 0) /* 已经产生了事件信息不需要上报 */
						if (move_timer[i] <= 2) /* 已经产生了事件信息不需要上报 */
						{
						    printf("+++++++++++ MOVE Detected, Time is in Schedule Time!!!+++++++++++\n");
							TL_addAlarm(TL_MSG_MOVEDETECT, i, &systime);
						}
						move_timer[i] = g_tl_globel_param.TL_Channel_Motion_Detect[i].nDuration;
						//printf("set move_timer[%d]=[%d]\n", i, move_timer[i]);
						///Need to Send resume message
						move_resume[i] = TRUE;
						g_motion_status[0] = FALSE;
					}
					else
					{
						if (move_timer[i] > 0)
						{
							move_timer[i]--;
						}
						if (move_timer[i]==0 && move_resume[i])
						{
							move_resume[i] = FALSE;
							if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_GET_SYSTIME, 0, &systime, sizeof(QMAPI_SYSTEMTIME)) != QMAPI_SUCCESS)
							{
								err();
							}
							TL_addAlarm(TL_MSG_MOVERESUME, i, &systime);
						}
					}	
				}
				else
				{
					if (g_motion_status[0] == TRUE)
                    {
						g_motion_status[0] = FALSE;
                    }

					if(move_timer[i]>0)
					{
						if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_GET_SYSTIME, 0, &systime, sizeof(QMAPI_SYSTEMTIME)) != QMAPI_SUCCESS)
						{
							err();
						}
						TL_addAlarm(TL_MSG_MOVERESUME, i, &systime);
						move_timer[i] = 0;
					}
				}
			}
		}
		select_count ++;
		AlarmSleep(100);
	}
	qmapi_alarm_count--;
	sem_post(&g_NVSalarmSem);
	
	return ;
}


