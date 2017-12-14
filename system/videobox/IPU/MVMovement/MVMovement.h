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

#ifndef IPU_MVMovement_H
#define IPU_MVMovement_H
#include <IPU.h>
#include <enccommon.h>

enum morphMethod {
    EROSION = 0,
    DILATION,
};

typedef enum
{
    E_MB64 = 0,
    E_MB32,
    E_MB16,
} E_MB_MODE;

class IPU_MVMovement: public IPU {

private:
    uint64_t frame_cnt;
    uint32_t mbPerRow;
    uint32_t mbPerCol;
    uint32_t mbPerFrame;
    uint32_t Tdlmv, Tlmv, Tcmv;
    static uint32_t SensitiveData[6][3];
    struct mvRoi_t roi[4];
    struct blkInfo_t blkInfo;
    uint32_t *blkMvCnt;
    std::mutex  *MutexSettingLock;
    uint32_t GetRefNum(uint32_t curNum, int mvx, int mvy);
    int GetQuadrant(int mvx, int mvy);
    E_MB_MODE m_enMbMode;
    uint32_t m_u32ProPortion;
    uint32_t m_u32BigMbPerFrame;
    uint32_t m_u32BigMbPerRow;
    uint32_t m_u32BigMbPerCol;
    uint32_t m_u32MbNumThreshold;

public:
    uint32_t mx, my;
    int Sensitivity;
    uint32_t MVTargetFps;
    uint32_t m_u32TargetPerFrameTime;
    uint64_t MVLastTimeStamp;
    struct MBInfo *CurInfo, *PreInfo;
    struct evd EventData;

    IPU_MVMovement(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);
    int SetBlkInfo(struct blkInfo_t blk);
    int GetBlkInfo(struct blkInfo_t *blk);
    int SetRoi(struct roiInfo_t roiInfo);
    int GetRoi(int roiNum, struct mvRoi_t *mvRoi);
    int SetSensitivity(int level);
    int GetSensitivity();
    int SetSensitivity(struct MBThreshold st);
    int GetSensitivity(struct MBThreshold *st);
    int SetSampleFps(int target_fps);
    int GetSampleFps();
};
#endif // IPU_MVMovement_H
