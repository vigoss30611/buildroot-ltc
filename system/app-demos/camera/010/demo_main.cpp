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
 * Description: ispost 1p & 2p & 4p switch
 *
 * Author:
 *     minghui.zhao <minghui.zhao@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/24/2017 init
 *
 * TODO:
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qsdk/videobox.h>

#define VIDEOBOX_PATH_JSON "./path.json"
#define FCAM "ispost"

int32_t show_2p_function();
int32_t show_4p_function();
#define SHOW_4P

int main(void)
{
    int s32Retval = 0;

    s32Retval = videobox_repath(VIDEOBOX_PATH_JSON);

#ifdef SHOW_4P
    sleep(4);

    s32Retval = show_4p_function();
    if (s32Retval != VBOK) {
        printf("show_4p_function failed\n");
    }
#else
    sleep(4);

    s32Retval = show_2p_function();
    if (s32Retval != VBOK) {
        printf("show_2p_function failed\n");
    }
#endif
    return s32Retval;
}

int32_t show_2p_function()
{
    int s32Retval = 0;
    cam_fcefile_param_t param;
    cam_fcefile_mode_t *mode_list;

    memset(&param, 0, sizeof(param));

    param.mode_number = 1;
    mode_list = param.mode_list;
    mode_list->file_number = 2;

    sprintf(mode_list->file_list[0].file_grid, "./lc_up_2p360_0-0_hermite32_1088x1088-1920x544_fisheyes_panorama_0.bin");
    sprintf(mode_list->file_list[1].file_grid, "./lc_up_2p360_0-0_hermite32_1088x1088-1920x544_fisheyes_panorama_1.bin");

    s32Retval = camera_load_and_fire_fcedata(FCAM, &param);

    return s32Retval;
}

int32_t show_4p_function()
{
    int s32Retval = 0;
    cam_fcefile_param_t param;
    cam_fcefile_mode_t *mode_list;

    memset(&param, 0, sizeof(param));

    param.mode_number = 1;
    mode_list = param.mode_list;
    mode_list->file_number = 4;

    sprintf(mode_list->file_list[0].file_grid, "./lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_0.bin");
    sprintf(mode_list->file_list[1].file_grid, "./lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_1.bin");
    sprintf(mode_list->file_list[2].file_grid, "./lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_2.bin");
    sprintf(mode_list->file_list[3].file_grid, "./lc_down_view_0-0_hermite32_1088x1088-960x544_fisheyes_downview_3.bin");

    s32Retval = camera_load_and_fire_fcedata(FCAM, &param);

    return s32Retval;
}


