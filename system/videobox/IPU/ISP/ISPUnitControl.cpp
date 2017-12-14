// Copyright (C) 2016 InfoTM, sam.zhou@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <System.h>
#include <sys/shm.h>
#include "ISP.h"

int IPU_ISP::UnitControl(std::string func, void *arg) {

    IMG_UINT32 Idx = 0;

    LOGD("%s: %s\n", __func__, func.c_str());

    Idx = m_iCurCamIdx;
    UCSet(func);

#if 1
    UCFunction(camera_load_parameters) {
//        return LoadParameters((const char *)arg);
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_mirror) {
        enum cam_mirror_state &st = *(enum cam_mirror_state *)arg;

        return GetMirror(Idx, st);
    }

    UCFunction(camera_set_mirror) {
        return SetMirror(Idx, *(enum cam_mirror_state *)arg);
    }

    UCFunction(camera_get_fps) {
        int &val = *(int *)arg;

        return GetFPS(Idx, val);
    }

    UCFunction(camera_set_fps) {
        return SetFPS(Idx, *(int *)arg);
    }

    UCFunction(camera_get_brightness) {
        int &val = *(int *)arg;

        return GetYUVBrightness(Idx, val);
    }

    UCFunction(camera_set_brightness) {
        return SetYUVBrightness(Idx, *(int *)arg);
    }

    UCFunction(camera_get_contrast) {
        int &val = *(int *)arg;

        return GetContrast(Idx, val);
    }

    UCFunction(camera_set_contrast) {
        return SetContrast(Idx, *(int *)arg);
    }

    UCFunction(camera_get_saturation) {
        int &val = *(int *)arg;

        return GetSaturation(Idx, val);
    }
    UCFunction(camera_set_saturation) {
        return SetSaturation(Idx, *(int *)arg);
    }

    UCFunction(camera_get_sharpness) {
        int &val = *(int *)arg;

        return GetSharpness(Idx, val);
    }

    UCFunction(camera_set_sharpness) {
        return SetSharpness(Idx, *(int *)arg);
    }

    UCFunction(camera_get_antifog) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_set_antifog) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_antiflicker) {
        int &freq = *(int *)arg;

        return GetAntiFlicker(Idx, freq);
    }

    UCFunction(camera_set_antiflicker) {
        return SetAntiFlicker(Idx, *(int *)arg);
    }

    UCFunction(camera_get_wb) {
        int &val = *(int *)arg;

        return GetWB(Idx, val);
    }

    UCFunction(camera_set_wb) {
        return SetWB(Idx, *(int *)arg);
    }

    UCFunction(camera_get_scenario) {
        enum cam_scenario &val = *(enum cam_scenario *)arg;

        return GetScenario(Idx, val);

    }

    UCFunction(camera_set_scenario) {
        return SetScenario(Idx, *(int *)arg);
    }

    UCFunction(camera_monochrome_is_enabled) {
        return IsMonchromeEnabled(Idx);
    }

    UCFunction(camera_monochrome_enable) {
        return EnableMonochrome(Idx, *(int *)arg);
    }

    UCFunction(camera_ae_is_enabled) {
        return IsAEEnabled(Idx);
    }

    UCFunction(camera_ae_enable) {
        return EnableAE(Idx, *(int *)arg);
    }

    UCFunction(camera_get_exposure) {
        int &val = *(int *)arg;

        return GetExposureLevel(Idx, val);
    }

    UCFunction(camera_set_exposure) {
        return SetExposureLevel(Idx, *(int *)arg);
    }

    UCFunction(camera_wdr_is_enabled) {
        return IsWDREnabled(Idx);
    }

    UCFunction(camera_wdr_enable) {
        return EnableWDR(Idx, *(int *)arg);
    }

    UCFunction(camera_get_snap_res) {
        return GetSnapResolution(Idx, (reslist_t *)arg);
    }

    UCFunction(camera_set_snap_res) {
        return SetSnapResolution(Idx, *(res_t*)arg);
    }

    UCFunction(camera_snap_one_shot) {
        LOGD("in V2500 notify the snap one shot");
        return SnapOneShot();
    }

    UCFunction(camera_snap_exit) {
        return SnapExit();
    }

    UCFunction(camera_get_ISOLimit) {
        int &val = *(int *)arg;
        return GetSensorISO(Idx, val);
    }

    UCFunction(camera_set_ISOLimit) {
        return SetSensorISO(Idx, *(int *)arg);
    }

    UCFunction(camera_af_is_enabled) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_af_enable) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_reset_lens) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_focus_is_locked) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_focus_lock) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_focus) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_set_focus) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_zoom) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_hue) {
        int &val = *(int *)arg;

        return GetHue(Idx, val);
    }

    UCFunction(camera_set_hue) {
        return SetHue(Idx, *(int *)arg);
    }

    UCFunction(camera_get_brightnessvalue) {
        int &val = *(int *)arg;

        return GetEnvBrightnessValue(Idx, val);
    }

    UCFunction(camera_set_sensor_exposure) {
        return SetSensorExposure(Idx, *(int *)arg);
    }

    UCFunction(camera_get_sensor_exposure) {
        int &val = *(int *)arg;

        return GetSensorExposure(Idx, val);
    }

    UCFunction(camera_set_dpf_attr) {
        return SetDpfAttr(Idx, (cam_dpf_attr_t*)arg);
    }

    UCFunction(camera_get_dpf_attr) {
        return GetDpfAttr(Idx, (cam_dpf_attr_t*)arg);
    }

    UCFunction(camera_set_dns_attr) {
        return SetDnsAttr(Idx, (cam_dns_attr_t*)arg);
    }

    UCFunction(camera_get_dns_attr) {
        return GetDnsAttr(Idx, (cam_dns_attr_t*)arg);
    }

    UCFunction(camera_set_sha_dns_attr) {
        return SetShaDenoiseAttr(Idx, (cam_sha_dns_attr_t*)arg);
    }

    UCFunction(camera_get_sha_dns_attr) {
        return GetShaDenoiseAttr(Idx, (cam_sha_dns_attr_t*)arg);
    }

    UCFunction(camera_set_wb_attr) {
        return SetWbAttr(Idx, (cam_wb_attr_t*)arg);
    }

    UCFunction(camera_get_wb_attr) {
        return GetWbAttr(Idx, (cam_wb_attr_t*)arg);
    }

    UCFunction(camera_set_3d_dns_attr) {
        return Set3dDnsAttr((cam_3d_dns_attr_t*)arg);
    }

    UCFunction(camera_get_3d_dns_attr) {
        return Get3dDnsAttr((cam_3d_dns_attr_t*)arg);
    }

    UCFunction(camera_set_yuv_gamma_attr) {
        return SetYuvGammaAttr(Idx, (cam_yuv_gamma_attr_t*)arg);
    }

    UCFunction(camera_get_yuv_gamma_attr) {
        return GetYuvGammaAttr(Idx, (cam_yuv_gamma_attr_t*)arg);
    }

    UCFunction(camera_set_fps_range) {
        return SetFpsRange(Idx, (cam_fps_range_t*)arg);
    }

    UCFunction(camera_get_fps_range) {
        return GetFpsRange(Idx, (cam_fps_range_t*)arg);
    }

    UCFunction(camera_get_day_mode) {
        enum cam_day_mode &val = *(enum cam_day_mode *)arg;

        return GetDayMode(Idx, val);
    }

    UCFunction(camera_get_index) {
        int &Idx = *(int *)arg;
        return GetCameraIdx(Idx);
    }

    UCFunction(camera_set_index) {

        return SetCameraIdx(*(int *)arg);
    }

    UCFunction(camera_save_data) {
        return SaveData((cam_save_data_t*)arg);
    }

    UCFunction(camera_set_gamcurve) {
        return SetGAMCurve(Idx,(cam_gamcurve_t *)arg);
    }

