// Copyright (C) 2016 InfoTM, beca.zhang@infotm.com
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
#include <math.h>
#include <sys/time.h>
#include "MVMovement.h"

#define CONTINUE_FCNT 5
//#define SEND_EVERY_MVMB
#define BASE_CMV 5
#define SAMPLE_MODE 0

DYNAMIC_IPU(IPU_MVMovement, "mvmovement");

void IPU_MVMovement::WorkLoop()
{
    Port *p = GetPort("in");
    Buffer buf;
    uint32_t skipFlag = CONTINUE_FCNT;
    bool IsSkip = false;
    uint32_t i, j;
    int dlmv, damv;
    int mvx, mvy;
    uint32_t px, py;
    int mvMbNum[4] = {0};
    encOutputMbInfo_s *mbInfo;
    struct roiInfo_t eroi;
    uint32_t u32RelMbNum;

    uint32_t u32FirstNum = 0;
    int s32Count = 0;
#if SAMPLE_MODE
    int s32OffsetCL, s32OffsetCR, s32OffsetCU, s32OffsetCD, s32OffsetC;
    char s8SelectCount = 1;

    /* select point in one portion('-' means one mb)
       -  -  -  -
       -  -  U  -
       -  L  C  R
       -  -  D  -
    */
    if (m_enMbMode == E_MB64)
    {
        s32OffsetCL = m_u32ProPortion/4 + m_u32ProPortion/2*mbPerRow;
        s32OffsetCR = m_u32ProPortion*3/4 + m_u32ProPortion/2*mbPerRow;
        s32OffsetCU = m_u32ProPortion/2 + m_u32ProPortion/4*mbPerRow;
        s32OffsetCD = m_u32ProPortion/2 + m_u32ProPortion*3/4*mbPerRow;
        s32OffsetC = m_u32ProPortion/2 + m_u32ProPortion/2*mbPerRow;
        s8SelectCount = 5;
    }
#endif

    while(IsInState(ST::Running)) {
        try {
            buf = p->GetBuffer(&buf);
            if(!(IsSkip = ((MVTargetFps >= CONTINUE_FCNT) &&
                (m_u32TargetPerFrameTime > (buf.fr_buf.timestamp - MVLastTimeStamp))))) {
                skipFlag = CONTINUE_FCNT;
                MVLastTimeStamp = buf.fr_buf.timestamp;
            }

            if(IsSkip && !(skipFlag ? --skipFlag : skipFlag)) {
                p->PutBuffer(&buf);
                continue;
            }

            mbInfo = (encOutputMbInfo_s *)buf.fr_buf.virt_addr;
            for(i = 0; i < m_u32BigMbPerFrame - 1; i++, CurInfo++) {
                u32RelMbNum = i / m_u32BigMbPerRow * mbPerRow * m_u32ProPortion
                    + ((i % m_u32BigMbPerRow == 0) ? 0 : ((i % m_u32BigMbPerRow - 1) * m_u32ProPortion + 1));
                u32FirstNum = u32RelMbNum;
                s32Count = 0;
#if SAMPLE_MODE
                for(j=0;j < s8SelectCount;j++)//select adjustment point
                {
                    if (m_enMbMode == E_MB64)
                    {
                        switch(j)
                        {
                            case 0://center left
                                u32RelMbNum = u32FirstNum + s32OffsetCL;
                                break;
                            case 1://center point
                                u32RelMbNum = u32FirstNum + s32OffsetC;
                                break;
                            case 2://center right
                                u32RelMbNum = u32FirstNum + s32OffsetCR;
                                break;
                            case 4://center up
                                u32RelMbNum = u32FirstNum + s32OffsetCU;
                                break;
                            case 5://center down
                                u32RelMbNum = u32FirstNum + s32OffsetCD;
                                break;
                        }
                    }
#endif
                    mvx = (mbInfo + u32RelMbNum)->mvX[0];
                    mvy = (mbInfo + u32RelMbNum)->mvY[0];
                    CurInfo->lmv = abs(mvx) + abs(mvy);

                    CurInfo->amv = GetQuadrant(mvx, mvy);
                    CurInfo->mbNumRef = GetRefNum(i, mvx, mvy);
                    dlmv = CurInfo->lmv - (PreInfo + CurInfo->mbNumRef)->lmv;
                    damv = CurInfo->amv - (PreInfo + CurInfo->mbNumRef)->amv;

                    switch (((mbInfo + i)->mode) & 0x0f) {
                        case 0: //I16x16_V
                        case 1: //I16x16_H
                        case 2: //I16x16_DC
                        case 3: //I16x16_PLANE
                        case 4: //I4x4
                            CurInfo->cmv = BASE_CMV;
                            break;
                        case 6: //P16x16
                        case 7: //P16x8
                        case 8: //P8x16
                        case 9: //P8x8
                            if((CurInfo->lmv > Tlmv) && (abs(dlmv) < (int)Tdlmv) && (abs(damv) == 7 || abs(damv) < 2)) {
                                CurInfo->cmv = BASE_CMV + (((PreInfo + CurInfo->mbNumRef)->cmv) >> 1);
                            } else {
                                CurInfo->cmv = CurInfo->lmv ? ((PreInfo + CurInfo->mbNumRef)->cmv) >> 1 : 0;
                            }
                            if(CurInfo->cmv > Tcmv) {
                                s32Count++;
                            }
                            break;
                        default:
                            break;
                    }
#if SAMPLE_MODE
                    if (s32Count >= 2) {
#else
                    if (s32Count >= 1) {
#endif
                        u32RelMbNum = u32FirstNum;
                        EventData.mx = u32RelMbNum % mbPerRow;
                        EventData.my = u32RelMbNum / mbPerRow;

                        MutexSettingLock->lock();
                        EventData.blk_x = EventData.mx * blkInfo.blk_x / mbPerRow;
                        EventData.blk_y = EventData.my * blkInfo.blk_y / mbPerCol;
                        if(EventData.blk_x > blkInfo.blk_x - 1) //if not exact division case
                            EventData.blk_x--;
                        if(EventData.blk_y > blkInfo.blk_y - 1)
                            EventData.blk_y--;
                        blkMvCnt[EventData.blk_y * blkInfo.blk_x + EventData.blk_x]++;
                        MutexSettingLock->unlock();

#ifdef SEND_EVERY_MVMB
                        EventData.mbNum = u32RelMbNum;
                        EventData.blk_x = EventData.blk_y = 0;
                        EventData.timestamp = buf.fr_buf.timestamp;
                        event_send(EVENT_MVMOVE, (char *)&EventData, sizeof(struct evd));
#endif
                        px = EventData.mx << 4;
                        py = EventData.my << 4;
                        for(j = 0; j < 4; j++) {
                            if(px > roi[j].x && px < (roi[j].x + roi[j].w)
                                    && py > roi[j].y && py < (roi[j].y + roi[j].h))
                                mvMbNum[j]++;
                        }
#if SAMPLE_MODE
                        break;
#endif
                    }
#if SAMPLE_MODE
                }
#endif
            }
            for(j = 0; j < 4; j++) {
                roi[j].mv_mb_num = mvMbNum[j];
                if(roi[j].mv_mb_num > m_u32MbNumThreshold) {
                    eroi.roi_num = j;
                    eroi.mv_roi = roi[j];
                    event_send(EVENT_MVMOVE_ROI, (char *)&eroi, sizeof(struct roiInfo_t));
                }
                mvMbNum[j] = 0;
            }
            MutexSettingLock->lock();
            for(j = 0; j < blkInfo.blk_x * blkInfo.blk_y; j++) {
                if(blkMvCnt[j] > m_u32MbNumThreshold) {
                    EventData.mx = j % blkInfo.blk_x;
                    EventData.my = j / blkInfo.blk_x;
                    EventData.blk_x = blkInfo.blk_x;
                    EventData.blk_y = blkInfo.blk_y;
                    EventData.mbNum = j;
                    EventData.timestamp = buf.fr_buf.timestamp;
                    event_send(EVENT_MVMOVE, (char *)&EventData, sizeof(struct evd));
                }
            }
            memset(blkMvCnt, 0, blkInfo.blk_x * blkInfo.blk_y * 4);
            MutexSettingLock->unlock();
            CurInfo -= m_u32BigMbPerFrame - 1;
            memcpy(PreInfo, CurInfo, m_u32BigMbPerFrame * sizeof(struct MBInfo));
            p->PutBuffer(&buf);
        } catch (const char* err) {
            LOGE("Get ref failed, sleep 200ms\n");
            usleep(200000);
        }
    }

}

IPU_MVMovement::IPU_MVMovement(std::string name, Json *js) {
    Port *p = NULL;
    Name = name;
    p = CreatePort("in", Port::In);
    p->SetPixelFormat(Pixel::Format::MV);
    Sensitivity = 4; //[0,5]
    MVTargetFps = 5;
    m_u32TargetPerFrameTime = 1000 / (MVTargetFps / CONTINUE_FCNT);
    MVLastTimeStamp = 0;
    frame_cnt = 1;
    CurInfo = PreInfo = NULL;
    MutexSettingLock = NULL;
}

void IPU_MVMovement::Prepare()
{
    Port *p = GetPort("in");
    uint32_t u32RawWidth, u32RawHeight;

    if (p->GetPixelFormat() != Pixel::Format::MV)
    {
        LOGE("in Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

    u32RawWidth = p->GetWidth();
    u32RawHeight = p->GetHeight();
    mbPerRow = (u32RawWidth + 15) / 16;
    mbPerCol = (u32RawHeight + 15 ) / 16;

    if (u32RawWidth * u32RawHeight < 720 * 1280) {
        m_u32MbNumThreshold = 4; //the threshold MB number in one grid block
        m_u32ProPortion = 1;
        m_enMbMode = E_MB16;
    } else if (u32RawWidth * u32RawHeight < 1920 * 1080) {
        m_u32MbNumThreshold = 1;
        m_u32ProPortion = 2;
        m_enMbMode = E_MB32;
    } else {
        m_u32MbNumThreshold = 1;
        m_u32ProPortion = 4;
        m_enMbMode = E_MB64;
    }

    mbPerFrame = mbPerRow * mbPerCol;
    m_u32BigMbPerFrame = mbPerFrame / m_u32ProPortion / m_u32ProPortion;
    m_u32BigMbPerRow = mbPerRow / m_u32ProPortion;
    m_u32BigMbPerCol = mbPerCol / m_u32ProPortion;

    Tdlmv = SensitiveData[Sensitivity][0];
    Tlmv = SensitiveData[Sensitivity][1];
    Tcmv = SensitiveData[Sensitivity][2];
    memset(&roi, 0, sizeof(struct mvRoi_t) * 4);
    blkInfo.blk_x = 7;
    blkInfo.blk_y = 7;
    blkMvCnt = NULL;
    blkMvCnt = (uint32_t *)calloc(blkInfo.blk_x * blkInfo.blk_y, sizeof(uint32_t));
    if(blkMvCnt == NULL) {
        LOGE("blkMvCnt alloc mem failed\n");
    }
    CurInfo = (struct MBInfo*)calloc(mbPerFrame / m_u32ProPortion / m_u32ProPortion, sizeof(struct MBInfo));
    if(CurInfo == NULL) {
        LOGE("CurInfo alloc mem failed\n");
    }
    PreInfo = (struct MBInfo*)calloc(mbPerFrame / m_u32ProPortion / m_u32ProPortion, sizeof(struct MBInfo));
    if(PreInfo == NULL) {
        LOGE("PreInfo alloc mem failed\n");
    }
    if (MutexSettingLock ==  NULL ){
        MutexSettingLock =  new std::mutex();
    }
}

void IPU_MVMovement::Unprepare()
{
    Port *pIpuPort = NULL;

    pIpuPort = GetPort("in");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }

    if(CurInfo)
        free(CurInfo);
    if(PreInfo)
        free(PreInfo);
}

uint32_t IPU_MVMovement::GetRefNum(uint32_t curNum, int mvx, int mvy)
{
    uint32_t refNum = curNum;
    uint32_t incx, incy;

    incx = incy = 0;
    // |---*-->|-+1*-+1|-+2*-+2|
    // |-32*-32|-32*-32|-32*-32|
    // incx incy's value means N of 32
    if(abs(mvx) >= 32)
        incx = abs((mvx > 0 ? (int)ceil(mvx / (32.0)) >> 1 : (int)floor(mvx / (32.0)) >> 1));
    if(abs(mvy) >= 32)
        incy = abs((mvy > 0 ? (int)ceil(mvy / (32.0)) >> 1 : (int)floor(mvy / (32.0)) >> 1));

    switch (m_enMbMode) {
        case E_MB64:
            //refNum here means the number of 4x4 MB
            refNum += incx > 2 ? incx >> 1 : 0;
            refNum += (incy > 2 ? incy >> 1 : 0) * m_u32BigMbPerRow;
            break;
        case E_MB32:
            //refNum here means the number of 2x2 MB
            refNum += incx;
            refNum += incy * m_u32BigMbPerRow;
            break;
        case E_MB16:
            //refNum here means the number of MB
            refNum += incx;
            refNum += incy * mbPerRow;
            break;
        default:
            LOGE("Wrong MB mode\n");
            break;
    }

    //rm boundary noise
    while(refNum > (m_u32BigMbPerFrame - 1))
        refNum -= m_u32BigMbPerRow;
    while(refNum < 0)
        refNum += m_u32BigMbPerRow;

    return refNum;
}

int IPU_MVMovement::GetQuadrant(int mvx, int mvy)
{
    if(!mvx && !mvy)
        return 0;
    if(abs(mvx) > abs(mvy)) {
        if(mvx > 0)
            return mvy > 0 ? 1 : 8;
        else
            return mvy > 0 ? 4 : 5;
    } else {
        if(mvx > 0)
            return mvy > 0 ? 2 : 7;
        else
            return mvy > 0 ? 3 : 6;
    }
}

int IPU_MVMovement::UnitControl(std::string func, void *arg)
{
    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(va_mv_set_blkinfo) {
        return SetBlkInfo(*(struct blkInfo_t *)arg);
    }

    UCFunction(va_mv_get_blkinfo) {
        return GetBlkInfo((struct blkInfo_t *)arg);
    }

    UCFunction(va_mv_set_roi) {
        return SetRoi(*(struct roiInfo_t *)arg);
    }

    UCFunction(va_mv_get_roi) {
        struct roiInfo_t roiArg = *(struct roiInfo_t *)arg;
        return GetRoi(roiArg.roi_num, &((struct roiInfo_t *)arg)->mv_roi);
    }

    UCFunction(va_mv_set_sensitivity) {
        return SetSensitivity(*(struct MBThreshold *)arg);
    }

    UCFunction(va_mv_get_sensitivity) {
        return GetSensitivity((struct MBThreshold *)arg);
    }

    UCFunction(va_mv_set_senlevel) {
        return SetSensitivity(*(int *)arg);
    }

    UCFunction(va_mv_get_senlevel) {
        return GetSensitivity();
    }

    UCFunction(va_mv_set_sample_fps) {
        return SetSampleFps(*(int *)arg);
    }

    UCFunction(va_mv_get_sample_fps) {
        return GetSampleFps();
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}

int IPU_MVMovement::SetBlkInfo(struct blkInfo_t blk)
{
    if(blk.blk_x <= 0 || blk.blk_x > mbPerRow
            || blk.blk_y <= 0 || blk.blk_y > mbPerCol) {
        LOGE("invalid blk info parameter\n");
        return -1;
    }
    MutexSettingLock->lock();
    blkInfo = blk;
    if(blkMvCnt)
        free(blkMvCnt);
    blkMvCnt = (uint32_t *)calloc(blkInfo.blk_x * blkInfo.blk_y, sizeof(uint32_t));
    if(blkMvCnt == NULL) {
        LOGE("blkMvCnt alloc mem failed\n");
    }
    MutexSettingLock->unlock();
    return 0;
}

int IPU_MVMovement::GetBlkInfo(struct blkInfo_t *blk)
{
    if(!blk) {
        LOGE("pblk is NULL\n");
        return -1;
    }
    *blk = blkInfo;
    return 0;
}

int IPU_MVMovement::SetRoi(struct roiInfo_t roiInfo)
{
    if(roiInfo.roi_num < 0 || roiInfo.roi_num > 3
            || roiInfo.mv_roi.x / 16 > mbPerRow
            || roiInfo.mv_roi.y / 16 > mbPerCol
            || !roiInfo.mv_roi.w || (roiInfo.mv_roi.x + roiInfo.mv_roi.w) / 16 > mbPerRow
            || !roiInfo.mv_roi.h || (roiInfo.mv_roi.y + roiInfo.mv_roi.h) / 16 > mbPerCol) {
        LOGE("invalid mv roi info parameter\n");
        return -1;
    }
    roi[roiInfo.roi_num] = roiInfo.mv_roi;
    return 0;
}

int IPU_MVMovement::GetRoi(int roiNum, struct mvRoi_t *mvRoi)
{
    if(!mvRoi || roiNum < 0 || roiNum > 3) {
        LOGE("invalid roiNum\n");
        return -1;
    }
    *mvRoi = roi[roiNum];
    return 0;
}

uint32_t IPU_MVMovement::SensitiveData[6][3] = {
    //tdlmv, tlmv, tcmv
    {16, 7, BASE_CMV + 3}, //dark-low
    {20, 6, BASE_CMV + 2}, //dark-medium
    {32, 5, BASE_CMV + 1}, //dark-high
    {32, 4, BASE_CMV + 3}, //light-low
    {32, 3, BASE_CMV + 2}, //light-medium
    {32, 2, BASE_CMV + 1}, //light-high
};

int IPU_MVMovement::SetSensitivity(int level)
{
    if(level < 0 || level > 5) {
        LOGE("invalid VMA sensitivity level: %d\n", level);
        return -1;
    }
    Tdlmv = SensitiveData[level][0];
    Tlmv = SensitiveData[level][1];
    Tcmv = SensitiveData[level][2];
    return Sensitivity = level;
}

int IPU_MVMovement::GetSensitivity()
{
    return Sensitivity;
}

int IPU_MVMovement::SetSensitivity(struct MBThreshold st)
{
    Tlmv = st.tlmv;
    Tdlmv = st.tdlmv;
    Tcmv = st.tcmv;
    return 1;
}

int IPU_MVMovement::GetSensitivity(struct MBThreshold *st)
{
    if(!st) {
        LOGE("Get Sensitivity st is NULL\n");
        return -1;
    }
    st->tlmv = Tlmv;
    st->tdlmv = Tdlmv;
    st->tcmv = Tcmv;
    return 0;
}

int IPU_MVMovement::SetSampleFps(int target_fps)
{
    if(target_fps <= 0 || target_fps > 60) {
        LOGE("invalid VMA sample fps: %d\n", target_fps);
        return -1;
    }
    MVTargetFps = target_fps;
    m_u32TargetPerFrameTime = 1000/(MVTargetFps/CONTINUE_FCNT);
    return MVTargetFps;
}

int IPU_MVMovement::GetSampleFps()
{
    return MVTargetFps;
}

