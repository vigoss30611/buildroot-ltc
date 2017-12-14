#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/prctl.h>
#include <qsdk/vplay.h>
#include <unistd.h>
#include<time.h>
#include <qsdk/event.h>

///#include <qsdk/videobox.h>
 /* Prototypes for __malloc_hook, __free_hook */

#define TESTMAX 50
extern int video_enable_channel(const char *channel);
extern int video_disable_channel(const char *channel);

static int muteState = -1;
static int motionState = 0;
static vplay_event_info_t vplayState;
static time_t stopRecorder;
static int detectMv = 0;
static VRecorderInfo g_stRecorderInfo;
static int s_s32CacheMode = 0;
static int s_s32KeepTime = 0;
static int s_s32SetUUID = 0;


#define ENABLE_MV 0
#define ENABLE_SIGNAL 1 //if want to create core file, need disable it.

void print_help(char *app){

    printf("%s  [options]\n",app);
    printf(
             "Please only use short para"
             "\t-v, --video_channel            Video channel :enc1080p-strean ,enc720p-stream ...\n"
             "\t-a, --audio_channel            Audio channel:default_mic\n"
             "\t-g, --enable_gps            enable gps recorder ,default 0\n"
             "\taudio para\n"
             "\t-t, --audio_type            audio codec type  0: pcm 1: g711u 2:g711a 3:aac 4:g726 5:mp3 \n"
             "\t-r, --audio_sample_rate        Audio sample rate -> 8000,48000\n"
             "\t-f, --audio_sample_fmt        sample fmt ->0:8 1:16 2:24 3:32\n"
             "\t-c, --audio_channels        Audio channel \n"
             "\trecord file setting\n"
             "\t-s, --time_segment            File length :>5\n"
             "\t-b, --time_backward            back recorder length\n"
             "\t-S, --suffix                File name suffix\n"
             "\t-F, --av_format                File format\n"
             "\t-d, --dir_prefix            media file base dir\n"
             "\t-m, --mute                    mute some things ,0:audio 1:video 2:subtitle 3:data\n"
             "\t-M, --unmute                unmute some things ,0:audio 1:video 2:subtitle 3:data\n"
             "\t-k, --keep_temp                keep temp media file, 0: disable 1: enable\n"
             "\t-e, --fixed_fps                use fixed fps instand of auto check\n"
             "\t--slowmotion                set slowmotion rate \n"
             "\t--fastmotion                set fastmotion rate \n"
             "\t--ac_info                set audio channel info, formate channels:bitWidth:sampleRate:sampleSize \n"
             "\t--cache_mode             set recoder start from history cache\n"
             "\t--keep_time              keep recorded data for start new segment"
             "\t--set_uuid               Set uuid info to recoder"
             "\t\n"
             "\t\n"
             "\t\n"
             "\t-T, --test=case            Specify test case name\n"
             "\t\t\t nsps            new/delete test\n"
             "\t\t\t pause                pause test \n"
             "\t\t\t alert                pause test \n"
             "\t\n"
             "\t\n"

            );
}
int get_random_int(int min,int max){
    if (max <= min){
        printf("max must >= min\n");
        return min;
    }
    int ret = 0;
    srand(time(0));
    ret = rand()%(max - min)  + min;
    return ret;
}
#if 1
//"v:a:t:r:f:c:s:b:S:F:t:g"
const struct option recorder_long_options[] = {

    {"video_channel",         required_argument,  NULL,    'v'},
    {"audio_channel",         required_argument,  NULL,    'a'},
    {"audio_type",             required_argument,  NULL,    't'},
    {"audio_sample_rate",   required_argument,  NULL,    'r'},
    {"audio_sample_fmt",    required_argument,  NULL,    'f'},
    {"audio_channels",         required_argument,  NULL,    'c'},
    {"time_segment",         required_argument,  NULL,    's'},
    {"time_backward",         required_argument,  NULL,    'b'},
    {"suffix",                 required_argument,  NULL,    'S'},
    {"av_format",             required_argument,  NULL,    'F'},
    {"dir_prefix",             required_argument,  NULL,    'd'},
    {"test",                 required_argument,  NULL,    'T'},
    {"replace",                 required_argument,  NULL,    'R'},
    {"mute",                 required_argument,  NULL,    'm'},
    {"unmute",                 required_argument,  NULL,    'M'},
    {"fixed_fps",                 required_argument,  NULL,    'e'},
    {"keep_temp",             required_argument,  NULL,    'k'},
    {"enable_gps",           no_argument,        NULL,    'g'},
    {"disable_input",          no_argument,        NULL,    'i'},
    {"udta",             required_argument,  NULL,    'u'},
    {"slowmotion",             required_argument,  NULL,    'w'},
    {"fastmotion",             required_argument,  NULL,    'w'},
    {"ac_info",             required_argument,  NULL,    'w'},
    {"cache_mode",             required_argument,  NULL,    'w'},
    {"keep_time",             required_argument,  NULL,    'w'},
    {"set_uuid",             no_argument,  NULL,    'w'},
    {0,            0,                  NULL,   0}

};

