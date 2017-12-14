#include <qsdk/vplay.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/prctl.h>
#include <ctype.h>
#include <qsdk/event.h>

#define MAX_PLAYER_COUNT      4
#define INPUT_BUFFER_SIZE  2048
#define ENABLE_QUERY_THREAD   1

static int wait = 0;
static vplay_event_info_t vplayState;
static int s32StartPlay = 0;
static int s32ThreadExit = 0;
vplay_player_inst_t *pstPlayerList[MAX_PLAYER_COUNT] = {NULL};
vplay_player_inst_t **pstPlayer = NULL;

void vplayer_signal_handler(int sig)
{
    char name[32];

    if ((sig == SIGQUIT) || (sig == SIGTERM))
    {
        printf("sigquit & sigterm ->%d\n",sig);
        s32ThreadExit = 0;
    }
    else if (sig == SIGSEGV)
    {
        prctl(PR_GET_NAME, name);
        printf("vrec Segmentation Fault thread -> %s <- \n", name);
        s32ThreadExit = 0;
        sleep(1);
        exit(1);
    }

    return;
}
void vplayer_init_signal(void)
{
    signal(SIGTERM, vplayer_signal_handler);
    signal(SIGQUIT, vplayer_signal_handler);
    signal(SIGTSTP, vplayer_signal_handler);
    signal(SIGSEGV, vplayer_signal_handler);
}

void init_player_info(VPlayerInfo *info, int cnt){
    memset(info,0,sizeof(VPlayerInfo));
    if (cnt == 0)
    {
        strcat(info->video_channel,"dec0-stream");
        strcat(info->audio_channel,"default");
    }
    else if (cnt == 1)
    {
        strcat(info->video_channel,"dec1-stream");
        strcat(info->audio_channel,"default");
    }
    else if (cnt == 2)
    {
        strcat(info->video_channel,"dec2-stream");
        strcat(info->audio_channel,"default");
    }
    else if (cnt == 3)
    {
        strcat(info->video_channel,"dec3-stream");
        strcat(info->audio_channel,"default");
    }
}

void init_photo_player_info(VPlayerInfo *info){
    memset(info,0,sizeof(VPlayerInfo));

    strcat(info->video_channel,"ffphoto-stream");
    strcat(info->audio_channel,"default");
}

void init_audio_player_info(VPlayerInfo *info)
{
    memset(info,0,sizeof(VPlayerInfo));

    strcat(info->video_channel, "");
    strcat(info->audio_channel, "default");
}

void show_player_info(VPlayerInfo *info){
    printf("++show playerInfo\n");
    printf("dec channel ->%s<-\n",info->video_channel);
    printf("audio channel ->%s<-\n",info->audio_channel);
    printf("media_file ->%s\n", info->media_file);
}

void trim_string(char *str)
{
    char *start, *end;
    int len = strlen(str);

    if(str[len-1] == '\n')
    {
        len--;
        str[len] = 0;
    }

    start = str;
    end = str + len -1;
    while(*start && isspace(*start))
        start++;
    while(*end && isspace(*end))
        *end-- = 0;
    strcpy(str, start);
}

void remove_wait(int sig)
{
   if(sig==SIGALRM)
   {
       printf("get timer alarm, wait:%d!\n", wait);
       wait = 0;
   }
   else
       printf("get timer failed!\n");
}
void vplay_event_handler(char *name, void *args)
{

    if((NULL == name) || (NULL == args))
    {
        printf("invalid parameter!\n");
        return;
    }

    if(!strcmp(name, EVENT_VPLAY))
    {
        memcpy(&vplayState, (vplay_event_info_t *)args, sizeof(vplay_event_info_t));
        printf("vplay event type: %d, buf:%s\n", vplayState.type, vplayState.buf);

        switch (vplayState.type)
        {
            //Player
            case VPLAY_EVENT_PLAY_FINISH:
                printf("PLAY_STATE_END \n");
                s32StartPlay = 0;
                break;
            case VPLAY_EVENT_PLAY_ERROR:
                printf("PLAY_STATE_ERROR \n");
                s32StartPlay = 0;
                break;
            default:
                break;
        }
    }
}

void* query_position(void *pArg)
{
    vplay_media_info_t stMediaFileInfo = {0};
    while(!s32ThreadExit)
    {
        if (s32StartPlay && pstPlayer != NULL && *pstPlayer != NULL)
        {
            memset(&stMediaFileInfo, 0, sizeof(vplay_media_info_t));
            vplay_control_player(*pstPlayer, VPLAY_PLAYER_QUERY_MEDIA_INFO, &stMediaFileInfo);
            printf("play position --------->%8lld/%8lld\n",
                    stMediaFileInfo.time_pos,
                    stMediaFileInfo.time_length);
        }
        sleep(1);
    }
    printf("Exit query thread.\n");
}

