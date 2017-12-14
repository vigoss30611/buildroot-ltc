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
#include "Jenc.h"

int IPU_JENC::UnitControl(std::string func, void *arg) {

    LOGD("%s: %s\n", __func__, func.c_str());

    int s32Ret = 0;

    UCSet(func);

    UCFunction(video_get_snap) {
        TriggerSnap();
        return enJencRet;
    }


    UCFunction(video_get_thumbnail) {
    #ifdef COMPILE_OPT_JPEG_SWSCALE
        s32Ret = GetThumbnail();
    #else
        LOGE("error:thumbnail mode need jenc open scale macro!\n");
        s32Ret = VBEFAILED;
    #endif

        return s32Ret;
    }

    UCFunction(thumbnail_set_resolution) {
        pthread_mutex_lock(&m_stUpdateMut);
        #ifdef COMPILE_OPT_JPEG_SWSCALE
        memcpy(&m_stThumbResolution, (vbres_t *) arg, sizeof(vbres_t));
        s32Ret = VBOK;
        #else
        LOGE("error:thumbnail mode need Jenc open scale macro!\n");
        s32Ret = VBEFAILED;
        #endif
        pthread_mutex_unlock(&m_stUpdateMut);
        return s32Ret;
    }

    UCFunction(video_set_snap_quality) {
        return SetQuality(*(int *)arg);
    }

    UCFunction(video_set_snap) {
        return SetSnapParams((struct v_pip_info *) arg);
    }

    UCFunction(video_set_panoramic) {
        pthread_mutex_lock(&m_stUpdateMut);
        if(*((int *)arg) == 1)
        {
            m_bApp1Enable = true;
        }
        else
        {
            m_bApp1Enable = false;
        }
        pthread_mutex_unlock(&m_stUpdateMut);
        return VBOK;
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
