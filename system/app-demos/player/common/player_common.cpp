/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: Player common functions
 *
 * Author:
 *     ranson.ni <ranson.ni@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/25/2017 init
 */
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <qsdk/vplay.h>
#include <qsdk/event.h>
#include <qsdk/videobox.h>

extern int s32StartPlay;

void VplayEventHandler(char *ps8Name, void *pArgs)
{
    vplay_event_info_t stVplayState;

    memset(&stVplayState, 0, sizeof(vplay_event_info_t));
    if((NULL == ps8Name) || (NULL == pArgs))
    {
        printf("invalid parameter!\n");
        return;
    }

    if (!strcmp(ps8Name, EVENT_VPLAY))
    {
        memcpy(&stVplayState, (vplay_event_info_t *)pArgs, sizeof(vplay_event_info_t));
        printf("vplay event type: %d, buf:%s\n", stVplayState.type, stVplayState.buf);

        switch (stVplayState.type)
        {
            //Player
            case VPLAY_EVENT_PLAY_FINISH:
            {
                printf("PLAY_STATE_END\n");
                s32StartPlay = 0;
                break;
            }
            case VPLAY_EVENT_PLAY_ERROR:
            {
                printf("PLAY_STATE_ERROR\n");
                s32StartPlay = 0;
                break;
            }
            default:
                break;
        }
    }
}

void VplayerSignalHandler(int s32Sig)
{
    char ps8Name[32] = {0};

    if ((s32Sig == SIGQUIT) || (s32Sig == SIGTERM))
    {
        printf("sigquit & sigterm ->%d\n",s32Sig);
    }
    else if (s32Sig == SIGSEGV)
    {
        prctl(PR_GET_NAME, ps8Name);
        printf("vplay Segmentation Fault thread -> %s <- \n", ps8Name);
        exit(1);
    }

    return;
}

void VplayerInitSignal(void)
{
    signal(SIGTERM, VplayerSignalHandler);
    signal(SIGQUIT, VplayerSignalHandler);
    signal(SIGTSTP, VplayerSignalHandler);
    signal(SIGSEGV, VplayerSignalHandler);
}

void InitPlayerInfo(VPlayerInfo *pstInfo, int s32Cnt)
{
    memset(pstInfo, 0, sizeof(VPlayerInfo));
    if (s32Cnt == 0)
    {
        strcat(pstInfo->video_channel, "dec0-stream");
        strcat(pstInfo->audio_channel, "default");
    }
    else if (s32Cnt == 1)
    {
        strcat(pstInfo->video_channel, "dec1-stream");
        strcat(pstInfo->audio_channel, "default");
    }
    else if (s32Cnt == 2)
    {
        strcat(pstInfo->video_channel, "dec2-stream");
        strcat(pstInfo->audio_channel, "default");
    }
    else if (s32Cnt == 3)
    {
        strcat(pstInfo->video_channel, "dec3-stream");
        strcat(pstInfo->audio_channel, "default");
    }
}

void ShowPlayerInfo(VPlayerInfo *pstInfo)
{
    printf("++show player info\n");
    printf("dec channel ->%s<-\n", pstInfo->video_channel);
    printf("audio channel ->%s<-\n", pstInfo->audio_channel);
    printf("media_file ->%s\n", pstInfo->media_file);
}
