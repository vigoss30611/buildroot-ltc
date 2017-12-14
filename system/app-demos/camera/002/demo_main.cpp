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
 * Description: Time OSD and shelter demo
 *
 * Author:
 *     bin.yan <bin.yan@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/10/2017 init
 *
 * TODO:
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>

#define START_VIDEOBOXD "videoboxd ./path.json"
#define MAX_MARKER_NUM 8

/*
 * @brief check if keyboard is hit
 * @return 1: True,keyboard is hit.
 * @return 0: False,keyboard is not hit.
 */
static int IsKBHit(void)
{
    fd_set stRfds;
    struct timeval stTv;
    int s32Retval;

    FD_ZERO(&stRfds);
    // Watch stdin (fd 0) to see when it has input.
    FD_SET(0,&stRfds);
    // No wait
    stTv.tv_sec = 0;
    stTv.tv_usec = 0;

    s32Retval = select(1, &stRfds, NULL, NULL, &stTv);

    // Don't rely on the value of stTv now!
    if (s32Retval == -1) {
        perror("select()");
        return 0;
    } else if (s32Retval){
        // FD_ISSET(0, &rfds) will be true.
        return 1;
    }
    else{
        return 0;
    }

    return 0;
}

/*
 * @brief main function
 * @param s32Argc      -IN: args' count
 * @param ps8Argv      -IN: args
 * @return 0: Success.
 * @return -1: Failure.
 */
int main(int s32Argc, char** ps8Argv)
{
    system("vbctrl stop");
    if (system(START_VIDEOBOXD))
    {
        printf("Error: videobox start failed!\n");
        exit(-1);
    }

    struct font_attr stTutkAttr;
    struct termios stOrigTermAttr,stCurTermAttr;
    char s8MarkerName[32] = {0};
    char s8OvPortName[32] = {0};
    unsigned int u32Count = 0;
    unsigned int u32Width = 0;
    unsigned int u32Height = 0;
    int i;

    memset(&stTutkAttr, 0, sizeof(struct font_attr));
    /*
       * font name arial.ttf in the /usr/share/fonts/truetype/
       * directory on the demo board
       */
    sprintf((char *)stTutkAttr.ttf_name, "arial");
    // transparency 0x00--0,RGB  0xFFFFFF--white
    stTutkAttr.font_color = 0x00FFFFFF;
    // transparency 0x20--32,RGB  0x000000--black
    stTutkAttr.back_color = 0x20000000;

    memset(&stOrigTermAttr, 0, sizeof(struct termios));
    memset(&stCurTermAttr, 0, sizeof(struct termios));
    if (tcgetattr(STDIN_FILENO, &stOrigTermAttr))
    {
        perror("tcgetattr");
        exit(-1);
    }
    memcpy(&stCurTermAttr, &stOrigTermAttr, sizeof(struct termios) );
    // no ctrl key
    stCurTermAttr.c_lflag &= ~ICANON;
    // no echo
    stCurTermAttr.c_lflag &= ~ECHO;
    // receive one char at least
    stCurTermAttr.c_cc[VMIN] = 1;
    // read timeout
    stCurTermAttr.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &stCurTermAttr) != 0)
    {
        perror("tcsetattr");
        exit(-1);
    }

    // set time OSD
    marker_set_mode("marker0", "auto", "%t%Y/%M/%D  %H:%m:%S", &stTutkAttr);

    // transparency 0xFF--255,RGB  0x000000--black,full black colour
    stTutkAttr.back_color = 0xFF000000;

    while (1)
    {
        if (IsKBHit())
        {
            break;
        }

        if((u32Count % 2) == 0)
        {
            u32Width = u32Height = 200;
        }
        else
        {
            u32Width = u32Height = 100;
        }

        for (i = 1;i < MAX_MARKER_NUM;i++)
        {
            // marker1~marker7
            sprintf(s8MarkerName, "marker%d", i);

            // ispost-ov1~ispost-ov7
            sprintf(s8OvPortName, "ispost-ov%d", i);
            // resize shelter
            video_set_resolution(s8OvPortName, u32Width, u32Height);

            // String OSD(String marker) simulates shelter
            marker_set_mode(s8MarkerName, "manual", "%t%Y/%M/%D  %H:%m:%S", &stTutkAttr);
            marker_set_string(s8MarkerName, " ");

            usleep(600 * 1000);
        }

        u32Count++;
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &stOrigTermAttr) != 0)
    {
        perror("tcsetattr");
        exit(-1);
    }

    return 0;
}
