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
 * Description: Encode 720P and 480P H265 ES Stream with ISP Input Simultaneously
 *
 * Author:
 *     wission.zhang <wission.zhang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/08/2017 init
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <qsdk/videobox.h>
#include <qsdk/video.h>

#define JSON_PATH "./path.json"
#define MAX_VIDEO_HEADER_LENGTH 256
#define MAX_FILE_NAME_LENGTH 64
#define CHANNEL_NUMBER 2
static const char *s_VideoChannel[CHANNEL_NUMBER] = {"enc720p-stream", "enc480p-stream"};

typedef struct _venc_getstream
{
     bool bThreadStart;
     pthread_t s32ThreadID;
     uint32_t u32ChannelNum;
}ST_VENC_GET_STREAM;

/*
 * @brief   set video bitrate rate control parameters to video encoder
 * @param enRatectrlMode -IN: rate control type
 * @return void
 */
void VencSetRateCtrl(EN_VIDEO_RATE_CTRL_TYPE enRatectrlMode, uint32_t u32ChannelNum)
{
    int ret = -1;
    uint32_t i = 0;
    ST_VIDEO_RATE_CTRL_INFO stRatectrlInfo;

    memset(&stRatectrlInfo, 0, sizeof(stRatectrlInfo));
    for (i = 0; i < u32ChannelNum; i++)
    {
        ret = video_get_ratectrl(s_VideoChannel[i], &stRatectrlInfo);
        if (ret != 0)
        {
            printf("video_get_ratectrl Error ret:%d\n", ret);
        }
        stRatectrlInfo.gop_length = 30;
        stRatectrlInfo.idr_interval = 30;
        switch (enRatectrlMode)
        {
            case VENC_CBR_MODE:
            {
                stRatectrlInfo.rc_mode = VENC_CBR_MODE;
                stRatectrlInfo.cbr.qp_max = 47;
                stRatectrlInfo.cbr.qp_min = 21;
                stRatectrlInfo.cbr.bitrate = 0x500000;
                stRatectrlInfo.cbr.qp_delta = -1;
                stRatectrlInfo.cbr.qp_hdr = -1;
                break;
            }
            case VENC_VBR_MODE:
            {
                stRatectrlInfo.rc_mode = VENC_VBR_MODE;
                stRatectrlInfo.vbr.qp_max = 47;
                stRatectrlInfo.vbr.qp_min = 21;
                stRatectrlInfo.vbr.max_bitrate = 0x500000;
                stRatectrlInfo.vbr.qp_delta = -1;
                stRatectrlInfo.vbr.threshold = 80;
                break;
            }
            case VENC_FIXQP_MODE:
            {
                stRatectrlInfo.rc_mode = VENC_FIXQP_MODE;
                stRatectrlInfo.mbrc = 0;
                stRatectrlInfo.mb_qp_adjustment = 0;
                stRatectrlInfo.fixqp.qp_fix = 32;
                break;
            }
            default:
            {
                printf("VencSetRateCtrl Error invalid ratectrl mode:%d\n", enRatectrlMode);
                break;
            }
        }

        ret = video_set_ratectrl(s_VideoChannel[i], &stRatectrlInfo);
        if (ret != 0)
        {
            printf("video_set_ratectrl Error ret:%d\n", ret);
        }
    }
}

/*
 * @brief   show usage of programmer
 * @param s8ProgramName -IN: programmer name
 * @return void
 */
void VencUsage(char *s8ProgramName)
{
    printf("Usage : %s ratectrl_mode\n", s8ProgramName);
    printf("ratectrl_mode:\n");
    printf("\t 0:CBR mode\n");
    printf("\t 1:VBR mode\n");
    printf("\t 2:FixQP mode\n");
    printf("\t press q to exit this demo\n");
}

/*
 * @brief  Thread function that get stream from video channels and save them to file
 * @param pThreadPara -IN: thread info and video channel number.
 * @return NULL: No Care.
 */
