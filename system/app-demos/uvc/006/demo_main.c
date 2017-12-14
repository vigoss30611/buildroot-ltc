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
 * Description: uvc set brightness, contrast, hui, saturation, sharpness etc
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
#define CONTROL_ENTRY "isp"

typedef struct _uvc_control_request_param {
    struct uvc_processing_unit_control_obj pu_ctrl_def;
    struct uvc_processing_unit_control_obj pu_ctrl_max;
    struct uvc_processing_unit_control_obj pu_ctrl_min;
    struct uvc_processing_unit_control_obj pu_ctrl_cur;
    struct uvc_processing_unit_control_obj pu_ctrl_res;
}ST_UVC_CONTROL_REQUEST_PARAM;

static ST_UVC_CONTROL_REQUEST_PARAM s_stUvcParam;
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

void UvcPuBrightnessSetup(struct uvc_device *pstDev, 
               struct uvc_request_data *pstRest, uint8_t u8Req, uint8_t u8Cs)
{
    int s32Ret = 0;
    int s32Brightness = 0;
    union uvc_processing_unit_control_u *pstCtrl;

    UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, u8Req);

    pstCtrl = (union uvc_processing_unit_control_u*)&pstRest->data;
    pstRest->length =sizeof pstCtrl->wBrightness;

    memset(pstCtrl, 0, sizeof *pstCtrl);

    switch (u8Req) {
    case UVC_GET_CUR:
        s32Ret = camera_get_brightness(CONTROL_ENTRY, &s32Brightness);
        if (s32Ret < 0)
            UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_brightness failed \n",
                __func__);

        if (s32Brightness >=  0 &&  s32Brightness < 255) {
            s_stUvcParam.pu_ctrl_cur.wBrightness = s32Brightness;
        }
        pstCtrl->wBrightness =
            s_stUvcParam.pu_ctrl_cur.wBrightness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
            __func__, pstCtrl->wBrightness);
        break;

    case UVC_GET_MIN:
        pstCtrl->wBrightness =
            s_stUvcParam.pu_ctrl_min.wBrightness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
            __func__, pstCtrl->wBrightness);
        break;

    case UVC_GET_MAX:
        pstCtrl->wBrightness =
            s_stUvcParam.pu_ctrl_max.wBrightness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
            __func__, pstCtrl->wBrightness);
        break;

    case UVC_GET_DEF:
        pstCtrl->wBrightness =
            s_stUvcParam.pu_ctrl_def.wBrightness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
            __func__, pstCtrl->wBrightness);
        break;

    case UVC_GET_RES:
        pstCtrl->wBrightness =
            s_stUvcParam.pu_ctrl_res.wBrightness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
            __func__, pstCtrl->wBrightness);
        break;

    case UVC_GET_INFO:
        pstRest->data[0] = u8Cs;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
            __func__, u8Cs);
        break;
    }
}

void UvcPuContrastSetup(struct uvc_device *pstDev, 
               struct uvc_request_data *pstRest, uint8_t u8Req, uint8_t u8Cs)
{
    int s32Ret = 0;
    int s32Contrast = 0;
    union uvc_processing_unit_control_u *pstCtrl;

    UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, u8Req);

    pstCtrl = (union uvc_processing_unit_control_u*)&pstRest->data;
    pstRest->length =sizeof pstCtrl->wContrast;

    memset(pstCtrl, 0, sizeof *pstCtrl);

    switch (u8Req) {
    case UVC_GET_CUR:
        s32Ret  = camera_get_contrast(CONTROL_ENTRY, &s32Contrast);
        if (s32Ret < 0)
            UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_contrast failed \n",
                __func__);

        if (s32Contrast >= 0 && s32Contrast < 255) {
            s_stUvcParam.pu_ctrl_cur.wContrast = s32Contrast;
        }
        pstCtrl->wContrast =
            s_stUvcParam.pu_ctrl_cur.wContrast;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
            __func__, pstCtrl->wContrast);
        break;

    case UVC_GET_MIN:
        pstCtrl->wContrast =
            s_stUvcParam.pu_ctrl_min.wContrast;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
            __func__, pstCtrl->wContrast);
        break;

    case UVC_GET_MAX:
        pstCtrl->wContrast =
            s_stUvcParam.pu_ctrl_max.wContrast;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
            __func__, pstCtrl->wContrast);
        break;

    case UVC_GET_DEF:
        pstCtrl->wContrast =
            s_stUvcParam.pu_ctrl_def.wContrast;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
            __func__, pstCtrl->wContrast);
        break;

    case UVC_GET_RES:
        pstCtrl->wContrast =
            s_stUvcParam.pu_ctrl_res.wContrast;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
            __func__, pstCtrl->wContrast);
        break;

    case UVC_GET_INFO:
        pstRest->data[0] = u8Cs;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
            __func__, u8Cs);
        break;
    }
}