int main(int argc ,char ** argv)
{
    vplay_player_info_t playerInfo ;
    int ret = 0;
    char cmds[50] = "./cmds.txt";
    FILE *fp = NULL;
    char* tmp = NULL;
    int instance_cnt = 0;
    int value = 0;
    int s32Err;
    char input[INPUT_BUFFER_SIZE];
    vplay_media_info_t mediaFileInfo = {0};
    pthread_t stQueryThread;

    init_player_info(&playerInfo, 0);
    printf("argc number ->%d\n", argc);
    if(argc < 2){
        printf("add your media file later\n");
    }
    else if(argc == 2){
        printf("add media first\n");
        sprintf(playerInfo.media_file, "%s", argv[1]);
    }
    else{
        //not handle
    }

#if ENABLE_QUERY_THREAD
    s32Err = pthread_create(&stQueryThread, NULL, query_position, NULL);
    if (s32Err != 0)
    {
        printf("can't create thread: %s\n", strerror(s32Err));
    }
#endif
    vplayer_init_signal();
    show_player_info(&playerInfo);
    memset(&vplayState, 0, sizeof(vplay_event_info_t));
    event_register_handler(EVENT_VPLAY, 0, vplay_event_handler);

    printf("input control cmd -> p:play s:stop p:pause c:continue \n");

    pstPlayer = &pstPlayerList[0];
    if (!(fp = fopen(cmds,"r")))
    {
        printf("has no cmd file.\n");
    }
    while(1){
        memset(input, 0, INPUT_BUFFER_SIZE);
        if (fp)
        {
            if (wait == 0)
            {
                if(!feof(fp))
                {
                    fgets(input, INPUT_BUFFER_SIZE, fp);
                }
                else
                {
                    fclose(fp);
                    fp = NULL;
                    printf("cmd file finish!\n");
                    continue;
                }
            }
            else
            {
                sleep(1);
                continue;
            }
        }
        else
        {
            fgets(input, INPUT_BUFFER_SIZE, stdin);
        }

        printf("input cmd:%s\n", input);
        if (strncmp("frinfo", input, 6) == 0)
        {
            system("cat /proc/fr_swap");
            continue;
        }
        if (strncmp("sys:", input, 4) == 0)
        {
            if(strlen(input) > 4)
            {
                tmp = input + 4;
                system(tmp);
            }
            else
            {
                printf("error cmd:%s\n", input);
            }
            continue;
        }
        if (strncmp("wait:", input, 5) == 0)
        {
            if(strlen(input) > 5)
            {
                value = atoi(input+5);
                if (value > 0)
                {
                    wait = value;
                    alarm(value);
                    signal(SIGALRM,remove_wait);
                    printf("wait for %d seconds\n", wait);
                }
            }
            else
            {
                printf("error cmd:%s\n", input);
            }
            continue;
        }
        if (strncmp("switch",input,6) == 0)
        {
            value = atoi(input+6);
            if ((value >= MAX_PLAYER_COUNT) || (value < 0))
            {
                printf("switch player error, %d not in[0,%d)\n", value, MAX_PLAYER_COUNT);
                continue;
            }
            pstPlayer = &pstPlayerList[value];
            init_player_info(&playerInfo, value);
            printf("switch to play %d.\n", value);
            continue;
        }
        if (strncmp("play",input,4) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            value = 0;
            trim_string(input);
            if (strlen(input) > 4)
            {
                value = atoi(input+4);
            }
            if (value == 0)
            {
                value = 1;
            }
            printf("play video ->%d\n", value);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_PLAY, &value);
            s32StartPlay = 1;
        }
        else if (strncmp("stop", input, 4) == 0)
        {
            int dat = 1;
            s32StartPlay = 0;

            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            printf("stop video \n");
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_STOP, &dat);
        }
        else if (strncmp("new", input, 3) == 0)
        {
            printf("new player \n");
            *pstPlayer = vplay_new_player(&playerInfo);
            if (*pstPlayer == NULL)
            {
                printf("init vplay player error \n");
                return  0;
            }
            printf("\ninput control cmd -> p:play s:stop p:pause c:continue \n");
        }
        else if (strncmp("delete", input, 6) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            printf("delete video \n");
            vplay_delete_player(*pstPlayer);
            *pstPlayer = NULL;
            instance_cnt --;
        }
        else if (strncmp("pause", input, 5) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            value = atoi(input+5);
            printf("pause video ->%d\n",value);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_PAUSE, &value);
            printf("\ninput control cmd -> p:play s:stop p:pause c:continue \n");
        }
        else if (strncmp("continue", input, 8) == 0)
        {
            if (*pstPlayer == NULL){
                printf("please new instance first\n");
                continue;
            }
            value = atoi(input+8);
            printf("continue video ->%d\n",value);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_CONTINUE, &value);
            printf("\ninput control cmd -> p:play s:stop p:pause c:continue \n");
        }
        else if (strncmp("seek", input, 4) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            value = atoi(input+4);
            printf("seek video ->%d\n",value);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_SEEK, &value);
            printf("\ninput control cmd -> p:play s:stop p:pause c:continue \n");
        }
        else if (strncmp("info", input, 4) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            memset(&mediaFileInfo,0,sizeof(vplay_media_info_t));
            printf("query video media info**********->%s\n", input);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_QUERY_MEDIA_INFO, &mediaFileInfo);
            printf("video info result->%8lld %8lld %d\n", ret,
                    mediaFileInfo.time_length,
                    mediaFileInfo.time_pos,
                    mediaFileInfo.track_number);
            vplay_show_media_info(&mediaFileInfo);
        }
        else if (strncmp("query", input, 5) == 0)
        {
            char *file_name = input + 6;
            printf("query media file -> %s\n", file_name);
            memset(&mediaFileInfo,0,sizeof(vplay_media_info_t));
            printf("query video media info**********->%s<-\n", file_name);
            ret = vplay_query_media_info(file_name, &mediaFileInfo);
            printf("video file info result->%d %8lld %lld %d\n", ret,
                    mediaFileInfo.time_length,
                    mediaFileInfo.time_pos,
                    mediaFileInfo.track_number);
            vplay_show_media_info(&mediaFileInfo);
        }
        else if (strncmp("vfilter", input, 7) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            int index = atoi(input+8);
            printf("video filter ->%d<-\n", index);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_SET_VIDEO_FILTER, &index);
        }
        else if (strncmp("afilter", input, 7) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            int index = atoi(input+8);
            printf("audio filter ->%d<-\n", index);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_SET_AUDIO_FILTER, &index);
        }
        else if (strncmp("efilter", input, 7) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            int index = atoi(input+8);
            printf("extra filter ->%d<-\n", index);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_SET_VIDEO_FILTER, &index);
        }
        else if (strncmp("amute", input, 5) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            int mute = atoi(input+6);
            printf("audio mute ->%d<-\n", mute);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_MUTE_AUDIO, &mute);
        }
        else if (strncmp("exit", input, 4) == 0)
        {
#if ENABLE_QUERY_THREAD
            s32ThreadExit = 1;
            pthread_join(stQueryThread, NULL);
#endif
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
            }
            for (int i = 0; i < MAX_PLAYER_COUNT; i++)
            {
                pstPlayer = &pstPlayerList[i];
                if (*pstPlayer != NULL)
                {
                    ret = vplay_delete_player(*pstPlayer);
                    printf("delete %d player, ret:%d\n", i, ret);
                    *pstPlayer = NULL;
                }
            }
            printf("video player exit\n");
            break;
        }
        else if (strncmp("quit", input, 4) == 0)
        {
#if ENABLE_QUERY_THREAD
            s32ThreadExit = 1;
            pthread_join(stQueryThread, NULL);
#endif
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
            }
            for (int i = 0; i < MAX_PLAYER_COUNT; i++)
            {
                pstPlayer = &pstPlayerList[i];
                if (*pstPlayer != NULL)
                {
                    ret = vplay_delete_player(*pstPlayer);
                    printf("delete %d player, ret:%d\n", i, ret);
                    *pstPlayer = NULL;
                }
            }
            printf("video player quit\n");
            break;
        }
        else if (strncmp("queue", input, 5) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            char *file = input + 6;
            trim_string(file);
            printf("add play file ->%s\n", file);
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_QUEUE_FILE, file);
        }
        else if (strncmp("photo", input, 5) == 0)
        {
            init_photo_player_info(&playerInfo);
            printf("begin play photo\n");
        }
        else if (strncmp("audio", input, 5) == 0)
        {
            init_audio_player_info(&playerInfo);
            printf("begin play audio\n");
        }
        else if (strncmp("step", input, 4) == 0)
        {
            if (*pstPlayer == NULL)
            {
                printf("please new instance first\n");
                continue;
            }
            ret = vplay_control_player(*pstPlayer, VPLAY_PLAYER_STEP_DISPLAY, NULL);
            printf("video step display\n");
        }
        else
        {
            printf("not supprot cmd ->%s\n", input);
        }
    }
    event_unregister_handler(EVENT_VPLAY, vplay_event_handler);
    return 1;
}