#endif
void print_recorder_info(VRecorderInfo *info){

    printf("\trecorder info -> \n");
    printf("\t video_channel    ->%s<-\n", info->video_channel);
    printf("\t videoextra_channel    ->%s<-\n", info->videoextra_channel);
    printf("\t audio_channel    ->%s<-\n", info->audio_channel);
    printf("\t enable gps       ->%d<-\n", info->enable_gps);
    printf("\t keep temp        ->%d<-\n", info->keep_temp);
    printf("\t audio type       ->%d<-\n", info->audio_format.type);
    printf("\t audio sample_rate->%d<-\n", info->audio_format.sample_rate);
    printf("\t audio sample_fmt ->%d<-\n", info->audio_format.sample_fmt);
    printf("\t audio channels   ->%d<-\n", info->audio_format.channels);
    printf("\t audio effect     ->%d<-\n", info->audio_format.effect);
    printf("\t audio channel channels      ->%d<-\n", info->stAudioChannelInfo.s32Channels);
    printf("\t audio channel bitwidth      ->%d<-\n", info->stAudioChannelInfo.s32BitWidth);
    printf("\t audio channel sample_rate   ->%d<-\n", info->stAudioChannelInfo.s32SamplingRate);
    printf("\t audio channel sample_size   ->%d<-\n", info->stAudioChannelInfo.s32SampleSize);
    printf("\t time_segment     ->%d<-\n", info->time_segment );
    printf("\t time_backward    ->%d<-\n", info->time_backward );
    printf("\t fixed fps        ->%d<-\n", info->fixed_fps);
    printf("\t keep frames      ->%d<-\n", info->s32KeepFramesTime);
    printf("\t time_format      ->");
    puts(info->time_format);
    printf("\t suffix           ->");
    puts(info->suffix);
    printf("\t av_format        ->");
    puts(info->av_format);
    printf("\t dir_prefix       ->");
    puts(info->dir_prefix);
}
void vrec_signal_handler(int sig)
{
    char name[32];

    if ((sig == SIGQUIT) || (sig == SIGTERM)) {
        printf("sigquit & sigterm ->%d\n",sig);
    } else if (sig == SIGSEGV) {
        prctl(PR_GET_NAME, name);
        printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
        printf("\nvrec Segmentation Fault thread -> %s <- \n", name);
        printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
        exit(1);
    }

    return;
}
void vrec_init_signal(void)
{
    signal(SIGTERM, vrec_signal_handler);
    signal(SIGQUIT, vrec_signal_handler);
    signal(SIGTSTP, vrec_signal_handler);
    signal(SIGSEGV, vrec_signal_handler);
}
void init_recorder_default(VRecorderInfo *info){

    memset(info ,0,sizeof(VRecorderInfo));
//    char temp[] = "REC_VIDEO_%ts-te___%us-%ue_%c_%l";
//    char temp[] = "REC_VIDEO_%ts-___%us-%ue_%c_%l";
//    char temp[] = "REC_VIDEO_%ts-%te___%us-_%c_%l";
//    char temp[] = "REC_VIDEO_%ts-___%us-%ue_%c_%l";
    char temp[] = "%ts_%l_F";
    sprintf(info->video_channel,"enc1080p-stream" );
    sprintf(info->videoextra_channel,"" );
    sprintf(info->audio_channel,"default_mic" );
    info->enable_gps = 0;

    info->audio_format.type        = AUDIO_CODEC_TYPE_G711A;
    info->audio_format.sample_rate = 8000;
    info->audio_format.sample_fmt  = AUDIO_SAMPLE_FMT_S16;
    info->audio_format.channels    = 2;

    info->stAudioChannelInfo.s32Channels     = 2;
    info->stAudioChannelInfo.s32BitWidth     = 32;
    info->stAudioChannelInfo.s32SamplingRate = 16000;
    info->stAudioChannelInfo.s32SampleSize   = 1024;

    info->time_segment = 30;
    info->time_backward = 30;

//    strcat(info->time_format , "%d_%H_%M_%S");
    strcpy(info->time_format , "%Y_%m%d_%H%M%S___");
    strcat(info->av_format,"mkv");
    strcat(info->suffix,temp);
    strcat(info->dir_prefix,"/nfs/");

}

