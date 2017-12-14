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
 * Description: uvc set resolution for h1264 stream
 *
 * Author:
 *     ivan.zhuang <ivan.zhuang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/17/2017 init
 */


#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <pthread.h>
#include <dirent.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include <qsdk/videobox.h>
#include <guvc.h>


#define UVC_THREAD_STOPPING 0
#define UVC_THREAD_EXIT 1
#define UVC_THREAD_RUNNING 2

pthread_t g_stPthread;
char g_s8Device[128] = {"/dev/video0"};
char g_s8Channel[] = "h1264-stream";
volatile int g_s32UvcThreadState = 1;
static struct uvc_device *s_pstDev = NULL;

void UvcKillHandler(int s32Signo)
{
    UVC_PRINTF(UVC_LOG_ERR,"Exiting signo:%d\n", s32Signo);
    uvc_exit(s_pstDev);
    exit(0);
}

void UvcSignal()
{
    // Setup kill handler
    signal(SIGINT, UvcKillHandler);
    signal(SIGTERM, UvcKillHandler);
    signal(SIGHUP, UvcKillHandler);
    signal(SIGQUIT, UvcKillHandler);
    signal(SIGABRT, UvcKillHandler);
    signal(SIGKILL, UvcKillHandler);
    signal(SIGSEGV, UvcKillHandler);
}

void UvcSetUserResolution(struct uvc_device *pstDev, int s32Width, int s32Height, int s32Fps)
{
    /*Note: set resolution to videobox*/
    UVC_PRINTF(UVC_LOG_ERR,"# uvc set resolution :%dx%d@%dfps ,format=%08x\n", s32Width, s32Height, s32Fps, pstDev->fcc);

    if (pstDev->fcc == V4L2_PIX_FMT_H264){
        video_set_resolution(g_s8Channel, s32Width, s32Height);
    }
}

void *UvcLooping(void *c)
{
    struct uvc_device *pstDev;
    struct fr_buf_info stBuf = FR_BUFINITIALIZER;
    fd_set stFds;
    char s8HeaderBuf[HEADER_LENGTH] = {0};
    int s32HeaderSize = 0;
    int s32Ret;
    char *s8Channel = (char*)c;
    void *pImgData = NULL;
    int s32ImgSize = 0;
    static uint32_t s_u32Fno = 0;

    pstDev = uvc_open(g_s8Device, 0);
    if (pstDev == NULL) {
        UVC_PRINTF(UVC_LOG_ERR, " guvc open device %s failed \n", g_s8Device);
        exit(-1);
    }

    UVC_PRINTF(UVC_LOG_ERR, " #guvc open device %s, channel %s, h1264.\n", g_s8Device, s8Channel);
    s_pstDev = pstDev; /* for signal handler */

    pstDev->set_res_f = UvcSetUserResolution;

    /* Note: init first frame data */
    s32HeaderSize = video_get_header(s8Channel, s8HeaderBuf, HEADER_LENGTH);
    s32Ret = video_get_frame(s8Channel, &stBuf);
    if (!s32Ret && s32HeaderSize > 0) {

        pImgData = malloc(stBuf.size + s32HeaderSize);
        s32ImgSize = s32HeaderSize + stBuf.size;

        uvc_set_def_img(pstDev,
            pImgData,
            s32ImgSize,
            UVC_DEF_IMG_FORM_PIC);
    }

    video_put_frame(s8Channel, &stBuf);

    FD_ZERO(&stFds);
    FD_SET(pstDev->fd, &stFds);

    while (g_s32UvcThreadState == UVC_THREAD_RUNNING) {
        fd_set efds = stFds;
        fd_set wfds = stFds;

        s32Ret = select(pstDev->fd + 1, NULL, &wfds, &efds, NULL);

        if (FD_ISSET(pstDev->fd, &efds))
            uvc_events_process(pstDev);

        if (FD_ISSET(pstDev->fd, &wfds)) {
            s_u32Fno++;
            s32HeaderSize = video_get_header(s8Channel, s8HeaderBuf, HEADER_LENGTH);
            s32Ret = video_get_frame(s8Channel, &stBuf);
            if (!s32Ret) {
                s32Ret = uvc_video_process(pstDev, stBuf.virt_addr, stBuf.size, s8HeaderBuf, s32HeaderSize);
                (void)s32Ret;
            } else
                UVC_PRINTF(UVC_LOG_WARRING, "# guvc WARINNING: "
                        "video_get_frame << %d KB F:%d\n",
                        stBuf.size >> 10, s_u32Fno);
            video_put_frame(s8Channel, &stBuf);
        }
    }

    UVC_PRINTF(UVC_LOG_ERR, "# %s uvc_thread_state = %d\n", __func__, g_s32UvcThreadState);
    uvc_exit(pstDev);
    g_s32UvcThreadState = UVC_THREAD_STOPPING;

    return NULL;
}

