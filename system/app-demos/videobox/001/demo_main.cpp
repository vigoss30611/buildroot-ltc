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
 * Description: start videobox through api demo
 *
 * Author:
 *     beca.zhang <beca.zhang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  11/08/2017 init
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <qsdk/videobox.h>

/*
 * @brief main function
 * @param s32Argc      -IN: args' count
 * @param ps8Argv      -IN: args
 * @return 0: Success.
 * @return -1: Failure.
 */
int main(int s32Argc, char** ps8Argv)
{
    ST_VIDEOBOX_PORT_ATTRIBUTE stPortAttr;
    int s32PropertyValue = 0;
    int s32Ret = 0;

    s32Ret = videobox_create_path();
    if(s32Ret == -1) {
        printf("Error, videobox path create failed: %d\n", s32Ret);
        return -1;
    } else if(s32Ret == VBEALREADYUP) {
        printf("Warning, vieobox is already running\n");
        return -1;
    }

    //isp
    s32PropertyValue = 3;
    memset(&stPortAttr, 0, sizeof(stPortAttr));
    videobox_ipu_create((char*)"v2500-0");
    videobox_ipu_set_property((char*)"v2500-0", E_VIDEOBOX_ISP_NBUFFERS, &s32PropertyValue, sizeof(int));
    s32PropertyValue = 1;
    videobox_ipu_set_property((char*)"v2500-0", E_VIDEOBOX_ISP_SKIPFRAME, &s32PropertyValue, sizeof(int));
    stPortAttr.u32Width = 1920;
    stPortAttr.u32Height = 1088;
    strncpy(stPortAttr.s8PixelFormat, "nv12", 5);
    videobox_ipu_set_attribute((char*)"v2500-0",(char*)"out" , &stPortAttr);
    memset(&stPortAttr.s8PixelFormat, 0, sizeof(stPortAttr.s8PixelFormat));
    videobox_ipu_set_attribute((char*)"v2500-0",(char*)"cap" , &stPortAttr);

    //marker
    videobox_ipu_create((char*)"marker-0");
    videobox_ipu_set_property((char*)"marker-0", E_VIDEOBOX_MARKER_MODE, (void*)"nv12", sizeof("nv12"));
    memset(&stPortAttr, 0, sizeof(stPortAttr));
    stPortAttr.u32Width = 800;
    stPortAttr.u32Height = 64;
    strncpy(stPortAttr.s8PixelFormat, "nv12", 5);
    videobox_ipu_set_attribute((char*)"marker-0",(char*)"out" , &stPortAttr);
    memset(&stPortAttr, 0, sizeof(stPortAttr));

    //ispost
    videobox_ipu_create((char*)"ispost-0");
    videobox_ipu_bind((char*)"v2500-0", (char*)"out", (char*)"ispost-0", (char*)"in");
    videobox_ipu_bind((char*)"marker-0", (char*)"out", (char*)"ispost-0", (char*)"ol");
    s32PropertyValue = 1;
    videobox_ipu_set_property((char*)"ispost-0", E_VIDEOBOX_ISPOST_DNENABLE, &s32PropertyValue, sizeof(int));
    s32PropertyValue = 0;
    videobox_ipu_set_property((char*)"ispost-0", E_VIDEOBOX_ISPOST_DNTARGETINDEX, &s32PropertyValue, sizeof(int));
    videobox_ipu_set_property((char*)"ispost-0", E_VIDEOBOX_ISPOST_LCGRIDFILENAME1, 
            (void*)"/root/.ispost/lc_v1_hermite32_1920x1088_scup_0~30.bin",
            sizeof("root/.ispost/lc_v1_hermite32_1920x1088_scup_0~30.bin")
            );
    memset(&stPortAttr, 0, sizeof(stPortAttr));
    stPortAttr.u32PiPWidth = 576;
    stPortAttr.u32PipHeight = 960;
    videobox_ipu_set_attribute((char*)"ispost-0",(char*)"ol" , &stPortAttr);
    memset(&stPortAttr, 0, sizeof(stPortAttr));
    stPortAttr.u32Width = 1920;
    stPortAttr.u32Height = 1088;
    strncpy(stPortAttr.s8PixelFormat, "nv12", 5);
    videobox_ipu_set_attribute((char*)"ispost-0",(char*)"dn" , &stPortAttr);
    videobox_ipu_bind((char*)"ispost-0", (char*)"dn", (char*)"vencoder-h21080p", (char*)"frame");
    videobox_ipu_bind((char*)"ispost-0", (char*)"dn", (char*)"ids-0", (char*)"osd0");
    stPortAttr.u32Width = 1280;
    stPortAttr.u32Height = 720;
    videobox_ipu_set_attribute((char*)"ispost-0",(char*)"ss0" , &stPortAttr);
    videobox_ipu_bind((char*)"ispost-0", (char*)"ss0", (char*)"vencoder-h2720p", (char*)"frame");
    videobox_ipu_bind((char*)"ispost-0", (char*)"ss0", (char*)"vencoder-h2record", (char*)"frame");
    stPortAttr.u32Width = 640;
    stPortAttr.u32Height = 360;
    videobox_ipu_set_attribute((char*)"ispost-0",(char*)"ss1" , &stPortAttr);
    videobox_ipu_bind((char*)"ispost-0", (char*)"ss1", (char*)"vencoder-h2vga", (char*)"frame");

    //jpeg
    videobox_ipu_create((char*)"h1jpeg-0");
    videobox_ipu_bind((char*)"v2500-0", (char*)"cap", (char*)"h1jpeg-0", (char*)"in");
    videobox_ipu_set_property((char*)"h1jpeg-0", E_VIDEOBOX_H1JPEG_MODE, (void*)"trigger", sizeof("trigger"));

    //enc720p
    videobox_ipu_create((char*)"vencoder-h2720p");
    videobox_ipu_set_property((char*)"vencoder-h2720p", E_VIDEOBOX_VENCODER_ENCODERTYPE, (void*)"h265", sizeof("h265"));
    ST_VIDEO_RATE_CTRL_INFO stRateInfo;
    memset(&stRateInfo, 0, sizeof(ST_VIDEO_RATE_CTRL_INFO));
    stRateInfo.rc_mode = VENC_CBR_MODE;
    stRateInfo.cbr.qp_max = 40;
    videobox_ipu_set_property((char*)"vencoder-h2720p", E_VIDEOBOX_VENCODER_RATECTRL, &stRateInfo, sizeof(ST_VIDEO_RATE_CTRL_INFO));
    memset(&stRateInfo, 0, sizeof(ST_VIDEO_RATE_CTRL_INFO));
    videobox_ipu_get_property((char*)"vencoder-h2720p", E_VIDEOBOX_VENCODER_RATECTRL, &stRateInfo, sizeof(ST_VIDEO_RATE_CTRL_INFO));
    printf("get qp_max: %d\n", stRateInfo.cbr.qp_max);

    //enc1080p
    videobox_ipu_create((char*)"vencoder-h21080p");
    videobox_ipu_set_property((char*)"vencoder-h21080p", E_VIDEOBOX_VENCODER_ENCODERTYPE, (void*)"h265", sizeof("h265"));

    //encvga
    videobox_ipu_create((char*)"vencoder-h2vga");
    videobox_ipu_set_property((char*)"vencoder-h2vga", E_VIDEOBOX_VENCODER_ENCODERTYPE, (void*)"h265", sizeof("h265"));

    //encrecord
    videobox_ipu_create((char*)"vencoder-h2record");
    videobox_ipu_set_property((char*)"vencoder-h2record", E_VIDEOBOX_VENCODER_ENCODERTYPE, (void*)"h265", sizeof("h265"));

    //display
    videobox_ipu_create((char*)"ids-0");
    s32PropertyValue = 1;
    videobox_ipu_set_property((char*)"ids-0", E_VIDEOBOX_IDS_NOOSD1, &s32PropertyValue, sizeof(int));

    //vam
    videobox_ipu_create((char*)"vamovement-0");
    videobox_ipu_bind((char*)"v2500-0", (char*)"his", (char*)"vamovement-0", (char*)"in");

    videobox_start_path();

    printf("please press 'q' key to exit this sample\n");
    while (1)
    {
        if(getchar() == 'q')
        {
            break;
        }
        usleep(10*1000);
    }

    videobox_stop_path();
    videobox_ipu_unbind((char*)"v2500-0", (char*)"out", (char*)"ispost-0", (char*)"in");
    videobox_ipu_unbind((char*)"marker-0", (char*)"out", (char*)"ispost-0", (char*)"ol");
    videobox_ipu_unbind((char*)"ispost-0", (char*)"dn", (char*)"vencoder-h21080p", (char*)"frame");
    videobox_ipu_unbind((char*)"ispost-0", (char*)"dn", (char*)"ids-0", (char*)"osd0");
    videobox_ipu_unbind((char*)"ispost-0", (char*)"ss0", (char*)"vencoder-h2720p", (char*)"frame");
    videobox_ipu_unbind((char*)"ispost-0", (char*)"ss0", (char*)"vencoder-h2record", (char*)"frame");
    videobox_ipu_unbind((char*)"ispost-0", (char*)"ss1", (char*)"vencoder-h2vga", (char*)"frame");
    videobox_ipu_unbind((char*)"v2500-0", (char*)"cap", (char*)"h1jpeg-0", (char*)"in");
    videobox_ipu_unbind((char*)"v2500-0", (char*)"his", (char*)"vamovement-0", (char*)"in");

    videobox_ipu_delete((char*)"v2500-0");
    videobox_ipu_delete((char*)"marker-0");
    videobox_ipu_delete((char*)"ispost-0");
    videobox_ipu_delete((char*)"h1jpeg-0");
    videobox_ipu_delete((char*)"vencoder-h2720p");
    videobox_ipu_delete((char*)"vencoder-h21080p");
    videobox_ipu_delete((char*)"vencoder-h2vga");
    videobox_ipu_delete((char*)"vencoder-h2record");
    videobox_ipu_delete((char*)"ids-0");
    videobox_ipu_delete((char*)"vamovement-0");

    videobox_delete_path();
    return 0;
}