void UvcPuHueSetup(struct uvc_device *pstDev, 
               struct uvc_request_data *pstRest, uint8_t u8Req, uint8_t u8Cs)
{
    int s32Ret = 0;
    union uvc_processing_unit_control_u *pstCtrl;
    int s32Hue;

    UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, u8Req);

    pstCtrl = (union uvc_processing_unit_control_u*)&pstRest->data;
    pstRest->length =sizeof pstCtrl->wHue;

    memset(pstCtrl, 0, sizeof *pstCtrl);

    switch (u8Req) {
    case UVC_GET_CUR:
        s32Ret = camera_get_hue(CONTROL_ENTRY, 
                &s32Hue);
        s_stUvcParam.pu_ctrl_cur.wHue = s32Hue;
        if (s32Ret < 0) {
            s_stUvcParam.pu_ctrl_cur.wHue = s_stUvcParam.pu_ctrl_def.wHue;
            UVC_PRINTF(UVC_LOG_INFO,"# camera_get_hue failed \n");
        }
        pstCtrl->wHue =
            s_stUvcParam.pu_ctrl_cur.wHue;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
            __func__, pstCtrl->wHue);
        break;

    case UVC_GET_MIN:
        pstCtrl->wHue =
            s_stUvcParam.pu_ctrl_min.wHue;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
            __func__, pstCtrl->wHue);
        break;

    case UVC_GET_MAX:
        pstCtrl->wHue =
            s_stUvcParam.pu_ctrl_max.wHue;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
            __func__, pstCtrl->wHue);
        break;

    case UVC_GET_DEF:
        pstCtrl->wHue =
            s_stUvcParam.pu_ctrl_def.wHue;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
            __func__, pstCtrl->wHue);
        break;

    case UVC_GET_RES:
        pstCtrl->wHue =
            s_stUvcParam.pu_ctrl_res.wHue;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
            __func__, pstCtrl->wHue);
        break;

    case UVC_GET_INFO:
        pstRest->data[0] = u8Cs;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
            __func__, u8Cs);
        break;
    }
}

void UvcPuSaturationSetup(struct uvc_device *pstDev, 
               struct uvc_request_data *pstRest, uint8_t u8Req, uint8_t u8Cs)
{
    int s32Ret = 0;
    int saturation = 0;
    union uvc_processing_unit_control_u *pstCtrl;

    UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, u8Req);

    pstCtrl = (union uvc_processing_unit_control_u*)&pstRest->data;
    pstRest->length =sizeof pstCtrl->wSaturation;

    memset(pstCtrl, 0, sizeof *pstCtrl);

    switch (u8Req) {
    case UVC_GET_CUR:
        s32Ret  = camera_get_saturation(CONTROL_ENTRY, &saturation);
        if (s32Ret < 0)
            UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_saturation failed \n",
                __func__);

        if (saturation >= 0 && saturation < 255) {
            s_stUvcParam.pu_ctrl_cur.wSaturation = saturation;
        }
        pstCtrl->wSaturation =
            s_stUvcParam.pu_ctrl_cur.wSaturation;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
            __func__, pstCtrl->wSaturation);
        break;

    case UVC_GET_MIN:
        pstCtrl->wSaturation =
            s_stUvcParam.pu_ctrl_min.wSaturation;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
            __func__, pstCtrl->wSaturation);
        break;

    case UVC_GET_MAX:
        pstCtrl->wSaturation =
            s_stUvcParam.pu_ctrl_max.wSaturation;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
            __func__, pstCtrl->wSaturation);
        break;

    case UVC_GET_DEF:
        pstCtrl->wSaturation =
            s_stUvcParam.pu_ctrl_def.wSaturation;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
            __func__, pstCtrl->wSaturation);
        break;

    case UVC_GET_RES:
        pstCtrl->wSaturation =
            s_stUvcParam.pu_ctrl_res.wSaturation;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
            __func__, pstCtrl->wSaturation);
        break;

    case UVC_GET_INFO:
        pstRest->data[0] = u8Cs;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
            __func__, u8Cs);
        break;
    }
}

