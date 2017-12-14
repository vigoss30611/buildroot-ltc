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
 * Description:  ISP mirror image out to ISPost& IDS demo
 *
 * Author:
 *     davinci.liang <davinci.liang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/22/2017 init
 *
 * TODO:
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qsdk/videobox.h>

#define ISP_CHANNEL "isp"
#define VIDEOBOX_PATH_JSON "./path.json"

/*
* @brief main function
* @param  void
* @return 0: Success
* @return -1: Failure
*/
int main(void)
{
    int32_t s32Ret = 0;

    // 1. re-setting path.json
    s32Ret = videobox_repath(VIDEOBOX_PATH_JSON);
    if (s32Ret != VBOK)
    {
        printf("videobox_repath %s failed ret:%d\n", VIDEOBOX_PATH_JSON, s32Ret);
    }

    sleep(2);

    // 2. set sensor horizontal mirror.
    printf("set sensor mirror %s order\n", "H");

    s32Ret = camera_set_mirror(ISP_CHANNEL, CAM_MIRROR_H);
    if (s32Ret != VBOK)
    {
        printf("camera_set_mirror %s failed ret:%d\n", "H", s32Ret);
    }

    sleep(2);

    // 3. set sensor vertical mirror.
    printf("set sensor mirror %s order\n", "V");

    s32Ret = camera_set_mirror(ISP_CHANNEL, CAM_MIRROR_V);
    if (s32Ret != VBOK)
    {
        printf("camera_set_mirror %s failed ret:%d\n", "V", s32Ret);
    }

    sleep(2);

    // 4. set sensor horizontal and vertical mirror.
    printf("set sensor mirror %s order\n", "HV");

    s32Ret = camera_set_mirror(ISP_CHANNEL, CAM_MIRROR_HV);
    if (s32Ret != VBOK)
    {
        printf("camera_set_mirror %s failed ret:%d\n", "HV", s32Ret);
    }

    sleep(2);

    // 5. set sensor none mirror.
    printf("set sensor mirror %s order\n", "NONE");

    s32Ret = camera_set_mirror(ISP_CHANNEL, CAM_MIRROR_NONE);
    if (s32Ret != VBOK)
    {
        printf("camera_set_mirror %s failed ret:%d\n", "Def", s32Ret);
    }

    return s32Ret;
}
