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
 * Description: Adjust FPS and enable monochrome.
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
 * @brief Enable or disable monochrome mode
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int EnableMonochrome(const char *ps8Cam)
{
    int s32Ret = 0;

    s32Ret = camera_monochrome_enable(ps8Cam, 1);

    return s32Ret;
}
/*
 * @brief Adjust sensor FPS. It depend the below tags.
 * The tags are in /root/.ispddk/sensor0-isp-config.txt file.
 * AE_TARGET_MAX_FPS_LOCK_ENABLE  1
 * AE_TARGET_LOWLUX_GAIN_ENTER 6
 * AE_TARGET_LOWLUX_GAIN_EXIT 4
 * AE_TARGET_LOWLUX_EXPOSURE_ENTER 39500
 * CMC_SENSOR_MAX_GAIN_ACM  12
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int AdjustFps(const char *ps8Cam)
{
    int s32Ret = 0;
    cam_fps_range_t stFps;

    memset(&stFps, 0, sizeof(stFps));
    stFps.min_fps = 10;
    stFps.max_fps = 25;
    s32Ret = camera_set_fps_range(ps8Cam, &stFps);

    return s32Ret;
}
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

    s32Ret = AdjustFps(FCAM);
    //s32Ret = EnableMonochrome(FCAM);

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