void *VencStartStreamProc(void *pThreadPara)
{
    int ret = 0;
    int s32Width = 0, s32Height = 0;
    uint32_t u32EncodeNumber[CHANNEL_NUMBER] = {0, 0};
    ST_FR_BUF_INFO stFrBuffer[CHANNEL_NUMBER];
    ST_VENC_GET_STREAM *pstPara = (ST_VENC_GET_STREAM*)pThreadPara;
    FILE *pFileHandler[CHANNEL_NUMBER] = {NULL, NULL};
    char s8pFileName[MAX_FILE_NAME_LENGTH] = {'\0'};
    bool bGetHeader[CHANNEL_NUMBER] = {false, false};
    char s8VideoHeader[MAX_VIDEO_HEADER_LENGTH];
    uint32_t i, u32ChannelNum = pstPara->u32ChannelNum;

    /*
     step 1:  Set filepath && name and open to write.
   */
    printf("Enter thread VencStartStreamProc\n");

    memset(stFrBuffer, 0, sizeof(stFrBuffer));
    for (i = 0; i < u32ChannelNum; i++)
    {
        memset(s8pFileName, 0, sizeof(s8pFileName));
        ret = video_get_resolution(s_VideoChannel[i], &s32Width, &s32Height);
        sprintf(s8pFileName, "/mnt/sd0/%s_%d_%d_%d.h265", s_VideoChannel[i], i, s32Width, s32Height);
        pFileHandler[i] = fopen(s8pFileName, "wb+");
        if (pFileHandler[i] == NULL)
        {
            printf("open file %s failed!\n", s8pFileName);
            return NULL;
        }
    }
    /*
     step 2:  Start to get streams of channel.
    */
    while (true == pstPara->bThreadStart)
    {
        for (i = 0; i < u32ChannelNum; i++)
        {
            if (bGetHeader[i] == false)
            {
                ret = video_get_header(s_VideoChannel[i], s8VideoHeader, MAX_VIDEO_HEADER_LENGTH);
                if (ret <= 0 )
                {
                    printf("video_get_header error ret:->%d\n", ret);
                    usleep(1000);
                    continue;
                }
                else
                {
                    if (fwrite(s8VideoHeader, ret, 1, pFileHandler[i]) != 1)
                    {
                        printf("Error:save video header fail!\n");
                        break;
                    }
                }
                bGetHeader[i] = true;
            }
            if(video_test_frame(s_VideoChannel[i], &stFrBuffer[i]) <= 0)
            {
                continue;
            }

            ret = video_get_frame(s_VideoChannel[i], &stFrBuffer[i]);
            if (ret < 0 )
            {
                printf("video_get_frame error ->%d\n", ret);
                usleep(1000);
                continue;
            }
            if (u32EncodeNumber[i] == 0 && stFrBuffer[i].priv != VIDEO_FRAME_I)
            {
                video_put_frame(s_VideoChannel[i], &stFrBuffer[i]);
                continue;
            }
            u32EncodeNumber[i]++;
            if (fwrite(stFrBuffer[i].virt_addr, stFrBuffer[i].size, 1, pFileHandler[i]) != 1)
            {
                printf("Error:save stream fail!\n");
                break;
            }
            video_put_frame(s_VideoChannel[i], &stFrBuffer[i]);
        }
    }

    /*
    * step 3 : close save-file
    */
    for (i = 0; i < u32ChannelNum; i++)
    {
        if (pFileHandler[i] != NULL)
        {
            fclose(pFileHandler[i]);
            pFileHandler[i] = NULL;
        }
    }
    printf("Exit thread VencStartStreamProc\n");
    return NULL;
}

/*
 * @brief  stop get video encode stream process.
 * @param pThreadPara -IN: thread info and video channel number.
 * @return void.
 */
void VencStopStream(ST_VENC_GET_STREAM *pstPara)
{
    if (true == pstPara->bThreadStart)
    {
        pstPara->bThreadStart = false;
        pthread_join(pstPara->s32ThreadID, 0);
    }
}

/*
 * @brief main function
 * @param s32Argc      -IN: args' count
 * @param ps8Argv      -IN: args
 * @return 0: Success.
 * @return -1: Failure.
 */
int main(int s32Argc, char **ps8Argv)
{
    ST_VENC_GET_STREAM stPara;
    EN_VIDEO_RATE_CTRL_TYPE enRatectrlMode = VENC_CBR_MODE;

    if (s32Argc < 2)
    {
        VencUsage(ps8Argv[0]);
        return -1;
    }
    memset(&stPara, 0, sizeof(stPara));
    enRatectrlMode = (EN_VIDEO_RATE_CTRL_TYPE)atoi(ps8Argv[1]);
    if (videobox_repath(JSON_PATH) < 0)
    {
        printf("Error: videobox start failed!\n");
        return -1;
    }
    stPara.bThreadStart = true;
    stPara.u32ChannelNum = CHANNEL_NUMBER;
    VencSetRateCtrl(enRatectrlMode, stPara.u32ChannelNum);
    pthread_create(&stPara.s32ThreadID, 0, VencStartStreamProc, (void *)&stPara);

    printf("you can press q to exit this sample\n");
    printf("you can press q to exit this sample\n");
    printf("you can press q to exit this sample\n");

    while (1)
    {
        if(getchar() == 'q')
        {
            break;
        }
        usleep(10*1000);
    }
    VencStopStream(&stPara);
    videobox_stop();
    return 0;
}