void UvcPuSharpnessSetup(struct uvc_device *pstDev, 
               struct uvc_request_data *pstRest, uint8_t u8Req, uint8_t u8Cs)
{
    int s32Sharpness = 0;
    int s32Ret  = 0;
    union uvc_processing_unit_control_u *pstCtrl;

    UVC_PRINTF(UVC_LOG_TRACE,"# %s (req %02x)\n", __func__, u8Req);

    pstCtrl = (union uvc_processing_unit_control_u*)&pstRest->data;
    pstRest->length =sizeof pstCtrl->wSharpness;

    memset(pstCtrl, 0, sizeof *pstCtrl);

    switch (u8Req) {
    case UVC_GET_CUR:
        s32Ret = camera_get_sharpness(CONTROL_ENTRY, &s32Sharpness);
        if (s32Ret < 0)
            UVC_PRINTF(UVC_LOG_ERR,"# %s camera_get_sharpness failed \n",
            __func__);

        if (s32Sharpness  >= 0 && s32Sharpness < 255) {
            s_stUvcParam.pu_ctrl_cur.wSharpness = s32Sharpness;
        }
        pstCtrl->wSharpness =
                    s_stUvcParam.pu_ctrl_cur.wSharpness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_CUR val = %d\n",
            __func__, pstCtrl->wSharpness);
        break;

    case UVC_GET_MIN:
        pstCtrl->wSharpness =
            s_stUvcParam.pu_ctrl_min.wSharpness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MIN val = %d\n",
            __func__, pstCtrl->wSharpness);
        break;

    case UVC_GET_MAX:
        pstCtrl->wSharpness =
            s_stUvcParam.pu_ctrl_max.wSharpness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_MAX val = %d\n",
            __func__, pstCtrl->wSharpness);
        break;

    case UVC_GET_DEF:
        pstCtrl->wSharpness =
            s_stUvcParam.pu_ctrl_def.wSharpness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_DEF val = %d\n",
            __func__, pstCtrl->wSharpness);
        break;

    case UVC_GET_RES:
        pstCtrl->wSharpness =
            s_stUvcParam.pu_ctrl_res.wSharpness;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_RES val = %d\n",
            __func__, pstCtrl->wSharpness);
        break;

    case UVC_GET_INFO:
        pstRest->data[0] = u8Cs;
        UVC_PRINTF(UVC_LOG_INFO,"# %s UVC_GET_INFO val = %d\n",
            __func__, u8Cs);
        break;
    }
}

void UvcPuBrightnessData(struct uvc_device *pstDev,
               struct uvc_request_data *pstReqData, uint8_t u8Cs)
{
    int s32Ret = 0;
    int s32Brightness;
    union uvc_processing_unit_control_u *pstCtrl;

    pstCtrl = (union uvc_processing_unit_control_u*)&pstReqData->data;
    if (pstReqData->length != sizeof pstCtrl->wBrightness) {
        UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
            __func__, pstReqData->length);
        return;
    }

    s32Brightness = pstCtrl->wBrightness;
    UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wBrightness]: %d\n",__func__,
        s32Brightness);

    if (s32Brightness >= 0 && s32Brightness < 256) {
        s32Ret = camera_set_brightness(CONTROL_ENTRY, s32Brightness);
        if (s32Ret <0)
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_brightness failed! "
                "brightness: %d\n", s32Brightness);
        else {
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_brightness successful! "
                    "brightness: %d\n", s32Brightness);
            s_stUvcParam.pu_ctrl_cur.wBrightness = s32Brightness;
        }
    } else {
        UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wBrightness]: %d\n",__func__,
            s32Brightness);
    }
}

