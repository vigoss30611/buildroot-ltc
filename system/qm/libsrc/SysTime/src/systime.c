
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <linux/rtc.h>
#include <assert.h>
#include <errno.h>
#include <sys/time.h>


#include "QMAPI.h"
#include "QMAPIDataType.h"
#include "system_msg.h"

#define SYSTIME_BASE_YEAR                1900

struct ntp_arg{
	char server_name[64];
	unsigned short port;
	int timezone;
};

QMAPI_SYSTEMTIME SysTime;
static int Systime_Run_Thread = 0;
static int Ntp_Run_Thread = 0;
static pthread_t Ntp_Thread_Id;
static pthread_mutex_t tm_Mutex;

extern int GetNtpTime(unsigned long *pNtpTime, const char * pHostname, unsigned short server_port);


void Systime_Sleep(unsigned int  MilliSecond)
{
    struct timeval time;
    time.tv_sec  = MilliSecond / 1000;//seconds
    time.tv_usec = MilliSecond % 1000;//microsecond
    select(0, NULL, NULL, NULL, &time);
}

int QMAPI_Systime_Get_Timezone(int nTimeZone, int *tz, int *min)
{
    *min = 0;
    switch(nTimeZone)
    {
        case QMAPI_GMT_N12:
            *tz = -12;
            break;
        case QMAPI_GMT_N11:
            *tz = -11;
            break;
        case QMAPI_GMT_N10:
            *tz = -10;
            break;
        case QMAPI_GMT_N09:
            *tz = -9;
            break;
        case QMAPI_GMT_N08:
            *tz = -8;
            break;
        case QMAPI_GMT_N07:
            *tz = -7;
            break;
        case QMAPI_GMT_N06:
            *tz = -6;
            break;
        case QMAPI_GMT_N05:
            *tz = -5;
            break;
        case QMAPI_GMT_N0430:
            *tz = -4;
            *min = 30;
            break;
        case QMAPI_GMT_N04:
            *tz = -4;
            break;
        case QMAPI_GMT_N0330:
             *tz = -3;
            *min = 30;
            break;
        case QMAPI_GMT_N03:
            *tz = -3;
            break;
        case QMAPI_GMT_N02:
            *tz = -2;
            break;
        case QMAPI_GMT_N01:
            *tz = -1;
            break;
        case QMAPI_GMT_00:
        default:
            *tz = 0;
            break;
        case QMAPI_GMT_01:
            *tz = 1;
            break;
        case QMAPI_GMT_02:
            *tz = 2;
            break;
        case QMAPI_GMT_03:
            *tz = 3;
            break;
        case QMAPI_GMT_0330:
            *tz = 3;
            *min = 30;
            break;
        case QMAPI_GMT_04:
            *tz = 4;
            break;
        case QMAPI_GMT_0430:
            *tz = 4;
            *min = 30;
            break;
        case QMAPI_GMT_05:
            *tz = 5;
            break;
        case QMAPI_GMT_0530:
            *tz = 5;
            *min = 30;
            break;
        case QMAPI_GMT_0545:
            *tz = 5;
            *min = 45;
            break;
        case QMAPI_GMT_06:
            *tz = 6;
            break;
        case QMAPI_GMT_0630:
            *tz = 6;
            *min = 30;
            break;
        case QMAPI_GMT_07:
            *tz = 7;
            break;
        case QMAPI_GMT_08:
            *tz = 8;
            break;
        case QMAPI_GMT_09:
            *tz = 9;
            break;
        case QMAPI_GMT_0930:
            *tz = 9;
            *min = 30;
            break;
        case QMAPI_GMT_10:
             *tz = 10;
            break;
        case QMAPI_GMT_11:
            *tz = 11;
            break;
        case QMAPI_GMT_12:
            *tz = 12;
            break;
        case QMAPI_GMT_13:
            *tz = 13;
            break;
    }

    return 0;
}
void QMAPI_Systime_Set_Timezone(int nTimeZone)
{
    int tz = 0, min = 0;
    char cmdbuf[32];

    QMAPI_Systime_Get_Timezone(nTimeZone, &tz, &min);

    if(tz > 0)
    {
        if(min)
            sprintf(cmdbuf,"echo GMT-%d:%d > /etc/TZ", tz, min);
        else
            sprintf(cmdbuf,"echo GMT-%d > /etc/TZ",tz);
    }
    else
    {
        tz = 0 - tz;
        if(min)
            sprintf(cmdbuf,"echo GMT+%d:%d > /etc/TZ", tz, min);
        else
            sprintf(cmdbuf,"echo GMT+%d > /etc/TZ",tz);
    }
    SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
}


