// Copyright (C) 2016 InfoTM, warits.wang@infotm.com
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

#include <stdio.h>
#include <string.h>
#include <qsdk/videobox.h>
#include <errno.h>
#include <sys/shm.h>

int camera_get_sensors(const char *cam, cam_list_t *list)
{
    videobox_launch_rpc(cam, list);
    return _rpc.ret;
}

int camera_load_parameters(const char *cam, const char *js, int len)
{
    videobox_launch_rpc_l(cam, js, len);
    return _rpc.ret;
}

int camera_get_direction(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_set_direction(const char *cam, enum cam_direction dir)
{
    videobox_launch_rpc(cam, &dir);
    return _rpc.ret;
}

int camera_get_mirror(const char *cam, enum cam_mirror_state *cms)
{
    if (cms) {
        videobox_launch_rpc(cam, cms);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_mirror(const char *cam, enum cam_mirror_state cms)
{
    videobox_launch_rpc(cam, &cms);
    return _rpc.ret;
}

int camera_get_fps(const char *cam, int *out_fps)
{
    if (out_fps) {
        videobox_launch_rpc(cam, out_fps);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_fps(const char *cam, int fps)
{
    videobox_launch_rpc(cam, &fps);
    return _rpc.ret;
}

int camera_get_brightness(const char *cam, int *out_bri)
{
    if (out_bri) {
        videobox_launch_rpc(cam, out_bri);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_brightness(const char *cam, int bri)
{
    videobox_launch_rpc(cam, &bri);
    return _rpc.ret;
}

int camera_get_contrast(const char *cam, int *out_con)
{
    if (out_con) {
        videobox_launch_rpc(cam, out_con);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_contrast(const char *cam, int con)
{
    videobox_launch_rpc(cam, &con);
    return _rpc.ret;
}

int camera_get_saturation(const char *cam, int *out_sat)
{
    if (out_sat) {
        videobox_launch_rpc(cam, out_sat);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_saturation(const char *cam, int sat)
{
    videobox_launch_rpc(cam, &sat);
    return _rpc.ret;
}

int camera_get_sharpness(const char *cam, int *out_sp)
{
    if (out_sp) {
        videobox_launch_rpc(cam, out_sp);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_sharpness(const char *cam, int sp)
{
    videobox_launch_rpc(cam, &sp);
    return _rpc.ret;
}

int camera_get_antifog(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_set_antifog(const char *cam, int level)
{
    videobox_launch_rpc(cam, &level);
    return _rpc.ret;
}

int camera_get_antiflicker(const char *cam, int *out_freq)
{
    if (out_freq) {
        videobox_launch_rpc(cam, out_freq);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_antiflicker(const char *cam, int freq)
{
    videobox_launch_rpc(cam, &freq);
    return _rpc.ret;
}

int camera_get_wb(const char *cam, enum cam_wb_mode *out_mode)
{
    if (out_mode) {
        videobox_launch_rpc(cam, out_mode);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_wb(const char *cam, enum cam_wb_mode mode)
{
    videobox_launch_rpc(cam, &mode);
    return _rpc.ret;
}

int camera_set_scenario(const char *cam, enum cam_scenario sno)
{
    videobox_launch_rpc(cam, &sno);
    return _rpc.ret;
}

int camera_get_scenario(const char *cam, enum cam_scenario *out_sno)
{
    if (out_sno) {
        videobox_launch_rpc(cam, out_sno);
        return _rpc.ret;
    } else {
        return -1;
    }
}

/* features on/off */
int camera_monochrome_is_enabled(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_monochrome_enable(const char *cam, int en)
{
    videobox_launch_rpc(cam, &en);
    return _rpc.ret;
}

int camera_ae_is_enabled(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_ae_enable(const char *cam, int en)
{
    videobox_launch_rpc(cam, &en);
    return _rpc.ret;
}

int camera_set_exposure(const char *cam, int level)
{
    videobox_launch_rpc(cam, &level);
    return _rpc.ret;
}

int camera_get_exposure(const char *cam, int *out_level)
{
    if (out_level) {
        videobox_launch_rpc(cam, out_level);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_wdr_is_enabled(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_wdr_enable(const char *cam, int en)
{
    videobox_launch_rpc(cam, &en);
    return _rpc.ret;
}

int camera_set_ISOLimit(const char *cam, int iso)
{
    videobox_launch_rpc(cam, &iso);
    return _rpc.ret;
}

int camera_get_ISOLimit(const char *cam, int *out_iso)
{
    if (out_iso) {
        videobox_launch_rpc(cam, out_iso);
        return _rpc.ret;
    } else {
        return -1;
    }
}

/* focus functions */
int camera_af_is_enabled(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_af_enable(const char *cam, int en)
{
    videobox_launch_rpc(cam, &en);
    return _rpc.ret;
}

int camera_reset_lens(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_focus_is_locked(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_focus_lock(const char *cam, int lock)
{
    videobox_launch_rpc(cam, &lock);
    return _rpc.ret;
}

int camera_get_focus(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_set_focus(const char *cam, int distance)
{
    videobox_launch_rpc(cam, &distance);
    return _rpc.ret;
}

int camera_get_zoom(const char *cam, struct cam_zoom_info *info)
{
    videobox_launch_rpc(cam, info);
    return _rpc.ret;
}

int camera_set_zoom(const char *cam, struct cam_zoom_info *info)
{
    videobox_launch_rpc(cam, info);
    return _rpc.ret;
}

int camera_get_hue(const char *cam, int *out_hue)
{
    if (out_hue) {
        videobox_launch_rpc(cam, out_hue);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_hue(const char *cam, int hue)
{
    videobox_launch_rpc(cam, &hue);
    return _rpc.ret;
}

int camera_get_brightnessvalue(const char *cam, int *out_bv)
{
    if (out_bv) {
        videobox_launch_rpc(cam, out_bv);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_snap_res(const char *cam, cam_reslist_t *preslist)
{
    videobox_launch_rpc(cam, preslist);
    return _rpc.ret;
}

int camera_set_snap_res(const char *cam, cam_res_t res)
{

    videobox_launch_rpc(cam, &res);
    return _rpc.ret;
}

int camera_snap_one_shot(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_snap_exit(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_v4l2_snap_one_shot(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_v4l2_snap_exit(const char *cam)
{
    videobox_launch_rpc_l(cam, NULL, 0);
    return _rpc.ret;
}

int camera_set_sensor_exposure(const char *cam, int usec)
{
    videobox_launch_rpc(cam, &usec);
    return _rpc.ret;
}

int camera_get_sensor_exposure(const char *cam, int *out_usec)
{
    if (out_usec) {
        videobox_launch_rpc(cam, out_usec);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_dpf_attr(const char *cam, const cam_dpf_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_dpf_attr(const char *cam, cam_dpf_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_dns_attr(const char *cam, const cam_dns_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_dns_attr(const char *cam, cam_dns_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_sha_dns_attr(const char *cam, const cam_sha_dns_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_sha_dns_attr(const char *cam, cam_sha_dns_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_wb_attr(const char *cam, const cam_wb_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_wb_attr(const char *cam, cam_wb_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_3d_dns_attr(const char *cam, const cam_3d_dns_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_3d_dns_attr(const char *cam, cam_3d_dns_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_yuv_gamma_attr(const char *cam, const cam_yuv_gamma_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_yuv_gamma_attr(const char *cam, cam_yuv_gamma_attr_t *attr)
{
    if (attr) {
        videobox_launch_rpc(cam, attr);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_fps_range(const char *cam, const cam_fps_range_t *fps_range)
{
    if (fps_range) {
        videobox_launch_rpc(cam, fps_range);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_fps_range(const char *cam, cam_fps_range_t *fps_range)
{
    if (fps_range) {
        videobox_launch_rpc(cam, fps_range);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_day_mode(const char *cam, enum cam_day_mode *out_day_mode)
{
    if (out_day_mode) {
        videobox_launch_rpc(cam, out_day_mode);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_gamcurve(const char *cam, cam_gamcurve_t *curve)
{
    if (curve) {
        videobox_launch_rpc(cam, curve);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_index(const char *cam, int idx)
{
    videobox_launch_rpc(cam, &idx);
    return _rpc.ret;
}

int camera_get_index(const char *cam, int *idx)
{
    if (idx) {
        videobox_launch_rpc(cam, idx);
        return _rpc.ret;
    } else {
        return -1;
    }

}

int camera_save_data(const char *cam, const cam_save_data_t *save_data)
{
    if (save_data) {
        videobox_launch_rpc(cam, save_data);
        return _rpc.ret;
    } else {
        return -1;
    }
}

#define IPC_KEY                 (0x06)
static char g_shm_path_name[] = "/usr/bin";

int camera_get_iq(const char *cam, void *pdata, int len)
{
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
  #if 0
    videobox_launch_rpc_l(cam, pdata, len);
    return _rpc.ret;
  #else
    if (pdata) {
        int iRet = -1;
        key_t key = 0;
        int iShmId = 0;
        int iSize = 0;
        char *pbyMap = NULL;

        key = ftok(g_shm_path_name, IPC_KEY);
        if (-1 == key) {
            printf("(%s:L%d) ftok is error (path_name=%s) errono:%s!!!\n", __func__, __LINE__, g_shm_path_name, strerror(errno));
            return -1;
        }
        iSize = len;
        iShmId = shmget(key, iSize, IPC_CREAT | IPC_EXCL | 0666);
        if (-1  == iShmId) {
            iShmId = shmget(key, iSize, 0666);
            if (-1 == iShmId) {
                printf("(%s:L%d) shmget is error (key=%d, iSize=%d) errono:%s!!!\n", __func__, __LINE__, key, iSize, strerror(errno));
                return -1;
            }
        }
        pbyMap = shmat(iShmId, NULL, 0);
        if (NULL == pbyMap) {
            printf("(%s:L%d) p_map is NULL (iShmId=%d) errono:%s!!!\n", __func__, __LINE__, iShmId, strerror(errno));
            return -1;
        }
        memcpy(pbyMap, pdata, iSize);
        videobox_launch_rpc(cam, &key);
        memcpy(pdata, pbyMap, iSize);

        if (-1 == shmdt(pbyMap)) {
            printf("(%s:L%d) shmdt error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
            iRet = -1;
        }

        if (-1 == shmctl(iShmId, IPC_RMID, NULL)) {
            printf("(%s:L%d) shmctl error (iShmId=%d) !!!\n", __func__, __LINE__, iShmId);
            iRet = -1;
        }

        return _rpc.ret;
    } else {
        printf("%s: the command data structure is NULL!!!\n", __func__);
        return -1;
    }
  #endif //#if 0
#else
    printf("%s: is not support!!! Please turn on the \"IQ tuning tool APIs\" to compile it.\n", __func__);
    return -1;
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
}

int camera_set_iq(const char *cam, const void *pdata, int len)
{
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
  #if 0
    videobox_launch_rpc_l(cam, pdata, len);
    return _rpc.ret;
  #else
    if (pdata) {
        int iRet = -1;
        key_t key = 0;
        int iShmId = 0;
        int iSize = 0;
        char *pbyMap = NULL;

        key = ftok(g_shm_path_name, IPC_KEY);
        if (-1 == key) {
            printf("(%s:L%d) ftok is error (path_name=%s) errono:%s!!!\n", __func__, __LINE__, g_shm_path_name, strerror(errno));
            return -1;
        }
        iSize = len;
        iShmId = shmget(key, iSize, IPC_CREAT | IPC_EXCL | 0666);
        if (-1  == iShmId) {
            iShmId = shmget(key, iSize, 0666);
            if (-1 == iShmId) {
                printf("(%s:L%d) shmget is error (key=%d, iSize=%d) errono:%s!!!\n", __func__, __LINE__, key, iSize, strerror(errno));
                return -1;
            }
        }
        pbyMap = shmat(iShmId, NULL, 0);
        if (NULL == pbyMap) {
            printf("(%s:L%d) p_map is NULL (iShmId=%d) errono:%s!!!\n", __func__, __LINE__, iShmId, strerror(errno));
            return -1;
        }
        memcpy(pbyMap, pdata, iSize);

        videobox_launch_rpc(cam, &key);

        if (-1 == shmdt(pbyMap)) {
            printf("(%s:L%d) shmdt error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
            iRet = -1;
        }

        if (-1 == shmctl(iShmId, IPC_RMID, NULL)) {
            printf("(%s:L%d) shmctl error (iShmId=%d) !!!\n", __func__, __LINE__, iShmId);
            iRet = -1;
        }

        return _rpc.ret;
    } else {
        printf("%s: the command data structure is NULL!!!\n", __func__);
        return -1;
    }
  #endif //#if 0
#else
    printf("%s: is not support!!! Please turn on the \"IQ tuning tool APIs\" to compile it.\n", __func__);
    return -1;
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
}

int camera_save_isp_parameters(const char *cam, const cam_save_data_t *save_data)
{
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    if (save_data) {
        videobox_launch_rpc(cam, save_data);
        return _rpc.ret;
    } else {
        return -1;
    }
#else
    printf("%s: is not support!!! Please turn on the \"IQ tuning tool APIs\" to compile it.\n", __func__);
    return -1;
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
}

int camera_get_ispost(const char *cam, void *pdata, int len)
{
#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)||defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
  #if 0
    videobox_launch_rpc_l(cam, pdata, len);
    return _rpc.ret;
  #else
    if (pdata) {
        int iRet = -1;
        key_t key = 0;
        int iShmId = 0;
        int iSize = 0;
        char *pbyMap = NULL;

        key = ftok(g_shm_path_name, IPC_KEY);
        if (-1 == key) {
            printf("(%s:L%d) ftok is error (path_name=%s) errono:%s!!!\n", __func__, __LINE__, g_shm_path_name, strerror(errno));
            return -1;
        }
        iSize = len;
        iShmId = shmget(key, iSize, IPC_CREAT | IPC_EXCL | 0666);
        if (-1  == iShmId) {
            iShmId = shmget(key, iSize, 0666);
            if (-1 == iShmId) {
                printf("(%s:L%d) shmget is error (key=%d, iSize=%d) errono:%s!!!\n", __func__, __LINE__, key, iSize, strerror(errno));
                return -1;
            }
        }
        pbyMap = shmat(iShmId, NULL, 0);
        if (NULL == pbyMap) {
            printf("(%s:L%d) p_map is NULL (iShmId=%d) errono:%s!!!\n", __func__, __LINE__, iShmId, strerror(errno));
            return -1;
        }
        memcpy(pbyMap, pdata, iSize);
        videobox_launch_rpc(cam, &key);
        memcpy(pdata, pbyMap, iSize);

        if (-1 == shmdt(pbyMap)) {
            printf("(%s:L%d) shmdt error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
            iRet = -1;
        }

        if (-1 == shmctl(iShmId, IPC_RMID, NULL)) {
            printf("(%s:L%d) shmctl error (iShmId=%d) !!!\n", __func__, __LINE__, iShmId);
            iRet = -1;
        }

        return _rpc.ret;
    } else {
        printf("%s: the command data structure is NULL!!!\n", __func__);
        return -1;
    }
  #endif //#if 0
#else
    printf("%s: is not support!!! Please turn on the \"Ispost V1 and V2 IQ tuning tool APIs\" to compile it.\n", __func__);
    return -1;
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
}

int camera_set_ispost(const char *cam, const void *pdata, int len)
{
#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)||defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
  #if 0
    videobox_launch_rpc_l(cam, pdata, len);
    return _rpc.ret;
  #else
    if (pdata) {
        int iRet = -1;
        key_t key = 0;
        int iShmId = 0;
        int iSize = 0;
        char *pbyMap = NULL;

        key = ftok(g_shm_path_name, IPC_KEY);
        if (-1 == key) {
            printf("(%s:L%d) ftok is error (path_name=%s) errono:%s!!!\n", __func__, __LINE__, g_shm_path_name, strerror(errno));
            return -1;
        }
        iSize = len;
        iShmId = shmget(key, iSize, IPC_CREAT | IPC_EXCL | 0666);
        if (-1  == iShmId) {
            iShmId = shmget(key, iSize, 0666);
            if (-1 == iShmId) {
                printf("(%s:L%d) shmget is error (key=%d, iSize=%d) errono:%s!!!\n", __func__, __LINE__, key, iSize, strerror(errno));
                return -1;
            }
        }
        pbyMap = shmat(iShmId, NULL, 0);
        if (NULL == pbyMap) {
            printf("(%s:L%d) p_map is NULL (iShmId=%d) errono:%s!!!\n", __func__, __LINE__, iShmId, strerror(errno));
            return -1;
        }
        memcpy(pbyMap, pdata, iSize);

        videobox_launch_rpc(cam, &key);

        if (-1 == shmdt(pbyMap)) {
            printf("(%s:L%d) shmdt error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
            iRet = -1;
        }

        if (-1 == shmctl(iShmId, IPC_RMID, NULL)) {
            printf("(%s:L%d) shmctl error (iShmId=%d) !!!\n", __func__, __LINE__, iShmId);
            iRet = -1;
        }

        return _rpc.ret;
    } else {
        printf("%s: the command data structure is NULL!!!\n", __func__);
        return -1;
    }
  #endif //#if 0
#else
    printf("%s: is not support!!! Please turn on the \"Ispost V1 and V2 IQ tuning tool APIs\" to compile it.\n", __func__);
    return -1;
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
}

int camera_create_and_fire_fcedata(const char *cam, cam_fcedata_param_t *fcedata)
{
    if (fcedata) {
        videobox_launch_rpc(cam, fcedata);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_load_and_fire_fcedata(const char *cam, cam_fcefile_param_t *fcefile)
{
    if (fcefile) {
        int ret = -1;
        char *p_map = NULL;
        key_t key = 0;
        int shm_id = 0;
        int size = 0;


        size = sizeof(*fcefile);
        key = ftok(g_shm_path_name, 0x03);
        if (key == -1) {
            printf("(%s:L%d) ftok is error (path_name=%s) !!!\n", __func__, __LINE__, g_shm_path_name) ;
            return -1;
        }
        shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
        if (shm_id == -1) {
            shm_id = shmget(key, size, 0666);
            if (shm_id == -1) {
                printf("(%s:L%d) shmget is error (key=%d, size=%d) !!!\n", __func__, __LINE__, key, size) ;
                return -1;
            }
        }
        p_map = shmat(shm_id, NULL, 0);
        if (NULL == p_map) {
            printf("(%s:L%d) p_map is NULL (shm_id=%d) !!!\n", __func__, __LINE__, shm_id);
            return -1;
        }
        memcpy(p_map, fcefile, size);

        videobox_launch_rpc(cam, &key);

        if (shmdt(p_map) == -1)	{
            printf("(%s:L%d) shmdt error (p_map=%p) !!!\n", __func__, __LINE__, p_map);
            ret = -1;
        }

        if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
            printf("(%s:L%d) shmctl error (shm_id=%d) !!!\n", __func__, __LINE__, shm_id);
            ret = -1;
        }

        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_set_fce_mode(const char *cam, int mode)
{
    videobox_launch_rpc(cam, &mode);
    return _rpc.ret;
}

int camera_get_fce_mode(const char *cam, int *mode)
{
    if (mode) {
        videobox_launch_rpc(cam, mode);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_get_fce_totalmodes(const char *cam, int *mode_count)
{
    if (mode_count) {
        videobox_launch_rpc(cam, mode_count);
        return _rpc.ret;
    } else {
        return -1;
    }
}

int camera_save_fcedata(const char *cam, cam_save_fcedata_param_t *param)
{
    if (param) {
        videobox_launch_rpc(cam, param);
        return _rpc.ret;
    } else {
        return -1;
    }
}
