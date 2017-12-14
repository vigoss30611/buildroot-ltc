// Copyright (C) 2016 InfoTM, bright.jiang@infotm.com
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
#include "H2.h"

int IPU_H2::UnitControl(std::string func, void *arg) {

    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(video_get_channels) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(video_enable_channel) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(video_disable_channel) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(video_get_resolution) {
        Port *p = GetPort("stream");
        vbres_t *res = (vbres_t *)arg;
        res->w = VencPpCfg.offsetW != 0 ? VencPpCfg.offsetW : p->GetWidth();
        res->h = VencPpCfg.offsetH != 0 ? VencPpCfg.offsetH : p->GetHeight();
        return 0;
    }

    UCFunction(video_get_fps) {
        Port *p = GetPort("stream");
        *(int *)arg = p->GetFPS();
        return 0;
    }

    UCFunction(video_get_basicinfo) {
        return GetBasicInfo((struct v_basic_info *)arg);
    }

    UCFunction(video_set_basicinfo) {
        return SetBasicInfo((struct v_basic_info *)arg);
    }

    UCFunction(video_get_rtbps) {
        return GetRTBps();
    }


    UCFunction(video_get_bitrate) {
        return GetBitrate((struct v_bitrate_info *)arg);
    }

    UCFunction(video_set_bitrate) {
        return SetBitrate((struct v_bitrate_info *)arg);
    }

    UCFunction(video_get_ratectrl) {
        return GetRateCtrl((struct v_rate_ctrl_info *)arg);
    }

    UCFunction(video_set_ratectrl) {
        struct v_rate_ctrl_info * p = (struct v_rate_ctrl_info *) arg;
        if ( this->smartrc )
        {
            Port *pout = GetPort("stream");
            sprintf(p->name, "%s%s%s", this->Name.c_str(), "-", pout->GetName().c_str() );
            event_send(EVENT_SMARTRC_CTRL, (char *)arg, sizeof(struct v_rate_ctrl_info));
            return 0;
        }
        else
        {
            return SetRateCtrl((struct v_rate_ctrl_info *)arg);
        }
    }

    UCFunction(video_set_ratectrl_ex) {
        return SetRateCtrl((struct v_rate_ctrl_info *)arg);
    }

    UCFunction(video_trigger_key_frame) {
        return TriggerKeyFrame();
    }

    UCFunction(video_get_roi_count) {
        return 2;
    }

    UCFunction(video_get_roi) {
        return GetROI((struct v_roi_info *)arg);
    }

    UCFunction(video_set_roi) {
        return SetROI((struct v_roi_info *)arg);
    }

    UCFunction(video_get_header) {
        int timeout = 100; //1s
        while(!IsHeaderReady) {
            usleep(10000);
            if(!timeout--) {
                LOGE("Error, get header time out!\n");
                return VBEFAILED;
            }
        }
        if(HeaderLength <= VIDEO_HEADER_MAXLEN) {
            memcpy(arg, Header, HeaderLength);
            LOGD("copy %d bytes header to RPC call\n", HeaderLength);
            return HeaderLength;
        }
        return VBENOFUNC;
    }

    UCFunction(video_get_tail) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(video_set_scenario) {
        return SetScenario(*( (enum video_scenario *)(arg) ));
    }

    UCFunction(video_get_scenario) {
        return  GetScenario();
    }

    UCFunction(video_set_frame_mode) {
        if((*(v_enc_frame_type*)arg != VENC_HEADER_NO_IN_FRAME_MODE) && (*(v_enc_frame_type*)arg != VENC_HEADER_IN_FRAME_MODE))
            return VBEBADPARAM;
        return SetFrameMode(*(v_enc_frame_type*)arg);
    }

    UCFunction(video_get_frame_mode) {
        return GetFrameMode();
    }

    UCFunction(video_set_frc) {
        return SetFrc((struct v_frc_info *)arg);
    }

    UCFunction(video_get_frc) {
        return GetFrc((struct v_frc_info *)arg);
    }
    UCFunction(video_set_pip_info)
    {
        SetPipInfo((struct v_pip_info *)arg);
        return 0;
    }
    UCFunction(video_get_pip_info)
    {
        GetPipInfo((struct v_pip_info *)arg);
        return 0;
    }

    UCFunction(video_set_sei_user_data)
    {
        return SetSeiUserData((ST_VIDEO_SEI_USER_DATA_INFO *)(arg));
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