static void testcase_switch_resolution(VRecorderInfo *info, char* target_channel){
    printf("run ->%s swtich %s to %s\n",__FUNCTION__, info->video_channel, target_channel);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    char switch_channel[VPLAY_CHANNEL_MAX];

    video_disable_channel("venc0-stream");
    video_disable_channel("venc0-mv");
    video_disable_channel("venc1-stream");
    video_disable_channel("venc1-mv");

    video_disable_channel("venc0");
    video_disable_channel("venc1");
    while(counter < TESTMAX) {
        counter++;
        printf("\n======>enable channel %s\n", info->video_channel);
        video_enable_channel("venc0");
        recorder = vplay_new_recorder(info);
        if (recorder == NULL){
            printf("new recorder error ->%d\n", counter);
            return;
        }
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
            printf("start recorder error ->%d\n", counter);
            break;
        }
        sleep(3);
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
            printf("stop recorder error ->%d\n", counter);
            break;
        }
        if (vplay_delete_recorder(recorder) < 0) {
            printf("delete recorder error ->%d\n", counter);
            return;
        }
        sleep(3);
        printf("\n======>disable channel %s\n", info->video_channel);
        video_disable_channel("venc0");
        strncpy(switch_channel, info->video_channel, VPLAY_CHANNEL_MAX);

        printf("\n======>switch resolution,from %s to %s\n", info->video_channel, target_channel);

        strncpy(info->video_channel, target_channel, VPLAY_CHANNEL_MAX);
        printf("\n======>enable channel %s\n", info->video_channel);
        video_enable_channel("venc1");
        recorder = vplay_new_recorder(info);
        if (recorder == NULL){
            printf("new recorder error ->%d\n", counter);
            return;
        }
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0) {
            printf("start recorder error ->%d\n", counter);
        }

        sleep(3);
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0) {
            printf("stop recorder error ->%d\n", counter);
            break;
        }
        if (vplay_delete_recorder(recorder) < 0) {
            printf("delete recorder error ->%d\n", counter);
            return;
        }
        sleep(3);
        printf("\n======>disable channel %s\n", info->video_channel);
        video_disable_channel("venc1");
        strncpy(info->video_channel, switch_channel, VPLAY_CHANNEL_MAX);

        printf("\n======>switch resolution count:%d, from %s to %s\n",counter, target_channel, info->video_channel);
    }

}

