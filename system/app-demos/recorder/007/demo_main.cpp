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
 * Description: Recorder Set Container.
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
#include "recorder_common.h"

#define JSON_PATH_265       "video_enc_h2.json"

typedef enum _container_type
{
    EN_MP4_CONTAINER = 0,
    EN_MOV_CONTAINER,
    EN_MKV_CONTAINER,
    EN_MAX_CONTAINER
} EN_CONTAINER_TYPE;

/*
 * @brief   show usage of programmer
 * @param s8ProgramName -IN: programmer name
 * @return void
 */
void RecorderUsage(char *s8ProgramName);

/*
 * @brief start recode stream
 * @param  pstInfo      -IN: recorder info
 * @return void
 */
void RecodeStream(VRecorderInfo *pstInfo);

void RecorderUsage(char *s8ProgramName)
{
    printf("Usage : %s ContainerType\n", s8ProgramName);
    printf("ContainerType:\n");
    printf("\t 0: MP4\n");
    printf("\t 1: MOV\n");
    printf("\t 2: MKV\n");
}

void RecodeStream(VRecorderInfo *pstInfo)
{
    VRecorder *pstRecorder = NULL;
    time_t s32StartTime;
    time_t s32Now;

    pstRecorder = vplay_new_recorder(pstInfo);
    if (pstRecorder == NULL)
    {
        printf("new recorder error\n");
        return;
    }

    if (vplay_control_recorder(pstRecorder, VPLAY_RECORDER_START, NULL) <= 0)
    {
        printf("start recorder error\n");
        goto end;
    }

    time(&s32StartTime);
    s32Now = s32StartTime;
    printf("start recorder ok:%ld\n", s32StartTime);
    while (s32Now - s32StartTime <= RECODE_TIME)
    {
        sleep(1);
        time(&s32Now);
    }

    printf("reached reocde time, and stop recode.(%d)\n", RECODE_TIME);
    if (vplay_control_recorder(pstRecorder, VPLAY_RECORDER_STOP, NULL) <= 0)
    {
        printf("stop recorder error\n");
    }

end:
    if (pstRecorder)
    {
        printf("stop recorder ok\n");
        if (vplay_delete_recorder(pstRecorder) <= 0)
        {
            printf("delete recorder error\n");
            return;
        }
    }
    pstRecorder = NULL;
}

/*
 * @brief main function
 * @param s32Argc      -IN: args' count
 * @param ps8Argv      -IN: args
 * @return 0: Success
 * @return -1: Failure
 */
int main(int s32Argc, char *ps8Argv[])
{
    VRecorderInfo stRecorderInfo;
    int s32ContainerType = 0;

    if (s32Argc < 2)
    {
        RecorderUsage(ps8Argv[0]);
        return -1;
    }

    s32ContainerType = atoi(ps8Argv[1]);
    if ((s32ContainerType < 0) || (s32ContainerType >= EN_MAX_CONTAINER))
    {
        printf("Set container error\n");
        RecorderUsage(ps8Argv[0]);
        return -1;
    }

    if (videobox_repath(JSON_PATH_265) < 0)
    {
        printf("Error: videobox start failed!\n");
        return -1;
    }

    RecorderInitSignal();
    event_register_handler(EVENT_VPLAY, 0, RecorderEventHandler);

    InitRecorderDefault(&stRecorderInfo);
    switch (s32ContainerType)
    {
        case EN_MP4_CONTAINER:
        {
            snprintf(stRecorderInfo.av_format, VPLAY_CHANNEL_MAX, "mp4");
            break;
        }
        case EN_MOV_CONTAINER:
        {
            snprintf(stRecorderInfo.av_format, VPLAY_CHANNEL_MAX, "mov");
            break;
        }
        case EN_MKV_CONTAINER:
        {
            snprintf(stRecorderInfo.av_format, VPLAY_CHANNEL_MAX, "mkv");
            break;
        }
        default:
            break;
    }
    RecodeStream(&stRecorderInfo);

    event_unregister_handler(EVENT_VPLAY, RecorderEventHandler);

    return 0;
}