int QMAPI_SysTime_Set(QMAPI_SYSTEMTIME *systime)
{
	time_t now;
    struct tm tm;

    if (systime==NULL)
    {
        err();
        return 0;
    }

    memset(&tm, 0, sizeof(tm));
    tm.tm_year = systime->wYear-SYSTIME_BASE_YEAR;
    tm.tm_mon = systime->wMonth -1;
    tm.tm_mday = systime->wDay;
    tm.tm_hour = systime->wHour;
    tm.tm_min = systime->wMinute;
    tm.tm_sec = systime->wSecond;
    now = mktime(&tm);
    if (stime(&now) == 0)
    {
        pthread_mutex_lock(&tm_Mutex);
		memcpy(&SysTime, systime, sizeof(QMAPI_SYSTEMTIME));
        pthread_mutex_unlock(&tm_Mutex);
	}

    return 0;
}

void QMAPI_SysTime_Get(QMAPI_SYSTEMTIME *systime)
{
	if (systime)
	{
        pthread_mutex_lock(&tm_Mutex);
		memcpy(systime, &SysTime, sizeof(QMAPI_SYSTEMTIME));
        pthread_mutex_unlock(&tm_Mutex);
	}
}

void SysTime_Thread(void *arg)
{
	time_t t;
	struct tm tim;

	printf("%s:%s[%d] thread start...\n",__FILE__,__func__,__LINE__);
	prctl(PR_SET_NAME, (unsigned long)"SysTime_Thread", 0, 0, 0);
    pthread_detach(pthread_self()); /* 线程结束时自动清除 */

	while (Systime_Run_Thread)
	{
		t = time(NULL);
        if (localtime_r(&t, &tim))
        {
            pthread_mutex_lock(&tm_Mutex);
			SysTime.wSecond    = tim.tm_sec;
			SysTime.wMinute    = tim.tm_min;
			SysTime.wHour      = tim.tm_hour;
			SysTime.wDay       = tim.tm_mday;
			SysTime.wMonth     = tim.tm_mon + 1;
			SysTime.wDayOfWeek = tim.tm_wday;
			SysTime.wYear      = tim.tm_year+SYSTIME_BASE_YEAR;
            pthread_mutex_unlock(&tm_Mutex);
        }

        /*设备维护*/
		// need fixme yi.zhang

		Systime_Sleep(1000);
	}
	return ;
}

/*
	系统时间初始化, 设置时区，创建
*/
int QMAPI_SysTime_Init(int nTimeZone)
{
	pthread_t thd;
	QMAPI_Systime_Set_Timezone(nTimeZone);

	Systime_Run_Thread = 1;
    pthread_mutex_init(&tm_Mutex, NULL);

	if (pthread_create(&thd, NULL, (void *)&SysTime_Thread, NULL))
	{
		err();
	}
	return 0;
}

int QMAPI_SysTime_UnInit(void)
{
	Systime_Run_Thread = 0;
    pthread_mutex_destroy(&tm_Mutex);
	return 0;
}


