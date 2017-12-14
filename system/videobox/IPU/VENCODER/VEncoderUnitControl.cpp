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
 * Description: UnitControl
 *
 * Author:
 *     beca.zhang <beca.zhang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  09/12/2017 init
 */
#include <System.h>
#include "VEncoder.h"

int IPU_VENCODER::UnitControl(std::string func, void *arg)
{

    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(video_get_channels)
    {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(video_enable_channel)
    {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(video_disable_channel)
    {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(video_get_resolution)
    {
        Port *p = GetPort("stream");
        vbres_t *res = (vbres_t *)arg;
        res->w = m_stVencPpCfg.u32OffsetW != 0 ? m_stVencPpCfg.u32OffsetW : p->GetWidth();
        res->h = m_stVencPpCfg.u32OffsetH!= 0 ? m_stVencPpCfg.u32OffsetH : p->GetHeight();
        return 0;
    }

    UCFunction(video_get_fps)
    {
        Port *p = GetPort("stream");
        *(int *)arg = p->GetFPS();
        return 0;
    }

    UCFunction(video_get_basicinfo)
    {
        return GetBasicInfo((ST_VIDEO_BASIC_INFO *)arg);
    }

    UCFunction(video_set_basicinfo)
    {
        return SetBasicInfo((ST_VIDEO_BASIC_INFO *)arg);
    }

    UCFunction(video_get_rtbps)
    {
        return GetRTBps();
    }

    UCFunction(video_get_bitrate)
    {
        return GetBitrate((ST_VIDEO_BITRATE_INFO *)arg);
    }

    UCFunction(video_set_bitrate)
    {
        return SetBitrate((ST_VIDEO_BITRATE_INFO *)arg);
    }

    UCFunction(video_get_ratectrl)
    {
        return GetRateCtrl((ST_VIDEO_RATE_CTRL_INFO *)arg);
    }

    UCFunction(video_set_ratectrl)
    {
        ST_VIDEO_RATE_CTRL_INFO * pstP = (ST_VIDEO_RATE_CTRL_INFO *) arg;
        if (m_s32Smartrc)
        {
            Port *pout = GetPort("stream");
            sprintf(pstP->name, "%s%s%s", Name.c_str(), "-",
                    pout->GetName().c_str() );
            event_send(EVENT_SMARTRC_CTRL, (char *)arg,
                    sizeof(ST_VIDEO_RATE_CTRL_INFO));
            return 0;
        }
        else
            return SetRateCtrl((ST_VIDEO_RATE_CTRL_INFO *)arg);
    }

    UCFunction(video_set_ratectrl_ex)
    {
        return SetRateCtrl((ST_VIDEO_RATE_CTRL_INFO *)arg);
    }

    UCFunction(video_trigger_key_frame)
    {
        return TriggerKeyFrame();
    }

    UCFunction(video_get_roi_count)
    {
        return 2;
    }

    UCFunction(video_get_roi)
    {
        return GetROI((ST_VIDEO_ROI_INFO *)arg);
    }

    UCFunction(video_set_roi)
    {
        return SetROI((ST_VIDEO_ROI_INFO *)arg);
    }

    UCFunction(video_get_header)
    {
        int s32TimeOut = 100; //1s
        while(!m_bIsHeaderReady)
        {
            usleep(10000);
            if(!s32TimeOut--)
            {
                LOGE("Error, get header time out!\n");
                return VBEFAILED;
            }
        }
        if(m_s32HeaderLength <= VIDEO_HEADER_MAXLEN)
        {
            memcpy(arg, m_ps8Header, m_s32HeaderLength);
            LOGD("copy %d bytes header to RPC call\n", m_s32HeaderLength);
            return m_s32HeaderLength;
        }
        return VBENOFUNC;
    }

    UCFunction(video_get_tail)
    {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(video_set_scenario)
    {
        return SetScenario(*((EN_VIDEO_SCENARIO *)(arg) ));
    }

    UCFunction(video_get_scenario)
    {
        return  GetScenario();
    }

    UCFunction(video_set_frame_mode)
    {
        if((*(v_enc_frame_type*)arg != VENC_HEADER_NO_IN_FRAME_MODE)
                && (*(v_enc_frame_type*)arg != VENC_HEADER_IN_FRAME_MODE))
            return VBEBADPARAM;
        return SetFrameMode(*(EN_VIDEO_ENCODE_FRAME_TYPE *)arg);
    }

    UCFunction(video_get_frame_mode)
    {
        return GetFrameMode();
    }

    UCFunction(video_set_frc)
    {
        return SetFrc((ST_VIDEO_FRC_INFO *)arg);
    }

    UCFunction(video_get_frc)
    {
        return GetFrc((ST_VIDEO_FRC_INFO *)arg);
    }

    UCFunction(video_set_sei_user_data)
    {
        return SetSeiUserData((ST_VIDEO_SEI_USER_DATA_INFO *)arg);
    }

    UCFunction(video_set_pip_info)
    {
        SetPipInfo((ST_VIDEO_PIP_INFO *)arg);
        return 0;
    }

    UCFunction(video_get_pip_info)
    {
        GetPipInfo((ST_VIDEO_PIP_INFO *)arg);
        return 0;
    }

    UCFunction(video_set_slice_height)
    {
        return SetSliceHeight(*(int *)arg);
    }

    UCFunction(video_get_slice_height)
    {
        return GetSliceHeight((int *)arg);
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