static void testcase_new_delete(VRecorderInfo *info){
    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    while(1) {
        counter++;
        recorder = vplay_new_recorder(info);
        if (recorder == NULL){
            printf("new recorder error ->%d\n", counter);
            return;
        }
        printf("create new recoder ok ->%d\n", counter);
        sleep(5);
        if (vplay_delete_recorder(recorder) < 0) {
            printf("stop recorder error ->%d\n", counter);
            return;
        }
        printf("delete recoder ok ->%d\n", counter);
    }

}
static void testcase_new_start_delete(VRecorderInfo *info){
    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    while(1) {
        counter++;
        recorder = vplay_new_recorder(info);
        if (recorder == NULL){
            printf("new recorder error ->%d\n", counter);
            return;
        }
        printf("create new recoder ok ->%d\n", counter);
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
            printf("start recorder error ->%d\n", counter);
            break;
        }
        printf("start recoder ok ->%d\n", counter);
        sleep(3);
        if (vplay_delete_recorder(recorder) < 0) {
            printf("delete recorder error ->%d\n", counter);
            return;
        }
        sleep(3);
        printf("delete recoder ok ->%d\n", counter);
    }

}
static void testcase_start_stop(VRecorderInfo *info){

    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    recorder = vplay_new_recorder(info);
    if (recorder == NULL){
        printf("new recorder error ->%d\n", counter);
        return;
    }
    printf("instance ready ->%p\n", recorder);
    getchar();
    while(1) {
        counter++;
        printf("start recorder ->%d\n", counter);
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
            printf("start recorder error ->%d\n", counter);
        }
        printf("start recorder ok ->%d\n", counter);
        sleep(2);
        getchar();
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
            printf("stop recorder error ->%d\n", counter);
            break;
        }
        getchar();
        printf("stop recorder ok ->%d\n", counter);
    }

    if (vplay_delete_recorder(recorder) < 0) {
        printf("delete recorder error ->%d\n", counter);
        return;
    }

}
static void testcase_auto_start_stop(VRecorderInfo *info){

    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    recorder = vplay_new_recorder(info);
    if (recorder == NULL){
        printf("new recorder error ->%d\n", counter);
        return;
    }
    printf("instance ready ->%p\n", recorder);
    int interval;
    while(1) {
        counter++;
        printf("start recorder ->%d\n", counter);
        getchar();
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
            printf("start recorder error ->%d\n", counter);
            sleep(4);
            continue;
        }
        interval = get_random_int(0,5);
        printf("start recorder ok ->%d %d\n", counter, interval);
        sleep(interval);
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
            printf("stop recorder error ->%d\n", counter);
            //break;
        }
        interval = get_random_int(0,2);
        printf("stop recorder ok ->%d %d\n", counter, interval);
        sleep(interval);
    }

    if (vplay_delete_recorder(recorder) < 0) {
        printf("delete recorder error ->%d\n", counter);
        return;
    }

}
static void testcase_enable_disable(VRecorderInfo *info){

    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    recorder = vplay_new_recorder(info);
    if (recorder == NULL){
        printf("new recorder error ->%d\n", counter);
        return;
    }

    //    system(disable);
    //    system(disable);
    video_disable_channel(info->video_channel);
    video_disable_channel(info->video_channel);
    while(1) {
        counter++;
        video_enable_channel(info->video_channel);
        printf("++++++enable video channel->%s\n", info->video_channel);
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
            printf("start recorder error ->%d\n", counter);
        }
        printf("start recorder ok ->%d\n", counter);
        getchar();
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
            printf("stop recorder error ->%d\n", counter);
            break;
        }
        video_disable_channel(info->video_channel);
        printf("++++++disable video channel->%s\n", info->video_channel);
        getchar();
        printf("stop recorder ok ->%d\n", counter);
    }
    if (vplay_delete_recorder(recorder) < 0) {
        printf("delete recorder error ->%d\n", counter);
        return;
    }
}
static void testcase_sd_plug(VRecorderInfo *info){

    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    recorder = vplay_new_recorder(info);
    if (recorder == NULL){
        printf("new recorder error ->%d\n", counter);
        return;
    }

    char rec_temp[64];
    sprintf(rec_temp,"%s/flag", info->dir_prefix);

    printf("rec temp file ->%s\n",rec_temp);

    sleep(1);
    int start_flag = 0;
    while(1) {
        if (access(rec_temp,R_OK) == 0) {
            counter++;
            if (start_flag == 0) {
        //        sleep(2);
                if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
                    printf("start recorder error ->%d\n", counter);
                }
                printf("start recorder ok ->%d\n", counter);
                start_flag = 1;
            }

        }
        else {
            if (start_flag == 1 ){
                sleep(4);
                if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
                    printf("stop recorder error ->%d\n", counter);
                    break;
                }
                start_flag = 0;
                sleep(2);
                printf("stop recorder ok ->%d\n", counter);
            }
        }
        sleep(2);
    }

    if (vplay_delete_recorder(recorder) < 0) {
        printf("delete recorder error ->%d\n", counter);
        return;
    }

}
static void testcase_new_start_stop_delete2(VRecorderInfo *info){

    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    int counter  = 1;
    while(1) {
        printf("new recorder->%d\n", counter);
        recorder = vplay_new_recorder(info);
        if (recorder == NULL){
            printf("new recorder error ->%d\n", counter);
            return;
        }
        printf("new recorder ok->%d\n", counter);
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
            printf("start recorder error ->%d\n", counter);
        }
        printf("start recorder ok ->%d\n", counter);
        sleep(3);
        printf("stop recorder->%d\n", counter);
        if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
            printf("stop recorder error ->%d\n", counter);
            break;
        }
        printf("stop recorder ok ->%d\n", counter);

        printf("delete recorder->%d\n", counter);
        if (vplay_delete_recorder(recorder) < 0) {
            printf("delete recorder error ->%d\n", counter);
            return;
        }
        printf("delete recorder ok ->%d\n", counter);
        counter++;
        sleep(3);
    }

}
static void testcase_alert(VRecorderInfo *info){

    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    char alert[32];
    recorder = vplay_new_recorder(info);
    if (recorder == NULL){
        printf("new recorder error ->%d\n", counter);
        return;
    }
    sleep(5);
    if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
        printf("start recorder error ->%d\n", counter);
    }
    printf("start recorder ok ->%d\n", counter);
    while(1) {
        memset(alert,0,32);
        sprintf(alert,"alert->%d", counter);
        counter++;
        sleep(10);
#if 0
        if (vplay_alert_recorder(recorder,alert) < 0) {
            printf("alert recorder error\n");
            break;
        }
#endif
    }

    if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
        printf("stop recorder error ->%d\n", counter);
    }
        printf("stop recorder ok ->%d\n", counter);
    if (vplay_delete_recorder(recorder) < 0) {
        printf("delete recorder error ->%d\n", counter);
        return;
    }

}
static void normal_no_input_run(VRecorderInfo *info){
    printf("run ->%s\n",__FUNCTION__);
    VRecorder *recorder = NULL;
    static int counter  = 0;
    char alert[32];
    char input[64];
    char *tmp = NULL;
    int rate = 0;
    time_t now;
    int beginRecorder = 0;
    char pu8UUIDData[] = "<rdf:SphericalVideo xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" xmlns:GSpherical=\"http://ns."\
        "google.com/videos/1.0/spherical/\">  <GSpherical:Spherical>true</GSpherical:Spherical>  <GSpherical:Stitched>true"\
        "</GSpherical:Stitched>  <GSpherical:StitchingSoftware>Spherical Metadata Tool</GSpherical:StitchingSoftware>  <GSph"\
        "erical:ProjectionType>equirectangular</GSpherical:ProjectionType></rdf:SphericalVideo>";

    memset(input, 0, 64);
    recorder = vplay_new_recorder(info);
    if (recorder == NULL){
        printf("new recorder error ->%d\n", counter);
        return;
    }

    if (s_s32CacheMode)
        vplay_set_cache_mode(recorder, 1);
    else
        vplay_set_cache_mode(recorder, 0);

    if (s_s32SetUUID)
    {
        vplay_control_recorder(recorder, VPLAY_RECORDER_SET_UUID_DATA, pu8UUIDData);
    }
    if (muteState == 0)
    {
        vplay_control_recorder(recorder, VPLAY_RECORDER_MUTE, NULL);
    }
    else if ((muteState == 1) || (muteState == 2))
    {
        printf("mute %d not support!\n", muteState);
    }
    if (motionState != 0)
    {
        if (motionState < 0)
        {
            rate = - motionState;
            if (vplay_control_recorder(recorder, VPLAY_RECORDER_SLOW_MOTION, &rate) < 0){
                printf("start recorder error ->%d\n", counter);
            }
        }
        else
        {
            rate = motionState;
            if (vplay_control_recorder(recorder, VPLAY_RECORDER_FAST_MOTION, &rate) < 0){
                printf("start recorder error ->%d\n", counter);
            }
        }
    }
#if (ENABLE_MV == 0)
    if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
        printf("start recorder error ->%d\n", counter);
    }
