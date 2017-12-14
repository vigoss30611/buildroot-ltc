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
 * Description: Big resolution snap
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>

#define START_VIDEOBOXD "videoboxd ./path.json"
#define FCAM "isp"

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

    int s32Ret = 0;
    unsigned int u32Count = 0;
    int u32Width = 0;
    int u32Height = 0;
    char s8SnapName[128] = {0};
    struct fr_buf_info stBuf;
    int s32FileFd = -1;

    video_get_resolution("isp-out", &u32Width, &u32Height);// isp out 1280x720
    printf("get the width = %d, height = %d\n ", u32Width, u32Height);

    while (1)
    {
        if (u32Count % 2 != 0)
        {
            video_set_resolution("jpeg-in", 1920, 1088);// big resolution snap
        }
        else
        {
            video_set_resolution("jpeg-in", u32Width, u32Height);// 1280x720 snap
        }

        memset(s8SnapName, '\0', sizeof(s8SnapName));
        sprintf(s8SnapName, "%s_%d%s", "./snap_one_capture", u32Count, ".jpg");
        printf("%s\n", s8SnapName);

        s32FileFd = open(s8SnapName, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        s32Ret = camera_snap_one_shot(FCAM);// start snap
        if (s32Ret < 0)
        {
            break;
            printf("camera_snap_one_shot error:%d\n",s32Ret);
        }

        video_get_snap("jpeg-out", &stBuf);// get snap buffer

        if(-1 != s32FileFd)
        {
            printf("stBuf.size:%d\n", stBuf.size);
            s32Ret = write(s32FileFd, stBuf.virt_addr, stBuf.size);
            if(s32Ret < 0)
            {
                perror("Demo snap");
                video_put_snap("jpeg-out", &stBuf);
                camera_snap_exit(FCAM);
                continue;
            }
            close(s32FileFd);
        }
        else
        {
            printf("create file failed\n");
        }

        video_put_snap("jpeg-out", &stBuf);// release snap buffer

        u32Count++;

        usleep(50000);//for one frame not correct when continue snap no delay.

        s32Ret = camera_snap_exit(FCAM);// stop snap
        if (s32Ret < 0)
        {
            break;
            printf("camera_snap_exit error:%d\n", s32Ret);
        }

        if (u32Count == 10)
        {
            break;
        }
    }

    video_set_resolution("jpeg-in", 1920, 1088);// big resolution snap
    for (u32Count = 0; u32Count < 10; u32Count++)
    {
        memset(s8SnapName, '\0', sizeof(s8SnapName));
        s32Ret = camera_snap_one_shot(FCAM);// start snap
        if (s32Ret < 0)
        {
            break;
            printf("camera_snap_one_shot error:%d\n", s32Ret);
        }

        video_get_snap("jpeg-out", &stBuf);// get snap buffer

        sprintf(s8SnapName, "%s_%d%s", "./snap_mult_capture", u32Count, ".jpg");
        printf("%s\n", s8SnapName);

        s32FileFd = open(s8SnapName, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        if(-1 != s32FileFd)
        {
            s32Ret = write(s32FileFd, stBuf.virt_addr, stBuf.size);
            if(s32Ret < 0)
            {
                printf("write snapshot data to file fail, %d\n", s32Ret);
            }
        }
        else
        {
            printf("write file failed\n");
        }
        close(s32FileFd);

        video_put_snap("jpeg-out", &stBuf);// release snap buffer
    }
    s32Ret = camera_snap_exit(FCAM);// stop snap
    if (s32Ret < 0)
    {
        printf("camera_snap_one_shot error:%d\n", s32Ret);
    }

    return s32Ret;
 }
