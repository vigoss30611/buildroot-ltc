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
 * Description: Adjust wb, yuv gamma, sharpness, brightness, saturation, contrast
 * hue and disable ae.
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
 * @brief Adjust WB.
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int AdjustWb(const char *ps8Cam)
{
    int s32Ret = 0;

    s32Ret = camera_set_wb(ps8Cam, CAM_WB_2500K);

    return s32Ret;
}
/*
 * @brief Disable AE.
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int EnableAe(const char *ps8Cam)
{
    int s32Ret = 0;

    s32Ret = camera_ae_enable(ps8Cam, 0);

    return s32Ret;
}
/*
 * @brief Adjust gamma.
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int AdjustYuvGamma(const char *ps8Cam)
{
    int s32Ret = 0;
    float fCurve[] =
    {
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0
    };
    cam_yuv_gamma_attr_t stAttr;

    memset(&stAttr, 0, sizeof(stAttr));
    stAttr.cmn.mdl_en = 1;
    stAttr.cmn.mode = CAM_CMN_MANUAL;
    memcpy(stAttr.curve, fCurve, sizeof(fCurve));
    s32Ret = camera_set_yuv_gamma_attr(ps8Cam, &stAttr);

    return s32Ret;
}
/*
 * @brief Adjust sharpness. It depend the below tag.
 * The tag are in /root/.ispddk/sensor0-isp-config.txt file.
 * TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE  1
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int AdjustSharpness(const char *ps8Cam)
{
    int s32Ret = 0;

    s32Ret = camera_set_sharpness(ps8Cam, 160);

    return s32Ret;
}
/*
 * @brief Adjust brightness. It depend the below tag.
 * The tag are in /root/.ispddk/sensor0-isp-config.txt file.
 * TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE  1
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int AdjustBrightness(const char *ps8Cam)
{
    int s32Ret = 0;

    s32Ret = camera_set_brightness(ps8Cam, 160);

    return s32Ret;
}
/*
 * @brief Adjust saturation. It depend the below tag.
 * The tag are in /root/.ispddk/sensor0-isp-config.txt file.
 * CMC_ENABLE 1
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int AdjustSaturation(const char *ps8Cam)
{
    int s32Ret = 0;

    s32Ret = camera_set_saturation(ps8Cam, 160);

    return s32Ret;
}
/*
 * @brief Adjust contrast. It depend the below tag.
 * The tag are in /root/.ispddk/sensor0-isp-config.txt file.
 * TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE  1
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int AdjustContrast(const char *ps8Cam)
{
    int s32Ret = 0;

    s32Ret = camera_set_contrast(ps8Cam, 160);

    return s32Ret;
}
/*
 * @brief Adjust hue
 * @param ps8Cam -IN: ISP IPU instance name
 * @return 0: Success.
 * @return -1: Failure.
 */
int AdjustHue(const char *ps8Cam)
{
    int s32Ret = 0;

    s32Ret = camera_set_hue(ps8Cam, 220);

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

    //s32Ret = AdjustWb(FCAM);
    //s32Ret = EnableAe(FCAM);
    //s32Ret = AdjustYuvGamma(FCAM);
    //s32Ret = AdjustSharpness(FCAM);
    //s32Ret = AdjustBrightness(FCAM);
    //s32Ret = AdjustSaturation(FCAM);
    //s32Ret = AdjustContrast(FCAM);
    s32Ret = AdjustHue(FCAM);

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
