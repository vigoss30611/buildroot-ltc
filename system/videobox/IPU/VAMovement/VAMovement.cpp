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

#include <System.h>
#include "VAMovement.h"

DYNAMIC_IPU(IPU_VAMovement, "vamovement");

void IPU_VAMovement::WorkLoop()
{
    Port *p = GetPort("in");
    Buffer buf;
    histgram_t *tmp;
    histgram_t *h;
    int i, j, tiles, _c0, _c1;

    while(IsInState(ST::Running)) {
        try {
            if(m_u32Width != p->GetWidth()) {
                m_u32Width = p->GetWidth();
                m_u32Height = p->GetHeight();
                if (m_u32Width * m_u32Height < 640 * 480) {
                    m_u32ResoIndex = 3;
                } else if (m_u32Width * m_u32Height < 720 * 1280) {
                    m_u32ResoIndex = 2;
                } else if (m_u32Width * m_u32Height < 1920 * 1080) {
                    m_u32ResoIndex = 1;
                } else {
                    m_u32ResoIndex = 0;
                }
            }
            buf = p->GetBuffer(&buf);

            if(VATargetFps && (1000/VATargetFps > (buf.fr_buf.timestamp - VALastTimeStamp))) {
                if(VALastTimeStamp == 0)
                    VALastTimeStamp = buf.fr_buf.timestamp;
                p->PutBuffer(&buf);
                continue;
            }

            VALastTimeStamp = buf.fr_buf.timestamp;

            if((_c0 = buf.fr_buf.size / sizeof(uint32_t)) !=
            (_c1 = HIS_REGION_BINS * HIS_REGION_HTILES * HIS_REGION_VTILES))
                LOGE("Warning: Histogram size don't match imagination (%d <-> %d)\n",
                        _c0, _c1);

            h = (histgram_t *)buf.fr_buf.virt_addr;
            memcpy(HCurrent, h, sizeof(histgram_t));
            for(i = 0, tiles = 0; i < HIS_REGION_VTILES; i++) {
                for(j = 0; j < HIS_REGION_HTILES; j++) {
                    TCC(i, j) = CalculateBC(HLast->d[i][j], HCurrent->d[i][j])
                        ? TCC(i, j) + 1 : 0;
                    if(TCC(i, j) > GET_FT) {
                        tiles++;
                        if(SendBlkInfo) {
                            EV.x = j;
                            EV.y = i;
                            EV.block_count = TCC(i, j);
                            EV.timestamp = buf.fr_buf.timestamp;
                            event_send(EVENT_VAMOVE_BLK, (char *)&EV, sizeof(EV));
                        }
                    }
                }
            }

            if(tiles > GET_TT) {
                LOGD("movement detected %d (%d, %d, %d, %d) ..\n", tiles,
                    GET_FT, GET_TT, GET_DIFF(m_u32ResoIndex), GET_NUM);
                EV.block_count = tiles;
                EV.timestamp = buf.fr_buf.timestamp;
                event_send(EVENT_VAMOVE, (char *)&EV, sizeof(EV));
            } else EV.block_count = 0;

            p->PutBuffer(&buf);
            tmp = HCurrent;
            HCurrent = HLast;
            HLast = tmp;
        } catch (const char* err) {
            usleep(200000);
        }
    }
}

IPU_VAMovement::IPU_VAMovement(std::string name, Json *js) {
    Port *pin = NULL;
    Name = name;

    pin = CreatePort("in", Port::In);
    Sensitivity = 1;
    VATargetFps = 0;
    VALastTimeStamp = 0;
    SendBlkInfo = 0;
    m_u32ResoIndex = 0; //default for 1920x1088 resolution
    if(NULL != js)
        SendBlkInfo = js->GetInt("send_blk_info");
    pin->SetPixelFormat(Pixel::Format::VAM);
}

void IPU_VAMovement::Prepare()
{
    Port *pin = GetPort("in");
    if (GetPort("in")->GetPixelFormat() != Pixel::Format::VAM)
    {
        LOGE("in Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    HistogramSize = HIS_REGION_BINS * HIS_REGION_HTILES
            * HIS_REGION_VTILES * sizeof(double);
    HCurrent = (histgram_t *)calloc(HistogramSize, 1);
    HLast = (histgram_t *)calloc(HistogramSize, 1);
    m_u32Width = pin->GetWidth();
    if(!m_u32Width)
        m_u32Width = 1920;
}

void IPU_VAMovement::Unprepare()
{
    Port *pIpuPort = NULL;

    pIpuPort = GetPort("in");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    if(HCurrent) free(HCurrent);
    if(HLast) free(HLast);
}

int IPU_VAMovement::UnitControl(std::string func, void *arg)
{
    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(va_move_set_sensitivity) {
        return SetSensitivity(*(int *)arg);
    }

    UCFunction(va_move_get_sensitivity) {
        return GetSensitivity();
    }

    UCFunction(va_move_get_status) {
        int cr = *(int *)arg;
        return GetStatus(cr >> 16, cr & 0xffff);
    }

    UCFunction(va_move_set_sample_fps) {
        return SetSampleFps(*(int *)arg);
    }

    UCFunction(va_move_get_sample_fps) {
        return GetSampleFps();
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}

uint32_t IPU_VAMovement::CalculateBC(uint32_t f0[], uint32_t f1[])
{
    int DiffNum = 0;
    for(int k = 0; k < HIS_REGION_BINS; k++) {
        if(abs(f0[k] - f1[k]) > GET_DIFF(m_u32ResoIndex)) {
            DiffNum++;
        }
    }
    return DiffNum > GET_NUM;
}

int IPU_VAMovement::SensitiveData[3][7] = {
    //ft: move detected if n frame move
    //tt: move detected if N bins in one frame move. It's only for block event
    //num: move detected if N 16 range luminance(0~16, 16~32, 32~48 ...) move
    //diff-1920: only for 1920, move detected if the difference of this frame
    //          and last frame is bigger than this value in one 16 range luminance
    //
    //ft, tt, num, diff-1920 diff-1280 diff-640 diff-320
    {3, 2, 2, 150, 66, 22, 5}, //low sensitive
    {4, 1, 2, 100, 44, 15, 4},
    {4, 1, 3, 50, 22, 7, 2}, //high sensitive
};

int IPU_VAMovement::SetSensitivity(int level)
{
    if(level < 0 || level > 2) {
        LOGE("invalid VMA sensitivity level: %d\n", level);
        return -1;
    }
    return Sensitivity = level;
}

int IPU_VAMovement::GetSensitivity()
{
    return Sensitivity;
}

int IPU_VAMovement::GetStatus(int i, int j)
{
    return EV.block_count;
}

int IPU_VAMovement::SetSampleFps(int target_fps)
{
    if(target_fps < 0 || target_fps > 60) {
        LOGE("invalid VMA sample fps: %d\n", target_fps);
        return -1;
    }
    return VATargetFps = target_fps;
}

int IPU_VAMovement::GetSampleFps()
{
    return VATargetFps;
}
