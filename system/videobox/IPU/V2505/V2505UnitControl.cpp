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
#include "V2505.h"

int IPU_V2500::UnitControl(std::string func, void *arg) {

    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

#if 1
    UCFunction(camera_load_parameters) {
//        return LoadParameters((const char *)arg);
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_mirror) {
        enum cam_mirror_state &st = *(enum cam_mirror_state *)arg;

        return GetMirror(st);
    }

    UCFunction(camera_set_mirror) {
        return SetMirror(*(enum cam_mirror_state *)arg);
    }

    UCFunction(camera_get_fps) {
        int &val = *(int *)arg;

        return GetFPS(val);
    }

    UCFunction(camera_set_fps) {
        return SetFPS(*(int *)arg);
    }


    UCFunction(camera_get_brightness) {
        int &val = *(int *)arg;

        return GetYUVBrightness(val);
    }

    UCFunction(camera_set_brightness) {
        return SetYUVBrightness(*(int *)arg);
    }


    UCFunction(camera_get_contrast) {
        int &val = *(int *)arg;

        return GetContrast(val);
    }

    UCFunction(camera_set_contrast) {
        return SetContrast(*(int *)arg);
    }

    UCFunction(camera_get_saturation) {
        int &val = *(int *)arg;

        return GetSaturation(val);
    }
    UCFunction(camera_set_saturation) {
        return SetSaturation(*(int *)arg);
    }

    UCFunction(camera_get_sharpness) {
        int &val = *(int *)arg;

        return GetSharpness(val);
    }

    UCFunction(camera_set_sharpness) {
        return SetSharpness(*(int *)arg);
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

        return GetAntiFlicker(freq);
    }

    UCFunction(camera_set_antiflicker) {
        return SetAntiFlicker(*(int *)arg);
    }

    UCFunction(camera_get_wb) {
        int &mode = *(int *)arg;
        return GetWB(mode);
    }

    UCFunction(camera_set_wb) {
        return SetWB(*(int *)arg);
    }

    UCFunction(camera_get_scenario) {
        enum cam_scenario &scen = *(enum cam_scenario* )arg;

        return GetScenario(scen);
    }

    UCFunction(camera_set_scenario) {
        return SetScenario(*(int *)arg);
    }

    UCFunction(camera_monochrome_is_enabled) {
        return IsMonchromeEnabled();
    }

    UCFunction(camera_monochrome_enable) {
        return EnableMonochrome(*(int *)arg);
    }

    UCFunction(camera_awb_is_enabled) {
        return IsAWBEnabled();
    }

    UCFunction(camera_awb_enable) {
        return EnableAWB(*(int *)arg);
    }

    UCFunction(camera_ae_is_enabled) {
        return IsAEEnabled();
    }

    UCFunction(camera_ae_enable) {
        return EnableAE(*(int *)arg);
    }

    UCFunction(camera_wdr_is_enabled) {
        return IsWDREnabled();
    }

    UCFunction(camera_wdr_enable) {
        return EnableWDR(*(int *)arg);
    }

    UCFunction(camera_get_snap_res) {
        return GetSnapResolution((reslist_t *)arg);
    }

    UCFunction(camera_set_snap_res) {
        return SetSnapResolution(*(res_t*)arg);
    }

    UCFunction(camera_snap_one_shot) {
        LOGD("in V2505 notify the snap one shot");
        return SnapOneShot();
    }

    UCFunction(camera_snap_exit) {
        return SnapExit();
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

        return GetHue(val);
    }

    UCFunction(camera_set_hue) {
        return SetHue(*(int *)arg);
    }

    UCFunction(camera_get_wb) {
        int &val = *(int *)arg;

        return GetWB(val);
    }

    UCFunction(camera_set_wb) {
        return SetWB(*(int *)arg);
    }

    UCFunction(camera_get_brightnessvalue) {
        int &val = *(int *)arg;

        return GetEnvBrightnessValue(val);
    }

    UCFunction(camera_set_scenario) {
        return SetScenario(*(int *)arg);
    }

    UCFunction(camera_get_scenario) {
        enum cam_scenario &val = *(enum cam_scenario *)arg;

        return GetScenario(val);
    }

    UCFunction(camera_set_ISOLimit) {
        return SetSensorISO(*(int *)arg);
    }

    UCFunction(camera_get_ISOLimit) {
        int &val = *(int *)arg;

        return GetSensorISO(val);
    }

    UCFunction(camera_set_exposure) {
        return SetExposureLevel(*(int *)arg);
    }

    UCFunction(camera_get_exposure) {
        int &val = *(int *)arg;

        return GetExposureLevel(val);
    }


    UCFunction(camera_set_sensor_exposure) {
        return SetSensorExposure(*(int *)arg);
    }

    UCFunction(camera_get_sensor_exposure) {
        int &val = *(int *)arg;

        return GetSensorExposure(val);
    }

    UCFunction(camera_set_dpf_attr) {
        return SetDpfAttr((cam_dpf_attr_t*)arg);
    }

    UCFunction(camera_get_dpf_attr) {
        return GetDpfAttr((cam_dpf_attr_t*)arg);
    }

    UCFunction(camera_set_dns_attr) {
        return SetDnsAttr((cam_dns_attr_t*)arg);
    }

    UCFunction(camera_get_dns_attr) {
        return GetDnsAttr((cam_dns_attr_t*)arg);
    }

    UCFunction(camera_set_sha_dns_attr) {
        return SetShaDenoiseAttr((cam_sha_dns_attr_t*)arg);
    }

    UCFunction(camera_get_sha_dns_attr) {
        return GetShaDenoiseAttr((cam_sha_dns_attr_t*)arg);
    }

    UCFunction(camera_set_wb_attr) {
        return SetWbAttr((cam_wb_attr_t*)arg);
    }

    UCFunction(camera_get_wb_attr) {
        return GetWbAttr((cam_wb_attr_t*)arg);
    }

    UCFunction(camera_set_3d_dns_attr) {
        return Set3dDnsAttr((cam_3d_dns_attr_t*)arg);
    }

    UCFunction(camera_get_3d_dns_attr) {
        return Get3dDnsAttr((cam_3d_dns_attr_t*)arg);
    }

    UCFunction(camera_set_yuv_gamma_attr) {
        return SetYuvGammaAttr((cam_yuv_gamma_attr_t*)arg);
    }

    UCFunction(camera_get_yuv_gamma_attr) {
        return GetYuvGammaAttr((cam_yuv_gamma_attr_t*)arg);
    }

    UCFunction(camera_set_fps_range) {
        return SetFpsRange((cam_fps_range_t*)arg);
    }

    UCFunction(camera_get_fps_range) {
        return GetFpsRange((cam_fps_range_t*)arg);
    }

    UCFunction(camera_get_day_mode) {
        enum cam_day_mode &val = *(enum cam_day_mode *)arg;

        return GetDayMode(val);
    }

    UCFunction(camera_save_data) {
        return SaveData((cam_save_data_t*)arg);
    }

    UCFunction(camera_set_gamcurve) {
        return SetGAMCurve((cam_gamcurve_t *)arg);
    }

  #if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    UCFunction(camera_set_iq) {
    #if 0
        return SetIQ(arg);
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
        iRet = SetIQ((void *)pbyMap);
        if (shmdt(pbyMap) == -1)
            LOGE("(%s:L%d) shmdt is error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
        return iRet;
    #endif //#if 0
    }

    UCFunction(camera_get_iq) {
    #if 0
        return GetIQ(arg);
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
        iRet = GetIQ((void *)pbyMap);
        if (shmdt(pbyMap) == -1)
            LOGE("(%s:L%d) shmdt is error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
        return iRet;
    #endif //#if 0
    }

    UCFunction(camera_save_isp_parameters) {
        return SaveIspParameters((cam_save_data_t *)arg);
    }

  #endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    UCFunction(camera_get_sensors) {

        return GetSensorsName((cam_list_t *)arg);
    }
#endif

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
