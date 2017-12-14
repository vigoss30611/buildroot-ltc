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
 * Description: Face detection demo on q3f board
 *
 * Author:
 *     devin.zhu <devin.zhu@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/14/2017 init
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <qsdk/event.h>
#include <qsdk/videobox.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define LINEWIDTH 3
#define MAX_SCAN_NUM 10
#define RGB32BIT(a, r, g, b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

typedef struct fodet
{
    void *pFb;
    int32_t s32Osd1Fd;
    uint32_t u32ScreenSize;
    uint32_t u32Xres;
    uint32_t u32Yres;
} ST_FODET;

typedef struct event_fod
{
    uint32_t u32Num;
    uint32_t u32X[MAX_SCAN_NUM];
    uint32_t u32Y[MAX_SCAN_NUM];
    uint32_t u32W[MAX_SCAN_NUM];
    uint32_t u32H[MAX_SCAN_NUM];
} ST_EVENT_FOD;

ST_FODET g_stFodet;
static struct itimerval s_stOldtv;


 /*
  * @brief Draw rectangle
  * @param ps32Ptr   -IN: osd1 fd pointer
  * @param u32X      -IN: rectangle left top x cordinate
  * @param u32Y      -IN: rectangle left top y cordinate
  * @param u32W      -IN: rectangle width
  * @param u32H      -IN: rectangle height
  * @param u32Color  -IN: rectangle color
  * @return void
  */
static void DrawRectangle (int *ps32Ptr, uint32_t u32X, uint32_t u32Y,
         uint32_t u32W, uint32_t u32H, uint32_t u32Color)
{
    uint32_t i, j;

    for (i = 0; i < u32W; i++)
    {
        for (j = 0; j < LINEWIDTH; j++)
        {
            if (((u32Y + j) * g_stFodet.u32Xres + u32X + i)
                <= g_stFodet.u32Xres * g_stFodet.u32Yres -1)
            {
                *(ps32Ptr + (u32Y + j)*g_stFodet.u32Xres + u32X + i)
                    = RGB32BIT(u32Color >> 24, (u32Color >> 16) & 0xff,
                        (u32Color >> 8) & 0xff, u32Color & 0xff);
            }
            if (((u32Y + j + u32H)*g_stFodet.u32Xres + u32X + i)
                <= g_stFodet.u32Xres * g_stFodet.u32Yres -1)
            {
                *(ps32Ptr + (u32Y + j + u32H)*g_stFodet.u32Xres + u32X + i)
                    = RGB32BIT(u32Color >> 24, (u32Color >> 16) & 0xff,
                        (u32Color >> 8) & 0xff, u32Color & 0xff);
            }
        }
    }

    for (i = 0; i < u32H; i++)
    {
        for (j = 0; j < LINEWIDTH; j++)
        {
            if (((u32Y + i) * g_stFodet.u32Xres + u32X + u32W + j)
                <= g_stFodet.u32Xres * g_stFodet.u32Yres -1)
            {
                *(ps32Ptr + (u32Y + i)*g_stFodet.u32Xres + u32X + u32W + j)
                    = RGB32BIT(u32Color >> 24, (u32Color >> 16) & 0xff,
                        (u32Color >> 8) & 0xff, u32Color & 0xff);
            }
            if (((u32Y + i)*g_stFodet.u32Xres + u32X + j)
                <= g_stFodet.u32Xres * g_stFodet.u32Yres -1)
            {
                *(ps32Ptr + (u32Y + i)*g_stFodet.u32Xres + u32X + j)
                    = RGB32BIT(u32Color >> 24, (u32Color >> 16) & 0xff,
                        (u32Color >> 8) & 0xff, u32Color & 0xff);
            }
        }
    }
}

/*
 * @brief Fodet event handler function
 * @param ps8Event  -IN: event type
 * @param pArg      -IN: event data
 * @return void
 */
