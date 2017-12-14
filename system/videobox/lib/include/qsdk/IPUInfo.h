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
 * Description:Video debug infomation
 *
 * Author:
 *     devin.zhu <devin.zhu@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/21/2017 :init
 * 1.2  08/28/2017 :add isp debug info
 */

#ifndef __IPU_INFO_H
#define __IPU_INFO_H

#define CHANNEL_LENGTH 32
#define ARRAY_SIZE 8
#define N_CONTEXT 2

typedef enum ipu_info_type
{
    MTYPENONE = -1,
    ISP,
    ISPOST,
    VENC
} EN_IPU_INFO_TYPE;

typedef enum event_type
{
    ETYPENONE = -1,
    IPU_INFO_EVENT,
    IPU_DATA_EVENT
} EN_EVENT_TYPE;

typedef struct event_type_item
{
    int s32Type;
    char ps8MetaData[SUB_TYPE_LENGTH];
} ST_EVENT_TYPE_ITEM;

typedef enum ipu_type
{
    IPU_NONE = -1,
    EN_IPU_ISP = 0,
    IPU_ISPOST,
    IPU_H264,
    IPU_H265,
    IPU_JPEG
} EN_IPU_TYPE;

typedef enum isp_ipu_type
{
    ISP_IPU,
    ISPOST_IPU,
} EN_ISP_IPU_TYPE;

typedef enum channel_state
{
    STATENONE = -1,
    INIT,
    IDLE,
    RUN,
    STOP
} EN_CHANNEL_STATE;

typedef enum rc_mode
{
    RCMODNONE = -1,
    CBR,
    VBR,
    FIXQP
} EN_RC_MODE;

typedef struct rc_info
{
    EN_RC_MODE enRCMode;
    float f32ConfigBitRate;
    int s32Gop;
    int s32PictureSkip;
    int s32IInterval;
    int s32Mbrc;
    int s32QPDelta;
    unsigned int u32ThreshHold;
    char s8MinQP;
    char s8MaxQP;
    int s32RealQP;
} ST_RC_INFO;

typedef struct venc_info
{
    EN_IPU_TYPE enType;
    EN_CHANNEL_STATE enState;
    ST_RC_INFO stRcInfo;
    unsigned long long u64InCount;
    unsigned long long u64OutCount;
    float f32FPS;
    float f32RealFPS;
    float f32BitRate;
    float f32RealBitRate;
    int s32Width;
    int s32Height;
    unsigned int u32FrameDrop;
    char ps8Channel[CHANNEL_LENGTH];
} ST_VENC_INFO;

typedef struct venc_dbg_info
{
    ST_VENC_INFO stVencInfo[ARRAY_SIZE];
    char s8Capcity;
    char s8Size;
} ST_VENC_DBG_INFO;

typedef struct isp_dbg_info {
    int s32CtxN;
    double f64Gain[N_CONTEXT];
    double f64MinGain[N_CONTEXT];
    double f64MaxGain[N_CONTEXT];

    unsigned int u32Exp[N_CONTEXT];
    unsigned int u32MinExp[N_CONTEXT];
    unsigned int u32MaxExp[N_CONTEXT];

    int s32AWBEn[N_CONTEXT];
    int s32AEEn[N_CONTEXT];
    int s32HWAWBEn[N_CONTEXT];
    int s32TNMStaticCurve[N_CONTEXT];

    int s32DayMode[N_CONTEXT];
    int s32MirrorMode[N_CONTEXT];

    int s32OutW;
    int s32OutH;
    int s32CapW;
    int s32CapH;
} ST_ISP_DBG_INFO;

#endif
