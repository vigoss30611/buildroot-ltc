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
 * Description: Player Set Video Filter
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
#include "player_common.h"

#define JSON_PATH_1080P_G1  "player_320_g1.json"
#define FILE_PATH           "1080p_264_30fps_2track.mp4"

int s32StartPlay = 0;

/*
 * @brief main function
 * @param s32Argc      -IN: args' count
 * @param ps8Argv      -IN: args
 * @return 0: Success
 * @return -1: Failure
 */
int main(int s32Argc, char *ps8Argv[])
{
    int s32Ret = 0, s32PlaySpeed = 0, s32VideoFilter = 0;
    char pu8FilePath[] = FILE_PATH;
    vplay_player_inst_t *pstPlayer = NULL;
    vplay_player_info_t stPlayerInfo;

    if (videobox_repath(JSON_PATH_1080P_G1) < 0)
    {
        printf("Error: videobox start failed!\n");
        return -1;
    }
    VplayerInitSignal();
    event_register_handler(EVENT_VPLAY, 0, VplayEventHandler);

    InitPlayerInfo(&stPlayerInfo, 0);
    ShowPlayerInfo(&stPlayerInfo);

    printf("new player\n");
    pstPlayer = vplay_new_player(&stPlayerInfo);
    if (pstPlayer == NULL)
    {
        printf("init vplay player error \n");
        goto end;
    }

    printf("queue file:%s\n", pu8FilePath);
    s32Ret = vplay_control_player(pstPlayer, VPLAY_PLAYER_QUEUE_FILE, pu8FilePath);
    if (s32Ret <= 0)
    {
        printf("queue %s fail!\n", pu8FilePath);
        goto end;
    }

    s32VideoFilter = 1;
    printf("set video filter->%d\n", s32VideoFilter);
    s32Ret = vplay_control_player(pstPlayer,VPLAY_PLAYER_SET_VIDEO_FILTER, &s32VideoFilter);
    if (s32Ret <= 0)
    {
        printf("set video filter fail!\n");
        goto end;
    }

    s32PlaySpeed = 1;
    printf("play video ->%d\n", s32PlaySpeed);
    s32Ret = vplay_control_player(pstPlayer,VPLAY_PLAYER_PLAY, &s32PlaySpeed);
    if (s32Ret <= 0)
    {
        printf("play file fail!\n");
        goto end;
    }

    s32StartPlay = 1;
    while (s32StartPlay == 1)
    {
        sleep(1);
    }

    printf("stop video\n");
    s32Ret = vplay_control_player(pstPlayer, VPLAY_PLAYER_STOP, NULL);
    if (s32Ret <= 0)
    {
        printf("stop player fail!\n");
    }

end:
    if (pstPlayer)
    {
        printf("delete video\n");
        s32Ret = vplay_delete_player(pstPlayer);
        if (s32Ret <= 0)
        {
            printf("delete player fail!\n");
        }
    }
    pstPlayer = NULL;

    event_unregister_handler(EVENT_VPLAY, VplayEventHandler);

    return s32Ret;
}
