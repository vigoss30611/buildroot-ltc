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
 * Description: osd colorkey blending demo
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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <qsdk/videobox.h>
#include "fb.h"

#define VIDEOBOX_PATH_JSON "./path.json"
#define OSD1_DEV_NODE "/dev/fb1"

static const int g_s32ColorKeyDefValue = 0x01010101;
static const int g_s32ColorRedValue = 0x000000FF;

typedef struct rectangle_region {
    int *ps32Buf;
    int s32XOffset;
    int s32YOffset;
    int s32Width;
    int s32Height;
    int s32Stride;
} ST_RECTANGLE_REGION;
typedef struct fb_var_screeninfo ST_FB_VAR_SCREENINFO;

/*
* @brief set osd1 frame buffer context function
* @param  ps32Buf - OUT : memory will handle buffer
* @param s32Value - IN: memory will set value
* @param s32Size - IN: buffer size
* @return void
*/
static void Memset32(int *ps32Buf, int s32Value, int s32Size)
{
    int i = 0;
    for (i = 0; i < s32Size; i++)
    {
        ps32Buf[i] = s32Value;
    }
}

/*
* @brief set osd1 frame buffer context function
* @param  pstRectRegion - IN : rectangle in screen postion(FrameBuffer)
* @return void
*/
static void DrawRectangleColorKey(ST_RECTANGLE_REGION* pstRectRegion)
{
    int i,j;
    int *ps32BufStart = pstRectRegion->ps32Buf + \
        pstRectRegion->s32YOffset * pstRectRegion->s32Stride + \
        pstRectRegion->s32XOffset;

    for (i = 0; i < pstRectRegion->s32Height; i++)
    {
        for (j= 0; j < pstRectRegion->s32Width; j++)
        {
            ps32BufStart[j] = g_s32ColorKeyDefValue;
        }
        ps32BufStart += pstRectRegion->s32Stride;
    }
}

/*
* @brief test case color key blending mode
* @param  void
* @return void
*/
void TestOSDColorKeyBlending(void)
{
    int s32FdOsd1, s32Size;
    int *ps32Buf = NULL;
    ST_FB_VAR_SCREENINFO stScreenInfo;
    ST_RECTANGLE_REGION stRectRegion;

    s32FdOsd1 = open(OSD1_DEV_NODE, O_RDWR);
    if (s32FdOsd1 < 0)
    {
        printf("can not open %s devices!\n", OSD1_DEV_NODE);
        return ;
    }

    // get screen infomation form hardware
    ioctl(s32FdOsd1, FBIOGET_VSCREENINFO, &stScreenInfo);
    printf("osd1 info: bpp%d, std%d, x%d, y%d, vx%d, vy%d\n",
         stScreenInfo.bits_per_pixel, stScreenInfo.nonstd,
         stScreenInfo.xres, stScreenInfo.yres,
         stScreenInfo.xres_virtual, stScreenInfo.yres_virtual);

    s32Size = stScreenInfo.xres * stScreenInfo.yres * 4;
    ps32Buf = (int*)malloc(s32Size);

    // It set buffer backgroad to red
    Memset32(ps32Buf, g_s32ColorRedValue, s32Size/4);

    // It will draw rectangle in frame
    stRectRegion.ps32Buf = ps32Buf;
    stRectRegion.s32XOffset = 30;
    stRectRegion.s32YOffset = 30;
    stRectRegion.s32Width = stScreenInfo.xres-60;
    stRectRegion.s32Height = stScreenInfo.yres -60;
    stRectRegion.s32Stride =  stScreenInfo.xres;
    DrawRectangleColorKey(&stRectRegion);

    write(s32FdOsd1, (char *)ps32Buf, s32Size);
    free(ps32Buf);

    close(s32FdOsd1);
}

/*
* @brief main function
* @param  void
* @return 0: Success
* @return -1: Failure
*/
int main(void)
{
    int32_t s32Ret = 0;

    s32Ret = videobox_repath(VIDEOBOX_PATH_JSON);
    TestOSDColorKeyBlending();
    return s32Ret;
}