void * SysTime_Ntp_Thread(void *arg)
{
	int NtpStart = 0;
	unsigned long sysntpTime;
	struct ntp_arg ntpinfo;
	unsigned int wInterval, TimeAdjustCounter=0;
	QMAPI_NET_NTP_CFG *Ninfo = (QMAPI_NET_NTP_CFG *)arg;

	prctl(PR_SET_NAME, (unsigned long)"Ntpthread", 0, 0, 0);
	printf("%s:%s[%d] thread start...\n",__FILE__,__func__,__LINE__);

	strcpy(ntpinfo.server_name, (char*)Ninfo->csNTPServer);
	ntpinfo.port = Ninfo->wNtpPort;
	wInterval    = Ninfo->wInterval * 3600;/*转换为秒*/
	while(Ntp_Run_Thread)
	{
		sysntpTime = 0;
		if(NtpStart == 0)
		{
			printf( "TimeAdjustCounter: %d,server:%s:%u\n", TimeAdjustCounter, ntpinfo.server_name, ntpinfo.port);
			GetNtpTime(&sysntpTime, ntpinfo.server_name, ntpinfo.port);
			if (sysntpTime)
			{
				NtpStart = 1;
			}
		}
		else
		{
			if ((TimeAdjustCounter % wInterval)==0)
			{
				printf( "TimeAdjustCounter: %d, wInterval=%d\n",TimeAdjustCounter, Ninfo->wInterval);
				GetNtpTime(&sysntpTime, ntpinfo.server_name, ntpinfo.port);
				if (sysntpTime)
				{
					NtpStart = 1;
				}
			}
		}
		if (sysntpTime) /* 获取到时间，设置时间*/
		{
			unsigned long currtime	= time(NULL);//time before change

			if((currtime >= sysntpTime && currtime - sysntpTime < 3) || (currtime < sysntpTime && sysntpTime - currtime < 3))
			{
				//printf( "not need to changge time diff less than 3 second\n");
			}
			else
			{
				QMAPI_SYSTEMTIME systime;

				struct tm tm_time;
				time_t SecNum = (time_t)sysntpTime;

				printf( "need to changge time diff less than 3 second. %lu. %lu.\n", currtime,sysntpTime);
				memset(&tm_time, 0, sizeof(struct tm));
				if (localtime_r(&SecNum, &tm_time))
				{
					systime.wYear	= tm_time.tm_year + SYSTIME_BASE_YEAR;
					systime.wMonth  = tm_time.tm_mon + 1;
					systime.wDay    = tm_time.tm_mday;
					systime.wHour   = tm_time.tm_hour;
					systime.wMinute = tm_time.tm_min;
					systime.wSecond = tm_time.tm_sec;
					systime.wMilliseconds= 0;

					QMAPI_SysTime_Set(&systime);
				}
			}
		}
		TimeAdjustCounter++;

		Systime_Sleep(1000);
	}

	return (void *)0;
}

int QMAPI_SysTime_NTPStart(QMAPI_NET_NTP_CFG *Ninfo)
{
	if (!Ntp_Run_Thread)
	{
		Ntp_Run_Thread = 1;
		if (pthread_create(&Ntp_Thread_Id, NULL, SysTime_Ntp_Thread, (void *)Ninfo))
		{
			Ntp_Run_Thread = 0;
		}
	}

	return 0;
}


int QMAPI_SysTime_NTPStop()
{
	Ntp_Run_Thread = 0;
	pthread_join(Ntp_Thread_Id, NULL);

	return 0;
}

int QMapi_SysTime_ioctrl(int Handle, int nCmd, int nChannel, void* Param, int nSize)
{
	int ret = QMAPI_FAILURE;
	
	if (QMAPI_NET_STA_SET_SYSTIME != nCmd
		&& QMAPI_NET_STA_GET_SYSTIME != nCmd
		&& QMAPI_SYSCFG_SET_ZONEANDDSTCFG != nCmd
		&& QMAPI_SYSCFG_SET_NTPCFG != nCmd)
	{
		return ret;
	}
	switch(nCmd)
	{
		case QMAPI_NET_STA_SET_SYSTIME:
		{
			ret = QMAPI_SysTime_Set((QMAPI_SYSTEMTIME *)Param);
			break;
		}
		case QMAPI_NET_STA_GET_SYSTIME:
		{
			QMAPI_SysTime_Get((QMAPI_SYSTEMTIME *)Param);
			ret = QMAPI_SUCCESS;
			break;
		}
		case QMAPI_SYSCFG_SET_ZONEANDDSTCFG:
		{
			QMAPI_NET_ZONEANDDST *info = (QMAPI_NET_ZONEANDDST *)Param;
			QMAPI_Systime_Set_Timezone(info->nTimeZone);
			ret = QMAPI_SUCCESS;
		}
		case QMAPI_SYSCFG_SET_NTPCFG:
		{
			 QMAPI_NET_NTP_CFG *ntpinfo = (QMAPI_NET_NTP_CFG *)Param;
			 if (ntpinfo->byEnableNTP)
			 {
				QMAPI_SysTime_NTPStart(ntpinfo);
			 }
			 else
			 {
				QMAPI_SysTime_NTPStop();
			 }
			 ret = QMAPI_SUCCESS;
		}
		default:
			break;
	}

	return ret;
}

