#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <log.h>
#include <smartrc.h>

static int smartrc_stop = 0;
bool g_tosetroi;
struct v_roi_info g_roi;

static void smartrc_exit(void)
{
    //audiobox_listener_deinit();
    smartrc_defaultsetting_deinit();
    smartrc_unregister_msg_handler_cb();
}

void smartrc_signal_handler(int sig)
{
    LOGD("smartrc caught signal: %d\n", sig);
    smartrc_stop = 1;
}

static void smartrc_signal_init(void)            
{                                            
    signal(SIGTERM, smartrc_signal_handler);
    signal(SIGQUIT, smartrc_signal_handler);
    signal(SIGINT, smartrc_signal_handler); 
    signal(SIGTSTP, smartrc_signal_handler);
    signal(SIGABRT, smartrc_signal_handler);
    signal(SIGALRM, roi_alarm_handler);
}                                            

#define RC_JSON "/root/.videobox/rc.json"

extern void test_videobox_api();
void *roi_process(void *arg)
{
    g_tosetroi = false;
    while(1) {
        usleep(300000);
        if(g_tosetroi) {
            setroi(g_roi);
            g_tosetroi = false;
        }
    }
    return NULL;
}
int main(int argc, char *argv[])
{
    struct timeval timetv;
    int ret;
    Json js;
    gettimeofday(&timetv, 0);
    LOGD("smartrc start at: %s\n", asctime(localtime(&timetv.tv_sec)));
    
    
    /* set signal init */
    smartrc_signal_init();
  
    try { 
    js.Load(RC_JSON);
    } catch (const char *err) { 
        LOGE( "ERROR: %s, %s\n", err, RC_JSON); 
        kill(getppid(), SIGTERM);
        return -1;
    }
    
    if (smartrc_defaultsetting_init(&js) <0 )
    {
        smartrc_defaultsetting_deinit();
        kill(getppid(), SIGTERM);
        return -1;
    }
    
    ret = smartrc_register_msg_handler_cb();
    if (ret < 0) {
        LOGE(" register event cb failed\n");
        smartrc_exit();
        return -1;
    }
    
    pthread_t roi_thread;
    pthread_create(&roi_thread, NULL, roi_process, NULL);

    while (1) {                
        sleep(TIME_GAP_2S); 
        
        //LOGE("smartrc running ... \n");
        smartrc_monitor_system_status();

        if (smartrc_stop) {
            LOGE("smartrc exit\n");
            smartrc_exit();
            break;
        }
    }

    return 0;
}

