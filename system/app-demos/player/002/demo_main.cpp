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
 * Description: Play Two H264 Stream
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

#define JSON_PATH_1080P_G1  "player_1080p_g1.json"
#define FILE_PATH           "1080p_264_30fps_2track.mp4"

int s32StartPlay = 0;

/*
 * @brief start play
 * @param pstPlayerInstance      -IN: player instance
 * @return >0: Success
 * @return <=0: Failure
 */
int PlayStream(vplay_player_inst_t **pstPlayerInstance);

/*
 * @brief stop play
 * @param pstPlayerInstance      -IN: player instance
 * @return >0: Success
 * @return <=0: Failure
 */
int Stop(vplay_player_inst_t **pstPlayerInstance);

int PlayStream(vplay_player_inst_t **pstPlayerInstance)
{
    vplay_player_info_t stPlayerInfo;
    char pu8FilePath[] = FILE_PATH;
    vplay_player_inst_t *pstPlayer = NULL;
    int s32Ret = -1, s32PlaySpeed = 0, s32AudioMute = 1;
    static int s32Cnt = 0;

    if (pstPlayerInstance == NULL)
    {
        return -1;
    }
    InitPlayerInfo(&stPlayerInfo, s32Cnt);
    ShowPlayerInfo(&stPlayerInfo);
    s32Cnt++;

    printf("new player\n");
    pstPlayer = vplay_new_player(&stPlayerInfo);
    if (pstPlayer == NULL)
    {
        printf("init vplay player error \n");
        return -1;
    }

    printf("queue file:%s\n", pu8FilePath);
    s32Ret = vplay_control_player(pstPlayer, VPLAY_PLAYER_QUEUE_FILE, pu8FilePath);
    if (s32Ret <= 0)
    {
        printf("queue %s fail!\n", pu8FilePath);
        goto error;
    }

    printf("mute audio\n");
    s32Ret = vplay_control_player(pstPlayer, VPLAY_PLAYER_MUTE_AUDIO, &s32AudioMute);
    if (s32Ret <= 0)
    {
        printf("mute audio fail!\n");
    }

    s32PlaySpeed = 1;
    printf("play video ->%d\n", s32PlaySpeed);
    s32Ret = vplay_control_player(pstPlayer,VPLAY_PLAYER_PLAY, &s32PlaySpeed);
    if (s32Ret <= 0)
    {
        printf("play file fail!\n");
        goto error;
    }

    *pstPlayerInstance = pstPlayer;
    return 1;
error:
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
    *pstPlayerInstance = NULL;
    return -1;
}

int Stop(vplay_player_inst_t **pstPlayerInstance)
{
    int s32Ret = -1;
    vplay_player_inst_t *pstPlayer = NULL;

    if ((pstPlayerInstance == NULL) || (*pstPlayerInstance == NULL))
    {
        printf("player instance error.\n");
        return -1;
    }

    pstPlayer = *pstPlayerInstance;
    printf("stop video\n");
    s32Ret = vplay_control_player(pstPlayer, VPLAY_PLAYER_STOP, NULL);
    if (s32Ret <= 0)
    {
        printf("stop player fail!\n");
    }

    if (pstPlayer)
    {
        printf("delete video\n");
        s32Ret = vplay_delete_player(pstPlayer);
        if (s32Ret <= 0)
        {
            printf("delete player fail!\n");
        }
    }
    *pstPlayerInstance = NULL;

    return s32Ret;
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
    int s32Ret = 0;
    vplay_player_inst_t *pstPlayer1 = NULL, *pstPlayer2 = NULL;

    if (videobox_repath(JSON_PATH_1080P_G1) < 0)
    {
        printf("Error: videobox start failed!\n");
        return -1;
    }
    VplayerInitSignal();
    event_register_handler(EVENT_VPLAY, 0, VplayEventHandler);

    if (PlayStream(&pstPlayer1) <= 0)
    {
        printf("Play stream error.\n");
        return -1;
    }

    if (PlayStream(&pstPlayer2) <= 0)
    {
        printf("Play second stream error.\n");
        return -1;
    }

    s32StartPlay = 1;
    while (s32StartPlay == 1)
    {
        sleep(1);
    }

    Stop(&pstPlayer1);
    Stop(&pstPlayer2);

    event_unregister_handler(EVENT_VPLAY, VplayEventHandler);

    return s32Ret;
}