void UvcPuContrastData(struct uvc_device *pstDev,
               struct uvc_request_data *pstReqData, uint8_t u8Cs)
{
    int s32Ret = 0;
    int s32Contrast;
    union uvc_processing_unit_control_u* pstCtrl;

    pstCtrl = (union uvc_processing_unit_control_u*)&pstReqData->data;
    if (pstReqData->length != sizeof pstCtrl->wContrast) {
        UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
            __func__, pstReqData->length);
        return;
    }

    s32Contrast = pstCtrl->wContrast;
    UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wContrast]: %d\n",__func__,
        s32Contrast);

    if (s32Contrast >= 0 && s32Contrast < 256) {
        s32Ret = camera_set_contrast(CONTROL_ENTRY, s32Contrast);
        if (s32Ret <0)
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_contrast failed! "
                "contrast: %d\n", s32Contrast);
        else {
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_contrast successful! "
                "contrast: %d\n", s32Contrast);
            s_stUvcParam.pu_ctrl_cur.wContrast= s32Contrast;
        }
    } else {
        UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wContrast]: %d\n",__func__,
            s32Contrast);
    }
}

void UvcPuHueData(struct uvc_device *pstDev,
               struct uvc_request_data *pstReqData, uint8_t u8Cs)
{
    int s32Ret = 0;
    int s32Hue;
    union uvc_processing_unit_control_u* pstCtrl;

    pstCtrl = (union uvc_processing_unit_control_u*)&pstReqData->data;
    if (pstReqData->length != sizeof pstCtrl->wHue) {
        UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
            __func__, pstReqData->length);
        return;
    }

    s32Hue = pstCtrl->wHue;
    UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wHue]: %d\n",__func__,
        s32Hue);

    if (s32Hue >= 0 && s32Hue < 256) {
        /*camera_set_hue do not realize at current Qsdk*/
        s32Ret = camera_set_hue(CONTROL_ENTRY, s32Hue);
        if (s32Ret <0)
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_hue failed! hue: %d\n", s32Hue);
        else {
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_hue successful! hue: %d\n", s32Hue);
            s_stUvcParam.pu_ctrl_cur.wHue= s32Hue;
        }
    } else {
        UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wHue]: %d\n",__func__,
            s32Hue);
    }
}

void UvcPuSaturationData(struct uvc_device *pstDev,
               struct uvc_request_data *pstReqData, uint8_t u8Cs)
{
    int s32Ret = 0;
    int saturation;
    union uvc_processing_unit_control_u *pstCtrl;

    pstCtrl = (union uvc_processing_unit_control_u*)&pstReqData->data;
    if (pstReqData->length != sizeof pstCtrl->wSaturation) {
        UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
            __func__, pstReqData->length);
        return;
    }

    saturation = pstCtrl->wSaturation;
    UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wSaturation]: %d\n",__func__,
        saturation);

    if (saturation > 0 && saturation < 256) {
        s32Ret = camera_set_saturation(CONTROL_ENTRY, saturation);
        if (s32Ret <0)
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_saturation failed! "
                "saturation: %d\n", saturation);
        else {
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_saturation successful! "
                "saturation: %d\n", saturation);
            s_stUvcParam.pu_ctrl_cur.wSaturation= saturation;
        }
    } else {
        UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wSaturation]: %d\n",__func__,
            saturation);
    }
}

void UvcPuSharpnessData(struct uvc_device *pstDev,
               struct uvc_request_data *pstReqData, uint8_t u8Cs)
{
    int s32Ret = 0;
    int s32Sharpness;
    union uvc_processing_unit_control_u* pstCtrl;