#endif
    printf("start recorder ok ->%d\n", counter);
    while(1) {
#if ENABLE_MV
        if (detectMv)
        {
            if (beginRecorder == 0)
            {
                if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
                    printf("start recorder error ->%d\n", counter);
                }
                beginRecorder = 1;
            }
            detectMv = 0;
            while (detectMv == 0)
            {
                time(&now);
                if (now - stopRecorder > 30)
                {
                    if (beginRecorder == 1)
                    {
                        if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
                            printf("stop recorder error ->%d\n", counter);
                        }
                        beginRecorder= 0;
                    }
                    break;
                }
                sleep(1);
            }
        }
        sleep(1);
#else
        memset(input, 0, 64);
        fgets(input, 60, stdin);
        printf("input:%s\n", input);
        if (strncmp("stop", input, 4) == 0){
            printf("stop recorder.\n");
            break;
        }
        else if (strncmp("sys:", input, 4) == 0)
        {
            if (strlen(input) > 4)
            {
                tmp = input + 4;
                system(tmp);
            }
        }
        else if (strncmp("audioinfo", input, strlen("audioinfo")) == 0)
        {
            ST_AUDIO_CHANNEL_INFO stInfo;

            memset(&stInfo, 0, sizeof(stInfo));
            if (vplay_control_recorder(recorder, VPLAY_RECORDER_GET_AUDIO_CHANNEL_INFO, &stInfo) < 0) {
                printf("get audio info error.\n");
            }
            else
            {
                printf("\t audio channel channels      ->%d<-\n", stInfo.s32Channels);
                printf("\t audio channel bitwidth      ->%d<-\n", stInfo.s32BitWidth);
                printf("\t audio channel sample_rate   ->%d<-\n", stInfo.s32SamplingRate);
                printf("\t audio channel sample_size   ->%d<-\n", stInfo.s32SampleSize);
            }
        }
        else if (strncmp("new2", input, 4) == 0)
        {
            char temp[] = "NEW2_%ts_%l_F";
            memset(g_stRecorderInfo.suffix, 0, STRING_PARA_MAX_LEN);

            strcat(g_stRecorderInfo.suffix, temp);
            g_stRecorderInfo.s32KeepFramesTime = 0;
            if (vplay_control_recorder(recorder, VPLAY_RECORDER_NEW_SEGMENT, &g_stRecorderInfo) < 0) {
                printf("start new segment.\n");
            }
            system("date");
        }
        else if ((strncmp("new1", input, 4) == 0) || (strncmp("new", input, 3) == 0))
        {
            char temp[] = "NEW1_%ts_%l_F";
            memset(g_stRecorderInfo.suffix, 0, STRING_PARA_MAX_LEN);

            strcat(g_stRecorderInfo.suffix, temp);
            g_stRecorderInfo.s32KeepFramesTime = -1;
            if (vplay_control_recorder(recorder, VPLAY_RECORDER_NEW_SEGMENT, &g_stRecorderInfo) < 0) {
                printf("start new segment.\n");
            }
            system("date");
        }

