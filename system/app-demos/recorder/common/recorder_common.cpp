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
 * Description: Recorder common functions
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

void RecorderEventHandler(char *ps8Name, void *pArgs)
{
    vplay_event_info_t stVplayState;

    if ((NULL == ps8Name) || (NULL == pArgs))
    {
        printf("invalid parameter!\n");
        return;
    }

    memset(&stVplayState, 0, sizeof(vplay_event_info_t));
    if (!strcmp(ps8Name, EVENT_VPLAY))
    {
        memcpy(&stVplayState, (vplay_event_info_t *)pArgs,\
           sizeof(vplay_event_info_t));
        printf("vplay event type: %d, buf:%s\n",\
            stVplayState.type, stVplayState.buf);

        switch (stVplayState.type)
        {
            // Recorder Event
            case VPLAY_EVENT_NEWFILE_START:
            {
                printf("Start new file recorder!\n");
                break;
            }
            case VPLAY_EVENT_NEWFILE_FINISH:
            {
                stVplayState.type = VPLAY_EVENT_NONE;
                printf("Add file:%s\n", (char *)stVplayState.buf);
                break;
            }
            case VPLAY_EVENT_VIDEO_ERROR:
            case VPLAY_EVENT_AUDIO_ERROR:
            case VPLAY_EVENT_EXTRA_ERROR:
            case VPLAY_EVENT_INTERNAL_ERROR:
            {
                printf("Fatal error, need stop recorder, type = %d!\n",\
                    stVplayState.type);
                break;
            }
            case VPLAY_EVENT_DISKIO_ERROR:
            {
                printf("Fatal error, need stop recorder, type = %d!\n",\
                    stVplayState.type);
                break;
            }
            default:
                break;
        }
    }
}

void RecorderSignalHandler(int s32Sig)
{
    char ps8Name[32] = {0};

    if ((s32Sig == SIGQUIT) || (s32Sig == SIGTERM))
    {
        printf("sigquit & sigterm ->%d\n", s32Sig);
    }
    else if (s32Sig == SIGSEGV)
    {
        prctl(PR_GET_NAME, ps8Name);
        printf("\nvrec Segmentation Fault thread -> %s <- \n", ps8Name);
        exit(1);
    }

    return;
}

void RecorderInitSignal(void)
{
    signal(SIGTERM, RecorderSignalHandler);
    signal(SIGQUIT, RecorderSignalHandler);
    signal(SIGTSTP, RecorderSignalHandler);
    signal(SIGSEGV, RecorderSignalHandler);
}

void InitRecorderDefault(VRecorderInfo *pstInfo)
{
    memset(pstInfo , 0, sizeof(VRecorderInfo));
    sprintf(pstInfo->video_channel, "enc1080p-stream");
    memset(pstInfo->videoextra_channel, 0, VPLAY_CHANNEL_MAX);
    sprintf(pstInfo->audio_channel, "default_mic");
    pstInfo->enable_gps = 0;

    pstInfo->audio_format.type        = AUDIO_CODEC_TYPE_AAC;
    pstInfo->audio_format.sample_rate = 8000;
    pstInfo->audio_format.sample_fmt  = AUDIO_SAMPLE_FMT_S16;
    pstInfo->audio_format.channels    = 2;

    pstInfo->stAudioChannelInfo.s32Channels     = 2;
    pstInfo->stAudioChannelInfo.s32BitWidth     = 32;
    pstInfo->stAudioChannelInfo.s32SamplingRate = 16000;
    pstInfo->stAudioChannelInfo.s32SampleSize   = 1024;

    pstInfo->time_segment = 300;
    pstInfo->time_backward = 0;

    strcpy(pstInfo->time_format, "%Y_%m%d_%H%M%S___");
    strcat(pstInfo->av_format, "mp4");
    strcat(pstInfo->suffix, "%ts_%l_F");
    strcat(pstInfo->dir_prefix, "/mnt/sd0/");
}