#ifdef COMPILE_ISP_V2505
    UCFunction(camera_awb_is_enabled) {
        return IsAWBEnabled(Idx);
    }

    UCFunction(camera_awb_enable) {
        return EnableAWB(Idx, *(int *)arg);
    }
#elif defined(COMPILE_ISP_V2500)

#endif

  #if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    UCFunction(camera_set_iq) {
    #if 0
        return SetIQ(Idx, arg);
    #else
        int iRet = 0;
        key_t key = *(key_t*)arg;
        int iShmId = 0;
        char *pbyMap = NULL;

        iShmId = shmget(key, 0, 0666);
        if (iShmId == -1) {
            LOGE("(%s:L%d) shmget is error (key=%d) !!!\n", __func__, __LINE__, key) ;
            return -1;
        }
        pbyMap = (char*)shmat(iShmId, NULL, 0);
        if (NULL == pbyMap) {
            LOGE("(%s:L%d) pbyMap is NULL (iShmId=%d) !!!\n", __func__, __LINE__, iShmId);
            return -1;
        }
        iRet = SetIQ(Idx, (void *)pbyMap);
        if (shmdt(pbyMap) == -1)
            LOGE("(%s:L%d) shmdt is error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
        return iRet;
    #endif //#if 0
    }

    UCFunction(camera_get_iq) {
    #if 0
        return GetIQ(Idx, arg);
    #else
        int iRet = 0;
        key_t key = *(key_t*)arg;
        int iShmId = 0;
        char *pbyMap = NULL;

        iShmId = shmget(key, 0, 0666);
        if (iShmId == -1) {
            LOGE("(%s:L%d) shmget is error (key=%d) !!!\n", __func__, __LINE__, key) ;
            return -1;
        }
        pbyMap = (char*)shmat(iShmId, NULL, 0);
        if (NULL == pbyMap) {
            LOGE("(%s:L%d) pbyMap is NULL (iShmId=%d) !!!\n", __func__, __LINE__, iShmId);
            return -1;
        }
        iRet = GetIQ(Idx, (void *)pbyMap);
        if (shmdt(pbyMap) == -1)
            LOGE("(%s:L%d) shmdt is error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
        return iRet;
    #endif //#if 0
    }

    UCFunction(camera_save_isp_parameters) {
        return SaveIspParameters(Idx, (cam_save_data_t *)arg);
    }

  #endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    UCFunction(camera_get_sensors) {

        return GetSensorsName(Idx, (cam_list_t *)arg);
    }
#endif

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