int UvcStopServer(void)
{
    int s32Retry = 10;

    UVC_PRINTF(UVC_LOG_ERR, "#1 uvc_thread_state = %d\n", g_s32UvcThreadState);
    if (g_s32UvcThreadState ==UVC_THREAD_RUNNING) {
        g_s32UvcThreadState = UVC_THREAD_EXIT;

        /* it wait for thread exit successfully */
        while (g_s32UvcThreadState != UVC_THREAD_STOPPING && s32Retry > 0) {
            UVC_PRINTF(UVC_LOG_ERR, "#2 uvc_thread_state = %d\n", g_s32UvcThreadState);
            sleep(1);
            s32Retry --;
        }

        if (g_s32UvcThreadState == UVC_THREAD_STOPPING) {
            UVC_PRINTF(UVC_LOG_ERR, "#3 uvc_thread_state = %d\n", g_s32UvcThreadState);
            pthread_cancel(g_stPthread);
        }
    }

    if (s32Retry == 0) {
        pthread_cancel(g_stPthread);
        uvc_exit(s_pstDev);
    }

    sleep(1);
    UVC_PRINTF(UVC_LOG_ERR, "#4  uvc_thread_state = %d\n", g_s32UvcThreadState);
    return 0;
}

int FindValidNode(char *ps8DevName)
{
    struct dirent *pstDirent = NULL;
    DIR *pstDir = NULL;

    if ((pstDir = opendir("/dev")) == NULL) {
        UVC_PRINTF(UVC_LOG_ERR,"Cannot open dir dev\n");
        return -1;
    }

    while ((pstDirent = readdir(pstDir)) != NULL) {
        if (!strncmp("video", pstDirent->d_name, 4)) {
            sprintf(ps8DevName, "/dev/%s", pstDirent->d_name);
            UVC_PRINTF(UVC_LOG_ERR,"dev name %s\n", ps8DevName);
            return 0;
        }
    }

    UVC_PRINTF(UVC_LOG_ERR,"cannot find video node\n");
    return -1;
}

int UvcStartServer(char *s8Channel)
{
    int s32Ret  = 0;
    int s32RetryCnt = 0;

    if(s8Channel == NULL) {
        UVC_PRINTF(UVC_LOG_ERR, "# input channel is NULL\n");
        return -1;
    }

retry:
    FindValidNode(g_s8Device);
    s32Ret = access(g_s8Device, F_OK);
    if(s32Ret < 0 && s32RetryCnt < 10) {
        UVC_PRINTF(UVC_LOG_ERR, "# uvc access %s failed\n", g_s8Device);
        s32RetryCnt ++;
        sleep(1);
        goto retry;
    }

    if (s32RetryCnt >= 10) {
        UVC_PRINTF(UVC_LOG_ERR, "# uvc access %s retry failed \n", g_s8Device);
        return -3;
    }

    pthread_create(&g_stPthread, NULL, UvcLooping, s8Channel);

    UVC_PRINTF(UVC_LOG_INFO, "# UvcStartServer finished\n");
    sleep(1);
    return 0;
}

int ExecShellCmd(const char * s8Cmd)
{
    FILE *pstFile = NULL;

    printf("%s\n", s8Cmd);
    pstFile=popen(s8Cmd, "r");
    if (!pstFile) {
        printf("popen error \n");
        return -1;
    }

    pclose(pstFile);
    return 0;
}

/*
 *    @brief main function
 *    @param none
 *    @return 0: success
 */
int main(int argc, char *argv[])
{
    char s8Cmd[256] = {0};
    int s32Ret;

    UvcSignal();

    videobox_repath("./path.json");
    uvc_set_log_level(UVC_LOG_TRACE);
    g_log_level = UVC_LOG_TRACE;

    ExecShellCmd("echo 0 > /sys/class/infotm_usb/infotm0/enable");
    snprintf(s8Cmd, sizeof(s8Cmd),
        "echo \"%s\" > /sys/class/infotm_usb/infotm0/functions",
        "uvc");
    ExecShellCmd(s8Cmd);
    ExecShellCmd("echo 1 > /sys/class/infotm_usb/infotm0/enable");

    UVC_PRINTF(UVC_LOG_ERR, "# UvcStartServer .\n");
    g_s32UvcThreadState = UVC_THREAD_RUNNING;
    s32Ret = UvcStartServer(g_s8Channel);
    if (s32Ret < 0)
        goto err;

    sleep(1000);
    UvcStopServer();

err:
    videobox_stop();
    return 0;
}

