#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/prctl.h>
#include "tl_common.h"

static int g_saveStart = 0;
static pthread_t g_flashRtcTid = 0;
static FILE *g_frFp = NULL;

void flash_rtc_thread(void *arg)
{
    int cnt =0;
    char buf[32] = {0};
    time_t cur_time;
    
    prctl(PR_SET_NAME, (unsigned long)"flashrtc", 0,0,0);
    

    while(1)
    {
    	// need fixme yi.zhang
       // if(g_tl_updateflash.bUpdatestatus)
        {
        	#if 0
            cur_time = time(NULL);
            fseek(g_frFp, 0, SEEK_SET);
            fwrite(&cur_time, 1, sizeof(cur_time), g_frFp);
            fclose(g_frFp);
			g_frFp = NULL;
            return;
			#endif
			
		//	sleep(1);
		//	continue;
        }

        if(g_saveStart)
        {
            if(cnt%30 == 0)
            {
                memset(buf, 0, sizeof(buf));
                cur_time = time(NULL);
                sprintf(buf, "%lu", cur_time);
                fseek(g_frFp, 0, SEEK_SET);
                fwrite(buf, 1, strlen(buf), g_frFp);
            }

            cnt++;
        }

        sleep(2);
    }
}

void TL_flash_rtc_save_time()
{
    char buf[32] = {0};
    time_t cur_time;

    if(!g_frFp)
        return;

    if(!g_saveStart)
        return;
    
    cur_time = time(NULL);
    sprintf(buf, "%lu", cur_time);
    fseek(g_frFp, 0, SEEK_SET);
    fwrite(buf, 1, strlen(buf), g_frFp);
    fflush(g_frFp);

    return;
}

void TL_flash_rtc_start()
{
    g_saveStart = 1;
}


void TL_init_flash_rtc()
{
    char buf[32] = {0};
    time_t cur_time;
	char csRtcTimeFile[32] = {0};
	sprintf(csRtcTimeFile, "%s/flashrtc", TL_CONFIG_PATH);
    if(access(csRtcTimeFile, F_OK)==0)
    {
        g_frFp = fopen(csRtcTimeFile, "r+");
        if(!g_frFp)
        {
            printf("%s  %d, open %s failed!!\n",__FUNCTION__, __LINE__,csRtcTimeFile);
            return;
        }
        fgets(buf, 32, g_frFp);
        sscanf(buf, "%lu", &cur_time);
        cur_time += 15;
        stime(&cur_time);
        printf("%s  %d, set time from flash:%ld\n",__FUNCTION__, __LINE__,cur_time);
    }
    else
    {
        g_frFp = fopen(csRtcTimeFile, "w");
        if(!g_frFp)
        {
            printf("%s  %d, open %s failed!!\n",__FUNCTION__, __LINE__,csRtcTimeFile);
            return;
        }
    }
	
    if(g_flashRtcTid == 0)
        pthread_create(&g_flashRtcTid, NULL, (void *)flash_rtc_thread, NULL);
}

void TL_deinit_flash_rtc()
{
    if(g_flashRtcTid)
    {
        void *status;
        pthread_cancel(g_flashRtcTid);
        pthread_join(g_flashRtcTid, &status);
        g_flashRtcTid = 0;
    }

    if(g_frFp)
    {
        fclose(g_frFp);
        g_frFp = NULL;
    }
}


