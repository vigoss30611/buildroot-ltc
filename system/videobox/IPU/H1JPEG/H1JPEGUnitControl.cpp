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
#include "H1JPEG.h"

int IPU_H1JPEG::UnitControl(std::string func, void *arg) {

    LOGD("%s: %s\n", __func__, func.c_str());
    int ret = VBOK;
    UCSet(func);

    UCFunction(video_get_snap) {
        TriggerSnap();
        return enJencRet;
    }

    UCFunction(video_get_thumbnail) {
        #ifdef COMPILE_OPT_JPEG_SWSCALE
        if (thumbnailMode != EXTRA) {
            LOGE("error:thumbnail mode is not extra mode!\n");
            return VBEFAILED;
        }
        ret = GetThumbnail();
        #else
        LOGE("error:thumbnail mode need H1Jpeg open scale macro!\n");
        ret = VBEFAILED;
        #endif

        return ret;
    }

    UCFunction(thumbnail_set_resolution) {
        pthread_mutex_lock(&updateMut);
        #ifdef COMPILE_OPT_JPEG_SWSCALE
        memcpy(&configData.thumbRes, (vbres_t *) arg, sizeof(vbres_t));
        configData.flag |= UPDATE_THUMBRES;
        ret = VBOK;
        #else
        LOGE("error:thumbnail mode need H1Jpeg open scale macro!\n");
        ret = VBEFAILED;
        #endif

        pthread_mutex_unlock(&updateMut);
        return ret;
    }
    UCFunction(video_set_snap_quality) {
        int qLevel = *(int *)arg;

        if(qLevel >= 0 && qLevel <= 9) {
            pthread_mutex_lock(&updateMut);
            memcpy(&configData.qLevel, (int *) arg, sizeof(int));
            configData.flag |= UPDATE_QUALITY;
            pthread_mutex_unlock(&updateMut);
            ret = VBOK;
        } else {
            ret = VBEBADPARAM;
        }
        return ret;
    }

    UCFunction(video_set_snap) {
        pthread_mutex_lock(&updateMut);
        memcpy(&configData.pipInfo, (struct v_pip_info *) arg, sizeof(struct v_pip_info));
        configData.flag |= UPDATE_CROPINFO;
        pthread_mutex_unlock(&updateMut);

        return VBOK;
    }
    UCFunction(video_set_panoramic) {
        pthread_mutex_lock(&updateMut);
        if(*((int *)arg) == 1)
        {
            m_bApp1Enable = true;
        }
        else
        {
            m_bApp1Enable = false;
        }
        pthread_mutex_unlock(&updateMut);
        return VBOK;
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