#endif
    }


    if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
        printf("stop recorder error ->%d\n", counter);
    }
        printf("stop recorder ok ->%d\n", counter);
    if (vplay_delete_recorder(recorder) < 0) {
        printf("delete recorder error ->%d\n", counter);
        return;
    }

}
static void normal_input_run(VRecorderInfo *info){
    printf("run ->%s\n",__FUNCTION__);

    VRecorder *recorder = NULL;
    static int counter  = 0;
    char alert[32];
    int c;
    #if 0
    recorder = vplay_new_recorder(info);
    if (recorder == NULL){
        printf("new recorder error ->%d\n", counter);
        return;
    }
    #endif
    int ret;
    printf("input control cmd : \n\tstart \n\tdelete \n\tcontinue \n\tq:stop \n\tquit \n\tpause \n\tmute 1/2/3\n\tunmute 1/2/3 lapse 1/2/3/-2/-3\n");
    int value;
    int len;
    char input[64];
    while(1) {
        memset(input, 0, 64);
        fgets(input, 60, stdin);
        if (strncmp("new", input, 3) == 0){
            if (recorder == NULL){
                recorder = vplay_new_recorder(info);
                printf("start recorder ->%d\n", ret);
            }
            else{
                printf("recorder instace is ready ,no need->%p\n", recorder);
            }
        }
        else if (strncmp("start", input, 5) == 0){
            printf("start recorder\n");
            if (recorder == NULL){
                printf("the instance not exist ,create it\n");
                recorder = vplay_new_recorder(info);
            }
            printf("start recorder ->%p\n", recorder);
            if (vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL) < 0){
                printf("start recorder error ->%d\n", counter);
            }
        }
        else if (strncmp("delete", input, 6) == 0){
            ret = vplay_delete_recorder(recorder);
            printf("delete recorder ->%d\n", ret);
            printf("the instance has been delete , you can ctrl+c exit app\n");
            recorder = NULL;
        }
        else if (strncmp("stop", input,4) == 0){
            printf("stop recorder\n");
            if (recorder == NULL){
                printf("the recorder instance is not exist, do nothing\n");
                continue;
            }
            if (vplay_control_recorder(recorder, VPLAY_RECORDER_STOP, NULL) < 0){
                printf("stop recorder ->%d\n", ret);
                sleep(4);
            }
        }
        else if (strncmp("enable", input, 6) == 0){
            sprintf(input,"vbctrl video %s  --enable", info->video_channel);
            printf("enable video channel ->%s\n", info->video_channel);
            printf("enable cmd ->%s\n", input);
            video_enable_channel(info->video_channel);
        }

        else if (strncmp("disable", input, 7) == 0){
            sprintf(input,"vbctrl video %s  --disable", info->video_channel);
            printf("disable video channel ->%s\n", info->video_channel);
            printf("enable cmd ->%s\n", input);
//            system(input);
            video_disable_channel(info->video_channel);
        }
        else if (strncmp("continue", input, 7) == 0){
            if (recorder == NULL){
                printf("the recorder instance is not exist, do nothing\n");
                continue;
            }
        }
        else if (strncmp("mute", input, 4) == 0){
            if (recorder == NULL){
                printf("the recorder instance is not exist, do nothing\n");
                continue;
            }
            if (input[4] == ' '){
                value = atoi(input + 4);
                if (value == 0){
                    printf("1: mute audio 2: mute video 3: mute subtitle ,4 :extra");
                    break;
                }
                printf("mute ->%d\n", value);
                vplay_mute_recorder(recorder, value, 1);
            }
        }
        else if (strncmp("unmute", input,6) == 0){
            if (recorder == NULL){
                printf("the recorder instance is not exist, do nothing\n");
                continue;
            }
            if (input[6] == ' '){
                value = atoi(input + 6);
                printf("unmute ->%d\n", value);
                if (value == 0){
                    printf("1: mute audio 2: mute video 3: mute subtitle ,4 :extra");
                    break;
                }
                vplay_mute_recorder(recorder, value, 0);
            }
        }
        else if (strncmp("udta", input,4) == 0){
            if (recorder == NULL){
                printf("the recorder instance is not exist, do nothing\n");
                continue;
            }
            char *p = input + 4;
            if (*p == ' '){
                p++;
                len = strlen(p);
                if (len <= 0 ){
                    printf("udta data is null");
                    continue;
                }
                vplay_udta_info_t udta;
                if (access(p,R_OK) == 0){
                    printf("udta file->%s<-\n", p);
                    udta.buf  = p;
                    udta.size = 0;
                }
                else{
                    printf("udta data->%s<-\n", p);
                    udta.buf  = p;
                    udta.size = len;
                }
                vplay_control_recorder(recorder, VPLAY_RECORDER_SET_UDTA, &udta);
            }
        }
        else if (strncmp("fast", input, 4) == 0){
            if (recorder == NULL){
                printf("the recorder instance is not exist, do nothing\n");
                continue;
            }
            if (input[4] == ' '){
                value = atoi(input + 4);
                if (value == 0 ){
                    printf("must set motion value\n");
                    break;
                }
                printf("fast motion ->%d\n", value);
                vplay_control_recorder(recorder,  VPLAY_RECORDER_FAST_MOTION, &value);
            }
        }
        else if (strncmp("slow", input, 4) == 0){
            if (recorder == NULL){
                printf("the recorder instance is not exist, do nothing\n");
                continue;
            }
            if (input[4] == ' '){
                value = atoi(input + 4);
                if (value == 0 ){
                    printf("must set motion value\n");
                    break;
                }
                printf("slow motion ->%d\n", value);
                vplay_control_recorder(recorder,  VPLAY_RECORDER_SLOW_MOTION, &value);
            }
        }
        else if (strncmp("exit", input,4) == 0){
            break;
        }
        else if (strncmp("help", input,4) == 0){
            printf("input control cmd : \n\tstart \n\tdelete \n\tcontinue \n\tq:stop \n\tquit \n\tpause \n\tmute 1/2/3\n\tunmute 1/2/3\n");
        }
        else if (strncmp("check", input,5) == 0){
            video_disable_channel(info->video_channel);
            video_enable_channel(info->video_channel);
            printf("input control cmd : \n\tstart \n\tdelete \n\tcontinue \n\tq:stop \n\tquit \n\tpause \n\tmute 1/2/3\n\tunmute 1/2/3\n");
            printf("start recorder\n");
            if (recorder == NULL){
                printf("the instance not exist ,create it\n");
                recorder = vplay_new_recorder(info);
            }
            printf("start recorder ->%p\n", recorder);
            ret = vplay_control_recorder(recorder, VPLAY_RECORDER_START, NULL);
            printf("start recorder result ->%d\n", ret);
        }
        else{
            printf("cmd not support->%s\n", input);
        }
    }

    if (vplay_delete_recorder(recorder) < 0) {
        printf("delete recorder error ->%d\n", counter);
        return;
    }

}

