// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
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
#include "ISPOSTv2.h"

#include <qsdk/tcpcommand.h>

int IPU_ISPOSTV2::UnitControl(std::string func, void *arg)
{

    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(videobox_control) {
        vbctrl_t *c = (vbctrl_t *)arg;
        switch (c->code) {
            case VBCTRL_INDEX:
                if(c->para < 0) return IspostGetLcGridTargetIndex();
                else return IspostSetLcGridTargetIndex(c->para);
            case VBCTRL_GET:
                return IspostGetLcGridTargetCount();
        }
        return 0;
    }

    UCFunction(video_set_pip_info) {
        IspostSetPipInfo((struct v_pip_info *)arg);
        return 0;
    }

    UCFunction(video_get_pip_info) {
        IspostGetPipInfo((struct v_pip_info *)arg);
        return 0;
    }
#ifdef MACRO_IPU_ISPOSTV2_FCE
    UCFunction(camera_create_and_fire_fcedata) {
        return IspostCreateAndFireFceData((cam_fcedata_param_t *)arg);
    }
#endif
    UCFunction(camera_load_and_fire_fcedata) {
        int ret = 0;
        key_t key = *(key_t*)arg;
        int id = 0;
        char *pMap = NULL;


        id = shmget(key, 0, 0666);
        if (id == -1) {
            LOGE("(%s:L%d) shmget is error (key=%d) !!!\n", __func__, __LINE__, key) ;
            goto toFail;
        }
        pMap = (char*)shmat(id, NULL, 0);
        if (NULL == pMap) {
            LOGE("(%s:L%d) p_map is NULL (shm_id=%d) !!!\n", __func__, __LINE__, id);
            goto toFail;
        }
        ret = IspostLoadAndFireFceData((cam_fcefile_param_t *)pMap);
        if (shmdt(pMap) == -1)
            LOGE("(%s:L%d) shmdt is error (pMap=%p) !!!\n", __func__, __LINE__, pMap);
        return ret;
toFail:
        return -1;
    }
    UCFunction(camera_set_fce_mode) {
        return IspostSetFceMode(*(int *)arg);
    }
    UCFunction(camera_get_fce_mode) {
        return IspostGetFceMode((int *)arg);
    }
    UCFunction(camera_get_fce_totalmodes) {
        return IspostGetFceTotalModes((int *)arg);
    }
    UCFunction(camera_save_fcedata) {
        return IspostSaveFceData((cam_save_fcedata_param_t *)arg);
    }
#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
    UCFunction(camera_set_ispost) {
        //LOGE("UnitCtrl:camera set ispostv2 : %s\n", func.c_str());
  #if 0
        return SetIspost(arg);
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
        iRet = SetIspost((void *)pbyMap);
        if (shmdt(pbyMap) == -1)
            LOGE("(%s:L%d) shmdt is error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
        return iRet;
  #endif //#if 0
    }

    UCFunction(camera_get_ispost) {
        //LOGE("UnitCtrl:camera get ispostv2 : %s\n", func.c_str());
  #if 0
        return GetIspost(arg);
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
        iRet = GetIspost((void *)pbyMap);
        if (shmdt(pbyMap) == -1)
            LOGE("(%s:L%d) shmdt is error (pbyMap=%p) !!!\n", __func__, __LINE__, pbyMap);
        return iRet;
  #endif //#if 0
    }
#endif //#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
