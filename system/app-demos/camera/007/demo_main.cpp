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
 * Description: YUV image through IPU ISPOST, then through IPU IDS
 *
 * Author:
 *     robert.wang <robert.wang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/23/2017 init
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
 * @brief Main function
 * @param s32Argc -IN: args' count
 * @param ps8Argv -IN: args
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

    while (1)
    {
        if(getchar() == 'q')
        {
            break;
        }
        usleep(10 * 1000);
    }
    videobox_stop();

    return s32Ret;
}