static int vrec_parse_long_options(char *program_name, int index, char **argv)
{
    int ret = 0;
    char *pStr = NULL;
    char *pTmp = NULL;
    if (0 == strcmp(recorder_long_options[index].name, "slowmotion"))
    {
        motionState = 0 - atoi(optarg);
        printf("slow motion:%d\n", motionState);
    }
    else if (0 == strcmp(recorder_long_options[index].name, "fastmotion"))
    {
        motionState = atoi(optarg);
        printf("fast motion:%d\n", motionState);
    }
    else if (0 == strcmp(recorder_long_options[index].name, "ac_info") && optarg != NULL)
    {
        printf("ac_info %s\n", optarg);
        g_stRecorderInfo.stAudioChannelInfo.s32Channels = strtol(optarg, &pStr, 0);

        if (*pStr != ':')
        {
            return ret;
        }
        pStr++;
        g_stRecorderInfo.stAudioChannelInfo.s32BitWidth = strtol(pStr, &pTmp, 0);
        pStr = pTmp;

        if (*pStr != ':')
        {
            return ret;
        }
        pStr++;
        g_stRecorderInfo.stAudioChannelInfo.s32SamplingRate = strtol(pStr, &pTmp, 0);
        pStr = pTmp;

        if (*pStr != ':')
        {
            return ret;
        }
        pStr++;
        g_stRecorderInfo.stAudioChannelInfo.s32SampleSize = strtol(pStr, &pTmp, 0);
    }
    else if (0 == strcmp(recorder_long_options[index].name, "cache_mode"))
    {
        s_s32CacheMode = atoi(optarg);;
    }
    else if (0 == strcmp(recorder_long_options[index].name, "keep_time"))
    {
        s_s32KeepTime = atoi(optarg);
        printf("keep time:%d\n", s_s32KeepTime);
    }
    else if (0 == strcmp(recorder_long_options[index].name, "set_uuid"))
    {
        s_s32SetUUID = 1;
        printf("set UUID\n");
    }
    else
    {
        printf("long option:%s\n", optarg);
        return -1;
    }
    return ret;
}

void recorder_event_handler(char *name, void *args)
{
    if ((NULL == name) || (NULL == args)) {
        printf("invalid parameter!\n");
        return;
    }

    if (!strcmp(name, EVENT_VPLAY)) {
        memcpy(&vplayState, (vplay_event_info_t *)args, sizeof(vplay_event_info_t));
        printf("vplay event type: %d, buf:%s\n", vplayState.type, vplayState.buf);

        switch (vplayState.type) {
            //Recorder Event
            case VPLAY_EVENT_NEWFILE_START:
                printf("Start new file recorder!\n");
                break;
            case VPLAY_EVENT_NEWFILE_FINISH:
                {
                    vplayState.type = VPLAY_EVENT_NONE;
                    printf("Add file:%s\n", (char *)vplayState.buf);
                    break;
                }
            case VPLAY_EVENT_VIDEO_ERROR:
            case VPLAY_EVENT_AUDIO_ERROR:
            case VPLAY_EVENT_EXTRA_ERROR:
            case VPLAY_EVENT_INTERNAL_ERROR:
                printf("Fatal error, need stop recorder, type = %d!\n", vplayState.type);
                break;
            case VPLAY_EVENT_DISKIO_ERROR:
                {
                    printf("Fatal error, need stop recorder, type = %d!\n", vplayState.type);
                    break;
                }
            default:
                break;
        }
    }
#if ENABLE_MV
    else if (!strcmp(name, EVENT_VAMOVE)) {
        time(&stopRecorder);
        detectMv = 1;
        printf("Detect movement, start recorder %lld !\n", stopRecorder);
    }
#endif
}