    pstCtrl = (union uvc_processing_unit_control_u*)&pstReqData->data;
    if (pstReqData->length != sizeof pstCtrl->wSharpness) {
        UVC_PRINTF(UVC_LOG_ERR, "!!! %s return a no vaild data, req len = %d\n",
            __func__, pstReqData->length);
        return;
    }

    s32Sharpness = pstCtrl->wSharpness;
    UVC_PRINTF(UVC_LOG_INFO, "### %s [PU][wSharpness]: %d\n",__func__,
        s32Sharpness);

    if (s32Sharpness >= 0 && s32Sharpness < 256) {
        s32Ret = camera_set_sharpness(CONTROL_ENTRY, s32Sharpness);
        if (s32Ret <0)
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_sharpness failed! "
                "sharpness: %d\n", s32Sharpness);
        else {
            UVC_PRINTF(UVC_LOG_ERR, "#camera_set_sharpness successful! "
                "sharpness: %d\n", s32Sharpness);
            s_stUvcParam.pu_ctrl_cur.wSharpness= s32Sharpness;
        }
    } else {
        UVC_PRINTF(UVC_LOG_ERR, "### %s [PU][wSharpness] is novalid: %d\n",__func__,
            s32Sharpness);
    }
}

void UvcControlRequestCallBackInit() {
    uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
        UVC_PU_BRIGHTNESS_CONTROL,
        UvcPuBrightnessSetup,
        UvcPuBrightnessData, 0);

    uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
        UVC_PU_CONTRAST_CONTROL,
        UvcPuContrastSetup,
        UvcPuContrastData, 0);

    uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
        UVC_PU_HUE_CONTROL,
        UvcPuHueSetup,
        UvcPuHueData, 0);

    uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
        UVC_PU_SATURATION_CONTROL,
        UvcPuSaturationSetup,
        UvcPuSaturationData, 0);

    uvc_cs_ops_register(UVC_CTRL_PROCESSING_UNIT_ID,
        UVC_PU_SHARPNESS_CONTROL,
        UvcPuSharpnessSetup,
        UvcPuSharpnessData, 0);
}

void UvcControlRequestParamInit()
{
    memset(&s_stUvcParam, 0, sizeof s_stUvcParam);

    s_stUvcParam.pu_ctrl_def.wBrightness = 127;
    s_stUvcParam.pu_ctrl_cur.wBrightness = 127;
    s_stUvcParam.pu_ctrl_res.wBrightness = 10;
    s_stUvcParam.pu_ctrl_min.wBrightness = 0; 
    s_stUvcParam.pu_ctrl_max.wBrightness = 255;

    s_stUvcParam.pu_ctrl_def.wContrast= 127;
    s_stUvcParam.pu_ctrl_cur.wContrast  = 127;
    s_stUvcParam.pu_ctrl_res.wContrast  = 10;
    s_stUvcParam.pu_ctrl_min.wContrast  = 0;
    s_stUvcParam.pu_ctrl_max.wContrast  = 255;

    s_stUvcParam.pu_ctrl_def.wHue = 127;
    s_stUvcParam.pu_ctrl_cur.wHue = 127;
    s_stUvcParam.pu_ctrl_res.wHue = 10;
    s_stUvcParam.pu_ctrl_min.wHue = 0;
    s_stUvcParam.pu_ctrl_max.wHue = 255;

    s_stUvcParam.pu_ctrl_def.wSaturation = 30;
    s_stUvcParam.pu_ctrl_cur.wSaturation = 30;
    s_stUvcParam.pu_ctrl_res.wSaturation = 10;
    s_stUvcParam.pu_ctrl_min.wSaturation = 0;
    s_stUvcParam.pu_ctrl_max.wSaturation = 255;

    s_stUvcParam.pu_ctrl_def.wSharpness = 127;
    s_stUvcParam.pu_ctrl_cur.wSharpness = 127;
    s_stUvcParam.pu_ctrl_res.wSharpness = 10;
    s_stUvcParam.pu_ctrl_min.wSharpness = 0;
    s_stUvcParam.pu_ctrl_max.wSharpness = 255;
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

    UvcControlRequestCallBackInit();
    UvcControlRequestParamInit();

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
    int s32Ret = 0;
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