static void EventFodetHandler(char *ps8Event, void *pArg)
{
    uint32_t i = 0;
    struct itimerval stItv;
    ST_EVENT_FOD stEvent;

    if (!strncmp(ps8Event, EVENT_FODET, strlen(EVENT_FODET)))
    {
        memcpy(&stEvent, (struct event_fodet *)pArg, sizeof(struct event_fodet));
        memset((int *)g_stFodet.pFb, 0x01, g_stFodet.u32ScreenSize);
        stItv.it_interval.tv_sec = 10;
        stItv.it_interval.tv_usec = 0;
        stItv.it_value.tv_sec = 10;
        stItv.it_value.tv_usec = 0;
        setitimer(ITIMER_REAL, &stItv, &s_stOldtv);

        for (i=0; i < stEvent.u32Num; i++)
        {
            printf("num:%d, x:%d, y:%d, w:%d, h:%d\n", stEvent.u32Num, stEvent.u32X[i], stEvent.u32Y[i], stEvent.u32W[i], stEvent.u32H[i]);
            DrawRectangle((int32_t *)g_stFodet.pFb, stEvent.u32X[i], stEvent.u32Y[i], stEvent.u32W[i], stEvent.u32H[i], 0xff00);
        }
    }
 }

/*
 * @brief Deal with osd1 and get frame buffer
 * @param void
 * @return void
 */
static int InitFrameBuffer()
{
    struct fb_var_screeninfo stVinfo;
    int32_t s32Ret = 0;

    g_stFodet.s32Osd1Fd = open("/dev/fb1", O_RDWR);
    if (g_stFodet.s32Osd1Fd < 0)
    {
        s32Ret = -1;
        printf("open /dev/fb1 failed\n");
        return s32Ret;
    }

    ioctl(g_stFodet.s32Osd1Fd, FBIOGET_VSCREENINFO, &stVinfo);

    g_stFodet.u32Xres = stVinfo.xres;
    g_stFodet.u32Yres = stVinfo.yres;
    g_stFodet.u32ScreenSize = stVinfo.xres * stVinfo.yres * stVinfo.bits_per_pixel / 8;
    g_stFodet.pFb = mmap(0, g_stFodet.u32ScreenSize, PROT_READ|PROT_WRITE, MAP_SHARED, g_stFodet.s32Osd1Fd, 0);

    if (g_stFodet.pFb == MAP_FAILED)
    {
        printf("Error: failed to map framebuffer device to memory.\n");
        s32Ret = -1;
        close(g_stFodet.s32Osd1Fd);
        return s32Ret;
    }

    stVinfo.yoffset = 0;
    ioctl(g_stFodet.s32Osd1Fd, FBIOPAN_DISPLAY, &stVinfo);

    return s32Ret;
}

/*
 * @brief Free resource of frame buffer
 * @param void
 * @return void
 */
static void FreeFrameBuffer()
{
    close(g_stFodet.s32Osd1Fd);
}

/*
 * @brief Signal(SIGINT) handler function for exiting ap process
 * @param u32SigNum   -IN: signal type
 * @return void
 */
static void KillHandler(int32_t u32SigNum)
{
    event_unregister_handler(EVENT_FODET, EventFodetHandler);

    FreeFrameBuffer();
    videobox_stop();
    exit(0);
}

/*
 * @brief Signal(SIGALRM) handler function for clearing detection rectangle
 * @param u32SigNum   -IN: signal type
 * @return void
 */
static void AlarmHandler(int32_t u32SigNum)
{
    memset((int32_t *)g_stFodet.pFb, 0x01, g_stFodet.u32ScreenSize);
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

    videobox_repath("./path.json");
    signal(SIGINT, KillHandler);
    signal(SIGALRM, AlarmHandler);
    event_register_handler(EVENT_FODET, 0, EventFodetHandler);

    s32Ret = InitFrameBuffer();
    if (s32Ret < 0)
    {
        return s32Ret;
    }

    while(1)
    {
        usleep(20000);
    }
 }