int main(int argc, char *argv[]){

    int testMode = 0;
    int inputMode = 0;
    char testcase[32] = {0};
    static char switch_channel[STRING_PARA_MAX_LEN] = {0};
    int temp = 0;
    int option_index = 0;

#if ENABLE_SIGNAL
    vrec_init_signal();
#endif

    printf("start %s\n", argv[0]);
    init_recorder_default(&g_stRecorderInfo);

    if (argc > 1)
    {
        int opt = 0;
        while((opt = getopt_long(argc, argv,"u:v:V:a:t:r:R:f:c:s:b:m:S:F:d:T:k:e:l:gi",recorder_long_options,&option_index)) != -1 ){
            printf("opt ->%d:%s %d\n", opt, optarg, option_index);
            switch (opt){
                case 'v' :
                    memset(g_stRecorderInfo.video_channel, 0, VPLAY_CHANNEL_MAX);
                    strcat(g_stRecorderInfo.video_channel, optarg);
                    break;
                case 'V':
                    memset(g_stRecorderInfo.videoextra_channel, 0, VPLAY_CHANNEL_MAX);
                    strcat(g_stRecorderInfo.videoextra_channel, optarg);
                    break;
                case 'a' :
                    printf("get audio channel ->%d:%s\n", opt, optarg);
                    memset(g_stRecorderInfo.audio_channel, 0, VPLAY_CHANNEL_MAX);
                    if (strstr(optarg, "null") == NULL ){
                        strcat(g_stRecorderInfo.audio_channel, optarg);
                    }
                    else {
                        printf("disable audio channel\n");
                    }
                    break;
                case 't' :
                    g_stRecorderInfo.audio_format.type = atoi(optarg);
                    break;
                case 'r' :
                    g_stRecorderInfo.audio_format.sample_rate = atoi(optarg);
                    break;
                case 'f' :
                    g_stRecorderInfo.audio_format.sample_fmt = atoi(optarg);
                    break;
                case 'c' :
                    g_stRecorderInfo.audio_format.channels = atoi(optarg);
                    break;
                case 's' :
                    g_stRecorderInfo.time_segment = atoi(optarg);
                    break;
                case 'b' :
                    g_stRecorderInfo.time_backward = atoi(optarg);
                    break;
                case 'S' :
                    memset(g_stRecorderInfo.suffix, 0, STRING_PARA_MAX_LEN);
                    strcat(g_stRecorderInfo.suffix, optarg);
                    break;
                case 'F' :
                    memset(g_stRecorderInfo.av_format, 0, STRING_PARA_MAX_LEN);
                    strcat(g_stRecorderInfo.av_format, optarg);
                    break;
                case 'd' :
                    memset(g_stRecorderInfo.dir_prefix, 0, STRING_PARA_MAX_LEN);
                    strcat(g_stRecorderInfo.dir_prefix, optarg);
                    break;
                case 'T' :
                    memset(testcase, 0, STRING_PARA_MAX_LEN);
                    strcat(testcase,optarg);
                    testMode = 1;
                    break;
                case 'R':
                    memset(switch_channel, 0, STRING_PARA_MAX_LEN);
                    strcat(switch_channel, optarg);
                    break;
                case 'g' :
                    g_stRecorderInfo.enable_gps = 1;
                    break;
                case 'i' :
                    inputMode = 1;
                    break;
                case 'e' :
                    g_stRecorderInfo.fixed_fps = atoi(optarg);
                    break;
                case 'k' :
                    g_stRecorderInfo.keep_temp = atoi(optarg);
                    break;
                case 'm' :
                    muteState = atoi(optarg);
                    printf("muteState:%d\n", muteState);
                    break;
                case 'w' :
                    vrec_parse_long_options(optarg, option_index, argv);
                    printf("motionState:%d\n", motionState);
                    break;
                default :
                    print_help(argv[0]);
                    exit(1);
            }

        }

    }

    time(&stopRecorder);

    memset(&vplayState, 0, sizeof(vplay_event_info_t));
    event_register_handler(EVENT_VPLAY, 0, recorder_event_handler);
#if ENABLE_MV
    event_register_handler(EVENT_VAMOVE, 0, recorder_event_handler);
#endif

    if (s_s32KeepTime > 0)
    {
        g_stRecorderInfo.s32KeepFramesTime = s_s32KeepTime;
    }
    print_recorder_info(&g_stRecorderInfo);
    printf("test mode ->%d -->%s<-\n", testMode, testcase);
    printf("input mode ->%d\n\n", inputMode);
    printf("switch_channel:%s\n", switch_channel);
    if (testMode){
        if (strstr(testcase, "new_delete") != NULL){
            testcase_new_delete(&g_stRecorderInfo);
        }
        else if (strstr(testcase, "new_start_delete") != NULL){
            testcase_new_start_delete(&g_stRecorderInfo);
        }
        else if (strstr(testcase, "new_start_stop_delete") != NULL){
            testcase_new_start_stop_delete2(&g_stRecorderInfo);
        }
        else if (strstr(testcase, "start_stop")  == testcase){
            testcase_start_stop(&g_stRecorderInfo);
        }
        else if (strstr(testcase, "auto_start_stop") == testcase){
            testcase_auto_start_stop(&g_stRecorderInfo);
        }
        else if (strstr(testcase, "enable_disable") != NULL){
            testcase_enable_disable(&g_stRecorderInfo);
        }
        else if (strstr(testcase, "sd_plug") != NULL){
            testcase_sd_plug(&g_stRecorderInfo);
        }
        else if (strstr(testcase, "alert") != NULL){
            testcase_alert(&g_stRecorderInfo);
        }
        else if (strstr(testcase, "switch_recorder_resolution") != NULL){
            testcase_switch_resolution(&g_stRecorderInfo, switch_channel);
        }
        else {
            printf("wrong test case name \n");
            print_help(argv[0]);
        }
    }
    else {
        if (inputMode){
            normal_input_run(&g_stRecorderInfo);
        }
        else {
            normal_no_input_run(&g_stRecorderInfo);
        }
    }

    event_unregister_handler(EVENT_VPLAY, recorder_event_handler);
#if ENABLE_MV
    event_unregister_handler(EVENT_VAMOVE, recorder_event_handler);
#endif

    return 1;
}
